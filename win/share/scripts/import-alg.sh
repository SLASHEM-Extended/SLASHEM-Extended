#!/bin/sh
#
#   Copyright (c) J. Ali Harlow 2000
#   NetHack may be freely redistributed.  See license for details.
#
#   This shell script imports Kelly Youngblood's tilesets from his
#   AllegroHack package. You must have a copy of the binary zip
#   file. anh_161b.zip is available from
#     http://www.pinn.net/~jry/allegrohack/
#   Set the variable below set to point at it (there's no need to
#   unpack the archive first).
#
#   Configuration variables
#
input=anh_161b.zip
# Backup input for missing bitmaps (eg., lords.bmp, missing from version 1.61)
input2=anh_15b.zip
xpm2txt=../../../util/xpm2txt
txtfilt=../../../util/txtfilt
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
    echo "s£^# tile ${ad_count} (unknown)£# tile ${id} ($ad_name)£" >> ${ad_sed}
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
	winter wolf cub
	Cerberus
	gas spore
	flaming sphere
	shocking sphere
	master mind flayer
	glass piercer
	mastodon
	trapper
	pony
	horse
	warhorse
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
	barrow wight
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
	silver saber
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
	teleportation trap
	level teleporter
	grave
	EOF
    toplevel=ahack-dist
    toplevel_backup=ahack-dist-backup
    output=ahack
    /bin/rm -rf $toplevel $toplevel_backup
    unzip -L -q -d $toplevel ${input}
    unzip -L -q -d $toplevel_backup ${input2}
    rm -f ${output}-monsters.tile ${output}-objects.tile ${output}-other.tile
    rm -rf $output
    mkdir $output
    id=1
}
#
# Usage: get_tile <input-dir> <tile> [<set>]
get_tile()
{
    tile_dir=$1
    tile_pat=$2
    if [ $# -gt 2 ]; then
	set_filter=": $3[/\\]"
    else
	set_filter='.*'
    fi
    if test "$tile_pat" = '*'; then
	egrep_flags="-v"
	egrep_args="^#|^[ 	]*$"
    else
	egrep_flags="--"
	egrep_args="^[^ 	]+[ 	]+\(${tile_pat}\)"
    fi
    tr -d '[\015]' < $tile_dir/tiles/index | sed -e 's:\\:/:g' | \
      egrep "${egrep_flags}" "${egrep_args}" | egrep -- "${set_filter}" | \
      while read "gt_line"; do
	gt_name=`echo $gt_line | sed -n 's/.*(\([^)]*\)).*/\1/p'`
	gt_bmp=`echo $gt_line | sed -n 's/.*:[ 	]*\([^ 	]*\).*/\1/p'`
	gt_offset=`echo $gt_line | sed -n 's/.*\@[ 	]*\([^ 	]*\).*/\1/p'`
	echo "$gt_bmp" "${gt_offset:-0}" "$gt_name"
    done
}
#
# Usage: proc_tile <bmpfile> <offset> <name>
proc_tile()
{
    pt_bmp=$1
    pt_offset=$2
    pt_name=$3
    if echo $pt_name | egrep -s '^cmap [0-9]+$' >/dev/null; then
	no=`echo $pt_name | sed 's/cmap *//'`
	nline=`egrep "^${no}	" cmap.names`
	if test -n "$nline"; then
	    pt_name=`echo $nline | sed 's:^[^ 	]*[ 	]:cmap / :'`
	fi
    fi
    case $pt_bmp in
    */mon/*)
	ex_output=mon32alg
	echo ${output}@monsters@${pt_name}@${id} >> ${output}-monsters.tile
	;;
    */obj/*)
	ex_output=obj32alg
	echo ${output}@objects@${pt_name}@${id} >> ${output}-objects.tile
	;;
    */etc/*)
	ex_output=oth32alg
	echo ${output}@other@${pt_name}@${id} >> ${output}-other.tile
	;;
    *)
	echo "Warning: tile not in a known tileset ($pt_bmp): ignored" 1>&2
	return
	;;
    esac
    bmptoppm $pt_bmp > $output/x${id}.ppm 2> /dev/null
    pnmcut 0 "$pt_offset" 32 32 $output/x${id}.ppm > $output/x${id}.pnm
    addtile ${ex_output}.ppm "$id" "$pt_name" $output/x${id}.pnm
}
#
ppm_package()
{
    rm -f mon32alg.ppm obj32alg.ppm oth32alg.ppm
    for pp_set in mon obj etc; do
	get_tile "$toplevel" '*' $pp_set | while read bmp offset "name"; do
	    bmpfile=$toplevel/tiles/$bmp
	    if test \! -r $bmpfile; then
		set +e
		get_tile "$toplevel_backup" "$name" |
		{
		    read sbmp soffset "sname"
		    if test $? -ne 0; then
			echo "Warning: tile bitmap ($bmpfile) not readable: ignored" 1>&2
			exit 1;
		    elif test \! -r $toplevel_backup/tiles/$sbmp; then
			echo "Warning: tile bitmap ($bmpfile) not readable: ignored" 1>&2
			exit 1;
		    else
			echo "Warning: tile bitmap ($bmp) not readable: using backup" 1>&2
			echo "$toplevel_backup/tiles/$sbmp" "$soffset" "$sname"
			exit 0;
		    fi
		}
		set -e
	    else
		echo "$bmpfile" "$offset" "$name"
	    fi
	done
    done | while read bmpfile offset "name"; do
	proc_tile "$bmpfile" "$offset" "$name"
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
    rm -rf -- ${toplevel} ${toplevel_backup} ${output}
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

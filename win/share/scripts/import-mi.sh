#!/bin/sh
#
#   Copyright (c) J. Ali Harlow 2000
#   NetHack may be freely redistributed.  See license for details.
#
#   This shell script imports Mitshurio Itakura's tilesets from
#   his diy33 package. You must have a copy of this package first!
#   diy33.zip is available from
#     http://www.geocities.co.jp/SiliconValley-SanJose/9606/nh/diy33.html
#   It should be unpacked into a suitable direcory and the variable
#   below set to point at it.
#
#   You will also need a copy of the relevant tilemap. For the version of
#   diy33 current at 22 Sep 2000, this is the NetHack 3.3.0 map.
#
#   Note: It would be nice to be able to create these from the name.txt
#   file included in diy33, but this lacks the information as to wether
#   a tile is a monster, object or other. SlashEM's supplemental cmap
#   names (see win/share/tilemap.c) also need special handling.
#
#   There appear to be a few tiles that didn't make it into diy33. These
#   are included as mon32mix.txt and obj32mix.txt (which were imported
#   from t32-1024.xpm). Hopefully the need for these files should go
#   away in future.
#
#   Finally, you need xpm2txt, txtfilt and txtmerge. These can be created
#   by running "make tileutils" in the util directory.
#
#   Output is in the current directory. You must copy it by hand into
#   win/share if so desired.
#
# Configuration variables
#
input=diy33
tilemap=nh330
extra_mon=mon32mix.txt
extra_obj=obj32mix.txt
xpm2txt=../../../util/xpm2txt
txtfilt=../../../util/txtfilt
txtmerge=../../../util/txtmerge
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
    # Some special cases for spellbooks that Slash'em has a different
    # mapping between description and name from NetHack 3.3.0.
    # By removing the name, we allow txtmerge to match them.
    oc_descr=`echo $ad_name | sed 's: /.*::'`
    case "$oc_descr" in
    "dull"|"thin")
	ad_name=$oc_descr;;
    esac
    echo "s£^# tile ${ad_count} (unknown)£# tile ${id} ($ad_name)£" >> ${ad_sed}
}
import()
# Usage: import <tileset> <sub>
# tileset can be either 32 or 3d
{
    tileset=$1
    long=$2
    short=`echo $long | sed 's/^\(...\).*/\1/'`
    ex_output=${short}${tileset}mi
    if test "$tileset" = "32"; then
	key="p"
    elif test "$tileset" = "3d"; then
	key="b"
    else
	echo "Error: import: tileset unknown \"${tileset}\""
    fi
    if test "$short" = "mon"; then
	offset=0
    elif test "$short" = "obj"; then
	offset=`wc -l ${tilemap}-monsters.tile`
    else
	offset=`cat ${tilemap}-monsters.tile ${tilemap}-objects.tile | wc -l`
    fi
    offset=`echo $offset | awk '{ print $1 }'`
    cat ${tilemap}-${long}.tile | while read line; do
	id=`echo $line | sed 's:.*@\([^@]*\)$:\1:'`
	id=`echo 1000 + ${offset} + ${id} | bc | sed 's/^1//'`
	name=`echo $line | sed 's:.*@\([^@]*\)@[^@]*$:\1:'`
	bmp=$input/${key}xxx/${key}$id.bmp
	if test -r $bmp; then
	    bmptoppm $bmp > /tmp/et$$.ppm 2> /dev/null
	    addtile ${ex_output}.ppm "$id" "$name" /tmp/et$$.ppm
	fi
    done
    if test -r ${ex_output}.ppm; then
	ppmtoxpm ${ex_output}.ppm > ${ex_output}.xpm
	if test "$tileset" = "32"; then
	    $xpm2txt -w32 -h32 ${ex_output}.xpm /tmp/et$$.txt
	    sed -f ${ex_output}.sed < /tmp/et$$.txt > /tmp/et$$-2.txt
	    if test "$short" = "mon" -a -n "${extra_mon}"; then
		${txtmerge} /tmp/et$$.txt /tmp/et$$-2.txt ${extra_mon}
		${txtfilt} /tmp/et$$.txt ${ex_output}.txt
	    elif test "$short" = "obj" -a -n "${extra_obj}"; then
		${txtmerge} /tmp/et$$.txt /tmp/et$$-2.txt ${extra_obj}
		${txtfilt} /tmp/et$$.txt ${ex_output}.txt
	    else
		${txtfilt} /tmp/et$$-2.txt ${ex_output}.txt
	    fi
	else
	    $xpm2txt -w48 -h64 ${ex_output}.xpm /tmp/et$$.txt
	    sed -f ${ex_output}.sed < /tmp/et$$.txt > ${ex_output}.txt
	fi
    else
	echo "Warning: No ${tileset} tiles imported from subset $long"
    fi
    rm -f ${ex_output}.sed /tmp/et$$.txt /tmp/et$$-2.txt /tmp/et$$.ppm
    rm -f ${ex_output}.ppm ${ex_output}.xpm
}
set -e
if test ! -d ${input}/pxxx; then
    echo "Error: Can't find bitmaps: ${input}/pxxx: No such directory"
    exit 1
fi
if test ! \( -r ${tilemap}-monsters.tile -a -r ${tilemap}-objects.tile -a \
  -r ${tilemap}-other.tile \) ; then
    echo "Error: Can't find tilemap"
    exit 1
fi
if test ! \( -x "${xpm2txt}" -a -x "${txtfilt}" \) ; then
    echo "Error: Can't find tile utils"
    exit 1
fi
rm -f mon32mi.ppm obj32mi.ppm oth32mi.ppm
import 32 monsters
import 32 objects
import 32 other
rm -f mon3dmi.ppm obj3dmi.ppm oth3dmi.ppm
import 3d monsters
import 3d objects
import 3d other

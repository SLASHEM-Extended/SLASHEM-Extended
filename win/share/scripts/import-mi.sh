#!/bin/sh
#
#   Copyright (c) J. Ali Harlow 2003
#   NetHack may be freely redistributed.  See license for details.
#
#   This shell script imports Mitshurio Itakura's tilesets from
#   his diy package. You must have a copy of this package first!
#   diy.tgz is available from
#     http://www.geocities.co.jp/SiliconValley-SanJose/9606/nh/eng.html
#   It should be unpacked into a suitable direcory and the variable
#   below set to point at it.
#
#   There appear to be a few tiles that didn't make it into diy. These
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
input=diy
tilemap=mi
palette=../palette.txt
extra_mon=mon32mix.txt
extra_obj=obj32mix.txt
xpm2txt=../../../util/xpm2txt
txtfilt=../../../util/txtfilt
txtmerge=../../../util/txtmerge
#
# Functions
#
set -e
generate_palette()
# Usage: generate_palette <input-txt> <output-ppm>
{
    awk '/^[A-Za-z$_]+[ 	]*=[ 	]*\([0-9][0-9]*,[ 	]*[0-9][0-9]*,[ 	]*[0-9][0-9]*\)$/ {print}' $1 > /tmp/pal$$.txt
    echo P3 1 `wc -l /tmp/pal$$.txt | awk '{print $1}'` 255 > $2
    sed -e 's/.*(\(.*\))/\1/' -e 's/,[ 	]*/ /g' < /tmp/pal$$.txt >> $2
    rm -f /tmp/pal$$.txt
}
generate_tilemap()
# Usage: generate_tilemap <tilemap> <input>
{
    tr -d '\015' < $2 | grep -v '^#' | awk "BEGIN {subset=\"monsters\"; \
	cmap[67]=\"dig beam\"; \
	cmap[68]=\"camera flash\"; \
	cmap[69]=\"thrown boomerang, open left\"; \
	cmap[70]=\"thrown boomerang, open right\"; \
	cmap[71]=\"magic shield 1\"; \
	cmap[72]=\"magic shield 2\"; \
	cmap[73]=\"magic shield 3\"; \
	cmap[74]=\"magic shield 4\"; \
	cmap[75]=\"swallow top left\"; \
	cmap[76]=\"swallow top center\"; \
	cmap[77]=\"swallow top right\"; \
	cmap[78]=\"swallow middle left\"; \
	cmap[79]=\"swallow middle right\"; \
	cmap[80]=\"swallow bottom left\"; \
	cmap[81]=\"swallow bottom center\"; \
	cmap[82]=\"swallow bottom right\"; \
	} \
	{ \
	    id=\$1; \
	    \$1=\"\"; \
	    if (\$0 == \" strange object\") \
		subset=\"objects\"; \
	    else if (\$0 == \" wall\") \
		subset=\"other\"; \
	    else if (\$0 == \" blank\") \
		subset=\"special\"; \
	    if (\$2 == \"explode\" || \$2 == \"cmap\" && \
	      \$3 + 0 >= 83 && \$3 + 0 <= 91) { \
		\$2 = \"explosion\"; \
		if (\$3 == \"black\") \
		    \$3 = \"dark\"; \
		else if (\$3 == \"green\") \
		    \$3 = \"noxious\"; \
		else if (\$3 == \"brown\") \
		    \$3 = \"muddy\"; \
		else if (\$3 == \"blue\") \
		    \$3 = \"wet\"; \
		else if (\$3 == \"purple\") \
		    \$3 = \"magical\"; \
		else if (\$3 == \"darkblue\") \
		    \$3 = \"frosty\"; \
		else { \
		    \$4 = \$3; \
		    \$3 = \"fiery\"; \
		} \
		\$4 = \$4 - 83; \
	    } else if (\$2 == \"cmap\" && cmap[\$3]\"\" != \"\") \
	    { \
		\$4 = cmap[\$3]; \
		\$3 = \"/\"; \
	    } else if (\$0 == \" anti-magic trap field\") \
	    { \
		\$3 = \$4; \
		NF--; \
	    } else if (\$0 == \" beholder\") \
		\$2 = \"Beholder\"; \
	    printf(\"%s@%s@\",\"$1\",subset); \
	    for(i=2;i<NF;i++) \
		printf(\"%s \",\$i); \
	    printf(\"%s@%s\\n\",\$NF,id); \
	}" > ${tilemap}.tile
    fgrep "@monsters@" ${tilemap}.tile > ${tilemap}-monsters.tile
    fgrep "@objects@" ${tilemap}.tile > ${tilemap}-objects.tile
    fgrep "@other@" ${tilemap}.tile > ${tilemap}-other.tile
    rm -f ${tilemap}.tile
}
addtile()
# Usage: addtile <output> <id> <name> <input>
{
    ad_output=$1
    ad_id=$2
    ad_name=$3
    ad_input=$4
    ad_sed=`echo ${ad_output} | sed 's/\.[^.]*$/.sed/'`
    if test -r $ad_output; then
	pnmcat -lr $ad_output $ad_input > /tmp/ad$$.pnm || exit 1
	mv /tmp/ad$$.pnm $ad_output
	ad_count=`echo ${ad_count} + 1 | bc`
    else
	rm -f ${ad_sed}
	anytopnm $ad_input > $ad_output || exit 1
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
    echo "s£^# tile ${ad_count} (unknown)£# tile ${ad_id} ($ad_name)£" >> ${ad_sed}
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
    if test "$tileset" = "32"; then
	ppmmake rgb:47/6c/6c 32 32 > background.ppm
    else
	ppmmake rgb:47/6c/6c 48 64 > background.ppm
    fi
    rm -f ${ex_output}.ppm ${ex_output}-fs.ppm
    # We maintain both an undithered and a dithered collection.
    # The undithered collection is used to filter out tiles which
    # are simply a magnification of the 16x16 tiles. The dithered
    # collection are used for the final tile bitmaps.
    cat ${tilemap}-${long}.tile | while read line; do
	tag=`echo $line | sed 's:.*@\([^@]*\)$:\1:'`
	name=`echo $line | sed 's:.*@\([^@]*\)@[^@]*$:\1:'`
	bmp=$input/${key}xxx/$tag.bmp
	if test -r $bmp; then
	    bmptoppm $bmp > /tmp/et$$.ppm 2> /dev/null
	    ppmcolormask rgb:47/6c/6c /tmp/et$$.ppm > /tmp/et$$.pbm
	    ppmquant -map palette.ppm /tmp/et$$.ppm 2> /dev/null | \
	      ppmchange rgb:47/6c/6c rgb:48/6c/6c | \
	      pnmcomp -alpha=/tmp/et$$.pbm background.ppm > /tmp/et-2$$.ppm
	    addtile ${ex_output}.ppm 0 "$name" /tmp/et-2$$.ppm
	    rm -f /tmp/et$$.ppm /tmp/et$$-2.ppm /tmp/et$$.pbm
	fi
    done || exit 1
    cat ${tilemap}-${long}.tile | while read line; do
	tag=`echo $line | sed 's:.*@\([^@]*\)$:\1:'`
	name=`echo $line | sed 's:.*@\([^@]*\)@[^@]*$:\1:'`
	bmp=$input/${key}xxx/$tag.bmp
	if test -r $bmp; then
	    bmptoppm $bmp > /tmp/et$$.ppm 2> /dev/null
	    ppmcolormask rgb:47/6c/6c /tmp/et$$.ppm > /tmp/et$$.pbm
	    ppmquant -fs -map palette.ppm /tmp/et$$.ppm 2> /dev/null | \
	      ppmchange rgb:47/6c/6c rgb:48/6c/6c | \
	      pnmcomp -alpha=/tmp/et$$.pbm background.ppm > /tmp/et-2$$.ppm
	    addtile ${ex_output}-fs.ppm 0 "$name" /tmp/et-2$$.ppm
	    rm -f /tmp/et$$.ppm /tmp/et$$-2.ppm /tmp/et$$.pbm
	fi
    done || exit 1
    rm -f background.ppm
    if test -r ${ex_output}.ppm; then
	ppmcolormask rgb:47/6c/6c ${ex_output}.ppm > /tmp/et$$.pbm
	ppmtoxpm -alphamask=/tmp/et$$.pbm ${ex_output}.ppm > ${ex_output}.xpm
	ppmcolormask rgb:47/6c/6c ${ex_output}-fs.ppm > /tmp/et$$.pbm
	ppmtoxpm -alphamask=/tmp/et$$.pbm ${ex_output}-fs.ppm \
	  > ${ex_output}-fs.xpm
	rm -f /tmp/et$$.pbm
	if test "$tileset" = "32"; then
	    $xpm2txt -w32 -h32 ${ex_output}.xpm /tmp/et$$.txt
	    sed -f ${ex_output}.sed < /tmp/et$$.txt > /tmp/et$$-2.txt
	    $xpm2txt -w32 -h32 ${ex_output}-fs.xpm /tmp/et$$.txt
	    sed -f ${ex_output}-fs.sed < /tmp/et$$.txt > /tmp/et$$-3.txt
	    if test "$short" = "mon" -a -n "${extra_mon}"; then
		extra=${extra_mon}
	    elif test "$short" = "obj" -a -n "${extra_obj}"; then
		extra=${extra_obj}
	    else
		extra=
	    fi
	    ${txtmerge} /tmp/et$$.txt /tmp/et$$-2.txt ${extra}
	    ${txtfilt} -p ../palette.txt /tmp/et$$.txt /tmp/et$$-4.txt
	    egrep '^# tile [0-9]+ (.*)$' /tmp/et$$-4.txt | \
	      sed 's/^# tile [0-9][0-9]* (\(.*\))$/\1/' > /tmp/et$$.pat
	    ${txtmerge} /tmp/et$$.txt /tmp/et$$-3.txt ${extra}
	    ${txtfilt} -p ../palette.txt -f /tmp/et$$.pat /tmp/et$$.txt \
	      ${ex_output}.txt
	else
	    $xpm2txt -w48 -h64 ${ex_output}-fs.xpm /tmp/et$$.txt
	    sed -f ${ex_output}.sed < /tmp/et$$.txt > ${ex_output}.txt
	fi
    else
	echo "Warning: No ${tileset} tiles imported from subset $long"
    fi
    rm -f ${ex_output}.sed /tmp/et$$.txt /tmp/et$$-2.txt /tmp/et$$.ppm
    rm -f ${ex_output}-fs.sed /tmp/et$$-3.txt /tmp/et$$-4.txt  /tmp/et$$.pat
    rm -f ${ex_output}.ppm ${ex_output}.xpm
    rm -f ${ex_output}-fs.ppm ${ex_output}-fs.xpm
}
set -e
if test ! -d ${input}/pxxx; then
    echo "Error: Can't find bitmaps: ${input}/pxxx: No such directory"
    exit 1
fi
if test ! -r ${palette}; then
    echo "Error: Can't find palette file: $palette"
    exit 1
fi
if test ! -r palette.ppm; then
    generate_palette ${palette} palette.ppm
fi
if test ! \( -r ${tilemap}-monsters.tile -a -r ${tilemap}-objects.tile -a \
  -r ${tilemap}-other.tile \) ; then
    generate_tilemap $tilemap ${input}/name.txt
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

#!/bin/bash
morethanone=false
levels=$( cat $1 | grep "^MAZE:\|^LEVEL:" | cut -d'"' -f2 | sed s/$/.lev/ )
levels=$( echo ${levels} )
echo SPECIAL_LEVELS += ${levels}
for levfile in ${levels}; do
    if ${morethanone} ; then
        echo dat/${levfile}: dat/${firstlevfile}
    else
        firstlevfile=${levfile}
        morethanone=true
        echo dat/${levfile}: dat/$1 util/lev_comp
        echo "	( cd dat ; ../util/lev_comp $1 )"
    fi
done


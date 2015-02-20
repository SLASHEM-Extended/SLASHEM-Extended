#!/bin/bash

for directory in src util sys/unix sys/share win/tty ; do
    for srcfile in ${directory}/*.c ; do
        cc -Iinclude -MM -MG ${srcfile} | sed "/:/ s=^=${directory}/="
    done
done

( cd dat ; for desfile in *.des ; do
    morethanone=false
    levels=$( cat ${desfile} | grep "^MAZE:\|^LEVEL:" | cut -d'"' -f2 | sed s/$/.lev/ )
    levels=$( echo ${levels} )
    echo SPECIAL_LEVELS += ${levels}
    for levfile in ${levels}; do
        if ${morethanone} ; then
            echo dat/${levfile}: dat/${firstlevfile}
        else
            firstlevfile=${levfile}
            morethanone=true
            echo dat/${levfile}: dat/${desfile} util/lev_comp
            echo "	( cd dat ; ../util/lev_comp ${desfile} )"
        fi
    done
done )

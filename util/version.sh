#!/bin/sh

printf "%s.%s.%s" $( echo -e '#include "include/patchlevel.h"\nVERSION_MAJOR VERSION_MINOR PATCHLEVEL' | gcc -E - | tail -n1 )

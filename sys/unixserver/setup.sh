#!/bin/sh
# Link makefiles to their correct locations.

# Were we started from the top level?  Cope.
if [ -f sys/unixserver/Makefile.top ]; then cd sys/unixserver; fi

umask 0
ln -s sys/unixserver/Makefile.top ../../Makefile
ln -s ../sys/unixserver/Makefile.dat ../../dat/Makefile
ln -s ../sys/unixserver/Makefile.doc ../../doc/Makefile
ln -s ../sys/unixserver/Makefile.src ../../src/Makefile
ln -s ../sys/unixserver/Makefile.utl ../../util/Makefile

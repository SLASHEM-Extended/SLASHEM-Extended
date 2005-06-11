#!/bin/sh
#
# $Id: bootstrap.sh,v 1.1 2005-06-11 15:52:45 j_ali Exp $
#
case "$0" in
    */*)	confdir=`echo $0 | sed 's:/[^/]*$::'` ;;
    *)		confdir='.' ;;
esac
set -e
cd $confdir
# Avoid autoheader creating backup file
rm -f autoconf.hin
aclocal
# We can't avoid running autotools in multiple directories,
# but at least we can have only one cache.
rm -rf ../../autom4te.cache
mv autom4te.cache ../..
# autoheader must be run from topdir since configure.ac refers
# to sys/autoconf/autoconf.hin
(cd ../.. && autoheader -I sys/autoconf sys/autoconf/configure.ac)
mv ../../autom4te.cache .
autoconf configure.ac > ../../configure
rm -rf autom4te.cache aclocal.m4

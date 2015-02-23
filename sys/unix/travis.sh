#!/bin/bash
set -ev

sh sys/unix/setup.sh

sed "s/WINTTYLIB = -ltermlib/WINTTYLIB = -lncurses/" -i src/Makefile
sed "s/\/\* #define LINUX \*\//#define LINUX/" -i include/unixconf.h

make
sudo make install

#!/bin/bash
set -ev

ln -s sys/unix/GNUmakefile

make
sudo make install

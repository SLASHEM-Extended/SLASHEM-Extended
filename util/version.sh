#!/bin/sh

${1:-cc} -E - <<EOF | tail -n1 | sed "s/ //g"
#include "include/patchlevel.h"
VERSION_MAJOR . VERSION_MINOR . PATCHLEVEL
EOF

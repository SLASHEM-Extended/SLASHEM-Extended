/* $Id: prxychar.c,v 1.2 2002-11-02 15:47:04 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "nhxdr.h"
#include "proxycom.h"
#include "proxysvr.h"

long *
proxy_map_glyph2char(glyph_map)
struct proxycb_get_glyph_mapping_res *glyph_map;
{
    int glyph = 0;
    struct proxy_glyph_map_info info;
    struct proxy_glyph_mapping *mapping;
    long *glyph2rgbsym;
    glyph2rgbsym = (long *)alloc(glyph_map->no_glyph * sizeof(long));
    mapping = proxy_glyph_map_first(&info, glyph_map);
    while (mapping) {
	glyph2rgbsym[glyph++] = mapping->rgbsym;
	mapping = proxy_glyph_map_next(&info);
    }
    proxy_glyph_map_close(&info);
    return glyph2rgbsym;
}

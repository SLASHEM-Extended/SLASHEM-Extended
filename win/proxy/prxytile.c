/* $Id: prxytile.c,v 1.2 2002-09-12 18:21:48 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <stdio.h>
#include "hack.h"
#include "winproxy.h"
#include "proxysvr.h"
#include "proxycb.h"

/* #define DEBUG */

/*
 * entry is a comma seperated list of descriptions.
 * '\' is treated as an escape character.
 */

static void
proxy_tilemap_add_entry(struct proxy_tilemap *map, int tn, char *entry)
{
    int i, j, k, idx;
    char buf[1024];
    if (map->no_entries >= map->max_entries) {
	if (map->max_entries) {
	    map->max_entries *= 2;
	    map->entries = realloc(map->entries,
		    map->max_entries * sizeof(struct proxy_tilemap_entry));
	} else {
	    map->max_entries = 32;
	    map->entries =
		    malloc(map->max_entries * sizeof(struct proxy_tilemap_entry));
	}
	if (!map->entries)
	    panic("Not enough memory to load tile map");
    }
    idx = map->no_entries++;
    map->entries[idx].refs = 0;
    map->entries[idx].tile = tn;
    for(i = 0, j = 1; entry[i]; i++)
	if (entry[i] == '\\' && entry[i + 1])
	    i++;
	else if (entry[i] == ',')
	    j++;
    map->entries[idx].no_descs = j;
    map->entries[idx].descs = (char **)alloc(j * sizeof(char *));
    for(i = j = k = 0; ; i++)
	if (entry[i] == '\\' && entry[i + 1])
	    buf[k++] = entry[++i];
	else if (entry[i] == ',' || !entry[i]) {
	    map->entries[idx].descs[j] = (char *)alloc(k + 1);
	    strncpy(map->entries[idx].descs[j], buf, k);
	    map->entries[idx].descs[j++][k] = '\0';
	    if (!entry[i])
		break;
	    k = 0;
	}
	else if (k || entry[i] != ' ')
	    buf[k++] = entry[i];
    if (j != map->entries[idx].no_descs)
	panic("Bad description count in proxy_tilemap_set_entry");
#ifdef DEBUG
    fprintf(stderr, "tile %d \"", tn);
    for(i = j = 0; i < map->entries[idx].no_descs; i++) {
	if (!map->entries[idx].descs[i] ||
	  !*map->entries[idx].descs[i])
	    continue;
	if (j)
	    fputs(", ", stderr);
	fputs(map->entries[idx].descs[i], stderr);
	j++;
    }
    fputs("\"\n", stderr);
#endif
}

struct proxy_tilemap *
proxy_load_tilemap(fh)
int fh;
{
    int i, j, k, tn;
    char buf[1024], buf2[256];
    char *cmd, *arg, *s, c;
    struct proxy_tilemap *map;
    map = (struct proxy_tilemap *)alloc(sizeof(struct proxy_tilemap));
    map->no_entries = map->max_entries = 0;
    map->no_tiles = 0;
    map->entries = NULL;
    while(proxy_cb_dlbh_fgets(buf, 1024, fh)) {
	cmd = buf;
	arg = strchr(buf, ' ');
	if (arg)
	    *arg++ = '\0';
	if (!strcmp(cmd, "tile") && arg &&
		sscanf(arg, "%d \"%255[^\"]", &tn, buf2) == 2) {
	    map->no_tiles ++;
	    /* The string consists of alternate descriptions seperated
	     * by '/' characters. Split these up (honouring the '\'
	     * escape) and add each description to the tilemap.
	     * Spaces are removed surrounding '/' characters.
	     */
	    for(i = j = 0; ; i++)
		if (buf2[i] == '\\' && buf2[i + 1])
		    i++;
		else if (buf2[i] == '/' || !buf2[i]) {
		    for(k = i - 1; buf2[k] == ' '; k--)
			;
		    k++;
		    c = buf2[k];
		    buf2[k] = '\0';
		    proxy_tilemap_add_entry(map, tn, buf2 + j);
		    buf2[k] = c;
		    if (!buf2[i])
			break;
		    j = i + 1;
		    while(buf2[j] == ' ')
			j++;
		}
	}
	/* else ignore unrecognized map commands */
    }
    return map;
}

void
proxy_free_tilemap(map)
struct proxy_tilemap *map;
{
    int i, j;
    for(i = 0; i < map->no_entries; i++) {
	for(j = 0; j < map->entries[i].no_descs; j++)
	    free(map->entries[i].descs[j]);
	free(map->entries[i].descs);
    }
    free(map);
}

/*
 * Return the number of ordered matches between the descriptions.
 * Encoded as number of matches in top 16 bits and exact matches in lower 16.
 * This has the effect that a larger number of matches will always win but
 * where there are an equal number of matches, the number of exact matches
 * is taken into account.
 */

static int
proxy_match_descriptions(struct proxy_tilemap_entry *tile_entry,
  struct proxy_glyph_mapping *glyph_desc)
{
    int i, j;
    int no_matches = 0, no_exact_matches = 0;
    int last_match = -1;
    for(i = 0; i < tile_entry->no_descs; i++) {
	if (!tile_entry->descs[i])
	    continue;
	for(j = last_match + 1; j < glyph_desc->no_descs; j++) {
	    if (!glyph_desc->descs[j])
		continue;
	    if (!strcmp(tile_entry->descs[i], glyph_desc->descs[j])) {
		no_matches++;
		no_exact_matches++;
		last_match = j;
		break;
	    } else if (!strcmp(tile_entry->descs[i], "*")) {
		no_matches++;
		last_match = j;
		break;
	    }
	}
    }
    return no_matches << 16 | no_exact_matches;
}

static short
proxy_map_glyph(struct proxy_tilemap *tile_map,
  struct proxy_glyph_mapping *desc)
{
    int i, j, k;
    int best = -1;
    int best_refs;
    int best_matches;
    for(i = 0; i < tile_map->no_entries; i++) {
	j = proxy_match_descriptions(tile_map->entries + i, desc);
	if (best < 0 || j > best_matches ||
	  j == best_matches && tile_map->entries[i].refs < best_refs) {
	    best = i;
	    best_refs = tile_map->entries[i].refs;
	    best_matches = j;
	}
    }
    if (best >= 0) {
	tile_map->entries[best].refs++;
	return tile_map->entries[best].tile;
    }
    else
	return -1;
}

#ifdef DEBUG
static short
proxy_log_mapping(int glyph, int tile, struct proxy_tilemap *tile_map, struct proxy_glyph_description *desc)
{
    int i, j, k;
    fprintf(stderr, "glyph %d", glyph);
    if (desc) {
	fputs(" \"", stderr);
	for(i = j = 0; i < desc->no_descs; i++) {
	    if (!desc->descs[i] || !*desc->descs[i])
		continue;
	    if (j)
		fputs(", ", stderr);
	    fputs(desc->descs[i], stderr);
	    j++;
	}
	fputc('"', stderr);
    }
    if (tile >= 0) {
	fprintf(stderr, " mapped to tile %d \"", tile);
	for(k = j = 0; k < tile_map->no_entries; k++) {
	    if (tile_map->entries[k].tile == tile) {
		if (j)
		    fputs(" / ", stderr);
		for(i = j = 0; i < tile_map->entries[k].no_descs; i++) {
		    if (!tile_map->entries[k].descs[i] ||
		      !*tile_map->entries[k].descs[i])
			continue;
		    if (j)
			fputs(", ", stderr);
		    fputs(tile_map->entries[k].descs[i], stderr);
		    j++;
		}
		fprintf(stderr, " {%d}", tile_map->entries[k].refs);
	    }
	}
	fputs("\"\n", stderr);
    } else
	fputs(" not mapped\n", stderr);
}

#define PROXY_MAP_GLYPH(glyph, tile, tile_map, desc) \
	if (1) { \
	    int PROXY_MAP_GLYPH_gn = (glyph); \
	    glyph2tile[PROXY_MAP_GLYPH_gn] = (tile); \
	    proxy_log_mapping(PROXY_MAP_GLYPH_gn, tile, tile_map, desc); \
	} else
#else	/* DEBUG */
#define PROXY_MAP_GLYPH(glyph, tile, tile_map, desc) \
	(glyph2tile[(glyph)] = (tile))
#endif	/* DEBUG */

short *
proxy_map_glyph2tile(glyph_map, tile_map)
struct proxycb_get_glyph_mapping_res *glyph_map;
struct proxy_tilemap *tile_map;
{
    int i, j, k, m, glyph = 0;
    struct proxy_glyph_map_info info;
    struct proxy_glyph_mapping *mapping;
    struct forward_ref {
	int first_glyph;
	int no_glyphs;
	int ref_glyph;
    };
    int no_forward_refs = 0;
    struct forward_ref *forward_refs = NULL, *fr;
    short *glyph2tile;
    glyph2tile = (short *)alloc(glyph_map->no_glyph * sizeof(short));
    for(i = 0; i < glyph_map->no_glyph; i++)
	glyph2tile[i] = -1;
    mapping = proxy_glyph_map_first(&info, glyph_map);
    while (mapping) {
	/* TODO: Where the tileset defines tiles for a mapping, this should
	 * take precedence over the alternate glyph. Currently, we always
	 * use the alternative glyph, if set.
	 */
	if (mapping->alt_glyph != glyph_map->no_glyph) {
	    m = glyph2tile[mapping->alt_glyph];
	    if (m < 0) {
		/* Referenced glyph has not yet been mapped */
		if (no_forward_refs && fr->ref_glyph == mapping->alt_glyph &&
		  fr->first_glyph + fr->no_glyphs == glyph)
		    fr->no_glyphs++;
		else {
		    if (no_forward_refs++)
			forward_refs = realloc(forward_refs,
			  no_forward_refs * sizeof (struct forward_ref));
		    else
			forward_refs = malloc(sizeof (struct forward_ref));
		    if (!forward_refs)
			panic("Not enough memory to map glyphs");
		    fr = forward_refs + no_forward_refs - 1;
		    fr->first_glyph = glyph;
		    fr->no_glyphs = 1;
		    fr->ref_glyph = mapping->alt_glyph;
		}
		glyph++;
	    }
	    else
		PROXY_MAP_GLYPH(glyph++, m, tile_map, NULL);
	} else {
	    m = proxy_map_glyph(tile_map, mapping);
	    PROXY_MAP_GLYPH(glyph++, m, tile_map, mapping);
	}
	mapping = proxy_glyph_map_next(&info);
    }
    proxy_glyph_map_close(&info);
    /* Handle any forward references (ignoring any that reference undefined
     * glyphs - we treat these just like any other undefined glyphs).
     */
    do {
	k = 0;
	fr = forward_refs;
	for(i = 0; i < no_forward_refs; i++, fr++) {
	    if (!fr->no_glyphs)
		continue;
	    m = glyph2tile[fr->ref_glyph];
	    if (m >= 0) {
		for(j = 0; j < fr->no_glyphs; j++)
		    PROXY_MAP_GLYPH(fr->first_glyph + j, m, tile_map, NULL);
		fr->no_glyphs = 0;
		k = 1;	/* We've done some work */
	    }
	    else if (!k) {
		m = fr->ref_glyph;
		fr = forward_refs;
		for(j = 0; j < no_forward_refs; j++, fr++)
		    if (fr->no_glyphs && m >= fr->first_glyph &&
		      m < fr->first_glyph + fr->no_glyphs)
			break;
		fr = forward_refs + i;
		if (j < no_forward_refs)
		    /* There's work still to do (and we haven't done any) */
		    k = -1;
	    }
	}
	if (k < 0)
	    panic("Cyclic forward references in glyph mapping");
    } while(k);
    free(forward_refs);
    /* Make certain all glyphs map to _something_ */
    glyph = cmap_to_glyph(S_stone);
    j = glyph2tile[glyph];
    if (j < 0)
	j = 0;
    for(i = 0; i < glyph_map->no_glyph; i++)
	if (glyph2tile[i] < 0)
	    glyph2tile[i] = j;
    return glyph2tile;
}

/* $Id: proxysvr.h,v 1.2 2002-10-05 19:22:54 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROXYSVR_H
#define PROXYSVR_H

extern short *proxy_glyph2tile;

struct proxy_tilemap_entry {
    unsigned int tile;
    unsigned int refs;
    unsigned int no_descs;
    char **descs;
};

struct proxy_tilemap {
    unsigned int no_entries;		/* One tile may have several entries */
    unsigned int no_tiles;
    unsigned int max_entries;		/* No. allocted entries */
    struct proxy_tilemap_entry *entries;
};

#define PROXY_LEVEL_MAPPING		0
#define PROXY_LEVEL_FLAGS		1
#define PROXY_LEVEL_SUBMAPPING		2
#define PROXY_LEVEL_GLYPH		3
#define PROXY_LEVEL_BASED_MAPPING	4
#define PROXY_LEVEL_BASED_SUBMAPPING	5
#define PROXY_LEVEL_BASED_GLYPH		6
#define PROXY_MAX_NO_LEVELS		7

struct proxy_glyph_mapping {
    unsigned long rgbsym;
    int alt_glyph;
    unsigned int no_descs;
    const char **descs;
};

struct proxy_glyph_map_info {
    struct proxy_glyph_mapping current;
    struct proxycb_get_glyph_mapping_res *glyph_map;
    int mi, smi, gi, bsmi, bgi;
    const char *descs[PROXY_MAX_NO_LEVELS];
};

/* ### proxysvc.c ### */

extern void proxy_svc_set_ext_procs(void (*)(void), struct window_ext_procs *);
extern int win_proxy_svr_init(void *, void *);
extern char *win_proxy_svr_get_failed_packet(int *);

/* ### prxymap.c ### */

extern struct proxy_glyph_mapping *proxy_glyph_map_first(
	struct proxy_glyph_map_info *, struct proxycb_get_glyph_mapping_res *);
extern struct proxy_glyph_mapping *proxy_glyph_map_next(
	struct proxy_glyph_map_info *);
extern void proxy_glyph_map_close(struct proxy_glyph_map_info *);

/* ### prxytile.c ### */

extern struct proxy_tilemap *proxy_load_tilemap(int);
extern void proxy_free_tilemap(struct proxy_tilemap *);
extern short * proxy_map_glyph2tile(
	struct proxycb_get_glyph_mapping_res *glyph_map,
	struct proxy_tilemap *);

/* ### prxychar.c ### */

extern long *proxy_map_glyph2char(struct proxycb_get_glyph_mapping_res *);

#endif /* PROXYSVR_H */

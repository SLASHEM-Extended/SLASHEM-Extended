/* $Id: proxysvr.h,v 1.1 2002-09-01 21:58:18 j_ali Exp $ */
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

/* ### proxysvc.c ### */

extern void proxy_svc_set_ext_procs(void (*)(void), struct window_ext_procs *);
extern int win_proxy_svr_init(int, int);

/* ### prxytile.c ### */

extern struct proxy_tilemap *proxy_load_tilemap(int);
extern void proxy_free_tilemap(struct proxy_tilemap *);
extern int proxy_map_glyph2tile(struct proxycb_get_glyph_mapping_res *glyph_map,
	struct proxy_tilemap *);

#endif /* PROXYSVR_H */

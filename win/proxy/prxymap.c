/* $Id: prxymap.c,v 1.3 2002-11-02 15:47:04 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <stdio.h>
#include "nhxdr.h"
#include "proxycom.h"
#include "proxysvr.h"

static void
proxy_set_description(struct proxy_glyph_mapping *datum, int level,
  const char *description)
{
    for(; datum->no_descs < level; datum->no_descs++)
	datum->descs[datum->no_descs] = NULL;
    datum->descs[level] = !description || !*description ? NULL : description;
    if (datum->no_descs <= level)
	datum->no_descs = level + 1;
}

static void
proxy_set_symdef(struct proxy_glyph_map_info *info, int level,
  struct proxycb_get_glyph_mapping_res_symdef *symdef)
{
    int rgb, sym;
    /*
     * Update the rgbsym with the new information, if present.
     *
     * Where both the existing infomation and the new information
     * are opaque then the existing information is kept when the
     * new data comes mapping which is based on a second mapping
     * and the new information is used when the mapping isn't based
     * on any other. This allows the mapping for corpses to override
     * the symbol of the monster mappings (colours could also be
     * overridden if so desired).
     */
    if (level < PROXY_LEVEL_BASED_MAPPING) {
	rgb = RGBSYM_RGB(info->current.rgbsym);
	if (rgb == RGBSYM_RGB(info->glyph_map->transparent))
	    rgb = RGBSYM_RGB(symdef->rgbsym);
	sym = RGBSYM_SYM(info->current.rgbsym);
	if (sym == RGBSYM_SYM(info->glyph_map->transparent))
	    sym = RGBSYM_SYM(symdef->rgbsym);
    } else {
	rgb = RGBSYM_RGB(symdef->rgbsym);
	if (rgb == RGBSYM_RGB(info->glyph_map->transparent))
	    rgb = RGBSYM_RGB(info->current.rgbsym);
	sym = RGBSYM_SYM(symdef->rgbsym);
	if (sym == RGBSYM_SYM(info->glyph_map->transparent))
	    sym = RGBSYM_SYM(info->current.rgbsym);
    }
    info->current.rgbsym = RGB_SYM(rgb, sym);
    proxy_set_description(&info->current, level, symdef->description);
}

static struct proxy_glyph_mapping *
proxy_glyph_map_current(info)
struct proxy_glyph_map_info *info;
{
    struct proxycb_get_glyph_mapping_res_mapping *mapping, *base;
    mapping = info->glyph_map->mappings + info->mi;
    info->current.no_descs = 0;
    info->current.rgbsym = info->glyph_map->transparent;
    if (mapping->base_mapping >= 0) {
	if (mapping->base_mapping < info->mi)
	    base = info->glyph_map->mappings + mapping->base_mapping;
	else
	    /* Forward references to mappings are not supported */
	    panic("Glyph mapping %d based on undefined mapping", info->mi);
	proxy_set_symdef(info, PROXY_LEVEL_MAPPING, &base->symdef);
	proxy_set_symdef(info, PROXY_LEVEL_BASED_MAPPING, &mapping->symdef);
    } else {
	base = NULL;
	proxy_set_symdef(info, PROXY_LEVEL_MAPPING, &mapping->symdef);
    }
    /* We ignore flags from our base mapping (if any)
     * and always use our own.
     */
    proxy_set_description(&info->current, PROXY_LEVEL_FLAGS, mapping->flags);
    info->current.alt_glyph = mapping->alt_glyph;
    if (!mapping->n_submappings) {
	if (!base)
	    panic("Glyph mapping %d has no base and no sub-mappings", info->mi);
	proxy_set_symdef(info, PROXY_LEVEL_SUBMAPPING,
	  &base->submappings[info->bsmi].symdef);
	proxy_set_symdef(info, PROXY_LEVEL_GLYPH,
	  &base->submappings[info->bsmi].glyphs[info->bgi]);
    } else if (base) {
	if (!base->n_submappings)
	    panic("Glyph mapping %d based on mapping with no sub-mappings", 
	      info->mi);
	proxy_set_symdef(info, PROXY_LEVEL_SUBMAPPING,
	  &base->submappings[info->bsmi].symdef);
	proxy_set_symdef(info, PROXY_LEVEL_GLYPH,
	  &base->submappings[info->bsmi].glyphs[info->bgi]);
	proxy_set_symdef(info, PROXY_LEVEL_BASED_SUBMAPPING,
	  &mapping->submappings[info->smi].symdef);
	proxy_set_symdef(info, PROXY_LEVEL_BASED_GLYPH,
	  &mapping->submappings[info->smi].glyphs[info->gi]);
    } else {
	proxy_set_symdef(info, PROXY_LEVEL_SUBMAPPING,
	  &mapping->submappings[info->smi].symdef);
	proxy_set_symdef(info, PROXY_LEVEL_GLYPH,
	  &mapping->submappings[info->smi].glyphs[info->gi]);
    }
    return &info->current;
}

struct proxy_glyph_mapping *
proxy_glyph_map_first(info, glyph_map)
struct proxy_glyph_map_info *info;
struct proxycb_get_glyph_mapping_res *glyph_map;
{
    info->glyph_map = glyph_map;
    info->mi = info->smi = info->gi = 0;
    info->bsmi = info->bgi = 0;
    info->current.descs = info->descs;
    return proxy_glyph_map_current(info);
}

struct proxy_glyph_mapping *
proxy_glyph_map_next(info)
struct proxy_glyph_map_info *info;
{
    struct proxycb_get_glyph_mapping_res_mapping *mapping, *base;
    if (info->mi >= info->glyph_map->n_mappings)
	return (struct proxy_glyph_mapping *)0;
    mapping = info->glyph_map->mappings + info->mi;
    if (mapping->base_mapping >= 0)
	base = info->glyph_map->mappings + mapping->base_mapping;
    else
	base = NULL;
    if (++info->gi >= (mapping->n_submappings ?
      mapping->submappings[info->smi].n_glyphs : 0)) {
	if (++info->smi >= mapping->n_submappings) {
	    if (++info->bgi >= (base && base->n_submappings ?
	      base->submappings[info->bsmi].n_glyphs : 0)) {
		if (++info->bsmi >= (base ? base->n_submappings : 0)) {
		    if (++info->mi >= info->glyph_map->n_mappings)
			return (struct proxy_glyph_mapping *)0;
		    info->bsmi = 0;
		}
		info->bgi = 0;
	    }
	    info->smi = 0;
	}
	info->gi = 0;
    }
    return proxy_glyph_map_current(info);
}

void
proxy_glyph_map_close(info)
struct proxy_glyph_map_info *info;
{
}

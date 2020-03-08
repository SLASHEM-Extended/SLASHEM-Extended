/* $Id: glyphmap.c,v 1.4 2002-11-25 14:23:46 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "nhxdr.h"
#include "proxycom.h"
#include "winproxy.h"

/* #define DEBUG */

/* Glyph mapping ignores the value of iflags.use_color. This flag should be
 * checked by the window port where appropriate. However, we must take account
 * of TEXTCOLOR because colours will not be available when this is not defined.
 */

#ifdef C
#undef C
#endif
#ifdef TEXTCOLOR
#define C(n)	n
#else
#define C(n)	NO_COLOR
#endif

#define RGB(r, g, b)	((r)<<16 | (g)<<8 | (b))

/* Some arbitary, unused, colour */

#define RGB_TRANSPARENT	RGB(0, 0, 1)

long rgb_colors[] = {
    /* Black */			RGB(0,0,0),
    /* Red */			RGB(127,0,0),
    /* Green */			RGB(0,127,0),
    /* Brown */			RGB(127,127,0),
    /* Blue */			RGB(0,0,127),
    /* Magenta */		RGB(127,0,127),
    /* Cyan */			RGB(0,127,127),
    /* Light grey */		RGB(127,127,127),
    /* Dark grey */		RGB(63,63,63),
    /* Bright red */		RGB(255,0,0),
    /* Bright green */		RGB(0,255,0),
    /* Yellow */		RGB(255,255,0),
    /* Bright blue */		RGB(0,0,255),
    /* Bright magenta */	RGB(255,0,255),
    /* Bright cyan */		RGB(0,255,255),
    /* White */			RGB(255,255,255)
};

short glyph2proxy[MAX_GLYPH];

static int
set_glyph_mapping_monsters(glyph)
int glyph;
{
    int i, j;
    for(i = 0; i < MAXMCLASSES; i++)
	if (monexplain[i])
	    for(j = 0; j < NUMMONS; j++)
		if (mons[j].mlet == i)
		    glyph2proxy[monnum_to_glyph(j)] = glyph++;
    return glyph;
}

static void
get_glyph_mapping_monsters(map)
struct proxycb_get_glyph_mapping_res_mapping *map;
{
    int i, j, k, l;
    int n_mons[MAXMCLASSES];
    map->flags = "";
    map->base_mapping = -1;
    map->alt_glyph = NO_GLYPH;
    map->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    map->symdef.description = "monster";
    for(i = 0; i < MAXMCLASSES; i++)
	n_mons[i] = 0;
    for(i = 0; i < NUMMONS; i++)
	if (mons[i].mlet >= 0 && mons[i].mlet < MAXMCLASSES)
	    n_mons[mons[i].mlet]++;
    map->n_submappings = MAXMCLASSES;
    for(i = 0; i < MAXMCLASSES; i++)
	if (!monexplain[i] || !n_mons[i])
	    map->n_submappings--;
    map->submappings = (struct proxycb_get_glyph_mapping_res_submapping *)
      alloc(map->n_submappings * sizeof(*map->submappings));
    for(i = j = 0; i < MAXMCLASSES; i++) {
	if (monexplain[i] && n_mons[i]) {
	    map->submappings[j].symdef.rgbsym =
	      RGB_SYM(RGB_TRANSPARENT, monsyms[i]);
	    map->submappings[j].symdef.description = monexplain[i];
	    map->submappings[j].n_glyphs = n_mons[i];
	    map->submappings[j].glyphs =
	      (struct proxycb_get_glyph_mapping_res_symdef *)
	      alloc(map->submappings[j].n_glyphs *
	      sizeof(*map->submappings[j].glyphs));
	    for(k = l = 0; k < NUMMONS; k++)
		if (mons[k].mlet == i) {
		    map->submappings[j].glyphs[l].rgbsym =
		      RGB_SYM(rgb_colors[C(mons[k].mcolor)], monsyms[i]);
		    map->submappings[j].glyphs[l].description = mons[k].mname;
		    l++;
		}
	    if (l != map->submappings[j].n_glyphs)
		panic("glyph_mapping_monsters: Bad no glyphs");
	    j++;
	}
    }
}

static int
set_glyph_mapping_invisible_monster(glyph)
int glyph;
{
    glyph2proxy[GLYPH_INVISIBLE] = glyph;
    return glyph + 1;
}

static void
get_glyph_mapping_invisible_monster(map)
struct proxycb_get_glyph_mapping_res_mapping *map;
{
    map->flags = "";
    map->base_mapping = -1;
    map->alt_glyph = NO_GLYPH;
    map->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    map->symdef.description = "monster";
    map->n_submappings = 1;
    map->submappings = (struct proxycb_get_glyph_mapping_res_submapping *)
      alloc(sizeof(*map->submappings));
    map->submappings->symdef.rgbsym =
      RGB_SYM(rgb_colors[NO_COLOR], DEF_INVISIBLE);	/* see mapglyph.c */
    map->submappings->symdef.description = "";
    map->submappings->n_glyphs = 1;
    map->submappings->glyphs = (struct proxycb_get_glyph_mapping_res_symdef *)
      alloc(sizeof(*map->submappings->glyphs));
    map->submappings->glyphs->rgbsym =
      RGB_SYM(rgb_colors[NO_COLOR], DEF_INVISIBLE);
    map->submappings->glyphs->description = "invisible monster";
}

static int
set_glyph_mapping_monsters_based(base_glyphs, glyph, type)
int base_glyphs, glyph, type;
{
    int i;
    switch(type)
    {
	case GLYPH_PET_OFF:
	    for(i = 0; i < base_glyphs; i++)
		glyph2proxy[petnum_to_glyph(i)] =
		  glyph2proxy[monnum_to_glyph(i)] + glyph;
	    break;
	case GLYPH_RIDDEN_OFF:
	    for(i = 0; i < base_glyphs; i++)
		glyph2proxy[ridden_monnum_to_glyph(i)] =
		  glyph2proxy[monnum_to_glyph(i)] + glyph;
	    break;
	case GLYPH_DETECT_OFF:
	    for(i = 0; i < base_glyphs; i++)
		glyph2proxy[detected_monnum_to_glyph(i)] =
		  glyph2proxy[monnum_to_glyph(i)] + glyph;
	    break;
	default:
	    panic("Bad derived monster type in glyph map");
    }
    return glyph + base_glyphs;
}

static void
get_glyph_mapping_monsters_based(map, base, type)
struct proxycb_get_glyph_mapping_res_mapping *map;
int base, type;
{
    int i;
    switch(type)
    {
	case GLYPH_PET_OFF:
	    map->flags = "pet";
	    break;
	case GLYPH_RIDDEN_OFF:
	    map->flags = "ridden";
	    break;
	case GLYPH_DETECT_OFF:
	    map->flags = "detected";
	    break;
	default:
	    panic("Bad derived monster type in glyph map");
    }
    map->base_mapping = base;
    map->alt_glyph = NO_GLYPH;
    map->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    map->symdef.description = "";
    map->n_submappings = 0;
    map->submappings = NULL;
}

static int
set_glyph_mapping_monster_corpses(base_glyphs, glyph)
int base_glyphs, glyph;
{
    int i;
    for(i = 0; i < base_glyphs; i++)
	glyph2proxy[body_to_glyph(i)] = glyph2proxy[monnum_to_glyph(i)] + glyph;
    return glyph + base_glyphs;
}

static void
get_glyph_mapping_monster_corpses(map, base)
struct proxycb_get_glyph_mapping_res_mapping *map;
int base;
{
    map->flags = "corpse";
    map->base_mapping = base;
    map->alt_glyph = objnum_to_glyph(CORPSE);
    map->symdef.rgbsym =		/* see mapglyph.c */
      RGB_SYM(RGB_TRANSPARENT, oc_syms[(int)objects[CORPSE].oc_class]);
    map->symdef.description = "";
    map->n_submappings = 0;
    map->submappings = NULL;
}

static int
set_glyph_mapping_objects(glyph)
int glyph;
{
    int i, j;
    for(i = 0; i < MAXOCLASSES; i++) {
	if (oclass_names[i]) {
	    for(j = 0; j < NUM_OBJECTS; j++)
		if (objects[j].oc_class == i)
		    glyph2proxy[objnum_to_glyph(j)] = glyph++;
	}
    }
    return glyph;
}

static void
get_glyph_mapping_objects(map)
struct proxycb_get_glyph_mapping_res_mapping *map;
{
    int i, j, k, l;
    map->flags = "";
    map->base_mapping = -1;
    map->alt_glyph = NO_GLYPH;
    map->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    map->symdef.description = "object";
    map->n_submappings = MAXOCLASSES;
    for(i = 0; i < MAXOCLASSES; i++)
	if (!oclass_names[i])
	    map->n_submappings--;
    map->submappings = (struct proxycb_get_glyph_mapping_res_submapping *)
      alloc(map->n_submappings * sizeof(*map->submappings));
    for(i = j = 0; i < MAXOCLASSES; i++) {
	if (oclass_names[i]) {
	    map->submappings[j].symdef.rgbsym =
	      RGB_SYM(RGB_TRANSPARENT, oc_syms[i]);
	    map->submappings[j].symdef.description = oclass_names[i];
	    map->submappings[j].n_glyphs = 0;
	    for(k = 0; k < NUM_OBJECTS; k++)
		if (objects[k].oc_class == i)
		    map->submappings[j].n_glyphs++;
	    map->submappings[j].glyphs =
	      (struct proxycb_get_glyph_mapping_res_symdef *)
	      alloc(map->submappings[j].n_glyphs *
	      sizeof(*map->submappings[j].glyphs));
	    for(k = l = 0; k < NUM_OBJECTS; k++)
		if (objects[k].oc_class == i) {
		    map->submappings[j].glyphs[l].rgbsym =
		      RGB_SYM(rgb_colors[C(objects[k].oc_color)], oc_syms[i]);
		    map->submappings[j].glyphs[l].description =
		      obj_descr[k].oc_descr ?
		      obj_descr[k].oc_descr : obj_descr[k].oc_name;
		    l++;
		}
	    j++;
	}
    }
}

/* Defines for cmap groups, used for submappings. Cmaps in each group must be
 * contiguous (assumed below). */

#define IS_CMAP_WALL(cmap)	((cmap) && (cmap) <= S_trwall)
#define IS_CMAP_DOOR(cmap)	((cmap) >= S_ndoor && (cmap) <= S_hcdoor)
#define IS_CMAP_TRAP(cmap)	((cmap) >= MAXDCHARS && \
					(cmap) < MAXDCHARS + MAXTCHARS)
#define IS_CMAP_EFFECT(cmap)	((cmap) >= MAXDCHARS + MAXTCHARS && \
					(cmap) < MAXPCHARS - MAXEXPCHARS - 8)
#define IS_CMAP_SWALLOW(cmap)	((cmap) >= MAXPCHARS - MAXEXPCHARS - 8 && \
					(cmap) < MAXPCHARS - MAXEXPCHARS)

#define CMAP_WALL	0
#define CMAP_DOOR	1
#define CMAP_TRAP	2
#define CMAP_EFFECT	3
#define CMAP_SWALLOW	4
#define CMAP_NO_GROUP	5

#define CMAP_GROUP(cmap)	(IS_CMAP_WALL(cmap) ? CMAP_WALL : \
				IS_CMAP_DOOR(cmap) ? CMAP_DOOR : \
				IS_CMAP_TRAP(cmap) ? CMAP_TRAP : \
				IS_CMAP_EFFECT(cmap) ? CMAP_EFFECT : \
				IS_CMAP_SWALLOW(cmap) ? CMAP_SWALLOW : -1)

static char *cmap_groups[] = {
    "wall", "door", "trap", "effect", "swallow" };

static int
set_glyph_mapping_cmap_submapping(first, next, glyph)
int first, next, glyph;
{
    int i;
    for(i = first; i < next; i++)
	glyph2proxy[cmap_to_glyph(i)] = glyph++;
    return glyph;
}

static void
get_glyph_mapping_cmap_submapping(submap, desc, first, next)
struct proxycb_get_glyph_mapping_res_submapping *submap;
char *desc;
int first, next;
{
    int i;
    submap->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    submap->symdef.description = desc;
    submap->n_glyphs = next - first;
    submap->glyphs = (struct proxycb_get_glyph_mapping_res_symdef *)
      alloc(submap->n_glyphs * sizeof(*submap->glyphs));
    for(i = first; i < next; i++) {
	submap->glyphs[i - first].rgbsym =
	  RGB_SYM(rgb_colors[C(defsyms[i].color)], showsyms[i]);
	submap->glyphs[i - first].description = defsyms[i].explanation;
    }
}

static struct {
    int group, first_cmap, next_cmap;	/* first <= cmap < next */
} cmap_idx[CMAP_NO_GROUP];

static int
set_glyph_mapping_cmap(glyph)
int glyph;
{
    int i, j, k, l;
    for(i = 0; i < CMAP_NO_GROUP; i++)
	cmap_idx[i].group = -1;
    for(i = j = 0; i <= MAXPCHARS - MAXEXPCHARS; i++) {
	k = CMAP_GROUP(i);
	if (k >= 0) {
	    if (cmap_idx[j].group != k) {
		if (cmap_idx[j].group >= 0)
		    cmap_idx[j++].next_cmap = i;
		cmap_idx[j].group = k;
		cmap_idx[j].first_cmap = i;
	    }
	} else if (cmap_idx[j].group >= 0)
	    cmap_idx[j++].next_cmap = i;
    }
    for(i = 0; /* i <= CMAP_NO_GROUP */; i++) {
	k = i ? cmap_idx[i - 1].next_cmap : 0;
	l = i == CMAP_NO_GROUP ?
	  MAXPCHARS - MAXEXPCHARS : cmap_idx[i].first_cmap;
	if (l != k)
	    glyph = set_glyph_mapping_cmap_submapping(k, l, glyph);
	if (i == CMAP_NO_GROUP)
	    break;
	glyph = set_glyph_mapping_cmap_submapping(cmap_idx[i].first_cmap,
	  cmap_idx[i].next_cmap, glyph);
    }
    return glyph;
}

static void
get_glyph_mapping_cmap(map)
struct proxycb_get_glyph_mapping_res_mapping *map;
{
    int i, j, k, l;
    map->flags = "";
    map->base_mapping = -1;
    map->alt_glyph = NO_GLYPH;
    map->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    map->symdef.description = "cmap";
    map->n_submappings = CMAP_NO_GROUP;
    if (cmap_idx[0].first_cmap)
	map->n_submappings++;
    for(i = 1; i < CMAP_NO_GROUP; i++)
	if (cmap_idx[i].first_cmap != cmap_idx[i - 1].next_cmap)
	    map->n_submappings++;
    if (cmap_idx[CMAP_NO_GROUP - 1].next_cmap != MAXPCHARS - MAXEXPCHARS)
	map->n_submappings++;
    map->submappings = (struct proxycb_get_glyph_mapping_res_submapping *)
      alloc(map->n_submappings * sizeof(*map->submappings));
    for(i = j = 0; /* i <= CMAP_NO_GROUP */; i++) {
	k = i ? cmap_idx[i - 1].next_cmap : 0;
	l = i == CMAP_NO_GROUP ?
	  MAXPCHARS - MAXEXPCHARS : cmap_idx[i].first_cmap;
	if (l != k) {
	    get_glyph_mapping_cmap_submapping(map->submappings + j, "", k, l);
	    j++;
	}
	if (i == CMAP_NO_GROUP)
	    break;
	get_glyph_mapping_cmap_submapping(map->submappings + j,
	  cmap_groups[cmap_idx[i].group], cmap_idx[i].first_cmap,
	  cmap_idx[i].next_cmap);
	j++;
    }
}

static char *explosion_types[] = {	/* hack.h, see also tilemap.c */
    "dark", "noxious", "muddy", "wet", "magical", "fiery", "frosty"
};

static int
set_glyph_mapping_explosions(glyph)
int glyph;
{
    int i, j;
    for(i = 0; i < EXPL_MAX; i++)
	for(j = 0; j < MAXEXPCHARS; j++)
	    glyph2proxy[explosion_to_glyph(i, j + S_explode1)] = glyph++;
    return glyph;
}

static void
get_glyph_mapping_explosions(map, base)
struct proxycb_get_glyph_mapping_res_mapping *map;
int base;
{
    int i, j;
    extern int explcolors[];
    map->flags = explosion_types[0];
    map->base_mapping = -1;
    map->alt_glyph = NO_GLYPH;
    map->symdef.rgbsym = RGB_SYM(rgb_colors[C(explcolors[0])], 0);
    map->symdef.description = "explosion";
    map->n_submappings = 1;
    map->submappings = (struct proxycb_get_glyph_mapping_res_submapping *)
      alloc(map->n_submappings * sizeof(*map->submappings));
    map->submappings->symdef.rgbsym = RGB_SYM(rgb_colors[C(explcolors[0])], 0);
    map->submappings->symdef.description = "";
    map->submappings->n_glyphs = MAXEXPCHARS;
    map->submappings->glyphs = (struct proxycb_get_glyph_mapping_res_symdef *)
      alloc(map->submappings->n_glyphs * sizeof(*map->submappings->glyphs));
    for(i = 0; i < MAXEXPCHARS; i++) {
	map->submappings->glyphs[i].rgbsym =
	  RGB_SYM(rgb_colors[C(explcolors[0])], showsyms[i + S_explode1]);
	map->submappings->glyphs[i].description =
	  defsyms[i + S_explode1].explanation;
    }
    for(i = 1; i < EXPL_MAX; i++) {
	map++;
	map->flags = explosion_types[i];
	map->base_mapping = base;
	map->alt_glyph = NO_GLYPH;
	map->symdef.rgbsym = RGB_SYM(rgb_colors[C(explcolors[i])], 0);
	map->symdef.description = "";
	map->n_submappings = 0;
	map->submappings = NULL;
    }
}

static char *zap_types[] = {	/* spell.h */
    "magic missile", "fire", "cold", "sleep", "death", "lightning",
    "poison gas", "acid"
};

static int
set_glyph_mapping_zaps(glyph)
int glyph;
{
    int i, j;
    for(i = 0; i < NUM_ZAP; i++)
	for(j = 0; j < 4; j++)
	    glyph2proxy[i * 4 + j + GLYPH_ZAP_OFF] = glyph++;
    return glyph;
}

static void
get_glyph_mapping_zaps(map, base)
struct proxycb_get_glyph_mapping_res_mapping *map;
int base;
{
    int i, j;
    map->flags = zap_types[0];
    map->base_mapping = -1;
    map->alt_glyph = NO_GLYPH;
    map->symdef.rgbsym = RGB_SYM(rgb_colors[C(zapcolors[0])], 0);
    map->symdef.description = "zap";
    map->n_submappings = 1;
    map->submappings = (struct proxycb_get_glyph_mapping_res_submapping *)
      alloc(map->n_submappings * sizeof(*map->submappings));
    map->submappings->symdef.rgbsym = RGB_SYM(rgb_colors[C(zapcolors[0])], 0);
    map->submappings->symdef.description = "";
    map->submappings->n_glyphs = 4;
    map->submappings->glyphs = (struct proxycb_get_glyph_mapping_res_symdef *)
      alloc(map->submappings->n_glyphs * sizeof(*map->submappings->glyphs));
    for(i = 0; i < 4; i++) {
	map->submappings->glyphs[i].rgbsym =
	  RGB_SYM(rgb_colors[C(zapcolors[0])], showsyms[i + S_vbeam]);
	map->submappings->glyphs[i].description =
	  defsyms[i + S_vbeam].explanation;
    }
    for(i = 1; i < NUM_ZAP; i++) {
	map++;
	map->flags = zap_types[i];
	map->base_mapping = base;
	map->alt_glyph = NO_GLYPH;
	map->symdef.rgbsym = RGB_SYM(rgb_colors[C(zapcolors[i])], 0);
	map->symdef.description = "";
	map->n_submappings = 0;
	map->submappings = NULL;
    }
}

static struct {
    int cmap;
    const char *description;
} swallow_cmaps[] = {
    S_sw_tl,        "top left",
    S_sw_tc,        "top center",
    S_sw_tr,        "top right",
    S_sw_ml,        "middle left",
    S_sw_mr,        "middle right",
    S_sw_bl,        "bottom left",
    S_sw_bc,        "bottom center",
    S_sw_br,        "bottom right"
};

static int
set_glyph_mapping_swallow(base_glyphs, glyph)
int base_glyphs, glyph;
{
    int i, j;
    for(i = 0; i < SIZE(swallow_cmaps); i++)
	for(j = 0; j < base_glyphs; j++)
	    glyph2proxy[j * SIZE(swallow_cmaps) + i + GLYPH_SWALLOW_OFF] =
	      glyph2proxy[monnum_to_glyph(j)] * SIZE(swallow_cmaps) + i + glyph;
    return glyph + SIZE(swallow_cmaps) * base_glyphs;
}

static void
get_glyph_mapping_swallow(map, base)
struct proxycb_get_glyph_mapping_res_mapping *map;
int base;
{
    int i;
    map->flags = "";
    map->base_mapping = base;
    map->alt_glyph = NO_GLYPH;
    map->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    map->symdef.description = "swallow";
    map->n_submappings = 1;
    map->submappings = (struct proxycb_get_glyph_mapping_res_submapping *)
      alloc(map->n_submappings * sizeof(*map->submappings));
    map->submappings->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    map->submappings->symdef.description = "";
    map->submappings->n_glyphs = SIZE(swallow_cmaps);
    map->submappings->glyphs = (struct proxycb_get_glyph_mapping_res_symdef *)
      alloc(map->submappings->n_glyphs * sizeof(*map->submappings->glyphs));
    for(i = 0; i < SIZE(swallow_cmaps); i++) {
	map->submappings->glyphs[i].rgbsym =
	  RGB_SYM(RGB_TRANSPARENT, showsyms[swallow_cmaps[i].cmap]);
	map->submappings->glyphs[i].description = swallow_cmaps[i].description;
    }
}

static int
set_glyph_mapping_warning(glyph)
int glyph;
{
    int i;
    for(i = 0; i < WARNCOUNT; i++)
	glyph2proxy[warning_to_glyph(i)] = glyph++;
    return glyph;
}

static void
get_glyph_mapping_warning(map)
struct proxycb_get_glyph_mapping_res_mapping *map;
{
    int i;
    map->flags = "";
    map->base_mapping = -1;
    map->alt_glyph = NO_GLYPH;
    map->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    map->symdef.description = "warning";
    map->n_submappings = 1;
    map->submappings = (struct proxycb_get_glyph_mapping_res_submapping *)
      alloc(map->n_submappings * sizeof(*map->submappings));
    map->submappings->symdef.rgbsym = RGB_SYM(RGB_TRANSPARENT, 0);
    map->submappings->symdef.description = "";
    map->submappings->n_glyphs = WARNCOUNT;
    map->submappings->glyphs = (struct proxycb_get_glyph_mapping_res_symdef *)
      alloc(map->submappings->n_glyphs * sizeof(*map->submappings->glyphs));
    for(i = 0; i < WARNCOUNT; i++) {
	map->submappings->glyphs[i].rgbsym =
	  RGB_SYM(rgb_colors[C(def_warnsyms[i].color)], warnsyms[i]);
	map->submappings->glyphs[i].description = def_warnsyms[i].explanation;
    }
}

#ifdef DEBUG
static void
print_glyph_mapping(fp, mapping)
FILE *fp;
struct proxycb_get_glyph_mapping_res *mapping;
{
    int i, j, k;
    struct proxycb_get_glyph_mapping_res_mapping *m;
    struct proxycb_get_glyph_mapping_res_submapping *s;
    struct proxycb_get_glyph_mapping_res_symdef *g;
    fprintf(fp,"%s", "Glyph mapping:\n");
    fprintf(fp, "No. glyphs: %d\n", mapping->no_glyph);
    fprintf(fp, "Transparent RGBsym: 0x%lX\n", mapping->transparent);
    fprintf(fp, "No. mappings: %d\n", mapping->n_mappings);
    fprintf(fp,"%s", "Mappings:\n");
    for(i = 0; i < mapping->n_mappings; i++) {
	m = mapping->mappings + i;
	fprintf(fp, "%-3d Flags: %s\n", i, m->flags);
	fprintf(fp, "    Base mapping: %d\n", m->base_mapping);
	fprintf(fp, "    Alt. glyph: %d\n", m->alt_glyph);
	fprintf(fp, "    RGBsym: 0x%lX\n", m->symdef.rgbsym);
	fprintf(fp, "    Description: %s\n", m->symdef.description);
	fprintf(fp, "    No. sub mappings: %d\n", m->n_submappings);
	fprintf(fp,"%s", "    Sub mappings:\n");
	for(j = 0; j < m->n_submappings; j++) {
	    s = m->submappings + j;
	    fprintf(fp, "    %-3d RGBsym: 0x%lX\n", j, s->symdef.rgbsym);
	    fprintf(fp, "        Description: %s\n", s->symdef.description);
	    fprintf(fp, "        No. glyphs: %d\n", s->n_glyphs);
	    fprintf(fp,"%s", "        Glyphs:\n");
	    for(k = 0; k < s->n_glyphs; k++) {
		g = s->glyphs + k;
		fprintf(fp, "        %-3d RGBsym: 0x%lX\n", k, g->rgbsym);
		fprintf(fp, "            Description: %s\n", g->description);
	    }
	}
    }
}
#endif

void
set_glyph_mapping()
{
    int glyph;
    int monster_glyphs;
#ifdef DEBUG
    for(glyph = 0; glyph < NO_GLYPH; glyph++)
	glyph2proxy[glyph] = -1;
#endif
    /* Monsters */
    /* Warning: There are assumptions all through this module that the
     * base monsters start at glyph 0
     */
    monster_glyphs = set_glyph_mapping_monsters(0);
    glyph = monster_glyphs;
    glyph = set_glyph_mapping_monsters_based(monster_glyphs, glyph,
      GLYPH_PET_OFF);
    glyph = set_glyph_mapping_invisible_monster(glyph);
    glyph = set_glyph_mapping_monsters_based(monster_glyphs, glyph,
      GLYPH_DETECT_OFF);
    glyph = set_glyph_mapping_monster_corpses(monster_glyphs, glyph);
    glyph = set_glyph_mapping_monsters_based(monster_glyphs, glyph,
      GLYPH_RIDDEN_OFF);
    /* Objects */
    glyph = set_glyph_mapping_objects(glyph);
    /* Others */
    glyph = set_glyph_mapping_cmap(glyph);
    glyph = set_glyph_mapping_explosions(glyph);
    glyph = set_glyph_mapping_zaps(glyph);
    glyph = set_glyph_mapping_swallow(monster_glyphs, glyph);
    glyph = set_glyph_mapping_warning(glyph);
    if (glyph != NO_GLYPH)
	panic("Glyph count wrong in mapping (%d != %d)", glyph, NO_GLYPH);
#ifdef DEBUG
    for(glyph = 0; glyph < NO_GLYPH; glyph++)
	if (glyph2proxy[glyph] < 0 || glyph2proxy[glyph] >= NO_GLYPH)
	    panic("Incomplete glyph mapping; internal glyph %d missing", glyph);
#endif
}

struct proxycb_get_glyph_mapping_res *
get_glyph_mapping()
{
    static struct proxycb_get_glyph_mapping_res mapping;
    mapping.no_glyph = NO_GLYPH;
    mapping.transparent = RGB_SYM(RGB_TRANSPARENT, 0);
    mapping.n_mappings = 8 + EXPL_MAX + NUM_ZAP + 2;
    mapping.mappings = (struct proxycb_get_glyph_mapping_res_mapping *)
      alloc(mapping.n_mappings * sizeof(*mapping.mappings));
    /* Monsters */
    get_glyph_mapping_monsters(mapping.mappings + 0);
    get_glyph_mapping_monsters_based(mapping.mappings + 1, 0,
      GLYPH_PET_OFF);
    get_glyph_mapping_invisible_monster(mapping.mappings + 2);
    get_glyph_mapping_monsters_based(mapping.mappings + 3, 0,
      GLYPH_DETECT_OFF);
    get_glyph_mapping_monster_corpses(mapping.mappings + 4, 0);
    get_glyph_mapping_monsters_based(mapping.mappings + 5, 0,
      GLYPH_RIDDEN_OFF);
    /* Objects */
    get_glyph_mapping_objects(mapping.mappings + 6);
    /* Others */
    get_glyph_mapping_cmap(mapping.mappings + 7);
    get_glyph_mapping_explosions(mapping.mappings + 8, 8);
    get_glyph_mapping_zaps(mapping.mappings + 8 + EXPL_MAX, 8 + EXPL_MAX);
    get_glyph_mapping_swallow(mapping.mappings + 8 + EXPL_MAX + NUM_ZAP, 0);
    get_glyph_mapping_warning(mapping.mappings + 8 + EXPL_MAX + NUM_ZAP + 1);
#ifdef DEBUG
    {
	FILE *fp;
	fp = fopen("/tmp/glyph.map", "w");
	print_glyph_mapping(fp, &mapping);
	fclose(fp);
    }
#endif
    return &mapping;
}

void
free_glyph_mapping(mapping)
struct proxycb_get_glyph_mapping_res *mapping;
{
    int i, j;
    for(i = 0; i < mapping->n_mappings; i++) {
	for(j = 0; j < mapping->mappings[i].n_submappings; j++)
	    if (mapping->mappings[i].submappings[j].n_glyphs)
		free(mapping->mappings[i].submappings[j].glyphs);
	if (mapping->mappings[i].n_submappings)
	    free(mapping->mappings[i].submappings);
    }
    if (mapping->n_mappings)
	free(mapping->mappings);
}

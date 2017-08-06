/*
  $Id: gtkmap.c,v 1.36.2.2 2004-11-12 20:44:47 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2000-2004
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include "winGTK.h"
#include "dlb.h"
#include "patchlevel.h"
#include "decl.h"
#include "proxycb.h"
#include "prxyclnt.h"
#include "gtkprogress.h"

#undef red
#undef green
#undef blue

/*
  if map_click is true, we do gtk_main_quit() when clicking map
 */
static int map_click;
static int map_update;

static GtkWidget *map;
static GdkFont *map_font;
static gchar *map_font_name;
static unsigned char *map_xoffsets;		/* For character mode only */
static unsigned int map_font_width;		/* Maximum width */
static enum xshm_map_mode map_mode;
#ifdef GTK_PROXY
int no_glyph;
static short *map_glyph2colsym = (short *)0;
#endif

static GdkGC *map_color_gc[N_NH_COLORS];

static gint map_button_event(void *map, GdkEventButton *event, gpointer data);
static void nh_map_cliparound(int x, int y, gboolean exact);
static void nh_map_redraw();
static int configure_map(GtkWidget *w, gpointer data);

#ifdef	WINGTK_RADAR
static int use_radar = TRUE;
static int radar_is_created;

static GtkWidget *radar;
static GtkWidget *radar_darea;
/*
 * we use 2 pixmaps for radar.
 *
 * radar_pixmap keeps radar dot
 * radar_pixmap2 keeps radar dot and frame rectangle
 */
static GdkPixmap *radar_pixmap;
static GdkPixmap *radar_pixmap2;
extern GtkAccelGroup *accel_group;

#define RADAR_MONSTER	CLR_YELLOW
#define RADAR_HUMAN	MAP_WHITE
#define RADAR_PET	CLR_GREEN
#define RADAR_OBJECT	CLR_BLUE

#define RADAR_WALL	CLR_GRAY
#define RADAR_FLOOR	MAP_DARK_GREEN
#define RADAR_DOOR	CLR_ORANGE
#define RADAR_LADDER	CLR_MAGENTA
#define RADAR_WATER	CLR_BLUE
#define RADAR_TRAP	CLR_RED
#define RADAR_SWALLOW	CLR_RED
#define RADAR_ICE	CLR_GRAY
#define RADAR_LAVA	CLR_ORANGE
#define RADAR_BRIDGE	CLR_GRAY
#define RADAR_AIR	CLR_CYAN
#define RADAR_CLOUD	CLR_GRAY
#define RADAR_BEAM	CLR_YELLOW

static void		nh_radar_configure();
#endif	/* WINGTK_RADAR */

static GdkGC *map_gc;

/*
 *        BIG3DTILE
 *
 *                 +----------+
 * ^              /          /| 
 * |             /          / | 
 * |            /          /  | 
 * |           /          /   | 
 * *6         /          /    |
 * |         +----------+     +
 * |       ^ |          |    / ^
 * |       | |          |   /  |
 * |      *2 |          |  /   *4
 * |       | |          | /    |
 * v       v |          |/     v
 *           +----------+      
 *            <-- *1 --> <-*3->
 *            <-- *5        -->
 *
 * *1 = 3D_WIDTH	= 32
 * *2 = 3D_HEIGHT 	= 32
 * *3 = 3D_OFSET	= 16
 * *4 = 3D_OFSETY	= 32
 * *5 = WIDTH		= 48
 * *6 = HEIGHT		= 64
 */

TileTab tileTab[MAXNOTILESETS+1] = {
    { "", "" },		/* dummy */
};
int no_tileTab = 0;	/* Not including dummy (index 0) */

static TileTab *Tile;
int map_visual = -1;
int map_clip_dist2 = 0;

/* from tile.c */
extern int tiles_per_row;
extern int tiles_per_col;

#ifdef WINGTK_RADAR
#define NH_RADAR_UNIT	4
#define NH_RADAR_WIDTH	(no_cols * NH_RADAR_UNIT)
#define NH_RADAR_HEIGHT	(no_rows * NH_RADAR_UNIT)
#endif

#ifdef GTK_PROXY
extern short *GTK_glyph2tile;
#define	glyph2tile GTK_glyph2tile
#else
extern short glyph2tile[];
#endif
#define GLYPH2TILE(g)	((g) == NO_GLYPH ? stone_tile : glyph2tile[g])

extern GtkWidget *main_window;

int cursx;
int cursy;
int cursm;

static int c_width;
static int c_height;

static int c_3dwidth;
static int c_3dheight;
static int c_3dofset;

static int c_map_width;
static int c_map_height;

#define TILEMAP_UPDATE	1

static int no_rows = -1, no_cols = -1, no_layers = -1;
static int tilemap_size = 0;	/* sizeof(struct tilemap) */

struct tilemap {
    unsigned int flags;
    int	glyphs[1];
};

static struct tilemap **gtkmap = NULL;

#define GTKMAP_PTR(ptr, col, size)	\
				((struct tilemap *)((char *)(ptr)+(col)*(size)))
#define GTKMAP(row, col)	GTKMAP_PTR(gtkmap[row], col, tilemap_size)

/*
 * fix tile number
 */
static int
fix_tile(int tile)
{
    return tile;
}

/*
 * (Re)configure the map for new dimensions (layers = -1 to free).
 */

static int
nh_conf_map_dimens(int rows, int cols, int layers, int preserve)
{
    int i, j, k;
    int glyph = cmap_to_glyph(S_stone);
    int retval = 0;
    int new_size = sizeof(struct tilemap) + (layers - 1) * sizeof(int);
    struct tilemap **new, *tm;
    if (rows == no_rows && cols == no_cols && layers == no_layers)
	return 0;
    if (layers >= 0) {
	/* Allocate new map */
	new = malloc(rows * sizeof(struct tilemap *));
	if (!new) {
	    retval = -1;
	    goto out;
	}
	for(j = 0; j < rows; j++) {
	    new[j] = malloc(cols * new_size);
	    if (!new[j]) {
		retval = -1;
		for(j--; j >= 0; j--)
		    free(new[j]);
		free(new);
		goto out;
	    }
	}
	/* Copy data from old map to new and initialize new areas */
	if (gtkmap && preserve)
	    for(j = 0; j < min(rows, no_rows); j++) {
		if (gtkmap[j])
		    for(i = 0; i < min(cols, no_cols); i++) {
			tm = GTKMAP_PTR(new[j], i, new_size);
			tm->flags = GTKMAP(j, i)->flags;
			for(k = 0; k < min(layers, no_layers); k++)
			    tm->glyphs[k] = GTKMAP(j, i)->glyphs[k];
			for(; k < layers; k++)
			    tm->glyphs[k] = NO_GLYPH;
		    }
		else
		    i = 0;
		for(; i < cols; i++) {
		    tm = GTKMAP_PTR(new[j], i, new_size);
		    tm->flags = TILEMAP_UPDATE;
		    for(k = 0; k < layers; k++)
			tm->glyphs[k] = NO_GLYPH;
		}
	    }
	else
	    j = 0;
	for(; j < rows; j++)
	    for(i = 0; i < cols; i++) {
		tm = GTKMAP_PTR(new[j], i, new_size);
		tm->flags = TILEMAP_UPDATE;
		for(k = 0; k < layers; k++)
		    tm->glyphs[k] = NO_GLYPH;
	    }
	/* Release old map */
	if (gtkmap) {
	    for(j = 0; j < no_rows; j++)
		free(gtkmap[j]);
	    free(gtkmap);
	}
    }
out:
    if (layers < 0 || retval) {
	for(j = 0; j < rows; j++)
	    free(gtkmap[j]);
	gtkmap = NULL;
	no_layers = -1;
	no_rows = -1;
	no_cols = -1;
	tilemap_size = 0;
    } else {
	gtkmap = new;
	map_update = 1;
	no_layers = layers;
	no_rows = rows;
	no_cols = cols;
	tilemap_size = new_size;
	if (map_visual)
	    map = xshm_map_init(map_mode, c_map_width, c_map_height);
	if (GTK_WIDGET_REALIZED(map))
	    configure_map(map, 0);
    }
#ifdef WINGTK_RADAR
    nh_radar_configure();
#endif
    return retval;
}

static int
nh_conf_map_font(void)
{
    int i, j, min_width, width;
#ifdef GTK_PROXY
    int rgb, sym, best;
    double e, err = 0;
    int r, g, b;
    double d;
    double X, Y, Z, up, vp;	/* Tri-stimulus and CIE-1976 UCS values */
    double Ls, us, vs;
    struct { double L, up, vp; } nh_Luv[N_NH_COLORS];
    long *glyph2rgbsym;
    struct proxycb_get_glyph_mapping_res *glyph_map;
#endif
    if (!map_font) {
	if (map_font_name) {
	    PangoFontDescription *desc;
	    desc = pango_font_description_from_string(map_font_name);
	    map_font = gdk_font_from_description(desc);
	    pango_font_description_free(desc);
	    if (!map_font)
		g_warning("Can't load map font \"%s\"", map_font_name);
	}
	if (!map_font) {
	    g_return_val_if_fail(map->style != NULL, 1);
	    map_font = gtk_style_get_font(map->style);
	    g_return_val_if_fail(map_font != NULL, 1);
	    gdk_font_ref(map_font);
	    map_font_name =
	      pango_font_description_to_string(map->style->font_desc);
	}
    }

    /*
     * ALI
     * We might want to consider making the size of this array
     * variable in the future, but for now 8 bits is always enough.
     */
    map_xoffsets = (unsigned char *) alloc(256);
    for(i = 0; i < 256; i++)
	map_xoffsets[i] = 0;
#ifdef GTK_PROXY
    if (!map_glyph2colsym) {
	glyph_map = nh_proxy_cache_get_glyph_mapping(NULL);
	if (!glyph_map) {
	    pline("Cannot get glyph mapping.");
	    return 0;
	}
	glyph2rgbsym = proxy_map_glyph2char(glyph_map);
	no_glyph = glyph_map->no_glyph;
	proxy_cb_free_glyph_mapping(glyph_map);
	map_glyph2colsym = (short *)alloc(no_glyph * sizeof(short));
	for(i = 0; i < no_glyph; i++)
	    map_glyph2colsym[i] = -1;
	/*
	 * The algorithm for determining colour closeness used here is
	 * a sledgehammer to crack a nut. It is based on the CIE 1976
	 * colour difference value (delta-E*(L*u*v*)), but modified
	 * slightly to give much more weight to the chroma information.
	 * Normally one would simply use (r-r0)^2 + (g-g0)^2 + (b-b0)^2
	 * but this gives too much weight to lightness for our purposes
	 * and while one can conceive of a number of ways to modify it
	 * we really need to do some testing before using one of these.
	 * This algorithm will work, although it's a little slow.
	 *
	 * Note that the values in nh_colors are in 16-bit, but are
	 * specified as 8-bit * 257 so we can simply divide by 257
	 * to get 8-bit.
	 */
	for(i = 0; i < N_NH_COLORS; i++) {
	    r = nh_color[i].red/257;
	    g = nh_color[i].green/257;
	    b = nh_color[i].blue/257;
	    if (r | g | b) {
		X = 0.412453 * r + 0.357580 * g + 0.180423 * b;
		Y = 0.212671 * r + 0.715160 * g + 0.072169 * b;
		Z = 0.019334 * r + 0.119193 * g + 0.950227 * b;
		nh_Luv[i].L = Y;
		d = X + 15 * Y + 3 * Z;
		nh_Luv[i].up = 4 * X / d;
		nh_Luv[i].vp = 9 * Y / d;
	    } else {
		nh_Luv[i].L = 0.5;	/* Zero would imply infinite ratio */
		nh_Luv[i].up = nh_Luv[i].vp = 0;
	    }
	}
	for(i = 0; i < no_glyph; i++) {
	    if (map_glyph2colsym[i] == -1) {
		rgb = RGBSYM_RGB(glyph2rgbsym[i]);
		r = rgb>>16;
		g = (rgb>>8)&0xFF;
		b = rgb&0xFF;
		if (r | g | b) {
		    X = 0.412453 * r + 0.357580 * g + 0.180423 * b;
		    Y = 0.212671 * r + 0.715160 * g + 0.072169 * b;
		    Z = 0.019334 * r + 0.119193 * g + 0.950227 * b;
		    d = X + 15 * Y + 3 * Z;
		    up = 4 * X / d;
		    vp = 9 * Y / d;
		} else {
		    Y = 0.5;
		    up = vp = 0;
		}
		best = 0;
		for(j = 0; j < N_NH_COLORS; j++) {
		    Ls = 116 * pow(nh_Luv[j].L / Y, 1/3.) - 16;
		    if (nh_Luv[j].up || up) {
			/* These values are ten times the normal CIE 1976
			 * u* and v* values. (Chroma is much more important
			 * in NetHack than lightness.)
			 */
			us = 130 * Ls * (nh_Luv[j].up - up);
			vs = 130 * Ls * (nh_Luv[j].vp - vp);
		    } else
			us = vs = 0;
		    e = Ls * Ls + us * us + vs * vs;
#ifdef DEBUG
		    fprintf(stderr, "E[(%d,%d,%d), (%d,%d,%d)] = %lg\n",
		      nh_color[j].red/257, nh_color[j].green/257,
		      nh_color[j].blue/257, r, g, b, e);
#endif
		    if (!j || e < err) {
			best = j;
			err = e;
		    }
		}
		map_glyph2colsym[i] = best << 8 | RGBSYM_SYM(glyph2rgbsym[i]);
		for(j = i + 1; j < no_glyph; j++)
		    if (RGBSYM_RGB(glyph2rgbsym[j]) == rgb)
			map_glyph2colsym[j] =
			  best << 8 | RGBSYM_SYM(glyph2rgbsym[j]);
	    }
#ifdef DEBUG
	    fprintf(stderr,"Glyph %d (0x%lX) -> sym %d, colour %d\n",
	      i, glyph2rgbsym[i],
	      map_glyph2colsym[i] & 0xFF, map_glyph2colsym[i] >> 8);
#endif
	}
	free(glyph2rgbsym);
    }
    sym = map_glyph2colsym[0] & 0xFF;
    map_font_width = min_width = gdk_char_width_wc(map_font, (GdkWChar)sym);
    if (c_width > 0)
	map_xoffsets[sym] = c_width;
    for(i = 1; i < no_glyph; i++) {
	sym = map_glyph2colsym[i] & 0xFF;
	width = gdk_char_width_wc(map_font, (GdkWChar)sym);
	if (width > 0)
	    map_xoffsets[sym] = width;
	if (width < min_width)
	    min_width = width;
	if (width > map_font_width)
	    map_font_width = width;
    }
#else
    map_font_width = min_width =
      gdk_char_width_wc(map_font, (GdkWChar)oc_syms[0]);
    if (c_width > 0)
	map_xoffsets[oc_syms[0]] = c_width;
    for(i = 1; i < SIZE(oc_syms); i++) {
	width = gdk_char_width_wc(map_font, (GdkWChar)oc_syms[i]);
	if (width > 0)
	    map_xoffsets[oc_syms[i]] = width;
	if (width < min_width)
	    min_width = width;
	if (width > map_font_width)
	    map_font_width = width;
    }
    for(i = 0; i < SIZE(showsyms); i++) {
	width = gdk_char_width_wc(map_font, (GdkWChar)showsyms[i]);
	if (width > 0)
	    map_xoffsets[showsyms[i]] = width;
	if (width < min_width)
	    min_width = width;
	if (width > map_font_width)
	    map_font_width = width;
    }
    for(i = 0; i < SIZE(monsyms); i++) {
	width = gdk_char_width_wc(map_font, (GdkWChar)monsyms[i]);
	if (width > 0)
	    map_xoffsets[monsyms[i]] = width;
	if (width < min_width)
	    min_width = width;
	if (width > map_font_width)
	    map_font_width = width;
    }
    for(i = 0; i < SIZE(warnsyms); i++) {
	width = gdk_char_width_wc(map_font, (GdkWChar)warnsyms[i]);
	if (width > 0)
	    map_xoffsets[warnsyms[i]] = width;
	if (width < min_width)
	    min_width = width;
	if (width > map_font_width)
	    map_font_width = width;
    }
#endif /* GTK_PROXY */
    if (min_width <= 0)
	pline("Warning: Not all expected glyphs present in map font \"%s\".",
	  map_font_name);

    /* Convert widths to offsets */
    for(i = 0; i < 256; i++)
	if (map_xoffsets[i])
	    map_xoffsets[i] = (map_font_width - map_xoffsets[i]) / 2;
    return 1;
}

/*
 * -1:   deallocate
 * zero: character
 * else: various tiles
 */
int
nh_set_map_visual(int mode)
{
    static int setting_visual = FALSE;		/* Ignore recursive calls */
    int saved_vis = map_visual;
    gchar *buf;
    GdkCursor *cursor;
    GtkWidget *progress;

    if (setting_visual)
	return 0;
    setting_visual++;

    if (mode < -1 || mode > no_tileTab)
	panic("Bad visual!\n");

    if (saved_vis != mode) {
	if (map)
	    gtk_widget_hide(map);

	if (saved_vis > 0)
	    x_tile_destroy();
switch_mode:
	if (mode < 0) {
	    map_visual = mode;
	    setting_visual--;
	    return 1;
	} else if (mode != 0) {      /* mode 0 is handled in configure_map() */
	    buf = g_strdup_printf("Selecting %s", tileTab[mode].ident);
	    progress = nh_gtk_progress_window_new(buf, GTK_WINDOW(main_window));
	    g_free(buf);
	    if (tileTab[mode].ident[0]) {
		Tile = tileTab + mode;
		x_tile_init_add_stages(Tile, NH_GTK_PROGRESS_WINDOW(progress));
	    }
	    gtk_widget_realize(progress);
#if GTK_CHECK_VERSION(2,2,0)
	    cursor = gdk_cursor_new_for_display(
	      gdk_drawable_get_display(progress->window), GDK_WATCH);
#else
	    cursor = gdk_cursor_new(GDK_WATCH);
#endif
	    gdk_window_set_cursor(progress->window, cursor);
	    gdk_cursor_unref(cursor);
	    gtk_widget_show_now(progress);
	    if (!tileTab[mode].ident[0])
		map_mode = XSHM_MAP_NONE;
	    else {
		map_mode = x_tile_init(Tile, NH_GTK_PROGRESS_WINDOW(progress));
		while(gtk_events_pending())
		    gtk_main_iteration();
	    }

	    if (map_mode != XSHM_MAP_NONE) {
		c_width = Tile->unit_width;
		c_height = Tile->unit_height;
		c_3dwidth = Tile->unit_width - Tile->ofsetx_3d;
		c_3dheight = Tile->unit_height - Tile->ofsety_3d;
		c_3dofset = Tile->ofsetx_3d;
		c_map_width = c_3dwidth * (no_cols - 1) + c_3dofset * no_rows +
		  Tile->unit_width;
		c_map_height = c_3dheight * (no_rows - 1) + Tile->unit_height;
	    } else {
		if (saved_vis > 0 && tileTab[saved_vis].ident || !saved_vis) {
		    gtk_widget_destroy(progress);
		    pline("Warning: Switching back to %s.",
		      saved_vis?tileTab[saved_vis].ident:"character mode");
		    mode = saved_vis;
		    saved_vis = -1;
		    goto switch_mode;
		} else if (map_visual > 0) {
		    gtk_widget_destroy(progress);
		    panic("Failed to switch back to previous mode.");
		} else {
		    setting_visual--;
		    gtk_widget_destroy(progress);
		    return 0;
		}
	    }

	    map = xshm_map_init(map_mode, c_map_width, c_map_height);
	    while(gtk_events_pending())
		gtk_main_iteration();
	    gtk_widget_destroy(progress);
	}

	map_visual = mode;
	if (GTK_WIDGET_REALIZED(map))
	    configure_map(map, 0);
	nh_map_check_visibility();
#ifdef WINGTK_RADAR
	nh_radar_update();
#endif
	nh_option_cache_set("tileset", tileTab[map_visual].ident);
	nh_map_redraw();
	nh_map_flush();

	gtk_widget_show(map);
    }

    setting_visual--;
    return 1;
}

int
nh_get_map_visual(void)
{
    return map_visual;
}

int
nh_check_map_visual(int mode)
{
    if (mode < 0 || mode > no_tileTab || mode && !tileTab[mode].ident[0])
	return -1;
    else
	return 0;
}

int
nh_set_map_font(gchar *name)
{
    PangoFontDescription *desc;
    g_return_if_fail(name != NULL);
    if (map_font_name) {
	if (!strcmp(name, map_font_name))
	    return 0;
	else
	    g_free(map_font_name);
    }
    map_font_name = g_strdup(name);
    if (map_font) {
	gdk_font_unref(map_font);
	desc = pango_font_description_from_string(name);
	map_font = gdk_font_from_description(desc);
	pango_font_description_free(desc);
	if (map_visual >= 0) {
	    configure_map(map, 0);
	    nh_map_redraw();
	}
    }
    return 0;
}

gchar *
nh_get_map_font(void)
{
    if (map_font_name)
	return g_strdup(map_font_name);
    else if (map && map->style && map->style->font_desc)
	return pango_font_description_to_string(map->style->font_desc);
    else
	return NULL;
}

#ifdef WINGTK_RADAR
static gint
radar_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    gdk_draw_pixmap(widget->window,
      widget->style->fg_gc[GTK_WIDGET_STATE(widget)], radar_pixmap2,
      event->area.x, event->area.y, event->area.x, event->area.y,
      event->area.width, event->area.height);

    return FALSE;
}

static gint
radar_configure_event(GtkWidget *widget, GdkEventConfigure *event,
  gpointer data)
{
    /*
     * The drawing area has changed size (or is receiving its initial
     * size). This always happens under program control.
     * We need to (re-)create the pixmaps to match.
     */

    GdkPixmap *pixmap;
    GdkGC *gc;
    gint w, h;

    if (radar_pixmap2)
	gdk_pixmap_unref(radar_pixmap2);

    radar_pixmap2 = gdk_pixmap_new(widget->window,
      event->width, event->height, -1);
    gc = gdk_gc_new(radar_pixmap2);
    gdk_gc_set_foreground(gc, &nh_color[CLR_BLACK]);
    gdk_draw_rectangle(radar_pixmap2, gc, TRUE, 0, 0, event->width,
      event->height);
    gdk_gc_unref(gc);
    pixmap = gdk_pixmap_new(widget->window, event->width, event->height, -1);
    gc = gdk_gc_new(pixmap);
    gdk_gc_set_foreground(gc, &nh_color[CLR_BLACK]);
    gdk_draw_rectangle(pixmap, gc, TRUE, 0, 0, event->width, event->height);
    if (radar_pixmap) {
	gdk_drawable_get_size(GDK_DRAWABLE(radar_pixmap), &w, &h);
	if (w > event->width)
	    w = event->width;
	if (h > event->height)
	    h = event->height;
	gdk_draw_pixmap(pixmap, gc, radar_pixmap, 0, 0, 0, 0, w, h);
	gdk_pixmap_unref(radar_pixmap);
    }
    gdk_gc_unref(gc);
    radar_pixmap = pixmap;

    return FALSE;
}

static gint
radar_destroy_event(GtkWidget *widget, gpointer data)
{
    nh_radar_set_use(FALSE);

    return TRUE;
}
#endif	/* WINGTK_RADAR */

static gint
map_button_event(void *map, GdkEventButton *event, gpointer data)
{
    int x, y;
    y = event->y / c_3dheight;
    if (y < 0 || y >= no_rows)	/* Click outside area of map */
	return FALSE;
    x = (event->x - (no_rows - y) * c_3dofset ) / c_3dwidth;
    if (x < 0 || x >= no_cols)
	return FALSE;
    GTK_curs(NHW_MAP, x, y);
    if (event->button == 1)
	cursm = CLICK_1;
    else
	cursm = CLICK_2;

    if (map_click)
	quit_hook();
    return FALSE;
}

void
nh_map_check_visibility()
{
    nh_map_cliparound(cursx, cursy, FALSE);
}

void
nh_map_clear(int rows, int cols, int layers)
{
    int i, j, k;
    int glyph = cmap_to_glyph(S_stone);
    const char *tileset = nh_option_cache_get("tileset");

    nh_conf_map_dimens(rows, cols, layers, FALSE);

    /*
     * Check if tileset has changed and change map_visual if required.
     * This can happen if tileset changed via doset() and doredraw() was
     * called.  --ALI
     */
    if (strcmp(tileset, tileTab[map_visual].ident)) {
	int i;
	for(i = 0; i <= no_tileTab; i++)
	    if (!strcmp(tileset, tileTab[i].ident)) {
		nh_set_map_visual(i);
		break;
	    }
	if (i > no_tileTab) {
	    pline("Tileset %s not valid.", tileset);
	    nh_option_cache_set("tileset", tileTab[map_visual].ident);
	}
    }

    /* TODO: We should implement a method of quickly clearing large
     * portions of the map to a (repeating) tile to speed this up.
     */
    for(j = 0; j < rows; j++)
	for(i = 0; i < cols; i++)
	    for(k = 0; k < layers; k++)
		if (GTKMAP(j, i)->glyphs[k] != NO_GLYPH) {
		    GTKMAP(j, i)->glyphs[k] = NO_GLYPH;
		    GTKMAP(j, i)->flags = TILEMAP_UPDATE;  /* Reset all flags */
		    map_update = 1;
		}
}

void
nh_map_destroy()
{
    int i;
    nh_conf_map_dimens(-1, -1, -1, FALSE);
    nh_set_map_visual(-1);
    if (map_font) {
	gdk_font_unref(map_font);
	free(map_xoffsets);
    }
#ifdef GTK_PROXY
    if (map_glyph2colsym) {
	free(map_glyph2colsym);
	map_glyph2colsym = (short *)0;
    }
#endif
    gdk_gc_unref(map_gc);
    for (i = 0; i < N_NH_COLORS; i++)
	gdk_gc_unref(map_color_gc[i]);
    x_tile_destroy();
    xshm_map_destroy();
}

static int
configure_map(GtkWidget *w, gpointer data)
{
    int i;
    int width, height;
    guint path_length;
    gchar *path;

    /*
     * Configure for new font metrics
     */
    if (map_visual == 0) {
	nh_conf_map_font();
	c_width = map_font_width;
	c_height = map_font->ascent + map_font->descent;
	c_3dwidth = c_width;
	c_3dheight = c_height;
	c_3dofset = 0;

	c_map_width = no_cols * c_width;
	c_map_height = no_rows * c_height;
	w = map = xshm_map_init(XSHM_MAP_PIXMAP, c_map_width, c_map_height);
    }
    /*
     * set gc 
     */
    if (!map_gc)
	map_gc = gdk_gc_new(w->window);

#define COLOUR_IS_RGB(colour,r,g,b)   \
      ((colour).red==(r) && (colour).green==(g) && (colour).blue==(b))

    if (COLOUR_IS_RGB(w->style->bg[GTK_STATE_NORMAL], 0, 0, 0))
	nh_color[CLR_BLACK] = nh_color[MAP_WHITE];
    else if (COLOUR_IS_RGB(w->style->bg[GTK_STATE_NORMAL], 65535, 65535, 65535))
	nh_color[CLR_WHITE] = nh_color[MAP_BLACK];

#undef COLOUR_IS_RGB

    for(i = 0; i < N_NH_COLORS; i++) {
	if (!map_color_gc[i])
	    map_color_gc[i] = gdk_gc_new(w->window);
	gdk_gc_set_foreground(map_color_gc[i], &nh_color[i]);
	gdk_gc_set_background(map_color_gc[i], &w->style->bg[GTK_STATE_NORMAL]);
    }

    return FALSE;
}

GtkWidget *
nh_map_new(GtkWidget *w)
{
    int i;
    int width, height;
    int visual;

    visual = tile_scan();

    nh_conf_map_dimens(21, 80, 1, FALSE);
    map = xshm_map_init(XSHM_MAP_NONE, 0, 0);
    gtk_widget_set_name(map, "map");
    xshm_map_button_handler(GTK_SIGNAL_FUNC(map_button_event), NULL);
    gtk_signal_connect_after(GTK_OBJECT(map), "realize",
      GTK_SIGNAL_FUNC(configure_map), 0);

    if (!nh_set_map_visual(visual)) {
	for(i = 0; i <= no_tileTab; i++)
	    if (i != visual && nh_set_map_visual(i))
		break;
	if (i > no_tileTab)
	    panic("No valid map modes!");
    }

    nh_map_clear(no_rows, no_cols, no_layers);

    return map;
}

#ifdef WINGTK_RADAR

/*
 * create radar
 */

void
nh_radar_destroy()
{
    if (radar_pixmap)
	gdk_pixmap_unref(radar_pixmap);
    if (radar_pixmap2)
	gdk_pixmap_unref(radar_pixmap2);
    gtk_widget_destroy(radar);
}

GtkWidget *
nh_radar_new()
{
    radar = nh_session_window_new("radar");
#if GTK_CHECK_VERSION(1,3,12)
    gtk_window_add_accel_group(GTK_WINDOW(radar), accel_group);
#else
    gtk_accel_group_attach(accel_group, G_OBJECT(radar));
#endif

    radar_is_created = 1;

    gtk_window_set_title(GTK_WINDOW(radar), DEF_GAME_NAME " Radar");

    radar_darea = nh_gtk_new_and_add(gtk_drawing_area_new(), radar, "");

    gtk_signal_connect(GTK_OBJECT(radar_darea), "expose_event",
      GTK_SIGNAL_FUNC(radar_expose_event), NULL);

    gtk_signal_connect(GTK_OBJECT(radar_darea), "configure_event",
      GTK_SIGNAL_FUNC(radar_configure_event), NULL);

    gtk_signal_connect(GTK_OBJECT(radar), "delete_event",
      GTK_SIGNAL_FUNC(radar_destroy_event), 0);

    if (use_radar && NH_RADAR_WIDTH > 0 && NH_RADAR_HEIGHT > 0) {
	gtk_drawing_area_size(GTK_DRAWING_AREA(radar_darea),
	  NH_RADAR_WIDTH, NH_RADAR_HEIGHT);
	gtk_widget_show(radar);
    }

    return radar;
}

static void
nh_radar_configure()
{
    if (NH_RADAR_WIDTH > 0 && NH_RADAR_HEIGHT > 0) {
	gtk_drawing_area_size(GTK_DRAWING_AREA(radar_darea),
	  NH_RADAR_WIDTH, NH_RADAR_HEIGHT);
	nh_radar_update();
	if (use_radar)
	    gtk_widget_show(radar);
    } else {
	if (radar_pixmap)
	    gdk_pixmap_unref(radar_pixmap);
	if (radar_pixmap2)
	    gdk_pixmap_unref(radar_pixmap2);
	radar_pixmap = NULL;
	radar_pixmap2 = NULL;
    }
}

void
nh_print_radar(int x, int y, struct tilemap *tmap)
{
    int k, glyph;
    int c;
    if (!radar_pixmap)
	return;

    glyph = cmap_to_glyph(S_stone);
    for(k = 0; k < no_layers; k++)
	if (tmap->glyphs[k] != NO_GLYPH) {
	    glyph = tmap->glyphs[k];
	    break;
	}

    c = CLR_BLACK;
    if (glyph < PM_ARCHEOLOGIST)
	c = RADAR_MONSTER;
    else if (glyph_is_monster(glyph))
	c = RADAR_HUMAN;
    else if (glyph_is_pet(glyph))
	c = RADAR_PET;
    else if (glyph_is_object(glyph))
	c = RADAR_OBJECT;
    else if (glyph_is_trap(glyph))
	c = RADAR_TRAP;
    else if (glyph_is_swallow(glyph))
	c = RADAR_SWALLOW;
    else if( glyph_is_cmap(glyph)) {
	if (glyph == GLYPH_CMAP_OFF)
	    ;
	else if (glyph <= GLYPH_CMAP_OFF + S_trwall)
	    c = RADAR_WALL;
	else if (glyph <= GLYPH_CMAP_OFF + S_hcdoor)
	    c = RADAR_DOOR;
	else if (glyph <= GLYPH_CMAP_OFF + S_litcorr)
	    c = RADAR_FLOOR;
	else if (glyph <= GLYPH_CMAP_OFF + S_dnladder)
	    c = RADAR_LADDER;
	else if (glyph <= GLYPH_CMAP_OFF + S_pool)
	    c = RADAR_WATER;
	else if (glyph <= GLYPH_CMAP_OFF + S_ice)
	    c = RADAR_ICE;	/* fountain sink ice */
	else if (glyph <= GLYPH_CMAP_OFF + S_lava)
	    c = RADAR_LAVA;	/* lava */
	else if (glyph <= GLYPH_CMAP_OFF + S_hcdbridge)
	    c = RADAR_BRIDGE;	/* bridge */
	else if (glyph <= GLYPH_CMAP_OFF + S_air)
	    c = RADAR_AIR;	/* air */
	else if (glyph <= GLYPH_CMAP_OFF + S_cloud)
	    c = RADAR_CLOUD;	/* cloud */
	else if (glyph <= GLYPH_CMAP_OFF + S_water)
	    c = RADAR_WATER;	/* water */
	else if (glyph < GLYPH_CMAP_OFF + S_explode9)
	    c = RADAR_BEAM;	/* beam */
    }
    else
	c = RADAR_WALL;

    gdk_draw_rectangle(radar_pixmap, map_color_gc[c], TRUE, 
      x * NH_RADAR_UNIT, y * NH_RADAR_UNIT, NH_RADAR_UNIT, NH_RADAR_UNIT);
}
#endif	/* WINGTK_RADAR */

#ifndef GTK_PROXY
#ifdef TEXTCOLOR
#define zap_color(n)	(zapcolors[n])
#define cmap_color(n)	(defsyms[n].color)
#define obj_color(n)	(objects[n].oc_color)
#define mon_color(n)	(mons[n].mcolor)
#define pet_color(n)	(mons[n].mcolor)
#else
#define zap_color(n)	(n)
#define cmap_color(n)	(n)
#define obj_color(n)	(n)
#define mon_color(n)	(n)
#define pet_color(n)	(n)
#endif
#endif	/* GTK_PROXY */

static void
nh_map_print_glyph_traditional(XCHAR_P x, XCHAR_P y, struct tilemap *tmap)
{
    static GdkRectangle update_rect;
    int color;
    int k, glyph;
#ifndef GTK_PROXY
    int offset;
#endif
    GdkWChar ch[2];

    color = 0;
    glyph = cmap_to_glyph(S_stone);
    for(k = 0; k < no_layers; k++)
	if (tmap->glyphs[k] != NO_GLYPH) {
	    glyph = tmap->glyphs[k];
	    break;
	}

#ifdef GTK_PROXY
    ch[0] = map_glyph2colsym[glyph] & 0xFF;
    color = map_glyph2colsym[glyph] >> 8 & 0xFF;
#else
    if ((offset = (glyph - GLYPH_SWALLOW_OFF)) >= 0) {	/* swallow */
	ch[0] = (uchar) showsyms[S_sw_tl + (offset & 0x7)];
	color = mon_color(offset>>3);
    } else if ((offset = (glyph - GLYPH_ZAP_OFF)) >= 0) {	/* zap beam */
	ch[0] = showsyms[S_vbeam + (offset & 0x3)];
	color = zap_color(offset>>2);
    } else if ((offset = (glyph - GLYPH_CMAP_OFF)) >= 0) {/* cmap */
	ch[0] = showsyms[offset];
	color = cmap_color(offset);
    } else if ((offset = (glyph - GLYPH_OBJ_OFF)) >= 0) {	/* object */
	ch[0] = oc_syms[(int)objects[offset].oc_class];
	color = obj_color(offset);
    } else if ((offset = (glyph - GLYPH_BODY_OFF)) >= 0) {/* a corpse */
	ch[0] = oc_syms[(int)objects[CORPSE].oc_class];
	color = mon_color(offset);
    } else if ((offset = (glyph - GLYPH_PET_OFF)) >= 0) {	/* a pet */
	ch[0] = monsyms[(int)mons[offset].mlet];
	color = pet_color(offset);
    } else{						/* a monster */
	ch[0] = monsyms[(int)mons[glyph].mlet];
	color = mon_color(glyph);
    }
#endif

    ch[1] = '\0';

    gdk_draw_rectangle(xshm_map_pixmap,
      map->style->bg_gc[GTK_WIDGET_STATE(map)],
      TRUE, x * c_width, y * c_height -  map_font->ascent, c_width, c_height);
    
    gdk_draw_text_wc(xshm_map_pixmap, map_font, copts.use_color ?
      map_color_gc[color] : map->style->fg_gc[GTK_WIDGET_STATE(map)],
      x * c_width + map_xoffsets[ch[0]], y * c_height, ch, 1);
    
    if (glyph_is_pet(glyph) && copts.hilite_pet) {
	gdk_draw_rectangle(xshm_map_pixmap,
	  map_color_gc[copts.use_color ? CLR_RED : CLR_WHITE], FALSE,
	  x * c_width, y * c_height - map_font->ascent,
	  c_width - 1, c_height - 1);
    }
    else if (x == cursx && y == cursy)
	gdk_draw_rectangle(xshm_map_pixmap, map_color_gc[CLR_WHITE], FALSE,
	  x * c_width, y * c_height - map_font->ascent,
	  c_width - 1, c_height - 1);

    update_rect.x = x * c_width;
    update_rect.y = y * c_height - map_font->ascent;
    update_rect.width = c_width;
    update_rect.height = c_height;
    xshm_map_draw(&update_rect);
}

/*
 * The assumption that the glyph in the layer furthest from the viewer is
 * the only surface glyph present is flawed. However, it's the best we can
 * do without knowing details of the tileset in use. This might well be a
 * sensible addition, but for now, this suffices.
 */

static void
nh_map_print_glyph_tmp(struct tilemap *tmap, int ofsx, int ofsy, int dosurface)
{
    int k, tile;

    if (Tile->transparent) {
	if (dosurface) {
	    tile = GLYPH2TILE(tmap->glyphs[no_layers - 1]);
	    x_tile_tmp_draw_tile(tile, ofsx, ofsy);
	} else
	    for(k = no_layers - 2; k >= 0; k--)
		if (tmap->glyphs[k] != NO_GLYPH) {
		    tile = glyph2tile[tmap->glyphs[k]];
		    x_tile_tmp_draw_tile(tile, ofsx, ofsy);
		}
    } else if (!dosurface) {
	for(k = 0; k < no_layers - 1; k++)
	    if (tmap->glyphs[k] != NO_GLYPH)
		break;
	tile = GLYPH2TILE(tmap->glyphs[k]);
	x_tile_tmp_draw_tile(tile, ofsx, ofsy);
    }
    if (!dosurface) {
	if (tmap == GTKMAP(cursy, cursx))
	    x_tile_tmp_draw_rectangle(ofsx, ofsy, MAP_WHITE);
	else if (copts.hilite_pet)
	    for(k = 0; k < no_layers; k++)
		if (glyph_is_pet(tmap->glyphs[k])) {
		    x_tile_tmp_draw_rectangle(ofsx, ofsy, CLR_RED);
		    break;
		}
    }
}

#define NH_MAP_PRINT_GLYPH_TMP(y, x, j, i, k) \
	nh_map_print_glyph_tmp(GTKMAP((y)+(j), (x)+(i)), \
	(i)*c_3dwidth - (j)*c_3dofset, (j)*c_3dheight, k)

/*
 * The ordering of glyph drawing here is very complex in order to
 * meet the following rules:
 *	- Surface glyphs should never obscure anything.
 *	- Glyphs in locations towards the bottom of the screen should
 *	  obscure glyphs higher up.
 *	- Glyphs in locations towards the right of the screen should
 *	  obscure glyphs to the left.
 *
 * Note that these rules are not perfect. For example, a dragon standing
 * in a doorway of a horizontal wall will correctly have part of the
 * right hand of its body obscured by the wall to its right, but its
 * feet stick out under the wall and are thus visible. Where possible,
 * tiles should be drawn with these problems in mind so that the
 * experience is not spoiled for the player.
 */

static void
nh_map_print_glyph_tile(XCHAR_P x, XCHAR_P y, struct tilemap *tmap)
{
    int k;
    x_tile_tmp_clear();

    if (Tile->spread) {
	for(k = 1; k >= 0; k--) {
	    if (y > 0) {
		if (x > 0)
		    NH_MAP_PRINT_GLYPH_TMP(y, x, -1, -1, k);
		NH_MAP_PRINT_GLYPH_TMP(y, x, -1, 0, k);
	    }
	    if (x > 0)
		NH_MAP_PRINT_GLYPH_TMP(y, x, 0, -1, k);
	    NH_MAP_PRINT_GLYPH_TMP(y, x, 0, 0, k);
	    if (x < no_cols - 1)
		NH_MAP_PRINT_GLYPH_TMP(y, x, 0, 1, k);
	    if (y < no_rows - 1) {
		NH_MAP_PRINT_GLYPH_TMP(y, x, 1, 0, k);
		if (x < no_cols - 1)
		    NH_MAP_PRINT_GLYPH_TMP(y, x, 1, 1, k);
	    }
	}
    } else {
	NH_MAP_PRINT_GLYPH_TMP(y, x, 0, 0, 1);
	NH_MAP_PRINT_GLYPH_TMP(y, x, 0, 0, 0);
    }

    x_tile_draw_tmp(x * c_3dwidth + (no_rows - y) * c_3dofset, y * c_3dheight);
}

static void
nh_map_print_glyph_simple_tile(XCHAR_P x, XCHAR_P y, struct tilemap *tmap)
{
    int k;
    for(k = 0; k < no_layers - 1; k++)
	if (tmap->glyphs[k] != NO_GLYPH)
	    break;
    x_tile_draw_tile(GLYPH2TILE(tmap->glyphs[k]), x * c_width, y * c_height);
    if (x == cursx && y == cursy)
	x_tile_draw_rectangle(x * c_width, y * c_height, &nh_color[MAP_WHITE]);
    else if (copts.hilite_pet)
	for(k = 0; k < no_layers; k++)
	    if (glyph_is_pet(tmap->glyphs[k])) {
		x_tile_draw_rectangle(x * c_width, y * c_height,
		  &nh_color[CLR_RED]);
		break;
	    }
}

static void
nh_map_print_glyph(XCHAR_P x, XCHAR_P y, struct tilemap *tmap)
{
#ifdef WINGTK_RADAR
    nh_print_radar(x, y, tmap);
#endif
    
    if (map_visual == 0)
	nh_map_print_glyph_traditional(x, y, tmap);
    else if (!Tile->transparent && !Tile->spread)
	nh_map_print_glyph_simple_tile(x, y, tmap);
    else
	nh_map_print_glyph_tile(x, y, tmap);
}

void
GTK_ext_print_glyph_layered(winid id, int nl, struct proxy_glyph_layer *layers)
{
    int i, j, k, x, y;
    struct proxy_glyph_layer *l;

    for(k = 0, l = layers; k < nl; k++, l++)
	for(j = 0, y = l->start; j < l->nr; j++, y++)
	    for(i = 0, x = l->rows[j].start; i < l->rows[j].ng; i++, x++)
		if (GTKMAP(y, x)->glyphs[k] != l->rows[j].glyphs[i]) {
		    GTKMAP(y, x)->glyphs[k] = l->rows[j].glyphs[i];
		    GTKMAP(y, x)->flags |= TILEMAP_UPDATE;
		    map_update = 1;
		}
}

void
GTK_ext_print_glyph(winid id, int x, int y, int glyph)
{
    int k;

    if (GTKMAP(y, x)->glyphs[0] != glyph || GTKMAP(y, x)->flags != 0) {
	GTKMAP(y, x)->glyphs[0] = glyph;
	GTKMAP(y, x)->flags = TILEMAP_UPDATE;
	map_update = 1;
    }
    for (k = 1; k < no_layers; k++)
	if (GTKMAP(y, x)->glyphs[k] != NO_GLYPH) {
	    GTKMAP(y, x)->glyphs[k] = NO_GLYPH;
	    GTKMAP(y, x)->flags |= TILEMAP_UPDATE;
	    map_update = 1;
	}
}

void
GTK_curs(winid id, int x, int y)
{
    if (id != NHW_MAP)
	return;

    if (cursx != x || cursy != y) {
	map_update = 1;
	GTKMAP(y, x)->flags |= TILEMAP_UPDATE;
	GTKMAP(cursy, cursx)->flags |= TILEMAP_UPDATE;
	cursx = x;
	cursy = y;
    }
}

#ifdef WINGTK_RADAR
static void
nh_radar_redraw()
{
    int i, j;
    for(j = 0; j < no_rows; j++)
	for(i = 0; i < no_cols; i++)
	    nh_print_radar(i, j, GTKMAP(j, i));
    nh_radar_update();
}

boolean nh_radar_get_use(void)
{
    return use_radar;
}

void nh_radar_set_use(boolean use)
{
    int old = use_radar;
    use_radar = use;
    if (radar_is_created) {
	if (!use)
	    gtk_widget_hide(radar);
	else {
	    gtk_window_present(GTK_WINDOW(radar));
	    if (!old)
		nh_radar_redraw();
	}
    }
}

void
nh_radar_update()
{
    GdkRectangle update_rect;
    GtkAdjustment *hadj, *vadj;

    if (use_radar && radar_pixmap2 && map && map_gc) {
	hadj = xshm_map_get_hadjustment();
	vadj = xshm_map_get_vadjustment();

	gdk_draw_pixmap(radar_pixmap2, map_gc, radar_pixmap, 0, 0, 0, 0, 
	  NH_RADAR_WIDTH, NH_RADAR_HEIGHT);
	
	gdk_draw_rectangle(radar_pixmap2, map_color_gc[CLR_WHITE], FALSE,
	  hadj->value / c_3dwidth * NH_RADAR_UNIT,
	  vadj->value / c_3dheight * NH_RADAR_UNIT,
	  hadj->page_size / c_3dwidth * NH_RADAR_UNIT,
	  vadj->page_size / c_3dheight * NH_RADAR_UNIT);
	
	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = NH_RADAR_WIDTH;
	update_rect.height = NH_RADAR_HEIGHT;
	
	gtk_widget_draw(radar, &update_rect);
	gdk_window_raise(radar->window);
    }
}
#endif	/* WINGTK_RADAR */

static void
nh_map_cliparound(int x, int y, gboolean exact)
{
    static int sx = -1, sy;
    if (!map || !exact && sx >= 0 &&
      (x - sx) * (x - sx) + (y - sy) * (y - sy) <= map_clip_dist2)
	return;
    sx = x;
    sy = y;
    xshm_map_cliparound(x * c_3dwidth + (no_rows - y) * c_3dofset,
      y * c_3dheight);
}

#undef DIFF

void
GTK_cliparound(int x, int y)
{
    nh_map_cliparound(x, y, FALSE);
}

/*
 * [ALI]
 * This function returns a pixmap of size 3dwidth x 3dheight pixels.
 * In non-3D mode, this is the full tile size, but in 3D mode this
 * strips out the 3D part. A complete algorithm for extracting the
 * non-3D part of the tile is quite complex (see bigtile.c for a
 * starting point), but since this routine is currently only used
 * for objects, we can safely assume that all tiles are padded,
 * ie., that the non-3D part is centered in the tile.
 */

GdkPixmap *
GTK_glyph_to_gdkpixmap(int glyph)
{
    int tile;
    int width, height;
    GdkPixmap *pix;

    width = Tile->unit_width - Tile->ofsetx_3d;
    height = Tile->unit_height - Tile->ofsety_3d;
    pix = gdk_pixmap_new(NULL, width, height, gdk_visual_get_system()->depth);
    if (!pix)
	return NULL;

    tile = fix_tile(GLYPH2TILE(glyph));

    if (!x_tile_render_to_drawable(pix, map_gc, tile,
      Tile->ofsetx_3d/2, Tile->ofsety_3d/2, 0, 0, width, height)) {
	gdk_pixmap_unref(pix);
	pix = NULL;
    }

    return pix;
}

void
nh_map_click(int f)
{
    map_click = f;
}

void
nh_map_pos(int *x, int *y, int *mod)
{
    *x = cursx;
    *y = cursy;
    *mod = cursm;
}

static void
nh_map_redraw()
{
    int i, j;
    map_update = 1;
    for(j = 0; j < no_rows; j++)
	for(i = 0; i < no_cols; i++)
	    GTKMAP(j, i)->flags |= TILEMAP_UPDATE;
}

void
nh_map_color_changed(boolean new_value)
{
    if (map_visual == 0)
	nh_map_redraw();
}

void
nh_map_hilite_pet_changed(boolean new_value)
{
    int i, j, k;
    map_update = 1;
    for(j = 0; j < no_rows; j++)
	for(i = 0; i < no_cols; i++)
	    for(k = 0; k < no_layers; k++)
		if (glyph_is_pet(GTKMAP(j, i)->glyphs[k]))
		    GTKMAP(j, i)->flags |= TILEMAP_UPDATE;
}

void
nh_map_flush()
{
    int i, j;

    if (!xshm_map_pixmap && !xshm_map_image && !xshm_map_pixbuf)
	return;		/* Map not realized yet - pend flush */

    if (map_update) {
	map_update = 0;

	for(j = 0; j < no_rows; j++)
	    for(i = 0; i < no_cols; i++)
		if (GTKMAP(j, i)->flags & TILEMAP_UPDATE) {
		    GTKMAP(j, i)->flags &= ~TILEMAP_UPDATE;
		    nh_map_print_glyph(i, j, GTKMAP(j, i));
		}
	nh_map_check_visibility();
	xshm_map_flush();
#ifdef WINGTK_RADAR
	nh_radar_update();
#endif
    }
    GTK_curs(NHW_MAP, cursx, cursy);
}

int
nh_tile_height()
{
    return c_height;
}

int
nh_tile_3dheight()
{
    return c_3dheight;
}

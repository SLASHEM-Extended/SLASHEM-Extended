/*
  $Id: gtkmap.c,v 1.10 2002-07-10 16:31:23 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2000-2001
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include "winGTK.h"
#include "dlb.h"
#ifdef SHORT_FILENAMES
#include "patchlev.h"
#else
#include "patchlevel.h"
#endif
#include "decl.h"

#undef red
#undef green
#undef blue

/*
  if map_click is true, we do gtk_main_quit() when clicking map
 */
static int		map_click;
static int		map_update;

static GtkWidget	*map;
static GdkFont		*map_font;
static gchar		*map_font_name;
static unsigned char	*map_xoffsets;		/* For character mode only */
static unsigned int	map_font_width;		/* Maximum width */

static GdkGC		*map_color_gc[N_NH_COLORS];

static gint		map_button_event(void *map, GdkEventButton *event,
			  gpointer data);
static void		nh_map_cliparound(int x, int y, gboolean exact);
static void		nh_map_redraw();
static int		configure_map(GtkWidget *w, gpointer data);

#ifdef	RADAR
static int	 radar_is_popuped;
static int	 radar_is_created;

static GtkWidget *radar;
static GtkWidget *radar_darea;
/*
  we use 2 pixmaps for radar.

  radar_pixmap keeps radar dot
  radar_pixmap2 keeps radar dot and frame rectangle
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

#endif

static GdkGC	 *map_gc;

/*
	BIG3DTILE

                  +----------+
  ^              /          /| 
  |             /          / | 
  |            /          /  | 
  |           /          /   | 
  *6         /          /    |
  |         +----------+     +
  |       ^ |          |    / ^
  |       | |          |   /  |
  |      *2 |          |  /   *4
  |       | |          | /    |
  v       v |          |/     v
            +----------+      
             <-- *1 --> <-*3->
             <-- *5        -->

*1 = 3D_WIDTH	= 32
*2 = 3D_HEIGHT	= 32
*3 = 3D_OFSET	= 16
*4 = 3D_OFSETY	= 32
*5 = WIDTH	= 48
*6 = HEIGHT	= 64
 */

TileTab tileTab[MAXNOTILESETS+1] = {
    { "", "" },		/* dummy */
};
int no_tileTab = 0;	/* Not including dummy (index 0) */

static TileTab *Tile;
int	map_visual = -1;

/* from tile.c */
extern int tiles_per_row;
extern int tiles_per_col;

#ifdef RADAR
#define NH_RADAR_UNIT	4
#define NH_RADAR_WIDTH	(COLNO * NH_RADAR_UNIT)
#define NH_RADAR_HEIGHT	(ROWNO * NH_RADAR_UNIT)
#endif

extern short	glyph2tile[];
extern int	root_width;
extern int	root_height;

extern GtkWidget	*main_window;

int		cursx;
int		cursy;
int		cursm;

static int	c_width;
static int	c_height;

static int	c_3dwidth;
static int	c_3dheight;
static int	c_3dofset;

static int	c_map_width;
static int	c_map_height;

#define TILEMAP_UPDATE	1

static int no_layers = -1;
static int tilemap_size = 0;	/* sizeof(struct tilemap) */

struct tilemap {
    int glyph;
    unsigned int flags;
    int	tiles[1];
};

static struct tilemap *gtkmap[ROWNO] = {NULL};

#define GTKMAP_PTR(ptr, col, size)	\
				((struct tilemap *)((char *)(ptr)+(col)*(size)))
#define GTKMAP(row, col)	GTKMAP_PTR(gtkmap[row], col, tilemap_size)

/*
  fix tile number
 */
static int
fix_tile(int tile)
{
    return tile;
}

/*
 * (Re)configure the map for n layers (-1 to free).
 */

static int
nh_conf_map_layers(int no)
{
    int i, j, k;
    int glyph = cmap_to_glyph(S_stone);
    int stone = fix_tile(glyph2tile[glyph]);
    int retval = 0;
    int new_size = sizeof(struct tilemap) + (no - 1) * sizeof(int);
    struct tilemap *new, *tm;
    if (no == no_layers)
	return 0;
    if (no >= 0)
	for(j = 0; j < ROWNO; j++) {
	    new = malloc(COLNO * new_size);
	    if (!new) {
		retval = -1;
		break;
	    }
	    if (gtkmap[j]) {
		for(i = 0; i < COLNO; i++) {
		    tm = GTKMAP_PTR(new, i, new_size);
		    tm->glyph = GTKMAP(j, i)->glyph;
		    tm->flags = GTKMAP(j, i)->flags;
		    for(k = 0; k < min(no, no_layers); k++)
			tm->tiles[k] = GTKMAP(j, i)->tiles[k];
		    for(; k < no; k++)
			tm->tiles[k] = stone;
		}
		free(gtkmap[j]);
		gtkmap[j] = new;
	    }
	    else {
		for(i = 0; i < COLNO; i++) {
		    tm = GTKMAP_PTR(new, i, new_size);
		    tm->glyph = glyph;
		    tm->flags = TILEMAP_UPDATE;
		    for(k = 0; k < no; k++)
			tm->tiles[k] = stone;
		}
		gtkmap[j] = new;
	    }
	}
    if (no < 0 || retval) {
	for(j = 0; j < ROWNO; j++) {
	    free(gtkmap[j]);
	    gtkmap[j] = NULL;
	}
	no_layers = -1;
	tilemap_size = 0;
    }
    else {
	no_layers = no;
	tilemap_size = new_size;
    }
    return retval;
}

static int
nh_conf_map_font(void)
{
    int i, min_width, width;
    if (!map_font) {
	g_return_val_if_fail(map->style != NULL, 1);
	map_font = gtk_style_get_font(map->style);
	g_return_val_if_fail(map_font != NULL, 1);
	gdk_font_ref(map_font);
	map_font_name = pango_font_description_to_string(map->style->font_desc);
    }

    /*
     * ALI
     * We might want to consider making the size of this array
     * variable in the future, but for now 8 bits is always enough.
     */
    map_xoffsets = (unsigned char *) alloc(256);
    for(i = 0; i < 256; i++)
	map_xoffsets[i] = 0;
    map_font_width = min_width =
      gdk_char_width_wc(map_font, (GdkWChar)oc_syms[0]);
    if (c_width > 0)
	map_xoffsets[oc_syms[i]] = c_width;
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
    if (min_width <= 0)
	pline("Warning: Not all expected glyphs present in map font \"%s\".",
	  map_font_name);

    /* Convert widths to offsets */
    for(i = 0; i < 256; i++)
	if (map_xoffsets[i])
	    map_xoffsets[i] = (map_font_width - map_xoffsets[i]) / 2;
    return 0;
}

/*
  -1:   deallocate
  zero: character
  else: various tiles
 */
int
nh_set_map_visual(int mode)
{
    static int setting_visual = FALSE;		/* Ignore recursive calls */
    int saved_vis = map_visual;
    enum xshm_map_mode map_mode;

    if (setting_visual)
	return 0;
    setting_visual++;

    if(mode < -1 || mode > no_tileTab)
	panic("Bad visual!\n");

    if(saved_vis != mode){
	if (map)
	    gtk_widget_hide(map);

	if(saved_vis > 0)
	    x_tile_destroy();
switch_mode:
	if (mode < 0) {
	    map_visual = mode;
	    setting_visual--;
	    return 1;
	} else if (mode != 0) {      /* mode 0 is handled in configure_map() */
	    if (!tileTab[mode].ident[0])
		map_mode = XSHM_MAP_NONE;
	    else {
		Tile = tileTab + mode;
		map_mode = x_tile_init(Tile);
	    }

	    if (map_mode != XSHM_MAP_NONE) {
		c_width = Tile->unit_width;
		c_height = Tile->unit_height;
		c_3dwidth = Tile->unit_width - Tile->ofsetx_3d;
		c_3dheight = Tile->unit_height - Tile->ofsety_3d;
		c_3dofset = Tile->ofsetx_3d;
		c_map_width = c_3dwidth * (COLNO - 1) + c_3dofset * ROWNO +
		  Tile->unit_width;
		c_map_height = c_3dheight * (ROWNO - 1) + Tile->unit_height;
	    }
	    else {
		if (saved_vis > 0 && tileTab[saved_vis].ident || !saved_vis) {
		    pline("Warning: Switching back to %s.",
		      saved_vis?tileTab[saved_vis].ident:"character mode");
		    mode = saved_vis;
		    saved_vis = -1;
		    goto switch_mode;
		}
		else if (map_visual > 0)
		    panic("Failed to switch back to previous mode.");
		else {
		    setting_visual--;
		    return 0;
		}
	    }
	
	    map = xshm_map_init(map_mode, c_map_width, c_map_height);
	}

	map_visual = mode;
	if (GTK_WIDGET_REALIZED(map))
	    configure_map(map, 0);
	nh_map_check_visibility();
#ifdef RADAR
	nh_radar_update();
#endif
	nh_option_cache_set("tileset", tileTab[map_visual].ident);
	nh_map_redraw();

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
    if(mode < 0 || mode > no_tileTab || mode && !tileTab[mode].ident[0])
	return -1;
    else
	return 0;
}

int
nh_set_map_font(GdkFont *font, gchar *name)
{
    g_return_if_fail(font != NULL);
    if (font == map_font)
	return 0;
    if (map_font)
	gdk_font_unref(map_font);
    if (map_font_name)
	g_free(map_font_name);
    map_font = font;
    map_font_name = g_strdup(name);
    gdk_font_ref(map_font);
    configure_map(map, 0);
    return 0;
}

gchar *nh_get_map_font(void)
{
    if (map_font_name)
	return g_strdup(map_font_name);
    else if (map && map->style && map->style->font_desc)
	return pango_font_description_to_string(map->style->font_desc);
    else
	return NULL;
}

#ifdef RADAR
static gint
radar_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    gdk_draw_pixmap(
	widget->window,
	widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
	radar_pixmap2,
	event->area.x, event->area.y,
	event->area.x, event->area.y,
	event->area.width, event->area.height);

    return FALSE;
}

static gint
radar_destroy_event(GtkWidget *widget, gpointer data)
{

    radar_is_popuped = 0;
    gtk_widget_hide_all(radar);
    nh_option_cache_set_bool("radar", FALSE);
    nh_option_cache_sync();

    return TRUE;
}
#endif	/* RADAR */

static gint
map_button_event(void *map, GdkEventButton *event, gpointer data)
{
    int x, y;
    y = event->y / c_3dheight;
    if (y < 0 || y >= ROWNO)	/* Click outside area of map */
	return FALSE;
    x = (event->x - (ROWNO - y) * c_3dofset ) / c_3dwidth;
    if (x < 0 || x >= COLNO)
	return FALSE;
    GTK_curs(NHW_MAP, x, y);
    if (event->button == 1)
	cursm = CLICK_1;
    else
	cursm = CLICK_2;

    if(map_click)
	gtk_main_quit();
    return FALSE;
}

void
nh_map_check_visibility()
{
    nh_map_cliparound(cursx, cursy, TRUE);
}

void
nh_map_clear()
{
    int i, j, k;
    int glyph = cmap_to_glyph(S_stone);
    int stone = fix_tile(glyph2tile[glyph]);
    const char *tileset = nh_option_cache_get("tileset");
    /*
     * Check if tileset has changed and change map_visual if required.
     * This can happen if tileset changed via doset() and doredraw() was
     * called.  --ALI
     */
    if (strcmp(tileset, tileTab[map_visual].ident)){
	int i;
	for(i = 0; i <= no_tileTab; i++)
	    if (!strcmp(tileset, tileTab[i].ident)){
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
    for(j = 0; j < ROWNO; j++)
	for(i = 0; i < COLNO; i++) {
	    for(k = 0; k < no_layers; k++)
		if (GTKMAP(j, i)->tiles[k] != stone) {
		    GTKMAP(j, i)->tiles[k] = stone;
		    GTKMAP(j, i)->flags = TILEMAP_UPDATE;  /* Reset all flags */
		    map_update = 1;
		}
	    if (GTKMAP(j, i)->glyph != glyph) {
		GTKMAP(j, i)->glyph = glyph;
		GTKMAP(j, i)->flags = TILEMAP_UPDATE;
		map_update = 1;
	    }
	}

    nh_map_flush();
}

void
nh_map_destroy()
{
    int i;
    nh_conf_map_layers(-1);
    nh_set_map_visual(-1);
    if (map_font) {
	gdk_font_unref(map_font);
	free(map_xoffsets);
    }
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

	c_map_width = COLNO * c_width;
	c_map_height = ROWNO * c_height;
	w = map = xshm_map_init(XSHM_MAP_PIXMAP, c_map_width, c_map_height);
    }
    /*
     * set gc 
     */
    if (!map_gc)
	map_gc = gdk_gc_new(w->window);

#define COLOUR_IS_RGB(colour,r,g,b)   \
      ((colour).red==(r) && (colour).green==(g) && (colour).blue==(b))

    if (COLOUR_IS_RGB(w->style->bg[GTK_STATE_NORMAL],0,0,0))
      nh_color[CLR_BLACK] = nh_color[MAP_WHITE];
    else if (COLOUR_IS_RGB(w->style->bg[GTK_STATE_NORMAL],65535,65535,65535))
      nh_color[CLR_WHITE] = nh_color[MAP_BLACK];

#undef COLOUR_IS_RGB

    for(i=0 ; i < N_NH_COLORS ; ++i){
	if (!map_color_gc[i])
	    map_color_gc[i] = gdk_gc_new(w->window);
	gdk_gc_set_foreground(map_color_gc[i], &nh_color[i]);
	gdk_gc_set_background(map_color_gc[i], &w->style->bg[GTK_STATE_NORMAL]);
    }

    /*
     * set map scrolling window size
     */
    width = c_map_width;
    height = c_map_height;

    if(width >= root_width)
	width = root_width - 50;

    if(height >= (root_height / 2))
	height = (root_height / 2) - 50;

    gtk_widget_set_usize(map, width, height);

    return FALSE;
}

GtkWidget *
nh_map_new(GtkWidget *w)
{
    int i;
    int width, height;
    int visual;

    visual = tile_scan();

    nh_conf_map_layers(1);
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

    nh_map_clear();

    return map;
}

/*
  create radar
 */

#ifdef RADAR
void
nh_radar_destroy()
{
    gdk_pixmap_unref(radar_pixmap);
    gdk_pixmap_unref(radar_pixmap2);
    gtk_widget_destroy(radar);
}

GtkWidget *
nh_radar_new()
{
    radar = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#if GTK_CHECK_VERSION(1,3,12)
    gtk_window_add_accel_group(GTK_WINDOW(radar), accel_group);
#else
    gtk_accel_group_attach(accel_group, G_OBJECT(radar));
#endif
    gtk_widget_realize(radar);

    radar_is_created = 1;

    gtk_window_set_title(GTK_WINDOW(radar), DEF_GAME_NAME " Radar");

    radar_darea = nh_gtk_new_and_add(gtk_drawing_area_new(), radar, "");
    gtk_drawing_area_size(
	GTK_DRAWING_AREA(radar_darea),
	NH_RADAR_WIDTH, NH_RADAR_HEIGHT);

    gtk_signal_connect(
	GTK_OBJECT(radar_darea), "expose_event",
	GTK_SIGNAL_FUNC(radar_expose_event), NULL);

    gtk_signal_connect(
	GTK_OBJECT(radar), "delete_event",
	GTK_SIGNAL_FUNC(radar_destroy_event), 0);

    radar_pixmap = gdk_pixmap_new(
	radar->window,
	NH_RADAR_WIDTH, NH_RADAR_HEIGHT, -1);
  
    radar_pixmap2 = gdk_pixmap_new(
	radar->window,
	NH_RADAR_WIDTH, NH_RADAR_HEIGHT, -1);

    return radar;
}

void
nh_print_radar(int x, int y, int glyph)
{
    int c;
    /*
      int tile = glyph2tile[glyph];
      */
    c = CLR_BLACK;

    if(glyph < PM_ARCHEOLOGIST)
	c = RADAR_MONSTER;
    else if(glyph_is_monster(glyph))
	c = RADAR_HUMAN;
    else if(glyph_is_pet(glyph))
	c = RADAR_PET;
    else if(glyph_is_object(glyph))
	c = RADAR_OBJECT;
    else if(glyph_is_trap(glyph))
	c = RADAR_TRAP;
    else if(glyph_is_swallow(glyph))
	c = RADAR_SWALLOW;
    else if(glyph_is_cmap(glyph)){
	if(glyph == GLYPH_CMAP_OFF)
	    ;
	else if(glyph <= GLYPH_CMAP_OFF + S_trwall)
	    c = RADAR_WALL;
	else if(glyph <= GLYPH_CMAP_OFF + S_hcdoor)
	    c = RADAR_DOOR;
	else if(glyph <= GLYPH_CMAP_OFF + S_litcorr)
	    c = RADAR_FLOOR;
	else if(glyph <= GLYPH_CMAP_OFF + S_dnladder)
	    c = RADAR_LADDER;
	else if(glyph <= GLYPH_CMAP_OFF + S_pool)
	    c = RADAR_WATER;
	else if(glyph <= GLYPH_CMAP_OFF + S_ice)
	    c = RADAR_ICE;	/* fountain sink ice */
	else if(glyph <= GLYPH_CMAP_OFF + S_lava)
	    c = RADAR_LAVA;	/* lava */
	else if(glyph <= GLYPH_CMAP_OFF + S_hcdbridge)
	    c = RADAR_BRIDGE;	/* bridge */
	else if(glyph <= GLYPH_CMAP_OFF + S_air)
	    c = RADAR_AIR;	/* air */
	else if(glyph <= GLYPH_CMAP_OFF + S_cloud)
	    c = RADAR_CLOUD;	/* cloud */
	else if(glyph <= GLYPH_CMAP_OFF + S_water)
	    c = RADAR_WATER;	/* water */
	else if(glyph < GLYPH_CMAP_OFF + S_explode9)
	    c = RADAR_BEAM;	/* beam */
    }
    else
	c = RADAR_WALL;

    gdk_draw_rectangle(
	radar_pixmap, map_color_gc[c],
	TRUE, 
	x * NH_RADAR_UNIT, y * NH_RADAR_UNIT,
	NH_RADAR_UNIT, NH_RADAR_UNIT);
}
#endif	/* RADAR */

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

static void
nh_map_print_glyph_traditional(XCHAR_P x, XCHAR_P y, struct tilemap *tmap)
{
    static GdkRectangle update_rect;
    int color;
    int glyph = tmap->glyph;
    int offset;
    GdkWChar ch[2];

    color = 0;

    if ((offset = (glyph - GLYPH_SWALLOW_OFF)) >= 0) {	/* swallow */
	ch[0] = (uchar) showsyms[S_sw_tl + (offset & 0x7)];
	color = mon_color(offset>>3);
    }
    else if ((offset = (glyph - GLYPH_ZAP_OFF)) >= 0) {	/* zap beam */
	ch[0] = showsyms[S_vbeam + (offset & 0x3)];
	color = zap_color(offset>>2);
    }
    else if ((offset = (glyph - GLYPH_CMAP_OFF)) >= 0) {/* cmap */
	ch[0] = showsyms[offset];
	color = cmap_color(offset);
    }
    else if ((offset = (glyph - GLYPH_OBJ_OFF)) >= 0) {	/* object */
	ch[0] = oc_syms[(int)objects[offset].oc_class];
	color = obj_color(offset);
    }
    else if ((offset = (glyph - GLYPH_BODY_OFF)) >= 0) {/* a corpse */
	ch[0] = oc_syms[(int)objects[CORPSE].oc_class];
	color = mon_color(offset);
    }
    else if ((offset = (glyph - GLYPH_PET_OFF)) >= 0) {	/* a pet */
	ch[0] = monsyms[(int)mons[offset].mlet];
	color = pet_color(offset);
    }
    else{						/* a monster */
	ch[0] = monsyms[(int)mons[glyph].mlet];
	color = mon_color(glyph);
    }
    
    ch[1] = '\0';
    
    gdk_draw_rectangle(xshm_map_pixmap,
      map->style->bg_gc[GTK_WIDGET_STATE(map)],
      TRUE, x * c_width, y * c_height -  map_font->ascent, c_width, c_height);
    
    gdk_draw_text_wc(
	xshm_map_pixmap, map_font,
	copts.use_color ? map_color_gc[color] : map->style->fg_gc[GTK_WIDGET_STATE(map)],
	x * c_width + map_xoffsets[ch[0]], y * c_height, ch, 1);
    
    if (glyph_is_pet(glyph) && copts.hilite_pet) {
	gdk_draw_rectangle(xshm_map_pixmap,
	    map_color_gc[copts.use_color ? CLR_RED : CLR_WHITE],
	    FALSE, x * c_width, y * c_height - map_font->ascent,
	    c_width - 1, c_height - 1);
    }
    else if (x == cursx && y == cursy)
	gdk_draw_rectangle(xshm_map_pixmap,
	    map_color_gc[CLR_WHITE], FALSE,
	    x * c_width, y * c_height - map_font->ascent,
	    c_width - 1, c_height - 1);

    update_rect.x = x * c_width;
    update_rect.y = y * c_height -  map_font->ascent;
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
    int k, stone = glyph2tile[cmap_to_glyph(S_stone)];

    if (Tile->transparent) {
	if (dosurface)
	    x_tile_tmp_draw_tile(tmap->tiles[no_layers - 1], ofsx, ofsy);
	else
	    for(k = no_layers - 2; k >= 0; k--)
		if (tmap->tiles[k] != stone)
		    x_tile_tmp_draw_tile(tmap->tiles[k], ofsx, ofsy);
    } else if (!dosurface) {
	for(k = 0; k < no_layers - 1; k++)
	    if (tmap->tiles[k] != stone)
		break;
	x_tile_tmp_draw_tile(tmap->tiles[k], ofsx, ofsy);
    }
    if (!dosurface)
	if (glyph_is_pet(tmap->glyph) && copts.hilite_pet)
	    x_tile_tmp_draw_rectangle(ofsx, ofsy, CLR_RED);
	else if (tmap == GTKMAP(cursy, cursx))
	    x_tile_tmp_draw_rectangle(ofsx, ofsy, MAP_WHITE);
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
	    if (x < COLNO - 1)
		NH_MAP_PRINT_GLYPH_TMP(y, x, 0, 1, k);
	    if (y < ROWNO - 1) {
		NH_MAP_PRINT_GLYPH_TMP(y, x, 1, 0, k);
		if (x < COLNO - 1)
		    NH_MAP_PRINT_GLYPH_TMP(y, x, 1, 1, k);
	    }
	}
    }
    else {
	NH_MAP_PRINT_GLYPH_TMP(y, x, 0, 0, 1);
	NH_MAP_PRINT_GLYPH_TMP(y, x, 0, 0, 0);
    }

    x_tile_draw_tmp(x * c_3dwidth + (ROWNO - y) * c_3dofset, y * c_3dheight);
}

static void
nh_map_print_glyph_simple_tile(XCHAR_P x, XCHAR_P y, struct tilemap *tmap)
{
    int k;
    int stone = glyph2tile[cmap_to_glyph(S_stone)];
    for(k = 0; k < no_layers - 1; k++)
	if (tmap->tiles[k] != stone)
	    break;
    x_tile_draw_tile(tmap->tiles[k], x * c_width, y * c_height);
    if (glyph_is_pet(tmap->glyph) && copts.hilite_pet)
	x_tile_draw_rectangle(x * c_width, y * c_height, &nh_color[CLR_RED]);
    else if (x == cursx && y == cursy)
	x_tile_draw_rectangle(x * c_width, y * c_height, &nh_color[MAP_WHITE]);
}

static void
nh_map_print_glyph(XCHAR_P x, XCHAR_P y, struct tilemap *tmap)
{
#ifdef RADAR
    nh_print_radar(x, y, tmap->glyph);
#endif
    
    if (map_visual == 0)
	nh_map_print_glyph_traditional(x, y, tmap);
    else if (!Tile->transparent && !Tile->spread)
	nh_map_print_glyph_simple_tile(x, y, tmap);
    else
	nh_map_print_glyph_tile(x, y, tmap);
}

void
GTK_ext_print_glyph_layered(winid id, int x, int y, int ng, int *glyphs)
{
    int k, tile;
    int stone = glyph2tile[cmap_to_glyph(S_stone)];

    if (ng != no_layers)
	nh_conf_map_layers(ng);
    for(k = 0; k + 1 < ng; k++)
	if (glyphs[k] != NO_GLYPH)
	    break;
    if (GTKMAP(y, x)->glyph != glyphs[k] || GTKMAP(y, x)->flags != 0) {
	GTKMAP(y, x)->glyph = glyphs[k];
	GTKMAP(y, x)->flags = TILEMAP_UPDATE;
	map_update = 1;
    }
    for (k = 0; k < no_layers; k++) {
	tile = glyphs[k] == NO_GLYPH ? stone : glyph2tile[glyphs[k]];
	if (GTKMAP(y, x)->tiles[k] != tile) {
	    GTKMAP(y, x)->tiles[k] = tile;
	    GTKMAP(y, x)->flags |= TILEMAP_UPDATE;
	    map_update = 1;
	}
    }
}

void
GTK_ext_print_glyph(winid id, int x, int y, int glyph)
{
    int k;
    int stone = glyph2tile[cmap_to_glyph(S_stone)];

    if (GTKMAP(y, x)->glyph != glyph || GTKMAP(y, x)->flags != 0) {
	GTKMAP(y, x)->glyph = glyph;
	GTKMAP(y, x)->flags = TILEMAP_UPDATE;
	map_update = 1;
    }
    if (GTKMAP(y, x)->tiles[0] != glyph2tile[glyph]) {
	GTKMAP(y, x)->tiles[0] = glyph2tile[glyph];
	GTKMAP(y, x)->flags = TILEMAP_UPDATE;
	map_update = 1;
    }
    for (k = 1; k < no_layers; k++)
	if (GTKMAP(y, x)->tiles[k] != stone) {
	    GTKMAP(y, x)->tiles[k] = stone;
	    GTKMAP(y, x)->flags |= TILEMAP_UPDATE;
	    map_update = 1;
	}
}

void
GTK_curs(winid id, int x, int y)
{
    if(id != NHW_MAP)
	return;

    if (cursx != x || cursy != y) {
	map_update = 1;
	GTKMAP(y, x)->flags |= TILEMAP_UPDATE;
	GTKMAP(cursy, cursx)->flags |= TILEMAP_UPDATE;
	cursx = x;
	cursy = y;
	nh_map_flush();
    }
}

#ifdef RADAR
void
nh_radar_update()
{
    GdkRectangle update_rect;
    GtkAdjustment *hadj, *vadj;
    boolean use_radar = nh_option_cache_get_bool("radar");

    if (use_radar && !radar_is_popuped) {
	if(radar_is_created == 0)
	    nh_radar_new();

	gtk_widget_show_all(radar);
	radar_is_popuped= 1;
    } else if (!use_radar && radar_is_popuped) {
	gtk_widget_hide_all(radar);
	radar_is_popuped= 0;
    }
    
    if (radar_is_popuped && map) {
	hadj = xshm_map_get_hadjustment();
	vadj = xshm_map_get_vadjustment();

	gdk_draw_pixmap(
	    radar_pixmap2, map_gc,
	    radar_pixmap, 0, 0, 0, 0, 
	    NH_RADAR_WIDTH, NH_RADAR_HEIGHT);
	
	gdk_draw_rectangle(
	    radar_pixmap2, map_color_gc[CLR_WHITE],
	    FALSE,
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
#endif	/* RADAR */

#define DIFF(a, b)	((a) >= (b) ? (a) - (b) : (b) - (a))

static void
nh_map_cliparound(int x, int y, gboolean exact)
{
    static int sx = -1, sy;
    if (!exact && sx >= 0 && DIFF(x, sx) <= 1 && DIFF(y, sy) <= 1)
	return;
    sx = x;
    sy = y;
    if (map)
	xshm_map_cliparound(x * c_3dwidth + (ROWNO - y) * c_3dofset,
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

    tile = fix_tile(glyph2tile[glyph]);

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
    for(j = 0; j < ROWNO; j++)
	for(i = 0; i < COLNO; i++)
	    GTKMAP(j, i)->flags |= TILEMAP_UPDATE;
    nh_map_flush();
}

void
nh_map_flush()
{
    int i, j;

    if (!xshm_map_pixmap && !xshm_map_image && !xshm_map_pixbuf)
	return;		/* Map not realized yet - pend flush */

    if(map_update != 0){
	map_update = 0;

	for(j = 0; j < ROWNO; j++)
	    for(i = 0; i < COLNO; i++)
		if (GTKMAP(j, i)->flags & TILEMAP_UPDATE) {
		    GTKMAP(j, i)->flags &= ~TILEMAP_UPDATE;
		    nh_map_print_glyph(i, j, GTKMAP(j, i));
		}
	nh_map_check_visibility();
	xshm_map_flush();
#ifdef RADAR
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

/*
  $Id: gtkmap.c,v 1.16 2000-11-10 17:59:36 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
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

/*
  if map_click is true, we do gtk_main_quit() when clicking map
 */
static int		map_click;
static int		map_update;

static GtkWidget	*map_scroll;
static GtkWidget	*map;
static GdkPixmap	*map_pixmap;
static GdkFont		*map_font;

#ifdef WINGTK_X11
static Display		*display;
static int		screen;
static int		scr_depth;
#endif

static GdkGC		*map_color_gc[N_NH_COLORS];

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

#define RADAR_MONSTER	MAP_YELLOW
#define RADAR_HUMAN	MAP_WHITE
#define RADAR_PET	MAP_GREEN
#define RADAR_OBJECT	MAP_BLUE

#define RADAR_WALL	MAP_GRAY
#define RADAR_FLOOR	MAP_DARK_GREEN
#define RADAR_DOOR	MAP_ORANGE
#define RADAR_LADDER	MAP_MAGENTA
#define RADAR_WATER	MAP_BLUE
#define RADAR_TRAP	MAP_RED
#define RADAR_SWALLOW	MAP_RED
#define RADAR_ICE	MAP_GRAY
#define RADAR_LAVA	MAP_ORANGE
#define RADAR_BRIDGE	MAP_GRAY
#define RADAR_AIR	MAP_CYAN
#define RADAR_CLOUD	MAP_GRAY
#define RADAR_BEAM	MAP_YELLOW

#endif

static GdkPixmap *tile_pixmap;
static GdkBitmap *tile_mask;
static GdkImage  *tile_image;
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

static char	*NH_TILE_FILE;
static int	NH_TILE_3D_WIDTH;
static int	NH_TILE_3D_HEIGHT;
static int	NH_TILE_3D_OFSET;
static int	NH_TILE_3D_OFSETY;
static int	NH_TILE_WIDTH;
static int	NH_TILE_HEIGHT;
static int	NH_TILE_CACHE_WIDTH;
static int	NH_TILE_CACHE_HEIGHT;
static int	NH_MAP_WIDTH;
static int	NH_MAP_HEIGHT;
static int	NH_TILEMAP_WIDTH;
static int	NH_TILEMAP_HEIGHT;

static int	NH_MAP_MAX_WIDTH;
static int	NH_MAP_MAX_HEIGHT;

#define FLOOR_SRC_X (glyph2tile[cmap_to_glyph(S_room)] % tiles_per_row * \
	NH_TILE_WIDTH)
#define FLOOR_SRC_Y (glyph2tile[cmap_to_glyph(S_room)] / tiles_per_row * \
	NH_TILE_HEIGHT)

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

static struct tilemap{
    int glyph;
    int	bgtile;
    int tile;
    int update;
} gtkmap[ROWNO][COLNO];

#ifdef WIN32 
/* 
 * Windows systems don't have the expected fonts 
 */
#define	NH_FONT		"-*-Courier New-normal-r-normal--20-*-*-*-*-*-*-*"
#define	NH_FONT2	"-*-Fixedsys-normal-r-normal--20-*-*-*-*-*-*-*"
#define	NH_FONT3	"-*-Terminal-normal-r-normal--20-*-*-*-*-*-*-*"
#else
#define	NH_FONT		"nh10"
#define	NH_FONT2	"-misc-fixed-medium-r-normal--20-*-*-*-*-*-iso8859-1"
#define	NH_FONT3	"fixed"	
#endif

static void	nh_map_init();

/*
  zero: character
  else: various tiles
 */
void
nh_set_map_visual(int mode)
{
    static int setting_visual = FALSE;		/* Ignore recursive calls */
    int saved_vis = map_visual;

    if (setting_visual)
	return;
    setting_visual++;

    if(mode < 0 || mode > no_tileTab)
	panic("Bad visual!\n");

    if(saved_vis != mode){
	gtk_widget_hide(map);

	nh_map_clear();

	if(saved_vis > 0)
	    gdk_image_destroy(tile_image);
	if(mode == 0){
	    int i, width, min_width;
	    c_width = min_width = gdk_char_width(map_font, oc_syms[0]);
	    for(i = 1; i < SIZE(oc_syms); i++) {
		width = gdk_char_width(map_font, oc_syms[i]);
		if (width < min_width)
		    min_width = width;
		if (width > c_width)
		    c_width = width;
	    }
	    for(i = 0; i < SIZE(showsyms); i++) {
		width = gdk_char_width(map_font, showsyms[i]);
		if (width < min_width)
		    min_width = width;
		if (width > c_width)
		    c_width = width;
	    }
	    for(i = 0; i < SIZE(monsyms); i++) {
		width = gdk_char_width(map_font, monsyms[i]);
		if (width < min_width)
		    min_width = width;
		if (width > c_width)
		    c_width = width;
	    }
	    for(i = 0; i < SIZE(warnsyms); i++) {
		width = gdk_char_width(map_font, warnsyms[i]);
		if (width < min_width)
		    min_width = width;
		if (width > c_width)
		    c_width = width;
	    }
	    if (min_width <= 0)
		pline("Warning: Not all expected glyphs present in map font.");
	    else if (min_width < c_width)
		/* Variable width fonts work after a fashion, but since each
		 * character is not centered in its cell, they look somewhat
		 * odd. If we want to add proper support for these fonts then
		 * nh_map_print_glyph_traditional() should center the glyphs.
		 */
		pline("Warning: Map font is not fixed width.");

	    c_height = map_font->ascent + map_font->descent;
	    c_3dwidth = c_width;
	    c_3dheight = c_height;
	    c_3dofset = 0;

	    c_map_width = COLNO * c_width;
	    c_map_height = ROWNO * c_height;
	}
	else{
	    if(!tileTab[mode].ident[0])
		panic("Disabled visual!\n");
	    Tile = tileTab + mode;
	    nh_map_init();

	    tile_pixmap = gdk_pixmap_create_from_xpm(
		main_window->window,
		&tile_mask,
		&nh_color[MAP_MAGENTA],
		NH_TILE_FILE
		);
	    
	    if (!tile_pixmap)
		panic("Cannot open tile file %s!",NH_TILE_FILE);

	    tile_image = gdk_image_get(
		(GdkWindow *)tile_pixmap,
		0, 0,
		NH_TILEMAP_WIDTH, NH_TILEMAP_HEIGHT);
	    
	    x_tile_init(tile_image, Tile);
	    
	    gdk_pixmap_unref(tile_pixmap);
	    gdk_bitmap_unref(tile_mask);

	    c_width = NH_TILE_WIDTH;
	    c_height = NH_TILE_HEIGHT;
	    c_3dwidth = NH_TILE_3D_WIDTH;
	    c_3dheight = NH_TILE_3D_HEIGHT;
	    c_3dofset = NH_TILE_3D_OFSET;

	    c_map_width = NH_MAP_WIDTH;
	    c_map_height = NH_MAP_HEIGHT;
	}
	
	gtk_drawing_area_size(
	    GTK_DRAWING_AREA(map),
	    c_map_width, c_map_height);
	
	nh_map_check_visibility();
#ifdef RADAR
	nh_radar_update();
#endif
	map_visual = mode;
	strcpy(tileset, tileTab[map_visual].ident);
	doredraw();

	gtk_widget_show(map);
    }

    setting_visual--;
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

/*
  fix tile number
 */
static int
fix_tile(int tile)
{
    return tile;
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
    flags.radar = 0;

    return TRUE;
}

#endif

static gint
map_button_event(GtkWidget *widget, GdkEventButton *event)
{
    GTK_curs(NHW_MAP, event->x / c_width, event->y / c_height);
    if (event->button == 1)
	cursm = CLICK_1;
    else
	cursm = CLICK_2;

    if(map_click)
	gtk_main_quit();
    return FALSE;
}

static gint
map_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    if(map_visual == 0){
	gdk_draw_pixmap(
	    widget->window,
	    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
	    map_pixmap,
	    event->area.x, event->area.y,
	    event->area.x, event->area.y,
	    event->area.width, event->area.height);
    }
    else
	xshm_map_draw(
	    widget->window,
	    event->area.x, event->area.y,
	    event->area.x, event->area.y,
	    event->area.width, event->area.height);
    
    return FALSE;
}

void
nh_map_check_visibility()
{
    GtkAdjustment *adj;
    int width, height;
    int x, y;
    int adjx, adjy;

    x = cursx;
    y = cursy;

    if(map_scroll){
	width = map_scroll->allocation.width;
	height = map_scroll->allocation.height;

	adj = gtk_scrolled_window_get_vadjustment(
	    GTK_SCROLLED_WINDOW(map_scroll)
	    );
	if(map_visual == 0)
	    adjy = (y - (ROWNO/2)) * c_3dheight;
	else
	    adjy = y * c_3dheight - height/2;

	if(adjy > c_map_height - adj->page_size)
	    adjy = c_map_height - adj->page_size;

	gtk_adjustment_set_value(adj, adjy);
	
	adj = gtk_scrolled_window_get_hadjustment(
	    GTK_SCROLLED_WINDOW(map_scroll)
	    );
	if(map_visual == 0)
	    adjx = (x - (COLNO/2)) * c_3dwidth;
	else
	    adjx = x * c_3dwidth - width/2 + (ROWNO - y) * c_3dofset;

	if(adjx > c_map_width - adj->page_size)
	    adjx = c_map_width - adj->page_size;

	gtk_adjustment_set_value(adj, adjx);
    }
}

void
nh_map_clear()
{
#ifdef RADAR
    GdkRectangle update_rect;
#endif
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
		return;		/* done (setting visual clears map) */
	    }
	pline("Tileset %s not valid.", tileset);
	strcpy(tileset, tileTab[map_visual].ident);
    }

    if(map_visual == 0){
	gdk_draw_rectangle(
	    map_pixmap, map->style->black_gc,
	    TRUE, 0, 0, c_map_width, c_map_height);
	 
	gdk_draw_pixmap(
	    map->window,
	    map->style->fg_gc[GTK_WIDGET_STATE(map)],
	    map_pixmap,
	    0, 0,
	    0, 0,
	    c_map_width, c_map_height);
    }
    else{
	gdk_draw_rectangle(
	    map_pixmap, map->style->black_gc,
	    TRUE, 0, 0, c_map_width, c_map_height);
	
	gdk_draw_pixmap(
	    map->window,
	    map->style->fg_gc[GTK_WIDGET_STATE(map)],
	    map_pixmap,
	    0, 0,
	    0, 0,
	    c_map_width, c_map_height);
    }
#ifdef RADAR
    gdk_draw_rectangle(
	radar_pixmap, map->style->black_gc,
	TRUE, 0, 0, NH_RADAR_WIDTH, NH_RADAR_HEIGHT);

    update_rect.x = 0;
    update_rect.y = 0;
    update_rect.width = NH_RADAR_WIDTH;
    update_rect.height = NH_RADAR_HEIGHT;

    gtk_widget_draw(radar, &update_rect);
    if(radar_is_popuped)
	gdk_window_raise(radar->window);
#endif

    {
	int i, j;

	xshm_map_clear();

	for(i=0 ; i<ROWNO ; ++i)
	    for(j=0 ; j<COLNO ; ++j){
		gtkmap[i][j].tile = fix_tile(glyph2tile[cmap_to_glyph(S_stone)]);
		gtkmap[i][j].bgtile = fix_tile(glyph2tile[cmap_to_glyph(S_stone)]);
	    }
    }
}

static void
nh_map_init()
{
#ifndef FILE_AREAS
    NH_TILE_FILE = 		(Tile->file);
#else
    NH_TILE_FILE = 		make_file_name(FILE_AREA_SHARE, Tile->file);
#endif
    NH_TILE_3D_WIDTH = 		(Tile->unit_width - Tile->ofsetx_3d);
    NH_TILE_3D_HEIGHT = 	(Tile->unit_height - Tile->ofsety_3d);
    NH_TILE_3D_OFSET =		(Tile->ofsetx_3d);
    NH_TILE_3D_OFSETY = 	(Tile->ofsety_3d);
    NH_TILE_WIDTH = 		(Tile->unit_width);
    NH_TILE_HEIGHT = 		(Tile->unit_height);
    NH_TILE_CACHE_WIDTH =	(Tile->unit_width);
    NH_TILE_CACHE_HEIGHT =	(Tile->unit_height);
    NH_MAP_WIDTH =		(Tile->unit_width * COLNO);
    NH_MAP_HEIGHT =		(Tile->unit_height * ROWNO);
    NH_TILEMAP_WIDTH =		(Tile->tilemap_width);
    NH_TILEMAP_HEIGHT =		(Tile->tilemap_height);
}

void
nh_map_destroy()
{
    map_scroll = NULL;
}

/*
 * [ALI]
 *
 * Scan an XPM tile file for its width and height and fill in the
 * tile table appropriately. We could use functions from the XPM
 * library to do this but that would mean reading the whole file
 * into memory. This would take a significant amount of time. The
 * XPM format is loosely defined, but this should parse anything
 * that sxpm can. It won't reject all invalid XPM files, however.
 */

static void
tile_scan(void)
{
    int i, v, ch;
    int state;
    FILE *fp;

    v = 1;	/* Index into tileTab[] array */
    for(i=0 ; i < no_tilesets ; ++i){
	if ((tilesets[i].flags & ~(TILESET_TRANSPARENT | TILESET_PSEUDO3D)) != 0)
	    continue;	/* Unsupported flag set */
#ifdef FILE_AREAS
	fp = fopen_datafile_area(FILE_AREA_SHARE, tilesets[i].file, RDTMODE, FALSE);
#else
	fp = fopen_datafile(tilesets[i].file, RDTMODE, FALSE);
#endif
	if(!fp)
	    continue;
	state = 0;
	do
	{
	    ch = getc(fp);
	    if (ch == EOF)
	    {
		state = 5;
		break;
	    }
	    switch (state)
	    {
		case 0:
		    if (ch == '/')
			state++;
		    else if (ch == '"')
			state = 4;
		    break;
		case 1:
		    if (ch == '*')
			state++;
		    else
			state--;
		    break;
		case 2:
		    if (ch == '*')
			state++;
		    break;
		case 3:
		    if (ch == '/')
			state = 0;
		    else
			state--;
		    break;
	    }
	} while(state < 4);
	if(state == 5)
	    continue;
	tileTab[v].ident = tilesets[i].name;
	tileTab[v].file = tilesets[i].file;
	if(fscanf(fp, "%d %d", &tileTab[v].tilemap_width,
	  &tileTab[v].tilemap_height) != 2)
	    continue;
	tileTab[v].unit_width = tileTab[v].tilemap_width / tiles_per_row;
	tileTab[v].unit_height = tileTab[v].tilemap_height / tiles_per_col;
	if(tilesets[i].flags & TILESET_PSEUDO3D){
	    tileTab[v].spread = TRUE;
	    tileTab[v].ofsetx_3d = tileTab[v].unit_width / 3;
	    tileTab[v].ofsety_3d = tileTab[v].unit_height / 2;
	}
	else{
	    tileTab[v].spread = FALSE;
	    tileTab[v].ofsetx_3d = 0;
	    tileTab[v].ofsety_3d = 0;
	}
	tileTab[v].transparent = !!(tilesets[i].flags & TILESET_TRANSPARENT);
	fclose(fp);
	v++;
    }
    no_tileTab = v - 1;
    map_visual = -1;
    for (v = 1; v <= no_tileTab; v++){
	if (!strcmp(tileset, tileTab[v].ident)){
	    map_visual = v;
	    break;
	}
    }
    /*
     * [ALI]
     *
     * Default to the first valid tile set
     */
    if (map_visual < 0 && no_tileTab > 0) {
	if (tileset[0])
	    pline("Warning: Tile set \"%s\" not supported.", tileset);
	else
	    pline("Warning: Can't start in character mode.");
	map_visual = 1;
	strcpy(tileset, tileTab[map_visual].ident);
    }
    /*
     * [ALI]
     *
     * If no tiles valid, we'd like to use character mode but it
     * requires too many changes to nh_map_init and nh_map_new.
     * For now, we just abort.
     */
    if (map_visual < 0)
	panic("No valid tiles found");
    Tile = tileTab + map_visual;
}

GtkWidget *
nh_map_new(GtkWidget *w)
{
    int i/*, n*/;
    int width, height;

    tile_scan();
    nh_map_init();

    NH_MAP_MAX_WIDTH = -1;
    NH_MAP_MAX_HEIGHT = -1;
    for(i=1 ; i <= no_tileTab; ++i){

	width = (COLNO + 1) * (tileTab[i].unit_width - tileTab[i].ofsetx_3d)
	    + (ROWNO + 1) * tileTab[i].ofsetx_3d;
	height = (ROWNO + 1) * (tileTab[i].unit_height - tileTab[i].ofsety_3d);

	if(NH_MAP_MAX_WIDTH < width)
	    NH_MAP_MAX_WIDTH = width;
	if(NH_MAP_MAX_HEIGHT < height)
	    NH_MAP_MAX_HEIGHT = height;
    }

#ifdef WINGTK_X11
    display = GDK_DISPLAY();
    screen = DefaultScreen(display);
    scr_depth = DefaultDepth(display, screen);
#endif

    map_scroll = gtk_scrolled_window_new(
	NULL, NULL);

    gtk_scrolled_window_set_policy(
	GTK_SCROLLED_WINDOW(map_scroll),
	GTK_POLICY_AUTOMATIC,
	GTK_POLICY_AUTOMATIC);

    map = gtk_drawing_area_new();
    gtk_widget_set_name(map, DEF_GAME_NAME " map");
    gtk_widget_set_rc_style(map);

    gtk_signal_connect(GTK_OBJECT(map), "expose_event",
		       GTK_SIGNAL_FUNC(map_expose_event), NULL);

    gtk_signal_connect(GTK_OBJECT(map), "button_press_event",
		       GTK_SIGNAL_FUNC(map_button_event), NULL);

    gtk_widget_set_events(map, GDK_EXPOSURE_MASK
			  | GDK_BUTTON_PRESS_MASK);


#ifdef WINGTK_X11
    xshm_init(display);
#else
    xshm_init(w->window);
#endif
    (void) xshm_map_init(NH_MAP_MAX_WIDTH, NH_MAP_MAX_HEIGHT);


    if (map->style && map->style->font)
	map_font = map->style->font;
    else
	map_font = gdk_font_load(NH_FONT);
    if(!map_font){
/*	fprintf(stderr, "warning: cannot load %s. try to load %s", NH_FONT, NH_FONT2);*/
	map_font = gdk_font_load(NH_FONT2);
    }

    if(!map_font){
	fprintf(stderr, "warning: cannot load %s. try to load %s", NH_FONT2, NH_FONT3);
	map_font = gdk_font_load(NH_FONT3);
    }

    if(!map_font)
	panic("Cannot open map font!");

    map_gc = gdk_gc_new(w->window);

    i = map_visual;
    map_visual = -1;			/* Force initialisation */
    nh_set_map_visual(i);

    map_pixmap = gdk_pixmap_new(
	w->window,
	NH_MAP_MAX_WIDTH, NH_MAP_MAX_HEIGHT, -1);

    gtk_scrolled_window_add_with_viewport(
        GTK_SCROLLED_WINDOW(map_scroll),
        map);

/*
  set map area size
  */
    width = NH_MAP_MAX_WIDTH + 22;
    height = NH_MAP_MAX_HEIGHT + 22;

    if(width >= root_width)
	width = root_width - 50;

    if(height >= (root_height / 2))
	height = (root_height / 2) - 50;

    gtk_widget_set_usize(GTK_WIDGET(map_scroll), width, height);
    
/*
  set gc 
 */
    for(i=0 ; i < N_NH_COLORS ; ++i){
	map_color_gc[i] = gdk_gc_new(w->window);
	gdk_gc_set_foreground(map_color_gc[i], &nh_color[i]);
	gdk_gc_set_background(map_color_gc[i], &nh_color[MAP_BACKGROUND]);
    }

    return map_scroll;
}

/*
  create radar
 */

#ifdef RADAR
GtkWidget *
nh_radar_new()
{
    radar = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_accel_group_attach(accel_group, GTK_OBJECT(radar));
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
#endif

#ifdef RADAR
void
nh_print_radar(int x, int y, int glyph)
{
    int c;
    /*
      int tile = glyph2tile[glyph];
      */
    c = MAP_BLACK;

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
#endif

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
nh_map_print_glyph_traditional(XCHAR_P x, XCHAR_P y, struct tilemap *tmap, GdkRectangle *rect)
{
    static GdkRectangle update_rect;
    int color;
    int glyph = tmap->glyph;
    int offset;
    gchar ch[2];

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
    
    update_rect.x = x * c_width;
    update_rect.y = y * c_height - map_font->ascent;
    update_rect.width = c_width;
    update_rect.height = c_height;

#ifdef TEXTCOLOR
    gdk_draw_rectangle(
	map_pixmap, map_color_gc[iflags.use_color?MAP_BACKGROUND:MAP_BLACK],
	TRUE, x * c_width, y * c_height -  map_font->ascent, c_width, c_height);
    
    gdk_draw_text(
	map_pixmap, map_font,
	map_color_gc[iflags.use_color?color:MAP_WHITE],
	x * c_width, y * c_height, ch, 1);
    
    if(glyph_is_pet(glyph) && iflags.hilite_pet){
	gdk_draw_rectangle(
	    map_pixmap,
	    map_color_gc[iflags.use_color?MAP_RED:MAP_WHITE],
	    FALSE,
	    x * c_width, y * c_height - map_font->ascent,
	    c_width - 1, c_height - 1
	    );
    }
#else
    gdk_draw_rectangle(
	map_pixmap, map->style->bg_gc,
	TRUE, x * c_width, y * c_height - map_font->ascent, c_width, c_height);
    
    gdk_draw_text(
	map_pixmap, map_font,
	map->style->black_gc,
	x*c_width, y*c_height, ch, 1);
#endif
    if(rect)
	*rect = update_rect;
}

/*
  flag	FALSE:	draw tile
  	TRUE:	draw bgtile
 */
static void
nh_map_print_glyph_tmp(struct tilemap *tmap, int ofsx, int ofsy, int flag)
{
    int glyph = tmap->glyph;
    int tile = tmap->tile;
    int bgtile = tmap->bgtile;
    int src_x, src_y;
    int bgsrc_x, bgsrc_y;

    src_x = (tile % tiles_per_row) * c_width;
    src_y = (tile / tiles_per_row) * c_height;
    
    bgsrc_x = (bgtile % tiles_per_row) * c_width;
    bgsrc_y = (bgtile / tiles_per_row) * c_height;

    if(!Tile->transparent){
	if(flag == FALSE)
	    x_tile_tmp_draw(src_x, src_y, ofsx, ofsy);
    }
    else if(flag == FALSE){
	if(bgtile != tile)
	    x_tile_tmp_draw(src_x, src_y, ofsx, ofsy);
    }
    else{
	x_tile_tmp_draw(FLOOR_SRC_X, FLOOR_SRC_Y, ofsx, ofsy);
	x_tile_tmp_draw(bgsrc_x, bgsrc_y, ofsx, ofsy);
    }

    if(glyph_is_pet(glyph)
#ifdef TEXTCOLOR
	&& iflags.hilite_pet
#endif
	){
	x_tile_tmp_draw_rectangle(ofsx, ofsy, nh_color[MAP_RED].pixel);
    }
}

static void
nh_map_print_glyph_tile(XCHAR_P x, XCHAR_P y, struct tilemap *tmap, GdkRectangle *rect)
{
    static GdkRectangle update_rect;

    x_tmp_clear();

    if(Tile->spread){
/*
  draw bgtiles
  */
	if(x > 0 && y > 0){
	    nh_map_print_glyph_tmp(
		&gtkmap[y-1][x-1],
		-c_3dwidth + c_3dofset, -c_3dheight, 1);
	}
	if(y > 0){
	    nh_map_print_glyph_tmp(
		&gtkmap[y-1][x],
		c_3dofset, -c_3dheight, 1);
	}
/*
  draw tiles
 */
	if(x > 0 && y > 0){
	    nh_map_print_glyph_tmp(
		&gtkmap[y-1][x-1],
		-c_3dwidth + c_3dofset, -c_3dheight, 0);
	}
	if(y > 0){
	    nh_map_print_glyph_tmp(
		&gtkmap[y-1][x],
		c_3dofset, -c_3dheight, 0);
	}
    }
/*
  draw bgtiles
  */
    if(Tile->spread){
	if(x > 0){
	    nh_map_print_glyph_tmp(
		&gtkmap[y][x-1],
		-c_3dwidth, 0, 1);
	}
    }
    
    nh_map_print_glyph_tmp(&gtkmap[y][x], 0, 0, 1);
    
    if(Tile->spread){
	if(x < COLNO - 1){
	    nh_map_print_glyph_tmp(
		&gtkmap[y][x+1],
		c_3dwidth, 0, 1);
	}
    }
/*
  draw tiles
  */
    if(Tile->spread){
	if(x > 0){
	    nh_map_print_glyph_tmp(
		&gtkmap[y][x-1],
		-c_3dwidth, 0, 0);
	}
    }
    
    nh_map_print_glyph_tmp(&gtkmap[y][x], 0, 0, 0);
    
    if(Tile->spread){
	if(x < COLNO - 1){
	    nh_map_print_glyph_tmp(
		&gtkmap[y][x+1],
		c_3dwidth, 0, 0);
	}
    }
/*
  draw bgtiles
  */
    if(Tile->spread){
	if(y < ROWNO - 1){
	    nh_map_print_glyph_tmp(
		&gtkmap[y+1][x],
		-c_3dofset, c_3dheight, 1);
	}
	if(x < COLNO - 1 && y < ROWNO - 1){
	    nh_map_print_glyph_tmp(
		&gtkmap[y+1][x+1],
		c_3dwidth - c_3dofset, c_3dheight, 1);
	}
/*
  draw tiles
  */
	if(y < ROWNO - 1){
	    nh_map_print_glyph_tmp(
		&gtkmap[y+1][x],
		-c_3dofset, c_3dheight, 0);
	}
	if(x < COLNO - 1 && y < ROWNO - 1){
	    nh_map_print_glyph_tmp(
		&gtkmap[y+1][x+1],
		c_3dwidth - c_3dofset, c_3dheight, 0);
	}
    }
    
    xshm_map_tile_draw(
	x * c_3dwidth + (ROWNO - y) * c_3dofset,
	y * c_3dheight);

    update_rect.x = x * c_3dwidth + (ROWNO - y) * c_3dofset;
    update_rect.y = y * c_3dheight;
    update_rect.width = c_width;
    update_rect.height = c_height;

    if(rect)
	*rect = update_rect;
}
    
static void
nh_map_print_glyph(XCHAR_P x, XCHAR_P y, struct tilemap *tmap, GdkRectangle *rect)
{
    int glyph = tmap->glyph;

#ifdef RADAR
    nh_print_radar(x, y, glyph);
#endif
    
    if(map_visual == 0)
	nh_map_print_glyph_traditional(x, y, tmap, rect);
    else
	nh_map_print_glyph_tile(x, y, tmap, rect);
}

void
GTK_print_glyph(winid id, XCHAR_P x, XCHAR_P y, int glyph)
{
    struct rm *lev = &levl[x][y];
    int tile;
    int bgtile;
    int bg = lev->glyph;

    tile = fix_tile(glyph2tile[glyph]);
    if(Blind || (viz_array && !cansee(x, y))){

	if(glyph_is_object(bg)){
	    if(!lev->waslit)
		bg = cmap_to_glyph(S_stone);
	    else
		bg = back_to_glyph(x, y);
	}
	bgtile = fix_tile(glyph2tile[bg]);
    }
    else
	bgtile = fix_tile(glyph2tile[back_to_glyph(x, y)]);

    gtkmap[y][x].glyph = glyph;
    gtkmap[y][x].bgtile = bgtile;
    gtkmap[y][x].tile = tile;
    gtkmap[y][x].update = 1;
    map_update++;
}

void
GTK_curs(winid id, int x, int y)
{
    GdkRectangle update_rect;

    if(id != NHW_MAP)
	return;

    if(cursx == x && cursy == y)
	;
    else{
	update_rect.x = cursx * c_3dwidth + (ROWNO - cursy) * c_3dofset;
	update_rect.y = cursy * c_3dheight - (map_visual == 0 ? map_font->ascent : 0);
	update_rect.width = c_width;
	update_rect.height = c_height;
	gtk_widget_draw(map, &update_rect);

	cursx = x;
	cursy = y;
    }

    update_rect.x = x * c_3dwidth + (ROWNO - y) * c_3dofset;
    update_rect.y = y * c_3dheight - (map_visual == 0 ? map_font->ascent : 0);
    update_rect.width = c_width;
    update_rect.height = c_height;

    gdk_draw_rectangle(
	map->window, map->style->white_gc, FALSE,
	update_rect.x, update_rect.y,
	c_width - 1, c_height - 1);
}

#ifdef RADAR
void
nh_radar_update()
{
    GdkRectangle update_rect;
    GtkAdjustment *hadj, *vadj;

    if(flags.radar && !radar_is_popuped){
	if(radar_is_created == 0)
	    nh_radar_new();

	gtk_widget_show_all(radar);
	radar_is_popuped= 1;
    }
    if(!flags.radar && radar_is_popuped){
	gtk_widget_hide_all(radar);
	radar_is_popuped= 0;
    }
    
    if(radar_is_popuped){
	if(map_scroll){
	    hadj = gtk_scrolled_window_get_hadjustment(
		GTK_SCROLLED_WINDOW(map_scroll)
		);
	    
	    vadj = gtk_scrolled_window_get_vadjustment(
		GTK_SCROLLED_WINDOW(map_scroll)
		);
/*
  Anyone know the function which copy from pixmap to pixmap in GDK?
  */
	    gdk_window_copy_area(
		radar_pixmap2, map->style->white_gc,
		0, 0,
		radar_pixmap,
		0, 0, 
		NH_RADAR_WIDTH, NH_RADAR_HEIGHT);
	    
	    gdk_draw_rectangle(
		radar_pixmap2, map->style->white_gc,
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
	    if(radar_is_popuped)
		gdk_window_raise(radar->window);
	}
    }
}
#endif

void
GTK_cliparound(int x, int y)
{
    ;
}

GdkPixmap *
GTK_glyph_to_gdkpixmap(int glyph)
{
    int tile;
    int src_x, src_y;
    GdkImage *img;
    GdkPixmap *pix;

    img = gdk_image_new(GDK_IMAGE_FASTEST, gdk_visual_get_system(),
      Tile->unit_width, Tile->unit_height);
    if (!img)
	return NULL;
    pix = gdk_pixmap_new(NULL, Tile->unit_width, Tile->unit_height, 
    	gdk_visual_get_system()->depth);
    if (!pix)
    {
	gdk_image_destroy(img);
	return NULL;
    }

    tile = fix_tile(glyph2tile[glyph]);
    
    src_x = (tile % tiles_per_row) * c_width;
    src_y = (tile / tiles_per_row) * c_height;

    x_tile_gdkimage_draw(img, FALSE, src_x, src_y, 0, 0);
    gdk_draw_image(pix, map_gc, img, 0, 0, 0, 0, img->width, img->height);
    gdk_image_destroy(img);

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

void
nh_map_flush()
{
    int i, j;
    GdkRectangle update_rect;

    if(map_update != 0){
	map_update = 0;

	for(i=0 ; i<ROWNO ; ++i)
	    for(j=0 ; j<COLNO ; ++j)
		if(gtkmap[i][j].update){
		    gtkmap[i][j].update = 0;
		    nh_map_print_glyph(j, i, &gtkmap[i][j], &update_rect);
		    gtk_widget_draw(map, &update_rect);
		}
#ifdef RADAR
	nh_radar_update();
#endif
    }
    GTK_curs(NHW_MAP, cursx, cursy);
}

int
nh_tile_height()
{
    return (c_width);
}


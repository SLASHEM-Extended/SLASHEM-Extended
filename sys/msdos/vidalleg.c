/*   SCCS Id: @(#)vidalleg.c   3.4 - 1.5   96/02/16                 */
/*   Copyright (c) NetHack PC Development Team 1995                 */
/*   NetHack may be freely redistributed.  See license for details. */
/*   AllegroHack (c) Kelly Youngblood 2000 */
/*   AllegroHack is licensed on the same terms as nethack, again,
        see license for details. */
/*
 * vidalleg.c - Video using Allegro library
 */

#define COPYRIGHT_BANNER_D "Based on AllegroHack v1.5 By Kelly Youngblood."

#include "hack.h"
#include "epri.h"

#include <allegro.h>

#ifdef ALLEGRO_USE_AA
# include <aastr.h>
#endif

#ifdef SHORT_FILENAMES
#include "patchlev.h"
#else
#include "patchlevel.h"
#endif

#include <dos.h>
#include <ctype.h>
#include "wintty.h"
#include "pcvideo.h"
#include "tile.h"

void nh_stretch_blit(BITMAP *, BITMAP *, int, int, int, int, int, int, int, int);

#ifdef ALLEGRO_USE_AA
#define nh_stretch_sprite        aa_stretch_sprite
#else
#define aa_stretch_blit          stretch_blit
#define nh_stretch_sprite        stretch_sprite
#endif

/* accessory functions */
#include "alnames.h"
#include "alfuncs.h"
#include "loadfont.h"

/* As Allegro works only with the DJGPP compiler, overlay support and
   support for non-DJGPP compilers isn't needed. */

/* prototypes */
int FDECL(alleg_swallowed,(int,int));
STATIC_DCL void FDECL(alleg_redrawmap,(BOOLEAN_P));
STATIC_DCL void NDECL(alleg_full_redraw);
STATIC_DCL void NDECL(alleg_load_cnf);
void FDECL(alleg_cliparound,(int,int));

STATIC_DCL void FDECL(alleg_scrollmap,(BOOLEAN_P));
STATIC_DCL void NDECL(positionbar);
STATIC_DCL void NDECL(init_progress_meter);
STATIC_DCL void NDECL(inc_progress_meter);
STATIC_DCL void FDECL(alleg_printGlyph_at, (int,int,int)); /* row, col, level */
#define PRINT_BACKGROUND	1
#define PRINT_FOREGROUND	2
STATIC_DCL int FDECL(alleg_SwitchMode,(int));
#define ALLEG_MODETEXT	0
#define ALLEG_MODEGFX	1
#define ALLEG_MODE_NONE	-1

#define FLOOR_GLYPH	cmap_to_glyph(S_room)

/* Settings */
static int video_mode   = GFX_AUTODETECT;
static int smoothing    = 0;           /* slightly smoother scrolling */
static int hud_setting  = 0;           /* HUD status */
static int fx_delay     = 1;           /* vsyncs per animation frame */
static int alleg_colordepth = 16;            /* color depth 15-32 */
static int X_RES	= 640;                /* X resolution min 640 */
static int Y_RES	= 480;                /* Y resolution min 400 */
static int X_WIDTH	= 640;             /* viewport sixe, automatically set */
static int Y_HEIGHT	= 480;             /* viewport sixe, automatically set */
static int attrib_allegro_normal = CLR_CYAN; /* was ATTRIB_VGA_NORMAL */
static int attrib_allegro_intense = CLR_BRIGHT_MAGENTA; /* was ATTRIB_VGA_INTENSE */
static int attrib_allegro_status = CLR_BRIGHT_BLUE; /* was ATTRIB_STATUS */
static int scroll_lim	= 0;             /* how many pixels to wait before a
                                           scroll*/
static int need_update  = 0;           /* screen-update code flag */
static int no_update    = 0;
static int did_update   = 0;           /* cursor-drawing code flag */
static int tiles_loaded = 0;           /* initialization flag */
static int player_dead  = 0;           /* darken the screen if true */
static int fancy_meters = 1;
static int faded_out    = 0;
static int use_2xsai    = 0;		/* use 2xsai interpolation routines */

static int draw_3Dtiles = -1;

static PALETTE tilepal;		/* Save the palette of the tile file
 				 */

/*
 * When draw_3Dtiles is on, this switches the system into "pseudo-3D" tile mode
 * This currently uses 3D tiles that are 48x64,  and are put onto the screen
 * in an overlapping manner (top ones furthest back, left on top of right)
 *
 * It kinda looks like this (from the GTK winport)

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

*1 = 3D_WIDTH	= 32 = tile_x * 2 / 3
*2 = 3D_HEIGHT	= 32 = tile_y / 2
*3 = 3D_OFFSETX	= 16 = tile_x - tile_y / 2
*4 = 3D_OFFSETY	= 32 = tile_y / 2
*5 = WIDTH	= 48 = tile_x
*6 = HEIGHT	= 64 = tile_y
*/

#define TILE_3D_WIDTH	(tile_x * 2 / 3)
#define TILE_3D_HEIGHT	(tile_y / 2)
#define TILE_3D_OFFSETX	(tile_x - TILE_3D_WIDTH)
#define TILE_3D_OFFSETY	(tile_y - TILE_3D_HEIGHT)

/*
 * Global Variables
 */

extern int clipx, clipxmax;        /* current clipping column from wintty.c */
extern boolean clipping;        /* clipping on? from wintty.c */
extern int curcol,currow;        /* current column and row        */
extern int g_attribute;
extern int attrib_text_normal;        /* text mode normal attribute */
extern int attrib_gr_normal;        /* graphics mode normal attribute */
extern int attrib_text_intense;        /* text mode intense attribute */
extern int attrib_gr_intense;        /* graphics mode intense attribute */
extern boolean inmap;                /* in the map window */

extern short glyph2tile[MAX_GLYPH];
extern long colorpal[16];

int tile_x = 0;
int tile_y = 0;

STATIC_VAR int TILEX = 0;
STATIC_VAR int TILEY = 0;

int font_x, font_y;

static const char *default_tilesets[] = {
	"slam3d.bmp", "slam32.bmp", "slam16.bmp"
};

#define TILEDIR "tiles\\"
#define EXT     ".bmp"
#define FONTX      8
#define FONTY      16

STATIC_VAR struct map_struct {
        int glyph;
        int ch;
        int attr;
}  map[ROWNO][COLNO];        /* track the glyphs */

# define alleg_clearmap() { int x,y; for (y=0; y < ROWNO; ++y) \
        for (x=0; x < COLNO; ++x) { map[y][x].glyph = cmap_to_glyph(S_stone); \
        map[y][x].ch = S_stone; map[y][x].attr = 0;} }

# define TOP_MAP_ROW 1

/* The following are macros that convert from x, y pixel coordinates
 * to their 3D versions
 * posx = normal x position = col * tile_x
 * posy = normal y position = row * tile_y
 */
#define posxy_to_posx3d(posx, posy) \
				(((posx) * TILE_3D_WIDTH)/tile_x + \
				 (ROWNO+TOP_MAP_ROW)*TILE_3D_OFFSETX - \
				 ((posy) * TILE_3D_OFFSETX/tile_y))
#define posxy3d_to_posx(posx3d, posy3d) \
				(((posx3d) - \
				 (ROWNO+TOP_MAP_ROW)*TILE_3D_OFFSETX + \
				 ((posy3d)*TILE_3D_OFFSETX/TILE_3D_HEIGHT)) * tile_x \
				  / TILE_3D_WIDTH)
#define colrow_to_posx3d(col, row)	(posxy_to_posx3d(((col) * tile_x), ((row) * tile_y)))
#define col_to_maxx3d(col)	(posxy_to_posx3d(((col) * tile_x), 0))

#define posy_to_posy3d(posy)	(((posy) * TILE_3D_HEIGHT) / tile_y)
#define posy3d_to_posy(posy3d)	(((posy3d) * tile_y) / TILE_3D_HEIGHT)
#define row_to_posy3d(row)	(posy_to_posy3d((row) * tile_y))
#define row_to_maxy3d(row)	(posy_to_posy3d((row) * tile_y))

static int clipwinx, clipwiny;
static BITMAP *subscreen = (BITMAP *)0;
static BITMAP *txt_subscreen = (BITMAP *)0;
static BITMAP *tile_subscreen = (BITMAP *)0;
static BITMAP *minimap = (BITMAP *)0;
static BITMAP *under_cursor = (BITMAP *)0;
static BITMAP *cursorbmp = (BITMAP *)0;
static BITMAP *ovcursor = (BITMAP *)0;
static BITMAP *font_cursor = (BITMAP *)0;
static BITMAP *errorbmp = (BITMAP *)0;
static BITMAP *tilecache[TOTAL_TILES_USED];
static BITMAP *explosions[10];
static BITMAP *shields[16];
static BITMAP *auras[8][28];
static BITMAP *hudwidgets[4];
static BITMAP *mapwidgets[3];
static BITMAP *alltiles = (BITMAP *)0;	/* Holds the combined tiles file */

/* blender colormaps for 256 color mode */
static COLOR_MAP half_solidity;

static char bigtile_file[BUFSZ] = "";

#define ON32  0
#define OFF32 1
#define ON16  2
#define OFF16 3

#define MW_U    0
#define MW_UP   1
#define MW_DOWN 2

int
alleg_detect()
{
	boolean retval;

	alleg_load_cnf();

        set_color_depth(alleg_colordepth);
        retval = !((alleg_SwitchMode(ALLEG_MODEGFX)) < 0);

        (void) alleg_SwitchMode(ALLEG_MODETEXT);

        return(retval);
}

void
alleg_backsp()
{
        int col,row;

        col = curcol;                 /* Character cell row and column */
        row = currow;

        if (col > 0) col = col-1;
        alleg_gotoloc(col,row);
        alleg_xputc(' ',colorpal[g_attribute]);

        alleg_gotoloc(col,row);
}


void
alleg_clear_screen(colour)
int colour;
{
        clear_to_color(screen, colour);
        clear_to_color(subscreen, colour);
        clear_to_color(minimap, makecol(0,0,0));

        if (iflags.tile_view) alleg_clearmap();
        alleg_gotoloc(0,0);        /* is this needed? */
}

void
alleg_cl_end(col,row)        /* clear to end of line */
int col,row;
{
        int count;

        /*
         * This is being done via character writes.
         * This should perhaps be optimized for speed by using VGA write
         * mode 2 methods as did clear_screen()
         */
        for (count = col; count < (CO-1); ++count) {
                alleg_WriteChar(' ',count,row,BACKGROUND_ALLEGRO_COLOR);
        }
}

void
alleg_cl_eos(cy)        /* clear to end of screen */
int cy;
{
        int count;

        cl_end();
        while(cy <= LI-2) {
                for (count = 0; count < (CO-1); ++count) {
                        alleg_WriteChar(' ',count,cy,
                                BACKGROUND_ALLEGRO_COLOR);
                }
                cy++;
        }
}

void
alleg_tty_end_screen()
{
/*        vga_clear_screen(BACKGROUND_ALLEGRO_COLOR); */
          (void) alleg_SwitchMode(ALLEG_MODETEXT);
}

void
alleg_get_scr_size()
{
        CO = X_RES/FONTX;
        LI = Y_RES/FONTY;
}

void
alleg_tty_startup(wid, hgt)
    int *wid, *hgt;
{
        CO = X_RES/FONTX;
        LI = Y_RES/FONTY;

        *wid = CO;
        *hgt = LI;

        clipx = 0;
        clipxmax = CO - 1;

        attrib_gr_normal    = attrib_allegro_normal;
        attrib_gr_intense   = attrib_allegro_intense;
        g_attribute         = attrib_gr_normal; /* Give it a starting value */
}

/*
 * Screen output routines (these are heavily used).
 *
 * These are the 3 routines used to place information on the screen
 * in the VGA PC tty port of NetHack.  These are the routines
 * that get called by the general interface routines in video.c.
 *
 * alleg_xputs -Writes a c null terminated string at the current location.
 *
 * alleg_xputc -Writes a single character at the current location. Since
 *            various places in the code assume that control characters
 *            can be used to control, we are forced to interpret some of
 *            the more common ones, in order to keep things looking correct.
 *
 * alleg_xputg -This routine is used to display a graphical representation of a
 *            NetHack glyph (a tile) at the current location.  For more
 *            information on NetHack glyphs refer to the comments in
 *            include/display.h.
 *
 */

#if 0 /* in 2xSai.c */
inline long Q_INTERPOLATE(long A, long B, long C, long D)
{
        register long qcolorMask = 0xE79CE79C;
        register long qlowpixelMask = 0x18631863;
        register long x = ((A & qcolorMask) >> 2) +
                            ((B & qcolorMask) >> 2) +
                            ((C & qcolorMask) >> 2) +
                            ((D & qcolorMask) >> 2);
        register long y = (A & qlowpixelMask) +
                            (B & qlowpixelMask) +
                            (C & qlowpixelMask) +
                            (D & qlowpixelMask);
        y = (y>>2) & qlowpixelMask;
        return x+y;
}
#endif

#define alleg_text(s,x,y,color) textout(screen, font, s, x, y, color)

/* Magenta-Blue-Cyan gradient for energy bars */
long
MBC(n)
int n;
{
        long c;

        if(n >= 512)
                c = makecol(0,255,255);
        else if(n < 256)
                c = makecol(255-n,0,255);
        else
                c = makecol(0, n-256, 255);

        return c;
}

/* Red-Yellow-Green gradient for health bars */
long
RYG(n)
int n;
{
        long c;

        if(n >= 512)
                c = makecol(0,255,0);
        else if(n < 256)
                c = makecol(255,n,0);
        else
                c = makecol(511-n, 255, 0);

        return c;
}

void
drawbars()
{
        int maxx;
        int hmax = max((Upolyd ? u.mhmax : u.uhpmax), 1);
        int emax = max(u.uenmax, 1);
        int hcur = (Upolyd ? u.mh : u.uhp);
        BITMAP *healthbar = create_bitmap(hmax, 1);
        BITMAP *energybar = create_bitmap(u.uenmax, 1);
        int i;
        long hpfact = 512*hcur;
        long enfact = 512*u.uen;
        int hcolor = RYG(hpfact/(long)hmax);
        int ecolor = MBC(enfact/(long)emax);

        if(hud_setting == 2)
                maxx = X_RES-81;
        else if (hud_setting)
                maxx = X_RES-97;
        else
                maxx = X_RES-1;

        clear_to_color(healthbar,makecol(0,0,0));
        clear_to_color(energybar,makecol(0,0,0));

        if(!fancy_meters)
        {
                hline(healthbar, 0, 0, hcur, hcolor);
                hline(energybar, 0, 0, u.uen, ecolor);
        }
        else
        {
            for(i = 0; i < hcur; i++)
                putpixel(healthbar, i, 0, RYG((i*512L)/(long)hmax));

            for(i = 0; i < u.uen; i++)
		putpixel(energybar, i, 0, MBC((i*512L)/(long)emax));
        }

        rect(screen, 0, Y_RES-48, maxx, Y_RES-41, hcolor);
        rect(screen, 0, Y_RES-40, maxx, Y_RES-33, ecolor);

        nh_stretch_blit(healthbar, screen, 0, 0, hmax, 1,
                                        1, Y_RES-47, maxx-1, 6);
	nh_stretch_blit(energybar, screen, 0, 0, emax, 1,
                                        1, Y_RES-39, maxx-1, 6);



        destroy_bitmap(healthbar);
        destroy_bitmap(energybar);
}

void
alleg_stats(s1, row)
char *s1;
int row;
{
        int y, s2siz, maxx;
        char *s2 = (char *) 0;

	if (row == 0) {
		char* bp;

		/* Find St: */
    		bp = s2 = index(s1, ':') - 2;

    		for (bp--; *(bp - 1) == ' '; bp--);

    		*bp = '\0';
	}

        if (s2){
            s2siz = strlen(s2)*FONTX;
        } else {
        }

        if(hud_setting == 2)
                maxx = X_RES-81;
        else if (hud_setting)
                maxx = X_RES-97;
        else
                maxx = X_RES-1;

        if(row) y = Y_RES-FONTY;
        else    y = Y_RES-FONTY*2;

        if(row) drawbars();

        rectfill(screen, 0, y, X_RES, y+(FONTY-1), BACKGROUND_ALLEGRO_COLOR);

	alleg_text(s1, 0, y, colorpal[attrib_allegro_status]);

        if (s2) {
        	alleg_text(" ", maxx-s2siz - FONTX, y, colorpal[attrib_allegro_status]);
        	alleg_text(s2, maxx-s2siz, y, colorpal[attrib_allegro_status]);
        }
}

/* This assumes that the ON32 and the ON16 are 32x32 and 16x16 respectively
 * Will scale the object tiles as needed
 */
void
draw_HUD()
{
        int i=6;

        draw_sprite(screen, hudwidgets[ON32], X_RES-3*32, Y_RES-48);
        if(uwep)
                nh_stretch_sprite(screen, tilecache[glyph2tile[obj_to_glyph(uwep)]],
                             X_RES-3*32, Y_RES-48, 32, 32);

        draw_sprite(screen, hudwidgets[ u.twoweap ? ON32 : OFF32],
                             X_RES-2*32, Y_RES-48);
        if(uswapwep)
                nh_stretch_sprite(screen, tilecache[glyph2tile[obj_to_glyph(uswapwep)]],
                             X_RES-2*32, Y_RES-48, 32, 32);
        if(uquiver && (is_missile(uquiver) || ammo_and_launcher(uquiver,uwep)))
                draw_sprite(screen, hudwidgets[ON32],
                            X_RES-32, Y_RES-48);
        else
                draw_sprite(screen, hudwidgets[OFF32],
                            X_RES-32, Y_RES-48);
        if(uquiver)
                stretch_sprite(screen, tilecache[glyph2tile[obj_to_glyph(uquiver)]],
                             X_RES-32, Y_RES-48, 32, 32);

        if(uarms)
        {
                draw_sprite(screen, hudwidgets[ON16], X_RES-i*16, Y_RES-16);
                nh_stretch_sprite(screen,
                  tilecache[glyph2tile[obj_to_glyph(uarms)]],
                  X_RES-i*16, Y_RES-16, 16, 16);
        }
        else
                draw_sprite(screen, hudwidgets[OFF16], X_RES-i*16, Y_RES-16);
        i--;
        if(uarm || uskin
#ifdef TOURIST /* this is still a #define! sheesh! */
                || uarmu
#endif
        ) {
                draw_sprite(screen, hudwidgets[ON16], X_RES-i*16, Y_RES-16);
                if(uskin)
                        nh_stretch_sprite(screen,
                          tilecache[glyph2tile[obj_to_glyph(uskin)]],
                          X_RES-i*16, Y_RES-16, 16, 16);
                else if(uarm)
                        nh_stretch_sprite(screen,
                          tilecache[glyph2tile[obj_to_glyph(uarm)]],
                          X_RES-i*16, Y_RES-16, 16, 16);
#ifdef TOURIST
                else
                        nh_stretch_sprite(screen,
                          tilecache[glyph2tile[obj_to_glyph(uarmu)]],
                          X_RES-i*16, Y_RES-16, 16, 16);
#endif
        }
        else
                draw_sprite(screen, hudwidgets[OFF16], X_RES-i*16, Y_RES-16);
        i--;

        if(uarmc)
        {
                draw_sprite(screen, hudwidgets[ON16], X_RES-i*16, Y_RES-16);
                nh_stretch_sprite(screen,
                  tilecache[glyph2tile[obj_to_glyph(uarmc)]],
                  X_RES-i*16, Y_RES-16, 16, 16);
        }
        else
                draw_sprite(screen, hudwidgets[OFF16], X_RES-i*16, Y_RES-16);
        i--;
        if(uarmh)
        {
                draw_sprite(screen, hudwidgets[ON16], X_RES-i*16, Y_RES-16);
                nh_stretch_sprite(screen,
                  tilecache[glyph2tile[obj_to_glyph(uarmh)]],
                  X_RES-i*16, Y_RES-16, 16, 16);
        }
        else
                draw_sprite(screen, hudwidgets[OFF16], X_RES-i*16, Y_RES-16);
        i--;
        if(uarmg)
        {
                draw_sprite(screen, hudwidgets[ON16], X_RES-i*16, Y_RES-16);
                nh_stretch_sprite(screen,
                  tilecache[glyph2tile[obj_to_glyph(uarmg)]],
                  X_RES-i*16, Y_RES-16, 16, 16);
        }
        else
                draw_sprite(screen, hudwidgets[OFF16], X_RES-i*16, Y_RES-16);
        i--;
        if(uarmf)
        {
                draw_sprite(screen, hudwidgets[ON16], X_RES-i*16, Y_RES-16);
                nh_stretch_sprite(screen,
                  tilecache[glyph2tile[obj_to_glyph(uarmf)]],
                  X_RES-i*16, Y_RES-16, 16, 16);
        }
        else
                draw_sprite(screen, hudwidgets[OFF16], X_RES-i*16, Y_RES-16);

        return;
}

void
draw_miniHUD()
{
        draw_sprite(screen, hudwidgets[ON16], X_RES-3*32, Y_RES-48);
        if(uwep)
                nh_stretch_sprite(screen,
                        tilecache[glyph2tile[obj_to_glyph(uwep)]],
                        X_RES-3*32, Y_RES-48, 16, 16);

        draw_sprite(screen, hudwidgets[u.twoweap ? ON16 : OFF16],
                        X_RES-3*32, Y_RES-32);
        if(uswapwep)
                nh_stretch_sprite(screen,
                        tilecache[glyph2tile[obj_to_glyph(uswapwep)]],
                        X_RES-3*32, Y_RES-32, 16, 16);

        if(uquiver && (is_missile(uquiver) || ammo_and_launcher(uquiver,uwep)))
                draw_sprite(screen, hudwidgets[ON16],
                        X_RES-3*32, Y_RES-16);
        else
                draw_sprite(screen, hudwidgets[OFF16],
                        X_RES-3*32, Y_RES-16);
        if(uquiver)
                nh_stretch_sprite(screen,
                        tilecache[glyph2tile[obj_to_glyph(uquiver)]],
                        X_RES-3*32, Y_RES-16, 16, 16);
}


void
maybe_stairs(dmap,x,y,up)
BITMAP *dmap;
int x,y,up;
{
        if(!levl[x][y].seenv)
                return;
        draw_sprite(dmap, mapwidgets[up ? MW_UP : MW_DOWN], x-2, y*2-2);
        return;
}

void
draw_minimap()
{
    BITMAP *dmap = create_bitmap(80,48);
    int mapwinx = clipwinx;
    int mapwiny = clipwiny;

    /* don't anti-alias */
    stretch_blit(minimap, dmap, 0, 0, 80, 24, 0, 0, 80, 48);

    if (draw_3Dtiles && !iflags.traditional_view) {
    	mapwinx = posxy3d_to_posx(clipwinx, clipwiny);
    	mapwiny = posy3d_to_posy(clipwiny);
    }

    draw_sprite(dmap, mapwidgets[MW_U], u.ux-2, u.uy*2-2);

    rect(dmap, mapwinx/tile_x, (mapwiny/tile_y)*2-2,
        (mapwinx+X_RES)/tile_x, ((mapwiny+Y_HEIGHT)/tile_y)*2-2,
        colorpal[CLR_GREEN]);
    rect(dmap, 0, 0, 79, 47, colorpal[CLR_WHITE]);

    if(xdnstair)
        maybe_stairs(dmap,xdnstair,ydnstair,0);
    if(xdnladder)
        maybe_stairs(dmap,xdnstair,ydnstair,0);
    if(sstairs.sx)
        maybe_stairs(dmap,sstairs.sx,sstairs.sy,sstairs.up);
    if(xupstair)
        maybe_stairs(dmap,xupstair,yupstair,1);
    if(xupladder)
        maybe_stairs(dmap,xupladder,yupladder,1);

    blit(dmap, screen, 0, 0, X_RES-80, Y_RES-48, 80, 48);
    destroy_bitmap(dmap);
}

void
display_HUD()
{
        switch(hud_setting)
        {
                case 1: draw_HUD(); break;
                case 2: draw_minimap(); break;
                case 3: draw_minimap();
                        draw_miniHUD(); break;
                default:  break;
        }
}

void
cycleHUD()
{
        hud_setting++;
        if(hud_setting > 3)
                hud_setting = 0;
        rectfill(screen, X_RES-96, Y_RES-48, X_RES-1, Y_RES-1, 0);
        bot();
        display_HUD();
}



void
alleg_xputs(s,col,row)
char *s;
int col,row;
{
        int x,y;

        x = min(col,(CO-1));   /* min() used protection from callers */
        y = min(row,(LI-1));

        if (s != (char *)0) {
                alleg_text(s, x*FONTX, y*FONTY, colorpal[g_attribute]);
        }
}

void
alleg_xputc(ch,attr)        /* write out character (and attribute) */
char ch;
int attr;
{
        int col,row;

        col = curcol;
        row = currow;

        CO = X_RES/FONTX;
        LI = Y_RES/FONTY;

        switch(ch) {
            case '\n':
                        col = 0;
                        ++row;
                        break;
            default:
                        alleg_WriteChar((unsigned char)ch,col,row,attr);
                        if (col < (CO -1 )) ++col;
                        break;
        } /* end switch */
        alleg_gotoloc(col,row);

        need_update = 2;
}

inline int getmalign(mtmp)
struct monst *mtmp;
{
        int alignment;

        if (mtmp->ispriest || mtmp->data == &mons[PM_ALIGNED_PRIEST]
                        || mtmp->data == &mons[PM_ANGEL])
                return(EPRI(mtmp)->shralign);
        else
                alignment = mtmp->data->maligntyp;

        alignment = (alignment > 0) ? A_LAWFUL :
                    (alignment < 0) ? A_CHAOTIC : A_NEUTRAL;

        return alignment;
}

BITMAP*
subst_mon(pm, fem, lev, a)
int pm, fem, lev, a;
{
        int v = 0;
        BITMAP *ptr;

        switch(pm)
        {
                case PM_ALIGNED_PRIEST:
                case PM_PRIEST:
                case PM_PRIESTESS:
                        v = fem ? -PM_PRIESTESS: -PM_PRIEST;
                        switch(a) {
                                case A_LAWFUL:
                                        v = fem ? SUB_PLF : SUB_PLM; break;
                                case A_NEUTRAL:
                                        v = fem ? SUB_PNF : SUB_PNM; break;
                                case A_CHAOTIC:
                                        v = fem ? SUB_PCF : SUB_PCM; break;
                        }
                        break;
                case PM_ARCHEOLOGIST:   if(fem) v = SUB_ARCF; break;
                case PM_BARBARIAN:      if(fem) v = SUB_BARF; break;
                case PM_HEALER:         if(fem) v = SUB_HEAF; break;
                case PM_KNIGHT:         if(fem) v = SUB_KNIF; break;
                case PM_MONK:           if(fem) v = SUB_MONF; break;
                case PM_RANGER:         if(fem) v = SUB_RANF; break;
                case PM_ROGUE:          if(fem) v = SUB_ROGF; break;
                case PM_SAMURAI:        if(fem) v = SUB_SAMF; break;
                case PM_TOURIST:        if(fem) v = SUB_TOUF; break;
                case PM_WIZARD:         if(fem) v = SUB_WIZF; break;
                case PM_VALKYRIE:       if(!fem) v = -PM_KNIGHT; break;
                default: break;
        }
        if((v > 0) && substitutes[v])
                ptr = substitutes[v];
        else
        {
            if(v < 0)
                pm = -v;

                ptr = tilecache[glyph2tile[pm]];
        }
        return ptr;
}

BITMAP*
getutile()
{
        if(u.umonnum == PM_WIZARD && Race_if(PM_GNOME) && !flags.female)
                return(tilecache[glyph2tile[PM_GNOMISH_WIZARD]]);

        return (subst_mon(u.umonnum, flags.female, u.ulevel, u.ualign.type));
}

BITMAP*
sub_altar(a)
int a;
{
	if (draw_3Dtiles)
		return (tilecache[glyph2tile[cmap_to_glyph(S_altar)]]);
        switch(a) {
                case A_LAWFUL:
                        return(substitutes[SUB_ALTARL]);
                case A_NEUTRAL:
                        return(substitutes[SUB_ALTARN]);
                case A_CHAOTIC:
                        return(substitutes[SUB_ALTARC]);
                default:
                        return(substitutes[SUB_ALTARM]);
        }
}

void
rogue_xputg(x,y,ch,attr)
int x,y;
char ch;
int attr;
{
        char buf[2];

        buf[0] = ch;
        buf[1] = 0;

       	/* Check the subscreen */
       	if (subscreen != txt_subscreen) {
       		/* alleg_full_redraw() should fix this */
       		alleg_full_redraw();
       		/* No need to do any more - the whole screen has been painted */
       		return;
       	}

        /* Actually, tile_x and tile_y should be set to FONTX, FONTY ... */
        textout(subscreen, font, buf, x, y, colorpal[attr]);
}


void
alleg_xputg(glyphnum,ch)  /* Place tile represent. a glyph at current location */
int glyphnum;
int ch;
{
        int col,row;
        int attr;
        int mapx, mapy;

        row = currow;
        col = curcol;
        if ((col < 0 || col >= COLNO) ||
            (row < TOP_MAP_ROW || row >= (ROWNO + TOP_MAP_ROW)))
            	return;

	/* Update glyph table */
        map[row - TOP_MAP_ROW][col].glyph = glyphnum;
        map[row - TOP_MAP_ROW][col].ch = ch;
        attr = g_attribute;
        map[row - TOP_MAP_ROW][col].attr = attr;

        mapx = col+1;
        mapy = row - TOP_MAP_ROW;

	/* Update the minimap */
        if(ch == ' ')
                putpixel(minimap, mapx, mapy, 0);
        else if(glyphnum == cmap_to_glyph(S_room))
                putpixel(minimap, mapx, mapy, colorpal[CLR_BLACK]);
        else
                putpixel(minimap, mapx, mapy, colorpal[attr]);

	if (
#ifdef REINCARNATION
        	Is_rogue_level(&u.uz) ||
#endif
        	iflags.traditional_view)
        {
                rogue_xputg(col*tile_x, row*tile_y, ch, attr);

	        if (col < (CO - 1 )) ++col;
	        alleg_gotoloc(col,row);

		need_update = 1;
		return;
        }

        /* Draw in tilemode */
        if (subscreen == txt_subscreen) {
        	alleg_full_redraw();
        	return;
        }
        if (!draw_3Dtiles) {
        	alleg_printGlyph_at(col, row, PRINT_BACKGROUND|PRINT_FOREGROUND);
        } else {
        	/* To update the 3D tiles, we need to get parts of all the surrounding tiles
        	 */
        	 /* Update Backgrounds, then foregrounds */
		/* Set clipping */
		int posx = colrow_to_posx3d(col, row);
		int posy = row_to_posy3d(row);
		int i, j;

		set_clip(subscreen, posx, posy, posx + tile_x - 1, posy + tile_y - 1);

#if 0
		rect(subscreen, posx, posy, posx + tile_x - 1, posy + tile_y - 1,
				colorpal[CLR_BLACK]);
#endif

		for (j = row - 1; j <= row + 1; j++)
		    for (i = col - 1; i <= col + 1; i++)
			alleg_printGlyph_at(i, j, PRINT_BACKGROUND);

		for (j = row - 1; j <= row + 1; j++)
		    for (i = col - 1; i <= col + 1; i++)
			alleg_printGlyph_at(i, j, PRINT_FOREGROUND);

		/* Reset clip */
		set_clip(subscreen, 0, 0, subscreen->w, subscreen->h);

		/* Put cursor to the right position */
	        if (col < (CO - 1 )) ++col;
	        alleg_gotoloc(col,row);
        }
}

static void
alleg_printGlyph_at(col, row, mode)
int col, row, mode;
{
       	int glyphnum = NO_GLYPH;
        int mapx = col+1;
        int mapy = row-1;
        int glyph2num = NO_GLYPH;
       	int posx = col*tile_x;
       	int posy = row*tile_y;
        register struct trap *trap;
        int draw_trans = 0, draw_gray = 0;
        struct obj *otmp;

        BITMAP *f_tile;
        BITMAP *b_tile;

        if ((col < 0 || col >= COLNO) ||
            (row < TOP_MAP_ROW || row >= (ROWNO + TOP_MAP_ROW))) return;

	glyphnum = map[row - TOP_MAP_ROW][col].glyph;
	glyph2num = memory_glyph(mapx, mapy);
	if(Blind || (viz_array && !cansee(mapx, mapy))){
	    if(glyph_is_object(glyph2num)){
		if(!levl[mapx][mapy].waslit)
		    glyph2num = cmap_to_glyph(S_stone);
		else
		    glyph2num = back_to_glyph(mapx, mapy);
	    }
	} else {
	    glyph2num = back_to_glyph(mapx, mapy);
	}

	if (draw_3Dtiles) {
		posx = posxy_to_posx3d(posx, posy);
		posy = posy_to_posy3d(posy);
	}
	need_update = 1;

	if (mode & PRINT_BACKGROUND) {
		/* Draw floor */

		b_tile = tilecache[glyph2tile[FLOOR_GLYPH]];
		draw_sprite(subscreen, b_tile, posx, posy);

                /* Use alternate altar tiles if appropriate */
		if(!draw_3Dtiles && glyph2num == cmap_to_glyph(S_altar) &&
		          !(Is_astralevel(&u.uz) || Is_sanctum(&u.uz))) {
		        b_tile = sub_altar((aligntyp)Amask2align(levl[mapx][mapy].altarmask & ~AM_SHRINE));
		} else {
		        b_tile = tilecache[glyph2tile[glyph2num]];
		}

#define glyph_is_wall(glyphnum)		(glyph_to_cmap(glyphnum) != NO_GLYPH && \
	    		glyph_to_cmap(glyphnum) > S_stone && \
	    		glyph_to_cmap(glyphnum) <= S_hcdoor)
#define glyph_is_wall_or_stone(glyphnum)	\
	    		(glyph_to_cmap(glyphnum) != NO_GLYPH && \
	    		glyph_to_cmap(glyphnum) >= S_stone && \
	    		glyph_to_cmap(glyphnum) <= S_hcdoor)

	    /* Check above, above right and to the left */
	    if (draw_3Dtiles && glyph_is_wall(glyph2num) && (
	 	((row > TOP_MAP_ROW) &&
	 	 !glyph_is_wall_or_stone(map[row - TOP_MAP_ROW - 1][col].glyph) ||
		 	 (col > 0 &&
	 	   !glyph_is_wall_or_stone(map[row - TOP_MAP_ROW - 1][col - 1].glyph)) ||
	 	 (col < (COLNO - 1) &&
	 	   !glyph_is_wall_or_stone(map[row - TOP_MAP_ROW - 1][col + 1].glyph))) ||
		 	(col > 0 &&
	 	   !glyph_is_wall_or_stone(map[row - TOP_MAP_ROW][col - 1].glyph))))
	    {
		/* Make walls see-through */
		if (alleg_colordepth != 8)
		    set_trans_blender(255, 255, 255, 128);
		draw_trans_sprite(subscreen, b_tile, posx, posy);
	    } else {
	        draw_sprite(subscreen, b_tile, posx, posy);
	    }
	}
	if (!(mode & PRINT_FOREGROUND)) {
	        if (col < (CO - 1 )) ++col;
	        alleg_gotoloc(col,row);
	        return;
	}

	/* Draw the foreground layers */

	f_tile = tilecache[glyph2tile[glyphnum]];

#if 0
        if ((trap = t_at(mapx,mapy)) != 0 && trap->tseen &&
            !covers_traps(mapx,mapy))
                glyph2num = trap_to_glyph(trap);
        else
                glyph2num = back_to_glyph(mapx,mapy);

#define glyph_is_wall(glyphnum)		(glyph_to_cmap(glyphnum) != NO_GLYPH && \
	    		glyph_to_cmap(glyphnum) > S_stone && \
	    		glyph_to_cmap(glyphnum) <= S_hcdoor)
#define glyph_is_wall_or_stone(glyphnum)	\
	    		(glyph_to_cmap(glyphnum) != NO_GLYPH && \
	    		glyph_to_cmap(glyphnum) >= S_stone && \
	    		glyph_to_cmap(glyphnum) <= S_hcdoor)
	/* Check above, above right and to the left */
	if (draw_3Dtiles && glyphnum == glyph2num && glyph_is_wall(glyph2num) && (
	 	((row > TOP_MAP_ROW) &&
	 	 !glyph_is_wall_or_stone(map[row - TOP_MAP_ROW - 1][col].glyph) ||
		 	 (col > 0 &&
	 	   !glyph_is_wall_or_stone(map[row - TOP_MAP_ROW - 1][col - 1].glyph)) ||
	 	 (col < (COLNO - 1) &&
	 	   !glyph_is_wall_or_stone(map[row - TOP_MAP_ROW - 1][col + 1].glyph))) ||
		 	(col > 0 &&
	 	   !glyph_is_wall_or_stone(map[row - TOP_MAP_ROW][col - 1].glyph))))
	{
		/* Make walls see-through */
		if (alleg_colordepth != 8)
		    set_trans_blender(255, 255, 255, 128);
		draw_trans_sprite(subscreen, f_tile, posx, posy);
	} else {
	    draw_sprite(subscreen, b_tile, posx, posy);
	}
#endif

	/* Only draw foreground if it isn't the same as the background */
        if(glyphnum != glyph2num) {
		if(glyph_to_mon(glyphnum) == NO_GLYPH) {
			/* Do nothing */
	         }
	        else /* (glyphnum < NUMBER_OF_MONS) */
	        {
	                struct monst* mtmp = level.monsters[mapx][mapy];

	                if(mapx == u.ux && mapy == u.uy && !u.usteed &&
	                   (Blind || !Invisible))
	                          f_tile = getutile();
	                else if(mtmp)
	                        f_tile = subst_mon(glyph_to_mon(glyphnum),
	                                           mtmp->female,
	                                           mtmp->m_lev, getmalign(mtmp));

	                if(mapx == u.ux && mapy == u.uy && Invis &&
	                		!Blind && See_invisible)
	                        draw_trans = 1;
	                else if(mtmp && mtmp->minvis && See_invisible)
	                        draw_trans = 1;

	                if(cansee(mapx,mapy) && !covers_objects(mapx,mapy) &&
	                        (otmp = vobj_at(mapx,mapy)))
	                {
	                    if(!Hallucination && (otmp->otyp == STATUE))
	                        draw_gray_sprite(subscreen,
	                                 tilecache[glyph2tile[otmp->corpsenm]],
	                                 posx, posy);
	                    else
	                    {
	                        glyph2num = obj_to_glyph(otmp);
	                        draw_sprite(subscreen,
	                                    tilecache[glyph2tile[glyph2num]],
	                                    posx, posy);
	                    }
	                }
	        }

	        if(f_tile == tilecache[glyph2tile[PM_ALIGNED_PRIEST]])
	                f_tile = tilecache[glyph2tile[PM_PRIEST]];

	        otmp = vobj_at(mapx,mapy);

	        if(glyphnum == objnum_to_glyph(STATUE) && !Hallucination && otmp)
	        {
	                f_tile = tilecache[glyph2tile[otmp->corpsenm]];
	                draw_gray = 1;
	        }

	        if (draw_gray) {
	                draw_gray_sprite(subscreen,f_tile,posx, posy);
	        }
	        else
	        if(draw_trans)
	        {
		    if (alleg_colordepth != 8)
			set_trans_blender(255, 255, 255, 128);

	            draw_trans_sprite(subscreen, f_tile, posx, posy);
	        }
	        else
	        {
	            draw_sprite(subscreen, f_tile, posx, posy);
	        }
	}

	/* Draw Aura if protection is active */
        if(u.uspellprot && mapx == u.ux && mapy == u.uy)
        {
        	int pixelcolor = ((u.uspellprot*20 < 200) ? (u.uspellprot*20 + 55) : 255);

                drawing_mode(DRAW_MODE_TRANS, screen, 0, 0);

		if (alleg_colordepth != 8)
                    set_trans_blender(255,255,255,128);

                circlefill(subscreen, posx+(tile_x/2), posy+(tile_y/2),
                           (min((tile_x/2), (tile_y/2)) - 1),
                           makecol(pixelcolor, pixelcolor,0));
                drawing_mode(DRAW_MODE_SOLID, screen, 0, 0);
        }
        if (col < (CO - 1 )) ++col;
        alleg_gotoloc(col,row);
}

/*
 * Cursor location manipulation, and location information fetching
 * routines.
 * These include:
 *
 * alleg_gotoloc(x,y)     - Moves the "cursor" on screen to the specified x
 *                         and y character cell location.  This routine
 *                       determines the location where screen writes
 *                       will occur next, it does not change the location
 *                       of the player on the NetHack level.
 */

void
alleg_gotoloc(col,row)
int col,row;
{

#if 0
    if (row < TOP_MAP_ROW || row >= (ROWNO + TOP_MAP_ROW)) {
        curcol = min(col,CO - 1); /* protection from callers */
    } else {
        curcol = min(col,COLNO - 1); /* protection from callers */
    }

    currow = min(row,LI - 1);
#endif

    curcol = min(col,CO - 1); /* protection from callers */
    currow = min(row,LI - 1);
}

void
alleg_cliparound(x, y)
int x, y;
{
        extern boolean restoring;
        static int oldtx = 0;
        static int oldty = 0;
        int oldx = clipwinx;
        int oldy = clipwiny;

        int finalx, finaly;
        int minx, miny, maxx, maxy;

	if(scroll_lim)
	{
	    if(abs(oldtx-x)<scroll_lim)
		x = oldtx;
	    else
		oldtx = x;

	    if(abs(oldty-y)<scroll_lim)
		y = oldty;
	    else
		oldty = y;
	}

        if(In_sokoban(&u.uz))
        {
                switch(u.uz.dlevel)
                {
                        case 1: minx = 26*tile_x; miny = 4*tile_y;
                                maxx = 52*tile_x; maxy = 22*tile_y;
                                break;
                        case 2: minx = 30*tile_x; miny = 6*tile_y;
                                maxx = 50*tile_x; maxy = 19*tile_y;
                                break;
                        case 3: minx = 24*tile_x; miny = 6*tile_y;
                                maxx = 53*tile_x; maxy = 20*tile_y;
                                break;
                        case 4: minx = 32*tile_x; miny = 6*tile_y;
                                maxx = 47*tile_x; maxy = 19*tile_y;
                                break;
                }
        }
        else if(In_V_tower(&u.uz))
        {
                minx = 16*tile_x;
                miny = 6*tile_y;

                if(u.uz.dlevel == 3)
                { maxx = 35*tile_x; maxy = 19*tile_y; }
                else
                { maxx = 31*tile_x; maxy = 17*tile_y; }
        }
        else
        {
                minx = 0;               miny = tile_y;
                maxx = 80*tile_x;       maxy = 22*tile_y;
        }

        finalx = (x*tile_x - X_WIDTH/2) - tile_x/2;
        finaly = ((y+1)*tile_y - Y_HEIGHT/2) + tile_y/2;

        if((maxx - minx) < X_WIDTH)
        {
                finalx = ((minx+maxx) - X_RES)/2;
        }
        else
        {
                if(finalx < minx)
                        finalx = minx;
                if(finalx > (maxx - X_WIDTH))
                        finalx = maxx - X_WIDTH;
        }

        if((maxy - miny) < Y_HEIGHT)
        {
                finaly = ((miny+maxy) - Y_HEIGHT)/2;
        }
        else
        {
                if(finaly < miny)
                        finaly = miny;
                if(finaly > (maxy - Y_HEIGHT))
                        finaly = maxy - Y_HEIGHT;
        }

        /* Convert to 3D pixel equivalents if needed */
        if(draw_3Dtiles && !iflags.traditional_view) {
        	finalx = posxy_to_posx3d(finalx, finaly) - X_WIDTH/4;
        	finaly = posy_to_posy3d(finaly) - Y_HEIGHT/4;

        }

        if( (finalx != oldx) || (finaly != oldy) ) {
            if (!restoring)
            {
                if(smoothing == 2)
                {
                        clipwinx = (oldx*3+finalx)/4;
                        clipwiny = (oldy*3+finaly)/4;
                        need_update = 1;
                        alleg_redrawmap(0);
                }

                if(smoothing)
                {
                        clipwinx = (oldx+finalx)/2;
                        clipwiny = (oldy+finaly)/2;
                        need_update = 1;
                        alleg_redrawmap(0);
                }

                if(smoothing == 2)
                {
                        clipwinx = (oldx+finalx*3)/4;
                        clipwiny = (oldy+finaly*3)/4;
                        need_update = 0;
                        alleg_redrawmap(0);
                }
                clipwinx = finalx;
                clipwiny = finaly;
                need_update = 1;
                alleg_redrawmap(1);
            }
        }
}

void alleg_overview(on)
boolean on;
{
        if (on && !iflags.traditional_view && !draw_3Dtiles) {
                if(iflags.over_view)
                        return;
                iflags.over_view = TRUE;
        } else {
                if(!iflags.over_view)
                        return;
                iflags.over_view = FALSE;
        }
        need_update = 1;

        alleg_redrawmap(1);
}

void
alleg_redrawmap(final)
boolean final;
{
    if(hud_setting)
        display_HUD();

    if(no_update || faded_out)
        return;

    if ((iflags.traditional_view && subscreen != txt_subscreen) ||
    	(!iflags.traditional_view && subscreen == txt_subscreen) ||
    	(need_update == 2))
		alleg_full_redraw();

    if(u.uswallow && !iflags.over_view && !iflags.traditional_view)
    {
        rectfill(screen, 0, FONTY, X_RES, (Y_HEIGHT)-1, makecol(0,0,0));
        alleg_swallowed(u.ux,u.uy);
        return;
    }
    if (need_update)
    {
        if (iflags.over_view)
        {
             int maxx = 80*tile_x;
             int maxy = 23*tile_y;

#if 0
             if (draw_3Dtiles && !iflags.traditional_view) {
             	    maxx = posxy_to_posx3d(COLNO*tile_x, 0);
             	    maxy = posy_to_posy3d((ROWNO-1)*tile_y) + tile_y;
             }
#endif

             if(final)
                /* don't anti-alias, it's too much of a slowdown */
                stretch_blit(subscreen, screen, 0, 0, maxx, maxy,
                             0, FONTY, X_RES, Y_HEIGHT);
        }
        else
        {
            if(player_dead)
                tinted_blit(subscreen, screen, clipwinx, clipwiny,
                            0, FONTY, X_RES, Y_HEIGHT, makecol(0,0,0), 128);
            else if(Hallucination)
            {
                int r, g, b;

                hsv_to_rgb(moves % 360, 1, 1, &r, &g, &b);
                tinted_blit(subscreen, screen, clipwinx, clipwiny,
                            0, FONTY, X_RES, Y_HEIGHT, makecol(r,g,b), 96);
            }
#if 0
            else if (use_2xsai && subscreen != txt_subscreen) {
            	/* Only when drawing in gfx mode */
            	nh_stretch_blit(subscreen, screen,
            		clipwinx + (X_RES / 4),
            		clipwiny + (Y_RES / 4),
            		X_RES / 2, Y_HEIGHT / 2,
			0, FONTY, X_RES, Y_HEIGHT);

            }
#endif
            else
                blit(subscreen, screen, clipwinx, clipwiny,
                             0, FONTY, X_RES, Y_HEIGHT);
        }
        need_update = 0;
        did_update = 1;
    }
}

static
void alleg_full_redraw()
{
	int x, y;
	int t;
	int old_currow = currow;
	int old_curcol = curcol;

	/* Check for right subscreen */
        if (
#ifdef REINCARNATION
	    Is_rogue_level(&u.uz) ||
#endif
	    iflags.traditional_view) {
        	if (subscreen != txt_subscreen) subscreen = txt_subscreen;
        } else {
        	if (subscreen == txt_subscreen) subscreen = tile_subscreen;
        }

        clear_to_color(subscreen, BACKGROUND_ALLEGRO_COLOR);

	for (y = TOP_MAP_ROW; y < (ROWNO + TOP_MAP_ROW); ++y)
	    for (x = 0; x < COLNO; ++x) {
	    	t = y - TOP_MAP_ROW;
		if (!(map[t][x].glyph == cmap_to_glyph(S_stone))) {
		    curcol = x;
		    currow = y;
		    g_attribute = map[t][x].attr;
		    alleg_xputg(map[t][x].glyph, map[t][x].ch);
		}
	    }

	currow = old_currow;
	curcol = old_curcol;

	need_update = 1;
}

void
alleg_userpan(dir)
int dir;
{
        int i;
        int delta = 10;

        for(i = 0; i < Y_HEIGHT/2; i+=delta)
        {
                switch(dir)
                {
                        case 0: clipwinx-=delta; break;
                        case 1: clipwinx+=delta; break;
                        case 2: clipwiny-=delta; break;
                        case 3: clipwiny+=delta; break;
                }

                if(dir < 2) {
                        if(clipwinx < 0)
                        { clipwinx = 0; i=1000; }
                        if(clipwinx > ((80*tile_x) - X_RES))
                        { clipwinx = 80*tile_x - X_RES; i=1000; }
                } else {
                        if(Y_HEIGHT >= 21*tile_y)
                        { clipwiny = tile_y-(Y_HEIGHT-21*tile_y)/2; return; }
                        else
                        {
                          if(clipwiny < tile_y)
                          { clipwiny = tile_y; i=1000; }
                          if(clipwiny > ((22*tile_y) - Y_HEIGHT))
                          { clipwiny = 22*tile_y - Y_HEIGHT; i=1000; }
                        }
                }

                need_update = 1;
                alleg_redrawmap(0);
        }
}

/* take a sceeenshot */
void alleg_screenshot()
{
        BITMAP *buffer = create_bitmap(X_RES,Y_RES);

        blit(screen, buffer, 0, 0, 0, 0, X_RES,Y_RES);

        save_bitmap("nethack.bmp", buffer, tilepal);
        save_bitmap("nhmap.bmp", subscreen, tilepal);
        return;
}

/* Currently does nothing */
void alleg_traditional(on)
boolean on;
{
	alleg_overview(FALSE);

	if (on) {
		iflags.traditional_view = TRUE;
		tile_x = FONTX;
		tile_y = FONTY;
	} else {
		iflags.traditional_view = FALSE;
		tile_x = TILEX;
		tile_y = TILEY;
	}

	alleg_cliparound(u.ux, u.uy);

	need_update = 2;

	alleg_redrawmap(1);

        return;
}

void alleg_refresh()
{
        positionbar();
        alleg_redrawmap(0);
        alleg_DrawCursor();
}

void alleg_vid_refresh()
{
        alleg_redrawmap(1);
}

STATIC_OVL void
alleg_scrollmap(left)
boolean left;
{
        return;
}

int
string_to_int(buf)
char *buf;
{
        char *bufp;
        int result = 0;

        bufp = buf;
        while (*bufp && (*bufp >= '0') && (*bufp <= '9') )
        {
                result *= 10;
                result += *bufp - '0';
                bufp++;
        }
        return result;
}

/*
 * Open tile files,
 * initialize the SCREEN, switch it to graphics mode,
 * initialize the pointers to the fonts, clear
 * the screen.
 *
 */

#define TOTAL_PROGRESS TOTAL_TILES_USED+NUM_SUBSTITUTES+10+224+16

static
void init_progress_meter()
{
        BITMAP *progress_meter = create_bitmap(TOTAL_PROGRESS, 30);

	/* Progress display bar box */
	rect(screen, 0, Y_RES-64, X_RES-1, Y_RES-33,
        	        colorpal[CLR_WHITE]);

	/* Fill with empty color */
        clear_to_color(progress_meter, colorpal[CLR_BLUE]);

	/* No anti-alias */
        stretch_blit(progress_meter, screen, 0, 0, TOTAL_PROGRESS,
                     30, 1, Y_RES-63, X_RES-2, 30);

}

static
void inc_progress_meter()
{
        BITMAP *progress_meter = create_bitmap(TOTAL_PROGRESS, 30);
        static int i = 0;

        i++;

        clear_to_color(progress_meter, colorpal[CLR_BLUE]);

        rectfill(progress_meter, 0, 0, i, 30, colorpal[CLR_GREEN]);

	/* No anti-alias */
        stretch_blit(progress_meter, screen, 0, 0, TOTAL_PROGRESS,
                     30, 1, Y_RES-63, X_RES-2, 30);

        destroy_bitmap(progress_meter);
}

/* The initialization function is so big, it's now a seperate file */
#include "alginit.h"

/*
 * This allows grouping of several tasks to be done when
 * switching back to text mode. This is a public (extern) function.
 *
 * Note that this should not deallocate memory, etc.,  as it is often paired
 * with alleg_Init()
 */
void alleg_Finish(void)
{
     int i;
#if 0
     windowprocs.win_cliparound = tty_cliparound;
#endif

     g_attribute = attrib_text_normal;
     alleg_SwitchMode(ALLEG_MODETEXT);

/*
     iflags.tile_view = FALSE;

     destroy_bitmap(txt_subscreen);
     destroy_bitmap(tile_subscreen);
     destroy_bitmap(cursorbmp);
     destroy_bitmap(ovcursor);
     destroy_bitmap(font_cursor);
     destroy_bitmap(errorbmp);
     destroy_bitmap(under_cursor);
     destroy_bitmap(alltiles);
     for(i=0; i < TOTAL_TILES_USED; i++)
     {
        destroy_bitmap(tilecache[i]);
     }
     allegro_exit(); */
}

/*
 * Write character 'ch', at (x,y) and
 * do it using the colour 'colour'.
 *
 */
void
alleg_WriteChar(chr,col,row,colour)
int chr,col,row,colour;
{
        int actual_colour = colorpal[colour];
        int x,y;
        char buf[2];

        x = min(col,(CO-1));   /* min() used protection from callers */
        y = min(row,(LI-1));

        buf[0] = chr;
        buf[1] = 0;

        alleg_text(buf, x*FONTX, y*FONTY, actual_colour);
}

void
alleg_update_positionbar(posbar)
char *posbar;
{
#if 1
        return;
#else
        char *p = pbar;
        if (posbar) while (*posbar) *p++ = *posbar++;
        *p = 0;
#endif
}

STATIC_OVL void
positionbar()
{
        return;
}

int oldcursx = -1;
int oldcursy = -1;

void
alleg_DrawCursor()
{
        int pixelx, pixely;
        int x,y;

        if(!inmap)
                return;

        x = min(curcol,(CO - 1));
        y = min(currow,(LI - 1));

        if (draw_3Dtiles && !iflags.traditional_view) {
        	pixelx = posxy_to_posx3d(x*tile_x, y*tile_y) - clipwinx;
        	pixely = posy_to_posy3d(y*tile_y) - clipwiny + FONTY;
        } else {
        	pixelx = x*tile_x - clipwinx;
        	pixely = y*tile_y - clipwiny + FONTY;
        }

        if( (oldcursx != x) || (oldcursy != y) )
        {
                if(!did_update)
                {
                        need_update = 1;
                        alleg_redrawmap(0);
                }
                did_update = 0;
                oldcursx = x;
                oldcursy = y;
        }


	if(iflags.over_view)
        {
                masked_blit(ovcursor, screen, 0, 0, (x*X_RES)/80,
                    (y*Y_HEIGHT)/23+FONTY, X_RES/80, Y_HEIGHT/23);
        } else if (
#ifdef REINCARNATION
            Is_rogue_level(&u.uz) ||
#endif
            iflags.traditional_view) {
	        line(screen, pixelx, pixely+FONTY,
	        	pixelx+FONTX, pixely+FONTY, colorpal[CLR_WHITE]);
	        return;
	} else {
                draw_sprite(screen, cursorbmp, pixelx, pixely);
#if 0
                masked_blit(cursorbmp, screen, 0, 0, pixelx, pixely,
                            tile_x, tile_y);
#endif
	}
}

void
alleg_HideCursor()
{
        int pixelx, pixely;
        int x,y;

        if(!inmap)
                return;

        x = min(curcol,(CO - 1));
        y = min(currow,(LI - 1));

        pixelx = x*tile_x - clipwinx;
        pixely = y*tile_y - clipwiny + FONTY;
}

/* ##################################################################### */
/* Let the special effects begin. */

void
delayfx()
{
        int i;
        for(i = 0; i < fx_delay; i++)
                vsync();
}

void
cleanup_explosions()
{
        no_update = 0;
        need_update = 1;
        alleg_redrawmap(1);
}

#define maxof4(a,b,c,d) max(max(a,b),max(c,d))

/* Integer square root function without using floating point. */
static int
lsqrt(val)
long val;
{
    long rt = 0;
    long odd = 1;
    while(val >= odd) {
        val = val-odd;
        odd = odd+2;
        rt = rt + 1;
    }
    return rt;
}

void
nh_fade_in(portal)
int portal;
{
        int factor = 64;
        BITMAP *buffer = create_bitmap(X_RES, Y_HEIGHT);
        BITMAP *buffer2;

        if(iflags.over_view)
                nh_stretch_blit(subscreen, buffer, 0, 0, 80*tile_x, 23*tile_y,
                             0, 0, X_RES, Y_HEIGHT);
        else
                blit(subscreen, buffer, clipwinx, clipwiny,
                             0, 0, X_RES, Y_HEIGHT);
        while(factor > 1)
        {
		if(portal)
		    buffer2 = blur(buffer, factor);
		else
		    buffer2 = mosaic(buffer, factor);
                blit(buffer2, screen, 0, 0, 0, FONTY, X_RES, Y_HEIGHT);
                destroy_bitmap(buffer2);
                delayfx();
                factor-=3;
        }
        destroy_bitmap(buffer);
        faded_out = 0;
}


void
nh_fade_out(portal)
int portal;
{
        int factor = 1;
        BITMAP *buffer = create_bitmap(X_RES, Y_HEIGHT);
        BITMAP *buffer2;
        blit(screen, buffer, 0, FONTY, 0, 0, X_RES, Y_HEIGHT);

        while(factor <= 64)
        {
		if(portal)
		    buffer2 = blur(buffer, factor);
		else
		    buffer2 = mosaic(buffer, factor);
                blit(buffer2, screen, 0, 0, 0, FONTY, X_RES, Y_HEIGHT);
                destroy_bitmap(buffer2);
                delayfx();
		factor+=3;
        }
        destroy_bitmap(buffer);
        faded_out = 1;
}

void
fade_to_black()
{
        int pixelx = u.ux*tile_x - clipwinx - (tile_x/2);
        int pixely = u.uy*tile_y - clipwiny + (tile_y/2) + FONTY;
        BITMAP *circlebmp = create_bitmap(X_RES, Y_HEIGHT);
        int i;
        long start_rad, incr, rt, odd;

        no_update = 0;

        if (draw_3Dtiles) {
        	pixelx = colrow_to_posx3d(u.ux, u.uy) - clipwinx - (tile_x/2);
        	pixely = row_to_posy3d(u.uy) - clipwiny + (tile_y/2) + FONTY;
        }

        /* use the furthest corner to determine the starting radius of the
           circle */
        start_rad = maxof4(abs(dist2(pixelx, pixely, 0, FONTY)),
                           abs(dist2(pixelx, pixely, X_RES, FONTY)),
                           abs(dist2(pixelx, pixely, 0, Y_HEIGHT+FONTY)),
                           abs(dist2(pixelx, pixely, X_RES, Y_HEIGHT+FONTY)));

        start_rad = lsqrt(start_rad);

        if(u.uswallow)
                start_rad = 64;

        start_rad += 90;
        player_dead = 1;

        incr = max(start_rad/80,1);

        for(i=start_rad; i >= 0; i-=incr)
        {
                int v,w;
                v = max(i-90,0);
                w = i;
                clear_to_color(circlebmp, makecol(255,0,255));
                circlefill(circlebmp, pixelx, pixely,
                           w, 0);
                circlefill(circlebmp, pixelx, pixely,
                           v, makecol(255,0,255));

		if (alleg_colordepth != 8)
		    set_trans_blender(0,0,0,max(incr*2,8));

                draw_trans_sprite(screen, circlebmp, 0, FONTY);
                delayfx();
        }
}

int
alleg_swallowed(x,y)
int x,y;
{
        int mpixelx = x*tile_x - clipwinx - tile_x*2;
        int mpixely = y*tile_y - clipwiny + FONTY;
        int upixelx = x*tile_x - clipwinx - tile_x;
        int upixely = y*tile_y - clipwiny + FONTY + tile_y;
        int v;
        BITMAP* utile;
        BITMAP* mtile;

        if(iflags.over_view || iflags.traditional_view)
                return 0;

        if (draw_3Dtiles) {
        	upixelx = colrow_to_posx3d(x - 1, y + 1) - clipwinx;
        	upixely = row_to_posy3d(y + 1) - clipwiny + FONTY;
        }

	mpixelx = upixelx - tile_x;
	mpixely = upixely - tile_y;

	if (alleg_colordepth != 8)
            set_trans_blender(0, 0, 0, 128);

        mtile = subst_mon(monsndx(u.ustuck->data), u.ustuck->female, u.ustuck->m_lev);

        utile = getutile();

        nh_stretch_sprite(screen, mtile, mpixelx, mpixely, tile_x*3, tile_y*3);
        draw_trans_sprite(screen, utile, upixelx, upixely);

        if(player_dead)
        {
            drawing_mode(DRAW_MODE_TRANS, screen, 0, 0);
	    if (alleg_colordepth != 8)
        	set_trans_blender(0,0,0,128);
            rectfill(screen, mpixelx, mpixely, mpixelx+(tile_x*3), mpixely+(tile_y*3), 0);
            drawing_mode(DRAW_MODE_SOLID, screen, 0, 0);
        }

        return 1;
}

void
alleg_aura(x,y,skill)
int x,y,skill;
{
        int pixelx = x*tile_x - clipwinx - tile_x*3/2;
        int pixely = y*tile_y - clipwiny + FONTY + tile_y/2;
        BITMAP* undersh;
        BITMAP* buffer2;
        int i;
        int type = skill - P_ATTACK_SPELL + 1;

        if(iflags.over_view)
                return;

        if (draw_3Dtiles) {
        	pixelx = colrow_to_posx3d(x, y) - clipwinx - tile_x*3/2;
        	pixely = row_to_posy3d(y) - clipwiny + FONTY;
        }

        if(pixelx < 0 || pixely < 0 ||
           pixelx>X_RES || pixely > (Y_RES-3*FONTY))
                return;

	undersh=create_bitmap(tile_x*2,tile_y*2);

        need_update = 1;
        alleg_redrawmap(1);

        blit(screen, undersh, pixelx, pixely, 0, 0, tile_x*2, tile_y*2);

        for(i=0; i < 28; i++)
        {
	    if (alleg_colordepth != 8)
        	set_trans_blender(0, 0, 0, 255-5*i);
            draw_sprite(screen, undersh, pixelx, pixely);
            draw_trans_sprite(screen, auras[type][i], pixelx, pixely);
            delayfx();
        }
        draw_sprite(screen, undersh, pixelx, pixely);
        destroy_bitmap(undersh);
}

int
alleg_shield(x,y)
int x,y;
{
        int pixelx = x*tile_x - clipwinx - tile_x;
        int pixely = y*tile_y - clipwiny + FONTY + tile_y;
        BITMAP* undersh;
        BITMAP* buffer2;
        int i;

        if(iflags.over_view)
                return 0;

        if (draw_3Dtiles) {
        	pixelx = colrow_to_posx3d(x, y) - clipwinx - tile_x;
        	pixely = row_to_posy3d(y) - clipwiny + FONTY + tile_y/2;
        }

        if(pixelx < 0 || pixely < 0 ||
           pixelx>X_RES || pixely > (Y_RES-3*FONTY))
                return 1;

	undersh=create_bitmap(tile_x,tile_y);

        need_update = 1;
        alleg_redrawmap(0);

        blit(screen, undersh, pixelx, pixely, 0, 0, tile_x, tile_y);

        for(i=0; i < 16; i++)
        {
	    if (alleg_colordepth != 8)
        	set_trans_blender(0, 0, 0, 192-8*i);

            draw_sprite(screen, undersh, pixelx, pixely);
            draw_trans_sprite(screen, shields[i], pixelx, pixely);
            delayfx();
        }
        draw_sprite(screen, undersh, pixelx, pixely);

        destroy_bitmap(undersh);

        return 1;
}

void
alleg_explode(x,y,adtyp)
int x,y,adtyp;
{
        int pixelx = x*tile_x - clipwinx - tile_x*2;
        int pixely = y*tile_y - clipwiny + FONTY;
        int expl_x = 3*tile_x;
        int expl_y = 3*tile_y;
        BITMAP *buffer, *underexp, *buffer2;
        int i;

        if (draw_3Dtiles) {
        	pixelx = colrow_to_posx3d(x, y) - clipwinx - TILE_3D_WIDTH - tile_x;
        	pixely = row_to_posy3d(y) - clipwiny + FONTY;
        	expl_x = tile_x + 2*TILE_3D_WIDTH;
        	expl_y = tile_y + 2*TILE_3D_HEIGHT;
        }

        buffer=create_bitmap(expl_x,expl_y);
	underexp=create_bitmap(expl_x,expl_y);

        if(iflags.over_view)
                alleg_overview(0);

        if(Hallucination)
                adtyp = rn2(SIZE(explosions));
        else if (adtyp < 0 || adtyp > SIZE(explosions))
        	adtyp = 1; /* Magical explosion */

        no_update = 1;

        blit(screen, underexp, pixelx, pixely, 0, 0, expl_x, expl_y);

        for(i = (min((expl_x*5/12), (expl_y*5/12))); i > 0; i-=2)
        {
            clear_to_color(buffer, makecol(255, 0, 255));

	    /* explosions are 96x96 */
            nh_stretch_blit(explosions[adtyp], buffer, 0, 0,
            		explosions[adtyp]->w, explosions[adtyp]->h,
                        i, i, (expl_x-(2*i)), (expl_y-(2*i)));

            if(adtyp == AD_ELEC || adtyp == AD_MAGM)
            {
                buffer2=create_bitmap(expl_x,expl_y);
                clear_to_color(buffer2, makecol(255, 0, 255));
                rotate_sprite(buffer2, buffer, 0, 0, itofix(rn2(256)));
                destroy_bitmap(buffer);
                buffer = buffer2;
            }

	    if (alleg_colordepth != 8)
		set_trans_blender(0, 0, 0, min(255,100+4*i));

            draw_sprite(screen, underexp, pixelx, pixely);
            draw_trans_sprite(screen, buffer, pixelx, pixely);
            draw_trans_sprite(screen, buffer, pixelx, pixely);
            delayfx();
        }
/*        draw_sprite(screen, underexp, pixelx, pixely); */
        destroy_bitmap(buffer);
        destroy_bitmap(underexp);
}

/* Use this to heopfully prevent unnecessary mode switches
 * Same return values as set_gfx_mode (0 on success, negative otherwise)
 */
int
alleg_SwitchMode(mode)
int mode;
{
    static curr_mode = ALLEG_MODE_NONE;
    int retval = 0;

    if (curr_mode == mode) return retval;

    if (mode == ALLEG_MODEGFX) {
     	retval = set_gfx_mode(video_mode, X_RES, Y_RES, 0, 0);
      	/* Set palette for 8-bit mode */
       	if (alleg_colordepth == 8) {
       		set_palette(tilepal);
       	}
    } else if (mode == ALLEG_MODETEXT) {
	retval = set_gfx_mode(GFX_TEXT,80,25,0,0);
    }

    curr_mode = mode;
    return retval;
}

void nh_stretch_blit(source, dest, source_x, source_y, source_width, source_height,
			dest_x, dest_y, dest_width, dest_height)
BITMAP *source;
BITMAP *dest;
int source_x, source_y, source_width, source_height;
int dest_x, dest_y, dest_width, dest_height;
{

    if (use_2xsai) {

	BITMAP *tmptile, *tmptile2;
	tmptile = new_bitmap(source_width, source_height + 1);

        clear_to_color(tmptile, makecol(0,0,0));

	blit(source, tmptile, source_x, source_y, 0, 0,
			source_width, source_height + 1);

	tmptile2 = new_bitmap(source_width * 2, (source_height * 2) + 2);

        clear_to_color(tmptile2, makecol(0,0,0));

	switch (use_2xsai) {
	    case 3:
		Super2xSaiBlit(tmptile, tmptile2);
		break;
	    case 2:
		SuperEagleBlit(tmptile, tmptile2);
		break;
	    case 1:
	    default:
		_2xSaiBlit(tmptile, tmptile2);
		break;
	}

	stretch_blit(tmptile2, dest, 0, 0, source_width * 2 , source_height * 2,
		dest_x, dest_y, dest_width, dest_height);

    	destroy_bitmap(tmptile);
    	destroy_bitmap(tmptile2);
    } else aa_stretch_blit(source, dest, source_x, source_y, source_width, source_height,
			dest_x, dest_y, dest_width, dest_height);
}

/* vidalleg.c */



/*   SCCS Id: @(#)vidvga.c   3.2     96/02/16			    */
/*   Copyright (c) NetHack PC Development Team 1995                 */
/*   NetHack may be freely redistributed.  See license for details. */
/*
 * vidalleg.c - Allegro video support - WAC's version [non-working]
 */

#include "hack.h"

#ifdef USE_ALLEGRO               /* this file is for USE_ALLEGRO only    */
#include "pcvideo.h"
#include "tile.h"
#include "pctiles.h"

#include <dos.h>
#include <ctype.h>
#include "wintty.h"

# ifdef __GO32__
#include <pc.h>
#include <unistd.h>
# endif

#include <allegro.h>

void FDECL(alleg_gotoloc, (int,int));  /* This should be made a macro */
void NDECL(alleg_backsp);
#ifdef SCROLLMAP
STATIC_DCL void FDECL(alleg_scrollmap,(BOOLEAN_P));
#endif
STATIC_DCL void FDECL(alleg_redrawmap,(BOOLEAN_P));
void FDECL(alleg_cliparound,(int, int));

#ifdef POSITIONBAR
STATIC_DCL void NDECL(positionbar);
static void FDECL(alleg_special,(int, int, int));
#endif

extern int clipx, clipxmax;	/* current clipping column from wintty.c */
extern boolean clipping;	/* clipping on? from wintty.c */
extern int savevmode;		/* store the original video mode */
extern int curcol,currow;	/* current column and row        */
extern int g_attribute;
extern int attrib_text_normal;	/* text mode normal attribute */
extern int attrib_gr_normal;	/* graphics mode normal attribute */
extern int attrib_text_intense;	/* text mode intense attribute */
extern int attrib_gr_intense;	/* graphics mode intense attribute */
extern boolean inmap;		/* in the map window */

/*
 * Global Variables
 */

STATIC_VAR BITMAP *screentable[SCREENHEIGHT];
STATIC_VAR BITMAP *planar_tile_bmp;
STATIC_VAR BITMAP *overview_tile_bmp;
STATIC_VAR char tmp[SCREENWIDTH];
STATIC_VAR PALETTE *paletteptr;
STATIC_VAR struct map_struct {
	int glyph;
	int ch;
	int attr;
}  map[ROWNO][COLNO];	/* track the glyphs */

# define alleg_clearmap() { int x,y; for (y=0; y < ROWNO; ++y) \
	for (x=0; x < COLNO; ++x) { map[y][x].glyph = cmap_to_glyph(S_stone); \
	map[y][x].ch = S_stone; map[y][x].attr = 0;} }
# define TOP_MAP_ROW 1
#  if defined(OVLB)
STATIC_VAR int vgacmap[CLR_MAX] = {0,3,5,9,4,8,12,14,11,2,6,7,1,8,12,13};
STATIC_VAR int viewport_size = 50;
STATIC_VAR char masktable[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
STATIC_VAR char bittable[8]= {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
STATIC_VAR PALETTE defpalette[] = {        /* Default Allegro palette ? */
        {0, 0, 0},
        {0, 182, 255},
        {255, 108, 0},
        {255, 0, 0},
        {0, 0, 255},
        {0, 145, 0},
        {108, 255, 0},
        {255, 255, 0},
        {255, 0, 255},
        {145, 71, 0},
        {182, 71, 0},
        {255, 182, 145},
        {71, 108, 108},
        {255, 255, 255},
        {218, 218, 182},
        {108, 145, 182},        
	};
#   ifndef ALTERNATE_VIDEO_METHOD
int vp[SCREENPLANES] = {8,4,2,1};
#   endif
int vp2[SCREENPLANES] = {1,2,4,8};
#  else
extern int vgacmap[CLR_MAX];
extern int viewport_size;
extern char masktable[8];
extern char bittable[8];
extern char defpalette[];
#   ifndef ALTERNATE_VIDEO_METHOD
extern int vp[SCREENPLANES];
#   endif
extern int vp2[SCREENPLANES];
#  endif /* OVLB */

STATIC_VAR struct planar_cell_struct *planecell;
STATIC_VAR struct overview_planar_cell_struct *planecell_O;

# if defined(USE_TILES)
STATIC_VAR struct tibhdr_struct tibheader;
/* extern FILE *tilefile; /* Not needed in here most likely */
# endif

/* STATIC_VAR int  g_attribute;		/* Current attribute to use */

/*
 * Write character 'ch', at (x,y) and
 * do it using the colour 'colour'.
 *
 */
#define alleg_WriteChar(chr,col,row,colour)     textout(screen,font,chr,col,row,colour)

#ifdef OVLB
void
alleg_get_scr_size()
{
        CO = 100;
        LI = 36;
}
#endif /*OVLB*/



# ifdef OVLB

void
alleg_backsp()
{
	int col,row;

	col = curcol; 		/* Character cell row and column */
	row = currow;

	if (col > 0) col = col-1;
        alleg_gotoloc(col,row);
        alleg_xputc(' ',g_attribute);
        alleg_gotoloc(col,row);
}

# endif /* OVLB */
# ifdef OVL0

void
alleg_clear_screen(colour)
int colour;
{
        clear_to_color(screen, colour);
        alleg_gotoloc(0,0);       /* is this needed? */
}

void
alleg_cl_end(col,row)     /* clear to end of line */
int col,row;
{
	int count;

	/*
	 * This is being done via character writes.
	 * This should perhaps be optimized for speed by using VGA write
	 * mode 2 methods as did clear_screen()
	 */
	for (count = col; count < (CO-1); ++count) {
                alleg_WriteChar(' ',count,row,BACKGROUND_alleg_COLOR);
	}
}

void
alleg_cl_eos(cy)  /* clear to end of screen */
int cy;
{
	int count;

	cl_end();
	while(cy <= LI-2) {
		for (count = 0; count < (CO-1); ++count) {
                        alleg_WriteChar(' ',count,cy,
                                BACKGROUND_alleg_COLOR);
		}
		cy++;
	}
}


# endif /* OVL0 */

# ifdef OVLB
void
alleg_tty_end_screen()
{
        alleg_clear_screen(BACKGROUND_alleg_COLOR);
        alleg_SwitchMode(MODETEXT);
}


void
alleg_tty_startup(wid, hgt)
    int *wid, *hgt;
{

	/* code to sense display adapter is required here - MJA */

        alleg_get_scr_size();
	if (CO && LI) {
		*wid = CO;
		*hgt = LI;
	}

        attrib_gr_normal    = ATTRIB_alleg_NORMAL;
        attrib_gr_intense   = ATTRIB_alleg_INTENSE;
	g_attribute         = attrib_gr_normal;	/* Give it a starting value */
}
# endif /* OVLB */

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

# ifdef OVL0
void
alleg_xputs(s,col,row)
const char *s;
int col,row;
{

	if (s != (char *)0) {
                alleg_WriteStr((char *)s,strlen(s),col,row,g_attribute);
	}
}

void
alleg_xputc(ch,attr)      /* write out character (and attribute) */
char ch;
int attr;
{
	int col,row;

	col = curcol;
	row = currow;

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
}

#  if defined(USE_TILES)
void
alleg_xputg(glyphnum,ch)  /* Place tile represent. a glyph at current location */
int glyphnum;
int ch;
{
	int col,row;
	int attr;

	row = currow;
	col = curcol;
	if ((col < 0 || col >= COLNO) ||
	    (row < TOP_MAP_ROW || row >= (ROWNO + TOP_MAP_ROW))) return;
	map[row - TOP_MAP_ROW][col].glyph = glyphnum;
	map[row - TOP_MAP_ROW][col].ch = ch;
	attr = (g_attribute == 0) ? attrib_gr_normal : g_attribute;
	map[row - TOP_MAP_ROW][col].attr = attr;
	if (iflags.traditional_view) {
            alleg_WriteChar((unsigned char)ch,col,row,attr);
	} else if (!iflags.over_view) {
	    if ((col >= clipx) && (col <= clipxmax)) {
		if (!ReadPlanarTileFile(glyph2tile[glyphnum], &planecell))
                        alleg_DisplayCell(planecell, 
					col - clipx, row);
		else
                        pline("alleg_xputg: Error reading tile (%d,%d) from file",
					glyphnum,glyph2tile[glyphnum]);
	    }
	} else {
	    if (!ReadPlanarTileFile_O(glyph2tile[glyphnum], &planecell_O))
                        alleg_DisplayCell_O(planecell_O, col, row);
	    else
                        pline("alleg_xputg: Error reading tile (%d,%d) from file",
					glyphnum,glyph2tile[glyphnum]);
	}
	if (col < (CO - 1 )) ++col;
        alleg_gotoloc(col,row);
}
#  endif /* USE_TILES */

/*
 * Cursor location manipulation, and location information fetching
 * routines.
 * These include:
 *
 * alleg_gotoloc(x,y)  - Moves the "cursor" on screen to the specified x
 *			 and y character cell location.  This routine
 *                       determines the location where screen writes
 *                       will occur next, it does not change the location
 *                       of the player on the NetHack level.
 */
 
void
alleg_gotoloc(col,row)
int col,row;
{
	curcol = min(col,CO - 1); /* protection from callers */
	currow = min(row,LI - 1);
}

#  if defined(USE_TILES) && defined(CLIPPING)
void
alleg_cliparound(x, y)
int x, y;
{
	extern boolean restoring;
	int oldx = clipx;

	if (!iflags.tile_view || iflags.over_view || iflags.traditional_view)
		return;

	if (x < clipx + 5) {
		clipx = max(0, x - (viewport_size / 2));
		clipxmax = clipx + (viewport_size - 1);
	}
	else if (x > clipxmax - 5) {
		clipxmax = min(COLNO - 1, x + (viewport_size / 2));
		clipx = clipxmax - (viewport_size - 1);
	}
	if (clipx != oldx) {
	    if (on_level(&u.uz0, &u.uz) && !restoring)
		/* (void) doredraw(); */
                alleg_redrawmap(1);
	}
}

STATIC_OVL void
alleg_redrawmap(clearfirst)
boolean clearfirst;
{
	int j,x,y,t;
	char __far *pch;
	char volatile a;

	if (clearfirst) {
		/* y here is in pixel rows */
		outportb(0x3ce,5);
		outportb(0x3cf,2);
		t = TOP_MAP_ROW * ROWS_PER_CELL;
		for (y = t; y < (ROWNO * ROWS_PER_CELL) + t; ++y) {
			pch = screentable[y];
			for (j=0; j < SCREENBYTES; ++j) {
				outportb(0x3ce,8);
				outportb(0x3cf,255);
				 /* On VGA mode2, must read first, then write */
				a = READ_ABSOLUTE(pch);
                                WRITE_ABSOLUTE(pch, (char)BACKGROUND_alleg_COLOR);
				++pch;
			}
		}
		outportb(0x3ce,5);
		outportb(0x3cf,0);
	}
	/* y here is in screen rows*/
#    ifdef ROW_BY_ROW
	for (y = 0; y < ROWNO; ++y)
		for (x = clipx; x <= clipxmax; ++x) {
#    else
	for (x = clipx; x <= clipxmax; ++x)
		for (y = 0; y < ROWNO; ++y) {
#    endif
		    if (iflags.traditional_view) {
			if (!(clearfirst && map[y][x].ch == S_stone))
                                alleg_WriteChar(
					(unsigned char)map[y][x].ch,
					x,y + TOP_MAP_ROW,map[y][x].attr);
		    } else {
		      t = map[y][x].glyph;
		      if (!(clearfirst && t == cmap_to_glyph(S_stone))) {
			if (!iflags.over_view) {
			  	if (!ReadPlanarTileFile(glyph2tile[t], 
				    &planecell)) {
                                        alleg_DisplayCell(planecell,
						x - clipx, y + TOP_MAP_ROW);
		  	  	} else
                              pline("alleg_redrawmap: Error reading tile (%d,%d)",
					 t,glyph2tile[t]);
		     	} else {
				if (!ReadPlanarTileFile_O(glyph2tile[t], 
				     &planecell_O)) {
                                        alleg_DisplayCell_O(planecell_O,
						x, y + TOP_MAP_ROW);
		  	  	} else
                             pline("alleg_redrawmap: Error reading tile (%d,%d)",
					t,glyph2tile[t]);
		  	}
		      }
		    }
		}
}
#  endif /* USE_TILES && CLIPPING */
# endif /* OVL0 */
# ifdef OVL2

void
alleg_userpan(left)
boolean left;
{
	int x;

/*	pline("Into userpan"); */
	if (iflags.over_view || iflags.traditional_view) return;
	if (left)
		x = min(COLNO - 1, clipxmax + 10);
	else 
		x = max(0, clipx - 10);
        alleg_cliparound(x, 10);  /* y value is irrelevant on VGA clipping */
	positionbar();
        alleg_DrawCursor();
}


void alleg_overview(on)
boolean on;
{
/*      alleg_HideCursor(); */
	if (on) {
		iflags.over_view = TRUE;
		clipx = 0;
		clipxmax = CO - 1;
	} else {
		iflags.over_view = FALSE;
		clipx = max(0, (curcol - viewport_size / 2));
		if (clipx > ((CO - 1) - viewport_size)) 
			clipx = (CO - 1) - viewport_size;
     		clipxmax = clipx + (viewport_size - 1);
	}
}

void alleg_traditional(on)
boolean on;
{
/*      alleg_HideCursor(); */
	if (on) {
/*		switch_graphics(ASCII_GRAPHICS); */
		iflags.traditional_view = TRUE;
		clipx = 0;
		clipxmax = CO - 1;
	} else {
		iflags.traditional_view = FALSE;
		if (!iflags.over_view) {
			clipx = max(0, (curcol - viewport_size / 2));
			if (clipx > ((CO - 1) - viewport_size)) 
				clipx = (CO - 1) - viewport_size;
     			clipxmax = clipx + (viewport_size - 1);
		}
	}
}

void alleg_refresh()
{
	positionbar();
        alleg_redrawmap(1);
        alleg_DrawCursor();
}

#  ifdef SCROLLMAP
STATIC_OVL void
alleg_scrollmap(left)
boolean left;
{
	int j,x,y,t;
	int i,pixx,pixy,x1,y1,x2,y2;
	int byteoffset, vplane;
	char __far *tmp1;
	char __far *tmp2;
	unsigned char source[SCREENPLANES][80];
	unsigned char first,second;

	
	pixy = row2y(TOP_MAP_ROW);		  /* convert to pixels */
	pixx = col2x(x1);
	if (left) {
		x1 = 20;
		x2 = 0;
	} else {
		x1 = 0;
		x2 = 20;
	}
	/* read each row, all columns but the one to be replaced */
	for(i = 0;i < (ROWNO-1) * ROWS_PER_CELL; ++i) {
	    tmp1 = screentable[i + pixy];
	    tmp1 += x1;
	    for(vplane=0; vplane < SCREENPLANES; ++vplane) {
		egareadplane(vplane);
		for (byteoffset = 0; byteoffset < 20; ++byteoffset) {
			tmp2 = tmp1 + byteoffset;
			source[vplane][byteoffset] = READ_ABSOLUTE(tmp2);
		}
	    }
	    tmp1 = screentable[i + pixy];
	    tmp1 += x2;
	    for(vplane=0; vplane < SCREENPLANES; ++vplane) {
		egawriteplane(vp2[vplane]);
		for (byteoffset = 0; byteoffset < 20; ++byteoffset) {
			tmp2 = tmp1 + byteoffset;
			WRITE_ABSOLUTE(tmp2,source[vplane][byteoffset]);
		}
	    }
	    egawriteplane(15);
	}

	if (left) {
		i = clipxmax - 1;
		j = clipxmax;
	} else {
		i = clipx;
		j = clipx + 1;
	}
	for (y = 0; y < ROWNO; ++y) {
	    for (x = i; x < j; x += 2) {
		t = map[y][x].glyph;
		if (!ReadPlanarTileFile(glyph2tile[t], &planecell)) 
                        alleg_DisplayCell(planecell, x - clipx, y + TOP_MAP_ROW);
		else
                        pline("alleg_shiftmap: Error reading tile (%d,%d)",
				t, glyph2tile[t]);		
	    }
	}
}
#   endif /* SCROLLMAP */
# endif /* OVL2 */

# ifdef OVLB

/*
 * Open tile files,
 * initialize the SCREEN, switch it to graphics mode,
 * initialize the pointers to the fonts, clear
 * the screen.
 *
 */
void alleg_Init(void)
{
     int i, c;

     (void) allegro_init();

#   ifdef USE_TILES
     int tilefailure = 0;
/*
 * Attempt to open the required tile files. If we can't
 * don't perform the video mode switch, use TTY code instead.
 *
 */
/*
     if (alleg_OpenTileBMP(NETHACK_PLANAR_TILEFILE, FALSE)) tilefailure |= 1;
     if (alleg_OpenTileBMP(NETHACK_OVERVIEW_TILEFILE, TRUE)) tilefailure |= 2;      
     if (!(planar_tile_bmp = load_bitmap("ptile.bmp",paletteptr))) tilefailure |= 1;
     if (!(overview_tile_bmp = load_bitmap("otile.bmp", NULL))) tilefailure |= 2;      
     if (!paletteptr) tilefailure |= 4;
*/

     if (tilefailure) {
	raw_printf("Reverting to TTY mode, tile initialization failure (%d).",
		tilefailure);
	wait_synch();
	iflags.usevga = 0;
	iflags.tile_view = FALSE;
	iflags.over_view = FALSE;
	CO = 80;
	LI = 25;
/*       clear_screen()  /* not alleg_clear_screen() */
	return;
     }
#   endif

     if (iflags.usealleg) {
	for (i=0; i < SCREENHEIGHT; ++i) {
                screentable[i]=create_bitmap(800, 600);
                clear(screentable[i]);
	}
     }

     alleg_SwitchMode(MODE800x600);
     windowprocs.win_cliparound = alleg_cliparound;

#   ifdef USE_TILES
     iflags.tile_view = TRUE;
     iflags.over_view = FALSE;

     set_palette(paletteptr);
#   endif

     g_attribute  = attrib_gr_normal;
     clear_screen();
     clipx = 0;
     clipxmax = clipx + (viewport_size - 1);
}

/*
 * Switches modes of the video card.
 *
 * If mode == MODETEXT (0x03), then the card is placed into text
 * mode.  If mode == 640x480, then the card is placed into vga
 * mode (video mode 0x12). No other modes are currently supported.
 *
 */
void alleg_SwitchMode(unsigned int mode)
{
        if ((mode == MODE800x600) || (mode == MODETEXT)) {
                if (iflags.usealleg && (mode == MODE800x600)) {
     			iflags.grmode = 1;
		} else {
	        	iflags.grmode = 0;
		}
                (void) set_gfx_mode(GFX_AUTODETECT,800,600,0,0);
	} else {
		iflags.grmode = 0;	/* force text mode for error msg */
                (void) set_gfx_mode(GFX_TEXT,0,0,0,0);
		g_attribute  = attrib_text_normal;
                impossible("alleg_SwitchMode: Bad video mode requested 0x%X",
			mode);
	}
}

/*
 * This allows grouping of several tasks to be done when
 * switching back to text mode. This is a public (extern) function.
 *
 */
void alleg_Finish(void)
{
     CloseTileFile(0);
     CloseTileFile(1);
     alleg_SwitchMode(MODETEXT);
     windowprocs.win_cliparound = tty_cliparound;
     g_attribute  = attrib_text_normal;
     iflags.tile_view = FALSE;
}

#if 0
/*
 * Turn off any border colour that might be enabled in the VGA card
 * register.
 *
 * I disabled this after modifying tile2bin.c to remap black & white
 * to a more standard values - MJA 94/04/23.
 *
 */
STATIC_OVL void 
alleg_NoBorder(int bc)
{
	union REGS regs;

	regs.h.ah = (char)0x10;
	regs.h.al = (char)0x01;
	regs.h.bh = (char)bc;
	regs.h.bl = 0;
	(void) int86(VIDEO_BIOS, &regs, &regs);	
}
#endif

/*
 * This will verify the existance of a VGA adapter on the machine.
 * Video function call 0x1a returns 0x1a in AL if successful, and
 * returns the following values in BL for the active display:
 *
 * 0=no display, 1=MDA, 2=CGA, 4=EGA(color-monitor), 
 * 5=EGA(mono-monitor), 6=PGA, 7=VGA(mono-monitor), 8=VGA(color-monitor),
 * 0xB=MCGA(mono-monitor), 0xC=MCGA(color-monitor), 0xFF=unknown)
 */
int alleg_detect()
{
        return 1;
}

/*
 * This is the routine that displays a high-res "cell" pointed to by 'gp'
 * at the desired location (col,row).
 *
 * Note: (col,row) in this case refer to the coordinate location in
 * NetHack character grid terms, (ie. the 40 x 25 character grid),
 * not the x,y pixel location.
 *
 */
void
alleg_DisplayCell(gp,col,row)
struct planar_cell_struct *gp;
int col,row;
{
	int i,pixx,pixy;
	char __far *tmp_s;	/* source pointer */
	char __far *tmp_d;	/* destination pointer */
	int vplane;

	pixy = row2y(row);		/* convert to pixels */
	pixx = col2x(col);
	for(vplane=0; vplane < SCREENPLANES; ++vplane) {
		egawriteplane(vp[vplane]);
		for(i=0;i < ROWS_PER_CELL; ++i) {
			tmp_d = screentable[i+pixy];
 			tmp_d += pixx;
		/*
		 * memcpy((void *)tmp,(void *)gp->plane[vplane].image[i],
		 *         BYTES_PER_CELL);
		 */
			tmp_s = gp->plane[vplane].image[i];
			WRITE_ABSOLUTE(tmp_d, (*tmp_s));
			++tmp_s; ++tmp_d;
			WRITE_ABSOLUTE(tmp_d, (*tmp_s));
		}
	}
	egawriteplane(15);
}

void
alleg_DisplayCell_O(gp,col,row)
struct overview_planar_cell_struct *gp;
int col,row;
{
	int i,pixx,pixy;
	char __far *tmp_s;	/* source pointer */
	char __far *tmp_d;	/* destination pointer */
	int vplane;

	pixy = row2y(row);		/* convert to pixels */
	pixx = col;
	for(vplane=0; vplane < SCREENPLANES; ++vplane) {
		egawriteplane(vp[vplane]);
		for(i=0;i < ROWS_PER_CELL; ++i) {
			tmp_d = screentable[i+pixy];
 			tmp_d += pixx;
		/*
		 * memcpy((void *)tmp,(void *)gp->plane[vplane].image[i],
		 *         BYTES_PER_CELL);
		 */
			tmp_s = gp->plane[vplane].image[i];
			WRITE_ABSOLUTE(tmp_d, (*tmp_s));
		}
	}
	egawriteplane(15);
}

/*
 * Write the character string pointed to by 's', whose maximum length
 * is 'len' at location (x,y) using the 'colour' colour.
 *
 */
void 
alleg_WriteStr(s,len,col,row,colour)
char *s;
int len,col,row,colour;
{
	unsigned char *us;
	int i = 0;

	/* protection from callers */
	if (row > (LI-1)) return;

	i  = 0;
	us = (unsigned char *)s;
	while( (*us != 0) && (i < len) && (col < (CO - 1))) {
                alleg_WriteChar(*us,col,row,colour);
		++us;
		++i;
		++col;
	}
}

# endif /* OVLB */


# ifdef OVLB
/*static unsigned char colorbits[]={0x01,0x02,0x04,0x08}; /* wrong */
static unsigned char colorbits[]={0x08,0x04,0x02,0x01}; 

#ifdef POSITIONBAR

#define PBAR_ROW (LI - 4)
#define PBAR_COLOR_ON	  15	/* slate grey background colour of tiles */
#define PBAR_COLOR_OFF	  12	/* bluish grey, used in old style only */
#define PBAR_COLOR_STAIRS  9	/* brown */
#define PBAR_COLOR_HERO   14	/* creamy white */

static unsigned char pbar[COLNO];

void 
alleg_update_positionbar(posbar)
char *posbar;
{
	char *p = pbar;
	if (posbar) while (*posbar) *p++ = *posbar++;
	*p = 0;
}

STATIC_OVL void 
positionbar()
{
	char *posbar = pbar;
	int feature, ucol;
	int k, y, colour, row;
	char __far *pch;
	char bitblock;
	int startk, stopk;
	char volatile a;
	boolean nowhere = FALSE;
	int pixy = (PBAR_ROW * MAX_ROWS_PER_CELL);
	int tmp;

	if (!iflags.grmode || !iflags.tile_view) return;
	if ((clipx < 0)  || (clipxmax <= 0) || (clipx >= clipxmax)) 
		nowhere = TRUE;
	if (nowhere) {
#ifdef DEBUG
		pline("Would have put bar using %d - %d.",clipx,clipxmax);
#endif
		return;
        }
#ifdef OLD_STYLE
	outportb(0x3ce,5);
	outportb(0x3cf,2);
	for (y=pixy; y < (pixy + MAX_ROWS_PER_CELL); ++y) {
		pch = screentable[y];
		for (k=0; k < SCREENBYTES; ++k) {
			if ((k < clipx) || (k > clipxmax)) {
				colour = PBAR_COLOR_OFF;			
			} else colour = PBAR_COLOR_ON;
			outportb(0x3ce,8);
			outportb(0x3cf,255);
			a = READ_ABSOLUTE(pch); /* Must read , then write */
			WRITE_ABSOLUTE(pch, (char)colour);
			++pch;
		}
	}
	outportb(0x3ce,5);
	outportb(0x3cf,0);
#else
	colour = PBAR_COLOR_ON;
	outportb(0x3ce,5);
	outportb(0x3cf,2);
	for (y=pixy, row = 0; y < (pixy + MAX_ROWS_PER_CELL); ++y, ++row) {
		pch = screentable[y];
		if ((!row) || (row == (ROWS_PER_CELL-1))) {
			startk = 0;
			stopk  = SCREENBYTES;
		} else {
			startk = clipx;
			stopk  = clipxmax;
		}
		for (k=0; k < SCREENBYTES; ++k) {
			if ((k < startk) || (k > stopk))
                                colour = BACKGROUND_alleg_COLOR;
			else
				colour = PBAR_COLOR_ON;
			outportb(0x3ce,8);
			outportb(0x3cf,255);
			a = READ_ABSOLUTE(pch); /* Must read , then write */
			WRITE_ABSOLUTE(pch, (char)colour);
			++pch;
		}
	}
	outportb(0x3ce,5);
	outportb(0x3cf,0);
#endif
	ucol = 0;
	if (posbar) {
	    while (*posbar != 0) {
		feature = *posbar++;
		switch (feature) {
		    case '>':
                        alleg_special(feature, (int)*posbar++, PBAR_COLOR_STAIRS);
			break;
		    case '<':
                        alleg_special(feature, (int)*posbar++, PBAR_COLOR_STAIRS);
			break;
		    case '@':
			ucol = (int)*posbar++;
                        alleg_special(feature, ucol, PBAR_COLOR_HERO);
			break;
		    default: /* unanticipated symbols */
                        alleg_special(feature, (int)*posbar++, PBAR_COLOR_STAIRS);
			break;
		}
	    }
	}
#  ifdef SIMULATE_CURSOR
	if (inmap) {
		tmp = curcol + 1;
		if ((tmp != ucol) && (curcol >= 0))	
                        alleg_special('_', tmp, PBAR_COLOR_HERO);
	}
#  endif
}

void
alleg_special(chr,col,color)
int chr,col,color;
{
	int i,y,pixx,pixy;
	char __far *tmp_d;	/* destination pointer */
	int vplane;
	char fnt;
	char bits[SCREENPLANES][ROWS_PER_CELL];

	pixy = PBAR_ROW * MAX_ROWS_PER_CELL;
	for(vplane=0; vplane < SCREENPLANES; ++vplane) {
		egareadplane(vplane);
		y = pixy;
		for(i=0;i < ROWS_PER_CELL; ++i) {
			tmp_d = screentable[y++] + col;
			bits[vplane][i] = READ_ABSOLUTE(tmp_d);
			fnt = READ_ABSOLUTE((font + ((chr<<4) + i)));
			if (colorbits[vplane] & color)
				bits[vplane][i] |= fnt;
			else
				bits[vplane][i] &= ~fnt;
		}
	}
	for(vplane=0; vplane < SCREENPLANES; ++vplane) {
		egawriteplane(vp[vplane]);
		y = pixy;
		for(i=0;i < ROWS_PER_CELL; ++i) {
			tmp_d = screentable[y++] + col;
			WRITE_ABSOLUTE(tmp_d, (bits[vplane][i]));
		}
     	}
	egawriteplane(15);
}

#  endif POSITIONBAR

#  ifdef SIMULATE_CURSOR

static struct planar_cell_struct undercursor;
static struct planar_cell_struct cursor;

void
alleg_DrawCursor()
{
	int i,pixx,pixy,x,y,p;
	char __far *tmp1;
	char __far *tmp2;
	unsigned char first,second;
/*	char on[2] =  {0xFF,0xFF}; */
/*	char off[2] = {0x00,0x00}; */
	boolean isrogue = Is_rogue_level(&u.uz);
	boolean singlebyte = (isrogue || iflags.over_view
			      || iflags.traditional_view || !inmap);
	int curtyp;

	if (!cursor_type && inmap) return;	/* CURSOR_INVIS - nothing to do */

	x = min(curcol,(CO - 1)); /* protection from callers */
	y = min(currow,(LI - 1));		  /* protection from callers */
	if (!singlebyte && ((x < clipx) || (x > clipxmax))) return;
	    pixy = row2y(y);		  /* convert to pixels */
	    if (singlebyte)
		    pixx = x;
	    else
		    pixx = col2x((x-clipx));

	    for(i=0;i < ROWS_PER_CELL; ++i) {
		tmp1 = screentable[i+pixy];
 		tmp1 += pixx;
 		tmp2 = tmp1 + 1;
		egareadplane(3);
		/* memcpy(undercursor.plane[3].image[i],tmp1,BYTES_PER_CELL); */
		undercursor.plane[3].image[i][0] = READ_ABSOLUTE(tmp1);
		if (!singlebyte)
			undercursor.plane[3].image[i][1] = READ_ABSOLUTE(tmp2);

		egareadplane(2);
		/* memcpy(undercursor.plane[2].image[i],tmp1,BYTES_PER_CELL); */
		undercursor.plane[2].image[i][0] = READ_ABSOLUTE(tmp1);
		if (!singlebyte)
			undercursor.plane[2].image[i][1] = READ_ABSOLUTE(tmp2);

		egareadplane(1);
		/* memcpy(undercursor.plane[1].image[i],tmp1,BYTES_PER_CELL); */
		undercursor.plane[1].image[i][0] = READ_ABSOLUTE(tmp1);
		if (!singlebyte)
			undercursor.plane[1].image[i][1] = READ_ABSOLUTE(tmp2);

		egareadplane(0);
		/* memcpy(undercursor.plane[0].image[i],tmp1,BYTES_PER_CELL); */
		undercursor.plane[0].image[i][0] = READ_ABSOLUTE(tmp1);
		if (!singlebyte)
			undercursor.plane[0].image[i][1] = READ_ABSOLUTE(tmp2);
	    }

	    /*
             * Now we have a snapshot of the current cell.
             * Make a copy of it, then manipulate the copy
             * to include the cursor, and place the tinkered
             * version on the display.
             */ 

	    cursor = undercursor;
	    if (inmap) curtyp = cursor_type;
	    else curtyp = CURSOR_UNDERLINE;

	    switch(curtyp) {

		case CURSOR_CORNER:
		    for(i = 0; i < 2; ++i) {
			if (!i) {
				if (singlebyte) first = 0xC3;
				else first  = 0xC0;
				second = 0x03;
			} else {
				if (singlebyte) first = 0x81;
				else first  = 0x80;
				second = 0x01;
			}
			for (p=0; p < 4; ++p) {
				if (cursor_color & colorbits[p]) {
					cursor.plane[p].image[i][0] |= first;
					if (!singlebyte)
					cursor.plane[p].image[i][1] |= second;
				} else {
					cursor.plane[p].image[i][0] &= ~first;
					if (!singlebyte)
					cursor.plane[p].image[i][1] &= ~second;
				}
			}
		    }

		    for(i = ROWS_PER_CELL - 2; i < ROWS_PER_CELL; ++i) {
			if (i != (ROWS_PER_CELL-1)) {
				if (singlebyte) first = 0x81;
				else first  = 0x80;
				second = 0x01;
			} else {
				if (singlebyte) first = 0xC3;
				else first  = 0xC0;
				second = 0x03;
			}
			for (p=0; p < SCREENPLANES; ++p) {
				if (cursor_color & colorbits[p]) {
					cursor.plane[p].image[i][0] |= first;
					if (!singlebyte)
					cursor.plane[p].image[i][1] |= second;
				} else {
					cursor.plane[p].image[i][0] &= ~first;
					if (!singlebyte)
					cursor.plane[p].image[i][1] &= ~second;
				}
			}
		    }
		    break;

		case CURSOR_UNDERLINE:

		    i = ROWS_PER_CELL - 1;
		    first  = 0xFF;
		    second = 0xFF;
		    for (p=0; p < SCREENPLANES; ++p) {
			if (cursor_color & colorbits[p]) {
				cursor.plane[p].image[i][0] |= first;
				if (!singlebyte)
				cursor.plane[p].image[i][1] |= second;
			} else {
				cursor.plane[p].image[i][0] &= ~first;
				if (!singlebyte)
				cursor.plane[p].image[i][1] &= ~second;
			}
		    }
		    break;

		case CURSOR_FRAME:

		    /* fall through */

		default:			
		    for(i = 0; i < ROWS_PER_CELL; ++i) {

			if ((i == 0) || (i == (ROWS_PER_CELL-1))) {
				first  = 0xFF;
				second = 0xFF;
			} else {
				if (singlebyte) first = 0x81;
				else first  = 0x80;
				second = 0x01;
			}
			for (p=0; p < SCREENPLANES; ++p) {
				if (cursor_color & colorbits[p]) {
					cursor.plane[p].image[i][0] |= first;
					if (!singlebyte)
					cursor.plane[p].image[i][1] |= second;
				} else {
					cursor.plane[p].image[i][0] &= ~first;
					if (!singlebyte)
					cursor.plane[p].image[i][1] &= ~second;
				}
			}
		    }
		    break;
	    }

	   /*
            * Place the new cell onto the display.
            *
            */
	    
	    for(i=0;i < ROWS_PER_CELL; ++i) {
 		tmp1 = screentable[i+pixy];
 		tmp1 += pixx;
 		tmp2 = tmp1 + 1;
		egawriteplane(8);
		/* memcpy(tmp1,cursor.plane[3].image[i],BYTES_PER_CELL); */
		WRITE_ABSOLUTE(tmp1,cursor.plane[3].image[i][0]);
		if (!singlebyte)
		WRITE_ABSOLUTE(tmp2,cursor.plane[3].image[i][1]);

		egawriteplane(4);
		/* memcpy(tmp1,cursor.plane[2].image[i],BYTES_PER_CELL); */
		WRITE_ABSOLUTE(tmp1,cursor.plane[2].image[i][0]);
		if (!singlebyte)
		WRITE_ABSOLUTE(tmp2,cursor.plane[2].image[i][1]);

		egawriteplane(2);
		/* memcpy(tmp1,cursor.plane[1].image[i],BYTES_PER_CELL); */
		WRITE_ABSOLUTE(tmp1,cursor.plane[1].image[i][0]);
		if (!singlebyte)
		WRITE_ABSOLUTE(tmp2,cursor.plane[1].image[i][1]);

		egawriteplane(1);
		/* memcpy(tmp1,cursor.plane[0].image[i],BYTES_PER_CELL); */
		WRITE_ABSOLUTE(tmp1,cursor.plane[0].image[i][0]);
		if (!singlebyte)
		WRITE_ABSOLUTE(tmp2,cursor.plane[0].image[i][1]);
	    }
	    egawriteplane(15);
#ifdef POSITIONBAR
	    if (inmap) positionbar();
#endif
}

void
alleg_HideCursor()
{

	int i,pixx,pixy,x,y;
	char __far *tmp1;
	char __far *tmp2;
	boolean isrogue = Is_rogue_level(&u.uz);
	boolean singlebyte = (isrogue || iflags.over_view
			      || iflags.traditional_view || !inmap);
	int curtyp;
	
	if (inmap && !cursor_type) return;	/* CURSOR_INVIS - nothing to do */
	/* protection from callers */
	x = min(curcol,(CO - 1)); 
	y = min(currow,(LI-1));
	if (!singlebyte && ((x < clipx) || (x > clipxmax))) return;

	    pixy = row2y(y);		/* convert to pixels */
	    if (singlebyte)
		    pixx = x;
	    else
		    pixx = col2x((x-clipx));

	    if (inmap) curtyp = cursor_type;
	    else curtyp = CURSOR_UNDERLINE;

	    if (curtyp == CURSOR_UNDERLINE)  /* optimization for uline */
		i = ROWS_PER_CELL - 1;
	    else
		i = 0;

	    for(;i < ROWS_PER_CELL; ++i) {
		tmp1 = screentable[i+pixy];
 		tmp1 += pixx;
 		tmp2 = tmp1 + 1;
		egawriteplane(8);
		/* memcpy(tmp,undercursor.plane[3].image[i],BYTES_PER_CELL); */
		WRITE_ABSOLUTE(tmp1,undercursor.plane[3].image[i][0]);
		if (!singlebyte)
		WRITE_ABSOLUTE(tmp2,undercursor.plane[3].image[i][1]);

		egawriteplane(4);
		/* memcpy(tmp,undercursor.plane[2].image[i],BYTES_PER_CELL); */
		WRITE_ABSOLUTE(tmp1,undercursor.plane[2].image[i][0]);
		if (!singlebyte)
		WRITE_ABSOLUTE(tmp2,undercursor.plane[2].image[i][1]);

		egawriteplane(2);
		/* memcpy(tmp,undercursor.plane[1].image[i],BYTES_PER_CELL); */
		WRITE_ABSOLUTE(tmp1,undercursor.plane[1].image[i][0]);
		if (!singlebyte)
		WRITE_ABSOLUTE(tmp2,undercursor.plane[1].image[i][1]);

		egawriteplane(1);
		/* memcpy(tmp,undercursor.plane[0].image[i],BYTES_PER_CELL); */
		WRITE_ABSOLUTE(tmp1,undercursor.plane[0].image[i][0]);
		if (!singlebyte)
		WRITE_ABSOLUTE(tmp2,undercursor.plane[0].image[i][1]);
	    }
	    egawriteplane(15);
}
#  endif /* SIMULATE_CURSOR */
# endif /* OVLB */
#endif /* SCREEN_VGA  */

/* vidvga.c */

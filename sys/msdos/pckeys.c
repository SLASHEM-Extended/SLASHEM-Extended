/*	SCCS Id: @(#)pckeys.c	 3.4	 1996/05/11		  */
/* Copyright (c) NetHack PC Development Team 1996                 */
/* NetHack may be freely redistributed.  See license for details. */

/*
 *  MSDOS specific key handling.
 *  WAC - Used to be tile-specific, but also used for access to function
 *      keys for help
 */

#include "hack.h"

#ifdef MSDOS
#include "wintty.h" 
#include "pcvideo.h"

# ifdef PC_MOUSE
#include <dos.h>
# endif

/* WAC is now a char */
char FDECL(pckeys, (unsigned char, unsigned char));

extern struct WinDesc *wins[MAXWIN];	/* from wintty.c */
extern boolean inmap;			/* from video.c */

#define SHIFT		(0x1 | 0x2)
#define CTRL		0x4
#define ALT		0x8

# ifdef PC_MOUSE
STATIC_DCL boolean NDECL(mouse_detect);
STATIC_DCL void NDECL(mouse_show);
STATIC_DCL void NDECL(mouse_hide);
STATIC_DCL unsigned char FDECL(mouse_button, (unsigned char));
STATIC_DCL void FDECL(mouse_abs_pos, (int *,int *));
STATIC_DCL int FDECL(mouse_rel_pos, (int *,int *));
# endif

/*
 * Check for special interface manipulation keys.
 * Returns equivalent keypress if the scan code triggered something.
 * WAC changed from boolean to char (used to returnt TRUE or FALSE)
 */
char
pckeys(scancode, shift)
unsigned char scancode;
unsigned char shift;
{
    boolean opening_dialog;

    opening_dialog = pl_character[0] ? FALSE : TRUE;
    
    switch(scancode) {
        case 0x3B:      /* F1 = Help */
                return ('?');
                break;
# ifdef USE_TILES
#  ifdef SIMULATE_CURSOR
        case 0x3D:      /* F3 = toggle cursor type */
#   ifdef ALLEG_FX
                if (iflags.usealleg) cycleHUD();  /* Cycle HUD mode on Allegro */
#   else
		HideCursor();
		cursor_type += 1;
		if (cursor_type >= NUM_CURSOR_TYPES) cursor_type = 0;
		DrawCursor();
		break;
#   endif /* ALLEG_FX */
#  endif /* SIMULATE_CURSOR */
#ifdef ALLEG_FX
        case 0x76:      /* Control-page_down = scroll vertically downwards*/
                if ((shift & CTRL) && iflags.tile_view && !opening_dialog) {
#  ifdef SCREEN_VGA
			if (iflags.usevga) {
				vga_userpan(3);
			} else
#  endif
#  ifdef ALLEG_FX
		        if (iflags.usealleg) {
		                alleg_userpan(3);
			} else
#  endif
			;
                }
                break;
        case 0x84:      /* Control-page_up = scroll vertically upwards*/
                if ((shift & CTRL) && iflags.tile_view && !opening_dialog)
#  ifdef SCREEN_VGA
			if (iflags.usevga) {
	                        vga_userpan(2);
			} else
#  endif
#  ifdef ALLEG_FX
		        if (iflags.usealleg) {
		                alleg_userpan(2);
			} else
#  endif
			;
                break;
#endif
	case 0x74:	/* Control-right_arrow = scroll horizontal to right */
		if ((shift & CTRL) && iflags.tile_view && !opening_dialog)
#  ifdef SCREEN_VGA
			if (iflags.usevga) {
	                        vga_userpan(1);
			} else
#  endif
#  ifdef ALLEG_FX
		        if (iflags.usealleg) {
		                alleg_userpan(1);
			} else
#  endif
			;
		break;

	case 0x73:	/* Control-left_arrow = scroll horizontal to left */
		if ((shift & CTRL) && iflags.tile_view && !opening_dialog)
#  ifdef SCREEN_VGA
			if (iflags.usevga) {
	                        vga_userpan(0);
			} else
#  endif
#  ifdef ALLEG_FX
		        if (iflags.usealleg) {
		                alleg_userpan(0);
			} else
#  endif
			;
		break;
	case 0x3E:	/* F4 = toggle overview mode */
		if (iflags.tile_view && 
		    !opening_dialog) {
#ifdef REINCARNATION
			if(Is_rogue_level(&u.uz)) break;
#endif
		    	
#  ifdef SCREEN_VGA
			if (iflags.usevga) {
				iflags.traditional_view = FALSE;
				vga_overview(iflags.over_view ? FALSE : TRUE);
				vga_refresh();
			} else
#  endif
#  ifdef ALLEG_FX
		        if (iflags.usealleg) {
				alleg_overview(iflags.over_view ? FALSE : TRUE);
				alleg_refresh();
			} else
#  endif
			;
		}
		break;
	case 0x3F:	/* F5 = toggle traditional mode */
		if (iflags.tile_view &&
		    !opening_dialog
#ifdef REINCARNATION
				&& !Is_rogue_level(&u.uz)
#endif
							) {
			iflags.over_view = FALSE;

#  ifdef SCREEN_VGA
			if (iflags.usevga) {
				vga_traditional(iflags.traditional_view ? FALSE : TRUE);
				vga_refresh();
			} else
#  endif
#  ifdef ALLEG_FX
		        if (iflags.usealleg) {
				alleg_traditional(iflags.traditional_view ? FALSE : TRUE);
				/* NEED TO ADD REDRAW CODE */
			} else
#  endif
			;
		}
		break;
	case 0x40:	/* F6 = screenshot */
#  ifdef ALLEG_FX
		        if (iflags.usealleg) {
				alleg_screenshot();
				alleg_refresh();
		        }
		        break;
#  endif
# endif /* USE_TILES */
	default:
                return NULL;
    }
    return (0xFF);
}

# ifdef PC_MOUSE

#define MOUSE_LB    0x01    /* mouse left button */
#define MOUSE_RB    0x02    /* mouse right button */
#define MOUSE_CB    0x04    /* mouse center button */

void
mouse_init(void)
{
/*        mouse_show();*/
}

void
mouse_finish(void)
{
/*        mouse_hide();*/
}

# endif /* PC_MOUSE */
#endif /* MSDOS */

/*pckeys.c*/

/*	SCCS Id: @(#)wintty.c	3.4	2002/09/27	*/
/* Copyright (c) David Cohrs, 1991				  */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Neither a standard out nor character-based control codes should be
 * part of the "tty look" windowing implementation.
 * h+ 930227
 */

#define AVTC_GLYPH_START 0
#define AVTC_GLYPH_END 1

#include "hack.h"
#include "dlb.h"
#include "patchlevel.h"

#ifdef TTY_GRAPHICS

#ifdef MAC
# define MICRO /* The Mac is a MICRO only for this file, not in general! */
# ifdef THINK_C
extern void msmsg(const char *,...);
# endif
#endif


#ifndef NO_TERMS
#include "tcap.h"
#endif

#include "wintty.h"

#ifdef CLIPPING		/* might want SIGWINCH */
# if defined(BSD) || defined(ULTRIX) || defined(AIX_31) || defined(_BULL_SOURCE)
#include <signal.h>
# endif
#endif

#ifdef BORG
#include "sys/ioctl.h"
#endif

extern char mapped_menu_cmds[]; /* from options.c */

#ifdef BORG
int tty_kbhit(void);
#endif

/* Interface definition, for windows.c */
struct window_procs tty_procs = {
    "tty",
#ifdef MSDOS
    WC_TILED_MAP|WC_ASCII_MAP|
#endif
#if defined(WIN32CON)
    WC_MOUSE_SUPPORT|
#endif
    WC_COLOR|WC_HILITE_PET|WC_INVERSE|WC_EIGHT_BIT_IN,
    0L,
    tty_init_nhwindows,
    tty_player_selection,
    tty_askname,
    tty_get_nh_event,
    tty_exit_nhwindows,
    tty_suspend_nhwindows,
    tty_resume_nhwindows,
    tty_create_nhwindow,
    tty_clear_nhwindow,
    tty_display_nhwindow,
    tty_destroy_nhwindow,
    tty_curs,
    tty_putstr,
    tty_display_file,
    tty_start_menu,
    tty_add_menu,
    tty_end_menu,
    tty_select_menu,
    tty_message_menu,
    tty_update_inventory,
    tty_mark_synch,
    tty_wait_synch,
#ifdef CLIPPING
    tty_cliparound,
#endif
#ifdef POSITIONBAR
    tty_update_positionbar,
#endif
    tty_print_glyph,
    tty_raw_print,
    tty_raw_print_bold,
    tty_nhgetch,
    tty_nh_poskey,
    tty_nhbell,
    tty_doprev_message,
    tty_yn_function,
    tty_getlin,
    tty_get_ext_cmd,
    tty_number_pad,
    tty_delay_output,
#ifdef CHANGE_COLOR	/* the Mac uses a palette device */
    tty_change_color,
#ifdef MAC
    tty_change_background,
    set_tty_font_name,
#endif
    tty_get_color_string,
#endif

    /* other defs that really should go away (they're tty specific) */
    tty_start_screen,
    tty_end_screen,
    genl_outrip,
#if defined(WIN32CON)
    nttty_preference_update,
#else
    genl_preference_update,
#endif
/*    tty_kbhit,*/
};

static int maxwin = 0;			/* number of windows in use */
winid BASE_WINDOW;
struct WinDesc *wins[MAXWIN];
struct DisplayDesc *ttyDisplay;	/* the tty display descriptor */

extern void cmov(int,int); /* from termcap.c */
extern void nocmov(int,int); /* from termcap.c */
#if defined(UNIX) || defined(VMS)
static char obuf[BUFSIZ];	/* BUFSIZ is defined in stdio.h */
#endif

static char winpanicstr[] = "Bad window id %d";
char defmorestr[] = "--More--";

#ifdef MENU_COLOR
extern struct menucoloring *menu_colorings;
#endif

#ifdef CLIPPING
# if defined(USE_TILES) && defined(MSDOS)
boolean clipping = FALSE;	/* clipping on? */
int clipx = 0, clipxmax = 0;
# else
static boolean clipping = FALSE;	/* clipping on? */
static int clipx = 0, clipxmax = 0;
# endif
static int clipy = 0, clipymax = 0;
#endif /* CLIPPING */

#if defined(USE_TILES) && defined(MSDOS)
extern void adjust_cursor_flags(struct WinDesc *);
#endif

#if defined(ASCIIGRAPH) && !defined(NO_TERMS)
boolean GFlag = FALSE;
boolean HE_resets_AS;	/* see termcap.c */
#endif

#if defined(MICRO) || defined(WIN32CON)
static const char to_continue[] = "to continue";
#define getret() getreturn(to_continue)
#else
STATIC_DCL void getret(void);
#endif
STATIC_DCL void erase_menu_or_text(winid, struct WinDesc *, BOOLEAN_P);
STATIC_DCL void free_window_info(struct WinDesc *, BOOLEAN_P);
STATIC_DCL void dmore(struct WinDesc *, const char *);
STATIC_DCL void set_item_state(winid, int, tty_menu_item *);
STATIC_DCL void set_all_on_page(winid,tty_menu_item *,tty_menu_item *);
STATIC_DCL void unset_all_on_page(winid,tty_menu_item *,tty_menu_item *);
STATIC_DCL void invert_all_on_page(winid,tty_menu_item *,tty_menu_item *, CHAR_P);
STATIC_DCL void invert_all(winid,tty_menu_item *,tty_menu_item *, CHAR_P);
STATIC_DCL void process_menu_window(winid,struct WinDesc *);
STATIC_DCL void process_text_window(winid,struct WinDesc *);
STATIC_DCL tty_menu_item *reverse(tty_menu_item *);
const char * compress_str(const char *);
STATIC_DCL void tty_putsym(winid, int, int, CHAR_P);
static char *copy_of(const char *);
STATIC_DCL void bail(const char *);	/* __attribute__((noreturn) */
STATIC_DCL int tty_role_select(char *, char *);
STATIC_DCL int tty_race_select(char *, char *);

/*
 * A string containing all the default commands -- to add to a list
 * of acceptable inputs.
 */
static const char default_menu_cmds[] = {
	MENU_FIRST_PAGE,
	MENU_LAST_PAGE,
	MENU_NEXT_PAGE,
	MENU_PREVIOUS_PAGE,
	MENU_SELECT_ALL,
	MENU_UNSELECT_ALL,
	MENU_INVERT_ALL,
	MENU_SELECT_PAGE,
	MENU_UNSELECT_PAGE,
	MENU_INVERT_PAGE,
	0	/* null terminator */
};


/* clean up and quit */
STATIC_OVL void
bail(mesg)
const char *mesg;
{
    clearlocks();
    tty_exit_nhwindows(mesg);
    terminate(EXIT_SUCCESS);
    /*NOTREACHED*/
}

#if defined(SIGWINCH) && defined(CLIPPING)
STATIC_OVL void
winch()
{
    int oldLI = LI, oldCO = CO, i;
    register struct WinDesc *cw;

    getwindowsz();
    if((oldLI != LI || oldCO != CO) && ttyDisplay) {
	ttyDisplay->rows = LI;
	ttyDisplay->cols = CO;

	cw = wins[BASE_WINDOW];
	cw->rows = ttyDisplay->rows;
	cw->cols = ttyDisplay->cols;

	if(iflags.window_inited) {
	    cw = wins[WIN_MESSAGE];
	    cw->curx = cw->cury = 0;

	    tty_destroy_nhwindow(WIN_STATUS);
	    WIN_STATUS = tty_create_nhwindow(NHW_STATUS);

	    if(u.ux) {
#ifdef CLIPPING
		if(CO < COLNO || LI < ROWNO+3) {
		    setclipped();
		    tty_cliparound(u.ux, u.uy);
		} else {
		    clipping = FALSE;
		    clipx = clipy = 0;
		}
#endif
		i = ttyDisplay->toplin;
		ttyDisplay->toplin = 0;
		docrt();
		bot();
		ttyDisplay->toplin = i;
		flush_screen(1);
		if(i) {
		    addtopl(toplines);
		} else
		    for(i=WIN_INVEN; i < MAXWIN; i++)
			if(wins[i] && wins[i]->active) {
			    /* cop-out */
			    addtopl("Press Return to continue: ");
			    break;
			}
		(void) fflush(stdout);
		if(i < 2) flush_screen(1);
	    }
	}
    }
}
#endif

/*ARGSUSED*/
void
tty_init_nhwindows(argcp,argv)
int* argcp;
char** argv;
{
#if defined(MAC_MPW)
# pragma unused(argcp,argv)
#endif
    int wid, hgt;

    /*
     *  Remember tty modes, to be restored on exit.
     *
     *  gettty() must be called before tty_startup()
     *    due to ordering of LI/CO settings
     *  tty_startup() must be called before initoptions()
     *    due to ordering of graphics settings
     */
#if defined(UNIX) || defined(VMS)
    setbuf(stdout,obuf);
#endif
    gettty();

    /* to port dependant tty setup */
    tty_startup(&wid, &hgt);
    setftty();			/* calls start_screen */

    /* set up tty descriptor */
    ttyDisplay = (struct DisplayDesc*) alloc(sizeof(struct DisplayDesc));
    ttyDisplay->toplin = 0;
    ttyDisplay->rows = hgt;
    ttyDisplay->cols = wid;
    ttyDisplay->curx = ttyDisplay->cury = 0;
    ttyDisplay->inmore = ttyDisplay->inread = ttyDisplay->intr = 0;
    ttyDisplay->dismiss_more = 0;
#ifdef TEXTCOLOR
    ttyDisplay->color = NO_COLOR;
#endif
    ttyDisplay->attrs = 0;

    /* set up the default windows */
    BASE_WINDOW = tty_create_nhwindow(NHW_BASE);
    wins[BASE_WINDOW]->active = 1;

    ttyDisplay->lastwin = WIN_ERR;

#if defined(SIGWINCH) && defined(CLIPPING)
    (void) signal(SIGWINCH, winch);
#endif

    /* add one a space forward menu command alias */
    add_menu_cmd_alias(' ', MENU_NEXT_PAGE);

    tty_clear_nhwindow(BASE_WINDOW);

    tty_putstr(BASE_WINDOW, 0, "");
    tty_putstr(BASE_WINDOW, 0, COPYRIGHT_BANNER_A);
    tty_putstr(BASE_WINDOW, 0, COPYRIGHT_BANNER_B);
    tty_putstr(BASE_WINDOW, 0, COPYRIGHT_BANNER_C);
    tty_putstr(BASE_WINDOW, 0, COPYRIGHT_BANNER_D);
    tty_putstr(BASE_WINDOW, 0, "");
    tty_display_nhwindow(BASE_WINDOW, FALSE);
}

void
tty_player_selection()
{
	int i, k, n;
	char pick4u = 'n';
	char pbuf[QBUFSZ], plbuf[QBUFSZ];
	winid win;
	anything any;
	menu_item *selected = 0;

	/* prevent an unnecessary prompt */
	rigid_role_checks();

	/* Should we randomly pick for the player? */
	if (!flags.randomall &&
	    (flags.initrole == ROLE_NONE || flags.initrace == ROLE_NONE ||
	     flags.initgend == ROLE_NONE || flags.initalign == ROLE_NONE)) {
	    int echoline;
	    char *prompt = build_plselection_prompt(pbuf, QBUFSZ, flags.initrole,
				flags.initrace, flags.initgend, flags.initalign);

	    tty_putstr(BASE_WINDOW, 0, "");
	    echoline = wins[BASE_WINDOW]->cury;
	    tty_putstr(BASE_WINDOW, 0, prompt);
	    do {
		pick4u = lowc(readchar());
		if (index(quitchars, pick4u)) pick4u = 'y';
	    } while(!index(ynqchars, pick4u));
	    if ((int)strlen(prompt) + 1 < CO) {
		/* Echo choice and move back down line */
		tty_putsym(BASE_WINDOW, (int)strlen(prompt)+1, echoline, pick4u);
		tty_putstr(BASE_WINDOW, 0, "");
	    } else
		/* Otherwise it's hard to tell where to echo, and things are
		 * wrapping a bit messily anyway, so (try to) make sure the next
		 * question shows up well and doesn't get wrapped at the
		 * bottom of the window.
		 */
		tty_clear_nhwindow(BASE_WINDOW);
	    
	    if (pick4u != 'y' && pick4u != 'n') {
give_up:	/* Quit */
		if (selected) free((void *) selected);
		bail((char *)0);
		/*NOTREACHED*/
		return;
	    }
	}

	(void)  root_plselection_prompt(plbuf, QBUFSZ - 1,
			flags.initrole, flags.initrace, flags.initgend, flags.initalign);

	/* Select a role, if necessary */
	/* we'll try to be compatible with pre-selected race/gender/alignment,
	 * but may not succeed */
	if (flags.initrole < 0) {
	    /* Process the choice */
	    if (pick4u == 'y' || flags.initrole == ROLE_RANDOM || flags.randomall) {
			/* Pick a random role */
			flags.initrole = pick_role(flags.initrace, flags.initgend,
						flags.initalign, PICK_RANDOM);
			if (flags.initrole < 0) {
				tty_putstr(BASE_WINDOW, 0, "Incompatible role!");
				flags.initrole = randrole();
			}
	    } else {
	    	if (tty_role_select(pbuf, plbuf) < 0) goto give_up;
	    }
	    (void)  root_plselection_prompt(plbuf, QBUFSZ - 1,
			flags.initrole, flags.initrace, flags.initgend, flags.initalign);
	}

	/* Select a race, if necessary */
	/* force compatibility with role, try for compatibility with
	 * pre-selected gender/alignment */
	if (flags.initrace < 0 || !validrace(flags.initrole, flags.initrace)) {
	    /* pre-selected race not valid */
	    if (pick4u == 'y' || flags.initrace == ROLE_RANDOM || flags.randomall) {
			flags.initrace = pick_race(flags.initrole, flags.initgend,
								flags.initalign, PICK_RANDOM);
			if (flags.initrace < 0) {
				tty_putstr(BASE_WINDOW, 0, "Incompatible race!");
				flags.initrace = randrace(flags.initrole);
			}
	    } else {	/* pick4u == 'n' */
	    	if (tty_race_select(pbuf, plbuf) < 0) goto give_up;

		if (yn("Add hybrid races to your character? (these can change how the game plays; most of them will make the game harder and are generally meant for experienced players)") == 'y') {

			int tryct = 0;

			if (yn("Use the ones you set in your config file? (example: OPTIONS=hybridangbander)") == 'y') {

				flags.hybridcancel = 1; /* don't give more than the player wanted */
				flags.randomhybrids = 0;
				/* every other option will first set all hybridization flags to zero --Amy */

			} else if (yn("Randomized hybridization? (like selecting a random race, this has only a 10% chance of actually adding any hybridization)") == 'y') {

				flags.hybridization = flags.hybridangbander = flags.hybridaquarian = flags.hybridcurser = flags.hybridhaxor = flags.hybridhomicider = flags.hybridsuxxor = flags.hybridwarper = flags.hybridrandomizer = flags.hybridnullrace = flags.hybridmazewalker = flags.hybridsoviet = flags.hybridxrace = flags.hybridheretic = flags.hybridsokosolver = flags.hybridspecialist = flags.hybridamerican = flags.hybridminimalist = flags.hybridnastinator = flags.hybridrougelike = flags.hybridsegfaulter = flags.hybridironman = flags.hybridamnesiac = flags.hybridproblematic = flags.hybridwindinhabitant = flags.hybridaggravator = flags.hybridevilvariant = flags.hybridlevelscaler = flags.hybriderosator = flags.hybridroommate = flags.hybridextravator = flags.hybridhallucinator = flags.hybridbossrusher = flags.hybriddorian = flags.hybridtechless = flags.hybridblait = flags.hybridgrouper = flags.hybridscriptor = flags.hybridunbalancor = flags.hybridbeacher = flags.hybridstairseeker = flags.hybridmatrayser = flags.hybridbadstatter = flags.hybridhardmoder = flags.hybridkillfiller = flags.hybriddroughter = flags.hybridvanillaoid = flags.hybridfeminizer = flags.hybridchallenger = flags.hybridstunfish = 0;

			    if (!rn2(10)) {

				switch (rnd(49)) {

					case 1:
						if (!Race_if(PM_ANGBANDER)) {flags.hybridangbander = 1; flags.hybridization++;
						}
						break;
					case 2:
						if (!Race_if(PM_AQUATIC_MONSTER)) {flags.hybridaquarian = 1; flags.hybridization++;
						}
						break;
					case 3:
						if (!Race_if(PM_CURSER)) {flags.hybridcurser = 1; flags.hybridization++;
						}
						break;
					case 4:
						if (!Race_if(PM_HAXOR)) {flags.hybridhaxor = 1; flags.hybridization++;
						}
						break;
					case 5:
						if (!Race_if(PM_HOMICIDER)) {flags.hybridhomicider = 1; flags.hybridization++;
						}
						break;
					case 6:
						if (!Race_if(PM_SUXXOR)) {flags.hybridsuxxor = 1; flags.hybridization++;
						}
						break;
					case 7:
						if (!Race_if(PM_WARPER)) {flags.hybridwarper = 1; flags.hybridization++;
						}
						break;
					case 8:
						if (!Race_if(PM_RANDOMIZER)) {flags.hybridrandomizer = 1; flags.hybridization++;
						}
						break;
					case 9:
						if (!Race_if(PM_NULL)) {flags.hybridnullrace = 1; flags.hybridization++;
						}
						break;
					case 10:
						if (!Race_if(PM_MAZEWALKER)) {flags.hybridmazewalker = 1; flags.hybridization++;
						}
						break;
					case 11:
						if (!Race_if(PM_SOVIET)) {flags.hybridsoviet = 1; flags.hybridization++;
						}
						break;
					case 12:
						if (!Race_if(PM_RACE_X)) {flags.hybridxrace = 1; flags.hybridization++;
						}
						break;
					case 13:
						if (!Race_if(PM_HERETIC)) {flags.hybridheretic = 1; flags.hybridization++;
						}
						break;
					case 14:
						if (!Race_if(PM_SOKOSOLVER)) {flags.hybridsokosolver = 1; flags.hybridization++;
						}
						break;
					case 15:
						if (!Race_if(PM_SPECIALIST)) {flags.hybridspecialist = 1; flags.hybridization++;
						}
						break;
					case 16:
						if (!Race_if(PM_AMERICAN)) {flags.hybridamerican = 1; flags.hybridization++;
						}
						break;
					case 17:
						if (!Race_if(PM_MINIMALIST)) {flags.hybridminimalist = 1; flags.hybridization++;
						}
						break;
					case 18:
						if (!Race_if(PM_NASTINATOR)) {flags.hybridnastinator = 1; flags.hybridization++;
						}
						break;
					case 19:
						if (!Race_if(PM_ROUGELIKE)) {flags.hybridrougelike = 1; flags.hybridization++;
						}
						break;
					case 20:
						if (!Race_if(PM_SEGFAULTER)) {flags.hybridsegfaulter = 1; flags.hybridization++;
						}
						break;
					case 21:
						if (!Race_if(PM_IRONMAN)) {flags.hybridironman = 1; flags.hybridization++;
						}
						break;
					case 22:
						if (!Race_if(PM_AMNESIAC)) {flags.hybridamnesiac = 1; flags.hybridization++;
						}
						break;
					case 23:
						if (!Race_if(PM_PROBLEMATIC)) {flags.hybridproblematic = 1; flags.hybridization++;
						}
						break;
					case 24:
						if (!Race_if(PM_WIND_INHABITANT)) {flags.hybridwindinhabitant = 1; flags.hybridization++;
						}
						break;
					case 25:
						if (!Race_if(PM_AGGRAVATOR)) {flags.hybridaggravator = 1; flags.hybridization++;
						}
						break;
					case 26:
						if (!Race_if(PM_EVILVARIANT)) {flags.hybridevilvariant = 1; flags.hybridization++;
						}
						break;
					case 27:
						if (!Race_if(PM_LEVELSCALER)) {flags.hybridlevelscaler = 1; flags.hybridization++;
						}
						break;
					case 28:
						if (!Race_if(PM_EROSATOR)) {flags.hybriderosator = 1; flags.hybridization++;
						}
						break;
					case 29:
						if (!Race_if(PM_ROOMMATE)) {flags.hybridroommate = 1; flags.hybridization++;
						}
						break;
					case 30:
						if (!Race_if(PM_EXTRAVATOR)) {flags.hybridextravator = 1; flags.hybridization++;
						}
						break;
					case 31:
						if (!(flags.hybridhallucinator)) {flags.hybridhallucinator = 1; flags.hybridization++; 
						}
						break;
					case 32:
						if (!(flags.hybridbossrusher)) {flags.hybridbossrusher = 1; flags.hybridization++; 
						}
						break;
					case 33:
						if (!(flags.hybriddorian)) {flags.hybriddorian = 1; flags.hybridization++;
						}
						break;
					case 34:
						if (!(flags.hybridtechless)) {flags.hybridtechless = 1; flags.hybridization++;
						}
						break;
					case 35:
						if (!(flags.hybridblait)) {flags.hybridblait = 1; flags.hybridization++;
						}
						break;
					case 36:
						if (!(flags.hybridgrouper)) {flags.hybridgrouper = 1; flags.hybridization++;
						}
						break;
					case 37:
						if (!(flags.hybridscriptor)) {flags.hybridscriptor = 1; flags.hybridization++;
						}
						break;
					case 38:
						if (!(flags.hybridunbalancor)) {flags.hybridunbalancor = 1; flags.hybridization++;
						}
						break;
					case 39:
						if (!(flags.hybridbeacher)) {flags.hybridbeacher = 1; flags.hybridization++;
						}
						break;
					case 40:
						if (!(flags.hybridstairseeker)) {flags.hybridstairseeker = 1; flags.hybridization++;
						}
						break;
					case 41:
						if (!(flags.hybridmatrayser)) {flags.hybridmatrayser = 1; flags.hybridization++;
						}
						break;
					case 42:
						if (!(flags.hybridfeminizer)) {flags.hybridfeminizer = 1; flags.hybridization++;
						}
						break;
					case 43:
						if (!(flags.hybridchallenger)) {flags.hybridchallenger = 1; flags.hybridization++;
						}
						break;
					case 44:
						if (!(flags.hybridhardmoder)) {flags.hybridhardmoder = 1; flags.hybridization++;
						}
						break;
					case 45:
						if (!(flags.hybridstunfish)) {flags.hybridstunfish = 1; flags.hybridization++;
						}
						break;
					case 46:
						if (!(flags.hybridkillfiller)) {flags.hybridkillfiller = 1; flags.hybridization++;
						}
						break;
					case 47:
						if (!(flags.hybridmatrayser)) {flags.hybridmatrayser = 1; flags.hybridization++;
						}
						break;
					case 48:
						if (!(flags.hybriddroughter)) {flags.hybriddroughter = 1; flags.hybridization++;
						}
						break;
					case 49:
						if (!(flags.hybridvanillaoid)) {flags.hybridvanillaoid = 1; flags.hybridization++;
						}
						break;
				}

				while (((rnd(7)) < 3) && (tryct < 100)) {

					tryct++;

					switch (rnd(49)) {
	
						case 1:
							if (!(flags.hybridangbander)) {
								flags.hybridangbander = 1; flags.hybridization++; break;
							}
							break;
						case 2:
							if (!(flags.hybridaquarian)) {
								flags.hybridaquarian = 1; flags.hybridization++; break;
							}
							break;
						case 3:
							if (!(flags.hybridcurser)) {
								flags.hybridcurser = 1; flags.hybridization++; break;
							}
							break;
						case 4:
							if (!(flags.hybridhaxor)) {
								flags.hybridhaxor = 1; flags.hybridization++; break;
							}
							break;
						case 5:
							if (!(flags.hybridhomicider)) {
								flags.hybridhomicider = 1; flags.hybridization++; break;
							}
							break;
						case 6:
							if (!(flags.hybridsuxxor)) {
								flags.hybridsuxxor = 1; flags.hybridization++; break;
							}
							break;
						case 7:
							if (!(flags.hybridwarper)) {
								flags.hybridwarper = 1; flags.hybridization++; break;
							}
							break;
						case 8:
							if (!(flags.hybridrandomizer)) {
								flags.hybridrandomizer = 1; flags.hybridization++; break;
							}
							break;
						case 9:
							if (!(flags.hybridnullrace)) {
								flags.hybridnullrace = 1; flags.hybridization++; break;
							}
							break;
						case 10:
							if (!(flags.hybridmazewalker)) {
								flags.hybridmazewalker = 1; flags.hybridization++; break;
							}	
							break;
						case 11:
							if (!(flags.hybridsoviet)) {
								flags.hybridsoviet = 1; flags.hybridization++; break;
							}	
							break;
						case 12:
							if (!(flags.hybridxrace)) {
								flags.hybridxrace = 1; flags.hybridization++; break;
							}	
							break;
						case 13:
							if (!(flags.hybridheretic)) {
								flags.hybridheretic = 1; flags.hybridization++; break;
							}	
							break;
						case 14:
							if (!(flags.hybridsokosolver)) {
								flags.hybridsokosolver = 1; flags.hybridization++; break;
							}	
							break;
						case 15:
							if (!(flags.hybridspecialist)) {
								flags.hybridspecialist = 1; flags.hybridization++; break;
							}	
							break;
						case 16:
							if (!(flags.hybridamerican)) {
								flags.hybridamerican = 1; flags.hybridization++; break;
							}	
							break;
						case 17:
							if (!(flags.hybridminimalist)) {
								flags.hybridminimalist = 1; flags.hybridization++; break;
							}	
							break;
						case 18:
							if (!(flags.hybridnastinator)) {
								flags.hybridnastinator = 1; flags.hybridization++; break;
							}	
							break;
						case 19:
							if (!(flags.hybridrougelike)) {
								flags.hybridrougelike = 1; flags.hybridization++; break;
							}	
							break;
						case 20:
							if (!(flags.hybridsegfaulter)) {
								flags.hybridsegfaulter = 1; flags.hybridization++; break;
							}
							break;
						case 21:
							if (!(flags.hybridironman)) {
								flags.hybridironman = 1; flags.hybridization++; break;
							}
							break;
						case 22:
							if (!(flags.hybridamnesiac)) {
								flags.hybridamnesiac = 1; flags.hybridization++; break;
							}
							break;
						case 23:
							if (!(flags.hybridproblematic)) {
								flags.hybridproblematic = 1; flags.hybridization++; break;
							}
							break;
						case 24:
							if (!(flags.hybridwindinhabitant)) {
								flags.hybridwindinhabitant = 1; flags.hybridization++; break;
							}
							break;
						case 25:
							if (!(flags.hybridaggravator)) {
								flags.hybridaggravator = 1; flags.hybridization++; break;
							}
							break;
						case 26:
							if (!(flags.hybridevilvariant)) {
								flags.hybridevilvariant = 1; flags.hybridization++; break;
							}
							break;
						case 27:
							if (!(flags.hybridlevelscaler)) {
								flags.hybridlevelscaler = 1; flags.hybridization++; break;
							}
							break;
						case 28:
							if (!(flags.hybriderosator)) {
								flags.hybriderosator = 1; flags.hybridization++; break;
							}
							break;
						case 29:
							if (!(flags.hybridroommate)) {
								flags.hybridroommate = 1; flags.hybridization++; break;
							}
							break;
						case 30:
							if (!(flags.hybridextravator)) {
								flags.hybridextravator = 1; flags.hybridization++; break;
							}
							break;
						case 31:
							if (!(flags.hybridhallucinator)) {
								flags.hybridhallucinator = 1; flags.hybridization++; break;
							}
							break;
						case 32:
							if (!(flags.hybridbossrusher)) {
							flags.hybridbossrusher = 1; flags.hybridization++; break;
							}
							break;
						case 33:
							if (!(flags.hybriddorian)) {
							flags.hybriddorian = 1; flags.hybridization++; break;
							}
							break;
						case 34:
							if (!(flags.hybridtechless)) {
							flags.hybridtechless = 1; flags.hybridization++; break;
							}
							break;
						case 35:
							if (!(flags.hybridblait)) {
							flags.hybridblait = 1; flags.hybridization++; break;
							}
							break;
						case 36:
							if (!(flags.hybridgrouper)) {
							flags.hybridgrouper = 1; flags.hybridization++; break;
							}
							break;
						case 37:
							if (!(flags.hybridscriptor)) {
							flags.hybridscriptor = 1; flags.hybridization++; break;
							}
							break;
						case 38:
							if (!(flags.hybridunbalancor)) {
							flags.hybridunbalancor = 1; flags.hybridization++; break;
							}
							break;
						case 39:
							if (!(flags.hybridbeacher)) {
							flags.hybridbeacher = 1; flags.hybridization++; break;
							}
							break;
						case 40:
							if (!(flags.hybridstairseeker)) {
							flags.hybridstairseeker = 1; flags.hybridization++; break;
							}
							break;
						case 41:
							if (!(flags.hybridmatrayser)) {
							flags.hybridmatrayser = 1; flags.hybridization++; break;
							}
							break;
						case 42:
							if (!(flags.hybridfeminizer)) {
							flags.hybridfeminizer = 1; flags.hybridization++; break;
							}
							break;
						case 43:
							if (!(flags.hybridchallenger)) {
							flags.hybridchallenger = 1; flags.hybridization++; break;
							}
							break;
						case 44:
							if (!(flags.hybridhardmoder)) {
							flags.hybridhardmoder = 1; flags.hybridization++; break;
							}
							break;
						case 45:
							if (!(flags.hybridstunfish)) {
							flags.hybridstunfish = 1; flags.hybridization++; break;
							}
							break;
						case 46:
							if (!(flags.hybridkillfiller)) {
							flags.hybridkillfiller = 1; flags.hybridization++; break;
							}
							break;
						case 47:
							if (!(flags.hybridbadstatter)) {
							flags.hybridbadstatter = 1; flags.hybridization++; break;
							}
							break;
						case 48:
							if (!(flags.hybriddroughter)) {
							flags.hybriddroughter = 1; flags.hybridization++; break;
							}
							break;
						case 49:
							if (!(flags.hybridvanillaoid)) {
							flags.hybridvanillaoid = 1; flags.hybridization++; break;
							}
							break;
					}
				}

			    }

			flags.hybridcancel = 1; /* don't give more than the player wanted */
			flags.randomhybrids = 0;

			} else if (yn("Randomized hybridization (but always add at least one hybrid race)?") == 'y') {

				flags.hybridization = flags.hybridangbander = flags.hybridaquarian = flags.hybridcurser = flags.hybridhaxor = flags.hybridhomicider = flags.hybridsuxxor = flags.hybridwarper = flags.hybridrandomizer = flags.hybridnullrace = flags.hybridmazewalker = flags.hybridsoviet = flags.hybridxrace = flags.hybridheretic = flags.hybridsokosolver = flags.hybridspecialist = flags.hybridamerican = flags.hybridminimalist = flags.hybridnastinator = flags.hybridrougelike = flags.hybridsegfaulter = flags.hybridironman = flags.hybridamnesiac = flags.hybridproblematic = flags.hybridwindinhabitant = flags.hybridaggravator = flags.hybridevilvariant = flags.hybridlevelscaler = flags.hybriderosator = flags.hybridroommate = flags.hybridextravator = flags.hybridhallucinator = flags.hybridbossrusher = flags.hybriddorian = flags.hybridtechless = flags.hybridblait = flags.hybridgrouper = flags.hybridscriptor = flags.hybridunbalancor = flags.hybridbeacher = flags.hybridstairseeker = flags.hybridmatrayser = flags.hybridbadstatter = flags.hybridfeminizer = flags.hybridchallenger = flags.hybridhardmoder = flags.hybridkillfiller = flags.hybriddroughter = flags.hybridvanillaoid = flags.hybridstunfish = 0;

				switch (rnd(49)) {

					case 1:
						if (!Race_if(PM_ANGBANDER)) {
						flags.hybridangbander = 1; flags.hybridization++;
						}
						break;
					case 2:
						if (!Race_if(PM_AQUATIC_MONSTER)) {
						flags.hybridaquarian = 1; flags.hybridization++;
						}
						break;
					case 3:
						if (!Race_if(PM_CURSER)) {
						flags.hybridcurser = 1; flags.hybridization++;
						}
						break;
					case 4:
						if (!Race_if(PM_HAXOR)) {
						flags.hybridhaxor = 1; flags.hybridization++;
						}
						break;
					case 5:
						if (!Race_if(PM_HOMICIDER)) {
						flags.hybridhomicider = 1; flags.hybridization++;
						}
						break;
					case 6:
						if (!Race_if(PM_SUXXOR)) {
						flags.hybridsuxxor = 1; flags.hybridization++;
						}
						break;
					case 7:
						if (!Race_if(PM_WARPER)) {
						flags.hybridwarper = 1; flags.hybridization++;
						}
						break;
					case 8:
						if (!Race_if(PM_RANDOMIZER)) {
						flags.hybridrandomizer = 1; flags.hybridization++;
						}
						break;
					case 9:
						if (!Race_if(PM_NULL)) {
						flags.hybridnullrace = 1; flags.hybridization++;
						}
						break;
					case 10:
						if (!Race_if(PM_MAZEWALKER)) {
						flags.hybridmazewalker = 1; flags.hybridization++;
						}
						break;
					case 11:
						if (!Race_if(PM_SOVIET)) {
						flags.hybridsoviet = 1; flags.hybridization++;
						}
						break;
					case 12:
						if (!Race_if(PM_RACE_X)) {
						flags.hybridxrace = 1; flags.hybridization++;
						}
						break;
					case 13:
						if (!Race_if(PM_HERETIC)) {
						flags.hybridheretic = 1; flags.hybridization++;
						}
						break;
					case 14:
						if (!Race_if(PM_SOKOSOLVER)) {
						flags.hybridsokosolver = 1; flags.hybridization++;
						}
						break;
					case 15:
						if (!Race_if(PM_SPECIALIST)) {
						flags.hybridspecialist = 1; flags.hybridization++;
						}
						break;
					case 16:
						if (!Race_if(PM_AMERICAN)) {
						flags.hybridamerican = 1; flags.hybridization++;
						}
						break;
					case 17:
						if (!Race_if(PM_MINIMALIST)) {
						flags.hybridminimalist = 1; flags.hybridization++;
						}
						break;
					case 18:
						if (!Race_if(PM_NASTINATOR)) {
						flags.hybridnastinator = 1; flags.hybridization++;
						}
						break;
					case 19:
						if (!Race_if(PM_ROUGELIKE)) {
						flags.hybridrougelike = 1; flags.hybridization++;
						}
						break;
					case 20:
						if (!Race_if(PM_SEGFAULTER)) {
						flags.hybridsegfaulter = 1; flags.hybridization++;
						}
						break;
					case 21:
						if (!Race_if(PM_IRONMAN)) {
						flags.hybridironman = 1; flags.hybridization++;
						}
						break;
					case 22:
						if (!Race_if(PM_AMNESIAC)) {
						flags.hybridamnesiac = 1; flags.hybridization++;
						}
						break;
					case 23:
						if (!Race_if(PM_PROBLEMATIC)) {flags.hybridproblematic = 1; flags.hybridization++;
						}
						break;
					case 24:
						if (!Race_if(PM_WIND_INHABITANT)) {flags.hybridwindinhabitant = 1; flags.hybridization++;
						}
						break;
					case 25:
						if (!Race_if(PM_AGGRAVATOR)) {flags.hybridaggravator = 1; flags.hybridization++;
						}
						break;
					case 26:
						if (!Race_if(PM_EVILVARIANT)) {flags.hybridevilvariant = 1; flags.hybridization++;
						}
						break;
					case 27:
						if (!Race_if(PM_LEVELSCALER)) {flags.hybridlevelscaler = 1; flags.hybridization++;
						}
						break;
					case 28:
						if (!Race_if(PM_EROSATOR)) {flags.hybriderosator = 1; flags.hybridization++;
						}
						break;
					case 29:
						if (!Race_if(PM_ROOMMATE)) {flags.hybridroommate = 1; flags.hybridization++;
						}
						break;
					case 30:
						if (!Race_if(PM_EXTRAVATOR)) {flags.hybridextravator = 1; flags.hybridization++;
						}
						break;
					case 31:
						if (!Race_if(PM_HALLUCINATOR)) {flags.hybridhallucinator = 1; flags.hybridization++; 
						}
						break;
					case 32:
						if (!Race_if(PM_BOSSRUSHER)) {flags.hybridbossrusher = 1; flags.hybridization++; 
						}
						break;
					case 33:
						if (!Race_if(PM_DORIAN)) {flags.hybriddorian = 1; flags.hybridization++;
						}
						break;
					case 34:
						if (!Race_if(PM_TECHLESS)) {flags.hybridtechless = 1; flags.hybridization++;
						}
						break;
					case 35:
						if (!Race_if(PM_BLAIT)) {flags.hybridblait = 1; flags.hybridization++;
						}
						break;
					case 36:
						if (!Race_if(PM_GROUPER)) {flags.hybridgrouper = 1; flags.hybridization++;
						}
						break;
					case 37:
						if (!Race_if(PM_SCRIPTOR)) {flags.hybridscriptor = 1; flags.hybridization++;
						}
						break;
					case 38:
						if (!Race_if(PM_UNBALANCOR)) {flags.hybridunbalancor = 1; flags.hybridization++;
						}
						break;
					case 39:
						if (!Race_if(PM_BEACHER)) {flags.hybridbeacher = 1; flags.hybridization++;
						}
						break;
					case 40:
						if (!Race_if(PM_STAIRSEEKER)) {flags.hybridstairseeker = 1; flags.hybridization++;
						}
						break;
					case 41:
						if (!Race_if(PM_MATRAYSER)) {flags.hybridmatrayser = 1; flags.hybridization++;
						}
						break;
					case 42:
						if (!Race_if(PM_FEMINIZER)) {flags.hybridfeminizer = 1; flags.hybridization++;
						}
						break;
					case 43:
						if (!Race_if(PM_CHALLENGER)) {flags.hybridchallenger = 1; flags.hybridization++;
						}
						break;
					case 44:
						if (!Race_if(PM_HARDMODER)) {flags.hybridhardmoder = 1; flags.hybridization++;
						}
						break;
					case 45:
						if (!Race_if(PM_STUNNED_LIKE_A_FISH)) {flags.hybridstunfish = 1; flags.hybridization++;
						}
						break;
					case 46:
						if (!Race_if(PM_KILLFILLER)) {flags.hybridkillfiller = 1; flags.hybridization++;
						}
						break;
					case 47:
						if (!Race_if(PM_BADSTATTER)) {flags.hybridbadstatter = 1; flags.hybridization++;
						}
						break;
					case 48:
						if (!Race_if(PM_DROUGHTER)) {flags.hybriddroughter = 1; flags.hybridization++;
						}
						break;
					case 49:
						if (!Race_if(PM_VANILLAOID)) {flags.hybridvanillaoid = 1; flags.hybridization++;
						}
						break;

				}

				while (((rnd(7)) < 3) && (tryct < 100)) {

					tryct++;

					switch (rnd(49)) {
	
						case 1:
							if (!(flags.hybridangbander)) {
								flags.hybridangbander = 1; flags.hybridization++; break;
							}
							break;
						case 2:
							if (!(flags.hybridaquarian)) {
								flags.hybridaquarian = 1; flags.hybridization++; break;
							}
							break;
						case 3:
							if (!(flags.hybridcurser)) {
								flags.hybridcurser = 1; flags.hybridization++; break;
							}
							break;
						case 4:
							if (!(flags.hybridhaxor)) {
								flags.hybridhaxor = 1; flags.hybridization++; break;
							}
							break;
						case 5:
							if (!(flags.hybridhomicider)) {
								flags.hybridhomicider = 1; flags.hybridization++; break;
							}
							break;
						case 6:
							if (!(flags.hybridsuxxor)) {
								flags.hybridsuxxor = 1; flags.hybridization++; break;
							}
							break;
						case 7:
							if (!(flags.hybridwarper)) {
								flags.hybridwarper = 1; flags.hybridization++; break;
							}
							break;
						case 8:
							if (!(flags.hybridrandomizer)) {
								flags.hybridrandomizer = 1; flags.hybridization++; break;
							}
							break;
						case 9:
							if (!(flags.hybridnullrace)) {
								flags.hybridnullrace = 1; flags.hybridization++; break;
							}
							break;
						case 10:
							if (!(flags.hybridmazewalker)) {
								flags.hybridmazewalker = 1; flags.hybridization++; break;
							}	
							break;
						case 11:
							if (!(flags.hybridsoviet)) {
								flags.hybridsoviet = 1; flags.hybridization++; break;
							}	
							break;
						case 12:
							if (!(flags.hybridxrace)) {
								flags.hybridxrace = 1; flags.hybridization++; break;
							}	
							break;
						case 13:
							if (!(flags.hybridheretic)) {
								flags.hybridheretic = 1; flags.hybridization++; break;
							}	
							break;
						case 14:
							if (!(flags.hybridsokosolver)) {
								flags.hybridsokosolver = 1; flags.hybridization++; break;
							}	
							break;
						case 15:
							if (!(flags.hybridspecialist)) {
								flags.hybridspecialist = 1; flags.hybridization++; break;
							}	
							break;
						case 16:
							if (!(flags.hybridamerican)) {
								flags.hybridamerican = 1; flags.hybridization++; break;
							}	
							break;
						case 17:
							if (!(flags.hybridminimalist)) {
								flags.hybridminimalist = 1; flags.hybridization++; break;
							}	
							break;
						case 18:
							if (!(flags.hybridnastinator)) {
								flags.hybridnastinator = 1; flags.hybridization++; break;
							}	
							break;
						case 19:
							if (!(flags.hybridrougelike)) {
								flags.hybridrougelike = 1; flags.hybridization++; break;
							}	
							break;
						case 20:
							if (!(flags.hybridsegfaulter)) {
								flags.hybridsegfaulter = 1; flags.hybridization++; break;
							}
							break;
						case 21:
							if (!(flags.hybridironman)) {
								flags.hybridironman = 1; flags.hybridization++; break;
							}
							break;
						case 22:
							if (!(flags.hybridamnesiac)) {
								flags.hybridamnesiac = 1; flags.hybridization++; break;
							}
							break;
						case 23:
							if (!(flags.hybridproblematic)) {
								flags.hybridproblematic = 1; flags.hybridization++; break;
							}
							break;
						case 24:
							if (!(flags.hybridwindinhabitant)) {
								flags.hybridwindinhabitant = 1; flags.hybridization++; break;
							}
							break;
						case 25:
							if (!(flags.hybridaggravator)) {
								flags.hybridaggravator = 1; flags.hybridization++; break;
							}
							break;
						case 26:
							if (!(flags.hybridevilvariant)) {
								flags.hybridevilvariant = 1; flags.hybridization++; break;
							}
							break;
						case 27:
							if (!(flags.hybridlevelscaler)) {
								flags.hybridlevelscaler = 1; flags.hybridization++; break;
							}
							break;
						case 28:
							if (!(flags.hybriderosator)) {
								flags.hybriderosator = 1; flags.hybridization++; break;
							}
							break;
						case 29:
							if (!(flags.hybridroommate)) {
								flags.hybridroommate = 1; flags.hybridization++; break;
							}
							break;
						case 30:
							if (!(flags.hybridextravator)) {
								flags.hybridextravator = 1; flags.hybridization++; break;
							}
							break;
						case 31:
							if (!(flags.hybridhallucinator)) {
								flags.hybridhallucinator = 1; flags.hybridization++; break;
							}
							break;
						case 32:
							if (!(flags.hybridbossrusher)) {
								flags.hybridbossrusher = 1; flags.hybridization++; break;
							}
							break;
						case 33:
							if (!(flags.hybriddorian)) {
								flags.hybriddorian = 1; flags.hybridization++; break;
							}
							break;
						case 34:
							if (!(flags.hybridtechless)) {
								flags.hybridtechless = 1; flags.hybridization++; break;
							}
							break;
						case 35:
							if (!(flags.hybridblait)) {
								flags.hybridblait = 1; flags.hybridization++; break;
							}
							break;
						case 36:
							if (!(flags.hybridgrouper)) {
								flags.hybridgrouper = 1; flags.hybridization++; break;
							}
							break;
						case 37:
							if (!(flags.hybridscriptor)) {
								flags.hybridscriptor = 1; flags.hybridization++; break;
							}
							break;
						case 38:
							if (!(flags.hybridunbalancor)) {
								flags.hybridunbalancor = 1; flags.hybridization++; break;
							}
							break;
						case 39:
							if (!(flags.hybridbeacher)) {
								flags.hybridbeacher = 1; flags.hybridization++; break;
							}
							break;
						case 40:
							if (!(flags.hybridstairseeker)) {
								flags.hybridstairseeker = 1; flags.hybridization++; break;
							}
							break;
						case 41:
							if (!(flags.hybridmatrayser)) {
								flags.hybridmatrayser = 1; flags.hybridization++; break;
							}
							break;
						case 42:
							if (!(flags.hybridfeminizer)) {
								flags.hybridfeminizer = 1; flags.hybridization++; break;
							}
							break;
						case 43:
							if (!(flags.hybridchallenger)) {
								flags.hybridchallenger = 1; flags.hybridization++; break;
							}
							break;
						case 44:
							if (!(flags.hybridhardmoder)) {
								flags.hybridhardmoder = 1; flags.hybridization++; break;
							}
							break;
						case 45:
							if (!(flags.hybridstunfish)) {
								flags.hybridstunfish = 1; flags.hybridization++;
							}
							break;
						case 46:
							if (!(flags.hybridkillfiller)) {
								flags.hybridkillfiller = 1; flags.hybridization++; break;
							}
							break;
						case 47:
							if (!(flags.hybridbadstatter)) {
								flags.hybridbadstatter = 1; flags.hybridization++; break;
							}
							break;
						case 48:
							if (!(flags.hybriddroughter)) {
								flags.hybriddroughter = 1; flags.hybridization++; break;
							}
							break;
						case 49:
							if (!(flags.hybridvanillaoid)) {
								flags.hybridvanillaoid = 1; flags.hybridization++; break;
							}
							break;

					}
				}

			flags.hybridcancel = 1; /* don't give more than the player wanted */
			flags.randomhybrids = 0;

			} else {

				flags.hybridization = flags.hybridangbander = flags.hybridaquarian = flags.hybridcurser = flags.hybridhaxor = flags.hybridhomicider = flags.hybridsuxxor = flags.hybridwarper = flags.hybridrandomizer = flags.hybridnullrace = flags.hybridmazewalker = flags.hybridsoviet = flags.hybridxrace = flags.hybridheretic = flags.hybridsokosolver = flags.hybridspecialist = flags.hybridamerican = flags.hybridminimalist = flags.hybridnastinator = flags.hybridrougelike = flags.hybridsegfaulter = flags.hybridironman = flags.hybridamnesiac = flags.hybridproblematic = flags.hybridwindinhabitant = flags.hybridaggravator = flags.hybridevilvariant = flags.hybridlevelscaler = flags.hybriderosator = flags.hybridroommate = flags.hybridextravator = flags.hybridhallucinator = flags.hybridbossrusher = flags.hybriddorian = flags.hybridtechless = flags.hybridblait = flags.hybridgrouper = flags.hybridscriptor = flags.hybridunbalancor = flags.hybridbeacher = flags.hybridstairseeker = flags.hybridmatrayser = flags.hybridbadstatter = flags.hybridfeminizer = flags.hybridchallenger = flags.hybridhardmoder = flags.hybridkillfiller = flags.hybriddroughter = flags.hybridvanillaoid = flags.hybridstunfish = 0;

				if (!Race_if(PM_AGGRAVATOR)) {if (yn("Add the aggravator hybrid race to your character?") == 'y')
					{flags.hybridaggravator = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_AMERICAN)) {if (yn("Add the american hybrid race to your character?") == 'y')
					{flags.hybridamerican = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_AMNESIAC)) {if (yn("Add the amnesiac hybrid race to your character?") == 'y')
					{flags.hybridamnesiac = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_ANGBANDER)) {if (yn("Add the angbander hybrid race to your character?") == 'y')
					{flags.hybridangbander = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_AQUATIC_MONSTER)) {if (yn("Add the aquarian hybrid race to your character?") == 'y')
					{flags.hybridaquarian = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_BADSTATTER)) {if (yn("Add the badstatter hybrid race to your character?") == 'y')
					{flags.hybridbadstatter = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_BEACHER)) {if (yn("Add the beacher hybrid race to your character?") == 'y')
					{flags.hybridbeacher = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_BLAIT)) {if (yn("Add the blait hybrid race to your character?") == 'y')
					{flags.hybridblait = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_BOSSRUSHER)) {if (yn("Add the bossrusher hybrid race to your character?") == 'y')
					{flags.hybridbossrusher = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_CHALLENGER)) {if (yn("Add the challenger hybrid race to your character?") == 'y')
					{flags.hybridchallenger = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_CURSER)) {if (yn("Add the curser hybrid race to your character?") == 'y')
					{flags.hybridcurser = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_DROUGHTER)) {if (yn("Add the droughter hybrid race to your character?") == 'y')
					{flags.hybriddroughter = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_DORIAN)) {if (yn("Add the dorian hybrid race to your character?") == 'y')
					{flags.hybriddorian = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_EROSATOR)) {if (yn("Add the erosator hybrid race to your character?") == 'y')
					{flags.hybriderosator = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_EVILVARIANT)) {if (yn("Add the evilvariant hybrid race to your character?") == 'y')
					{flags.hybridevilvariant = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_EXTRAVATOR)) {if (yn("Add the extravator hybrid race to your character?") == 'y')
					{flags.hybridextravator = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_FEMINIZER)) {if (yn("Add the feminizer hybrid race to your character?") == 'y')
					{flags.hybridfeminizer = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_GROUPER)) {if (yn("Add the grouper hybrid race to your character?") == 'y')
					{flags.hybridgrouper = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_HALLUCINATOR)) {if (yn("Add the hallucinator hybrid race to your character?") == 'y')
					{flags.hybridhallucinator = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_HARDMODER)) {if (yn("Add the hardmoder hybrid race to your character?") == 'y')
					{flags.hybridhardmoder = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_HAXOR)) {if (yn("Add the haxor hybrid race to your character?") == 'y')
					{flags.hybridhaxor = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_HERETIC)) {if (yn("Add the heretic hybrid race to your character?") == 'y')
					{flags.hybridheretic = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_HOMICIDER)) {if (yn("Add the homicider hybrid race to your character?") == 'y')
					{flags.hybridhomicider = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_IRONMAN)) {if (yn("Add the ironman hybrid race to your character?") == 'y')
					{flags.hybridironman = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_KILLFILLER)) {if (yn("Add the killfiller hybrid race to your character?") == 'y')
					{flags.hybridkillfiller = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_LEVELSCALER)) {if (yn("Add the levelscaler hybrid race to your character?") == 'y')
					{flags.hybridlevelscaler = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_MATRAYSER)) {if (yn("Add the matrayser hybrid race to your character?") == 'y')
					{flags.hybridmatrayser = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_MAZEWALKER)) {if (yn("Add the mazewalker hybrid race to your character?") == 'y')
					{flags.hybridmazewalker = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_MINIMALIST)) {if (yn("Add the minimalist hybrid race to your character?") == 'y')
					{flags.hybridminimalist = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_NASTINATOR)) {if (yn("Add the nastinator hybrid race to your character?") == 'y')
					{flags.hybridnastinator = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_NULL)) {if (yn("Add the null hybrid race to your character?") == 'y')
					{flags.hybridnullrace = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_PROBLEMATIC)) {if (yn("Add the problematic hybrid race to your character?") == 'y')
					{flags.hybridproblematic = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_RANDOMIZER)) {if (yn("Add the randomizer hybrid race to your character?") == 'y')
					{flags.hybridrandomizer = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_ROOMMATE)) {if (yn("Add the roommate hybrid race to your character?") == 'y')
					{flags.hybridroommate = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_ROUGELIKE)) {if (yn("Add the rougelike hybrid race to your character?") == 'y')
					{flags.hybridrougelike = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_SCRIPTOR)) {if (yn("Add the scriptor hybrid race to your character?") == 'y')
					{flags.hybridscriptor = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_SOKOSOLVER)) {if (yn("Add the sokosolver hybrid race to your character?") == 'y')
					{flags.hybridsokosolver = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_SOVIET)) {if (yn("Add the soviet hybrid race to your character?") == 'y')
					{flags.hybridsoviet = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_SPECIALIST)) {if (yn("Add the specialist hybrid race to your character?") == 'y')
					{flags.hybridspecialist = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_STAIRSEEKER)) {if (yn("Add the stairseeker hybrid race to your character?") == 'y')
					{flags.hybridstairseeker = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_STUNNED_LIKE_A_FISH)) {if (yn("Add the stunned-like-a-fish hybrid race to your character?") == 'y')
					{flags.hybridstunfish = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_SUXXOR)) {if (yn("Add the suxxor hybrid race to your character?") == 'y')
					{flags.hybridsuxxor = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_TECHLESS)) {if (yn("Add the techless hybrid race to your character?") == 'y')
					{flags.hybridtechless = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_UNBALANCOR)) {if (yn("Add the unbalancor hybrid race to your character?") == 'y')
					{flags.hybridunbalancor = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_VANILLAOID)) {if (yn("Add the vanillaoid hybrid race to your character?") == 'y')
					{flags.hybridvanillaoid = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_WARPER)) {if (yn("Add the warper hybrid race to your character?") == 'y')
					{flags.hybridwarper = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_WIND_INHABITANT)) {if (yn("Add the wind inhabitant hybrid race to your character?") == 'y')
					{flags.hybridwindinhabitant = 1; flags.hybridization++;}
				}
				if (!Race_if(PM_RACE_X)) {if (yn("Add the x-race hybrid race to your character?") == 'y')
					{flags.hybridxrace = 1; flags.hybridization++;}
				}

				if (!Race_if(PM_SEGFAULTER)) {if (yn("Add the segfaulter hybrid race to your character (WARNING: NOT RECOMMENDED!)?") == 'y')
					{flags.hybridsegfaulter = 1; flags.hybridization++;}
				}

			flags.hybridcancel = 1; /* don't give more than the player wanted */
			flags.randomhybrids = 0;

			}

		}

		else {

			flags.hybridization = flags.hybridangbander = flags.hybridaquarian = flags.hybridcurser = flags.hybridhaxor = flags.hybridhomicider = flags.hybridsuxxor = flags.hybridwarper = flags.hybridrandomizer = flags.hybridnullrace = flags.hybridmazewalker = flags.hybridsoviet = flags.hybridxrace = flags.hybridheretic = flags.hybridsokosolver = flags.hybridspecialist = flags.hybridamerican = flags.hybridminimalist = flags.hybridnastinator = flags.hybridrougelike = flags.hybridsegfaulter = flags.hybridironman = flags.hybridamnesiac = flags.hybridproblematic = flags.hybridwindinhabitant = flags.hybridaggravator = flags.hybridevilvariant = flags.hybridlevelscaler = flags.hybriderosator = flags.hybridroommate = flags.hybridextravator = flags.hybridhallucinator = flags.hybridbossrusher = flags.hybriddorian = flags.hybridtechless = flags.hybridblait = flags.hybridgrouper = flags.hybridscriptor = flags.hybridunbalancor = flags.hybridbeacher = flags.hybridstairseeker = flags.hybridmatrayser = flags.hybridbadstatter = flags.hybridfeminizer = flags.hybridchallenger = flags.hybridhardmoder = flags.hybridkillfiller = flags.hybriddroughter = flags.hybridvanillaoid = flags.hybridstunfish = 0;

			flags.hybridcancel = 1; /* player answered no to the "do you want hybridization" prompt */
			flags.randomhybrids = 0;
		}
	    }

	    (void)  root_plselection_prompt(plbuf, QBUFSZ - 1,
			flags.initrole, flags.initrace, flags.initgend, flags.initalign);
	}

	/* Select a gender, if necessary */
	/* force compatibility with role/race, try for compatibility with
	 * pre-selected alignment */
	if (flags.initgend < 0 || !validgend(flags.initrole, flags.initrace,
						flags.initgend)) {
			/* pre-selected gender not valid */
		if (pick4u == 'y' || flags.initgend == ROLE_RANDOM || flags.randomall) {
			flags.initgend = pick_gend(flags.initrole, flags.initrace,
						flags.initalign, PICK_RANDOM);
			if (flags.initgend < 0) {
				tty_putstr(BASE_WINDOW, 0, "Incompatible gender!");
				flags.initgend = randgend(flags.initrole, flags.initrace);
			}
		} else {	/* pick4u == 'n' */
			/* Count the number of valid genders */
			n = 0;	/* number valid */
			k = 0;	/* valid gender */
			for (i = 0; i < ROLE_GENDERS; i++) {
				if (ok_gend(flags.initrole, flags.initrace, i,
								flags.initalign)) {
					n++;
					k = i;
				}
			}
			if (n == 0) {
				for (i = 0; i < ROLE_GENDERS; i++) {
					if (validgend(flags.initrole, flags.initrace, i)) {
						n++;
						k = i;
					}
				}
			}

			/* Permit the user to pick, if there is more than one */
			if (n > 1) {
				tty_clear_nhwindow(BASE_WINDOW);
				tty_putstr(BASE_WINDOW, 0, "Choosing Gender");
				win = create_nhwindow(NHW_MENU);
				start_menu(win);
				any.a_void = 0;         /* zero out all bits */
				for (i = 0; i < ROLE_GENDERS; i++)
					if (ok_gend(flags.initrole, flags.initrace, i,
										flags.initalign)) {
						any.a_int = i+1;
						add_menu(win, NO_GLYPH, &any, genders[i].adj[0],
						0, ATR_NONE, genders[i].adj, MENU_UNSELECTED);
					}
				any.a_int = pick_gend(flags.initrole, flags.initrace,
							flags.initalign, PICK_RANDOM)+1;
				if (any.a_int == 0)	/* must be non-zero */
					any.a_int = randgend(flags.initrole, flags.initrace)+1;
				add_menu(win, NO_GLYPH, &any , '*', 0, ATR_NONE,
						"Random", MENU_UNSELECTED);
				any.a_int = i+1;	/* must be non-zero */
				add_menu(win, NO_GLYPH, &any , '!', 0, ATR_NONE,
						"Quit", MENU_UNSELECTED);
				sprintf(pbuf, "Pick the gender of your %s", plbuf);
				end_menu(win, pbuf);
				n = select_menu(win, PICK_ONE, &selected);
				destroy_nhwindow(win);
				if (n != 1 || selected[0].item.a_int == any.a_int)
					goto give_up;		/* Selected quit */

				k = selected[0].item.a_int - 1;
				free((void *) selected),	selected = 0;
			}
			flags.initgend = k;
		}
	    (void)  root_plselection_prompt(plbuf, QBUFSZ - 1,
			flags.initrole, flags.initrace, flags.initgend, flags.initalign);
	}

	/* Select an alignment, if necessary */
	/* force compatibility with role/race/gender */
	if (flags.initalign < 0 || !validalign(flags.initrole, flags.initrace,
							flags.initalign)) {
	    /* pre-selected alignment not valid */
	    if (pick4u == 'y' || flags.initalign == ROLE_RANDOM || flags.randomall) {
			flags.initalign = pick_align(flags.initrole, flags.initrace,
								flags.initgend, PICK_RANDOM);
			if (flags.initalign < 0) {
				tty_putstr(BASE_WINDOW, 0, "Incompatible alignment!");
				flags.initalign = randalign(flags.initrole, flags.initrace);
			}
	    } else {	/* pick4u == 'n' */
			/* Count the number of valid alignments */
			n = 0;	/* number valid */
			k = 0;	/* valid alignment */
			for (i = 0; i < ROLE_ALIGNS; i++) {
				if (ok_align(flags.initrole, flags.initrace, flags.initgend,
								i)) {
					n++;
					k = i;
				}
			}
			if (n == 0) {
				for (i = 0; i < ROLE_ALIGNS; i++) {
					if (validalign(flags.initrole, flags.initrace, i)) {
						n++;
						k = i;
					}
				}
			}

			/* Permit the user to pick, if there is more than one */
			if (n > 1) {
				tty_clear_nhwindow(BASE_WINDOW);
				tty_putstr(BASE_WINDOW, 0, "Choosing Alignment");
				win = create_nhwindow(NHW_MENU);
				start_menu(win);
				any.a_void = 0;         /* zero out all bits */
				for (i = 0; i < ROLE_ALIGNS; i++)
					if (ok_align(flags.initrole, flags.initrace,
									flags.initgend, i)) {
						any.a_int = i+1;
						add_menu(win, NO_GLYPH, &any, aligns[i].adj[0],
						 0, ATR_NONE, aligns[i].adj, MENU_UNSELECTED);
					}
				any.a_int = pick_align(flags.initrole, flags.initrace,
							flags.initgend, PICK_RANDOM)+1;
				if (any.a_int == 0)	/* must be non-zero */
					any.a_int = randalign(flags.initrole, flags.initrace)+1;
				add_menu(win, NO_GLYPH, &any , '*', 0, ATR_NONE,
						"Random", MENU_UNSELECTED);
				any.a_int = i+1;	/* must be non-zero */
				add_menu(win, NO_GLYPH, &any , '!', 0, ATR_NONE,
						"Quit", MENU_UNSELECTED);
				sprintf(pbuf, "Pick the alignment of your %s", plbuf);
				end_menu(win, pbuf);
				n = select_menu(win, PICK_ONE, &selected);
				destroy_nhwindow(win);
				if (n != 1 || selected[0].item.a_int == any.a_int)
					goto give_up;		/* Selected quit */

				k = selected[0].item.a_int - 1;
				free((void *) selected),	selected = 0;
			}
			flags.initalign = k;
	    }
	}
	/* Success! */
	tty_display_nhwindow(BASE_WINDOW, FALSE);
}

#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

STATIC_OVL int
tty_role_select(pbuf, plbuf)
char * pbuf;
char * plbuf;
{
	int i, n;
    char rolenamebuf[QBUFSZ];
	winid win;
	anything any;
	menu_item *selected = 0;

   	tty_clear_nhwindow(BASE_WINDOW);
	tty_putstr(BASE_WINDOW, 0, "Choosing Character's Role");

	/* Prompt for a role */
	win = create_nhwindow(NHW_MENU);
	start_menu(win);
	any.a_void = 0;         /* zero out all bits */

	/* handhold players who are acting spectacularly stupid --Amy */
	add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
		 "Scroll with the space bar or the > key, NOT the arrow keys!", MENU_UNSELECTED);
	add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, " ", MENU_UNSELECTED);

	for (i = 0; roles[i].name.m; i++) {
	    if (ok_role(i, flags.initrace, flags.initgend,
						flags.initalign)) {
		any.a_int = i+1;	/* must be non-zero */
		if (flags.initgend != ROLE_NONE && flags.initgend != ROLE_RANDOM) {
			if (flags.initgend == 1  && roles[i].name.f)
				strcpy(rolenamebuf, roles[i].name.f);
			else
				strcpy(rolenamebuf, roles[i].name.m);
		} else {
			if (roles[i].name.f) {
				strcpy(rolenamebuf, roles[i].name.m);
				strcat(rolenamebuf, "/");
				strcat(rolenamebuf, roles[i].name.f);
			} else 
				strcpy(rolenamebuf, roles[i].name.m);
		}	
		add_menu(win, NO_GLYPH, &any, LETTERS[i % strlen(LETTERS)],
			    0, ATR_NONE, an(rolenamebuf), MENU_UNSELECTED);
	    }
	}
	any.a_int = pick_role(flags.initrace, flags.initgend,
				    flags.initalign, PICK_RANDOM)+1;
	if (any.a_int == 0)	/* must be non-zero */
	    any.a_int = randrole()+1;
	add_menu(win, NO_GLYPH, &any , '*', 0, ATR_NONE,
			"Random", MENU_UNSELECTED);
	any.a_int = i+1;	/* must be non-zero */
	add_menu(win, NO_GLYPH, &any , '!', 0, ATR_NONE,
			"Quit", MENU_UNSELECTED);
	sprintf(pbuf, "Pick a role for your %s", plbuf);
	end_menu(win, pbuf);
	n = select_menu(win, PICK_ONE, &selected);
	destroy_nhwindow(win);

	/* Process the choice */
	if (n != 1 || selected[0].item.a_int == any.a_int) {
	    free((void *) selected),	selected = 0;	
	    return (-1);		/* Selected quit */
	}

	flags.initrole = selected[0].item.a_int - 1;
	free((void *) selected),	selected = 0;
	return (flags.initrole);
}

STATIC_OVL int
tty_race_select(pbuf, plbuf)
char * pbuf;
char * plbuf;
{
	int i, k, n;
	winid win;
	anything any;
	menu_item *selected = 0;

	/* Count the number of valid races */
	n = 0;	/* number valid */
	k = 0;	/* valid race */
	for (i = 0; races[i].noun; i++) {
	    if (ok_race(flags.initrole, i, flags.initgend,
						flags.initalign)) {
		n++;
		k = i;
	    }
	}
	if (n == 0) {
	    for (i = 0; races[i].noun; i++) {
		if (validrace(flags.initrole, i)) {
		    n++;
		    k = i;
		}
	    }
	}

	/* Permit the user to pick, if there is more than one */
	if (n > 1) {
	    tty_clear_nhwindow(BASE_WINDOW);
	    tty_putstr(BASE_WINDOW, 0, "Choosing Race");
	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);
	    any.a_void = 0;         /* zero out all bits */

	    add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
		 "Scroll with the space bar or the > key, NOT the arrow keys!", MENU_UNSELECTED);
	    add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, " ", MENU_UNSELECTED);

	    for (i = 0; races[i].noun; i++)
		if (ok_race(flags.initrole, i, flags.initgend,
						flags.initalign)) {
		    any.a_int = i+1;	/* must be non-zero */
		    add_menu(win, NO_GLYPH, &any, LETTERS[i % strlen(LETTERS)],
			0, ATR_NONE, races[i].noun, MENU_UNSELECTED);
		}
	    any.a_int = pick_race(flags.initrole, flags.initgend,
					flags.initalign, PICK_RANDOM)+1;
	    if (any.a_int == 0)	/* must be non-zero */
		any.a_int = randrace(flags.initrole)+1;
	    add_menu(win, NO_GLYPH, &any , '*', 0, ATR_NONE,
			    "Random", MENU_UNSELECTED);
	    any.a_int = i+1;	/* must be non-zero */
	    add_menu(win, NO_GLYPH, &any , '!', 0, ATR_NONE,
			    "Quit", MENU_UNSELECTED);
	    sprintf(pbuf, "Pick the race of your %s", plbuf);
	    end_menu(win, pbuf);
	    n = select_menu(win, PICK_ONE, &selected);
	    destroy_nhwindow(win);
	    if (n != 1 || selected[0].item.a_int == any.a_int)
		return(-1);		/* Selected quit */

	    k = selected[0].item.a_int - 1;
	    free((void *) selected),	selected = 0;
	}
	
	flags.initrace = k;
	return (k);
	
#if 0 /* This version deals with more than 2 races per letter */
	int i, k, n, choicelet = 0;
	char thisch;
	char choicestr[3];
	winid win;
	anything any;
	menu_item *selected = 0;
	char pbuf[QBUFSZ];

	/* Count the number of valid races */
	n = 0;	/* number valid */
	k = 0;	/* valid race */
	for (i = 0; races[i].noun; i++) {
	    if (ok_race(flags.initrole, i, flags.initgend,
						flags.initalign)) {
		n++;
		k = i;
	    }
	}
	if (n == 0) {
	    for (i = 0; races[i].noun; i++) {
		if (validrace(flags.initrole, i)) {
		    n++;
		    k = i;
		}
	    }
	}

	/* Permit the user to pick, if there is more than one */
	if (n > 1) do {
	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);
	    any.a_void = 0;         /* zero out all bits */
	    for (i = 0; races[i].noun; i++)
		if (ok_race(flags.initrole, i, flags.initgend,
						flags.initalign)
			&& (!choicelet || !strncmpi(races[i].noun, 
					choicestr, choicelet))) {
		    
		    thisch = lowc(races[i].noun[choicelet]);
		    any.a_int = i+1;	/* must be non-zero */
		    add_menu(win, NO_GLYPH, &any, thisch,
			0, ATR_NONE, races[i].noun, MENU_UNSELECTED);
		}
	    any.a_int = pick_race(flags.initrole, flags.initgend,
				flags.initalign)+1;
	    if (any.a_int == 0)	/* must be non-zero */
		any.a_int = randrace(flags.initrole)+1;
	    add_menu(win, NO_GLYPH, &any , '*', 0, ATR_NONE,
			    "Random", MENU_UNSELECTED);
	    any.a_int = i+1;	/* must be non-zero */
	    add_menu(win, NO_GLYPH, &any , 'q', 0, ATR_NONE,
			    "Quit", MENU_UNSELECTED);
	    sprintf(pbuf, "Pick the race of your %s",
			    roles[flags.initrole].name.m);
	    end_menu(win, pbuf);
	    n = select_menu(win, PICK_ONE, &selected);
	    destroy_nhwindow(win);


	    if (n != 1 || selected[0].item.a_int == any.a_int) {
	    	free((void *) selected),	selected = 0;
	    	if (!choicelet) {
		    return (-1);		/* Selected quit */
	    	} else {
	    	    choicelet--;
	    	    n = 2; /* there are at least 2 */
	    	    continue;
	    	}
	    } else {
		k = selected[0].item.a_int - 1;
	    	free((void *) selected),	selected = 0;
		choicestr[choicelet] = races[k].noun[choicelet];
		choicelet++;
	    }
	    
	    /* Check whether there are at least 2 choices left */
	    n = 0;
	    for (i = 0; (races[i].noun && (n <= 1)); i++)
		if (ok_race(flags.initrole, i, flags.initgend,
						flags.initalign)
			&& (!choicelet || !strncmpi(races[i].noun, 
					choicestr, choicelet))) 
		    n++;
	} while (n > 1);
	
	flags.initrace = k;
	return (k);
#endif
}

/*
 * plname is filled either by an option (-u Player  or  -uPlayer) or
 * explicitly (by being the wizard) or by askname.
 * It may still contain a suffix denoting the role, etc.
 * Always called after init_nhwindows() and before display_gamewindows().
 */
void
tty_askname()
{
    static char who_are_you[] = "Who are you? ";
    register int c, ct, tryct = 0;

    tty_putstr(BASE_WINDOW, 0, "");
    do {
	if (++tryct > 1) {
	    if (tryct > 10) bail("Giving up after 10 tries.\n");
	    tty_curs(BASE_WINDOW, 1, wins[BASE_WINDOW]->cury - 1);
	    tty_putstr(BASE_WINDOW, 0, "Enter a name for your character...");
	    /* erase previous prompt (in case of ESC after partial response) */
	    tty_curs(BASE_WINDOW, 1, wins[BASE_WINDOW]->cury),  cl_end();
	}
	tty_putstr(BASE_WINDOW, 0, who_are_you);
	tty_curs(BASE_WINDOW, (int)(sizeof who_are_you),
		 wins[BASE_WINDOW]->cury - 1);
	ct = 0;
	while((c = tty_nhgetch()) != '\n') {
		if(c == EOF) error("End of input\n");
		if (c == DOESCAPE) { ct = 0; break; }  /* continue outer loop */
#if defined(WIN32CON)
		if (c == '\003') bail("^C abort.\n");
#endif
		/* some people get confused when their erase char is not ^H */
		if (c == '\b' || c == '\177') {
			if(ct) {
				ct--;
#ifdef WIN32CON
				ttyDisplay->curx--;
#endif
#if defined(MICRO) || defined(WIN32CON)
# if defined(WIN32CON) || defined(MSDOS)
				backsp();       /* \b is visible on NT */
				(void) putchar(' ');
				backsp();
# else
				msmsg("\b \b");
# endif
#else
				(void) putchar('\b');
				(void) putchar(' ');
				(void) putchar('\b');
#endif
			}
			continue;
		}
#if defined(UNIX) || defined(VMS)
		if(c != '-' && c != '@')
		if(c < 'A' || (c > 'Z' && c < 'a') || c > 'z') c = '_';
#endif
		if (ct < (int)(sizeof plname) - 1) {
#if defined(MICRO)
# if defined(MSDOS)
			if (iflags.grmode) {
				(void) putchar(c);
			} else
# endif
			msmsg("%c", c);
#else
			(void) putchar(c);
#endif
			plname[ct++] = c;
#ifdef WIN32CON
			ttyDisplay->curx++;
#endif
		}
	}
	plname[ct] = 0;
    } while (ct == 0);

    /* move to next line to simulate echo of user's <return> */
    tty_curs(BASE_WINDOW, 1, wins[BASE_WINDOW]->cury + 1);
}

void
tty_get_nh_event()
{
    return;
}

#if !defined(MICRO) && !defined(WIN32CON)
STATIC_OVL void
getret()
{
	if (program_state.done_hup) {
		morc = '\033';
		return;
	}
	xputs("\n");
	if(flags.standout)
		standoutbeg();
	xputs("Hit ");
	xputs(iflags.cbreak ? "space" : "return");
	xputs(" to continue: ");
	if(flags.standout)
		standoutend();
	xwaitforspace(" ");
}
#endif

void
tty_suspend_nhwindows(str)
    const char *str;
{
    settty(str);		/* calls end_screen, perhaps raw_print */
    if (!str) tty_raw_print("");	/* calls fflush(stdout) */
}

void
tty_resume_nhwindows()
{
    gettty();
    setftty();			/* calls start_screen */
    docrt();
}

void
tty_exit_nhwindows(str)
    const char *str;
{
    winid i;

    tty_suspend_nhwindows(str);
    /* Just forget any windows existed, since we're about to exit anyway.
     * Disable windows to avoid calls to window routines.
     */
    for(i=0; i<MAXWIN; i++)
	if (wins[i] && (i != BASE_WINDOW)) {
#ifdef FREE_ALL_MEMORY
	    free_window_info(wins[i], TRUE);
	    free((void *) wins[i]);
#endif
	    wins[i] = 0;
	}
#ifndef NO_TERMS		/*(until this gets added to the window interface)*/
    tty_shutdown();		/* cleanup termcap/terminfo/whatever */
#endif
    iflags.window_inited = 0;
}

winid
tty_create_nhwindow(type)
    int type;
{
    struct WinDesc* newwin;
    int i;
    int newid;

    if(maxwin == MAXWIN)
	return WIN_ERR;

    newwin = (struct WinDesc*) alloc(sizeof(struct WinDesc));
    newwin->type = type;
    newwin->flags = 0;
    newwin->active = FALSE;
    newwin->curx = newwin->cury = 0;
    newwin->morestr = 0;
    newwin->mlist = (tty_menu_item *) 0;
    newwin->plist = (tty_menu_item **) 0;
    newwin->npages = newwin->plist_size = newwin->nitems = newwin->how = 0;
    switch(type) {
    case NHW_BASE:
	/* base window, used for absolute movement on the screen */
	newwin->offx = newwin->offy = 0;
	newwin->rows = ttyDisplay->rows;
	newwin->cols = ttyDisplay->cols;
	newwin->maxrow = newwin->maxcol = 0;
	break;
    case NHW_MESSAGE:
	/* message window, 1 line long, very wide, top of screen */
	newwin->offx = newwin->offy = 0;
	/* sanity check */
	if(iflags.msg_history < 20) iflags.msg_history = 20;
	else if(iflags.msg_history > 60) iflags.msg_history = 60;
	newwin->maxrow = newwin->rows = iflags.msg_history;
	newwin->maxcol = newwin->cols = 0;
	break;
    case NHW_STATUS:
	/* status window, 2 lines long, full width, bottom of screen */
	/* WAC make it a variable lines long */
	newwin->offx = 0;
#if defined(USE_TILES) && defined(MSDOS)
	/* We're in VGA 640x480 mode.  There's room for 3 lines */
	if (iflags.grmode) {
		newwin->offy = ttyDisplay->rows-3;
	} else
#endif
	newwin->offy = min((int)ttyDisplay->rows-2, ROWNO+1);
	/* newwin->rows = newwin->maxrow = 2; */
	newwin->rows = newwin->maxrow =
	    ((ttyDisplay->rows - newwin->offy) > 3) ? 4 : ((ttyDisplay->rows - newwin->offy) > 2) ? 3 : 2;
	newwin->cols = newwin->maxcol = min(ttyDisplay->cols, COLNO);

	break;
    case NHW_MAP:
	/* map window, ROWNO lines long, full width, below message window */
	newwin->offx = 0;
	newwin->offy = 1;
	newwin->rows = ROWNO;
	newwin->cols = COLNO;
	newwin->maxrow = 0;	/* no buffering done -- let gbuf do it */
	newwin->maxcol = 0;
	break;
    case NHW_MENU:
    case NHW_TEXT:
	/* inventory/menu window, variable length, full width, top of screen */
	/* help window, the same, different semantics for display, etc */
	newwin->offx = newwin->offy = 0;
	newwin->rows = 0;
	newwin->cols = ttyDisplay->cols;
	newwin->maxrow = newwin->maxcol = 0;
	break;
   default:
	panic("Tried to create window type %d\n", (int) type);
	return WIN_ERR;
    }

    for(newid = 0; newid<MAXWIN; newid++) {
	if(wins[newid] == 0) {
	    wins[newid] = newwin;
	    break;
	}
    }
    if(newid == MAXWIN) {
	panic("No window slots!");
	return WIN_ERR;
    }

    if(newwin->maxrow) {
	newwin->data =
		(char **) alloc(sizeof(char *) * (unsigned)newwin->maxrow);
	newwin->datlen =
		(short *) alloc(sizeof(short) * (unsigned)newwin->maxrow);
	if(newwin->maxcol) {
	    for (i = 0; i < newwin->maxrow; i++) {
		newwin->data[i] = (char *) alloc((unsigned)newwin->maxcol);
		newwin->datlen[i] = newwin->maxcol;
	    }
	} else {
	    for (i = 0; i < newwin->maxrow; i++) {
		newwin->data[i] = (char *) 0;
		newwin->datlen[i] = 0;
	    }
	}
	if(newwin->type == NHW_MESSAGE)
	    newwin->maxrow = 0;
    } else {
	newwin->data = (char **)0;
	newwin->datlen = (short *)0;
    }

    return newid;
}

STATIC_OVL void
erase_menu_or_text(window, cw, clear)
    winid window;
    struct WinDesc *cw;
    boolean clear;
{
    if(cw->offx == 0)
	if(cw->offy) {
	    tty_curs(window, 1, 0);
	    cl_eos();
	} else if (clear)
	    clear_screen();
	else
	    docrt();
    else
	docorner((int)cw->offx, cw->maxrow+1);
}

STATIC_OVL void
free_window_info(cw, free_data)
    struct WinDesc *cw;
    boolean free_data;
{
    int i;

    if (cw->data) {
	if (cw == wins[WIN_MESSAGE] && cw->rows > cw->maxrow)
	    cw->maxrow = cw->rows;		/* topl data */
	for(i=0; i<cw->maxrow; i++)
	    if(cw->data[i]) {
		free((void *)cw->data[i]);
		cw->data[i] = (char *)0;
		if (cw->datlen) cw->datlen[i] = 0;
	    }
	if (free_data) {
	    free((void *)cw->data);
	    cw->data = (char **)0;
	    if (cw->datlen) free((void *)cw->datlen);
	    cw->datlen = (short *)0;
	    cw->rows = 0;
	}
    }
    cw->maxrow = cw->maxcol = 0;
    if(cw->mlist) {
	tty_menu_item *temp;
	while ((temp = cw->mlist) != 0) {
	    cw->mlist = cw->mlist->next;
	    if (temp->str) free((void *)temp->str);
	    free((void *)temp);
	}
    }
    if (cw->plist) {
	free((void *)cw->plist);
	cw->plist = 0;
    }
    cw->plist_size = cw->npages = cw->nitems = cw->how = 0;
    if(cw->morestr) {
	free((void *)cw->morestr);
	cw->morestr = 0;
    }
}

void
tty_clear_nhwindow(window)
    winid window;
{
    register struct WinDesc *cw = 0;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    if(window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0)
	panic(winpanicstr,  window);
    ttyDisplay->lastwin = window;

    switch(cw->type) {
    case NHW_MESSAGE:
	if(ttyDisplay->toplin) {
	    home();
	    cl_end();
	    if(cw->cury)
		docorner(1, cw->cury+1);
	    ttyDisplay->toplin = 0;
	}
	break;
    case NHW_STATUS:
	tty_curs(window, 1, 0);
	cl_end();
	tty_curs(window, 1, 1);
	cl_end();
	break;
    case NHW_MAP:
	/* cheap -- clear the whole thing and tell nethack to redraw botl */
	flags.botlx = 1;
	/* fall into ... */
    case NHW_BASE:
	clear_screen();
	break;
    case NHW_MENU:
    case NHW_TEXT:
	if(cw->active)
	    erase_menu_or_text(window, cw, TRUE);
	free_window_info(cw, FALSE);
	break;
    }
    cw->curx = cw->cury = 0;
}

STATIC_OVL void
dmore(cw, s)
    register struct WinDesc *cw;
    const char *s;			/* valid responses */
{
    const char *prompt = cw->morestr ? cw->morestr : defmorestr;
    int offset = (cw->type == NHW_TEXT) ? 1 : 2;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

	if (youmonst.data && !(cw->morestr) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover

#if defined(WIN32)
&& !program_state.exiting
#endif

	&& automore_active() && (u.automorefuckthisshit == TRUE) ) {
		return;
	}

    tty_curs(BASE_WINDOW,
	     (int)ttyDisplay->curx + offset, (int)ttyDisplay->cury);
    if(flags.standout)
	standoutbeg();
    xputs(prompt);
    ttyDisplay->curx += strlen(prompt);
    if(flags.standout)
	standoutend();

    xwaitforspace(s);
}

STATIC_OVL void
set_item_state(window, lineno, item)
    winid window;
    int lineno;
    tty_menu_item *item;
{
    char ch = item->selected ? (item->count == -1L ? '+' : '#') : '-';

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    tty_curs(window, 4, lineno);
    term_start_attr(item->attr);
    (void) putchar(ch);
    ttyDisplay->curx++;
    term_end_attr(item->attr);
}

STATIC_OVL void
set_all_on_page(window, page_start, page_end)
    winid window;
    tty_menu_item *page_start, *page_end;
{
    tty_menu_item *curr;
    int n;

    for (n = 0, curr = page_start; curr != page_end; n++, curr = curr->next)
	if (curr->identifier.a_void && !curr->selected) {
	    curr->selected = TRUE;
	    set_item_state(window, n, curr);
	}
}

STATIC_OVL void
unset_all_on_page(window, page_start, page_end)
    winid window;
    tty_menu_item *page_start, *page_end;
{
    tty_menu_item *curr;
    int n;

    for (n = 0, curr = page_start; curr != page_end; n++, curr = curr->next)
	if (curr->identifier.a_void && curr->selected) {
	    curr->selected = FALSE;
	    curr->count = -1L;
	    set_item_state(window, n, curr);
	}
}

STATIC_OVL void
invert_all_on_page(window, page_start, page_end, acc)
    winid window;
    tty_menu_item *page_start, *page_end;
    char acc;	/* group accelerator, 0 => all */
{
    tty_menu_item *curr;
    int n;

    for (n = 0, curr = page_start; curr != page_end; n++, curr = curr->next)
	if (curr->identifier.a_void && (acc == 0 || curr->gselector == acc)) {
	    if (curr->selected) {
		curr->selected = FALSE;
		curr->count = -1L;
	    } else
		curr->selected = TRUE;
	    set_item_state(window, n, curr);
	}
}

/*
 * Invert all entries that match the give group accelerator (or all if
 * zero).
 */
STATIC_OVL void
invert_all(window, page_start, page_end, acc)
    winid window;
    tty_menu_item *page_start, *page_end;
    char acc;	/* group accelerator, 0 => all */
{
    tty_menu_item *curr;
    boolean on_curr_page;
    struct WinDesc *cw =  wins[window];

    invert_all_on_page(window, page_start, page_end, acc);

    /* invert the rest */
    for (on_curr_page = FALSE, curr = cw->mlist; curr; curr = curr->next) {
	if (curr == page_start)
	    on_curr_page = TRUE;
	else if (curr == page_end)
	    on_curr_page = FALSE;

	if (!on_curr_page && curr->identifier.a_void
				&& (acc == 0 || curr->gselector == acc)) {
	    if (curr->selected) {
		curr->selected = FALSE;
		curr->count = -1;
	    } else
		curr->selected = TRUE;
	}
    }
}

#ifdef MENU_COLOR
STATIC_OVL boolean
get_menu_coloring(str, color, attr)
char *str;
int *color, *attr;
{
   struct menucoloring *tmpmc;
   if (iflags.use_menu_color)
     for (tmpmc = menu_colorings; tmpmc; tmpmc = tmpmc->next)
#ifdef USE_REGEX_MATCH
# ifdef GNU_REGEX
       if (re_search(&tmpmc->match, str, strlen(str), 0, 9999, 0) >= 0) {
# else
#  ifdef POSIX_REGEX
       if (regexec(&tmpmc->match, str, 0, NULL, 0) == 0) {
#  endif
# endif
#else
       if (pmatch(tmpmc->match, str)) {
#endif
	  *color = tmpmc->color;
	  *attr = tmpmc->attr;
	 return TRUE;
       }
   return FALSE;
}
#endif /* MENU_COLOR */

STATIC_OVL boolean
get_fleece_coloring(str, color, attr)
char *str;
int *color, *attr;
{
   struct menucoloring *tmpmc;
   if (iflags.use_menu_color)
     for (tmpmc = menu_colorings; tmpmc; tmpmc = tmpmc->next)
#ifdef USE_REGEX_MATCH
# ifdef GNU_REGEX
       if (re_search(&tmpmc->match, str, strlen(str), 0, 9999, 0) >= 0) {
# else
#  ifdef POSIX_REGEX
       if (regexec(&tmpmc->match, str, 0, NULL, 0) == 0) {
#  endif
# endif
#else
       if (pmatch(tmpmc->match, str)) {
#endif
	  *color = tmpmc->color;
	  *attr = tmpmc->attr;
	 return TRUE;
       }
   return FALSE;
}

STATIC_OVL void
process_menu_window(window, cw)
winid window;
struct WinDesc *cw;
{
    tty_menu_item *page_start, *page_end, *curr;
    long count;
    int n, curr_page, page_lines;
    boolean finished, counting, reset_count;
    char *cp, *rp, resp[QBUFSZ], gacc[QBUFSZ],
	 *msave, *morestr;

    curr_page = page_lines = 0;
    page_start = page_end = 0;
    msave = cw->morestr;	/* save the morestr */
    cw->morestr = morestr = (char*) alloc((unsigned) QBUFSZ);
    counting = FALSE;
    count = 0L;
    reset_count = TRUE;
    finished = FALSE;

    /* collect group accelerators; for PICK_NONE, they're ignored;
       for PICK_ONE, only those which match exactly one entry will be
       accepted; for PICK_ANY, those which match any entry are okay */
    gacc[0] = '\0';
    if (cw->how != PICK_NONE) {
	int i, gcnt[128];
#define GSELIDX(c) (c & 127)	/* guard against `signed char' */

	for (i = 0; i < SIZE(gcnt); i++) gcnt[i] = 0;
	for (n = 0, curr = cw->mlist; curr; curr = curr->next)
	    if (curr->gselector && curr->gselector != curr->selector) {
		++n;
		++gcnt[GSELIDX(curr->gselector)];
	    }

	if (n > 0)	/* at least one group accelerator found */
	    for (rp = gacc, curr = cw->mlist; curr; curr = curr->next)
		if (curr->gselector && !index(gacc, curr->gselector) &&
			(cw->how == PICK_ANY ||
			    gcnt[GSELIDX(curr->gselector)] == 1)) {
		    *rp++ = curr->gselector;
		    *rp = '\0';	/* re-terminate for index() */
		}
    }

    /* loop until finished */
    while (!finished) {
	if (program_state.done_hup) {
		morc = '\033';
		return;
	}
	if (reset_count) {
	    counting = FALSE;
	    count = 0;
	} else
	    reset_count = TRUE;

	if (!page_start) {
	    /* new page to be displayed */
	    if (curr_page < 0 || (cw->npages > 0 && curr_page >= cw->npages))
		panic("bad menu screen page #%d", curr_page);

	    /* clear screen */
	    if (!cw->offx) {	/* if not corner, do clearscreen */
		if(cw->offy) {
		    tty_curs(window, 1, 0);
		    cl_eos();
		} else
		    clear_screen();
	    }

	    rp = resp;
	    if (cw->npages > 0) {
		/* collect accelerators */
		page_start = cw->plist[curr_page];
		page_end = cw->plist[curr_page + 1];
		for (page_lines = 0, curr = page_start;
			curr != page_end;
			page_lines++, curr = curr->next) {
#ifdef MENU_COLOR
		   int color = NO_COLOR, attr = ATR_NONE;
		   boolean menucolr = FALSE;
#endif
		    if (curr->selector)
			*rp++ = curr->selector;

		    tty_curs(window, 1, page_lines);
		    if (cw->offx) cl_end();

		    (void) putchar(' ');
		    ++ttyDisplay->curx;
		    /*
		     * Don't use xputs() because (1) under unix it calls
		     * tputstr() which will interpret a '*' as some kind
		     * of padding information and (2) it calls xputc to
		     * actually output the character.  We're faster doing
		     * this.
		     */
		    /* add selector for display */  
		    if (curr->selector) {  
			/* because WIN32CON this must be done in  
			 * a brain-dead way */  
			putchar(curr->selector); ttyDisplay->curx++;  
			putchar(' '); ttyDisplay->curx++;  
			/* set item state */  
			if (curr->identifier.a_void != 0 && curr->selected) {  
			    if (curr->count == -1L)  
				(void) putchar('+'); /* all selected */  
			    else  
				(void) putchar('#'); /* count selected */  
			} else {  
			    putchar('-');  
			}  
			ttyDisplay->curx++;  
			putchar(' '); ttyDisplay->curx++;  
		    }  
		    if (curr->glyph != NO_GLYPH && iflags.use_menu_glyphs && !PlayerUninformation) {  
			int glyph_color = NO_COLOR;  
			int character;  
			unsigned special; /* unused */  
			/* map glyph to character and color */  
			mapglyph(curr->glyph, &character, &glyph_color, &special, 0, 0);  
  
/*			print_vt_code(AVTC_GLYPH_START, glyph2tile[curr->glyph]);  */
			if (glyph_color != NO_COLOR) term_start_color(glyph_color);  
			putchar(character);  
			if (glyph_color != NO_COLOR) term_end_color();  
/*			print_vt_code(AVTC_GLYPH_END, -1);  */
			/* Amy edit: use the brain-dead implementation and SUDDENLY IT WORKS ON WINDOWS WOOHOO :D */
			ttyDisplay->curx++;
			putchar(' ');  
			ttyDisplay->curx++;
		    }  
#ifdef MENU_COLOR
		   if (FleeceyScripts) {
			int fleececolor = rn2(CLR_MAX);
			while (fleececolor == NO_COLOR) fleececolor = rn2(CLR_MAX);
			term_start_color(fleececolor);
		   } else if (iflags.use_menu_color &&
		       (menucolr = get_menu_coloring(curr->str, &color,&attr))) {
		      term_start_attr(attr);
		      if (color != NO_COLOR) term_start_color(color);
		   } else
#endif
		    term_start_attr(curr->attr);
		    for (n = 0, cp = curr->str;
#ifndef WIN32CON
			  *cp && (int) ++ttyDisplay->curx < (int) ttyDisplay->cols;
			  cp++, n++) {
#else
			  *cp && (int) ttyDisplay->curx < (int) ttyDisplay->cols;
			  cp++, n++, ttyDisplay->curx++) {
#endif

			  if (FleeceyScripts && FunnyHallu) {
				int fleececolor = rn2(CLR_MAX);
				while (fleececolor == NO_COLOR) fleececolor = rn2(CLR_MAX);
				term_start_color(fleececolor);

			  }

			  (void) putchar(*cp);
			}
#ifdef MENU_COLOR
		   if (FleeceyScripts) {
			term_end_color();
		   } else if (iflags.use_menu_color && menucolr) {
		      if (color != NO_COLOR) term_end_color();
		      term_end_attr(attr);
		   } else
#endif
		    term_end_attr(curr->attr);
		}
	    } else {
		page_start = 0;
		page_end = 0;
		page_lines = 0;
	    }
	    *rp = 0;

	    /* corner window - clear extra lines from last page */
	    if (cw->offx) {
		for (n = page_lines + 1; n < cw->maxrow; n++) {
		    tty_curs(window, 1, n);
		    cl_end();
		}
	    }

	    /* set extra chars.. */
	    strcat(resp, default_menu_cmds);
	    strcat(resp, "0123456789\033\n\r");	/* counts, quit */
	    strcat(resp, gacc);			/* group accelerators */
	    strcat(resp, mapped_menu_cmds);

	    if (cw->npages > 1)
			sprintf(cw->morestr, "(%d of %d)",
				curr_page + 1, (int) cw->npages);
	    else if (msave)
			strcpy(cw->morestr, msave);
	    else
			strcpy(cw->morestr, defmorestr);

	    tty_curs(window, 1, page_lines);
	    cl_end();
	    dmore(cw, resp);
	} else {
	    /* just put the cursor back... */
	    tty_curs(window, (int) strlen(cw->morestr) + 2, page_lines);
	    xwaitforspace(resp);
	}

	morc = map_menu_cmd(morc);
	switch (morc) {
	    case '0':
		/* special case: '0' is also the default ball class */
		if (!counting && index(gacc, morc)) goto group_accel;
		/* fall through to count the zero */
	    case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
		count = (count * 10L) + (long) (morc - '0');
		/*
		 * It is debatable whether we should allow 0 to
		 * start a count.  There is no difference if the
		 * item is selected.  If not selected, then
		 * "0b" could mean:
		 *
		 *	count starting zero:	"zero b's"
		 *	ignore starting zero:	"select b"
		 *
		 * At present I don't know which is better.
		 */
		if (count != 0L) {	/* ignore leading zeros */
		    counting = TRUE;
		    reset_count = FALSE;
		}
		break;
	    case '\033':	/* cancel - from counting or loop */
		if (!counting) {
		    /* deselect everything */
		    for (curr = cw->mlist; curr; curr = curr->next) {
			curr->selected = FALSE;
			curr->count = -1L;
		    }
		    cw->flags |= WIN_CANCELLED;
		    finished = TRUE;
		}
		/* else only stop count */
		break;
	    case '\0':		/* finished (commit) */
	    case '\n':
	    case '\r':
		/* only finished if we are actually picking something */
		if (cw->how != PICK_NONE) {
		    finished = TRUE;
		    break;
		}
		/* else fall through */
	    case MENU_NEXT_PAGE:
		if (cw->npages > 0 && curr_page != cw->npages - 1) {
		    curr_page++;
		    page_start = 0;
		} else
		    finished = TRUE;	/* questionable behavior */
		break;
	    case MENU_PREVIOUS_PAGE:
		if (cw->npages > 0 && curr_page != 0) {
		    --curr_page;
		    page_start = 0;
		}
		break;
	    case MENU_FIRST_PAGE:
		if (cw->npages > 0 && curr_page != 0) {
		    page_start = 0;
		    curr_page = 0;
		}
		break;
	    case MENU_LAST_PAGE:
		if (cw->npages > 0 && curr_page != cw->npages - 1) {
		    page_start = 0;
		    curr_page = cw->npages - 1;
		}
		break;
	    case MENU_SELECT_PAGE:
		if (cw->how == PICK_ANY)
		    set_all_on_page(window, page_start, page_end);
		break;
	    case MENU_UNSELECT_PAGE:
		unset_all_on_page(window, page_start, page_end);
		break;
	    case MENU_INVERT_PAGE:
		if (cw->how == PICK_ANY)
		    invert_all_on_page(window, page_start, page_end, 0);
		break;
	    case MENU_SELECT_ALL:
		if (cw->how == PICK_ANY) {
		    set_all_on_page(window, page_start, page_end);
		    /* set the rest */
		    for (curr = cw->mlist; curr; curr = curr->next)
			if (curr->identifier.a_void && !curr->selected)
			    curr->selected = TRUE;
		}
		break;
	    case MENU_UNSELECT_ALL:
		unset_all_on_page(window, page_start, page_end);
		/* unset the rest */
		for (curr = cw->mlist; curr; curr = curr->next)
		    if (curr->identifier.a_void && curr->selected) {
			curr->selected = FALSE;
			curr->count = -1;
		    }
		break;
	    case MENU_INVERT_ALL:
		if (cw->how == PICK_ANY)
		    invert_all(window, page_start, page_end, 0);
		break;
	    default:
		if (cw->how == PICK_NONE || !index(resp, morc)) {
		    /* unacceptable input received */
		    tty_nhbell();
		    break;
		} else if (index(gacc, morc)) {
 group_accel:
		    /* group accelerator; for the PICK_ONE case, we know that
		       it matches exactly one item in order to be in gacc[] */
		    invert_all(window, page_start, page_end, morc);
		    if (cw->how == PICK_ONE) finished = TRUE;
		    break;
		}
		/* find, toggle, and possibly update */
		for (n = 0, curr = page_start;
			curr != page_end;
			n++, curr = curr->next)
		    if (morc == curr->selector) {
			if (curr->selected) {
			    if (counting && count > 0) {
				curr->count = count;
				set_item_state(window, n, curr);
			    } else { /* change state */
				curr->selected = FALSE;
				curr->count = -1L;
				set_item_state(window, n, curr);
			    }
			} else {	/* !selected */
			    if (counting && count > 0) {
				curr->count = count;
				curr->selected = TRUE;
				set_item_state(window, n, curr);
			    } else if (!counting) {
				curr->selected = TRUE;
				set_item_state(window, n, curr);
			    }
			    /* do nothing counting&&count==0 */
			}

			if (cw->how == PICK_ONE) finished = TRUE;
			break;	/* from `for' loop */
		    }
		break;
	}

    } /* while */
    cw->morestr = msave;
    free((void *)morestr);
}

STATIC_OVL void
process_text_window(window, cw)
winid window;
struct WinDesc *cw;
{
    int i, n, attr;
    register char *cp;

    for (n = 0, i = 0; i < cw->maxrow; i++) {
	if (program_state.done_hup) {
		morc = '\033';
		return;
	}
	if (!cw->offx && (n + cw->offy == ttyDisplay->rows - 1)) {
	    tty_curs(window, 1, n);
	    cl_end();
	    dmore(cw, quitchars);
	    if (morc == DOESCAPE) {
		cw->flags |= WIN_CANCELLED;
		break;
	    }
	    if (cw->offy) {
		tty_curs(window, 1, 0);
		cl_eos();
	    } else
		clear_screen();
	    n = 0;
	}
	tty_curs(window, 1, n++);
	if (cw->offx) cl_end();
	if (cw->data[i]) {
	    attr = cw->data[i][0] - 1;
	    if (cw->offx) {
		(void) putchar(' '); ++ttyDisplay->curx;
	    }
	    term_start_attr(attr);
	    for (cp = &cw->data[i][1];
#ifndef WIN32CON
		    *cp && (int) ++ttyDisplay->curx < (int) ttyDisplay->cols;
		    cp++)
#else
		    *cp && (int) ttyDisplay->curx < (int) ttyDisplay->cols;
		    cp++, ttyDisplay->curx++)
#endif
		(void) putchar(*cp);
	    term_end_attr(attr);
	}
    }
    if (i == cw->maxrow) {
	tty_curs(BASE_WINDOW, (int)cw->offx + 1,
		 (cw->type == NHW_TEXT) ? (int) ttyDisplay->rows - 1 : n);
	cl_end();
	dmore(cw, quitchars);
	if (morc == DOESCAPE)
	    cw->flags |= WIN_CANCELLED;
    }
}

/*ARGSUSED*/
void
tty_display_nhwindow(window, blocking)
    winid window;
    boolean blocking;	/* with ttys, all windows are blocking */
{
    register struct WinDesc *cw = 0;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    if(window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0)
	panic(winpanicstr,  window);
    if(cw->flags & WIN_CANCELLED)
	return;
    ttyDisplay->lastwin = window;
    ttyDisplay->rawprint = 0;

    switch(cw->type) {
    case NHW_MESSAGE:
	if(ttyDisplay->toplin == 1) {
	    more();
	    ttyDisplay->toplin = 1; /* more resets this */
	    tty_clear_nhwindow(window);
	} else
	    ttyDisplay->toplin = 0;
	cw->curx = cw->cury = 0;
	if(!cw->active)
	    iflags.window_inited = TRUE;
	break;
    case NHW_MAP:
	end_glyphout();
	if(blocking) {
	    if(!ttyDisplay->toplin) ttyDisplay->toplin = 1;
	    tty_display_nhwindow(WIN_MESSAGE, TRUE);
	    return;
	}
    case NHW_BASE:
	(void) fflush(stdout);
	break;
    case NHW_TEXT:
	cw->maxcol = ttyDisplay->cols; /* force full-screen mode */
	/*FALLTHRU*/
    case NHW_MENU:
	cw->active = 1;
	/* avoid converting to uchar before calculations are finished */
	cw->offx = (uchar) (int)
	    max((int) 10, (int) (ttyDisplay->cols - cw->maxcol - 1));
	if(cw->type == NHW_MENU)
	    cw->offy = 0;
	if(ttyDisplay->toplin == 1)
	    tty_display_nhwindow(WIN_MESSAGE, TRUE);
	if(cw->offx == 10 || cw->maxrow >= (int) ttyDisplay->rows) {
	    cw->offx = 0;
	    if(cw->offy) {
		tty_curs(window, 1, 0);
		cl_eos();
	    } else
		clear_screen();
	    ttyDisplay->toplin = 0;
	} else
	    tty_clear_nhwindow(WIN_MESSAGE);

	if (cw->data || !cw->maxrow)
	    process_text_window(window, cw);
	else
	    process_menu_window(window, cw);
	break;
    }
    cw->active = 1;
}

void
tty_dismiss_nhwindow(window)
    winid window;
{
    register struct WinDesc *cw = 0;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    if(window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0)
	panic(winpanicstr,  window);

    switch(cw->type) {
    case NHW_MESSAGE:
	if (ttyDisplay->toplin)
	    tty_display_nhwindow(WIN_MESSAGE, TRUE);
	/*FALLTHRU*/
    case NHW_STATUS:
    case NHW_BASE:
    case NHW_MAP:
	/*
	 * these should only get dismissed when the game is going away
	 * or suspending
	 */
	tty_curs(BASE_WINDOW, 1, (int)ttyDisplay->rows-1);
	cw->active = 0;
	break;
    case NHW_MENU:
    case NHW_TEXT:
	if(cw->active) {
	    if (iflags.window_inited) {
		/* otherwise dismissing the text endwin after other windows
		 * are dismissed tries to redraw the map and panics.  since
		 * the whole reason for dismissing the other windows was to
		 * leave the ending window on the screen, we don't want to
		 * erase it anyway.
		 */
		erase_menu_or_text(window, cw, FALSE);
	    }
	    cw->active = 0;
	}
	break;
    }
    cw->flags = 0;
}

void
tty_destroy_nhwindow(window)
    winid window;
{
    register struct WinDesc *cw = 0;

    if(window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0)
	panic(winpanicstr,  window);

    if(cw->active)
	tty_dismiss_nhwindow(window);
    if(cw->type == NHW_MESSAGE)
	iflags.window_inited = 0;
    if(cw->type == NHW_MAP)
	clear_screen();

    free_window_info(cw, TRUE);
    free((void *)cw);
    wins[window] = 0;
}

void
tty_curs(window, x, y)
winid window;
register int x, y;	/* not xchar: perhaps xchar is unsigned and
			   curx-x would be unsigned as well */
{
    struct WinDesc *cw = 0;
    int cx = ttyDisplay->curx;
    int cy = ttyDisplay->cury;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    if(window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0)
	panic(winpanicstr,  window);
    ttyDisplay->lastwin = window;

#if defined(USE_TILES) && defined(MSDOS)
    adjust_cursor_flags(cw);
#endif
    cw->curx = --x;	/* column 0 is never used */
    cw->cury = y;
#ifdef DEBUG
    if(x<0 || y<0 || y >= cw->rows || x > cw->cols) {
	const char *s = "[unknown type]";
	switch(cw->type) {
	case NHW_MESSAGE: s = "[topl window]"; break;
	case NHW_STATUS: s = "[status window]"; break;
	case NHW_MAP: s = "[map window]"; break;
	case NHW_MENU: s = "[corner window]"; break;
	case NHW_TEXT: s = "[text window]"; break;
	case NHW_BASE: s = "[base window]"; break;
	}
	impossible("bad curs positioning win %d %s (%d,%d)", window, s, x, y);
	return;
    }
#endif
    x += cw->offx;
    y += cw->offy;

#ifdef CLIPPING
    if(clipping && window == WIN_MAP) {
	x -= clipx;
	y -= clipy;
    }
#endif

    if (y == cy && x == cx)
	return;

    if(cw->type == NHW_MAP)
	end_glyphout();

#ifndef NO_TERMS
    if(!nh_ND && (cx != x || x <= 3)) { /* Extremely primitive */
	cmov(x, y); /* bunker!wtm */
	return;
    }
#endif

    if((cy -= y) < 0) cy = -cy;
    if((cx -= x) < 0) cx = -cx;
    if(cy <= 3 && cx <= 3) {
	nocmov(x, y);
#ifndef NO_TERMS
    } else if ((x <= 3 && cy <= 3) || (!nh_CM && x < cx)) {
	(void) putchar('\r');
	ttyDisplay->curx = 0;
	nocmov(x, y);
    } else if (!nh_CM) {
	nocmov(x, y);
#endif
    } else
	cmov(x, y);

    ttyDisplay->curx = x;
    ttyDisplay->cury = y;
}

STATIC_OVL void
tty_putsym(window, x, y, ch)
    winid window;
    int x, y;
    char ch;
{
    register struct WinDesc *cw = 0;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    if(window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0)
	panic(winpanicstr,  window);

    switch(cw->type) {
    case NHW_STATUS:
    case NHW_MAP:
    case NHW_BASE:
	tty_curs(window, x, y);
	(void) putchar(ch);
	ttyDisplay->curx++;
	cw->curx++;
	break;
    case NHW_MESSAGE:
    case NHW_MENU:
    case NHW_TEXT:
	impossible("Can't putsym to window type %d", cw->type);
	break;
    }
}


const char*
compress_str(str)
const char *str;
{
	static char cbuf[BUFSZ];
	/* compress in case line too long */
	if((int)strlen(str) >= CO) {
		register const char *bp0 = str;
		register char *bp1 = cbuf;

		do {
#ifdef CLIPPING
			if(*bp0 != ' ' || bp0[1] != ' ')
#else
			if(*bp0 != ' ' || bp0[1] != ' ' || bp0[2] != ' ')
#endif
				*bp1++ = *bp0;
		} while(*bp0++);
	} else
	    return str;
	return cbuf;
}

void
tty_putstr(window, attr, str)
    winid window;
    int attr;
    const char *str;
{
    register struct WinDesc *cw = 0;
    register char *ob;
    register const char *nb;
    register int i, j, n0;
    register int k;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    /* Assume there's a real problem if the window is missing --
     * probably a panic message
     */
    if(window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0) {
	tty_raw_print(str);
	return;
    }

    if(str == (const char*)0 ||
	((cw->flags & WIN_CANCELLED) && (cw->type != NHW_MESSAGE)))
		return;
    if(cw->type != NHW_MESSAGE)
		str = compress_str(str);

    ttyDisplay->lastwin = window;

    switch(cw->type) {
    case NHW_MESSAGE:
	/* really do this later */
#if defined(USER_SOUNDS) && defined(WIN32CON)
	play_sound_for_message(str);
#endif
	update_topl(str);
	break;

    case NHW_STATUS:
#ifdef ALLEG_FX
        if (iflags.usealleg) {
            alleg_stats(str, cw->cury);
            break;
        }
#endif
	ob = &cw->data[cw->cury][j = cw->curx];
	if(flags.botlx) *ob = 0;
	if(!cw->cury && (int)strlen(str) >= CO) {
	    /* the characters before "St:" are unnecessary */
	    nb = index(str, ':');
	    if(nb && nb > str+2)
		str = nb - 2;
	}
	k = 0;
	/* WAC - attempt to break or shorten line 2 if it's too long */
	if(cw->cury && (int)strlen(str) >= CO) {
	    if(cw->cury < (cw->maxrow - 1))
		for(k = CO - 1; k && str[k] != ' ';)
		    k--;
	    if(!k || (int)strlen(str + k + 1) >= CO) {
		str = shorten_bot2(str, CO);
		k = 0;
	    }
	}

	nb = str;
	for(i = cw->curx+1, n0 = cw->cols; i < n0; i++, nb++) {
	    if(!*nb) {
		if(*ob || flags.botlx) {
		    /* last char printed may be in middle of line */
		    tty_curs(WIN_STATUS, i, cw->cury);
		    cl_end();
		}
		break;
	    }
	    if(*ob != *nb)
		tty_putsym(WIN_STATUS, i, cw->cury, *nb);
	    if(*ob) ob++;

	    /* String break? --WAC */
	    if(i == k) {
		(void) strncpy(&cw->data[cw->cury][j], str, cw->cols - j - 1);
		cw->data[cw->cury][min(k+1, cw->cols-1)] = '\0';

		if(*ob || flags.botlx) {
		    /* last char printed may be in middle of line */
		    tty_curs(WIN_STATUS, k+1, cw->cury);
		    cl_end();
		}
		nb++;

		str = nb + 1;
		i = j = 0;
		cw->curx = 0;
		cw->cury++;

		ob = &cw->data[cw->cury][cw->curx];
		if(flags.botlx) *ob = 0;

		tty_curs(WIN_STATUS, 1, cw->cury);
		k = 0;
	    }
	}

	(void) strncpy(&cw->data[cw->cury][j], str, cw->cols - j - 1);
	cw->data[cw->cury][cw->cols-1] = '\0'; /* null terminate */
	/* ALI - Clear third line if present and unused */
	if (cw->cury == 1 && cw->cury < (cw->maxrow - 1))
	{
	    cw->data[cw->cury + 1][0] = '\0';
	    tty_curs(WIN_STATUS, 1, cw->cury + 1);
	    cl_end();
	}
	cw->cury = (cw->cury+1) % 2;
	cw->curx = 0;
	break;
    case NHW_MAP:
	tty_curs(window, cw->curx+1, cw->cury);
	term_start_attr(attr);
	while(*str && (int) ttyDisplay->curx < (int) ttyDisplay->cols-1) {
	    (void) putchar(*str);
	    str++;
	    ttyDisplay->curx++;
	}
	cw->curx = 0;
	cw->cury++;
	term_end_attr(attr);
	break;
    case NHW_BASE:
	tty_curs(window, cw->curx+1, cw->cury);
	term_start_attr(attr);
	while (*str) {
	    if ((int) ttyDisplay->curx >= (int) ttyDisplay->cols-1) {
		cw->curx = 0;
		cw->cury++;
		tty_curs(window, cw->curx+1, cw->cury);
	    }
	    (void) putchar(*str);
	    str++;
	    ttyDisplay->curx++;
	}
	cw->curx = 0;
	cw->cury++;
	term_end_attr(attr);
	break;
    case NHW_MENU:
    case NHW_TEXT:
	if(cw->type == NHW_TEXT && cw->cury == ttyDisplay->rows-1) {
	    /* not a menu, so save memory and output 1 page at a time */
	    cw->maxcol = ttyDisplay->cols; /* force full-screen mode */
	    tty_display_nhwindow(window, TRUE);
	    for(i=0; i<cw->maxrow; i++)
		if(cw->data[i]){
		    free((void *)cw->data[i]);
		    cw->data[i] = 0;
		}
	    cw->maxrow = cw->cury = 0;
	}
	/* always grows one at a time, but alloc 12 at a time */
	if(cw->cury >= cw->rows) {
	    char **tmp;

	    cw->rows += 12;
	    tmp = (char **) alloc(sizeof(char *) * (unsigned)cw->rows);
	    for(i=0; i<cw->maxrow; i++)
		tmp[i] = cw->data[i];
	    if(cw->data)
		free((void *)cw->data);
	    cw->data = tmp;

	    for(i=cw->maxrow; i<cw->rows; i++)
		cw->data[i] = 0;
	}
	if(cw->data[cw->cury])
	    free((void *)cw->data[cw->cury]);
	n0 = strlen(str) + 1;
	ob = cw->data[cw->cury] = (char *)alloc((unsigned)n0 + 1);
	*ob++ = (char)(attr + 1);	/* avoid nuls, for convenience */
	strcpy(ob, str);

	if(n0 > cw->maxcol)
	    cw->maxcol = n0;
	if(++cw->cury > cw->maxrow)
	    cw->maxrow = cw->cury;
	if(n0 > CO) {
	    /* attempt to break the line */
	    for(i = CO-1; i && str[i] != ' ' && str[i] != '\n';)
			i--;
	    if(i) {
		cw->data[cw->cury-1][++i] = '\0';
		tty_putstr(window, attr, &str[i]);
	    }

	}
	break;
    }
}

void
#ifdef FILE_AREAS
tty_display_file(farea, fname, complain)
const char *farea;
#else
tty_display_file(fname, complain)
#endif
const char *fname;
boolean complain;
{
#ifdef DEF_PAGER			/* this implies that UNIX is defined */
    {
	/* use external pager; this may give security problems */
#ifdef FILE_AREAS
    register int fd = open_area(farea, fname, 0, 0);
#else
	register int fd = open(fname, 0);
#endif

	if(fd < 0) {
	    if(complain) pline("Cannot open %s.", fname);
	    else docrt();
	    return;
	}
	if(child(1)) {
	    /* Now that child() does a setuid(getuid()) and a chdir(),
	       we may not be able to open file fname anymore, so make
	       it stdin. */
	    (void) close(0);
	    if(dup(fd)) {
		if(complain) raw_printf("Cannot open %s as stdin.", fname);
	    } else {
		(void) execlp(catmore, "page", (char *)0);
		if(complain) raw_printf("Cannot exec %s.", catmore);
	    }
	    if(complain) sleep(10); /* want to wait_synch() but stdin is gone */
	    terminate(EXIT_FAILURE);
	}
	(void) close(fd);
    }
#else	/* DEF_PAGER */
    {
	dlb *f;
	char buf[BUFSZ];
	char *cr;

	tty_clear_nhwindow(WIN_MESSAGE);
#ifdef FILE_AREAS
    f = dlb_fopen_area(farea, fname, "r");
#else
    f = dlb_fopen(fname, "r");
#endif
	if (!f) {
	    if(complain) {
		home();  tty_mark_synch();  tty_raw_print("");
		perror(fname);  tty_wait_synch();
		pline("Cannot open \"%s\".", fname);
	    } else if(u.ux) docrt();
	} else {
	    winid datawin = tty_create_nhwindow(NHW_TEXT);
	    boolean empty = TRUE;

	    if(complain
#ifndef NO_TERMS
		&& nh_CD
#endif
	    ) {
		/* attempt to scroll text below map window if there's room */
		wins[datawin]->offy = wins[WIN_STATUS]->offy+3;
		if((int) wins[datawin]->offy + 12 > (int) ttyDisplay->rows)
		    wins[datawin]->offy = 0;
	    }
	    while (dlb_fgets(buf, BUFSZ, f)) {
		if ((cr = index(buf, '\n')) != 0) *cr = 0;
#ifdef MSDOS
		if ((cr = index(buf, '\r')) != 0) *cr = 0;
#endif
		if (index(buf, '\t') != 0) (void) tabexpand(buf);
		empty = FALSE;
		tty_putstr(datawin, 0, buf);
		if(wins[datawin]->flags & WIN_CANCELLED)
		    break;
	    }
	    if (!empty) tty_display_nhwindow(datawin, FALSE);
	    tty_destroy_nhwindow(datawin);
	    (void) dlb_fclose(f);
	}
    }
#endif /* DEF_PAGER */
}

void
tty_start_menu(window)
    winid window;
{
    tty_clear_nhwindow(window);
    return;
}

/*ARGSUSED*/
/*
 * Add a menu item to the beginning of the menu list.  This list is reversed
 * later.
 */
void
tty_add_menu(window, glyph, identifier, ch, gch, attr, str, preselected)
    winid window;	/* window to use, must be of type NHW_MENU */
    int glyph;		/* glyph to display with item */
    const anything *identifier;	/* what to return if selected */
    char ch;		/* keyboard accelerator (0 = pick our own) */
    char gch;		/* group accelerator (0 = no group) */
    int attr;		/* attribute for string (like tty_putstr()) */
    const char *str;	/* menu string */
    boolean preselected; /* item is marked as selected */
{
#if defined(MAC_MPW)
# pragma unused(glyph)
#endif
    register struct WinDesc *cw = 0;
    tty_menu_item *item;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    if (str == (const char*) 0)
	return;

    if (window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0
		|| cw->type != NHW_MENU)
	panic(winpanicstr,  window);

    cw->nitems++;

    item = (tty_menu_item *) alloc(sizeof(tty_menu_item));
    item->identifier = *identifier;
    item->count = -1L;
    item->selected = preselected;
    item->selector = ch;
    item->gselector = gch;
    item->attr = attr;
    item->str = copy_of(str);
    item->glyph = glyph;

    item->next = cw->mlist;
    cw->mlist = item;
}

/* Invert the given list, can handle NULL as an input. */
STATIC_OVL tty_menu_item *
reverse(curr)
    tty_menu_item *curr;
{
    tty_menu_item *next, *head = 0;

    while (curr) {
	next = curr->next;
	curr->next = head;
	head = curr;
	curr = next;
    }
    return head;
}

/*
 * End a menu in this window, window must a type NHW_MENU.  This routine
 * processes the string list.  We calculate the # of pages, then assign
 * keyboard accelerators as needed.  Finally we decide on the width and
 * height of the window.
 */
void
tty_end_menu(window, prompt)
    winid window;	/* menu to use */
    const char *prompt;	/* prompt to for menu */
{
    struct WinDesc *cw = 0;
    tty_menu_item *curr;
    short len;
    int lmax, n;
    char menu_ch;

    if (window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0 ||
		cw->type != NHW_MENU)
	panic(winpanicstr,  window);

    /* Reverse the list so that items are in correct order. */
    cw->mlist = reverse(cw->mlist);

    /* Put the promt at the beginning of the menu. */
    if (prompt) {
	anything any;

	any.a_void = 0;	/* not selectable */
	tty_add_menu(window, NO_GLYPH, &any, 0, 0, ATR_NONE, "", MENU_UNSELECTED);
	tty_add_menu(window, NO_GLYPH, &any, 0, 0, ATR_NONE, prompt, MENU_UNSELECTED);
    }

    lmax = min(52, (int)ttyDisplay->rows - 1);		/* # lines per page */
    cw->npages = (cw->nitems + (lmax - 1)) / lmax;	/* # of pages */

    /* make sure page list is large enough */
    if (cw->plist_size < cw->npages+1 /*need 1 slot beyond last*/) {
	if (cw->plist) free((void *)cw->plist);
	cw->plist_size = cw->npages + 1;
	cw->plist = (tty_menu_item **)
			alloc(cw->plist_size * sizeof(tty_menu_item *));
    }

    cw->cols = 0; /* cols is set when the win is initialized... (why?) */
    menu_ch = '?';	/* lint suppression */
    for (n = 0, curr = cw->mlist; curr; n++, curr = curr->next) {
	/* set page boundaries and character accelerators */
	if ((n % lmax) == 0) {
	    menu_ch = 'a';
	    cw->plist[n/lmax] = curr;
	}
	if (curr->identifier.a_void && !curr->selector) {
	    curr->selector = menu_ch;
	    if (menu_ch++ == 'z') menu_ch = 'A';
	}

	/* cut off any lines that are too long */
	len = strlen(curr->str) + 2;	/* extra space at beg & end */
  
	if (curr->selector) {  
	    /* extra space for keyboard accelerator */  
	    len += 4;  
	    if (curr->glyph != NO_GLYPH && iflags.use_menu_glyphs && !PlayerUninformation) {  
		/* extra space for glyph */  
		len += 2;  
	    }  
	}  
  
	if (len > (int)ttyDisplay->cols) {
	    curr->str[strlen(curr->str) - (len - ttyDisplay->cols)] = 0;
	    len = ttyDisplay->cols;
	}
	if (len > cw->cols) cw->cols = len;
    }
    cw->plist[cw->npages] = 0;	/* plist terminator */

    /*
     * If greater than 1 page, morestr is "(x of y) " otherwise, "(end) "
     */
    if (cw->npages > 1) {
	char buf[QBUFSZ];
	/* produce the largest demo string */
	sprintf(buf, "(%d of %d) ", cw->npages, cw->npages);
	len = strlen(buf);
	cw->morestr = copy_of("");
    } else {
	cw->morestr = copy_of("(end) ");
	len = strlen(cw->morestr);
    }

    if (len > (int)ttyDisplay->cols) {
	/* truncate the prompt if its too long for the screen */
	if (cw->npages <= 1)	/* only str in single page case */
	    cw->morestr[ttyDisplay->cols] = 0;
	len = ttyDisplay->cols;
    }
    if (len > cw->cols) cw->cols = len;

    cw->maxcol = cw->cols;

    /*
     * The number of lines in the first page plus the morestr will be the
     * maximum size of the window.
     */
    if (cw->npages > 1)
	cw->maxrow = cw->rows = lmax + 1;
    else
	cw->maxrow = cw->rows = cw->nitems + 1;
}

int
tty_select_menu(window, how, menu_list)
    winid window;
    int how;
    menu_item **menu_list;
{
    register struct WinDesc *cw = 0;
    tty_menu_item *curr;
    menu_item *mi;
    int n, cancelled;

    if(window == WIN_ERR || (cw = wins[window]) == (struct WinDesc *) 0
       || cw->type != NHW_MENU)
	panic(winpanicstr,  window);

    *menu_list = (menu_item *) 0;
    cw->how = (short) how;
    morc = 0;
    tty_display_nhwindow(window, TRUE);
    cancelled = !!(cw->flags & WIN_CANCELLED);
    tty_dismiss_nhwindow(window);	/* does not destroy window data */

    if (cancelled) {
	n = -1;
    } else {
	for (n = 0, curr = cw->mlist; curr; curr = curr->next)
	    if (curr->selected) n++;
    }

    if (n > 0) {
	*menu_list = (menu_item *) alloc(n * sizeof(menu_item));
	for (mi = *menu_list, curr = cw->mlist; curr; curr = curr->next)
	    if (curr->selected) {
		mi->item = curr->identifier;
		mi->count = curr->count;
		mi++;
	    }
    }

    return n;
}

/* special hack for treating top line --More-- as a one item menu */
char
tty_message_menu(let, how, mesg)
char let;
int how;
const char *mesg;
{
	if (program_state.done_hup) {
		morc = '\033';
		return 0;
	}

    /* "menu" without selection; use ordinary pline, no more() */
    if (how == PICK_NONE) {
	pline("%s", mesg);
	return 0;
    }

    ttyDisplay->dismiss_more = let;
    morc = 0;
    /* barebones pline(); since we're only supposed to be called after
       response to a prompt, we'll assume that the display is up to date */
    tty_putstr(WIN_MESSAGE, 0, mesg);
    /* if `mesg' didn't wrap (triggering --More--), force --More-- now */
    if (ttyDisplay->toplin == 1) {
		more();
		ttyDisplay->toplin = 1; /* more resets this */
		tty_clear_nhwindow(WIN_MESSAGE);
    }
    /* normally <ESC> means skip further messages, but in this case
       it means cancel the current prompt; any other messages should
       continue to be output normally */
    wins[WIN_MESSAGE]->flags &= ~WIN_CANCELLED;
    ttyDisplay->dismiss_more = 0;

    return ((how == PICK_ONE && morc == let) || morc == DOESCAPE) ? morc : '\0';
}

void
tty_update_inventory()
{
    return;
}

void
tty_mark_synch()
{
	if (program_state.done_hup) {
		morc = '\033';
		return;
	}
    (void) fflush(stdout);
}

void
tty_wait_synch()
{
	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    /* we just need to make sure all windows are synch'd */
    if(!ttyDisplay || ttyDisplay->rawprint) {
	getret();
	if(ttyDisplay) ttyDisplay->rawprint = 0;
    } else {
	tty_display_nhwindow(WIN_MAP, FALSE);
	if(ttyDisplay->inmore) {

	if (youmonst.data && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover

#if defined(WIN32)
&& !program_state.exiting
#endif

	&& automore_active() && (u.automorefuckthisshit == TRUE) ) {
		return;
	}

	    addtopl("--More--");
	    (void) fflush(stdout);
	} else if(ttyDisplay->inread > program_state.gameover) {
	    /* this can only happen if we were reading and got interrupted */
	    ttyDisplay->toplin = 3;
	    /* do this twice; 1st time gets the Quit? message again */
	    (void) tty_doprev_message();
	    (void) tty_doprev_message();
	    ttyDisplay->intr++;
	    (void) fflush(stdout);
	}
    }
}

void
docorner(xmin, ymax)
    register int xmin, ymax;
{
    register int y;
    register struct WinDesc *cw = wins[WIN_MAP];

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    /*if (u.uswallow) {*/	/* Can be done more efficiently */
	/*swallowed(1);*/	/* according to amateurhour this can result in an infinite loop and isn't really needed --Amy */
	/*return;
    }*/

#if defined(SIGWINCH) && defined(CLIPPING)
    if(ymax > LI) ymax = LI;		/* can happen if window gets smaller */
#endif
    for (y = 0; y < ymax; y++) {
	tty_curs(BASE_WINDOW, xmin,y);	/* move cursor */
	cl_end();			/* clear to end of line */
#ifdef CLIPPING
	if (y<(int) cw->offy || y+clipy > ROWNO)
		continue; /* only refresh board */
#if defined(USE_TILES) && defined(MSDOS)
	if (iflags.tile_view)
		row_refresh((xmin/2)+clipx-((int)cw->offx/2),COLNO-1,y+clipy-(int)cw->offy);
	else
#endif
	row_refresh(xmin+clipx-(int)cw->offx,COLNO-1,y+clipy-(int)cw->offy);
#else
	if (y<cw->offy || y > ROWNO) continue; /* only refresh board  */
	row_refresh(xmin-(int)cw->offx,COLNO-1,y-(int)cw->offy);
#endif
    }

    end_glyphout();
    if (ymax >= (int) wins[WIN_STATUS]->offy) {
					/* we have wrecked the bottom line */
	flags.botlx = 1;
	bot();
    }
}

void
end_glyphout()
{

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

#if defined(ASCIIGRAPH) && !defined(NO_TERMS)
    if (GFlag) {
	GFlag = FALSE;
	graph_off();
    }
#endif
#ifdef TEXTCOLOR
    if(ttyDisplay->color != NO_COLOR) {
	term_end_color();
	ttyDisplay->color = NO_COLOR;
    }
#endif
}

#ifndef WIN32
void
g_putch(in_ch)
int in_ch;
{
    register char ch = (char)in_ch;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

# if defined(ASCIIGRAPH) && !defined(NO_TERMS)
    if (iflags.IBMgraphics || iflags.eight_bit_tty) {
	/* IBM-compatible displays don't need other stuff */
	(void) putchar(ch);
    } else if (ch & 0x80) {
	if (!GFlag || HE_resets_AS) {
	    graph_on();
	    GFlag = TRUE;
	}
	(void) putchar((ch ^ 0x80)); /* Strip 8th bit */
    } else {
	if (GFlag) {
	    graph_off();
	    GFlag = FALSE;
	}
	(void) putchar(ch);
    }

#else
    (void) putchar(ch);

#endif	/* ASCIIGRAPH && !NO_TERMS */

    return;
}
#endif /* !WIN32 */

#ifdef CLIPPING
void
setclipped()
{
	clipping = TRUE;
	clipx = clipy = 0;
	clipxmax = CO;
	clipymax = LI - 3;
}

void
tty_cliparound(x, y)
int x, y;
{
	extern boolean restoring;
	int oldx = clipx, oldy = clipy;

	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

	if (!clipping) return;
	if (x < clipx + 5) {
		clipx = max(0, x - 20);
		clipxmax = clipx + CO;
	}
	else if (x > clipxmax - 5) {
		clipxmax = min(COLNO, clipxmax + 20);
		clipx = clipxmax - CO;
	}
	if (y < clipy + 2) {
		clipy = max(0, y - (clipymax - clipy) / 2);
		clipymax = clipy + (LI - 3);
	}
	else if (y > clipymax - 2) {
		clipymax = min(ROWNO, clipymax + (clipymax - clipy) / 2);
		clipy = clipymax - (LI - 3);
	}
	if (clipx != oldx || clipy != oldy) {
	    if (on_level(&u.uz0, &u.uz) && !restoring)
		(void) doredraw();
	}
}
#endif /* CLIPPING */


/*
 *  tty_print_glyph
 *
 *  Print the glyph to the output device.  Don't flush the output device.
 *
 *  Since this is only called from show_glyph(), it is assumed that the
 *  position and glyph are always correct (checked there)!
 */

void
tty_print_glyph(window, x, y, glyph)
    winid window;
    xchar x, y;
    int glyph;
{
    int ch;
    boolean reverse_on = FALSE;
    int	    color;
    unsigned special;
    
	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

#ifdef CLIPPING
    if(clipping) {
#if 0
#ifdef ALLEG_FX
    	if (!iflags.usealleg)
#endif
#endif
	if(x <= clipx || y < clipy || x >= clipxmax || y >= clipymax)
	    return;
    }
#endif
    /* map glyph to character and color */
    mapglyph(glyph, &ch, &color, &special, x, y);

    /* Move the cursor. */
    tty_curs(window, x,y);

#ifndef NO_TERMS
    if (ul_hack && ch == '_') {		/* non-destructive underscore */
	(void) putchar((char) ' ');
	backsp();
    }
#endif

#ifdef TEXTCOLOR
    if (color != ttyDisplay->color) {
	if(ttyDisplay->color != NO_COLOR)
	    term_end_color();
	ttyDisplay->color = color;
	if(color != NO_COLOR)
	    term_start_color(color);
    }
#endif /* TEXTCOLOR */

    /* must be after color check; term_end_color may turn off inverse too */
    if (((special & MG_PET) && iflags.hilite_pet) ||
	((special & MG_DETECT) && iflags.use_inverse)) {
	term_start_attr(ATR_INVERSE);
	reverse_on = TRUE;
    }

    if (!reverse_on && (special & (MG_ENGRAVING)))
		    term_start_bgcolor(CLR_CYAN);

#ifdef TEXTCOLOR 
    if (!reverse_on && (special & (MG_STAIRS|MG_OBJPILE))) { 
	    if ((special & MG_STAIRS)) 
		    term_start_bgcolor(CLR_ORANGE); /* Amy edit - I want the color to be bright and easily recognizable. */
	    else 
		    term_start_bgcolor(CLR_BRIGHT_BLUE); /* Amy edit - dark blue is almost invisible! */
    } 
#endif 

    if (!reverse_on && (special & (MG_EGOTYPE)))
		    term_start_bgcolor(CLR_YELLOW); /* Amy edit - I want the color to be bright and easily recognizable. */

    if (!reverse_on && (special & (MG_PEACEFUL)))
		    term_start_bgcolor(CLR_BRIGHT_GREEN);

    if (!reverse_on && (special & (MG_AVOIDATTACK)))
		    term_start_bgcolor(CLR_RED);

#if defined(USE_TILES) && defined(MSDOS)
    if (iflags.grmode && iflags.tile_view)
      xputg(glyph,ch,special);
    else
#endif
		g_putch(ch);		/* print the character */

    if (reverse_on) {
	term_end_attr(ATR_INVERSE);
#ifdef TEXTCOLOR
	/* turn off color as well, ATR_INVERSE may have done this already */
	if(ttyDisplay->color != NO_COLOR) {
	    term_end_color();
	    ttyDisplay->color = NO_COLOR;
	}
#endif
    }

    if (!reverse_on && (special & (MG_ENGRAVING))) { 
	    term_end_bgcolor(); 
	    term_end_color(); 
	    ttyDisplay->color = NO_COLOR;
    } 

#ifdef TEXTCOLOR 
    if (!reverse_on && (special & (MG_STAIRS|MG_OBJPILE))) { 
	    term_end_bgcolor(); 
	    term_end_color(); 
	    ttyDisplay->color = NO_COLOR;
    } 
#endif 

    if (!reverse_on && (special & (MG_EGOTYPE))) { 
	    term_end_bgcolor(); 
	    term_end_color(); 
	    ttyDisplay->color = NO_COLOR;
    } 

    if (!reverse_on && (special & (MG_PEACEFUL))) { 
	    term_end_bgcolor(); 
	    term_end_color(); 
	    ttyDisplay->color = NO_COLOR;
    } 

    if (!reverse_on && (special & (MG_AVOIDATTACK))) { 
	    term_end_bgcolor(); 
	    term_end_color(); 
	    ttyDisplay->color = NO_COLOR;
    } 

    wins[window]->curx++;	/* one character over */
    ttyDisplay->curx++;		/* the real cursor moved too */
}

void
tty_raw_print(str)
    const char *str;
{
	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    if(ttyDisplay) ttyDisplay->rawprint++;
#if defined(MICRO) || defined(WIN32CON)
    msmsg("%s\n", str);
#else
    puts(str); (void) fflush(stdout);
#endif
}

void
tty_raw_print_bold(str)
    const char *str;
{
	if (program_state.done_hup) {
		morc = '\033';
		return;
	}

    if(ttyDisplay) ttyDisplay->rawprint++;
    term_start_raw_bold();
#if defined(MICRO) || defined(WIN32CON)
    msmsg("%s", str);
#else
    (void) fputs(str, stdout);
#endif
    term_end_raw_bold();
#if defined(MICRO) || defined(WIN32CON)
    msmsg("\n");
#else
    puts("");
    (void) fflush(stdout);
#endif
}

int
tty_nhgetch()
{
    int i;
#ifdef UNIX
    /* kludge alert: Some Unix variants return funny values if getc()
     * is called, interrupted, and then called again.  There
     * is non-reentrant code in the internal _filbuf() routine, called by
     * getc().
     */
    static volatile int nesting = 0;
    char nestbuf;
#endif

    if (program_state.done_hup) {
	return ('\033');
    }

    (void) fflush(stdout);
    /* Note: if raw_print() and wait_synch() get called to report terminal
     * initialization problems, then wins[] and ttyDisplay might not be
     * available yet.  Such problems will probably be fatal before we get
     * here, but validate those pointers just in case...
     */
    if (WIN_MESSAGE != WIN_ERR && wins[WIN_MESSAGE])
	    wins[WIN_MESSAGE]->flags &= ~WIN_STOP;
    if (iflags.debug_fuzzer) {
	i = randomkey();
    } else {
#ifdef UNIX
    i = ((++nesting == 1) ? tgetch() :
	 (read(fileno(stdin), (void *)&nestbuf,1) == 1 ? (int)nestbuf :
								EOF));
    --nesting;
#else
    i = tgetch();
#endif
    } /* fuzzer check */
    if (!i) i = DOESCAPE; /* map NUL to ESC since nethack doesn't expect NUL */
    if (ttyDisplay && ttyDisplay->toplin == 1)
	ttyDisplay->toplin = 2;
    return i;
}

/*
 * return a key, or 0, in which case a mouse button was pressed
 * mouse events should be returned as character postitions in the map window.
 * Since normal tty's don't have mice, just return a key.
 */
/*ARGSUSED*/
int
tty_nh_poskey(x, y, mod)
    int *x, *y, *mod;
{
#if defined(MAC_MPW)
# pragma unused(x,y,mod)
#endif
# if defined(WIN32CON)
    int i;

    if (program_state.done_hup) {
	return ('\033');
    }

    (void) fflush(stdout);
    /* Note: if raw_print() and wait_synch() get called to report terminal
     * initialization problems, then wins[] and ttyDisplay might not be
     * available yet.  Such problems will probably be fatal before we get
     * here, but validate those pointers just in case...
     */
    if (WIN_MESSAGE != WIN_ERR && wins[WIN_MESSAGE])
	    wins[WIN_MESSAGE]->flags &= ~WIN_STOP;
    i = ntposkey(x, y, mod);
    if (!i && mod && *mod == 0)
    	i = DOESCAPE; /* map NUL to ESC since nethack doesn't expect NUL */
    if (ttyDisplay && ttyDisplay->toplin == 1)
		ttyDisplay->toplin = 2;
    return i;
# else
    return tty_nhgetch();
# endif
}

/* Thanks to https://stackoverflow.com/questions/29335758/using-kbhit-and-getch-on-linux and https://web.archive.org/web/20170713065718/www.flipcode.com/archives/_kbhit_for_Linux.shtml */
#ifdef BORG
int tty_kbhit(void) {
	int byteswaiting;
	ioctl(0, FIONREAD, &byteswaiting);
	return byteswaiting;
}
#endif

void
win_tty_init()
{
# if defined(WIN32CON)
    nttty_open();
# endif
    return;
}

#ifdef POSITIONBAR
void
tty_update_positionbar(posbar)
char *posbar;
{
# ifdef MSDOS
	video_update_positionbar(posbar);
# endif
}
#endif

/*
 * Allocate a copy of the given string.  If null, return a string of
 * zero length.
 *
 * This is an exact duplicate of copy_of() in X11/winmenu.c.
 */
static char *
copy_of(s)
    const char *s;
{
    if (!s) s = "";
    return strcpy((char *) alloc((unsigned) (strlen(s) + 1)), s);
}

#endif /* TTY_GRAPHICS */

/*wintty.c*/

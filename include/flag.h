/*	SCCS Id: @(#)flag.h	3.4	2002/08/22	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* If you change the flag structure make sure you increment EDITLEVEL in   */
/* patchlevel.h if needed.  Changing the instance_flags structure does	   */
/* not require incrementing EDITLEVEL.					   */

#ifndef FLAG_H
#define FLAG_H

/*
 * Persistent flags that are saved and restored with the game.
 *
 */

struct flag {

	boolean  alphabetinv;	/* alphabetic sorting in inventory */

#ifdef AMIFLUSH
	boolean  altmeta;	/* use ALT keys as META */
	boolean  amiflush;	/* kill typeahead */
#endif
#ifdef	MFLOPPY
	boolean  asksavedisk;
#endif
	boolean  autodig;       /* MRKR: Automatically dig */
	boolean  autoquiver;	/* Automatically fill quiver */

	boolean  bash_reminder;	/* remind player about bashing with a launcher or non-weapon */
	boolean  etimed_autosave;	/* automatically save state every 100 turns to ward off phantom crash bugs --Amy */
	boolean  xtimed_autosave;	/* automatically save state every 100 turns to ward off phantom crash bugs --Amy */

	boolean  beginner;
#ifdef MAIL
	boolean  biff;		/* enable checking for mail */
#endif
	boolean  botl;		/* partially redo status line */
	boolean  botlx;		/* print an entirely new bottom line */
	boolean  confirm;	/* confirm before hitting tame monsters */
	boolean  debug;		/* in debugging mode */
#define wizard	 flags.debug
	boolean  eatingconfirm;	/* confirm when eating non-comestibles */
	boolean  eatingwalls; /* being a scurrier or similar will eat walls by moving into them */
	boolean  end_own;	/* list all own scores */
	boolean  explore;	/* in exploration mode */
#ifdef OPT_DISPMAP
	boolean  fast_map;	/* use optimized, less flexible map display */
#endif
#define discover flags.explore
	boolean  female;
	boolean  forcefight;
	boolean  friday13;	/* it's Friday the 13th */

	boolean  gmmessage;	/* if on, game master mode will tell you what monster got spawned */

	boolean  graffitihilite;	/* graffiti is highlighted cyan */
	boolean  groundhogday;	/* KMH -- February 2 */
	boolean  help;		/* look in data file for info about stuff */
    boolean  hitpointbar; /* colourful hit point status bar */
	boolean  ignintr;	/* ignore interrupts */
	boolean  inertiaconfirm;	/* controls whether inertia control has y/n or yes/no */
#ifdef INSURANCE
	boolean  ins_chkpt;	/* checkpoint as appropriate */
#endif
	boolean  invlet_constant; /* let objects keep their inventory symbol */
#ifdef SHOW_WEIGHT
	boolean  invweight;    /* show weight in inventory and when picking up */
#endif

/*WAC keep_save option*/
#ifdef KEEP_SAVE
	boolean  keep_savefile; /* Keep Old Save files*/
#endif
	boolean  knapsacklimit;	/* re-activates the 52 item limit */
	boolean  legacy;	/* print game entry "story" */
	boolean  lit_corridor;	/* show a dark corr as lit if it is in sight */
	boolean  made_amulet;
	boolean  materialglyph;	/* always show glyph for item materials in inventory */
	boolean  menu_on_esc;	/* show menu when hitting esc */
	boolean  missing_safety;	/* don't show the player's polymorphed species as a missingno */
	boolean  pokedex;		/* show pokedex (not on public servers) */
	boolean  tech_description;	/* show descriptions when choosing techniques */
	boolean  mon_moving;	/* monsters' turn to move */
	boolean  moreforced;	/* Amy's predefined --More-- prompts */
	boolean  move;
	boolean  mv;
	boolean  bypasses;	/* bypass flag is set on at least one fobj */
	boolean  nap;		/* `timed_delay' option for display effects */
	boolean  nopick;	/* do not pickup objects (as when running) */
	boolean  null;		/* OK to send nulls to the terminal */
#ifdef MAC
	boolean  page_wait;	/* put up a --More-- after a page of messages */
#endif
	boolean  paranoidquit;	/* whether #quit requires a full "yes" confirmation */
	boolean  perm_invent;	/* keep full inventories up until dismissed */
	boolean  pickup;	/* whether you pickup or move and look */
	boolean  pickup_thrown;		/* auto-pickup items you threw */
	boolean  pickup_cursed;		/* auto-pickup cursed items you threw */

	boolean  pushweapon;	/* When wielding, push old weapon into second slot */
	boolean  rest_on_space; /* space means rest */
	boolean  safe_dog;	/* give complete protection to the dog */
	boolean  showexp;	/* show experience points */
	boolean  showscore;	/* show score */
#ifdef SHOW_DMG
	boolean  showdmg;       /* show damage */
#endif
#ifdef SHOW_WEIGHT
	boolean  showweight;    /* show weight on status line */
#endif
	boolean  showsymbiotehp;	/* show symbiote's health on status line */
	boolean  showmc;	/* shows MC on bottom status line */
	boolean  showmovement;	/* shows movement speed on bottom status line */
	boolean  showlongstats;	/* don't abbreviate status on bottom line */
	boolean  showsanity;	/* shows sanity on bottom status line */

	boolean  silent;	/* whether the bell rings or not */
	boolean  simpledescs;	/* make item descriptions more simple so that they take up less space */
	boolean  sortpack;	/* sorted inventory */
	boolean  soundok;	/* ok to tell about sounds heard */
	boolean  sparkle;	/* show "resisting" special FX (Scott Bigham) */
	boolean  standout;	/* use standout for --More-- */

	boolean  tabcursesconfirm;	/* certain prompts in the curses interface require Tab to advance */

	boolean  time;		/* display elapsed 'time' */
	boolean  tombstone;	/* print tombstone */
	boolean  toptenwin;	/* ending list in window instead of stdout */
	boolean  verbose;	/* max battle info */
	boolean  wallglyph;	/* solid rock is drawn as # glyph */
	boolean  prayconfirm;	/* confirm before praying */
	int	 end_top, end_around;	/* describe desired score list */
	unsigned ident;		/* social security number for each monster */
	unsigned moonphase;
	unsigned long suppress_alert;
 
	boolean gehenna; 
	boolean dudley; 
	boolean iwbtg; 
	boolean hippie; 
	boolean blindfox; 
	boolean lostsoul; 
	boolean uberlostsoul; 
	boolean gmmode;
	boolean supergmmode;
	boolean wonderland;
	boolean elmstreet;
	boolean zapem;

	boolean askforalias; /* set this in the options file if you want to be asked what your char's name is */

#define NEW_MOON	0
#define FULL_MOON	4
	unsigned no_of_wizards; /* 0, 1 or 2 (wizard and his shadow) */
	boolean  travel;        /* find way automatically to u.tx,u.ty */
	unsigned run;		/* 0: h (etc), 1: H (etc), 2: fh (etc) */
				/* 3: FH, 4: ff+, 5: ff-, 6: FF+, 7: FF- */
				/* 8: travel */
	unsigned long warntype; /* warn_of_mon monster type M2 */
	int	 warnlevel;
	int	 djinni_count, ghost_count, dao_count, marid_count, wineghost_count, efreeti_count;	/* potion effect tuning */
	int	 pickup_burden;		/* maximum burden before prompt */
	/* KMH, role patch -- Variables used during startup.
	 *
	 * If the user wishes to select a role, race, gender, and/or alignment
	 * during startup, the choices should be recorded here.  This
	 * might be specified through command-line options, environmental
	 * variables, a popup dialog box, menus, etc.
	 *
	 * These values are each an index into an array.  They are not
	 * characters or letters, because that limits us to 26 roles.
	 * They are not booleans, because someday someone may need a neuter
	 * gender.  Negative values are used to indicate that the user
	 * hasn't yet specified that particular value.	If you determine
	 * that the user wants a random choice, then you should set an
	 * appropriate random value; if you just left the negative value,
	 * the user would be asked again!
	 *
	 * These variables are stored here because the u structure is
	 * cleared during character initialization, and because the
	 * flags structure is restored for saved games.  Thus, we can
	 * use the same parameters to build the role entry for both
	 * new and restored games.
	 *
	 * These variables should not be referred to after the character
	 * is initialized or restored (specifically, after role_init()
	 * is called).
	 */
	int    startingrole; /* for recursion trap - "starting" role (below) can change now --Amy */
	int    startingrace; /* ditto for "starting" race */
	int	 initrole;	/* starting role      (index into roles[])   */
	int	 initrace;	/* starting race      (index into races[])   */
	int	 initgend;	/* starting gender    (index into genders[]) */
	int	 initalign;	/* starting alignment (index into aligns[])  */
	int	 randomall;	/* randomly assign everything not specified */
	int	 pantheon;	/* deity selection for priest character */

	int hybridization;	/* number of the player's hybrid races */
	/* this needs to be set outside of you.h, since otherwise u_init would overwrite stuff! */
	boolean hybridangbander;
	boolean hybridaquarian;
	boolean hybridcurser;
	boolean hybridhaxor;
	boolean hybridhomicider;
	boolean hybridsuxxor;
	boolean hybridwarper;
	boolean hybridrandomizer;
	boolean hybridnullrace;
	boolean hybridmazewalker;
	boolean hybridsoviet;
	boolean hybridxrace;
	boolean hybridheretic;
	boolean hybridsokosolver;
	boolean hybridspecialist;
	boolean hybridamerican;
	boolean hybridminimalist;
	boolean hybridnastinator;
	boolean hybridrougelike;
	boolean hybridsegfaulter;
	boolean hybridironman;
	boolean hybridamnesiac;
	boolean hybridproblematic;
	boolean hybridwindinhabitant;
	boolean hybridaggravator;
	boolean hybridevilvariant;
	boolean hybridlevelscaler;
	boolean hybriderosator;
	boolean hybridroommate;
	boolean hybridextravator;
	boolean hybridhallucinator;
	boolean hybridbossrusher;
	boolean hybriddorian;
	boolean hybridtechless;
	boolean hybridblait;
	boolean hybridgrouper;
	boolean hybridscriptor;
	boolean hybridunbalancor;
	boolean hybridbeacher;
	boolean hybridstairseeker;
	boolean hybridmatrayser;
	boolean hybridfeminizer;

	boolean hybridcancel;	/* to make sure players don't get hybrid races if they explicitly don't want any */
	boolean randomhybrids;	/* turn it off if you don't want random hybrid races */

	/* KMH, balance patch */
	int      boot_count; /* boots from fishing pole */
	int      cram_count; /* cram rations from fishing pole */
	char	 inv_order[MAXOCLASSES];
	char	 pickup_types[MAXOCLASSES];
#define NUM_DISCLOSURE_OPTIONS         5
#define DISCLOSE_PROMPT_DEFAULT_YES    'y'
#define DISCLOSE_PROMPT_DEFAULT_NO     'n'
#define DISCLOSE_YES_WITHOUT_PROMPT    '+'
#define DISCLOSE_NO_WITHOUT_PROMPT     '-'
	char     end_disclose[NUM_DISCLOSURE_OPTIONS + 1];  /* disclose various info
							       upon exit */
	char	 menu_style;	/* User interface style setting */
#ifdef AMII_GRAPHICS
	int numcols;
	unsigned short amii_dripens[ 20 ]; /* DrawInfo Pens currently there are 13 in v39 */
	AMII_COLOR_TYPE amii_curmap[ AMII_MAXCOLORS ]; /* colormap */
#endif
};

/*
 * Flags that are set each time the game is started.
 * These are not saved with the game.
 *
 */

struct instance_flags {
	boolean  cbreak;	/* in cbreak mode, rogue format */
#ifdef CURSES_GRAPHICS
    boolean  classic_status;    /* What kind of horizontal statusbar to use */
    boolean  cursesgraphics;    /* Use portable curses extended characters */
#endif
	boolean  debug_fuzzer;	/* fuzz testing */
	boolean  DECgraphics;	/* use DEC VT-xxx extended character set */
	boolean  echo;		/* 1 to echo characters */
	boolean  IBMgraphics;	/* use IBM extended character set */
	unsigned msg_history;	/* hint: # of top lines to save */
	boolean  num_pad;	/* use numbers for movement commands */
	boolean  news;		/* print news */
	boolean  window_inited; /* true if init_nhwindows() completed */
	boolean  vision_inited; /* true if vision is ready */
	boolean  menu_tab_sep;	/* Use tabs to separate option menu fields */
	boolean  menu_requested; /* Flag for overloaded use of 'm' prefix
				  * on some non-move commands */
	boolean numpadmessage; /* because Elronnd is so weird and keeps inputting invalid commands, so we have to
					    * make it possible to disable the message you're getting */
	boolean memorizationknown;	/* for players who don't want messages about memorization skill */
	boolean winggraphics;	/* wing yellow graphics */
	boolean bones;
	boolean use_menu_glyphs;
	uchar num_pad_mode;
	int	menu_headings;	/* ATR for menu headings */
	int	purge_monsters;	/* # of dead monsters still on fmon list */
	int	*opt_booldup;	/* for duplication of boolean opts in config file */
	int	*opt_compdup;	/* for duplication of compound opts in config file */
	uchar	bouldersym;	/* symbol for boulder display */
	boolean	travel1;	/* first travel step */
	coord	travelcc;	/* coordinates for travel_cache */
	boolean  qwertz_layout; /* swap y/z for this key layout */
#ifdef SIMPLE_MAIL
	boolean simplemail;	/* simple mail format $NAME:$MESSAGE */
#endif
#ifdef WIZARD
	boolean  sanity_check;	/* run sanity checks */
	boolean  mon_polycontrol;	/* debug: control monster polymorphs */
#endif
#ifdef TTY_GRAPHICS
	char prevmsg_window;	/* type of old message window to use */
#endif
#if defined(TTY_GRAPHICS) || defined(CURSES_GRAPHICS)
	boolean  extmenu;	/* extended commands use menu interface */
#endif
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	boolean use_status_colors; /* use color in status line; only if wc_color */
#endif
#ifdef MENU_COLOR
        boolean use_menu_color; /* use color in menus; only if wc_color */
#endif
#ifdef MFLOPPY
	boolean  checkspace;	/* check disk space before writing files */
				/* (in iflags to allow restore after moving
				 * to >2GB partition) */
#endif
#ifdef MICRO
	boolean  BIOS;		/* use IBM or ST BIOS calls when appropriate */
#endif
#if defined(MICRO) || defined(WIN32)
	boolean  rawio;		/* whether can use rawio (IOCTL call) */
#endif
#ifdef MAC_GRAPHICS_ENV
	boolean  MACgraphics;	/* use Macintosh extended character set, as
				   as defined in the special font HackFont */
	unsigned  use_stone;		/* use the stone ppats */
#endif
#if defined(MSDOS) || defined(WIN32)
	boolean hassound;	/* has a sound card */
	boolean usesound;	/* use the sound card */
	boolean usepcspeaker;	/* use the pc speaker */
	boolean tile_view;
	boolean over_view;
	boolean traditional_view;
#endif
#ifdef MSDOS
	boolean	hasalleg;	/* has a Allegor compatible adapter  */
	boolean usealleg;	/* use the Allegro library           */
	boolean hasvga;		/* has a vga adapter */
	boolean usevga;		/* use the vga adapter */
	boolean grmode;		/* currently in graphics mode */
#endif
#ifdef LAN_FEATURES
	boolean lan_mail;	/* mail is initialized */
	boolean lan_mail_fetched; /* mail is awaiting display */
#endif
	boolean death_expl;	/* prompt for explore mode at death */
	boolean quiver_fired;   /* quiver with f command */
/*
 * Window capability support.
 */
	boolean wc_color;		/* use color graphics                  */
	boolean wc_hilite_pet;		/* hilight pets                        */
	boolean wc_ascii_map;		/* show map using traditional ascii    */
	boolean wc_tiled_map;		/* show map using tiles                */
	boolean wc_preload_tiles;	/* preload tiles into memory           */
	int	wc_tile_width;		/* tile width                          */
	int	wc_tile_height;		/* tile height                         */
	char	*wc_tile_file;		/* name of tile file;overrides default */
	boolean wc_inverse;		/* use inverse video for some things   */
	int	wc_align_status;	/*  status win at top|bot|right|left   */
	int	wc_align_message;	/* message win at top|bot|right|left   */
	int     wc_vary_msgcount;	/* show more old messages at a time    */
	char    *wc_foregrnd_menu;	/* points to foregrnd color name for menu win   */
	char    *wc_backgrnd_menu;	/* points to backgrnd color name for menu win   */
	char    *wc_foregrnd_message;	/* points to foregrnd color name for msg win    */
	char    *wc_backgrnd_message;	/* points to backgrnd color name for msg win    */
	char    *wc_foregrnd_status;	/* points to foregrnd color name for status win */
	char    *wc_backgrnd_status;	/* points to backgrnd color name for status win */
	char    *wc_foregrnd_text;	/* points to foregrnd color name for text win   */
	char    *wc_backgrnd_text;	/* points to backgrnd color name for text win   */
	char    *wc_font_map;		/* points to font name for the map win */
	char    *wc_font_message;	/* points to font name for message win */
	char    *wc_font_status;	/* points to font name for status win  */
	char    *wc_font_menu;		/* points to font name for menu win    */
	char    *wc_font_text;		/* points to font name for text win    */
	int     wc_fontsiz_map;		/* font size for the map win           */
	int     wc_fontsiz_message;	/* font size for the message window    */
	int     wc_fontsiz_status;	/* font size for the status window     */
	int     wc_fontsiz_menu;	/* font size for the menu window       */
	int     wc_fontsiz_text;	/* font size for text windows          */
	int	wc_scroll_amount;	/* scroll this amount at scroll_margin */
	int	wc_scroll_margin;	/* scroll map when this far from
						the edge */
	int	wc_map_mode;		/* specify map viewing options, mostly
						for backward compatibility */
	int	wc_player_selection;	/* method of choosing character */
	boolean	wc_splash_screen;	/* display an opening splash screen or not */
	boolean	wc_popup_dialog;	/* put queries in pop up dialogs instead of
				   		in the message window */
	boolean wc_eight_bit_input;	/* allow eight bit input               */
	boolean wc_mouse_support;	/* allow mouse support */
	boolean wc2_fullscreen;		/* run fullscreen */
	boolean wc2_softkeyboard;	/* use software keyboard */
	boolean wc2_wraptext;		/* wrap text */
    int     wc2_term_cols;      /* terminal width, in characters */
    int     wc2_term_rows;      /* terminal height, in characters */
    int     wc2_windowborders;  /* display borders on NetHack windows */
    int     wc2_petattr;        /* points to text attributes for pet */
    boolean wc2_guicolor;       /* allow colors in GUI (outside map) */

	boolean  cmdassist;	/* provide detailed assistance for some commands */
	boolean	 obsolete;	/* obsolete options can point at this, it isn't used */
	/* Items which belong in flags, but are here to allow save compatibility */
	boolean  lootabc;	/* use "a/b/c" rather than "o/i/b" when looting */
	boolean  showrace;	/* show hero glyph by race rather than by role */
	boolean  travelcmd;	/* allow travel command */
	boolean  showsym;	/* show object symbols in menus */
	int	 runmode;	/* update screen display during run moves */
	int  pilesize; /* how many items to list automatically */
#ifdef AUTOPICKUP_EXCEPTIONS
	struct autopickup_exception *autopickup_exceptions[2];
#define AP_LEAVE 0
#define AP_GRAB	 1
#endif
#ifdef WIN32CON
#define MAX_ALTKEYHANDLER 25
	char	 altkeyhandler[MAX_ALTKEYHANDLER];
#endif
	uchar	sortloot;
#ifdef REALTIME_ON_BOTL
  boolean  showrealtime; /* show actual elapsed time */
#endif
#ifdef AUTO_OPEN
	boolean  autoopen;	/* open doors by walking into them */
#endif
	boolean should_change_color; /* should curses change the colourscheme? */
};

/*
 * Old deprecated names
 */
#ifdef TTY_GRAPHICS
#define eight_bit_tty wc_eight_bit_input
#endif
#ifdef TEXTCOLOR
#define use_color wc_color
#endif
#define hilite_pet wc_hilite_pet
#define use_inverse wc_inverse
#ifdef MAC_GRAPHICS_ENV
#define large_font obsolete
#endif
#ifdef MAC
#define popup_dialog wc_popup_dialog
#endif
#define preload_tiles wc_preload_tiles

extern NEARDATA struct flag flags;
extern NEARDATA struct instance_flags iflags;

/* runmode options */
#define RUN_TPORT	0	/* don't update display until movement stops */
#define RUN_LEAP	1	/* update display every 7 steps */
#define RUN_STEP	2	/* update display every single step */
#define RUN_CRAWL	3	/* walk w/ extra delay after each update */

#endif /* FLAG_H */

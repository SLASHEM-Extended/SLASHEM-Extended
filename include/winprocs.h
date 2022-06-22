/*	SCCS Id: @(#)winprocs.h 3.4	2003/01/08	*/
/* Copyright (c) David Cohrs, 1992				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef WINPROCS_H
#define WINPROCS_H

struct window_procs {
    const char *name;
    unsigned long wincap;	/* window port capability options supported */
    unsigned long wincap2;	/* additional window port capability options supported */
    void (*win_init_nhwindows)(int *, char **);
    void (*win_player_selection)(void);
    void (*win_askname)(void);
    void (*win_get_nh_event)(void) ;
    void (*win_exit_nhwindows)(const char *);
    void (*win_suspend_nhwindows)(const char *);
    void (*win_resume_nhwindows)(void);
    winid (*win_create_nhwindow)(int);
    void (*win_clear_nhwindow)(winid);
    void (*win_display_nhwindow)(winid, BOOLEAN_P);
    void (*win_destroy_nhwindow)(winid);
    void (*win_curs)(winid,int,int);
    void (*win_putstr)(winid, int, const char *);
#ifdef FILE_AREAS
    void (*win_display_file)(const char *, const char *, BOOLEAN_P);
#else
    void (*win_display_file)(const char *, BOOLEAN_P);
#endif
    void (*win_start_menu)(winid);
    void (*win_add_menu)(winid,int,const ANY_P *,
		CHAR_P,CHAR_P,int,const char *, BOOLEAN_P);
    void (*win_end_menu)(winid, const char *);
    int (*win_select_menu)(winid, int, MENU_ITEM_P **);
    char (*win_message_menu)(CHAR_P,int,const char *);
    void (*win_update_inventory)(void);
    void (*win_mark_synch)(void);
    void (*win_wait_synch)(void);
#ifdef CLIPPING
    void (*win_cliparound)(int, int);
#endif
#ifdef POSITIONBAR
    void (*win_update_positionbar)(char *);
#endif
    void (*win_print_glyph)(winid,XCHAR_P,XCHAR_P,int);
    void (*win_raw_print)(const char *);
    void (*win_raw_print_bold)(const char *);
    int (*win_nhgetch)(void);
    int (*win_nh_poskey)(int *, int *, int *);
    void (*win_nhbell)(void);
    int (*win_doprev_message)(void);
    char (*win_yn_function)(const char *, const char *, CHAR_P);
    void (*win_getlin)(const char *,char *);
    int (*win_get_ext_cmd)(void);
    void (*win_number_pad)(int);
    void (*win_delay_output)(void);
#ifdef CHANGE_COLOR
    void (*win_change_color)(int,long,int);
#ifdef MAC
    void (*win_change_background)(int);
    short (*win_set_font_name)(winid, char *);
#endif
    char * (*win_get_color_string)(void);
#endif

    /* other defs that really should go away (they're tty specific) */
    void (*win_start_screen)(void);
    void (*win_end_screen)(void);

    void (*win_outrip)(winid,int);
    void (*win_preference_update)(const char *);

    int (*win_kbhit)(void); 
};

extern
#ifdef HANGUPHANDLING
    volatile
#endif
    NEARDATA struct window_procs windowprocs;

/*
 * If you wish to only support one window system and not use procedure
 * pointers, add the appropriate #ifdef below.
 */

#define init_nhwindows (*windowprocs.win_init_nhwindows)
#define player_selection (*windowprocs.win_player_selection)
#define askname (*windowprocs.win_askname)
#define get_nh_event (*windowprocs.win_get_nh_event)
#define exit_nhwindows (*windowprocs.win_exit_nhwindows)
#define suspend_nhwindows (*windowprocs.win_suspend_nhwindows)
#define resume_nhwindows (*windowprocs.win_resume_nhwindows)
#define create_nhwindow (*windowprocs.win_create_nhwindow)
#define clear_nhwindow (*windowprocs.win_clear_nhwindow)
#define display_nhwindow (*windowprocs.win_display_nhwindow)
#define destroy_nhwindow (*windowprocs.win_destroy_nhwindow)
#define curs (*windowprocs.win_curs)
#define putstr (*windowprocs.win_putstr)
#define display_file (*windowprocs.win_display_file)
#ifdef FILE_AREAS
#define display_file_area(area,file,complain) display_file(area,file,complain)
#else
#define display_file_area(area,file,complain) display_file(file,complain)
#endif
#define start_menu (*windowprocs.win_start_menu)
#define add_menu (*windowprocs.win_add_menu)
#define end_menu (*windowprocs.win_end_menu)
#define select_menu (*windowprocs.win_select_menu)
#define message_menu (*windowprocs.win_message_menu)
#define update_inventory (*windowprocs.win_update_inventory)
#define mark_synch (*windowprocs.win_mark_synch)
#define wait_synch (*windowprocs.win_wait_synch)
#ifdef CLIPPING
#define cliparound (*windowprocs.win_cliparound)
#endif
#ifdef POSITIONBAR
#define update_positionbar (*windowprocs.win_update_positionbar)
#endif
#define print_glyph (*windowprocs.win_print_glyph)
#define raw_print (*windowprocs.win_raw_print)
#define raw_print_bold (*windowprocs.win_raw_print_bold)
#define nhgetch (*windowprocs.win_nhgetch)
#define nh_poskey (*windowprocs.win_nh_poskey)
#define nhbell (*windowprocs.win_nhbell)
#define nh_doprev_message (*windowprocs.win_doprev_message)
#define getlin (*windowprocs.win_getlin)
#define get_ext_cmd (*windowprocs.win_get_ext_cmd)
#define number_pad (*windowprocs.win_number_pad)
#define delay_output (*windowprocs.win_delay_output)
#ifdef CHANGE_COLOR
#define change_color (*windowprocs.win_change_color)
#ifdef MAC
#define change_background (*windowprocs.win_change_background)
#define set_font_name (*windowprocs.win_set_font_name)
#endif
#define get_color_string (*windowprocs.win_get_color_string)
#endif
#ifdef BORG
#define kbhit (*windowprocs.win_kbhit)
#endif

/* 3.4.2: There is a real yn_function() in the core now, which does
 *        some buffer length validation on the parameters prior to
 *        invoking the window port routine. yn_function() is in cmd.c
 */
/* #define yn_function (*windowprocs.win_yn_function) */

/* other defs that really should go away (they're tty specific) */
#define start_screen (*windowprocs.win_start_screen)
#define end_screen (*windowprocs.win_end_screen)

#define outrip (*windowprocs.win_outrip)
#define preference_update (*windowprocs.win_preference_update)

/*
 * WINCAP
 * Window port preference capability bits.
 * Some day this might be better in its own wincap.h file.
 */
#define WC_COLOR	 0x01L		/* 01 Port can display things in color       */
#define WC_HILITE_PET	 0x02L		/* 02 supports hilite pet                    */
#define WC_ASCII_MAP	 0x04L		/* 03 supports an ascii map                  */
#define WC_TILED_MAP	 0x08L		/* 04 supports a tiled map                   */
#define WC_PRELOAD_TILES 0x10L		/* 05 supports pre-loading tiles             */
#define WC_TILE_WIDTH	 0x20L		/* 06 prefer this width of tile              */
#define WC_TILE_HEIGHT	 0x40L		/* 07 prefer this height of tile             */
#define WC_TILE_FILE	 0x80L		/* 08 alternative tile file name             */
#define WC_INVERSE	 0x100L		/* 09 Port supports inverse video            */
#define WC_ALIGN_MESSAGE 0x200L		/* 10 supports message alignmt top|b|l|r     */
#define WC_ALIGN_STATUS	 0x400L		/* 11 supports status alignmt top|b|l|r      */
#define WC_VARY_MSGCOUNT 0x800L		/* 12 supports varying message window        */
#define WC_FONT_MAP	 0x1000L	/* 13 supports specification of map win font */
#define WC_FONT_MESSAGE	 0x2000L	/* 14 supports specification of msg win font */
#define WC_FONT_STATUS	 0x4000L	/* 15 supports specification of sts win font */
#define WC_FONT_MENU	 0x8000L	/* 16 supports specification of mnu win font */
#define WC_FONT_TEXT	 0x10000L	/* 17 supports specification of txt win font */
#define WC_FONTSIZ_MAP	 0x20000L	/* 18 supports specification of map win font */
#define WC_FONTSIZ_MESSAGE 0x40000L	/* 19 supports specification of msg win font */
#define WC_FONTSIZ_STATUS 0x80000L	/* 20 supports specification of sts win font */
#define WC_FONTSIZ_MENU	 0x100000L	/* 21 supports specification of mnu win font */
#define WC_FONTSIZ_TEXT	 0x200000L	/* 22 supports specification of txt win font */
#define WC_SCROLL_MARGIN 0x400000L	/* 23 supports setting scroll margin for map */
#define WC_SPLASH_SCREEN 0x800000L	/* 24 supports display of splash screen      */
#define WC_POPUP_DIALOG	 0x1000000L	/* 25 supports queries in pop dialogs        */
#define WC_SCROLL_AMOUNT 0x2000000L	/* 26 scroll this amount at scroll margin    */
#define WC_EIGHT_BIT_IN	 0x4000000L	/* 27 8-bit character input                  */
#define WC_PERM_INVENT	 0x8000000L	/* 28 8-bit character input                  */
#define WC_MAP_MODE	 0x10000000L	/* 29 map_mode option                        */
#define WC_WINDOWCOLORS  0x20000000L	/* 30 background color for message window    */
#define WC_PLAYER_SELECTION  0x40000000L /* 31 background color for message window    */
#define WC_MOUSE_SUPPORT 0x80000000L	/* 32 mouse support                          */
					/* no free bits */

#define WC2_FULLSCREEN		0x01L	/* 01 display full screen                    */
#define WC2_SOFTKEYBOARD	0x02L	/* 02 software keyboard                      */
#define WC2_WRAPTEXT		0x04L	/* 03 wrap long lines of text                */
#define WC2_TERM_COLS		0x08L	/* 04 supports setting terminal width        */
#define WC2_TERM_ROWS		0x10L	/* 05 supports setting terminal height       */
#define WC2_WINDOWBORDERS   0x20L	/* 06 display borders for NetHack windows    */
#define WC2_PETATTR         0x40L	/* 07 attributes for highlight_pet */
#define WC2_GUICOLOR        0x80L	/* 08 attributes for highlight_pet */
					/* 25 free bits */

#define ALIGN_LEFT	1
#define ALIGN_RIGHT	2
#define ALIGN_TOP	3
#define ALIGN_BOTTOM	4

/* player_selection */
#define VIA_DIALOG	0
#define VIA_PROMPTS	1

/* map_mode settings - deprecated */
#define MAP_MODE_TILES		0
#define MAP_MODE_ASCII4x6	1
#define MAP_MODE_ASCII6x8	2
#define MAP_MODE_ASCII8x8	3
#define MAP_MODE_ASCII16x8	4
#define MAP_MODE_ASCII7x12	5
#define MAP_MODE_ASCII8x12	6
#define MAP_MODE_ASCII16x12	7
#define MAP_MODE_ASCII12x16	8
#define MAP_MODE_ASCII10x18	9
#define MAP_MODE_ASCII_FIT_TO_SCREEN 10
#define MAP_MODE_TILES_FIT_TO_SCREEN 11

#if 0
#define WC_SND_SOUND	 0x01L		/* 01 Port has some sound capabilities       */
#define WC_SND_SPEAKER	 0x02L		/* 02 Sound supported via built-in speaker   */
#define WC_SND_STEREO	 0x04L		/* 03 Stereo sound supported                 */
#define WC_SND_RAW	 0x08L		/* 04 Raw sound supported                    */
#define WC_SND_WAVE	 0x10L		/* 05 Wave support                           */
#define WC_SND_MIDI	 0x20L		/* 06 Midi support                           */
					/* 26 free bits */
#endif

struct wc_Opt {
	const char *wc_name;
	unsigned long wc_bit;
};

#endif /* WINPROCS_H */

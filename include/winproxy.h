/* $Id: winproxy.h,v 1.15 2003-05-31 08:12:43 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2003 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef WINEXT_H
#define WINEXT_H

#define E extern

#ifdef NHXDR_H
struct proxy_extension {
    const char *name;
    const char *version;
    void (*init)(unsigned short);
    int no_procedures;
    void (*handler)(unsigned short, NhExtXdr *, NhExtXdr *);
};

extern struct proxy_extension proxy_extents[];
#endif

extern struct window_procs proxy_procs;

extern unsigned long proxy_interface_mode;

extern short glyph2proxy[MAX_GLYPH];

extern FILE *proxy_config_fp;

/* external declarations */
E void proxy_init_nhwindows(int *, char **);
E void proxy_player_selection(void);
E void proxy_askname(void);
E void proxy_get_nh_event(void) ;
E void proxy_exit_nhwindows(const char *);
E void proxy_suspend_nhwindows(const char *);
E void proxy_resume_nhwindows(void);
E winid proxy_create_nhwindow(int);
E void proxy_clear_nhwindow(winid);
E void proxy_display_nhwindow(winid, BOOLEAN_P);
E void proxy_dismiss_nhwindow(winid);
E void proxy_destroy_nhwindow(winid);
E void proxy_curs(winid, int, int);
E void proxy_putstr(winid, int, const char *);
#ifdef FILE_AREAS
E void proxy_display_file(const char *, const char *, BOOLEAN_P);
#else
E void proxy_display_file(const char *, BOOLEAN_P);
#endif
E void proxy_start_menu(winid);
E void proxy_add_menu(winid, int, const ANY_P *,
			CHAR_P, CHAR_P, int, const char *, BOOLEAN_P);
E void proxy_end_menu(winid, const char *);
E int proxy_select_menu(winid, int, MENU_ITEM_P **);
E char proxy_message_menu(CHAR_P, int, const char *);
E void proxy_update_inventory(void);
E void proxy_mark_synch(void);
E void proxy_wait_synch(void);
#ifdef CLIPPING
E void proxy_cliparound(int, int);
#endif
#ifdef POSITIONBAR
E void proxy_update_positionbar(char *);
#endif
E void proxy_print_glyph(winid, XCHAR_P, XCHAR_P, int);
E void proxy_raw_print(const char *);
E void proxy_raw_print_bold(const char *);
E int proxy_nhgetch(void);
E int proxy_nh_poskey(int *, int *, int *);
E void proxy_nhbell(void);
E int proxy_doprev_message(void);
E char proxy_yn_function(const char *, const char *, CHAR_P);
E void proxy_getlin(const char *,char *);
E int proxy_get_ext_cmd(void);
E void proxy_number_pad(int);
E void proxy_delay_output(void);
#ifdef CHANGE_COLOR
E void proxy_change_color(int, long, int);
#ifdef MAC
E void proxy_change_background(int);
E short set_proxy_font_name(winid, char *);
#endif
E char * proxy_get_color_string(void);
#endif
E void proxy_start_screen(void);
E void proxy_end_screen(void);
E void proxy_outrip(winid, int);
E void proxy_preference_update(const char *);
E void proxy_status(int, int, const char **);
E FILE *proxy_config_file_open(void);

#undef E

#endif /* WINPROXY_H */

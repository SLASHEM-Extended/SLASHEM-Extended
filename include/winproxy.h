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
    void FDECL((*init), (unsigned short));
    int no_procedures;
    void FDECL((*handler), (unsigned short, NhExtXdr *, NhExtXdr *));
};

extern struct proxy_extension proxy_extents[];
#endif

extern struct window_procs proxy_procs;

extern unsigned long proxy_interface_mode;

extern short glyph2proxy[MAX_GLYPH];

extern FILE *proxy_config_fp;

/* external declarations */
E void FDECL(proxy_init_nhwindows, (int *, char **));
E void NDECL(proxy_player_selection);
E void NDECL(proxy_askname);
E void NDECL(proxy_get_nh_event) ;
E void FDECL(proxy_exit_nhwindows, (const char *));
E void FDECL(proxy_suspend_nhwindows, (const char *));
E void NDECL(proxy_resume_nhwindows);
E winid FDECL(proxy_create_nhwindow, (int));
E void FDECL(proxy_clear_nhwindow, (winid));
E void FDECL(proxy_display_nhwindow, (winid, BOOLEAN_P));
E void FDECL(proxy_dismiss_nhwindow, (winid));
E void FDECL(proxy_destroy_nhwindow, (winid));
E void FDECL(proxy_curs, (winid, int, int));
E void FDECL(proxy_putstr, (winid, int, const char *));
#ifdef FILE_AREAS
E void FDECL(proxy_display_file, (const char *, const char *, BOOLEAN_P));
#else
E void FDECL(proxy_display_file, (const char *, BOOLEAN_P));
#endif
E void FDECL(proxy_start_menu, (winid));
E void FDECL(proxy_add_menu, (winid, int, const ANY_P *,
			CHAR_P, CHAR_P, int, const char *, BOOLEAN_P));
E void FDECL(proxy_end_menu, (winid, const char *));
E int FDECL(proxy_select_menu, (winid, int, MENU_ITEM_P **));
E char FDECL(proxy_message_menu, (CHAR_P, int, const char *));
E void NDECL(proxy_update_inventory);
E void NDECL(proxy_mark_synch);
E void NDECL(proxy_wait_synch);
#ifdef CLIPPING
E void FDECL(proxy_cliparound, (int, int));
#endif
#ifdef POSITIONBAR
E void FDECL(proxy_update_positionbar, (char *));
#endif
E void FDECL(proxy_print_glyph, (winid, XCHAR_P, XCHAR_P, int));
E void FDECL(proxy_raw_print, (const char *));
E void FDECL(proxy_raw_print_bold, (const char *));
E int NDECL(proxy_nhgetch);
E int FDECL(proxy_nh_poskey, (int *, int *, int *));
E void NDECL(proxy_nhbell);
E int NDECL(proxy_doprev_message);
E char FDECL(proxy_yn_function, (const char *, const char *, CHAR_P));
E void FDECL(proxy_getlin, (const char *,char *));
E int NDECL(proxy_get_ext_cmd);
E void FDECL(proxy_number_pad, (int));
E void NDECL(proxy_delay_output);
#ifdef CHANGE_COLOR
E void FDECL(proxy_change_color, (int, long, int));
#ifdef MAC
E void FDECL(proxy_change_background, (int));
E short FDECL(set_proxy_font_name, (winid, char *));
#endif
E char * NDECL(proxy_get_color_string);
#endif
E void NDECL(proxy_start_screen);
E void NDECL(proxy_end_screen);
E void FDECL(proxy_outrip, (winid, int));
E void FDECL(proxy_preference_update, (const char *));
E void FDECL(proxy_status, (int, int, const char **));
E FILE *NDECL(proxy_config_file_open);

#undef E

#endif /* WINPROXY_H */

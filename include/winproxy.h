/* $ Id: $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef WINEXT_H
#define WINEXT_H

#define E extern

extern struct window_procs proxy_procs;

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
E int NDECL(proxy_get_proxy_cmd);
E void FDECL(proxy_number_pad, (int));
E void NDECL(proxy_delay_output);
#ifdef CHANGE_COLOR
E void FDECL(proxy_change_color,(int, long, int));
#ifdef MAC
E void FDECL(proxy_change_background,(int));
E short FDECL(set_proxy_font_name, (winid, char *));
#endif
E char * NDECL(proxy_get_color_string);
#endif
E void NDECL(proxy_start_screen);
E void NDECL(proxy_end_screen);
E void FDECL(proxy_outrip, (winid,int));

#ifdef NHXDR_H
struct nhext_svc {
    unsigned short id;
    void (*handler)(unsigned short id, NhExtXdr *request, NhExtXdr *reply);
};

typedef int (*nhext_io_func)(void *handle, void *buf, unsigned int len);

E int FDECL(nhext_subprotocol1_init,
    (nhext_io_func, void *, nhext_io_func, void *, struct nhext_svc *));
E void NDECL(nhext_subprotocol1_end);
E int VDECL(nhext_rpc_params, (NhExtXdr *xdrs, int, ...));
E int VDECL(nhext_rpc, (unsigned short, ...));
E int FDECL(nhext_svc, (struct nhext_svc *));
#endif	/* NHXDR_H */

#undef E

#define EXT_PARAM_INT		0x4001
#define EXT_PARAM_STRING	0x4002
#define EXT_PARAM_WINID		0x4003
#define EXT_PARAM_BOOLEAN	0x4004
#define EXT_PARAM_CHAR		0x4005
#define EXT_PARAM_XDRF		0x4006

#define EXT_PARAM_PTR		0x8000

#define EXT_INT(i)		EXT_PARAM_INT, i
#define EXT_STRING(s)		EXT_PARAM_STRING, s
#define EXT_WINID(w)		EXT_PARAM_WINID, w
#define EXT_BOOLEAN(b)		EXT_PARAM_BOOLEAN, b
#define EXT_CHAR(c)		EXT_PARAM_CHAR, c
#define EXT_XDRF(func,data)	EXT_PARAM_XDRF, func, data

#define EXT_INT_P(i)		EXT_PARAM_PTR | EXT_PARAM_INT, &(i)
#define EXT_STRING_P(s)		EXT_PARAM_PTR | EXT_PARAM_STRING, &(s)
#define EXT_WINID_P(w)		EXT_PARAM_PTR | EXT_PARAM_WINID, &(w)
#define EXT_BOOLEAN_P(b)	EXT_PARAM_PTR | EXT_PARAM_BOOLEAN, &(b)
#define EXT_CHAR_P(c)		EXT_PARAM_PTR | EXT_PARAM_CHAR, &(c)

#endif /* WINPROXY_H */

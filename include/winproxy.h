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
E void FDECL(proxy_status, (int, int, const char **));

#ifdef NHXDR_H
struct nhext_svc {
    unsigned short id;
    void FDECL((*handler), (unsigned short, NhExtXdr *, NhExtXdr *));
};

typedef int (*nhext_io_func)(void *handle, void *buf, unsigned int len);

E int FDECL(nhext_subprotocol1_init,
    (nhext_io_func, void *, nhext_io_func, void *, struct nhext_svc *));
E void NDECL(nhext_subprotocol1_end);
E void FDECL(nhext_subprotocol1_end_c, (int));
E int VDECL(nhext_rpc_params, (NhExtXdr *xdrs, int, ...));
E int VDECL(nhext_rpc, (unsigned short, ...));
E int VDECL(nhext_rpc_c, (int, unsigned short, ...));
E int FDECL(nhext_svc, (struct nhext_svc *));
E int FDECL(nhext_svc_c, (int, struct nhext_svc *));
#endif	/* NHXDR_H */

E char * FDECL(get_killer_string, (int));

#define EXT_PARAM_INT		0x4001
#define EXT_PARAM_LONG		0x4002
#define EXT_PARAM_STRING	0x4003
#define EXT_PARAM_WINID		0x4004
#define EXT_PARAM_BOOLEAN	0x4005
#define EXT_PARAM_CHAR		0x4006
#define EXT_PARAM_XDRF		0x4007

#define EXT_PARAM_PTR		0x8000

#define EXT_INT(i)		EXT_PARAM_INT, i
#define EXT_LONG(l)		EXT_PARAM_LONG, l
#define EXT_STRING(s)		EXT_PARAM_STRING, (s) ? (s) : ""
#define EXT_WINID(w)		EXT_PARAM_WINID, w
#define EXT_BOOLEAN(b)		EXT_PARAM_BOOLEAN, b
#define EXT_CHAR(c)		EXT_PARAM_CHAR, c
#define EXT_XDRF(func,data)	EXT_PARAM_XDRF, func, data

#define EXT_INT_P(i)		EXT_PARAM_PTR | EXT_PARAM_INT, &(i)
#define EXT_LONG_P(l)		EXT_PARAM_PTR | EXT_PARAM_LONG, &(l)
#define EXT_STRING_P(s)		EXT_PARAM_PTR | EXT_PARAM_STRING, &(s)
#define EXT_WINID_P(w)		EXT_PARAM_PTR | EXT_PARAM_WINID, &(w)
#define EXT_BOOLEAN_P(b)	EXT_PARAM_PTR | EXT_PARAM_BOOLEAN, &(b)
#define EXT_CHAR_P(c)		EXT_PARAM_PTR | EXT_PARAM_CHAR, &(c)

#define EXT_FID_INIT			0x01
#define EXT_FID_INIT_NHWINDOWS		0x02
#define EXT_FID_PLAYER_SELECTION	0x03
#define EXT_FID_ASKNAME			0x04
#define EXT_FID_GET_NH_EVENT		0x05
#define EXT_FID_EXIT_NHWINDOWS		0x06
#define EXT_FID_SUSPEND_NHWINDOWS	0x07
#define EXT_FID_RESUME_NHWINDOWS	0x08
#define EXT_FID_CREATE_NHWINDOW		0x09
#define EXT_FID_CLEAR_NHWINDOW		0x0A
#define EXT_FID_DISPLAY_NHWINDOW	0x0B
#define EXT_FID_DESTROY_NHWINDOW	0x0C
#define EXT_FID_CURS			0x0D
#define EXT_FID_PUTSTR			0x0E
#define EXT_FID_DISPLAY_FILE		0x0F
#define EXT_FID_START_MENU		0x10
#define EXT_FID_ADD_MENU		0x11
#define EXT_FID_END_MENU		0x12
#define EXT_FID_SELECT_MENU		0x13
#define EXT_FID_MESSAGE_MENU		0x14
#define EXT_FID_UPDATE_INVENTORY	0x15
#define EXT_FID_MARK_SYNC		0x16
#define EXT_FID_WAIT_SYNC		0x17
#define EXT_FID_CLIPAROUND		0x18
#define EXT_FID_UPDATE_POSITIONBAR	0x19
#define EXT_FID_PRINT_GLYPH		0x1A
#define EXT_FID_RAW_PRINT		0x1B
#define EXT_FID_RAW_PRINT_BOLD		0x1C
#define EXT_FID_NHGETCH			0x1D
#define EXT_FID_NH_POSKEY		0x1E
#define EXT_FID_NHBELL			0x1F
#define EXT_FID_DOPREV_MESSAGE		0x20
#define EXT_FID_YN_FUNCTION		0x21
#define EXT_FID_GETLIN			0x22
#define EXT_FID_GET_EXT_CMD		0x23
#define EXT_FID_NUMBER_PAD		0x24
#define EXT_FID_DELAY_OUTPUT		0x25
#define EXT_FID_CHANGE_COLOR		0x26
#define EXT_FID_CHANGE_BACKGROUND	0x27
#define EXT_FID_SET_FONT_NAME		0x28
#define EXT_FID_GET_COLOR_STRING	0x29
#define EXT_FID_START_SCREEN		0x2A
#define EXT_FID_END_SCREEN		0x2B
#define EXT_FID_OUTRIP			0x2C
#define EXT_FID_STATUS			0x2D

struct proxy_init_nhwindow_req {
    int argc;
    char **argv;
};

struct proxy_init_nhwindow_res {
    boolean inited;
    int argc;
    char **argv;
};

struct proxy_mi {
    int item;			/* identifier */
    long count;			/* count */
};

struct proxy_select_menu_res {
    int retval;
    int n;
    struct proxy_mi *selected;
};

struct proxy_status_req {
    int reconfig;
    int nv;
    const char **values;
};

#ifdef NHXDR_H
E boolean FDECL(proxy_xdr_init_nhwindow_req,
		(NhExtXdr *, struct proxy_init_nhwindow_req *));
E boolean FDECL(proxy_xdr_init_nhwindow_res,
		(NhExtXdr *, struct proxy_init_nhwindow_res *));
E boolean FDECL(proxy_xdr_proxy_mi, (NhExtXdr *, struct proxy_mi *));
E boolean FDECL(proxy_xdr_select_menu_res, (NhExtXdr *, struct proxy_select_menu_res *));
E boolean FDECL(proxy_xdr_status_req, (NhExtXdr *, struct proxy_status_req *));
#endif	/* NHXDR_H */

struct window_ext_procs {
    const char *name;
    int FDECL((*winext_init_nhwindows), (int *, char **));
    int FDECL((*winext_player_selection), (int *, int *, int *, int *));
    char *NDECL((*winext_askname));
    void NDECL((*winext_get_nh_event)) ;
    void FDECL((*winext_exit_nhwindows), (const char *));
    void FDECL((*winext_suspend_nhwindows), (const char *));
    void NDECL((*winext_resume_nhwindows));
    winid FDECL((*winext_create_nhwindow), (int));
    void FDECL((*winext_clear_nhwindow), (winid));
    void FDECL((*winext_display_nhwindow), (winid, int));
    void FDECL((*winext_destroy_nhwindow), (winid));
    void FDECL((*winext_curs), (winid,int,int));
    void FDECL((*winext_putstr), (winid, int, const char *));
    void FDECL((*winext_display_file), (int));
    void FDECL((*winext_start_menu), (winid));
    void FDECL((*winext_add_menu), (winid,int,int,
		int,int,int,const char *, int));
    void FDECL((*winext_end_menu), (winid, const char *));
    int FDECL((*winext_select_menu), (winid, int, struct proxy_mi **));
    int FDECL((*winext_message_menu), (int,int,const char *));
    void NDECL((*winext_update_inventory));
    void NDECL((*winext_mark_synch));
    void NDECL((*winext_wait_synch));
    void FDECL((*winext_cliparound), (int, int));
    void FDECL((*winext_update_positionbar), (char *));
    void FDECL((*winext_print_glyph), (winid,int,int,int));
    void FDECL((*winext_raw_print), (const char *));
    void FDECL((*winext_raw_print_bold), (const char *));
    int NDECL((*winext_nhgetch));
    int FDECL((*winext_nh_poskey), (int *, int *, int *));
    void NDECL((*winext_nhbell));
    int NDECL((*winext_doprev_message));
    char FDECL((*winext_yn_function), (const char *, const char *, int, int *));
    char *FDECL((*winext_getlin), (const char *));
    int NDECL((*winext_get_ext_cmd));
    void FDECL((*winext_number_pad), (int));
    void NDECL((*winext_delay_output));
    void FDECL((*winext_change_color), (int, long, int));
    void FDECL((*winext_change_background), (int));
    int FDECL((*winext_set_font_name), (winid, char *));
    char *NDECL((*winext_get_color_string));
    void NDECL((*winext_start_screen));
    void NDECL((*winext_end_screen));
    int FDECL((*winext_outrip), (winid, char *));
    void FDECL((*winext_status), (int, int, const char **));
};

#undef E

#endif /* WINPROXY_H */

/* $Id: proxysvc.c,v 1.1 2001-09-18 22:20:21 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "winproxy.h"

static struct window_procs *proxy_svc;

/*
 * The proxy svc module provides a set of service functions (ie., suitable
 * to act as handlers for nhext_svc()). These service functions decode the
 * incoming parameters; call the relevant windowing interface function;
 * and then encode the results.
 *
 * Global functions:
 *	proxy_svc_main(struct window_procs *windowprocs)
 *	proxy_svc_main_quit(void)
 */

static void
proxy_svc_init_nhwindows(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    /* FIXME */
}

static void
proxy_svc_player_selection(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int initrole, initrace, initgend, initalign;
    nhext_rpc_params(request, 4, EXT_INT_P(initrole), EXT_INT_P(initrace),
      EXT_INT_P(initgend), EXT_INT_P(initalign));
    (*proxy_svc.player_selection)();

    nhext_rpc_params(reply,
      5, EXT_INT_P(role), EXT_INT_P(race), EXT_INT_P(gend), EXT_INT_P(align),
         EXT_BOOL_P(quit));
}

static void
proxy_svc_askname(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_ASKNAME, 0, 1, EXT_STRING_P(plname));
}

static void
proxy_svc_get_nh_event(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_NH_EVENT, 0, 0);
}

static void
proxy_svc_exit_nhwindows(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_EXIT_NHWINDOWS, 1, EXT_STRING(str), 0);
}

static void
proxy_svc_suspend_nhwindows(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_SUSPEND_NHWINDOWS, 1, EXT_STRING(str), 0);
}

static void
proxy_svc_resume_nhwindows(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_RESUME_NHWINDOWS, 0, 0);
}

static void
proxy_svc_create_nhwindow(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid id;
    proxy_rpc(EXT_FID_CREATE_NHWINDOWS, 1, EXT_INT(type), 1, EXT_WINID_P(1, &id));
    return id;
}

static void
proxy_svc_clear_nhwindow(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_CLEAR_NHWINDOW, 1, EXT_WINID(window), 0);
}

static void
proxy_svc_display_nhwindow(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_DISPLAY_NHWINDOW,
      2, EXT_WINID(window), EXT_BOOLEAN(blocking), 0);
}

static void
proxy_svc_destroy_nhwindow(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_DESTROY_NHWINDOW, 1, EXT_WINID(window), 0);
    proxy__del_winid(window);
}

static void
proxy_svc_curs(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_CURS, 3, EXT_WINID(window), EXT_INT(x), EXT_INT(y), 0);
}

static void
proxy_svc_putstr(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_PUTSTR,
      3, EXT_WINID(window), EXT_INT(attr), EXT_STRING(sr), 0);
}

static void
proxy_svc_display_file(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int fh;
    /* FIXME [ALI] Complete guess; check with typical display_file(). */
#ifdef FILE_AREAS
    fh = dlb_fopen(farea, fname);
#else
    fh = dlb_fopen(fname);
#endif
    if (!fh) {
	if (complain)  pline("Can't open %s.", fname);
	return;
    }
    proxy_rpc(EXT_FID_DISPLAY_FILE_AREA,
      1, EXT_INT(fh), 0);
    dlb_fclose(fh);
}

static void
proxy_svc_start_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_START_MENU, 1, EXT_WINID(window), 0);
    proxy__del_identifiers(window);
}

static void
proxy_svc_add_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int mapping = proxy__map_identifier(window, identifier);
    proxy_rpc(EXT_FID_ADD_MENU,
      8, EXT_WINID(window), EXT_INT(glpyh), EXT_INT(mapping), EXT_CHAR(ch),
         EXT_CHAR(gch), EXT_INT(attr), EXT_STRING(str),
	 EXT_BOOLEAN(preselected),
      0);
}

static void
proxy_svc_end_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_END_MENU, 2, EXT_WINID(window), EXT_STRING(prompt), 0);
}

struct proxy_select_menu_res {
    int retval;
    int n;
    struct proxy_mi {
	int item;	/* identifier */
	long count;	/* count */
    } *selected;
}

int proxy_xdr_proxy_mi(xdrs, datum)
ExtXdr *xdrs;
struct proxy_mi *datum;
{
    return proxy_xdr_int(xdrs, &datum->item) || proxy_xdr_long(xdrs, &datum->count);
}

int proxy_xdr_select_menu_res(xdr, datum)
ExtXdr *xdr;
struct proxy_select_menu_res *datum;
{
    int retval;
    retval = proxy_xdr_int(xdrs, &datum->retval);
    retval |= proxy_xdr_array(xdrs, (genericptr_t *)&datum->selected, 
      &datum->n, 0xffffffff, sizeof(struct proxy_mi), proxy_xdr_proxy_mi);
    return retval;
}

static void
proxy_svc_select_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int i;
    struct proxy_select_menu_res ret;
    proxy_rpc(EXT_FID_SELECT_MENU,
      2, EXT_WINID(window), EXT_INT(how),
      1, EXT_XDRF(proxy_xdr_select_menu_res, &ret));
    *menu_list = (menu_item *) alloc(ret.n * sizeof(menu_item));
    for(i = 0; i < ret.n; i++) {
	proxy__unmap_identifier(window, ret.selected[i].item,
	  &(*menu_item).selected[i].item);
	(*menu_list)[i].count = ret.selected[i].count;
    }
    i = ret.retval
    xdr_free(proxy_xdr_select_menu_res, &ret);
    return i;
}

static void
proxy_svc_message_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char ret;
    proxy_rpc(EXT_FID_MESSAGE_MENU,
      3, EXT_CHAR(let), EXT_INT(how), EXT_STRING(mesg), 1, EXT_CHAR_P(ret));
    return ret;
}

static void
proxy_svc_update_inventory(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_UPDATE_INVENTORY, 0, 0);
}

static void
proxy_svc_mark_synch(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_MARK_SYNC, 0, 0);
}

static void
proxy_svc_wait_synch(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_WAIT_SYNC, 0, 0);
}

static void
proxy_svc_cliparound(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_PROXY_CLIPAROUND, 2, EXT_INT(x), EXT_INT(y), 0);
}

static void
proxy_svc_update_positionbar(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_UPDATE_POSITIONBAR, 1, EXT_STRING(posbar), 0);
}

static void
proxy_svc_print_glyph(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_PRINT_GLYPH,
      4, EXT_WINID(window), EXT_CHAR(x), EXT_CHAR(y), EXT_INT(glyph), 0);
}

static void
proxy_svc_raw_print(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_RAW_PRINT, 1, EXT_STRING(str), 0);
}

static void
proxy_svc_raw_print_bold(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_RAW_PRINT_BOLD, 1, EXT_STRING(str), 0);
}

static void
proxy_svc_nhgetch(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int ret;
    proxy_rpc(EXT_FID_NHGETCH, 0, 1, EXT_INT_P(ret));
    return ret;
}

static void
proxy_svc_nh_poskey(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int ret, lx, ly, lmod;
    proxy_rpc(EXT_FID_NH_POSKEY, 0,
      4, EXT_INT_P(ret), EXT_INT_P(lx), EXT_INT_P(ly), EXT_INT_P(lmod));
    *x = lx;
    *y = ly;
    *mod = lmod;
    return ret;
}

static void
proxy_svc_nhbell(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_NHBELL, 0, 0);
}

static void
proxy_svc_doprev_message(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int ret;
    proxy_rpc(EXT_FID_DOPREV_MESSAGE, 0, 1, EXT_INT_P(ret));
    return ret;
}

static void
proxy_svc_yn_function(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int ret;
    proxy_rpc(EXT_FID_YN_FUNCTION,
      3, EXT_STRING(query), EXT_STRING(resp), EXT_INT(def),
      2, EXT_INT_P(ret), EXT_INT_P(count));
    return ret;
}

static void
proxy_svc_getlin(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_GETLIN, 1, EXT_STRING(query), 1, EXT_STRING_P(bufp));
}

static void
proxy_svc_get_ext_cmd(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int extcmd;
    proxy_rpc(EXT_FID_GET_EXT_CMD, 0, 1, EXT_INT_P(extcmd));
    return extcmd;
}

static void
proxy_svc_number_pad(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_NUMBER_PAD, 1, EXT_INT(STATE), 0);
}

static void
proxy_svc_delay_output(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_DELAY_OUTPUT, 0, 0);
}

static void
proxy_svc_change_color(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_CHANGE_COLOR,
      3, EXT_INT(color), EXT_LONG(rgb), EXT_INT(reverse), 0);
}

static void
proxy_svc_change_background(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_CHANGE_BACKGROUND, 1, EXT_BOOL(white_or_black), 0);
}

static void
proxy_svc_set_font_name(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int ret;
    proxy_rpc(EXT_FID_SET_FONT_NAME, 2, EXT_WINID(window), EXT_STRING(font),
      1, EXT_INT_P(ret));
    return (short)ret;
}

static void
proxy_svc_get_color_string(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char *ret;
    proxy_rpc(EXT_FID_GET_COLOR_STRING, 0, 1, EXT_STRING_P(ret));
    return ret;
}

static void
proxy_svc_start_screen(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_START_SCREEN, 0, 0);
}

static void
proxy_svc_end_screen(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    proxy_rpc(EXT_FID_END_SCREEN, 0, 0);
}

static void
proxy_svc_outrip(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    boolean handled;
    proxy_rpc(EXT_FID_OUTRIP, 2, EXT_WINID(window), EXT_INT(how),
      1, EXT_BOOL_P(handled));
    if (!handled)
	genl_outrip(window, how);
}

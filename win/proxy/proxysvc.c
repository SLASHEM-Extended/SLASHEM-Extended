/* $Id: proxysvc.c,v 1.7 2002-09-01 21:58:19 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "nhxdr.h"
#include "winproxy.h"
#include "proxysvr.h"
#ifdef WIN32
#include "win32api.h"
#endif

static void NDECL((*proxy_ini));		/* optional (can be 0) */
static struct window_ext_procs *proxy_svc;

/*
 * The proxy svc module provides a set of service functions (ie., suitable
 * to act as handlers for nhext_svc()). These service functions decode the
 * incoming parameters; call the relevant windowing interface ext function;
 * and then encode the results.
 *
 * Global functions:
 *	proxy_svc_set_ext_procs(struct window_ext_procs *windowprocs)
 *	proxy_svc_main(struct window_ext_procs *windowprocs)
 *	proxy_svc_main_quit(void)
 */

int proxy_svc_connection;

/*
 * Warning: This uses a gcc extension. The assumption is that we're going to
 * remove the connection number before release anyway, so it's easier not to
 * add a new parameter to every call to next_rpc().
 */

#define nhext_rpc(id, args...) nhext_rpc_c(proxy_svc_connection, id, args)

static void FDECL(proxy_svc_init, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_init_nhwindows,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_player_selection,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_askname, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_get_nh_event, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_exit_nhwindows,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_suspend_nhwindows,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_resume_nhwindows,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_create_nhwindow,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_clear_nhwindow,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_display_nhwindow,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_destroy_nhwindow,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_curs, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_putstr, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_display_file, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_start_menu, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_add_menu, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_end_menu, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_select_menu, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_message_menu, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_update_inventory,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_mark_synch, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_wait_synch, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_cliparound, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_update_positionbar,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_print_glyph, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_raw_print, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_raw_print_bold,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_nhgetch, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_nh_poskey, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_nhbell, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_doprev_message,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_yn_function, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_getlin, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_get_ext_cmd, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_number_pad, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_delay_output, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_change_color, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_change_background,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_set_font_name, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_get_color_string,
			(unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_start_screen, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_end_screen, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_outrip, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_status, (unsigned short, NhExtXdr *, NhExtXdr *));
static void FDECL(proxy_svc_print_glyph_layered,
			(unsigned short, NhExtXdr *, NhExtXdr *));

static void
proxy_svc_init(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    if (proxy_ini)
	(*proxy_ini)();
}

static void
proxy_svc_init_nhwindows(id, request, reply)
unsigned short id; 
NhExtXdr *request, *reply;
{
    int i, j, retval;
    int argc;
    char **argv;
    struct proxy_init_nhwindow_req req = { 0, (char **)0 };
    struct proxy_init_nhwindow_res res;
    nhext_rpc_params(request, 1, EXT_XDRF(proxy_xdr_init_nhwindow_req, &req));
    res.argc = req.argc;
    res.argv = req.argv;
    res.inited = (*proxy_svc->winext_init_nhwindows)(&res.argc, res.argv);
    nhext_rpc_params(reply, 1, EXT_XDRF(proxy_xdr_init_nhwindow_res, &res));
}

static void
proxy_svc_player_selection(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int role, race, gend, align;
    boolean quit;
    nhext_rpc_params(request, 4, EXT_INT_P(role), EXT_INT_P(race),
      EXT_INT_P(gend), EXT_INT_P(align));
    quit = (boolean)
      (*proxy_svc->winext_player_selection)(&role, &race, &gend, &align);
    nhext_rpc_params(reply,
      5, EXT_INT(role), EXT_INT(race), EXT_INT(gend), EXT_INT(align),
         EXT_BOOLEAN(quit));
}

static void
proxy_svc_askname(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char *plname;
    plname = (*proxy_svc->winext_askname)();
    nhext_rpc_params(reply, 1, EXT_STRING(plname));
    free(plname);
}

static void
proxy_svc_get_nh_event(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    (*proxy_svc->winext_get_nh_event)();
}

static void
proxy_svc_exit_nhwindows(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char *str = (char *)0;
    nhext_rpc_params(request, 1, EXT_STRING_P(str));
    (*proxy_svc->winext_exit_nhwindows)(str);
    free(str);
}

static void
proxy_svc_suspend_nhwindows(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char *str = (char *)0;
    nhext_rpc_params(request, 1, EXT_STRING_P(str));
    (*proxy_svc->winext_exit_nhwindows)(str);
    free(str);
}

static void
proxy_svc_resume_nhwindows(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    (*proxy_svc->winext_resume_nhwindows)();
}

static void
proxy_svc_create_nhwindow(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int type;
    winid window;
    nhext_rpc_params(request, 1, EXT_INT_P(type));
    window = (*proxy_svc->winext_create_nhwindow)(type);
    nhext_rpc_params(reply, 1, EXT_WINID(window));
}

static void
proxy_svc_clear_nhwindow(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    nhext_rpc_params(request, 1, EXT_WINID_P(window));
    (*proxy_svc->winext_clear_nhwindow)(window);
}

static void
proxy_svc_display_nhwindow(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    boolean blocking;
    nhext_rpc_params(request, 2, EXT_WINID_P(window), EXT_BOOLEAN_P(blocking));
    (*proxy_svc->winext_display_nhwindow)(window, blocking);
}

static void
proxy_svc_destroy_nhwindow(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    nhext_rpc_params(request, 1, EXT_WINID_P(window));
    (*proxy_svc->winext_destroy_nhwindow)(window);
}

static void
proxy_svc_curs(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    int x, y;
    nhext_rpc_params(request, 3, EXT_WINID_P(window),
      EXT_INT_P(x), EXT_INT_P(y));
    (*proxy_svc->winext_curs)(window, x, y);
}

static void
proxy_svc_putstr(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    int attr;
    char *str = (char *)0;
    nhext_rpc_params(request,
      3, EXT_WINID_P(window), EXT_INT_P(attr), EXT_STRING_P(str));
    (*proxy_svc->winext_putstr)(window, attr, str);
    free(str);
}

static void
proxy_svc_display_file(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int fh;
    nhext_rpc_params(request, 1, EXT_INT_P(fh));
    (*proxy_svc->winext_display_file)(fh);
}

static void
proxy_svc_start_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    nhext_rpc_params(request, 1, EXT_WINID_P(window));
    (*proxy_svc->winext_start_menu)(window);
}

static void
proxy_svc_add_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    int glyph, identifier, accelerator, groupacc, attr;
    char *str = (char *)0;
    boolean preselected;
    nhext_rpc_params(request,
      8, EXT_WINID_P(window), EXT_INT_P(glyph), EXT_INT_P(identifier),
         EXT_INT_P(accelerator), EXT_INT_P(groupacc), EXT_INT_P(attr),
	 EXT_STRING_P(str), EXT_BOOLEAN_P(preselected));
    (*proxy_svc->winext_add_menu)(window, glyph, identifier, accelerator,
      groupacc, attr, str, preselected);
    free(str);
}

static void
proxy_svc_end_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    char *prompt = (char *)0;
    nhext_rpc_params(request, 2, EXT_WINID_P(window), EXT_STRING_P(prompt));
    (*proxy_svc->winext_end_menu)(window, prompt);
    free(prompt);
}

static void
proxy_svc_select_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int i;
    winid window;
    int how;
    struct proxy_mi *selected;
    struct proxy_select_menu_res ret;
    nhext_rpc_params(request, 2, EXT_WINID_P(window), EXT_INT_P(how));
    ret.retval = (*proxy_svc->winext_select_menu)(window, how, &selected);
    ret.n = ret.retval > 0 && selected ? ret.retval : 0;
    if (ret.n) {
	ret.selected =
	  (struct proxy_mi *)alloc(ret.n * sizeof (struct proxy_mi));
	for(i = 0; i < ret.n; i++) {
	    ret.selected[i].item = selected[i].item;
	    ret.selected[i].count = selected[i].count;
	}
    }
    else
	ret.selected = (struct proxy_mi *)0;
    nhext_rpc_params(reply, 1, EXT_XDRF(proxy_xdr_select_menu_res, &ret));
    if (ret.n)
	free(ret.selected);
}

static void
proxy_svc_message_menu(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int retval;
    int let, how;
    char *mesg = (char *)0;
    nhext_rpc_params(request,
      3, EXT_INT_P(let), EXT_INT_P(how), EXT_STRING_P(mesg));
    retval = (*proxy_svc->winext_message_menu)(let, how, mesg);
    nhext_rpc_params(reply, 1, EXT_INT(retval));
    free(mesg);
}

static void
proxy_svc_update_inventory(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    (*proxy_svc->winext_update_inventory)();
}

static void
proxy_svc_mark_synch(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    (*proxy_svc->winext_mark_synch)();
}

static void
proxy_svc_wait_synch(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    (*proxy_svc->winext_wait_synch)();
}

static void
proxy_svc_cliparound(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int x, y;
    nhext_rpc_params(request, 2, EXT_INT_P(x), EXT_INT_P(y));
    (*proxy_svc->winext_cliparound)(x, y);
}

static void
proxy_svc_update_positionbar(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char *posbar = (char *)0;
    nhext_rpc_params(request, 1, EXT_STRING_P(posbar));
    (*proxy_svc->winext_update_positionbar)(posbar);
    free(posbar);
}

static void
proxy_svc_print_glyph(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    int x, y, glyph;
    nhext_rpc_params(request,
      4, EXT_WINID_P(window), EXT_INT_P(x), EXT_INT_P(y), EXT_INT_P(glyph));
    (*proxy_svc->winext_print_glyph)(window, x, y, glyph);
}

static void
proxy_svc_raw_print(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char *str = (char *)0;
    nhext_rpc_params(request, 1, EXT_STRING_P(str));
    (*proxy_svc->winext_raw_print)(str);
    free(str);
}

static void
proxy_svc_raw_print_bold(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char *str = (char *)0;
    nhext_rpc_params(request, 1, EXT_STRING_P(str));
    (*proxy_svc->winext_raw_print_bold)(str);
    free(str);
}

static void
proxy_svc_nhgetch(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int ret;
    ret = (*proxy_svc->winext_nhgetch)();
    nhext_rpc_params(reply, 1, EXT_INT(ret));
}

static void
proxy_svc_nh_poskey(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int ret, lx, ly, lmod;
    ret = (*proxy_svc->winext_nh_poskey)(&lx, &ly, &lmod);
    nhext_rpc_params(reply,
      4, EXT_INT(ret), EXT_INT(lx), EXT_INT(ly), EXT_INT(lmod));
}

static void
proxy_svc_nhbell(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    (*proxy_svc->winext_nhbell)();
}

static void
proxy_svc_doprev_message(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int ret;
    ret = (*proxy_svc->winext_doprev_message)();
    nhext_rpc_params(request, 1, EXT_INT(ret));
}

static void
proxy_svc_yn_function(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int retval, count, def;
    char *ques = (char *)0, *choices = (char *)0;
    nhext_rpc_params(request,
      3, EXT_STRING_P(ques), EXT_STRING_P(choices), EXT_INT_P(def));
    retval = (*proxy_svc->winext_yn_function)(ques, choices, def, &count);
    nhext_rpc_params(reply, 2, EXT_INT(retval), EXT_INT(count));
    free(ques);
    free(choices);
}

static void
proxy_svc_getlin(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char *ques = (char *)0, *input;
    nhext_rpc_params(request, 1, EXT_STRING_P(ques));
    input = (*proxy_svc->winext_getlin)(ques);
    nhext_rpc_params(reply, 1, EXT_STRING(input));
    free(ques);
    free(input);
}

static void
proxy_svc_get_ext_cmd(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int extcmd;
    extcmd = (*proxy_svc->winext_get_ext_cmd)();
    nhext_rpc_params(reply, 1, EXT_INT(extcmd));
}

static void
proxy_svc_number_pad(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int state;
    nhext_rpc_params(request, 1, EXT_INT_P(state));
    (*proxy_svc->winext_number_pad)(state);
}

static void
proxy_svc_delay_output(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    (*proxy_svc->winext_delay_output)();
}

static void
proxy_svc_change_color(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    int color;
    long rgb;
    boolean reverse;
    nhext_rpc_params(request,
      3, EXT_INT_P(color), EXT_LONG_P(rgb), EXT_BOOLEAN_P(reverse));
    (*proxy_svc->winext_change_color)(color, rgb, reverse);
}

static void
proxy_svc_change_background(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    boolean white_or_black;
    nhext_rpc_params(request, 1, EXT_BOOLEAN_P(white_or_black));
    (*proxy_svc->winext_change_background)(white_or_black);
}

static void
proxy_svc_set_font_name(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    char *font = (char *)0;
    int ret;
    nhext_rpc_params(request, 2, EXT_WINID_P(window), EXT_STRING_P(font));
    ret = (*proxy_svc->winext_set_font_name)(window, font);
    nhext_rpc_params(reply, 1, EXT_INT(ret));
    free(font);
}

static void
proxy_svc_get_color_string(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    char *ret;
    ret = (*proxy_svc->winext_get_color_string)();
    nhext_rpc_params(reply, 1, EXT_STRING(ret));
    free(ret);
}

static void
proxy_svc_start_screen(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    (*proxy_svc->winext_start_screen)();
}

static void
proxy_svc_end_screen(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    (*proxy_svc->winext_end_screen)();
}

static void
proxy_svc_outrip(id, request, reply)
unsigned short id;
NhExtXdr *request, *reply;
{
    winid window;
    char *killed_by = (char *)0;
    boolean handled;
    nhext_rpc_params(request, 2, EXT_WINID_P(window), EXT_STRING_P(killed_by));
    handled = (boolean)(*proxy_svc->winext_outrip)(window, killed_by);
    nhext_rpc_params(reply, 1, EXT_BOOLEAN(handled));
    free(killed_by);
}

static void
proxy_svc_status(id, request, reply)
unsigned short id; 
NhExtXdr *request, *reply;
{
    struct proxy_status_req req = { 0, 0, (const char **)0 };
    nhext_rpc_params(request, 1, EXT_XDRF(proxy_xdr_status_req, &req));
    (*proxy_svc->winext_status)(req.reconfig, req.nv, req.values);
}

static void
proxy_svc_print_glyph_layered(id, request, reply)
unsigned short id; 
NhExtXdr *request, *reply;
{
    struct proxy_print_glyph_layered_req req = { 0, 0, 0, 0, (int *)0 };
    nhext_rpc_params(request,
      1, EXT_XDRF(proxy_xdr_print_glyph_layered_req, &req));
    (*proxy_svc->winext_print_glyph_layered)(req.window, req.x, req.y,
      req.ng, req.glyphs);
    free(req.glyphs);
}

static struct nhext_svc services[] = {
    EXT_FID_INIT,			proxy_svc_init,
    EXT_FID_INIT_NHWINDOWS,		proxy_svc_init_nhwindows,
    EXT_FID_PLAYER_SELECTION,		proxy_svc_player_selection,
    EXT_FID_ASKNAME,			proxy_svc_askname,
    EXT_FID_GET_NH_EVENT,		proxy_svc_get_nh_event,
    EXT_FID_EXIT_NHWINDOWS,		proxy_svc_exit_nhwindows,
    EXT_FID_SUSPEND_NHWINDOWS,		proxy_svc_suspend_nhwindows,
    EXT_FID_RESUME_NHWINDOWS,		proxy_svc_resume_nhwindows,
    EXT_FID_CREATE_NHWINDOW,		proxy_svc_create_nhwindow,
    EXT_FID_CLEAR_NHWINDOW,		proxy_svc_clear_nhwindow,
    EXT_FID_DISPLAY_NHWINDOW,		proxy_svc_display_nhwindow,
    EXT_FID_DESTROY_NHWINDOW,		proxy_svc_destroy_nhwindow,
    EXT_FID_CURS,			proxy_svc_curs,
    EXT_FID_PUTSTR,			proxy_svc_putstr,
    EXT_FID_DISPLAY_FILE,		proxy_svc_display_file,
    EXT_FID_START_MENU,			proxy_svc_start_menu,
    EXT_FID_ADD_MENU,			proxy_svc_add_menu,
    EXT_FID_END_MENU,			proxy_svc_end_menu,
    EXT_FID_SELECT_MENU,		proxy_svc_select_menu,
    EXT_FID_MESSAGE_MENU,		proxy_svc_message_menu,
    EXT_FID_UPDATE_INVENTORY,		proxy_svc_update_inventory,
    EXT_FID_MARK_SYNC,			proxy_svc_mark_synch,
    EXT_FID_WAIT_SYNC,			proxy_svc_wait_synch,
    EXT_FID_CLIPAROUND,			proxy_svc_cliparound,
    EXT_FID_UPDATE_POSITIONBAR,		proxy_svc_update_positionbar,
    EXT_FID_PRINT_GLYPH,		proxy_svc_print_glyph,
    EXT_FID_RAW_PRINT,			proxy_svc_raw_print,
    EXT_FID_RAW_PRINT_BOLD,		proxy_svc_raw_print_bold,
    EXT_FID_NHGETCH,			proxy_svc_nhgetch,
    EXT_FID_NH_POSKEY,			proxy_svc_nh_poskey,
    EXT_FID_NHBELL,			proxy_svc_nhbell,
    EXT_FID_DOPREV_MESSAGE,		proxy_svc_doprev_message,
    EXT_FID_YN_FUNCTION,		proxy_svc_yn_function,
    EXT_FID_GETLIN,			proxy_svc_getlin,
    EXT_FID_GET_EXT_CMD,		proxy_svc_get_ext_cmd,
    EXT_FID_NUMBER_PAD,			proxy_svc_number_pad,
    EXT_FID_DELAY_OUTPUT,		proxy_svc_delay_output,
    EXT_FID_CHANGE_COLOR,		proxy_svc_change_color,
    EXT_FID_CHANGE_BACKGROUND,		proxy_svc_change_background,
    EXT_FID_SET_FONT_NAME,		proxy_svc_set_font_name,
    EXT_FID_GET_COLOR_STRING,		proxy_svc_get_color_string,
    EXT_FID_START_SCREEN,		proxy_svc_start_screen,
    EXT_FID_END_SCREEN,			proxy_svc_end_screen,
    EXT_FID_OUTRIP,			proxy_svc_outrip,
    EXT_FID_STATUS,			proxy_svc_status,
    EXT_FID_PRINT_GLYPH_LAYERED,	proxy_svc_print_glyph_layered,
    0,					NULL,
};

void
proxy_svc_set_ext_procs(ini_routine, windowprocs)
void NDECL((*ini_routine));
struct window_ext_procs *windowprocs;
{
    proxy_ini = ini_routine;
    proxy_svc = windowprocs;
}

extern unsigned long proxy_unread, proxy_svc_unread;

#ifdef WIN32
static int
server_read(void *handle, void *buf, unsigned int len)
{
    DWORD nb;
#ifdef PROXY_INTERNAL
    /*
     * Hack for single process operation - let the game handle the pending
     * callback.
     */
    if (!proxy_svc_unread)
	win_proxy_iteration();
#endif
    if (!ReadFile((HANDLE)handle, buf, len, &nb, NULL))
	return -1;
    else {
#ifdef PROXY_INTERNAL
	proxy_svc_unread -= nb;
#endif
	return nb;
    }
}

static int
server_write(void *handle, void *buf, unsigned int len)
{
    DWORD nb;
    if (!WriteFile((HANDLE)handle, buf, len, &nb, NULL))
	return -1;
    else {
#ifdef PROXY_INTERNAL
	proxy_unread += nb;
#endif
	return nb;
    }
}
#else	/* WIN32 */
static int
server_read(void *handle, void *buf, unsigned int len)
{
    int nb;
#ifdef PROXY_INTERNAL
    /*
     * Hack for single process operation - let the game handle the pending
     * callback.
     */
    if (!proxy_svc_unread)
	win_proxy_iteration();
#endif
    nb = read((int)handle, buf, len);
#ifdef PROXY_INTERNAL
    if (nb > 0)
	proxy_svc_unread -= nb;
#endif
    return nb;
}

static int
server_write(void *handle, void *buf, unsigned int len)
{
    int nb;
    nb = write((int)handle, buf, len);
#ifdef PROXY_INTERNAL
    if (nb > 0)
	proxy_unread += nb;
#endif
    return nb;
}
#endif	/* WIN32 */

extern void NDECL(win_GTK_init);
extern struct window_ext_procs GTK_ext_procs;

int
win_proxy_svr_init(read_h, write_h)
int read_h, write_h;
{
    proxy_svc_connection = nhext_subprotocol1_init(server_read, (void *)read_h,
      server_write, (void *)write_h, services);
    if (proxy_svc_connection < 0) {
	fprintf(stderr, "proxy_svc: Failed to initialize sub-protocol1.\n");
	return FALSE;
    }
    /*
     * For now, we hardcode the client side to the GTK interface
     */
    proxy_svc_set_ext_procs(win_GTK_init, &GTK_ext_procs);
    return TRUE;
}

int
win_proxy_svr_iteration()
{
    int i;
    i = nhext_svc_c(proxy_svc_connection, services);
    if (!i)
	fprintf(stderr, "proxy_svc: Ignoring packet with zero ID\n");
    return i;
}

/* $ Id: $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "winproxy.h"

/* Interface definition for plug-in windowing ports */
struct window_procs proxy_procs = {
    "proxy",
    proxy_init_nhwindows,
    proxy_player_selection,
    proxy_askname,
    proxy_get_nh_event,
    proxy_exit_nhwindows,
    proxy_suspend_nhwindows,
    proxy_resume_nhwindows,
    proxy_create_nhwindow,
    proxy_clear_nhwindow,
    proxy_display_nhwindow,
    proxy_destroy_nhwindow,
    proxy_curs,
    proxy_putstr,
    proxy_display_file,
    proxy_start_menu,
    proxy_add_menu,
    proxy_end_menu,
    proxy_select_menu,
    proxy_message_menu,
    proxy_update_inventory,
    proxy_mark_synch,
    proxy_wait_synch,
#ifdef CLIPPING
    proxy_cliparound,
#endif
#ifdef POSITIONBAR
    proxy_update_positionbar,
#endif
    proxy_print_glyph,
    proxy_raw_print,
    proxy_raw_print_bold,
    proxy_nhgetch,
    proxy_nh_poskey,
    proxy_nhbell,
    proxy_doprev_message,
    proxy_yn_function,
    proxy_getlin,
    proxy_get_proxy_cmd,
    proxy_number_pad,
    proxy_delay_output,
#ifdef CHANGE_COLOR
    proxy_change_color,
#ifdef MAC
    proxy_change_background,
    proxy_set_font_name,
#endif
    proxy_get_color_string,
#endif
    proxy_start_screen,
    proxy_end_screen,
    proxy_outrip
};

/*
 * The proxy module tracks the plug-in module using the following routines:
 */

struct proxy__window {
    winid id;
    int no_identifiers;
    anything *identifiers;
};
static int proxy__no_windows;
static struct proxy__window *proxy__windows;

/*
 * Get the index into proxy__windows for window id, adding an
 * entry if necessary.
 */

static int proxy__add_winid(id)
winid id;
{
    int i;
    for(i = 0; i < proxy__no_windows; i++)
	if (proxy__windows[i].id == id)
	    return i;
    for(i = 0; i < proxy__no_windows; i++)
	if (proxy__windows[i].id == WIN_ERR)
	    break;
    if (i == proxy__no_windows) {
	if (proxy__no_windows++)
	    proxy__windows = (struct proxy__window *)realloc(proxy__windows, 
	      proxy__no_windows * sizeof(*proxy__windows));
	else
	    proxy__windows = (struct proxy__window *)malloc(sizeof(*proxy__windows));
	if (!proxy__windows)
	    panic("proxy: can't get %d bytes",
	      proxy__no_windows * sizeof(*proxy__windows));
    }
    proxy__windows[i].id = id;
    proxy__windows[i].no_identifiers = 0;
    proxy__windows[i].identifiers = (anything *)0;
    return i;
}

/*
 * Scrap any entry in proxy__windows for window id.
 */

static void proxy__del_winid(id)
winid id;
{
    int i;
    for(i = 0; i < proxy__no_windows; i++)
	if (proxy__windows[i].id == id) {
	    if (proxy__windows[i].no_identifiers)
		free(proxy__windows[i].identifiers);
	    proxy__windows[i].id = WIN_ERR;
	    break;
	}
}

/*
 * Map identifier into an integer which can be used to unmap back to
 * the same identifier later. Zero is treated specially and will always
 * map to zero (and non-zero identifiers will never map to zero).
 */

static int proxy__map_identifier(id, identifier)
winid id;
const anything *identifier;
{
    int i, j;
    if (identifier->a.void == 0)
	return 0;
    i = proxy__add_winid(id);
    if (j = proxy__windows[i].no_identifiers++)
	proxy__windows[i].identifiers =
	  (anything *)realloc(proxy__windows[i].identifiers, 
	  proxy__windows[i].no_identifiers * sizeof(*proxy__windows->identifiers));
    else
	proxy__windows[i].identifiers =
	  (anything *)malloc(sizeof(*proxy__windows->identifiers));
    if (!proxy__windows[i].identifiers)
	panic("proxy: can't get %d bytes",
	  proxy__windows[i].no_identifiers * sizeof(*proxy__windows->identifiers));
    proxy__windows[i].identifiers[j] = *identifier;
    return j + 1;
}

/*
 * Retrieve the identifier from the mapping.
 */

static void proxy__unmap_identifier(id, mapping, identifier)
winid id;
int mapping;
anything *identifier;
{
    int i;
    if (mapping == 0) {
	identifier->a.void = 0;
	return;
    }
    for(i = 0; i < proxy__no_windows; i++)
	if (proxy__windows[i].id == id)
	    break;
    if (i == proxy__no_windows)
    {
	impossible("Ext: Trying to unmap on an unopened window?");
	identifier->a.void = 0;
	return;
    }
    if (mapping < 1 || mapping > proxy__windows[i].no_identifiers)
    {
	impossible("Bad identifier returned from plug-in.");
	identifier->a.void = 0;
	return;
    }
    *identifier = proxy__windows[i].identifiers[mapping - 1];
}

/*
 * Discard all previously mapped identifiers for window.
 */

static void proxy__del_identifiers(id)
winid id;
{
    for(i = 0; i < proxy__no_windows; i++)
	if (proxy__windows[i].id == id) {
	    if (proxy__windows[i].no_identifiers)
		free(proxy__windows[i].identifiers);
	    proxy__windows[i].identifiers = (anything *)0;
	    proxy__windows[i].no_identifiers = 0;
	    break;
	}
}

/*
 * The glue functions.
 */

/*
 * Call packet:
 *	ushort ID
 *	int argc
 *	string argv[argc]
 * Reply packet:
 *	ushort ID
 *	boolean inited
 *	int argc
 *	string argv[argc]
 */

void
proxy_init_nhwindows(argcp, argv)
int *argcp;
char **argv;
{
    boolean r_inited;
    int r_argc;
    char **r_argv;
    proxy_rpc(EXT_FID_INIT_NHWINDOWS, 2, EXT_INT(*argcp), EXT_ *** );
}

void
proxy_player_selection()
{
    proxy_rpc(EXT_FID_PLAYER_SELECTION, 0, 0);
}

void
proxy_askname()
{
    proxy_rpc(EXT_FID_ASKNAME, 0, 0);
}

void
proxy_get_nh_event()
{
    proxy_rpc(EXT_FID_NH_EVENT, 0, 0);
}

void
proxy_exit_nhwindows(str)
const char *str;
{
    proxy_rpc(EXT_FID_EXIT_NHWINDOWS, 1, EXT_STRING(str), 0);
}

void
proxy_suspend_nhwindows(str)
const char *str;
{
    proxy_rpc(EXT_FID_SUSPEND_NHWINDOWS, 1, EXT_STRING(str), 0);
}

void
proxy_resume_nhwindows()
{
    proxy_rpc(EXT_FID_RESUME_NHWINDOWS, 0, 0);
}

winid
proxy_create_nhwindow(type)
int type;
{
    winid id;
    proxy_rpc(EXT_FID_CREATE_NHWINDOWS, 1, EXT_INT(type), 1, EXT_WINID_P(1, &id));
    return id;
}

void
proxy_clear_nhwindow(window)
winid window;
{
    proxy_rpc(EXT_FID_CLEAR_NHWINDOW, 1, EXT_WINID(window), 0);
}

void
proxy_display_nhwindow(window, blocking)
winid window;
boolean blocking;
{
    proxy_rpc(EXT_FID_DISPLAY_NHWINDOW,
      2, EXT_WINID(window), EXT_BOOLEAN(blocking), 0);
}

void
proxy_destroy_nhwindow(window)
winid window;
{
    proxy_rpc(EXT_FID_DESTROY_NHWINDOW, 1, EXT_WINID(window), 0);
    proxy__del_winid(window);
}

void
proxy_curs(window, x, y)
winid window;
int x, y;
{
    proxy_rpc(EXT_FID_CURS, 3, EXT_WINID(window), EXT_INT(x), EXT_INT(y), 0);
}

void
proxy_putstr(window, attr, str)
winid window;
int attr;
const char *str;
{
    proxy_rpc(EXT_FID_PUTSTR,
      3, EXT_WINID(window), EXT_INT(attr), EXT_STRING(sr), 0);
}

void
#ifdef FILE_AREAS
proxy_display_file(farea, fname, complain)
const char *farea;
const char *fname;
boolean complain;
{
    proxy_rpc(EXT_FID_DISPLAY_FILE_AREA,
      3, EXT_STRING(farea), EXT_STRING(fname), EXT_BOOLEAN(complain), 0);
}
#else
proxy_display_file(fname, complain)
const char *fname;
boolean complain;
{
    proxy_rpc(EXT_FID_DISPLAY_FILE,
      2, EXT_STRING(fname), EXT_BOOLEAN(complain), 0);
}
#endif

/*
 * [ALI] It would be far too much work to pass generic identifiers to
 * plug-in window ports (and also pointless). Instead we substitute
 * integers. We keep a record of the mapping so that we can reverse
 * this in the reply.
 * Note: zero is a special value and must be preserved. Window ports
 * should treat all other values as opaque.
 */

void
proxy_start_menu(window)
winid window;
{
    proxy_rpc(EXT_FID_START_MENU, 1, EXT_WINID(window), 0);
    proxy__del_identifiers(window);
}

void
proxy_add_menu(window, glyph, identifier, ch, gch, attr, str, preselected)
winid window;
int glyph;
const anything *identifier;
char ch, gch;
int attr;
const char *str;
boolean preselected;
{
    int mapping = proxy__map_identifier(window, identifier);
    proxy_rpc(EXT_FID_ADD_MENU,
      8, EXT_WINID(window), EXT_INT(glpyh), EXT_INT(mapping), EXT_CHAR(ch),
         EXT_CHAR(gch), EXT_INT(attr), EXT_STRING(str),
	 EXT_BOOLEAN(preselected),
      0);
}

void
proxy_end_menu(window, prompt)
winid window;
const char *prompt;
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

int
proxy_select_menu(window, how, menu_list)
winid window;
int how;
menu_item **menu_list;
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

char
proxy_message_menu(let, how, mesg)
char let;
int how;
const char *mesg;
{
    proxy_rpc(EXT_FID_MESSAGE_MENU,
      3, EXT_CHAR(let), EXT_INT(how), EXT_STRING(mesg), 0);
}

void
proxy_update_inventory()
{
}

void
proxy_mark_synch()
{
}

void
proxy_wait_synch()
{
}

#ifdef CLIPPING
void
proxy_cliparound(x, y)
int x, y;
{
}
#endif

#ifdef POSITIONBAR
void
proxy_update_positionbar(posbar)
char *posbar;
{
}
#endif

void
proxy_print_glyph(window, x, y, glyph)
winid window;
xchar x, y;
int glyph;
{
}

void
proxy_raw_print(str)
const char *str;
{
}

void
proxy_raw_print_bold(str)
const char *str;
{
}

int
proxy_nhgetch()
{
}

int
proxy_nh_poskey(x, y, mod)
int *x, *y, *mod;
{
}

void
proxy_nhbell()
{
}

int
proxy_doprev_message()
{
}

char
proxy_yn_function(query, resp, def)
const char *query, *resp;
char def;
{
}

void
proxy_getlin(query, bufp)
const char *query;
char *bufp;
{
}

int
proxy_get_proxy_cmd()
{
}

void
proxy_number_pad(state)
int state;
{
}

void
proxy_delay_output()
{
}

#ifdef CHANGE_COLOR
void
proxy_change_color(color, rgb, reverse)
int color;
long rgb;
int reverse;
{
}

#ifdef MAC
void
proxy_change_background(white_or_black)
int white_or_black;
{
}

short
proxy_set_font_name(window, font)
winid window;
char *font;
{
}
#endif	/* MAC */

char *
proxy_get_color_string()
{
}
#endif	/* CHANGE_COLOR */

void
proxy_start_screen()
{
}

void
proxy_end_screen()
{
}

void
proxy_outrip(window, how)
winid window;
int how;
{
}

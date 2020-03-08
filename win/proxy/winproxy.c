/* $Id: winproxy.c,v 1.34.2.1 2004-10-30 08:38:31 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2004 */
/* NetHack may be freely redistributed.  See license for details. */

/* #define DEBUG */

#include <sys/time.h>
#ifdef DEBUG
#include <ctype.h>
#endif
#include "hack.h"
#include "patchlevel.h"
#include "date.h"
#include "nhxdr.h"
#include "proxycom.h"
#include "winproxy.h"
#ifdef WIN32
#include "win32api.h"
#endif

#ifdef DISPLAY_LAYERS
#define MAX_LAYERS	5
#else
#define MAX_LAYERS	1
#endif

static void proxy_flush_layers();
static void proxy_print_glyph_layered();

static int proxy_protocol = 0;

static int proxy_no_mapwins = 0;

static int no_layers = MAX_LAYERS;

static struct proxy_mapwin {
    int id;
    int c_rows, c_cols, c_layers;	/* Configured dimensions */
#ifdef DISPLAY_LAYERS
    struct gbuf_layer {
	char start, stop;
	struct {
	    char start, stop;
	    int glyphs[COLNO];
	} rows[ROWNO];
    } gbuf_layers[MAX_LAYERS];
#endif
} *proxy_mapwins;

/* Window to redirect raw output to, if not WIN_ERR */

int proxy_rawprint_win = WIN_ERR;

/* Current interface mode */

unsigned long proxy_interface_mode = 0;

/* Flag to advise raw print functions not to attempt to use proxy */

static int in_proxy_init = 0;

/* Flag to indicate that the remote interface is authorized */

int proxy_authorized = 0;

/* Interface definition for plug-in windowing ports */
struct window_procs proxy_procs = {
    "proxy",
    0x0,
    0L,
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
    proxy_get_ext_cmd,
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
    proxy_outrip,
    proxy_preference_update,
};

/* Extensions to the NhExt protocol */
struct proxy_extension proxy_extents[] = {
    { 0, 0, 0, 0, 0 }		/* must be last */
};

/*
 * The glue functions.
 */

void
proxy_init_nhwindows(argcp, argv)
int *argcp;
char **argv;
{
    int i, j, retval;
    struct proxy_init_nhwindow_req req;
    struct proxy_init_nhwindow_res res = {0, 0, 0, 0, 0};
    extern struct wc_Opt wc_options[];
    req.argc = *argcp;
    req.argv = argv;
    retval = nhext_rpc(EXT_FID_INIT_NHWINDOWS,
      1, EXT_XDRF(proxy_xdr_init_nhwindow_req, &req),
      1, EXT_XDRF(proxy_xdr_init_nhwindow_res, &res));
    if (retval)
    {
	if (res.argc > *argcp)
	    panic("Proxy: Bad argument list from init_nhwindows");
	for(i = 0; i < res.argc; i++)
	{
	    for(j = i; j < *argcp; j++)
		if (!strcmp(res.argv[i], argv[j]))
		{
		    argv[i] = argv[j];
		    break;
		}
	    if (j == *argcp)
		panic("Proxy: Bad argument from init_nhwindows");
	}
	*argcp = res.argc;
	if (res.inited)
	    iflags.window_inited = TRUE;
	for(i = 0; i < res.capc; i++)
	    for(j = 0; wc_options[j].wc_name; j++)
		if (!strcmp(wc_options[j].wc_name, res.capv[i]))
		    windowprocs.wincap |= wc_options[j].wc_bit;
    }
}

void
proxy_player_selection()
{
    int role, race, gend, alignm;
    nhext_xdr_bool_t quit;
    if (!nhext_rpc(EXT_FID_PLAYER_SELECTION,
      4, EXT_INT(flags.initrole), EXT_INT(flags.initrace),
         EXT_INT(flags.initgend), EXT_INT(flags.initalign),
      5, EXT_INT_P(role), EXT_INT_P(race), EXT_INT_P(gend), EXT_INT_P(alignm),
         EXT_BOOLEAN_P(quit)))
	quit = 1;
    if (quit)
    {
	clearlocks();
	proxy_exit_nhwindows((char *)0);
	terminate(0);
    }
    flags.initrole = role;
    flags.initrace = race;
    flags.initgend = gend;
    flags.initalign = alignm;
}

void
proxy_askname()
{
    char *name = (char *)0;
    if (nhext_rpc(EXT_FID_ASKNAME, 0, 1, EXT_STRING_P(name))) {
	strncpy(plname, name, sizeof(plname) - 1);
	plname[sizeof(plname) - 1] = '\0';
    }
    free(name);
}

void
proxy_get_nh_event()
{
    (void)nhext_rpc(EXT_FID_GET_NH_EVENT, 0, 0);
}

void
proxy_exit_nhwindows(str)
const char *str;
{
    (void)nhext_rpc(EXT_FID_EXIT_NHWINDOWS, 1, EXT_STRING(str), 0);
}

void
proxy_suspend_nhwindows(str)
const char *str;
{
    (void)nhext_rpc(EXT_FID_SUSPEND_NHWINDOWS, 1, EXT_STRING(str), 0);
}

void
proxy_resume_nhwindows()
{
    (void)nhext_rpc(EXT_FID_RESUME_NHWINDOWS, 0, 0);
}

winid
proxy_create_nhwindow(type)
int type;
{
    winid id;
    int w;
    if (type == NHW_MAP) {
	struct proxy_mapwin *new;
	for(w = 0; w < proxy_no_mapwins; w++)
	    if (proxy_mapwins[w].id < 0)
		break;
	if (w == proxy_no_mapwins) {
	    if (proxy_mapwins)
		new = (struct proxy_mapwin *) realloc(proxy_mapwins,
		  (proxy_no_mapwins + 1) * sizeof(struct proxy_mapwin));
	    else
		new = (struct proxy_mapwin *)
		  malloc(sizeof(struct proxy_mapwin));
	    if (!new)
		return -1;
	    proxy_mapwins = new;
	    proxy_no_mapwins++;
	}
	proxy_mapwins[w].c_rows = 21;
	proxy_mapwins[w].c_cols = 80;
	proxy_mapwins[w].c_layers = 1;
    }
    if (!nhext_rpc(EXT_FID_CREATE_NHWINDOW, 1, EXT_INT(type), 1, EXT_INT_P(id)))
	id = -1;
    if (type == NHW_MAP) {
	proxy_mapwins[w].id = id;
#ifdef DISPLAY_LAYERS
	if (id >= 0)
	    proxy_flush_layers(w, TRUE);
#endif
    }
    return id;
}

static int
proxy_get_mapwin(window)
winid window;
{
    int w;
    for(w = 0; w < proxy_no_mapwins; w++)
	if (proxy_mapwins[w].id == window)
	    return w;
    return -1;
}

void
proxy_clear_nhwindow(window)
winid window;
{
    int w = proxy_get_mapwin(window);
    no_layers = level.flags.hero_memory ? 5 : 2;
    if (w >= 0) {
#ifdef DISPLAY_LAYERS
	proxy_flush_layers(w, TRUE);
#endif
	proxy_mapwins[w].c_rows = ROWNO;
	proxy_mapwins[w].c_cols = COLNO;
	proxy_mapwins[w].c_layers = no_layers;
	(void)nhext_rpc(EXT_FID_CLEAR_NHWINDOW, 4, EXT_INT(window),
	  EXT_INT(ROWNO), EXT_INT(COLNO), EXT_INT(no_layers), 0);
    } else
	(void)nhext_rpc(EXT_FID_CLEAR_NHWINDOW, 4, EXT_INT(window),
	  EXT_INT(0), EXT_INT(0), EXT_INT(0), 0);
}

void
proxy_display_nhwindow(window, blocking)
winid window;
boolean blocking;
{
    int w = proxy_get_mapwin(window);
#ifdef DISPLAY_LAYERS
    if (w >= 0)
	proxy_flush_layers(w, FALSE);
#endif
    (void)nhext_rpc(EXT_FID_DISPLAY_NHWINDOW,
      2, EXT_INT(window), EXT_BOOLEAN((nhext_xdr_bool_t)blocking), 0);
}

void
proxy_destroy_nhwindow(window)
winid window;
{
    struct proxy_mapwin *new;
    int w = proxy_get_mapwin(window);
    (void)nhext_rpc(EXT_FID_DESTROY_NHWINDOW, 1, EXT_INT(window), 0);
    if (w >= 0) {
	proxy_mapwins[w].id = -1;
	if (w == proxy_no_mapwins - 1) {
	    if (proxy_no_mapwins == 1) {
		free(proxy_mapwins);
		proxy_mapwins = (struct proxy_mapwin *)0;
		proxy_no_mapwins = 0;
	    } else {
		new = (struct proxy_mapwin *) realloc(proxy_mapwins,
		  (proxy_no_mapwins - 1) * sizeof(struct proxy_mapwin));
		if (new) {
		    proxy_mapwins = new;
		    proxy_no_mapwins--;
		}
	    }
	}
    }
    mapid_del_winid(window);
}

int proxy_curs_on_u = FALSE;

void
proxy_curs(window, x, y)
winid window;
int x, y;
{
    proxy_curs_on_u = x == u.ux && y == u.uy;
    (void)nhext_rpc(EXT_FID_CURS,
      3, EXT_INT(window), EXT_INT(x), EXT_INT(y), 0);
}

void
proxy_putstr(window, attr, str)
winid window;
int attr;
const char *str;
{
    if (window >= 0)
	(void)nhext_rpc(EXT_FID_PUTSTR,
	  3, EXT_INT(window), EXT_INT(attr), EXT_STRING(str), 0);
    else if (attr == ATR_NONE)
	proxy_raw_print(str);
    else
	proxy_raw_print_bold(str);
}

void
#ifdef FILE_AREAS
proxy_display_file(farea, fname, complain)
const char *farea;
#else
proxy_display_file(fname, complain)
#endif
const char *fname;
BOOLEAN_P complain;
{
    int fh;
#ifdef FILE_AREAS
    fh = dlbh_fopen_area(farea, fname, "r");
#else
    fh = dlbh_fopen(fname, "r");
#endif
    if (fh < 0) {
	if (complain)  pline("Can't open %s.", fname);
	return;
    }
    (void)nhext_rpc(EXT_FID_DISPLAY_FILE,
      1, EXT_INT(fh), 0);
    dlbh_fclose(fh);
}

void
proxy_start_menu(window)
winid window;
{
    (void)nhext_rpc(EXT_FID_START_MENU, 1, EXT_INT(window), 0);
    mapid_del_identifiers(window);
}

void
proxy_add_menu(window, glyph, identifier, ch, gch, attr, str, preselected)
winid window;
int glyph;
const anything *identifier;
CHAR_P ch, gch;
int attr;
const char *str;
BOOLEAN_P preselected;
{
    int mapping = mapid_map_identifier(window, identifier);
    if (glyph != NO_GLYPH)
	glyph = glyph2proxy[glyph];
    (void)nhext_rpc(EXT_FID_ADD_MENU,
      8, EXT_INT(window), EXT_INT(glyph), EXT_INT(mapping), EXT_INT(ch),
         EXT_INT(gch), EXT_INT(attr), EXT_STRING(str),
	 EXT_BOOLEAN(preselected),
      0);
}

void
proxy_end_menu(window, prompt)
winid window;
const char *prompt;
{
    (void)nhext_rpc(EXT_FID_END_MENU,
      2, EXT_INT(window), EXT_STRING(prompt), 0);
}

int
proxy_select_menu(window, how, menu_list)
winid window;
int how;
menu_item **menu_list;
{
    int i;
    struct proxy_select_menu_res ret = {0, 0, (struct proxy_mi *)0};
    if (!nhext_rpc(EXT_FID_SELECT_MENU,
      2, EXT_INT(window), EXT_INT(how),
      1, EXT_XDRF(proxy_xdr_select_menu_res, &ret))) {
	if (ret.selected)
	    nhext_xdr_free(proxy_xdr_select_menu_res, (char *)&ret);
	return 0;	/* Nothing selected */
    }
    *menu_list = (menu_item *) alloc(ret.n * sizeof(menu_item));
    for(i = 0; i < ret.n; i++) {
	mapid_unmap_identifier(window, ret.selected[i].item,
	  &(*menu_list)[i].item);
	(*menu_list)[i].count = ret.selected[i].count;
    }
    i = ret.retval;
    nhext_xdr_free(proxy_xdr_select_menu_res, (char *)&ret);
    return i;
}

char
proxy_message_menu(let, how, mesg)
char let;
int how;
const char *mesg;
{
    int ret;
    if (!nhext_rpc(EXT_FID_MESSAGE_MENU,
      3, EXT_INT(let), EXT_INT(how), EXT_STRING(mesg), 1, EXT_INT_P(ret)))
	ret = -1;
    return (char)ret;
}

void
proxy_update_inventory()
{
    (void)nhext_rpc(EXT_FID_UPDATE_INVENTORY, 0, 0);
}

void
proxy_mark_synch()
{
    (void)nhext_rpc(EXT_FID_MARK_SYNC, 0, 0);
}

void
proxy_wait_synch()
{
    (void)nhext_rpc(EXT_FID_WAIT_SYNC, 0, 0);
}

#ifdef CLIPPING
void
proxy_cliparound(x, y)
int x, y;
{
    (void)nhext_rpc(EXT_FID_CLIPAROUND, 2, EXT_INT(x), EXT_INT(y), 0);
}
#endif

#ifdef POSITIONBAR
void
proxy_update_positionbar(posbar)
char *posbar;
{
    (void)nhext_rpc(EXT_FID_UPDATE_POSITIONBAR, 1, EXT_STRING(posbar), 0);
}
#endif

#ifdef DISPLAY_LAYERS
static void
proxy_print_layer(w, x, y, z, glyph)
int w;
xchar x, y, z;
int glyph;
{
    int g;
    struct gbuf_layer *l = proxy_mapwins[w].gbuf_layers + z;
    g = glyph == NO_GLYPH ? NO_GLYPH : glyph2proxy[glyph];
    if (l->rows[y].glyphs[x] != g) {
	l->rows[y].glyphs[x] = g;
	if (l->start > y)
	    l->start = y;
	if (l->stop < y)
	    l->stop = y;
	if (l->rows[y].start > x)
	    l->rows[y].start = x;
	if (l->rows[y].stop < x)
	    l->rows[y].stop = x;
    }
}

static void
proxy_flush_layers(w, clearing)
int w;
int clearing;
{
    int x, y, z;
    struct gbuf_layer *l = proxy_mapwins[w].gbuf_layers;
    if (!clearing) {
	if (proxy_mapwins[w].c_rows != ROWNO ||
	  proxy_mapwins[w].c_cols != COLNO ||
	  proxy_mapwins[w].c_layers != no_layers) {
	    proxy_mapwins[w].c_rows = ROWNO;
	    proxy_mapwins[w].c_cols = COLNO;
	    proxy_mapwins[w].c_layers = no_layers;
	    (void)nhext_rpc(EXT_FID_CLEAR_NHWINDOW, 4,
	      EXT_INT(proxy_mapwins[w].id), EXT_INT(ROWNO), EXT_INT(COLNO),
	      EXT_INT(no_layers), 0);
	}
	proxy_print_glyph_layered(proxy_mapwins[w].id,
	  no_layers, proxy_mapwins[w].gbuf_layers);
    }
    for(z = 0; z < no_layers; z++) {
	l->stop = 0;
	l->start = ROWNO - 1;
	for(y = 0; y < ROWNO; y++) {
	    l->rows[y].stop = 0;
	    l->rows[y].start = COLNO - 1;
	    if (clearing)
		for(x = 0; x < COLNO; x++)
		    l->rows[y].glyphs[x] = NO_GLYPH;
	}
	l++;
    }
}
#endif

void
proxy_print_glyph(window, x, y, glyph)
winid window;
xchar x, y;
int glyph;
{
    int w = proxy_get_mapwin(window);
#ifdef DISPLAY_LAYERS
    if (w >= 0 && proxy_interface_mode & EXT_IM_DISPLAY_LAYERS) {
	struct rm *lev = &levl[x][y];

	if (!level.flags.hero_memory || glyph_is_floating(glyph))
	    proxy_print_layer(w, x, y, 0, glyph);
	else
	    proxy_print_layer(w, x, y, 0,
	      lev->mem_invis ? GLYPH_INVISIBLE : NO_GLYPH);
	if (level.flags.hero_memory) {
	    if (lev->mem_obj)
		proxy_print_layer(w, x, y, 1, lev->mem_corpse ?
		  body_to_glyph(lev->mem_obj - 1) :
		  objnum_to_glyph(lev->mem_obj - 1));
	    else
		proxy_print_layer(w, x, y, 1, NO_GLYPH);
	    proxy_print_layer(w, x, y, 2, lev->mem_trap ?
	      cmap_to_glyph(lev->mem_trap - 1 + MAXDCHARS) : NO_GLYPH);
	    switch (lev->mem_bg) {
		case S_room:
		case S_corr:
		case S_litcorr:
		case S_air:
		case S_water:
		    proxy_print_layer(w, x, y, 3, NO_GLYPH);
		    proxy_print_layer(w, x, y, 4, cmap_to_glyph(lev->mem_bg));
		    break;
		default:
		    proxy_print_layer(w, x, y, 3, cmap_to_glyph(lev->mem_bg));
		    if (Is_airlevel(&u.uz))
			proxy_print_layer(w, x, y, 4, cmap_to_glyph(S_air));
		    else if (Is_waterlevel(&u.uz))
			proxy_print_layer(w, x, y, 4, cmap_to_glyph(S_water));
		    else if (lev->roomno != NO_ROOM)
			proxy_print_layer(w, x, y, 4, cmap_to_glyph(S_room));
		    else if (lev->waslit || flags.lit_corridor)
			proxy_print_layer(w, x, y, 4, cmap_to_glyph(S_litcorr));
		    else
			proxy_print_layer(w, x, y, 4, cmap_to_glyph(S_corr));
		    break;
	    }
	} else
	    proxy_print_layer(w, x, y, 1, back_to_glyph(x, y));
    }
    else
#endif
    (void)nhext_rpc(EXT_FID_PRINT_GLYPH,
      4, EXT_INT(window), EXT_INT(x), EXT_INT(y), EXT_INT(glyph2proxy[glyph]),
      0);
}

void
proxy_raw_print(str)
const char *str;
{
    static int active = 0;
    if (proxy_rawprint_win != WIN_ERR) {
	proxy_putstr(proxy_rawprint_win, 0, str);
	return;
    }
    if (active++ || in_proxy_init ||
      !nhext_rpc(EXT_FID_RAW_PRINT, 1, EXT_STRING(str), 0)) {
	fputs(str, stderr);
	fputc('\n', stderr);
	(void) fflush(stderr);
    }
    active--;
}

void
proxy_raw_print_bold(str)
const char *str;
{
    static int active = 0;
    if (proxy_rawprint_win != WIN_ERR) {
	proxy_putstr(proxy_rawprint_win, ATR_BOLD, str);
	return;
    }
    if (active++ || in_proxy_init ||
      !nhext_rpc(EXT_FID_RAW_PRINT_BOLD, 1, EXT_STRING(str), 0)) {
	fputs(str, stderr);
	fputc('\n', stderr);
	(void) fflush(stderr);
    }
    active--;
}

int
proxy_nhgetch()
{
    int ret;
    if (!nhext_rpc(EXT_FID_NHGETCH, 0, 1, EXT_INT_P(ret)))
	ret = 0;
    return ret;
}

int
proxy_nh_poskey(x, y, mod)
int *x, *y, *mod;
{
    int ret, lx, ly, lmod;
    if (!nhext_rpc(EXT_FID_NH_POSKEY, 0,
      4, EXT_INT_P(ret), EXT_INT_P(lx), EXT_INT_P(ly), EXT_INT_P(lmod)))
	return proxy_nhgetch();
    *x = lx;
    *y = ly;
    *mod = lmod;
    return ret;
}

void
proxy_nhbell()
{
    (void)nhext_rpc(EXT_FID_NHBELL, 0, 0);
}

int
proxy_doprev_message()
{
    int ret;
    if (!nhext_rpc(EXT_FID_DOPREV_MESSAGE, 0, 1, EXT_INT_P(ret)))
	ret = 0;
    return ret;
}

char
proxy_yn_function(query, resp, def)
const char *query, *resp;
char def;
{
    int ret, count;
    if (!nhext_rpc(EXT_FID_YN_FUNCTION,
      3, EXT_STRING(query), EXT_STRING(resp), EXT_INT(def),
      2, EXT_INT_P(ret), EXT_INT_P(count)))
	ret = def;
    if (ret == '#')
	yn_number = count;
    return ret;
}

void
proxy_getlin(query, bufp)
const char *query;
char *bufp;
{
    char *reply = (char *)0;
    if (nhext_rpc(EXT_FID_GETLIN,
      1, EXT_STRING(query), 1, EXT_STRING_P(reply))) {
	strncpy(bufp, reply, BUFSZ - 1);
	bufp[BUFSZ - 1] = '\0';
    } else
	bufp[0] = '\0';
    free(reply);
}

int
proxy_get_ext_cmd()
{
    int extcmd;
    if (!nhext_rpc(EXT_FID_GET_EXT_CMD, 0, 1, EXT_INT_P(extcmd)))
	extcmd = -1;
    return extcmd;
}

void
proxy_number_pad(state)
int state;
{
    (void)nhext_rpc(EXT_FID_NUMBER_PAD, 1, EXT_INT(state), 0);
}

void
proxy_delay_output()
{
    (void)nhext_rpc(EXT_FID_DELAY_OUTPUT, 0, 0);
}

#ifdef CHANGE_COLOR
void
proxy_change_color(color, rgb, reverse)
int color;
long rgb;
int reverse;
{
    (void)nhext_rpc(EXT_FID_CHANGE_COLOR,
      3, EXT_INT(color), EXT_LONG(rgb), EXT_BOOL(reverse), 0);
}

#ifdef MAC
void
proxy_change_background(white_or_black)
int white_or_black;
{
    (void)nhext_rpc(EXT_FID_CHANGE_BACKGROUND, 1, EXT_BOOL(white_or_black), 0);
}

short
proxy_set_font_name(window, font)
winid window;
char *font;
{
    int ret;
    if (!nhext_rpc(EXT_FID_SET_FONT_NAME, 2, EXT_INT(window), EXT_STRING(font),
      1, EXT_INT_P(ret)))
	ret = -1;
    return (short)ret;
}
#endif	/* MAC */

char *
proxy_get_color_string()
{
    char *ret = (char *)0;
    if (!nhext_rpc(EXT_FID_GET_COLOR_STRING, 0, 1, EXT_STRING_P(ret))) {
	free(ret);
	return "";
    }
    return ret;
}
#endif	/* CHANGE_COLOR */

void
proxy_start_screen()
{
    (void)nhext_rpc(EXT_FID_START_SCREEN, 0, 0);
}

void
proxy_end_screen()
{
    (void)nhext_rpc(EXT_FID_END_SCREEN, 0, 0);
}

void
proxy_outrip(window, how)
winid window;
int how;
{
    nhext_xdr_bool_t handled;
    char *killed_by;
    killed_by = get_killer_string(how);
    if (!nhext_rpc(EXT_FID_OUTRIP, 2, EXT_INT(window), EXT_STRING(killed_by),
      1, EXT_BOOLEAN_P(handled)))
	handled = 0;
    if (!handled)
	genl_outrip(window, how);
}

void
proxy_preference_update(pref)
const char *pref;
{
    const char *value = get_option(pref);
    (void)nhext_rpc(EXT_FID_PREFERENCE_UPDATE, 2, EXT_STRING(pref),
      EXT_STRING(value), 0);
}

void
proxy_status(reconfig, nv, values)
int reconfig, nv;
const char **values;
{
    struct proxy_status_req req;
    req.reconfig = reconfig;
    req.nv = nv;
    req.values = values;
    (void)nhext_rpc(EXT_FID_STATUS, 1, EXT_XDRF(proxy_xdr_status_req, &req), 0);
}

#ifdef DISPLAY_LAYERS
static void
proxy_print_glyph_layered(window, nl, layers)
winid window;
int nl;
struct gbuf_layer *layers;
{
    int i, j, k;
    struct proxy_print_glyph_layered_req req;
    req.window = window;
    req.nl = nl;
    req.layers = (struct proxy_glyph_layer *)alloc(nl *
      sizeof(struct proxy_glyph_layer));
    for(i = 0; i < nl; i++) {
	if (layers[i].stop - layers[i].start >= 0) {
	    req.layers[i].start = layers[i].start;
	    req.layers[i].nr = layers[i].stop - layers[i].start + 1;
	    req.layers[i].rows = (struct proxy_glyph_row *)
	      alloc(req.layers[i].nr * sizeof(struct proxy_glyph_row));
	    for(j = 0, k = layers[i].start; j < req.layers[i].nr; j++, k++)
		if (layers[i].rows[k].stop - layers[i].rows[k].start >= 0) {
		    req.layers[i].rows[j].start = layers[i].rows[k].start;
		    req.layers[i].rows[j].ng =
		      layers[i].rows[k].stop - layers[i].rows[k].start + 1;
		    req.layers[i].rows[j].glyphs =
		      layers[i].rows[k].glyphs + layers[i].rows[k].start;
		} else {
		    req.layers[i].rows[j].start = 0;
		    req.layers[i].rows[j].ng = 0;
		    req.layers[i].rows[j].glyphs = (int *)0;
		}
	} else {
	    req.layers[i].start = 0;
	    req.layers[i].nr = 0;
	    req.layers[i].rows = (struct proxy_glyph_row *)0;
	}
    }
    (void)nhext_rpc(EXT_FID_PRINT_GLYPH_LAYERED,
      1, EXT_XDRF(proxy_xdr_print_glyph_layered_req, &req), 0);
    for(i = 0; i < nl; i++)
	if (req.layers[i].rows)
	    free(req.layers[i].rows);
    free(req.layers);
}
#endif	/* DISPLAY_LAYERS */

FILE *proxy_config_fp = NULL;

FILE *
proxy_config_file_open()
{
    proxy_config_fp = tmpfile();
    if (proxy_config_fp) {
	/* Since this is currently the only use of writable dlbh streams,
	 * we simply use a hard-coded handle of zero.
	 */
	(void)nhext_rpc(EXT_FID_SEND_CONFIG_FILE, 1, EXT_INT(0), 0);
	rewind(proxy_config_fp);		/* Ready to read */
    }
    return proxy_config_fp;
}

void
proxy_config_file_close(FILE *fp)
{
    fclose(proxy_config_fp);
    proxy_config_fp = NULL;
}

extern struct nhext_svc proxy_callbacks[];

#ifdef DEBUG
static int debug_read(void *, void *, unsigned int);
static int debug_write(void *, void *, unsigned int);

#define READ_F	debug_read
#define WRITE_F	debug_write
#else
#define READ_F	proxy_read
#define WRITE_F	proxy_write
#endif

#ifdef WIN32
#define READ_H	((void *)_get_osfhandle(0))
#define WRITE_H	((void *)_get_osfhandle(1))
#else
#define READ_H	((void *)0)
#define WRITE_H	((void *)1)
#endif

#ifdef WIN32
int proxy_read(handle, buf, len)
void *handle;
void *buf;
int len;
{
    DWORD d;
    if (!ReadFile((HANDLE)handle, buf, len, &d, NULL)) {
	d = GetLastError();
	return d == ERROR_HANDLE_EOF || d == ERROR_BROKEN_PIPE ? 0 : -1;
    } else
	return d;
}

int proxy_pipe_read_nb(handle, buf, len)
void *handle;
void *buf;
int len;
{
    DWORD nb;
    if (!PeekNamedPipe((HANDLE)handle, NULL, 0, NULL, &nb, NULL))
	return -1;
    if (!nb)
	return -2;
    return READ_F(handle, buf, len);
}

int proxy_write(handle, buf, len)
void *handle;
void *buf;
int len;
{
    DWORD nb;
    if (!WriteFile((HANDLE)handle, buf, len, &nb, NULL))
	return -1;
    else
	return nb;
}
#else	/* WIN32 */
static int
proxy_read(handle, buf, len)
void *handle;
void *buf;
unsigned int len;
{
    int fd = (int)handle, nb;
    nb = read(fd, buf, len);
    return nb >= 0 ? nb : -1;
}

static int
proxy_read_nb(handle, buf, len)
void *handle;
void *buf;
int len;
{
    int fd = (int)handle, retval;
    fd_set readfds;
    struct timeval tv = {0, 0};
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    retval = select(fd + 1, &readfds, NULL, NULL, &tv);
    if (retval < 0)
	return -1;
    if (!retval)
	return -2;
    return READ_F(handle, buf, len);
}

static int
proxy_write(handle, buf, len)
void *handle;
void *buf;
unsigned int len;
{
    int fd = (int)handle, nb;
    fd_set fds;
    struct timeval tv;
    FD_ZERO(&fds);
    FD_SET(fd,&fds);
    tv.tv_sec = 30;	/* Don't block forever but allow for slow connections */
    tv.tv_usec = 0;
    if (select(fd+1, NULL, &fds, NULL, &tv)) {
	nb = write(fd, buf, len);
	return nb >= 0 ? nb : -1;
    } else
	return -1;
}
#endif	/* WIN32 */

struct proxy_auth_connection {
    int sin, sout, serr, pid;
    char error[BUFSZ + 1];
};

static void
proxy_auth_emit_error(struct proxy_auth_connection *auth)
{
    /*
     * If auth->error is empty then proxy_auth_verify was never called,
     * which implies that the remote end probably doesn't support
     * an authentication method which we allow.
     */
    const char *message = *auth->error ?
      auth->error : "Please upgrade your client.";
    winid w;
    int argc = 1;
    char *argv[2];
    /*
     * We are called before init_nhwindows,
     * but a window is much more user friendly.
     */
    argv[0] = DEF_GAME_NAME;
    argv[1] = NULL;
    proxy_init_nhwindows(&argc, argv);
    w = proxy_create_nhwindow(NHW_MENU);
    proxy_putstr(w, ATR_BOLD, "Authentication failed.");
    proxy_putstr(w, ATR_NONE, "");
    proxy_putstr(w, ATR_NONE, message);
    proxy_display_nhwindow(w, TRUE);
    proxy_destroy_nhwindow(w);
    proxy_exit_nhwindows(NULL);
}

/*
 * Returns non-NULL if helper program opened.
 */
 
static struct proxy_auth_connection *
proxy_auth_open()
{
    struct proxy_auth_connection *pac;
    char *authopts = nh_getenv("HACKAUTHENTICATION");
#ifdef WIN32
    if (authopts) {
	pac = (struct proxy_auth_connection *)alloc(sizeof(*pac));
	pac->sin = pac->sout = pac->serr = pac->pid = -1;
	strcpy(pac->error, "Authentication not supported under win32.");
	return pac;
    } else
	return NULL;
#else
    int s_in[2], s_out[2], s_err[2], r, status;
    char buf[BUFSZ + 1];
    if (authopts)
	parseauthentication(authopts);
    if (authentication.prog[0]) {
	pac = (struct proxy_auth_connection *)alloc(sizeof(*pac));
	pac->sin = pac->sout = pac->serr = pac->pid = -1;
	pac->error[0] = '\0';
	if (pipe(s_in) || pipe(s_out) || pipe(s_err)) {
	    strcpy(pac->error, "Can't create pipes to helper program.");
	    return pac;
	}
	if (!(pac->pid = fork())) {
	    int i, j, argc = 1;
	    char **argv;
	    char args[BUFSZ];
	    (void)setgid(getgid());
	    (void)setuid(getuid());
	    dup2(s_in[0], 0);
	    dup2(s_out[1], 1);
	    dup2(s_err[1], 2);
	    close(s_in[0]);
	    close(s_in[1]);
	    close(s_out[0]);
	    close(s_out[1]);
	    close(s_err[0]);
	    close(s_err[1]);
	    strcpy(args, authentication.args);
	    for(i = 0; args[i]; i++)
		if (args[i] == ' ' && i && args[i - 1] != ' ')
		    argc++;
	    argv = (char **) malloc((argc + 1) * sizeof(char *));
	    if (!argv)
	    {
		fprintf(stderr,"%s", "Resource failure\n");
		exit(1);
	    }
	    argv[0] = authentication.prog;
	    j = 1;
	    argv[j] = args;
	    for(i = 0; args[i]; i++)
		if (args[i] == ' ' && i) {
		    args[i] = '\0';
		    while(args[i + 1] == ' ')
			i++;
		    argv[++j] = args + i + 1;
		}
	    argv[++j] = NULL;
	    execvp(authentication.prog, argv);
	    perror(authentication.prog);
	    exit(1);
	}
	close(s_in[0]);
	close(s_out[1]);
	close(s_err[1]);
	pac->sin = s_in[1];
	pac->sout = s_out[0];
	pac->serr = s_err[0];
	r = read(pac->sout, buf, BUFSZ);
	if (r < 0) {
	    strcpy(pac->error, "Error reading from helper program.");
	    return pac;
	}
	if (r == 0) {
	    /*
	     * Authentication program has failed. If it has output an
	     * error on stardard error then report this.
	     */
	    r = read(pac->serr, buf, BUFSZ);
	    if (r > 0) {
		if (buf[r - 1] == '\n')
		    r--;
		buf[r] = '\0';
		strcpy(pac->error, buf);
	    } else {
		waitpid(pac->pid, &status, 0);
		if (!WIFEXITED(status)) {
		    if (WIFSIGNALED(status))
			sprintf(pac->error, "Helper program died (signal %d).",
			  WTERMSIG(status));
		    else
			strcpy(pac->error, "Helper program died.");
		} else if (WEXITSTATUS(status)) {
		    r = read(pac->serr, buf, BUFSZ);
		    if (r > 0) {
			if (buf[r - 1] == '\n')
			    r--;
			buf[r] = '\0';
			strcpy(pac->error, buf);
		    } else
			sprintf(pac->error, "Helper program exited (error %d).",
			  WEXITSTATUS(status));
		}
		else
		    sprintf(pac->error, "Helper program exited.");
	    }
	}
	/* TODO: Do something with the challenge string if not empty */
	return pac;
    }
    else
	return NULL;
#endif	/* WIN32 */
}

static int
proxy_auth_verify(struct proxy_auth_connection *pac,
  const char *name, const char *response)
{
#ifndef WIN32
    int r, status, verified = 0;
    char buf[BUFSZ + 1];
    if (*pac->error)
	return 0;
    strcpy(buf, name);
    strcat(buf, "\n");
    write(pac->sin, buf, strlen(buf));
    strcpy(buf, response);
    strcat(buf, "\n");
    write(pac->sin, buf, strlen(buf));
    waitpid(pac->pid, &status, 0);
    pac->pid = -1;
    if (!WIFEXITED(status)) {
	if (WIFSIGNALED(status))
	    sprintf(pac->error, "Helper program died (signal %d).",
	      WTERMSIG(status));
	else
	    strcpy(pac->error, "Helper program died.");
    } else if (WEXITSTATUS(status)) {
	r = read(pac->serr, buf, BUFSZ);
	if (r > 0) {
	    if (buf[r - 1] == '\n')
		r--;
	    buf[r] = '\0';
	    strcpy(pac->error, buf);
	} else
	    strcpy(pac->error, "Helper program exited.");
    }
    else
	verified = 1;
    return verified;
#else
    return 0;
#endif	/* WIN32 */
}

static void
proxy_auth_close(struct proxy_auth_connection *pac)
{
#ifndef WIN32
    int status;
    if (pac->sin >= 0)
	close(pac->sin);
    if (pac->sout >= 0)
	close(pac->sout);
    if (pac->serr >= 0)
	close(pac->serr);
    if (pac->pid >= 0) {
	if (!waitpid(pac->pid, &status, WNOHANG)) {
	    kill(pac->pid, 15);
	    sleep(1);
	    (void)waitpid(pac->pid, &status, WNOHANG);
	}
    }
#endif
    free(pac);
}

static int
proxy_init(struct proxy_auth_connection *auth)
{
    int i, j, k;
    static char *name = (char *)0;
    NhExtIO *rd, *wr;
    struct nhext_svc *services;
    struct nhext_line *lp = (struct nhext_line *)0, line;
    char standard[8];
#ifdef UNIX
    rd = nhext_io_open(READ_F, READ_H, NHEXT_IO_RDONLY);
    if (rd)
	nhext_io_setnbfunc(rd, proxy_read_nb);
#else
# ifdef WIN32
    void *read_h = READ_H;
    if (GetFileType((HANDLE)read_h) == FILE_TYPE_PIPE) {
	rd = nhext_io_open(READ_F, read_h, NHEXT_IO_RDONLY);
	if (rd)
	    nhext_io_setnbfunc(rd, proxy_pipe_read_nb);
    } else
# endif
    rd = nhext_io_open(READ_F, READ_H, NHEXT_IO_RDONLY | NHEXT_IO_NBLOCK);
#endif
    if (!rd) {
	pline("Proxy: Failed to open read NhExtIO stream");
	return FALSE;
    }
    wr = nhext_io_open(WRITE_F, WRITE_H, NHEXT_IO_WRONLY);
    if (!wr) {
	pline("Proxy: Failed to open write NhExtIO stream");
	nhext_io_close(rd);
	return FALSE;
    }
    if (proxy_extents[0].name) {
	for(i = j = 0; proxy_extents[i].name; i++)
	    j += proxy_extents[i].no_procedures;
	for(i = 0; proxy_callbacks[i].id; i++)
	    ;
	j += i;
	services = (struct nhext_svc *) alloc(j * sizeof(struct nhext_svc));
	for(i = j = 0; proxy_extents[i].name; i++) {
	    (*proxy_extents[i].init)(0x8000 + j);
	    for(k = 0; k < proxy_extents[i].no_procedures; k++, j++) {
		services[j].id = 0x8000 + j;
		services[j].handler = proxy_extents[i].handler;
	    }
	}
	for(i = 0; proxy_callbacks[i].id; i++)
	    services[j + i] = proxy_callbacks[i];
    } else
	services = proxy_callbacks;
    if (nhext_init(rd, wr, services) < 0) {
	pline("Proxy: Failed to initialize NhExt");
	nhext_io_close(rd);
	nhext_io_close(wr);
	return FALSE;
    }
    line.type = "NhExt";
    line.n = auth ? 5 : 4;
    line.tags = (char **)alloc(line.n * sizeof(char *));
    line.values = (char **)alloc(line.n * sizeof(char *));
    line.tags[0] = "standard";
    sprintf(standard, "%d.%d", EXT_STANDARD_MAJOR, EXT_STANDARD_MINOR);
    line.values[0] = standard;
    line.tags[1] = "game";
    line.values[1] = DEF_GAME_NAME;
    line.tags[2] = "version";
    line.values[2] = VERSION_STRING;
    line.tags[3] = "protocols";
    line.values[3] = "1,2";
    if (auth) {
	line.tags[4] = "authmethods";
	line.values[4] = "1";
    }
    i = nhext_subprotocol0_write_line(&line);
    free(line.tags);
    free(line.values);
    if (!i) {
	pline("Proxy: Failed to write NhExt greeting");
failed:
	if (lp)
	    nhext_subprotocol0_free_line(lp);
	nhext_end();
	nhext_io_close(rd);
	nhext_io_close(wr);
	return FALSE;
    }
    lp = nhext_subprotocol0_read_line();
    if (!lp) {
	pline("Proxy: Failed to read reply to NhExt greeting");
	goto failed;
    }
    if (strcmp(lp->type,"Ack")) {
	pline("Proxy: NhExt greeting not acknowledged");
	goto failed;
    }
    for(i = 0; i < lp->n; i++)
	if (!strcmp(lp->tags[i], "protocol")) {
	    if (!strcmp(lp->values[i], "1"))
		proxy_protocol = 1;
	    else if (!strcmp(lp->values[i], "2"))
		proxy_protocol = 2;
	    else {
		pline("Proxy: Illegal sub-protocol \"%s\"", lp->values[i]);
		goto failed;
	    }
	    break;
	}
    if (i == lp->n) {
	pline("Proxy: Missing protocol in acknowledgment");
	goto failed;
    }
    if (auth) {
	int method = 0;
	char *user = NULL;
	char *passwd = NULL;
	for(i = 0; i < lp->n; i++) {
	    if (!strcmp(lp->tags[i], "authmethod")) {
		if (!strcmp(lp->values[i], "1"))
		    method = 1;
	    } else if (!strcmp(lp->tags[i], "username"))
		user = lp->values[i];
	    else if (!strcmp(lp->tags[i], "password"))
		passwd = lp->values[i];
	}
	if (method == 1) {
	    proxy_authorized = proxy_auth_verify(auth, user, passwd);
	    if (proxy_authorized) {
		strncpy(plname, user, sizeof(plname) - 1);
		plname[sizeof(plname) - 1] = '\0';
	    }
	}
    } else
	proxy_authorized = 1;
    for(i = 0; i < lp->n; i++)
	if (!strcmp(lp->tags[i], "windowtype"))
	    break;
    if (name)
	free(name);
    if (i == lp->n)
	windowprocs.name = proxy_procs.name;
    else {
	name = (char *)alloc(strlen(lp->values[i]) + 7);
	sprintf(name, "proxy/%s", lp->values[i]);
	windowprocs.name = name;
    }
    nhext_set_protocol(proxy_protocol);
    return TRUE;
} 

#undef READ_F
#undef WRITE_F
#undef READ_H
#undef WRITE_H

#ifdef DEBUG
static void
debug_dump(buf, len, arrow)
void *buf;
unsigned int len;
char *arrow;
{
    int i, j, nc;
    long l;
    char cbuf[17];
    unsigned char *bp = buf;
    for(i = 0; i < len; ) {
	if ((i & 15) == 0) {
	    if (!i)
		fputs(arrow, stderr);
	    else {
		cbuf[16] = '\0';
		while(nc++ < 40)
		    fputc(' ', stderr);
		fputs(cbuf, stderr);
		fputs("\n  ", stderr);
	    }
	    nc = 2;
	}
	if (len - i >= 4) {
	    l = (long)bp[i] << 24 | (long)bp[i + 1] << 16 |
	      (long)bp[i + 2] << 8 | bp[i + 3];
	    fprintf(stderr, " %08X", l);
	    nc += 9;
	    for(j = 0; j < 4; j++, i++)
		cbuf[i & 15] = isgraph(bp[i]) || bp[i] == ' ' ?  bp[i] : '.';
	}
	else {
	    fprintf(stderr, " %02X", bp[i]);
	    nc += 3;
	    cbuf[i & 15] = isgraph(bp[i]) || bp[i] == ' ' ? bp[i] : '.';
	    i++;
	}
    }
    if (len) {
	cbuf[i & 15 ? i & 15 : 16] = '\0';
	while(nc++ < 40)
	    fputc(' ', stderr);
	fputs(cbuf, stderr);
    }
    fputc('\n', stderr);
}

static int
debug_read(handle, buf, len)
void *handle;
void *buf;
unsigned int len;
{
    int retval;
    retval = proxy_read(handle, buf, len);
    if (retval == -2)
	fputs("<- PENDING\n", stderr);
    else if (retval < 0)
	fputs("<- ERROR\n", stderr);
    else if (!retval)
	fputs("<- EOF\n", stderr);
    else
	debug_dump(buf, retval, "<-");
    return retval;
}

static int
debug_write(handle, buf, len)
void *handle;
void *buf;
unsigned int len;
{
    int retval;
    retval = proxy_write(handle, buf, len);
    if (retval < 0)
	fputs("-> ERROR\n", stderr);
    else
	debug_dump(buf, retval, "->");
    return retval;
}
#endif	/* DEBUG */

static void
win_proxy_errhandler(class, error)
int class;
const char *error;
{
    if (class == EXT_ERROR_COMMS) {
	fputs(error, stderr);
	fputc('\n', stderr);
	(void) fflush(stderr);
	hangup(0);
    } else {
	pline(error);
	pline("Program in disorder - perhaps you'd better #quit.");
    }
}

static unsigned long async_callbacks[] = {
    1 << EXT_CID_DISPLAY_INVENTORY - EXT_CID_DISPLAY_INVENTORY |
      1 << EXT_CID_DOREDRAW - EXT_CID_DISPLAY_INVENTORY |
      1 << EXT_CID_INTERFACE_MODE - EXT_CID_DISPLAY_INVENTORY |
      1 << EXT_CID_SET_OPTION_MOD_STATUS - EXT_CID_DISPLAY_INVENTORY,
};

void
win_proxy_init()
{
    int retval;
    struct proxycb_subprot2_init request, result;
    struct proxy_auth_connection *auth;
    in_proxy_init = TRUE;
    set_glyph_mapping();
    (void)nhext_set_errhandler(win_proxy_errhandler);
    auth = proxy_auth_open();
    if (!proxy_init(auth))
	panic("Proxy: Failed to initialize");
    if (proxy_protocol == 1)
	retval = nhext_rpc(EXT_FID_INIT, 0, 0);
    else {
	request.n_masks = SIZE(async_callbacks);
	request.masks = async_callbacks;
	result.masks = (unsigned long *)0;
	retval = nhext_rpc(EXT_FID_INIT,
	  1, EXT_XDRF(proxycb_xdr_subprot2_init, &request),
	  1, EXT_XDRF(proxycb_xdr_subprot2_init, &result));
	if (retval) {
	    nhext_set_async_masks(result.n_masks, result.masks);
	    free(result.masks);
	}
    }
    if (!retval)
	panic("Proxy: Failed to initialize window interface");
    /*
     * raw_print and raw_print_bold are now useable
     * and proxy_auth_emit_error() needs them.
     */
    in_proxy_init = FALSE;
    if (auth) {
	if (!proxy_authorized) {
	    proxy_auth_emit_error(auth);
	    clearlocks();
	    terminate(EXIT_SUCCESS);
	}
	proxy_auth_close(auth);
    }
}

int
win_proxy_iteration()
{
    int i;
    i = nhext_svc(proxy_callbacks);
    if (!i)
	fprintf(stderr,"%s", "proxy: Ignoring packet with zero ID\n");
    return i;
}

/* $Id: winproxy.c,v 1.36 2004-12-31 19:22:49 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2004 */
/* NetHack may be freely redistributed.  See license for details. */

/* #define DEBUG */

#include <sys/time.h>
#ifdef DEBUG
#include <ctype.h>
#endif
#include <nhproxy/nhproxy.h>
#include "hack.h"
#include "patchlevel.h"
#include "date.h"
#include "winproxy.h"
#ifdef WIN32
#include "win32api.h"
#endif

static void proxy_flush_layers();

/* Window to redirect raw output to, if not WIN_ERR */

int proxy_rawprint_win = WIN_ERR;

/* Current interface mode */

unsigned long proxy_interface_mode = 0;

/* Interface definition for plug-in windowing ports */
struct window_procs proxy_procs = {
    "proxy",
    0x0,
    0L,
    proxy_init_nhwindows,
    proxy_player_selection,
    proxy_askname,
    (nhproxy_proc_get_nh_event),
    (nhproxy_proc_exit_nhwindows),
    (nhproxy_proc_suspend_nhwindows),
    (nhproxy_proc_resume_nhwindows),
    (nhproxy_proc_create_nhwindow),
    proxy_clear_nhwindow,
    (void (*)())(nhproxy_proc_display_nhwindow),
    proxy_destroy_nhwindow,
    proxy_curs,
    proxy_putstr,
    proxy_display_file,
    proxy_start_menu,
    proxy_add_menu,
    (nhproxy_proc_end_menu),
    proxy_select_menu,
    (char (*)())(nhproxy_proc_message_menu),
    (nhproxy_proc_update_inventory),
    (nhproxy_proc_mark_synch),
    (nhproxy_proc_wait_synch),
#ifdef CLIPPING
    (nhproxy_proc_cliparound),
#endif
#ifdef POSITIONBAR
    (nhproxy_proc_update_positionbar),
#endif
    proxy_print_glyph,
    proxy_raw_print,
    proxy_raw_print_bold,
    (nhproxy_proc_nhgetch),
    (nhproxy_proc_nh_poskey),
    (nhproxy_proc_nhbell),
    (nhproxy_proc_doprev_message),
    proxy_yn_function,
    proxy_getlin,
    (nhproxy_proc_get_ext_cmd),
    (nhproxy_proc_number_pad),
    (nhproxy_proc_delay_output),
#ifdef CHANGE_COLOR
    (nhproxy_proc_change_color),
#ifdef MAC
    (nhproxy_proc_change_background),
    (nhproxy_proc_set_font_name),
#endif
    (nhproxy_proc_get_color_string),
#endif
    (nhproxy_proc_start_screen),
    (nhproxy_proc_end_screen),
    proxy_outrip,
    proxy_preference_update,
};

/* Extensions to the NhExt protocol */
struct nhproxy_extension win_proxy_extents[] = {
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
    int i, j, capc;
    char **capv;
    extern struct wc_Opt wc_options[];
    if (nhproxy_proc_init_nhwindows(argcp, argv))
    {
	iflags.window_inited = TRUE;
	nhproxy_serv_get_capabilities(&capc, &capv);
	for(i = 0; i < capc; i++)
	    for(j = 0; wc_options[j].wc_name; j++)
		if (!strcmp(wc_options[j].wc_name, capv[i]))
		    windowprocs.wincap |= wc_options[j].wc_bit;
    }
}

void
proxy_player_selection()
{
    if (!nhproxy_proc_player_selection(&flags.initrole, &flags.initrace,
      &flags.initgend, &flags.initalign))
    {
	clearlocks();
	nhproxy_proc_exit_nhwindows((char *)0);
	terminate(0);
    }
}

void
proxy_askname()
{
    char *name = nhproxy_proc_askname();
    if (name) {
	strncpy(plname, name, sizeof(plname) - 1);
	plname[sizeof(plname) - 1] = '\0';
	free(name);
    }
}

void
proxy_clear_nhwindow(window)
winid window;
{
    nhproxy_proc_clear_nhwindow(window, ROWNO, COLNO,
      level.flags.hero_memory ? 5 : 2);
}

void
proxy_destroy_nhwindow(window)
winid window;
{
    nhproxy_proc_destroy_nhwindow(window);
    mapid_del_winid(window);
}

int proxy_curs_on_u = FALSE;

void
proxy_curs(window, x, y)
winid window;
int x, y;
{
    proxy_curs_on_u = x == u.ux && y == u.uy;
    nhproxy_proc_curs(window, x, y);
}

void
proxy_putstr(window, attr, str)
winid window;
int attr;
const char *str;
{
    if (window >= 0)
	nhproxy_proc_putstr(window, attr, str);
    else if (attr == ATR_NONE)
	nhproxy_proc_raw_print(str);
    else
	nhproxy_proc_raw_print_bold(str);
}

void
#ifdef FILE_AREAS
proxy_display_file(farea, fname, complain)
const char *farea;
#else
proxy_display_file(fname, complain)
#endif
const char *fname;
boolean complain;
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
    nhproxy_proc_display_file(fh);
    dlbh_fclose(fh);
}

void
proxy_start_menu(window)
int window;
{
    nhproxy_proc_start_menu(window);
    mapid_del_identifiers(window);
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
    int mapping = mapid_map_identifier(window, identifier);
    if (glyph == NO_GLYPH)
	glyph = -1;
    else
	glyph = glyph2proxy[glyph];
    nhproxy_proc_add_menu(window, glyph, mapping, ch, gch, attr, str,
      preselected);
}

int
proxy_select_menu(window, how, menu_list)
winid window;
int how;
menu_item **menu_list;
{
    int i, n;
    struct nhproxy_mi *selected;
    n = nhproxy_proc_select_menu(window, how, &selected);
    if (n) {
	*menu_list = (menu_item *) alloc(n * sizeof(menu_item));
	for(i = 0; i < n; i++) {
	    mapid_unmap_identifier(window, selected[i].item,
	      &(*menu_list)[i].item);
	    (*menu_list)[i].count = selected[i].count;
	}
	free(selected);
    }
    return n;
}

#ifdef DISPLAY_LAYERS
static void
proxy_print_layer(w, x, y, z, glyph)
int w;
xchar x, y, z;
int glyph;
{
    int g;
    g = glyph == NO_GLYPH ? NO_GLYPH : glyph2proxy[glyph];
    nhproxy_serv_print_layer(w, x, y, z, g);
}

static void
proxy_flush_layers(w, clearing)
int w;
int clearing;
{
    if (!nhproxy_serv_flush_layers(w, clearing, NO_GLYPH))
	panic("Not enough memory to resize window");
}
#endif

void
proxy_print_glyph(window, x, y, glyph)
winid window;
xchar x, y;
int glyph;
{
#ifdef DISPLAY_LAYERS
    int w = nhproxy_serv_get_mapwin(window);
    if (w >= 0 && proxy_interface_mode & NHPROXY_EXT_IM_DISPLAY_LAYERS) {
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
	nhproxy_proc_print_glyph(window, x, y, glyph2proxy[glyph]);
}

void
proxy_raw_print(str)
const char *str;
{
    if (proxy_rawprint_win != WIN_ERR)
	nhproxy_proc_putstr(proxy_rawprint_win, 0, str);
    else if (!nhproxy_proc_raw_print(str)) {
	fputs(str, stderr);
	fputc('\n', stderr);
	(void) fflush(stderr);
    }
}

void
proxy_raw_print_bold(str)
const char *str;
{
    if (proxy_rawprint_win != WIN_ERR)
	nhproxy_proc_putstr(proxy_rawprint_win, NHPROXY_EXT_ATR_BOLD, str);
    else if (!nhproxy_proc_raw_print_bold(str)) {
	fputs(str, stderr);
	fputc('\n', stderr);
	(void) fflush(stderr);
    }
}

char
proxy_yn_function(query, resp, def)
const char *query, *resp;
char def;
{
    char retval;
    int count;
    retval = nhproxy_proc_yn_function(query, resp, def, &count);
    yn_number = count;
    return retval;
}

void
proxy_getlin(query, bufp)
const char *query;
char *bufp;
{
    char *reply;
    reply = nhproxy_proc_getlin(query);
    if (reply) {
	strncpy(bufp, reply, BUFSZ - 1);
	bufp[BUFSZ - 1] = '\0';
	free(reply);
    } else
	bufp[0] = '\0';
}

void
proxy_outrip(window, how)
winid window;
int how;
{
    nhproxy_bool_t handled;
    handled = nhproxy_proc_outrip(window, get_killer_string(how));
    if (!handled)
	genl_outrip(window, how);
}

void
proxy_preference_update(pref)
const char *pref;
{
    nhproxy_proc_preference_update(pref, get_option(pref));
}

FILE *
proxy_config_file_open()
{
    return nhproxy_serv_config_file_open();
}

void
proxy_config_file_close(FILE *fp)
{
    nhproxy_serv_config_file_close(fp);
}

static void
win_proxy_errhandler(class, error)
int class;
const char *error;
{
    if (class == NHPROXY_ERROR_COMMS) {
	fputs(error, stderr);
	fputc('\n', stderr);
	(void) fflush(stderr);
	hangup(0);
    } else {
	pline(error);
	pline("Program in disorder - perhaps you'd better #quit.");
    }
}

extern struct nhproxy_serv_callbacks win_proxy_callbacks;

void
win_proxy_init()
{
    char *windowtype, *name;
    (void)nhproxy_rpc_set_errhandler(win_proxy_errhandler);
    set_glyph_mapping();
    nhproxy_serv_set_callbacks(&win_proxy_callbacks);
    nhproxy_serv_set_extensions(win_proxy_extents);
    if (!nhproxy_serv_accept(DEF_GAME_NAME, VERSION_STRING))
	panic("Proxy: Failed to initialize window interface");
    windowtype = nhproxy_serv_get_windowtype();
    if (windowtype) {
	name = (char *)alloc(strlen(windowtype) + 7);
	sprintf(name, "proxy/%s", windowtype);
	windowprocs.name = name;
    } else
	windowprocs.name = proxy_procs.name;
}

#if 0
int
win_proxy_iteration()
{
    int i;
    i = nhproxy_rpc_svc(proxy_callbacks);
    if (!i)
	fprintf(stderr, "proxy: Ignoring packet with zero ID\n");
    return i;
}
#endif

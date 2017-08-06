/*
  $Id: gtkint.c,v 1.6.2.1 2004-11-03 09:32:01 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2001-2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

/*
 * This module contains everything that is only needed for the internal
 * interface (ie., when linking the GTK interface with the game executable).
 */

#include <sys/types.h>
#include <signal.h>
#include "winGTK.h"
#include "nhxdr.h"
#include "proxycom.h"
#include "prxyclnt.h"
#include "wintype.h"
#include "func_tab.h"
#include "dlb.h"
#include "patchlevel.h"

static void
hook()
{
}

#ifdef GTK_PROXY

static void GTK_proxy_init_nhwindows(int *, char **);
static void GTK_proxy_askname(void);
static int GTK_proxy_nhgetch(void);
static void GTK_proxy_raw_print(const char *);
static void GTK_proxy_raw_print_bold(const char *);

/* Only the GTK_proxy_... functions are expected to be called.
 * Calling any other of these before GTK_proxy_init_nhwindows()
 * returns is likely to result in undefined behaviour.
 */

struct window_procs GTK_procs = {
    "gtk",
    0,
    0L,
    GTK_proxy_init_nhwindows,
    hook, /* player_selection */
    GTK_proxy_askname,
    GTK_get_nh_event,
    GTK_exit_nhwindows,
    hook, /* suspend_nhwindows */
    hook, /* resume_nhwindows */
    GTK_create_nhwindow,
    hook, /* clear_nhwindow */
    GTK_display_nhwindow,
    GTK_destroy_nhwindow,
    GTK_curs,
    GTK_putstr,
#ifdef FILE_AREAS
    (void (*)(const char *, const char *, BOOLEAN_P))hook, /* display_file */
#else
    (void (*)(const char *, BOOLEAN_P))hook, /* display_file */
#endif
    GTK_start_menu,
    (void (*)(winid, int, const anything *, CHAR_P, CHAR_P, int, const char *,
      BOOLEAN_P))hook, /* add_menu */
    GTK_end_menu,
    (int (*)())hook, /* select_menu */
    genl_message_menu,
    GTK_update_inventory,
    GTK_mark_synch,
    GTK_wait_synch,
#ifdef CLIPPING
    GTK_cliparound,
#endif
#ifdef POSITIONBAR
    hook, /* update_positionbar */
#endif
    (void (*)(winid, XCHAR_P, XCHAR_P, int))hook, /* print_glyph */
    GTK_proxy_raw_print,
    GTK_proxy_raw_print_bold,
    GTK_proxy_nhgetch,
    GTK_nh_poskey,
    hook, /* nhbell */
    GTK_doprev_message,
    (char (*)(const char *, const char *, CHAR_P))hook, /* yn_function */
    hook, /* getlin */
    GTK_get_ext_cmd,
    GTK_number_pad,
    GTK_delay_output,
#ifdef CHANGE_COLOR
    hook, /* change_color */
#ifdef MAC
    hook, /* change_background */
    hook, /* set_font_name */
#endif
    hook, /* get_color_string */
#endif
    hook, /* start_screen,*/
    hook, /* end_screen,*/
    genl_outrip,
    hook, /* preference_update,*/
};

static void
GTK_proxy_askname(void)
{
    strcpy(plname, "games");
}

static int
GTK_proxy_nhgetch(void)
{
    int c;
    do {
	c = getchar();
    } while(c == '\r');
    return c;
}

static void
GTK_proxy_raw_print(const char *str)
{
    fputs(str, stderr); fputc('\n', stderr); (void) fflush(stderr);
}

static void
GTK_proxy_raw_print_bold(const char *str)
{
    fputs(str, stderr); fputc('\n', stderr); (void) fflush(stderr);
}

#ifndef WIN32
static int
GTK_client_read(void *handle, void *buf, unsigned int len)
{
    int nb;
    nb = read((int)handle, buf, len);
    return nb >= 0 ? nb : -1;
}

static int
GTK_client_write(void *handle, void *buf, unsigned int len)
{
    int nb;
    nb = write((int)handle, buf, len);
    return nb >= 0 ? nb : -1;
}
#endif

static void
GTK_proxy_init_nhwindows(int *argcp, char **argv)
{
    int retval;
#ifdef WIN32
    /* Win32 has no concept of fork, so we simply execute ourselves */
    char *s;
    iflags.window_inited = 1;
    proxy_svc_set_ext_procs(win_GTK_init, &GTK_ext_procs);
    s = g_find_program_in_path(argv[0]);
    retval = proxy_connect("file", s ? s : argv[0], argcp, argv);
    if (!retval)
	proxy_start_client_services();
    exit(1);
#else
    int to_game[2],from_game[2];
#ifdef UNIX
    uid_t uid;
    gid_t gid;
#endif
    if (pipe(to_game) || pipe(from_game))
	panic("%s: Can't create NhExt stream", argv[0]);
    iflags.window_inited = 1;
    if (fork()) {
	dup2(to_game[0],0);
	dup2(from_game[1],1);
	close(to_game[1]);
	close(from_game[0]);
	choose_windows("proxy");
	if (strncmp(windowprocs.name, "proxy/", 6))
	    terminate(EXIT_SUCCESS);
	init_nhwindows(argcp, argv);
    } else {
	close(to_game[0]);
	close(from_game[1]);
#ifdef UNIX
	/*
	 * Drop all privileges. For non-Linux systems we just assume that
	 * seteuid()/setuid() suffices. Add more cases as needed.
	 */
	gid = getgid();
	uid = getuid();
#ifdef LINUX
	setresgid(gid, gid, gid);
	setresuid(uid, uid, uid);
#else
	setegid(gid);
	setgid(gid);
	seteuid(uid);
	setuid(uid);
#endif
#endif
	proxy_svc_set_ext_procs(win_GTK_init, &GTK_ext_procs);
	retval = proxy_init_client_services(GTK_client_read,
	  (void *)from_game[0], GTK_client_write, (void *)to_game[1]);
	if (!retval)
	    proxy_start_client_services();
	exit(1);
    }
#endif	/* WIN32 */
}

#else	/* GTK_PROXY */

static void GTK_int_init_nhwindows(int *, char **);
static void GTK_int_player_selection(void);
static void GTK_int_askname(void);
static void GTK_int_clear_nhwindow(winid);
#ifdef FILE_AREAS
static void GTK_int_display_file(const char *, const char *, BOOLEAN_P);
#else
static void GTK_int_display_file(const char *, BOOLEAN_P);
#endif
static void GTK_int_add_menu(winid, int, const ANY_P *, CHAR_P, CHAR_P, int, const char *, BOOLEAN_P);
static int GTK_int_select_menu(winid, int, MENU_ITEM_P **);
static void GTK_int_print_glyph(winid, XCHAR_P, XCHAR_P, int);
static char GTK_int_yn_function(const char *, const char *, CHAR_P);
static void GTK_int_outrip(winid, int);
static void GTK_int_getlin(const char *, char *);
static void GTK_int_preference_update(const char *);

struct window_procs GTK_procs = {
    "gtk",
    /* If you want to change these you probably also want to change the
     * equivalent values in GTK_capv[].
     */
    WC_COLOR | WC_HILITE_PET |
#if 0
    WC_ASCII_MAP | WC_TILED_MAP | WC_SPLASH_SCREEN | WC_POPUP_DIALOG |
    WC_EIGHT_BIT_IN |
#endif
    WC_PERM_INVENT,
    0L,
    GTK_int_init_nhwindows,
    GTK_int_player_selection,
    GTK_int_askname,
    GTK_get_nh_event,
    GTK_exit_nhwindows,
    hook, /* suspend_nhwindows */
    hook, /* resume_nhwindows */
    GTK_create_nhwindow,
    GTK_int_clear_nhwindow,
    GTK_display_nhwindow,
    GTK_destroy_nhwindow,
    GTK_curs,
    GTK_putstr,
    GTK_int_display_file,
    GTK_start_menu,
    GTK_int_add_menu,
    GTK_end_menu,
    GTK_int_select_menu,
    genl_message_menu,
    GTK_update_inventory,
    GTK_mark_synch,
    GTK_wait_synch,
#ifdef CLIPPING
    GTK_cliparound,
#endif
#ifdef POSITIONBAR
    hook, /* update_positionbar */
#endif
    GTK_int_print_glyph,
    GTK_raw_print,
    GTK_raw_print_bold,
    GTK_nhgetch,
    GTK_nh_poskey,
    hook, /* nhbell */
    GTK_doprev_message,
    GTK_int_yn_function,
    GTK_int_getlin,
    GTK_get_ext_cmd,
    GTK_number_pad,
    GTK_delay_output,
#ifdef CHANGE_COLOR
    hook, /* change_color */
#ifdef MAC
    hook, /* change_background */
    hook, /* set_font_name */
#endif
    hook, /* get_color_string */
#endif
    hook, /* start_screen,*/
    hook, /* end_screen,*/
#ifdef GRAPHIC_TOMBSTONE
    GTK_int_outrip,
#else
    genl_outrip,
#endif
    GTK_int_preference_update,
};

static void
GTK_int_init_nhwindows(int *argcp, char **argv)
{
    char **capv;
    if (GTK_ext_init_nhwindows(argcp, argv, &capv))
	iflags.window_inited = 1;
}

static void
GTK_int_player_selection(void)
{
    /*
     * The GTK interface does not provide a quit option within
     * player selection.
     */
    (void)GTK_ext_player_selection(&flags.initrole, &flags.initrace,
      &flags.initgend, &flags.initalign);
}

static void
GTK_int_askname() {
    char *name;
    name = GTK_ext_askname();
    strcpy(plname, name);
    free(name);
}

static void
GTK_int_clear_nhwindow(window)
winid window;
{
    GTK_ext_clear_nhwindow(window, ROWNO, COLNO, 1);
}

#ifdef FILE_AREAS
static void
GTK_int_display_file(farea, fname, complain)
const char *farea, *fname;
BOOLEAN_P complain;
#else
static void
GTK_int_display_file(fname, complain)
const char *fname;
BOOLEAN_P complain;
#endif
{
    int fh;
#ifdef FILE_AREAS
    fh = dlbh_fopen(farea, fname, "r");
#else
    fh = dlbh_fopen(fname, "r");
#endif
    if (fh < 0) {
	if (complain)  pline("Can't open %s.", fname);
	return;
    }
    GTK_ext_display_file(fh);
    dlbh_fclose(fh);
}

static void
GTK_int_add_menu(window, glyph, identifier, ch, gch, attr, str, preselected)
winid window;
int glyph;
const anything *identifier;
CHAR_P ch, gch;
int attr;
const char *str;
BOOLEAN_P preselected;
{
    int mapping = mapid_map_identifier(window, identifier);
    GTK_ext_add_menu(window, glyph, mapping, ch, gch, attr, str, preselected);
}

int
GTK_int_select_menu(window, how, menu_list)
winid window;
int how;
menu_item **menu_list;
{
    int i, n, retval;
    struct proxy_mi *selected;
    retval = GTK_ext_select_menu(window, how, &selected);
    n = retval <= 0 ? 0 : retval;
    *menu_list = (menu_item *) alloc(n * sizeof(menu_item));
    for(i = 0; i < n; i++) {
	mapid_unmap_identifier(window, selected[i].item, &(*menu_list)[i].item);
	(*menu_list)[i].count = selected[i].count;
    }
    if (n)
	free(selected);
    return retval;
}

static void
GTK_int_print_glyph(window, x, y, glyph)
winid window;
xchar x, y;
int glyph;
{
    GTK_ext_print_glyph(window, x, y, glyph);
}

static char
GTK_int_yn_function(query, resp, def)
const char *query, *resp;
char def;
{
    char ret;
    int count;
    ret = GTK_ext_yn_function(query, resp ? resp : "", def, &count);
    if (ret == '#')
	yn_number = count;
    return ret;
}

static void
GTK_int_outrip(winid id, int how)
{
    if (!GTK_ext_outrip(id, get_killer_string(how)))
	genl_outrip(id, how);
}

static void
GTK_int_getlin(const char *query, char *ret)
{
    char *line;
    line = GTK_ext_getlin(query);
    strcpy(ret, line);
    free(line);
}

static void
GTK_int_preference_update(const char *optnam)
{
    GTK_ext_preference_update(optnam, get_option(optnam));
}
#endif	/* GTK_PROXY */

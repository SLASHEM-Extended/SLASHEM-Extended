/*
  $Id: gtkint.c,v 1.2 2002-03-11 00:09:21 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2001-2002
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

/*
 * This module contains everything that is only needed for the internal
 * interface (ie., when linking the GTK interface with the game executable).
 */

#include <sys/types.h>
#include <signal.h>
#include "winGTK.h"
#include "wintype.h"
#include "winproxy.h"
#include "func_tab.h"
#include "dlb.h"
#ifdef SHORT_FILENAMES
#include "patchlev.h"
#else
#include "patchlevel.h"
#endif

static void hook();
static void FDECL(GTK_int_init_nhwindows, (int *, char **));
static void NDECL(GTK_int_player_selection);
static void NDECL(GTK_int_askname);
#ifdef FILE_AREAS
static void FDECL(GTK_int_display_file, (const char *, const char *,
		BOOLEAN_P));
#else
static void FDECL(GTK_int_display_file, (const char *, BOOLEAN_P));
#endif
static void FDECL(GTK_int_add_menu, (winid, int, const ANY_P *,
		CHAR_P, CHAR_P, int, const char *, BOOLEAN_P));
static int FDECL(GTK_int_select_menu, (winid, int, MENU_ITEM_P **));
static void FDECL(GTK_int_print_glyph, (winid, XCHAR_P, XCHAR_P, int));
static char FDECL(GTK_int_yn_function, (const char *, const char *, CHAR_P));
static void FDECL(GTK_int_outrip, (winid, int));
static void FDECL(GTK_int_getlin, (const char *, char *));

struct window_procs GTK_procs = {
    "gtk",
    GTK_int_init_nhwindows,
    GTK_int_player_selection,
    GTK_int_askname,
    GTK_get_nh_event,
    GTK_exit_nhwindows,
    hook, /* suspend_nhwindows */
    hook, /* resume_nhwindows */
    GTK_create_nhwindow,
    GTK_clear_nhwindow,
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
};

static void
hook()
{
    ;
}

static void
GTK_int_init_nhwindows(int *argcp, char **argv)
{
    if (GTK_ext_init_nhwindows(argcp, argv))
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
	mapid_unmap_identifier(window, selected[i].item,
	  &(*menu_list)[i].item);
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

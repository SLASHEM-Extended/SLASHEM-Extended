/*
  $Id: gtkext.c,v 1.8 2003-04-26 10:56:45 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2001-2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

/*
 * This module contains everything that is only needed for the external
 * interface (ie., when linking the GTK interface seperately from the game
 * executable).
 */

#include "winGTK.h"
#include "prxyclnt.h"

static void v_hook();
static int i_hook();
static char *s_hook();
static int GTK_ext_message_menu(int let, int how, const char *mesg);

struct window_ext_procs GTK_ext_procs = {
    "gtk",
    GTK_ext_init_nhwindows,
    GTK_ext_player_selection,
    GTK_ext_askname,
    GTK_get_nh_event,
    GTK_exit_nhwindows,
    v_hook, /* suspend_nhwindows */
    v_hook, /* resume_nhwindows */
    GTK_create_nhwindow,
    GTK_ext_clear_nhwindow,
    GTK_display_nhwindow,
    GTK_destroy_nhwindow,
    GTK_curs,
    GTK_putstr,
    GTK_ext_display_file,
    GTK_start_menu,
    GTK_ext_add_menu,
    GTK_end_menu,
    GTK_ext_select_menu,
    GTK_ext_message_menu,
    GTK_update_inventory,
    GTK_mark_synch,
    GTK_wait_synch,
    GTK_cliparound,
    v_hook, /* update_positionbar */
    GTK_ext_print_glyph,
    GTK_raw_print,
    GTK_raw_print_bold,
    GTK_nhgetch,
    GTK_nh_poskey,
    v_hook, /* nhbell */
    GTK_doprev_message,
    GTK_ext_yn_function,
    GTK_ext_getlin,
    GTK_get_ext_cmd,
    GTK_number_pad,
    GTK_delay_output,
    v_hook, /* change_color */
    v_hook, /* change_background */
    i_hook, /* set_font_name */
    s_hook, /* get_color_string */
    v_hook, /* start_screen,*/
    v_hook, /* end_screen,*/
    GTK_ext_outrip,
    GTK_ext_status,
    GTK_ext_print_glyph_layered,
};

static void
v_hook()
{
}

static int
i_hook()
{
    return FALSE;
}

static char *
s_hook()
{
    char *s;
    s = (char *)alloc(1);
    *s = '\0';
    return s;
}

/*ARGSUSED*/
static int
GTK_ext_message_menu(int let, int how, const char *mesg)
{
    pline("%s", mesg);
    return 0;
}

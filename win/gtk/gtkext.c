/*
  $Id: gtkext.c,v 1.4 2003-10-25 18:06:01 j_ali Exp $
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
static int GTK_ext_message_menu(int let, int how, const char *mesg);

struct window_ext_procs GTK_ext_procs = {
    "gtk",
    GTK_ext_init_nhwindows,
    GTK_ext_player_selection,
    GTK_ext_askname,
    NULL,	/* get_nh_event */
    GTK_exit_nhwindows,
    NULL,	/* suspend_nhwindows */
    NULL,	/* resume_nhwindows */
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
    NULL,	/* mark_sync */
    NULL,	/* wait_sync */
    GTK_cliparound,
    NULL,	/* update_positionbar */
    GTK_ext_print_glyph,
    GTK_raw_print,
    GTK_raw_print_bold,
    GTK_nhgetch,
    GTK_nh_poskey,
    NULL,	/* nhbell */
    NULL,	/* doprev_message */
    GTK_ext_yn_function,
    GTK_ext_getlin,
    GTK_get_ext_cmd,
    GTK_number_pad,
    GTK_delay_output,
    NULL,	/* change_color */
    NULL,	/* change_background */
    NULL,	/* set_font_name */
    NULL,	/* get_color_string */
    NULL,	/* start_screen,*/
    NULL,	/* end_screen,*/
    GTK_ext_outrip,
    GTK_ext_preference_update,
    GTK_ext_status,
    GTK_ext_print_glyph_layered,
#ifdef GTKHACK
    GTK_ext_send_config_file,
#else
    v_hook,	/* send_config_file */
#endif
};

static void
v_hook()
{
}

/*ARGSUSED*/
static int
GTK_ext_message_menu(int let, int how, const char *mesg)
{
    pline("%s", mesg);
    return 0;
}

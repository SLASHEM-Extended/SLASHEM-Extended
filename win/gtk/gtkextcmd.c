/*
  $Id: gtkextcmd.c,v 1.7 2003-12-28 18:43:40 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
       Modifications Copyright (c) J. Ali Harlow 2000
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "winGTK.h"
#include "proxycb.h"

#ifndef GTK_PROXY
#include "func_tab.h"
extern struct ext_func_tab extcmdlist[];
#endif

static int n_extcmds = 0;
static const char **extcmds = NULL;

/*
 * These must agree with the list in winGTK.h -ALI
 */

struct ext_cmd_map {
    char *txt;
} extcmdmap[NO_EXT_CMD_MAPS] = {
    { "adjust" },
    { "borrow" },
    { "chat" },
    { "conduct" },
    { "dip" },
    { "enhance" },
    { "explore" },
    { "force" },
    { "invoke" },
    { "jump" },
    { "loot" },
    { "monster" },
    { "name" },
    { "offer" },
    { "pray" },
    { "quit" },
    { "ride" },
    { "rub" },
    { "sit" },
    { "technique" },
    { "turn" },
    { "twoweapon" },
    { "untrap" },
    { "version" },
    { "wipe" },
    { "youpoly" },
};

static int extcmd = 0;

static void extcmd_init(boolean init)
{
    int i;
#ifdef GTK_PROXY
    struct proxycb_get_extended_commands_res *list;
#endif
    if (!init) {	/* Exit */
	for(i = 0; i < n_extcmds; i++)
	    free((char *)extcmds[i]);
	free(extcmds);
	extcmds = NULL;
	n_extcmds = 0;
    }
#ifdef GTK_PROXY
    list = proxy_cb_get_extended_commands();
    n_extcmds = list->n_commands;
    extcmds = (const char **)malloc(n_extcmds * sizeof(const char *));
    for(i = 0; i < n_extcmds; i++)
	extcmds[i] = strdup(list->commands[i]);
    proxy_cb_free_extended_commands(list);
#else
    for(n_extcmds = 0; extcmdlist[n_extcmds].ef_txt; n_extcmds++)
	;
    extcmds = (const char **)malloc(n_extcmds * sizeof(const char *));
    for(i = 0; i < n_extcmds; i++)
	extcmds[i] = strdup(extcmdlist[i].ef_txt);
#endif
}

void GTK_extcmd_set(int cmd)
{
    int j;

    if (!extcmds)
	extcmd_init(TRUE);

    extcmd = -1;

    if (cmd < 0 || cmd >= NO_EXT_CMD_MAPS)
	return;
    else {
	for(j = 0; j < n_extcmds; j++)
	    if (!nh_strncmpi(extcmdmap[cmd].txt, extcmds[j], -1)) {
		extcmd = j;
		return;
	    }
    }
}

static void
extcmd_clicked(GtkWidget *widget, gpointer data)
{
    extcmd = (int)data;
    gtk_main_quit();
}

static gint
extcmd_destroy(GtkWidget *widget, gpointer data)
{
    guint *hid = (guint *)data;
    *hid = 0;
    extcmd = -1;
    
    gtk_main_quit();

    return FALSE;
}

/*
 * This is not ideal since you can't choose between extended commands
 * that start with the same letter from the keyboard, but it will do
 * for now. --- ALI
 */

static gint
extcmd_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    int i, keysym;
    keysym = nh_keysym(event);

    if (keysym) {
	extcmd = -1;
	for (i = 0; i < n_extcmds; i++)
	    if (extcmds[i][0] == keysym)
		extcmd = i;
	if (extcmd >= 0 || keysym == '\033')
	    gtk_main_quit();
    }

    return FALSE;
}

int
GTK_get_ext_cmd()
{
    guint	hid;

    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *table = NULL;
    GtkWidget *d;
    int i, col, data;
    const char *label;

    if (!extcmds)
	extcmd_init(TRUE);

    if (extcmd) {
	i = extcmd;
	extcmd = 0;
	return i;
    }

    window = nh_gtk_window_dialog(TRUE);
    nh_position_popup_dialog(GTK_WIDGET(window));
    nh_gtk_focus_set_master(GTK_WINDOW(window),
      GTK_SIGNAL_FUNC(extcmd_key_press), 0, TRUE);
    hid = gtk_signal_connect(GTK_OBJECT(window), "destroy",
      GTK_SIGNAL_FUNC(extcmd_destroy), &hid);

    frame = nh_gtk_new_and_add(gtk_frame_new(NULL), window, "");

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);
    table = nh_gtk_new_and_pack(gtk_table_new((n_extcmds+2)/3, 3, TRUE), hbox,
      "", FALSE, FALSE, NH_PAD);
    gtk_table_set_row_spacings(GTK_TABLE(table), NH_PAD);
    gtk_table_set_col_spacings(GTK_TABLE(table), NH_PAD);
    for(i = 0; i < n_extcmds; i++) {
	if (!strcmp(extcmds[i], "?")) {
	    /*
	     * Rather more useful than '?' in these circumstances -ALI
	     */
	    d = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	    col = (i == n_extcmds - 1 && i%3 == 0) ? 1 : i%3;
	    data = -1;
	} else {
	    d = gtk_button_new_with_label(extcmds[i]);
	    col = i%3;
	    data = i;
	}
	d = nh_gtk_new_and_attach(d, table, "", col, col+1, i/3, i/3+1);
	gtk_signal_connect(GTK_OBJECT(d), "clicked",
	  GTK_SIGNAL_FUNC(extcmd_clicked), (gpointer)data);
    }

    gtk_grab_add(window);
    gtk_widget_show_all(window);

    gtk_main();

    if (hid > 0) {
	gtk_widget_unmap(window);
	gtk_signal_disconnect(GTK_OBJECT(window), hid);

	gtk_widget_destroy(vbox);
	gtk_widget_destroy(frame);
	gtk_widget_destroy(window);
    }

    i = extcmd;
    extcmd = 0;
    return i;
}

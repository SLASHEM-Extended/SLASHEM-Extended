/*
  $Id: gtkextcmd.c,v 1.3 2000-12-03 15:07:38 j_ali Exp $
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
#include "func_tab.h"

extern struct ext_func_tab extcmdlist[];

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

void GTK_extcmd_set(int cmd)
{
    int j;
    
    extcmd = -1;
    
    if (cmd < 0 || cmd >= NO_EXT_CMD_MAPS)
	return;
    else {
	for(j = 0; extcmdlist[j].ef_txt; j++)
	    if (!strcmpi(extcmdmap[cmd].txt, extcmdlist[j].ef_txt))
	    {
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
    
    if(keysym){
	i = 0;
	extcmd=-1;
	while(extcmdlist[i].ef_txt){
	    if(extcmdlist[i].ef_txt[0] == keysym){
		extcmd=i;
	    }
	    ++i;
	}
	if(extcmd>=0 || keysym=='\033')
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
    int i, data;
    const char *label;

    if (extcmd)
    {
	i = extcmd;
	extcmd = 0;
	return i;
    }

    window = gtk_window_new(GTK_WINDOW_DIALOG);
    nh_position_popup_dialog(GTK_WIDGET(window));
    gtk_signal_connect_after(
	GTK_OBJECT(window), "key_press_event",
	GTK_SIGNAL_FUNC(extcmd_key_press), NULL);
    hid = gtk_signal_connect(
	GTK_OBJECT(window), "destroy",
	GTK_SIGNAL_FUNC(extcmd_destroy), &hid);

    frame = nh_gtk_new_and_add(gtk_frame_new(NULL), window, "");

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    hbox = nh_gtk_new_and_pack(
	gtk_hbox_new(FALSE, 0), vbox, "",
	FALSE, FALSE, NH_PAD);
    for(i=0; extcmdlist[i].ef_txt; i++)
	;
    table = nh_gtk_new_and_pack(
	gtk_table_new((i+2)/3, 3, TRUE), hbox, "",
	FALSE, FALSE, NH_PAD);
    for(i = 0; extcmdlist[i].ef_txt; i++)
    {
	if (!strcmp(extcmdlist[i].ef_txt, "?"))
	{
	    /*
	     * Rather more useful than '?' in these circumstances -ALI
	     */
	    label = "Cancel";
	    data = -1;
	}
	else
	{
	    label = extcmdlist[i].ef_txt;
	    data = i;
	}
	d = nh_gtk_new_and_attach(
	    gtk_button_new_with_label(label), table, "",
	    i%3, i%3+1, i/3, i/3+1);
	gtk_signal_connect(
	    GTK_OBJECT(d), "clicked",
	    GTK_SIGNAL_FUNC(extcmd_clicked), (gpointer)data);
    }

    gtk_grab_add(window);
    gtk_widget_show_all(window);
    
    gtk_main();

    if(hid > 0){
	gtk_widget_unmap(window);
	gtk_signal_disconnect(GTK_OBJECT(window), hid);

	gtk_widget_destroy(frame);
	gtk_widget_destroy(vbox);
	gtk_widget_destroy(window);
    }

    i = extcmd;
    extcmd = 0;
    return i;
}

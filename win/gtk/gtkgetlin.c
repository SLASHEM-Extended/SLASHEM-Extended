/*
  $Id: gtkgetlin.c,v 1.5 2003-05-24 15:15:15 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2000-2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "winGTK.h"

static GtkWidget	*window;
static int		cancelled;

static gint
getlin_destroy(GtkWidget *widget, gpointer data)
{
    window = NULL;
    cancelled = 1;
    gtk_main_quit();
    return FALSE;
}

static gint
entry_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    if (event->keyval == GDK_Return)
	gtk_main_quit();

    if (event->keyval == GDK_Escape) {
	cancelled = 1;
	gtk_main_quit();
    }

    return FALSE;
}

static gint
entry_ok(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gtk_main_quit();

    return FALSE;
}

static gint
entry_cancel(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    cancelled = 1;
    gtk_main_quit();

    return FALSE;
}

char *
GTK_ext_getlin(const char *query)
{
    char *s, *ret;
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *entry;
    GtkWidget *ok;
    GtkWidget *cancel;
    gulong h;

    cancelled = 0;

    window = nh_gtk_window_dialog(TRUE);
    nh_position_popup_dialog(GTK_WIDGET(window));

    frame = nh_gtk_new_and_add(gtk_frame_new(query), window, "");
    gtk_container_border_width(GTK_CONTAINER(frame), NH_PAD);

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    entry = nh_gtk_new_and_pack(gtk_entry_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

#if GTK_CHECK_VERSION(2,0,0)
    gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
#endif

    nh_gtk_focus_set_master(GTK_WINDOW(window),
      GTK_SIGNAL_FUNC(entry_key_press), 0);

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

    ok = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_OK), hbox, "",
      FALSE, FALSE, NH_PAD);

    GTK_WIDGET_SET_FLAGS(ok, GTK_CAN_DEFAULT);
    gtk_widget_grab_default(ok);

    gtk_signal_connect(GTK_OBJECT(ok), "clicked",
      GTK_SIGNAL_FUNC(entry_ok), NULL);

    cancel = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_CANCEL),
      hbox, "", FALSE, FALSE, NH_PAD);

    gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
      GTK_SIGNAL_FUNC(entry_cancel), NULL);

    h = gtk_signal_connect(GTK_OBJECT(window), "destroy",
      GTK_SIGNAL_FUNC(getlin_destroy), NULL);

    gtk_widget_grab_focus(entry);
    gtk_grab_add(window);
    gtk_widget_show_all(window);

    gtk_main();

    if (!cancelled) {
	s = (char *)gtk_entry_get_text(GTK_ENTRY(entry));
	ret = (char *)alloc(strlen(s) + 1);
	Strcpy(ret, s);
    } else {
	ret = (char *)alloc(1);
	*ret = '\0';
    }

    if (window) {
	gtk_signal_disconnect(GTK_OBJECT(window), h);
	gtk_widget_unmap(window);
	gtk_widget_destroy(window);
    }

    return ret;
}

/*
  $Id: gtkyn.c,v 1.12.2.1 2005-01-22 19:09:35 j_ali Exp $
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

static struct yn_params {
    const char *query;
    const char *resp;
    CHAR_P def;
    int *count;
    int isdir;
    int watch;
    GtkWidget *w;
} params;

static gint
yn_clicked(GtkWidget *widget, gpointer data)
{
    nh_key_add((char)GPOINTER_TO_INT(data));
    return TRUE;
}

static gint
yn_destroy(GtkWidget *widget, gpointer data)
{
    params.w = 0;
    params.watch++;
    return FALSE;
}

static int
yn_valid_response(int keysym)
{
    if (!*params.resp || index(params.resp, keysym))
	return keysym;
    else if (keysym == '\n' || keysym == ' ' || keysym == '\033')
	return params.def;
    else
	return 0;
}

static gint
yn_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    char keysym;

    keysym = params.isdir ? nh_dir_keysym(event) : 0;
    if (!keysym)
	keysym = nh_keysym(event);
    keysym = yn_valid_response(keysym);
    if (keysym)
	nh_key_add(keysym);
    return TRUE;
}

static gboolean
yn_show_window(gpointer data)
{
    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *table = NULL;
    GtkWidget *y, *n, *q;
    GtkWidget *d;
    gchar buf[NH_BUFSIZ], *bp;

    if (nh_key_check())		/* Avoid race condition */
	return FALSE;

    if (*params.resp) {
	sprintf(buf, "%s [", params.query);
	bp = eos(buf);
	strcpy(bp, params.resp);
	bp = index(bp, '\033');
	if (bp)
	    *bp = '\0';
	bp = eos(buf);
	*bp++ = ']';
	if (params.def)
	    sprintf(bp, "(%c) ", params.def);
	else {
	    *bp++ = ' ';
	    *bp = '\0';
	}
    } else
	sprintf(buf, "%s ", params.query);

    window = params.w;
    gtk_signal_connect(GTK_OBJECT(window), "destroy",
      GTK_SIGNAL_FUNC(yn_destroy), 0);

    frame = nh_gtk_new_and_add(gtk_frame_new(buf), window, "");
    gtk_container_border_width(GTK_CONTAINER(frame), NH_PAD);
    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    if (*params.resp) {
	hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
	  FALSE, FALSE, NH_PAD);
	if (!strcmp(params.resp, "yn") || !strcmp(params.resp, "ynq")) {
	    y = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_YES),
	      hbox, "", FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(GTK_OBJECT(y), "clicked",
	      GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'y');
	}
	if (!strcmp(params.resp, "yn") || !strcmp(params.resp, "ynq") ||
	 !strcmp(params.resp, "ynaq") || !strcmp(params.resp, "yn#aq")) {
	    n = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_NO),
	      hbox, "", FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(GTK_OBJECT(n), "clicked",
	      GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'n');
	}
	if (!strcmp(params.resp, "ynaq") || !strcmp(params.resp, "yn#aq")) {
	    q = nh_gtk_new_and_pack(gtk_button_new_with_label("All"), hbox, "",
	      FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(GTK_OBJECT(q), "clicked",
	      GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'a');
	}
	if (!strcmp(params.resp, "ynq") || !strcmp(params.resp, "ynaq") ||
	  !strcmp(params.resp, "yn#aq")) {
	    q = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_CANCEL),
	      hbox, "", FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(GTK_OBJECT(q), "clicked",
	      GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'q');
	}
    } else {
	hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
	  FALSE, FALSE, NH_PAD);
	if (index(params.query, '*')) {
	    q = nh_gtk_new_and_pack(gtk_button_new_with_label("List"), hbox, "",
	      FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(GTK_OBJECT(q), "clicked",
	      GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'?');
	    q = nh_gtk_new_and_pack(gtk_button_new_with_label("All List"), hbox,
	      "", FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(GTK_OBJECT(q), "clicked",
	      GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'*');
	} else if (params.isdir) {
	    /* maybe direction */
	    int i, j;
	    struct {
		char *str;
		int  key;
	    } dirstr[4][3] = {
		{
		    {"Y", 'y'},
		    {"K", 'k'},
		    {"U", 'u'},
		},
		{
		    {"H", 'h'},
		    {".", '.'},
		    {"L", 'l'},
		},
		{
		    {"B", 'b'},
		    {"J", 'j'},
		    {"N", 'n'},
		},
		{
		    {"Up", '<'},
		    {"", 0},
		    {"Down", '>'},
		}
	    }, 
	    np_dirstr[4][3] = {
		{
		    {"7", '7'},
		    {"8", '8'},
		    {"9", '9'},
		},
		{
		    {"4", '4'},
		    {".", '.'},
		    {"6", '6'},
		},
		{
		    {"1", '1'},
		    {"2", '2'},
		    {"3", '3'},
		},
		{
		    {"Up", '<'},
		    {"", 0},
		    {"Down", '>'},
		}
	    };

	    table = nh_gtk_new_and_pack(gtk_table_new(4, 3, TRUE), hbox, "",
	      FALSE, FALSE, NH_PAD);
	    for(i = 0; i < 4; i++)
		for(j = 0; j < 3; j++) {
		    d = nh_gtk_new_and_attach(
		      gtk_button_new_with_label(copts.num_pad ?
		        np_dirstr[i][j].str : dirstr[i][j].str),
		      table, "", j, j+1, i, i+1);
		    gtk_signal_connect(GTK_OBJECT(d), "clicked",
		      GTK_SIGNAL_FUNC(yn_clicked), 
		      copts.num_pad ? (gpointer)np_dirstr[i][j].key : 
			(gpointer)dirstr[i][j].key);
		}
	}
    }

    gtk_widget_show_all(window);
    return FALSE;
}

char
GTK_ext_yn_function(const char *query, const char *resp, CHAR_P def, int *count)
{
    int keysym = 0, exiting;
    guint timeout_id;

    while (nh_key_check()) {
	keysym = nh_key_get();
	keysym = yn_valid_response(keysym);
    }

    if (!keysym) {
	params.query = query;
	params.resp = resp;
	params.def = def;
	params.count = count;
	params.watch = 0;
	params.isdir = !*resp && strstr(query, "In what direction");
	params.w = nh_gtk_window_dialog(FALSE);
	nh_position_popup_dialog(GTK_WIDGET(params.w));
	nh_gtk_focus_set_master(GTK_WINDOW(params.w),
	  GTK_SIGNAL_FUNC(yn_key_press), 0, TRUE);
	timeout_id = g_timeout_add(500, yn_show_window, 0);
	exiting = main_hook(&params.watch);
	g_source_remove(timeout_id);
	if (exiting || params.watch) {
	    /* Either exiting or dialog was created and then closed by user */
	    nh_key_add(def);
	}
	if (params.w) {
	    gtk_widget_destroy(params.w);
	    params.w = 0;
	}
	keysym = nh_key_get();
    }

    return keysym;
}

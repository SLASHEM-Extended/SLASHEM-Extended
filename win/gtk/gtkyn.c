/*
  $Id: gtkyn.c,v 1.8 2001-06-16 18:14:40 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "winGTK.h"

static int		keysym;
static const char	*yn_resp;
static int		yn_def;
static int		yn_isdir;

static void
yn_clicked(GtkWidget *widget, gpointer data)
{
    keysym = (int)data;
    gtk_main_quit();
}

static gint
yn_destroy(GtkWidget *widget, gpointer data)
{
    guint *hid = (guint *)data;
    *hid = 0;
    keysym = yn_def;
    
    gtk_main_quit();

    return FALSE;
}

static gint
yn_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    keysym = yn_isdir ? nh_dir_keysym(event) : 0;

    if (!keysym)
	keysym = nh_keysym(event);
    
    if(keysym){
	if(!yn_resp || index(yn_resp, keysym))
	    gtk_main_quit();
	if (keysym == '\n' || keysym == ' ' || keysym == '\033') {
	    keysym = yn_def;	
	    gtk_main_quit();
	}
    }
    
    return FALSE;
}

char
GTK_yn_function(const char *query, const char *resp, CHAR_P def)
{
    guint	hid;

    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *clist = NULL;
    GtkWidget *table = NULL;
    GtkWidget	*y, *n, *q;
    GtkWidget	*d;
    gchar	buf[NH_BUFSIZ], *bp;
    gchar	*text[1];

    if(resp)
    {
	sprintf(buf, "%s [", query);
	bp = eos(buf);
	strcpy(bp, resp);
	bp = index(bp, '\033');
	if (bp)
	    *bp = '\0';
	bp = eos(buf);
	*bp++ = ']';
	if (def)
	    sprintf(bp, "(%c) ", def);
	else
	{
	    *bp++ = ' ';
	    *bp = '\0';
	}
    }
    else
	sprintf(buf, "%s ", query);
    text[0] = (gchar *)buf;

    yn_def = def;
    yn_resp = resp;
    yn_isdir = FALSE;

    window = gtk_window_new(GTK_WINDOW_DIALOG);
    nh_position_popup_dialog(GTK_WIDGET(window));
    gtk_signal_connect_after(
	GTK_OBJECT(window), "key_press_event",
	GTK_SIGNAL_FUNC(yn_key_press), NULL);
    hid = gtk_signal_connect(
	GTK_OBJECT(window), "destroy",
	GTK_SIGNAL_FUNC(yn_destroy), &hid);

    if(query){
	frame = nh_gtk_new_and_add(gtk_frame_new(buf), window, "");
	gtk_container_border_width(GTK_CONTAINER(frame), NH_PAD);

	vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");
    }
    else{
	frame = nh_gtk_new_and_add(gtk_frame_new(NULL), window, "");

	vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

	clist = nh_gtk_new_and_pack(
	    gtk_clist_new(1), vbox, "",
	    FALSE, FALSE, NH_PAD);
	gtk_clist_set_column_auto_resize(GTK_CLIST(clist), 0, TRUE);
	gtk_clist_append(GTK_CLIST(clist), text);
    }

    if(resp){
	hbox = nh_gtk_new_and_pack(
	    gtk_hbox_new(FALSE, 0), vbox, "",
	    FALSE, FALSE, NH_PAD);
	
	if(!strcmp(resp, "yn") || !strcmp(resp, "ynq")){
	    y = nh_gtk_new_and_pack(
		gtk_button_new_with_label("Yes"), hbox, "",
		FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(
		GTK_OBJECT(y), "clicked",
		GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'y');
	}
	if(!strcmp(resp, "yn") ||
	   !strcmp(resp, "ynq") ||
	   !strcmp(resp, "ynaq") ||
	   !strcmp(resp, "yn#aq")){
	    n = nh_gtk_new_and_pack(
		gtk_button_new_with_label("No"), hbox, "",
		FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(
		GTK_OBJECT(n), "clicked",
		GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'n');
	}
	if(!strcmp(resp, "ynaq") ||
	   !strcmp(resp, "yn#aq")){
	    q = nh_gtk_new_and_pack(
		gtk_button_new_with_label("All"), hbox, "",
		FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(
		GTK_OBJECT(q), "clicked",
		GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'a');
	}
	if(!strcmp(resp, "ynq") ||
	   !strcmp(resp, "ynaq") ||
	   !strcmp(resp, "yn#aq")){
	    q = nh_gtk_new_and_pack(
		gtk_button_new_with_label("Cancel"), hbox, "",
		FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(
		GTK_OBJECT(q), "clicked",
		GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'q');
	}
    }
    else{
	hbox = nh_gtk_new_and_pack(
	    gtk_hbox_new(FALSE, 0), vbox, "",
	    FALSE, FALSE, NH_PAD);
	if(index(query, '*')){
	    q = nh_gtk_new_and_pack(
		gtk_button_new_with_label("List"), hbox, "",
		FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(
		GTK_OBJECT(q), "clicked",
		GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'?');
	    q = nh_gtk_new_and_pack(
		gtk_button_new_with_label("All List"), hbox, "",
		FALSE, FALSE, NH_PAD);
	    gtk_signal_connect(
		GTK_OBJECT(q), "clicked",
		GTK_SIGNAL_FUNC(yn_clicked), (gpointer)'*');
	}
	else if (strstr(query, "In what direction")){	/* maybe direction */
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
	    
	    table = nh_gtk_new_and_pack(
		gtk_table_new(4, 3, TRUE), hbox, "",
		FALSE, FALSE, NH_PAD);
	    for(i=0 ; i<4 ; ++i)
		for(j=0 ; j<3 ; ++j){
		    d = nh_gtk_new_and_attach(
			gtk_button_new_with_label(
			    (iflags.num_pad ? np_dirstr[i][j].str : dirstr[i][j].str)),
			table, "",
			j, j+1,
			i, i+1);
		    gtk_signal_connect(
			GTK_OBJECT(d), "clicked",
			GTK_SIGNAL_FUNC(yn_clicked), 
			(iflags.num_pad ? (gpointer)np_dirstr[i][j].key : 
			    (gpointer)dirstr[i][j].key));
		}
	    yn_isdir = TRUE;
	}
    }

    gtk_grab_add(window);
    gtk_widget_show_all(window);
    nh_gtk_perm_invent_hack();
    
    gtk_main();

    if(hid > 0){
	gtk_widget_unmap(window);
	gtk_signal_disconnect(GTK_OBJECT(window), hid);

	gtk_widget_destroy(frame);
	if(clist)
	    gtk_widget_destroy(clist);
	gtk_widget_destroy(window);
    }

    return keysym;
}

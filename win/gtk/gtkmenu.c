/*
  $Id: gtkmenu.c,v 1.1 2000-08-15 19:55:13 wacko Exp $
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

static int	cancelled;
static int	n_menuitem;
static int	c_menuitem;
static int	n_select;

static int	selmode;

static int 	keysym;

typedef struct _NHMenuItem{
     GtkWidget	*clist;
     int	ch;
     int	gch;
     int	selected;
     ANY_P	id;
} NHMenuItem;

static NHMenuItem	nhMenuItem[100];	/* may be enough */
extern NHWindow		gtkWindows[];

static gint
menu_destroy(GtkWidget *widget, gpointer data)
{
    NHWindow *w = (NHWindow *)data;

    keysym = '\033';
    cancelled = 1;
    
    gtk_main_quit();
    
    w->w = NULL;
    return FALSE;
}

static gint
menu_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    keysym = map_menu_cmd(nh_keysym(event));
    if(event->keyval == GDK_Escape){
	keysym = '\033';
	cancelled = 1;
    }

    if(keysym)
	gtk_main_quit();
    
    return FALSE;
}

static gint
menu_button_clicked(GtkWidget *widget, gpointer data)
{
     keysym = (int)data;

     if(keysym == '\033')
	  cancelled = 1;

     if(keysym)
	  gtk_main_quit();

     return FALSE;
}

static gint
menu_selected(GtkWidget *clist, gint row, gint column,
	      GdkEventButton *event, gpointer data)
{
     nhMenuItem[row].selected = 1;
     ++n_select;
/*
     if(selmode == PICK_ONE)
	 gtk_main_quit();
*/
     return FALSE;
}

static gint
menu_unselected(GtkWidget *clist, gint row, gint column,
	      GdkEventButton *event, gpointer data)
{
     nhMenuItem[row].selected = 0;
     --n_select;

     return FALSE;
}

void
GTK_start_menu(winid id)
{
     NHWindow *w;
     GtkWidget *b;

     w = &gtkWindows[id];

     n_menuitem = 0;
     c_menuitem = 'a';

     w->w = gtk_window_new(GTK_WINDOW_DIALOG);
     gtk_widget_set_name(GTK_WIDGET(w->w), "fixed font");
     gtk_window_set_position(GTK_WINDOW(w->w), GTK_WIN_POS_MOUSE);
     gtk_signal_connect(GTK_OBJECT(w->w), "key_press_event",
			GTK_SIGNAL_FUNC(menu_key_press), NULL);
     w->hid = gtk_signal_connect(
	 GTK_OBJECT(w->w), "destroy",
	 GTK_SIGNAL_FUNC(menu_destroy), w);

     w->frame = nh_gtk_new_and_add(
	 gtk_frame_new(NULL), w->w, "");

     w->vbox = nh_gtk_new_and_add(
	 gtk_vbox_new(FALSE, 0), w->frame, "");

     w->hbox = nh_gtk_new_and_pack(
	 gtk_hbox_new(FALSE, 0), w->vbox, "",
	 FALSE, FALSE, NH_PAD);

     b = w->button[0] = nh_gtk_new_and_pack(
	 gtk_button_new_with_label("ok"), w->hbox, "",
	 FALSE, FALSE, NH_PAD);
     gtk_signal_connect(GTK_OBJECT(b), "clicked",
			GTK_SIGNAL_FUNC(menu_button_clicked), (gpointer)'\n');

     b = w->button[1] = nh_gtk_new_and_pack(
	 gtk_button_new_with_label("cancel"), w->hbox, "",
	 FALSE, FALSE, NH_PAD);
     gtk_signal_connect(GTK_OBJECT(b), "clicked",
			GTK_SIGNAL_FUNC(menu_button_clicked), (gpointer)'\033');

     b = w->button[2] = nh_gtk_new_and_pack(
	 gtk_button_new_with_label("all"), w->hbox, "",
	 FALSE, FALSE, NH_PAD);
     gtk_signal_connect(GTK_OBJECT(b), "clicked",
			GTK_SIGNAL_FUNC(menu_button_clicked),
			(gpointer)MENU_SELECT_ALL);

     b = w->button[3] = nh_gtk_new_and_pack(
	 gtk_button_new_with_label("none"), w->hbox, "",
	 FALSE, FALSE, NH_PAD);
     gtk_signal_connect(GTK_OBJECT(b), "clicked",
			GTK_SIGNAL_FUNC(menu_button_clicked),
			(gpointer)MENU_UNSELECT_ALL);

     b = w->button[4] = nh_gtk_new_and_pack(
	 gtk_button_new_with_label("invert"), w->hbox, "",
	 FALSE, FALSE, NH_PAD);
     gtk_signal_connect(GTK_OBJECT(b), "clicked",
			GTK_SIGNAL_FUNC(menu_button_clicked),
			(gpointer)MENU_INVERT_ALL);

     w->hbox2 = nh_gtk_new_and_pack(
	 gtk_hbox_new(FALSE, 0), w->vbox, "",
	 FALSE, FALSE, NH_PAD);

     w->adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
     w->clist = nh_gtk_new_and_pack(
	 gtk_clist_new(3), w->hbox2, "",
	 FALSE, FALSE, NH_PAD);

     gtk_clist_set_vadjustment(GTK_CLIST(w->clist), w->adj);
     gtk_clist_set_selection_mode(GTK_CLIST(w->clist), GTK_SELECTION_EXTENDED);

     gtk_clist_set_shadow_type(GTK_CLIST(w->clist), GTK_SHADOW_ETCHED_IN);
     gtk_clist_set_column_auto_resize(GTK_CLIST(w->clist), 0, TRUE);
     gtk_clist_set_column_auto_resize(GTK_CLIST(w->clist), 1, TRUE);
     gtk_clist_set_column_auto_resize(GTK_CLIST(w->clist), 2, TRUE);

     w->scrolled = nh_gtk_new_and_pack(
	 gtk_vscrollbar_new(GTK_CLIST(w->clist)->vadjustment), w->hbox2, "",
	 FALSE, FALSE, NH_PAD);

     gtk_signal_connect(GTK_OBJECT(w->clist), "select_row",
			GTK_SIGNAL_FUNC(menu_selected), (gpointer)w);
     gtk_signal_connect(GTK_OBJECT(w->clist), "unselect_row",
			GTK_SIGNAL_FUNC(menu_unselected), (gpointer)w);

     w->hbox3 = nh_gtk_new_and_pack(
	 gtk_hbox_new(FALSE, 0), w->vbox, "",
	 FALSE, FALSE, NH_PAD);

     b = w->button[5] = nh_gtk_new_and_pack(
	 gtk_button_new_with_label("Close"), w->hbox3, "",
	 TRUE, FALSE, 0);
     gtk_signal_connect(GTK_OBJECT(b), "clicked",
			GTK_SIGNAL_FUNC(menu_button_clicked), (gpointer)'\n');

     w->n_button = 6;
}

void
GTK_add_menu(winid id, int glyph, const ANY_P *identifier,
	     CHAR_P ch,CHAR_P gch ,int attr ,const char *str, 
	     BOOLEAN_P preselected)
{
     GtkWidget	*c;
     NHWindow	*w;
     char 	buf[2], buf2[2];
     gchar	*text[3];

     if(!str || str[0] == '\0')
	  return;

     w = &gtkWindows[id];

     c = w->clist;

     if(identifier->a_void && !ch){
	  ch = c_menuitem++;
	  if(c_menuitem > 'z')
	       c_menuitem = 'A';
     }

     nhMenuItem[n_menuitem].clist = c;
     nhMenuItem[n_menuitem].ch = ch;
     nhMenuItem[n_menuitem].gch = gch;
     nhMenuItem[n_menuitem].selected = FALSE;
     nhMenuItem[n_menuitem].id = *identifier;

     sprintf(buf, "%c", ch);
     text[0] = buf;

     if(gch){
	  sprintf(buf2, "%c", gch);
	  text[1] = buf2;
     }	  
     else
	  text[1] = "";

     text[2] = (gchar *)str;

     gtk_clist_append(GTK_CLIST(c), text);
     if(!identifier->a_void){
	  gtk_clist_set_selectable(GTK_CLIST(c), n_menuitem, FALSE);
	  if(attr != 0)
	      gtk_clist_set_background(GTK_CLIST(c), n_menuitem, c->style->dark);
     }
     
     ++n_menuitem;
}

void
GTK_end_menu(winid id, const char *prompt)
{
     NHWindow	*w;

     w = &gtkWindows[id];

     if(prompt)
	  gtk_frame_set_label(GTK_FRAME(w->frame), prompt);
}

int 
GTK_select_menu(winid id, int how, MENU_ITEM_P **menu_list)
{
    int	i;
    int 	n;
    NHWindow	*w;
    NHMenuItem *item;
/*
    GtkAdjustment *a, *aa;
    */

    n = 0;
    n_select = 0;
    selmode = how;

    w = &gtkWindows[id];
    *menu_list = 0;

    cancelled = 0;

    if(how != PICK_ANY){
	gtk_clist_set_selection_mode(GTK_CLIST(w->clist), GTK_SELECTION_SINGLE);
	gtk_widget_set_sensitive(w->button[1], FALSE);
	gtk_widget_set_sensitive(w->button[2], FALSE);
	gtk_widget_set_sensitive(w->button[3], FALSE);
	gtk_widget_set_sensitive(w->button[4], FALSE);
    }
    
    {
	extern int root_height;
	int height = w->clist->requisition.height;
	
	if(height >= (2 * root_height) / 3)
	    gtk_widget_set_usize(w->clist, -1, (2 * root_height) / 3);
    }
    
    gtk_grab_add(w->w);
    gtk_widget_show_all(w->w);
    
    gtk_main();
/*    
    a = GTK_CLIST(w->clist)->vadjustment;
    aa = GTK_CLIST(w->clist)->hadjustment;
    */
    while(1){
	if(keysym == '\033' || keysym == '\n' || keysym == ' ')
	    break;

	if(how == PICK_ONE && n_select == 1)
	    break;

	if(how != PICK_NONE){
	    for(i=0 ; i<n_menuitem ; ++i){
		item = &nhMenuItem[i];
		if(how == PICK_ANY){
		    if(keysym == MENU_INVERT_PAGE || keysym == MENU_INVERT_ALL){
			if(item->id.a_void){
			    if(item->selected){
				gtk_clist_unselect_row(
				    GTK_CLIST(item->clist),
				    i, 0);
			    }
			    else{
				gtk_clist_select_row(
				    GTK_CLIST(item->clist),
				    i, 0);
			    }
			}
		    }
		    if(keysym == MENU_UNSELECT_PAGE || keysym == MENU_UNSELECT_ALL){
			if(item->id.a_void){
			    gtk_clist_unselect_row(
				GTK_CLIST(item->clist),
				i, 0);
			}
		    }
		    if(keysym == MENU_SELECT_PAGE || keysym == MENU_SELECT_ALL){
			if(item->id.a_void){
			    gtk_clist_select_row(
				GTK_CLIST(item->clist),
				i, 0);
			}
		    }
		    else if(item->gch == keysym){
			if(item->selected){
			    gtk_clist_unselect_row(
				GTK_CLIST(item->clist),
				i, 0);
			}
			else{
			    gtk_clist_select_row(
				GTK_CLIST(item->clist),
				i, 0);
			}
		    }
		}
		if(item->ch == keysym){
		    if(item->selected){
			gtk_clist_unselect_row(
			    GTK_CLIST(item->clist),
			    i, 0);
		    }
		    else{
			gtk_clist_select_row(
			    GTK_CLIST(item->clist),
			    i, 0);
		    }
		    if(how == PICK_ONE)
			goto loopout;
		}
	    }
	}
	
	gtk_main();
    }
 loopout:
    
    GTK_destroy_nhwindow(id);
    
    if(cancelled)
	return 0;
    
    for(i=0 ; i<n_menuitem ; ++i)
	if(nhMenuItem[i].selected && nhMenuItem[i].id.a_void)
	    ++n;
    
    if(n > 0){
	*menu_list = (menu_item *) alloc(n * sizeof(menu_item));
	
	n = 0;
	for(i=0 ; i<n_menuitem ; ++i)
	    if(nhMenuItem[i].selected && nhMenuItem[i].id.a_void){
		(*menu_list)[n].item = nhMenuItem[i].id;
		(*menu_list)[n].count = -1;
		++n;
	    }
    }
    
    return n;
}

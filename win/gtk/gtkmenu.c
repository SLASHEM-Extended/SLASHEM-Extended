/*
  $Id: gtkmenu.c,v 1.17 2001-10-15 06:26:32 j_ali Exp $
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
#ifdef SHORT_FILENAMES
#include "patchlev.h"
#else
#include "patchlevel.h"
#endif

#ifdef WINGTK_MENU_IMAGES
#define MENU_COLS	4
#else
#define MENU_COLS	3
#endif

typedef struct _NHMenuItem{
     int	ch;
     int	gch;
     int	selected;
     long	count;
     ANY_P	id;
     int	attr;
     int	glyph;
} NHMenuItem;

extern NHWindow		gtkWindows[];
extern GtkAccelGroup	*accel_group;

static void move_menu(struct menu *src_menu, struct menu *dest_menu);
static void free_menu(struct menu *m);
static void GTK_load_menu_clist(NHWindow *w);
static gint perm_invent_key_press(GtkWidget *widget, GdkEventKey *event,
  gpointer data);

static gint
menu_destroy(GtkWidget *widget, gpointer data)
{
    NHWindow *w = (NHWindow *)data;

    if (!w->menu_information->cancelled){
	w->menu_information->keysym = '\033';
	w->menu_information->cancelled = 1;
	gtk_main_quit();
    }
    
    w->menu_information->valid_widgets = FALSE;
    w->w = NULL;
    return FALSE;
}

static gint
menu_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    NHWindow *w = (NHWindow *)data;
    struct menu_info_t *menu_info = w->menu_information;

    menu_info->keysym = map_menu_cmd(nh_keysym(event));
    if (event->keyval == GDK_Escape) {
	menu_info->keysym = '\033';
	menu_info->cancelled = 1;
    }

    if (menu_info->keysym)
	gtk_main_quit();
    
    return FALSE;
}

static gint
menu_button_clicked(GtkWidget *widget, gpointer data)
{
    int i, j;
    NHWindow *w = NULL;
    
    for(i = 0; i < MAXWIN; i++)
	if (gtkWindows[i].type == NHW_MENU)
	    for(j = 0; j < gtkWindows[i].n_button; j++)
		if (widget == gtkWindows[i].button[j]) {
		    w = &gtkWindows[i];
		    break;
		}
    if(!w)
	panic("menu_button_clicked: Can't find window");

    w->menu_information->keysym = (int)data;

    if(w->menu_information->keysym == '\033')
	w->menu_information->cancelled = 1;

    if(w->menu_information->keysym)
	gtk_main_quit();

    return FALSE;
}

static gint
menu_selected(GtkWidget *clist, gint row, gint column,
	      GdkEventButton *event, gpointer data)
{
    gchar buf[4];
    NHWindow *w = (NHWindow *)data;
    struct menu_info_t *menu_info = w->menu_information;

    menu_info->curr_menu.nhMenuItem[row].selected = 1;
    ++menu_info->n_select;
    buf[0] = menu_info->curr_menu.nhMenuItem[row].count >= 0 ? '#' : '-';
    if (menu_info->curr_menu.nhMenuItem[row].gch) {
	buf[1] = ' ';
	buf[2] = menu_info->curr_menu.nhMenuItem[row].gch;
	buf[3] = '\0';
    }
    else
	buf[1] = '\0';
    gtk_clist_set_text(GTK_CLIST(clist), row, MENU_COLS - 2, buf);
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
    gchar buf[4];
    NHWindow *w = (NHWindow *)data;
    struct menu_info_t *menu_info = w->menu_information;

    menu_info->curr_menu.nhMenuItem[row].selected = 0;
    --menu_info->n_select;
    if (menu_info->curr_menu.nhMenuItem[row].gch)
	sprintf(buf, "- %c", menu_info->curr_menu.nhMenuItem[row].gch);
    else
	strcpy(buf, "-");
    gtk_clist_set_text(GTK_CLIST(clist), row, MENU_COLS - 2, buf);
    return FALSE;
}

void
GTK_start_menu(winid id)
{
    NHWindow *w;
    struct menu_info_t *menu_info;

    w = &gtkWindows[id];
    if (w->type != NHW_MENU)
	panic("GTK_start_menu: Window id (%d) is not a menu (type %d)",
	  id, w->type);
    menu_info = w->menu_information;

    /* make sure we're starting with a clean slate */
    menu_info->new_menu.n_menuitem = 0;

    if (!menu_info->new_menu.nhMenuItem)
    {
	menu_info->new_menu.alloc_menuitem = 32;	/* Arbitary */
	menu_info->new_menu.nhMenuItem = (NHMenuItem *)
	  malloc((unsigned)(sizeof(NHMenuItem) *
	  menu_info->new_menu.alloc_menuitem));
    }
    if (!menu_info->new_menu.nhMenuItem)
    {
	menu_info->new_menu.alloc_menuitem = 1;		/* Minimum */
	menu_info->new_menu.nhMenuItem =
	  (NHMenuItem *) malloc(sizeof(NHMenuItem));
    }
    if (!menu_info->new_menu.nhMenuItem)
	panic("GTK_start_menu: Memory allocation failure; cannot get %u bytes",
	  sizeof(NHMenuItem));
    menu_info->new_menu.n_menuitem = 0;
    menu_info->new_menu.c_menuitem = 'a';
    menu_info->new_menu.clist = GTK_CLIST(gtk_clist_new(MENU_COLS));
}

static void
GTK_init_menu_widgets(NHWindow *w)
{
     int i;
     GtkWidget *b;

     if (w->menu_information->cancelled >= 0)
	 w->w = gtk_window_new(GTK_WINDOW_DIALOG);
     else
	 w->w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
     gtk_widget_set_name(GTK_WIDGET(w->w), "fixed font");
     nh_position_popup_dialog(GTK_WIDGET(w->w));
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

     if (w->menu_information->cancelled >= 0) {
	 gtk_signal_connect(GTK_OBJECT(w->w), "key_press_event",
			    GTK_SIGNAL_FUNC(menu_key_press), w);
	 b = w->button[0] = nh_gtk_new_and_pack(
	     gtk_button_new_with_label("ok"), w->hbox, "",
	     FALSE, FALSE, NH_PAD);
	 gtk_signal_connect(GTK_OBJECT(b), "clicked",
		    	    GTK_SIGNAL_FUNC(menu_button_clicked),
			    (gpointer)'\n');

	 b = w->button[1] = nh_gtk_new_and_pack(
	     gtk_button_new_with_label("cancel"), w->hbox, "",
	     FALSE, FALSE, NH_PAD);
	 gtk_signal_connect(GTK_OBJECT(b), "clicked",
		    	    GTK_SIGNAL_FUNC(menu_button_clicked),
			    (gpointer)'\033');

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
     }
     else
     {
	 gtk_signal_connect(GTK_OBJECT(w->w), "key_press_event",
			    GTK_SIGNAL_FUNC(perm_invent_key_press), w);
	 gtk_accel_group_attach(accel_group, GTK_OBJECT(w->w));
     }
     w->hbox2 = nh_gtk_new_and_pack(
	 gtk_hbox_new(FALSE, 0), w->vbox, "",
	 FALSE, FALSE, NH_PAD);

     w->adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

     GTK_load_menu_clist(w);
     w->clist = nh_gtk_new_and_pack(
	 GTK_WIDGET(w->clist), w->hbox2, "", FALSE, FALSE, NH_PAD);

     gtk_clist_set_vadjustment(GTK_CLIST(w->clist), w->adj);
     gtk_clist_set_selection_mode(GTK_CLIST(w->clist), GTK_SELECTION_EXTENDED);

     gtk_clist_set_shadow_type(GTK_CLIST(w->clist), GTK_SHADOW_ETCHED_IN);
     for(i = 0; i < MENU_COLS; i++)
	 gtk_clist_set_column_auto_resize(GTK_CLIST(w->clist), i, TRUE);
#ifdef WINGTK_MENU_IMAGES
     if (w->menu_information->pixmaps &&
       GTK_CLIST(w->clist)->row_height < nh_tile_3dheight())
	gtk_clist_set_row_height(GTK_CLIST(w->clist), nh_tile_3dheight());
#endif

     w->scrolled = nh_gtk_new_and_pack(
	 gtk_vscrollbar_new(GTK_CLIST(w->clist)->vadjustment), w->hbox2,
	 "", FALSE, FALSE, NH_PAD);

     if (w->menu_information->cancelled >= 0) {
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
			    GTK_SIGNAL_FUNC(menu_button_clicked),
			    (gpointer)'\n');

	 w->n_button = 6;
     }
     else
	 w->n_button = 0;

     if(w->menu_information->curr_menu.prompt)
	  gtk_frame_set_label(GTK_FRAME(w->frame),
	    w->menu_information->curr_menu.prompt);
    
     w->menu_information->valid_widgets = TRUE;

     /* Find our "ancestor" pointer. This pointer would normally be NULL,
      * but we set it to point at grab widgets to implement a type of
      * partial modality. Warning: this is a hack.
      */
     for(b = w->w; b->parent; b = b->parent)
	 ;
     w->menu_information->ancestor = &b->parent;
}

static void
GTK_destroy_menu_widgets(NHWindow *w)
{
    if(w->w && w->menu_information->valid_widgets){
	gtk_widget_hide_all(w->w);
	if(w->hid > 0)
	    gtk_signal_disconnect(GTK_OBJECT(w->w), w->hid);

	gtk_widget_destroy(w->w);
	w->w = NULL;
	w->menu_information->valid_widgets = FALSE;
    }
}

static void
GTK_load_menu_clist(NHWindow *w)
{
    int i, j;
    GtkCList *c;
    GtkStyle *s;
    struct menu *menu;
    gchar *text[MENU_COLS];
    
    if (w->menu_information->valid_widgets)
	c = GTK_CLIST(w->clist);
    else
    {
	c = GTK_CLIST(gtk_clist_new(MENU_COLS));
	w->clist = GTK_WIDGET(c);
    }
    s = gtk_rc_get_style(w->clist);
    if (!s)
	s = w->clist->style;
    gtk_clist_freeze(c);
    gtk_clist_clear(c);
    menu = &w->menu_information->curr_menu;
#ifdef WINGTK_MENU_IMAGES
    w->menu_information->pixmaps = FALSE;
#endif
    for(j = 0; ; j++) {
    	
	for(i = 0; i < MENU_COLS; i++)
	    if (!gtk_clist_get_text(menu->clist, j, i, &text[i]))
		break;
	if (i < MENU_COLS)
	    break;
	gtk_clist_append(c, text);
	if(!menu->nhMenuItem[j].id.a_void){
	    gtk_clist_set_selectable(c, j, FALSE);
	    if(menu->nhMenuItem[j].attr == ATR_INVERSE)
		gtk_clist_set_background(c, j, s->dark + GTK_STATE_NORMAL);
	    switch(menu->nhMenuItem[j].attr) {
		case ATR_BOLD:
		    gtk_clist_set_foreground(c, j, &s->black);
		    break;
		case ATR_DIM:
		    gtk_clist_set_foreground(c, j, s->light + GTK_STATE_NORMAL);
		    break;
		default:
		    gtk_clist_set_foreground(c, j, s->text + GTK_STATE_NORMAL);
		    break;
	    }
	}
#ifdef WINGTK_MENU_IMAGES
	if(menu->nhMenuItem[j].glyph != NO_GLYPH && map_visual){
	    GdkPixmap *pixmap;
	    pixmap = GTK_glyph_to_gdkpixmap(menu->nhMenuItem[j].glyph);
	    if (pixmap) {
		gtk_clist_set_pixmap(c, j, 0, pixmap, NULL);
		gdk_pixmap_unref(pixmap);
		w->menu_information->pixmaps = TRUE;
	    }
	}
#endif
    }
    /* Inventory window doesn't really look good if it's completely empty */
    if (w-gtkWindows == WIN_INVEN && !j) {
	if (u.ugold)
	    text[0] = "Not carrying anything except gold.";
	else
	    text[0] = "Not carrying anything.";
	for(i = 1; i < MENU_COLS; i++)
	    text[i] = "";
	gtk_clist_append(c, text);
    }
    gtk_clist_thaw(c);
}

void
GTK_add_menu(winid id, int glyph, const ANY_P *identifier,
	     CHAR_P ch,CHAR_P gch ,int attr ,const char *str, 
	     BOOLEAN_P preselected)
{
     GtkCList	*c;
     NHWindow	*w;
     struct menu *menu;
     char 	buf[2], buf2[4];
     gchar	*text[MENU_COLS];

     if(!str || str[0] == '\0')
	  return;

     w = &gtkWindows[id];
     menu = &w->menu_information->new_menu;

     c = menu->clist;

     if(identifier->a_void && !ch){
	  ch = menu->c_menuitem++;
	  if(ch == 'z')
	       menu->c_menuitem = 'A';
	  else if(ch == 'Z')
	       menu->c_menuitem = 0;
     }

     if (menu->n_menuitem >= menu->alloc_menuitem)
     {
	NHMenuItem *new;
	new = (NHMenuItem *) realloc((genericptr_t)menu->nhMenuItem,
	  (unsigned)(sizeof(NHMenuItem) * 2 * menu->alloc_menuitem));
	if (new)
	    menu->alloc_menuitem *= 2;
	else
	    new = (NHMenuItem *) realloc((genericptr_t)menu->nhMenuItem,
	      (unsigned)(sizeof(NHMenuItem) * ++menu->alloc_menuitem));
	if (!new)
	    panic("GTK_add_menu: Memory allocation failure; cannot get %u bytes",
	      sizeof(NHMenuItem));
	menu->nhMenuItem = new;
     }

     menu->nhMenuItem[menu->n_menuitem].ch = ch;
     menu->nhMenuItem[menu->n_menuitem].gch = gch;
     menu->nhMenuItem[menu->n_menuitem].selected = FALSE;
     menu->nhMenuItem[menu->n_menuitem].count = -1;
     menu->nhMenuItem[menu->n_menuitem].id = *identifier;
     menu->nhMenuItem[menu->n_menuitem].attr = attr;
     menu->nhMenuItem[menu->n_menuitem].glyph = glyph;

     text[0] = "";

     sprintf(buf, "%c", ch);
     text[MENU_COLS-3] = buf;

     if(gch){
	  sprintf(buf2, "- %c", gch);
	  text[MENU_COLS-2] = buf2;
     }	  
     else
	  text[MENU_COLS-2] = identifier->a_void ? "-" : "";

     text[MENU_COLS-1] = (gchar *)str;

     gtk_clist_append(c, text);
     
     ++menu->n_menuitem;
}

void
GTK_end_menu(winid id, const char *prompt)
{
     NHWindow	*w;
     NHMenuItem *new;
     struct menu *menu;

     w = &gtkWindows[id];
     menu = &w->menu_information->new_menu;

     new = (NHMenuItem *) realloc((genericptr_t)menu->nhMenuItem,
      (unsigned)(sizeof(NHMenuItem) * menu->n_menuitem));
     if (new)
     {
          menu->alloc_menuitem = menu->n_menuitem;
	  menu->nhMenuItem = new;
     }

     menu->prompt = prompt;
}

static gint
ancestor_destroy(GtkWidget *widget, gpointer data)
{
    NHWindow *w = (NHWindow *)data;
    struct menu_info_t *menu_info = w->menu_information;
    *(menu_info->ancestor) = NULL;
    return 0;
}

static gint
perm_invent_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    NHWindow *w = (NHWindow *)data;

    /*
     * If we're faking being a descendant of a window it's because that
     * window has a grab active. In this case all key press events should
     * be going to the grab widget.
     */
    if (w->menu_information && *(w->menu_information->ancestor)) {
	/* As Gtk+ internal function gtk_propagate_event() */
	widget = gtk_widget_get_ancestor(*(w->menu_information->ancestor),
	  GTK_TYPE_WINDOW);
	if (widget) {
	    if (GTK_WIDGET_IS_SENSITIVE(widget))
		gtk_widget_event(widget, (GdkEvent *)event);
	    return 1;
	}
	else
	    return 0;
    }
    else
	return GTK_default_key_press(widget, event, data);
}

/*
 * Call this function after changing the grab to retain the ability to
 * scroll the permanent inventory window.
 */

void
nh_gtk_perm_invent_hack(void)
{
    NHWindow *w = &gtkWindows[WIN_INVEN];
    GtkWidget *grab;
    if (!w->w)
	return;			/* Inventory window not open */
    grab = gtk_grab_get_current();
    *(w->menu_information->ancestor) = NULL;
    if (grab && !gtk_widget_is_ancestor(w->w, grab)) {
	/* If there is a grab in effect the user will not be able to
	 * scroll the permanent inventory window. The real solution
	 * is not to use modal windows (see gtk2 for an implementation
	 * of this) but this is very hard to do in Gtk+ v1.2.x so
	 * instead we temporarily hack the inventory window to appear
	 * to be a descendant of the current grab. We automatically
	 * undo this if the grab window is destroyed.
	 */
	*(w->menu_information->ancestor) = grab;
	gtk_signal_connect(GTK_OBJECT(grab), "destroy",
	  GTK_SIGNAL_FUNC(ancestor_destroy), w);
    }
}

int 
GTK_select_menu(winid id, int how, MENU_ITEM_P **menu_list)
{
    int	i;
    int 	n;
    NHWindow	*w;
    struct menu_info_t *menu_info;
    NHMenuItem	*item;
    GtkCList	*c;
/*
    GtkAdjustment *a, *aa;
    */

    n = 0;

    w = &gtkWindows[id];
    menu_info = w->menu_information;
    menu_info->n_select = 0;
    menu_info->selmode = how;
    menu_info->count = -1;
    *menu_list = 0;

    if (id == WIN_INVEN)
    {
	if (menu_info->cancelled < 0 && !flags.perm_invent ||
	    menu_info->cancelled == 0 && flags.perm_invent)
	    GTK_destroy_menu_widgets(w);	/* perm_invent changed */
	menu_info->cancelled = flags.perm_invent ? -1 : 0;
    }
    else
	menu_info->cancelled = 0;

    /* make new menu the current menu */
    if (menu_info->new_menu.n_menuitem)
	move_menu(&menu_info->new_menu, &menu_info->curr_menu);

    if (!menu_info->valid_widgets)
	GTK_init_menu_widgets(w);
    else
	GTK_load_menu_clist(w);

    if (id == WIN_INVEN && flags.perm_invent)
	gtk_window_set_title(GTK_WINDOW(w->w), DEF_GAME_NAME " Inventory");
    else
	gtk_window_set_title(GTK_WINDOW(w->w), DEF_GAME_NAME " Menu");
	
    if(how != PICK_ANY){
	gtk_clist_set_selection_mode(GTK_CLIST(w->clist),
	  GTK_SELECTION_SINGLE);
	if (w->n_button>=4) {
	    gtk_widget_set_sensitive(w->button[1], FALSE);
	    gtk_widget_set_sensitive(w->button[2], FALSE);
	    gtk_widget_set_sensitive(w->button[3], FALSE);
	    gtk_widget_set_sensitive(w->button[4], FALSE);
	}
    }
    
    {
	extern int root_height;
	int height = w->clist->requisition.height;
	
	if(height >= (2 * root_height) / 3)
	    gtk_widget_set_usize(w->clist, -1, (2 * root_height) / 3);
    }
    
    gtk_widget_show_all(w->w);
    
    if (menu_info->cancelled < 0) {
	gdk_window_raise(w->w->window);
	return 0;
    }
    gtk_grab_add(w->w);
    gtk_widget_grab_focus(w->clist);

    c = GTK_CLIST(w->clist);
/*    
    a = GTK_CLIST(w->clist)->vadjustment;
    aa = GTK_CLIST(w->clist)->hadjustment;
    */
    while(1){
	gtk_main();
	if(menu_info->keysym == '\033' || menu_info->keysym == '\n' ||
	  menu_info->keysym == ' ')
	    break;

	if(how == PICK_ONE && menu_info->n_select == 1)
	    break;

	if(how != PICK_NONE){
	    for(i=0 ; i<menu_info->curr_menu.n_menuitem ; ++i){
		item = &menu_info->curr_menu.nhMenuItem[i];
		if(how == PICK_ANY){
		    if(menu_info->keysym == MENU_INVERT_PAGE ||
		      menu_info->keysym == MENU_INVERT_ALL){
			if(item->id.a_void){
			    if(item->selected)
				gtk_clist_unselect_row(c, i, 0);
			    else {
				menu_info->curr_menu.nhMenuItem[i].count = -1;
				gtk_clist_select_row(c, i, 0);
			    }
			}
		    }
		    if(menu_info->keysym == MENU_UNSELECT_PAGE ||
		      menu_info->keysym == MENU_UNSELECT_ALL){
			if(item->id.a_void)
			    gtk_clist_unselect_row(c, i, 0);
		    }
		    if(menu_info->keysym == MENU_SELECT_PAGE ||
		      menu_info->keysym == MENU_SELECT_ALL){
			if (item->id.a_void) {
			    menu_info->curr_menu.nhMenuItem[i].count = -1;
			    gtk_clist_select_row(c, i, 0);
			}
		    }
		    else if(item->gch == menu_info->keysym){
			if(item->selected)
			    gtk_clist_unselect_row(c, i, 0);
			else {
			    menu_info->curr_menu.nhMenuItem[i].count = -1;
			    gtk_clist_select_row(c, i, 0);
			}
		    }
		}
		if(item->ch == menu_info->keysym){
		    if (item->selected && menu_info->count < 0)
			gtk_clist_unselect_row(c, i, 0);
		    else {
			menu_info->curr_menu.nhMenuItem[i].count =
			  menu_info->count;
			gtk_clist_select_row(c, i, 0);
		    }
		    if(how == PICK_ONE)
			goto loopout;
		}
	    }
	    if (menu_info->keysym >= '0' && menu_info->keysym <= '9') {
		if (menu_info->count > 0)
		    menu_info->count *= 10;
		else
		    menu_info->count = 0;
		menu_info->count += menu_info->keysym - '0';
		if (menu_info->count <= 0)
		    menu_info->count = -1;
	    }
	    else
		menu_info->count = -1;
	}
    }
 loopout:
    
    GTK_destroy_menu_widgets(w);
    
    if(menu_info->cancelled)
	return -1;
    
    for(i=0 ; i<menu_info->curr_menu.n_menuitem ; ++i)
	if(menu_info->curr_menu.nhMenuItem[i].selected &&
	  menu_info->curr_menu.nhMenuItem[i].id.a_void)
	    ++n;
    
    if(n > 0){
	*menu_list = (menu_item *) alloc(n * sizeof(menu_item));
	
	n = 0;
	for(i=0 ; i<menu_info->curr_menu.n_menuitem ; ++i) {
	    item = &menu_info->curr_menu.nhMenuItem[i];
	    if (item->selected && item->id.a_void) {
		(*menu_list)[n].item = item->id;
		(*menu_list)[n].count = item->count;
		++n;
	    }
	}
    }
    
    return n;
}

static void
move_menu(struct menu *src_menu, struct menu *dest_menu)
{
    free_menu(dest_menu);	/* toss old menu */
    *dest_menu = *src_menu;	/* make new menu current */
    /* leave no dangling pointers */
    src_menu->clist = (GtkCList *)0;
    src_menu->nhMenuItem = (NHMenuItem *)0;
    src_menu->alloc_menuitem = 0;
    src_menu->n_menuitem = 0;
}

static void
free_menu(struct menu *m)
{
    if (m->clist) {
	gtk_widget_unref(GTK_WIDGET(m->clist));
	m->clist = (GtkCList *)0;
    }
    free((genericptr_t) m->nhMenuItem);
    m->nhMenuItem = (NHMenuItem *)0;
    m->alloc_menuitem = 0;
    m->n_menuitem = 0;
}

void
GTK_create_menu_window(NHWindow *w)
{
    w->menu_information =
		(struct menu_info_t *) alloc(sizeof(struct menu_info_t));
    (void) memset((genericptr_t) w->menu_information, '\0',
						sizeof(struct menu_info_t));
}

void
GTK_destroy_menu_window(NHWindow *w)
{
    GTK_destroy_menu_widgets(w);
    free_menu(&w->menu_information->curr_menu);
    free_menu(&w->menu_information->new_menu);
    free((genericptr_t) w->menu_information);
    w->menu_information = (struct menu_info_t *) 0;
}

void
GTK_unmap_menu_window(NHWindow *w)
{
    if (w && w->w) /* FIXME:  This shouldn't be necessary, but is */
    	gtk_widget_unmap(w->w);
}

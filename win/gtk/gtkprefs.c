/*
  $Id: gtkprefs.c,v 1.4 2003-08-21 19:00:24 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2000-2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <math.h>
#include <gtk/gtk.h>
#include "winGTK.h"
#include "patchlevel.h"

#ifdef WINGTK_RADAR
static GtkWidget *radar_button;
#endif
static GtkWidget *font_selection_map;
static GtkWidget *map_clip_spinbutton;

static void GTK_pref_cancel(GtkWidget *w, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(data));
}

static void GTK_pref_apply(GtkWidget *w, gpointer data)
{
    int clip_dist;
    gchar *font_name;
    font_name = gtk_font_selection_get_font_name(
      GTK_FONT_SELECTION(font_selection_map));
    nh_set_map_font(font_name);
    g_free(font_name);
#ifdef WINGTK_RADAR
    nh_radar_set_use(
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radar_button)));
#endif
    clip_dist = gtk_spin_button_get_value_as_int(
      GTK_SPIN_BUTTON(map_clip_spinbutton));
    map_clip_dist2 = clip_dist * clip_dist;
    nh_map_check_visibility();
}

int
GTK_preferences_save(struct gtkhackrc *rc)
{
    gchar *map_font = nh_get_map_font();
    if (map_font)
	nh_gtkhackrc_store(rc, "map.font = \"%s\"", map_font);
    nh_gtkhackrc_store(rc, "radar = %d", nh_radar_get_use());
    nh_gtkhackrc_store(rc, "map.clip_dist2 = %d", map_clip_dist2);
}

static void GTK_pref_ok(GtkWidget *w, gpointer data)
{
    GTK_pref_apply(w,data);
    GTK_pref_cancel(w,data);
}

#ifdef WINGTK_RADAR
static GtkWidget *
GTK_pref_radar_new()
{
    radar_button = gtk_check_button_new_with_label("Display radar window");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radar_button),
      nh_radar_get_use());
    return radar_button;
}
#endif

static GtkWidget *
GTK_pref_general_new()
{
    GtkWidget *vbox;

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_border_width(GTK_CONTAINER(vbox), NH_PAD);

#ifdef WINGTK_RADAR
    nh_gtk_new_and_pack(GTK_pref_radar_new(), vbox, "",
      FALSE, FALSE, NH_PAD);
#endif

    return vbox;
}

static GtkWidget *
GTK_pref_font_new()
{
    GtkWidget *frame;
    GtkWidget *vbox;
    gchar *font_name;

    frame = gtk_frame_new("Font");

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    font_selection_map = nh_gtk_new_and_pack(gtk_font_selection_new(), vbox, "",      FALSE, FALSE, NH_PAD);

    font_name = nh_get_map_font();
    if (font_name) {
	(void)gtk_font_selection_set_font_name(
	  GTK_FONT_SELECTION(font_selection_map), font_name);
	g_free(font_name);
    }

    return frame;
}

static GtkWidget *
GTK_pref_map_new()
{
    GtkWidget *frame;
    GtkWidget *vbox,*hbox;
    gchar *font_name;

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_border_width(GTK_CONTAINER(vbox), NH_PAD);

    frame = nh_gtk_new_and_pack(gtk_frame_new("Automatic scrolling"), vbox, "",
      FALSE, FALSE, NH_PAD);

    hbox = nh_gtk_new_and_add(gtk_hbox_new(FALSE, 0), frame, "");

    nh_gtk_new_and_pack(gtk_label_new("Maximum allowable deviation:"), hbox, "",
      FALSE, FALSE, NH_PAD);

    map_clip_spinbutton = nh_gtk_new_and_pack(
      gtk_spin_button_new_with_range(0.0, 255.0, 1.0), hbox, "",
      FALSE, FALSE, NH_PAD);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(map_clip_spinbutton),
      (gdouble)(int)(sqrt(map_clip_dist2) + 0.5));

    nh_gtk_new_and_pack(GTK_pref_font_new(), vbox, "", FALSE, FALSE, NH_PAD);

    return vbox;
}

GtkWidget *
GTK_preferences_new()
{
    guint hid;	
    GtkWidget *w;
    GtkWidget *note;
    GtkWidget *vbox, *hbox;
    GtkWidget *ok;
    GtkWidget *apply;
    GtkWidget *cancel;

    w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(w), DEF_GAME_NAME " Preferences");
    gtk_window_set_destroy_with_parent(GTK_WINDOW(w), TRUE);
    gtk_container_border_width(GTK_CONTAINER(w), NH_PAD);
#if 0
    hid = gtk_signal_connect(GTK_OBJECT(w), "destroy",
      GTK_SIGNAL_FUNC(default_destroy), &hid);
#endif
    nh_position_popup_dialog(GTK_WIDGET(w));

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), w, "");

    note = nh_gtk_new_and_pack(gtk_notebook_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

    nh_gtk_new_and_add(GTK_pref_general_new(), note, "");
    nh_gtk_new_and_add(GTK_pref_map_new(), note, "");

    gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(note),
      gtk_notebook_get_nth_page(GTK_NOTEBOOK(note), 0), "General");

    gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(note),
      gtk_notebook_get_nth_page(GTK_NOTEBOOK(note), 1), "Map");

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

    ok = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_OK),
      hbox, "", FALSE, FALSE, NH_PAD);
    apply = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_APPLY),
      hbox, "", FALSE, FALSE, NH_PAD);
    cancel = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_CANCEL),
      hbox, "", FALSE, FALSE, NH_PAD);

    gtk_signal_connect(GTK_OBJECT(ok), "clicked",
      GTK_SIGNAL_FUNC(GTK_pref_ok), w);

    gtk_signal_connect(GTK_OBJECT(apply), "clicked",
      GTK_SIGNAL_FUNC(GTK_pref_apply), w);

    gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
      GTK_SIGNAL_FUNC(GTK_pref_cancel), w);

    gtk_widget_show_all(w);

    return w;
}

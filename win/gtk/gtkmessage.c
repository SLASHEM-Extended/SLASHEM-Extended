/*
  $Id: gtkmessage.c,v 1.10 2003-08-02 14:27:26 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
  GTK+ NetHack Copyright (c) Slash'EM Development Team 2000-2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include "winGTK.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

static GtkWidget *message_text;

extern int root_width;
extern int root_height;

#if GTK_CHECK_VERSION(2,0,0)
# define USE_TEXTVIEW
#else	/* GTK_CHECK_VERSION */
# ifdef WIN32
   /* ALI: Switching this on avoids strange scrolling effects (see bug 124233).
    */
#  define FROZEN_INSERT
# endif
#endif	/* GTK_CHECK_VERSION */

#ifdef USE_TEXTVIEW
GtkWidget *
nh_message_new()
{
    GtkWidget *message_h;
    GtkWidget *sw;
    GtkTextIter iter;
    GtkTextBuffer *t;

    message_h = gtk_handle_box_new();
    GTK_HANDLE_BOX(message_h)->shrink_on_detach = 1;
    
    message_text = gtk_text_view_new();
    gtk_widget_show(message_text);
    GTK_WIDGET_UNSET_FLAGS(message_text, GTK_CAN_FOCUS);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(message_text), GTK_WRAP_WORD);
    t = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_text));
    gtk_text_buffer_create_tag(t, "warning", "foreground", "red", NULL);
    gtk_text_buffer_get_end_iter(t, &iter);
    gtk_text_buffer_create_mark(t, "nh_end", &iter, FALSE);

    sw = nh_gtk_new_and_add(gtk_scrolled_window_new(NULL, NULL), message_h, "");
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(sw), message_text);
 
    return message_h;
}

void
nh_message_putstr(const char *str)
{
    int	len;
    char *buf;
    GtkTextIter iter, iter2;
    GtkTextBuffer *t;

    if (!message_text)
	return;

    t = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_text));
    gtk_text_buffer_get_end_iter(t, &iter);

    len = strlen(str);
    buf = (char *)alloc(len + 2);

    sprintf(buf, "\n%s", str);

    if (nh_status_in_trouble())
	gtk_text_buffer_insert_with_tags_by_name(t, &iter, buf, len + 1,
	  "warning", NULL);
    else
	gtk_text_buffer_insert(t, &iter, buf, len + 1);
    free(buf);
  
    len = gtk_text_buffer_get_char_count(t);
    if (len > NH_TEXT_REMEMBER) {
	gtk_text_buffer_get_iter_at_offset(t, &iter, len - NH_TEXT_REMEMBER);
	gtk_text_buffer_get_iter_at_line(t, &iter2,
	  gtk_text_iter_get_line(&iter) + 1);
	gtk_text_buffer_get_start_iter(t, &iter);
	gtk_text_buffer_delete(t, &iter, &iter2);
    }

    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(message_text),
      gtk_text_buffer_get_mark(t, "nh_end"));
}

#else	/* USE_TEXTVIEW */

GtkWidget *
nh_message_new()
{
    GtkWidget *message_h;
    GtkWidget *message_hbox;

    message_h = gtk_handle_box_new();
    GTK_HANDLE_BOX(message_h)->shrink_on_detach = 1;

    message_hbox = nh_gtk_new_and_add(gtk_hbox_new(FALSE, 0), message_h, "");

    message_text = nh_gtk_new_and_pack(gtk_text_new(NULL, NULL),
      message_hbox, "", TRUE, TRUE, NH_PAD);

    GTK_WIDGET_UNSET_FLAGS(message_text, GTK_CAN_FOCUS);
    gtk_text_set_word_wrap((GtkText *) message_text, TRUE);

    (void)nh_gtk_new_and_pack(gtk_vscrollbar_new(GTK_TEXT(message_text)->vadj),
      message_hbox, "", FALSE, FALSE, NH_PAD);

    return message_h;
}

void
nh_message_putstr(const char *str)
{
    int i;
    int	len;
    char *buf;
    GtkText *t;
#ifdef FROZEN_INSERT
    static int	freeze_count=0;
#endif

    if (!message_text)
	return;

    t = GTK_TEXT(message_text);

    len = strlen(str);
    buf = (char *)alloc(len + 2);

    sprintf(buf, "\n%s", str);

#ifdef FROZEN_INSERT
    /* ALI: gimpwin 20001226 looks very bad if you update a text widget without
     * freezing it (the text is displayed half-scrolled, with lines overlapping
     * each other). This is not ideal (the text is redrawn each thaw), but it
     * is an improvement. Due to a bug in gimpwin we can't trim text if we've
     * frozen the widget, thus every so often we don't freeze but trim instead.
     */
    if (++freeze_count >= 50)	/* Trim text every 50 inserts */
	freeze_count = 0;
    else
	gtk_text_freeze(t);
#endif

    if (nh_status_in_trouble())
	i = CLR_RED;
    else
	i = MAP_BLACK;
    gtk_text_insert(t, NULL, &nh_color[i], &nh_color[MAP_WHITE], buf, len + 1);

    len = gtk_text_get_length(t);
#ifdef FROZEN_INSERT
    if (!freeze_count && len > NH_TEXT_REMEMBER) {
#else
    if (len > NH_TEXT_REMEMBER) {
#endif
	gtk_text_freeze(t);
	for(i = 0; i < len && len > NH_TEXT_REMEMBER; i++)
	    if (GTK_TEXT_INDEX(t, i) == '\n') {
		i++;
		gtk_text_set_point(t, i);
		gtk_text_backward_delete(t, i);
		len -= i;
	    }
	gtk_text_set_point(t, len);
	gtk_text_thaw(t);
    }
#ifdef FROZEN_INSERT
    /* ALI: t->vadj->upper would be more correct, but causes gimpwin to crash */
    if (freeze_count) {
	gtk_adjustment_set_value(t->vadj, t->vadj->upper - 1);
	gtk_text_thaw(t);
    }
#endif

    free(buf);
}

#endif	/* USE_TEXTVIEW */

int
GTK_doprev_message()
{
    return 0;
}

void
nh_message_destroy(void)
{
    message_text = NULL;
}

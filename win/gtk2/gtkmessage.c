/*
  $Id: gtkmessage.c,v 1.3 2002-03-02 19:44:06 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
  GTK+ NetHack Copyright (c) Slash'EM Development Team 2000-2001
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include "winGTK.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

static GtkWidget	*message_h;
static GtkWidget	*message_hbox;
static GtkWidget	*message_text;
static GtkWidget	*message_scrollbar;

extern int		root_width;
extern int		root_height;

#ifdef WIN32
/* ALI: Switching this on avoids strange scrolling effects (see bug 124233).
 */
#define FROZEN_INSERT
#endif

GtkWidget *
nh_message_new()
{
    message_h = gtk_handle_box_new();
    GTK_HANDLE_BOX(message_h)->shrink_on_detach = 1;

    message_hbox = nh_gtk_new_and_add(gtk_hbox_new(FALSE, 0), message_h, "");
    
    message_text = nh_gtk_new_and_pack(
	gtk_text_new(NULL, NULL), message_hbox, "",
	TRUE, TRUE, NH_PAD);

    GTK_WIDGET_UNSET_FLAGS(message_text, GTK_CAN_FOCUS);
    gtk_text_set_word_wrap((GtkText *) message_text, TRUE);
  
    message_scrollbar = nh_gtk_new_and_pack(
	gtk_vscrollbar_new(GTK_TEXT(message_text)->vadj), message_hbox, "",
	FALSE, FALSE, NH_PAD);
    
    return message_h;
}

void
nh_message_putstr(const char *str)
{
  int		i;
  int		len;
  char		*buf;
  GtkText	*t;
#ifdef FROZEN_INSERT
  static int	freeze_count=0;
#endif

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
  if (++freeze_count>=50)	/* Trim text every 50 inserts */
    freeze_count=0;
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
  if(!freeze_count && len > NH_TEXT_REMEMBER){
#else
  if(len > NH_TEXT_REMEMBER){
#endif
    gtk_text_freeze(t);
    for(i=0 ; i<len && len > NH_TEXT_REMEMBER; ++i)
      if(GTK_TEXT_INDEX(t, i) == '\n'){
	++i;
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

int
GTK_doprev_message()
{
  return 0;
}

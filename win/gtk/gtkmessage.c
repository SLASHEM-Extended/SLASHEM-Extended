/*
  $Id: gtkmessage.c,v 1.4 2000-12-22 14:54:56 j_ali Exp $
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

static GtkWidget	*message_h;
static GtkWidget	*message_hbox;
static GtkWidget	*message_text;
static GtkWidget	*message_scrollbar;

extern int		root_width;
extern int		root_height;

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

  t = GTK_TEXT(message_text);

  len = strlen(str);
  buf = (char *)alloc(len + 2);

  sprintf(buf, "\n%s", str);

  gtk_text_freeze(t);

  if(u.uhpmax > 0 && (((double)u.uhp) / u.uhpmax < 0.1 || u.uhp < 5))
    i = MAP_RED;
  else
    i = MAP_BLACK;
  gtk_text_insert(t, NULL, &nh_color[i], &nh_color[MAP_WHITE], buf, len + 1);
  
  len = gtk_text_get_length(t);
  if(len > NH_TEXT_REMEMBER){
    for(i=0 ; i<len ; ++i)
      if(GTK_TEXT_INDEX(t, i) == '\n'){
	++i;
	gtk_text_set_point(t, i);
	gtk_text_backward_delete(t, i);
	gtk_text_set_point(t, len - i);
	break;
      }
  }
  gtk_text_thaw(t);
  gtk_adjustment_set_value(t->vadj, t->vadj->upper);

  free(buf);
}

int
GTK_doprev_message()
{
  return 0;
}

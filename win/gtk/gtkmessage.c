/*
  $Id: gtkmessage.c,v 1.1 2000-08-15 19:55:13 wacko Exp $
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
	FALSE, FALSE, NH_PAD);
    GTK_WIDGET_UNSET_FLAGS(message_text, GTK_CAN_FOCUS);

    gtk_widget_set_usize(GTK_WIDGET(message_text), (root_width)/2, -1);
    
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

  buf = malloc(strlen(str) + 2);

  sprintf(buf, "\n%s", str);

  if(u.uhpmax > 0 && (((double)u.uhp) / u.uhpmax < 0.1 || u.uhp < 5))
      gtk_text_insert(t, NULL, &nh_color[MAP_RED], &nh_color[MAP_WHITE], buf, strlen(buf));
  else
      gtk_text_insert(t, NULL, &nh_color[MAP_BLACK], &nh_color[MAP_WHITE], buf, strlen(buf));
  
  len = gtk_text_get_length(t);
  if(len > NH_TEXT_REMEMBER){
    for(i=0 ; i<len ; ++i)
      if(GTK_TEXT_INDEX(t, i) == '\n'){
	break;
      }
    ++i;
    gtk_text_freeze(t);
    gtk_text_set_point(t, i);
    gtk_text_backward_delete(t, i);
    gtk_text_set_point(t, len - i);
    gtk_text_thaw(t);
  }

  free(buf);
}

int
GTK_doprev_message()
{
  return 0;
}

/*
  $Id: gtkprogress.h,v 1.1 2003-08-31 12:54:24 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Slash'EM Development Team 2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#define NH_GTK_TYPE_PROGRESS_WINDOW	nh_gtk_progress_window_get_type()
#define NH_GTK_PROGRESS_WINDOW(obj)	G_TYPE_CHECK_INSTANCE_CAST(obj,\
					  NH_GTK_TYPE_PROGRESS_WINDOW,\
					  NhGtkProgressWindow)
#define NH_GTK_PROGRESS_WINDOW_CLASS(klass) \
					G_TYPE_CHECK_CLASS_CAST(klass,\
					  NH_GTK_TYPE_PROGRESS_WINDOW,\
					  NhGtkProgressWindowClass)
#define NH_GTK_IS_PROGRESS_WINDOW(obj)	G_TYPE_CHECK_INSTANCE_TYPE(obj,\
					  NH_GTK_TYPE_PROGRESS_WINDOW)
#define NH_GTK_IS_PROGRESS_WINDOW_CLASS(klass) \
					G_TYPE_CHECK_CLASS_TYPE(klass,\
					  NH_GTK_TYPE_PROGRESS_WINDOW)
#define NH_GTK_PROGRESS_WINDOW_GET_CLASS(obj) \
					G_TYPE_INSTANCE_GET_CLASS(obj,\
					  NH_GTK_TYPE_PROGRESS_WINDOW,\
					  NhGtkProgressWindowClass)

typedef struct _NhGtkProgressWindowClass NhGtkProgressWindowClass;

GType nh_gtk_progress_window_get_type(void) G_GNUC_CONST;
GtkWidget *nh_gtk_progress_window_new(const gchar *title, GtkWindow *parent);
void nh_gtk_progress_window_add_stage(NhGtkProgressWindow *w,
  const gchar *name);
void nh_gtk_progress_window_complete_stage(NhGtkProgressWindow *w);
void nh_gtk_progress_window_stage_pulse(NhGtkProgressWindow *w);
int nh_gtk_progress_window_stage_auto_pulse(NhGtkProgressWindow *w,
  gboolean enable);
void nh_gtk_progress_window_stage_set_fraction(NhGtkProgressWindow *w,
  gdouble fraction);

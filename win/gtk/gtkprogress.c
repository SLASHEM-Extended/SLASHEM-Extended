/*
  $Id: gtkprogress.c,v 1.2 2003-09-03 08:36:56 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Slash'EM Development Team 2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>
#include "winGTK.h"
#include "gtkprogress.h"

#define NH_GTK_TYPE_PROGRESS_CHECK	nh_gtk_progress_check_get_type()
#define NH_GTK_PROGRESS_CHECK(obj)	G_TYPE_CHECK_INSTANCE_CAST(obj,\
					  NH_GTK_TYPE_PROGRESS_CHECK,\
					  NhGtkProgressCheck)
#define NH_GTK_PROGRESS_CHECK_CLASS(klass) \
					G_TYPE_CHECK_CLASS_CAST(klass,\
					  NH_GTK_TYPE_PROGRESS_CHECK,\
					  NhGtkProgressCheckClass)
#define NH_GTK_IS_PROGRESS_CHECK(obj)	G_TYPE_CHECK_INSTANCE_TYPE(obj,\
					  NH_GTK_TYPE_PROGRESS_CHECK)
#define NH_GTK_IS_PROGRESS_CHECK_CLASS(klass) \
					G_TYPE_CHECK_CLASS_TYPE(klass,\
					  NH_GTK_TYPE_PROGRESS_CHECK)
#define NH_GTK_PROGRESS_CHECK_GET_CLASS(obj) \
					G_TYPE_INSTANCE_GET_CLASS(obj,\
					  NH_GTK_TYPE_PROGRESS_CHECK,\
					  NhGtkProgressCheckClass)

#define MIN_CHECK_SIZE	11

typedef enum {
    NH_GTK_PROGRESS_CHECK_TODO,
    NH_GTK_PROGRESS_CHECK_CURRENT,
    NH_GTK_PROGRESS_CHECK_COMPLETE,
} NhGtkProgressCheckState;

struct _NhGtkProgressCheck {
    GtkMisc parent_instance;
    NhGtkProgressCheckState state;
};

struct _NhGtkProgressCheckClass {
    GtkMiscClass parent_class;
};

typedef struct _NhGtkProgressCheck NhGtkProgressCheck;
typedef struct _NhGtkProgressCheckClass NhGtkProgressCheckClass;

static void nh_gtk_progress_check_init(NhGtkProgressCheck *check);
static void nh_gtk_progress_check_class_init(NhGtkProgressCheckClass *klass);
static gboolean nh_gtk_progress_check_expose(GtkWidget *widget,
  GdkEventExpose *event);

GType nh_gtk_progress_check_get_type(void)
{
    static GType type = 0;
    static const GTypeInfo info = {
	sizeof(NhGtkProgressCheckClass),
	(GBaseInitFunc)NULL,
	(GBaseFinalizeFunc)NULL,
	(GClassInitFunc)nh_gtk_progress_check_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data */
	sizeof(NhGtkProgressCheck),
	0,		/* n_preallocs */
	(GInstanceInitFunc)nh_gtk_progress_check_init
    };
    if (!type)
	type = g_type_register_static(GTK_TYPE_MISC, "NhGtkProgressCheck",
	  &info, 0);
    return type;
}

static void nh_gtk_progress_check_init(NhGtkProgressCheck *check)
{
    GTK_WIDGET_SET_FLAGS(check, GTK_NO_WINDOW);
    GTK_WIDGET(check)->requisition.width =
      MIN_CHECK_SIZE + GTK_MISC(check)->xpad * 2;
    GTK_WIDGET(check)->requisition.height =
      MIN_CHECK_SIZE + GTK_MISC(check)->ypad * 2;
    check->state = NH_GTK_PROGRESS_CHECK_TODO;
}

static void nh_gtk_progress_check_class_init(NhGtkProgressCheckClass *klass)
{
    ((GtkWidgetClass *)klass)->expose_event = nh_gtk_progress_check_expose;
}

static GtkWidget *nh_gtk_progress_check_new()
{
    return (GtkWidget *)g_object_new(NH_GTK_TYPE_PROGRESS_CHECK, NULL);
}

static void nh_gtk_progress_check_set(NhGtkProgressCheck *check,
  NhGtkProgressCheckState state)
{
    g_return_if_fail(NH_GTK_IS_PROGRESS_CHECK(check));
    if (check->state != state) {
	check->state = state;
	if (GTK_WIDGET_DRAWABLE(check))
	    gtk_widget_queue_draw(GTK_WIDGET(check));
    }
}

static gboolean nh_gtk_progress_check_expose(GtkWidget *widget,
  GdkEventExpose *event)
{
    NhGtkProgressCheck *check = NH_GTK_PROGRESS_CHECK(widget);
    GtkMisc *misc;
    gint width, height;
    gint x, y;
    gint extent;
    gfloat xalign;
    if (GTK_WIDGET_DRAWABLE(widget) &&
      check->state != NH_GTK_PROGRESS_CHECK_TODO) {
	misc = GTK_MISC(widget);
	width = widget->allocation.width - misc->xpad * 2;
	height = widget->allocation.height - misc->ypad * 2;
	extent = MIN(width, height) * 0.7;
	xalign = gtk_widget_get_direction(widget) == GTK_TEXT_DIR_LTR ?
	  misc->xalign : 1.0 - misc->xalign;
	x = floor(widget->allocation.x + misc->xpad +
	  (widget->allocation.width - extent) * xalign + 0.5);
	y = floor(widget->allocation.y + misc->ypad +
	  (widget->allocation.height - extent) * misc->yalign + 0.5);
	if (check->state == NH_GTK_PROGRESS_CHECK_CURRENT)
	    gtk_paint_arrow(widget->style, widget->window, widget->state,
	      GTK_SHADOW_NONE, &event->area, widget, "arrow",
	      GTK_ARROW_RIGHT, TRUE, x, y, extent, extent);
	else
	    gtk_paint_check(widget->style, widget->window, widget->state,
	      GTK_SHADOW_IN, &event->area, widget, "check",
	      x, y, extent, extent);
    }
    return FALSE;
}

struct _NhGtkProgressWindow {
    GtkDialog parent_instance;
    GtkTable *checklist;
    GtkProgressBar *bar;
    GtkLabel *status;
    int current_stage, n_stages;
    struct _NhGtkProgresStage {
	NhGtkProgressCheck *check;
	GtkLabel *text;
    } *stages;
    unsigned long last_updated;		/* in ms */
};

struct _NhGtkProgressWindowClass {
    GtkDialogClass parent_class;
};

static GObjectClass *parent_class=NULL;

static void nh_gtk_progress_window_init(NhGtkProgressWindow *w);
static void nh_gtk_progress_window_class_init(NhGtkProgressWindowClass *klass);
static void nh_gtk_progress_window_dispose(GObject *object);
static void nh_gtk_progress_window_finalize(GObject *object);

GType nh_gtk_progress_window_get_type(void)
{
    static GType type=0;
    static const GTypeInfo info = {
	sizeof(NhGtkProgressWindowClass),
	(GBaseInitFunc)NULL,
	(GBaseFinalizeFunc)NULL,
	(GClassInitFunc)nh_gtk_progress_window_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data */
	sizeof(NhGtkProgressWindow),
	0,		/* n_preallocs */
	(GInstanceInitFunc)nh_gtk_progress_window_init
    };
    if (!type)
	type = g_type_register_static(GTK_TYPE_DIALOG, "NhGtkProgressWindow",
	  &info, 0);
    return type;
}

static void nh_gtk_progress_window_class_init(NhGtkProgressWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    parent_class = g_type_class_peek_parent(klass);
    object_class->dispose = nh_gtk_progress_window_dispose;
    object_class->finalize = nh_gtk_progress_window_finalize;
}

static void nh_gtk_progress_window_init(NhGtkProgressWindow *w)
{
    w->checklist = (GtkTable *)gtk_table_new(0, 2, FALSE);
    gtk_table_set_row_spacings(w->checklist, 4);
    gtk_table_set_col_spacings(w->checklist, 4);
    gtk_table_set_homogeneous(w->checklist, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(w->checklist), 8);
    g_object_ref(w->checklist);
    gtk_object_sink(GTK_OBJECT(w->checklist));
    w->bar = (GtkProgressBar *)gtk_progress_bar_new();
    g_object_ref(w->bar);
    gtk_progress_bar_set_pulse_step(w->bar, 0.01);
    gtk_object_sink(GTK_OBJECT(w->bar));
#if 0
    /*
     * The HIG suggests that you display status text to the right of the
     * progress bar containing something like "18 of 40", but this is not
     * yet implemented.
     */
    w->status = (GtkLabel *)gtk_label_new(NULL);
    g_object_ref(w->status);
    gtk_object_sink(GTK_OBJECT(w->status));
#endif
    w->current_stage = -1;
}

GtkWidget *nh_gtk_progress_window_new(const gchar *title, GtkWindow *parent)
{
    NhGtkProgressWindow *w;
    GtkWidget *hbox;
    GtkDialog *dialog;
    w = (NhGtkProgressWindow *)g_object_new(NH_GTK_TYPE_PROGRESS_WINDOW, NULL);
    if (title)
	gtk_window_set_title(GTK_WINDOW(w), title);
    if (parent)
	gtk_window_set_transient_for(GTK_WINDOW(w), parent);
    gtk_window_set_modal(GTK_WINDOW(w), TRUE);
    dialog = GTK_DIALOG(w);
    gtk_container_add(GTK_CONTAINER(dialog->vbox), GTK_WIDGET(w->checklist));
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 2);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(w->bar), FALSE, FALSE, 6);
    if (w->status)
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(w->status),
	  FALSE, FALSE, 6);
    gtk_container_add(GTK_CONTAINER(dialog->vbox), GTK_WIDGET(hbox));
    gtk_dialog_add_button(dialog, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
    gtk_dialog_set_has_separator(dialog, FALSE);
    /* [ALI] We really should be able to cancel operations, but as yet
     * the GTK interface has no support for this, so we set the button
     * to be insensitive, which at least gives the user some clue.
     */
    gtk_dialog_set_response_sensitive(dialog, GTK_RESPONSE_CANCEL, FALSE);
    gtk_widget_show_all(dialog->vbox);
    gtk_widget_show_all(dialog->action_area);
    return (GtkWidget *)w;
}

static void nh_gtk_progress_window_dispose(GObject *object)
{
    int i;
    NhGtkProgressWindow *w;
    g_return_if_fail(NH_GTK_IS_PROGRESS_WINDOW(object));
    w = NH_GTK_PROGRESS_WINDOW(object);
    if (w->checklist) {
	g_object_unref(w->checklist);
	w->checklist = NULL;
    }
    if (w->bar) {
	g_object_unref(w->bar);
	w->bar = NULL;
    }
    if (w->status) {
	g_object_unref(w->status);
	w->status = NULL;
    }
    if (w->stages) {
	for(i = 0; i < w->n_stages; i++) {
	    g_object_unref(w->stages[i].check);
	    g_object_unref(w->stages[i].text);
	}
	w->n_stages = 0;
    }
    (*G_OBJECT_CLASS(parent_class)->dispose)(object);
}

static void nh_gtk_progress_window_finalize(GObject *object)
{
    NhGtkProgressWindow *w;
    g_return_if_fail(NH_GTK_IS_PROGRESS_WINDOW(object));
    w = NH_GTK_PROGRESS_WINDOW(object);
    if (w->stages) {
	if (w->n_stages) {
	    g_warning("NhGtkProgressWindow: finalized with undisposed stages");
	    w->n_stages = 0;
	}
	g_free(w->stages);
	w->stages = NULL;
    }
    (*G_OBJECT_CLASS(parent_class)->finalize)(object);
}

static void nh_gtk_progress_window_set_stage(NhGtkProgressWindow *w,
  guint stage, NhGtkProgressCheckState state)
{
    PangoAttribute *attr;
    PangoAttrList *attrs;
    g_return_if_fail(NH_GTK_IS_PROGRESS_WINDOW(w));
    g_return_if_fail(stage < w->n_stages);
    attrs = pango_attr_list_new();
    attr = pango_attr_weight_new(state == NH_GTK_PROGRESS_CHECK_CURRENT ?
      PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
    attr->start_index = 0;
    attr->end_index = G_MAXINT;
    pango_attr_list_insert(attrs, attr);
    gtk_label_set_attributes(w->stages[stage].text, attrs);
    pango_attr_list_unref(attrs);
    nh_gtk_progress_check_set(w->stages[stage].check, state);
}

void nh_gtk_progress_window_add_stage(NhGtkProgressWindow *w,
  const gchar *name)
{
    int n;
    GtkWidget *check, *label;
    g_return_if_fail(NH_GTK_IS_PROGRESS_WINDOW(w));
    n = w->n_stages;
    gtk_table_resize(w->checklist, n + 1, 2);
    check = nh_gtk_progress_check_new();
    gtk_widget_show(check);
    w->stages = g_realloc(w->stages, ++w->n_stages * sizeof(*w->stages));
    w->stages[n].check = (NhGtkProgressCheck *)check;
    g_object_ref(check);
    label = gtk_label_new(name);
    gtk_widget_show(label);
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    w->stages[n].text = (GtkLabel *)label;
    g_object_ref(label);
    gtk_table_attach(w->checklist, check, 0, 1, n, n + 1, 0, 0, 0, 0);
    gtk_table_attach(w->checklist, label, 1, 2, n, n + 1, GTK_SHRINK | GTK_FILL,
      0, 0, 0);
    nh_gtk_progress_window_set_stage(w, n,
      n ? NH_GTK_PROGRESS_CHECK_TODO : NH_GTK_PROGRESS_CHECK_CURRENT);
    if (w->current_stage < 0) {
	gtk_progress_bar_set_fraction(w->bar, 0.0);
	w->current_stage = 0;
    }
}

void nh_gtk_progress_window_complete_stage(NhGtkProgressWindow *w)
{
    g_return_if_fail(NH_GTK_IS_PROGRESS_WINDOW(w));
    g_return_if_fail(w->n_stages != 0);
    g_return_if_fail(w->current_stage < w->n_stages);
    nh_gtk_progress_window_set_stage(w, w->current_stage,
      NH_GTK_PROGRESS_CHECK_COMPLETE);
    w->current_stage++;
    if (w->current_stage < w->n_stages) {
	gtk_progress_bar_set_fraction(w->bar, 0.0);
	nh_gtk_progress_window_set_stage(w, w->current_stage,
	  NH_GTK_PROGRESS_CHECK_CURRENT);
    }
    while(gtk_events_pending())
	gtk_main_iteration();
}

static nh_gtk_progress_window_update(NhGtkProgressWindow *w)
{
    unsigned long now_ms;
    GTimeVal now;
    g_get_current_time(&now);
    now_ms = (unsigned long)now.tv_sec * 1000 +
	    (unsigned long)now.tv_usec / 1000;
    /* Update after 50ms or immediately if now_ms wraps (about 50 days) */
    if (now_ms < w->last_updated || now_ms - 50 > w->last_updated) {
	w->last_updated = now_ms;
	while(gtk_events_pending())
	    gtk_main_iteration();
    }
}

void nh_gtk_progress_window_stage_pulse(NhGtkProgressWindow *w)
{
    g_return_if_fail(NH_GTK_IS_PROGRESS_WINDOW(w));
    gtk_progress_bar_pulse(w->bar);
    nh_gtk_progress_window_update(w);
}

void nh_gtk_progress_window_stage_set_fraction(NhGtkProgressWindow *w,
  gdouble fraction)
{
    g_return_if_fail(NH_GTK_IS_PROGRESS_WINDOW(w));
    gtk_progress_bar_set_fraction(w->bar, fraction);
    nh_gtk_progress_window_update(w);
}

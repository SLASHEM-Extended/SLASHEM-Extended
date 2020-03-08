/*
  $Id: gtkstatus.c,v 1.12 2003-08-02 14:44:52 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2001-2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <math.h>
#include <sys/types.h>
#include <signal.h>
#include "winGTK.h"

/******************************************************************************
  The NhLight widget is a trivial widget that is used to indicate stat changed
 ******************************************************************************/

#define NH_TYPE_LIGHT			nh_light_get_type()
#define NH_LIGHT(obj)			G_TYPE_CHECK_INSTANCE_CAST(obj, \
						NH_TYPE_LIGHT, NhLight)
#define NH_LIGHT_CLASS(klass)		G_TYPE_CHECK_CLASS_CAST(klass, \
						NH_TYPE_LIGHT, NhLightClass)
#define NH_IS_LIGHT(obj)		G_TYPE_CHECK_INSTANCE_TYPE(obj, \
						NH_TYPE_LIGHT)
#define NH_IS_LIGHT_CLASS(klass)	G_TYPE_CHECK_CLASS_TYPE(klass, \
						NH_TYPE_LIGHT)
#define NH_LIGHT_GET_CLASS(obj)		G_TYPE_INSTANCE_GET_CLASS(obj, \
						NH_TYPE_LIGHT, NhLightClass)

#define NH_LIGHT_MAX_CONTRAST	10
#define NH_LIGHT_WIDTH		4
#define NH_LIGHT_HEIGHT		16

typedef struct _NhLight {
    GtkMisc misc;
    int contrast;		/* From 0 (none) to 10 (max), or -1 (transp) */
} NhLight;

typedef struct _NhLightClass {
    GtkMiscClass parent_class;
} NhLightClass;

static gpointer parent_class;

GType nh_light_get_type(void);

GtkWidget *nh_light_new(void);
void nh_light_set_contrast(NhLight *light, int contrast);

static void nh_light_class_init(NhLightClass *klass);
static void nh_light_init(NhLight *light);
static gint nh_light_expose(GtkWidget *widget, GdkEventExpose *expose);
static void nh_light_size_request(GtkWidget *widget,
  GtkRequisition *requisition);

GType nh_light_get_type(void)
{
    static GType type = 0;
    if (!type) {
	static const GTypeInfo info = {
	    sizeof(NhLightClass),
	    NULL,			/* base_init */
	    NULL,			/* base_finalize */
	    (GClassInitFunc)nh_light_class_init,
	    NULL,			/* class_finalize */
	    NULL,			/* class_data */
	    sizeof(NhLight),
	    0,				/* n_preallocs */
	    (GInstanceInitFunc)nh_light_init
	};
	type = g_type_register_static(GTK_TYPE_MISC, "NhLight", &info, 0);
    }
    return type;
}

GtkWidget *nh_light_new(void)
{
    return g_object_new(NH_TYPE_LIGHT, NULL);
}

void nh_light_set_contrast(NhLight *light, int contrast)
{
    if (contrast < 0)
	contrast = 0;
    else if (contrast > NH_LIGHT_MAX_CONTRAST)
	contrast = NH_LIGHT_MAX_CONTRAST;
    if (light->contrast != contrast) {
	light->contrast = contrast;
	gtk_widget_queue_draw(GTK_WIDGET(light));
    }
}

static void nh_light_class_init(NhLightClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    parent_class = g_type_class_peek_parent(klass);
    widget_class->expose_event = nh_light_expose;
    widget_class->size_request = nh_light_size_request;
}

static void nh_light_init(NhLight *light)
{
    GTK_WIDGET_SET_FLAGS(light, GTK_NO_WINDOW);
    light->contrast = 0;
}

static gint nh_light_expose(GtkWidget *widget, GdkEventExpose *expose)
{
    GtkMisc *misc = GTK_MISC(widget);
    NhLight *light = NH_LIGHT(widget);
    GdkGC *gc;
    GdkColormap *cmap;
    GdkColor color = {0};
    GdkRectangle area;
    gfloat xalign;
    gint x, y;
    guint16 bg;

    if (GTK_WIDGET_MAPPED(widget)) {
	area = expose->area;
	if (!gdk_rectangle_intersect(&area, &widget->allocation, &area))
	    return FALSE;
	if (gtk_widget_get_direction(widget) == GTK_TEXT_DIR_LTR)
	    xalign = misc->xalign;
	else
	    xalign = 1.0 - misc->xalign;
	x = floor(0.5 + widget->allocation.x + misc->xpad + xalign *
	  (widget->allocation.width - widget->requisition.width));
	y = floor(0.5 + widget->allocation.y + misc->ypad + misc->yalign *
	  (widget->allocation.height - widget->requisition.height));
	gc = gdk_gc_new(widget->window);
	cmap = gdk_drawable_get_colormap(widget->window);
	if (!cmap)
	    fprintf(stderr,"%s", "nh_light_expose: No colormap!\n");
	color.red = bg = widget->style->bg[GTK_STATE_NORMAL].red;
	color.red += (65535L - bg) * light->contrast / NH_LIGHT_MAX_CONTRAST;
	color.green = bg = widget->style->bg[GTK_STATE_NORMAL].green;
	color.green += (65535L - bg) * light->contrast / NH_LIGHT_MAX_CONTRAST;
	color.blue = bg = widget->style->bg[GTK_STATE_NORMAL].blue;
	color.blue += (65535L - bg) * light->contrast / NH_LIGHT_MAX_CONTRAST;
	if (gdk_colormap_alloc_color(cmap, &color, FALSE, TRUE))
	    gdk_gc_set_foreground(gc, &color);
	else
	    gdk_gc_set_foreground(gc, &widget->style->bg[GTK_STATE_NORMAL]);
	gdk_draw_rectangle(widget->window, gc, TRUE,
	  x, y, NH_LIGHT_WIDTH, NH_LIGHT_HEIGHT);
	g_object_unref(gc);
    }
}

static void nh_light_size_request(GtkWidget *widget,
  GtkRequisition *requisition)
{
    widget->requisition.width = NH_LIGHT_WIDTH + GTK_MISC(widget)->xpad * 2;
    widget->requisition.height = NH_LIGHT_HEIGHT + GTK_MISC(widget)->ypad * 2;
    GTK_WIDGET_CLASS(parent_class)->size_request(widget, requisition);
}

/******************************************************************************/

#define	NH_BAR_WIDTH	150
#define NH_BAR_HEIGHT	8

#define STAT_ROWS	8
#define STAT_COLS	2
#define STAT_BARS	2

static GtkWidget *handle;
static GtkWidget *frame;
static GtkWidget *dlvl;
static GtkWidget *stat_table;
static GtkWidget *hbox2;
static GtkWidget *vbox;

static GtkWidget *levi;
static GtkWidget *conf;
static GtkWidget *blin;
static GtkWidget *stun;
static GtkWidget *hall;
static GtkWidget *hung;
static GtkWidget *sick;
static GtkWidget *encu;
static GtkWidget *slim;

static GtkWidget *bar_table;
static struct {
    GtkWidget *hbox, *vbox, *lbl, *area;
    GdkPixmap *pixmap;
    GdkGC *gc;
} bar[STAT_BARS];

static struct {
    GtkLabel *label, *value;
    NhLight *light;
} stat_widgets[STAT_COLS][STAT_ROWS];

static gint
bar_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    int i;
    for(i = 0; i < STAT_BARS; i++)
	if (widget == bar[i].area)
	    break;
    if (i == STAT_BARS)
	return FALSE;
    gdk_draw_pixmap(widget->window,
      widget->style->fg_gc[GTK_WIDGET_STATE(widget)], bar[i].pixmap,
      event->area.x, event->area.y, event->area.x, event->area.y,
      event->area.width, event->area.height);

    return TRUE;
}

#define STAT_COLUMN(n)		(n)
#define STAT_BAR(n)		(STAT_COLS + (n))
#define STAT_FRAME		(STAT_COLS + STAT_BARS + 1)
#define STAT_DLEVEL		(STAT_COLS + STAT_BARS + 2)
#define STAT_HUNGER		(STAT_COLS + STAT_BARS + 3)
#define STAT_ENCUMBERANCE	(STAT_COLS + STAT_BARS + 4)
#define STAT_FLAGS		(STAT_COLS + STAT_BARS + 5)

struct nh_stat_tab {
    int		where;
    char	*label;
    char	*quan;		/* Name of quantity from game executable */
    char	*divisor;	/* Value to show as max */
    int		vi,dvi;		/* indecies into values array */
    int		row;
    gchar	*oldvalue;
} stat_tab[] = {
    {STAT_FRAME,	"",		"player",	NULL},
    {STAT_DLEVEL,	"",		"dlevel",	NULL},
    {STAT_HUNGER,	"",		"hunger",	NULL},
    {STAT_ENCUMBERANCE,	"",		"encumberance",	NULL},
    {STAT_FLAGS,	"",		"flags",	NULL},
    {STAT_BAR(1),	"HP",		"hp",		"hpmax"},
    {STAT_BAR(2),	"MP",		"pw",		"pwmax"},
    {STAT_COLUMN(1),	"AC",   	"ac",		NULL},
    {STAT_COLUMN(1),	"GOLD",  	"gold",		NULL},
    {STAT_COLUMN(1),	"LEVEL",	"elevel",	NULL},
    {STAT_COLUMN(1),	"HD",		"hitdice",	NULL},
    {STAT_COLUMN(1),	"ALIGN",	"align",	NULL},
    {STAT_COLUMN(1),	"TIME",		"time",		NULL},
    {STAT_COLUMN(1),	"EXP", 		"experience",	NULL},
    {STAT_COLUMN(1),	"SCORE",	"score",	NULL},
    {STAT_COLUMN(1),	"WEIGHT",	"weight",	"capacity"},
    {STAT_COLUMN(2),	"STR",		"strength",	NULL},
    {STAT_COLUMN(2),	"DEX",		"dexterity",	NULL},
    {STAT_COLUMN(2),	"CON",		"constitution",	NULL},
    {STAT_COLUMN(2),	"INT",		"intelligence",	NULL},
    {STAT_COLUMN(2),	"WIS",		"wisdom",	NULL},
    {STAT_COLUMN(2),	"CHA",		"charisma",	NULL},
};

static int in_trouble = FALSE;
static stat_tab_hp = -1;

boolean
nh_status_in_trouble(void)
{
    return in_trouble;
}

char *
nh_status_last_displayed(char *quan)
{
    int i;
    for(i = 0; i < SIZE(stat_tab); i++)
	if (!strcmp(quan, stat_tab[i].quan))
	    return stat_tab[i].oldvalue;
    return NULL;
}

static void
nh_status_reconfig(nv, values)
int nv;
char **values;
{
    int i, j, k;
    int rowno[STAT_COLS];
    for(i = 0; i < STAT_COLS; i++) {
	rowno[i] = 0;
    }
    for(i = 0; i < SIZE(stat_tab); i++) {
	stat_tab[i].vi = stat_tab[i].dvi = -1;
	for(j = 0; j < nv; j++) {
	    if (stat_tab[i].divisor &&
	      !strcmp(stat_tab[i].divisor, values[j])) {
		stat_tab[i].dvi = j;
		if (!stat_tab[i].quan || stat_tab[i].vi >= 0)
		    break;
	    } else if (stat_tab[i].quan &&
	      !strcmp(stat_tab[i].quan, values[j])) {
		stat_tab[i].vi = j;
		if (!strcmp(stat_tab[i].quan, "hp"))
		    stat_tab_hp = i;
		k = stat_tab[i].where - STAT_COLUMN(1);
		if (k >=0 && k < STAT_COLS) {
		    if (stat_tab[i].row != rowno[k]) {
			stat_tab[i].row = rowno[k];
			g_free(stat_tab[i].oldvalue);
			stat_tab[i].oldvalue = (gchar *)0;
		    }
		    gtk_label_set_text(stat_widgets[k][rowno[k]++].label,
		      stat_tab[i].label);
		}
		if (!stat_tab[i].divisor || stat_tab[i].dvi >= 0)
		    break;
	    }
	}
	if (stat_tab[i].vi < 0 && stat_tab[i].oldvalue) {
	    g_free(stat_tab[i].oldvalue);
	    stat_tab[i].oldvalue = (gchar *)0;
	}
    }
    for(i = 0; i < STAT_COLS; i++) {
	for(j = rowno[i]; j < STAT_ROWS; j++) {
	    gtk_label_set_text(stat_widgets[i][j].label, "");
	    gtk_label_set_text(stat_widgets[i][j].value, "");
	}
    }
}

static light_timer(gpointer data)
{
    int i, j;
    int retval = FALSE;
    NhLight *light;
    for(j = 0; j < STAT_ROWS; j++)
	for(i = 0; i < STAT_COLS; i++) {
	    light = stat_widgets[i][j].light;
	    if (light && light->contrast > 0) {
		nh_light_set_contrast(light, light->contrast - 1);
		if (light->contrast > 0)
		    retval = TRUE;
	    }
	}
    *(int *)data = retval;
    return retval;
}

void
GTK_ext_status(reconfig, nv, values)
int reconfig, nv;
const char **values;
{
    int i, j, val, dval;
    unsigned long fl;
    char buf[NH_BUFSIZ];
    char *Dummy = NULL;
    const char *value;
    gchar *str = NULL, *s;
    GdkRectangle update_rect;
    static int light_timer_active = 0;
    int start_light_timer = 0;

    if (reconfig) {
	nh_status_reconfig(nv, values);
	return;
    }
    in_trouble = FALSE;
    for(i = 0; i < SIZE(stat_tab); i++) {
	if (stat_tab[i].vi < 0)
	    continue;
	value = values[stat_tab[i].vi];
	if (stat_tab[i].dvi >= 0)
	    s = g_strdup_printf("%s/%s", value, values[stat_tab[i].dvi]);
	else
	    s = g_strdup(value);
	j = stat_tab[i].where - STAT_BAR(1);
	if (j >= 0 && j < 2 && *stat_tab[i].label)
	    str = g_strdup_printf("%s %s", stat_tab[i].label, s);
	else if (*s && (stat_tab[i].where == STAT_HUNGER ||
	  stat_tab[i].where == STAT_ENCUMBERANCE))
	    str = g_strdup_printf(" %s ", s);
	else {
	    str = s;
	    s = (gchar *)0;
	}
	g_free(s);
	if (!stat_tab[i].oldvalue || strcmp(str, stat_tab[i].oldvalue)) {
	    switch(stat_tab[i].where) {
		case STAT_COLUMN(1):
		case STAT_COLUMN(2):
		    j = stat_tab[i].where - STAT_COLUMN(1);
		    gtk_label_set_text(stat_widgets[j][stat_tab[i].row].value,
		      str);
		    nh_light_set_contrast(
		      stat_widgets[j][stat_tab[i].row].light,
		      NH_LIGHT_MAX_CONTRAST);
		    start_light_timer++;
		    break;
		case STAT_BAR(1):
		case STAT_BAR(2):
		    j = stat_tab[i].where - STAT_BAR(1);
		    update_rect.x = 0;
		    update_rect.y = 0;
		    update_rect.width = NH_BAR_WIDTH;
		    update_rect.height = NH_BAR_HEIGHT;
		    sscanf(value, "%d", &val);
		    if (stat_tab[i].dvi >= 0) {
			sscanf(values[stat_tab[i].dvi], "%d", &dval);
			if (i == stat_tab_hp && (val <= 5 || val * 7 <= dval))
			    in_trouble = TRUE;
			val = dval ? val * NH_BAR_WIDTH / dval : 0;
		    }
		    gdk_draw_rectangle(bar[j].pixmap, bar[j].gc, TRUE,
			0, 0, val, NH_BAR_HEIGHT);
		    if (val < NH_BAR_WIDTH)
			gdk_draw_rectangle(bar[j].pixmap,
			  bar[j].area->style->black_gc,
			  TRUE, val, 0, NH_BAR_WIDTH - val, NH_BAR_HEIGHT);
		    gtk_widget_draw(bar[j].area, &update_rect);
		    gtk_label_set_text(GTK_LABEL(bar[j].lbl), str);
		    break;
		case STAT_FRAME:
		    gtk_frame_set_label(GTK_FRAME(frame), str);
		    break;
		case STAT_DLEVEL:
		    gtk_label_set_text(GTK_LABEL(dlvl), str);
		    break;
		case STAT_HUNGER:
		    gtk_label_set_text(GTK_LABEL(hung), str);
		    break;
		case STAT_ENCUMBERANCE:
		    gtk_label_set_text(GTK_LABEL(encu), str);
		    break;
		case STAT_FLAGS:
		    sscanf(value, "%lX", &fl);
		    gtk_label_set_text(GTK_LABEL(levi),
		      (fl & RAW_STAT_LEVITATION) ? " Levitation " : "");
		    gtk_label_set_text(GTK_LABEL(blin),
		      (fl & RAW_STAT_BLIND) ? " Blind " : "");
		    gtk_label_set_text(GTK_LABEL(conf),
		      (fl & RAW_STAT_CONFUSION) ? " Confused " : "");
		    gtk_label_set_text(GTK_LABEL(sick),
		      (fl & RAW_STAT_FOODPOIS) ? " FoodPois " :
		      (fl & RAW_STAT_ILL) ? " Ill " : "");
		    gtk_label_set_text(GTK_LABEL(stun),
		      (fl & RAW_STAT_STUNNED) ? " Stunned " : "");
		    gtk_label_set_text(GTK_LABEL(hall),
		      (fl & RAW_STAT_HALLUCINATION) ? " Hallucination " : "");
		    gtk_label_set_text(GTK_LABEL(slim),
		      (fl & RAW_STAT_SLIMED) ? " Slimed " : "");
		    break;
	    }
	    g_free(stat_tab[i].oldvalue);
	    stat_tab[i].oldvalue = str;
	    str = (gchar *)0;
	}
	g_free(str);
    }
    if (!light_timer_active && start_light_timer)
	g_timeout_add(200, light_timer, &light_timer_active);
}

void
nh_status_destroy()
{
    /*
     * [ALI] Most objects will be destroyed when the status widget is
     * destroyed (ie., as part of destroying the main window).
     */
    int i;
    for(i = 0; i < STAT_BARS; i++)
	gdk_pixmap_unref(bar[i].pixmap);
}

GtkWidget *
nh_status_new()
{
    extern GtkWidget *main_window;
    GtkWidget *hbox, *w;
    GdkPixbuf *light;
    int	i, j;
    gchar *text[3] = { "", "", NULL};

    handle = gtk_handle_box_new();
    GTK_HANDLE_BOX(handle)->shrink_on_detach = 1;

    frame = nh_gtk_new_and_add(gtk_frame_new(NULL), handle, "");
    gtk_container_set_border_width(GTK_CONTAINER(frame), NH_PAD);

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");
    gtk_container_set_border_width(GTK_CONTAINER(vbox), NH_PAD);

    dlvl = nh_gtk_new_and_pack(gtk_label_new(""), vbox, "", FALSE, FALSE, 0);

    bar_table = nh_gtk_new_and_add(gtk_table_new(2, 2, FALSE), vbox, "");

    for(i = 0; i < STAT_BARS; i++) {
	bar[i].hbox = nh_gtk_new_and_attach(gtk_hbox_new(FALSE, 0), bar_table,
	  "", 0, 1, i, i + 1);

	for(j = 0; j < SIZE(stat_tab); j++)
	    if (stat_tab[j].where == STAT_BAR(i + 1))
		break;
	bar[i].lbl = nh_gtk_new_and_pack(gtk_label_new(stat_tab[j].label),
	  bar[i].hbox, "", FALSE, FALSE, 0);

	bar[i].vbox = nh_gtk_new_and_attach(gtk_vbox_new(TRUE, 0), bar_table,
	  "", 1, 2, i, i + 1);

	bar[i].area = nh_gtk_new_and_pack(gtk_drawing_area_new(), bar[i].vbox,
	  "", FALSE, FALSE, 0);

	gtk_signal_connect(GTK_OBJECT(bar[i].area), "expose_event",
	  GTK_SIGNAL_FUNC(bar_expose_event), NULL);
	bar[i].gc = gdk_gc_new(main_window->window);

	gtk_drawing_area_size(GTK_DRAWING_AREA(bar[i].area),
	  NH_BAR_WIDTH, NH_BAR_HEIGHT);

	bar[i].pixmap = gdk_pixmap_new(main_window->window,
	  NH_BAR_WIDTH, NH_BAR_HEIGHT, -1);
    }

    /*
     * [ALI] Spacing in the stat table is a little unconventional. What we
     * want to achive is:
     *
     * P< label>PP<  value>PP<light>PP< label>PP<  value>PP<light>P
     *
     * (where P is a padding of width NH_PAD). But we want to achive this
     * without using a column for the light (so that the table can be
     * homogeneous which looks better and keeps the resizing simple).
     *
     * We achieve this with a combination of table spacing and putting
     * the value and its light into an hbox.
     *
     * |P< label>P|P |<  value>PP<light>|P |P< label>P|P |<  value>PP<light>|P
     *
     * Column 0: Contents: P< label>P, Spacing: P
     * Column 1: Contents: <  value>PP<light>, Spacing: P
     * Column 2: Contents: P< label>P, Spacing: P
     * Column 3: Contents: <  value>PP<light>, Spacing: N/A
     *
     * The final spacing of P on the right is achieved by placing the
     * table in a horizontal box with an empty box.
     *
     * Note: It would be easier to move the padding out of the label
     * columns and then the now symetric padding could be added by
     * setting the padding of the table itself:
     *
     * P|< label>|PP |<  value>PP<light>|PP |< label>|PP |<  value>PP<light>|P
     *
     * However, this means that the width of the labels will be expanded
     * to be at least <  value>PP<light> rather than <  value><light>
     * which the current scheme achieves. Ideally, we'd like it to be
     * simply <  value>, but that doesn't seem possible.
     */
    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      TRUE, FALSE, 0);
    gtk_box_set_spacing(GTK_BOX(hbox), NH_PAD);
    stat_table = nh_gtk_new_and_pack(
      gtk_table_new(STAT_ROWS, STAT_COLS * 2, TRUE), hbox, "",
      TRUE, FALSE, 0);
    nh_gtk_new_and_pack(gtk_vbox_new(FALSE, 0), hbox, "",
      FALSE, FALSE, 0);
    gtk_table_set_col_spacings(GTK_TABLE(stat_table), NH_PAD);

    for(j = 0; j < STAT_COLS; j++)
	for(i = 0; i < STAT_ROWS; i++) {
	    w = nh_gtk_new_and_attach(gtk_label_new(""), stat_table, "",
	      j * 2, j * 2 + 1, i, i + 1);
	    gtk_misc_set_alignment(GTK_MISC(w), 1, 1);
	    gtk_misc_set_padding(GTK_MISC(w), NH_PAD, 0);
	    stat_widgets[j][i].label = GTK_LABEL(w);
	    hbox = nh_gtk_new_and_attach(gtk_hbox_new(FALSE, 0), stat_table, "",
	      j * 2 + 1, j * 2 + 2, i, i + 1);
	    gtk_box_set_spacing(GTK_BOX(hbox), 2 * NH_PAD);
	    w = nh_gtk_new_and_pack(gtk_label_new(""), hbox, "",
	      TRUE, TRUE, 0);
	    gtk_misc_set_alignment(GTK_MISC(w), 1, 1);
	    stat_widgets[j][i].value = GTK_LABEL(w);
	    w = nh_gtk_new_and_pack(nh_light_new(), hbox, "",
	      FALSE, FALSE, 0);
	    stat_widgets[j][i].light = NH_LIGHT(w);
	}

    hbox2 = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, 0);

    hung = nh_gtk_new_and_pack(gtk_label_new(""), hbox2, "", FALSE, FALSE, 0);

    levi = nh_gtk_new_and_pack(gtk_label_new(""), hbox2, "", FALSE, FALSE, 0);

    conf = nh_gtk_new_and_pack(gtk_label_new(""), hbox2, "", FALSE, FALSE, 0);

    blin = nh_gtk_new_and_pack(gtk_label_new(""), hbox2, "", FALSE, FALSE, 0);

    stun = nh_gtk_new_and_pack(gtk_label_new(""), hbox2, "", FALSE, FALSE, 0);

    hall = nh_gtk_new_and_pack(gtk_label_new(""), hbox2, "", FALSE, FALSE, 0);

    sick = nh_gtk_new_and_pack(gtk_label_new(""), hbox2, "", FALSE, FALSE, 0);

    slim = nh_gtk_new_and_pack(gtk_label_new(""), hbox2, "", FALSE, FALSE, 0);

    encu = nh_gtk_new_and_pack(gtk_label_new(""), hbox2, "", FALSE, FALSE, 0);

    /* Clear HP/MP bars */
    for(i = 0; i < STAT_BARS; i++) {
	GdkRectangle update_rect;

	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = NH_BAR_WIDTH;
	update_rect.height = NH_BAR_HEIGHT;

	gdk_gc_set_foreground(bar[i].gc, &nh_color[MAP_BLACK]);
	gdk_draw_rectangle(bar[i].pixmap, bar[i].gc, TRUE,
	  0, 0, NH_BAR_WIDTH, NH_BAR_HEIGHT);
	
	gtk_widget_draw(bar[i].area, &update_rect);
	gdk_gc_set_foreground(bar[i].gc, &nh_color[i ? CLR_GREEN : CLR_BLUE]);
    }

    for(i = 0; i < SIZE(stat_tab); i++)
	stat_tab[i].vi = stat_tab[i].dvi = -1;

    return handle;
}

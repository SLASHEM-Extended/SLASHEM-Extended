/*
  $Id: gtkstatus.c,v 1.6 2002-07-07 14:38:10 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2001-2002
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include "winGTK.h"

#define	NH_BAR_WIDTH	150
#define NH_BAR_HEIGHT	8

#define STAT_ROWS	8
#define STAT_COLS	2
#define STAT_BARS	2

static GtkWidget *handle;
static GtkWidget *frame;
static GtkWidget *dlvl;
static GtkWidget *hbox;
static GtkWidget *hbox2;
static GtkWidget *vbox;
static GtkWidget *clist[STAT_COLS+1];

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

static gint
bar_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    int i;
    for(i = 0; i < STAT_BARS; i++)
	if (widget == bar[i].area)
	    break;
    if (i == STAT_BARS)
	return FALSE;
    gdk_draw_pixmap(
	widget->window,
	widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
	bar[i].pixmap,
	event->area.x, event->area.y,
	event->area.x, event->area.y,
	event->area.width, event->area.height);

    return TRUE;
}

#if defined(MONITOR_HEAP) && defined(INTERNAL_MALLOC)
static int
stat_mem()
{
    return monitor_heap_getmem();
}
#endif

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
#if defined(MONITOR_HEAP) && defined(INTERNAL_MALLOC)
    {STAT_COLUMN(2),	"MEM",		NULL,		NULL},
#endif
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
    	gtk_clist_freeze(GTK_CLIST(clist[i]));
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
		if (!strcmp(stat_tab[i].quan, "HP"))
		    stat_tab_hp = i;
		k = stat_tab[i].where - STAT_COLUMN(1);
		if (k >=0 && k < STAT_COLS) {
		    if (stat_tab[i].row != rowno[k]) {
			stat_tab[i].row = rowno[k];
			g_free(stat_tab[i].oldvalue);
			stat_tab[i].oldvalue = (gchar *)0;
		    }
		    gtk_clist_set_text(GTK_CLIST(clist[k]), rowno[k]++,
			    0, stat_tab[i].label);
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
	    gtk_clist_set_text(GTK_CLIST(clist[i]), j, 0, "");
	    gtk_clist_set_text(GTK_CLIST(clist[i]), j, 1, "");
	}
	gtk_clist_set_column_min_width(GTK_CLIST(clist[i]), 0, 
		gtk_clist_optimal_column_width(GTK_CLIST(clist[i]), 0));
    	gtk_clist_thaw(GTK_CLIST(clist[i]));
    }
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

    if (reconfig) {
	nh_status_reconfig(nv, values);
	return;
    }
    in_trouble = FALSE;
    for(i = 0; i < STAT_COLS; i++)
    	gtk_clist_freeze(GTK_CLIST(clist[i]));
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
		    gtk_clist_set_text(GTK_CLIST(clist[j]), stat_tab[i].row,
			    1, str);
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
			val = val * NH_BAR_WIDTH / dval;
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
    for(i = 0; i < STAT_COLS; i++) {
	gtk_clist_set_column_min_width(GTK_CLIST(clist[i]), 1, 
		gtk_clist_optimal_column_width(GTK_CLIST(clist[i]), 1));
    	gtk_clist_thaw(GTK_CLIST(clist[i]));
    }
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
    extern GtkWidget	*main_window;
    GtkWidget	*w;
    int	i, j;
    gchar	*text[3] = { "", "", NULL};

    handle = gtk_handle_box_new();
    GTK_HANDLE_BOX(handle)->shrink_on_detach = 1;
    
/*
    gtk_widget_realize(handle);
*/

    frame = nh_gtk_new_and_add(gtk_frame_new(NULL), handle, "");

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    dlvl = nh_gtk_new_and_pack(
	gtk_label_new(""), vbox, "",
	FALSE, FALSE, 0);

    bar_table = nh_gtk_new_and_add(gtk_table_new(2, 2, FALSE), vbox, "");

    for(i = 0; i < STAT_BARS; i++) {
	bar[i].hbox = nh_gtk_new_and_attach(
	    gtk_hbox_new(FALSE, 0), bar_table, "", 0, 1, i, i + 1);

	for(j = 0; j < SIZE(stat_tab); j++)
	    if (stat_tab[j].where == STAT_BAR(i + 1))
		break;
	bar[i].lbl = nh_gtk_new_and_pack(
	    gtk_label_new(stat_tab[j].label), bar[i].hbox, "", FALSE, FALSE, 0);

	bar[i].vbox = nh_gtk_new_and_attach(
	    gtk_vbox_new(TRUE, 0), bar_table, "", 1, 2, i, i + 1);

	bar[i].area = nh_gtk_new_and_pack(
	    gtk_drawing_area_new(), bar[i].vbox, "", FALSE, FALSE, 0);

	gtk_signal_connect(
	    GTK_OBJECT(bar[i].area), "expose_event",
	    GTK_SIGNAL_FUNC(bar_expose_event), NULL);
	bar[i].gc = gdk_gc_new(main_window->window);

	gtk_drawing_area_size(
	    GTK_DRAWING_AREA(bar[i].area), NH_BAR_WIDTH, NH_BAR_HEIGHT);

	bar[i].pixmap = gdk_pixmap_new(
	    main_window->window,
	    NH_BAR_WIDTH, NH_BAR_HEIGHT, -1);
    }

    hbox = nh_gtk_new_and_pack(
	gtk_hbox_new(FALSE, 0), vbox, "",
	TRUE, FALSE, 0);

    for(j = 0; j < STAT_COLS; j++) {
	w = clist[j] = nh_gtk_new_and_pack(
	    gtk_clist_new(2), hbox, "", FALSE, FALSE, 0);
	GTK_WIDGET_UNSET_FLAGS(w, GTK_CAN_FOCUS);

	gtk_clist_set_shadow_type(GTK_CLIST(w), GTK_SHADOW_ETCHED_IN);

	gtk_clist_set_column_min_width(GTK_CLIST(w), 0, 50);
	gtk_clist_set_column_min_width(GTK_CLIST(w), 1, 50);

	gtk_clist_set_column_justification(GTK_CLIST(w), 0, GTK_JUSTIFY_RIGHT);
	gtk_clist_set_column_justification(GTK_CLIST(w), 1, GTK_JUSTIFY_RIGHT);

	for(i = 0; i < STAT_ROWS; i++) {
	    gtk_clist_append(GTK_CLIST(w), text);
	    /* gtk_clist_set_selectable(GTK_CLIST(w), i, FALSE); */
	}
    }
    hbox2 = nh_gtk_new_and_pack(
	gtk_hbox_new(FALSE, 0), vbox, "",
	FALSE, FALSE, 0);

    hung = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    levi = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    conf = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    blin = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    stun = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    hall = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    sick = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    slim = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    encu = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    /* Clear HP/MP bars */
    for(i = 0; i < STAT_BARS; i++) {
	GdkRectangle update_rect;

	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = NH_BAR_WIDTH;
	update_rect.height = NH_BAR_HEIGHT;

	gdk_gc_set_foreground(bar[i].gc, &nh_color[MAP_BLACK]);
	gdk_draw_rectangle(
		bar[i].pixmap, bar[i].gc, TRUE,
		0, 0, NH_BAR_WIDTH, NH_BAR_HEIGHT);
	
	gtk_widget_draw(bar[i].area, &update_rect);
	gdk_gc_set_foreground(bar[i].gc, &nh_color[i ? CLR_GREEN : CLR_BLUE]);
    }

    for(i = 0; i < SIZE(stat_tab); i++)
	stat_tab[i].vi = stat_tab[i].dvi = -1;

    return handle;
}

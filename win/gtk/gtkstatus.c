/*
  $Id: gtkstatus.c,v 1.9 2001-04-22 17:21:20 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include "winGTK.h"

#define	NH_BAR_WIDTH	150
#define NH_BAR_HEIGHT	8

#define STAT_ROWS	8
#define STAT_COLS	2

static GtkWidget *handle;
static GtkWidget *frame;
static GtkWidget *dlvl;
static GtkWidget *hbox;
static GtkWidget *hbox2;
static GtkWidget *vbox;
static GtkWidget *clist[STAT_COLS+1];

static GtkWidget *conf;
static GtkWidget *blin;
static GtkWidget *stun;
static GtkWidget *hall;
static GtkWidget *hung;
static GtkWidget *sick;
static GtkWidget *encu;

static GtkWidget *bar_table;
static GtkWidget *hp_hbox;
static GtkWidget *mp_hbox;
static GtkWidget *hp_bar_vbox;
static GtkWidget *mp_bar_vbox;
static GtkWidget *hp_lbl;
static GtkWidget *mp_lbl;
static GtkWidget *hp_bar;
static GtkWidget *mp_bar;

static GdkPixmap *hp_bar_pixmap;
static GdkPixmap *mp_bar_pixmap;

static GdkGC	*hp_gc;
static GdkGC	*mp_gc;

extern const char *hu_stat[];	/* eat.c */
extern const char *enc_stat[];	/* botl.c */

static gint
hp_bar_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    gdk_draw_pixmap(
	widget->window,
	widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
	hp_bar_pixmap,
	event->area.x, event->area.y,
	event->area.x, event->area.y,
	event->area.width, event->area.height);

    return FALSE;
}

static gint
mp_bar_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    gdk_draw_pixmap(
	widget->window,
	widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
	mp_bar_pixmap,
	event->area.x, event->area.y,
	event->area.x, event->area.y,
	event->area.width, event->area.height);

    return FALSE;
}

static int
stat_mhp(void *data)
{
    return u.mtimedone ? u.mhmax : u.uhpmax;
}

static int
stat_hp(void *data)
{
    return u.mtimedone ? (u.mh > 0 ? u.mh : 0) : (u.uhp > 0 ? u.uhp : 0);
}

static int
stat_lvl(void *data)
{
    return u.mtimedone ? mons[u.umonnum].mlevel : u.ulevel;     
}

/*
static int
stat_char(void *data)
{
    return *((char *)data);
}
*/

static int
stat_stat(void *data)
{
    int d = (int)data;

    return (int)ACURR(d);
}

static char *
stat_str(void)
{
    static char buf[6];
    if (ACURR(A_STR) > 18) {
	if (ACURR(A_STR) > STR18(100))
	    Sprintf(buf,"%2d",ACURR(A_STR)-100);
	else if (ACURR(A_STR) < STR18(100))
	    Sprintf(buf, "18/%02d",ACURR(A_STR)-18);
	else
	    Sprintf(buf,"18/**");
    } else
	Sprintf(buf, "%-1d",ACURR(A_STR));
    return buf;
}

#ifdef SCORE_ON_BOTL
static int
f_score()
{
    return flags.showscore;
}

static int
stat_score(void *data)
{
    return flags.showscore ? botl_score() : 0L;
}
#endif

#ifdef SHOW_WEIGHT
static int
f_weight()
{
    return flags.showweight;
}

static char *
stat_weight(void *data)
{
    static char buf[50];
    if (flags.showweight)
	Sprintf(buf, "%ld/%ld", (long)(inv_weight()+weight_cap()),
	  (long)weight_cap());
    else
	buf[0] = '\0';
    return buf;
}
#endif

#ifdef EXP_ON_BOTL
static int
f_exp()
{
    return flags.showexp;
}
#endif

static int
f_time()
{
    return flags.time;
}

static char *
stat_align(void *data)
{
    switch(u.ualign.type){
    case A_CHAOTIC:
	return "Chaotic";
	break;
    case A_NEUTRAL:
	return "Neutral";
	break;
    case A_LAWFUL:
	return "Lawful";
	break;
    }
    return "";
}

#if defined(MONITOR_HEAP) && defined(INTERNAL_MALLOC)
static int
stat_mem()
{
    return monitor_heap_getmem();
}
#endif

#define	NOVALUE		-999

enum {
    STAT_TYPE_NULL,
    STAT_TYPE_INT_P,
    STAT_TYPE_CHAR_P,
    STAT_TYPE_F_INT,
    STAT_TYPE_F_CHAR_P
};

struct nh_stat_tab {
    int 	oldvalue;
    int		typ;
    int		(*flg)();
    char	*name;
    void	*v;	/* pointer for value or function */
    void	*arg;
} stat_tab[STAT_COLS][STAT_ROWS] = {
    {
	{NOVALUE, STAT_TYPE_CHAR_P, NULL,	"AC",   	(void *)&u.uac},
	{NOVALUE, STAT_TYPE_INT_P, NULL,	"GOLD",  	(void *)&u.ugold},
	{NOVALUE, STAT_TYPE_F_INT, NULL,	"LEVEL",	stat_lvl},
	{NOVALUE, STAT_TYPE_F_CHAR_P, NULL,	"ALIGN",	stat_align},
	{NOVALUE, STAT_TYPE_INT_P, f_time,	"TIME",		(void *)&moves},
#ifdef EXP_ON_BOTL
	{NOVALUE, STAT_TYPE_INT_P, f_exp,	"EXP", 		(void *)&u.uexp},
#else
	{NOVALUE, STAT_TYPE_NULL},
#endif
#ifdef SCORE_ON_BOTL
	{NOVALUE, STAT_TYPE_F_INT, f_score,	"SCORE",	stat_score},
#else
	{NOVALUE, STAT_TYPE_NULL},
#endif
#ifdef SHOW_WEIGHT
	{NOVALUE, STAT_TYPE_F_CHAR_P, f_weight,	"WEIGHT",	stat_weight},
#else
	{NOVALUE, STAT_TYPE_NULL},
#endif
    },
    {
	{NOVALUE, STAT_TYPE_F_CHAR_P, NULL,	"STR",		stat_str},
	{NOVALUE, STAT_TYPE_F_INT, NULL,	"DEX",		stat_stat, (void *)A_DEX},
	{NOVALUE, STAT_TYPE_F_INT, NULL,	"CON",		stat_stat, (void *)A_CON},
	{NOVALUE, STAT_TYPE_F_INT, NULL,	"INT",		stat_stat, (void *)A_INT},
	{NOVALUE, STAT_TYPE_F_INT, NULL,	"WIS",		stat_stat, (void *)A_WIS},
	{NOVALUE, STAT_TYPE_F_INT, NULL,	"CHA",		stat_stat, (void *)A_CHA},
#if defined(MONITOR_HEAP) && defined(INTERNAL_MALLOC)
	{NOVALUE, STAT_TYPE_F_INT, NULL,	"MEM",		stat_mem},
#endif
    },
};

void
nh_status_update()
{
    char cval;
    int i, j, val;
    char buf[NH_BUFSIZ];
    char *Dummy = NULL;

    {
	int hp;
	int mp;
	gchar *str;
	GdkRectangle update_rect;

	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = NH_BAR_WIDTH;
	update_rect.height = NH_BAR_HEIGHT;

	hp = ((1.0 * stat_hp(Dummy)) / stat_mhp(Dummy)) * NH_BAR_WIDTH;
	mp = ((1.0 * u.uen) / u.uenmax) * NH_BAR_WIDTH;

	gdk_draw_rectangle(
	    hp_bar_pixmap, hp_gc, TRUE,
	    0, 0, hp, NH_BAR_HEIGHT);
	gdk_draw_rectangle(
	    mp_bar_pixmap, mp_gc, TRUE,
	    0, 0, mp, NH_BAR_HEIGHT);

	if(hp < NH_BAR_WIDTH)
	    gdk_draw_rectangle(
		hp_bar_pixmap, hp_bar->style->black_gc, TRUE,
		hp, 0, NH_BAR_WIDTH - hp, NH_BAR_HEIGHT);
	
	if(mp < NH_BAR_WIDTH)
	    gdk_draw_rectangle(
		mp_bar_pixmap, mp_bar->style->black_gc, TRUE,
		mp, 0, NH_BAR_WIDTH - mp, NH_BAR_HEIGHT);

	gtk_widget_draw(hp_bar, &update_rect);
	gtk_widget_draw(mp_bar, &update_rect);

	str = g_strdup_printf("HP %d/%d", stat_hp(Dummy), stat_mhp(Dummy));
	gtk_label_set_text(GTK_LABEL(hp_lbl), str);
	g_free(str);

	str = g_strdup_printf("MP %d/%d", u.uen , u.uenmax);
	gtk_label_set_text(GTK_LABEL(mp_lbl), str);
	g_free(str);
    }

    Strcpy(buf, plname);
    if ('a' <= buf[0] && buf[0] <= 'z') buf[0] += 'A'-'a';
    Strcat(buf, " ");
    if (u.mtimedone) {
	char mname[BUFSZ];
	Strcpy(mname, mons[u.umonnum].mname);
	Strcat(buf, mname);
    } else
	Strcat(buf, rank_of(u.ulevel, pl_character[0], flags.female));

    if(buf[0])
	gtk_frame_set_label(GTK_FRAME(frame), buf);

    if (In_endgame(&u.uz)) {
	Strcpy(buf, (Is_astralevel(&u.uz) ? "Astral Plane":"End Game"));
    } else {
	Strcpy(buf, dungeons[u.uz.dnum].dname);
	Sprintf(eos(buf), ", level %d", depth(&u.uz));
    }
    if(buf[0])
	gtk_label_set_text(GTK_LABEL(dlvl), buf);


    for(j=0 ; j<STAT_COLS ; ++j) {
    	gtk_clist_freeze(GTK_CLIST(clist[j]));
    	
	for(i=0 ; i<STAT_ROWS ; ++i) {
	    struct nh_stat_tab *t;
	    t = &stat_tab[j][i];

	    if(t->typ == STAT_TYPE_NULL)
		continue;
	    else if(t->flg && !t->flg()){
		gtk_clist_set_text(
		    GTK_CLIST(clist[j]),
		    i, 1, "");
	    }
	    else if(t->typ == STAT_TYPE_INT_P){
		val = *((int *)(t->v));
		if(val != t->oldvalue){
		    sprintf(buf, "%d", val);
		    gtk_clist_set_text(
			GTK_CLIST(clist[j]),
			i, 1, buf);
		    t->oldvalue = val;
		}
	    }
	    else if(t->typ == STAT_TYPE_CHAR_P){
		cval = *((char *)(t->v));
		if(cval != t->oldvalue){
		    sprintf(buf, "%d", cval);
		    gtk_clist_set_text(
			GTK_CLIST(clist[j]),
			i, 1, buf);
		    t->oldvalue = cval;
		}
	    }
	    else if(t->typ == STAT_TYPE_F_INT){
		int	(*f)();

		f = t->v;
		val = (*f)(t->arg);
		if(val != t->oldvalue){
		    sprintf(buf, "%d", val);
		    gtk_clist_set_text(
			GTK_CLIST(clist[j]),
			i, 1, buf);
		    t->oldvalue = val;
		}
	    }
	    else if(t->typ == STAT_TYPE_F_CHAR_P){
		char	*(*f)();

		f = t->v;
		Strcpy(buf, (*f)(t->arg));
		gtk_clist_set_text(
		    GTK_CLIST(clist[j]),
		    i, 1, buf);
	    }
	    gtk_clist_set_text(
		GTK_CLIST(clist[j]),
		i, 0, t->name);
	}
	
	gtk_clist_set_column_min_width(GTK_CLIST(clist[j]), 0, 
		gtk_clist_optimal_column_width(GTK_CLIST(clist[j]), 0));
	gtk_clist_set_column_min_width(GTK_CLIST(clist[j]), 1, 
		gtk_clist_optimal_column_width(GTK_CLIST(clist[j]), 1));
    	gtk_clist_thaw(GTK_CLIST(clist[j]));
    }


    if(Blind)
	gtk_label_set_text(GTK_LABEL(blin), "Blind");
    else
	gtk_label_set_text(GTK_LABEL(blin), "");

    if(Confusion)
	gtk_label_set_text(GTK_LABEL(conf), "Confused");
    else
	gtk_label_set_text(GTK_LABEL(conf), "");

    if(Sick){
	if(u.usick_type & SICK_VOMITABLE)
	    gtk_label_set_text(GTK_LABEL(sick), "FoodPois");
	else
	    gtk_label_set_text(GTK_LABEL(sick), "Ill");
    }
    else
	gtk_label_set_text(GTK_LABEL(sick), "");

    if(Stunned)
	gtk_label_set_text(GTK_LABEL(stun), "Stunned");
    else
	gtk_label_set_text(GTK_LABEL(stun), "");

    if(Hallucination)
	gtk_label_set_text(GTK_LABEL(hall), "Hallucinating");
    else
	gtk_label_set_text(GTK_LABEL(hall), "");

    if(u.uhs != 1)
	gtk_label_set_text(GTK_LABEL(hung), hu_stat[u.uhs]);
    else
	gtk_label_set_text(GTK_LABEL(hung), "");

    if(near_capacity())
	gtk_label_set_text(GTK_LABEL(encu), enc_stat[near_capacity()]);
    else
	gtk_label_set_text(GTK_LABEL(encu), "");
}

void
nh_status_index_update()
{
    int i, j;
    gchar	*text[3];
    struct nh_stat_tab *t;

    for(j=0 ; j<2 ; ++j){
	for(i=0 ; i<7 ; ++i){
	    t = &stat_tab[j][i];
      
	    text[0] = t->name;
	}
    }
}

void
nh_status_destroy()
{
    /*
     * [ALI] Most objects will be destroyed when the status widget is
     * destroyed (ie., as part of destroying the main window).
     */
    gdk_pixmap_unref(hp_bar_pixmap);
    gdk_pixmap_unref(mp_bar_pixmap);
}

GtkWidget *
nh_status_new()
{
    extern GtkWidget	*main_window;
    GtkWidget	*w;
    int	i, j;
    gchar	*text[3];

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

    hp_hbox = nh_gtk_new_and_attach(
	gtk_hbox_new(FALSE, 0), bar_table, "",
	0, 1, 0, 1);

    mp_hbox = nh_gtk_new_and_attach(
	gtk_hbox_new(FALSE, 0), bar_table, "",
	0, 1, 1, 2);

    hp_lbl = nh_gtk_new_and_pack(
	gtk_label_new("HP"), hp_hbox, "",
	FALSE, FALSE, 0);

    mp_lbl = nh_gtk_new_and_pack(
	gtk_label_new("MP"), mp_hbox, "",
	FALSE, FALSE, 0);

    hp_bar_vbox = nh_gtk_new_and_attach(
	gtk_vbox_new(TRUE, 0), bar_table, "",
	1, 2, 0, 1);
    mp_bar_vbox = nh_gtk_new_and_attach(
	gtk_vbox_new(TRUE, 0), bar_table, "",
	1, 2, 1, 2);

    hp_bar = nh_gtk_new_and_pack(
	gtk_drawing_area_new(), hp_bar_vbox, "",
	FALSE, FALSE, 0);
    mp_bar = nh_gtk_new_and_pack(
	gtk_drawing_area_new(), mp_bar_vbox, "",
	FALSE, FALSE, 0);

    gtk_signal_connect(
	GTK_OBJECT(hp_bar), "expose_event",
	GTK_SIGNAL_FUNC(hp_bar_expose_event), NULL);
    gtk_signal_connect(
	GTK_OBJECT(mp_bar), "expose_event",
	GTK_SIGNAL_FUNC(mp_bar_expose_event), NULL);

    hp_gc = gdk_gc_new(main_window->window);
    mp_gc = gdk_gc_new(main_window->window);

    gtk_drawing_area_size(
	GTK_DRAWING_AREA(hp_bar),
	NH_BAR_WIDTH, NH_BAR_HEIGHT);

    gtk_drawing_area_size(
	GTK_DRAWING_AREA(mp_bar),
	NH_BAR_WIDTH, NH_BAR_HEIGHT);

    hp_bar_pixmap = gdk_pixmap_new(
	main_window->window,
	NH_BAR_WIDTH, NH_BAR_HEIGHT, -1);

    mp_bar_pixmap = gdk_pixmap_new(
	main_window->window,
	NH_BAR_WIDTH, NH_BAR_HEIGHT, -1);


    hbox = nh_gtk_new_and_pack(
	gtk_hbox_new(FALSE, 0), vbox, "",
	TRUE, FALSE, 0);

    for(j=0 ; j<STAT_COLS ; ++j){
	w = clist[j] = nh_gtk_new_and_pack(
	    gtk_clist_new(2), hbox, "",
	    FALSE, FALSE, 0);
	GTK_WIDGET_UNSET_FLAGS(w, GTK_CAN_FOCUS);

	gtk_clist_set_shadow_type(GTK_CLIST(w), GTK_SHADOW_ETCHED_IN);

	gtk_clist_set_column_min_width(GTK_CLIST(w), 0, 50);
	gtk_clist_set_column_min_width(GTK_CLIST(w), 1, 50);

	gtk_clist_set_column_justification(
	    GTK_CLIST(w), 0, GTK_JUSTIFY_RIGHT
	    );
	gtk_clist_set_column_justification(
	    GTK_CLIST(w), 1, GTK_JUSTIFY_RIGHT
	    );

	for(i=0 ; i<STAT_ROWS ; ++i){
	    struct nh_stat_tab *t;

	    t = &stat_tab[j][i];

/*	    text[0] = t->name;*/
	    text[0] = "";

	    text[1] = "";
	    gtk_clist_append(GTK_CLIST(w), text);
/*	    gtk_clist_set_selectable(GTK_CLIST(w), i, FALSE);*/
	}
    }
    hbox2 = nh_gtk_new_and_pack(
	gtk_hbox_new(FALSE, 0), vbox, "",
	FALSE, FALSE, 0);

    hung = nh_gtk_new_and_pack(
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

    encu = nh_gtk_new_and_pack(
	gtk_label_new(""), hbox2, "",
	FALSE, FALSE, 0);

    /* Clear HP/MP bars */
    gdk_gc_set_foreground(hp_gc, &nh_color[MAP_BLACK]);
    gdk_gc_set_foreground(mp_gc, &nh_color[MAP_BLACK]);
    {
	GdkRectangle update_rect;

	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = NH_BAR_WIDTH;
	update_rect.height = NH_BAR_HEIGHT;

	gdk_draw_rectangle(
		hp_bar_pixmap, hp_gc, TRUE,
		0, 0, NH_BAR_WIDTH, NH_BAR_HEIGHT);
	
	gdk_draw_rectangle(
		mp_bar_pixmap, mp_gc, TRUE,
		0, 0, NH_BAR_WIDTH, NH_BAR_HEIGHT);

	gtk_widget_draw(hp_bar, &update_rect);
	gtk_widget_draw(mp_bar, &update_rect);

    }
    gdk_gc_set_foreground(hp_gc, &nh_color[CLR_BLUE]);
    gdk_gc_set_foreground(mp_gc, &nh_color[CLR_GREEN]);

    return handle;
}

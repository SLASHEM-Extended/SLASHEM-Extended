/*
  $Id: gtkmisc.c,v 1.17 2004-04-10 14:30:50 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2000-2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <sys/types.h>
#include <signal.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "winGTK.h"
#ifdef GTK_PROXY
#include "proxycb.h"
#endif

static gboolean	 option_lock;
static int	 keysym;
static GtkWidget *entry_url;
static GtkWidget *entry_plname;
static GtkWidget *entry_dogname;
static GtkWidget *entry_catname;
static GtkWidget *entry_fruit;
static GtkWidget *entry_proxy, *entry_proxy_port;
static GtkWidget *radio_k, *radio_d, *radio_r;
static GtkWidget *radio_menu_t, *radio_menu_p, *radio_menu_c, *radio_menu_f;

static struct GTK_Option {
    char      *opt_name;
    char      *on;
    char      *off;
    char      *option;
    boolean   not;
    GSList    *group;
    GtkWidget *radio1;
    GtkWidget *radio2;
} gtk_option[] = {
    {"prevent you from attacking your pet", "Yes", "No", "safe_pet"},
    {"ask before hitting peaceful monsters", "Yes", "No", "confirm"},
#ifdef TEXTCOLOR
    {"display pets in a red square", "Yes", "No", "hilite_pet"},
#endif
    {NULL,},
    {"display experience points", "Yes", "No", "showexp"},
    {"display score points", "Yes", "No", "showscore"},
    {"display elapsed game time", "Yes", "No", "time"},
    {NULL,},
    {"automatically pick up objects", "Yes", "No", "autopickup"},
    {NULL,},
    {"print introductory message", "Yes", "No", "legacy"},
#ifdef NEWS
    {"print any news", "Yes", "No", "news"},
#endif
#ifdef MAIL
    {"enable the mail dameon", "Yes", "No", "mail"},
#endif
    {NULL,},
    {"space bar as a rest character", "Yes", "No", "rest_on_space"},
    {"print more commentary", "Yes", "No", "verbose"},
    {NULL,},
    {"print tombstone when die", "Yes", "No", "tombstone"},
    {NULL,},
    {"try to retain the same letter for the same objects", "Yes", "No", "fixinv"},
    {"group similar kinds of objects in inventory", "Yes", "No", "sortpack"},
};

static void nh_option_set(void);
static void nh_option_get(void);
static int nh_option_has_changed(void);
static int nh_option_more_confirm(void);

static gint
default_destroy(GtkWidget *widget, gpointer data)
{
    guint *hid = (guint *)data;
    *hid = 0;
    keysym = '\033';

    gtk_main_quit();

    return FALSE;
}

static gint
default_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    keysym = nh_keysym(event);

    if (keysym == '\n' || keysym == '\033')
	gtk_main_quit();

    return FALSE;
}

static gint
default_clicked(GtkWidget *widget, gpointer data)
{
    if (data)
	keysym = (int)data;
    else
	keysym = '\n';

    if (keysym == 'm') {
	if (nh_option_has_changed()) {
	    switch(nh_option_more_confirm()) {
		case -1:	/* Cancel "more options" */
		    keysym = 0;
		    return FALSE;
		default:	/* Discard changes */
		    nh_option_set();
		    break;
		case 1:		/* Apply changes first */
		    nh_option_get();
		    break;
	    }
	}
	/*
	 * We have to disable the cache during the call to doset since the
	 * game may call a number of window interface functions during the
	 * call and there is no way for us to know at what point the options
	 * have been set.
	 */
	nh_option_cache_disable();
#ifdef GTK_PROXY
	proxy_cb_doset();
#else
	doset();
#endif
	nh_option_cache_enable();
	nh_option_set();
    } else
	gtk_main_quit();

    return FALSE;
}

/* ALI
 * Extra dialogue to confirm action before bringing up "more options"
 * dialogue if any of the options have been changed first.
 *
 * Returns: -1: Cancel, 0: Discard, 1: Apply
 */

static gint
mc_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    keysym = nh_keysym(event);
    gtk_main_quit();
    return FALSE;
}

static gint
mc_clicked(GtkWidget *widget, gpointer data)
{
    keysym = (int)data;
    gtk_main_quit();
    return FALSE;
}

static int
nh_option_more_confirm(void)
{
    guint hid;	
    GtkWidget *w;
    GtkWidget *frame;
    GtkWidget *vbox, *hbox;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *button3;

    w = nh_gtk_window_dialog(TRUE);
    nh_gtk_focus_set_master(GTK_WINDOW(w),
      GTK_SIGNAL_FUNC(mc_key_press), 0, TRUE);
    hid = gtk_signal_connect(GTK_OBJECT(w), "destroy",
      GTK_SIGNAL_FUNC(default_destroy), &hid);
    nh_position_popup_dialog(GTK_WIDGET(w));

    frame = nh_gtk_new_and_add(gtk_frame_new(NULL), w, "");
    gtk_container_border_width(GTK_CONTAINER(frame), NH_PAD);
    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");
    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, 0);
    nh_gtk_new_and_pack(gtk_label_new("Commit changes made?"),
      hbox, "", FALSE, FALSE, 0);
    nh_gtk_new_and_pack(gtk_vseparator_new(), vbox, "", FALSE, FALSE, NH_PAD);
    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

    button1 = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_YES),
      hbox, "", FALSE, FALSE, NH_PAD);
    button2 = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_NO),
      hbox, "", FALSE, FALSE, NH_PAD);
    button3 = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_CANCEL),
      hbox, "", FALSE, FALSE, NH_PAD);
    gtk_signal_connect(GTK_OBJECT(button1), "clicked",
      GTK_SIGNAL_FUNC(mc_clicked), (gpointer)'y');
    gtk_signal_connect(GTK_OBJECT(button2), "clicked",
      GTK_SIGNAL_FUNC(mc_clicked), (gpointer)'n');
    gtk_signal_connect(GTK_OBJECT(button3), "clicked",
      GTK_SIGNAL_FUNC(mc_clicked), (gpointer)'c');

    gtk_widget_show_all(w);
    gtk_grab_add(w);
    gtk_main();

    if (hid > 0) {
	gtk_signal_disconnect(GTK_OBJECT(w), hid);
	gtk_widget_destroy(w);
    }
    switch(keysym) {
	case 'a':
	case 'A':
	case 'y':
	case 'Y':
	case '\n':
	    return 1;
	case '\033':
	case 'c':
	case 'C':
	    return -1;
	default:
	    return 0;
    }
}

static void
nh_option_set(void)
{
    int i;
    GtkWidget *button;
    struct GTK_Option *p;
    char *s;

    gtk_entry_set_text(GTK_ENTRY(entry_plname), nh_option_cache_get("name"));
    gtk_entry_set_text(GTK_ENTRY(entry_catname), nh_option_cache_get("catname"));
    gtk_entry_set_text(GTK_ENTRY(entry_dogname), nh_option_cache_get("dogname"));
    gtk_entry_set_text(GTK_ENTRY(entry_fruit), nh_option_cache_get("fruit"));

    s = nh_option_cache_get("pettype");
    if (*s == 'c')
	button = radio_k;
    else if (*s == 'd')
	button = radio_d;
    else
	button = radio_r;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

    s = nh_option_cache_get("menustyle");
    switch (*s) {
	default:
	case 't':
	    button = radio_menu_t;
	    break;
	case 'p':
	    button = radio_menu_p;
	    break;
	case 'c':
	    button = radio_menu_c;
	    break;
	case 'f':
	    button = radio_menu_f;
	    break;
    }
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

    for(i = 0; i < SIZE(gtk_option); i++) {
	p = &gtk_option[i];
	p->group = NULL;
	if (p->opt_name) {
	    button = p->not ^ nh_option_cache_get_bool(p->option) ?
	      p->radio1 : p->radio2;
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
	}
    }

    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(tileTab[nh_get_map_visual()].data), TRUE);
}

/* [ALI] We could probably do this better with glib functions */

#define NHOF_NOCACHE	1

#define NHOF_DIRTY	1
#define NHOF_BOOLEAN	2

static char *boolean_set = "yes";	/* Where value points for booleans */
static char *boolean_reset = "no";

static unsigned nh_option_cache_flags;
static int nh_option_cache_size;
static struct nh_option {
    char *option;
    char *value;
    void *addr;
    void (*callback)();	/* boolean: func(boolean) non-boolean: func(char *) */
    unsigned flags;
} *nh_option_cache;

static struct nh_option *
nh_option_cache_getent(const char *option)
{
    int i;
    struct nh_option *new;
    for(i = 0; i < nh_option_cache_size; i++)
	if (nh_option_cache[i].option &&
	  !strcmp(option, nh_option_cache[i].option))
	    return nh_option_cache + i;
    for(i = 0; i < nh_option_cache_size; i++)
	if (!nh_option_cache[i].option)
	    break;
    if (i == nh_option_cache_size) {
	new = (struct nh_option *)realloc(nh_option_cache,
	  (nh_option_cache_size + 1) * sizeof(*nh_option_cache));
	if (!new)
	    return NULL;
	nh_option_cache = new;
	i = nh_option_cache_size++;
    }
    memset(nh_option_cache + i, 0, sizeof(*nh_option_cache));
    nh_option_cache[i].option = strdup(option);
    return nh_option_cache[i].option ? nh_option_cache + i : NULL;
}

/*
 * Synchronize the game core with the cache
 */

int
nh_option_cache_sync(void)
{
    int i, nb = 0;
    char *buf, *bp;
    for(i = 0; i < nh_option_cache_size; i++)
	if (nh_option_cache[i].flags & NHOF_DIRTY) {
	    nb += strlen(nh_option_cache[i].option);
	    if (nh_option_cache[i].flags & NHOF_BOOLEAN)
		nb += nh_option_cache[i].value == boolean_set ? 1 : 2;
	    else
		nb += strlen(nh_option_cache[i].value) + 2;
	}
    if (!nb)
	return TRUE;
    bp = buf = malloc(nb);
    if (!buf)
	return FALSE;
    for(i = 0; i < nh_option_cache_size; i++)
	if (nh_option_cache[i].flags & NHOF_DIRTY) {
	    if (bp != buf)
		*bp++ = ',';
	    if ((nh_option_cache[i].flags & NHOF_BOOLEAN) &&
	      nh_option_cache[i].value != boolean_set)
		*bp++ = '!';
	    strcpy(bp, nh_option_cache[i].option);
	    bp = eos(bp);
	    if (!(nh_option_cache[i].flags & NHOF_BOOLEAN)) {
		*bp++ = '=';
		strcpy(bp, nh_option_cache[i].value);
		bp = eos(bp);
	    }
	    nh_option_cache[i].flags &= ~NHOF_DIRTY;
	}
#ifdef GTK_PROXY
    proxy_cb_parse_options(buf);
#else
    parseoptions(buf, FALSE, FALSE);
#endif
    free(buf);
    return TRUE;
}

/*
 * Disable the cache. This is normally only done during calls to doset().
 * Note that this will cause spurious callbacks to be triggered for options
 * that are not changed by doset() when the options are re-entered into
 * the cache. Options with an address set do not suffer from this problem.
 */

int
nh_option_cache_disable(void)
{
    int i;
    char *value;
    boolean bv;
    struct nh_option *no = nh_option_cache;
    nh_option_cache_flags |= NHOF_NOCACHE;
    for(i = 0; i < nh_option_cache_size; i++, no++) {
	if (!(no->flags & NHOF_BOOLEAN))
	    free(no->value);
	if (!no->addr) {
	    free(no->option);
	    no->option = NULL;
	    no->flags = 0;
	}
	no->value = NULL;
    }
}

int
nh_option_cache_enable(void)
{
    int i;
    char *value, *old_value;
    boolean bv, obv;
    struct nh_option *no = nh_option_cache;
    nh_option_cache_flags &= ~NHOF_NOCACHE;
    for(i = 0; i < nh_option_cache_size; i++, no++) {
	if (no->addr) {
#ifdef GTK_PROXY
	    value = proxy_cb_get_option(no->option);
#else
	    value = get_option(no->option);
#endif
	    no->flags &= ~NHOF_DIRTY;
	    if (no->flags & NHOF_BOOLEAN) {
		bv = !strcmp(value, "yes");
		no->value = bv ? boolean_set : boolean_reset;
		obv = *(boolean *)no->addr;
		*(boolean *)no->addr = bv;
		if (no->callback && bv != obv)
		    (*no->callback)(bv);
	    } else {
		no->value = strdup(value);
		old_value = *(char **)no->addr;
		*(char **)no->addr = strdup(value);
		if (no->callback && strcmp(no->value, old_value))
		    (*no->callback)(no->value);
		free(old_value);
	    }
	}
    }
}

void
nh_option_cache_set(char *option, const char *value)
{
    boolean changed = FALSE;
    char *buf;
    struct nh_option *no = nh_option_cache_getent(option);
    if (no && no->flags & NHOF_BOOLEAN)
	panic("Setting value for boolean option %s", option);
    if (nh_option_cache_flags & NHOF_NOCACHE) {
	buf = malloc(strlen(option) + strlen(value) + 2);
	if (buf) {
	    sprintf(buf, "%s=%s", option, value);
#ifdef GTK_PROXY
	    proxy_cb_parse_options(buf);
#else
	    parseoptions(buf, FALSE, FALSE);
#endif
	    free(buf);
	}
	changed = TRUE;
    } else if (no && (!no->value || strcmp(value, no->value))) {
	free(no->value);
	no->value = strdup(value);
	no->flags |= NHOF_DIRTY;
	changed = TRUE;
    }
    if (no && no->addr) {
	free(*(char **)no->addr);
	*(char **)no->addr = strdup(value);
    }
    if (no->callback && changed)
	(*no->callback)(no->value);
}

void
nh_option_cache_set_bool(char *option, boolean value)
{
    boolean changed;
    char *buf;
    struct nh_option *no = nh_option_cache_getent(option);
    if (no && !(no->flags & NHOF_BOOLEAN) && no->value)
	panic("Setting boolean value for text option %s", option);
    if (nh_option_cache_flags & NHOF_NOCACHE) {
	buf = malloc(strlen(option) + value ? 1 : 2);
	if (buf) {
	    sprintf(buf, "%s%s", value ? "" : "!", option);
#ifdef GTK_PROXY
	    proxy_cb_parse_options(buf);
#else
	    parseoptions(buf, FALSE, FALSE);
#endif
	    free(buf);
	}
	changed = TRUE;
    } else if (no && no->value != (value ? boolean_set : boolean_reset)) {
	no->value = value ? boolean_set : boolean_reset;
	no->flags |= NHOF_DIRTY | NHOF_BOOLEAN;
	changed = TRUE;
    }
    if (no && no->addr)
	*(boolean *)no->addr = value;
    if (no->callback && changed)
	(*no->callback)(value);
}

void
nh_option_cache_set_addr(char *option, char **addr)
{
    struct nh_option *no = nh_option_cache_getent(option);
    if (no) {
	if (no->flags & NHOF_BOOLEAN)
	    panic("Setting address for boolean option %s", option);
	if (no->addr) {
	    free(*(char **)no->addr);
	    *(char **)no->addr = (char *)0;
	}
	no->addr = addr;
	*addr = nh_option_cache_get(option);
    }
}

void
nh_option_cache_set_bool_addr(char *option, boolean *addr)
{
    struct nh_option *no = nh_option_cache_getent(option);
    if (no) {
	if (!(no->flags & NHOF_BOOLEAN) && no->value)
	    panic("Setting boolean address for text option %s", option);
	no->addr = addr;
	no->flags |= NHOF_BOOLEAN;
	*addr = nh_option_cache_get_bool(option);
    }
}

void
nh_option_cache_set_callback(char *option, void (*func)(char *new_value))
{
    struct nh_option *no = nh_option_cache_getent(option);
    if (no) {
	if (no->flags & NHOF_BOOLEAN)
	    panic("Setting callback for boolean option %s", option);
	no->callback = (void (*)())func;
    }
}

void
nh_option_cache_set_bool_callback(char *option, void (*func)(boolean new_value))
{
    struct nh_option *no = nh_option_cache_getent(option);
    if (no) {
	if (!(no->flags & NHOF_BOOLEAN) && no->value)
	    panic("Setting boolean callback for text option %s", option);
	no->callback = (void (*)())func;
    }
}

char *
nh_option_cache_get(char *option)
{
    char *value;
    struct nh_option *no = nh_option_cache_getent(option);
    if (!no)
	return "";
    if (!no->value) {
#ifdef GTK_PROXY
	value = proxy_cb_get_option(option);
#else
	value = get_option(option);
#endif
	if (nh_option_cache_flags & NHOF_NOCACHE)
	    return value;
	no->value = strdup(value);
    }
    return no->value;
}

boolean
nh_option_cache_get_bool(char *option)
{
    char *value;
    int v;
    struct nh_option *no = nh_option_cache_getent(option);
    if (!no)
	return FALSE;
    if (!no->value) {
#ifdef GTK_PROXY
	value = proxy_cb_get_option(option);
#else
	value = get_option(option);
#endif
	no->flags |= NHOF_BOOLEAN;
	/*
	 * Some options (eg., number_pad) are booleans under some versions
	 * of NetHack but have been expanded to have more than one "true"
	 * value in later versions. As long as gtkhack doesn't need to
	 * differentiate between the various true values we can simply
	 * map them to true and false here. This provides full backwards
	 * compatibility. We assume that "0" means false and non-zero
	 * values mean various shades of true.
	 */
	if (!strcmp(value, "yes"))
	    value = boolean_set;
	else if (!strcmp(value, "no"))
	    value = boolean_reset;
	else if (sscanf(value, "%d", &v) == 1)
	    value = v ? boolean_set : boolean_reset;
	else
	    value = boolean_reset;
	if (nh_option_cache_flags & NHOF_NOCACHE)
	    return value == boolean_set;
	no->value = value;
    }
    return no->value == boolean_set;
}

void
GTK_ext_preference_update(const char *option, const char *value)
{
    boolean bv;
    struct nh_option *no = nh_option_cache_getent(option);
    if (no && no->flags & NHOF_BOOLEAN) {
	bv = !strcmp(value, "yes");
	if (!(nh_option_cache_flags & NHOF_NOCACHE) &&
	  no->value != (bv ? boolean_set : boolean_reset))
	    no->value = bv ? boolean_set : boolean_reset;
	if (no->addr)
	    *(boolean *)no->addr = bv;
	if (no->callback)
	    (*no->callback)(bv);
    } else if (no) {
	if (!(nh_option_cache_flags & NHOF_NOCACHE) &&
	  (!no->value || strcmp(value, no->value))) {
	    free(no->value);
	    no->value = strdup(value);
	}
	if (no->addr) {
	    free(*(char **)no->addr);
	    *(char **)no->addr = strdup(value);
	}
	if (no->callback)
	    (*no->callback)(value);
    }
}

static void
nh_option_get(void)
{
    int i;
    struct GTK_Option *p;

    if (!option_lock) {
	nh_option_cache_set("name",
	  gtk_entry_get_text(GTK_ENTRY(entry_plname)));
	nh_option_cache_set("catname",
	  gtk_entry_get_text(GTK_ENTRY(entry_catname)));
	nh_option_cache_set("dogname",
	  gtk_entry_get_text(GTK_ENTRY(entry_dogname)));
    }
    nh_option_cache_set("fruit", gtk_entry_get_text(GTK_ENTRY(entry_fruit)));

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_k)))
	nh_option_cache_set("pettype", "cat");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_d)))
	nh_option_cache_set("pettype", "dog");
    else
	nh_option_cache_set("pettype", "random");

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_menu_t)))
	nh_option_cache_set("menustyle", "traditional");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_menu_c)))
	nh_option_cache_set("menustyle", "combination");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_menu_p)))
	nh_option_cache_set("menustyle", "partial");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_menu_f)))
	nh_option_cache_set("menustyle", "full");

    for(i = 0; i < SIZE(gtk_option); i++) {
	char buf[BUFSIZ];
	p = &gtk_option[i];
	if (p->opt_name) {
	    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p->radio1)))
		nh_option_cache_set_bool(p->option, TRUE);
	    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p->radio2)))
		nh_option_cache_set_bool(p->option, FALSE);
	}
    }
    for(i = 0; i <= no_tileTab; i++)
	if (tileTab[i].data &&
	  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tileTab[i].data))){
	    nh_set_map_visual(i);
	    break;
	}
#ifdef GTK_PROXY
    proxy_cb_flush_screen();
#else
    flush_screen(cursx == u.ux && cursy == u.uy ? 1 : 0);
#endif
    nh_option_cache_sync();
}

static int
nh_option_has_changed(void)
{
    int i;
    struct GTK_Option *p;
    GtkWidget *button;
    char *s;
    boolean b;

    if (strcmp(nh_option_cache_get("name"),
      gtk_entry_get_text(GTK_ENTRY(entry_plname))))
	return TRUE;
    if (strcmp(nh_option_cache_get("catname"),
      gtk_entry_get_text(GTK_ENTRY(entry_catname))))
	return TRUE;
    if (strcmp(nh_option_cache_get("dogname"),
      gtk_entry_get_text(GTK_ENTRY(entry_dogname))))
	return TRUE;
    if (strcmp(nh_option_cache_get("fruit"),
      gtk_entry_get_text(GTK_ENTRY(entry_fruit))))
	return TRUE;

    s = nh_option_cache_get("pettype");
    switch (*s) {
	case 'c':
	    button = radio_k;
	    break;
	case 'd':
	    button = radio_d;
	    break;
	default:
	    button = radio_r;
	    break;
    }

    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
	return TRUE;

    s = nh_option_cache_get("menustyle");
    switch(*s) {
	case 'n':
	case 't':
	    button = radio_menu_t;
	    break;
	case 'c':
	    button = radio_menu_c;
	    break;
	case 'p':
	    button = radio_menu_p;
	    break;
	default:
	    button = radio_menu_f;
	    break;
    }
    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
	return TRUE;

    for (i = 0; i < SIZE(gtk_option); i++) {
	p = &gtk_option[i];
	if (p->opt_name && p->option) {
	    button = p->not ^ nh_option_cache_get_bool(p->option) ?
	      p->radio1 : p->radio2;
	    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
		return TRUE;
	}
    }
    i = nh_get_map_visual();
    return !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tileTab[i].data));
}

static GtkWidget*
nh_option_plname_new()
{
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox, *hbox2;
    GtkWidget *label;

    frame = gtk_frame_new("Player");

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");
    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);
    label = nh_gtk_new_and_pack(gtk_label_new("Name:"), hbox, "",
      FALSE, FALSE, NH_PAD);

    entry_plname = nh_gtk_new_and_pack(gtk_entry_new_with_max_length(PL_NSIZ),
      hbox, "", FALSE, FALSE, NH_PAD);

    hbox2 = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);
    gtk_widget_set_sensitive(GTK_WIDGET(label), !option_lock);

    return frame;
}

static GtkWidget *
nh_option_pet_kitten_new()
{
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *label;

    frame = gtk_frame_new("Kitten");
    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

    label = nh_gtk_new_and_pack(gtk_label_new("Name:"),
      hbox, "", FALSE, FALSE, NH_PAD);
    entry_catname = nh_gtk_new_and_pack(gtk_entry_new_with_max_length(PL_NSIZ),
      hbox, "", FALSE, FALSE, NH_PAD);

    gtk_widget_set_sensitive(GTK_WIDGET(label), !option_lock);

    return frame;
}

static GtkWidget *
nh_option_pet_dog_new()
{
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *label;

    frame = gtk_frame_new("Dog");
    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

    label = nh_gtk_new_and_pack(gtk_label_new("Name:"),
      hbox, "", FALSE, FALSE, NH_PAD);
    entry_dogname = nh_gtk_new_and_pack(gtk_entry_new_with_max_length(PL_NSIZ),
      hbox, "", FALSE, FALSE, NH_PAD);

    gtk_widget_set_sensitive(GTK_WIDGET(label), !option_lock);

    return frame;
}

static GtkWidget *
nh_option_pet_new()
{
    GtkWidget *frame;
    GtkWidget *label;
    GtkWidget *vbox;
    GtkWidget *hbox, *hbox2;
    GtkWidget *kitten;
    GtkWidget *dog;
    GSList *pet_group = NULL;

    frame = gtk_frame_new("Pet");

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

    kitten = nh_gtk_new_and_pack(nh_option_pet_kitten_new(), hbox, "",
      FALSE, FALSE, NH_PAD);

    dog = nh_gtk_new_and_pack(nh_option_pet_dog_new(), hbox, "",
      FALSE, FALSE, NH_PAD);

    hbox2 = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

    label = nh_gtk_new_and_pack(gtk_label_new("Start with:"), hbox2, "",
      FALSE, FALSE, NH_PAD);

    radio_k = nh_gtk_new_and_pack(
      gtk_radio_button_new_with_label(pet_group, "Kitten"), hbox2, "",
      FALSE, FALSE, NH_PAD);
    pet_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radio_k));

    radio_d = nh_gtk_new_and_pack(
      gtk_radio_button_new_with_label(pet_group, "Dog"), hbox2, "",
      FALSE, FALSE, NH_PAD);
    pet_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radio_d));

    radio_r = nh_gtk_new_and_pack(
      gtk_radio_button_new_with_label(pet_group, "Random"), hbox2, "",
      FALSE, FALSE, NH_PAD);
    pet_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radio_r));

    gtk_widget_set_sensitive(GTK_WIDGET(radio_k), !option_lock);
    gtk_widget_set_sensitive(GTK_WIDGET(radio_d), !option_lock);
    gtk_widget_set_sensitive(GTK_WIDGET(radio_r), !option_lock);

    return frame;
}

static GtkWidget *
nh_option_fruit_new()
{
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *label;

    frame = gtk_frame_new("Fruit");
    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

    label = nh_gtk_new_and_pack(gtk_label_new("Name:"),
      hbox, "", FALSE, FALSE, NH_PAD);
    entry_fruit = nh_gtk_new_and_pack(gtk_entry_new_with_max_length(PL_NSIZ),
      hbox, "", FALSE, FALSE, NH_PAD);

    return frame;
}

static GtkWidget *
nh_option_menu_new()
{
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GSList *menu_group = NULL;

    frame = gtk_frame_new("Menu style");

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

    radio_menu_t = nh_gtk_new_and_pack(
      gtk_radio_button_new_with_label(menu_group, "Traditional"), hbox, "",
      FALSE, FALSE, NH_PAD);
    menu_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radio_menu_t));

    radio_menu_p = nh_gtk_new_and_pack(
      gtk_radio_button_new_with_label(menu_group, "Partial"), hbox, "",
      FALSE, FALSE, NH_PAD);
    menu_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radio_menu_t));

    radio_menu_c = nh_gtk_new_and_pack(
      gtk_radio_button_new_with_label(menu_group, "Combination"), hbox, "",
      FALSE, FALSE, NH_PAD);
    menu_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radio_menu_t));

    radio_menu_f = nh_gtk_new_and_pack(
      gtk_radio_button_new_with_label(menu_group, "Full"), hbox, "",
      FALSE, FALSE, NH_PAD);
    menu_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radio_menu_t));

    return frame;
}


static GtkWidget *
nh_option_player_new()
{
    GtkWidget *vbox;

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_border_width(GTK_CONTAINER(vbox), NH_PAD);

    nh_gtk_new_and_pack(nh_option_plname_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

    nh_gtk_new_and_pack(nh_option_pet_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

    nh_gtk_new_and_pack(nh_option_fruit_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

    return vbox;
}

static GtkWidget *
nh_option_game_new()
{
    int i;
    GtkWidget *htmp;
    GtkWidget *ltmp;
    GtkWidget *stmp;
    GtkWidget *tbl;
    struct GTK_Option *p;

    tbl = gtk_table_new(sizeof(gtk_option)/sizeof(struct GTK_Option)*2, 3,
      FALSE);
    gtk_container_border_width(GTK_CONTAINER(tbl), NH_PAD);

    for(i = 0; i < SIZE(gtk_option); i++) {
	p = &gtk_option[i];
	p->group = NULL;
	if (p->opt_name) {
	    htmp = nh_gtk_new_and_attach(gtk_hbox_new(FALSE, 0), tbl, "",
	      0, 1, i * 2, i * 2 + 1);
	    ltmp = nh_gtk_new_and_pack(gtk_label_new(p->opt_name), htmp, "",
	      FALSE, FALSE, NH_PAD);

	    htmp = nh_gtk_new_and_attach(gtk_hbox_new(FALSE, 0), tbl, "",
	      1, 2, i * 2, i * 2 +1);

	    p->radio1 = nh_gtk_new_and_pack(
	      gtk_radio_button_new_with_label(p->group, p->on), htmp, "",
	      FALSE, FALSE, 0);
	    p->group = gtk_radio_button_group(GTK_RADIO_BUTTON(p->radio1));

	    htmp = nh_gtk_new_and_attach(gtk_hbox_new(FALSE, 0), tbl, "",
	      2, 3, i * 2, i * 2 +1);

	    p->radio2 = nh_gtk_new_and_pack(
	      gtk_radio_button_new_with_label(p->group, p->off), htmp, "", 
	      FALSE, FALSE, 0);
	    p->group = gtk_radio_button_group(GTK_RADIO_BUTTON(p->radio2));
	} else
	    stmp = nh_gtk_new_and_attach(gtk_hseparator_new(), tbl, "",
	      0, 3, i * 2 +1, i * 2 +2);
    }
    return tbl;
}

static GtkWidget *
nh_option_visual_new()
{
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GSList *menu_group = NULL;
    int i;

    frame = gtk_frame_new("Map visual");

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

    if (!nh_check_map_visual(0)) {
	tileTab[0].data = nh_gtk_new_and_pack(
	  gtk_radio_button_new_with_label(menu_group, "Characters"), hbox, "", 
	  FALSE, FALSE, NH_PAD);
	menu_group = gtk_radio_button_group(GTK_RADIO_BUTTON(tileTab[0].data));
    } else
      tileTab[0].data = NULL;

    for(i = 1; i <= no_tileTab; i++) {
	if (!nh_check_map_visual(i)) {
	    tileTab[i].data = nh_gtk_new_and_pack(
	      gtk_radio_button_new_with_label(menu_group, tileTab[i].ident),
	      hbox, "", FALSE, FALSE, NH_PAD);
	    menu_group =
	      gtk_radio_button_group(GTK_RADIO_BUTTON(tileTab[i].data));
	} else
	    tileTab[i].data = NULL;
    }

    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(tileTab[nh_get_map_visual()].data), TRUE);

    return frame;
}

static GtkWidget *
nh_option_display_new()
{
    GtkWidget *vbox;

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_border_width(GTK_CONTAINER(vbox), NH_PAD);

    nh_gtk_new_and_pack(nh_option_menu_new(), vbox, "", FALSE, FALSE, NH_PAD);

    nh_gtk_new_and_pack(nh_option_visual_new(), vbox, "", FALSE, FALSE, NH_PAD);

    return vbox;

}

void
nh_option_new()
{
    guint hid;	
    GtkWidget *w;
    GtkWidget *note;
    GtkWidget *vbox, *hbox;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *button3;

    w = nh_gtk_window_dialog(FALSE);
    gtk_container_border_width(GTK_CONTAINER(w), NH_PAD);
    nh_gtk_focus_set_master(GTK_WINDOW(w),
      GTK_SIGNAL_FUNC(default_key_press), 0, TRUE);
    hid = gtk_signal_connect(GTK_OBJECT(w), "destroy",
      GTK_SIGNAL_FUNC(default_destroy), &hid);
    nh_position_popup_dialog(GTK_WIDGET(w));

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), w, "");

    note = nh_gtk_new_and_pack(gtk_notebook_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

    nh_gtk_new_and_add(nh_option_player_new(), note, "");
    nh_gtk_new_and_add(nh_option_game_new(), note, "");
    nh_gtk_new_and_add(nh_option_display_new(), note, "");

    gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(note),
      gtk_notebook_get_nth_page(GTK_NOTEBOOK(note), 0), "Player");

    gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(note),
      gtk_notebook_get_nth_page(GTK_NOTEBOOK(note), 1), "Game");

    gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(note),
      gtk_notebook_get_nth_page(GTK_NOTEBOOK(note), 2), "Misc");

    hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

    button1 = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_OK),
      hbox, "", FALSE, FALSE, NH_PAD);
    button3 = nh_gtk_new_and_pack(gtk_button_new_with_label("More Options"),
      hbox, "", FALSE, FALSE, NH_PAD);
    button2 = nh_gtk_new_and_pack(gtk_button_new_from_stock(GTK_STOCK_CANCEL),
      hbox, "", FALSE, FALSE, NH_PAD);

    gtk_signal_connect(GTK_OBJECT(button1), "clicked",
      GTK_SIGNAL_FUNC(default_clicked), (gpointer)'\n');

    gtk_signal_connect(GTK_OBJECT(button2), "clicked",
      GTK_SIGNAL_FUNC(default_clicked), (gpointer)'\033');

    gtk_signal_connect(GTK_OBJECT(button3), "clicked",
      GTK_SIGNAL_FUNC(default_clicked), (gpointer)'m');

    nh_option_set();

    gtk_entry_set_editable(GTK_ENTRY(entry_plname), !option_lock);
    gtk_entry_set_editable(GTK_ENTRY(entry_dogname), !option_lock);
    gtk_entry_set_editable(GTK_ENTRY(entry_catname), !option_lock);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_plname), !option_lock);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_dogname), !option_lock);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_catname), !option_lock);

    gtk_widget_show_all(w);

    gtk_main();

    if (keysym == '\n')
      nh_option_get();

    if (hid > 0) {
      gtk_signal_disconnect(GTK_OBJECT(w), hid);

      gtk_widget_destroy(w);
    }
}

void
nh_option_lock(boolean lock)
{
    option_lock = lock;
}

/*
  $Id: gtkmisc.c,v 1.8 2002-06-23 18:31:23 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2000-2002
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

/*
 * Gtk+ font selector sometimes hangs in 1.2.x
 * (not that 1.3.x is much better; hopefully 2.0 will be stable)
 */
#if GTK_CHECK_VERSION(1,3,4) && defined(DEBUG)
#define NH_MAP_FONT_SELECTOR
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
#ifdef NH_MAP_FONT_SELECTOR
static GtkWidget *font_selection_map;
#endif

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
#ifdef RADAR
    {"display radar", "Yes", "No", "radar"},
#endif
    {NULL,},
    {"display experience points", "Yes", "No", "showexp"},
#ifdef SCORE_ON_BOTL
    {"display score points", "Yes", "No", "showscore"},
#endif
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

static void	nh_option_set(void);
static void	nh_option_get(void);
static int	nh_option_has_changed(void);
static int	nh_option_more_confirm(void);

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

    if(keysym == '\n' || keysym == '\033')
      gtk_main_quit();
    
    return FALSE;
}

static gint
default_clicked(GtkWidget *widget, gpointer data)
{
    if(data)
	keysym = (int)data;
    else
	keysym = '\n';

    if(keysym == 'm'){
	if (nh_option_has_changed()) {
	    switch(nh_option_more_confirm()){
		case -1:	/* Cancel "more options" */
		    keysym = 0;
		    return FALSE;
		default:	/* Discard changes */
		    break;
		case 1:		/* Apply changes first */
		    nh_option_get();
		    break;
	    }
	}
#ifdef GTK_PROXY
	proxy_cb_doset();
#else
	doset();
#endif
	nh_option_set();
    }
    else
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
    nh_gtk_focus_set_master(GTK_WINDOW(w), GTK_SIGNAL_FUNC(mc_key_press), 0);
    hid = gtk_signal_connect(
      GTK_OBJECT(w), "destroy",
      GTK_SIGNAL_FUNC(default_destroy), &hid);
    nh_position_popup_dialog(GTK_WIDGET(w));

    frame = nh_gtk_new_and_add(gtk_frame_new(NULL), w, "");
    gtk_container_border_width(GTK_CONTAINER(frame), NH_PAD);
    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");
    hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, 0);
    nh_gtk_new_and_pack(
      gtk_label_new("Commit changes made?"),
      hbox, "", FALSE, FALSE, 0);
    nh_gtk_new_and_pack(
      gtk_vseparator_new(), vbox, "", FALSE, FALSE, NH_PAD);
    hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

    button1 = nh_gtk_new_and_pack(
      gtk_button_new_with_label("Yes"), hbox, "",
      FALSE, FALSE, NH_PAD);
    button2 = nh_gtk_new_and_pack(
      gtk_button_new_with_label("No"), hbox, "",
      FALSE, FALSE, NH_PAD);
    button3 = nh_gtk_new_and_pack(
      gtk_button_new_with_label("Cancel"), hbox, "",
      FALSE, FALSE, NH_PAD);
    gtk_signal_connect(
      GTK_OBJECT(button1), "clicked",
      GTK_SIGNAL_FUNC(mc_clicked), (gpointer)'y');
    gtk_signal_connect(
      GTK_OBJECT(button2), "clicked",
      GTK_SIGNAL_FUNC(mc_clicked), (gpointer)'n');
    gtk_signal_connect(
      GTK_OBJECT(button3), "clicked",
      GTK_SIGNAL_FUNC(mc_clicked), (gpointer)'c');

    gtk_widget_show_all(w);
    gtk_grab_add(w);
    main_hook();

    if(hid > 0){
	gtk_signal_disconnect(GTK_OBJECT(w), hid);
	gtk_widget_destroy(w);
    }
    switch(keysym)
    {
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

#define NHOF_DIRTY	1
#define NHOF_BOOLEAN	2

static char *boolean_set = "yes";	/* Where value points for booleans */
static char *boolean_reset = "no";

static int nh_option_cache_size;
static struct nh_option {
    char *option;
    char *value;
    void *addr;
    unsigned flags;
} *nh_option_cache;

static struct nh_option *
nh_option_cache_getent(char *option)
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

void
nh_option_cache_set(char *option, const char *value)
{
    struct nh_option *no = nh_option_cache_getent(option);
    if (no && no->flags & NHOF_BOOLEAN)
	panic("Setting value for boolean option %s", option);
    if (no && (!no->value || strcmp(value, no->value))) {
	free(no->value);
	no->value = strdup(value);
	no->flags |= NHOF_DIRTY;
    }
    if (no && no->addr) {
	free(*(char **)no->addr);
	*(char **)no->addr = strdup(value);
    }
}

void
nh_option_cache_set_bool(char *option, boolean value)
{
    struct nh_option *no = nh_option_cache_getent(option);
    if (no && !(no->flags & NHOF_BOOLEAN) && no->value)
	panic("Setting boolean value for text option %s", option);
    if (no && no->value != (value ? boolean_set : boolean_reset)) {
	no->value = value ? boolean_set : boolean_reset;
	no->flags |= NHOF_DIRTY | NHOF_BOOLEAN;
    }
    if (no && no->addr)
	*(boolean *)no->addr = value;
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
	no->value = strdup(value);
    }
    return no->value;
}

boolean
nh_option_cache_get_bool(char *option)
{
    char *value;
    struct nh_option *no = nh_option_cache_getent(option);
    if (!no)
	return FALSE;
    if (!no->value) {
#ifdef GTK_PROXY
	value = proxy_cb_get_option(option);
#else
	value = get_option(option);
#endif
	no->value = strcmp(value, "yes") ? boolean_reset : boolean_set;
    }
    return no->value == boolean_set;
}

static void
nh_option_get(void)
{
    int i;
    struct GTK_Option *p;
#ifdef NH_EXTENSION
    char buf[BUFSIZ];
    char port[16];
#endif
#ifdef NH_MAP_FONT_SELECTOR
    GdkFont *font;
    gchar *font_name;
#endif

    nh_option_cache_set("name", gtk_entry_get_text(GTK_ENTRY(entry_plname)));
    nh_option_cache_set("catname",
	    gtk_entry_get_text(GTK_ENTRY(entry_catname)));
    nh_option_cache_set("dogname",
	    gtk_entry_get_text(GTK_ENTRY(entry_dogname)));
    nh_option_cache_set("fruit", gtk_entry_get_text(GTK_ENTRY(entry_fruit)));
#ifdef NH_EXTENSION
    set_homeurl(gtk_entry_get_text(GTK_ENTRY(entry_url)));
    sprintf(port, "%s", gtk_entry_get_text(GTK_ENTRY(entry_proxy_port)));
    if(*port)
	snprintf(buf, BUFSIZ, "%s:%s", 
	  gtk_entry_get_text(GTK_ENTRY(entry_proxy)), port);
    else
	snprintf(buf, BUFSIZ, "%s", 
	  gtk_entry_get_text(GTK_ENTRY(entry_proxy)));
    set_proxy(buf);
#endif

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_k)))
	nh_option_cache_set("pettype", "cat");
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_d)))
	nh_option_cache_set("pettype", "dog");
    else
	nh_option_cache_set("pettype", "random");

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_menu_t)))
	nh_option_cache_set("menustyle", "traditional");
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_menu_c)))
	nh_option_cache_set("menustyle", "combination");
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_menu_p)))
	nh_option_cache_set("menustyle", "partial");
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_menu_f)))
	nh_option_cache_set("menustyle", "full");

    for(i=0 ; i<sizeof(gtk_option)/sizeof(struct GTK_Option) ; ++i){
	char buf[BUFSIZ];
	p = &gtk_option[i];
	if (p->opt_name) {
	    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p->radio1)))
#ifdef GTK_PROXY
		proxy_cb_parse_options(p->option);
#else
		parseoptions(p->option, FALSE, FALSE);
#endif
	    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p->radio2))) {
		sprintf(buf, "!%s", p->option);
#ifdef GTK_PROXY
		proxy_cb_parse_options(buf);
#else
		parseoptions(buf, FALSE, FALSE);
#endif
	    }
	}
    }
#ifdef NH_MAP_FONT_SELECTOR
    font = gtk_font_selection_get_font(GTK_FONT_SELECTION(font_selection_map));
    if (font) {
	font_name = gtk_font_selection_get_font_name(
	  GTK_FONT_SELECTION(font_selection_map));
	nh_set_map_font(font, font_name);
#ifdef DEBUG
	fprintf(stderr,"Map font set to \"%s\"\n", font_name);
#endif
	g_free(font_name);
    }
#endif
    for(i = 0; i <= no_tileTab; i++)
	if(tileTab[i].data &&
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
#ifdef NH_EXTENSION
    char buf[BUFSIZ];
    char port[16];
#endif

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
#ifdef NH_EXTENSION
    if (strcmp(get_homeurl(), gtk_entry_get_text(GTK_ENTRY(entry_url))))
	return TRUE;
    sprintf(port, "%s", gtk_entry_get_text(GTK_ENTRY(entry_proxy_port)));
    if(*port)
	snprintf(buf, BUFSIZ, "%s:%s", 
	  gtk_entry_get_text(GTK_ENTRY(entry_proxy)), port);
    else
	snprintf(buf, BUFSIZ, "%s", 
	  gtk_entry_get_text(GTK_ENTRY(entry_proxy)));
    if (strcmp(get_proxy(), buf))
	return TRUE;
#endif

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

    if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
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
/*  gtk_container_border_width(GTK_CONTAINER(frame), NH_PAD);*/
  
  vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");
  hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "", FALSE, FALSE, NH_PAD);
  label = nh_gtk_new_and_pack(gtk_label_new("Name:"), hbox, "", FALSE, FALSE, NH_PAD);

  entry_plname = nh_gtk_new_and_pack(gtk_entry_new_with_max_length(PL_NSIZ),
			      hbox, "", FALSE, FALSE, NH_PAD);
  gtk_entry_set_text(GTK_ENTRY(entry_plname), nh_option_cache_get("name"));

  hbox2 = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), vbox, "", FALSE, FALSE, NH_PAD);
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
  vbox = nh_gtk_new_and_add(
      gtk_vbox_new(FALSE, 0), frame, "");

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

  label = nh_gtk_new_and_pack(
      gtk_label_new("Name:"),
      hbox, "", FALSE, FALSE, NH_PAD);
  entry_catname = nh_gtk_new_and_pack(
      gtk_entry_new_with_max_length(PL_NSIZ),
      hbox, "", FALSE, FALSE, NH_PAD);

  gtk_entry_set_text(GTK_ENTRY(entry_catname), nh_option_cache_get("catname"));

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
  vbox = nh_gtk_new_and_add(
      gtk_vbox_new(FALSE, 0), frame, "");

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

  label = nh_gtk_new_and_pack(
      gtk_label_new("Name:"),
      hbox, "", FALSE, FALSE, NH_PAD);
  entry_dogname = nh_gtk_new_and_pack(
      gtk_entry_new_with_max_length(PL_NSIZ),
      hbox, "", FALSE, FALSE, NH_PAD);

  gtk_entry_set_text(GTK_ENTRY(entry_dogname), nh_option_cache_get("dogname"));

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
  GSList    *pet_group = NULL;

  frame = gtk_frame_new("Pet");
/*  gtk_container_border_width(GTK_CONTAINER(w), NH_PAD);*/

  
  vbox = nh_gtk_new_and_add(
      gtk_vbox_new(FALSE, 0), frame, "");

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

  kitten = nh_gtk_new_and_pack(
      nh_option_pet_kitten_new(), hbox, "",
      FALSE, FALSE, NH_PAD);

  dog = nh_gtk_new_and_pack(
      nh_option_pet_dog_new(), hbox, "",
      FALSE, FALSE, NH_PAD);

  hbox2 = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

  label = nh_gtk_new_and_pack(
      gtk_label_new("Start with:"), hbox2, "",
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
  vbox = nh_gtk_new_and_add(
      gtk_vbox_new(FALSE, 0), frame, "");

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

  label = nh_gtk_new_and_pack(
      gtk_label_new("Name:"),
      hbox, "", FALSE, FALSE, NH_PAD);
  entry_fruit = nh_gtk_new_and_pack(
      gtk_entry_new_with_max_length(PL_NSIZ),
      hbox, "", FALSE, FALSE, NH_PAD);

  gtk_entry_set_text(GTK_ENTRY(entry_fruit), nh_option_cache_get("fruit"));

  return frame;
}

#ifdef NH_EXTENSION_REPORT
static GtkWidget *
nh_option_url_new()
{
  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *label;

  frame = gtk_frame_new("スコア報告時のURL");

  vbox = nh_gtk_new_and_add(
      gtk_vbox_new(FALSE, 0), frame, "");

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

  label = nh_gtk_new_and_pack(
      gtk_label_new("URL:"),
      hbox, "", FALSE, FALSE, NH_PAD);

  entry_url = nh_gtk_new_and_pack(
      gtk_entry_new_with_max_length(128),
      hbox, "", TRUE, TRUE, NH_PAD);

  gtk_entry_set_text(GTK_ENTRY(entry_url), (const gchar *)get_homeurl());

  return frame;
}

static GtkWidget *
nh_option_proxy_new()
{
  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *label;

  frame = gtk_frame_new("プロキシ");

  vbox = nh_gtk_new_and_add(
      gtk_vbox_new(FALSE, 0), frame, "");

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

  label = nh_gtk_new_and_pack(
      gtk_label_new("サーバ:"),
      hbox, "", FALSE, FALSE, NH_PAD);

  entry_proxy = nh_gtk_new_and_pack(
      gtk_entry_new_with_max_length(128),
      hbox, "", TRUE, TRUE, NH_PAD);

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

  label = nh_gtk_new_and_pack(
      gtk_label_new("ポート:"),
      hbox, "", FALSE, FALSE, NH_PAD);

  entry_proxy_port = nh_gtk_new_and_pack(
      gtk_entry_new_with_max_length(6),
      hbox, "", FALSE, FALSE, NH_PAD);

  {
      char *proxy;
      char port[16];
      sprintf(port, "%d", get_proxy_port());

      proxy = get_proxy_host();

      gtk_entry_set_text(GTK_ENTRY(entry_proxy), (const gchar *)proxy);
      if(*proxy)
	  gtk_entry_set_text(GTK_ENTRY(entry_proxy_port), (const gchar *)port);
  }

  return frame;
}
#endif

static GtkWidget *
nh_option_menu_new()
{
  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GSList    *menu_group = NULL;

  frame = gtk_frame_new("Menu style");

  vbox = nh_gtk_new_and_add(
      gtk_vbox_new(FALSE, 0), frame, "");

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0),
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

  nh_gtk_new_and_pack(
      nh_option_plname_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

  nh_gtk_new_and_pack(
      nh_option_pet_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

  nh_gtk_new_and_pack(
      nh_option_fruit_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

#ifdef NH_EXTENSION_REPORT
  nh_gtk_new_and_pack(
      nh_option_url_new(), vbox, "",
      FALSE, FALSE, NH_PAD);
#endif

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

  tbl = gtk_table_new(sizeof(gtk_option)/sizeof(struct GTK_Option)*2, 3, FALSE);
  gtk_container_border_width(GTK_CONTAINER(tbl), NH_PAD);

  for(i=0 ; i<sizeof(gtk_option)/sizeof(struct GTK_Option) ; ++i){
      p = &gtk_option[i];
      p->group = NULL;
      if(p->opt_name){
	  htmp = nh_gtk_new_and_attach(
	      gtk_hbox_new(FALSE, 0), tbl, "",
	      0, 1, i*2, i*2 + 1);
	  ltmp = nh_gtk_new_and_pack(
	      gtk_label_new(p->opt_name), htmp, "",
	      FALSE, FALSE, NH_PAD);
	  
	  htmp = nh_gtk_new_and_attach(
	      gtk_hbox_new(FALSE, 0), tbl, "",
	      1, 2, i*2, i*2 +1);
	  
	  p->radio1 = nh_gtk_new_and_pack(
	      gtk_radio_button_new_with_label(p->group, p->on), htmp, "",
	      FALSE, FALSE, 0);
	  p->group = gtk_radio_button_group(GTK_RADIO_BUTTON(p->radio1));
	  
	  htmp = nh_gtk_new_and_attach(
	      gtk_hbox_new(FALSE, 0), tbl, "",
	      2, 3, i*2, i*2 +1);
	  
	  p->radio2 = nh_gtk_new_and_pack(
	      gtk_radio_button_new_with_label(p->group, p->off), htmp, "", 
	      FALSE, FALSE, 0);
	  p->group = gtk_radio_button_group(GTK_RADIO_BUTTON(p->radio2));
      }
      else{
	  stmp = nh_gtk_new_and_attach(
	      gtk_hseparator_new(), tbl, "",
	      0, 3, i*2 +1, i*2 +2);
      }
  }
  return tbl;
}

static GtkWidget *
nh_option_visual_new()
{
  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GSList    *menu_group = NULL;
  int       i;

  frame = gtk_frame_new("Map visual");

  vbox = nh_gtk_new_and_add(
      gtk_vbox_new(FALSE, 0), frame, "");

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0),
      vbox, "", FALSE, FALSE, NH_PAD);

  if (!nh_check_map_visual(0)){
      tileTab[0].data = nh_gtk_new_and_pack(
	  gtk_radio_button_new_with_label(menu_group, "Characters"), hbox, "", 
	  FALSE, FALSE, NH_PAD);
      menu_group = gtk_radio_button_group(GTK_RADIO_BUTTON(tileTab[0].data));
  }
  else
      tileTab[0].data = NULL;

  for(i = 1; i <= no_tileTab; i++){
      if (!nh_check_map_visual(i)){
	  tileTab[i].data = nh_gtk_new_and_pack(
	      gtk_radio_button_new_with_label(menu_group, tileTab[i].ident),
	      hbox, "", FALSE, FALSE, NH_PAD);
	  menu_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(tileTab[i].data));
      }
      else
	  tileTab[i].data = NULL;
  }

  gtk_toggle_button_set_active(
    GTK_TOGGLE_BUTTON(tileTab[nh_get_map_visual()].data), TRUE);

  return frame;
}

#ifdef NH_MAP_FONT_SELECTOR
static GtkWidget *
nh_option_font_new()
{
    GtkWidget *frame;
    GtkWidget *vbox;
    gchar *font_name;

    frame = gtk_frame_new("Map font");

    vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), frame, "");

    font_selection_map = nh_gtk_new_and_pack(gtk_font_selection_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

    font_name = nh_get_map_font();
    if (font_name) {
#ifdef DEBUG
	fprintf(stderr,"Map font currently \"%s\"\n", font_name);
#endif
	(void)gtk_font_selection_set_font_name(
	  GTK_FONT_SELECTION(font_selection_map), font_name);
	g_free(font_name);
    }
#ifdef DEBUG
    else
	fprintf(stderr,"Map font currently unset\n");
#endif

    return frame;
}
#endif

static GtkWidget *
nh_option_display_new()
{
  GtkWidget *vbox;

  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_border_width(GTK_CONTAINER(vbox), NH_PAD);

#ifdef NH_EXTENSION_REPORT
  nh_gtk_new_and_pack(
      nh_option_proxy_new(), vbox, "",
      FALSE, FALSE, NH_PAD);
#endif

  nh_gtk_new_and_pack(
      nh_option_menu_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

  nh_gtk_new_and_pack(
      nh_option_visual_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

#ifdef NH_MAP_FONT_SELECTOR
  nh_gtk_new_and_pack(
      nh_option_font_new(), vbox, "",
      FALSE, FALSE, NH_PAD);
#endif

  return vbox;

}


void
nh_option_new()
{
  guint	hid;	
  GtkWidget *w;
  GtkWidget *note;
  GtkWidget *vbox, *hbox;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;

  w = nh_gtk_window_dialog(TRUE);
  gtk_container_border_width(GTK_CONTAINER(w), NH_PAD);
  nh_gtk_focus_set_master(GTK_WINDOW(w),
    GTK_SIGNAL_FUNC(default_key_press), 0);
  hid = gtk_signal_connect(
      GTK_OBJECT(w), "destroy",
      GTK_SIGNAL_FUNC(default_destroy), &hid);
  nh_position_popup_dialog(GTK_WIDGET(w));

  vbox = nh_gtk_new_and_add(gtk_vbox_new(FALSE, 0), w, "");

  note = nh_gtk_new_and_pack(
      gtk_notebook_new(), vbox, "",
      FALSE, FALSE, NH_PAD);

  nh_gtk_new_and_add(nh_option_player_new(), note, "");
  nh_gtk_new_and_add(nh_option_game_new(), note, "");
  nh_gtk_new_and_add(nh_option_display_new(), note, "");

  gtk_notebook_set_tab_label_text(
      GTK_NOTEBOOK(note),
      gtk_notebook_get_nth_page(GTK_NOTEBOOK(note), 0), 
      "Player");

  gtk_notebook_set_tab_label_text(
      GTK_NOTEBOOK(note),
      gtk_notebook_get_nth_page(GTK_NOTEBOOK(note), 1), 
      "Game");

  gtk_notebook_set_tab_label_text(
      GTK_NOTEBOOK(note),
      gtk_notebook_get_nth_page(GTK_NOTEBOOK(note), 2), 
      "Misc");

  hbox = nh_gtk_new_and_pack(
      gtk_hbox_new(FALSE, 0), vbox, "",
      FALSE, FALSE, NH_PAD);

  button1 = nh_gtk_new_and_pack(
      gtk_button_new_with_label("OK"), hbox, "",
      FALSE, FALSE, NH_PAD);
  button3 = nh_gtk_new_and_pack(
      gtk_button_new_with_label("More Options"), hbox, "",
      FALSE, FALSE, NH_PAD);
  button2 = nh_gtk_new_and_pack(
      gtk_button_new_with_label("Cancel"), hbox, "",
      FALSE, FALSE, NH_PAD);

  gtk_signal_connect(
      GTK_OBJECT(button1), "clicked",
      GTK_SIGNAL_FUNC(default_clicked), (gpointer)'\n');

  gtk_signal_connect(
      GTK_OBJECT(button2), "clicked",
      GTK_SIGNAL_FUNC(default_clicked), (gpointer)'\033');

  gtk_signal_connect(
      GTK_OBJECT(button3), "clicked",
      GTK_SIGNAL_FUNC(default_clicked), (gpointer)'m');

  nh_option_set();

  gtk_entry_set_editable(GTK_ENTRY(entry_plname), !option_lock);
  gtk_entry_set_editable(GTK_ENTRY(entry_dogname), !option_lock);
  gtk_entry_set_editable(GTK_ENTRY(entry_catname), !option_lock);
  gtk_widget_set_sensitive(GTK_WIDGET(entry_plname), !option_lock);
  gtk_widget_set_sensitive(GTK_WIDGET(entry_dogname), !option_lock);
  gtk_widget_set_sensitive(GTK_WIDGET(entry_catname), !option_lock);

  gtk_widget_show_all(w);

  gtk_grab_add(w);

  main_hook();

  if(keysym == '\n')
      nh_option_get();

  if(hid > 0){
      gtk_signal_disconnect(GTK_OBJECT(w), hid);

      gtk_widget_destroy(w);
  }
}

void
nh_option_lock(boolean lock)
{
    option_lock = lock;
}

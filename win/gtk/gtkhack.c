/* $Id: gtkhack.c,v 1.10 2004-04-19 06:56:42 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002-2004 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdio.h>
#include <signal.h>
#include "winGTK.h"
#include "prxyclnt.h"
#include "proxycb.h"
#include "gtkconnect.h"
#include "gtksupport.h"

static GtkWidget *treeview = NULL;

GtkTreeRowReference *GTK_default_connection = NULL;
GtkTreeRowReference *GTK_current_connection;
GtkListStore *GTK_connections;
GtkTextBuffer *GTK_nhext_log = NULL;

struct lookup_datum {
    const char *name;
    GtkTreeRowReference *ref;
};

static gboolean lookup(GtkTreeModel *model, GtkTreePath *path,
  GtkTreeIter *iter, gpointer data)
{
    gchar *name;
    struct lookup_datum *datum = data;
    gtk_tree_model_get(model, iter, COLUMN_NAME, &name, -1);
    if (name && !strcmp(name, datum->name))
	datum->ref = gtk_tree_row_reference_new(model, path);
    g_free(name);
    return !!datum->ref;
}

GtkTreeRowReference *
GTK_connection_lookup(const char *name)
{
    GtkTreePath *path;
    struct lookup_datum datum;
    datum.name = name;
    datum.ref = NULL;
    gtk_tree_model_foreach(GTK_TREE_MODEL(GTK_connections),
      (GtkTreeModelForeachFunc)lookup, &datum);
    return datum.ref;
}

struct update_datum {
    const char *name;
    const char *scheme;
    const char *address;
    unsigned long flags;
    gboolean found;
};

static gboolean update(GtkTreeModel *model, GtkTreePath *path,
  GtkTreeIter *iter, gpointer data)
{
    gchar *name;
    struct update_datum *datum = data;
    gtk_tree_model_get(model, iter, COLUMN_NAME, &name, -1);
    if (name && !strcmp(name, datum->name)) {
	gtk_list_store_set(GTK_connections, iter, COLUMN_NAME, datum->name,
	  COLUMN_SCHEME, datum->scheme, COLUMN_ADDRESS, datum->address,
	  COLUMN_FLAGS, datum->flags, -1);
	datum->found = TRUE;
    }
    g_free(name);
    return datum->found;
}

void
GTK_connection_set_default(const char *name)
{
    gtk_tree_row_reference_free(GTK_default_connection);
    GTK_default_connection = GTK_connection_lookup(name);
}

void
GTK_connection_set_weak_default(const char *name)
{
    if (!GTK_default_connection ||
      !gtk_tree_row_reference_valid(GTK_default_connection))
	GTK_connection_set_default(name);
}

/*
 * Add a new connection to the model (or update the existing one if
 * a connection of the same name exists). If there is no default
 * connection then make this one the default one. In all cases, make
 * the new/updated connection the current selection.
 */

void
GTK_connection_add(const char *name, const char *scheme, const char *address,
  unsigned long flags)
{
    GtkTreeRowReference *ref;
    GtkTreePath *path;
    GtkTreeSelection *sel;
    GtkTreeIter iter;
    struct update_datum datum;
    datum.name = name;
    datum.scheme = scheme;
    datum.address = address;
    datum.flags = flags;
    datum.found = FALSE;
    gtk_tree_model_foreach(GTK_TREE_MODEL(GTK_connections),
      (GtkTreeModelForeachFunc)update, &datum);
    if (!datum.found) {
	gtk_list_store_append(GTK_connections, &iter);
	gtk_list_store_set(GTK_connections, &iter, COLUMN_NAME, name,
	  COLUMN_SCHEME, scheme, COLUMN_ADDRESS, address,
	  COLUMN_FLAGS, flags, -1);
    }
    GTK_connection_set_weak_default(name);
    if (treeview) {
	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	ref = GTK_connection_lookup(name);
	path = gtk_tree_row_reference_get_path(ref);
	if (gtk_tree_model_get_iter(GTK_TREE_MODEL(GTK_connections), &iter,
	  path))
	    gtk_tree_selection_select_iter(sel, &iter);
	gtk_tree_row_reference_free(ref);
	gtk_tree_path_free(path);
    }
}

static gboolean save(GtkTreeModel *model, GtkTreePath *path,
  GtkTreeIter *iter, gpointer data)
{
    int n = 0;
    gchar *name, *scheme, *address;
    gulong flags;
    GString *str = data;
    gtk_tree_model_get(model, iter, COLUMN_NAME, &name, COLUMN_SCHEME, &scheme, 
      COLUMN_ADDRESS, &address, COLUMN_FLAGS, &flags, -1);
    g_string_append_printf(str, "{\"%s\",\"%s\",\"%s\",[",
      g_strescape(name, ""), scheme, g_strescape(address, ""));
    if (flags & PROXY_CLNT_SYNCHRONOUS) {
	n++;
	g_string_append(str, "\"synchronous\"");
    }
    if (flags & PROXY_CLNT_LOGGED) {
	if (n++)
	    g_string_append(str, ", ");
	g_string_append(str, "\"logged\"");
    }
    g_string_append(str, "]},");
    g_free(name);
    g_free(scheme);
    g_free(address);
    return FALSE;
}

void
GTK_connection_save(struct gtkhackrc *rc)
{
    gchar *name;
    GtkTreeIter iter;
    GtkTreePath *path;
    GString *str = g_string_new("connections = [");
    gtk_tree_model_foreach(GTK_TREE_MODEL(GTK_connections),
      (GtkTreeModelForeachFunc)save, str);
    g_string_truncate(str, str->len - 1);	/* Remove final ',' */
    g_string_append_c(str, ']');
    nh_gtkhackrc_store(rc, str->str);
    if (GTK_default_connection &&
      gtk_tree_row_reference_valid(GTK_default_connection)) {
	path = gtk_tree_row_reference_get_path(GTK_default_connection);
	if (gtk_tree_model_get_iter(GTK_TREE_MODEL(GTK_connections), &iter,
	  path)) {
	    gtk_tree_model_get(GTK_TREE_MODEL(GTK_connections), &iter,
	      COLUMN_NAME, &name, -1);
	    g_string_printf(str, "default_connection = \"%s\"",
	      g_strescape(name, ""));
	    g_free(name);
	    nh_gtkhackrc_store(rc, str->str);
	}
	gtk_tree_path_free(path);
    }
    (void)g_string_free(str, TRUE);
}

static void GTK_proxy_clnt_errhandler(const char *error)
{
    GtkWidget *w;
    w = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
      error);
    gtk_dialog_run(GTK_DIALOG(w));
    gtk_widget_destroy(w);
    return;
}

static void GTK_nhext_errhandler(int class, const char *error)
{
    GtkWidget *w;
    if (class == EXT_ERROR_COMMS) {
	w = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_NONE,
	  "Connection to game server lost.\n\n%s", error);
	gtk_dialog_add_button(GTK_DIALOG(w), GTK_STOCK_QUIT, 0);
	gtk_dialog_run(GTK_DIALOG(w));
	exit(1);
    } else
	GTK_proxy_clnt_errhandler(error);
}

static int GTK_proxy_clnt_authhandler(unsigned long methods)
{
    int r;
    char *s;
    GtkSizeGroup *group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
    GtkWidget *dialog, *w, *hbox, *vbox, *user, *passwd;

    /*
     * Always use method 1 if available (simple password).
     * Otherwise, use method 0 (no authentication).
     * If neither method is allowed, then fail.
     */
    if (!(methods & (1UL << 1))) {
	if (methods & (1UL << 0)) {
	    win_proxy_clnt_settag("authmethod", "0");
	    return 0;
	}
	s = g_strdup_printf("Game server requires authentication,\n"
	  "but doesn't allow any of my supported methods.\n\n"
	  "Supported methods: 0,1\n"
	  "Allowed methods: %s",win_proxy_clnt_gettag("authmethods"));
	GTK_proxy_clnt_errhandler(s);
	g_free(s);
	win_proxy_clnt_settag("mesg", "No supported authentication methods");
	return 1;
    }

    dialog = gtk_dialog_new_with_buttons("Authentication",
      GTK_WINDOW(main_window),
      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 12);

    vbox = gtk_vbox_new(FALSE, 6);

    hbox = gtk_hbox_new(FALSE, 6);
    w = gtk_label_new_with_mnemonic("_User:");
    gtk_box_pack_start(GTK_BOX(hbox), w, FALSE, FALSE, 0);
    gtk_size_group_add_widget(group, w);
    user = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox), user, FALSE, FALSE, 0);
    gtk_label_set_mnemonic_widget(GTK_LABEL(w), user);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    hbox = gtk_hbox_new(FALSE, 6);
    w = gtk_label_new_with_mnemonic("_Password:");
    gtk_box_pack_start(GTK_BOX(hbox), w, FALSE, FALSE, 0);
    gtk_size_group_add_widget(group, w);
    passwd = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(passwd), FALSE);
    gtk_entry_set_activates_default(GTK_ENTRY(passwd), TRUE);
    g_signal_connect_swapped(G_OBJECT(user), "activate",
      (GCallback)gtk_widget_grab_focus, passwd);
    gtk_box_pack_start(GTK_BOX(hbox), passwd, FALSE, FALSE, 0);
    gtk_label_set_mnemonic_widget(GTK_LABEL(w), passwd);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    hbox = gtk_hbox_new(FALSE, 0);
    w = gtk_label_new("    ");
    gtk_box_pack_start(GTK_BOX(hbox), w, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

    vbox = gtk_vbox_new(FALSE, 6);
    w = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(w),
      "<span weight=\"bold\">Your details</span>");
    gtk_misc_set_alignment(GTK_MISC(w), 0, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox), w, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    w = gtk_vbox_new(FALSE, 18);
    gtk_box_pack_start(GTK_BOX(w), vbox, TRUE, TRUE, 0);
    gtk_widget_show_all(w);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), w, TRUE, TRUE, 0);

    do {
	r = gtk_dialog_run(GTK_DIALOG(dialog));
    } while (r != GTK_RESPONSE_ACCEPT);

    win_proxy_clnt_settag("authmethod", "1");
    win_proxy_clnt_settag("username", gtk_entry_get_text(GTK_ENTRY(user)));
    win_proxy_clnt_settag("password", gtk_entry_get_text(GTK_ENTRY(passwd)));
    g_object_unref(group);
    gtk_widget_destroy(dialog);
    return 0;
}

void
GTK_ext_send_config_file(int fh)
{
    FILE *fp;
    char buffer[512];
    int nb;
#ifdef WIN32
    fp = fopen("defaults.nh", "r");
#else
    int i;
    const char *home, *game;
    char *buf, *s;
    home = getenv("HOME");
    game = win_proxy_clnt_gettag("game");
    if (home && game) {
	buf = malloc(strlen(home) + strlen(game) + 5);
	if (buf) {
	    /* ~/.@LOWER@rc */
	    strcpy(buf, home);
	    s = eos(buf);
	    *s++ = '/';
	    *s++ = '.';
	    /* Convert the ASCII encoded game into lower case */
	    for(i = 0; game[i]; i++)
		*s++ = game[i] >= 'A' && game[i] <= 'Z' ?
		  game[i] | 0x20 : game[i];
	    *s++ = 'r';
	    *s++ = 'c';
	    *s = '\0';
	}
	fp = fopen(buf, "r");
	free(buf);
    }
    else
	return;
#endif
    if (fp) {
	while((nb = fread(buffer, 1, 512, fp)) > 0)
	    proxy_cb_dlbh_fwrite(buffer, nb, 1, fh);
	fclose(fp);
    }
}

static gtkhack_nhext_log(void *handle, void *buf, unsigned int len)
{
    GtkTextBuffer *t = GTK_TEXT_BUFFER(handle);
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(t, &iter);
    gtk_text_buffer_insert(t, &iter, buf, len);
    return len;
}

void
gtkhack_enable_logging(gboolean setting)
{
    if (setting) {
	if (!GTK_nhext_log)
	    GTK_nhext_log = gtk_text_buffer_new(NULL);
	win_proxy_clnt_log_open(gtkhack_nhext_log, GTK_nhext_log);
    } else
	win_proxy_clnt_log_open(NULL, NULL);
}

GtkWidget*
GTK_troubleshooting_new(void)
{
    unsigned long flags;
    GtkTreePath *path;
    GtkTreeIter iter;
    GtkWidget *cb;
    GtkWidget *w = create_Troubleshooting();
    path = gtk_tree_row_reference_get_path(GTK_current_connection);
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(GTK_connections), &iter, path)) {
	gtk_tree_model_get(GTK_TREE_MODEL(GTK_connections), &iter,
	  COLUMN_FLAGS, &flags, -1);
    }
    gtk_tree_path_free(path);
    cb = lookup_widget(w, "DisableAsync");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb),
      flags & PROXY_CLNT_SYNCHRONOUS);
    cb = lookup_widget(w, "EnableLogging");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb),
      flags & PROXY_CLNT_LOGGED);
    /* Stop the clicked signal from propogating from the ViewLog button
     * to the dialog and thus causing gtk_dialog_run() to return. Really,
     * we should arrange for the ViewLog button to be a non-activatable
     * widget but glade doesn't have support for this.
     */
    g_signal_handlers_block_matched(lookup_widget(w, "ViewLog"),
      G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_DATA,
      g_signal_lookup("clicked", GTK_TYPE_BUTTON), 0, NULL, NULL, G_OBJECT(w));
    gtk_widget_show(w);
    return w;
}

int
main(int argc, char **argv)
{
    int retval;
    char *s;
    GtkWidget *w, *connections, *progress;
    GdkCursor *cursor;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreeSelection *sel;
    GtkTreePath *path;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GError *err;
    gchar *name, *scheme, *address, *os_path;
    gulong flags;
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif
    g_type_init();
    GTK_connections = gtk_list_store_new(N_COLUMNS,
      G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_ULONG);
    GTK_init_gtk(&argc, argv);
    connections = create_Connections();
    /* Stop the clicked signal from propogating from the revert button
     * to the dialog and thus causing gtk_dialog_run() to return. Really,
     * we should arrange for the revert button to be a non-activatable
     * widget but glade doesn't have support for this.
     */
    g_signal_handlers_block_matched(lookup_widget(connections, "revertbutton1"),
      G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_DATA,
      g_signal_lookup("clicked", GTK_TYPE_BUTTON), 0, NULL, NULL,
      G_OBJECT(connections));
    if (!gtk_tree_model_iter_n_children(GTK_TREE_MODEL(GTK_connections), NULL))
	GTK_connection_add("local", "file", "slashem", 0UL);
    treeview = lookup_widget(connections, "ConnectionsTreeView");
    g_object_ref(treeview);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview),
      GTK_TREE_MODEL(GTK_connections));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", renderer,
      "text", COLUMN_NAME, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    column = gtk_tree_view_column_new_with_attributes("Scheme", renderer,
      "text", COLUMN_SCHEME, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    column = gtk_tree_view_column_new_with_attributes("Address", renderer,
      "text", COLUMN_ADDRESS, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    if (GTK_default_connection &&
      gtk_tree_row_reference_valid(GTK_default_connection)) {
	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	path = gtk_tree_row_reference_get_path(GTK_default_connection);
	if (gtk_tree_model_get_iter(GTK_TREE_MODEL(GTK_connections), &iter,
	  path))
	    gtk_tree_selection_select_iter(sel, &iter);
	gtk_tree_path_free(path);
    }
    while (gtk_dialog_run(GTK_DIALOG(connections)) == GTK_RESPONSE_OK) {
	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
	    path = gtk_tree_model_get_path(model, &iter);
	    GTK_current_connection =  gtk_tree_row_reference_new(model, path);
	    gtk_tree_path_free(path);
	    gtk_tree_model_get(model, &iter, COLUMN_NAME, &name,
	      COLUMN_SCHEME, &scheme, COLUMN_ADDRESS, &address,
	      COLUMN_FLAGS, &flags, -1);
	    GTK_connection_set_weak_default(name);
	    gtk_widget_hide(connections);
	    progress = gtk_message_dialog_new(NULL,
	      0, GTK_MESSAGE_INFO, GTK_BUTTONS_CANCEL,
	      "Opening %s connection.\n\nEstablishing connection...", name);
	    gtk_window_set_title(GTK_WINDOW(progress),
	      "GtkHack: Connecting...");
	    proxy_svc_set_ext_procs(win_GTK_init, &GTK_ext_procs);
	    proxy_clnt_set_errhandler(GTK_proxy_clnt_errhandler);
	    proxy_clnt_set_authhandler(GTK_proxy_clnt_authhandler);
	    nhext_set_errhandler(GTK_nhext_errhandler);
	    win_proxy_clnt_set_flags(PROXY_CLNT_SYNCHRONOUS, flags);
	    gtkhack_enable_logging(TRUE);
	    if (flags & PROXY_CLNT_LOGGED)
		gtkhack_enable_logging(TRUE);
	    gtk_widget_realize(progress);
#if GTK_CHECK_VERSION(2,2,0)
	    cursor = gdk_cursor_new_for_display(
	      gdk_drawable_get_display(progress->window), GDK_WATCH);
#else
	    cursor = gdk_cursor_new(GDK_WATCH);
#endif
	    gdk_window_set_cursor(progress->window, cursor);
	    gdk_cursor_unref(cursor);
	    gtk_widget_show_now(progress);
	    while(g_main_context_iteration(NULL, FALSE))
		;
	    if (!strcmp(scheme, "file")) {
		err = NULL;
		os_path = g_filename_from_utf8(address, -1, NULL, NULL, &err);
		if (os_path) {
		    s = g_find_program_in_path(os_path);
		    retval = proxy_connect(scheme, s ? s : os_path,
		      &argc, argv);
		    g_free(s);
		    g_free(os_path);
		} else {
		    w = gtk_message_dialog_new(GTK_WINDOW(progress),
		      GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
		      GTK_BUTTONS_CLOSE,
		      "Unable to convert filename to OS-specific path.\n\n"
		      "%s", err->message);
		    gtk_dialog_run(GTK_DIALOG(w));
		    gtk_widget_destroy(w);
		    retval = 1;
		}
	    } else
		retval = proxy_connect(scheme, address, &argc, argv);
	    gtk_widget_destroy(progress);
	    g_free(name);
	    g_free(scheme);
	    g_free(address);
	    if (!retval) {
		g_object_unref(treeview);
		treeview = NULL;
		gtk_widget_destroy(connections);
		nh_write_gtkhackrc();
		proxy_start_client_services();
		exit(0);
	    }
	}
    }
    g_object_unref(treeview);
    treeview = NULL;
    gtk_widget_destroy(connections);
    exit(0);
}

/*
 * Some utility functions that are normally provided by NetHack.
 */

char *
eos(char *s)
{
    while(*s)
	s++;
    return s;
}

int
getyear()
{
    time_t t;
    struct tm *tm;
    (void)time(&t);
    tm = localtime(&t);
    return tm->tm_year + 1900;
}

long *
alloc(bytes)
unsigned int bytes;
{
    void *p;
    p = malloc(bytes);
    if (!p)
	panic("GtkHack: Memory allocation failure; cannot get %u bytes", bytes);
    return (long *)p;
}

char
highc(c)		/* force 'c' into uppercase */
char c;
{
    return((char)(('a' <= c && c <= 'z') ? (c & ~040) : c));
}

char
lowc(c)			/* force 'c' into lowercase */
char c;
{
    return((char)(('A' <= c && c <= 'Z') ? (c | 040) : c));
}

int
nh_strncmpi(s1, s2, n)	/* case insensitive counted string comparison */
const char *s1, *s2;
int n;
{
    register char t1, t2;

    while (n--) {
	if (!*s2) return (*s1 != 0);    /* s1 >= s2 */
	else if (!*s1) return -1;       /* s1  < s2 */
	t1 = lowc(*s1++);
	t2 = lowc(*s2++);
	if (t1 != t2) return (t1 > t2) ? 1 : -1;
    }
    return 0;                           /* s1 == s2 */
}

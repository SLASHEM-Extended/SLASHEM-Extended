/* $Id: gtkhack.c,v 1.2 2003-05-27 09:48:45 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002-2003 */
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
GtkListStore *GTK_connections;

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
	  COLUMN_SCHEME, datum->scheme, COLUMN_ADDRESS, datum->address, -1);
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
GTK_connection_add(const char *name, const char *scheme, const char *address)
{
    GtkTreeRowReference *ref;
    GtkTreePath *path;
    GtkTreeSelection *sel;
    GtkTreeIter iter;
    struct update_datum datum;
    datum.name = name;
    datum.scheme = scheme;
    datum.address = address;
    datum.found = FALSE;
    gtk_tree_model_foreach(GTK_TREE_MODEL(GTK_connections),
      (GtkTreeModelForeachFunc)update, &datum);
    if (!datum.found) {
	gtk_list_store_append(GTK_connections, &iter);
	gtk_list_store_set(GTK_connections, &iter, COLUMN_NAME, name,
	  COLUMN_SCHEME, scheme, COLUMN_ADDRESS, address, -1);
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
    GTK_proxy_clnt_errhandler(error);
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
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif
    g_type_init();
    GTK_connections = gtk_list_store_new(N_COLUMNS,
      G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GTK_init_gtk(&argc, argv);
    connections=create_Connections();
    if (!gtk_tree_model_iter_n_children(GTK_TREE_MODEL(GTK_connections), NULL))
	GTK_connection_add("local", "file", "slashem");
    treeview = lookup_widget(connections, "ConnectionsTreeView");
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
	    gtk_tree_model_get(model, &iter, COLUMN_NAME, &name,
	      COLUMN_SCHEME, &scheme, COLUMN_ADDRESS, &address, -1);
	    GTK_connection_set_weak_default(name);
	    gtk_widget_hide(connections);
	    progress = gtk_message_dialog_new(NULL,
	      0, GTK_MESSAGE_INFO, GTK_BUTTONS_CANCEL,
	      "Opening %s connection.\n\nEstablishing connection...", name);
	    gtk_window_set_title(GTK_WINDOW(progress),
	      "GtkHack: Connecting...");
	    proxy_svc_set_ext_procs(win_GTK_init, &GTK_ext_procs);
	    proxy_clnt_set_errhandler(GTK_proxy_clnt_errhandler);
	    nhext_set_errhandler(GTK_nhext_errhandler);
	    gtk_widget_realize(progress);
	    cursor = gdk_cursor_new_for_display(
	      gdk_drawable_get_display(progress->window), GDK_WATCH);
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
		gtk_widget_destroy(connections);
		proxy_start_client_services();
		exit(0);
	    }
	}
    }
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

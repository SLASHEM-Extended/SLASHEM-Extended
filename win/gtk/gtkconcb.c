/* $Id: gtkconcb.c,v 1.3 2003-12-13 12:52:58 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2003 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <errno.h>
#include <gtk/gtk.h>
#include "gtkconcb.h"
#include "gtkconnect.h"
#include "gtksupport.h"
#include "winGTK.h"
#include "prxyclnt.h"

void
GTK_add_connection(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_AddConnection();
    gtk_widget_show(w);
}

void
GTK_edit_connection(GtkTreeView *treeview, gpointer user_data)
{
    GtkTreeSelection *sel = gtk_tree_view_get_selection(treeview);
    GtkWidget *w = NULL;
    GtkEntry *entry;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *name, *scheme, *address, *port;
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
	gtk_tree_model_get(model, &iter, COLUMN_NAME, &name,
	  COLUMN_SCHEME, &scheme, COLUMN_ADDRESS, &address, -1);
	if (!strcmp(scheme, "file")) {
	    w = create_LocalExecutable();
	    entry = GTK_ENTRY(lookup_widget(w, "LocalExecutablePath"));
	    gtk_entry_set_text(GTK_ENTRY(entry), address);
	} else if (!strcmp(scheme, "tcp")) {
	    w = create_RemoteMachine();
	    port = strrchr(address, ':');
	    if (port)
		*port++ = '\0';
	    else
		port = "2700";		/* Should never happen */
	    entry = GTK_ENTRY(lookup_widget(w, "RemoteMachineHostname"));
	    gtk_entry_set_text(GTK_ENTRY(entry), address);
	    entry = GTK_ENTRY(lookup_widget(w, "RemoteMachinePort"));
	    gtk_entry_set_text(GTK_ENTRY(entry), port);
	} else
	    g_critical("Unknown server scheme '%s'", scheme);
	if (w) {
	    g_object_set_data_full(G_OBJECT(w), "connection-name", name,
	      g_free);
	    gtk_widget_show(w);
	} else
	    g_free(name);
	g_free(scheme);
	g_free(address);
    }
}

void
GTK_delete_connection(GtkTreeView *treeview, gpointer user_data)
{
    GtkTreeSelection *sel = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(sel, &model, &iter))
	gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
}

void
GTK_set_default_connection(GtkTreeView *treeview, gpointer user_data)
{
    GtkTreeSelection *sel = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *name;
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
	gtk_tree_model_get(model, &iter, COLUMN_NAME, &name, -1);
	GTK_connection_set_default(name);
	g_free(name);
    }
}

void
GTK_revert_connections(GtkTreeView *treeview, gpointer user_data)
{
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    gtk_list_store_clear(GTK_LIST_STORE(model));
    nh_read_gtkhackrc();
    if (!gtk_tree_model_iter_n_children(model, NULL))
	GTK_connection_add("local", "file", "slashem", 0UL);
}

void
GTK_create_connection(GtkWidget *dialog, gpointer user_data)
{
    GtkWidget *w;
    GtkWidget *ConnectionName = lookup_widget(dialog, "ConnectionName");
    GtkWidget *ServerType = lookup_widget(dialog, "ServerType");
    GtkWidget *DisableAsync = lookup_widget(dialog, "DisableAsync");
    GtkWidget *EnableLogging = lookup_widget(dialog, "EnableLogging");
    GtkTreeRowReference *ref;
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(ConnectionName));
    const gchar *type = gtk_entry_get_text(GTK_ENTRY(ServerType));
    unsigned long flags = 0UL;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(DisableAsync)))
	flags |= PROXY_CLNT_SYNCHRONOUS;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(EnableLogging)))
	flags |= PROXY_CLNT_LOGGED;
    if (!*name) {
	w = gtk_message_dialog_new(GTK_WINDOW(dialog),
	  GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
	  "Please enter a name for your new connection.");
	gtk_dialog_run(GTK_DIALOG(w));
	gtk_widget_destroy(w);
	return;
    } else if ((ref = GTK_connection_lookup(name))) {
	gtk_tree_row_reference_free(ref);
	w = gtk_message_dialog_new(GTK_WINDOW(dialog),
	  GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
	  "You already have a connection with the name %s.\n\n"
	  "Please enter a different name for your connection.", name);
	gtk_dialog_run(GTK_DIALOG(w));
	gtk_widget_destroy(w);
	return;
    }
    if (!strcmp(type, "Local executable"))
	w = create_LocalExecutable();
    else if (!strcmp(type, "Remote machine"))
	w = create_RemoteMachine();
    else {
	g_critical("Unknown server type '%s'", type);
	return;
    }
    g_object_set_data_full(G_OBJECT(w), "connection-name", g_strdup(name),
      g_free);
    g_object_set_data(G_OBJECT(w), "proxy-clnt-flags", GUINT_TO_POINTER(flags));
    gtk_widget_destroy(dialog);
    gtk_widget_show(w);
}

void
GTK_browse_server_executable(GtkWidget *entry, gpointer user_data)
{
    GtkWidget *w;
    const gchar *path = gtk_entry_get_text(GTK_ENTRY(entry));
    const gchar *os_path;
    gchar *s;
    w = gtk_file_selection_new("Select Local Executable");
    gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(w));
    if (*path) {
	os_path = g_filename_from_utf8(path, -1, NULL, NULL, NULL);
	if (os_path) {
	    s = g_find_program_in_path(os_path);
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(w),
	      s ? s : os_path);
	    g_free(s);
	    g_free((void *)os_path);
	}
    }
    if (gtk_dialog_run(GTK_DIALOG(w)) == GTK_RESPONSE_OK) {
	os_path = gtk_file_selection_get_filename(GTK_FILE_SELECTION(w));
	path = g_filename_to_utf8(os_path, -1, NULL, NULL, NULL);
	if (path) {
	    gtk_entry_set_text(GTK_ENTRY(entry), path);
	    g_free((void *)path);
	}
    }
    gtk_widget_destroy(w);
}

void
GTK_add_file_server(GtkWidget *entry, gpointer user_data)
{
    GtkWidget *LocalExecutable = lookup_widget(entry, "LocalExecutable");
    const gchar *name = g_object_get_data(G_OBJECT(LocalExecutable),
      "connection-name");
    unsigned long flags = GPOINTER_TO_UINT(
      g_object_get_data(G_OBJECT(LocalExecutable), "proxy-clnt-flags"));
    const gchar *path = gtk_entry_get_text(GTK_ENTRY(entry));
    GTK_connection_add(name, "file", path, flags);
    gtk_widget_destroy(LocalExecutable);
}

void
GTK_add_tcp_server(GtkWidget *RemoteExecutable, gpointer user_data)
{
    GtkWidget *w;
    gchar *address;
    GtkWidget *Hostname = lookup_widget(RemoteExecutable,
      "RemoteMachineHostname");
    GtkWidget *Port = lookup_widget(RemoteExecutable, "RemoteMachinePort");
    const gchar *name = g_object_get_data(G_OBJECT(RemoteExecutable),
      "connection-name");
    unsigned long flags = GPOINTER_TO_UINT(
      g_object_get_data(G_OBJECT(RemoteExecutable), "proxy-clnt-flags"));
    const gchar *host = gtk_entry_get_text(GTK_ENTRY(Hostname));
    const gchar *port = gtk_entry_get_text(GTK_ENTRY(Port));
    if (!*host) {
	w = gtk_message_dialog_new(GTK_WINDOW(RemoteExecutable),
	  GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
	  "Please enter the name of the host on which the server runs.");
	gtk_dialog_run(GTK_DIALOG(w));
	gtk_widget_destroy(w);
	return;
    }
    if (!*port) {
	w = gtk_message_dialog_new(GTK_WINDOW(RemoteExecutable),
	  GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
	  "Please enter the TCP/IP port that the server listens to.");
	gtk_dialog_run(GTK_DIALOG(w));
	gtk_widget_destroy(w);
	return;
    }
    /* We should check the port number more carefully, but this at least
     * means that we won't get confused.
     */
    if (!atoi(port) || strchr(port, ':')) {
	w = gtk_message_dialog_new(GTK_WINDOW(RemoteExecutable),
	  GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
	  "Illegal TCP/IP port number.\n\n"
	  "Please enter a number corresponding to the remote TCP/IP port.");
	gtk_dialog_run(GTK_DIALOG(w));
	gtk_widget_destroy(w);
	return;
    }
    address = g_strdup_printf("%s:%s", host, port);
    GTK_connection_add(name, "tcp", address, flags);
    g_free(address);
    gtk_widget_destroy(RemoteExecutable);
}

void
on_DisableAsync_toggled(GtkToggleButton *button, gpointer user_data)
{
    unsigned long flags;
    GtkTreePath *path;
    GtkTreeIter iter;
    path = gtk_tree_row_reference_get_path(GTK_current_connection);
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(GTK_connections), &iter, path)) {
	gtk_tree_model_get(GTK_TREE_MODEL(GTK_connections), &iter,
	  COLUMN_FLAGS, &flags, -1);
	if (gtk_toggle_button_get_active(button))
	    flags |= PROXY_CLNT_SYNCHRONOUS;
	else
	    flags &= ~PROXY_CLNT_SYNCHRONOUS;
	gtk_list_store_set(GTK_connections, &iter, COLUMN_FLAGS, flags, -1);
    }
    gtk_tree_path_free(path);
}

void
on_EnableLogging_toggled(GtkToggleButton *button, gpointer user_data)
{
    unsigned long flags;
    GtkTreePath *path;
    GtkTreeIter iter;
    path = gtk_tree_row_reference_get_path(GTK_current_connection);
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(GTK_connections), &iter, path)) {
	gtk_tree_model_get(GTK_TREE_MODEL(GTK_connections), &iter,
	  COLUMN_FLAGS, &flags, -1);
	if (gtk_toggle_button_get_active(button))
	    flags |= PROXY_CLNT_LOGGED;
	else
	    flags &= ~PROXY_CLNT_LOGGED;
	gtk_list_store_set(GTK_connections, &iter, COLUMN_FLAGS, flags, -1);
    }
    gtk_tree_path_free(path);
    gtkhack_enable_logging(flags & PROXY_CLNT_LOGGED);
}

void
on_ViewLog_clicked(GtkButton *button, gpointer user_data)
{
    static GtkWidget *ViewLog = NULL;
    GtkWidget *textview;
    if (ViewLog)
	gtk_window_present(GTK_WINDOW(ViewLog));
    else {
	ViewLog = create_ViewLog();
	textview = lookup_widget(ViewLog, "ViewLogTextView");
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview), GTK_nhext_log);
	gtk_window_set_default_size(GTK_WINDOW(ViewLog), 200, 300);
	gtk_widget_show(ViewLog);
	gtk_signal_connect(GTK_OBJECT(ViewLog), "destroy",
	  GTK_SIGNAL_FUNC(gtk_widget_destroyed), &ViewLog);
    }
}

void
on_viewlog_clear(GtkMenuItem *menuitem, gpointer user_data)
{
    gtk_text_buffer_set_text(GTK_nhext_log, "", 0);
}

void
on_viewlog_saveas_ok(GtkButton *button, GtkFileSelection *fs)
{
    gint resp;
    FILE *fp;
    const gchar *filename;
    gchar *text;
    GtkTextIter start, end;
    GtkWidget *dialog;
    filename = gtk_file_selection_get_filename(fs);
    if (g_file_test(filename, G_FILE_TEST_EXISTS)) {
	dialog = gtk_message_dialog_new(GTK_WINDOW(fs),
	   GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING,
	   GTK_BUTTONS_NONE, "File '%s' exists.", filename);
	gtk_dialog_add_buttons(GTK_DIALOG(dialog), "gtk-cancel",
	  GTK_RESPONSE_CANCEL, "_Overwrite", GTK_RESPONSE_OK, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
	resp = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	if (resp != GTK_RESPONSE_OK)
	    return;
    }
    fp = fopen(filename, "w");
    gtk_text_buffer_get_bounds(GTK_nhext_log, &start, &end);
    text = gtk_text_buffer_get_text(GTK_nhext_log, &start, &end, FALSE);
    if (fwrite(text, strlen(text), 1, fp) != 1) {
	dialog = gtk_message_dialog_new(GTK_WINDOW(fs),
	   GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
	   GTK_BUTTONS_CLOSE, "Error saving file '%s': %s",
	   filename, g_strerror(errno));
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
    }
    fclose(fp);
    free(text);
    gtk_widget_destroy(GTK_WIDGET(fs));
}

void
on_viewlog_save_as(GtkMenuItem *menuitem, gpointer user_data)
{
    static GtkWidget *SaveAs = NULL;
    if (SaveAs)
	gtk_window_present(GTK_WINDOW(SaveAs));
    else {
	GtkWidget *SaveAs = gtk_file_selection_new("Save connection log");
	g_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(SaveAs)->ok_button),
	  "clicked", G_CALLBACK(on_viewlog_saveas_ok), SaveAs);
	g_signal_connect_swapped(
	  GTK_OBJECT(GTK_FILE_SELECTION(SaveAs)->cancel_button), "clicked",
	  G_CALLBACK(gtk_widget_destroy), SaveAs); 
	gtk_widget_show(SaveAs);
	gtk_signal_connect(GTK_OBJECT(SaveAs), "destroy",
	  GTK_SIGNAL_FUNC(gtk_widget_destroyed), &SaveAs);
    }
}

void
on_viewlog_quit(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkWidget *w;
    w = lookup_widget(GTK_WIDGET(menuitem), "ViewLog");
    gtk_widget_destroy(w);
}

void
on_viewlog_copy(GtkMenuItem *menuitem, gpointer user_data)
{
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_copy_clipboard( GTK_nhext_log, clipboard);
}

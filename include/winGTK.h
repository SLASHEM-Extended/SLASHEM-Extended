/*
  $Id: winGTK.h,v 1.44 2003-08-02 16:02:44 j_ali Exp $
 */

#ifndef WINGTK_H
#define WINGTK_H

/*
 * GtkText is broken and replaced by the much better GtkTextView in GTK+ 1.3
 * As a temporary measure, we will continue to use it even though we
 * would do much better to change to GtkTextView.
 * FIXME: This define should go away.
 */

#define GTK_ENABLE_BROKEN	/* Enable support for old (broken) widgets */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#ifndef WIN32
#define WINGTK_X11		/* X11 libraries are available for our use */
#ifdef DEBUG
#define MONITOR_XRESOURCES	/* Debugging */
#endif
#endif

#define GTK_GRAPHICS	/* Needed when building an external interface */
#ifdef GTKHACK
#define GTK_PROXY
#endif

#ifdef WINGTK_X11
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#ifdef MONITOR_XRESOURCES
#include <X11/Xlibint.h>
#endif
#include <X11/extensions/XShm.h>
#endif

#include "hack.h"
#include "wintty.h"
#include "nhxdr.h"
#include "proxycom.h"

#define WINGTK_MENU_IMAGES	/* Pretty images (tiles) in first column */
#define WINGTK_RADAR		/* Radar map window */

#define NHW_NONE 0		/* Unallocated window type.  Must be	*/
				/* different from any other NHW_* type. */

#define	NH_PAD			5

#define NH_SESSION_RESIZABLE	1    /* Allow the user to resize window */
#define NH_SESSION_USER_POS	2    /* Window position from user */
#define NH_SESSION_USER_SIZE	4    /* Window size from user */
#define NH_SESSION_PLACED	8    /* Initial window placement has occured */

struct gtkhackrc;

extern GtkWidget *nh_gtk_window_dialog(boolean is_modal);

extern void nh_gtk_focus_set_master(GtkWindow *w, GtkSignalFunc func,
  gpointer data);
extern void nh_gtk_focus_set_slave_for(GtkWindow *w, GtkWindow *slave_for);

extern GtkWidget *nh_session_window_new(const char *name);
extern unsigned long nh_session_window_flags(const char *name);
extern int nh_session_set_geometry(const char *name, int x, int y,
  int width, int height);
extern int nh_session_save(struct gtkhackrc *rc);

extern int nh_read_gtkhackrc(void);
extern void nh_write_gtkhackrc(void);
extern void nh_gtkhackrc_store(struct gtkhackrc *rc, const char *fmt, ...);

extern void nh_game_preferences(GtkWidget *w, gpointer data);

extern GtkWidget *nh_gtk_new(GtkWidget *w, GtkWidget *parent, gchar *lbl);
extern GtkWidget *nh_gtk_new_and_add(GtkWidget *w, GtkWidget *parent,
  gchar *lbl);
extern GtkWidget *nh_gtk_new_and_pack(GtkWidget *w, GtkWidget *parent,
  gchar *lbl, gboolean a1, gboolean a2, guint a3);
extern GtkWidget *nh_gtk_new_and_attach(GtkWidget *w, GtkWidget *parent,
  gchar *lbl, guint a1, guint a2, guint a3, guint a4);
extern GtkWidget *nh_gtk_new_and_attach2(GtkWidget *w, GtkWidget *parent,
  gchar *lbl, guint a1, guint a2, guint a3, guint a4,
  GtkAttachOptions xoptions, GtkAttachOptions yoptions,
  guint xpadding, guint ypadding);

#define		N_NH_COLORS	(CLR_MAX + 3)
extern GdkColor  nh_color[N_NH_COLORS];

enum {
    MAP_DARK_GREEN = CLR_MAX,
    MAP_BLACK,
    MAP_WHITE
};

extern GtkWidget *main_window;

#define	NH_BUFSIZ		4096
#define NH_TEXT_REMEMBER	4096

enum xshm_map_mode {
    XSHM_MAP_NONE,
    XSHM_MAP_IMAGE,
    XSHM_MAP_PIXMAP,
    XSHM_MAP_PIXBUF
};

extern struct window_ext_procs GTK_ext_procs;

extern void win_GTK_init(void);
extern void GTK_init_nhwindows(char ***capvp);
extern void GTK_init_gtk(int *argc, char **argv);
extern int GTK_ext_init_nhwindows(int *, char **, char ***);
extern char *GTK_ext_askname(void);
extern int GTK_ext_player_selection(int *, int *, int *, int *);
extern void GTK_ext_display_file(int fh);
extern void GTK_ext_add_menu(winid, int, int, CHAR_P, CHAR_P, int,
  const char *, BOOLEAN_P);
extern int GTK_ext_select_menu(winid, int, struct proxy_mi **);
extern void GTK_ext_print_glyph(winid id, int x, int y, int glyph);
extern void GTK_ext_print_glyph_layered(winid id, int nl,
  struct proxy_glyph_layer *layers);
extern char GTK_ext_yn_function(const char *, const char *, CHAR_P, int *);
extern int GTK_ext_outrip(winid, char *);
extern char *GTK_ext_getlin(const char *query);
extern void GTK_get_nh_event(void);
extern void GTK_exit_nhwindows(const char *);
extern void GTK_suspend_nhwindows(void);
extern void GTK_resume_nhwindows(void);
extern winid GTK_create_nhwindow(int);
extern void GTK_ext_clear_nhwindow(winid, int, int, int);
extern void GTK_display_nhwindow(winid, BOOLEAN_P);
extern void GTK_destroy_nhwindow(winid);
extern void GTK_curs(winid, int, int);
extern void GTK_putstr(winid, int, const char *);
extern void GTK_start_menu(winid);
extern void GTK_end_menu(winid, const char *);
extern void GTK_update_inventory(void);
extern void GTK_mark_synch(void);
extern void GTK_wait_synch(void);
#ifdef CLIPPING
extern void GTK_cliparound(int, int);
#endif
extern int GTK_nhgetch(void);
extern int GTK_nh_poskey(int *, int *, int *);
extern void GTK_nhbell(void);
extern int GTK_doprev_message(void);
extern void GTK_extcmd_set(int cmd);
extern int GTK_get_ext_cmd(void);
extern void GTK_number_pad(int);
extern void GTK_delay_output(void);
extern void GTK_start_screen(void);
extern void GTK_end_screen(void);
extern void GTK_raw_print(const char *);
extern void GTK_raw_print_bold(const char *);
extern void GTK_ext_preference_update(const char *, const char *);
#ifdef GTKHACK
extern void GTK_ext_send_config_file(int);
#endif

extern GdkPixmap *GTK_glyph_to_gdkpixmap(int);

extern GtkWidget *nh_map_new(GtkWidget *);
extern void nh_map_destroy(void);
extern void nh_map_clear(int, int, int);
extern void nh_map_check_visibility(void);
extern void nh_map_pos(int *, int *, int *);
extern void nh_map_click(int);
extern void nh_map_flush(void);
extern int nh_set_map_visual(int);
extern int nh_set_map_font(gchar *name);
extern gchar *nh_get_map_font(void);
extern int nh_get_map_visual(void);
extern int nh_check_map_visual(int);

extern GtkWidget *GTK_preferences_new(void);

extern int main_hook(int *watch);
extern void quit_hook(void);
extern gint GTK_default_key_press(GtkWidget *widget, GdkEventKey *event,
  gpointer data);
extern gint GTK_default_focus_in(GtkWidget *widget, GdkEventFocus *event,
  gpointer data);

extern GtkWidget *nh_radar_new(void);
extern boolean nh_radar_get_use(void);
extern void nh_radar_set_use(boolean use);
extern void nh_radar_update(void);

extern GtkWidget *nh_message_new(void);
extern void nh_message_destroy(void);
extern void nh_message_putstr(const char *);

extern GtkWidget *nh_status_new(void);
extern boolean nh_status_in_trouble(void);
extern char *nh_status_last_displayed(char *quan);
extern void GTK_ext_status(int, int, const char **);

extern int nh_keysym(GdkEventKey *ev);
extern int nh_dir_keysym(GdkEventKey *ev);

extern void nh_option_new(void);
extern void nh_option_lock(boolean);
extern int nh_option_cache_sync(void);
extern void nh_option_cache_set(char *option, const char *value);
extern void nh_option_cache_set_bool(char *option, boolean value);
extern void nh_option_cache_set_addr(char *option, char **addr);
extern void nh_option_cache_set_bool_addr(char *option, boolean *addr);
extern char *nh_option_cache_get(char *option);
extern boolean nh_option_cache_get_bool(char *option);

extern int nh_tile_height();
extern int nh_tile_3dheight();

extern void nh_position_popup_dialog(GtkWidget *w);
/*
 * topten.c
 */
extern int create_toptenwin();

struct cached_options {
    boolean num_pad;
    boolean use_color;
    boolean hilite_pet;
    boolean perm_invent;
} copts;

struct menu {
    GtkCList *clist;
    const char *prompt;
    struct _NHMenuItem *nhMenuItem;
    int alloc_menuitem;
    int n_menuitem;
    int c_menuitem;
};

struct menu_info_t {
    struct menu	curr_menu;		/* Menu being displayed. */
    struct menu	new_menu;		/* New menu being built. */
    /*
     * cancelled may take the following values:
     *	0	Menu window is blocking on user input
     *	1	Menu window is being torn down
     *	-1	Menu window is non-blocking
     */
    int cancelled;
    int n_select;
    int selmode;
    int keysym;
    int valid_widgets;
    long count;				/* Pending count */
#ifdef WINGTK_MENU_IMAGES
    int	 pixmaps;			/* Set if any pixmaps set in clist */
#endif
};

#ifdef md5_INCLUDED
struct text_info_t {
    md5_state_t md5_state;
    GtkTextBuffer *buffer;
};
#endif

#define NHWF_DISPLAYED		1

typedef struct _NHWindow{
    int type;
    unsigned flags;

    guint hid;
    GtkWidget *w;
    GtkWidget *hbox, *hbox2, *hbox3;
    GtkWidget *vbox;
    GtkWidget *clist;
    GtkWidget *scrolled, *scrolled2;

    GtkWidget *frame;

    GtkAdjustment *adj, *adj2;

    int	n_button;
    GtkWidget *button[20];

    union {
	struct menu_info_t *Menu_info;	/* menu window info */
	struct text_info_t *Text_info;	/* text window info */
    } Win_info;

} NHWindow;

/* Defines to use for the window information union. */
#define menu_information   Win_info.Menu_info
#define text_information   Win_info.Text_info

#ifdef md5_INCLUDED
/* This linked list describes non-blocking text windows which may have no
 * corresponding NetHack window (ie., the game has called destroy_nhwindow).
 */
extern struct nbtw {
    GtkWidget *w;
    md5_byte_t digest[16];		/* MD5 checksum of window contents */
    struct nbtw *next;
} *non_blocking_text_windows;
#endif

typedef struct _TileTab{
    char *ident;
    char *file;
#ifdef GTK_PROXY
    char *mapfile;
#endif
    int tilemap_width, tilemap_height;
    int unit_width, unit_height;

    int ofsetx_3d;
    int ofsety_3d;

    int transparent:1;
    int spread:1;

    void *data;
} TileTab;
extern TileTab tileTab[];
extern int no_tileTab;
extern int stone_tile;

extern GtkWidget *xshm_map_init(enum xshm_map_mode, int width, int height);
extern void xshm_map_button_handler(GtkSignalFunc func, gpointer data);
extern void xshm_map_flush();
extern void xshm_map_cliparound(int x, int y);
extern void xshm_map_destroy();
extern void xshm_map_clear();
extern void xshm_map_tile_draw(int dst_x, int dst_y);
extern void xshm_map_draw(GdkRectangle *rect);
extern void xshm_map_draw_image(GdkImage *src, int src_x, int src_y,
  int dest_x, int dest_y, int width, int height);
extern void xshm_map_draw_drawable(GdkDrawable *src, int src_x, int src_y,
  int dest_x, int dest_y, int width, int height);
extern GtkAdjustment *xshm_map_get_hadjustment(void);
extern GtkAdjustment *xshm_map_get_vadjustment(void);
extern enum xshm_map_mode x_tile_init(TileTab *t);
extern void x_tile_tmp_clear();
extern int x_tile_render_to_drawable(GdkDrawable *drawable, GdkGC *gc,
  int tile, int src_x, int src_y, int dest_x, int dest_y,
  int width, int height);
extern void x_tile_draw_rectangle(int dstx, int dsty, GdkColor *c);
extern void x_tile_draw_tile(int tile, int dstx, int dsty);
extern void x_tile_draw_tmp(int dstx, int dsty);
extern void x_tile_tmp_draw_tile(int tile, int ofsx, int ofsy);
extern void x_tile_tmp_draw_rectangle(int ofsx, int ofsy, int c);
extern void x_tile_destroy();

/*
 * These must agree with the table in gtkextcmd.c -ALI
 */

#define EXT_CMD_ADJUST		0
#define EXT_CMD_BORROW		1
#define EXT_CMD_CHAT		2
#define EXT_CMD_CONDUCT		3
#define EXT_CMD_DIP		4
#define EXT_CMD_ENHANCE		5
#define EXT_CMD_EXPLORE		6
#define EXT_CMD_FORCE		7
#define EXT_CMD_INVOKE		8
#define EXT_CMD_JUMP		9
#define EXT_CMD_LOOT		10
#define EXT_CMD_MONSTER		11
#define EXT_CMD_NAME		12
#define EXT_CMD_OFFER		13
#define EXT_CMD_PRAY		14
#define EXT_CMD_QUIT		15
#define EXT_CMD_RIDE		16
#define EXT_CMD_RUB		17
#define EXT_CMD_SIT		18
#define EXT_CMD_TECHNIQUE	19
#define EXT_CMD_TURN		20
#define EXT_CMD_TWOWEAPON	21
#define EXT_CMD_UNTRAP		22
#define EXT_CMD_VERSION		23
#define EXT_CMD_WIPE		24
#define EXT_CMD_YOUPOLY		25

#define NO_EXT_CMD_MAPS		26

/* Current Map Mode */
extern int map_visual;

/* Current Map Clipping Squared distance */
extern int map_clip_dist2;

/* Current map cursor position */
extern int cursx, cursy;

/* Xshm map rasters. Warning: only one of these will be valid */
extern GdkImage *xshm_map_image;
extern GdkPixmap *xshm_map_pixmap;
extern GdkPixbuf *xshm_map_pixbuf;

#if defined(MONITOR_HEAP) && defined(INTERNAL_MALLOC)
#define XCreatePixmap(dpy, root, width, height, depth) \
	(monitor_heap_push(__FILE__, __LINE__), \
	monitor_heap_pop(__FILE__, __LINE__, \
	  (XCreatePixmap)(dpy, root, width, height, depth)))
#define gdk_pixmap_new(w, width, height, depth) \
	(GdkPixmap *)(monitor_heap_push(__FILE__, __LINE__), \
	monitor_heap_pop(__FILE__, __LINE__, \
	  (unsigned long)(gdk_pixmap_new)(w, width, height, depth)))
#define gdk_image_new(type, visual, width, height) \
	(GdkImage *)(monitor_heap_push(__FILE__, __LINE__), \
	monitor_heap_pop(__FILE__, __LINE__, \
	  (unsigned long)(gdk_image_new)(type, visual, width, height)))
#define gtk_window_new(type) \
	(GtkWidget *)(monitor_heap_push(__FILE__, __LINE__), \
	monitor_heap_pop(__FILE__, __LINE__, \
	  (unsigned long)(gtk_window_new)(type)))
#define gtk_hbox_new(homogeneous, spacing) \
	(GtkWidget *)(monitor_heap_push(__FILE__, __LINE__), \
	monitor_heap_pop(__FILE__, __LINE__, \
	  (unsigned long)(gtk_hbox_new)(homogeneous, spacing)))
#define gtk_vbox_new(homogeneous, spacing) \
	(GtkWidget *)(monitor_heap_push(__FILE__, __LINE__), \
	monitor_heap_pop(__FILE__, __LINE__, \
	  (unsigned long)(gtk_vbox_new)(homogeneous, spacing)))
#define gtk_clist_new(columns) \
	(GtkWidget *)(monitor_heap_push(__FILE__, __LINE__), \
	monitor_heap_pop(__FILE__, __LINE__, \
	  (unsigned long)(gtk_clist_new)(columns)))
#endif

#ifndef GTKHACK
#define nh_strncmpi	strncmpi
#endif

#ifdef GTKHACK
enum {
    COLUMN_NAME,
    COLUMN_SCHEME,
    COLUMN_ADDRESS,
    N_COLUMNS
};

extern GtkTreeRowReference *GTK_default_connection;
extern GtkListStore *GTK_connections;

extern GtkTreeRowReference *GTK_connection_lookup(const char *name);
extern void GTK_connection_set_default(const char *name);
extern void GTK_connection_add(const char *name, const char *scheme,
  const char *address);
#endif

#endif	/* WINGTK_H */

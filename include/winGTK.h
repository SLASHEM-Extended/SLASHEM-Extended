/*
  $Id: winGTK.h,v 1.15 2001-04-12 14:44:31 j_ali Exp $
 */

#ifndef WINGTK_H
#define WINGTK_H

#ifdef DEVEL_BRANCH
/*
 * GtkText is broken and replaced by the much better GtkTextView in GTK+ 1.3
 * As a temporary measure, we will continue to use it even though we
 * would do much better to change to GtkTextView.
 * FIXME: This define should go away.
 */
#endif

#define GTK_ENABLE_BROKEN	/* Enable support for old (broken) widgets */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#ifdef WIN32
# ifndef GTK_ASKNAME
#  define GTK_ASKNAME
# endif
#else
#define WINGTK_X11	/* X11 libraries are available for our use */
#ifdef DEBUG
#define MONITOR_XRESOURCES	/* Debugging */
#endif
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

#define WINGTK_MENU_IMAGES	/* Pretty images (tiles) in first column */

#define NHW_NONE 0		/* Unallocated window type.  Must be	*/
				/* different from any other NHW_* type. */

#define	NH_PAD			5

extern GtkWidget *nh_gtk_window_dialog(void);

extern GtkWidget *nh_gtk_new(GtkWidget *w, GtkWidget *parent, gchar *lbl);

extern GtkWidget *nh_gtk_new_and_add(GtkWidget *w, GtkWidget *parent, gchar *lbl);

extern GtkWidget *nh_gtk_new_and_pack(GtkWidget *w, GtkWidget *parent, gchar *lbl, 
				      gboolean a1, gboolean a2, guint a3);

extern GtkWidget *nh_gtk_new_and_attach(GtkWidget *w, GtkWidget *parent, gchar *lbl,
					guint a1, guint a2, guint a3, guint a4);

extern GtkWidget *nh_gtk_new_and_attach2(GtkWidget *w, GtkWidget *parent, gchar *lbl,
					 guint a1, guint a2, guint a3, guint a4,
					 GtkAttachOptions xoptions,
					 GtkAttachOptions yoptions,
					 guint xpadding,
					 guint ypadding);


#define		N_NH_COLORS	20
extern GdkColor  nh_color[N_NH_COLORS];

enum {
    MAP_BLACK,
    MAP_RED,
    MAP_GREEN,
    MAP_BROWN,
    MAP_BLUE,
    MAP_MAGENTA,
    MAP_CYAN,
    MAP_GRAY,
    MAP_FOREGROUND,
    MAP_ORANGE,
    MAP_BRIGHT_GREEN,
    MAP_YELLOW,
    MAP_BRIGHT_BLUE,
    MAP_BRIGHT_MAGENTA,
    MAP_BRIGHT_CYAN,
    MAP_WHITE,
    MAP_DARK_GREEN,
    MAP_BACKGROUND
};


#define	NH_BUFSIZ		4096
#define NH_TEXT_REMEMBER	4096

#if defined(DEVEL_BRANCH) && defined(GTK_V20)
enum xshm_map_mode {
    XSHM_MAP_NONE,
    XSHM_MAP_IMAGE,
    XSHM_MAP_PIXMAP,
    XSHM_MAP_PIXBUF
};
#endif

extern void	GTK_init_nhwindows(int *, char **);
extern void	GTK_player_selection(void);
extern void	GTK_askname(void);
extern void	GTK_get_nh_event(void);
extern void	GTK_exit_nhwindows(const char *);
extern void	GTK_suspend_nhwindows(void);
extern void	GTK_resume_nhwindows(void);
extern winid	GTK_create_nhwindow(int);
extern void	GTK_clear_nhwindow(winid);
extern void	GTK_display_nhwindow(winid, BOOLEAN_P);
extern void	GTK_destroy_nhwindow(winid);
extern void	GTK_curs(winid, int, int);
extern void	GTK_putstr(winid, int, const char *);
#ifdef FILE_AREAS
extern void	GTK_display_file(const char *, const char *, BOOLEAN_P);
#else
extern void	GTK_display_file(const char *, BOOLEAN_P);
#endif
extern void	GTK_start_menu(winid);
extern void	GTK_add_menu(winid, int, const ANY_P *, CHAR_P,CHAR_P,int,const char *, BOOLEAN_P);
extern void	GTK_end_menu(winid, const char *);
extern int	GTK_select_menu(winid, int, MENU_ITEM_P **);
extern void	GTK_update_inventory(void);
extern void	GTK_mark_synch(void);
extern void	GTK_wait_synch(void);
#ifdef CLIPPING
extern void	GTK_cliparound(int, int);
#endif
extern void	GTK_prvoid_glyph(void);
extern void	GTK_raw_prvoid(void);
extern void	GTK_raw_prvoid_bold(void);
extern int	GTK_nhgetch(void);
extern int	GTK_nh_poskey(int *, int *, int *);
extern void	GTK_nhbell(void);
extern int	GTK_doprev_message(void);
extern char	GTK_yn_function(const char *, const char *, CHAR_P);
extern void	GTK_getlin(const char *, char *);
extern void	GTK_extcmd_set(int cmd);
extern int	GTK_get_ext_cmd(void);
extern void	GTK_number_pad(void);
extern void	GTK_delay_output(void);
extern void	GTK_start_screen(void);
extern void	GTK_end_screen(void);
#ifdef GRAPHIC_TOMBSTONE
extern void	GTK_outrip(winid, int);
#endif
extern void	GTK_print_glyph(winid, XCHAR_P, XCHAR_P, int);
extern void	GTK_raw_print(const char *);
extern void	GTK_raw_print_bold(const char *);

extern GdkPixmap * GTK_glyph_to_gdkpixmap(int);

extern GtkWidget	*nh_map_new(GtkWidget *);
extern void		nh_map_destroy(void);
extern void		nh_map_clear(void);
extern void		nh_map_check_visibility(void);
extern void		nh_map_pos(int *, int *, int *);
extern void		nh_map_click(int);
extern void		nh_map_flush(void);

#if defined(DEVEL_BRANCH) && defined(GTK_V20)
extern int		nh_set_map_visual(int);
#else
extern void		nh_set_map_visual(int);
#endif
extern int		nh_get_map_visual(void);
extern int		nh_check_map_visual(int);

extern void		main_hook(void);
extern void		quit_hook(void);
extern gint		GTK_default_key_press(GtkWidget *widget,
			  GdkEventKey *event, gpointer data);

extern GtkWidget	*nh_radar_new(void);
extern void		nh_radar_update(void);

extern GtkWidget	*nh_message_new(void);
extern void		nh_message_putstr(const char *);

extern GtkWidget	*nh_status_new(void);
extern void		nh_status_update(void);
extern void		nh_status_index_update(void);

extern int		nh_keysym(GdkEventKey *ev);
extern int		nh_dir_keysym(GdkEventKey *ev);

extern void		nh_option_new(void);
extern void		nh_option_lock(void);

extern int		nh_tile_height();
extern int		nh_tile_3dheight();

extern void		nh_position_popup_dialog(GtkWidget *w);
/*
  topten.c
 */
extern int		create_toptenwin();


struct menu {
    GtkCList	*clist;
    const char	*prompt;
    struct _NHMenuItem *nhMenuItem;
    int		alloc_menuitem;
    int		n_menuitem;
    int		c_menuitem;
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
    int		cancelled;
    int		n_select;
    int		selmode;
    int 	keysym;
    int		valid_widgets;
#ifdef WINGTK_MENU_IMAGES
    int		pixmaps;		/* Set if any pixmaps set in clist */
#endif
};

typedef struct _NHWindow{
    int		type;

    guint	hid;
    GtkWidget	*w;
    GtkWidget	*hbox, *hbox2, *hbox3;
    GtkWidget	*vbox, *vbox2;
    GtkWidget	*clist;
    GtkWidget	*scrolled;

    GtkWidget	*frame;
    GtkWidget	*query;

    GtkAdjustment *adj;

    int	n_subclist;
    GtkWidget	*subclist[20];

    int	n_subframe;
    GtkWidget	*subframe[20];

    int	n_button;
    GtkWidget	*button[20];

    union {
	struct menu_info_t	*Menu_info;	/* menu window info */
    } Win_info;

}NHWindow;

/* Defines to use for the window information union. */
#define menu_information   Win_info.Menu_info

typedef struct _TileTab{
    char *ident;
    char *file;
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

#if defined(DEVEL_BRANCH) && defined(GTK_V20)
extern GtkWidget *xshm_map_init(enum xshm_map_mode, int width, int height);
extern void xshm_map_button_handler(GtkSignalFunc func, gpointer data);
extern void	xshm_map_flush();
extern void	xshm_map_cliparound(int x, int y);
#else
#ifdef WINGTK_X11
extern void	xshm_init(Display *dpy);
#else
extern void	xshm_init(GdkWindow *w);
#endif
extern int	xshm_map_init(int width, int height);
#endif
extern void	xshm_map_destroy();
extern void	xshm_map_clear();
extern void	xshm_map_tile_draw(int dst_x, int dst_y);
#if defined(DEVEL_BRANCH) && defined(GTK_V20)
extern void	xshm_map_draw(GdkRectangle *rect);
extern void	xshm_map_draw_image(GdkImage *src, int src_x, int src_y, int dest_x, int dest_y, int width, int height);
extern void	xshm_map_draw_drawable(GdkDrawable *src, int src_x, int src_y, int dest_x, int dest_y, int width, int height);
extern GtkAdjustment *xshm_map_get_hadjustment(void);
extern GtkAdjustment *xshm_map_get_vadjustment(void);
extern enum xshm_map_mode x_tile_init(TileTab *t);
extern void	x_tile_tmp_clear();
extern int	x_tile_render_to_drawable(GdkDrawable *drawable, GdkGC *gc,
int tile, int src_x, int src_y, int dest_x, int dest_y, int width, int height);
extern void	x_tile_draw_rectangle(int dstx, int dsty, GdkColor *c);
extern void	x_tile_draw_tile(int tile, int dstx, int dsty);
extern void	x_tile_draw_tmp(int dstx, int dsty);
extern void	x_tile_tmp_draw_tile(int tile, int ofsx, int ofsy);
#else
extern void	xshm_map_draw(GdkWindow *, int src_x, int src_y, int dst_x, int dst_y, int width, int height);
extern void	x_tile_init(GdkImage *img, TileTab *t);
extern void	x_tmp_clear();
extern void	x_tile_gdkimage_draw(GdkImage *dst, int transparency, int srcx, int srcy, int ofsx, int ofsy);
extern void	x_tile_tmp_draw(int src_x, int src_y, int ofsx, int ofsy);
#endif
extern void	x_tile_tmp_draw_rectangle(int ofsx, int ofsy, int c);
extern void	x_tile_destroy();

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
extern int	map_visual;

/* Current map cursor position */
extern int	cursx, cursy;

#if defined(DEVEL_BRANCH) && defined(GTK_V20)
/* Xshm map rasters. Warning: only one of these will be valid */
extern GdkImage *xshm_map_image;
extern GdkPixmap *xshm_map_pixmap;
extern GdkPixbuf *xshm_map_pixbuf;
#endif

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

#endif

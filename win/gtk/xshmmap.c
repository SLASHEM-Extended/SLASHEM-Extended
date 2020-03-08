/*
  $Id: xshmmap.c,v 1.10 2003-12-13 16:35:16 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
               Copyright (c) Slash'EM Development Team 2000-2003
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include "winGTK.h"

/*
 * [ALI] This module deals with displaying the map on the screen.
 * 
 * API:
 *
 * xshm_map_init(mode, width, height)
 *	Initialize the map. This function should also be called if the
 *	size needs to be changed (ie., when changing tilesets). It is
 *	better not to call xshm_map_destroy() first. Mode should be
 *	set to XSHM_MAP_IMAGE if the data to be sent to the map requires
 *	uploading raster data to the server and XSHM_MAP_PIXMAP if the
 *	data will either already be present on the server or if it can
 *	be created with back-end rendering calls. In the first case
 *	xshm_map_image will be initialised and xshm_map_pixmap will be
 *	NULL, in the second case the reverse applies.
 *	Note: XSHM_MAP_PIXBUF is supported (but unused by default).
 *	It appears to have no advantages over XSHM_MAP_IMAGE.
 * xshm_map_destroy()
 *	Free all resources allocated by xshm_map_init().
 * xshm_map_get_hadjustment() and xshm_map_get_vadjustment()
 *	Get the current scrolling adjustment controls.
 * xshm_map_cliparound(x, y)
 *	Arrange for xshm_map_flush() to scroll the map so that it is centered
 *	on pixel (x, y).
 * xshm_map_set_button_handler(func, data)
 *	Register a function to be called when a button press event is
 *	received on the map.
 *
 * Drawing:
 *
 * xshm_map_draw(area)
 *	Mark the given area in map as needing to be propagated to the screen.
 * xshm_map_flush()
 *	Action the draw queue and cliparound requested.
 *
 * It is perfectly permissible to draw directly to xshm_map_pixbuf,
 * xshm_map_image or xshm_map_pixmap as appropriate and then call
 * xshm_map_draw() to request the areas to be updated. The following
 * functions are provided for convenience:
 *
 * xshm_map_clear()
 *	Clear the map to zero.
 * xshm_map_draw_image(src, src_x, src_y, dest_x, dest_y, width, height)
 *	Copy the src image to the map. The image must be of the same
 *	format as the map. This can be guaranteed by using gdk_image_new()
 *	and using the visual of a realised window on the same display.
 *	Supported modes: image and pixmap.
 * xshm_map_draw_drawable(src, src_x, src_y, dest_x, dest_y, width, height)
 *	Copy the src drawable to the map.
 *	Supported modes: image and pixmap.
 * xshm_map_draw_pixbuf(src, src_x, src_y, dest_x, dest_y, width, height)
 *	Copy the src pixbuf to the map.
 *	Supported modes: pixbuf only.
 */

GdkImage *xshm_map_image;
GdkPixmap *xshm_map_pixmap;
GdkPixbuf *xshm_map_pixbuf;
static struct xshm_private {
    GdkPixmap *pixmap;		/* Backing store if needed */
    GtkWidget *area;		/* The on-screen window */
    GdkRegion *dirty;		/* The draw list */
    GdkGC *gc;			/* gc used to draw to xshm_map_pixmap */
    enum xshm_map_mode mode;
    int map_width, map_height;
    GtkAdjustment *hadj, *vadj;
    GtkWidget *hscrollbar;
    GtkWidget *vscrollbar;
    GtkWidget *vbox, *hbox;
    int areax, areay;		/* The current offset of area within map */
    				/* Handler for button press events */
    void (*button_press)(void *, GdkEventButton *, gpointer);
    gpointer button_press_data;
    int is_pending;		/* True if flush() needed to update map */
} xshm;

#ifdef DEBUG
#include <sys/time.h>
#include <unistd.h>

static void
time_stamp(FILE *fp)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    fprintf(fp, "%d.%06d: ",tv.tv_sec,tv.tv_usec);
}
#endif

GtkAdjustment *
xshm_map_get_hadjustment(void)
{
    return xshm.hadj;
}

GtkAdjustment *
xshm_map_get_vadjustment(void)
{
    return xshm.vadj;
}

/*
 * This routine maintains the dirty region which is the set of rectangles
 * which have been modified in the map since the last xshm_map_flush().
 * If we are in non-shared image or pixbuf mode then this region needs
 * copying to the backing store. In all cases, this region needs copying
 * from the backing store to the screen.
 */

void
xshm_map_draw(GdkRectangle *rect)
{
    if (xshm.dirty)
	gdk_region_union_with_rect(xshm.dirty, rect);
    else
	xshm.dirty = gdk_region_rectangle(rect);
    xshm.is_pending = TRUE;
}

/*
 * This routine is responsible for arranging for the expose handler
 * to be called and for providing it with the correct area of the
 * screen to be exposed. This will be the whole of the visble map
 * if scrolling has taken place, or the visible part of the dirty
 * list otherwise.
 *
 * Note: The expose handler is responsible for clearing the dirty
 * list in due course.
 */

void
xshm_map_flush(void)
{
    int i;
    GdkRectangle clip, expose_area;
    g_return_if_fail(xshm.area != NULL);
    xshm.is_pending = FALSE;
    if (xshm.areax != (int)xshm.hadj->value ||
      xshm.areay != (int)xshm.vadj->value) {
	clip.x = clip.y = 0;
	clip.width = xshm.hadj->page_size;
	clip.height = xshm.vadj->page_size;
#ifdef DEBUG
	time_stamp(stderr);
	fprintf(stderr, "Map: flush at (%d, %d) size %d x %d\n",
	  (int)xshm.hadj->value, (int)xshm.vadj->value,
	  clip.width, clip.height);
#endif
	gtk_widget_draw(xshm.area, &clip);
    } else if (xshm.dirty) {
	gdk_region_get_clipbox(xshm.dirty, &expose_area);
#ifdef DEBUG
	time_stamp(stderr);
	fprintf(stderr, "Map: flush at (%d, %d) size %d x %d\n",
	  expose_area.x, expose_area.y, expose_area.width, expose_area.height);
#endif
	expose_area.x -= xshm.hadj->value;
	expose_area.y -= xshm.vadj->value;
	clip.x = clip.y = 0;
	clip.width = xshm.hadj->page_size;
	clip.height = xshm.vadj->page_size;
	if (gdk_rectangle_intersect(&expose_area, &clip, &expose_area))
	    gtk_widget_draw(xshm.area, &expose_area);
    }
}

/*
 * Adjust clipping so that pixel (x, y) on the map is approximately
 * in the centre of the drawing area.
 */

void
xshm_map_cliparound(int x, int y)
{
    int vx, vy;
#ifdef DEBUG
    time_stamp(stderr);
    fprintf(stderr, "Map: Cliparound (%d, %d)\n", x, y);
#endif
    vx = x - xshm.hadj->page_size / 2;
    vy = y - xshm.vadj->page_size / 2;
    if (vx < xshm.hadj->lower)
	vx = xshm.hadj->lower;
    else if (vx > xshm.hadj->upper - xshm.hadj->page_size)
	vx = xshm.hadj->upper - xshm.hadj->page_size;
    if (vy < xshm.vadj->lower)
	vy = xshm.vadj->lower;
    else if (vy > xshm.vadj->upper - xshm.vadj->page_size)
	vy = xshm.vadj->upper - xshm.vadj->page_size;
    /* Must be done before changing the adjustments so that scroll_event()
     * knows that a xshm_map_flush() is pending.
     */
    xshm.is_pending = TRUE;
    if (vx != xshm.hadj->value)
	gtk_adjustment_set_value(xshm.hadj, vx);
    if (vy != xshm.vadj->value)
	gtk_adjustment_set_value(xshm.vadj, vy);
}

/*
 * Re-configure the drawing area. If the drawing area is bigger than the
 * map (ie., the user has stretched it), the area beyond the map will
 * just be background.
 */

static int
configure_event(GtkWidget *widget, GdkEventConfigure *event)
{
#ifdef DEBUG
    fprintf(stderr, "Area: Configure (%d x %d)\n", event->width, event->height);
#endif
    xshm.hadj->page_size = (gfloat)event->width;
    xshm.hadj->page_increment = (gfloat)event->width;
    if (xshm.hadj->value < xshm.hadj->lower)
	gtk_adjustment_set_value(xshm.hadj, xshm.hadj->lower);
    else if (xshm.hadj->value > xshm.hadj->upper - xshm.hadj->page_size)
	gtk_adjustment_set_value(xshm.hadj,
	  xshm.hadj->upper - xshm.hadj->page_size);
    gtk_signal_emit_by_name(GTK_OBJECT(xshm.hadj), "changed");
    xshm.vadj->page_size = (gfloat)event->height;
    xshm.vadj->page_increment = (gfloat)event->height;
    if (xshm.vadj->value < xshm.vadj->lower)
	gtk_adjustment_set_value(xshm.vadj, xshm.vadj->lower);
    else if (xshm.vadj->value > xshm.vadj->upper - xshm.vadj->page_size)
	gtk_adjustment_set_value(xshm.vadj,
	  xshm.vadj->upper - xshm.vadj->page_size);
    gtk_signal_emit_by_name(GTK_OBJECT(xshm.vadj), "changed");
}

/*
 * This function serves three purposes:
 *	- It updates the backing store from the master copy.
 *	  (only needed in non-shared image and pibxuf modes)
 *	- It copies the dirty region of the backing store to the window.
 *	- It refreshes the exposed area of the window from backing store.
 *
 * By doing all these at the same time, we can reduce problems with jerky
 * motion caused by either updating the map and then scrolling it or by
 * scrolling it and then updating (neither of which look good).
 *
 * In order to maintain this, callers shouldn't flush the map until all
 * updating and scrolling has been completed.
 */

static gint
expose_event(GtkWidget *widget, GdkEventExpose *event)
{
    GdkGC *gc;
    int x = (int)xshm.hadj->value;
    int y = (int)xshm.vadj->value;
#ifdef DEBUG
    time_stamp(stderr);
    fprintf(stderr, "Map: expose_event at (%d, %d) size %d x %d ... ",
	event->area.x, event->area.y, event->area.width, event->area.height);
#endif
    gc = gdk_gc_new(widget->window);
    /* First update the backing store if that needs doing */
    if (xshm.pixmap && xshm.dirty && (xshm_map_image || xshm_map_pixbuf)) {
	/* TODO: Should restrict this to the visible portion */
	gdk_gc_set_clip_region(gc, xshm.dirty);
	if (xshm_map_image)
	    gdk_draw_image(xshm.pixmap,
	      gc, xshm_map_image, 0, 0, 0, 0, xshm.map_width, xshm.map_height);
	else /* if (xshm_map_pixbuf) */
	    gdk_pixbuf_render_to_drawable(xshm_map_pixbuf, xshm.pixmap, gc,
	      0, 0, 0, 0, xshm.map_width, xshm.map_height,
	      GDK_RGB_DITHER_NORMAL, 0, 0);
	gdk_gc_set_clip_region(gc, NULL);
#ifdef DEBUG
	fprintf(stderr,"%s", "backing pixmap updated ... ");
#endif
    }
    /* Now update the screen from the backing store */
    if (xshm.pixmap)
	gdk_draw_pixmap(widget->window, gc, xshm.pixmap,
	  x + event->area.x, y + event->area.y, event->area.x, event->area.y,
	  min(event->area.width, xshm.map_width),
	  min(event->area.height, xshm.map_height));
    else
	gdk_draw_image(widget->window, gc, xshm_map_image,
	  x + event->area.x, y + event->area.y, event->area.x, event->area.y,
	  min(event->area.width, xshm.map_width),
	  min(event->area.height, xshm.map_height));
    if (event->area.width > xshm.map_width ||
      event->area.height > xshm.map_height) {
	gdk_gc_set_foreground(gc, &widget->style->bg[GTK_STATE_NORMAL]);
	if (event->area.width > xshm.map_width)
	    gdk_draw_rectangle(widget->window, gc, TRUE, xshm.map_width, 0,
	      event->area.width - xshm.map_width, xshm.map_height);
	if (event->area.height > xshm.map_height)
	    gdk_draw_rectangle(widget->window, gc, TRUE, 0, xshm.map_height,
	      event->area.width, event->area.height - xshm.map_height);
    }
    xshm.areax = x;
    xshm.areay = y;
#ifdef DEBUG
    fprintf(stderr,"%s", "screen updated\n");
#endif
    /* And clean up */
    if (xshm.dirty) {
	gdk_region_destroy(xshm.dirty);
	xshm.dirty = NULL;
    }
    gdk_gc_unref(gc);
    return TRUE;
}

static void
scroll_event(GtkAdjustment *adj, gpointer data)
{
    GdkRectangle view;
    g_return_if_fail(adj == xshm.hadj || adj == xshm.vadj);
#ifdef DEBUG
    time_stamp(stderr);
#endif
    if (!xshm.is_pending) {
#ifdef DEBUG
	if (adj == xshm.hadj)
	    fprintf(stderr,"%s", "Horizontal scroll event\n");
	else
	    fprintf(stderr,"%s", "Vertical scroll event\n");
#endif
	view.x = view.y = 0;
	view.width = xshm.hadj->page_size;
	view.height = xshm.vadj->page_size;
	gtk_widget_draw(xshm.area, &view);
    }
#ifdef DEBUG
    else
	fprintf(stderr,"%s", "Clipping scroll event\n");
#endif
}

static int
xshm_map_size(int mode, int width, int height)
{
    GdkGC *gc;
    GdkVisual *visual;
    GdkRectangle rect;
    int i, j;
    xshm.hadj->upper = width - 1;
    if (xshm.hadj->value < xshm.hadj->lower)
	gtk_adjustment_set_value(xshm.hadj, xshm.hadj->lower);
    else if (xshm.hadj->value > xshm.hadj->upper - xshm.hadj->page_size)
	gtk_adjustment_set_value(xshm.hadj,
	  xshm.hadj->upper - xshm.hadj->page_size);
    gtk_signal_emit_by_name(GTK_OBJECT(xshm.hadj), "changed");
    xshm.vadj->upper = height - 1;
    if (xshm.vadj->value < xshm.vadj->lower)
	gtk_adjustment_set_value(xshm.vadj, xshm.vadj->lower);
    else if (xshm.vadj->value > xshm.vadj->upper - xshm.vadj->page_size)
	gtk_adjustment_set_value(xshm.vadj,
	  xshm.vadj->upper - xshm.vadj->page_size);
    gtk_signal_emit_by_name(GTK_OBJECT(xshm.vadj), "changed");
    if (width == xshm.map_width && height == xshm.map_height &&
      (mode == XSHM_MAP_IMAGE ? xshm_map_image != 0 : mode == XSHM_MAP_PIXMAP ?
      xshm_map_pixmap != 0 : xshm_map_pixbuf != 0))
	return 0;			/* Map configuration unchanged */
#ifdef DEBUG
    time_stamp(stderr);
    fprintf(stderr, "Map: Resize (%d x %d)\n", width, height);
#endif
    if (xshm.dirty) {
	gdk_region_destroy(xshm.dirty);
	xshm.dirty = NULL;
    }
    if (xshm.gc) {
	gdk_gc_unref(xshm.gc);
	xshm.gc = NULL;
    }
    if (xshm_map_pixmap) {
	gdk_pixmap_unref(xshm_map_pixmap);
	xshm_map_pixmap = NULL;
	xshm.pixmap = NULL;
    } else if (xshm_map_image) {
	gdk_image_destroy(xshm_map_image);
	xshm_map_image = NULL;
	if (xshm.pixmap) {
	    gdk_pixmap_unref(xshm.pixmap);
	    xshm.pixmap = NULL;
	}
    } else if (xshm_map_pixbuf) {
	gdk_pixbuf_unref(xshm_map_pixbuf);
	xshm_map_pixbuf = NULL;
	gdk_pixmap_unref(xshm.pixmap);
	xshm.pixmap = NULL;
    }
    if (mode == XSHM_MAP_PIXBUF) {
#ifdef DEBUG
	fprintf(stderr,"%s", "Map: Switching to pixbuf mode\n");
#endif
	/* We don't really want an alpha channel (it doesn't have
	 * any meaning) but GDK doesn't support alpha -> non-alpha.
	 */
	xshm_map_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE,
	  8, width, height);
	memset(gdk_pixbuf_get_pixels(xshm_map_pixbuf), 0,
	  height * gdk_pixbuf_get_rowstride(xshm_map_pixbuf));
	xshm.pixmap = gdk_pixmap_new(xshm.area->window, width, height, -1);
	/* Pixmap needs updating; mark it as such */
	rect.x = rect.y = 0;
	rect.width = max(width, xshm.map_width);
	rect.height = max(height, xshm.map_height);
	xshm.dirty = gdk_region_rectangle(&rect);
    } else if (mode == XSHM_MAP_IMAGE) {
#ifdef DEBUG
	fprintf(stderr,"%s", "Map: Switching to image mode\n");
#endif
	visual = gdk_drawable_get_visual(xshm.area->window);
	xshm_map_image = gdk_image_new(GDK_IMAGE_SHARED, visual, width, height);
	if (!xshm_map_image) {
	    xshm_map_image =
	      gdk_image_new(GDK_IMAGE_NORMAL, visual, width, height);
	    xshm.pixmap = gdk_pixmap_new(xshm.area->window, width, height, -1);
	} else
	    xshm.pixmap = NULL;
	memset(xshm_map_image->mem, 0, height * xshm_map_image->bpl);
	if (xshm.pixmap) {
	    /* Pixmap also needs updating; mark it as such */
	    rect.x = rect.y = 0;
	    rect.width = max(width, xshm.map_width);
	    rect.height = max(height, xshm.map_height);
	    xshm.dirty = gdk_region_rectangle(&rect);
	} else
	    xshm.dirty = NULL;
    } else {
#ifdef DEBUG
	fprintf(stderr,"%s", "Map: Switching to pixmap mode\n");
#endif
	xshm.pixmap = gdk_pixmap_new(xshm.area->window, width, height, -1);
	xshm.gc = gdk_gc_new(xshm.pixmap);
	gdk_gc_set_foreground(xshm.gc, &xshm.area->style->bg[GTK_STATE_NORMAL]);
	gdk_draw_rectangle(xshm.pixmap, xshm.gc, TRUE, 0, 0, width, height);
	xshm_map_pixmap = xshm.pixmap;
	/* Area outside new map needs updating; mark it as such */
	if (width < xshm.map_width)
	{
	    rect.x = width;
	    rect.y = 0;
	    rect.width = xshm.map_width - width;
	    rect.height = xshm.map_height;
	    xshm.dirty = gdk_region_rectangle(&rect);
	} else
	    xshm.dirty = NULL;
	if (height < xshm.map_height)
	{
	    rect.x = 0;
	    rect.y = height;
	    rect.width = xshm.map_width;
	    rect.height = xshm.map_height - height;
	    xshm.dirty = gdk_region_rectangle(&rect);
	    if (xshm.dirty)
		gdk_region_union_with_rect(xshm.dirty, &rect);
	    else
		xshm.dirty = gdk_region_rectangle(&rect);
	}
    }
    xshm.map_width = width;
    xshm.map_height = height;
#ifdef DEBUG
    fprintf(stderr,"%s", "New configuration:\n");
    if (xshm_map_image)
	fprintf(stderr, "\txshm_map_image: %d x %d\n",
	  xshm_map_image->width, xshm_map_image->height);
    else
	fprintf(stderr,"%s", "\txshm_map_image: none\n");
    if (xshm_map_pixmap) {
	gdk_drawable_get_size(xshm_map_pixmap, &i, &j);
	fprintf(stderr, "\txshm_map_pixmap: %d x %d\n", i, j);
    } else
	fprintf(stderr,"%s", "\txshm_map_pixmap: none\n");
    if (xshm_map_pixbuf)
	fprintf(stderr, "\txshm_map_pixbuf: %d x %d\n",
	  gdk_pixbuf_get_width(xshm_map_pixbuf),
	  gdk_pixbuf_get_height(xshm_map_pixbuf));
    else
	fprintf(stderr,"%s", "\txshm_map_pixbuf: none\n");
    if (xshm.pixmap) {
	gdk_drawable_get_size(xshm.pixmap, &i, &j);
	fprintf(stderr, "\tpixmap: %d x %d\n",i, j);
    } else
	fprintf(stderr,"%s", "\tpixmap: none\n");
    fprintf(stderr, "\tarea: %d x %d\n",
      (int)xshm.hadj->page_size, (int)xshm.vadj->page_size);
#endif
}

static void
realize_event(GtkWidget *w, gpointer data)
{
    xshm_map_size(xshm.mode, xshm.map_width, xshm.map_height);
}

GtkWidget *
xshm_map_init(enum xshm_map_mode mode, int width, int height)
{
#ifdef DEBUG
    time_stamp(stderr);
    fprintf(stderr, "Map: Init (%s, %d, %d)\n",
      mode == XSHM_MAP_PIXMAP ? "Pixmap" : mode == XSHM_MAP_PIXBUF ? "Pixbuf" :
      mode == XSHM_MAP_IMAGE ? "Image" : "None", width, height);
#endif
    if (!xshm.area) {
	/* TODO: Tweak scrollbar placement to be more like scrolled_window
	 * and remove them when not necessary */
	xshm.vbox = gtk_vbox_new(FALSE, 0);
	xshm.hbox = nh_gtk_new_and_pack(gtk_hbox_new(FALSE, 0), xshm.vbox, "",
	  TRUE, TRUE, 0);
	xshm.area = nh_gtk_new_and_pack(gtk_drawing_area_new(), xshm.hbox, "",
	  TRUE, TRUE, 0);
	/* FIXME: step_adjustment is 1. It should probably be one square */
	xshm.hadj = GTK_ADJUSTMENT(
	  gtk_adjustment_new(0.0, 0.0, 0.0, 1.0, 0.0, 0.0));
	gtk_signal_connect(GTK_OBJECT(xshm.hadj), "value_changed",
	  (GtkSignalFunc) scroll_event, 0);
	xshm.hscrollbar = gtk_hscrollbar_new(xshm.hadj);
	(void) nh_gtk_new_and_pack(xshm.hscrollbar, xshm.vbox, "",
	  FALSE, FALSE, 0);
	xshm.vadj = GTK_ADJUSTMENT(
	  gtk_adjustment_new(0.0, 0.0, 0.0, 1.0, 0.0, 0.0));
	gtk_signal_connect(GTK_OBJECT(xshm.vadj), "value_changed",
	  (GtkSignalFunc) scroll_event, 0);
	xshm.vscrollbar = gtk_vscrollbar_new(xshm.vadj);
	(void) nh_gtk_new_and_pack(xshm.vscrollbar, xshm.hbox, "",
	  FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(1,3,2)
	gtk_widget_set_double_buffered(xshm.area, FALSE);
#endif
	gtk_widget_set_events(xshm.area,
	  GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
	gtk_signal_connect(GTK_OBJECT(xshm.area), "configure_event",
	  GTK_SIGNAL_FUNC(configure_event), 0);
	gtk_signal_connect(GTK_OBJECT(xshm.area), "expose_event",
	  GTK_SIGNAL_FUNC(expose_event), 0);
	gtk_signal_connect(GTK_OBJECT(xshm.area), "realize",
	  GTK_SIGNAL_FUNC(realize_event), 0);
	gtk_widget_ref(xshm.area);
    }
    if (mode != XSHM_MAP_NONE) {
	if (GTK_WIDGET_REALIZED(xshm.area))
	    xshm_map_size(mode, width, height);
	else {
	    xshm.mode = mode;
	    xshm.map_width = width;
	    xshm.map_height = height;
	}
    }
    return xshm.vbox;
}

static void
button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GdkEventButton map_event = *event;
    map_event.x += xshm.areax;
    map_event.y += xshm.areay;
    xshm.button_press(xshm_map_pixmap ? (void *)xshm_map_pixmap :
      xshm_map_pixbuf ? (void *)xshm_map_pixbuf : (void *)xshm_map_image,
      &map_event, data);
}

void
xshm_map_button_handler(GtkSignalFunc func, gpointer data)
{
    if (xshm.button_press)
	gtk_signal_disconnect_by_func(GTK_OBJECT(xshm.area),
	  GTK_SIGNAL_FUNC(button_press_event), xshm.button_press_data);
    xshm.button_press = (void (*)())func;
    xshm.button_press_data = data;
    if (func)
	gtk_signal_connect(GTK_OBJECT(xshm.area), "button_press_event",
	  GTK_SIGNAL_FUNC(button_press_event), data);
}

void
xshm_map_destroy()
{
#ifdef DEBUG
    fprintf(stderr,"%s", "Map: Destroy\n");
#endif
    if (xshm.pixmap) {
	gdk_pixmap_unref(xshm.pixmap);
	xshm.pixmap = NULL;
    }
    if (xshm.dirty) {
	gdk_region_destroy(xshm.dirty);
	xshm.dirty = NULL;
    }
    if (xshm_map_image) {
	gdk_image_destroy(xshm_map_image);
	xshm_map_image = NULL;
    }
    if (xshm_map_pixbuf) {
	gdk_pixbuf_unref(xshm_map_pixbuf);
	xshm_map_pixmap = NULL;
    }
    if (xshm.gc) {
	gdk_gc_unref(xshm.gc);
	xshm.gc = NULL;
    }
    xshm_map_pixmap = NULL;
    xshm_map_button_handler(NULL, 0);
    if (xshm.area) {
	gtk_widget_unref(xshm.area);
	xshm.area = NULL;
    }
    xshm.map_width = xshm.map_height = 0;
}

void
xshm_map_clear()
{
    GdkRectangle area;
    g_return_if_fail(xshm_map_image != NULL || xshm_map_pixmap != NULL);
    if (xshm_map_image)
	memset(xshm_map_image->mem, 0,
	  xshm_map_image->height * xshm_map_image->bpl);
    else if (xshm_map_pixmap)
	gdk_draw_rectangle(xshm_map_pixmap, xshm.gc, TRUE, 0, 0,
	  xshm.map_width, xshm.map_height);
    else if (xshm_map_pixbuf)
	memset(gdk_pixbuf_get_pixels(xshm_map_pixbuf), 0,
	  gdk_pixbuf_get_height(xshm_map_pixbuf) *
	  gdk_pixbuf_get_rowstride(xshm_map_pixbuf));
    area.x = area.y = 0;
    area.width = xshm.map_width;
    area.height = xshm.map_height;
    xshm_map_draw(&area);
}

void
xshm_map_draw_image(GdkImage *src, int src_x, int src_y, int dest_x, int dest_y,
  int width, int height)
{
    int i, j;
    unsigned char *dp, *sp;
    GdkRectangle area;
    g_return_if_fail(xshm_map_image != NULL || xshm_map_pixmap != NULL);
    g_return_if_fail(src != NULL);
    /* Clip to src */
    if (src_x < 0) {
	width += src_x;
	dest_x -= src_x;
	src_x = 0;
    }
    if (src_y < 0) {
	height += src_y;
	dest_y -= src_y;
	src_y = 0;
    }
    if (src_x + width > src->width)
	width = src->width - src_x;
    if (src_y + height > src->height)
	height = src->height - src_y;
    /* Clip to dest */
    if (dest_x < 0) {
	width += dest_x;
	src_x -= dest_x;
	dest_x = 0;
    }
    if (dest_y < 0) {
	height += dest_y;
	src_y -= dest_y;
	dest_y = 0;
    }
    if (dest_x + width > xshm.map_width)
	width = xshm.map_width - dest_x;
    if (dest_y + height > xshm.map_height)
	height = xshm.map_height - dest_y;
    /* Copy */
    if (xshm_map_image) {
	if (xshm_map_image->bpp != src->bpp ||
	  xshm_map_image->byte_order != src->byte_order)
	    for(j = 0; j < height; j++)
		for(i = 0; i < width; i++)
		    gdk_image_put_pixel(xshm_map_image, dest_x + i, dest_y + j,
		      gdk_image_get_pixel(src, src_x + i, src_y + j));
	else {
	    dp = xshm_map_image->mem + dest_y * xshm_map_image->bpl +
	      dest_x * xshm_map_image->bpp;
	    sp = src->mem + src_y * src->bpl + src_x * src->bpp;
	    i = width * xshm_map_image->bpp;
	    for(j = 0; j < height; j++) {
		memcpy(dp, sp, i);
		dp += xshm_map_image->bpl;
		sp += src->bpl;
	    }
	}
    } else
	gdk_draw_image(xshm_map_pixmap, xshm.gc,
	  src, src_x, src_y, dest_x, dest_y, width, height);
    /* Propagate changes */
    area.x = dest_x;
    area.y = dest_y;
    area.width = width;
    area.height = height;
    xshm_map_draw(&area);
}

void
xshm_map_draw_drawable(GdkDrawable *src, int src_x, int src_y,
  int dest_x, int dest_y, int width, int height)
{
    int i, j;
    gint src_width, src_height;
    GdkRectangle area;
    g_return_if_fail(xshm_map_image != NULL || xshm_map_pixmap != NULL);
    g_return_if_fail(src != NULL);
    /* Clip to src */
    if (src_x < 0) {
	width += src_x;
	dest_x -= src_x;
	src_x = 0;
    }
    if (src_y < 0) {
	height += src_y;
	dest_y -= src_y;
	src_y = 0;
    }
    gdk_drawable_get_size(src, &src_width, &src_height);
    if (src_x + width > src_width)
	width = src_width - src_x;
    if (src_y + height > src_height)
	height = src_height - src_y;
    /* Clip to dest */
    if (dest_x < 0) {
	width += dest_x;
	src_x -= dest_x;
	dest_x = 0;
    }
    if (dest_y < 0) {
	height += dest_y;
	src_y -= dest_y;
	dest_y = 0;
    }
    if (dest_x + width > xshm.map_width)
	width = xshm.map_width - dest_x;
    if (dest_y + height > xshm.map_height)
	height = xshm.map_height - dest_y;
    /* Copy */
    if (xshm_map_image) {
	/* [ALI] You shouldn't be doing this. If you need to draw drawables
	 * then xshm_map should be in pixmap mode.
	 */
	GdkImage *tmp_img;
	g_warning("xshm_map_draw_drawable: Not in pixmap mode");
	tmp_img = gdk_drawable_get_image(src, src_x, src_y, width, height);
	for(j = 0; j < height; j++)
	    for(i = 0; i < width; i++)
		gdk_image_put_pixel(xshm_map_image, dest_x + i, dest_y + j,
		  gdk_image_get_pixel(tmp_img, i, j));
	gdk_image_destroy(tmp_img);
    } else
	gdk_draw_drawable(xshm_map_pixmap, xshm.gc,
	  src, src_x, src_y, dest_x, dest_y, width, height);
    /* Propagate changes */
    area.x = dest_x;
    area.y = dest_y;
    area.width = width;
    area.height = height;
    xshm_map_draw(&area);
}

void
xshm_map_draw_pixbuf(GdkPixbuf *src, int src_x, int src_y,
  int dest_x, int dest_y, int width, int height)
{
    GdkRectangle area;
    int src_width;
    int src_height;
    g_return_if_fail(xshm_map_pixbuf != NULL);
    g_return_if_fail(src != NULL);
    src_width = gdk_pixbuf_get_width(src);
    src_height = gdk_pixbuf_get_height(src);
    /* Clip to src */
    if (src_x < 0) {
	width += src_x;
	dest_x -= src_x;
	src_x = 0;
    }
    if (src_y < 0) {
	height += src_y;
	dest_y -= src_y;
	src_y = 0;
    }
    if (src_x + width > src_width)
	width = src_width - src_x;
    if (src_y + height > src_height)
	height = src_height - src_y;
    /* Clip to dest */
    if (dest_x < 0) {
	width += dest_x;
	src_x -= dest_x;
	dest_x = 0;
    }
    if (dest_y < 0) {
	height += dest_y;
	src_y -= dest_y;
	dest_y = 0;
    }
    if (dest_x + width > xshm.map_width)
	width = xshm.map_width - dest_x;
    if (dest_y + height > xshm.map_height)
	height = xshm.map_height - dest_y;
    /* Copy */
    gdk_pixbuf_copy_area(src, src_x, src_y, width, height,
      xshm_map_pixbuf, dest_x, dest_y);
    /* Propagate changes */
    area.x = dest_x;
    area.y = dest_y;
    area.width = width;
    area.height = height;
    xshm_map_draw(&area);
}

/*
  $Id: xshm.c,v 1.5 2000-12-15 15:38:10 j_ali Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

/*
  MIT-SHM library
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef __FreeBSD__
#include <machine/param.h>
#endif
#include <sys/types.h>
#include "winGTK.h"
#include "xshm.h"
#ifdef WINGTK_X11
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#ifdef WINGTK_X11
static volatile int	errflg;

static int
ErrorHandler(Display *dpy, XErrorEvent *event)
{
    errflg = 1;

    return 0;
}
#endif

#ifdef WINGTK_X11
void
XShmDestroyXShmImage(Display *dpy, XShmImage *xshm)
{
    XFreePixmap(dpy, xshm->pixmap);
    XDestroyImage(xshm->image);
    if(xshm->shmflg){
	XShmDetach(dpy, &xshm->shminfo);
	shmdt(xshm->shminfo.shmaddr);
	shmctl(xshm->shminfo.shmid, IPC_RMID, 0);
    }
    free(xshm);
}
#else
void
XShmDestroyXShmImage(XShmImage *xshm)
{
    gdk_bitmap_unref(xshm->pixmap);
    gdk_image_destroy(xshm->image);
    free(xshm);
}
#endif

#ifdef WINGTK_X11
XShmImage *
XShmCreateXShmImage(Display *dpy, int width, int height)
{
    int		(*saved_handler)(Display *, XErrorEvent *);
    int		ret;
    XShmImage	*xshm;
    int		major, minor;
    Bool	pixmaps;

    int		screen;
    Window	root;
    int		depth;
    Visual	*visual;

    screen = XDefaultScreen(dpy);
    root = RootWindow(dpy, screen);
    depth = DefaultDepth(dpy, screen);
    visual = DefaultVisual(dpy, screen);
    
    xshm = (XShmImage *)alloc(sizeof(XShmImage));

    if(XShmQueryExtension(dpy) != TRUE)
	goto no_xshm;

    xshm->image = NULL;
    XShmQueryVersion(dpy, &major, &minor, &pixmaps);
    xshm->shmflg = pixmaps;

    if(xshm->shmflg != TRUE)
	goto no_xshm;
/*
  caliculate image size
 */
    xshm->image = XShmCreateImage(
	dpy, visual, depth, ZPixmap, 0,
	&xshm->shminfo, width, height);
/*
  Allocate shared memory
 */
    ret = xshm->shminfo.shmid = shmget(
	IPC_PRIVATE,
	xshm->image->bytes_per_line * xshm->image->height,
	IPC_CREAT | 0777);

    if(ret < 0)
	goto no_xshm;

    ret = (int)xshm->shminfo.shmaddr = shmat(xshm->shminfo.shmid, 0, 0);

    if(ret < 0){
	shmctl(xshm->shminfo.shmid, IPC_RMID, 0);
	goto no_xshm;
    }
    xshm->shminfo.readOnly = FALSE;

    errflg = 0;
    saved_handler = XSetErrorHandler(ErrorHandler);

    ret = XShmAttach(dpy, &xshm->shminfo);
    if(ret != TRUE){
	shmdt(xshm->shminfo.shmaddr);
	shmctl(xshm->shminfo.shmid, IPC_RMID, 0);
	goto no_xshm;
    }

    XSync(dpy, FALSE);
    XSetErrorHandler(saved_handler);
    if(errflg){
	shmdt(xshm->shminfo.shmaddr);
	shmctl(xshm->shminfo.shmid, IPC_RMID, 0);
	goto no_xshm;
    }

    ret = shmctl(xshm->shminfo.shmid, IPC_RMID, 0);
    if(ret < 0){
	XShmDetach(dpy, &xshm->shminfo);
	shmdt(xshm->shminfo.shmaddr);
	shmctl(xshm->shminfo.shmid, IPC_RMID, 0);
	goto no_xshm;
    }

    xshm->image->data = xshm->shminfo.shmaddr;

    xshm->pixmap = XShmCreatePixmap(
	dpy, root, xshm->shminfo.shmaddr,
	&xshm->shminfo, width, height, depth);

    return xshm;

no_xshm:
    fprintf(stderr, "warning: cannot use shared memory\n");

    xshm->shmflg = 0;
    xshm->pixmap = XCreatePixmap(dpy, root, width, height, depth);
    xshm->image = XGetImage(
	dpy, xshm->pixmap, 0, 0, width, height,
	visual->red_mask | visual->green_mask | visual->blue_mask,
	ZPixmap);

    return xshm;
}
#else		/* WINGTK_X11 */
XShmImage *
XShmCreateXShmImage(GdkWindow *w, int width, int height)
{
    XShmImage	*xshm;

    xshm = (XShmImage *)alloc(sizeof(XShmImage));

    xshm->shmflg = 0;
    xshm->pixmap = gdk_pixmap_new(w, width, height, -1);
    xshm->image = gdk_image_get((GdkWindow *)xshm->pixmap, 0, 0, width, height);

    return xshm;
}
#endif		/* WINGTK_X11 */

#ifdef WINGTK_X11
void
XShmClearXShmImage(Display *dpy, XShmImage *xshm)
{
    memset(xshm->image->data, 0, xshm->image->bytes_per_line * xshm->image->height);

    if(xshm->shmflg == FALSE){
	GC		gc;
	XGCValues	gcval;

	gcval.foreground = BlackPixel(dpy, DefaultScreen(dpy));

	gc = XCreateGC(
	    dpy,
	    RootWindow(dpy, DefaultScreen(dpy)),
	    GCForeground,
	    &gcval);
	XFillRectangle(
	    dpy, xshm->pixmap, gc,
	    0, 0,
	    xshm->image->width, xshm->image->height);

	XFreeGC(dpy, gc);
    }
}
#else		/* WINGTK_X11 */
void
XShmClearXShmImage(XShmImage *xshm)
{
    int		i, j;
    gint	width, height;
    GdkGC	*gc;
    GdkColor	black={0, 0, 0, 0};

    gdk_window_get_size((GdkWindow *)xshm->pixmap, &width, &height);

    /*
     * How do we efficiently zero a GdkImage?
     * More to the point perhaps, do we actually need the image?
     */

    for(i=0;i<width;i++)
	for(j=0;j<height;j++)
	    gdk_image_put_pixel(xshm->image, i, j, 0);

    gc = gdk_gc_new((GdkWindow *)xshm->pixmap);
    gdk_gc_set_foreground(gc, &black);
    gdk_draw_rectangle(xshm->pixmap, gc, TRUE, 0, 0, width, height);
    gdk_gc_unref(gc);
}
#endif		/* WINGTK_X11 */

#ifdef WINGTK_X11
void
XShmSyncXShmImage(Display *dpy, XShmImage *xshm)
{
    if(xshm->shmflg == FALSE){
	XPutImage(
	    dpy, xshm->pixmap, DefaultGC(dpy, DefaultScreen(dpy)), xshm->image,
	    0, 0, 0, 0,
	    xshm->image->width, xshm->image->height);
    }
}
#else
void
XShmSyncXShmImage(XShmImage *xshm)
{
    gint	width, height;
    GdkGC	*gc;

    gdk_window_get_size((GdkWindow *)xshm->pixmap, &width, &height);
    gc = gdk_gc_new((GdkWindow *)xshm->pixmap);
    gdk_draw_image(xshm->pixmap, gc, xshm->image, 0, 0, 0, 0, width, height);
    gdk_gc_unref(gc);
}
#endif

#ifdef WINGTK_X11
void
XShmSyncXShmImageRegion(Display *dpy, XShmImage *xshm, int x, int y, int width, int height)
{
    if(xshm->shmflg == FALSE){
	XPutImage(
	    dpy, xshm->pixmap, DefaultGC(dpy, DefaultScreen(dpy)), xshm->image,
	    x, y, x, y,
	    width, height);
    }
}
#else		/* WINGTK_X11 */
void
XShmSyncXShmImageRegion(XShmImage *xshm, int x, int y, int width, int height)
{
    GdkGC	*gc;
    gc = gdk_gc_new((GdkWindow *)xshm->pixmap);
    gdk_draw_image(xshm->pixmap, gc, xshm->image, x, y, x, y, width, height);
    gdk_gc_unref(gc);
}
#endif		/* WINGTK_X11 */

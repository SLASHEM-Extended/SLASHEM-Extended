/*
  $Id: xshm.c,v 1.1 2000-08-15 19:55:16 wacko Exp $
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
#include <sys/ipc.h>
#include <sys/shm.h>

#include "xshm.h"

static volatile int	errflg;

static int
ErrorHandler(Display *dpy, XErrorEvent *event)
{
    errflg = 1;

    return 0;
}

void
XShmDestroyXShmImage(Display *dpy, XShmImage *xshm)
{
    if(xshm->shmflg){
	XShmDetach(dpy, &xshm->shminfo);
	XDestroyImage(xshm->image);
	XFreePixmap(dpy, xshm->pixmap);
	shmdt(xshm->shminfo.shmaddr);
	shmctl(xshm->shminfo.shmid, IPC_RMID, 0);
	free(xshm);
    }
}

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
    
    xshm = (XShmImage *)malloc(sizeof(XShmImage));

    if(XShmQueryExtension(dpy) != True)
	goto no_xshm;

    if(xshm == NULL){
	fprintf(stderr, "warning: cannot allocate shared memory(size = %d)\n", 
		xshm->image->bytes_per_line * xshm->image->height);

	return NULL;
    }

    xshm->image = NULL;
    XShmQueryVersion(dpy, &major, &minor, &pixmaps);
    xshm->shmflg = pixmaps;

    if(xshm->shmflg != True)
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
    xshm->shminfo.readOnly = False;

    errflg = 0;
    saved_handler = XSetErrorHandler(ErrorHandler);

    ret = XShmAttach(dpy, &xshm->shminfo);
    if(ret != True){
	shmdt(xshm->shminfo.shmaddr);
	shmctl(xshm->shminfo.shmid, IPC_RMID, 0);
	goto no_xshm;
    }

    XSync(dpy, False);
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

void
XShmClearXShmImage(Display *dpy, XShmImage *xshm)
{
    memset(xshm->image->data, 0, xshm->image->bytes_per_line * xshm->image->height);

    if(xshm->shmflg == False){
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

void
XShmSyncXShmImage(Display *dpy, XShmImage *xshm)
{
    if(xshm->shmflg == False){
	XPutImage(
	    dpy, xshm->pixmap, DefaultGC(dpy, DefaultScreen(dpy)), xshm->image,
	    0, 0, 0, 0,
	    xshm->image->width, xshm->image->height);
    }
}

void
XShmSyncXShmImageRegion(Display *dpy, XShmImage *xshm, int x, int y, int width, int height)
{
    if(xshm->shmflg == False){
	XPutImage(
	    dpy, xshm->pixmap, DefaultGC(dpy, DefaultScreen(dpy)), xshm->image,
	    x, y, x, y,
	    width, height);
    }
}

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/Xmd.h>

typedef struct _XShmImage{
    int		    shmflg; /* if True image and pixmap are on shared memory */
    XShmSegmentInfo shminfo;
    XImage	    *image;
    Pixmap	    pixmap;
} XShmImage;

extern void		 XShmDestroyXShmImage(Display *dpy, XShmImage *xshm);
extern XShmImage	*XShmCreateXShmImage(Display *dpy, int width, int height);
extern void		XShmClearXShmImage(Display *dpy, XShmImage *map);
extern void		XShmSyncXShmImage(Display *dpy, XShmImage *map);
extern void		XShmSyncXShmImageRegion(Display *dpy, XShmImage *xshm, int x, int y, int width, int height);

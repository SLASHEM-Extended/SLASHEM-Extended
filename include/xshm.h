#ifdef WINGTK_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/Xmd.h>
#endif

typedef struct _XShmImage{
    int		    shmflg; /* if True image and pixmap are on shared memory */
#ifdef WINGTK_X11
    XShmSegmentInfo shminfo;
    XImage	    *image;
    Pixmap	    pixmap;
#else
    GdkImage	    *image;
    GdkPixmap	    *pixmap;
#endif
} XShmImage;

#ifdef WINGTK_X11
extern void		 XShmDestroyXShmImage(Display *dpy, XShmImage *xshm);
extern XShmImage	*XShmCreateXShmImage(Display *dpy, int width, int height);
extern void		XShmClearXShmImage(Display *dpy, XShmImage *map);
extern void		XShmSyncXShmImage(Display *dpy, XShmImage *map);
extern void		XShmSyncXShmImageRegion(Display *dpy, XShmImage *xshm, int x, int y, int width, int height);
#else
extern void		 XShmDestroyXShmImage(XShmImage *xshm);
extern XShmImage	*XShmCreateXShmImage(GdkWindow *w, int width, int height);
extern void		XShmClearXShmImage(XShmImage *map);
extern void		XShmSyncXShmImage(XShmImage *map);
extern void		XShmSyncXShmImageRegion(XShmImage *xshm, int x, int y, int width, int height);
#endif

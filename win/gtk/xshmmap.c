/*
  $Id: xshmmap.c,v 1.4 2000-09-20 01:48:58 wacko Exp $
 */
/*
  GTK+ NetHack Copyright (c) Issei Numata 1999-2000
  GTK+ NetHack may be freely redistributed.  See license for details. 
*/

/*
  fast X map library
 */

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include "winGTK.h"
#include "xshm.h"

#define PIXEL8(x)	(*((CARD8 *)x))
#define PIXEL16(x)	(*((CARD16 *)x))
#define PIXEL24(x)	((((CARD8 *)x)[0]) | ((((CARD8 *)x)[1])<<8) | ((((CARD8 *)x)[2])<<16))
#define PIXEL32(x)	(*((CARD32 *)x))

static XShmImage	*map;

#ifdef WINGTK_X11
static XImage		*tile_img;
static XImage		*tmp_img;
#else
static GdkImage		*tile_img;
static GdkImage		*tmp_img;
#endif

static TileTab		*Tile;

#ifdef WINGTK_X11
static Display		*dpy;
static GC		gc;
#else
static GdkWindow	*dpy;		/* A (random) window on display */
static GdkGC		*gc;
#endif

#ifdef WINGTK_X11
void
xshm_init(Display *d)
{
    dpy = d;
}
#else
void
xshm_init(GdkWindow *w)
{
    dpy = w;
}
#endif

int
xshm_map_init(int width, int height)
{
#ifdef WINGTK_X11
    gc = DefaultGC(dpy, DefaultScreen(dpy));

    map = XShmCreateXShmImage(dpy, width, height);
#else
    map = XShmCreateXShmImage(dpy, width, height);
    gc = gdk_gc_new(map->pixmap);
#endif

    return map->shmflg;
}

void
xshm_map_destroy()
{
#ifdef WINGTK_X11
    XShmDestroyXShmImage(dpy, map);
#else
    XShmDestroyXShmImage(map);
#endif
}

void
xshm_map_clear()
{
#ifdef WINGTK_X11
    XShmClearXShmImage(dpy, map);
#else
    XShmClearXShmImage(map);
#endif
}

void
xshm_map_draw(GdkWindow *w, int srcx, int srcy, int dstx, int dsty, int width, int height)
{
#ifdef WINGTK_X11
    XCopyArea(
	dpy, map->pixmap, GDK_WINDOW_XWINDOW(w), gc,
	srcx, srcy,
	width, height,
	dstx, dsty);
#else
    gdk_draw_pixmap(w, gc, map->pixmap, srcx, srcy, dstx, dsty, width, height);
#endif
}

void
x_tmp_clear()
{
#ifdef WINGTK_X11
    memset(tmp_img->data, 0, tmp_img->bytes_per_line * tmp_img->height);
#else
    memset(tmp_img->mem, 0, tmp_img->bpl * tmp_img->height);
#endif
}

void
x_tile_init(GdkImage *img, TileTab *t)
{
    Tile = t;
#ifdef WINGTK_X11
    tile_img = GDK_IMAGE_XIMAGE(img);
    tmp_img = XSubImage(tile_img, 0, 0, t->unit_width, t->unit_height);
#else
    tile_img = img;
    tmp_img = gdk_image_new(GDK_IMAGE_FASTEST, gdk_visual_get_system(),
      t->unit_width, t->unit_height);
#endif
}

void
x_tile_destroy()
{
#ifdef WINGTK_X11
    XDestroyImage(tile_img);
#else
    gdk_image_destroy(tile_img);
#endif
}

void
x_tile_tmp_draw_rectangle(int ofsx, int ofsy, int c)
{
    int i;
    int width = tmp_img->width;
    int height = tmp_img->height;

    if(ofsy >= 0 && ofsy < height)
	for(i=0 ; i<width ; ++i)
	    if(ofsx + i >= 0 && ofsx + i < width)
#ifdef WINGTK_X11
		XPutPixel(tmp_img, ofsx + i, ofsy, c);
#else
		gdk_image_put_pixel(tmp_img, ofsx + i, ofsy, c);
#endif

    if(ofsy + height - 1 >= 0 && ofsy + height - 1 < height)
	for(i=0 ; i<width ; ++i)
	    if(ofsx + i >= 0 && ofsx + i < width)
#ifdef WINGTK_X11
		XPutPixel(tmp_img, ofsx + i, ofsy + height - 1, c);
#else
		gdk_image_put_pixel(tmp_img, ofsx + i, ofsy + height -1, c);
#endif

    if(ofsx >= 0 && ofsx < width)
	for(i=0 ; i<height ; ++i)
	    if(ofsy + i >= 0 && ofsy + i < height)
#ifdef WINGTK_X11
		XPutPixel(tmp_img, ofsx, ofsy + i, c);
#else
		gdk_image_put_pixel(tmp_img, ofsx, ofsy + i, c);
#endif

    if(ofsx + width - 1 >= 0 && ofsx + width - 1 < width)
	for(i=0 ; i<height ; ++i)
	    if(ofsy + i >= 0 && ofsy + i < height)
#ifdef WINGTK_X11
		XPutPixel(tmp_img, ofsx + width - 1, ofsy + i, c);
#else
		gdk_image_put_pixel(tmp_img, ofsx + width - 1, ofsy + i, c);
#endif
}

void
x_tile_tmp_draw(int srcx, int srcy, int ofsx, int ofsy)
{
    int i, j;
#ifdef WINGTK_X11
    char *src, *tmp;
    CARD8 *src8, *tmp8;
    CARD16 *src16, *tmp16;
    CARD32 *src24, *tmp24;
    CARD32 *src32, *tmp32;
    CARD32 nul_pixel = 0;

    int bpp = tile_img->bytes_per_line / tile_img->width;
    int width = tmp_img->width;
    int height = tmp_img->height;

    if(tile_img->bits_per_pixel <= 8)
	nul_pixel = PIXEL8(tile_img->data);
    else if(tile_img->bits_per_pixel <= 16)
	nul_pixel = PIXEL16(tile_img->data);
    else if(tile_img->bits_per_pixel <= 24)
	nul_pixel = PIXEL24(tile_img->data);
    else if(tile_img->bits_per_pixel <= 32)
	nul_pixel = PIXEL32(tile_img->data);
#else
    guint32 nul_pixel, pixel;
    int width = tmp_img->width;
    int height = tmp_img->height;

    nul_pixel = gdk_image_get_pixel(tile_img, 0, 0);
#endif

    if(ofsx < 0){
	srcx -= ofsx;
	width += ofsx;
	ofsx = 0;
    }
    if(ofsy < 0){
	srcy -= ofsy;
	height += ofsy;
	ofsy = 0;
    }
    if(ofsx > 0){
	width -= ofsx;
    }

    if(ofsy > 0){
	height -= ofsy;
    }

    if(width <= 0)
	return;

    if(height <= 0)
	return;

#ifdef WINGTK_X11
    src32 = (CARD32 *)src16 = (CARD16 *)src24 = src8 = src = 
	tile_img->data +
	(tile_img->bytes_per_line * srcy) + 
	bpp * srcx;

    tmp32 = (CARD32 *)tmp16 = (CARD16 *)tmp24 = tmp8 = tmp = 
	tmp_img->data +
	(tmp_img->bytes_per_line * ofsy) + 
	bpp * ofsx;

    if(!Tile->transparent){
	for(i=0 ; i < height ; ++i){
	    memcpy(tmp8, src8, width * bpp);
	    src8 += tile_img->bytes_per_line;
	    tmp8 += tmp_img->bytes_per_line;
	}
    }
    else if(tile_img->bits_per_pixel <= 8){
	for(i=0 ; i < height ; ++i){
	    for(j=0 ; j<width ; ++j)
		if(src8[j] != nul_pixel)
		    tmp8[j] = src8[j];
	    src8 += tile_img->bytes_per_line;
	    tmp8 += tmp_img->bytes_per_line;
	}
    }
    else if(tile_img->bits_per_pixel <= 16){
	for(i=0 ; i < height ; ++i){
	    for(j=0 ; j<width ; ++j)
		if(src16[j] != nul_pixel)
		    tmp16[j] = src16[j];
	    src16 += (tile_img->bytes_per_line / bpp);
	    tmp16 += (tmp_img->bytes_per_line / bpp);
	}
    }
    else if(tile_img->bits_per_pixel <= 24){
	for(i=0 ; i < height ; ++i){
	    for(j=0 ; j<width ; ++j)
		if(PIXEL24(&src24[j]) != nul_pixel)
		    memcpy(&tmp24[j], &src24[j], 3);
	    src24 += tile_img->bytes_per_line;
	    tmp24 += tmp_img->bytes_per_line;
	}
    }
    else if(tile_img->bits_per_pixel <= 32){
	for(i=0 ; i < height ; ++i){
	    for(j=0 ; j<width ; ++j)
		if(src32[j] != nul_pixel)
		    tmp32[j] = src32[j];
	    src32 += (tile_img->bytes_per_line / bpp);
	    tmp32 += (tmp_img->bytes_per_line / bpp);
	}
    }
#else
    for(i=0 ; i < height ; ++i)
	for(j=0 ; j < width ; ++j){
	    pixel = gdk_image_get_pixel(tile_img, srcx + j, srcy + i);
	    if (!Tile->transparent || pixel != nul_pixel)
		gdk_image_put_pixel(tmp_img, ofsx + j, ofsy + i, pixel);
	}
#endif
}

void
xshm_map_tile_draw(int dstx, int dsty)
{
#ifdef WINGTK_X11
    int i;
    char *tmp, *dst;

    int bpp = tile_img->bytes_per_line / tile_img->width;

    tmp = tmp_img->data;
    dst = map->image->data +
	(map->image->bytes_per_line * dsty) + 
	bpp * dstx;
	
    for(i=0 ; i < tmp_img->height ; ++i){
	memcpy(dst, tmp, tmp_img->bytes_per_line); 
	tmp += tmp_img->bytes_per_line;
	dst += map->image->bytes_per_line;
    }

    XShmSyncXShmImageRegion(dpy, map, dstx, dsty,
      tmp_img->width, tmp_img->height); 
#else
    int i, j;

    for(i=0; i < tmp_img->height; ++i)
	for(j=0; j < tmp_img->width; ++j)
	    gdk_image_put_pixel(map->image, dstx + j, dsty + i,
	      gdk_image_get_pixel(tmp_img, j, i));

    XShmSyncXShmImageRegion(map, dstx, dsty, tmp_img->width, tmp_img->height); 
#endif
}

GdkPixmap *
GTK_tmp_to_pixmap()
{
    GdkPixmap * ret;

    ret = gdk_pixmap_new ( NULL, tmp_img->width, tmp_img->height, 
    	(gdk_visual_get_system())->depth);
    
#ifdef WINGTK_X11
    /* FIXME 
     * Need to add code that takes tmp_img (which is an XSubImage) and draw it onto
     * ret (which is a GdkPixmap).   Anyone know how to do this?
     */
#else   
    gdk_draw_image(ret, gc, tmp_img, 0, 0, 
    			0, 0, tmp_img->width, tmp_img->height);
#endif

    return ret;
}

/*
  $Id: xshmmap.c,v 1.1 2000-08-15 19:55:16 wacko Exp $
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

static XImage		*tile_img;
static XImage		*tmp_img;

static TileTab		*Tile;

static Display		*dpy;
static GC		gc;

void
xshm_init(Display *d)
{
    dpy = d;
}

int
xshm_map_init(int width, int height)
{
    gc = DefaultGC(dpy, DefaultScreen(dpy));

    map = XShmCreateXShmImage(dpy, width, height);

    return map->shmflg;
}

void
xshm_map_destroy()
{
    XShmDestroyXShmImage(dpy, map);
}

void
xshm_map_clear()
{
    XShmClearXShmImage(dpy, map);
}

void
xshm_map_draw(Window w, int srcx, int srcy, int dstx, int dsty, int width, int height)
{
    XCopyArea(
	dpy, map->pixmap, w, gc,
	srcx, srcy,
	width, height,
	dstx, dsty);
}

void
x_tmp_clear()
{
    memset(tmp_img->data, 0, tmp_img->bytes_per_line * tmp_img->height);
}

void
x_tile_init(XImage *img, TileTab *t)
{
    tile_img = img;
    Tile = t;
    tmp_img = XSubImage(img, 0, 0, t->unit_width, t->unit_height);
}

void
x_tile_destroy()
{
    XDestroyImage(tile_img);
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
		XPutPixel(tmp_img, ofsx + i, ofsy, c);

    if(ofsy + height - 1 >= 0 && ofsy + height - 1 < height)
	for(i=0 ; i<width ; ++i)
	    if(ofsx + i >= 0 && ofsx + i < width)
		XPutPixel(tmp_img, ofsx + i, ofsy + height - 1, c);

    if(ofsx >= 0 && ofsx < width)
	for(i=0 ; i<height ; ++i)
	    if(ofsy + i >= 0 && ofsy + i < height)
		XPutPixel(tmp_img, ofsx, ofsy + i, c);

    if(ofsx + width - 1 >= 0 && ofsx + width - 1 < width)
	for(i=0 ; i<height ; ++i)
	    if(ofsy + i >= 0 && ofsy + i < height)
		XPutPixel(tmp_img, ofsx + width - 1, ofsy + i, c);
}
void
x_tile_tmp_draw(int srcx, int srcy, int ofsx, int ofsy)
{
    int i, j;
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
}

void
xshm_map_tile_draw(int dstx, int dsty)
{
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

    XShmSyncXShmImageRegion(dpy, map, dstx, dsty, tmp_img->width, tmp_img->height); 
}







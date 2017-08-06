/* $Id: tile_t.h,v 1.3 2003-05-21 04:15:53 wacko Exp $ */
/* NetHack may be freely redistributed.  See license for details. */

typedef unsigned char pixval;

typedef struct {
    pixval r, g, b;
} pixel;

#define MAXCOLORMAPSIZE 	256

#define CM_RED		0
#define CM_GREEN	1
#define CM_BLUE 	2

#define DEFAULT_BACKGROUND	{ 71, 108, 108 }	/* For transparancy */

/* shared between reader and writer */
extern pixval ColorMap[3][MAXCOLORMAPSIZE];
extern int colorsinmap;
/* writer's accumulated colormap */
extern pixval MainColorMap[3][MAXCOLORMAPSIZE];
extern int colorsinmainmap;

#include "dlb.h"	/* for MODEs */

extern int tile_x, tile_y;

/*
#define MAX_TILE_X 32
#define MAX_TILE_Y 32
#define MAX_TILE_X 48
#define MAX_TILE_Y 64
*/
#define MAX_TILE_X 128
#define MAX_TILE_Y 128

#define pixel_equal(x,y) ((x.r == y.r) && (x.g == y.g) && (x.b == y.b))

extern boolean fopen_text_file(const char *, const char *);
extern boolean fopen_virtual_text_file(const char *base_name, const char *overlay_name, const char *type);
extern boolean peek_text_tile_info(char ttype[BUFSZ], int *number, char name[BUFSZ]);
extern boolean read_text_tile_info(pixel (*)[MAX_TILE_X], char *ttype, int *number, char *name);
extern boolean read_text_tile(pixel (*)[MAX_TILE_X]);
extern boolean write_text_tile_info(pixel (*)[MAX_TILE_X], const char *ttype, int number, const char *name);
extern boolean write_text_tile(pixel (*)[MAX_TILE_X]);
extern boolean fclose_text_file(void);

extern void init_colormap(void);
extern void merge_colormap(void);

#if defined(MICRO) || defined(WIN32)
#undef exit
# if !defined(MSDOS) && !defined(WIN32)
extern void exit(int);
# endif
#endif

/*tile_t.h*/

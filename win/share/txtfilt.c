/*   Copyright (c) J. Ali Harlow 2000                               */
/*   NetHack may be freely redistributed.  See license for details. */

/*
 * Filter a text file file by removing any tiles which have, or appear
 * to have been magnified from a lower resolution.
 */

#include "config.h"
#include "tile.h"

#define pixel_eq(pixa, pixb)  ((pixa.r == pixb.r) && (pixa.g == pixb.g) \
					 	  && (pixa.b == pixb.b))

/* [WAC] do this without memcmp() */
static boolean
low_res_tile(bitmap)
pixel (*bitmap)[MAX_TILE_X];
{
    int x, y;
    pixel def_background = DEFAULT_BACKGROUND;
    boolean retval = TRUE;
    int blanks;
    int topblanks, bottomblanks;

    for(y = 0; y < tile_y - 1; y += 2)
    	for(x = 0; x < tile_x - 1; x += 2) {
    		if (!pixel_eq(bitmap[y][x], bitmap[y][x+1])) retval = FALSE;
    		if (!pixel_eq(bitmap[y][x], bitmap[y+1][x])) retval = FALSE;
    		if (!pixel_eq(bitmap[y][x], bitmap[y+1][x+1])) retval = FALSE;
    	}

    if (tile_x != 48 || tile_y != 64 || retval) return TRUE;

    /* 3D tiles - detected via tile_x = 48 and tile_y = 64 
     * Remove recentered tiles
     */

    /* For x axis, tiles may be "shifted", so count whitespace per line */
    for(y = 0; y < tile_y; y++) {
    	blanks = 0;
    	for(x = 0; x < tile_x; x++) {
    		if (pixel_eq(bitmap[y][x], def_background)) blanks++;
    	}
    	if (blanks < 16) return FALSE;
    }

#if 0
    for(x = 0; x < tile_x; x++) {
    	blanks = 0;
	for(y = 0; y < tile_y; y++) {
    		if (pixel_eq(bitmap[y][x], def_background)) blanks++;
    	}
    	minyblanks = min(minyblanks, blanks);
    }
#endif

    topblanks = 0;
    bottomblanks = 0;

    for(y = 0; y < tile_y; y++) {
    	for(x = 0; x < tile_x; x++) {
    		if (!pixel_eq(bitmap[y][x], def_background)) break;
    	}
    	if (x == tile_x) topblanks++;
    	else break;
    }

    for(y = tile_y -1 ; y > topblanks; y--) {
    	for(x = 0; x < tile_x; x++) {
    		if (!pixel_eq(bitmap[y][x], def_background)) break;
    	}
    	if (x == tile_x) bottomblanks++;
    	else break;
    }
    
    if ((topblanks + bottomblanks) < 32) return FALSE;


#if 0
    for(y = 0; y < 16; y++)
    	for(x = 0; x < tile_x; x++) {
    		if (!pixel_eq(bitmap[y][x], def_background)) return FALSE;
    	}

    for(y = 16; y < 48; y++) {
    	for(x = 0; x < 8; x++) {
    		if (!pixel_eq(bitmap[y][x], def_background)) return FALSE;
    	}
    	for(x = 40; x < 48; x++) {
    		if (!pixel_eq(bitmap[y][x], def_background)) return FALSE;
    	}
    }
   
    for(y = 48; y < 16; y++)
    	for(x = 0; x < tile_x; x++) {
    		if (!pixel_eq(bitmap[y][x], def_background)) return FALSE;
    	}

#endif
    	
    return TRUE;
}

#if 0
static boolean
low_res_tile(bitmap)
pixel (*bitmap)[MAX_TILE_X];
{
    int x, y;

    for(y = 0; y < tile_y - 1; y += 2)
	if (memcmp(bitmap[y], bitmap[y + 1], sizeof(*bitmap)))
	    return FALSE;

    for(x = 0; x < tile_x - 1; x += 2)
	for(y = 0; y < tile_y; y++)
	    if (memcmp(&bitmap[y][x],&bitmap[y][x + 1],sizeof(bitmap[y][x])))
		return FALSE;

    return TRUE;
}
#endif

int
main(argc, argv)
int argc;
char **argv;
{
    char ttype[BUFSZ];
    int number;
    char name[BUFSZ];
    pixel pixels[MAX_TILE_Y][MAX_TILE_X];

    if (argc != 3) {
	Fprintf(stderr, "usage: txtfilt infile outfile\n");
	exit(EXIT_FAILURE);
    }

    if (!fopen_text_file(argv[1], RDTMODE))
	exit(EXIT_FAILURE);
    if (!fopen_text_file(argv[2], WRTMODE))
	exit(EXIT_FAILURE);

    while (read_text_tile_info(pixels, ttype, &number, name)) {
	if (!low_res_tile(pixels))
	    write_text_tile_info(pixels, ttype, number, name);
    }
    fclose_text_file();

    exit(EXIT_SUCCESS);
    /*NOTREACHED*/
    return 0;
}

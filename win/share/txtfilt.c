/*   Copyright (c) J. Ali Harlow 2000                               */
/*   NetHack may be freely redistributed.  See license for details. */

/*
 * Filter a text file file by removing any tiles which have, or appear
 * to have been magnified from a lower resolution.
 */

#include <errno.h>
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

    if (tile_x != 48 || tile_y != 64 || retval) return retval;

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

static int match_np = 0;
static char **match_p = NULL;

int match_load(char *file)
{
    FILE *fp;
    int i, no_alloc = 32;
    char buf[1024];
    match_np = 0;
    fp = fopen(file, "r");
    if (!fp)
	return -1;
    free(match_p);
    match_p = malloc(no_alloc * sizeof (*match_p));
    if (!match_p)
    {
	errno = ENOMEM;			/* Not all mallocs set errno */
	return -1;
    }
    while (fgets(buf, sizeof(buf), fp))
    {
	i = strlen(buf);
	if (buf[i - 1] != '\n')
	{
	    Fprintf(stderr, "Line %d: Too long (max %d)\n", match_np + 1,
	      sizeof(buf) - 1);
	    errno = EINVAL;
	    return -1;
	}
	buf[i - 1] = '\0';
	if (match_np >= no_alloc)
	{
	    no_alloc *= 2;
	    match_p = realloc(match_p, no_alloc * sizeof (*match_p));
	    if (!match_p)
	    {
		errno = ENOMEM;
		return -1;
	    }
	}
	match_p[match_np] = strdup(buf);
	if (!match_p[match_np++])
	{
	    errno = ENOMEM;
	    return -1;
	}
    }
    fclose(fp);
    match_p = realloc(match_p, match_np * sizeof (*match_p));
    return 0;
}

boolean
match_found(char *name)
{
    int i;
    for(i = 0; i < match_np; i++)
    {
	if (!strcmp(match_p[i], name))
	    return TRUE;
    }
    return FALSE;
}

int
main(argc, argv)
int argc;
char **argv;
{
    int argn = 1;
    boolean match_mode = FALSE;
    FILE *match_fp;
    char ttype[BUFSZ];
    int number;
    char name[BUFSZ];
    pixel pixels[MAX_TILE_Y][MAX_TILE_X];

    while (argn < argc) {
	if (!strcmp(argv[argn], "-f")) {
	    match_mode = TRUE;
	    argn ++;
	    if (argn >= argc)
	    {
		Fprintf(stderr, "txtfilt: -f option requires a match file\n");
		exit(EXIT_FAILURE);
	    }
	    if (match_load(argv[argn]))
	    {
		perror(argv[argn]);
		exit(EXIT_FAILURE);
	    }
	    argn ++;
	} else if (!strcmp(argv[argn], "-p")) {
	    argn ++;
	    if (argn >= argc)
	    {
		Fprintf(stderr, "txtfilt: -p option requires a palette file\n");
		exit(EXIT_FAILURE);
	    }
	    if (!read_text_file_colormap(argv[argn]))
	    {
		perror(argv[argn]);
		exit(EXIT_FAILURE);
	    }
	    init_colormap();
	    argn ++;
	}
	else
	    break;
    }

    if (argc - argn != 2) {
	Fprintf(stderr,
	  "usage: txtfilt [-f match-file] [-p palette-file] infile outfile\n");
	exit(EXIT_FAILURE);
    }

    if (!fopen_text_file(argv[argn], RDTMODE))
	exit(EXIT_FAILURE);
    if (!fopen_text_file(argv[argn + 1], WRTMODE))
	exit(EXIT_FAILURE);

    while (read_text_tile_info(pixels, ttype, &number, name)) {
	if (match_mode) {
	    if (match_found(name))
		write_text_tile_info(pixels, ttype, number, name);
	}
	else if (!low_res_tile(pixels))
	    write_text_tile_info(pixels, ttype, number, name);
    }
    fclose_text_file();

    exit(EXIT_SUCCESS);
    /*NOTREACHED*/
    return 0;
}

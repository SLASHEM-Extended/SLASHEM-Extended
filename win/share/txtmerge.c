/*   Copyright (c) J. Ali Harlow 2000                               */
/*   NetHack may be freely redistributed.  See license for details. */

/*
 * Merge multiple txt tile files into one. Where a tile is present in more
 * than one file the last tile read is used.
 * The first tile file read must contain the required tile mapping. The
 * output file will contain this same mapping; tiles which do not match
 * any of the tiles found in the first file will be ignored.
 */

/*#include "config.h"*/
#include "hack.h"
#include "tile.h"

extern char *FDECL((strdup), (const char *));

struct tile {
    char *name;
    pixel *bitmap;
    unsigned char ph, source;
};

static struct tile *tiles=NULL;
static int no_tiles=0;
static int alloc_tiles=0;
static pixel file_bg=DEFAULT_BACKGROUND;

static void
set_background(bitmap)
pixel (*bitmap)[MAX_TILE_X];
{
    int x, y;
    const pixel bg = DEFAULT_BACKGROUND;
    if (file_bg.r == bg.r && file_bg.g == bg.g && file_bg.b == bg.b)
	return;
    for(y = 0; y < tile_y; y++)
	for(x = 0; x < tile_x; x++)
	    if (bitmap[y][x].r == file_bg.r &&
	      bitmap[y][x].g == file_bg.g &&
	      bitmap[y][x].b == file_bg.b) {
	    	bitmap[y][x].r = bg.r;
	    	bitmap[y][x].g = bg.g;
	    	bitmap[y][x].b = bg.b;
	    }	    
}

static void
read_tiles()
{
    char ttype[BUFSZ];
    int i, j, number;
    char name[BUFSZ];
    pixel pixels[MAX_TILE_Y][MAX_TILE_X], *p;
    while (read_text_tile_info(pixels, ttype, &number, name)) {
	if (no_tiles == alloc_tiles) {
	    if (alloc_tiles)
		alloc_tiles *= 2;
	    else
		alloc_tiles = 1024;
	    if (!tiles)
		tiles = (struct tile *)malloc(alloc_tiles * sizeof(*tiles));
	    else
		tiles = (struct tile *)realloc(tiles,
		  alloc_tiles * sizeof(*tiles));
	    if (!tiles) {
		Fprintf(stderr, "Not enough memory\n");
		exit(EXIT_FAILURE);
	    }
	    for(i = no_tiles; i < alloc_tiles; i++) {
		tiles[i].bitmap = (pixel *)
		  malloc(tile_x * tile_y * sizeof(pixel));
		if (!tiles[i].bitmap) {
		    Fprintf(stderr, "Not enough memory\n");
		    exit(EXIT_FAILURE);
		}
	    }
	}
	set_background(pixels);
	p = tiles[no_tiles].bitmap;
	for(j = 0; j < tile_y; j++) {
	    memcpy(p, &pixels[j], tile_x * sizeof(pixel));
	    p += tile_x;
	}
	tiles[no_tiles].ph = strcmp(ttype, "placeholder") == 0;
	tiles[no_tiles].source = 1;
	if (!strcmp(name, "null"))
	    tiles[no_tiles].name = NULL;
	else {
	    tiles[no_tiles].name = strdup(name);
	    if (!tiles[no_tiles].name) {
		Fprintf(stderr, "Not enough memory\n");
		exit(EXIT_FAILURE);
	    }
	}
	no_tiles++;
    }
}

static boolean
match(name1, name2)
const char *name1, *name2;
{
    int n;
    char *s1, *s2;
    if (!strcmp(name1, "unknown") || !strcmp(name2, "unknown"))
	return FALSE;
    if (!strcmp(name1, name2))
	return TRUE;
    s1 = strchr(name1, '/');
    s2 = strchr(name2, '/');
    if (s1 && !s2) {
	n = s1 - name1 - 1;
	if (strlen(name2) == n && !strncmp(name1, name2, n))
	    return TRUE;
	else
	    return !strcmp(s1 + 2, name2);
    }
    else if (s2 && !s1) {
	n = s2 - name2 - 1;
	if (strlen(name1) == n && !strncmp(name1, name2, n))
	    return TRUE;
	else
	    return !strcmp(name1, s2 + 2);
    }
    else
	return FALSE;
}

static void
merge_tiles(source)
int source;
{
    char ttype[BUFSZ];
    int number;
    char name[BUFSZ];
    pixel tile[MAX_TILE_Y][MAX_TILE_X], *p;
    int i, j;

    while (read_text_tile_info(tile, ttype, &number, name)) {
	for(i = 0; i < no_tiles; i++)
	    if (tiles[i].source != source && tiles[i].name &&
	      !strcmp(tiles[i].name, name))
		break;
	if (i == no_tiles)
	    for(i = 0; i < no_tiles; i++)
		if (tiles[i].source != source && tiles[i].name &&
		  match(tiles[i].name, name)) {
		    Fprintf(stderr, "warning: replacing tile %s with %s\n",
		      tiles[i].name, name);
		    break;
		}
	if (i != no_tiles) {
	    tiles[i].source = source;
	    set_background(tile);
	    p = tiles[i].bitmap;
	    for(j = 0; j < tile_y; j++) {
		memcpy(p, &tile[j], tile_x * sizeof(pixel));
		p += tile_x;
	    }
	}
	else
	    Fprintf(stderr, "info: tile %s ignored\n",name);
    }
}

static void
write_tiles()
{
    const char *type;
    pixel tile[MAX_TILE_Y][MAX_TILE_X], *p;
    int i, j;

    for(i = 0; i < no_tiles; i++) {
	if (tiles[i].ph)
	    type = "placeholder";
	else
	    type = "tile";

	p = tiles[i].bitmap;
	for(j = 0; j < tile_y; j++) {
	    memcpy(&tile[j], p, tile_x * sizeof(pixel));
	    p += tile_x;
	}
	write_text_tile_info(tile, type, i, tiles[i].name);
    }
}

int
main(argc, argv)
int argc;
char **argv;
{
    int argn = 1, fn;
    char *outfile;
    if (argc > 2 && !strcmp(argv[1], "-p")) {
	if (!read_text_file_colormap(argv[2])) {
	    perror(argv[2]);
	    exit(EXIT_FAILURE);
	}
	init_colormap();
	argn += 2;
    }
    if (argc - argn < 1) {
	Fprintf(stderr,
	  "usage: txtmerge [-p palette-file] outfile [[-b<bg>] infile] ...\n");
	exit(EXIT_FAILURE);
    }
    outfile = argv[argn++];

    for(fn = 1; argn < argc; argn++) {
	if (argv[argn][0] == '-' && argv[argn][1] == 'b') {
	    int r, g, b;
	    pixel bg = DEFAULT_BACKGROUND;
	    if (argv[argn][2]) {
		if (sscanf(argv[argn] + 2, "%02X%02X%02X", &r, &g, &b) != 3) {
		    Fprintf(stderr, "Background %s not understood.\n",
		      argv[argn] + 2);
		}
		else {
		    bg.r = (unsigned char) r;
		    bg.g = (unsigned char) g;
		    bg.b = (unsigned char) b;
		}
	    }
	    file_bg.r = bg.r;
	    file_bg.g = bg.g;
	    file_bg.b = bg.b;
	}
	else {
	    if (!fopen_text_file(argv[argn], RDTMODE))
		exit(EXIT_FAILURE);
	    if (fn == 1)
		read_tiles();
	    else
		merge_tiles(fn);
	    fn++;
	    fclose_text_file();
	}
    }

    if (fn > 1) {
	if (!fopen_text_file(outfile, WRTMODE))
	    exit(EXIT_FAILURE);
	write_tiles();
	fclose_text_file();
    }

    exit(EXIT_SUCCESS);
    /*NOTREACHED*/
    return 0;
}

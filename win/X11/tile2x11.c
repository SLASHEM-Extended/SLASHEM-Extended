/*
 * Convert the given input files into an output file that is expected
 * by nethack.
 * 
 * Assumptions:
 * 	+ Two dimensional byte arrays are in row order and are not padded
 *	  between rows (x11_colormap[][]).
 */
#include "hack.h"		/* for MAX_GLYPH */
#include "tile.h"
#include "tile2x11.h"		/* x11 output file header structure */

#define OUTNAME "x11tiles"	/* Default output file name */
/* #define PRINT_COLORMAP */	/* define to print the colormap */


x11_header	header;
unsigned char	*tile_bytes = NULL;
unsigned char	*curr_tb = NULL;
unsigned char	x11_colormap[MAXCOLORMAPSIZE][3];


/* Look up the given pixel and return its colormap index. */
static unsigned char
pix_to_colormap(pix)
    pixel pix;
{
    int i;

    for (i = 0; i < header.ncolors; i++) {
	if (pix.r == x11_colormap[i][CM_RED] &&
		pix.g == x11_colormap[i][CM_GREEN] &&
		pix.b == x11_colormap[i][CM_BLUE])
	    break;
    }

    if (i == header.ncolors) {
	Fprintf(stderr, "can't find color: [%u,%u,%u]\n", pix.r, pix.g, pix.b);
	exit(1);
    }
    return (unsigned char) (i & 0xFF);
}


/* Convert the tiles in the file to our format of bytes. */
static unsigned long
convert_tiles(tb_ptr)
    unsigned char **tb_ptr;	/* pointer to a tile byte pointer */
{
    unsigned char *tb = *tb_ptr;
    unsigned long count = 0;
    pixel tile[MAX_TILE_Y][MAX_TILE_X];
    int x, y;

    while (read_text_tile(tile)) {
	count++;
	for (y = 0; y < tile_y; y++)
	    for (x = 0; x < tile_x; x++)
		*tb++ = pix_to_colormap(tile[y][x]);
    }

    *tb_ptr = tb;	/* update return val */
    return count;
}


/* Merge the current text colormap (ColorMap) with ours (x11_colormap). */
static void
merge_text_colormap()
{
    int i, j;

    for (i = 0; i < colorsinmap; i++) {
	for (j = 0; j < header.ncolors; j++)
	    if (x11_colormap[j][CM_RED] == ColorMap[CM_RED][i] &&
		    x11_colormap[j][CM_GREEN] == ColorMap[CM_GREEN][i] &&
		    x11_colormap[j][CM_BLUE] == ColorMap[CM_BLUE][i])
		break;

	if (j >= MAXCOLORMAPSIZE) {
	    Fprintf(stderr, "colormap overflow\n");
	    exit(1);
	}

	if (j == header.ncolors) {	/* couldn't find it */
#ifdef PRINT_COLORMAP
	    printf("color %2d: %3d %3d %3d\n", header.ncolors,
	    	ColorMap[CM_RED][i], ColorMap[CM_GREEN][i],
	    	ColorMap[CM_BLUE][i]);
#endif

	    x11_colormap[j][CM_RED] = ColorMap[CM_RED][i];
	    x11_colormap[j][CM_GREEN] = ColorMap[CM_GREEN][i];
	    x11_colormap[j][CM_BLUE] = ColorMap[CM_BLUE][i];
	    header.ncolors++;
	}
    }
}


/* Open the given file, read & merge the colormap, convert the tiles. */
static void
process_file(fname)
    char *fname;
{
    unsigned long count;

    if (!fopen_text_file(fname, RDTMODE)) {
	Fprintf(stderr, "can't open file \"%s\"\n", fname);
	exit(1);
	}

    if (!tile_bytes) {
	/*
	 * Delayed until we open the first input file so that
	 * we know the size of the tiles we are processing.
	 */
	tile_bytes = malloc(tile_x*tile_y*MAX_GLYPH);
	if (!tile_bytes) {
	    Fprintf(stderr, "Not enough memory.\n");
	    exit(1);
	}
	curr_tb = tile_bytes;
    }
    merge_text_colormap();
    count = convert_tiles(&curr_tb);
    Fprintf(stderr, "%s: %lu tiles\n", fname, count);
    header.ntiles += count;
    fclose_text_file();
}


#ifdef USE_XPM
static int
xpm_write(fp)
FILE *fp;
{
    int i,j,x,y;
    char c[3]="?";
    unsigned char *bytes;

    if (header.ncolors > 4096) {
	Fprintf(stderr, "Sorry, only configured for up to 4096 colors\n");
	exit(1);
	/* All you need to do is add more char per color - below */
    }

    /* Fill unused entries with "checkerboard" pattern */
    for(i = TILES_PER_COL * TILES_PER_ROW - header.ntiles; i > 0; i--)
	for (y = 0; y < tile_y; y++)
	    for (x = 0; x < tile_x; x+=2)
	    {
		*curr_tb++ = 0;
		*curr_tb++ = 1;
	    }

    Fprintf(fp, "/* XPM */\n");
    Fprintf(fp, "static char* nhtiles[] = {\n");
    Fprintf(fp, "\"%lu %lu %lu %d\",\n",
		header.tile_width*TILES_PER_ROW,
		header.tile_height*TILES_PER_COL,
		header.ncolors,
		header.ncolors > 64 ? 2 : 1 /* char per color */);
    for (i = 0; i < header.ncolors; i++) {
	if (header.ncolors > 64) {
	    /* two chars per color */
	    c[0] = i / 64 + '0';
	    c[1] = i % 64 + '0';
	}
	else
	    c[0] = i + '0';	/* just one char per color */
	Fprintf(fp, "\"%s  c #%02x%02x%02x\",\n", c,
		x11_colormap[i][0],
		x11_colormap[i][1],
		x11_colormap[i][2]);
    }

    for (j = 0; j < TILES_PER_COL; j++)
	for (y = 0; y < header.tile_height; y++) {
	    bytes=tile_bytes+(j*TILES_PER_ROW*header.tile_height+y)*
	      header.tile_width;
	    Fprintf(fp, "\"");
	    for (i = 0; i < TILES_PER_ROW; i++) {
		for (x = 0; x < header.tile_width; x++) {
		    if (header.ncolors > 64) {
			/* two chars per color */
			c[0] = bytes[x] / 64 + '0';
			c[1] = bytes[x] % 64 + '0';
		    }
		    else
			c[0] = bytes[x] + '0';	/* just one char per color */
		    fputs(c, fp);
		}
		bytes+=header.tile_height*header.tile_width;
	    }
	    Fprintf(fp, "\",\n");
	}

    return fprintf(fp, "};\n")>=0;
}
#endif	/* USE_XPM */

/*
 * ALI
 *
 * Architecture independent tile file so that x11tiles can always be
 * stored in FILE_AREA_SHARE, thus simplifying the configuration.
 */

static boolean
fwrite_tile_header_item(item, fp)
long item;
FILE *fp;
{
    putc((item>>24)&0xff,fp);
    putc((item>>16)&0xff,fp);
    putc((item>>8)&0xff,fp);
    putc(item&0xff,fp);
    return !ferror(fp);
}

static boolean
fwrite_tile_header(header, fp)
x11_header *header;
FILE *fp;
{
    return fwrite_tile_header_item(header->version,fp) &&
      fwrite_tile_header_item(header->ncolors,fp) &&
      fwrite_tile_header_item(header->tile_width,fp) &&
      fwrite_tile_header_item(header->tile_height,fp) &&
      fwrite_tile_header_item(header->ntiles,fp) &&
      fwrite_tile_header_item(header->per_row,fp);
}

int
main(argc, argv)
    int argc;
    char **argv;
{
    FILE *fp;
    int i, argn = 1;
    char *outname = OUTNAME;

    header.version	= 2;		/* version 1 had no per_row field */
    header.ncolors	= 0;
    header.ntiles	= 0;		/* updated as we read in files */
    header.per_row	= 1;

    while (argn < argc) {
	if (argn + 1 < argc && !strcmp(argv[argn], "-o")) {
	    outname = argv[argn + 1];
	    argn += 2;
	}
	else
	    break;
    }

    if (argn == argc) {
	Fprintf(stderr, "usage: %s [-o out_file] txt_file1 [txt_file2 ...]\n",
	  argv[0]);
	exit(1);
    }

    fp = fopen(outname, "w");
    if (!fp) {
	perror(outname);
	exit(1);
	}

    for (i = argn; i < argc; i++)
	process_file(argv[i]);
    Fprintf(stderr, "Total tiles: %ld\n", header.ntiles);

    header.tile_width	= tile_x;
    header.tile_height	= tile_y;

#ifdef USE_XPM
    if (xpm_write(fp) == 0) {
	Fprintf(stderr, "can't write XPM file\n");
	exit(1);
    }
#else
    if (fwrite_tile_header(&header, fp) == 0) {
	Fprintf(stderr, "can't write output header\n");
	exit(1);
	}

    if (fwrite((char *)x11_colormap, 1, header.ncolors*3, fp) == 0) {
	Fprintf(stderr, "can't write output colormap\n");
	exit(1);
    }

    if (fwrite((char *)tile_bytes, 1,
	(int) header.ntiles*header.tile_width*header.tile_height, fp) == 0) {

	Fprintf(stderr, "can't write tile bytes\n");
	exit(1);
    }
#endif

    fclose(fp);
    free(tile_bytes);
    return 0;
}

/*   Copyright (c) NetHack Development Team 1995                    */
/*   NetHack may be freely redistributed.  See license for details. */

/* Create a set of magnified tiles by centering the original tiles */
/* Currently does 32x32 to 48x64 */

#include "config.h"
#include "tile.h"

#ifdef __GO32__
#include <unistd.h>
#endif

static char *tilefiles[] = {	"../win/share/mon32.txt",
				"../win/share/obj32.txt",
				"../win/share/oth32.txt"};

static char *bigfiles[] = {	"../win/share/monbig.txt",
				"../win/share/objbig.txt",
				"../win/share/othbig.txt"};
static int tilecount;
static int tilecount_per_file;
static int filenum;

static int
write_padtile(pixels, ttype, number, name)
pixel (*pixels)[MAX_TILE_X];
const char *ttype;
int number;
const char *name;
{
	int i, j;
	int retval;
	pixel bigpixels[MAX_TILE_Y][MAX_TILE_X];
	int pad_x = 8, pad_y = 16;
	pixel default_background = DEFAULT_BACKGROUND;

	/* Put blanks before */
	/* Pad (new_tile_y - old_tile_y)/2 = (64-32)/2 = 16 */
	for (j = 0; j < pad_y; j++)
		for(i = 0; i < 48; i++)
			bigpixels[j][i] = default_background;
	
	for(j = 0; j < tile_y; j++) {
		/* Put blanks before */
		/* Pad (new_tile_x - old_tile_x)/2 = (48-32)/2 = 8 */
		for(i = 0; i < 8; i++)
		{
			bigpixels[j + pad_y][i] = default_background;
		}

		for(i = 0; i < tile_x; i++)
		{
			bigpixels[j + pad_y][i + pad_x] = pixels[j][i];
		}

		/* Put blanks before */
		/* Pad (new_tile_x - old_tile_x)/2 = (48-32)/2 = 8 */
		for(i = 0; i < pad_x; i++)
		{
			bigpixels[j + pad_y][i+ tile_x + pad_x] = default_background;
		}

	}

	/* Put blanks after */
	/* Pad (new_tile_y - old_tile_y)/2 = (64-32)/2 = 16 */
	for (j = 0; j < pad_y; j++) {
		for(i = 0; i < 48; i++)
		{
			bigpixels[j + tile_y + pad_y][i] = default_background;
		}
	}
	
	tile_x = 48;
	tile_y = 64;
	retval = write_text_tile_info(bigpixels, ttype, number, name);
	tile_x = 32;
	tile_y = 32;
	return retval;
}

static int
write_slanttile(pixels, ttype, number, name)
pixel (*pixels)[MAX_TILE_X];
const char *ttype;
int number;
const char *name;
{
	int i, j;
	int retval;
	pixel bigpixels[MAX_TILE_Y][MAX_TILE_X];
	int pad_x = 16, pad_y = 32;
	pixel default_background = DEFAULT_BACKGROUND;

	/* Put blanks before */
	/* Pad (new_tile_y - old_tile_y) = (64-32) = 32 */
	for (j = 0; j < pad_y; j++)
		for(i = 0; i < 48; i++)
			bigpixels[j][i] = default_background;
	
	for(j = 0; j < tile_y; j++) {
		/* Put blanks before */
		/* Pad (new_tile_x - old_tile_x) = (48-32) = 16 */
		for(i = 0; i < (pad_x - (j/2)); i++)
		{
			bigpixels[j + pad_y][i] = default_background;
		}

		for(i = 0; i < tile_x; i++)
		{
			bigpixels[j + pad_y][i + (pad_x - (j/2))] = pixels[j][i];
		}

		/* Put blanks before */
		/* Pad (new_tile_x - old_tile_x)/2 = (48-32)/2 = 8 */
		for(i = 0; i < (j/2); i++)
		{
			bigpixels[j + pad_y][i + tile_x + (pad_x - j/2)] = default_background;
		}

	}

	tile_x = 48;
	tile_y = 64;
	retval = write_text_tile_info(bigpixels, ttype, number, name);
	tile_x = 32;
	tile_y = 32;
	return retval;
}


int
main(argc, argv)
int argc;
char *argv[];
{
	int tile_no;
	char buf[BUFSZ], ttype[BUFSZ];
	pixel pixels[MAX_TILE_Y][MAX_TILE_X];
	boolean x;
	
	while (filenum < 3) {
		tilecount_per_file = 0;
		if (!fopen_text_file(tilefiles[filenum], RDTMODE))
			exit(EXIT_FAILURE);
#if 0
		if (tile_x * 2 > MAX_TILE_X || tile_y * 2 > MAX_TILE_Y) {
			Fprintf(stderr, "tile size too large\n");
			exit(EXIT_FAILURE);
		}
#endif
		if (!fopen_text_file(bigfiles[filenum], WRTMODE))
			exit(EXIT_FAILURE);
		while (read_text_tile_info(pixels, ttype, &tile_no, buf)) {
			if (filenum < 2)
				write_padtile(pixels, ttype, tile_no, buf);
			else 
				write_slanttile(pixels, ttype, tile_no, buf);
			tilecount_per_file++;
			tilecount++;
		}
		fclose_text_file();
		printf("%d tiles processed from %s\n",
			tilecount_per_file, tilefiles[filenum]);
		++filenum;
	}
	printf("Grand total of %d tiles processed.\n", tilecount);
	exit(EXIT_SUCCESS);
	/*NOTREACHED*/
	return 0;
}

/*magtile.c*/

/*   Copyright (c) NetHack Development Team 1995                    */
/*   NetHack may be freely redistributed.  See license for details. */

/* Create a set of magnified tiles by duplicating pixels in original */

#include "config.h"
#include "tile.h"

#ifdef __GO32__
#include <unistd.h>
#endif

static char *tilefiles[] = {	"../win/share/monsters.txt",
				"../win/share/objects.txt",
				"../win/share/other.txt"};

static char *magfiles[] = {	"../win/share/monmag.txt",
				"../win/share/objmag.txt",
				"../win/share/othmag.txt"};
static int tilecount;
static int tilecount_per_file;
static int filenum;

static int
write_magtile(pixels, ttype, number, name)
pixel (*pixels)[MAX_TILE_X];
const char *ttype;
int number;
const char *name;
{
	int i, j;
	int retval;
	pixel bigpixels[MAX_TILE_Y][MAX_TILE_X];

	for(j = 0; j < tile_y; j++)
		for(i = 0; i < tile_x; i++)
		{
			bigpixels[2 * j][2 * i] = pixels[j][i];
			bigpixels[2 * j + 1][2 * i] = pixels[j][i];
			bigpixels[2 * j][2 * i + 1] = pixels[j][i];
			bigpixels[2 * j + 1][2 * i + 1] = pixels[j][i];
		}
	tile_x *= 2;
	tile_y *= 2;
	retval = write_text_tile_info(bigpixels, ttype, number, name);
	tile_x /= 2;
	tile_y /= 2;
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
		if (tile_x * 2 > MAX_TILE_X || tile_y * 2 > MAX_TILE_Y) {
			Fprintf(stderr, "tile size too large\n");
			exit(EXIT_FAILURE);
		}
		if (!fopen_text_file(magfiles[filenum], WRTMODE))
			exit(EXIT_FAILURE);
		while (read_text_tile_info(pixels, ttype, &tile_no, buf)) {
			write_magtile(pixels, ttype, tile_no, buf);
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

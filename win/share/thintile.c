/*   SCCS Id: @(#)thintile.c   3.4     1995/11/26                     */
/*   Copyright (c) NetHack Development Team 1995                    */
/*   NetHack may be freely redistributed.  See license for details. */

/* Create a set of overview tiles by eliminating even pixels in original */

#include "config.h"
#include "tile.h"

#ifdef __GO32__
#include <unistd.h>
#endif

static char *tilefiles[] = {	"../win/share/monsters.txt",
				"../win/share/objects.txt",
				"../win/share/other.txt"};

static char *thinfiles[] = {	"../win/share/monthin.txt",
				"../win/share/objthin.txt",
				"../win/share/oththin.txt"};
static int tilecount;
static int tilecount_per_file;
static int filenum;

static int
write_thintile(pixels, ttype, number, name)
pixel (*pixels)[MAX_TILE_X];
const char *ttype;
int number;
const char *name;
{
	int i, j;
	int retval;
	pixel thinpixels[MAX_TILE_Y][MAX_TILE_X];

	for (j = 0; j < tile_y; j++)
		for (i = 0; i < tile_x; i+=2) {
			thinpixels[j][i/2] = pixels[j][i];
		}
	
	tile_x /= 2;
	retval = write_text_tile_info(thinpixels, ttype, number, name);
	tile_x *= 2;
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

	while (filenum < 3) {
		tilecount_per_file = 0;
		if (!fopen_text_file(tilefiles[filenum], RDTMODE))
			exit(EXIT_FAILURE);
		if (!fopen_text_file(thinfiles[filenum], WRTMODE))
			exit(EXIT_FAILURE);
		while (read_text_tile_info(pixels, ttype, &tile_no, buf)) {
			write_thintile(pixels, ttype, tile_no, buf);
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

/*thintile.c*/

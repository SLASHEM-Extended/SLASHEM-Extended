/*   Copyright (c) NetHack Development Team 1995                    */
/*   NetHack may be freely redistributed.  See license for details. */

/* Create a set of magnified tiles by duplicating pixels in original */

#include "config.h"
#include "tile.h"

#ifdef __GO32__
#include <unistd.h>
#endif

#define SCALE2X

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
#ifdef SCALE2X
			/* Scale2X algorithm from AdvanceMAME
			 * http://advancemame.sourceforge.net/scale2x.html
			 *
			 * Pixel E from a source image
			 *
			 *       B 
			 *     D E F
			 *       H 
			 *
			 * is magnified to a set of 4 destination pixels
			 *
			 *     E0 E1
			 *     E2 E3
			 *
			 * by the following rules
			 *
			 * E0 = D == B && B != F && D != H ? D : E;
			 * E1 = B == F && B != D && F != H ? F : E;
			 * E2 = D == H && D != B && H != F ? D : E;
			 * E3 = H == F && D != H && B != F ? F : E;
			 *
			 */
			pixel       pixB;
			pixel pixD, pixE, pixF;
			pixel       pixH;

			pixE = pixels[j][i];
			pixB = ( (j == 0) ? pixE : pixels[j-1][i]);
			pixD = ( (i == 0) ? pixE : pixels[j][i-1]);
			pixF = ( (i == (tile_x - 1)) ? pixE : pixels[j][i+1]);
			pixH = ( (j == (tile_y - 1)) ? pixE : pixels[j+1][i]);
			
			bigpixels[2 * j][2 * i] = ((pixel_equal(pixD,pixB) && !pixel_equal(pixB,pixF) && !pixel_equal(pixD,pixH)) ? pixD : pixE);
			bigpixels[2 * j][2 * i + 1] = ((pixel_equal(pixB,pixF) && !pixel_equal(pixB,pixD) && !pixel_equal(pixF,pixH)) ? pixF : pixE);
			bigpixels[2 * j + 1][2 * i] = ((pixel_equal(pixD,pixH) && !pixel_equal(pixD,pixB) && !pixel_equal(pixH,pixF)) ? pixD : pixE);
			bigpixels[2 * j + 1][2 * i + 1] = ((pixel_equal(pixH,pixF) && !pixel_equal(pixD,pixH) && !pixel_equal(pixB,pixF)) ? pixF : pixE);
#else
			bigpixels[2 * j][2 * i] = pixels[j][i];
			bigpixels[2 * j + 1][2 * i] = pixels[j][i];
			bigpixels[2 * j][2 * i + 1] = pixels[j][i];
			bigpixels[2 * j + 1][2 * i + 1] = pixels[j][i];
#endif
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

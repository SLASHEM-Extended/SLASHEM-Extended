/*   Copyright (c) NetHack Development Team 1995                    */
/*   NetHack may be freely redistributed.  See license for details. */

/*
 * Create a set of pseudo 3D tiles by centering monsters/objects/furniture
 * and slanting floors. See Mitsuhiro Itakura's instructions at:
 * http://www.geocities.co.jp/SiliconValley-SanJose/9606/nh/3d/index.html
 */

#include "config.h"
#include "tile.h"

#ifdef __GO32__
#include <unistd.h>
#endif

static char *tilefiles[] = {	"../win/share/mon32-t.txt",
				"../win/share/obj32-t.txt",
				"../win/share/oth32-t.txt"};

static char *bigfiles[] = {	"../win/share/monbig.txt",
				"../win/share/objbig.txt",
				"../win/share/othbig.txt"};
static char *no_slant[] = {
	"iron bars",
	"tree",
	"staircase up",
	"staircase down",
	"ladder up",
	"ladder down",
	"altar",
	"opulent throne",
	"sink",
	"toilet",
	"fountain",
	"squeaky board",
	"bear trap",
	"land mine",
	"rust trap",
	"fire trap",
	"magic portal",
	"grave",
	"cmap / thrown boomerang, open left",
	"cmap / thrown boomerang, open right",
	"cmap / magic shield 1",
	"cmap / magic shield 2",
	"cmap / magic shield 3",
	"cmap / magic shield 4",
	"warning 0",
	"warning 1",
	"warning 2",
	"warning 3",
	"warning 4",
	"warning 5",
	};

static char *raised_up[] = {  /* these matches are partial */
	"explosion",
	"zap",
	"cmap / dig beam",
	"cmap / camera flash",
	"cmap / swallow",
	};

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
	pixel default_background = DEFAULT_BACKGROUND;

	for(j = 0; j < 2 * tile_y; j++)
		for(i = 0; i < 3 * tile_x / 2; i++)
			bigpixels[j][i] = default_background;
	for(j = 0; j < tile_y; j++)
		for(i = 0; i < tile_x; i++)
			bigpixels[j + tile_y / 2][i + tile_x / 4] =
			  pixels[j][i];
	i = tile_x;
	j = tile_y;
	tile_x = tile_x * 3 / 2;
	tile_y *= 2;
	retval = write_text_tile_info(bigpixels, ttype, number, name);
	tile_x = i;
	tile_y = j;
	return retval;
}

static int
write_slanttile(pixels, ttype, number, name, yoffset)
pixel (*pixels)[MAX_TILE_X];
const char *ttype;
int number;
const char *name;
int yoffset;
{
	int i, j;
	int retval;
	pixel bigpixels[MAX_TILE_Y][MAX_TILE_X];
	pixel default_background = DEFAULT_BACKGROUND;

	for(j = 0; j < 2 * tile_y; j++)
		for(i = 0; i < 3 * tile_x / 2; i++)
			bigpixels[j][i] = default_background;
	for(j = 0; j < tile_y; j++)
		for(i = 0; i < tile_x; i++)
			bigpixels[j + tile_y - yoffset][i + tile_x / 2 - j / 2] =
			  pixels[j][i];
	i = tile_x;
	j = tile_y;
	tile_x = tile_x * 3 / 2;
	tile_y *= 2;
	retval = write_text_tile_info(bigpixels, ttype, number, name);
	tile_x = i;
	tile_y = j;
	return retval;
}

int
main(argc, argv)
int argc;
char *argv[];
{
	int i, tile_no;
	char buf[BUFSZ], ttype[BUFSZ];
	pixel pixels[MAX_TILE_Y][MAX_TILE_X];
	boolean x;
	
	while (filenum < 3) {
		tilecount_per_file = 0;
		if (!fopen_text_file(tilefiles[filenum], RDTMODE))
			exit(EXIT_FAILURE);
		if (tile_x * 3 / 2 > MAX_TILE_X || tile_y * 2 > MAX_TILE_Y) {
			Fprintf(stderr, "tile size too large\n");
			exit(EXIT_FAILURE);
		}
		if (!fopen_text_file(bigfiles[filenum], WRTMODE))
			exit(EXIT_FAILURE);
		while (read_text_tile_info(pixels, ttype, &tile_no, buf)) {
			if (filenum < 2)
				write_padtile(pixels, ttype, tile_no, buf);
			else 
			{
				for(i = 0; i < SIZE(no_slant); i++)
					if (!strcmp(buf, no_slant[i]))
						break;
				if (i < SIZE(no_slant))
					write_padtile(pixels, ttype, tile_no, buf);
				else
				{
					int yoffset = 0;

					for (i = 0; i < SIZE(raised_up); i++)
						if (!strncmp(buf, raised_up[i], strlen(raised_up[i])))
							break;

					if (i < SIZE(raised_up))
						yoffset = 14;

					write_slanttile(pixels, ttype, tile_no, buf, yoffset);
				}
			}
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

/*bigtile.c*/

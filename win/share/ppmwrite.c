/* this produces a raw ppm file, with a 15-character header of
 * "P6 4-digit-width 4-digit-height 255\n"
 */

#include "config.h"
#include "tile.h"

#ifndef MONITOR_HEAP
extern long *FDECL(alloc, (unsigned int));
#endif

FILE *ppm_file;

struct ppmscreen {
	int	Width;
	int	Height;
} PpmScreen;

static int tiles_across, tiles_down, curr_tiles_across;
static pixel **image;

static int NDECL(write_header);
static void NDECL(WriteTileStrip);

static int
write_header()
{
	if (PpmScreen.Width > 9999 || PpmScreen.Height > 9999) {
		/* Just increase the number of digits written to solve */
		Fprintf(stderr, "PPM dimensions too large\n");
		return FALSE;
	}
	(void) fprintf(ppm_file, "P6 %04d %04d 255\n",
				PpmScreen.Width, PpmScreen.Height);
	return TRUE;
}

static void
WriteTileStrip()
{
	int i, j;

	for (j = 0; j < tile_y; j++) {
		for (i = 0; i < PpmScreen.Width; i++) {
			(void) fputc((char)image[j][i].r, ppm_file);
			(void) fputc((char)image[j][i].g, ppm_file);
			(void) fputc((char)image[j][i].b, ppm_file);
		}
	}
}

boolean
fopen_ppm_file(filename, type)
const char *filename;
const char *type;
{
	int i;

	if (strcmp(type, WRBMODE)) {
		Fprintf(stderr, "using writing routine for non-writing?\n");
		return FALSE;
	}

        if (ppm_file != (FILE*)0) return TRUE;
         	
	ppm_file = fopen(filename, type);
	if (ppm_file == (FILE *)0) {
		Fprintf(stderr, "cannot open ppm file %s\n", filename);
		return FALSE;
	}

	if (!colorsinmainmap) {
		Fprintf(stderr, "no colormap set yet\n");
		return FALSE;
	}

	curr_tiles_across = 0;
	PpmScreen.Width = tiles_across * tile_x;

	tiles_down = 0;
	PpmScreen.Height = 0;	/* will be rewritten later */

	if (!write_header())
		return FALSE;

	image = (pixel **)alloc(tile_y * sizeof(pixel *));
	for (i = 0; i < tile_y; i++) {
		image[i] = (pixel *) alloc(PpmScreen.Width * sizeof(pixel));
	}

	return TRUE;
}

boolean
write_ppm_tile(pixels)
pixel (*pixels)[MAX_TILE_X];
{
	int i, j;

	for (j = 0; j < tile_y; j++) {
		for (i = 0; i < tile_x; i++) {
			image[j][curr_tiles_across*tile_x + i] = pixels[j][i];
		}
	}
	curr_tiles_across++;
	if (curr_tiles_across == tiles_across) {
		WriteTileStrip();
		curr_tiles_across = 0;
		tiles_down++;
	}
	return TRUE;
}

int
fclose_ppm_file()
{
	int i, j;

	if (curr_tiles_across) {	/* partial row */
		/* fill with checkerboard, for lack of a better idea */
		for (j = 0; j < tile_y; j++) {
			for (i = curr_tiles_across * tile_x;
						i < PpmScreen.Width; i += 2 ) {
				image[j][i].r = MainColorMap[CM_RED][0];
				image[j][i].g = MainColorMap[CM_GREEN][0];
				image[j][i].b = MainColorMap[CM_BLUE][0];
				image[j][i+1].r = MainColorMap[CM_RED][1];
				image[j][i+1].g = MainColorMap[CM_GREEN][1];
				image[j][i+1].b = MainColorMap[CM_BLUE][1];
			}
		}
		WriteTileStrip();
		curr_tiles_across = 0;
		tiles_down++;
	}

	for (i = 0; i < tile_y; i++) {
		free((genericptr_t)image[i]);
	}
	free((genericptr_t)image);

	PpmScreen.Height = tiles_down * tile_y;
	rewind(ppm_file);
	if (!write_header())	/* update size */
		return -1;

	return(fclose(ppm_file));
}


int
main(argc, argv)
int argc;
char *argv[];
{
        int fileargs=1;
        char *ppmfile;

	pixel pixels[MAX_TILE_Y][MAX_TILE_X];

        tiles_across = 20;

	if (argc > 1 && !strcmp(argv[1],"-w")) {
		tiles_across=atoi(argv[2]);
		fileargs+=2;
	}

	if (argc-fileargs < 2) {
		Fprintf(stderr,
		  "usage: txt2ppm [-w tiles-across] ppmfile txtfile ... \n");
		exit(EXIT_FAILURE);
	}

	ppmfile=argv[fileargs++];

	while (fileargs < argc) {
	
		if (!fopen_text_file(argv[fileargs++], RDTMODE))
			exit(EXIT_FAILURE);
          
		init_colormap();
		if (!fopen_ppm_file(ppmfile, WRBMODE)) {
			(void) fclose_text_file();
			exit(EXIT_FAILURE);
		}

		while (read_text_tile(pixels))
			(void) write_ppm_tile(pixels);

		(void) fclose_text_file();
        }
        
	if (fclose_ppm_file())
		exit(EXIT_FAILURE);
	else
		exit(EXIT_SUCCESS);
	/*NOTREACHED*/
	return 0;
}

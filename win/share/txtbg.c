/*   Copyright (c) J. Ali Harlow 2000                               */
/*   NetHack may be freely redistributed.  See license for details. */

/*
 * ALI
 *
 * Overlay transparent tiles on various backgrounds.
 *
 * This is a bit of a kludge. It would be much better if all window
 * ports would use the back_to_glyph() function. As it is, we
 * compromise. Monsters and objects are left transparent. This means
 * that they display correctly overlaid on the background in the GTK
 * port, but appear with a solid surround in the Qt and X11 ports.
 * Traps and furniture have backgrounds set. This means that they
 * look almost acceptable in the Qt, X11 and Gnome ports, but that
 * dark rooms etc. cause odd effects in the GTK port (the floor under
 * traps and furniture remains lit).
 *
 * I don't attempt to justify these compromises; they are based on what
 * the old tile system used to do. The intention is to upgrade the Qt, X11
 * and Gnome ports to handle transparancy and make this program obsolete.
 * Presumably the Allegro port would also need attention to judge from
 * Kelly's comment on http://www.pinn.net/~jry/allegrohack/tileedit.htm:
 *     "Making areas of wall/floor/trap/furniture tiles transparent
 *      will result in HOM effects."
 * I am not able to test this for myself.
 * Note that the GTK port will also need quite a bit of work before all
 * its transparency problems are sorted.
 *
 * Warning: textbg uses the numbers in the files and not the position.
 */

#include "hack.h"
#include "tile.h"

#define MON_GLYPH 1
#define OBJ_GLYPH 2
#define OTH_GLYPH 3

#define eos(str)		((str) + strlen(str))
#define pixel_eq(pixa, pixb)	((pixa).r == (pixb).r && (pixa).g == (pixb).g \
					 	  && (pixa).b == (pixb).b)
#define pixel_set(pixa, pixb)	if (1) \
  {(pixa).r = (pixb).r; (pixa).g = (pixb).g; (pixa).b = (pixb).b; } else

struct backgrounds {
    int sequence;
    int first;
    int last;		/* or -1 for end of file */
    int bg_sequence;	/* Currently must be the same as sequence */
    int bg;
    pixel pixels[MAX_TILE_Y][MAX_TILE_X];
} backgrounds[] = {
    /* Furniture normally found in rooms */
    { OTH_GLYPH, S_ndoor, S_pool, OTH_GLYPH, S_room },
    /* Furniture normally outside */
    { OTH_GLYPH, S_ice, S_water, OTH_GLYPH, S_corr },
    /* Traps are normally (always?) found in rooms */
    { OTH_GLYPH, S_arrow_trap, S_polymorph_trap, OTH_GLYPH, S_room },
    /* Substitute walls */
    { OTH_GLYPH, MAX_GLYPH - GLYPH_CMAP_OFF + 1, -1, OTH_GLYPH, S_corr },
};

void
do_background(tile, bg)
pixel tile[MAX_TILE_Y][MAX_TILE_X];
pixel bg[MAX_TILE_Y][MAX_TILE_X];
{
    int i, j;
    pixel def_background = DEFAULT_BACKGROUND;

    for(j = 0; j < tile_y; j++)
	for(i = 0; i < tile_x; i++)
	    if (pixel_eq(tile[j][i], def_background))
		pixel_set(tile[j][i], bg[j][i]);
}

int
main(argc, argv)
int argc;
char **argv;
{
    int i, seq=0;
    char ttype[BUFSZ];
    int number;
    char name[BUFSZ];
    pixel pixels[MAX_TILE_Y][MAX_TILE_X];

    while (argc > 1 && argv[1][0] == '-') {
	if (argv[1][1] == 's') {
	    seq = atoi(argv[1] + 2);
	    if (seq < MON_GLYPH || seq > OTH_GLYPH) {
		Fprintf(stderr, "txtbg: Illegal sequence number %d\n", seq);
		exit(EXIT_FAILURE);
	    }
	}
    }

    if (argc != 3) {
	Fprintf(stderr, "usage: txtbg [-s<seq>] infile outfile\n");
	exit(EXIT_FAILURE);
    }

    if (!seq) {
	char *s;
	for(s = eos(argv[1]); s > argv[1] && *s != '/' && *s != '\\'; s--)
	    ;
	if (s > argv[1])
	    s++;
	if (!strncmp(s, "mon", 3))
	    seq = MON_GLYPH;
	else if (!strncmp(s, "obj", 3))
	    seq = OBJ_GLYPH;
	else if (!strncmp(s, "oth", 3))
	    seq = OTH_GLYPH;
    }

    if (!seq) {
	Fprintf(stderr,
	  "txtbg: Unable to determine sequence number (use -s)\n");
	exit(EXIT_FAILURE);
    }

    for(i = 0; i < SIZE(backgrounds); i++)
	if (backgrounds[i].bg_sequence == seq)
	    break;
    if (i < SIZE(backgrounds)) {
	/* At least one background needed from this file - read them in */
	if (!fopen_text_file(argv[1], RDTMODE))
	    exit(EXIT_FAILURE);
	while (read_text_tile_info(pixels, ttype, &number, name)) {
	    for(i = 0; i < SIZE(backgrounds); i++)
		if (seq == backgrounds[i].bg_sequence &&
		  number == backgrounds[i].bg)
		    memcpy(backgrounds[i].pixels, pixels, sizeof(pixels));
	}
	fclose_text_file();
    }

    if (!fopen_text_file(argv[1], RDTMODE))
	exit(EXIT_FAILURE);
    if (!fopen_text_file(argv[2], WRTMODE))
	exit(EXIT_FAILURE);

    while (read_text_tile_info(pixels, ttype, &number, name)) {
	for(i = 0; i < SIZE(backgrounds); i++)
	    if (seq == backgrounds[i].sequence && (backgrounds[i].first < 0
	      || number >= backgrounds[i].first) && (backgrounds[i].last < 0
	      || number <= backgrounds[i].last))
		do_background(pixels, backgrounds[i].pixels);
	write_text_tile_info(pixels, ttype, number, name);
    }
    fclose_text_file();

    exit(EXIT_SUCCESS);
    /*NOTREACHED*/
    return 0;
}

/*   SCCS Id: @(#)idx2bmp.c   3.3     95/01/26                     */
/*   Copyright (c) NetHack PC Development Team 1993, 1994, 1995     */
/*   NetHack may be freely redistributed.  See license for details. */

/*
 * This creates an idx2bmp.exe [indexfile [outputfile]]
 *
 * This takes an index file (index or indexfile)
 * (generated via txt2bmp.exe -i  or otherwise)
 * and uses the same parsing algorithm as in the alleg_init().
 * The bmp files referenced by the index file are then taken and put into
 * the output file (tiles.bmp or outputfile)
 *
 * Edit History:
 *
 *      Initial Creation                        W.Cheung  00/06/23
 *
 */

#define alleg_mouse_unused
#define alleg_timer_unused
#define alleg_keyboard_unused
#define alleg_joystick_unused
#define alleg_sound_unused
#define alleg_gui_unused

#include <allegro.h>

#include "hack.h"
#include "pcvideo.h"
#include "tile.h"
#include "pctiles.h"

#include <ctype.h>
#include <dos.h>
#ifndef MONITOR_HEAP
#include <stdlib.h>
#endif
#include <time.h>

extern char *FDECL(tilename, (int, int));

#define Fprintf (void) fprintf
#define Fclose  (void) fclose


static const char
    *output_file = "tiles.bmp",
    *index_file = "index";

/* These next two functions were stolen from hacklib.c */

char *
eos(s)			/* return the end of a string (pointing at '\0') */
    register char *s;
{
    while (*s) s++;	/* s += strlen(s); */
    return s;
}

/* remove excess whitespace from a string buffer (in place) */
char *
mungspaces(bp)
char *bp;
{
    register char c, *p, *p2;
    boolean was_space = TRUE;

    for (p = p2 = bp; (c = *p) != '\0'; p++) {
	if (c == '\t') c = ' ';
	if (c != ' ' || !was_space) *p2++ = c;
	was_space = (c == ' ');
    }
    if (was_space && p2 > bp) p2--;
    *p2 = '\0';
    return bp;
}

int
main(argc, argv)
int argc;
char *argv[];
{
        int i;
        FILE *fp;
        char            buf[BUFSZ];
        char            *bufp;
        BITMAP *bigtile_bmp = (BITMAP *)0;
        BITMAP *tilebmp;
        struct tm *newtime;
        time_t aclock;
        char filename[60];
        int tile_x = 32, tile_y = 32;
        int col, row;
        boolean has_index = 0, has_output = 0;

        if (argc > 3) {        	
	    	Fprintf(stderr, "Bad arg count (%d).\n", argc-1);
	    	(void) fflush(stderr);
                exit(EXIT_FAILURE);
        }
        has_index = (argc > 1);
        has_output = (argc > 2);
        
        set_color_depth(24);

        time(&aclock);
        newtime = localtime(&aclock);
             
        /* Open the index file */
        
	if (has_index) sprintf(filename, argv[1]);
	else sprintf(filename, index_file);
        if ((fp = fopen(filename, "r")) == (FILE *)0)
        {
                Fprintf(stderr, "Could not open index file '%s'!\n", filename);
	        exit(EXIT_FAILURE);
        }
              
        i = 0;

        while(fgets(buf,120,fp))
        {
                if (*buf == '#')
                        continue;

                bufp = eos(buf);
                while (--bufp > buf && isspace(*bufp))
                        continue;

                if (bufp <= buf)
                        continue;               /* skip all-blank lines */
                else
                        *(bufp + 1) = '\0';     /* terminate line */

                /* find the '=' or ':' */
                bufp = index(buf, ':');
                if (!bufp)
                	continue;
                	
		*bufp = '\0';
		
                bufp++; /* we only want what's after the ':' */

                /* skip  whitespace between '=' and value */
                do { ++bufp; } while (isspace(*bufp));

                bufp = mungspaces(bufp);

                sprintf(filename, "%s", bufp);
                tilebmp = load_bitmap(filename, (RGB *)0);

                if(!tilebmp) {
                	Fprintf(stderr, "Could not open file '%s', continuing.\n", filename);
                	continue;
                }
		
                if (!bigtile_bmp) {
                	tile_x = tilebmp->w;
                	tile_y = tilebmp->h;
                	
	        	bigtile_bmp = create_bitmap(tile_x * TILES_PER_ROW, tile_y * TILES_PER_COL);
                }
                
	    	col = (int)(i % TILES_PER_ROW);
		row = (int)(i / TILES_PER_ROW);
#ifdef DEBUG
		Fprintf(stderr, "col: %i row: %i\n", col, row);
#endif
		blit(tilebmp, bigtile_bmp, 0, 0, col * tile_x, row * tile_y, tile_x, tile_y);
	                
	                i++;
        }


	if (has_output) sprintf(filename, argv[2]);
	else sprintf(filename, output_file);
      	if (save_bitmap(filename, bigtile_bmp, 0)) {
                Fprintf(stderr, "Could not save bitmap '%s'!\n", filename);
	        exit(EXIT_FAILURE);
      	}

        exit(EXIT_SUCCESS);
        /*NOTREACHED*/
        return 0;
}


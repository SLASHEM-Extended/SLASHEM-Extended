/* $Id: mapmerge.c,v 1.1 2002-09-01 21:58:19 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

int
main(argc, argv)
int argc;
char **argv;
{
    FILE *fpi, *fpo;
    int i, j, tile_number = 0;
    char buf[BUFSZ], buf2[BUFSZ];
    if (argc < 2) {
	(void) fprintf(stderr,"%s", "usage: mapmerge outfile [infile] ...\n");
	exit(EXIT_FAILURE);
    }
    fpo = fopen(argv[1], "w");
    if (!fpo) {
	perror(argv[1]);
	exit(EXIT_FAILURE);
    }
    i = -1;
    for(j = 2; j < argc; j++) {
	fpi = fopen(argv[j], "r");
	if (!fpi) {
	    perror(argv[j]);
	    fclose(fpo);
	    remove(argv[1]);
	    exit(EXIT_FAILURE);
	}
	while (fgets(buf, sizeof(buf), fpi)) {
	    if (sscanf(buf, "tile %*d %[^\n]", buf2) == 1)
		fprintf(fpo, "tile %d %s\n", tile_number++, buf2);
	    else
		fputs(buf, fpo);
	}
	fclose(fpi);
    }
    fclose(fpo);
    exit(EXIT_SUCCESS);
}

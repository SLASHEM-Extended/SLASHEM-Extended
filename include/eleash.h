/* $Id: eleash.h,v 1.1 2005-06-05 16:12:21 j_ali Exp $ */
/* Copyright (c) J. Ali Harlow, 2005.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef ELEASH_H
#define ELEASH_H

struct eleash {
    xchar pathlen;		/* Number of squares of extended leash
				   (2 means hero and pet are adjacent) */
    xchar pathd2;		/* Length of path from hero to pet along
				   leash, squared. */
#define ELEASH_PSZ	6	/* check_leash_pos() assumes at least 2 */
    coord path[ELEASH_PSZ+1];	/* Positions containing extended leash,
				   from hero to pet (inclusive) */
};

#define ELEASH(obj)	((struct eleash *)&(obj)->oextra[0])

#endif /* ELEASH_H */

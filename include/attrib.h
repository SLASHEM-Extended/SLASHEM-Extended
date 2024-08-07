/*	SCCS Id: @(#)attrib.h	3.4	1990/22/02	*/
/* Copyright 1988, Mike Stephenson				  */
/* NetHack may be freely redistributed.  See license for details. */

/*	attrib.h - Header file for character class processing. */

#ifndef ATTRIB_H
#define ATTRIB_H

#define A_STR	0
#define A_INT	1
#define A_WIS	2
#define A_DEX	3
#define A_CON	4
#define A_CHA	5

#define A_MAX	6	/* used in rn2() selection of attrib */

#define ABASE(x)	(u.acurr.a[x])
#define ASTART(x)	((x == A_STR) ? u.startstr : (x == A_DEX) ? u.startdex : (x == A_INT) ? u.startint : (x == A_WIS) ? u.startwis : (x == A_CON) ? u.startcon : (x == A_CHA) ? u.startcha : 0)
#define ABON(x)		(u.abon.a[x])
#define AEXE(x)		(u.aexe.a[x])
#define ACURR(x)	(acurr(x))
#define ACURRSTR	(acurrstr())
/* should be: */
/* #define ACURR(x) (ABON(x) + ATEMP(x) + (Upolyd  ? MBASE(x) : ABASE(x)) */
#define MCURR(x)	(u.macurr.a[x])
#define AMAX(x)		(u.amax.a[x])
#define MMAX(x)		(u.mamax.a[x])

#define ATEMP(x)	(u.atemp.a[x])
#define ATIME(x)	(u.atime.a[x])

/* KMH -- Conveniences when dealing with strength constants */
#define STR18(x)	(18+(x))	/* 18/xx */
#define STR19(x)	(100+(x))	/* For 19 and above */


struct	attribs {
	schar	a[A_MAX];
};

#define ATTRMAX(x) (urace.attrmax[x])
#define ATTRMIN(x) (urace.attrmin[x])
#define ATTRABSMIN(x) (1)

#endif /* ATTRIB.H */

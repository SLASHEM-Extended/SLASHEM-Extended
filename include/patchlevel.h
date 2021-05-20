/*	SCCS Id: @(#)patchlevel.h	3.4	2003/12/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*The name of the compiled game- should be same as stuff in makefile*/
/*for makedefs*/
/* KMH -- Made it mixed case, from which upper & lower case versions are made */
#define DEF_GAME_NAME   "SlashEMExtended"
/*#define DEF_GAME_NAME   "NetHack"*/

/* Version */
#define VERSION_MAJOR   2
#define VERSION_MINOR   7
/*
 * PATCHLEVEL is updated for each release.
 */
#define PATCHLEVEL      1
/*
#define EDITLEVEL	0
#define FIXLEVEL        0
*/

#define COPYRIGHT_BANNER_A \
"This is Sadistic Levels of Endless X-Citement (SLEX) 1997-2018 by Amy"

#define COPYRIGHT_BANNER_B \
"NetHack, Copyright 1985-2003 Stichting Mathematisch Centrum, M. Stephenson."

#define COPYRIGHT_BANNER_C \
"Contact the dev team via the #em.slashem.me channel on Libera IRC."

#define COPYRIGHT_BANNER_D \
"See license for details. Bug reports can also be posted on Nethackwiki."

#if 0
/*
 * If two or more successive releases have compatible data files, define
 * this with the version number of the oldest such release so that the
 * new release will accept old save and bones files.  The format is
 *	0xMMmmPPeeL
 * 0x = literal prefix "0x", MM = major version, mm = minor version,
 * PP = patch level, ee = edit level, L = literal suffix "L",
 * with all four numbers specified as two hexadecimal digits.
 */
#define VERSION_COMPATIBILITY 0x00000000L
#endif

/*patchlevel.h*/

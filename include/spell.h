/*	SCCS Id: @(#)spell.h	3.4	1995/06/01	*/
/* Copyright 1986, M. Stephenson				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef SPELL_H
#define SPELL_H

struct spell {
    short	sp_id;			/* spell id (== object.otyp) */
    xchar	sp_lev;			/* power level */
    int		sp_know;		/* knowlege of spell */
};

/* levels of memory destruction with a scroll of amnesia */
#define ALL_MAP		0x1
#define ALL_SPELLS	0x2

#define decrnknow(spell)	spl_book[spell].sp_know--
#define spellid(spell)		spl_book[spell].sp_id
#define spellknow(spell)	spl_book[spell].sp_know

/* Zap types, also used for explosions */
#define ZT_MAGIC_MISSILE        (AD_MAGM-1)
#define ZT_FIRE                 (AD_FIRE-1)
#define ZT_COLD                 (AD_COLD-1)
#define ZT_SLEEP                (AD_SLEE-1)
#define ZT_DEATH                (AD_DISN-1)     /* or disintegration */
#define ZT_LIGHTNING            (AD_ELEC-1)
#define ZT_POISON_GAS           (AD_DRST-1)
#define ZT_ACID                 (AD_ACID-1)

#define ZT_LAST                 (ZT_ACID) /*For checking of spells of a type*/
#define ZT_FIRST                (ZT_MAGIC_MISSILE)
/* 8 and 9 are currently unassigned */

#define ZT_WAND(x)              (x)
#define ZT_SPELL(x)             (10+(x))
#define ZT_BREATH(x)    (20+(x))
#define ZT_MEGA(x)      (30+(x))
#define ZT_MONWAND(x)   (-(30+(x)))

#endif /* SPELL_H */

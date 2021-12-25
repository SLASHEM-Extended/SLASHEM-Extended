/*	SCCS Id: @(#)mkroom.h	3.4	1992/11/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MKROOM_H
#define MKROOM_H

/* mkroom.h - types and structures for room and shop initialization */

struct mkroom {
	schar lx,hx,ly,hy;	/* usually xchar, but hx may be -1 */
	schar rtype;		/* type of room (zoo, throne, etc...) */
	schar rlit;			/* is the room lit ? */
	schar doorct;		/* door count */
	schar fdoor;		/* index for the first door of the room */
	schar nsubrooms;	/* number of subrooms */
	boolean irregular;	/* true if room is non-rectangular */
	struct mkroom *sbrooms[MAX_SUBROOMS];  /* Subrooms pointers */
	struct monst *resident; /* priest/shopkeeper/guard for this room */
	schar colouur; /* random color */
};

struct shclass {
	const char *name;	/* name of the shop type */
	char	symb;		/* this identifies the shop type */
	int	prob;		/* the shop type probability in % */
	schar	shdist;		/* object placement type */
#define D_SCATTER	0	/* normal placement */
#define D_SHOP		1	/* shop-like placement */
#define D_TEMPLE	2	/* temple-like placement */
	struct itp {
	    int iprob;		/* probability of an item type */
	    int itype;		/* item type: if >=0 a class, if < 0 a specific item */
	} iprobs[8];
	const char * const *shknms;	/* list of shopkeeper names for this type */
};

extern NEARDATA struct mkroom rooms[(MAXNROFROOMS+1)*2];
extern NEARDATA struct mkroom* subrooms;
/* the normal rooms on the current level are described in rooms[0..n] for
 * some n<MAXNROFROOMS
 * the vault, if any, is described by rooms[n+1]
 * the next rooms entry has hx -1 as a flag
 * there is at most one non-vault special room on a level
 */

extern struct mkroom *dnstairs_room, *upstairs_room, *sstairs_room;

struct door {
	xchar x,y;
	short arti_key;		/* Index (ART_) of key for this door */
};

extern NEARDATA struct door doors[DOORMAX];

/* values for rtype in the room definition structure */
#define OROOM		 0	/* ordinary room */
#define COURT		 2	/* contains a throne */
#define SWAMP		 3	/* contains pools */
#define VAULT		 4	/* contains piles of gold */
#define BEEHIVE		 5	/* contains killer bees and royal jelly */
#define MORGUE		 6	/* contains corpses, undead and ghosts */
#define BARRACKS	 7	/* contains soldiers and their gear */
#define ZOO		 8	/* floor covered with treasure and monsters */
#define DELPHI		 9	/* contains Oracle and peripherals */
#define TEMPLE		10	/* contains a shrine */
/* [Tom] new rooms... */
#define REALZOO         11      /* zoo-like monsters (monkeys, etc.) */
#define GIANTCOURT      12      /* a giant courtroom */
#define LEPREHALL       13      /* leprechaun hall (Tom Proudfoot) */
#define DRAGONLAIR      14      /* dragons and $$$ */
#define BADFOODSHOP     15      /* bad food! */
#define COCKNEST        16      /* cockatrice nest! */
#define ANTHOLE         17      /* ants! */
#define LEMUREPIT       18      /* lemures */
#define MIGOHIVE        19      /* Mi-Go */
#define FUNGUSFARM      20      /* molds, puddings, and slime */
#define CLINIC      21      /* nurses */
#define TERRORHALL      22      /* hulks */
#define ELEMHALL      23      /* elementals */
#define ANGELHALL      24      /* angels */
#define MIMICHALL      25      /* mimics */
#define NYMPHHALL      26      /* nymphs */
#define SPIDERHALL      27      /* spiders */
#define TROLLHALL      28      /* trolls */
#define HUMANHALL      29      /* humans */
#define GOLEMHALL      30      /* golems */
#define COINHALL      31      /* bad coins */
#define DOUGROOM	32	/* Adams, creeping 42's etc. */
#define ARMORY	33	/* from Unnethack */
#define TENSHALL	34	/* ADOM idea */
#define TRAPROOM	35	/* from Sporkhack */
#define POOLROOM	36	/* from Sporkhack */
#define GRUEROOM	37	/* Zork reference */
#define STATUEROOM	38	/* statues and statue traps */
#define INSIDEROOM	39	/* very weird room */
#define RIVERROOM	40	/* underground rivers in a room */
#define EMPTYNEST	41	/* no monsters, just eggs */

#define CRYPTROOM 42	/* floor of the crypt and many undead */
#define TROUBLEZONE 43	/* T, U, &, v, sometimes K */
#define WEAPONCHAMBER 44	/* monsters with weapon attacks */
#define HELLPIT 45	/* lava, fire-based traps, demons */
#define FEMINISMROOM 46	/* feminism monsters and traps */
#define MEADOWROOM 47	/* grassland, food, a, q, x and n */
#define COOLINGCHAMBER 48	/* snow and ice, cold-based monsters */
#define VOIDROOM 49	/* nether mist, giant chasms, ghosts */
#define HAMLETROOM 50	/* grassland, wells, wagons, H, statue traps, warp zones, monster cubes */
#define KOPSTATION 51	/* keystone kops */
#define BOSSROOM 52	/* boss spawner traps */
#define RNGCENTER 53	/* randomly generated monsters */
#define WIZARDSDORM 54	/* bed, table, caster and spitter monsters, dragons */
#define DOOMEDBARRACKS 55	/* zombieman, former sergeant, wolfensteiner, arachnotron, mancubus */
#define SLEEPINGROOM 56	/* k, r, o, O, mattresses */
#define DIVERPARADISE 57	/* water tunnels and water monsters */
#define MENAGERIE 58	/* d, f, r, u, z, B, S, Y */
#define NASTYCENTRAL 59	/* nasty gray stones everywhere */
#define EMPTYDESERT 60	/* sand and shifting sand */
#define RARITYROOM 61	/* random terrain, Q, J, 7, X, wall monsters */
#define EXHIBITROOM 62	/* e, g, p, t, w, y, N */
#define PRISONCHAMBER 63	/* O and a prisoner on altar */
#define NUCLEARCHAMBER 64	/* styx rivers, contamination monsters */
#define LEVELSEVENTYROOM 65	/* grave walls, traps, monsters */
#define VARIANTROOM 66	/* monsters from a specific variant */

#define EVILROOM 67	/* evilvariant monsters */
#define RELIGIONCENTER 68	/* MS_CONVERT and MS_ALIEN */
#define CHAOSROOM 69	/* chaotic terrain */
#define CURSEDMUMMYROOM 70	/* M and AD_ICUR, one L */
#define MIXEDPOOL 71	/* water and crystal water */
#define ARDUOUSMOUNTAIN 72	/* mountains and tunnels, a few uncommon monsters */
#define LEVELFFROOM 73	/* level 55 room - farmland, a few highways and grassland tiles, joke monsters */
#define VERMINROOM 74	/* rats, snakes and stuff */
#define MIRASPA 75	/* urine lake, acid-based monsters */
#define MACHINEROOM 76	/* tunnels, stalactites and crystal water, nonliving monsters */
#define SHOWERROOM 77	/* bubbles and rain clouds */
#define GREENCROSSROOM 78	/* paved floors and grassland, shoes and humans that should often be peaceful */
#define CENTRALTEDIUM 79	/* highway, grassland, teleporters */
#define RUINEDCHURCH 80	/* ghosts, pieces of glass, altar with spellbook, maybe some scrolls */
#define RAMPAGEROOM 81	/* rockwalls, boulders, traps */
#define GAMECORNER 82	/* monsters from another game */
#define ILLUSIONROOM 83	/* monsters from the Illusory Castle */
#define ROBBERCAVE 84	/* thieving monsters */
#define SANITATIONCENTRAL 85	/* sanity monsters */
#define PLAYERCENTRAL 86	/* player monsters */
#define CASINOROOM 87	/* croupiers */

#define RANDOMROOM 88	/* supposed to select one from an array of other special rooms, for use in .des files */

#define SHOPBASE        89     	/* everything above this is a shop */
#define ARMORSHOP       (SHOPBASE+ 1)   /* specific shop defines for level compiler */
#define SCROLLSHOP      (SHOPBASE+ 2)
#define POTIONSHOP      (SHOPBASE+ 3)
#define WEAPONSHOP      (SHOPBASE+ 4)
#define FOODSHOP        (SHOPBASE+ 5)
#define RINGSHOP        (SHOPBASE+ 6)
#define WANDSHOP        (SHOPBASE+ 7)
#define TOOLSHOP        (SHOPBASE+ 8)
#define PETSHOP         (SHOPBASE+ 9)		/* Stephen White */
#define TINSHOP         (SHOPBASE+10)		/* Robin Johnson */
#define BOOKSHOP        (SHOPBASE+11)
#define USELESSSHOP     (SHOPBASE+12)
#define GUNSHOP         (SHOPBASE+13)
#define BANGSHOP        (SHOPBASE+14)
#define AMMOSHOP        (SHOPBASE+15)
#define LITENSHOP       (SHOPBASE+16)
#define BOULDSHOP       (SHOPBASE+17)
#define ROCKSSHOP       (SHOPBASE+18)
#define WIERDSHOP       (SHOPBASE+19)
#define ZORKSHOP       (SHOPBASE+20)
#define RANDOMSHOP       (SHOPBASE+21)
#define UNIQUESHOP      (SHOPBASE+22)      /* shops here & above not randomly gen'd. */
#define CANDLESHOP      (UNIQUESHOP)
#define BLACKSHOP       (UNIQUESHOP+1)
#define MAXRTYPE        (UNIQUESHOP+1)      /* maximum valid room type */

/* Special type for search_special() */
#define ANY_TYPE	(-1)
#define ANY_SHOP	(-2)

#define NO_ROOM		0	/* indicates lack of room-occupancy */
#define SHARED		1	/* indicates normal shared boundary */
#define SHARED_PLUS	2	/* indicates shared boundary - extra adjacent-
				 * square searching required */

#define ROOMOFFSET	3	/*
				 * (levl[x][y].roomno - ROOMOFFSET) gives
				 * rooms[] index, for inside-squares and
				 * non-shared boundaries.
				 */

#define IS_ROOM_PTR(x)		((x) >= rooms && (x) < rooms + MAXNROFROOMS)
#define IS_ROOM_INDEX(x)	((x) >= 0 && (x) < MAXNROFROOMS)
#define IS_SUBROOM_PTR(x)	((x) >= subrooms && \
				 (x) < subrooms + MAXNROFROOMS)
#define IS_SUBROOM_INDEX(x)	((x) > MAXNROFROOMS && (x) < (MAXNROFROOMS*2))
#define ROOM_INDEX(x)		((x) - rooms)
#define SUBROOM_INDEX(x)	((x) - subrooms)
#define IS_LAST_ROOM_PTR(x)	(ROOM_INDEX(x) == nroom)
#define IS_LAST_SUBROOM_PTR(x)	(!nsubroom || SUBROOM_INDEX(x) == nsubroom)

#endif /* MKROOM_H */

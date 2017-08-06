/*	SCCS Id: @(#)o_init.c	3.4	1999/12/09	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"	/* save & restore info */

STATIC_DCL void setgemprobs(d_level*);
STATIC_DCL void shuffle(int,int,BOOLEAN_P);
STATIC_DCL void shuffle_all(void);
STATIC_DCL boolean interesting_to_discover(int);

static NEARDATA short disco[NUM_OBJECTS] = DUMMY;

#ifdef USE_TILES
STATIC_DCL void shuffle_tiles(void);
extern short glyph2tile[];	/* from tile.c */

/* Shuffle tile assignments to match descriptions, so a red potion isn't
 * displayed with a blue tile and so on.
 *
 * Tile assignments are not saved, and shouldn't be so that a game can
 * be resumed on an otherwise identical non-tile-using binary, so we have
 * to reshuffle the assignments from oc_descr_idx information when a game
 * is restored.  So might as well do that the first time instead of writing
 * another routine.
 */
STATIC_OVL void
shuffle_tiles()
{
	int i;
	short tmp_tilemap[NUM_OBJECTS];

	for (i = 0; i < NUM_OBJECTS; i++)
		tmp_tilemap[i] =
			glyph2tile[objects[i].oc_descr_idx + GLYPH_OBJ_OFF];

	for (i = 0; i < NUM_OBJECTS; i++)
		glyph2tile[i + GLYPH_OBJ_OFF] = tmp_tilemap[i];
}
#endif	/* USE_TILES */

#ifdef PROXY_GRAPHICS
STATIC_DCL void shuffle_proxy_glyphs(void);
extern short glyph2proxy[];	/* from glyphmap.c */

/* Shuffle proxy glyph assignments for the same reason as tiles
 * (internal glyphs are based on object numbers, proxy glyphs
 * are based on object descriptions).
 */
STATIC_OVL void
shuffle_proxy_glyphs()
{
	int i;
	short tmp_glyphmap[NUM_OBJECTS];

	for (i = 0; i < NUM_OBJECTS; i++)
		tmp_glyphmap[i] =
			glyph2proxy[objects[i].oc_descr_idx + GLYPH_OBJ_OFF];

	for (i = 0; i < NUM_OBJECTS; i++)
		glyph2proxy[i + GLYPH_OBJ_OFF] = tmp_glyphmap[i];
}
#endif	/* USE_TILES */

STATIC_OVL void
setgemprobs(dlev)
d_level *dlev;
{
	return; /* what's the point of this stupidity anyway??? --Amy */

	int j, first, lev;

	if (dlev)
	    lev = (ledger_no(dlev) > maxledgerno())
				? maxledgerno() : ledger_no(dlev);
	else
	    lev = 0;
	first = bases[GEM_CLASS];

	for(j = 0; j < 9-lev/3; j++)
		objects[first+j].oc_prob = 0;
	first += j;
	if (first > LAST_GEM) {
		raw_printf("Not enough gems? - first=%d j=%d LAST_GEM=%d",
			first, j, LAST_GEM);
		wait_synch();
	    }
	if (objects[first].oc_class != GEM_CLASS) {
		raw_printf("something is not a gem? - first=%d j=%d LAST_GEM=%d",
			first, j, LAST_GEM);
		wait_synch();
	    }

	if (OBJ_NAME(objects[first]) == (char *)0) {
		raw_printf("gem with bugged description? - first=%d j=%d LAST_GEM=%d",
			first, j, LAST_GEM);
		wait_synch();
	    }

	for (j = first; j <= LAST_GEM; j++)
		/* KMH, balance patch -- valuable gems now sum to 612 */
		objects[j].oc_prob = (612+j-first)/(LAST_GEM+1-first);
}

/* shuffle descriptions on objects o_low to o_high */
STATIC_OVL void
shuffle(o_low, o_high, domaterial)
	int o_low, o_high;
	boolean domaterial;
{
	int i, j, num_to_shuffle;
	short sw;
	int color;

	for (num_to_shuffle = 0, j=o_low; j <= o_high; j++)
		/*if (!objects[j].oc_name_known) num_to_shuffle++;*/
		if (OBJ_DESCR(objects[j])) num_to_shuffle++;
	if (num_to_shuffle < 2) return;

	for (j=o_low; j <= o_high; j++) {
		/*if (objects[j].oc_name_known) continue;*/
		if (!OBJ_DESCR(objects[j])) continue;
		do
			i = j + rn2(o_high-j+1);
		/*while (objects[i].oc_name_known);*/
		while (!OBJ_DESCR(objects[i]));
		sw = objects[j].oc_descr_idx;
		objects[j].oc_descr_idx = objects[i].oc_descr_idx;
		objects[i].oc_descr_idx = sw;
		sw = objects[j].oc_tough;
		objects[j].oc_tough = objects[i].oc_tough;
		objects[i].oc_tough = sw;
		color = objects[j].oc_color;
		objects[j].oc_color = objects[i].oc_color;
		objects[i].oc_color = color;

		/* shuffle material */
		if (domaterial) {
			sw = objects[j].oc_material;
			objects[j].oc_material = objects[i].oc_material;
			objects[i].oc_material = sw;
		}
	}
}

/* A function that assigns random materials to a few objects, called once at game start. --Amy */
void
randommaterials()
{
	register int objtomodify;
	while (rn2(20)) { /* to randomize the # of objects affected --Amy */
		objtomodify = -1;
		while ( (objtomodify == -1) || (objtomodify == GOLD_PIECE) || (objtomodify == STRANGE_OBJECT) || (objtomodify == AMULET_OF_YENDOR) || (objtomodify == CANDELABRUM_OF_INVOCATION) || (objtomodify == BELL_OF_OPENING) || (objtomodify == SPE_BOOK_OF_THE_DEAD) || (objects[objtomodify].oc_prob < 1) ) objtomodify = rn2(NUM_OBJECTS);
		/* we roll until we get an item that can be randomly generated and is not a gold piece or other weird thing --Amy */

		objects[objtomodify].oc_material = rn2(COMPOST + 1); /* allow every material, including 0 (mysterious) */
		/*if (wizard) pline("Modifying material of object %d", objtomodify);*/ /* debug message */

	}

	if (!rn2(5)) {

		while (rn2(20)) {
			objtomodify = -1;
			while ( (objtomodify == -1) || (objtomodify == GOLD_PIECE) || (objtomodify == STRANGE_OBJECT) || (objtomodify == AMULET_OF_YENDOR) || (objtomodify == CANDELABRUM_OF_INVOCATION) || (objtomodify == BELL_OF_OPENING) || (objtomodify == SPE_BOOK_OF_THE_DEAD) || (objects[objtomodify].oc_prob < 1) ) objtomodify = rn2(NUM_OBJECTS);

			objects[objtomodify].oc_material = rn2(COMPOST + 1); /* allow every material, including 0 (mysterious) */

		}

	}

	if (!rn2(20)) {

		while (rn2(50)) {
			objtomodify = -1;
			while ( (objtomodify == -1) || (objtomodify == GOLD_PIECE) || (objtomodify == STRANGE_OBJECT) || (objtomodify == AMULET_OF_YENDOR) || (objtomodify == CANDELABRUM_OF_INVOCATION) || (objtomodify == BELL_OF_OPENING) || (objtomodify == SPE_BOOK_OF_THE_DEAD) || (objects[objtomodify].oc_prob < 1) ) objtomodify = rn2(NUM_OBJECTS);

			objects[objtomodify].oc_material = rn2(COMPOST + 1); /* allow every material, including 0 (mysterious) */

		}

	}

	if (!rn2(100)) {

		while (rn2(200)) {
			objtomodify = -1;
			while ( (objtomodify == -1) || (objtomodify == GOLD_PIECE) || (objtomodify == STRANGE_OBJECT) || (objtomodify == AMULET_OF_YENDOR) || (objtomodify == CANDELABRUM_OF_INVOCATION) || (objtomodify == BELL_OF_OPENING) || (objtomodify == SPE_BOOK_OF_THE_DEAD) || (objects[objtomodify].oc_prob < 1) ) objtomodify = rn2(NUM_OBJECTS);

			objects[objtomodify].oc_material = rn2(COMPOST + 1); /* allow every material, including 0 (mysterious) */

		}

	}

}

void
init_objects()
{
register int i, first, last, sum;
register char oclass;
#ifdef TEXTCOLOR
# define COPY_OBJ_DESCR(o_dst,o_src) \
			o_dst.oc_descr_idx = o_src.oc_descr_idx,\
			o_dst.oc_color = o_src.oc_color
#else
# define COPY_OBJ_DESCR(o_dst,o_src) o_dst.oc_descr_idx = o_src.oc_descr_idx
#endif

	/* bug fix to prevent "initialization error" abort on Intel Xenix.
	 * reported by mikew@semike
	 */
	for (i = 0; i < MAXOCLASSES; i++)
		bases[i] = 0;
	/* initialize object descriptions */
	for (i = 0; i < NUM_OBJECTS; i++)
		objects[i].oc_name_idx = objects[i].oc_descr_idx = i;
	/* init base; if probs given check that they add up to 10000,
	   otherwise compute probs */
	first = 0;
	while( first < NUM_OBJECTS ) {
		oclass = objects[first].oc_class;
		last = first+1;
		while (last < NUM_OBJECTS && objects[last].oc_class == oclass) last++;
		bases[(int)oclass] = first;

		if (oclass == GEM_CLASS) {
			setgemprobs((d_level *)0);

			if (rn2(2)) { /* change turquoise from green to blue? */
			    COPY_OBJ_DESCR(objects[TURQUOISE],objects[SAPPHIRE]);
			}
			if (rn2(2)) { /* change aquamarine from green to blue? */
			    COPY_OBJ_DESCR(objects[AQUAMARINE],objects[SAPPHIRE]);
			}
			if (rn2(2)) { /* change epidote from green to red? */
			    COPY_OBJ_DESCR(objects[EPIDOTE],objects[RUBY]);
			}
			if (rn2(2)) { /* change rhodochrosite from red to pink? */
			    COPY_OBJ_DESCR(objects[RHODOCHROSITE],objects[ROSE_QUARTZ]);
			}
			if (rn2(2)) { /* change amazonite from cyan to green? */
			    COPY_OBJ_DESCR(objects[AMAZONITE],objects[EMERALD]);
			}
			if (rn2(2)) { /* change covelline from black to blue? */
			    COPY_OBJ_DESCR(objects[COVELLINE],objects[SAPPHIRE]);
			}
			if (rn2(2)) { /* change chrysocolla from cyan to teal? */
			    COPY_OBJ_DESCR(objects[CHRYSOCOLLA],objects[CYAN_STONE]);
			}
			if (rn2(2)) { /* change peridot from radiant to cyan? */
			    COPY_OBJ_DESCR(objects[PERIDOT],objects[CHALCEDON]);
			}
			if (rn2(2)) { /* change anhydrite from teal to cyan? */
			    COPY_OBJ_DESCR(objects[ANHYDRITE],objects[CHALCEDON]);
			}
			if (rn2(2)) { /* change marble from white to black? */
			    COPY_OBJ_DESCR(objects[MARBLE],objects[JET]);
			}
			if (rn2(2)) { /* change moonstone from white to yellow? */
			    COPY_OBJ_DESCR(objects[MOONSTONE],objects[CHRYSOBERYL]);
			}
			switch (rn2(4)) { /* change fluorite from violet? */
			    case 0:  break;
			    case 1:	/* blue */
				COPY_OBJ_DESCR(objects[FLUORITE],objects[SAPPHIRE]);
				break;
			    case 2:	/* white */
				COPY_OBJ_DESCR(objects[FLUORITE],objects[DIAMOND]);
				break;
			    case 3:	/* green */
				COPY_OBJ_DESCR(objects[FLUORITE],objects[EMERALD]);
				break;
			}
			switch (rn2(4)) { /* change tourmaline from red? */
			    case 0:  break;
			    case 1:	/* red */
				COPY_OBJ_DESCR(objects[TOURMALINE],objects[RUBY]);
				break;
			    case 2:	/* magenta */
				COPY_OBJ_DESCR(objects[TOURMALINE],objects[AMETHYST]);
				break;
			    case 3:	/* green */
				COPY_OBJ_DESCR(objects[TOURMALINE],objects[EMERALD]);
				break;
			}
			switch (rn2(4)) { /* change apophyllite from teal? */
			    case 0:  break;
			    case 1:	/* cyan */
				COPY_OBJ_DESCR(objects[APOPHYLLITE],objects[CHALCEDON]);
				break;
			    case 2:	/* radiant */
				COPY_OBJ_DESCR(objects[APOPHYLLITE],objects[PREHNITE]);
				break;
			    case 3:	/* green */
				COPY_OBJ_DESCR(objects[APOPHYLLITE],objects[EMERALD]);
				break;
			}
			switch (rn2(3)) { /* change spinel from pink? */
			    case 0:  break;
			    case 1:	/* red */
				COPY_OBJ_DESCR(objects[SPINEL],objects[RUBY]);
				break;
			    case 2:	/* magenta */
				COPY_OBJ_DESCR(objects[SPINEL],objects[AMETHYST]);
				break;
			}
			switch (rn2(3)) { /* change vivianite from cyan? */
			    case 0:  break;
			    case 1:	/* black */
				COPY_OBJ_DESCR(objects[VIVIANITE],objects[JET]);
				break;
			    case 2:	/* blue */
				COPY_OBJ_DESCR(objects[VIVIANITE],objects[SAPPHIRE]);
				break;
			}
			switch (rn2(13)) { /* change wonder stone from magenta? */
			    case 0:  break;
			    case 1:	/* black */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[JET]);
				break;
			    case 2:	/* blue */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[SAPPHIRE]);
				break;
			    case 3:	/* red */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[RUBY]);
				break;
			    case 4:	/* radiant */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[PREHNITE]);
				break;
			    case 5:	/* green */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[EMERALD]);
				break;
			    case 6:	/* white */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[DIAMOND]);
				break;
			    case 7:	/* brown */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[TOPAZ]);
				break;
			    case 8:	/* yellow */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[CHRYSOBERYL]);
				break;
			    case 9:	/* pink */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[ROSE_QUARTZ]);
				break;
			    case 10:	/* orange */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[AGATE]);
				break;
			    case 11:	/* cyan */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[CHALCEDON]);
				break;
			    case 12:	/* teal */
				COPY_OBJ_DESCR(objects[WONDER_STONE],objects[CYAN_STONE]);
				break;
			}
		}
	check:
		sum = 0;
		for(i = first; i < last; i++) sum += objects[i].oc_prob;
		if(sum == 0) {
			for(i = first; i < last; i++)
			    objects[i].oc_prob = (10000+i-first)/(last-first);
			goto check;
		}
		if(sum != sum)
			error("init-prob error for class %d (%d%%)", oclass, sum);
		first = last;
	}

	objects[COPPER_DRAGON_SCALE_MAIL].oc_oprop = objects[COPPER_DRAGON_SCALES].oc_oprop = objects[COPPER_DRAGON_SCALE_SHIELD].oc_oprop = randenchantment();
	objects[PLATINUM_DRAGON_SCALE_MAIL].oc_oprop = objects[PLATINUM_DRAGON_SCALES].oc_oprop = objects[PLATINUM_DRAGON_SCALE_SHIELD].oc_oprop = randenchantment();
	objects[BRASS_DRAGON_SCALE_MAIL].oc_oprop = objects[BRASS_DRAGON_SCALES].oc_oprop = objects[BRASS_DRAGON_SCALE_SHIELD].oc_oprop = randenchantment();
	objects[AMETHYST_DRAGON_SCALE_MAIL].oc_oprop = objects[AMETHYST_DRAGON_SCALES].oc_oprop = objects[AMETHYST_DRAGON_SCALE_SHIELD].oc_oprop = randenchantment();
	objects[DIAMOND_DRAGON_SCALE_MAIL].oc_oprop = objects[DIAMOND_DRAGON_SCALES].oc_oprop = objects[DIAMOND_DRAGON_SCALE_SHIELD].oc_oprop = randenchantment();
	objects[EMERALD_DRAGON_SCALE_MAIL].oc_oprop = objects[EMERALD_DRAGON_SCALES].oc_oprop = objects[EMERALD_DRAGON_SCALE_SHIELD].oc_oprop = randenchantment();
	objects[SAPPHIRE_DRAGON_SCALE_MAIL].oc_oprop = objects[SAPPHIRE_DRAGON_SCALES].oc_oprop = objects[SAPPHIRE_DRAGON_SCALE_SHIELD].oc_oprop = randenchantment();
	objects[RUBY_DRAGON_SCALE_MAIL].oc_oprop = objects[RUBY_DRAGON_SCALES].oc_oprop = objects[RUBY_DRAGON_SCALE_SHIELD].oc_oprop = randenchantment();

	objects[PURPLE_DRAGON_SCALE_MAIL].oc_oprop = randenchantment();
	objects[PURPLE_DRAGON_SCALES].oc_oprop = randenchantment();
	objects[PURPLE_DRAGON_SCALE_SHIELD].oc_oprop = randenchantment();

	objects[RANDOMIZED_HELMET].oc_oprop = randenchantment();
	objects[ROBE_OF_RANDOMNESS].oc_oprop = randenchantment();
	objects[HIGH_STILETTOS].oc_oprop = randenchantment();
	objects[UNKNOWN_GAUNTLETS].oc_oprop = randenchantment();
	objects[MISSING_CLOAK].oc_oprop = randenchantment();

	objects[RANDOMIZED_HELMET].a_can = rnd(10) ? 0 : rnd(3);
	objects[ROBE_OF_RANDOMNESS].a_can = rnd(10) ? 0 : rnd(3);
	objects[HIGH_STILETTOS].a_can = rnd(10) ? 0 : rnd(3);
	objects[UNKNOWN_GAUNTLETS].a_can = rnd(10) ? 0 : rnd(3);
	objects[MISSING_CLOAK].a_can = rn2(4);

	objects[WONDER_HELMET].a_can = rnd(10) ? 0 : rnd(3);
	objects[SKY_HIGH_HEELS].a_can = rnd(10) ? 0 : rnd(3);
	objects[ARCANE_GAUNTLETS].a_can = rnd(10) ? 0 : rnd(3);
	objects[SPECIAL_CLOAK].a_can = rn2(4);

	objects[POINTED_HELMET].a_can = rnd(10) ? 0 : rnd(3);
	objects[PREHISTORIC_BOOTS].a_can = rnd(10) ? 0 : rnd(3);
	objects[PLACEHOLDER_GLOVES].a_can = rnd(10) ? 0 : rnd(3);
	objects[PLAIN_CLOAK].a_can = rn2(4);

	objects[BOG_STANDARD_HELMET].a_can = rnd(10) ? 0 : rnd(3);
	objects[SYNTHETIC_SANDALS].a_can = rnd(10) ? 0 : rnd(3);
	objects[PROTECTIVE_GLOVES].a_can = rnd(10) ? 0 : rnd(3);
	objects[ARCHAIC_CLOAK].a_can = rn2(4);

	objects[RANDOMIZED_HELMET].a_ac = 0;
	if (rn2(10)) objects[RANDOMIZED_HELMET].a_ac = 1;
	while (!rn2(3)) {
		if (objects[RANDOMIZED_HELMET].a_ac < 20) objects[RANDOMIZED_HELMET].a_ac++;
	}
	objects[ROBE_OF_RANDOMNESS].a_ac = 0;
	if (rn2(10)) objects[ROBE_OF_RANDOMNESS].a_ac = 1;
	while (!rn2(2)) {
		if (objects[ROBE_OF_RANDOMNESS].a_ac < 20) objects[ROBE_OF_RANDOMNESS].a_ac++;
	}
	objects[HIGH_STILETTOS].a_ac = 0;
	if (rn2(10)) objects[HIGH_STILETTOS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[HIGH_STILETTOS].a_ac < 20) objects[HIGH_STILETTOS].a_ac++;
	}
	objects[UNKNOWN_GAUNTLETS].a_ac = 0;
	if (rn2(10)) objects[UNKNOWN_GAUNTLETS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[UNKNOWN_GAUNTLETS].a_ac < 20) objects[UNKNOWN_GAUNTLETS].a_ac++;
	}
	objects[MISSING_CLOAK].a_ac = 0;
	if (rn2(10)) objects[MISSING_CLOAK].a_ac = 1;
	while (!rn2(3)) {
		if (objects[MISSING_CLOAK].a_ac < 20) objects[MISSING_CLOAK].a_ac++;
	}
	objects[SPECIAL_CLOAK].a_ac = 0;
	if (rn2(10)) objects[SPECIAL_CLOAK].a_ac = 1;
	while (!rn2(3)) {
		if (objects[SPECIAL_CLOAK].a_ac < 20) objects[SPECIAL_CLOAK].a_ac++;
	}
	objects[WONDER_HELMET].a_ac = 0;
	if (rn2(10)) objects[WONDER_HELMET].a_ac = 1;
	while (!rn2(3)) {
		if (objects[WONDER_HELMET].a_ac < 20) objects[WONDER_HELMET].a_ac++;
	}
	objects[ARCANE_GAUNTLETS].a_ac = 0;
	if (rn2(10)) objects[ARCANE_GAUNTLETS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[ARCANE_GAUNTLETS].a_ac < 20) objects[ARCANE_GAUNTLETS].a_ac++;
	}
	objects[SKY_HIGH_HEELS].a_ac = 0;
	if (rn2(10)) objects[SKY_HIGH_HEELS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[SKY_HIGH_HEELS].a_ac < 20) objects[SKY_HIGH_HEELS].a_ac++;
	}
	objects[PLAIN_CLOAK].a_ac = 0;
	if (rn2(10)) objects[PLAIN_CLOAK].a_ac = 1;
	while (!rn2(3)) {
		if (objects[PLAIN_CLOAK].a_ac < 20) objects[PLAIN_CLOAK].a_ac++;
	}
	objects[POINTED_HELMET].a_ac = 0;
	if (rn2(10)) objects[POINTED_HELMET].a_ac = 1;
	while (!rn2(3)) {
		if (objects[POINTED_HELMET].a_ac < 20) objects[POINTED_HELMET].a_ac++;
	}
	objects[PLACEHOLDER_GLOVES].a_ac = 0;
	if (rn2(10)) objects[PLACEHOLDER_GLOVES].a_ac = 1;
	while (!rn2(3)) {
		if (objects[PLACEHOLDER_GLOVES].a_ac < 20) objects[PLACEHOLDER_GLOVES].a_ac++;
	}
	objects[PREHISTORIC_BOOTS].a_ac = 0;
	if (rn2(10)) objects[PREHISTORIC_BOOTS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[PREHISTORIC_BOOTS].a_ac < 20) objects[PREHISTORIC_BOOTS].a_ac++;
	}
	objects[ARCHAIC_CLOAK].a_ac = 0;
	if (rn2(10)) objects[ARCHAIC_CLOAK].a_ac = 1;
	while (!rn2(3)) {
		if (objects[ARCHAIC_CLOAK].a_ac < 20) objects[ARCHAIC_CLOAK].a_ac++;
	}
	objects[BOG_STANDARD_HELMET].a_ac = 0;
	if (rn2(10)) objects[BOG_STANDARD_HELMET].a_ac = 1;
	while (!rn2(3)) {
		if (objects[BOG_STANDARD_HELMET].a_ac < 20) objects[BOG_STANDARD_HELMET].a_ac++;
	}
	objects[PROTECTIVE_GLOVES].a_ac = 0;
	if (rn2(10)) objects[PROTECTIVE_GLOVES].a_ac = 1;
	while (!rn2(3)) {
		if (objects[PROTECTIVE_GLOVES].a_ac < 20) objects[PROTECTIVE_GLOVES].a_ac++;
	}
	objects[SYNTHETIC_SANDALS].a_ac = 0;
	if (rn2(10)) objects[SYNTHETIC_SANDALS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[SYNTHETIC_SANDALS].a_ac < 20) objects[SYNTHETIC_SANDALS].a_ac++;
	}

	objects[RIN_RANDOM_EFFECTS].oc_oprop = randenchantment();
	objects[RIN_SPECIAL_EFFECTS].oc_oprop = randenchantment();
	objects[AMULET_OF_THE_RNG].oc_oprop = randenchantment();
	objects[AMULET_OF_INFINITY].oc_oprop = randenchantment();
	objects[RIN_DANGER].oc_oprop = randnastyenchantment();
	objects[AMULET_OF_DANGER].oc_oprop = randnastyenchantment();
	objects[NASTY_CLOAK].oc_oprop = randnastyenchantment();
	objects[ROBE_OF_NASTINESS].oc_oprop = randnastyenchantment();
	objects[UNWANTED_HELMET].oc_oprop = randnastyenchantment();
	objects[EVIL_GLOVES].oc_oprop = randnastyenchantment();
	objects[UNFAIR_STILETTOS].oc_oprop = randnastyenchantment();

	objects[MAGICAL_SHIELD].oc_oprop = randenchantment();
	objects[EVIL_DRAGON_SCALE_MAIL].oc_oprop = randnastyenchantment();
	objects[EVIL_DRAGON_SCALES].oc_oprop = randnastyenchantment();
	objects[EVIL_DRAGON_SCALE_SHIELD].oc_oprop = randnastyenchantment();
	objects[DIFFICULT_SHIELD].oc_oprop = randnastyenchantment();
	objects[BAD_SHIRT].oc_oprop = randnastyenchantment();
	objects[EVIL_PLATE_MAIL].oc_oprop = randnastyenchantment();
	objects[EVIL_LEATHER_ARMOR].oc_oprop = randnastyenchantment();

	objects[DUMMY_CLOAK_A].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_B].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_C].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_D].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_E].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_F].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_G].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_H].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_I].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_J].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_K].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_L].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_M].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_N].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_O].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_P].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_Q].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_R].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_S].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_T].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_U].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_V].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

	objects[DUMMY_CLOAK_A].a_can = rn2(4);
	objects[DUMMY_CLOAK_B].a_can = rn2(4);
	objects[DUMMY_CLOAK_C].a_can = rn2(4);
	objects[DUMMY_CLOAK_D].a_can = rn2(4);
	objects[DUMMY_CLOAK_E].a_can = rn2(4);
	objects[DUMMY_CLOAK_F].a_can = rn2(4);
	objects[DUMMY_CLOAK_G].a_can = rn2(4);
	objects[DUMMY_CLOAK_H].a_can = rn2(4);
	objects[DUMMY_CLOAK_I].a_can = rn2(4);
	objects[DUMMY_CLOAK_J].a_can = rn2(4);
	objects[DUMMY_CLOAK_K].a_can = rn2(4);
	objects[DUMMY_CLOAK_L].a_can = rn2(4);
	objects[DUMMY_CLOAK_M].a_can = rn2(4);
	objects[DUMMY_CLOAK_N].a_can = rn2(4);
	objects[DUMMY_CLOAK_O].a_can = rn2(4);
	objects[DUMMY_CLOAK_P].a_can = rn2(4);
	objects[DUMMY_CLOAK_Q].a_can = rn2(4);
	objects[DUMMY_CLOAK_R].a_can = rn2(4);
	objects[DUMMY_CLOAK_S].a_can = rn2(4);
	objects[DUMMY_CLOAK_T].a_can = rn2(4);
	objects[DUMMY_CLOAK_U].a_can = rn2(4);
	objects[DUMMY_CLOAK_V].a_can = rn2(4);

	objects[DUMMY_CLOAK_A].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_A].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_A].a_ac < 20) objects[DUMMY_CLOAK_A].a_ac++;
	}
	objects[DUMMY_CLOAK_B].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_B].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_B].a_ac < 20) objects[DUMMY_CLOAK_B].a_ac++;
	}
	objects[DUMMY_CLOAK_C].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_C].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_C].a_ac < 20) objects[DUMMY_CLOAK_C].a_ac++;
	}
	objects[DUMMY_CLOAK_D].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_D].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_D].a_ac < 20) objects[DUMMY_CLOAK_D].a_ac++;
	}
	objects[DUMMY_CLOAK_E].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_E].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_E].a_ac < 20) objects[DUMMY_CLOAK_E].a_ac++;
	}
	objects[DUMMY_CLOAK_F].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_F].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_F].a_ac < 20) objects[DUMMY_CLOAK_F].a_ac++;
	}
	objects[DUMMY_CLOAK_G].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_G].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_G].a_ac < 20) objects[DUMMY_CLOAK_G].a_ac++;
	}
	objects[DUMMY_CLOAK_H].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_H].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_H].a_ac < 20) objects[DUMMY_CLOAK_H].a_ac++;
	}
	objects[DUMMY_CLOAK_I].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_I].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_I].a_ac < 20) objects[DUMMY_CLOAK_I].a_ac++;
	}
	objects[DUMMY_CLOAK_J].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_J].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_J].a_ac < 20) objects[DUMMY_CLOAK_J].a_ac++;
	}
	objects[DUMMY_CLOAK_K].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_K].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_K].a_ac < 20) objects[DUMMY_CLOAK_K].a_ac++;
	}
	objects[DUMMY_CLOAK_L].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_L].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_L].a_ac < 20) objects[DUMMY_CLOAK_L].a_ac++;
	}
	objects[DUMMY_CLOAK_M].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_M].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_M].a_ac < 20) objects[DUMMY_CLOAK_M].a_ac++;
	}
	objects[DUMMY_CLOAK_N].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_N].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_N].a_ac < 20) objects[DUMMY_CLOAK_N].a_ac++;
	}
	objects[DUMMY_CLOAK_O].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_O].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_O].a_ac < 20) objects[DUMMY_CLOAK_O].a_ac++;
	}
	objects[DUMMY_CLOAK_P].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_P].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_P].a_ac < 20) objects[DUMMY_CLOAK_P].a_ac++;
	}
	objects[DUMMY_CLOAK_Q].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_Q].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_Q].a_ac < 20) objects[DUMMY_CLOAK_Q].a_ac++;
	}
	objects[DUMMY_CLOAK_R].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_R].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_R].a_ac < 20) objects[DUMMY_CLOAK_R].a_ac++;
	}
	objects[DUMMY_CLOAK_S].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_S].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_S].a_ac < 20) objects[DUMMY_CLOAK_S].a_ac++;
	}
	objects[DUMMY_CLOAK_T].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_T].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_T].a_ac < 20) objects[DUMMY_CLOAK_T].a_ac++;
	}
	objects[DUMMY_CLOAK_U].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_U].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_U].a_ac < 20) objects[DUMMY_CLOAK_U].a_ac++;
	}
	objects[DUMMY_CLOAK_V].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_V].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_V].a_ac < 20) objects[DUMMY_CLOAK_V].a_ac++;
	}

	objects[DUMMY_HELMET_A].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_B].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_C].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_D].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_E].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_F].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_G].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_H].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_I].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_J].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_K].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

	objects[DUMMY_HELMET_A].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_B].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_C].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_D].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_E].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_F].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_G].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_H].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_I].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_J].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_K].a_can = rnd(10) ? 0 : rnd(3);

	objects[DUMMY_HELMET_A].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_A].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_A].a_ac < 20) objects[DUMMY_HELMET_A].a_ac++;
	}
	objects[DUMMY_HELMET_B].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_B].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_B].a_ac < 20) objects[DUMMY_HELMET_B].a_ac++;
	}
	objects[DUMMY_HELMET_C].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_C].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_C].a_ac < 20) objects[DUMMY_HELMET_C].a_ac++;
	}
	objects[DUMMY_HELMET_D].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_D].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_D].a_ac < 20) objects[DUMMY_HELMET_D].a_ac++;
	}
	objects[DUMMY_HELMET_E].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_E].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_E].a_ac < 20) objects[DUMMY_HELMET_E].a_ac++;
	}
	objects[DUMMY_HELMET_F].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_F].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_F].a_ac < 20) objects[DUMMY_HELMET_F].a_ac++;
	}
	objects[DUMMY_HELMET_G].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_G].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_G].a_ac < 20) objects[DUMMY_HELMET_G].a_ac++;
	}
	objects[DUMMY_HELMET_H].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_H].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_H].a_ac < 20) objects[DUMMY_HELMET_H].a_ac++;
	}
	objects[DUMMY_HELMET_I].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_I].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_I].a_ac < 20) objects[DUMMY_HELMET_I].a_ac++;
	}
	objects[DUMMY_HELMET_J].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_J].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_J].a_ac < 20) objects[DUMMY_HELMET_J].a_ac++;
	}
	objects[DUMMY_HELMET_K].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_K].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_K].a_ac < 20) objects[DUMMY_HELMET_K].a_ac++;
	}

	objects[DUMMY_GLOVES_A].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_B].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_C].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_D].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_E].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_F].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_G].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_H].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_I].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

	objects[DUMMY_GLOVES_A].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_B].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_C].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_D].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_E].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_F].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_G].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_H].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_I].a_can = rnd(10) ? 0 : rnd(3);

	objects[DUMMY_GLOVES_A].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_A].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_A].a_ac < 20) objects[DUMMY_GLOVES_A].a_ac++;
	}
	objects[DUMMY_GLOVES_B].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_B].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_B].a_ac < 20) objects[DUMMY_GLOVES_B].a_ac++;
	}
	objects[DUMMY_GLOVES_C].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_C].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_C].a_ac < 20) objects[DUMMY_GLOVES_C].a_ac++;
	}
	objects[DUMMY_GLOVES_D].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_D].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_D].a_ac < 20) objects[DUMMY_GLOVES_D].a_ac++;
	}
	objects[DUMMY_GLOVES_E].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_E].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_E].a_ac < 20) objects[DUMMY_GLOVES_E].a_ac++;
	}
	objects[DUMMY_GLOVES_F].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_F].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_F].a_ac < 20) objects[DUMMY_GLOVES_F].a_ac++;
	}
	objects[DUMMY_GLOVES_G].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_G].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_G].a_ac < 20) objects[DUMMY_GLOVES_G].a_ac++;
	}
	objects[DUMMY_GLOVES_H].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_H].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_H].a_ac < 20) objects[DUMMY_GLOVES_H].a_ac++;
	}
	objects[DUMMY_GLOVES_I].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_I].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_I].a_ac < 20) objects[DUMMY_GLOVES_I].a_ac++;
	}

	objects[DUMMY_BOOTS_A].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_B].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_C].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_D].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_E].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_F].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_G].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_H].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_I].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_J].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_K].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_L].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

	objects[DUMMY_BOOTS_A].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_B].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_C].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_D].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_E].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_F].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_G].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_H].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_I].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_J].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_K].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_L].a_can = rnd(10) ? 0 : rnd(3);

	objects[DUMMY_BOOTS_A].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_A].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_A].a_ac < 20) objects[DUMMY_BOOTS_A].a_ac++;
	}
	objects[DUMMY_BOOTS_B].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_B].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_B].a_ac < 20) objects[DUMMY_BOOTS_B].a_ac++;
	}
	objects[DUMMY_BOOTS_C].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_C].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_C].a_ac < 20) objects[DUMMY_BOOTS_C].a_ac++;
	}
	objects[DUMMY_BOOTS_D].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_D].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_D].a_ac < 20) objects[DUMMY_BOOTS_D].a_ac++;
	}
	objects[DUMMY_BOOTS_E].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_E].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_E].a_ac < 20) objects[DUMMY_BOOTS_E].a_ac++;
	}
	objects[DUMMY_BOOTS_F].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_F].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_F].a_ac < 20) objects[DUMMY_BOOTS_F].a_ac++;
	}
	objects[DUMMY_BOOTS_G].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_G].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_G].a_ac < 20) objects[DUMMY_BOOTS_G].a_ac++;
	}
	objects[DUMMY_BOOTS_H].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_H].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_H].a_ac < 20) objects[DUMMY_BOOTS_H].a_ac++;
	}
	objects[DUMMY_BOOTS_I].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_I].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_I].a_ac < 20) objects[DUMMY_BOOTS_I].a_ac++;
	}
	objects[DUMMY_BOOTS_J].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_J].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_J].a_ac < 20) objects[DUMMY_BOOTS_J].a_ac++;
	}
	objects[DUMMY_BOOTS_K].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_K].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_K].a_ac < 20) objects[DUMMY_BOOTS_K].a_ac++;
	}
	objects[DUMMY_BOOTS_L].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_L].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_L].a_ac < 20) objects[DUMMY_BOOTS_L].a_ac++;
	}

	objects[SPE_FORBIDDEN_KNOWLEDGE].oc_level = rnd(8);
	objects[SPE_FORBIDDEN_KNOWLEDGE].oc_cost = (objects[SPE_FORBIDDEN_KNOWLEDGE].oc_level * 100);

	objects[SPE_CHARACTER_RECURSION].oc_level = rnd(8);
	objects[SPE_CHARACTER_RECURSION].oc_cost = (objects[SPE_CHARACTER_RECURSION].oc_level * 100);

	/* shuffle descriptions */
	shuffle_all();
#ifdef USE_TILES
	shuffle_tiles();
#endif
#ifdef PROXY_GRAPHICS
	shuffle_proxy_glyphs();
#endif
}

STATIC_OVL void
shuffle_all()
{
	int first, last, oclass;

	for (oclass = 1; oclass < MAXOCLASSES; oclass++) {
		first = bases[oclass];
		last = first+1;
		while (last < NUM_OBJECTS && objects[last].oc_class == oclass)
			last++;

		if (OBJ_DESCR(objects[first]) != (char *)0 &&
				oclass != TOOL_CLASS &&
				oclass != WEAPON_CLASS &&
				oclass != ARMOR_CLASS &&
				oclass != BALL_CLASS &&
				oclass != CHAIN_CLASS &&
				oclass != VENOM_CLASS &&
				oclass != GEM_CLASS) {
			int j = last-1;

			if (oclass == POTION_CLASS)
			    /* water and following have fixed descriptions */
			    j = POT_WATER - 1;
			else if (oclass == AMULET_CLASS ||
				 oclass == SCROLL_CLASS ||
				 oclass == SPBOOK_CLASS) {
			    while (!objects[j].oc_magic || objects[j].oc_unique)
				j--;
			}

			/* non-magical amulets, scrolls, and spellbooks
			 * (ex. imitation Amulets, blank, scrolls of mail)
			 * and one-of-a-kind magical artifacts at the end of
			 * their class in objects[] have fixed descriptions.
			 */
			shuffle(first, j, TRUE);
		}
	}

	/* shuffle the helmets */
	shuffle(HELMET, HELM_OF_TELEPATHY, TRUE);

	/* shuffle the gloves */
	shuffle(LEATHER_GLOVES, GAUNTLETS_OF_DEXTERITY, TRUE);

    /* shuffle the robes */
    shuffle(ROBE, ROBE_OF_WEAKNESS, TRUE);

	/* shuffle the cloaks */
	shuffle(CLOAK_OF_PROTECTION, CLOAK_OF_DISPLACEMENT, TRUE);

	/* shuffle the boots [if they change, update find_skates() below] */
	shuffle(SPEED_BOOTS, LEVITATION_BOOTS, TRUE);
}

/* find the object index for snow boots; used [once] by slippery ice code */
int
find_skates()
{
    register int i;
    register const char *s;

    for (i = SPEED_BOOTS; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "snow boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "zimniye sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qor chizilmasin"))
	    return i;
    }

    impossible("snow boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_skates2()
{
    register int i;
    register const char *s;

    for (i = SPEED_BOOTS; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "winter boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sapogi zimniye"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qish chizilmasin"))
	    return i;
    }

    impossible("winter boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_skates3()
{
    register int i;
    register const char *s;

    for (i = SPEED_BOOTS; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "winter stilettos"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "zima stilety"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qish sandal chizilmasin"))
	    return i;
    }

    impossible("winter stilettos not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_skates4()
{
    register int i;
    register const char *s;

    for (i = SPEED_BOOTS; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ski heels"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lyzhnyye kabluki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chang'i poshnalar"))
	    return i;
    }

    impossible("ski heels not found?");
    return -1;	/* not 0, or caller would try again each move */
}

/* tons of more such cases for randarts, the ones that depend on randomized appearances --Amy */

int
find_opera_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "opera cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nakidka"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "operasi plash"))
	    return i;
    }

    impossible("opera cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_explosive_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "explosive boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vzryvnyye sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "portlovchi chizilmasin"))
	    return i;
    }

    impossible("explosive boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_irregular_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "irregular boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "neregulyarnyye sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tartibsizlik chizilmasin"))
	    return i;
    }

    impossible("irregular boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_wedge_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "wedge boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "klin sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "xanjar chizilmasin"))
	    return i;
    }

    impossible("wedge boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_aluminium_helmet()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "aluminium helmet"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shlem iz alyuminiya"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "alyuminiy dubulg'a"))
	    return i;
    }

    impossible("aluminium helmet not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_ghostly_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ghostly cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "prizrachnyy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "diniy plash"))
	    return i;
    }

    impossible("ghostly cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_polnish_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polnish gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pol'skiye perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polsha qo'lqop"))
	    return i;
    }

    impossible("polnish gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_velcro_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velcro boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lipuchki sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cirt chizilmasin"))
	    return i;
    }

    impossible("velcro boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_clumsy_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "clumsy gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "neuklyuzhiye perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qo'pol qo'lqop"))
	    return i;
    }

    impossible("clumsy gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_fin_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fin boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plavnik sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "kanatcik chizilmasin"))
	    return i;
    }

    impossible("fin boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_profiled_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "profiled boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "profilirovannyye sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "profilli chizilmasin"))
	    return i;
    }

    impossible("profiled boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_void_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "void cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nedeystvitel'nym plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "haqiqiy emas plash"))
	    return i;
    }

    impossible("void cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_grey_shaded_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grey-shaded gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sero-zatenennykh perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "kulrang-soyali qo'lqop"))
	    return i;
    }

    impossible("grey-shaded gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_weeb_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weeb cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "zese plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yaponiya ucube rido"))
	    return i;
    }

    impossible("weeb cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_persian_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "persian boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "persidskiye sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fors chizilmasin"))
	    return i;
    }

    impossible("persian boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_hugging_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hugging boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "obnimat'sya sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "havola etdi chizilmasin"))
	    return i;
    }

    impossible("hugging boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_fleecy_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleecy boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "flis sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tozalamoq chizilmasin"))
	    return i;
    }

    impossible("fleecy boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_fingerless_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fingerless gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mitenki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "kam qo'lqop barmoq"))
	    return i;
    }

    impossible("fingerless gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_mantle_of_coat()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mantle of coat"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mantiya pal'to"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ko'ylagi mantiya"))
	    return i;
    }

    impossible("mantle of coat not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_fatal_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fatal gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fatal'nyye perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "halokatli qo'lqop"))
	    return i;
    }

    impossible("fatal gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_beautiful_heels()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "beautiful heels"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "krasivyye kabluki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chiroyli ko'chirish to'piqlarni"))
	    return i;
    }

    impossible("beautiful heels not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_homicidal_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "homicidal cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "smertonosnyy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "halokatli plash"))
	    return i;
    }

    impossible("homicidal cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_castlevania_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "castlevania boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "zamok vaney sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qal'a vania chizilmasin"))
	    return i;
    }

    impossible("castlevania boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_greek_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "greek cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "grecheskiy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yunon plash"))
	    return i;
    }

    impossible("greek cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_celtic_helmet()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "celtic helmet"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "kel'tskaya shlem"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "seltik dubulg'a"))
	    return i;
    }

    impossible("celtic helmet not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_english_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "english gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "angliyskiye perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ingliz tili qo'lqop"))
	    return i;
    }

    impossible("english gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_korean_sandals()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "korean sandals"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "koreyskiye sandalii"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "janubiy koreyaning kavushlari"))
	    return i;
    }

    impossible("korean sandals not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_octarine_robe()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "octarine robe"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vos'moy tsvet khalata"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sakkizinchi rang to'n"))
	    return i;
    }

    impossible("octarine robe not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_chinese_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chinese cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "kitayskiy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "xitoy plash"))
	    return i;
    }

    impossible("chinese cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_polyform_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "polyform cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sopolimer forma plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "belgigacha bo'lgan poli shakli plash"))
	    return i;
    }

    impossible("polyform cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_absorbing_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "absorbing cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pogloshchayushchiy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yutucu plash"))
	    return i;
    }

    impossible("absorbing cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_birthcloth()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "birthcloth"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rozhdeniye tkan'"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tug'ilgan mato"))
	    return i;
    }

    impossible("birthcloth not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_poke_mongo_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "poke mongo cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sovat' mongo plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "soktudun mongo plash"))
	    return i;
    }

    impossible("poke mongo cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_levuntation_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "levuntation cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "levitatsii plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "havo rido kiygan suzadi"))
	    return i;
    }

    impossible("levuntation cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_quicktravel_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "quicktravel cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bystryy plashch puteshestviya"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "tez safar plash"))
	    return i;
    }

    impossible("quicktravel cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_angband_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "angband cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plashch sredizem'ye krepost'"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "o'rta yer qal'a plash"))
	    return i;
    }

    impossible("angband cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_anorexia_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "anorexia cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yedyat plashch rasstroystvo"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "eb buzilishi plash"))
	    return i;
    }

    impossible("anorexia cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_dnethack_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dnethack cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "podzemeliy i vnezemnyye plashch vzlomat'"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "hamzindon va dunyo bo'lmagan doirasi so'yish plash"))
	    return i;
    }

    impossible("dnethack cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_team_splat_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "team splat cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "vosklitsatel'nyy znak plashch komanda"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "jamoasi xavfsizlik plash"))
	    return i;
    }

    impossible("team splat cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_eldritch_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "eldritch cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sverkh'yestestvennyy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "aql bovar qilmaydigan plash"))
	    return i;
    }

    impossible("eldritch cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_erotic_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "erotic boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "eroticheskiye sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "erotik chizilmasin"))
	    return i;
    }

    impossible("erotic boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_secret_helmet()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "secret helmet"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sekret shlem"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yashirin dubulg'a"))
	    return i;
    }

    impossible("secret helmet not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_difficult_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "difficult cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "trudnyy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qiyin plash"))
	    return i;
    }

    impossible("difficult cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_velvet_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velvet gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "barkhatnyye perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "baxmal qo'lqop"))
	    return i;
    }

    impossible("velvet gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_sputa_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sputa boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mokrota sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sputa chizilmasin"))
	    return i;
    }

    impossible("sputa boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_formula_one_helmet()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "formula one helmet"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "formula odin shlem"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "formula bir zarbdan"))
	    return i;
    }

    impossible("formula one helmet not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_excrement_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "excrement cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ekskrementy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "chiqindi plash"))
	    return i;
    }

    impossible("excrement cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_racer_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "racer gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gonshchik perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "poygachi qo'lqop"))
	    return i;
    }

    impossible("racer gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_turbo_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "turbo boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "turbo sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qidiruvi va turbo chizilmasin"))
	    return i;
    }

    impossible("turbo boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_guild_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "guild cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "gil'dii plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "birlik plash"))
	    return i;
    }

    impossible("guild cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_shitty_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shitty gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "der'movyye perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "boktan qo'lqop"))
	    return i;
    }

    impossible("shitty gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_foundry_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "foundry cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "liteynyy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "quyish plash"))
	    return i;
    }

    impossible("foundry cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_spellsucking_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "spellsucking cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "plashch zaklinaniy"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "so'rib imlo plash"))
	    return i;
    }

    impossible("spellsucking cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_storm_coat()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "storm coat"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "shtorm"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bo'ron palto"))
	    return i;
    }

    impossible("storm coat not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_fleeceling_cloak()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fleeceling cloak"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pushistyy plashch"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "serjunrangli plash"))
	    return i;
    }

    impossible("fleeceling cloak not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_princess_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "princess gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "perchatki printsessy"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "malika qo'lqop"))
	    return i;
    }

    impossible("princess gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_uncanny_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "uncanny gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sverkh''yestestvennyye perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "dahshatli qo'lqop"))
	    return i;
    }

    impossible("uncanny gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_slaying_gloves()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "slaying gloves"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ubiystvennyye perchatki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "o'ldirish qo'lqop"))
	    return i;
    }

    impossible("slaying gloves not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_blue_sneakers()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "blue sneakers"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "siniye krossovki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "ko'k shippak"))
	    return i;
    }

    impossible("blue sneakers not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_femmy_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "femmy boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "zhenskiye sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "nazokat etigi"))
	    return i;
    }

    impossible("femmy boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_red_sneakers()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "red sneakers"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "krasnyye krossovki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qizil shippak"))
	    return i;
    }

    impossible("red sneakers not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_yellow_sneakers()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yellow sneakers"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "zheltyye krossovki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sariq shippak"))
	    return i;
    }

    impossible("yellow sneakers not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_pink_sneakers()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pink sneakers"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rozovyye krossovki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "pushti shippak"))
	    return i;
    }

    impossible("pink sneakers not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_calf_leather_sandals()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "calf-leather sandals"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sandalii iz telyach'yey kozhi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "buzoq-charm kavushlari"))
	    return i;
    }

    impossible("calf-leather sandals not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_velcro_sandals()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "velcro sandals"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "sandalii na lipuchkakh"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "cirt kavushlari"))
	    return i;
    }

    impossible("velcro sandals not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_buffalo_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "buffalo boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "buyvolovyye sapogi"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qo'tos botlarni"))
	    return i;
    }

    impossible("buffalo boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_heroine_mocassins()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "heroine mocassins"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "mokasiny dlya geroini"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qahramoni mokasen"))
	    return i;
    }

    impossible("heroine mocassins not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_lolita_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "lolita boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "botinki s lolitoy"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "bosh ketish etigi"))
	    return i;
    }

    impossible("lolita boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_fetish_heels()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "fetish heels"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "idol kabluki"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "but poshnalar"))
	    return i;
    }

    impossible("fetish heels not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_weapon_light_boots()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "weapon light boots"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "legkiye botinki dlya oruzhiya"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "qurol engil etigi"))
	    return i;
    }

    impossible("weapon light boots not found?");
    return -1;	/* not 0, or caller would try again each move */
}

int
find_rubynus_helmet()
{
    register int i;
    register const char *s;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rubynus helmet"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "rubinovyy shlem"))
	    return i;
	if ((s = OBJ_DESCR(objects[i])) != 0 && !strcmp(s, "yoqut asosiy dubulg'a"))
	    return i;
    }

    impossible("rubynus helmet not found?");
    return -1;	/* not 0, or caller would try again each move */
}

void
oinit()			/* level dependent initialization */
{
	setgemprobs(&u.uz);
}

void
savenames(fd, mode)
int fd, mode;
{
	register int i;
	unsigned int len;

	if (perform_bwrite(mode)) {
	    bwrite(fd, (void *)bases, sizeof bases);
	    bwrite(fd, (void *)disco, sizeof disco);
	    bwrite(fd, (void *)objects,
		   sizeof(struct objclass) * NUM_OBJECTS);
	}
	/* as long as we use only one version of Hack we
	   need not save oc_name and oc_descr, but we must save
	   oc_uname for all objects */
	for (i = 0; i < NUM_OBJECTS; i++)
	    if (objects[i].oc_uname) {
		if (perform_bwrite(mode)) {
		    len = strlen(objects[i].oc_uname)+1;
		    bwrite(fd, (void *)&len, sizeof len);
		    bwrite(fd, (void *)objects[i].oc_uname, len);
		}
		if (release_data(mode)) {
		    free((void *)objects[i].oc_uname);
		    objects[i].oc_uname = 0;
		}
	    }
}

void
restnames(fd)
register int fd;
{
	register int i;
	unsigned int len;

	mread(fd, (void *) bases, sizeof bases);
	mread(fd, (void *) disco, sizeof disco);
	mread(fd, (void *) objects, sizeof(struct objclass) * NUM_OBJECTS);
	for (i = 0; i < NUM_OBJECTS; i++)
	    if (objects[i].oc_uname) {
		mread(fd, (void *) &len, sizeof len);
		objects[i].oc_uname = (char *) alloc(len);
		mread(fd, (void *)objects[i].oc_uname, len);
	    }
#ifdef USE_TILES
	shuffle_tiles();
#endif
#ifdef PROXY_GRAPHICS
	shuffle_proxy_glyphs();
#endif
}

void
discover_object(oindx, mark_as_known, credit_hero)
register int oindx;
boolean mark_as_known;
boolean credit_hero;
{
	/* KMH -- If we are hallucinating, we aren't sure of the object description */
	if (Hallucination) return;

    if (!objects[oindx].oc_name_known) {
	register int dindx, acls = objects[oindx].oc_class;

	/* Loop thru disco[] 'til we find the target (which may have been
	   uname'd) or the next open slot; one or the other will be found
	   before we reach the next class...
	 */
	for (dindx = bases[acls]; disco[dindx] != 0; dindx++)
	    if (disco[dindx] == oindx) break;
	disco[dindx] = oindx;

	if (mark_as_known) {
	    objects[oindx].oc_name_known = 1;
	    if (credit_hero) exercise(A_WIS, TRUE);
	}
	if (moves > 1L) update_inventory();
    }
}

/* if a class name has been cleared, we may need to purge it from disco[] */
void
undiscover_object(oindx)
register int oindx;
{
    if (!objects[oindx].oc_name_known) {
	register int dindx, acls = objects[oindx].oc_class;
	register boolean found = FALSE;

	/* find the object; shift those behind it forward one slot */
	for (dindx = bases[acls];
	      dindx < NUM_OBJECTS && disco[dindx] != 0
		&& objects[dindx].oc_class == acls; dindx++)
	    if (found)
		disco[dindx-1] = disco[dindx];
	    else if (disco[dindx] == oindx)
		found = TRUE;

	/* clear last slot */
	if (found) disco[dindx-1] = 0;
	else impossible("named object not in disco");
	update_inventory();
    }
}

STATIC_OVL boolean
interesting_to_discover(i)
register int i;
{
	/* Pre-discovered objects are now printed with a '*' */
    return((boolean)(objects[i].oc_uname != (char *)0 ||
	    (objects[i].oc_name_known && OBJ_DESCR(objects[i]) != (char *)0)));
}

/* items that should stand out once they're known */
static short uniq_objs[] = {
	AMULET_OF_YENDOR,
	SPE_BOOK_OF_THE_DEAD,
	CANDELABRUM_OF_INVOCATION,
	BELL_OF_OPENING,
};

int
dodiscovered()				/* free after Robert Viduya */
{
    register int i, dis;
    int	ct = 0;
    char *s, oclass, prev_class, classes[MAXOCLASSES];
    char buf[BUFSZ];    /* WAC */
    winid tmpwin;
    anything any;
    menu_item *selected;

    if (UninformationProblem || u.uprops[UNINFORMATION].extrinsic || have_uninformationstone() || (uarms && uarms->oartifact == ART_FIVE_STAR_PARTY) ) {
	    pline("You've discovered absolutely everything, yet you still don't know what do to with it.");
	    return 0;
    }

    tmpwin = create_nhwindow(NHW_MENU);
    /*
     * Use the add_menu() interface so that eg., GTK windowing port
     * can display the relevant glyphs --ALI
     */
    start_menu(tmpwin);

    any.a_void = 0;
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, "Discoveries",
      MENU_UNSELECTED);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "",
      MENU_UNSELECTED);

    /* gather "unique objects" into a pseudo-class; note that they'll
       also be displayed individually within their regular class */
    for (i = dis = 0; i < SIZE(uniq_objs); i++)
	if (objects[uniq_objs[i]].oc_name_known) {
	    if (!dis++)
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
		  "Unique Items", MENU_UNSELECTED);
		sprintf(buf, "  %s", OBJ_NAME(objects[uniq_objs[i]]));
	    add_menu(tmpwin, objnum_to_glyph(uniq_objs[i]), &any,
	      0, 0, ATR_NONE, buf, MENU_UNSELECTED);
	    ++ct;
	}
    /* display any known artifacts as another pseudo-class */

	/* temporarily disabled by Amy because of crashiness. Will be re-enabled if I figure out why artifacts with
	 * an ID number greater than 127 are segfaulting here, but for the time being, please use an artifact spoiler. */
    ct += disp_artifact_discoveries(tmpwin);

    /* several classes are omitted from packorder; one is of interest here */
    strcpy(classes, flags.inv_order);
    if (!index(classes, VENOM_CLASS)) {
	s = eos(classes);
	*s++ = VENOM_CLASS;
	*s = '\0';
    }

    for (s = classes; *s; s++) {
	oclass = *s;
	prev_class = oclass + 1;	/* forced different from oclass */
	for (i = bases[(int)oclass];
	     i < NUM_OBJECTS && objects[i].oc_class == oclass; i++) {
	    if ((dis = disco[i]) && interesting_to_discover(dis)) {
		ct++;
		if (oclass != prev_class) {
		    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
		      let_to_name(oclass, FALSE, FALSE), MENU_UNSELECTED);
		    prev_class = oclass;
		}
		sprintf(buf, "%s %s",(objects[dis].oc_pre_discovered ? "*" : " "),
				obj_typename(dis));
		add_menu(tmpwin, objnum_to_glyph(dis), &any, 0, 0,
		  ATR_NONE, buf, MENU_UNSELECTED);
	    }
	}
    }
    end_menu(tmpwin, (char *) 0);
    if (ct == 0) {
	You("haven't discovered anything yet...");
    } else
	(void) select_menu(tmpwin, PICK_NONE, &selected);
    destroy_nhwindow(tmpwin);

    return 0;
}

#ifdef DUMP_LOG
void 
dump_discoveries()
{

    register int i, dis;
    int	ct = 0;
    char *s, oclass, prev_class, classes[MAXOCLASSES];
    char buf[BUFSZ];    /* WAC */
    winid tmpwin;

    dump("", "Discoveries");

    /* gather "unique objects" into a pseudo-class; note that they'll
       also be displayed individually within their regular class */
    for (i = dis = 0; i < SIZE(uniq_objs); i++)
	if (objects[uniq_objs[i]].oc_name_known) {
	    if (!dis++)
		    dump("", "  Unique Items");
		sprintf(buf, "  %s", OBJ_NAME(objects[uniq_objs[i]]));
		dump("  ", buf);
	    ++ct;
	}

    dump_artifact_discoveries();

    /* several classes are omitted from packorder; one is of interest here */
    strcpy(classes, flags.inv_order);
    if (!index(classes, VENOM_CLASS)) {
	s = eos(classes);
	*s++ = VENOM_CLASS;
	*s = '\0';
    }

    for (s = classes; *s; s++) {
	oclass = *s;
	prev_class = oclass + 1;	/* forced different from oclass */
	for (i = bases[(int)oclass];
	     i < NUM_OBJECTS && objects[i].oc_class == oclass; i++) {
	    if ((dis = disco[i]) && interesting_to_discover(dis)) {
		ct++;
		if (oclass != prev_class) {

		    sprintf(buf, "  %s", let_to_name(oclass, FALSE, FALSE));
		    dump("", buf);

		    prev_class = oclass;
		}
		sprintf(buf, "%s %s",(objects[dis].oc_pre_discovered ? "*" : " "),
				obj_typename(dis));
		    dump("  ", buf);
	    }
	}
    }
    if (ct == 0) {
	dump("", "You haven't discovered anything at all...");
    }
    dump("", "");

} /* dump_discoveries */
#endif

/*o_init.c*/

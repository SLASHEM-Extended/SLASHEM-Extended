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
		color = objects[j].oc_appearindex;
		objects[j].oc_appearindex = objects[i].oc_appearindex;
		objects[i].oc_appearindex = color;

		/* shuffle material */
		if (domaterial) {
			color = objects[j].oc_material;
			objects[j].oc_material = objects[i].oc_material;
			objects[i].oc_material = color;
		}
	}
}

/* for matrayser race: shuffle materials per item class --Amy */
void
matraysershuffle()
{
	int first, last, oclass;
	int i, lastgood, k, savemat;

	for (oclass = 1; oclass < MAXOCLASSES; oclass++) {
		first = bases[oclass];
		last = first+1;
		while (last < NUM_OBJECTS && objects[last].oc_class == oclass)
			last++;
		lastgood = last-1;
		/* lastgood stays constant from here on */

		if (objects[lastgood].oc_class != oclass) {
			impossible("matraysershuffle() ERROR: object %d is not object class %d!", lastgood, oclass);
		}

		/* shuffle(first, lastgood, TRUE);
		 * first = o_low, lastgood = o_high */

		for (i = first; i <= lastgood; i++) {
			k = i + rn2(lastgood - i + 1);

			savemat = objects[i].oc_material;
			objects[i].oc_material = objects[k].oc_material;
			objects[k].oc_material = savemat;
		}

	}

}

/* A function that assigns random materials to a few objects, called once at game start. --Amy */
void
randommaterials()
{
	register int objtomodify;
	while (rn2(25)) { /* to randomize the # of objects affected --Amy */
		objtomodify = -1;
		while ( (objtomodify == -1) || (objtomodify == GOLD_PIECE) || (objtomodify == STRANGE_OBJECT) || (objtomodify == AMULET_OF_YENDOR) || (objtomodify == CANDELABRUM_OF_INVOCATION) || (objtomodify == BELL_OF_OPENING) || (objtomodify == SPE_BOOK_OF_THE_DEAD) || (objects[objtomodify].oc_prob < 1) ) objtomodify = rn2(NUM_OBJECTS);
		/* we roll until we get an item that can be randomly generated and is not a gold piece or other weird thing --Amy */

		objects[objtomodify].oc_material = rn2(LASTMATERIAL + 1); /* allow every material, including 0 (mysterious) */
		/*if (wizard) pline("Modifying material of object %d", objtomodify);*/ /* debug message */

	}

	if (!rn2(5)) {

		while (rn2(25)) {
			objtomodify = -1;
			while ( (objtomodify == -1) || (objtomodify == GOLD_PIECE) || (objtomodify == STRANGE_OBJECT) || (objtomodify == AMULET_OF_YENDOR) || (objtomodify == CANDELABRUM_OF_INVOCATION) || (objtomodify == BELL_OF_OPENING) || (objtomodify == SPE_BOOK_OF_THE_DEAD) || (objects[objtomodify].oc_prob < 1) ) objtomodify = rn2(NUM_OBJECTS);

			objects[objtomodify].oc_material = rn2(LASTMATERIAL + 1); /* allow every material, including 0 (mysterious) */

		}

	}

	if (!rn2(20)) {

		while (rn2(60)) {
			objtomodify = -1;
			while ( (objtomodify == -1) || (objtomodify == GOLD_PIECE) || (objtomodify == STRANGE_OBJECT) || (objtomodify == AMULET_OF_YENDOR) || (objtomodify == CANDELABRUM_OF_INVOCATION) || (objtomodify == BELL_OF_OPENING) || (objtomodify == SPE_BOOK_OF_THE_DEAD) || (objects[objtomodify].oc_prob < 1) ) objtomodify = rn2(NUM_OBJECTS);

			objects[objtomodify].oc_material = rn2(LASTMATERIAL + 1); /* allow every material, including 0 (mysterious) */

		}

	}

	if (!rn2(100)) {

		while (rn2(240)) {
			objtomodify = -1;
			while ( (objtomodify == -1) || (objtomodify == GOLD_PIECE) || (objtomodify == STRANGE_OBJECT) || (objtomodify == AMULET_OF_YENDOR) || (objtomodify == CANDELABRUM_OF_INVOCATION) || (objtomodify == BELL_OF_OPENING) || (objtomodify == SPE_BOOK_OF_THE_DEAD) || (objects[objtomodify].oc_prob < 1) ) objtomodify = rn2(NUM_OBJECTS);

			objects[objtomodify].oc_material = rn2(LASTMATERIAL + 1); /* allow every material, including 0 (mysterious) */

		}

	}

}

void
initobjectsamnesia()
{
	/* discovered by Tariru: when the randomized objects are being re-initialized, and you're wearing someting
	 * like a ring of random effects, you don't lose the property that it used to give, even if you save and load.
	 * Therefore we'll use this ugly hack that takes the item off, re-initializes its effects, and puts it back on --Amy */

	register struct obj *otmp, *otmp2, *otmp3, *otmp4, *otmp5, *otmp6, *otmp7, *otmp8, *otmp9, *otmp10, *otmp11;
	boolean hasamulet, hasleftring, hasrightring, hasshirt, hascloak, hasarmor, hasboots, hasgloves, hashelmet, hasshield, hasimplant;

	u.cnd_appearanceshufflingcount++;

	if (uamul) {
		hasamulet = TRUE;
		otmp = uamul;
		setworn((struct obj *)0, W_AMUL);
	} else hasamulet = FALSE;
	if (uleft) {
		hasleftring = TRUE;
		otmp2 = uleft;
		setworn((struct obj *)0, W_RINGL);
	} else hasleftring = FALSE;
	if (uright) {
		hasrightring = TRUE;
		otmp3 = uright;
		setworn((struct obj *)0, W_RINGR);
	} else hasrightring = FALSE;
	if (uarmu) {
		hasshirt = TRUE;
		otmp4 = uarmu;
		setworn((struct obj *)0, W_ARMU);
	} else hasshirt = FALSE;
	if (uarmc) {
		hascloak = TRUE;
		otmp5 = uarmc;
		setworn((struct obj *)0, W_ARMC);
	} else hascloak = FALSE;
	if (uarm) {
		hasarmor = TRUE;
		otmp6 = uarm;
		setworn((struct obj *)0, W_ARM);
	} else hasarmor = FALSE;
	if (uarmf) {
		hasboots = TRUE;
		otmp7 = uarmf;
		setworn((struct obj *)0, W_ARMF);
	} else hasboots = FALSE;
	if (uarmg) {
		hasgloves = TRUE;
		otmp8 = uarmg;
		setworn((struct obj *)0, W_ARMG);
	} else hasgloves = FALSE;
	if (uarmh) {
		hashelmet = TRUE;
		otmp9 = uarmh;
		setworn((struct obj *)0, W_ARMH);
	} else hashelmet = FALSE;
	if (uarms) {
		hasshield = TRUE;
		otmp10 = uarms;
		setworn((struct obj *)0, W_ARMS);
	} else hasshield = FALSE;
	if (uimplant) {
		hasimplant = TRUE;
		otmp11 = uimplant;
		setworn((struct obj *)0, W_IMPLANT);
	} else hasimplant = FALSE;

	init_objects(FALSE);

	if (hasamulet) {
		setworn(otmp, W_AMUL);
	}
	if (hasleftring) {
		setworn(otmp2, W_RINGL);
	}
	if (hasrightring) {
		setworn(otmp3, W_RINGR);
	}
	if (hasshirt) {
		setworn(otmp4, W_ARMU);
	}
	if (hascloak) {
		setworn(otmp5, W_ARMC);
	}
	if (hasarmor) {
		setworn(otmp6, W_ARM);
	}
	if (hasboots) {
		setworn(otmp7, W_ARMF);
	}
	if (hasgloves) {
		setworn(otmp8, W_ARMG);
	}
	if (hashelmet) {
		setworn(otmp9, W_ARMH);
	}
	if (hasshield) {
		setworn(otmp10, W_ARMS);
	}
	if (hasimplant) {
		setworn(otmp11, W_IMPLANT);
	}

	/* random appearances have now been shuffled - please re-align randarts that depend on them --Amy
	 * it's important that every effect that shuffles appearances will run this code, including e.g. being a kurwa */
	init_appearance_randarts();
}

void
init_objects(descrinit)
boolean descrinit;
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
	if (descrinit) {
		for (i = 0; i < NUM_OBJECTS; i++)
			objects[i].oc_name_idx = objects[i].oc_descr_idx = i;
	}
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
			    objects[i].oc_prob = (100000+i-first)/(last-first);
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

	objects[EXCITING_ARMOR].oc_oprop = randenchantment();

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

	objects[MAIDRAGON_SCALE_MAIL].oc_oprop = randfemienchantment();
	objects[MAIDRAGON_SCALES].oc_oprop = randfemienchantment();
	objects[MAIDRAGON_SCALE_SHIELD].oc_oprop = randfemienchantment();
	objects[ROBE_OF_FEMINISM].oc_oprop = randfemienchantment();
	objects[FEMINISM_CLOAK].oc_oprop = randfemienchantment();
	objects[FEMINISM_HELMET].oc_oprop = randfemienchantment();
	objects[FEMALE_GLOVES].oc_oprop = randfemienchantment();
	objects[FEMININE_STILETTOS].oc_oprop = randfemienchantment();
	objects[RIN_FEMINISM].oc_oprop = randfemienchantment();
	objects[AMULET_OF_FEMINISM].oc_oprop = randfemienchantment();

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
	objects[DUMMY_CLOAK_W].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_X].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_Y].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_Z].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AB].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AC].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AD].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AF].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AG].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AH].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AI].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AJ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AK].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AL].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AM].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AN].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AO].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AP].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AQ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AR].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AS].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AT].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AU].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AV].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AW].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_CLOAK_AX].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

	objects[RANDOM_JACKET].a_ac = rnd(10);

	objects[IMPLANT_OF_ABSORPTION].a_ac = rnd(10);
	objects[IMPLANT_OF_PUNCTURING].a_ac = rnd(10);
	objects[IMPLANT_OF_CRAFTSMANSHIP].a_ac = rnd(10);
	objects[IMPLANT_OF_PRECISION].a_ac = rnd(10);
	objects[IMPLANT_OF_VILENESS].a_ac = rnd(10);
	objects[IMPLANT_OF_REMEDY].a_ac = rnd(10);
	objects[IMPLANT_OF_STOICISM].a_ac = rnd(10);
	objects[IMPLANT_OF_AVARICE].a_ac = rnd(10);
	objects[IMPLANT_OF_CREMATION].a_ac = rnd(10);
	objects[IMPLANT_OF_SEARING].a_ac = rnd(10);
	objects[IMPLANT_OF_REDEMPTION].a_ac = rnd(10);
	objects[IMPLANT_OF_EROSION].a_ac = rnd(10);
	objects[IMPLANT_OF_JOY].a_ac = rnd(10);
	objects[IMPLANT_OF_CRUELTY].a_ac = rnd(10);
	objects[IMPLANT_OF_BADNESS].a_ac = rnd(10);
	objects[IMPLANT_OF_PROPOGATION].a_ac = rnd(10);
	objects[IMPLANT_OF_PASSION].a_ac = rnd(10);
	objects[IMPLANT_OF_WINTER].a_ac = rnd(10);
	objects[IMPLANT_OF_ACCELERATION].a_ac = rnd(10);
	objects[IMPLANT_OF_PROSPERITY].a_ac = rnd(10);

	objects[IMPLANT_OF_QUEEB_BUTT].a_ac = rnd(10);
	objects[IMPLANT_OF_BEHOLDING].a_ac = rnd(10);
	objects[IMPLANT_OF_DA_GUSHA].a_ac = rnd(10);
	objects[IMPLANT_OF_FINGER_POINTING].a_ac = rnd(10);

	objects[IMPLANT_OF_QUICKENING].a_ac = rnd(8);
	objects[IMPLANT_OF_KARMA].a_ac = rnd(8);
	objects[IMPLANT_OF_FERVOR].a_ac = rnd(8);
	objects[IMPLANT_OF_TRANSCENDENCE].a_ac = rnd(8);
	objects[IMPLANT_OF_ELUSION].a_ac = rnd(8);
	objects[IMPLANT_OF_STATURE].a_ac = rnd(8);
	objects[IMPLANT_OF_SUFFERING].a_ac = rnd(8);
	objects[IMPLANT_OF_BADASS].a_ac = rnd(8);
	objects[IMPLANT_OF_FAST_REPAIR].a_ac = rnd(8);
	objects[IMPLANT_OF_PILFERING].a_ac = rnd(8);
	objects[IMPLANT_OF_REPLENISHING].a_ac = rnd(8);
	objects[IMPLANT_OF_HONOR].a_ac = rnd(8);
	objects[IMPLANT_OF_CONTROL].a_ac = rnd(8);
	objects[IMPLANT_OF_CLUMSINESS].a_ac = rnd(8);
	objects[IMPLANT_OF_INSULATION].a_ac = rnd(8);
	objects[IMPLANT_OF_FRAILTY].a_ac = rnd(8);
	objects[IMPLANT_OF_KNOWLEDGE].a_ac = rnd(8);
	objects[IMPLANT_OF_VENGEANCE].a_ac = rnd(8);
	objects[IMPLANT_OF_BLISS].a_ac = rnd(8);
	objects[IMPLANT_OF_BLITZEN].a_ac = rnd(8);

	objects[IMPLANT_OF_TOTAL_NONSENSE].a_ac = rnd(8);
	objects[IMPLANT_OF_BIG_OL].a_ac = rnd(8);
	objects[IMPLANT_OF_ANAL_RETENTION].a_ac = rnd(8);
	objects[IMPLANT_OF_GALVANIZATION].a_ac = rnd(8);

	objects[IMPLANT_OF_IRE].a_ac = rnd(5);
	objects[IMPLANT_OF_MALICE].a_ac = rnd(5);
	objects[IMPLANT_OF_AGES].a_ac = rnd(5);
	objects[IMPLANT_OF_SUSTENANCE].a_ac = rnd(5);
	objects[IMPLANT_OF_TRUTH].a_ac = rnd(5);
	objects[IMPLANT_OF_REMORSE].a_ac = rnd(5);
	objects[IMPLANT_OF_GRACE].a_ac = rnd(5);
	objects[IMPLANT_OF_WASTE].a_ac = rnd(5);
	objects[IMPLANT_OF_COMBAT].a_ac = rnd(5);
	objects[IMPLANT_OF_FAITH].a_ac = rnd(5);
	objects[IMPLANT_OF_DISPATCH].a_ac = rnd(5);
	objects[IMPLANT_OF_DREAD].a_ac = rnd(5);
	objects[IMPLANT_OF_VITA].a_ac = rnd(5);
	objects[IMPLANT_OF_MAGGOTS].a_ac = rnd(5);
	objects[IMPLANT_OF_BEAUTY].a_ac = rnd(5);
	objects[IMPLANT_OF_DUSK].a_ac = rnd(5);
	objects[IMPLANT_OF_TRIBUTE].a_ac = rnd(5);
	objects[IMPLANT_OF_INERTIA].a_ac = rnd(5);
	objects[IMPLANT_OF_SWEETNESS].a_ac = rnd(5);
	objects[IMPLANT_OF_IRRIGATION].a_ac = rnd(5);
	objects[IMPLANT_OF_TWILIGHT].a_ac = rnd(5);
	objects[IMPLANT_OF_MEMORY].a_ac = rnd(5);
	objects[IMPLANT_OF_LOVE].a_ac = rnd(5);
	objects[IMPLANT_OF_VINES].a_ac = rnd(5);
	objects[IMPLANT_OF_ANIMA].a_ac = rnd(5);
	objects[IMPLANT_OF_LINES].a_ac = rnd(5);
	objects[IMPLANT_OF_THAWING].a_ac = rnd(5);
	objects[IMPLANT_OF_DESIRE].a_ac = rnd(5);
	objects[IMPLANT_OF_PAIN].a_ac = rnd(5);
	objects[IMPLANT_OF_DARING].a_ac = rnd(5);
	objects[IMPLANT_OF_CORRUPTION].a_ac = rnd(5);
	objects[IMPLANT_OF_EVISCERATION].a_ac = rnd(5);
	objects[IMPLANT_OF_TRAVELING].a_ac = rnd(5);
	objects[IMPLANT_OF_CHEATING].a_ac = rnd(5);
	objects[IMPLANT_OF_ANTHRAX].a_ac = rnd(5);
	objects[IMPLANT_OF_ATTRITION].a_ac = rnd(5);
	objects[IMPLANT_OF_HACKING].a_ac = rnd(5);
	objects[IMPLANT_OF_PROSPERING].a_ac = rnd(5);
	objects[IMPLANT_OF_VALHALLA].a_ac = rnd(5);
	objects[IMPLANT_OF_DECEPTION].a_ac = rnd(5);
	objects[IMPLANT_OF_BUTCHERY].a_ac = rnd(5);
	objects[IMPLANT_OF_BLIZZARD].a_ac = rnd(5);
	objects[IMPLANT_OF_TERROR].a_ac = rnd(5);
	objects[IMPLANT_OF_DAWN].a_ac = rnd(5);
	objects[IMPLANT_OF_BILE].a_ac = rnd(5);
	objects[IMPLANT_OF_CREDIT].a_ac = rnd(5);
	objects[IMPLANT_OF_QUOTA].a_ac = rnd(5);
	objects[IMPLANT_OF_VIRILITY].a_ac = rnd(5);
	objects[IMPLANT_OF_VANILLA].a_ac = rnd(5);
	objects[IMPLANT_OF_HOPE].a_ac = rnd(5);
	objects[IMPLANT_OF_ABRASION].a_ac = rnd(5);
	objects[IMPLANT_OF_OSMOSIS].a_ac = rnd(5);
	objects[IMPLANT_OF_NIRVANA].a_ac = rnd(5);
	objects[IMPLANT_OF_ENVY].a_ac = rnd(5);
	objects[IMPLANT_OF_ENNUI].a_ac = rnd(5);
	objects[IMPLANT_OF_IMPOSSIBILITY].a_ac = rnd(5);
	objects[IMPLANT_OF_ADMIRATION].a_ac = rnd(5);
	objects[IMPLANT_OF_SUNLIGHT].a_ac = rnd(5);
	objects[IMPLANT_OF_TSUNAMI].a_ac = rnd(5);
	objects[IMPLANT_OF_FREEDOM].a_ac = rnd(5);

	objects[IMPLANT_OF_TEN_THOUSAND_THINGS].a_ac = rnd(5);
	objects[IMPLANT_OF_TOSSING].a_ac = rnd(5);
	objects[IMPLANT_OF_HEAVY_LUBRICATION].a_ac = rnd(5);
	objects[IMPLANT_OF_MISANTHROPHY].a_ac = rnd(5);
	objects[IMPLANT_OF_CRAP].a_ac = rnd(5);
	objects[IMPLANT_OF_KING_BEEFCAKE].a_ac = rnd(5);
	objects[IMPLANT_OF_PUMPING].a_ac = rnd(5);
	objects[IMPLANT_OF_WANNABE].a_ac = rnd(5);
	objects[IMPLANT_OF_YOUR_MOMMA].a_ac = rnd(5);
	objects[IMPLANT_OF_SWALLOWING].a_ac = rnd(5);
	objects[IMPLANT_OF_MENSTRATING].a_ac = rnd(5);
	objects[IMPLANT_OF_ENFORCING].a_ac = rnd(5);

	objects[IMPLANT_OF_SERENITY].a_ac = rnd(9);
	objects[IMPLANT_OF_GILGAMESH].a_ac = rnd(9);
	objects[IMPLANT_OF_THE_PLANES].a_ac = rnd(9);
	objects[IMPLANT_OF_RECKONING].a_ac = rnd(9);
	objects[IMPLANT_OF_RESPECT].a_ac = rnd(9);
	objects[IMPLANT_OF_EXCELLENCE].a_ac = rnd(9);
	objects[IMPLANT_OF_PUNITION].a_ac = rnd(9);
	objects[IMPLANT_OF_HUBRIS].a_ac = rnd(9);
	objects[IMPLANT_OF_OMEN].a_ac = rnd(9);
	objects[IMPLANT_OF_CARNAGE].a_ac = rnd(9);

	objects[IMPLANT_OF_LEGEND].a_ac = rnd(9);
	objects[IMPLANT_OF_VIRTUE].a_ac = rnd(9);

	objects[IMPLANT_OF_FATE].a_ac = rnd(8);
	objects[IMPLANT_OF_THE_SEA_LION].a_ac = rnd(8);
	objects[IMPLANT_OF_JUSTICE].a_ac = rnd(8);
	objects[IMPLANT_OF_DISASTER].a_ac = rnd(8);
	objects[IMPLANT_OF_REDRESS].a_ac = rnd(8);
	objects[IMPLANT_OF_QUARRELS].a_ac = rnd(8);
	objects[IMPLANT_OF_PARANOIA].a_ac = rnd(8);
	objects[IMPLANT_OF_TORMENT].a_ac = rnd(8);
	objects[IMPLANT_OF_SWIFT_DEFEAT].a_ac = rnd(8);
	objects[IMPLANT_OF_INTEGRITY].a_ac = rnd(8);
	objects[IMPLANT_OF_CHASTITY].a_ac = rnd(8);
	objects[IMPLANT_OF_BINDING].a_ac = rnd(8);
	objects[IMPLANT_OF_MOLLIFICATION].a_ac = rnd(8);
	objects[IMPLANT_OF_FULL_HEALING].a_ac = rnd(8);
	objects[IMPLANT_OF_DESTRUCTION].a_ac = rnd(8);
	objects[IMPLANT_OF_FRIENDSHIP].a_ac = rnd(8);
	objects[IMPLANT_OF_SATIATION].a_ac = rnd(8);
	objects[IMPLANT_OF_PRESERVATION].a_ac = rnd(8);
	objects[IMPLANT_OF_QUICK_DRAWING].a_ac = rnd(8);
	objects[IMPLANT_OF_WAND_CHARGING].a_ac = rnd(8);

	objects[IMPLANT_OF_SORROW].a_ac = rnd(8);
	objects[IMPLANT_OF_CLEAR_THINKING].a_ac = rnd(8);
	objects[IMPLANT_OF_MIND_BLASTING].a_ac = rnd(8);
	objects[IMPLANT_OF_THE_OCTOPUS].a_ac = rnd(8);

	objects[IMPLANT_OF_FATE].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_THE_SEA_LION].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_JUSTICE].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_DISASTER].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_REDRESS].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_QUARRELS].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_PARANOIA].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_TORMENT].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_SWIFT_DEFEAT].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_INTEGRITY].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_CHASTITY].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_BINDING].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_MOLLIFICATION].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_FULL_HEALING].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_DESTRUCTION].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_FRIENDSHIP].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_SATIATION].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_PRESERVATION].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_QUICK_DRAWING].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_WAND_CHARGING].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);

	objects[IMPLANT_OF_SORROW].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_CLEAR_THINKING].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_MIND_BLASTING].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);
	objects[IMPLANT_OF_THE_OCTOPUS].a_can = !rn2(5) ? rn1(6, 4) : rnd(3);

	objects[IMPLANT_OF_QUICKENING].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_KARMA].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_FERVOR].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_TRANSCENDENCE].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_ELUSION].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_STATURE].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_SUFFERING].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_BADASS].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_FAST_REPAIR].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_PILFERING].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_REPLENISHING].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_HONOR].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_CONTROL].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_CLUMSINESS].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_INSULATION].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_FRAILTY].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_KNOWLEDGE].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_VENGEANCE].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_BLISS].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_BLITZEN].oc_oprop = randnastyenchantment();

	objects[IMPLANT_OF_TOTAL_NONSENSE].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_BIG_OL].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_ANAL_RETENTION].oc_oprop = randnastyenchantment();
	objects[IMPLANT_OF_GALVANIZATION].oc_oprop = randnastyenchantment();

	objects[IMPLANT_OF_SERENITY].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_GILGAMESH].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_THE_PLANES].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_RECKONING].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_RESPECT].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_EXCELLENCE].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_PUNITION].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_HUBRIS].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_OMEN].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_CARNAGE].oc_oprop = randfemienchantment();

	objects[IMPLANT_OF_LEGEND].oc_oprop = randfemienchantment();
	objects[IMPLANT_OF_VIRTUE].oc_oprop = randfemienchantment();

	objects[IMPLANT_OF_IRE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_MALICE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_AGES].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_SUSTENANCE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_TRUTH].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_REMORSE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_GRACE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_WASTE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_COMBAT].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_FAITH].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_DISPATCH].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_DREAD].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_VITA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_MAGGOTS].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_BEAUTY].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_DUSK].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_TRIBUTE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_INERTIA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_SWEETNESS].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_IRRIGATION].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_TWILIGHT].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_MEMORY].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_LOVE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_VINES].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_ANIMA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_LINES].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_THAWING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_DESIRE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_PAIN].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_DARING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_CORRUPTION].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_EVISCERATION].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_TRAVELING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_CHEATING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_ANTHRAX].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_ATTRITION].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_HACKING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_PROSPERING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_VALHALLA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_DECEPTION].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_BUTCHERY].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_BLIZZARD].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_TERROR].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_DAWN].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_BILE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_CREDIT].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_QUOTA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_VIRILITY].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_VANILLA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_HOPE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_ABRASION].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_OSMOSIS].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_NIRVANA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_ENVY].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_ENNUI].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_IMPOSSIBILITY].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_ADMIRATION].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_SUNLIGHT].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_TSUNAMI].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_FREEDOM].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

	objects[IMPLANT_OF_TEN_THOUSAND_THINGS].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_TOSSING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_HEAVY_LUBRICATION].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_MISANTHROPHY].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_CRAP].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_KING_BEEFCAKE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_PUMPING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_WANNABE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_YOUR_MOMMA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_SWALLOWING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_MENSTRATING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[IMPLANT_OF_ENFORCING].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

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
	objects[DUMMY_CLOAK_W].a_can = rn2(4);
	objects[DUMMY_CLOAK_X].a_can = rn2(4);
	objects[DUMMY_CLOAK_Y].a_can = rn2(4);
	objects[DUMMY_CLOAK_Z].a_can = rn2(4);
	objects[DUMMY_CLOAK_AA].a_can = rn2(4);
	objects[DUMMY_CLOAK_AB].a_can = rn2(4);
	objects[DUMMY_CLOAK_AC].a_can = rn2(4);
	objects[DUMMY_CLOAK_AD].a_can = rn2(4);
	objects[DUMMY_CLOAK_AE].a_can = rn2(4);
	objects[DUMMY_CLOAK_AF].a_can = rn2(4);
	objects[DUMMY_CLOAK_AG].a_can = rn2(4);
	objects[DUMMY_CLOAK_AH].a_can = rn2(4);
	objects[DUMMY_CLOAK_AI].a_can = rn2(4);
	objects[DUMMY_CLOAK_AJ].a_can = rn2(4);
	objects[DUMMY_CLOAK_AK].a_can = rn2(4);
	objects[DUMMY_CLOAK_AL].a_can = rn2(4);
	objects[DUMMY_CLOAK_AM].a_can = rn2(4);
	objects[DUMMY_CLOAK_AN].a_can = rn2(4);
	objects[DUMMY_CLOAK_AO].a_can = rn2(4);
	objects[DUMMY_CLOAK_AP].a_can = rn2(4);
	objects[DUMMY_CLOAK_AQ].a_can = rn2(4);
	objects[DUMMY_CLOAK_AR].a_can = rn2(4);
	objects[DUMMY_CLOAK_AS].a_can = rn2(4);
	objects[DUMMY_CLOAK_AT].a_can = rn2(4);
	objects[DUMMY_CLOAK_AU].a_can = rn2(4);
	objects[DUMMY_CLOAK_AV].a_can = rn2(4);
	objects[DUMMY_CLOAK_AW].a_can = rn2(4);
	objects[DUMMY_CLOAK_AX].a_can = rn2(4);

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
	objects[DUMMY_CLOAK_W].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_W].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_W].a_ac < 20) objects[DUMMY_CLOAK_W].a_ac++;
	}
	objects[DUMMY_CLOAK_X].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_X].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_X].a_ac < 20) objects[DUMMY_CLOAK_X].a_ac++;
	}
	objects[DUMMY_CLOAK_Y].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_Y].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_Y].a_ac < 20) objects[DUMMY_CLOAK_Y].a_ac++;
	}
	objects[DUMMY_CLOAK_Z].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_Z].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_Z].a_ac < 20) objects[DUMMY_CLOAK_Z].a_ac++;
	}
	objects[DUMMY_CLOAK_AA].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AA].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AA].a_ac < 20) objects[DUMMY_CLOAK_AA].a_ac++;
	}
	objects[DUMMY_CLOAK_AB].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AB].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AB].a_ac < 20) objects[DUMMY_CLOAK_AB].a_ac++;
	}
	objects[DUMMY_CLOAK_AC].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AC].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AC].a_ac < 20) objects[DUMMY_CLOAK_AC].a_ac++;
	}
	objects[DUMMY_CLOAK_AD].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AD].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AD].a_ac < 20) objects[DUMMY_CLOAK_AD].a_ac++;
	}
	objects[DUMMY_CLOAK_AE].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AE].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AE].a_ac < 20) objects[DUMMY_CLOAK_AE].a_ac++;
	}
	objects[DUMMY_CLOAK_AF].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AF].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AF].a_ac < 20) objects[DUMMY_CLOAK_AF].a_ac++;
	}
	objects[DUMMY_CLOAK_AG].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AG].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AG].a_ac < 20) objects[DUMMY_CLOAK_AG].a_ac++;
	}
	objects[DUMMY_CLOAK_AH].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AH].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AH].a_ac < 20) objects[DUMMY_CLOAK_AH].a_ac++;
	}
	objects[DUMMY_CLOAK_AI].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AI].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AI].a_ac < 20) objects[DUMMY_CLOAK_AI].a_ac++;
	}
	objects[DUMMY_CLOAK_AJ].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AJ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AJ].a_ac < 20) objects[DUMMY_CLOAK_AJ].a_ac++;
	}
	objects[DUMMY_CLOAK_AK].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AK].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AK].a_ac < 20) objects[DUMMY_CLOAK_AK].a_ac++;
	}
	objects[DUMMY_CLOAK_AL].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AL].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AL].a_ac < 20) objects[DUMMY_CLOAK_AL].a_ac++;
	}
	objects[DUMMY_CLOAK_AM].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AM].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AM].a_ac < 20) objects[DUMMY_CLOAK_AM].a_ac++;
	}
	objects[DUMMY_CLOAK_AN].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AN].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AN].a_ac < 20) objects[DUMMY_CLOAK_AN].a_ac++;
	}
	objects[DUMMY_CLOAK_AO].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AO].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AO].a_ac < 20) objects[DUMMY_CLOAK_AO].a_ac++;
	}
	objects[DUMMY_CLOAK_AP].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AP].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AP].a_ac < 20) objects[DUMMY_CLOAK_AP].a_ac++;
	}
	objects[DUMMY_CLOAK_AQ].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AQ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AQ].a_ac < 20) objects[DUMMY_CLOAK_AQ].a_ac++;
	}
	objects[DUMMY_CLOAK_AR].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AR].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AR].a_ac < 20) objects[DUMMY_CLOAK_AR].a_ac++;
	}
	objects[DUMMY_CLOAK_AS].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AS].a_ac < 20) objects[DUMMY_CLOAK_AS].a_ac++;
	}
	objects[DUMMY_CLOAK_AT].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AT].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AT].a_ac < 20) objects[DUMMY_CLOAK_AT].a_ac++;
	}
	objects[DUMMY_CLOAK_AU].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AU].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AU].a_ac < 20) objects[DUMMY_CLOAK_AU].a_ac++;
	}
	objects[DUMMY_CLOAK_AV].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AV].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AV].a_ac < 20) objects[DUMMY_CLOAK_AV].a_ac++;
	}
	objects[DUMMY_CLOAK_AW].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AW].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AW].a_ac < 20) objects[DUMMY_CLOAK_AW].a_ac++;
	}
	objects[DUMMY_CLOAK_AX].a_ac = 0;
	if (rn2(10)) objects[DUMMY_CLOAK_AX].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_CLOAK_AX].a_ac < 20) objects[DUMMY_CLOAK_AX].a_ac++;
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
	objects[DUMMY_HELMET_L].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_M].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_N].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_O].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_P].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_Q].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_R].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_S].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_T].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_U].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_V].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_W].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_X].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_Y].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_Z].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_AA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_AB].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_AC].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_AD].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_AE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_AF].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_HELMET_AG].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

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
	objects[DUMMY_HELMET_L].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_M].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_N].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_O].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_P].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_Q].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_R].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_S].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_T].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_U].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_V].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_W].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_X].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_Y].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_Z].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_AA].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_AB].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_AC].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_AD].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_AE].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_AF].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_HELMET_AG].a_can = rnd(10) ? 0 : rnd(3);

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
	objects[DUMMY_HELMET_L].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_L].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_L].a_ac < 20) objects[DUMMY_HELMET_L].a_ac++;
	}
	objects[DUMMY_HELMET_M].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_M].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_M].a_ac < 20) objects[DUMMY_HELMET_M].a_ac++;
	}
	objects[DUMMY_HELMET_N].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_N].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_N].a_ac < 20) objects[DUMMY_HELMET_N].a_ac++;
	}
	objects[DUMMY_HELMET_O].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_O].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_O].a_ac < 20) objects[DUMMY_HELMET_O].a_ac++;
	}
	objects[DUMMY_HELMET_P].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_P].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_P].a_ac < 20) objects[DUMMY_HELMET_P].a_ac++;
	}
	objects[DUMMY_HELMET_Q].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_Q].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_Q].a_ac < 20) objects[DUMMY_HELMET_Q].a_ac++;
	}
	objects[DUMMY_HELMET_R].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_R].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_R].a_ac < 20) objects[DUMMY_HELMET_R].a_ac++;
	}
	objects[DUMMY_HELMET_S].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_S].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_S].a_ac < 20) objects[DUMMY_HELMET_S].a_ac++;
	}
	objects[DUMMY_HELMET_T].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_T].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_T].a_ac < 20) objects[DUMMY_HELMET_T].a_ac++;
	}
	objects[DUMMY_HELMET_U].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_U].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_U].a_ac < 20) objects[DUMMY_HELMET_U].a_ac++;
	}
	objects[DUMMY_HELMET_V].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_V].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_V].a_ac < 20) objects[DUMMY_HELMET_V].a_ac++;
	}
	objects[DUMMY_HELMET_W].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_W].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_W].a_ac < 20) objects[DUMMY_HELMET_W].a_ac++;
	}
	objects[DUMMY_HELMET_X].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_X].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_X].a_ac < 20) objects[DUMMY_HELMET_X].a_ac++;
	}
	objects[DUMMY_HELMET_Y].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_Y].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_Y].a_ac < 20) objects[DUMMY_HELMET_Y].a_ac++;
	}
	objects[DUMMY_HELMET_Z].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_Z].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_Z].a_ac < 20) objects[DUMMY_HELMET_Z].a_ac++;
	}
	objects[DUMMY_HELMET_AA].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_AA].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_AA].a_ac < 20) objects[DUMMY_HELMET_AA].a_ac++;
	}
	objects[DUMMY_HELMET_AB].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_AB].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_AB].a_ac < 20) objects[DUMMY_HELMET_AB].a_ac++;
	}
	objects[DUMMY_HELMET_AC].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_AC].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_AC].a_ac < 20) objects[DUMMY_HELMET_AC].a_ac++;
	}
	objects[DUMMY_HELMET_AD].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_AD].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_AD].a_ac < 20) objects[DUMMY_HELMET_AD].a_ac++;
	}
	objects[DUMMY_HELMET_AE].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_AE].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_AE].a_ac < 20) objects[DUMMY_HELMET_AE].a_ac++;
	}
	objects[DUMMY_HELMET_AF].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_AF].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_AF].a_ac < 20) objects[DUMMY_HELMET_AF].a_ac++;
	}
	objects[DUMMY_HELMET_AG].a_ac = 0;
	if (rn2(10)) objects[DUMMY_HELMET_AG].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_HELMET_AG].a_ac < 20) objects[DUMMY_HELMET_AG].a_ac++;
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
	objects[DUMMY_GLOVES_J].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_K].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_L].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_M].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_N].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_O].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_P].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_Q].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_R].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_S].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_T].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_U].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_V].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_W].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_X].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_Y].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_Z].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_AA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_AB].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_AC].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_AD].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_AE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_AF].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_AG].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_AH].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_GLOVES_AI].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

	objects[DUMMY_GLOVES_A].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_B].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_C].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_D].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_E].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_F].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_G].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_H].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_I].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_J].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_K].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_L].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_M].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_N].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_O].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_P].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_Q].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_R].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_S].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_T].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_U].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_V].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_W].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_X].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_Y].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_Z].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_AA].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_AB].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_AC].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_AD].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_AE].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_AF].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_AG].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_AH].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_GLOVES_AI].a_can = rnd(10) ? 0 : rnd(3);

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
	objects[DUMMY_GLOVES_J].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_J].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_J].a_ac < 20) objects[DUMMY_GLOVES_J].a_ac++;
	}
	objects[DUMMY_GLOVES_K].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_K].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_K].a_ac < 20) objects[DUMMY_GLOVES_K].a_ac++;
	}
	objects[DUMMY_GLOVES_L].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_L].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_L].a_ac < 20) objects[DUMMY_GLOVES_L].a_ac++;
	}
	objects[DUMMY_GLOVES_M].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_M].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_M].a_ac < 20) objects[DUMMY_GLOVES_M].a_ac++;
	}
	objects[DUMMY_GLOVES_N].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_N].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_N].a_ac < 20) objects[DUMMY_GLOVES_N].a_ac++;
	}
	objects[DUMMY_GLOVES_O].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_O].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_O].a_ac < 20) objects[DUMMY_GLOVES_O].a_ac++;
	}
	objects[DUMMY_GLOVES_P].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_P].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_P].a_ac < 20) objects[DUMMY_GLOVES_P].a_ac++;
	}
	objects[DUMMY_GLOVES_Q].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_Q].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_Q].a_ac < 20) objects[DUMMY_GLOVES_Q].a_ac++;
	}
	objects[DUMMY_GLOVES_R].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_R].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_R].a_ac < 20) objects[DUMMY_GLOVES_R].a_ac++;
	}
	objects[DUMMY_GLOVES_S].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_S].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_S].a_ac < 20) objects[DUMMY_GLOVES_S].a_ac++;
	}
	objects[DUMMY_GLOVES_T].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_T].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_T].a_ac < 20) objects[DUMMY_GLOVES_T].a_ac++;
	}
	objects[DUMMY_GLOVES_U].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_U].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_U].a_ac < 20) objects[DUMMY_GLOVES_U].a_ac++;
	}
	objects[DUMMY_GLOVES_V].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_V].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_V].a_ac < 20) objects[DUMMY_GLOVES_V].a_ac++;
	}
	objects[DUMMY_GLOVES_W].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_W].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_W].a_ac < 20) objects[DUMMY_GLOVES_W].a_ac++;
	}
	objects[DUMMY_GLOVES_X].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_X].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_X].a_ac < 20) objects[DUMMY_GLOVES_X].a_ac++;
	}
	objects[DUMMY_GLOVES_Y].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_Y].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_Y].a_ac < 20) objects[DUMMY_GLOVES_Y].a_ac++;
	}
	objects[DUMMY_GLOVES_Z].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_Z].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_Z].a_ac < 20) objects[DUMMY_GLOVES_Z].a_ac++;
	}
	objects[DUMMY_GLOVES_AA].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_AA].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_AA].a_ac < 20) objects[DUMMY_GLOVES_AA].a_ac++;
	}
	objects[DUMMY_GLOVES_AB].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_AB].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_AB].a_ac < 20) objects[DUMMY_GLOVES_AB].a_ac++;
	}
	objects[DUMMY_GLOVES_AC].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_AC].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_AC].a_ac < 20) objects[DUMMY_GLOVES_AC].a_ac++;
	}
	objects[DUMMY_GLOVES_AD].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_AD].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_AD].a_ac < 20) objects[DUMMY_GLOVES_AD].a_ac++;
	}
	objects[DUMMY_GLOVES_AE].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_AE].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_AE].a_ac < 20) objects[DUMMY_GLOVES_AE].a_ac++;
	}
	objects[DUMMY_GLOVES_AF].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_AF].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_AF].a_ac < 20) objects[DUMMY_GLOVES_AF].a_ac++;
	}
	objects[DUMMY_GLOVES_AG].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_AG].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_AG].a_ac < 20) objects[DUMMY_GLOVES_AG].a_ac++;
	}
	objects[DUMMY_GLOVES_AH].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_AH].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_AH].a_ac < 20) objects[DUMMY_GLOVES_AH].a_ac++;
	}
	objects[DUMMY_GLOVES_AI].a_ac = 0;
	if (rn2(10)) objects[DUMMY_GLOVES_AI].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_GLOVES_AI].a_ac < 20) objects[DUMMY_GLOVES_AI].a_ac++;
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
	objects[DUMMY_BOOTS_M].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_N].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_O].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_P].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_Q].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_R].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_S].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_T].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_U].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_V].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_W].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_X].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_Y].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_Z].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AB].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AC].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AD].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AF].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AG].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AH].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AI].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AJ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AK].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AL].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AM].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AN].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AO].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AP].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AQ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AR].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AS].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AT].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AU].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AV].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AW].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_BOOTS_AX].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

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
	objects[DUMMY_BOOTS_M].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_N].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_O].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_P].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_Q].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_R].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_S].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_T].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_U].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_V].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_W].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_X].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_Y].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_Z].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AA].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AB].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AC].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AD].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AE].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AF].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AG].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AH].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AI].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AJ].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AK].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AL].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AM].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AN].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AO].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AP].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AQ].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AR].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AS].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AT].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AU].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AV].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AW].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_BOOTS_AX].a_can = rnd(10) ? 0 : rnd(3);

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
	objects[DUMMY_BOOTS_M].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_M].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_M].a_ac < 20) objects[DUMMY_BOOTS_M].a_ac++;
	}
	objects[DUMMY_BOOTS_N].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_N].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_N].a_ac < 20) objects[DUMMY_BOOTS_N].a_ac++;
	}
	objects[DUMMY_BOOTS_O].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_O].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_O].a_ac < 20) objects[DUMMY_BOOTS_O].a_ac++;
	}
	objects[DUMMY_BOOTS_P].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_P].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_P].a_ac < 20) objects[DUMMY_BOOTS_P].a_ac++;
	}
	objects[DUMMY_BOOTS_Q].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_Q].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_Q].a_ac < 20) objects[DUMMY_BOOTS_Q].a_ac++;
	}
	objects[DUMMY_BOOTS_R].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_R].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_R].a_ac < 20) objects[DUMMY_BOOTS_R].a_ac++;
	}
	objects[DUMMY_BOOTS_S].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_S].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_S].a_ac < 20) objects[DUMMY_BOOTS_S].a_ac++;
	}
	objects[DUMMY_BOOTS_T].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_T].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_T].a_ac < 20) objects[DUMMY_BOOTS_T].a_ac++;
	}
	objects[DUMMY_BOOTS_U].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_U].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_U].a_ac < 20) objects[DUMMY_BOOTS_U].a_ac++;
	}
	objects[DUMMY_BOOTS_V].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_V].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_V].a_ac < 20) objects[DUMMY_BOOTS_V].a_ac++;
	}
	objects[DUMMY_BOOTS_W].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_W].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_W].a_ac < 20) objects[DUMMY_BOOTS_W].a_ac++;
	}
	objects[DUMMY_BOOTS_X].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_X].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_X].a_ac < 20) objects[DUMMY_BOOTS_X].a_ac++;
	}
	objects[DUMMY_BOOTS_Y].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_Y].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_Y].a_ac < 20) objects[DUMMY_BOOTS_Y].a_ac++;
	}
	objects[DUMMY_BOOTS_Z].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_Z].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_Z].a_ac < 20) objects[DUMMY_BOOTS_Z].a_ac++;
	}
	objects[DUMMY_BOOTS_AA].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AA].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AA].a_ac < 20) objects[DUMMY_BOOTS_AA].a_ac++;
	}
	objects[DUMMY_BOOTS_AB].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AB].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AB].a_ac < 20) objects[DUMMY_BOOTS_AB].a_ac++;
	}
	objects[DUMMY_BOOTS_AC].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AC].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AC].a_ac < 20) objects[DUMMY_BOOTS_AC].a_ac++;
	}
	objects[DUMMY_BOOTS_AD].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AD].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AD].a_ac < 20) objects[DUMMY_BOOTS_AD].a_ac++;
	}
	objects[DUMMY_BOOTS_AE].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AE].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AE].a_ac < 20) objects[DUMMY_BOOTS_AE].a_ac++;
	}
	objects[DUMMY_BOOTS_AF].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AF].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AF].a_ac < 20) objects[DUMMY_BOOTS_AF].a_ac++;
	}
	objects[DUMMY_BOOTS_AG].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AG].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AG].a_ac < 20) objects[DUMMY_BOOTS_AG].a_ac++;
	}
	objects[DUMMY_BOOTS_AH].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AH].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AH].a_ac < 20) objects[DUMMY_BOOTS_AH].a_ac++;
	}
	objects[DUMMY_BOOTS_AI].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AI].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AI].a_ac < 20) objects[DUMMY_BOOTS_AI].a_ac++;
	}
	objects[DUMMY_BOOTS_AJ].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AJ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AJ].a_ac < 20) objects[DUMMY_BOOTS_AJ].a_ac++;
	}
	objects[DUMMY_BOOTS_AK].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AK].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AK].a_ac < 20) objects[DUMMY_BOOTS_AK].a_ac++;
	}
	objects[DUMMY_BOOTS_AL].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AL].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AL].a_ac < 20) objects[DUMMY_BOOTS_AL].a_ac++;
	}
	objects[DUMMY_BOOTS_AM].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AM].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AM].a_ac < 20) objects[DUMMY_BOOTS_AM].a_ac++;
	}
	objects[DUMMY_BOOTS_AN].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AN].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AN].a_ac < 20) objects[DUMMY_BOOTS_AN].a_ac++;
	}
	objects[DUMMY_BOOTS_AO].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AO].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AO].a_ac < 20) objects[DUMMY_BOOTS_AO].a_ac++;
	}
	objects[DUMMY_BOOTS_AP].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AP].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AP].a_ac < 20) objects[DUMMY_BOOTS_AP].a_ac++;
	}
	objects[DUMMY_BOOTS_AQ].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AQ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AQ].a_ac < 20) objects[DUMMY_BOOTS_AQ].a_ac++;
	}
	objects[DUMMY_BOOTS_AR].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AR].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AR].a_ac < 20) objects[DUMMY_BOOTS_AR].a_ac++;
	}
	objects[DUMMY_BOOTS_AS].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AS].a_ac < 20) objects[DUMMY_BOOTS_AS].a_ac++;
	}
	objects[DUMMY_BOOTS_AT].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AT].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AT].a_ac < 20) objects[DUMMY_BOOTS_AT].a_ac++;
	}
	objects[DUMMY_BOOTS_AU].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AU].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AU].a_ac < 20) objects[DUMMY_BOOTS_AU].a_ac++;
	}
	objects[DUMMY_BOOTS_AV].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AV].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AV].a_ac < 20) objects[DUMMY_BOOTS_AV].a_ac++;
	}
	objects[DUMMY_BOOTS_AW].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AW].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AW].a_ac < 20) objects[DUMMY_BOOTS_AW].a_ac++;
	}
	objects[DUMMY_BOOTS_AX].a_ac = 0;
	if (rn2(10)) objects[DUMMY_BOOTS_AX].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_BOOTS_AX].a_ac < 20) objects[DUMMY_BOOTS_AX].a_ac++;
	}

	objects[DUMMY_ROBE_A].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_B].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_C].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_D].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_E].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_F].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_G].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_H].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_I].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_J].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_K].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_L].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_M].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_N].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_O].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_P].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_Q].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_R].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_S].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_T].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_U].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_V].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_W].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_X].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_Y].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_Z].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AB].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AC].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AD].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AF].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AG].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AH].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AI].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AJ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AK].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AL].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AM].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AN].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AO].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AP].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AQ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AR].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AS].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AT].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AU].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AV].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AW].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AX].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AY].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_AZ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BA].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BB].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BC].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BD].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BE].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BF].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BG].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BH].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BI].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BJ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BK].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BL].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BM].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BN].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BO].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BP].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BQ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BR].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BS].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BT].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BU].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BV].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BW].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BX].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BY].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();
	objects[DUMMY_ROBE_BZ].oc_oprop = !rn2(10) ? randnastyenchantment() : randenchantment();

	objects[DUMMY_ROBE_A].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_B].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_C].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_D].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_E].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_F].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_G].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_H].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_I].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_J].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_K].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_L].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_M].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_N].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_O].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_P].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_Q].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_R].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_S].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_T].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_U].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_V].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_W].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_X].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_Y].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_Z].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AA].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AB].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AC].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AD].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AE].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AF].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AG].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AH].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AI].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AJ].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AK].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AL].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AM].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AN].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AO].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AP].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AQ].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AR].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AS].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AT].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AU].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AV].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AW].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AX].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AY].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_AZ].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BA].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BB].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BC].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BD].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BE].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BF].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BG].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BH].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BI].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BJ].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BK].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BL].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BM].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BN].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BO].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BP].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BQ].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BR].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BS].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BT].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BU].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BV].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BW].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BX].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BY].a_can = rnd(10) ? 0 : rnd(3);
	objects[DUMMY_ROBE_BZ].a_can = rnd(10) ? 0 : rnd(3);

	objects[DUMMY_ROBE_A].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_A].a_ac < 20) objects[DUMMY_ROBE_A].a_ac++;
	}
	objects[DUMMY_ROBE_B].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_B].a_ac < 20) objects[DUMMY_ROBE_B].a_ac++;
	}
	objects[DUMMY_ROBE_C].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_C].a_ac < 20) objects[DUMMY_ROBE_C].a_ac++;
	}
	objects[DUMMY_ROBE_D].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_D].a_ac < 20) objects[DUMMY_ROBE_D].a_ac++;
	}
	objects[DUMMY_ROBE_E].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_E].a_ac < 20) objects[DUMMY_ROBE_E].a_ac++;
	}
	objects[DUMMY_ROBE_F].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_F].a_ac < 20) objects[DUMMY_ROBE_F].a_ac++;
	}
	objects[DUMMY_ROBE_G].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_G].a_ac < 20) objects[DUMMY_ROBE_G].a_ac++;
	}
	objects[DUMMY_ROBE_H].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_H].a_ac < 20) objects[DUMMY_ROBE_H].a_ac++;
	}
	objects[DUMMY_ROBE_I].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_I].a_ac < 20) objects[DUMMY_ROBE_I].a_ac++;
	}
	objects[DUMMY_ROBE_J].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_J].a_ac < 20) objects[DUMMY_ROBE_J].a_ac++;
	}
	objects[DUMMY_ROBE_K].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_K].a_ac < 20) objects[DUMMY_ROBE_K].a_ac++;
	}
	objects[DUMMY_ROBE_L].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_L].a_ac < 20) objects[DUMMY_ROBE_L].a_ac++;
	}
	objects[DUMMY_ROBE_M].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_M].a_ac < 20) objects[DUMMY_ROBE_M].a_ac++;
	}
	objects[DUMMY_ROBE_N].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_N].a_ac < 20) objects[DUMMY_ROBE_N].a_ac++;
	}
	objects[DUMMY_ROBE_O].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_O].a_ac < 20) objects[DUMMY_ROBE_O].a_ac++;
	}
	objects[DUMMY_ROBE_P].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_P].a_ac < 20) objects[DUMMY_ROBE_P].a_ac++;
	}
	objects[DUMMY_ROBE_Q].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_Q].a_ac < 20) objects[DUMMY_ROBE_Q].a_ac++;
	}
	objects[DUMMY_ROBE_R].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_R].a_ac < 20) objects[DUMMY_ROBE_R].a_ac++;
	}
	objects[DUMMY_ROBE_S].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_S].a_ac < 20) objects[DUMMY_ROBE_S].a_ac++;
	}
	objects[DUMMY_ROBE_T].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_T].a_ac < 20) objects[DUMMY_ROBE_T].a_ac++;
	}
	objects[DUMMY_ROBE_U].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_U].a_ac < 20) objects[DUMMY_ROBE_U].a_ac++;
	}
	objects[DUMMY_ROBE_V].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_V].a_ac < 20) objects[DUMMY_ROBE_V].a_ac++;
	}
	objects[DUMMY_ROBE_W].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_W].a_ac < 20) objects[DUMMY_ROBE_W].a_ac++;
	}
	objects[DUMMY_ROBE_X].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_X].a_ac < 20) objects[DUMMY_ROBE_X].a_ac++;
	}
	objects[DUMMY_ROBE_Y].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_Y].a_ac < 20) objects[DUMMY_ROBE_Y].a_ac++;
	}
	objects[DUMMY_ROBE_Z].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_Z].a_ac < 20) objects[DUMMY_ROBE_Z].a_ac++;
	}
	objects[DUMMY_ROBE_AA].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AA].a_ac < 20) objects[DUMMY_ROBE_AA].a_ac++;
	}
	objects[DUMMY_ROBE_AB].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AB].a_ac < 20) objects[DUMMY_ROBE_AB].a_ac++;
	}
	objects[DUMMY_ROBE_AC].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AC].a_ac < 20) objects[DUMMY_ROBE_AC].a_ac++;
	}
	objects[DUMMY_ROBE_AD].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AD].a_ac < 20) objects[DUMMY_ROBE_AD].a_ac++;
	}
	objects[DUMMY_ROBE_AE].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AE].a_ac < 20) objects[DUMMY_ROBE_AE].a_ac++;
	}
	objects[DUMMY_ROBE_AF].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AF].a_ac < 20) objects[DUMMY_ROBE_AF].a_ac++;
	}
	objects[DUMMY_ROBE_AG].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AG].a_ac < 20) objects[DUMMY_ROBE_AG].a_ac++;
	}
	objects[DUMMY_ROBE_AH].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AH].a_ac < 20) objects[DUMMY_ROBE_AH].a_ac++;
	}
	objects[DUMMY_ROBE_AI].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AI].a_ac < 20) objects[DUMMY_ROBE_AI].a_ac++;
	}
	objects[DUMMY_ROBE_AJ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AJ].a_ac < 20) objects[DUMMY_ROBE_AJ].a_ac++;
	}
	objects[DUMMY_ROBE_AK].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AK].a_ac < 20) objects[DUMMY_ROBE_AK].a_ac++;
	}
	objects[DUMMY_ROBE_AL].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AL].a_ac < 20) objects[DUMMY_ROBE_AL].a_ac++;
	}
	objects[DUMMY_ROBE_AM].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AM].a_ac < 20) objects[DUMMY_ROBE_AM].a_ac++;
	}
	objects[DUMMY_ROBE_AN].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AN].a_ac < 20) objects[DUMMY_ROBE_AN].a_ac++;
	}
	objects[DUMMY_ROBE_AO].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AO].a_ac < 20) objects[DUMMY_ROBE_AO].a_ac++;
	}
	objects[DUMMY_ROBE_AP].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AP].a_ac < 20) objects[DUMMY_ROBE_AP].a_ac++;
	}
	objects[DUMMY_ROBE_AQ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AQ].a_ac < 20) objects[DUMMY_ROBE_AQ].a_ac++;
	}
	objects[DUMMY_ROBE_AR].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AR].a_ac < 20) objects[DUMMY_ROBE_AR].a_ac++;
	}
	objects[DUMMY_ROBE_AS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AS].a_ac < 20) objects[DUMMY_ROBE_AS].a_ac++;
	}
	objects[DUMMY_ROBE_AT].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AT].a_ac < 20) objects[DUMMY_ROBE_AT].a_ac++;
	}
	objects[DUMMY_ROBE_AU].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AU].a_ac < 20) objects[DUMMY_ROBE_AU].a_ac++;
	}
	objects[DUMMY_ROBE_AV].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AV].a_ac < 20) objects[DUMMY_ROBE_AV].a_ac++;
	}
	objects[DUMMY_ROBE_AW].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AW].a_ac < 20) objects[DUMMY_ROBE_AW].a_ac++;
	}
	objects[DUMMY_ROBE_AX].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AX].a_ac < 20) objects[DUMMY_ROBE_AX].a_ac++;
	}
	objects[DUMMY_ROBE_AY].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AY].a_ac < 20) objects[DUMMY_ROBE_AY].a_ac++;
	}
	objects[DUMMY_ROBE_AZ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_AZ].a_ac < 20) objects[DUMMY_ROBE_AZ].a_ac++;
	}
	objects[DUMMY_ROBE_BA].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BA].a_ac < 20) objects[DUMMY_ROBE_BA].a_ac++;
	}
	objects[DUMMY_ROBE_BB].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BB].a_ac < 20) objects[DUMMY_ROBE_BB].a_ac++;
	}
	objects[DUMMY_ROBE_BC].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BC].a_ac < 20) objects[DUMMY_ROBE_BC].a_ac++;
	}
	objects[DUMMY_ROBE_BD].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BD].a_ac < 20) objects[DUMMY_ROBE_BD].a_ac++;
	}
	objects[DUMMY_ROBE_BE].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BE].a_ac < 20) objects[DUMMY_ROBE_BE].a_ac++;
	}
	objects[DUMMY_ROBE_BF].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BF].a_ac < 20) objects[DUMMY_ROBE_BF].a_ac++;
	}
	objects[DUMMY_ROBE_BG].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BG].a_ac < 20) objects[DUMMY_ROBE_BG].a_ac++;
	}
	objects[DUMMY_ROBE_BH].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BH].a_ac < 20) objects[DUMMY_ROBE_BH].a_ac++;
	}
	objects[DUMMY_ROBE_BI].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BI].a_ac < 20) objects[DUMMY_ROBE_BI].a_ac++;
	}
	objects[DUMMY_ROBE_BJ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BJ].a_ac < 20) objects[DUMMY_ROBE_BJ].a_ac++;
	}
	objects[DUMMY_ROBE_BK].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BK].a_ac < 20) objects[DUMMY_ROBE_BK].a_ac++;
	}
	objects[DUMMY_ROBE_BL].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BL].a_ac < 20) objects[DUMMY_ROBE_BL].a_ac++;
	}
	objects[DUMMY_ROBE_BM].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BM].a_ac < 20) objects[DUMMY_ROBE_BM].a_ac++;
	}
	objects[DUMMY_ROBE_BN].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BN].a_ac < 20) objects[DUMMY_ROBE_BN].a_ac++;
	}
	objects[DUMMY_ROBE_BO].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BO].a_ac < 20) objects[DUMMY_ROBE_BO].a_ac++;
	}
	objects[DUMMY_ROBE_BP].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BP].a_ac < 20) objects[DUMMY_ROBE_BP].a_ac++;
	}
	objects[DUMMY_ROBE_BQ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BQ].a_ac < 20) objects[DUMMY_ROBE_BQ].a_ac++;
	}
	objects[DUMMY_ROBE_BR].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BR].a_ac < 20) objects[DUMMY_ROBE_BR].a_ac++;
	}
	objects[DUMMY_ROBE_BS].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BS].a_ac < 20) objects[DUMMY_ROBE_BS].a_ac++;
	}
	objects[DUMMY_ROBE_BT].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BT].a_ac < 20) objects[DUMMY_ROBE_BT].a_ac++;
	}
	objects[DUMMY_ROBE_BU].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BU].a_ac < 20) objects[DUMMY_ROBE_BU].a_ac++;
	}
	objects[DUMMY_ROBE_BV].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BV].a_ac < 20) objects[DUMMY_ROBE_BV].a_ac++;
	}
	objects[DUMMY_ROBE_BW].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BW].a_ac < 20) objects[DUMMY_ROBE_BW].a_ac++;
	}
	objects[DUMMY_ROBE_BX].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BX].a_ac < 20) objects[DUMMY_ROBE_BX].a_ac++;
	}
	objects[DUMMY_ROBE_BY].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BY].a_ac < 20) objects[DUMMY_ROBE_BY].a_ac++;
	}
	objects[DUMMY_ROBE_BZ].a_ac = 1;
	while (!rn2(3)) {
		if (objects[DUMMY_ROBE_BZ].a_ac < 20) objects[DUMMY_ROBE_BZ].a_ac++;
	}

	objects[SPE_FORBIDDEN_KNOWLEDGE].oc_level = rnd(8);

	objects[SPE_CHARACTER_RECURSION].oc_level = rnd(8);

	objects[WONDER_BALL].oc_material = rn2(LASTMATERIAL + 1);
	objects[WONDER_CHAIN].oc_material = rn2(LASTMATERIAL + 1);
	objects[TOILET_ROLL].oc_material = rn2(LASTMATERIAL + 1);
	objects[WONDER_PLATE].oc_material = rn2(LASTMATERIAL + 1);
	objects[MEDIUM_MITHRIL_COAT].oc_material = rn2(LASTMATERIAL + 1);
	objects[TAPERED_MAIL].oc_material = rn2(LASTMATERIAL + 1);
	objects[EXCITING_ARMOR].oc_material = rn2(LASTMATERIAL + 1);
	objects[LORICATED_CLOAK].oc_material = rn2(LASTMATERIAL + 1);
	objects[BASINET].oc_material = rn2(LASTMATERIAL + 1);
	objects[ROCKET_GAUNTLETS].oc_material = rn2(LASTMATERIAL + 1);
	objects[ANCIENT_SHIELD].oc_material = rn2(LASTMATERIAL + 1);
	objects[ITALIAN_HEELS].oc_material = rn2(LASTMATERIAL + 1);
	objects[WONDER_DAGGER].oc_material = rn2(LASTMATERIAL + 1);
	objects[UNKNOWN_KNIFE].oc_material = rn2(LASTMATERIAL + 1);
	objects[SHARP_AXE].oc_material = rn2(LASTMATERIAL + 1);
	objects[SHORT_BLADE].oc_material = rn2(LASTMATERIAL + 1);
	objects[ELEGANT_BROADSWORD].oc_material = rn2(LASTMATERIAL + 1);
	objects[MAIN_SWORD].oc_material = rn2(LASTMATERIAL + 1);
	objects[ROMAN_SWORD].oc_material = rn2(LASTMATERIAL + 1);
	objects[MYTHICAL_SABLE].oc_material = rn2(LASTMATERIAL + 1);
	objects[CRYPTIC_SABER].oc_material = rn2(LASTMATERIAL + 1);
	objects[BLOW_AKLYS].oc_material = rn2(LASTMATERIAL + 1);
	objects[TELEPHONE].oc_material = rn2(LASTMATERIAL + 1);
	objects[BATLETH].oc_material = rn2(LASTMATERIAL + 1);
	objects[ELITE_BATLETH].oc_material = rn2(LASTMATERIAL + 1);
	objects[CALCULATOR].oc_material = rn2(LASTMATERIAL + 1);
	objects[WEIGHTED_FLAIL].oc_material = rn2(LASTMATERIAL + 1);
	objects[BLOCK_HEELED_SANDAL].oc_material = rn2(LASTMATERIAL + 1);
	objects[PROSTITUTE_SHOE].oc_material = rn2(LASTMATERIAL + 1);
	objects[BO_STAFF].oc_material = rn2(LASTMATERIAL + 1);
	objects[LONG_POLE].oc_material = rn2(LASTMATERIAL + 1);
	objects[SHARP_POLE].oc_material = rn2(LASTMATERIAL + 1);
	objects[RANDOSPEAR].oc_material = rn2(LASTMATERIAL + 1);
	objects[STACK_JAVELIN].oc_material = rn2(LASTMATERIAL + 1);
	objects[SPEC_LANCE].oc_material = rn2(LASTMATERIAL + 1);
	objects[META_BOW].oc_material = rn2(LASTMATERIAL + 1);
	objects[WONDER_ARROW].oc_material = rn2(LASTMATERIAL + 1);
	objects[MATERIAL_BOLT].oc_material = rn2(LASTMATERIAL + 1);
	objects[RANDOM_DART].oc_material = rn2(LASTMATERIAL + 1);
	objects[CUBIC_STAR].oc_material = rn2(LASTMATERIAL + 1);
	objects[SEXPLAY_WHIP].oc_material = rn2(LASTMATERIAL + 1);
	objects[MYSTERY_PICK].oc_material = rn2(LASTMATERIAL + 1);
	objects[MYSTERY_LIGHTSABER].oc_material = rn2(LASTMATERIAL + 1);
	objects[RAINBOW_LIGHTSABER].oc_material = rn2(LASTMATERIAL + 1);
	objects[SLING_AMMO].oc_material = rn2(LASTMATERIAL + 1);
	objects[CIGARETTE].oc_material = rn2(LASTMATERIAL + 1);
	objects[VARIED_GRINDER].oc_material = rn2(LASTMATERIAL + 1);
	objects[CAMO_QATAR].oc_material = rn2(LASTMATERIAL + 1);

	objects[RAINBOW_LIGHTSABER].oc_color = rn2(CLR_MAX);
	{
		int dyepotion = find_appearance_potion(APP_POTION_DYE);
		objects[dyepotion].oc_color = rn2(CLR_MAX);
	}
	while (objects[RAINBOW_LIGHTSABER].oc_color == NO_COLOR) objects[RAINBOW_LIGHTSABER].oc_color = rn2(CLR_MAX);

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
	shuffle(STANDARD_HELMET, HELM_OF_TELEPATHY, TRUE);

	/* shuffle the gloves */
	shuffle(REGULAR_GLOVES, GAUNTLETS_OF_DEXTERITY, TRUE);

    /* shuffle the robes */
    shuffle(PLAIN_ROBE, ROBE_OF_WEAKNESS, TRUE);

	/* shuffle the cloaks */
	shuffle(CLOAK_OF_PROTECTION, CLOAK_OF_DISPLACEMENT, TRUE);

	/* shuffle the boots [if they change, update find_skates() below] */
	shuffle(SPEED_BOOTS, LEVITATION_BOOTS, TRUE);
}

/* find the object index for a specified randomized appearance armor piece, e.g. APP_SNOW_BOOTS */
int
find_appearance_armor(armortype)
int armortype;
{
    register int i;

    for (i = HAWAIIAN_SHIRT; i <= LEVITATION_BOOTS; i++) {
	if (itemnumwithappearance(i, armortype)) return i;
    }

    impossible("armor appearance %d not found?", armortype);
    return -1;	/* not 0, or caller would try again each move */

}

int
find_appearance_potion(potiontype)
int potiontype;
{
    register int i;

    for (i = POT_BOOZE; i <= POT_AMNESIA; i++) {
	if (itemnumwithappearance(i, potiontype)) return i;
    }

    impossible("potion appearance %d not found?", potiontype);
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
	    /*bwrite(fd, (void *)mons,
		   sizeof(struct permonst) * NUMMONS);*/
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
	/*mread(fd, (void *) mons, sizeof(struct permonst) * NUMMONS);*/
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

	if (Race_if(PM_DE_ENERGISER)) return;
	if (u.deenergiserpersist) return;

	if (mark_as_known) {
	    objects[oindx].oc_name_known = 1;
	    if (credit_hero) exercise(A_WIS, TRUE);
	}
	if (moves > 1L) update_inventory();

	if ((Role_if(PM_PRIEST) || Role_if(PM_NECROMANCER) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA)) && !(LeftInventoryBug || u.uprops[LEFT_INVENTORY].extrinsic || have_leftinventorystone()) ) {
		use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? 3 : 1);

	}

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

    if (PlayerUninformation) {
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

    ct += disp_artifact_discoveries(tmpwin);

    /* several classes are omitted from packorder; one is of interest here */
    strcpy(classes, flags.inv_order);
    if (!index(classes, MAXOCLASSES)) {
	s = eos(classes);
	*s++ = MAXOCLASSES;
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
    if (!index(classes, MAXOCLASSES)) {
	s = eos(classes);
	*s++ = MAXOCLASSES;
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

/*	SCCS Id: @(#)tilemap.c	3.3	2000/06/04	*/
/* NetHack may be freely redistributed.  See license for details. */

/*
 *	This source file is compiled twice:
 *	once without TILETEXT defined to make tilemap.{o,obj},
 *	then again with it defined to produce tiletxt.{o,obj}.
 */

#include "hack.h"

const char * FDECL(tilename, (int, int));
void NDECL(init_tilemap);
void FDECL(process_substitutions, (FILE *));

#ifdef MICRO
#undef exit
#if !defined(MSDOS) && !defined(WIN32)
extern void FDECL(exit, (int));
#endif
#endif

#define MON_GLYPH 1
#define OBJ_GLYPH 2
#define OTH_GLYPH 3	/* fortunately unnecessary */
#define COM_GLYPH 4	/* combined glyphs (for importing tile sets) */

/* note that the ifdefs here should be the opposite sense from monst.c/
 * objects.c/rm.h
 */

struct conditionals {
	int sequence, predecessor;
	const char *name;
} conditionals[] = {
#ifndef CHARON /* not supported yet */
	{ MON_GLYPH, PM_HELL_HOUND, "Cerberus" },
#endif

	/* commented out in monst.c at present */
	{ MON_GLYPH, PM_BLACK_LIGHT, "zruty" },

#ifndef KOPS
	{ MON_GLYPH, PM_VORPAL_JABBERWOCK, "Keystone Kop" },
	{ MON_GLYPH, PM_VORPAL_JABBERWOCK, "Kop Sergeant" },
	{ MON_GLYPH, PM_VORPAL_JABBERWOCK, "Kop Lieutenant" },
	{ MON_GLYPH, PM_VORPAL_JABBERWOCK, "Kop Kaptain" },
#endif

#ifndef BLACKMARKET
	{ MON_GLYPH, PM_SHOPKEEPER, "black marketeer" },
#endif

#ifndef CHARON /* not supported yet */
	{ MON_GLYPH, PM_CROESUS, "Charon" },
#endif

#ifndef MAIL
	{ MON_GLYPH, PM_FAMINE, "mail daemon" },
#endif

/* Messing up Djinni probs */
	{ MON_GLYPH, PM_DJINNI, "efreeti" },
	{ MON_GLYPH, PM_DJINNI, "dao" },
	{ MON_GLYPH, PM_DJINNI, "marid" },

#ifndef TOURIST
	{ MON_GLYPH, PM_SAMURAI, "tourist" },
#endif

#ifndef YEOMAN
	{ MON_GLYPH, PM_VALKYRIE, "yeoman" },
#endif

	/* commented out in monst.c at present */
	{ MON_GLYPH, PM_SHAMAN_KARNOV, "Master Shifter" },
	{ MON_GLYPH, PM_SHAMAN_KARNOV, "Thorin" },
	{ MON_GLYPH, PM_SHAMAN_KARNOV, "Earendil" },
	{ MON_GLYPH, PM_SHAMAN_KARNOV, "Elwing" },

#ifndef TOURIST
	{ MON_GLYPH, PM_LORD_SATO, "Twoflower" },
#endif

	/* commented out in monst.c at present */
	{ MON_GLYPH, PM_WIZARD_OF_BALANCE, "Lolth" },

#ifndef YEOMAN
	{ MON_GLYPH, PM_WIZARD_OF_BALANCE, "Chief Yeoman Warder" },
#endif

	/* commented out in monst.c at present */
	{ MON_GLYPH, PM_CHROMATIC_DRAGON, "Transmuter" },
	{ MON_GLYPH, PM_CHROMATIC_DRAGON, "Smaug" },
	{ MON_GLYPH, PM_CHROMATIC_DRAGON, "Goblin King" },
	{ MON_GLYPH, PM_WATER_MAGE, "Lareth" },
	{ MON_GLYPH, PM_CYCLOPS, "Gollum" },
	{ MON_GLYPH, PM_IXOTH, "Sir Lorimar" },

#ifndef YEOMAN
	{ MON_GLYPH, PM_DARK_ONE, "Colonel Blood" },
#endif

	/* commented out in monst.c at present */
	{ MON_GLYPH, PM_NEANDERTHAL, "shifter" },
	{ MON_GLYPH, PM_NEANDERTHAL, "dwarf warrior" },
	{ MON_GLYPH, PM_NEANDERTHAL, "High-elf" },
	{ MON_GLYPH, PM_FROSTER, "fiend" },
	{ MON_GLYPH, PM_ATTENDANT, "proudfoot" },
	{ MON_GLYPH, PM_ATTENDANT, "intern" },

	{ MON_GLYPH, PM_NINJA, "ronin" },

#ifndef TOURIST
	{ MON_GLYPH, PM_ROSHI, "guide" },
#endif

#ifndef YEOMAN
	{ MON_GLYPH, PM_APPRENTICE, "Yeoman Warder" },
	/* commented out in monst.c at present */
	{ MON_GLYPH, PM_APPRENTICE, "Farmer Maggot" },
#else
	/* commented out in monst.c at present */
	{ MON_GLYPH, PM_YEOMAN_WARDER, "Farmer Maggot" },
#endif

#ifndef FIREARMS
	{ OBJ_GLYPH, SLING, "pistol" },
	{ OBJ_GLYPH, SLING, "submachine gun" },
	{ OBJ_GLYPH, SLING, "heavy machine gun" },
	{ OBJ_GLYPH, SLING, "rifle" },
	{ OBJ_GLYPH, SLING, "assault rifle" },
	{ OBJ_GLYPH, SLING, "sniper rifle" },
	{ OBJ_GLYPH, SLING, "shotgun" },
	{ OBJ_GLYPH, SLING, "auto shotgun" },
	{ OBJ_GLYPH, SLING, "rocket launcher" },
	{ OBJ_GLYPH, SLING, "grenade launcher" },
	{ OBJ_GLYPH, SLING, "bullet" },
	{ OBJ_GLYPH, SLING, "silver bullet" },
	{ OBJ_GLYPH, SLING, "shotgun shell" },
	{ OBJ_GLYPH, SLING, "rocket" },
	{ OBJ_GLYPH, SLING, "frag grenade" },
	{ OBJ_GLYPH, SLING, "gas grenade" },
	{ OBJ_GLYPH, SLING, "stick of dynamite" },
#endif

#ifndef KOPS
	{ OBJ_GLYPH, BULLWHIP, "rubber hose" },
#endif
#ifndef TOURIST

#ifdef KOPS
	{ OBJ_GLYPH, RUBBER_HOSE, "Hawaiian shirt" },
	{ OBJ_GLYPH, RUBBER_HOSE, "T-shirt" },
#else
	{ OBJ_GLYPH, BULLWHIP, "Hawaiian shirt" },
	{ OBJ_GLYPH, BULLWHIP, "T-shirt" },
#endif
	{ OBJ_GLYPH, LOCK_PICK, "credit card" },
#endif

#ifndef P_SPOON
	{ OBJ_GLYPH, BEARTRAP, "spoon" },
#endif

#ifndef D_SABER
	{ OBJ_GLYPH, GREEN_LIGHTSABER, "blue lightsaber" },
#endif

#ifndef TOURIST
	{ OBJ_GLYPH, RED_DOUBLE_LIGHTSABER, "expensive camera" },
#endif

#ifndef STEED
	{ OBJ_GLYPH, TOWEL, "saddle" },
#endif


	/* allow slime mold to look like slice of pizza, since we
	 * don't know what a slime mold should look like when renamed anyway
	 */
#ifndef MAIL
	{ OBJ_GLYPH, SCR_STINKING_CLOUD+4, "stamped / mail" },
#endif
	{ 0, 0, 0}
};


/*
 * Some entries in glyph2tile[] should be substituted for on various levels.
 * The tiles used for the substitute entries will follow the usual ones in
 * other.til in the order given here, which should have every substitution
 * for the same set of tiles grouped together.  You will have to change
 * more code in process_substitutions()/substitute_tiles() if the sets
 * overlap in the future.
 */
struct substitute {
	int first_glyph, last_glyph;
	const char *sub_name;		/* for explanations */
	const char *level_test;
} substitutes[] = {
	{ GLYPH_CMAP_OFF + S_vwall, GLYPH_CMAP_OFF + S_trwall,
					"mine walls", "In_mines(plev)" },
	{ GLYPH_CMAP_OFF + S_vwall, GLYPH_CMAP_OFF + S_trwall,
					"gehennom walls", "In_hell(plev)" },
	{ GLYPH_CMAP_OFF + S_vwall, GLYPH_CMAP_OFF + S_trwall,
					"knox walls", "Is_knox(plev)" },
	{ GLYPH_CMAP_OFF + S_vwall, GLYPH_CMAP_OFF + S_trwall,
					"sokoban walls", "In_sokoban(plev)" }
};


#ifdef TILETEXT

/*
 * ALI
 *
 * The missing cmap names. These allow us to intelligently interpret
 * tilesets from other variants of NetHack (eg., Mitsuhiro Itakura's 32x32
 * tileset which is based on JNetHack).
 */

struct {
	int cmap;
	const char *name;
} cmaps[] = {
	S_digbeam,	"dig beam",
	S_flashbeam,	"camera flash",
	S_boomleft,	"thrown boomerang, open left",
	S_boomright,	"thrown boomerang, open right",
	S_ss1,		"magic shield 1",
	S_ss2,		"magic shield 2",
	S_ss3,		"magic shield 3",
	S_ss4,		"magic shield 4",
	S_sw_tl,	"swallow top left",
	S_sw_tc,	"swallow top center",
	S_sw_tr,	"swallow top right",
	S_sw_ml,	"swallow middle left",
	S_sw_mr,	"swallow middle right",
	S_sw_bl,	"swallow bottom left",
	S_sw_bc,	"swallow bottom center",
	S_sw_br,	"swallow bottom right",
#ifdef S_mexplode1
	S_mexplode1,	"magical explosion top left",
	S_mexplode2,	"magical explosion top center",
	S_mexplode3,	"magical explosion top right",
	S_mexplode4,	"magical explosion middle left",
	S_mexplode5,	"magical explosion middle center",
	S_mexplode6,	"magical explosion middle right",
	S_mexplode7,	"magical explosion bottom left",
	S_mexplode8,	"magical explosion bottom center",
	S_mexplode9,	"magical explosion bottom right",
#endif
	S_explode1,	"explosion top left",
	S_explode2,	"explosion top center",
	S_explode3,	"explosion top right",
	S_explode4,	"explosion middle left",
	S_explode5,	"explosion middle center",
	S_explode6,	"explosion middle right",
	S_explode7,	"explosion bottom left",
	S_explode8,	"explosion bottom center",
	S_explode9,	"explosion bottom right",
#ifdef S_cexplode1
	S_cexplode1,	"cold explosion top left",
	S_cexplode2,	"cold explosion top center",
	S_cexplode3,	"cold explosion top right",
	S_cexplode4,	"cold explosion middle left",
	S_cexplode5,	"cold explosion middle center",
	S_cexplode6,	"cold explosion middle right",
	S_cexplode7,	"cold explosion bottom left",
	S_cexplode8,	"cold explosion bottom center",
	S_cexplode9,	"cold explosion bottom right",
#endif
#ifdef S_dexplode1
	S_dexplode1,	"death explosion top left",
	S_dexplode2,	"death explosion top center",
	S_dexplode3,	"death explosion top right",
	S_dexplode4,	"death explosion middle left",
	S_dexplode5,	"death explosion middle center",
	S_dexplode6,	"death explosion middle right",
	S_dexplode7,	"death explosion bottom left",
	S_dexplode8,	"death explosion bottom center",
	S_dexplode9,	"death explosion bottom right",
#endif
#ifdef S_lexplode1
	S_lexplode1,	"lightning explosion top left",
	S_lexplode2,	"lightning explosion top center",
	S_lexplode3,	"lightning explosion top right",
	S_lexplode4,	"lightning explosion middle left",
	S_lexplode5,	"lightning explosion middle center",
	S_lexplode6,	"lightning explosion middle right",
	S_lexplode7,	"lightning explosion bottom left",
	S_lexplode8,	"lightning explosion bottom center",
	S_lexplode9,	"lightning explosion bottom right",
#endif
#ifdef S_pexplode1
	S_pexplode1,	"poison explosion top left",
	S_pexplode2,	"poison explosion top center",
	S_pexplode3,	"poison explosion top right",
	S_pexplode4,	"poison explosion middle left",
	S_pexplode5,	"poison explosion middle center",
	S_pexplode6,	"poison explosion middle right",
	S_pexplode7,	"poison explosion bottom left",
	S_pexplode8,	"poison explosion bottom center",
	S_pexplode9,	"poison explosion bottom right",
#endif
#ifdef S_aexplode1
	S_aexplode1,	"acid explosion top left",
	S_aexplode2,	"acid explosion top center",
	S_aexplode3,	"acid explosion top right",
	S_aexplode4,	"acid explosion middle left",
	S_aexplode5,	"acid explosion middle center",
	S_aexplode6,	"acid explosion middle right",
	S_aexplode7,	"acid explosion bottom left",
	S_aexplode8,	"acid explosion bottom center",
	S_aexplode9,	"acid explosion bottom right",
#endif
};

/*
 * entry is the position of the tile within the monsters/objects/other set
 */
const char *
tilename(set, entry)
int set, entry;
{
	int i, j, condnum, tilenum;
	int in_set, oth_origin;
	static char buf[BUFSZ];

	/* Note:  these initializers don't do anything except guarantee that
		we're linked properly.
	*/
	monst_init();
	objects_init();
	(void) def_char_to_objclass(']');

	condnum = tilenum = 0;

	in_set = set == MON_GLYPH || set == COM_GLYPH;
        for (i = 0; i < NUMMONS; i++) {
		if (in_set && tilenum == entry)
			return mons[i].mname;
		tilenum++;
		while (conditionals[condnum].sequence == MON_GLYPH &&
			conditionals[condnum].predecessor == i) {
			if (in_set && tilenum == entry)
				return conditionals[condnum].name;
			condnum++;
			tilenum++;
		}
	}
	if (in_set && tilenum == entry)
		return "invisible monster";
	tilenum++;

	if (set != COM_GLYPH)
		tilenum = 0;	/* set-relative number */
	in_set = set == OBJ_GLYPH || set == COM_GLYPH;
	for (i = 0; i < NUM_OBJECTS; i++) {
		/* prefer to give the description - that's all the tile's
		 * appearance should reveal */
		if (in_set && tilenum == entry) {
			if ( !obj_descr[i].oc_descr )
			return obj_descr[i].oc_name;
			if ( !obj_descr[i].oc_name )
			    return obj_descr[i].oc_descr;

			Sprintf(buf, "%s / %s",
				obj_descr[i].oc_descr,
				obj_descr[i].oc_name);
			return buf;
		}

		tilenum++;
		while (conditionals[condnum].sequence == OBJ_GLYPH &&
			conditionals[condnum].predecessor == i) {
			if (in_set && tilenum == entry)
				return conditionals[condnum].name;
			condnum++;
			tilenum++;
		}
	}

	if (set != COM_GLYPH)
		tilenum = 0;	/* set-relative number */
	in_set = set == OTH_GLYPH || set == COM_GLYPH;
	oth_origin = tilenum;
	for (i = 0; i < MAXPCHARS; i++) {
		if (in_set && tilenum == entry) {
			if (*defsyms[i].explanation)
				return defsyms[i].explanation;
			else {
				/* if SINKS are turned off, this
				 * string won't be there (and can't be there
				 * to prevent symbol-identification and
				 * special-level mimic appearances from
				 * thinking the items exist)
				 */
				switch (i) {
				    case S_sink:
					    Sprintf(buf, "sink");
					    break;
				    default:
					for(j = 0; j < SIZE(cmaps); j++)
					    if (cmaps[j].cmap ==
					      tilenum - oth_origin)
					    {
						Sprintf(buf, "cmap / %s",
						  cmaps[j].name);
						break;
					    }
					if (j == SIZE(cmaps))
					    Sprintf(buf, "cmap %d",
					      tilenum - oth_origin);
					    break;
				}
				return buf;
			}
		}
		tilenum++;
		while (conditionals[condnum].sequence == OTH_GLYPH &&
			conditionals[condnum].predecessor == i) {
			if (in_set && tilenum == entry)
				return conditionals[condnum].name;
			condnum++;
			tilenum++;
		}
	}
	
	i = entry - tilenum;
	if (i < (NUM_ZAP << 2)) {
		if (in_set) {
			Sprintf(buf, "zap %d %d", i/4, i%4);
			return buf;
		}
	}
	tilenum += (NUM_ZAP << 2);

	i = entry - tilenum;
	if (i < WARNCOUNT) {
		if (set == OTH_GLYPH) {
			Sprintf(buf, "warning %d", i);
			return buf;
	        }
	}
	tilenum += WARNCOUNT;

	for (i = 0; i < SIZE(substitutes); i++) {
	    j = entry - tilenum;
	    if (j <= substitutes[i].last_glyph - substitutes[i].first_glyph) {
		if (in_set) {
		    Sprintf(buf, "sub %s %d", substitutes[i].sub_name, j);
		    return buf;
		}
	    }
	    tilenum += substitutes[i].last_glyph
				- substitutes[i].first_glyph + 1;
	}

	Sprintf(buf, "unknown %d %d", set, entry);
	return buf;
}

#else	/* TILETEXT */

#define TILE_FILE_H	"tile.h"
#define TILE_FILE_C	"tile.c"

#ifdef AMIGA
# define INCLUDE_TEMPLATE	"NH:include/t.%s"
# define SOURCE_TEMPLATE	"NH:src/%s"
# define SHARE_IN_TEMPLATE	"NH:share/%s"
#else
# ifdef MAC
#   define INCLUDE_TEMPLATE	":include:%s"
#   define SOURCE_TEMPLATE	":src:%s"
#   define SHARE_IN_TEMPLATE	":share:%s"
# else
#  ifdef OS2
#   define INCLUDE_TEMPLATE	"..\\include\\%s"
#   define SOURCE_TEMPLATE	"..\\src\\%s"
#   define SHARE_IN_TEMPLATE	"..\\win\\share\\%s"
#  else
#   define INCLUDE_TEMPLATE	"../include/%s"
#   define SOURCE_TEMPLATE	"../src/%s"
#   define SHARE_IN_TEMPLATE	"../win/share/%s"
#  endif
# endif
#endif

short tilemap[MAX_GLYPH];
int lastmontile, lastobjtile, lastothtile;

static char     in_line[256];

/* Number of tiles for invisible monsters */
#define NUM_INVIS_TILES 1

/*
 * ALI
 *
 * Compute the value of ceil(sqrt(c)) using only integer arithmetic.
 *
 * Newton-Raphson gives us the following algorithm for solving sqrt(c):
 *
 *            a[n]^2+c
 * a[n+1]  =  --------
 *             2*a[n]
 *
 * It would be tempting to use a[n+1] = (a[n]^2+c+2*a[n]-1) div 2*a[n]
 * to solve for ceil(sqrt(c)) but this does not converge correctly.
 * Instead we solve floor(sqrt(c)) first and then adjust as necessary.
 *
 * The proposed algorithm to solve floor(sqrt(c)):
 *
 * a[n+1] = a[n]^2+c div 2*a[n]
 *
 * If we define the deviation of approximation n as follows:
 *
 * e[n] = a[n] - sqrt(c)
 *
 * Then it follows that:
 *
 *              e[n]^2
 * e[n+1] = ---------------
 *          2(e[n]+sqrt(c))
 *
 * The sequence will converge to the solution if:
 *
 * | e[n+1] | < | e[n] |
 *
 * which becomes:
 *
 *                      |     e[n]^2      |
 *                      | --------------- | < | e[n] |
 *                      | 2(e[n]+sqrt(c)) |
 *
 * This splits into three cases:
 *
 * If e[n] > 0          * If 0 > e[n] >= -sqrt(c) * If e[n] < -sqrt(c)
 *                      *                         *
 * Converges iff:       * Converges iff:          * Converges iff:
 *                      *             2           *
 *    e[n] > -2*sqrt(c) *    e[n] > - - sqrt(c)   *    e[n] > -2*sqrt(c)
 *                      *             3           *
 *                      *                 sqrt(c) *
 * True for all cases.  * True iff a[n] > ------- * True iff 0 > a[n] > -sqrt(c)
 *                      *                    3    *
 *
 * Case 3 represents failure, but this can be avoided by choosing a positive
 * initial value. In both case 1 and case 2, e[n+1] is positive regardless
 * of the sign of e[n]. It therefore follows that even if an initial value
 * between 0 and sqrt(c)/3 is chosen, we will only diverge for one iteration.
 *
 * Therefore the algorithm will converge correctly as long as we start
 * with a positve inital value (it will converge to the negative root if
 * we start with a negative initial value and fail if we start with zero).
 *
 * We choose an initial value designed to be close to the solution we expect
 * for typical values of c. This also makes it unlikely that we will cause
 * a divergence. If we do, it will only take a few more iterations.
 */

int ceil_sqrt(c)
int c;
{
    int a=c/36,la;	/* Approximation and last approximation */
    /* Compute floor(sqrt(c)) */
    do
    {
	la=a;
	a=(a*a+c)/(2*a);
    } while (a!=la);
    /* Adjust for ceil(sqrt(c)) */
    return a*a==c?a:a+1;
}

/*
 * set up array to map glyph numbers to tile numbers
 *
 * assumes tiles are numbered sequentially through monsters/objects/other,
 * with entries for all supported compilation options
 *
 * "other" contains cmap and zaps (the swallow sets are a repeated portion
 * of cmap), as well as the "flash" glyphs for the new warning system
 * introduced in 3.3.1.
 */
void
init_tilemap()
{
	int i, j, condnum, tilenum;
	int corpsetile, swallowbase;

	for (i = 0; i < MAX_GLYPH; i++) {
		tilemap[i] = -1;
	}

	corpsetile = NUMMONS + NUM_INVIS_TILES + CORPSE;
	swallowbase= NUMMONS + NUM_INVIS_TILES + NUM_OBJECTS + S_sw_tl;

	/* add number compiled out */
	for (i = 0; conditionals[i].sequence; i++) {
		switch (conditionals[i].sequence) {
			case MON_GLYPH:
				corpsetile++;
				swallowbase++;
				break;
			case OBJ_GLYPH:
				if (conditionals[i].predecessor < CORPSE)
					corpsetile++;
				swallowbase++;
				break;
			case OTH_GLYPH:
				if (conditionals[i].predecessor < S_sw_tl)
					swallowbase++;
				break;
		}
	}

	condnum = tilenum = 0;
	for (i = 0; i < NUMMONS; i++) {
		tilemap[GLYPH_MON_OFF+i] = tilenum;
		tilemap[GLYPH_PET_OFF+i] = tilenum;
		tilemap[GLYPH_DETECT_OFF+i] = tilenum;
		tilemap[GLYPH_RIDDEN_OFF+i] = tilenum;
		tilemap[GLYPH_BODY_OFF+i] = corpsetile;
		j = GLYPH_SWALLOW_OFF + 8*i;
		tilemap[j] = swallowbase;
		tilemap[j+1] = swallowbase+1;
		tilemap[j+2] = swallowbase+2;
		tilemap[j+3] = swallowbase+3;
		tilemap[j+4] = swallowbase+4;
		tilemap[j+5] = swallowbase+5;
		tilemap[j+6] = swallowbase+6;
		tilemap[j+7] = swallowbase+7;
		tilenum++;
		while (conditionals[condnum].sequence == MON_GLYPH &&
			conditionals[condnum].predecessor == i) {
			condnum++;
			tilenum++;
		}
	}
	tilemap[GLYPH_INVISIBLE] = tilenum++;
	lastmontile = tilenum - 1;

	for (i = 0; i < NUM_OBJECTS; i++) {
		tilemap[GLYPH_OBJ_OFF+i] = tilenum;
		tilenum++;
		while (conditionals[condnum].sequence == OBJ_GLYPH &&
			conditionals[condnum].predecessor == i) {
			condnum++;
			tilenum++;
		}
	}
	lastobjtile = tilenum - 1;

	for (i = 0; i < MAXPCHARS; i++) {
		tilemap[GLYPH_CMAP_OFF+i] = tilenum;
		tilenum++;
		while (conditionals[condnum].sequence == OTH_GLYPH &&
			conditionals[condnum].predecessor == i) {
			condnum++;
			tilenum++;
		}
	}

	for (i = 0; i < NUM_ZAP << 2; i++) {
		tilemap[GLYPH_ZAP_OFF+i] = tilenum;
		tilenum++;
		while (conditionals[condnum].sequence == OTH_GLYPH &&
			conditionals[condnum].predecessor == (i + MAXPCHARS)) {
			condnum++;
			tilenum++;
		}
	}

	for (i = 0; i < WARNCOUNT; i++) {
		tilemap[GLYPH_WARNING_OFF+i] = tilenum;
		tilenum++;
	}

	lastothtile = tilenum - 1;
}

const char *prolog[] = {
	"",
	"",
	"void",
	"substitute_tiles(plev)",
	"d_level *plev;",
	"{",
	"\tint i;",
	""
};

const char *epilog[] = {
	"}"
};

/* write out the substitutions in an easily-used form. */
void
process_substitutions(ofp)
FILE *ofp;
{
	int i, j, k, span, start;

	fprintf(ofp, "\n\n");

	j = 0;	/* unnecessary */
	span = -1;
	for (i = 0; i < SIZE(substitutes); i++) {
	    if (i == 0
		|| substitutes[i].first_glyph != substitutes[j].first_glyph
		|| substitutes[i].last_glyph != substitutes[j].last_glyph) {
			j = i;
			span++;
			fprintf(ofp, "short std_tiles%d[] = { ", span);
			for (k = substitutes[i].first_glyph;
				k < substitutes[i].last_glyph; k++)
					fprintf(ofp, "%d, ", tilemap[k]);
			fprintf(ofp, "%d };\n",
				tilemap[substitutes[i].last_glyph]);
	    }
	}

	for (i = 0; i < SIZE(prolog); i++) {
		fprintf(ofp, "%s\n", prolog[i]);
	}
	j = -1;
	span = -1;
	start = lastothtile + 1;
	for (i = 0; i < SIZE(substitutes); i++) {
	    if (i == 0
		    || substitutes[i].first_glyph != substitutes[j].first_glyph
		    || substitutes[i].last_glyph != substitutes[j].last_glyph) {
		if (i != 0) {	/* finish previous span */
		    fprintf(ofp, "\t} else {\n");
		    fprintf(ofp, "\t\tfor (i = %d; i <= %d; i++)\n",
					substitutes[j].first_glyph,
					substitutes[j].last_glyph);
		    fprintf(ofp, "\t\t\tglyph2tile[i] = std_tiles%d[i - %d];\n",
					span, substitutes[j].first_glyph);
		    fprintf(ofp, "\t}\n\n");
		}
		j = i;
		span++;
	    }
	    if (i != j) fprintf(ofp, "\t} else ");
	    fprintf(ofp, "\tif (%s) {\n", substitutes[i].level_test);
	    fprintf(ofp, "\t\tfor (i = %d; i <= %d; i++)\n",
				substitutes[i].first_glyph,
				substitutes[i].last_glyph);
	    fprintf(ofp, "\t\t\tglyph2tile[i] = %d + i - %d;\n",
				start, substitutes[i].first_glyph);
	    start += substitutes[i].last_glyph - substitutes[i].first_glyph + 1;
	}
	/* finish last span */
	fprintf(ofp, "\t} else {\n");
	fprintf(ofp, "\t\tfor (i = %d; i <= %d; i++)\n",
			    substitutes[j].first_glyph,
			    substitutes[j].last_glyph);
	fprintf(ofp, "\t\t\tglyph2tile[i] = std_tiles%d[i - %d];\n",
			    span, substitutes[j].first_glyph);
	fprintf(ofp, "\t}\n\n");

	for (i = 0; i < SIZE(epilog); i++) {
		fprintf(ofp, "%s\n", epilog[i]);
	}

	fprintf(ofp, "\nint total_tiles_used = %d;\n", start);
	i = ceil_sqrt(start);
	fprintf(ofp, "int tiles_per_row = %d;\n", i);
	fprintf(ofp, "int tiles_per_col = %d;\n", (start + i - 1) / i);
	lastothtile = start - 1;
}

int main()
{
    register int i;
    char filename[30];
    FILE *ifp,*ofp;

    init_tilemap();

    /*
     * create the source file, "tile.c"
     */
    Sprintf(filename, SOURCE_TEMPLATE, TILE_FILE_C);
    if (!(ofp = fopen(filename, "w"))) {
	    perror(filename);
	    exit(EXIT_FAILURE);
    }
    fprintf(ofp,"/* This file is automatically generated.  Do not edit. */\n");
    fprintf(ofp,"\n#include \"hack.h\"\n\n");
    fprintf(ofp,"short glyph2tile[MAX_GLYPH] = {\n");

    for (i = 0; i < MAX_GLYPH; i++) {
	fprintf(ofp,"%2d,%c", tilemap[i], (i % 12) ? ' ' : '\n');
    }
    fprintf(ofp,"%s};\n", (i % 12) ? "\n" : "");

    process_substitutions(ofp);

    fprintf(ofp,"\n#define MAXMONTILE %d\n", lastmontile);
    fprintf(ofp,"#define MAXOBJTILE %d\n", lastobjtile);
    fprintf(ofp,"#define MAXOTHTILE %d\n", lastothtile);

    fprintf(ofp,"\n/*tile.c*/\n");

    fclose(ofp);

    /*
     * create the include file, "tile.h"
     */
    Sprintf(filename, SHARE_IN_TEMPLATE, TILE_FILE_H);
    if (!(ifp = fopen(filename, "r"))) {
	    perror(filename);
	    exit(EXIT_FAILURE);
    }
    Sprintf(filename, INCLUDE_TEMPLATE, TILE_FILE_H);
    if (!(ofp = fopen(filename, "w"))) {
	    perror(filename);
	    exit(EXIT_FAILURE);
    }
    fprintf(ofp,"/* This file is automatically generated.  Do not edit. */\n");

    fprintf(ofp,"\n#define TOTAL_TILES_USED %d\n", lastothtile + 1);
    i = ceil_sqrt(lastothtile + 1);
    fprintf(ofp,"#define TILES_PER_ROW %d\n", i);
    fprintf(ofp,"#define TILES_PER_COL %d\n\n", (lastothtile + i) / i);

    while (fgets(in_line, sizeof in_line, ifp) != 0)
	(void) fputs(in_line, ofp);

    fprintf(ofp,"\n/*tile.h*/\n");

    fclose(ofp);

    exit(EXIT_SUCCESS);
    /*NOTREACHED*/
    return 0;
}

#endif	/* TILETEXT */

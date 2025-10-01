/*	SCCS Id: @(#)mhitm.c	3.4	2003/01/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#include "edog.h"

extern boolean notonhead;
extern const char *breathwep[];		/* from mthrowu.c */

#define POLE_LIM 8	/* How far monsters can use pole-weapons */

#ifdef OVLB

static NEARDATA boolean vis, far_noise;
static NEARDATA long noisetime;
static NEARDATA struct obj *otmp;

static const char brief_feeling[] =
	"have a %s feeling for a moment, then it passes.";

STATIC_DCL char *mon_nam_too(char *,struct monst *,struct monst *);
STATIC_DCL void mrustm(struct monst *, struct monst *, struct obj *);
STATIC_DCL int breamm(struct monst *, struct monst *, struct attack *);
STATIC_DCL int spitmm(struct monst *, struct monst *, struct attack *);
STATIC_DCL int thrwmm(struct monst *, struct monst *);
STATIC_DCL int hitmm(struct monst *,struct monst *,struct attack *);
STATIC_DCL int gazemm(struct monst *,struct monst *,struct attack *);
STATIC_DCL int gulpmm(struct monst *,struct monst *,struct attack *);
STATIC_DCL int explmm(struct monst *,struct monst *,struct attack *);
STATIC_DCL int mdamagem(struct monst *,struct monst *,struct attack *);
STATIC_DCL void mswingsm(struct monst *, struct monst *, struct obj *);
STATIC_DCL void noises(struct monst *,struct attack *);
STATIC_DCL void missmm(struct monst *,struct monst *, int, int, struct attack *);
STATIC_DCL int passivemm(struct monst *, struct monst *, BOOLEAN_P, int, int);
STATIC_DCL void stoogejoke();

STATIC_PTR void set_lit(int,int,void *);

/* Needed for the special case of monsters wielding vorpal blades (rare).
 * If we use this a lot it should probably be a parameter to mdamagem()
 * instead of a global variable.
 */
static int dieroll;

static const char *random_joke[] = {
	"Why I ought a ...",
	"You'll get what's comming!",
	"I'll murder you!",
	"I get no respect!",
	"Right in the kisser!",
	"Wait a minute!",
	"Take it easy!",
	"Alright already!",
	"That's more like it!",
	"Well excuse me!",
	"Take that!",
	"I'll fix you!",
	"I'm sorry!",
	"Your mama!",
	"Shut up!",
	"Listen you!",
	"Pardon me!",
	"Not that!",
	"Quiet!",
	"Relax!",
	"Certainly!",
	"Ouch!",
	"What happened?",
	"What was that for?",
	"What's the matter with you?",
	"Oh Yea?",
	"Wise guy eh?",
	"How about a knuckle sandwich?",
	"You coward!",
	"You rat you!",
	"You chuckelhead!",
	"You bonehead!",
	"You numbskull!",
	"Nyak Nyak Nyak ...",
	"Woop Woop Woop Woop ..."
};

/* returns mon_nam(mon) relative to other_mon; normal name unless they're
   the same, in which case the reference is to {him|her|it} self */
STATIC_OVL char *
mon_nam_too(outbuf, mon, other_mon)
char *outbuf;
struct monst *mon, *other_mon;
{
	strcpy(outbuf, mon_nam(mon));
	if (mon == other_mon)
	    switch (pronoun_gender(mon)) {
	    case 0:	strcpy(outbuf, "himself");  break;
	    case 1:	strcpy(outbuf, "herself");  break;
	    default:	strcpy(outbuf, "itself"); break;
	    }
	return outbuf;
}

STATIC_OVL void
noises(magr, mattk)
	register struct monst *magr;
	register struct	attack *mattk;
{
	boolean farq = (distu(magr->mx, magr->my) > 15);

	if(flags.soundok && (farq != far_noise || moves-noisetime > 10)) {
		far_noise = farq;
		noisetime = moves;
		You_hear("%s%s.",
			(mattk->aatyp == AT_EXPL) ? "an explosion" : "some noises",
			farq ? " in the distance" : "");
	}
}

STATIC_OVL
void
missmm(magr, mdef, target, roll, mattk)
	register struct monst *magr, *mdef;
	struct attack *mattk;
	int target, roll;
{
	boolean nearmiss = (target == roll);
	const char *fmt;
        char buf[BUFSZ], mon_name[BUFSZ];

	register struct obj *blocker = (struct obj *)0;
	long mwflags = mdef->misc_worn_check;

		/* 3 values for blocker
		 *	No blocker:  (struct obj *) 0
		 * 	Piece of armour:  object
		 */

	/* This is a hack,  since there is no fast equivalent for uarm, uarms, etc.
	 * Technically, we really should check from the inside out...
	 */
	if (target < roll) {
	    for (blocker = mdef->minvent; blocker; blocker = blocker->nobj) {
		if (blocker->owornmask & mwflags) {
			target += ARM_BONUS(blocker);
			if (target > roll) break;
		}
	    }
	}

	if (vis) {
		if (!canspotmon(magr) && !(monstersoundtype(magr) == MS_DEEPSTATE) && !(magr->egotype_deepstatemember))
		    map_invisible(magr->mx, magr->my);
		if (!canspotmon(mdef) && !(monstersoundtype(mdef) == MS_DEEPSTATE) && !(mdef->egotype_deepstatemember))
		    map_invisible(mdef->mx, mdef->my);
		if (mdef->m_ap_type) seemimic(mdef);
		if (magr->m_ap_type) seemimic(magr);
		if (flags.verbose && !nearmiss && blocker) {
			fmt = "%s %s blocks";
			sprintf(buf,fmt, s_suffix(Monnam(mdef)),
				aobjnam(blocker, (char *)0));
	                pline("%s %s.", buf, mon_nam_too(mon_name, magr, mdef));
		} else {
		fmt = (could_seduce(magr,mdef,mattk) && !magr->mcan) ?
				"%s pretends to be friendly to" :
				((flags.verbose && nearmiss) ? "%s just misses" :
				  "%s misses");
		sprintf(buf, fmt, Monnam(magr));
	                pline("%s %s.", buf, mon_nam_too(mon_name, mdef, magr));
		}
	} else  noises(magr, mattk);
}

/*
 *  fightm()  -- fight some other monster
 *
 *  Returns:
 *	0 - Monster did nothing.
 *	1 - If the monster made an attack.  The monster might have died.
 *
 *  There is an exception to the above.  If mtmp has the hero swallowed,
 *  then we report that the monster did nothing so it will continue to
 *  digest the hero.
 */
int
fightm(mtmp)		/* have monsters fight each other */
	register struct monst *mtmp;
{
	if (DEADMONSTER(mtmp)) return 0;

	register struct monst *mon, *nmon;
	int result, has_u_swallowed;
#ifdef LINT
	nmon = 0;
#endif
	/* perhaps the monster will resist Conflict */
	if(resist(mtmp, RING_CLASS, 0, 0))
	    return(0);
	if(resist(mtmp, RING_CLASS, 0, 0))
	    return(0);
	if (!rn2(2)) return(0);
	/* they're now highly resistant to conflict, because otherwise things would be too easy --Amy */

	if(u.ustuck == mtmp) {
	    /* perhaps we're holding it... */
	    if(itsstuck(mtmp))
		return(0);
	}
	has_u_swallowed = (u.uswallow && (mtmp == u.ustuck));

	for(mon = fmon; mon; mon = nmon) {
	    nmon = mon->nmon;
	    if(nmon == mtmp) nmon = mtmp->nmon;
	    /* Be careful to ignore monsters that are already dead, since we
	     * might be calling this before we've cleaned them up.  This can
	     * happen if the monster attacked a cockatrice bare-handedly, for
	     * instance.
	     */
	    if(mon != mtmp && !DEADMONSTER(mon)) {
		if(monnear(mtmp,mon->mx,mon->my)) {
		    if(!u.uswallow && (mtmp == u.ustuck)) {
			if(!rn2(4)) {
			    pline("%s releases you!", Monnam(mtmp));
			    setustuck(0);
			} else
			    break;
		    }

		    /* mtmp can be killed */
		    bhitpos.x = mon->mx;
		    bhitpos.y = mon->my;
		    notonhead = 0;
		    result = mattackm(mtmp,mon);

		    if (result & MM_AGR_DIED) return 1;	/* mtmp died */
		    /*
		     *  If mtmp has the hero swallowed, lie and say there
		     *  was no attack (this allows mtmp to digest the hero).
		     */
		    if (has_u_swallowed) return 0;

		    /* Allow attacked monsters a chance to hit back. Primarily
		     * to allow monsters that resist conflict to respond.
		     * Amy edit: since monsters can lash out at pets now, the pets should have a considerable chance of
		     * retaliating, regardless of that weird movement value
		     */

		    if ( ((result & MM_HIT) || (mon && mon->mtame && mtmp && !mtmp->mtame && !rn2(4))) && !(result & MM_DEF_DIED) &&
			((rn2(4) && mon && mon->movement >= NORMAL_SPEED) || (mon && mon->mtame && mtmp && !mtmp->mtame))
			) {
			mon->movement -= NORMAL_SPEED;
			if (mon->movement < 0) mon->movement = 0; /* fail safe */
			notonhead = 0;

			if (!DEADMONSTER(mon) && !DEADMONSTER(mtmp)) {
				(void) mattackm(mon, mtmp);	/* return attack */
			}
		    }

		    return ((result & MM_HIT) ? 1 : 0);
		}
	    }
	}
	return 0;
}

/*
 * mattackm() -- a monster attacks another monster.
 *
 * This function returns a result bitfield:
 *
 *	    --------- aggressor died
 *	   /  ------- defender died
 *	  /  /  ----- defender was hit
 *	 /  /  /
 *	x  x  x
 *
 *	0x4	MM_AGR_DIED
 *	0x2	MM_DEF_DIED
 *	0x1	MM_HIT
 *	0x0	MM_MISS
 *
 * Each successive attack has a lower probability of hitting.  Some rely on the
 * success of previous attacks.  ** this doen't seem to be implemented -dl **
 *
 * In the case of exploding monsters, the monster dies as well.
 */
int
mattackm(magr, mdef)
    register struct monst *magr,*mdef;
{
    int		    i,		/* loop counter */
		    tmp,	/* amour class difference */
		    strike,	/* hit this attack */
		    attk,	/* attack attempted this time */
		    struck = 0,	/* hit at least once */
		    res[NATTK];	/* results of all attacks */
    int magrlev, magrhih; /* for to-hit calculations */
    struct attack   *mattk, alt_attk;
    struct permonst *pa, *pd;
    struct attack *a;
    struct permonst *mdat2;
    /*
     * Pets don't use "ranged" attacks for fear of hitting their master
     */
    boolean range;

	char buf[BUFSZ];

    if (!magr || !mdef) return(MM_MISS);		/* mike@genat */
    if (DEADMONSTER(mdef)) { /* catchall by Amy */
	return(MM_MISS);
    }
    if (DEADMONSTER(magr)) { /* shouldn't happen, but who knows? */
	return(MM_MISS);
    }
    if (mdef->handytime) return(MM_MISS);
    /* note by Amy: it's weird if pets can still fight back while eating, so now they can't */
    if (!magr->mcanmove || magr->msleeping || magr->meating) return(MM_MISS);
    pa = magr->data;  pd = mdef->data;

    /* Grid bugs cannot attack at an angle. */
    if ((isgridbug(pa) || (uwep && uwep->oartifact == ART_EGRID_BUG && magr->data->mlet == S_XAN) || (uarmf && !rn2(10) && itemhasappearance(uarmf, APP_CHESS_BOOTS) ) ) && magr->mx != mdef->mx && magr->my != mdef->my)
	return(MM_MISS);

    range = (!magr->mtame || rn2(3)) && !monnear(magr, mdef->mx, mdef->my);

    /* Calculate the armour class differential. */
    tmp = find_mac(mdef);

    boolean didrangedattack = FALSE;
    boolean canweaphit = TRUE;

    boolean norangepassive = FALSE; /* don't run passives against phantom attacks, please --Amy */

    boolean egotypearcane = magr->egotype_arcane;
    boolean egotypeclerical = magr->egotype_clerical;
    boolean egotypemastercaster = magr->egotype_mastercaster;
    boolean egotypethief = magr->egotype_thief;
    boolean egotypedisenchant = magr->egotype_disenchant;
    boolean egotyperust = magr->egotype_rust;
    boolean egotypecorrosion = magr->egotype_corrosion;
    boolean egotypedecay = magr->egotype_decay;
    boolean egotypeflamer = magr->egotype_flamer;
    boolean egotypeblasphemer = magr->egotype_blasphemer;
    boolean egotypeinverter = magr->egotype_inverter;
    boolean egotypedebtor = magr->egotype_debtor;
    boolean egotypedropper = magr->egotype_dropper;
    boolean egotypewither = magr->egotype_wither;
    boolean egotypegrab = magr->egotype_grab;
    boolean egotypefaker = magr->egotype_faker;
    boolean egotypeslows = magr->egotype_slows;
    boolean egotypevampire = magr->egotype_vampire;
    boolean egotypeteleportyou = magr->egotype_teleportyou;
    boolean egotypewrap = magr->egotype_wrap;
    boolean egotypedisease = magr->egotype_disease;
    boolean egotypeslime = magr->egotype_slime;
    boolean egotypepoisoner = magr->egotype_poisoner;
    boolean egotypeelementalist = magr->egotype_elementalist;
    boolean egotypeacidspiller = magr->egotype_acidspiller;
    boolean egotypeengrave = magr->egotype_engrave;
    boolean egotypedark = magr->egotype_dark;
    boolean egotypesounder = magr->egotype_sounder;
    boolean egotypetimer = magr->egotype_timer;
    boolean egotypethirster = magr->egotype_thirster;
    boolean egotypenexus = magr->egotype_nexus;
    boolean egotypegravitator = magr->egotype_gravitator;
    boolean egotypeinert = magr->egotype_inert;
    boolean egotypeantimage = magr->egotype_antimage;
    boolean egotypeunskillor = magr->egotype_unskillor;
    boolean egotypevenomizer = magr->egotype_venomizer;
    boolean egotypedreameater = magr->egotype_dreameater;
    boolean egotypenastinator = magr->egotype_nastinator;
    boolean egotypebaddie = magr->egotype_baddie;
    boolean egotypebadowner = magr->egotype_badowner;
    boolean egotypebleeder = magr->egotype_bleeder;
    boolean egotypesludgepuddle = magr->egotype_sludgepuddle;
    boolean egotypevulnerator = magr->egotype_vulnerator;
    boolean egotypemarysue = magr->egotype_marysue;
    boolean egotypeplasmon = magr->egotype_plasmon;
    boolean egotypelasher = magr->egotype_lasher;
    boolean egotypebreather = magr->egotype_breather;
    boolean egotypeluck = magr->egotype_luck;
    boolean egotypeamnesiac = magr->egotype_amnesiac;
    boolean egotypeshanker = magr->egotype_shanker;
    boolean egotypeterrorizer = magr->egotype_terrorizer;
    boolean egotypeseducer = magr->egotype_seducer;
    boolean egotypecullen = magr->egotype_cullen;
    boolean egotypewebber = magr->egotype_webber;
    boolean egotypetrapmaster = magr->egotype_trapmaster;
    boolean egotypeitemporter = magr->egotype_itemporter;
    boolean egotypesinner = magr->egotype_sinner;
    boolean egotypeschizo = magr->egotype_schizo;
    boolean egotypealigner = magr->egotype_aligner;
    boolean egotypefeminizer = magr->egotype_feminizer;
    boolean egotypedestructor = magr->egotype_destructor;
    boolean egotypedatadeleter = magr->egotype_datadeleter;
    boolean egotypelevitator = magr->egotype_levitator;
    boolean egotypeillusionator = magr->egotype_illusionator;
    boolean egotypestealer = magr->egotype_stealer;
    boolean egotypestoner = magr->egotype_stoner;
    boolean egotypetrembler = magr->egotype_trembler;
    boolean egotypeworldender = magr->egotype_worldender;
    boolean egotypedamager = magr->egotype_damager;
    boolean egotypeantitype = magr->egotype_antitype;
    boolean egotypestatdamager = magr->egotype_statdamager;
    boolean egotypedamagedisher = magr->egotype_damagedisher;
    boolean egotypemaecke = magr->egotype_maecke;
    boolean egotypesanitizer = magr->egotype_sanitizer;
    boolean egotypenastycurser = magr->egotype_nastycurser;
    boolean egotypethiefguildmember = magr->egotype_thiefguildmember;
    boolean egotyperogue = magr->egotype_rogue;
    boolean egotypepainlord = magr->egotype_painlord;
    boolean egotypeempmaster = magr->egotype_empmaster;
    boolean egotypespellsucker = magr->egotype_spellsucker;
    boolean egotypeeviltrainer = magr->egotype_eviltrainer;
    boolean egotypecontaminator = magr->egotype_contaminator;
    boolean egotypereactor = magr->egotype_reactor;
    boolean egotyperadiator = magr->egotype_radiator;
    boolean egotypeminator = magr->egotype_minator;
    boolean egotypeaggravator = magr->egotype_aggravator;
    boolean egotypemidiplayer = magr->egotype_midiplayer;
    boolean egotyperngabuser = magr->egotype_rngabuser;
    boolean egotypewatersplasher = magr->egotype_watersplasher;
    boolean egotypecancellator = magr->egotype_cancellator;
    boolean egotypebanisher = magr->egotype_banisher;
    boolean egotypeshredder = magr->egotype_shredder;
    boolean egotypeabductor = magr->egotype_abductor;
    boolean egotypeincrementor = magr->egotype_incrementor;
    boolean egotypemirrorimage = magr->egotype_mirrorimage;
    boolean egotypecurser = magr->egotype_curser;
    boolean egotypehorner = magr->egotype_horner;
    boolean egotypepush = magr->egotype_push;
    boolean egotyperandomizer = magr->egotype_randomizer;
    boolean egotypeblaster = magr->egotype_blaster;
    boolean egotypepsychic = magr->egotype_psychic;
    boolean egotypeabomination = magr->egotype_abomination;
    boolean egotypeweeper = magr->egotype_weeper;
    boolean egotypeweaponizer = magr->egotype_weaponizer;
    boolean egotypeengulfer = magr->egotype_engulfer;
    boolean egotypecameraclicker = magr->egotype_cameraclicker;
    boolean egotypealladrainer = magr->egotype_alladrainer;

    /* To-hit based on the monster's level. Nerf by Amy: high-level monsters shouldn't autohit. */
    magrlev = magr->m_lev;
    if (magrlev > 19) {
	magrhih = magrlev - 19;
	magrlev -= rnd(magrhih);
    if (magrlev > 29) {
	magrhih = magrlev - 19;
		if (magrhih > 1) magrhih /= 2;
		magrlev -= magrhih;
	}
    }
	/* pets need to be subjected to penalties or they'll be overpowered :P --Amy */
    if (magr->mtame) {
	if (magr->mflee) tmp -= 20;
	if (magr->mstun) tmp -= rnd(20);
	if (magr->mconf) tmp -= rnd(5);
	if (magr->mblinded && haseyes(magr->data)) tmp -= rnd(8);
	if (mdef->minvis && haseyes(magr->data) && !perceives(magr->data)) tmp -= 10;
	if (mdef->minvisreal) tmp -= (haseyes(magr->data) ? rnd(30) : rnd(20));

	if ( (mdef->data == &mons[PM_DISPLACER_BEAST] || mdef->data == &mons[PM_HYPER_CAVE_BEAR] || mdef->data == &mons[PM_KURIKKUSHITEKUDASAI__ANATA_NO_OKANE_WANAKU_NATTE_IMASU] || mdef->data == &mons[PM_REPLACER_BEAST] || mdef->data == &mons[PM_MISPLACER_BEAST] || mdef->data == &mons[PM_ELOCATOR] || mdef->data == &mons[PM_ANCIENT_OF_THE_BURNING_WASTES] || mdef->data == &mons[PM_SARTAN_TANNIN] || mdef->data == &mons[PM_POLYMORPH_CODE] || mdef->data == &mons[PM_SECRET_POLYMORPH_CODE] || mdef->data == &mons[PM_FIRST_WRAITHWORM] || mdef->data == &mons[PM_WRAITHWORM] || mdef->data == &mons[PM_LILAC_FEMMY] || mdef->data == &mons[PM_SHARAB_KAMEREL] || mdef->data == &mons[PM_WUXTINA] || mdef->data == &mons[PM_IVEL_WUXTINA] || mdef->data == &mons[PM_FLUTTERBUG] || mdef->data == &mons[PM_LIPSTICK_HC_GIRL] || mdef->data == &mons[PM_CREEPY_EVIL_GHOST] || mdef->data == &mons[PM_ORTHOS] || mdef->data == &mons[PM_SHIMMERING_DRACONIAN] || mdef->data == &mons[PM_JUMPING_CHAMPION] || mdef->data->mlet == S_GRUE || mdef->data == &mons[PM_QUANTUM_MOLD] || mdef->data == &mons[PM_QUANTUM_GROWTH] || mdef->data == &mons[PM_QUANTUM_FUNGUS] || mdef->data == &mons[PM_QUANTUM_PATCH] || mdef->data == &mons[PM_QUANTUM_STALK] || mdef->data == &mons[PM_QUANTUM_MUSHROOM] || mdef->data == &mons[PM_QUANTUM_SPORE] || mdef->data == &mons[PM_QUANTUM_COLONY] || mdef->data == &mons[PM_QUANTUM_FORCE_FUNGUS] || mdef->data == &mons[PM_QUANTUM_WORT] || mdef->data == &mons[PM_QUANTUM_FORCE_PATCH] || mdef->data == &mons[PM_QUANTUM_WARP_FUNGUS] || mdef->data == &mons[PM_QUANTUM_WARP_PATCH] || mdef->egotype_displacer) && !rn2(2)) tmp -= 100;

	if (mdef->data == &mons[PM_XXXXXXXXXXXXXXXXXXXX]) tmp -= 100;

    } /* attacking monster is tame */

	/* monster attacks should be fully effective against pets so you can't just cheese out everything --Amy
	 * (basically, symmetrical with uhitm.c but only if the attack targets your pet; this isn't FIQslex) */
    if (mdef->mtame) {
	if (!rn2(5) && (level_difficulty() > 20) && magrlev < 5) magrlev = 5;
	if (!rn2(5) && (level_difficulty() > 40) && magrlev < 10) magrlev = 10;
	if (!rn2(5) && (level_difficulty() > 60) && magrlev < 15) magrlev = 15;
	if (!rn2(5) && (level_difficulty() > 80) && magrlev < 20) magrlev = 20;

	if (level_difficulty() > 5 && magrlev < 5 && !rn2(5)) magrlev++;
	if (level_difficulty() > 10 && magrlev < 5 && !rn2(2)) magrlev++;
	if (level_difficulty() > 10 && magrlev < 10 && !rn2(5)) magrlev++;
	if (level_difficulty() > 20 && magrlev < 10 && !rn2(3)) magrlev++;
	if (level_difficulty() > 20 && magrlev < 15 && !rn2(5)) magrlev++;
	if (level_difficulty() > 40 && magrlev < 15 && !rn2(4)) magrlev++;
	if (level_difficulty() > 30 && magrlev < 20 && !rn2(5)) magrlev++;
	if (level_difficulty() > 60 && magrlev < 20 && !rn2(5)) magrlev++;

	if (magr->egotype_hitter) tmp += 10;
	if (magr->egotype_piercer) tmp += 25;
	if (!(mdef->mcanmove)) tmp += 4;
	if (mdef->mtrapped) tmp += 2;

	if (magr->data == &mons[PM_IVORY_COAST_STAR]) tmp += 30; /* this monster is aiming abnormally well */
	if (magr->data == &mons[PM_HAND_OF_GOD]) tmp += 100; /* God personally is guiding this one's blows */
	if (magr->data == &mons[PM_AKIRA_GIRL]) tmp += 20; /* she needs some boost */
	if (magr->data == &mons[PM_FIRST_DUNVEGAN]) tmp += 100; /* this monster also almost always hits */
	if (magr->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) tmp += rnd(100); /* the elder priest uses an aimbot and a wallhack */

	if (monstersoundtype(magr) == MS_FART_LOUD && !magr->butthurt) tmp += rnd(5);
	if (monstersoundtype(magr) == MS_FART_NORMAL && !magr->butthurt) tmp += rnd(7);
	if (monstersoundtype(magr) == MS_FART_QUIET && !magr->butthurt) tmp += rnd(10);
	if (monstersoundtype(magr) == MS_WHORE && !magr->butthurt) tmp += rnd(15);
	if (monstersoundtype(magr) == MS_SHOE) tmp += rnd(10);
	if (monstersoundtype(magr) == MS_STENCH) tmp += rnd(15);
	if (monstersoundtype(magr) == MS_CONVERT) tmp += rnd(5);
	if (monstersoundtype(magr) == MS_HCALIEN) tmp += rnd(10);
	if (magr->egotype_farter) tmp += rnd(7);
	if (magr->fartbonus) tmp += rnd(magr->fartbonus);
	if (magr->crapbonus) tmp += rno(magr->crapbonus);
	if (is_table(magr->mx, magr->my)) tmp += 3;
	if (humanoid(magr->data) && is_female(magr->data) && attacktype(magr->data, AT_KICK) && FemtrapActiveMadeleine) tmp += rnd(100);
	if (humanoid(magr->data) && is_female(magr->data) && FemtrapActiveWendy) tmp += rnd(SuperFemtrapWendy ? 20 : 10);

	if (!rn2(20)) tmp += 20; /* "natural 20" like in D&D --Amy */

	if(!magr->cham && (is_demon(magr->data) || magr->egotype_gator) && monnear(magr, mdef->mx, mdef->my) && magr->data != &mons[PM_BALROG]
	   && magr->data != &mons[PM_SUCCUBUS]
	   && magr->data != &mons[PM_INCUBUS]
 	   && magr->data != &mons[PM_NEWS_DAEMON]
 	   && magr->data != &mons[PM_PRINTER_DAEMON]) {
		if(!magr->mcan && !rn2(magr->data == &mons[PM_PERCENTI_OPENS_A_GATE_] ? 5 : magr->data == &mons[PM_CHEATER_LION] ? 5 : magr->data == &mons[PM_TEA_HUSSY] ? 5 : magr->data == &mons[PM_PERCENTI_PASSES_TO_YOU_] ? 5 : 23)) {
			msummon(magr, TRUE);
			pline("%s opens a gate!", Monnam(magr) );
			if (PlayerHearsSoundEffects) pline(issoviet ? "Sovetskaya nadeyetsya, chto demony zapolnyayut ves' uroven' i ubit' vas." : "Pitschaeff!");
		}
	 }

	if(!magr->cham && is_were(magr->data) && monnear(magr, mdef->mx, mdef->my)) {

	    if(!rn2(10) && !magr->mcan) {
	    	int numseen, numhelp;
		char buf[BUFSZ], genericwere[BUFSZ];

		strcpy(genericwere, "creature");
		numhelp = were_summon(magr->data, FALSE, &numseen, genericwere, TRUE);
		pline("%s summons help!", Monnam(magr));
		if (numhelp > 0) {
		    if (numseen == 0)
			You_feel("hemmed in.");
		} else pline("But none comes.");
	    }
	}

    } /* defending monster is tame */

    tmp += magrlev;

    if (mdef->mconf || !mdef->mcanmove || mdef->msleeping) {
	tmp += 4;
	mdef->msleeping = 0;
    }

	if (mdef->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) {
		mdef->isegotype = 1;
		mdef->egotype_covetous = 1;
	}

    /* undetect monsters become un-hidden if they are attacked */
    if (mdef->mundetected) {
	mdef->mundetected = 0;
	newsym(mdef->mx, mdef->my);
	if(canseemon(mdef) && !sensemon(mdef)) {
	    if (u.usleep) You("dream of %s.",
				(mdef->data->geno & G_UNIQ) ?
				a_monnam(mdef) : makeplural(m_monnam(mdef)));
	    else pline("Suddenly, you notice %s.", a_monnam(mdef));
	}
    }

    /* Elves hate orcs. */
    if (is_elf(pa) && is_orc(pd)) tmp++;


    /* Set up the visibility of action */
    vis = (cansee(magr->mx,magr->my) && cansee(mdef->mx,mdef->my) && (canspotmon(magr) || canspotmon(mdef)));

    /*	Set flag indicating monster has moved this turn.  Necessary since a
     *	monster might get an attack out of sequence (i.e. before its move) in
     *	some cases, in which case this still counts as its move for the round
     *	and it shouldn't move again.
     */
    magr->mlstmv = monstermoves;

    /* Now perform all attacks for the monster. */
    for (i = 0; i < NATTK; i++) {
	res[i] = MM_MISS;
	mattk = getmattk(pa, i, res, &alt_attk);
	otmp = (struct obj *)0;
	attk = 1;
	if (DEADMONSTER(mdef) || DEADMONSTER(magr)) { /* ANOTHER catchall by Amy because it keeps malfunctioning! */
		break;
	}

	switch (mattk->aatyp) {
	    case AT_BREA:
	    case AT_SPIT:

		if (range && !linedup(magr->mx,magr->my,mdef->mx,mdef->my, FALSE) ) {
		    strike = 0;
		    attk = 0;
		    norangepassive = TRUE;
		    break;

		}

		if (range || (mattk->aatyp == AT_SPIT && mdef->mtame) || (mdef->mtame && !mon_reflects(mdef, (char *)0) ) ) {

			if (!rn2(3)) {
				goto meleeattack;
			}
		    if (mattk->aatyp == AT_BREA) {

			if (mattk->adtyp == AD_FIRE && resists_fire(mdef) && !player_will_pierce_resistance()) {
				strike = 0;
			} else if (mattk->adtyp == AD_COLD && (resists_cold(mdef) && !player_will_pierce_resistance()) ) {
				strike = 0;
			} else if (mattk->adtyp == AD_SLEE && resists_sleep(mdef)) {
				strike = 0;
			} else if (mattk->adtyp == AD_DISN && resists_disint(mdef)) {
				strike = 0;
			} else if (mattk->adtyp == AD_ELEC && (resists_elec(mdef) && !player_will_pierce_resistance()) ) {
				strike = 0;
			} else if (mattk->adtyp == AD_DRST && (resists_poison(mdef) && !player_will_pierce_resistance()) ) {
				strike = 0;
			} else if (mattk->adtyp == AD_ACID && (resists_acid(mdef) && !player_will_pierce_resistance()) ) {
				strike = 0;
			} else if ((mattk->adtyp < AD_MAGM || mattk->adtyp > AD_SPC2) && mattk->adtyp != AD_RBRE) {
				goto meleeattack;
			}
			else res[i] = breamm(magr, mdef, mattk);
		    } else {
			if (mattk->adtyp == AD_ACID || mattk->adtyp == AD_BLND || mattk->adtyp == AD_TCKL || mattk->adtyp == AD_DRLI || mattk->adtyp == AD_NAST || mattk->adtyp == AD_ICEB) {
				res[i] = spitmm(magr, mdef, mattk);
			} else goto meleeattack;

		    }
		    /* We can't distinguish no action from failed attack
		     * so assume defender doesn't waken unless actually hit.
		     */
		    strike = res[i] & MM_HIT;
		} else
		    strike = 0;
		attk = 0;
		break;

	    case AT_MAGC:

		if (range && !linedup(magr->mx,magr->my,mdef->mx,mdef->my, FALSE) ) {
		    strike = 0;
		    attk = 0;
		    norangepassive = TRUE;

		    break;

		}

		/* [ALI] Monster-on-monster spell casting always fails. This
		 * is partly for balance reasons and partly because the
		 * amount of code required to implement it is prohibitive.
		 */
		/*strike = 0;
		attk = 0;*/
		if (canseemon(magr) && couldsee(magr->mx, magr->my)) {
		    char buf[BUFSZ];
		    strcpy(buf, Monnam(magr));
		    if (vis)
			pline("%s points at %s, then curses.", buf,
				mon_nam(mdef));
		    else
			pline("%s points and curses at something.", buf);
		} else if (flags.soundok)
		    Norep("You hear a mumbled curse.");

		goto meleeattack;
		break;

	    case AT_WEAP:
		/* "ranged" attacks */
		if ((range || (!rn2(4) && mdef->mtame) ) && !didrangedattack) {

		    res[i] = thrwmm(magr, mdef);
		    attk = 0;
		    strike = res[i] & MM_HIT;
		    didrangedattack = TRUE;
		    break;
		}
		/* "hand to hand" attacks */
		if (magr->weapon_check == NEED_WEAPON || !MON_WEP(magr)) {
		    magr->weapon_check = NEED_HTH_WEAPON;
		    if (mon_wield_item(magr) != 0) {
			return 0;
		    }
		}
		possibly_unwield(magr, FALSE);
		otmp = MON_WEP(magr);

		if (otmp) {
		    if (canweaphit) {
			    if (vis) mswingsm(magr, mdef, otmp);
			    tmp += hitval(otmp, mdef);
		    }
		    if (bimanual(otmp)) canweaphit = !canweaphit;
		}
		/* fall through */
	    case AT_CLAW:
	    case AT_KICK:
	    case AT_BITE:
	    case AT_STNG:
	    case AT_TUCH:
	    case AT_BUTT:
	    case AT_LASH:
	    case AT_TRAM:
	    case AT_SCRA:
	    case AT_TENT:
	    case AT_BEAM:

		if ((mattk->aatyp != AT_BEAM) && !(mattk->aatyp == AT_WEAP && range) && (!monnear(magr, mdef->mx, mdef->my)) ) norangepassive = TRUE;

meleeattack:
		/* Nymph that teleported away on first attack? */
		if ((distmin(magr->mx,magr->my,mdef->mx,mdef->my) > 1) && mattk->aatyp != AT_BREA && mattk->aatyp != AT_SPIT && mattk->aatyp != AT_MAGC ) {
		    strike = 0;
		    break;
		}

		/* is the monster lined up? if not, the attack always fails */
		if ((distmin(magr->mx,magr->my,mdef->mx,mdef->my) > 1) && !linedup(magr->mx,magr->my,mdef->mx,mdef->my, FALSE)) {
		    strike = 0;
		    break;

		}

		/* Monsters won't attack cockatrices physically if they
		 * have a weapon instead.  This instinct doesn't work for
		 * players, or under conflict or confusion.
		 */
		if (!magr->mconf && !Conflict && otmp &&
		    mattk->aatyp != AT_WEAP && touch_petrifies(mdef->data)) {
		    strike = 0;
		    break;
		}
		dieroll = rnd(20 + i);
		if (!rn2(3) && magr->m_lev > 0) {
			magrlev = magr->m_lev;
			if (magrlev > 19) {
				magrhih = magrlev - 19;
				magrlev -= rnd(magrhih);

				if (magrlev > 29) {
					magrhih = magrlev - 19;
					if (magrhih > 1) magrhih /= 2;
					magrlev -= magrhih;
				}
			}
			tmp += rno(magrlev);
		}
		if (magr->data == &mons[PM_STOOGE_MOE] || magr->data == &mons[PM_STOOGE_CURLY] || magr->data == &mons[PM_STOOGE_LARRY]) tmp += 50;
		strike = (tmp > dieroll);
		if (strike) {
		    res[i] = hitmm(magr, mdef, mattk);
		    if((mdef->data == &mons[PM_BLACK_PUDDING] || mdef->data == &mons[PM_GEMINICROTTA] || mdef->data == &mons[PM_DIVISIBLE_ICE_CREAM_BAR] || mdef->data == &mons[PM_GREY_PUDDING] || mdef->data == &mons[PM_STICKY_PUDDING] || mdef->data == &mons[PM_SHOCK_PUDDING] || mdef->data == &mons[PM_VOLT_PUDDING] || mdef->data == &mons[PM_DRUDDING] || mdef->data == &mons[PM_BLACK_DRUDDING] || mdef->data == &mons[PM_BLACKSTEEL_PUDDING] || mdef->data == &mons[PM_BLOOD_PUDDING] || mdef->data == &mons[PM_MORAL_HAZARD] || mdef->data == &mons[PM_MORAL_EVENT_HORIZON] || mdef->data == &mons[PM_BLACK_PIERCER] || mdef->data == &mons[PM_BROWN_PUDDING])
		       && otmp && objects[otmp->otyp].oc_material == MT_IRON
		       && mdef->mhp > 1 && !mdef->mcan && !rn2(100) ) /* slowing pudding farming to a crawl --Amy */
		    {
			if (clone_mon(mdef, 0, 0)) {
			    if (vis) {
				char buf[BUFSZ];

				strcpy(buf, Monnam(mdef));
				pline("%s divides as %s hits it!", buf, mon_nam(magr));
			    }
			}
		    }
		} else
		    missmm(magr, mdef, tmp, dieroll, mattk);
		/* KMH -- don't accumulate to-hit bonuses */
		if (otmp)
		    tmp -= hitval(otmp, mdef);
		break;

	    case AT_HUGS:	/* automatic if prev two attacks succeed, but also with a low chance otherwise --Amy */
		strike = (((i >= 2 && res[i-1] == MM_HIT && res[i-2] == MM_HIT) || (!rn2(mdef->mtame ? 10 : 30) && (dist2(mdef->mx, mdef->my, magr->mx, magr->my) <= (BOLT_LIM * BOLT_LIM)) ) ) && (tmp > dieroll));
		if (strike)
		    res[i] = hitmm(magr, mdef, mattk);
		else norangepassive = TRUE;

		break;

	    case AT_GAZE:
		strike = 0;	/* will not wake up a sleeper */

		/* endless gaze spammage against pets was really getting annoying... --Amy
		 * In Soviet Russia, the enemy's troops can stare down at your troops from endlessly far away, and do so
		 * constantly, meaning that your only hope is to equip them all with AK-47's and pray they manage to line
		 * themselves up for a shot. Otherwise, your guys just get wrecked with no chance. */

		if (!range || (clear_path(magr->mx, magr->my, mdef->mx, mdef->my) && (!rn2(10) || issoviet) ) ) {

			res[i] = gazemm(magr, mdef, mattk);
		} else norangepassive = TRUE;

		break;

	    case AT_EXPL:
		if (!magr->mtame && rn2(20)) break; /* we want the things to explode at YOU! Since monsters are immune to quite some attack types anyway, and the exploding lights would just suicide without causing any effect. --Amy */

		if (!monnear(magr, mdef->mx, mdef->my)) break; /* we don't want them to suddenly warp and explode in someone's face --Amy */

		res[i] = explmm(magr, mdef, mattk);
		if (res[i] == MM_MISS) { /* cancelled--no attack */
		    strike = 0;
		    attk = 0;
		} else
		    strike = 1;	/* automatic hit */
		break;

	    case AT_ENGL:
		if (u.usteed && (mdef == u.usteed)) {
		    strike = 0;
		    break;
		}

		if (!monnear(magr, mdef->mx, mdef->my)) break; /* we don't want them to suddenly warp and engulf someone from far away --Amy */

		/* Engulfing attacks are directed at the hero if
		 * possible. -dlc
		 */
		if (u.uswallow && magr == u.ustuck)
		    strike = 0;
		else {
		    if (!rn2(3) && magr->m_lev > 0) tmp += rno(magr->m_lev);

		    if ((strike = (tmp > (dieroll = rnd(20+i)))))
			res[i] = gulpmm(magr, mdef, mattk);
		    else
			missmm(magr, mdef, tmp, dieroll, mattk);
		}
		break;

	    default:		/* no attack */
		strike = 0;
		attk = 0;
		break;
	}

	boolean hashit = FALSE;

	if (!norangepassive && attk && !(res[i] & MM_AGR_DIED)) {

	    res[i] = passivemm(magr, mdef, strike, res[i] & MM_DEF_DIED, 0);
	    if (res[i] & MM_HIT) hashit = TRUE;
	    if (hashit && !(res[i] & MM_HIT)) res[i] |= MM_HIT;
	    if (res[i] & MM_DEF_DIED) return res[i];

	    if (!(res[i] & MM_AGR_DIED) && !(res[i] & MM_DEF_DIED)) {
		    res[i] = passivemm(magr, mdef, strike, res[i] & MM_DEF_DIED, 1);
		    if (res[i] & MM_HIT) hashit = TRUE;
		    if (hashit && !(res[i] & MM_HIT)) res[i] |= MM_HIT;
		    if (res[i] & MM_DEF_DIED) return res[i];
	    }
	    if (!(res[i] & MM_AGR_DIED) && !(res[i] & MM_DEF_DIED)) {
		    res[i] = passivemm(magr, mdef, strike, res[i] & MM_DEF_DIED, 2);
		    if (res[i] & MM_HIT) hashit = TRUE;
		    if (hashit && !(res[i] & MM_HIT)) res[i] |= MM_HIT;
		    if (res[i] & MM_DEF_DIED) return res[i];
	    }
	    if (!(res[i] & MM_AGR_DIED) && !(res[i] & MM_DEF_DIED)) {
		    res[i] = passivemm(magr, mdef, strike, res[i] & MM_DEF_DIED, 3);
		    if (res[i] & MM_HIT) hashit = TRUE;
		    if (hashit && !(res[i] & MM_HIT)) res[i] |= MM_HIT;
		    if (res[i] & MM_DEF_DIED) return res[i];
	    }
	    if (!(res[i] & MM_AGR_DIED) && !(res[i] & MM_DEF_DIED)) {
		    res[i] = passivemm(magr, mdef, strike, res[i] & MM_DEF_DIED, 4);
		    if (res[i] & MM_HIT) hashit = TRUE;
		    if (hashit && !(res[i] & MM_HIT)) res[i] |= MM_HIT;
		    if (res[i] & MM_DEF_DIED) return res[i];
	    }
	    if (!(res[i] & MM_AGR_DIED) && !(res[i] & MM_DEF_DIED)) {
		    res[i] = passivemm(magr, mdef, strike, res[i] & MM_DEF_DIED, 5);
		    if (res[i] & MM_HIT) hashit = TRUE;
		    if (hashit && !(res[i] & MM_HIT)) res[i] |= MM_HIT;
		    if (res[i] & MM_DEF_DIED) return res[i];
	    }

	}

	if (hashit && !(res[i] & MM_HIT)) res[i] |= MM_HIT;

	if (res[i] & MM_DEF_DIED) return res[i];

	/*
	 *  Wake up the defender.  NOTE:  this must follow the check
	 *  to see if the defender died.  We don't want to modify
	 *  unallocated monsters!
	 */
	if (strike) {
		mdef->msleeping = 0;

		if (mdef->masleep && !rn2(3)) {
			mdef->mcanmove = 1;
			mdef->mfrozen = 0;
			mdef->masleep = 0;
		}
	}

	if (res[i] & MM_AGR_DIED)  return res[i];
	/* return if aggressor can no longer attack */
	if (!magr->mcanmove || magr->msleeping) return res[i];
	if (res[i] & MM_HIT) struck = 1;	/* at least one hit */
    }

    /* egotypes and other extra attacks, by Amy */
    if (mdef->mtame) {

	if (egotypearcane ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SPEL;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeclerical ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CLRC;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypemastercaster ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CAST;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypethief ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SITM;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedisenchant ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ENCH;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotyperust ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RUST;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypecorrosion ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CORR;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedecay ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DCAY;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeflamer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FLAM;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeblasphemer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BLAS;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeinverter ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_INVE;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedebtor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DEBT;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedropper ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DROP;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypewither ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_WTHR;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypegrab ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STCK;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypefaker ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FAKE;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeslows ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SLOW;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypevampire ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DRLI;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeteleportyou ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TLPT;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypewrap ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_WRAP;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedisease ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DISE;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeslime ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SLIM;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypepoisoner ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_POIS;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeelementalist ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_AXUS;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeacidspiller ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ACID;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeengrave ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NGRA;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedark ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DARK;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypesounder ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SOUN;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypetimer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TIME;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypethirster ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_THIR;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypenexus ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NEXU;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypegravitator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_GRAV;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeinert ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_INER;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeantimage ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MANA;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeunskillor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SKIL;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypevenomizer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_VENO;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedreameater ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DREA;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypenastinator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NAST;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypebaddie ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BADE;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypebadowner ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RBAD;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypebleeder ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BLEE;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypesludgepuddle ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SLUD;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypevulnerator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_VULN;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypemarysue ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FUMB;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeplasmon ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PLAS;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypelasher ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_LASH;
		a->adtyp = AD_MALK;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypebreather ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RBRE;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeluck ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_LUCK;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeamnesiac ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_AMNE;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeshanker ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SHAN;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeterrorizer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TERR;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeseducer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SSEX;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypecullen ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_VAMP;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypewebber ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_WEBS;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypetrapmaster ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TRAP;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeitemporter ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STTP;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypesinner  ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SIN;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeschizo ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DEPR;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypealigner) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ALIN;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypefeminizer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FEMI;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedestructor) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DEST;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedatadeleter) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_LASH;
		a->adtyp = AD_DATA;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypelevitator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_LEVI;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeillusionator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ILLU;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypestealer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SAMU;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypestoner) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STON;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypetrembler) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TREM;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeworldender) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RAGN;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedamager) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_IDAM;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeantitype) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ANTI;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypestatdamager) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STAT;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypedamagedisher) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DAMA;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypemaecke) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MCRE;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypesanitizer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SANI;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypenastycurser) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NACU;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypethiefguildmember) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_THIE;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotyperogue) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SEDU;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypepainlord) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PAIN;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeempmaster) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TECH;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypespellsucker) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MEMO;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeeviltrainer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TRAI;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypecontaminator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CONT;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypereactor) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CONT;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotyperadiator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CONT;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeminator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MINA;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeaggravator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_AGGR;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypemidiplayer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MIDI;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotyperngabuser ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RNG;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypewatersplasher  ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = level.flags.lethe ? AD_LETH : AD_WET;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypecancellator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CNCL;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypebanisher ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BANI;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeshredder ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SHRD;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeabductor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ABDC;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeincrementor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CHKH;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypemirrorimage ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_HODS;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypecurser ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CURS;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypehorner ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = rn2(1000) ? AD_CHRN : AD_UVUU;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypepush ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DISP;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotyperandomizer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RBRE;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeblaster) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TENT;
		a->adtyp = AD_DRIN;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypepsychic) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SPC2;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeabomination) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SPC2;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeweeper) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CONT;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeweaponizer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_WEAP;
		a->adtyp = AD_PHYS;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (egotypeengulfer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PHYS;
		a->damn = 2;
		a->damd = (1 + (magr->m_lev));

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (evilfriday && magr->data->mlet == S_GIANT) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_HUGS;
		a->adtyp = AD_PHYS;
		a->damn = 2;
		a->damd = 1 + (magr->m_lev / 2);

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (evilfriday && magr->data->mlet == S_ZOMBIE) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DISE;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (evilfriday && magr->data->mlet == S_MUMMY) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = rn2(20) ? AD_ICUR : AD_NACU;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (evilfriday && (is_mplayer(magr->data) || is_umplayer(magr->data))) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SAMU;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (evilfriday && magr->data->mlet == S_GHOST) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_INER;
		a->damn = 1;
		a->damd = 1;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (uimplant && uimplant->oartifact == ART_POTATOROK && !rn2(10)) {

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) {
				ragnarok(FALSE);
				if (evilfriday && magr->m_lev > 1) evilragnarok(FALSE, magr->m_lev);
			}
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON && !rn2(10)) {

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) {
				ragnarok(FALSE);
				if (evilfriday && magr->m_lev > 1) evilragnarok(FALSE, magr->m_lev);
			}
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON && !rn2(10)) {

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) {
				ragnarok(FALSE);
				if (evilfriday && magr->m_lev > 1) evilragnarok(FALSE, magr->m_lev);
			}
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (magr->data == &mons[PM_BOFH] && isevilvariant) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PHYS;
		a->damn = 200;
		a->damd = 200;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (magr->data == &mons[PM_ALPHANUMEROGORGON] && isevilvariant) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PHYS;
		a->damn = 200;
		a->damd = 200;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (magr->data == &mons[PM_YOUR_GAME_ENDS_NOW] && isevilvariant) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PHYS;
		a->damn = 200;
		a->damd = 200;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (magr->data == &mons[PM_MR__CONCLUSIO] && isevilvariant) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PHYS;
		a->damn = 200;
		a->damd = 200;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (magr->data == &mons[PM_LEFTOGORGON] && isevilvariant) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PHYS;
		a->damn = 200;
		a->damd = 200;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if ((magr->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_] || (magr->data == &mons[PM_GIANT_ENEMY_CRAB] && !rn2(20)) || (magr->data == &mons[PM_E_PALE_WRAITH_WITH_A_LIGHTNING_STROKE_] && !rn2(20)) || magr->data == &mons[PM_SANDRA_S_MINDDRILL_SANDAL]) && isevilvariant) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PHYS;
		a->damn = 200;
		a->damd = 200;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) {
				res[i] = hitmm(magr, mdef, a);
				ragnarok(FALSE);
				if (evilfriday && magr->m_lev > 1) evilragnarok(FALSE, magr->m_lev);
			}
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if (monstersoundtype(magr) == MS_SHRIEK && monnear(magr, mdef->mx, mdef->my)) {
		if (!YouAreDeaf) pline("%s shrieks.", Monnam(magr));
		aggravate();
	}

	if ((monstersoundtype(magr) == MS_PHOTO || egotypecameraclicker) && !rn2(50)) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BLND;
		a->damn = 1;
		a->damd = 100;

		if(monnear(magr, mdef->mx, mdef->my)) {
			dieroll = rnd(20 + i);
			strike = (tmp > dieroll);
			if (strike) res[i] = hitmm(magr, mdef, a);
		}
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];

	}

	if ((monstersoundtype(magr) == MS_FART_QUIET || monstersoundtype(magr) == MS_FART_NORMAL || monstersoundtype(magr) == MS_FART_LOUD || magr->egotype_farter) && monnear(magr, mdef->mx, mdef->my)) {
		if (magr->fartbonus > 9) magr->fartbonus = 9; /* fail save */
		int monfartchance = 10 + magr->butthurt - magr->fartbonus;
		if (monfartchance < 1) monfartchance = 1; /* yet another fail safe */

		if (!rn2(monfartchance)) {
			if (vis) {
				if (monstersoundtype(magr) == MS_FART_QUIET) pline("%s produces %s farting noises with %s %s butt.", Monnam(magr), rn2(2) ? "tender" : "soft", mhis(magr), magr->female ? "sexy" : "ugly");
				else if (monstersoundtype(magr) == MS_FART_NORMAL) pline("%s produces %s farting noises with %s %s butt.", Monnam(magr), rn2(2) ? "beautiful" : "squeaky", mhis(magr), magr->female ? "sexy" : "ugly");
				else if (monstersoundtype(magr) == MS_FART_LOUD) pline("%s produces %s farting noises with %s %s butt.", Monnam(magr), rn2(2) ? "loud" : "disgusting", mhis(magr), magr->female ? "sexy" : "ugly");
				else pline("%s produces farting noises with %s %s butt.", Monnam(magr), mhis(magr), magr->female ? "sexy" : "ugly");
			}
			else if (!YouAreDeaf) pline("You hear farting noises.");
			badpeteffect(mdef);
		}

		if (magr->crapbonus && (rn2(100) < magr->crapbonus)) {

			/* STUPID bug where the game displays the same name twice for some inexplicable reason --Amy */
			strcpy(buf, mon_nam(mdef));

			if (vis) pline("%s craps in %s's face.", Monnam(magr), buf);
			else if (!YouAreDeaf) pline("You hear crapping noises.");
			int rnd_tmp;
			rnd_tmp = rnd(1 + (level_difficulty() * 3));
			if ((rnd_tmp += mdef->mblinded) > 127) rnd_tmp = 127;
			mdef->mblinded = rnd_tmp;
			mdef->mcansee = 0;

		}

	}

	if ((monstersoundtype(magr) == MS_SOUND || egotypesounder) && !rn2(20) && monnear(magr, mdef->mx, mdef->my)) {
		if (vis) pline("%s lets out an ear-splitting scream!", Monnam(magr) );
		else if (!YouAreDeaf) pline("You hear a scream.");
		mdef->mstun = TRUE;
		wake_nearby();
		if (!rn2(5)) badpeteffect(mdef);
	}

	if ((monstersoundtype(magr) == MS_ALLA || egotypealladrainer) && monnear(magr, mdef->mx, mdef->my)) {
		if (vis) pline("%s's alla is reduced!", Monnam(mdef));
		badpeteffect(mdef);
	}

	if ((monstersoundtype(magr) == MS_SOCKS) && mdef->mcanmove && !rn2(10) && monnear(magr, mdef->mx, mdef->my)) {

		/* STUPID bug where the game displays the same name twice for some inexplicable reason --Amy */
		strcpy(buf, mon_nam(mdef));

		if (vis) pline("%s's beguiling smell affects %s!", Monnam(magr), buf);
		mdef->mcanmove = 0;
		mdef->mfrozen = rn1(3,4);
		mdef->mstrategy &= ~STRAT_WAITFORU;
	}
	if ((monstersoundtype(magr) == MS_PANTS) && !rn2(10) && monnear(magr, mdef->mx, mdef->my)) {

		/* STUPID bug where the game displays the same name twice for some inexplicable reason --Amy */
		strcpy(buf, Monnam(mdef));

		if (vis) pline("%s catches a whiff from %s!", buf, mon_nam(magr) );
		badpeteffect(mdef);
	}

	if (monstersoundtype(magr) == MS_CUSS && !rn2(5) && monnear(magr, mdef->mx, mdef->my)) {

		/* STUPID bug where the game displays the same name twice for some inexplicable reason --Amy */
		strcpy(buf, mon_nam(mdef));

		if (magr->iswiz) {
			badpeteffect(mdef);
			mdef->healblock += (1 + magr->m_lev);
			if (vis) pline("%s calls %s nasty names.", Monnam(magr), buf );
		} else if (magr->data->mlet == S_ANGEL || magr->mnum == PM_CHRISTMAS_CHILD || magr->mnum == PM_HELLS_ANGEL || !rn2(5)) {
			mdef->healblock += (1 + magr->m_lev);
			if (vis) pline("%s is dimmed.", Monnam(mdef));
		}
	}

	if (monstersoundtype(magr) == MS_WHORE && !rn2(5) && monnear(magr, mdef->mx, mdef->my)) {
		mdef->healblock += (1 + magr->m_lev);
		if (!rn2(50)) badpeteffect(mdef);
		if (vis) pline("%s is dimmed.", Monnam(mdef));
	}

	if (monstersoundtype(magr) == MS_SUPERMAN && !rn2(5) && monnear(magr, mdef->mx, mdef->my)) {
		if (vis) pline("%s is terrorized by the superman!", Monnam(mdef));
		else You_feel("that something terrible is happening to your companion right now!");
		allbadpeteffects(mdef);
	}

	if ((monstersoundtype(magr) == MS_CONVERT || magr->egotype_converter) && !rn2(10) && monnear(magr, mdef->mx, mdef->my)) {
		mdef->healblock += (1 + magr->m_lev);
		if (vis) pline("%s seems less faithful.", Monnam(mdef));
		else You_hear("some foreign sermon.");
		if (!rn2(200)) {
			mdat2 = &mons[PM_CAST_DUMMY];
			a = &mdat2->mattk[3];
			a->aatyp = AT_TUCH;
			a->adtyp = AD_CALM;
			a->damn = 1;
			a->damd = 1;

			res[i] = hitmm(magr, mdef, a);
			if (res[i] & MM_AGR_DIED) return res[i];
			if (res[i] & MM_DEF_DIED) return res[i];

		}
	}

	if ((monstersoundtype(magr) == MS_HCALIEN || magr->egotype_wouwouer) && !rn2(15) && monnear(magr, mdef->mx, mdef->my)) {
		if (vis) pline("%s seems terrorified.", Monnam(mdef));
		else You_hear("a frightening taunt.");
		badpeteffect(mdef);
		badpeteffect(mdef);
		if (!rn2(50)) {
			mdat2 = &mons[PM_CAST_DUMMY];
			a = &mdat2->mattk[3];
			a->aatyp = AT_LASH;
			a->adtyp = AD_FREN;
			a->damn = 1;
			a->damd = 1;

			res[i] = hitmm(magr, mdef, a);
			if (res[i] & MM_AGR_DIED) return res[i];
			if (res[i] & MM_DEF_DIED) return res[i];

		}
	}

 	if (monstersoundtype(magr) == MS_SHOE && !rn2(50) && evilfriday && monnear(magr, mdef->mx, mdef->my)) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PLYS;
		a->damn = 1;
		a->damd = (1 + (magr->m_lev));

		res[i] = hitmm(magr, mdef, a);
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];
	}

 	if (monstersoundtype(magr) == MS_BONES && !rn2(100) && monnear(magr, mdef->mx, mdef->my)) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TRAM;
		a->adtyp = AD_PLYS;
		a->damn = 1;
		a->damd = (1 + (magr->m_lev));

		res[i] = hitmm(magr, mdef, a);
		if (res[i] & MM_AGR_DIED) return res[i];
		if (res[i] & MM_DEF_DIED) return res[i];
	}

	if ((monstersoundtype(magr) == MS_STENCH || magr->egotype_perfumespreader) && !rn2(20) && monnear(magr, mdef->mx, mdef->my)) {
		if (vis) pline("%s inhales the feminine perfume.", Monnam(mdef));
		badpeteffect(mdef);
	}

	if ((magr->data == &mons[PM_DHWTY] || magr->data == &mons[PM_MELUGAST_AO_I_DIMENSION_DRIVER] || magr->data == &mons[PM_AUTO_AIM_CHEATAH] || magr->data == &mons[PM_PARTICLE_MAN] || magr->data == &mons[PM_LAST_DANCER] || magr->data == &mons[PM_CURTAIN_CALL_LAST_DANCER] || magr->data == &mons[PM_GRAND_FINALE_LAST_DANCER] || magr->data == &mons[PM_PROVIDENCE_GAZE] || magr->data == &mons[PM_CHAOS_SERPENT] || magr->data == &mons[PM_CHAOTIC_SERPENT] || magr->data == &mons[PM_ARCHAYEEK_GUNNER] || magr->data == &mons[PM_SIN_GORILLA] || magr->data == &mons[PM_ELITE_GUARD] || magr->data == &mons[PM_ELITE_CROUPIER] || magr->data == &mons[PM_BLUEBEAM_GOLEM] || magr->data == &mons[PM_AIRTIGHT_FEMMY] || magr->data == &mons[PM_DRICERADOPS] || magr->data == &mons[PM_SVEN] || magr->data == &mons[PM_GRANDMASTER_SVEN] || magr->data == &mons[PM_WORLD_PWNZOR] || magr->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_] || magr->data == &mons[PM_SANDRA_S_MINDDRILL_SANDAL] || magr->egotype_laserpwnzor) && monnear(magr, mdef->mx, mdef->my)) {
		if (!magr->hominglazer && !rn2(20)) {
			pline("ATTENTION: Something has started to load an ultra-mega-hyper-dyper laser cannon!");
			magr->hominglazer = 1;
		} else if (magr->hominglazer >= 20) {
			pline("ZIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEtschhhhhhhhhhhhhhhhhhhhhhhh...");
			magr->hominglazer = 0;

			mdat2 = &mons[PM_CAST_DUMMY];
			a = &mdat2->mattk[3];
			a->aatyp = AT_BEAM;
			a->adtyp = AD_PHYS;
			a->damn = 200;
			a->damd = 200;

			res[i] = hitmm(magr, mdef, a);
			if (res[i] & MM_AGR_DIED) return res[i];
			if (res[i] & MM_DEF_DIED) return res[i];

		}
	}

   } /* special attacks targetting pets */

    return(struck ? MM_HIT : MM_MISS);
}

/* monster attempts breath attack against another monster */
STATIC_OVL int
breamm(magr, mdef, mattk)
struct monst *magr, *mdef;
struct attack *mattk;
{
    /* if new breath types are added, change AD_ACID to max type */
    int typ = mattk->adtyp == AD_RBRE ? rnd(AD_SPC2) : mattk->adtyp;
    int mhp;

    if (DEADMONSTER(magr)) return 0;
    if (DEADMONSTER(mdef)) return 0;

    if (linedup(mdef->mx, mdef->my, magr->mx, magr->my, FALSE)) {
	if (magr->mcan) {
	    if (flags.soundok) {
		if (canseemon(magr))
		    pline("%s coughs.", Monnam(magr));
		else
		    You_hear("a cough.");
	    }
	} else if (!magr->mspec_used && rn2(3)) {
	    if (typ >= AD_MAGM && typ <= AD_SPC2) {
		if (canseemon(magr))
		    pline("%s breathes %s!", Monnam(magr), breathwep[typ-1]);
		mhp = mdef->mhp;
		buzz((int)(-20 - (typ-1)), (rn2(2) ? (int)mattk->damn : (int)mattk->damd ),
			magr->mx, magr->my, sgn(tbx), sgn(tby));
		nomul(0, 0, FALSE);
		/* breath runs out sometimes. */
		if (!rn2(3))
		    magr->mspec_used = 10+rn2(20);
		return (mdef->mhp < 1 ? MM_DEF_DIED : 0) |
		       (mdef->mhp < mhp ? MM_HIT : 0) |
		       (magr->mhp < 1 ? MM_AGR_DIED : 0);
	    } else impossible("Breath weapon %d used", typ-1);
	}
    }
    return MM_MISS;
}

/* monster attempts spit attack against another monster */
STATIC_OVL int
spitmm(magr, mdef, mattk)
struct monst *magr, *mdef;
struct attack *mattk;
{
    register struct obj *obj;
    int mhp;

    if (DEADMONSTER(magr)) return 0;
    if (DEADMONSTER(mdef)) return 0;

    if (magr->mcan) {
	if (flags.soundok) {
	    if (canseemon(magr))
		pline("A dry rattle comes from %s throat.",
			s_suffix(mon_nam(magr)));
	    else
		You_hear("a dry rattle.");
	}
	return MM_MISS;
    }

    if (linedup(mdef->mx, mdef->my, magr->mx, magr->my, FALSE)) {
	switch (mattk->adtyp) {
	    case AD_BLND:
	    case AD_DRST:
		obj = mksobj(BLINDING_VENOM, TRUE, FALSE, FALSE);
		break;
	    case AD_DRLI:
		obj = mksobj(FAERIE_FLOSS_RHING, TRUE, FALSE, FALSE);
		break;
	    case AD_ICEB:
		obj = mksobj(ICE_BLOCK, TRUE, FALSE, FALSE);
		break;
	    case AD_TCKL:
		obj = mksobj(TAIL_SPIKES, TRUE, FALSE, FALSE);
		break;
	    case AD_NAST:
		obj = mksobj(SEGFAULT_VENOM, TRUE, FALSE, FALSE);
		break;
	    default:
		pline("bad attack type in spitmm");
	    /* fall through */
	    case AD_ACID:
		obj = mksobj(ACID_VENOM, TRUE, FALSE, FALSE);
		break;
	}
	if (!obj) return MM_MISS;
	if (!rn2(BOLT_LIM - distmin(magr->mx, magr->my, mdef->mx, mdef->my))) {
	    if (canseemon(magr))
		pline("%s spits venom!", Monnam(magr));
	    mhp = mdef->mhp;
	    m_throw(magr, magr->mx, magr->my, sgn(tbx), sgn(tby),
		    distmin(magr->mx, magr->my, mdef->mx, mdef->my), obj);
	    nomul(0, 0, FALSE);
	    return (mdef->mhp < 1 ? MM_DEF_DIED : 0) |
		   (mdef->mhp < mhp ? MM_HIT : 0) |
		   (magr->mhp < 1 ? MM_AGR_DIED : 0);
	}
    }
    return MM_MISS;
}

/* monster attempts ranged weapon attack against another monster */
STATIC_OVL int
thrwmm(magr, mdef)
struct monst *magr, *mdef;
{
    struct obj *obj, *mwep;
    schar skill;
    int multishot, mhp;
    const char *onm;

    if (DEADMONSTER(magr)) return 0;
    if (DEADMONSTER(mdef)) return 0;

	int polelimit = POLE_LIM;

	if (magr->data == &mons[PM_MECHTNED]) return MM_MISS;
	if (magr->data == &mons[PM_IMPALAZON]) return MM_MISS;
	if (magr->data == &mons[PM_MYSTERY_WOMAN]) return MM_MISS;

    /* Rearranged beginning so monsters can use polearms not in a line */
    if (magr->weapon_check == NEED_WEAPON || !MON_WEP(magr)) {
	magr->weapon_check = NEED_RANGED_WEAPON;
	/* mon_wield_item resets weapon_check as appropriate */
	if(mon_wield_item(magr) != 0) return MM_MISS;
    }

    /* Pick a weapon */
    obj = select_rwep(magr,TRUE); /* can also select polearms even when far away from the player --Amy */
    if (!obj) return MM_MISS;

    if (is_applypole(obj)) {
	int dam, hitv, vis = canseemon(magr);

	if (obj->otyp == NOOB_POLLAX || obj->otyp == GREAT_POLLAX) polelimit += 5;
	if (obj->otyp == YITH_TENTACLE) polelimit += 2;
	if (obj->otyp == POLE_LANTERN) polelimit += 10;
	if (obj->otyp == NASTYPOLE) polelimit += 8;
	if (obj->oartifact == ART_ETHER_PENETRATOR) polelimit += 5;
	if (obj->oartifact == ART_FUURKER) polelimit += 6;
	if (obj->otyp == WOODEN_BAR) polelimit += 7;
	if (obj->oartifact == ART_OVERLONG_STICK) polelimit += 12;

	if (dist2(magr->mx, magr->my, mdef->mx, mdef->my) > polelimit ||
		!m_cansee(magr, mdef->mx, mdef->my))
	    return MM_MISS;	/* Out of range, or intervening wall */

	if (vis) {
	    onm = xname(obj);
	    pline("%s thrusts %s.", Monnam(magr),
		  obj_is_pname(obj) ? the(onm) : an(onm));
	}

	dam = dmgval(obj, mdef);
	hitv = 3 - distmin(mdef->mx, mdef->my, magr->mx, magr->my);
	if (hitv < -4) hitv = -4;
	if (bigmonst(mdef->data)) hitv++;
	hitv += 8 + obj->spe;
	if (mdef->mtame) hitv += magr->m_lev;
	if (dam < 1) dam = 1;

	if (find_mac(mdef) + hitv <= rnd(20)) {
	    if (flags.verbose && canseemon(mdef))
		pline("It misses %s.", mon_nam(mdef));
	    else if (vis)
		pline("It misses.");
	    return MM_MISS;
	} else {
	    if (flags.verbose && canseemon(mdef))
		pline("It hits %s%s", a_monnam(mdef), exclam(dam));
	    else if (vis)
		pline("It hits.");
	    if (objects[obj->otyp].oc_material == MT_SILVER &&
		    hates_silver(mdef->data) && canseemon(mdef)) {
		if (vis)
		    pline_The("silver sears %s flesh!",
			    s_suffix(mon_nam(mdef)));
		else
		    pline("%s flesh is seared!", s_suffix(Monnam(mdef)));
	    }
	    if (objects[obj->otyp].oc_material == MT_VIVA && hates_viva(mdef->data) && canseemon(mdef)) {
		    pline("%s is severely hurt by the radiation!", s_suffix(Monnam(mdef)));
	    }
	    if (objects[obj->otyp].oc_material == MT_COPPER && hates_copper(mdef->data) && canseemon(mdef)) {
		    pline("%s decomposes from the contact with copper!", s_suffix(Monnam(mdef)));
	    }
	    if (objects[obj->otyp].oc_material == MT_PLATINUM && hates_platinum(mdef->data) && canseemon(mdef)) {
		    pline("%s is smashed!", s_suffix(Monnam(mdef)));
	    }
	    if (obj->cursed && hates_cursed(mdef->data) && canseemon(mdef)) {
		    pline("%s is blasted by darkness!", s_suffix(Monnam(mdef)));
	    }
	    if (objects[obj->otyp].oc_material == MT_INKA && hates_inka(mdef->data) && canseemon(mdef)) {
		    pline("%s is hurt by the inka string!", s_suffix(Monnam(mdef)));
	    }
	    if (obj->otyp == ODOR_SHOT && hates_odor(mdef->data) && canseemon(mdef)) {
		    pline("%s groans at the odor!", s_suffix(Monnam(mdef)));
	    }
	    mdef->mhp -= dam;
	    if (mdef->mhp < 1) {
		if (canseemon(mdef))
		    pline("%s is %s!", Monnam(mdef),
			    (nonliving(mdef->data) || !canspotmon(mdef))
			    ? "destroyed" : "killed");
		mondied(mdef);
		return MM_DEF_DIED | MM_HIT;
	    }
	    else {
		monster_pain(mdef);
		return MM_HIT;
	    }
	}
    }

    if (!linedup(mdef->mx, mdef->my, magr->mx, magr->my, FALSE))
	return MM_MISS;

    skill = objects[obj->otyp].oc_skill;
    mwep = MON_WEP(magr);		/* wielded weapon */

    if (mwep && ammo_and_launcher(obj, mwep) && objects[mwep->otyp].oc_range &&
	    dist2(magr->mx, magr->my, mdef->mx, mdef->my) >
	    objects[mwep->otyp].oc_range * objects[mwep->otyp].oc_range)
	return MM_MISS; /* Out of range */

    /* Multishot calculations */
    multishot = 1;
    if (( (mwep && ammo_and_launcher(obj, mwep)) || skill == P_DAGGER ||
	    skill == -P_DART || skill == -P_SHURIKEN) && !magr->mconf) {
	/* Assumes lords are skilled, princes are expert */
	if (is_prince(magr->data)) multishot += 2;
	else if (is_lord(magr->data)) multishot++;

	/* strong, nasty or high-level monsters can also shoot more --Amy */
	if (magr->m_lev >= 10 && strongmonst(magr->data) && !rn2(3)) multishot++;
	if (magr->m_lev >= 10 && strongmonst(magr->data) && !rn2(9)) multishot++;
	if (magr->m_lev >= 10 && strongmonst(magr->data) && !rn2(27)) multishot++;

	if (magr->m_lev >= 10 && extra_nasty(magr->data) && !rn2(2)) multishot++;
	if (magr->m_lev >= 10 && extra_nasty(magr->data) && !rn2(4)) multishot++;
	if (magr->m_lev >= 10 && extra_nasty(magr->data) && !rn2(8)) multishot++;

	if (magr->m_lev >= 10 && magr->m_lev < 20) multishot += 1;
	if (magr->m_lev >= 20 && magr->m_lev < 30) multishot += rnd(2);
	if (magr->m_lev >= 30 && magr->m_lev < 40) multishot += rnd(3);
	if (magr->m_lev >= 40 && magr->m_lev < 50) multishot += rnd(4);
	if (magr->m_lev >= 50 && magr->m_lev < 60) multishot += rnd(5);
	if (magr->m_lev >= 60 && magr->m_lev < 70) multishot += rnd(6);
	if (magr->m_lev >= 70 && magr->m_lev < 80) multishot += rnd(7);
	if (magr->m_lev >= 80 && magr->m_lev < 90) multishot += rnd(8);
	if (magr->m_lev >= 90 && magr->m_lev < 100) multishot += rnd(9);
	if (magr->m_lev >= 100) multishot += rnd(10);

	/*  Elven Craftsmanship makes for light,  quick bows */
	if (obj->otyp == ELVEN_ARROW && !obj->cursed)
	    multishot++;
	if (mwep && mwep->otyp == ELVEN_BOW && !mwep->cursed) multishot++;

	if (mwep && mwep->otyp == WILDHILD_BOW && obj->otyp == ODOR_SHOT) multishot++;
	if (mwep && mwep->otyp == COMPOST_BOW && obj->otyp == FORBIDDEN_ARROW) multishot++;

	if (mwep && mwep->otyp == CATAPULT) multishot += rnd(5);

	if (mwep && mwep->otyp == HYDRA_BOW) multishot += 2;
	if (mwep && mwep->otyp == DEMON_CROSSBOW) multishot += 4;
	if (mwep && mwep->otyp == WILDHILD_BOW) multishot += 2;

	/* 1/3 of object enchantment */
	if (mwep && mwep->spe > 1)
	    multishot += rounddiv(mwep->spe, 3);
	/* Some randomness */
	if (multishot > 1)
	    multishot = rnd(multishot);
	if (mwep && objects[mwep->otyp].oc_rof && is_launcher(mwep))
	    multishot += objects[mwep->otyp].oc_rof;

	switch (monsndx(magr->data)) {
	case PM_SPARD:
	case PM_IBERIAN_SOLDIER:
		multishot += 3;
		break;
	case PM_BLUE_ARCHER:
		multishot += 2;
		break;
	case PM_RANGER:
	case PM_ROCKER:
	case PM_GATLING_ARCHER:
		multishot++;
		break;
	case PM_PELLET_ARCHER:
	case PM_ECM_ARCHER:
	case PM_SHOTGUN_HORROR:
	case PM_SHOTGUN_TERROR:
	case PM_KOBOLD_PEPPERMASTER:
		multishot++;
		multishot++;
		break;
	case PM_BRA_GIANT:
		multishot += 5;
		break;
	case PM_ELPH:
		multishot++;
		if (obj->otyp == ELVEN_ARROW && mwep && mwep->otyp == ELVEN_BOW) multishot++;
		break;
	case PM_ROGUE:
		if (skill == P_DAGGER) multishot++;
		break;
	case PM_NINJA_GAIDEN:
	case PM_NINJA:
	case PM_SAMURAI:
		if (obj->otyp == YA && mwep && mwep->otyp == YUMI) multishot++;
		if (obj->otyp == FAR_EAST_ARROW && mwep && mwep->otyp == YUMI) multishot++;
		break;
	default:
	    break;
	}
	/* racial bonus */
	if ((is_elf(magr->data) &&
		obj->otyp == ELVEN_ARROW &&
		mwep && mwep->otyp == ELVEN_BOW) ||
	    (is_orc(magr->data) &&
		obj->otyp == ORCISH_ARROW &&
		mwep && mwep->otyp == ORCISH_BOW))
	    multishot++;

	/* monster-versus-monster is less critical than monster-versus-player, so we don't put the reduction for
	 * weaker monsters here that is present in mthrowu.c --Amy */

	if ((long)multishot > obj->quan) multishot = (int)obj->quan;
	if (multishot < 1) multishot = 1;
	/* else multishot = rnd(multishot); */
    }

    if (canseemon(magr)) {
	char onmbuf[BUFSZ];

	if (multishot > 1) {
	    /* "N arrows"; multishot > 1 implies obj->quan > 1, so
	       xname()'s result will already be pluralized */
	    sprintf(onmbuf, "%d %s", multishot, xname(obj));
	    onm = onmbuf;
	} else {
	    /* "an arrow" */
	    onm = singular(obj, xname);
	    onm = obj_is_pname(obj) ? the(onm) : an(onm);
	}
	m_shot.s = (mwep && ammo_and_launcher(obj,mwep)) ? TRUE : FALSE;
	pline("%s %s %s!", Monnam(magr),
	      m_shot.s ? is_bullet(obj) ? "fires" : "shoots" : "throws",
	      onm);
	m_shot.o = obj->otyp;
    } else {
	m_shot.o = STRANGE_OBJECT;	/* don't give multishot feedback */
    }

    mhp = mdef->mhp;
    m_shot.n = multishot;
    for (m_shot.i = 1; m_shot.i <= m_shot.n; m_shot.i++)
	m_throw(magr, magr->mx, magr->my, sgn(tbx), sgn(tby),
		distmin(magr->mx, magr->my, mdef->mx, mdef->my), obj);
    m_shot.n = m_shot.i = 0;
    m_shot.o = STRANGE_OBJECT;
    m_shot.s = FALSE;

    nomul(0, 0, FALSE);

    return (mdef->mhp < 1 ? MM_DEF_DIED : 0) | (mdef->mhp < mhp ? MM_HIT : 0) |
	   (magr->mhp < 1 ? MM_AGR_DIED : 0);
}

/* Returns the result of mdamagem(). */
STATIC_OVL int
hitmm(magr, mdef, mattk)
	register struct monst *magr,*mdef;
	struct	attack *mattk;
{
	if (DEADMONSTER(magr)) return 0;
	if (DEADMONSTER(mdef)) return 0;

	if(vis){
		int compat;
		char buf[BUFSZ], mdef_name[BUFSZ];

		if (!canspotmon(magr) && !(monstersoundtype(magr) == MS_DEEPSTATE) && !(magr->egotype_deepstatemember))
		    map_invisible(magr->mx, magr->my);
		if (!canspotmon(mdef) && !(monstersoundtype(mdef) == MS_DEEPSTATE) && !(mdef->egotype_deepstatemember))
		    map_invisible(mdef->mx, mdef->my);
		if(mdef->m_ap_type) seemimic(mdef);
		if(magr->m_ap_type) seemimic(magr);
		if((compat = could_seduce(magr,mdef,mattk)) && !magr->mcan) {
			sprintf(buf, "%s %s", Monnam(magr),
				mdef->mcansee ? "smiles at" : "talks to");
			pline("%s %s %s.", buf, mon_nam(mdef),
				compat == 2 ?
					"engagingly" : "seductively");
		} else {
		    char magr_name[BUFSZ];

		    strcpy(magr_name, Monnam(magr));
		    switch (mattk->aatyp) {
			case AT_BITE:
				sprintf(buf,"%s bites", magr_name);
				break;
			case AT_CLAW:
				sprintf(buf,"%s claws", magr_name);
				break;
			case AT_STNG:
				sprintf(buf,"%s stings", magr_name);
				break;
			case AT_BUTT:
				sprintf(buf,"%s butts", magr_name);
				break;
			case AT_LASH:
				sprintf(buf,"%s lashes", magr_name);
				break;
			case AT_TRAM:
				sprintf(buf,"%s tramples over", magr_name);
				break;
			case AT_SCRA:
				sprintf(buf,"%s scratches", magr_name);
				break;
			case AT_TUCH:
				sprintf(buf,"%s touches", magr_name);
				break;
			case AT_BEAM:
				sprintf(buf,"%s blasts", magr_name);
				break;
			case AT_KICK:
				sprintf(buf,"%s kicks", magr_name);
				break;
			case AT_BREA:
				sprintf(buf,"%s breathes at", magr_name);
				break;
			case AT_SPIT:
				sprintf(buf,"%s spits at", magr_name);
				break;
			case AT_TENT:
				sprintf(buf, "%s tentacles suck",
					s_suffix(magr_name));
				break;
			case AT_HUGS:
				if (magr != u.ustuck) {
				    sprintf(buf,"%s squeezes", magr_name);
				    break;
				}
			case AT_MULTIPLY:
				/* No message. */
				break;
			default:
				sprintf(buf,"%s hits", magr_name);
		    }
		    pline("%s %s.", buf, mon_nam_too(mdef_name, mdef, magr));
		}
	} else /* not vis */  noises(magr, mattk);

	/* stooges infighting but not actually hurting each other, ported from nethack 2.3e by Amy */
	if ((magr->data == &mons[PM_STOOGE_LARRY] || magr->data == &mons[PM_STOOGE_CURLY] || magr->data == &mons[PM_STOOGE_MOE]) && (mdef->data == &mons[PM_STOOGE_LARRY] || mdef->data == &mons[PM_STOOGE_CURLY] || mdef->data == &mons[PM_STOOGE_MOE])) {

		if (!rn2(6) && !mdef->mblinded && mdef->mcansee) {
			if(vis) pline("%s is poked in the %s!", Monnam(mdef), mbodypart(mdef, EYE));
			mdef->mcansee = 0;
			mdef->mblinded += rnd(10);
			if (mdef->mblinded <= 0) mdef->mblinded = 127;
		} else if (vis) {
			switch (rn2(100)) {
			case 0 : pline("%s is shoved!", Monnam(mdef));
				break;
			case 1 : pline("%s is kicked!", Monnam(mdef));
				break;
			case 2 : pline("%s is slapped!", Monnam(mdef));
				break;
			case 3 : pline("%s is slugged!", Monnam(mdef));
				break;
			case 4 : pline("%s is punched!", Monnam(mdef));
				break;
			case 5 : pline("%s is pinched!", Monnam(mdef));
				break;
			case 6 : pline("But %s dodges!", mon_nam(mdef));
				break;
			case 7 : pline("But %s ducks!", mon_nam(mdef));
				break;
			case 8 : pline("%s gets a black %s!", Monnam(mdef), mbodypart(mdef, EYE));
				break;
			case 9 : pline("%s gets a bloody %s!", Monnam(mdef), mbodypart(mdef, NOSE));
				break;
			case 10: pline("%s gets a broken tooth!", Monnam(mdef));
				break;
			default: break; /* nothing */
			}
		}
		if (!rn2(2))
			stoogejoke();

		return 0;
	}

	if (mdef->mtame && !rn2(3) && mattk->aatyp == AT_KICK) {
		struct obj *footwear = which_armor(magr, W_ARMF);
		if (footwear && ishighheeled(footwear)) {
			if (vis) pline("%s is kicked by female shoes!", Monnam(mdef));
			badpeteffect(mdef);
			if (DEADMONSTER(mdef)) return 2; /* defender died */

		}
	}

	if (mdef->mtame && !rn2(3) && mattk->aatyp == AT_WEAP) {

		struct obj *heelshoe = MON_WEP(magr);
		if (heelshoe && (heelshoe->otyp == WEDGED_LITTLE_GIRL_SANDAL || heelshoe->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || heelshoe->otyp == BLOCK_HEELED_SANDAL || heelshoe->otyp == PROSTITUTE_SHOE || heelshoe->otyp == BLOCK_HEELED_COMBAT_BOOT || heelshoe->otyp == HUGGING_BOOT || heelshoe->otyp == WOODEN_GETA || heelshoe->otyp == LACQUERED_DANCING_SHOE || heelshoe->otyp == SEXY_MARY_JANE || heelshoe->otyp == KITTEN_HEEL_PUMP || heelshoe->otyp == HIGH_HEELED_SANDAL || heelshoe->otyp == SEXY_LEATHER_PUMP || heelshoe->otyp == INKA_BOOT || heelshoe->otyp == SOFT_LADY_SHOE || heelshoe->otyp == STEEL_CAPPED_SANDAL || heelshoe->otyp == DOGSHIT_BOOT || heelshoe->otyp == SPIKED_BATTLE_BOOT) ) {
			if (vis) pline("%s is whacked by a high-heeled shoe!", Monnam(mdef));
			badpeteffect(mdef);
			if (DEADMONSTER(mdef)) return 2; /* defender died */

		}
	}

	if (DEADMONSTER(mdef)) return 2; /* defender died */

	return(mdamagem(magr, mdef, mattk));
}

/* Returns the same values as mdamagem(). */
STATIC_OVL int
gazemm(magr, mdef, mattk)
	register struct monst *magr, *mdef;
	struct attack *mattk;
{
	char buf[BUFSZ];

	if (DEADMONSTER(magr)) return 0;
	if (DEADMONSTER(mdef)) return 0;

	if(vis) {
		sprintf(buf,"%s gazes at", Monnam(magr));
		pline("%s %s...", buf, mon_nam(mdef));
	}

	if (magr->mcan || !magr->mcansee || magr->minvisreal ||
	    (magr->minvis && !perceives(mdef->data)) ||
	    !mdef->mcansee || mdef->msleeping) {
	    if(vis) pline("but nothing happens.");
	    return(MM_MISS);
	}
	/* call mon_reflects 2x, first test, then, if visible, print message */
	if (magr->data == &mons[PM_MEDUSA] && mon_reflects(mdef, (char *)0)) {
	    if (canseemon(mdef))
		(void) mon_reflects(mdef,
				    "The gaze is reflected away by %s %s.");
	    if (mdef->mcansee) {
		if (mon_reflects(magr, (char *)0)) {
		    if (canseemon(magr))
			(void) mon_reflects(magr,
					"The gaze is reflected away by %s %s.");
		    return (MM_MISS);
		}
		if ((mdef->minvis && !perceives(magr->data)) || mdef->minvisreal) {
		    if (canseemon(magr)) {
			pline("%s doesn't seem to notice that %s gaze was reflected.",
			      Monnam(magr), mhis(magr));
		    }
		    return (MM_MISS);
		}
		if (canseemon(magr))
		    pline("%s is turned to stone!", Monnam(magr));
		monstone(magr);
		if (magr->mhp > 0) return (MM_MISS);
		return (MM_AGR_DIED);
	    }
	}

	return(mdamagem(magr, mdef, mattk));
}

/* Returns the same values as mattackm(). */
STATIC_OVL int
gulpmm(magr, mdef, mattk)
	register struct monst *magr, *mdef;
	register struct	attack *mattk;
{
	xchar	ax, ay, dx, dy;
	int	status;
	char buf[BUFSZ];
	struct obj *obj;

	if (DEADMONSTER(magr)) return 0;
	if (DEADMONSTER(mdef)) return 0;

	if (mdef->data->msize >= MZ_HUGE && magr->data->msize < MZ_HUGE) return MM_MISS;

	if (vis) {
		sprintf(buf,"%s swallows", Monnam(magr));
		pline("%s %s.", buf, mon_nam(mdef));
	}
	for (obj = mdef->minvent; obj; obj = obj->nobj)
	    (void) snuff_lit(obj);

	/*
	 *  All of this maniuplation is needed to keep the display correct.
	 *  There is a flush at the next pline().
	 */
	ax = magr->mx;
	ay = magr->my;
	dx = mdef->mx;
	dy = mdef->my;
	/*
	 *  Leave the defender in the monster chain at it's current position,
	 *  but don't leave it on the screen.  Move the agressor to the def-
	 *  ender's position.
	 */
	remove_monster(ax, ay);
	place_monster(magr, dx, dy);
	newsym(ax,ay);			/* erase old position */
	newsym(dx,dy);			/* update new position */

	status = mdamagem(magr, mdef, mattk);

	if ((status & MM_AGR_DIED) && (status & MM_DEF_DIED)) {
	    ;					/* both died -- do nothing  */
	}
	else if (status & MM_DEF_DIED) {	/* defender died */
	    /*
	     *  Note:  remove_monster() was called in relmon(), wiping out
	     *  magr from level.monsters[mdef->mx][mdef->my].  We need to
	     *  put it back and display it.	-kd
	     */
	    place_monster(magr, dx, dy);
	    newsym(dx, dy);
	}
	else if (status & MM_AGR_DIED) {	/* agressor died */
	    place_monster(mdef, dx, dy);
	    newsym(dx, dy);
	}
	else {					/* both alive, put them back */
	    if (cansee(dx, dy))
		pline("%s is regurgitated!", Monnam(mdef));

	    place_monster(magr, ax, ay);
	    place_monster(mdef, dx, dy);
	    newsym(ax, ay);
	    newsym(dx, dy);
	}

	return status;
}

STATIC_OVL int
explmm(magr, mdef, mattk)
	register struct monst *magr, *mdef;
	register struct	attack *mattk;
{
	int result;

	if (DEADMONSTER(magr)) return 0;
	if (DEADMONSTER(mdef)) return 0;

	if (magr->mcan)
	    return MM_MISS;

	if(cansee(magr->mx, magr->my))
		pline("%s explodes!", Monnam(magr));
	else	noises(magr, mattk);

	remove_monster(magr->mx, magr->my);     /* MAR */
	result = mdamagem(magr, mdef, mattk);
	place_monster(magr,magr->mx, magr->my); /* MAR */

	/* Kill off agressor if it didn't die. */
	if (!(result & MM_AGR_DIED)) {
	    mondead(magr);
	    if (magr->mhp > 0) return result;	/* life saved */
	    result |= MM_AGR_DIED;
	}
	/* KMH -- Player gets blame for flame/freezing sphere */
	if (magr->isspell && !(result & MM_DEF_DIED))
		setmangry(mdef);
	/* give this one even if it was visible, except for spell creatures */
	if (magr->mtame && !magr->isspell)
	    You(brief_feeling, "melancholy");

	return result;
}

/*
 *  See comment at top of mattackm(), for return values.
 */
STATIC_OVL int
mdamagem(magr, mdef, mattk)
	register struct monst	*magr, *mdef;
	register struct attack	*mattk;
{

	if (DEADMONSTER(magr)) return 0;
	if (DEADMONSTER(mdef)) return 0;

	struct obj *obj;
	char buf[BUFSZ];
	struct permonst *pa = magr->data, *pd = mdef->data;
	int armpro, num, tmp = d((int)mattk->damn, (int)mattk->damd);
	boolean cancelled;
	int canhitmon, objenchant;
        boolean nohit = FALSE;

	int petdamagebonus;
	int atttyp;

	if (touch_petrifies(pd) && !rn2(4) && !resists_ston(magr)) {
	    long protector = attk_protection((int)mattk->aatyp),
		 wornitems = magr->misc_worn_check;

	    /* wielded weapon gives same protection as gloves here */
	    if (otmp != 0) wornitems |= W_ARMG;

	    if (protector == 0L ||
		  (protector != ~0L && (wornitems & protector) != protector)) {
		if (poly_when_stoned(pa)) {
		    mon_to_stone(magr);
		    return MM_HIT; /* no damage during the polymorph */
		}
		if (vis) pline("%s turns to stone!", Monnam(magr));
		monstone(magr);
		if (magr->mhp > 0) return 0;
		else if (magr->mtame && !vis)
		    You(brief_feeling, "peculiarly sad");
		return MM_AGR_DIED;
	    }
	}

	canhitmon = 0;
	if (need_one(mdef))    canhitmon += 1;
	if (need_two(mdef))    canhitmon += 2;
	if (need_three(mdef))  canhitmon += 3;
	if (need_four(mdef))   canhitmon += 4;
	if (uarmf && uarmf->oartifact == ART_KILLCAP) canhitmon = 0;

	if (mattk->aatyp == AT_WEAP && otmp) {
	    objenchant = otmp->spe;
	    if (objenchant < 0) objenchant = 0;
	    if (otmp->oartifact) {
		if (otmp->spe < 2) objenchant += 1;
		else objenchant = 2;
	    }
	    if (is_lightsaber(otmp)) objenchant = 4;
	} else objenchant = 0;

	/* a monster that needs a +1 weapon to hit it hits as a +1 weapon... */
	/* overridden by specific flags */
	if (need_one(magr) || hit_as_one(magr))    objenchant += 1;
	if (need_two(magr) || hit_as_two(magr))    objenchant += 2;
	if (need_three(magr) || hit_as_three(magr))  objenchant += 3;
	if (need_four(magr) || hit_as_four(magr))   objenchant += 4;

	if (objenchant < canhitmon && !rn2(3)) nohit = TRUE;

	/* cancellation factor is the same as when attacking the hero */
	armpro = magic_negation(mdef);
	if (mdef->data->mr >= 49) armpro++; /* highly magic resistant monsters should have magic cancellation --Amy */
	if (mdef->data->mr >= 69) armpro++;
	if (mdef->data->mr >= 99) armpro++;
	cancelled = magr->mcan || !((rn2(3) >= armpro) || !rn2(50));

	petdamagebonus = 100;

	if (magr->mtame && !mdef->mtame && !PlayerCannotUseSkills) { /* bonus damage to make pets more viable --Amy */
		switch (P_SKILL(P_PETKEEPING)) {

	      	case P_BASIC:	petdamagebonus += 16; break;
	      	case P_SKILLED:	petdamagebonus += 32; break;
	      	case P_EXPERT:	petdamagebonus += 50; break;
	      	case P_MASTER:	petdamagebonus += 75; break;
	      	case P_GRAND_MASTER:	petdamagebonus += 100; break;
	      	case P_SUPREME_MASTER:	petdamagebonus += 150; break;
			default: break;

		}

	}

	if (magr->mtame && !mdef->mtame) petdamagebonus += (boost_power_value() * 3);

	if (magr->mtame && !mdef->mtame && (magr->data->mlet == S_QUADRUPED) && Race_if(PM_ENGCHIP)) {
		petdamagebonus += 25;
	}

	if (magr->mtame && !mdef->mtame) {
		/* and a little help if pet's experience level is very high, to make large cats etc. more useful --Amy */
		int overlevelled = 0;
		if (magr->m_lev > magr->data->mlevel) overlevelled = ((magr->m_lev - magr->data->mlevel) * 3 / 2);
		if (overlevelled > 0) {
			petdamagebonus += overlevelled;
		}

		/* it is not a bug that uhitm.c multiplies the level difference by two and this function only gives a
		 * 50% boost, because your max level is only 30, while pets can reach 49 --Amy */

	}

	/* riding skill is now finally useful too, as it boosts steed damage --Amy */
	if (u.usteed && magr == u.usteed && !mdef->mtame && !PlayerCannotUseSkills) {
		switch (P_SKILL(P_RIDING)) {

	      	case P_BASIC:	petdamagebonus += 16; break;
	      	case P_SKILLED:	petdamagebonus += 32; break;
	      	case P_EXPERT:	petdamagebonus += 50; break;
	      	case P_MASTER:	petdamagebonus += 75; break;
	      	case P_GRAND_MASTER:	petdamagebonus += 100; break;
	      	case P_SUPREME_MASTER:	petdamagebonus += 150; break;
			default: break;

		}

	}
	if (u.usteed && magr == u.usteed && !mdef->mtame && !PlayerCannotUseSkills && Race_if(PM_PERVERT)) {
		switch (P_SKILL(P_RIDING)) {

	      	case P_BASIC:	petdamagebonus += 8; break;
	      	case P_SKILLED:	petdamagebonus += 16; break;
	      	case P_EXPERT:	petdamagebonus += 25; break;
	      	case P_MASTER:	petdamagebonus += 37; break;
	      	case P_GRAND_MASTER:	petdamagebonus += 50; break;
	      	case P_SUPREME_MASTER:	petdamagebonus += 75; break;
			default: break;

		}

	}

	if (magr->egotype_champion) petdamagebonus += 10; /* smaller bonuses than mhitu, intentional --Amy */
	if (magr->egotype_boss) petdamagebonus += 25;
	if (magr->egotype_atomizer) petdamagebonus += 50;

	/* tame bosses are simply better --Amy */
	if (magr->mtame && !mdef->mtame && (magr->data->geno & G_UNIQ)) petdamagebonus += 25;

	if (petdamagebonus > 100 && magr->mtame && (tmp > 1 || (tmp == 1 && petdamagebonus >= 150) )) {

		tmp *= petdamagebonus;
		tmp /= 100;

	}

	if (mdef->mtame) {
		if (magr->egotype_champion) {
			tmp *= 110;
			tmp /= 100;
		}
		if (magr->egotype_boss) {
			tmp *= 125;
			tmp /= 100;
		}
		if (magr->data->geno & G_UNIQ) {
			tmp *= 125;
			tmp /= 100;
		}
		if (magr->egotype_atomizer) {
			tmp *= 150;
			tmp /= 100;
		}

	}

	atttyp = mattk->adtyp;

	if (mdef->mtame) {
		if (atttyp == AD_RBRE) {
			while (atttyp == AD_ENDS || atttyp == AD_RBRE || atttyp == AD_WERE) {
				atttyp = randattack();
			}
		}

		if (atttyp == AD_DAMA) {
			atttyp = randomdamageattack();
		}

		if (atttyp == AD_ILLU) {
			atttyp = randomillusionattack();
		}

		if (atttyp == AD_THIE) {
			atttyp = randomthievingattack();
		}

		if (atttyp == AD_RNG) {
			while (atttyp == AD_ENDS || atttyp == AD_RNG || atttyp == AD_WERE) {
				atttyp = rn2(AD_ENDS); }
		}

		if (atttyp == AD_PART) atttyp = u.adpartattack;

		if (atttyp == AD_MIDI) {
			atttyp = magr->m_id;
			if (atttyp < 0) atttyp *= -1;
			while (atttyp >= AD_ENDS) atttyp -= AD_ENDS;
			if (!(atttyp >= AD_PHYS && atttyp < AD_ENDS)) atttyp = AD_PHYS; /* fail safe --Amy */
			if (atttyp == AD_WERE) atttyp = AD_PHYS;
		}
	}

	switch(atttyp) {
	    case AD_DGST:

          if (!rnd(25)) { /* since this is an instakill, greatly lower the chance of it connecting --Amy */
		if (nohit) nohit = FALSE;
		/* eating a Rider or its corpse is fatal */
		if (is_rider(mdef->data) || is_deadlysin(mdef->data) ) {
		    if (vis)
			pline("%s %s!", Monnam(magr),
			      mdef->data == &mons[PM_FAMINE] ?
				"belches feebly, shrivels up and dies" :
			      mdef->data == &mons[PM_PESTILENCE] ?
				"coughs spasmodically and collapses" :
				"vomits violently and drops dead");
		    mondied(magr);
		    if (magr->mhp > 0) return 0;	/* lifesaved */
		    else if (magr->mtame && !vis)
			You(brief_feeling, "queasy");
		    return MM_AGR_DIED;
		}
		if(flags.verbose && flags.soundok) verbalize("Burrrrp!");
		tmp = mdef->mhp;
		/* Use up amulet of life saving */
		if (!!(obj = mlifesaver(mdef))) m_useup(mdef, obj);

		/* Is a corpse for nutrition possible?  It may kill magr */
		if (!corpse_chance(mdef, magr, TRUE) || magr->mhp < 1)
		    break;

		/* Pets get nutrition from swallowing monster whole.
		 * No nutrition from G_NOCORPSE monster, eg, undead.
		 * DGST monsters don't die from undead corpses
		 */
		num = monsndx(mdef->data);
		if (magr->mtame && !magr->isminion &&
		    !(mvitals[num].mvflags & G_NOCORPSE)) {
		    struct obj *virtualcorpse = mksobj(CORPSE, FALSE, FALSE, FALSE);
		    int nutrit;

		    if (virtualcorpse) {

			    virtualcorpse->corpsenm = num;
			    virtualcorpse->owt = weight(virtualcorpse);
			    nutrit = dog_nutrition(magr, virtualcorpse);
			    dealloc_obj(virtualcorpse);

		    }

		    /* only 50% nutrition, 25% of normal eating time */
		    if (magr->meating > 1) magr->meating = (magr->meating+3)/4;
		    if (nutrit > 1) nutrit /= 2;
		    EDOG(magr)->hungrytime += nutrit;
		}
          }
		break;
	    case AD_STUN:
		if (magr->mcan) break;
		if (canseemon(mdef))
		    pline("%s %s for a moment.", Monnam(mdef), makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		goto physical;
	    case AD_FUMB:
	    case AD_DROP:
		if (magr->mcan) break;
		if (canseemon(mdef))
		    pline("%s %s for a moment.", Monnam(mdef), makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame && !rn2(4)) {
			mon_adjust_speed(mdef, -1, (struct obj *)0);
			mdef->mstrategy &= ~STRAT_WAITFORU;
			if (!rn2(4)) mdef->inertia += (2 + (tmp * 2));
		}
		goto physical;
	    case AD_TREM:
		if (magr->mcan) break;
		if (canseemon(mdef))
		    pline("%s %s for a moment.", Monnam(mdef), makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (!rn2(3)) badpeteffect(mdef);
		}
		goto physical;
	    case AD_SOUN:
		if (magr->mcan) break;
		if (canseemon(mdef))
		    pline("%s %s for a moment.", Monnam(mdef), makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame) {
			mdef->mconf = 1;
			if (!rn2(5)) badpeteffect(mdef);
		}
		goto physical;
	    case AD_LEGS:
		if (magr->mcan) {
		    tmp = 0;
		    break;
		}
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame && !rn2(7)) {
			mon_adjust_speed(mdef, -1, (struct obj *)0);
			mdef->mstrategy &= ~STRAT_WAITFORU;
			if (tmp > 0) mdef->bleedout += rnd(tmp);
			if (!rn2(3)) mdef->inertia += (3 + (tmp * 3));
		}
		goto physical;
	    case AD_WERE:
		if (mdef->mtame && !rn2(3)) {

		int untamingchance = 10;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_PETKEEPING)) {
				default: untamingchance = 10; break;
				case P_BASIC: untamingchance = 9; break;
				case P_SKILLED: untamingchance = 8; break;
				case P_EXPERT: untamingchance = 7; break;
				case P_MASTER: untamingchance = 6; break;
				case P_GRAND_MASTER: untamingchance = 5; break;
				case P_SUPREME_MASTER: untamingchance = 4; break;
			}
		}

		/* Certain monsters aren't even made peaceful. */
		if (!mdef->iswiz && mdef->data != &mons[PM_MEDUSA] &&
			!(mdef->data->mflags3 & M3_COVETOUS) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && mdef->data->mlet == S_DRAGON) && !(mdef->data->geno & G_UNIQ) &&
			((magr->mtame && !rn2(10)) || (mdef->mtame && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4)) )) {
		    if (vis) pline("%s looks calmer.", Monnam(mdef));
		    if (mdef == u.usteed && !mayfalloffsteed())
			dismount_steed(DISMOUNT_THROWN);
		    if (!mdef->mfrenzied && !rn2(3)) mdef->mpeaceful = 1;
		    else mdef->mpeaceful = 0;
		    mdef->mtame = 0;
		    tmp = 0;
		}

		}
		goto physical;
	    case AD_HEAL:
	    case AD_PHYS:
physical:
		if (mattk->aatyp == AT_WEAP && otmp) {
		    if (otmp->otyp == CORPSE &&
			    touch_petrifies(&mons[otmp->corpsenm]) && nohit)
			nohit = FALSE;
		} else if(nohit) break;
		if (mattk->aatyp == AT_KICK && thick_skinned(pd) && tmp && rn2(3)) {
		    tmp = 1;
		} else if(mattk->aatyp == AT_WEAP) {
		    if(otmp) {
			if (otmp->otyp == CORPSE &&
				touch_petrifies(&mons[otmp->corpsenm]) && !(magr->mtame && nocorpsedecay(&mons[otmp->corpsenm])) )
			    goto do_stone;

			if (otmp->otyp == CORPSE &&
				touch_petrifies(&mons[otmp->corpsenm]) && (magr->mtame && nocorpsedecay(&mons[otmp->corpsenm])) ) {
				m_useup(magr, otmp);
				otmp = (struct obj *) 0;
				possibly_unwield(magr, FALSE);
				return 0;
			}

			/* WAC -- Real weapon?
			 * Could be stuck with a cursed bow/polearm it wielded
			 */
			if (/* if you strike with a bow... */
			    is_launcher(otmp) ||
			    /* or strike with a missile in your hand... */
			    (is_missile(otmp) || is_ammo(otmp)) ||
			    /* lightsaber that isn't lit ;) */
			    (is_lightsaber(otmp) && !otmp->lamplit) ||
			    /* WAC -- or using a pole at short range... */
			    (is_pole(otmp))) {
			    /* then do only 1-2 points of damage */
			    if ( (is_shade(pd) || (mdef && mdef->egotype_shader)) && !(uwep && uwep->oartifact == ART_AP_) && objects[otmp->otyp].oc_material != MT_SILVER && objects[otmp->otyp].oc_material != MT_ARCANIUM)
				tmp = 0;
			    else
				tmp = rnd(2);

#if 0 /* Monsters don't wield boomerangs */
		    	    if(otmp->otyp == BOOMERANG /* && !rnl(3) */) {
				pline("As %s hits you, %s breaks into splinters.",
				      mon_nam(mtmp), the(xname(otmp)));
				useup(otmp);
				otmp = (struct obj *) 0;
				possibly_unwield(mtmp);
				if ((!is_shade(pd) && !(mdef && mdef->egotype_shader)) || (uwep && uwep->oartifact == ART_AP_) )
				    tmp++;
		    	    }
#endif
			} else tmp += dmgval(otmp, mdef);

			/* MRKR: Handling damage when hitting with */
			/*       a burning torch */

			if(otmp->otyp == TORCH && otmp->lamplit && (!resists_fire(mdef) || player_will_pierce_resistance() ) ) {

			  if (!Blind) {
			    static char outbuf[BUFSZ];
			    char *s = Shk_Your(outbuf, otmp);

			    boolean water = (mdef->data ==
					     &mons[PM_WATER_ELEMENTAL]);

			    pline("%s %s %s%s %s%s.", s, xname(otmp),
				  (water ? "vaporize" : "burn"),
				  (otmp->quan > 1L ? "" : "s"),
				  (water ? "part of " : ""), mon_nam(mdef));
			  }

			  burn_faster(otmp, 1);

			  tmp++;
			  if (resists_cold(mdef)) tmp += rnd(3);

			  if (!rn2(33)) burnarmor(mdef);
			    if (!rn2(33))
			      (void)destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
			    if (!rn2(33))
			      (void)destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
			    if (!rn2(50))
			      (void)destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);

			}

                        /* WAC Weres get seared */
                        if(otmp && objects[otmp->otyp].oc_material == MT_SILVER && (hates_silver(pd))) {
                                tmp += 8;
                                if (vis) pline("The silver sears %s!", mon_nam(mdef));
                        }
                        if(otmp && objects[otmp->otyp].oc_material == MT_COPPER && (hates_copper(pd))) {
                                tmp += 20;
                                if (vis) pline("The copper decomposes %s!", mon_nam(mdef));
                        }
                        if(otmp && objects[otmp->otyp].oc_material == MT_MERCURIAL && !rn2(10) && !(resists_poison(mdef))) {
                                tmp += rnd(4);
                                if (vis) pline("The mercury poisons %s!", mon_nam(mdef));
                        }
                        if(otmp && objects[otmp->otyp].oc_material == MT_PLATINUM && (hates_platinum(pd))) {
                                tmp += 20;
                                if (vis) pline("The platinum smashes %s!", mon_nam(mdef));
                        }
                        if(otmp && otmp->cursed && (hates_cursed(pd))) {
                                tmp += 4;
					  if (otmp->hvycurse) tmp += 4;
					  if (otmp->prmcurse) tmp += 7;
					  if (otmp->bbrcurse) tmp += 15;
					  if (otmp->evilcurse) tmp += 15;
					  if (otmp->morgcurse) tmp += 15;
                                if (vis) pline("The unholy aura blasts %s!", mon_nam(mdef));
                        }
                        if(otmp && objects[otmp->otyp].oc_material == MT_VIVA && (hates_viva(pd))) {
                                tmp += 20;
                                if (vis) pline("The radiation damages %s!", mon_nam(mdef));
                        }
                        if(otmp && objects[otmp->otyp].oc_material == MT_INKA && (hates_inka(pd))) {
                                tmp += 5;
                                if (vis) pline("The inka string damages %s!", mon_nam(mdef));
                        }
                        if(otmp && otmp->otyp == ODOR_SHOT && (hates_odor(pd))) {
                                tmp += rnd(10);
                                if (vis) pline("The odor beguils %s!", mon_nam(mdef));
                        }
                        /* Stakes do extra dmg agains vamps */
                        if (otmp && (otmp->otyp == WOODEN_STAKE || otmp->oartifact == ART_VAMPIRE_KILLER) && is_vampire(pd)) {
                                if(otmp->oartifact == ART_STAKE_OF_VAN_HELSING) {
                                        if (!rn2(10)) {
                                                if (vis) {
                                                        strcpy(buf, Monnam(magr));
                                                        pline("%s plunges the stake into the heart of %s.",
                                                                buf, mon_nam(mdef));
                                                        pline("%s's body vaporizes!", Monnam(mdef));
                                                }
                                                mondead(mdef); /* no corpse */
                                                if (mdef->mhp < 0) return (MM_DEF_DIED |
                                                        (grow_up(magr,mdef) ? 0 : MM_AGR_DIED));
                                        } else {
                                                if (vis) {
                                                        strcpy(buf, Monnam(magr));
                                                        pline("%s drives the stake into %s.",
                                                                buf, mon_nam(mdef));
                                                }
                                                tmp += rnd(6) + 2;
                                        }
                                }else if (otmp->oartifact == ART_VAMPIRE_KILLER) {
                                        if (vis) {
                                                strcpy(buf, Monnam(magr));
                                                pline("%s whips %s good!",
                                                        buf, mon_nam(mdef));
                                        }
                                        tmp += rnd(6);
                                }
					 else {
                                        if (vis) {
                                                strcpy(buf, Monnam(magr));
                                                pline("%s drives the stake into %s.",
                                                        buf, mon_nam(mdef));
                                        }
                                        tmp += rnd(6);
                                }
                        }

                        if (otmp && otmp->oartifact) {
			    (void)artifact_hit(magr,mdef, otmp, &tmp, dieroll);
			    if (mdef->mhp <= 0)
				return (MM_DEF_DIED |
					(grow_up(magr,mdef) ? 0 : MM_AGR_DIED));
			}
			if (otmp && tmp)
				mrustm(magr, mdef, otmp);
		    }
		} else if (magr->data == &mons[PM_PURPLE_WORM] &&
			    mdef->data == &mons[PM_SHRIEKER]) {
		    /* hack to enhance mm_aggression(); we don't want purple
		       worm's bite attack to kill a shrieker because then it
		       won't swallow the corpse; but if the target survives,
		       the subsequent engulf attack should accomplish that */
		    if (tmp >= mdef->mhp) tmp = mdef->mhp - 1;
		}
		break;
	    case AD_FIRE:
		if (nohit) break;

		if (cancelled) {
		    tmp = 0;
		    break;
		}
		if (vis)
		    pline("%s is %s!", Monnam(mdef),
			  on_fire(mdef->data, mattk));
		if (pd == &mons[PM_STRAW_GOLEM] ||
		    pd == &mons[PM_WAX_GOLEM] ||
		    pd == &mons[PM_PAPER_GOLEM]) {
			if (vis) pline("%s burns completely!", Monnam(mdef));
			mondied(mdef);
			if (mdef->mhp > 0) return 0;
			else if (mdef->mtame && !vis)
			    pline("May %s roast in peace.", mon_nam(mdef));
			return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		}
		if (!rn2(33)) tmp += destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
		if (!rn2(33)) tmp += destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
		if (resists_fire(mdef) && !player_will_pierce_resistance()) {
		    if (vis)
			pline_The("fire doesn't seem to burn %s!",
								mon_nam(mdef));
		    shieldeff(mdef->mx, mdef->my);
		    golemeffects(mdef, AD_FIRE, tmp);
		    tmp = 0;
		}
		/* only potions damage resistant players in destroy_item */
		if (!rn2(33)) tmp += destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
		break;
	    case AD_COLD:
		if (nohit) break;

		if (cancelled) {
		    tmp = 0;
		    break;
		}
		if (vis) pline("%s is covered in frost!", Monnam(mdef));
		if (resists_cold(mdef) && !player_will_pierce_resistance()) {
		    if (vis)
			pline_The("frost doesn't seem to chill %s!",
								mon_nam(mdef));
		    shieldeff(mdef->mx, mdef->my);
		    golemeffects(mdef, AD_COLD, tmp);
		    tmp = 0;
		}
		if (!rn2(33)) tmp += destroy_mitem(mdef, POTION_CLASS, AD_COLD);
		break;
	    case AD_ELEC:
		if (nohit) break;

		if (cancelled) {
		    tmp = 0;
		    break;
		}
		if (vis) pline("%s gets zapped!", Monnam(mdef));
		if (!rn2(33)) tmp += destroy_mitem(mdef, WAND_CLASS, AD_ELEC);
		if (resists_elec(mdef) && !player_will_pierce_resistance()) {
		    if (vis) pline_The("zap doesn't shock %s!", mon_nam(mdef));
		    shieldeff(mdef->mx, mdef->my);
		    golemeffects(mdef, AD_ELEC, tmp);
		    tmp = 0;
		}
		/* only rings damage resistant players in destroy_item */
		if (!rn2(33)) tmp += destroy_mitem(mdef, RING_CLASS, AD_ELEC);
		break;
	    case AD_ACID:
		if (nohit) break;

		if (magr->mcan) {
		    tmp = 0;
		    break;
		}
		if (resists_acid(mdef) && !player_will_pierce_resistance()) {
		    if (vis)
			pline("%s is covered in acid, but it seems harmless.",
			      Monnam(mdef));
		    tmp = 0;
		} else if (vis) {
		    pline("%s is covered in acid!", Monnam(mdef));
		    pline("It burns %s!", mon_nam(mdef));
		}
		if (!rn2(30)) erode_armor(mdef, TRUE);
		if (!rn2(6)) erode_obj(MON_WEP(mdef), TRUE, TRUE);
		break;
	    case AD_RUST:
		if (magr->mcan) break;
		if (pd == &mons[PM_IRON_GOLEM]) {
			if (vis) pline("%s falls to pieces!", Monnam(mdef));
			mondied(mdef);
			if (mdef->mhp > 0) return 0;
			else if (mdef->mtame && !vis)
			    pline("May %s rust in peace.", mon_nam(mdef));
			return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		}
		hurtmarmor(mdef, AD_RUST);
		mdef->mstrategy &= ~STRAT_WAITFORU;
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (pd == &mons[PM_IRON_GOLEM]) tmp = 0;
		if (mdef->mtame && !rn2(5)) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (vis) pline("%s rusts!", Monnam(mdef));
		}
		break;
	    case AD_LITE:
		if (is_vampire(mdef->data)) {
			tmp *= 2; /* vampires take more damage from sunlight --Amy */
			if (vis) pline("%s is irradiated!", Monnam(mdef));
		}
		if (mdef->mtame) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}
		break;

	    case AD_CORR:
		if (magr->mcan) break;
		hurtmarmor(mdef, AD_CORR);
		mdef->mstrategy &= ~STRAT_WAITFORU;
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		/*tmp = 0;*/
		if (mdef->mtame && !rn2(5)) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (vis) pline("%s corrodes!", Monnam(mdef));
		}
		break;
	    case AD_FLAM:
	    case AD_WNCE:
		if (magr->mcan) break;
		hurtmarmor(mdef, AD_FLAM);
		mdef->mstrategy &= ~STRAT_WAITFORU;
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		/*tmp = 0;*/
		if (mdef->mtame && !rn2(5)) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (vis) pline("%s is engulfed by flames!", Monnam(mdef));
		}
		break;
	    case AD_DCAY:
		if (magr->mcan) break;
		if (pd == &mons[PM_WOOD_GOLEM] ||
		    pd == &mons[PM_LEATHER_GOLEM]) {
			if (vis) pline("%s falls to pieces!", Monnam(mdef));
			mondied(mdef);
			if (mdef->mhp > 0) return 0;
			else if (mdef->mtame && !vis)
			    pline("May %s rot in peace.", mon_nam(mdef));
			return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		}
		hurtmarmor(mdef, AD_DCAY);
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (pd == &mons[PM_WOOD_GOLEM] || pd == &mons[PM_LEATHER_GOLEM]) tmp = 0;
		if (mdef->mtame && !rn2(5)) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (vis) pline("%s decays!", Monnam(mdef));
		}
		break;
	    case AD_STON:
	    case AD_EDGE:
		if (magr->mcan) break;

		if (mdef->mtame && atttyp == AD_EDGE) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}

		if (mattk->aatyp == AT_GAZE && mon_reflects(mdef, (char *)0)) {
		    tmp = 0;
		    (void) mon_reflects(mdef, "But it reflects from %s %s!");
		    if (poly_when_stoned(pa)) {
			mon_to_stone(magr);
			break;
		    }
		    if (!resists_ston(magr) && !rn2(4) ) {
			if (vis) pline("%s turns to stone!", Monnam(magr));
			monstone(magr);
			if (magr->mhp > 0) return 0;
			else if (magr->mtame && !vis)
			    You(brief_feeling, "peculiarly sad");
			return MM_AGR_DIED;
		    }
		}
 do_stone:
		/* may die from the acid if it eats a stone-curing corpse */
		if (munstone(mdef, FALSE)) goto post_stone;
		if (poly_when_stoned(pd)) {
			mon_to_stone(mdef);
			tmp = 0;
			break;
		}
		if (!resists_ston(mdef) && !rn2(4) ) {
			if (vis) pline("%s turns to stone!", Monnam(mdef));
			monstone(mdef);
 post_stone:		if (mdef->mhp > 0) return 0;
			else if (mdef->mtame && !vis)
			    You(brief_feeling, "peculiarly sad");
			return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
			tmp = (mattk->adtyp == AD_STON ? 0 : mattk->adtyp == AD_EDGE ? 0 : 1);
		}
		break;
	    case AD_BANI:
		if (mdef->mtame && !rn2(3)) {
			mdef->willbebanished = TRUE;
			break;
		} /* else fall through */
	    case AD_TLPT:
		if (!cancelled && tmp < mdef->mhp && !tele_restrict(mdef)) {
		    char mdef_Monnam[BUFSZ];
		    /* save the name before monster teleports, otherwise
		       we'll get "it" in the suddenly disappears message */
		    if (vis) strcpy(mdef_Monnam, Monnam(mdef));
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    (void) rloc(mdef, FALSE);
		    if (vis && !canspotmon(mdef) && mdef != u.usteed )
			pline("%s suddenly disappears!", mdef_Monnam);
		}
		break;
	    case AD_AMNE:

		if (mdef->mtame) {
			if (vis) pline("%s seems oblivious!", Monnam(mdef));
			mdef->mconf = 1;
			if (!rn2(3)) badpeteffect(mdef);
		}
		break;

	    case AD_NEXU:
		if (!cancelled && tmp < mdef->mhp && !tele_restrict(mdef)) {
		    char mdef_Monnam[BUFSZ];
		    /* save the name before monster teleports, otherwise
		       we'll get "it" in the suddenly disappears message */
		    if (vis) strcpy(mdef_Monnam, Monnam(mdef));
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    (void) rloc(mdef, FALSE);
		    if (vis && !canspotmon(mdef) && mdef != u.usteed )
			pline("%s suddenly disappears!", mdef_Monnam);
		}
		if (mdef->mtame) {
			if (!rn2(2)) {
				if (mdef->mhpmax > 1) mdef->mhpmax--;
				if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			}
			if (!rn2(7)) badpeteffect(mdef);
		}
		break;
	    case AD_ABDC:
		if (!cancelled && tmp < mdef->mhp && !tele_restrict(mdef)) {
		    char mdef_Monnam[BUFSZ];
		    /* save the name before monster teleports, otherwise
		       we'll get "it" in the suddenly disappears message */
		    if (vis) strcpy(mdef_Monnam, Monnam(mdef));
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    (void) rloc(mdef, FALSE);
		    if (vis && !canspotmon(mdef) && mdef != u.usteed )
			pline("%s suddenly disappears!", mdef_Monnam);
		}
		if (mdef->mtame) badpeteffect(mdef);
		break;
	    case AD_SLEE:
		if (nohit) break;

		if (cancelled) break;
		if (mattk->aatyp == AT_GAZE && mon_reflects(mdef, (char *)0)) {
		    tmp = 0;
		    (void) mon_reflects(mdef, "But it reflects from %s %s!");
		    if (sleep_monst(magr, rnd(10), -1))
			if (vis) pline("%s is put to sleep!", Monnam(magr));
		    break;
		}

		{
		int parlyzdur = rnd(magr->mtame ? 4 : 10);
		if (!rn2(4)) parlyzdur = tmp;
		if (magr->mtame) {
			if (parlyzdur > 3) {
				parlyzdur = rnd(parlyzdur);
				if (parlyzdur < 3) parlyzdur = 2;
			}
			if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
		}
		if (parlyzdur > 127) parlyzdur = 127;

		if (!cancelled && !(magr->mtame && (rn2(100) <= mdef->data->mr)) && !mdef->msleeping && (!rn2(5) || !(magr->mtame)) &&
			sleep_monst(mdef, parlyzdur, -1)) {
		    if (vis) {
			strcpy(buf, Monnam(mdef));
			pline("%s is put to sleep by %s.", buf, mon_nam(magr));
		    }
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    slept_monst(mdef);
		}

		}
		break;
	    /* WAC DEATH (gaze) */
	    case AD_DETH:
		if (rn2(16)) {
		    /* No death, but still cause damage */
		    break;
		}
		if (vis && mattk->aatyp == AT_GAZE)
		    pline("%s gazes intently!", Monnam(magr));
		if (mattk->aatyp == AT_GAZE && mon_reflects(mdef, (char *)0)) {
		    /* WAC reflected gaze
		     * Oooh boy...that was a bad move :B
		     */
		    tmp = 0;
		    if (vis) {
			shieldeff(mdef->mx, mdef->my);
			(void) mon_reflects(mdef, "But it reflects from %s %s!");
		    }
		    if (resists_magm(magr)) {
			if (vis) pline("%s shudders momentarily...", Monnam(magr));
			break;
		    }
		    if (vis) pline("%s dies!", Monnam(magr));
		    mondied(magr);
		    if (magr->mhp > 0) return 0;  /* lifesaved */
		    else if (magr->mtame && !vis)
			You(brief_feeling, "peculiarly sad");
		    return MM_AGR_DIED;
		} else if (is_undead(mdef->data) || mdef->egotype_undead) {
		    /* Still does normal damage */
		    if (vis) pline("Something didn't work...");
		    break;
		} else if (resists_magm(mdef)) {
		    if (vis) pline("%s shudders momentarily...", Monnam(mdef));
		} else {
		    tmp = mdef->mhp;
		}
		break;
	    case AD_PLYS:
		if (nohit) break;
		if(!cancelled && mdef->mcanmove && (!rn2(4) || !(magr->mtame)) && !(dmgtype(mdef->data, AD_PLYS))) {

			int parlyzdur = rnd(magr->mtame ? 4 : 10);
			if (!rn2(4)) parlyzdur = tmp;
			if (magr->mtame) {
				if (parlyzdur > 3) {
					parlyzdur = rnd(parlyzdur);
					if (parlyzdur < 3) parlyzdur = 3;
				}
				if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
			}
			if (parlyzdur > 127) parlyzdur = 127;

		    if (vis) {
			strcpy(buf, Monnam(mdef));
			pline("%s is frozen by %s.", buf, mon_nam(magr));
		    }
		    mdef->mcanmove = 0;
		    mdef->mfrozen = parlyzdur;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		break;
	    case AD_TCKL:
		if(!cancelled && mdef->mcanmove && (!rn2(8) || !(magr->mtame)) && !(dmgtype(mdef->data, AD_PLYS))) {

			int parlyzdur = rnd(magr->mtame ? 3 : 10);
			if (!rn2(5)) parlyzdur = tmp;
			if (magr->mtame) {
				if (parlyzdur > 2) {
					parlyzdur = rnd(parlyzdur);
					if (parlyzdur < 2) parlyzdur = 2;
				}
				if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
			}
			if (parlyzdur > 127) parlyzdur = 127;

		    if (vis) {
			strcpy(buf, Monnam(magr));
			pline("%s mercilessly tickles %s.", buf, mon_nam(mdef));
		    }
		    mdef->mcanmove = 0;
		    mdef->mfrozen = parlyzdur;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
  		}
		break;
	    case AD_SLOW:
		if (nohit) break;
		if(!cancelled && vis && mdef->mspeed != MSLOW && !(magr->mtame && (rn2(100) <= mdef->data->mr)) ) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (mdef->mspeed != oldspeed && vis)
			pline("%s slows down.", Monnam(mdef));
		}
		break;
	    case AD_WGHT:
		if (nohit) break;
		if(!cancelled && vis && mdef->mspeed != MSLOW && !(magr->mtame && (rn2(100) <= mdef->data->mr))) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (mdef->mspeed != oldspeed && vis)
			pline("%s slows down.", Monnam(mdef));
		}
		if (mdef->mtame) {
			if (vis) pline("%s's weight increases.", Monnam(mdef));
			mdef->inertia += tmp;
		}
		break;
	    case AD_INER:
		if (nohit) break;
		if(!cancelled && vis && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (mdef->mspeed != oldspeed && vis)
			pline("%s slows down.", Monnam(mdef));
		}
		if (mdef->mtame) {
			if (vis) {
				if (!mdef->inertia) pline("%s slows down to a crawl.", Monnam(mdef));
				else pline("%s seems even slower.", Monnam(mdef));
			}
			mdef->inertia += (5 + (tmp * 5));
		}
		break;
	    case AD_LAZY:
		if (nohit) break;
		if(!cancelled && vis && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (mdef->mspeed != oldspeed && vis)
			pline("%s slows down.", Monnam(mdef));
		}
		if(!cancelled && !rn2(3) && (!rn2(4) || !(magr->mtame)) && mdef->mcanmove && !(dmgtype(mdef->data, AD_PLYS))) {

			int parlyzdur = rnd(magr->mtame ? 4 : 10);
			if (!rn2(4)) parlyzdur = tmp;
			if (magr->mtame) {
				if (parlyzdur > 3) {
					parlyzdur = rnd(parlyzdur);
					if (parlyzdur < 3) parlyzdur = 3;
				}
				if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
			}
			if (parlyzdur > 127) parlyzdur = 127;

		    if (vis) {
			strcpy(buf, Monnam(mdef));
			pline("%s is frozen by %s.", buf, mon_nam(magr));
		    }
		    mdef->mcanmove = 0;
		    mdef->mfrozen = parlyzdur;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		if (mdef->mtame) {
			mdef->inertia += (3 + (tmp * 3));
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}
		break;
	    case AD_NUMB:
		if (nohit) break;
		if(!cancelled && !rn2(10) && vis && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (mdef->mspeed != oldspeed && vis)
			pline("%s is numbed.", Monnam(mdef));
		}
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame && !rn2(15)) badpeteffect(mdef);
		break;
	    case AD_DARK:
		do_clear_area(mdef->mx,mdef->my, 7, set_lit, (void *)((char *)0));
		if (vis) pline("A sinister darkness fills the area!");
		if (mdef->data->mlet == S_ANGEL) tmp *= 2;
		break;

	    case AD_THIR:
		if (magr->mhp > 0) {
		magr->mhp += tmp;
		if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		if (vis) pline("%s feeds on the lifeblood!", Monnam(magr) );
		}

		break;

	    case AD_NTHR:
		if (magr->mhp > 0) {
		magr->mhp += tmp;
		if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		if (vis) pline("%s feeds on the lifeblood!", Monnam(magr) );
		}

		if (mdef->mtame && !rn2(10)) {

			if (!cancelled && rn2(2) && !resists_drli(mdef)) {
				if (vis) pline("%s suddenly seems weaker!", Monnam(mdef));
				mdef->mhpmax -= d(2,6);
				if (mdef->mhpmax < 1) mdef->mhpmax = 1;
				if (mdef->m_lev == 0) tmp = mdef->mhp;
				else mdef->m_lev--;
				/* Automatic kill if drained past level 0 */
			}

		}

		break;

	    case AD_VULN:
		if (mdef->mtame) {

			if (vis) pline("%s seems more vulnerable!", Monnam(mdef));
			mdef->mhpmax -= rnd(8);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev == 0) tmp = mdef->mhp;
			else mdef->m_lev--;

		}

		break;

	    case AD_NAST:
		if (mdef->mtame) {
			badpeteffect(mdef);
			badpeteffect(mdef);
			badpeteffect(mdef);
		}
		break;

	    case AD_CAST:
		if (mdef->mtame && !rn2(2)) tmp *= 3;
		break;

	    case AD_CLRC:
		if (mdef->mtame && !rn2(2)) tmp *= 2;
		break;

	    case AD_SPEL:
		if (mdef->mtame && !rn2(2)) tmp *= 2;
		break;

	    case AD_SAMU:
		if (mdef->mtame && !rn2(3)) badpeteffect(mdef);
		break;

	    case AD_RUNS:
		if (mdef->mtame) {
			badpeteffect(mdef);
			badpeteffect(mdef);
			badpeteffect(mdef);
		}
		break;

	    case AD_MINA:
		if (mdef->mtame) {
			badpeteffect(mdef);
			badpeteffect(mdef);
			badpeteffect(mdef);
		}
		break;

	    case AD_DATA:
		if (mdef->mtame) {
			tmp += 10000;
			allbadpeteffects(mdef);
		}
		break;

	    case AD_BLEE:
		if (tmp > 0) {
			mdef->bleedout += tmp;
			if (vis) pline("%s sustains a cut.", Monnam(mdef));
		}
		break;

	    case AD_SHAN:
		if (mdef->mtame) {
			if (!rn2(3)) badpeteffect(mdef);
			if (!rn2(3)) {
				if (mdef->mhpmax > 1) mdef->mhpmax--;
				if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			}
		}
		break;

	    case AD_SCOR:
		if (mdef->mtame && !rn2(40)) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}
		break;

	    case AD_TERR:
		if (magr->data == &mons[PM_BUILDER]) terrainterror(1);
		else terrainterror(0);
		break;

	    case AD_FEMI:
		if (mdef->mtame) {
			badpeteffect(mdef);
		}
		break;

	    case AD_LEVI:
		if (mdef->mtame && !rn2(5)) {
			badpeteffect(mdef);
		}
		break;

	    case AD_MCRE:
		if (mdef->mtame && !rn2(3)) {
			badpeteffect(mdef);
		}
		break;

	    case AD_BADE:
		if (mdef->mtame) {
			badpeteffect(mdef);
		}
		break;

	    case AD_RBAD:
		if (mdef->mtame) {
			badpeteffect(mdef);
			badpeteffect(mdef);
			badpeteffect(mdef);
		}
		break;

	    case AD_RAGN:

		ragnarok(FALSE);
		if (evilfriday && magr->m_lev > 1) evilragnarok(FALSE,magr->m_lev);
		break;

	    case AD_AGGR:
		aggravate();
		if (!rn2(20)) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			(void) makemon((struct permonst *)0, magr->mx, magr->my, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
			u.aggravation = 0;
		}

		break;

	    case AD_CONT:

		if (!rn2(30)) {
			mdef->isegotype = 1;
			mdef->egotype_contaminator = 1;
		}
		if (!rn2(100)) {
			mdef->isegotype = 1;
			mdef->egotype_weeper = 1;
		}
		if (!rn2(250)) {
			mdef->isegotype = 1;
			mdef->egotype_radiator = 1;
		}
		if (!rn2(250)) {
			mdef->isegotype = 1;
			mdef->egotype_reactor = 1;
		}

		if (mdef->mtame) {
			if (!rn2(30)) {
				int untamingchance = 10;

				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_PETKEEPING)) {
						default: untamingchance = 10; break;
						case P_BASIC: untamingchance = 9; break;
						case P_SKILLED: untamingchance = 8; break;
						case P_EXPERT: untamingchance = 7; break;
						case P_MASTER: untamingchance = 6; break;
						case P_GRAND_MASTER: untamingchance = 5; break;
						case P_SUPREME_MASTER: untamingchance = 4; break;
					}
				}

				/* Certain monsters aren't even made peaceful. */
				if (!mdef->iswiz && mdef->data != &mons[PM_MEDUSA] &&
					!(mdef->data->mflags3 & M3_COVETOUS) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && mdef->data->mlet == S_DRAGON) && !(mdef->data->geno & G_UNIQ) &&
					((magr->mtame && !rn2(10)) || (mdef->mtame && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4)) )) {
				    if (vis) pline("%s looks calmer.", Monnam(mdef));
				    if (mdef == u.usteed && !mayfalloffsteed())
					dismount_steed(DISMOUNT_THROWN);
				    if (!mdef->mfrenzied && !rn2(3)) mdef->mpeaceful = 1;
				    else mdef->mpeaceful = 0;
				    mdef->mtame = 0;
				    tmp = 0;
				}

			} else if (!rn2(3)) {
				badpeteffect(mdef);
			} else {
				if (mdef->mhpmax > 1) mdef->mhpmax--;
				if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			}
		}

		break;

	    case AD_FRZE:
		if (!resists_cold(mdef) && resists_fire(mdef)) {
			tmp *= 2;
			if (vis) pline("%s is freezing!", Monnam(mdef));
		}
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame) {
			pline("%s seems to be moving slower.", Monnam(mdef));
			if (!rn2(3)) mon_adjust_speed(mdef, -1, (struct obj *)0);
			if (!rn2(3)) mdef->inertia += tmp;
		}

		break;
	    case AD_ICEB:
		if (!resists_cold(mdef)) {
			tmp *= 2;
			if (vis) pline("%s is hit with ice blocks!", Monnam(mdef));
		}
		if (mdef->mtame && !rn2(2)) {
			pline("%s seems to be moving slower.", Monnam(mdef));
			if (!rn2(3)) mon_adjust_speed(mdef, -1, (struct obj *)0);
			if (!rn2(3)) mdef->inertia += tmp;
		}

		break;

	    case AD_MALK:
		if (!resists_elec(mdef)) {
			tmp *= 2;
			if (vis) pline("%s is shocked!", Monnam(mdef));
		}

		if (mdef->mtame && !rn2(5)) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}

		break;

	    case AD_UVUU:
		if (has_head(mdef->data)) {
			tmp *= 2;
			if (!rn2(1000)) {
				tmp *= 100;
				if (vis) pline("%s's %s is torn apart!", Monnam(mdef), mbodypart(mdef, HEAD));
			} else if (vis) pline("%s's %s is spiked!", Monnam(mdef), mbodypart(mdef, HEAD));
		}

		if (mdef->mtame) {
			tmp *= 2;
			if (!which_armor(mdef, W_ARMH)) tmp *= 2;
			badpeteffect(mdef);
		}

		break;

	    case AD_GRAV:
		if (!is_flyer(mdef->data)) {
			tmp *= 2;
			if (vis) pline("%s is slammed into the ground!", Monnam(mdef));
		}

		if (mdef->mtame && !rn2(2) && !mdef->mfrozen) {
			mdef->mcanmove = 0;
			mdef->mfrozen = rnd(10);
			mdef->mstrategy &= ~STRAT_WAITFORU;
		}

		break;

	    case AD_CHKH:
		if (magr->m_lev > mdef->m_lev) tmp += (magr->m_lev - mdef->m_lev);
		break;

	    case AD_CHRN:
		if ((tmp > 0) && (mdef->mhpmax > 1)) {
			mdef->mhpmax--;
			if (vis) pline("%s feels bad!", Monnam(mdef));
		}
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame) badpeteffect(mdef);
		break;

	    case AD_HODS:
		tmp += mdef->m_lev;
		break;

	    case AD_DIMN:
		tmp += magr->m_lev;
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame && !rn2(10)) badpeteffect(mdef);
		break;

	    case AD_BURN:
		if (resists_cold(mdef) && !resists_fire(mdef)) {
			tmp *= 2;
			if (vis) pline("%s is burning!", Monnam(mdef));
		}
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame) mdef->healblock += (5 + (tmp * 5));

		break;

	    case AD_PLAS:
		if (!resists_fire(mdef)) {
			tmp *= 2;
			if (vis) pline("%s is enveloped by searing plasma radiation!", Monnam(mdef));
		}
		if (mdef->mtame) {
			mdef->healblock += (10 + (tmp * 10));
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}

		break;

	    case AD_SLUD:
		if (!resists_acid(mdef)) {
			tmp *= 2;
			if (vis) pline("%s is covered with sludge!", Monnam(mdef));
		}
		if (mdef->mtame) {
			if (mdef->mhpmax > 1) mdef->mhpmax -= rnd(3);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;

		}

		break;

	    case AD_LAVA:
		if (resists_cold(mdef) && !resists_fire(mdef)) {
			tmp *= 4;
			if (vis) pline("%s is scorched!", Monnam(mdef));
		} else if (!resists_fire(mdef)) {
			tmp *= 2;
			if (vis) pline("%s is severely burned!", Monnam(mdef));
		}
		if (mdef->mtame) {
			mdef->healblock += (20 + (tmp * 20));
			if (mdef->mhpmax > 1) mdef->mhpmax -= 5;
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev > 0) mdef->m_lev--;
		}

		break;

	    case AD_FAKE:
		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}

		break;

	    case AD_WEBS:
		(void) maketrap(mdef->mx, mdef->my, WEB, 0, FALSE);
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap(FALSE);

		break;

	    case AD_TRAP:
		if (t_at(mdef->mx, mdef->my) == 0) (void) maketrap(mdef->mx, mdef->my, rndtrap(), 0, FALSE);
		else makerandomtrap(FALSE);

		break;

	    case AD_CNCL:
		if (rnd(100) > mdef->data->mr) {
			cancelmonsterlite(mdef);
			if (vis) pline("%s is covered in sparkling lights!", Monnam(mdef));
		}
		if (mdef->mtame && !rn2(2)) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			badpeteffect(mdef);
		}

		break;

	    case AD_FEAR:
		if (rnd(100) > mdef->data->mr) {
		     monflee(mdef, rnd(1 + tmp), FALSE, TRUE, FALSE);
			if (vis) pline("%s screams in fear!",Monnam(mdef));
		}
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}

		break;

	    case AD_SANI:
		if (!rn2(10)) {
			mdef->mconf = 1;
			/* STUPID bug where the game displays the same name twice for some inexplicable reason --Amy */
			strcpy(buf, Monnam(mdef));
			if (vis) {
			switch (rnd(4)) {

				case 1:
					pline("%s sees %s chow dead bodies.", buf, mon_nam(magr)); break;
				case 2:
					pline("%s shudders at %s's terrifying %s.", buf, mon_nam(magr), makeplural(mbodypart(magr, EYE)) ); break;
				case 3:
					pline("%s feels sick at entrails caught in %s's tentacles.", buf, mon_nam(magr)); break;
				case 4:
					pline("%s sees maggots breed in the rent %s of %s.", buf, mbodypart(magr, STOMACH), mon_nam(magr)); break;

			}
			}

		}
		if (mdef->mtame && !rn2(5)) badpeteffect(mdef);

		break;

	    case AD_INSA:
		if (rnd(100) > mdef->data->mr) {
		     monflee(mdef, rnd(1 + tmp), FALSE, TRUE, FALSE);
			if (vis) pline("%s screams in fear!",Monnam(mdef));
		}
		if (!magr->mcan && !mdef->mconf && !magr->mspec_used) {
		    if (vis) pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		if (!magr->mcan && canseemon(mdef))
		    pline("%s %s for a moment.", Monnam(mdef), makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;

		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame && !rn2(2)) badpeteffect(mdef);

		break;

	    case AD_DREA:
		if (!mdef->mcanmove) {
			tmp *= 4;
			if (vis) pline("%s's dream is eaten!",Monnam(mdef));
		}

		break;

	    case AD_CONF:
		if (nohit) break;
		/* Since confusing another monster doesn't have a real time
		 * limit, setting spec_used would not really be right (though
		 * we still should check for it).
		 */
		if (!magr->mcan && !mdef->mconf && !magr->mspec_used) {
		    if (vis) pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		break;

	    case AD_SPC2:
		if (nohit) break;
		/* Since confusing another monster doesn't have a real time
		 * limit, setting spec_used would not really be right (though
		 * we still should check for it).
		 */
		if (!magr->mcan && !mdef->mconf && !magr->mspec_used) {
		    if (vis) pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame && !rn2(2)) badpeteffect(mdef);
		break;

	    case AD_FAMN:
		if (mdef->mtame) {
			makedoghungry(mdef, tmp * rnd(50));
			if (vis) pline("%s suddenly looks hungry.", Monnam(mdef));
		}
		break;

	    case AD_WRAT:
		mon_drain_en(mdef, ((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1 + tmp);
		if (mdef->mtame) {
			badpeteffect(mdef);
			if (mdef->mhpmax > 1) mdef->mhpmax -= rnd(5);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}
		break;

	    case AD_MANA:
		if (mdef->mtame) tmp *= 2;
		mon_drain_en(mdef, ((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1 + tmp);
		break;

	    case AD_TECH:
		mon_drain_en(mdef, ((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1 + tmp);
		if (mdef->mtame && !rn2(7)) badpeteffect(mdef);
		break;

	    case AD_MEMO:
		mon_drain_en(mdef, ((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1 + tmp);
		if (mdef->mtame && !rn2(50)) {
			int untamingchance = 10;

			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_PETKEEPING)) {
					default: untamingchance = 10; break;
					case P_BASIC: untamingchance = 9; break;
					case P_SKILLED: untamingchance = 8; break;
					case P_EXPERT: untamingchance = 7; break;
					case P_MASTER: untamingchance = 6; break;
					case P_GRAND_MASTER: untamingchance = 5; break;
					case P_SUPREME_MASTER: untamingchance = 4; break;
				}
			}

			/* Certain monsters aren't even made peaceful. */
			if (!mdef->iswiz && mdef->data != &mons[PM_MEDUSA] &&
				!(mdef->data->mflags3 & M3_COVETOUS) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && mdef->data->mlet == S_DRAGON) && !(mdef->data->geno & G_UNIQ) &&
				((magr->mtame && !rn2(10)) || (mdef->mtame && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4)) )) {
			    if (vis) pline("%s looks calmer.", Monnam(mdef));
			    if (mdef == u.usteed && !mayfalloffsteed())
				dismount_steed(DISMOUNT_THROWN);
			    if (!mdef->mfrenzied && !rn2(3)) mdef->mpeaceful = 1;
			    else mdef->mpeaceful = 0;
			    mdef->mtame = 0;
			    tmp = 0;
			}

		}
		break;

	    case AD_TRAI:
		mon_drain_en(mdef, ((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1 + tmp);
		if (mdef->mtame && !rn2(8)) {
			if (mdef->mhpmax > 1) mdef->mhpmax -= rnd(8);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev > 0) mdef->m_lev--;
		}
		break;

	    case AD_DREN:
		if (nohit) break;
	    	if (resists_magm(mdef)) {
		    if (vis) {
			shieldeff(mdef->mx,mdef->my);
			pline("%s is unaffected.", Monnam(mdef));
		    }
	    	} else {
	    	    mon_drain_en(mdef,
				((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1);
	    	}
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		break;
	    case AD_BLND:
		if (nohit) break;

		if (can_blnd(magr, mdef, mattk->aatyp, (struct obj*)0)) {
		    register unsigned rnd_tmp;

		    if (vis && mdef->mcansee)
			pline("%s is blinded.", Monnam(mdef));
		    rnd_tmp = d((int)mattk->damn, (int)mattk->damd);
		    if ((rnd_tmp += mdef->mblinded) > 127) rnd_tmp = 127;
		    mdef->mblinded = rnd_tmp;
		    mdef->mcansee = 0;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		/*tmp = 0;*/
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		break;
	    case AD_HALU:
		if (!magr->mcan && haseyes(pd) && mdef->mcansee) {
		    if (vis) pline("%s looks %sconfused.",
				    Monnam(mdef), mdef->mconf ? "more " : "");
		    mdef->mconf = 1;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		/*tmp = 0;*/
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame && !rn2(3)) badpeteffect(mdef);
		break;
	    case AD_DEPR:
		if (!magr->mcan && haseyes(pd) && mdef->mcansee) {
		    if (vis) pline("%s looks %sconfused.",
				    Monnam(mdef), mdef->mconf ? "more " : "");
		    mdef->mconf = 1;
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		}
		/*tmp = 0;*/
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 5;
			if (tmp < 1) tmp = 1;
		}
		if (mdef->mtame) {
			badpeteffect(mdef);
			if (mdef->m_lev > 0) mdef->m_lev--;
			if (mdef->m_lev > 0) mdef->m_lev--;
			if (mdef->m_lev == 0) tmp *= 10;
		}
		break;
	    case AD_CURS:
		if (nohit) break;

		if (!magr->mcan && !rn2(10) && (rnd(100) > mdef->data->mr) ) {
		    cancelmonsterlite(mdef); /* cancelled regardless of lifesave */
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (is_were(pd) && pd->mlet != S_HUMAN)
			were_change(mdef);
		    if (pd == &mons[PM_CLAY_GOLEM]) {
			    if (vis) {
				pline("Some writing vanishes from %s head!",
				    s_suffix(mon_nam(mdef)));
				pline("%s is destroyed!", Monnam(mdef));
			    }
			    mondied(mdef);
			    if (mdef->mhp > 0) return 0;
			    else if (mdef->mtame && !vis)
				You(brief_feeling, "strangely sad");
			    return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		    }
		    if (flags.soundok) {
			    if (!vis) You_hear("laughter.");
			    else pline("%s chuckles.", Monnam(magr));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tip bloka l'da smeyetsya tozhe." : "Hoehoehoehoe!");
		    }
		}
		break;
	    case AD_ICUR:
		if (nohit) break;

		if (!magr->mcan && !rn2(10) && (rnd(100) > mdef->data->mr) ) {
		    cancelmonsterlite(mdef);	/* cancelled regardless of lifesave */
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (is_were(pd) && pd->mlet != S_HUMAN)
			were_change(mdef);
		    if (pd == &mons[PM_CLAY_GOLEM]) {
			    if (vis) {
				pline("Some writing vanishes from %s head!",
				    s_suffix(mon_nam(mdef)));
				pline("%s is destroyed!", Monnam(mdef));
			    }
			    mondied(mdef);
			    if (mdef->mhp > 0) return 0;
			    else if (mdef->mtame && !vis)
				You(brief_feeling, "strangely sad");
			    return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		    }
		    if (flags.soundok) {
			    if (!vis) You_hear("laughter.");
			    else pline("%s chuckles.", Monnam(magr));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tip bloka l'da smeyetsya tozhe." : "Hoehoehoehoe!");
		    }
		}
		if (mdef->mtame) {
			badpeteffect(mdef);
			if (mdef->mhpmax > 1) mdef->mhpmax -= rnd(8);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev > 0) mdef->m_lev--;
		}
		break;
	    case AD_NACU:
		if (nohit) break;

		if (!magr->mcan && !rn2(10) && (rnd(100) > mdef->data->mr) ) {
		    cancelmonsterlite(mdef); /* cancelled regardless of lifesave */
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    if (is_were(pd) && pd->mlet != S_HUMAN)
			were_change(mdef);
		    if (pd == &mons[PM_CLAY_GOLEM]) {
			    if (vis) {
				pline("Some writing vanishes from %s head!",
				    s_suffix(mon_nam(mdef)));
				pline("%s is destroyed!", Monnam(mdef));
			    }
			    mondied(mdef);
			    if (mdef->mhp > 0) return 0;
			    else if (mdef->mtame && !vis)
				You(brief_feeling, "strangely sad");
			    return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
		    }
		    if (flags.soundok) {
			    if (!vis) You_hear("laughter.");
			    else pline("%s chuckles.", Monnam(magr));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tip bloka l'da smeyetsya tozhe." : "Hoehoehoehoe!");
		    }
		}
		if (mdef->mtame) allbadpeteffects(mdef);
		break;
	    case AD_SGLD:
	    case AD_DEBT:
		tmp = 0;
#ifndef GOLDOBJ
		if (magr->mcan || !mdef->mgold) break;
		/* technically incorrect; no check for stealing gold from
		 * between mdef's feet...
		 */
		magr->mgold += mdef->mgold;
		mdef->mgold = 0;
#else
                if (magr->mcan) break;
		/* technically incorrect; no check for stealing gold from
		 * between mdef's feet...
		 */
                {
		    struct obj *gold = findgold(mdef->minvent);
		    if (!gold) break;
                    obj_extract_self(gold);
		    add_to_minv(magr, gold);
                }
#endif
		mdef->mstrategy &= ~STRAT_WAITFORU;
		if (vis) {
		    strcpy(buf, Monnam(magr));
		    pline("%s steals some gold from %s.", buf, mon_nam(mdef));
		}
		if (!tele_restrict(magr)) {
		    (void) rloc(magr, FALSE);
		    if (vis && !canspotmon(magr))
			pline("%s suddenly disappears!", buf);
		}
		break;

	    case AD_PAIN:
		if (mdef->mhp > 9) tmp += (mdef->mhp / 10);
		if (vis) pline("%s shrieks in pain!", Monnam(mdef));
		break;

	    case AD_DRLI:
		if (nohit) break;

		if (!cancelled && magr->mtame && !magr->isminion &&
			is_vampire(pa) && mattk->aatyp == AT_BITE &&
			has_blood(pd))
		    EDOG(magr)->hungrytime += ((int)((mdef->data)->cnutrit / 20) + 1);

		if (!cancelled && rn2(2) && !resists_drli(mdef)) {
			if (vis) pline("%s suddenly seems weaker!", Monnam(mdef));
			mdef->mhpmax -= d(2,6);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev == 0) tmp = mdef->mhp;
			else mdef->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		break;
	    case AD_TIME:
		if (nohit) break;

		if (!cancelled && magr->mtame && !magr->isminion &&
			is_vampire(pa) && mattk->aatyp == AT_BITE &&
			has_blood(pd))
		    EDOG(magr)->hungrytime += ((int)((mdef->data)->cnutrit / 20) + 1);

		if (!cancelled && rn2(2) && (!resists_drli(mdef) || mdef->mtame) ) {
			if (vis) pline("%s suddenly seems weaker!", Monnam(mdef));
			mdef->mhpmax -= d(2,6);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev == 0) tmp = mdef->mhp;
			else mdef->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		if (mdef->mtame && !rn2(2)) badpeteffect(mdef);
		break;
	    case AD_DFOO:
		if (nohit) break;

		if (!cancelled && magr->mtame && !magr->isminion &&
			is_vampire(pa) && mattk->aatyp == AT_BITE &&
			has_blood(pd))
		    EDOG(magr)->hungrytime += ((int)((mdef->data)->cnutrit / 20) + 1);

		if (!cancelled && rn2(2) && !resists_drli(mdef)) {
			if (vis) pline("%s suddenly seems weaker!", Monnam(mdef));
			mdef->mhpmax -= d(2,6);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev == 0) tmp = mdef->mhp;
			else mdef->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		if (mdef->mtame) {
			if (mdef->mhpmax > 1) mdef->mhpmax -= rnd(8);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev > 0) mdef->m_lev--;
		}
		break;
	    case AD_WEEP:
		if (nohit) break;

		if (!cancelled && magr->mtame && !magr->isminion &&
			is_vampire(pa) && mattk->aatyp == AT_BITE &&
			has_blood(pd))
		    EDOG(magr)->hungrytime += ((int)((mdef->data)->cnutrit / 20) + 1);

		if (!cancelled && rn2(2) && !resists_drli(mdef)) {
			if (vis) pline("%s suddenly seems weaker!", Monnam(mdef));
			mdef->mhpmax -= d(2,6);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev == 0) tmp = mdef->mhp;
			else mdef->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		if (mdef->mtame && !rn2(50)) mdef->willbebanished = 1;
		break;
	    case AD_VAMP:
		if (nohit) break;

		if (!cancelled && magr->mtame && !magr->isminion &&
			is_vampire(pa) && mattk->aatyp == AT_BITE &&
			has_blood(pd))
		    EDOG(magr)->hungrytime += ((int)((mdef->data)->cnutrit / 20) + 1);

		if ((!cancelled || mdef->mtame) && (rn2(2) || mdef->mtame) && !resists_drli(mdef)) {
			if (vis) pline("%s suddenly seems weaker!", Monnam(mdef));
			mdef->mhpmax -= d(2,6);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev == 0) tmp = mdef->mhp;
			else mdef->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		break;
	    case AD_SSEX:
	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
	    case AD_STTP:
		if (magr->mcan) break;
		/* find an object to steal, non-cursed if magr is tame */
		for (obj = mdef->minvent; obj; obj = obj->nobj)
		    if (!magr->mtame || !obj->cursed)
			break;

		if (obj) {
			char onambuf[BUFSZ], mdefnambuf[BUFSZ];

			/* make a special x_monnam() call that never omits
			   the saddle, and save it for later messages */
			strcpy(mdefnambuf, x_monnam(mdef, ARTICLE_THE, (char *)0, 0, FALSE));

			otmp = obj;
			if (u.usteed == mdef &&
					otmp == which_armor(mdef, W_SADDLE)) {

	/* I took the liberty of making saddles less likely to be stolen, but for a long time that code was only in steal.c
	 * and therefore never actually did anything. Now, your steed should no longer be super vulnerable
	 * to those motherfucker item-stealers! --Amy */
				if (rn2(5) && !issoviet) break;

				/* "You can no longer ride <steed>." */
				dismount_steed(DISMOUNT_POLY);
			}
			obj_extract_self(otmp);
			if (otmp->owornmask) {
				mdef->misc_worn_check &= ~otmp->owornmask;
				if (otmp->owornmask & W_WEP)
				    setmnotwielded(mdef,otmp);
				otmp->owornmask = 0L;
				update_mon_intrinsics(mdef, otmp, FALSE, FALSE);
			}
			/* add_to_minv() might free otmp [if it merges] */
			if (vis)
				strcpy(onambuf, doname(otmp));
			(void) add_to_minv(magr, otmp);
			if (vis) {
				strcpy(buf, Monnam(magr));
				pline("%s steals %s from %s!", buf,
				    onambuf, mdefnambuf);
			}
			possibly_unwield(mdef, FALSE);
			mdef->mstrategy &= ~STRAT_WAITFORU;
			mselftouch(mdef, (const char *)0, FALSE);
			if (mdef->mhp <= 0)
				return (MM_DEF_DIED | (grow_up(magr,mdef) ?
							0 : MM_AGR_DIED));
			if (magr->data->mlet == S_NYMPH &&
			    !tele_restrict(magr) && !rn2(5) ) {
			    (void) rloc(magr, FALSE);
			    if (vis && !canspotmon(magr))
				pline("%s suddenly disappears!", buf);
			}
		}

		if (mdef->mtame && atttyp == AD_STTP) {
			badpeteffect(mdef);
			badpeteffect(mdef);
		}

		tmp = 0;
		break;
	    case AD_DRST:
	    case AD_DRDX:
	    case AD_DRCO:
	    case AD_POIS:
	    case AD_DEBU:
	    case AD_WISD:
	    case AD_DRCH:
		if (nohit) break;

		if (!cancelled && !rn2(8)) {
		    if (vis)
			pline("%s %s was poisoned!", s_suffix(Monnam(magr)),
			      mpoisons_subj(magr, mattk));
		    if (resists_poison(mdef) && !player_will_pierce_resistance()) {
			if (vis)
			    pline_The("poison doesn't seem to affect %s.",
				mon_nam(mdef));
		    } else {
			if (rn2(100) || resists_poison(mdef)) tmp += rn1(10,6);
			else {
			    if (vis) pline_The("poison was deadly...");
			    tmp = mdef->mhp;
			}
		    }
		}
		break;
	    case AD_STAT:
		if (nohit) break;

		if (!cancelled && !rn2(8)) {
		    if (vis)
			pline("%s %s was poisoned!", s_suffix(Monnam(magr)),
			      mpoisons_subj(magr, mattk));
		    if (resists_poison(mdef) && !player_will_pierce_resistance()) {
			if (vis)
			    pline_The("poison doesn't seem to affect %s.",
				mon_nam(mdef));
		    } else {
			if (rn2(100) || resists_poison(mdef)) tmp += rn1(10,6);
			else {
			    if (vis) pline_The("poison was deadly...");
			    tmp = mdef->mhp;
			}
		    }
		}
		if (mdef->mtame) {
			badpeteffect(mdef);
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}
		break;

	    case AD_VENO:
		if (nohit) break;

		if (!cancelled && !rn2(3)) {
		    if (resists_poison(mdef) && !player_will_pierce_resistance()) {
			if (vis)
			    pline_The("poison doesn't seem to affect %s.",
				mon_nam(mdef));
		    } else {
			if (vis) pline("%s is badly poisoned!", Monnam(mdef));
			if (rn2(10) || resists_poison(mdef)) tmp += rn1(20,12);
			else {
			    if (vis) pline_The("poison was deadly...");
			    tmp = mdef->mhp;
			}
		    }
		}
		if (mdef->mtame) {
			badpeteffect(mdef);
			badpeteffect(mdef);
			if (mdef->mhpmax > 1) mdef->mhpmax -= rnd(8);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;

		}
		break;

	    case AD_DRIN:
		if (notonhead || !has_head(pd)) {
		    if (vis) pline("%s doesn't seem harmed.", Monnam(mdef));
		    /* Not clear what to do for green slimes */
		    tmp = 0;
		    break;
		}
		if ((mdef->misc_worn_check & W_ARMH) && rn2(8)) {
		    if (vis) {
			strcpy(buf, s_suffix(Monnam(mdef)));
			pline("%s helmet blocks %s attack to %s head.",
				buf, s_suffix(mon_nam(magr)),
				mhis(mdef));
		    }
		    break;
		}
		if (vis) pline("%s brain is eaten!", s_suffix(Monnam(mdef)));
		if (mindless(pd) || mdef->egotype_undead ) {
		    if (vis) pline("%s doesn't notice.", Monnam(mdef));
		    break;
		}
		tmp += rnd(10); /* fakery, since monsters lack INT scores */
		if (magr->mtame && !magr->isminion) {
		    EDOG(magr)->hungrytime += rnd(60);
		    magr->mconf = 0;
		}
		if (tmp >= mdef->mhp && vis)
		    pline("%s last thought fades away...",
			          s_suffix(Monnam(mdef)));
		break;
	    case AD_SLIM: /* no easy sliming Death or Famine --Amy */
		if (cancelled || (rn2(100) < mdef->data->mr) ) break;   /* physical damage only */
		if (!rn2(400) && !flaming(mdef->data) &&
				!slime_on_touch(mdef->data) ) {
		    if (newcham(mdef, &mons[PM_GREEN_SLIME], FALSE, vis)) {
			mdef->oldmonnm = PM_GREEN_SLIME;
			(void) stop_timer(UNPOLY_MON, (void *) mdef);
		    }
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    tmp = 0;
		}
	    case AD_LITT:
		if (cancelled || (rn2(100) < mdef->data->mr) ) break;   /* physical damage only */
		if (!rn2(mdef->mtame ? 40 : 400) && !flaming(mdef->data) &&
				!slime_on_touch(mdef->data) ) {
		    if (newcham(mdef, &mons[PM_GREEN_SLIME], FALSE, vis)) {
			mdef->oldmonnm = PM_GREEN_SLIME;
			(void) stop_timer(UNPOLY_MON, (void *) mdef);
		    }
		    mdef->mstrategy &= ~STRAT_WAITFORU;
		    tmp = 0;
		}
		if (mdef->mtame) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (!rn2(3)) badpeteffect(mdef);
		}
		break;
	    case AD_DISN:
		if (mdef->mtame && !rn2(10) && !resists_disint(mdef)) {
			if (vis) pline("%s is hit by disintegration!", Monnam(mdef));
			struct obj *otmp2;
			otmp2 = (struct obj *)0;
			if (mdef->misc_worn_check & W_ARMS) {
			otmp2 = which_armor(mdef, W_ARMS);
		    } else if (mdef->misc_worn_check & W_ARMC) {
			otmp2 = which_armor(mdef, W_ARMC);
		    } else if (mdef->misc_worn_check & W_ARM) {
			otmp2 = which_armor(mdef, W_ARM);
		    } else if (mdef->misc_worn_check & W_ARMU) {
			otmp2 = which_armor(mdef, W_ARMU);
		    } else {
			/* no body armor, victim dies; destroy cloak
			   and shirt now in case target gets life-saved */
			tmp += 10000;
		    }
			if (otmp2 && (otmp2 != (struct obj *)0) ) m_useup(mdef, otmp2);

		}
		break;
	    case AD_SHRD:
		if (mdef->mtame) {
			if (vis) pline("%s is being shredded!", Monnam(mdef));
			struct obj *otmp2;
			otmp2 = (struct obj *)0;

			if (rn2(3)) {
				badpeteffect(mdef);
				badpeteffect(mdef);
			} else {

				if (mdef->misc_worn_check & W_ARMS) {
				otmp2 = which_armor(mdef, W_ARMS);
			    } else if (mdef->misc_worn_check & W_ARMC) {
				otmp2 = which_armor(mdef, W_ARMC);
			    } else if (mdef->misc_worn_check & W_ARM) {
				otmp2 = which_armor(mdef, W_ARM);
			    } else if (mdef->misc_worn_check & W_ARMU) {
				otmp2 = which_armor(mdef, W_ARMU);
			    } else {
				/* no body armor, victim dies; destroy cloak
				   and shirt now in case target gets life-saved */
				tmp += 10000;
			    }
				if (otmp2 && (otmp2 != (struct obj *)0) ) m_useup(mdef, otmp2);
			}

		}
		break;
	    case AD_WET:
		if (mdef->mtame) {
			if (vis) pline("%s is doused with water!", Monnam(mdef));
			if (rn2(3)) mdef->mconf = 1;
			if (!rn2(3)) mon_adjust_speed(mdef, -1, (struct obj *)0);
			if (!rn2(5)) badpeteffect(mdef);
		}
		break;
	    case AD_AXUS:
		if (mdef->mtame) tmp *= rnd(4);
		break;
	    case AD_SKIL:
		if (mdef->mtame && !rn2(4)) badpeteffect(mdef);
		break;
	    case AD_TDRA:
		if (mdef->mtame && !rn2(4)) badpeteffect(mdef);
		break;
	    case AD_BLAS:
		if (mdef->mtame && !rn2(5)) badpeteffect(mdef);
		break;
	    case AD_SUCK:
		if (mdef->mtame) {
			if (vis) pline("%s is sucked by a vacuum cleaner!", Monnam(mdef));
			badpeteffect(mdef);
			badpeteffect(mdef);
			badpeteffect(mdef);
			if (!rn2(3)) {
				if (mdef->mhpmax > 1) mdef->mhpmax--;
				if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			}
		}
		break;
	    case AD_VAPO:
		if (mdef->mtame) tmp *= 3;
		if (mdef->mtame && !rn2(10) && !resists_disint(mdef)) {
			if (vis) pline("%s is hit by vaporization!", Monnam(mdef));
			struct obj *otmp2;
			otmp2 = (struct obj *)0;
			if (mdef->misc_worn_check & W_ARMS) {
			otmp2 = which_armor(mdef, W_ARMS);
		    } else if (mdef->misc_worn_check & W_ARMC) {
			otmp2 = which_armor(mdef, W_ARMC);
		    } else if (mdef->misc_worn_check & W_ARM) {
			otmp2 = which_armor(mdef, W_ARM);
		    } else if (mdef->misc_worn_check & W_ARMU) {
			otmp2 = which_armor(mdef, W_ARMU);
		    } else {
			/* no body armor, victim dies; destroy cloak
			   and shirt now in case target gets life-saved */
			tmp += 10000;
		    }
			if (otmp2 && (otmp2 != (struct obj *)0) ) m_useup(mdef, otmp2);

		}
		break;
	    case AD_DISE:
		if (mdef->mtame && !rn2(3)) {
			if (vis) pline("%s looks sick.", Monnam(mdef));
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}
		break;
	    case AD_VOMT:
		if (mdef->mtame && !rn2(4)) {
			if (vis) pline("%s looks sick.", Monnam(mdef));
			if (!rn2(3)) {
				if (mdef->mhpmax > 1) mdef->mhpmax--;
				if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			}
			if (!rn2(3)) mdef->mconf = TRUE;
			if (!rn2(3)) mdef->mstun = TRUE;
			if (!rn2(5) && mdef->mcanmove) {
				mdef->mcanmove = 0;
				mdef->mfrozen = 5;
				mdef->mstrategy &= ~STRAT_WAITFORU;
			}
		}
		break;
	    case AD_NGRA:
		if (mdef->mtame && !rn2(20)) badpeteffect(mdef);
		break;
	    case AD_WTHR:
		if (mdef->mtame) {
			if (vis) pline("%s withers!", Monnam(mdef));
			if (mdef->mhpmax > 1) mdef->mhpmax -= rnd(8);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev > 0) mdef->m_lev--;
		}
		break;
	    case AD_LUCK:
		if (mdef->mtame && !rn2(5)) badpeteffect(mdef);
		break;
	    case AD_LETH:
		if (mdef->mtame) {
			if (vis) pline("%s seems oblivious!", Monnam(mdef));
			mdef->mhpmax -= 8;
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev > 0) mdef->m_lev--;
			badpeteffect(mdef);
			badpeteffect(mdef);
			badpeteffect(mdef);
		}
		break;
	    case AD_NPRO:
	    case AD_INVE:
		if (mdef->mtame) {
			if (vis) pline("%s seems less protected!", Monnam(mdef));
			mdef->mhpmax -= 8;
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (mdef->m_lev > 0) mdef->m_lev--;

		}
		break;
	    case AD_DISP:
	    case AD_TPTO:
	    case AD_SWAP:
		if (mdef->mtame && !rn2(3) && mdef->mcanmove) {
			mdef->mcanmove = 0;
			mdef->mfrozen = 2;
			mdef->mstrategy &= ~STRAT_WAITFORU;
			if (vis) pline("%s is stopped in %s tracks.", Monnam(mdef), mhis(mdef));
		}
		break;
	    case AD_PEST:
		if (mdef->mtame) {
			if (vis) pline("%s looks very sick.", Monnam(mdef));
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
		}
		break;
	    case AD_STCK:
		if (cancelled) tmp = 0;
		break;
	    case AD_WRAP: /* monsters cannot grab one another, it's too hard */
		if (magr->mcan) tmp = 0;
		if (mdef->mtame && !rn2(10)) {
			badpeteffect(mdef);
			if (is_drowningpool(magr->mx, magr->my)) tmp *= 10;
		}
		break;
	    case AD_ENCH:
	    case AD_UNPR:
		/* There's no msomearmor() function, so just do damage */
	     /* if (cancelled) break; */
		if (mdef->mtame && !rn2(3)) {
			if (mdef->mhpmax > 1) mdef->mhpmax--;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			badpeteffect(mdef);
		}
		break;
	    case AD_NIVE:
		if (mdef->mtame) {
			if (mdef->mhpmax > (Role_if(PM_ZOOKEEPER) ? 480 : 240)) {
				int reduction = rnd(mdef->mhpmax / 10);
				if (reduction < 1) reduction = 1; /* shouldn't happen */
				mdef->mhpmax -= reduction;
				if (mdef->mhpmax < 1) mdef->mhpmax = 1;
				if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
				if (vis) pline("%s is in pain!", Monnam(mdef));
			} else if (mdef->mhpmax > (Role_if(PM_ZOOKEEPER) ? 320 : 160)) {
				mdef->mhpmax--;
				if (mdef->mhpmax < 1) mdef->mhpmax = 1;
				if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
				if (vis) pline("%s seems to be hurt!", Monnam(mdef));
			}
		}
		break;
	    case AD_DEST:
		if (mdef->mtame) tmp *= 10;
		break;
	    case AD_SIN:
		if (mdef->mtame && !rn2(3)) {
			badpeteffect(mdef);
			if (mdef->mhpmax > 1) mdef->mhpmax -= 8;
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;

			if (!rn2(10)) {
				int untamingchance = 10;

				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_PETKEEPING)) {
						default: untamingchance = 10; break;
						case P_BASIC: untamingchance = 9; break;
						case P_SKILLED: untamingchance = 8; break;
						case P_EXPERT: untamingchance = 7; break;
						case P_MASTER: untamingchance = 6; break;
						case P_GRAND_MASTER: untamingchance = 5; break;
						case P_SUPREME_MASTER: untamingchance = 4; break;
					}
				}

				if (!mdef->mfrenzied && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && mdef->data->mlet == S_DRAGON) && (!mdef->mtame || (mdef->mtame <= rnd(21) && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4) ) ) ) {
					mdef->mpeaceful = mdef->mtame = 0;
					mdef->mfrenzied = 1;
					if (vis) pline("%s enters a state of frenzy!", Monnam(mdef));
				}
			}

		}
		break;
	    case AD_ALIN:
		if (mdef->mtame && !rn2(10)) badpeteffect(mdef);
		break;
	    case AD_NGEN:
		if (mdef->mtame) {
			if (mdef->mhpmax > 1) mdef->mhpmax -= 5;
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			badpeteffect(mdef);
			badpeteffect(mdef);
		}
		break;
	    case AD_IDAM:
		if (mdef->mtame) {
			if (mdef->mhpmax > 1) mdef->mhpmax -= rnd(3);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			badpeteffect(mdef);
		}
		break;
	    case AD_ANTI:
		if (mdef->mtame) {
			if (mdef->mhpmax > 1) mdef->mhpmax -= rnd(20);
			if (mdef->mhpmax < 1) mdef->mhpmax = 1;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			badpeteffect(mdef);
			badpeteffect(mdef);
			badpeteffect(mdef);
			badpeteffect(mdef);
			badpeteffect(mdef);
		}
		break;
	    case AD_POLY:
		if (!magr->mcan && tmp < mdef->mhp) {
		    if (resists_magm(mdef) || (rn2(100) < mdef->data->mr) ) { /* no easy taming Death or Famine! --Amy */
			/* magic resistance protects from polymorph traps, so
			 * make it guard against involuntary polymorph attacks
			 * too... */
			if (vis) shieldeff(mdef->mx, mdef->my);
			break;
		    }
#if 0
		    if (!rn2(25) || !mon_poly(mdef)) {
			if (vis)
			    pline("%s shudders!", Monnam(mdef));
			/* no corpse after system shock */
			tmp = rnd(30);
		    } else
#endif
		    (void) mon_poly(mdef, FALSE,
			    "%s undergoes a freakish metamorphosis!");
		}
		break;

	    case AD_CHAO:
		if (!magr->mcan && tmp < mdef->mhp) {
		    if (resists_magm(mdef) || (rn2(100) < mdef->data->mr) ) { /* no easy taming Death or Famine! --Amy */
			/* magic resistance protects from polymorph traps, so
			 * make it guard against involuntary polymorph attacks
			 * too... */
			if (vis) shieldeff(mdef->mx, mdef->my);
			break;
		    }
		    (void) mon_poly(mdef, FALSE,
			    "%s undergoes a freakish metamorphosis!");
		}
		if ((tmp > 0) && (mdef && mdef->mhpmax > 1)) {
			mdef->mhpmax--;
			if (vis) pline("%s feels bad!", Monnam(mdef));
		}
		if (mdef->mtame) badpeteffect(mdef);

		break;

	    case AD_CALM:	/* KMH -- koala attack */

		{
		int untamingchance = 10;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_PETKEEPING)) {
				default: untamingchance = 10; break;
				case P_BASIC: untamingchance = 9; break;
				case P_SKILLED: untamingchance = 8; break;
				case P_EXPERT: untamingchance = 7; break;
				case P_MASTER: untamingchance = 6; break;
				case P_GRAND_MASTER: untamingchance = 5; break;
				case P_SUPREME_MASTER: untamingchance = 4; break;
			}
		}

		/* Certain monsters aren't even made peaceful. */
		if (!mdef->iswiz && mdef->data != &mons[PM_MEDUSA] &&
			!(mdef->data->mflags3 & M3_COVETOUS) &&
			!(mdef->data->geno & G_UNIQ) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && mdef->data->mlet == S_DRAGON) &&
			((magr->mtame && !rn2(10)) || (mdef->mtame && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4)) )) {
		    if (vis) pline("%s looks calmer.", Monnam(mdef));
		    if (mdef == u.usteed && !mayfalloffsteed())
			dismount_steed(DISMOUNT_THROWN);
		    if (!mdef->mfrenzied) mdef->mpeaceful = 1;
		    mdef->mtame = 0;
		    tmp = 0;
		}

		}

		break;
	    case AD_FREN:

		{
		int untamingchance = 10;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_PETKEEPING)) {
				default: untamingchance = 10; break;
				case P_BASIC: untamingchance = 9; break;
				case P_SKILLED: untamingchance = 8; break;
				case P_EXPERT: untamingchance = 7; break;
				case P_MASTER: untamingchance = 6; break;
				case P_GRAND_MASTER: untamingchance = 5; break;
				case P_SUPREME_MASTER: untamingchance = 4; break;
			}
		}

		if (!mdef->mfrenzied && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && mdef->data->mlet == S_DRAGON) && (!mdef->mtame || (mdef->mtame <= rnd(21) && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4) ) ) ) {
			mdef->mpeaceful = mdef->mtame = 0;
			mdef->mfrenzied = 1;
		    if (vis) pline("%s enters a state of frenzy!", Monnam(mdef));
		}

		}

		break;
	    default:	/*tmp = 0;*/
		if (mattk->aatyp == AT_EXPL && tmp > 1) {
			tmp /= 20;
			if (tmp < 1) tmp = 1;
		}
			break; /* necessary change to make pets more viable --Amy */
	}
	if(!tmp) return(MM_MISS);

	/* STEPHEN WHITE'S NEW CODE */
	if (objenchant < canhitmon && vis && nohit) {
			strcpy(buf, Monnam(magr));
			pline("%s doesn't seem to harm %s.", buf,
								mon_nam(mdef));
		return(MM_HIT);
	}
	/* WAC -- Caveman Primal Roar ability */
	if (magr->mtame != 0 && tech_inuse(T_PRIMAL_ROAR)) {
		tmp *= 2; /* Double Damage! */
	}

	if (Race_if(PM_BOVER) && u.usteed && (mdef == u.usteed) && tmp > 1) tmp /= 2;
	if (Race_if(PM_CARTHAGE) && u.usteed && (mdef == u.usteed) && (mcalcmove(u.usteed) < 12) && tmp > 1) tmp /= 2;

	if((mdef->mhp -= tmp) < 1) {
	    if (m_at(mdef->mx, mdef->my) == magr) {  /* see gulpmm() */
		remove_monster(mdef->mx, mdef->my);
		mdef->mhp = 1;	/* otherwise place_monster will complain */
		place_monster(mdef, mdef->mx, mdef->my);
		mdef->mhp = 0;
	    }
	    /* get experience from spell creatures */
	    if (magr->uexp) mon_xkilled(mdef, "", (int)mattk->adtyp);
	    else monkilled(mdef, "", (int)mattk->adtyp);

	    if (mdef->mhp > 0) return 0; /* mdef lifesaved */

	    if (magr->mhp > 0 && magr->mtame) {
		use_skill(P_PETKEEPING,1);
		/* note by Amy: it's dumb if inediate pets can never gain tameness! have a (really low) chance */
		if (!rn2(100)) {
			if (magr->mtame < 20 && !FemtrapActiveAntje) magr->mtame++;
		}
		if (uamul && uamul->oartifact == ART_MEDI_LEASH) {
			if (magr->mtame < 20 && !FemtrapActiveAntje) magr->mtame++;
		}
	    }

	    if (mattk->adtyp == AD_DGST) {
		/* various checks similar to dog_eat and meatobj.
		 * after monkilled() to provide better message ordering */
		if (mdef->cham != CHAM_ORDINARY) {
		    (void) newcham(magr, (struct permonst *)0, FALSE, TRUE);
		} else if (mdef->data == &mons[PM_GREEN_SLIME]) {
		    (void) newcham(magr, &mons[PM_GREEN_SLIME], FALSE, TRUE);
		} else if (mdef->data == &mons[PM_WRAITH]) {
		    (void) grow_up(magr, (struct monst *)0);
		    /* don't grow up twice */
		    return (MM_DEF_DIED | (magr->mhp > 0 ? 0 : MM_AGR_DIED));
		} else if (mdef->data == &mons[PM_NURSE]) {
		    magr->mhp = magr->mhpmax;
		}
	    }

	    return (MM_DEF_DIED |
		    ((magr->mhp > 0 && grow_up(magr,mdef)) ? 0 : MM_AGR_DIED));
	}
	if (mdef->mhp > 0) monster_pain(mdef);

	return(MM_HIT);
}

#endif /* OVLB */


#ifdef OVL0

/* monster has no active attack: means all of them are AT_NONE */
int
noattacks(ptr)			/* returns 1 if monster doesn't attack */
	struct	permonst *ptr;
{
	int i;

	for(i = 0; i < NATTK; i++)
		if(ptr->mattk[i].aatyp) return(0);

	return(1);
}

/* for polymorphed player, or symbiote participating in combat: non-melee attacks still count as "no attacks" --Amy */
int
noattacks_plr(ptr)			/* returns 1 if monster doesn't attack */
	struct	permonst *ptr;
{
	int i;

	for(i = 0; i < NATTK; i++)
		if(ptr->mattk[i].aatyp) {
			if (ptr->mattk[i].aatyp >= AT_CLAW && ptr->mattk[i].aatyp <= AT_LASH) {
				return(0);
			}
			if (ptr->mattk[i].aatyp == AT_ENGL || ptr->mattk[i].aatyp == AT_EXPL || ptr->mattk[i].aatyp == AT_MULTIPLY) {
				return(0);
			}
			if (ptr->mattk[i].aatyp >= AT_GAZE && ptr->mattk[i].aatyp <= AT_BEAM) {
				return(0);
			}
			if (ptr->mattk[i].aatyp == AT_WEAP || ptr->mattk[i].aatyp == AT_MAGC) {
				return(0);
			}
		}

	return(1);
}

/* `mon' is hit by a sleep attack; return 1 if it's affected, 0 otherwise */
int
sleep_monst(mon, amt, how)
struct monst *mon;
int amt, how;
{
	if (resists_sleep(mon) ||
		(how >= 0 && resist(mon, (char)how, 0, NOTELL))) {
	    if (resists_sleep(mon) && canseemon(mon)) pline("%s is immune to sleep!", Monnam(mon));
	    shieldeff(mon->mx, mon->my);
	} else if (mon->mcanmove) {
	    amt += (int) mon->mfrozen;
	    if (amt > 0) {	/* sleep for N turns */
		mon->mcanmove = 0;
		mon->mfrozen = min(amt, 127);
		mon->masleep = 1;
	    } else {		/* sleep until awakened */
		mon->msleeping = 1;
	    }
	    return 1;
	}
	return 0;
}

/* sleeping grabber releases, engulfer doesn't; don't use for paralysis! */
void
slept_monst(mon)
struct monst *mon;
{
	if ((mon->msleeping || !mon->mcanmove) && mon == u.ustuck &&
		!sticks(youmonst.data) && !(uactivesymbiosis && sticks(&mons[u.usymbiote.mnum]) ) && !u.uswallow) {
	    pline("%s grip relaxes.", s_suffix(Monnam(mon)));
	    unstuck(mon);
	}
}

#endif /* OVL0 */
#ifdef OVLB

STATIC_OVL void
mrustm(magr, mdef, obj)
register struct monst *magr, *mdef;
register struct obj *obj;
{
	boolean is_acid;

	if (DEADMONSTER(magr)) return;
	if (DEADMONSTER(mdef)) return;

	if (!magr || !mdef || !obj) return; /* just in case */

	/* It is just teh uber cheat0r that non-passive rusting attacks still cause the attacking monster's shit to rust. */
	if (attackdamagetype(mdef->data, AT_NONE, AD_CORR)) {
	    is_acid = TRUE;
	} else if (attackdamagetype(mdef->data, AT_NONE, AD_RUST)) {
	    is_acid = FALSE;
	} else if (attackdamagetype(mdef->data, AT_RATH, AD_CORR)) {
	    is_acid = TRUE;
	} else if (attackdamagetype(mdef->data, AT_RATH, AD_RUST)) {
	    is_acid = FALSE;

	/* In Soviet Russia, the Amy is considered the antichrist and everything she does must be bad. She can go ahead and
	 * do obvious bug fixes that every sane person would immediately recognize as such, but the type of ice block goes
	 * ahead and says 'she made this change, therefore it must be bad REVERTREVERTREVERTREVERTREVERTREVERTREVERT'. */
	} else if (issoviet) {
		if (dmgtype(mdef->data, AD_CORR))
		    is_acid = TRUE;
		else if (dmgtype(mdef->data, AD_RUST))
		    is_acid = FALSE;
		else return;
	} else
	    return;

	if (!mdef->mcan &&
	    (is_acid ? (is_corrodeable(obj) && !(objects[obj->otyp].oc_material == MT_GREEN_STEEL && rn2(2)) ) : (is_rustprone(obj) && !(objects[obj->otyp].oc_material == MT_COBALT && rn2(2) ) && !(objects[obj->otyp].oc_material == MT_BRONZE && rn2(2) ) ) ) && !stack_too_big(obj) &&
	    (is_acid ? obj->oeroded2 : obj->oeroded) < MAX_ERODE) {
		if (obj->greased || (obj->oartifact && rn2(4)) || obj->oerodeproof || (obj->blessed && rn2(3))) {
		    if (cansee(mdef->mx, mdef->my) && flags.verbose)
			pline("%s weapon is not affected.",
			                 s_suffix(Monnam(magr)));
		    if (obj->greased && !rn2(2)) obj->greased -= 1;
		} else {
		    if (cansee(mdef->mx, mdef->my)) {
			pline("%s %s%s!", s_suffix(Monnam(magr)),
			    aobjnam(obj, (is_acid ? "corrode" : "rust")),
			    (is_acid ? obj->oeroded2 : obj->oeroded)
				? " further" : "");
		    }
		    if (is_acid) obj->oeroded2++;
		    else obj->oeroded++;
		}
	}
}

STATIC_OVL void
mswingsm(magr, mdef, otemp)
register struct monst *magr, *mdef;
register struct obj *otemp;
{
	if (DEADMONSTER(magr)) return;
	if (DEADMONSTER(mdef)) return;

	char buf[BUFSZ];
	if (!flags.verbose || Blind || !mon_visible(magr)) return;
	strcpy(buf, mon_nam(mdef));
	pline("%s %s %s %s at %s.", Monnam(magr),
	      (objects[otemp->otyp].oc_dir & PIERCE) ? "thrusts" : "swings",
	      mhis(magr), singular(otemp, xname), buf);
}

/*
 * Passive responses by defenders.  Does not replicate responses already
 * handled above.  Returns same values as mattackm.
 */
STATIC_OVL int
passivemm(magr,mdef,mhit,mdead,attnumber)
register struct monst *magr, *mdef;
boolean mhit;
int mdead;
int attnumber;
{
	register struct permonst *mddat = mdef->data;
	register struct permonst *madat = magr->data;
	char buf[BUFSZ];
	int i, tmp;

	if (DEADMONSTER(magr)) return 0;
	/* mdef may be dead --Amy */

	int atttypB;

	if ( (mdef->mtame || !(attacktype(mdef->data, AT_RATH))) && !monnear(magr, mdef->mx, mdef->my)) return 0;

	for(i = 0; ; i++) {
	    if(i >= NATTK) return (mdead | mhit); /* no passive attacks */
	    if((i == attnumber) && (mddat->mattk[i].aatyp == AT_NONE || mddat->mattk[i].aatyp == AT_RATH)) break;
	}

	if (mddat->mattk[i].damn)
	    tmp = d((int)mddat->mattk[i].damn,
				    (int)mddat->mattk[i].damd);
	else if(mddat->mattk[i].damd)
	    tmp = d((int)mddat->mlevel+1, (int)mddat->mattk[i].damd);
	else
	    tmp = 0;

	atttypB = mddat->mattk[i].adtyp;

	if (magr->mtame) {

		if (atttypB == AD_RBRE) {
			while (atttypB == AD_ENDS || atttypB == AD_RBRE || atttypB == AD_WERE) {
				atttypB = randattack();
			}
		}

		if (atttypB == AD_DAMA) {
			atttypB = randomdamageattack();
		}

		if (atttypB == AD_ILLU) {
			atttypB = randomillusionattack();
		}

		if (atttypB == AD_THIE) {
			atttypB = randomthievingattack();
		}

		if (atttypB == AD_RNG) {
			while (atttypB == AD_ENDS || atttypB == AD_RNG || atttypB == AD_WERE) {
				atttypB = rn2(AD_ENDS); }
		}

		if (atttypB == AD_PART) atttypB = u.adpartattack;

		if (atttypB == AD_MIDI) {
			atttypB = mdef->m_id;
			if (atttypB < 0) atttypB *= -1;
			while (atttypB >= AD_ENDS) atttypB -= AD_ENDS;
			if (!(atttypB >= AD_PHYS && atttypB < AD_ENDS)) atttypB = AD_PHYS; /* fail safe --Amy */
			if (atttypB == AD_WERE) atttypB = AD_PHYS;
		}
	}

	/* These affect the enemy even if defender killed */
	switch(atttypB) {
	    case AD_ACID:
		if (mhit && !rn2(2)) {
		    strcpy(buf, Monnam(magr));
		    if(canseemon(magr))
			pline("%s is splashed by %s acid!",
			      buf, s_suffix(mon_nam(mdef)));
		    if (resists_acid(magr) && !player_will_pierce_resistance()) {
			if(canseemon(magr))
			    pline("%s is not affected.", Monnam(magr));
			tmp = 0;
		    }
		} else tmp = 0;
		break;
		case AD_MAGM:
	    /* wrath of gods for attacking Oracle */
	    if(resists_magm(magr)) {
		if(canseemon(magr)) {
		shieldeff(magr->mx, magr->my);
		pline("A hail of magic missiles narrowly misses %s!",mon_nam(magr));
		}
	    } else {
		if(canseemon(magr))
			pline(magr->data == &mons[PM_WOODCHUCK] ? "ZOT!" :
			"%s is hit by magic missiles appearing from thin air!",Monnam(magr));
		break;
	    }
	    break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		if (mhit && !mdef->mcan && otmp) {
			drain_item(otmp);
		    /* No message */
		}
		if (magr->mtame && !rn2(3)) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			badpeteffect(magr);
		}
		break;
	    case AD_UNPR:
		if (magr->mtame && !rn2(3)) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			badpeteffect(magr);
		}
		break;
	    case AD_NIVE:
		if (magr->mtame) {
			if (magr->mhpmax > (Role_if(PM_ZOOKEEPER) ? 480 : 240)) {
				int reduction = rnd(magr->mhpmax / 10);
				if (reduction < 1) reduction = 1; /* shouldn't happen */
				magr->mhpmax -= reduction;
				if (magr->mhpmax < 1) magr->mhpmax = 1;
				if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
				if (canseemon(magr)) pline("%s is in pain!", Monnam(magr));
			} else if (magr->mhpmax > (Role_if(PM_ZOOKEEPER) ? 320 : 160)) {
				magr->mhpmax--;
				if (magr->mhpmax < 1) magr->mhpmax = 1;
				if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
				if (canseemon(magr)) pline("%s seems to be hurt!", Monnam(magr));
			}
		}
		break;

	    case AD_NGEN:
		if (mhit && !mdef->mcan && otmp) {
			drain_item(otmp);
		    /* No message */
		}
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax -= 5;
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			badpeteffect(magr);
			badpeteffect(magr);

		}
		break;
	    default:
		break;
	}
	if ((mdead && !magr->mtame) || mdef->mcan) return (mdead|mhit);

	/* These affect the enemy only if defender is still alive */
	if (rn2(3)) switch(atttypB) {
	    case AD_PLYS: /* Floating eye */

		{
		int parlyzdur = rnd(magr->mtame ? 4 : 10);
		if (!rn2(4)) parlyzdur = tmp;
		if (magr->mtame) {
			if (parlyzdur > 3) {
				parlyzdur = rnd(parlyzdur);
				if (parlyzdur < 3) parlyzdur = 3;
			}
			if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
		}
		if (parlyzdur > 127) parlyzdur = 127;

		if (dmgtype(magr->data, AD_PLYS)) return 1;

		if (tmp > 127) tmp = 127;
		if (mddat == &mons[PM_FLOATING_EYE]) {
		    /*if (!rn2(4)) tmp = 127;*/
		    if (magr->mcansee && (!rn2(4) || !(mdef->mtame)) && haseyes(madat) && mdef->mcansee && !mdef->minvisreal &&
			(perceives(madat) || !mdef->minvis)) {
			sprintf(buf, "%s gaze is reflected by %%s %%s.",
				s_suffix(mon_nam(mdef)));
			if (mon_reflects(magr,
					 canseemon(magr) ? buf : (char *)0))
				return(mdead|mhit);
			strcpy(buf, Monnam(magr));
			if(canseemon(magr))
			    pline("%s is frozen by %s gaze!",
				  buf, s_suffix(mon_nam(mdef)));
			magr->mcanmove = 0;
			magr->mfrozen = parlyzdur;
			return (mdead|mhit);
		    }
		} else if (!rn2(4) || !(mdef->mtame)) { /* gelatinous cube */
		    strcpy(buf, Monnam(magr));
		    if(canseemon(magr))
			pline("%s is frozen by %s.", buf, mon_nam(mdef));
		    magr->mcanmove = 0;
		    magr->mfrozen = parlyzdur;
		    return (mdead|mhit);
		}

		}
		return 1;
	    case AD_COLD:
		if (resists_cold(magr) && !player_will_pierce_resistance()) {
		    if (canseemon(magr)) {
			pline("%s is mildly chilly.", Monnam(magr));
			golemeffects(magr, AD_COLD, tmp);
		    }
		    tmp = 0;
		    break;
		}
		if(canseemon(magr))
		    pline("%s is suddenly very cold!", Monnam(magr));
		if (!mdead) {
			mdef->mhp += tmp / 2;
			if (mdef->mhpmax < mdef->mhp) mdef->mhpmax = mdef->mhp;
			if (mdef->mhpmax > ((int) (mdef->m_lev+1) * 8) && !rn2(50) ) /* split much less often --Amy */
			(void)split_mon(mdef, magr);
		}
		break;
	    case AD_WERE:
		if (magr->mtame && !rn2(3)) {

		int untamingchance = 10;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_PETKEEPING)) {
				default: untamingchance = 10; break;
				case P_BASIC: untamingchance = 9; break;
				case P_SKILLED: untamingchance = 8; break;
				case P_EXPERT: untamingchance = 7; break;
				case P_MASTER: untamingchance = 6; break;
				case P_GRAND_MASTER: untamingchance = 5; break;
				case P_SUPREME_MASTER: untamingchance = 4; break;
			}
		}

		/* Certain monsters aren't even made peaceful. */
		if (!magr->iswiz && magr->data != &mons[PM_MEDUSA] &&
			!(magr->data->mflags3 & M3_COVETOUS) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && magr->data->mlet == S_DRAGON) && !(magr->data->geno & G_UNIQ) &&
			((mdef->mtame && !rn2(10)) || (magr->mtame && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4)) )) {
		    if (canseemon(magr)) pline("%s looks calmer.", Monnam(magr));
		    if (magr == u.usteed && !mayfalloffsteed())
			dismount_steed(DISMOUNT_THROWN);
		    if (!magr->mfrenzied && !rn2(3)) magr->mpeaceful = 1;
		    else magr->mpeaceful = 0;
		    magr->mtame = 0;
		    tmp = 0;
		}

		}
		break;
	    case AD_CALM:
		if (magr->mtame) {

		int untamingchance = 10;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_PETKEEPING)) {
				default: untamingchance = 10; break;
				case P_BASIC: untamingchance = 9; break;
				case P_SKILLED: untamingchance = 8; break;
				case P_EXPERT: untamingchance = 7; break;
				case P_MASTER: untamingchance = 6; break;
				case P_GRAND_MASTER: untamingchance = 5; break;
				case P_SUPREME_MASTER: untamingchance = 4; break;
			}
		}

		/* Certain monsters aren't even made peaceful. */
		if (!magr->iswiz && magr->data != &mons[PM_MEDUSA] &&
			!(magr->data->mflags3 & M3_COVETOUS) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && magr->data->mlet == S_DRAGON) && !(magr->data->geno & G_UNIQ) &&
			((mdef->mtame && !rn2(10)) || (magr->mtame && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4)) )) {
		    if (canseemon(magr)) pline("%s looks calmer.", Monnam(magr));
		    if (magr == u.usteed && !mayfalloffsteed())
			dismount_steed(DISMOUNT_THROWN);
		    if (!magr->mfrenzied && !rn2(3)) magr->mpeaceful = 1;
		    else magr->mpeaceful = 0;
		    magr->mtame = 0;
		    tmp = 0;
		}

		}
		break;
	    case AD_FREN:
		if (magr->mtame) {

		int untamingchance = 10;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_PETKEEPING)) {
				default: untamingchance = 10; break;
				case P_BASIC: untamingchance = 9; break;
				case P_SKILLED: untamingchance = 8; break;
				case P_EXPERT: untamingchance = 7; break;
				case P_MASTER: untamingchance = 6; break;
				case P_GRAND_MASTER: untamingchance = 5; break;
				case P_SUPREME_MASTER: untamingchance = 4; break;
			}
		}

		if (!magr->mfrenzied && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && magr->data->mlet == S_DRAGON) && (!magr->mtame || (magr->mtame <= rnd(21) && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4) ) ) ) {
			magr->mpeaceful = magr->mtame = 0;
			magr->mfrenzied = 1;
		    if (canseemon(magr)) pline("%s enters a state of frenzy!", Monnam(magr));
		}

		}
		break;
	    case AD_EDGE:
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;
	    case AD_STUN:
		if (!magr->mtame) tmp = 0;
		if (!magr->mstun) {
		    magr->mstun = 1;
		    if (canseemon(magr))
			pline("%s %s...", Monnam(magr), makeplural(stagger(magr->data, "stagger")));
		}
		break;
	    case AD_FUMB:
	    case AD_DROP:
		if (!magr->mstun) {
		    magr->mstun = 1;
		    if (canseemon(magr))
			pline("%s %s...", Monnam(magr), makeplural(stagger(magr->data, "stagger")));
		}
		if (magr->mtame && !rn2(4)) {
			mon_adjust_speed(magr, -1, (struct obj *)0);
			magr->mstrategy &= ~STRAT_WAITFORU;
			if (!rn2(4)) magr->inertia += (2 + (tmp * 2));
		}
		break;
	    case AD_TREM:
		if (!magr->mstun) {
		    magr->mstun = 1;
		    if (canseemon(magr))
			pline("%s %s...", Monnam(magr), makeplural(stagger(magr->data, "stagger")));
		}
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (!rn2(3)) badpeteffect(magr);
		}
		break;
	    case AD_SOUN:
		if (!magr->mstun) {
		    magr->mstun = 1;
		    if (canseemon(magr))
			pline("%s %s...", Monnam(magr), makeplural(stagger(magr->data, "stagger")));
		}
		if (magr->mtame) {
			magr->mconf = 1;
			if (!rn2(5)) badpeteffect(magr);
		}
		break;
	    case AD_FIRE:
		if (resists_fire(magr) && !player_will_pierce_resistance()) {
		    if (canseemon(magr)) {
			pline("%s is mildly warmed.", Monnam(magr));
			golemeffects(magr, AD_FIRE, tmp);
		    }
		    tmp = 0;
		    break;
		}
		if(canseemon(magr))
		    pline("%s is suddenly very hot!", Monnam(magr));
		break;
	    case AD_ELEC:
		if (resists_elec(magr) && !player_will_pierce_resistance()) {
		    if (canseemon(magr)) {
			pline("%s is mildly tingled.", Monnam(magr));
			golemeffects(magr, AD_ELEC, tmp);
		    }
		    tmp = 0;
		    break;
		}
		if(canseemon(magr))
		    pline("%s is jolted with electricity!", Monnam(magr));
		break;

	    case AD_LITE:
		if (is_vampire(magr->data)) {
			tmp *= 2; /* vampires take more damage from sunlight --Amy */
			if (canseemon(magr)) pline("%s is irradiated!", Monnam(magr));
		}
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;
	    case AD_BANI:
		if (magr->mtame && !rn2(3)) {
			magr->willbebanished = TRUE;
			break;
		} /* else fall through */
	    case AD_TLPT:
		if (!tele_restrict(magr)) (void) rloc(magr, FALSE);
		break;
	    case AD_NEXU:
		if (!tele_restrict(magr)) (void) rloc(magr, FALSE);
		if (magr->mtame) {
			if (!rn2(2)) {
				if (magr->mhpmax > 1) magr->mhpmax--;
				if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			}
			if (!rn2(7)) badpeteffect(magr);
		}
		break;
	    case AD_ABDC:
		if (!tele_restrict(magr)) (void) rloc(magr, FALSE);
		if (magr->mtame) badpeteffect(magr);
		break;
	    case AD_SLEE:
		{
		int parlyzdur = rnd(mdef->mtame ? 3 : 10);
		if (!rn2(5)) parlyzdur = tmp;
		if (mdef->mtame) {
			if (parlyzdur > 2) {
				parlyzdur = rnd(parlyzdur);
				if (parlyzdur < 2) parlyzdur = 2;
			}
			if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
		}
		if (parlyzdur > 127) parlyzdur = 127;

		if (!magr->msleeping && (!rn2(5) || !(mdef->mtame)) && sleep_monst(magr, parlyzdur, -1)) {
		    if (canseemon(magr)) {
			pline("%s is put to sleep.", Monnam(magr));
		    }
		    magr->mstrategy &= ~STRAT_WAITFORU;
		    slept_monst(magr);
		}

		}
		break;

	    case AD_SLOW:
		if(magr->mspeed != MSLOW) {
		    unsigned int oldspeed = magr->mspeed;

		    mon_adjust_speed(magr, -1, (struct obj *)0);
		    magr->mstrategy &= ~STRAT_WAITFORU;
		    if (magr->mspeed != oldspeed && canseemon(magr))
			pline("%s slows down.", Monnam(magr));
		}
		break;

	    case AD_WGHT:
		if(magr->mspeed != MSLOW) {
		    unsigned int oldspeed = magr->mspeed;

		    mon_adjust_speed(magr, -1, (struct obj *)0);
		    magr->mstrategy &= ~STRAT_WAITFORU;
		    if (magr->mspeed != oldspeed && canseemon(magr))
			pline("%s slows down.", Monnam(magr));
		}
		if (magr->mtame) {
			if (canseemon(magr)) pline("%s's weight increases.", Monnam(magr));
			magr->inertia += tmp;
		}
		break;

	    case AD_INER:
		if(magr->mspeed != MSLOW) {
		    unsigned int oldspeed = magr->mspeed;

		    mon_adjust_speed(magr, -1, (struct obj *)0);
		    magr->mstrategy &= ~STRAT_WAITFORU;
		    if (magr->mspeed != oldspeed && canseemon(magr))
			pline("%s slows down.", Monnam(magr));
		}
		if (magr->mtame) {
			if (canseemon(magr)) {
				if (!magr->inertia) pline("%s slows down to a crawl.", Monnam(magr));
				else pline("%s seems even slower.", Monnam(magr));
			}
			magr->inertia += (5 + (tmp * 5));
		}
		break;

	    case AD_LAZY:
		if(magr->mspeed != MSLOW) {
		    unsigned int oldspeed = magr->mspeed;

		    mon_adjust_speed(magr, -1, (struct obj *)0);
		    magr->mstrategy &= ~STRAT_WAITFORU;
		    if (magr->mspeed != oldspeed && canseemon(magr))
			pline("%s slows down.", Monnam(magr));
		}
		if(!rn2(3) && (!rn2(4) || !(mdef->mtame)) && magr->mcanmove && !(dmgtype(magr->data, AD_PLYS))) {

			int parlyzdur = rnd(magr->mtame ? 4 : 10);
			if (!rn2(4)) parlyzdur = tmp;
			if (magr->mtame) {
				if (parlyzdur > 3) {
					parlyzdur = rnd(parlyzdur);
					if (parlyzdur < 3) parlyzdur = 3;
				}
				if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
			}
			if (parlyzdur > 127) parlyzdur = 127;

		    if (canseemon(magr)) {
			pline("%s is paralyzed.", Monnam(magr));
		    }
		    magr->mcanmove = 0;
		    magr->mfrozen = parlyzdur;
		    magr->mstrategy &= ~STRAT_WAITFORU;
		}
		if (magr->mtame) {
			magr->inertia += (3 + (tmp * 3));
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;

	    case AD_NUMB:
		if(!rn2(10) && magr->mspeed != MSLOW) {
		    unsigned int oldspeed = magr->mspeed;

		    mon_adjust_speed(magr, -1, (struct obj *)0);
		    magr->mstrategy &= ~STRAT_WAITFORU;
		    if (magr->mspeed != oldspeed && canseemon(magr))
			pline("%s is numbed.", Monnam(magr));
		}
		if (magr->mtame && !rn2(15)) badpeteffect(magr);
		break;

	    case AD_STAT:
		if (magr->mtame) {
			tmp += rn1(10,6);
			if (!rn2(100)) tmp += 10000; /* The poison was deadly... */
			badpeteffect(magr);
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;

	    case AD_DARK:
		do_clear_area(magr->mx,magr->my, 7, set_lit, (void *)((char *)0));
		if (canseemon(magr)) pline("A sinister darkness fills the area!");
		if (magr->data->mlet == S_ANGEL) tmp *= 2;
		break;

	    case AD_THIR:
		if (!mdead && (mdef->mhp > 0)) {
			mdef->mhp += tmp;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (canseemon(mdef)) pline("%s looks healthier!", Monnam(mdef) );
		}
		break;

	    case AD_NTHR:
		if (!mdead && (mdef->mhp > 0)) {
			mdef->mhp += tmp;
			if (mdef->mhp > mdef->mhpmax) mdef->mhp = mdef->mhpmax;
			if (canseemon(mdef)) pline("%s looks healthier!", Monnam(mdef) );
		}
		if (magr->mtame && !rn2(10)) {

			if (rn2(2) && !resists_drli(magr)) {
				if (canseemon(magr)) pline("%s suddenly seems weaker!", Monnam(magr));
				magr->mhpmax -= d(2,6);
				if (magr->mhpmax < 1) magr->mhpmax = 1;
				if (magr->m_lev == 0) tmp = magr->mhp;
				else magr->m_lev--;
				/* Automatic kill if drained past level 0 */
			}

		}

		break;

	    case AD_RAGN:
		ragnarok(FALSE);
		if (evilfriday && mdef->m_lev > 1) evilragnarok(FALSE,mdef->m_lev);
		break;

	    case AD_AGGR:

		aggravate();
		if (!rn2(20)) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			if (mdef) (void) makemon((struct permonst *)0, mdef->mx, mdef->my, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
			u.aggravation = 0;
		}

		break;

	    case AD_CONT:

		if (!rn2(30)) {
			magr->isegotype = 1;
			magr->egotype_contaminator = 1;
		}
		if (!rn2(100)) {
			magr->isegotype = 1;
			magr->egotype_weeper = 1;
		}
		if (!rn2(250)) {
			magr->isegotype = 1;
			magr->egotype_radiator = 1;
		}
		if (!rn2(250)) {
			magr->isegotype = 1;
			magr->egotype_reactor = 1;
		}

		if (magr->mtame) {
			if (!rn2(30)) {
				int untamingchance = 10;

				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_PETKEEPING)) {
						default: untamingchance = 10; break;
						case P_BASIC: untamingchance = 9; break;
						case P_SKILLED: untamingchance = 8; break;
						case P_EXPERT: untamingchance = 7; break;
						case P_MASTER: untamingchance = 6; break;
						case P_GRAND_MASTER: untamingchance = 5; break;
						case P_SUPREME_MASTER: untamingchance = 4; break;
					}
				}

				/* Certain monsters aren't even made peaceful. */
				if (!magr->iswiz && magr->data != &mons[PM_MEDUSA] &&
					!(magr->data->mflags3 & M3_COVETOUS) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && magr->data->mlet == S_DRAGON) && !(magr->data->geno & G_UNIQ) &&
					((mdef->mtame && !rn2(10)) || (magr->mtame && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4)) )) {
				    if (canseemon(magr)) pline("%s looks calmer.", Monnam(magr));
				    if (magr == u.usteed && !mayfalloffsteed())
					dismount_steed(DISMOUNT_THROWN);
				    if (!magr->mfrenzied && !rn2(3)) magr->mpeaceful = 1;
				    else magr->mpeaceful = 0;
				    magr->mtame = 0;
				    tmp = 0;
				}

			} else if (!rn2(3)) {
				badpeteffect(magr);
			} else {
				if (magr->mhpmax > 1) magr->mhpmax--;
				if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			}
		}

		break;

	    case AD_RUST:
		if (magr->mtame && !rn2(5)) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (canseemon(magr)) pline("%s rusts!", Monnam(magr));
		}
		break;

	    case AD_CORR:
		if (magr->mtame && !rn2(5)) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (canseemon(magr)) pline("%s corrodes!", Monnam(magr));
		}
		break;

	    case AD_DCAY:
		if (magr->mtame && !rn2(5)) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (canseemon(magr)) pline("%s decays!", Monnam(magr));
		}
		break;

	    case AD_FLAM:
	    case AD_WNCE:
		if (magr->mtame && !rn2(5)) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (canseemon(magr)) pline("%s burns!", Monnam(magr));
		}
		break;

	    case AD_FRZE:
		if (!resists_cold(magr) && resists_fire(magr)) {
			tmp *= 2;
			if (canseemon(magr)) pline("%s is suddenly ice-cold!", Monnam(magr));
		}
		if (magr->mtame) {
			pline("%s seems to be moving slower.", Monnam(magr));
			if (!rn2(3)) mon_adjust_speed(magr, -1, (struct obj *)0);
			if (!rn2(3)) magr->inertia += tmp;
		}

		break;
	    case AD_ICEB:
		if (!resists_cold(magr)) {
			tmp *= 2;
			if (canseemon(magr)) pline("%s is suddenly shockfrosted!", Monnam(magr));
		}
		if (magr->mtame && !rn2(2)) {
			pline("%s seems to be moving slower.", Monnam(magr));
			if (!rn2(3)) mon_adjust_speed(magr, -1, (struct obj *)0);
			if (!rn2(3)) magr->inertia += tmp;
		}
		break;
	    case AD_MALK:
		if (!resists_elec(magr)) {
			tmp *= 2;
			if (canseemon(magr)) pline("%s is jolted by high voltage!", Monnam(magr));
		}
		if (magr->mtame && !rn2(5)) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;
	    case AD_UVUU:
		if (has_head(magr->data)) {
			tmp *= 2;
			if (!rn2(1000)) {
				tmp *= 100;
				if (canseemon(magr)) pline("%s's %s is torn apart!", Monnam(magr), mbodypart(magr, HEAD));
			} else if (canseemon(magr)) pline("%s's %s is spiked!", Monnam(magr), mbodypart(magr, HEAD));
		}
		if (magr->mtame) {
			tmp *= 2;
			if (!which_armor(magr, W_ARMH)) tmp *= 2;
			badpeteffect(magr);
		}
		break;
	    case AD_GRAV:
		if (!is_flyer(magr->data)) {
			tmp *= 2;
			if (canseemon(magr)) pline("%s slams into the ground!", Monnam(magr));
		}
		if (magr->mtame && !rn2(2) && !magr->mfrozen) {
			magr->mcanmove = 0;
			magr->mfrozen = rnd(10);
			magr->mstrategy &= ~STRAT_WAITFORU;
		}
		break;
	    case AD_CHKH:
		if (mdef->m_lev > magr->m_lev) tmp += (mdef->m_lev - magr->m_lev);
		break;
	    case AD_CHRN:
		if ((tmp > 0) && (magr->mhpmax > 1)) {
			magr->mhpmax--;
			if (canseemon(magr)) pline("%s feels bad!", Monnam(magr));
		}
		if (magr->mtame) badpeteffect(magr);
		break;
	    case AD_HODS:
		tmp += magr->m_lev;
		break;
	    case AD_DIMN:
		tmp += mdef->m_lev;
		if (magr->mtame && !rn2(10)) badpeteffect(magr);
		break;
	    case AD_BURN:
		if (resists_cold(magr) && !resists_fire(magr)) {
			tmp *= 2;
			if (canseemon(magr)) pline("%s is burning!", Monnam(magr));
		}
		if (magr->mtame) magr->healblock += (5 + (tmp * 5));
		break;
	    case AD_PLAS:
		if (!resists_fire(magr)) {
			tmp *= 2;
			if (canseemon(magr)) pline("%s is suddenly extremely hot!", Monnam(magr));
		}
		if (magr->mtame) {
			magr->healblock += (10 + (tmp * 10));
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;
	    case AD_DETH:
		if (magr->mtame) {
			if (!rn2(16)) tmp += 10000; /* instant death */
			else {
				if (canseemon(magr)) pline("%s is hit by the touch of death!", Monnam(magr));
				magr->mhpmax -= rnd(10);
				if (magr->mhpmax < 1) magr->mhpmax = 1;
				if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			}
		}
		break;
	    case AD_SLUD:
		if (!resists_acid(magr)) {
			tmp *= 2;
			if (canseemon(magr)) pline("%s is covered with sludge!", Monnam(magr));
		}
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax -= rnd(3);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;

		}
		break;
	    case AD_LAVA:
		if (resists_cold(magr) && !resists_fire(magr)) {
			tmp *= 4;
			if (canseemon(magr)) pline("%s is scorched by hot lava!", Monnam(magr));
		} else if (!resists_fire(magr)) {
			tmp *= 2;
			if (canseemon(magr)) pline("%s is covered with hot lava!", Monnam(magr));
		}
		if (magr->mtame) {
			magr->healblock += (20 + (tmp * 20));
			if (magr->mhpmax > 1) magr->mhpmax -= 5;
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (magr->m_lev > 0) magr->m_lev--;
		}
		break;
	    case AD_FAKE:
		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}
		break;
	    case AD_WEBS:
		(void) maketrap(magr->mx, magr->my, WEB, 0, FALSE);
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap(FALSE);
		break;
	    case AD_TRAP:
		if (t_at(magr->mx, magr->my) == 0) (void) maketrap(magr->mx, magr->my, rndtrap(), 0, FALSE);
		else makerandomtrap(FALSE);

		break;

	    case AD_CNCL:
		if (rnd(100) > magr->data->mr) {
			cancelmonsterlite(magr);
			if (canseemon(magr)) pline("%s is covered in sparkling lights!", Monnam(magr));
		}
		if (magr->mtame && !rn2(2)) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			badpeteffect(magr);
		}
		break;
	    case AD_ICUR:
		if (!rn2(10) && (rnd(100) > magr->data->mr)) {
			cancelmonsterlite(magr);
		}
		if (magr->mtame) {
			badpeteffect(magr);
			if (magr->mhpmax > 1) magr->mhpmax -= rnd(8);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (magr->m_lev > 0) magr->m_lev--;
		}
		break;
	    case AD_NACU:
		if (!rn2(10) && (rnd(100) > magr->data->mr)) {
			cancelmonsterlite(magr);
		}
		if (magr->mtame) allbadpeteffects(magr);
		break;
	    case AD_CURS:
		if (!rn2(10) && (rnd(100) > magr->data->mr)) {
			cancelmonsterlite(magr);
		}
		break;
	    case AD_LEGS:
		if (magr->mtame && !rn2(7)) {
			mon_adjust_speed(magr, -1, (struct obj *)0);
			magr->mstrategy &= ~STRAT_WAITFORU;
			if (tmp > 0) magr->bleedout += rnd(tmp);
			if (!rn2(3)) magr->inertia += (3 + (tmp * 3));
		}
		break;
	    case AD_FEAR:
		if (rnd(100) > magr->data->mr) {
		     monflee(magr, rnd(1 + tmp), FALSE, TRUE, FALSE);
			if (canseemon(magr)) pline("%s is suddenly very afraid!",Monnam(magr));
		}
		break;
	    case AD_INSA:
		if (rnd(100) > magr->data->mr) {
		     monflee(magr, rnd(1 + tmp), FALSE, TRUE, FALSE);
			if (canseemon(magr)) pline("%s is suddenly very afraid!",Monnam(magr));
		}
		if (!magr->mstun) {
		    magr->mstun = 1;
		    if (canseemon(magr))
			pline("%s %s...", Monnam(magr),
			      makeplural(stagger(magr->data, "stagger")));
		}
		if (!magr->mconf) {
		    if (canseemon(magr)) pline("%s is suddenly very confused!", Monnam(magr));
		    magr->mconf = 1;
		    magr->mstrategy &= ~STRAT_WAITFORU;
		}
		if (magr->mtame && !rn2(2)) badpeteffect(magr);
		break;
	    case AD_SANI:

		if (!rn2(10)) {

			/* STUPID bug where the game displays the same name twice for some inexplicable reason --Amy */
			strcpy(buf, Monnam(magr));

			magr->mconf = 1;
			switch (rnd(4)) {

				case 1:
					pline("%s sees %s chow dead bodies.", buf, mon_nam(mdef)); break;
				case 2:
					pline("%s shudders at %s's terrifying %s.", buf, mon_nam(mdef), makeplural(mbodypart(mdef, EYE)) ); break;
				case 3:
					pline("%s feels sick at entrails caught in %s's tentacles.", buf, mon_nam(mdef)); break;
				case 4:
					pline("%s sees maggots breed in the rent %s of %s.", buf, mbodypart(mdef, STOMACH), mon_nam(mdef)); break;

			}

		}
		if (magr->mtame && !rn2(5)) badpeteffect(magr);

		break;
	    case AD_DREA:
		if (!magr->mcanmove) {
			tmp *= 4;
			if (canseemon(magr)) pline("%s's dream is eaten!",Monnam(magr));
		}
		break;
	    case AD_CONF:
		if (!magr->mconf) {
		    if (canseemon(magr)) pline("%s is suddenly very confused!", Monnam(magr));
		    magr->mconf = 1;
		    magr->mstrategy &= ~STRAT_WAITFORU;
		}
		break;

	    case AD_HALU:
		if (!magr->mconf) {
		    if (canseemon(magr)) pline("%s is suddenly very confused!", Monnam(magr));
		    magr->mconf = 1;
		    magr->mstrategy &= ~STRAT_WAITFORU;
		}
		if (magr->mtame && !rn2(3)) badpeteffect(magr);
		break;

	    case AD_DEPR:
		if (!magr->mconf) {
		    if (canseemon(magr)) pline("%s is suddenly very confused!", Monnam(magr));
		    magr->mconf = 1;
		    magr->mstrategy &= ~STRAT_WAITFORU;
		}
		if (magr->mtame) {
			badpeteffect(magr);
			if (magr->m_lev > 0) magr->m_lev--;
			if (magr->m_lev > 0) magr->m_lev--;
			if (magr->m_lev == 0) tmp *= 10;
		}
		break;

	    case AD_SPC2:
		if (!magr->mconf) {
		    if (canseemon(magr)) pline("%s is suddenly very confused!", Monnam(magr));
		    magr->mconf = 1;
		    magr->mstrategy &= ~STRAT_WAITFORU;
		}
		if (magr->mtame && !rn2(2)) badpeteffect(magr);
		break;

	    case AD_AMNE:

		if (magr->mtame) {
			if (canseemon(magr)) pline("%s seems oblivious!", Monnam(magr));
			magr->mconf = 1;
			if (!rn2(3)) badpeteffect(magr);
		}
		break;

	    case AD_FAMN:
		if (magr->mtame) {
			makedoghungry(magr, tmp * rnd(50));
			if (canseemon(magr)) pline("%s suddenly looks hungry.", Monnam(magr));
		}
		break;

	    case AD_WRAT:
		mon_drain_en(magr, ((magr->m_lev > 0) ? (rnd(magr->m_lev)) : 0) + 1 + tmp);
		if (magr->mtame) {
			badpeteffect(magr);
			if (magr->mhpmax > 1) magr->mhpmax -= rnd(5);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;
	    case AD_MANA:
		if (magr->mtame) tmp *= 2;
		mon_drain_en(magr, ((magr->m_lev > 0) ? (rnd(magr->m_lev)) : 0) + 1 + tmp);
		break;
	    case AD_TECH:
		mon_drain_en(magr, ((magr->m_lev > 0) ? (rnd(magr->m_lev)) : 0) + 1 + tmp);
		if (magr->mtame && !rn2(7)) badpeteffect(magr);
		break;
	    case AD_MEMO:
		mon_drain_en(magr, ((magr->m_lev > 0) ? (rnd(magr->m_lev)) : 0) + 1 + tmp);
		if (magr->mtame && !rn2(50)) {
			int untamingchance = 10;

			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_PETKEEPING)) {
					default: untamingchance = 10; break;
					case P_BASIC: untamingchance = 9; break;
					case P_SKILLED: untamingchance = 8; break;
					case P_EXPERT: untamingchance = 7; break;
					case P_MASTER: untamingchance = 6; break;
					case P_GRAND_MASTER: untamingchance = 5; break;
					case P_SUPREME_MASTER: untamingchance = 4; break;
				}
			}

			/* Certain monsters aren't even made peaceful. */
			if (!magr->iswiz && magr->data != &mons[PM_MEDUSA] &&
				!(magr->data->mflags3 & M3_COVETOUS) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && magr->data->mlet == S_DRAGON) && !(magr->data->geno & G_UNIQ) &&
				((mdef->mtame && !rn2(10)) || (magr->mtame && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4)) )) {
			    if (canseemon(magr)) pline("%s looks calmer.", Monnam(magr));
			    if (magr == u.usteed && !mayfalloffsteed())
				dismount_steed(DISMOUNT_THROWN);
			    if (!magr->mfrenzied && !rn2(3)) magr->mpeaceful = 1;
			    else magr->mpeaceful = 0;
			    magr->mtame = 0;
			    tmp = 0;
			}

		}
		break;
	    case AD_TRAI:
		mon_drain_en(magr, ((magr->m_lev > 0) ? (rnd(magr->m_lev)) : 0) + 1 + tmp);
		if (magr->mtame && !rn2(8)) {
			if (magr->mhpmax > 1) magr->mhpmax -= rnd(8);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (magr->m_lev > 0) magr->m_lev--;
		}
		break;
	    case AD_DREN:
	    	if (!resists_magm(magr)) {
	    	    mon_drain_en(magr, ((magr->m_lev > 0) ? (rnd(magr->m_lev)) : 0) + 1);
	    	}
		break;
	    case AD_BLND:
		    if (canseemon(magr) && magr->mcansee)
			pline("%s is blinded.", Monnam(magr));
		    if ((tmp += magr->mblinded) > 127) tmp = 127;
		    magr->mblinded = tmp;
		    magr->mcansee = 0;
		    magr->mstrategy &= ~STRAT_WAITFORU;
		if (!magr->mtame) tmp = 0;
		break;

	    case AD_WET:
		if (magr->mtame) {
			if (canseemon(magr)) pline("%s is doused with water!", Monnam(magr));
			if (rn2(3)) magr->mconf = 1;
			if (!rn2(3)) mon_adjust_speed(magr, -1, (struct obj *)0);
			if (!rn2(5)) badpeteffect(magr);
		}
		break;
	    case AD_AXUS:
		if (magr->mtame) tmp *= rnd(4);
		break;
	    case AD_DEST:
		if (magr->mtame) tmp *= 10;
		break;
	    case AD_SIN:
		if (magr->mtame && !rn2(3)) {
			badpeteffect(magr);
			if (magr->mhpmax > 1) magr->mhpmax -= 8;
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;

			if (!rn2(10)) {
				int untamingchance = 10;

				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_PETKEEPING)) {
						default: untamingchance = 10; break;
						case P_BASIC: untamingchance = 9; break;
						case P_SKILLED: untamingchance = 8; break;
						case P_EXPERT: untamingchance = 7; break;
						case P_MASTER: untamingchance = 6; break;
						case P_GRAND_MASTER: untamingchance = 5; break;
						case P_SUPREME_MASTER: untamingchance = 4; break;
					}
				}

				if (!magr->mfrenzied && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && magr->data->mlet == S_DRAGON) && (!magr->mtame || (magr->mtame <= rnd(21) && (untamingchance > rnd(10)) && !((rnd(30 - ACURR(A_CHA))) < 4) ) ) ) {
					magr->mpeaceful = magr->mtame = 0;
					magr->mfrenzied = 1;
					if (canseemon(magr)) pline("%s enters a state of frenzy!", Monnam(magr));
				}
			}

		}
		break;
	    case AD_ALIN:
		if (magr->mtame && !rn2(10)) badpeteffect(magr);
		break;
	    case AD_NGEN:
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax -= 5;
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			badpeteffect(magr);
			badpeteffect(magr);
		}
		break;
	    case AD_IDAM:
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax -= rnd(3);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			badpeteffect(magr);
		}
		break;
	    case AD_ANTI:
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax -= rnd(20);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			badpeteffect(magr);
			badpeteffect(magr);
			badpeteffect(magr);
			badpeteffect(magr);
			badpeteffect(magr);
		}
		break;
	    case AD_WRAP:
		if (magr->mtame && !rn2(10)) {
			badpeteffect(magr);
			if (!mdead && mdef && is_drowningpool(mdef->mx, mdef->my)) tmp *= 10;
		}
		break;
	    case AD_SKIL:
		if (magr->mtame && !rn2(4)) badpeteffect(magr);
		break;
	    case AD_TDRA:
		if (magr->mtame && !rn2(4)) badpeteffect(magr);
		break;
	    case AD_BLAS:
		if (magr->mtame && !rn2(5)) badpeteffect(magr);
		break;
	    case AD_SUCK:
		if (magr->mtame) {
			if (canseemon(magr)) pline("%s is sucked by a vacuum cleaner!", Monnam(magr));
			badpeteffect(magr);
			badpeteffect(magr);
			badpeteffect(magr);
			if (!rn2(3)) {
				if (magr->mhpmax > 1) magr->mhpmax--;
				if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			}
		}
		break;
	    case AD_VAPO:
		if (magr->mtame) tmp *= 3;
		if (magr->mtame && !rn2(10) && !resists_disint(magr)) {
			if (canseemon(magr)) pline("%s is hit by vaporization!", Monnam(magr));
			struct obj *otmp2;
			otmp2 = (struct obj *)0;
			if (magr->misc_worn_check & W_ARMS) {
			otmp2 = which_armor(magr, W_ARMS);
		    } else if (magr->misc_worn_check & W_ARMC) {
			otmp2 = which_armor(magr, W_ARMC);
		    } else if (magr->misc_worn_check & W_ARM) {
			otmp2 = which_armor(magr, W_ARM);
		    } else if (magr->misc_worn_check & W_ARMU) {
			otmp2 = which_armor(magr, W_ARMU);
		    } else {
			/* no body armor, victim dies; destroy cloak
			   and shirt now in case target gets life-saved */
			tmp += 10000;
		    }
			if (otmp2 && (otmp2 != (struct obj *)0) ) m_useup(magr, otmp2);

		}
		break;
	    case AD_DISE:
		if (magr->mtame && !rn2(3)) {
			if (canseemon(magr)) pline("%s looks sick.", Monnam(magr));
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;
	    case AD_VOMT:
		if (magr->mtame && !rn2(4)) {
			if (canseemon(magr)) pline("%s looks sick.", Monnam(magr));
			if (!rn2(3)) {
				if (magr->mhpmax > 1) magr->mhpmax--;
				if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			}
			if (!rn2(3)) magr->mconf = TRUE;
			if (!rn2(3)) magr->mstun = TRUE;
			if (!rn2(5) && magr->mcanmove) {
				magr->mcanmove = 0;
				magr->mfrozen = 5;
				magr->mstrategy &= ~STRAT_WAITFORU;
			}
		}
		break;
	    case AD_NGRA:
		if (magr->mtame && !rn2(20)) badpeteffect(magr);
		break;
	    case AD_WTHR:
		if (magr->mtame) {
			if (canseemon(magr)) pline("%s withers!", Monnam(magr));
			if (magr->mhpmax > 1) magr->mhpmax -= rnd(8);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (magr->m_lev > 0) magr->m_lev--;
		}
		break;
	    case AD_LUCK:
		if (magr->mtame && !rn2(5)) badpeteffect(magr);
		break;
	    case AD_LETH:
		if (magr->mtame) {
			if (canseemon(magr)) pline("%s seems oblivious!", Monnam(magr));
			magr->mhpmax -= 8;
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (magr->m_lev > 0) magr->m_lev--;
			badpeteffect(magr);
			badpeteffect(magr);
			badpeteffect(magr);
		}
		break;
	    case AD_NPRO:
	    case AD_INVE:
		if (magr->mtame) {
			if (canseemon(magr)) pline("%s seems less protected!", Monnam(magr));
			magr->mhpmax -= 8;
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (magr->m_lev > 0) magr->m_lev--;

		}
		break;
	    case AD_DISP:
	    case AD_TPTO:
	    case AD_SWAP:
		if (magr->mtame && !rn2(3) && magr->mcanmove) {
			magr->mcanmove = 0;
			magr->mfrozen = 2;
			magr->mstrategy &= ~STRAT_WAITFORU;
			if (canseemon(magr)) pline("%s is stopped in %s tracks.", Monnam(magr), mhis(magr));
		}
		break;
	    case AD_PEST:
		if (magr->mtame) {
			if (canseemon(magr)) pline("%s looks very sick.", Monnam(magr));
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;

	    case AD_PAIN:
		if (magr->mhp > 9) tmp += (magr->mhp / 10);
		if (canseemon(magr)) pline("%s shrieks in pain!", Monnam(magr));
		break;

	    case AD_VULN:
		if (magr->mtame) {

			if (canseemon(magr)) pline("%s seems more vulnerable!", Monnam(magr));
			magr->mhpmax -= rnd(8);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (magr->m_lev == 0) tmp = magr->mhp;
			else magr->m_lev--;

		}
		break;

	    case AD_NAST:
		if (magr->mtame) {
			badpeteffect(magr);
			badpeteffect(magr);
			badpeteffect(magr);
		}
		break;

	    case AD_CAST:
		if (magr->mtame && !rn2(2)) tmp *= 3;
		break;

	    case AD_CLRC:
		if (magr->mtame && !rn2(2)) tmp *= 2;
		break;

	    case AD_SPEL:
		if (magr->mtame && !rn2(2)) tmp *= 2;
		break;

	    case AD_SAMU:
		if (magr->mtame && !rn2(3)) badpeteffect(magr);
		break;

	    case AD_SLIM:
		if (magr->mtame) {
			if (!rn2(5)) badpeteffect(magr);
		}
		break;

	    case AD_DISN:
		if (magr->mtame && !rn2(10) && !resists_disint(magr)) {
			if (canseemon(magr)) pline("%s is hit by disintegration!", Monnam(magr));
			struct obj *otmp2;
			otmp2 = (struct obj *)0;
			if (magr->misc_worn_check & W_ARMS) {
			otmp2 = which_armor(magr, W_ARMS);
		    } else if (magr->misc_worn_check & W_ARMC) {
			otmp2 = which_armor(magr, W_ARMC);
		    } else if (magr->misc_worn_check & W_ARM) {
			otmp2 = which_armor(magr, W_ARM);
		    } else if (magr->misc_worn_check & W_ARMU) {
			otmp2 = which_armor(magr, W_ARMU);
		    } else {
			tmp += 10000;
		    }
			if (otmp2 && (otmp2 != (struct obj *)0) ) m_useup(magr, otmp2);

		}
		break;
	    case AD_SHRD:
		if (magr->mtame) {
			if (canseemon(magr)) pline("%s is being shredded!", Monnam(magr));
			struct obj *otmp2;
			otmp2 = (struct obj *)0;

			if (rn2(3)) {
				badpeteffect(magr);
				badpeteffect(magr);
			} else {

				if (magr->misc_worn_check & W_ARMS) {
				otmp2 = which_armor(magr, W_ARMS);
			    } else if (magr->misc_worn_check & W_ARMC) {
				otmp2 = which_armor(magr, W_ARMC);
			    } else if (magr->misc_worn_check & W_ARM) {
				otmp2 = which_armor(magr, W_ARM);
			    } else if (magr->misc_worn_check & W_ARMU) {
				otmp2 = which_armor(magr, W_ARMU);
			    } else {
				/* no body armor, victim dies; destroy cloak
				   and shirt now in case target gets life-saved */
				tmp += 10000;
			    }
				if (otmp2 && (otmp2 != (struct obj *)0) ) m_useup(magr, otmp2);
			}

		}
		break;

	    case AD_LITT:
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (!rn2(3)) badpeteffect(magr);
		}
		break;

	    case AD_POLY:
		if (magr->mtame && !rn2(3)) {
			badpeteffect(magr);
		}
		break;

	    case AD_CHAO:
		if (magr->mtame) {
			badpeteffect(magr);
		}
		break;

	    case AD_RUNS:
		if (magr->mtame) {
			badpeteffect(magr);
			badpeteffect(magr);
			badpeteffect(magr);
		}
		break;

	    case AD_MINA:
		if (magr->mtame) {
			badpeteffect(magr);
			badpeteffect(magr);
			badpeteffect(magr);
		}
		break;

	    case AD_DATA:
		if (magr->mtame) {
			tmp += 10000;
			allbadpeteffects(magr);
		}
		break;

	    case AD_BADE:
		if (magr->mtame) {
			badpeteffect(magr);
		}
		break;

	    case AD_RBAD:
		if (magr->mtame) {
			badpeteffect(magr);
			badpeteffect(magr);
			badpeteffect(magr);
		}
		break;

	    case AD_BLEE:
		if (tmp > 0) {
			magr->bleedout += tmp;
			if (vis) pline("%s is struck by several thorns.", Monnam(magr));
		}
		break;

	    case AD_SHAN:
		if (magr->mtame) {
			if (!rn2(3)) badpeteffect(magr);
			if (!rn2(3)) {
				if (magr->mhpmax > 1) magr->mhpmax--;
				if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			}
		}
		break;

	    case AD_SCOR:
		if (magr->mtame && !rn2(40)) {
			if (magr->mhpmax > 1) magr->mhpmax--;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
		}
		break;

	    case AD_TERR:
		if (mdef->data == &mons[PM_BUILDER]) terrainterror(1);
		else terrainterror(0);
		break;

	    case AD_FEMI:
		if (magr->mtame) {
			badpeteffect(magr);
		}
		break;

	    case AD_LEVI:
		if (magr->mtame && !rn2(5)) {
			badpeteffect(magr);
		}
		break;

	    case AD_MCRE:
		if (magr->mtame && !rn2(3)) {
			badpeteffect(magr);
		}
		break;

	    case AD_DRLI:
		if (!resists_drli(magr)) {
			if (canseemon(magr))
			    pline("%s suddenly seems weaker!", Monnam(magr));
			if (magr->m_lev == 0)
				tmp = magr->mhp;
			else magr->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		break;

	    case AD_STTP:
		if (magr->mtame) {
			badpeteffect(magr);
			badpeteffect(magr);
		}
		break;

	    case AD_TIME:
		if (!resists_drli(magr)) {
			if (canseemon(magr))
			    pline("%s suddenly seems weaker!", Monnam(magr));
			if (magr->m_lev == 0)
				tmp = magr->mhp;
			else magr->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		if (magr->mtame && !rn2(2)) badpeteffect(magr);
		break;

	    case AD_DFOO:
		if (!resists_drli(magr)) {
			if (canseemon(magr))
			    pline("%s suddenly seems weaker!", Monnam(magr));
			if (magr->m_lev == 0)
				tmp = magr->mhp;
			else magr->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		if (magr->mtame) {
			if (magr->mhpmax > 1) magr->mhpmax -= rnd(8);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			if (magr->m_lev > 0) magr->m_lev--;
		}
		break;

	    case AD_WEEP:
		if (!resists_drli(magr)) {
			if (canseemon(magr))
			    pline("%s suddenly seems weaker!", Monnam(magr));
			if (magr->m_lev == 0)
				tmp = magr->mhp;
			else magr->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		if (magr->mtame && !rn2(50)) magr->willbebanished = 1;
		break;

	    case AD_VAMP:
		if (!resists_drli(magr)) {
			if (canseemon(magr))
			    pline("%s suddenly seems weaker!", Monnam(magr));
			if (magr->m_lev == 0)
				tmp = magr->mhp;
			else magr->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		break;
	    case AD_VENO:
		if (resists_poison(magr) && !player_will_pierce_resistance()) {
			if (canseemon(magr))
			    pline_The("poison doesn't seem to affect %s.",
				mon_nam(magr));
		} else {
			if (canseemon(magr)) pline("%s is badly poisoned!", Monnam(magr));
			if (rn2(10) || resists_poison(magr)) tmp += rn1(20,12);
			else {
			    if (canseemon(magr)) pline_The("poison was deadly...");
			    tmp = magr->mhp;
			}
		}
		if (magr->mtame) {
			badpeteffect(magr);
			badpeteffect(magr);
			if (magr->mhpmax > 1) magr->mhpmax -= rnd(8);
			if (magr->mhpmax < 1) magr->mhpmax = 1;
			if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;

		}
		break;


	    default: /*tmp = 0;*/
		break;
	}
	else tmp = 0;

    assess_dmg:

	if (Race_if(PM_BOVER) && u.usteed && (magr == u.usteed) && tmp > 1) tmp /= 2;
	if (Race_if(PM_CARTHAGE) && u.usteed && (magr == u.usteed) && (mcalcmove(u.usteed) < 12) && tmp > 1) tmp /= 2;

	if((magr->mhp -= tmp) <= 0) {
		/* get experience from spell creatures */
		if (mdef->uexp) mon_xkilled(magr, "", (int)mddat->mattk[i].adtyp);
		else monkilled(magr, "", (int)mddat->mattk[i].adtyp);

		return (mdead | mhit | MM_AGR_DIED);
	}
	return (mdead | mhit);
}

/* "aggressive defense"; what type of armor prevents specified attack
   from touching its target? */
long
attk_protection(aatyp)
int aatyp;
{
    long w_mask = 0L;

    switch (aatyp) {
    case AT_NONE:
    case AT_RATH:
    case AT_SPIT:
    case AT_EXPL:
    case AT_BOOM:
    case AT_GAZE:
    case AT_BREA:
    case AT_MAGC:
    case AT_BEAM:
	w_mask = ~0L;		/* special case; no defense needed */
	break;
    case AT_CLAW:
    case AT_TUCH:
    case AT_WEAP:
	w_mask = W_ARMG;	/* caller needs to check for weapon */
	break;
    case AT_KICK:
	w_mask = W_ARMF;
	break;
    case AT_BUTT:
	w_mask = W_ARMH;
	break;
    case AT_HUGS:
	w_mask = (W_ARMC|W_ARMG); /* attacker needs both to be protected */
	break;
    case AT_BITE:
    case AT_STNG:
    case AT_LASH:
    case AT_TRAM:
    case AT_SCRA:
    case AT_ENGL:
    case AT_TENT:
    default:
	w_mask = 0L;		/* no defense available */
	break;
    }
    return w_mask;
}

STATIC_PTR void
set_lit(x,y,val)
int x, y;
void * val;
{
	if (val)
	    levl[x][y].lit = 1;
	else {
	    levl[x][y].lit = 0;
	    snuff_light_source(x, y);
	}
}

/* have the stooges say something funny */
STATIC_OVL void
stoogejoke()
{
	verbalize("%s", random_joke[rn2(SIZE(random_joke))]);
}

#endif /* OVLB */

/*mhitm.c*/

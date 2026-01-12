/*	SCCS Id: @(#)apply.c	3.4	2003/11/18	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h"

#ifdef OVLB

static const char tools[] = { TOOL_CLASS, WEAPON_CLASS, WAND_CLASS, 0 };
static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static const char allowxall[] = { ALL_CLASSES, 0 };
static const char tools_too[] = { ALL_CLASSES, TOOL_CLASS, POTION_CLASS,
				  WEAPON_CLASS, WAND_CLASS, GEM_CLASS, 0 };
static const char tinnables[] = { ALLOW_FLOOROBJ, FOOD_CLASS, 0 };
static NEARDATA const char recharge_type[] = { ALLOW_COUNT, ALL_CLASSES, 0 };

STATIC_DCL int use_camera(struct obj *);
STATIC_DCL int use_towel(struct obj *);
STATIC_DCL boolean its_dead(int,int,int *);
STATIC_DCL int use_stethoscope(struct obj *);
STATIC_DCL int use_symbiote(struct obj *);
STATIC_DCL void use_whistle(struct obj *);
STATIC_DCL void use_magic_whistle(struct obj *);
STATIC_DCL void use_dark_magic_whistle(struct obj *);
STATIC_DCL void use_leash(struct obj *);
STATIC_DCL int use_mirror(struct obj *);
STATIC_DCL int use_bell(struct obj **);
STATIC_DCL void use_candelabrum(struct obj *);
STATIC_DCL void use_candle(struct obj **);
STATIC_DCL void use_lamp(struct obj *);
STATIC_DCL int use_torch(struct obj *);
STATIC_DCL void light_cocktail(struct obj *);
STATIC_DCL void use_tinning_kit(struct obj *);
STATIC_DCL void use_binning_kit(struct obj *);
STATIC_DCL void use_figurine(struct obj **);
STATIC_DCL void use_grease(struct obj *);
STATIC_DCL void use_trap(struct obj *);
STATIC_DCL void use_stone(struct obj *);
STATIC_PTR int set_trap(void);		/* occupation callback */
STATIC_DCL int use_whip(struct obj *);
STATIC_DCL int use_pole(struct obj *);
STATIC_DCL int use_cream_pie(struct obj *);
STATIC_DCL int use_grapple(struct obj *);
STATIC_DCL int do_break_wand(struct obj *);
STATIC_DCL boolean figurine_location_checks(struct obj *, coord *, BOOLEAN_P);
STATIC_DCL boolean uhave_graystone(void);
STATIC_DCL void add_class(char *, CHAR_P);

#ifdef	AMIGA
void amii_speaker(struct obj *, char *, int );
#endif

const char no_elbow_room[] = "don't have enough elbow-room to maneuver.";

STATIC_OVL int
use_camera(obj)
	struct obj *obj;
{
	register struct monst *mtmp;
	boolean useagain = FALSE;

	if(Underwater) {
		pline(FunnyHallu ? "You see tons of little fishes, jellyfish, seaweed and submarines..." : "Using your camera underwater would void the warranty.");
		return(0);
	}

usecameraagain:

	if(!getdir((char *)0)) return(0);

	if (obj->spe <= 0) {
		pline("%s", nothing_happens);
		if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
		return (1);
	}
	if (obj->oartifact == ART_LIGHTS__CAMERA__ACTION) {
		register struct monst *mtmp2;
		for(mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {
		    if (DEADMONSTER(mtmp2)) continue;
		    if(cansee(mtmp2->mx,mtmp2->my))
			  monflee(mtmp2, rnd(10), FALSE, FALSE, FALSE);
		}
	}

	int nochargechange = 10;
	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_DEVICES)) {
			default: break;
			case P_BASIC: nochargechange = 9; break;
			case P_SKILLED: nochargechange = 8; break;
			case P_EXPERT: nochargechange = 7; break;
			case P_MASTER: nochargechange = 6; break;
			case P_GRAND_MASTER: nochargechange = 5; break;
			case P_SUPREME_MASTER: nochargechange = 4; break;
		}
	}

	if (!useagain && (nochargechange >= rnd(10))) consume_obj_charge(obj, TRUE);

	if (obj->cursed && !rn2(2)) {
		(void) zapyourself(obj, TRUE, FALSE);
	} else if (u.uswallow) {
		You("take a picture of %s %s.", s_suffix(mon_nam(u.ustuck)),
		    mbodypart(u.ustuck, STOMACH));
	} else if (u.dz) {
		You("take a picture of the %s.",
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
	} else if (!u.dx && !u.dy) {
		(void) zapyourself(obj, TRUE, FALSE);
	} else if ((mtmp = bhit(u.dx,u.dy,COLNO,FLASHED_LIGHT,
				(int (*)(MONST_P,OBJ_P))0,
				(int (*)(OBJ_P,OBJ_P))0,
				&obj, TRUE)) != 0) {
		obj->ox = u.ux,  obj->oy = u.uy;
		(void) flash_hits_mon(mtmp, obj);
	}
	if (obj->oartifact == ART_LIGHTS__CAMERA__ACTION) pline("Your flash scares nearby monsters!");
	use_skill(P_DEVICES,1);
	if (Race_if(PM_FAWN)) {
		use_skill(P_DEVICES,1);
	}
	if (Race_if(PM_SATRE)) {
		use_skill(P_DEVICES,1);
		use_skill(P_DEVICES,1);
	}

	if (practicantterror) {
		pline("%s rings out: 'Photography is strictly forbidden in the lab, and therefore you pay 100 zorkmids.'", noroelaname());
		fineforpracticant(100, 0, 0);
	}

	if (obj->oartifact == ART_GOODFILM && !useagain) {
		useagain = TRUE;
		You("can take another photo.");
		goto usecameraagain;
	}

	return 1;
}

STATIC_OVL int
use_towel(obj)
	struct obj *obj;
{
	if(!freehandX()) {
		You("have no free %s!", body_part(HAND));
		return 0;
	} else if (obj->owornmask) {
		You(FunnyHallu ? "cannot get that sticky thing off!" : "cannot use it while you're wearing it!");
		return 0;
	} else if (obj->cursed) {
		long old;
		switch (rn2(3)) {
		case 2:
		    old = Glib;
		    incr_itimeout(&Glib, rn1(10, 3));
		    flags.botl = TRUE;
		    Your("%s %s!", makeplural(body_part(HAND)),
			(old ? "are filthier than ever" : "get slimy"));
		    return 1;
		case 1:
		    if (!ublindf) {
			old = u.ucreamed;
			u.ucreamed += rn1(10, 3);
			pline("Yecch! Your %s %s gunk on it!", body_part(FACE),
			      (old ? "has more" : "now has"));
			make_blinded(Blinded + (long)u.ucreamed - old, TRUE);
		    } else {
			const char *what = (ublindf->otyp == LENSES || ublindf->otyp == RADIOGLASSES || ublindf->otyp == SHIELD_PATE_GLASSES || ublindf->otyp == BOSS_VISOR || ublindf->otyp == NIGHT_VISION_GOGGLES) ?
					    "lenses" : "blindfold";
			if (ublindf->cursed) {
			    You("push your %s %s.", what,
				rn2(2) ? "cock-eyed" : "crooked");
			} else {
			    struct obj *saved_ublindf = ublindf;
			    You("push your %s off.", what);
			    Blindf_off(ublindf, TRUE);
			    dropx(saved_ublindf);
			}
		    }
		    return 1;
		case 0:
		    break;
		}
	}

	if (obj && obj->oartifact == ART_ANSWER_IS___) {
		badeffect();
	}

	if (Glib) {
		if ((obj && obj->blessed) || rn2(2)) {
			Glib = 0;
			You("wipe off your %s.", makeplural(body_part(HAND)));
		} else You("wipe off your %s, but there's still gunk on it.", makeplural(body_part(HAND)));

		if (!rn2(isfriday ? 50 : 100) && obj) {
			if (obj->blessed)
				unbless(obj);
			else
				curse(obj);
			pline("Your towel glows %s.", hcolor(obj->cursed ? "black" : "brown"));
			obj->bknown = 1;

		}

		return 1;
	} else if(u.ucreamed) {
		if ((obj && obj->blessed) || rn2(2)) {
			Blinded -= u.ucreamed;
			u.ucreamed = 0;

			if (!Blinded) {
				pline("You've got the glop off.");
				Blinded = 1;
				make_blinded(0L,TRUE);
			} else {
				Your("%s feels clean now.", body_part(FACE));
			}
		} else Your("%s still has glop on it.", body_part(FACE));

		if (!rn2(isfriday ? 50 : 100) && obj) {
			if (obj->blessed)
				unbless(obj);
			else
				curse(obj);
			pline("Your towel glows %s.", hcolor(obj->cursed ? "black" : "brown"));
			obj->bknown = 1;

		}

		return 1;
	}

	Your("%s and %s are already clean.",
		body_part(FACE), makeplural(body_part(HAND)));

	return 0;
}

/* maybe give a stethoscope message based on floor objects */
STATIC_OVL boolean
its_dead(rx, ry, resp)
int rx, ry, *resp;
{
	struct obj *otmp;
	struct trap *ttmp;

	if (!can_reach_floor()) return FALSE;

	/* additional stethoscope messages from jyoung@apanix.apana.org.au */
	if (FunnyHallu && sobj_at(CORPSE, rx, ry)) {
	    /* (a corpse doesn't retain the monster's sex,
	       so we're forced to use generic pronoun here) */
	    You_hear("a voice say, \"It's dead, Jim.\"");
	    *resp = 1;
	    return TRUE;
	} else if ( (Role_if(PM_HEALER) || Race_if(PM_HERBALIST)) && ((otmp = sobj_at(CORPSE, rx, ry)) != 0 ||
				    (otmp = sobj_at(STATUE, rx, ry)) != 0)) {
	    /* possibly should check uppermost {corpse,statue} in the pile
	       if both types are present, but it's not worth the effort */
	    if (vobj_at(rx, ry)->otyp == STATUE) otmp = vobj_at(rx, ry);
	    if (otmp->otyp == CORPSE) {
		You("determine that %s unfortunate being is dead.",
		    (rx == u.ux && ry == u.uy) ? "this" : "that");
	    } else {
		ttmp = t_at(rx, ry);
		pline("%s appears to be in %s health for a statue.",
		      The(mons[otmp->corpsenm].mname),
		      (ttmp && (ttmp->ttyp == STATUE_TRAP || ttmp->ttyp == SATATUE_TRAP)) ?
			"extraordinary" : "excellent");
	    }
	    return TRUE;
	}
	/* listening to eggs is a little fishy, but so is stethoscopes detecting alignment
	 * The overcomplex wording is because all the monster-naming functions operate
	 * on actual instances of the monsters, and we're dealing with just an index
	 * so we can avoid things like "a owlbear", etc. */
	if ((otmp = sobj_at(EGG,rx,ry)) != 0) {
		if (issoviet && rn2(10)) {
			pline("Sovetskaya skazal, chto ne mozhet vernut' nedopustimoye monstra, no ya vernus' iskazhennyy yego pryamo seychas!");
			pline("You listen to the egg and guess... %s?", generate_garbage_string() );
		} else if (Hallucination || !rn2(20) ) { /* Let's make it fail sometimes. --Amy */
			pline("You listen to the egg and guess... %s?",rndmonnam());
		} else {
			if (stale_egg(otmp) || otmp->corpsenm == NON_PM || rn2(2) ) {
				pline("The egg doesn't make much noise at all.");
			} else {
				pline("You listen to the egg and guess... %s?",mons[otmp->corpsenm].mname);
			}
		}
		return TRUE;
	}

	return FALSE;
}

static const char hollow_str[] = "a hollow sound.  This must be a secret %s!";

STATIC_OVL int
use_symbiote(obj)
	register struct obj *obj;
{
	struct monst *mtmp;
	int rx, ry, res;
	int symchecks = 1;
	int symdiff = 50;

	if (obj->oartifact == ART_XOM_S_SCROLLINATOR) {

		if (uinsymbiosis) {

			if (uarmf && itemhasappearance(uarmf, APP_REMORA_HEELS) && u.usymbiote.mnum == PM_REMORA) {
				if (uarmf->spe > -1) uarmf->spe = -1;
			}

			u.shutdowntime = 0;
			u.usymbiote.active = 0;
			u.usymbiote.mnum = PM_PLAYERMON;
			u.usymbiote.mhp = 0;
			u.usymbiote.mhpmax = 0;
			u.usymbiote.cursed = u.usymbiote.hvycurse = u.usymbiote.prmcurse = u.usymbiote.bbcurse = u.usymbiote.morgcurse = u.usymbiote.evilcurse = u.usymbiote.stckcurse = 0;
			if (flags.showsymbiotehp) flags.botl = TRUE;
			u.cnd_symbiotesdied++;
			Your("old symbiote was erased!");
		}
		getrandomsymbiote(FALSE, FALSE);
		useup(obj);
		return 1;
	}

	if (!getdir((char *)0)) return 0;

	if (u.uswallow) {
		if (FunnyHallu) pline("What? With all that slime around you? Forget it!");
		else pline_The("monster that engulfs you is preventing the symbiosis from working.");
		return 1;
	}
	if (u.usteed && u.dz > 0) {
		pline(FunnyHallu ? "Trying to turn your car into a symbiote isn't very fruitful." : "Trying to turn your steed into a symbiote isn't very fruitful.");
		return 1;
	}
	if (!u.dx && !u.dy && !u.dz) {
		pline(FunnyHallu ? "Your body seems to turn inward on itself, but relaxes again." : "You can't turn yourself into a symbiote!");
		return 1;
	}
	rx = u.ux + u.dx; ry = u.uy + u.dy;
	if (!isok(rx,ry)) {
		pline(FunnyHallu ? "A wild MISSINGNO appeared! Too bad you don't have a pokeball to catch it though..." : "Grabbing a glitched symbiote from out of bounds doesn't seem to work.");
		return 1;
	}
	if ((mtmp = m_at(rx,ry)) != 0) {
		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_SYMBIOSIS)) {
				default: break;
				case P_BASIC: if (!rn2(3)) symchecks++; symdiff = 40; break;
				case P_SKILLED: if (!rn2(3)) symchecks += rno(2); symdiff = 30; break;
				case P_EXPERT: if (!rn2(3)) symchecks += rnd(2); symdiff = 20; break;
				case P_MASTER: if (!rn2(2)) symchecks += rnd(2); symdiff = 10; break;
				case P_GRAND_MASTER: if (!rn2(2)) symchecks += rnd(3); symdiff = 0; break;
				case P_SUPREME_MASTER: symchecks += rnd(3); symdiff = 0; break;
			}
		}

		/* normally the monster must be nonmoving, but for goauld (invented by Demo) it's the other way around --Amy */
		if ((!(stationary(mtmp->data) || mtmp->data->mmove == 0 || mtmp->data->mlet == S_TURRET)) && !Race_if(PM_GOAULD)) {
			pline(FunnyHallu ? "It moves around too much and you can't seem to catch it!" : "Only stationary monsters can be turned into symbiotes!");
			return 1;

		}
		if (((stationary(mtmp->data) || mtmp->data->mmove == 0 || mtmp->data->mlet == S_TURRET)) && Race_if(PM_GOAULD)) {
			pline(FunnyHallu ? "It doesn't move at all and so doesn't want to enter you!" : "Only non-stationary monsters can be turned into symbiotes!");
			return 1;

		}
		/* arabella monsters *are* eligible for symbiosis... but you have to get lucky to receive them randomly,
		 * you can't just misuse arabella rooms as a ready source for ubersymbiotes! --Amy */
		if (is_arabellamonster(mtmp->data)) {
			pline(FunnyHallu ? "Oh joy, the monster is telling you it would rather like to party with you! Seems like you have to abandon your attempt!" : "Some sinister force prevents this monster from becoming a symbiote.");
			return 1;
		}
		if (mtmp->mfrenzied) {
			if (FunnyHallu) pline("Suddenly the monster rakes its claws over your %s and reveals that you're indeed noble because you're bleeding blue %s.", body_part(BODY_SKIN), body_part(BLOOD));
			else pline("Frenzied monsters will never turn into symbiotes.");
			return 1;

		}
		if (cannot_be_tamed(mtmp->data) || (mtmp->cham == CHAM_ZRUTINATOR) || mtmp->isshk || mtmp->isgd || mtmp->ispriest || mtmp->isminion || mtmp->isgyp || (mtmp->oldmonnm != monsndx(mtmp->data)) ) {
			pline(FunnyHallu ? "This particular monster can't be turned into a symbiote for cookie reasons." : "This particular monster can't be turned into a symbiote.");
			return 1;

		if (mtmp->data->mlevel > 29 || (mtmp->data->mlevel > (u.ulevel + 10))) {
			pline(FunnyHallu ? "The damn monster is fighting back!" : "This monster's level is too high for symbiosis!");
			return 1;
		}

		/* Amy note: the oldmonnm part may be misleading, but I don't want the player to be able to scan monsters for
		 * being a polymorph result just because they have a symbiote item. And I also don't want to allow you to
		 * simply wait until that chameleon turns into a stationary level 90 ubermonster and get a free pass,
		 * so I had to ban polymorphed monsters from being made into symbiotes. */
		}

		/* At this point we've established that the monster is a valid form for symbiosis. */

		if (uinsymbiosis && u.usymbiote.cursed) {
			pline(FunnyHallu ? "Apparently Morgoth himself decided to curse you with some ancient hex." : "Since your current symbiote is cursed, you cannot get a new one.");
			return 1;
		}

		while (symchecks > 0) {
			symchecks--;
			int resistrounds = 1;
			if (!mindless(mtmp->data)) resistrounds++;
			if (humanoid(mtmp->data)) resistrounds++;

			/* Amy note: for tame monsters it simply works, every time, and also gives a special boost */
			if (!mtmp->mtame && resist(mtmp, TOOL_CLASS, 0, 0) && (!Role_if(PM_SYMBIANT) || resist(mtmp, TOOL_CLASS, 0, 0) ) ) continue;
			if (!mtmp->mtame && resistrounds >= 2 && resist(mtmp, TOOL_CLASS, 0, 0) && ((!Role_if(PM_SYMBIANT) || resist(mtmp, TOOL_CLASS, 0, 0) ) ) ) continue;
			if (!mtmp->mtame && resistrounds >= 3 && resist(mtmp, TOOL_CLASS, 0, 0) && ((!Role_if(PM_SYMBIANT) || resist(mtmp, TOOL_CLASS, 0, 0) ) ) )  continue;
			if (!mtmp->mtame && mtmp->m_lev > u.ulevel && (rn2(100) < (mtmp->m_lev - u.ulevel) ) ) continue;

			/* it shouldn't be guaranteed just because the monster has no MR, except if you're very skilled --Amy */
			if (!mtmp->mtame && symdiff > rn2(100)) continue;
			if (!mtmp->mtame && humanoid(mtmp->data) && symdiff > rn2(100)) continue;

			/* we caught it! */
			symchecks = 0;

			if (touch_petrifies(mtmp->data) && (!Stone_resistance || (!IntStone_resistance && !rn2(20))) ) {
				if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
					display_nhwindow(WIN_MESSAGE, FALSE);
				else {
					static char kbuf[BUFSZ];
					pline("Incorporating a petrifying creature is a fatal mistake.");
					sprintf(kbuf, "picking the wrong symbiote");
					instapetrify(kbuf);
				}

			}

			if (slime_on_touch(mtmp->data) && !Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data)) {
				You("don't feel very well.");
				make_slimed(100);
				killer_format = KILLED_BY_AN;
				delayed_killer = "slimed by picking the wrong symbiote";

			}

			if (mtmp->mtame) {
				use_skill(P_SQUEAKING,1);
				use_skill(P_PETKEEPING,1);
			}

			if (obj->oartifact == ART_POOLING) {
				if (uinsymbiosis) {

					struct monst *mtmp2;
					mtmp2 = makemon(&mons[u.usymbiote.mnum], u.ux, u.uy, 0);
					if (mtmp2) {
						mtmp2->mfrenzied = FALSE;
						mtmp2->mpeaceful = TRUE;
					}

					u.usymbiote.active = 0;
					u.usymbiote.mnum = PM_PLAYERMON;
					u.usymbiote.mhp = 0;
					u.usymbiote.mhpmax = 0;
					u.usymbiote.cursed = u.usymbiote.hvycurse = u.usymbiote.prmcurse = u.usymbiote.bbcurse = u.usymbiote.morgcurse = u.usymbiote.evilcurse = u.usymbiote.stckcurse = 0;

				}

			}

			turnmonintosymbiote(mtmp, (obj->oartifact == ART_HOLDEN_MIDDLE_POST));

			if (obj->oartifact == ART_GIMMIE_DAT_SKILL) use_skill(P_SYMBIOSIS, 50);

			if (uinsymbiosis) {
				if (obj->cursed) u.usymbiote.cursed = TRUE;
				if (obj->hvycurse) u.usymbiote.hvycurse = TRUE;
				if (obj->prmcurse) u.usymbiote.prmcurse = TRUE;
				if (obj->bbrcurse) u.usymbiote.bbcurse = TRUE;
				if (obj->morgcurse) u.usymbiote.morgcurse = TRUE;
				if (obj->evilcurse) u.usymbiote.evilcurse = TRUE;
				if (obj->stckcurse) u.usymbiote.stckcurse = TRUE;
				if (obj->blessed) {
					u.usymbiote.mhpmax *= 3;
					u.usymbiote.mhpmax /= 2;
					if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500; /* cap value */
				}
				if (obj->oartifact == ART_HEALTHY_PARASITE) {
					u.usymbiote.mhpmax *= 3;
					if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500; /* cap value */
				}
			}
			if (obj->oartifact == ART_SCIENTIFIC_SYMBIONT_KIT && rn2(5)) {
				pline("You use some material from the kit.");
			} else {
				useup(obj);
			}
			return 1;
		}

		int frenzychance = 33;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_SYMBIOSIS)) {
				default: break;
				case P_BASIC: frenzychance = 30; break;
				case P_SKILLED: frenzychance = 25; break;
				case P_EXPERT: frenzychance = 20; break;
				case P_MASTER: frenzychance = 15; break;
				case P_GRAND_MASTER: frenzychance = 10; break;
				case P_SUPREME_MASTER: frenzychance = 7; break;
			}
		}
		if (frenzychance > rn2(100)) {
			mtmp->mtame = FALSE;
			mtmp->mpeaceful = FALSE;
			mtmp->mfrenzied = TRUE;
		}

		pline(FunnyHallu ? "The stupid pokeball is apparently made of plastic because the monster easily broke out!" : "Your symbiosis attempt failed.");
		if (mtmp->mfrenzied) pline(FunnyHallu ? "And in fact the monster now wants to EAT you! Quick, run back to Professor Oak so he can save you!" : "Now the monster is frenzied and you cannot use symbiosis on it anymore.");

		/* it's stupid if you can just try indefinitely... --Amy
		 * but artifacts should be guaranteed to work eventually, rather than being destroyed */
		if (obj && !rn2(10) && !obj->oartifact) {
			useup(obj);
			pline("Sadly, your symbiote token has become unusable due to your failed attempt...");
		}

		return 1;

	}
	pline(FunnyHallu ? "Knock-knock - hmmmm, seems there's nobody home..." : "There is no monster at that location!");
	return 1;
}

/* Strictly speaking it makes no sense for usage of a stethoscope to
   not take any time; however, unless it did, the stethoscope would be
   almost useless.  As a compromise, one use per turn is free, another
   uses up the turn; this makes curse status have a tangible effect. */
STATIC_OVL int
use_stethoscope(obj)
	register struct obj *obj;
{
	static long last_used_move = -1;
	static short last_used_movement = 0;
	struct monst *mtmp;
	struct rm *lev;
	int rx, ry, res;
	boolean interference = (u.uswallow && is_whirly(u.ustuck->data) &&
				!rn2(Role_if(PM_HEALER) ? 10 : Race_if(PM_HERBALIST) ? 10 : 3));

	if ((!rn2(20) || !obj->blessed) && !((obj->oartifact == ART_DOCTOR_S_ASSISTANT) && rn2(10)) && !rn2((obj->oartifact == ART_FISSILITY) ? 50 : 500)) {
	    useup(obj);
	    pline("Your stethoscope breaks!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 2;
		}

	if ((!rn2(20) || !obj->blessed) && isfriday && !((obj->oartifact == ART_DOCTOR_S_ASSISTANT) && rn2(10)) && !rn2((obj->oartifact == ART_FISSILITY) ? 50 : 500)) {
	    useup(obj);
	    pline("Your stethoscope breaks!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 2;
		}

	if (!rn2(isfriday ? 30 : 100)) {
		if (obj->blessed) unbless(obj);
		else curse(obj);
		pline("Your stethoscope seems less effective.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");

	}

	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) {	/* should also check for no ears and/or deaf */
		You("have no hands, so you can't use the stethoscope!");	/* not `body_part(HAND)' */
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		if (yn("Attempt it anyway? Warning: this can fail and cause bleeding.") == 'y') {
			if (rn2(3) && !polyskillchance()) {
				playerbleed(rnd(2 + (level_difficulty() * 10)));
				pline("Great. Now your %s is squirting everywhere.", body_part(BLOOD));
				if (!rn2(20)) badeffect();
				return 1;
			}

		}
		else return 0;
	}
	if (!freehandX()) {
		You("have no free %s.", body_part(HAND));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}
	if (!getdir((char *)0)) return 0;

	res = (moves == last_used_move) &&
	      (youmonst.movement == last_used_movement);
	last_used_move = moves;
	last_used_movement = youmonst.movement;
	if (u.stethocheat == moves) res = 1; /* just restored the game and trying to cheat? Nice try. --Amy */

	if (u.usteed && u.dz > 0) {

		if (obj->oartifact == ART_PIERCE_DEVICE && u.usteed->mhp > 1) u.usteed->mhp--;

		if (interference) {
			pline("%s interferes.", Monnam(u.ustuck));

			if (obj->oartifact == ART_SARINA_S_EXAM)
				probe_monster(u.ustuck);
			else if ((obj->blessed || (obj->otyp == UNSTABLE_STETHOSCOPE && !rn2(5)) || obj->oartifact == ART_MEDICAL_OPHTHALMOSCOPE) && !issoviet)
				mstatuslinebl(u.ustuck);
			else
				mstatusline(u.ustuck);

		} else

			if (obj->oartifact == ART_SARINA_S_EXAM)
				probe_monster(u.usteed);
			else if ((obj->blessed || (obj->otyp == UNSTABLE_STETHOSCOPE && !rn2(5)) || obj->oartifact == ART_MEDICAL_OPHTHALMOSCOPE) && !issoviet)
				mstatuslinebl(u.usteed); /* make blessed one better than uncursed --Amy */
			else
				mstatusline(u.usteed);

		return res;
	} else
	if (u.uswallow && (u.dx || u.dy || u.dz)) {

		if (obj->oartifact == ART_PIERCE_DEVICE && u.ustuck->mhp > 1) {
			u.ustuck->mhp--;
			if (u.ustuck->mpeaceful && !u.ustuck->mtame) { /* exploit discovered by amateurhour --Amy */
				u.ustuck->mpeaceful = FALSE; /* no easy shopkeeper cheesing! */
				pline("%s is no longer peaceful.", Monnam(u.ustuck));
			}
		}

		if (obj->oartifact == ART_SARINA_S_EXAM)
			probe_monster(u.ustuck);
		else if ((obj->blessed || (obj->otyp == UNSTABLE_STETHOSCOPE && !rn2(5)) || obj->oartifact == ART_MEDICAL_OPHTHALMOSCOPE) && !issoviet)
			mstatuslinebl(u.ustuck);
		else
			mstatusline(u.ustuck);
		return res;
	} else if (u.uswallow && interference) {
		pline("%s interferes.", Monnam(u.ustuck));

		if (obj->oartifact == ART_PIERCE_DEVICE && u.ustuck->mhp > 1) {
			u.ustuck->mhp--;
			if (u.ustuck->mpeaceful && !u.ustuck->mtame) { /* exploit discovered by amateurhour --Amy */
				u.ustuck->mpeaceful = FALSE; /* no easy shopkeeper cheesing! */
				pline("%s is no longer peaceful.", Monnam(u.ustuck));
			}
		}

		if (obj->oartifact == ART_SARINA_S_EXAM)
			probe_monster(u.ustuck);
		else if ((obj->blessed || (obj->otyp == UNSTABLE_STETHOSCOPE && !rn2(5)) || obj->oartifact == ART_MEDICAL_OPHTHALMOSCOPE) && !issoviet)
			mstatuslinebl(u.ustuck);
		else
			mstatusline(u.ustuck);
		return res;
	} else if (u.dz) {
		if (Underwater)
		    You_hear("faint splashing.");
		else if (u.dz < 0 || !can_reach_floor())
		    You_cant("reach the %s.",
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
		else if (its_dead(u.ux, u.uy, &res))
		    ;	/* message already given */
		else if (Is_stronghold(&u.uz))
		    You_hear("the crackling of hellfire.");
		else
		    pline_The("%s seems healthy enough.", surface(u.ux,u.uy));
		return res;
	} else if (obj->cursed && !rn2(2)) {
		if (obj->otyp == STETHOSCOPE) You_hear("your %s %s.", body_part(HEART), body_part(BEAT));
		else {
			pline_The("stethoscope pierces your %s!", body_part(HEART));
			if (u.uhpmax > 0) u.uhpmax--;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (Upolyd) {
				u.mhmax--;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			}
			losehp(rnd(u.ulevel * 5), "evil stethoscope", KILLED_BY_AN);
		}
		return res;
	}
	if ((Stunned && !rn2(issoviet ? 1 : StrongStun_resist ? 20 : Stun_resist ? 8 : 2)) || (Confusion && !rn2(issoviet ? 2 : StrongConf_resist ? 200 : Conf_resist ? 40 : 8))) confdir();
	if (!u.dx && !u.dy) {
		ustatusline();
		return res;
	}
	rx = u.ux + u.dx; ry = u.uy + u.dy;
	if (!isok(rx,ry)) {
		You_hear("a faint typing noise.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Eto kakoy-to durak sidit pered pryamougol'nym veshchi." : "tipptipptipptipptipp");
		return 0;
	}
	if ((mtmp = m_at(rx,ry)) != 0) {

		if (mtmp->mnum == PM_DARK_GOKU || mtmp->mnum == PM_FRIEZA) { /* idea by Bug Sniper */
	    useup(obj);
	    pline("Your stethoscope breaks, and you scream in terror!");
		wake_nearby();
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 2;
		}

		if (evilfriday && dmgtype(mtmp->data, AD_RUST)) {
			erode_obj(obj, FALSE, FALSE);
			return 0;
		}

		if (obj->oartifact == ART_PIERCE_DEVICE && mtmp->mhp > 1) {
			mtmp->mhp--;
			if (mtmp->mpeaceful && !mtmp->mtame) { /* exploit discovered by amateurhour --Amy */
				mtmp->mpeaceful = FALSE; /* no easy shopkeeper cheesing! */
				pline("%s is no longer peaceful.", Monnam(mtmp));
			}
		}

		if (obj->oartifact == ART_SARINA_S_EXAM)
			probe_monster(mtmp);
		else if ((obj->blessed || (obj->otyp == UNSTABLE_STETHOSCOPE && !rn2(5)) || obj->oartifact == ART_MEDICAL_OPHTHALMOSCOPE) && !issoviet)
			mstatuslinebl(mtmp);
		else
			mstatusline(mtmp);
		if (obj->blessed && issoviet) pline("Sovetskaya ne khochet stetoskopa, chtoby byt' poleznym.");

		if (mtmp->mundetected) {
			mtmp->mundetected = 0;
			if (cansee(rx,ry)) newsym(mtmp->mx,mtmp->my);
		}
		if (!canspotmon(mtmp) && !(monstersoundtype(mtmp) == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember) )
			map_invisible(rx,ry);
		return res;
	}
	if (memory_is_invisible(rx, ry)) {
		unmap_object(rx, ry);
		newsym(rx, ry);
		FunnyHallu ? pline("No one there! Oh no, where have they gone?") : pline_The("invisible monster must have moved.");
	}
	lev = &levl[rx][ry];
	switch(lev->typ) {
	case SDOOR:
		You_hear(hollow_str, "door");
		cvt_sdoor_to_door(lev);		/* ->typ = DOOR */
		if (Blind) feel_location(rx,ry);
		else newsym(rx,ry);
		return res;
	case SCORR:
		You_hear(hollow_str, "passage");
		lev->typ = CORR;
		unblock_point(rx,ry);
		if (Blind) feel_location(rx,ry);
		else newsym(rx,ry);
		return res;
	}

	if (!its_dead(rx, ry, &res))
	    You(FunnyHallu ? "hear something special." : "hear nothing special.");	/* not You_hear()  */
	return res;
}

static const char whistle_str[] = "produce a %s whistling sound.";

STATIC_OVL void
use_whistle(obj)
struct obj *obj;
{

	register struct monst *mtmp;

	You(whistle_str, obj->cursed ? "shrill" : "high");

	if (obj->cursed) {
				/* watchmen don't like that --Amy */
				awaken_soldiers(0);
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vse bodrstvuyet. Otlichno srabotano." : "KRIIIIIIIIII!");
	} else {
				if (PlayerHearsSoundEffects) pline(issoviet ? "Arbitr svistnul, dazhe yesli on ne imeyet svistok." : "Pfiiiiiiet!");
	}
	wake_nearby();
	if (obj->oartifact == ART_LOUDNESS_AMPLIFIER) wake_nearby();

	if (obj->oartifact == ART_GUANTANAMERA) {
		pline("The whistle plays a lullaby...");

		mtmp = fmon;

		while(mtmp) {
			if (!DEADMONSTER(mtmp) && distu(mtmp->mx, mtmp->my) < 10 &&
				sleep_monst(mtmp, d(10,10), TOOL_CLASS)) {
			    mtmp->msleeping = 1; /* 10d10 turns + wake_nearby to rouse */
			    slept_monst(mtmp);
			}
			mtmp = mtmp->nmon;
		}

		fall_asleep(-rnd(10), TRUE);
		You("are put to sleep!");

	}

	if (obj->cursed || !rn2(obj->blessed ? 200 : 50) ) { /* shrill whistling sound wakes up the entire level */

		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (!DEADMONSTER(mtmp)) {
			mtmp->msleeping = 0;
			if (mtmp->mtame && !mtmp->isminion)
			    EDOG(mtmp)->whistletime = moves;
		    }
		}
	}

	if (obj->oartifact == ART_PET_COME_HERE_) {
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (!DEADMONSTER(mtmp)) {

			if (mtmp->mtame && !mtmp->isminion) {
				int whistletimer = moves - rn1(10, 10);
				if (whistletimer < 0) whistletimer = 0;
			    EDOG(mtmp)->whistletime = whistletimer;
			}
		    }
		}

	}

}

STATIC_OVL void
use_magic_whistle(obj)
struct obj *obj;
{
	register struct monst *mtmp, *nextmon;

	if(obj->cursed && !rn2((obj->oartifact == ART_LELI_DAB) ? 3 : 2)) {
		You(FunnyHallu ? "produce a grating, annoying sound." : "produce a high-pitched humming noise.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Potomu chto vy ne mozhete igrat' der'mo." : "Dueueueueue!");
		wake_nearby();
	} else if (!rn2(obj->blessed ? 200 : 50)) {
		You(FunnyHallu ? "produce a grating, annoying sound." : "produce a high-pitched humming noise.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Potomu chto vy ne mozhete igrat' der'mo." : "Dueueueueue!");
		wake_nearby();

	} else {
		int pet_cnt = 0;
		You(whistle_str, FunnyHallu ? "normal" : "strange");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe vy mozhete sdelat' chto-to pravil'no, v redkikh sluchayakh, eto kazhetsya." : "dueueueueue");
		for(mtmp = fmon; mtmp; mtmp = nextmon) {
		    nextmon = mtmp->nmon; /* trap might kill mon */
		    if (DEADMONSTER(mtmp)) continue;
		    if (mtmp == u.usteed) continue;
		    if (evilfriday && level.flags.noteleport) continue;
		    if (mtmp->mtame) {
			if (mtmp->mtrapped) {
			    /* no longer in previous trap (affects mintrap) */
			    mtmp->mtrapped = 0;
			    fill_pit(mtmp->mx, mtmp->my);
			}
			mnexto(mtmp);
			if (canspotmon(mtmp)) ++pet_cnt;
			if (mintrap(mtmp) == 2) change_luck(-1);
		    }
		}
		if (pet_cnt > 0) makeknown(obj->otyp);
	}
}

/* Dark magic whistle: idea by Amy, sends pets away instead of sending them to you */
STATIC_OVL void
use_dark_magic_whistle(obj)
struct obj *obj;
{
	register struct monst *mtmp, *nextmon;

	if(obj->cursed && !rn2(2)) {
		You(FunnyHallu ? "produce something that sounds like an elegy." : "produce a terrible whistling sound.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Odin dolzhen kamen' vy do smerti dlya etogo." : "Kwaaaaaaaa!");
		badeffect();
	} else if (!rn2(obj->blessed ? 200 : 50)) {
		You(FunnyHallu ? "produce something that sounds like an elegy." : "produce a terrible whistling sound.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Odin dolzhen kamen' vy do smerti dlya etogo." : "Kwaaaaaaaa!");
		badeffect();
	} else {
		You(whistle_str, FunnyHallu ? "soothing" : "terrifying");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vy pomnite, chto v kommunizm, igrayet muzyka zapreshchena?" : "Traaaaaaaa!");
		for(mtmp = fmon; mtmp; mtmp = nextmon) {
		    nextmon = mtmp->nmon; /* trap might kill mon */
		    if (DEADMONSTER(mtmp)) continue;
		    if (mtmp == u.usteed) continue;
		    if (evilfriday && level.flags.noteleport) continue;
		    if (!monnear(mtmp, u.ux, u.uy)) continue;
		    if (mtmp->mtame) {
			if (mtmp->mtrapped) {
			    /* no longer in previous trap (affects mintrap) */
			    mtmp->mtrapped = 0;
			    fill_pit(mtmp->mx, mtmp->my);
			}
			rloc(mtmp, FALSE);
		    }
		}
	}
}

boolean
um_dist(x,y,n)
register xchar x, y, n;
{
	return((boolean)(abs(u.ux - x) > n  || abs(u.uy - y) > n));
}

int
number_leashed()
{
	register int i = 0;
	register struct obj *obj;

	for(obj = invent; obj; obj = obj->nobj)
		if((obj->otyp == LEATHER_LEASH || obj->otyp == ARMORED_LEASH || obj->otyp == INKA_LEASH || obj->otyp == ADAMANT_LEASH) && obj->leashmon != 0) i++;
	return(i);
}

void
o_unleash(otmp)		/* otmp is about to be destroyed or stolen */
register struct obj *otmp;
{
	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		if(mtmp->m_id == (unsigned)otmp->leashmon)
			mtmp->mleashed = 0;
	otmp->leashmon = 0;
}

void
m_unleash(mtmp, feedback, leashbreakchance)	/* mtmp is about to die, or become untame */
register struct monst *mtmp;
boolean feedback;
int leashbreakchance; /* 0 = never break, other number = 1 in X chance to break --Amy */
{
	register struct obj *otmp;

	if (feedback) {
	    if (canseemon(mtmp))
		pline("%s pulls free of %s leash!", Monnam(mtmp), mhis(mtmp));
	    else
		Your("leash falls slack.");
	}
	for(otmp = invent; otmp; otmp = otmp->nobj)
		if((otmp->otyp == LEATHER_LEASH || otmp->otyp == ARMORED_LEASH || otmp->otyp == INKA_LEASH || otmp->otyp == ADAMANT_LEASH) &&
				otmp->leashmon == (int)mtmp->m_id) {
			otmp->leashmon = 0;
			if (leashbreakchance > 0) {
				if (!rn2(leashbreakchance) && (!otmp->oartifact || !rn2(5)) ) {
					useup(otmp);
					pline_The("leash is ripped apart!");
				}
			}
		}
	mtmp->mleashed = 0;
}

void
unleash_all()		/* player is about to die (for bones) */
{
	register struct obj *otmp;
	register struct monst *mtmp;

	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == LEATHER_LEASH || otmp->otyp == ARMORED_LEASH || otmp->otyp == INKA_LEASH || otmp->otyp == ADAMANT_LEASH) otmp->leashmon = 0;
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		mtmp->mleashed = 0;
}

#define MAXLEASHED	2

/* ARGSUSED */
STATIC_OVL void
use_leash(obj)
struct obj *obj;
{
	coord cc;
	register struct monst *mtmp;
	int spotmon;

	if(!obj->leashmon && number_leashed() >= MAXLEASHED) {
		You(FunnyHallu ? "futilely try to leash that bitch but it slips away!" : "cannot leash any more pets.");
		return;
	}

	if(!get_adjacent_loc((char *)0, (char *)0, u.ux, u.uy, &cc)) return;

	if((cc.x == u.ux) && (cc.y == u.uy)) {
		if (u.usteed && u.dz > 0) {
		    mtmp = u.usteed;
		    spotmon = 1;
		    goto got_target;
		}
		pline(FunnyHallu ? "Wow, now is that a string-tanga or a leather belt? Hard to see with those colors..." : "Leash yourself?  Very funny...");
		return;
	}

	if(!(mtmp = m_at(cc.x, cc.y))) {
		There("is no creature there.");
		return;
	}

	spotmon = canspotmon(mtmp);
 got_target:

	/* KMH, balance patch -- This doesn't work properly.
	 * Pets need extra memory for their edog structure.
	 * Normally, this is handled by tamedog(), but that
	 * rejects all demons.  Our other alternative would
	 * be to duplicate tamedog()'s functionality here.
	 * Yuck.  So I've merged it into the nymph code below.
	if (((mtmp->data == &mons[PM_SUCCUBUS]) || (mtmp->data == &mons[PM_INCUBUS]))
	     && (!mtmp->mtame) && (spotmon) && (!mtmp->mleashed) && !(mtmp->mfrenzied) ) {
	       pline("%s smiles seductively at the sight of this prop!", Monnam(mtmp));
	       mtmp->mtame = 10;
	       mtmp->mpeaceful = 1;
	       set_malign(mtmp);
	}*/
	if ((mtmp->data->mlet == S_NYMPH || mtmp->data == &mons[PM_SUCCUBUS]
		 || mtmp->data == &mons[PM_INCUBUS])
	     && (spotmon) && (!mtmp->mleashed)) {
	       pline("%s looks shocked! \"I'm not that way!\"", Monnam(mtmp));
	       mtmp->mtame = 0;
	       mtmp->mpeaceful = 0;
	       mtmp->msleeping = 0;
	}
	if(!mtmp->mtame) {
	    if(!spotmon)
		There("is no creature there.");
	    else
		pline("%s %s leashed!", Monnam(mtmp), (!obj->leashmon) ?
				"cannot be" : "is not");
	    return;
	}
	if(!obj->leashmon) {
		if(mtmp->mleashed) {
			pline("This %s is already leashed.",
			      spotmon ? l_monnam(mtmp) : "monster");
			return;
		}
		You("slip the leash around %s%s.",
		    spotmon ? "your " : "", l_monnam(mtmp));
		mtmp->mleashed = 1;
		obj->leashmon = (int)mtmp->m_id;
		mtmp->msleeping = 0;
		return;
	}
	if(obj->leashmon != (int)mtmp->m_id) {
		pline(FunnyHallu ? "It's not working! If you only knew why..." : "This leash is not attached to that creature.");
		return;
	} else {
		if(obj->cursed) {
			FunnyHallu ? pline("You pull and you pull, yet it just won't snap?!") : pline_The("leash would not come off!");
			obj->bknown = TRUE;
			return;
		}
		mtmp->mleashed = 0;
		obj->leashmon = 0;
		You("remove the leash from %s%s.",
		    spotmon ? "your " : "", l_monnam(mtmp));
		/* KMH, balance patch -- this is okay */
		if ((mtmp->data == &mons[PM_SUCCUBUS]) ||
				(mtmp->data == &mons[PM_INCUBUS]))
		{
		    FunnyHallu ? pline("%s is suddenly getting all emo!", Monnam(mtmp)) : pline("%s is infuriated!", Monnam(mtmp));
		    mtmp->mtame = 0;
		    mtmp->mpeaceful = 0;
		}

	}
	return;
}

struct obj *
get_mleash(mtmp)	/* assuming mtmp->mleashed has been checked */
register struct monst *mtmp;
{
	register struct obj *otmp;

	otmp = invent;
	while(otmp) {
		if((otmp->otyp == LEATHER_LEASH || otmp->otyp == ARMORED_LEASH || otmp->otyp == INKA_LEASH || otmp->otyp == ADAMANT_LEASH) && (otmp->leashmon == (int)mtmp->m_id) )
			return(otmp);
		otmp = otmp->nobj;
	}
	return((struct obj *)0);
}

/* but sometimes we want to see exactly what type of leash it is... --Amy */
int
get_mleash_otyp(mtmp)	/* assuming mtmp->mleashed has been checked */
register struct monst *mtmp;
{
	register struct obj *otmp;

	otmp = invent;
	while(otmp) {
		if((otmp->otyp == LEATHER_LEASH || otmp->otyp == ARMORED_LEASH || otmp->otyp == INKA_LEASH || otmp->otyp == ADAMANT_LEASH) && (otmp->leashmon == (int)mtmp->m_id) )
			return (otmp->otyp);
		otmp = otmp->nobj;
	}
	return -1;
}

/* or maybe we want to see which specific artifact leash it is --Amy */
int
get_mleash_artifact_type(mtmp)	/* assuming mtmp->mleashed has been checked */
register struct monst *mtmp;
{
	register struct obj *otmp;

	otmp = invent;
	while(otmp) {
		if((otmp->otyp == LEATHER_LEASH || otmp->otyp == ARMORED_LEASH || otmp->otyp == INKA_LEASH || otmp->otyp == ADAMANT_LEASH) && (otmp->leashmon == (int)mtmp->m_id) && otmp->oartifact)
			return (otmp->oartifact);
		otmp = otmp->nobj;
	}
	return -1;
}

#endif /* OVLB */
#ifdef OVL1

boolean
next_to_u()
{
	register struct monst *mtmp;
	register struct obj *otmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		if(mtmp->mleashed) {
			if (distu(mtmp->mx,mtmp->my) > 2) mnexto(mtmp);
			if (distu(mtmp->mx,mtmp->my) > 2) {
			    for(otmp = invent; otmp; otmp = otmp->nobj)
				if((otmp->otyp == LEATHER_LEASH || otmp->otyp == ARMORED_LEASH || otmp->otyp == INKA_LEASH || otmp->otyp == ADAMANT_LEASH) &&
					otmp->leashmon == (int)mtmp->m_id) {
				    if(otmp->cursed) return(FALSE);
				    You_feel("%s leash go slack.",
					(number_leashed() > 1) ? "a" : "the");
				    mtmp->mleashed = 0;
				    otmp->leashmon = 0;
				}
			}
		}
	}
	/* no pack mules for the Amulet */
	if (u.usteed && mon_has_amulet(u.usteed)) return FALSE;
	return(TRUE);
}

#endif /* OVL1 */
#ifdef OVL0

void
check_leash(x, y)
register xchar x, y;
{
	register struct obj *otmp;
	register struct monst *mtmp;
	int chokedamage = 2;

	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if ((otmp->otyp != LEATHER_LEASH && otmp->otyp != ARMORED_LEASH && otmp->otyp != INKA_LEASH && otmp->otyp != ADAMANT_LEASH) || otmp->leashmon == 0) continue;
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		if ((int)mtmp->m_id == otmp->leashmon) break; 
	    }
	    if (!mtmp) {
		impossible("leash in use isn't attached to anything?");
		otmp->leashmon = 0;
		continue;
	    }

	    if (otmp && otmp->otyp == INKA_LEASH) chokedamage = 20;

	    if (dist2(u.ux,u.uy,mtmp->mx,mtmp->my) > dist2(x,y,mtmp->mx,mtmp->my)) {
		if (!um_dist(mtmp->mx, mtmp->my, 3)) {
		    ;	/* still close enough */
		} else if (otmp->oartifact == ART_INFINITE_RANGE) {
			; /* doesn't snap or anything, no matter how far away the pet is */
		} else if (otmp->cursed && otmp->otyp != ADAMANT_LEASH && !breathless(mtmp->data) && (!mtmp->egotype_undead) ) {
		    if (um_dist(mtmp->mx, mtmp->my, 5) ||
			    (mtmp->mhp -= rnd(chokedamage)) <= 0) {
			long save_pacifism = u.uconduct.killer;

			Your("leash chokes %s to death!", mon_nam(mtmp));
			/* hero might not have intended to kill pet, but
			   that's the result of his actions; gain experience,
			   lose pacifism, take alignment and luck hit, make
			   corpse less likely to remain tame after revival */
			xkilled(mtmp, 0);	/* no "you kill it" message */
			/* life-saving doesn't ordinarily reset this */
			if (mtmp->mhp > 0) u.uconduct.killer = save_pacifism;
		    } else {
			pline("%s chokes on the leash!", Monnam(mtmp));
			/* tameness eventually drops to 1 here (never 0) */
			if (mtmp->mtame && rn2(mtmp->mtame) && !((rnd(30 - ACURR(A_CHA))) < 4) ) mtmp->mtame--;
		    }
		} else {
		    if (um_dist(mtmp->mx, mtmp->my, 5)) {
			if (otmp->otyp == LEATHER_LEASH || otmp->otyp == ARMORED_LEASH || otmp->otyp == ADAMANT_LEASH) {
				pline("%s leash snaps loose!", s_suffix(Monnam(mtmp)));
				m_unleash(mtmp, FALSE, 0);
			} else if (otmp->otyp == INKA_LEASH) {
				pline("%s warps to you!", Monnam(mtmp));
				mnexto(mtmp);
			} else {
				impossible("weird leash snapping effect %d", otmp->otyp);
				pline("%s leash snaps loose!", s_suffix(Monnam(mtmp)));
				m_unleash(mtmp, FALSE, 0);
			}
		    } else {
			You("pull on the leash.");
			if (monstersoundtype(mtmp) != MS_SILENT)
			    switch (rn2(3)) {
			    case 0:  growl(mtmp);   break;
			    case 1:  yelp(mtmp);    break;
			    default: whimper(mtmp); break;
			    }
		    }
		}
	    }
	}
}

#endif /* OVL0 */
#ifdef OVLB

#define WEAK	3	/* from eat.c */

static const char look_str[] = "look %s.";

STATIC_OVL int
use_mirror(obj)
struct obj *obj;
{
	register struct monst *mtmp;
	register char mlet;
	boolean vis = !Blind && !obj->oinvisreal && (!obj->oinvis || See_invisible);

	if(!getdir((char *)0)) return 0;

	if (obj->cursed && !(obj->oartifact == ART_MAKE_THE_FOG_AWAY) && !rn2(100)) {
		useup(obj);
		change_luck(-2);
		Your("mirror suddenly shatters into a thousand pieces!");
		return 2;
	}
	if (!rn2(100)) {
		if (obj->blessed) unbless(obj);
		else curse(obj);
		pline("Your mirror seems less effective.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
	}

	if(obj->cursed && !(obj->oartifact == ART_MAKE_THE_FOG_AWAY) && !rn2(2)) {
		if (vis)
			FunnyHallu ? pline("Trippy messy rainbow colors... wow!") : pline_The("mirror fogs up and doesn't reflect!");
		return 1;
	}
	if(!u.dx && !u.dy && !u.dz) {
		if(vis && !Invisible) {
		    if (u.umonnum == PM_FLOATING_EYE) {
			if (!Free_action || !rn2(StrongFree_action ? 100 : 20)) {
			pline(FunnyHallu ?
			      "Yow!  The mirror stares back!" :
			      "Yikes!  You've frozen yourself!");
			nomul(-rnd((MAXULEV+6) - u.ulevel), "gazing into a mirror", TRUE);
			nomovemsg = 0;
			} else You("stiffen momentarily under your gaze.");
		    } else if (is_vampire(youmonst.data))
			You("don't have a reflection.");
		    else if (u.umonnum == PM_UMBER_HULK) {
			pline("Huh?  That doesn't look like you!");
			make_confused(HConfusion + d(3,4),FALSE);
		    } else if (Hallucination)
			You(look_str, hcolor((char *)0));
		    else if (Sick)
			You(look_str, "peaked");
		    else if (u.uhs >= WEAK && !have_anorexia() )
			You(look_str, "undernourished");
		    else if (u.uhs >= WEAK && have_anorexia() )
			You(look_str, "beautiful and skinny");
		    else {
			You("look as %s as ever.",
				ACURR(A_CHA) > 14 ?
				(poly_gender()==1 ? "beautiful" : "handsome") :
				"ugly");
			Your("hairstyle is '%s.", bundledescription());
			Your("areola diameter is %d.", u.areoladiameter);
		    }
		} else {
			You_cant("see your %s %s.",
				ACURR(A_CHA) > 14 ?
				(poly_gender()==1 ? "beautiful" : "handsome") :
				"ugly",
				body_part(FACE));
		}
		return 1;
	}
	if(u.uswallow) {
		if (vis) You("reflect %s %s.", s_suffix(mon_nam(u.ustuck)),
		    mbodypart(u.ustuck, STOMACH));
		return 1;
	}
	if(Underwater) {
		if (!obj->oinvis)
		You(FunnyHallu ?
		    "give the fish a chance to fix their makeup." :
		    "reflect the murky water.");
		return 1;
	}
	if(u.dz) {
		if (vis)
		    You("reflect the %s.",
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
		return 1;
	}
	mtmp = bhit(u.dx, u.dy, COLNO, INVIS_BEAM,
		    (int (*)(MONST_P,OBJ_P))0,
		    (int (*)(OBJ_P,OBJ_P))0,
		    &obj, FALSE);
	if (!mtmp || !haseyes(mtmp->data))
		return 1;

	vis = canseemon(mtmp);
	mlet = mtmp->data->mlet;
	if (mtmp->msleeping) {
		if (vis)
		    pline ("%s is too tired to look at your mirror.",
			    Monnam(mtmp));
	} else if (!mtmp->mcansee) {
	    if (vis)
		pline("%s can't see anything right now.", Monnam(mtmp));
	} else if ((obj->oinvis && !perceives(mtmp->data)) || obj->oinvisreal) {
	    if (vis)
		pline("%s can't see your mirror.", Monnam(mtmp));
	/* some monsters do special things */
	} else if (is_vampire(mtmp->data) || mlet == S_GHOST) {
	    if (vis)
		pline ("%s doesn't have a reflection.", Monnam(mtmp));
	} else if(!mtmp->mcan && !mtmp->minvis && !mtmp->minvisreal &&
					mtmp->data == &mons[PM_MEDUSA]) {
		if (mon_reflects(mtmp, "The gaze is reflected away by %s %s!"))
			return 1;
		if (vis)
			pline("%s is turned to stone!", Monnam(mtmp));
		stoned = TRUE;
		killed(mtmp);
	} else if(!mtmp->mcan && !(dmgtype(mtmp->data, AD_PLYS)) && !mtmp->minvis && !mtmp->minvisreal &&
					mtmp->data == &mons[PM_FLOATING_EYE]) {
		int tmp = d((int)mtmp->m_lev, (int)mtmp->data->mattk[0].damd);
		if (!rn2(4)) tmp = 120;
		if (vis)
			pline("%s is frozen by its reflection.", Monnam(mtmp));
		else You_hear("%s stop moving.",something);
		mtmp->mcanmove = 0;
		if ( (int) mtmp->mfrozen + tmp > 127)
			mtmp->mfrozen = 127;
		else mtmp->mfrozen += tmp;
	} else if(!mtmp->mcan && !mtmp->minvis && !mtmp->minvisreal &&
					mtmp->data == &mons[PM_UMBER_HULK]) {
		if (vis)
			pline ("%s confuses itself!", Monnam(mtmp));
		mtmp->mconf = 1;
	} else if(!mtmp->mcan && !mtmp->minvis && !mtmp->minvisreal && (mlet == S_NYMPH
				     || mtmp->data==&mons[PM_SUCCUBUS])) {
		if (vis) {
		    pline ("%s admires herself in your mirror.", Monnam(mtmp));
		    pline ("She takes it!");
		} else pline ("It steals your mirror!");
		if (obj && obj->oartifact == ART_FAIREST_IN_THE_LAND && !obj->cursed && !mtmp->mfrenzied) {
			mtmp->mpeaceful = 1;
			if (!rn2(20)) {
				if (obj->blessed) unbless(obj);
				else curse(obj);
				pline("Your mirror seems less effective.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			}
		}
		setnotworn(obj); /* in case mirror was wielded */
		freeinv(obj);
		(void) mpickobj(mtmp,obj,FALSE);
		if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
	} else if (!is_unicorn(mtmp->data) && !humanoid(mtmp->data) && !mtmp->minvisreal &&
			(!mtmp->minvis || perceives(mtmp->data)) && !resist(mtmp, TOOL_CLASS, 0, 0) && !rn2(5)) {
		if (vis)
		    pline("%s is frightened by its reflection.", Monnam(mtmp));
		monflee(mtmp, d(2,4), FALSE, FALSE, FALSE);
	} else if (!Blind) {
		if ((mtmp->minvis && !See_invisible) || mtmp->minvisreal)
		    ;
		else if ((mtmp->minvis && !perceives(mtmp->data)) || mtmp->minvisreal
			 || !haseyes(mtmp->data))
		    pline("%s doesn't seem to notice its reflection.",
			Monnam(mtmp));
		else
		    pline("%s ignores %s reflection.",
			  Monnam(mtmp), mhis(mtmp));
	}
	return 1;
}

/* use_bell: return TRUE if the bell broke, FALSE otherwise --Amy */
STATIC_OVL int
use_bell(optr)
struct obj **optr;
{
	register struct obj *obj = *optr;
	struct monst *mtmp;
	boolean wakem = FALSE, learno = FALSE,
		ordinary = (obj->otyp != BELL_OF_OPENING || !obj->spe),
		invoking = (obj->otyp == BELL_OF_OPENING &&
			 invocation_pos(u.ux, u.uy) && !On_stairs(u.ux, u.uy));

	if (obj && obj->otyp == BELL && !rn2(evilfriday ? 200 : 500)) {
		useup(obj);
		Your("bell suddenly breaks into pieces.");
		return TRUE;
	}

	You("ring %s.", the(xname(obj)));
	if (PlayerHearsSoundEffects) pline(issoviet ? "Tip bloka l'da net doma pryamo seychas!" : "Bimmelimm!");

	if (obj->otyp == BELL_OF_OPENING && !u.bellimbued) {

		/* in wizard mode we're capable of just magically imbuing the bell for easier testing --Amy */
		if (wizard && yn("Imbue the bell?") == 'y') {
			u.bellimbued = TRUE;
		} else {
			pline("But nothing happens because you didn't imbue the bell yet. Look for a magic portal in the Quest, which will lead to the Subquest. In that Subquest, find the staircase to the Bell Caves in order to imbue the bell.");
			return FALSE;
		}
	}

	if (obj && obj->oartifact == ART_BIMMEL_BIMMEL && !obj->cursed) {
	    int i, j, bd = 1;
		struct monst *bimmel;

	    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
		if (!isok(u.ux + i, u.uy + j)) continue;
		if ((bimmel = m_at(u.ux + i, u.uy + j)) != 0 && bimmel->data->mlet == S_XAN)
			if (!rn2(5) && bimmel && !(bimmel->mtame)) {
				bimmel->mpeaceful = FALSE;
				bimmel->mfrenzied = TRUE;

			} else if (bimmel && !(bimmel->mtame) && !(bimmel->mfrenzied)) {
				if (!resist(bimmel, RING_CLASS, 0, TELL) || ((rnd(30 - ACURR(A_CHA))) < 4) ) (void) tamedog(bimmel, (struct obj *) 0, FALSE);
			}
	    }

		if (!rn2(20)) {
			if (obj->blessed) unbless(obj);
			else curse(obj);
			pline("Your bell seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}

	}

	if (Underwater || (u.uswallow && ordinary)) {
#ifdef	AMIGA
	    amii_speaker( obj, "AhDhGqEqDhEhAqDqFhGw", AMII_MUFFLED_VOLUME );
#endif
	    pline(FunnyHallu ? "Sounds like a christmas song..." : "But the sound is muffled.");

	} else if (invoking && ordinary) {
	    /* needs to be recharged... */
	    pline(FunnyHallu ? "Where is the button? You can't seem to find it..." : "But it makes no sound.");
	    learno = TRUE;	/* help player figure out why */

	} else if (ordinary) {
#ifdef	AMIGA
	    amii_speaker( obj, "ahdhgqeqdhehaqdqfhgw", AMII_MUFFLED_VOLUME );
#endif
	    if (obj->cursed && !rn2(4) &&
		    /* note: once any of them are gone, we stop all of them */
		    !(mvitals[PM_WOOD_NYMPH].mvflags & G_GONE) &&
		    !(mvitals[PM_WATER_NYMPH].mvflags & G_GONE) &&
		    !(mvitals[PM_MOUNTAIN_NYMPH].mvflags & G_GONE) &&
		    (mtmp = makemon(mkclass(S_NYMPH, 0),
					u.ux, u.uy, NO_MINVENT)) != 0) {
		You("summon %s!", a_monnam(mtmp));
		if (!obj_resists(obj, 93, 100)) {
		    pline("%s shattered!", Tobjnam(obj, "have"));
		    useup(obj);
		    *optr = 0;
		} else switch (rn2(3)) {
			default:
				break;
			case 1:
				mon_adjust_speed(mtmp, 2, (struct obj *)0);
				break;
			case 2: /* no explanation; it just happens... */
				nomovemsg = "";
				nomul(-rnd(2), 0, TRUE);
				break;
		}
	    }
	    wakem = TRUE;

	} else {
	    /* charged Bell of Opening */
	    	int nochargechange = 10;
		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_DEVICES)) {
				default: break;
				case P_BASIC: nochargechange = 9; break;
				case P_SKILLED: nochargechange = 8; break;
				case P_EXPERT: nochargechange = 7; break;
				case P_MASTER: nochargechange = 6; break;
				case P_GRAND_MASTER: nochargechange = 5; break;
				case P_SUPREME_MASTER: nochargechange = 4; break;
			}
		}

		if (nochargechange >= rnd(10)) consume_obj_charge(obj, TRUE);

	    if (u.uswallow) {
		if (!obj->cursed && !rn2(10))
		    (void) openit();
		else {
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

	    } else if (obj->cursed && (!invoking || !rn2(2)) ) {
		coord mm;

		mm.x = u.ux;
		mm.y = u.uy;
		mkundead(&mm, FALSE, NO_MINVENT, TRUE);
		wakem = TRUE;
		badeffect();
		aggravate();

	    } else  if (invoking) {
		pline("%s an unsettling shrill sound...",
		      Tobjnam(obj, "issue"));
#ifdef	AMIGA
		amii_speaker( obj, "aefeaefeaefeaefeaefe", AMII_LOUDER_VOLUME );
#endif
		obj->age = moves;
		learno = TRUE;
		wakem = TRUE;

	    } else if (obj->blessed) {
		int res = 0;

#ifdef	AMIGA
		amii_speaker( obj, "ahahahDhEhCw", AMII_SOFT_VOLUME );
#endif
		if (uchain && !rn2(10)) {
		    unpunish();
		    res = 1;
		}
		/* the bell now has many more charges... but I didn't do that so you can endlessly detect traps! --Amy
		 * it's *only* meant to make sure you won't run out as easily during the ritual! */
		if (!rn2(10)) res += openit();
		switch (res) {
		  case 0:
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
			break;
		  case 1:  pline("%s opens...", Something);
			   learno = TRUE; break;
		  default: pline("Things open around you...");
			   learno = TRUE; break;
		}

	    } else {  /* uncursed */
#ifdef	AMIGA
		amii_speaker( obj, "AeFeaeFeAefegw", AMII_OKAY_VOLUME );
#endif
		if (!rn2(10)) {
			if (findit() != 0) learno = TRUE;
			else {
				pline("%s", nothing_happens);
				if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
					pline("Oh wait, actually something bad happens...");
					badeffect();
				}
			}
		} else {
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
	    }

	}	/* charged BofO */

	if (learno) {
	    makeknown(BELL_OF_OPENING);
	    obj->known = 1;
	}
	if (obj->oartifact == ART_MAKE_BROKEN && !rn2(2)) wakem = FALSE;

	if (obj->oartifact == ART_SOLVEDBRIDGE) {
		if(Is_stronghold(&u.uz)) {
			int x,y;
			for(y=u.uy-1; y<=u.uy+1; y++) {
			  for(x=u.ux-1; x<=u.ux+1; x++) {

				if(isok(x,y)) {

					if(find_drawbridge(&x,&y)) {
					    if(levl[x][y].typ == DRAWBRIDGE_DOWN) {
						pline("Hmm, the drawbridge doesn't seem to react.");
					    } else {
						open_drawbridge(x,y);
					    }
					    goto dbridgedone; /* wtf without that there'd be an infinite loop??? --Amy */
					}
				}
			  }
			}
		}
	}
dbridgedone:

	if (wakem) wake_nearby();

	return FALSE; /* didn't break */
}

STATIC_OVL void
use_candelabrum(obj)
register struct obj *obj;
{
	const char *s = (obj->spe != 1) ? "candles" : "candle";

	if (!u.bellimbued) {
		pline("The menora will not do anything unless you find and imbue a 'silver bell'.");
		return;
	}

	if(Underwater) {
		You(FunnyHallu ? "don't even know what this thing is good for." : "cannot make fire under water.");
		return;
	}
	if(obj->lamplit) {
		You("snuff the %s.", s);
		end_burn(obj, TRUE);
		return;
	}
	if(obj->spe <= 0) {
		pline("This %s has no %s.", xname(obj), s);
		return;
	}

	if (evilfriday && obj->cursed) {
		pline("Ouch! You burnt your %s!", body_part(FINGER));
		losehp(rnd(2), "igniting a cursed light source", KILLED_BY);
	}

	if(u.uswallow || (obj->cursed && !rn2(2))) {
		if (!Blind) {
		    pline_The("%s %s for a moment, then %s.", s, vtense(s, "flicker"), vtense(s, "die"));
		}
		return;
	}
	if(obj->spe < 7) {
		There("%s only %d %s in %s.",
		      vtense(s, "are"), obj->spe, s, the(xname(obj)));
		if (!Blind)
		    pline("%s lit.  %s dimly.",
			  obj->spe == 1 ? "It is" : "They are",
			  Tobjnam(obj, "shine"));
	} else {
		pline("%s's %s burn%s", The(xname(obj)), s,
			(Blind ? "." : " brightly!"));
	}
	if (!invocation_pos(u.ux, u.uy)) {
		pline_The("%s %s being rapidly consumed!", s, vtense(s, "are"));
		obj->age = (obj->age + 1L) / 2L;
	} else {
		if(obj->spe == 7) {
		    if (Blind)
		      pline("%s a strange warmth!", Tobjnam(obj, "radiate"));
		    else
		      pline("%s with a strange light!", Tobjnam(obj, "glow"));
		}
		obj->known = 1;
	}
	begin_burn(obj, FALSE);
}

STATIC_OVL void
use_candle(optr)
struct obj **optr;
{
	register struct obj *obj = *optr;
	register struct obj *otmp;
	const char *s = (obj->quan != 1) ? "candles" : "candle";
	char qbuf[QBUFSZ];

	if(u.uswallow) {
		You(no_elbow_room);
		return;
	}
	if(Underwater) {
		pline(FunnyHallu ? "This gearing seems to be made for use on dry land!" : "Sorry, fire and water don't mix.");
		return;
	}

	otmp = carrying(CANDELABRUM_OF_INVOCATION);
	/* [ALI] Artifact candles can't be attached to candelabrum
	 *       (magic candles still can be).
	 */
	if(obj->oartifact || !otmp || otmp->spe == 7) {
		use_lamp(obj);
		return;
	}

	sprintf(qbuf, "Attach %s", the(xname(obj)));
	sprintf(eos(qbuf), " to %s?",
		safe_qbuf(qbuf, sizeof(" to ?"), the(xname(otmp)),
			the(simple_typename(otmp->otyp)), "it"));
	if(yn(qbuf) == 'n') {
		if (!obj->lamplit)
		    You("try to light %s...", the(xname(obj)));
		use_lamp(obj);
		return;
	} else {
		if ((long)otmp->spe + obj->quan > 7L)
		    obj = splitobj(obj, 7L - (long)otmp->spe);
		else *optr = 0;
		You("attach %ld%s %s to %s.",
		    obj->quan, !otmp->spe ? "" : " more",
		    s, the(xname(otmp)));
		if (obj->otyp == MAGIC_CANDLE) {
		    if (obj->lamplit)
			pline_The("new %s %s very ordinary.", s,
				vtense(s, "look"));
		    else
			pline("%s very ordinary.",
				(obj->quan > 1L) ? "They look" : "It looks");
		    if (!otmp->spe)
			otmp->age = 600L;
		} else
		if (!otmp->spe || otmp->age > obj->age)
		    otmp->age = obj->age;
		otmp->spe += (int)obj->quan;
		if (otmp->lamplit && !obj->lamplit)
		    pline_The("new %s magically %s!", s, vtense(s, "ignite"));
		else if (!otmp->lamplit && obj->lamplit)
		    pline("%s out.", (obj->quan > 1L) ? "They go" : "It goes");
		if (obj->unpaid)
		    verbalize("You %s %s, you bought %s!",
			      otmp->lamplit ? "burn" : "use",
			      (obj->quan > 1L) ? "them" : "it",
			      (obj->quan > 1L) ? "them" : "it");
		if (obj->quan < 7L && otmp->spe == 7)
		    pline("%s now has seven%s candles attached.",
			  The(xname(otmp)), otmp->lamplit ? " lit" : "");
		/* candelabrum's light range might increase */
		if (otmp->lamplit) obj_merge_light_sources(otmp, otmp);
		/* candles are no longer a separate light source */
		if (obj->lamplit) end_burn(obj, TRUE);
		/* candles are now gone */
		useupall(obj);
	}
}

boolean
snuff_candle(otmp)  /* call in drop, throw, and put in box, etc. */
register struct obj *otmp;
{
	register boolean candle = Is_candle(otmp);

	if (((candle && otmp->oartifact != ART_CANDLE_OF_ETERNAL_FLAME)
		|| otmp->otyp == CANDELABRUM_OF_INVOCATION) &&
		otmp->lamplit) {
	    char buf[BUFSZ];
	    xchar x, y;
	    register boolean many = candle ? otmp->quan > 1L : otmp->spe > 1;

	    (void) get_obj_location(otmp, &x, &y, 0);
	    if (otmp->where == OBJ_MINVENT ? cansee(x,y) : !Blind)
		pline("%s %scandle%s flame%s extinguished.",
		      Shk_Your(buf, otmp),
		      (candle ? "" : "candelabrum's "),
		      (many ? "s'" : "'s"), (many ? "s are" : " is"));
	   end_burn(otmp, TRUE);
	   return(TRUE);
	}
	return(FALSE);
}

/* called when lit lamp is hit by water or put into a container or
   you've been swallowed by a monster; obj might be in transit while
   being thrown or dropped so don't assume that its location is valid */
boolean
snuff_lit(obj)
struct obj *obj;
{
	xchar x, y;

	if (obj->lamplit) {
	    if (artifact_light(obj)) return FALSE; /* Artifact lights are never snuffed */
	    if (obj->otyp == OIL_LAMP || obj->otyp == PIT_LAMP || obj->otyp == FEANORIAN_LAMP || obj->otyp == ELECTRIC_LAMP || obj->otyp == MAGIC_LAMP ||
		obj->otyp == BRASS_LANTERN || obj->otyp == DWARVEN_LANTERN || obj->otyp == DIM_LANTERN || obj->otyp == POT_OIL ||
		obj->otyp == TORCH) {
		(void) get_obj_location(obj, &x, &y, 0);
		if (obj->where == OBJ_MINVENT ? cansee(x,y) : !Blind)
		    pline("%s %s out!", Yname2(obj), otense(obj, "go"));
		end_burn(obj, TRUE);
		return TRUE;
	    }
	    if (snuff_candle(obj)) return TRUE;
	}
	return FALSE;
}

/* Called when potentially lightable object is affected by fire_damage().
   Return TRUE if object was lit and FALSE otherwise --ALI */
boolean
catch_lit(obj)
struct obj *obj;
{
	xchar x, y;

	if (!obj->lamplit && (obj->otyp == MAGIC_LAMP || obj->otyp == FEANORIAN_LAMP || ignitable(obj))) {
	    if ((obj->otyp == MAGIC_LAMP ||
		 obj->otyp == CANDELABRUM_OF_INVOCATION) &&
		obj->spe == 0)
		return FALSE;
	    else if (obj->otyp != MAGIC_LAMP && obj->otyp != FEANORIAN_LAMP && obj->age == 0)
		return FALSE;
	    if (!get_obj_location(obj, &x, &y, 0))
		return FALSE;
	    if (obj->otyp == CANDELABRUM_OF_INVOCATION && obj->cursed)
		return FALSE;
	    if ((obj->otyp == OIL_LAMP || obj->otyp == PIT_LAMP || obj->otyp == FEANORIAN_LAMP || obj->otyp == ELECTRIC_LAMP || obj->otyp == MAGIC_LAMP ||
		 obj->otyp == BRASS_LANTERN || obj->otyp == DWARVEN_LANTERN || obj->otyp == DIM_LANTERN) && obj->cursed && !rn2(2))
		return FALSE;
	    if (obj->where == OBJ_MINVENT ? cansee(x,y) : !Blind)
		pline("%s %s light!", Yname2(obj), otense(obj, "catch"));
	    if (obj->otyp == POT_OIL) makeknown(obj->otyp);
	    if (obj->unpaid && costly_spot(u.ux, u.uy) && (obj->where == OBJ_INVENT)) {
	        /* if it catches while you have it, then it's your tough luck */
		check_unpaid(obj);
	        verbalize("That's in addition to the cost of %s %s, of course.",
				Yname2(obj), obj->quan == 1 ? "itself" : "themselves");
		bill_dummy_object(obj);
	    }
	    begin_burn(obj, FALSE);
	    return TRUE;
	}
	return FALSE;
}

STATIC_OVL void
use_lamp(obj)
struct obj *obj;
{
	char buf[BUFSZ];
	char qbuf[QBUFSZ];

	boolean vaapadcheck = FALSE;

	if (!PlayerCannotUseSkills && P_SKILL(P_VAAPAD) >= P_EXPERT) vaapadcheck = TRUE;

	if (Race_if(PM_SATRE) && !is_lightsaber(obj)) {
		pline("As a satre, you are unable to use a light source.");
		return;
	}

	if (UnKnowledgeEffect || u.uprops[UN_KNOWLEDGE_EFFECT].extrinsic || have_unknowledgestone() || (uarmu && uarmu->oartifact == ART_STOCKTON_BRAND)) {
		if (obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && !(objects[obj->otyp].oc_name_known) ) {
			pline("Unfortunately, you don't know how to use that tool.");
			return;
		}
		if (obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && UnKnowledgeXtra &&  (!obj->known || !obj->rknown || !obj->dknown || !obj->bknown) )	{
			pline("Unfortunately, you don't know how to use that tool.");
			return;
		}
	}

	if(Underwater) {
		pline(FunnyHallu ? "You fumble around with that thing but it won't work." : "This is not a diving lamp.");
		return;
	}
	if(obj->lamplit) {
		if(obj->otyp == OIL_LAMP || obj->otyp == PIT_LAMP || obj->otyp == FEANORIAN_LAMP || obj->otyp == ELECTRIC_LAMP || obj->otyp == MAGIC_LAMP || obj->otyp == DIM_LANTERN ||
				obj->otyp == BRASS_LANTERN || obj->otyp == DWARVEN_LANTERN) {
		    pline("%s lamp is now off.", Shk_Your(buf, obj));
		} else if(is_lightsaber(obj)) {
		    if (obj->otyp == RED_DOUBLE_LIGHTSABER || obj->otyp == CYAN_DOUBLE_LIGHTSABER || obj->otyp == WHITE_DOUBLE_LIGHTSABER || obj->otyp == LASERDENT || obj->otyp == LASER_FLYAXE || obj->otyp == PINK_DOUBLE_LIGHTSWORD || obj->otyp == LASERXBOW || obj->otyp == SITH_STAFF) {

			/* Do we want to activate dual bladed mode? */
			if (vaapadcheck) {

				if (!obj->altmode) {
					if (yn("Turn the double lightsaber off? (If you say no, you activate the second blade)") != 'n') {
						obj->altmode = FALSE;
						lightsaber_deactivate(obj, TRUE);
						return;
					} else {
						if (obj->cursed && !rn2(4)) {
							obj->altmode = FALSE;
							lightsaber_deactivate(obj, TRUE);
							return;
						}
						You("ignite the second blade of %s.", yname(obj));
						obj->altmode = TRUE;
						return;
					}
				} else {
					if (yn("Turn only one blade of the double lightsaber off? (If you say no, you turn it off completely)") != 'n') {
						if (obj->cursed && !rn2(4)) {
							obj->altmode = FALSE;
							lightsaber_deactivate(obj, TRUE);
							return;
						}
						You("turn off the second blade of %s.", yname(obj));
						obj->altmode = FALSE;
						return;
					} else {
						obj->altmode = FALSE;
						lightsaber_deactivate(obj, TRUE);
						return;
					}

				}

			} else if (!obj->altmode && (!obj->cursed || rn2(4))) {
			    You("ignite the second blade of %s.", yname(obj));
			    obj->altmode = TRUE;
			    return;
			} else obj->altmode = FALSE;
		    }
		    lightsaber_deactivate(obj, TRUE);
		    return;
		} else if (artifact_light(obj)) {
		    You_cant("snuff out %s.", yname(obj));
		    return;
		} else {
		    You("snuff out %s.", yname(obj));
		}
		end_burn(obj, TRUE);
		return;
	}
	// for some reason, the lightsaber prototype is created with
	// age == 0
	/* Amy edit: because of artifact invoke timers which (stupidly) use the same variable!
	 * edit: fuck that noise, using a new "obj->invoketimer" variable now */
	/* if (obj->oartifact == ART_LIGHTSABER_PROTOTYPE)
		obj->age = 300L;
	if (obj->oartifact == ART_DEFINITE_LIGHTSABER)
		obj->age = 300L; */
	/* magic lamps with an spe == 0 (wished for) cannot be lit */
	if ((!Is_candle(obj) && obj->age == 0)
			|| (obj->otyp == MAGIC_LAMP && obj->spe == 0)) {
		if ((obj->otyp == BRASS_LANTERN) || (obj->otyp == DWARVEN_LANTERN) || (obj->otyp == DIM_LANTERN)
			|| is_lightsaber(obj)
			) {
			Your("%s has run out of power.", xname(obj));
			if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
		}
		else if (obj->otyp == TORCH) {
			Your("torch has burnt out and needs to be recharged.");
			if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
		}
		else {
			pline("This %s has no oil.", xname(obj));
			if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
		}


		return;
	}

	if (evilfriday && obj->cursed) {
		pline("Ouch! You burnt your %s!", body_part(FINGER));
		losehp(rnd(2), "igniting a cursed light source", KILLED_BY);
	}

	if (obj->cursed && !(obj->oartifact == ART_ILLUMER_THE_GREAT) && !rn2(2)) {
		pline("%s for a moment, then %s.",
		      Tobjnam(obj, "flicker"), otense(obj, "die"));
	} else {
		if(obj->otyp == OIL_LAMP || obj->otyp == PIT_LAMP || obj->otyp == FEANORIAN_LAMP || obj->otyp == ELECTRIC_LAMP || obj->otyp == MAGIC_LAMP || obj->otyp == BRASS_LANTERN || obj->otyp == DWARVEN_LANTERN || obj->otyp == DIM_LANTERN) {
		    check_unpaid(obj);
		    pline("%s lamp is now on.", Shk_Your(buf, obj));
		    if (obj->oartifact == ART_PSEUDO_MAGIC_LAMP) BishopGridbug += rnz(5000);
		} else if (obj->otyp == TORCH) {
		    check_unpaid(obj);
		    pline("%s flame%s burn%s%s",
			s_suffix(Yname2(obj)),
			plur(obj->quan),
			obj->quan > 1L ? "" : "s",
			Blind ? "." : " brightly!");
		} else if (is_lightsaber(obj)) {
		    /* WAC -- lightsabers */
		    /* you can see the color of the blade */
		    
		    if (!Blind) makeknown(obj->otyp);
		    You("ignite %s.", yname(obj));

			if ((obj->otyp == RED_DOUBLE_LIGHTSABER || obj->otyp == CYAN_DOUBLE_LIGHTSABER || obj->otyp == WHITE_DOUBLE_LIGHTSABER || obj->otyp == LASERDENT || obj->otyp == LASER_FLYAXE || obj->otyp == PINK_DOUBLE_LIGHTSWORD || obj->otyp == LASERXBOW || obj->otyp == SITH_STAFF) && vaapadcheck) {
				if (yn("Use only one of the two blades? (If you say no, you ignite both)") != 'n') {
					; /* do nothing */
				} else {
					You("ignite the second blade of %s.", yname(obj));
					obj->altmode = TRUE;
				}

			}

		    unweapon = FALSE;
		} else {	/* candle(s) */
		    sprintf(qbuf, "Light all of %s?", the(xname(obj)));
		    if (obj->quan > 1L && (yn(qbuf) == 'n')) {
			/* Check if player wants to light all the candles */
			struct obj *rest;	     /* the remaining candles */
			rest = splitobj(obj, obj->quan - 1L);
			obj_extract_self(rest);	     /* free from inv */
			obj->spe++;	/* this prevents merging */
			(void)hold_another_object(rest, "You drop %s!",
					  doname(rest), (const char *)0);
			obj->spe--;
		    }
		    pline("%s flame%s %s%s",
			s_suffix(Yname2(obj)),
			plur(obj->quan), otense(obj, "burn"),
			Blind ? "." : " brightly!");
		    if (obj->unpaid && costly_spot(u.ux, u.uy) &&
			  obj->otyp != MAGIC_CANDLE) {
			const char *ithem = obj->quan > 1L ? "them" : "it";
			verbalize("You burn %s, you bought %s!", ithem, ithem);
			bill_dummy_object(obj);
		    }
		}
		begin_burn(obj, FALSE);
	}
}

/* MRKR: Torches */

STATIC_OVL int
use_torch(obj)
struct obj *obj;
{
    struct obj *otmp = NULL;
    if (u.uswallow) {
	You(no_elbow_room);
	return 0;
    }
    if (Underwater) {
	pline(FunnyHallu ? "The crappy tool doesn't seem to do what you want it to do!" : "Sorry, fire and water don't mix.");
	return 0;
    }
    if (obj->quan > 1L) {
	if (obj == uwep && welded(obj)) {
		You("can only hold one lit torch, but can't drop any to hold only one.");
		return 0;
	}
	otmp = obj;
	obj = splitobj(otmp, 1L);
	obj_extract_self(otmp);	/* free from inv */
    }
    /* You can use a torch in either wielded weapon slot */
    if (obj != uwep && (obj != uswapwep || !u.twoweap))
	if (!wield_tool(obj, (const char *)0)) return 0;
    use_lamp(obj);
    /* shouldn't merge */
    if (otmp)
	otmp = hold_another_object(otmp, "You drop %s!",
				   doname(otmp), (const char *)0);
    return 1;
}

STATIC_OVL void
light_cocktail(obj)
	struct obj *obj;        /* obj is a potion of oil or a stick of dynamite */
{
	char buf[BUFSZ];
	const char *objnam =
	    obj->otyp == POT_OIL ? "potion" : "stick";

	if (u.uswallow) {
	    You(no_elbow_room);
	    return;
	}

	if(Underwater) {
		You(FunnyHallu ? "fumble around with the fuse but nothing happens." : "can't light this underwater!");
		return;
	}

	if (obj->lamplit) {
	    You("snuff the lit %s.", objnam);
	    end_burn(obj, TRUE);
	    /*
	     * Free & add to re-merge potion.  This will average the
	     * age of the potions.  Not exactly the best solution,
	     * but its easy.
	     */
	    freeinv(obj);
	    (void) addinv(obj);
	    return;
	} else if (Underwater) {
	    There("is not enough oxygen to sustain a fire.");
	    return;
	}

	You("light %s %s.%s", shk_your(buf, obj), objnam,
	    Blind ? "" : "  It gives off a dim light.");
	if (obj->unpaid && costly_spot(u.ux, u.uy)) {
	    /* Normally, we shouldn't both partially and fully charge
	     * for an item, but (Yendorian Fuel) Taxes are inevitable...
	     */
	    if (obj->otyp != STICK_OF_DYNAMITE) {
	    check_unpaid(obj);
	    verbalize("That's in addition to the cost of the potion, of course.");
	    } else {
		const char *ithem = obj->quan > 1L ? "them" : "it";
		verbalize("You burn %s, you bought %s!", ithem, ithem);
	    }
	    bill_dummy_object(obj);
	}
	makeknown(obj->otyp);
	if (obj->otyp == STICK_OF_DYNAMITE) obj->yours=TRUE;

	if (obj->quan > 1L) {
	    obj = splitobj(obj, 1L);
	    begin_burn(obj, FALSE);	/* burn before free to get position */
	    obj_extract_self(obj);	/* free from inv */

	    /* shouldn't merge */
	    obj = hold_another_object(obj, "You drop %s!",
				      doname(obj), (const char *)0);
	} else
	    begin_burn(obj, FALSE);
}

static NEARDATA const char cuddly[] = { TOOL_CLASS, GEM_CLASS, 0 };

int
dorub()
{
	if (MenuIsBugged) {
	pline("The rub command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	if (isevilvariant && (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) ) {
		pline("Well, you obviously don't have hands. How do you expect to be able to rub things???");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	struct obj *obj = getobj(cuddly, "rub");

	if (obj && obj->oclass == GEM_CLASS) {
	    if (is_graystone(obj)) {
		use_stone(obj);
		return 1;
	    } else {
		pline(FunnyHallu ? "You rub it over your hand... better stop before you hurt yourself." : "Sorry, I don't know how to use that.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	    }
	}

	if (!obj || !wield_tool(obj, "rub")) return 0;

	/* now uwep is obj */
	if (uwep->otyp == MAGIC_LAMP) {
	    if (uwep->spe > 0 && !rn2(3)) {
		check_unpaid_usage(uwep, TRUE);		/* unusual item use */
		djinni_from_bottle(uwep, rnd(4) );
		makeknown(MAGIC_LAMP);
		uwep->otyp = OIL_LAMP;
		uwep->spe = 0; /* for safety */
		uwep->age = rn1(500,1000);
		if (uwep->lamplit) begin_burn(uwep, TRUE);
		update_inventory();
	    } else if (rn2(2) && !Blind)
		You("see a puff of smoke.");
	    else {
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
	} else if (obj->otyp == BRASS_LANTERN || obj->otyp == DWARVEN_LANTERN || obj->otyp == DIM_LANTERN) {
	    /* message from Adventure */
	    pline("Rubbing the electric lamp is not particularly rewarding.");
	    pline("Anyway, nothing exciting happens.");
	} else {
		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
	}
	return 1;
}

int
dojump()
{
	/* Physical jump */
	return jump(0);
}

int
jump(magic)
int magic; /* 0=Physical, otherwise skill level; -5 = forcible move spell with very low range */
{
	coord cc;
	struct trap *trap;

	boolean minijump = FALSE;

	int jumpdistance = 0;

	if (magic == -5) {
		minijump = TRUE;
		magic = 1;
	}

	int jumpcost = 10;
	if (uarmf && uarmf->oartifact == ART_WORLD_CLASS_JUMP) {
		if (Role_if(PM_FEAT_MASTER)) jumpcost /= 5;
		else {
			jumpcost *= 4;
			jumpcost /= 5;
		}
	}

	if (jumpcost < 1) jumpcost = 1; /* fail safe */

	if (trap = t_at(u.ux, u.uy)) {
		if (trap->ttyp == VIVISECTION_TRAP) {
			You("are in vivisection, and therefore unable to jump!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 0;
		}
	}

	if (!magic && !Race_if(PM_TRANSFORMER) && (nolimbs(youmonst.data) || slithy(youmonst.data))) {
		/* normally (nolimbs || slithy) implies !Jumping,
		   but that isn't necessarily the case for knights */
		You_cant("jump; you have no legs!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	} else if (!magic && !Jumping && !(u.horsehopturns && u.usteed) ) {
		You_cant("jump very far.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	} else if (!magic && u.uen < jumpcost) { /* No longer completely free. --Amy */
		You("don't have enough energy to jump! You need at least %d points of mana!", jumpcost);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	} else if (u.uswallow) {
		if (magic) {
			You("bounce around a little.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 1;
		} else {
		pline(FunnyHallu ? "It's all wobbly here, like a ship on a stormy sea!" : "You've got to be kidding!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
		}
		return 0;
	} else if (u.uinwater) {
		if (magic) {
			You("swish around a little.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 1;
		} else {
		pline(FunnyHallu ? "You try to jump but that weird liquid stuff around you resists your attempts." : "This calls for swimming, not jumping!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
		}
		return 0;
	} else if (u.ustuck) {
		if (u.ustuck->mtame && !Conflict && !u.ustuck->mconf) {
		    You("pull free from %s.", mon_nam(u.ustuck));
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		    setustuck(0);
		    return 1;
		}
		if (magic) {
			You("writhe a little in the grasp of %s!", mon_nam(u.ustuck));
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 1;
		} else {
		You("cannot escape from %s!", mon_nam(u.ustuck));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
		}

		return 0;
	} else if (Levitation || Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)) {
		if (magic) {
			You("flail around a little.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 1;
		} else {
		You("don't have enough traction to jump.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
		}
	} else if (!magic && near_capacity() > UNENCUMBERED) {
		You("are carrying too much to jump!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	} else if (!magic && (u.uhunger <= 100 || ACURR(A_STR) < 6)) {
		You("lack the strength to jump!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	} else if (Wounded_legs) {
		if (u.usteed)
		    pline("%s is in no shape for jumping.", Monnam(u.usteed));
		else
		    Your("%s is in no shape for jumping.", body_part(LEG));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}
	else if (u.usteed && u.utrap) {
		pline("%s is stuck in a trap.", Monnam(u.usteed));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	}

	if (magic) {
		jumpdistance = 6 + magic * 3;
		if (uimplant && uimplant->oartifact == ART_GRAV_NODE_OF_SERAPHIS_VALE) jumpdistance += 2;
		if (StrongJumping) jumpdistance += 3;
	} else {
		jumpdistance = 9;
		if (uimplant && uimplant->oartifact == ART_GRAV_NODE_OF_SERAPHIS_VALE) jumpdistance += 2;
		if (StrongJumping) jumpdistance += 3;
	}

	pline("Where do you want to jump?");
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the desired position") < 0)
		return 0;	/* user pressed ESC */
	if (!magic && !(HJumping & ~INTRINSIC) && !EJumping &&
			distu(cc.x, cc.y) != 5) {
		/* The Knight jumping restriction still applies when riding a
		 * horse.  After all, what shape is the knight piece in chess?
		 */
		pline(FunnyHallu ? "The referee suggests you to try another move!" : "Illegal move!");
		return 0;
	} else if (minijump && distu(cc.x, cc.y) > 2) {
		pline(FunnyHallu ? "Now that would be a world-class jump." : "Too far!");
		return 0;
	} else if (distu(cc.x, cc.y) > jumpdistance) {
		pline(FunnyHallu ? "Now that would be a world-class jump." : "Too far!");
		return 0;
	} else if (!cansee(cc.x, cc.y)) {
		You(FunnyHallu ? "are too afraid of grues that might lurk over there!" : "cannot see where to land!");
		return 0;
	} else if (!isok(cc.x, cc.y)) {
		You(FunnyHallu ? "see a cactus over there! Better not jump into it..." : "cannot jump there!");
		return 0;
	} else {
	    coord uc;
	    int range, temp;

	    if(u.utrap)
		switch(u.utraptype) {
		case TT_BEARTRAP: {
		    register long side = rn2(3) ? LEFT_SIDE : RIGHT_SIDE;
		    You("rip yourself free of the bear trap!  Ouch!");
			if (!u.usteed)
		    losehp(rnd(10), "jumping out of a bear trap", KILLED_BY);
		    set_wounded_legs(side, HWounded_legs + rn1(1000,500));
		    break;
		  }
		case TT_PIT:
		    You("leap from the pit!");
		    break;
		case TT_GLUE:
		    You("try to leap, but the glue holds you in place.");
		    return 1;
		    break;
		case TT_WEB:
		    You("tear the web apart as you pull yourself free!");
		    deltrap(t_at(u.ux,u.uy));
		    break;
		case TT_LAVA:
		    You("pull yourself above the lava!");
		    u.utrap = 0;
		    return 1;
		case TT_INFLOOR:
		    You("strain your %s, but you're still stuck in the floor.",
			makeplural(body_part(LEG)));
		    set_wounded_legs(LEFT_SIDE, HWounded_legs + rn1(10, 11));
		    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rn1(10, 11));
		    return 1;
		}

	    /*
	     * Check the path from uc to cc, calling hurtle_step at each
	     * location.  The final position actually reached will be
	     * in cc.
	     */
	    uc.x = u.ux;
	    uc.y = u.uy;
	    /* calculate max(abs(dx), abs(dy)) as the range */
	    range = cc.x - uc.x;
	    if (range < 0) range = -range;
	    temp = cc.y - uc.y;
	    if (temp < 0) temp = -temp;
	    if (range < temp)
		range = temp;
	    (void) walk_path(&uc, &cc, hurtle_step, (void *)&range);

	    /* This used to give sokoban penalties but you can't actually bypass anything so the penalty is removed --Amy */
	    /* In Soviet Russia, you're even penalized for cheating if you didn't actually cheat at all. Communism dictates
	     * that we have to make extra sure, so we check for certain arbitrary actions that may have been considered
	     * cheating decades ago and reduce luck for those who perform them. Why modernize anything when you can continue
	     * using the ancient methods? */

		if (issoviet && In_sokoban(&u.uz)) {
			change_luck(-1);
			pline("Teper' vy teryayete ochko udachi KHAR KHAR. Eto deystviye ne pomoglo vam reshit' golovolomki, no my takiye elitnyye.");
			if (evilfriday) u.ugangr++;
		}

	    teleds(cc.x, cc.y, TRUE);

	if ( (sobj_at(ORCISH_SHORT_SWORD,cc.x,cc.y) || sobj_at(SHORT_SWORD,cc.x,cc.y) || sobj_at(FALCATA,cc.x,cc.y) || sobj_at(SILVER_SHORT_SWORD,cc.x,cc.y) || sobj_at(DWARVISH_SHORT_SWORD,cc.x,cc.y)  || sobj_at(ELVEN_SHORT_SWORD,cc.x,cc.y) || sobj_at(HIGH_ELVEN_WARSWORD,cc.x,cc.y)  || sobj_at(DARK_ELVEN_SHORT_SWORD,cc.x,cc.y)  || sobj_at(DROVEN_SHORT_SWORD,cc.x,cc.y)  || sobj_at(TRASH_SWORD,cc.x,cc.y) || sobj_at(VIBROBLADE,cc.x,cc.y)  || sobj_at(PARRY_SWORD,cc.x,cc.y)  || sobj_at(INKA_BLADE,cc.x,cc.y)  || sobj_at(SAND_SWORD,cc.x,cc.y)  || sobj_at(KHOPESH,cc.x,cc.y)  || sobj_at(MAGISWORD,cc.x,cc.y)  || sobj_at(SHORT_BLADE,cc.x,cc.y)  || sobj_at(ETERNIUM_BLADE,cc.x,cc.y)  || sobj_at(BROADSWORD,cc.x,cc.y)  || sobj_at(RUNESWORD,cc.x,cc.y)   || sobj_at(SUGUHANOKEN,cc.x,cc.y)   || sobj_at(ISAMUSEI,cc.x,cc.y)   || sobj_at(KACHISEI,cc.x,cc.y)   || sobj_at(MIRRORBLADE,cc.x,cc.y)   || sobj_at(BLADE_OF_MERCY,cc.x,cc.y)   || sobj_at(BLADE_OF_GRACE,cc.x,cc.y)   || sobj_at(GLADIUS,cc.x,cc.y)   || sobj_at(FLEURE,cc.x,cc.y)   || sobj_at(GREAT_HOUCHOU,cc.x,cc.y)   || sobj_at(BLACK_AESTIVALIS,cc.x,cc.y)  || sobj_at(VOLCANIC_BROADSWORD,cc.x,cc.y)  || sobj_at(ELEGANT_BROADSWORD,cc.x,cc.y)  || sobj_at(DARK_SWORD,cc.x,cc.y)  || sobj_at(PAPER_SWORD,cc.x,cc.y)  || sobj_at(MEATSWORD,cc.x,cc.y)  || sobj_at(WHITE_FLOWER_SWORD,cc.x,cc.y) || sobj_at(ELVEN_BROADSWORD,cc.x,cc.y)  || sobj_at(LONG_SWORD,cc.x,cc.y)  || sobj_at(SILVER_LONG_SWORD,cc.x,cc.y)  || sobj_at(CRYSTAL_SWORD,cc.x,cc.y)  || sobj_at(MAIN_SWORD,cc.x,cc.y)  || sobj_at(KATANA,cc.x,cc.y)  || sobj_at(BASILARD,cc.x,cc.y)  || sobj_at(OSBANE_KATANA,cc.x,cc.y)  || sobj_at(HONOR_KATANA,cc.x,cc.y)  || sobj_at(AUTOMATIC_KATANA,cc.x,cc.y)  || sobj_at(HEAVY_LONG_SWORD,cc.x,cc.y)  || sobj_at(ICKY_BLADE,cc.x,cc.y) || sobj_at(TINSEL_LONGSWORD,cc.x,cc.y)  || sobj_at(GRANITE_IMPALER,cc.x,cc.y)  || sobj_at(FLAME_MOUNTAIN,cc.x,cc.y)  || sobj_at(ELECTRIC_SWORD,cc.x,cc.y)  || sobj_at(TWO_HANDED_SWORD,cc.x,cc.y)  || sobj_at(TSURUGI,cc.x,cc.y)   || sobj_at(CHAINSWORD,cc.x,cc.y)   || sobj_at(ZANBATO,cc.x,cc.y)   || sobj_at(ORCISH_LONG_SWORD,cc.x,cc.y)   || sobj_at(FLOWING_SWORD,cc.x,cc.y)   || sobj_at(ULTRA_SWORD,cc.x,cc.y)   || sobj_at(UNNATURAL_SWORD,cc.x,cc.y)   || sobj_at(BASTERD_SWORD,cc.x,cc.y) || sobj_at(BIDENHANDER,cc.x,cc.y) || sobj_at(BUBBLETAR,cc.x,cc.y) || sobj_at(CUDSWORD,cc.x,cc.y) || sobj_at(ORGANOBLADE,cc.x,cc.y) || sobj_at(ESPANDON,cc.x,cc.y) || sobj_at(ROMAN_SWORD,cc.x,cc.y) || sobj_at(SHADOWBLADE,cc.x,cc.y) || sobj_at(ETHER_SAW,cc.x,cc.y) || sobj_at(COLOSSUS_BLADE,cc.x,cc.y) || sobj_at(DROVEN_GREATSWORD,cc.x,cc.y)  || sobj_at(SCIMITAR,cc.x,cc.y)  || sobj_at(BENT_SABLE,cc.x,cc.y)  || sobj_at(RAPIER,cc.x,cc.y)   || sobj_at(PLATINUM_SABER,cc.x,cc.y)  || sobj_at(WILD_BLADE,cc.x,cc.y)  || sobj_at(LEATHER_SABER,cc.x,cc.y)  || sobj_at(ARCANE_RAPIER,cc.x,cc.y) || sobj_at(INKUTLASS,cc.x,cc.y) || sobj_at(SHAMSHIR,cc.x,cc.y)  || sobj_at(HOE_SABLE,cc.x,cc.y)  || sobj_at(MYTHICAL_SABLE,cc.x,cc.y)  || sobj_at(DESERT_SWORD,cc.x,cc.y)  || sobj_at(CHROME_BLADE,cc.x,cc.y)  || sobj_at(YATAGAN,cc.x,cc.y)  || sobj_at(SILVER_SABER,cc.x,cc.y)  || sobj_at(GOLDEN_SABER,cc.x,cc.y)  || sobj_at(CROW_QUILL,cc.x,cc.y)  || sobj_at(RAKUYO,cc.x,cc.y)  || sobj_at(STINGING_SWORD,cc.x,cc.y)  || sobj_at(ORCISH_SCIMITAR,cc.x,cc.y)  || sobj_at(FALCHION,cc.x,cc.y)  || sobj_at(MOUNTAIN_SWORD,cc.x,cc.y)  || sobj_at(GREEN_SABER,cc.x,cc.y)  || sobj_at(TULWAR,cc.x,cc.y)  || sobj_at(CRYPTIC_SABER,cc.x,cc.y)  || sobj_at(ETERNIUM_SABER,cc.x,cc.y)  || sobj_at(IRON_SABER,cc.x,cc.y) ) && flags.iwbtg ) {

		u.youaredead = 1;
		killer = "a sharp-edged sword";		/* the thing that killed you */
		killer_format = KILLED_BY;
		pline("YOU JUMPED INTO A SWORD, YOU RETARD!");
		done(DIED);
		u.youaredead = 0;

	}

	    nomul(-1, "jumping around", TRUE);
	    nomovemsg = "";
	    morehungry(rnd(25));
	    if (!magic) u.uen -= jumpcost;
	    return 1;
	}
}

boolean
tinnable(corpse)
struct obj *corpse;
{
	if (corpse->otyp != CORPSE) return 0;
	if (corpse->oeaten) return 0;
	if (corpse->odrained) return 0;
	if (!mons[corpse->corpsenm].cnutrit) return 0;
	return 1;
}

STATIC_OVL void
use_tinning_kit(obj)
register struct obj *obj;
{
	register struct obj *corpse, *can;
/*
	char *badmove;
 */
	/* This takes only 1 move.  If this is to be changed to take many
	 * moves, we've got to deal with decaying corpses...
	 */
	if (obj->spe <= 0) {
		You(FunnyHallu ? "can't seem to generate anything. Weird..." : "seem to be out of tins.");
		if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}
	if (!(corpse = getobj((const char *)tinnables, "tin"))) return;
	if (corpse->otyp == CORPSE && (corpse->oeaten || corpse->odrained)) {
		You("cannot tin %s which is partly eaten.",something);
		return;
	}
	if (!tinnable(corpse)) {
		You_cant("tin that!");
		return;
	}
	if (touch_petrifies(&mons[corpse->corpsenm])
		&& (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && (!uarmg || FingerlessGloves) ) {
	    static char kbuf[BUFSZ];

	    if (poly_when_stoned(youmonst.data))
		You("tin %s without wearing gloves.",
			an(mons[corpse->corpsenm].mname));
	    else {
		pline("Tinning %s without wearing gloves is a fatal mistake...",
			an(mons[corpse->corpsenm].mname));
		sprintf(kbuf, "trying to tin a petrifying monster without gloves");
	    }
	    instapetrify(kbuf);
	}
	if (is_rider(&mons[corpse->corpsenm])) {
		(void) revive_corpse(corpse, FALSE);
		verbalize("Yes...  But War does not preserve its enemies...");
		return;
	}
	if (is_deadlysin(&mons[corpse->corpsenm])) {
		(void) revive_corpse(corpse, FALSE);
		verbalize("Sealing such a powerful evil in a can is never a good idea.");
		return;
	}
	if (mons[corpse->corpsenm].cnutrit == 0) {
		pline(FunnyHallu ? "Huh... those bits are going everywhere but into the tin..." : "That's too insubstantial to tin.");
		return;
	}

	int nochargechange = 10;
	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_DEVICES)) {
			default: break;
			case P_BASIC: nochargechange = 9; break;
			case P_SKILLED: nochargechange = 8; break;
			case P_EXPERT: nochargechange = 7; break;
			case P_MASTER: nochargechange = 6; break;
			case P_GRAND_MASTER: nochargechange = 5; break;
			case P_SUPREME_MASTER: nochargechange = 4; break;
		}
	}
	if (obj && obj->oartifact == ART_SPOIL_YOUR_STOMACH && rn2(2)) nochargechange = 0;

	if (nochargechange >= rnd(10)) consume_obj_charge(obj, TRUE);

	if ((can = mksobj(TIN, FALSE, FALSE, FALSE)) != 0) {
	    static const char you_buy_it[] = "You tin it, you bought it!";

	    can->corpsenm = corpse->corpsenm;
	    can->cursed = obj->cursed;
	    can->blessed = obj->blessed;

		/* evil patch idea by hackedhead: eroded tinning kits are less reliable */
		if ( (obj->oeroded == 3 || (obj->oeroded2 == 3 && !(objects[(obj)->otyp].oc_material == MT_COMPOST) ) ) && !rn2(2) ) {
			can->cursed = 1; can->blessed = 0;
		}
		else if ( (obj->oeroded == 2 || (obj->oeroded2 == 2 && !(objects[(obj)->otyp].oc_material == MT_COMPOST) ) ) && !rn2(5) ) {
			can->cursed = 1; can->blessed = 0;
		}
		else if ( (obj->oeroded == 1 || (obj->oeroded2 == 1 && !(objects[(obj)->otyp].oc_material == MT_COMPOST) ) ) && !rn2(10) ) {
			can->cursed = 1; can->blessed = 0;
		}
	    if (obj && obj->oartifact == ART_YASDORIAN_S_TROPHY_GETTER) {
		can->cursed = 0;
		can->blessed = 1;
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}
		(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
		u.aggravation = 0;

	    }
	    if (obj && obj->oartifact == ART_HOLY_CONSERVE) {
		can->cursed = 0;
		can->blessed = 1;
	    }
	    if (obj && obj->oartifact == ART_SPOIL_YOUR_STOMACH) {
		can->cursed = 1;
		can->blessed = 0;
	    }

	    can->owt = weight(can);
	    can->known = 1;
	    /* WAC You know the type of tinned corpses */
	    if (mvitals[corpse->corpsenm].eaten < 255) 
	    	mvitals[corpse->corpsenm].eaten++;
	    can->spe = -1;  /* Mark tinned tins. No spinach allowed... */
	    can->selfmade = TRUE;
	    if (carried(corpse)) {
		if (corpse->unpaid)
		    verbalize(you_buy_it);
		useup(corpse);
	    } else if (mcarried(corpse)) {
		m_useup(corpse->ocarry, corpse);
	    } else {
		if (costly_spot(corpse->ox, corpse->oy) && !corpse->no_charge)
		    verbalize(you_buy_it);
		useupf(corpse, 1L);
	    }
	    can = hold_another_object(can, "You make, but cannot pick up, %s.",
				      doname(can), (const char *)0);
		use_skill(P_DEVICES,1);
		if (Race_if(PM_FAWN)) {
			use_skill(P_DEVICES,1);
		}
		if (Race_if(PM_SATRE)) {
			use_skill(P_DEVICES,1);
			use_skill(P_DEVICES,1);
		}
	} else impossible("Tinning failed.");
}

STATIC_OVL void
use_binning_kit(obj)
register struct obj *obj;
{
	register struct obj *corpse, *can;
/*
	char *badmove;
 */
	/* This takes only 1 move.  If this is to be changed to take many
	 * moves, we've got to deal with decaying corpses...
	 */
	if (obj->spe <= 0) {
		if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
		You(FunnyHallu ? "can't seem to generate anything. Weird..." : "seem to be out of tins.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}
	if (!(corpse = getobj((const char *)tinnables, "tin"))) return;
	if (corpse->otyp == CORPSE && (corpse->oeaten || corpse->odrained)) {
		You("cannot tin %s which is partly eaten.",something);
		return;
	}
	if (!tinnable(corpse)) {
		You_cant("tin that!");
		return;
	}
	if (touch_petrifies(&mons[corpse->corpsenm])
		&& (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && (!uarmg || FingerlessGloves) ) {
	    static char kbuf[BUFSZ];

	    if (poly_when_stoned(youmonst.data))
		You("tin %s without wearing gloves.",
			an(mons[corpse->corpsenm].mname));
	    else {
		pline("Tinning %s without wearing gloves is a fatal mistake...",
			an(mons[corpse->corpsenm].mname));
		sprintf(kbuf, "trying to tin a petrifying monster without gloves");
	    }
	    instapetrify(kbuf);
	}
	if (is_rider(&mons[corpse->corpsenm])) {
		(void) revive_corpse(corpse, FALSE);
		verbalize("Yes...  But War does not preserve its enemies...");
		return;
	}
	if (is_deadlysin(&mons[corpse->corpsenm])) {
		(void) revive_corpse(corpse, FALSE);
		verbalize("Sealing such a powerful evil in a can is never a good idea.");
		return;
	}
	if (mons[corpse->corpsenm].cnutrit == 0) {
		pline(FunnyHallu ? "Huh... those bits are going everywhere but into the tin..." : "That's too insubstantial to tin.");
		return;
	}

	int nochargechange = 10;
	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_DEVICES)) {
			default: break;
			case P_BASIC: nochargechange = 9; break;
			case P_SKILLED: nochargechange = 8; break;
			case P_EXPERT: nochargechange = 7; break;
			case P_MASTER: nochargechange = 6; break;
			case P_GRAND_MASTER: nochargechange = 5; break;
			case P_SUPREME_MASTER: nochargechange = 4; break;
		}
	}

	if (nochargechange >= rnd(10)) consume_obj_charge(obj, TRUE);

	if ((can = mksobj(TIN, FALSE, FALSE, FALSE)) != 0) {
	    static const char you_buy_it[] = "You tin it, you bought it!";

	    can->corpsenm = NON_PM;
	    can->cursed = obj->cursed;
	    can->blessed = obj->blessed;

		/* evil patch idea by hackedhead: eroded tinning kits are less reliable */
		if ( (obj->oeroded == 3 || (obj->oeroded2 == 3 && !(objects[(obj)->otyp].oc_material == MT_COMPOST) ) ) && !rn2(2) ) {
			can->cursed = 1; can->blessed = 0;
		}
		else if ( (obj->oeroded == 2 || (obj->oeroded2 == 2 && !(objects[(obj)->otyp].oc_material == MT_COMPOST) ) ) && !rn2(5) ) {
			can->cursed = 1; can->blessed = 0;
		}
		else if ( (obj->oeroded == 1 || (obj->oeroded2 == 1 && !(objects[(obj)->otyp].oc_material == MT_COMPOST) ) ) && !rn2(10) ) {
			can->cursed = 1; can->blessed = 0;
		}

	    can->owt = weight(can);
	    can->known = 1;
	    can->spe = -1;  /* Mark tinned tins. No spinach allowed... */
	    can->selfmade = TRUE;
	    if (carried(corpse)) {
		if (corpse->unpaid)
		    verbalize(you_buy_it);
		useup(corpse);
	    } else if (mcarried(corpse)) {
		m_useup(corpse->ocarry, corpse);
	    } else {
		if (costly_spot(corpse->ox, corpse->oy) && !corpse->no_charge)
		    verbalize(you_buy_it);
		useupf(corpse, 1L);
	    }
	    can = hold_another_object(can, "You make, but cannot pick up, %s.",
				      doname(can), (const char *)0);
		use_skill(P_DEVICES,1);
		if (Race_if(PM_FAWN)) {
			use_skill(P_DEVICES,1);
		}
		if (Race_if(PM_SATRE)) {
			use_skill(P_DEVICES,1);
			use_skill(P_DEVICES,1);
		}
	    if (obj && obj->oartifact == ART_TRUE_GRIME) {
		adjalign(5);
		u.alignlim += 1;
		pline("Your alignment has increased, and is now %d. Your current maximum alignment is %d.", u.ualign.record, u.alignlim);
	    }
	    if (obj && obj->oartifact == ART_FERTILIZATOR && Role_if(PM_MILL_SWALLOWER)) {
			struct obj *uammo;
			int fertilammotyp = FLINT;
			switch (rnd(18)) {
				case 1: fertilammotyp = SALT_CHUNK; break;
				case 2: fertilammotyp = SILVER_SLINGSTONE; break;
				case 3: fertilammotyp = SMALL_PIECE_OF_UNREFINED_MITHR; break;
				case 4: fertilammotyp = TALC; break;
				case 5: fertilammotyp = GRAPHITE; break;
				case 6: fertilammotyp = VOLCANIC_GLASS_FRAGMENT; break;
				case 7: fertilammotyp = FLINT; break;
				case 8: fertilammotyp = LEAD_CLUMP; break;
				case 9: fertilammotyp = BONE_FRAGMENT; break;
				case 10: fertilammotyp = SLING_AMMO; break;
				case 11: fertilammotyp = CONUNDRUM_NUGGET; break;
				case 12: fertilammotyp = AMBER_FRAGMENT; break;
				case 13: fertilammotyp = METEOR_FRAGMENT; break;
				case 14: fertilammotyp = JUNK_METAL; break;
				case 15: fertilammotyp = COBALT_CHUNK; break;
				case 16: fertilammotyp = BRONZE_NUGGET; break;
				case 17: fertilammotyp = STEEL_SLUG; break;
				case 18: fertilammotyp = POINTY_SHELL; break;
				default : fertilammotyp = FLINT; break;
			}

			uammo = mksobj(fertilammotyp, TRUE, FALSE, FALSE);
			if (uammo) {
				uammo->quan = rn1(15, 15);
				if (uammo->quan < 1) uammo->quan = 1;
				uammo->known = uammo->dknown = uammo->bknown = uammo->rknown = 1;
				uammo->owt = weight(uammo);
				dropy(uammo);
				stackobj(uammo);
				You("created some sling ammo.");
			}

	    }

	} else impossible("Tinning failed.");
}


boolean
use_unicorn_horn(obj)
struct obj *obj;
{
#define PROP_COUNT 6		/* number of properties we're dealing with */
#define ATTR_COUNT (A_MAX*3)	/* number of attribute points we might fix */
	int idx, val, val_limit,
	    trouble_count, unfixable_trbl, did_prop, did_attr, did_atno;
	int trouble_list[PROP_COUNT + ATTR_COUNT];
	int chance;	/* KMH */
	int nochargechange = 0;

	if (obj && obj->oartifact == ART_DARKENING_THING) {
		if (!rn2(5)) pline("You produce an annoying sound.");
		wake_nearby();
		aggravate();
	}

	if (obj && obj->oartifact == ART_EFFECTERF && obj->spe < 15 && !rn2(1000)) {
		obj->spe++;
		Your("unicorn horn seems more effective.");
	}

	/* higher unihorn skill makes it a bit less likely to break --Amy */
	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_UNICORN_HORN)) {
			default: break;
			case P_BASIC: nochargechange = 1; break;
			case P_SKILLED: nochargechange = 2; break;
			case P_EXPERT: nochargechange = 3; break;
			case P_MASTER: nochargechange = 4; break;
			case P_GRAND_MASTER: nochargechange = 5; break;
			case P_SUPREME_MASTER: nochargechange = 6; break;
		}
	}

	/* higher chance for vaporizing the horn as a centaur --Amy */
	if (obj && !obj->oartifact && !(obj->otyp == TITANIUM_HORN && (rnd(5) > 2) ) && !(uwep && uwep->oartifact == ART_STAB_ALL_OVER && rn2(2)) && (rnd(8) > nochargechange) && !rn2(Race_if(PM_HUMANOID_CENTAUR) ? 10 : 100)) {

degradeagain:
	    if (obj->oartifact == ART_ATLUS_HEAVE) {
		curse(obj);
		obj->hvycurse = TRUE;
		pline(FunnyHallu ? "The tool is glowing in a wide array of colors!" : "Your unicorn horn seems less effective.");
	    } else if (obj->spe < 1) {
	    useup(obj);
	    pline(FunnyHallu ? "Suddenly, you hold some fine powder in your hands. Maybe you can smoke that for the extra kick?" : "The horn suddenly turns to dust.");
	    if (PlayerHearsSoundEffects) pline(issoviet ? "Podelom tebe, ty vechnyy neudachnik." : "Krrrrrtsch!");
		return 1;
	    } else {
		obj->spe -= 1;
		pline(FunnyHallu ? "The tool is glowing in a wide array of colors!" : "Your unicorn horn seems less effective.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		if (!rn2(3)) goto degradeagain;
		if(obj->blessed && !rn2(10) )
			unbless(obj);
		else if (!obj->blessed && !rn2(5))
			curse(obj);

	    }
	}
	if (obj && obj->oartifact && !(obj->otyp == TITANIUM_HORN && (rnd(5) > 2) ) && !(uwep && uwep->oartifact == ART_STAB_ALL_OVER && rn2(2)) && (rnd(8) > nochargechange) && !rn2(Race_if(PM_HUMANOID_CENTAUR) ? 100 : 10000)) {
	    if (obj->oartifact == ART_ATLUS_HEAVE) {
		curse(obj);
		obj->hvycurse = TRUE;
		pline(FunnyHallu ? "The tool is glowing in a wide array of colors!" : "Your unicorn horn seems less effective.");
	    } else if (obj->spe < 1) {
	    useup(obj);
	    pline(FunnyHallu ? "Suddenly, you hold some fine powder in your hands. Maybe you can smoke that for the extra kick?" : "The horn suddenly turns to dust.");
	    if (PlayerHearsSoundEffects) pline(issoviet ? "Podelom tebe, ty vechnyy neudachnik." : "Krrrrrtsch!");
		return 1;
	    } else {
		obj->spe -= 1;
		pline(FunnyHallu ? "The tool is glowing in a wide array of colors!" : "Your unicorn horn seems less effective.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		if(obj->blessed && !rn2(10) )
			unbless(obj);
		else if (!obj->blessed && !rn2(5))
			curse(obj);

	    }
	}

	if (obj && obj->cursed) {
	    long lcount = (long) rnd(100);

	    if (!obsidianprotection()) switch (rn2(11)) {
	    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
			xname(obj), TRUE, SICK_NONVOMITABLE);
		    break;
	    case 1: make_blinded(Blinded + lcount, TRUE);
		    break;
	    case 2: if (!Confusion)
			You("suddenly feel %s.",
			    FunnyHallu ? "trippy" : "confused");
		    make_confused(HConfusion + lcount, TRUE);
		    break;
	    case 3: make_stunned(HStun + lcount, TRUE);
		    break;
	    case 4: make_numbed(HNumbed + lcount, TRUE);
		    break;
	    case 5: make_frozen(HFrozen + lcount, TRUE);
		    break;
	    case 6: make_burned(HBurned + lcount, TRUE);
		    break;
	    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
		    break;
	    case 8: (void) make_hallucinated(HHallucination + lcount, TRUE, 0L);
		    break;
	    case 9: make_feared(HFeared + lcount, TRUE);
		    break;
	    case 10: make_dimmed(HDimmed + lcount, TRUE);
		    break;
	    }
	    badeffect(); /* otherwise it would be too easy to get confusion on demand --Amy */
	    return 0;
	}

/*
 * Entries in the trouble list use a very simple encoding scheme.
 */
#define prop2trbl(X)	((X) + A_MAX)
#define attr2trbl(Y)	(Y)
#define prop_trouble(X) trouble_list[trouble_count++] = prop2trbl(X)
#define attr_trouble(Y) trouble_list[trouble_count++] = attr2trbl(Y)

	trouble_count = unfixable_trbl = did_prop = did_attr = did_atno = 0;

	/* collect property troubles */
	if (Sick) prop_trouble(SICK);


	if (Blinded > (long)u.ucreamed) prop_trouble(BLINDED);
	if (HHallucination) prop_trouble(HALLUC);
	if (Vomiting && !(uarmf && uarmf->oartifact == ART_CRUEL_GODDESS_ANA) ) prop_trouble(VOMITING);
	if (HConfusion) prop_trouble(CONFUSION);
	if (HStun) prop_trouble(STUNNED); /* trying to prevent players from fixing everything */
	if (HNumbed) prop_trouble(NUMBED);
	if (HFrozen) prop_trouble(FROZEN);
	if (HBurned) prop_trouble(BURNED);
	if (HFeared) prop_trouble(FEARED);
	if (HDimmed) prop_trouble(DIMMED);

	unfixable_trbl = unfixable_trouble_count(TRUE);

	/* Amy edit: I decided that it would be nice if unicorn horns always tried to fix status effects first,
	 * since you don't care about that one lousy dexterity point if you're fighting Famine and he's stunlocking you.
	 * Also, attribute troubles can be permanently not-fixed in slex, and you might not want that either, yet you
	 * probably still want to use the unihorn to get rid of status problems, so there.
	 * In Soviet Russia it's par for the course if you get that dexterity point while deathly sick
	 * and then die from the sickness, naturally. */

	if (trouble_count && !issoviet) goto fixthings;

	/* collect attribute troubles */

	for (idx = 0; idx < A_MAX; idx++) {
	    val_limit = AMAX(idx);
	    /* don't recover strength lost from hunger */
	    if (idx == A_STR && u.uhs >= WEAK) val_limit--;
	    /* don't recover more than 3 points worth of any attribute */
	    if (val_limit > ABASE(idx) + 3) val_limit = ABASE(idx) + 3;

	    for (val = ABASE(idx); val < val_limit; val++)
		attr_trouble(idx);
	    /* keep track of unfixed trouble, for message adjustment below */
	    unfixable_trbl += (AMAX(idx) - val_limit);
	}  

fixthings:
	if (trouble_count == 0) {
	    if (!(powerfulimplants() && uimplant && uimplant->oartifact == ART_HEALENERATION)) {
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
	    return 0;
	} else if (trouble_count > 1) {		/* shuffle */
	    int i, j, k;

	    for (i = trouble_count - 1; i > 0; i--)
		if ((j = rn2(i + 1)) != i) {
		    k = trouble_list[j];
		    trouble_list[j] = trouble_list[i];
		    trouble_list[i] = k;
		}
	}

#if 0	/* Old NetHack success rate */
	/*
	 *		Chances for number of troubles to be fixed
	 *		 0	1      2      3      4	    5	   6	  7
	 *   blessed:  22.7%  22.7%  19.5%  15.4%  10.7%   5.7%   2.6%	 0.8%
	 *  uncursed:  35.4%  35.4%  22.9%   6.3%    0	    0	   0	  0
	 */
	val_limit = rn2( d(2, (obj && obj->blessed) ? 4 : 2) );
	if (val_limit > trouble_count) val_limit = trouble_count;
#else	/* KMH's new success rate */
	/*
	 * blessed:  Tries all problems, each with chance given below.
	 * uncursed: Tries one problem, with chance given below.
	 * ENCHANT  +0 or less  +1   +2   +3   +4   +5   +6 or more
	 * CHANCE       30%     40%  50%  60%  70%  80%     90%
	 */
	val_limit = (obj && obj->blessed) ? trouble_count : 1;
	if (obj && obj->spe > 0)
		chance = (obj->spe < 13) ? obj->spe+6 : 18;
	else
		chance = 6;
#endif

	if (Race_if(PM_HUMANOID_CENTAUR)) chance += 6; /* to offset the fact that it vanishes more often for them */

	/* unihorn skill should not be useless - make it improve the effectiveness --Amy */
	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_UNICORN_HORN)) {
			default: break;
			case P_BASIC: chance += 1; break;
			case P_SKILLED: chance += 2; break;
			case P_EXPERT: chance += 3; break;
			case P_MASTER: chance += 4; break;
			case P_GRAND_MASTER: chance += 5; break;
			case P_SUPREME_MASTER: chance += 6; break;
		}
	}

	if (obj && obj->otyp == REPLICA_UNICORN_HORN) chance -= 2;
	if (obj && obj->oartifact == ART_FFFF_ERR_WHAT_AM_I_TYPING_) chance += 2;

	if (obj && obj->oartifact == ART_CURE_GOOD) chance += 2;
	if (obj && obj->otyp == TITANIUM_HORN) chance += 4;
	if (uwep && uwep->oartifact == ART_STAB_ALL_OVER) chance++;

	if (isfriday && chance > 1) chance /= 2;

	if (chance < 1) chance = 1;
	if (chance > 18) chance = 18;
	if (issoviet && chance > 9) chance = 9;

	/* fix [some of] the troubles */
	for (val = 0; val < val_limit; val++) {
	    idx = trouble_list[val];

		if (rn2(issoviet ? 10 : 20) < chance)	/* KMH */
	    switch (idx) {
	    case prop2trbl(SICK):
		make_sick(0L, (char *) 0, TRUE, SICK_ALL);
		did_prop++;
		break;
	    case prop2trbl(BLINDED):
		make_blinded((long)u.ucreamed, TRUE);
		did_prop++;
		break;
	    case prop2trbl(HALLUC):
		(void) make_hallucinated(0L, TRUE, 0L);
		did_prop++;
		break;
	    case prop2trbl(VOMITING):
		make_vomiting(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(CONFUSION):
		make_confused(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(STUNNED):
		make_stunned(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(NUMBED):
		make_numbed(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(FEARED):
		make_feared(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(FROZEN):
		make_frozen(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(BURNED):
		make_burned(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(DIMMED):
		make_dimmed(0L, TRUE);
		did_prop++;
		break;
	    default:
		if (idx >= 0 && idx < A_MAX) {

	/* "Unihorns no longer permanently remove attributes. A bit too cruel, perhaps?" In Soviet Russia, people want stat-draining effects to be irrelevant, and the restore ability potion/spell to be useless. Therefore they make sure that you can easily restore everything for as many times as you like, so as soon as you have a unicorn horn, basically you've won already. And you're never gonna need that restore ability spell. This is obviously not what I had in mind, so I won't allow such cheese for any other races. --Amy */

		    if (rn2(3) || issoviet)
		    { ABASE(idx) += 1;
		    did_attr++;}
                else
		    { did_atno++;
                AMAX(idx) -= 1;}
		} else
		    panic("use_unicorn_horn: bad trouble? (%d)", idx);
		break;
	    }
	}

	if (did_attr)
	    pline("This makes you feel %s!",
		  (did_prop + did_attr) == (trouble_count + unfixable_trbl) ?
		  "great" : "better");
	else if (did_atno)
	    pline(FunnyHallu ? "Bummer! It just beeps loudly! You feel that this is a bad sign!" : "Damn! It didn't work and your stat point is lost forever!");
	else if (!did_prop)
	    pline("Nothing seems to happen.");

	if (issoviet && !rn2(3)) {
		/* Harharharharharharhar harharhar harhar! --Amy */
		pline("Tip bloka l'da govorit, chto slesh im eto der'mo!");
		badeffect();
	}

	flags.botl = (did_attr || did_prop || did_atno);

	if (did_attr || did_prop) u.cnd_unihorncount++;

#undef PROP_COUNT
#undef ATTR_COUNT
#undef prop2trbl
#undef attr2trbl
#undef prop_trouble
#undef attr_trouble

	/* the function has to return something; 0 means the horn is still there */
	return 0;
}

/* brick horn: restore temporarily drained stats, so that you don't have to wait them out --Amy */
boolean
use_brick_horn(obj)
struct obj *obj;
{
	if (obj && obj->cursed) {
		(void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
		return 0;
	}

	if (u.tsloss_str || u.tsloss_dex || u.tsloss_con || u.tsloss_wis || u.tsloss_int || u.tsloss_cha) {

		obj->spe--;
		if (obj->spe < 0) {
			int randospe = abs(obj->spe);
			if (randospe > rnd(20)) {
				useup(obj);
				pline(FunnyHallu ? "Suddenly, you hold some fine powder in your hands. Maybe you can smoke that for the extra kick?" : "The horn suddenly turns to dust.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Podelom tebe, ty vechnyy neudachnik." : "Krrrrrtsch!");
				return 1;
			}
		}

		if (obj && obj->oartifact == ART_FOOK_YOO_FOR_DRAINING_ME) {
			u.tsloss_str = 0;
			u.tsloss_dex = 0;
			u.tsloss_con = 0;
			u.tsloss_int = 0;
			u.tsloss_wis = 0;
			u.tsloss_cha = 0;
			You_feel("restored.");
		}

		restore_drained_stat();

		return 0;

	} else {
		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
	}

	/* the function has to return something; 0 means the horn is still there */
	return 0;
}

/* bubblehorn: restore damaged stats like regular horn, but not fix status effects */
boolean
use_bubble_horn(obj)
struct obj *obj;
{
	int i, ii, lim;
	int resulteffect = 0;

	if (obj && obj->cursed) {
		(void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
		return 0;
	}

	i = rn2(A_MAX);
	for (ii = 0; ii < A_MAX; ii++) {
		lim = AMAX(i);
		if (ABASE(i) < lim) {
			if (rn2(3)) {
				ABASE(i) += 1;
				pline("This makes you feel better!");
				flags.botl = 1;
				resulteffect = 1;
				break;
			} else {
				AMAX(i) -= 1;
				pline(FunnyHallu ? "Bummer! It just beeps loudly! You feel that this is a bad sign!" : "Damn! It didn't work and your stat point is lost forever!");
				resulteffect = 2;
				break;
			}
		}
		if(++i >= A_MAX) i = 0;

	}

	if (resulteffect) {
		obj->spe--;
		if (obj->spe < 0) {
			int randospe = abs(obj->spe);
			if (randospe > rnd(20)) {
				useup(obj);
				pline(FunnyHallu ? "Suddenly, you hold some fine powder in your hands. Maybe you can smoke that for the extra kick?" : "The horn suddenly turns to dust.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Podelom tebe, ty vechnyy neudachnik." : "Krrrrrtsch!");
				return 1;
			}
		}
		return 0;
	} else {
		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}

	}

	if (obj->spe > 0) {
		obj->spe--;
		adjattrib(rn2(A_MAX), 1, 0, TRUE);

	}

	/* the function has to return something; 0 means the horn is still there */
	return 0;
}

/*
 * Timer callback routine: turn figurine into monster
 */
void
fig_transform(arg, timeout)
void * arg;
long timeout;
{
	struct obj *figurine = (struct obj *)arg;
	struct monst *mtmp;
	coord cc;
	boolean cansee_spot, silent, okay_spot;
	boolean redraw = FALSE;
	char monnambuf[BUFSZ], carriedby[BUFSZ];

	if (!figurine) {
#ifdef DEBUG
	    pline("null figurine in fig_transform()");
#endif
	    return;
	}
	silent = (timeout != monstermoves); /* happened while away */
	okay_spot = get_obj_location(figurine, &cc.x, &cc.y, 0);
	if (figurine->where == OBJ_INVENT ||
	    figurine->where == OBJ_MINVENT)
		okay_spot = enexto(&cc, cc.x, cc.y,
				   &mons[figurine->corpsenm]);
	if (!okay_spot ||
	    !figurine_location_checks(figurine,&cc, TRUE)) {
		/* reset the timer to try again later */
		(void) start_timer((long)rnd(5000), TIMER_OBJECT,
				FIG_TRANSFORM, (void *)figurine);
		return;
	}

	cansee_spot = cansee(cc.x, cc.y);
	mtmp = make_familiar(figurine, cc.x, cc.y, TRUE, FALSE);
	if (mtmp) {
	    sprintf(monnambuf, "%s",an(m_monnam(mtmp)));
	    switch (figurine->where) {
		case OBJ_INVENT:
		    if (Blind)
			You_feel("%s %s from your pack!", something,
			    locomotion(mtmp->data,"drop"));
		    else
			You("see %s %s out of your pack!",
			    monnambuf,
			    locomotion(mtmp->data,"drop"));
		    break;

		case OBJ_FLOOR:
		    if (cansee_spot && !silent) {
			You("suddenly see a figurine transform into %s!",
				monnambuf);
			redraw = TRUE;	/* update figurine's map location */
		    }
		    break;

		case OBJ_MINVENT:
		    if (cansee_spot && !silent) {
			struct monst *mon;
			mon = figurine->ocarry;
			/* figurine carring monster might be invisible */
			if (canseemon(figurine->ocarry)) {
			    sprintf(carriedby, "%s pack",
				     s_suffix(a_monnam(mon)));
			}
			else if (is_pool(mon->mx, mon->my))
			    strcpy(carriedby, "empty water");
			else
			    strcpy(carriedby, "thin air");
			You("see %s %s out of %s!", monnambuf,
			    locomotion(mtmp->data, "drop"), carriedby);
		    }
		    break;
#if 0
		case OBJ_MIGRATING:
		    break;
#endif

		default:
		    impossible("figurine came to life where? (%d)",
				(int)figurine->where);
		break;
	    }
	}
	/* free figurine now */
	obj_extract_self(figurine);
	obfree(figurine, (struct obj *)0);
	if (redraw) newsym(cc.x, cc.y);
}

STATIC_OVL boolean
figurine_location_checks(obj, cc, quietly)
struct obj *obj;
coord *cc;
boolean quietly;
{
	xchar x,y;

	if (carried(obj) && u.uswallow) {
		if (!quietly)
			You("don't have enough room in here.");
		return FALSE;
	}
	x = cc->x; y = cc->y;
	if (!isok(x,y)) {
		if (!quietly)
			You("cannot put the figurine there.");
		return FALSE;
	}
	if (IS_ROCK(levl[x][y].typ) &&
	    !(passes_walls(&mons[obj->corpsenm]) && may_passwall(x,y))) {
		if (!quietly)
		    You("cannot place a figurine in %s!",
			IS_TREE(levl[x][y].typ) ? "a tree" : IS_FARMLAND(levl[x][y].typ) ? "a farmland" : IS_MOUNTAIN(levl[x][y].typ) ? "a mountain" : "solid rock");
		return FALSE;
	}
	if (sobj_at(BOULDER,x,y) && !passes_walls(&mons[obj->corpsenm])
			&& !throws_rocks(&mons[obj->corpsenm])) {
		if (!quietly)
			You("cannot fit the figurine on the boulder.");
		return FALSE;
	}
	return TRUE;
}

STATIC_OVL void
use_figurine(optr)
struct obj **optr;
{
	register struct obj *obj = *optr;
	xchar x, y;
	coord cc;

	if (u.uswallow) {
		/* can't activate a figurine while swallowed */
		if (!figurine_location_checks(obj, (coord *)0, FALSE))
			return;
	}
	if(!getdir((char *)0)) {
		flags.move = multi = 0;
		return;
	}
	x = u.ux + u.dx; y = u.uy + u.dy;
	cc.x = x; cc.y = y;
	/* Passing FALSE arg here will result in messages displayed */
	if (!figurine_location_checks(obj, &cc, FALSE)) return;
	You("%s and it transforms.",
	    (u.dx||u.dy) ? "set the figurine beside you" :
	    (Is_airlevel(&u.uz) || Is_waterlevel(&u.uz) ||
	     is_pool(cc.x, cc.y)) ?
		"release the figurine" :
	    (u.dz < 0 ?
		"toss the figurine into the air" :
		"set the figurine on the ground"));
	(void) make_familiar(obj, cc.x, cc.y, FALSE, FALSE);
	(void) stop_timer(FIG_TRANSFORM, (void *)obj);
	useup(obj);
	*optr = 0;
}

static NEARDATA const char lubricables[] = { ALL_CLASSES, ALLOW_NONE, 0 };
static NEARDATA const char need_to_remove_outer_armor[] =
			"need to remove your %s to grease your %s.";

STATIC_OVL void
use_grease(obj)
struct obj *obj;
{
	struct obj *otmp;
	char buf[BUFSZ];

	if (Glib) {
	    pline("%s from your %s.", Tobjnam(obj, "slip"),
		  makeplural(body_part(FINGER)));
	    dropx(obj);
	    return;
	}

	if (obj->spe > 0) {

		if (obj->oartifact == ART_SING_S_LAST_LAUGH) {

			int attempts = 0;
			struct permonst *pm = 0;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

newbossSING:
			do {
				pm = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!pm || (pm && !(pm->msound == MS_SHOE )) || (pm && !(type_is_pname(pm))) ) && attempts < 50000);

			if (!pm && rn2(50) ) {
				attempts = 0;
				goto newbossSING;
			}
			if (pm && !(pm->msound == MS_SHOE) && rn2(50) ) {
				attempts = 0;
				goto newbossSING;
			}
			if (pm && !(type_is_pname(pm)) && rn2(50) ) {
				attempts = 0;
				goto newbossSING;
			}

			if (pm) {
				struct monst *singbitch;
				singbitch = makemon(pm, 0, 0, MM_ANGRY); /* not frenzied --Amy */
				if (singbitch) singbitch->singannoyance = TRUE;
			}

			u.aggravation = 0;
			u.mondiffhack = 0;

		}

		if ((obj->cursed || Fumbling) && !rn2(2)) {
			consume_obj_charge(obj, TRUE);

			pline("%s from your %s.", Tobjnam(obj, "slip"),
			      makeplural(body_part(FINGER)));
			dropx(obj);
			return;
		}
		otmp = getobj(lubricables, "grease");
		if (!otmp) return;
		if ((otmp->owornmask & WORN_ARMOR) && uarmc) {
			strcpy(buf, xname(uarmc));
			You(need_to_remove_outer_armor, buf, xname(otmp));
			return;
		}
		if ((otmp->owornmask & WORN_SHIRT) && (uarmc || uarm)) {
			strcpy(buf, uarmc ? xname(uarmc) : "");
			if (uarmc && uarm) strcat(buf, " and ");
			strcat(buf, uarm ? xname(uarm) : "");
			You(need_to_remove_outer_armor, buf, xname(otmp));
			return;
		}
		int nochargechange = 10;
		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_DEVICES)) {
				default: break;
				case P_BASIC: nochargechange = 9; break;
				case P_SKILLED: nochargechange = 8; break;
				case P_EXPERT: nochargechange = 7; break;
				case P_MASTER: nochargechange = 6; break;
				case P_GRAND_MASTER: nochargechange = 5; break;
				case P_SUPREME_MASTER: nochargechange = 4; break;
			}
		}

		if ((nochargechange > rnd(10)) && !(obj->oartifact == ART_EURO_S_UNWASTE && rn2(2)) ) consume_obj_charge(obj, TRUE);

		if (stack_too_big(otmp)) {
			pline("The amount of grease was not enough for your stack of %s!", yname(otmp));
			return;
		}

		if (otmp != &zeroobj) {
			You("cover %s with a thick layer of grease.",
			    yname(otmp));
			if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
				if (!otmp->cursed) bless(otmp);
				else uncurse(otmp, FALSE);
			}
			if (obj && obj->otyp == LUBRICANT_CAN) otmp->greased += rn2(3);
			if (otmp->greased < 3) otmp->greased += 1;
			if (obj && obj->oartifact == ART_VIBE_LUBE) otmp->greased = 3;
			if (otmp->greased > 3) otmp->greased = 3; /* fail safe */
			if (obj->cursed && !nohands(youmonst.data)) {
			    incr_itimeout(&Glib, rnd(15));
			    flags.botl = TRUE;
			    pline("Some of the grease gets all over your %s.",
				makeplural(body_part(HAND)));
			}
			use_skill(P_DEVICES,1);
			if (Race_if(PM_FAWN)) {
				use_skill(P_DEVICES,1);
			}
			if (Race_if(PM_SATRE)) {
				use_skill(P_DEVICES,1);
				use_skill(P_DEVICES,1);
			}
		} else {
			Glib += rnd(15);
			You("coat your %s with grease.",
			    makeplural(body_part(FINGER)));
		}
	} else {
	    if (obj->known)
		pline("%s empty.", Tobjnam(obj, "are"));
	    else {
		pline("%s to be empty.", Tobjnam(obj, "seem"));
		if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
	    }

	    if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	}
	update_inventory();
}

static struct trapinfo {
	struct obj *tobj;
	xchar tx, ty;
	int time_needed;
	boolean force_bungle;
} trapinfo;

void
reset_trapset()
{
	trapinfo.tobj = 0;
	trapinfo.force_bungle = 0;
}

static struct whetstoneinfo {
	struct obj *tobj, *wsobj;
	int time_needed;
} whetstoneinfo;

void
reset_whetstone()
{
	whetstoneinfo.tobj = 0;
	whetstoneinfo.wsobj = 0;
}

/* occupation callback */
STATIC_PTR
int
set_whetstone()
{
	struct obj *otmp = whetstoneinfo.tobj, *ows = whetstoneinfo.wsobj;
	/* otmp = the object being worked on, ows = the whetstone itself */
	int chance;

	if (!otmp || !ows) {
	    reset_whetstone();
	    return 0;
	} else
	if (!carried(otmp) || !carried(ows)) {
	    You("seem to have mislaid %s.",
		!carried(otmp) ? yname(otmp) : yname(ows));
	    reset_whetstone();
	    return 0;
	}

	if (--whetstoneinfo.time_needed > 0) {
	    int adj = 2;
	    if (Blind) adj--;
	    if (Fumbling) adj--;
	    if (Confusion) adj--;
	    if (Stunned) adj--;
	    if (Numbed) adj--;
	    if (Feared) adj--;
	    if (Frozen) adj--;
	    if (Burned) adj--;
	    if (Dimmed) adj--;
	    if (Hallucination) adj--;
	    if (adj > 0)
		whetstoneinfo.time_needed -= adj;
	    return 1;
	}

	chance = 4 - (ows->blessed) + (ows->cursed*2) + (otmp->oartifact ? 3 : 0);
	if (ows->oartifact == ART_SHERPER_NOISE) chance /= 2;

	if (chance < 2) chance = 2;

	if (!rn2(chance) && (ows->otyp == WHETSTONE)) {

	    /* Remove rust first, then sharpen dull edges */
	    if (otmp->oeroded) {
		otmp->oeroded--;
		pline("%s %s%s now.", Yname2(otmp),
		    (Blind ? "probably " : (otmp->oeroded ? "almost " : "")),
		    otense(otmp, "shine"));
	    } else
	    if (otmp->spe < 0) {
		otmp->spe++;
		pline("%s %s %ssharper now.%s", Yname2(otmp),
		    otense(otmp, Blind ? "feel" : "look"),
		    (otmp->spe >= 0 ? "much " : ""),
		    Blind ? "  (Ow!)" : "");
	    }
	    makeknown(WHETSTONE);

		if (!rn2(ows->blessed ? 50 : ows->cursed ? 10 : 30) ) {
			pline("Your whetstone is too badly damaged and suddenly turns to dust.");
			useup(ows);
			return 0;
		}
	    reset_whetstone();
	} else {
	    if (FunnyHallu)
		pline("%s %s must be faulty!",
		    is_plural(ows) ? "These" : "This", xname(ows));
	    else pline("%s", Blind ? "Pheww!  This is hard work!" :
		"There are no visible effects despite your efforts.");
	    reset_whetstone();
	}

	return 0;
}

/* use stone on obj. the stone doesn't necessarily need to be a whetstone. */
STATIC_OVL void
use_whetstone(stone, obj)
struct obj *stone, *obj;
{
	boolean fail_use = TRUE;
	const char *occutext = "sharpening";
	int tmptime = 130 + (rnl(13) * 5);

	if (u.ustuck && sticks(youmonst.data)) {
	    You("should let go of %s first.", mon_nam(u.ustuck));
	} else
	if ((welded(uwep) && (uwep != stone)) ||
		(uswapwep && u.twoweap && welded(uswapwep) && (uswapwep != obj))) {
	    You("need both hands free.");
	} else
	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
	    You("can't handle %s with your %s.",
		an(xname(stone)), makeplural(body_part(HAND)));
	} else
	if (verysmall(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
	    You("are too small to use %s effectively.", an(xname(stone)));
	} else
#ifdef GOLDOBJ
	if (obj == &goldobj) {
	    pline("Shopkeepers would spot the lighter coin%s immediately.",
		obj->quan > 1 ? "s" : "");
	} else
#endif
	if (!is_waterypool(u.ux, u.uy) && !is_watertunnel(u.ux, u.uy) && !IS_FOUNTAIN(levl[u.ux][u.uy].typ)
	    && !IS_SINK(levl[u.ux][u.uy].typ) && !IS_TOILET(levl[u.ux][u.uy].typ)
	    ) {
	    if (carrying(POT_WATER) && objects[POT_WATER].oc_name_known) {
		pline(FunnyHallu ? "You'd probably just spill it everywhere." : "Better not waste bottled water for that.");
	    } else
		You(FunnyHallu ? "try to rub, but the stone just seems to get duller and duller?!" : "need some water when you use that.");
	} else
	if (Levitation && !Lev_at_will && !u.uinwater) {
	    You("can't reach the water.");
	} else
	    fail_use = FALSE;

	if (fail_use) {
	    reset_whetstone();
	    return;
	}

	if (stone == whetstoneinfo.wsobj && obj == whetstoneinfo.tobj &&
	    carried(obj) && carried(stone)) {
	    You("resume %s %s.", occutext, yname(obj));
	    set_occupation(set_whetstone, occutext, 0);
	    return;
	}

	if (obj) {
	    int ttyp = obj->otyp;
	    boolean isweapon = (obj->oclass == WEAPON_CLASS || is_weptool(obj));
	    boolean isedged = (is_pick(obj) ||
				(objects[ttyp].oc_dir & (PIERCE|SLASH)));
	    if (obj == &zeroobj) {
		You("file your nails.");
	    } else
	    if (!isweapon || !isedged) {
		pline("%s sharp enough already.",
			is_plural(obj) ? "They are" : "It is");
	    } else
	    if (stone->quan > 1) {
		pline("Using one %s is easier.", singular(stone, xname));
	    } else
	    if (obj->quan > 1) {
		You("can apply %s only on one %s at a time.",
		    the(xname(stone)),
		    (obj->oclass == WEAPON_CLASS ? "weapon" : "item"));
	    } else
	    if (!is_metallic(obj)) {
		pline("That would ruin the %s %s.",
			materialnm[objects[ttyp].oc_material],
		xname(obj));
	    } else
	    if (((obj->spe >= 0) || !obj->known) && !obj->oeroded) {
		pline("%s %s sharp and pointy enough.",
			is_plural(obj) ? "They" : "It",
			otense(obj, Blind ? "feel" : "look"));
	    } else {
		if (stone->cursed) tmptime *= 2;
		whetstoneinfo.time_needed = tmptime;
		whetstoneinfo.tobj = obj;
		whetstoneinfo.wsobj = stone;
		You("start %s %s.", occutext, yname(obj));
		set_occupation(set_whetstone, occutext, 0);
		if (IS_FOUNTAIN(levl[u.ux][u.uy].typ)) whetstone_fountain_effects(obj);
		else if (IS_SINK(levl[u.ux][u.uy].typ)) whetstone_sink_effects(obj);
		else if (IS_TOILET(levl[u.ux][u.uy].typ)) whetstone_toilet_effects(obj);
	    }
	} else You("wave %s in the %s.", the(xname(stone)),
	    (IS_POOL(levl[u.ux][u.uy].typ) && Underwater) ? "water" : "air");
}

/* touchstones - by Ken Arnold */
STATIC_OVL void
use_stone(tstone)
struct obj *tstone;
{
    struct obj *obj;
    boolean do_scratch;
    const char *streak_color, *choices;
    char stonebuf[QBUFSZ];
    static const char scritch[] = "\"scritch, scritch\"";
    static const char allowall[3] = { COIN_CLASS, ALL_CLASSES, 0 };
    static const char justgems[3] = { ALLOW_NONE, GEM_CLASS, 0 };
#ifndef GOLDOBJ
    struct obj goldobj;
#endif

    /* in case it was acquired while blinded */
    if (!Blind) tstone->dknown = 1;
    /* when the touchstone is fully known, don't bother listing extra
       junk as likely candidates for rubbing */
    choices = (tstone->otyp == TOUCHSTONE && tstone->dknown &&
		objects[TOUCHSTONE].oc_name_known) ? justgems : allowall;
    sprintf(stonebuf, "rub on the stone%s", plur(tstone->quan));
    if ((obj = getobj(choices, stonebuf)) == 0)
	return;
#ifndef GOLDOBJ
    if (obj->oclass == COIN_CLASS) {
	u.ugold += obj->quan;	/* keep botl up to date */
	goldobj = *obj;
	dealloc_obj(obj);
	obj = &goldobj;
    }
#endif

    if (obj == tstone && obj->quan == 1) {
	You_cant("rub %s on itself.", the(xname(obj)));
	return;
    }

    if (tstone->otyp == TOUCHSTONE && tstone->cursed &&
	    obj->oclass == GEM_CLASS && !is_graystone(obj) && !is_feminismstone(obj) &&
	    !obj_resists(obj, 80, 100)) {
	if (Blind)
	    You_feel("something shatter.");
	else if (FunnyHallu)
	    pline("Oh, wow, look at the pretty shards.");
	else
	    pline("A sharp crack shatters %s%s.",
		  (obj->quan > 1) ? "one of " : "", the(xname(obj)));
#ifndef GOLDOBJ
     /* assert(obj != &goldobj); */
#endif
	useup(obj);
	return;
    }

    if (Blind) {
	pline(scritch);
	return;
    } else if (Hallucination) {
	pline("Oh wow, man: Fractals!");
	return;
    }

    do_scratch = FALSE;
    streak_color = 0;

    if (Race_if(PM_PLAYABLE_NEANDERTHAL) && tstone->otyp == FLINT && !evades_destruction(obj) && (obj->oclass == SCROLL_CLASS || obj->oclass == SPBOOK_CLASS)) {

	if (obj->quan > 1) {
		obj->quan -= 1;
		obj->owt = weight(obj);
	} else {
		useupall(obj);
	}
	You("start a fire.");
	gain_alla(5);
	reducesanity(10);
	litroom(TRUE, tstone);

	return;
    }

    switch (obj->oclass) {
    case WEAPON_CLASS:
    case TOOL_CLASS:
	use_whetstone(tstone, obj);
	return;
    case GEM_CLASS:	/* these have class-specific handling below */
    case RING_CLASS:
	if (tstone->otyp != TOUCHSTONE) {
	    do_scratch = TRUE;
	} else if (obj->oclass == GEM_CLASS && (tstone->blessed || ((tstone->oartifact == ART_BERUEHRSKI_NIXI) && !tstone->cursed) || 
		(!tstone->cursed &&
		    (Role_if(PM_ARCHEOLOGIST) || Role_if(PM_GOLDMINER) || Race_if(PM_GNOME))))) {
	    makeknown(TOUCHSTONE);
	    makeknown(obj->otyp);
	    if ((tstone->oartifact == ART_BERUEHRSKI_NIXI) && !tstone->blessed) {
		randomfeminismtrap(5000);
	    }
	    if (!rn2(10)) {
			if (tstone->blessed) tstone->blessed = 0;
			else curse(tstone);
			pline("Your touchstone seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
	    }
	    prinv((char *)0, obj, 0L);
	    return;
	} else {
	    /* either a ring or the touchstone was not effective */
	    if (objects[obj->otyp].oc_material == MT_GLASS) {
		do_scratch = TRUE;
		break;
	    }
	}
	streak_color = c_obj_colors[objects[obj->otyp].oc_color];
	break;		/* gem or ring */

	/* Amy grepping target: "materialeffect" */
    default:
	switch (objects[obj->otyp].oc_material) {
	case MT_CLOTH:
	    pline("%s a little more polished now.", Tobjnam(tstone, "look"));
	    return;
	case MT_SILK:
	    pline("%s a little softer now.", Tobjnam(tstone, "look"));
	    return;
	case MT_COMPOST:
	    pline("%s a little dirtier now.", Tobjnam(tstone, "look"));
	    return;
	case MT_ETHER:
	    pline("%s seems contaminated.", Tobjnam(tstone, "look"));
	    return;
	case MT_SECREE:
	    pline("%s has gunk on it now.", Tobjnam(tstone, "look"));
	    return;
	case MT_LIQUID:
	    if (!obj->known)		/* note: not "whetstone" */
		You("must think this is a wetstone, do you?");
	    else
		pline("%s a little wetter now.", Tobjnam(tstone, "are"));
	    return;
	case MT_WAX:
	    streak_color = "waxy";
	    break;		/* okay even if not touchstone */
	case MT_WOOD:
	    streak_color = "wooden";
	    break;		/* okay even if not touchstone */
	case MT_GOLD:
	    do_scratch = TRUE;	/* scratching and streaks */
	    streak_color = "golden";
	    break;
	case MT_SILVER:
	    do_scratch = TRUE;	/* scratching and streaks */
	    streak_color = "silvery";
	    break;
	case MT_TAR:
	    do_scratch = TRUE;	/* scratching and streaks */
	    streak_color = "inky black";
	    break;
	case MT_VIVA:
	    do_scratch = TRUE;	/* scratching and streaks */
	    streak_color = "radiating";
	    break;
	default:
	    /* Objects passing the is_flimsy() test will not
	       scratch a stone.  They will leave streaks on
	       non-touchstones and touchstones alike. */
	    if (is_flimsy(obj))
		streak_color = c_obj_colors[objects[obj->otyp].oc_color];
	    else
		do_scratch = (tstone->otyp != TOUCHSTONE);
	    break;
	}
	break;		/* default oclass */
    }

    sprintf(stonebuf, "stone%s", plur(tstone->quan));
    if (do_scratch)
	pline("You make %s%sscratch marks on the %s.",
	      streak_color ? streak_color : (const char *)"",
	      streak_color ? " " : "", stonebuf);
    else if (streak_color)
	pline("You see %s streaks on the %s.", streak_color, stonebuf);
    else
	pline(scritch);
    return;
}

/* Place a landmine/bear trap.  Helge Hafting */
STATIC_OVL void
use_trap(otmp)
struct obj *otmp;
{
	int ttyp, tmp;
	const char *what = (char *)0;
	char buf[BUFSZ];
	const char *occutext = "setting the trap";

	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER))
	    what = "without hands";
	else if (Stunned)
	    what = "while stunned";
	else if (u.uswallow)
	    what = is_animal(u.ustuck->data) ? "while swallowed" :
			"while engulfed";
	else if (Underwater)
	    what = "underwater";
	else if (Levitation)
	    what = "while levitating";
	else if (is_pool(u.ux, u.uy))
	    what = "in water";
	else if (is_lava(u.ux, u.uy))
	    what = "in lava";
	else if (On_stairs(u.ux, u.uy))
	    what = (u.ux == xdnladder || u.ux == xupladder) ?
			"on the ladder" : "on the stairs";
	else if (IS_FURNITURE(levl[u.ux][u.uy].typ) ||
		IS_ROCK(levl[u.ux][u.uy].typ) ||
		closed_door(u.ux, u.uy) || (t_at(u.ux, u.uy) && !(otmp && otmp->oartifact == ART_PLACE_ON_TOP_OF_SNARE)) )
	    what = "here";
	if (what) {
	    You_cant("set a trap %s!",what);
	    reset_trapset();
	    return;
	}
	ttyp = (otmp->otyp == LAND_MINE) ? LANDMINE : BEAR_TRAP;
	if (otmp == trapinfo.tobj &&
		u.ux == trapinfo.tx && u.uy == trapinfo.ty) {
	    You("resume setting %s %s.",
		shk_your(buf, otmp),
		defsyms[trap_to_defsym(what_trap(ttyp))].explanation);
	    set_occupation(set_trap, occutext, 0);
	    return;
	}
	trapinfo.tobj = otmp;
	trapinfo.tx = u.ux,  trapinfo.ty = u.uy;
	tmp = ACURR(A_DEX);
	trapinfo.time_needed = (tmp > 17) ? 2 : (tmp > 12) ? 3 :
				(tmp > 7) ? 4 : 5;
	if (Blind) trapinfo.time_needed *= 2;
	tmp = ACURR(A_STR);
	if (ttyp == BEAR_TRAP && tmp < 18)
	    trapinfo.time_needed += (tmp > 12) ? 1 : (tmp > 7) ? 2 : 4;
	/*[fumbling and/or confusion and/or cursed object check(s)
	   should be incorporated here instead of in set_trap]*/
	if (u.usteed && (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) ) {
	    boolean chance;

	    if (Fumbling || otmp->cursed) chance = (rnl(10) > 3);
	    else  chance = (rnl(10) > 5);
	    You("aren't very skilled at reaching from %s.",
		mon_nam(u.usteed));
	    sprintf(buf, "Continue your attempt to set %s?",
		the(defsyms[trap_to_defsym(what_trap(ttyp))].explanation));
	    if(yn(buf) == 'y') {
		if (chance && !(otmp && otmp->oartifact == ART_PRECISION_DETONATING_STUFF) ) {
			switch(ttyp) {
			    case LANDMINE:	/* set it off */
			    	trapinfo.time_needed = 0;
			    	trapinfo.force_bungle = TRUE;
				break;
			    case BEAR_TRAP:	/* drop it without arming it */
				reset_trapset();
				You("drop %s!",
			  the(defsyms[trap_to_defsym(what_trap(ttyp))].explanation));
				dropx(otmp);
				return;
			}
		}
	    } else {
	    	reset_trapset();
		return;
	    }
	}
	You("begin setting %s %s.",
	    shk_your(buf, otmp),
	    defsyms[trap_to_defsym(what_trap(ttyp))].explanation);
	set_occupation(set_trap, occutext, 0);
	return;
}

STATIC_PTR
int
set_trap()
{
	struct obj *otmp = trapinfo.tobj;
	struct trap *ttmp, *ttsnare;
	int ttyp;

	if (!otmp || !carried(otmp) ||
		u.ux != trapinfo.tx || u.uy != trapinfo.ty) {
	    /* ?? */
	    reset_trapset();
	    return 0;
	}

	if (--trapinfo.time_needed > 0) return 1;	/* still busy */

	ttyp = (otmp->otyp == LAND_MINE) ? LANDMINE : BEAR_TRAP;

	if (otmp && otmp->oartifact == ART_PLACE_ON_TOP_OF_SNARE && (ttsnare = t_at(u.ux, u.uy)) ) {
		if (ttsnare->ttyp == MAGIC_PORTAL || ttsnare->ttyp == ONE_WAY_PORTAL) {
			pline("Somehow, setting the trap failed.");
			return 0;
		}
		deltrap(ttsnare);
		pline_The("existing snare disappeared!");
	}

	ttmp = maketrap(u.ux, u.uy, ttyp, 0, FALSE);

	if (otmp && otmp->oartifact == ART_CLICKFIELD) {
		int i, j, bd = 3;
		for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ != ROOM && levl[u.ux + i][u.uy + j].typ != CORR) continue;					if (t_at(u.ux + i, u.uy + j)) continue;
			if (i == u.ux && j == u.uy) continue;
			if (!rn2(2)) continue;
			maketrap(u.ux + i, u.uy + j, LANDMINE, 0, FALSE);
		}
		pline("Minefield created!");
	}

	if (otmp && otmp->oartifact == ART_WEBBUROUND) {
		int i, j, bd = 1;
		for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ != ROOM && levl[u.ux + i][u.uy + j].typ != CORR) continue;					if (t_at(u.ux + i, u.uy + j)) continue;
			if (i == u.ux && j == u.uy) continue;
			maketrap(u.ux + i, u.uy + j, WEB, 0, FALSE);
		}
		pline("Webs have been created around the bear trap.");
	}

	if (ttmp && !ttmp->hiddentrap) {
	    ttmp->tseen = 1;
	    ttmp->madeby_u = 1;
	    if (otmp && otmp->oartifact == ART_MOTHERFUCKING_BOMB) ttmp->launch_otyp = 33; /* see trap.c --Amy */
	    newsym(u.ux, u.uy); /* if our hero happens to be invisible */
	    if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
		add_damage(u.ux, u.uy, 0L);		/* schedule removal */
	    }

		if ((ttmp && ttmp->ttyp == LANDMINE) && practicantterror) {
			pline("%s booms: 'Practicants are not allowed to lay mines! Now you pay 1000 zorkmids!'", noroelaname());
			fineforpracticant(1000, 0, 0);
		}

	    if (!trapinfo.force_bungle)
		You("finish arming %s.",
			the(defsyms[trap_to_defsym(what_trap(ttyp))].explanation));
	    if (((otmp->cursed || Fumbling) && !(otmp && otmp->oartifact == ART_PRECISION_DETONATING_STUFF) && (rnl(10) > 5)) || trapinfo.force_bungle)
		dotrap(ttmp,
			(unsigned)(trapinfo.force_bungle ? FORCEBUNGLE : 0));

	} else {
	    /* this shouldn't happen */
	    Your("trap setting attempt fails.");
	}
	useup(otmp);
	reset_trapset();
	return 0;
}

STATIC_OVL int
use_whip(obj)
struct obj *obj;
{
    char buf[BUFSZ];
    struct monst *mtmp;
    struct obj *otmp;
    int rx, ry, proficient, res = 0;
    const char *msg_slipsfree = "The bullwhip slips free.";
    const char *msg_snap = "Snap!";

    if (obj != uwep) {
	if (!wield_tool(obj, "lash")) return 0;
	else res = 1;
    }
    if (!getdir((char *)0)) return res;

    if ((Stunned && !rn2(issoviet ? 1 : StrongStun_resist ? 20 : Stun_resist ? 8 : 2)) || (Confusion && !rn2(issoviet ? 2 : StrongConf_resist ? 200 : Conf_resist ? 40 : 8))) confdir();
    rx = u.ux + u.dx;
    ry = u.uy + u.dy;

	if(!isok(rx, ry)) { /* gotta fix that unneccessary segfault for once and for all! --Amy */

	pline(FunnyHallu ? "You get a great rebound effect!" : "Your whip hits an invisible barrier.");
	return(1);
	}

    mtmp = m_at(rx, ry);

    /* fake some proficiency checks */
    proficient = 0;
    if (Role_if(PM_ARCHEOLOGIST)) ++proficient;
    if (ACURR(A_DEX) < 6) proficient--;
    else if (ACURR(A_DEX) >= 14) proficient += (ACURR(A_DEX) - 14);
    if (Fumbling) --proficient;
    if (proficient > 3) proficient = 3;
    if (proficient < 0) proficient = 0;
	if (Race_if(PM_LEVITATOR) && !proficient) proficient = 1;

    if (u.uswallow && attack(u.ustuck)) {
	There("is not enough room to flick your bullwhip.");

    } else if (Underwater) {
	There("is too much resistance to flick your bullwhip.");

    } else if (u.dz < 0) {
	You("flick a bug off of the %s.",ceiling(u.ux,u.uy));

    } else if ((!u.dx && !u.dy) || (u.dz > 0)) {
	int dam;

	/* Sometimes you hit your steed by mistake */
	if (u.usteed && !rn2(proficient + 2)) {
	    You("whip %s!", mon_nam(u.usteed));
	    kick_steed();
	    return 1;
	}
	if (Levitation || Flying || u.usteed) {
	    /* Have a shot at snaring something on the floor */
	    otmp = level.objects[u.ux][u.uy];
	    if (otmp && otmp->otyp == CORPSE && otmp->corpsenm == PM_HORSE) {
		pline(FunnyHallu ? "Crack! Some messy stuff flies around." : "Why beat a dead horse?");
		return 1;
	    }
	    if (otmp && proficient) {
		You("wrap your bullwhip around %s on the %s.",
		    an(singular(otmp, xname)), surface(u.ux, u.uy));
		if (rnl(6 - proficient) || pickup_object(otmp, Race_if(PM_LEVITATOR) ? otmp->quan : 1L, TRUE, FALSE) < 1)
		    pline("%s", msg_slipsfree);
		return 1;
	    }
	}
	dam = rnd(2) + dbon() + obj->spe;
	if (dam <= 0) dam = 1;
	You("hit your %s with your bullwhip.", body_part(FOOT));
	sprintf(buf, "killed %sself with %s bullwhip", uhim(), uhis());
	losehp(dam, buf, NO_KILLER_PREFIX);
	flags.botl = 1;
	return 1;

    } else if ((Fumbling || IsGlib) && !rn2(5)) {
	pline_The("bullwhip slips out of your %s.", body_part(HAND));
	dropx(obj);

    } else if (u.utrap && u.utraptype == TT_PIT) {
	/*
	 *     Assumptions:
	 *
	 *	if you're in a pit
	 *		- you are attempting to get out of the pit
	 *		- or, if you are applying it towards a small
	 *		  monster then it is assumed that you are
	 *		  trying to hit it.
	 *	else if the monster is wielding a weapon
	 *		- you are attempting to disarm a monster
	 *	else
	 *		- you are attempting to hit the monster
	 *
	 *	if you're confused (and thus off the mark)
	 *		- you only end up hitting.
	 *
	 */
	const char *wrapped_what = (char *)0;

	if (mtmp) {
	    if (bigmonst(mtmp->data)) {
		wrapped_what = strcpy(buf, mon_nam(mtmp));
	    } else if (proficient) {
		if (attack(mtmp)) return 1;
		else pline("%s", msg_snap);
	    }
	}
	if (!wrapped_what) {
	    if (IS_FURNITURE(levl[rx][ry].typ))
		wrapped_what = something;
	    else if (sobj_at(BOULDER, rx, ry))
		wrapped_what = "a boulder";
	}
	if (wrapped_what) {
	    coord cc;

	    cc.x = rx; cc.y = ry;
	    You("wrap your bullwhip around %s.", wrapped_what);
	    if (proficient && rn2(proficient + 2)) {
		if (!mtmp || enexto(&cc, rx, ry, youmonst.data)) {
		    You("yank yourself out of the pit!");
		    teleds(cc.x, cc.y, TRUE);
		    u.utrap = 0;
		    vision_full_recalc = 1;
		}
	    } else {
		pline("%s", msg_slipsfree);
	    }
	    if (mtmp) wakeup(mtmp);
	} else pline("%s", msg_snap);

    } else if (mtmp) {
	if (!canspotmon(mtmp) && !(monstersoundtype(mtmp) == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember) &&
		!memory_is_invisible(rx, ry)) {
	   pline(FunnyHallu ? "Oh no, it hit some invisible barrier... or wait... that barrier must be a monster! Help!" : "A monster is there that you couldn't see.");
	   map_invisible(rx, ry);
	}
	otmp = MON_WEP(mtmp);	/* can be null */
	if (otmp) {
	    char onambuf[BUFSZ];
	    const char *mon_hand;
	    boolean gotit = proficient && (!Fumbling || !rn2(10));

	    strcpy(onambuf, cxname(otmp));
	    if (gotit) {
		mon_hand = mbodypart(mtmp, HAND);
		if (bimanual(otmp)) mon_hand = makeplural(mon_hand);
	    } else
		mon_hand = 0;	/* lint suppression */

	    You("wrap your bullwhip around %s %s.",
		s_suffix(mon_nam(mtmp)), onambuf);
	    if (gotit && otmp->cursed) {
		pline("%s welded to %s %s%c",
		      (otmp->quan == 1L) ? "It is" : "They are",
		      mhis(mtmp), mon_hand,
		      !otmp->bknown ? '!' : '.');
		otmp->bknown = 1;
		gotit = FALSE;	/* can't pull it free */
	    }
	    if (gotit) {

		int whipchance = 5; /* was waaaaaaay too easy to disarm foes --Amy */
		if (mtmp->m_lev > u.ulevel) whipchance += (mtmp->m_lev - u.ulevel);
		if (mtmp->data->geno & G_UNIQ) whipchance *= 5;
		if (whipchance < 5) whipchance = 5; /* fail safe */

		if (!rn2(50)) {
			setnotworn(obj);
			freeinv(obj);
			(void) mpickobj(mtmp,obj,FALSE);
			pline("%s laughs fiendishly, and snatches your weapon!", Monnam(mtmp));
			return 1;
		}

		if (otmp && otmp->mstartinventB && !(otmp->oartifact) && !(otmp->fakeartifact && timebasedlowerchance()) && (!rn2(4) || (rn2(100) < u.equipmentremovechance) || !timebasedlowerchance() ) ) {
			if (obj->spe > -20) obj->spe--;
			else curse(obj);
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(otmp));
			delobj(otmp);
			return 1;
		}
		if (otmp && otmp->mstartinventC && !(otmp->oartifact) && !(otmp->fakeartifact && !rn2(10)) && rn2(10) ) {
			if (obj->spe > -20) obj->spe--;
			else curse(obj);
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(otmp));
			delobj(otmp);
			return 1;
		}

		if (otmp && otmp->mstartinventE && !(otmp->oartifact) && !(otmp->fakeartifact && !rn2(20)) && rn2(20) ) {
			if (obj->spe > -20) obj->spe--;
			else curse(obj);
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(otmp));
			delobj(otmp);
			return 1;
		}

		if (otmp && otmp->mstartinventD && !(otmp->oartifact) && !(otmp->fakeartifact && !rn2(4)) && rn2(4) ) {
			if (obj->spe > -20) obj->spe--;
			else curse(obj);
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(otmp));
			delobj(otmp);
			return 1;
		}

		if (otmp && otmp->mstartinventX) {
			if (obj->spe > -20) obj->spe--;
			else curse(obj);
			You("vaporize %s %s!", s_suffix(mon_nam(mtmp)), xname(otmp));
			delobj(otmp);
			return 1;
		}

		if (rn2(whipchance)) {
			pline("%s", msg_slipsfree);
			return 1;
		}

		obj_extract_self(otmp);
		possibly_unwield(mtmp, FALSE);
		setmnotwielded(mtmp,otmp);

		switch (rn2(proficient + 1)) {
		case 2:
		default:
		    /* to floor near you */
		    You("yank %s %s to the %s!", s_suffix(mon_nam(mtmp)),
			onambuf, surface(u.ux, u.uy));
		    place_object(otmp, u.ux, u.uy);
		    stackobj(otmp);
		    break;
		case 3:
		    /* right to you */
#if 0
		    if (!rn2(25)) {
			/* proficient with whip, but maybe not
			   so proficient at catching weapons */
			int hitu, hitvalu;

			hitvalu = 8 + otmp->spe;
			hitu = thitu(hitvalu,
				     dmgval(otmp, &youmonst),
				     otmp, (char *)0);
			if (hitu) {
			    pline_The("%s hits you as you try to snatch it!",
				the(onambuf));
			}
			place_object(otmp, u.ux, u.uy);
			stackobj(otmp);
			break;
		    }
#endif /* 0 */
		    /* right into your inventory */
		    You("snatch %s %s!", s_suffix(mon_nam(mtmp)), onambuf);

		    if (otmp->otyp == PETRIFYIUM_BAR && (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			static char kbuf[BUFSZ];
			sprintf(kbuf, "a petrifyium bar");
			pline("Snatching a petrifyium bar is a fatal mistake.");
			instapetrify(kbuf);

		    }

		    if (otmp->otyp == PETRIFYIUM_BRA && (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			static char kbuf[BUFSZ];
			sprintf(kbuf, "a petrifyium bra");
			pline("Snatching a petrifyium bra is a fatal mistake.");
			instapetrify(kbuf);

		    }

		    if (otmp->otyp == CORPSE &&
			    touch_petrifies(&mons[otmp->corpsenm]) &&
			    (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
			    !(poly_when_stoned(youmonst.data) &&
				polymon(PM_STONE_GOLEM))) {
			static char kbuf[BUFSZ];

			sprintf(kbuf, "snatching a petrifying corpse");
			pline("Snatching this kind of corpse is a fatal mistake.");
			instapetrify(kbuf);
		    }
		    otmp = hold_another_object(otmp, "You drop %s!",
					       doname(otmp), (const char *)0);
		    break;
		}
	    } else {
		pline("%s", msg_slipsfree);
	    }
	    wakeup(mtmp);
	} else {
	    if (mtmp->m_ap_type &&
		!Protection_from_shape_changers && !sensemon(mtmp))
		stumble_onto_mimic(mtmp);
	    else You("flick your bullwhip towards %s.", mon_nam(mtmp));
	    if (proficient) {
		if (attack(mtmp)) return 1;
		else pline("%s", msg_snap);
	    }
	}

    } else if (Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)) {
	    /* it must be air -- water checked above */
	    You("snap your whip through thin air.");

    } else {
	pline("%s", msg_snap);

    }
    return 1;
}


static const char
	not_enough_room[] = "There's not enough room here to use that.",
	where_to_hit[] = "Where do you want to hit?",
	cant_see_spot[] = "won't hit anything if you can't see that spot.",
	cant_reach[] = "can't reach that spot from here.";

/* Distance attacks by pole-weapons */
STATIC_OVL int
use_pole (obj)
	struct obj *obj;
{
	int grindx, grindy;
	int res = 0, typ, max_range;
	int min_range = obj->otyp == FISHING_POLE ? 1 : 4;
	coord cc;
	struct monst *mtmp;
	struct obj *otmp;
	boolean fishing;
	int levitatorskill = 1;
	int effectvar = 0;

	if (Race_if(PM_LEVITATOR) && !(PlayerCannotUseSkills)) {

		switch (P_SKILL(P_POLEARMS)) {
			default: levitatorskill = 1; break;
			case P_BASIC: levitatorskill = 2; break;
			case P_SKILLED: levitatorskill = 4; break;
			case P_EXPERT: levitatorskill = 7; break;
			case P_MASTER: levitatorskill = 10; break;
			case P_GRAND_MASTER: levitatorskill = 20; break;
			case P_SUPREME_MASTER: levitatorskill = 50; break;
		}

	}


	/* Are you allowed to use the pole? */
	if (u.uswallow) {
	    pline(not_enough_room);
	    return (0);
	}
	if (obj != uwep) {
	    if (!wield_tool(obj, "swing")) return(0);
	    else res = 1;
	}

	/* Prompt for a location */
	pline(where_to_hit);
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the spot to hit") < 0)
	    return 0;	/* user pressed ESC */

#ifdef WEAPON_SKILLS
	/* Calculate range */
	typ = weapon_type(obj);
	if (typ == P_NONE || PlayerCannotUseSkills || P_SKILL(typ) <= P_BASIC) max_range = 4;
	else if (P_SKILL(typ) <= P_SKILLED) max_range = 5;
	else max_range = 8;
#else
	max_range = 8;
#endif

	typ = weapon_type(obj);
	if (obj->otyp == SPINED_BALL || (obj->otyp == AKLYS && !Role_if(PM_AKLYST)) || (obj->otyp == BLOW_AKLYS && !Role_if(PM_AKLYST)) || obj->otyp == CHAIN_AND_SICKLE) {
		if (PlayerCannotUseSkills || P_SKILL(typ) <= P_BASIC) max_range = 4;
		else if (P_SKILL(typ) <= P_SKILLED) max_range = 5;
		else max_range = 8;
	}
	if (obj->oartifact == ART_STRECKOUT_AND_PULL) max_range = 4;

	/* bonuses to polearm range are below this point --Amy */

	if (uarm && uarm->oartifact == ART_ONDONS) max_range += 2;

	if (obj->otyp == NOOB_POLLAX || obj->otyp == GREAT_POLLAX) max_range += 5;
	if (obj->oartifact == ART_SIMON_S_ULTRALONGNESS) max_range += 7;
	if (obj->otyp == YITH_TENTACLE) max_range += 2;
	if (obj->otyp == POLE_LANTERN) {
		max_range += 10;
		min_range += 5;
	}
	if (obj->oartifact == ART_PAYBACK_TIME) max_range += 8;
	if (obj->oartifact == ART_ETHER_PENETRATOR) max_range += 5;
	if (obj->oartifact == ART_FUURKER) max_range += 6;
	if (obj->otyp == WOODEN_BAR) max_range += 7;
	if (uarmf && uarmf->oartifact == ART_JONACE_S_TALLNESS) max_range += 4;
	if (obj->oartifact == ART_JAPANESE_WOMEN) min_range -= 3;
	if (obj->oartifact == ART_YOU_RE_STUCCO) min_range += 40; /* can never apply it successfully --Amy */
	if (obj->oartifact == ART_OVERLONG_STICK) {
		max_range += 12;
		min_range += 5;
	}

	if (uarmc && uarmc->oartifact == ART_POST_OFFICE_FARMER_HENG && max_range < 9) max_range = 9;

	if (distu(cc.x, cc.y) > max_range) {
	    pline(FunnyHallu ? "Your stick's not long enough, it seems!" : "Too far!");
	    return (res);
	} else if (distu(cc.x, cc.y) < min_range) {
	    pline(FunnyHallu ? "Your stick's too long, it seems!" : "Too close!");
	    return (res);
	} else if (!cansee(cc.x, cc.y) && !(obj->oartifact == ART_FOOK_THE_OBSTACLES) && !(obj->oartifact == ART_HIT_A_CORNER) && !(obj->oartifact == ART_BURGLED_NIGHT_SCYTHE) && obj->otyp != DARK_BAR &&
		   ((mtmp = m_at(cc.x, cc.y)) == (struct monst *)0 || !canseemon(mtmp))) {
	    You(cant_see_spot);
	    return (res);
	} else if (!couldsee(cc.x, cc.y) && !(obj->oartifact == ART_WEAKITE_THRUST) && !(obj->oartifact == ART_FOOK_THE_OBSTACLES) ) { /* Eyes of the Overworld */
	    You(cant_reach);
	    return res;
	}

	/* So you did successfully apply your pole. If you try to camp on an Elbereth, it should be scuffed now --Amy */
	u_wipe_engr(rnd(5));

	if (u.mushroompoles) u.mushroompoleused = TRUE;

	if (obj->oartifact == ART_BRIDGEBANE && find_drawbridge(&cc.x,&cc.y)) {
		destroy_drawbridge(cc.x,cc.y);
	}

	/* What is there? */
	mtmp = m_at(cc.x, cc.y);

	effectvar = 1;
	if (Race_if(PM_LEVITATOR)) {
		if (rn2(levitatorskill)) effectvar = 1 + rnd(2);
		else effectvar = 1 + rnd(4);
	} else {
		effectvar = rnd(6);
		if (uwep && uwep->oartifact == ART_LAKITU_GET_ME_OUT) effectvar = 1 + rnd(5);
	}

	if (obj->otyp == FISHING_POLE) {
	    fishing = is_pool(cc.x, cc.y);
	    /* Try a random effect */
	    switch (effectvar) {
		case 1:
		    /* Snag yourself */
		    You("hook yourself!");
		    losehp(rn1(10,10), "a fishing hook", KILLED_BY);
		    return 1;
		case 2:
		    /* Reel in a fish */
		    if (mtmp) {
			if ((bigmonst(mtmp->data) || strongmonst(mtmp->data))
				&& !rn2(2)) {
			    You("are yanked toward the %s", surface(cc.x,cc.y));
			    hurtle(sgn(cc.x-u.ux), sgn(cc.y-u.uy), 1, TRUE);
			    return 1;
			} else if (enexto(&cc, u.ux, u.uy, 0)) {
			    You("reel in %s!", mon_nam(mtmp));
			    mtmp->mundetected = 0;
			    rloc_to(mtmp, cc.x, cc.y);
			    return 1;
			}
		    }
		    break;
		case 3:
		    /* Snag an existing object */
		    if ((otmp = level.objects[cc.x][cc.y]) != (struct obj *)0) {
			You("snag an object from the %s!", surface(cc.x, cc.y));
			pickup_object(otmp, Race_if(PM_LEVITATOR) ? otmp->quan : 1, FALSE, FALSE);
			/* If pickup fails, leave it alone */
			newsym(cc.x, cc.y);
			return 1;
		    }
		    break;
		case 4:
		    /* Snag some garbage */
		    if (fishing && flags.boot_count < 1 &&
			    (otmp = mksobj(LOW_BOOTS, TRUE, FALSE, FALSE)) !=
			    (struct obj *)0) {
			flags.boot_count++;
			You("snag some garbage from the %s!",
				surface(cc.x, cc.y));
			if (pickup_object(otmp, 1, FALSE, FALSE) <= 0) {
			    obj_extract_self(otmp);
			    place_object(otmp, u.ux, u.uy);
			    newsym(u.ux, u.uy);
			}
			return 1;
		    }
		    /* Or a rat in the sink/toilet */
		    if (!(mvitals[PM_SEWER_RAT].mvflags & G_GONE) &&
			    (IS_SINK(levl[cc.x][cc.y].typ) ||
			    IS_TOILET(levl[cc.x][cc.y].typ))) {
			mtmp = makemon(&mons[PM_SEWER_RAT], cc.x, cc.y,
				NO_MM_FLAGS);
			pline("Eek!  There's %s there!",
				Blind ? "something squirmy" : a_monnam(mtmp));
			return 1;
		    }
		    break;
		case 5:
		    /* Catch your dinner */
		    if (fishing && flags.cram_count < 50 && (otmp = mksobj(CRAM_RATION, TRUE, FALSE, FALSE)) !=
			    (struct obj *)0) {
			flags.cram_count++; /* I swear I implemented that once already, but apparently the change got eaten,
							just like that annoying polearms code... --Amy */
			You("catch tonight's dinner!");
			if (pickup_object(otmp, 1, FALSE, FALSE) <= 0) {
			    obj_extract_self(otmp);
			    place_object(otmp, u.ux, u.uy);
			    newsym(u.ux, u.uy);
			}
			return 1;
		    }
		    break;
		default:
		case 6:
		    /* Untrap */
		    /* FIXME -- needs to deal with non-adjacent traps */
		    break;
	    }
	}

	/* The effect didn't apply.  Attack the monster there. */
	if (mtmp) {

		if (obj->oartifact == ART_DESTRUCTION_BALL && !rn2(3)) {
			if (obj->spe > -20) {
				obj->spe--;
				pline("Your ball sustains damage.");
			} else {
				uwepgone();              /* set unweapon */
				pline(FunnyHallu ? "You lost your stick!" : "Your weapon shatters into pieces!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu u vas takoy malen'kiy polovogo chlena v lyubom sluchaye?" : "Krrrrrrrtsch!");
				useup(obj);
				return (1);

			}
		}
		if (obj->oartifact == ART_DONNNNNNNNNNNNG && !rn2(3)) {
			if (obj->spe > -20) {
				obj->spe--;
				pline("Your weapon sustains damage.");
			} else {
				uwepgone();              /* set unweapon */
				pline(FunnyHallu ? "You lost your stick!" : "Your weapon shatters into pieces!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu u vas takoy malen'kiy polovogo chlena v lyubom sluchaye?" : "Krrrrrrrtsch!");
				useup(obj);
				return (1);

			}
		}
		if (obj->oartifact == ART_SIGIX_BROADSWORD && !rn2(20)) {
			if (obj->spe > -20) {
				obj->spe--;
				pline("Your weapon sustains damage.");
			} else {
				uwepgone();              /* set unweapon */
				pline(FunnyHallu ? "You lost your stick!" : "Your weapon shatters into pieces!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu u vas takoy malen'kiy polovogo chlena v lyubom sluchaye?" : "Krrrrrrrtsch!");
				useup(obj);
				return (1);

			}
		}
		if (obj->oartifact == ART_DULL_METAL && !rn2(50)) {
			if (obj->spe > -20) {
				obj->spe--;
				pline("Your weapon sustains damage.");
			} else {
				uwepgone();              /* set unweapon */
				pline(FunnyHallu ? "You lost your stick!" : "Your weapon shatters into pieces!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu u vas takoy malen'kiy polovogo chlena v lyubom sluchaye?" : "Krrrrrrrtsch!");
				useup(obj);
				return (1);

			}
		}

	    if ((!rn2(isfriday ? 500 : 1000) && !obj->oartifact) || (!rn2(isfriday ? 125 : 250) && obj->otyp == WOODEN_BAR && (!obj->oartifact || !rn2(10))) || (!rn2(isfriday ? 125 : 250) && (obj->otyp == AKLYS || obj->otyp == BLOW_AKLYS) && (!obj->oartifact || !rn2(10))) || (!rn2(isfriday ? 5000 : 10000) && obj->oartifact)) {
		if (obj->oartifact == ART_FONLAUSCHI && obj->spe < 1) {
			if (obj->spe > -20) obj->spe--;
			pline(FunnyHallu ? "Your stick seems shorter now!" : "Your weapon seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		} else if (obj->oartifact == ART_NOBREAK && obj->spe < 1) {
			if (obj->spe > -20) obj->spe--;
			pline(FunnyHallu ? "Your stick seems shorter now!" : "Your weapon seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		} else if (obj->spe < 1) {
			uwepgone();              /* set unweapon */
			pline(FunnyHallu ? "You lost your stick!" : "Your weapon shatters into pieces!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu u vas takoy malen'kiy polovogo chlena v lyubom sluchaye?" : "Krrrrrrrtsch!");
			useup(obj);
			return (1);
		} else {
			obj->spe -= rnd(obj->spe);
			pline(FunnyHallu ? "Your stick seems shorter now!" : "Your weapon seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		}
	    }

	    if ((!rn2(isfriday ? 500 : 1000) && !obj->oartifact) || (!rn2(isfriday ? 125 : 250) && obj->otyp == WOODEN_BAR && (!obj->oartifact || !rn2(10))) || (!rn2(isfriday ? 125 : 250) && (obj->otyp == AKLYS || obj->otyp == BLOW_AKLYS) && (!obj->oartifact || !rn2(10))) || (!rn2(isfriday ? 5000 : 10000) && obj->oartifact)) {

		if (obj->oartifact == ART_FONLAUSCHI && (obj->oeroded >= 3 || obj->oeroded2 >= 3)) {
			if (obj->spe > -20) obj->spe--;
			pline(FunnyHallu ? "Your stick seems shorter now!" : "Your weapon seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		} else if (obj->oartifact == ART_NOBREAK && (obj->oeroded >= 3 || obj->oeroded2 >= 3)) {
			if (obj->spe > -20) obj->spe--;
			pline(FunnyHallu ? "Your stick seems shorter now!" : "Your weapon seems less effective.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		} else if (obj->oeroded >= 3 || obj->oeroded2 >= 3) {
			uwepgone();              /* set unweapon */
			pline(FunnyHallu ? "You lost your stick!" : "Your weapon shatters into pieces!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu u vas takoy malen'kiy polovogo chlena v lyubom sluchaye?" : "Krrrrrrrtsch!");
			useup(obj);
			return (1);
		}

		(void)wither_dmg(obj, xname(obj), rn2(4), TRUE, &youmonst);

	    }

	    if ((!rn2(isfriday ? 50 : 100) || (!rn2(isfriday ? 12 : 25) && obj->otyp == WOODEN_BAR) || (!rn2(isfriday ? 12 : 25) && (obj->otyp == AKLYS || obj->otyp == BLOW_AKLYS))) && !mtmp->mpeaceful && !mtmp->mtame && !mtmp->mfrenzied && !(amorphous(mtmp->data) || notake(mtmp->data) || nolimbs(mtmp->data) ) ) {
		mtmp->mfrenzied = 1;
		pline("%s is frenzied!", Monnam(mtmp));
	    }

	    if ((!rn2(isfriday ? 50 : 100) || (uarm && uarm->oartifact == ART_RHIANNON_S_RAINCLOUD && uarm->otyp != WEATHER_DRAGON_SCALES && !rn2(isfriday ? 50 : 100) ) || (!rn2(isfriday ? 12 : 25) && obj->otyp == WOODEN_BAR) || (!rn2(isfriday ? 12 : 25) && (obj->otyp == AKLYS || obj->otyp == BLOW_AKLYS))) && !mtmp->mpeaceful && !mtmp->mtame && !(amorphous(mtmp->data) || notake(mtmp->data) || nolimbs(mtmp->data) ) ) {
		mon_adjust_speed(mtmp, 1, (struct obj *)0);
	    }

	    if ((!rn2(isfriday ? 100 : 200) || (!rn2(isfriday ? 12 : 25) && obj->otyp == WOODEN_BAR) || (!rn2(isfriday ? 12 : 25) && (obj->otyp == AKLYS || obj->otyp == BLOW_AKLYS))) && attacktype(mtmp->data, AT_WEAP)) {
		pline("%s laughs fiendishly, and snatches your weapon!", Monnam(mtmp));
		setnotworn(obj);
		freeinv(obj);
		(void) mpickobj(mtmp,obj,FALSE);
		return (1);
	    }

	    if ((!rn2(isfriday ? 125 : 250) || (!rn2(10) && mtmp->data == &mons[PM_GREUVENENE] ) || (!rn2(isfriday ? 25 : 50) && obj->otyp == WOODEN_BAR) || (!rn2(isfriday ? 25 : 50) && (obj->otyp == AKLYS || obj->otyp == BLOW_AKLYS))) && humanoid(mtmp->data) && (avoid_player(mtmp->data) || mtmp->egotype_avoider) ) {
			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			u.heavyaggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {
				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			u.heavyaggravation = 0;
			pline("%s calls you out for using such devious weapons and uses countermeasures...", Monnam(mtmp));
			pline("Several monsters come out of a portal.");

	    }

	    int oldhp = mtmp->mhp;

	    bhitpos = cc;
	    check_caitiff(mtmp);
	    grindx = mtmp->mx;
	    grindy = mtmp->my;

	    (void) thitmonst(mtmp, uwep, 1, TRUE);


	    /* check the monster's HP because thitmonst() doesn't return
	     * an indication of whether it hit.  Not perfect (what if it's a
	     * non-silver weapon on a shade?)
	     */
	    if (mtmp->mhp < oldhp) {
		u.uconduct.weaphit++;

			if (obj && objects[obj->otyp].oc_skill == (tech_inuse(T_GRAP_SWAP) ? P_LANCE : P_GRINDER)) {
				int grindirection = 0;
				if (grindx > u.ux && grindy == u.uy) grindirection = 1; /* east */
				if (grindx > u.ux && grindy > u.uy) grindirection = 2; /* southeast */
				if (grindx == u.ux && grindy < u.uy) grindirection = 3; /* north */
				if (grindx < u.ux && grindy < u.uy) grindirection = 4; /* northwest */
				if (grindx < u.ux && grindy == u.uy) grindirection = 5; /* west */
				if (grindx < u.ux && grindy > u.uy) grindirection = 6; /* southwest */
				if (grindx == u.ux && grindy > u.uy) grindirection = 7; /* south */
				if (grindx > u.ux && grindy < u.uy) grindirection = 8; /* northeast */
				if (grindirection >= 1 && grindirection <= 8) grinderattack(grindirection);
			}

		    if (obj && obj->oartifact == ART_RIGHTLASH_LEFT && !rn2(100) && obj->spe < 15) {
			obj->spe++;
			pline("Your weapon seems sharper!");
		    }

		    if (obj && obj->oartifact == ART_HEY_THERE && mtmp && mtmp->mhpmax > 1) {
			mtmp->mhpmax--;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		    }

		    if (obj && obj->oartifact == ART_ELEMENTS_TIME_THREE && isok(u.ux, u.uy) && !rn2(100)) {
			int melteestrength = 5 + (GushLevel / 4);

			switch (rnd(8)) {
				case 1:
					buzz(15, melteestrength, u.ux, u.uy, -1, 0);
					break;
				case 2:
					buzz(15, melteestrength, u.ux, u.uy, 1, 0);
					break;
				case 3:
					buzz(15, melteestrength, u.ux, u.uy, -1, 1);
					break;
				case 4:
					buzz(15, melteestrength, u.ux, u.uy, 1, 1);
					break;
				case 5:
					buzz(15, melteestrength, u.ux, u.uy, 0, 1);
					break;
				case 6:
					buzz(15, melteestrength, u.ux, u.uy, -1, -1);
					break;
				case 7:
					buzz(15, melteestrength, u.ux, u.uy, 1, -1);
					break;
				case 8:
					buzz(15, melteestrength, u.ux, u.uy, 0, -1);
					break;
			}

		    }

		    if (obj && obj->oartifact == ART_ROYAL_CASINO_BETS) {

			if (!rn2(1000) && obj->spe < 5) {
				obj->spe += rnd(5);
				if (obj->spe > 5) obj->spe = 5;
				pline("Ding, we have a winner! Your weapon is +%d now.", obj->spe);
			}
			if (!rn2(10000) && obj->spe < 10) {
				obj->spe += rnd(10);
				if (obj->spe > 10) obj->spe = 10;
				pline("Awesome, you scored a big win!!! Now your weapon's enchantmend was boosted to +%d!", obj->spe);
			}
			if (!rn2(100000) && obj->spe < 15) {
				obj->spe += rnd(15);
				if (obj->spe < 5) obj->spe = 5;
				if (obj->spe > 15) obj->spe = 15;
				pline("Way to go, %s! You're an extremely lucky individual and your weapon has an awesome +%d enchantment now!!!", playeraliasname, obj->spe);
			}
			if (!rn2(1000000) && obj->spe < 20) {
				obj->spe += rnd(20);
				if (obj->spe < 12) obj->spe = 12;
				if (obj->spe > 20) obj->spe = 20;
				pline("JACKPOT!!!!!!! %s, you're the new champion of the Royal Casino and as a reward we boost your weapon's enchantment all the way to %d!!! Far out, you beat the impossible odds!", playeraliasname, obj->spe);
				change_luck(20);
			}

		    }

		    if (obj && obj->oartifact == ART_HEALHEALHEALHEAL) {
			healup(1, 0, FALSE, FALSE);
		    }

		    if (obj && obj->oartifact == ART_HIGH_ROLLER_S_LUCK && !rn2(100) && obj->spe > 0) {
			obj->spe--;
			pline("Your weapon seems less sharp!");
		    }
		    if (obj && obj->oartifact == ART_HIGH_ROLLER_S_LUCK && !rn2(2000) && obj->spe < 21) {
			obj->spe += 10;
			pline("Your weapon seems very sharp!");
		    }

	    }
	} else {
		/* Now you know that nothing is there... */
		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
	}
	u.mushroompoleused = 0;
	return (1);
}

STATIC_OVL int
use_cream_pie(obj)
struct obj *obj;
{
	boolean wasblind = Blind;
	boolean wascreamed = u.ucreamed;
	boolean several = FALSE;

	if (obj->quan > 1L) {
		several = TRUE;
		obj = splitobj(obj, 1L);
	}
	if (FunnyHallu)
		You("give yourself a facial.");
	else
		pline("You immerse your %s in %s%s.", body_part(FACE),
			several ? "one of " : "",
			several ? makeplural(the(xname(obj))) : the(xname(obj)));
	if(can_blnd((struct monst*)0, &youmonst, AT_WEAP, obj)) {
		int blindinc = rnd(25);
		u.ucreamed += blindinc;
		make_blinded(Blinded + (long)blindinc, FALSE);
		if (!Blind || (Blind && wasblind))
			pline("There's %ssticky goop all over your %s.",
				wascreamed ? "more " : "",
				body_part(FACE));
		else /* Blind  && !wasblind */
			You_cant("see through all the sticky goop on your %s.",
				body_part(FACE));
	}
	if (obj->unpaid) {
		verbalize("You used it, you bought it!");
		bill_dummy_object(obj);
	}
	obj_extract_self(obj);
	delobj(obj);
	return(0);
}

STATIC_OVL int
use_grapple (obj)
	struct obj *obj;
{
	int res = 0, typ, max_range = 4, tohit;
	coord cc;
	struct monst *mtmp;
	struct obj *otmp;

	/* Are you allowed to use the hook? */
	if (u.uswallow) {
	    pline(not_enough_room);
	    return (0);
	}
	if (obj != uwep) {
	    if (!wield_tool(obj, "cast")) return(0);
	    else res = 1;
	}
     /* assert(obj == uwep); */

	/* Prompt for a location */
	pline(where_to_hit);
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the spot to hit") < 0)
	    return 0;	/* user pressed ESC */

	/* Calculate range */
	typ = uwep_skill_type();
	if (typ == P_NONE || PlayerCannotUseSkills || P_SKILL(typ) <= P_BASIC) max_range = 4;
	else if (P_SKILL(typ) == P_SKILLED) max_range = 5;
	else max_range = 8;
	if (distu(cc.x, cc.y) > max_range) {
		pline(FunnyHallu ? "Seems like you can't reach it!" : "Too far!");
		return (res);
	} else if (!cansee(cc.x, cc.y)) {
		You(cant_see_spot);
		return (res);
	}

	/* What do you want to hit? */
	tohit = rn2(5);
	if (typ != P_NONE && !(PlayerCannotUseSkills) && P_SKILL(typ) >= P_SKILLED) {
	    winid tmpwin = create_nhwindow(NHW_MENU);
	    anything any;
	    char buf[BUFSZ];
	    menu_item *selected;

	    any.a_void = 0;	/* set all bits to zero */
	    any.a_int = 1;	/* use index+1 (cant use 0) as identifier */
	    start_menu(tmpwin);
	    any.a_int++;
	    sprintf(buf, "an object on the %s", surface(cc.x, cc.y));
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 buf, MENU_UNSELECTED);
	    any.a_int++;
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			"a monster", MENU_UNSELECTED);
	    any.a_int++;
	    sprintf(buf, "the %s", surface(cc.x, cc.y));
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 buf, MENU_UNSELECTED);
	    end_menu(tmpwin, "Aim for what?");
	    tohit = rn2(4);
	    if (select_menu(tmpwin, PICK_ONE, &selected) > 0 &&
			rn2(P_SKILL(typ) > P_SKILLED ? 20 : 2))
		tohit = selected[0].item.a_int - 1;
	    free((void *)selected);
	    destroy_nhwindow(tmpwin);
	}

	if (Race_if(PM_LEVITATOR) && (tohit < 0) ) tohit = rn2(4);
	if (Race_if(PM_LEVITATOR) && (tohit > 3) ) tohit = rn2(4);

	/* What did you hit? */
	switch (tohit) {
	case 0:	/* Trap */
	    /* FIXME -- untrap needs to deal with non-adjacent traps */
	    break;
	case 1:	/* Object */
	    if ((otmp = level.objects[cc.x][cc.y]) != 0) {

		if (costly_spot(cc.x, cc.y)) {
			pline("This item is magically protected from such theft attempts.");
			return(1);
		}

		You("snag an object from the %s!", surface(cc.x, cc.y));
		(void) pickup_object(otmp, Race_if(PM_LEVITATOR) ? otmp->quan : 1L, FALSE, FALSE);
		/* If pickup fails, leave it alone */
		newsym(cc.x, cc.y);
		return (1);
	    }
	    break;
	case 2:	/* Monster */
	    if ((mtmp = m_at(cc.x, cc.y)) == (struct monst *)0) break;
	    if (verysmall(mtmp->data) && !rn2(4) &&
			enexto(&cc, u.ux, u.uy, (struct permonst *)0)) {
		You("pull in %s!", mon_nam(mtmp));
		mtmp->mundetected = 0;
		rloc_to(mtmp, cc.x, cc.y);
		return (1);
	    } else if ((!bigmonst(mtmp->data) && !strongmonst(mtmp->data)) ||
		       rn2(4)) {
		(void) thitmonst(mtmp, uwep, 1, TRUE);
		return (1);
	    }
	    /* FALL THROUGH */
	case 3:	/* Surface */
	    if (IS_AIR(levl[cc.x][cc.y].typ) || is_pool(cc.x, cc.y))
		pline_The("hook slices through the %s.", surface(cc.x, cc.y));
	    else {
		You("are yanked toward the %s!", surface(cc.x, cc.y));
		hurtle(sgn(cc.x-u.ux), sgn(cc.y-u.uy), 1, FALSE);
		spoteffects(TRUE);
	    }
	    return (1);
	default:	/* Yourself (oops!) */
	    if (PlayerCannotUseSkills || P_SKILL(typ) <= P_BASIC) {
		You("hook yourself!");
		losehp(rn1(10,10), "a grappling hook", KILLED_BY);
		return (1);
	    }
	    break;
	}
	pline("%s", nothing_happens);
	if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
		pline("Oh wait, actually something bad happens...");
		badeffect();
	}
	return (1);
}


#define BY_OBJECT	((struct monst *)0)

/* return 1 if the wand is broken, hence some time elapsed */
STATIC_OVL int
do_break_wand(obj)
    struct obj *obj;
{
    char confirm[QBUFSZ], the_wand[BUFSZ];

    pline("Attention! You are about to break %s!", yname(obj));
    getlin ("Do you really want to break it? [yes/no]", confirm);

    if (strcmp (confirm, "yes")) {
	return 0;
    }

    strcpy(the_wand, yname(obj));

    if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) {
	You_cant("break %s without hands!", the_wand);
	if (yn("Attempt it anyway? Warning: this can fail and cause bad effects.") == 'y') {
		if (rn2(3) && !polyskillchance()) {
			badeffect();
			pline("Hmm, that seems to have been a magical discharge!");
			if (FunnyHallu) pline("Maybe the weather forecast would have warned you of that if you had actually bothered to read it today?");
			if (!rn2(20)) badeffect();
			return 1;
		}
	}
	else return 0;
    }
    if (ACURR(A_STR) < 10) {
	You("don't have the strength to break %s!", the_wand);
	return 0;
    }
    pline("Raising %s high above your %s, you break it in two!",
	  the_wand, body_part(HEAD));
    u.cnd_wandbreakcount++;
    return wand_explode(obj, TRUE);
}

/* This function takes care of the effects wands exploding, via
 * user-specified 'applying' as well as wands exploding by accident
 * during use (called by backfire() in zap.c)
 *
 * If the effect is directly recognisable as pertaining to a 
 * specific wand, the wand should be makeknown()
 * Otherwise, if there is an ambiguous or indirect but visible effect
 * the wand should be allowed to be named by the user.
 *
 * If there is no obvious effect,  do nothing. (Should this be changed
 * to letting the user call that type of wand?)
 *
 * hero_broke is nonzero if the user initiated the action that caused
 * the wand to explode (zapping or applying).
 */
int
wand_explode(obj, hero_broke)
    struct obj *obj;
    boolean hero_broke;
{
    static const char nothing_else_happens[] = "But nothing else happens...";
    register int i, x, y;
    register struct monst *mon;
    int dmg, damage;
    boolean affects_objects;
    boolean shop_damage = FALSE;
    int expltype = EXPL_MAGICAL;
    char buf[BUFSZ];

    int devicetrain = 0;
    if (hero_broke && obj && obj->spe > 0) devicetrain = obj->spe;

    if (!hero_broke && obj && (obj->oartifact == ART_DONTPLODE || obj->oartifact == ART_CAST_AK) ) {
	pline_The("explosion didn't harm the wand!");
	return 0;
    }

    /* [ALI] Do this first so that wand is removed from bill. Otherwise,
     * the freeinv() below also hides it from setpaid() which causes problems.
     */
    if (carried(obj) ? obj->unpaid :
	    !obj->no_charge && costly_spot(obj->ox, obj->oy)) {
	if (hero_broke)
	check_unpaid(obj);		/* Extra charge for use */
	bill_dummy_object(obj);
    }

    current_wand = obj;		/* destroy_item might reset this */
    freeinv(obj);		/* hide it from destroy_item instead... */
    setnotworn(obj);		/* so we need to do this ourselves */

    if (obj->spe <= 0) {
	pline(nothing_else_happens);
	goto discard_broken_wand;
    }
    obj->ox = u.ux;
    obj->oy = u.uy;
    dmg = obj->spe * 4;
    affects_objects = FALSE;

	if (issoviet) pline("Kabum, ty byl glup, i teper' tvoya palochka vzorvalas', i ty vzyal na sebya der'mo. Obsluzhivayet vas pravil'no, chtoby byt' takoy zatormozhennost'yu.");

    switch (obj->otyp) {
    case WAN_WISHING:
    case WAN_ACQUIREMENT:
    case WAN_NOTHING:
    case WAN_SHARE_PAIN:
    case WAN_LOCKING:
    case WAN_PROBING:
    case WAN_ENLIGHTENMENT:
    case WAN_ENTRAPPING:
    case WAN_TELE_LEVEL:
    case WAN_GENOCIDE:
    case WAN_STINKING_CLOUD:
    case WAN_TIME_STOP:
    case WAN_MAGIC_MAPPING:
    case WAN_DARKNESS:
    case WAN_TRAP_CREATION:
    case WAN_SUMMON_SEXY_GIRL:
    case WAN_GAIN_LEVEL:
    case WAN_MANA:
    case WAN_BAD_EFFECT:
    case WAN_OBJECTION:
    case WAN_DETECT_MONSTERS:
    case WAN_IDENTIFY:
    case WAN_LEVITATION:
    case WAN_DEBUGGING:
    case WAN_SPELLBINDER:
    case WAN_INERTIA_CONTROL:
    case WAN_STERILIZE:
    case WAN_RESTORATION:
    case WAN_REMOVE_CURSE:
    case WAN_PUNISHMENT:
    case WAN_OPENING:
    case WAN_WONDER:
    case WAN_BUGGING:
    case WAN_CLONE_MONSTER:
    case WAN_SUMMON_UNDEAD:
    case WAN_SECRET_DOOR_DETECTION:
    case WAN_TRAP_DISARMING:
    case WAN_CREATE_FAMILIAR:
	pline(nothing_else_happens);
	goto discard_broken_wand;
    case WAN_DEATH:
    case WAN_MISFIRE:
    case WAN_LIGHTNING:
    case WAN_THUNDER:
    case WAN_CHARGING:
    case WAN_PSYBEAM:
    case WAN_NETHER_BEAM:
    case WAN_SOLAR_BEAM:
    case WAN_AURORA_BEAM:
	dmg *= 4;
	goto wanexpl;
    case WAN_COLD:
    case WAN_ICE_BEAM:
	expltype = EXPL_FROSTY;
    case WAN_HYPER_BEAM:
	dmg *= 2;
    case WAN_MAGIC_MISSILE:
    wanexpl:
	u.explodewandhack = 1;
	explode(u.ux, u.uy, ZT_MAGIC_MISSILE, dmg, WAND_CLASS, expltype);
	u.explodewandhack = 0;
	makeknown(obj->otyp);	/* explode described the effect */
	goto discard_broken_wand;
    case WAN_ACID:
    case WAN_SLUDGE:
	expltype = EXPL_NOXIOUS;
	dmg *= 2;
	u.explodewandhack = 1;
	explode(u.ux, u.uy, ZT_MAGIC_MISSILE, dmg, WAND_CLASS, expltype);
	u.explodewandhack = 0;
	makeknown(obj->otyp);	/* explode described the effect */
	goto discard_broken_wand;
/*WAC for wands of fireball- no double damage
 * As well, effect is the same as fire, so no makeknown
 */

    case WAN_FIRE:
    case WAN_INFERNO:
	dmg *= 2;
    case WAN_FIREBALL:
	expltype = EXPL_FIERY;
	  u.explodewandhack = 1;
        explode(u.ux, u.uy, ZT_FIRE, dmg, WAND_CLASS, expltype);
	  u.explodewandhack = 0;
	if (obj->dknown && !objects[obj->otyp].oc_name_known &&
		!objects[obj->otyp].oc_uname)
        docall(obj);
	goto discard_broken_wand;
    case WAN_STRIKING:
    case WAN_GRAVITY_BEAM:
	/* we want this before the explosion instead of at the very end */
	pline("A wall of force smashes down around you!");
	if (PlayerHearsSoundEffects) pline(issoviet ? "Sovetskaya khochet zastavit' vas vyyti iz igry!" : "Schrang!");
	dmg = d(1 + obj->spe,6);	/* normally 2d12 */
	affects_objects = TRUE;
	break;
    case WAN_DISINTEGRATION:
	pline("A wall of force smashes down around you!");
	if (PlayerHearsSoundEffects) pline(issoviet ? "Sovetskaya khochet zastavit' vas vyyti iz igry!" : "Schrang!");
	dmg = d(1 + obj->spe,30);
	affects_objects = TRUE;
	break;
    case WAN_CANCELLATION:
    case WAN_POLYMORPH:
    case WAN_MUTATION:
    case WAN_UNDEAD_TURNING:
    case WAN_WIND:
    case WAN_DRAINING:	/* KMH */
    case WAN_REDUCE_MAX_HITPOINTS:
    case WAN_INCREASE_MAX_HITPOINTS:
	affects_objects = TRUE;
	break;
    case WAN_TELEPORTATION:
		/* WAC make tele trap if you broke a wand of teleport */
		/* But make sure the spot is valid! */
	    if ((obj->spe > 2) && rn2(obj->spe - 2) && !level.flags.noteleport && !Race_if(PM_STABILISATOR) && !u.antitelespelltimeout &&
		    !u.uswallow && !On_stairs(u.ux, u.uy) && (!IS_FURNITURE(levl[u.ux][u.uy].typ) &&
		    !IS_ROCK(levl[u.ux][u.uy].typ) &&
		    !closed_door(u.ux, u.uy) && !t_at(u.ux, u.uy))) {

			struct trap *ttmp;

			ttmp = maketrap(u.ux, u.uy, TELEP_TRAP, 0, FALSE);
			if (ttmp) {
				ttmp->madeby_u = 1;
				newsym(u.ux, u.uy); /* if our hero happens to be invisible */
				if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
					/* shopkeeper will remove it */
					add_damage(u.ux, u.uy, 0L);             
				}
			}
		}
	affects_objects = TRUE;
	break;
    case WAN_BANISHMENT:

	    if ((obj->spe > 2) && rn2(obj->spe - 2) && !level.flags.noteleport && !Race_if(PM_STABILISATOR) && !u.antitelespelltimeout &&
		    !u.uswallow && !On_stairs(u.ux, u.uy) && (!IS_FURNITURE(levl[u.ux][u.uy].typ) &&
		    !IS_ROCK(levl[u.ux][u.uy].typ) &&
		    !Is_knox(&u.uz) && !Is_blackmarket(&u.uz) && !Is_aligned_quest(&u.uz) &&
		    !In_endgame(&u.uz) && !In_sokoban_real(&u.uz) &&
		    !closed_door(u.ux, u.uy) && !t_at(u.ux, u.uy))) {

			struct trap *ttmp;

			ttmp = maketrap(u.ux, u.uy, LEVEL_TELEP, 0, FALSE);
			if (ttmp) {
				ttmp->madeby_u = 1;
				newsym(u.ux, u.uy); /* if our hero happens to be invisible */
				if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
					/* shopkeeper will remove it */
					add_damage(u.ux, u.uy, 0L);             
				}
			}
		}
	affects_objects = TRUE;
	break;

    case WAN_CREATE_HORDE: /* More damage than Create monster */
	        dmg *= 2;
	        break;
    case WAN_HEALING:
    case WAN_EXTRA_HEALING:
    case WAN_FULL_HEALING:
    case WAN_MEDIUM_HEALING:
    case WAN_STRONG_HEALING:
    case WAN_SUPER_HEALING:
		dmg = 0;
		break;
    default:
	break;
    }

    /* strangely enough, all wands that use the "goto discard_broken_wand" code above WILL NOT RUN THE CODE BELOW --Amy */

    u.explodewandhack = 1;

    /* magical explosion and its visual effect occur before specific effects */
    explode(obj->ox, obj->oy, ZT_MAGIC_MISSILE, dmg ? rnd(dmg) : 0, WAND_CLASS,
	    EXPL_MAGICAL);

    u.explodewandhack = 0;

    /* this makes it hit us last, so that we can see the action first */
    for (i = 0; i <= 8; i++) {
	bhitpos.x = x = obj->ox + xdir[i];
	bhitpos.y = y = obj->oy + ydir[i];
	if (!isok(x,y)) continue;

	if (obj->otyp == WAN_DIGGING) {
	    if(dig_check(BY_OBJECT, FALSE, x, y)) {
		if (IS_WALL(levl[x][y].typ) || IS_DOOR(levl[x][y].typ)) {
		    /* normally, pits and holes don't anger guards, but they
		     * do if it's a wall or door that's being dug */
		    watch_dig((struct monst *)0, x, y, TRUE);
		    if (*in_rooms(x,y,SHOPBASE)) shop_damage = TRUE;
		}		    
		digactualhole(x, y, BY_OBJECT,
			      (rn2(obj->spe) < 3 || !Can_dig_down(&u.uz)) ?
			       PIT : HOLE);
	    }
	    continue;
/* WAC catch Create Horde wands too */
/* MAR make the monsters around you */
	} else if(obj->otyp == WAN_CREATE_MONSTER
                || obj->otyp == WAN_CREATE_HORDE) {
	    /* u.ux,u.uy creates it near you--x,y might create it in rock */

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	    (void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);

		u.aggravation = 0;

	    continue;
	} else {
	    if (x == u.ux && y == u.uy) {
		/* teleport objects first to avoid race with tele control and
		   autopickup.  Other wand/object effects handled after
		   possible wand damage is assessed */
		if (obj->otyp == WAN_TELEPORTATION &&
		    affects_objects && level.objects[x][y]) {
		    (void) bhitpile(obj, bhito, x, y);
		    if (flags.botl) bot();		/* potion effects */
			/* makeknown is handled in zapyourself */
		}
		/* sorry SLASH'EM dev team, but getting your whole inventory cancelled when you get hit by lightning is WAAAAAAAAAAAAAY too harsh. --Amy
		 * In Soviet Russia, the Kreml dictates how evil the laws of the country are supposed to be, and WOE to ANYONE who DARES question them. So,
		 * they will fully support wand explosions that utterly ruin your character, and nothing can protect you other than not having a wand of
		 * cancellation or polymorph in your open inventory at any given time unless you're absolutely 200% sure that you won't get hit by lightning.
		 * also evil variant mode is evil too, so you get super dangerous wand explosions in that mode, too */
		damage = zapyourself(obj, FALSE, issoviet ? FALSE : isevilvariant ? FALSE : TRUE);
		if (issoviet) pline("KHOOOO KHE KHE KHE KHE KHE, vot kakoy ledyanoy blok tebya podlovil, potomu chto on vzorval tvoyu duratskuyu palochku i podverg tebya uzhasnym posledstviyam! MUA KHA KHA, sovetskaya pyaterka takaya klassnaya, soglasny?");
		if (damage) {
		    if (hero_broke) {
		    sprintf(buf, "killed %sself by breaking a wand", uhim());
		    losehp(damage, buf, NO_KILLER_PREFIX);
		    } else
			losehp(damage, "exploding wand", KILLED_BY_AN);
		}
		if (flags.botl) bot();		/* blindness */
	    } else if ((mon = m_at(x, y)) != 0 && !DEADMONSTER(mon)) {
		(void) bhitm(mon, obj);
	     /* if (flags.botl) bot(); */
	    }
	    if (affects_objects && level.objects[x][y]) {
		(void) bhitpile(obj, bhito, x, y);
		if (flags.botl) bot();		/* potion effects */
	    }
	}
    }

    /* Note: if player fell thru, this call is a no-op.
       Damage is handled in digactualhole in that case */
    if (shop_damage) pay_for_damage("dig into", FALSE);

    if (obj->otyp == WAN_LIGHT)
	litroom(TRUE, obj);	/* only needs to be done once */

 discard_broken_wand:
    obj = current_wand;		/* [see dozap() and destroy_item()] */
    current_wand = 0;
    if (obj)
	delobj(obj);
    nomul(0, 0, FALSE);

    /* did the player break it? if so, train devices skill depending on the # of charges */
    if (devicetrain) use_skill(P_DEVICES, devicetrain);
    return 1;
}

STATIC_OVL boolean
uhave_graystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(is_graystone(otmp))
			return TRUE;
	return FALSE;
}

STATIC_OVL void
add_class(cl, class)
char *cl;
char class;
{
	char tmp[2];
	tmp[0] = class;
	tmp[1] = '\0';
	strcat(cl, tmp);
}

void use_floppies(struct obj *obj)
{
	char *softwares[] = {
		"Microsoft Windows 3.1",
		"Bill Gates",
		"Leisure Suit Larry V",
		"LINUX 1.00",
		"your NetHack patches",
		"nothing of importance"
	};
	int x;

	if (!Role_if(PM_GEEK) && !Role_if(PM_GRADUATE) && !Role_if(PM_CRACKER) && !Role_if(PM_SOFTWARE_ENGINEER) && !(obj->oartifact == ART_EASYREAD_FILE) ) {
		pline(FunnyHallu ? "Is this some old Atari or Commodore machine? It's not starting, it seems..." : "If only you knew what the heck this is ... ");
		return;
	}

	pline("Due to years of experience with computers, you can read the");
	pline("disks' contents by merely looking at the magnetic surface ...");

	if (Blind) {
		pline("Yet, without seeing, not even you can read anyting.");
		return;
	}

	if (obj->oartifact == ART_F_PROT && !rn2(7) ) {
		pline("You see Bandarchor on the disk");
		pline("All your personal data, documents, photos etc. are encrypted. Please pay 600 euros (approximately 1 bitcoin) to receive the decryption key.");
		badeffect();
		return;
	}

	if (obj->oartifact == ART_NETHACK_SOURCES) {
		com_pager(9999);
	} else {
		x = rn2(sizeof(softwares) / sizeof(softwares[0]));
		pline("You see %s on the disk",softwares[x]);
		if (x == 0) {		/* Windows */
			pline("You shriek in pain!");
			make_confused(HConfusion+rn2(50)+50,TRUE);
		} else if (x == 1) {	/* Bill Gates */
			You_feel("horrible!");
			/* nothing happens (yet ...) */
		}
	}
}

static int
potion_charge_cost(struct obj *pot)
{
	int cost;

	/*cost = objects[pot->otyp].oc_cost / 150;*/ cost = 2;
	switch (pot->otyp) {
	case POT_EXTRA_HEALING: cost += 1; break;
	case POT_ESP: cost += 1; break;
	case POT_GAIN_ENERGY: cost += 1; break;
	case POT_GAIN_HEALTH: cost += 3; break;
	case POT_BOOST: cost += 3; break;
	case POT_INVISIBILITY: cost += 1; break;
	case POT_SEE_INVISIBLE: cost += 1; break;
	case POT_CURE_CRITICAL_WOUNDS: cost += 2; break;
	case POT_FULL_HEALING: cost += 3; break;
	case POT_SPEED: cost += 1; break;
	case POT_RECOVERY: cost += 1; break;
	case POT_GAIN_ABILITY: cost += 1; break;
	case POT_GAIN_LEVEL: cost += 2; break;
	case POT_CYANIDE: cost += 2; break;
	case POT_RANDOM_INTRINSIC: cost += 2; break;
	case POT_BENEFICIAL_EFFECT: cost += 2; break;
	case POT_TECH_LEVEL_UP: cost += 5; break;
	case POT_INVULNERABILITY: cost += 8; break;
	case POT_TRAINING: cost += 8; break;
	case POT_EXTREME_POWER: cost += 2; break;
	case POT_WONDER: cost += 10; break;
	case POT_CURE_INSANITY: cost += 10; break;
	case POT_TERCES_DLU: cost += 10; break;
	case POT_HEROISM: cost += 18; break;
	case POT_ULTIMATE_TSUYOSHI_SPECIAL: cost += 18; break;
	default: break;
	}
	if (cost < 1) cost = 1;
	return(cost);
}

static void
use_chemistry_set(struct obj *chemset)
{
	struct obj *bottle;
	static char bottles[] = { TOOL_CLASS, 0 };
	char namebuf[BUFSZ],potbuf[BUFSZ];
	struct obj *new_obj;
	int cost;
	char c;
	boolean explosionpossible = TRUE;
	int phialtype = 0;

	/* We will allow the player to make a potion occasionally, even if they don't know the spell. --Amy */

	if (!spell_known(SPE_CHEMISTRY) && (issoviet || rn2(3)) ) {
		if (!issoviet) pline("Huh? You don't understand anything about such stuff!");
		else pline("Sovetskiy upal iz sredney shkoly iz-za yego nesposobnosti delat' khimiyu. On ne khochet, chtoby vy byli sposobny delat' chto-to on ne mog.");
		if (chemset->spe > 0) chemset->spe -= 1;
		return;

	/* if spe == 0, the potion will always blast the player anyway. */

	}
	makeknown(CHEMISTRY_SET);

	bottle = getobj(bottles,"hold the resulting potion in");
	if (!bottle) return;
	if (bottle->otyp != BOTTLE && bottle->otyp != PHIAL) {
		pline("Exactly how are you going to do this?");
		return;
	}

	if (bottle && bottle->oartifact == ART_SUPER_VONK) explosionpossible = FALSE;
	if (bottle && bottle->oartifact == ART_MULTIAL) phialtype = 1; /* because it's used up below */
	if (bottle && bottle->oartifact == ART_SKILLOGUP) phialtype = 2;

	getlin("What potion do you want to make?",namebuf);
	if (!namebuf[0] || namebuf[0] == '\033') return;

	potbuf[0] = 0;
	if (strncmp(namebuf,"potion of",9) != 0) {
		strcpy(potbuf,"potion of ");
	}
	strcat(potbuf,namebuf);
	new_obj = readobjnam(potbuf, (struct obj *)0, TRUE, FALSE);

	if (!new_obj || new_obj->oclass != POTION_CLASS) {
		goto blast_him;
	}
	if (!(objects[new_obj->otyp].oc_name_known) && 
	    !(objects[new_obj->otyp].oc_uname)) {
		You("don't know how to make such a potion!");
		c =yn_function("Do you want to give it a try anyway?","yn",'n');
		if (c != 'y') {	
			obfree(new_obj,(struct obj *) 0);
			return;
		}
		if (rnl(5)) {
			useup(bottle);

			if (phialtype == 1) {
				struct obj *ufials;
				ufials = mksobj(PHIAL, TRUE, FALSE, FALSE);
				if (ufials) {
					ufials->quan = 4;
					ufials->owt = weight(ufials);
					dropy(ufials);
					stackobj(ufials);
					pline("A bunch of phials appeared on the ground!");
				}

			}

			goto blast_him;
		}
	}
	new_obj->selfmade = TRUE;
	new_obj->cursed = bottle->cursed || chemset->cursed;
	new_obj->blessed = bottle->blessed || chemset->blessed;
	if (new_obj->blessed && new_obj->cursed) {
		new_obj->blessed = new_obj->cursed = FALSE;
	}
	new_obj->hvycurse = new_obj->prmcurse = new_obj->morgcurse = new_obj->evilcurse = new_obj->bbrcurse = new_obj->stckcurse = FALSE;
	cost = potion_charge_cost(new_obj);

	if (objects[new_obj->otyp].oc_name_known) {
		pline("Making this potion will use %d charges.", cost);
		if (yn("Do you want to give it a try?") != 'y') {
			obfree(new_obj, (struct obj *) 0);
			return;
		}
	}

	int nochargechange = 10;
	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_DEVICES)) {
			default: break;
			case P_BASIC: nochargechange = 9; break;
			case P_SKILLED: nochargechange = 8; break;
			case P_EXPERT: nochargechange = 7; break;
			case P_MASTER: nochargechange = 6; break;
			case P_GRAND_MASTER: nochargechange = 5; break;
			case P_SUPREME_MASTER: nochargechange = 4; break;
		}
	}
	if (nochargechange < rnd(10)) cost -= 1;
	if (cost < 0) cost = 0; /* fail safe */

	if (cost > chemset->spe) {
		pline("There is too little material left in your chemistry set!");
		goto blast_him;
	}
	chemset->spe -= cost;
	useup(bottle);

	if (phialtype == 1) {
		struct obj *ufials;
		ufials = mksobj(PHIAL, TRUE, FALSE, FALSE);
		if (ufials) {
			ufials->quan = 4;
			ufials->owt = weight(ufials);
			dropy(ufials);
			stackobj(ufials);
			pline("A bunch of phials appeared on the ground!");
		}
	}

	if (!chemset->blessed && explosionpossible && !(uarmc && uarmc->oartifact == ART_NO_MORE_EXPLOSIONS) && !rn2(chemset->cursed ? 2 : 10)) {
blast_him:
		pline("You seem to have made a mistake!");
		pline("The bottle explodes!");
		losehp(rnd(chemset->cursed ? 25 : 10),"alchemic blast",KILLED_BY_AN);
		if (Has_contents(new_obj))
			delete_contents(new_obj);
		obfree(new_obj,(struct obj *) 0);
		if (practicantterror) {
			pline("%s booms: 'Quit trying to create bombs, maggot. 200 zorkmids.'", noroelaname());
			fineforpracticant(200, 0, 0);
		}
		return;
	}

	if (!(objects[new_obj->otyp].oc_name_known)) makeknown(new_obj->otyp);
	hold_another_object(new_obj,"Oops! Where did you put that potion?",(const char *) 0,(const char *) 0);
	You("have done it!");
	if (practicantterror && new_obj->otyp == POT_CYANIDE) {
		pline("%s thunders: 'You dare manufacturing such highly toxic substances??? 5000 zorkmids. Don't even think about actually using that type of poison!'", noroelaname());
		fineforpracticant(5000, 0, 0);
	}
	use_skill(P_DEVICES,2);
	if (Race_if(PM_FAWN)) {
		use_skill(P_DEVICES,2);
	}
	if (Race_if(PM_SATRE)) {
		use_skill(P_DEVICES,2);
		use_skill(P_DEVICES,2);
	}

	if (phialtype == 2) use_skill(P_DEVICES,10);

	u.cnd_chemistrycount++;
}

/* player tries to apply an item: return 1 if it used a turn */
int
doapply()
{
	struct obj *obj;
	register int res = 1;
	register boolean can_use = FALSE;
	char class_list[MAXOCLASSES+2];
	boolean noartispeak = FALSE; /* if item was vaporized, don't call arti_speaks because segfaults despite fail safe */

	if (u.powerfailure || (uarmf && uarmf->oartifact == ART_BRITTA_S_MURDER_STORY) || (isselfhybrid && (moves % 3 == 0 && moves % 11 != 0) ) ) {
		pline("Your power's down, and therefore you cannot apply anything.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (rn2(5) && check_capacity((char *)0)) return (1); /* overtaxed, tried (20% chance of success) and failed, so you lose a turn --Amy */

	if (carrying(POT_OIL) || carryingappearance(APP_POTION_VACCINE) || carryingappearance(APP_POTION_DYE) || uhave_graystone())
		strcpy(class_list, tools_too);
	else
		strcpy(class_list, tools);
	if (carrying(CREAM_PIE) || carrying(EUCALYPTUS_LEAF))
		add_class(class_list, FOOD_CLASS);
	if (carrying(HEAVY_LASER_BALL))
		add_class(class_list, BALL_CLASS);
	if (carryingarti(ART_VORGATH_S_GRAVITON))
		add_class(class_list, BALL_CLASS);
	if (carrying(LASER_CHAIN))
		add_class(class_list, CHAIN_CLASS);
	if (carrying(STATUE))
		add_class(class_list, ROCK_CLASS);

	obj = getobj(class_list, "use or apply");
	if(!obj) return 0;

	if (obj->mstartinventX) {
		pline("That item is aetheric and can't be applied!");
		return 0;
	}

	/* un-knowledge nastytrap: can't apply any tool, but invocation tools have to be exempt --Amy
	 * candles must also be exempt because those can be put on the candelabrum */
	if (UnKnowledgeEffect || u.uprops[UN_KNOWLEDGE_EFFECT].extrinsic || have_unknowledgestone() || (uarmu && uarmu->oartifact == ART_STOCKTON_BRAND)) {
		if (obj->oclass == TOOL_CLASS && obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && !Is_candle(obj) && !(objects[obj->otyp].oc_name_known) ) {
			pline("Unfortunately you don't know how to use that item.");
			return(0);
		}
		if (obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && UnKnowledgeXtra &&  (!obj->known || !obj->rknown || !obj->dknown || !obj->bknown) )	{
			pline("Unfortunately, you don't know how to use that tool.");
			return(0);
		}
	}

	if (EpviProblemActive && obj && (objects[obj->otyp].oc_minlvl > (u.ulevel + u.xtralevelmult - 1) ) ) {
		verbalize("I cannot use that yet.");
		return(0);
	}

	if (CurseAsYouUse && obj && obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != SPE_BOOK_OF_THE_DEAD && obj->otyp != BELL_OF_OPENING) curse(obj);

	if (obj && obj->oartifact == ART_NO_STOPPING_THE_COURIER) obj->oerodeproof = TRUE;

	if (InterruptEffect || u.uprops[INTERRUPT_EFFECT].extrinsic || have_interruptionstone()) {
		nomul(-(rnd(5)), "applying a tool", TRUE);
	}

	if (obj->oartifact && !touch_artifact(obj, &youmonst))
	    return 1;	/* evading your grasp costs a turn; just be
			   grateful that you don't drop it as well */

	if (FreeHandLoss || u.uprops[FREE_HAND_LOST].extrinsic || (uarmc && uarmc->oartifact == ART_ARABELLA_S_SEXY_GIRL_BUTT) || have_freehandbugstone() ) {
		if (!(uwep && uwep == obj)) {
			pline("You must wield this item first if you want to apply it!"); 
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			wield_tool(obj, "swing");
			return 1;
		}
	}

	if (obj->oclass == WAND_CLASS)
	    return do_break_wand(obj);

	/* Mushroom can use every weapon as a polearm, including bows, arrows etc., can be turned on via #monster --Amy */
	if ( (Race_if(PM_PLAYER_MUSHROOM) || (uchain && uchain->oartifact == ART_ERO_ERO_ERO_ERO_MUSHROOM_M)) && obj->oclass == WEAPON_CLASS && u.mushroompoles) {

		if (uwep && uwep == obj) res = use_pole(obj);
		else {
			pline("You must wield this item first if you want to apply it!"); 
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			wield_tool(obj, "swing");
		}

		goto mushroompolecheck;
	}

	if (itemhasappearance(obj, APP_POTION_VACCINE)) {
		if (yn("Inject the vaccine?") == 'y') {
			noartispeak = TRUE; /* item is now gone */
			useup(obj);
			You("inject the covid-19 vaccine...");
			if (FunnyHallu) pline("Now you should no longer be obligated to wear a butt-ugly face mask and keep 2 meters of distance to other people. After all, you're immune to the plague now.");
			upnivel(TRUE);
			return 1;
		}
	}

	if (itemhasappearance(obj, APP_POTION_DYE)) {
		if (yn("Dye an item?") == 'y') {
			noartispeak = TRUE; /* item is now gone */
			useup(obj);

			register struct obj *steeling;
			if (CannotSelectItemsInPrompts) return 1;
			pline("You may dye an object.");
dyechoice:
			steeling = getobj(allowxall, "dye");
			if (!steeling) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to dye an item.");
				else goto dyechoice;
				pline("Oh well, if you don't wanna...");
			} else {
				int dyepotion = find_appearance_potion(APP_POTION_DYE);
				objects[steeling->otyp].oc_color = objects[dyepotion].oc_color;
				pline_The("item was dyed successfully!");
			}

			return 1;
		}
	}

	switch(obj->otyp){
	case BLINDFOLD:
	case EYECLOSER:
	case DRAGON_EYEPATCH:
	case LENSES:
	case RADIOGLASSES:
	case SHIELD_PATE_GLASSES:
	case BOSS_VISOR:
	case NIGHT_VISION_GOGGLES:
	case CONDOME:
	case CLIMBING_SET:
	case DEFUSING_BOX:
	case SOFT_CHASTITY_BELT:
		if (obj == ublindf) {
		    if (!cursed(obj)) Blindf_off(obj, TRUE);
		} else if (!ublindf)
		    Blindf_on(obj);
		else You("are already %s.",
			ublindf->otyp == TOWEL ?     "covered by a towel" :
			ublindf->otyp == BLINDFOLD ? "wearing a blindfold" :
			ublindf->otyp == EYECLOSER ? "wearing a blindfold" :
			ublindf->otyp == DRAGON_EYEPATCH ? "wearing a blindfold" :
			ublindf->otyp == CONDOME ? "wearing a condome" :
			ublindf->otyp == SOFT_CHASTITY_BELT ? "wearing a condome" :
			ublindf->otyp == CLIMBING_SET ? "using a climbing set" :
			ublindf->otyp == DEFUSING_BOX ? "using a defusing box" :
						     "wearing lenses");
		break;
	case CREAM_PIE:
		res = use_cream_pie(obj);
		break;
	case BULLWHIP:
	case INKA_SHACKLE:
	case BULLETPROOF_CHAINWHIP:
	case STEEL_WHIP:
	case ETHER_WHIP:
	case VIPERWHIP:
	case LONG_WHIP:
	case GARBOWHIP:
	case HEAVENLY_WHIP:
	case SEXPLAY_WHIP:
	case FORCE_WHIP:
	case CHITIN_WHIP:
	case FORMICAN_WHIP:
	case CHAINWHIP:
	case MITHRIL_WHIP:
	case FLAME_WHIP:
	case ROSE_WHIP:
	case RAZOR_WHIP:
	case SECRET_WHIP:
	case SECRET_SOUND_WHIP:

		if (obj->oartifact == ART_WOETSCHERSMAG || obj->oartifact == ART_SIMON_S_ULTRALONGNESS) {
			if (uwep && uwep == obj) res = use_pole(obj);
			else {pline("You must wield this item first if you want to apply it!"); 
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				wield_tool(obj, "swing"); }
			break;
		}

		if (uwep && uwep == obj) res = use_whip(obj);
		else {pline("You must wield this item first if you want to apply it!"); 
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			wield_tool(obj, "lash"); }
		break;
	case GRAPPLING_HOOK:
	case JACK_KNIFE:
		if (uwep && uwep == obj) res = use_grapple(obj);
		else {pline("You must wield this item first if you want to apply it!"); 
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			wield_tool(obj, "cast"); }
		break;
	case LARGE_BOX:
	case LEAD_BOX:
	case TOP_BOX:
	case TREASURE_CHEST:
	case LOOT_CHEST:
	case CHEST:
	case NANO_CHEST:
	case CHEST_OF_HOLDING:
	case ICE_BOX:
	case ICE_BOX_OF_HOLDING:
	case ICE_BOX_OF_WATERPROOFING:
	case DISPERSION_BOX:
	case SACK:
	case BAG_OF_HOLDING:
	case OILSKIN_SACK:
	case TITAN_SACK:
	case POTATO_BAG:
	case RUGGED_SACK:
	case HANDYBAG:
		if ((obj->otyp == BAG_OF_HOLDING || obj->otyp == CHEST_OF_HOLDING || obj->otyp == ICE_BOX_OF_HOLDING) && obj->cursed) {
			use_container(&obj, 1);
			res = 1;
		} else {
			res = use_container(&obj, 1);
		}
		if (obj && obj->oartifact == ART_KNOW_OF_THE_CURSE) {
			res = 1; /* always uses a turn */
			if (!u.knowofthecursetimer) {
				u.knowofthecursetimer = 50;
				makerandomtrap(TRUE);
			}
		}
		noartispeak = TRUE; /* because it could explode! */
		break;
	case BAG_OF_DIGESTION:
	case LARGE_BOX_OF_DIGESTION:
	case ICE_BOX_OF_DIGESTION:
		use_container(&obj, 1);
		res = 1;
		/* always uses up a turn now, which makes it trivial to ID I guess but that's okay, because
		 * at least it means you can no longer easily get rid of troll corpses and stuff :-P --Amy */
		noartispeak = TRUE; /* because it could explode! */
		break;
	case BAG_OF_TRICKS:
		bagotricks(obj);
		break;
	case CAN_OF_GREASE:
		use_grease(obj);
		break;
	case LUBRICANT_CAN:
		use_grease(obj);
		if (!rn2(4)) {
			pline("Klack! The lid slides over your %s, and %s is shooting out.", body_part(HAND), body_part(BLOOD));
			losehp(rnd(30), "mis-applying a lubricant can", KILLED_BY);
		}
		if (obj && (obj->spe > 0) && obj->oartifact == ART_GREASE_YOUR_BUTT) {
			int attempts = 0;
			register struct permonst *ptrZ;

			do {

				ptrZ = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !(ptrZ->msound == MS_FART_NORMAL || ptrZ->msound == MS_FART_QUIET || ptrZ->msound == MS_FART_LOUD))) && attempts < 50000);

			if (ptrZ->msound == MS_FART_NORMAL || ptrZ->msound == MS_FART_QUIET || ptrZ->msound == MS_FART_LOUD) {
				u.wormpolymorph = monsndx(ptrZ);
				You_feel("sexy!");
				polyself(FALSE);
			}
		}
		break;
	case CREDIT_CARD:
	case DATA_CHIP:
	case LOCK_PICK:
	case HAIRCLIP:
	case SKELETON_KEY:
	case CONTROVERSY_CODE:
	case SECRET_KEY:
		if (obj->oartifact == ART_STRANGE_LILCHEN) {
			if (FemaleTrapJil < 5000) FemaleTrapJil = 5000;
		}
		if (obj->oartifact == ART_VANULLA_SCORE) {
			if (NastinessProblem < 5000) NastinessProblem = 5000;
		}
		(void) pick_lock(&obj);
		break;
	case STATUE:
		if (obj && obj->oartifact == ART_SATAN_S_SECRET_STORAGE) {
			res = use_container(&obj, 1);
		} else pline("There's nothing special that you could do with that %s.", FunnyHallu ? "hunk of stone" : "statue");
		break;
	case TELEPHONE:
		if (obj && obj->spe > ((obj->oartifact == ART_BUNDLEABRUM_OF_INVOCATION) ? -10 : 0)) {
			obj->spe--;
			You("dial a number...");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy nabrali nepravil'nyy nomer, slabak!" : "BeepbeepBeepbeepbeepbeepBeepbeepbeep!");

			if (obj->oartifact == ART_MOLOCH_S_PERSONAL_PHONE) {
				int pm;
				pm = !rn2(5) ? dprince(rn2((int)A_LAWFUL+2) - 1) : dlord(rn2((int)A_LAWFUL+2) - 1);
				if (pm >= PM_ORCUS && pm <= PM_DEMOGORGON) u.conclusiocount++;
				if (pm && (pm != NON_PM)) {
					(void) makemon(&mons[pm], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
					pline("An angry demon was summoned!");
				}

			} else {

				int summondemon = ndemon(A_CHAOTIC);
				if (summondemon == NON_PM) {
					pline("But no one picks up your call.");
				} else {
					pline("%s", Blind ? "You hear an evil chuckle!" : "A miasma of stinking vapors coalesces around you!");
					make_pet_minion(summondemon, A_CHAOTIC);
					godvoice(A_CHAOTIC, "My minion shall serve thee!");
				}
			}

		} else pline("You try to call someone, but there's no prepaid balance on this phone and therefore it fails.");
		break;
	case PICK_AXE:
	case CONGLOMERATE_PICK:
	case UNWIELDY_PICK:
	case CONUNDRUM_PICK:
	case SHOVEL:
	case MYSTERY_PICK:
	case BRONZE_PICK:
	case BRICK_PICK:
	case MYSTERIOUS_PICK:
	case NANO_PICK:
	case SOFT_MATTOCK:
	case ETERNIUM_MATTOCK:
	case DWARVISH_MATTOCK: /* KMH, balance patch -- the mattock is a pick, too */
		if (uwep && uwep == obj) res = use_pick_axe(obj);
		else {pline("You must wield this item first if you want to apply it!"); 
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			wield_tool(obj, "swing"); }
		break;
	case FISHING_POLE:
		if (uwep && uwep == obj) res = use_pole(obj);
		else {pline("You must wield this item first if you want to apply it!"); 
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			wield_tool(obj, "swing"); }
		break;
	case TINNING_KIT:
		use_tinning_kit(obj);
		break;
	case BINNING_KIT:
		use_binning_kit(obj);
		break;
	case LEATHER_LEASH:
	case ARMORED_LEASH:
	case INKA_LEASH:
	case ADAMANT_LEASH:
		use_leash(obj);
		break;
	case LEATHER_SADDLE:
	case INKA_SADDLE:
	case TANK_SADDLE:
	case BARDING:
	case MESH_SADDLE:
	case SEATBELT_SADDLE:
	case REPELLANT_SADDLE:
	case SOFTNESS_SADDLE:
		res = use_saddle(obj);
		break;
	case MAGIC_WHISTLE:

		if ((evilfriday || !rn2(10)) && !rn2(100) && !(obj->oartifact == ART_SUMMONOR && rn2(10)) && (obj->cursed || !rn2(5)) ) {
			useup(obj);
			noartispeak = TRUE;
			pline("Your whistle breaks apart.");
			break; /* obj no longer exists now */
		}

		use_magic_whistle(obj);
		/* Amy edit: because of our design philosophy that says nothing's supposed to be permanent, give a small chance
		 * of whistles degrading on use. They will never be vaporized, but eventually they'll become cursed. */
		if (!rn2(isfriday ? 20 : 50)) {

			int cursingchance = 10;

			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_PETKEEPING)) {
					default: cursingchance = 10; break;
					case P_BASIC: cursingchance = 9; break;
					case P_SKILLED: cursingchance = 8; break;
					case P_EXPERT: cursingchance = 7; break;
					case P_MASTER: cursingchance = 6; break;
					case P_GRAND_MASTER: cursingchance = 5; break;
					case P_SUPREME_MASTER: cursingchance = 4; break;
				}
			}

			if (obj->oartifact == ART_SUMMONOR && rn2(10)) cursingchance = 0;

			if (cursingchance > rnd(10)) {
				if (obj->blessed) unbless(obj);
				else curse(obj);
				pline("Your whistle seems less effective.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			}
		}
		break;
	case DARK_MAGIC_WHISTLE:

		if ((evilfriday || !rn2(10)) && !rn2(100) && (obj->cursed || !rn2(5)) ) {
			useup(obj);
			noartispeak = TRUE;
			pline("Your whistle breaks apart.");
			break; /* obj no longer exists now */
		}

		use_dark_magic_whistle(obj);
		if (!rn2(isfriday ? 20 : 50)) {

			int cursingchance = 10;

			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_PETKEEPING)) {
					default: cursingchance = 10; break;
					case P_BASIC: cursingchance = 9; break;
					case P_SKILLED: cursingchance = 8; break;
					case P_EXPERT: cursingchance = 7; break;
					case P_MASTER: cursingchance = 6; break;
					case P_GRAND_MASTER: cursingchance = 5; break;
					case P_SUPREME_MASTER: cursingchance = 4; break;
				}
			}

			if (cursingchance > rnd(10)) {
				if (obj->blessed) unbless(obj);
				else curse(obj);
				pline("Your whistle seems less effective.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			}
	
		}
		if (obj->oartifact == ART_GO_AWAY_YOU_BASTARD) {
			if (!obj->cursed) phase_door(0);
			if (!rn2(4)) curse(obj);
		}

		break;
	case TIN_WHISTLE:
	case GRASS_WHISTLE:

		if ((evilfriday || !rn2(10)) && !rn2(100) && !(obj->otyp == GRASS_WHISTLE && is_grassland(u.ux, u.uy) && rn2(50)) && (obj->cursed || !rn2(5)) ) {
			useup(obj);
			noartispeak = TRUE;
			pline("Your whistle breaks apart.");
			break; /* obj no longer exists now */
		}

		if (obj && obj->oartifact == ART_INVISO_PLUS_VALUE && !obj->cursed && !rn2(50) && obj->spe < 10) obj->spe++;

		use_whistle(obj);
		if (!rn2(isfriday ? 20 : 50)) {
			int cursingchance = 10;

			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_PETKEEPING)) {
					default: cursingchance = 10; break;
					case P_BASIC: cursingchance = 9; break;
					case P_SKILLED: cursingchance = 8; break;
					case P_EXPERT: cursingchance = 7; break;
					case P_MASTER: cursingchance = 6; break;
					case P_GRAND_MASTER: cursingchance = 5; break;
					case P_SUPREME_MASTER: cursingchance = 4; break;
				}
			}

			if ((cursingchance > rnd(10)) && !(obj->otyp == GRASS_WHISTLE && is_grassland(u.ux, u.uy)) ) {
				if (obj->blessed) unbless(obj);
				else curse(obj);
				pline("Your whistle seems less effective.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			}
		}
		break;
	case EUCALYPTUS_LEAF:
		/* MRKR: Every Australian knows that a gum leaf makes an */
		/*	 excellent whistle, especially if your pet is a  */
		/*	 tame kangaroo named Skippy.			 */
		if (obj->blessed) {
		    use_magic_whistle(obj);
		    /* sometimes the blessing will be worn off */
		    if (!rn2(isfriday ? 15 : 49)) {
			if (!Blind) {
			    char buf[BUFSZ];

			    pline("%s %s %s.", Shk_Your(buf, obj),
				  aobjnam(obj, "glow"), hcolor("brown"));
			    obj->bknown = 1;
			}
			unbless(obj);
		    }
		} else {
		    use_whistle(obj);
		}
		break;
	case STETHOSCOPE:
	case UNSTABLE_STETHOSCOPE:
		res = use_stethoscope(obj);
		if (res >= 2) noartispeak = TRUE;
		break;
	case MIRROR:
		res = use_mirror(obj);
		if (res >= 2) noartispeak = TRUE;
		break;
	case SPOON:
		pline(FunnyHallu ? "Seems like exactly the thing needed to kill everything in one hit." : "It's a finely crafted antique spoon; what do you want to do with it?");
		break;
	case BELL:
	case BELL_OF_OPENING:
		if (use_bell(&obj)) noartispeak = TRUE;
		break;
	case CANDELABRUM_OF_INVOCATION:
		use_candelabrum(obj);
		break;
	case WAX_CANDLE:
/* STEPHEN WHITE'S NEW CODE */           
	case MAGIC_CANDLE:
	case TALLOW_CANDLE:
	case OIL_CANDLE:
	case UNSPECIFIED_CANDLE:
	case SPECIFIC_CANDLE:
	case __CANDLE:
	case GENERAL_CANDLE:
	case NATURAL_CANDLE:
	case UNAFFECTED_CANDLE:
	case JAPAN_WAX_CANDLE:
		use_candle(&obj);
		break;
	case HEAVY_LASER_BALL:
	case GREEN_LIGHTSABER:
	case ORANGE_LIGHTSABER:
	case BLACK_LIGHTSABER:
  	case BLUE_LIGHTSABER:
  	case DARK_LIGHTSABER:
  	case LASER_TIN_OPENER:
  	case LASERFIST:
  	case MYSTERY_LIGHTSABER:
	case VIOLET_LIGHTSABER:
	case WHITE_LIGHTSABER:
	case YELLOW_LIGHTSABER:
	case RAINBOW_LIGHTSABER:
	case RED_LIGHTSABER:
	case LASER_SWATTER:
	case NANO_HAMMER:
	case LOWER_ARM_SWORD:
	case LIGHTWHIP:
	case ELECTRIC_CIGARETTE:
	case RED_DOUBLE_LIGHTSABER:
	case CYAN_DOUBLE_LIGHTSABER:
	case WHITE_DOUBLE_LIGHTSABER:
	case LASERDENT:
	case LASERXBOW:
	case KLIUSLING:
	case STARWARS_MACE:
	case LASER_SWORD:
	case LIGHTTORCH:
	case LASER_FLYAXE:
	case PINK_LIGHTSWORD:
	case PINK_DOUBLE_LIGHTSWORD:
	case BEAMSWORD:
	case SITH_STAFF:

		if (obj && obj->oartifact == ART_COLONEL_PROUDSTER && !obj->lamplit && obj->age == 0 && u.ugold >= 10000) {
			if (yn("Recharge the lightsaber for 10000 zorkmids?") == 'y') {
				u.ugold -= 10000;
				obj->age += 1000;
				if (obj->otyp == ORANGE_LIGHTSABER) obj->age += (1000 * rnd(2));
				if (obj->oartifact == ART_DESANN_S_WRATH) obj->age += (1000 * rnd(2));
				Your("lightsaber was recharged.");
			}
		}

		if (!(uswapwep == obj && u.twoweap))
		  if (uwep != obj && !wield_tool(obj, (const char *)0)) break;
		/* Fall through - activate via use_lamp */
	case OIL_LAMP:
	case PIT_LAMP:
	case FEANORIAN_LAMP:
	case ELECTRIC_LAMP:
	case MAGIC_LAMP:
	case BRASS_LANTERN:
	case DWARVEN_LANTERN:
	case DIM_LANTERN:
		if (obj && obj->oartifact == ART_FLICKERFLICKER && !obj->cursed) curse(obj);
		if (obj && obj->oartifact == ART_GLOIN_S_BOOM) {
			pline("*krrrrrrrrzzzzzzzzz*");
			wake_nearby();
			HAggravate_monster += rnz(1000);
		}
		use_lamp(obj);
		break;

	case LASER_POLE:

		if (obj && obj->oartifact == ART_COLONEL_PROUDSTER && !obj->lamplit && obj->age == 0 && u.ugold >= 10000) {
			if (yn("Recharge the lightsaber for 10000 zorkmids?") == 'y') {
				u.ugold -= 10000;
				obj->age += 1000;
				if (obj->otyp == ORANGE_LIGHTSABER) obj->age += (1000 * rnd(2));
				if (obj->oartifact == ART_DESANN_S_WRATH) obj->age += (1000 * rnd(2));
				Your("lightsaber was recharged.");
			}
		}

		if (uwep && uwep == obj && uwep->lamplit && uwep->altmode) {
			res = use_pole(obj);
			break;
		}

		if (!(uswapwep == obj && u.twoweap))
		  if (uwep != obj && !wield_tool(obj, (const char *)0)) break;
		use_lamp(obj);
		break;

	case LASER_CHAIN:

		if (obj && obj->oartifact == ART_COLONEL_PROUDSTER && !obj->lamplit && obj->age == 0 && u.ugold >= 10000) {
			if (yn("Recharge the lightsaber for 10000 zorkmids?") == 'y') {
				u.ugold -= 10000;
				obj->age += 1000;
				if (obj->otyp == ORANGE_LIGHTSABER) obj->age += (1000 * rnd(2));
				if (obj->oartifact == ART_DESANN_S_WRATH) obj->age += (1000 * rnd(2));
				Your("lightsaber was recharged.");
			}
		}

		if (uwep && uwep == obj && uwep->lamplit) {
			res = use_pole(obj);
			break;
		}

		if (!(uswapwep == obj && u.twoweap))
		  if (uwep != obj && !wield_tool(obj, (const char *)0)) break;
		use_lamp(obj);
		break;

	case TORCH:
	        res = use_torch(obj);
		break;
	case POT_OIL:
		light_cocktail(obj);
		break;
	case EXPENSIVE_CAMERA:
		if (obj->oartifact == ART_RAPIDCLICK && obj->spe > 0) {
			use_camera(obj);
			res = 0;
			break;
		} else {
			res = use_camera(obj);
		}
		break;
	case TOWEL:
		res = use_towel(obj);
		break;
	case CRYSTAL_BALL:
		if (use_crystal_ball(obj)) noartispeak = TRUE;
		break;
/* STEPHEN WHITE'S NEW CODE
 * KMH, balance patch -- source of abuse
 * Amy edit: we can balance it! for example, by making orbs of charging much more limited */
	case ORB_OF_ENCHANTMENT:
	    if(obj->spe > 0) {

		check_unpaid(obj);
		if (uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == GEM_CLASS || uwep->oclass == CHAIN_CLASS || uwep->oclass == VENOM_CLASS || is_weptool(uwep) ) ) {

			if (stack_too_big(uwep)) {
				pline("Unfortunately your stack of weapons was too big and therefore nothing happens.");
			} else if (uwep->spe < 5) {
				if (obj->blessed) {
					if (!Blind) pline("Your %s glows silver.",xname(uwep));
					uwep->spe += rnd(2);
				} else if (obj->cursed) {                               
					if (!Blind) pline("Your %s glows black.",xname(uwep));
					uwep->spe -= rnd(2);
				} else {
					if (rn2(3)) {
						if (!Blind) pline("Your %s glows bright for a moment." ,xname(uwep));
						uwep->spe += 1;
					} else {
						if (!Blind) pline("Your %s glows dark for a moment." ,xname(uwep));
						uwep->spe -= 1;
					}
				}

				if (uwep->spe > 5) uwep->spe = 5;

			} else {
				pline("Nothing seems to happen.");                
			}
		} else {
			pline("The orb glows for a moment, then fades.");
		}

		int nochargechange = 10;
		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_DEVICES)) {
				default: break;
				case P_BASIC: nochargechange = 9; break;
				case P_SKILLED: nochargechange = 8; break;
				case P_EXPERT: nochargechange = 7; break;
				case P_MASTER: nochargechange = 6; break;
				case P_GRAND_MASTER: nochargechange = 5; break;
				case P_SUPREME_MASTER: nochargechange = 4; break;
			}
		}

		if (nochargechange >= rnd(10)) consume_obj_charge(obj, FALSE);

	    } else {
		pline("This orb is burnt out.");
		if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
	    }
	    break;
	case ORB_OF_CHARGING:
		if(obj->spe > 0) {

			register struct obj *otmp;
			makeknown(ORB_OF_CHARGING);
			/* always uses up a charge because this thing can only be recharged once --Amy */
			consume_obj_charge(obj, TRUE);
			otmp = getobj(all_count, "charge");
			if (!otmp) break;
			recharge(otmp, obj->cursed ? -1 : (obj->blessed ? 1 : 0));
		} else {
			pline("This orb is burnt out.");
			if (!PlayerCannotUseSkills && P_SKILL(P_DEVICES) >= P_SKILLED) obj->known = TRUE;
		}
		break;
	case ORB_OF_DESTRUCTION:
		{
			boolean extrakaboom = FALSE;
			if (obj->oartifact == ART_CAEAUWAESH_) extrakaboom = TRUE;

			int explodestrength = min(u.ulevel, 12); /* avoid poison pill problem --Amy */
			noartispeak = TRUE;
			useup(obj);
			pline("As you activate the orb, it explodes!");
			explode(u.ux, u.uy, ZT_SPELL(ZT_MAGIC_MISSILE), d(explodestrength,6), WAND_CLASS, EXPL_MAGICAL);

			if (extrakaboom) {
				coord cc;
				int kaboomxtras = 10;
				pline("Kaboomboom-booooooom!");

				while (kaboomxtras > 0) {
					kaboomxtras--;
					cc.x = rn1(COLNO-3,2);
					cc.y = rn2(ROWNO);

					explode(cc.x, cc.y, ZT_SPELL(ZT_MAGIC_MISSILE), d(explodestrength,6), WAND_CLASS, EXPL_MAGICAL);

				}
				pline("CAEAUWAESH!");

			}

			check_unpaid(obj);
		}
		break;
	case MAGIC_MARKER:
		res = dowrite(obj);
		break;
	case TIN_OPENER:
	case BUDO_NO_SASU:
	case JEONTU_GEOM:
		if(!carrying(TIN)) {
			You("have no tin to open.");
			goto xit;
		}
		You("cannot open a tin without eating or discarding its contents.");
		if(flags.verbose)
			pline("In order to eat, use the 'e' command.");
		if(obj != uwep)
			pline("Opening the tin will be much easier if you wield the tin opener.");
		goto xit;

	case FIGURINE:
		use_figurine(&obj);
		break;
	case ENERGY_SAP:
		energysap(obj);
		if (obj && obj->oartifact == ART_LOWER_TURNCOUNT) {
			int turnreduce = rn1(500, 500);
			monstermoves -= turnreduce;
			if (monstermoves < 1) monstermoves = 1;
			moves -= turnreduce;
			if (moves < 1) moves = 1;
			pline_The("turn counter has been reduced, courtesy of bugsniper.");
		}

		noartispeak = TRUE;
		break;
	case UNICORN_HORN:
	case SKY_HORN:
	case REPLICA_UNICORN_HORN:
	case TITANIUM_HORN:

		if (obj && obj->oartifact == ART_FFFF_ERR_WHAT_AM_I_TYPING_) {
			if (u.ualign.type == A_LAWFUL) {
				if (!FemaleTrapInge) pline("All the elegant ladies will produce very tender farting noises that you just cannot resist.");
				FemaleTrapInge += rnz(500);
			}
			if (u.ualign.type == A_NEUTRAL) {
				if (!FemaleTrapElif) pline("A ghostly girl (named Elif) starts following you around, and apparently she wants to play with you!");
				FemaleTrapElif += rnz(500);
			}
			if (u.ualign.type == A_CHAOTIC) {
				if (!FemaleTrapJessica) pline("Your butt cheeks suddenly feel very tender, and in fact, a similar thing is happening to your entire body!");
				FemaleTrapJessica += rnz(500);
			}
		}

		if (use_unicorn_horn(obj)) noartispeak = TRUE;
		break;
	case BUBBLEHORN:
		if (use_bubble_horn(obj)) noartispeak = TRUE;
		break;
	case BRICK_HORN:
		if (use_brick_horn(obj)) noartispeak = TRUE;
		break;
	case DARK_HORN:
		if (Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) goto undark;
		pline("That felt evil and sinister!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Konechno, Sovetskiy sdelal eto tak, chto vy ne mozhete uvidet' bol'shinstvo monstrov pryamo seychas. Kha-kha-kha ..." : "Diedaedodiedaerr!");
		litroomlite(FALSE);
undark:
		if (use_unicorn_horn(obj)) noartispeak = TRUE;
		break;
	case WOODEN_FLUTE:
	case MAGIC_FLUTE:
	case TOOLED_HORN:
	case FOG_HORN:
	case GUITAR:
	case BANJO:
	case PIANO:
	case FROST_HORN:
	case TEMPEST_HORN:
	case ETHER_HORN:
	case SHADOW_HORN:
	case CHROME_HORN:
	case DEATH_HORN:
	case FIRE_HORN:
	case WOODEN_HARP:
	case MAGIC_HARP:
	case BUGLE:
	case LEATHER_DRUM:
	case DRUM_OF_EARTHQUAKE:
	/* KMH, balance patch -- removed, but re-inserted by Amy */
	case PAN_PIPE_OF_SUMMONING:                
	case PAN_PIPE_OF_THE_SEWERS:
	case PAN_PIPE:
		if (obj->oartifact == ART_PFIE_PFIEPFIE) obj->known = TRUE;
		res = do_play_instrument(obj);
		if (res == 2) noartispeak = TRUE; /* it broke */
		if (res != 2 && obj && obj->oartifact == ART_TAETAERAETAEAE_TAE && rn2(2)) res = 0;
		break;
	case MEDICAL_KIT:        
		if (Role_if(PM_HEALER) || (uarm && uarm->oartifact == ART_GREEN_DRESS_NURSE) || (obj->oartifact == ART_MARVELOUS_FUNCTION) || Race_if(PM_HERBALIST) || (uarmf && uarmf->oartifact == ART_GOT_THAT_STARWARS_ENTRANCE) ) can_use = TRUE;
		else if ((Role_if(PM_PRIEST) || Role_if(PM_MONK) ||
			Role_if(PM_UNDEAD_SLAYER) || Role_if(PM_SAMURAI)) &&
			!rn2(2)) can_use = TRUE;
		else if(!rn2(4)) can_use = TRUE;

		if (obj->cursed && rn2(3)) can_use = FALSE;
		if (obj->blessed && rn2(3)) can_use = TRUE;  

		makeknown(MEDICAL_KIT);
		if (obj->cobj) {
		    struct obj *otmp;
		    for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
			if (otmp->otyp == PILL)
			    break;
		    if (!otmp)
			You_cant("find any more pills in %s.", yname(obj));
		    else if (!is_edible(otmp))
			You("find, but cannot eat, a white pill in %s.",
			  yname(obj));
		    else {
			int pilltype = 0; /* for artifact pills, because they may be freed below --Amy */
			if (otmp->oartifact == ART_PILL_THAT_KILLED_MICHAEL_J) pilltype = 1;
			if (otmp->oartifact == ART_LASTER_EVER) pilltype = 2;
			if (otmp->oartifact == ART_GARAWORK) can_use = TRUE;

			if (pilltype == 2 && rn2(5)) goto pillremoved;

			check_unpaid(obj);
			if (otmp->quan > 1L) {
			    otmp->quan--;
			    obj->owt = weight(obj);
			} else {
			    obj_extract_self(otmp);
			    obfree(otmp, (struct obj *)0);
			}

pillremoved:
			use_skill(P_DEVICES,1);
			if (Race_if(PM_FAWN)) {
				use_skill(P_DEVICES,1);
			}
			if (Race_if(PM_SATRE)) {
				use_skill(P_DEVICES,1);
				use_skill(P_DEVICES,1);
			}

			/*
			 * Note that while white and pink pills share the
			 * same otyp value, they are quite different.
			 */
			You("take a white pill from %s and swallow it.",
				yname(obj));

			/* artifact pill effects are checked first, then there's the regular ones --Amy */

			if (pilltype == 1) {
				if (!Sick) make_sick(rn1(15,15), "Michael Jackson's pill", TRUE, SICK_VOMITABLE);
				losehp(rn1(30, 30),"poisonous pill",KILLED_BY_AN);
				(void) adjattrib(A_STR, -rnd(6), FALSE, TRUE);
				(void) adjattrib(A_DEX, -rnd(6), FALSE, TRUE);
				(void) adjattrib(A_CON, -rnd(6), FALSE, TRUE);
				(void) adjattrib(A_INT, -rnd(6), FALSE, TRUE);
				(void) adjattrib(A_WIS, -rnd(6), FALSE, TRUE);
				(void) adjattrib(A_CHA, -rnd(6), FALSE, TRUE);
			} else if (can_use) {
			    upnivel(FALSE);
			    if (Sick) make_sick(0L, (char *) 0,TRUE ,SICK_ALL);
			    else if (Blinded > (long)(u.ucreamed+1))
				make_blinded(u.ucreamed ?
					(long)(u.ucreamed+1) : 0L, TRUE);
			    else if (HHallucination)
				make_hallucinated(0L, TRUE, 0L);
			    else if (Vomiting && !(uarmf && uarmf->oartifact == ART_CRUEL_GODDESS_ANA)) make_vomiting(0L, TRUE);
			    else if (HConfusion) make_confused(0L, TRUE);
			    else if (HStun) make_stunned(0L, TRUE);
			    else if (HNumbed) make_numbed(0L, TRUE);
			    else if (HFrozen) make_frozen(0L, TRUE);
			    else if (HBurned) make_burned(0L, TRUE);
			    else if (HFeared) make_feared(0L, TRUE);
			    else if (HDimmed) make_dimmed(0L, TRUE);
			    else if (u.uhp < u.uhpmax) {
				u.uhp += rn1(10,10);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				You_feel("better.");
				flags.botl = TRUE;
			    } else {
					pline("%s", nothing_happens);
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
			} else if (!rn2(3))
			    pline("Nothing seems to happen.");
			else if (!Sick) {
			    make_sick(rn1(15,15), "bad pill", TRUE,
			      SICK_VOMITABLE);
			} else {
			    You("seem to have made your condition worse!");
			    losehp(rn1(10,10), "a drug overdose", KILLED_BY);
			}
		    }
		} else You("seem to be out of medical supplies");
		break;
	case HORN_OF_PLENTY:	/* not a musical instrument */

		if (obj && obj->oartifact == ART_PLENTYHORN_OF_FAMINE) {
			pline("You feel more hungry.");
			morehungry(rnz(80));
		}

		if (obj->spe > 0) {
		    struct obj *otmp;
		    const char *what;

			int nochargechange = 10;
			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_DEVICES)) {
					default: break;
					case P_BASIC: nochargechange = 9; break;
					case P_SKILLED: nochargechange = 8; break;
					case P_EXPERT: nochargechange = 7; break;
					case P_MASTER: nochargechange = 6; break;
					case P_GRAND_MASTER: nochargechange = 5; break;
					case P_SUPREME_MASTER: nochargechange = 4; break;
				}
			}

			if (nochargechange >= rnd(10)) consume_obj_charge(obj, TRUE);
		    if (!rn2(13)) {
			otmp = mkobj(POTION_CLASS, FALSE, FALSE);
			/* KMH, balance patch -- rewritten */
			while ((otmp->otyp == POT_SICKNESS) || (otmp->otyp == POT_POISON) ||
					objects[otmp->otyp].oc_magic)
			    otmp->otyp = rnd_class(POT_BOOZE, POT_AMNESIA);

			/* Amy: lol as if you could just change the potion's type post-creation. if it originally wasn't oil,
			 * and then becomes oil, it'll not have this bit set properly! */
			if (otmp->otyp == POT_OIL) otmp->age = MAX_OIL_IN_FLASK;

			what = "A potion";
		    } else {
			otmp = mkobj(FOOD_CLASS, FALSE, FALSE);
			if (otmp->otyp == FOOD_RATION && !rn2(7))
			    otmp->otyp = LUMP_OF_ROYAL_JELLY;
			what = "Some food";
		    }
		    pline("%s spills out.", what);
		    otmp->blessed = obj->blessed;
		    otmp->cursed = obj->cursed;
		    otmp->owt = weight(otmp);
		    otmp = hold_another_object(otmp, u.uswallow ?
				       "Oops!  %s out of your reach!" :
					(Is_airlevel(&u.uz) ||
					 Is_waterlevel(&u.uz) ||
					 levl[u.ux][u.uy].typ < IRONBARS ||
					 levl[u.ux][u.uy].typ >= ICE) ?
					       "Oops!  %s away from you!" :
					       "Oops!  %s to the floor!",
					       The(aobjnam(otmp, "slip")),
					       (const char *)0);
		    makeknown(HORN_OF_PLENTY);
			use_skill(P_DEVICES,1);
			if (Race_if(PM_FAWN)) {
				use_skill(P_DEVICES,1);
			}
			if (Race_if(PM_SATRE)) {
				use_skill(P_DEVICES,1);
				use_skill(P_DEVICES,1);
			}
		} else {
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case LAND_MINE:
	case BEARTRAP:
		use_trap(obj);
		break;
	case FLINT:
	case LUCKSTONE:
	case LOADSTONE:
	case TOUCHSTONE:
	case HEALTHSTONE:
	case WHETSTONE:
	case MANASTONE:
	case LOADBOULDER:
	case TALC:
	case GRAPHITE:
	case BONE_FRAGMENT:
	case METEOR_FRAGMENT:
	case AMBER_FRAGMENT:
	case LEAD_CLUMP:
	case JUNK_METAL:
	case COBALT_CHUNK:
	case POINTY_SHELL:
	case BRONZE_NUGGET:
	case STEEL_SLUG:
	case SLING_AMMO:
	case VOLCANIC_GLASS_FRAGMENT:
	case STARLIGHTSTONE:
	case STONE_OF_MAGIC_RESISTANCE:
	case SLEEPSTONE:

	case ELIF_S_JEWEL:
	case MADELEINE_S_JEWEL:
	case SANDRA_S_JEWEL:
	case NADJA_S_JEWEL:
	case SOLVEJG_S_JEWEL:
	case THAI_S_JEWEL:
	case ELENA_S_JEWEL:
	case WENDY_S_JEWEL:
	case ANASTASIA_S_JEWEL:
	case JESSICA_S_JEWEL:
	case MARLENA_S_JEWEL:
	case JOHANETTA_S_JEWEL:
	case CHLOE_S_JEWEL:
	case ANJA_S_JEWEL:
	case CELIA_S_JEWEL:
	case JUDITH_S_JEWEL:
	case TANJA_S_JEWEL:
	case SONJA_S_JEWEL:
	case RHEA_S_JEWEL:
	case LARA_S_JEWEL:
	case NADINE_S_JEWEL:
	case LUISA_S_JEWEL:
	case IRINA_S_JEWEL:
	case LISELOTTE_S_JEWEL:
	case GRETA_S_JEWEL:
	case JANE_S_JEWEL:
	case SUE_LYN_S_JEWEL:
	case SABINE_S_JEWEL:
	case CHARLOTTE_S_JEWEL:
	case HANNAH_S_JEWEL:
	case LITTLE_MARIE_S_JEWEL:
	case ROSA_S_JEWEL:
	case FEMMY_S_JEWEL:
	case NATALJE_S_JEWEL:
	case KARIN_S_JEWEL:
	case JEANETTA_S_JEWEL:
	case KATI_S_JEWEL:
	case CLAUDIA_S_JEWEL:
	case VICTORIA_S_JEWEL:
	case MAURAH_S_JEWEL:
	case JUEN_S_JEWEL:
	case KRISTINA_S_JEWEL:
	case SARAH_S_JEWEL:
	case KATHARINA_S_JEWEL:
	case JULIETTA_S_JEWEL:
	case MELTEM_S_JEWEL:
	case SABRINA_S_JEWEL:
	case MELISSA_S_JEWEL:
	case LUDGERA_S_JEWEL:
	case YVONNE_S_JEWEL:
	case EVELINE_S_JEWEL:
	case NELLY_S_JEWEL:
	case ARABELLA_S_JEWEL:
	case RUEA_S_JEWEL:
	case JETTE_S_JEWEL:
	case VERENA_S_JEWEL:
	case LOU_S_JEWEL:
	case ANITA_S_JEWEL:
	case MARIKE_S_JEWEL:
	case KRISTIN_S_JEWEL:
	case HENRIETTA_S_JEWEL:
	case ALMUT_S_JEWEL:
	case ANNA_S_JEWEL:
	case SING_S_JEWEL:
	case INA_S_JEWEL:
	case DORA_S_JEWEL:
	case ANNEMARIE_S_JEWEL:
	case JIL_S_JEWEL:
	case JANA_S_JEWEL:
	case KATRIN_S_JEWEL:
	case GUDRUN_S_JEWEL:
	case ELLA_S_JEWEL:
	case MANUELA_S_JEWEL:
	case JENNIFER_S_JEWEL:
	case PATRICIA_S_JEWEL:
	case ANTJE_S_JEWEL:
	case ANTJE_S_SECOND_JEWEL:
	case KERSTIN_S_JEWEL:
	case LAURA_S_JEWEL:
	case LARISSA_S_JEWEL:
	case NORA_S_JEWEL:
	case NATALIA_S_JEWEL:
	case SUSANNE_S_JEWEL:
	case LISA_S_JEWEL:
	case BRIDGHITTE_S_JEWEL:
	case JULIA_S_JEWEL:
	case NICOLE_S_JEWEL:
	case RITA_S_JEWEL:
	case JANINA_S_JEWEL:
	case KSENIA_S_JEWEL:
	case LYDIA_S_JEWEL:
	case JANET_S_JEWEL:
	case CONNY_S_JEWEL:
	case KATIA_S_JEWEL:
	case MARIYA_S_JEWEL:
	case ELISE_S_JEWEL:
	case RONJA_S_JEWEL:
	case ARIANE_S_JEWEL:
	case JOHANNA_S_JEWEL:
	case INGE_S_JEWEL:
	case RUTH_S_JEWEL:
	case MAGDALENA_S_JEWEL:
	case MARLEEN_S_JEWEL:
	case KLARA_S_JEWEL:
	case FRIEDERIKE_S_JEWEL:
	case NAOMI_S_JEWEL:
	case UTE_S_JEWEL:
	case JASIEEN_S_JEWEL:
	case YASAMAN_S_JEWEL:
	case MAY_BRITT_S_JEWEL:

	case RIGHT_MOUSE_BUTTON_STONE:
 	case DISPLAY_LOSS_STONE:
 	case SPELL_LOSS_STONE:
 	case YELLOW_SPELL_STONE:
 	case AUTO_DESTRUCT_STONE:
 	case MEMORY_LOSS_STONE:
 	case INVENTORY_LOSS_STONE:
 	case BLACKY_STONE:
 	case MENU_BUG_STONE:
 	case SPEEDBUG_STONE:
 	case SUPERSCROLLER_STONE:
 	case FREE_HAND_BUG_STONE:
 	case UNIDENTIFY_STONE:
 	case STONE_OF_THIRST:
 	case UNLUCKY_STONE:
 	case SHADES_OF_GREY_STONE:
 	case STONE_OF_FAINTING:
 	case STONE_OF_CURSING:
 	case STONE_OF_DIFFICULTY:
 	case DEAFNESS_STONE:
 	case ANTIMAGIC_STONE:
 	case WEAKNESS_STONE:
 	case ROT_THIRTEEN_STONE:
 	case BISHOP_STONE:
 	case CONFUSION_STONE:
 	case DROPBUG_STONE:
 	case DSTW_STONE:
 	case STATUS_STONE:
 	case ALIGNMENT_STONE:
 	case STAIRSTRAP_STONE:
	case UNINFORMATION_STONE:
	case CAPTCHA_STONE:
	case FARLOOK_STONE:
	case RESPAWN_STONE:

	case AMNESIA_STONE:
	case BIGSCRIPT_STONE:
	case BANK_STONE:
	case MAP_STONE:
	case TECHNIQUE_STONE:
	case DISENCHANTMENT_STONE:
	case VERISIERT_STONE:
	case CHAOS_TERRAIN_STONE:
	case MUTENESS_STONE:
	case ENGRAVING_STONE:
	case MAGIC_DEVICE_STONE:
	case BOOK_STONE:
	case LEVEL_STONE:
	case QUIZ_STONE:

	case STONE_OF_INTRINSIC_LOSS:
	case BLOOD_LOSS_STONE:
	case BAD_EFFECT_STONE:
	case TRAP_CREATION_STONE:
	case BOUND_DAY_STONE:
	case STONE_OF_VULNERABILITY:
	case ITEM_TELEPORTING_STONE:
	case NASTY_STONE:

	case ORANGE_SPELL_STONE:
	case VIOLET_SPELL_STONE:
	case LONGING_STONE:
	case CURSED_PART_STONE:
	case QUAVERSAL_STONE:
	case APPEARANCE_SHUFFLING_STONE:
	case BROWN_SPELL_STONE:
	case CHOICELESS_STONE:
	case GOLDSPELL_STONE:
	case DEPROVEMENT_STONE:
	case INITIALIZATION_STONE:
	case GUSHLUSH_STONE:
	case SOILTYPE_STONE:
	case DANGEROUS_TERRAIN_STONE:
	case FALLOUT_STONE:
	case MOJIBAKE_STONE:
	case GRAVATION_STONE:
	case UNCALLED_STONE:
	case EXPLODING_DICE_STONE:
	case PERMACURSE_STONE:
	case SHROUDED_IDENTITY_STONE:
	case FEELER_GAUGE_STONE:
	case LONG_SCREWUP_STONE:
	case WING_YELLOW_STONE:
	case LIFE_SAVING_STONE:
	case CURSEUSE_STONE:
	case CUT_NUTRITION_STONE:
	case SKILL_LOSS_STONE:
	case AUTOPILOT_STONE:
	case FORCE_STONE:
	case MONSTER_GLYPH_STONE:
	case CHANGING_DIRECTIVE_STONE:
	case CONTAINER_KABOOM_STONE:
	case STEAL_DEGRADE_STONE:
	case LEFT_INVENTORY_STONE:
	case FLUCTUATING_SPEED_STONE:
	case TARMUSTROKINGNORA_STONE:
	case FAILURE_STONE:
	case BRIGHT_CYAN_SPELL_STONE:
	case FREQUENTATION_SPAWN_STONE:
	case PET_AI_STONE:
	case SATAN_STONE:
	case REMEMBERANCE_STONE:
	case POKELIE_STONE:
	case AUTOPICKUP_STONE:
	case DYWYPI_STONE:
	case SILVER_SPELL_STONE:
	case METAL_SPELL_STONE:
	case PLATINUM_SPELL_STONE:
	case MANLER_STONE:
	case DOORNING_STONE:
	case NOWNSIBLE_STONE:
	case ELM_STREET_STONE:
	case MONNOISE_STONE:
	case RANG_CALL_STONE:
	case RECURRING_SPELL_LOSS_STONE:
	case ANTI_TRAINING_STONE:
	case TECHOUT_STONE:
	case STAT_DECAY_STONE:
	case MOVEMORK_STONE:
	case SANE_TREBLE_STONE:
	case STATCREASE_STONE:
	case SIMEOUT_STONE:

	case METABOLIC_STONE:
	case STONE_OF_NO_RETURN:
	case EGOSTONE:
	case FAST_FORWARD_STONE:
	case ROTTEN_STONE:
	case UNSKILLED_STONE:
	case LOW_STAT_STONE:
	case TRAINING_STONE:
	case EXERCISE_STONE:

	case TURN_LIMIT_STONE:
	case WEAK_SIGHT_STONE:
	case CHATTER_STONE:
 	case NONSACRED_STONE:
 	case STARVATION_STONE:
 	case DROPLESS_STONE:
 	case LOW_EFFECT_STONE:
 	case INVISO_STONE:
 	case GHOSTLY_STONE:
 	case DEHYDRATING_STONE:
 	case STONE_OF_HATE:
 	case DIRECTIONAL_SWAP_STONE:
 	case NONINTRINSICAL_STONE:
 	case DROPCURSE_STONE:
 	case STONE_OF_NAKED_STRIPPING:
 	case ANTILEVEL_STONE:
 	case STEALER_STONE:
 	case REBEL_STONE:
 	case SHIT_STONE:
 	case STONE_OF_MISFIRING:
 	case STONE_OF_PERMANENCE:
 	case EPVI_STONE:
 	case STONE_OF_COMPLETE_FUCKUP:

	case DISCONNECT_STONE:
	case SCREW_STONE:
	case BOSSFIGHT_STONE:
	case ENTIRE_LEVEL_STONE:
	case BONE_STONE:
	case AUTOCURSE_STONE:
	case HIGHLEVEL_STONE:
	case SPELL_MEMORY_STONE:
	case SOUND_EFFECT_STONE:
	case TIME_USE_STONE:
	case MULCH_STONE:
	case ANTISWITCH_STONE:
	case SPELL_COOLDOWN_STONE:
	case HAVE_A_PIN_STONE:
	case TURBODULL_STONE:
	case INACCURACY_STONE:
	case INVENTORY_SIZE_STONE:
	case COSTLY_FAILURE_STONE:
	case REDDAM_STONE:
	case REDINC_STONE:
	case MONSTER_MULTISHOT_STONE:
	case NASTYCURSE_STONE:
	case BAD_PART_STONE:
	case COMPLETELY_BAD_PART_STONE:
	case EVIL_VARIANT_STONE:
	case REALLY_BAD_STONE:
	case COVID_STONE:
	case BLAST_STONE:

	case OPTION_STONE:
	case MISCOLOR_STONE:
	case AEFDE_STONE:
	case ONE_RAINBOW_STONE:
	case COLORSHIFT_STONE:
	case TOP_LINE_STONE:
	case CAPS_STONE:
	case UN_KNOWLEDGE_STONE:
	case DARKHANCE_STONE:
	case DSCHUEUEUET_STONE:
	case NOPESKILL_STONE:

	case REAL_LIE_STONE:
	case ESCAPE_PAST_STONE:
	case PETHATE_STONE:
	case PET_LASHOUT_STONE:
	case PETSTARVE_STONE:
	case PETSCREW_STONE:
	case TECH_LOSS_STONE:
	case PROOFLOSS_STONE:
	case UN_INVIS_STONE:
	case DETECTATION_STONE:

	case LOOTCUT_STONE:
	case MONSTER_SPEED_STONE:
	case SCALING_STONE:
	case INIMICAL_STONE:
	case WHITE_SPELL_STONE:
	case GREYOUT_STONE:
	case QUASAR_STONE:
	case MOMMY_STONE:
	case HORROR_STONE:
	case ARTIFICIAL_STONE:
	case WEREFORM_STONE:
	case ANTIPRAYER_STONE:
	case EVIL_PATCH_STONE:
	case HARD_MODE_STONE:
	case SECRET_ATTACK_STONE:
	case EATER_STONE:
	case COVETOUS_STONE:
	case NON_SEEING_STONE:
	case DARKMODE_STONE:
	case UNFINDABLE_STONE:
	case HOMICIDE_STONE:
	case MULTITRAPPING_STONE:
	case WAKEUP_CALL_STONE:
	case GRAYOUT_STONE:
	case GRAY_CENTER_STONE:
	case CHECKERBOARD_STONE:
	case CLOCKWISE_STONE:
	case COUNTERCLOCKWISE_STONE:
	case LAG_STONE:
	case BLESSCURSE_STONE:
	case DELIGHT_STONE:
	case DISCHARGE_STONE:
	case TRASH_STONE:
	case FILTERING_STONE:
	case DEFORMATTING_STONE:
	case FLICKER_STRIP_STONE:
	case UNDRESSING_STONE:
	case HYPER_BLUE_STONE:
	case NO_LIGHT_STONE:
	case PARANOIA_STONE:
	case FLEECE_STONE:
	case INTERRUPTION_STONE:
	case DUSTBIN_STONE:
	case BATTERY_STONE:
	case BUTTERFINGER_STONE:
	case MISCASTING_STONE:
	case MESSAGE_SUPPRESSION_STONE:
	case STUCK_ANNOUNCEMENT_STONE:
	case STORM_STONE:
	case MAXIMUM_DAMAGE_STONE:
	case LATENCY_STONE:
	case STARLIT_SKY_STONE:
	case TRAP_KNOWLEDGE_STONE:
	case HIGHSCORE_STONE:
	case PINK_SPELL_STONE:
	case GREEN_SPELL_STONE:
	case EVC_STONE:
	case UNDERLAID_STONE:
	case DAMAGE_METER_STONE:
	case WEIGHT_STONE:
	case INFOFUCK_STONE:
	case BLACK_SPELL_STONE:
	case CYAN_SPELL_STONE:
	case HEAP_STONE:
	case BLUE_SPELL_STONE:
	case TRON_STONE:
	case RED_SPELL_STONE:
	case TOO_HEAVY_STONE:
	case ELONGATED_STONE:
	case WRAPOVER_STONE:
	case DESTRUCTION_STONE:
	case MELEE_PREFIX_STONE:
	case AUTOMORE_STONE:
	case MEAN_BURDEN_STONE:
	case CARRCAP_STONE:
	case UMENG_STONE:
	case UNFAIR_ATTACK_STONE:

	case SALT_CHUNK:
	case SILVER_SLINGSTONE:
	case CONUNDRUM_NUGGET:
	case SMALL_PIECE_OF_UNREFINED_MITHR:
		use_stone(obj);
		break;
	case ASSAULT_RIFLE:
	case STORM_RIFLE:
	case KALASHNIKOV:
	case MILITARY_RIFLE:
		/* Switch between WP_MODE_SINGLE, WP_MODE_BURST and WP_MODE_AUTO */
		/* Amy edit: with enough firearm or gun control skill, you can select the mode */

		if (!PlayerCannotUseSkills && (P_SKILL(P_FIREARM) >= P_EXPERT || P_SKILL(P_GUN_CONTROL) >= P_EXPERT)) {

			winid tmpwin;
			anything any;
			menu_item *selected;
			int n;

			any.a_void = 0;         /* zero out all bits */
			tmpwin = create_nhwindow(NHW_MENU);
			start_menu(tmpwin);
			any.a_int = 1;
			add_menu(tmpwin, NO_GLYPH, &any , 'a', 0, ATR_NONE, "Automatic", MENU_UNSELECTED);
			any.a_int = 2;
			add_menu(tmpwin, NO_GLYPH, &any , 'b', 0, ATR_NONE, "Burst Fire", MENU_UNSELECTED);
			any.a_int = 3;
			add_menu(tmpwin, NO_GLYPH, &any , 's', 0, ATR_NONE, "Single Shot", MENU_UNSELECTED);

			end_menu(tmpwin, "Select a fire mode");
			n = select_menu(tmpwin, PICK_ONE, &selected);
			destroy_nhwindow(tmpwin);

			if (n > 0) {
				switch (selected[0].item.a_int) {
					case 1:
						obj->altmode = WP_MODE_AUTO;
						break;
					case 2:
						obj->altmode = WP_MODE_BURST;
						break;
					case 3:
						obj->altmode = WP_MODE_SINGLE;
						break;

				}

			}

		} else {

			if (obj->altmode == WP_MODE_AUTO) {
				obj->altmode = WP_MODE_BURST;
			} else if (obj->altmode == WP_MODE_BURST) {
				obj->altmode = WP_MODE_SINGLE;
			} else {
				obj->altmode = WP_MODE_AUTO;
			}
		}
		
		You("switch %s to %s mode.", yname(obj), 
			((obj->altmode == WP_MODE_SINGLE) ? "single shot" : 
			 ((obj->altmode == WP_MODE_BURST) ? "burst" :
			  "full automatic")));
		if (PlayerHearsSoundEffects) pline(issoviet ? "Eto ne budet v lyubom sluchaye pomozhet vam." : "Sr!");
		break;	
	case AUTO_SHOTGUN:
	case DEMON_CROSSBOW:
	case SUBMACHINE_GUN:
	case LEAD_UNLOADER:

		if (obj->oartifact == ART_NOZZLE_CHANGE) {
			pline("Sorry, the time for a nozzle change hasn't come yet!");
			break;
		}

		if (obj->altmode == WP_MODE_AUTO) obj->altmode = WP_MODE_SINGLE;
		else obj->altmode = WP_MODE_AUTO;
		You("switch %s to %s mode.", yname(obj), 
			(obj->altmode ? "semi-automatic" : "full automatic"));
		if (PlayerHearsSoundEffects) pline(issoviet ? "Eto ne budet v lyubom sluchaye pomozhet vam." : "Sr!");
		break;
	case FRAG_GRENADE:
	case GAS_GRENADE:
		if (!obj->oarmed) {
			You("arm %s.", yname(obj));
			arm_bomb(obj, TRUE);
		} else pline(FunnyHallu ? "Oh, CRAP! You hear the ticking of a bomb! Throw it away, NOW!" : "It's already armed!");
		break;
	case STICK_OF_DYNAMITE:
		light_cocktail(obj);
		if (practicantterror) {
			pline("%s thunders: 'High explosives??? Are you out of your mind? In expectation of the damage to property that you'll likely cause, I decided that you pay 5000 zorkmids as compensation and 1000 stones to allow me to rebuild everything that you've destroyed.'", noroelaname());
			fineforpracticant(5000, 1000, 0);
		}
		break;

	case HITCHHIKER_S_GUIDE_TO_THE_GALA:
		if (obj->oartifact == ART_END_OF_THE_WORLD_SWITCH) {
			if (!u.ragnaroktimer) u.ragnaroktimer = rnz(100000);
			pline("click"); /* intentionally vague */
			if (!rn2(5)) pline("Hmm... nothing seems to have happened.");
		} else if (HHallucination) {
			pline("You carelessly push the buttons. On the screen is a text ... ");
			outrumor(-1,42,TRUE);	/* always false */
		} else if (u.usanity < 900) {
			pline("So many knobs to turn! So many buttons to press!");
			/* Amy edit: omg no, we can't allow such an easy guaranteed way for confusion on demand... */
			if (!rn2(20)) {
				if (obj->blessed) unbless(obj);
				else curse(obj);
				pline("And now the universe seems to be turning inwards on itself! Oh no!");
			}
			badeffect();
			increasesanity(1000);
			if (!obj->cursed || !rn2(4)) make_confused(HConfusion+rn2(10),TRUE);
		} else {
			pline("No! It's insane to mess with that thing!");
		}
		break;

	case RELAY:
		if (obj->oartifact == ART_BURNED_MOTH_RELAY) {	
			pline("There's a little badly burned moth in that relay!");
			makeknown(RELAY);
			if (Role_if(PM_GEEK) || Role_if(PM_GRADUATE) || Role_if(PM_CRACKER) || Role_if(PM_SOFTWARE_ENGINEER)) {
				You_feel("remembered of %s.",FunnyHallu ? "when the net was flat" : "the old times");
				break;
			}
		}	/* fall through */

		
	case DIODE:
	case TRANSISTOR:
	case IC:
		pline(FunnyHallu ? "Hmm... is this stuff edible?" : "You don't understand anything about electronics !!!");
		break;

	case MATERIAL_KIT:

		if (CannotSelectItemsInPrompts) return 0;

		pline("You may change the material of a base item type.");
materialchoice:
	    	{
			int materialkittype = 0;
			/* thwart those fucked-ass hangup cheater assholes --Amy */
			if (obj->oartifact == ART_MATERIAL_RAIN) materialkittype = 1;

			struct obj *otmpC = getobj(allowxall, "change the material of");
			if (!otmpC) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to change an item's material.");
				else goto materialchoice;
				return(0);
			}
			if ((otmpC->otyp == GOLD_PIECE) || (otmpC->otyp == STRANGE_OBJECT) || (otmpC->otyp == AMULET_OF_YENDOR) || (otmpC->otyp == CANDELABRUM_OF_INVOCATION) || (otmpC->otyp == BELL_OF_OPENING) || (otmpC->otyp == SPE_BOOK_OF_THE_DEAD) || (objects[otmpC->otyp].oc_prob < 1)) {
				pline("The material of that item cannot be changed!");
				return(1);
			} else {
				objects[otmpC->otyp].oc_material = (obj->shirtmessage % (LASTMATERIAL + 1));

				if (obj->oartifact == ART_NEW_OVERCAST) {
					obj->in_use = TRUE; /* fuck you for trying to hangup cheat. fuck you. --Amy */
					while (rn2(25)) {
						int objtomodify;
						objtomodify = -1;
	/* we roll until we get an item that can be randomly generated and is not a gold piece or other weird thing --Amy */
						while ( (objtomodify == -1) || (objtomodify == GOLD_PIECE) || (objtomodify == STRANGE_OBJECT) || (objtomodify == AMULET_OF_YENDOR) || (objtomodify == CANDELABRUM_OF_INVOCATION) || (objtomodify == BELL_OF_OPENING) || (objtomodify == SPE_BOOK_OF_THE_DEAD) || (objects[objtomodify].oc_prob < 1) ) objtomodify = rn2(NUM_OBJECTS);
						objects[objtomodify].oc_material = (obj->shirtmessage % (LASTMATERIAL + 1));
					}
					pline("The material has been applied to several random items.");
				}

				if (obj->oartifact == ART_GAROK_S_HAMMER_KIT && rn2(5)) {
					pline("You use some material from the kit.");
				} else if (obj->oartifact == ART_DICEROLLER_S_KIT) {
					pline("You use some material from the kit.");

					if (!rn2(5)) {
						if (obj->unpaid) {
							struct monst *shkp = shop_keeper(*u.ushops);
							if (shkp) {
								You("use it, you pay for it!");
								bill_dummy_object(obj);
							}
						}
						delobj(obj);
						noartispeak = TRUE;
					} else {
						obj->shirtmessage = rnd(1000000);
						pline("You use the material kit... and now it contains a different material.");
					}

				} else {
					if (obj->unpaid) {
						struct monst *shkp = shop_keeper(*u.ushops);
						if (shkp) {
							You("use it, you pay for it!");
							bill_dummy_object(obj);
						}
					}

					delobj(obj);

					noartispeak = TRUE;
				}

				pline("Success! The item's material got changed.");

				if (materialkittype == 1) {
					pline("Some items have had their base material changed.");
					randommaterials();
				}
			}

		}

		break;

	case SYMBIOTE:

		res = use_symbiote(obj);
		noartispeak = TRUE;
		break;

	case ACID_SYRINGE:
	{
		int syringetype = 0;
		/* fuck you for hangup cheating! either you win the game by playing it properly or not at all! play slashthem or dnethack if you want an easy game! --Amy */
		if (obj && obj->oartifact == ART_BLUBBLEGARBLE) syringetype = 1;
		if (obj && obj->oartifact == ART_DNN_DNN_DNN__DNDNDNDNNNNNN) syringetype = 2;

		noartispeak = TRUE;

		if (obj && obj->oartifact == ART_YO_WHAZZAH) {
			if (!rn2(5)) delobj(obj);
			else pline_The("syringe's contents aren't used up yet!");
		} else {
			delobj(obj);
		}

		if (syringetype == 1) {
			pline("Whoa! The colors, man!");
			make_hallucinated(HHallucination + rnz(500),FALSE,0L);
		}

		if (!Acid_resistance) {
			pline("Ugghh, your %s burns!", body_part(BLOOD));
			losehp(d(2, 2), "acid injection", KILLED_BY_AN);
		}

		if (syringetype == 2) {
			incr_itimeout(&Invulnerable, rn1(5,5));
			You_feel(FunnyHallu ? "like a super-duper hero!" : "invulnerable!");
		}

		if (Stoned) fix_petrification();
	}

		break;

	case INFUSION:
	{
		int infusiontype = 0;

		noartispeak = TRUE;

		/* dirty hangup cheater!!! proc useful effects AFTER the item is deleted --Amy */
		if (obj && obj->oartifact == ART_COVIDIVAC) infusiontype = 1;
		if (obj && obj->oartifact == ART_TURBOCURARINE_NEEDLE) infusiontype = 2;
		if (obj && obj->oartifact == ART_SOPORIL_SHOT) infusiontype = 3;

		delobj(obj);

		if (infusiontype == 1) {
			upnivel(TRUE);
		}

		if (infusiontype == 2) {
			pline("You're badly poisoned!");
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE, TRUE);
			statdrain(FALSE); /* can't resist */

			You_feel("your muscles relaxing.");
			You("gain paralysis resistance!");
			incr_itimeout(&HFree_action, rnz(10000));
			incr_itimeout(&HDiscount_action, rnz(10000));
		}

		if (infusiontype == 3) {
			You("fall asleep.");
			fall_asleep(-rnd(20), TRUE);
			more_experienced(u.ulevel * 5 * (deepest_lev_reached(FALSE) + 1), 0);
			newexplevel();
			upnivel(FALSE);
			if (!rn2(5)) {
				int i, ii, lim;
				i = rn2(A_MAX);
				for (ii = 0; ii < A_MAX; ii++) {
					lim = AMAX(i);
					if (ABASE(i) < lim) {
						ABASE(i) = lim;
						flags.botl = 1;
						break;
					}
					if(++i >= A_MAX) i = 0;
				}
			}
		}

		u.uhpmax++;
		if (Role_if(PM_BLEEDER)) u.uhpmax += 5;
		u.uenmax++;
		if (Upolyd) u.mhmax++;
		if (uinsymbiosis) {
			u.usymbiote.mhpmax++;
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			maybe_evolve_symbiote();
		}

		flags.botl = TRUE;
		You("inject the solution into your veins, and suddenly your %s burns and a new strength fills your body!", body_part(BLOOD));
	}

		break;

	case RAD_X:
	{
		int radxtype = 0;
		/* prevent filthy hangup cheating --Amy */
		if (obj && obj->oartifact == ART_RESIDUAL_BLOOD_EFFECT) radxtype = 1;
		if (obj && obj->oartifact == ART_THROUGH_THE_HEAT) radxtype = 2;

		int radxpower = rn1(500, 500);
		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_HEALING_SPELL)) {
				default: break;
				case P_BASIC: radxpower += 50; break;
				case P_SKILLED: radxpower += 100; break;
				case P_EXPERT: radxpower += 150; break;
				case P_MASTER: radxpower += 200; break;
				case P_GRAND_MASTER: radxpower += 250; break;
				case P_SUPREME_MASTER: radxpower += 300; break;
			}
		}

		if (obj && obj->oartifact == ART_RAIMUND_S_RARITY) radxpower *= 10;

		noartispeak = TRUE;

		delobj(obj);

		if (radxtype == 1) u.extracontres = TRUE;
		if (radxtype == 2) {
			incr_itimeout(&HFire_resistance, rnz(1000));
			incr_itimeout(&HStun_resist, rnz(1000));
			You_feel("capable of surviving the heat.");
		}

		incr_itimeout(&HCont_resist, radxpower);
		You_feel("more resistant to contamination.");

	}

		break;

	case RADAWAY:
	{
		int radawaytype = 0;
		/* prevent filthy hangup cheating --Amy */
		if (obj && obj->oartifact == ART_WRONG_CIGARETTE_BRAND) radawaytype = 1;

		int radawaypower = 100;
		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_HEALING_SPELL)) {
				default: break;
				case P_BASIC: radawaypower += 50; break;
				case P_SKILLED: radawaypower += 100; break;
				case P_EXPERT: radawaypower += 150; break;
				case P_MASTER: radawaypower += 200; break;
				case P_GRAND_MASTER: radawaypower += 250; break;
				case P_SUPREME_MASTER: radawaypower += 300; break;
			}
		}
		if (obj && obj->oartifact == ART_EMERGENCY_RADIATION_REMOVE) radawaypower *= 10;

		noartispeak = TRUE;

		delobj(obj);

		if (radawaytype == 1) {
			register struct obj *radcig;
			radcig = mksobj(CIGARETTE, TRUE, FALSE, FALSE);
			if (radcig) {
				(void) pickup_object(radcig, radcig->quan, TRUE, TRUE);

				if (uwep) setnotworn(uwep);
				if (bimanual(radcig)) {
					if (uswapwep) uswapwepgone();
					if (uarms) remove_worn_item(uarms, TRUE);
				}
				if (!uwep) setuwep(radcig, FALSE, TRUE);
				if (radcig) curse(radcig);
				You("suddenly have to smoke a stinking cigarette!");
			}
		}

		decontaminate(radawaypower);
		Your("contamination has been reduced.");

	}

		break;

	case CASINO_CHIP:
	{
		noartispeak = TRUE;

		int chiptype = 0;

		/* you can't trust players... the fact that someone hangup cheated in the past means anyone could try to do it again,
		 * and so we have to bulletproof the game as much as we can because we can't assume that they'll start playing by the rules --Amy */
		if (obj && obj->oartifact == ART_GAMBLEBOY) chiptype = 1;
		if (obj && obj->oartifact == ART_WELCOMING_GIFT) chiptype = 2;
		if (obj && obj->oartifact == ART_WOOZIE_S_FISH) chiptype = 3;
		if (obj && obj->oartifact == ART_MAIN_WIN) chiptype = 4;

		if (obj && obj->oartifact == ART_SINDACCO_S_FORGERY) {
			delobj(obj);
			u.copwantedlevel += rnz(1000);
			verbalize("WEOWEO");
			break;
		}

		delobj(obj);

		if (chiptype == 1) u.casinochips += 4;
		if (chiptype == 2) u.casinochips += 19;
		if (chiptype == 3) {
			struct monst *crpr = makemon(&mons[PM_CROUPIER], u.ux, u.uy, MM_ADJACENTOK);
			if (crpr) {
				crpr->mpeaceful = TRUE;
				crpr->mfrenzied = FALSE;
			}

		}
		if (chiptype == 4) {
			u.ugold += 2000;
		}

		u.casinochips++;
		You("add the chip to your collection. Your total amount of chips is %d now.", u.casinochips);
	}

		break;

	case BLESSER:
	{
		int blessertype = 0;
		if (obj->oartifact == ART_HOLYGE_AURA) blessertype = 1; /* because it's used up below */
		if (obj->oartifact == ART_MEH_ALTADOON) blessertype = 2; /* filthy hangup cheater!!! */

		noartispeak = TRUE;

		if (obj->oartifact == ART_WOLDERHOOH && rn2(5)) {
			pline("The special blesser isn't used up yet!");
		} else {
			delobj(obj);
		}

		if (blessertype == 1) {
			if (uarm && !uarm->cursed && !uarm->blessed) bless(uarm);
			if (uarmc && !uarmc->cursed && !uarmc->blessed) bless(uarmc);
			if (uarmf && !uarmf->cursed && !uarmf->blessed) bless(uarmf);
			if (uarmg && !uarmg->cursed && !uarmg->blessed) bless(uarmg);
			if (uarmh && !uarmh->cursed && !uarmh->blessed) bless(uarmh);
			if (uarmu && !uarmu->cursed && !uarmu->blessed) bless(uarmu);
			if (uarms && !uarms->cursed && !uarms->blessed) bless(uarms);
		}

		if (blessertype == 2) {
			NastinessProblem |= FROMOUTSIDE;
			FemaleTrapJulietta |= FROMOUTSIDE;
			You("learn the blessing technique!");
			learntech_or_leveltech(T_BLESSING, FROMOUTSIDE, 1);
		}

		pline("You may bless an uncursed item.");

blesschoice:
		{
			struct obj *otmpC = getobj(allowxall, "bless");

			if (!otmpC) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to bless an item.");
				else goto blesschoice;
				break;
			}
			if (otmpC->cursed || otmpC->blessed) {
				pline("The item didn't have the right BUC state and therefore nothing happened!");
				break;
			}
			if (!stack_too_big(otmpC)) {
				bless(otmpC);
				pline("Success! The item was blessed.");
				otmpC->bknown = TRUE;
			} else pline("The stack was so big that the blessing failed.");

		}

		break;

	}
	case BANDAGE:
	{
		int bandagetype = 0;
		if (obj->oartifact == ART_ROBIN_S_EMERGENCY) bandagetype = 1; /* because it's used up below */
		if (obj->oartifact == ART_HANDAID) bandagetype = 2; /* filthy hangup cheater!!! */
		if (obj->oartifact == ART_VIRUZIDE) bandagetype = 3;

		noartispeak = TRUE;

		delobj(obj);

		if (bandagetype == 2) {
			if (!(HProtection & INTRINSIC))  {
				HProtection |= FROMOUTSIDE;
				if (!u.ublessed)  u.ublessed = 1;
			} else {
				u.ublessed++;
			}
			You_feel("protected.");
		}

		if (bandagetype == 3) {
			upnivel(TRUE);
		}

		You("dress your wounds with the bandage.");
		if (bandagetype == 1) {
			healup(max(40, (u.uhpmax / 2)), 0, FALSE, FALSE);
		} else {
			healup(max(8, (u.uhpmax / 10)), 0, FALSE, FALSE);
		}

	}
		break;

	case BEAUTY_PACK:
	{
		int beautypacktype = 0;
		/* we absolutely don't want lame hangup cheating! --Amy */
		if (obj->oartifact == ART_PRIMA_DONNA) beautypacktype = 1;
		if (obj->oartifact == ART_IT_S_IN_THE_EYE_OF_THE_BEH) beautypacktype = 2;

		noartispeak = TRUE;

		delobj(obj);
		(void) adjattrib(A_CHA, 1, FALSE, TRUE);
		if (Race_if(PM_SUSTAINER) && ABASE(A_CHA) < AMAX(A_CHA)) {
			ABASE(A_CHA) += 1;
			AMAX(A_CHA) += 1;
			flags.botl = 1;
			pline("Your charisma increases.");
		}

		if (beautypacktype == 1) { /* because "helpfully" obj has been freed... --Amy */
			(void) adjattrib(A_CHA, 1, FALSE, TRUE);
			(void) adjattrib(A_CHA, 1, FALSE, TRUE);
			(void) adjattrib(A_CHA, 1, FALSE, TRUE);
			(void) adjattrib(A_CHA, 1, FALSE, TRUE);
			if (Race_if(PM_SUSTAINER) && ABASE(A_CHA) < AMAX(A_CHA)) {
				ABASE(A_CHA) += 1;
				AMAX(A_CHA) += 1;
				flags.botl = 1;
			}
			if (Race_if(PM_SUSTAINER) && ABASE(A_CHA) < AMAX(A_CHA)) {
				ABASE(A_CHA) += 1;
				AMAX(A_CHA) += 1;
				flags.botl = 1;
			}
			if (Race_if(PM_SUSTAINER) && ABASE(A_CHA) < AMAX(A_CHA)) {
				ABASE(A_CHA) += 1;
				AMAX(A_CHA) += 1;
				flags.botl = 1;
			}
			if (Race_if(PM_SUSTAINER) && ABASE(A_CHA) < AMAX(A_CHA)) {
				ABASE(A_CHA) += 1;
				AMAX(A_CHA) += 1;
				flags.botl = 1;
			}

		}
		if (beautypacktype == 2) {
			incr_itimeout(&HPolymorph_control, rnz(5000));
			You_feel("capable of controlling your polymorphs.");
		}

	}
		break;

	case INTELLIGENCE_PACK:
	{
		int intpacktype = 0;
		/* fuck off hangup cheater bastard, play some easymode variants if you want to cheat, slex is meant for those who want to play properly --Amy */
		if (obj->oartifact == ART_SUCK_THE_MIND_FLAYER) intpacktype = 1;
		if (obj->oartifact == ART_TWOHA) intpacktype = 2;

		if (obj->unpaid) {
			struct monst *shkp = shop_keeper(*u.ushops);
			if (shkp) {
				You("use it, you pay for it!");
				bill_dummy_object(obj);
			}
		}

		delobj(obj);
		noartispeak = TRUE;
		(void) adjattrib(A_INT, 1, FALSE, TRUE);
		if (Race_if(PM_SUSTAINER) && ABASE(A_WIS) < AMAX(A_WIS)) {
			ABASE(A_WIS) += 1;
			AMAX(A_WIS) += 1;
			flags.botl = 1;
			pline("Your wisdom increases.");
		}

		if (intpacktype == 2) {
			register struct obj *extratwohas;

			extratwohas = mksobj(randarttwohandedsword(), TRUE, FALSE, FALSE);
			if (extratwohas) {
				extratwohas = mk_artifact(extratwohas, (aligntyp)A_NONE, TRUE);
			}
			if (extratwohas) {
				dropy(extratwohas);
				pline("Check out the sword on the ground!");
			}
		}

		if (intpacktype == 1) { /* because "helpfully" obj has been freed... --Amy */
			(void) adjattrib(A_INT, 1, FALSE, TRUE);
			(void) adjattrib(A_INT, 1, FALSE, TRUE);
			(void) adjattrib(A_INT, 1, FALSE, TRUE);
			(void) adjattrib(A_INT, 1, FALSE, TRUE);
			if (Race_if(PM_SUSTAINER) && ABASE(A_WIS) < AMAX(A_WIS)) {
				ABASE(A_WIS) += 1;
				AMAX(A_WIS) += 1;
				flags.botl = 1;
			}
			if (Race_if(PM_SUSTAINER) && ABASE(A_WIS) < AMAX(A_WIS)) {
				ABASE(A_WIS) += 1;
				AMAX(A_WIS) += 1;
				flags.botl = 1;
			}
			if (Race_if(PM_SUSTAINER) && ABASE(A_WIS) < AMAX(A_WIS)) {
				ABASE(A_WIS) += 1;
				AMAX(A_WIS) += 1;
				flags.botl = 1;
			}
			if (Race_if(PM_SUSTAINER) && ABASE(A_WIS) < AMAX(A_WIS)) {
				ABASE(A_WIS) += 1;
				AMAX(A_WIS) += 1;
				flags.botl = 1;
			}

		}
	}

		break;

	case CHARGER:
	{
		int chargertype = 0;

		/* nope, you WILL NOT GET to abuse hangup cheats here --Amy */
		if (obj->oartifact == ART_AMMY_S_TEMPTATION) chargertype = 1;
		if (obj->oartifact == ART_OUTRADIATE) chargertype = 2;

		if (CannotSelectItemsInPrompts) return 0;

		if (obj->unpaid) {
			struct monst *shkp = shop_keeper(*u.ushops);
			if (shkp) {
				You("use it, you pay for it!");
				bill_dummy_object(obj);
			}
		}

		if (obj->oartifact == ART_MULTICHARGE && rn2(5)) {
			pline("You use one of the charges.");
		} else {
			delobj(obj);
			noartispeak = TRUE;
		}
		/* obj has been freed now unless it's the multicharge artifact and you hit the 80% chance */

		if (chargertype == 2) {
			decontaminate(1000);
		}

		pline("You may charge an object.");
chargingchoice:
	    	{
			struct obj *otmpC = getobj(recharge_type, "charge");
			if (!otmpC) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to charge your items.");
				else goto chargingchoice;
				return(0);
			}
			recharge(otmpC, (chargertype == 1) ? 1 : 0);
		}

		use_skill(P_DEVICES,10);
		if (Race_if(PM_FAWN)) {
			use_skill(P_DEVICES,10);
		}
		if (Race_if(PM_SATRE)) {
			use_skill(P_DEVICES,10);
			use_skill(P_DEVICES,10);
		}
		if (Role_if(PM_SPACEWARS_FIGHTER)) {
			use_skill(P_DEVICES,10);
		}
		if (Role_if(PM_CAMPERSTRIKER)) {
			use_skill(P_DEVICES,10);
			use_skill(P_DEVICES,10);
		}

		break;

	}
	case BITCHER:
	{
		int bitchertype = 0;
		/* prevent hangup cheating just in case someone tries to be a wise guy --Amy */
		if (obj->oartifact == ART_BITCHER____THE_WILD_CUNT) bitchertype = 1;

		You_hear("bitching noises.");
		pline("The bitcher dissolves in your hands...");

		if (obj->unpaid) {
			struct monst *shkp = shop_keeper(*u.ushops);
			if (shkp) {
				You("use it, you pay for it!");
				bill_dummy_object(obj);
			}
		}

		if (obj->cursed && rn2(2)) {

			delobj(obj);
			noartispeak = TRUE;
			pline("Somehow you feel that the bitches are making fun of you.");
			break; /* do not call delobj twice or the game will destabilize! */

		}

		delobj(obj);
		noartispeak = TRUE;

		use_skill(P_DEVICES,20);
		if (Race_if(PM_FAWN)) {
			use_skill(P_DEVICES,20);
		}
		if (Race_if(PM_SATRE)) {
			use_skill(P_DEVICES,20);
			use_skill(P_DEVICES,20);
		}
		if (Role_if(PM_SPACEWARS_FIGHTER)) {
			use_skill(P_DEVICES,20);
		}
		if (Role_if(PM_CAMPERSTRIKER)) {
			use_skill(P_DEVICES,20);
			use_skill(P_DEVICES,20);
		}

		cure_feminism_traps();

		if (bitchertype == 1) randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));

		break;
	}

	case SWITCHER:
	{
		int switchertype = 0;
		/* just in case someone tries to be super smart and hangup cheat... --Amy */
		if (obj->oartifact == ART_I_THE_SAGE) switchertype = 1;

		pline("You carefully pull the switch...");
		if (!Blind) pline("The red status light goes out while the green light starts shining brightly!");
		pline("The switcher dissolves in your hands...");

		if (obj->unpaid) {
			struct monst *shkp = shop_keeper(*u.ushops);
			if (shkp) {
				You("use it, you pay for it!");
				bill_dummy_object(obj);
			}
		}

		if (obj->cursed && !(obj->oartifact == ART_RED__GREEN_) && rn2(2)) {

			delobj(obj);
			noartispeak = TRUE;
			break; /* do not call delobj twice or the game will destabilize! */

		}

		delobj(obj);
		noartispeak = TRUE;

		if (switchertype == 1) {
		    (void) makemon(&mons[PM_GUNNHILD_S_GENERAL_STORE], 0, 0, NO_MM_FLAGS);
		}

		use_skill(P_DEVICES,20);
		if (Race_if(PM_FAWN)) {
			use_skill(P_DEVICES,20);
		}
		if (Race_if(PM_SATRE)) {
			use_skill(P_DEVICES,20);
			use_skill(P_DEVICES,20);
		}
		if (Role_if(PM_SPACEWARS_FIGHTER)) {
			use_skill(P_DEVICES,20);
		}
		if (Role_if(PM_CAMPERSTRIKER)) {
			use_skill(P_DEVICES,20);
			use_skill(P_DEVICES,20);
		}

		cure_nasty_traps();
	}

		break;
	case GOD_O_METER:

		if (!rn2(100)) {
		    useup(obj);
		    pline("Your god-o-meter explodes!");
			if (!ishaxor) u.ublesscnt += rn2(20);
			else u.ublesscnt += rn2(10);
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			noartispeak = TRUE;
			u.cnd_applycount++;
			return 0;
		}

		if (Blind) {
			pline("Being blind, you cannot see it.");
		} else if (!obj->blessed && (rn2(2) || obj->cursed) && !(obj->oartifact == ART_HOYO_HOYO_WOLOLO) ) {
			You_feel("uncomfortable.");
			if (!ishaxor) u.ublesscnt += rn2((obj->cursed) ? 200 : 100);
			else u.ublesscnt += rn2((obj->cursed) ? 100 : 50);
		} else {
			You("see a%s flash from the device.",(u.ublesscnt>0) ? " black" : "n amber");
			if (wizard || (!rn2(obj->blessed ? 3 : 10)) ) {
				Your("prayer timeout is %i.",u.ublesscnt);
			}
		}
		break;

	case PACK_OF_FLOPPIES:
		use_floppies(obj);
		break;

	case CHEMISTRY_SET:
		use_chemistry_set(obj);
		break;

	default:
		/* KMH, balance patch -- polearms can strike at a distance */

		if (is_applypole(obj)) {
			if (uwep && uwep == obj) res = use_pole(obj);
			else {pline("You must wield this item first if you want to apply it!"); 
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				wield_tool(obj, "swing"); }
			break;
		} else if (is_pick(obj) || is_axe(obj) || is_antibar(obj) ) {
			if (uwep && uwep == obj) res = use_pick_axe(obj);
			else {pline("You must wield this item first if you want to apply it!"); 
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				wield_tool(obj, "swing"); }
			break;
		}
		pline("Sorry, I don't know how to use that.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	xit:
		nomul(0, 0, FALSE);
		return 0;
	}

mushroompolecheck:

	u.cnd_applycount++;

	nomul(0, 0, FALSE);
	if (!obj) return res;
	if (noartispeak) return res;
	if (res && obj && obj->oartifact) arti_speak(obj);
	return res;
}

/* Keep track of unfixable troubles for purposes of messages saying you feel
 * great.
 */
int
unfixable_trouble_count(is_horn)
	boolean is_horn;
{
	int unfixable_trbl = 0;

	if (Stoned) unfixable_trbl++;
	if (Strangled) unfixable_trbl++;
	if (PlayerBleeds) unfixable_trbl++;
	if (Wounded_legs
		    && !u.usteed
				) unfixable_trbl++;
	if (Slimed) unfixable_trbl++;
	/* lycanthropy is not desirable, but it doesn't actually make you feel
	   bad */

	/* we'll assume that intrinsic stunning from being a bat/stalker
	   doesn't make you feel bad */
	if (!is_horn) {
	    if (Confusion) unfixable_trbl++;
	    if (Numbed) unfixable_trbl++;
	    if (Feared) unfixable_trbl++;
	    if (Frozen) unfixable_trbl++;
	    if (Burned) unfixable_trbl++;
	    if (Dimmed) unfixable_trbl++;
	    if (Sick) unfixable_trbl++;
	    if (HHallucination) unfixable_trbl++;
	    if (Vomiting) unfixable_trbl++;
	    if (HStun) unfixable_trbl++;
	}
	return unfixable_trbl;
}

#endif /* OVLB */

/*apply.c*/

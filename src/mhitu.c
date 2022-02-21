/*	SCCS Id: @(#)mhitu.c	3.4	2003/11/26	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "extern.h"
#include "artifact.h"
#include "edog.h"

STATIC_VAR NEARDATA struct obj *otmp;
STATIC_PTR int katicleaning(void);
STATIC_PTR int singcleaning(void);
static NEARDATA schar delay;            /* moves left for kati cleaning */

STATIC_DCL void urustm(struct monst *, struct obj *);
# ifdef OVL1
STATIC_DCL boolean u_slip_free(struct monst *,struct attack *);
STATIC_DCL int passiveum(struct permonst *,struct monst *,struct attack *);
# endif /* OVL1 */

#ifdef OVLB
STATIC_DCL void mayberem(struct obj *, const char *);
#endif /* OVLB */

STATIC_DCL boolean diseasemu(struct permonst *);
STATIC_DCL int hitmu(struct monst *,struct attack *);
STATIC_DCL int gulpmu(struct monst *,struct attack *);
STATIC_DCL int explmu(struct monst *,struct attack *,BOOLEAN_P);
STATIC_DCL void missmu(struct monst *,int,int,struct attack *);
STATIC_DCL void mswings(struct monst *,struct obj *);
STATIC_DCL void wildmiss(struct monst *,struct attack *);
STATIC_DCL boolean slextest(int, int);
STATIC_DCL void stdmsg(const char *);

/*STATIC_DCL void hurtarmor(int);*/
/*STATIC_DCL void witherarmor(void);*/
STATIC_DCL void hitmsg(struct monst *,struct attack *);

/* See comment in mhitm.c.  If we use this a lot it probably should be */
/* changed to a parameter to mhitu. */
static int dieroll;

boolean
player_shades_of_grey()
{
	if (!uarmg) return FALSE;
	if (uarmg && itemhasappearance(uarmg, APP_GREY_SHADED_GLOVES) ) return TRUE;
	if (RngeBDSM) return TRUE;
	return FALSE;

}


#ifdef OVL1

STATIC_OVL void
hitmsg(mtmp, mattk)
register struct monst *mtmp;
register struct attack *mattk;
{
	int compat;
	int monsterlev;
	int armproX = 0;
	int armprolimitX = 75;
	int randomkick;

	/* Note: if opposite gender, "seductively" */
	/* If same gender, "engagingly" for nymph, normal msg for others */
	if((compat = could_seduce(mtmp, &youmonst, mattk)) && !mtmp->mcan &&
	    !mtmp->mspec_used) {
	        pline("%s %s you %s.", Monnam(mtmp), Blind ? "talks to" :
	              "smiles at", compat == 2 ? "engagingly" :
	              "seductively");
	} /*else*/ switch (mattk->aatyp) {

	/* "Reduce chances of some special effects. These happened way too often.  It's not fun being level drained by a rat
on the first floor, especially when you're playing as something with drain resistance." Well they are already reduced (they used to happen EVERY time :P), but sure, in Soviet Russia they may be reduced by more. I'm not really happy about that but at least for all the other races the chances will stay the same. --Amy */

		case AT_CLAW:
			pline("%s claws you!", Monnam(mtmp));

			if (Role_if(PM_PROSTITUTE) && (multi < 0) && (mtmp->data->msound == MS_SQEEK || !rn2(3)) && !rn2(4)) {

				You_cant("move!");
				nomul(-2, "being clawed", TRUE);

			}

			if (Role_if(PM_PROSTITUTE) && (multi >= 0) && (mtmp->data->msound == MS_SQEEK || !rn2(3)) && !rn2(2)) {

				You_cant("move!");
				nomul(-2, "being clawed", TRUE);

			}

			if ((Role_if(PM_KURWA) || (uwep && uwep->oartifact == ART_DESERT_MAID) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_DESERT_MAID) || (uarmf && uarmf->oartifact == ART_RUEA_S_FAILED_CONVERSION) ) && (multi < 0) && (mtmp->data->msound == MS_SQEEK || !rn2(3)) && rn2(4)) {

				You_cant("move!");
				nomul(-2, "being clawed", TRUE);

			}

			if ((Role_if(PM_KURWA) || (uwep && uwep->oartifact == ART_DESERT_MAID) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_DESERT_MAID) || (uarmf && uarmf->oartifact == ART_RUEA_S_FAILED_CONVERSION) ) && (multi >= 0) && (mtmp->data->msound == MS_SQEEK || !rn2(3)) && rn2(10)) {

				You_cant("move!");
				nomul(-2, "being clawed", TRUE);

			}

			if (FemtrapActiveDora && uarmf && (mtmp->data->msound == MS_SQEEK)) {
				if (PlayerInHighHeels && !rn2(15)) {
					pline("The asshole claws damage your pretty high heels!");
					if (!rn2(2)) {
						if (uarmf->oeroded < MAX_ERODE) uarmf->oeroded++;
						else {
							useup(uarmf);
							pline("With a loud crack, your high heels shatter into a thousand fragments. Now you're really angry and bitter.");
							if (!u.berserktime) u.berserktime = 25;
						}
					} else {
						if (uarmf->oeroded2 < MAX_ERODE) uarmf->oeroded2++;
						else {
							useup(uarmf);
							pline("With a loud crack, your high heels shatter into a thousand fragments. Now you're really angry and bitter.");
							if (!u.berserktime) u.berserktime = 25;
						}
					}
				} else if (!PlayerInHighHeels && !rn2(100)) {
					pline("The claws damage your footwear!");
					if (!rn2(2)) {
						if (uarmf->oeroded < MAX_ERODE) uarmf->oeroded++;
						else {
							useup(uarmf);
							pline("Now your shoes are broken. Great.");
						}
					} else {
						if (uarmf->oeroded2 < MAX_ERODE) uarmf->oeroded2++;
						else {
							useup(uarmf);
							pline("Now your shoes are broken. Great.");
						}
					}
				}
			}

			if ((flags.female && !(uwep && uwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY) && (!issoviet || !rn2(5)) && !rn2(player_shades_of_grey() ? 3 : (u.ualign.type == A_LAWFUL) ? 50 : (u.ualign.type == A_NEUTRAL) ? 30 : 10)) || (uarmf && itemhasappearance(uarmf, APP_FETISH_HEELS)) ) { 

				if (uarmf && uarmf->oartifact == ART_HUGGING__GROPING_AND_STROK) {
					pline("%s gently strokes and squeezes your breasts, and you are sexually aroused by the soft touch.", Monnam(mtmp));
					u.cnd_breastripamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					healup(mtmp->m_lev, 0, FALSE, FALSE);

				} else {
					pline("%s rips into your breast with maniacal fervor!", Monnam(mtmp));
					u.cnd_breastripamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

					monsterlev = ((mtmp->m_lev) + 1);
					if (monsterlev <= 0) monsterlev = 1;

					losehp(d(2,monsterlev), "cowardly attack on her breasts", KILLED_BY_AN);
				}

			}

			if ((uarmf && (itemhasappearance(uarmf, APP_VELCRO_SANDALS) || itemhasappearance(uarmf, APP_NOBLE_SANDALS))) && !rn2(3)) {

				pline("%s claws sensitive parts of your body!", Monnam(mtmp));

				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

				losehp(d(2,monsterlev), "sensitive clawing attack", KILLED_BY_AN);

			}

			if (uwep && uwep->oartifact == ART_GIRLFUL_BONKING) {
				pline("Ouch, it hurts your soft skin!");
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				losehp(d(2,monsterlev), "evil claw attack", KILLED_BY_AN);
			}

			if (u.twoweap && uswapwep && uswapwep->oartifact == ART_GIRLFUL_BONKING) {
				pline("Ouch, it hurts your soft skin!");
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				losehp(d(2,monsterlev), "evil claw attack", KILLED_BY_AN);
			}

			break;
		case AT_SPIT:  
			pline("%s spits on you!", Monnam(mtmp));  
			break;  
		case AT_BEAM:  
			pline("%s blasts you!", Monnam(mtmp));  
			break;  
		case AT_BREA:  
			pline("%s breathes on you!", Monnam(mtmp));  
			break;  
		case AT_BITE:
			pline("%s bites you!", Monnam(mtmp));
			armproX = magic_negation(&youmonst);

			if (!(PlayerCannotUseSkills)) {

				switch (P_SKILL(P_SPIRITUALITY)) {
					default: armprolimitX = 75; break;
					case P_BASIC: armprolimitX = 78; break;
					case P_SKILLED: armprolimitX = 81; break;
					case P_EXPERT: armprolimitX = 84; break;
					case P_MASTER: armprolimitX = 87; break;
					case P_GRAND_MASTER: armprolimitX = 90; break;
					case P_SUPREME_MASTER: armprolimitX = 93; break;
				}
			}

			if (rn2(25) && moves < 1000) break; /* players are getting killed unfairly... --Amy */

			if (!rn2(player_shades_of_grey() ? 50 : (u.ualign.type == A_LAWFUL) ? 500 : (u.ualign.type == A_NEUTRAL) ? 750 : 1000) && (!issoviet || !rn2(5)) && ((rn2(3) >= armproX) || ((rnd(100) > armprolimitX) && ((armproX < 4) || (rnd(armproX) < 4) ) ) ) ) {
			if (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) {
			pline("%s sinks %s teeth deep into your skin and drinks your %s!", Monnam(mtmp), mhis(mtmp), body_part(BLOOD));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha kha kha, eto ne byl otklyuchen, i teper' vy osushilos'. Sovetskaya Pyat' Lo ne khochet, chtoby eta igra byla vyigrana v lyubom sluchaye." : "SCHHUEUEOEUEOEUEOEUEOE");
		      losexp("life drainage", FALSE, TRUE);
			}
			}
			break;
		case AT_KICK:
			pline("%s kicks you%c", Monnam(mtmp),
				    thick_skinned(youmonst.data) ? '.' : (uwep && uwep->oartifact == ART_ETRUSCIAN_SWIMMING_LESSON) ? '.' : (uarmf && uarmf->oartifact == ART_ANTJE_S_POWERSTRIDE) ? '.' : (uarmf && uarmf->oartifact == ART_THICK_FARTING_GIRL) ? '.' : Race_if(PM_DUTHOL) ? '.' : (uwep && uwep->oartifact == ART_PATRICIA_S_FEMININITY) ? '.' : '!');

			if (humanoid(mtmp->data) && is_female(mtmp->data) && FemtrapActiveJeanetta) {
				pline("%s uses her cute little boots to scrape a bit of skin off your %s!", Monnam(mtmp), body_part(LEG));

		/* this can now increase the player's legscratching variable. Since the damage you take depends on how much
		 * legscratching you already have, and you might get hit by a long-lasting effect of this trap, we need to
		 * make absolutely sure that the increases don't happen too quickly or it becomes unplayable; this is achieved
		 * by having only 1 in (legscratching squared) chance for an increase --Amy */

				if (!u.legscratching) u.legscratching++;
				else if (u.legscratching > 60000) {
					if (!rn2(60000) && !rn2(60000)) u.legscratching++;
				} else if (!rn2(u.legscratching * u.legscratching)) u.legscratching++;

				losehp(rno(u.legscratching + 1), "being scratched by Jeanetta's little boots", KILLED_BY);
			}

			if (Role_if(PM_SHOE_FETISHIST) && mtmp->data->msound == MS_SHOE && !rn2(5)) {
				switch (rnd(7)) {
					case 1:
						pline_The("shoe clamps your arm!");
						make_numbed(HNumbed + rnz(150),FALSE);
						break;
					case 2:
						pline_The("kick hits your %s!", body_part(HAND));
						incr_itimeout(&Glib, rnz(50) );
						break;
					case 3:
						pline_The("kick bludgeoned you, and you drop to the floor!");
						nomul(-rnd(10), "knocked out by a lovely shoe", TRUE);
						nomovemsg = "You stand back up.";
						break;
					case 4:
						You("start to really fear the dangerous shoe because you're such a little wimp.");
						make_feared(HFeared + rnz(150),FALSE);
						break;
					case 5:
						pline("Ouch, that kick really hurt!");
						losehp(1 + u.chokhmahdamage + rnd(u.ualign.sins > 0 ? (isqrt(u.ualign.sins) + 1) : (1)),"escalating damage effect",KILLED_BY_AN);
						u.chokhmahdamage++;
						break;
					case 6:
						pline("You're shaken, and start trembling. Dude you're really a wussie, reacting like that just because a soft shoe kicked you.");
						u.tremblingamount++;
						break;
					case 7:
						pline("Suddenly the lovely shoe scratches along the whole length of your %s, and your %s squirts in all directions.", body_part(LEG), body_part(BLOOD));
						playerbleed(rnd(2 + (level_difficulty() * 10)));
						break;
				}
			}

			if (FemtrapActiveKati && humanoid(mtmp->data) && is_female(mtmp->data)) {

				pline("%s painfully kicks you in the %s with her sexy Kati shoes!", mtmp->data->mname, makeplural(body_part(LEG)));
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				losehp((monsterlev), "being kicked by Kati shoes", KILLED_BY);

			}

			if (FemtrapActiveLou && !rn2(10)) {
				pline("Eww, %s's dirty footwear brushed your clothing!", mon_nam(mtmp));
				register struct obj *objX, *objX2;
				for (objX = invent; objX; objX = objX2) {
					objX2 = objX->nobj;
					if (!rn2(20)) wither_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
				}
			}

			if (FemtrapActiveJuen && humanoid(mtmp->data) && is_female(mtmp->data) && (multi < 0) && !rn2(3)) {

				pline("Ouch! You're in pain!");
				if (uarmf && uarmf->oartifact == ART_SHIN_KICKING_GAME) nomul(-3, "having aching legs", TRUE);
				else nomul(-2, "having aching legs", TRUE);

			}

			if (FemtrapActiveJuen && humanoid(mtmp->data) && is_female(mtmp->data) && (multi >= 0) && rn2(2)) {

				pline("Ouch! You're in pain!");
				if (uarmf && uarmf->oartifact == ART_SHIN_KICKING_GAME) nomul(-3, "having aching legs", TRUE);
				else nomul(-2, "having aching legs", TRUE);

			}

			if (uarmf && !rn2(3) && itemhasappearance(uarmf, APP_PLOF_HEELS) ) {
				pline("*plof*");
				if (uarmf->spe > ((uarmf->oartifact == ART_STEFANJE_S_PROBLEM) ? -50 : -21)) uarmf->spe--;
				else {
					useup(uarmf);
					pline("Your plof heels are destroyed by the filthy %s.", l_monnam(mtmp));
				}
			}

			if (uarmf && uarmf->oartifact == ART_STEFANJE_S_PROBLEM) {
				pline("Oh no, the kick fully hit your heels and you're becoming unsteady!");
				make_stunned(HStun + rnz(20), FALSE);
				if (!rn2(20)) badeffect();
			}

			if ( ((!flags.female && !(uarmu && uarmu->oartifact == ART_LUISA_S_FELLOW_FEELING) && !(uwep && uwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY)) || FemtrapActiveKarin) && (!issoviet || !rn2(5)) && !rn2(Role_if(PM_PROSTITUTE) ? 1 : Role_if(PM_KURWA) ? 1 : player_shades_of_grey() ? 3 : (u.ualign.type == A_LAWFUL) ? 50 : (u.ualign.type == A_NEUTRAL) ? 30 : 10) ) {

				if (uarmf && uarmf->oartifact == ART_HUGGING__GROPING_AND_STROK) {
					pline("%s powerfully kicks you in the nuts, and you moan in lust because you love the pain.", Monnam(mtmp));
					u.cnd_nutkickamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					healup(mtmp->m_lev, 0, FALSE, FALSE);

				} else {

					pline("%s's kick painfully slams into your nuts!", Monnam(mtmp));
					u.cnd_nutkickamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

					monsterlev = ((mtmp->m_lev) + 1);
					if (monsterlev <= 0) monsterlev = 1;

					losehp(d(2,monsterlev), "kick in the nuts", KILLED_BY_AN);

					if (FemtrapActiveKarin && humanoid(mtmp->data) && is_female(mtmp->data) && rn2(3)) {
						int karincount = 0;
karinrepeat:
						if (karincount < (4 + rn2(7)) ) {
							pline("%s quickly knees you in the nuts again!", Monnam(mtmp));
							u.cnd_nutkickamount++;
							if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
							monsterlev = ((mtmp->m_lev) + 1);
							if (monsterlev <= 0) monsterlev = 1;
							losehp(d(2,monsterlev), "getting his nuts squashed by a female knee", KILLED_BY);
							if (!rn2(20)) verbalize("Do you like my sexy knee?");
						} else {
							pline("%s massages your aching nuts with her fleecy hands!", Monnam(mtmp));
							healup(mtmp->m_lev, 0, FALSE, FALSE);
							if (!rn2(20)) verbalize("I'm sorry for hurting you so much. Here, does that make it feel better?");
						}
						karincount++;
						if (karincount > (6 + rn3(7)) ) karincount = 0;
						if (rn2(3)) goto karinrepeat;
					}

				}

			}

			struct obj *footwear = which_armor(mtmp, W_ARMF);

			if (uarmf && itemhasappearance(uarmf, APP_BUSINESS_SHOES) && humanoid(mtmp->data) && is_female(mtmp->data) && footwear && ishighheeled(footwear)) {
				pline("%s scratches your butt-ugly business shoes with her high heels.", Monnam(mtmp));
				if (FunnyHallu) pline("Serves you right for wearing them, you should don some good-looking pair instead.");
				if (uarmf) wither_obj(uarmf, TRUE, FALSE);

			}

			int randomsexyheels = 0;
			if (uarmf && uarmf->oartifact == ART_SYSTEM_OF_SEXUAL_PLEASURE && humanoid(mtmp->data) && is_female(mtmp->data)) randomsexyheels = rnd(29);
			if (FemtrapActiveKristin && !rn2(10) && humanoid(mtmp->data) && is_female(mtmp->data)) randomsexyheels = rnd(29);

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == WEDGE_SANDALS) || (footwear && footwear->otyp == EVELINE_WEDGE_SANDALS) || (footwear && footwear->otyp == NATALIA_WEDGE_SANDALS) || (footwear && footwear->otyp == GUDRUN_WEDGE_SANDALS) || (footwear && footwear->otyp == ELLA_WEDGE_SANDALS) || (footwear && footwear->otyp == KERSTIN_WOODEN_SANDALS) || (footwear && footwear->otyp == ANNEMARIE_PLATFORM_SANDALS) || (footwear && footwear->otyp == KARIN_LADY_SANDALS) || (footwear && footwear->otyp == CLAUDIA_WOODEN_SANDALS) || (footwear && itemhasappearance(footwear, APP_CALF_LEATHER_SANDALS)) || (footwear && itemhasappearance(footwear, APP_WOODEN_CLOGS)) || (footwear && itemhasappearance(footwear, APP_PRINTED_WEDGES)) || mtmp->data == &mons[PM_ANIMATED_WEDGE_SANDAL] || (randomsexyheels == 1) || mtmp->data == &mons[PM_WEREWEDGESANDAL] || mtmp->data == &mons[PM_HUMAN_WEREWEDGESANDAL]) ) {
elenaWDG:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s sandals against your shins! It hurts!", Monnam(mtmp), mhis(mtmp) );
				losehp(d(1,monsterlev), "sandal to the shin bone", KILLED_BY_AN);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elenaWDG;
				}
			}


			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == ATSUZOKO_BOOTS) || (footwear && footwear->otyp == BUM_BUM_BOOTS) || (footwear && footwear->otyp == KATHARINA_PLATFORM_BOOTS) || (footwear && footwear->otyp == LARISSA_PLATFORM_BOOTS) || (footwear && footwear->otyp == MADELEINE_PLATEAU_BOOTS) || (footwear && footwear->otyp == ANTJE_COMBAT_BOOTS) || (footwear && footwear->otyp == SING_PLATFORM_BOOTS) || (footwear && itemhasappearance(footwear, APP_BUFFALO_BOOTS)) || (footwear && itemhasappearance(footwear, APP_CHELSEA_BOOTS)) || (footwear && itemhasappearance(footwear, APP_PLATEAU_BOOTS)) || mtmp->data == &mons[PM_ANIMATED_ATSUZOKO_BOOT] || mtmp->data == &mons[PM_PLATEAU_GIRL] || mtmp->data == &mons[PM_VERY_THICK_GIRL] || mtmp->data == &mons[PM_KATI_S_PLATEAU_BOOT] || mtmp->data == &mons[PM_VAMPIRIC_FEMMY] || mtmp->data == &mons[PM_VERENA_S_PLATEAU_BOOT] || mtmp->data == &mons[PM_PLATFORMINA_S_THICK_WOMENS_SHOE] || mtmp->data == &mons[PM_SUPER_STRONG_GIRL] || (randomsexyheels == 2) || mtmp->data == &mons[PM_BUFFALO_HC_GIRL] || mtmp->data == &mons[PM_SUE_LYN_S_PLATEAU_BOOT]) ) {
elena1:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s plateau soles against your shins, and stomps your %s!", Monnam(mtmp), mhis(mtmp), makeplural(body_part(FOOT)) );
				losehp(d(1,monsterlev), "being kicked by an atsuzoko boot", KILLED_BY);
				make_stunned(HStun + (monsterlev * rnd(3)), FALSE);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena1;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == HIGH_STILETTOS) || (footwear && itemhasappearance(footwear, APP_IMAGINARY_HEELS)) || (footwear && itemhasappearance(footwear, APP_ANKLE_STRAP_SANDALS)) || (footwear && footwear->otyp == VERENA_STILETTO_SANDALS) || (footwear && footwear->otyp == NATALJE_BLOCK_HEEL_SANDALS) || (footwear && itemhasappearance(footwear, APP_NOBLE_SANDALS)) || mtmp->data == &mons[PM_ANIMATED_STILETTO_SANDAL] || mtmp->data == &mons[PM_WERESTILETTOSANDAL] || mtmp->data == &mons[PM_HUMAN_WERESTILETTOSANDAL] || (randomsexyheels == 3) || mtmp->data == &mons[PM_SUPER_STRONG_GIRL] || mtmp->data == &mons[PM_SANDRA_S_EVIL_SANDAL] || mtmp->data == &mons[PM_SANDRA_S_MINDDRILL_SANDAL] || mtmp->data == &mons[PM_NADINE_S_ANKLE_STRAP_SANDAL]) ) {
elena2:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(10);
				pline("%s slams against your shins with %s sandals, and also scratches over the full length of your %s with %s heel!", Monnam(mtmp), mhis(mtmp), body_part(LEG), mhis(mtmp) );

				if (!rn2(5)) {
				if (Upolyd) {u.mhmax--; if (u.mh > u.mhmax) u.mh = u.mhmax;}
				else {u.uhpmax--; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }
				pline("You're bleeding!");
				}

				losehp(d(1,monsterlev), "an unyielding stiletto sandal", KILLED_BY);
				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, HWounded_legs + monsterlev);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena2;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == UNFAIR_STILETTOS) || (footwear && itemhasappearance(footwear, APP_FAILED_SHOES)) || (footwear && itemhasappearance(footwear, APP_BAYONET_HEELS)) || (randomsexyheels == 4) || mtmp->data == &mons[PM_ANIMATED_UNFAIR_STILETTO] || mtmp->data == &mons[PM_WEREUNFAIRSTILETTO] || mtmp->data == &mons[PM_BERBAL] || mtmp->data == &mons[PM_NUTCRUNCHER_BITCH] || mtmp->data == &mons[PM_HUMAN_WEREUNFAIRSTILETTO]) ) {
elena3:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				pline("%s uses %s stilettos in a very unfair way!", Monnam(mtmp), mhis(mtmp) );
				if (!rn2(3)) pline("Your %s are scratched to the bone and your %s is squirting everywhere!", makeplural(body_part(LEG)), body_part(BLOOD) );
				else if (!rn2(2)) pline("Your sensitive sexual organs are squeezed flat and you're gasping for air!");
				else You_feel("a vicious impact as the hard heel slams on your %s with full force, and you're seeing asterisks everywhere.", body_part(HEAD) );
				badeffect();
				if (!rn2(2)) badeffect();
				if (!rn2(3)) badeffect();
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena3;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == SKY_HIGH_HEELS) || (randomsexyheels == 5) || mtmp->data == &mons[PM_ANIMATED_SKY_HIGH_HEEL]) ) {
elena4:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				pline("%s kicks you in the %s with %s very high heel!", Monnam(mtmp), body_part(HEAD), mhis(mtmp) );
				monsterlev = ((mtmp->m_lev) + 1);

				if (!rn2(3) && !(Role_if(PM_COURIER)) ) {
					if (!Blind) pline("Everything suddenly goes dark.");
					make_blinded(Blinded+monsterlev,FALSE);
					if (!Blind) Your("%s", vision_clears);
				}
				if (!rn2(3) && !(Role_if(PM_COURIER)) ) make_stunned(HStun + monsterlev, TRUE);
				if (!rn2(3) && !(Role_if(PM_COURIER)) ) pushplayer(FALSE);
				if (Role_if(PM_COURIER)) pline("But since you don't actually have a head, you're unharmed.");
				else losehp(monsterlev, "sky-high heel", KILLED_BY_AN);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena4;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == SYNTHETIC_SANDALS) || (footwear && itemhasappearance(footwear, APP_PLOF_HEELS)) || (footwear && itemhasappearance(footwear, APP_RADIANT_HEELS)) || (randomsexyheels == 6) || mtmp->data == &mons[PM_ANIMATED_SYNTHETIC_SANDAL]) ) {
elena5:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);

				pline("The wonderful rainbow-colored female sandal slams against your shins!");

				switch (rnd(29)) {
					case 1:
						pline("Wow!");
						break;
					case 2:
						pline("Lovely!");
						break;
					case 3:
						pline("What fleecy colors!");
						break;
					case 4:
						pline("So feminine...");
						break;
					case 5:
						pline("<3");
						break;
					case 6:
						pline("It's painful!");
						break;
					case 7:
						pline("Soothing!");
						break;
					case 8:
						pline("Incredible!");
						break;
					case 9:
						pline("You really like the painful sensation!");
						break;
					case 10:
						pline("They're asian, too!");
						break;
					case 11:
						pline("You think you're in love!");
						break;
					case 12:
						pline("Such a lovely, fleecy pair of sandals!");
						break;
					case 13:
						pline("Sweetheart bundle!");
						break;
					case 14:
						pline("The pain is wonderful!");
						break;
					case 15:
						pline("You're about to get an orgasm!");
						break;
					case 16:
						pline("It's pure lust!");
						break;
					case 17:
						pline("The Amy is envious of you!");
						break;
					case 18:
						pline("You want to wear them too!");
						break;
					case 19:
						pline("You need those beautiful sandals for yourself!");
						break;
					case 20:
						pline("Scintillating rainbows!");
						break;
					case 21:
						pline("Glinting set off by rainbow fluttering fairily in the sky.");
						break;
					case 22:
						pline("Heavenly!");
						break;
					case 23:
						pline("Your sweet %s would like them...", makeplural(body_part(FOOT)));
						break;
					case 24:
						pline("This kick felt wonderful!");
						break;
					case 25:
						pline("You're about to come!");
						break;
					case 26:
						pline("Your sexual organs are getting all wet...");
						break;
					case 27:
						pline("It's like a sign of true love!");
						break;
					case 28:
						pline("Beautifully soft leather... oh wait, they're made of synthetic material!");
						break;
					case 29:
						pline("Red, yellow, green, blue, pink and purple...");
						break;
				}

				randomnastytrapeffect(monsterlev, 1000);

				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena5;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == ROLLER_BLADE) || (footwear && itemhasappearance(footwear, APP_ROLLER_BOOTS) ) || (randomsexyheels == 7) || mtmp->data == &mons[PM_ANIMATED_ROLLER_BLADE] || mtmp->data == &mons[PM_HUGGING_BOOT_GIRL_WITH_A_BMW] || mtmp->data == &mons[PM_JUEN_S_ROLLER_BLADE]) ) {
elena6:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s rolls over you with %s roller blades!", Monnam(mtmp), mhis(mtmp) );
				badeffect();
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena6;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == PET_STOMPING_PLATFORM_BOOTS) || (footwear && itemhasappearance(footwear, APP_PLATFORM_BOOTS) ) || (randomsexyheels == 8) ) ) {
elena7:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(5);
				pline("%s smashes your %s with %s platform boots!", Monnam(mtmp), makeplural(body_part(TOE)), mhis(mtmp) );
				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, HWounded_legs + monsterlev);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena7;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == ASS_KICKER_BOOTS)) || (randomsexyheels == 9) || mtmp->data == &mons[PM_SUPER_STRONG_GIRL] || mtmp->data == &mons[PM_FOURFOLD_SHOE_ENEMY] || mtmp->data == &mons[PM_DANIELLE_S_COMBAT_BOOT] ) {
elena8:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(10);
				pline("%s kicks you in the ass!", Monnam(mtmp) );
				morehungry(monsterlev);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena8;
				}

			}


			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == DANCING_SHOES) || (footwear && footwear->otyp == ANASTASIA_DANCING_SHOES) || (randomsexyheels == 10) ) ) {
elena9:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s dancing shoes against your shins! You stagger...", Monnam(mtmp), mhis(mtmp) );
				make_stunned(HStun + monsterlev, FALSE);
				losehp(1, "soft dancing shoe", KILLED_BY_AN);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena9;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == SWEET_MOCASSINS) || (footwear && footwear->otyp == BRIGHT_CYAN_BEAUTIES) || (footwear && footwear->otyp == JESSICA_LADY_SHOES) || (footwear && footwear->otyp == SOLVEJG_MOCASSINS) || (footwear && itemhasappearance(footwear, APP_MARJI_SHOES)) || (randomsexyheels == 11) ) ) {
elena10:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slides %s mocassins over your %ss, opening your arteries and squirting %s everywhere!", Monnam(mtmp), mhis(mtmp), body_part(HAND), body_part(BLOOD) );
				    incr_itimeout(&Glib, monsterlev);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena10;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == SOFT_SNEAKERS) || (footwear && footwear->otyp == MARIKE_SNEAKERS) || (footwear && footwear->otyp == LOU_SNEAKERS) || (footwear && footwear->otyp == JANA_SNEAKERS) || (footwear && footwear->otyp == ALMUT_SNEAKERS) || (footwear && footwear->otyp == YVONNE_GIRL_SNEAKERS) || (footwear && footwear->otyp == ELIF_SNEAKERS) || (footwear && footwear->otyp == KERSTIN_SNEAKERS) || (footwear && footwear->otyp == ANTJE_SNEAKERS) || (footwear && itemhasappearance(footwear, APP_BLUE_SNEAKERS)) || (footwear && itemhasappearance(footwear, APP_PLATFORM_SNEAKERS)) || (footwear && itemhasappearance(footwear, APP_RED_SNEAKERS)) || (footwear && itemhasappearance(footwear, APP_PINK_SNEAKERS)) || (footwear && itemhasappearance(footwear, APP_REGULAR_SNEAKERS)) || (footwear && itemhasappearance(footwear, APP_EXCEPTIONAL_SNEAKERS)) || (footwear && itemhasappearance(footwear, APP_ELITE_SNEAKERS)) || (footwear && itemhasappearance(footwear, APP_CYAN_SNEAKERS)) || (footwear && itemhasappearance(footwear, APP_GENTLE_SNEAKERS)) || (footwear && itemhasappearance(footwear, APP_YELLOW_SNEAKERS)) || mtmp->data == &mons[PM_SUPER_STRONG_GIRL] || (randomsexyheels == 12) || mtmp->data == &mons[PM_GIRL_WHO_LOOKS_LIKE_HER_NAME_WAS_FANNY] || mtmp->data == &mons[PM_RITA_S_HIGH_HEELED_SNEAKER]) ) {
elena11:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s hits your %s with %s sneakers!", Monnam(mtmp), body_part(HAND), mhis(mtmp) );
				    incr_itimeout(&Glib, 2); /* just enough to make you drop your weapon */
				losehp(d(1,monsterlev), "soft sneaker to the hand", KILLED_BY_AN);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena11;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == HIPPIE_HEELS) || (footwear && itemhasappearance(footwear, APP_LOLITA_BOOTS)) || (footwear && itemhasappearance(footwear, APP_EROTIC_BOOTS)) || (footwear && itemhasappearance(footwear, APP_SPUTA_BOOTS)) || mtmp->data == &mons[PM_ANIMATED_PROSTITUTE_SHOE] || (randomsexyheels == 13) || mtmp->data == &mons[PM_WEREPROSTITUTESHOE] || mtmp->data == &mons[PM_HUMAN_WEREPROSTITUTESHOE] || mtmp->data == &mons[PM_SPIDER_FAGUS] || mtmp->data == &mons[PM_LUISA_S_SPUTA_FLOWING_BLOCK_HEEL_BOOT]) ) {
elena12:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				randomkick = rnd(15);
				if (randomkick == 1) {
				pline("%s steps on your %s with %s plateau heels!", Monnam(mtmp), body_part(HAND), mhis(mtmp) );
				    incr_itimeout(&Glib, 20); /* painfully jamming your fingers */
				losehp(d(1,monsterlev), "hippie plateau boot", KILLED_BY_AN);
				}
				if (randomkick == 2) {
				pline("%s jams your %ss with %s red whore boots!", Monnam(mtmp), body_part(TOE), mhis(mtmp) );

				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
			    exercise(A_STR, FALSE);
			    exercise(A_DEX, FALSE);

				losehp(d(1,monsterlev), "red whore boot", KILLED_BY_AN);
				}

				if (randomkick == 3) {
					if (!flags.female) { 
					pline("%s kicks right into your nuts with %s prostitute shoes!", Monnam(mtmp), mhis(mtmp) );

					losehp(d(2,monsterlev), "prostitute shoe to the nuts", KILLED_BY_AN);
					}

					else {
					pline("%s gently strokes you with %s prostitute shoes.", Monnam(mtmp), mhis(mtmp) );
					if (Upolyd) u.mh += monsterlev; /* heal some hit points */
					else u.uhp += monsterlev; /* heal some hit points */
					pline("It feels very soothing.");
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					}

				}

				if (randomkick == 4) {
				pline("%s painfully stomps your body with %s erotic lady boots!", Monnam(mtmp), mhis(mtmp) );
				u.uhp -= 1;
				u.uhpmax -= 1;
				u.uen -= 1;
				u.uenmax -= 1;
				losehp(d(3,monsterlev), "being stomped by an erotic lady boot", KILLED_BY);
				}

				if (randomkick == 5) {
				pline("You fall in love with %s's incredibly erotic footwear, and are unable to fight back.", mon_nam(mtmp) );
					nomovemsg = "You finally decide to stop admiring the sexy leather boots.";
					nomul(-rnd(5), "mesmerized by a pair of sexy leather boots", TRUE);

				}

				if (randomkick == 6) {
				pline("Think of the sweet red leather your sputa will flow down.");
				morehungry(1000);
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
				}

				if (randomkick == 7) {
				pline("You decide to close your %ss for a while, thinking about the lovely %s and %s sexy red overknees.", body_part(EYE), m_monnam(mtmp), mhis(mtmp) );
		    make_blinded(Blinded+monsterlev,FALSE);
				}

				if (randomkick == 8) {
				pline("Wow... those wonderful high heels are soooooo mesmerizing and beautiful...");
		make_hallucinated(HHallucination + monsterlev,FALSE,0L);
				}

				if (randomkick == 9) {
				pline("You wonder where %s got %s plateau heels from. Who the hell wears those in a dungeon, anyway? Your %s spins in bewilderment.", mon_nam(mtmp), mhis(mtmp), body_part(HEAD));
		    make_confused(HConfusion+monsterlev,FALSE);
				}

				if (randomkick == 10) {
				pline("Argh! The massive heel strained a vital muscle!");
		    make_stunned(HStun+monsterlev,FALSE);
				}

				if (randomkick == 11) {
				if (!rn2(25)) {
					pline("Fuck! The high heel was contaminated with spores!" );
					    make_sick(rn1(25,25), "contaminated high heel", TRUE, SICK_VOMITABLE);
					losehp(d(1,monsterlev), "unclean high heel", KILLED_BY_AN);
					}
				else {
				pline("The devious %s rubbed %s female boots with venom! How unfair!", m_monnam(mtmp), mhis(mtmp) );
						poisoned("block heel", A_STR, "poisoned block heel", 8);
					losehp(d(1,monsterlev), "impregnated block heel", KILLED_BY_AN);
					}
				}

				if (randomkick == 12) {
				pline("Ow, %s is stomping you repeatedly with %s slutty boots!", m_monnam(mtmp), mhis(mtmp));
		    make_numbed(HNumbed+monsterlev,FALSE);
				}

				if (randomkick == 13) {
				pline("The fact that %s is wearing such incredibly high heels is giving you the creeps.", m_monnam(mtmp));
		    make_frozen(HFrozen+monsterlev,FALSE);
				}

				if (randomkick == 14) {
				pline("Your skin is burned by %s's block-heeled lady boots!", m_monnam(mtmp));
		    make_burned(HBurned+monsterlev,FALSE);
				}

				if (randomkick == 15) {
				pline("You shudder in fear as you see the violent %s using %s high-heeled plateau boots as lethal weapons.", l_monnam(mtmp), mhis(mtmp));
		    make_feared(HFeared+monsterlev,FALSE);
				}

				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena12;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == LEATHER_PEEP_TOES) || (footwear && footwear->otyp == JULIETTA_PEEP_TOES) || (footwear && footwear->otyp == JUEN_PEEP_TOES) || (randomsexyheels == 14) || mtmp->data == &mons[PM_ANIMATED_LEATHER_PEEP_TOE] || mtmp->data == &mons[PM_WEREPEEPTOE] || mtmp->data == &mons[PM_HUMAN_WEREPEEPTOE] || mtmp->data == &mons[PM_NORTHERN_SHIN_SMASHER]) ) {
elena13:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s leather peep-toes against your shins!", Monnam(mtmp), mhis(mtmp) );
				losehp(d(1,monsterlev), "asian peep-toe", KILLED_BY_AN);

				if (multi >= 0 && !rn2(2)) {
				    if (flags.female) {
					pline("You congratulate %s for %s valiant attempt to hurt you.", mon_nam(mtmp), mhis(mtmp) );
				    } else if (Free_action) {
					pline("It hurts like hell, but you bear it like a man.");            
				    } else {
					pline("It hurts like hell! You pass out from the intense pain.");            
					nomovemsg = "Still groaning with pain, you get back up, ready to fight again.";
					monsterlev = ((mtmp->m_lev) + 1);
					monsterlev /= 2;
					if (monsterlev <= 0) monsterlev = 1;
					if (monsterlev > 1) monsterlev = rnd(monsterlev);
					if (monsterlev > 10) {
						while (rn2(5) && (monsterlev > 10)) {
							monsterlev--;
						}
					}
					nomul(-monsterlev, "knocked out by an asian peep-toe", TRUE);
					exercise(A_DEX, FALSE);
				    }
				}
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena13;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == FEMININE_PUMPS) || (footwear && footwear->otyp == ANITA_LADY_PUMPS) || (footwear && footwear->otyp == WENDY_LEATHER_PUMPS) || (footwear && footwear->otyp == NELLY_LADY_PUMPS) || (footwear && footwear->otyp == DESTRUCTIVE_HEELS) || (footwear && footwear->otyp == AUTOSCOOTER_HEELS) || (footwear && footwear->otyp == VIOLET_BEAUTY_HEELS) || (footwear && itemhasappearance(footwear, APP_VELVET_PUMPS)) || (footwear && itemhasappearance(footwear, APP_FEELGOOD_HEELS)) || (footwear && itemhasappearance(footwear, APP_PARAGRAPH_SHOES)) || (footwear && itemhasappearance(footwear, APP_BONE_HEELS)) || (footwear && itemhasappearance(footwear, APP_BRIDAL_SHOES)) || (footwear && itemhasappearance(footwear, APP_ORGASM_PUMPS)) || (footwear && itemhasappearance(footwear, APP_PRINCESS_PUMPS)) || (footwear && itemhasappearance(footwear, APP_SEXY_HEELS)) || (footwear && itemhasappearance(footwear, APP_SHADOWY_HEELS)) || (footwear && itemhasappearance(footwear, APP_MARY_JANES)) || mtmp->data == &mons[PM_FOURFOLD_SHOE_ENEMY] || mtmp->data == &mons[PM_ANIMATED_SEXY_LEATHER_PUMP] || mtmp->data == &mons[PM_WERESEXYLEATHERPUMP] || mtmp->data == &mons[PM_HUMAN_WERESEXYLEATHERPUMP] || mtmp->data == &mons[PM_ARVOGENIC_TOPMODEL] || (randomsexyheels == 15) || mtmp->data == &mons[PM_ANIMATED_BEAUTIFUL_FUNNEL_HEELED_PUMP] || mtmp->data == &mons[PM_WEREBEAUTIFULFUNNELHEELEDPUMP] || mtmp->data == &mons[PM_HUMAN_WEREBEAUTIFULFUNNELHEELEDPUMP] || mtmp->data == &mons[PM_BLOODY_BEAUTIES] || mtmp->data == &mons[PM_DISINTERESTED_OLDER_WOMAN] || mtmp->data == &mons[PM_JOY_PORN_STAR]) ) {
elena14:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s scratches up and down your %ss with %s heels!", Monnam(mtmp), body_part(LEG), mhis(mtmp) );
				losehp(d(2,monsterlev), "leg-scratch from feminine pumps", KILLED_BY_AN);

			if (u.legscratching <= 5)
		    	    pline("It stings a little.");
			else if (u.legscratching <= 10)
		    	    pline("It hurts quite a bit as some of your skin is scraped off!");
			else if (u.legscratching <= 20)
			    pline("Blood drips from your %s as the heel scratches over your open wounds!", body_part(LEG));
			else if (u.legscratching <= 40)
			    pline("You can feel the heel scratching on your shin bone! It hurts and bleeds a lot!");
			else
			    pline("You watch in shock as your blood is squirting everywhere, all the while feeling the razor-sharp high heel mercilessly opening your %ss!", body_part(LEG));

			/* Need to add a timeout function that will lower the legscratching variable. --Amy
			   It's in timeout.c and will just lower u.legscratching by one every 1000 ticks for now. */

				losehp(u.legscratching, "bleedout from leg scratches", KILLED_BY_AN);
				u.legscratching++;
				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
			    exercise(A_STR, FALSE);
			    exercise(A_DEX, FALSE);

				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena14;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == LADY_BOOTS) || (footwear && footwear->otyp == NADJA_BUCKLED_LADY_SHOES) || (footwear && footwear->otyp == LISA_COMBAT_BOOTS) || (footwear && footwear->otyp == SELF_WILLED_HEELS) || (footwear && itemhasappearance(footwear, APP_DYKE_BOOTS)) || (footwear && itemhasappearance(footwear, APP_ICEBLOCK_HEELS)) || (randomsexyheels == 16) || mtmp->data == &mons[PM_FOURFOLD_SHOE_ENEMY] || mtmp->data == &mons[PM_SUPER_STRONG_GIRL] || mtmp->data == &mons[PM_ANIMATED_LADY_BOOT] ) ) {
elenalady:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(2);
				pline("%s scratches over your %s with her block heel!", Monnam(mtmp), body_part(LEG));
				playerbleed(monsterlev);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elenalady;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == ITALIAN_HEELS) || (footwear && itemhasappearance(footwear, APP_SHARP_EDGED_SANDALS)) || (footwear && footwear->otyp == RED_SPELL_HEELS) || (footwear && footwear->otyp == LONG_POINTY_HEELS) || (footwear && footwear->otyp == MANUELA_COMBAT_BOOTS) || (randomsexyheels == 17)) ) {
elenaitaly:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(2);
				pline("%s's high heels scratch your %s very painfully!", Monnam(mtmp), body_part(LEG));
				playerbleed(monsterlev);
				badeffect();
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elenaitaly;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == STILETTO_SANDALS) || (footwear && footwear->otyp == PATRICIA_STEEL_CAPPED_SANDALS) || (footwear && itemhasappearance(footwear, APP_BALLET_HEELS)) || (footwear && itemhasappearance(footwear, APP_PARAGRAPH_SANDALS)) || (randomsexyheels == 18) || mtmp->data == &mons[PM_ANIMATED_PRETTY_SANDAL]) ) {
elenass:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(2);
				pline("%s's razor-sharp stiletto heel scratches very %sy wounds on your %s!", Monnam(mtmp), body_part(BLOOD), body_part(LEG));
				playerbleed(monsterlev);
				losehp(rnd(monsterlev), "extremely sharp-edged stiletto sandal", KILLED_BY_AN);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elenass;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == COMBAT_STILETTOS) || (footwear && itemhasappearance(footwear, APP_PISTOL_BOOTS)) || (footwear && footwear->otyp == KILLER_HEELS) || (footwear && footwear->otyp == FEMMY_STILETTO_BOOTS) || (randomsexyheels == 19) || mtmp->data == &mons[PM_ANIMATED_COMBAT_STILETTO] || mtmp->data == &mons[PM_WERECOMBATSTILETTO] || mtmp->data == &mons[PM_HUMAN_WERECOMBATSTILETTO]) ) {
elena15:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s does a round-house and painfully hits you with %s combat boots!", Monnam(mtmp), mhis(mtmp) );
				losehp(d(4,monsterlev), "kung-fu attack", KILLED_BY_AN);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena15;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_BEAUTIFUL_HEELS) ) || (randomsexyheels == 20) || mtmp->data == &mons[PM_ANIMATED_BEAUTIFUL_SANDAL] || mtmp->data == &mons[PM_PLOF_ANJE] || mtmp->data == &mons[PM_STEFANJE] || mtmp->data == &mons[PM_OFFICER_HANH] ) ) {
elena16:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				if (multi >= 0) {
					pline("You come because %s's high-heeled sandals are so beautiful.", mon_nam(mtmp));
					nomul(-2, "having a sexual orgasm", TRUE);
					if (practicantterror) {
						pline("%s rings out: 'Wanking off is not permitted in my laboratory, but you know that. 200 zorkmids.'", noroelaname());
						fineforpracticant(200, 0, 0);
					}
				} else {
					switch (rnd(13)) {
						case 1: pline("%s exploits your momentary weakness and kicks your %s with %s high heel.", Monnam(mtmp), body_part(ARM), mhis(mtmp) ); break;
						case 2: pline("%s exploits your momentary weakness and slams %s high heel right in your %s.", Monnam(mtmp), mhis(mtmp), body_part(FACE) ); break;
						case 3: pline("%s exploits your momentary weakness and places %s beautiful heel on your %s.", Monnam(mtmp), mhis(mtmp), body_part(FINGER) ); break;
						case 4: pline("%s exploits your momentary weakness and squeezes your %s with %s oval-shaped heel.", Monnam(mtmp), body_part(FOOT), mhis(mtmp) ); break;
						case 5: pline("%s exploits your momentary weakness and tries to sever your %s with %s lovely high heel.", Monnam(mtmp), body_part(HAND), mhis(mtmp) ); break;
						case 6: pline("%s exploits your momentary weakness and aims %s massive heel at your %s.", Monnam(mtmp), mhis(mtmp), body_part(HEAD) ); break;
						case 7: pline("%s exploits your momentary weakness and slams %s unyielding heel against your %s.", Monnam(mtmp), mhis(mtmp), body_part(LEG) ); break;
						case 8: pline("%s exploits your momentary weakness and kicks you in the %s with %s female sandal.", Monnam(mtmp), body_part(NECK), mhis(mtmp) ); break;
						case 9: pline("%s exploits your momentary weakness and tries to crush your %s with %s incredibly beautiful footwear.", Monnam(mtmp), body_part(SPINE), mhis(mtmp) ); break;
						case 10: pline("%s exploits your momentary weakness and stomps your unprotected %s with %s cyan-striped wood heel.", Monnam(mtmp), body_part(TOE), mhis(mtmp) ); break;
						case 11: pline("%s exploits your momentary weakness and draws lots of %s with %s razor-sharp high heel.", Monnam(mtmp), body_part(BLOOD), mhis(mtmp) ); break;
						case 12: pline("%s exploits your momentary weakness and lands a high-heeled kick in your %s.", Monnam(mtmp), body_part(NOSE) ); break;
						case 13: pline("%s exploits your momentary weakness and places a very fleecy kick into your %s with %s sexy women's sandals.", Monnam(mtmp), body_part(STOMACH), mhis(mtmp) ); break;
					}
					badeffect();
				}
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena16;
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_IRREGULAR_BOOTS) ) || (footwear && itemhasappearance(footwear, APP_SKI_HEELS)) || (footwear && footwear->otyp == HIGH_HEELED_SKIERS) || (footwear && footwear->otyp == BRIDGHITTE_SKI_HEELS) || (randomsexyheels == 21) || mtmp->data == &mons[PM_ANIMATED_IRREGULAR_HEEL] || mtmp->data == &mons[PM_MANGA_GIRL] || mtmp->data == &mons[PM_MANGA_WOMAN] || mtmp->data == &mons[PM_MANGA_LADY] || mtmp->data == &mons[PM_SANDRA_S_DISGUISED_BOOT] || mtmp->data == &mons[PM_RITA_S_HIGH_HEELED_SNEAKER] || mtmp->data == &mons[PM_BRIDGHITTE_S_SKIERS] ) ) {
elena17:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(5);
				pline("Those weird high heels look like they should make %s's %s hurt... but they definitely hurt you, and you feel an icy chill!", mon_nam(mtmp), makeplural(mbodypart(mtmp,FOOT)) );
				make_frozen(HFrozen+monsterlev,FALSE);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena17;
				}

			}

			if ((!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_CUDDLE_CLOTH_BOOTS) ) || (randomsexyheels == 28) ) ) {
elenacuddle:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				pline("%s pushes you with her fleecy black block heels in a very female way!", Monnam(mtmp));
				if (FunnyHallu) pline("(Since when are heels 'fleecy'? Are they made of cuddle cloth or what?)");
				pushplayer(FALSE);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elenacuddle;
				}

			}

			if ((!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_HEAP_OF_SHIT_BOOTS) ) || (footwear && footwear->otyp == HENRIETTA_COMBAT_BOOTS) || (randomsexyheels == 29) ) ) {
elenahos:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				pline("Ewwwww! %s's boots have dog shit clinging on the soles!", Monnam(mtmp));

				register struct obj *objX, *objX2;
				for (objX = invent; objX; objX = objX2) {
				      objX2 = objX->nobj;
					if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
					if (objX && !rn2(100)) wither_dmg(objX, xname(objX), 3, TRUE, &youmonst);
				}
				increasesanity(1);

				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elenahos;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_WEDGE_BOOTS) ) || (footwear && footwear->otyp == MELISSA_WEDGE_BOOTS) || (footwear && itemhasappearance(footwear, APP_WEDGE_SNEAKERS) ) || (randomsexyheels == 22) || mtmp->data == &mons[PM_TOPLESS_NURSE] || (footwear && footwear->otyp == KRISTINA_PLATFORM_SNEAKERS) || mtmp->data == &mons[PM_NORDIC_LADY] || mtmp->data == &mons[PM_ANN_KATHRIN_S_CUDDLY_BOOT] || mtmp->data == &mons[PM_ATHLETIC_FEMMY] || mtmp->data == &mons[PM_LISA_S_CUDDLY_BOOT] ) ) {
elena18:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

				pline("The wedge heel feels very soft and lovely. In fact, you enjoyed %s's kick.", mon_nam(mtmp));
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

			    if (Upolyd) {
				u.mh += monsterlev;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			    } else {
				u.uhp += monsterlev;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			    }
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena18;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_HUGGING_BOOTS) ) || (footwear && footwear->otyp == INA_HUGGING_BOOTS) || (footwear && footwear->otyp == ANNA_HUGGING_BOOTS) || (footwear && footwear->otyp == ARABELLA_HUGGING_BOOTS) || (footwear && footwear->otyp == LUDGERA_HIKING_BOOTS) || (footwear && footwear->otyp == KATI_GIRL_BOOTS) || (footwear && footwear->otyp == LAURA_WINTER_BOOTS) || (footwear && footwear->otyp == SARAH_HUGGING_BOOTS) || (footwear && footwear->otyp == MAURAH_HUGGING_BOOTS) || mtmp->data == &mons[PM_ANIMATED_HUGGING_BOOT] || mtmp->data == &mons[PM_NASTY_FEMMY] || mtmp->data == &mons[PM_SLEEPY_LADY] || mtmp->data == &mons[PM_WEREHUGGINGBOOT] || mtmp->data == &mons[PM_HUMAN_WEREHUGGINGBOOT] || mtmp->data == &mons[PM_THE_EXTRA_FLEECY_BUNDLE_HER_HUGGING_BOOT] || mtmp->data == &mons[PM_LUISA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_REBECCA_S_HUGGING_BOOT] || (randomsexyheels == 23) || mtmp->data == &mons[PM_BITCHY_LARA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_MARLEEN_S_HUGGING_BOOT] || mtmp->data == &mons[PM_VILEA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_LISELOTTE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_OVERSLEPT_GIRL] || mtmp->data == &mons[PM_AMELJE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_MELANIE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_HUGGING_BOOT_GIRL_WITH_A_BMW] || mtmp->data == &mons[PM_BUNDLE_NADJA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_MARIE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_SUPER_STRONG_GIRL] || mtmp->data == &mons[PM_KRISTIN_S_HUGGING_BOOT] || mtmp->data == &mons[PM_ARABELLA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_SHY_ASIAN_GIRL] || mtmp->data == &mons[PM_MARIAN_S_PERSIAN_BOOT] || (footwear && footwear->otyp == JEANETTA_GIRL_BOOTS) || mtmp->data == &mons[PM_LAURA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_LITTLE_MARIE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_CHARLOTTE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_STUPID_BLONDE_GIRL] || mtmp->data == &mons[PM_FRIEDERIKE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_SOPHIA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_HEIKE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_HUSSY_FEMMY] || mtmp->data == &mons[PM_ANOREXIC_FEMMY] || (footwear && footwear->otyp == MARLENA_HIKING_BOOTS) || mtmp->data == &mons[PM_INDIAN_QUEEN] || mtmp->data == &mons[PM_GLITTER_FLAX] || mtmp->data == &mons[PM_EMMELIE_S_WHITE_HUGGING_BOOT] || mtmp->data == &mons[PM_DORA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_THE_HUGGING_TOPMODEL_HER_HUGGING_BOOT] || mtmp->data == &mons[PM_BUNDLE_NADJA] || mtmp->data == &mons[PM_JOHANETTA_S_WINTER_BOOT] || mtmp->data == &mons[PM_SADISTIC_ASIAN_GIRL] || mtmp->data == &mons[PM_ANJA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_LOVING_ASIAN_GIRL] || mtmp->data == &mons[PM_STARLET_BUNDLE] || mtmp->data == &mons[PM_ABSOLUTELY_CUDDLY_GIRL] || mtmp->data == &mons[PM_ALIDA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_GRENEUVENIC_TOPMODEL] || mtmp->data == &mons[PM_OVERSLEPT_TROLL] || mtmp->data == &mons[PM_FANNY_S_LOVELY_WINTER_BOOT] || mtmp->data == &mons[PM_OFFICER_INA] || mtmp->data == &mons[PM_HUGGER_DRAGON] || mtmp->data == &mons[PM_TAIL_GUTTER] ) ) {
elena19:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				randomkick = rnd(3);
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				if ((monsterlev > 1) && (mtmp->mspeed == MFAST)) monsterlev /= 2;

				if (randomkick != 3) {
					pline("%s slams %s hugging boots against your shins, causing wonderfully soothing pain.", Monnam(mtmp), mhis(mtmp) );
					losehp(d(1,monsterlev), "lovely hugging boot", KILLED_BY_AN);
					if (!rn2((mtmp->mspeed == MFAST) ? 10 : 5)) {

						register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
						  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
					    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    exercise(A_STR, FALSE);
					    exercise(A_DEX, FALSE);
						pline("In fact, the pain is so intense, you have trouble moving your %s.", body_part(LEG) );
						if (!rn2(3)) pline("But don't you love the fact that these female hugging boots can hurt you in such a beautiful way?");
					}
				}

				if (randomkick != 1) {
					if (randomkick == 2) pline("%s also places %s hugging boots on your unprotected %s, and it hurts in a wonderful way.", Monnam(mtmp), mhis(mtmp), makeplural(body_part(TOE)) );
					else pline("%s places %s hugging boots on your unprotected %s, and you love the intense pain!", Monnam(mtmp), mhis(mtmp), makeplural(body_part(TOE)) );
					make_numbed(HNumbed+monsterlev,FALSE);
					if (!rn2((mtmp->mspeed == MFAST) ? 10 : 5)) {
						pline("Wow, your little %s really got squashed flat by the very lovely female hugging boot!", body_part(TOE));
						if (Upolyd) u.mhmax--; /* lose one hit point */
						else u.uhpmax--; /* lose one hit point */
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						if (u.mh > u.mhmax) u.mh = u.mhmax;

					}
				}
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena19;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_WINTER_STILETTOS) ) || (footwear && footwear->otyp == RUEA_COMBAT_BOOTS) || (footwear && footwear->otyp == RITA_STILETTOS) || (footwear && footwear->otyp == SANDRA_COMBAT_BOOTS) || (footwear && itemhasappearance(footwear, APP_FEMMY_BOOTS)) || (footwear && itemhasappearance(footwear, APP_FILIGREE_STILETTOS)) || (footwear && itemhasappearance(footwear, APP_COPPER_STILETTOS)) || (footwear && itemhasappearance(footwear, APP_PLATINUM_STILETTOS)) || (footwear && itemhasappearance(footwear, APP_SILVER_STILETTOS)) || (footwear && itemhasappearance(footwear, APP_WEAPON_LIGHT_BOOTS)) || (footwear && itemhasappearance(footwear, APP_FETISH_HEELS)) || mtmp->data == &mons[PM_ANIMATED_WINTER_STILETTO] || mtmp->data == &mons[PM_WEREWINTERSTILETTO] || (randomsexyheels == 24) || mtmp->data == &mons[PM_HUMAN_WEREWINTERSTILETTO] || mtmp->data == &mons[PM_STILETTO_LOVER] || mtmp->data == &mons[PM_SABRINA_S_CONE_HEELED_COMBAT_BOOT] || (FemtrapActiveSandra && humanoid(mtmp->data) && is_female(mtmp->data) ) || mtmp->data == &mons[PM_BOOT_GIRL] ) ) {
elena20:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				int comboscratch = 1;
				if (!rn2(5)) comboscratch += rnd(3);
				monsterlev *= comboscratch;

				if (comboscratch == 1) pline("%s scratches down your %s with %s stiletto heel!", Monnam(mtmp), body_part(LEG), mhis(mtmp) );
				else if (comboscratch == 2) pline("%s scratches down your %s with %s stiletto heel, then pauses for a moment and scratches back up!", Monnam(mtmp), body_part(LEG), mhis(mtmp) );
				else if (comboscratch == 3) pline("%s scratches down your %s with %s stiletto heel, then pauses for a moment and scratches back up and down again!", Monnam(mtmp), body_part(LEG), mhis(mtmp) );
				else if (comboscratch == 4) pline("%s scratches down your %s with %s stiletto heel, and up and down again, and finally the heel scratches back up in an extremely painful way!", Monnam(mtmp), body_part(LEG), mhis(mtmp) );

				losehp(d(1,monsterlev), "sharp-edged winter stilettos", KILLED_BY);


				if (comboscratch == 1) {
					if (u.legscratching <= 5)
				    	    pline("Little skin rashes are developing on your shins. It actually feels soothing.");
					else if (u.legscratching <= 10)
				    	    pline("You see that the high heel scratched a bit of skin off your shins.");
					else if (u.legscratching <= 20)
					    pline("The heel actually managed to hurt your %s, and you're bleeding a little!", body_part(LEG));
					else if (u.legscratching <= 40)
					    pline("The lovely heel is causing your %s to flow! It's dangerous!", body_part(BLOOD));
					else
					    pline("The wonderful winter stiletto is smeared with %s - your %s...", body_part(BLOOD), body_part(BLOOD));
				}
				else if (comboscratch == 2) {
					if (u.legscratching <= 5)
				    	    pline("When the heel scratched back up again, you noticed your unprotected skin getting hurt.");
					else if (u.legscratching <= 10)
				    	    pline("You love the fact that the heel ripped off some of your skin.");
					else if (u.legscratching <= 20)
					    pline("Wow, the high heel has caused a bloody wound with their scratching combo!");
					else if (u.legscratching <= 40)
					    pline("Actually, you think it's unfair that the heel is scratching back up over your already bloody shin.");
					else
					    pline("Whoa, the tender stiletto heel is merciless, and freely scratches up and down your badly bleeding %s!", body_part(LEG) );
				}
				else if (comboscratch == 3) {
					if (u.legscratching <= 5)
				    	    pline("Your skin is scratched very beautifully by the lovely stiletto heel.");
					else if (u.legscratching <= 10)
				    	    pline("The female high-heeled combat boot destroyed large amounts of your skin!");
					else if (u.legscratching <= 20)
					    pline("You see your %s squirting while the unfair high heel abuses your %s with their scratching combos!", body_part(BLOOD), body_part(LEG));
					else if (u.legscratching <= 40)
					    pline("Wow, the winter stiletto is truly able to cause lots of damage, drawing your %s and continually scratching over your terrible open wounds!", body_part(BLOOD));
					else
					    pline("The %s shoots out of your %s like a fountain while the incredibly tender heel keeps slitting your veins in a very merciless way!", body_part(BLOOD), body_part(LEG));
				}
				else if (comboscratch == 4) {
					if (u.legscratching <= 5)
				    	    pline("You love the beautiful female combat boot and the fact that your %s are very susceptible to scratches.", body_part(LEG) );
					else if (u.legscratching <= 10)
				    	    pline("Scratching wounds appear on your %s, and you're curious to see how much more damage those wonderful heels can cause to you!", body_part(LEG));
					else if (u.legscratching <= 20)
					    pline("Your %s is covered by pretty scratching wounds, some of which are bleeding! The feminine leather boots are really good when it comes to causing damage!", body_part(LEG));
					else if (u.legscratching <= 40)
					    pline("The blood-smeared heel just doesn't seem to stop scratching up and down your shins, even though you're badly bleeding and suffering immense pain!");
					else {
					    pline("Your %s covers the floor, your %ss, and of course also the sexy stiletto heels that keep scratching up and down with their beautiful, yet very destructive combos.", body_part(BLOOD), body_part(LEG) );
						pline("These high heels will kill you if you allow them to continue. But on the other hand, they're soooooo cute and lovely, and they look like they would spare you if you're hurt badly enough...");
					}
				}

				losehp(u.legscratching, "bleedout from leg scratches", KILLED_BY_AN);
				u.legscratching += comboscratch;
				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
			    exercise(A_STR, FALSE);
			    exercise(A_DEX, FALSE);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena20;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_CLUNKY_HEELS) ) || (footwear && footwear->otyp == SINFUL_HEELS) || (randomsexyheels == 25) || mtmp->data == &mons[PM_THE_HIGH_HEEL_LOVING_ASIAN_GIRL_HER_HEELS] || mtmp->data == &mons[PM_ALMUTH_S_BLOCK_HEELED_BOOT] || mtmp->data == &mons[PM_HENRIETTA_S_THICK_BLOCK_HEELED_BOOT] || mtmp->data == &mons[PM_KRISTIN_S_SUPER_HIGH_LADY_SHOE] || mtmp->data == &mons[PM_COMMA_LADY] || mtmp->data == &mons[PM_KRISTIN_S_BLOCK_HEELED_SANDAL] || mtmp->data == &mons[PM_THICK_BEAUTY] || mtmp->data == &mons[PM_NINA_S_CLUNKY_HEEL_BOOT] ) ) {
elena21:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rn2(20) ? rnd(10) : rnd(20);
				pline("Argh! Those clunky heels hurt like hell!");
				losehp(d(1,monsterlev), "big fat clunky heel", KILLED_BY_AN);
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena21;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_ANKLE_BOOTS) ) || mtmp->data == &mons[PM_EMMA_S_ANKLE_BOOT] || mtmp->data == &mons[PM_MADELEINE_S_ANKLE_BOOT] || (randomsexyheels == 26) || mtmp->data == &mons[PM_BETTINA_S_COMBAT_BOOT] || mtmp->data == &mons[PM_JANINA_S_COMBAT_BOOT] || mtmp->data == &mons[PM_ARABELLA_S_HIGH_HEELED_LADY_SHOE] || mtmp->data == &mons[PM_HC_HIGHHEEL_HUSSY] ) ) {
elena22:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

				if (!flags.female && !rn2(3) ) { 
					pline("%s kicks right into your nuts with the heel of %s ankle boot!", Monnam(mtmp), mhis(mtmp) );
					losehp(d(2,monsterlev), "ankle boot to the nuts", KILLED_BY_AN);

					if (multi >= 0 && !rn2(5)) {
						pline("You can't breathe!");            
						nomovemsg = "Finally you get yourself back together.";
						nomul(-5, "knocked out by an ankle boot", TRUE);
						exercise(A_DEX, FALSE);
				    }
				}

				else {

					pline("%s scratches up and down your %ss with %s heels!", Monnam(mtmp), body_part(LEG), mhis(mtmp) );
					losehp(d(2,monsterlev), "leg-scratch from ankle boots", KILLED_BY_AN);

					if (u.legscratching <= 5)
				    	    pline("It stings a little.");
					else if (u.legscratching <= 10)
				    	    pline("It hurts quite a bit as some of your skin is scraped off!");
					else if (u.legscratching <= 20)
					    pline("Blood drips from your %s as the heel scratches over your open wounds!", body_part(LEG));
					else if (u.legscratching <= 40)
					    pline("You can feel the heel scratching on your shin bone! It hurts and bleeds a lot!");
					else
					    pline("You watch in shock as your blood is squirting everywhere, all the while feeling the razor-sharp high heel mercilessly opening your %ss!", body_part(LEG));

					losehp(u.legscratching, "bleedout from leg scratches", KILLED_BY);
					u.legscratching++;
					register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
					  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
				    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
				    exercise(A_STR, FALSE);
				    exercise(A_DEX, FALSE);

				}
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena22;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && itemhasappearance(footwear, APP_BLOCK_HEELED_BOOTS) ) || (footwear && itemhasappearance(footwear, APP_BLOCKCHOC_BOOTS) ) || (footwear && footwear->otyp == VICTORIA_COMBAT_BOOTS) || (footwear && footwear->otyp == JETTE_COMBAT_BOOTS) || (footwear && footwear->otyp == DORA_COMBAT_BOOTS) || (footwear && footwear->otyp == SUSANNE_COMBAT_BOOTS) || (footwear && footwear->otyp == NORA_COMBAT_BOOTS) || (footwear && footwear->otyp == KRISTIN_COMBAT_BOOTS) || (footwear && footwear->otyp == MELTEM_COMBAT_BOOTS) || (footwear && footwear->otyp == JULIA_COMBAT_BOOTS) || (footwear && footwear->otyp == NICOLE_COMBAT_BOOTS) || (footwear && footwear->otyp == ELENA_COMBAT_BOOTS) || (footwear && footwear->otyp == THAI_COMBAT_BOOTS) || (footwear && footwear->otyp == KERSTIN_COMBAT_BOOTS) || (footwear && footwear->otyp == JENNIFER_COMBAT_BOOTS) || (footwear && footwear->otyp == KATRIN_COMBAT_BOOTS) || (footwear && footwear->otyp == COVETED_BOOTS) || (footwear && footwear->otyp == HIGH_SCORING_HEELS) || mtmp->data == &mons[PM_ANIMATED_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_SLAP_HEELED_SANDAL_BOOT] || mtmp->data == &mons[PM_WEREBLOCKHEELEDCOMBATBOOT] || mtmp->data == &mons[PM_HUMAN_WEREBLOCKHEELEDCOMBATBOOT] || mtmp->data == &mons[PM_SHY_LAURA_S_LOVELY_COMBAT_BOOT] || mtmp->data == &mons[PM_LILLY_S_FLEECY_COMBAT_BOOT] || (randomsexyheels == 27) || mtmp->data == &mons[PM_HANNAH_S_COMBAT_BOOT] || mtmp->data == &mons[PM_SABINE_S_ZIPPER_BOOT] || mtmp->data == &mons[PM_LARISSA_S_BLOCK_HEELED_BOOT] || mtmp->data == &mons[PM_NICOLE_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_ANTJE_S_BLOCK_HEELED_BOOT] || mtmp->data == &mons[PM_LISA_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_FOURFOLD_SHOE_ENEMY] || mtmp->data == &mons[PM_KRISTIN_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_RUEA_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_JUEN_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_RUTH_S_BLOCK_HEELED_LADY_BOOT] || mtmp->data == &mons[PM_PATRICIA_S_COMBAT_BOOT] || mtmp->data == &mons[PM_DESIREE_S_COMBAT_BOOT] || mtmp->data == &mons[PM_INGE_S_COMBAT_BOOT] || mtmp->data == &mons[PM_CORINA_S_SPECIAL_COMBAT_BOOT] || mtmp->data == &mons[PM_KATRIN_S_COMBAT_BOOT] || mtmp->data == &mons[PM_BIRGIT_S_LADY_BOOT] || mtmp->data == &mons[PM_BLOCK_HEELED_GIRL] || mtmp->data == &mons[PM_LAURA_S__SISTER__COMBAT_BOOT] || mtmp->data == &mons[PM_POWERFUL_BLONDE_GIRL] || mtmp->data == &mons[PM_KARATE_FEMMY] || mtmp->data == &mons[PM_MARLEEN_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_CZECH_WENCH] || mtmp->data == &mons[PM_UNFORTUNATE_FOREST] || mtmp->data == &mons[PM_BITCHY_LARA_S_BLOCK_HEELED_BOOT] || mtmp->data == &mons[PM_PERSONA_NON_GRATA] || mtmp->data == &mons[PM_BLOCK_HEELED_PUSSY] ) ) {
elena23:
				u.cnd_shoedamageamount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				randomkick = rnd(12);
				if (randomkick == 1) {
				pline("%s steps on your %s with %s block heels!", Monnam(mtmp), body_part(HAND), mhis(mtmp) );
				    incr_itimeout(&Glib, 20); /* painfully jamming your fingers */
				losehp(d(1,monsterlev), "lovely block-heeled boot", KILLED_BY_AN);
				}
				if (randomkick == 2) {
				pline("%s jams your %ss with %s wonderful high-heeled combat boots!", Monnam(mtmp), body_part(TOE), mhis(mtmp) );

				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
			    exercise(A_STR, FALSE);
			    exercise(A_DEX, FALSE);

				losehp(d(1,monsterlev), "block-heeled lady boot", KILLED_BY_AN);
				}

				if (randomkick == 3) {
					if (!flags.female) { 
					pline("%s kicks right into your nuts with %s soft block heel!", Monnam(mtmp), mhis(mtmp) );

					losehp(d(2,monsterlev), "block heel to the nuts", KILLED_BY_AN);
					}

					else {
					pline("%s gently strokes you with %s soft block heel.", Monnam(mtmp), mhis(mtmp) );
					if (Upolyd) u.mh += monsterlev; /* heal some hit points */
					else u.uhp += monsterlev; /* heal some hit points */
					pline("It feels very soothing.");
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					}

				}

				if (randomkick == 4) {
				pline("%s painfully stomps your body with %s fleecy combat boots!", Monnam(mtmp), mhis(mtmp) );
				u.uhp -= 1;
				u.uhpmax -= 1;
				u.uen -= 1;
				u.uenmax -= 1;
				losehp(d(3,monsterlev), "being stomped by a fleecy combat boot", KILLED_BY);
				}

				if (randomkick == 5) {
				pline("The beautiful block heel kicks your ass, which feels very fleecy-soft.");
				morehungry(monsterlev * 5);

				}

				if (randomkick == 6) {
				pline("The massive heel hits your optical nerve, and suddenly you can't see a thing." );
				make_blinded(Blinded+monsterlev,FALSE);
				}

				if (randomkick == 7) {
				pline("Suddenly %s fully hits your %s with %s massive block heel.", mon_nam(mtmp), body_part(HEAD), mhis(mtmp) );
				make_confused(HConfusion+monsterlev,FALSE);
				}

				if (randomkick == 8) {
				pline("You sustain a critical hit by the sexy high heel.");
				make_stunned(HStun+monsterlev,FALSE);
				}

				if (randomkick == 9) {
				if (!rn2(25)) {
					pline("The lovely block heel was contaminated! Seems they're not really as lovely as you thought!" );
					    make_sick(rn1(25,25), "contaminated block heel", TRUE, SICK_VOMITABLE);
					losehp(d(1,monsterlev), "unclean block heel", KILLED_BY_AN);
					}
				else {
				pline("You're hit by a poisoned part of %s's footwear!", mon_nam(mtmp));
						poisoned("block heel", A_STR, "poisoned block heel", 8);
					losehp(d(1,monsterlev), "impregnated block heel", KILLED_BY_AN);
					}
				}

				if (randomkick == 10) {
				pline("Ow, %s is stomping you repeatedly with %s wonderful combat boots!", mon_nam(mtmp), mhis(mtmp));
				make_numbed(HNumbed+monsterlev,FALSE);
				}

				if (randomkick == 11) {
				pline("The sexy block heel scrapes quite some skin off your shins!");
				make_burned(HBurned+monsterlev,FALSE);
				}

				if (randomkick == 12) {
				pline("You are hit by %s's high-heeled female footwear, and suddenly become afraid of %s like the little coward you are. Wimp. :-P", l_monnam(mtmp), mhis(mtmp));
				make_feared(HFeared+monsterlev,FALSE);
				}
				if (FemtrapActiveElena && !rn2(3)) {
					pline("You long for more!");
					goto elena23;
				}

			}

			if (!rn2(250)) pushplayer(FALSE);

			break;
		case AT_STNG:
			pline("%s stings you!", Monnam(mtmp));

			if (uwep && uwep->oartifact == ART_DROP_EVERYTHING_AND_KILL_T) {

				pline("Arrgh! You get really angry at %s.", mon_nam(mtmp));
				if (u.berserktime) {
					if (!obsidianprotection()) switch (rn2(11)) {
					case 0:
						make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20), "mosquito sickness", TRUE, SICK_NONVOMITABLE);
						break;
					case 1: make_blinded(Blinded + 25, TRUE);
						break;
					case 2: if (!Confusion)
						You("suddenly feel %s.", FunnyHallu ? "trippy" : "confused");
						make_confused(HConfusion + 25, TRUE);
						break;
					case 3: make_stunned(HStun + 25, TRUE);
						break;
					case 4: make_numbed(HNumbed + 25, TRUE);
						break;
					case 5: make_frozen(HFrozen + 25, TRUE);
						break;
					case 6: make_burned(HBurned + 25, TRUE);
						break;
					case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
						break;
					case 8: (void) make_hallucinated(HHallucination + 25, TRUE, 0L);
						break;
					case 9: make_feared(HFeared + 25, TRUE);
						break;
					case 10: make_dimmed(HDimmed + 25, TRUE);
						break;
					}

				} else u.berserktime = 25;

			}
			if (u.twoweap && uswapwep && uswapwep->oartifact == ART_DROP_EVERYTHING_AND_KILL_T) {

				pline("Arrgh! You get really angry at %s.", mon_nam(mtmp));
				if (u.berserktime) {
					if (!obsidianprotection()) switch (rn2(11)) {
					case 0:
						make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20), "mosquito sickness", TRUE, SICK_NONVOMITABLE);
						break;
					case 1: make_blinded(Blinded + 25, TRUE);
						break;
					case 2: if (!Confusion)
						You("suddenly feel %s.", FunnyHallu ? "trippy" : "confused");
						make_confused(HConfusion + 25, TRUE);
						break;
					case 3: make_stunned(HStun + 25, TRUE);
						break;
					case 4: make_numbed(HNumbed + 25, TRUE);
						break;
					case 5: make_frozen(HFrozen + 25, TRUE);
						break;
					case 6: make_burned(HBurned + 25, TRUE);
						break;
					case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
						break;
					case 8: (void) make_hallucinated(HHallucination + 25, TRUE, 0L);
						break;
					case 9: make_feared(HFeared + 25, TRUE);
						break;
					case 10: make_dimmed(HDimmed + 25, TRUE);
						break;
					}

				} else u.berserktime = 25;

			}

			if ((!rn2(player_shades_of_grey() ? 200 : (u.ualign.type == A_LAWFUL) ? 1000 : (u.ualign.type == A_NEUTRAL) ? 500 : 1000)) && (!issoviet || !rn2(5)) ) {
			pline("You are bleeding out from your stinging injury!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Ne prosto poteryayet odnu maksimal'nuyu khitpoint. Poteryat' ikh vsekh, i nadeyus', chto yeshche odnu glupuyu smert' vse ravno nichego ne poluchite vy." : "Ffffffffschhhhhhhhhh!");
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

			if (Upolyd) {u.mhmax -= rnd(monsterlev); if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax -= rnd(monsterlev); if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }


			}

			if ((uarmg && itemhasappearance(uarmg, APP_PRINCESS_GLOVES)) && !rn2(25)) {

				pline("%s deeply stings you for being such a spoiled princess.", Monnam(mtmp));
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

				if (Upolyd) {u.mhmax -= rnd(monsterlev); if (u.mh > u.mhmax) u.mh = u.mhmax;}
				else {u.uhpmax -= rnd(monsterlev); if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }

			}

			if ((uarmf && itemhasappearance(uarmf, APP_PRINCESS_PUMPS) ) && !rn2(15)) {

				pline("%s deeply stings you for being such a spoiled princess.", Monnam(mtmp));
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

				if (Upolyd) {u.mhmax -= rnd(monsterlev); if (u.mh > u.mhmax) u.mh = u.mhmax;}
				else {u.uhpmax -= rnd(monsterlev); if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }

			}

			break;
		case AT_BUTT:
			pline("%s butts you!", Monnam(mtmp));

			if (multi >= 0 && (!issoviet || !rn2(5)) && !rn2(player_shades_of_grey() ? 25 : (u.ualign.type == A_LAWFUL) ? 100 : (u.ualign.type == A_NEUTRAL) ? 50 : 200)) {
			    if (Free_action) {
				You_feel("a slight shaking.");            
			    } else {
				You("flinch!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				monsterlev = ((mtmp->m_lev) + 1);
				monsterlev /= 3;
				if (monsterlev <= 0) monsterlev = 1;
				if (monsterlev > 1) monsterlev = rnd(monsterlev);
				if (monsterlev > 4) {
					while (rn2(5) && (monsterlev > 4)) {
						monsterlev--;
					}
				}
				nomul(-monsterlev, "flinching", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}

			if (!rn2(50)) pushplayer(FALSE);

			break;
		case AT_SCRA:
			pline("%s scratches you!", Monnam(mtmp));
			if ((!rn2(player_shades_of_grey() ? 75 : (u.ualign.type == A_LAWFUL) ? 300 : (u.ualign.type == A_NEUTRAL) ? 500 : 400)) && (!issoviet || !rn2(5)) ) {
			pline("One of your arteries bursts open! You suffer from %s loss!", body_part(BLOOD));
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
			monsterlev = rnd(monsterlev);
			losehp((monsterlev), "scratching attack", KILLED_BY_AN);

			if (Upolyd) {u.mhmax -= monsterlev/2; if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax -= monsterlev/2; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }

			}

			if ((uarmg && itemhasappearance(uarmg, APP_PRINCESS_GLOVES)) && !rn2(15)) {

				pline("%s angrily cuts up your unprotected princess skin.", Monnam(mtmp));
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev = rnd(monsterlev);
				losehp((monsterlev), "scratching attack", KILLED_BY_AN);

				if (Upolyd) {u.mhmax -= monsterlev/2; if (u.mh > u.mhmax) u.mh = u.mhmax;}
				else {u.uhpmax -= monsterlev/2; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }

			}

			if ((uarmf && itemhasappearance(uarmf, APP_PRINCESS_PUMPS) ) && !rn2(15)) {

				pline("%s angrily cuts up your unprotected princess skin.", Monnam(mtmp));
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev = rnd(monsterlev);
				losehp((monsterlev), "scratching attack", KILLED_BY_AN);

				if (Upolyd) {u.mhmax -= monsterlev/2; if (u.mh > u.mhmax) u.mh = u.mhmax;}
				else {u.uhpmax -= monsterlev/2; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }

			}

			break;
		case AT_LASH:
			pline("%s lashes you!", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Tvoya zadnitsa sobirayetsya poluchit' vzbityye, suka!" : "Rhaeaeaeaei!");
			if (FemtrapActiveJessica && !rn2(3)) {
				pline("Oh no! %s bashed your tender, vulnerable butt cheeks!", Monnam(mtmp));
				monsterlev = ((mtmp->m_lev) + 1);
				losehp(monsterlev, "a slap on the butt cheeks", KILLED_BY);
			}

			if ((!rn2(player_shades_of_grey() ? 5 : (u.ualign.type == A_LAWFUL) ? 40 : (u.ualign.type == A_NEUTRAL) ? 50 : 30)) && (!issoviet || !rn2(5)) ) {
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("Your %s spins in confusion.", body_part(HEAD));
			make_confused(HConfusion + monsterlev, FALSE);
			}

			if ((uarmf && itemhasappearance(uarmf, APP_PRINCESS_PUMPS) ) && !rn2(15)) {

				pline("%s lashes you because you're such a spoiled princess.", Monnam(mtmp));

				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("Your %s spins in confusion.", body_part(HEAD));
				make_confused(HConfusion + monsterlev, FALSE);

			}

			if (uarmf && uarmf->oartifact == ART_HUGGING__GROPING_AND_STROK) {
				u.uen += mtmp->m_lev;
				if (u.uen > u.uenmax) u.uen = u.uenmax;
				pline("The lashing energizes you.");

			}

			if (!rn2(200)) pushplayer(FALSE);
			break;
		case AT_TRAM:
			pline("%s tramples over you!", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Monstry budut toptat' vas, potomu chto vy ochen' plokhoy igrok." : "Klatsch klatsch!");
			if (!rn2(player_shades_of_grey() ? 3 : 15) && !(Role_if(PM_ASTRONAUT) && rn2(5)) && (!issoviet || !rn2(5)) ) {
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("You can't think straight as your every muscle is aching!");
			make_stunned(HStun + monsterlev, FALSE);
			}
			if (!rn2(25)) pushplayer(FALSE);
			break;
		case AT_TUCH:
			pline("%s touches you!", Monnam(mtmp));
			if ((!issoviet || !rn2(5)) && !rn2(3)) losehp(player_shades_of_grey() ? 2 : 1, "icy touch", KILLED_BY_AN);

			if (FemtrapActiveJessica && !rn2(3)) {
				pline("Your tender butt cheeks are stimulated by %s's caressing touch... and you start producing tender farting noises.", mon_nam(mtmp));
				if (!extralongsqueak()) badeffect();
			}

			if (mtmp->data == &mons[PM_BLACK_DEATH]) { /* lose one maximum HP --Amy */

				if (Upolyd) {u.mhmax -= 1; if (u.mh > u.mhmax) u.mh = u.mhmax;}
				else {u.uhpmax -= 1; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }

			}

			break;
		case AT_TENT:
			pline("%s tentacles suck you!",
				        s_suffix(Monnam(mtmp)));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' my budem yest' vash mozg, i vy budete umirat' ot gluposti." : "TschuecktschueckTschuecktschueckTschuecktschueck!");
			if ((!issoviet || !rn2(5)) && rn2(2)) {
				monsterlev = ((mtmp->m_lev) + 1);
				monsterlev /= player_shades_of_grey() ? 2 : 5;
				if (monsterlev <= 0) monsterlev = 1;
				losehp((monsterlev), "sucking tentacle attack", KILLED_BY_AN);
			}
			break;
		case AT_EXPL:
		case AT_BOOM:
			pline("%s explodes!", Monnam(mtmp));
			break;
		case AT_MAGC:
			pline("%s curses at you!", Monnam(mtmp));
			break;
		case AT_MULTIPLY:
			/* No message. */
		break;
		default:
			pline("%s hits you!", Monnam(mtmp));
	    }
}


STATIC_OVL void
missmu(mtmp, target, roll, mattk)           /* monster missed you */
register struct monst *mtmp;
register int target;
register int roll;
register struct attack *mattk;
{
	register boolean nearmiss = (target == roll);
	register struct obj *blocker = (struct obj *)0;	
		/* 3 values for blocker
		 *	No blocker:  (struct obj *) 0  
		 * 	Piece of armour:  object
		 *	magical: &zeroobj
		 */

	if (target < roll) {
		/* get object responsible 
		 * Work from the closest to the skin outwards
		 */
		/* Try undershirt if tourist */
		if (uarmu && target <= roll) {
			target += ARM_BONUS(uarmu);
			if (target > roll) blocker = uarmu;
		}
		/* Try body armour */
		if (uarm && target <= roll) {
			target += ARM_BONUS(uarm);
			if (target > roll) blocker = uarm;
		}

		if (uarmg && !rn2(10)) {
			/* Try gloves */
			target += ARM_BONUS(uarmg);
			if (target > roll) blocker = uarmg;
		}
		if (uarmf && !rn2(10)) {
			/* Try boots */
			target += ARM_BONUS(uarmf);
			if (target > roll) blocker = uarmf;
		}
		if (uarmh && !rn2(5)) {
			/* Try helm */
			target += ARM_BONUS(uarmh);
			if (target > roll) blocker = uarmh;
		}
		if (uarmc && target <= roll) {
			/* Try cloak */
			target += ARM_BONUS(uarmc);
			if (target > roll) blocker = uarmc;
		}
		if (uarms && target <= roll) {
			/* Try shield */
			target += ARM_BONUS(uarms);
			if (target > roll) blocker = uarms;
		}
		if (uimplant && target <= roll) {
			/* Try implant */
			target += ARM_BONUS(uimplant);
			if (target > roll) blocker = uimplant;
		}
		if (target <= roll) {
			/* Try spell protection */
			target += u.uspellprot;
			if (target > roll) blocker = &zeroobj;
		}			
	}

	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);

	if(could_seduce(mtmp, &youmonst, mattk) && !mtmp->mcan)
	    pline("%s pretends to be friendly.", Monnam(mtmp));
	else {
	    if (!flags.verbose || !nearmiss && !blocker)
		pline("%s misses.", Monnam(mtmp));
	    else if (!blocker)
		pline("%s just misses!", Monnam(mtmp));
	    else if (blocker == &zeroobj)
		pline("%s is stopped by the golden haze.", Monnam(mtmp));
	    else {
		Your("%s %s%s %s attack.", 
			simple_typename(blocker->otyp),
			rn2(2) ? "block" : "deflect",
			(blocker == uarmg || blocker == uarmf) ? "" : "s",
			s_suffix(mon_nam(mtmp)));
		if (uarms && (blocker == uarms)) use_skill(P_SHIELD, 1);
		if (uimplant && (blocker == uimplant)) use_skill(P_IMPLANTS, 1);
		if (uarm && (blocker == uarm) && uwep && is_lightsaber(uwep) && uwep->lamplit && (uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS) ) { /* has to train quickly, otherwise it's too much of a PITA because of low robe AC --Amy */
			use_skill(P_SORESU, rnd(2));
			if (!rn2(5)) use_skill(P_SORESU, rnd(2));
		}
		if (uarm && (blocker == uarm) && uarm->oartifact == ART_SORESURE) {
			use_skill(P_SORESU, rnd(2));
			if (!rn2(5)) use_skill(P_SORESU, rnd(2));
		}
		u.ubodyarmorturns++;
		if (u.ubodyarmorturns >= 5) {
			u.ubodyarmorturns = 0;
			use_skill(P_BODY_ARMOR, 1);
		}

		if (Race_if(PM_VIETIS) && blocker && !rn2(1000)) {
			if (blocker == uarm && blocker->spe < 7) {
				blocker->spe++;
				pline("Your armor becomes harder!");
			}
			else if (blocker == uarmc && blocker->spe < 7) {
				blocker->spe++;
				pline("Your cloak becomes harder!");
			}
			else if (blocker == uarmu && blocker->spe < 7) {
				blocker->spe++;
				pline("Your shirt becomes harder!");
			}
			else if (blocker == uarms && blocker->spe < 7) {
				blocker->spe++;
				pline("Your shield becomes harder!");
			}
			else if (blocker == uarmg && blocker->spe < 7) {
				blocker->spe++;
				pline("Your pair of gloves becomes harder!");
			}
			else if (blocker == uarmf && blocker->spe < 7) {
				blocker->spe++;
				pline("Your pair of shoes becomes harder!");
			}
			else if (blocker == uarmh && blocker->spe < 7) {
				blocker->spe++;
				pline("Your helmet becomes harder!");
			}
		}

		/* compost equipment will occasionally undo a point of negative enchantment --Amy */
		if (blocker && objects[blocker->otyp].oc_material == MT_COMPOST && blocker->spe < 0 && !rn2(100)) {
			if (blocker == uarm) {
				blocker->spe++;
				pline("Your armor repairs itself a bit!");
			}
			else if (blocker == uarmc) {
				blocker->spe++;
				pline("Your cloak repairs itself a bit!");
			}
			else if (blocker == uarmu) {
				blocker->spe++;
				pline("Your shirt repairs itself a bit!");
			}
			else if (blocker == uarms) {
				blocker->spe++;
				pline("Your shield repairs itself a bit!");
			}
			else if (blocker == uarmg) {
				blocker->spe++;
				pline("Your pair of gloves repairs itself a bit!");
			}
			else if (blocker == uarmf) {
				blocker->spe++;
				pline("Your pair of shoes repairs itself a bit!");
			}
			else if (blocker == uarmh) {
				blocker->spe++;
				pline("Your helmet repairs itself a bit!");
			}
		}

		int savechance = 0;

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_BODY_ARMOR)) {

			    case P_BASIC:		savechance = 1; break;
			    case P_SKILLED:	savechance = 2; break;
			    case P_EXPERT:	savechance = 3; break;
			    case P_MASTER:	savechance = 4; break;
			    case P_GRAND_MASTER:savechance = 5; break;
			    case P_SUPREME_MASTER:savechance = 6; break;
			    default: savechance += 0; break;
			}
		}

		/* evil patch idea: if equipment is used very often, it eventually degrades --Amy */

		if (armorwilldull(blocker) && (rnd(7) > savechance) && !issoviet) {
			if (blocker->greased) {
				blocker->greased--;
				pline("Your %s loses its grease.", simple_typename(blocker->otyp));
			} else {
				blocker->spe--;
				pline("Your %s dulls.", simple_typename(blocker->otyp));
			}
		}


	    }

	    if (MON_WEP(mtmp)) {
		struct obj *obj = MON_WEP(mtmp);
		if (obj) {
			obj->owornmask &= ~W_WEP;
			if ((rnd(100) < (obj->oeroded * 5 / 2)) && !stack_too_big(obj)) {
			    if (obj->spe > -5) {    
				obj->spe--;
				pline("%s %s is damaged further!",
					s_suffix(Monnam(mtmp)), xname(obj));
			    } else
				pline("%s %s is badly battered!", 
					s_suffix(Monnam(mtmp)), xname(obj));
			}
		}
	    }
	}
	stop_occupation();
}

STATIC_OVL void
mswings(mtmp, otemp)		/* monster swings obj */
register struct monst *mtmp;
register struct obj *otemp;
{
        if (!flags.verbose || Blind || !mon_visible(mtmp)) return;
	pline("%s %s %s %s.", Monnam(mtmp),
	      (objects[otemp->otyp].oc_dir & PIERCE) ? "thrusts" : "swings",
	      mhis(mtmp), singular(otemp, xname));
}

/* return how a poison attack was delivered */
const char *
mpoisons_subj(mtmp, mattk)
struct monst *mtmp;
struct attack *mattk;
{
	if (mattk->aatyp == AT_WEAP) {
	    struct obj *mwep = (mtmp == &youmonst) ? uwep : MON_WEP(mtmp);
	    /* "Foo's attack was poisoned." is pretty lame, but at least
	       it's better than "sting" when not a stinging attack... */
	    return (!mwep || !mwep->opoisoned) ? "attack" : "weapon";
	} else {
	    return (mattk->aatyp == AT_TUCH) ? "contact" :
		   (mattk->aatyp == AT_GAZE) ? "gaze" :
		   (mattk->aatyp == AT_BITE) ? "bite" : "sting";
	}
}

/* called when your intrinsic speed is taken away */
void
u_slow_down()
{
	HFast = 0L;
	if (!Fast) You("slow down.");
	   /* speed boots */
	else Your("quickness feels less natural.");
	if (PlayerHearsSoundEffects) pline(issoviet ? "Kha, vy tol'ko chto poteryali skorost'. Udachi vam poluchit' yego obratno. I kogda vy eto sdelayete, igra budet ubedit'sya, chto vy mgnovenno poteryat' yego snova i tip bloka l'da budet ochen' pozabavilo." : "Wschlschlschlschlsch!");
	exercise(A_DEX, FALSE);
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL void
wildmiss(mtmp, mattk)		/* monster attacked your displaced image */
	register struct monst *mtmp;
	register struct attack *mattk;
{
	int compat;

	/* no map_invisible() -- no way to tell where _this_ is coming from */

	if (!flags.verbose) return;
	if (!cansee(mtmp->mx, mtmp->my)) return;
		/* maybe it's attacking an image around the corner? */

	compat = (mattk->adtyp == AD_SEDU || mattk->adtyp == AD_SSEX) &&
		 could_seduce(mtmp, &youmonst, (struct attack *)0);

	if (!mtmp->mcansee || (Invis && !perceives(mtmp->data))) {
	    const char *swings =
		mattk->aatyp == AT_BEAM ? "blasts" :
		mattk->aatyp == AT_BREA ? "breathes" :
		mattk->aatyp == AT_SPIT ? "spits" :
		mattk->aatyp == AT_BITE ? "snaps" :
		mattk->aatyp == AT_KICK ? "kicks" :
		mattk->aatyp == AT_LASH ? "lashes" :
		mattk->aatyp == AT_TRAM ? "stomps" :
		(mattk->aatyp == AT_STNG ||
		 mattk->aatyp == AT_BUTT ||
		 nolimbs(mtmp->data)) ? "lunges" : "swings";

	    if (compat)
		pline("%s tries to touch you and misses!", Monnam(mtmp));
	    else
		switch(rn2(3)) {
		case 0: pline("%s %s wildly and misses!", Monnam(mtmp),
			      swings);
		    break;
		case 1: pline("%s attacks a spot beside you.", Monnam(mtmp));
		    break;
		case 2: pline("%s strikes at %s!", Monnam(mtmp),
				levl[mtmp->mux][mtmp->muy].typ == WATER
				    ? "empty water" : "thin air");
		    break;
		default:pline("%s %s wildly!", Monnam(mtmp), swings);
		    break;
		}
	} else if (Displaced) {
	    if (compat)
		pline("%s smiles %s at your %sdisplaced image...",
			Monnam(mtmp),
			compat == 2 ? "engagingly" : "seductively",
			Invis ? "invisible " : "");
	    else
		pline("%s strikes at your %sdisplaced image and misses you!",
			/* Note: if you're both invisible and displaced,
			 * only monsters which see invisible will attack your
			 * displaced image, since the displaced image is also
			 * invisible.
			 */
                        Monnam(mtmp),Invis ? "invisible " : "");

	} else if (Underwater) {
	    /* monsters may miss especially on water level where
	       bubbles shake the player here and there */
	    if (compat)
		pline("%s reaches towards your distorted image.",Monnam(mtmp));
	    else
		pline("%s is fooled by water reflections and misses!",Monnam(mtmp));

	} else impossible("monster attacks you without knowing your location?");
}

void
expels(mtmp, mdat, message)
register struct monst *mtmp;
register struct permonst *mdat; /* if mtmp is polymorphed, mdat != mtmp->data */
boolean message;
{
	if (message) {
		if (is_animal(mdat))
			You("get regurgitated!");
		else {
			char blast[40];
			register int i;

			blast[0] = '\0';
			for(i = 0; i < NATTK; i++)
				if(mdat->mattk[i].aatyp == AT_ENGL)
					break;
			if (mdat->mattk[i].aatyp != AT_ENGL)
			      /*impossible*/pline("You get expelled from a swallower that has no engulfing attack!");
			else {
				if (is_whirly(mdat)) {
					switch (mdat->mattk[i].adtyp) {
						case AD_ELEC:
							strcpy(blast,
						      " in a shower of sparks");
							break;
						case AD_COLD:
							strcpy(blast,
							" in a blast of frost");
							break;
					}
				} else
					strcpy(blast, " with a squelch");
                                You("get expelled from %s%s!",mon_nam(mtmp), blast);
			}
		}
	}
	unstuck(mtmp);	/* ball&chain returned in unstuck() */
	mnexto(mtmp);
	newsym(u.ux,u.uy);
	spoteffects(TRUE);
	/* to cover for a case where mtmp is not in a next square */
	if(um_dist(mtmp->mx,mtmp->my,1))
		pline("Brrooaa...  You land hard at some distance.");
}

#endif /* OVLB */
#ifdef OVL0

/* select a monster's next attack, possibly substituting for its usual one */
struct attack *
getmattk(mptr, indx, prev_result, alt_attk_buf)
struct permonst *mptr;
int indx, prev_result[];
struct attack *alt_attk_buf;
{
    struct attack *attk = &mptr->mattk[indx];

    /* prevent a monster with two consecutive disease or hunger attacks
       from hitting with both of them on the same turn; if the first has
       already hit, switch to a stun attack for the second */
    if (indx > 0 && prev_result[indx - 1] > 0 &&
	    (attk->adtyp == AD_DISE ||
		attk->adtyp == AD_PEST ||
		attk->adtyp == AD_FAMN) &&
	    attk->adtyp == mptr->mattk[indx - 1].adtyp && rn2(5) /* let's be a sadistic programmer --Amy */ ) {
	*alt_attk_buf = *attk;
	attk = alt_attk_buf;
	attk->adtyp = AD_STUN;
    }
    return attk;
}

/* Intelligent monsters try and avoid "blue on blue" incidents.
 */
STATIC_OVL int
blue_on_blue(mtmp)
struct monst *mtmp;
{
    int x, y;
    struct monst *mon;
    if (!mtmp->mconf && !Conflict && !mtmp->mflee && !mindless(mtmp->data)) {
	if (!lined_up(mtmp))
	    return FALSE;	/* Irrelevant; monster won't attack anyway */
	x = mtmp->mx + sgn(tbx);
	y = mtmp->my + sgn(tby);
	while(x != mtmp->mux || y != mtmp->muy) {
	    mon = m_at(x, y);
	    if (mon && m_cansee(mtmp, x, y) && !mon->mundetected && !mon->minvisreal &&
		    (!mon->minvis || perceives(mtmp->data)))
		return TRUE;
	    x += sgn(tbx);
	    y += sgn(tby);
	}
    }
    return FALSE;
}

/*
 * mattacku: monster attacks you
 *	returns 1 if monster dies (e.g. "yellow light"), 0 otherwise
 *	Note: if you're displaced or invisible the monster might attack the
 *		wrong position...
 *	Assumption: it's attacking you or an empty square; if there's another
 *		monster which it attacks by mistake, the caller had better
 *		take care of it...
 */
int
mattacku(mtmp)
	register struct monst *mtmp;
{
	char buf[BUFSZ];
	struct	attack	*mattk, alt_attk;
      struct attack *a;
	int	i, j, tmp, sum[NATTK];
	struct	permonst *mdat = mtmp->data;
	struct permonst *mdat2;
	boolean ranged = (distu(mtmp->mx, mtmp->my) > 3);
		/* Is it near you?  Affects your actions */
	boolean range2 = !monnear(mtmp, mtmp->mux, mtmp->muy);
		/* Does it think it's near you?  Affects its actions */
	boolean foundyou = (mtmp->mux==u.ux && mtmp->muy==u.uy);
		/* Is it attacking you or your image? */
	boolean youseeit = canseemon(mtmp);
		/* Might be attacking your image around the corner, or
		 * invisible, or you might be blind....
		 */

	int yourarmorclass;
	int hittmp;
	int mlevfortohit;

	/* you can attack land-based monsters while underwater, so why should YOU be protected from THEIR attacks??? --Amy */
	if(!ranged) nomul(0, 0, FALSE);
	if(mtmp->mhp <= 0 /*|| (Underwater && !is_swimmer(mtmp->data))*/)
	    return(0);

	/* If swallowed, can only be affected by u.ustuck */
	if(u.uswallow) {
		if(mtmp != u.ustuck) return(0);
	    u.ustuck->mux = u.ux;
	    u.ustuck->muy = u.uy;
	    range2 = 0;
	    foundyou = 1;
	    if(u.uinvulnerable) return (0); /* stomachs can't hurt you! */
	}

	else if (u.usteed) {
		if (mtmp == u.usteed)
			/* Your steed won't attack you */
			return (0);
		/* Orcs like to steal and eat horses and the like
		 * Amy edit: wtf. replace that with just a generic chance that your steed is attacked */
		if (will_hit_steed() && distu(mtmp->mx, mtmp->my) <= 2) {
			/* Attack your steed instead */
			i = mattackm(mtmp, u.usteed);
			if ((i & MM_AGR_DIED))
				return (1);
			if (i & MM_DEF_DIED || (u.umoved && issoviet)) {
				if (u.umoved && issoviet) pline("Vasha loshad' ne soprotivlyayetsya. Potomu chto ty igrayesh' v der'movom variante.");
				return (0);
			}
			/* Let your steed retaliate, Amy edit: of course also if you've moved */

			/* In Soviet Russia, no horse may ever fight back when attacked unless it stands perfectly still.
			 * After all, horses aren't meant to be used in combat much, for cookie reasons. */

			return (!!(mattackm(u.usteed, mtmp) & MM_DEF_DIED));
		}
	}

	if (u.uundetected && !range2 && foundyou && !u.uswallow) {
		u.uundetected = 0;
		if (is_hider(youmonst.data)) {
		    coord cc; /* maybe we need a unexto() function? */
		    struct obj *obj;

		    You("fall from the %s!", ceiling(u.ux,u.uy));
		    if (enexto(&cc, u.ux, u.uy, youmonst.data)) {
			remove_monster(mtmp->mx, mtmp->my);
			newsym(mtmp->mx,mtmp->my);
			place_monster(mtmp, u.ux, u.uy);
			if(mtmp->wormno) worm_move(mtmp);
			teleds(cc.x, cc.y, TRUE);
			set_apparxy(mtmp);
			newsym(u.ux,u.uy);
		    } else {
			pline("%s is killed by a falling %s (you)!",
					Monnam(mtmp), !missingnoprotect ? youmonst.data->mname : "creature");
			killed(mtmp);
			newsym(u.ux,u.uy);
			if (mtmp->mhp > 0) return 0;
			else return 1;
		    }
		    if (youmonst.data->mlet != S_PIERCER)
			return(0);	/* trappers don't attack */

		    obj = which_armor(mtmp, WORN_HELMET);
		    if (obj && is_metallic(obj) && !is_etheritem(obj)) {
			Your("blow glances off %s helmet.",
			               s_suffix(mon_nam(mtmp)));
		    } else {
			if (3 + find_mac(mtmp) <= rnd(20)) {
			    pline("%s is hit by a falling piercer (you)!",
								Monnam(mtmp));
			    if ((mtmp->mhp -= d(3,6)) < 1)
				killed(mtmp);
			} else
			  pline("%s is almost hit by a falling piercer (you)!",
								Monnam(mtmp));
		    }
		} else {
		    if (!youseeit)
			pline("It tries to move where you are hiding.");
		    else {
			/* Ugly kludge for eggs.  The message is phrased so as
			 * to be directed at the monster, not the player,
			 * which makes "laid by you" wrong.  For the
			 * parallelism to work, we can't rephrase it, so we
			 * zap the "laid by you" momentarily instead.
			 */
			struct obj *obj = level.objects[u.ux][u.uy];

			if (obj || is_wagon(u.ux, u.uy) ||
			      (youmonst.data->mlet == S_EEL && is_waterypool(u.ux, u.uy))) {
			    int save_spe = 0; /* suppress warning */
			    if (obj) {
				save_spe = obj->spe;
				if (obj->otyp == EGG) obj->spe = 0;
			    }
			    if (is_wagon(u.ux, u.uy)) {
	     pline("Wait, %s!  There's a %s named %s hiding under a wagon!",
				m_monnam(mtmp), !missingnoprotect ? youmonst.data->mname : "creature", playeraliasname);

			    } else if (youmonst.data->mlet == S_EEL)
		pline("Wait, %s!  There's a hidden %s named %s there!",
				m_monnam(mtmp), !missingnoprotect ? youmonst.data->mname : "creature", playeraliasname);
			    else if (uarmh && uarmh->oartifact == ART_JANA_S_DECEPTIVE_MASK && !rn2(100)) {
					u.youaredead = 1;
					pline("NETHACK caused a General Protection Fault at address 0014:2035.");
					killer_format = KILLED_BY;
					killer = "Jana's deception";
					done(DIED);
					u.youaredead = 0;
				} else
	     pline("Wait, %s!  There's a %s named %s hiding under %s!",
				m_monnam(mtmp), !missingnoprotect ? youmonst.data->mname : "creature", playeraliasname,
				doname(level.objects[u.ux][u.uy]));
			    if (obj) obj->spe = save_spe;
			} else
			    impossible("hiding under nothing?");
		    }
		    newsym(u.ux,u.uy);
		}
		return(0);
	}
	if (youmonst.data->mlet == S_MIMIC && youmonst.m_ap_type &&
		    !range2 && foundyou && !u.uswallow) {
		if (!youseeit) pline("It gets stuck on you.");
		else pline("Wait, %s!  That's a %s named %s!",
			   m_monnam(mtmp), !missingnoprotect ? youmonst.data->mname : "creature", playeraliasname);
		setustuck(mtmp);
		youmonst.m_ap_type = M_AP_NOTHING;
		youmonst.mappearance = 0;
		newsym(u.ux,u.uy);
		return(0);
	}

	/* player might be mimicking an object */
	if (youmonst.m_ap_type == M_AP_OBJECT && !range2 && foundyou && !u.uswallow) {
	    if (!youseeit)
		 pline("%s %s!", Something,
			(likes_gold(mtmp->data) && youmonst.mappearance == GOLD_PIECE) ?
			"tries to pick you up" : "disturbs you");
	    else pline("Wait, %s!  That %s is really %s named %s!",
			m_monnam(mtmp),
			mimic_obj_name(&youmonst),
			!missingnoprotect ? an(mons[u.umonnum].mname) : "a polymorphed missingno",
			playeraliasname);
	    /*if (multi < 0) {*/	/* this should always be the case but is not, due to mimicry spell --Amy */
		char buf[BUFSZ];
		sprintf(buf, "You appear to be %s again.",
			(Upolyd && !missingnoprotect) ? (const char *) an(youmonst.data->mname) :
			    (const char *) "yourself");
		unmul(buf);	/* immediately stop mimicking */
		youmonst.m_ap_type = M_AP_NOTHING;
		youmonst.mappearance = 0;
	    /*}*/
	    return 0;
	}

/*	Work out the armor class differential	*/
	/*tmp = AC_VALUE(u.uac) + 10;*/		/* tmp ~= 0 - 20 */

	if (u.uac >= 0) yourarmorclass = u.uac;
	else if (u.uac > -40) yourarmorclass = -rnd(-(u.uac));
	else if (u.uac > -80) {
		yourarmorclass = -u.uac;
		yourarmorclass -= rn2(yourarmorclass - 38);
		yourarmorclass = -rnd(yourarmorclass);
	}
	else if (u.uac > -120) {
		yourarmorclass = -u.uac;
		yourarmorclass -= rn3(yourarmorclass - 78);
		yourarmorclass -= rn2(yourarmorclass - 38);
		yourarmorclass = -rnd(yourarmorclass);
	}
	else { /* AC of -120 or better */
		yourarmorclass = -u.uac;
		yourarmorclass -= rn3(yourarmorclass - 118);
		yourarmorclass -= rn3(yourarmorclass - 78);
		yourarmorclass -= rn2(yourarmorclass - 38);
		yourarmorclass = -rnd(yourarmorclass);
	}

	tmp = yourarmorclass + 10;

	/* very high-level monsters shouldn't require absurd amounts of AC --Amy */
	mlevfortohit = mtmp->m_lev;
	if (mlevfortohit > 39) mlevfortohit -= rn3(mlevfortohit - 38);
	if (mlevfortohit > 29) mlevfortohit -= rn2(mlevfortohit - 28);
	if (mlevfortohit > 19) mlevfortohit -= rn2(mlevfortohit - 18);

	/* deep down, the "chaff" monsters shouldn't be completely irrelevant */
	if (!rn2(5) && (level_difficulty() > 20) && mlevfortohit < 5) mlevfortohit = 5;
	if (!rn2(5) && (level_difficulty() > 40) && mlevfortohit < 10) mlevfortohit = 10;
	if (!rn2(5) && (level_difficulty() > 60) && mlevfortohit < 15) mlevfortohit = 15;
	if (!rn2(5) && (level_difficulty() > 80) && mlevfortohit < 20) mlevfortohit = 20;

	/* and here we give them another little boost */
	if (level_difficulty() > 5 && mlevfortohit < 5 && !rn2(5)) mlevfortohit++;
	if (level_difficulty() > 10 && mlevfortohit < 5 && !rn2(2)) mlevfortohit++;
	if (level_difficulty() > 10 && mlevfortohit < 10 && !rn2(5)) mlevfortohit++;
	if (level_difficulty() > 20 && mlevfortohit < 10 && !rn2(3)) mlevfortohit++;
	if (level_difficulty() > 20 && mlevfortohit < 15 && !rn2(5)) mlevfortohit++;
	if (level_difficulty() > 40 && mlevfortohit < 15 && !rn2(4)) mlevfortohit++;
	if (level_difficulty() > 30 && mlevfortohit < 20 && !rn2(5)) mlevfortohit++;
	if (level_difficulty() > 60 && mlevfortohit < 20 && !rn2(5)) mlevfortohit++;

	tmp += mlevfortohit;
	if (mtmp->egotype_hitter) tmp += 10;
	if (mtmp->egotype_piercer) tmp += 25;
	if(multi < 0) tmp += 4;
        if((Invis && !perceives(mdat) && (StrongInvis || !rn2(3)) ) || !mtmp->mcansee) tmp -= 2;
	if(mtmp->mtrapped) tmp -= 2;
	if(tmp <= 0) tmp = 1;
	if (mtmp->data == &mons[PM_IVORY_COAST_STAR]) tmp += 30; /* this monster is aiming abnormally well */
	if (mtmp->data == &mons[PM_HAND_OF_GOD]) tmp += 100; /* God personally is guiding this one's blows */
	if (mtmp->data == &mons[PM_AKIRA_GIRL]) tmp += 20; /* she needs some boost */
	if (mtmp->data == &mons[PM_FIRST_DUNVEGAN]) tmp += 100; /* this monster also almost always hits */
	if (mtmp->data == &mons[PM_JOURHEA]) tmp -= 40;	/* has terribly bad aim */
	if (mtmp->data == &mons[PM_THE_DIN_MISSED_YOU_]) tmp -= 20;	/* also aims badly */
	if (mtmp->data == &mons[PM_FUNNY_MISSER]) tmp -= rnd(20);	/* ditto */
	if (mtmp->data == &mons[PM_CURSED____LEFTHANDED_FARTING_ELEPHANT]) tmp -= 10;	/* left-handed */
	if (mtmp->data == &mons[PM_UNEXPECTED_BANNER]) tmp -= rnd(20);	/* ditto */
	if (mtmp->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) tmp += rnd(100); /* the elder priest uses an aimbot and a wallhack */

	/* farting monsters are simply more likely to hit you, except if you bash their sexy butts --Amy */
	if (mtmp->data->msound == MS_FART_LOUD && !mtmp->butthurt) tmp += rnd(5);
	if (mtmp->data->msound == MS_FART_NORMAL && !mtmp->butthurt) tmp += rnd(7);
	if (mtmp->data->msound == MS_FART_QUIET && !mtmp->butthurt) tmp += rnd(10);
	if (mtmp->data->msound == MS_WHORE && !mtmp->butthurt) tmp += rnd(15);
	if (mtmp->data->msound == MS_SHOE) tmp += rnd(10);
	if (mtmp->data->msound == MS_STENCH) tmp += rnd(15);
	if (mtmp->data->msound == MS_CONVERT) tmp += rnd(5);
	if (mtmp->data->msound == MS_HCALIEN) tmp += rnd(10);
	if (mtmp->egotype_farter) tmp += rnd(7);
	if (mtmp->fartbonus) tmp += rnd(mtmp->fartbonus);
	if (mtmp->crapbonus) tmp += rno(mtmp->crapbonus);
	if (is_table(mtmp->mx, mtmp->my)) tmp += 3;
	if (humanoid(mtmp->data) && is_female(mtmp->data) && attacktype(mtmp->data, AT_KICK) && FemtrapActiveMadeleine) tmp += rnd(100);
	if (humanoid(mtmp->data) && is_female(mtmp->data) && FemtrapActiveWendy) tmp += rnd(20);

	if (!rn2(20)) tmp += 20; /* "natural 20" like in D&D --Amy */

	/* weaker monsters should hit you less often --Amy */
	if (!rn2(2) && mdat && !strongmonst(mdat) && !extra_nasty(mdat) && !(mdat->geno & G_UNIQ) && tmp > 1) tmp -= rnd(tmp / 2);

	if (uimplant && uimplant->oartifact == ART_GYMNASTIC_LOVE && !rn2(5)) tmp -= 100;

	if (u.twoweap && uwep && uswapwep && tech_inuse(T_WEAPON_BLOCKER)) tmp -= rnd(20);

	if ((uarmf && itemhasappearance(uarmf, APP_VELCRO_SANDALS)) && attacktype(mtmp->data, AT_CLAW)) tmp += rnd(100);

	if (Conflict) tmp += rnd(1 + mlevfortohit);
	if (StrongConflict) tmp += rnd(1 + mlevfortohit);
	/* after all, they're also hitting each other, so need to make things more difficult for you --Amy */

	if (!rn2(2) && (tmp > 1)) tmp /= 2; /* don't make high-level monsters automatically hit you --Amy */

	/* make eels visible the moment they hit/miss us */
	if(mdat->mlet == S_EEL && mtmp->minvis && !mtmp->minvisreal && cansee(mtmp->mx,mtmp->my) && rn2(5) ) {
		mtmp->minvis = 0;
		newsym(mtmp->mx,mtmp->my);
	}
	/* but not always --Amy */

	if(mdat->mlet == S_FLYFISH && mtmp->minvis && !mtmp->minvisreal && cansee(mtmp->mx,mtmp->my) && !rn2(5) ) {
		mtmp->minvis = 0;
		newsym(mtmp->mx,mtmp->my);
	}

	/* Make Star Vampires visible the moment they hit/miss us */
	if(mtmp->data == &mons[PM_STAR_VAMPIRE] && mtmp->minvis && !mtmp->minvisreal
	   && cansee(mtmp->mx, mtmp->my)) {
	    mtmp->minvis = 0;
	    newsym(mtmp->mx, mtmp->my);
	}

/*	Special demon handling code */
	if(!mtmp->cham && (is_demon(mdat) || mtmp->egotype_gator) && (!range2 || (Race_if(PM_WEAPON_XORN) && distu(mtmp->mx, mtmp->my) < 9) )
	   && mtmp->data != &mons[PM_DEMON_SPOTTER] /* moved to monmove.c --Amy */
	   && mtmp->data != &mons[PM_FUNK_CAR] /* ditto */
	   && mtmp->data != &mons[PM_BALROG]
	   && mtmp->data != &mons[PM_SUCCUBUS]
	   && mtmp->data != &mons[PM_INCUBUS]
 	   && mtmp->data != &mons[PM_NEWS_DAEMON]
 	   && mtmp->data != &mons[PM_PRINTER_DAEMON])
	    if(!mtmp->mcan && !rn2(mtmp->data == &mons[PM_PERCENTI_OPENS_A_GATE_] ? 5 : mtmp->data == &mons[PM_CHEATER_LION] ? 5 : mtmp->data == &mons[PM_TEA_HUSSY] ? 5 : mtmp->data == &mons[PM_PERCENTI_PASSES_TO_YOU_] ? 5 : 23)) {
			msummon(mtmp, FALSE);
			pline("%s opens a gate!",Monnam(mtmp) );
			if (PlayerHearsSoundEffects) pline(issoviet ? "Sovetskaya nadeyetsya, chto demony zapolnyayut ves' uroven' i ubit' vas." : "Pitschaeff!");
		}


/*	Special arbitrator handling code --Amy */

	if (mtmp->data == &mons[PM_ARBITRATOR] && !rn2(25) ) {

		wake_nearby();
		pline("%s blows a whistle!",Monnam(mtmp) ); 
		if (PlayerHearsSoundEffects) pline(issoviet ? "Arbitr svistnul, dazhe yesli on ne imeyet svistok." : "Pfiiiiiiet!");
		/* even if the player stole it; arbitrators can somehow whistle anyway */
	}

	if (mtmp->data == &mons[PM_WHISTLE_HEAD_NERVE_HEAD] && !rn2(25) ) {

		wake_nearby();
		pline("%s makes an ultra-annoying whistling sound!",Monnam(mtmp) ); 
		if (PlayerHearsSoundEffects) pline(issoviet ? "Kakoy-to ublyudok - nadoyedlivyy mudak." : "Pfiepfiepfie pfiiiie pfiePFIEpfie");
	}
	if (mtmp->data == &mons[PM_DURATION_HALLER]) {

		wake_nearby();
		pline("%s makes a lot of noise!",Monnam(mtmp) ); 
	}
	if (mtmp->data == &mons[PM_TLITTLE_TCHILD] && (mtmp->mhp < mtmp->mhpmax) && (mtmp->mhp < (mtmp->mhpmax * 9 / 10)) ) {

		wake_nearby();
		pline("%s cries like a little baby!",Monnam(mtmp) ); 
	}

	if (mtmp->data == &mons[PM_DINGBAT] && !rn2(25) ) {

		wake_nearby();
		pline("*ding*"); 
	}

	if (mtmp->data == &mons[PM_LUXMAS] && !rn2(20) ) {

		wake_nearby();
		You_hear(FunnyHallu ? "someone sing 'Jingle bells, jingle bells...'" : "a jingle.");
	}

/*	Special lycanthrope handling code */
	if(!mtmp->cham && is_were(mdat) && (!range2 || (Race_if(PM_WEAPON_XORN) && distu(mtmp->mx, mtmp->my) < 9) ) ) {
	    if(is_human(mdat)) {
		if(!rn2(15 - (night() * 5)) && !mtmp->mcan) new_were(mtmp);
	    } else if(!rn2(30) && !mtmp->mcan) new_were(mtmp);
	    mdat = mtmp->data;

	    if(!rn2(10) && !mtmp->mcan) {
	    	int numseen, numhelp;
		char buf[BUFSZ], genericwere[BUFSZ];

		strcpy(genericwere, "creature");
		numhelp = were_summon(mdat, FALSE, &numseen, genericwere, FALSE);
		if (youseeit) {
			pline("%s summons help!", Monnam(mtmp));
			if (numhelp > 0) {
			    if (numseen == 0)
				You_feel("hemmed in.");
			} else pline("But none comes.");
		} else {
			const char *from_nowhere;

			if (flags.soundok) {
				pline("%s %s!", Something,
					makeplural(growl_sound(mtmp)));
				from_nowhere = "";
			} else from_nowhere = " from nowhere";
			if (numhelp > 0) {
			    if (numseen < 1) You_feel("hemmed in.");
			    else {
				if (numseen == 1)
			    		sprintf(buf, "%s appears",
							an(genericwere));
			    	else
			    		sprintf(buf, "%s appear",
							makeplural(genericwere));
				pline("%s%s!", upstart(buf), from_nowhere);
			    }
			} /* else no help came; but you didn't know it tried */
		}
	    }
	}

	if(u.uinvulnerable) {
	    /* monsters won't attack you */
	    if(mtmp == u.ustuck)
		pline("%s loosens its grip slightly.", Monnam(mtmp));
	    else if(!range2) {
		if (youseeit || sensemon(mtmp))
		    pline("%s starts to attack you, but pulls back.",
			  Monnam(mtmp));
		else
		    You_feel("%s move nearby.", something);
	    }
	    return (0);
	}

	/* Unlike defensive stuff, don't let them use item _and_ attack. */
	if(!blue_on_blue(mtmp) && find_offensive(mtmp)) {
		int foo = use_offensive(mtmp);

		if (foo != 0) return(foo==1);
	}

	for(i = 0; i < NATTK; i++) {

	    sum[i] = 0;
	    mattk = getmattk(mdat, i, sum, &alt_attk);
	    if (u.uswallow && !mtmp->egotype_engulfer && (mattk->aatyp != AT_ENGL))
		continue;
	    switch(mattk->aatyp) {
		case AT_CLAW:	/* "hand to hand" attacks */
		case AT_KICK:
		case AT_BITE:
		case AT_LASH:
		case AT_TRAM:
		case AT_SCRA:
		case AT_STNG:
		case AT_TUCH:
		case AT_BUTT:
		case AT_TENT:
			if(( (!range2 || (Race_if(PM_WEAPON_XORN) && distu(mtmp->mx, mtmp->my) < 9) ) && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
					!touch_petrifies(youmonst.data))) || (mtmp->egotype_hugger && !rn2(20) && ((dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= (BOLT_LIM * BOLT_LIM)) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) ) ) ) {
			    if (foundyou) {
				if ((tmp > (j = rnd(20+i))) || (uarmf && itemhasappearance(uarmf, APP_KOREAN_SANDALS) && !rn2(3) ) ) {
				    if ( (mattk->aatyp != AT_KICK || !rn2(5)) ||
					    (!thick_skinned(youmonst.data) && !(uwep && uwep->oartifact == ART_ETRUSCIAN_SWIMMING_LESSON) && !Race_if(PM_DUTHOL) && !(uarmf && uarmf->oartifact == ART_THICK_FARTING_GIRL) && !(uarmf && uarmf->oartifact == ART_ANTJE_S_POWERSTRIDE) && !(uwep && uwep->oartifact == ART_PATRICIA_S_FEMININITY) ) )
					sum[i] = hitmu(mtmp, mattk);
				} else
				    missmu(mtmp, tmp, j, mattk);
			    } else wildmiss(mtmp, mattk);
			}

			if(lined_up(mtmp) && ((dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone())) && !rn2(5) && mtmp->egotype_beamer && (tmp > (rnd(20+i))) ){  
				if (foundyou) sum[i] = hitmu(mtmp, mattk);  
				else wildmiss(mtmp, mattk);  
			}  

			if ((!range2 || (Race_if(PM_WEAPON_XORN) && distu(mtmp->mx, mtmp->my) < 9) ) && mtmp->egotype_engulfer) {
			    if(foundyou) {
				if((u.uswallow || tmp > (j = rnd(20+i))) && (rnd(125) > ACURR(A_WIS)) && (issoviet || rn2(10)) ) { /* 10% chance to miss --Amy */
				    /* Force swallowing monster to be
				     * displayed even when player is
				     * moving away */
				    flush_screen(1);
				    sum[i] = gulpmu(mtmp, mattk);
				    if (evilfriday && !rn2(2)) {
					if(!Confusion) You("are getting confused from spinning around.");
					make_confused(HConfusion + (mtmp->m_lev + 5), FALSE);
				    }
				} else {
		                                missmu(mtmp, tmp, j, mattk);
				}
			    } else if (is_animal(mtmp->data)) {
				pline("%s gulps some air!", Monnam(mtmp));
			    } else {
				if (youseeit)
				    pline("%s lunges forward and recoils!",
					  Monnam(mtmp));
				else
				    You_hear("a %s nearby.",
					     is_whirly(mtmp->data) ?
						"rushing noise" : "splat");
			   }
			}

			if (mtmp->egotype_gazer) {
				sum[i] = gazemu(mtmp, mattk);
			}

			if (mtmp->egotype_weaponizer) goto swingweapon;

			break;
		case AT_HUGS:	/* automatic if prev two attacks succeed */
			/* Note: if displaced, prev attacks never succeeded */
		/* Note by Amy: come on, allow it to hit sometimes even if there are no previous attacks (shambling horror)! */
		                if(((!range2 || (Race_if(PM_WEAPON_XORN) && distu(mtmp->mx, mtmp->my) < 9) ) && i>=2 && sum[i-1] && sum[i-2]) || mtmp == u.ustuck || (!rn2(Race_if(PM_IRRITATOR) ? 4 : 50) && ((dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= (BOLT_LIM * BOLT_LIM)) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone())) ) ) {
				if ( (tmp > (rnd(20+i))) || (tmp > (rnd(20+i))) ) sum[i]= hitmu(mtmp, mattk);
				}
		/* This has the side effect of AT_HUGS hitting from far away. I decided to declare this "bug" a feature. */
			break;
		case AT_BEAM:  /* ranged non-contact attack by Chris - only go off 40% of the time for balance reasons --Amy */
			if (!rn2(2) && !issoviet) break;
cursesatyou:
			if(lined_up(mtmp) && ((dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) ) && (tmp > (rnd(20+i))) && (rnd(5) > 3) ) {  
				if (foundyou) sum[i] = hitmu(mtmp, mattk);  
				else wildmiss(mtmp, mattk);  
			}
			break;
		case AT_GAZE:	/* can affect you either ranged or not */
			if (!rn2(2) && !issoviet) break;
			/* Medusa gaze already operated through m_respond in
			 * dochug(); don't gaze more than once per round.
			 */
			if (mdat != &mons[PM_MEDUSA])
				sum[i] = gazemu(mtmp, mattk);
			break;
		case AT_EXPL:	/* automatic hit if next to, and aimed at you */
			if(!range2) sum[i] = explmu(mtmp, mattk, foundyou);
			break;
		case AT_ENGL:
			if (!range2 || (Race_if(PM_WEAPON_XORN) && distu(mtmp->mx, mtmp->my) < 9) ) {
			    if(foundyou) {
				if((u.uswallow || tmp > (j = rnd(20+i))) && (issoviet || rn2(10)) ) { /* 10% chance to miss --Amy */
				    /* Force swallowing monster to be
				     * displayed even when player is
				     * moving away */
				    flush_screen(1);
				    sum[i] = gulpmu(mtmp, mattk);
				    if (evilfriday && !rn2(2)) {
					if(!Confusion) You("are getting confused from spinning around.");
					make_confused(HConfusion + (mtmp->m_lev + 5), FALSE);
				    }
				} else {
		                                missmu(mtmp, tmp, j, mattk);
				}
			    } else if (is_animal(mtmp->data)) {
				pline("%s gulps some air!", Monnam(mtmp));
			    } else {
				if (youseeit)
				    pline("%s lunges forward and recoils!",
					  Monnam(mtmp));
				else
				    You_hear("a %s nearby.",
					     is_whirly(mtmp->data) ?
						"rushing noise" : "splat");
			   }
			}
			break;
		case AT_BREA:

			if (!issoviet && rn2(2)) break;

			if (/*range2 &&*/ !blue_on_blue(mtmp) && (ZAP_POS(levl[u.ux][u.uy].typ) ) && rn2(25) && (mattk->adtyp == AD_RBRE || (mattk->adtyp >= AD_MAGM && mattk->adtyp <= AD_SPC2) ) )
			    sum[i] = breamu(mtmp, mattk);
			else if ( (rnd(5) > 3) && ( (tmp > (rnd(20+i))) || (tmp > (rnd(20+i))) ) && lined_up(mtmp) && ((dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) ) )
			{  
				if (foundyou) sum[i] = hitmu(mtmp, mattk);  
				else wildmiss(mtmp, mattk);  
			}  

			/* Note: breamu takes care of displacement */
			break;
		case AT_SPIT:

			if (!issoviet && rn2(2)) break;

			if (/*range2 &&*/ !blue_on_blue(mtmp) && rn2(25) && (mattk->adtyp == AD_ACID || mattk->adtyp == AD_BLND || mattk->adtyp == AD_DRLI || mattk->adtyp == AD_TCKL || mattk->adtyp == AD_NAST) )
			    sum[i] = spitmu(mtmp, mattk);
			/* Note: spitmu takes care of displacement */

			else if ( (rnd(5) > 3) && ( (tmp > (rnd(20+i))) || (tmp > (rnd(20+i))) ) && lined_up(mtmp) && ((dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone())) )
			{  
				if (foundyou) sum[i] = hitmu(mtmp, mattk);  
				else wildmiss(mtmp, mattk);  
			}  

			break;
		case AT_MULTIPLY:
			/*
			 * Monster multiplying is an AT_ for the following
			 * reasons:
			 *   1. Monsters will only multiply when they're close
			 *      to you.  The whole level will not become clogged
			 *      up with giant lice from monsters multiplying
			 *      where you can't see them.
			 *   2. Tame monsters won't multiply.  Too bad! (unless
			 *      they are conflicted or confused from hunger.
			 *      A bit of a "tactic" -- but then you'll have to
			 *      let them bite you, and anyway who really wants
			 *      a dozen pet fleas to feed?)
			 *   3. Monsters have to be next to you to multiply.
			 *      This makes the inevitable altar abuse a little
			 *      harder.
			 *   4. Elbereth will stop monsters multiplying.
			 *      Otherwise a ring of conflict would crowd out a
			 *      whole level in no time.
			 *   5. It is a hack.  (Shrug)
			 *
			 * Multiplying monsters must be low-level and
			 * low-frequency, so as to minimise altar/experience
			 * abuse.  Any multiplying monsters above about
			 * level 5 should be G_NOCORPSE.
			 *
			 * RJ
			 */
			if (!u.sterilized && (!range2 || (Race_if(PM_WEAPON_XORN) && distu(mtmp->mx, mtmp->my) < 9) ) && (!rn2(5)) ) { /* greatly reduced chance --Amy */

			    pline("%s multiplies!",Monnam(mtmp) );
			    clone_mon(mtmp, 0, 0);
				}
			break;
		case AT_WEAP:

			if (mtmp->egotype_hugger && !rn2(20) && ((dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= (BOLT_LIM * BOLT_LIM)) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone())) && (tmp > (rnd(20+i))) )
			hitmu(mtmp, mattk);

			if(lined_up(mtmp) && ((dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone())) && !rn2(5) && mtmp->egotype_beamer && (tmp > (rnd(20+i))) ){  
				if (foundyou) sum[i] = hitmu(mtmp, mattk);  
				else wildmiss(mtmp, mattk);  
			}  

swingweapon:

			if(range2 || !rn2(4) ) {
				register struct obj *rangewepon;
#ifdef REINCARNATION
				if (!Is_rogue_level(&u.uz) || !rn2(3) ) {
#endif

					rangewepon = select_rwep(mtmp, FALSE);
					if (!rangewepon && !range2) goto usemelee;

					if (!blue_on_blue(mtmp)) thrwmu(mtmp);
#ifdef REINCARNATION
				}
#endif
			} else {
usemelee:
			    hittmp = 0;

			    /* Rare but not impossible.  Normally the monster
			     * wields when 2 spaces away, but it can be
			     * teleported or whatever....
			     */
			     if (mtmp->weapon_check == NEED_WEAPON || !MON_WEP(mtmp)) {
				mtmp->weapon_check = NEED_HTH_WEAPON;
				/* mon_wield_item resets weapon_check as
				 * appropriate */
				if (mon_wield_item(mtmp) != 0) break;
			    }
			    if (foundyou) {
				otmp = MON_WEP(mtmp);
				if (otmp) {
				    hittmp = hitval(otmp, &youmonst);
				    tmp += hittmp;
				    mswings(mtmp, otmp);
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == WEDGED_LITTLE_GIRL_SANDAL && (tmp > rnd(20+i)) ) {
elena24:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("The massive wedge heel thunders painfully on your %s!", body_part(HEAD));
					losehp(rnd(4),"a wedged little-girl sandal",KILLED_BY);
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena24;
						}
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == SOFT_GIRL_SNEAKER && (tmp > rnd(20+i)) ) {
elena25:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("The soft leather sneaker actually feels quite soothing.");

					if (Upolyd) u.mh++; /* heal one hit point */
					else u.uhp++; /* heal one hit point */

					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena25;
						}
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS && (tmp > rnd(20+i)) ) {
elena26:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("The unyielding plateau boot bonks your %s!", body_part(HEAD));
					losehp(rnd(10),"a sturdy plateau boot for girls",KILLED_BY);
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena26;
						}
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == BLOCK_HEELED_SANDAL && (tmp > rnd(20+i)) ) {
elenabhs:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("Your %s is bashed by a very fleecy block heel!", body_part(HEAD));
					make_dimmed(HDimmed + rnd(50), TRUE);
					losehp(rnd(10),"a pretty block-heeled sandal",KILLED_BY);

						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elenabhs;
						}
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == PROSTITUTE_SHOE && (tmp > rnd(20+i)) ) {
elenaps:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("Wow, you just can't resist the beautiful prostitute shoe as the massive heel slams on your %s.", body_part(HEAD));
					losehp(rnd(10),"a sexy prostitute shoe",KILLED_BY);
					badeffect();
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elenaps;
						}

					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == BLOCK_HEELED_COMBAT_BOOT && (tmp > rnd(20+i)) ) {
elena27:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					if (flags.female) {
						pline("The massive heel hits your %s. Wow, this feels soothing and lovely!", body_part(HEAD));

						if (Upolyd) u.mh++; /* heal one hit point */
						else u.uhp++; /* heal one hit point */

						if (!rn2(5)) {
							if (Upolyd) u.mh += 5; /* heal one hit point */
							else u.uhp += 5; /* heal one hit point */
						}

						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						if (u.mh > u.mhmax) u.mh = u.mhmax;

						You_feel("a strange sensation.");
						make_confused(HConfusion + rnd(4), FALSE);
					}
					else {

						pline("The massive heel painfully hits your %s!", body_part(HEAD));
						losehp(rnd(12),"a block-heeled combat boot",KILLED_BY);
						pline("You're seeing little asterisks everywhere.");
						make_confused(HConfusion + rnd(10), FALSE);
						}
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena27;
						}
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == HUGGING_BOOT && (tmp > rnd(20+i)) ) {
elena28:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("Uff! Your %s got hit hard!", body_part(HEAD));
					losehp(rnd(12),"a hugging boot",KILLED_BY);
						if (Upolyd) u.mhmax--; /* lose one hit point */
						else u.uhpmax--; /* lose one hit point */
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						if (u.mh > u.mhmax) u.mh = u.mhmax;
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena28;
						}
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == WOODEN_GETA && (tmp > rnd(20+i)) ) {
elena29:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("Argh! The unyielding piece of wood painfully lands on your %s!", body_part(HEAD));
					losehp(rnd(15),"a wooden Japanese sandal",KILLED_BY);

					if (multi >= 0 && !rn2(2)) {
					    if (Free_action) {
						pline("You struggle to stay on your %s.", makeplural(body_part(FOOT)));
					    } else {
						pline("You're knocked out and helplessly drop to the floor.");
						nomovemsg = 0;	/* default: "you can move again" */
						nomul(-rnd(5), "knocked out by a wooden Japanese sandal", TRUE);
						exercise(A_DEX, FALSE);
						    }
						}
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena29;
						}
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == LACQUERED_DANCING_SHOE && (tmp > rnd(20+i)) ) {
elena30:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					if (Role_if(PM_COURIER)) pline("The lacquered dancing shoe harmlessly scratches you.");
					else {pline("The lacquered dancing shoe scratches your %s!", body_part(HEAD));
						if (PlayerHearsSoundEffects) pline(issoviet ? "Budem nadeyat'sya, chto mat' Selin zastavit vas istekat' krov'yu do smerti neodnokratno tsarapat' vas kablukami." : "KRRRRRRIEZ!");

						if (!uarmh || uarmh->otyp != DUNCE_CAP) {

					    /* No such thing as mindless players... */
					    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
						int lifesaved = 0;
						struct obj *wore_amulet = uamul;
			
						while(1) {
						    /* avoid looping on "die(y/n)?" */
						    if (lifesaved && (discover || wizard)) {
							if (wore_amulet && !uamul) {
							    /* used up AMULET_OF_LIFE_SAVING; still
							       subject to dying from brainlessness */
							    wore_amulet = 0;
							} else if (wizard) {
							    /* explicitly chose not to die;
							       arbitrarily boost intelligence */
							    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
							    You_feel("like a scarecrow.");
							    break;
							}
						    }
						    u.youaredead = 1;
						    u.youarereallydead = 1;

						    if (lifesaved)
							pline("Unfortunately your brain is still gone.");
						    else
							Your("last thought fades away.");
						    killer = "brainlessness";
						    killer_format = KILLED_BY;
						    done(DIED);
						    if (wizard) {
							    u.youaredead = 0;
							    u.youarereallydead = 0;
						    }
						    lifesaved++;
						}
					    }
					}
					/* adjattrib gives dunce cap message when appropriate */
					if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
					else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE, TRUE);
					if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
					if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
					exercise(A_WIS, FALSE);

						}
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena30;
						}
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == HIGH_HEELED_SANDAL && (tmp > rnd(20+i)) ) {
elena31:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("Your %s is hit painfully by the high heel!", body_part(HEAD));
					losehp(rnd(12),"a high-heeled sandal",KILLED_BY);
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena31;
						}
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == SEXY_LEATHER_PUMP && (tmp > rnd(20+i)) ) {
elena32:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("Klock! The heel slams on your %s, producing a beautiful sound.", body_part(HEAD));
					losehp(rnd(20),"a sexy leather pump",KILLED_BY);
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena32;
						}
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == INKA_BOOT && (tmp > rnd(20+i)) ) {
elena33:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("You have an orgasm because the soft inka leather is so lovely.");
					if (practicantterror) {
						pline("%s rings out: 'Wanking off is not permitted in my laboratory, but you know that. 200 zorkmids.'", noroelaname());
						fineforpracticant(200, 0, 0);
					}
					badeffect();
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena33;
						}
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == SOFT_LADY_SHOE && (tmp > rnd(20+i)) ) {
elena34:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("Ouch! Despite being incredibly soft, the lady shoe hit your optical nerve!");
					make_blinded(Blinded + rnd(30),FALSE);
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena34;
						}
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == STEEL_CAPPED_SANDAL && (tmp > rnd(20+i)) ) {
elena35:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("AAAAAHHHHH! Your %s screams as it gets hit by the massive, unyielding metal heel!", body_part(HEAD));
					make_stunned(HStun + rnd(100), FALSE);
						if (!rn2(10)) {
							pline("You're knocked out and helplessly drop to the floor.");
							nomovemsg = 0;	/* default: "you can move again" */
							if (StrongFree_action) nomul(-rnd(2), "knocked out by a steel-capped sandal", TRUE);
							else if (Free_action) nomul(-rnd(4), "knocked out by a steel-capped sandal", TRUE);
							else nomul(-rnd(8), "knocked out by a steel-capped sandal", TRUE);
						}
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena35;
						}
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == DOGSHIT_BOOT && (tmp > rnd(20+i)) ) {
elena36:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("Ulch! You're immersed with dog shit!");

					    register struct obj *objX, *objX2;
					    for (objX = invent; objX; objX = objX2) {
					      objX2 = objX->nobj;
						if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
						if (objX && !rn2(100)) wither_dmg(objX, xname(objX), 3, TRUE, &youmonst);

					    }
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena36;
						}

					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == SPIKED_BATTLE_BOOT && (tmp > rnd(20+i)) ) {
elena37:
					u.cnd_shoedamageamount++;
					if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
					pline("Ouch! The spiked boot soles bore themselves into your skin!");
					losehp(rnd(10),"a spiked battle boot",KILLED_BY);
				    if (!rn2(6))
					poisoned("spikes", A_STR, "poisoned boot spike", 8);
						if (FemtrapActiveElena && !rn2(3)) {
							pline("You long for more!");
							goto elena37;
						}
					}

/* The gnome swings her spiked battle boot! Ouch! The spiked boot soles bore themselves into your skin! The spikes were poisoned! The poison was deadly... Do you want your possessions identified? */

				}
				if((tmp > (j = dieroll = rnd(20+i))) || (uarmf && itemhasappearance(uarmf, APP_KOREAN_SANDALS) && !rn2(3) ) ) {
				    sum[i] = hitmu(mtmp, mattk);
					if (!rn2(75)) pushplayer(FALSE);
				}
				else
				    missmu(mtmp, tmp , j, mattk);
				/* KMH -- Don't accumulate to-hit bonuses */
				if (otmp)
					tmp -= hittmp;
			     } else wildmiss(mtmp, mattk);
			}
			break;
		case AT_MAGC:
			if (!rn2(iswarper ? 2 : 8)) /* yeah they need to be toned down a lot */{

			if (range2) {
			    if (!blue_on_blue(mtmp))
				sum[i] = buzzmu(mtmp, mattk);
			} else {
			    if (foundyou)
				sum[i] = castmu(mtmp, mattk, TRUE, TRUE);
			    else
				sum[i] = castmu(mtmp, mattk, TRUE, FALSE);
			}

			}

			/* random stats monsters and such can sometimes have weird damage types to go with this...
			 * they should do something meaningful with that damage type --Amy */
			if (mattk->adtyp != AD_SPEL && mattk->adtyp != AD_CAST && mattk->adtyp != AD_CLRC && mattk->adtyp != AD_FIRE && mattk->adtyp != AD_COLD && mattk->adtyp != AD_ACID && mattk->adtyp != AD_ELEC && mattk->adtyp != AD_DRST && mattk->adtyp != AD_LITE && mattk->adtyp != AD_SPC2 && mattk->adtyp != AD_DISN && mattk->adtyp != AD_MAGM) 
				goto cursesatyou;

			break;

		default:		/* no attack */
			break;
	    }
	    if(flags.botl) bot();
	/* give player a chance of waking up before dying -kaa */
	    if(sum[i] == 1) {	    /* successful attack */
		if (u.usleep && u.usleep < monstermoves && !rn2(have_sleepstone() ? 20 : 10)) {
		    multi = -1;
		    nomovemsg = "The combat suddenly awakens you.";
		}
	    }
	    if(sum[i] == 2) return 1;		/* attacker dead */
	    if(sum[i] == 3) break;  /* attacker teleported, no more attacks */
	    /* sum[i] == 0: unsuccessful attack */
	}

	if (mtmp->egotype_arcane && !rn2(iswarper ? 2 : 8) && !range2) {

	    if (foundyou) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[0];
		castmu(mtmp, a, TRUE, TRUE);
		}
	    else {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[0];
		castmu(mtmp, a, TRUE, FALSE);
		}

	}

	if (mtmp->egotype_clerical && !rn2(iswarper ? 2 : 8) && !range2) {

	    if (foundyou) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[1];
		castmu(mtmp, a, TRUE, TRUE);
		}
	    else {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[1];
		castmu(mtmp, a, TRUE, FALSE);
		}

	}

	if (mtmp->egotype_mastercaster && !rn2(iswarper ? 2 : 8) && !range2) {

	    if (foundyou) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		castmu(mtmp, a, TRUE, TRUE);
		}
	    else {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		castmu(mtmp, a, TRUE, FALSE);
		}

	}

	if (mtmp->egotype_thief ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SITM;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_disenchant ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ENCH;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_rust ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RUST;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_corrosion ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CORR;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_decay ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DCAY;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_flamer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FLAM;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_blasphemer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BLAS;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_dropper ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DROP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_wither ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_WTHR;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_grab ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STCK;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_faker ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FAKE;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_slows ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SLOW;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_vampire ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DRLI;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_teleportyou ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TLPT;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_wrap ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_WRAP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_disease ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DISE;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_slime ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SLIM;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_poisoner ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_POIS;
		a->damn = 1;
		a->damd = 1;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_elementalist ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_AXUS;
		a->damn = 1;
		a->damd = 1;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_acidspiller ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ACID;
		a->damn = 1;
		a->damd = 1;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_engrave ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NGRA;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_dark ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DARK;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_sounder ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SOUN;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_timer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TIME;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_thirster ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_THIR;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_nexus ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NEXU;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_gravitator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_GRAV;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_inert ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_INER;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_antimage ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MANA;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_unskillor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SKIL;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_venomizer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_VENO;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_dreameater ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DREA;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_nastinator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NAST;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_baddie ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BADE;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_sludgepuddle ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SLUD;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_vulnerator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_VULN;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_marysue ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FUMB;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_plasmon ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PLAS;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev * 3 / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_lasher ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_LASH;
		a->adtyp = AD_MALK;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_breather ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_BREA;
		a->adtyp = AD_RBRE;
		a->damn = (1 + (mtmp->m_lev / 4));
		a->damd = (1 + (mtmp->m_lev / 4));

		if (range2 && (rn2(2) || issoviet) && !blue_on_blue(mtmp) && (ZAP_POS(levl[u.ux][u.uy].typ) ) )
		    (void) breamu(mtmp, a);
		/* Note: breamu takes care of displacement */

	}

	if (mtmp->egotype_luck ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_LUCK;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_amnesiac ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_AMNE;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_seducer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SSEX;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_cullen ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_VAMP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_webber ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_WEBS;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_trapmaster ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TRAP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_itemporter ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STTP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_sinner ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SIN;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_schizo ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DEPR;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_aligner) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ALIN;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_destructor) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DEST;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_datadeleter) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_LASH;
		a->adtyp = AD_DATA;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_amberite || is_amberite(mtmp->data)) {

		if (!range2 && !rn2(20)) {
			pline("%s afflicts you with the Curse of Amber!", Monnam(mtmp));

			switch (rnd(27)) {
				case 1:
				case 2:
				case 3:
				case 4:
				case 5: /* aggravate monster */
					You_feel("that monsters are aware of your presence.");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
					aggravate();
					break;
				case 6:
				case 7:
				case 8: /* summon nasties */
					(void)nasty((struct monst *)0);
					break;
				case 9:
				case 10:
				case 11:
				case 12: /* summon vortices */
					if (Aggravate_monster) {
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
					}
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    if (!rn2(2)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    if (!rn2(5)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    if (!rn2(12)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			 	    if (!rn2(27)) (void) makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
					u.aggravation = 0;
					break;
				case 13:
				case 14:
				case 15: /* level drain */
					if(!Drain_resistance || !rn2(StrongDrain_resistance ? 15 : 4) )
					    losexp("amber drainage", FALSE, TRUE);
					break;
				case 16:
				case 17:
				case 18:
				case 19:
				case 20: /* paralysis: up to 3 turns with free action, up to 13 without */
					You_feel("like a statue!");
					if (StrongFree_action) nomul(-rnd(2), "paralyzed by the Curse of Amber", TRUE);
					else if (Free_action) nomul(-rnd(3), "paralyzed by the Curse of Amber", TRUE);
					else nomul(-rnd(13), "paralyzed by the Curse of Amber", TRUE);
					break;
				case 21:
				case 22:
				case 23: /* drain a random stat by 3 points */
					(void) adjattrib(rn2(A_MAX), -3, FALSE, TRUE);
					break;
				case 24: /* amnesia, magnitude 1-3 */
					forget(rnd(3));
					break;
				case 25: /* summon "cyberdemon" */
					{
					int attempts = 0;
					register struct permonst *ptrZ;

					if (Aggravate_monster) {
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
					}
newboss:
					do {

						ptrZ = rndmonst();
						attempts++;
						if (!rn2(2000)) reset_rndmonst(NON_PM);

					} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

					if (ptrZ && ptrZ->geno & G_UNIQ) {
						if (wizard) pline("monster generation: %s", ptrZ->mname);
						(void) makemon(ptrZ, u.ux, u.uy, NO_MM_FLAGS);
					}
					else if (rn2(50)) {
						attempts = 0;
						goto newboss;
					}
					pline("Boss monsters appear from nowhere!");

					}
					u.aggravation = 0;
					break;
				case 26:
				case 27: /* drain random stats by one, 50% chance for each of being affected */
					if (rn2(2)) (void) adjattrib(A_STR, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_INT, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_DEX, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_CHA, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_CON, -1, FALSE, TRUE);
					if (rn2(2)) (void) adjattrib(A_WIS, -1, FALSE, TRUE);
					break;
			}

		}

	}

	if (mtmp->egotype_trembler) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TREM;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_worldender) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RAGN;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_damager) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_IDAM;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_antitype) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ANTI;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_statdamager) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STAT;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_damagedisher) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DAMA;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_sanitizer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SANI;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_nastycurser) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NACU;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_thiefguildmember) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_THIE;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_badowner) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RBAD;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_bleeder) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BLEE;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_shanker) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SHAN;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_terrorizer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TERR;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_feminizer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FEMI;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_levitator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_LEVI;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_illusionator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ILLU;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_rogue) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SEDU;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_stealer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SAMU;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_stoner) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STON;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_painlord) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PAIN;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_empmaster) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TECH;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_spellsucker) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MEMO;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_eviltrainer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TRAI;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_maecke) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MCRE;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_contaminator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CONT;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_reactor) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_HUGS;
		a->adtyp = AD_CONT;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if((!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
					!touch_petrifies(youmonst.data))) || (!rn2(20) && ((dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= (BOLT_LIM * BOLT_LIM)) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) ) ) ) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (FemtrapActiveNelly && humanoid(mtmp->data) && is_female(mtmp->data)) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_HUGS;
		a->adtyp = AD_PHYS;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if((!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
					!touch_petrifies(youmonst.data))) || (!rn2(20) && ((dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= (BOLT_LIM * BOLT_LIM)) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) ) ) ) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (FemtrapActiveKristina && humanoid(mtmp->data) && is_female(mtmp->data)) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BURN;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (FemtrapActiveAnita && humanoid(mtmp->data) && is_female(mtmp->data)) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_KICK;
		a->adtyp = AD_BLEE;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (FemtrapActiveMelissa && mtmp->female && humanoid(mtmp->data)) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = (is_female(mtmp->data) ? AD_SEDU : AD_SITM);
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (FemtrapActiveAlmut && humanoid(mtmp->data) && is_female(mtmp->data)) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_KICK;
		a->adtyp = AD_GLIB;
		a->damn = 1;
		a->damd = 1;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (uarmf && uarmf->oartifact == ART_DO_NOT_PEE && !range2 && foundyou && (tmp > (j = rnd(20+i))) && humanoid(mtmp->data) && is_male(mtmp->data)) {

		if (!rn2(3)) {
			pline("%s pees at your boots!", Monnam(mtmp));
			if (rn2(100)) pline("But thankfully you manage to dodge.");
			else {
				if (rn2(2)) {
					if (uarmf && uarmf->oeroded < MAX_ERODE) uarmf->oeroded++;
					pline("Nooooooo! Your pretty boots have been defiled!");
				} else {
					if (uarmf && uarmf->oeroded2 < MAX_ERODE) uarmf->oeroded2++;
					pline("Nooooooo! Your pretty boots have been defiled!");
				}
			}

		}

	}

	if (mtmp->egotype_radiator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_BREA;
		a->adtyp = AD_CONT;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if ( (rnd(5) > 3) && ( (tmp > (rnd(20+i))) || (tmp > (rnd(20+i))) ) && lined_up(mtmp) && ((dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM) || (elongation_monster(mtmp->data) || ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) ) )
		{
		    if (foundyou) {
			(void) hitmu(mtmp, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_minator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MINA;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_aggravator) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_AGGR;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_midiplayer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MIDI;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_rngabuser ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RNG;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_watersplasher ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = level.flags.lethe ? AD_LETH : AD_WET;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_cancellator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CNCL;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_banisher ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BANI;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_shredder ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SHRD;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_abductor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ABDC;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_incrementor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CHKH;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_mirrorimage ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_HODS;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_curser ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CURS;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_horner ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = rn2(1000) ? AD_CHRN : AD_UVUU;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_push ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DISP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_randomizer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RBRE;
		a->damn = 2;
		a->damd = 1 + (mtmp->m_lev / 2);

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_blaster) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TENT;
		a->adtyp = AD_DRIN;
		a->damn = 1;
		a->damd = 1;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_psychic) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SPC2;
		a->damn = 2;
		a->damd = 1 + (mtmp->m_lev / 2);

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_abomination) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_GAZE;
		a->adtyp = AD_SPC2;
		a->damn = 2;
		a->damd = 1 + (mtmp->m_lev / 2);
		gazemu(mtmp, a);
	}

	if (uarmf && itemhasappearance(uarmf, APP_CHRISTMAS_CHILD_MODE_BOOTS) && dmgtype(mtmp->data, AD_NIVE)) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_GAZE;
		a->adtyp = AD_FEAR;
		a->damn = 2;
		a->damd = 1 + (mtmp->m_lev / 2);
		gazemu(mtmp, a);
	}

	if (mtmp->egotype_weeper) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_GAZE;
		a->adtyp = AD_CONT;
		a->damn = 2;
		a->damd = 1 + (mtmp->m_lev / 2);
		gazemu(mtmp, a);

	}

	if (mtmp->egotype_multiplicator) {

		if (!range2 && (!rn2(5)) ) {
		    pline("%s multiplies!",Monnam(mtmp) );
		    clone_mon(mtmp, 0, 0);
		}

	}

	if (evilfriday && mtmp->data->mlet == S_GIANT) { /* evil patch idea by jonadab */
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_HUGS;
		a->adtyp = AD_PHYS;
		a->damn = 2;
		a->damd = 1 + (mtmp->m_lev / 2);

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}
	}

	if (evilfriday && mtmp->data->mlet == S_ZOMBIE) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DISE;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}
	}

	if (evilfriday && (is_mplayer(mtmp->data) || is_umplayer(mtmp->data))) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SAMU;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (evilfriday && mtmp->data->mlet == S_MUMMY) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = rn2(20) ? AD_ICUR : AD_NACU;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}
	}

	if (mtmp->data == &mons[PM_EVIL_RAPIST] && flags.female) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_INER;
		a->damn = 2;
		a->damd = 4;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
				You_feel("violated!");
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (evilfriday && mtmp->data->mlet == S_GHOST) {
		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict || !touch_petrifies(youmonst.data))) {
			if (foundyou && tmp > (j = rnd(20+i))) {
				pline("%s ages you!", Monnam(mtmp));
			      u_slow_down();
				u.uprops[DEAC_FAST].intrinsic += ((mtmp->m_lev + 2) * 4);
				u.inertia += (mtmp->m_lev + 2);
				monstermoves += (mtmp->m_lev + 2);
				moves += (mtmp->m_lev + 2);
			}
		}
	}

	if (uarmu && uarmu->oartifact == ART_GIVE_ME_STROKE__JO_ANNA && humanoid(mtmp->data) && is_female(mtmp->data) && !rn2(10) ) {
		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_HEAL;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
				(void) hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (Role_if(PM_FJORDE) && mtmp->data->mlet == S_EEL) {
		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict || !touch_petrifies(youmonst.data))) {
			if (foundyou && !rn2(5) && tmp > (j = rnd(20+i))) {
				pline("%s nibbles you in a sexually arousing way!", Monnam(mtmp));
				u.tremblingamount++;
			}
		}
	}

	if (uimplant && uimplant->oartifact == ART_POTATOROK && !range2 && foundyou && !rn2(10) && (tmp > (j = rnd(20+i)))) {
		ragnarok(FALSE);
		if (evilfriday && mtmp->m_lev > 1) evilragnarok(FALSE,mtmp->m_lev);
	}

	if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON && !range2 && foundyou && !rn2(10) && (tmp > (j = rnd(20+i)))) {
		ragnarok(FALSE);
		if (evilfriday && mtmp->m_lev > 1) evilragnarok(FALSE,mtmp->m_lev);
	}

	if (uswapwep && uswapwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON && !range2 && foundyou && !rn2(10) && (tmp > (j = rnd(20+i)))) {
		ragnarok(FALSE);
		if (evilfriday && mtmp->m_lev > 1) evilragnarok(FALSE,mtmp->m_lev);
	}

	/* bugsniper wants the BOFH to have data delete; that would be extremely unfair, so I'm restraining it to the
	 * evil variant because after all SLEX is supposed to be a balanced game (yeah I know "ha ha", but I guess players
	 * who consider the concept of "you can lose the game through no fault of your own" to be innately unbalanced
	 * will not consider SLEX a balanced game. It is balanced around the fact that you can die unfairly :P) --Amy */
	if (mtmp->data == &mons[PM_BOFH] && isevilvariant) {
		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			u.datadeletedefer = 1;
			datadeleteattack();
		}
	}
	if (mtmp->data == &mons[PM_ALPHANUMEROGORGON] && isevilvariant) {
		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			u.datadeletedefer = 1;
			datadeleteattack();
		}
	}
	if (mtmp->data == &mons[PM_MR__CONCLUSIO] && isevilvariant) {
		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			u.datadeletedefer = 1;
			datadeleteattack();
		}
	}
	if (mtmp->data == &mons[PM_YOUR_GAME_ENDS_NOW] && isevilvariant) {
		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			u.datadeletedefer = 1;
			datadeleteattack();
		}
	}
	if (mtmp->data == &mons[PM_LEFTOGORGON] && isevilvariant) {
		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			u.datadeletedefer = 1;
			datadeleteattack();
		}
	}

	if (mtmp->data == &mons[PM_TAUNTBUGGER]) {
		mtmp->minvis = TRUE;
		mtmp->perminvis = TRUE;
	}

	/* ultra-mega laser cannon for some specific ubermonsters, including the elder priest */
	if (mtmp->data == &mons[PM_DHWTY] || mtmp->data == &mons[PM_PARTICLE_MAN] || mtmp->data == &mons[PM_LAST_DANCER] || mtmp->data == &mons[PM_CURTAIN_CALL_LAST_DANCER] || mtmp->data == &mons[PM_GRAND_FINALE_LAST_DANCER] || mtmp->data == &mons[PM_PROVIDENCE_GAZE] || mtmp->data == &mons[PM_CHAOS_SERPENT] || mtmp->data == &mons[PM_CHAOTIC_SERPENT] || mtmp->data == &mons[PM_ARCHAYEEK_GUNNER] || mtmp->data == &mons[PM_SIN_GORILLA] || mtmp->data == &mons[PM_ELITE_GUARD] || mtmp->data == &mons[PM_ELITE_CROUPIER] || mtmp->data == &mons[PM_BLUEBEAM_GOLEM] || mtmp->data == &mons[PM_AIRTIGHT_FEMMY] || mtmp->data == &mons[PM_DRICERADOPS] || mtmp->data == &mons[PM_SVEN] || mtmp->data == &mons[PM_GRANDMASTER_SVEN] || mtmp->data == &mons[PM_WORLD_PWNZOR] || mtmp->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_] || mtmp->data == &mons[PM_SANDRA_S_MINDDRILL_SANDAL] || mtmp->egotype_laserpwnzor) {
		if (range2 && lined_up(mtmp) && !blue_on_blue(mtmp) && (ZAP_POS(levl[u.ux][u.uy].typ) ) ) {
			if (!mtmp->hominglazer && !rn2(20)) {
				pline("ATTENTION: %s has started to load an ultra-mega-hyper-dyper laser cannon!", Monnam(mtmp));
				mtmp->hominglazer = 1;
			} else if (mtmp->hominglazer >= 20) {
				pline("ZIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEIEtschhhhhhhhhhhhhhhhhhhhhhhh...");
				losehp(rnz(mtmp->m_lev * 20), "an ultra-mega-hyper-dyper laser cannon", KILLED_BY);

			/* fire some beams; the hominglazer flag also makes the monster reflecting so it won't kill itself */
				u.uprops[DEAC_REFLECTING].intrinsic += rnd(5);
				mdat2 = &mons[PM_CAST_DUMMY];
				a = &mdat2->mattk[3];
				a->aatyp = AT_BREA;
				a->adtyp = AD_DISN;
				a->damn = (1 + mtmp->m_lev);
				a->damd = (1 + mtmp->m_lev);
				(void) breamu(mtmp, a);

				a->adtyp = AD_LITE;
				(void) breamu(mtmp, a);

				mtmp->hominglazer = 0;
			}
		}

	}

	if (mtmp->data == &mons[PM_GIANT_ENEMY_CRAB]) {
		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			if (!rn2(20) && !bigmonst(youmonst.data) && !StrongDiminishedBleeding && !Invulnerable && !(Stoned_chiller && Stoned) ) {
				pline("Bad luck - the giant enemy crab bisects you. Goodbye.");
				losehp(2 * (Upolyd ? u.mh : u.uhp) + 200, "being bisected by a giant enemy crab",KILLED_BY);
			}
		}
	}

	/* and now, the unholy satanic motherfucker from hell, aka the most evil monster in existence... --Amy
	 * thanks Chris_ANG for creating it, you evil person :P */
	if (mtmp->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_] || mtmp->data == &mons[PM_SANDRA_S_MINDDRILL_SANDAL]) {

		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			if (!rn2(20) && !bigmonst(youmonst.data) && !StrongDiminishedBleeding && !Invulnerable && !(Stoned_chiller && Stoned) ) {
				pline("Bad luck - the elder priest bisects you. Goodbye.");
				losehp(2 * (Upolyd ? u.mh : u.uhp) + 200, "being bisected by the elder priest",KILLED_BY);
			}
		}

		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			if (!rn2(20) && has_head(youmonst.data) && !Role_if(PM_COURIER) ) {
				pline("Bad luck - the elder priest decapitates you. Goodbye.");
				losehp(2 * (Upolyd ? u.mh : u.uhp) + 200, "being decapitated by the elder priest",KILLED_BY);
			}

		}

		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			ragnarok(FALSE);
			if (evilfriday && mtmp->m_lev > 1) evilragnarok(FALSE,mtmp->m_lev);
		}

		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			u.datadeletedefer = 1;
			datadeleteattack();
		}

		if(!range2 && foundyou && (tmp > (j = rnd(20+i)))) {
			pline("The elder priest tentacles to tentacle you! OH NO!!!");

			if(!uarmc && !uarm) {
				if(uarmu) {
					You_feel("the tentacles squirm under your shirt.");
					if( rnd(100) > 15) {
						pline("The tentacles begin to tear at your shirt!");
						if(uarmu->spe > 1){
							for(i=rn2(4); i>=0; i--)
								drain_item_severely(uarmu);
								Your("%s less effective.", aobjnam(uarmu, "seem"));
								u.cnd_disenchantamount++;
								if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
						}
						else{
							(void) destroy_arm(uarmu);
							verbalize("Harharhar you stupid player, you've been playing for long enough! Say goodbye!");
						}
					}
					else{
						pline("The tentacles pull your shirt off!");
						otmp = uarmu;
						if (donning(otmp)) cancel_don();
						(void) Shirt_off();
						freeinv(otmp);
						(void) mpickobj(mtmp,otmp,FALSE);
					}
				}
			}

			if(!uarmc) {
				if(uarm){
					You_feel("the tentacles squirm under your armor.");
					if( rnd(100) > 25){
						pline("The tentacles begin to tear at your armor!");
						if(uarm->spe > 1){
							for(i=rn2(4); i>=0; i--)
								drain_item_severely(uarm);
								Your("%s less effective.", aobjnam(uarm, "seem"));
								u.cnd_disenchantamount++;
								if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
						 }
						 else{
							(void) destroy_arm(uarm);
							verbalize("Harharhar you stupid player, you've been playing for long enough! Say goodbye!");
						 }
					}
					else{
						pline("The tentacles shuck you out of your armor!");
						otmp = uarm;
						if (donning(otmp)) cancel_don();
						(void) Armor_gone();
						freeinv(otmp);
						(void) mpickobj(mtmp,otmp,FALSE);
					}
				}
			}

			if(uarmc){
				You_feel("the tentacles work their way under your cloak.");
				if( rnd(100) > 66){
					pline("The tentacles begin to tear at the cloak!");
					if(uarmc->spe > 1){
						for(i=rn2(4); i>=0; i--)
							drain_item_severely(uarmc);
							Your("%s less effective.", aobjnam(uarmc, "seem"));
							u.cnd_disenchantamount++;
							if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
				 	}
					else{
						(void) destroy_arm(uarmc);
						verbalize("Harharhar you stupid player, you've been playing for long enough! Say goodbye!");
				 	}
				}
				else{
					pline("The tentacles strip off your cloak!");
					otmp = uarmc;
					if (donning(otmp)) cancel_don();
					(void) Cloak_off();
					freeinv(otmp);
					(void) mpickobj(mtmp,otmp,FALSE);
				}
			}

			register int dmg = d(15,15);

			switch(d(1,12)){
			case 1:
			if(uarmf){
				You_feel("the tentacles squirm into your boots.");
				if( rnd(100) > 66){
					pline("The tentacles begin to tear at your boots!");
					if(uarmf->spe > 1){
						for(i=rn2(4); i>=0; i--)
							drain_item_severely(uarmf);
							Your("%s less effective.", aobjnam(uarmf, "seem"));
							u.cnd_disenchantamount++;
							if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
					}
					else{
						(void) destroy_arm(uarmf);
						verbalize("Harharhar you stupid player, you've been playing for long enough! Say goodbye!");
					}
				}
				else{
					pline("The tentacles suck off your boots!");
					otmp = uarmf;
					if (donning(otmp)) cancel_don();
					(void) Boots_off();
					freeinv(otmp);
					(void) mpickobj(mtmp,otmp,FALSE);
				}
			}
			break;
			case 2:
			if(uwep){
				You_feel("the tentacles wrap around your weapon.");
				if( rnd(130) > ACURR(A_STR)){
					pline("The tentacles yank your weapon out of your grasp!");
					otmp = uwep;
					uwepgone();
					freeinv(otmp);
					(void) mpickobj(mtmp,otmp,FALSE);
				 }
				 else{
					/* dnethack wouldn't do anything here, but this is the Evil Patch(TM) - disenchant weapon */

					if (uwep->spe > -20) {
						drain_item_severely(uwep);
						drain_item_severely(uwep);
						drain_item_severely(uwep);
						drain_item_severely(uwep);
						drain_item_severely(uwep);
						Your("%s less effective.", aobjnam(uwep, "seem"));
						u.cnd_disenchantamount++;
						if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
					}
				 }
			}
			break;
			case 3:
			if(uarmg){
				You_feel("the tentacles squirm into your gloves.");
				if( rnd(40) <= ACURR(A_STR) || uwep){
					pline("The tentacles begin to tear at your gloves!");
					if(uarmg->spe > 1){
						for(i=rn2(4); i>=0; i--)
							drain_item_severely(uarmg);
							Your("%s less effective.", aobjnam(uarmg, "seem"));
							u.cnd_disenchantamount++;
							if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
					}
					else{
						(void) destroy_arm(uarmg);
						verbalize("Harharhar you stupid player, you've been playing for long enough! Say goodbye!");
					}
				}
				else{
					pline("The tentacles suck your gloves off!");
					otmp = uarmg;
					if (donning(otmp)) cancel_don();
					(void) Gloves_off();
					freeinv(otmp);
					(void) mpickobj(mtmp,otmp,FALSE);
				}
			}
			break;
			case 4:
			if(uarms){
				You_feel("the tentacles wrap around your shield.");
				if( rnd(150) > ACURR(A_STR)){
					pline("The tentacles pull your shield out of your grasp!");
					otmp = uarms;
					if (donning(otmp)) cancel_don();
					Shield_off();
					freeinv(otmp);
					(void) mpickobj(mtmp,otmp,FALSE);
				 }
				 else{

					/* dnethack wouldn't do anything here, but this is the Evil Patch(TM) - disenchant shield */

					if (uarms->spe > -20) {
						drain_item_severely(uarms);
						drain_item_severely(uarms);
						drain_item_severely(uarms);
						drain_item_severely(uarms);
						drain_item_severely(uarms);
						Your("%s less effective.", aobjnam(uarms, "seem"));
						u.cnd_disenchantamount++;
						if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
					}
				 }
			}
			break;
			case 5:
			if(uarmh){
				You_feel("the tentacles squirm under your helmet.");
				if( rnd(100) > 90){
					pline("The tentacles begin to tear at your helmet!");
					if(uarmh->spe > 1){
						for(i=rn2(4); i>=0; i--)
							drain_item_severely(uarmh);
							Your("%s less effective.", aobjnam(uarmh, "seem"));
							u.cnd_disenchantamount++;
							if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
					}
					else{
						(void) destroy_arm(uarmh);
						verbalize("Harharhar you stupid player, you've been playing for long enough! Say goodbye!");
					}
				}
				else{
					pline("The tentacles pull your helmet off!");
					otmp = uarmh;
					if (donning(otmp)) cancel_don();
					(void) Helmet_off();
					freeinv(otmp);
					(void) mpickobj(mtmp,otmp,FALSE);
				}
			}
			break;
			case 6:
				if(u.uenmax == 0) {
					u.youaredead = 1;
					/* dnethack wouldn't do anything here, but this is the Evil Patch(TM) - instadeath! */
					You_feel("little mouths sucking on your exposed skin... and scream in agony as your body is sucked away completely.");

					killer = "being sucked into the elder priest's belly and digested whole";
					killer_format = KILLED_BY;
					done(DIED);
					u.youaredead = 0;
					break;
				}
				if(uarmc || uarm || uarmu){
					You_feel("the tentacles sucking on your %s", uarm ? "armor" : "clothes");

					if (!rn2(5)) { /* stealstealsteal :P */

						if (uarmc) {
							pline("The tentacles strip off your cloak!");
							otmp = uarmc;
							if (donning(otmp)) cancel_don();
							(void) Cloak_off();
							freeinv(otmp);
							(void) mpickobj(mtmp,otmp,FALSE);
						} else if (uarm) {
							pline("The tentacles shuck you out of your armor!");
							otmp = uarm;
							if (donning(otmp)) cancel_don();
							(void) Armor_gone();
							freeinv(otmp);
							(void) mpickobj(mtmp,otmp,FALSE);
						} else if (uarmu) {
							pline("The tentacles pull your shirt off!");
							otmp = uarmu;
							if (donning(otmp)) cancel_don();
							(void) Shirt_off();
							freeinv(otmp);
							(void) mpickobj(mtmp,otmp,FALSE);
						}

					}

				break;  //blocked
				} //else
				You_feel("little mouths sucking on your exposed %s.",body_part(STOMACH));
				u.uen = 0;

				/* replace screwy non-integer dnethack calculations with normal ones --Amy */
				if (StrongHalf_physical_damage) u.uenmax -= (u.uenmax / 8);
				else if (Half_physical_damage) u.uenmax -= (u.uenmax / 4);
				else u.uenmax -= (u.uenmax / 2);
				if (u.uenmax < 0) u.uenmax = 0;
			break;
			case 7:
				if(uarmh) {
					You_feel("the tentacles squirm over your helmet this sentence is missing a period");

					if (!rn2(5)) { /* stealstealsteal :P */
						pline("The tentacles pull your helmet off!");
						otmp = uarmh;
						if (donning(otmp)) cancel_don();
						(void) Helmet_off();
						freeinv(otmp);
						(void) mpickobj(mtmp,otmp,FALSE);
					}

				break; //blocked
				} //else
				You_feel("the tentacles bore into your skull!");
				i = d(1,6);
				(void) adjattrib(A_INT, -i, 1, TRUE);
				while(i-- > 0){
					if (!rn2(2)) losexp("brain damage",FALSE,TRUE);
					forget(10);	/* lose 10% of memory per point lost*/
					exercise(A_WIS, FALSE);
				}
				//begin moved brain removal messages
				Your("brain is cored like an apple!");
				if (ABASE(A_INT) <= 3) {
					u.youaredead = 1;
					u.youarereallydead = 1;
					Your("last thought fades away.");
					killer = "destruction of the brain and spinal cord";
					killer_format = KILLED_BY;
					done(DIED);
		
					/* player still alive somehow? kill them again :P */
		
					pline("Unfortunately your brain is still gone.");
					killer = "destruction of the brain and spinal cord";
					killer_format = KILLED_BY;
					done(DIED);

					if (!wizard) {
						pline("Unfortunately your brain is STILL gone. Your game ends here.");
						done(ESCAPED);
					}

					u.youaredead = 0;
					u.youarereallydead = 0;

					ABASE(A_INT) = ATTRMIN(A_INT);
					You_feel("like a scarecrow.");
					AMAX(A_INT) = ABASE(A_INT);
				}
				losehp(StrongHalf_physical_damage ? dmg/4 + 1 : Half_physical_damage ? dmg/2 + 1 : dmg, "head trauma", KILLED_BY);
				
			break;
			case 8:
				if(uarmc || uarm){
					You_feel("a tentacle squirm over your %s.", uarmc ? "cloak" : "armor");

					if (!rn2(5)) { /* stealstealsteal :P */

						if (uarmc) {
							pline("The tentacles strip off your cloak!");
							otmp = uarmc;
							if (donning(otmp)) cancel_don();
							(void) Cloak_off();
							freeinv(otmp);
							(void) mpickobj(mtmp,otmp,FALSE);
						} else if (uarm) {
							pline("The tentacles shuck you out of your armor!");
							otmp = uarm;
							if (donning(otmp)) cancel_don();
							(void) Armor_gone();
							freeinv(otmp);
							(void) mpickobj(mtmp,otmp,FALSE);
						}

					}

				break;  //blocked
				} //else
				You_feel("the tentacles drill through your unprotected %s and into your soul!",body_part(HEAD));
				if (!Drain_resistance || !rn2(StrongDrain_resistance ? 20 : 5)) {
					losexp("soul-shreding tentacles",FALSE,FALSE);
					losexp("soul-shreding tentacles",FALSE,FALSE);
					losexp("soul-shreding tentacles",FALSE,FALSE);
					i = d(1,4);
					while(i-- > 0){
						losexp("soul-shreding tentacles",FALSE,TRUE);
						exercise(A_WIS, FALSE);
						exercise(A_WIS, FALSE);
						exercise(A_WIS, FALSE);
					}
					(void) adjattrib(A_CON, -4, 1, TRUE);
					You_feel("violated and very fragile. Your soul seems a thin and tattered thing.");
				} else {
					(void) adjattrib(A_CON, -2, 1, TRUE);
					You_feel("a bit fragile, but strangly whole.");
				}
				losehp(StrongHalf_physical_damage ? dmg/8+1 : Half_physical_damage ? dmg/4+1 : dmg/2+1, "drilling tentacles", KILLED_BY);
			break;
			case 9:
				if(uarmc || uarm) {
					You_feel("the tentacles press into your %s once again someone forgot to close the sentence with a period", uarmc ? "cloak" : "armor");

					if (!rn2(5)) { /* stealstealsteal :P */

						if (uarmc) {
							pline("The tentacles strip off your cloak!");
							otmp = uarmc;
							if (donning(otmp)) cancel_don();
							(void) Cloak_off();
							freeinv(otmp);
							(void) mpickobj(mtmp,otmp,FALSE);
						} else if (uarm) {
							pline("The tentacles shuck you out of your armor!");
							otmp = uarm;
							if (donning(otmp)) cancel_don();
							(void) Armor_gone();
							freeinv(otmp);
							(void) mpickobj(mtmp,otmp,FALSE);
						}

					}

				break; //blocked
				} //else
				You_feel("the tentacles spear into your unarmored body!");
				losehp(StrongHalf_physical_damage ? dmg/2+1 : Half_physical_damage ? dmg : 4*dmg, "impaled by tentacles", NO_KILLER_PREFIX);
				(void) adjattrib(A_STR, -6, 1, TRUE);
				(void) adjattrib(A_CON, -3, 1, TRUE);
				You_feel("weak and helpless in their grip!");
			break;
			case 10:
			case 11:
			case 12:
				if(uarmc) {
					You_feel("the tentacles writhe over your cloak.");

					if (!rn2(5)) { /* stealstealsteal :P */

						pline("The tentacles strip off your cloak!");
						otmp = uarmc;
						if (donning(otmp)) cancel_don();
						(void) Cloak_off();
						freeinv(otmp);
						(void) mpickobj(mtmp,otmp,FALSE);

					}

					break; /* blocked */
				} //else

					/* dnethack would only steal things if you were wearing absolutely no armor and had no weapon
					 * but this is the Evil Patch(TM), and players looooooove having their items stolen! --Amy */
				if(invent){
					You_feel("the tentacles pick through your remaining possessions.");
					buf[0] = '\0';
					steal(mtmp, buf, TRUE, FALSE);
					buf[0] = '\0';
					steal(mtmp, buf, TRUE, FALSE);
					buf[0] = '\0';
					steal(mtmp, buf, TRUE, FALSE);
					buf[0] = '\0';
					steal(mtmp, buf, TRUE, FALSE);
				}
			break;
		}

		}

	}

	/* Bomber egotype must come last, because the monster dies and should no longer get attacks after exploding. --Amy */
	if (mtmp->egotype_bomber ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[3];
		a->aatyp = AT_EXPL;
		a->adtyp = AD_PHYS;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev * 5));

		if(!range2) {
			(void) explmu(mtmp, a, foundyou);
			return(0);
		}

	}

	return(0);
}

#endif /* OVL0 */
#ifdef OVLB

/*
 * helper function for some compilers that have trouble with hitmu
 */

void
hurtarmor(attk)
int attk;
{
	int	hurt;

	switch(attk) {
	    /* 0 is burning, which we should never be called with */
		/* Amy edit: AD_LAVA does now. */
	    case AD_RUST: hurt = 1; break;
	    case AD_CORR: hurt = 3; break;
	    case AD_LAVA: hurt = 0; break;
	    case AD_FLAM: hurt = 0; break;
	    default: hurt = 2; break;
	}

	/* What the following code does: it keeps looping until it
	 * finds a target for the rust monster.
	 * Head, feet, etc... not covered by metal, or covered by
	 * rusty metal, are not targets.  However, your body always
	 * is, no matter what covers it.
	 *
	 * WAC fixed code so that it keeps looping until it either hits
	 * your body or finds a rustable item
	 * changed the last parm of !rust_dmg for non-body targets to FALSE
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !rust_dmg(uarmh, xname(uarmh), hurt, FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)rust_dmg(uarmc, xname(uarmc), hurt, TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)rust_dmg(uarm, xname(uarm), hurt, TRUE, &youmonst);
		else if (uarmu)
		    (void)rust_dmg(uarmu, xname(uarmu), hurt, TRUE, &youmonst);
		break;
	    case 2:
		if (!uarms || !rust_dmg(uarms, xname(uarms), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !rust_dmg(uarmg, xname(uarmg), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !rust_dmg(uarmf, xname(uarmf), hurt, FALSE, &youmonst))
		    continue;
		break;
	    }
	    
	    break; /* Out of while loop */
	}
}

void
witherarmor()
{

	/* What the following code does: it keeps looping until it
	 * finds a target for the rust monster.
	 * Head, feet, etc... not covered by metal, or covered by
	 * rusty metal, are not targets.  However, your body always
	 * is, no matter what covers it.
	 *
	 * WAC fixed code so that it keeps looping until it either hits
	 * your body or finds a rustable item
	 * changed the last parm of !rust_dmg for non-body targets to FALSE
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !wither_dmg(uarmh, xname(uarmh), rn2(4), FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)wither_dmg(uarmc, xname(uarmc), rn2(4), TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)wither_dmg(uarm, xname(uarm), rn2(4), TRUE, &youmonst);
		else if (uarmu)
		    (void)wither_dmg(uarmu, xname(uarmu), rn2(4), TRUE, &youmonst);
		break;
	    case 2:
		if (!uarms || !wither_dmg(uarms, xname(uarms), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !wither_dmg(uarmg, xname(uarmg), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !wither_dmg(uarmf, xname(uarmf), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    }
	    
	    break; /* Out of while loop */
	}
}

#endif /* OVLB */
#ifdef OVL1

STATIC_OVL boolean
diseasemu(mdat)
struct permonst *mdat;
{
	if (IntSick_resistance || (ExtSick_resistance && rn2(20)) || !rn2(10) ) { /* small chance to not get infected even if not resistant --Amy */
		You_feel("a slight illness.");
		return FALSE;
	} else {
		make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			mdat->mname, TRUE, SICK_NONVOMITABLE);
		return TRUE;
	}
}

/* check whether slippery clothing protects from hug or wrap attack */
STATIC_OVL boolean
u_slip_free(mtmp, mattk)
struct monst *mtmp;
struct attack *mattk;
{
	struct obj *obj = (uarmc ? uarmc : uarm);

	if (!obj) obj = uarmu;
	if (mattk->adtyp == AD_DRIN) obj = uarmh;

	/* if your cloak/armor is greased, monster slips off; this
	   protection might fail (33% chance) when the armor is cursed */
	if (obj && (obj->greased || obj->otyp == OILSKIN_CLOAK || obj->oartifact == ART_PREMIUM_VISCOSITY || obj->oartifact == ART_LAURA_S_SWIMSUIT) && rn2(50) && /* low chance to fail anyway --Amy */
		(!obj->cursed || rn2(3))) {
	    pline("%s %s your %s %s!",
		  Monnam(mtmp),
		  (mattk->adtyp == AD_WRAP) ?
			"slips off of" : "grabs you, but cannot hold onto",
		  obj->greased ? "greased" : "slippery",
		  /* avoid "slippery slippery cloak"
		     for undiscovered oilskin cloak */
		  (obj->greased || objects[obj->otyp].oc_name_known) ?
			xname(obj) : cloak_simple_name(obj));

	    if (obj->greased && (!rn2(2) || (isfriday && !rn2(2)))) {
		pline_The("grease wears off.");
		obj->greased -= 1;
		update_inventory();
	    }
	    return TRUE;
	/* 50% chance (with a luck bonus) of slipping free with free action */
	} else if (Free_action && (rnl(10) < 5)) {
                pline("%s %s you, but you quickly free yourself!",
                       Monnam(mtmp),
                       (mattk->adtyp == AD_WRAP) ?
                      "swings itself around of" : "grabs");
                return TRUE;
	} else if (StrongFree_action && (rnl(10) < 5)) {
                pline("%s %s you, but you quickly free yourself!",
                       Monnam(mtmp),
                       (mattk->adtyp == AD_WRAP) ?
                      "swings itself around of" : "grabs");
                return TRUE;
	} else if (uarmg && itemhasappearance(uarmg, APP_SLIPPERY_GLOVES) && rn2(10) ) {
                pline("%s %s you, but your slippery gloves allow you to slip free!",
                       Monnam(mtmp),
                       (mattk->adtyp == AD_WRAP) ?
                      "swings itself around of" : "grabs");
                return TRUE;
	} else if (RngeSlipping && rn2(10) ) {
                pline("%s %s you, but your slipperyness allows you to slip free!",
                       Monnam(mtmp),
                       (mattk->adtyp == AD_WRAP) ?
                      "swings itself around of" : "grabs");
                return TRUE;
	}
	return FALSE;
}

/* armor that sufficiently covers the body might be able to block magic */
int
magic_negation(mon)
struct monst *mon;
{
	struct obj *armor;
	int armpro = 0;

	armor = (mon == &youmonst) ? uarm : which_armor(mon, W_ARM);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmc : which_armor(mon, W_ARMC);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmh : which_armor(mon, W_ARMH);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

	/* armor types for shirt, gloves, shoes, and shield don't currently
	   provide any magic cancellation but we might as well be complete */
	armor = (mon == &youmonst) ? uarmu : which_armor(mon, W_ARMU);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmg : which_armor(mon, W_ARMG);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmf : which_armor(mon, W_ARMF);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarms : which_armor(mon, W_ARMS);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

	/* this one is really a stretch... */
	armor = (mon == &youmonst) ? 0 : which_armor(mon, W_SADDLE);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

	if (bmwride(ART_MACAN_STRETCH) && armpro < 3) armpro = 3;
	if (uarmg && uarmg->oartifact == ART_EGASSO_S_GIBBERISH && armpro < 5) armpro = 5;

	if (mon == &youmonst) {
		if (MCReduction) armpro -= (1 + (MCReduction / 5000));
		if (u.magicshield) armpro += 1;
		if (Race_if(PM_GERTEUT)) armpro++;
		if (uarm && uarm->oartifact == ART_MITHRAL_CANCELLATION) armpro++;
		if (uarm && uarm->oartifact == ART_FREE_EXTRA_CANCEL) armpro++;
		if (uarm && uarm->oartifact == ART_IMPRACTICAL_COMBAT_WEAR) armpro++;
		if (uarmc && uarmc->oartifact == ART_RESISTANT_PUNCHING_BAG) armpro++;
		if (uleft && uleft->otyp == RIN_THREE_POINT_SEVEN_PROTECTI) armpro++;
		if (uright && uright->otyp == RIN_THREE_POINT_SEVEN_PROTECTI) armpro++;
		if (uamul && uamul->otyp == AMULET_OF_GUARDING) armpro++;
		if (uarmc && Role_if(PM_PRIEST) && itemhasappearance(uarmc, APP_ORNAMENTAL_COPE) ) armpro++;
		if (uwep && uwep->oartifact == ART_DAINTY_SLOAD) armpro++;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_HENRIETTA_S_TENACIOUSNESS) armpro++;
		if (Race_if(PM_INKA)) armpro++;
		if (ACURR(A_CHA) >= 18) armpro++;
		if (isunbalancor) armpro = 0;
	}
	if (armpro < 0) armpro = 0;

	return armpro;
}

/*
 * hitmu: monster hits you
 *	  returns 2 if monster dies (e.g. "yellow light"), 1 otherwise
 *	  3 if the monster lives but teleported/paralyzed, so it can't keep
 *	       attacking you
 */
STATIC_OVL int
hitmu(mtmp, mattk)
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	register struct permonst *mdat = mtmp->data;
	register int tmp = d((int)mattk->damn, (int)mattk->damd*10);
	if (MaximumDamageBug || u.uprops[MAXIMUM_DAMAGE_BUG].extrinsic || have_maximumdamagestone() || (uwep && uwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI)) tmp = (int)mattk->damn * (int)mattk->damd * 10;
	register boolean not_affected = defends((int)mattk->adtyp, uwep);
	register int uncancelled, ptmp;
	register boolean statsavingthrow = 0;
	if (rnd(200) < (ACURR(A_WIS) + ACURR(A_CHA))) statsavingthrow = 1;
	if (!PlayerCannotUseSkills && PlayerInSexyFlats) {
		int sexyflatchance = 0;
		switch (P_SKILL(P_SEXY_FLATS)) {
			case P_BASIC: sexyflatchance = 1; break;
			case P_SKILLED: sexyflatchance = 2; break;
			case P_EXPERT: sexyflatchance = 3; break;
			case P_MASTER: sexyflatchance = 4; break;
			case P_GRAND_MASTER: sexyflatchance = 5; break;
			case P_SUPREME_MASTER: sexyflatchance = 6; break;
			}
		if (sexyflatchance > rn2(100)) statsavingthrow = 1;
	}

	register struct engr *ep = engr_at(u.ux,u.uy);
	int dmg, armpro, permdmg;
	int armprolimit = 75;
	int	nobj = 0;
	char	 buf[BUFSZ];
	struct permonst *olduasmon = youmonst.data;
	int res;
	boolean burnmsg = FALSE;
	struct obj *optr;
	struct obj *otmpi, *otmpii;
	int hallutime;

	/*int randattack = 0;*/
	int atttyp;

	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);

/*	If the monster is undetected & hits you, you should know where
 *	the attack came from.
 */
	if(mtmp->mundetected && (hides_under(mdat) || mdat->mlet == S_EEL || mdat->mlet == S_FLYFISH)) {
	    mtmp->mundetected = 0;
	    if (!(Blind ? Blind_telepat : Unblind_telepat)) {
		struct obj *obj;
		const char *what;

		if ((obj = level.objects[mtmp->mx][mtmp->my]) != 0) {
		    if (Blind && !obj->dknown)
			what = something;
		    else if (is_waterypool(mtmp->mx, mtmp->my) && !Underwater)
			what = "the water";
		    else if (is_watertunnel(mtmp->mx, mtmp->my) && !Underwater)
			what = "a water tunnel";
		    else if (is_shiftingsand(mtmp->mx, mtmp->my) && !Underwater)
			what = "the shifting sand";
		    else
			what = doname(obj);

		    pline("%s was hidden under %s!", Amonnam(mtmp), what);
		}
		newsym(mtmp->mx, mtmp->my);
	    }
	}

/*	First determine the base damage done */
	dmg = d((int)mattk->damn, (int)mattk->damd);
	if( (is_undead(mdat) || mtmp->egotype_undead) && midnight())
		dmg += d((int)mattk->damn, (int)mattk->damd); /* extra damage */

	if (MaximumDamageBug || u.uprops[MAXIMUM_DAMAGE_BUG].extrinsic || have_maximumdamagestone() || (uwep && uwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI)) {
		dmg = (int)mattk->damn * (int)mattk->damd;
		if( (is_undead(mdat) || mtmp->egotype_undead) && midnight())
			dmg *= 2;
	}

	if (mtmp->mfrenzied && !rn2(2) && (dmg > 0)) dmg += rnd(dmg);
/*	Next a cancellation factor	*/

/*	Use uncancelled when the cancellation factor takes into account certain
 *	armor's special magic protection.  Otherwise just use !mtmp->mcan.
 */
	armpro = magic_negation(&youmonst);

	if (!(PlayerCannotUseSkills)) {

		switch (P_SKILL(P_SPIRITUALITY)) {
			default: armprolimit = 75; break;
			case P_BASIC: armprolimit = 78; break;
			case P_SKILLED: armprolimit = 81; break;
			case P_EXPERT: armprolimit = 84; break;
			case P_MASTER: armprolimit = 87; break;
			case P_GRAND_MASTER: armprolimit = 90; break;
			case P_SUPREME_MASTER: armprolimit = 93; break;
		}
	}

	uncancelled = !mtmp->mcan && ((rn2(3) >= armpro) || ((rnd(100) > armprolimit) && ((armpro < 4) || (rnd(armpro) < 4) ) ) ); /* mc3 no longer protects that much --Amy */

	permdmg = 0;
/*	Now, adjust damages via resistances or specific attacks */

	/* Monsters with AD_RBRE will choose a random attack instead. --Amy */

	atttyp = mattk->adtyp;

	if (mattk->aatyp == AT_SPIT && atttyp == AD_TCKL) atttyp = AD_PHYS; /* manticore fix */

	if ((SecretAttackBug || u.uprops[SECRET_ATTACK_BUG].extrinsic || have_secretattackstone()) && atttyp == AD_PHYS && !rn2(100)) {
		while (atttyp == AD_ENDS || atttyp == AD_RBRE || atttyp == AD_WERE || atttyp == AD_PHYS) {
			atttyp = randattack(); }

	}

	if ((UnfairAttackBug || u.uprops[UNFAIR_ATTACK_BUG].extrinsic || have_unfairattackstone()) && atttyp == AD_PHYS && !rn2(100)) {
		while (atttyp == AD_ENDS || atttyp == AD_RBRE || atttyp == AD_WERE || atttyp == AD_PHYS) {
			atttyp = rn2(AD_ENDS); }

	}

	if (atttyp == AD_RBRE) {
		while (atttyp == AD_ENDS || atttyp == AD_RBRE || atttyp == AD_WERE) {
			atttyp = randattack(); }
		/*randattack = 1;*/
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
		atttyp = mtmp->m_id;
		if (atttyp < 0) atttyp *= -1;
		while (atttyp >= AD_ENDS) atttyp -= AD_ENDS;
		if (!(atttyp >= AD_PHYS && atttyp < AD_ENDS)) atttyp = AD_PHYS; /* fail safe --Amy */
		if (atttyp == AD_WERE) atttyp = AD_PHYS;
	}

	if (RngeDonors && atttyp == AD_PHYS) atttyp = AD_THIR;

	if (mtmp->data == &mons[PM_BUNDLE_MONSTER] && mattk->aatyp == AT_HUGS) {
		You("are being bundled.");
	} else if (mtmp->data == &mons[PM_CUDDLE_MONSTER] && mattk->aatyp == AT_HUGS) {
		You("are being cuddled.");
	} else if (mtmp->data == &mons[PM_FLEECE_MONSTER] && mattk->aatyp == AT_HUGS) {
		You("are being fleeced.");
	} else if (mtmp->data == &mons[PM_FELEECHY] && mattk->aatyp == AT_HUGS) {
		You("are being feleechyd.");
	}

	otmp = MON_WEP(mtmp);

	switch(atttyp) {
	    case AD_PHYS:
		if (mattk->aatyp == AT_HUGS && !sticks(youmonst.data)) {
		    if(!u.ustuck && rn2(2)) {
			if (u_slip_free(mtmp, mattk)) {
			    dmg = 0;
			} else if (Role_if(PM_ASTRONAUT) && rn2(5)) {
			/* astronauts are capable of wriggling free; the attack still damages them though --Amy */
			    pline("%s crushes you!", Monnam(mtmp));
			} else {
			    setustuck(mtmp);
			    pline("%s grabs you!", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
			}
		    } else if(u.ustuck == mtmp) {
			exercise(A_STR, FALSE);
			if (mtmp->data == &mons[PM_ROPE_GOLEM] && Breathless) {
			    You("are being strangled.");
			    dmg = (dmg+1) / 2;
				if (StrongMagical_breathing && dmg > 1) dmg /= 2;
			} else
			    You("are being %s.",
				    (mtmp->data == &mons[PM_ROPE_GOLEM])
				    ? "choked" : "crushed");
		    }
		} else {			  /* hand to hand weapon */
		    if( (mtmp->egotype_weaponizer || mattk->aatyp == AT_WEAP) && otmp) {
			int nopoison = (10/* - (otmp->owt/10)*/);
			if (otmp->otyp == CORPSE &&
				touch_petrifies(&mons[otmp->corpsenm])) {
			    dmg = 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[otmp->corpsenm].mname);
			    if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) goto do_stone;
			}

			/* MRKR: If hit with a burning torch,     */
			/*       then do an extra point of damage */
			/*       but save the message till after  */
			/*       the hitmsg()                     */

			if (otmp->otyp == TORCH && otmp->lamplit && !Fire_resistance) {
			  burnmsg = TRUE;
			  dmg++;
			}

			/* WAC -- Real weapon?
			 * Could be stuck with a cursed bow/polearm it wielded
			 */
			if (/* if you strike with a bow... */
				(is_launcher(otmp) && otmp->otyp != LASERXBOW && otmp->otyp != KLIUSLING) ||
				/* or strike with a missile in your hand... */
				(is_missile(otmp) || is_ammo(otmp)) ||
				/* lightsaber that isn't lit ;) */
				(is_lightsaber(otmp) && !otmp->lamplit) ||
				/* WAC -- or using a pole at short range... */
				(is_pole(otmp))) {
			    /* then do only 1-2 points of damage */
			    if (is_shade(youmonst.data) && objects[otmp->otyp].oc_material != MT_SILVER && objects[otmp->otyp].oc_material != MT_ARCANIUM && rn2(3))
				dmg = 0;
			    else
				dmg += rnd(2); /* don't lose the base damage from monst.txt --Amy */

#if 0 /* Monsters don't wield boomerangs */
			    if (otmp->otyp == BOOMERANG /* && !rnl(3) */) {
				pline("As %s hits you, %s breaks into splinters.",
				      mon_nam(mtmp), the(xname(otmp)));
				useup(otmp);
				otmp = (struct obj *) 0;
				possibly_unwield(mtmp);
				if (!is_shade(youmonst.data) || !rn2(3) )
				    dmg++;
			    }
#endif
			} else dmg += dmgval(otmp, &youmonst);

			if (otmp && otmp->otyp == COLLUSION_KNIFE && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) ) {

				pline("Collusion!");
				litroomlite(FALSE);

			}

			if (otmp && otmp->otyp == SPRAY_BOX && !Blind) {
				make_blinded(Blinded + rnd(10), TRUE);
			}

			if (otmp && otmp->otyp == DARKNESS_CLUB && !(Race_if(PM_PLAYER_NIBELUNG) && rn2(5))) {

				pline("Collusion!");
				litroomlite(FALSE);

			}

			if (otmp && otmp->otyp == YITH_TENTACLE) {

				increasesanity(rnz(monster_difficulty() + 1));

			}

			if (otmp && otmp->otyp == NASTYPOLE && !rn2(10)) {

				badeffect();

			}

			if (otmp && otmp->otyp == PETRIFYIUM_BAR) {

			    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
				!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
				if (!Stoned) {
					if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
					else {
						Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
						u.cnd_stoningcount++;
						pline("You start turning to stone!");
					}
				}
				sprintf(killer_buf, "petrifyium bar");
				delayed_killer = killer_buf;
		
			    }

			}

			if (otmp && otmp->otyp == DISINTEGRATION_BAR) {

			if ((!Disint_resistance || !rn2(StrongDisint_resistance ? 1000 : 100) || (evilfriday && (uarms || uarmc || uarm || uarmu)) ) && !rn2(10)) {
				You_feel("like you're falling apart!");
	
				if (uarms) {
				    /* destroy shield; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
				} else if (uarmc) {
				    /* destroy cloak; other possessions are safe */
				    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
				} else if (uarm) {
				    /* destroy suit */
				    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
				} else if (uarmu) {
				    /* destroy shirt */
				    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
				} else {
					if (u.uhpmax > 20) {
						u.uhpmax -= rnd(20);
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						losehp(rnz(100 + level_difficulty()), "click click click click click you died", KILLED_BY);

					} else {
						u.youaredead = 1;
						done(DIED);
						u.youaredead = 0;
					}
				}
	
			}

			}

			if (objects[otmp->otyp].oc_material == MT_SILVER &&
				hates_silver(youmonst.data) || (uwep && uwep->oartifact == ART_PORKMAN_S_BALLS_OF_STEEL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_PORKMAN_S_BALLS_OF_STEEL) ) {
			    pline("The silver sears your flesh!");
			}
			if (objects[otmp->otyp].oc_material == MT_COPPER &&
				hates_copper(youmonst.data)) {
			    pline("The copper decomposes you!");
			}
			if (objects[otmp->otyp].oc_material == MT_PLATINUM &&
				(hates_platinum(youmonst.data) || u.contamination >= 1000) ) {
			    pline("The platinum smashes you!");
			}
			if (otmp->cursed && (hates_cursed(youmonst.data) || youmonst.data->mlet == S_ANGEL || Race_if(PM_HUMANOID_ANGEL)) ) {
			    pline("An unholy aura blasts you!");
			}
			if (objects[otmp->otyp].oc_material == MT_VIVA && hates_viva(youmonst.data)) {
			    pline("The irradiation severely hurts you!");
			}
			if (objects[otmp->otyp].oc_material == MT_INKA) { /* you are always vulnerable to inka, no matter what --Amy */
			    pline("The inka string hurts you!");
			}
			if (otmp->otyp == ODOR_SHOT) {
			    pline("You inhale a horrific odor!");
			}
			/* Stakes do extra dmg agains vamps */
			if ((otmp->otyp == WOODEN_STAKE || otmp->oartifact == ART_VAMPIRE_KILLER) &&
				is_vampire(youmonst.data)) {
			    if (otmp->oartifact == ART_STAKE_OF_VAN_HELSING) {
				if (!rn2(10)) {
				    u.youaredead = 1;
				    pline("%s plunges the stake into your heart.",
					    Monnam(mtmp));
				    killer = "a wooden stake in the heart.";
				    killer_format = KILLED_BY_AN;
				    u.ugrave_arise = NON_PM; /* No corpse */
				    done(DIED);
				    u.youaredead = 0;
				} else {
				    pline("%s drives the stake into you.",
					    Monnam(mtmp));
				    dmg += rnd(6) + 2;
				}
			    }else if (otmp->oartifact == ART_VAMPIRE_KILLER) {
				pline("%s whips you good!",
					Monnam(mtmp));
				dmg += rnd(6);
			    } else {
				pline("%s drives the stake into you.",
					Monnam(mtmp));
				dmg += rnd(6);
			    }
			}

			if (otmp->opoisoned) {
			    poisoned(simple_typename(otmp->otyp), A_STR,
				    killer_xname(otmp), 10);
			    if (nopoison < 2) nopoison = 2;
			    if (!rn2(nopoison) && (!otmp->superpoison || !rn2(10)) ) {
				otmp->opoisoned = FALSE;
				otmp->superpoison = FALSE;
				pline("%s %s no longer poisoned.",
				       s_suffix(Monnam(mtmp)),
				       aobjnam(otmp, "are"));
			    }
			}
			if (dmg <= 0) dmg = 1;
			if (!otmp->oartifact || !artifact_hit(mtmp, &youmonst,
				otmp, &dmg, dieroll))
			     hitmsg(mtmp, mattk);

			if (burnmsg) {
			  boolean plural = (Blind ? FALSE : otmp->quan > 1L);
			  boolean water = (youmonst.data ==
					   &mons[PM_WATER_ELEMENTAL]);

			  pline("%s %s%s %syou!",
				(Blind ? "It" : Yname2(otmp)),
				(water ? "vaporize" : "burn"),
				(plural ? "" : "s"),
				(water ? "part of " : ""));

			  if ((isevilvariant || !rn2(2)) && burnarmor(&youmonst)) {
			    dmg++;

			    /* Torch flame is not hot enough to guarantee */
			    /* burning away slime */

			    if (!rn2(4)) burn_away_slime();
			    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
			      (void)destroy_item(POTION_CLASS, AD_FIRE);
			    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
			      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
			    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
			      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
			  }
			  burn_faster(otmp, 1);
			}

			if (!dmg) break;
			if (u.mh > 1 && u.mh > ((u.uac>0) ? dmg : dmg+u.uac) &&
				   objects[otmp->otyp].oc_material == MT_IRON &&
					(u.umonnum==PM_SHOCK_PUDDING || u.umonnum==PM_VOLT_PUDDING || u.umonnum==PM_BLACK_PUDDING || u.umonnum==PM_DIVISIBLE_ICE_CREAM_BAR || u.umonnum==PM_GEMINICROTTA || u.umonnum==PM_GREY_PUDDING || u.umonnum==PM_STICKY_PUDDING || u.umonnum==PM_DRUDDING || u.umonnum==PM_BLACK_DRUDDING || u.umonnum==PM_BLACKSTEEL_PUDDING || u.umonnum==PM_BLOOD_PUDDING || u.umonnum==PM_MORAL_HAZARD || u.umonnum==PM_MORAL_EVENT_HORIZON
					|| u.umonnum==PM_BROWN_PUDDING || u.umonnum==PM_BLACK_PIERCER)) {
			    /* This redundancy necessary because you have to
			     * take the damage _before_ being cloned.
			     */
			    if (u.uac < 0) dmg += u.uac;
			    if (dmg < 1) dmg = 1;
			    if (dmg > 1) exercise(A_STR, FALSE);
			    u.mh -= dmg;
			    flags.botl = 1;
			    dmg = 0;
			    if (!rn2(50)) { if(cloneu()) /* greatly reduce ability to farm puddings --Amy */
			    You("divide as %s hits you!",mon_nam(mtmp));
				}
			}
			urustm(mtmp, otmp);
		    } else if (mattk->aatyp != AT_TUCH || dmg != 0 ||
			    mtmp != u.ustuck)
			hitmsg(mtmp, mattk);
		}
		break;
	    case AD_DISE:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (rn2(3)) break;
                if (!diseasemu(mdat) || Invulnerable || (Stoned_chiller && Stoned)) dmg = 0;
		break;

	    case AD_SPEL:
	    case AD_CAST:
	    case AD_CLRC:
		hitmsg(mtmp, mattk);
		castmu(mtmp, mattk, TRUE, TRUE);
		break;

	    case AD_VOMT:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (!rn2(StrongSick_resistance ? 100 : 10) || !Sick_resistance) {
			if (!Vomiting) {
				make_vomiting(Vomiting+d(10,4), TRUE);
				pline("You feel nauseated.");
				if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10);
			} else if (!rn2(2)) diseasemu(mdat);
		}
		break;

	    case AD_NGRA:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		/* This attack can remove any Elbereth engraving, even burned ones. --Amy */
			pline("The engraving underneath you vanishes!");
		    del_engr(ep);
		    ep = (struct engr *)0;
		}

		break;
	    case AD_GLIB:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		/* hurt the player's hands --Amy */
		pline("Your hands got hit hard!");
		incr_itimeout(&Glib, dmg);

		break;
	    case AD_DARK:

		if (youmonst.data->mlet == S_ANGEL || Race_if(PM_HUMANOID_ANGEL)) dmg *= 2;
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) break;

		/* create darkness around the player --Amy */
		pline("That felt evil and sinister!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Konechno, Sovetskiy sdelal eto tak, chto vy ne mozhete uvidet' bol'shinstvo monstrov pryamo seychas. Kha-kha-kha ..." : "Diedaedodiedaerr!");
			litroomlite(FALSE);
		break;

	    case AD_UVUU:{
		int wdmg = (int)(dmg/6) + 1;
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		poisoned(buf, A_CON, mdat->mname, 60);
		if(Poison_resistance) wdmg -= ACURR(A_CON)/2;
		if (StrongPoison_resistance && wdmg > 1) wdmg /= 2;
		if(wdmg > 0){
		
			while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
				wdmg--;
				(void) adjattrib(A_WIS, -1, TRUE, TRUE);
				forget_levels(1);	/* lose memory of 1% of levels per point lost*/
				forget_objects(1);	/* lose memory of 1% of objects per point lost*/
				exercise(A_WIS, FALSE);
			}
			if(AMAX(A_WIS) > ATTRMIN(A_WIS) && 
				ABASE(A_WIS) < AMAX(A_WIS)/2) AMAX(A_WIS) -= 1; //permanently drain wisdom
			if(wdmg){
				boolean chg;
				chg = make_hallucinated(HHallucination + (long)(wdmg*5),FALSE,0L);
			}
		}
		drain_en( (int)(dmg/2) );
		if(!rn2(20)){
			if (!has_head(youmonst.data) || Role_if(PM_COURIER) ) {
				dmg *= 2;
			}
			else if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) {
				pline("%s passes through your %s.",
				      mon_nam(mtmp), body_part(HEAD));
				dmg *= 2;
			}
			else {
				if(!uarmh){
					dmg = (ABASE(A_WIS) <= ATTRMIN(A_WIS)) ? ( 2 * (Upolyd ? u.mh : u.uhp) + 400) : (dmg * 2); 
					pline("%s smashes your %s!",
						mon_nam(mtmp), body_part(HEAD));
				} else pline("%s's headspike hits your %s!",
						mon_nam(mtmp), xname(uarmh) );
			}
		 }
 		}
	    break;

	    case AD_ICEB:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		pline("You are hit by ice blocks!");
		if (issoviet) pline("KHA KHA KHA!");
		if (Race_if(PM_GAVIL)) dmg *= 2;
		if (Race_if(PM_HYPOTHERMIC)) dmg *= 3;
		if(!mtmp->mcan && !rn2(3)) {
		    make_frozen(HFrozen + dmg, TRUE);
		    dmg /= 2;
		}
		if (!mtmp->mcan && (isevilvariant || !rn2(issoviet ? 2 : Race_if(PM_GAVIL) ? 2 : Race_if(PM_HYPOTHERMIC) ? 2 : 10)) ) {
			destroy_item(POTION_CLASS, AD_COLD);
		}
		if (Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) {
			pline("The cold doesn't seem to affect you.");
			dmg = 0;
		}
	    break;

	    case AD_MALK:
		hitmsg(mtmp, mattk);
		if (uncancelled) {

		    setustuck(mtmp);
		    pline("%s grabs you!", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
		    if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		    You("get zapped!");
		    if (Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) {
			pline_The("zap doesn't shock you!");
			dmg = 0;
		    }
		    if (isevilvariant || !rn2(issoviet ? 2 : 10)) /* high voltage - stronger than ordinary shock attack --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 2 : 10))
			destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 10 : 50))
			destroy_item(AMULET_CLASS, AD_ELEC);
		}
		break;

	    case AD_LAVA:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		    pline("You're %s!", on_fire(youmonst.data, mattk));
		    if (youmonst.data == &mons[PM_STRAW_GOLEM] ||
		        youmonst.data == &mons[PM_PAPER_GOLEM]) {
			    You("roast!");
			    /* KMH -- this is okay with unchanging */
			    rehumanize();
			    break;
		    } else if (Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) {
			pline_The("fire doesn't feel hot!");
			dmg = 0;
                        } else if (u.umonnum == PM_STRAW_GOLEM ||
				   u.umonnum == PM_PAPER_GOLEM ||
				   u.umonnum == PM_WAX_GOLEM) {
				/* This code ASSUMES that you are polymorphed 
				 * Code will need to be changed if we ever implement
				 * Golems as a class.
				 */
				You("burn up!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0; /* Kill monster form */
				rehumanize();
				break;
		    }
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 4 : 20)) /* hotter than ordinary fire attack, so more likely to burn items --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 4 : 20))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 30))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		    if (evilfriday || rn2(3)) hurtarmor(AD_LAVA);
		break;

	    case AD_FIRE:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("You're %s!", on_fire(youmonst.data, mattk));
		    if (youmonst.data == &mons[PM_STRAW_GOLEM] ||
		        youmonst.data == &mons[PM_PAPER_GOLEM]) {
			    You("roast!");
			    /* KMH -- this is okay with unchanging */
			    rehumanize();
			    break;
		    } else if (Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) {
			pline_The("fire doesn't feel hot!");
			dmg = 0;
                        } else if (u.umonnum == PM_STRAW_GOLEM ||
				   u.umonnum == PM_PAPER_GOLEM ||
				   u.umonnum == PM_WAX_GOLEM) {
				/* This code ASSUMES that you are polymorphed 
				 * Code will need to be changed if we ever implement
				 * Golems as a class.
				 */
				You("burn up!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0; /* Kill monster form */
				rehumanize();
				break;
		    }
/*		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(SCROLL_CLASS, AD_FIRE);
		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(POTION_CLASS, AD_FIRE);
		    if((int) mtmp->m_lev > rn2(25))
			destroy_item(SPBOOK_CLASS, AD_FIRE);*/
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		} /*else dmg = 0;*/
		break;
	    case AD_COLD:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("You're covered in frost!");
		    if (Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) {
			pline_The("frost doesn't seem cold!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (Race_if(PM_GAVIL)) dmg *= 2;
		    if (Race_if(PM_HYPOTHERMIC)) dmg *= 3;
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);
		} /*else dmg = 0;*/
		break;
	    case AD_ELEC:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    You("get zapped!");
		    if (Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) {
			pline_The("zap doesn't shock you!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */
			destroy_item(AMULET_CLASS, AD_ELEC);
		} /*else dmg = 0;*/
		break;
	    case AD_SLEE:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(5)) {
		    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) break;
		    fall_asleep(-rnd(10), TRUE);
		    if (Blind) You("are put to sleep!");
		    else You("are put to sleep by %s!", mon_nam(mtmp));
		}
		break;
	    case AD_BLND:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0) && !rn2(3) ) {
		    if (!Blind) pline("%s blinds you!", Monnam(mtmp));
		    make_blinded(Blinded+(long)dmg,FALSE);
		    if (!Blind) Your("%s", vision_clears);
		}
		/*dmg = 0;*/ /* why??? --Amy */
		break;

	    case AD_DREA:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		if (multi < 0) {
			dmg *= 4;
			pline("Your dream is eaten!");
		}

		break;

	    case AD_BADE:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		badeffect();

		break;

	    case AD_BLEE:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (dmg > 0) playerbleed(dmg);

		break;

	    case AD_SHAN:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		shank_player();

		break;

	    case AD_DEBU:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		statdebuff();

		break;

	    case AD_UNPR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		{
			struct obj *obj = some_armor(&youmonst);
			if (obj && obj->oerodeproof) {
				obj->oerodeproof = FALSE;
				Your("%s brown for a moment.", aobjnam(obj, "glow"));
			}
		}

		break;

	    case AD_NIVE:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		nivellate();
		break;

	    case AD_SCOR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		u.urexp -= (dmg * 50);
		if (u.urexp < 0) u.urexp = 0;
		Your("score is drained!");

		break;

	    case AD_TERR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		terrainterror();

		break;

	    case AD_FEMI:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		randomfeminismtrap(rnz( (dmg + 2) * rnd(100)));
		if (Role_if(PM_CLIMACTERIAL)) climtrainsqueaking(dmg);

		break;

	    case AD_LEVI:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		You("float up!");
		HLevitation &= ~I_SPECIAL;
		incr_itimeout(&HLevitation, dmg);

		break;

	    case AD_MCRE:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		MCReduction += (100 * dmg);
		pline("The magic cancellation granted by your armor seems weaker now...");

		break;

	    case AD_RBAD:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		reallybadeffect();

		break;

	    case AD_FUMB:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(10 * (dmg + 1) );

		break;

	    case AD_TREM:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		pline("Your %s are trembling!", makeplural(body_part(HAND)));
		u.tremblingamount++;

		break;

	    case AD_IDAM:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(20)) rust_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}

		break;

	    case AD_ANTI:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(20)) wither_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}

		break;

	    case AD_PAIN:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		pline("It's painful!");
		losehp(Upolyd ? ((u.mh / 10) + 1) : ((u.uhp / 10) + 1), "a painful attack", KILLED_BY);

		break;

	    case AD_TECH:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		techcapincrease(dmg * rnd(50));

		break;

	    case AD_MEMO:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		spellmemoryloss(dmg);

		break;

	    case AD_TRAI:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		skilltrainingdecrease(dmg);
		if (!rn2(100)) skillcaploss();

		break;

	    case AD_STAT:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		if (uncancelled && !rn2(3)) {
			statdrain();
		}

		break;

	    case AD_VULN:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		deacrandomintrinsic(rnz( (dmg * rnd(30) ) + 1));

		break;

	    case AD_ICUR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		if (!rn2(5)) {
			You_feel("as if you need some help.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			rndcurse();
		}

		break;

	    case AD_NACU:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (uncancelled) nastytrapcurse();

		break;

	    case AD_SLUD:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
		    }
		}

		break;

	    case AD_NAST:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		if (!rn2(10)) {
			pline("Nasty!");

			randomnastytrapeffect(rnz( (dmg + 2) * rnd(100)), 1000 - (dmg * 3));
		}

		break;

	    case AD_VENO:
		hitmsg(mtmp, mattk);

		if (chromeprotection()) break;

		if (!Poison_resistance) pline("You're badly poisoned!");
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE, TRUE);
		ptmp = rn2(A_MAX);

		if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(POTION_CLASS, AD_VENO);
		if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(FOOD_CLASS, AD_VENO);

		goto dopois;

	    case AD_POIS:
		ptmp = rn2(A_MAX);
		goto dopois;
	    case AD_DRST:
		ptmp = A_STR;
		goto dopois;
	    case AD_DRDX:
		ptmp = A_DEX;
		goto dopois;
	    case AD_WISD:
		ptmp = A_WIS;
		goto dopois;
	    case AD_DRCH:
		ptmp = A_CHA;
		goto dopois;
	    case AD_DRCO:
		ptmp = A_CON;
dopois:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(8)) {
		    sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, ptmp, mdat->mname, 30);
		}
		break;
	    case AD_DFOO:
	      pline("%s determines to take you down a peg or two...", Monnam(mtmp));
		if (!rn2(3)) {
		    sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, rn2(A_MAX), mdat->mname, 30);
		}
		if (!rn2(4)) {
			You_feel("drained...");
			permdmg = 1;
		}
		if (!rn2(4)) {
			You_feel("less energised!");
			u.uenmax -= rn1(10,10);
			if (u.uenmax < 0) u.uenmax = 0;
			if(u.uen > u.uenmax) u.uen = u.uenmax;
		}
		if (!rn2(4)) {
			if((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && (u.urmaxlvlUP >= 2) )
			    losexp("life drainage", FALSE, TRUE);
			else You_feel("woozy for an instant, but shrug it off.");
		}

		break;

	    case AD_DRIN:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		if (uarmh && itemhasappearance(uarmh, APP_SERRATED_HELMET)) {
			if (mtmp->mhp < 5) break;
			mtmp->mhp -= rnd(4);
			if (mtmp->mhp < 1) break;
		}

		if (defends(AD_DRIN, uwep) || (StrongPsi_resist && rn2(3)) || !has_head(youmonst.data) || Role_if(PM_COURIER)  || Race_if(PM_KUTAR) || (rn2(8) && uarmf && itemhasappearance(uarmf, APP_MARY_JANES) ) ) {
		    You("don't seem harmed.");
		    /* Not clear what to do for green slimes */
		    break;
		}
		if (u_slip_free(mtmp,mattk)) break;
		if (uarmh && uarmh->otyp == OILSKIN_COIF && rn2(10)) {
			Your("helmet blocks the attack to your head.");
			break;
		}

		if (uarmh && !(uarmh && itemhasappearance(uarmh, APP_STRIP_BANDANA)) && rn2(evilfriday ? 3 : 8)) {
		    /* not body_part(HEAD) */
		    Your("helmet blocks the attack to your head.");
		    break;
		}
		
                /* conflicted dog, perhaps? */
		if (mtmp->mtame && !mtmp->isminion) {
		    EDOG(mtmp)->hungrytime += rnd(60);
		    mtmp->mconf = 0;
		}

		if (Half_physical_damage && rn2(2) ) dmg = (dmg+1) / 2;
		if (StrongHalf_physical_damage && rn2(2) ) dmg = (dmg+1) / 2;
		mdamageu(mtmp, dmg);

		if (!uarmh || uarmh->otyp != DUNCE_CAP) {
		    Your("brain is eaten!");

		    if (mtmp->data == &mons[PM_MINDGAYER]) {
				u.homosexual = 2;
		    }

		    /* No such thing as mindless players... */
		    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
			int lifesaved = 0;
			struct obj *wore_amulet = uamul;

			while(1) {
			    /* avoid looping on "die(y/n)?" */
			    if (lifesaved && (discover || wizard)) {
				if (wore_amulet && !uamul) {
				    /* used up AMULET_OF_LIFE_SAVING; still
				       subject to dying from brainlessness */
				    wore_amulet = 0;
				} else if (wizard) {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }
			    u.youaredead = 1;
			    u.youarereallydead = 1;

			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    if (wizard) {
				u.youaredead = 0;
				u.youarereallydead = 0;
			    }
			    lifesaved++;
			}
		    }
		}
		/* adjattrib gives dunce cap message when appropriate */
		if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
		else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE, TRUE);
		if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
		if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		exercise(A_WIS, FALSE);
		break;
	    case AD_PLYS:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (uncancelled && multi >= 0 && !rn2(3)) {
		    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
			You("momentarily stiffen.");            
		    } else {
			if (Blind) You("are frozen!");
			else You("are frozen by %s!", mon_nam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
			nomovemsg = 0;	/* default: "you can move again" */
			{
				int paralysistime = dmg;
				if (paralysistime > 1) paralysistime = rnd(paralysistime);
				if (paralysistime > 5) {
					while (rn2(5) && (paralysistime > 5)) {
						paralysistime--;
					}
				}
				if (!rn2(3)) nomul(-rnd(5), "paralyzed by a monster attack", TRUE);
				else nomul(-(paralysistime), "paralyzed by a monster attack", TRUE);
			}
			exercise(A_DEX, FALSE);
		    }
		}
		break;
	    case AD_TCKL:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (uncancelled && multi >= 0 && !rn2(3)) {
		    if (Free_action && rn2(StrongFree_action ? 100 : 20))
			You_feel("horrible tentacles probing your flesh!");
		    else {
			if (Blind) You("are mercilessly tickled!");
			else You("are mercilessly tickled by %s!", mon_nam(mtmp));
			nomovemsg = 0;	/* default: "you can move again" */
			{
				int paralysistime = dmg;
				if (paralysistime > 1) paralysistime = rnd(paralysistime);
				if (paralysistime > 4) {
					while (rn2(12) && (paralysistime > 4)) {
						paralysistime--;
					}
				}
				if (!rn2(3)) nomul(-rnd(4), "tickled by a monster", TRUE);
				else nomul(-(paralysistime), "tickled by a monster", TRUE);
			}
			exercise(A_DEX, FALSE);
			exercise(A_CON, FALSE);
		    }
		}
		break;

	    case AD_AXUS:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("You're %s!", on_fire(youmonst.data, mattk));
		    if (youmonst.data == &mons[PM_STRAW_GOLEM] ||
		        youmonst.data == &mons[PM_PAPER_GOLEM]) {
			    You("roast!");
			    /* KMH -- this is okay with unchanging */
			    rehumanize();
			    break;
		    } else if (Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) {
			pline_The("fire doesn't feel hot!");
			if (dmg >= 4) dmg -= (dmg / 4);
	          } else if (u.umonnum == PM_STRAW_GOLEM ||
				   u.umonnum == PM_PAPER_GOLEM ||
				   u.umonnum == PM_WAX_GOLEM) {
				/* This code ASSUMES that you are polymorphed 
				 * Code will need to be changed if we ever implement
				 * Golems as a class.
				 */
				You("burn up!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0; /* Kill monster form */
				rehumanize();
				break;
		    }
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		}
		if (uncancelled) {
		    pline("You're covered in frost!");
		    if (Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) {
			pline_The("frost doesn't seem cold!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    if (Race_if(PM_GAVIL)) dmg *= 2;
		    if (Race_if(PM_HYPOTHERMIC)) dmg *= 3;
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);
		}
		if (uncancelled) {
		    You("get zapped!");
		    if (Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) {
			pline_The("zap doesn't shock you!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */
			destroy_item(AMULET_CLASS, AD_ELEC);
		}

		if (uncancelled && !rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) ) && (u.urmaxlvlUP >= 2) ) {
		    losexp("life drainage", FALSE, TRUE);
		}

		break;

	    case AD_DRLI:
		hitmsg(mtmp, mattk);
		/* if vampire biting (and also a pet) */
		if (is_vampire(mtmp->data) && mattk->aatyp == AT_BITE &&
			has_blood(youmonst.data)) {
			   Your("blood is being drained!");
			   /* Get 1/20th of full corpse value
			    * Therefore 4 bites == 1 drink
			    */
			    if (mtmp->mtame && !mtmp->isminion)
			    	EDOG(mtmp)->hungrytime += ((int)((youmonst.data)->cnutrit / 20) + 1);
		}
		
		if (moves < 1000 && (u.urmaxlvlUP == 1) && rn2(25)) { /* grace period --Amy */
			break;
		}

		if (uncancelled && !rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )  ) {
		    losexp("life drainage", FALSE, TRUE);
		}
		break;

	    case AD_WEBS: 
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		{
			struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB, 0, FALSE);
			if (ttmp2) {
				pline_The("webbing sticks to you. You're caught!");
				dotrap(ttmp2, NOWEBMSG);
				if (u.usteed && u.utrap && !mayfalloffsteed()) {
				/* you, not steed, are trapped */
				dismount_steed(DISMOUNT_FELL);
				}
			}
		}
		/* Amy addition: sometimes, also make a random trap somewhere on the level :D */
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap(FALSE);
		break;

	    case AD_TRAP: 
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		if (t_at(u.ux, u.uy) == 0) (void) maketrap(u.ux, u.uy, randomtrap(), 0, FALSE);
		else makerandomtrap(FALSE);

		break;

	    case AD_STTP:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (rn2(3)) break;
		pline("You are surrounded by a purple glow!");
		if (invent) {
		    int itemportchance = 10 + rn2(21);
		    for (otmpi = invent; otmpi; otmpi = otmpii) {

		      otmpii = otmpi->nobj;

			if (!rn2(itemportchance) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}

		    }
		}
		break;

	    case AD_DEPR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (!rn2(3)) {

		    switch(rnd(20)) {
		    case 1:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
			      polyself(FALSE);
			}
			break;
		    case 2:
			You("need reboot.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
			if (!Race_if(PM_UNGENOMOLD)) newman();
			else polyself(FALSE);
			break;
		    case 3: case 4:
			if(!rn2(4) && u.ulycn == NON_PM &&
				!Protection_from_shape_changers &&
				!is_were(youmonst.data) &&
				!defends(AD_WERE,uwep)) {
			    You_feel("feverish.");
			    exercise(A_CON, FALSE);
			    u.ulycn = PM_WERECOW;
			    u.cnd_lycanthropecount++;
			} else {
				if (multi >= 0) {
				    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) break;
				    fall_asleep(-rnd(10), TRUE);
				    if (Blind) You("are put to sleep!");
				    else You("are put to sleep by %s!", mon_nam(mtmp));
				}
			}
			break;
		    case 5: case 6:
			if (!u.ustuck && !sticks(youmonst.data)) {
				setustuck(mtmp);
				pline("%s grabs you!", Monnam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
			break;
		    case 7:
		    case 8:
			Your("position suddenly seems very uncertain!");
			teleX();
			break;
		    case 9:
			u_slow_down();
			break;
		    case 10:
			hurtarmor(AD_RUST);
			break;
		    case 11:
			hurtarmor(AD_DCAY);
			break;
		    case 12:
			hurtarmor(AD_CORR);
			break;
		    case 13:
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(5), "paralyzed by a monster attack", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 14:
			if (FunnyHallu)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			hallutime = rn1(7, 16);
			make_stunned(HStun + hallutime + dmg, FALSE);
			(void) make_hallucinated(HHallucination + hallutime + dmg,TRUE,0L);
			break;
		    case 15:
			if(!Blind)
				Your("vision bugged.");
			hallutime += rn1(10, 25);
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + dmg + dmg,TRUE,0L);
			break;
		    case 16:
			if(!Blind)
				Your("vision turns to screen saver.");
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + dmg,TRUE,0L);
			break;
		    case 17:
			{
			    struct obj *obj = some_armor(&youmonst);

			    if (obj && drain_item(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			    }
			}
			break;
		    default:
			    if(Confusion)
				 You("are getting even more confused.");
			    else You("are getting confused.");
			    make_confused(HConfusion + dmg, FALSE);
			break;
		    }
		    exercise(A_INT, FALSE);

		}
		break;

	    case AD_VAMP:
		hitmsg(mtmp, mattk);
		/* if vampire biting (and also a pet) */
		if (is_vampire(mtmp->data) && mattk->aatyp == AT_BITE &&
			has_blood(youmonst.data)) {
			   Your("blood is being drained!");
			   /* Get 1/20th of full corpse value
			    * Therefore 4 bites == 1 drink
			    */
			    if (mtmp->mtame && !mtmp->isminion)
			    	EDOG(mtmp)->hungrytime += ((int)((youmonst.data)->cnutrit / 20) + 1);
		}
		
		if (moves < 1000 && (u.urmaxlvlUP == 1) && rn2(25)) { /* grace period --Amy */
			break;
		}

		if (!rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )  ) {
		    losexp("life drainage", FALSE, TRUE);
		}
		break;
	    case AD_LEGS:
		{ register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
		  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";

		/* This case is too obvious to ignore, but Nethack is not in
		 * general very good at considering height--most short monsters
		 * still _can_ attack you when you're flying or mounted.
		 * [FIXME: why can't a flying attacker overcome this?]
		 */
		  if (uarmf && itemhasappearance(uarmf, APP_THICK_BOOTS) ) {
		    pline("%s tries futilely to sting through your thick boots!", Monnam(mtmp));
		    dmg = 0; 
		  } else if (uarmf && itemhasappearance(uarmf, APP_HIGH_IRON_BOOTS) ) {
		    pline("%s tries futilely to sting through your high iron boots!", Monnam(mtmp));
		    dmg = 0; 
		  } else if ((u.usteed && !is_flyer(mtmp->data) && rn2(3) ) || 
			Levitation || (Flying && !is_flyer(mtmp->data)) ) {
		    pline("%s tries to reach your %s %s!", Monnam(mtmp), sidestr, body_part(LEG));
		    dmg = 0;
		  } else if (mtmp->mcan) {
		    pline("%s nuzzles against your %s %s!", Monnam(mtmp), sidestr, body_part(LEG));
		    dmg = 0;
		  } else {
		    if (uarmf) {
			if (rn2(2) && (uarmf->otyp == LOW_BOOTS || uarmf->otyp == IRON_SHOES))
			    pline("%s scratches the exposed part of your %s %s!",
				Monnam(mtmp), sidestr, body_part(LEG));
			else if (!rn2(5))
			    pline("%s scratches through your %s boot!", Monnam(mtmp), sidestr);
			else {
			    pline("%s scratches your %s boot!", Monnam(mtmp), sidestr);
			    dmg = 0;
			    break;
			}
		    } else pline("%s scratches your %s %s!", Monnam(mtmp), sidestr, body_part(LEG));
		    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
		    exercise(A_STR, FALSE);
		    exercise(A_DEX, FALSE);
		  }
		  break;
		}

	    case AD_HODS:
		if (statsavingthrow) break;
		 if(uwep){
			if (uwep->otyp == CORPSE
				&& touch_petrifies(&mons[uwep->corpsenm])) {
			    dmg = 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[uwep->corpsenm].mname);
			    if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ))
				goto do_stone;
			}
			dmg += dmgval(uwep, &youmonst);
			
			if (uwep->opoisoned){
				sprintf(buf, "%s %s",
					s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
				poisoned(buf, A_CON, mdat->mname, 30);
			}
			
			if (dmg <= 0) dmg = 1;
			if (!(uwep->oartifact &&
				artifact_hit(mtmp, &youmonst, uwep, &dmg,dieroll)))
			     hitmsg(mtmp, mattk);
			if (!dmg) break;
			hitmsg(mtmp, mattk);
		 }
		break;

	    case AD_STON:	/* cockatrice */
		if (StrongStone_resistance && rn2(10) && dmg > 1) dmg = 1;
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!rn2(3)) {
		    if (mtmp->mcan) {
			if (flags.soundok)
			    You_hear("a cough from %s!", mon_nam(mtmp));
		    } else {
			if (flags.soundok) {
			    You_hear("%s hissing!", s_suffix(mon_nam(mtmp)));
			    if (PlayerHearsSoundEffects) pline(issoviet ? "Eto menyayet status pamyatnika. I vy, veroyatno, ne imeyut dostatochnogo kolichestva predmetov otverzhdeniya, potomu chto vy vpustuyu ikh vse uzhe KHAR ty glupyy nub." : "Schhhhhhhhhhhhh!");
			}

			if (uarmc && itemhasappearance(uarmc, APP_PETRIFIED_CLOAK) && rn2(4)) break;
	
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
 do_stone:
			    if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {

				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = mtmp->data->mname;
					if (mtmp->data->geno & G_UNIQ) {
					    if (!type_is_pname(mtmp->data)) {
						static char kbuf[BUFSZ];
	
						/* "the" buffer may be reallocated */
						strcpy(kbuf, the(delayed_killer));
						delayed_killer = kbuf;
					    }
					    killer_format = KILLED_BY;
					} else killer_format = KILLED_BY_AN;
					return(1);
					/* You("turn to stone..."); */
					/* done_in_by(mtmp); */
				}
			    }
			}
		    }
		}
		break;

	    case AD_EDGE:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (!Stone_resistance || !rn2(StrongStone_resistance ? 100 : 20)) {
			if (Upolyd) {u.mhmax--; if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax--; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }
		}
		if(!rn2(3)) {
		    if (mtmp->mcan) {
			if (flags.soundok)
			    You_hear("a cough from %s!", mon_nam(mtmp));
		    } else {
			if (flags.soundok) {
			    You_hear("%s hissing!", s_suffix(mon_nam(mtmp)));
			    if (PlayerHearsSoundEffects) pline(issoviet ? "Eto menyayet status pamyatnika. I vy, veroyatno, ne imeyut dostatochnogo kolichestva predmetov otverzhdeniya, potomu chto vy vpustuyu ikh vse uzhe KHAR ty glupyy nub." : "Schhhhhhhhhhhhh!");
			}

			if (uarmc && itemhasappearance(uarmc, APP_PETRIFIED_CLOAK) && rn2(4)) break;
	
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
			    if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {

				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = mtmp->data->mname;
					if (mtmp->data->geno & G_UNIQ) {
					    if (!type_is_pname(mtmp->data)) {
						static char kbuf[BUFSZ];
	
						/* "the" buffer may be reallocated */
						strcpy(kbuf, the(delayed_killer));
						delayed_killer = kbuf;
					    }
					    killer_format = KILLED_BY;
					} else killer_format = KILLED_BY_AN;
					return(1);
					/* You("turn to stone..."); */
					/* done_in_by(mtmp); */
				}
			    }
			}
		    }
		}
		break;

	    case AD_STCK:
		hitmsg(mtmp, mattk);
		if (uncancelled && !u.ustuck && !(uwep && uwep->oartifact == ART_FOAMONIA_WATER) && !sticks(youmonst.data)) {
			setustuck(mtmp);
			pline("%s grabs you!", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}
		break;
	    case AD_WRAP:

		/* silent ocean technique completely prevents this attack from doing anything --Amy */
		if (tech_inuse(T_SILENT_OCEAN)) {
			pline("%s fails to wrap around you.", Monnam(mtmp));
			break;
		}

		if ((!mtmp->mcan || u.ustuck == mtmp) && !sticks(youmonst.data)) {
		    if (!u.ustuck && !rn2(10)) {
			if (u_slip_free(mtmp, mattk) || statsavingthrow) {
			    dmg = 0;
			} else if (Role_if(PM_ASTRONAUT) && rn2(5)) {
			/* astronauts are good at wriggling free, but will still get hurt by the attack itself --Amy */
				pline("%s constricts you!", Monnam(mtmp)); /* but doesn't wrap around you */
			} else {
			    pline("%s swings itself around you!", Monnam(mtmp));
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    setustuck(mtmp);
			}
		    } else if(u.ustuck == mtmp) {

			if (is_drowningpool(mtmp->mx,mtmp->my) && !(uwep && uwep->oartifact == ART_REGULARER_ANCHOR) && !Race_if(PM_KORONST)) {

		/* Being drowned should ALWAYS involve your stuff getting wet. 
		 * Even if you're unbreathing, swimming or whatever. Your stuff isn't immune just because you are.  --Amy	*/

			    pline("%s pulls you into the water!", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Ves' vash inventar' promokli. Pochemu vy ne ispol'zuyete svoy pobeg detal' vy neudachnoye opravdaniye igroka?" : "Platsch!");
				if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
					water_damage(invent, FALSE, FALSE);
					if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
				}
				if (Burned) make_burned(0L, TRUE);

			}

			if (is_lava(mtmp->mx,mtmp->my)) {

		/* drowning in lava  --Amy	*/
		/* This can only be done by monsters that can go on lava (flying couatls for example) and have wrap attacks */

			    pline("%s pulls you into the lava!", Monnam(mtmp));

				if (Slimed) {        
			      pline("The slime that covers you is burned away!");
			      Slimed = 0;
				}
			    if (Frozen) {
				pline("The ice thaws!");
				make_frozen(0L, FALSE);
			    }

			    destroy_item(SCROLL_CLASS, AD_FIRE);
			    destroy_item(SPBOOK_CLASS, AD_FIRE);
			    destroy_item(POTION_CLASS, AD_FIRE);
				burnarmor(&youmonst);

			}

			if (is_urinelake(mtmp->mx,mtmp->my)) {
				pline("%s pulls you into the urine lake!", Monnam(mtmp));
				if (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record > 0) {
					pline("Ulch - your divine body is tainted by that filthy yellow liquid!");
					u.ualign.record = -20;
				}

			}

			if (is_moorland(mtmp->mx,mtmp->my)) {

				pline("%s pulls you into the moorland!", Monnam(mtmp));
				/* do nothing */

			}

			if (is_shiftingsand(mtmp->mx,mtmp->my)) {

				pline("%s pulls you into the shifting sand!", Monnam(mtmp));
				losehp((u.uhp / 2), "being pulled into shifting sand", KILLED_BY);

			}

			if (is_styxriver(mtmp->mx,mtmp->my)) {
				pline("%s pulls you into the styx river!", Monnam(mtmp));
				contaminate(rnd(10), TRUE);
			}

			if (is_urinelake(mtmp->mx,mtmp->my) && !rn2(StrongFlying ? 5 : 3)) {

				if (u.ualign.record > 0) {
					pline("The yellow liquid seems harmless.");
				}

				else if (u.ualign.record == 0) {
					pline("The yellow liquid tickles your skin.");
					losehp(1, "being pulled into a urine lake while nominally aligned", KILLED_BY);
				}

				else if (u.ualign.record < 0) {
					pline("The yellow liquid %scorrodes your unprotected skin!", !Acid_resistance ? "severely " : "");
					losehp(rnd(10 + (level_difficulty() / 2)), "being pulled into a urine lake", KILLED_BY);
					if (!Acid_resistance) losehp(rnd(30 + level_difficulty()), "being pulled into a urine lake", KILLED_BY);
					if (!rn2(3)) badeffect();

					if (!rn2(3)) {
						register struct obj *objU, *objU2;
						for (objU = invent; objU; objU = objU2) {
						      objU2 = objU->nobj;
							if (!rn2(5)) rust_dmg(objU, xname(objU), 3, TRUE, &youmonst);
						}
					}

				}

			}

			if (is_moorland(mtmp->mx,mtmp->my) && !rn2(StrongFlying ? 5 : 3)) {
				pline("The adverse conditions in the moorland hurt your health!");
				losehp(rnd(10 + (level_difficulty() / 3)), "being pulled into moorland", KILLED_BY);

			}

			if (is_shiftingsand(mtmp->mx,mtmp->my) && !rn2(StrongFlying ? 5 : 3)) {
				/* instakill the poor sap - there is *no* resistance against this, and that's intentional --Amy */
				u.youaredead = 1;
				You("are pulled below the surface and suffocate.");
				killer_format = KILLED_BY_AN;
				sprintf(buf, "shifting sand by %s", an(mtmp->data->mname));
				killer = buf;
				done(DROWNING);
				u.youaredead = 0;

			}

			if (is_styxriver(mtmp->mx,mtmp->my) && !rn2(StrongFlying ? 5 : 3)) {
				pline("You're exposed to the styx river, and your contamination greatly increases.");
				contaminate(rnd(100 + (level_difficulty() * 10)), TRUE);

			}

			if (is_lava(mtmp->mx,mtmp->my) && !rn2(StrongFlying ? 5 : 3)) {

				if (!Fire_resistance) {
					u.youaredead = 1;
					You("burn to ashes...");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Kha! Ne ozhidal, chto budet mgnovennaya smert'? Nu, vashi poteri. Spasibo prikhodi yeshche." : "brrtzlbrrtzlbrrtzlbrrtzlbrrtzl");
					killer_format = KILLED_BY_AN;
					    sprintf(buf, "pool of lava by %s", an(mtmp->data->mname));
					    killer = buf;
					done(BURNING);
					u.youaredead = 0;

				}
				else {
					if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					You("scream "); verbalize("HAHAHAHAHAHAHAAAAAAAA!"); /* Super Mario 64 */
					u.uhpmax -= rnd(StrongFire_resistance ? 3 : 10);
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				    losehp(10, "lava dive", KILLED_BY_AN);
				}

			}

			if (is_drowningpool(mtmp->mx,mtmp->my) && !(uwep && uwep->oartifact == ART_REGULARER_ANCHOR) && !Race_if(PM_KORONST) && !Swimming
			    && !Amphibious && !Breathless && !rn2(StrongFlying ? 5 : 3) && !(uarmf && itemhasappearance(uarmf, APP_FIN_BOOTS)  ) ) { /* greatly lowered chance of getting drowned --Amy */
			    boolean moat =
				(levl[mtmp->mx][mtmp->my].typ != POOL) &&
				(levl[mtmp->mx][mtmp->my].typ != WATER) &&
				!Is_medusa_level(&u.uz) &&
				!Is_waterlevel(&u.uz);

			    u.youaredead = 1;

			    pline("%s drowns you...", Monnam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vy znali, chto sluchilos' by, ne tak li? I do sikh por vy ne ispol'zovali element, kotoryy by spas vas, potomu chto vy glupost' v dvizhenii! Geniy!" : "HUAAAAAAA-A-AAAAHHHHHH!");
			    killer_format = KILLED_BY_AN;
			    sprintf(buf, "%s by %s",
				    moat ? "moat" : "pool of water",
				    an(mtmp->data->mname));
			    killer = buf;
			    done(DROWNING);

			    u.youaredead = 0;

			} /*else*/ if(mattk->aatyp == AT_HUGS)
			    You("are being crushed.");
			    else You("are being wrapped.");
		    } else {
			dmg = 0;
			if(flags.verbose)
			    pline("%s brushes against your %s.", Monnam(mtmp),
				   body_part(LEG));
		    }
		} else dmg = 0;
		break;
	    case AD_WERE:
		hitmsg(mtmp, mattk);
		if (uncancelled && (!rn2(4) || (phase_of_the_moon() == FULL_MOON && !rn2(3) ) ) && u.ulycn == NON_PM &&
			!Protection_from_shape_changers &&
			!is_were(youmonst.data) &&
			!defends(AD_WERE,uwep)) {
		    You_feel("feverish.");
		    exercise(A_CON, FALSE);
		    u.ulycn = monsndx(mdat);
			u.cnd_lycanthropecount++;
			if (u.ulycn == PM_HUMAN_WERERAT) u.ulycn = PM_WERERAT;
			if (u.ulycn == PM_HUMAN_WERERABBIT) u.ulycn = PM_WERERABBIT;
			if (u.ulycn == PM_HUMAN_WEREBOAR) u.ulycn = PM_WEREBOAR;
			if (u.ulycn == PM_HUMAN_VORPAL_WERE_ALHOONTRICE_ZOMBIE) u.ulycn = PM_VORPAL_WERE_ALHOONTRICE_ZOMBIE;
			if (u.ulycn == PM_HUMAN_WERELOCUST) u.ulycn = PM_WERELOCUST;
			if (u.ulycn == PM_HUMAN_WEREJACKAL) u.ulycn = PM_WEREJACKAL;
			if (u.ulycn == PM_HUMAN_WEREWOLF) u.ulycn = PM_WEREWOLF;
			if (u.ulycn == PM_HUMAN_WEREPANTHER) u.ulycn = PM_WEREPANTHER;
			if (u.ulycn == PM_HUMAN_WERETIGER) u.ulycn = PM_WERETIGER;
			if (u.ulycn == PM_HUMAN_WERESNAKE) u.ulycn = PM_WERESNAKE;
			if (u.ulycn == PM_HUMAN_WERESPIDER) u.ulycn = PM_WERESPIDER;
			if (u.ulycn == PM_HUMAN_WERELICHEN) u.ulycn = PM_WERELICHEN;
			if (u.ulycn == PM_HUMAN_WEREPIERCER) u.ulycn = PM_WEREPIERCER;
			if (u.ulycn == PM_HUMAN_WEREPENETRATOR) u.ulycn = PM_WEREPENETRATOR;
			if (u.ulycn == PM_HUMAN_WERESMASHER) u.ulycn = PM_WERESMASHER;
			if (u.ulycn == PM_HUMAN_WERENYMPH) u.ulycn = PM_WERENYMPH;
			if (u.ulycn == PM_HUMAN_WEREBEE) u.ulycn = PM_WEREBEE;
			if (u.ulycn == PM_HUMAN_WEREMIMIC) u.ulycn = PM_WEREMIMIC;
			if (u.ulycn == PM_HUMAN_WEREPERMAMIMIC) u.ulycn = PM_WEREPERMAMIMIC;
			if (u.ulycn == PM_HUMAN_WERECOW) u.ulycn = PM_WERECOW;
			if (u.ulycn == PM_HUMAN_WEREBEAR) u.ulycn = PM_WEREBEAR;
			if (u.ulycn == PM_HUMAN_WEREBRONZEGRAM) u.ulycn = PM_WEREBRONZEGRAM;
			if (u.ulycn == PM_HUMAN_WERECHROMEGRAM) u.ulycn = PM_WERECHROMEGRAM;
			if (u.ulycn == PM_HUMAN_WEREDEMON) u.ulycn = PM_WEREDEMON;
			if (u.ulycn == PM_HUMAN_WEREPHANT) u.ulycn = PM_WEREPHANT;
			if (u.ulycn == PM_HUMAN_WEREVORTEX) u.ulycn = PM_WEREVORTEX;
			if (u.ulycn == PM_HUMAN_WEREGIANT) u.ulycn = PM_WEREGIANT;
			if (u.ulycn == PM_HUMAN_WEREGHOST) u.ulycn = PM_WEREGHOST;
			if (u.ulycn == PM_HUMAN_WEREMINDFLAYER) u.ulycn = PM_WEREMINDFLAYER;
			if (u.ulycn == PM_HUMAN_WERECOCKATRICE) u.ulycn = PM_WERECOCKATRICE;
			if (u.ulycn == PM_HUMAN_WERESOLDIERANT) u.ulycn = PM_WERESOLDIERANT;
			if (u.ulycn == PM_HUMAN_WERETROLL) u.ulycn = PM_WERETROLL;
			if (u.ulycn == PM_HUMAN_WEREBAT) u.ulycn = PM_WEREBAT;
			if (u.ulycn == PM_HUMAN_WEREPIRANHA) u.ulycn = PM_WEREPIRANHA;
			if (u.ulycn == PM_HUMAN_WEREEEL) u.ulycn = PM_WEREEEL;
			if (u.ulycn == PM_HUMAN_WEREFLYFISH) u.ulycn = PM_WEREFLYFISH;
			if (u.ulycn == PM_HUMAN_WEREKRAKEN) u.ulycn = PM_WEREKRAKEN;
			if (u.ulycn == PM_HUMAN_WERELICH) u.ulycn = PM_WERELICH;
			if (u.ulycn == PM_HUMAN_WEREJABBERWOCK) u.ulycn = PM_WEREJABBERWOCK;
			if (u.ulycn == PM_HUMAN_WEREGRIDBUG) u.ulycn = PM_WEREGRIDBUG;
			if (u.ulycn == PM_HUMAN_WEREACIDBLOB) u.ulycn = PM_WEREACIDBLOB;
			if (u.ulycn == PM_HUMAN_WEREFOX) u.ulycn = PM_WEREFOX;
			if (u.ulycn == PM_HUMAN_WEREMONKEY) u.ulycn = PM_WEREMONKEY;
			if (u.ulycn == PM_HUMAN_WEREFLOATINGEYE) u.ulycn = PM_WEREFLOATINGEYE;
			if (u.ulycn == PM_HUMAN_WEREGIANTANT) u.ulycn = PM_WEREGIANTANT;
			if (u.ulycn == PM_HUMAN_WEREKOBOLD) u.ulycn = PM_WEREKOBOLD;
			if (u.ulycn == PM_HUMAN_WERELEPRECHAUN) u.ulycn = PM_WERELEPRECHAUN;
			if (u.ulycn == PM_HUMAN_WEREHORSE) u.ulycn = PM_WEREHORSE;
			if (u.ulycn == PM_HUMAN_WERERUSTMONSTER) u.ulycn = PM_WERERUSTMONSTER;
			if (u.ulycn == PM_HUMAN_WEREBLACKLIGHT) u.ulycn = PM_WEREBLACKLIGHT;
			if (u.ulycn == PM_HUMAN_WEREAUTON) u.ulycn = PM_WEREAUTON;
			if (u.ulycn == PM_HUMAN_WEREGREMLIN) u.ulycn = PM_WEREGREMLIN;
			if (u.ulycn == PM_HUMAN_WEREGREENSLIME) u.ulycn = PM_WEREGREENSLIME;
			if (u.ulycn == PM_HUMAN_WEREJELLY) u.ulycn = PM_WEREJELLY;
			if (u.ulycn == PM_HUMAN_WEREXAN) u.ulycn = PM_WEREXAN;
			if (u.ulycn == PM_HUMAN_WEREMUMAK) u.ulycn = PM_WEREMUMAK;
			if (u.ulycn == PM_HUMAN_WERECENTAUR) u.ulycn = PM_WERECENTAUR;
			if (u.ulycn == PM_HUMAN_WERECOUATL) u.ulycn = PM_WERECOUATL;
			if (u.ulycn == PM_HUMAN_WEREGELATINOUSCUBE) u.ulycn = PM_WEREGELATINOUSCUBE;
			if (u.ulycn == PM_HUMAN_WEREAIRELEMENTAL) u.ulycn = PM_WEREAIRELEMENTAL;
			if (u.ulycn == PM_HUMAN_WEREEARTHELEMENTAL) u.ulycn = PM_WEREEARTHELEMENTAL;
			if (u.ulycn == PM_HUMAN_WEREFIREELEMENTAL) u.ulycn = PM_WEREFIREELEMENTAL;
			if (u.ulycn == PM_HUMAN_WEREWATERELEMENTAL) u.ulycn = PM_WEREWATERELEMENTAL;
			if (u.ulycn == PM_HUMAN_WEREASTRALELEMENTAL) u.ulycn = PM_WEREASTRALELEMENTAL;
			if (u.ulycn == PM_HUMAN_WEREXORN) u.ulycn = PM_WEREXORN;
			if (u.ulycn == PM_HUMAN_WERESTALKER) u.ulycn = PM_WERESTALKER;
			if (u.ulycn == PM_HUMAN_WEREDRACONIAN) u.ulycn = PM_WEREDRACONIAN;
			if (u.ulycn == PM_HUMAN_WEREUMBERHULK) u.ulycn = PM_WEREUMBERHULK;
			if (u.ulycn == PM_HUMAN_WEREVAMPIRE) u.ulycn = PM_WEREVAMPIRE;
			if (u.ulycn == PM_HUMAN_WEREKILLERBEE) u.ulycn = PM_WEREKILLERBEE;
			if (u.ulycn == PM_HUMAN_WEREKANGAROO) u.ulycn = PM_WEREKANGAROO;
			if (u.ulycn == PM_HUMAN_WEREGRUE) u.ulycn = PM_WEREGRUE;
			if (u.ulycn == PM_HUMAN_WERECOINS) u.ulycn = PM_WERECOINS;
			if (u.ulycn == PM_HUMAN_WERETRAPPER) u.ulycn = PM_WERETRAPPER;
			if (u.ulycn == PM_HUMAN_WERESHADE) u.ulycn = PM_WERESHADE;
			if (u.ulycn == PM_HUMAN_WEREDISENCHANTER) u.ulycn = PM_WEREDISENCHANTER;
			if (u.ulycn == PM_HUMAN_WERENAGA) u.ulycn = PM_WERENAGA;
			if (u.ulycn == PM_HUMAN_WEREWORM) u.ulycn = PM_WEREWORM;
			if (u.ulycn == PM_HUMAN_WEREDRAGON) u.ulycn = PM_WEREDRAGON;
			if (u.ulycn == PM_HUMAN_WEREWEDGESANDAL) u.ulycn = PM_WEREWEDGESANDAL;
			if (u.ulycn == PM_HUMAN_WEREHUGGINGBOOT) u.ulycn = PM_WEREHUGGINGBOOT;
			if (u.ulycn == PM_HUMAN_WEREPEEPTOE) u.ulycn = PM_WEREPEEPTOE;
			if (u.ulycn == PM_HUMAN_WERESEXYLEATHERPUMP) u.ulycn = PM_WERESEXYLEATHERPUMP;
			if (u.ulycn == PM_HUMAN_WEREBLOCKHEELEDCOMBATBOOT) u.ulycn = PM_WEREBLOCKHEELEDCOMBATBOOT;
			if (u.ulycn == PM_HUMAN_WERECOMBATSTILETTO) u.ulycn = PM_WERECOMBATSTILETTO;
			if (u.ulycn == PM_HUMAN_WEREBEAUTIFULFUNNELHEELEDPUMP) u.ulycn = PM_WEREBEAUTIFULFUNNELHEELEDPUMP;
			if (u.ulycn == PM_HUMAN_WEREPROSTITUTESHOE) u.ulycn = PM_WEREPROSTITUTESHOE;
			if (u.ulycn == PM_HUMAN_WERESTILETTOSANDAL) u.ulycn = PM_WERESTILETTOSANDAL;
			if (u.ulycn == PM_HUMAN_WEREUNFAIRSTILETTO) u.ulycn = PM_WEREUNFAIRSTILETTO;
			if (u.ulycn == PM_HUMAN_WEREWINTERSTILETTO) u.ulycn = PM_WEREWINTERSTILETTO;
		    upermonst.mflags2 |= (M2_WERE);
		}
		break;
	    case AD_SGLD:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (youmonst.data->mlet == mdat->mlet) break;
		if(mtmp->mcan) break;
		if (!issoviet && !rn2(3)) {
			You_feel("a tug on your purse"); break;
		}
		if (rn2(10)) {stealgold(mtmp);
		break;
		}
		/* fall through --Amy */

	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
		if (mattk->aatyp == AT_KICK) hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (is_animal(mtmp->data)) {
			hitmsg(mtmp, mattk);
			if (mtmp->mcan) break;
			/* Continue below */
		} else if (rn2(5) && !(u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) || (uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_COPPERED_OFF_FROM_ME) || have_stealerstone() || (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) ) && (dmgtype(youmonst.data, AD_SEDU) || (uarmg && uarmg->oartifact == ART_LORSKEL_S_SPECIAL_PROTECTI) || (ublindf && ublindf->oartifact == ART_CLICKPASS) || (uwep && uwep->oartifact == ART_ONE_HUNDRED_STARS)
			|| dmgtype(youmonst.data, AD_SSEX)
						) ) {
			pline("%s %s.", Monnam(mtmp), mtmp->minvent ?
		    "brags about the goods some dungeon explorer provided" :
		    "makes some remarks about how difficult theft is lately");
			if ( (issoviet || !rn2(4)) && !tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
			return 3;
		} else if (mtmp->mcan) {
		    if (!Blind) {
			/*
			 * We use flags.female here on the basis that the
			 * monster chooses whether to charm or to seduce
			 * based on your visible gender.  --ALI
			 */
			int do_charm = is_neuter(mdat) || \
			  flags.female == mtmp->female;
			pline("%s tries to %s you, but you seem %s.",
			    Adjmonnam(mtmp, "plain"),
			    do_charm ? "charm" : "seduce",
			    do_charm ? "unaffected" : "uninterested");
		    }
		    if(rn2(3)) {
			if ( (issoviet || !rn2(4)) && !tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
			return 3;
		    }
		    break;
		}

		if (!rn2(3) && atttyp != AD_SEDU && !issoviet && !(u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) || have_stealerstone() || (uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) ) ) {
			You_feel("a tug on your knapsack"); break;
		}

		if (!rn2(5) && uarmc && itemhasappearance(uarmc, APP_SLEXUAL_CLOAK)) {
			if(could_seduceX(mtmp, &youmonst, mattk) == 1 && !mtmp->mcan)
			    if (doseduce(mtmp)) return 3;
		}

		if (!rn2(RngeSlexuality ? 5 : 25)) { /* Nymphs want to have some fun! :-) --Amy */

			if(could_seduceX(mtmp, &youmonst, mattk) == 1 && !mtmp->mcan)
			    if (doseduce(mtmp)) return 3;
		/* What kind of male person wouldn't want to be seduced by such a wonderfully lovely, sweet lady? */

		}

		if (mtmp->data == &mons[PM_TAUNTBUGGER] && multi >= 0) {
			pline("%s taunts you relentlessly!", Monnam(mtmp));
			nomul(-5, "being taunted relentlessly", FALSE);
		}

		buf[0] = '\0';

		if (u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) || (uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_COPPERED_OFF_FROM_ME) || have_stealerstone() || (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) ) {
			switch (steal(mtmp, buf, atttyp == AD_SEDU ? TRUE : FALSE, FALSE)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if (!is_animal(mtmp->data) && !tele_restrict(mtmp) && (issoviet || atttyp == AD_SEDU || !rn2(4)) )
			    (void) rloc(mtmp, FALSE);
			if (is_animal(mtmp->data) && *buf) {
			    if (!isevilvariant && canseemon(mtmp))
				pline("%s tries to %s away with %s.",
				      Monnam(mtmp),
				      locomotion(mtmp->data, "run"),
				      buf);
			}
			monflee(mtmp, rnd(10), FALSE, FALSE);
			return 3;
			};

		} else if (issoviet || (  (rnd(100) > ACURR(A_CHA)) && ((mtmp->female) && !flags.female && rn2(5) ) || ((!mtmp->female) && flags.female && rn2(3) ) || 
			((mtmp->female) && flags.female && rn2(2) ) || ((!mtmp->female) && !flags.female && rn2(2) ) ) )
/* male characters are more susceptible to nymphs --Amy */
			{ switch (steal(mtmp, buf, atttyp == AD_SEDU ? TRUE : FALSE, FALSE)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if (!is_animal(mtmp->data) && !tele_restrict(mtmp) && (issoviet || !rn2(4) ) )
			    (void) rloc(mtmp, FALSE);
			if (is_animal(mtmp->data) && *buf) {
			    if (!isevilvariant && canseemon(mtmp))
				pline("%s tries to %s away with %s.",
				      Monnam(mtmp),
				      locomotion(mtmp->data, "run"),
				      buf);
			}
			monflee(mtmp, rnd(10), FALSE, FALSE);
			return 3;
			};
		}
		break;
	    case AD_SSEX:

		if (flags.female && uarm && uarm->oartifact == ART_PRECIOUS_VIRGINITY) break;
		if (statsavingthrow) break;

		if(could_seduceX(mtmp, &youmonst, mattk) == 1
			&& !mtmp->mcan && (issoviet || iserosator || rn2(2) || (uarmf && uarmf->oartifact == ART_LORSKEL_S_INTEGRITY) || (uwep && uwep->oartifact == ART_MARTHA_S_FOREIGN_GOER) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_MARTHA_S_FOREIGN_GOER) || (uarmf && itemhasappearance(uarmf, APP_LOLITA_BOOTS)) ) ) /* 50% chance --Amy */
		    if (doseduce(mtmp))
			return 3;
		break;
	    case AD_SAMU:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		/* when the Wiz hits, 1/20 steals the amulet */
	      if (!rn2(20)) stealamulet(mtmp);
		break;

	    case AD_TLPT:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    if(flags.verbose)
			Your("position suddenly seems very uncertain!");
		    teleX();
		}
		break;

	    case AD_ABDC:
		hitmsg(mtmp, mattk);
		if (!rn2(3)) {
		    if(flags.verbose)
			Your("position suddenly seems very uncertain!");
		    teleX();
		}
		break;

	    case AD_WEEP:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		/* if vampire biting (and also a pet) */
		if (!rn2(3) && (!u.uevent.udemigod || u.freeplaymode) && !playerlevelportdisabled() ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.levelporting) {
				u.levelporting = 1;
				nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
			}

		}
		else if (!rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) ) && (u.urmaxlvlUP >= 2) ) {
		    losexp("loss of potential", FALSE, TRUE);
		}
		break;

	    case AD_BANI:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (!rn2(3)) {
			if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed)) ) { pline("You shudder for a moment."); (void) safe_teleds_normalterrain(FALSE); break;}
			if (playerlevelportdisabled()) {
				pline("For some reason you resist the banishment!");
				break;
			}

			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.banishmentbeam) {
				u.banishmentbeam = 1;
				nomul(-2, "being banished", FALSE); /* because it's not called until you get another turn... */
			}
		}
		break;

	    case AD_DISP:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		pushplayer(FALSE);
		break;

	    case AD_RUST:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (u.umonnum == PM_IRON_GOLEM) {
			You("rust!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		}
		if (evilfriday || rn2(3)) hurtarmor(AD_RUST);
		break;
	    case AD_CORR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (evilfriday || rn2(3)) hurtarmor(AD_CORR);
		break;
	    case AD_WTHR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (evilfriday || rn2(3)) witherarmor();
		break;

	    case AD_SHRD:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		struct obj *obj = some_armor(&youmonst);

		if (obj && drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		} else if (obj && rn2(3)) wither_dmg(obj, xname(obj), rn2(4), FALSE, &youmonst);
		
		break;

	    case AD_NPRO:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (!rn2(3)) {
			u.negativeprotection++;
			if (evilfriday && u.ublessed > 0) {
				u.ublessed -= 1;
				if (u.ublessed < 0) u.ublessed = 0;
			}
			You_feel("less protected!");
		}
		break;

	    case AD_CHKH:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		dmg += u.chokhmahdamage;
		dmg += rnd(u.ualign.sins > 0 ? (isqrt(u.ualign.sins) + 1) : (1));
		u.chokhmahdamage++;
		break;

	    case AD_THIR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		pline("%s sucks your %s!", Monnam(mtmp), body_part(BLOOD) );
		if (PlayerHearsSoundEffects) pline(issoviet ? "A u vas yest' dostatochno sil'nyye oruzhiye dlya preodoleniya zazhivleniyu monstra?" : "Fffffffff!");
		mtmp->mhp += dmg;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

		break;

	    case AD_NTHR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		pline("%s sucks your %s!", Monnam(mtmp), body_part(BLOOD) );
		if (PlayerHearsSoundEffects) pline(issoviet ? "A u vas yest' dostatochno sil'nyye oruzhiye dlya preodoleniya zazhivleniyu monstra?" : "Fffffffff!");
		mtmp->mhp += dmg;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

		if ((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && u.uexp > 100) {
			u.uexp -= (u.uexp / 100);
			You_feel("your life slipping away!");
			if (u.uexp < newuexp(u.ulevel - 1)) {
			      losexp("nether forces", TRUE, FALSE);
			}
		}

		break;

	    case AD_AGGR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		incr_itimeout(&HAggravate_monster, dmg);
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();
		if (!rn2(20)) {

			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		break;

	    case AD_DATA:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		u.datadeletedefer = 1;
		datadeleteattack();

		break;

	    case AD_RAGN:

		hitmsg(mtmp, mattk);
		ragnarok(FALSE);
		if (evilfriday && mtmp->m_lev > 1) evilragnarok(FALSE,mtmp->m_lev);

		break;

	    case AD_DEST:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		destroyarmorattack();

		break;

	    case AD_MINA:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		/* No messages for AD_MINA. This is intentional. It's a *nasty* attack. --Amy */

		{
			register int midentity = mtmp->m_id;
			if (midentity < 0) midentity *= -1;
			while (midentity > 235) midentity -= 235;

			register int nastyduration = ((dmg + 2) * rnd(10));
			if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) nastyduration *= 20;

			switch (midentity) {

				case 1: RMBLoss += rnz(nastyduration); break;
				case 2: NoDropProblem += rnz(nastyduration); break;
				case 3: DSTWProblem += rnz(nastyduration); break;
				case 4: StatusTrapProblem += rnz(nastyduration); 
					if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
					if (HStun) set_itimeout(&HeavyStunned, HStun);
					if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
					if (HFeared) set_itimeout(&HeavyFeared, HFeared);
					if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
					if (HBurned) set_itimeout(&HeavyBurned, HBurned);
					if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
					if (Blinded) set_itimeout(&HeavyBlind, Blinded);
					if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
					break;
				case 5: Superscroller += rnz(nastyduration * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz(nastyduration); break;
				case 7: FreeHandLoss += rnz(nastyduration); break;
				case 8: Unidentify += rnz(nastyduration); break;
				case 9: Thirst += rnz(nastyduration); break;
				case 10: LuckLoss += rnz(nastyduration); break;
				case 11: ShadesOfGrey += rnz(nastyduration); break;
				case 12: FaintActive += rnz(nastyduration); break;
				case 13: Itemcursing += rnz(nastyduration); break;
				case 14: DifficultyIncreased += rnz(nastyduration); break;
				case 15: Deafness += rnz(nastyduration); flags.soundok = 0; break;
				case 16: CasterProblem += rnz(nastyduration); break;
				case 17: WeaknessProblem += rnz(nastyduration); break;
				case 18: RotThirteen += rnz(nastyduration); break;
				case 19: BishopGridbug += rnz(nastyduration); break;
				case 20: UninformationProblem += rnz(nastyduration); break;
				case 21: StairsProblem += rnz(nastyduration); break;
				case 22: AlignmentProblem += rnz(nastyduration); break;
				case 23: ConfusionProblem += rnz(nastyduration); break;
				case 24: SpeedBug += rnz(nastyduration); break;
				case 25: DisplayLoss += rnz(nastyduration); break;
				case 26: SpellLoss += rnz(nastyduration); break;
				case 27: YellowSpells += rnz(nastyduration); break;
				case 28: AutoDestruct += rnz(nastyduration); break;
				case 29: MemoryLoss += rnz(nastyduration); break;
				case 30: InventoryLoss += rnz(nastyduration); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (dmg * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz(nastyduration); break;
				case 33: BloodLossProblem += rnz(nastyduration); break;
				case 34: BadEffectProblem += rnz(nastyduration); break;
				case 35: TrapCreationProblem += rnz(nastyduration); break;
				case 36: AutomaticVulnerabilitiy += rnz(nastyduration); break;
				case 37: TeleportingItems += rnz(nastyduration); break;
				case 38: NastinessProblem += rnz(nastyduration); break;
				case 39: CaptchaProblem += rnz(nastyduration); break;
				case 40: FarlookProblem += rnz(nastyduration); break;
				case 41: RespawnProblem += rnz(nastyduration); break;
				case 42: RecurringAmnesia += rnz(nastyduration); break;
				case 43: BigscriptEffect += rnz(nastyduration); break;
				case 44: {
					BankTrapEffect += rnz(nastyduration);
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz(nastyduration); break;
				case 46: TechTrapEffect += rnz(nastyduration); break;
				case 47: RecurringDisenchant += rnz(nastyduration); break;
				case 48: verisiertEffect += rnz(nastyduration); break;
				case 49: ChaosTerrain += rnz(nastyduration); break;
				case 50: Muteness += rnz(nastyduration); break;
				case 51: EngravingDoesntWork += rnz(nastyduration); break;
				case 52: MagicDeviceEffect += rnz(nastyduration); break;
				case 53: BookTrapEffect += rnz(nastyduration); break;
				case 54: LevelTrapEffect += rnz(nastyduration); break;
				case 55: QuizTrapEffect += rnz(nastyduration); break;
				case 56: FastMetabolismEffect += rnz(nastyduration); break;
				case 57: NoReturnEffect += rnz(nastyduration); break;
				case 58: AlwaysEgotypeMonsters += rnz(nastyduration); break;
				case 59: TimeGoesByFaster += rnz(nastyduration); break;
				case 60: FoodIsAlwaysRotten += rnz(nastyduration); break;
				case 61: AllSkillsUnskilled += rnz(nastyduration); break;
				case 62: AllStatsAreLower += rnz(nastyduration); break;
				case 63: PlayerCannotTrainSkills += rnz(nastyduration); break;
				case 64: PlayerCannotExerciseStats += rnz(nastyduration); break;
				case 65: TurnLimitation += rnz(nastyduration); break;
				case 66: WeakSight += rnz(nastyduration); break;
				case 67: RandomMessages += rnz(nastyduration); break;

				case 68: Desecration += rnz(nastyduration); break;
				case 69: StarvationEffect += rnz(nastyduration); break;
				case 70: NoDropsEffect += rnz(nastyduration); break;
				case 71: LowEffects += rnz(nastyduration); break;
				case 72: InvisibleTrapsEffect += rnz(nastyduration); break;
				case 73: GhostWorld += rnz(nastyduration); break;
				case 74: Dehydration += rnz(nastyduration); break;
				case 75: HateTrapEffect += rnz(nastyduration); break;
				case 76: TotterTrapEffect += rnz(nastyduration); break;
				case 77: Nonintrinsics += rnz(nastyduration); break;
				case 78: Dropcurses += rnz(nastyduration); break;
				case 79: Nakedness += rnz(nastyduration); break;
				case 80: Antileveling += rnz(nastyduration); break;
				case 81: ItemStealingEffect += rnz(nastyduration); break;
				case 82: Rebellions += rnz(nastyduration); break;
				case 83: CrapEffect += rnz(nastyduration); break;
				case 84: ProjectilesMisfire += rnz(nastyduration); break;
				case 85: WallTrapping += rnz(nastyduration); break;
				case 86: DisconnectedStairs += rnz(nastyduration); break;
				case 87: InterfaceScrewed += rnz(nastyduration); break;
				case 88: Bossfights += rnz(nastyduration); break;
				case 89: EntireLevelMode += rnz(nastyduration); break;
				case 90: BonesLevelChange += rnz(nastyduration); break;
				case 91: AutocursingEquipment += rnz(nastyduration); break;
				case 92: HighlevelStatus += rnz(nastyduration); break;
				case 93: SpellForgetting += rnz(nastyduration); break;
				case 94: SoundEffectBug += rnz(nastyduration); break;
				case 95: TimerunBug += rnz(nastyduration); break;
				case 96: LootcutBug += rnz(nastyduration); break;
				case 97: MonsterSpeedBug += rnz(nastyduration); break;
				case 98: ScalingBug += rnz(nastyduration); break;
				case 99: EnmityBug += rnz(nastyduration); break;
				case 100: WhiteSpells += rnz(nastyduration); break;
				case 101: CompleteGraySpells += rnz(nastyduration); break;
				case 102: QuasarVision += rnz(nastyduration); break;
				case 103: MommaBugEffect += rnz(nastyduration); break;
				case 104: HorrorBugEffect += rnz(nastyduration); break;
				case 105: ArtificerBug += rnz(nastyduration); break;
				case 106: WereformBug += rnz(nastyduration); break;
				case 107: NonprayerBug += rnz(nastyduration); break;
				case 108: EvilPatchEffect += rnz(nastyduration); break;
				case 109: HardModeEffect += rnz(nastyduration); break;
				case 110: SecretAttackBug += rnz(nastyduration); break;
				case 111: EaterBugEffect += rnz(nastyduration); break;
				case 112: CovetousnessBug += rnz(nastyduration); break;
				case 113: NotSeenBug += rnz(nastyduration); break;
				case 114: DarkModeBug += rnz(nastyduration); break;
				case 115: AntisearchEffect += rnz(nastyduration); break;
				case 116: HomicideEffect += rnz(nastyduration); break;
				case 117: NastynationBug += rnz(nastyduration); break;
				case 118: WakeupCallBug += rnz(nastyduration); break;
				case 119: GrayoutBug += rnz(nastyduration); break;
				case 120: GrayCenterBug += rnz(nastyduration); break;
				case 121: CheckerboardBug += rnz(nastyduration); break;
				case 122: ClockwiseSpinBug += rnz(nastyduration); break;
				case 123: CounterclockwiseSpin += rnz(nastyduration); break;
				case 124: LagBugEffect += rnz(nastyduration); break;
				case 125: BlesscurseEffect += rnz(nastyduration); break;
				case 126: DeLightBug += rnz(nastyduration); break;
				case 127: DischargeBug += rnz(nastyduration); break;
				case 128: TrashingBugEffect += rnz(nastyduration); break;
				case 129: FilteringBug += rnz(nastyduration); break;
				case 130: DeformattingBug += rnz(nastyduration); break;
				case 131: FlickerStripBug += rnz(nastyduration); break;
				case 132: UndressingEffect += rnz(nastyduration); break;
				case 133: Hyperbluewalls += rnz(nastyduration); break;
				case 134: NoliteBug += rnz(nastyduration); break;
				case 135: ParanoiaBugEffect += rnz(nastyduration); break;
				case 136: FleecescriptBug += rnz(nastyduration); break;
				case 137: InterruptEffect += rnz(nastyduration); break;
				case 138: DustbinBug += rnz(nastyduration); break;
				case 139: ManaBatteryBug += rnz(nastyduration); break;
				case 140: Monsterfingers += rnz(nastyduration); break;
				case 141: MiscastBug += rnz(nastyduration); break;
				case 142: MessageSuppression += rnz(nastyduration); break;
				case 143: StuckAnnouncement += rnz(nastyduration); break;
				case 144: BloodthirstyEffect += rnz(nastyduration); break;
				case 145: MaximumDamageBug += rnz(nastyduration); break;
				case 146: LatencyBugEffect += rnz(nastyduration); break;
				case 147: StarlitBug += rnz(nastyduration); break;
				case 148: KnowledgeBug += rnz(nastyduration); break;
				case 149: HighscoreBug += rnz(nastyduration); break;
				case 150: PinkSpells += rnz(nastyduration); break;
				case 151: GreenSpells += rnz(nastyduration); break;
				case 152: EvencoreEffect += rnz(nastyduration); break;
				case 153: UnderlayerBug += rnz(nastyduration); break;
				case 154: DamageMeterBug += rnz(nastyduration); break;
				case 155: ArbitraryWeightBug += rnz(nastyduration); break;
				case 156: FuckedInfoBug += rnz(nastyduration); break;
				case 157: BlackSpells += rnz(nastyduration); break;
				case 158: CyanSpells += rnz(nastyduration); break;
				case 159: HeapEffectBug += rnz(nastyduration); break;
				case 160: BlueSpells += rnz(nastyduration); break;
				case 161: TronEffect += rnz(nastyduration); break;
				case 162: RedSpells += rnz(nastyduration); break;
				case 163: TooHeavyEffect += rnz(nastyduration); break;
				case 164: ElongationBug += rnz(nastyduration); break;
				case 165: WrapoverEffect += rnz(nastyduration); break;
				case 166: DestructionEffect += rnz(nastyduration); break;
				case 167: MeleePrefixBug += rnz(nastyduration); break;
				case 168: AutomoreBug += rnz(nastyduration); break;
				case 169: UnfairAttackBug += rnz(nastyduration); break;
				case 170: OrangeSpells += rnz(nastyduration); break;
				case 171: VioletSpells += rnz(nastyduration); break;
				case 172: LongingEffect += rnz(nastyduration); break;
				case 173: CursedParts += rnz(nastyduration); break;
				case 174: Quaversal += rnz(nastyduration); break;
				case 175: AppearanceShuffling += rnz(nastyduration); break;
				case 176: BrownSpells += rnz(nastyduration); break;
				case 177: Choicelessness += rnz(nastyduration); break;
				case 178: Goldspells += rnz(nastyduration); break;
				case 179: Deprovement += rnz(nastyduration); break;
				case 180: InitializationFail += rnz(nastyduration); break;
				case 181: GushlushEffect += rnz(nastyduration); break;
				case 182: SoiltypeEffect += rnz(nastyduration); break;
				case 183: DangerousTerrains += rnz(nastyduration); break;
				case 184: FalloutEffect += rnz(nastyduration); break;
				case 185: MojibakeEffect += rnz(nastyduration); break;
				case 186: GravationEffect += rnz(nastyduration); break;
				case 187: UncalledEffect += rnz(nastyduration); break;
				case 188: ExplodingDiceEffect += rnz(nastyduration); break;
				case 189: PermacurseEffect += rnz(nastyduration); break;
				case 190: ShroudedIdentity += rnz(nastyduration); break;
				case 191: FeelerGauges += rnz(nastyduration); break;
				case 192: LongScrewup += rnz(nastyduration * 20); break;
				case 193: WingYellowChange += rnz(nastyduration); break;
				case 194: LifeSavingBug += rnz(nastyduration); break;
				case 195: CurseuseEffect += rnz(nastyduration); break;
				case 196: CutNutritionEffect += rnz(nastyduration); break;
				case 197: SkillLossEffect += rnz(nastyduration); break;
				case 198: AutopilotEffect += rnz(nastyduration); break;
				case 199: MysteriousForceActive += rnz(nastyduration); break;
				case 200: MonsterGlyphChange += rnz(nastyduration); break;
				case 201: ChangingDirectives += rnz(nastyduration); break;
				case 202: ContainerKaboom += rnz(nastyduration); break;
				case 203: StealDegrading += rnz(nastyduration); break;
				case 204: LeftInventoryBug += rnz(nastyduration); break;
				case 205: FluctuatingSpeed += rnz(nastyduration); break;
				case 206: TarmuStrokingNora += rnz(nastyduration); break;
				case 207: FailureEffects += rnz(nastyduration); break;
				case 208: BrightCyanSpells += rnz(nastyduration); break;
				case 209: FrequentationSpawns += rnz(nastyduration); break;
				case 210: PetAIScrewed += rnz(nastyduration); break;
				case 211: SatanEffect += rnz(nastyduration); break;
				case 212: RememberanceEffect += rnz(nastyduration); break;
				case 213: PokelieEffect += rnz(nastyduration); break;
				case 214: AlwaysAutopickup += rnz(nastyduration); break;
				case 215: DywypiProblem += rnz(nastyduration); break;
				case 216: SilverSpells += rnz(nastyduration); break;
				case 217: MetalSpells += rnz(nastyduration); break;
				case 218: PlatinumSpells += rnz(nastyduration); break;
				case 219: ManlerEffect += rnz(nastyduration); break;
				case 220: DoorningEffect += rnz(nastyduration); break;
				case 221: NownsibleEffect += rnz(nastyduration); break;
				case 222: ElmStreetEffect += rnz(nastyduration); break;
				case 223: MonnoiseEffect += rnz(nastyduration); break;
				case 224: RangCallEffect += rnz(nastyduration); break;
				case 225: RecurringSpellLoss += rnz(nastyduration); break;
				case 226: AntitrainingEffect += rnz(nastyduration); break;
				case 227: TechoutBug += rnz(nastyduration); break;
				case 228: StatDecay += rnz(nastyduration); break;
				case 229: Movemork += rnz(nastyduration); break;
				case 230: BadPartBug += rnz(nastyduration); break;
				case 231: CompletelyBadPartBug += rnz(nastyduration); break;
				case 232: EvilVariantActive += rnz(nastyduration); break;
				case 233: SanityTrebleEffect += rnz(nastyduration); break;
				case 234: StatDecreaseBug += rnz(nastyduration); break;
				case 235: SimeoutBug += rnz(nastyduration); break;

				default: impossible("AD_MINA called with invalid value %d", midentity); break;
			}

		}

		break;

	    case AD_RUNS:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		/* No messages for AD_RUNS. This is intentional. It's a *nasty* attack. --Amy */

		{
			register int nastyduration = ((dmg + 2) * rnd(10));
			if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) nastyduration *= 20;

			switch (u.adrunsattack) {

				case 1: RMBLoss += rnz(nastyduration); break;
				case 2: NoDropProblem += rnz(nastyduration); break;
				case 3: DSTWProblem += rnz(nastyduration); break;
				case 4: StatusTrapProblem += rnz(nastyduration); 
					if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
					if (HStun) set_itimeout(&HeavyStunned, HStun);
					if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
					if (HFeared) set_itimeout(&HeavyFeared, HFeared);
					if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
					if (HBurned) set_itimeout(&HeavyBurned, HBurned);
					if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
					if (Blinded) set_itimeout(&HeavyBlind, Blinded);
					if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
					break;
				case 5: Superscroller += rnz(nastyduration * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz(nastyduration); break;
				case 7: FreeHandLoss += rnz(nastyduration); break;
				case 8: Unidentify += rnz(nastyduration); break;
				case 9: Thirst += rnz(nastyduration); break;
				case 10: LuckLoss += rnz(nastyduration); break;
				case 11: ShadesOfGrey += rnz(nastyduration); break;
				case 12: FaintActive += rnz(nastyduration); break;
				case 13: Itemcursing += rnz(nastyduration); break;
				case 14: DifficultyIncreased += rnz(nastyduration); break;
				case 15: Deafness += rnz(nastyduration); flags.soundok = 0; break;
				case 16: CasterProblem += rnz(nastyduration); break;
				case 17: WeaknessProblem += rnz(nastyduration); break;
				case 18: RotThirteen += rnz(nastyduration); break;
				case 19: BishopGridbug += rnz(nastyduration); break;
				case 20: UninformationProblem += rnz(nastyduration); break;
				case 21: StairsProblem += rnz(nastyduration); break;
				case 22: AlignmentProblem += rnz(nastyduration); break;
				case 23: ConfusionProblem += rnz(nastyduration); break;
				case 24: SpeedBug += rnz(nastyduration); break;
				case 25: DisplayLoss += rnz(nastyduration); break;
				case 26: SpellLoss += rnz(nastyduration); break;
				case 27: YellowSpells += rnz(nastyduration); break;
				case 28: AutoDestruct += rnz(nastyduration); break;
				case 29: MemoryLoss += rnz(nastyduration); break;
				case 30: InventoryLoss += rnz(nastyduration); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (dmg * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz(nastyduration); break;
				case 33: BloodLossProblem += rnz(nastyduration); break;
				case 34: BadEffectProblem += rnz(nastyduration); break;
				case 35: TrapCreationProblem += rnz(nastyduration); break;
				case 36: AutomaticVulnerabilitiy += rnz(nastyduration); break;
				case 37: TeleportingItems += rnz(nastyduration); break;
				case 38: NastinessProblem += rnz(nastyduration); break;
				case 39: CaptchaProblem += rnz(nastyduration); break;
				case 40: FarlookProblem += rnz(nastyduration); break;
				case 41: RespawnProblem += rnz(nastyduration); break;
				case 42: RecurringAmnesia += rnz(nastyduration); break;
				case 43: BigscriptEffect += rnz(nastyduration); break;
				case 44: {
					BankTrapEffect += rnz(nastyduration);
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz(nastyduration); break;
				case 46: TechTrapEffect += rnz(nastyduration); break;
				case 47: RecurringDisenchant += rnz(nastyduration); break;
				case 48: verisiertEffect += rnz(nastyduration); break;
				case 49: ChaosTerrain += rnz(nastyduration); break;
				case 50: Muteness += rnz(nastyduration); break;
				case 51: EngravingDoesntWork += rnz(nastyduration); break;
				case 52: MagicDeviceEffect += rnz(nastyduration); break;
				case 53: BookTrapEffect += rnz(nastyduration); break;
				case 54: LevelTrapEffect += rnz(nastyduration); break;
				case 55: QuizTrapEffect += rnz(nastyduration); break;
				case 56: FastMetabolismEffect += rnz(nastyduration); break;
				case 57: NoReturnEffect += rnz(nastyduration); break;
				case 58: AlwaysEgotypeMonsters += rnz(nastyduration); break;
				case 59: TimeGoesByFaster += rnz(nastyduration); break;
				case 60: FoodIsAlwaysRotten += rnz(nastyduration); break;
				case 61: AllSkillsUnskilled += rnz(nastyduration); break;
				case 62: AllStatsAreLower += rnz(nastyduration); break;
				case 63: PlayerCannotTrainSkills += rnz(nastyduration); break;
				case 64: PlayerCannotExerciseStats += rnz(nastyduration); break;
				case 65: TurnLimitation += rnz(nastyduration); break;
				case 66: WeakSight += rnz(nastyduration); break;
				case 67: RandomMessages += rnz(nastyduration); break;

				case 68: Desecration += rnz(nastyduration); break;
				case 69: StarvationEffect += rnz(nastyduration); break;
				case 70: NoDropsEffect += rnz(nastyduration); break;
				case 71: LowEffects += rnz(nastyduration); break;
				case 72: InvisibleTrapsEffect += rnz(nastyduration); break;
				case 73: GhostWorld += rnz(nastyduration); break;
				case 74: Dehydration += rnz(nastyduration); break;
				case 75: HateTrapEffect += rnz(nastyduration); break;
				case 76: TotterTrapEffect += rnz(nastyduration); break;
				case 77: Nonintrinsics += rnz(nastyduration); break;
				case 78: Dropcurses += rnz(nastyduration); break;
				case 79: Nakedness += rnz(nastyduration); break;
				case 80: Antileveling += rnz(nastyduration); break;
				case 81: ItemStealingEffect += rnz(nastyduration); break;
				case 82: Rebellions += rnz(nastyduration); break;
				case 83: CrapEffect += rnz(nastyduration); break;
				case 84: ProjectilesMisfire += rnz(nastyduration); break;
				case 85: WallTrapping += rnz(nastyduration); break;
				case 86: DisconnectedStairs += rnz(nastyduration); break;
				case 87: InterfaceScrewed += rnz(nastyduration); break;
				case 88: Bossfights += rnz(nastyduration); break;
				case 89: EntireLevelMode += rnz(nastyduration); break;
				case 90: BonesLevelChange += rnz(nastyduration); break;
				case 91: AutocursingEquipment += rnz(nastyduration); break;
				case 92: HighlevelStatus += rnz(nastyduration); break;
				case 93: SpellForgetting += rnz(nastyduration); break;
				case 94: SoundEffectBug += rnz(nastyduration); break;
				case 95: TimerunBug += rnz(nastyduration); break;
				case 96: LootcutBug += rnz(nastyduration); break;
				case 97: MonsterSpeedBug += rnz(nastyduration); break;
				case 98: ScalingBug += rnz(nastyduration); break;
				case 99: EnmityBug += rnz(nastyduration); break;
				case 100: WhiteSpells += rnz(nastyduration); break;
				case 101: CompleteGraySpells += rnz(nastyduration); break;
				case 102: QuasarVision += rnz(nastyduration); break;
				case 103: MommaBugEffect += rnz(nastyduration); break;
				case 104: HorrorBugEffect += rnz(nastyduration); break;
				case 105: ArtificerBug += rnz(nastyduration); break;
				case 106: WereformBug += rnz(nastyduration); break;
				case 107: NonprayerBug += rnz(nastyduration); break;
				case 108: EvilPatchEffect += rnz(nastyduration); break;
				case 109: HardModeEffect += rnz(nastyduration); break;
				case 110: SecretAttackBug += rnz(nastyduration); break;
				case 111: EaterBugEffect += rnz(nastyduration); break;
				case 112: CovetousnessBug += rnz(nastyduration); break;
				case 113: NotSeenBug += rnz(nastyduration); break;
				case 114: DarkModeBug += rnz(nastyduration); break;
				case 115: AntisearchEffect += rnz(nastyduration); break;
				case 116: HomicideEffect += rnz(nastyduration); break;
				case 117: NastynationBug += rnz(nastyduration); break;
				case 118: WakeupCallBug += rnz(nastyduration); break;
				case 119: GrayoutBug += rnz(nastyduration); break;
				case 120: GrayCenterBug += rnz(nastyduration); break;
				case 121: CheckerboardBug += rnz(nastyduration); break;
				case 122: ClockwiseSpinBug += rnz(nastyduration); break;
				case 123: CounterclockwiseSpin += rnz(nastyduration); break;
				case 124: LagBugEffect += rnz(nastyduration); break;
				case 125: BlesscurseEffect += rnz(nastyduration); break;
				case 126: DeLightBug += rnz(nastyduration); break;
				case 127: DischargeBug += rnz(nastyduration); break;
				case 128: TrashingBugEffect += rnz(nastyduration); break;
				case 129: FilteringBug += rnz(nastyduration); break;
				case 130: DeformattingBug += rnz(nastyduration); break;
				case 131: FlickerStripBug += rnz(nastyduration); break;
				case 132: UndressingEffect += rnz(nastyduration); break;
				case 133: Hyperbluewalls += rnz(nastyduration); break;
				case 134: NoliteBug += rnz(nastyduration); break;
				case 135: ParanoiaBugEffect += rnz(nastyduration); break;
				case 136: FleecescriptBug += rnz(nastyduration); break;
				case 137: InterruptEffect += rnz(nastyduration); break;
				case 138: DustbinBug += rnz(nastyduration); break;
				case 139: ManaBatteryBug += rnz(nastyduration); break;
				case 140: Monsterfingers += rnz(nastyduration); break;
				case 141: MiscastBug += rnz(nastyduration); break;
				case 142: MessageSuppression += rnz(nastyduration); break;
				case 143: StuckAnnouncement += rnz(nastyduration); break;
				case 144: BloodthirstyEffect += rnz(nastyduration); break;
				case 145: MaximumDamageBug += rnz(nastyduration); break;
				case 146: LatencyBugEffect += rnz(nastyduration); break;
				case 147: StarlitBug += rnz(nastyduration); break;
				case 148: KnowledgeBug += rnz(nastyduration); break;
				case 149: HighscoreBug += rnz(nastyduration); break;
				case 150: PinkSpells += rnz(nastyduration); break;
				case 151: GreenSpells += rnz(nastyduration); break;
				case 152: EvencoreEffect += rnz(nastyduration); break;
				case 153: UnderlayerBug += rnz(nastyduration); break;
				case 154: DamageMeterBug += rnz(nastyduration); break;
				case 155: ArbitraryWeightBug += rnz(nastyduration); break;
				case 156: FuckedInfoBug += rnz(nastyduration); break;
				case 157: BlackSpells += rnz(nastyduration); break;
				case 158: CyanSpells += rnz(nastyduration); break;
				case 159: HeapEffectBug += rnz(nastyduration); break;
				case 160: BlueSpells += rnz(nastyduration); break;
				case 161: TronEffect += rnz(nastyduration); break;
				case 162: RedSpells += rnz(nastyduration); break;
				case 163: TooHeavyEffect += rnz(nastyduration); break;
				case 164: ElongationBug += rnz(nastyduration); break;
				case 165: WrapoverEffect += rnz(nastyduration); break;
				case 166: DestructionEffect += rnz(nastyduration); break;
				case 167: MeleePrefixBug += rnz(nastyduration); break;
				case 168: AutomoreBug += rnz(nastyduration); break;
				case 169: UnfairAttackBug += rnz(nastyduration); break;
				case 170: OrangeSpells += rnz(nastyduration); break;
				case 171: VioletSpells += rnz(nastyduration); break;
				case 172: LongingEffect += rnz(nastyduration); break;
				case 173: CursedParts += rnz(nastyduration); break;
				case 174: Quaversal += rnz(nastyduration); break;
				case 175: AppearanceShuffling += rnz(nastyduration); break;
				case 176: BrownSpells += rnz(nastyduration); break;
				case 177: Choicelessness += rnz(nastyduration); break;
				case 178: Goldspells += rnz(nastyduration); break;
				case 179: Deprovement += rnz(nastyduration); break;
				case 180: InitializationFail += rnz(nastyduration); break;
				case 181: GushlushEffect += rnz(nastyduration); break;
				case 182: SoiltypeEffect += rnz(nastyduration); break;
				case 183: DangerousTerrains += rnz(nastyduration); break;
				case 184: FalloutEffect += rnz(nastyduration); break;
				case 185: MojibakeEffect += rnz(nastyduration); break;
				case 186: GravationEffect += rnz(nastyduration); break;
				case 187: UncalledEffect += rnz(nastyduration); break;
				case 188: ExplodingDiceEffect += rnz(nastyduration); break;
				case 189: PermacurseEffect += rnz(nastyduration); break;
				case 190: ShroudedIdentity += rnz(nastyduration); break;
				case 191: FeelerGauges += rnz(nastyduration); break;
				case 192: LongScrewup += rnz(nastyduration * 20); break;
				case 193: WingYellowChange += rnz(nastyduration); break;
				case 194: LifeSavingBug += rnz(nastyduration); break;
				case 195: CurseuseEffect += rnz(nastyduration); break;
				case 196: CutNutritionEffect += rnz(nastyduration); break;
				case 197: SkillLossEffect += rnz(nastyduration); break;
				case 198: AutopilotEffect += rnz(nastyduration); break;
				case 199: MysteriousForceActive += rnz(nastyduration); break;
				case 200: MonsterGlyphChange += rnz(nastyduration); break;
				case 201: ChangingDirectives += rnz(nastyduration); break;
				case 202: ContainerKaboom += rnz(nastyduration); break;
				case 203: StealDegrading += rnz(nastyduration); break;
				case 204: LeftInventoryBug += rnz(nastyduration); break;
				case 205: FluctuatingSpeed += rnz(nastyduration); break;
				case 206: TarmuStrokingNora += rnz(nastyduration); break;
				case 207: FailureEffects += rnz(nastyduration); break;
				case 208: BrightCyanSpells += rnz(nastyduration); break;
				case 209: FrequentationSpawns += rnz(nastyduration); break;
				case 210: PetAIScrewed += rnz(nastyduration); break;
				case 211: SatanEffect += rnz(nastyduration); break;
				case 212: RememberanceEffect += rnz(nastyduration); break;
				case 213: PokelieEffect += rnz(nastyduration); break;
				case 214: AlwaysAutopickup += rnz(nastyduration); break;
				case 215: DywypiProblem += rnz(nastyduration); break;
				case 216: SilverSpells += rnz(nastyduration); break;
				case 217: MetalSpells += rnz(nastyduration); break;
				case 218: PlatinumSpells += rnz(nastyduration); break;
				case 219: ManlerEffect += rnz(nastyduration); break;
				case 220: DoorningEffect += rnz(nastyduration); break;
				case 221: NownsibleEffect += rnz(nastyduration); break;
				case 222: ElmStreetEffect += rnz(nastyduration); break;
				case 223: MonnoiseEffect += rnz(nastyduration); break;
				case 224: RangCallEffect += rnz(nastyduration); break;
				case 225: RecurringSpellLoss += rnz(nastyduration); break;
				case 226: AntitrainingEffect += rnz(nastyduration); break;
				case 227: TechoutBug += rnz(nastyduration); break;
				case 228: StatDecay += rnz(nastyduration); break;
				case 229: Movemork += rnz(nastyduration); break;
				case 230: BadPartBug += rnz(nastyduration); break;
				case 231: CompletelyBadPartBug += rnz(nastyduration); break;
				case 232: EvilVariantActive += rnz(nastyduration); break;
				case 233: SanityTrebleEffect += rnz(nastyduration); break;
				case 234: StatDecreaseBug += rnz(nastyduration); break;
				case 235: SimeoutBug += rnz(nastyduration); break;

				default: impossible("AD_RUNS called with invalid value %d", u.adrunsattack); break;
			}

		}

		break;

	    case AD_SIN:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		You_feel("sinful!");
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-5);

		break;

	    case AD_ALIN:

		hitmsg(mtmp, mattk);
		if (uncancelled && dmg) {
			You("lose alignment points!");
			adjalign(-dmg);
		}

		break;

	    case AD_CONT:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		Your("contamination increases.");
		contaminate(dmg, TRUE);

		break;

	    case AD_SUCK:
			if (statsavingthrow) break;
			if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) dmg = 0;
			else{
				if( has_head(youmonst.data) && !(Role_if(PM_COURIER)) && !uarmh && !rn2(20) && 
					((!Upolyd && u.uhp < (u.uhpmax / 10) ) || (Upolyd && u.mh < (u.mhmax / 10) ))
				){
					dmg = 2 * (Upolyd ? u.mh : u.uhp)
						  + 400; //FATAL_DAMAGE_MODIFIER;
					pline("%s sucks your %s off!",
					      Monnam(mtmp), body_part(HEAD));
				}
				else{
					You_feel("%s trying to suck your extremities off!",mon_nam(mtmp));
					if(!rn2(10)){
						Your("%s twist from the suction!", makeplural(body_part(LEG)));
					    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    set_wounded_legs(LEFT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    exercise(A_STR, FALSE);
					    exercise(A_DEX, FALSE);
					}
					if(uwep && !rn2(6)){
						You_feel("%s pull on your weapon!",mon_nam(mtmp));
						if( rnd(130) > ACURR(A_STR)){
							Your("weapon is sucked out of your grasp!");
							optr = uwep;
							uwepgone();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						}
						else{
							You("keep a tight grip on your weapon!");
						}
					}
					if(!rn2(10) && uarmf){
						Your("boots are sucked off!");
						optr = uarmf;
						if (donning(optr)) cancel_don();
						(void) Boots_off();
						freeinv(optr);
						(void) mpickobj(mtmp,optr,FALSE);
					}
					if(!rn2(6) && uarmg && !uwep){
						You_feel("%s pull on your gloves!",mon_nam(mtmp));
						if( rnd(130) > ACURR(A_STR)){
							Your("gloves are sucked off!");
							optr = uarmg;
							if (donning(optr)) cancel_don();
							(void) Gloves_off();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						}
						else You("keep your %s closed.", makeplural(body_part(HAND)));
					}
					if(!rn2(8) && uarms){
						You_feel("%s pull on your shield!",mon_nam(mtmp));
						if( rnd(150) > ACURR(A_STR)){
							Your("shield is sucked out of your grasp!");
							optr = uarms;
							if (donning(optr)) cancel_don();
							Shield_off();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						 }
						 else{
							You("keep a tight grip on your shield!");
						 }
					}
					if(!rn2(4) && uarmh){
						Your("helmet is sucked off!");
						optr = uarmh;
						if (donning(optr)) cancel_don();
						(void) Helmet_off();
						freeinv(optr);
						(void) mpickobj(mtmp,optr,FALSE);
					}
				}
			}
		break;

	    case AD_WET:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (!rn2(3)) {
			pline("Water washes over you!");
			if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
				water_damage(invent, FALSE, FALSE);
				if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
			}
		}
		break;

	    case AD_AMNE:

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		pline("You feel reminded of Maud.");
		maprot();
		break;

	    case AD_LETH:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (!rn2(3)) {
			pline("Sparkling water washes over you!");
			lethe_damage(invent, FALSE, FALSE);
			if (!rn2(3)) actual_lethe_damage(invent, FALSE, FALSE);
			if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
			if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		}
		break;

	    case AD_CNCL:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (!rn2(3)) {
			(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		}
		break;

	    case AD_LUCK:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (!rn2(3)) {change_luck(-1);
			You_feel("unlucky.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Prosto stoyat' i poteryat' stol'ko udachi, kak vy mozhete! Eto sdelayet igru boleye legkoy dlya Vas! V samom dele! Potomu chto togda vy budete umirat' bystreye i sdelat' s ney, tak chto vy mozhete sosredotochit'sya na tom, kakoy na samom dele produktivnoy veshchi vmesto togo, chtoby, kak delat' posudu!" : "Dieuuuuuuu!");
		}
		break;

	    case AD_FAKE:
		hitmsg(mtmp, mattk);
		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}
		break;

	    case AD_NEXU:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;

		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) dmg *= (1 + rnd(2));

		switch (rnd(7)) {

			case 1:
			case 2:
			case 3:
				pline("%s sends you far away!", Monnam(mtmp) );
				teleX();
				break;
			case 4:
			case 5:
				pline("%s sends you away!", Monnam(mtmp) );
				phase_door(0);
				break;
			case 6:

				if ((!u.uevent.udemigod || u.freeplaymode) && !playerlevelportdisabled() ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
					}
				}
				break;
			case 7:
				{
					nexus_swap();

					if (!rn2(3)) {

						int reducedstat = rn2(A_MAX);
						if(ABASE(reducedstat) <= ATTRMIN(reducedstat)) {
							pline("Your health was damaged!");
							u.uhpmax -= rnd(5);
							if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
							if (u.uhp < 1) {
								u.youaredead = 1;
								killer = "nexus scrambling";
								killer_format = KILLED_BY;
								done(DIED);
								u.youaredead = 0;
							}

						} else {
							ABASE(reducedstat) -= 1;
							AMAX(reducedstat) -= 1;
							flags.botl = 1;
							pline("Your attributes were damaged!");
						}
					}

				}
				break;
		}
		break;

	    case AD_SOUN:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		pline("Your ears are blasted by hellish noise!");
		if (YouAreDeaf) dmg /= 2;
		make_stunned(HStun + dmg, TRUE);
		if (isevilvariant || !rn2(issoviet ? 2 : 5)) (void)destroy_item(POTION_CLASS, AD_COLD);
		wake_nearby();
		break;

	    case AD_GRAV:

		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) dmg *= 2;

		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		pline("Gravity warps around you...");
		phase_door(0);
		pushplayer(FALSE);
		u.uprops[DEAC_FAST].intrinsic += (dmg + 2);
		make_stunned(HStun + dmg, TRUE);
		break;

	    case AD_WGHT:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		pline("Your load feels heavier!");
		IncreasedGravity += (1 + (dmg * rnd(20)));

		break;

	    case AD_INER:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
	      u_slow_down();
		u.uprops[DEAC_FAST].intrinsic += ((dmg + 2) * 10);
		pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
		u.inertia += (dmg + 2);
		break;

	    case AD_TIME:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) break;
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Zhizn' razgonyal nazad, potomu chto vy ne smotreli, i teper' vy dolzhny poluchit', chto poteryannyy uroven' nazad." : "Kloeck!");

				if (u.urmaxlvlUP >= 2) losexp("time", FALSE, FALSE); /* resistance is futile :D */
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				switch (rnd(A_MAX)) {
					case A_STR:
						pline("You're not as strong as you used to be...");
						ABASE(A_STR) -= 5;
						if(ABASE(A_STR) < ATTRMIN(A_STR)) {dmg *= 3; ABASE(A_STR) = ATTRMIN(A_STR);}
						break;
					case A_DEX:
						pline("You're not as agile as you used to be...");
						ABASE(A_DEX) -= 5;
						if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {dmg *= 3; ABASE(A_DEX) = ATTRMIN(A_DEX);}
						break;
					case A_CON:
						pline("You're not as hardy as you used to be...");
						ABASE(A_CON) -= 5;
						if(ABASE(A_CON) < ATTRMIN(A_CON)) {dmg *= 3; ABASE(A_CON) = ATTRMIN(A_CON);}
						break;
					case A_WIS:
						pline("You're not as wise as you used to be...");
						ABASE(A_WIS) -= 5;
						if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {dmg *= 3; ABASE(A_WIS) = ATTRMIN(A_WIS);}
						break;
					case A_INT:
						pline("You're not as bright as you used to be...");
						ABASE(A_INT) -= 5;
						if(ABASE(A_INT) < ATTRMIN(A_INT)) {dmg *= 3; ABASE(A_INT) = ATTRMIN(A_INT);}
						break;
					case A_CHA:
						pline("You're not as beautiful as you used to be...");
						ABASE(A_CHA) -= 5;
						if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {dmg *= 3; ABASE(A_CHA) = ATTRMIN(A_CHA);}
						break;
				}
				break;
			case 10:
				pline("You're not as powerful as you used to be...");
				ABASE(A_STR)--;
				ABASE(A_DEX)--;
				ABASE(A_CON)--;
				ABASE(A_WIS)--;
				ABASE(A_INT)--;
				ABASE(A_CHA)--;
				if(ABASE(A_STR) < ATTRMIN(A_STR)) {dmg *= 2; ABASE(A_STR) = ATTRMIN(A_STR);}
				if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {dmg *= 2; ABASE(A_DEX) = ATTRMIN(A_DEX);}
				if(ABASE(A_CON) < ATTRMIN(A_CON)) {dmg *= 2; ABASE(A_CON) = ATTRMIN(A_CON);}
				if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {dmg *= 2; ABASE(A_WIS) = ATTRMIN(A_WIS);}
				if(ABASE(A_INT) < ATTRMIN(A_INT)) {dmg *= 2; ABASE(A_INT) = ATTRMIN(A_INT);}
				if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {dmg *= 2; ABASE(A_CHA) = ATTRMIN(A_CHA);}
				break;
		}
		break;

	    case AD_PLAS:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;

			pline("You're seared by %s hot plasma radiation!", StrongFire_resistance ? "" : Fire_resistance ? "very" : "extremely");
			if (!Fire_resistance) dmg *= 2;
			if (StrongFire_resistance && dmg > 1) dmg /= 2;

		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) /* extremely hot - very high chance to burn items! --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
			make_stunned(HStun + dmg, TRUE);

		break;

	    case AD_MANA:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		drain_en(dmg);
		break;

	    case AD_SKIL:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (!rn2(10)) skillcaploss();
		break;

	    case AD_TDRA:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (!rn2(10)) techdrain();
		break;

	    case AD_DROP:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		dropitemattack();
		break;

	    case AD_BLAS:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (!rn2(25)) {
			u.ugangr++;
		      You("get the feeling that %s is angry...", u_gname());
		}
		break;

	    case AD_HALU:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		not_affected |= Blind ||
			(u.umonnum == PM_BLACK_LIGHT ||
			 u.umonnum == PM_VIOLET_FUNGUS ||
			 u.umonnum == PM_VIOLET_STALK ||
			 u.umonnum == PM_VIOLET_SPORE ||
			 u.umonnum == PM_VIOLET_COLONY ||
			 dmgtype(youmonst.data, AD_STUN));
		if (!not_affected && rn2(2) ) {
		    boolean chg;
		    if (!Hallucination)
			You("are caught in a blast of kaleidoscopic light!");
		    chg = make_hallucinated(HHallucination + (long)tmp,FALSE,0L);
		    You("%s", chg ? "inhale a cloud of psychedelic drugs!" : "took another hit of the good stuff!");
		}
		break;
	    case AD_DISN:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		if (!rn2(10))  {
		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu)) ) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable || (Stoned_chiller && Stoned)) {
                pline("You are unharmed!");
                break;
		} else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
		    break;
		} else if (uarmu) {
		    /* destroy shirt */
		    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
		    break;
		}

		if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "monster-induced died", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}
	    return 1; /* lifesaved */

	}
	      break;

	    case AD_VAPO:
		if (!Disint_resistance) dmg *= 3;
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		if (!rn2(10))  {
		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable || (Stoned_chiller && Stoned)) {
                pline("You are unharmed!");
                break;
		} else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
		    break;
		} else if (uarmu) {
		    /* destroy shirt */
		    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
		    break;
		}
		if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "monster-induced vapor died", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}
	    return 1; /* lifesaved */

	}
	      break;

	    case AD_DCAY:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("rot!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			u.mh = 0;
			/* KMH, balance patch -- this is okay with unchanging */
			rehumanize();
			break;
		}
		if (evilfriday || rn2(3)) hurtarmor(AD_DCAY);
		break;
	    case AD_FLAM:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if (mtmp->mcan) break;
		if (evilfriday || rn2(3)) hurtarmor(AD_FLAM);
		break;
	    case AD_HEAL:
		/* a cancelled nurse is just an ordinary monster */
		if (mtmp->mcan) {
		    hitmsg(mtmp, mattk);
		    break;
		}
		if(!uwep
		   && (!uarmu || (uarmu && uarmu->oartifact == ART_GIVE_ME_STROKE__JO_ANNA))
		   && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    boolean goaway = FALSE;
		    pline("%s hits!  (I hope you don't mind.)", Monnam(mtmp));
		    reducesanity(1);
		    if (Upolyd) {
			u.mh += rnd(7);
/* STEPHEN WHITE'S NEW CODE */                                            
			if (!rn2(7)) {
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			    u.cnd_nursehealamount++;
			    if (!rn2(13)) goaway = TRUE;
			}
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			if (!rn2(10)) {
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10)) {
				u.uhpmax++;
				u.cnd_nursehealamount++;
			    }
			    if (!rn2(10)) goaway = TRUE;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    }
		    if (!rn2(3)) exercise(A_STR, TRUE);
		    if (!rn2(3)) exercise(A_CON, TRUE);
		    if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    flags.botl = 1;
		    if (goaway) {
			mongone(mtmp);
			return 2;
		    } else if (!rn2(15)) {
			if (!tele_restrict(mtmp) || !rn2(5) ) (void) rloc(mtmp, FALSE); /* sometimes ignore noteleport --Amy */
			monflee(mtmp, d(3, 6), TRUE, FALSE);
			return 3;
		    }
		    dmg = 0;
		} else {
		    if (Role_if(PM_HEALER) || Race_if(PM_HERBALIST)) {
			if (flags.soundok && !rn2(5))
		      verbalize("Doc, I can't help you unless you cooperate.");
			dmg = 0;
		    } else hitmsg(mtmp, mattk);
		}
		break;
	    case AD_CURS:
	    case AD_LITE:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		/* if(!night() && mdat == &mons[PM_GREMLIN]) break; */

		/* Yeah I know, I just made gremlins and other AD_CURS using monsters a lot more dangerous.
		They're supposed to appear late in the game, adding a bit of risk to high-level-characters.
		I mean come on, early game is hell but late game is cake? Now you can lose your intrinsics at any time!
		If you lose poison resistance, try eating some corpses to get it back.
		If you lose sickness resistance, well, tough luck - it's not coming back. Ever. --Amy*/
		if((!mtmp->mcan && !rn2(10)) || (!mtmp->mcan && night() && !rn2(3)) ) {
		    if (flags.soundok) {
			if (Blind) You_hear("laughter.");
			else       pline("%s chuckles.", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
		    }
		    if (u.umonnum == PM_CLAY_GOLEM) {
			pline("Some writing vanishes from your head!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		    }
		    attrcurse();
		}
		break;

	    case AD_SPC2:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!mtmp->mcan && !rn2(4) && (!Psi_resist || !rn2(StrongPsi_resist ? 100 : 20)) ) {

			pline("Your mind is blasted by psionic energy.");

			switch (rnd(10)) {

				case 1:
				case 2:
				case 3:
					make_confused(HConfusion + dmg, FALSE);
					break;
				case 4:
				case 5:
				case 6:
					make_stunned(HStun + dmg, FALSE);
					break;
				case 7:
					make_confused(HConfusion + dmg, FALSE);
					make_stunned(HStun + dmg, FALSE);
					break;
				case 8:
					make_hallucinated(HHallucination + dmg, FALSE, 0L);
					break;
				case 9:
					make_feared(HFeared + dmg, FALSE);
					break;
				case 10:
					make_numbed(HNumbed + dmg, FALSE);
					break;
	
			}
			if (!rn2(200)) {
				forget(rnd(5));
				pline("You forget some important things...");
			}
			if (!rn2(200) && (u.urmaxlvlUP >= 2)) {
				losexp("psionic drain", FALSE, TRUE);
			}
			if (!rn2(200)) {
				adjattrib(A_INT, -1, 1, TRUE);
				adjattrib(A_WIS, -1, 1, TRUE);
			}
			if (!rn2(200)) {
				pline("You scream in pain!");
				wake_nearby();
			}
			if (!rn2(200)) {
				badeffect();
			}
			if (!rn2(5)) increasesanity(rnz(5));

		}
		break;

	    case AD_STUN:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!mtmp->mcan && !rn2(4)) {
		    make_stunned(HStun + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_NUMB:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!mtmp->mcan && !rn2(4)) {
		    make_numbed(HNumbed + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_FRZE:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!mtmp->mcan && !rn2(10)) {
		    make_frozen(HFrozen + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_BURN:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!mtmp->mcan && !rn2(5)) {
		    make_burned(HBurned + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_DIMN:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && uncancelled) {
		    make_dimmed(HDimmed + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_FEAR:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!mtmp->mcan && !rn2(3)) {
		    make_feared(HFeared + dmg, TRUE);
		    dmg /= 2;
		}
		break;

	    case AD_SANI:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		increasesanity(dmg);

		break;

	    case AD_INSA:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!mtmp->mcan) {
		    make_feared(HFeared + dmg, TRUE);
		    make_stunned(HStun + dmg, TRUE);
		    if(Confusion) You("are getting even more confused.");
		    else You("are getting confused.");
		    make_confused(HConfusion + dmg, FALSE);
		    increasesanity(1);
		}
		break;

	    case AD_CHRN:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;

		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: diseasemu(mdat);
			    break;
		    case 1: make_blinded(Blinded + dmg, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + dmg, TRUE);
			    break;
		    case 3: make_stunned(HStun + dmg, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + dmg, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + dmg, TRUE);
			    break;
		    case 6: make_burned(HBurned + dmg, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + dmg, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + dmg, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + dmg, TRUE);
			    break;
		    }

		break;

	    case AD_ACID:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!mtmp->mcan && !rn2(3)) {
		    if (Acid_resistance && rn2(StrongAcid_resistance ? 20 : 5)) {
			pline("You're covered in acid, but it seems harmless.");
			dmg = 0;
		    } else {
			pline("You're covered in acid! It burns!");
			exercise(A_STR, FALSE);
		    }
			if(!rn2(3)) erode_armor(&youmonst, TRUE);

		if (Stoned) fix_petrification();

		} /*else		dmg = 0;*/
		break;
	    case AD_SLOW:
		hitmsg(mtmp, mattk);
		if (uncancelled && HFast && !defends(AD_SLOW, uwep) && !rn2(4)) {
		    u_slow_down();

			if (Race_if(PM_SPIRIT) && !rn2(3)) {
				u.uprops[DEAC_FAST].intrinsic += ((dmg + 2) * 10);
				pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
				u.inertia += (dmg + 2);
			}
		}
		break;
	    case AD_DREN:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(4))
		    drain_en(dmg);
		dmg = 0;
		break;
	    case AD_CONF:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		if(!mtmp->mcan && !rn2(4) && !mtmp->mspec_used) {
		    mtmp->mspec_used = mtmp->mspec_used + (dmg + rn2(6));
		    if(Confusion)
			 You("are getting even more confused.");
		    else You("are getting confused.");
		    make_confused(HConfusion + dmg, FALSE);
		}
		/*dmg = 0;*/
		break;
	    case AD_DETH:
		pline("%s reaches out with its deadly touch.", Monnam(mtmp));
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vy odin shag blizhe k provalu v nastoyashcheye vremya. Pozdravleniya." : "Doaing!");
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("Was that the touch of death?");
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!PlayerResistsDeathRays) {
			u.youaredead = 1;
			killer_format = KILLED_BY_AN;
			killer = "touch of death";
			done(DIED);
			u.youaredead = 0;
			dmg = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You_feel("your life force draining away...");
		    permdmg = 1;	/* actual damage done below */
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("Lucky for you, it didn't work!");
		    dmg = 0;
		    break;
		}
		break;
	    case AD_PEST:
		pline("%s reaches out, and you feel fever and chills.",
			Monnam(mtmp));
		if (PlayerHearsSoundEffects) pline(issoviet ? "Bolezn' ub'yet vas, potomu chto vash rog ne rabotayet pravil'no, kha-kha. Eto rabota vsekh sovetskikh, potomu chto on nenavidit kazhdogo igroka, i on OSOBENNO nenavidit VAS." : "Aeaeaeaeaeiiiiiiiiiiii!");
		(void) diseasemu(mdat); /* plus the normal damage */
		/* No damage if invulnerable; setting dmg zero prevents
		 * "You are unharmed!" after a sickness inducing attack */
		if (Invulnerable || (Stoned_chiller && Stoned)) dmg = 0;
		break;
	    case AD_FAMN:
		pline("%s reaches out, and your body shrivels.",
			Monnam(mtmp));
		if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy budete prosto golodat'. Nikto ne mozhet pobedit' golod v etom rezhime, potomu chto yest' pishchu prinimayet navsegda, i on sdelayet vas golodnym bystreye, chem vy mozhete s''yest'." : "Klatschklatschklatschklatschklatsch!");
		exercise(A_CON, FALSE);
		if (!is_fainted() && rn2(10) ) morehungry(rnz(40));
		if (!is_fainted() && rn2(10) ) morehungry(rnz(40));
		morehungry(dmg); morehungry(dmg); /* This attack was way too weak. --Amy */
		/* plus the normal damage */
		break;

	    case AD_WRAT:
		pline("%s reaches out, and the life is drawn from your bones.", Monnam(mtmp));

		if(u.uen < 1) {
		    You_feel("less energised!");
		    u.uenmax -= rn1(10,10);
		    if(u.uenmax < 0) u.uenmax = 0;
		} else if(u.uen <= 10) {
		    You_feel("your magical energy dwindle to nothing!");
		    u.uen = 0;
		} else {
		    You_feel("your magical energy dwindling rapidly!");
		    u.uen /= 2;
		}

		break;

	    case AD_LAZY: /* laziness attack; do lots of nasty things at random */
		if(!rn2(2) || statsavingthrow) {
		    pline("%s tries to touch you, but can't really be bothered.",
			Monnam(mtmp));
		    break;
		}
		pline("%s reaches out with an apathetic finger...",
		    Monnam(mtmp));
		switch(rn2(7)) {
		    case 0: /* destroy certain things */
			pline("%s touches you!", Monnam(mtmp));
			witherarmor();
			break;
		    case 1: /* sleep */
			if (multi >= 0) {
			    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) {pline("You yawn."); break;}
			    fall_asleep(-rnd(10), TRUE);
			    if (Blind) You("are put to sleep!");
			    else You("are put to sleep by %s!", mon_nam(mtmp));
			}
			break;
		    case 2: /* paralyse */
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(5), "paralyzed by a monster attack", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 3: /* slow */
			if(HFast)  u_slow_down();
			else You("pause momentarily.");
			break;
		    case 4: /* drain Dex */
			adjattrib(A_DEX, -rn1(1,1), 0, TRUE);
			break;
		    case 5: /* steal teleportitis */
			if(HTeleportation & INTRINSIC) {
			      HTeleportation &= ~INTRINSIC;
			}
	 		if (HTeleportation & TIMEOUT) {
				HTeleportation &= ~TIMEOUT;
			}
			if(HTeleport_control & INTRINSIC) {
			      HTeleport_control &= ~INTRINSIC;
			}
	 		if (HTeleport_control & TIMEOUT) {
				HTeleport_control &= ~TIMEOUT;
			}
		      You("don't feel in the mood for jumping around.");
			break;
		    case 6: /* steal sleep resistance */
			if(HSleep_resistance & INTRINSIC) {
				HSleep_resistance &= ~INTRINSIC;
			} 
			if(HSleep_resistance & TIMEOUT) {
				HSleep_resistance &= ~TIMEOUT;
			} 
			You_feel("like you could use a nap.");
			break;
		}
		break;

	    case AD_CALM:	/* KMH -- koala attack */
		hitmsg(mtmp, mattk);
		if (uncancelled)
		    docalm();
		break;
	    case AD_FREN:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
			pline("You are frenzied!");
			if (u.berserktime) {
			    if (!obsidianprotection()) switch (rn2(11)) {
			    case 0: diseasemu(mdat);
				    break;
			    case 1: make_blinded(Blinded + dmg, TRUE);
				    break;
			    case 2: if (!Confusion)
					You("suddenly feel %s.",
					    FunnyHallu ? "trippy" : "confused");
				    make_confused(HConfusion + dmg, TRUE);
				    break;
			    case 3: make_stunned(HStun + dmg, TRUE);
				    break;
			    case 4: make_numbed(HNumbed + dmg, TRUE);
				    break;
			    case 5: make_frozen(HFrozen + dmg, TRUE);
				    break;
			    case 6: make_burned(HBurned + dmg, TRUE);
				    break;
			    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
				    break;
			    case 8: (void) make_hallucinated(HHallucination + dmg, TRUE, 0L);
				    break;
			    case 9: make_feared(HFeared + dmg, TRUE);
				    break;
			    case 10: make_dimmed(HDimmed + dmg, TRUE);
				    break;
			    }

			} else u.berserktime = dmg;
		}
		break;
	    case AD_POLY:
		hitmsg(mtmp, mattk);
		if (uncancelled && !Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
		    polyself(FALSE);
		}
		break;
	    case AD_CHAO:
		hitmsg(mtmp, mattk);
		if (uncancelled && !Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
		    polyself(FALSE);
		}
		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: diseasemu(mdat);
			    break;
		    case 1: make_blinded(Blinded + dmg, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + dmg, TRUE);
			    break;
		    case 3: make_stunned(HStun + dmg, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + dmg, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + dmg, TRUE);
			    break;
		    case 6: make_burned(HBurned + dmg, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + dmg, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + dmg, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + dmg, TRUE);
			    break;
		    }

		break;
	    case AD_MAGM:
		hitmsg(mtmp, mattk);
		if (statsavingthrow) break;
		    if(Antimagic && !Race_if(PM_KUTAR) && rn2(StrongAntimagic ? 5 : 3)) {
			shieldeff(u.ux, u.uy);
			dmg = 0;
			pline("A hail of magic missiles narrowly misses you!");
		    } else {
			You("are hit by magic missiles appearing from thin air!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "To, chto vy ne magiya ustoychivy yeshche? Togda vasha smert' yavlyayetsya lish' voprosom vremeni. Pochemu by vam ne postavit' nekotoryye usiliya v nego i nachat' igrat' luchshe srazu?" : "Schiaeaeaeaeau!");
	    }
	    break;
		break;
	    case AD_SLIM:    
		hitmsg(mtmp, mattk);
		if (!uncancelled) break;
		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		    dmg = 0;
		} else if (Unchanging || slime_on_touch(youmonst.data)) {
		    You("are unaffected.");
		    dmg = 0;
		} else if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    char kbuf[BUFSZ];
		    sprintf(kbuf, "slimed by %s", mtmp->data->mname);
		    delayed_killer = kbuf;
		} else
		    pline("Yuck!");
		break;
	    case AD_LITT:
		hitmsg(mtmp, mattk);
		if (!uncancelled) break;
		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		    dmg = 0;
		} else if (Unchanging || slime_on_touch(youmonst.data)) {
		    You("are unaffected.");
		    dmg = 0;
		} else if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(20);
		    killer_format = KILLED_BY_AN;
		    char kbuf[BUFSZ];
		    sprintf(kbuf, "slimed by %s", mtmp->data->mname);
		    delayed_killer = kbuf;
		} else
		    pline("Yuck!");

		{
		    register struct obj *littX, *littX2;
		    for (littX = invent; littX; littX = littX2) {
		      littX2 = littX->nobj;
			if (!rn2(StrongAcid_resistance ? 1000 : Acid_resistance ? 100 : 10)) rust_dmg(littX, xname(littX), 3, TRUE, &youmonst);
		    }
		}

		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		hitmsg(mtmp, mattk);
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		if (uncancelled) {
		    struct obj *obj = some_armor(&youmonst);

		    if (obj && drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		    }
		}
		break;
	    case AD_NGEN:
		hitmsg(mtmp, mattk);
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		if (uncancelled) {
		    struct obj *obj = some_armor(&youmonst);

		    if (obj && drain_item_severely(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		    }
		}
		break;
	    default:	dmg = 0;
			break;
	}

	/*if (randattack == 1) {
		mattk->adtyp = AD_RBRE;
		randattack = 0;
	}*/

		/* weapon attacks should be done even if they don't use AD_PHYS --Amy */

		/* first, set up the monster's weapon so we don't get crashes from uninitialized variables... */
		otmp = MON_WEP(mtmp);

		if( (mtmp->egotype_weaponizer || mattk->aatyp == AT_WEAP) && otmp && atttyp > AD_PHYS) {
			int nopoison = (10/* - (otmp->owt/10)*/);
			if (otmp->otyp == CORPSE &&
				touch_petrifies(&mons[otmp->corpsenm])) {
			    dmg += 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[otmp->corpsenm].mname);
			    if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM)) ) goto do_stone;
			}

			/* MRKR: If hit with a burning torch,     */
			/*       then do an extra point of damage */
			/*       but save the message till after  */
			/*       the hitmsg()                     */

			if (otmp->otyp == TORCH && otmp->lamplit && !Fire_resistance) {
			  burnmsg = TRUE;
			  dmg++;
			}

			/* WAC -- Real weapon?
			 * Could be stuck with a cursed bow/polearm it wielded
			 */
			if (/* if you strike with a bow... */
				(is_launcher(otmp) && otmp->otyp != LASERXBOW && otmp->otyp != KLIUSLING) ||
				/* or strike with a missile in your hand... */
				(is_missile(otmp) || is_ammo(otmp)) ||
				/* lightsaber that isn't lit ;) */
				(is_lightsaber(otmp) && !otmp->lamplit) ||
				/* WAC -- or using a pole at short range... */
				(is_pole(otmp))) {
			    /* then do only 1-2 points of damage */
				dmg += rnd(2); /* don't lose the base damage from monst.txt --Amy */

			} else dmg += dmgval(otmp, &youmonst);

			if (objects[otmp->otyp].oc_material == MT_SILVER &&
				hates_silver(youmonst.data) || (uwep && uwep->oartifact == ART_PORKMAN_S_BALLS_OF_STEEL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_PORKMAN_S_BALLS_OF_STEEL) ) {
			    pline("The silver sears your flesh!");
			}
			if (objects[otmp->otyp].oc_material == MT_COPPER &&
				hates_copper(youmonst.data)) {
			    pline("The copper decomposes you!");
			}
			if (objects[otmp->otyp].oc_material == MT_PLATINUM &&
				(hates_platinum(youmonst.data) || u.contamination >= 1000) ) {
			    pline("The platinum smashes you!");
			}
			if (otmp->cursed && (hates_cursed(youmonst.data) || youmonst.data->mlet == S_ANGEL || Race_if(PM_HUMANOID_ANGEL)) ) {
			    pline("An unholy aura blasts you!");
			}
			if (objects[otmp->otyp].oc_material == MT_VIVA && hates_viva(youmonst.data)) {
			    pline("The irradiation severely hurts you!");
			}
			if (objects[otmp->otyp].oc_material == MT_INKA) {
			    pline("The inka string severely hurts you!");
			}
			if (otmp->otyp == ODOR_SHOT) {
			    pline("The odor is terrible!");
			}
			/* Stakes do extra dmg agains vamps */
			if ((otmp->otyp == WOODEN_STAKE || otmp->oartifact == ART_VAMPIRE_KILLER) &&
				is_vampire(youmonst.data)) {
			    if (otmp->oartifact == ART_STAKE_OF_VAN_HELSING) {
				if (!rn2(10)) {
					u.youaredead = 1;
				    pline("%s plunges the stake into your heart.",
					    Monnam(mtmp));
				    killer = "a wooden stake in the heart.";
				    killer_format = KILLED_BY_AN;
				    u.ugrave_arise = NON_PM; /* No corpse */
				    done(DIED);
					u.youaredead = 0;
				} else {
				    pline("%s drives the stake into you.",
					    Monnam(mtmp));
				    dmg += rnd(6) + 2;
				}
			    }else if (otmp->oartifact == ART_VAMPIRE_KILLER) {
				pline("%s whips you good!",
					Monnam(mtmp));
				dmg += rnd(6);
			    } else {
				pline("%s drives the stake into you.",
					Monnam(mtmp));
				dmg += rnd(6);
			    }
			}

			if (otmp->opoisoned) {
			    poisoned(obj_typename(otmp->otyp), A_STR,
				    killer_xname(otmp), 10);
			    if (nopoison < 2) nopoison = 2;
			    if (!rn2(nopoison) && (!otmp->superpoison || !rn2(10)) ) {
				otmp->opoisoned = FALSE;
				otmp->superpoison = FALSE;
				pline("%s %s no longer poisoned.",
				       s_suffix(Monnam(mtmp)),
				       aobjnam(otmp, "are"));
			    }
			}
			if (dmg <= 0) dmg = 1;
			if (!otmp->oartifact || !artifact_hit(mtmp, &youmonst,
				otmp, &dmg, dieroll))
			     hitmsg(mtmp, mattk);

			if (burnmsg) {
			  boolean plural = (Blind ? FALSE : otmp->quan > 1L);
			  boolean water = (youmonst.data ==
					   &mons[PM_WATER_ELEMENTAL]);

			  pline("%s %s%s %syou!",
				(Blind ? "It" : Yname2(otmp)),
				(water ? "vaporize" : "burn"),
				(plural ? "" : "s"),
				(water ? "part of " : ""));

			  if ((isevilvariant || !rn2(2)) && burnarmor(&youmonst)) {
			    dmg++;

			    /* Torch flame is not hot enough to guarantee */
			    /* burning away slime */

			    if (!rn2(4)) burn_away_slime();
			    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
			      (void)destroy_item(POTION_CLASS, AD_FIRE);
			    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
			      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
			    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
			      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
			  }
			  burn_faster(otmp, 1);
			}

			if (u.mh > 1 && u.mh > ((u.uac>0) ? dmg : dmg+u.uac) &&
				   objects[otmp->otyp].oc_material == MT_IRON &&
					(u.umonnum==PM_SHOCK_PUDDING || u.umonnum==PM_VOLT_PUDDING || u.umonnum==PM_BLACK_PUDDING || u.umonnum==PM_DIVISIBLE_ICE_CREAM_BAR || u.umonnum==PM_GEMINICROTTA || u.umonnum==PM_GREY_PUDDING || u.umonnum==PM_STICKY_PUDDING || u.umonnum==PM_DRUDDING || u.umonnum==PM_BLACK_DRUDDING || u.umonnum==PM_BLACKSTEEL_PUDDING || u.umonnum==PM_BLOOD_PUDDING || u.umonnum==PM_MORAL_HAZARD || u.umonnum==PM_MORAL_EVENT_HORIZON
					|| u.umonnum==PM_BROWN_PUDDING || u.umonnum==PM_BLACK_PIERCER)) {
			    /* This redundancy necessary because you have to
			     * take the damage _before_ being cloned.
			     */
			    if (u.uac < 0) dmg += u.uac;
			    if (dmg < 1) dmg = 1;
			    if (dmg > 1) exercise(A_STR, FALSE);
			    u.mh -= dmg;
			    flags.botl = 1;
			    dmg = 0;
			    if (!rn2(50)) { if(cloneu()) /* greatly reduce ability to farm puddings --Amy */
			    You("divide as %s hits you!",mon_nam(mtmp));
				}
			}
		}


	if(u.uhp < 1) done_in_by(mtmp);

/*	Negative armor class reduces damage done instead of fully protecting
 *	against hits.
 */
	if (dmg && u.uac < /*-1*/ (issoviet ? -20 : 0) ) {  /* damage reduction will start at -1 rather than -11 AC now --Amy */
		int tempval;

		int effectiveac = (-(u.uac));
		if (issoviet) effectiveac -= 20;
		if (effectiveac > (issoviet ? 100 : 120)) {
			if (issoviet) effectiveac -= rn3(effectiveac - 99);
			else effectiveac -= rn3(effectiveac - 119);
		}
		if (effectiveac > (issoviet ? 60 : 80)) {
			if (issoviet) effectiveac -= rn3(effectiveac - 59);
			else effectiveac -= rn3(effectiveac - 79);
		}
		if (effectiveac > (issoviet ? 20 : 40)) {
			if (issoviet) effectiveac -= rn2(effectiveac - 19);
			else effectiveac -= rn2(effectiveac - 39);
		}

		tempval = rnd((effectiveac / (issoviet ? 5 : 4)) + 1);
		if (tempval < 1)  tempval = 1;
		if (tempval > (issoviet ? 20 : 50)) tempval = (issoviet ? 20 : 50); /* max limit increased --Amy */

		if (uarmf && itemhasappearance(uarmf, APP_HEROINE_MOCASSINS)) tempval *= 2;

		if (uarmf && uarmf->oartifact == ART_INDIAN_SMOKE_SYMBOL) tempval *= 2;
		if (Conflict) tempval /= 2; /* conflict is so powerful that it requires a bunch of nerfs --Amy */
		if (Race_if(PM_SPARD)) tempval /= 2;
		if (StrongConflict) tempval /= 2; /* conflict is so powerful that it requires a bunch of nerfs --Amy */

		/* Amy edit: high AC is just far too strong, especially against already weak monsters! */
		if (issoviet) {
			dmg -= tempval;
			if (dmg < 1) dmg = 1;
		}

		/* so outside of soviet russia, have it give percentage-based damage reduction instead */
		if (dmg > 1 && tempval > 0) {
			dmg *= (100 - rnd(tempval));
			dmg++;
			dmg /= 100;
			if (dmg < 1) dmg = 1;
		}
	}

	if(dmg) {
	    if ( (Half_physical_damage && rn2(2)) 
					/* Mitre of Holiness */
		|| (Role_if(PM_PRIEST) && uarmh && is_quest_artifact(uarmh) &&
		    (is_undead(mtmp->data) || is_demon(mtmp->data))))
		dmg = (dmg+1) / 2;

		if (StrongHalf_physical_damage && rn2(2)) dmg = (dmg+1) / 2;

	    if (permdmg) {	/* Death's life force drain */
		int lowerlimit, *hpmax_p;
		/*
		 * Apply some of the damage to permanent hit points:
		 *	polymorphed	    100% against poly'd hpmax
		 *	hpmax > 25*lvl	    100% against normal hpmax
		 *	hpmax > 10*lvl	50..100%
		 *	hpmax >  5*lvl	25..75%
		 *	otherwise	 0..50%
		 * Never reduces hpmax below 1 hit point per level.
		 */
		permdmg = rn2(dmg / 2 + 1);
		if (Upolyd || u.uhpmax > 25 * u.ulevel) permdmg = dmg;
		else if (u.uhpmax > 10 * u.ulevel) permdmg += dmg / 2;
		else if (u.uhpmax > 5 * u.ulevel) permdmg += dmg / 4;

		if (Upolyd) {
		    hpmax_p = &u.mhmax;
		    /* [can't use youmonst.m_lev] */
		    lowerlimit = min((int)youmonst.data->mlevel, u.ulevel);
		} else {
		    hpmax_p = &u.uhpmax;
		    lowerlimit = u.ulevel;
		}
		if (*hpmax_p - permdmg > lowerlimit)
		    *hpmax_p -= permdmg;
		else if (*hpmax_p > lowerlimit)
		    *hpmax_p = lowerlimit;
		else	/* unlikely... */
		    ;	/* already at or below minimum threshold; do nothing */
		flags.botl = 1;

		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
	    }

	    mdamageu(mtmp, dmg);
	}

	if (DEADMONSTER(mtmp))
	    res = 2;
	else if (dmg)
	    res = passiveum(olduasmon, mtmp, mattk);
	else
	    res = 1;
	stop_occupation();
	return res;
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL int
gulpmu(mtmp, mattk)	/* monster swallows you, or damage if u.uswallow */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	char	 buf[BUFSZ];
	struct trap *t = t_at(u.ux, u.uy);
	int	tmp = d((int)mattk->damn, (int)mattk->damd);
	if (MaximumDamageBug || u.uprops[MAXIMUM_DAMAGE_BUG].extrinsic || have_maximumdamagestone() || (uwep && uwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI)) tmp = (int)mattk->damn * (int)mattk->damd;
	int	tim_tmp;
	register struct obj *otmp2;
	int	i;
	/*int randattackA = 0;*/
	int atttypA;
	int hallutime;
	struct obj *otmpi, *otmpii;
	struct obj *optr;

	if (!u.uswallow) {	/* swallows you */
		if (youmonst.data->msize >= MZ_HUGE && mtmp->data->msize < MZ_HUGE) return(0);
		if (uwep && uwep->oartifact == ART_PEEPLUE) return(0);
		if ((t && ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT) || (t->ttyp == GIANT_CHASM) || (t->ttyp == SHIT_PIT) || (t->ttyp == MANA_PIT) || (t->ttyp == ANOXIC_PIT) || (t->ttyp == ACID_PIT) )) &&
		    sobj_at(BOULDER, u.ux, u.uy))
			return(0);

		if (Punished) unplacebc();	/* ball&chain would go away but it's crashy --Amy */
		remove_monster(mtmp->mx, mtmp->my);
		mtmp->mtrapped = 0;		/* no longer on old trap */
		place_monster(mtmp, u.ux, u.uy);
		newsym(mtmp->mx,mtmp->my);
		if (is_animal(mtmp->data) && u.usteed && !mayfalloffsteed()) {
			char buf[BUFSZ];
			/* Too many quirks presently if hero and steed
			 * are swallowed. Pretend purple worms don't
			 * like horses for now :-)
			 */
			strcpy(buf, mon_nam(u.usteed));
			pline ("%s lunges forward and plucks you off %s!",
				Monnam(mtmp), buf);
			dismount_steed(DISMOUNT_ENGULFED);
		} else
		pline("%s engulfs you!", Monnam(mtmp));
		stop_occupation();
		reset_occupations();	/* behave as if you had moved */

		if (u.utrap) {
			You("are released from the %s!",
				u.utraptype==TT_WEB ? "web" : "trap");
			u.utrap = 0;
		}

		i = number_leashed();
		if (i > 0) {
		    const char *s = (i > 1) ? "leashes" : "leash";
		    pline_The("%s %s loose.", s, vtense(s, "snap"));
		    unleash_all();
		}

		if (touch_petrifies(youmonst.data) && !resists_ston(mtmp) && !rn2(4)) {
			minstapetrify(mtmp, TRUE);
			if (mtmp->mhp > 0) return 0;
			else return 2;
		}

		display_nhwindow(WIN_MESSAGE, FALSE);
		vision_recalc(2);	/* hero can't see anything */
		u.uswallow = 1;
		setustuck(mtmp);
		/* u.uswldtim always set > 1 */
		tim_tmp = 25 - (int)mtmp->m_lev;
		if (tim_tmp > 0) tim_tmp = rnd(tim_tmp) / 2;
		else if (tim_tmp < 0) tim_tmp = -(rnd(-tim_tmp) / 2);
		tim_tmp += -u.uac + 10;
		u.uswldtim = (unsigned)((tim_tmp < 2) ? 2 : tim_tmp);
		swallowed(1);
		for (otmp2 = invent; otmp2; otmp2 = otmp2->nobj)
		    (void) snuff_lit(otmp2);
	}

	if (mtmp != u.ustuck) return(0);
	if (u.uswldtim > 0) u.uswldtim -= 1;

	/* Monsters with AD_RBRE have random engulfing attacks. --Amy */

	atttypA = mattk->adtyp;

	if ((SecretAttackBug || u.uprops[SECRET_ATTACK_BUG].extrinsic || have_secretattackstone()) && atttypA == AD_PHYS && !rn2(100)) {
		while (atttypA == AD_ENDS || atttypA == AD_RBRE || atttypA == AD_WERE || atttypA == AD_PHYS) {
			atttypA = randattack(); }

	}

	if ((UnfairAttackBug || u.uprops[UNFAIR_ATTACK_BUG].extrinsic || have_unfairattackstone()) && atttypA == AD_PHYS && !rn2(100)) {
		while (atttypA == AD_ENDS || atttypA == AD_RBRE || atttypA == AD_WERE || atttypA == AD_PHYS) {
			atttypA = rn2(AD_ENDS); }

	}

	if (atttypA == AD_RBRE) {
		while (atttypA == AD_ENDS || atttypA == AD_RBRE || atttypA == AD_WERE) {
			atttypA = randattack(); }
		/*randattack = 1;*/
	}

	if (atttypA == AD_DAMA) {
		atttypA = randomdamageattack();
	}

	if (atttypA == AD_ILLU) {
		atttypA = randomillusionattack();
	}

	if (atttypA == AD_THIE) {
		atttypA = randomthievingattack();
	}

	if (atttypA == AD_RNG) {
		while (atttypA == AD_ENDS || atttypA == AD_RNG || atttypA == AD_WERE) {
			atttypA = rn2(AD_ENDS); }
	}

	if (atttypA == AD_PART) atttypA = u.adpartattack;

	if (atttypA == AD_MIDI) {
		atttypA = mtmp->m_id;
		if (atttypA < 0) atttypA *= -1;
		while (atttypA >= AD_ENDS) atttypA -= AD_ENDS;
		if (!(atttypA >= AD_PHYS && atttypA < AD_ENDS)) atttypA = AD_PHYS; /* fail safe --Amy */
		if (atttypA == AD_WERE) atttypA = AD_PHYS;
	}

	switch(atttypA) {

		case AD_DGST:
		    if (Slow_digestion && (StrongSlow_digestion ? rn2(5) : !rn2(5)) ) {
			/* Messages are handled below */
			u.uswldtim = 0;
			tmp = 0;
		    } else if (u.uswldtim == 0) {
			pline("%s totally digests you!", Monnam(mtmp));
			tmp = u.uhp;
			if (Half_physical_damage) tmp *= 2; /* sorry */
			if (StrongHalf_physical_damage) tmp *= 2; /* sorry */
		    } else {
			pline("%s%s digests you!", Monnam(mtmp),
			      (u.uswldtim == 3) ? " strongly" :
			      (u.uswldtim == 2) ? " thoroughly" :
			      (u.uswldtim == 1) ? " utterly" : "");
			exercise(A_STR, FALSE);
		    }
		    break;

	    case AD_HEAL:
		if (mtmp->mcan) {
		    break;
		}
		if(!uwep && (!uarmu || (uarmu && uarmu->oartifact == ART_GIVE_ME_STROKE__JO_ANNA)) && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    boolean goaway = FALSE;
		    pline("It hits!  (I hope you don't mind.)");
		    reducesanity(1);
		    if (Upolyd) {
			u.mh += rnd(7);
			if (!rn2(7)) {
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			    u.cnd_nursehealamount++;
			    if (!rn2(13)) goaway = TRUE;
			}
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			if (!rn2(10)) {
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10)) {
				u.uhpmax++;
				u.cnd_nursehealamount++;
			    }
			    if (!rn2(10)) goaway = TRUE;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    }
		    if (!rn2(3)) exercise(A_STR, TRUE);
		    if (!rn2(3)) exercise(A_CON, TRUE);
		    if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    flags.botl = 1;
		    if (goaway) {
			mongone(mtmp);
			return 2;
		    } else if (!rn2(15)) {
			if (!tele_restrict(mtmp) || !rn2(5) ) (void) rloc(mtmp, FALSE); /* sometimes ignore noteleport --Amy */
			monflee(mtmp, d(3, 6), TRUE, FALSE);
			return 3;
		    }
		    tmp = 0;
		} else {
		    if (Role_if(PM_HEALER) || Race_if(PM_HERBALIST)) {
			tmp = 0;
		    }
		}
		break;

	      case AD_CURS:
	    case AD_LITE:
			pline("It curses you!");

			if(!rn2(10) || (night() && !rn2(3)) )  {
			    if (u.umonnum == PM_CLAY_GOLEM) {
				pline("Some writing vanishes from your head!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0;
				rehumanize();
				break;
			    }
				if (flags.soundok) {
					You_hear("a chuckling laughter.");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
				}
			    attrcurse();
			}
			break;

	      case AD_SPC2:
			if (Psi_resist && rn2(StrongPsi_resist ? 100 : 20) ) break;

			You_feel("something focusing on your mind!");

			switch (rnd(10)) {

				case 1:
				case 2:
				case 3:
					make_confused(HConfusion + tmp, FALSE);
					break;
				case 4:
				case 5:
				case 6:
					make_stunned(HStun + tmp, FALSE);
					break;
				case 7:
					make_confused(HConfusion + tmp, FALSE);
					make_stunned(HStun + tmp, FALSE);
					break;
				case 8:
					make_hallucinated(HHallucination + tmp, FALSE, 0L);
					break;
				case 9:
					make_feared(HFeared + tmp, FALSE);
					break;
				case 10:
					make_numbed(HNumbed + tmp, FALSE);
					break;
	
			}
			if (!rn2(200)) {
				forget(rnd(5));
				pline("You forget some important things...");
			}
			if (!rn2(200) && (u.urmaxlvlUP >= 2)) {
				losexp("psionic drain", FALSE, TRUE);
			}
			if (!rn2(200)) {
				adjattrib(A_INT, -1, 1, TRUE);
				adjattrib(A_WIS, -1, 1, TRUE);
			}
			if (!rn2(200)) {
				pline("You scream in pain!");
				wake_nearby();
			}
			if (!rn2(200)) {
				badeffect();
			}
			if (!rn2(5)) increasesanity(rnz(5));

			break;

		case AD_SLEE:
			pline("It slaps you!");
			if (!rn2(5) && multi >= 0) {
			    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) break;
			    fall_asleep(-rnd(10), TRUE);
			    You("suddenly fall asleep!");
			}
			break;
		case AD_DRST:
			You_feel("your strength drain away!");
			if (!rn2(3)) {
			    poisoned("The attack", A_STR, "strength drain", 30);
			}
			break;
		case AD_DRDX:
			You_feel("your muscles cramping!");
			if (!rn2(3)) {
			    poisoned("The attack", A_DEX, "dexterity drain", 30);
			}
			break;
		case AD_DRCO:
			You_feel("a lack of force!");
			if (!rn2(3)) {
			    poisoned("The attack", A_CON, "constitution drain", 30);
			}
			break;
		case AD_WISD:
			You_feel("naive!");
			if (!rn2(3)) {
			    poisoned("The attack", A_WIS, "wisdom drain", 30);
			}
			break;
		case AD_DRCH:
			You_feel("ugly!");
			if (!rn2(3)) {
			    poisoned("The attack", A_CHA, "charisma drain", 30);
			}
			break;

	      case AD_DREA:
			pline("You have a nightmare!");
	
			if (multi < 0) {
				tmp *= 4;
				pline("Your dream is eaten!");
			}

			break;

	      case AD_BADE:
			pline("Bad things are happening to you!");

			badeffect();

			break;

	      case AD_RBAD:
			pline("Oh no, something really bad happens!");

			reallybadeffect();

			break;

		case AD_BLEE:

			pline("It rips into your body!");
			if (tmp > 0) playerbleed(tmp);

			break;

		case AD_SHAN:

			You("have trouble keeping your clothes on!");
			shank_player();

		break;

		case AD_DEBU:

			You("seem to lose stats...");
			statdebuff();

		break;

		case AD_UNPR:

		{
			struct obj *obj = some_armor(&youmonst);
			if (obj && obj->oerodeproof) {
				obj->oerodeproof = FALSE;
				Your("%s brown for a moment.", aobjnam(obj, "glow"));
			}
		}

		break;

		case AD_NIVE:

			You("seem to be brought down to a lower level...");
			nivellate();
		break;

		case AD_SCOR:

			u.urexp -= (tmp * 50);
			if (u.urexp < 0) u.urexp = 0;
			Your("score is drained!");

		break;

		case AD_TERR:
			You("are whirled together!");
			terrainterror();
		break;

		case AD_FEMI:

			pline("It feels very feminine in here!");
			randomfeminismtrap(rnz( (tmp + 2) * rnd(100)));
			if (Role_if(PM_CLIMACTERIAL)) climtrainsqueaking(tmp);

		break;

		case AD_LEVI:
			You("float up!");
			HLevitation &= ~I_SPECIAL;
			incr_itimeout(&HLevitation, tmp);

		break;

		case AD_MCRE:
			MCReduction += (100 * tmp);
			pline("The magic cancellation granted by your armor seems weaker now...");

		break;

	      case AD_FUMB:
			pline("Mary-Sue alert!");

			HFumbling = FROMOUTSIDE | rnd(5);
			incr_itimeout(&HFumbling, rnd(20));
			u.fumbleduration += rnz(10 * (tmp + 1) );

			break;

	      case AD_TREM:
			Your("%s are trembling uncontrollably!", makeplural(body_part(HAND)));

			u.tremblingamount++;

			break;

	    case AD_IDAM:

		pline("This environment isn't healthy for your gear...");

		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(20)) rust_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}

		break;

	    case AD_ANTI:

		pline("Your equipment is falling apart!");

		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(20)) wither_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}

		break;

	    case AD_PAIN:

		pline("Pain shoots into your body!");
		losehp(Upolyd ? ((u.mh / 10) + 1) : ((u.uhp / 10) + 1), "painful engulfing", KILLED_BY);

		break;

	    case AD_TECH:

		pline("An EMP wave hits you!");
		techcapincrease(tmp * rnd(50));

		break;

	    case AD_MEMO:

		You_feel("forgetful!");
		spellmemoryloss(tmp);

		break;

	    case AD_TRAI:

		You_feel("very exhausted!");
		skilltrainingdecrease(tmp);
		if (!rn2(100)) skillcaploss();

		break;

	    case AD_STAT:

		pline("The very essence of you seems to be sapped!");
		if (!rn2(3)) {
			statdrain();
		}

		break;

	      case AD_VULN:
			pline("You are covered with aggressive bacteria!");

			deacrandomintrinsic(rnz( (tmp * rnd(30) ) + 1));

			break;

		case AD_NACU:

			pline("A hideous-sounding curse resonates all around you...");
			if (!rn2(7)) nastytrapcurse();

		break;

	      case AD_ICUR:
				pline("You are sapped by dark magic!");

			if (!rn2(5)) {
				You_feel("as if you need some help.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
				rndcurse();
			}

		break;

	      case AD_SLUD:
				pline("You are completely immersed in aggressive sludge!");

			{
			    register struct obj *objX, *objX2;
			    for (objX = invent; objX; objX = objX2) {
			      objX2 = objX->nobj;
				if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
			    }
			}

		break;

	    case AD_NAST:
				pline("A big fat gob of nastiness splashes over you!");

		if (!rn2(10)) {
			pline("It latches on to your body!");

			randomnastytrapeffect(rnz( (tmp + 2) * rnd(100) ), 1000 - (tmp * 3));

		}

		break;

	      case AD_VENO:
				pline("You are covered with toxic venom!");

			if (chromeprotection()) break;

			if (!Poison_resistance) pline("You're badly poisoned!");
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE, TRUE);

			if (!rn2(2)) {
			    poisoned("The attack", rn2(A_MAX), "extremely poisonous interior", 30);
			}

			if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(POTION_CLASS, AD_VENO);
			if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(FOOD_CLASS, AD_VENO);

			break;

		case AD_POIS:
				You("are covered with poison!");
			if (!rn2(3)) {
			    poisoned("The attack", rn2(A_MAX), "poisonous interior", 30);
			}
			break;

		case AD_NPRO:
				You_feel("unsafe in here...");
			if (!rn2(10)) {
				u.negativeprotection++;
				if (evilfriday && u.ublessed > 0) {
					u.ublessed -= 1;
					if (u.ublessed < 0) u.ublessed = 0;
				}
				You_feel("less protected!");
			}
			break;

	      case AD_THIR:
			pline("It sucks your %s!", body_part(BLOOD) );
			if (PlayerHearsSoundEffects) pline(issoviet ? "A u vas yest' dostatochno sil'nyye oruzhiye dlya preodoleniya zazhivleniyu monstra?" : "Fffffffff!");
			mtmp->mhp += tmp;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

			break;

	      case AD_NTHR:
			pline("It sucks your %s!", body_part(BLOOD) );
			if (PlayerHearsSoundEffects) pline(issoviet ? "A u vas yest' dostatochno sil'nyye oruzhiye dlya preodoleniya zazhivleniyu monstra?" : "Fffffffff!");
			mtmp->mhp += tmp;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

			if ((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && u.uexp > 100) {
				u.uexp -= (u.uexp / 100);
				You_feel("your life slipping away!");
				if (u.uexp < newuexp(u.ulevel - 1)) {
				      losexp("nether forces", TRUE, FALSE);
				}
			}
			break;

		case AD_AGGR:

			incr_itimeout(&HAggravate_monster, tmp);
			You_feel("that monsters are aware of your presence.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
			aggravate();
			if (!rn2(20)) {

				int aggroamount = rnd(6);
				if (isfriday) aggroamount *= 2;
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
				while (aggroamount) {

					u.cnd_aggravateamount++;
					makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
					aggroamount--;
					if (aggroamount < 0) aggroamount = 0;
				}
				u.aggravation = 0;
				pline("Several monsters come out of a portal.");
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

			}

			break;

		case AD_DATA:

			u.datadeletedefer = 1;
			datadeleteattack();
			break;

		case AD_RAGN:

			ragnarok(FALSE);
			if (evilfriday && mtmp->m_lev > 1) evilragnarok(FALSE,mtmp->m_lev);
			break;

		case AD_DEST:

			destroyarmorattack();
			break;

		case AD_MINA:

		{
			register int midentity = mtmp->m_id;
			if (midentity < 0) midentity *= -1;
			while (midentity > 235) midentity -= 235;

			register int nastyduration = ((tmp + 2) * rnd(10));
			if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) nastyduration *= 20;

			switch (midentity) {

				case 1: RMBLoss += rnz(nastyduration); break;
				case 2: NoDropProblem += rnz(nastyduration); break;
				case 3: DSTWProblem += rnz(nastyduration); break;
				case 4: StatusTrapProblem += rnz(nastyduration); 
					if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
					if (HStun) set_itimeout(&HeavyStunned, HStun);
					if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
					if (HFeared) set_itimeout(&HeavyFeared, HFeared);
					if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
					if (HBurned) set_itimeout(&HeavyBurned, HBurned);
					if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
					if (Blinded) set_itimeout(&HeavyBlind, Blinded);
					if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
					break;
				case 5: Superscroller += rnz(nastyduration * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz(nastyduration); break;
				case 7: FreeHandLoss += rnz(nastyduration); break;
				case 8: Unidentify += rnz(nastyduration); break;
				case 9: Thirst += rnz(nastyduration); break;
				case 10: LuckLoss += rnz(nastyduration); break;
				case 11: ShadesOfGrey += rnz(nastyduration); break;
				case 12: FaintActive += rnz(nastyduration); break;
				case 13: Itemcursing += rnz(nastyduration); break;
				case 14: DifficultyIncreased += rnz(nastyduration); break;
				case 15: Deafness += rnz(nastyduration); flags.soundok = 0; break;
				case 16: CasterProblem += rnz(nastyduration); break;
				case 17: WeaknessProblem += rnz(nastyduration); break;
				case 18: RotThirteen += rnz(nastyduration); break;
				case 19: BishopGridbug += rnz(nastyduration); break;
				case 20: UninformationProblem += rnz(nastyduration); break;
				case 21: StairsProblem += rnz(nastyduration); break;
				case 22: AlignmentProblem += rnz(nastyduration); break;
				case 23: ConfusionProblem += rnz(nastyduration); break;
				case 24: SpeedBug += rnz(nastyduration); break;
				case 25: DisplayLoss += rnz(nastyduration); break;
				case 26: SpellLoss += rnz(nastyduration); break;
				case 27: YellowSpells += rnz(nastyduration); break;
				case 28: AutoDestruct += rnz(nastyduration); break;
				case 29: MemoryLoss += rnz(nastyduration); break;
				case 30: InventoryLoss += rnz(nastyduration); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (tmp * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz(nastyduration); break;
				case 33: BloodLossProblem += rnz(nastyduration); break;
				case 34: BadEffectProblem += rnz(nastyduration); break;
				case 35: TrapCreationProblem += rnz(nastyduration); break;
				case 36: AutomaticVulnerabilitiy += rnz(nastyduration); break;
				case 37: TeleportingItems += rnz(nastyduration); break;
				case 38: NastinessProblem += rnz(nastyduration); break;
				case 39: CaptchaProblem += rnz(nastyduration); break;
				case 40: FarlookProblem += rnz(nastyduration); break;
				case 41: RespawnProblem += rnz(nastyduration); break;
				case 42: RecurringAmnesia += rnz(nastyduration); break;
				case 43: BigscriptEffect += rnz(nastyduration); break;
				case 44: {
					BankTrapEffect += rnz(nastyduration);
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz(nastyduration); break;
				case 46: TechTrapEffect += rnz(nastyduration); break;
				case 47: RecurringDisenchant += rnz(nastyduration); break;
				case 48: verisiertEffect += rnz(nastyduration); break;
				case 49: ChaosTerrain += rnz(nastyduration); break;
				case 50: Muteness += rnz(nastyduration); break;
				case 51: EngravingDoesntWork += rnz(nastyduration); break;
				case 52: MagicDeviceEffect += rnz(nastyduration); break;
				case 53: BookTrapEffect += rnz(nastyduration); break;
				case 54: LevelTrapEffect += rnz(nastyduration); break;
				case 55: QuizTrapEffect += rnz(nastyduration); break;
				case 56: FastMetabolismEffect += rnz(nastyduration); break;
				case 57: NoReturnEffect += rnz(nastyduration); break;
				case 58: AlwaysEgotypeMonsters += rnz(nastyduration); break;
				case 59: TimeGoesByFaster += rnz(nastyduration); break;
				case 60: FoodIsAlwaysRotten += rnz(nastyduration); break;
				case 61: AllSkillsUnskilled += rnz(nastyduration); break;
				case 62: AllStatsAreLower += rnz(nastyduration); break;
				case 63: PlayerCannotTrainSkills += rnz(nastyduration); break;
				case 64: PlayerCannotExerciseStats += rnz(nastyduration); break;
				case 65: TurnLimitation += rnz(nastyduration); break;
				case 66: WeakSight += rnz(nastyduration); break;
				case 67: RandomMessages += rnz(nastyduration); break;

				case 68: Desecration += rnz(nastyduration); break;
				case 69: StarvationEffect += rnz(nastyduration); break;
				case 70: NoDropsEffect += rnz(nastyduration); break;
				case 71: LowEffects += rnz(nastyduration); break;
				case 72: InvisibleTrapsEffect += rnz(nastyduration); break;
				case 73: GhostWorld += rnz(nastyduration); break;
				case 74: Dehydration += rnz(nastyduration); break;
				case 75: HateTrapEffect += rnz(nastyduration); break;
				case 76: TotterTrapEffect += rnz(nastyduration); break;
				case 77: Nonintrinsics += rnz(nastyduration); break;
				case 78: Dropcurses += rnz(nastyduration); break;
				case 79: Nakedness += rnz(nastyduration); break;
				case 80: Antileveling += rnz(nastyduration); break;
				case 81: ItemStealingEffect += rnz(nastyduration); break;
				case 82: Rebellions += rnz(nastyduration); break;
				case 83: CrapEffect += rnz(nastyduration); break;
				case 84: ProjectilesMisfire += rnz(nastyduration); break;
				case 85: WallTrapping += rnz(nastyduration); break;
				case 86: DisconnectedStairs += rnz(nastyduration); break;
				case 87: InterfaceScrewed += rnz(nastyduration); break;
				case 88: Bossfights += rnz(nastyduration); break;
				case 89: EntireLevelMode += rnz(nastyduration); break;
				case 90: BonesLevelChange += rnz(nastyduration); break;
				case 91: AutocursingEquipment += rnz(nastyduration); break;
				case 92: HighlevelStatus += rnz(nastyduration); break;
				case 93: SpellForgetting += rnz(nastyduration); break;
				case 94: SoundEffectBug += rnz(nastyduration); break;
				case 95: TimerunBug += rnz(nastyduration); break;
				case 96: LootcutBug += rnz(nastyduration); break;
				case 97: MonsterSpeedBug += rnz(nastyduration); break;
				case 98: ScalingBug += rnz(nastyduration); break;
				case 99: EnmityBug += rnz(nastyduration); break;
				case 100: WhiteSpells += rnz(nastyduration); break;
				case 101: CompleteGraySpells += rnz(nastyduration); break;
				case 102: QuasarVision += rnz(nastyduration); break;
				case 103: MommaBugEffect += rnz(nastyduration); break;
				case 104: HorrorBugEffect += rnz(nastyduration); break;
				case 105: ArtificerBug += rnz(nastyduration); break;
				case 106: WereformBug += rnz(nastyduration); break;
				case 107: NonprayerBug += rnz(nastyduration); break;
				case 108: EvilPatchEffect += rnz(nastyduration); break;
				case 109: HardModeEffect += rnz(nastyduration); break;
				case 110: SecretAttackBug += rnz(nastyduration); break;
				case 111: EaterBugEffect += rnz(nastyduration); break;
				case 112: CovetousnessBug += rnz(nastyduration); break;
				case 113: NotSeenBug += rnz(nastyduration); break;
				case 114: DarkModeBug += rnz(nastyduration); break;
				case 115: AntisearchEffect += rnz(nastyduration); break;
				case 116: HomicideEffect += rnz(nastyduration); break;
				case 117: NastynationBug += rnz(nastyduration); break;
				case 118: WakeupCallBug += rnz(nastyduration); break;
				case 119: GrayoutBug += rnz(nastyduration); break;
				case 120: GrayCenterBug += rnz(nastyduration); break;
				case 121: CheckerboardBug += rnz(nastyduration); break;
				case 122: ClockwiseSpinBug += rnz(nastyduration); break;
				case 123: CounterclockwiseSpin += rnz(nastyduration); break;
				case 124: LagBugEffect += rnz(nastyduration); break;
				case 125: BlesscurseEffect += rnz(nastyduration); break;
				case 126: DeLightBug += rnz(nastyduration); break;
				case 127: DischargeBug += rnz(nastyduration); break;
				case 128: TrashingBugEffect += rnz(nastyduration); break;
				case 129: FilteringBug += rnz(nastyduration); break;
				case 130: DeformattingBug += rnz(nastyduration); break;
				case 131: FlickerStripBug += rnz(nastyduration); break;
				case 132: UndressingEffect += rnz(nastyduration); break;
				case 133: Hyperbluewalls += rnz(nastyduration); break;
				case 134: NoliteBug += rnz(nastyduration); break;
				case 135: ParanoiaBugEffect += rnz(nastyduration); break;
				case 136: FleecescriptBug += rnz(nastyduration); break;
				case 137: InterruptEffect += rnz(nastyduration); break;
				case 138: DustbinBug += rnz(nastyduration); break;
				case 139: ManaBatteryBug += rnz(nastyduration); break;
				case 140: Monsterfingers += rnz(nastyduration); break;
				case 141: MiscastBug += rnz(nastyduration); break;
				case 142: MessageSuppression += rnz(nastyduration); break;
				case 143: StuckAnnouncement += rnz(nastyduration); break;
				case 144: BloodthirstyEffect += rnz(nastyduration); break;
				case 145: MaximumDamageBug += rnz(nastyduration); break;
				case 146: LatencyBugEffect += rnz(nastyduration); break;
				case 147: StarlitBug += rnz(nastyduration); break;
				case 148: KnowledgeBug += rnz(nastyduration); break;
				case 149: HighscoreBug += rnz(nastyduration); break;
				case 150: PinkSpells += rnz(nastyduration); break;
				case 151: GreenSpells += rnz(nastyduration); break;
				case 152: EvencoreEffect += rnz(nastyduration); break;
				case 153: UnderlayerBug += rnz(nastyduration); break;
				case 154: DamageMeterBug += rnz(nastyduration); break;
				case 155: ArbitraryWeightBug += rnz(nastyduration); break;
				case 156: FuckedInfoBug += rnz(nastyduration); break;
				case 157: BlackSpells += rnz(nastyduration); break;
				case 158: CyanSpells += rnz(nastyduration); break;
				case 159: HeapEffectBug += rnz(nastyduration); break;
				case 160: BlueSpells += rnz(nastyduration); break;
				case 161: TronEffect += rnz(nastyduration); break;
				case 162: RedSpells += rnz(nastyduration); break;
				case 163: TooHeavyEffect += rnz(nastyduration); break;
				case 164: ElongationBug += rnz(nastyduration); break;
				case 165: WrapoverEffect += rnz(nastyduration); break;
				case 166: DestructionEffect += rnz(nastyduration); break;
				case 167: MeleePrefixBug += rnz(nastyduration); break;
				case 168: AutomoreBug += rnz(nastyduration); break;
				case 169: UnfairAttackBug += rnz(nastyduration); break;
				case 170: OrangeSpells += rnz(nastyduration); break;
				case 171: VioletSpells += rnz(nastyduration); break;
				case 172: LongingEffect += rnz(nastyduration); break;
				case 173: CursedParts += rnz(nastyduration); break;
				case 174: Quaversal += rnz(nastyduration); break;
				case 175: AppearanceShuffling += rnz(nastyduration); break;
				case 176: BrownSpells += rnz(nastyduration); break;
				case 177: Choicelessness += rnz(nastyduration); break;
				case 178: Goldspells += rnz(nastyduration); break;
				case 179: Deprovement += rnz(nastyduration); break;
				case 180: InitializationFail += rnz(nastyduration); break;
				case 181: GushlushEffect += rnz(nastyduration); break;
				case 182: SoiltypeEffect += rnz(nastyduration); break;
				case 183: DangerousTerrains += rnz(nastyduration); break;
				case 184: FalloutEffect += rnz(nastyduration); break;
				case 185: MojibakeEffect += rnz(nastyduration); break;
				case 186: GravationEffect += rnz(nastyduration); break;
				case 187: UncalledEffect += rnz(nastyduration); break;
				case 188: ExplodingDiceEffect += rnz(nastyduration); break;
				case 189: PermacurseEffect += rnz(nastyduration); break;
				case 190: ShroudedIdentity += rnz(nastyduration); break;
				case 191: FeelerGauges += rnz(nastyduration); break;
				case 192: LongScrewup += rnz(nastyduration * 20); break;
				case 193: WingYellowChange += rnz(nastyduration); break;
				case 194: LifeSavingBug += rnz(nastyduration); break;
				case 195: CurseuseEffect += rnz(nastyduration); break;
				case 196: CutNutritionEffect += rnz(nastyduration); break;
				case 197: SkillLossEffect += rnz(nastyduration); break;
				case 198: AutopilotEffect += rnz(nastyduration); break;
				case 199: MysteriousForceActive += rnz(nastyduration); break;
				case 200: MonsterGlyphChange += rnz(nastyduration); break;
				case 201: ChangingDirectives += rnz(nastyduration); break;
				case 202: ContainerKaboom += rnz(nastyduration); break;
				case 203: StealDegrading += rnz(nastyduration); break;
				case 204: LeftInventoryBug += rnz(nastyduration); break;
				case 205: FluctuatingSpeed += rnz(nastyduration); break;
				case 206: TarmuStrokingNora += rnz(nastyduration); break;
				case 207: FailureEffects += rnz(nastyduration); break;
				case 208: BrightCyanSpells += rnz(nastyduration); break;
				case 209: FrequentationSpawns += rnz(nastyduration); break;
				case 210: PetAIScrewed += rnz(nastyduration); break;
				case 211: SatanEffect += rnz(nastyduration); break;
				case 212: RememberanceEffect += rnz(nastyduration); break;
				case 213: PokelieEffect += rnz(nastyduration); break;
				case 214: AlwaysAutopickup += rnz(nastyduration); break;
				case 215: DywypiProblem += rnz(nastyduration); break;
				case 216: SilverSpells += rnz(nastyduration); break;
				case 217: MetalSpells += rnz(nastyduration); break;
				case 218: PlatinumSpells += rnz(nastyduration); break;
				case 219: ManlerEffect += rnz(nastyduration); break;
				case 220: DoorningEffect += rnz(nastyduration); break;
				case 221: NownsibleEffect += rnz(nastyduration); break;
				case 222: ElmStreetEffect += rnz(nastyduration); break;
				case 223: MonnoiseEffect += rnz(nastyduration); break;
				case 224: RangCallEffect += rnz(nastyduration); break;
				case 225: RecurringSpellLoss += rnz(nastyduration); break;
				case 226: AntitrainingEffect += rnz(nastyduration); break;
				case 227: TechoutBug += rnz(nastyduration); break;
				case 228: StatDecay += rnz(nastyduration); break;
				case 229: Movemork += rnz(nastyduration); break;
				case 230: BadPartBug += rnz(nastyduration); break;
				case 231: CompletelyBadPartBug += rnz(nastyduration); break;
				case 232: EvilVariantActive += rnz(nastyduration); break;
				case 233: SanityTrebleEffect += rnz(nastyduration); break;
				case 234: StatDecreaseBug += rnz(nastyduration); break;
				case 235: SimeoutBug += rnz(nastyduration); break;

				default: impossible("AD_MINA called with invalid value %d", midentity); break;
			}

		}

		break;

		case AD_RUNS:

		{
			register int nastyduration = ((tmp + 2) * rnd(10));
			if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) nastyduration *= 20;

			switch (u.adrunsattack) {

				case 1: RMBLoss += rnz(nastyduration); break;
				case 2: NoDropProblem += rnz(nastyduration); break;
				case 3: DSTWProblem += rnz(nastyduration); break;
				case 4: StatusTrapProblem += rnz(nastyduration); 
					if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
					if (HStun) set_itimeout(&HeavyStunned, HStun);
					if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
					if (HFeared) set_itimeout(&HeavyFeared, HFeared);
					if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
					if (HBurned) set_itimeout(&HeavyBurned, HBurned);
					if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
					if (Blinded) set_itimeout(&HeavyBlind, Blinded);
					if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
					break;
				case 5: Superscroller += rnz(nastyduration * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz(nastyduration); break;
				case 7: FreeHandLoss += rnz(nastyduration); break;
				case 8: Unidentify += rnz(nastyduration); break;
				case 9: Thirst += rnz(nastyduration); break;
				case 10: LuckLoss += rnz(nastyduration); break;
				case 11: ShadesOfGrey += rnz(nastyduration); break;
				case 12: FaintActive += rnz(nastyduration); break;
				case 13: Itemcursing += rnz(nastyduration); break;
				case 14: DifficultyIncreased += rnz(nastyduration); break;
				case 15: Deafness += rnz(nastyduration); flags.soundok = 0; break;
				case 16: CasterProblem += rnz(nastyduration); break;
				case 17: WeaknessProblem += rnz(nastyduration); break;
				case 18: RotThirteen += rnz(nastyduration); break;
				case 19: BishopGridbug += rnz(nastyduration); break;
				case 20: UninformationProblem += rnz(nastyduration); break;
				case 21: StairsProblem += rnz(nastyduration); break;
				case 22: AlignmentProblem += rnz(nastyduration); break;
				case 23: ConfusionProblem += rnz(nastyduration); break;
				case 24: SpeedBug += rnz(nastyduration); break;
				case 25: DisplayLoss += rnz(nastyduration); break;
				case 26: SpellLoss += rnz(nastyduration); break;
				case 27: YellowSpells += rnz(nastyduration); break;
				case 28: AutoDestruct += rnz(nastyduration); break;
				case 29: MemoryLoss += rnz(nastyduration); break;
				case 30: InventoryLoss += rnz(nastyduration); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (tmp * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz(nastyduration); break;
				case 33: BloodLossProblem += rnz(nastyduration); break;
				case 34: BadEffectProblem += rnz(nastyduration); break;
				case 35: TrapCreationProblem += rnz(nastyduration); break;
				case 36: AutomaticVulnerabilitiy += rnz(nastyduration); break;
				case 37: TeleportingItems += rnz(nastyduration); break;
				case 38: NastinessProblem += rnz(nastyduration); break;
				case 39: CaptchaProblem += rnz(nastyduration); break;
				case 40: FarlookProblem += rnz(nastyduration); break;
				case 41: RespawnProblem += rnz(nastyduration); break;
				case 42: RecurringAmnesia += rnz(nastyduration); break;
				case 43: BigscriptEffect += rnz(nastyduration); break;
				case 44: {
					BankTrapEffect += rnz(nastyduration);
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz(nastyduration); break;
				case 46: TechTrapEffect += rnz(nastyduration); break;
				case 47: RecurringDisenchant += rnz(nastyduration); break;
				case 48: verisiertEffect += rnz(nastyduration); break;
				case 49: ChaosTerrain += rnz(nastyduration); break;
				case 50: Muteness += rnz(nastyduration); break;
				case 51: EngravingDoesntWork += rnz(nastyduration); break;
				case 52: MagicDeviceEffect += rnz(nastyduration); break;
				case 53: BookTrapEffect += rnz(nastyduration); break;
				case 54: LevelTrapEffect += rnz(nastyduration); break;
				case 55: QuizTrapEffect += rnz(nastyduration); break;
				case 56: FastMetabolismEffect += rnz(nastyduration); break;
				case 57: NoReturnEffect += rnz(nastyduration); break;
				case 58: AlwaysEgotypeMonsters += rnz(nastyduration); break;
				case 59: TimeGoesByFaster += rnz(nastyduration); break;
				case 60: FoodIsAlwaysRotten += rnz(nastyduration); break;
				case 61: AllSkillsUnskilled += rnz(nastyduration); break;
				case 62: AllStatsAreLower += rnz(nastyduration); break;
				case 63: PlayerCannotTrainSkills += rnz(nastyduration); break;
				case 64: PlayerCannotExerciseStats += rnz(nastyduration); break;
				case 65: TurnLimitation += rnz(nastyduration); break;
				case 66: WeakSight += rnz(nastyduration); break;
				case 67: RandomMessages += rnz(nastyduration); break;

				case 68: Desecration += rnz(nastyduration); break;
				case 69: StarvationEffect += rnz(nastyduration); break;
				case 70: NoDropsEffect += rnz(nastyduration); break;
				case 71: LowEffects += rnz(nastyduration); break;
				case 72: InvisibleTrapsEffect += rnz(nastyduration); break;
				case 73: GhostWorld += rnz(nastyduration); break;
				case 74: Dehydration += rnz(nastyduration); break;
				case 75: HateTrapEffect += rnz(nastyduration); break;
				case 76: TotterTrapEffect += rnz(nastyduration); break;
				case 77: Nonintrinsics += rnz(nastyduration); break;
				case 78: Dropcurses += rnz(nastyduration); break;
				case 79: Nakedness += rnz(nastyduration); break;
				case 80: Antileveling += rnz(nastyduration); break;
				case 81: ItemStealingEffect += rnz(nastyduration); break;
				case 82: Rebellions += rnz(nastyduration); break;
				case 83: CrapEffect += rnz(nastyduration); break;
				case 84: ProjectilesMisfire += rnz(nastyduration); break;
				case 85: WallTrapping += rnz(nastyduration); break;
				case 86: DisconnectedStairs += rnz(nastyduration); break;
				case 87: InterfaceScrewed += rnz(nastyduration); break;
				case 88: Bossfights += rnz(nastyduration); break;
				case 89: EntireLevelMode += rnz(nastyduration); break;
				case 90: BonesLevelChange += rnz(nastyduration); break;
				case 91: AutocursingEquipment += rnz(nastyduration); break;
				case 92: HighlevelStatus += rnz(nastyduration); break;
				case 93: SpellForgetting += rnz(nastyduration); break;
				case 94: SoundEffectBug += rnz(nastyduration); break;
				case 95: TimerunBug += rnz(nastyduration); break;
				case 96: LootcutBug += rnz(nastyduration); break;
				case 97: MonsterSpeedBug += rnz(nastyduration); break;
				case 98: ScalingBug += rnz(nastyduration); break;
				case 99: EnmityBug += rnz(nastyduration); break;
				case 100: WhiteSpells += rnz(nastyduration); break;
				case 101: CompleteGraySpells += rnz(nastyduration); break;
				case 102: QuasarVision += rnz(nastyduration); break;
				case 103: MommaBugEffect += rnz(nastyduration); break;
				case 104: HorrorBugEffect += rnz(nastyduration); break;
				case 105: ArtificerBug += rnz(nastyduration); break;
				case 106: WereformBug += rnz(nastyduration); break;
				case 107: NonprayerBug += rnz(nastyduration); break;
				case 108: EvilPatchEffect += rnz(nastyduration); break;
				case 109: HardModeEffect += rnz(nastyduration); break;
				case 110: SecretAttackBug += rnz(nastyduration); break;
				case 111: EaterBugEffect += rnz(nastyduration); break;
				case 112: CovetousnessBug += rnz(nastyduration); break;
				case 113: NotSeenBug += rnz(nastyduration); break;
				case 114: DarkModeBug += rnz(nastyduration); break;
				case 115: AntisearchEffect += rnz(nastyduration); break;
				case 116: HomicideEffect += rnz(nastyduration); break;
				case 117: NastynationBug += rnz(nastyduration); break;
				case 118: WakeupCallBug += rnz(nastyduration); break;
				case 119: GrayoutBug += rnz(nastyduration); break;
				case 120: GrayCenterBug += rnz(nastyduration); break;
				case 121: CheckerboardBug += rnz(nastyduration); break;
				case 122: ClockwiseSpinBug += rnz(nastyduration); break;
				case 123: CounterclockwiseSpin += rnz(nastyduration); break;
				case 124: LagBugEffect += rnz(nastyduration); break;
				case 125: BlesscurseEffect += rnz(nastyduration); break;
				case 126: DeLightBug += rnz(nastyduration); break;
				case 127: DischargeBug += rnz(nastyduration); break;
				case 128: TrashingBugEffect += rnz(nastyduration); break;
				case 129: FilteringBug += rnz(nastyduration); break;
				case 130: DeformattingBug += rnz(nastyduration); break;
				case 131: FlickerStripBug += rnz(nastyduration); break;
				case 132: UndressingEffect += rnz(nastyduration); break;
				case 133: Hyperbluewalls += rnz(nastyduration); break;
				case 134: NoliteBug += rnz(nastyduration); break;
				case 135: ParanoiaBugEffect += rnz(nastyduration); break;
				case 136: FleecescriptBug += rnz(nastyduration); break;
				case 137: InterruptEffect += rnz(nastyduration); break;
				case 138: DustbinBug += rnz(nastyduration); break;
				case 139: ManaBatteryBug += rnz(nastyduration); break;
				case 140: Monsterfingers += rnz(nastyduration); break;
				case 141: MiscastBug += rnz(nastyduration); break;
				case 142: MessageSuppression += rnz(nastyduration); break;
				case 143: StuckAnnouncement += rnz(nastyduration); break;
				case 144: BloodthirstyEffect += rnz(nastyduration); break;
				case 145: MaximumDamageBug += rnz(nastyduration); break;
				case 146: LatencyBugEffect += rnz(nastyduration); break;
				case 147: StarlitBug += rnz(nastyduration); break;
				case 148: KnowledgeBug += rnz(nastyduration); break;
				case 149: HighscoreBug += rnz(nastyduration); break;
				case 150: PinkSpells += rnz(nastyduration); break;
				case 151: GreenSpells += rnz(nastyduration); break;
				case 152: EvencoreEffect += rnz(nastyduration); break;
				case 153: UnderlayerBug += rnz(nastyduration); break;
				case 154: DamageMeterBug += rnz(nastyduration); break;
				case 155: ArbitraryWeightBug += rnz(nastyduration); break;
				case 156: FuckedInfoBug += rnz(nastyduration); break;
				case 157: BlackSpells += rnz(nastyduration); break;
				case 158: CyanSpells += rnz(nastyduration); break;
				case 159: HeapEffectBug += rnz(nastyduration); break;
				case 160: BlueSpells += rnz(nastyduration); break;
				case 161: TronEffect += rnz(nastyduration); break;
				case 162: RedSpells += rnz(nastyduration); break;
				case 163: TooHeavyEffect += rnz(nastyduration); break;
				case 164: ElongationBug += rnz(nastyduration); break;
				case 165: WrapoverEffect += rnz(nastyduration); break;
				case 166: DestructionEffect += rnz(nastyduration); break;
				case 167: MeleePrefixBug += rnz(nastyduration); break;
				case 168: AutomoreBug += rnz(nastyduration); break;
				case 169: UnfairAttackBug += rnz(nastyduration); break;
				case 170: OrangeSpells += rnz(nastyduration); break;
				case 171: VioletSpells += rnz(nastyduration); break;
				case 172: LongingEffect += rnz(nastyduration); break;
				case 173: CursedParts += rnz(nastyduration); break;
				case 174: Quaversal += rnz(nastyduration); break;
				case 175: AppearanceShuffling += rnz(nastyduration); break;
				case 176: BrownSpells += rnz(nastyduration); break;
				case 177: Choicelessness += rnz(nastyduration); break;
				case 178: Goldspells += rnz(nastyduration); break;
				case 179: Deprovement += rnz(nastyduration); break;
				case 180: InitializationFail += rnz(nastyduration); break;
				case 181: GushlushEffect += rnz(nastyduration); break;
				case 182: SoiltypeEffect += rnz(nastyduration); break;
				case 183: DangerousTerrains += rnz(nastyduration); break;
				case 184: FalloutEffect += rnz(nastyduration); break;
				case 185: MojibakeEffect += rnz(nastyduration); break;
				case 186: GravationEffect += rnz(nastyduration); break;
				case 187: UncalledEffect += rnz(nastyduration); break;
				case 188: ExplodingDiceEffect += rnz(nastyduration); break;
				case 189: PermacurseEffect += rnz(nastyduration); break;
				case 190: ShroudedIdentity += rnz(nastyduration); break;
				case 191: FeelerGauges += rnz(nastyduration); break;
				case 192: LongScrewup += rnz(nastyduration * 20); break;
				case 193: WingYellowChange += rnz(nastyduration); break;
				case 194: LifeSavingBug += rnz(nastyduration); break;
				case 195: CurseuseEffect += rnz(nastyduration); break;
				case 196: CutNutritionEffect += rnz(nastyduration); break;
				case 197: SkillLossEffect += rnz(nastyduration); break;
				case 198: AutopilotEffect += rnz(nastyduration); break;
				case 199: MysteriousForceActive += rnz(nastyduration); break;
				case 200: MonsterGlyphChange += rnz(nastyduration); break;
				case 201: ChangingDirectives += rnz(nastyduration); break;
				case 202: ContainerKaboom += rnz(nastyduration); break;
				case 203: StealDegrading += rnz(nastyduration); break;
				case 204: LeftInventoryBug += rnz(nastyduration); break;
				case 205: FluctuatingSpeed += rnz(nastyduration); break;
				case 206: TarmuStrokingNora += rnz(nastyduration); break;
				case 207: FailureEffects += rnz(nastyduration); break;
				case 208: BrightCyanSpells += rnz(nastyduration); break;
				case 209: FrequentationSpawns += rnz(nastyduration); break;
				case 210: PetAIScrewed += rnz(nastyduration); break;
				case 211: SatanEffect += rnz(nastyduration); break;
				case 212: RememberanceEffect += rnz(nastyduration); break;
				case 213: PokelieEffect += rnz(nastyduration); break;
				case 214: AlwaysAutopickup += rnz(nastyduration); break;
				case 215: DywypiProblem += rnz(nastyduration); break;
				case 216: SilverSpells += rnz(nastyduration); break;
				case 217: MetalSpells += rnz(nastyduration); break;
				case 218: PlatinumSpells += rnz(nastyduration); break;
				case 219: ManlerEffect += rnz(nastyduration); break;
				case 220: DoorningEffect += rnz(nastyduration); break;
				case 221: NownsibleEffect += rnz(nastyduration); break;
				case 222: ElmStreetEffect += rnz(nastyduration); break;
				case 223: MonnoiseEffect += rnz(nastyduration); break;
				case 224: RangCallEffect += rnz(nastyduration); break;
				case 225: RecurringSpellLoss += rnz(nastyduration); break;
				case 226: AntitrainingEffect += rnz(nastyduration); break;
				case 227: TechoutBug += rnz(nastyduration); break;
				case 228: StatDecay += rnz(nastyduration); break;
				case 229: Movemork += rnz(nastyduration); break;
				case 230: BadPartBug += rnz(nastyduration); break;
				case 231: CompletelyBadPartBug += rnz(nastyduration); break;
				case 232: EvilVariantActive += rnz(nastyduration); break;
				case 233: SanityTrebleEffect += rnz(nastyduration); break;
				case 234: StatDecreaseBug += rnz(nastyduration); break;
				case 235: SimeoutBug += rnz(nastyduration); break;

				default: impossible("AD_RUNS called with invalid value %d", u.adrunsattack); break;
			}

		}

		break;

	      case AD_SIN:

			You_feel("sinful!");
			u.ualign.sins++;
			u.alignlim--;
			adjalign(-5);

			break;

	      case AD_ALIN:

			if (tmp) {
				You_feel("less aligned!");
				adjalign(-tmp);
			}

			break;

	      case AD_CONT:

			You("are being contaminated!");
			contaminate(tmp, TRUE);

			break;

	      case AD_STUN:
			{
				You("seem less steady!");
			    make_stunned(HStun + tmp, TRUE);
			}
			break;
	      case AD_NUMB:
			{
				You_feel("your body parts getting numb!");
			    make_numbed(HNumbed + tmp, TRUE);
			}
			break;
	      case AD_FRZE:
				You_feel("ice cold!");
			if (!rn2(3)) {
			    make_frozen(HFrozen + tmp, TRUE);
			}
			break;
	      case AD_BURN:
				You_feel("an overwhelming heat!");
			if (!rn2(2)) {
			    make_burned(HBurned + tmp, TRUE);
			}
			break;
	      case AD_DIMN:
				You_feel("some dimness inside!");
			if (!rn2(2)) {
			    make_dimmed(HDimmed + tmp, TRUE);
			}
			break;
	      case AD_FEAR:
				You_feel("a tight squeezing!");
			if (!rn2(2)) {
			    make_feared(HFeared + tmp, TRUE);
			}
			break;
		case AD_SANI:
				You_feel("atrocious!");
				increasesanity(tmp);
			break;
		case AD_INSA:
				You_feel("insane!");
			make_feared(HFeared + tmp, TRUE);
			make_stunned(HStun + tmp, TRUE);
			if(Confusion) You("are getting even more confused.");
			else You("are getting confused.");
			make_confused(HConfusion + tmp, FALSE);
			increasesanity(1);
			break;

	      case AD_SLOW:
				You_feel("a force field!");
			if (HFast && !defends(AD_SLOW, uwep) && !rn2(4)) {
			    u_slow_down();

			    if (Race_if(PM_SPIRIT) && !rn2(3)) {
				u.uprops[DEAC_FAST].intrinsic += ((tmp + 2) * 10);
				pline(u.inertia ? "You feel almost unable to move..." : "You feel very lethargic...");
				u.inertia += (tmp + 2);
			    }
			}
			break;
	      case AD_PLYS:
			pline("It whacks you!");
			if (multi >= 0 && !rn2(3)) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("cramp for a moment.");            
			    } else {
				You("can't move!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				{
					int paralysistime = tmp;
					if (paralysistime > 1) paralysistime = rnd(paralysistime);
					if (paralysistime > 5) {
						while (rn2(5) && (paralysistime > 5)) {
							paralysistime--;
						}
					}

					if (!rn2(3)) nomul(-rnd(5), "paralyzed by an engulfing monster", TRUE);
					else nomul(-(paralysistime), "paralyzed by an engulfing monster", TRUE);
				}
				exercise(A_DEX, FALSE);
			    }
			}
			break;
	      case AD_DRLI:
			pline("It sucks you!");
/* Imagine the facial expression of a player who thinks this is the mind flayer's amnesia attack. --Amy */

			if (moves < 1000 && (u.urmaxlvlUP == 1) && rn2(25)) {
				break;
			}

			if (!rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )  ) {
			    losexp("life drainage", FALSE, TRUE);
			}
			break;
	      case AD_VAMP:
			pline("It sucks you!");

			if (moves < 1000 && (u.urmaxlvlUP == 1) && rn2(25)) {
				break;
			}

			if (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) ) {
			    losexp("life drainage", FALSE, TRUE);
			}
			break;
	      case AD_DREN:
			pline("It drains you!");
			if (!rn2(4)) drain_en(tmp);
			break;

	      case AD_WEBS: /* No message. Because you can't see the web while engulfed. */
			(void) maketrap(u.ux, u.uy, WEB, 0, FALSE);
			/* Amy addition: sometimes, also make a random trap somewhere on the level :D */
			if (!rn2(8)) makerandomtrap(FALSE);
			break;

	      case AD_TRAP:
			if (t_at(u.ux, u.uy) == 0) (void) maketrap(u.ux, u.uy, randomtrap(), 0, FALSE);
			else makerandomtrap(FALSE);

			break;

	    case AD_DEPR:
			You_feel("manic-depressive...");
		if (!rn2(5)) {

		    switch(rnd(20)) {
		    case 1:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
			      polyself(FALSE);
			}
			break;
		    case 2:
			You("need reboot.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
			if (!Race_if(PM_UNGENOMOLD)) newman();
			else polyself(FALSE);
			break;
		    case 3: case 4:
			if(!rn2(4) && u.ulycn == NON_PM &&
				!Protection_from_shape_changers &&
				!is_were(youmonst.data) &&
				!defends(AD_WERE,uwep)) {
			    You_feel("feverish.");
			    exercise(A_CON, FALSE);
			    u.ulycn = PM_WERECOW;
			    u.cnd_lycanthropecount++;
			} else {
				if (multi >= 0) {
				    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) break;
				    fall_asleep(-rnd(10), TRUE);
				    if (Blind) You("are put to sleep!");
				    else You("are put to sleep by %s!", mon_nam(mtmp));
				}
			}
			break;
		    case 5: case 6:
			if (!u.ustuck && !sticks(youmonst.data)) {
				setustuck(mtmp);
				pline("%s grabs you!", Monnam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
			break;
		    case 7:
		    case 8:
			Your("position suddenly seems very uncertain!");
			teleX();
			break;
		    case 9:
			u_slow_down();
			break;
		    case 10:
			hurtarmor(AD_RUST);
			break;
		    case 11:
			hurtarmor(AD_DCAY);
			break;
		    case 12:
			hurtarmor(AD_CORR);
			break;
		    case 13:
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(5), "paralyzed by an engulfing monster", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 14:
			if (FunnyHallu)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			hallutime = rn1(7, 16);
			make_stunned(HStun + hallutime + tmp, FALSE);
			(void) make_hallucinated(HHallucination + hallutime + tmp,TRUE,0L);
			break;
		    case 15:
			if(!Blind)
				Your("vision bugged.");
			hallutime += rn1(10, 25);
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + tmp + tmp,TRUE,0L);
			break;
		    case 16:
			if(!Blind)
				Your("vision turns to screen saver.");
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + tmp,TRUE,0L);
			break;
		    case 17:
			{
			    struct obj *obj = some_armor(&youmonst);

			    if (obj && drain_item(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			    }
			}
			break;
		    default:
			    if(Confusion)
				 You("are getting even more confused.");
			    else You("are getting confused.");
			    make_confused(HConfusion + tmp, FALSE);
			break;
		    }
		    exercise(A_INT, FALSE);

		}
		break;

	    case AD_WRAT:
		You_feel("the life vanish from within you!");

		if(u.uen < 1) {
		    You_feel("less energised!");
		    u.uenmax -= rn1(10,10);
		    if(u.uenmax < 0) u.uenmax = 0;
		} else if(u.uen <= 10) {
		    You_feel("your magical energy dwindle to nothing!");
		    u.uen = 0;
		} else {
		    You_feel("your magical energy dwindling rapidly!");
		    u.uen /= 2;
		}

		break;

	    case AD_LAZY: /* laziness attack; do lots of nasty things at random */
		if(!rn2(2)) {
		    You_feel("momentarily lethargic.");
		    break;
		}
		You_feel("apathetic...");
		switch(rn2(7)) {
		    case 0: /* destroy certain things */
			witherarmor();
			break;
		    case 1: /* sleep */
			if (multi >= 0) {
			    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) {pline("You yawn."); break;}
			    fall_asleep(-rnd(10), TRUE);
			    if (Blind) You("are put to sleep!");
			    else You("are put to sleep by %s!", mon_nam(mtmp));
			}
			break;
		    case 2: /* paralyse */
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(5), "paralyzed by an engulfing monster", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 3: /* slow */
			if(HFast)  u_slow_down();
			else You("pause momentarily.");
			break;
		    case 4: /* drain Dex */
			adjattrib(A_DEX, -rn1(1,1), 0, TRUE);
			break;
		    case 5: /* steal teleportitis */
			if(HTeleportation & INTRINSIC) {
			      HTeleportation &= ~INTRINSIC;
			}
	 		if (HTeleportation & TIMEOUT) {
				HTeleportation &= ~TIMEOUT;
			}
			if(HTeleport_control & INTRINSIC) {
			      HTeleport_control &= ~INTRINSIC;
			}
	 		if (HTeleport_control & TIMEOUT) {
				HTeleport_control &= ~TIMEOUT;
			}
		      You("don't feel in the mood for jumping around.");
			break;
		    case 6: /* steal sleep resistance */
			if(HSleep_resistance & INTRINSIC) {
				HSleep_resistance &= ~INTRINSIC;
			} 
			if(HSleep_resistance & TIMEOUT) {
				HSleep_resistance &= ~TIMEOUT;
			} 
			You_feel("like you could use a nap.");
			break;
		}
		break;

	    case AD_DFOO:
	      You_feel("physically and mentally weaker!");
		if (!rn2(3)) {
		    sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, rn2(A_MAX), mtmp->data->mname, 30);
		}
		if (!rn2(4)) {
			You_feel("drained...");
			u.uhpmax -= rn1(10,10);
			if (u.uhpmax < 1) u.uhpmax = 1;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (!rn2(4)) {
			You_feel("less energised!");
			u.uenmax -= rn1(10,10);
			if (u.uenmax < 0) u.uenmax = 0;
			if(u.uen > u.uenmax) u.uen = u.uenmax;
		}
		if (!rn2(4)) {
			if((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && (u.urmaxlvlUP >= 2) )
			    losexp("life drainage", FALSE, TRUE);
			else You_feel("woozy for an instant, but shrug it off.");
		}

		break;


	    case AD_GLIB:
			pline("A disgusting substance pours all over your hands!");

			/* hurt the player's hands --Amy */
			incr_itimeout(&Glib, tmp);
			break;

	    case AD_DARK:

			if (youmonst.data->mlet == S_ANGEL || Race_if(PM_HUMANOID_ANGEL)) tmp *= 2;

			You_feel("a constricting darkness...");

			if (Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) break;

			/* create darkness around the player --Amy */
			litroomlite(FALSE);
			break;

	    case AD_LEGS:
			{ register long sideX = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
	
			if (tmp <= 5)
		    	    Your("legs itch badly for a moment.");
			else if (tmp <= 10)
			    pline("Wounds appear on your legs!");
			else if (tmp <= 20)
			    pline("Severe wounds appear on your legs!");
			else if (tmp <= 40)
			    Your("legs are covered with painful wounds!");
			else
			    Your("legs are covered with bloody wounds! It hurts like hell! Auuuggghhhh!");
			set_wounded_legs(sideX, HWounded_legs + rnd(60-ACURR(A_DEX)));
			exercise(A_STR, FALSE);
			exercise(A_DEX, FALSE);
			}
			break;
	    case AD_STON:
		if (StrongStone_resistance && rn2(10) && tmp > 1) tmp = 1;
	    pline("It strikes you hard!");
		if(!rn2(3)) {
			if (flags.soundok) {
			    You_hear("a hissing noise!");
			    if (PlayerHearsSoundEffects) pline(issoviet ? "Eto menyayet status pamyatnika. I vy, veroyatno, ne imeyut dostatochnogo kolichestva predmetov otverzhdeniya, potomu chto vy vpustuyu ikh vse uzhe KHAR ty glupyy nub." : "Schhhhhhhhhhhhh!");
			}
			if (uarmc && itemhasappearance(uarmc, APP_PETRIFIED_CLOAK) && rn2(4)) break;

do_stone2:
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
			    if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = mtmp->data->mname;
					if (mtmp->data->geno & G_UNIQ) {
					    if (!type_is_pname(mtmp->data)) {
						static char kbuf[BUFSZ];
	
						/* "the" buffer may be reallocated */
						strcpy(kbuf, the(delayed_killer));
						delayed_killer = kbuf;
					    }
					    killer_format = KILLED_BY;
					} else killer_format = KILLED_BY_AN;
					return(1);
					/* You("turn to stone..."); */
					/* done_in_by(mtmp); */
				}
			    }
			}
		}
		break;
	    case AD_EDGE:
	    pline("It strikes and slits you!");
		if (!Stone_resistance || !rn2(StrongStone_resistance ? 100 : 20)) {
			if (Upolyd) {u.mhmax--; if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax--; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }
		}
		if(!rn2(3)) {
			if (flags.soundok) {
			    You_hear("a hissing noise!");
			    if (PlayerHearsSoundEffects) pline(issoviet ? "Eto menyayet status pamyatnika. I vy, veroyatno, ne imeyut dostatochnogo kolichestva predmetov otverzhdeniya, potomu chto vy vpustuyu ikh vse uzhe KHAR ty glupyy nub." : "Schhhhhhhhhhhhh!");
			}
			if (uarmc && itemhasappearance(uarmc, APP_PETRIFIED_CLOAK) && rn2(4)) break;

			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
			    if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = mtmp->data->mname;
					if (mtmp->data->geno & G_UNIQ) {
					    if (!type_is_pname(mtmp->data)) {
						static char kbuf[BUFSZ];
	
						/* "the" buffer may be reallocated */
						strcpy(kbuf, the(delayed_killer));
						delayed_killer = kbuf;
					    }
					    killer_format = KILLED_BY;
					} else killer_format = KILLED_BY_AN;
					return(1);
					/* You("turn to stone..."); */
					/* done_in_by(mtmp); */
				}
			    }
			}
		}
		break;
	    case AD_STCK:
	    pline("You are covered with some sticky substance!");
		if (!u.ustuck && !(uwep && uwep->oartifact == ART_FOAMONIA_WATER) && !sticks(youmonst.data)) {
			setustuck(mtmp);
			pline("%s grabs you!", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}
		break;
	    case AD_SGLD:
		    pline("It shakes you!");

		if (!issoviet && !rn2(3)) {
			You_feel("a tug on your purse"); break;
		}

		if (rn2(10)) {stealgold(mtmp);
		break;
		}
		/* fall thru --Amy */

	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
	    case AD_SSEX:
		pline("It thrusts you!");

		if (!rn2(3) && atttypA != AD_SEDU && !issoviet && !(u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) || have_stealerstone() || (uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) ) ) {
			You_feel("a tug on your knapsack"); break;
		}

			buf[0] = '\0';

		if (u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) || have_stealerstone() || (uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) ) {
			switch (steal(mtmp, buf, atttypA == AD_SEDU ? TRUE : FALSE, FALSE)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp) && (issoviet || atttypA == AD_SEDU || !rn2(4) ) )
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, rnd(10), FALSE, FALSE);
			return 3;
			};
		} else if (issoviet || ( (rnd(100) > ACURR(A_CHA)) && ((mtmp->female) && !flags.female && rn2(5) ) || ((!mtmp->female) && flags.female && rn2(3) ) || 
			((mtmp->female) && flags.female && rn2(2) ) || ((!mtmp->female) && !flags.female && rn2(2) ) ) )
			{ 
			switch (steal(mtmp, buf, atttypA == AD_SEDU ? TRUE : FALSE, FALSE)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp) && (issoviet || atttypA == AD_SEDU || !rn2(4) ) )
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, rnd(10), FALSE, FALSE);
			return 3;
			};
		}
		break;

	    case AD_STTP:

		pline("You are surrounded by a purple glow!");
		if (invent) {
		    int itemportchance = 10 + rn2(21);
		    for (otmpi = invent; otmpi; otmpi = otmpii) {

		      otmpii = otmpi->nobj;

			if (!rn2(itemportchance) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}

		    }
		}
		break;

	    case AD_TLPT:
	    case AD_ABDC:
			pline("A stream of energy irradiates you!");
		    if(flags.verbose)
			pline("You are teleported away!");
		    teleX();
		break;
	    case AD_DISP:
		pline("You're shaken around!");
		pushplayer(FALSE);
		break;

	    case AD_SAMU:
		pline("You're pummeled with debris!");
		if (!rn2(20)) stealamulet(mtmp);
		break;

	    case AD_UVUU:{
		pline("A drill bores into your brain!");
		if (rn2(10)) break;
		int wdmg = (int)(tmp/6) + 1;
		sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		poisoned(buf, A_CON, mtmp->data->mname, 60);
		if(Poison_resistance) wdmg -= ACURR(A_CON)/2;
		if (StrongPoison_resistance && wdmg > 1) wdmg /= 2;
		if(wdmg > 0){
		
			while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
				wdmg--;
				(void) adjattrib(A_WIS, -1, TRUE, TRUE);
				forget_levels(1);	/* lose memory of 1% of levels per point lost*/
				forget_objects(1);	/* lose memory of 1% of objects per point lost*/
				exercise(A_WIS, FALSE);
			}
			if(AMAX(A_WIS) > ATTRMIN(A_WIS) && 
				ABASE(A_WIS) < AMAX(A_WIS)/2) AMAX(A_WIS) -= 1; //permanently drain wisdom
			if(wdmg){
				boolean chg;
				chg = make_hallucinated(HHallucination + (long)(wdmg*5),FALSE,0L);
			}
		}
		drain_en( (int)(tmp/2) );
		if(!rn2(20)){
			if (!has_head(youmonst.data) || Role_if(PM_COURIER) ) {
				tmp *= 2;
			}
			else if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) {
				pline("The drill passes through your %s.", body_part(HEAD));
				tmp *= 2;
			}
			else {
				if(!uarmh){
					tmp = (ABASE(A_WIS) <= ATTRMIN(A_WIS)) ? ( 2 * (Upolyd ? u.mh : u.uhp) + 400) : (tmp * 2); 
					pline("The drill penetrates your %s and bores it into two halves!", body_part(HEAD));
				} else pline("The drill penetrates your %s!", xname(uarmh) );
			}
		 }
 		}
	    break;

	    case AD_RUST:
		pline("You are covered with rust!");
		if (u.umonnum == PM_IRON_GOLEM) {
			You("rust!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		}
		if (evilfriday || rn2(3)) hurtarmor(AD_RUST);
		break;
	    case AD_CORR:
		pline("You are covered with acid!");
		if (Stoned) fix_petrification();
		if (evilfriday || rn2(3)) hurtarmor(AD_CORR);
		break;

	    case AD_LAVA:
		pline("You are covered with lava!");

		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly hot.");
				ugolemeffects(AD_FIRE,tmp);
				tmp = 0;
			} else You("are burning to a crisp!");
			burn_away_slime();
		    } else tmp = 0;

		if (evilfriday || rn2(3)) hurtarmor(AD_LAVA);
		break;

	    case AD_NEXU:
		if (mtmp->mcan) break;
		You_feel("an energy irradiation!");

		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= (1 + rnd(2));

		switch (rnd(7)) {

			case 1:
			case 2:
			case 3:
				pline("%s sends you far away!", Monnam(mtmp) );
				teleX();
				break;
			case 4:
			case 5:
				pline("%s sends you away!", Monnam(mtmp) );
				phase_door(0);
				break;
			case 6:

				if ((!u.uevent.udemigod || u.freeplaymode) && !playerlevelportdisabled() ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
					}
				}
				break;
			case 7:
				{
					nexus_swap();

					if (!rn2(3)) {

						int reducedstat = rn2(A_MAX);
						if(ABASE(reducedstat) <= ATTRMIN(reducedstat)) {
							pline("Your health was damaged!");
							u.uhpmax -= rnd(5);
							if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
							if (u.uhp < 1) {
								u.youaredead = 1;
								killer = "nexus scrambling";
								killer_format = KILLED_BY;
								done(DIED);
								u.youaredead = 0;
							}

						} else {
							ABASE(reducedstat) -= 1;
							AMAX(reducedstat) -= 1;
							flags.botl = 1;
							pline("Your attributes were damaged!");
						}
					}
				}
				break;
		}
		break;

	    case AD_SOUN:
		if (mtmp->mcan) break;
		pline("AUUUUUUGGGGGHHHHHGGHH - the noise in here is unbearable!");
		if (YouAreDeaf) tmp /= 2;
		make_stunned(HStun + tmp, TRUE);
		if (isevilvariant || !rn2(issoviet ? 2 : 5)) (void)destroy_item(POTION_CLASS, AD_COLD);
		wake_nearby();
		break;

	    case AD_GRAV:
		if (mtmp->mcan) break;

		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= 2;

		pline("You're turned upside down...");
		phase_door(0);
		pushplayer(FALSE);
		u.uprops[DEAC_FAST].intrinsic += (tmp + 2);
		make_stunned(HStun + tmp, TRUE);
		break;

	    case AD_WGHT:
		if (mtmp->mcan) break;
		pline("Your pack feels much heavier!");
		IncreasedGravity += (1 + (tmp * rnd(20)));

		break;
	    case AD_INER:
		if (mtmp->mcan) break;
	      u_slow_down();
		u.uprops[DEAC_FAST].intrinsic += ((tmp + 2) * 10);
		pline(u.inertia ? "You feel almost unable to move..." : "You feel very lethargic...");
		u.inertia += (tmp + 2);
		break;

	    case AD_TIME:
		if (mtmp->mcan) break;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) break;
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Zhizn' razgonyal nazad, potomu chto vy ne smotreli, i teper' vy dolzhny poluchit', chto poteryannyy uroven' nazad." : "Kloeck!");
			      if (u.urmaxlvlUP >= 2) losexp("time", FALSE, FALSE); /* resistance is futile :D */
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				switch (rnd(A_MAX)) {
					case A_STR:
						pline("You're not as strong as you used to be...");
						ABASE(A_STR) -= 5;
						if(ABASE(A_STR) < ATTRMIN(A_STR)) {tmp *= 3; ABASE(A_STR) = ATTRMIN(A_STR);}
						break;
					case A_DEX:
						pline("You're not as agile as you used to be...");
						ABASE(A_DEX) -= 5;
						if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {tmp *= 3; ABASE(A_DEX) = ATTRMIN(A_DEX);}
						break;
					case A_CON:
						pline("You're not as hardy as you used to be...");
						ABASE(A_CON) -= 5;
						if(ABASE(A_CON) < ATTRMIN(A_CON)) {tmp *= 3; ABASE(A_CON) = ATTRMIN(A_CON);}
						break;
					case A_WIS:
						pline("You're not as wise as you used to be...");
						ABASE(A_WIS) -= 5;
						if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {tmp *= 3; ABASE(A_WIS) = ATTRMIN(A_WIS);}
						break;
					case A_INT:
						pline("You're not as bright as you used to be...");
						ABASE(A_INT) -= 5;
						if(ABASE(A_INT) < ATTRMIN(A_INT)) {tmp *= 3; ABASE(A_INT) = ATTRMIN(A_INT);}
						break;
					case A_CHA:
						pline("You're not as beautiful as you used to be...");
						ABASE(A_CHA) -= 5;
						if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {tmp *= 3; ABASE(A_CHA) = ATTRMIN(A_CHA);}
						break;
				}
				break;
			case 10:
				pline("You're not as powerful as you used to be...");
				ABASE(A_STR)--;
				ABASE(A_DEX)--;
				ABASE(A_CON)--;
				ABASE(A_WIS)--;
				ABASE(A_INT)--;
				ABASE(A_CHA)--;
				if(ABASE(A_STR) < ATTRMIN(A_STR)) {tmp *= 2; ABASE(A_STR) = ATTRMIN(A_STR);}
				if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {tmp *= 2; ABASE(A_DEX) = ATTRMIN(A_DEX);}
				if(ABASE(A_CON) < ATTRMIN(A_CON)) {tmp *= 2; ABASE(A_CON) = ATTRMIN(A_CON);}
				if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {tmp *= 2; ABASE(A_WIS) = ATTRMIN(A_WIS);}
				if(ABASE(A_INT) < ATTRMIN(A_INT)) {tmp *= 2; ABASE(A_INT) = ATTRMIN(A_INT);}
				if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {tmp *= 2; ABASE(A_CHA) = ATTRMIN(A_CHA);}
				break;
		}
		break;

	    case AD_PLAS:
		if (mtmp->mcan) break;

			pline("It's extremely hot in here!");
			if (!Fire_resistance) tmp *= 2;
			if (StrongFire_resistance && tmp > 1) tmp /= 2;

		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) /* extremely hot - very high chance to burn items! --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
			make_stunned(HStun + tmp, TRUE);

		break;

	    case AD_MANA:
		if (mtmp->mcan) break;
		drain_en(tmp);
		break;

	    case AD_SKIL:
		if (mtmp->mcan) break;
		if (!rn2(10)) skillcaploss();
		break;

	    case AD_TDRA:
		if (mtmp->mcan) break;
		if (!rn2(10)) techdrain();
		break;

	    case AD_DROP:
		if (mtmp->mcan) break;
		dropitemattack();
		break;

	    case AD_BLAS:
		if (mtmp->mcan) break;
		if (!rn2(25)) {
			u.ugangr++;
		      You("get the feeling that %s is angry...", u_gname());
		}
		break;

	    case AD_WTHR:
		pline("You are covered with some aggressive substance!");
		if (evilfriday || rn2(3)) witherarmor();
		break;

	    case AD_FAKE:
		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}
		break;

	    case AD_AMNE:
		pline("You can't remember seeing anything like this before.");
		if (FunnyHallu) pline("You also can't remember ever suffering from amnesia.");
		maprot();
		break;

	    case AD_LETH:
		pline("Sparkling water washes all around you!");
		if (mtmp->mcan) break;
		if (!rn2(10)) {
			pline("You sparkle!");
			lethe_damage(invent, FALSE, FALSE);
			if (!rn2(3)) actual_lethe_damage(invent, FALSE, FALSE);
			if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
			if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		}
		break;

	    case AD_WET:
		pline("Water washes all around you!");
		if (mtmp->mcan) break;
		if (!rn2(10)) {
			pline("You are soaked with water!");
			if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
				water_damage(invent, FALSE, FALSE);
				if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
			}
		}
		break;

	    case AD_SUCK:
		You_feel("like being sucked in by a vacuum cleaner!");
			if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) tmp = 0;
			else{
				if( has_head(youmonst.data) && !(Role_if(PM_COURIER)) && !uarmh && !rn2(20) && 
					((!Upolyd && u.uhp < (u.uhpmax / 10) ) || (Upolyd && u.mh < (u.mhmax / 10) ))
				){
					tmp = 2 * (Upolyd ? u.mh : u.uhp)
						  + 400; //FATAL_DAMAGE_MODIFIER;
					pline("Your %s is sucked off!", body_part(HEAD));
				}
				else{
					You_feel("your extremities being sucked off!");
					if(!rn2(10)){
						Your("%s twist from the suction!", makeplural(body_part(LEG)));
					    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    set_wounded_legs(LEFT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    exercise(A_STR, FALSE);
					    exercise(A_DEX, FALSE);
					}
					if(uwep && !rn2(6)){
						You_feel("a pull on your weapon!");
						if( rnd(130) > ACURR(A_STR)){
							Your("weapon is sucked out of your grasp!");
							optr = uwep;
							uwepgone();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						}
						else{
							You("keep a tight grip on your weapon!");
						}
					}
					if(!rn2(10) && uarmf){
						Your("boots are sucked off!");
						optr = uarmf;
						if (donning(optr)) cancel_don();
						(void) Boots_off();
						freeinv(optr);
						(void) mpickobj(mtmp,optr,FALSE);
					}
					if(!rn2(6) && uarmg && !uwep){
						You_feel("a pull on your gloves!");
						if( rnd(130) > ACURR(A_STR)){
							Your("gloves are sucked off!");
							optr = uarmg;
							if (donning(optr)) cancel_don();
							(void) Gloves_off();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						}
						else You("keep your %s closed.", makeplural(body_part(HAND)));
					}
					if(!rn2(8) && uarms){
						You_feel("a pull on your shield!");
						if( rnd(150) > ACURR(A_STR)){
							Your("shield is sucked out of your grasp!");
							optr = uarms;
							if (donning(optr)) cancel_don();
							Shield_off();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						 }
						 else{
							You("keep a tight grip on your shield!");
						 }
					}
					if(!rn2(4) && uarmh){
						Your("helmet is sucked off!");
						optr = uarmh;
						if (donning(optr)) cancel_don();
						(void) Helmet_off();
						freeinv(optr);
						(void) mpickobj(mtmp,optr,FALSE);
					}
				}
			}
		break;

	    case AD_CNCL:
		pline("Sparkling lights are dancing around you!");
		if (mtmp->mcan) break;
		if (!rn2(10)) {
			(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		}
		break;

	    case AD_BANI:
		if (!rn2(10)) {
			if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds_normalterrain(FALSE); break;}
			if (playerlevelportdisabled()) {
				pline("For some reason you resist the banishment!");
				break;
			}

			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.banishmentbeam) {
				u.banishmentbeam = 1;
				nomul(-2, "being banished", FALSE); /* because it's not called until you get another turn... */
			}
		}
		break;

	    case AD_WEEP:
		if (flags.soundok) You_hear("weeping sounds!");
		if (rn2(10)) break;
		if (!rn2(3) && (!u.uevent.udemigod || u.freeplaymode) && !playerlevelportdisabled() ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
			if (!u.levelporting) {
				u.levelporting = 1;
				nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
			}
		}
		else if (!rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) ) && (u.urmaxlvlUP >= 2) ) {
		    losexp("loss of potential", FALSE, TRUE);
		}
		break;

	    case AD_LUCK:
		pline("It's sucking away all of your good feelings!");
		if (!rn2(3)) change_luck(-1);
		break;
	    case AD_CONF:
		    pline("Your head spins wildly!");
		    make_confused(HConfusion + tmp, FALSE);
		break;
	    case AD_DCAY:
		pline("You are covered with decaying waste!");
		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("rot!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			u.mh = 0;
			/* KMH, balance patch -- this is okay with unchanging */
			rehumanize();
			break;
		}
		if (evilfriday || rn2(3)) hurtarmor(AD_DCAY);
		break;
	    case AD_FLAM:
		pline("You are engulfed in flames!");
		if (evilfriday || rn2(3)) hurtarmor(AD_FLAM);
		break;
	    case AD_HALU:
		    pline("You inhale some great stuff!");
		    make_hallucinated(HHallucination + tmp,FALSE,0L);
		break;
	    case AD_DETH:
		pline("You see your life flash in front of your eyes...");
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("But since you're undead already, this doesn't matter to you.");
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!PlayerResistsDeathRays) {
			u.youaredead = 1;
			killer_format = KILLED_BY_AN;
			killer = "engulf of death";
			done(DIED);
			u.youaredead = 0;
			tmp = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You_feel("your life force draining away...");
			if (Antimagic || (Half_spell_damage && rn2(2) ) || (StrongHalf_spell_damage && rn2(2)) ) {
				shieldeff(u.ux, u.uy);
				tmp /= (StrongAntimagic ? 4 : 2);
			}
			u.uhpmax -= tmp/2;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("Nothing happens.");
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		    tmp = 0;
		    break;
		}
		break;
	    case AD_FAMN:
		pline("You are being %s!", (mtmp->data == &mons[PM_LEAD_SUGAR]) ? "stuffed with bitter sugar" : "eaten");
		exercise(A_CON, FALSE);

		if (mtmp->data == &mons[PM_LEAD_SUGAR]) {
			lesshungry(rnz(80) + (tmp * 2));
		} else {
			if (!is_fainted()) morehungry(rnz(40));
			if (!is_fainted()) morehungry(rnz(40));
			morehungry(tmp);
			morehungry(tmp);
		}

		/* plus the normal damage */
		break;
	    case AD_SLIM:    
		pline("A disgusting green goo pours all over you!");
		if (rn2(3)) break;
		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		    tmp = 0;
		} else if (Unchanging || slime_on_touch(youmonst.data) ) {
		    You("are unaffected.");
		    tmp = 0;
		} else if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    char kbuf[BUFSZ];
		    sprintf(kbuf, "slimed by %s", mtmp->data->mname);
		    delayed_killer = kbuf;
		} else
		    pline("Yuck!");
		break;
	    case AD_LITT:
		pline("A huge amount of run-off litter pours all over you!");
		if (rn2(3)) break;
		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		    tmp = 0;
		} else if (Unchanging || slime_on_touch(youmonst.data) ) {
		    You("are unaffected.");
		    tmp = 0;
		} else if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(20);
		    killer_format = KILLED_BY_AN;
		    char kbuf[BUFSZ];
		    sprintf(kbuf, "slimed by %s", mtmp->data->mname);
		    delayed_killer = kbuf;
		} else
		    pline("Yuck!");

		{
		    register struct obj *littX, *littX2;
		    for (littX = invent; littX; littX = littX2) {
		      littX2 = littX->nobj;
			if (!rn2(StrongAcid_resistance ? 1000 : Acid_resistance ? 100 : 10)) rust_dmg(littX, xname(littX), 3, TRUE, &youmonst);
		    }
		}

		break;
	    case AD_CALM:	/* KMH -- koala attack */
		You_feel("something sapping your energy!");
		    docalm();
		break;
	    case AD_FREN:
		pline("You suddenly feel angry!");
			if (u.berserktime) {
			    if (!obsidianprotection()) switch (rn2(11)) {
			    case 0: diseasemu(mtmp->data);
				    break;
			    case 1: make_blinded(Blinded + tmp, TRUE);
				    break;
			    case 2: if (!Confusion)
					You("suddenly feel %s.",
					    FunnyHallu ? "trippy" : "confused");
				    make_confused(HConfusion + tmp, TRUE);
				    break;
			    case 3: make_stunned(HStun + tmp, TRUE);
				    break;
			    case 4: make_numbed(HNumbed + tmp, TRUE);
				    break;
			    case 5: make_frozen(HFrozen + tmp, TRUE);
				    break;
			    case 6: make_burned(HBurned + tmp, TRUE);
				    break;
			    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
				    break;
			    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
				    break;
			    case 9: make_feared(HFeared + tmp, TRUE);
				    break;
			    case 10: make_dimmed(HDimmed + tmp, TRUE);
				    break;
			    }

			} else u.berserktime = tmp;

		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		pline("Mysterious waves pulsate through your body!");
		if (rn2(3)) break;
		    struct obj *obj = some_armor(&youmonst);

		    if (obj && drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		    }
		break;

	    case AD_NGEN:
		pline("You hear the devils laugh!");
		if (rn2(3)) break;
		    struct obj *objDV = some_armor(&youmonst);

		    if (objDV && drain_item_severely(objDV)) {
			Your("%s less effective.", aobjnam(objDV, "seem"));
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		    }
		break;

	    case AD_SHRD:
		pline("You are cut by razor-sharp shards!");
		if (rn2(4)) break;

		struct obj *objX = some_armor(&youmonst);

		if (objX && drain_item(objX)) {
			Your("%s less effective.", aobjnam(objX, "seem"));
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		} else if (objX && rn2(3)) wither_dmg(objX, xname(objX), rn2(4), FALSE, &youmonst);

		break;

	    case AD_POLY:
		pline("A weird green light pierces you!");
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
		    polyself(FALSE);
		}
		break;
	    case AD_CHAO:
		pline("A chaotic light pierces you!");
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
		    polyself(FALSE);
		}
		if (rn2(4)) break;
		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: diseasemu(mtmp->data);
			    break;
		    case 1: make_blinded(Blinded + tmp, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + tmp, TRUE);
			    break;
		    case 3: make_stunned(HStun + tmp, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + tmp, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + tmp, TRUE);
			    break;
		    case 6: make_burned(HBurned + tmp, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + tmp, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + tmp, TRUE);
			    break;
		    }
		break;
	    case AD_TCKL:
		pline("A bunch of feelers suddenly reaches out for you!");
		if (multi >= 0 && !rn2(3)) {
		    if (Free_action && rn2(StrongFree_action ? 100 : 20))
			pline("They don't seem to do all that much, though...");
		    else {
			pline("Wahahahaha! Arrgh! It tickles!");
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(5), "tickled by a bunch of feelers", TRUE);
			exercise(A_DEX, FALSE);
			exercise(A_CON, FALSE);
		    }
		}
		break;

		case AD_PHYS:
		    if (mtmp->data == &mons[PM_FOG_CLOUD]) {
			You("are laden with moisture and %s",
			    flaming(youmonst.data) ? "are smoldering out!" :
			    Breathless ? "find it mildly uncomfortable." :
			    amphibious(youmonst.data) ? "feel comforted." :
			    "can barely breathe!");
			/* NB: Amphibious includes Breathless */
			if (Amphibious && !flaming(youmonst.data)) tmp = 0;
		    } else {
			You("are pummeled with debris!");
			exercise(A_STR, FALSE);
		    }
		    break;

	      case AD_CHKH:
			You("are pummeled with heavy debris!");
			if (mtmp->mcan) break;
			tmp += u.chokhmahdamage;
			tmp += rnd(u.ualign.sins > 0 ? (isqrt(u.ualign.sins) + 1) : (1));
			u.chokhmahdamage++;
		  break;

	      case AD_HODS:
			You("are attacked by a mirror-image of you...");
		 if(uwep){
			if (uwep->otyp == CORPSE
				&& touch_petrifies(&mons[uwep->corpsenm])) {
			    tmp = 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[uwep->corpsenm].mname);
			    if (!Stoned)
				goto do_stone2;
			}
			tmp += dmgval(uwep, &youmonst);
			
			if (uwep->opoisoned){
				sprintf(buf, "%s %s",
					s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
				poisoned(buf, A_CON, mtmp->data->mname, 30);
			}
			
			if (tmp <= 0) tmp = 1;
			if (!(uwep->oartifact &&
				artifact_hit(mtmp, &youmonst, uwep, &tmp,dieroll)))
			     hitmsg(mtmp, mattk);
			if (!tmp) break;
		 }
		break;

	    case AD_CHRN:
		pline("You are surrounded by a black glow.");
		if (rn2(4)) break;
		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: diseasemu(mtmp->data);
			    break;
		    case 1: make_blinded(Blinded + tmp, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + tmp, TRUE);
			    break;
		    case 3: make_stunned(HStun + tmp, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + tmp, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + tmp, TRUE);
			    break;
		    case 6: make_burned(HBurned + tmp, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + tmp, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + tmp, TRUE);
			    break;
		    }

		break;

		case AD_ACID:
		    if (Acid_resistance && rn2(StrongAcid_resistance ? 20 : 5)) {
			You("are covered with a seemingly harmless goo.");
			tmp = 0;
		    } else {
		      if (FunnyHallu) pline("Ouch!  You've been slimed!");
		      else You("are covered in slime!  It burns!");
		      exercise(A_STR, FALSE);
		    }

		if (Stoned) fix_petrification();

		    /* Mik: Go corrode a few things... */
			/*for (otmp2 = invent; otmp2; otmp2 = otmp2->nobj)
			    if (is_corrodeable(otmp2) && !rn2(9))
		    		(void) rust_dmg(otmp2, xname(otmp2), 3, FALSE, 
					&youmonst);*/
			if(!rn2(3)) erode_armor(&youmonst, TRUE);
		    break;
		case AD_BLND:
		    if (can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0)) {
			if(!Blind) {
			    You_cant("see in here!");
			    make_blinded((long)tmp,FALSE);
			    if (!Blind) Your("%s", vision_clears);
			} else
			    /* keep him blind until disgorged */
				/* Amy edit - but also add the actual damage */
			    make_blinded(Blinded+1+(long)tmp,FALSE);
		    }
		    tmp = 0;
		    break;
		case AD_ELEC:
		    if(!mtmp->mcan && rn2(2)) {
			pline_The("air around you crackles with electricity.");
			if (Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) {
				shieldeff(u.ux, u.uy);
				You("seem unhurt.");
				ugolemeffects(AD_ELEC,tmp);
				tmp = 0;
			}
		    } else tmp = 0;
		    break;

		case AD_MALK:
		    if(!mtmp->mcan && rn2(2)) {
			pline_The("air around you crackles with high voltage.");
			if (Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) {
				shieldeff(u.ux, u.uy);
				You("seem unhurt.");
				ugolemeffects(AD_ELEC,tmp);
				tmp = 0;
			}
		    } else tmp = 0;

		    setustuck(mtmp);
		    pline("%s grabs you!", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
		    if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		    break;

		case AD_ICEB:
		    if(!mtmp->mcan && rn2(2)) {

			pline("You are pummeled with blocks of ice!");
			if (issoviet) pline("KHAR KHAR KHAR!");
			if (Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) {
				pline("The ice doesn't seem to affect you.");
				tmp = 0;
			}
		    } else tmp = 0;

		    if (Race_if(PM_GAVIL)) tmp *= 2;
		    if (Race_if(PM_HYPOTHERMIC)) tmp *= 3;
		    make_frozen(HFrozen + tmp, TRUE);
		    if (isevilvariant || !rn2(issoviet ? 2 : Race_if(PM_GAVIL) ? 2 : Race_if(PM_HYPOTHERMIC) ? 2 : 10)) {
			destroy_item(POTION_CLASS, AD_COLD);
		    }

		break;

		case AD_AXUS:
		    pline("Snap! Crackle! Pop!");

		    if(!mtmp->mcan && rn2(2)) {
			if (Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly chilly.");
				ugolemeffects(AD_COLD,tmp);
				if (tmp >= 4) tmp -= (tmp / 4);
			} else You("are freezing to death!");
		    }

		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly hot.");
				ugolemeffects(AD_FIRE,tmp);
				if (tmp >= 4) tmp -= (tmp / 4);
			} else You("are burning to a crisp!");
			burn_away_slime();
		    }

		    if(!mtmp->mcan && rn2(2)) {
			pline_The("air around you crackles with electricity.");
			if (Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) {
				shieldeff(u.ux, u.uy);
				You("seem unhurt.");
				ugolemeffects(AD_ELEC,tmp);
				if (tmp >= 4) tmp -= (tmp / 4);
			}
		    }

			if (!rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) ) && (u.urmaxlvlUP >= 2) ) {
			    losexp("life drainage", FALSE, TRUE);
			}

		    break;

		case AD_COLD:
		    if(!mtmp->mcan && rn2(2)) {
			if (Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly chilly.");
				ugolemeffects(AD_COLD,tmp);
				tmp = 0;
			} else You("are freezing to death!");
		    } else tmp = 0;
		    break;
		case AD_DRIN:
			pline("It messes with your mind!");
		    if(!mtmp->mcan && !rn2(4)) {
		    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
			int lifesaved = 0;
			struct obj *wore_amulet = uamul;

			while(1) {
			    /* avoid looping on "die(y/n)?" */
			    if (lifesaved && (discover || wizard)) {
				if (wore_amulet && !uamul) {
				    /* used up AMULET_OF_LIFE_SAVING; still
				       subject to dying from brainlessness */
				    wore_amulet = 0;
				} else if (wizard) {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }
			    u.youaredead = 1;
			    u.youarereallydead = 1;

			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    if (wizard) {
				    u.youaredead = 0;
				    u.youarereallydead = 0;
			    }
			    lifesaved++;
			}
		    }

			if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
			else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE, TRUE);
			if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
			if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
			exercise(A_WIS, FALSE);
		    } else tmp = 0;
		    break;
		case AD_FIRE:
		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly hot.");
				ugolemeffects(AD_FIRE,tmp);
				tmp = 0;
			} else You("are burning to a crisp!");
			burn_away_slime();
		    } else tmp = 0;
		    break;
		case AD_DISE:
			if (rn2(3)) break; /* lower chance of actually being diseased --Amy */
		case AD_PEST:
		    if (!diseasemu(mtmp->data)) tmp = 0;
		    break;

		case AD_SPEL:
		case AD_CAST:
		case AD_CLRC:
			castmu(mtmp, mattk, TRUE, TRUE);
			break;

		case AD_VOMT:
			if (!rn2(StrongSick_resistance ? 100 : 10) || !Sick_resistance) {
				if (!Vomiting) {
					make_vomiting(Vomiting+d(10,4), TRUE);
					pline("You feel nauseated.");
					if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10);
				} else if (!rn2(2)) diseasemu(mtmp->data);
			}
			break;

		case AD_MAGM:
		    if(Antimagic && !Race_if(PM_KUTAR) && rn2(StrongAntimagic ? 5 : 3)) {
			tmp = 0;
		    } else {
			You("are irradiated with energy!");
		    }
		    break;
		case AD_DISN:
		    You_feel("like a drill is tearing you apart!");
		if (!rn2(10))  {
		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable || (Stoned_chiller && Stoned)) {
                pline("You are unharmed!");
                break;
		}

		 else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
		    break;
		} else if (uarmu) {
		    /* destroy shirt */
		    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
		    break;
		}

		if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "a drill died", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}
	    return 1; /* lifesaved */

	}
		    break;

		case AD_VAPO:
		    You_feel("like you are being vaporized!");

		if (!Disint_resistance) tmp *= 3;

		if (!rn2(10))  {
		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable || (Stoned_chiller && Stoned)) {
                pline("You are unharmed!");
                break;
		}

		 else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
		    break;
		} else if (uarmu) {
		    /* destroy shirt */
		    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
		    break;
		}

		if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "vaporization died", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}
	    return 1; /* lifesaved */

	}
		break;

		default:
		    /*tmp = 0;*/
			pline(rn2(2) ? "It hits you!" : "It hits!");
		    break; /* There is absolutely no reason why this wouldn't do any damage. --Amy */
	}

	/*if (randattackA == 1) {
		mattk->adtyp = AD_RBRE;
		randattackA = 0;
	}*/

	if (Half_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;
	if (StrongHalf_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;

	mdamageu(mtmp, tmp);
	if (tmp) stop_occupation();

	if (touch_petrifies(youmonst.data) && !resists_ston(mtmp)) {
	    pline("%s very hurriedly %s you!", Monnam(mtmp),
		  is_animal(mtmp->data)? "regurgitates" : "expels");
	    expels(mtmp, mtmp->data, FALSE);
	} else if (!u.uswldtim || (youmonst.data->msize >= MZ_HUGE && mtmp->data->msize < MZ_HUGE)) {
	    You("get %s!", is_animal(mtmp->data)? "regurgitated" : "expelled");
	    if (flags.verbose && (is_animal(mtmp->data) ||
		    (dmgtype(mtmp->data, AD_DGST) && Slow_digestion)))
		pline("Obviously %s doesn't like your taste.", mon_nam(mtmp));
	    expels(mtmp, mtmp->data, FALSE);
	}
	return(1);
}

STATIC_OVL int
explmu(mtmp, mattk, ufound)	/* monster explodes in your face */
register struct monst *mtmp;
register struct attack  *mattk;
boolean ufound;
{
	register struct engr *ep = engr_at(u.ux,u.uy);
	register struct permonst *mdat = mtmp->data;
	char	 buf[BUFSZ];
	struct obj *otmpi, *otmpii;
	struct obj *optr;
	int hallutime;

    if (mtmp->mcan) return(0);

    if (!ufound) {
	if (rn2(20)) return(0); /* make it a lot less likely for them to blow up in vain --Amy */
	pline("%s explodes at a spot in %s!",
	    canseemon(mtmp) ? Monnam(mtmp) : "It",
	    levl[mtmp->mux][mtmp->muy].typ == WATER
		? "empty water" : "thin air");
	}
    else {
	register int tmp = d((int)mattk->damn, (int)mattk->damd);
	if (MaximumDamageBug || u.uprops[MAXIMUM_DAMAGE_BUG].extrinsic || have_maximumdamagestone() || (uwep && uwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI)) tmp = (int)mattk->damn * (int)mattk->damd;
	register boolean not_affected = defends((int)mattk->adtyp, uwep);

	hitmsg(mtmp, mattk);

	int atttypC;
	/* Monsters with AD_RBRE can use any random explosion. --Amy */

	atttypC = mattk->adtyp;

	if ((SecretAttackBug || u.uprops[SECRET_ATTACK_BUG].extrinsic || have_secretattackstone()) && atttypC == AD_PHYS && !rn2(100)) {
		while (atttypC == AD_ENDS || atttypC == AD_RBRE || atttypC == AD_WERE || atttypC == AD_PHYS) {
			atttypC = randattack(); }

	}

	if ((UnfairAttackBug || u.uprops[UNFAIR_ATTACK_BUG].extrinsic || have_unfairattackstone()) && atttypC == AD_PHYS && !rn2(100)) {
		while (atttypC == AD_ENDS || atttypC == AD_RBRE || atttypC == AD_WERE || atttypC == AD_PHYS) {
			atttypC = rn2(AD_ENDS); }

	}

	if (atttypC == AD_RBRE) {
		while (atttypC == AD_ENDS || atttypC == AD_RBRE || atttypC == AD_WERE) {
			atttypC = randattack(); }
	}

	if (atttypC == AD_DAMA) {
		atttypC = randomdamageattack();
	}

	if (atttypC == AD_ILLU) {
		atttypC = randomillusionattack();
	}

	if (atttypC == AD_THIE) {
		atttypC = randomthievingattack();
	}

	if (atttypC == AD_RNG) {
		while (atttypC == AD_ENDS || atttypC == AD_RNG || atttypC == AD_WERE) {
			atttypC = rn2(AD_ENDS); }
	}

	if (atttypC == AD_PART) atttypC = u.adpartattack;

	if (atttypC == AD_MIDI) {
		atttypC = mtmp->m_id;
		if (atttypC < 0) atttypC *= -1;
		while (atttypC >= AD_ENDS) atttypC -= AD_ENDS;
		if (!(atttypC >= AD_PHYS && atttypC < AD_ENDS)) atttypC = AD_PHYS; /* fail safe --Amy */
		if (atttypC == AD_WERE) atttypC = AD_PHYS;
	}

	if (mtmp->data == &mons[PM_FLEECEY_LIGHT]) {
		if (!rn2(2)) {
			u.soviettemporary += rnz(5000);
			pline("KHAR KHAR TY NUB TEPER' TY OBLAZHALSYA KHEKHEKHE!");
		} else {
			u.evilvartemporary += rnz(5000);
			pline("Have fun with the evil variant, sucker.");
		}
	}

	if (mtmp->data == &mons[PM_CURSE_LIGHT]) {
		u.badfcursed += rnz(10000);
		You("start a trip on the road to nowhere.");
	}
	if (mtmp->data == &mons[PM_DOOM_LIGHT]) {
		u.badfcursed += rnz(20000);
		u.badfdoomed += rnz(20000);
		if (u.badfcursed < u.badfdoomed) u.badfcursed += rnz(20000);
		if (u.badfcursed < u.badfdoomed) u.badfcursed = (u.badfdoomed * 2);
		You("continue a trip on the road to nowhere...");
	}

	switch (atttypC) {
	    case AD_CONF:
		    if (mon_visible(mtmp) || (rnd(tmp /= 2) > u.ulevel)) {
			if (!FunnyHallu) You("are confused by a blast of light!");
			else pline("%s TO ORDER! %s MESSED!",urole.name.m,urole.name.m);
			make_confused(HConfusion + (long)tmp, FALSE); }
		break;

	    case AD_ACID:
		not_affected |= Acid_resistance;
		if (Stoned) fix_petrification();
		goto common;
	    case AD_COLD:
		not_affected |= Cold_resistance;
		goto common;
	    case AD_MAGM:
		not_affected |= Antimagic;
		goto common;
	    case AD_FIRE:
		burn_away_slime();
		not_affected |= Fire_resistance;
		goto common;
	    case AD_LAVA:
		hurtarmor(AD_LAVA);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		burn_away_slime();
		not_affected |= Fire_resistance;
		goto common;
	    case AD_PLAS:
		pline("You're seared by %s hot plasma radiation!", StrongFire_resistance ? "" : Fire_resistance ? "very" : "extremely");
		if (!Fire_resistance) tmp *= 2;
		if (StrongFire_resistance && tmp > 1) tmp /= 2;
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		burn_away_slime();
		make_stunned(HStun + tmp, TRUE);
		goto common;

	    case AD_MALK:
		setustuck(mtmp);
		pline("%s grabs you!", Monnam(mtmp));
		if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		if (isevilvariant || !rn2(issoviet ? 6 : 33))
			destroy_item(WAND_CLASS, AD_ELEC);
		if (isevilvariant || !rn2(issoviet ? 6 : 33))
			destroy_item(RING_CLASS, AD_ELEC);
		if (isevilvariant || !rn2(issoviet ? 30 : 165))
			destroy_item(AMULET_CLASS, AD_ELEC);
		not_affected |= Shock_resistance;
		goto common;

	    case AD_ICEB:
		if (issoviet) pline("KHA KHA KHA KHA KHA KHA KHA.");
		not_affected |= Cold_resistance;
		if (Race_if(PM_GAVIL)) tmp *= 2;
		if (Race_if(PM_HYPOTHERMIC)) tmp *= 3;
		make_frozen(HFrozen + tmp, TRUE);
		if (isevilvariant || !rn2(issoviet ? 2 : Race_if(PM_GAVIL) ? 2 : Race_if(PM_HYPOTHERMIC) ? 2 : 10)) {
			destroy_item(POTION_CLASS, AD_COLD);
		}
		goto common;

	    case AD_AXUS:
		if ((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && (u.urmaxlvlUP >= 2) )
		    losexp("draining explosion", FALSE, TRUE);

		goto common;

	    case AD_STCK:
	    case AD_WRAP:
	    case AD_WERE:
	    case AD_SGLD:
	    case AD_SITM:
	    case AD_SEDU:
	    case AD_SSEX:
	    case AD_THIR:

		goto common;

	    case AD_SAMU:
		stealamulet(mtmp);
		goto common;

	    case AD_NTHR:

		if ((!Drain_resistance || !rn2(StrongDrain_resistance ? 20 : 5)) && u.uexp > 100) {
			u.uexp -= (u.uexp / 100);
			You_feel("your life slipping away!");
			if (u.uexp < newuexp(u.ulevel - 1)) {
			      losexp("nether forces", TRUE, FALSE);
			}
		}
		goto common;

	    case AD_ELEC:
		not_affected |= Shock_resistance;
	    case AD_PHYS: /* just do some plain physical damage, for golem's selfdestruct attack et. al. --Amy */
common:

		if (atttypC == AD_PHYS) not_affected = FALSE;

		if (!not_affected || !rn2(10) ) {
		    if (ACURR(A_DEX) > rnd(200)) {
			You("duck some of the blast.");
			tmp = (tmp+1) / 2;
		    } else {
		        if (flags.verbose) You("get blasted!");
		    }
		    if (mattk->adtyp == AD_FIRE) burn_away_slime();
		    if (Half_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;
		    if (StrongHalf_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;
		    mdamageu(mtmp, tmp);
		}
		break;

	    case AD_SLEE:
		if (multi >= 0) {
			if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) break;
			fall_asleep(-rnd(tmp), TRUE);
			pline("The explosion puts you into a deep sleep!");
		}
		break;

	    case AD_DREA:

		if (multi < 0) {
			tmp *= 4;
			pline("Your dream is eaten!");
		}
		mdamageu(mtmp, tmp);
		break;

	    case AD_AGGR:

		incr_itimeout(&HAggravate_monster, tmp);
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();

		{

			int aggroamount = rnd(6);
			if (!rn2(2)) aggroamount += rnd(25);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		break;

	    case AD_DATA:

		mdamageu(mtmp, tmp);
		u.datadeletedefer = 1;
		datadeleteattack();
		break;

	    case AD_RAGN:

		mdamageu(mtmp, tmp);
		ragnarok(TRUE);
		if (evilfriday && mtmp->m_lev > 1) evilragnarok(TRUE,mtmp->m_lev);
		break;

	    case AD_DEST:

		mdamageu(mtmp, tmp);
		destroyarmorattack();
		break;

	    case AD_MINA:

		{
			register int midentity = mtmp->m_id;
			if (midentity < 0) midentity *= -1;
			while (midentity > 235) midentity -= 235;

			register int nastyduration = ((tmp + 2) * rnd(10));
			if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) nastyduration *= 20;

			switch (midentity) {

				case 1: RMBLoss += rnz(nastyduration); break;
				case 2: NoDropProblem += rnz(nastyduration); break;
				case 3: DSTWProblem += rnz(nastyduration); break;
				case 4: StatusTrapProblem += rnz(nastyduration); 
					if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
					if (HStun) set_itimeout(&HeavyStunned, HStun);
					if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
					if (HFeared) set_itimeout(&HeavyFeared, HFeared);
					if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
					if (HBurned) set_itimeout(&HeavyBurned, HBurned);
					if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
					if (Blinded) set_itimeout(&HeavyBlind, Blinded);
					if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
					break;
				case 5: Superscroller += rnz(nastyduration * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz(nastyduration); break;
				case 7: FreeHandLoss += rnz(nastyduration); break;
				case 8: Unidentify += rnz(nastyduration); break;
				case 9: Thirst += rnz(nastyduration); break;
				case 10: LuckLoss += rnz(nastyduration); break;
				case 11: ShadesOfGrey += rnz(nastyduration); break;
				case 12: FaintActive += rnz(nastyduration); break;
				case 13: Itemcursing += rnz(nastyduration); break;
				case 14: DifficultyIncreased += rnz(nastyduration); break;
				case 15: Deafness += rnz(nastyduration); flags.soundok = 0; break;
				case 16: CasterProblem += rnz(nastyduration); break;
				case 17: WeaknessProblem += rnz(nastyduration); break;
				case 18: RotThirteen += rnz(nastyduration); break;
				case 19: BishopGridbug += rnz(nastyduration); break;
				case 20: UninformationProblem += rnz(nastyduration); break;
				case 21: StairsProblem += rnz(nastyduration); break;
				case 22: AlignmentProblem += rnz(nastyduration); break;
				case 23: ConfusionProblem += rnz(nastyduration); break;
				case 24: SpeedBug += rnz(nastyduration); break;
				case 25: DisplayLoss += rnz(nastyduration); break;
				case 26: SpellLoss += rnz(nastyduration); break;
				case 27: YellowSpells += rnz(nastyduration); break;
				case 28: AutoDestruct += rnz(nastyduration); break;
				case 29: MemoryLoss += rnz(nastyduration); break;
				case 30: InventoryLoss += rnz(nastyduration); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (tmp * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz(nastyduration); break;
				case 33: BloodLossProblem += rnz(nastyduration); break;
				case 34: BadEffectProblem += rnz(nastyduration); break;
				case 35: TrapCreationProblem += rnz(nastyduration); break;
				case 36: AutomaticVulnerabilitiy += rnz(nastyduration); break;
				case 37: TeleportingItems += rnz(nastyduration); break;
				case 38: NastinessProblem += rnz(nastyduration); break;
				case 39: CaptchaProblem += rnz(nastyduration); break;
				case 40: FarlookProblem += rnz(nastyduration); break;
				case 41: RespawnProblem += rnz(nastyduration); break;
				case 42: RecurringAmnesia += rnz(nastyduration); break;
				case 43: BigscriptEffect += rnz(nastyduration); break;
				case 44: {
					BankTrapEffect += rnz(nastyduration);
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz(nastyduration); break;
				case 46: TechTrapEffect += rnz(nastyduration); break;
				case 47: RecurringDisenchant += rnz(nastyduration); break;
				case 48: verisiertEffect += rnz(nastyduration); break;
				case 49: ChaosTerrain += rnz(nastyduration); break;
				case 50: Muteness += rnz(nastyduration); break;
				case 51: EngravingDoesntWork += rnz(nastyduration); break;
				case 52: MagicDeviceEffect += rnz(nastyduration); break;
				case 53: BookTrapEffect += rnz(nastyduration); break;
				case 54: LevelTrapEffect += rnz(nastyduration); break;
				case 55: QuizTrapEffect += rnz(nastyduration); break;
				case 56: FastMetabolismEffect += rnz(nastyduration); break;
				case 57: NoReturnEffect += rnz(nastyduration); break;
				case 58: AlwaysEgotypeMonsters += rnz(nastyduration); break;
				case 59: TimeGoesByFaster += rnz(nastyduration); break;
				case 60: FoodIsAlwaysRotten += rnz(nastyduration); break;
				case 61: AllSkillsUnskilled += rnz(nastyduration); break;
				case 62: AllStatsAreLower += rnz(nastyduration); break;
				case 63: PlayerCannotTrainSkills += rnz(nastyduration); break;
				case 64: PlayerCannotExerciseStats += rnz(nastyduration); break;
				case 65: TurnLimitation += rnz(nastyduration); break;
				case 66: WeakSight += rnz(nastyduration); break;
				case 67: RandomMessages += rnz(nastyduration); break;

				case 68: Desecration += rnz(nastyduration); break;
				case 69: StarvationEffect += rnz(nastyduration); break;
				case 70: NoDropsEffect += rnz(nastyduration); break;
				case 71: LowEffects += rnz(nastyduration); break;
				case 72: InvisibleTrapsEffect += rnz(nastyduration); break;
				case 73: GhostWorld += rnz(nastyduration); break;
				case 74: Dehydration += rnz(nastyduration); break;
				case 75: HateTrapEffect += rnz(nastyduration); break;
				case 76: TotterTrapEffect += rnz(nastyduration); break;
				case 77: Nonintrinsics += rnz(nastyduration); break;
				case 78: Dropcurses += rnz(nastyduration); break;
				case 79: Nakedness += rnz(nastyduration); break;
				case 80: Antileveling += rnz(nastyduration); break;
				case 81: ItemStealingEffect += rnz(nastyduration); break;
				case 82: Rebellions += rnz(nastyduration); break;
				case 83: CrapEffect += rnz(nastyduration); break;
				case 84: ProjectilesMisfire += rnz(nastyduration); break;
				case 85: WallTrapping += rnz(nastyduration); break;
				case 86: DisconnectedStairs += rnz(nastyduration); break;
				case 87: InterfaceScrewed += rnz(nastyduration); break;
				case 88: Bossfights += rnz(nastyduration); break;
				case 89: EntireLevelMode += rnz(nastyduration); break;
				case 90: BonesLevelChange += rnz(nastyduration); break;
				case 91: AutocursingEquipment += rnz(nastyduration); break;
				case 92: HighlevelStatus += rnz(nastyduration); break;
				case 93: SpellForgetting += rnz(nastyduration); break;
				case 94: SoundEffectBug += rnz(nastyduration); break;
				case 95: TimerunBug += rnz(nastyduration); break;
				case 96: LootcutBug += rnz(nastyduration); break;
				case 97: MonsterSpeedBug += rnz(nastyduration); break;
				case 98: ScalingBug += rnz(nastyduration); break;
				case 99: EnmityBug += rnz(nastyduration); break;
				case 100: WhiteSpells += rnz(nastyduration); break;
				case 101: CompleteGraySpells += rnz(nastyduration); break;
				case 102: QuasarVision += rnz(nastyduration); break;
				case 103: MommaBugEffect += rnz(nastyduration); break;
				case 104: HorrorBugEffect += rnz(nastyduration); break;
				case 105: ArtificerBug += rnz(nastyduration); break;
				case 106: WereformBug += rnz(nastyduration); break;
				case 107: NonprayerBug += rnz(nastyduration); break;
				case 108: EvilPatchEffect += rnz(nastyduration); break;
				case 109: HardModeEffect += rnz(nastyduration); break;
				case 110: SecretAttackBug += rnz(nastyduration); break;
				case 111: EaterBugEffect += rnz(nastyduration); break;
				case 112: CovetousnessBug += rnz(nastyduration); break;
				case 113: NotSeenBug += rnz(nastyduration); break;
				case 114: DarkModeBug += rnz(nastyduration); break;
				case 115: AntisearchEffect += rnz(nastyduration); break;
				case 116: HomicideEffect += rnz(nastyduration); break;
				case 117: NastynationBug += rnz(nastyduration); break;
				case 118: WakeupCallBug += rnz(nastyduration); break;
				case 119: GrayoutBug += rnz(nastyduration); break;
				case 120: GrayCenterBug += rnz(nastyduration); break;
				case 121: CheckerboardBug += rnz(nastyduration); break;
				case 122: ClockwiseSpinBug += rnz(nastyduration); break;
				case 123: CounterclockwiseSpin += rnz(nastyduration); break;
				case 124: LagBugEffect += rnz(nastyduration); break;
				case 125: BlesscurseEffect += rnz(nastyduration); break;
				case 126: DeLightBug += rnz(nastyduration); break;
				case 127: DischargeBug += rnz(nastyduration); break;
				case 128: TrashingBugEffect += rnz(nastyduration); break;
				case 129: FilteringBug += rnz(nastyduration); break;
				case 130: DeformattingBug += rnz(nastyduration); break;
				case 131: FlickerStripBug += rnz(nastyduration); break;
				case 132: UndressingEffect += rnz(nastyduration); break;
				case 133: Hyperbluewalls += rnz(nastyduration); break;
				case 134: NoliteBug += rnz(nastyduration); break;
				case 135: ParanoiaBugEffect += rnz(nastyduration); break;
				case 136: FleecescriptBug += rnz(nastyduration); break;
				case 137: InterruptEffect += rnz(nastyduration); break;
				case 138: DustbinBug += rnz(nastyduration); break;
				case 139: ManaBatteryBug += rnz(nastyduration); break;
				case 140: Monsterfingers += rnz(nastyduration); break;
				case 141: MiscastBug += rnz(nastyduration); break;
				case 142: MessageSuppression += rnz(nastyduration); break;
				case 143: StuckAnnouncement += rnz(nastyduration); break;
				case 144: BloodthirstyEffect += rnz(nastyduration); break;
				case 145: MaximumDamageBug += rnz(nastyduration); break;
				case 146: LatencyBugEffect += rnz(nastyduration); break;
				case 147: StarlitBug += rnz(nastyduration); break;
				case 148: KnowledgeBug += rnz(nastyduration); break;
				case 149: HighscoreBug += rnz(nastyduration); break;
				case 150: PinkSpells += rnz(nastyduration); break;
				case 151: GreenSpells += rnz(nastyduration); break;
				case 152: EvencoreEffect += rnz(nastyduration); break;
				case 153: UnderlayerBug += rnz(nastyduration); break;
				case 154: DamageMeterBug += rnz(nastyduration); break;
				case 155: ArbitraryWeightBug += rnz(nastyduration); break;
				case 156: FuckedInfoBug += rnz(nastyduration); break;
				case 157: BlackSpells += rnz(nastyduration); break;
				case 158: CyanSpells += rnz(nastyduration); break;
				case 159: HeapEffectBug += rnz(nastyduration); break;
				case 160: BlueSpells += rnz(nastyduration); break;
				case 161: TronEffect += rnz(nastyduration); break;
				case 162: RedSpells += rnz(nastyduration); break;
				case 163: TooHeavyEffect += rnz(nastyduration); break;
				case 164: ElongationBug += rnz(nastyduration); break;
				case 165: WrapoverEffect += rnz(nastyduration); break;
				case 166: DestructionEffect += rnz(nastyduration); break;
				case 167: MeleePrefixBug += rnz(nastyduration); break;
				case 168: AutomoreBug += rnz(nastyduration); break;
				case 169: UnfairAttackBug += rnz(nastyduration); break;
				case 170: OrangeSpells += rnz(nastyduration); break;
				case 171: VioletSpells += rnz(nastyduration); break;
				case 172: LongingEffect += rnz(nastyduration); break;
				case 173: CursedParts += rnz(nastyduration); break;
				case 174: Quaversal += rnz(nastyduration); break;
				case 175: AppearanceShuffling += rnz(nastyduration); break;
				case 176: BrownSpells += rnz(nastyduration); break;
				case 177: Choicelessness += rnz(nastyduration); break;
				case 178: Goldspells += rnz(nastyduration); break;
				case 179: Deprovement += rnz(nastyduration); break;
				case 180: InitializationFail += rnz(nastyduration); break;
				case 181: GushlushEffect += rnz(nastyduration); break;
				case 182: SoiltypeEffect += rnz(nastyduration); break;
				case 183: DangerousTerrains += rnz(nastyduration); break;
				case 184: FalloutEffect += rnz(nastyduration); break;
				case 185: MojibakeEffect += rnz(nastyduration); break;
				case 186: GravationEffect += rnz(nastyduration); break;
				case 187: UncalledEffect += rnz(nastyduration); break;
				case 188: ExplodingDiceEffect += rnz(nastyduration); break;
				case 189: PermacurseEffect += rnz(nastyduration); break;
				case 190: ShroudedIdentity += rnz(nastyduration); break;
				case 191: FeelerGauges += rnz(nastyduration); break;
				case 192: LongScrewup += rnz(nastyduration * 20); break;
				case 193: WingYellowChange += rnz(nastyduration); break;
				case 194: LifeSavingBug += rnz(nastyduration); break;
				case 195: CurseuseEffect += rnz(nastyduration); break;
				case 196: CutNutritionEffect += rnz(nastyduration); break;
				case 197: SkillLossEffect += rnz(nastyduration); break;
				case 198: AutopilotEffect += rnz(nastyduration); break;
				case 199: MysteriousForceActive += rnz(nastyduration); break;
				case 200: MonsterGlyphChange += rnz(nastyduration); break;
				case 201: ChangingDirectives += rnz(nastyduration); break;
				case 202: ContainerKaboom += rnz(nastyduration); break;
				case 203: StealDegrading += rnz(nastyduration); break;
				case 204: LeftInventoryBug += rnz(nastyduration); break;
				case 205: FluctuatingSpeed += rnz(nastyduration); break;
				case 206: TarmuStrokingNora += rnz(nastyduration); break;
				case 207: FailureEffects += rnz(nastyduration); break;
				case 208: BrightCyanSpells += rnz(nastyduration); break;
				case 209: FrequentationSpawns += rnz(nastyduration); break;
				case 210: PetAIScrewed += rnz(nastyduration); break;
				case 211: SatanEffect += rnz(nastyduration); break;
				case 212: RememberanceEffect += rnz(nastyduration); break;
				case 213: PokelieEffect += rnz(nastyduration); break;
				case 214: AlwaysAutopickup += rnz(nastyduration); break;
				case 215: DywypiProblem += rnz(nastyduration); break;
				case 216: SilverSpells += rnz(nastyduration); break;
				case 217: MetalSpells += rnz(nastyduration); break;
				case 218: PlatinumSpells += rnz(nastyduration); break;
				case 219: ManlerEffect += rnz(nastyduration); break;
				case 220: DoorningEffect += rnz(nastyduration); break;
				case 221: NownsibleEffect += rnz(nastyduration); break;
				case 222: ElmStreetEffect += rnz(nastyduration); break;
				case 223: MonnoiseEffect += rnz(nastyduration); break;
				case 224: RangCallEffect += rnz(nastyduration); break;
				case 225: RecurringSpellLoss += rnz(nastyduration); break;
				case 226: AntitrainingEffect += rnz(nastyduration); break;
				case 227: TechoutBug += rnz(nastyduration); break;
				case 228: StatDecay += rnz(nastyduration); break;
				case 229: Movemork += rnz(nastyduration); break;
				case 230: BadPartBug += rnz(nastyduration); break;
				case 231: CompletelyBadPartBug += rnz(nastyduration); break;
				case 232: EvilVariantActive += rnz(nastyduration); break;
				case 233: SanityTrebleEffect += rnz(nastyduration); break;
				case 234: StatDecreaseBug += rnz(nastyduration); break;
				case 235: SimeoutBug += rnz(nastyduration); break;

				default: impossible("AD_MINA called with invalid value %d", midentity); break;
			}

		}

		break;

	    case AD_RUNS:

		{
			register int nastyduration = ((tmp + 2) * rnd(10));
			if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) nastyduration *= 20;

			switch (u.adrunsattack) {

				case 1: RMBLoss += rnz(nastyduration); break;
				case 2: NoDropProblem += rnz(nastyduration); break;
				case 3: DSTWProblem += rnz(nastyduration); break;
				case 4: StatusTrapProblem += rnz(nastyduration); 
					if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
					if (HStun) set_itimeout(&HeavyStunned, HStun);
					if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
					if (HFeared) set_itimeout(&HeavyFeared, HFeared);
					if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
					if (HBurned) set_itimeout(&HeavyBurned, HBurned);
					if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
					if (Blinded) set_itimeout(&HeavyBlind, Blinded);
					if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
					break;
				case 5: Superscroller += rnz(nastyduration * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz(nastyduration); break;
				case 7: FreeHandLoss += rnz(nastyduration); break;
				case 8: Unidentify += rnz(nastyduration); break;
				case 9: Thirst += rnz(nastyduration); break;
				case 10: LuckLoss += rnz(nastyduration); break;
				case 11: ShadesOfGrey += rnz(nastyduration); break;
				case 12: FaintActive += rnz(nastyduration); break;
				case 13: Itemcursing += rnz(nastyduration); break;
				case 14: DifficultyIncreased += rnz(nastyduration); break;
				case 15: Deafness += rnz(nastyduration); flags.soundok = 0; break;
				case 16: CasterProblem += rnz(nastyduration); break;
				case 17: WeaknessProblem += rnz(nastyduration); break;
				case 18: RotThirteen += rnz(nastyduration); break;
				case 19: BishopGridbug += rnz(nastyduration); break;
				case 20: UninformationProblem += rnz(nastyduration); break;
				case 21: StairsProblem += rnz(nastyduration); break;
				case 22: AlignmentProblem += rnz(nastyduration); break;
				case 23: ConfusionProblem += rnz(nastyduration); break;
				case 24: SpeedBug += rnz(nastyduration); break;
				case 25: DisplayLoss += rnz(nastyduration); break;
				case 26: SpellLoss += rnz(nastyduration); break;
				case 27: YellowSpells += rnz(nastyduration); break;
				case 28: AutoDestruct += rnz(nastyduration); break;
				case 29: MemoryLoss += rnz(nastyduration); break;
				case 30: InventoryLoss += rnz(nastyduration); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (tmp * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz(nastyduration); break;
				case 33: BloodLossProblem += rnz(nastyduration); break;
				case 34: BadEffectProblem += rnz(nastyduration); break;
				case 35: TrapCreationProblem += rnz(nastyduration); break;
				case 36: AutomaticVulnerabilitiy += rnz(nastyduration); break;
				case 37: TeleportingItems += rnz(nastyduration); break;
				case 38: NastinessProblem += rnz(nastyduration); break;
				case 39: CaptchaProblem += rnz(nastyduration); break;
				case 40: FarlookProblem += rnz(nastyduration); break;
				case 41: RespawnProblem += rnz(nastyduration); break;
				case 42: RecurringAmnesia += rnz(nastyduration); break;
				case 43: BigscriptEffect += rnz(nastyduration); break;
				case 44: {
					BankTrapEffect += rnz(nastyduration);
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz(nastyduration); break;
				case 46: TechTrapEffect += rnz(nastyduration); break;
				case 47: RecurringDisenchant += rnz(nastyduration); break;
				case 48: verisiertEffect += rnz(nastyduration); break;
				case 49: ChaosTerrain += rnz(nastyduration); break;
				case 50: Muteness += rnz(nastyduration); break;
				case 51: EngravingDoesntWork += rnz(nastyduration); break;
				case 52: MagicDeviceEffect += rnz(nastyduration); break;
				case 53: BookTrapEffect += rnz(nastyduration); break;
				case 54: LevelTrapEffect += rnz(nastyduration); break;
				case 55: QuizTrapEffect += rnz(nastyduration); break;
				case 56: FastMetabolismEffect += rnz(nastyduration); break;
				case 57: NoReturnEffect += rnz(nastyduration); break;
				case 58: AlwaysEgotypeMonsters += rnz(nastyduration); break;
				case 59: TimeGoesByFaster += rnz(nastyduration); break;
				case 60: FoodIsAlwaysRotten += rnz(nastyduration); break;
				case 61: AllSkillsUnskilled += rnz(nastyduration); break;
				case 62: AllStatsAreLower += rnz(nastyduration); break;
				case 63: PlayerCannotTrainSkills += rnz(nastyduration); break;
				case 64: PlayerCannotExerciseStats += rnz(nastyduration); break;
				case 65: TurnLimitation += rnz(nastyduration); break;
				case 66: WeakSight += rnz(nastyduration); break;
				case 67: RandomMessages += rnz(nastyduration); break;

				case 68: Desecration += rnz(nastyduration); break;
				case 69: StarvationEffect += rnz(nastyduration); break;
				case 70: NoDropsEffect += rnz(nastyduration); break;
				case 71: LowEffects += rnz(nastyduration); break;
				case 72: InvisibleTrapsEffect += rnz(nastyduration); break;
				case 73: GhostWorld += rnz(nastyduration); break;
				case 74: Dehydration += rnz(nastyduration); break;
				case 75: HateTrapEffect += rnz(nastyduration); break;
				case 76: TotterTrapEffect += rnz(nastyduration); break;
				case 77: Nonintrinsics += rnz(nastyduration); break;
				case 78: Dropcurses += rnz(nastyduration); break;
				case 79: Nakedness += rnz(nastyduration); break;
				case 80: Antileveling += rnz(nastyduration); break;
				case 81: ItemStealingEffect += rnz(nastyduration); break;
				case 82: Rebellions += rnz(nastyduration); break;
				case 83: CrapEffect += rnz(nastyduration); break;
				case 84: ProjectilesMisfire += rnz(nastyduration); break;
				case 85: WallTrapping += rnz(nastyduration); break;
				case 86: DisconnectedStairs += rnz(nastyduration); break;
				case 87: InterfaceScrewed += rnz(nastyduration); break;
				case 88: Bossfights += rnz(nastyduration); break;
				case 89: EntireLevelMode += rnz(nastyduration); break;
				case 90: BonesLevelChange += rnz(nastyduration); break;
				case 91: AutocursingEquipment += rnz(nastyduration); break;
				case 92: HighlevelStatus += rnz(nastyduration); break;
				case 93: SpellForgetting += rnz(nastyduration); break;
				case 94: SoundEffectBug += rnz(nastyduration); break;
				case 95: TimerunBug += rnz(nastyduration); break;
				case 96: LootcutBug += rnz(nastyduration); break;
				case 97: MonsterSpeedBug += rnz(nastyduration); break;
				case 98: ScalingBug += rnz(nastyduration); break;
				case 99: EnmityBug += rnz(nastyduration); break;
				case 100: WhiteSpells += rnz(nastyduration); break;
				case 101: CompleteGraySpells += rnz(nastyduration); break;
				case 102: QuasarVision += rnz(nastyduration); break;
				case 103: MommaBugEffect += rnz(nastyduration); break;
				case 104: HorrorBugEffect += rnz(nastyduration); break;
				case 105: ArtificerBug += rnz(nastyduration); break;
				case 106: WereformBug += rnz(nastyduration); break;
				case 107: NonprayerBug += rnz(nastyduration); break;
				case 108: EvilPatchEffect += rnz(nastyduration); break;
				case 109: HardModeEffect += rnz(nastyduration); break;
				case 110: SecretAttackBug += rnz(nastyduration); break;
				case 111: EaterBugEffect += rnz(nastyduration); break;
				case 112: CovetousnessBug += rnz(nastyduration); break;
				case 113: NotSeenBug += rnz(nastyduration); break;
				case 114: DarkModeBug += rnz(nastyduration); break;
				case 115: AntisearchEffect += rnz(nastyduration); break;
				case 116: HomicideEffect += rnz(nastyduration); break;
				case 117: NastynationBug += rnz(nastyduration); break;
				case 118: WakeupCallBug += rnz(nastyduration); break;
				case 119: GrayoutBug += rnz(nastyduration); break;
				case 120: GrayCenterBug += rnz(nastyduration); break;
				case 121: CheckerboardBug += rnz(nastyduration); break;
				case 122: ClockwiseSpinBug += rnz(nastyduration); break;
				case 123: CounterclockwiseSpin += rnz(nastyduration); break;
				case 124: LagBugEffect += rnz(nastyduration); break;
				case 125: BlesscurseEffect += rnz(nastyduration); break;
				case 126: DeLightBug += rnz(nastyduration); break;
				case 127: DischargeBug += rnz(nastyduration); break;
				case 128: TrashingBugEffect += rnz(nastyduration); break;
				case 129: FilteringBug += rnz(nastyduration); break;
				case 130: DeformattingBug += rnz(nastyduration); break;
				case 131: FlickerStripBug += rnz(nastyduration); break;
				case 132: UndressingEffect += rnz(nastyduration); break;
				case 133: Hyperbluewalls += rnz(nastyduration); break;
				case 134: NoliteBug += rnz(nastyduration); break;
				case 135: ParanoiaBugEffect += rnz(nastyduration); break;
				case 136: FleecescriptBug += rnz(nastyduration); break;
				case 137: InterruptEffect += rnz(nastyduration); break;
				case 138: DustbinBug += rnz(nastyduration); break;
				case 139: ManaBatteryBug += rnz(nastyduration); break;
				case 140: Monsterfingers += rnz(nastyduration); break;
				case 141: MiscastBug += rnz(nastyduration); break;
				case 142: MessageSuppression += rnz(nastyduration); break;
				case 143: StuckAnnouncement += rnz(nastyduration); break;
				case 144: BloodthirstyEffect += rnz(nastyduration); break;
				case 145: MaximumDamageBug += rnz(nastyduration); break;
				case 146: LatencyBugEffect += rnz(nastyduration); break;
				case 147: StarlitBug += rnz(nastyduration); break;
				case 148: KnowledgeBug += rnz(nastyduration); break;
				case 149: HighscoreBug += rnz(nastyduration); break;
				case 150: PinkSpells += rnz(nastyduration); break;
				case 151: GreenSpells += rnz(nastyduration); break;
				case 152: EvencoreEffect += rnz(nastyduration); break;
				case 153: UnderlayerBug += rnz(nastyduration); break;
				case 154: DamageMeterBug += rnz(nastyduration); break;
				case 155: ArbitraryWeightBug += rnz(nastyduration); break;
				case 156: FuckedInfoBug += rnz(nastyduration); break;
				case 157: BlackSpells += rnz(nastyduration); break;
				case 158: CyanSpells += rnz(nastyduration); break;
				case 159: HeapEffectBug += rnz(nastyduration); break;
				case 160: BlueSpells += rnz(nastyduration); break;
				case 161: TronEffect += rnz(nastyduration); break;
				case 162: RedSpells += rnz(nastyduration); break;
				case 163: TooHeavyEffect += rnz(nastyduration); break;
				case 164: ElongationBug += rnz(nastyduration); break;
				case 165: WrapoverEffect += rnz(nastyduration); break;
				case 166: DestructionEffect += rnz(nastyduration); break;
				case 167: MeleePrefixBug += rnz(nastyduration); break;
				case 168: AutomoreBug += rnz(nastyduration); break;
				case 169: UnfairAttackBug += rnz(nastyduration); break;
				case 170: OrangeSpells += rnz(nastyduration); break;
				case 171: VioletSpells += rnz(nastyduration); break;
				case 172: LongingEffect += rnz(nastyduration); break;
				case 173: CursedParts += rnz(nastyduration); break;
				case 174: Quaversal += rnz(nastyduration); break;
				case 175: AppearanceShuffling += rnz(nastyduration); break;
				case 176: BrownSpells += rnz(nastyduration); break;
				case 177: Choicelessness += rnz(nastyduration); break;
				case 178: Goldspells += rnz(nastyduration); break;
				case 179: Deprovement += rnz(nastyduration); break;
				case 180: InitializationFail += rnz(nastyduration); break;
				case 181: GushlushEffect += rnz(nastyduration); break;
				case 182: SoiltypeEffect += rnz(nastyduration); break;
				case 183: DangerousTerrains += rnz(nastyduration); break;
				case 184: FalloutEffect += rnz(nastyduration); break;
				case 185: MojibakeEffect += rnz(nastyduration); break;
				case 186: GravationEffect += rnz(nastyduration); break;
				case 187: UncalledEffect += rnz(nastyduration); break;
				case 188: ExplodingDiceEffect += rnz(nastyduration); break;
				case 189: PermacurseEffect += rnz(nastyduration); break;
				case 190: ShroudedIdentity += rnz(nastyduration); break;
				case 191: FeelerGauges += rnz(nastyduration); break;
				case 192: LongScrewup += rnz(nastyduration * 20); break;
				case 193: WingYellowChange += rnz(nastyduration); break;
				case 194: LifeSavingBug += rnz(nastyduration); break;
				case 195: CurseuseEffect += rnz(nastyduration); break;
				case 196: CutNutritionEffect += rnz(nastyduration); break;
				case 197: SkillLossEffect += rnz(nastyduration); break;
				case 198: AutopilotEffect += rnz(nastyduration); break;
				case 199: MysteriousForceActive += rnz(nastyduration); break;
				case 200: MonsterGlyphChange += rnz(nastyduration); break;
				case 201: ChangingDirectives += rnz(nastyduration); break;
				case 202: ContainerKaboom += rnz(nastyduration); break;
				case 203: StealDegrading += rnz(nastyduration); break;
				case 204: LeftInventoryBug += rnz(nastyduration); break;
				case 205: FluctuatingSpeed += rnz(nastyduration); break;
				case 206: TarmuStrokingNora += rnz(nastyduration); break;
				case 207: FailureEffects += rnz(nastyduration); break;
				case 208: BrightCyanSpells += rnz(nastyduration); break;
				case 209: FrequentationSpawns += rnz(nastyduration); break;
				case 210: PetAIScrewed += rnz(nastyduration); break;
				case 211: SatanEffect += rnz(nastyduration); break;
				case 212: RememberanceEffect += rnz(nastyduration); break;
				case 213: PokelieEffect += rnz(nastyduration); break;
				case 214: AlwaysAutopickup += rnz(nastyduration); break;
				case 215: DywypiProblem += rnz(nastyduration); break;
				case 216: SilverSpells += rnz(nastyduration); break;
				case 217: MetalSpells += rnz(nastyduration); break;
				case 218: PlatinumSpells += rnz(nastyduration); break;
				case 219: ManlerEffect += rnz(nastyduration); break;
				case 220: DoorningEffect += rnz(nastyduration); break;
				case 221: NownsibleEffect += rnz(nastyduration); break;
				case 222: ElmStreetEffect += rnz(nastyduration); break;
				case 223: MonnoiseEffect += rnz(nastyduration); break;
				case 224: RangCallEffect += rnz(nastyduration); break;
				case 225: RecurringSpellLoss += rnz(nastyduration); break;
				case 226: AntitrainingEffect += rnz(nastyduration); break;
				case 227: TechoutBug += rnz(nastyduration); break;
				case 228: StatDecay += rnz(nastyduration); break;
				case 229: Movemork += rnz(nastyduration); break;
				case 230: BadPartBug += rnz(nastyduration); break;
				case 231: CompletelyBadPartBug += rnz(nastyduration); break;
				case 232: EvilVariantActive += rnz(nastyduration); break;
				case 233: SanityTrebleEffect += rnz(nastyduration); break;
				case 234: StatDecreaseBug += rnz(nastyduration); break;
				case 235: SimeoutBug += rnz(nastyduration); break;

				default: impossible("AD_RUNS called with invalid value %d", u.adrunsattack); break;
			}

		}

		break;

	    case AD_SIN:

		You_feel("sinful!");
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-5);

		break;

	    case AD_ALIN:

		if (tmp) {
			You_feel("less aligned!");
			adjalign(-tmp);
		}

		break;

	    case AD_CONT:

			pline("The explosion greatly increases your contamination.");
			contaminate(tmp, TRUE);

		break;

	    case AD_WEBS: 

		{
			struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB, 0, FALSE);
			if (ttmp2) {
				pline_The("webbing sticks to you. You're caught!");
				dotrap(ttmp2, NOWEBMSG);
				if (u.usteed && u.utrap && !mayfalloffsteed()) {
				/* you, not steed, are trapped */
				dismount_steed(DISMOUNT_FELL);
				}
			}
		}
		/* Amy addition: sometimes, also make a random trap somewhere on the level :D */
		if (issoviet || !rn2(2)) makerandomtrap(FALSE);

		mdamageu(mtmp, tmp);

		break;

	    case AD_TRAP:
		if (t_at(u.ux, u.uy) == 0) (void) maketrap(u.ux, u.uy, randomtrap(), 0, FALSE);
		else makerandomtrap(FALSE);

		mdamageu(mtmp, tmp);

		break;

	    case AD_ENCH:
		{
			struct obj *obj = some_armor(&youmonst);

			if (obj && drain_item(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			}
		}
		break;

	    case AD_NGEN:
		{
			struct obj *obj = some_armor(&youmonst);

			if (obj && drain_item_severely(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			}
		}
		break;

	    case AD_STTP:

		pline("You are surrounded by a purple glow!");
		if (invent) {
		    int itemportchance = 10 + rn2(21);
		    for (otmpi = invent; otmpi; otmpi = otmpii) {

		      otmpii = otmpi->nobj;

			if (!rn2(itemportchance) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}

		    }
		}
		break;

	    case AD_NACU:

		nastytrapcurse();
		break;

	    case AD_ICUR:

		You_feel("as if you need some help.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		rndcurse();
		mdamageu(mtmp, tmp);

		break;

	    case AD_TLPT:
		Your("position suddenly seems very uncertain!");
		teleX();
		break;

	    case AD_ABDC:
		Your("position suddenly seems very uncertain!");
		teleX();
		mdamageu(mtmp, tmp);
		break;

	    case AD_WEEP:

		if ((!u.uevent.udemigod || u.freeplaymode) && !playerlevelportdisabled() ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.levelporting) {
				u.levelporting = 1;
				nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
			}

		}
		else if ((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && (u.urmaxlvlUP >= 2) ) {
		    losexp("loss of potential", FALSE, TRUE);
		}
		mdamageu(mtmp, tmp);
		break;

	    case AD_BANI:
		if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed)) ) { pline("You shudder for a moment."); (void) safe_teleds_normalterrain(FALSE); break;}
		if (playerlevelportdisabled()) {
			pline("For some reason you resist the banishment!");
			break;
		}

		make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

		if (!u.banishmentbeam) {
			u.banishmentbeam = 1;
			nomul(-2, "being banished", FALSE); /* because it's not called until you get another turn... */
		}
		break;

	    case AD_DISP:
		mdamageu(mtmp, tmp);
		pushplayer(FALSE);
		break;

	    case AD_MANA:
		drain_en(tmp);
		mdamageu(mtmp, tmp);
		break;

	    case AD_DREN:
		drain_en(tmp);
		break;

	    case AD_DETH:

		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("Was that the touch of death?");
		    mdamageu(mtmp, tmp);
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!PlayerResistsDeathRays) {
			u.youaredead = 1;
			killer_format = KILLED_BY_AN;
			killer = "explosion of death";
			done(DIED);
			u.youaredead = 0;
			tmp = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You_feel("your life force draining away...");
			losehp(15 + tmp, "explosion of death", KILLED_BY_AN);
			u.uhpmax -= tmp;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("Lucky for you, it didn't work!");
		    tmp = 0;
		    break;
		}
		mdamageu(mtmp, tmp);
		break;
	    case AD_SKIL:
		skillcaploss();
		mdamageu(mtmp, tmp);
		break;

	    case AD_TDRA:
		techdrain();
		mdamageu(mtmp, tmp);
		break;

	    case AD_DROP:
		dropitemattack();
		mdamageu(mtmp, tmp);
		break;

	    case AD_BLAS:
		u.ugangr++;
	      You("get the feeling that %s is angry...", u_gname());
		mdamageu(mtmp, tmp);
		break;

	    case AD_SUCK:
			if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) tmp = 0;
			else{
				if( has_head(youmonst.data) && !(Role_if(PM_COURIER)) && !uarmh && !rn2(20) && 
					((!Upolyd && u.uhp < (u.uhpmax / 10) ) || (Upolyd && u.mh < (u.mhmax / 10) ))
				){
					tmp = 2 * (Upolyd ? u.mh : u.uhp)
						  + 400; //FATAL_DAMAGE_MODIFIER;
					pline("The explosion pulverizes your %s!", body_part(HEAD));
				}
				else{
					You_feel("the explosion sucking your extremities off!");
					if(!rn2(3)){
						Your("%s twist from the suction!", makeplural(body_part(LEG)));
					    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    set_wounded_legs(LEFT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    exercise(A_STR, FALSE);
					    exercise(A_DEX, FALSE);
					}
					if(uwep && !rn2(2)){
						pline("You're having trouble holding your weapon!");
						if( rnd(130) > ACURR(A_STR)){
							Your("weapon is blown out of your grasp!");
							optr = uwep;
							uwepgone();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						}
						else{
							You("keep a tight grip on your weapon!");
						}
					}
					if(!rn2(3) && uarmf){
						Your("boots are sucked off!");
						optr = uarmf;
						if (donning(optr)) cancel_don();
						(void) Boots_off();
						freeinv(optr);
						(void) mpickobj(mtmp,optr,FALSE);
					}
					if(!rn2(2) && uarmg && !uwep){
						pline("You're having difficulties keeping your gloves on!");
						if( rnd(130) > ACURR(A_STR)){
							Your("gloves are sucked off!");
							optr = uarmg;
							if (donning(optr)) cancel_don();
							(void) Gloves_off();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						}
						else You("keep your %s closed.", makeplural(body_part(HAND)));
					}
					if(!rn2(3) && uarms){
						pline("You can barely hold your shield!");
						if( rnd(150) > ACURR(A_STR)){
							Your("shield is sucked out of your grasp!");
							optr = uarms;
							if (donning(optr)) cancel_don();
							Shield_off();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						 }
						 else{
							You("keep a tight grip on your shield!");
						 }
					}
					if(rn2(3) && uarmh){
						Your("helmet is sucked off!");
						optr = uarmh;
						if (donning(optr)) cancel_don();
						(void) Helmet_off();
						freeinv(optr);
						(void) mpickobj(mtmp,optr,FALSE);
					}
				}
			}
		mdamageu(mtmp, tmp);
		break;

	    case AD_CORR:
		hurtarmor(AD_CORR);
		break;

	    case AD_DCAY:
		hurtarmor(AD_DCAY);
		break;

	    case AD_FLAM:
		hurtarmor(AD_FLAM);
		break;

	    case AD_HEAL:
		if(!uwep
		   && (!uarmu || (uarmu && uarmu->oartifact == ART_GIVE_ME_STROKE__JO_ANNA))
		   && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    reducesanity(1);
		    if (Upolyd) {
			u.mh += rnd(7);
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			    u.cnd_nursehealamount++;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10)) {
				u.uhpmax++;
				u.cnd_nursehealamount++;
			    }
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    }
		    if (!rn2(3)) exercise(A_STR, TRUE);
		    if (!rn2(3)) exercise(A_CON, TRUE);
		    if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    flags.botl = 1;
		    tmp = 0;
		} else {
		    if (Role_if(PM_HEALER) || Race_if(PM_HERBALIST)) {
		      pline("You are either not unarmed or not naked, and therefore the healing attempt fails. Bummer.");
			tmp = 0;
		    } else mdamageu(mtmp, tmp);
		}
		break;

	    case AD_CURS:

		if(!rn2(2) || night() ) {
			attrcurse();
		}
		break;

	    case AD_LITE:

		if(!rn2(2) || night() ) {
			attrcurse();
		}
		mdamageu(mtmp, tmp);
		break;

	    case AD_CHRN:

	      if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: diseasemu(mdat);
			    break;
		    case 1: make_blinded(Blinded + tmp, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + tmp, TRUE);
			    break;
		    case 3: make_stunned(HStun + tmp, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + tmp, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + tmp, TRUE);
			    break;
		    case 6: make_burned(HBurned + tmp, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + tmp, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + tmp, TRUE);
			    break;
		}

		break;

	    case AD_FREN:
			if (u.berserktime) {
			    if (!obsidianprotection()) switch (rn2(11)) {
			    case 0: diseasemu(mtmp->data);
				    break;
			    case 1: make_blinded(Blinded + tmp, TRUE);
				    break;
			    case 2: if (!Confusion)
					You("suddenly feel %s.",
					    FunnyHallu ? "trippy" : "confused");
				    make_confused(HConfusion + tmp, TRUE);
				    break;
			    case 3: make_stunned(HStun + tmp, TRUE);
				    break;
			    case 4: make_numbed(HNumbed + tmp, TRUE);
				    break;
			    case 5: make_frozen(HFrozen + tmp, TRUE);
				    break;
			    case 6: make_burned(HBurned + tmp, TRUE);
				    break;
			    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
				    break;
			    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
				    break;
			    case 9: make_feared(HFeared + tmp, TRUE);
				    break;
			    case 10: make_dimmed(HDimmed + tmp, TRUE);
				    break;
			    }

			} else u.berserktime = tmp;

		break;

	    case AD_SLOW:

		if (HFast && !defends(AD_SLOW, uwep)) {
		    u_slow_down();

		    if (Race_if(PM_SPIRIT) && !rn2(3)) {
			u.uprops[DEAC_FAST].intrinsic += ((tmp + 2) * 10);
			pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
			u.inertia += (tmp + 2);
		    }
		}
		break;
	    case AD_SHRD:
		{
			struct obj *obj = some_armor(&youmonst);

			if (obj && drain_item(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			} else if (obj) wither_dmg(obj, xname(obj), rn2(4), FALSE, &youmonst);
		}
		mdamageu(mtmp, tmp);

		break;

	    case AD_NPRO:
		u.negativeprotection++;
		if (evilfriday && u.ublessed > 0) {
			u.ublessed -= 1;
			if (u.ublessed < 0) u.ublessed = 0;
		}
		You_feel("less protected!");
		mdamageu(mtmp, tmp);
		break;

	    case AD_CHKH:

		tmp += u.chokhmahdamage;
		tmp += rnd(u.ualign.sins > 0 ? (isqrt(u.ualign.sins) + 1) : (1));
		u.chokhmahdamage++;
		mdamageu(mtmp, tmp);
		break;

	    case AD_DEPR:

		    switch(rnd(20)) {
		    case 1:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
			      polyself(FALSE);
			}
			break;
		    case 2:
			You("need reboot.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
			if (!Race_if(PM_UNGENOMOLD)) newman();
			else polyself(FALSE);
			break;
		    case 3: case 4:
			if(!rn2(4) && u.ulycn == NON_PM &&
				!Protection_from_shape_changers &&
				!is_were(youmonst.data) &&
				!defends(AD_WERE,uwep)) {
			    You_feel("feverish.");
			    exercise(A_CON, FALSE);
			    u.ulycn = PM_WERECOW;
			    u.cnd_lycanthropecount++;
			} else {
				if (multi >= 0) {
				    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) break;
				    fall_asleep(-rnd(10), TRUE);
				    if (Blind) You("are put to sleep!");
				    else You("are put to sleep by %s!", mon_nam(mtmp));
				}
			}
			break;
		    case 5: case 6:
			/* nothing, because monster cannot grab you - it just blew itself up! --Amy */
			break;
		    case 7:
		    case 8:
			Your("position suddenly seems very uncertain!");
			teleX();
			break;
		    case 9:
			u_slow_down();
			break;
		    case 10:
			hurtarmor(AD_RUST);
			break;
		    case 11:
			hurtarmor(AD_DCAY);
			break;
		    case 12:
			hurtarmor(AD_CORR);
			break;
		    case 13:
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(5), "paralyzed by a monster explosion", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 14:
			if (FunnyHallu)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			hallutime = rn1(7, 16);
			make_stunned(HStun + hallutime + tmp, FALSE);
			(void) make_hallucinated(HHallucination + hallutime + tmp,TRUE,0L);
			break;
		    case 15:
			if(!Blind)
				Your("vision bugged.");
			hallutime += rn1(10, 25);
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + tmp + tmp,TRUE,0L);
			break;
		    case 16:
			if(!Blind)
				Your("vision turns to screen saver.");
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + tmp,TRUE,0L);
			break;
		    case 17:
			{
			    struct obj *obj = some_armor(&youmonst);

			    if (obj && drain_item(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			    }
			}
			break;
		    default:
			    if(Confusion)
				 You("are getting even more confused.");
			    else You("are getting confused.");
			    make_confused(HConfusion + tmp, FALSE);
			break;
		    }
		    exercise(A_INT, FALSE);

		mdamageu(mtmp, tmp);
		break;

	    case AD_BADE:

		badeffect();
		mdamageu(mtmp, tmp);

		break;

	    case AD_RBAD:

		reallybadeffect();
		mdamageu(mtmp, tmp);

		break;

	    case AD_BLEE:
		if (tmp > 0) playerbleed(tmp);
		mdamageu(mtmp, tmp);

		break;

	    case AD_SHAN:
		shank_player();

		break;

	    case AD_DEBU:
		statdebuff();

		break;

	    case AD_UNPR:

		{
			struct obj *obj = some_armor(&youmonst);
			if (obj && obj->oerodeproof) {
				obj->oerodeproof = FALSE;
				Your("%s brown for a moment.", aobjnam(obj, "glow"));
			}

			pline("A brown glow surrounds you...");
			for(obj = invent; obj ; obj = obj->nobj)
				if (!rn2(3) && !stack_too_big(obj) && obj->oerodeproof) obj->oerodeproof = FALSE;

		}

		break;

	    case AD_NIVE:
		nivellate();

		break;

	    case AD_SCOR:
		u.urexp -= (tmp * 50);
		if (u.urexp < 0) u.urexp = 0;
		Your("score is drained!");

		break;

	    case AD_TERR:
		terrainterror();

		break;

	    case AD_FEMI:
		randomfeminismtrap(rnz( (tmp + 2) * rnd(100)));
		if (Role_if(PM_CLIMACTERIAL)) climtrainsqueaking(tmp);

		break;

	    case AD_LEVI:
		You("float up!");
		HLevitation &= ~I_SPECIAL;
		incr_itimeout(&HLevitation, tmp);

		break;

	    case AD_MCRE:
		MCReduction += (100 * tmp);
		pline("The magic cancellation granted by your armor seems weaker now...");

		break;

	    case AD_FUMB:

		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(10 * (tmp + 1) );

		break;

	    case AD_TREM:

		u.tremblingamount++;
		pline("Suddenly, your entire body trembles.");

		break;

	    case AD_IDAM:

		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(20)) rust_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}

		break;

	    case AD_ANTI:

		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			if (!rn2(20)) wither_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
		    }
		}

		break;

	    case AD_PAIN:

		pline("OUCH!");
		losehp(Upolyd ? ((u.mh / 4) + 1) : ((u.uhp / 4) + 1), "a painful explosion", KILLED_BY);

		break;

	    case AD_TECH:

		techcapincrease(tmp * rnd(50));

		break;

	    case AD_MEMO:

		hitmsg(mtmp, mattk);

		spellmemoryloss(tmp);

		break;

	    case AD_TRAI:

		hitmsg(mtmp, mattk);

		skilltrainingdecrease(tmp);
		if (!rn2(10)) skillcaploss();

		break;

	    case AD_STAT:

		statdrain();

		break;

	    case AD_NAST:

		pline("Nasty!");

		randomnastytrapeffect(rnz( (tmp + 2) * rnd(100) ), 1000 - (tmp * 3));

		break;

	    case AD_VULN:

		deacrandomintrinsic(rnz( (tmp * rnd(30) ) + 1));

		break;

	    case AD_BLND:
		not_affected = resists_blnd(&youmonst);
		if (!not_affected) {
		    /* sometimes you're affected even if it's invisible */
		    if (mon_visible(mtmp) || (rnd(tmp /= 2) > u.ulevel)) {
			You("are blinded by a blast of light!");
			make_blinded((long)tmp, FALSE);
			if (!Blind) Your("%s", vision_clears);
		    } else if (flags.verbose)
			You("get the impression it was not terribly bright.");
		}
		break;

	    case AD_TIME: /* timebomb */

		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) break;
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Zhizn' razgonyal nazad, potomu chto vy ne smotreli, i teper' vy dolzhny poluchit', chto poteryannyy uroven' nazad." : "Kloeck!");
			      if (u.urmaxlvlUP >= 2) losexp("time", FALSE, FALSE); /* resistance is futile :D */
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				switch (rnd(A_MAX)) {
					case A_STR:
						pline("You're not as strong as you used to be...");
						ABASE(A_STR) -= 5;
						if(ABASE(A_STR) < ATTRMIN(A_STR)) {tmp *= 3; ABASE(A_STR) = ATTRMIN(A_STR);}
						break;
					case A_DEX:
						pline("You're not as agile as you used to be...");
						ABASE(A_DEX) -= 5;
						if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {tmp *= 3; ABASE(A_DEX) = ATTRMIN(A_DEX);}
						break;
					case A_CON:
						pline("You're not as hardy as you used to be...");
						ABASE(A_CON) -= 5;
						if(ABASE(A_CON) < ATTRMIN(A_CON)) {tmp *= 3; ABASE(A_CON) = ATTRMIN(A_CON);}
						break;
					case A_WIS:
						pline("You're not as wise as you used to be...");
						ABASE(A_WIS) -= 5;
						if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {tmp *= 3; ABASE(A_WIS) = ATTRMIN(A_WIS);}
						break;
					case A_INT:
						pline("You're not as bright as you used to be...");
						ABASE(A_INT) -= 5;
						if(ABASE(A_INT) < ATTRMIN(A_INT)) {tmp *= 3; ABASE(A_INT) = ATTRMIN(A_INT);}
						break;
					case A_CHA:
						pline("You're not as beautiful as you used to be...");
						ABASE(A_CHA) -= 5;
						if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {tmp *= 3; ABASE(A_CHA) = ATTRMIN(A_CHA);}
						break;
				}
				break;
			case 10:
				pline("You're not as powerful as you used to be...");
				ABASE(A_STR)--;
				ABASE(A_DEX)--;
				ABASE(A_CON)--;
				ABASE(A_WIS)--;
				ABASE(A_INT)--;
				ABASE(A_CHA)--;
				if(ABASE(A_STR) < ATTRMIN(A_STR)) {tmp *= 2; ABASE(A_STR) = ATTRMIN(A_STR);}
				if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {tmp *= 2; ABASE(A_DEX) = ATTRMIN(A_DEX);}
				if(ABASE(A_CON) < ATTRMIN(A_CON)) {tmp *= 2; ABASE(A_CON) = ATTRMIN(A_CON);}
				if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {tmp *= 2; ABASE(A_WIS) = ATTRMIN(A_WIS);}
				if(ABASE(A_INT) < ATTRMIN(A_INT)) {tmp *= 2; ABASE(A_INT) = ATTRMIN(A_INT);}
				if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {tmp *= 2; ABASE(A_CHA) = ATTRMIN(A_CHA);}
				break;
		}
		break;

	    case AD_DISN: /* for jonadab's disintegrating sphere */

		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable || (Stoned_chiller && Stoned)) {
                pline("You are unharmed!");
                break;
		}

		 else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
		    break;
		} else if (uarmu) {
		    /* destroy shirt */
		    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
		    break;
		}

		if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "KABOOM you died", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}

		break;

	    case AD_VAPO:

	      if (!Disint_resistance) mdamageu(mtmp, (tmp * 3));
		else mdamageu(mtmp, tmp);

		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable || (Stoned_chiller && Stoned)) {
                pline("You are unharmed!");
                break;
		}

		 else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
		    break;
		} else if (uarmu) {
		    /* destroy shirt */
		    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
		    break;
		}

		if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "vaporization kaboom you died", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}

		break;

	    case AD_STON: /* mainly for jonadab's stoning sphere */

		if (StrongStone_resistance && rn2(10) && tmp > 1) tmp /= 3;

			if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) 
			{
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = mtmp->data->mname;
					if (mtmp->data->geno & G_UNIQ) {
					    if (!type_is_pname(mtmp->data)) {
						static char kbuf[BUFSZ];
	
						/* "the" buffer may be reallocated */
						strcpy(kbuf, the(delayed_killer));
						delayed_killer = kbuf;
					    }
					    killer_format = KILLED_BY;
					} else killer_format = KILLED_BY_AN;
				}
			}

		break;

	    case AD_EDGE:

		if (!Stone_resistance || !rn2(StrongStone_resistance ? 100 : 20)) {
			pline("The edgy splinters severely hurt you!");
			if (Upolyd) {u.mhmax--; if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax--; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }
		}

			if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) 
			{
				if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = mtmp->data->mname;
					if (mtmp->data->geno & G_UNIQ) {
					    if (!type_is_pname(mtmp->data)) {
						static char kbuf[BUFSZ];
	
						/* "the" buffer may be reallocated */
						strcpy(kbuf, the(delayed_killer));
						delayed_killer = kbuf;
					    }
					    killer_format = KILLED_BY;
					} else killer_format = KILLED_BY_AN;
				}
			}

		break;

	    case AD_PLYS: /* for jonadab's paralyzing sphere */
	    case AD_TCKL:

		    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
			You("momentarily stiffen.");            
		    } else {
			You("suddenly recognize an inability to move!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
			nomovemsg = 0;	/* default: "you can move again" */
			{
				int paralysistime = tmp;
				if (paralysistime > 1) paralysistime = rnd(paralysistime);
				if (paralysistime > 10) {
					while (rn2(5) && (paralysistime > 10)) {
						paralysistime--;
					}
				}
				nomul(-(paralysistime), "paralyzed by an explosion", TRUE);
			}
			exercise(A_DEX, FALSE);
		    }

		break;

	    case AD_RUST:

		if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
			water_damage(invent, FALSE, FALSE);
		}

		break;

	    case AD_AMNE:

		maprot();

		break;

	    case AD_WET:

		pline("Water washes over you!");
		if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
		}
		break;

	    case AD_LETH:

		pline("Sparkling water washes over you!");
		lethe_damage(invent, FALSE, FALSE);
		if (!rn2(3)) actual_lethe_damage(invent, FALSE, FALSE);
		if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
		if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		break;

	    case AD_CNCL:

		(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		break;

	    case AD_LUCK:
		change_luck(-1);
		You_feel("unlucky.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Prosto stoyat' i poteryat' stol'ko udachi, kak vy mozhete! Eto sdelayet igru boleye legkoy dlya Vas! V samom dele! Potomu chto togda vy budete umirat' bystreye i sdelat' s ney, tak chto vy mozhete sosredotochit'sya na tom, kakoy na samom dele produktivnoy veshchi vmesto togo, chtoby, kak delat' posudu!" : "Dieuuuuuuu!");
		break;

	    case AD_FAKE:
		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}
	      mdamageu(mtmp, tmp);
		break;

	    case AD_NEXU:
		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= (1 + rnd(2));

		switch (rnd(7)) {

			case 1:
			case 2:
			case 3:
				pline("%s sends you far away!", Monnam(mtmp) );
				teleX();
				break;
			case 4:
			case 5:
				pline("%s sends you away!", Monnam(mtmp) );
				phase_door(0);
				break;
			case 6:

				if ((!u.uevent.udemigod || u.freeplaymode) && !playerlevelportdisabled() ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
					}
				}
				break;
			case 7:
				{
					nexus_swap();

					if (!rn2(3)) {

						int reducedstat = rn2(A_MAX);
						if(ABASE(reducedstat) <= ATTRMIN(reducedstat)) {
							pline("Your health was damaged!");
							u.uhpmax -= rnd(5);
							if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
							if (u.uhp < 1) {
								u.youaredead = 1;
								killer = "nexus scrambling";
								killer_format = KILLED_BY;
								done(DIED);
								u.youaredead = 0;
							}

						} else {
							ABASE(reducedstat) -= 1;
							AMAX(reducedstat) -= 1;
							flags.botl = 1;
							pline("Your attributes were damaged!");
						}
					}
				}
				break;
		}
	      mdamageu(mtmp, tmp);
		break;

	    case AD_SOUN:

		pline("Your ears are blasted by hellish noise!");
		if (YouAreDeaf) tmp /= 2;
		make_stunned(HStun + tmp, TRUE);
		if (isevilvariant || issoviet || !rn2(2)) (void)destroy_item(POTION_CLASS, AD_COLD);
		wake_nearby();
	      mdamageu(mtmp, tmp);
		break;

	    case AD_GRAV:

		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= 2;

		pline("Gravity warps around you...");
		phase_door(0);
		pushplayer(FALSE);
		u.uprops[DEAC_FAST].intrinsic += (tmp + 2);
		make_stunned(HStun + tmp, TRUE);
	      mdamageu(mtmp, tmp);
		break;

	    case AD_WGHT:
		pline("Your load feels heavier!");
		IncreasedGravity += (1 + (tmp * rnd(20)));

		break;

	    case AD_INER:
	      u_slow_down();
		u.uprops[DEAC_FAST].intrinsic += ((tmp + 2) * 10);
		pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
		u.inertia += (tmp + 2);
	      mdamageu(mtmp, tmp);
		break;

	    case AD_SLUD:

		{
		    register struct obj *objX, *objX2;
		    for (objX = invent; objX; objX = objX2) {
		      objX2 = objX->nobj;
			rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
		    }
		}
		break;

	    case AD_DRLI:

		if (moves < 1000 && (u.urmaxlvlUP == 1) && rn2(25)) {
			break;
		}

		if (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )
		    losexp("draining explosion", FALSE, TRUE);
		break;

	    case AD_HODS:
		 if(uwep) {
			int artifactvar = tmp; /* to fix a weird compiler warning for artifact_hit() below --Amy */
			if (uwep->otyp == CORPSE
				&& touch_petrifies(&mons[uwep->corpsenm])) {
			    tmp = 1;
				pline("You touch the petrifying corpse...");
			    if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
					if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
					else {
					    Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
						u.cnd_stoningcount++;
					    killer_format = KILLED_BY;
					    delayed_killer = "their own petrifying corpse";
					}
			    }
			}
			tmp += dmgval(uwep, &youmonst);
			
			if (uwep->opoisoned){
				sprintf(buf, "%s %s",
					s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
				poisoned(buf, A_CON, mdat->mname, 30);
			}
			
			if (tmp <= 0) tmp = 1;
			if (!(uwep->oartifact &&
				artifact_hit(mtmp, &youmonst, uwep, &artifactvar, dieroll)))
			     {pline("Clink!");}
		 }
	      mdamageu(mtmp, tmp);
		break;

	    case AD_LEGS:
		{
			register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
			const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";

			set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)) + tmp );
			pline("The explosion blew off your %s %s!", sidestr, body_part(LEG));
			exercise(A_STR, FALSE);
			exercise(A_DEX, FALSE);
		}
		break;

	    case AD_VAMP:

		if (moves < 1000 && (u.urmaxlvlUP == 1) && rn2(25)) {
			break;
		}

		losexp("draining explosion", FALSE, TRUE);
	      mdamageu(mtmp, tmp);
		break;

	    case AD_DRST:

		if (!Poison_resistance || !rn2(StrongPoison_resistance ? 5 : 3) ) {
		poisoned("blast", A_STR, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DISE:
		diseasemu(mdat);
		break;

	    case AD_SPEL:
	    case AD_CAST:
	    case AD_CLRC:
		castmu(mtmp, mattk, TRUE, TRUE);
		break;

	    case AD_VOMT:
		if (!rn2(StrongSick_resistance ? 100 : 10) || !Sick_resistance) {
			if (!Vomiting) {
				make_vomiting(Vomiting+d(10,4), TRUE);
				pline("You feel nauseated.");
				if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10);
			} else diseasemu(mdat);
		}
		break;

	    case AD_PEST:
		diseasemu(mdat);
	      mdamageu(mtmp, tmp);
		break;

	    case AD_FAMN:

		if (!is_fainted() && rn2(10) ) morehungry(rnz(40));
		if (!is_fainted() && rn2(10) ) morehungry(rnz(40));
		morehungry(tmp); morehungry(tmp); /* This attack was way too weak. --Amy */
		/* plus the normal damage */
	      mdamageu(mtmp, tmp);
		break;

	    case AD_LAZY: /* laziness attack; do lots of nasty things at random */
		if(!rn2(2)) {
		    pline("For some reason you are not affected.");
		    break;
		}
		You_feel("apathetic...");
		switch(rn2(7)) {
		    case 0: /* destroy certain things */
			witherarmor();
			break;
		    case 1: /* sleep */
			if (multi >= 0) {
			    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) {pline("You yawn."); break;}
			    fall_asleep(-rnd(10), TRUE);
				pline("You fall asleep!");
			}
			break;
		    case 2: /* paralyse */
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				pline("You can't move!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(5), "paralyzed by an explosion", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 3: /* slow */
			if(HFast)  u_slow_down();
			else You("pause momentarily.");
			break;
		    case 4: /* drain Dex */
			adjattrib(A_DEX, -rn1(1,1), 0, TRUE);
			break;
		    case 5: /* steal teleportitis */
			if(HTeleportation & INTRINSIC) {
			      HTeleportation &= ~INTRINSIC;
			}
	 		if (HTeleportation & TIMEOUT) {
				HTeleportation &= ~TIMEOUT;
			}
			if(HTeleport_control & INTRINSIC) {
			      HTeleport_control &= ~INTRINSIC;
			}
	 		if (HTeleport_control & TIMEOUT) {
				HTeleport_control &= ~TIMEOUT;
			}
		      You("don't feel in the mood for jumping around.");
			break;
		    case 6: /* steal sleep resistance */
			if(HSleep_resistance & INTRINSIC) {
				HSleep_resistance &= ~INTRINSIC;
			} 
			if(HSleep_resistance & TIMEOUT) {
				HSleep_resistance &= ~TIMEOUT;
			} 
			You_feel("like you could use a nap.");
			break;
		}
	      mdamageu(mtmp, tmp);
		break;
	    case AD_WRAT:
		pline("The life is drawn from your bones.");

		if(u.uen < 1) {
		    You_feel("less energised!");
		    u.uenmax -= rn1(10,10);
		    if(u.uenmax < 0) u.uenmax = 0;
		} else if(u.uen <= 10) {
		    You_feel("your magical energy dwindle to nothing!");
		    u.uen = 0;
		} else {
		    You_feel("your magical energy dwindling rapidly!");
		    u.uen /= 2;
		}
	      mdamageu(mtmp, tmp);

		break;
	    case AD_NGRA:
		if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
			pline("The explosion wipes out the engraving underneath you!");
			del_engr(ep);
			ep = (struct engr *)0;
		}
	      mdamageu(mtmp, tmp);

		break;

	    case AD_POLY:
		if (!Unchanging && !Antimagic) {
			You("undergo a freakish metamorphosis!");
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
			polyself(FALSE);
		}
		break;

	    case AD_CHAO:
		if (!Unchanging && !Antimagic) {
			You("undergo a freakish metamorphosis!");
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
			polyself(FALSE);
		}

	      if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: diseasemu(mdat);
			    break;
		    case 1: make_blinded(Blinded + tmp, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + tmp, TRUE);
			    break;
		    case 3: make_stunned(HStun + tmp, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + tmp, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + tmp, TRUE);
			    break;
		    case 6: make_burned(HBurned + tmp, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + tmp, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + tmp, TRUE);
			    break;
		}

		break;

	    case AD_CALM:
		docalm();
		break;

	    case AD_DRDX:

		if (!Poison_resistance || !rn2(StrongPoison_resistance ? 5 : 3) ) {
		poisoned("blast", A_DEX, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DRCO:

		if (!Poison_resistance || !rn2(StrongPoison_resistance ? 5 : 3) ) {
		poisoned("blast", A_CON, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_WISD:

		if (!Poison_resistance || !rn2(StrongPoison_resistance ? 5 : 3) ) {
		poisoned("blast", A_WIS, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DRCH:

		if (!Poison_resistance || !rn2(StrongPoison_resistance ? 5 : 3) ) {
		poisoned("blast", A_CHA, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_POIS:

		if (!Poison_resistance || !rn2(StrongPoison_resistance ? 5 : 3) ) {
		poisoned("blast", rn2(A_MAX), "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_VENO:

		if (chromeprotection()) {
		      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
			break;
		}

		if (!Poison_resistance || !rn2(StrongPoison_resistance ? 5 : 3) ) {
		poisoned("blast", rn2(A_MAX), "venom explosion", 5);
		}
		if (!Poison_resistance) pline("You're badly poisoned!");
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE, TRUE);
		if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE, TRUE);

		if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(POTION_CLASS, AD_VENO);
		if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(FOOD_CLASS, AD_VENO);

	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DFOO:
		pline("The explosion takes you down a peg or two.");
		if (rn2(3)) {
		    sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, rn2(A_MAX), mdat->mname, 30);
		}
		if (!rn2(2)) {
			You_feel("drained...");
			u.uhpmax -= rn1(10,10);
			if (u.uhpmax < 1) u.uhpmax = 1;
			if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (!rn2(2)) {
			You_feel("less energised!");
			u.uenmax -= rn1(10,10);
			if (u.uenmax < 0) u.uenmax = 0;
			if(u.uen > u.uenmax) u.uen = u.uenmax;
		}
		if (!rn2(2)) {
			if((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && (u.urmaxlvlUP >= 2) )
			    losexp("life drainage", FALSE, TRUE);
			else You_feel("woozy for an instant, but shrug it off.");
		}
	      mdamageu(mtmp, tmp);
		break;

	    case AD_DRIN:

		if (defends(AD_DRIN, uwep) || !has_head(youmonst.data) || Role_if(PM_COURIER)) {
		    You("don't seem harmed.");
		    /* Not clear what to do for green slimes */
		    break;
		}

		if (uarmh && !(uarmh && itemhasappearance(uarmh, APP_STRIP_BANDANA)) && rn2(3)) {
		    /* not body_part(HEAD) */
		    Your("helmet blocks the attack to your head.");
		    break;
		}
		
		mdamageu(mtmp, tmp);

		if (!uarmh || (uarmh->otyp != DUNCE_CAP)) {
		    Your("brain is eaten!");
		    /* No such thing as mindless players... */
		    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
			int lifesaved = 0;
			struct obj *wore_amulet = uamul;

			while(1) {
			    /* avoid looping on "die(y/n)?" */
			    if (lifesaved && (discover || wizard)) {
				if (wore_amulet && !uamul) {
				    /* used up AMULET_OF_LIFE_SAVING; still
				       subject to dying from brainlessness */
				    wore_amulet = 0;
				} else if (wizard) {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }
				u.youaredead = 1;
				u.youarereallydead = 1;

			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    if (wizard) {
				    u.youaredead = 0;
				    u.youarereallydead = 0;
			    }
			    lifesaved++;
			}
		    }
		}
		/* adjattrib gives dunce cap message when appropriate */
		if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
		else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE, TRUE);
		if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
		if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		exercise(A_WIS, FALSE);
		break;

	    case AD_WTHR:

		withering_damage(invent, FALSE, FALSE);

		break;

	    case AD_SPC2:

		if (!Psi_resist || !rn2(StrongPsi_resist ? 100 : 20)) {

			pline("Your %s is spinning!", body_part(HEAD) );

			switch (rnd(10)) {

				case 1:
				case 2:
				case 3:
				make_confused(HConfusion + tmp, FALSE);
					break;
				case 4:
				case 5:
				case 6:
					make_stunned(HStun + tmp, FALSE);
					break;
				case 7:
					make_confused(HConfusion + tmp, FALSE);
					make_stunned(HStun + tmp, FALSE);
					break;
				case 8:
					make_hallucinated(HHallucination + tmp, FALSE, 0L);
					break;
				case 9:
					make_feared(HFeared + tmp, FALSE);
					break;
				case 10:
					make_numbed(HNumbed + tmp, FALSE);
					break;

			}
			if (!rn2(200)) {
				forget(rnd(5));
				pline("You forget some important things...");
			}
			if (!rn2(200) && (u.urmaxlvlUP >= 2)) {
				losexp("psionic drain", FALSE, TRUE);
			}
			if (!rn2(200)) {
				adjattrib(A_INT, -1, 1, TRUE);
				adjattrib(A_WIS, -1, 1, TRUE);
			}
			if (!rn2(200)) {
				pline("You scream in pain!");
				wake_nearby();
			}
			if (!rn2(200)) {
				badeffect();
			}
			if (!rn2(5)) increasesanity(rnd(100));

		}

	      mdamageu(mtmp, tmp); /* still does damage even if you resist psi --Amy */

		break;

	    case AD_SLIM:

		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		} else if (Unchanging || slime_on_touch(youmonst.data)) {
		    You("are unaffected.");
		} else if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a slimy explosion";
		} else
		    pline("Yuck!");

		break;

	    case AD_LITT:

		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		} else if (Unchanging || slime_on_touch(youmonst.data)) {
		    You("are unaffected.");
		} else if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(20);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a slimy explosion";
		} else
		    pline("Yuck!");

		{
		    register struct obj *littX, *littX2;
		    for (littX = invent; littX; littX = littX2) {
		      littX2 = littX->nobj;
			if (!rn2(StrongAcid_resistance ? 1000 : Acid_resistance ? 100 : 10)) rust_dmg(littX, xname(littX), 3, TRUE, &youmonst);
		    }
		}

		break;

	    case AD_STUN:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!FunnyHallu) You("are stunned by a blast of light!");
			else pline("%s NUMBED CAN'T DO",urole.name.m);
			make_stunned(HStun + (long)tmp, FALSE);
		}
		break;

	    case AD_NUMB:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!FunnyHallu) You("are numbed by a blast of light!");
			else pline("%s NUMBED YER",urole.name.m);
			make_numbed(HNumbed + (long)tmp, FALSE);
		}
		break;

	    case AD_GLIB:
		pline("Your hands got ripped off by the explosion!");
		incr_itimeout(&Glib, tmp);

		break;

	    case AD_UVUU:
		{
		int wdmg = (int)(tmp/6) + 1;
		hitmsg(mtmp, mattk);
		sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		poisoned(buf, A_CON, mdat->mname, 60);
		if(Poison_resistance) wdmg -= ACURR(A_CON)/2;
		if (StrongPoison_resistance && wdmg > 1) wdmg /= 2;
		if(wdmg > 0){
		
			while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
				wdmg--;
				(void) adjattrib(A_WIS, -1, TRUE, TRUE);
				forget_levels(1);	/* lose memory of 1% of levels per point lost*/
				forget_objects(1);	/* lose memory of 1% of objects per point lost*/
				exercise(A_WIS, FALSE);
			}
			if(AMAX(A_WIS) > ATTRMIN(A_WIS) && 
				ABASE(A_WIS) < AMAX(A_WIS)/2) AMAX(A_WIS) -= 1; //permanently drain wisdom
			if(wdmg){
				boolean chg;
				chg = make_hallucinated(HHallucination + (long)(wdmg*5),FALSE,0L);
			}
		}
		drain_en( (int)(tmp/2) );
		if(!rn2(20)){
			if (!has_head(youmonst.data) || Role_if(PM_COURIER) ) {
				tmp *= 2;
			}
			else if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) {
				pline("%s passes through your %s.",
				      mon_nam(mtmp), body_part(HEAD));
				tmp *= 2;
			}
			else {
				if(!uarmh){
					tmp = (ABASE(A_WIS) <= ATTRMIN(A_WIS)) ? ( 2 * (Upolyd ? u.mh : u.uhp) + 400) : (tmp * 2); 
					pline("%s's explosion rips off your %s!",
						mon_nam(mtmp), body_part(HEAD));
				} else pline("%s's explosion severely damages your %s!",
						mon_nam(mtmp), xname(uarmh) );
			}
		 }
 		}
	      mdamageu(mtmp, tmp);

	    break;

	    case AD_DARK:

		if (!(Race_if(PM_PLAYER_NIBELUNG) && rn2(5))) {
			pline("Everything gets dark!");
			litroomlite(FALSE);
		}
		if (youmonst.data->mlet == S_ANGEL || Race_if(PM_HUMANOID_ANGEL)) tmp *= 2;
	      mdamageu(mtmp, tmp);

		break;

	    case AD_FRZE:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!FunnyHallu) You("are frozen solid by a blast of light!");
			else pline("%s COOLING",urole.name.m);
			make_frozen(HFrozen + (long)tmp, FALSE);
		}
		break;

	    case AD_BURN:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!FunnyHallu) You("are burned by a blast of light!");
			else pline("%s BURNED",urole.name.m);
			make_burned(HBurned + (long)tmp, FALSE);
		}
		break;

	    case AD_DIMN:
		not_affected = (Blind && !rn2(3));
		if (!not_affected) {
			if (!FunnyHallu) You("are dimmed by a blast of light!");
			else pline("%s DISPIRITED",urole.name.m);
			make_dimmed(HDimmed + (long)tmp, FALSE);
		}
		break;

	    case AD_FEAR:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!FunnyHallu) You("are stricken with fear by a blast of light!");
			else pline("%s FEARED",urole.name.m);
			make_feared(HFeared + (long)tmp, FALSE);
		}
		break;

	    case AD_SANI:
		increasesanity(tmp);
		break;

	    case AD_INSA:
		not_affected = (Blind && !rn2(10));
		if (!not_affected) {
			You("are hit by an eldritch explosion and are getting insane!");
			make_feared(HFeared + tmp, TRUE);
			make_stunned(HStun + tmp, TRUE);
			if(Confusion) You("are getting even more confused.");
			else You("are getting confused.");
			make_confused(HConfusion + tmp, FALSE);
			increasesanity(rnd(100));
		}
		break;

	    case AD_HALU:
		not_affected |= (Blind && rn2(2)) ||
			(u.umonnum == PM_BLACK_LIGHT ||
			 u.umonnum == PM_VIOLET_FUNGUS ||
			 u.umonnum == PM_VIOLET_STALK ||
			 u.umonnum == PM_VIOLET_SPORE ||
			 u.umonnum == PM_VIOLET_COLONY ||
			 dmgtype(youmonst.data, AD_STUN));
		if (!not_affected) {
		    boolean chg;
		    if (!Hallucination)
			You("are caught in a blast of kaleidoscopic light!");
		    chg = make_hallucinated(HHallucination + (long)tmp,FALSE,0L);
		    You("%s.", chg ? "are freaked out" : "seem unaffected");
		}
		break;

	    default:
		break;
	}
	if (not_affected) {
	    You("seem unaffected by it.");
	    ugolemeffects((int)mattk->adtyp, tmp);
	}
    }
    /* let's be really mean and give them a chance of exploding several times... :-P --Amy */
    if (rn2(isevilvariant ? 2 : 10)) mondead(mtmp);
    wake_nearto(mtmp->mx, mtmp->my, 7*7);
    if (mtmp->mhp > 0) return(0);
    return(2);	/* it dies */
}

int
gazemu(mtmp, mattk)	/* monster gazes at you */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	register struct engr *ep = engr_at(u.ux,u.uy);
	char	 buf[BUFSZ];
	int dmgplus;
	struct obj *optr;
	struct obj *otmpi, *otmpii;
	int hallutime;

	/*int randattackB = 0;*/
	int atttypB;

	/* Monsters with AD_RBRE can use any random gaze. --Amy */

	atttypB = mattk->adtyp;

	if ((SecretAttackBug || u.uprops[SECRET_ATTACK_BUG].extrinsic || have_secretattackstone()) && atttypB == AD_PHYS && !rn2(100)) {
		while (atttypB == AD_ENDS || atttypB == AD_RBRE || atttypB == AD_WERE || atttypB == AD_PHYS) {
			atttypB = randattack(); }

	}

	if ((UnfairAttackBug || u.uprops[UNFAIR_ATTACK_BUG].extrinsic || have_unfairattackstone()) && atttypB == AD_PHYS && !rn2(100)) {
		while (atttypB == AD_ENDS || atttypB == AD_RBRE || atttypB == AD_WERE || atttypB == AD_PHYS) {
			atttypB = rn2(AD_ENDS); }

	}

	if (atttypB == AD_RBRE) {
		while (atttypB == AD_ENDS || atttypB == AD_RBRE || atttypB == AD_WERE) {
			atttypB = randattack(); }
		/*randattack = 1;*/
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
		atttypB = mtmp->m_id;
		if (atttypB < 0) atttypB *= -1;
		while (atttypB >= AD_ENDS) atttypB -= AD_ENDS;
		if (!(atttypB >= AD_PHYS && atttypB < AD_ENDS)) atttypB = AD_PHYS; /* fail safe --Amy */
		if (atttypB == AD_WERE) atttypB = AD_PHYS;
	}

	if (uamul && uamul->oartifact == ART_YOU_HAVE_UGH_MEMORY && rn2(10)) return 0; /* no message */

	if ((uarmg && itemhasappearance(uarmg, APP_MIRRORED_GLOVES) ) && !rn2(3) && !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee ) {
		/* cut down on message spam - only display it 1 out of 10 times --Amy */
		if (!rn2(10)) pline("%s gazes at you, but your mirrored gloves protect you from the effects!", Monnam(mtmp));
		return 0;
	}

	if ((uarmh && itemhasappearance(uarmh, APP_NETRADIATION_HELMET) ) && !rn2(2) && !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee ) {
		if (!rn2(10)) pline("%s gazes at you, but your netradiation helmet protects you from the effects!", Monnam(mtmp));
		return 0;
	}

	if (uarms && uarms->oartifact == ART_AEGIS && !rn2(2) && !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee) {
		if (!rn2(10)) pline("%s gazes at you, but Aegis you from the effects!", Monnam(mtmp));
		return 0;
	}

	if (RngeMirroring && !rn2(3) && !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee) {
		if (!rn2(10)) pline("%s gazes at you, but your mirroring protects you from the effects!", Monnam(mtmp));
		return 0;
	}

	/* charisma-based saving throw, because charisma should at least have some use --Amy */
	if (rnd(100) < ACURR(A_CHA)) return 0; /* no message because it would get too spammy */

	dmgplus = d((int)mattk->damn, (int)mattk->damd);	/* why the heck did gaze attacks have fixed damage??? --Amy */
	if (MaximumDamageBug || u.uprops[MAXIMUM_DAMAGE_BUG].extrinsic || have_maximumdamagestone() || (uwep && uwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI)) dmgplus = (int)mattk->damn * (int)mattk->damd;

	switch(atttypB) {
	    case AD_STON:
		if (mtmp->mcan || !mtmp->mcansee) {
		    if (!canseemon(mtmp)) break;	/* silently */
		    pline("%s %s.", Monnam(mtmp),
			  (mtmp->data == &mons[PM_MEDUSA] && mtmp->mcan) ?
				"doesn't look all that ugly" :
				"gazes ineffectually");
		    break;
		}
		if (Reflecting && couldsee(mtmp->mx, mtmp->my) &&
			mtmp->data == &mons[PM_MEDUSA]) {
		    /* hero has line of sight to Medusa and she's not blind */
		    boolean useeit = canseemon(mtmp);

		    if (useeit)
			(void) ureflects("%s gaze is reflected by your %s.",
					 s_suffix(Monnam(mtmp)));
		    if (mon_reflects(mtmp, !useeit ? (char *)0 :
				     "The gaze is reflected away by %s %s!"))
			break;
		    if (!m_canseeu(mtmp)) { /* probably you're invisible */
			if (useeit)
			    pline(
		      "%s doesn't seem to notice that %s gaze was reflected.",
				  Monnam(mtmp), mhis(mtmp));
			break;
		    }
		    if (useeit)
			pline("%s is turned to stone!", Monnam(mtmp));
		    stoned = TRUE;
		    killed(mtmp);

		    if (mtmp->mhp > 0) break;
		    return 2;
		}
		if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		    (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !rn2(15) ) {
		    You("meet %s gaze.", s_suffix(mon_nam(mtmp)));
		    stop_occupation();
		    if(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			break;
		    /*You("turn to stone...");
		    killer_format = KILLED_BY;
		    killer = mtmp->data->mname;
		    done(STONING);*/
		    You("start turning to stone...");
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("But you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = "petrifying gaze";
				}
			}
		}
		break;
	    case AD_EDGE:
		if (mtmp->mcan || !mtmp->mcansee) {
		    if (!canseemon(mtmp)) break;	/* silently */
		    pline("%s %s.", Monnam(mtmp),
			  (mtmp->data == &mons[PM_MEDUSA] && mtmp->mcan) ?
				"doesn't look all that ugly" :
				"gazes ineffectually");
		    break;
		}
		if (Reflecting && couldsee(mtmp->mx, mtmp->my) &&
			mtmp->data == &mons[PM_MEDUSA]) {
		    /* hero has line of sight to Medusa and she's not blind */
		    boolean useeit = canseemon(mtmp);

		    if (useeit)
			(void) ureflects("%s gaze is reflected by your %s.",
					 s_suffix(Monnam(mtmp)));
		    if (mon_reflects(mtmp, !useeit ? (char *)0 :
				     "The gaze is reflected away by %s %s!"))
			break;
		    if (!m_canseeu(mtmp)) { /* probably you're invisible */
			if (useeit)
			    pline(
		      "%s doesn't seem to notice that %s gaze was reflected.",
				  Monnam(mtmp), mhis(mtmp));
			break;
		    }
		    if (useeit)
			pline("%s is turned to stone!", Monnam(mtmp));
		    stoned = TRUE;
		    killed(mtmp);

		    if (mtmp->mhp > 0) break;
		    return 2;
		}
		if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		    (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !rn2(15) ) {
		    You("meet %s gaze.", s_suffix(mon_nam(mtmp)));
		    stop_occupation();
		    if(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			break;
		    /*You("turn to stone...");
		    killer_format = KILLED_BY;
		    killer = mtmp->data->mname;
		    done(STONING);*/
		    You("start turning to stone...");
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("But you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = "petrifying gaze";
				}
			}
		}
		if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) && !rn2(15) && (!Stone_resistance || !rn2(StrongStone_resistance ? 100 : 20)) ) {
			pline("Sharp-edged stones slit you!");
			if (Upolyd) {u.mhmax--; if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax--; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }
		}
		break;

	    case AD_HEAL:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(5) ) ) {
		pline("%s casts a healing spell!", Monnam(mtmp));
		if(!uwep && (!uarmu || (uarmu && uarmu->oartifact == ART_GIVE_ME_STROKE__JO_ANNA)) && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    boolean goaway = FALSE;
		    pline("You are healed.");
		    reducesanity(1);
		    if (Upolyd) {
			u.mh += rnd(7);
			if (!rn2(7)) {
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			    u.cnd_nursehealamount++;
			    if (!rn2(13)) goaway = TRUE;
			}
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			if (!rn2(10)) {
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10)) {
				u.uhpmax++;
				u.cnd_nursehealamount++;
			    }
			    if (!rn2(10)) goaway = TRUE;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    }
		    if (!rn2(3)) exercise(A_STR, TRUE);
		    if (!rn2(3)) exercise(A_CON, TRUE);
		    if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    flags.botl = 1;
		    if (goaway) {
			mongone(mtmp);
			return 2;
		    } else if (!rn2(15)) {
			if (!tele_restrict(mtmp) || !rn2(5) ) (void) rloc(mtmp, FALSE); /* sometimes ignore noteleport --Amy */
			monflee(mtmp, d(3, 6), TRUE, FALSE);
			return 3;
		    }
		    dmgplus = 0;
		} else {
		    if (Role_if(PM_HEALER) || Race_if(PM_HERBALIST)) {
			dmgplus = 0;
		    } else {
			pline("The healing spell hurts you...");
			mdamageu(mtmp, dmgplus);
		    }
		}
		}
		break;

	    case AD_CURS:
	    case AD_LITE:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5) ) )
 		{
		pline("%s gives you a mean look!", Monnam(mtmp));
		    stop_occupation();

			if(!rn2(10) || (night() && !rn2(3)) )  {
			    if (u.umonnum == PM_CLAY_GOLEM) {
				pline("Some writing vanishes from your head!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0;
				rehumanize();
				break;
			    }
				pline("%s laughs fiendishly!", Monnam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Kho-khe-khe-khe-khe! Tip bloka l'da katitsya po polu ot smekha, v to vremya kak vy tol'ko chto poteryali drugoy vnutrenney i, veroyatno, poteryayet gorazdo bol'she, potomu chto smotrel monstr prodolzhayet atakovat' izdaleka." : "Haehaehaehaehaehaehae-ae-ae-ae-ae!");
			    attrcurse();
			}
		}
		break;

	    case AD_SPC2:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(7)) )
 		{
			char visageword[BUFSZ]; /* from ToME */
			strcpy(visageword, "bad"); /* fail safe --Amy */

			if (!FunnyHallu) switch(rnd(20)) {

				case 1:
					strcpy(visageword, "abominable");
					break;
				case 2:
					strcpy(visageword, "abysmal");
					break;
				case 3:
					strcpy(visageword, "appalling");
					break;
				case 4:
					strcpy(visageword, "baleful");
					break;
				case 5:
					strcpy(visageword, "blasphemous");
					break;
				case 6:
					strcpy(visageword, "disgusting");
					break;
				case 7:
					strcpy(visageword, "dreadful");
					break;
				case 8:
					strcpy(visageword, "filthy");
					break;
				case 9:
					strcpy(visageword, "grisly");
					break;
				case 10:
					strcpy(visageword, "hideous");
					break;
				case 11:
					strcpy(visageword, "hellish");
					break;
				case 12:
					strcpy(visageword, "horrible");
					break;
				case 13:
					strcpy(visageword, "infernal");
					break;
				case 14:
					strcpy(visageword, "loathsome");
					break;
				case 15:
					strcpy(visageword, "nightmarish");
					break;
				case 16:
					strcpy(visageword, "repulsive");
					break;
				case 17:
					strcpy(visageword, "sacrilegious");
					break;
				case 18:
					strcpy(visageword, "terrible");
					break;
				case 19:
					strcpy(visageword, "unclean");
					break;
				case 20:
					strcpy(visageword, "unspeakable");
					break;

			} else switch(rnd(22)) {

				case 1:
					strcpy(visageword, "silly");
					break;
				case 2:
					strcpy(visageword, "hilarious");
					break;
				case 3:
					strcpy(visageword, "absurd");
					break;
				case 4:
					strcpy(visageword, "insipid");
					break;
				case 5:
					strcpy(visageword, "ridiculous");
					break;
				case 6:
					strcpy(visageword, "laughable");
					break;
				case 7:
					strcpy(visageword, "ludicrous");
					break;
				case 8:
					strcpy(visageword, "far-out");
					break;
				case 9:
					strcpy(visageword, "groovy");
					break;
				case 10:
					strcpy(visageword, "postmodern");
					break;
				case 11:
					strcpy(visageword, "fantastic");
					break;
				case 12:
					strcpy(visageword, "dadaistic");
					break;
				case 13:
					strcpy(visageword, "cubistic");
					break;
				case 14:
					strcpy(visageword, "cosmic");
					break;
				case 15:
					strcpy(visageword, "awesome");
					break;
				case 16:
					strcpy(visageword, "incomprehensible");
					break;
				case 17:
					strcpy(visageword, "fabulous");
					break;
				case 18:
					strcpy(visageword, "amazing");
					break;
				case 19:
					strcpy(visageword, "incredible");
					break;
				case 20:
					strcpy(visageword, "chaotic");
					break;
				case 21:
					strcpy(visageword, "wild");
					break;
				case 22:
					strcpy(visageword, "preposterous");
					break;

			}

		pline("You behold the %s visage of %s!", visageword, mon_nam(mtmp));
		if (FunnyHallu && rn2(2) ) switch (rnd(5)) {

			case 1:
				pline("Wow, cosmic, man!");
				break;
			case 2:
				pline("Rad!");
				break;
			case 3:
				pline("Groovy!");
				break;
			case 4:
				pline("Cool!");
				break;
			case 5:
				pline("Far out!");
				break;

		}

		/* In ToME, hallucination completely prevents the effects "because you can't see the monster clearly enough".
		 * Here, allow hallu to prevent it most of the time, but we don't want the character to be completely immune. --Amy */
		if (Hallucination && rn2(3)) break;
		if (Psi_resist && rn2(StrongPsi_resist ? 100 : 20) ) break;

		    stop_occupation();

			switch (rnd(10)) {

				case 1:
				case 2:
				case 3:
					make_confused(HConfusion + dmgplus + 5, FALSE);
					break;
				case 4:
				case 5:
				case 6:
					make_stunned(HStun + dmgplus + 5, FALSE);
					break;
				case 7:
					make_confused(HConfusion + dmgplus + 5, FALSE);
					make_stunned(HStun + dmgplus + 5, FALSE);
					break;
				case 8:
					make_hallucinated(HHallucination + dmgplus + 5, FALSE, 0L);
					break;
				case 9:
					make_feared(HFeared + dmgplus + 5, FALSE);
					break;
				case 10:
					make_numbed(HNumbed + dmgplus + 5, FALSE);
					break;
	
			}
			if (!rn2(200)) {
				forget(rnd(5));
				pline("You forget some important things...");
			}
			if (!rn2(200) && (u.urmaxlvlUP >= 2)) {
				losexp("psionic drain", FALSE, TRUE);
			}
			if (!rn2(200)) {
				adjattrib(A_INT, -1, 1, TRUE);
				adjattrib(A_WIS, -1, 1, TRUE);
			}
			if (!rn2(200)) {
				pline("You scream in pain!");
				wake_nearby();
			}
			if (!rn2(200)) {
				badeffect();
			}
			if (!rn2(5)) increasesanity(rnz(5));

		}
		break;

	    case AD_MAGM:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
 		{
		pline("%s's eye color suddenly changes!", Monnam(mtmp));
		    stop_occupation();
		    if(Antimagic && !Race_if(PM_KUTAR) && !rn2(StrongAntimagic ? 3 : 2)) {
			shieldeff(u.ux, u.uy);
			pline("A hail of magic missiles narrowly misses you!");
		    } else {
			You("are hit by magic missiles appearing from thin air!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "To, chto vy ne magiya ustoychivy yeshche? Togda vasha smert' yavlyayetsya lish' voprosom vremeni. Pochemu by vam ne postavit' nekotoryye usiliya v nego i nachat' igrat' luchshe srazu?" : "Schiaeaeaeaeau!");
 	            if (rn2(4)) mdamageu(mtmp, d(4,6));
			else mdamageu(mtmp, (d(4,6) + dmgplus));
		    }
		}
	    break;
	    case AD_DISN:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(4)) )
 		{
		if (!rn2(20))  {
		pline("%s's gaze seems to drill right into you!", Monnam(mtmp));
		    stop_occupation();
		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable || (Stoned_chiller && Stoned)) {
                pline("You are unharmed!");
                break;

		} else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
		    break;
		} else if (uarmu) {
		    /* destroy shirt */
		    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
		    break;
		}

		if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "a died gaze", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}
	    return 1; /* lifesaved */

		}
		}
	      break;

	    case AD_VAPO:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(4)) )
 		{
		if (!rn2(20))  {
		pline("%s's gaze seems to vaporize you!", Monnam(mtmp));
		    stop_occupation();

		    int dmg = d(2,6);
		    if (!rn2(2)) dmg += dmgplus;
			if (!Disint_resistance) dmg *= 3;
		    if (dmg) mdamageu(mtmp, dmg);

		if (Disint_resistance && rn2(StrongDisint_resistance ? 1000 : 100) && !(evilfriday && (uarms || uarmc || uarm || uarmu))) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable || (Stoned_chiller && Stoned)) {
                pline("You are unharmed!");
                break;

		} else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMS)) (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    if (!(EDisint_resistance & W_ARMC)) (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    if (!(EDisint_resistance & W_ARM)) (void) destroy_arm(uarm);
		    break;
		} else if (uarmu) {
		    /* destroy shirt */
		    if (!(EDisint_resistance & W_ARMU)) (void) destroy_arm(uarmu);
		    break;
		}

		if (u.uhpmax > 20) {
			u.uhpmax -= rnd(20);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			losehp(rnz(100 + level_difficulty()), "vaporgaze died", KILLED_BY);
			break;
		} else {
			u.youaredead = 1;
			done(DIED);
			u.youaredead = 0;
		}
	    return 1; /* lifesaved */

		}
		}
	      break;

	    case AD_ACID:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
 		{

		if(!rn2(issoviet ? 2 : 3)) {
		pline("%s sends a terrifying gaze at you!", Monnam(mtmp));
		    stop_occupation();
		    if (Acid_resistance && rn2(StrongAcid_resistance ? 20 : 5)) {
			pline("You're covered in acid, but it seems harmless.");
		    } else {
			pline("You're covered in acid! It burns!");
			exercise(A_STR, FALSE);
		if (Stoned) fix_petrification();
		    int dmg = d(2,6);
		    if (!rn2(10)) dmg += dmgplus;
		    if (dmg) mdamageu(mtmp, dmg);
		    }
			if(!rn2(3)) erode_armor(&youmonst, TRUE);
		}
		}
		break;
	      case AD_DRLI:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
 		{
			if (!rn2(issoviet ? 3 : 7) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )  ) {
				pline("%s seems to drain your life with its gaze!", Monnam(mtmp));
		    stop_occupation();

				if (moves < 1000 && (u.urmaxlvlUP == 1) && rn2(25)) {
					pline("But it failed to drain you to death.");
					break;
				} else {
					losexp("life drainage", FALSE, TRUE);
				}
				if (!rn2(4)) mdamageu(mtmp, dmgplus);
			}
		}
		break;

	      case AD_VAMP:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
 		{
			if (!rn2(issoviet ? 2 : 3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )  ) {
				pline("%s seems to drain your life with its gaze!", Monnam(mtmp));
		    stop_occupation();

				if (moves < 1000 && (u.urmaxlvlUP == 1) && rn2(25)) {
					pline("But it failed to drain you to death.");
					break;
				} else {
					losexp("life drainage", FALSE, TRUE);
				}

				if (!rn2(2)) mdamageu(mtmp, dmgplus);
			}
		}
		break;

	    case AD_STCK:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
		{ 
			if (!u.ustuck && !(uwep && uwep->oartifact == ART_FOAMONIA_WATER) && !sticks(youmonst.data)) {
				setustuck(mtmp);
				pline("%s gazes to hold you in place!", Monnam(mtmp));
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
		}
		break;

	    case AD_WEBS:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(3)) )
 		{
			pline("%s asks 'How do I shot web?' and spits at you.", Monnam(mtmp));
		    stop_occupation();
		{
			struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB, 0, FALSE);
			if (ttmp2) {
				pline_The("webbing sticks to you. You're caught!");
				dotrap(ttmp2, NOWEBMSG);
				if (u.usteed && u.utrap && !mayfalloffsteed()) {
				/* you, not steed, are trapped */
				dismount_steed(DISMOUNT_FELL);
				}
			}
		}
		/* Amy addition: sometimes, also make a random trap somewhere on the level :D */
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap(FALSE);

		}
		break;

	    case AD_TRAP:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(3)) )
 		{
			pline("%s cackles fiendishly.", Monnam(mtmp));
			stop_occupation();
			if (t_at(u.ux, u.uy) == 0) (void) maketrap(u.ux, u.uy, randomtrap(), 0, FALSE);
			else makerandomtrap(FALSE);
		}
		break;

	    case AD_SAMU:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(20)) )
 		{
			pline("%s commands you to hand over your macguffins!", Monnam(mtmp));
			stop_occupation();
			stealamulet(mtmp);
		}
		break;

	    case AD_STTP:

		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(25) ) )
 		{
		pline("%s gazes at you and curses.", Monnam(mtmp));
		    stop_occupation();
		if (invent) {
		    int itemportchance = 10 + rn2(21);
		    for (otmpi = invent; otmpi; otmpi = otmpii) {

		      otmpii = otmpi->nobj;

			if (!rn2(itemportchance) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

				if (otmpi->owornmask & W_ARMOR) {
				    if (otmpi == uskin) {
					skinback(TRUE);		/* uarm = uskin; uskin = 0; */
				    }
				    if (otmpi == uarm) (void) Armor_off();
				    else if (otmpi == uarmc) (void) Cloak_off();
				    else if (otmpi == uarmf) (void) Boots_off();
				    else if (otmpi == uarmg) (void) Gloves_off();
				    else if (otmpi == uarmh) (void) Helmet_off();
				    else if (otmpi == uarms) (void) Shield_off();
				    else if (otmpi == uarmu) (void) Shirt_off();
				    /* catchall -- should never happen */
				    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
				} else if (otmpi ->owornmask & W_AMUL) {
				    Amulet_off();
				} else if (otmpi ->owornmask & W_IMPLANT) {
				    Implant_off();
				} else if (otmpi ->owornmask & W_RING) {
				    Ring_gone(otmpi);
				} else if (otmpi ->owornmask & W_TOOL) {
				    Blindf_off(otmpi);
				} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
				    if (otmpi == uwep)
					uwepgone();
				    if (otmpi == uswapwep)
					uswapwepgone();
				    if (otmpi == uquiver)
					uqwepgone();
				}

				if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
				    unpunish();
				} else if (otmpi->owornmask) {
				/* catchall */
				    setnotworn(otmpi);
				}

				dropx(otmpi);
			      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
				u.cnd_itemportcount++;
			}
		    }
		}
		}
		break;

	      case AD_DREN:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
 		{
			pline("%s seems to drain your energy with its gaze!", Monnam(mtmp));
		    stop_occupation();
			if (!rn2(issoviet ? 1 : 4)) {drain_en(10); if (!rn2(5)) drain_en(dmgplus);
			}
		}
		break;
	    case AD_NGRA:
		if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
 		{

		      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		/* This attack can remove any Elbereth engraving, even burned ones. --Amy */
			pline("%s seems to suck in the words engraved on the surface below you!", Monnam(mtmp));
		    stop_occupation();
		    del_engr(ep);
		    ep = (struct engr *)0;
			}
		}

		break;

	    case AD_GLIB:
		if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(10)) )
 		{

		/* hurt the player's hands --Amy */
		pline("%s telepathically twists your hands!", Monnam(mtmp));
		    stop_occupation();
		incr_itimeout(&Glib, dmgplus );

		}
		break;

	    case AD_DARK:
		if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(4)) )
 		{

		if (Race_if(PM_PLAYER_NIBELUNG) && rn2(5)) break;

		/* create darkness around the player --Amy */
		pline("%s's sinister gaze fills your mind with dreadful, evil thoughts!", Monnam(mtmp));
		if (PlayerHearsSoundEffects) pline(issoviet ? "Konechno, Sovetskiy sdelal eto tak, chto vy ne mozhete uvidet' bol'shinstvo monstrov pryamo seychas. Kha-kha-kha ..." : "Diedaedodiedaerr!");
		    stop_occupation();
		litroomlite(FALSE);
		}
		break;

	    case AD_LEGS:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5) ) )
 		{
			{ register long sideX = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
	
			pline("%s's gaze makes your legs turn to jelly!", Monnam(mtmp));
		    stop_occupation();
			set_wounded_legs(sideX, HWounded_legs + rnd(60-ACURR(A_DEX)));
			exercise(A_STR, FALSE);
			exercise(A_DEX, FALSE);
			}
		}
		break;

	    case AD_SLIM:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(15) ) )
 		{

		pline("%s hurls some disgusting green goo at you!", Monnam(mtmp));
		    stop_occupation();

		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		} else if (Unchanging || slime_on_touch(youmonst.data) ) {
		    You("are unaffected.");
		} else if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    char kbuf[BUFSZ];
		    sprintf(kbuf, "slimed by %s", mtmp->data->mname);
		    delayed_killer = kbuf;
		} else
		    pline("Yuck!");
		}
		break;
	    case AD_LITT:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(15) ) )
 		{

		pline("%s hurls a chemical bomb at you!", Monnam(mtmp));
		    stop_occupation();

		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		} else if (Unchanging || slime_on_touch(youmonst.data) ) {
		    You("are unaffected.");
		} else if (!Slimed) {
		    You("don't feel very well.");
			make_slimed(20);
		    killer_format = KILLED_BY_AN;
		    char kbuf[BUFSZ];
		    sprintf(kbuf, "slimed by %s", mtmp->data->mname);
		    delayed_killer = kbuf;
		} else
		    pline("Yuck!");

		{
		    register struct obj *littX, *littX2;
		    for (littX = invent; littX; littX = littX2) {
		      littX2 = littX->nobj;
			if (!rn2(StrongAcid_resistance ? 1000 : Acid_resistance ? 100 : 10)) rust_dmg(littX, xname(littX), 3, TRUE, &youmonst);
		    }
		}

		}

		break;
	    case AD_CALM:	/* KMH -- koala attack */
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
 		{
		pline("%s gazes at you softly.", Monnam(mtmp));
		    stop_occupation();
		    docalm();
		}
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(25)) )
 		{
		pline("%s gazes at your belongings!", Monnam(mtmp));
		    stop_occupation();
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		    struct obj *obj = some_armor(&youmonst);

		    if (obj && drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		    }
		}
		break;

	    case AD_NGEN:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(25)) )
 		{
		pline("%s curses the existence of your belongings!", Monnam(mtmp));
		    stop_occupation();
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		    struct obj *obj = some_armor(&youmonst);

		    if (obj && drain_item_severely(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		    }
		}
		break;

	    case AD_SHRD:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(25)) )
 		{
		pline("%s shoots shards at your belongings!", Monnam(mtmp));
		    stop_occupation();
		struct obj *obj = some_armor(&youmonst);

		if (obj && drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
			u.cnd_disenchantamount++;
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
		} else if (obj && rn2(3)) wither_dmg(obj, xname(obj), rn2(4), FALSE, &youmonst);

            if (!rn2(3)) mdamageu(mtmp, (1 + dmgplus));
		}

		break;

	    case AD_POLY:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(20)) )
 		{
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			pline("%s throws a changing gaze at you!", Monnam(mtmp));
		    stop_occupation();
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
		    polyself(FALSE);
			}
		}
		break;

	    case AD_CHAO:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(20)) )
 		{

			pline("%s invokes a raw chaos!", Monnam(mtmp));

		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
		    stop_occupation();
			u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
		    polyself(FALSE);
			}
		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: diseasemu(mtmp->data);
			    break;
		    case 1: make_blinded(Blinded + dmgplus, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + dmgplus, TRUE);
			    break;
		    case 3: make_stunned(HStun + dmgplus, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + dmgplus, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + dmgplus, TRUE);
			    break;
		    case 6: make_burned(HBurned + dmgplus, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + dmgplus, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + dmgplus, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + dmgplus, TRUE);
			    break;
		    }
		}

		break;

	    case AD_FAKE:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee)
 		{
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
			if (!rn2(3)) {
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
			}
		}
		break;

	    case AD_CONF:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int conf = d(3,4);

		    mtmp->mspec_used = mtmp->mspec_used + (conf + rn2(6));
		    if(!Confusion)
			pline("%s gaze confuses you!",
			                  s_suffix(Monnam(mtmp)));
		    else
			You("are getting more and more confused.");
		    make_confused(HConfusion + conf, FALSE);
		    if (!rn2(4)) make_confused(HConfusion + dmgplus, FALSE);
		    stop_occupation();
		}
		break;
	    case AD_FAMN:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(4))) 		{
		pline("%s gazes at you with its hungry eyes!", Monnam(mtmp));
		    stop_occupation();
		exercise(A_CON, FALSE);
		if (!is_fainted()) {
			morehungry(rnz(40));
			if (!rn2(5)) morehungry(dmgplus);
		}
		if (!is_fainted()) {
			morehungry(rnz(40));
			if (!rn2(5)) morehungry(dmgplus);
		}
		/* plus the normal damage */
		}
		break;

	    case AD_DEPR:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(12))) 		{
		pline("%s gazes at you with depressing sorrow in its eyes!", Monnam(mtmp));
		    stop_occupation();

		if (!rn2(3)) {

		    switch(rnd(20)) {
		    case 1:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += 1;
			      polyself(FALSE);
			}
			break;
		    case 2:
			You("need reboot.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
			if (!Race_if(PM_UNGENOMOLD)) newman();
			else polyself(FALSE);
			break;
		    case 3: case 4:
			if(!rn2(4) && u.ulycn == NON_PM &&
				!Protection_from_shape_changers &&
				!is_were(youmonst.data) &&
				!defends(AD_WERE,uwep)) {
			    You_feel("feverish.");
			    exercise(A_CON, FALSE);
			    u.ulycn = PM_WERECOW;
			    u.cnd_lycanthropecount++;
			} else {
				if (multi >= 0) {
				    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) break;
				    fall_asleep(-rnd(10), TRUE);
				    if (Blind) You("are put to sleep!");
				    else You("are put to sleep by %s!", mon_nam(mtmp));
				}
			}
			break;
		    case 5: case 6:
			if (!u.ustuck && !sticks(youmonst.data)) {
				setustuck(mtmp);
				pline("%s grabs you!", Monnam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
			break;
		    case 7:
		    case 8:
			Your("position suddenly seems very uncertain!");
			teleX();
			break;
		    case 9:
			u_slow_down();
			break;
		    case 10:
			hurtarmor(AD_RUST);
			break;
		    case 11:
			hurtarmor(AD_DCAY);
			break;
		    case 12:
			hurtarmor(AD_CORR);
			break;
		    case 13:
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(5), "paralyzed by a monster attack", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 14:
			if (FunnyHallu)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			hallutime = rn1(7, 16);
			make_stunned(HStun + hallutime + dmgplus, FALSE);
			(void) make_hallucinated(HHallucination + hallutime + dmgplus,TRUE,0L);
			break;
		    case 15:
			if(!Blind)
				Your("vision bugged.");
			hallutime += rn1(10, 25);
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + dmgplus + dmgplus,TRUE,0L);
			break;
		    case 16:
			if(!Blind)
				Your("vision turns to screen saver.");
			hallutime += rn1(10, 25);
			(void) make_hallucinated(HHallucination + hallutime + dmgplus,TRUE,0L);
			break;
		    case 17:
			{
			    struct obj *obj = some_armor(&youmonst);

			    if (obj && drain_item(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
				u.cnd_disenchantamount++;
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
			    }
			}
			break;
		    default:
			    if(Confusion)
				 You("are getting even more confused.");
			    else You("are getting confused.");
			    make_confused(HConfusion + dmgplus, FALSE);
			break;
		    }
		    exercise(A_INT, FALSE);

		}
		}
		break;

	    case AD_WRAT:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(4))) 		{
		pline("%s gazes at you with its angry eyes!", Monnam(mtmp));
		    stop_occupation();

		if(u.uen < 1) {
		    You_feel("less energised!");
		    u.uenmax -= rn1(10,10);
		    if(u.uenmax < 0) u.uenmax = 0;
		} else if(u.uen <= 10) {
		    You_feel("your magical energy dwindle to nothing!");
		    u.uen = 0;
		} else {
		    You_feel("your magical energy dwindling rapidly!");
		    u.uen /= 2;
		}
		}

		break;

	    case AD_LAZY: /* laziness attack; do lots of nasty things at random */

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(4))) 		{
		pline("%s gazes at you with its apathetic eyes!", Monnam(mtmp));
		    stop_occupation();

		if(!rn2(2)) {
		    pline("Nothing seems to happen.");
		    break;
		}
		switch(rn2(7)) {
		    case 0: /* destroy certain things */
			pline("%s touches you!", Monnam(mtmp));
			witherarmor();
			break;
		    case 1: /* sleep */
			if (multi >= 0) {
			    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) {pline("You yawn."); break;}
			    fall_asleep(-rnd(10), TRUE);
			    if (Blind) You("are put to sleep!");
			    else You("are put to sleep by %s!", mon_nam(mtmp));
			}
			break;
		    case 2: /* paralyse */
			if (multi >= 0) {
			    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(5), "paralyzed by a monster attack", TRUE);
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 3: /* slow */
			if(HFast)  u_slow_down();
			else You("pause momentarily.");
			break;
		    case 4: /* drain Dex */
			adjattrib(A_DEX, -rn1(1,1), 0, TRUE);
			break;
		    case 5: /* steal teleportitis */
			if(HTeleportation & INTRINSIC) {
			      HTeleportation &= ~INTRINSIC;
			}
	 		if (HTeleportation & TIMEOUT) {
				HTeleportation &= ~TIMEOUT;
			}
			if(HTeleport_control & INTRINSIC) {
			      HTeleport_control &= ~INTRINSIC;
			}
	 		if (HTeleport_control & TIMEOUT) {
				HTeleport_control &= ~TIMEOUT;
			}
		      You("don't feel in the mood for jumping around.");
			break;
		    case 6: /* steal sleep resistance */
			if(HSleep_resistance & INTRINSIC) {
				HSleep_resistance &= ~INTRINSIC;
			} 
			if(HSleep_resistance & TIMEOUT) {
				HSleep_resistance &= ~TIMEOUT;
			} 
			You_feel("like you could use a nap.");
			break;
		}
		}
		break;

	    case AD_DFOO:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(4))) 		{
		pline("%s gazes at you with its glistening eyes!", Monnam(mtmp));
		    stop_occupation();
		if (!rn2(3)) {
		    sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, rn2(A_MAX), mtmp->data->mname, 30);
		}
		if (!rn2(4)) {
			You_feel("drained...");
			u.uhpmax -= rn1(10,10);
			if (u.uhpmax < 1) u.uhpmax = 1;
			if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (!rn2(4)) {
			You_feel("less energised!");
			u.uenmax -= rn1(10,10);
			if (u.uenmax < 0) u.uenmax = 0;
			if(u.uen > u.uenmax) u.uen = u.uenmax;
		}
		if (!rn2(4)) {
			if((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && (u.urmaxlvlUP >= 2) )
			    losexp("life drainage", FALSE, TRUE);
			else You_feel("woozy for an instant, but shrug it off.");
		}
		}
		break;

	    case AD_SGLD:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)))
 		{
			pline("%s uses a telepathic gaze!", Monnam(mtmp));
		    stop_occupation();
			if (!issoviet && !rn2(3)) {
				You_feel("a tug on your purse"); break;
			}
			if (rn2(10)) stealgold(mtmp);
			else if( (rnd(100) > ACURR(A_CHA)) &&  !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && /*!rn2(25)*/ 
		( ((mtmp->female) && !flags.female && !rn2(5) ) || ((!mtmp->female) && flags.female && !rn2(15) ) || 
			((mtmp->female) && flags.female && !rn2(25) ) || ((!mtmp->female) && !flags.female && !rn2(25) ) ) )
			{
				buf[0] = '\0';
				switch (steal(mtmp, buf, FALSE, FALSE)) {
			  case -1:
				return 2;
			  case 0:
				break;
			  default:
				if ( !tele_restrict(mtmp) && !rn2(4))
				    (void) rloc(mtmp, FALSE);
				monflee(mtmp, rnd(10), FALSE, FALSE);
				return 3;
				};

			}
		}
		break;

	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
	    case AD_SSEX:
		if (!rn2(3) && !issoviet && !(u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) || have_stealerstone() || (uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) ) && canseemon(mtmp) && mtmp->mcansee ) break; /* no message, we don't want too much spam --Amy */

		if ((u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) || have_stealerstone() || (uwep && uwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_COPPERED_OFF_FROM_ME) || (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) ) && !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee) {
		pline("%s gazes at you with its demanding eyes!", Monnam(mtmp));
		    stop_occupation();
		buf[0] = '\0';
			switch (steal(mtmp, buf, atttypB == AD_SEDU ? TRUE : FALSE, FALSE)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp) && (issoviet || atttypB == AD_SEDU || !rn2(4)) )
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, rnd(10), FALSE, FALSE);
			return 3;
			};

		} else if ( (issoviet && !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee) || ( (rnd(100) > ACURR(A_CHA)) &&  !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && /*!rn2(25)*/ 
		( ((mtmp->female) && !flags.female && !rn2(5) ) || ((!mtmp->female) && flags.female && !rn2(15) ) || 
			((mtmp->female) && flags.female && !rn2(25) ) || ((!mtmp->female) && !flags.female && !rn2(25) ) ) )

		) 		{
		pline("%s gazes at you with its demanding eyes!", Monnam(mtmp));
		    stop_occupation();
		buf[0] = '\0';
			switch (steal(mtmp, buf, atttypB == AD_SEDU ? TRUE : FALSE, FALSE)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp) && (issoviet || atttypB == AD_SEDU || !rn2(4)) )
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, rnd(10), FALSE, FALSE);
			return 3;
			};
		}
		break;
	    case AD_RUST:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(5))) 		{
		pline("%s squirts water at you!", Monnam(mtmp));
		    stop_occupation();

		if (u.umonnum == PM_IRON_GOLEM) {
			You("rust!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		}
		hurtarmor(AD_RUST);
		}
		break;

	    case AD_AMNE:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(5))) {
			pline("%s yells 'MAUD MAUD MAUD MAUD!'", Monnam(mtmp));
			maprot();
		}

		break;

	    case AD_LETH:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(50))) 		{
		pline("%s squirts sparkling water at you!", Monnam(mtmp));
		    stop_occupation();
		if (!rn2(3)) {
			pline("You sparkle!");
			lethe_damage(invent, FALSE, FALSE);
			if (!rn2(3)) actual_lethe_damage(invent, FALSE, FALSE);
			if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
			if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		}
		}
		break;

	    case AD_WET:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(50))) 		{
		pline("%s squirts cold water at you!", Monnam(mtmp));
		    stop_occupation();
		if (!rn2(3)) {
			pline("You're very wet!");
			if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
				water_damage(invent, FALSE, FALSE);
				if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
			}
		}
		}
		break;

	    case AD_SUCK:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(20))) 		{
		pline("%s uses a vacuum cleaner on you! Or is that a gluon gun?", Monnam(mtmp));
		    stop_occupation();

			if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) dmgplus = 0;
			else{
				if( has_head(youmonst.data) && !(Role_if(PM_COURIER)) && !uarmh && !rn2(20) && 
					((!Upolyd && u.uhp < (u.uhpmax / 10) ) || (Upolyd && u.mh < (u.mhmax / 10) ))
				){
					dmgplus += 2 * (Upolyd ? u.mh : u.uhp)
						  + 400; //FATAL_DAMAGE_MODIFIER;
					pline("%s sucks your %s off!",
					      Monnam(mtmp), body_part(HEAD));
				}
				else{
					You_feel("%s trying to suck your extremities off!",mon_nam(mtmp));
					if(!rn2(10)){
						Your("%s twist from the suction!", makeplural(body_part(LEG)));
					    set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    set_wounded_legs(LEFT_SIDE, HWounded_legs + rnd(60-ACURR(A_DEX)));
					    exercise(A_STR, FALSE);
					    exercise(A_DEX, FALSE);
					}
					if(uwep && !rn2(6)){
						You_feel("%s pull on your weapon!",mon_nam(mtmp));
						if( rnd(130) > ACURR(A_STR)){
							Your("weapon is sucked out of your grasp!");
							optr = uwep;
							uwepgone();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						}
						else{
							You("keep a tight grip on your weapon!");
						}
					}
					if(!rn2(10) && uarmf){
						Your("boots are sucked off!");
						optr = uarmf;
						if (donning(optr)) cancel_don();
						(void) Boots_off();
						freeinv(optr);
						(void) mpickobj(mtmp,optr,FALSE);
					}
					if(!rn2(6) && uarmg && !uwep){
						You_feel("%s pull on your gloves!",mon_nam(mtmp));
						if( rnd(130) > ACURR(A_STR)){
							Your("gloves are sucked off!");
							optr = uarmg;
							if (donning(optr)) cancel_don();
							(void) Gloves_off();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						}
						else You("keep your %s closed.", makeplural(body_part(HAND)));
					}
					if(!rn2(8) && uarms){
						You_feel("%s pull on your shield!",mon_nam(mtmp));
						if( rnd(150) > ACURR(A_STR)){
							Your("shield is sucked out of your grasp!");
							optr = uarms;
							if (donning(optr)) cancel_don();
							Shield_off();
							freeinv(optr);
							(void) mpickobj(mtmp,optr,FALSE);
						 }
						 else{
							You("keep a tight grip on your shield!");
						 }
					}
					if(!rn2(4) && uarmh){
						Your("helmet is sucked off!");
						optr = uarmh;
						if (donning(optr)) cancel_don();
						(void) Helmet_off();
						freeinv(optr);
						(void) mpickobj(mtmp,optr,FALSE);
					}
				}
			}
			if (dmgplus) mdamageu(mtmp, (d(4,6) + dmgplus));
		}
		break;

	    case AD_CNCL:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(50))) 		{
		pline("%s throws a blinky gaze at you!", Monnam(mtmp));
		    stop_occupation();
		if (!rn2(3)) {
			(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		}
		}
		break;

	    case AD_BANI:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(100))) 		{
		if (!rn2(3)) {
			if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds_normalterrain(FALSE); break;}
			if (playerlevelportdisabled()) {
				pline("For some reason you resist the banishment!");
				break;
			}

			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.banishmentbeam) {
				u.banishmentbeam = 1;
				nomul(-2, "being banished", FALSE); /* because it's not called until you get another turn... */
			}
		}
		}
		break;

	    case AD_WEEP:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(40))) 		{
		/* if vampire biting (and also a pet) */
		if (!rn2(3) && (!u.uevent.udemigod || u.freeplaymode) && !playerlevelportdisabled() ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
			if (!u.levelporting) {
				u.levelporting = 1;
				nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
			}
		}
		else if (!rn2(3) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) ) && (u.urmaxlvlUP >= 2) ) {
		    losexp("loss of potential", FALSE, TRUE);
		}
		}
		break;

	    case AD_DCAY:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(5))) 		{
		pline("%s flings organic matter at you!", Monnam(mtmp));
		    stop_occupation();

		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("rot!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			u.mh = 0;
			/* KMH, balance patch -- this is okay with unchanging */
			rehumanize();
			break;
		}
		hurtarmor(AD_DCAY);
		}
		break;
	    case AD_FLAM:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(5))) 		{
		pline("%s shoots a flamethrower at you!", Monnam(mtmp));
		    stop_occupation();

		hurtarmor(AD_FLAM);
		}
		break;
	    case AD_CORR:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(5))) 		{
		pline("%s throws corrosive stuff at you!", Monnam(mtmp));
		    stop_occupation();
		hurtarmor(AD_CORR);
		}
		break;
	    case AD_WTHR:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(5))) 		{
		pline("%s telepathically messes with your clothes!", Monnam(mtmp));
		    stop_occupation();
		witherarmor();
		}
		break;
	    case AD_LUCK:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(20))) 		{
		pline("%s's terrifying gaze makes you feel like you'll never be able to experience luck again!", Monnam(mtmp));
		    stop_occupation();
		change_luck(-1);
		}
		break;
	    case AD_STUN:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int stun = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (stun + rn2(6));
		    pline("%s stares piercingly at you!", Monnam(mtmp));
		    make_stunned(HStun + stun, TRUE);
		    if (!rn2(4)) make_stunned(HStun + dmgplus, FALSE);
		    stop_occupation();
		}
		break;
	    case AD_NUMB:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int numb = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (numb + rn2(6));
		    pline("%s stares numbingly at you!", Monnam(mtmp));
		    make_numbed(HNumbed + numb, TRUE);
		    if (!rn2(4)) make_numbed(HNumbed + dmgplus, FALSE);
		    stop_occupation();
		}
		break;

	    case AD_FRZE:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(25))) {
		    int frze = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (frze + rn2(6));
		    pline("%s stares freezingly at you!", Monnam(mtmp));
		    make_frozen(HFrozen + frze, TRUE);
		    if (!rn2(4)) make_frozen(HFrozen + dmgplus, FALSE);
		    stop_occupation();
		}
		break;

	    case AD_BURN:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int burn = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (burn + rn2(6));
		    pline("%s stares burningly at you!", Monnam(mtmp));
		    make_burned(HBurned + burn, TRUE);
		    if (!rn2(4)) make_burned(HBurned + dmgplus, FALSE);
		    stop_occupation();
		}
		break;

	    case AD_DIMN:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(3))) {
		    int dimming = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (dimming + rn2(6));
		    pline("%s stares dimmingly at you!", Monnam(mtmp));
		    make_dimmed(HDimmed + dimming, TRUE);
		    if (!rn2(4)) make_dimmed(HDimmed + dmgplus, FALSE);
		    stop_occupation();
		}
		break;

	    case AD_FEAR:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int fearing = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (fearing + rn2(6));
		    pline("%s stares terrifyingly at you!", Monnam(mtmp));
		    make_feared(HFeared + fearing, TRUE);
		    if (!rn2(4)) make_feared(HFeared + dmgplus, FALSE);
		    stop_occupation();
		}
		break;

	    case AD_SANI:
		if(!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {

			switch (rnd(4)) {

				case 1:
					pline("You see %s chow dead bodies.", mon_nam(mtmp)); break;
				case 2:
					pline("You shudder at %s's terrifying %s.", mon_nam(mtmp), makeplural(mbodypart(mtmp, EYE)) ); break;
				case 3:
					pline("You feel sick at entrails caught in %s's tentacles.", mon_nam(mtmp)); break;
				case 4:
					pline("You see maggots breed in the rent %s of %s.", mbodypart(mtmp, STOMACH), mon_nam(mtmp)); break;

			}

			increasesanity(dmgplus);
		}
		break;

	    case AD_INSA:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int insanity = d(2,6);
		    if (!rn2(3)) insanity += dmgplus;
		    pline("%s gazes at you with a facial expression of insanity!", Monnam(mtmp));
		    make_feared(HFeared + insanity, TRUE);
		    make_stunned(HStun + insanity, TRUE);
		    if(Confusion) You("are getting even more confused.");
		    else You("are getting confused.");
		    make_confused(HConfusion + insanity, FALSE);
		    increasesanity(1);
		}
		break;

	    case AD_BLND:
		if (!mtmp->mcan && canseemon(mtmp) && !resists_blnd(&youmonst)
			&& distu(mtmp->mx,mtmp->my) <= BOLT_LIM*BOLT_LIM && (issoviet || !rn2(6)) ) {
		    int blnd = d((int)mattk->damn, (int)mattk->damd);
		    if (MaximumDamageBug || u.uprops[MAXIMUM_DAMAGE_BUG].extrinsic || have_maximumdamagestone() || (uwep && uwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWILLSCHWILLSCHWILLSCHWI)) blnd = (int)mattk->damn * (int)mattk->damd;

		    if (FunnyHallu) pline("The power of %s aurora overwhelms you!", s_suffix(mon_nam(mtmp)));
		    else You("are blinded by %s radiance!", s_suffix(mon_nam(mtmp)));

		    make_blinded((long)blnd,FALSE);
		    stop_occupation();
		    /* not blind at this point implies you're wearing
		       the Eyes of the Overworld; make them block this
		       particular stun attack too */
		    if (!Blind) Your("%s", vision_clears);
		    else make_stunned((long)d(1,3),TRUE);
		}
		break;
	    case AD_FIRE:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int dmg = d(2,6);
		    if (!rn2(10)) dmg += dmgplus;

		    pline("%s attacks you with a fiery gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) {
			pline_The("fire doesn't feel hot!");
			dmg = 0;
		    }
		    burn_away_slime();
		    /*if ((int) mtmp->m_lev > rn2(20))
			destroy_item(SCROLL_CLASS, AD_FIRE);
		    if ((int) mtmp->m_lev > rn2(20))
			destroy_item(POTION_CLASS, AD_FIRE);
		    if ((int) mtmp->m_lev > rn2(25))
			destroy_item(SPBOOK_CLASS, AD_FIRE);*/
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);

		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;

	    case AD_NEXU:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(25))) {
		    pline("%s attacks you with a nether gaze!", Monnam(mtmp));
		    stop_occupation();
		int dmg = dmgplus;

		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) dmg *= (1 + rnd(2));

		switch (rnd(7)) {

			case 1:
			case 2:
			case 3:
				pline("%s sends you far away!", Monnam(mtmp) );
				teleX();
				break;
			case 4:
			case 5:
				pline("%s sends you away!", Monnam(mtmp) );
				phase_door(0);
				break;
			case 6:

				if ((!u.uevent.udemigod || u.freeplaymode) && !playerlevelportdisabled() ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
					}
				}
				break;
			case 7:
				{
					nexus_swap();

					if (!rn2(3)) {

						int reducedstat = rn2(A_MAX);
						if(ABASE(reducedstat) <= ATTRMIN(reducedstat)) {
							pline("Your health was damaged!");
							u.uhpmax -= rnd(5);
							if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
							if (u.uhp < 1) {
								u.youaredead = 1;
								killer = "nexus scrambling";
								killer_format = KILLED_BY;
								done(DIED);
								u.youaredead = 0;
							}

						} else {
							ABASE(reducedstat) -= 1;
							AMAX(reducedstat) -= 1;
							flags.botl = 1;
							pline("Your attributes were damaged!");
						}
					}
				}
				break;
		}
	      if (dmg) mdamageu(mtmp, dmg);
		}
		break;

	    case AD_SOUN:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(10))) {
		    pline("%s sends a deafening wave of sound in your direction!", Monnam(mtmp));
		    stop_occupation();
		if (YouAreDeaf) dmgplus /= 2;
		make_stunned(HStun + dmgplus, TRUE);
		if (isevilvariant || !rn2(issoviet ? 2 : 5)) (void)destroy_item(POTION_CLASS, AD_COLD);
		wake_nearby();
		}
		break;

	    case AD_GRAV:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(25))) {
		    pline("%s wiggles a %s, and suddenly you stand upside down...", Monnam(mtmp), mbodypart(mtmp, FINGER) );
		    stop_occupation();

		if (level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout || (u.uhave.amulet && !u.freeplaymode) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) dmgplus *= 2;

		phase_door(0);
		pushplayer(FALSE);
		u.uprops[DEAC_FAST].intrinsic += (dmgplus + 2);
		make_stunned(HStun + dmgplus, TRUE);
	      if (dmgplus) mdamageu(mtmp, dmgplus);
		}
		break;

	    case AD_WGHT:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(10))) {
		    pline("%s throws a metal object into your trouser pocket and hits!", Monnam(mtmp) );
		    stop_occupation();
		    IncreasedGravity += (1 + (dmgplus * rnd(20)));
		}

		break;

	    case AD_INER:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(35))) {
		    pline("%s gazes at you, and your body doesn't feel like moving around anymore...", Monnam(mtmp));
		    stop_occupation();
	      u_slow_down();
		u.uprops[DEAC_FAST].intrinsic += ((dmgplus + 2) * 10);
		pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
		u.inertia += (dmgplus + 2);
		}
		break;

	    case AD_TIME:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_TIMEAGE_OF_REALMS) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(50))) {
		    pline("%s gazes at you, and sucks the essence of life out of you...", Monnam(mtmp));
		    stop_occupation();

		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Zhizn' razgonyal nazad, potomu chto vy ne smotreli, i teper' vy dolzhny poluchit', chto poteryannyy uroven' nazad." : "Kloeck!");
			      if (u.urmaxlvlUP >= 2) losexp("time", FALSE, FALSE); /* resistance is futile :D */
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				switch (rnd(A_MAX)) {
					case A_STR:
						pline("You're not as strong as you used to be...");
						ABASE(A_STR) -= 5;
						if(ABASE(A_STR) < ATTRMIN(A_STR)) {ABASE(A_STR) = ATTRMIN(A_STR);}
						break;
					case A_DEX:
						pline("You're not as agile as you used to be...");
						ABASE(A_DEX) -= 5;
						if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {ABASE(A_DEX) = ATTRMIN(A_DEX);}
						break;
					case A_CON:
						pline("You're not as hardy as you used to be...");
						ABASE(A_CON) -= 5;
						if(ABASE(A_CON) < ATTRMIN(A_CON)) {ABASE(A_CON) = ATTRMIN(A_CON);}
						break;
					case A_WIS:
						pline("You're not as wise as you used to be...");
						ABASE(A_WIS) -= 5;
						if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {ABASE(A_WIS) = ATTRMIN(A_WIS);}
						break;
					case A_INT:
						pline("You're not as bright as you used to be...");
						ABASE(A_INT) -= 5;
						if(ABASE(A_INT) < ATTRMIN(A_INT)) {ABASE(A_INT) = ATTRMIN(A_INT);}
						break;
					case A_CHA:
						pline("You're not as beautiful as you used to be...");
						ABASE(A_CHA) -= 5;
						if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {ABASE(A_CHA) = ATTRMIN(A_CHA);}
						break;
				}
				break;
			case 10:
				pline("You're not as powerful as you used to be...");
				ABASE(A_STR)--;
				ABASE(A_DEX)--;
				ABASE(A_CON)--;
				ABASE(A_WIS)--;
				ABASE(A_INT)--;
				ABASE(A_CHA)--;
				if(ABASE(A_STR) < ATTRMIN(A_STR)) {ABASE(A_STR) = ATTRMIN(A_STR);}
				if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {ABASE(A_DEX) = ATTRMIN(A_DEX);}
				if(ABASE(A_CON) < ATTRMIN(A_CON)) {ABASE(A_CON) = ATTRMIN(A_CON);}
				if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {ABASE(A_WIS) = ATTRMIN(A_WIS);}
				if(ABASE(A_INT) < ATTRMIN(A_INT)) {ABASE(A_INT) = ATTRMIN(A_INT);}
				if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {ABASE(A_CHA) = ATTRMIN(A_CHA);}
				break;
		}
		}
		break;

	    case AD_PLAS:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(15))) {

			    pline("%s attacks you with a plasma gaze!", Monnam(mtmp));
		    int dmg = dmgplus;
		    stop_occupation();
			if (!Fire_resistance) dmg *= 2;
			if (StrongFire_resistance && dmg > 1) dmg /= 2;

		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) /* extremely hot - very high chance to burn items! --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
			make_stunned(HStun + dmg, TRUE);
	      if (dmg) mdamageu(mtmp, dmg);

		}
		break;

	    case AD_MANA:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(20))) {
		    pline("%s attacks you with a mana gaze, the damage of which is completely unresistable!", Monnam(mtmp));
		    stop_occupation();
		drain_en(dmgplus);
	      if (dmgplus) mdamageu(mtmp, dmgplus);
		}
		break;

	    case AD_SKIL:
		if (!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		  mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(10))) {
		    pline("%s tries to drain your skills with its gaze!", Monnam(mtmp));
		    stop_occupation();
		    skillcaploss();
		}
		break;

	    case AD_TDRA:
		if (!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		  mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(10))) {
		    pline("%s tries to drain your techniques with its gaze!", Monnam(mtmp));
		    stop_occupation();
		    techdrain();
		}
		break;

	    case AD_DROP:
		if (!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		  mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(25))) {
		    pline("%s tries to make you trip!", Monnam(mtmp));
		    dropitemattack();
		    techdrain();
		}
		break;

	    case AD_BLAS:
		if (!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		  mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(10))) {
		    pline("%s tries to rile up the gods against you!", Monnam(mtmp));
		    stop_occupation();
			if (!rn2(25)) {
				u.ugangr++;
			      You("get the feeling that %s is angry...", u_gname());
			}
		}
		break;

	    case AD_LAVA:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int dmg = d(3,6);
		    if (!rn2(3)) dmg += dmgplus;

		    pline("%s attacks you with a really hot gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) {
			pline_The("fire doesn't feel hot!");
			dmg = 0;
		    }
		    burn_away_slime();
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 4 : 20)) /* hotter than ordinary fire attack, so more likely to burn items --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 4 : 20))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);

		    if (dmg) mdamageu(mtmp, dmg);
		    if (evilfriday || !rn2(5)) hurtarmor(AD_LAVA);
		}
		break;

	    case AD_AXUS:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int dmg = d(2,6);
		    if (!rn2(10)) dmg += dmgplus;

		    pline("%s attacks you with a multicolor gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Fire_resistance && rn2(StrongFire_resistance ? 20 : 5)) {
			pline_The("fire doesn't feel hot!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    burn_away_slime();
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (isevilvariant || !rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);

		    if (Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) {
			pline_The("cold doesn't freeze you!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);

		    if (Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) {
			pline_The("gaze doesn't shock you!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */
			destroy_item(AMULET_CLASS, AD_ELEC);

		    if (dmg) mdamageu(mtmp, dmg);

			if (!rn2(7) && (!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) ) && (u.urmaxlvlUP >= 2) ) {
			    losexp("life drainage", FALSE, TRUE);
			}

		}
		break;


	    case AD_COLD:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int dmg = d(2,6);
		    if (!rn2(10)) dmg += dmgplus;

		    pline("%s attacks you with an icy gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) {
			pline_The("cold doesn't freeze you!");
			dmg = 0;
		    }
		    if (Race_if(PM_GAVIL)) dmg *= 2;
		    if (Race_if(PM_HYPOTHERMIC)) dmg *= 3;
		    /*if ((int) mtmp->m_lev > rn2(20))*/
		    if (isevilvariant || !rn2(issoviet ? 6 : Race_if(PM_GAVIL) ? 6 : Race_if(PM_HYPOTHERMIC) ? 6 : 33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);
		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;
	    case AD_ELEC:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int dmg = d(2,6);
		    if (!rn2(10)) dmg += dmgplus;

		    pline("%s attacks you with a shocking gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) {
			pline_The("gaze doesn't shock you!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */
			destroy_item(AMULET_CLASS, AD_ELEC);
		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;
	    case AD_MALK:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    int dmg = d(3,6);
		    if (!rn2(10)) dmg += dmgplus;

		    pline("%s attacks you with an electrifying gaze!", Monnam(mtmp));
		    stop_occupation();

		    setustuck(mtmp);
		    pline("%s grabs you!", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net vykhoda! Ty predatel' russkogo naroda i, sledovatel'no, budut zaderzhany navsegda!" : "Wroa!");
		    if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		    if (Shock_resistance && rn2(StrongShock_resistance ? 20 : 5)) {
			pline_The("gaze doesn't shock you!");
			dmg = 0;
		    }
		    if (isevilvariant || !rn2(issoviet ? 2 : 10)) /* high voltage - stronger than ordinary shock attack --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 2 : 10))
			destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 10 : 50))
			destroy_item(AMULET_CLASS, AD_ELEC);
		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;

	    case AD_ICEB:
		if (!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    pline("%s hurls an ice block at you and hits!", Monnam(mtmp));
			if (issoviet) pline("ON ON ON!");
		    stop_occupation();

		    int dmg = d(3,6);
		    if (!rn2(10)) dmg += dmgplus;

		    if (Race_if(PM_GAVIL)) dmg *= 2;
		    if (Race_if(PM_HYPOTHERMIC)) dmg *= 3;

		    if(!rn2(3)) {
			    make_frozen(HFrozen + dmg, TRUE);
		    }
		    if (isevilvariant || !rn2(issoviet ? 2 : Race_if(PM_GAVIL) ? 2 : Race_if(PM_HYPOTHERMIC) ? 2 : 10)) {
			destroy_item(POTION_CLASS, AD_COLD);
		    }
		    if (Cold_resistance && rn2(StrongCold_resistance ? 20 : 5)) {
			pline("The attack doesn't seem to damage you.");
		    dmg = 0;
		    }
		    if (dmg) mdamageu(mtmp, dmg);
		}
	    break;

	    case AD_UVUU:{

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(50))) 		{
		pline("%s used HORN DRILL!", Monnam(mtmp));
		    stop_occupation();

		int wdmg = (int)(dmgplus/6) + 1;
		sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		poisoned(buf, A_CON, mtmp->data->mname, 60);
		if(Poison_resistance) wdmg -= ACURR(A_CON)/2;
		if (StrongPoison_resistance && wdmg > 1) wdmg /= 2;
		if(wdmg > 0){
		
			while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
				wdmg--;
				(void) adjattrib(A_WIS, -1, TRUE, TRUE);
				forget_levels(1);	/* lose memory of 1% of levels per point lost*/
				forget_objects(1);	/* lose memory of 1% of objects per point lost*/
				exercise(A_WIS, FALSE);
			}
			if(AMAX(A_WIS) > ATTRMIN(A_WIS) && 
				ABASE(A_WIS) < AMAX(A_WIS)/2) AMAX(A_WIS) -= 1; //permanently drain wisdom
			if(wdmg){
				boolean chg;
				chg = make_hallucinated(HHallucination + (long)(wdmg*5),FALSE,0L);
			}
		}
		drain_en( (int)(dmgplus/2) );
		if(!rn2(20)){
			if (!has_head(youmonst.data) || Role_if(PM_COURIER) ) {
				dmgplus *= 2;
			}
			else if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) {
				pline("It's not very effective...");
				dmgplus *= 2;
			}
			else {
				if(!uarmh){
					dmgplus = (ABASE(A_WIS) <= ATTRMIN(A_WIS)) ? ( 2 * (Upolyd ? u.mh : u.uhp) + 400) : (dmgplus * 2); 
					pline("It's a 1-hit KO!");
				} else pline("It's super effective!");
			}
		 }
 		}
		}
	    break;

       case AD_DRIN:
     if(!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee && (issoviet || !rn2(10)) &&
        (!ublindf || ublindf->otyp != TOWEL)  &&
        !mtmp->mspec_used){
       pline("%s screeches at you!", Monnam(mtmp));
		    stop_occupation();
       if (u.usleep){
         multi = -1;
         nomovemsg = "You wake.";
       }
         if (ABASE(A_INT) > ATTRMIN(A_INT) && !rn2(10)) {
           /* adjattrib gives dunce cap message when appropriate */
           (void) adjattrib(A_INT, -1, FALSE, TRUE);
           losespells();
           forget_map(0);
           docrt();
         }
         mtmp->mspec_used += ABASE(A_INT) * rn1(1,3);
     }
     break;
#ifdef PM_BEHOLDER /* work in progress */
#if 0
	    case AD_SLEE:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee &&
		   multi >= 0 && !rn2(5) && !Sleep_resistance) {

		    fall_asleep(-rnd((rn2(10) ? 10 : (10+dmgplus)) ), TRUE);
		    pline("%s gaze makes you very sleepy...",
			  s_suffix(Monnam(mtmp)));
		}
		break;
#endif
	    case AD_SLOW:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee &&
		   (HFast & (INTRINSIC|TIMEOUT)) &&
		   !defends(AD_SLOW, uwep) && (issoviet || !rn2(4))) {

		    pline("%s uses a slowing gaze!",Monnam(mtmp));
		    stop_occupation();

		    u_slow_down();

			if (Race_if(PM_SPIRIT) && !rn2(3)) {
				u.uprops[DEAC_FAST].intrinsic += ((dmgplus + 2) * 10);
				pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
				u.inertia += (dmgplus + 2);

			}

		    stop_occupation();
			}
		break;
#endif
	    case AD_SLEE:
		if(!mtmp->mcan && canseemon(mtmp) &&
				mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    if (Displaced && rn2(StrongDisplaced ? 5 : 3)) {
			if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
			    break;
		    }
		    if ((Invisible && rn2(StrongInvis ? 5 : 3)) || rn2(4)) {
			if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
			break;
		    }
		    if (!Blind) pline("%s gazes directly at you!",Monnam(mtmp));
		    stop_occupation();
		    if(Reflecting && m_canseeu(mtmp) && !mtmp->mcan) {
			if(!Blind) {
		    	    (void) ureflects("%s gaze is reflected by your %s.",
		    			s_suffix(Monnam(mtmp)));
		    	    if (mon_reflects(mtmp,
		    			"The gaze is reflected away by %s %s!"))
				break;
			}
			if (sleep_monst(mtmp, rnd(10), -1) && !Blind)
			    pline("%s is put to sleep!", Monnam(mtmp));
			break;
		    } else if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) {
			pline("You yawn.");
		    } else {
			nomul(-rnd( (rn2(10) ? 10 : (10+dmgplus)) ), "sleeping from a monster's gaze", TRUE);
			u.usleep = 1;
			nomovemsg = "You wake up.";
			if (Blind)  You("are put to sleep!");
			else You("are put to sleep by %s!",mon_nam(mtmp));
		    }
		}
		break;
	    case AD_DETH:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(18))) {
		    if (Displaced && rn2(StrongDisplaced ? 5 : 3)) {
			if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
			    break;
		    }
		    if ((Invisible && rn2(StrongInvis ? 5 : 3)) || rn2(4)) {
			if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
			break;
		    }
		    if ((!Blind) && !rn2(8)) pline("%s gazes directly at you!",Monnam(mtmp));
		    if(Reflecting && m_canseeu(mtmp) && !mtmp->mcan && !resists_death(mtmp) && mtmp->data->msound != MS_NEMESIS  ) {
			if(!Blind) {
		    	    (void) ureflects("%s gaze is reflected by your %s.",
		    			s_suffix(Monnam(mtmp)));
		    	    if (mon_reflects(mtmp,
		    			"The gaze is reflected away by %s %s!"))
				break;
			    pline("%s is killed by its own gaze of death!",
							Monnam(mtmp));
			}
			killed(mtmp);
			if (mtmp->mhp > 0) break;
			return 2;
		    } else if (is_undead(youmonst.data)) {
			/* Still does normal damage */
			pline("Was that the gaze of death?");
			break;
		    } else if (rn2(5) ) {
			/* Still does normal damage */
			pline("It is pitch black...");
			losehp(15 + dmgplus, "black gaze", KILLED_BY_AN);
			u.uhpmax -= 2;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			break;
		    } else if (PlayerResistsDeathRays) {
			You("shudder momentarily...");
		    } else {
			u.youaredead = 1;
			You(isangbander ? "have died." : "die...");
			killer_format = KILLED_BY_AN;
			killer = "gaze of death";
			done(DIED);
			u.youaredead = 0;
		    }
		}
		break;
	    case AD_PHYS:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || rn2(3))) {
	                if (Displaced && rn2(StrongDisplaced ? 5 : 3)) {
	                        if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
	                        break;
	                }
	                if ((Invisible && rn2(StrongInvis ? 5 : 3)) || rn2(4)) {
	                        if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
	                        break;
	                }
	                if (!Blind) pline("%s gazes directly at you!",Monnam(mtmp));
		    stop_occupation();
	                pline("You are wracked with pains!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Da, segodnya my dadim vam, sosatel'nyy pleyer, kakoy-to staromodnyy khoroshaya BOL'." : "Tschackschwack!");
	                mdamageu(mtmp, d(3,8) + dmgplus);
	        }
	        break;

	    case AD_THIR:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s sucks off your life force!", Monnam(mtmp));
		    stop_occupation();
			mtmp->mhp += (1 + dmgplus) ;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
                  mdamageu(mtmp, d(3,8) + dmgplus);
		  }

		break;

	    case AD_NTHR:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s invokes a nether storm!", Monnam(mtmp));
		    stop_occupation();
			mtmp->mhp += (1 + dmgplus) ;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
                  mdamageu(mtmp, d(3,8) + dmgplus);

			if ((!Drain_resistance || !rn2(StrongDrain_resistance ? 20 : 5)) && u.uexp > 100) {
				u.uexp -= (u.uexp / 100);
				You_feel("your life slipping away!");
				if (u.uexp < newuexp(u.ulevel - 1)) {
				      losexp("nether forces", TRUE, FALSE);
				}
			}
		  }

		break;

	    case AD_AGGR:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s uses an aggravating gaze!", Monnam(mtmp));
			incr_itimeout(&HAggravate_monster, dmgplus);
			You_feel("that monsters are aware of your presence.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
			aggravate();
			if (!rn2(20)) {

				int aggroamount = rnd(6);
				if (isfriday) aggroamount *= 2;
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
				while (aggroamount) {

					u.cnd_aggravateamount++;
					makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
					aggroamount--;
					if (aggroamount < 0) aggroamount = 0;
				}
				u.aggravation = 0;
				pline("Several monsters come out of a portal.");
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

			}
		}

		break;

	    case AD_DATA:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(25))) {
			pline("%s infects your system with viruses!", Monnam(mtmp));
			u.datadeletedefer = 1;
			datadeleteattack();
		}

		break;

	    case AD_RAGN:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
			pline("%s mumbles an eldritch incantation!", Monnam(mtmp));
			ragnarok(FALSE);
			if (evilfriday && mtmp->m_lev > 1) evilragnarok(FALSE,mtmp->m_lev);
		}

		break;

	    case AD_DEST:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(25))) {
			pline("%s fires the special cannon!", Monnam(mtmp));
			destroyarmorattack();
		}

		break;

	    case AD_MINA:

		/* The fact that the gaze does not give a message is extra evil *and intentional*. --Amy */

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {

			register int midentity = mtmp->m_id;
			if (midentity < 0) midentity *= -1;
			while (midentity > 235) midentity -= 235;

			register int nastyduration = ((dmgplus + 2) * rnd(10));
			if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) nastyduration *= 20;

			switch (midentity) {

				case 1: RMBLoss += rnz(nastyduration); break;
				case 2: NoDropProblem += rnz(nastyduration); break;
				case 3: DSTWProblem += rnz(nastyduration); break;
				case 4: StatusTrapProblem += rnz(nastyduration); 
					if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
					if (HStun) set_itimeout(&HeavyStunned, HStun);
					if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
					if (HFeared) set_itimeout(&HeavyFeared, HFeared);
					if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
					if (HBurned) set_itimeout(&HeavyBurned, HBurned);
					if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
					if (Blinded) set_itimeout(&HeavyBlind, Blinded);
					if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
					break;
				case 5: Superscroller += rnz(nastyduration * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz(nastyduration); break;
				case 7: FreeHandLoss += rnz(nastyduration); break;
				case 8: Unidentify += rnz(nastyduration); break;
				case 9: Thirst += rnz(nastyduration); break;
				case 10: LuckLoss += rnz(nastyduration); break;
				case 11: ShadesOfGrey += rnz(nastyduration); break;
				case 12: FaintActive += rnz(nastyduration); break;
				case 13: Itemcursing += rnz(nastyduration); break;
				case 14: DifficultyIncreased += rnz(nastyduration); break;
				case 15: Deafness += rnz(nastyduration); flags.soundok = 0; break;
				case 16: CasterProblem += rnz(nastyduration); break;
				case 17: WeaknessProblem += rnz(nastyduration); break;
				case 18: RotThirteen += rnz(nastyduration); break;
				case 19: BishopGridbug += rnz(nastyduration); break;
				case 20: UninformationProblem += rnz(nastyduration); break;
				case 21: StairsProblem += rnz(nastyduration); break;
				case 22: AlignmentProblem += rnz(nastyduration); break;
				case 23: ConfusionProblem += rnz(nastyduration); break;
				case 24: SpeedBug += rnz(nastyduration); break;
				case 25: DisplayLoss += rnz(nastyduration); break;
				case 26: SpellLoss += rnz(nastyduration); break;
				case 27: YellowSpells += rnz(nastyduration); break;
				case 28: AutoDestruct += rnz(nastyduration); break;
				case 29: MemoryLoss += rnz(nastyduration); break;
				case 30: InventoryLoss += rnz(nastyduration); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (dmgplus * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz(nastyduration); break;
				case 33: BloodLossProblem += rnz(nastyduration); break;
				case 34: BadEffectProblem += rnz(nastyduration); break;
				case 35: TrapCreationProblem += rnz(nastyduration); break;
				case 36: AutomaticVulnerabilitiy += rnz(nastyduration); break;
				case 37: TeleportingItems += rnz(nastyduration); break;
				case 38: NastinessProblem += rnz(nastyduration); break;
				case 39: CaptchaProblem += rnz(nastyduration); break;
				case 40: FarlookProblem += rnz(nastyduration); break;
				case 41: RespawnProblem += rnz(nastyduration); break;
				case 42: RecurringAmnesia += rnz(nastyduration); break;
				case 43: BigscriptEffect += rnz(nastyduration); break;
				case 44: {
					BankTrapEffect += rnz(nastyduration);
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz(nastyduration); break;
				case 46: TechTrapEffect += rnz(nastyduration); break;
				case 47: RecurringDisenchant += rnz(nastyduration); break;
				case 48: verisiertEffect += rnz(nastyduration); break;
				case 49: ChaosTerrain += rnz(nastyduration); break;
				case 50: Muteness += rnz(nastyduration); break;
				case 51: EngravingDoesntWork += rnz(nastyduration); break;
				case 52: MagicDeviceEffect += rnz(nastyduration); break;
				case 53: BookTrapEffect += rnz(nastyduration); break;
				case 54: LevelTrapEffect += rnz(nastyduration); break;
				case 55: QuizTrapEffect += rnz(nastyduration); break;
				case 56: FastMetabolismEffect += rnz(nastyduration); break;
				case 57: NoReturnEffect += rnz(nastyduration); break;
				case 58: AlwaysEgotypeMonsters += rnz(nastyduration); break;
				case 59: TimeGoesByFaster += rnz(nastyduration); break;
				case 60: FoodIsAlwaysRotten += rnz(nastyduration); break;
				case 61: AllSkillsUnskilled += rnz(nastyduration); break;
				case 62: AllStatsAreLower += rnz(nastyduration); break;
				case 63: PlayerCannotTrainSkills += rnz(nastyduration); break;
				case 64: PlayerCannotExerciseStats += rnz(nastyduration); break;
				case 65: TurnLimitation += rnz(nastyduration); break;
				case 66: WeakSight += rnz(nastyduration); break;
				case 67: RandomMessages += rnz(nastyduration); break;

				case 68: Desecration += rnz(nastyduration); break;
				case 69: StarvationEffect += rnz(nastyduration); break;
				case 70: NoDropsEffect += rnz(nastyduration); break;
				case 71: LowEffects += rnz(nastyduration); break;
				case 72: InvisibleTrapsEffect += rnz(nastyduration); break;
				case 73: GhostWorld += rnz(nastyduration); break;
				case 74: Dehydration += rnz(nastyduration); break;
				case 75: HateTrapEffect += rnz(nastyduration); break;
				case 76: TotterTrapEffect += rnz(nastyduration); break;
				case 77: Nonintrinsics += rnz(nastyduration); break;
				case 78: Dropcurses += rnz(nastyduration); break;
				case 79: Nakedness += rnz(nastyduration); break;
				case 80: Antileveling += rnz(nastyduration); break;
				case 81: ItemStealingEffect += rnz(nastyduration); break;
				case 82: Rebellions += rnz(nastyduration); break;
				case 83: CrapEffect += rnz(nastyduration); break;
				case 84: ProjectilesMisfire += rnz(nastyduration); break;
				case 85: WallTrapping += rnz(nastyduration); break;
				case 86: DisconnectedStairs += rnz(nastyduration); break;
				case 87: InterfaceScrewed += rnz(nastyduration); break;
				case 88: Bossfights += rnz(nastyduration); break;
				case 89: EntireLevelMode += rnz(nastyduration); break;
				case 90: BonesLevelChange += rnz(nastyduration); break;
				case 91: AutocursingEquipment += rnz(nastyduration); break;
				case 92: HighlevelStatus += rnz(nastyduration); break;
				case 93: SpellForgetting += rnz(nastyduration); break;
				case 94: SoundEffectBug += rnz(nastyduration); break;
				case 95: TimerunBug += rnz(nastyduration); break;
				case 96: LootcutBug += rnz(nastyduration); break;
				case 97: MonsterSpeedBug += rnz(nastyduration); break;
				case 98: ScalingBug += rnz(nastyduration); break;
				case 99: EnmityBug += rnz(nastyduration); break;
				case 100: WhiteSpells += rnz(nastyduration); break;
				case 101: CompleteGraySpells += rnz(nastyduration); break;
				case 102: QuasarVision += rnz(nastyduration); break;
				case 103: MommaBugEffect += rnz(nastyduration); break;
				case 104: HorrorBugEffect += rnz(nastyduration); break;
				case 105: ArtificerBug += rnz(nastyduration); break;
				case 106: WereformBug += rnz(nastyduration); break;
				case 107: NonprayerBug += rnz(nastyduration); break;
				case 108: EvilPatchEffect += rnz(nastyduration); break;
				case 109: HardModeEffect += rnz(nastyduration); break;
				case 110: SecretAttackBug += rnz(nastyduration); break;
				case 111: EaterBugEffect += rnz(nastyduration); break;
				case 112: CovetousnessBug += rnz(nastyduration); break;
				case 113: NotSeenBug += rnz(nastyduration); break;
				case 114: DarkModeBug += rnz(nastyduration); break;
				case 115: AntisearchEffect += rnz(nastyduration); break;
				case 116: HomicideEffect += rnz(nastyduration); break;
				case 117: NastynationBug += rnz(nastyduration); break;
				case 118: WakeupCallBug += rnz(nastyduration); break;
				case 119: GrayoutBug += rnz(nastyduration); break;
				case 120: GrayCenterBug += rnz(nastyduration); break;
				case 121: CheckerboardBug += rnz(nastyduration); break;
				case 122: ClockwiseSpinBug += rnz(nastyduration); break;
				case 123: CounterclockwiseSpin += rnz(nastyduration); break;
				case 124: LagBugEffect += rnz(nastyduration); break;
				case 125: BlesscurseEffect += rnz(nastyduration); break;
				case 126: DeLightBug += rnz(nastyduration); break;
				case 127: DischargeBug += rnz(nastyduration); break;
				case 128: TrashingBugEffect += rnz(nastyduration); break;
				case 129: FilteringBug += rnz(nastyduration); break;
				case 130: DeformattingBug += rnz(nastyduration); break;
				case 131: FlickerStripBug += rnz(nastyduration); break;
				case 132: UndressingEffect += rnz(nastyduration); break;
				case 133: Hyperbluewalls += rnz(nastyduration); break;
				case 134: NoliteBug += rnz(nastyduration); break;
				case 135: ParanoiaBugEffect += rnz(nastyduration); break;
				case 136: FleecescriptBug += rnz(nastyduration); break;
				case 137: InterruptEffect += rnz(nastyduration); break;
				case 138: DustbinBug += rnz(nastyduration); break;
				case 139: ManaBatteryBug += rnz(nastyduration); break;
				case 140: Monsterfingers += rnz(nastyduration); break;
				case 141: MiscastBug += rnz(nastyduration); break;
				case 142: MessageSuppression += rnz(nastyduration); break;
				case 143: StuckAnnouncement += rnz(nastyduration); break;
				case 144: BloodthirstyEffect += rnz(nastyduration); break;
				case 145: MaximumDamageBug += rnz(nastyduration); break;
				case 146: LatencyBugEffect += rnz(nastyduration); break;
				case 147: StarlitBug += rnz(nastyduration); break;
				case 148: KnowledgeBug += rnz(nastyduration); break;
				case 149: HighscoreBug += rnz(nastyduration); break;
				case 150: PinkSpells += rnz(nastyduration); break;
				case 151: GreenSpells += rnz(nastyduration); break;
				case 152: EvencoreEffect += rnz(nastyduration); break;
				case 153: UnderlayerBug += rnz(nastyduration); break;
				case 154: DamageMeterBug += rnz(nastyduration); break;
				case 155: ArbitraryWeightBug += rnz(nastyduration); break;
				case 156: FuckedInfoBug += rnz(nastyduration); break;
				case 157: BlackSpells += rnz(nastyduration); break;
				case 158: CyanSpells += rnz(nastyduration); break;
				case 159: HeapEffectBug += rnz(nastyduration); break;
				case 160: BlueSpells += rnz(nastyduration); break;
				case 161: TronEffect += rnz(nastyduration); break;
				case 162: RedSpells += rnz(nastyduration); break;
				case 163: TooHeavyEffect += rnz(nastyduration); break;
				case 164: ElongationBug += rnz(nastyduration); break;
				case 165: WrapoverEffect += rnz(nastyduration); break;
				case 166: DestructionEffect += rnz(nastyduration); break;
				case 167: MeleePrefixBug += rnz(nastyduration); break;
				case 168: AutomoreBug += rnz(nastyduration); break;
				case 169: UnfairAttackBug += rnz(nastyduration); break;
				case 170: OrangeSpells += rnz(nastyduration); break;
				case 171: VioletSpells += rnz(nastyduration); break;
				case 172: LongingEffect += rnz(nastyduration); break;
				case 173: CursedParts += rnz(nastyduration); break;
				case 174: Quaversal += rnz(nastyduration); break;
				case 175: AppearanceShuffling += rnz(nastyduration); break;
				case 176: BrownSpells += rnz(nastyduration); break;
				case 177: Choicelessness += rnz(nastyduration); break;
				case 178: Goldspells += rnz(nastyduration); break;
				case 179: Deprovement += rnz(nastyduration); break;
				case 180: InitializationFail += rnz(nastyduration); break;
				case 181: GushlushEffect += rnz(nastyduration); break;
				case 182: SoiltypeEffect += rnz(nastyduration); break;
				case 183: DangerousTerrains += rnz(nastyduration); break;
				case 184: FalloutEffect += rnz(nastyduration); break;
				case 185: MojibakeEffect += rnz(nastyduration); break;
				case 186: GravationEffect += rnz(nastyduration); break;
				case 187: UncalledEffect += rnz(nastyduration); break;
				case 188: ExplodingDiceEffect += rnz(nastyduration); break;
				case 189: PermacurseEffect += rnz(nastyduration); break;
				case 190: ShroudedIdentity += rnz(nastyduration); break;
				case 191: FeelerGauges += rnz(nastyduration); break;
				case 192: LongScrewup += rnz(nastyduration * 20); break;
				case 193: WingYellowChange += rnz(nastyduration); break;
				case 194: LifeSavingBug += rnz(nastyduration); break;
				case 195: CurseuseEffect += rnz(nastyduration); break;
				case 196: CutNutritionEffect += rnz(nastyduration); break;
				case 197: SkillLossEffect += rnz(nastyduration); break;
				case 198: AutopilotEffect += rnz(nastyduration); break;
				case 199: MysteriousForceActive += rnz(nastyduration); break;
				case 200: MonsterGlyphChange += rnz(nastyduration); break;
				case 201: ChangingDirectives += rnz(nastyduration); break;
				case 202: ContainerKaboom += rnz(nastyduration); break;
				case 203: StealDegrading += rnz(nastyduration); break;
				case 204: LeftInventoryBug += rnz(nastyduration); break;
				case 205: FluctuatingSpeed += rnz(nastyduration); break;
				case 206: TarmuStrokingNora += rnz(nastyduration); break;
				case 207: FailureEffects += rnz(nastyduration); break;
				case 208: BrightCyanSpells += rnz(nastyduration); break;
				case 209: FrequentationSpawns += rnz(nastyduration); break;
				case 210: PetAIScrewed += rnz(nastyduration); break;
				case 211: SatanEffect += rnz(nastyduration); break;
				case 212: RememberanceEffect += rnz(nastyduration); break;
				case 213: PokelieEffect += rnz(nastyduration); break;
				case 214: AlwaysAutopickup += rnz(nastyduration); break;
				case 215: DywypiProblem += rnz(nastyduration); break;
				case 216: SilverSpells += rnz(nastyduration); break;
				case 217: MetalSpells += rnz(nastyduration); break;
				case 218: PlatinumSpells += rnz(nastyduration); break;
				case 219: ManlerEffect += rnz(nastyduration); break;
				case 220: DoorningEffect += rnz(nastyduration); break;
				case 221: NownsibleEffect += rnz(nastyduration); break;
				case 222: ElmStreetEffect += rnz(nastyduration); break;
				case 223: MonnoiseEffect += rnz(nastyduration); break;
				case 224: RangCallEffect += rnz(nastyduration); break;
				case 225: RecurringSpellLoss += rnz(nastyduration); break;
				case 226: AntitrainingEffect += rnz(nastyduration); break;
				case 227: TechoutBug += rnz(nastyduration); break;
				case 228: StatDecay += rnz(nastyduration); break;
				case 229: Movemork += rnz(nastyduration); break;
				case 230: BadPartBug += rnz(nastyduration); break;
				case 231: CompletelyBadPartBug += rnz(nastyduration); break;
				case 232: EvilVariantActive += rnz(nastyduration); break;
				case 233: SanityTrebleEffect += rnz(nastyduration); break;
				case 234: StatDecreaseBug += rnz(nastyduration); break;
				case 235: SimeoutBug += rnz(nastyduration); break;

				default: impossible("AD_MINA called with invalid value %d", midentity); break;
			}

		}

		break;

	    case AD_RUNS:

		/* The fact that the gaze does not give a message is extra evil *and intentional*. --Amy */

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {

			register int nastyduration = ((dmgplus + 2) * rnd(10));
			if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) nastyduration *= 20;

			switch (u.adrunsattack) {

				case 1: RMBLoss += rnz(nastyduration); break;
				case 2: NoDropProblem += rnz(nastyduration); break;
				case 3: DSTWProblem += rnz(nastyduration); break;
				case 4: StatusTrapProblem += rnz(nastyduration); 
					if (HConfusion) set_itimeout(&HeavyConfusion, HConfusion);
					if (HStun) set_itimeout(&HeavyStunned, HStun);
					if (HNumbed) set_itimeout(&HeavyNumbed, HNumbed);
					if (HFeared) set_itimeout(&HeavyFeared, HFeared);
					if (HFrozen) set_itimeout(&HeavyFrozen, HFrozen);
					if (HBurned) set_itimeout(&HeavyBurned, HBurned);
					if (HDimmed) set_itimeout(&HeavyDimmed, HDimmed);
					if (Blinded) set_itimeout(&HeavyBlind, Blinded);
					if (HHallucination) set_itimeout(&HeavyHallu, HHallucination);
					break;
				case 5: Superscroller += rnz(nastyduration * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz(nastyduration); break;
				case 7: FreeHandLoss += rnz(nastyduration); break;
				case 8: Unidentify += rnz(nastyduration); break;
				case 9: Thirst += rnz(nastyduration); break;
				case 10: LuckLoss += rnz(nastyduration); break;
				case 11: ShadesOfGrey += rnz(nastyduration); break;
				case 12: FaintActive += rnz(nastyduration); break;
				case 13: Itemcursing += rnz(nastyduration); break;
				case 14: DifficultyIncreased += rnz(nastyduration); break;
				case 15: Deafness += rnz(nastyduration); flags.soundok = 0; break;
				case 16: CasterProblem += rnz(nastyduration); break;
				case 17: WeaknessProblem += rnz(nastyduration); break;
				case 18: RotThirteen += rnz(nastyduration); break;
				case 19: BishopGridbug += rnz(nastyduration); break;
				case 20: UninformationProblem += rnz(nastyduration); break;
				case 21: StairsProblem += rnz(nastyduration); break;
				case 22: AlignmentProblem += rnz(nastyduration); break;
				case 23: ConfusionProblem += rnz(nastyduration); break;
				case 24: SpeedBug += rnz(nastyduration); break;
				case 25: DisplayLoss += rnz(nastyduration); break;
				case 26: SpellLoss += rnz(nastyduration); break;
				case 27: YellowSpells += rnz(nastyduration); break;
				case 28: AutoDestruct += rnz(nastyduration); break;
				case 29: MemoryLoss += rnz(nastyduration); break;
				case 30: InventoryLoss += rnz(nastyduration); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (dmgplus * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz(nastyduration); break;
				case 33: BloodLossProblem += rnz(nastyduration); break;
				case 34: BadEffectProblem += rnz(nastyduration); break;
				case 35: TrapCreationProblem += rnz(nastyduration); break;
				case 36: AutomaticVulnerabilitiy += rnz(nastyduration); break;
				case 37: TeleportingItems += rnz(nastyduration); break;
				case 38: NastinessProblem += rnz(nastyduration); break;
				case 39: CaptchaProblem += rnz(nastyduration); break;
				case 40: FarlookProblem += rnz(nastyduration); break;
				case 41: RespawnProblem += rnz(nastyduration); break;
				case 42: RecurringAmnesia += rnz(nastyduration); break;
				case 43: BigscriptEffect += rnz(nastyduration); break;
				case 44: {
					BankTrapEffect += rnz(nastyduration);
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1 + (long)mvitals[PM_ARABELLA_THE_MONEY_THIEF].born));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz(nastyduration); break;
				case 46: TechTrapEffect += rnz(nastyduration); break;
				case 47: RecurringDisenchant += rnz(nastyduration); break;
				case 48: verisiertEffect += rnz(nastyduration); break;
				case 49: ChaosTerrain += rnz(nastyduration); break;
				case 50: Muteness += rnz(nastyduration); break;
				case 51: EngravingDoesntWork += rnz(nastyduration); break;
				case 52: MagicDeviceEffect += rnz(nastyduration); break;
				case 53: BookTrapEffect += rnz(nastyduration); break;
				case 54: LevelTrapEffect += rnz(nastyduration); break;
				case 55: QuizTrapEffect += rnz(nastyduration); break;
				case 56: FastMetabolismEffect += rnz(nastyduration); break;
				case 57: NoReturnEffect += rnz(nastyduration); break;
				case 58: AlwaysEgotypeMonsters += rnz(nastyduration); break;
				case 59: TimeGoesByFaster += rnz(nastyduration); break;
				case 60: FoodIsAlwaysRotten += rnz(nastyduration); break;
				case 61: AllSkillsUnskilled += rnz(nastyduration); break;
				case 62: AllStatsAreLower += rnz(nastyduration); break;
				case 63: PlayerCannotTrainSkills += rnz(nastyduration); break;
				case 64: PlayerCannotExerciseStats += rnz(nastyduration); break;
				case 65: TurnLimitation += rnz(nastyduration); break;
				case 66: WeakSight += rnz(nastyduration); break;
				case 67: RandomMessages += rnz(nastyduration); break;

				case 68: Desecration += rnz(nastyduration); break;
				case 69: StarvationEffect += rnz(nastyduration); break;
				case 70: NoDropsEffect += rnz(nastyduration); break;
				case 71: LowEffects += rnz(nastyduration); break;
				case 72: InvisibleTrapsEffect += rnz(nastyduration); break;
				case 73: GhostWorld += rnz(nastyduration); break;
				case 74: Dehydration += rnz(nastyduration); break;
				case 75: HateTrapEffect += rnz(nastyduration); break;
				case 76: TotterTrapEffect += rnz(nastyduration); break;
				case 77: Nonintrinsics += rnz(nastyduration); break;
				case 78: Dropcurses += rnz(nastyduration); break;
				case 79: Nakedness += rnz(nastyduration); break;
				case 80: Antileveling += rnz(nastyduration); break;
				case 81: ItemStealingEffect += rnz(nastyduration); break;
				case 82: Rebellions += rnz(nastyduration); break;
				case 83: CrapEffect += rnz(nastyduration); break;
				case 84: ProjectilesMisfire += rnz(nastyduration); break;
				case 85: WallTrapping += rnz(nastyduration); break;
				case 86: DisconnectedStairs += rnz(nastyduration); break;
				case 87: InterfaceScrewed += rnz(nastyduration); break;
				case 88: Bossfights += rnz(nastyduration); break;
				case 89: EntireLevelMode += rnz(nastyduration); break;
				case 90: BonesLevelChange += rnz(nastyduration); break;
				case 91: AutocursingEquipment += rnz(nastyduration); break;
				case 92: HighlevelStatus += rnz(nastyduration); break;
				case 93: SpellForgetting += rnz(nastyduration); break;
				case 94: SoundEffectBug += rnz(nastyduration); break;
				case 95: TimerunBug += rnz(nastyduration); break;
				case 96: LootcutBug += rnz(nastyduration); break;
				case 97: MonsterSpeedBug += rnz(nastyduration); break;
				case 98: ScalingBug += rnz(nastyduration); break;
				case 99: EnmityBug += rnz(nastyduration); break;
				case 100: WhiteSpells += rnz(nastyduration); break;
				case 101: CompleteGraySpells += rnz(nastyduration); break;
				case 102: QuasarVision += rnz(nastyduration); break;
				case 103: MommaBugEffect += rnz(nastyduration); break;
				case 104: HorrorBugEffect += rnz(nastyduration); break;
				case 105: ArtificerBug += rnz(nastyduration); break;
				case 106: WereformBug += rnz(nastyduration); break;
				case 107: NonprayerBug += rnz(nastyduration); break;
				case 108: EvilPatchEffect += rnz(nastyduration); break;
				case 109: HardModeEffect += rnz(nastyduration); break;
				case 110: SecretAttackBug += rnz(nastyduration); break;
				case 111: EaterBugEffect += rnz(nastyduration); break;
				case 112: CovetousnessBug += rnz(nastyduration); break;
				case 113: NotSeenBug += rnz(nastyduration); break;
				case 114: DarkModeBug += rnz(nastyduration); break;
				case 115: AntisearchEffect += rnz(nastyduration); break;
				case 116: HomicideEffect += rnz(nastyduration); break;
				case 117: NastynationBug += rnz(nastyduration); break;
				case 118: WakeupCallBug += rnz(nastyduration); break;
				case 119: GrayoutBug += rnz(nastyduration); break;
				case 120: GrayCenterBug += rnz(nastyduration); break;
				case 121: CheckerboardBug += rnz(nastyduration); break;
				case 122: ClockwiseSpinBug += rnz(nastyduration); break;
				case 123: CounterclockwiseSpin += rnz(nastyduration); break;
				case 124: LagBugEffect += rnz(nastyduration); break;
				case 125: BlesscurseEffect += rnz(nastyduration); break;
				case 126: DeLightBug += rnz(nastyduration); break;
				case 127: DischargeBug += rnz(nastyduration); break;
				case 128: TrashingBugEffect += rnz(nastyduration); break;
				case 129: FilteringBug += rnz(nastyduration); break;
				case 130: DeformattingBug += rnz(nastyduration); break;
				case 131: FlickerStripBug += rnz(nastyduration); break;
				case 132: UndressingEffect += rnz(nastyduration); break;
				case 133: Hyperbluewalls += rnz(nastyduration); break;
				case 134: NoliteBug += rnz(nastyduration); break;
				case 135: ParanoiaBugEffect += rnz(nastyduration); break;
				case 136: FleecescriptBug += rnz(nastyduration); break;
				case 137: InterruptEffect += rnz(nastyduration); break;
				case 138: DustbinBug += rnz(nastyduration); break;
				case 139: ManaBatteryBug += rnz(nastyduration); break;
				case 140: Monsterfingers += rnz(nastyduration); break;
				case 141: MiscastBug += rnz(nastyduration); break;
				case 142: MessageSuppression += rnz(nastyduration); break;
				case 143: StuckAnnouncement += rnz(nastyduration); break;
				case 144: BloodthirstyEffect += rnz(nastyduration); break;
				case 145: MaximumDamageBug += rnz(nastyduration); break;
				case 146: LatencyBugEffect += rnz(nastyduration); break;
				case 147: StarlitBug += rnz(nastyduration); break;
				case 148: KnowledgeBug += rnz(nastyduration); break;
				case 149: HighscoreBug += rnz(nastyduration); break;
				case 150: PinkSpells += rnz(nastyduration); break;
				case 151: GreenSpells += rnz(nastyduration); break;
				case 152: EvencoreEffect += rnz(nastyduration); break;
				case 153: UnderlayerBug += rnz(nastyduration); break;
				case 154: DamageMeterBug += rnz(nastyduration); break;
				case 155: ArbitraryWeightBug += rnz(nastyduration); break;
				case 156: FuckedInfoBug += rnz(nastyduration); break;
				case 157: BlackSpells += rnz(nastyduration); break;
				case 158: CyanSpells += rnz(nastyduration); break;
				case 159: HeapEffectBug += rnz(nastyduration); break;
				case 160: BlueSpells += rnz(nastyduration); break;
				case 161: TronEffect += rnz(nastyduration); break;
				case 162: RedSpells += rnz(nastyduration); break;
				case 163: TooHeavyEffect += rnz(nastyduration); break;
				case 164: ElongationBug += rnz(nastyduration); break;
				case 165: WrapoverEffect += rnz(nastyduration); break;
				case 166: DestructionEffect += rnz(nastyduration); break;
				case 167: MeleePrefixBug += rnz(nastyduration); break;
				case 168: AutomoreBug += rnz(nastyduration); break;
				case 169: UnfairAttackBug += rnz(nastyduration); break;
				case 170: OrangeSpells += rnz(nastyduration); break;
				case 171: VioletSpells += rnz(nastyduration); break;
				case 172: LongingEffect += rnz(nastyduration); break;
				case 173: CursedParts += rnz(nastyduration); break;
				case 174: Quaversal += rnz(nastyduration); break;
				case 175: AppearanceShuffling += rnz(nastyduration); break;
				case 176: BrownSpells += rnz(nastyduration); break;
				case 177: Choicelessness += rnz(nastyduration); break;
				case 178: Goldspells += rnz(nastyduration); break;
				case 179: Deprovement += rnz(nastyduration); break;
				case 180: InitializationFail += rnz(nastyduration); break;
				case 181: GushlushEffect += rnz(nastyduration); break;
				case 182: SoiltypeEffect += rnz(nastyduration); break;
				case 183: DangerousTerrains += rnz(nastyduration); break;
				case 184: FalloutEffect += rnz(nastyduration); break;
				case 185: MojibakeEffect += rnz(nastyduration); break;
				case 186: GravationEffect += rnz(nastyduration); break;
				case 187: UncalledEffect += rnz(nastyduration); break;
				case 188: ExplodingDiceEffect += rnz(nastyduration); break;
				case 189: PermacurseEffect += rnz(nastyduration); break;
				case 190: ShroudedIdentity += rnz(nastyduration); break;
				case 191: FeelerGauges += rnz(nastyduration); break;
				case 192: LongScrewup += rnz(nastyduration * 20); break;
				case 193: WingYellowChange += rnz(nastyduration); break;
				case 194: LifeSavingBug += rnz(nastyduration); break;
				case 195: CurseuseEffect += rnz(nastyduration); break;
				case 196: CutNutritionEffect += rnz(nastyduration); break;
				case 197: SkillLossEffect += rnz(nastyduration); break;
				case 198: AutopilotEffect += rnz(nastyduration); break;
				case 199: MysteriousForceActive += rnz(nastyduration); break;
				case 200: MonsterGlyphChange += rnz(nastyduration); break;
				case 201: ChangingDirectives += rnz(nastyduration); break;
				case 202: ContainerKaboom += rnz(nastyduration); break;
				case 203: StealDegrading += rnz(nastyduration); break;
				case 204: LeftInventoryBug += rnz(nastyduration); break;
				case 205: FluctuatingSpeed += rnz(nastyduration); break;
				case 206: TarmuStrokingNora += rnz(nastyduration); break;
				case 207: FailureEffects += rnz(nastyduration); break;
				case 208: BrightCyanSpells += rnz(nastyduration); break;
				case 209: FrequentationSpawns += rnz(nastyduration); break;
				case 210: PetAIScrewed += rnz(nastyduration); break;
				case 211: SatanEffect += rnz(nastyduration); break;
				case 212: RememberanceEffect += rnz(nastyduration); break;
				case 213: PokelieEffect += rnz(nastyduration); break;
				case 214: AlwaysAutopickup += rnz(nastyduration); break;
				case 215: DywypiProblem += rnz(nastyduration); break;
				case 216: SilverSpells += rnz(nastyduration); break;
				case 217: MetalSpells += rnz(nastyduration); break;
				case 218: PlatinumSpells += rnz(nastyduration); break;
				case 219: ManlerEffect += rnz(nastyduration); break;
				case 220: DoorningEffect += rnz(nastyduration); break;
				case 221: NownsibleEffect += rnz(nastyduration); break;
				case 222: ElmStreetEffect += rnz(nastyduration); break;
				case 223: MonnoiseEffect += rnz(nastyduration); break;
				case 224: RangCallEffect += rnz(nastyduration); break;
				case 225: RecurringSpellLoss += rnz(nastyduration); break;
				case 226: AntitrainingEffect += rnz(nastyduration); break;
				case 227: TechoutBug += rnz(nastyduration); break;
				case 228: StatDecay += rnz(nastyduration); break;
				case 229: Movemork += rnz(nastyduration); break;
				case 230: BadPartBug += rnz(nastyduration); break;
				case 231: CompletelyBadPartBug += rnz(nastyduration); break;
				case 232: EvilVariantActive += rnz(nastyduration); break;
				case 233: SanityTrebleEffect += rnz(nastyduration); break;
				case 234: StatDecreaseBug += rnz(nastyduration); break;
				case 235: SimeoutBug += rnz(nastyduration); break;

				default: impossible("AD_RUNS called with invalid value %d", u.adrunsattack); break;
			}

		}

		break;

	    case AD_SIN:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                  pline("%s gazes at you, and you realize that you have transgressed.", Monnam(mtmp));
			u.ualign.sins++;
			u.alignlim--;
			adjalign(-5);
		}

		break;

	    case AD_ALIN:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                  pline("%s uses a depraved gaze!", Monnam(mtmp));
			adjalign(-(5 + dmgplus));
		}

		break;

	    case AD_CONT:

	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                  pline("%s's image will forever be in your mind...", Monnam(mtmp));

			contaminate(5 + dmgplus, TRUE);

		}

		break;

	    case AD_CHKH:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                  pline("%s gazes at you and screams the word 'DIE!'", Monnam(mtmp));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Sdelay eto seychas! Sprygnut' s mosta!" : "SCHRANG!");
		    stop_occupation();
			dmgplus += u.chokhmahdamage;
			dmgplus += rnd(u.ualign.sins > 0 ? (isqrt(u.ualign.sins) + 1) : (1));
			u.chokhmahdamage++;
                  mdamageu(mtmp, d(3,8) + dmgplus);
		  }
		break;

	    case AD_HODS:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                  pline("%s summons a mirror image of you, which promptly attacks you!", Monnam(mtmp));
		    stop_occupation();
		 if(uwep){
			if (uwep->otyp == CORPSE
				&& touch_petrifies(&mons[uwep->corpsenm])) {
			    dmgplus = 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[uwep->corpsenm].mname);

			    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
				!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {

				if (!Stoned) {
					if (Hallucination && rn2(10)) pline("Thankfully you are already stoned.");
					else {
						Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
						u.cnd_stoningcount++;
						sprintf(killer_buf, "being hit by a mirrored petrifying corpse");
						delayed_killer = killer_buf;
					}
				}
		
			    }
			}
			dmgplus += dmgval(uwep, &youmonst);
			
			if (uwep->opoisoned){
				sprintf(buf, "%s %s",
					s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
				poisoned(buf, A_CON, mtmp->data->mname, 30);
			}
			
			if (dmgplus <= 0) dmgplus = 1;
			if (!(uwep->oartifact &&
				artifact_hit(mtmp, &youmonst, uwep, &dmgplus,dieroll)))
			     hitmsg(mtmp, mattk);
		 }
             mdamageu(mtmp, d(3,8) + dmgplus);
		}
		break;
	    case AD_DREA:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s saps your soul...", Monnam(mtmp));

			if (multi < 0) {
				dmgplus *= 4;
				pline("Your dream is eaten!");
			}
	             mdamageu(mtmp, 1 + dmgplus);
		}

		break;

	    case AD_BADE:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(15))) {
                pline("%s jinxes you!", Monnam(mtmp));

			badeffect();

		}

		break;

	    case AD_RBAD:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(15))) {
                pline("%s hexes you!", Monnam(mtmp));

			reallybadeffect();

		}

		break;

	    case AD_BLEE:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(4))) {
                pline("%s points at you, incanting terribly!", Monnam(mtmp));

			if (dmgplus > 0) playerbleed(dmgplus);
		}

		break;

	    case AD_SHAN:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(12))) {
                pline("%s commands you to take your clothes off!", Monnam(mtmp));
			shank_player();
		}

		break;

	    case AD_DEBU:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(12))) {
                pline("%s fires a sapping beam!", Monnam(mtmp));
			statdebuff();
		}

		break;

	    case AD_UNPR:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(20))) {
			pline("%s sends airborne covid-19 viri your way!", Monnam(mtmp));
			struct obj *obj = some_armor(&youmonst);
			if (obj && obj->oerodeproof) {
				obj->oerodeproof = FALSE;
				Your("%s brown for a moment.", aobjnam(obj, "glow"));
			}
		}

		break;

	    case AD_NIVE:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(12))) {
                pline("%s reminds you that you won't ascend this time!", Monnam(mtmp));
			nivellate();
		}
		break;

	    case AD_SCOR:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(3))) {
                pline("%s deducts some of your points...", Monnam(mtmp));
			u.urexp -= (dmgplus * 50);
			if (u.urexp < 0) u.urexp = 0;
			Your("score is drained!");
		}

		break;

	    case AD_TERR:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(20))) {
                pline("%s casts chaos terrain!", Monnam(mtmp));
			terrainterror();
		}

		break;

	    case AD_FEMI:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(20))) {
                pline("%s gazes at you in a very female way!", Monnam(mtmp));
			randomfeminismtrap(rnz( (dmgplus + 2) * rnd(100)));
			if (Role_if(PM_CLIMACTERIAL)) climtrainsqueaking(dmgplus);
		}

		break;

	    case AD_LEVI:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(7))) {
                pline("%s elevates you!", Monnam(mtmp));
			You("float up!");
			HLevitation &= ~I_SPECIAL;
			incr_itimeout(&HLevitation, dmgplus);
		}

		break;

	    case AD_MCRE:

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s puts a hex on you!", Monnam(mtmp));
			MCReduction += (100 * dmgplus);
			pline("The magic cancellation granted by your armor seems weaker now...");
		}

		break;

	    case AD_FUMB:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s's gaze causes you to fumble!", Monnam(mtmp));

			HFumbling = FROMOUTSIDE | rnd(5);
			incr_itimeout(&HFumbling, rnd(20));
			u.fumbleduration += rnz(10 * (dmgplus + 1) );
		}

		break;

	    case AD_TREM:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s's gaze causes you to tremble!", Monnam(mtmp));

			u.tremblingamount++;
		}

		break;

	    case AD_IDAM:

	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s throws a wrench into your gears!", Monnam(mtmp));

			{
			    register struct obj *objX, *objX2;
			    for (objX = invent; objX; objX = objX2) {
			      objX2 = objX->nobj;
				if (!rn2(20)) rust_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
			    }
			}
		}

		break;

	    case AD_ANTI:

	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s uses %s acu phaser to breach your defenses!", Monnam(mtmp), mhis(mtmp));
			if (FunnyHallu) You_feel("reminded of Mandevil's Junethack 2018 feat.");

			{
			    register struct obj *objX, *objX2;
			    for (objX = invent; objX; objX = objX2) {
			      objX2 = objX->nobj;
				if (!rn2(20)) wither_dmg(objX, xname(objX), rn2(4), TRUE, &youmonst);
			    }
			}
		}

		break;

	    case AD_PAIN:

		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(3))) {
            	pline("%s spikes you!", Monnam(mtmp));
			losehp(Upolyd ? ((u.mh / 10) + 1) : ((u.uhp / 10) + 1), "a painful gaze", KILLED_BY);
		}

		break;

	    case AD_TECH:

		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(7))) {
            	pline("%s hacks your account!", Monnam(mtmp));
			techcapincrease(dmgplus * rnd(50));
		}

		break;

	    case AD_MEMO:

		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(7))) {
            	pline("%s shoots mind waves at you!", Monnam(mtmp));
			spellmemoryloss(dmgplus);
		}

		break;

	    case AD_TRAI:

		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(7))) {
            	pline("%s steals your training effort!", Monnam(mtmp));
			skilltrainingdecrease(dmgplus);
			if (!rn2(100)) skillcaploss();
		}

		break;

	    case AD_STAT:

		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(10))) {
            	pline("%s tries to drain your very soul!", Monnam(mtmp));

			if (!rn2(3)) {
				statdrain();
			}
		}

		break;

	    case AD_VULN:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
            	pline("%s laughs devilishly!", Monnam(mtmp));
			deacrandomintrinsic(rnz( (dmgplus * rnd(30) ) + 1));
		}

		break;

	    case AD_NACU:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(25))) {

                pline("%s points at you and mumbles an especially heinous curse!", Monnam(mtmp));
			nastytrapcurse();

		}

		break;

	    case AD_ICUR:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s points at you and mumbles an arcane incantation!", Monnam(mtmp));

			if (!rn2(5)) {
				You_feel("as if you need some help.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
				rndcurse();
			}
		}

		break;

	    case AD_SLUD:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s used SLUDGE BOMB!", Monnam(mtmp));

			{
			    register struct obj *objX, *objX2;
			    for (objX = invent; objX; objX = objX2) {
			      objX2 = objX->nobj;
				if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
			    }
			}
		}

		break;

	    case AD_NAST:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(15))) {
			pline("%s uses a Topi Ylinen curse on you!", Monnam(mtmp));

			if (!rn2(10)) {
				pline("The ancient foul curse takes possession of you...");
				randomnastytrapeffect(rnz( (dmgplus + 2) * rnd(100) ), 1000 - (dmgplus * 3));
			}
		}

		break;

	    case AD_DRST:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_STR, mtmp->data->mname, 30);
	        }
	        break;
	    case AD_DRDX:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_DEX, mtmp->data->mname, 30);
	        }
	        break;
	    case AD_DRCO:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_CON, mtmp->data->mname, 30);
	        }
	        break;
	    case AD_WISD:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_WIS, mtmp->data->mname, 30);
	        }
	        break;
	    case AD_DRCH:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_CHA, mtmp->data->mname, 30);
	        }
	        break;
	    case AD_POIS:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", rn2(A_MAX), mtmp->data->mname, 30);
	        }
	        break;
	    case AD_VENO:
		  if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s stares into your eyes...", Monnam(mtmp));

			if (chromeprotection()) break;

			if (!Poison_resistance) pline("You're badly poisoned!");
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE, TRUE);
			if (!rn2( (Poison_resistance && rn2(StrongPoison_resistance ? 20 : 5) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE, TRUE);
	                poisoned("The gaze", rn2(A_MAX), mtmp->data->mname, 30);
			if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(POTION_CLASS, AD_VENO);
			if (isevilvariant || !rn2(issoviet ? 2 : 20)) (void)destroy_item(FOOD_CLASS, AD_VENO);
		}
		break; 

	    case AD_NPRO:

		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(25))) {
                pline("%s gives you an excruciating look!", Monnam(mtmp));
		    stop_occupation();
			u.negativeprotection++;
			if (evilfriday && u.ublessed > 0) {
				u.ublessed -= 1;
				if (u.ublessed < 0) u.ublessed = 0;
			}
		}
		break;

          case AD_DISE:
		if (rn2(3)) break; /* lower chance for normal disease, so pestilence attack is unique --Amy */
          case AD_PEST:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(12))) {
	                pline("%s leers down on you!", Monnam(mtmp));
		    stop_occupation();
			(void) diseasemu(mtmp->data); /* plus the normal damage */
	        }
	        break;

	    case AD_SPEL:
	    case AD_CAST:
	    case AD_CLRC:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(10))) {
			castmu(mtmp, mattk, TRUE, TRUE);
		  }
		break;

          case AD_VOMT:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(12))) {
	                pline("%s throws a sickening gaze at you!", Monnam(mtmp));
		    stop_occupation();

			if (!rn2(StrongSick_resistance ? 100 : 10) || !Sick_resistance) {
				if (!Vomiting) {
					make_vomiting(Vomiting+d(10,4), TRUE);
					pline("You feel nauseated.");
					if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10);
				} else if (!rn2(2)) diseasemu(mtmp->data);
			}

	        }
	        break;

	    case AD_CHRN:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(12))) {
	                pline("%s gazes at you and curses horribly.", Monnam(mtmp));
		    stop_occupation();

		    if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: diseasemu(mtmp->data);
			    break;
		    case 1: make_blinded(Blinded + dmgplus, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    FunnyHallu ? "trippy" : "confused");
			    make_confused(HConfusion + dmgplus, TRUE);
			    break;
		    case 3: make_stunned(HStun + dmgplus, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + dmgplus, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + dmgplus, TRUE);
			    break;
		    case 6: make_burned(HBurned + dmgplus, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + dmgplus, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + dmgplus, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + dmgplus, TRUE);
			    break;
		    }

		}
		break;

	    case AD_FREN:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(6))) {
	                pline("%s shouts 'You motherfucker with your %s penis!'", Monnam(mtmp), flags.female ? "nonexistant" : "little");
		    stop_occupation();

			if (u.berserktime) {
			    if (!obsidianprotection()) switch (rn2(11)) {
			    case 0: diseasemu(mtmp->data);
				    break;
			    case 1: make_blinded(Blinded + dmgplus, TRUE);
				    break;
			    case 2: if (!Confusion)
					You("suddenly feel %s.",
					    FunnyHallu ? "trippy" : "confused");
				    make_confused(HConfusion + dmgplus, TRUE);
				    break;
			    case 3: make_stunned(HStun + dmgplus, TRUE);
				    break;
			    case 4: make_numbed(HNumbed + dmgplus, TRUE);
				    break;
			    case 5: make_frozen(HFrozen + dmgplus, TRUE);
				    break;
			    case 6: make_burned(HBurned + dmgplus, TRUE);
				    break;
			    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
				    break;
			    case 8: (void) make_hallucinated(HHallucination + dmgplus, TRUE, 0L);
				    break;
			    case 9: make_feared(HFeared + dmgplus, TRUE);
				    break;
			    case 10: make_dimmed(HDimmed + dmgplus, TRUE);
				    break;
			    }
			} else u.berserktime = dmgplus;

		}
		break;

	    case AD_HALU:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(5))) 		{
		    boolean chg;
		    if (!Hallucination)
			You("suddenly see a mess of colors!");
		    stop_occupation();
		    chg = make_hallucinated(HHallucination + dmgplus,FALSE,0L);
		    You("%s.", chg ? "are getting very trippy" : "seem to get even more trippy");
		}
		break;
	    case AD_PLYS:
	        if(!mtmp->mcan && multi >= 0 && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s stares at you!", Monnam(mtmp));
	                if (Free_action && rn2(StrongFree_action ? 100 : 20)) You("stiffen momentarily.");
	                else {
	                        You("are frozen by %s!", mon_nam(mtmp));
					if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
					nomovemsg = 0;
					{
						int paralysistime = dmgplus;
						if (paralysistime > 1) paralysistime = rnd(paralysistime);
						if (paralysistime > 5) {
							while (rn2(5) && (paralysistime > 5)) {
								paralysistime--;
							}
						}
						if (!rn2(3)) nomul(-rnd(4), "paralyzed by a monster's gaze", TRUE);
						else nomul(-(paralysistime), "paralyzed by a monster's gaze", TRUE);
						exercise(A_DEX, FALSE);
					}
	                }
	        }
	        break;
	    case AD_TLPT:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(15))) {
	                pline("%s stares blinkingly at you!", Monnam(mtmp));
		    stop_occupation();
	                if(flags.verbose)
	                        Your("position suddenly seems very uncertain!");
	                teleX();
		}
		break;

	    case AD_ABDC:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used) {
	                pline("%s stares blinkingly at you!", Monnam(mtmp));
		    stop_occupation();
	                if(flags.verbose)
	                        Your("position suddenly seems very uncertain!");
	                teleX();
		}
		break;

	    case AD_DISP:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s telepathically tries to move you around!", Monnam(mtmp));
		    stop_occupation();
		pushplayer(FALSE);
            if (!rn2(5)) mdamageu(mtmp, (1 + dmgplus));
		}
		break;
	    default: /*impossible*/pline("Gaze attack %d?", mattk->adtyp);
		break;
	}

	/*if (randattackB == 1) {
		mattk->adtyp = AD_RBRE;
		randattackB = 0;
	}*/

	return(0);
}

#endif /* OVLB */
#ifdef OVL1

void
mdamageu(mtmp, n)	/* mtmp hits you for n points damage */
register struct monst *mtmp;
register int n;
{
	int monsterdamagebonus;
	int enchrequired = 0;

	if (flags.iwbtg) {

		You("explode in a fountain of red pixels!");
		pline("GAME OVER - press R to try again");
		done_in_by(mtmp);

	}

	if (uimplant && uimplant->oartifact == ART_NO_ABNORMAL_FUTURE) {
		if (!rn2(20)) witherarmor();
		if (!rn2(500)) antimatter_damage(invent, FALSE, FALSE);
	}

	if (ExplodingDiceEffect || u.uprops[EXPLODING_DICE].extrinsic || have_explodingdicestone() || (uwep && uwep->oartifact == ART_FOOK_THE_OBSTACLES) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_FOOK_THE_OBSTACLES)) {
		int basedamage = n;
		while (!rn2(6)) n += basedamage;
	}

	/* sometimes you take less damage. The game is deadly enough already. High constitution helps. --Amy */
	if (!issoviet && rn2(ABASE(A_CON))) {
	if (!rn2(3) && n >= 1) {n++; n = n / 2; if (n < 1) n = 1;}
	if (!rn2(10) && rn2(ABASE(A_CON)) && n >= 1 && GushLevel >= 10) {n++; n = n / 3; if (n < 1) n = 1;}
	if (!rn2(15) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && n >= 1 && GushLevel >= 14) {n++; n = n / 4; if (n < 1) n = 1;}
	if (!rn2(20) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && n >= 1 && GushLevel >= 20) {n++; n = n / 5; if (n < 1) n = 1;}
	if (!rn2(50) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && rn2(ABASE(A_CON)) && n >= 1 && GushLevel >= 30) {n++; n = n / 10; if (n < 1) n = 1;}
	}

	if (PlayerInConeHeels && n > 0) {
		register int dmgreductor = 95;
		if (!(PlayerCannotUseSkills)) switch (P_SKILL(P_HIGH_HEELS)) {
			case P_BASIC: dmgreductor = 92; break;
			case P_SKILLED: dmgreductor = 89; break;
			case P_EXPERT: dmgreductor = 86; break;
			case P_MASTER: dmgreductor = 83; break;
			case P_GRAND_MASTER: dmgreductor = 80; break;
			case P_SUPREME_MASTER: dmgreductor = 77; break;
		}
		n++;
		n *= dmgreductor;
		n /= 100;
		if (n < 1) n = 1;
	}

	if (n > 0 && StrongDetect_monsters) {
		n++;
		n *= 9;
		n /= 10;
		if (n < 1) n = 1;
	}

	if (Race_if(PM_ITAQUE) && n > 0) {
		n++;
		n *= (100 - u.ulevel);
		n /= 100;
		if (n < 1) n = 1;
	}

	if (Race_if(PM_CARTHAGE) && u.usteed && (mcalcmove(u.usteed) < 12) && n > 0) {
		n++;
		n *= 4;
		n /= 5;
		if (n < 1) n = 1;
	}

	if (is_sand(u.ux,u.uy) && n > 0) {
		n++;
		n *= 4;
		n /= 5;
		if (n < 1) n = 1;
	}

	if (Race_if(PM_VIKING) && n > 0) {
		n *= 5;
		n /= 4;
	}

	if (Race_if(PM_JAVA) && n > 0) {
		n *= 5;
		n /= 4;
	}

	if (Race_if(PM_SPARD) && n > 0) {
		n *= 5;
		n /= 4;
	}

	if (Race_if(PM_MAYMES) && uwep && weapon_type(uwep) == P_FIREARM && n > 0) {
		n++;
		n *= 4;
		n /= 5;
		if (n < 1) n = 1;
	}

	if (Race_if(PM_GERTEUT) && n > 0) {
		n++;
		n *= 4;
		n /= 5;
		if (n < 1) n = 1;
	}

	if (n > 0 && uarmf && itemhasappearance(uarmf, APP_MARJI_SHOES) ) {
		n++;
		n *= 9;
		n /= 10;
		if (n < 1) n = 1;
	}

	/* very early on, low-level characters should be more survivable
	 * this can certainly be exploited in some way; if players start exploiting it I'll have to fix it
	 * but it should fix the annoying problem where you often instadie to a trap while your max HP are bad --Amy */
	if (depth(&u.uz) == 1 && u.ulevel == 1 && moves < 1000 && In_dod(&u.uz) && n > 1) { n /= 2; }
	if (depth(&u.uz) == 1 && u.ulevel == 2 && moves < 1000 && In_dod(&u.uz) && n > 1) { n *= 2; n /= 3; }

	if (n && Race_if(PM_YUKI_PLAYA)) n += rnd(5);
	if (Role_if(PM_BLEEDER)) n = n * 2; /* bleeders are harder than hard mode */
	if (have_cursedmagicresstone()) n = n * 2;
	if (Role_if(PM_DANCER) && !rn2(3)) n = n * 2;
	if (Race_if(PM_METAL)) n *= rnd(10);
	if (HardModeEffect || u.uprops[HARD_MODE_EFFECT].extrinsic || have_hardmodestone() || (uleft && uleft->oartifact == ART_RING_OF_FAST_LIVING) || (uright && uright->oartifact == ART_RING_OF_FAST_LIVING) || (uimplant && uimplant->oartifact == ART_IME_SPEW)) n = n * 2;
	if (uamul && uamul->otyp == AMULET_OF_VULNERABILITY) n *= rnd(4);
	if (RngeFrailness) n = n * 2;

	if (Race_if(PM_SHELL) && !Upolyd && n > 1) n /= 2;

	if (isfriday && !rn2(50)) n += rnd(n);

	if (Invulnerable || (StrongWonderlegs && !rn2(10) && Wounded_legs) || (Stoned_chiller && Stoned)) n=0;

	if (u.metalguard) {
		u.metalguard = 0;
		n = 0;
		Your("metal guard prevents the damage!");
	}

	if (n == 0) {
		pline("You are unharmed.");
		return;
	}

	if (uarmf && uarmf->oartifact == ART_STAR_SOLES) enchrequired = 1;
	if (Race_if(PM_PLAYER_SKELETON)) enchrequired = 2;
	if (uarmf && uarmf->oartifact == ART_PHANTO_S_RETARDEDNESS) enchrequired = 4;

	if ((enchrequired > 0) && rn2(3) && !(hit_as_four(mtmp) || (hit_as_three(mtmp) && enchrequired < 4) || (hit_as_two(mtmp) && enchrequired < 3) || (hit_as_one(mtmp) && enchrequired < 2) || (MON_WEP(mtmp) && (MON_WEP(mtmp))->spe >= enchrequired) ) ) {
		pline("The attack doesn't seem to harm you.");
		n = 0;
	}

	/* WAC For consistency...DO be careful using techniques ;B */
	if (mtmp->mtame != 0 && tech_inuse(T_PRIMAL_ROAR)) {
		n *= 2; /* Double Damage! */
	}

	monsterdamagebonus = 100;

	{

		int overlevelled = 0;
		if (mtmp->m_lev > mtmp->data->mlevel) overlevelled = ((mtmp->m_lev - mtmp->data->mlevel) * 5);
		if (overlevelled > 0) {
			monsterdamagebonus += overlevelled;
		}
	}

	if (mtmp->egotype_champion) monsterdamagebonus += 25;
	if (mtmp->egotype_boss) monsterdamagebonus += 75;
	if (mtmp->egotype_atomizer) monsterdamagebonus += 200;

	/* high-level monsters gain much more damage bonus versus you than high-level pets get damage bonus versus
	 * other monsters; this isn't FIQhack after all, full symmetry is not the focus of this game. --Amy */

	if (monsterdamagebonus > 100 && (n > 1 || (n == 1 && monsterdamagebonus >= 150) )) {

		n *= monsterdamagebonus;
		n /= 100;

	}

	flags.botl = 1; /* This needs to be AFTER the pline for botl to be 
	 		 * updated correctly -- Kelly Bailey
	 		 */

	if (uactivesymbiosis && !u.symbiotedmghack && (rn2(100) < u.symbioteaggressivity) && !(u.usymbiote.mhpmax >= 5 && u.usymbiote.mhp <= (u.usymbiote.mhpmax / 5) && rn2(5))) {
		if (tech_inuse(T_POWERBIOSIS) && n > 1) n /= 2;
		if (tech_inuse(T_IMPLANTED_SYMBIOSIS) && uimplant && objects[uimplant->otyp].oc_charged && uimplant->spe > 0) {
			int imbiophases = uimplant->spe;
			while ((imbiophases > 0) && n > 1) {
				imbiophases--;
				n *= 10;
				n /= 11;
			}
		}
		u.usymbiote.mhp -= n;
		Your("%s symbiote takes the damage for you.", mons[u.usymbiote.mnum].mname);
		if (u.usymbiote.mhp <= 0) {
			u.usymbiote.active = 0;
			u.usymbiote.mnum = PM_PLAYERMON;
			u.usymbiote.mhp = 0;
			u.usymbiote.mhpmax = 0;
			u.usymbiote.cursed = u.usymbiote.hvycurse = u.usymbiote.prmcurse = u.usymbiote.bbcurse = u.usymbiote.morgcurse = u.usymbiote.evilcurse = u.usymbiote.stckcurse = 0;
			u.cnd_symbiotesdied++;
			if (FunnyHallu) pline("Ack! You feel like you quaffed aqua pura by mistake, and feel like something inside you has been flushed away!");
			else Your("symbiote dies from protecting you, and you feel very sad...");
		}
		if (flags.showsymbiotehp) flags.botl = TRUE;
	} else if (u.disruptionshield && u.uen >= n) {
		u.uen -= n;
		pline("Your mana shield takes the damage for you!");
		flags.botl = 1;

	} else if (Upolyd) {
		u.mh -= n;
		if (u.mh < 1) {                
			if (Polymorph_control || !rn2(3)) {
			    u.uhp -= mons[u.umonnum].mlevel;
			    /*u.uhpmax -= mons[u.umonnum].mlevel;
			    if (u.uhpmax < 1) u.uhpmax = 1;*/
			}
			rehumanize();
		}
	} else {
		u.uhplast = u.uhp;
		u.uhp -= n;
		if (u.uhp >= 1 && n > 0 && u.uhp*10 < u.uhpmax)	maybe_wail(); /* Wizard is about to die. --Amy */
		if(u.uhp < 1) done_in_by(mtmp);
	}

#ifdef SHOW_DMG
	if (flags.showdmg && !(DamageMeterBug || u.uprops[DAMAGE_METER_BUG].extrinsic || have_damagemeterstone()) && !DisplayDoesNotGoAtAll ) {

		pline("[-%d -> %d]", n, (Upolyd ? (u.mh) : (u.uhp) ) );  /* WAC see damage */
		if (!Upolyd && (( (u.uhp) * 5) < u.uhpmax)) pline(isangbander ? "***LOW HITPOINT WARNING***" : "Warning: HP low!");
		if (isangbander && (!Upolyd && (( (u.uhp) * 5) < u.uhpmax)) && (PlayerHearsSoundEffects)) pline(issoviet ? "Umeret' glupyy igrok ublyudka!" : "TSCHINGTSCHINGTSCHINGTSCHING!");

	}
#endif

	if (u.uprops[TURNLIMITATION].extrinsic || (uarmf && uarmf->oartifact == ART_OUT_OF_TIME) || (uarmu && uarmu->oartifact == ART_THERMAL_BATH) || TurnLimitation || have_limitationstone() ) {
		if (n > 0) u.ascensiontimelimit -= n;
		if (u.ascensiontimelimit < 1) u.ascensiontimelimit = 1;
	}

	if (Race_if(PM_CELTIC) && !rn2(100)) {
		if (u.berserktime) {
			if (!obsidianprotection()) switch (rn2(11)) {
			case 0:
				make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20), "celtic sickness", TRUE, SICK_NONVOMITABLE);
				break;
			case 1: make_blinded(Blinded + 25, TRUE);
				break;
			case 2: if (!Confusion)
				You("suddenly feel %s.", FunnyHallu ? "trippy" : "confused");
				make_confused(HConfusion + 25, TRUE);
				break;
			case 3: make_stunned(HStun + 25, TRUE);
				break;
			case 4: make_numbed(HNumbed + 25, TRUE);
				break;
			case 5: make_frozen(HFrozen + 25, TRUE);
				break;
			case 6: make_burned(HBurned + 25, TRUE);
				break;
			case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
				break;
			case 8: (void) make_hallucinated(HHallucination + 25, TRUE, 0L);
				break;
			case 9: make_feared(HFeared + 25, TRUE);
				break;
			case 10: make_dimmed(HDimmed + 25, TRUE);
				break;
			}

		} else u.berserktime = 25;
	}

}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL void
urustm(mon, obj)
register struct monst *mon;
register struct obj *obj;
{
	boolean vis;
	boolean is_acid;

	if (!mon || !obj) return; /* just in case */
	if (dmgtype(youmonst.data, AD_CORR))
	    is_acid = TRUE;
	else if (dmgtype(youmonst.data, AD_RUST))
	    is_acid = FALSE;
	else
	    return;

	vis = cansee(mon->mx, mon->my);

	if ((is_acid ? is_corrodeable(obj) : is_rustprone(obj)) && !stack_too_big(obj) &&
	    (is_acid ? obj->oeroded2 : obj->oeroded) < MAX_ERODE) {
		if (obj->greased || (obj->oartifact && rn2(4)) || obj->oerodeproof || (obj->blessed && rn2(3))) {
		        if (vis) pline("Somehow, %s weapon is not affected.",
						s_suffix(mon_nam(mon)));
		    if (obj->greased && !rn2(2)) obj->greased -= 1;
		} else {
		        if (vis) pline("%s %s%s!",
			        s_suffix(Monnam(mon)),
				aobjnam(obj, (is_acid ? "corrode" : "rust")),
			        (is_acid ? obj->oeroded2 : obj->oeroded)
				    ? " further" : "");
		    if (is_acid) obj->oeroded2++;
		    else obj->oeroded++;
		}
	}
}

#endif /* OVLB */
#ifdef OVL1

int
could_seduce(magr,mdef,mattk)
struct monst *magr, *mdef;
struct attack *mattk;
/* returns 0 if seduction impossible,
 *	   1 if fine,
 *	   2 if wrong gender for nymph */
{
	register struct permonst *pagr;
	boolean agrinvis, defperc;
	xchar genagr, gendef;

	if (rn2(5) && is_animal(magr->data)) return (0); /* Oh come on. In Elona snails can have sex with humans too. --Amy */
	if(magr == &youmonst) {
		pagr = youmonst.data;
		agrinvis = (Invis != 0);
		genagr = poly_gender();
	} else {
		pagr = magr->data;
		agrinvis = magr->minvis;
		genagr = gender(magr);
	}
	if(mdef == &youmonst) {
		defperc = (See_invisible != 0);
		gendef = poly_gender();
	} else {
		defperc = perceives(mdef->data);
		gendef = gender(mdef);
	}

	if(agrinvis && !defperc
		&& mattk && mattk->adtyp != AD_SSEX
		)
		return 0;

/*	if(pagr->mlet != S_NYMPH
		&& ((pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS])
		    || (mattk && mattk->adtyp != AD_SSEX)
		   ))
		return 0; */

	if (mattk && mattk->adtyp != AD_SSEX && mattk->adtyp != AD_SEDU && mattk->adtyp != AD_SITM && pagr->mlet != S_NYMPH
&& pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS]) return 0;
	
	if(genagr == 1 - gendef)
		return 1;
	else
		/* Not everyone is straight... --Amy */
		return (!rn2(25)) ? 1 : (pagr->mlet == S_NYMPH) ? 2 : 0;
}

int
could_seduceX(magr,mdef,mattk)
struct monst *magr, *mdef;
struct attack *mattk;
/* returns 0 if seduction impossible,
 *	   1 if fine,
 *	   2 if wrong gender for nymph */
{
	register struct permonst *pagr;
	boolean agrinvis, defperc;
	xchar genagr, gendef;
	char buf[BUFSZ];

	if (u.homosexual == 0) {
		pline("You're discovering your sexuality...");
		getlin("Are you homosexual? [y/yes/no] (If you answer no, you're heterosexual.)", buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) ) u.homosexual = 2;
		else u.homosexual = 1;
	}

	if (ublindf && ublindf->oartifact == ART_CLICKPASS) return 0; /* immunity */

	if (rn2(5) && is_animal(magr->data)) return (0); /* Oh come on. In Elona snails can have sex with humans too. --Amy */

	if(magr == &youmonst) {
		pagr = youmonst.data;
		agrinvis = (Invis != 0);
		genagr = poly_gender();
	} else {
		pagr = magr->data;
		agrinvis = magr->minvis;
		genagr = gender(magr);
	}
	if(mdef == &youmonst) {
		defperc = (See_invisible != 0);
		gendef = poly_gender();
	} else {
		defperc = perceives(mdef->data);
		gendef = gender(mdef);
	}

	if(agrinvis && !defperc
		&& mattk && mattk->adtyp != AD_SSEX
		)
		return 0;

/*	if(pagr->mlet != S_NYMPH
		&& ((pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS])
		    || (mattk && mattk->adtyp != AD_SSEX)
		   ))
		return 0; */

	/*if (mattk && mattk->adtyp != AD_SSEX && mattk->adtyp != AD_SEDU && mattk->adtyp != AD_SITM && pagr->mlet != S_NYMPH
&& pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS]) return 0;*/

	if(genagr == 1 - gendef)
		return 1;
	else
		/* Not everyone is straight... --Amy */
		return (!rn2(25)) ? 1 : (pagr == &mons[PM_FEMME]) ? 1 : (pagr == &mons[PM_FAGA]) ? 1 : (pagr == &mons[PM_GAY_NAGA]) ? 1 : (pagr == &mons[PM_EMERGENCY_HORNY_TEENAGER]) ? 1 : (u.homosexual == 2) ? 1 : (pagr->mlet == S_NYMPH) ? 2 : 0;
}

#endif /* OVL1 */
#ifdef OVLB

/* Returns 1 if monster teleported */
int
doseduce(mon)
register struct monst *mon;
{
	int monsterlev;
	register struct obj *ring, *nring;
	boolean fem = /*(mon->data == &mons[PM_SUCCUBUS])*/ (mon->female); /* otherwise incubus */
	char qbuf[QBUFSZ];
	boolean birthing = 0;
	boolean complications = 0;

	if (uarmc && itemhasappearance(uarmc, APP_BIRTHCLOTH)) birthing = 1;
	else if (RngeChildbirth) birthing = 1;
	else if (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) birthing = 1;
	else if (Role_if(PM_GRENADONIN) && mon->data->mcolor == CLR_BLACK) birthing = 1;
	else birthing = 0;

	if (uarmc && uarmc->oartifact == ART_CATHERINE_S_SEXUALITY) complications = 1;

	if (mon->mcan || mon->mspec_used) {
		pline("%s acts as though %s has got a %sheadache.",
		      Monnam(mon), mhe(mon),
		      mon->mcan ? "severe " : "");
		return 0;
	}

	if (unconscious()) {
		pline("%s seems dismayed at your lack of response.",
		      Monnam(mon));
		return 0;
	}

	if (Blind) pline("It caresses you...");
	else You_feel("very attracted to %s.", mon_nam(mon));

	if (mon->data == &mons[PM_GAY_MOOSE]) {
		if (u.homosexual != 2) You("got turned, and are now gay.");
		u.homosexual = 2;
	}

	for(ring = invent; ring; ring = nring) {
	    nring = ring->nobj;
	    if (ring->otyp != RIN_ADORNMENT) continue;
	    if (fem) {
		if (rn2(120) < ACURR(A_CHA)) {
		    sprintf(qbuf, "\"That %s looks pretty.  May I have it?\"",
			safe_qbuf("",sizeof("\"That  looks pretty.  May I have it?\""),
			xname(ring), simple_typename(ring->otyp), "ring"));
		    makeknown(RIN_ADORNMENT);
		    if (yn(qbuf) == 'n') continue;
		} else pline("%s decides she'd like your %s, and takes it.",
			Blind ? "She" : Monnam(mon), xname(ring));
		makeknown(RIN_ADORNMENT);
		if (ring==uleft || ring==uright) Ring_gone(ring);
		if (ring==uwep) setuwep((struct obj *)0, FALSE, TRUE);
		if (ring==uswapwep) setuswapwep((struct obj *)0, FALSE);
		if (ring==uquiver) setuqwep((struct obj *)0);
		freeinv(ring);
		(void) mpickobj(mon,ring,FALSE);
	    } else {
		char buf[BUFSZ];

		if (uleft && uright && uleft->otyp == RIN_ADORNMENT
				&& uright->otyp==RIN_ADORNMENT)
			break;
		if (ring==uleft || ring==uright) continue;
		if (rn2(120) < ACURR(A_CHA)) {
		    sprintf(qbuf,"\"That %s looks pretty.  Would you wear it for me?\"",
			safe_qbuf("",
			    sizeof("\"That  looks pretty.  Would you wear it for me?\""),
			    xname(ring), simple_typename(ring->otyp), "ring"));
		    makeknown(RIN_ADORNMENT);
		    if (yn(qbuf) == 'n') continue;
		} else {
		    pline("%s decides you'd look prettier wearing your %s,",
			Blind ? "He" : Monnam(mon), xname(ring));
		    pline("and puts it on your finger.");
		}
		makeknown(RIN_ADORNMENT);
		if (!uright) {
		    pline("%s puts %s on your right %s.",
			Blind ? "He" : Monnam(mon), the(xname(ring)), body_part(HAND));
		    setworn(ring, RIGHT_RING);
		} else if (!uleft) {
		    pline("%s puts %s on your left %s.",
			Blind ? "He" : Monnam(mon), the(xname(ring)), body_part(HAND));
		    setworn(ring, LEFT_RING);
		} else if (uright && uright->otyp != RIN_ADORNMENT) {
		    strcpy(buf, xname(uright));
		    pline("%s replaces your %s with your %s.",
			Blind ? "He" : Monnam(mon), buf, xname(ring));
		    Ring_gone(uright);
		    setworn(ring, RIGHT_RING);
		} else if (uleft && uleft->otyp != RIN_ADORNMENT) {
		    strcpy(buf, xname(uleft));
		    pline("%s replaces your %s with your %s.",
			Blind ? "He" : Monnam(mon), buf, xname(ring));
		    Ring_gone(uleft);
		    setworn(ring, LEFT_RING);
		} else impossible("ring replacement");
		Ring_on(ring);
		prinv((char *)0, ring, 0L);
	    }
	}

	if (!uarmc && !uarmf && !uarmg && !uarms && !uarmh
								&& !uarmu
									)
		pline("%s murmurs sweet nothings into your ear.",
			Blind ? (fem ? "She" : "He") : Monnam(mon));
	else
		pline("%s murmurs in your ear, while helping you undress.",
			Blind ? (fem ? "She" : "He") : Monnam(mon));
	mayberem(uarmc, cloak_simple_name(uarmc));
	if(!uarmc)
		mayberem(uarm, "suit");
	mayberem(uarmf, "boots");
	if(!uwep || !welded(uwep))
		mayberem(uarmg, "gloves");
	/* 
	 * STEPHEN WHITE'S NEW CODE
	 *
	 * This will cause a game crash should the if statment be removed.
	 * It will try to de-referance a pointer that doesn't exist should 
	 * the player not have a shield
	 */

	if (uarms) mayberem(uarms, "shield");
	mayberem(uarmh, "helmet");
	if(!uarmc && !uarm)
		mayberem(uarmu, "shirt");

	if ((uarm && !(uarm->oartifact == ART_CHASTITY_ARMOR || uarm->oartifact == ART_LITTLE_PENIS_WANKER)) || uarmc) {
		verbalize("You're such a %s; I wish...",
				flags.female ? "sweet lady" : "nice guy");
		if (!tele_restrict(mon)) (void) rloc(mon, FALSE);
		return 1;
	}
	if (u.ualign.type == A_CHAOTIC)
		adjalign(1);

	/* yay graphical descriptions! --Amy */

	if (!issoviet) {
	if (!flags.female) pline("%s starts to gently pull down your pants with her soft, fleecy hands...", Monnam(mon));
	else pline("%s softly caresses your fleecy bra, and gently pulls it off to reveal your breasts...", Monnam(mon));
	}

	/* yay ULTRA graphical descriptions! I should suggest them to the noxico devs :P --Amy */
	if (mon->data == &mons[PM_FEMME]) {
		if (!flags.female) pline("You joyously experience pure lust as %s massages your delicate nuts.", mon_nam(mon));
		else pline("Full of joy, %s strokes your soft breasts, kneading them very tenderly.", mon_nam(mon));
	}

	if (mon->data == &mons[PM_UTE] && !mon->mfrenzied && !mon->mpeaceful) mon->mpeaceful = TRUE;

	/* "Remove a stupid line of dialogue. This is not an adult visual novel.  The rest of the dialogue scattered around the source files like this will be cleaned up in due time." In Soviet Russia, people are filthy heretics who don't fully appreciate the beauty of Slash'EM Extended, which causes them to pick the best features of the game and remove them. :( --Amy */

	if (u.homosexual == 2 && (flags.female && mon->female)) goto skiptreason;
	if (u.homosexual == 2 && (!flags.female && !(mon->female))) goto skiptreason;

	if ((rnd(ACURR(A_CHA)) < ((u.homosexual == 2) ? 9 : 3) ) && (mon->data != &mons[PM_UTE]) && (mon->data != &mons[PM_FEMME] || !rn2(3)) ) { /* random chance of being betrayed by your love interest... */

		monsterlev = ((mon->m_lev) + 1);
		if (monsterlev <= 0) monsterlev = 1;

		if (!flags.female) { pline("But %s suddenly rams her sexy knees right into your nuts! OUCH!", Monnam(mon));
			losehp(d(3,monsterlev), "treacherous lady", KILLED_BY_AN);
		}
		if (flags.female) { pline("But all of a sudden, %s clenches his burly hands to a fist and violently punches your breasts! AIIIEEEEEGGGGGHHHHH!", Monnam(mon));
			losehp(d(3,monsterlev), "filthy traitor", KILLED_BY_AN);
		}
		/* I declare the generic "his" a feature, see below :P --Amy */

		if (mon->data == &mons[PM_FEMME]) {
			if (!flags.female) pline("Wow, it sure was fun having your nuts squeezed by such a tender woman. Her female knee is very sexy!");
			else pline("Well that was a letdown, the announcer used the wrong pronoun for the femme. But at least the pain was enjoyable!");
		}

		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

		return 1;
	}

skiptreason:

	/* by this point you have discovered mon's identity, blind or not... */
	pline("Time stands still while you and %s lie in each other's arms...",
		noit_mon_nam(mon));
	/* Well,  IT happened ... */
	u.uconduct.celibacy++;
	u.pervertsex = 0;
	if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

	if (mon->data == &mons[PM_VIRGINITY_STEALER] && flags.female && !u.uconduct.celibacy) {
		pline("AIIEGH! %s's penis popped your cherry!", Monnam(mon));
		nomul(-10, "being deflowered", TRUE);
	}

	if (Role_if(PM_GRENADONIN) && mon->data->mcolor == CLR_BLACK) {
		u.ualign.sins++;
		u.alignlim--;
		adjalign(-50);
	}

	if (mon->data == &mons[PM_FEMME]) pline("The beautiful femme showers your body with kisses while your %s are busy stroking her very sexy butt cheeks.", makeplural(body_part(HAND)));

	if (u.homosexual == 2 && (flags.female && mon->female) && rn2(3)) goto enjoyable;
	if (u.homosexual == 2 && (!flags.female && !(mon->female)) && rn2(3)) goto enjoyable;

	if (((rn2(Race_if(PM_BOVER) ? 300 : 135) > ACURR(A_CHA) + ACURR(A_INT)) || (u.homosexual == 2 && flags.female && !(mon->female)) || (u.homosexual == 2 && !flags.female && mon->female) ) && (mon->data != &mons[PM_FEMME] || !rn2(2) ) ) /*much higher chance of negative outcome now --Amy */ {
		/* Don't bother with mspec_used here... it didn't get tired! */
		pline("%s seems to have enjoyed it more than you...",
			noit_Monnam(mon));

		if (mon->data == &mons[PM_FEMME]) pline("You are lustfully crushed underneath the full weight of the femme's very wonderful body. Being squeezed under such a beautiful woman is pure joy!");

		switch (rn2(5)) {
			case 0: You_feel("drained of energy.");
				u.uen = 0;
				u.uenmax -= rnd(StrongHalf_physical_damage ? 2 : Half_physical_damage ? 5 : 10);
			        exercise(A_CON, FALSE);
				if (u.uenmax < 0) u.uenmax = 0;
				break;
			case 1: You("are down in the dumps.");
				(void) adjattrib(A_CON, -1, TRUE, TRUE);
			        exercise(A_CON, FALSE);
				flags.botl = 1;
				break;
			case 2: Your("senses are dulled.");
				(void) adjattrib(A_WIS, -1, TRUE, TRUE);
			        exercise(A_WIS, FALSE);
				flags.botl = 1;
				break;
			case 3:
				if ((!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4)) && (u.urmaxlvlUP >= 2) ) {
				    You_feel("out of shape.");
				    losexp("overexertion", FALSE, TRUE);
				} else {
				    You("have a curious feeling...");
				}
				break;
			case 4: {
				int tmp;
				You_feel("exhausted.");
			        exercise(A_STR, FALSE);
				tmp = rn1(10, 6);
				if(Half_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;
				if(StrongHalf_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;
				losehp(tmp, "exhaustion", KILLED_BY);
				break;
			}
		}
		if ((u.homosexual == 2 && flags.female && !(mon->female)) || (u.homosexual == 2 && !flags.female && mon->female)) badeffect();
	} else {
enjoyable:
		mon->mspec_used = rnd(100); /* monster is worn out */
		You("seem to have enjoyed it more than %s...",
		    noit_mon_nam(mon));

		if (mon->data == &mons[PM_FEMME]) pline("Wow. WOW! You scream out your neverending orgasm and feel so full of lust while lying on top of the incredibly sexy femme. She must be the sexiest woman you ever saw!");

		switch (rn2(5)) {
		case 0: You_feel("raised to your full potential.");
			exercise(A_CON, TRUE);
			u.uen = (u.uenmax += rnd(5));
			break;
		case 1: You_feel("good enough to do it again.");
			(void) adjattrib(A_CON, 1, TRUE, TRUE);
			exercise(A_CON, TRUE);
			flags.botl = 1;
			break;
		case 2: You("will always remember %s...", noit_mon_nam(mon));
			(void) adjattrib(A_WIS, 1, TRUE, TRUE);
			exercise(A_WIS, TRUE);
			flags.botl = 1;
			break;
		case 3: pline("That was a very educational experience.");
			gainlevelmaybe();
			exercise(A_WIS, TRUE);
			break;
		case 4: You_feel("restored to health!");
			u.uhp = u.uhpmax;
			if (Upolyd) u.mh = u.mhmax;
			exercise(A_STR, TRUE);
			flags.botl = 1;
			break;
		}
	}

	if (Role_if(PM_PROSTITUTE) || Role_if(PM_KURWA)) {
		verbalize(rn2(2) ? "You're great! Here, this money is for you." : "Oh my god... Here, take this money, it's all I have!");
		u.ugold += rnz(100);
	} else if (u.homosexual == 2 && !rn2(2) && (flags.female && mon->female)) {
		verbalize("You're such a hot girl! I'll give you some money as a reward!");
		u.ugold += rnz(100);
	} else if (u.homosexual == 2 && !rn2(2) && (!flags.female && !(mon->female))) {
		verbalize("Dude, that was awesome! Have some money, my dear!");
		u.ugold += rnz(100);
	} else if (mon->mtame) /* don't charge */ ;
	else if (rn2(120) < ACURR(A_CHA)) {
		pline("%s demands that you pay %s, but you refuse...",
			noit_Monnam(mon),
			Blind ? (fem ? "her" : "him") : mhim(mon));
	} else if (u.umonnum == PM_LEPRECHAUN)
		pline("%s tries to take your money, but fails...",
				noit_Monnam(mon));
	else {
#ifndef GOLDOBJ
		long cost;

		if (u.ugold > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)u.ugold + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > u.ugold) cost = u.ugold;
		if (Role_if(PM_LADIESMAN) && rn2(5)) cost = 0;
		if (!cost) verbalize("It's on the house!");
		else {
		    pline("%s takes %ld %s for services rendered!",
			    noit_Monnam(mon), cost, currency(cost));
		    u.ugold -= cost;
		    mon->mgold += cost;
		    flags.botl = 1;
		}
#else
		long cost;
                long umoney = money_cnt(invent);

		if (umoney > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)umoney + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > umoney) cost = umoney;
		if (!cost) verbalize("It's on the house!");
		else { 
		    pline("%s takes %ld %s for services rendered!",
			    noit_Monnam(mon), cost, currency(cost));
                    money2mon(mon, cost);
		    flags.botl = 1;
		}
#endif
	}

	if (HardcoreAlienMode) {

		u.ugangr++;
		pline("Oh no - you had sex before your marriage. The gods are certainly angry now.");

	}

	/* "Disable Pregnancy via foocubus/seducing encounters - Let's not do this, shall we?" In Soviet Russia, people aren't being conceived by sexual intercourse. Rather, they just spawn because God decided to create them from thin air. They're also inexplicably prude, which probably is the reason why they don't want pregnancy in their video games either. I guess they won't touch Elona with a ten-foot pole... --Amy */

	if (!rn2(birthing ? 3 : 50) && !issoviet) {

	/* Yes, real-life pregnancy doesn't work like this. But I want to avoid having to make complicated functions,
	   so the player will just get an egg that immediately hatches and may be tame. --Amy */

		struct obj *uegg;

		if (flags.female) { pline("Uh-oh - you're pregnant!"); verbalize("Be a good mother, sweetheart!");
		}
		else { pline("Oh! %s is pregnant!",noit_Monnam(mon)); verbalize("Please take good care of my baby, %s!",playeraliasname);
		}

		uegg = mksobj(EGG, FALSE, FALSE, FALSE);
		if (uegg) {
			uegg->spe = (flags.female ? 1 : 0);
			uegg->quan = 1;
			uegg->owt = weight(uegg);
			if (!rn2(2)) uegg->corpsenm = mon->mnum;
			else if (Upolyd) uegg->corpsenm = u.umonnum;
			else if (urole.femalenum != NON_PM && !rn2(2)) uegg->corpsenm = urole.femalenum;
			else uegg->corpsenm = urole.malenum;
			uegg->known = uegg->dknown = 1;
			attach_egg_hatch_timeout(uegg);
			(void) start_timer(1, TIMER_OBJECT, HATCH_EGG, (void *)uegg);
			pickup_object(uegg, 1, FALSE, TRUE);
		}

		if (HardcoreAlienMode) {

			u.ugangr += 3;
			pline("Becoming pregnant before you're married is a grave sin, and the gods are really angry.");
			adjalign(-250);
			change_luck(-5);
			prayer_done();

		}

		if ((uarmc && uarmc->oartifact == ART_CATHERINE_S_SEXUALITY) || complications) {
			u.youaredead = 1;
			pline("Oh no... your heart... it's... getting... unsteady...");
			pline("BEEPBEEP BEEPBEEP BEEP BEEP BEEEEEEEEEEEEEEEEEEEEP!");
			pline("You die from a heart failure.");
			killer_format = KILLED_BY;
			killer = "complications from childbirth";
			done(DIED);
			u.youaredead = 0;
		}

	}

        boolean protect_test = !(ublindf && (ublindf->otyp == CONDOME || ublindf->otyp == SOFT_CHASTITY_BELT))
				    && !(uwep && uwep->oartifact == ART_HARMONY_VIRGIN)
				    && !(uwep && uwep->oartifact == ART_MISTY_S_MELEE_PLEASURE)
                            && !(uarm && (uarm->oartifact == ART_CHASTITY_ARMOR || uarm->oartifact == ART_LITTLE_PENIS_WANKER));

	/* maybe contract a disease? --Amy */
	if (protect_test) {
stdcontracting:
		 if (slextest(5, 25)) {
			stdmsg("syphilis");

			diseasemu(mon->data);
			increasesanity(rnz((monster_difficulty() * 5) + 1));
		}

            if (slextest(200, 1000)) {
		stdmsg("AIDS");
		u.uprops[DEAC_FIRE_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_COLD_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_SLEEP_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_DISINT_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_SHOCK_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_POISON_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_DRAIN_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_SICK_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_ACID_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_STONE_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_FEAR_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_SEE_INVIS].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_TELEPAT].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_WARNING].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_SEARCHING].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_INFRAVISION].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_INVIS].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_DISPLACED].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_STEALTH].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_JUMPING].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_FLYING].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_REGENERATION].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_FAST].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_REFLECTING].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_FREE_ACTION].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_DIMMOPATHY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_CORROSIVITY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_BURNOPATHY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_SICKOPATHY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_THE_FORCE].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_SIGHT_BONUS].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_VERSUS_CURSES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_STUN_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_CONF_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_QUAD_ATTACK].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_PSI_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_WONDERLEGS].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_GLIB_COMBAT].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_MANALEECH].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_PEACEVISION].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_CONT_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_DISCOUNT_ACTION].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_FULL_NUTRIENT].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_TECHNICALITY].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_SCENT_VIEW].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_DIMINISHED_BLEEDING].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_CONTROL_MAGIC].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_EXP_BOOST].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_ASTRAL_VISION].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_BLIND_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		u.uprops[DEAC_HALLUC_RES].intrinsic += rnz( (monster_difficulty() * 100) + 1);
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }
        

            if (slextest(50, 250)) {
		stdmsg("ataxia");
		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(1000);
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(20, 100)) {
		stdmsg("rust chancres");
                HAggravate_monster |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(40, 200)) {
		stdmsg("chills");
		make_frozen(HFrozen + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
		set_itimeout(&HeavyFrozen, HFrozen);
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(100, 500)) {
		stdmsg("blackheart plague");

		if (!flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) && !Slimed) {
		    You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY;
		    delayed_killer = "slimed by the blackheart plague";
		} else
		    pline("Yuck!");

		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(20, 100)) {
		stdmsg("swamp fever");
		make_burned(HBurned + rnd(100) + rnd((monster_difficulty() * 10) + 1), TRUE);
		set_itimeout(&HeavyBurned, HBurned);
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(20, 100)) {
		stdmsg("depression");
		make_dimmed(HDimmed + rnd(100) + rnd((monster_difficulty() * 10) + 1), TRUE);
		set_itimeout(&HeavyDimmed, HDimmed);
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(100, 500)) {
		stdmsg("gray star");
		make_blinded(Blinded + rnd(100) + rnd((monster_difficulty() * 10) + 1), TRUE);
		set_itimeout(&HeavyBlind, Blinded);
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(100, 500)) {
		stdmsg("voracious hunger");
                HHunger |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(1000, 5000)) {
		stdmsg("polymorphitis");
                HPolymorph |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(1000, 5000)) {
		stdmsg("green star");
                u.uprops[WEAKSIGHT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("an ancient foul curse");
                u.uprops[PREMDEATH].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("witbane");
                u.uprops[UNIDENTIFY].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(1000, 5000)) {
		stdmsg("brain rot");
                u.uprops[LOW_EFFECTS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(800, 4000)) {
		stdmsg("dehydration");
                u.uprops[DEHYDRATION].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(1000, 5000)) {
		stdmsg("alexithymia");
                u.uprops[HATE_TRAP_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("hydroanemia");
                u.uprops[THIRST].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("lung cancer");
                u.uprops[NONINTRINSIC_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("orientation loss");
                u.uprops[TOTTER_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("power anemia");
                u.uprops[GUSHLUSH].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("forced atheism");
                u.uprops[NON_PRAYER_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("speech stroke");
                u.uprops[UNCALLED_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("gangrene");
                u.uprops[HARD_MODE_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(4000, 20000)) {
		stdmsg("light sensitivity");
                u.uprops[DARK_MODE_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(3000, 15000)) {
		stdmsg("imperception");
                u.uprops[ANTISEARCH_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(50000, 250000)) {
		stdmsg("ether corruption");
                u.uprops[EVIL_PATCH_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("shapechanging anomaly");
                u.uprops[WEREFORM_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("slexual deprivation");
                u.uprops[LONGING_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("indecisiveness");
                u.uprops[CHOICELESSNESS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("development disorder");
                u.uprops[DEPROVEMENT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("Dudley's disease");
                u.uprops[LUCK_LOSS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(200, 1000)) {
		stdmsg("fuckedupness (the disease that has befallen Christian Grey)");
                u.uprops[SHADES_OF_GREY].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(200, 1000)) {
		stdmsg("Rodney's Black Cancer");
                u.uprops[ITEMCURSING].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("Tourette's syndrome");
                u.uprops[MOMMA_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("bad boss disease");
                u.uprops[CHANGING_DIRECTIVES].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("K-Measurer disease");
                u.uprops[FLUCTUATING_SPEED].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("Satan disease");
                u.uprops[SATAN_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("collectomania");
                u.uprops[AUTOPICKUP_ALWAYS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("narcolepsy");
                u.uprops[FAINT_ACTIVE].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(80, 400)) {
		stdmsg("diarrhea");
                u.uprops[CRAP_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("deafness");
                u.uprops[DEAFNESS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(1000, 5000)) {
		stdmsg("a tapeworm infection");
                u.uprops[WEAKNESS_PROBLEM].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(400, 2000)) {
		stdmsg("a heavy flu");
                u.uprops[STATUS_FAILURE].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(400, 2000)) {
		stdmsg("loss of will");
                u.uprops[ITEM_STEALING_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(800, 4000)) {
		stdmsg("incontinence");
                u.uprops[REBELLION_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("migraine");
                u.uprops[UNINFORMATION].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(6000, 30000)) {
		stdmsg("arrhythmia");
                u.uprops[MONSTER_SPEED_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(8000, 40000)) {
		stdmsg("optical nervitis");
                u.uprops[GRAYOUT_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("retina destruction");
                u.uprops[GRAY_CENTER_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("rupturization");
                u.uprops[DYWYPI_PROBLEM].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("mass murder syndrome");
                u.uprops[ELM_STREET_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("checkerboard disease");
                u.uprops[CHECKERBOARD_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("severe rotating worm");
                u.uprops[CLOCKWISE_SPIN_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("rotating worm");
                u.uprops[COUNTERCLOCKWISE_SPIN_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("mind lag");
                u.uprops[LAG_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(4000, 20000)) {
		stdmsg("cursed hands");
                u.uprops[BLESSCURSE_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("antechamber flickering");
                u.uprops[FLICKER_STRIP_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("exhibitionism");
                u.uprops[UNDRESSING_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 25000)) {
		stdmsg("dysentery");
                u.uprops[CUT_NUTRITION].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(8000, 40000)) {
		stdmsg("paranoia");
                u.uprops[PARANOIA_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(1000, 2000)) {
		pline("Ulch - you contracted polychromia from having unprotected intercourse with your lover! (But that is actually a fun disease which some might consider a boon. :-))");
                u.uprops[FLEECESCRIPT_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(3000, 15000)) {
		stdmsg("mana battery disease");
                u.uprops[MANA_BATTERY_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(3000, 15000)) {
		stdmsg("poison hands disease");
                u.uprops[MONSTERFINGERS_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(6000, 30000)) {
		stdmsg("monster schizophreny");
                u.uprops[EVC_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(3000, 6000)) {
		stdmsg("identity loss");
                u.uprops[FUCKED_INFO_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(5000, 10000)) {
		pline("Ulch - you contracted... you contracted... you contracted... you contracted... you contracted...");
                u.uprops[HEAP_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(4000, 20000)) {
		stdmsg("mongoloism (trisomia-13)");
                u.uprops[ANTILEVELING].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("quasar vision");
                u.uprops[QUASAR_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("apocalyptic madness");
                u.uprops[SIMEOUT_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("borderline disorder");
                u.uprops[SANITY_TREBLE_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("bomber disease");
                u.uprops[AUTOPILOT_EFFECT].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("ulcerous skin anomaly");
                u.uprops[ENMITY_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(4000, 20000)) {
		stdmsg("selective inertia");
                u.uprops[TIMERUN_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(4000, 20000)) {
		stdmsg("disconnection");
                u.uprops[DISCONNECTED_STAIRS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("screwedness");
                u.uprops[INTERFACE_SCREW].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("Alzheimer's disease");
                u.uprops[RECURRING_AMNESIA].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("intellectual poorness");
                u.uprops[SPELL_FORGETTING].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("accoustic hallucinations");
                u.uprops[SOUND_EFFECT_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("telomer cutting disease");
                u.uprops[INTRINSIC_LOSS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("tongue crippling");
                u.uprops[MUTENESS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("parkinson");
                u.uprops[ENGRAVINGBUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(400, 2000)) {
		stdmsg("jazzy hands");
                u.uprops[PROJECTILES_MISFIRE].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("shatterhands");
                u.uprops[MAGIC_DEVICE_BUG].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("dyslexia");
                u.uprops[BOOKBUG].intrinsic |= FROMOUTSIDE;
                u.uprops[CONFUSION_PROBLEM].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("hypermetabolism");
                u.uprops[FAST_METABOLISM].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("ghoulification");
                u.uprops[FOOD_IS_ROTTEN].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("autism");
                u.uprops[SKILL_DEACTIVATED].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("schizophreny");
                u.uprops[STATS_LOWERED].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("down syndrome");
                u.uprops[TRAINING_DEACTIVATED].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("locked-in syndrome");
                u.uprops[EXERCISE_DEACTIVATED].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("magic vacuum");
                u.uprops[NORETURN].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(2000, 10000)) {
		stdmsg("brittle bones");
                u.uprops[NAKEDNESS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(200, 1000)) {
		stdmsg("osteoporosis");
		set_wounded_legs(LEFT_SIDE, HWounded_legs + rnz(10000));
		set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnz(10000));
		increasesanity(rnz((monster_difficulty() * 5) + 1));

            }

            if (slextest(400, 2000)) {
		stdmsg("porphyric hemophilia");
                u.uprops[BLOOD_LOSS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (slextest(10000, 50000)) {
		stdmsg("an ancient Morgothian curse");
                u.uprops[NASTINESS_EFFECTS].intrinsic |= FROMOUTSIDE;
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

            if (!rn2(RngeSlexuality ? 15 : 75) && !flags.female ) {
		stdmsg("erective dysfunction");
		make_numbed(HNumbed + rnd(100) + rnd((monster_difficulty() * 10) + 1), TRUE);
		set_itimeout(&HeavyNumbed, HNumbed);
		increasesanity(rnz((monster_difficulty() * 5) + 1));
            }

		if (mon->data == &mons[PM_STD_CONTRACTOR] && rn2(5)) goto stdcontracting;

		if (practicantterror) {
			pline("%s thunders: 'What the hell, you're having sex in my lab??? And you're not even using contraceptives? Didn't your parents teach you anything, you can catch STDs that way! That's a fine of 5000 zorkmids, and here's also an additional punishment. At least get a fucking room next time!'", noroelaname());
			fineforpracticant(5000, 0, 0);
			badeffect();
		}

        } else {
            if (ublindf && ublindf->otyp == CONDOME) pline("Your condome kept you safe from any diseases you might otherwise have contracted.");
            if (ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) pline("Your condome kept you safe from any diseases you might otherwise have contracted.");
		if (uwep && uwep->oartifact == ART_HARMONY_VIRGIN) pline("Your harmony whip kept you safe from any diseases you might otherwise have contracted.");
		if (uwep && uwep->oartifact == ART_MISTY_S_MELEE_PLEASURE) pline("Your pleasure whip kept you safe from any diseases you might otherwise have contracted.");
            if (uarm && uarm->oartifact == ART_CHASTITY_ARMOR) pline("Your chastity armor kept you safe from any diseases you might otherwise have contracted.");
            if (uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) {
		pline("Your penis-protection armor kept you safe from any diseases you might otherwise have contracted.");
		if (flags.female) pline("Thankfully it's shaped such that it also protects vaginas. :-)");
            }

            if (!rn2(Role_if(PM_LADIESMAN) ? 100 : 10)) mon->mcan = 1; /* monster is worn out; chance is much higher now --Amy */
            if (!tele_restrict(mon)) (void) rloc(mon, FALSE);
 }
	return 1;
        
}

STATIC_OVL void
mayberem(obj, str)
register struct obj *obj;
const char *str;
{
	char qbuf[QBUFSZ];
	char buf[BUFSZ];

	if (!obj || !obj->owornmask) return;

	if (((rn2(120) < ACURR(A_CHA)) || (uarmf && uarmf->oartifact == ART_FINAL_CHALLENGE && flags.female) || (uarmf && uarmf->oartifact == ART_RARE_ASIAN_LADY)) && !Race_if(PM_BOVER) && !(uarmc && uarmc->oartifact == ART_KING_OF_PORN) ) { /*much lower chance for the player to resist --Amy*/

		sprintf(qbuf,"\"Shall I remove your %s, %s?\" [yes/no]",
			str, (!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart"));
		getlin(qbuf,buf);
		(void) lcase (buf);
		if (strcmp (buf, "yes")) return;

	} else {
		char hairbuf[BUFSZ];

		sprintf(hairbuf, "let me run my fingers through your %s",
			body_part(HAIR));
		verbalize("Take off your %s; %s.", str,
			(obj == uarm)  ? "let's get a little closer" :
			(obj == uarmc || obj == uarms) ? "it's in the way" :
			(obj == uarmf) ? "let me rub your feet" :
			(obj == uarmg) ? "they're too clumsy" :
			(obj == uarmu) ? "let me massage you" :
			/* obj == uarmh */
			hairbuf);
	}
	remove_worn_item(obj, TRUE);
}

#endif /* OVLB */

#ifdef OVL1

STATIC_OVL int
passiveum(olduasmon,mtmp,mattk)
struct permonst *olduasmon;
register struct monst *mtmp;
register struct attack *mattk;
{
	int i, tmp;
	boolean powerbiote = FALSE;

	if (Slimed && Corrosivity && !resists_acid(mtmp)) {
		int corrosivdamage = rnd(u.ulevel);
		if (StrongCorrosivity) corrosivdamage = rnd(u.ulevel * 2);

		pline("%s is covered with a corrosive substance!", Monnam(mtmp));
		if((mtmp->mhp -= corrosivdamage) <= 0) {
			pline("%s dies!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}

	}

	if (u.bodyfluideffect && !resists_acid(mtmp)) {

		pline("%s is covered with a corrosive substance!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(4) ) <= 0) {
			pline("%s dies!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}

	}

	/* from PRIME: "Very experienced Xel'Naga have stronger acid blood." --Amy */
	if (Role_if(PM_XELNAGA) && u.ulevel >= 15 && !resists_acid(mtmp)) {

		pline("%s is covered with a corrosive substance!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(4) ) <= 0) {
			pline("%s dies!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}

	}

	if (uwep && uwep->oartifact == ART_BRISTLY_STRING) {
		pline("%s is damaged by your bristly string!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(4) ) <= 0) {
			pline("%s bleeds to death!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}

	}

	if (uarms && uarms->oartifact == ART_LITTLE_THORN_ROSE) {
		pline("%s is damaged by your thorny shield!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(5) ) <= 0) {
			pline("%s bleeds to death!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
	}

	if (uwep && uwep->oartifact == ART_RHORN) {
		pline("%s is damaged by your thorns!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(u.ulevel) ) <= 0) {
			pline("%s bleeds to death!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
	}

	if (u.thornspell) {
		pline("%s is damaged by your thorns!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(5 + (u.ulevel / 6)) ) <= 0) {
			pline("%s bleeds to death!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}

	}

	if (uarmg && uarmg->oartifact == ART_NATASCHA_S_STROKING_UNITS && !(need_one(mtmp) || need_two(mtmp) || need_three(mtmp) || need_four(mtmp) )) {
		pline("%s is damaged by your thorns!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(10)) <= 0) {
			pline("%s bleeds to death!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
	}

	if (uarm && uarm->oartifact == ART_AWAY_HAMM_ARMOR && !rn2(1000)) {
		pline("Your armor suddenly bites %s!", mon_nam(mtmp));
		if((mtmp->mhp -= rnd(100)) <= 0) {
			pline("%s is instakilled!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
	}

	if (uarm && uarm->oartifact == ART_ROBE_OF_RETRIBUTION && !rn2(3)) {
		pline("%s is hurt by retribution!", Monnam(mtmp));
		int retrdamage = u.ulevel / 3;
		if (retrdamage < 1) retrdamage = 1;
		if((mtmp->mhp -= rnd(retrdamage)) <= 0) {
			pline("%s dies!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}

	}

	if (uarmf && uarmf->oartifact == ART_RHEA_S_COMBAT_PUMPS && !resists_poison(mtmp)) {
		pline("%s is poisoned by your black leather pumps!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(10) ) <= 0) {
			pline("%s dies!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
		if (!rn2(50)) {
			mtmp->mhp = 0;
			pline("The poison was deadly...");
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
	}

	if (uarmh && uarmh->oartifact == ART_TARI_FEFALAS && !resists_poison(mtmp)) {
		pline("%s is poisoned!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(5) ) <= 0) {
			pline("%s dies!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
	}

	if (uarmc && !resists_elec(mtmp) && itemhasappearance(uarmc, APP_ELECTROSTATIC_CLOAK) ) {
		if((mtmp->mhp -= rnd(4) ) <= 0) {
			pline("%s is electrocuted and dies!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
	}

	if (RngeVoltage && !resists_elec(mtmp)) {
		if((mtmp->mhp -= rnd(4) ) <= 0) {
			pline("%s is electrocuted and dies!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
	}

	/* You only get one passive attack, so if your symbiote has one, it takes predecende because it doesn't
	 * always trigger; we'll (ab)use the olduasmon structure for it --Amy */
	if (symbiotepassive()) {
		for(i = 0; ; i++) {
			if(i >= NATTK) break; /* symbiote has no passives - use the youmonst ones */

			if (haspassive(&mons[u.usymbiote.mnum]) || mons[u.usymbiote.mnum].mattk[i].aatyp == AT_RATH || mons[u.usymbiote.mnum].mattk[i].aatyp == AT_BOOM) {
				olduasmon = &mons[u.usymbiote.mnum];
				Your("%s symbiote retaliates!", mons[u.usymbiote.mnum].mname);
				u.usymbiosisfastturns++;
				if (u.usymbiosisfastturns >= 3) {
					u.usymbiosisfastturns = 0;
					use_skill(P_SYMBIOSIS, 1);
				}
				if (tech_inuse(T_POWERBIOSIS)) powerbiote = TRUE;
				goto dothepassive; /* i and olduasmon set up */
				break;
			}

		}

	}

	for(i = 0; ; i++) {
	    if(i >= NATTK) return 1;
	    if (olduasmon->mattk[i].aatyp == AT_NONE || olduasmon->mattk[i].aatyp == AT_RATH || olduasmon->mattk[i].aatyp == AT_BOOM) break;
	}

dothepassive:
	if (olduasmon->mattk[i].damn)
	    tmp = d((int)olduasmon->mattk[i].damn,
				    (int)olduasmon->mattk[i].damd);
	else if(olduasmon->mattk[i].damd)
	    tmp = d( ((int)olduasmon->mlevel / 5)+1, (int)olduasmon->mattk[i].damd); /* Players polymorphed into blue slimes or similar stuff aren't supposed to be OP. --Amy */
	else
	    tmp = 0;

	if (powerbiote && tmp > 0) tmp *= 2;

	/* These affect the enemy even if you were "killed" (rehumanized) */
	switch(olduasmon->mattk[i].adtyp) {
	    case AD_ACID:
		if (!rn2(2)) {
		    pline("%s is splashed by your acid!", Monnam(mtmp));
		    if (resists_acid(mtmp)) {
			pline("%s is not affected.", Monnam(mtmp));
			tmp = 0;
		    }
		} else tmp = 0;
		if (!rn2(30)) erode_armor(mtmp, TRUE);
		if (!rn2(6)) erode_obj(MON_WEP(mtmp), TRUE, TRUE);
		goto assess_dmg;
	    case AD_STON: /* cockatrice */
	    case AD_EDGE:
	    {
		long protector = attk_protection((int)mattk->aatyp),
		     wornitems = mtmp->misc_worn_check;

		/* wielded weapon gives same protection as gloves here */
		if (MON_WEP(mtmp) != 0) wornitems |= W_ARMG;

		if (!resists_ston(mtmp) && !rn2(4) && (protector == 0L ||
			(protector != ~0L &&
			    (wornitems & protector) != protector))) {
		    if (poly_when_stoned(mtmp->data)) {
			mon_to_stone(mtmp);
			return (1);
		    }
		    pline("%s turns to stone!", Monnam(mtmp));
		    stoned = 1;
		    xkilled(mtmp, 0);
		    if (mtmp->mhp > 0) return 1;
		    return 2;
		}
		return 1;
	    }
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
	    	if (otmp) {
	    	    (void) drain_item(otmp);
	    	    /* No message */
	    	}
	    	return (1);
	    default:
		break;
	}
	/*if (!Upolyd) return 1;*/ /* what the hell??? --Amy */

	/* These affect the enemy only if you are still a monster
	 * Amy edit: screw that :P */
	if (rn2(3)) switch(olduasmon->mattk[i].adtyp) {
	    case AD_PHYS:

		if (!Upolyd) break;

	    	if (olduasmon->mattk[i].aatyp == AT_BOOM ) {
	    	    You("explode!");
	    	    /* KMH, balance patch -- this is okay with unchanging */
	    	    if (!Race_if(PM_UNGENOMOLD)) rehumanize(); /* we don't want ungenomolds to die from being a graveler */
			else polyself(FALSE);
	    	    goto assess_dmg;
	    	}
	    	break;
	    case AD_PLYS: /* Floating eye */

		if (dmgtype(mtmp->data, AD_PLYS)) return 1;
		if (mtmp->m_lev > 1 && (rnd(mtmp->m_lev) > u.ulevel)) return 1;

		if (tmp > 127) tmp = 127;
		if (u.umonnum == PM_FLOATING_EYE) {
		    /*if (!rn2(4)) tmp = 127;*/
		    if (mtmp->mcansee && haseyes(mtmp->data) && !rn2(3) &&
				(perceives(mtmp->data) || !Invis)) {
			if (Blind)
			    pline("As a blind %s, you cannot defend yourself.", youmonst.data->mname);
		        else {
				int parlyzdur = tmp;
				if (parlyzdur > 3) {
					parlyzdur = rnd(parlyzdur);
					if (parlyzdur < 3) parlyzdur = 3;
				}
				if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
				if (parlyzdur > 127) parlyzdur = 127;

			    if (mon_reflects(mtmp, "Your gaze is reflected by %s %s."))
				return 1;
			    pline("%s is frozen by your gaze!", Monnam(mtmp));
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = parlyzdur;
			    return 3;
			}
		    }
		} else if (!rn2(3)) { /* gelatinous cube */
			int parlyzdur = tmp;
			if (parlyzdur > 3) {
				parlyzdur = rnd(parlyzdur);
				if (parlyzdur < 3) parlyzdur = 3;
			}
			if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
			if (parlyzdur > 127) parlyzdur = 127;

		    pline("%s is frozen by you.", Monnam(mtmp));
		    mtmp->mcanmove = 0;
		    mtmp->mfrozen = parlyzdur;
		    return 3;
		}
		return 1;
	    case AD_COLD: /* Brown mold or blue jelly */
		if (resists_cold(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("%s is mildly chilly.", Monnam(mtmp));
		    golemeffects(mtmp, AD_COLD, tmp);
		    tmp = 0;
		    break;
		}
		pline("%s is suddenly very cold!", Monnam(mtmp));
		if (Upolyd) {
			u.mh += tmp / 2;
			if (u.mhmax < u.mh) u.mhmax = u.mh;
			if (u.mhmax > ((youmonst.data->mlevel+1) * 8) && !rn2(25) ) /* slow down farming --Amy */
			    (void)split_mon(&youmonst, mtmp);
		}
		break;
	    case AD_STUN: /* Yellow mold */
		tmp = 0; /* fall through */
	    case AD_FUMB:
	    case AD_DROP:
	    case AD_TREM:
	    case AD_SOUN:
		if (!mtmp->mstun) {
		    mtmp->mstun = 1;
		    pline("%s %s.", Monnam(mtmp),
			  makeplural(stagger(mtmp->data, "stagger")));
		}
		break;
	    case AD_FIRE: /* Red mold */
		if (resists_fire(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("%s is mildly warm.", Monnam(mtmp));
		    golemeffects(mtmp, AD_FIRE, tmp);
		    tmp = 0;
		    break;
		}
		pline("%s is suddenly very hot!", Monnam(mtmp));
		break;
	    case AD_ELEC:
		if (resists_elec(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("%s is slightly tingled.", Monnam(mtmp));
		    golemeffects(mtmp, AD_ELEC, tmp);
		    tmp = 0;
		    break;
		}
		pline("%s is jolted with your electricity!", Monnam(mtmp));
		break;
	    case AD_LITE:
		if (is_vampire(mtmp->data)) {
			tmp *= 2; /* vampires take more damage from sunlight --Amy */
			pline("%s is irradiated!", Monnam(mtmp));
		}
		break;
	    case AD_TLPT:
	    case AD_NEXU:
	    case AD_BANI:
	    case AD_ABDC:
		if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);

		break;

	    case AD_SLEE:
		{
		int parlyzdur = tmp;
		if (parlyzdur > 3) {
			parlyzdur = rnd(parlyzdur);
			if (parlyzdur < 3) parlyzdur = 3;
		}
		if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
		if (parlyzdur > 127) parlyzdur = 127;

		if (!mtmp->msleeping && !rn2(3) && !(mtmp->m_lev > 1 && (rnd(mtmp->m_lev) > u.ulevel)) && sleep_monst(mtmp, parlyzdur, -1)) {
		    pline("%s is put to sleep.", Monnam(mtmp));
		    mtmp->mstrategy &= ~STRAT_WAITFORU;
		    slept_monst(mtmp);
		}

		}
		break;

	    case AD_SLOW:
	    case AD_WGHT:
	    case AD_INER:
		if(mtmp->mspeed != MSLOW) {
		    unsigned int oldspeed = mtmp->mspeed;

		    mon_adjust_speed(mtmp, -1, (struct obj *)0);
		    mtmp->mstrategy &= ~STRAT_WAITFORU;
		    if (mtmp->mspeed != oldspeed)
			pline("%s slows down.", Monnam(mtmp));
		}
		break;

	    case AD_LAZY:
		if(mtmp->mspeed != MSLOW) {
		    unsigned int oldspeed = mtmp->mspeed;

		    mon_adjust_speed(mtmp, -1, (struct obj *)0);
		    mtmp->mstrategy &= ~STRAT_WAITFORU;
		    if (mtmp->mspeed != oldspeed)
			pline("%s slows down.", Monnam(mtmp));
		}
		if(!rn2(10) && !(mtmp->m_lev > 1 && (rnd(mtmp->m_lev) > u.ulevel)) && mtmp->mcanmove && !(dmgtype(mtmp->data, AD_PLYS))) {
			int parlyzdur = tmp;
			if (parlyzdur > 3) {
				parlyzdur = rnd(parlyzdur);
				if (parlyzdur < 3) parlyzdur = 3;
			}
			if (parlyzdur > 1) parlyzdur = rnd(parlyzdur);
			if (parlyzdur > 127) parlyzdur = 127;

		    pline("%s is paralyzed.", Monnam(mtmp));
		    mtmp->mcanmove = 0;
		    mtmp->mfrozen = parlyzdur;
		    mtmp->mstrategy &= ~STRAT_WAITFORU;
		}
		break;

	    case AD_NUMB:
		if(!rn2(10) && mtmp->mspeed != MSLOW) {
		    unsigned int oldspeed = mtmp->mspeed;

		    mon_adjust_speed(mtmp, -1, (struct obj *)0);
		    mtmp->mstrategy &= ~STRAT_WAITFORU;
		    if (mtmp->mspeed != oldspeed)
			pline("%s is numbed.", Monnam(mtmp));
		}
		break;

	    case AD_DARK:
		if (!(Race_if(PM_PLAYER_NIBELUNG) && rn2(5))) {
			litroomlite(FALSE);
			pline("A sinister darkness fills the area!");
		}
		if (mtmp->data->mlet == S_ANGEL) tmp *= 2;
		break;

	    case AD_THIR:
	    case AD_NTHR:
		healup(tmp, 0, FALSE, FALSE);
		You_feel("healthier!");
		break;

	    case AD_RAGN:
		ragnarok(FALSE);
		if (evilfriday && u.ulevel > 1) evilragnarok(FALSE,u.ulevel);
		break;

	    case AD_AGGR:

		incr_itimeout(&HAggravate_monster, tmp);
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();
		if (!rn2(20)) {

			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {

				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		break;

	    case AD_BLEE:

		if (tmp > 0) {
			mtmp->bleedout += tmp;
			pline("%s is struck by several thorns.", Monnam(mtmp));
		}
		break;

	    case AD_TERR:

		terrainterror();
		break;

	    case AD_CONT:

		if (!rn2(30)) {
			mtmp->isegotype = 1;
			mtmp->egotype_contaminator = 1;
		}
		if (!rn2(100)) {
			mtmp->isegotype = 1;
			mtmp->egotype_weeper = 1;
		}
		if (!rn2(250)) {
			mtmp->isegotype = 1;
			mtmp->egotype_radiator = 1;
		}
		if (!rn2(250)) {
			mtmp->isegotype = 1;
			mtmp->egotype_reactor = 1;
		}

		break;

	    case AD_FRZE:
		if (!resists_cold(mtmp) && resists_fire(mtmp)) {
			tmp *= 2;
			pline("%s is suddenly ice-cold!", Monnam(mtmp));
		}
		break;
	    case AD_ICEB:
		if (!resists_cold(mtmp)) {
			tmp *= 2;
			pline("%s is suddenly shockfrosted!", Monnam(mtmp));
		}
		break;
	    case AD_MALK:
		if (!resists_elec(mtmp)) {
			tmp *= 2;
			pline("%s is jolted by high voltage!", Monnam(mtmp));
		}
		break;
	    case AD_UVUU:
		if (has_head(mtmp->data)) {
			tmp *= 2;
			if (!rn2(1000)) {
				tmp *= 100;
				pline("%s's %s is torn apart!", Monnam(mtmp), mbodypart(mtmp, HEAD));
			} else pline("%s's %s is spiked!", Monnam(mtmp), mbodypart(mtmp, HEAD));
		}
		break;
	    case AD_GRAV:
		if (!is_flyer(mtmp->data)) {
			tmp *= 2;
			pline("%s slams into the ground!", Monnam(mtmp));
		}
		break;
	    case AD_CHKH:
		if (u.ulevel > mtmp->m_lev) tmp += (u.ulevel - mtmp->m_lev);
		break;
	    case AD_CHRN:
		if ((tmp > 0) && (mtmp->mhpmax > 1)) {
			mtmp->mhpmax--;
			pline("%s feels bad!", Monnam(mtmp));
		}
		break;
	    case AD_HODS:
		tmp += mtmp->m_lev;
		break;
	    case AD_DIMN:
		tmp += u.ulevel;
		break;
	    case AD_BURN:
		if (resists_cold(mtmp) && !resists_fire(mtmp)) {
			tmp *= 2;
			pline("%s is burning!", Monnam(mtmp));
		}
		break;
	    case AD_PLAS:
		if (!resists_fire(mtmp)) {
			tmp *= 2;
			pline("%s is suddenly extremely hot!", Monnam(mtmp));
		}
		break;
	    case AD_SLUD:
		if (!resists_acid(mtmp)) {
			tmp *= 2;
			pline("%s is covered with sludge!", Monnam(mtmp));
		}
		break;
	    case AD_LAVA:
		if (resists_cold(mtmp) && !resists_fire(mtmp)) {
			tmp *= 4;
			pline("%s is scorched by hot lava!", Monnam(mtmp));
		} else if (!resists_fire(mtmp)) {
			tmp *= 2;
			pline("%s is covered with hot lava!", Monnam(mtmp));
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
		(void) maketrap(mtmp->mx, mtmp->my, WEB, 0, FALSE);
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap(FALSE);
		break;
	    case AD_TRAP:
		if (t_at(mtmp->mx, mtmp->my) == 0) (void) maketrap(mtmp->mx, mtmp->my, randomtrap(), 0, FALSE);
		else makerandomtrap(FALSE);

		break;
	    case AD_CNCL:
		if (rnd(100) > mtmp->data->mr) {
			cancelmonsterlite(mtmp);
			pline("%s is covered in sparkling lights!", Monnam(mtmp));
		}
		break;
	    case AD_ICUR:
	    case AD_NACU:
	    case AD_CURS:
		if (!rn2(10) && (rnd(100) > mtmp->data->mr)) {
			cancelmonsterlite(mtmp);
		}
		break;
	    case AD_FEAR:
		if (rnd(100) > mtmp->data->mr) {
		     monflee(mtmp, rnd(1 + tmp), FALSE, TRUE);
			pline("%s is suddenly very afraid!",Monnam(mtmp));
		}
		break;
	    case AD_SANI:
		if (!rn2(10)) {
			mtmp->mconf = 1;
			switch (rnd(4)) {

				case 1:
					pline("%s sees you chow dead bodies.", Monnam(mtmp)); break;
				case 2:
					pline("%s shudders at your terrifying %s.", Monnam(mtmp), makeplural(body_part(EYE)) ); break;
				case 3:
					pline("%s feels sick at entrails caught in your tentacles.", Monnam(mtmp)); break;
				case 4:
					pline("%s sees maggots breed in your rent %s.", Monnam(mtmp), body_part(STOMACH)); break;

			}

		}

		break;
	    case AD_INSA:
		if (rnd(100) > mtmp->data->mr) {
		     monflee(mtmp, rnd(1 + tmp), FALSE, TRUE);
			pline("%s is suddenly very afraid!",Monnam(mtmp));
		}
		if (!mtmp->mconf) {
		    pline("%s is suddenly very confused!", Monnam(mtmp));
		    mtmp->mconf = 1;
		    mtmp->mstrategy &= ~STRAT_WAITFORU;
		}
		if (!mtmp->mstun) {
		    mtmp->mstun = 1;
		    pline("%s %s.", Monnam(mtmp),
			  makeplural(stagger(mtmp->data, "stagger")));
		}
		break;
	    case AD_DREA:
		if (!mtmp->mcanmove) {
			tmp *= 4;
			pline("%s's dream is eaten!",Monnam(mtmp));
		}
		break;
	    case AD_CONF:
	    case AD_HALU:
	    case AD_DEPR:
	    case AD_SPC2:
		if (!mtmp->mconf) {
		    pline("%s is suddenly very confused!", Monnam(mtmp));
		    mtmp->mconf = 1;
		    mtmp->mstrategy &= ~STRAT_WAITFORU;
		}
		break;
	    case AD_WRAT:
	    case AD_MANA:
	    case AD_TECH:
	    case AD_MEMO:
	    case AD_TRAI:
	    	    mon_drain_en(mtmp, ((mtmp->m_lev > 0) ? (rnd(mtmp->m_lev)) : 0) + 1 + tmp);
		break;
	    case AD_DREN:
	    	if (!resists_magm(mtmp)) {
	    	    mon_drain_en(mtmp, ((mtmp->m_lev > 0) ? (rnd(mtmp->m_lev)) : 0) + 1);
	    	}	    
		break;
	    case AD_BLND:
		    if (mtmp->mcansee)
			pline("%s is blinded.", Monnam(mtmp));
		    if ((tmp += mtmp->mblinded) > 127) tmp = 127;
		    mtmp->mblinded = tmp;
		    mtmp->mcansee = 0;
		    mtmp->mstrategy &= ~STRAT_WAITFORU;
		tmp = 0;
		break;
	    case AD_PAIN:
		if (mtmp->mhp > 9) tmp += (mtmp->mhp / 10);
		pline("%s shrieks in pain!", Monnam(mtmp));
		break;
	    case AD_DRLI:
	    case AD_TIME:
	    case AD_DFOO:
	    case AD_WEEP:
	    case AD_VAMP:
		if (!resists_drli(mtmp)) {
			pline("%s suddenly seems weaker!", Monnam(mtmp));
			if (mtmp->m_lev == 0)
				tmp = mtmp->mhp;
			else mtmp->m_lev--;
			/* Automatic kill if drained past level 0 */
		}
		break;
	    case AD_VENO:
		if (resists_poison(mtmp)) {
		    pline_The("poison doesn't seem to affect %s.", mon_nam(mtmp));
		} else {
			pline("%s is badly poisoned!", Monnam(mtmp));
			if (rn2(10)) tmp += rn1(20,12);
			else {
			    pline_The("poison was deadly...");
			    tmp = mtmp->mhp;
			}
		}
		break;


	    default: /*tmp = 0;*/
		break;
	}
	else tmp = 0;

    assess_dmg:

	if((mtmp->mhp -= tmp) <= 0) {
		pline("%s dies!", Monnam(mtmp));
		xkilled(mtmp,0);
		if (mtmp->mhp > 0) return 1;
		return 2;
	}
	return 1;
}

#endif /* OVL1 */
#ifdef OVLB

#include "edog.h"
struct monst *
cloneu()
{
	register struct monst *mon;
	int mndx = monsndx(youmonst.data);

	if (monsndx(youmonst.data) >= NUMMONS) return(struct monst *)0; /* crash prevention */

	if (u.mh <= 1) return(struct monst *)0;
	if (mvitals[mndx].mvflags & G_EXTINCT) return(struct monst *)0;
	mon = makemon(youmonst.data, u.ux, u.uy, NO_MINVENT|MM_EDOG);
	if (mon) {
	mon = christen_monst(mon, playeraliasname);
	initedog(mon);
	mon->m_lev = youmonst.data->mlevel;
	mon->mhpmax = u.mhmax;
	mon->mhp = u.mh / 2;
	u.mh -= mon->mhp;
	flags.botl = 1;
	}
	return(mon);
}

STATIC_PTR int
katicleaning()
{
	if (delay) {
		delay++;
		return(1);
	} else {
		pline("Finally, you cleaned all the dog shit from the sexy Kati shoes!");
		u.katitrapocc = FALSE;
		return(0);
	}
}

STATIC_PTR int
singcleaning()
{
	if (delay) {
		delay++;
		return(1);
	} else {
		pline("Finally, you cleaned all the shit from the sexy female shoes!");
		u.singtrapocc = FALSE;
		return(0);
	}
}

STATIC_OVL boolean
slextest(numberone, numbertwo)
int numberone, numbertwo;
{
	if (RngeSlexuality || Race_if(PM_BOVER)) {
		if (!rn2(numberone)) return TRUE;
		else return FALSE;
	} else {
		if (!rn2(numbertwo)) return TRUE;
		else return FALSE;
	}
}

STATIC_OVL void
stdmsg(stdmsgse)
const char *stdmsgse;
{
	pline("Ulch - you contracted %s from having unprotected intercourse with your lover!", stdmsgse);
}

int
singclean(mtmp)
register struct monst *mtmp;
{
	boolean extraannoying = !rn2(5);

	pline("Sing announces that %s stepped into %s, and asks you to clean them.", mtmp->data->mname, extraannoying ? "cow dung" : "dog shit");
	if (yn("Do you want to clean them?") == 'y') {
			delay = (extraannoying ? -200 : -40);
			u.singtrapocc = TRUE;
			if (extraannoying) set_occupation(singcleaning, "cleaning cow dung from female shoes", 0);
			else set_occupation(singcleaning, "cleaning dog shit from female shoes", 0);
			mtmp->mpeaceful = TRUE;
			mtmp->singannoyance = FALSE;
			pline("You start cleaning the shit from %s...", Monnam(mtmp));
			return 0;

	} else {
		pline("Sing ushers all the girls to attack you relentlessly...");
		nomul(-5, "being bound by Sing", TRUE);
		mtmp->mtame = mtmp->mpeaceful = FALSE;
		mtmp->mfrenzied = TRUE;
		mtmp->singannoyance = FALSE;

	      register struct monst *mtmp2;
		for (mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {

			if (!mtmp2->mtame) {
				mtmp2->mpeaceful = 0;
				mtmp2->mfrenzied = 1;
				mtmp2->mhp = mtmp2->mhpmax;
			}
		}
	}
	return 1;
}

int
katiclean(mtmp)
register struct monst *mtmp;
{
	/* kati and sing trap effects are supposed to always tell you the exact name of the monster,
	 * even if you're blind, hallucinating or whatever, so you know whose shoes you're cleaning --Amy */

	if (!rn2(20) && !mtmp->mfrenzied) {
		pline("%s asks you to clean the dog shit from her soles. This will take a long time, but if you can do it, she'll no longer hurt you.", mtmp->data->mname);
		if (yn("Do you want to clean the sexy Kati shoes?") == 'y') {
			delay = -200;
			u.katitrapocc = TRUE;
			set_occupation(katicleaning, "cleaning the sexy Kati shoes", 0);
			mtmp->mpeaceful = TRUE;
			pline("You start cleaning the shit from the profiled girl boots...");
			return 0;
		} else {
			pline("%s is very sad that you don't want to clean her sexy Kati shoes...", mtmp->data->mname);
			mtmp->mpeaceful = FALSE;
			mtmp->mfrenzied = TRUE;
		}
	}
	return 1;
}

#endif /* OVLB */

/*mhitu.c*/

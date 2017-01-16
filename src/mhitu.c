/*	SCCS Id: @(#)mhitu.c	3.4	2003/11/26	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#include "edog.h"

STATIC_VAR NEARDATA struct obj *otmp;

STATIC_DCL void FDECL(urustm, (struct monst *, struct obj *));
# ifdef OVL1
STATIC_DCL boolean FDECL(u_slip_free, (struct monst *,struct attack *));
STATIC_DCL int FDECL(passiveum, (struct permonst *,struct monst *,struct attack *));
# endif /* OVL1 */

#ifdef OVLB
STATIC_DCL void FDECL(mayberem, (struct obj *, const char *));
#endif /* OVLB */

STATIC_DCL boolean FDECL(diseasemu, (struct permonst *));
STATIC_DCL int FDECL(hitmu, (struct monst *,struct attack *));
STATIC_DCL int FDECL(gulpmu, (struct monst *,struct attack *));
STATIC_DCL int FDECL(explmu, (struct monst *,struct attack *,BOOLEAN_P));
STATIC_DCL void FDECL(missmu,(struct monst *,int,int,struct attack *));
STATIC_DCL void FDECL(mswings,(struct monst *,struct obj *));
STATIC_DCL void FDECL(wildmiss, (struct monst *,struct attack *));

STATIC_DCL void FDECL(hurtarmor,(int));
STATIC_DCL void NDECL(witherarmor);
STATIC_DCL void FDECL(hitmsg,(struct monst *,struct attack *));

/* See comment in mhitm.c.  If we use this a lot it probably should be */
/* changed to a parameter to mhitu. */
static int dieroll;

boolean
player_shades_of_grey()
{
	if (!uarmg) return FALSE;
	if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "grey-shaded gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "sero-zatenennykh perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "kulrang-soyali qo'lqop") ) ) return TRUE;
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

			if (flags.female && (!issoviet || !rn2(5)) && !rn2(player_shades_of_grey() ? 3 : (u.ualign.type == A_LAWFUL) ? 10 : (u.ualign.type == A_NEUTRAL) ? 7 : 5) ) { 
				pline("%s rips into your breast with maniacal fervor!", Monnam(mtmp));

			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

				losehp(d(2,monsterlev), "cowardly attack on her breasts", KILLED_BY_AN);

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

			if (!(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || (uarmc && uarmc->oartifact == ART_PALEOLITHIC_ELBOW_CONTRACT) || have_unskilledstone())) {

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

			if (!rn2(player_shades_of_grey() ? 50 : (u.ualign.type == A_LAWFUL) ? 100 : (u.ualign.type == A_NEUTRAL) ? 150 : 250) && (!issoviet || !rn2(5)) && ((rn2(3) >= armproX) || ((rnd(100) > armprolimitX) && ((armproX < 4) || (rnd(armproX) < 4) ) ) ) ) {
			if (!Drain_resistance || !rn2(4)) {
			pline("%s sinks %s teeth deep into your skin and drinks your %s!", Monnam(mtmp), mhis(mtmp), body_part(BLOOD));
		      losexp("life drainage", FALSE, TRUE);
			}
			}
			break;
		case AT_KICK:
			pline("%s kicks you%c", Monnam(mtmp),
				    thick_skinned(youmonst.data) ? '.' : '!');

			if (!flags.female && (!issoviet || !rn2(5)) && !rn2(player_shades_of_grey() ? 3 : (u.ualign.type == A_LAWFUL) ? 10 : (u.ualign.type == A_NEUTRAL) ? 7 : 5) ) { 
				pline("%s's kick painfully slams into your nuts!", Monnam(mtmp));

			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

				losehp(d(2,monsterlev), "kick in the nuts", KILLED_BY_AN);

			}

			struct obj *footwear = which_armor(mtmp, W_ARMF);
			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == WEDGE_SANDALS) || mtmp->data == &mons[PM_ANIMATED_WEDGE_SANDAL]) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s sandals against your shins! It hurts!", Monnam(mtmp), mhis(mtmp) );
				losehp(d(1,monsterlev), "sandal to the shin bone", KILLED_BY_AN);
			}


			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == ATSUZOKO_BOOTS) || mtmp->data == &mons[PM_ANIMATED_ATSUZOKO_BOOT] || mtmp->data == &mons[PM_PLATEAU_GIRL]) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s plateau soles against your shins, and stomps your %s!", Monnam(mtmp), mhis(mtmp), makeplural(body_part(FOOT)) );
				losehp(d(1,monsterlev), "being kicked by an atsuzoko boot", KILLED_BY);
				make_stunned(HStun + (monsterlev * rnd(3)), FALSE);
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == HIGH_STILETTOS) || mtmp->data == &mons[PM_ANIMATED_STILETTO_SANDAL]) ) {
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

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == UNFAIR_STILETTOS) || mtmp->data == &mons[PM_ANIMATED_UNFAIR_STILETTO]) ) {
				pline("%s uses %s stilettos in a very unfair way!", Monnam(mtmp), mhis(mtmp) );
				if (!rn2(3)) pline("Your %s are scratched to the bone and your %s is squirting everywhere!", makeplural(body_part(LEG)), body_part(BLOOD) );
				else if (!rn2(2)) pline("Your sensitive sexual organs are squeezed flat and you're gasping for air!");
				else You_feel("a vicious impact as the hard heel slams on your %s with full force, and you're seeing asterisks everywhere.", body_part(HEAD) );
				badeffect();
				if (!rn2(2)) badeffect();
				if (!rn2(3)) badeffect();
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == SKY_HIGH_HEELS) || mtmp->data == &mons[PM_ANIMATED_SKY_HIGH_HEEL]) ) {
				pline("%s kicks you in the %s with %s very high heel!", Monnam(mtmp), body_part(HEAD), mhis(mtmp) );
				monsterlev = ((mtmp->m_lev) + 1);

				if (!rn2(3) && !(Role_if(PM_COURIER)) ) {
					if (!Blind) pline("Everything suddenly goes dark.");
					make_blinded(Blinded+monsterlev,FALSE);
					if (!Blind) Your(vision_clears);
				}
				if (!rn2(3) && !(Role_if(PM_COURIER)) ) make_stunned(HStun + monsterlev, TRUE);
				if (!rn2(3) && !(Role_if(PM_COURIER)) ) pushplayer();
				if (Role_if(PM_COURIER)) pline("But since you don't actually have a head, you're unharmed.");
				else losehp(monsterlev, "sky-high heel", KILLED_BY_AN);
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == SYNTHETIC_SANDALS) || mtmp->data == &mons[PM_ANIMATED_SYNTHETIC_SANDAL]) ) {

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

				switch (rnd(85)) {

					case 1: RMBLoss += monsterlev; break;
					case 2: NoDropProblem += monsterlev; break;
					case 3: DSTWProblem += monsterlev; break;
					case 4: StatusTrapProblem += monsterlev; 
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
					case 5: Superscroller += monsterlev; break;
					case 6: MenuBug += monsterlev; break;
					case 7: FreeHandLoss += monsterlev; break;
					case 8: Unidentify += monsterlev; break;
					case 9: Thirst += monsterlev; break;
					case 10: LuckLoss += monsterlev; break;
					case 11: ShadesOfGrey += monsterlev; break;
					case 12: FaintActive += monsterlev; break;
					case 13: Itemcursing += monsterlev; break;
					case 14: DifficultyIncreased += monsterlev; break;
					case 15: Deafness += monsterlev; flags.soundok = 0; break;
					case 16: CasterProblem += monsterlev; break;
					case 17: WeaknessProblem += monsterlev; break;
					case 18: RotThirteen += monsterlev; break;
					case 19: BishopGridbug += monsterlev; break;
					case 20: UninformationProblem += monsterlev; break;
					case 21: StairsProblem += monsterlev; break;
					case 22: AlignmentProblem += monsterlev; break;
					case 23: ConfusionProblem += monsterlev; break;
					case 24: SpeedBug += monsterlev; break;
					case 25: DisplayLoss += monsterlev; break;
					case 26: SpellLoss += monsterlev; break;
					case 27: YellowSpells += monsterlev; break;
					case 28: AutoDestruct += monsterlev; break;
					case 29: MemoryLoss += monsterlev; break;
					case 30: InventoryLoss += monsterlev; break;
					case 31: {
	
						if (BlackNgWalls) break;
	
						BlackNgWalls = 1000;
						(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
						break;
					}
					case 32: IntrinsicLossProblem += monsterlev; break;
					case 33: BloodLossProblem += monsterlev; break;
					case 34: BadEffectProblem += monsterlev; break;
					case 35: TrapCreationProblem += monsterlev; break;
					case 36: AutomaticVulnerabilitiy += monsterlev; break;
					case 37: TeleportingItems += monsterlev; break;
					case 38: NastinessProblem += monsterlev; break;
					case 39: CaptchaProblem += monsterlev; break;
					case 40: FarlookProblem += monsterlev; break;
					case 41: RespawnProblem += monsterlev; break;
					case 42: RecurringAmnesia += monsterlev; break;
					case 43: BigscriptEffect += monsterlev; break;
					case 44: {
						BankTrapEffect += monsterlev;
						if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
						u.bankcashamount += u.ugold;
						u.ugold = 0;
	
						break;
					}
					case 45: MapTrapEffect += monsterlev; break;
					case 46: TechTrapEffect += monsterlev; break;
					case 47: RecurringDisenchant += monsterlev; break;
					case 48: verisiertEffect += monsterlev; break;
					case 49: ChaosTerrain += monsterlev; break;
					case 50: Muteness += monsterlev; break;
					case 51: EngravingDoesntWork += monsterlev; break;
					case 52: MagicDeviceEffect += monsterlev; break;
					case 53: BookTrapEffect += monsterlev; break;
					case 54: LevelTrapEffect += monsterlev; break;
					case 55: QuizTrapEffect += monsterlev; break;
					case 56: FastMetabolismEffect += monsterlev; break;
					case 57: NoReturnEffect += monsterlev; break;
					case 58: AlwaysEgotypeMonsters += monsterlev; break;
					case 59: TimeGoesByFaster += monsterlev; break;
					case 60: FoodIsAlwaysRotten += monsterlev; break;
					case 61: AllSkillsUnskilled += monsterlev; break;
					case 62: AllStatsAreLower += monsterlev; break;
					case 63: PlayerCannotTrainSkills += monsterlev; break;
					case 64: PlayerCannotExerciseStats += monsterlev; break;
					case 65: TurnLimitation += monsterlev; break;
					case 66: WeakSight += monsterlev; break;
					case 67: RandomMessages += monsterlev; break;

					case 68: Desecration += monsterlev; break;
					case 69: StarvationEffect += monsterlev; break;
					case 70: NoDropsEffect += monsterlev; break;
					case 71: LowEffects += monsterlev; break;
					case 72: InvisibleTrapsEffect += monsterlev; break;
					case 73: GhostWorld += monsterlev; break;
					case 74: Dehydration += monsterlev; break;
					case 75: HateTrapEffect += monsterlev; break;
					case 76: TotterTrapEffect += monsterlev; break;
					case 77: Nonintrinsics += monsterlev; break;
					case 78: Dropcurses += monsterlev; break;
					case 79: Nakedness += monsterlev; break;
					case 80: Antileveling += monsterlev; break;
					case 81: ItemStealingEffect += monsterlev; break;
					case 82: Rebellions += monsterlev; break;
					case 83: CrapEffect += monsterlev; break;
					case 84: ProjectilesMisfire += monsterlev; break;
					case 85: WallTrapping += monsterlev; break;
				}

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == ROLLER_BLADE) || mtmp->data == &mons[PM_ANIMATED_ROLLER_BLADE]) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s rolls over you with %s roller blades!", Monnam(mtmp), mhis(mtmp) );
				badeffect();
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && (footwear && footwear->otyp == PET_STOMPING_PLATFORM_BOOTS) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(5);
				pline("%s smashes your %s with %s platform boots!", Monnam(mtmp), makeplural(body_part(TOE)), mhis(mtmp) );
				register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
				  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";
			    set_wounded_legs(side, HWounded_legs + monsterlev);

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && (footwear && footwear->otyp == ASS_KICKER_BOOTS) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(10);
				pline("%s kicks you in the ass!", Monnam(mtmp) );
				morehungry(monsterlev);

			}


			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && (footwear && footwear->otyp == DANCING_SHOES) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slams %s dancing shoes against your shins! You stagger...", Monnam(mtmp), mhis(mtmp) );
				make_stunned(HStun + monsterlev, FALSE);
				losehp(1, "soft dancing shoe", KILLED_BY_AN);
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && (footwear && footwear->otyp == SWEET_MOCASSINS) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s slides %s mocassins over your %ss, opening your arteries and squirting %s everywhere!", Monnam(mtmp), mhis(mtmp), body_part(HAND), body_part(BLOOD) );
				    incr_itimeout(&Glib, monsterlev);
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && (footwear && footwear->otyp == SOFT_SNEAKERS) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s hits your %s with %s sneakers!", Monnam(mtmp), body_part(HAND), mhis(mtmp) );
				    incr_itimeout(&Glib, 2); /* just enough to make you drop your weapon */
				losehp(d(1,monsterlev), "soft sneaker to the hand", KILLED_BY_AN);
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == HIPPIE_HEELS) || mtmp->data == &mons[PM_ANIMATED_PROSTITUTE_SHOE]) ) {
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
					nomul(-rnd(5), "mesmerized by a pair of sexy leather boots");

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

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == LEATHER_PEEP_TOES) || mtmp->data == &mons[PM_ANIMATED_LEATHER_PEEP_TOE] || mtmp->data == &mons[PM_NORTHERN_SHIN_SMASHER]) ) {
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
					nomul(-monsterlev, "knocked out by an asian peep-toe");
					exercise(A_DEX, FALSE);
				    }
				}
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == FEMININE_PUMPS) || mtmp->data == &mons[PM_ANIMATED_SEXY_LEATHER_PUMP] || mtmp->data == &mons[PM_ANIMATED_BEAUTIFUL_FUNNEL_HEELED_PUMP] || mtmp->data == &mons[PM_BLOODY_BEAUTIES]) ) {
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
			}
			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && footwear->otyp == COMBAT_STILETTOS) || mtmp->data == &mons[PM_ANIMATED_COMBAT_STILETTO]) ) {
				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("%s does a round-house and painfully hits you with %s combat boots!", Monnam(mtmp), mhis(mtmp) );
				losehp(d(4,monsterlev), "kung-fu attack", KILLED_BY_AN);
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && OBJ_DESCR(objects[footwear->otyp]) && ( !strcmp(OBJ_DESCR(objects[footwear->otyp]), "beautiful heels") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "krasivyye kabluki") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "chiroyli ko'chirish to'piqlarni") ) ) || mtmp->data == &mons[PM_ANIMATED_BEAUTIFUL_SANDAL] ) ) {
				if (multi >= 0) {
					pline("You come because %s's high-heeled sandals are so beautiful.", mon_nam(mtmp));
					nomul(-2, "having a sexual orgasm");
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
			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && OBJ_DESCR(objects[footwear->otyp]) && ( !strcmp(OBJ_DESCR(objects[footwear->otyp]), "irregular boots") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "neregulyarnyye sapogi") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "tartibsizlik chizilmasin") ) ) || mtmp->data == &mons[PM_ANIMATED_IRREGULAR_HEEL] || mtmp->data == &mons[PM_MANGA_GIRL] || mtmp->data == &mons[PM_MANGA_WOMAN] || mtmp->data == &mons[PM_MANGA_LADY] ) ) {

				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rnd(5);
				pline("Those weird high heels look like they should make %s's %s hurt... but they definitely hurt you, and you feel an icy chill!", mon_nam(mtmp), makeplural(mbodypart(mtmp,FOOT)) );
				make_frozen(HFrozen+monsterlev,FALSE);

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && OBJ_DESCR(objects[footwear->otyp]) && ( !strcmp(OBJ_DESCR(objects[footwear->otyp]), "wedge boots") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "klin sapogi") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "xanjar chizilmasin") ) ) || mtmp->data == &mons[PM_TOPLESS_NURSE] || mtmp->data == &mons[PM_ANN_KATHRIN_S_CUDDLY_BOOT] || mtmp->data == &mons[PM_LISA_S_CUDDLY_BOOT] ) ) {

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

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && OBJ_DESCR(objects[footwear->otyp]) && ( !strcmp(OBJ_DESCR(objects[footwear->otyp]), "hugging boots") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "obnimat'sya sapogi") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "havola etdi chizilmasin") ) ) || mtmp->data == &mons[PM_ANIMATED_HUGGING_BOOT] || mtmp->data == &mons[PM_THE_EXTRA_FLEECY_BUNDLE_HER_HUGGING_BOOT] || mtmp->data == &mons[PM_LUISA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_REBECCA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_BITCHY_LARA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_MARLEEN_S_HUGGING_BOOT] || mtmp->data == &mons[PM_VILEA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_LISELOTTE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_OVERSLEPT_GIRL] || mtmp->data == &mons[PM_AMELJE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_MELANIE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_BUNDLE_NADJA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_MARIE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_KRISTIN_S_HUGGING_BOOT] || mtmp->data == &mons[PM_ARABELLA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_SHY_ASIAN_GIRL] || mtmp->data == &mons[PM_MARIAN_S_PERSIAN_BOOT] || mtmp->data == &mons[PM_LAURA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_LITTLE_MARIE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_CHARLOTTE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_STUPID_BLONDE_GIRL] || mtmp->data == &mons[PM_FRIEDERIKE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_SOPHIA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_HEIKE_S_HUGGING_BOOT] || mtmp->data == &mons[PM_DORA_S_HUGGING_BOOT] || mtmp->data == &mons[PM_THE_HUGGING_TOPMODEL_HER_HUGGING_BOOT] || mtmp->data == &mons[PM_BUNDLE_NADJA] || mtmp->data == &mons[PM_SADISTIC_ASIAN_GIRL] || mtmp->data == &mons[PM_LOVING_ASIAN_GIRL] || mtmp->data == &mons[PM_OVERSLEPT_TROLL] ) ) {
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

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && OBJ_DESCR(objects[footwear->otyp]) && ( !strcmp(OBJ_DESCR(objects[footwear->otyp]), "winter stilettos") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "zima stilety") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "qish sandal chizilmasin") ) ) || mtmp->data == &mons[PM_ANIMATED_WINTER_STILETTO] || mtmp->data == &mons[PM_STILETTO_LOVER] || mtmp->data == &mons[PM_BOOT_GIRL] ) ) {

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

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && OBJ_DESCR(objects[footwear->otyp]) && ( !strcmp(OBJ_DESCR(objects[footwear->otyp]), "clunky heels") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "neuklyuzhiye kabluki") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "qisqa ko'chirish to'piqlarni") ) ) || mtmp->data == &mons[PM_THE_HIGH_HEEL_LOVING_ASIAN_GIRL_HER_HEELS] || mtmp->data == &mons[PM_ALMUTH_S_BLOCK_HEELED_BOOT] || mtmp->data == &mons[PM_HENRIETTA_S_THICK_BLOCK_HEELED_BOOT] || mtmp->data == &mons[PM_KRISTIN_S_SUPER_HIGH_LADY_SHOE] || mtmp->data == &mons[PM_KRISTIN_S_BLOCK_HEELED_SANDAL] || mtmp->data == &mons[PM_THICK_BEAUTY] ) ) {

				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				monsterlev *= rn2(20) ? rnd(10) : rnd(20);
				pline("Argh! Those clunky heels hurt like hell!");
				losehp(d(1,monsterlev), "big fat clunky heel", KILLED_BY_AN);

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && OBJ_DESCR(objects[footwear->otyp]) && ( !strcmp(OBJ_DESCR(objects[footwear->otyp]), "ankle boots") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "botil'ony") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "bilagi zo'r chizilmasin") ) ) || mtmp->data == &mons[PM_EMMA_S_ANKLE_BOOT] || mtmp->data == &mons[PM_BETTINA_S_COMBAT_BOOT] || mtmp->data == &mons[PM_JANINA_S_COMBAT_BOOT] || mtmp->data == &mons[PM_ARABELLA_S_HIGH_HEELED_LADY_SHOE] ) ) {

				monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

				if (!flags.female && !rn2(3) ) { 
					pline("%s kicks right into your nuts with the heel of %s ankle boot!", Monnam(mtmp), mhis(mtmp) );
					losehp(d(2,monsterlev), "ankle boot to the nuts", KILLED_BY_AN);

					if (multi >= 0 && !rn2(5)) {
						pline("You can't breathe!");            
						nomovemsg = "Finally you get yourself back together.";
						nomul(-5, "knocked out by an ankle boot");
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

			}

			if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && ((footwear && OBJ_DESCR(objects[footwear->otyp]) && ( !strcmp(OBJ_DESCR(objects[footwear->otyp]), "block-heeled boots") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "blok kablukakh sapogi") || !strcmp(OBJ_DESCR(objects[footwear->otyp]), "blok-o'tish chizilmasin") ) ) || mtmp->data == &mons[PM_ANIMATED_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_SHY_LAURA_S_LOVELY_COMBAT_BOOT] || mtmp->data == &mons[PM_LILLY_S_FLEECY_COMBAT_BOOT] || mtmp->data == &mons[PM_HANNAH_S_COMBAT_BOOT] || mtmp->data == &mons[PM_SABINE_S_ZIPPER_BOOT] || mtmp->data == &mons[PM_LARISSA_S_BLOCK_HEELED_BOOT] || mtmp->data == &mons[PM_NICOLE_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_ANTJE_S_BLOCK_HEELED_BOOT] || mtmp->data == &mons[PM_LISA_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_KRISTIN_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_RUEA_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_JUEN_S_BLOCK_HEELED_COMBAT_BOOT] || mtmp->data == &mons[PM_RUTH_S_BLOCK_HEELED_LADY_BOOT] || mtmp->data == &mons[PM_PATRICIA_S_COMBAT_BOOT] || mtmp->data == &mons[PM_DESIREE_S_COMBAT_BOOT] || mtmp->data == &mons[PM_INGE_S_COMBAT_BOOT] || mtmp->data == &mons[PM_KATRIN_S_COMBAT_BOOT] || mtmp->data == &mons[PM_BIRGIT_S_LADY_BOOT] || mtmp->data == &mons[PM_BLOCK_HEELED_GIRL] || mtmp->data == &mons[PM_POWERFUL_BLONDE_GIRL] || mtmp->data == &mons[PM_CZECH_WENCH] ) ) {

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

			}

			if (!rn2(250)) pushplayer();

			break;
		case AT_STNG:
			pline("%s stings you!", Monnam(mtmp));
			if ((!rn2(player_shades_of_grey() ? 200 : (u.ualign.type == A_LAWFUL) ? 300 : (u.ualign.type == A_NEUTRAL) ? 250 : 300)) && (!issoviet || !rn2(5)) ) {
			pline("You are bleeding out from your stinging injury!");
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;

			if (Upolyd) {u.mhmax -= rnd(monsterlev); if (u.mh > u.mhmax) u.mh = u.mhmax;}
			else {u.uhpmax -= rnd(monsterlev); if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }


			}
			break;
		case AT_BUTT:
			pline("%s butts you!", Monnam(mtmp));

			if (multi >= 0 && (!issoviet || !rn2(5)) && !rn2(player_shades_of_grey() ? 25 : (u.ualign.type == A_LAWFUL) ? 40 : (u.ualign.type == A_NEUTRAL) ? 33 : 50)) {
			    if (Free_action) {
				You_feel("a slight shaking.");            
			    } else {
				You("flinch!");
				nomovemsg = 0;	/* default: "you can move again" */
				monsterlev = ((mtmp->m_lev) + 1);
				monsterlev /= 3;
				if (monsterlev <= 0) monsterlev = 1;
				nomul(-monsterlev, "flinching");
				exercise(A_DEX, FALSE);
			    }
			}

			if (!rn2(50)) pushplayer();

			break;
		case AT_SCRA:
			pline("%s scratches you!", Monnam(mtmp));
			if ((!rn2(player_shades_of_grey() ? 75 : (u.ualign.type == A_LAWFUL) ? 100 : (u.ualign.type == A_NEUTRAL) ? 150 : 125)) && (!issoviet || !rn2(5)) ) {
			pline("One of your arteries bursts open! You suffer from %s loss!", body_part(BLOOD));
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
			if ((!rn2(player_shades_of_grey() ? 5 : (u.ualign.type == A_LAWFUL) ? 15 : (u.ualign.type == A_NEUTRAL) ? 20 : 10)) && (!issoviet || !rn2(5)) ) {
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("Your %s spins in confusion.", body_part(HEAD));
			make_confused(HConfusion + monsterlev, FALSE);
			}

			if (!rn2(200)) pushplayer();
			break;
		case AT_TRAM:
			pline("%s tramples over you!", Monnam(mtmp));
			if (!rn2(player_shades_of_grey() ? 3 : 5) && (!issoviet || !rn2(5)) ) {
			monsterlev = ((mtmp->m_lev) + 1);
				if (monsterlev <= 0) monsterlev = 1;
				pline("You can't think straight as your every muscle is aching!");
			make_stunned(HStun + monsterlev, FALSE);
			}
			if (!rn2(25)) pushplayer();
			break;
		case AT_TUCH:
			pline("%s touches you!", Monnam(mtmp));
			if (!issoviet || !rn2(5)) losehp(player_shades_of_grey() ? 2 : 1, "icy touch", KILLED_BY_AN);

			if (mtmp->data == &mons[PM_BLACK_DEATH]) { /* lose one maximum HP --Amy */

				if (Upolyd) {u.mhmax -= 1; if (u.mh > u.mhmax) u.mh = u.mhmax;}
				else {u.uhpmax -= 1; if (u.uhp > u.uhpmax) u.uhp = u.uhpmax; }

			}

			break;
		case AT_TENT:
			pline("%s tentacles suck you!",
				        s_suffix(Monnam(mtmp)));
			if (!issoviet || !rn2(5)) {
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
		if (blocker == uarms) use_skill(P_SHIELD, 1);
		u.ubodyarmorturns++;
		if (u.ubodyarmorturns >= 5) {
			u.ubodyarmorturns = 0;
			use_skill(P_BODY_ARMOR, 1);
		}

		int savechance = 0;

		if (!(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || (uarmc && uarmc->oartifact == ART_PALEOLITHIC_ELBOW_CONTRACT) || have_unskilledstone())) {
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
		if (!rn2((objects[blocker->otyp].oc_material == LIQUID) ? 125 : 1000) && blocker->spe > ((objects[blocker->otyp].oc_material == PLATINUM) ? 1 : 0) && (rnd(7) > savechance) && (!(blocker->blessed && !rnl(6))) && (!rn2(3) || !(objects[blocker->otyp].oc_material == GOLD) ) && !(objects[blocker->otyp].oc_material == SECREE || objects[blocker->otyp].oc_material == ARCANIUM) && (!(blocker->oartifact) || !rn2(4))) {
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

	} else impossible("%s attacks you without knowing your location?",
		Monnam(mtmp));
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
							Strcpy(blast,
						      " in a shower of sparks");
							break;
						case AD_COLD:
							Strcpy(blast,
							" in a blast of frost");
							break;
					}
				} else
					Strcpy(blast, " with a squelch");
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

	/* you can attack land-based monsters while underwater, so why should YOU be protected from THEIR attacks??? --Amy */
	if(!ranged) nomul(0, 0);
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
		/* Orcs like to steal and eat horses and the like */
		if (!rn2(is_orc(mtmp->data) ? 2 : 4) &&
				distu(mtmp->mx, mtmp->my) <= 2) {
			/* Attack your steed instead */
			i = mattackm(mtmp, u.usteed);
			if ((i & MM_AGR_DIED))
				return (1);
			if (i & MM_DEF_DIED || u.umoved)
				return (0);
			/* Let your steed retaliate */
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
		    if (obj && is_metallic(obj)) {
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

			if (obj ||
			      (youmonst.data->mlet == S_EEL && is_pool(u.ux, u.uy))) {
			    int save_spe = 0; /* suppress warning */
			    if (obj) {
				save_spe = obj->spe;
				if (obj->otyp == EGG) obj->spe = 0;
			    }
			    if (youmonst.data->mlet == S_EEL)
		pline("Wait, %s!  There's a hidden %s named %s there!",
				m_monnam(mtmp), !missingnoprotect ? youmonst.data->mname : "creature", plname);
			    else
	     pline("Wait, %s!  There's a %s named %s hiding under %s!",
				m_monnam(mtmp), !missingnoprotect ? youmonst.data->mname : "creature", plname,
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
			   m_monnam(mtmp), !missingnoprotect ? youmonst.data->mname : "creature", plname);
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
			plname);
	    /*if (multi < 0) {*/	/* this should always be the case but is not, due to mimicry spell --Amy */
		char buf[BUFSZ];
		Sprintf(buf, "You appear to be %s again.",
			(Upolyd && !missingnoprotect) ? (const char *) an(youmonst.data->mname) :
			    (const char *) "yourself");
		unmul(buf);	/* immediately stop mimicking */
		youmonst.m_ap_type = M_AP_NOTHING;
		youmonst.mappearance = 0;
	    /*}*/
	    return 0;
	}

/*	Work out the armor class differential	*/
	tmp = AC_VALUE(u.uac) + 10;		/* tmp ~= 0 - 20 */
	tmp += mtmp->m_lev;
	if (mtmp->egotype_hitter) tmp += 10;
	if (mtmp->egotype_piercer) tmp += 25;
	if(multi < 0) tmp += 4;
        if((Invis && !perceives(mdat)) || !mtmp->mcansee) tmp -= 2;
	if(mtmp->mtrapped) tmp -= 2;
	if(tmp <= 0) tmp = 1;
	if (mtmp->data == &mons[PM_IVORY_COAST_STAR]) tmp += 30; /* this monster is aiming abnormally well */
	if (mtmp->data == &mons[PM_HAND_OF_GOD]) tmp += 100; /* God personally is guiding this one's blows */

	/* farting monsters are simply more likely to hit you, except if you bash their sexy butts --Amy */
	if (mtmp->data->msound == MS_FART_LOUD && !mtmp->butthurt) tmp += 5;
	if (mtmp->data->msound == MS_FART_NORMAL && !mtmp->butthurt) tmp += 10;
	if (mtmp->data->msound == MS_FART_QUIET && !mtmp->butthurt) tmp += 15;
	if (mtmp->egotype_farter) tmp += 15;
	if (mtmp->fartbonus) tmp += mtmp->fartbonus;
	if (mtmp->crapbonus) tmp += mtmp->crapbonus;

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
	if(!mtmp->cham && (is_demon(mdat) || mtmp->egotype_gator) && !range2
	   && mtmp->data != &mons[PM_BALROG]
	   && mtmp->data != &mons[PM_SUCCUBUS]
	   && mtmp->data != &mons[PM_INCUBUS]
 	   && mtmp->data != &mons[PM_NEWS_DAEMON]
 	   && mtmp->data != &mons[PM_PRINTER_DAEMON])
	    if(!mtmp->mcan && !rn2(23))	{msummon(mtmp); pline("%s opens a gate!",Monnam(mtmp) );}


/*	Special arbitrator handling code --Amy */

	if (mtmp->data == &mons[PM_ARBITRATOR] && !rn2(25) ) {

		wake_nearby();
		pline("%s blows a whistle!",Monnam(mtmp) ); 
		/* even if the player stole it; arbitrators can somehow whistle anyway */
	}

/*	Special lycanthrope handling code */
	if(!mtmp->cham && is_were(mdat) && !range2) {
	    if(is_human(mdat)) {
		if(!rn2(15 - (night() * 5)) && !mtmp->mcan) new_were(mtmp);
	    } else if(!rn2(30) && !mtmp->mcan) new_were(mtmp);
	    mdat = mtmp->data;

	    if(!rn2(10) && !mtmp->mcan) {
	    	int numseen, numhelp;
		char buf[BUFSZ], genericwere[BUFSZ];

		Strcpy(genericwere, "creature");
		numhelp = were_summon(mdat, FALSE, &numseen, genericwere);
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
			    		Sprintf(buf, "%s appears",
							an(genericwere));
			    	else
			    		Sprintf(buf, "%s appear",
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
			if((!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
					!touch_petrifies(youmonst.data))) || (mtmp->egotype_hugger && !rn2(20) && dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= (BOLT_LIM * BOLT_LIM) ) ) {
			    if (foundyou) {
				if ((tmp > (j = rnd(20+i))) || (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmf->otyp]), "korean sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "koreyskiye sandalii") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "janubiy koreyaning kavushlari") ) && !rn2(3) ) ) {
				    if (mattk->aatyp != AT_KICK ||
					    !thick_skinned(youmonst.data))
					sum[i] = hitmu(mtmp, mattk);
				} else
				    missmu(mtmp, tmp, j, mattk);
			    } else wildmiss(mtmp, mattk);
			}

			if(lined_up(mtmp) && dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM && !rn2(5) && mtmp->egotype_beamer && (tmp > (rnd(20+i))) ){  
				if (foundyou) sum[i] = hitmu(mtmp, mattk);  
				else wildmiss(mtmp, mattk);  
			}  

			if (!range2 && mtmp->egotype_engulfer) {
			    if(foundyou) {
				if((u.uswallow || tmp > (j = rnd(20+i))) && (issoviet || rn2(10)) ) { /* 10% chance to miss --Amy */
				    /* Force swallowing monster to be
				     * displayed even when player is
				     * moving away */
				    flush_screen(1);
				    sum[i] = gulpmu(mtmp, mattk);
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
		                if((!range2 && i>=2 && sum[i-1] && sum[i-2]) || mtmp == u.ustuck || (!rn2(20) && dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= (BOLT_LIM * BOLT_LIM) ) ) {
				if ( (tmp > (rnd(20+i))) || (tmp > (rnd(20+i))) ) sum[i]= hitmu(mtmp, mattk);
				}
		/* This has the side effect of AT_HUGS hitting from far away. I decided to declare this "bug" a feature. */
			break;
		case AT_BEAM:  /* ranged non-contact attack by Chris - only go off 40% of the time for balance reasons --Amy */
			if(lined_up(mtmp) && dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM && (tmp > (rnd(20+i))) && (rnd(5) > 3) ) {  
				if (foundyou) sum[i] = hitmu(mtmp, mattk);  
				else wildmiss(mtmp, mattk);  
			}  
			break;
		case AT_GAZE:	/* can affect you either ranged or not */
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
			if (!range2) {
			    if(foundyou) {
				if((u.uswallow || tmp > (j = rnd(20+i))) && (issoviet || rn2(10)) ) { /* 10% chance to miss --Amy */
				    /* Force swallowing monster to be
				     * displayed even when player is
				     * moving away */
				    flush_screen(1);
				    sum[i] = gulpmu(mtmp, mattk);
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
			if (/*range2 &&*/ !blue_on_blue(mtmp) && rn2(25) && (mattk->adtyp == AD_RBRE || (mattk->adtyp >= AD_MAGM && mattk->adtyp <= AD_SPC2) ) )
			    sum[i] = breamu(mtmp, mattk);
			else if ( (rnd(5) > 3) && ( (tmp > (rnd(20+i))) || (tmp > (rnd(20+i))) ) && lined_up(mtmp) && dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM)
			{  
				if (foundyou) sum[i] = hitmu(mtmp, mattk);  
				else wildmiss(mtmp, mattk);  
			}  

			/* Note: breamu takes care of displacement */
			break;
		case AT_SPIT:
			if (/*range2 &&*/ !blue_on_blue(mtmp) && rn2(25) && (mattk->adtyp == AD_ACID || mattk->adtyp == AD_BLND || mattk->adtyp == AD_DRLI || mattk->adtyp == AD_TCKL || mattk->adtyp == AD_NAST) )
			    sum[i] = spitmu(mtmp, mattk);
			/* Note: spitmu takes care of displacement */

			else if ( (rnd(5) > 3) && ( (tmp > (rnd(20+i))) || (tmp > (rnd(20+i))) ) && lined_up(mtmp) && dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM)
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
			if (!u.sterilized && !range2 && (!rn2(5)) ) { /* greatly reduced chance --Amy */

			    pline("%s multiplies!",Monnam(mtmp) );
			    clone_mon(mtmp, 0, 0);
				}
			break;
		case AT_WEAP:

			if (mtmp->egotype_hugger && !rn2(20) && dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= (BOLT_LIM * BOLT_LIM) && (tmp > (rnd(20+i))) )
			hitmu(mtmp, mattk);

			if(lined_up(mtmp) && dist2(mtmp->mx,mtmp->my,mtmp->mux,mtmp->muy) <= BOLT_LIM*BOLT_LIM && !rn2(5) && mtmp->egotype_beamer && (tmp > (rnd(20+i))) ){  
				if (foundyou) sum[i] = hitmu(mtmp, mattk);  
				else wildmiss(mtmp, mattk);  
			}  

swingweapon:

			if(range2 || !rn2(4) ) {
#ifdef REINCARNATION
				if (!Is_rogue_level(&u.uz) || !rn2(3) )
#endif
				    if (!blue_on_blue(mtmp))
					thrwmu(mtmp);
			} else {
			    int hittmp = 0;

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
					pline("The massive wedge heel thunders painfully on your %s!", body_part(HEAD));
					losehp(rnd(4),"a wedged little-girl sandal",KILLED_BY);
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == SOFT_GIRL_SNEAKER && (tmp > rnd(20+i)) ) {
					pline("The soft leather sneaker actually feels quite soothing.");

					if (Upolyd) u.mh++; /* heal one hit point */
					else u.uhp++; /* heal one hit point */

					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS && (tmp > rnd(20+i)) ) {
					pline("The unyielding plateau boot bonks your %s!", body_part(HEAD));
					losehp(rnd(10),"a sturdy plateau boot for girls",KILLED_BY);
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == BLOCK_HEELED_COMBAT_BOOT && (tmp > rnd(20+i)) ) {

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
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == HUGGING_BOOT && (tmp > rnd(20+i)) ) {
					pline("Uff! Your %s got hit hard!", body_part(HEAD));
					losehp(rnd(12),"a hugging boot",KILLED_BY);
						if (Upolyd) u.mhmax--; /* lose one hit point */
						else u.uhpmax--; /* lose one hit point */
						if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
						if (u.mh > u.mhmax) u.mh = u.mhmax;
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == WOODEN_GETA && (tmp > rnd(20+i)) ) {
					pline("Argh! The unyielding piece of wood painfully lands on your %s!", body_part(HEAD));
					losehp(rnd(15),"a wooden Japanese sandal",KILLED_BY);

					if (multi >= 0 && !rn2(2)) {
					    if (Free_action) {
						pline("You struggle to stay on your %s.", makeplural(body_part(FOOT)));
					    } else {
						pline("You're knocked out and helplessly drop to the floor.");
						nomovemsg = 0;	/* default: "you can move again" */
						nomul(-rnd(5), "knocked out by a wooden Japanese sandal");
						exercise(A_DEX, FALSE);
						    }
						}
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == LACQUERED_DANCING_SHOE && (tmp > rnd(20+i)) ) {

					if (Role_if(PM_COURIER)) pline("The lacquered dancing shoe harmlessly scratches you.");
					else {pline("The lacquered dancing shoe scratches your %s!", body_part(HEAD));

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
							} else {
							    /* explicitly chose not to die;
							       arbitrarily boost intelligence */
							    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
							    You_feel("like a scarecrow.");
							    break;
							}
						    }

						    if (lifesaved)
							pline("Unfortunately your brain is still gone.");
						    else
							Your("last thought fades away.");
						    killer = "brainlessness";
						    killer_format = KILLED_BY;
						    done(DIED);
						    lifesaved++;
						}
					    }
					}
					/* adjattrib gives dunce cap message when appropriate */
					if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
					else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
					if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
					if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
					exercise(A_WIS, FALSE);

						}
					}

					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == HIGH_HEELED_SANDAL && (tmp > rnd(20+i)) ) {
					pline("Your %s is hit painfully by the high heel!", body_part(HEAD));
					losehp(rnd(12),"a high-heeled sandal",KILLED_BY);
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == SEXY_LEATHER_PUMP && (tmp > rnd(20+i)) ) {
					pline("Klock! The heel slams on your %s, producing a beautiful sound.", body_part(HEAD));
					losehp(rnd(20),"a sexy leather pump",KILLED_BY);
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == INKA_BOOT && (tmp > rnd(20+i)) ) {
					pline("You have an orgasm because the soft inka leather is so lovely.");
					badeffect();
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == SOFT_LADY_SHOE && (tmp > rnd(20+i)) ) {
					pline("Ouch! Despite being incredibly soft, the lady shoe hit your optical nerve!");
					make_blinded(Blinded + rnd(30),FALSE);
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == STEEL_CAPPED_SANDAL && (tmp > rnd(20+i)) ) {
					pline("AAAAAHHHHH! Your %s screams as it gets hit by the massive, unyielding metal heel!", body_part(HEAD));
					make_stunned(HStun + rnd(100), FALSE);
						if (!rn2(10)) {
							pline("You're knocked out and helplessly drop to the floor.");
							nomovemsg = 0;	/* default: "you can move again" */
							if (Free_action) nomul(-rnd(4), "knocked out by a steel-capped sandal");
							else nomul(-rnd(20), "knocked out by a steel-capped sandal");
						}
					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == DOGSHIT_BOOT && (tmp > rnd(20+i)) ) {
					pline("Ulch! You're immersed with dog shit!");

					    register struct obj *objX, *objX2;
					    for (objX = invent; objX; objX = objX2) {
					      objX2 = objX->nobj;
						if (!rn2(5)) rust_dmg(objX, xname(objX), 3, TRUE, &youmonst);
						if (objX && !rn2(100)) wither_dmg(objX, xname(objX), 3, TRUE, &youmonst);

					    }

					}
					if ( (!rn2(3) || player_shades_of_grey() ) && (!issoviet || !rn2(5)) && otmp->otyp == SPIKED_BATTLE_BOOT && (tmp > rnd(20+i)) ) {
					pline("Ouch! The spiked boot soles bore themselves into your skin!");
					losehp(rnd(10),"a spiked battle boot",KILLED_BY);
				    if (!rn2(6))
					poisoned("spikes", A_STR, "poisoned boot spike", 8);
					}

/* The gnome swings her spiked battle boot! Ouch! The spiked boot soles bore themselves into your skin! The spikes were poisoned! The poison was deadly... Do you want your possessions identified? */

				}
				if((tmp > (j = dieroll = rnd(20+i))) || (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmf->otyp]), "korean sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "koreyskiye sandalii") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "janubiy koreyaning kavushlari") ) && !rn2(3) ) ) {
				    sum[i] = hitmu(mtmp, mattk);
					if (!rn2(75)) pushplayer();
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
			if (!rn2(iswarper ? 2 : 4)) /* yeah they need to be toned down a lot */{

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

	if (mtmp->egotype_arcane && !rn2(iswarper ? 2 : 4) && !range2) {

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

	if (mtmp->egotype_clerical && !rn2(iswarper ? 2 : 4) && !range2) {

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

	if (mtmp->egotype_thief ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SITM;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_disenchant ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ENCH;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_rust ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RUST;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_corrosion ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CORR;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_decay ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DCAY;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_wither ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_WTHR;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_grab ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STCK;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_faker ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FAKE;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_slows ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SLOW;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_vampire ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DRLI;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_teleportyou ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TLPT;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_wrap ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_WRAP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_disease ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DISE;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_slime ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SLIM;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_poisoner ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_POIS;
		a->damn = 1;
		a->damd = 1;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_elementalist ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_AXUS;
		a->damn = 1;
		a->damd = 1;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_acidspiller ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ACID;
		a->damn = 1;
		a->damd = 1;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_engrave ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NGRA;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_dark ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DARK;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_sounder ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SOUN;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_timer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_TIME;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_thirster ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_THIR;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_bomber ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_EXPL;
		a->adtyp = AD_PHYS;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev * 5));

		if(!range2) sum[i] = explmu(mtmp, a, foundyou);

	}

	if (mtmp->egotype_nexus ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NEXU;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_gravitator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_GRAV;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_inert ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_INER;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_antimage ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_MANA;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_unskillor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SKIL;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_venomizer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_VENO;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_dreameater ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DREA;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_nastinator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_NAST;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_baddie ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BADE;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_sludgepuddle ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SLUD;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_vulnerator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_VULN;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_marysue ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_FUMB;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_plasmon ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_PLAS;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev * 3 / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_lasher ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_LASH;
		a->adtyp = AD_MALK;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev / 2));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_breather ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_BREA;
		a->adtyp = AD_RBRE;
		a->damn = (1 + (mtmp->m_lev / 4));
		a->damd = (1 + (mtmp->m_lev / 4));

		if (range2 && !blue_on_blue(mtmp))
		    sum[i] = breamu(mtmp, a);
		/* Note: breamu takes care of displacement */

	}

	if (mtmp->egotype_luck ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_LUCK;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_seducer ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SSEX;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_cullen ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_VAMP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_webber ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_WEBS;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_itemporter ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_STTP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_schizo ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DEPR;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_watersplasher ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = level.flags.lethe ? AD_LETH : AD_WET;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_cancellator ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CNCL;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_banisher ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_BANI;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_shredder ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SHRD;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_abductor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_ABDC;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_incrementor ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CHKH;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_mirrorimage ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_HODS;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_curser ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_CURS;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_horner ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = rn2(1000) ? AD_CHRN : AD_UVUU;
		a->damn = 2;
		a->damd = (1 + (mtmp->m_lev));

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_push ) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_DISP;
		a->damn = 0;
		a->damd = 0;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_randomizer) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_RBRE;
		a->damn = 2;
		a->damd = 1 + (mtmp->m_lev / 2);

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_blaster) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TENT;
		a->adtyp = AD_DRIN;
		a->damn = 1;
		a->damd = 1;

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_psychic) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_TUCH;
		a->adtyp = AD_SPC2;
		a->damn = 2;
		a->damd = 1 + (mtmp->m_lev / 2);

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_abomination) {

		mdat2 = &mons[PM_CAST_DUMMY];
		a = &mdat2->mattk[2];
		a->aatyp = AT_GAZE;
		a->adtyp = AD_SPC2;
		a->damn = 2;
		a->damd = 1 + (mtmp->m_lev / 2);

		if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
				!touch_petrifies(youmonst.data))) {
		    if (foundyou) {
			if(tmp > (j = rnd(20+i))) {
			    if (a->aatyp != AT_KICK ||
				    !thick_skinned(youmonst.data))
				sum[i] = hitmu(mtmp, a);
			} else
			    missmu(mtmp, tmp, j, a);
		    } else wildmiss(mtmp, a);
		}

	}

	if (mtmp->egotype_multiplicator) {

		if (!range2 && (!rn2(5)) ) {
		    pline("%s multiplies!",Monnam(mtmp) );
		    clone_mon(mtmp, 0, 0);
		}

	}

	return(0);
}

#endif /* OVL0 */
#ifdef OVLB

/*
 * helper function for some compilers that have trouble with hitmu
 */

STATIC_OVL void
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

STATIC_OVL void
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
	if (Sick_resistance || !rn2(10) ) { /* small chance to not get infected even if not resistant --Amy */
		You_feel("a slight illness.");
		return FALSE;
	} else {
		make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			mdat->mname, TRUE, SICK_NONVOMITABLE);
		return TRUE;
	}
}

STATIC_OVL boolean
digeasemu(mtmp)
struct permonst *mtmp;
{
	if (Sick_resistance || !rn2(10) ) {
		You_feel("a slight illness.");
		return FALSE;
	} else {
		make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			mtmp->mname, TRUE, SICK_NONVOMITABLE);
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

	    if (obj->greased && !rn2(2)) {
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
	} else if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "slippery gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "skol'zkiye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "sirg'anchiq qo'lqop") ) && rn2(10) ) {
                pline("%s %s you, but your slippery gloves allow you to slip free!",
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
	if (MCReduction && mon == &youmonst) armpro -= (1 + (MCReduction / 5000));
	if (u.magicshield) armpro += 1;
	if (Race_if(PM_INKA)) armpro++;
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
	register boolean not_affected = defends((int)mattk->adtyp, uwep);
	register int uncancelled, ptmp;
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
	uchar atttyp;

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
		    else if (is_pool(mtmp->mx, mtmp->my) && !Underwater)
			what = "the water";
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
	if (mtmp->mfrenzied) dmg *= 2;
/*	Next a cancellation factor	*/

/*	Use uncancelled when the cancellation factor takes into account certain
 *	armor's special magic protection.  Otherwise just use !mtmp->mcan.
 */
	armpro = magic_negation(&youmonst);

	if (!(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || (uarmc && uarmc->oartifact == ART_PALEOLITHIC_ELBOW_CONTRACT) || have_unskilledstone())) {

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

	if (atttyp == AD_RBRE) {
		while (atttyp == AD_ENDS ||atttyp == AD_RBRE || atttyp == AD_SPC2 || atttyp == AD_WERE) {
			atttyp = randattack(); }
		/*randattack = 1;*/
	}

	switch(atttyp) {
	    case AD_PHYS:
		if (mattk->aatyp == AT_HUGS && !sticks(youmonst.data)) {
		    if(!u.ustuck && rn2(2)) {
			if (u_slip_free(mtmp, mattk)) {
			    dmg = 0;
			} else {
			    setustuck(mtmp);
			    pline("%s grabs you!", Monnam(mtmp));
			}
		    } else if(u.ustuck == mtmp) {
			exercise(A_STR, FALSE);
			if (mtmp->data == &mons[PM_ROPE_GOLEM] && Breathless) {
			    You("are being strangled.");
			    dmg = (dmg+1) / 2;
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
			    if (!Stoned && !Stone_resistance) goto do_stone;
			}

			/* MRKR: If hit with a burning torch,     */
			/*       then do an extra point of damage */
			/*       but save the message till after  */
			/*       the hitmsg()                     */

			if (otmp->otyp == TORCH && otmp->lamplit &&
			    !Fire_resistance) {
			  burnmsg = TRUE;
			  dmg++;
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
			    if (u.umonnum == PM_SHADE && otmp->otyp != SILVER_ARROW)
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
				if (u.umonnum != PM_SHADE)
				    dmg++;
			    }
#endif
			} else dmg += dmgval(otmp, &youmonst);

			if (otmp && otmp->otyp == COLLUSION_KNIFE) {

				pline("Collusion!");
				litroomlite(FALSE);

			}

			if (objects[otmp->otyp].oc_material == SILVER &&
				hates_silver(youmonst.data)) {
			    pline("The silver sears your flesh!");
			}
			if (objects[otmp->otyp].oc_material == VIVA && hates_viva(youmonst.data)) {
			    pline("The irradiation severely hurts you!");
			}
			if (objects[otmp->otyp].oc_material == INKA) { /* you are always vulnerable to inka, no matter what --Amy */
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
				    pline("%s plunges the stake into your heart.",
					    Monnam(mtmp));
				    killer = "a wooden stake in the heart.";
				    killer_format = KILLED_BY_AN;
				    u.ugrave_arise = NON_PM; /* No corpse */
				    done(DIED);
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
			    if (!rn2(nopoison)) {
				otmp->opoisoned = FALSE;
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

			  if (!rn2(2) && burnarmor(&youmonst)) {
			    dmg++;

			    /* Torch flame is not hot enough to guarantee */
			    /* burning away slime */

			    if (!rn2(4)) burn_away_slime();
			    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
			      (void)destroy_item(POTION_CLASS, AD_FIRE);
			    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
			      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
			    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
			      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
			  }
			  burn_faster(otmp, 1);
			}

			if (!dmg) break;
			if (u.mh > 1 && u.mh > ((u.uac>0) ? dmg : dmg+u.uac) &&
				   objects[otmp->otyp].oc_material == IRON &&
					(u.umonnum==PM_BLACK_PUDDING || u.umonnum==PM_BLACKSTEEL_PUDDING || u.umonnum==PM_BLOOD_PUDDING
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
		if (rn2(3)) break;
                if (!diseasemu(mdat) || Invulnerable || (Stoned_chiller && Stoned)) dmg = 0;
		break;
	    case AD_NGRA:
		hitmsg(mtmp, mattk);

		      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		/* This attack can remove any Elbereth engraving, even burned ones. --Amy */
			pline("The engraving underneath you vanishes!");
		    del_engr(ep);
		    ep = (struct engr *)0;
		}

		break;
	    case AD_GLIB:
		hitmsg(mtmp, mattk);

		/* hurt the player's hands --Amy */
		pline("Your hands got hit hard!");
		incr_itimeout(&Glib, dmg);

		break;
	    case AD_DARK:
		hitmsg(mtmp, mattk);

		/* create darkness around the player --Amy */
		pline("That felt evil and sinister!");
			litroomlite(FALSE);
		break;

	    case AD_UVUU:{
		int wdmg = (int)(dmg/6) + 1;
		hitmsg(mtmp, mattk);
		Sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		poisoned(buf, A_CON, mdat->mname, 60);
		if(Poison_resistance) wdmg -= ACURR(A_CON)/2;
		if(wdmg > 0){
		
			while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
				wdmg--;
				(void) adjattrib(A_WIS, -1, TRUE);
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

	    case AD_MALK:
		hitmsg(mtmp, mattk);
		if (uncancelled) {

		    setustuck(mtmp);
		    pline("%s grabs you!", Monnam(mtmp));
		    display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		    You("get zapped!");
		    if (Shock_resistance && rn2(20)) {
			pline_The("zap doesn't shock you!");
			dmg = 0;
		    }
		    if (!rn2(issoviet ? 2 : 10)) /* high voltage - stronger than ordinary shock attack --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    if (!rn2(issoviet ? 2 : 10))
			destroy_item(RING_CLASS, AD_ELEC);
		}
		break;

	    case AD_LAVA:
		hitmsg(mtmp, mattk);
		    pline("You're %s!", on_fire(youmonst.data, mattk));
		    if (youmonst.data == &mons[PM_STRAW_GOLEM] ||
		        youmonst.data == &mons[PM_PAPER_GOLEM]) {
			    You("roast!");
			    /* KMH -- this is okay with unchanging */
			    rehumanize();
			    break;
		    } else if (Fire_resistance && rn2(20)) {
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
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 4 : 20)) /* hotter than ordinary fire attack, so more likely to burn items --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 4 : 20))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 30))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		    if (rn2(3)) hurtarmor(AD_LAVA);
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
		    } else if (Fire_resistance && rn2(20)) {
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
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		} /*else dmg = 0;*/
		break;
	    case AD_COLD:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("You're covered in frost!");
		    if (Cold_resistance && rn2(20)) {
			pline_The("frost doesn't seem cold!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);
		} /*else dmg = 0;*/
		break;
	    case AD_ELEC:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    You("get zapped!");
		    if (Shock_resistance && rn2(20)) {
			pline_The("zap doesn't shock you!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		} /*else dmg = 0;*/
		break;
	    case AD_SLEE:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(5)) {
		    if (Sleep_resistance && rn2(20)) break;
		    fall_asleep(-rnd(10), TRUE);
		    if (Blind) You("are put to sleep!");
		    else You("are put to sleep by %s!", mon_nam(mtmp));
		}
		break;
	    case AD_BLND:
		hitmsg(mtmp, mattk);
		if (can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0) && !rn2(3) ) {
		    if (!Blind) pline("%s blinds you!", Monnam(mtmp));
		    make_blinded(Blinded+(long)dmg,FALSE);
		    if (!Blind) Your(vision_clears);
		}
		/*dmg = 0;*/ /* why??? --Amy */
		break;

	    case AD_DREA:
		hitmsg(mtmp, mattk);

		if (multi < 0) {
			dmg *= 4;
			pline("Your dream is eaten!");
		}

		break;

	    case AD_BADE:
		hitmsg(mtmp, mattk);

		badeffect();

		break;

	    case AD_FUMB:
		hitmsg(mtmp, mattk);

		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(10 * (dmg + 1) );

		break;

	    case AD_VULN:
		hitmsg(mtmp, mattk);

		 switch (rnd(123)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				u.uprops[DEAC_FIRE_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having fire resistance!");
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				u.uprops[DEAC_COLD_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having cold resistance!");
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				u.uprops[DEAC_SLEEP_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having sleep resistance!");
				break;
			case 16:
			case 17:
				u.uprops[DEAC_DISINT_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having disintegration resistance!");
				break;
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
				u.uprops[DEAC_SHOCK_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having shock resistance!");
				break;
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				u.uprops[DEAC_POISON_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having poison resistance!");
				break;
			case 28:
			case 29:
			case 30:
				u.uprops[DEAC_DRAIN_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having drain resistance!");
				break;
			case 31:
			case 32:
				u.uprops[DEAC_SICK_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having sickness resistance!");
				break;
			case 33:
			case 34:
				u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having magic resistance!");
				break;
			case 35:
			case 36:
			case 37:
			case 38:
				u.uprops[DEAC_ACID_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having acid resistance!");
				break;
			case 39:
			case 40:
				u.uprops[DEAC_STONE_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having petrification resistance!");
				break;
			case 41:
				u.uprops[DEAC_FEAR_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having fear resistance!");
				break;
			case 42:
			case 43:
			case 44:
				u.uprops[DEAC_SEE_INVIS].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having see invisible!");
				break;
			case 45:
			case 46:
			case 47:
				u.uprops[DEAC_TELEPAT].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having telepathy!");
				break;
			case 48:
			case 49:
			case 50:
				u.uprops[DEAC_WARNING].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having warning!");
				break;
			case 51:
			case 52:
			case 53:
				u.uprops[DEAC_SEARCHING].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having automatic searching!");
				break;
			case 54:
				u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having clairvoyance!");
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
				u.uprops[DEAC_INFRAVISION].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having infravision!");
				break;
			case 60:
				u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having detect monsters!");
				break;
			case 61:
			case 62:
			case 63:
				u.uprops[DEAC_INVIS].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having invisibility!");
				break;
			case 64:
				u.uprops[DEAC_DISPLACED].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having displacement!");
				break;
			case 65:
			case 66:
			case 67:
				u.uprops[DEAC_STEALTH].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having stealth!");
				break;
			case 68:
				u.uprops[DEAC_JUMPING].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having jumping!");
				break;
			case 69:
			case 70:
			case 71:
				u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having teleport control!");
				break;
			case 72:
				u.uprops[DEAC_FLYING].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having flying!");
				break;
			case 73:
				u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having magical breathing!");
				break;
			case 74:
				u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having phasing!");
				break;
			case 75:
			case 76:
				u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having slow digestion!");
				break;
			case 77:
				u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having half spell damage!");
				break;
			case 78:
				u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having half physical damage!");
				break;
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
				u.uprops[DEAC_REGENERATION].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having regeneration!");
				break;
			case 84:
			case 85:
				u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having mana regeneration!");
				break;
			case 86:
			case 87:
			case 88:
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having polymorph control!");
				break;
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
				u.uprops[DEAC_FAST].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having speed!");
				break;
			case 94:
			case 95:
			case 96:
				u.uprops[DEAC_REFLECTING].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having reflection!");
				break;
			case 97:
			case 98:
			case 99:
				u.uprops[DEAC_FREE_ACTION].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having free action!");
				break;
			case 100:
				u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from hallu partying!");
				break;
			case 101:
				u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from drunken boxing!");
				break;
			case 102:
				u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having stunnopathy!");
				break;
			case 103:
				u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having numbopathy!");
				break;
			case 104:
				u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having freezopathy!");
				break;
			case 105:
				u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from being a stoned chiller!");
				break;
			case 106:
				u.uprops[DEAC_CORROSIVITY].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having corrosivity!");
				break;
			case 107:
				u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having an increased fear factor!");
				break;
			case 108:
				u.uprops[DEAC_BURNOPATHY].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having burnopathy!");
				break;
			case 109:
				u.uprops[DEAC_SICKOPATHY].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having sickopathy!");
				break;
			case 110:
				u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having keen memory!");
				break;
			case 111:
				u.uprops[DEAC_THE_FORCE].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from using the force like a real jedi!");
				break;
			case 112:
				u.uprops[DEAC_SIGHT_BONUS].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having extra sight!");
				break;
			case 113:
				u.uprops[DEAC_VERSUS_CURSES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having curse resistance!");
				break;
			case 114:
				u.uprops[DEAC_STUN_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having stun resistance!");
				break;
			case 115:
				u.uprops[DEAC_CONF_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having confusion resistance!");
				break;
			case 116:
				u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having double attacks!");
				break;
			case 117:
				u.uprops[DEAC_QUAD_ATTACK].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having quad attacks!");
				break;
			case 118:
				u.uprops[DEAC_PSI_RES].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having psi resistance!");
				break;
			case 119:
				u.uprops[DEAC_WONDERLEGS].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having wonderlegs!");
				break;
			case 120:
				u.uprops[DEAC_GLIB_COMBAT].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having glib combat!");
				break;
			case 121:
				u.uprops[DEAC_MANALEECH].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having manaleech!");
				break;
			case 122:
				u.uprops[DEAC_DIMMOPATHY].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having dimmopathy!");
				break;
			case 123:
				u.uprops[DEAC_PEACEVISION].intrinsic += rnz( (dmg * rnd(30) ) + 1);
				pline("You are prevented from having peacevision!");
				break;
		}

		break;

	    case AD_ICUR:
		hitmsg(mtmp, mattk);

		if (!rn2(5)) {
			You_feel("as if you need some help.");
			rndcurse();
		}

		break;

	    case AD_SLUD:
		hitmsg(mtmp, mattk);

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

		if (!rn2(50)) {
			pline("Nasty!");

			switch (rnd(85)) {

				case 1: RMBLoss += rnz( (dmg + 2) * rnd(100) ); break;
				case 2: NoDropProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 3: DSTWProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 4: StatusTrapProblem += rnz( (dmg + 2) * rnd(100) ); 
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
				case 5: Superscroller += rnz( (dmg + 2) * rnd(100) * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz( (dmg + 2) * rnd(100) ); break;
				case 7: FreeHandLoss += rnz( (dmg + 2) * rnd(100) ); break;
				case 8: Unidentify += rnz( (dmg + 2) * rnd(100) ); break;
				case 9: Thirst += rnz( (dmg + 2) * rnd(100) ); break;
				case 10: LuckLoss += rnz( (dmg + 2) * rnd(100) ); break;
				case 11: ShadesOfGrey += rnz( (dmg + 2) * rnd(100) ); break;
				case 12: FaintActive += rnz( (dmg + 2) * rnd(100) ); break;
				case 13: Itemcursing += rnz( (dmg + 2) * rnd(100) ); break;
				case 14: DifficultyIncreased += rnz( (dmg + 2) * rnd(100) ); break;
				case 15: Deafness += rnz( (dmg + 2) * rnd(100) ); flags.soundok = 0; break;
				case 16: CasterProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 17: WeaknessProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 18: RotThirteen += rnz( (dmg + 2) * rnd(100) ); break;
				case 19: BishopGridbug += rnz( (dmg + 2) * rnd(100) ); break;
				case 20: UninformationProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 21: StairsProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 22: AlignmentProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 23: ConfusionProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 24: SpeedBug += rnz( (dmg + 2) * rnd(100) ); break;
				case 25: DisplayLoss += rnz( (dmg + 2) * rnd(100) ); break;
				case 26: SpellLoss += rnz( (dmg + 2) * rnd(100) ); break;
				case 27: YellowSpells += rnz( (dmg + 2) * rnd(100) ); break;
				case 28: AutoDestruct += rnz( (dmg + 2) * rnd(100) ); break;
				case 29: MemoryLoss += rnz( (dmg + 2) * rnd(100) ); break;
				case 30: InventoryLoss += rnz( (dmg + 2) * rnd(100) ); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (dmg * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 33: BloodLossProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 34: BadEffectProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 35: TrapCreationProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 36: AutomaticVulnerabilitiy += rnz( (dmg + 2) * rnd(100) ); break;
				case 37: TeleportingItems += rnz( (dmg + 2) * rnd(100) ); break;
				case 38: NastinessProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 39: CaptchaProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 40: FarlookProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 41: RespawnProblem += rnz( (dmg + 2) * rnd(100) ); break;
				case 42: RecurringAmnesia += rnz( (dmg + 2) * rnd(100) ); break;
				case 43: BigscriptEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 44: {
					BankTrapEffect += rnz( (dmg + 2) * rnd(100) );
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 46: TechTrapEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 47: RecurringDisenchant += rnz( (dmg + 2) * rnd(100) ); break;
				case 48: verisiertEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 49: ChaosTerrain += rnz( (dmg + 2) * rnd(100) ); break;
				case 50: Muteness += rnz( (dmg + 2) * rnd(100) ); break;
				case 51: EngravingDoesntWork += rnz( (dmg + 2) * rnd(100) ); break;
				case 52: MagicDeviceEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 53: BookTrapEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 54: LevelTrapEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 55: QuizTrapEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 56: FastMetabolismEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 57: NoReturnEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 58: AlwaysEgotypeMonsters += rnz( (dmg + 2) * rnd(100) ); break;
				case 59: TimeGoesByFaster += rnz( (dmg + 2) * rnd(100) ); break;
				case 60: FoodIsAlwaysRotten += rnz( (dmg + 2) * rnd(100) ); break;
				case 61: AllSkillsUnskilled += rnz( (dmg + 2) * rnd(100) ); break;
				case 62: AllStatsAreLower += rnz( (dmg + 2) * rnd(100) ); break;
				case 63: PlayerCannotTrainSkills += rnz( (dmg + 2) * rnd(100) ); break;
				case 64: PlayerCannotExerciseStats += rnz( (dmg + 2) * rnd(100) ); break;
				case 65: TurnLimitation += rnz( (dmg + 2) * rnd(100) ); break;
				case 66: WeakSight += rnz( (dmg + 2) * rnd(100) ); break;
				case 67: RandomMessages += rnz( (dmg + 2) * rnd(100) ); break;

				case 68: Desecration += rnz( (dmg + 2) * rnd(100) ); break;
				case 69: StarvationEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 70: NoDropsEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 71: LowEffects += rnz( (dmg + 2) * rnd(100) ); break;
				case 72: InvisibleTrapsEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 73: GhostWorld += rnz( (dmg + 2) * rnd(100) ); break;
				case 74: Dehydration += rnz( (dmg + 2) * rnd(100) ); break;
				case 75: HateTrapEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 76: TotterTrapEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 77: Nonintrinsics += rnz( (dmg + 2) * rnd(100) ); break;
				case 78: Dropcurses += rnz( (dmg + 2) * rnd(100) ); break;
				case 79: Nakedness += rnz( (dmg + 2) * rnd(100) ); break;
				case 80: Antileveling += rnz( (dmg + 2) * rnd(100) ); break;
				case 81: ItemStealingEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 82: Rebellions += rnz( (dmg + 2) * rnd(100) ); break;
				case 83: CrapEffect += rnz( (dmg + 2) * rnd(100) ); break;
				case 84: ProjectilesMisfire += rnz( (dmg + 2) * rnd(100) ); break;
				case 85: WallTrapping += rnz( (dmg + 2) * rnd(100) ); break;
			}

		}

		break;

	    case AD_VENO:
		hitmsg(mtmp, mattk);

		if (!Poison_resistance) pline("You're badly poisoned!");
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE);
		ptmp = rn2(A_MAX);
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
		    Sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, ptmp, mdat->mname, 30);
		}
		break;
	    case AD_DFOO:
	      pline("%s determines to take you down a peg or two...", Monnam(mtmp));
		if (!rn2(3)) {
		    Sprintf(buf, "%s %s",
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
			if(!Drain_resistance || !rn2(4) )
			    losexp("life drainage", FALSE, TRUE);
			else You_feel("woozy for an instant, but shrug it off.");
		}

		break;

	    case AD_DRIN:
		hitmsg(mtmp, mattk);
		if (defends(AD_DRIN, uwep) || !has_head(youmonst.data) || Role_if(PM_COURIER)) {
		    You("don't seem harmed.");
		    /* Not clear what to do for green slimes */
		    break;
		}
		if (u_slip_free(mtmp,mattk)) break;

		if (uarmh && rn2(8)) {
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
		mdamageu(mtmp, dmg);

		if (!uarmh || uarmh->otyp != DUNCE_CAP) {
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
				} else {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }

			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    lifesaved++;
			}
		    }
		}
		/* adjattrib gives dunce cap message when appropriate */
		if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
		else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
		if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
		if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		exercise(A_WIS, FALSE);
		break;
	    case AD_PLYS:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(3)) {
		    if (Free_action) {
			You("momentarily stiffen.");            
		    } else {
			if (Blind) You("are frozen!");
			else You("are frozen by %s!", mon_nam(mtmp));
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10), "paralyzed by a monster attack");
			exercise(A_DEX, FALSE);
		    }
		}
		break;
	    case AD_TCKL:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(3)) {
		    if (Free_action)
			You_feel("horrible tentacles probing your flesh!");
		    else {
			if (Blind) You("are mercilessly tickled!");
			else You("are mercilessly tickled by %s!", mon_nam(mtmp));
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10), "tickled by a monster");
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
		    } else if (Fire_resistance && rn2(20)) {
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
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		}
		if (uncancelled) {
		    pline("You're covered in frost!");
		    if (Cold_resistance && rn2(20)) {
			pline_The("frost doesn't seem cold!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);
		}
		if (uncancelled) {
		    You("get zapped!");
		    if (Shock_resistance && rn2(20)) {
			pline_The("zap doesn't shock you!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		}

		if (uncancelled && !rn2(3) && (!Drain_resistance || !rn2(4) )  ) {
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
		
		if (uncancelled && !rn2(3) && (!Drain_resistance || !rn2(4) )  ) {
		    losexp("life drainage", FALSE, TRUE);
		}
		break;

	    case AD_WEBS: 
		hitmsg(mtmp, mattk);
		{
			struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB, 0);
			if (ttmp2) {
				pline_The("webbing sticks to you. You're caught!");
				dotrap(ttmp2, NOWEBMSG);
				if (u.usteed && u.utrap) {
				/* you, not steed, are trapped */
				dismount_steed(DISMOUNT_FELL);
				}
			}
		}
		/* Amy addition: sometimes, also make a random trap somewhere on the level :D */
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap();
		break;

	    case AD_STTP:

		hitmsg(mtmp, mattk);
		if (rn2(3)) break;
		pline("You are surrounded by a purple glow!");
		if (invent) {
		    int itemportchance = 10 + rn2(21);
		    for (otmpi = invent; otmpi; otmpi = otmpii) {

		      otmpii = otmpi->nobj;

			if (!rn2(itemportchance) && !stack_too_big(otmpi) ) {

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
			}

		    }
		}
		break;

	    case AD_DEPR:
		hitmsg(mtmp, mattk);
		if (!rn2(3)) {

		    switch(rnd(20)) {
		    case 1:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
			      polyself(FALSE);
			}
			break;
		    case 2:
			You("need reboot.");
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
			} else {
				if (multi >= 0) {
				    if (Sleep_resistance && rn2(20)) break;
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
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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
			    if (Free_action) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by a monster attack");
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 14:
			if (Hallucination)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
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
		
		if (!rn2(3) && (!Drain_resistance || !rn2(4) )  ) {
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
		  if (
			(u.usteed && !is_flyer(mtmp->data) && rn2(3) ) ||
				    Levitation || (Flying && !is_flyer(mtmp->data)) ) {
		    pline("%s tries to reach your %s %s!", Monnam(mtmp),
			  sidestr, body_part(LEG));
		    dmg = 0;
		  } else if (mtmp->mcan) {
		    pline("%s nuzzles against your %s %s!", Monnam(mtmp),
			  sidestr, body_part(LEG));
		    dmg = 0;
		  } else {
		    if (uarmf) {
			if (rn2(2) && (uarmf->otyp == LOW_BOOTS ||
					     uarmf->otyp == IRON_SHOES))
			    pline("%s scratches the exposed part of your %s %s!",
				Monnam(mtmp), sidestr, body_part(LEG));
			else if (!rn2(5))
			    pline("%s scratches through your %s boot!",
				Monnam(mtmp), sidestr);
			else {
			    pline("%s scratches your %s boot!", Monnam(mtmp),
				sidestr);
			    dmg = 0;
			    break;
			}
		    } else pline("%s scratches your %s %s!", Monnam(mtmp),
			  sidestr, body_part(LEG));
		    set_wounded_legs(side, HWounded_legs + rnd(60-ACURR(A_DEX)));
		    exercise(A_STR, FALSE);
		    exercise(A_DEX, FALSE);
		  }
		  break;
		}

	    case AD_HODS:
		 if(uwep){
			if (uwep->otyp == CORPSE
				&& touch_petrifies(&mons[uwep->corpsenm])) {
			    dmg = 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[uwep->corpsenm].mname);
			    if (!Stoned && !Stone_resistance)
				goto do_stone;
			}
			dmg += dmgval(uwep, &youmonst);
			
			if (uwep->opoisoned){
				Sprintf(buf, "%s %s",
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
		hitmsg(mtmp, mattk);
		if(!rn2(3)) {
		    if (mtmp->mcan) {
			if (flags.soundok)
			    You_hear("a cough from %s!", mon_nam(mtmp));
		    } else {
			if (flags.soundok)
			    You_hear("%s hissing!", s_suffix(mon_nam(mtmp)));

			if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "petrified cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "okamenela plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "qotib plash") ) && rn2(4)) break;
	
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
 do_stone:
			    if (!Stoned && !Stone_resistance
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				Stoned = 7;
				delayed_killer = mtmp->data->mname;
				if (mtmp->data->geno & G_UNIQ) {
				    if (!type_is_pname(mtmp->data)) {
					static char kbuf[BUFSZ];

					/* "the" buffer may be reallocated */
					Strcpy(kbuf, the(delayed_killer));
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
		hitmsg(mtmp, mattk);
		if (uncancelled && !u.ustuck && !sticks(youmonst.data)) {
			setustuck(mtmp);
			pline("%s grabs you!", Monnam(mtmp));
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		}
		break;
	    case AD_WRAP:
		if ((!mtmp->mcan || u.ustuck == mtmp) && !sticks(youmonst.data)) {
		    if (!u.ustuck && !rn2(10)) {
			if (u_slip_free(mtmp, mattk)) {
			    dmg = 0;
			} else {
			    pline("%s swings itself around you!",
				  Monnam(mtmp));
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			    setustuck(mtmp);
			}
		    } else if(u.ustuck == mtmp) {

			if (is_pool(mtmp->mx,mtmp->my)) {

		/* Being drowned should ALWAYS involve your stuff getting wet. 
		 * Even if you're unbreathing, swimming or whatever. Your stuff isn't immune just because you are.  --Amy	*/

			    pline("%s pulls you into the water!", Monnam(mtmp));
				water_damage(invent, FALSE, FALSE);
				if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
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

			if (is_lava(mtmp->mx,mtmp->my) && !rn2(3)) {

				if (!Fire_resistance) {
					You("burn to ashes...");
					killer_format = KILLED_BY_AN;
					    Sprintf(buf, "pool of lava by %s", an(mtmp->data->mname));
					    killer = buf;
					done(BURNING);

				}
				else {
					display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					You("scream "); verbalize("HAHAHAHAHAHAHAAAAAAAA!"); /* Super Mario 64 */
					u.uhpmax -= rnd(10);
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				    losehp(10, "lava dive", KILLED_BY_AN);
				}

			}

			if (is_pool(mtmp->mx,mtmp->my) && !Swimming
			    && !Amphibious && !Breathless && !rn2(3) && !(uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "fin boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "plavnik sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "kanatcik chizilmasin") ) ) ) { /* greatly lowered chance of getting drowned --Amy */
			    boolean moat =
				(levl[mtmp->mx][mtmp->my].typ != POOL) &&
				(levl[mtmp->mx][mtmp->my].typ != WATER) &&
				!Is_medusa_level(&u.uz) &&
				!Is_waterlevel(&u.uz);

			    pline("%s drowns you...", Monnam(mtmp));
			    killer_format = KILLED_BY_AN;
			    Sprintf(buf, "%s by %s",
				    moat ? "moat" : "pool of water",
				    an(mtmp->data->mname));
			    killer = buf;
			    done(DROWNING);
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
		if (uncancelled && !rn2(4) && u.ulycn == NON_PM &&
			!Protection_from_shape_changers &&
			!is_were(youmonst.data) &&
			!defends(AD_WERE,uwep)) {
		    You_feel("feverish.");
		    exercise(A_CON, FALSE);
		    u.ulycn = monsndx(mdat);
			if (u.ulycn == PM_HUMAN_WERERAT) u.ulycn = PM_WERERAT;
			if (u.ulycn == PM_HUMAN_WERERABBIT) u.ulycn = PM_WERERABBIT;
			if (u.ulycn == PM_HUMAN_WEREBOAR) u.ulycn = PM_WEREBOAR;
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
			if (u.ulycn == PM_HUMAN_WEREVORTEX) u.ulycn = PM_WEREVORTEX;
			if (u.ulycn == PM_HUMAN_WEREGIANT) u.ulycn = PM_WEREGIANT;
			if (u.ulycn == PM_HUMAN_WEREGHOST) u.ulycn = PM_WEREGHOST;
			if (u.ulycn == PM_HUMAN_WEREMINDFLAYER) u.ulycn = PM_WEREMINDFLAYER;
			if (u.ulycn == PM_HUMAN_WERECOCKATRICE) u.ulycn = PM_WERECOCKATRICE;
			if (u.ulycn == PM_HUMAN_WERESOLDIERANT) u.ulycn = PM_WERESOLDIERANT;
			if (u.ulycn == PM_HUMAN_WEREBAT) u.ulycn = PM_WEREBAT;
			if (u.ulycn == PM_HUMAN_WEREPIRANHA) u.ulycn = PM_WEREPIRANHA;
			if (u.ulycn == PM_HUMAN_WEREEEL) u.ulycn = PM_WEREEEL;
			if (u.ulycn == PM_HUMAN_WEREFLYFISH) u.ulycn = PM_WEREFLYFISH;
			if (u.ulycn == PM_HUMAN_WEREKRAKEN) u.ulycn = PM_WEREKRAKEN;
			if (u.ulycn == PM_HUMAN_WERELICH) u.ulycn = PM_WERELICH;
			if (u.ulycn == PM_HUMAN_WEREJABBERWOCK) u.ulycn = PM_WEREJABBERWOCK;
			if (u.ulycn == PM_HUMAN_WEREGRIDBUG) u.ulycn = PM_WEREGRIDBUG;
		    upermonst.mflags2 |= (M2_WERE);
		}
		break;
	    case AD_SGLD:
		hitmsg(mtmp, mattk);
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
		if (is_animal(mtmp->data)) {
			hitmsg(mtmp, mattk);
			if (mtmp->mcan) break;
			/* Continue below */
		} else if (rn2(5) && !(u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stealerstone() ) && (dmgtype(youmonst.data, AD_SEDU) || (uarmg && uarmg->oartifact == ART_LORSKEL_S_SPECIAL_PROTECTI)
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

		if (!rn2(3) && !issoviet && !(u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stealerstone() ) ) {
			You_feel("a tug on your knapsack"); break;
		}

		if (!rn2(5) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "slexual cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "polovoy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "jinsiy plash") )) {
			if(could_seduceX(mtmp, &youmonst, mattk) == 1 && !mtmp->mcan)
			    if (doseduce(mtmp)) return 3;
		}

		if (!rn2(25)) { /* Nymphs want to have some fun! :-) --Amy */

			if(could_seduceX(mtmp, &youmonst, mattk) == 1 && !mtmp->mcan)
			    if (doseduce(mtmp)) return 3;
		/* What kind of male person wouldn't want to be seduced by such a wonderfully lovely, sweet lady? */

		}

		buf[0] = '\0';

		if (u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stealerstone() ) {
			switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if (!is_animal(mtmp->data) && !tele_restrict(mtmp) && (issoviet || !rn2(4)) )
			    (void) rloc(mtmp, FALSE);
			if (is_animal(mtmp->data) && *buf) {
			    if (canseemon(mtmp))
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
			{ switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if (!is_animal(mtmp->data) && !tele_restrict(mtmp) && (issoviet || !rn2(4) ) )
			    (void) rloc(mtmp, FALSE);
			if (is_animal(mtmp->data) && *buf) {
			    if (canseemon(mtmp))
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

		if(could_seduceX(mtmp, &youmonst, mattk) == 1
			&& !mtmp->mcan && (issoviet || rn2(2) ) ) /* 50% chance --Amy */
		    if (doseduce(mtmp))
			return 3;
		break;
	    case AD_SAMU:
		hitmsg(mtmp, mattk);
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
		/* if vampire biting (and also a pet) */
		if (!rn2(3) && !u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.levelporting) {
				u.levelporting = 1;
				nomul(-2, "being levelported"); /* because it's not called until you get another turn... */
			}

		}
		else if (!rn2(3) && (!Drain_resistance || !rn2(4) )  ) {
		    losexp("loss of potential", FALSE, TRUE);
		}
		break;

	    case AD_BANI:
		hitmsg(mtmp, mattk);
		if (!rn2(3)) {
			if (u.uevent.udemigod || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed)) ) { pline("You shudder for a moment."); (void) safe_teleds(FALSE); break;}
			if (flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) {
			 pline("For some reason you resist the banishment!"); break;}

			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.banishmentbeam) {
				u.banishmentbeam = 1;
				nomul(-2, "being banished"); /* because it's not called until you get another turn... */
			}
		}
		break;

	    case AD_DISP:
		hitmsg(mtmp, mattk);
		pushplayer();
		break;

	    case AD_RUST:
		hitmsg(mtmp, mattk);
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
		if (rn2(3)) hurtarmor(AD_RUST);
		break;
	    case AD_CORR:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (rn2(3)) hurtarmor(AD_CORR);
		break;
	    case AD_WTHR:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (rn2(3)) witherarmor();
		break;

	    case AD_SHRD:
		hitmsg(mtmp, mattk);
		struct obj *obj = some_armor(&youmonst);

		if (obj && drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
		} else if (obj && rn2(3)) wither_dmg(obj, xname(obj), rn2(4), FALSE, &youmonst);
		
		break;

	    case AD_NPRO:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (!rn2(3)) {
			u.negativeprotection++;
			You_feel("less protected!");
		}
		break;

	    case AD_CHKH:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		dmg += u.chokhmahdamage;
		dmg += rnd(u.ualign.sins + 1);
		u.chokhmahdamage++;
		break;

	    case AD_THIR:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		pline("%s sucks your %s!", Monnam(mtmp), body_part(BLOOD) );
		mtmp->mhp += dmg;
		if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

		break;

	    case AD_SUCK:
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
						if( rnd(40) > ACURR(A_STR)){
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
		if (mtmp->mcan) break;
		if (!rn2(3)) {
			pline("Water washes over you!");
			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
		}
		break;

	    case AD_LETH:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (!rn2(3)) {
			pline("Sparkling water washes over you!");
			lethe_damage(invent, FALSE, FALSE);
			if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
			if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		}
		break;

	    case AD_CNCL:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (!rn2(3)) {
			(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		}
		break;

	    case AD_LUCK:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (!rn2(3)) {change_luck(-1);
			You_feel("unlucky.");
		}
		break;

	    case AD_FAKE:
		hitmsg(mtmp, mattk);
		pline(fauxmessage());
		if (!rn2(3)) pline(fauxmessage());
		break;

	    case AD_NEXU:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;

		if (level.flags.noteleport || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) dmg *= (1 + rnd(2));

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

				if (!u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported"); /* because it's not called until you get another turn... */
					}
				}
				break;
			case 7:
				{
					int firststat = rn2(A_MAX);
					int secondstat = rn2(A_MAX);
					int firstswapstat = ABASE(firststat);
					int secondswapstat = ABASE(secondstat);
					int difference = (firstswapstat - secondswapstat);
					ABASE(secondstat) += difference;
					ABASE(firststat) -= difference;
					AMAX(secondstat) = ABASE(secondstat);
					AMAX(firststat) = ABASE(firststat);
					pline("Your stats got scrambled!");
				}
				break;
		}
		break;

	    case AD_SOUN:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		pline("Your ears are blasted by hellish noise!");
		if (Deafness || (uwep && uwep->oartifact == ART_MEMETAL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_MEMETAL) || (uwep && uwep->oartifact == ART_BANG_BANG) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_BANG_BANG) || u.uprops[DEAFNESS].extrinsic || have_deafnessstone() ) dmg /= 2;
		make_stunned(HStun + dmg, TRUE);
		if (!rn2(issoviet ? 2 : 5)) (void)destroy_item(POTION_CLASS, AD_COLD);
		wake_nearby();
		break;

	    case AD_GRAV:

		if (level.flags.noteleport || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) dmg *= 2;

		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		pline("Gravity warps around you...");
		phase_door(0);
		pushplayer();
		u.uprops[DEAC_FAST].intrinsic += (dmg + 2);
		make_stunned(HStun + dmg, TRUE);
		break;

	    case AD_INER:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
	      u_slow_down();
		u.uprops[DEAC_FAST].intrinsic += ((dmg + 2) * 10);
		pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
		u.inertia += (dmg + 2);
		break;

	    case AD_TIME:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
			      losexp("time", FALSE, FALSE); /* resistance is futile :D */
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
		if (mtmp->mcan) break;

			pline("You're seared by %s hot plasma radiation!", Fire_resistance ? "very" : "extremely");
			if (!Fire_resistance) dmg *= 2;

		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) /* extremely hot - very high chance to burn items! --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
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
		if (mtmp->mcan) break;
		if (!rn2(1000)) skillcaploss();
		break;

	    case AD_HALU:
		hitmsg(mtmp, mattk);
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

		if (!rn2(10))  {
		if (Disint_resistance && rn2(100)) {
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
	    done(DIED);
	    return 1; /* lifesaved */

	}
	      break;
	    case AD_DCAY:
		hitmsg(mtmp, mattk);
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
		if (rn2(3)) hurtarmor(AD_DCAY);
		break;
	    case AD_HEAL:
		/* a cancelled nurse is just an ordinary monster */
		if (mtmp->mcan) {
		    hitmsg(mtmp, mattk);
		    break;
		}
		if(!uwep
		   && !uarmu
		   && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    boolean goaway = FALSE;
		    pline("%s hits!  (I hope you don't mind.)", Monnam(mtmp));
		    if (Upolyd) {
			u.mh += rnd(7);
/* STEPHEN WHITE'S NEW CODE */                                            
			if (!rn2(7)) {
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			    if (!rn2(13)) goaway = TRUE;
			}
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			if (!rn2(10)) {
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10)) {
				u.uhpmax++;
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
			if (flags.soundok && !(moves % 5))
		      verbalize("Doc, I can't help you unless you cooperate.");
			dmg = 0;
		    } else hitmsg(mtmp, mattk);
		}
		break;
	    case AD_CURS:
	    case AD_LITE:
		hitmsg(mtmp, mattk);
		/* if(!night() && mdat == &mons[PM_GREMLIN]) break; */

		/* Yeah I know, I just made gremlins and other AD_CURS using monsters a lot more dangerous.
		They're supposed to appear late in the game, adding a bit of risk to high-level-characters.
		I mean come on, early game is hell but late game is cake? Now you can lose your intrinsics at any time!
		If you lose poison resistance, try eating some corpses to get it back.
		If you lose sickness resistance, well, tough luck - it's not coming back. Ever. --Amy*/
		if((!mtmp->mcan && !rn2(10)) || (night() && !rn2(3)) ) {
		    if (flags.soundok) {
			if (Blind) You_hear("laughter.");
			else       pline("%s chuckles.", Monnam(mtmp));
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
		if(!mtmp->mcan && !rn2(4) && (!Psi_resist || !rn2(20)) ) {

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
			if (!rn2(200)) {
				losexp("psionic drain", FALSE, TRUE);
			}
			if (!rn2(200)) {
				adjattrib(A_INT, -1, 1);
				adjattrib(A_WIS, -1, 1);
			}
			if (!rn2(200)) {
				pline("You scream in pain!");
				wake_nearby();
			}
			if (!rn2(200)) {
				badeffect();
			}

		}
		break;

	    case AD_STUN:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(4)) {
		    make_stunned(HStun + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_NUMB:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(4)) {
		    make_numbed(HNumbed + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_FRZE:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(10)) {
		    make_frozen(HFrozen + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_BURN:
		hitmsg(mtmp, mattk);
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
		if(!mtmp->mcan && !rn2(3)) {
		    make_feared(HFeared + dmg, TRUE);
		    dmg /= 2;
		}
		break;

	    case AD_CHRN:
		hitmsg(mtmp, mattk);

		    switch (rn2(11)) {
		    case 0: diseasemu(mdat);
			    break;
		    case 1: make_blinded(Blinded + dmg, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    Hallucination ? "trippy" : "confused");
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
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE);
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
		if(!mtmp->mcan && !rn2(3)) {
		    if (Acid_resistance && rn2(20)) {
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
		if (uncancelled && HFast &&
					!defends(AD_SLOW, uwep) && !rn2(4))
		    u_slow_down();
		break;
	    case AD_DREN:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(4))
		    drain_en(dmg);
		dmg = 0;
		break;
	    case AD_CONF:
		hitmsg(mtmp, mattk);
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
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("Was that the touch of death?");
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!Antimagic) {
			killer_format = KILLED_BY_AN;
			killer = "touch of death";
			done(DIED);
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
		(void) diseasemu(mdat); /* plus the normal damage */
		/* No damage if invulnerable; setting dmg zero prevents
		 * "You are unharmed!" after a sickness inducing attack */
		if (Invulnerable || (Stoned_chiller && Stoned)) dmg = 0;
		break;
	    case AD_FAMN:
		pline("%s reaches out, and your body shrivels.",
			Monnam(mtmp));
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
		if(!rn2(2)) {
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
			    if (Sleep_resistance && rn2(20)) {pline("You yawn."); break;}
			    fall_asleep(-rnd(10), TRUE);
			    if (Blind) You("are put to sleep!");
			    else You("are put to sleep by %s!", mon_nam(mtmp));
			}
			break;
		    case 2: /* paralyse */
			if (multi >= 0) {
			    if (Free_action) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by a monster attack");
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 3: /* slow */
			if(HFast)  u_slow_down();
			else You("pause momentarily.");
			break;
		    case 4: /* drain Dex */
			adjattrib(A_DEX, -rn1(1,1), 0);
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
	    case AD_POLY:
		hitmsg(mtmp, mattk);
		if (uncancelled && !Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
		    polyself(FALSE);
		}
		break;
	    case AD_MAGM:
		hitmsg(mtmp, mattk);
		    if(Antimagic && rn2(5)) {
			shieldeff(u.ux, u.uy);
			dmg = 0;
			pline("A hail of magic missiles narrowly misses you!");
		    } else {
			You("are hit by magic missiles appearing from thin air!");
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
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Yuck!");
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		hitmsg(mtmp, mattk);
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		if (uncancelled) {
		    struct obj *obj = some_armor(&youmonst);

		    if (obj && drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
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
		if( (mtmp->egotype_weaponizer || mattk->aatyp == AT_WEAP) && otmp && atttyp > AD_PHYS) {
			int nopoison = (10/* - (otmp->owt/10)*/);
			if (otmp->otyp == CORPSE &&
				touch_petrifies(&mons[otmp->corpsenm])) {
			    dmg += 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[otmp->corpsenm].mname);
			    if (!Stoned && !Stone_resistance && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM)) ) goto do_stone;
			}

			/* MRKR: If hit with a burning torch,     */
			/*       then do an extra point of damage */
			/*       but save the message till after  */
			/*       the hitmsg()                     */

			if (otmp->otyp == TORCH && otmp->lamplit &&
			    !Fire_resistance) {
			  burnmsg = TRUE;
			  dmg++;
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
				dmg += rnd(2); /* don't lose the base damage from monst.txt --Amy */

			} else dmg += dmgval(otmp, &youmonst);

			if (objects[otmp->otyp].oc_material == SILVER &&
				hates_silver(youmonst.data)) {
			    pline("The silver sears your flesh!");
			}
			if (objects[otmp->otyp].oc_material == VIVA && hates_viva(youmonst.data)) {
			    pline("The irradiation severely hurts you!");
			}
			if (objects[otmp->otyp].oc_material == INKA) {
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
				    pline("%s plunges the stake into your heart.",
					    Monnam(mtmp));
				    killer = "a wooden stake in the heart.";
				    killer_format = KILLED_BY_AN;
				    u.ugrave_arise = NON_PM; /* No corpse */
				    done(DIED);
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
			    if (!rn2(nopoison)) {
				otmp->opoisoned = FALSE;
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

			  if (!rn2(2) && burnarmor(&youmonst)) {
			    dmg++;

			    /* Torch flame is not hot enough to guarantee */
			    /* burning away slime */

			    if (!rn2(4)) burn_away_slime();
			    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
			      (void)destroy_item(POTION_CLASS, AD_FIRE);
			    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
			      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
			    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
			      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
			  }
			  burn_faster(otmp, 1);
			}

			if (u.mh > 1 && u.mh > ((u.uac>0) ? dmg : dmg+u.uac) &&
				   objects[otmp->otyp].oc_material == IRON &&
					(u.umonnum==PM_BLACK_PUDDING || u.umonnum==PM_BLACKSTEEL_PUDDING || u.umonnum==PM_BLOOD_PUDDING
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
		tempval = rnd(-(issoviet ? (u.uac - 20) : u.uac)/5+1);
		if (tempval < 1)  tempval = 1;
		if (tempval > 20) tempval = 20; /* max limit increased --Amy */
		dmg -= tempval;
		if (dmg < 1) dmg = 1;
	}

	if(dmg) {
	    if ( (Half_physical_damage && rn2(2)) 
					/* Mitre of Holiness */
		|| (Role_if(PM_PRIEST) && uarmh && is_quest_artifact(uarmh) &&
		    (is_undead(mtmp->data) || is_demon(mtmp->data))))
		dmg = (dmg+1) / 2;

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
	int	tim_tmp;
	register struct obj *otmp2;
	int	i;
	/*int randattackA = 0;*/
	uchar atttypA;
	int hallutime;
	struct obj *optr;

	if (!u.uswallow) {	/* swallows you */
		if (youmonst.data->msize >= MZ_HUGE) return(0);
		if ((t && ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT) || (t->ttyp == GIANT_CHASM) || (t->ttyp == SHIT_PIT) || (t->ttyp == MANA_PIT))) &&
		    sobj_at(BOULDER, u.ux, u.uy))
			return(0);

		if (Punished) unplacebc();	/* ball&chain go away */
		remove_monster(mtmp->mx, mtmp->my);
		mtmp->mtrapped = 0;		/* no longer on old trap */
		place_monster(mtmp, u.ux, u.uy);
		newsym(mtmp->mx,mtmp->my);
		if (is_animal(mtmp->data) && u.usteed) {
			char buf[BUFSZ];
			/* Too many quirks presently if hero and steed
			 * are swallowed. Pretend purple worms don't
			 * like horses for now :-)
			 */
			Strcpy(buf, mon_nam(u.usteed));
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

	if (atttypA == AD_RBRE) {
		while (atttypA == AD_ENDS ||atttypA == AD_RBRE || atttypA == AD_SPC2 || atttypA == AD_WERE) {
			atttypA = randattack(); }
		/*randattack = 1;*/
	}

	switch(atttypA) {

		case AD_DGST:
		    if (Slow_digestion) {
			/* Messages are handled below */
			u.uswldtim = 0;
			tmp = 0;
		    } else if (u.uswldtim == 0) {
			pline("%s totally digests you!", Monnam(mtmp));
			tmp = u.uhp;
			if (Half_physical_damage) tmp *= 2; /* sorry */
		    } else {
			pline("%s%s digests you!", Monnam(mtmp),
			      (u.uswldtim == 3) ? " strongly" :
			      (u.uswldtim == 2) ? " thoroughly" :
			      (u.uswldtim == 1) ? " utterly" : "");
			exercise(A_STR, FALSE);
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
				if (flags.soundok) You_hear("a chuckling laughter.");
			    attrcurse();
			}
			break;

	      case AD_SPC2:
			if (Psi_resist && rn2(20) ) break;

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
			if (!rn2(200)) {
				losexp("psionic drain", FALSE, TRUE);
			}
			if (!rn2(200)) {
				adjattrib(A_INT, -1, 1);
				adjattrib(A_WIS, -1, 1);
			}
			if (!rn2(200)) {
				pline("You scream in pain!");
				wake_nearby();
			}
			if (!rn2(200)) {
				badeffect();
			}

			break;

		case AD_SLEE:
			pline("It slaps you!");
			if (!rn2(5) && multi >= 0) {
			    if (Sleep_resistance && rn2(20)) break;
			    fall_asleep(-rnd(10), TRUE);
			    You("suddenly fall asleep!");
			}
			break;
		case AD_DRST:
				You_feel("your strength drain away!");
			if (!rn2(8)) {
			    poisoned("The attack", A_STR, "strength drain", 30);
			}
			break;
		case AD_DRDX:
				You_feel("your muscles cramping!");
			if (!rn2(8)) {
			    poisoned("The attack", A_DEX, "dexterity drain", 30);
			}
			break;
		case AD_DRCO:
				You_feel("a lack of force!");
			if (!rn2(8)) {
			    poisoned("The attack", A_CON, "constitution drain", 30);
			}
			break;
		case AD_WISD:
				You_feel("naive!");
			if (!rn2(8)) {
			    poisoned("The attack", A_WIS, "wisdom drain", 30);
			}
			break;
		case AD_DRCH:
				You_feel("ugly!");
			if (!rn2(8)) {
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

	      case AD_FUMB:
				pline("Mary-Sue alert!");

			HFumbling = FROMOUTSIDE | rnd(5);
			incr_itimeout(&HFumbling, rnd(20));
			u.fumbleduration += rnz(10 * (tmp + 1) );

			break;

	      case AD_VULN:
				pline("You are covered with aggressive bacteria!");

		 switch (rnd(123)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				u.uprops[DEAC_FIRE_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having fire resistance!");
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				u.uprops[DEAC_COLD_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having cold resistance!");
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				u.uprops[DEAC_SLEEP_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having sleep resistance!");
				break;
			case 16:
			case 17:
				u.uprops[DEAC_DISINT_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having disintegration resistance!");
				break;
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
				u.uprops[DEAC_SHOCK_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having shock resistance!");
				break;
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				u.uprops[DEAC_POISON_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having poison resistance!");
				break;
			case 28:
			case 29:
			case 30:
				u.uprops[DEAC_DRAIN_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having drain resistance!");
				break;
			case 31:
			case 32:
				u.uprops[DEAC_SICK_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having sickness resistance!");
				break;
			case 33:
			case 34:
				u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having magic resistance!");
				break;
			case 35:
			case 36:
			case 37:
			case 38:
				u.uprops[DEAC_ACID_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having acid resistance!");
				break;
			case 39:
			case 40:
				u.uprops[DEAC_STONE_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having petrification resistance!");
				break;
			case 41:
				u.uprops[DEAC_FEAR_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having fear resistance!");
				break;
			case 42:
			case 43:
			case 44:
				u.uprops[DEAC_SEE_INVIS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having see invisible!");
				break;
			case 45:
			case 46:
			case 47:
				u.uprops[DEAC_TELEPAT].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having telepathy!");
				break;
			case 48:
			case 49:
			case 50:
				u.uprops[DEAC_WARNING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having warning!");
				break;
			case 51:
			case 52:
			case 53:
				u.uprops[DEAC_SEARCHING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having automatic searching!");
				break;
			case 54:
				u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having clairvoyance!");
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
				u.uprops[DEAC_INFRAVISION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having infravision!");
				break;
			case 60:
				u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having detect monsters!");
				break;
			case 61:
			case 62:
			case 63:
				u.uprops[DEAC_INVIS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having invisibility!");
				break;
			case 64:
				u.uprops[DEAC_DISPLACED].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having displacement!");
				break;
			case 65:
			case 66:
			case 67:
				u.uprops[DEAC_STEALTH].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having stealth!");
				break;
			case 68:
				u.uprops[DEAC_JUMPING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having jumping!");
				break;
			case 69:
			case 70:
			case 71:
				u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having teleport control!");
				break;
			case 72:
				u.uprops[DEAC_FLYING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having flying!");
				break;
			case 73:
				u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having magical breathing!");
				break;
			case 74:
				u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having phasing!");
				break;
			case 75:
			case 76:
				u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having slow digestion!");
				break;
			case 77:
				u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having half spell damage!");
				break;
			case 78:
				u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having half physical damage!");
				break;
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
				u.uprops[DEAC_REGENERATION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having regeneration!");
				break;
			case 84:
			case 85:
				u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having mana regeneration!");
				break;
			case 86:
			case 87:
			case 88:
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having polymorph control!");
				break;
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
				u.uprops[DEAC_FAST].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having speed!");
				break;
			case 94:
			case 95:
			case 96:
				u.uprops[DEAC_REFLECTING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having reflection!");
				break;
			case 97:
			case 98:
			case 99:
				u.uprops[DEAC_FREE_ACTION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having free action!");
				break;
			case 100:
				u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from hallu partying!");
				break;
			case 101:
				u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from drunken boxing!");
				break;
			case 102:
				u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having stunnopathy!");
				break;
			case 103:
				u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having numbopathy!");
				break;
			case 104:
				u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having freezopathy!");
				break;
			case 105:
				u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from being a stoned chiller!");
				break;
			case 106:
				u.uprops[DEAC_CORROSIVITY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having corrosivity!");
				break;
			case 107:
				u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having an increased fear factor!");
				break;
			case 108:
				u.uprops[DEAC_BURNOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having burnopathy!");
				break;
			case 109:
				u.uprops[DEAC_SICKOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having sickopathy!");
				break;
			case 110:
				u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having keen memory!");
				break;
			case 111:
				u.uprops[DEAC_THE_FORCE].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from using the force like a real jedi!");
				break;
			case 112:
				u.uprops[DEAC_SIGHT_BONUS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having extra sight!");
				break;
			case 113:
				u.uprops[DEAC_VERSUS_CURSES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having curse resistance!");
				break;
			case 114:
				u.uprops[DEAC_STUN_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having stun resistance!");
				break;
			case 115:
				u.uprops[DEAC_CONF_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having confusion resistance!");
				break;
			case 116:
				u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having double attacks!");
				break;
			case 117:
				u.uprops[DEAC_QUAD_ATTACK].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having quad attacks!");
				break;
			case 118:
				u.uprops[DEAC_PSI_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having psi resistance!");
				break;
			case 119:
				u.uprops[DEAC_WONDERLEGS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having wonderlegs!");
				break;
			case 120:
				u.uprops[DEAC_GLIB_COMBAT].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having glib combat!");
				break;
			case 121:
				u.uprops[DEAC_MANALEECH].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having manaleech!");
				break;
			case 122:
				u.uprops[DEAC_DIMMOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having dimmopathy!");
				break;
			case 123:
				u.uprops[DEAC_PEACEVISION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having peacevision!");
				break;
		}

			break;

	      case AD_ICUR:
				pline("You are sapped by dark magic!");

			if (!rn2(5)) {
				You_feel("as if you need some help.");
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

		if (!rn2(50)) {
			pline("It latches on to your body!");

			switch (rnd(85)) {

				case 1: RMBLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 2: NoDropProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 3: DSTWProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 4: StatusTrapProblem += rnz( (tmp + 2) * rnd(100) ); 
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
				case 5: Superscroller += rnz( (tmp + 2) * rnd(100) * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz( (tmp + 2) * rnd(100) ); break;
				case 7: FreeHandLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 8: Unidentify += rnz( (tmp + 2) * rnd(100) ); break;
				case 9: Thirst += rnz( (tmp + 2) * rnd(100) ); break;
				case 10: LuckLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 11: ShadesOfGrey += rnz( (tmp + 2) * rnd(100) ); break;
				case 12: FaintActive += rnz( (tmp + 2) * rnd(100) ); break;
				case 13: Itemcursing += rnz( (tmp + 2) * rnd(100) ); break;
				case 14: DifficultyIncreased += rnz( (tmp + 2) * rnd(100) ); break;
				case 15: Deafness += rnz( (tmp + 2) * rnd(100) ); flags.soundok = 0; break;
				case 16: CasterProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 17: WeaknessProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 18: RotThirteen += rnz( (tmp + 2) * rnd(100) ); break;
				case 19: BishopGridbug += rnz( (tmp + 2) * rnd(100) ); break;
				case 20: UninformationProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 21: StairsProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 22: AlignmentProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 23: ConfusionProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 24: SpeedBug += rnz( (tmp + 2) * rnd(100) ); break;
				case 25: DisplayLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 26: SpellLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 27: YellowSpells += rnz( (tmp + 2) * rnd(100) ); break;
				case 28: AutoDestruct += rnz( (tmp + 2) * rnd(100) ); break;
				case 29: MemoryLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 30: InventoryLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (tmp * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 33: BloodLossProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 34: BadEffectProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 35: TrapCreationProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 36: AutomaticVulnerabilitiy += rnz( (tmp + 2) * rnd(100) ); break;
				case 37: TeleportingItems += rnz( (tmp + 2) * rnd(100) ); break;
				case 38: NastinessProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 39: CaptchaProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 40: FarlookProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 41: RespawnProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 42: RecurringAmnesia += rnz( (tmp + 2) * rnd(100) ); break;
				case 43: BigscriptEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 44: {
					BankTrapEffect += rnz( (tmp + 2) * rnd(100) );
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 46: TechTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 47: RecurringDisenchant += rnz( (tmp + 2) * rnd(100) ); break;
				case 48: verisiertEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 49: ChaosTerrain += rnz( (tmp + 2) * rnd(100) ); break;
				case 50: Muteness += rnz( (tmp + 2) * rnd(100) ); break;
				case 51: EngravingDoesntWork += rnz( (tmp + 2) * rnd(100) ); break;
				case 52: MagicDeviceEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 53: BookTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 54: LevelTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 55: QuizTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 56: FastMetabolismEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 57: NoReturnEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 58: AlwaysEgotypeMonsters += rnz( (tmp + 2) * rnd(100) ); break;
				case 59: TimeGoesByFaster += rnz( (tmp + 2) * rnd(100) ); break;
				case 60: FoodIsAlwaysRotten += rnz( (tmp + 2) * rnd(100) ); break;
				case 61: AllSkillsUnskilled += rnz( (tmp + 2) * rnd(100) ); break;
				case 62: AllStatsAreLower += rnz( (tmp + 2) * rnd(100) ); break;
				case 63: PlayerCannotTrainSkills += rnz( (tmp + 2) * rnd(100) ); break;
				case 64: PlayerCannotExerciseStats += rnz( (tmp + 2) * rnd(100) ); break;
				case 65: TurnLimitation += rnz( (tmp + 2) * rnd(100) ); break;
				case 66: WeakSight += rnz( (tmp + 2) * rnd(100) ); break;
				case 67: RandomMessages += rnz( (tmp + 2) * rnd(100) ); break;

				case 68: Desecration += rnz( (tmp + 2) * rnd(100) ); break;
				case 69: StarvationEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 70: NoDropsEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 71: LowEffects += rnz( (tmp + 2) * rnd(100) ); break;
				case 72: InvisibleTrapsEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 73: GhostWorld += rnz( (tmp + 2) * rnd(100) ); break;
				case 74: Dehydration += rnz( (tmp + 2) * rnd(100) ); break;
				case 75: HateTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 76: TotterTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 77: Nonintrinsics += rnz( (tmp + 2) * rnd(100) ); break;
				case 78: Dropcurses += rnz( (tmp + 2) * rnd(100) ); break;
				case 79: Nakedness += rnz( (tmp + 2) * rnd(100) ); break;
				case 80: Antileveling += rnz( (tmp + 2) * rnd(100) ); break;
				case 81: ItemStealingEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 82: Rebellions += rnz( (tmp + 2) * rnd(100) ); break;
				case 83: CrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 84: ProjectilesMisfire += rnz( (tmp + 2) * rnd(100) ); break;
				case 85: WallTrapping += rnz( (tmp + 2) * rnd(100) ); break;
			}

		}

		break;

	      case AD_VENO:
				pline("You are covered with toxic venom!");

			if (!Poison_resistance) pline("You're badly poisoned!");
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE);

			if (!rn2(2)) {
			    poisoned("The attack", rn2(A_MAX), "extremely poisonous interior", 30);
			}
			break;

		case AD_POIS:
				You("are covered with poison!");
			if (!rn2(8)) {
			    poisoned("The attack", rn2(A_MAX), "poisonous interior", 30);
			}
			break;

		case AD_NPRO:
				You_feel("unsafe in here...");
			if (!rn2(10)) {
				u.negativeprotection++;
				You_feel("less protected!");
			}
			break;

	      case AD_THIR:
			pline("It sucks your %s!", body_part(BLOOD) );
			mtmp->mhp += tmp;
			if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

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
	      case AD_SLOW:
				You_feel("a force field!");
			if (HFast && !defends(AD_SLOW, uwep) && !rn2(4))
			    u_slow_down();
			break;
	      case AD_PLYS:
			pline("It whacks you!");
			if (multi >= 0 && !rn2(3)) {
			    if (Free_action) {
				You("cramp for a moment.");            
			    } else {
				You("can't move!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by an engulfing monster");
				exercise(A_DEX, FALSE);
			    }
			}
			break;
	      case AD_DRLI:
			pline("It sucks you!");
/* Imagine the facial expression of a player who thinks this is the mind flayer's amnesia attack. --Amy */
			if (!rn2(3) && (!Drain_resistance || !rn2(4) )  ) {
			    losexp("life drainage", FALSE, TRUE);
			}
			break;
	      case AD_VAMP:
			pline("It sucks you!");
			if (!Drain_resistance || !rn2(4) ) {
			    losexp("life drainage", FALSE, TRUE);
			}
			break;
	      case AD_DREN:
			pline("It drains you!");
			if (!rn2(4)) drain_en(tmp);
			break;

	      case AD_WEBS: /* No message. Because you can't see the web while engulfed. */
			(void) maketrap(u.ux, u.uy, WEB, 0);
			/* Amy addition: sometimes, also make a random trap somewhere on the level :D */
			if (!rn2(8)) makerandomtrap();
			break;

	    case AD_DEPR:
			You_feel("manic-depressive...");
		if (!rn2(5)) {

		    switch(rnd(20)) {
		    case 1:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
			      polyself(FALSE);
			}
			break;
		    case 2:
			You("need reboot.");
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
			} else {
				if (multi >= 0) {
				    if (Sleep_resistance && rn2(20)) break;
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
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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
			    if (Free_action) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by an engulfing monster");
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 14:
			if (Hallucination)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
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
			    if (Sleep_resistance && rn2(20)) {pline("You yawn."); break;}
			    fall_asleep(-rnd(10), TRUE);
			    if (Blind) You("are put to sleep!");
			    else You("are put to sleep by %s!", mon_nam(mtmp));
			}
			break;
		    case 2: /* paralyse */
			if (multi >= 0) {
			    if (Free_action) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by an engulfing monster");
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 3: /* slow */
			if(HFast)  u_slow_down();
			else You("pause momentarily.");
			break;
		    case 4: /* drain Dex */
			adjattrib(A_DEX, -rn1(1,1), 0);
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
		    Sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, rn2(A_MAX), mtmp->data->mname, 30);
		}
		if (!rn2(4)) {
			You_feel("drained...");
			u.uhpmax -= rn1(10,10);
			if (u.uhpmax < 0) u.uhpmax = 0;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (!rn2(4)) {
			You_feel("less energised!");
			u.uenmax -= rn1(10,10);
			if (u.uenmax < 0) u.uenmax = 0;
			if(u.uen > u.uenmax) u.uen = u.uenmax;
		}
		if (!rn2(4)) {
			if(!Drain_resistance || !rn2(4) )
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

			You_feel("a constricting darkness...");

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
	    pline("It strikes you hard!");
		if(!rn2(3)) {
			if (flags.soundok)
			    You_hear("a hissing noise!");
			if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "petrified cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "okamenela plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "qotib plash") ) && rn2(4)) break;

do_stone2:
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
			    if (!Stoned && !Stone_resistance
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				Stoned = 7;
				delayed_killer = mtmp->data->mname;
				if (mtmp->data->geno & G_UNIQ) {
				    if (!type_is_pname(mtmp->data)) {
					static char kbuf[BUFSZ];

					/* "the" buffer may be reallocated */
					Strcpy(kbuf, the(delayed_killer));
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
		break;
	    case AD_STCK:
	    pline("You are covered with some sticky substance!");
		if (!u.ustuck && !sticks(youmonst.data)) {
			setustuck(mtmp);
			pline("%s grabs you!", Monnam(mtmp));
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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
	    case AD_STTP:
		pline( (atttypA == AD_STTP) ? "You are surrounded by a purple glow!" : "It thrusts you!");

		if (!rn2(3) && !issoviet && !(u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stealerstone() ) ) {
			You_feel("a tug on your knapsack"); break;
		}

			buf[0] = '\0';

		if (u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stealerstone() ) {
			switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp) && (issoviet || !rn2(4) ) )
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, rnd(10), FALSE, FALSE);
			return 3;
			};
		} else if (issoviet || ( (rnd(100) > ACURR(A_CHA)) && ((mtmp->female) && !flags.female && rn2(5) ) || ((!mtmp->female) && flags.female && rn2(3) ) || 
			((mtmp->female) && flags.female && rn2(2) ) || ((!mtmp->female) && !flags.female && rn2(2) ) ) )
			{ 
			switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp) && (issoviet || !rn2(4) ) )
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, rnd(10), FALSE, FALSE);
			return 3;
			};
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
		pushplayer();
		break;

	    case AD_UVUU:{
		pline("A drill bores into your brain!");
		if (rn2(10)) break;
		int wdmg = (int)(tmp/6) + 1;
		Sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		poisoned(buf, A_CON, mtmp->data->mname, 60);
		if(Poison_resistance) wdmg -= ACURR(A_CON)/2;
		if(wdmg > 0){
		
			while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
				wdmg--;
				(void) adjattrib(A_WIS, -1, TRUE);
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
		if (rn2(3)) hurtarmor(AD_RUST);
		break;
	    case AD_CORR:
		pline("You are covered with acid!");
		if (Stoned) fix_petrification();
		if (rn2(3)) hurtarmor(AD_CORR);
		break;

	    case AD_LAVA:
		pline("You are covered with lava!");

		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance && rn2(20)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly hot.");
				ugolemeffects(AD_FIRE,tmp);
				tmp = 0;
			} else You("are burning to a crisp!");
			burn_away_slime();
		    } else tmp = 0;

		if (rn2(3)) hurtarmor(AD_LAVA);
		break;

	    case AD_NEXU:
		if (mtmp->mcan) break;
		You_feel("an energy irradiation!");

		if (level.flags.noteleport || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= (1 + rnd(2));

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

				if (!u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported"); /* because it's not called until you get another turn... */
					}
				}
				break;
			case 7:
				{
					int firststat = rn2(A_MAX);
					int secondstat = rn2(A_MAX);
					int firstswapstat = ABASE(firststat);
					int secondswapstat = ABASE(secondstat);
					int difference = (firstswapstat - secondswapstat);
					ABASE(secondstat) += difference;
					ABASE(firststat) -= difference;
					AMAX(secondstat) = ABASE(secondstat);
					AMAX(firststat) = ABASE(firststat);
					pline("Your stats got scrambled!");
				}
				break;
		}
		break;

	    case AD_SOUN:
		if (mtmp->mcan) break;
		pline("AUUUUUUGGGGGHHHHHGGHH - the noise in here is unbearable!");
		if (Deafness || (uwep && uwep->oartifact == ART_MEMETAL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_MEMETAL) || (uwep && uwep->oartifact == ART_BANG_BANG) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_BANG_BANG) || u.uprops[DEAFNESS].extrinsic || have_deafnessstone() ) tmp /= 2;
		make_stunned(HStun + tmp, TRUE);
		if (!rn2(issoviet ? 2 : 5)) (void)destroy_item(POTION_CLASS, AD_COLD);
		wake_nearby();
		break;

	    case AD_GRAV:
		if (mtmp->mcan) break;

		if (level.flags.noteleport || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= 2;

		pline("You're turned upside down...");
		phase_door(0);
		pushplayer();
		u.uprops[DEAC_FAST].intrinsic += (tmp + 2);
		make_stunned(HStun + tmp, TRUE);
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
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
			      losexp("time", FALSE, FALSE); /* resistance is futile :D */
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

		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) /* extremely hot - very high chance to burn items! --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
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
		if (!rn2(1000)) skillcaploss();
		break;

	    case AD_WTHR:
		pline("You are covered with some aggressive substance!");
		if (rn2(3)) witherarmor();
		break;

	    case AD_FAKE:
		pline(fauxmessage());
		if (!rn2(3)) pline(fauxmessage());
		break;

	    case AD_LETH:
		pline("Sparkling water washes all around you!");
		if (mtmp->mcan) break;
		if (!rn2(10)) {
			pline("You sparkle!");
			lethe_damage(invent, FALSE, FALSE);
			if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
			if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		}
		break;

	    case AD_WET:
		pline("Water washes all around you!");
		if (mtmp->mcan) break;
		if (!rn2(10)) {
			pline("You are soaked with water!");
			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
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
						if( rnd(40) > ACURR(A_STR)){
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
			if (u.uevent.udemigod || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds(FALSE); break;}
			if (flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) {
			 pline("For some reason you resist the banishment!"); break;}

			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.banishmentbeam) {
				u.banishmentbeam = 1;
				nomul(-2, "being banished"); /* because it's not called until you get another turn... */
			}
		}
		break;

	    case AD_WEEP:
		if (flags.soundok) You_hear("weeping sounds!");
		if (rn2(10)) break;
		if (!rn2(3) && !u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
			if (!u.levelporting) {
				u.levelporting = 1;
				nomul(-2, "being levelported"); /* because it's not called until you get another turn... */
			}
		}
		else if (!rn2(3) && (!Drain_resistance || !rn2(4) )  ) {
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
		if (rn2(3)) hurtarmor(AD_DCAY);
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
		    if (!Antimagic) {
			killer_format = KILLED_BY_AN;
			killer = "engulf of death";
			done(DIED);
			tmp = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You_feel("your life force draining away...");
			if (Antimagic || (Half_spell_damage && rn2(2) )) {
				shieldeff(u.ux, u.uy);
				tmp /= 2;
			}
			u.uhpmax -= tmp/2;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("Nothing happens.");
		    tmp = 0;
		    break;
		}
		break;
	    case AD_FAMN:
		pline("You are being eaten!");
		exercise(A_CON, FALSE);
		if (!is_fainted()) morehungry(rnz(40));
		if (!is_fainted()) morehungry(rnz(40));
		morehungry(tmp);
		morehungry(tmp);
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
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Yuck!");
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		You_feel("something sapping your energy!");
		    docalm();
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		pline("Mysterious waves pulsate through your body!");
		if (rn2(3)) break;
		    struct obj *obj = some_armor(&youmonst);

		    if (obj && drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
		    }
		break;

	    case AD_SHRD:
		pline("You are cut by razor-sharp shards!");
		if (rn2(4)) break;

		struct obj *objX = some_armor(&youmonst);

		if (objX && drain_item(objX)) {
			Your("%s less effective.", aobjnam(objX, "seem"));
		} else if (objX && rn2(3)) wither_dmg(objX, xname(objX), rn2(4), FALSE, &youmonst);

		break;

	    case AD_POLY:
		pline("A weird green light pierces you!");
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
		    polyself(FALSE);
		}
		break;
	    case AD_TCKL:
		pline("A bunch of feelers suddenly reaches out for you!");
		if (multi >= 0 && !rn2(3)) {
		    if (Free_action)
			pline("They don't seem to do all that much, though...");
		    else {
			pline("Wahahahaha! Arrgh! It tickles!");
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10), "tickled by a bunch of feelers");
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
			tmp += rnd(u.ualign.sins + 1);
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
				Sprintf(buf, "%s %s",
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
		    switch (rn2(11)) {
		    case 0: diseasemu(mtmp->data);
			    break;
		    case 1: make_blinded(Blinded + tmp, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    Hallucination ? "trippy" : "confused");
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
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE);
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
		    if (Acid_resistance && rn2(20)) {
			You("are covered with a seemingly harmless goo.");
			tmp = 0;
		    } else {
		      if (Hallucination) pline("Ouch!  You've been slimed!");
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
			    if (!Blind) Your(vision_clears);
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
			if (Shock_resistance && rn2(20)) {
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
			if (Shock_resistance && rn2(20)) {
				shieldeff(u.ux, u.uy);
				You("seem unhurt.");
				ugolemeffects(AD_ELEC,tmp);
				tmp = 0;
			}
		    } else tmp = 0;

		    setustuck(mtmp);
		    pline("%s grabs you!", Monnam(mtmp));
		    display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		    break;

		case AD_AXUS:
		    pline("Snap! Crackle! Pop!");

		    if(!mtmp->mcan && rn2(2)) {
			if (Cold_resistance && rn2(20)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly chilly.");
				ugolemeffects(AD_COLD,tmp);
				if (tmp >= 4) tmp -= (tmp / 4);
			} else You("are freezing to death!");
		    }

		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance && rn2(20)) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly hot.");
				ugolemeffects(AD_FIRE,tmp);
				if (tmp >= 4) tmp -= (tmp / 4);
			} else You("are burning to a crisp!");
			burn_away_slime();
		    }

		    if(!mtmp->mcan && rn2(2)) {
			pline_The("air around you crackles with electricity.");
			if (Shock_resistance && rn2(20)) {
				shieldeff(u.ux, u.uy);
				You("seem unhurt.");
				ugolemeffects(AD_ELEC,tmp);
				if (tmp >= 4) tmp -= (tmp / 4);
			}
		    }

			if (!rn2(3) && (!Drain_resistance || !rn2(4) )  ) {
			    losexp("life drainage", FALSE, TRUE);
			}

		    break;

		case AD_COLD:
		    if(!mtmp->mcan && rn2(2)) {
			if (Cold_resistance && rn2(20)) {
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
				} else {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }

			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    lifesaved++;
			}
		    }

			if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
			else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
			if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
			if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
			exercise(A_WIS, FALSE);
		    } else tmp = 0;
		    break;
		case AD_FIRE:
		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance && rn2(20)) {
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
		case AD_MAGM:
		    if(Antimagic && rn2(5)) {
			tmp = 0;
		    } else {
			You("are irradiated with energy!");
		    }
		    break;
		case AD_DISN:
		    You_feel("like a drill is tearing you apart!");
		if (!rn2(10))  {
		if (Disint_resistance && rn2(100)) {
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

	    done(DIED);
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

	mdamageu(mtmp, tmp);
	if (tmp) stop_occupation();

	if (touch_petrifies(youmonst.data) && !resists_ston(mtmp)) {
	    pline("%s very hurriedly %s you!", Monnam(mtmp),
		  is_animal(mtmp->data)? "regurgitates" : "expels");
	    expels(mtmp, mtmp->data, FALSE);
	} else if (!u.uswldtim || youmonst.data->msize >= MZ_HUGE) {
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
	register boolean not_affected = defends((int)mattk->adtyp, uwep);

	hitmsg(mtmp, mattk);

	switch (mattk->adtyp) {
	    case AD_CONF:
		    if (mon_visible(mtmp) || (rnd(tmp /= 2) > u.ulevel)) {
			if (!Hallucination) You("are confused by a blast of light!");
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
		if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		burn_away_slime();
		not_affected |= Fire_resistance;
		goto common;
	    case AD_PLAS:
		pline("You're seared by %s hot plasma radiation!", Fire_resistance ? "very" : "extremely");
		if (!Fire_resistance) tmp *= 2;
		if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		burn_away_slime();
		make_stunned(HStun + tmp, TRUE);
		goto common;

	    case AD_MALK:
		setustuck(mtmp);
		pline("%s grabs you!", Monnam(mtmp));
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		if (!rn2(issoviet ? 6 : 33))
			destroy_item(WAND_CLASS, AD_ELEC);
		if (!rn2(issoviet ? 6 : 33))
			destroy_item(RING_CLASS, AD_ELEC);
		not_affected |= Shock_resistance;
		goto common;

	    case AD_AXUS:
		if (!Drain_resistance || !rn2(4) )
		    losexp("draining explosion", FALSE, TRUE);

		goto common;

	    case AD_STCK:
	    case AD_WRAP:
	    case AD_WERE:
	    case AD_SGLD:
	    case AD_SITM:
	    case AD_SEDU:
	    case AD_SSEX:
	    case AD_SAMU:
	    case AD_THIR:

		goto common;

	    case AD_ELEC:
		not_affected |= Shock_resistance;
	    case AD_PHYS: /* just do some plain physical damage, for golem's selfdestruct attack et. al. --Amy */
common:

		if (!not_affected || !rn2(10) ) {
		    if (ACURR(A_DEX) > rnd(200)) {
			You("duck some of the blast.");
			tmp = (tmp+1) / 2;
		    } else {
		        if (flags.verbose) You("get blasted!");
		    }
		    if (mattk->adtyp == AD_FIRE) burn_away_slime();
		    if (Half_physical_damage && rn2(2) ) tmp = (tmp+1) / 2;
		    mdamageu(mtmp, tmp);
		}
		break;

	    case AD_SLEE:
		if (multi >= 0) {
			if (Sleep_resistance && rn2(20)) break;
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

	    case AD_WEBS: 

		{
			struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB, 0);
			if (ttmp2) {
				pline_The("webbing sticks to you. You're caught!");
				dotrap(ttmp2, NOWEBMSG);
				if (u.usteed && u.utrap) {
				/* you, not steed, are trapped */
				dismount_steed(DISMOUNT_FELL);
				}
			}
		}
		/* Amy addition: sometimes, also make a random trap somewhere on the level :D */
		if (issoviet || !rn2(2)) makerandomtrap();

		mdamageu(mtmp, tmp);

		break;

	    case AD_ENCH:
		{
			struct obj *obj = some_armor(&youmonst);

			if (obj && drain_item(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
			}
		}
		break;

	    case AD_STTP:

		pline("You are surrounded by a purple glow!");
		if (invent) {
		    int itemportchance = 10 + rn2(21);
		    for (otmpi = invent; otmpi; otmpi = otmpii) {

		      otmpii = otmpi->nobj;

			if (!rn2(itemportchance) && !stack_too_big(otmpi) ) {

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
			}

		    }
		}
		break;

	    case AD_ICUR:

		You_feel("as if you need some help.");
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

		if (!u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.levelporting) {
				u.levelporting = 1;
				nomul(-2, "being levelported"); /* because it's not called until you get another turn... */
			}

		}
		else if (!Drain_resistance || !rn2(4) ) {
		    losexp("loss of potential", FALSE, TRUE);
		}
		mdamageu(mtmp, tmp);
		break;

	    case AD_BANI:
		if (u.uevent.udemigod || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed)) ) { pline("You shudder for a moment."); (void) safe_teleds(FALSE); break;}
		if (flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) {
			pline("For some reason you resist the banishment!"); break;}

		make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

		if (!u.banishmentbeam) {
			u.banishmentbeam = 1;
			nomul(-2, "being banished"); /* because it's not called until you get another turn... */
		}
		break;

	    case AD_DISP:
		mdamageu(mtmp, tmp);
		pushplayer();
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
		    if (!Antimagic) {
			killer_format = KILLED_BY_AN;
			killer = "explosion of death";
			done(DIED);
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
		if (!rn2(50)) skillcaploss();
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
						if( rnd(40) > ACURR(A_STR)){
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

	    case AD_HEAL:
		if(!uwep
		   && !uarmu
		   && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    if (Upolyd) {
			u.mh += rnd(7);
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10)) {
				u.uhpmax++;
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

	      switch (rn2(11)) {
		    case 0: diseasemu(mdat);
			    break;
		    case 1: make_blinded(Blinded + tmp, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    Hallucination ? "trippy" : "confused");
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
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + tmp, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + tmp, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + tmp, TRUE);
			    break;
		}

		break;

	    case AD_SLOW:

		if (HFast && !defends(AD_SLOW, uwep))
		    u_slow_down();
		break;
	    case AD_SHRD:
		{
			struct obj *obj = some_armor(&youmonst);

			if (obj && drain_item(obj)) {
				Your("%s less effective.", aobjnam(obj, "seem"));
			} else if (obj) wither_dmg(obj, xname(obj), rn2(4), FALSE, &youmonst);
		}
		mdamageu(mtmp, tmp);

		break;

	    case AD_NPRO:
		u.negativeprotection++;
		You_feel("less protected!");
		mdamageu(mtmp, tmp);
		break;

	    case AD_CHKH:

		tmp += u.chokhmahdamage;
		tmp += rnd(u.ualign.sins + 1);
		u.chokhmahdamage++;
		mdamageu(mtmp, tmp);
		break;

	    case AD_DEPR:

		    switch(rnd(20)) {
		    case 1:
			if (!Unchanging && !Antimagic) {
				You("undergo a freakish metamorphosis!");
			      polyself(FALSE);
			}
			break;
		    case 2:
			You("need reboot.");
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
			} else {
				if (multi >= 0) {
				    if (Sleep_resistance && rn2(20)) break;
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
			    if (Free_action) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by a monster explosion");
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 14:
			if (Hallucination)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
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

	    case AD_FUMB:

		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(10 * (tmp + 1) );

		break;

	    case AD_NAST:

		pline("Nasty!");

			switch (rnd(85)) {

				case 1: RMBLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 2: NoDropProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 3: DSTWProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 4: StatusTrapProblem += rnz( (tmp + 2) * rnd(100) ); 
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
				case 5: Superscroller += rnz( (tmp + 2) * rnd(100) * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz( (tmp + 2) * rnd(100) ); break;
				case 7: FreeHandLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 8: Unidentify += rnz( (tmp + 2) * rnd(100) ); break;
				case 9: Thirst += rnz( (tmp + 2) * rnd(100) ); break;
				case 10: LuckLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 11: ShadesOfGrey += rnz( (tmp + 2) * rnd(100) ); break;
				case 12: FaintActive += rnz( (tmp + 2) * rnd(100) ); break;
				case 13: Itemcursing += rnz( (tmp + 2) * rnd(100) ); break;
				case 14: DifficultyIncreased += rnz( (tmp + 2) * rnd(100) ); break;
				case 15: Deafness += rnz( (tmp + 2) * rnd(100) ); flags.soundok = 0; break;
				case 16: CasterProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 17: WeaknessProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 18: RotThirteen += rnz( (tmp + 2) * rnd(100) ); break;
				case 19: BishopGridbug += rnz( (tmp + 2) * rnd(100) ); break;
				case 20: UninformationProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 21: StairsProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 22: AlignmentProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 23: ConfusionProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 24: SpeedBug += rnz( (tmp + 2) * rnd(100) ); break;
				case 25: DisplayLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 26: SpellLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 27: YellowSpells += rnz( (tmp + 2) * rnd(100) ); break;
				case 28: AutoDestruct += rnz( (tmp + 2) * rnd(100) ); break;
				case 29: MemoryLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 30: InventoryLoss += rnz( (tmp + 2) * rnd(100) ); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (tmp * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 33: BloodLossProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 34: BadEffectProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 35: TrapCreationProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 36: AutomaticVulnerabilitiy += rnz( (tmp + 2) * rnd(100) ); break;
				case 37: TeleportingItems += rnz( (tmp + 2) * rnd(100) ); break;
				case 38: NastinessProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 39: CaptchaProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 40: FarlookProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 41: RespawnProblem += rnz( (tmp + 2) * rnd(100) ); break;
				case 42: RecurringAmnesia += rnz( (tmp + 2) * rnd(100) ); break;
				case 43: BigscriptEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 44: {
					BankTrapEffect += rnz( (tmp + 2) * rnd(100) );
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 46: TechTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 47: RecurringDisenchant += rnz( (tmp + 2) * rnd(100) ); break;
				case 48: verisiertEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 49: ChaosTerrain += rnz( (tmp + 2) * rnd(100) ); break;
				case 50: Muteness += rnz( (tmp + 2) * rnd(100) ); break;
				case 51: EngravingDoesntWork += rnz( (tmp + 2) * rnd(100) ); break;
				case 52: MagicDeviceEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 53: BookTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 54: LevelTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 55: QuizTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 56: FastMetabolismEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 57: NoReturnEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 58: AlwaysEgotypeMonsters += rnz( (tmp + 2) * rnd(100) ); break;
				case 59: TimeGoesByFaster += rnz( (tmp + 2) * rnd(100) ); break;
				case 60: FoodIsAlwaysRotten += rnz( (tmp + 2) * rnd(100) ); break;
				case 61: AllSkillsUnskilled += rnz( (tmp + 2) * rnd(100) ); break;
				case 62: AllStatsAreLower += rnz( (tmp + 2) * rnd(100) ); break;
				case 63: PlayerCannotTrainSkills += rnz( (tmp + 2) * rnd(100) ); break;
				case 64: PlayerCannotExerciseStats += rnz( (tmp + 2) * rnd(100) ); break;
				case 65: TurnLimitation += rnz( (tmp + 2) * rnd(100) ); break;
				case 66: WeakSight += rnz( (tmp + 2) * rnd(100) ); break;
				case 67: RandomMessages += rnz( (tmp + 2) * rnd(100) ); break;

				case 68: Desecration += rnz( (tmp + 2) * rnd(100) ); break;
				case 69: StarvationEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 70: NoDropsEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 71: LowEffects += rnz( (tmp + 2) * rnd(100) ); break;
				case 72: InvisibleTrapsEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 73: GhostWorld += rnz( (tmp + 2) * rnd(100) ); break;
				case 74: Dehydration += rnz( (tmp + 2) * rnd(100) ); break;
				case 75: HateTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 76: TotterTrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 77: Nonintrinsics += rnz( (tmp + 2) * rnd(100) ); break;
				case 78: Dropcurses += rnz( (tmp + 2) * rnd(100) ); break;
				case 79: Nakedness += rnz( (tmp + 2) * rnd(100) ); break;
				case 80: Antileveling += rnz( (tmp + 2) * rnd(100) ); break;
				case 81: ItemStealingEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 82: Rebellions += rnz( (tmp + 2) * rnd(100) ); break;
				case 83: CrapEffect += rnz( (tmp + 2) * rnd(100) ); break;
				case 84: ProjectilesMisfire += rnz( (tmp + 2) * rnd(100) ); break;
				case 85: WallTrapping += rnz( (tmp + 2) * rnd(100) ); break;
			}

		break;

	    case AD_VULN:

		 switch (rnd(123)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				u.uprops[DEAC_FIRE_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having fire resistance!");
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				u.uprops[DEAC_COLD_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having cold resistance!");
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				u.uprops[DEAC_SLEEP_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having sleep resistance!");
				break;
			case 16:
			case 17:
				u.uprops[DEAC_DISINT_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having disintegration resistance!");
				break;
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
				u.uprops[DEAC_SHOCK_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having shock resistance!");
				break;
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				u.uprops[DEAC_POISON_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having poison resistance!");
				break;
			case 28:
			case 29:
			case 30:
				u.uprops[DEAC_DRAIN_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having drain resistance!");
				break;
			case 31:
			case 32:
				u.uprops[DEAC_SICK_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having sickness resistance!");
				break;
			case 33:
			case 34:
				u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having magic resistance!");
				break;
			case 35:
			case 36:
			case 37:
			case 38:
				u.uprops[DEAC_ACID_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having acid resistance!");
				break;
			case 39:
			case 40:
				u.uprops[DEAC_STONE_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having petrification resistance!");
				break;
			case 41:
				u.uprops[DEAC_FEAR_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having fear resistance!");
				break;
			case 42:
			case 43:
			case 44:
				u.uprops[DEAC_SEE_INVIS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having see invisible!");
				break;
			case 45:
			case 46:
			case 47:
				u.uprops[DEAC_TELEPAT].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having telepathy!");
				break;
			case 48:
			case 49:
			case 50:
				u.uprops[DEAC_WARNING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having warning!");
				break;
			case 51:
			case 52:
			case 53:
				u.uprops[DEAC_SEARCHING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having automatic searching!");
				break;
			case 54:
				u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having clairvoyance!");
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
				u.uprops[DEAC_INFRAVISION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having infravision!");
				break;
			case 60:
				u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having detect monsters!");
				break;
			case 61:
			case 62:
			case 63:
				u.uprops[DEAC_INVIS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having invisibility!");
				break;
			case 64:
				u.uprops[DEAC_DISPLACED].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having displacement!");
				break;
			case 65:
			case 66:
			case 67:
				u.uprops[DEAC_STEALTH].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having stealth!");
				break;
			case 68:
				u.uprops[DEAC_JUMPING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having jumping!");
				break;
			case 69:
			case 70:
			case 71:
				u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having teleport control!");
				break;
			case 72:
				u.uprops[DEAC_FLYING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having flying!");
				break;
			case 73:
				u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having magical breathing!");
				break;
			case 74:
				u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having phasing!");
				break;
			case 75:
			case 76:
				u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having slow digestion!");
				break;
			case 77:
				u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having half spell damage!");
				break;
			case 78:
				u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having half physical damage!");
				break;
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
				u.uprops[DEAC_REGENERATION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having regeneration!");
				break;
			case 84:
			case 85:
				u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having mana regeneration!");
				break;
			case 86:
			case 87:
			case 88:
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having polymorph control!");
				break;
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
				u.uprops[DEAC_FAST].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having speed!");
				break;
			case 94:
			case 95:
			case 96:
				u.uprops[DEAC_REFLECTING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having reflection!");
				break;
			case 97:
			case 98:
			case 99:
				u.uprops[DEAC_FREE_ACTION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having free action!");
				break;
			case 100:
				u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from hallu partying!");
				break;
			case 101:
				u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from drunken boxing!");
				break;
			case 102:
				u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having stunnopathy!");
				break;
			case 103:
				u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having numbopathy!");
				break;
			case 104:
				u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having freezopathy!");
				break;
			case 105:
				u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from being a stoned chiller!");
				break;
			case 106:
				u.uprops[DEAC_CORROSIVITY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having corrosivity!");
				break;
			case 107:
				u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having an increased fear factor!");
				break;
			case 108:
				u.uprops[DEAC_BURNOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having burnopathy!");
				break;
			case 109:
				u.uprops[DEAC_SICKOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having sickopathy!");
				break;
			case 110:
				u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having keen memory!");
				break;
			case 111:
				u.uprops[DEAC_THE_FORCE].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from using the force like a real jedi!");
				break;
			case 112:
				u.uprops[DEAC_SIGHT_BONUS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having extra sight!");
				break;
			case 113:
				u.uprops[DEAC_VERSUS_CURSES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having curse resistance!");
				break;
			case 114:
				u.uprops[DEAC_STUN_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having stun resistance!");
				break;
			case 115:
				u.uprops[DEAC_CONF_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having confusion resistance!");
				break;
			case 116:
				u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having double attacks!");
				break;
			case 117:
				u.uprops[DEAC_QUAD_ATTACK].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having quad attacks!");
				break;
			case 118:
				u.uprops[DEAC_PSI_RES].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having psi resistance!");
				break;
			case 119:
				u.uprops[DEAC_WONDERLEGS].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having wonderlegs!");
				break;
			case 120:
				u.uprops[DEAC_GLIB_COMBAT].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having glib combat!");
				break;
			case 121:
				u.uprops[DEAC_MANALEECH].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having manaleech!");
				break;
			case 122:
				u.uprops[DEAC_DIMMOPATHY].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having dimmopathy!");
				break;
			case 123:
				u.uprops[DEAC_PEACEVISION].intrinsic += rnz( (tmp * rnd(30) ) + 1);
				pline("You are prevented from having peacevision!");
				break;
		}

		break;

	    case AD_BLND:
		not_affected = resists_blnd(&youmonst);
		if (!not_affected) {
		    /* sometimes you're affected even if it's invisible */
		    if (mon_visible(mtmp) || (rnd(tmp /= 2) > u.ulevel)) {
			You("are blinded by a blast of light!");
			make_blinded((long)tmp, FALSE);
			if (!Blind) Your(vision_clears);
		    } else if (flags.verbose)
			You("get the impression it was not terribly bright.");
		}
		break;

	    case AD_TIME: /* timebomb */
		switch (rnd(10)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				You_feel("life has clocked back.");
			      losexp("time", FALSE, FALSE); /* resistance is futile :D */
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

		if (Disint_resistance && rn2(100)) {
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

	      done(DIED);

		break;

	    case AD_STON: /* mainly for jonadab's stoning sphere */

			if (!Stoned && !Stone_resistance && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) 
			{
				Stoned = 7;
				delayed_killer = mtmp->data->mname;
				if (mtmp->data->geno & G_UNIQ) {
				    if (!type_is_pname(mtmp->data)) {
					static char kbuf[BUFSZ];

					/* "the" buffer may be reallocated */
					Strcpy(kbuf, the(delayed_killer));
					delayed_killer = kbuf;
				    }
				    killer_format = KILLED_BY;
				} else killer_format = KILLED_BY_AN;
			}

		break;

	    case AD_PLYS: /* for jonadab's paralyzing sphere */
	    case AD_TCKL:

		    if (Free_action) {
			You("momentarily stiffen.");            
		    } else {
			You("suddenly recognize an inability to move!");
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-tmp, "paralyzed by an explosion");
			exercise(A_DEX, FALSE);
		    }

		break;

	    case AD_RUST:

		water_damage(invent, FALSE, FALSE);

		break;

	    case AD_WET:

		pline("Water washes over you!");
		water_damage(invent, FALSE, FALSE);
		if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
		break;

	    case AD_LETH:

		pline("Sparkling water washes over you!");
		lethe_damage(invent, FALSE, FALSE);
		if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
		if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		break;

	    case AD_CNCL:

		(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
		break;

	    case AD_LUCK:
		change_luck(-1);
		You_feel("unlucky.");
		break;

	    case AD_FAKE:
		pline(fauxmessage());
		if (!rn2(3)) pline(fauxmessage());
	      mdamageu(mtmp, tmp);
		break;

	    case AD_NEXU:
		if (level.flags.noteleport || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= (1 + rnd(2));

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

				if (!u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported"); /* because it's not called until you get another turn... */
					}
				}
				break;
			case 7:
				{
					int firststat = rn2(A_MAX);
					int secondstat = rn2(A_MAX);
					int firstswapstat = ABASE(firststat);
					int secondswapstat = ABASE(secondstat);
					int difference = (firstswapstat - secondswapstat);
					ABASE(secondstat) += difference;
					ABASE(firststat) -= difference;
					AMAX(secondstat) = ABASE(secondstat);
					AMAX(firststat) = ABASE(firststat);
					pline("Your stats got scrambled!");
				}
				break;
		}
	      mdamageu(mtmp, tmp);
		break;

	    case AD_SOUN:

		pline("Your ears are blasted by hellish noise!");
		if (Deafness || (uwep && uwep->oartifact == ART_MEMETAL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_MEMETAL) || (uwep && uwep->oartifact == ART_BANG_BANG) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_BANG_BANG) || u.uprops[DEAFNESS].extrinsic || have_deafnessstone() ) tmp /= 2;
		make_stunned(HStun + tmp, TRUE);
		if (issoviet || !rn2(2)) (void)destroy_item(POTION_CLASS, AD_COLD);
		wake_nearby();
	      mdamageu(mtmp, tmp);
		break;

	    case AD_GRAV:

		if (level.flags.noteleport || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) tmp *= 2;

		pline("Gravity warps around you...");
		phase_door(0);
		pushplayer();
		u.uprops[DEAC_FAST].intrinsic += (tmp + 2);
		make_stunned(HStun + tmp, TRUE);
	      mdamageu(mtmp, tmp);
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

		if (!Drain_resistance || !rn2(4) )
		    losexp("draining explosion", FALSE, TRUE);
		break;

	    case AD_HODS:
		 if(uwep) {
			int artifactvar = tmp; /* to fix a weird compiler warning for artifact_hit() below --Amy */
			if (uwep->otyp == CORPSE
				&& touch_petrifies(&mons[uwep->corpsenm])) {
			    tmp = 1;
				pline("You touch the petrifying corpse...");
			    if (!Stoned && !Stone_resistance) {
				    Stoned = 7;
				    killer_format = KILLED_BY;
				    delayed_killer = "their own petrifying corpse";
			    }
			}
			tmp += dmgval(uwep, &youmonst);
			
			if (uwep->opoisoned){
				Sprintf(buf, "%s %s",
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

		losexp("draining explosion", FALSE, TRUE);
	      mdamageu(mtmp, tmp);
		break;

	    case AD_DRST:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", A_STR, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DISE:
		diseasemu(mdat);
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
			    if (Sleep_resistance && rn2(20)) {pline("You yawn."); break;}
			    fall_asleep(-rnd(10), TRUE);
				pline("You fall asleep!");
			}
			break;
		    case 2: /* paralyse */
			if (multi >= 0) {
			    if (Free_action) {
				You("momentarily stiffen.");            
			    } else {
				pline("You can#t move!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by an explosion");
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 3: /* slow */
			if(HFast)  u_slow_down();
			else You("pause momentarily.");
			break;
		    case 4: /* drain Dex */
			adjattrib(A_DEX, -rn1(1,1), 0);
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
			polyself(FALSE);
		}
		break;

	    case AD_CALM:
		docalm();
		break;

	    case AD_DRDX:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", A_DEX, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DRCO:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", A_CON, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_WISD:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", A_WIS, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DRCH:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", A_CHA, "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_POIS:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", rn2(A_MAX), "poison explosion", 30);
		}
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_VENO:

		if (!Poison_resistance || !rn2(5) ) {
		poisoned("blast", rn2(A_MAX), "venom explosion", 5);
		}
		if (!Poison_resistance) pline("You're badly poisoned!");
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE);
		if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE);
	      mdamageu(mtmp, tmp); /* still does damage even if you resist the poison --Amy */
		break;

	    case AD_DFOO:
		pline("The explosion takes you down a peg or two.");
		if (rn2(3)) {
		    Sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, rn2(A_MAX), mdat->mname, 30);
		}
		if (!rn2(2)) {
			You_feel("drained...");
			u.uhpmax -= rn1(10,10);
			if (u.uhpmax < 0) u.uhpmax = 0;
			if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (!rn2(2)) {
			You_feel("less energised!");
			u.uenmax -= rn1(10,10);
			if (u.uenmax < 0) u.uenmax = 0;
			if(u.uen > u.uenmax) u.uen = u.uenmax;
		}
		if (!rn2(2)) {
			if(!Drain_resistance || !rn2(4) )
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

		if (uarmh && rn2(3)) {
		    /* not body_part(HEAD) */
		    Your("helmet blocks the attack to your head.");
		    break;
		}
		
		mdamageu(mtmp, tmp);

		if (!uarmh || uarmh->otyp != DUNCE_CAP) {
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
				} else {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }

			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    lifesaved++;
			}
		    }
		}
		/* adjattrib gives dunce cap message when appropriate */
		if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
		else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
		if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of levels */
		if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));	/* lose memory of 25% of objects */
		exercise(A_WIS, FALSE);
		break;

	    case AD_WTHR:

		withering_damage(invent, FALSE, FALSE);

		break;

	    case AD_SPC2:

		if (!Psi_resist || !rn2(20)) {

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
			if (!rn2(200)) {
				losexp("psionic drain", FALSE, TRUE);
			}
			if (!rn2(200)) {
				adjattrib(A_INT, -1, 1);
				adjattrib(A_WIS, -1, 1);
			}
			if (!rn2(200)) {
				pline("You scream in pain!");
				wake_nearby();
			}
			if (!rn2(200)) {
				badeffect();
			}

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
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimy explosion";
		} else
		    pline("Yuck!");

		break;

	    case AD_STUN:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are stunned by a blast of light!");
			else pline("%s NUMBED CAN'T DO",urole.name.m);
			make_stunned(HStun + (long)tmp, FALSE);
		}
		break;

	    case AD_NUMB:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are numbed by a blast of light!");
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
		Sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		poisoned(buf, A_CON, mdat->mname, 60);
		if(Poison_resistance) wdmg -= ACURR(A_CON)/2;
		if(wdmg > 0){
		
			while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
				wdmg--;
				(void) adjattrib(A_WIS, -1, TRUE);
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
		pline("Everything gets dark!");
		litroomlite(FALSE);
	      mdamageu(mtmp, tmp);

		break;

	    case AD_FRZE:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are frozen solid by a blast of light!");
			else pline("%s COOLING",urole.name.m);
			make_frozen(HFrozen + (long)tmp, FALSE);
		}
		break;

	    case AD_BURN:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are burned by a blast of light!");
			else pline("%s BURNED",urole.name.m);
			make_burned(HBurned + (long)tmp, FALSE);
		}
		break;

	    case AD_DIMN:
		not_affected = (Blind && !rn2(3));
		if (!not_affected) {
			if (!Hallucination) You("are dimmed by a blast of light!");
			else pline("%s DISPIRITED",urole.name.m);
			make_dimmed(HDimmed + (long)tmp, FALSE);
		}
		break;

	    case AD_FEAR:
		not_affected = (Blind && rn2(2));
		if (!not_affected) {
			if (!Hallucination) You("are stricken with fear by a blast of light!");
			else pline("%s FEARED",urole.name.m);
			make_feared(HFeared + (long)tmp, FALSE);
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
    mondead(mtmp);
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
	uchar atttypB;

	/* Monsters with AD_RBRE can use any random gaze. --Amy */

	atttypB = mattk->adtyp;

	if (atttypB == AD_RBRE) {
		while (atttypB == AD_ENDS ||atttypB == AD_RBRE || atttypB == AD_SPC2 || atttypB == AD_WERE) {
			atttypB = randattack(); }
		/*randattack = 1;*/
	}

	if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "mirrored gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "zerkal'nyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "akslantirish qo'lqop") ) && !rn2(3) && !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee ) {
		/* cut down on message spam - only display it 1 out of 10 times --Amy */
		if (!rn2(10)) pline("%s gazes at you, but your mirrored gloves protect you from the effects!", Monnam(mtmp));
		return 0;
	}

	dmgplus = d((int)mattk->damn, (int)mattk->damd);	/* why the heck did gaze attacks have fixed damage??? --Amy */

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
		    !Stone_resistance && !rn2(15) ) {
		    You("meet %s gaze.", s_suffix(mon_nam(mtmp)));
		    stop_occupation();
		    if(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			break;
		    /*You("turn to stone...");
		    killer_format = KILLED_BY;
		    killer = mtmp->data->mname;
		    done(STONING);*/
		    You("start turning to stone...");
			if (!Stoned) Stoned = 7; delayed_killer = "petrifying gaze";
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
			    attrcurse();
			}
		}
		break;

	    case AD_SPC2:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(7)) )
 		{
			char visageword[BUFSZ]; /* from ToME */
			Strcpy(visageword, "bad"); /* fail safe --Amy */

			if (!Hallucination) switch(rnd(20)) {

				case 1:
					Strcpy(visageword, "abominable");
					break;
				case 2:
					Strcpy(visageword, "abysmal");
					break;
				case 3:
					Strcpy(visageword, "appalling");
					break;
				case 4:
					Strcpy(visageword, "baleful");
					break;
				case 5:
					Strcpy(visageword, "blasphemous");
					break;
				case 6:
					Strcpy(visageword, "disgusting");
					break;
				case 7:
					Strcpy(visageword, "dreadful");
					break;
				case 8:
					Strcpy(visageword, "filthy");
					break;
				case 9:
					Strcpy(visageword, "grisly");
					break;
				case 10:
					Strcpy(visageword, "hideous");
					break;
				case 11:
					Strcpy(visageword, "hellish");
					break;
				case 12:
					Strcpy(visageword, "horrible");
					break;
				case 13:
					Strcpy(visageword, "infernal");
					break;
				case 14:
					Strcpy(visageword, "loathsome");
					break;
				case 15:
					Strcpy(visageword, "nightmarish");
					break;
				case 16:
					Strcpy(visageword, "repulsive");
					break;
				case 17:
					Strcpy(visageword, "sacrilegious");
					break;
				case 18:
					Strcpy(visageword, "terrible");
					break;
				case 19:
					Strcpy(visageword, "unclean");
					break;
				case 20:
					Strcpy(visageword, "unspeakable");
					break;

			} else switch(rnd(22)) {

				case 1:
					Strcpy(visageword, "silly");
					break;
				case 2:
					Strcpy(visageword, "hilarious");
					break;
				case 3:
					Strcpy(visageword, "absurd");
					break;
				case 4:
					Strcpy(visageword, "insipid");
					break;
				case 5:
					Strcpy(visageword, "ridiculous");
					break;
				case 6:
					Strcpy(visageword, "laughable");
					break;
				case 7:
					Strcpy(visageword, "ludicrous");
					break;
				case 8:
					Strcpy(visageword, "far-out");
					break;
				case 9:
					Strcpy(visageword, "groovy");
					break;
				case 10:
					Strcpy(visageword, "postmodern");
					break;
				case 11:
					Strcpy(visageword, "fantastic");
					break;
				case 12:
					Strcpy(visageword, "dadaistic");
					break;
				case 13:
					Strcpy(visageword, "cubistic");
					break;
				case 14:
					Strcpy(visageword, "cosmic");
					break;
				case 15:
					Strcpy(visageword, "awesome");
					break;
				case 16:
					Strcpy(visageword, "incomprehensible");
					break;
				case 17:
					Strcpy(visageword, "fabulous");
					break;
				case 18:
					Strcpy(visageword, "amazing");
					break;
				case 19:
					Strcpy(visageword, "incredible");
					break;
				case 20:
					Strcpy(visageword, "chaotic");
					break;
				case 21:
					Strcpy(visageword, "wild");
					break;
				case 22:
					Strcpy(visageword, "preposterous");
					break;

			}

		pline("You behold the %s visage of %s!", visageword, mon_nam(mtmp));
		if (Hallucination && rn2(2) ) switch (rnd(5)) {

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
		if (Psi_resist && rn2(20) ) break;

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
			if (!rn2(200)) {
				losexp("psionic drain", FALSE, TRUE);
			}
			if (!rn2(200)) {
				adjattrib(A_INT, -1, 1);
				adjattrib(A_WIS, -1, 1);
			}
			if (!rn2(200)) {
				pline("You scream in pain!");
				wake_nearby();
			}
			if (!rn2(200)) {
				badeffect();
			}

		}
		break;

	    case AD_MAGM:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
 		{
		pline("%s's eye color suddenly changes!", Monnam(mtmp));
		    stop_occupation();
		    if(Antimagic && !rn2(3)) {
			shieldeff(u.ux, u.uy);
			pline("A hail of magic missiles narrowly misses you!");
		    } else {
			You("are hit by magic missiles appearing from thin air!");
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
		if (Disint_resistance && rn2(100)) {
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

	    done(DIED);
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
		    if (Acid_resistance && rn2(20)) {
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
			if (!rn2(issoviet ? 3 : 7) && (!Drain_resistance || !rn2(4) )  ) {
				pline("%s seems to drain your life with its gaze!", Monnam(mtmp));
		    stop_occupation();
			    losexp("life drainage", FALSE, TRUE);
				if (!rn2(4)) mdamageu(mtmp, dmgplus);
			}
		}
		break;

	      case AD_VAMP:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
 		{
			if (!rn2(issoviet ? 2 : 3) && (!Drain_resistance || !rn2(4) )  ) {
				pline("%s seems to drain your life with its gaze!", Monnam(mtmp));
		    stop_occupation();
			    losexp("life drainage", FALSE, TRUE);
				if (!rn2(2)) mdamageu(mtmp, dmgplus);
			}
		}
		break;

	    case AD_STCK:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || rn2(5)) )
		{ 
			if (!u.ustuck && !sticks(youmonst.data)) {
				setustuck(mtmp);
				pline("%s gazes to hold you in place!", Monnam(mtmp));
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
		}
		break;

	    case AD_WEBS:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(3)) )
 		{
			pline("%s asks 'How do I shot web?' and spits at you.", Monnam(mtmp));
		    stop_occupation();
		{
			struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB, 0);
			if (ttmp2) {
				pline_The("webbing sticks to you. You're caught!");
				dotrap(ttmp2, NOWEBMSG);
				if (u.usteed && u.utrap) {
				/* you, not steed, are trapped */
				dismount_steed(DISMOUNT_FELL);
				}
			}
		}
		/* Amy addition: sometimes, also make a random trap somewhere on the level :D */
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap();

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

			if (!rn2(itemportchance) && !stack_too_big(otmpi) ) {

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

		/* create darkness around the player --Amy */
		pline("%s's sinister gaze fills your mind with dreadful, evil thoughts!", Monnam(mtmp));
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
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Yuck!");
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
		} else if (obj && rn2(3)) wither_dmg(obj, xname(obj), rn2(4), FALSE, &youmonst);
		}

            if (!rn2(3)) mdamageu(mtmp, (1 + dmgplus));

		break;

	    case AD_POLY:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(20)) )
 		{
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			pline("%s throws a changing gaze at you!", Monnam(mtmp));
		    stop_occupation();
		    polyself(FALSE);
			}
		}
		break;

	    case AD_FAKE:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee)
 		{
			pline(fauxmessage());
			if (!rn2(3)) pline(fauxmessage());
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
			      polyself(FALSE);
			}
			break;
		    case 2:
			You("need reboot.");
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
			} else {
				if (multi >= 0) {
				    if (Sleep_resistance && rn2(20)) break;
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
				display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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
			    if (Free_action) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by a monster attack");
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 14:
			if (Hallucination)
				pline("What a groovy feeling!");
			else
				You(Blind ? "%s and get dizzy..." :
					 "%s and your vision blurs...",
					    stagger(youmonst.data, "stagger"));
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
			    if (Sleep_resistance && rn2(20)) {pline("You yawn."); break;}
			    fall_asleep(-rnd(10), TRUE);
			    if (Blind) You("are put to sleep!");
			    else You("are put to sleep by %s!", mon_nam(mtmp));
			}
			break;
		    case 2: /* paralyse */
			if (multi >= 0) {
			    if (Free_action) {
				You("momentarily stiffen.");            
			    } else {
				if (Blind) You("are frozen!");
				else You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by a monster attack");
				exercise(A_DEX, FALSE);
			    }
			}
			break;
		    case 3: /* slow */
			if(HFast)  u_slow_down();
			else You("pause momentarily.");
			break;
		    case 4: /* drain Dex */
			adjattrib(A_DEX, -rn1(1,1), 0);
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
		    Sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, rn2(A_MAX), mtmp->data->mname, 30);
		}
		if (!rn2(4)) {
			You_feel("drained...");
			u.uhpmax -= rn1(10,10);
			if (u.uhpmax < 0) u.uhpmax = 0;
			if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (!rn2(4)) {
			You_feel("less energised!");
			u.uenmax -= rn1(10,10);
			if (u.uenmax < 0) u.uenmax = 0;
			if(u.uen > u.uenmax) u.uen = u.uenmax;
		}
		if (!rn2(4)) {
			if(!Drain_resistance || !rn2(4) )
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
				switch (steal(mtmp, buf)) {
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
		if (!rn2(3) && !issoviet && !(u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stealerstone() ) && canseemon(mtmp) && mtmp->mcansee ) break; /* no message, we don't want too much spam --Amy */

		if (u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stealerstone() ) {
		pline("%s gazes at you with its demanding eyes!", Monnam(mtmp));
		    stop_occupation();
		buf[0] = '\0';
			switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp) && (issoviet || !rn2(4)) )
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, rnd(10), FALSE, FALSE);
			return 3;
			};

		} else if (issoviet || ( (rnd(100) > ACURR(A_CHA)) &&  !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && /*!rn2(25)*/ 
		( ((mtmp->female) && !flags.female && !rn2(5) ) || ((!mtmp->female) && flags.female && !rn2(15) ) || 
			((mtmp->female) && flags.female && !rn2(25) ) || ((!mtmp->female) && !flags.female && !rn2(25) ) ) )

		) 		{
		pline("%s gazes at you with its demanding eyes!", Monnam(mtmp));
		    stop_occupation();
		buf[0] = '\0';
			switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp) && (issoviet || !rn2(4)) )
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

	    case AD_LETH:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(50))) 		{
		pline("%s squirts sparkling water at you!", Monnam(mtmp));
		    stop_occupation();
		if (!rn2(3)) {
			pline("You sparkle!");
			lethe_damage(invent, FALSE, FALSE);
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
			water_damage(invent, FALSE, FALSE);
			if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);
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
						if( rnd(40) > ACURR(A_STR)){
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
			if (u.uevent.udemigod || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds(FALSE); break;}
			if (flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) {
			 pline("For some reason you resist the banishment!"); break;}

			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.banishmentbeam) {
				u.banishmentbeam = 1;
				nomul(-2, "being banished"); /* because it's not called until you get another turn... */
			}
		}
		}
		break;

	    case AD_WEEP:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(40))) 		{
		/* if vampire biting (and also a pet) */
		if (!rn2(3) && !u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) ) {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
			if (!u.levelporting) {
				u.levelporting = 1;
				nomul(-2, "being levelported"); /* because it's not called until you get another turn... */
			}
		}
		else if (!rn2(3) && (!Drain_resistance || !rn2(4) )  ) {
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
		    make_burned(HDimmed + dimming, TRUE);
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

	    case AD_BLND:
		if (!mtmp->mcan && canseemon(mtmp) && !resists_blnd(&youmonst)
			&& distu(mtmp->mx,mtmp->my) <= BOLT_LIM*BOLT_LIM && (issoviet || !rn2(6)) ) {
		    int blnd = d((int)mattk->damn, (int)mattk->damd);

		    You("are blinded by %s radiance!",
			              s_suffix(mon_nam(mtmp)));
		    make_blinded((long)blnd,FALSE);
		    stop_occupation();
		    /* not blind at this point implies you're wearing
		       the Eyes of the Overworld; make them block this
		       particular stun attack too */
		    if (!Blind) Your(vision_clears);
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
		    if (Fire_resistance && rn2(20)) {
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
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
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

		if (level.flags.noteleport || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) dmg *= (1 + rnd(2));

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

				if (!u.uevent.udemigod && !(flags.lostsoul || flags.uberlostsoul || u.uprops[STORM_HELM].extrinsic) ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported"); /* because it's not called until you get another turn... */
					}
				}
				break;
			case 7:
				{
					int firststat = rn2(A_MAX);
					int secondstat = rn2(A_MAX);
					int firstswapstat = ABASE(firststat);
					int secondswapstat = ABASE(secondstat);
					int difference = (firstswapstat - secondswapstat);
					ABASE(secondstat) += difference;
					ABASE(firststat) -= difference;
					AMAX(secondstat) = ABASE(secondstat);
					AMAX(firststat) = ABASE(firststat);
					pline("Your stats got scrambled!");
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
		if (Deafness || (uwep && uwep->oartifact == ART_MEMETAL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_MEMETAL) || (uwep && uwep->oartifact == ART_BANG_BANG) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_BANG_BANG) || u.uprops[DEAFNESS].extrinsic || have_deafnessstone() ) dmgplus /= 2;
		make_stunned(HStun + dmgplus, TRUE);
		if (!rn2(issoviet ? 2 : 5)) (void)destroy_item(POTION_CLASS, AD_COLD);
		wake_nearby();
		}
		break;

	    case AD_GRAV:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(25))) {
		    pline("%s wiggles a %s, and suddenly you stand upside down...", Monnam(mtmp), mbodypart(mtmp, FINGER) );
		    stop_occupation();

		if (level.flags.noteleport || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) dmgplus *= 2;

		phase_door(0);
		pushplayer();
		u.uprops[DEAC_FAST].intrinsic += (dmgplus + 2);
		make_stunned(HStun + dmgplus, TRUE);
	      if (dmgplus) mdamageu(mtmp, dmgplus);
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
			couldsee(mtmp->mx, mtmp->my) &&
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
			      losexp("time", FALSE, FALSE); /* resistance is futile :D */
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

		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5)) /* extremely hot - very high chance to burn items! --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 2 : 5))
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
		  mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(1000))) {
		    pline("%s tries to drain your skills with its gaze!", Monnam(mtmp));
		    stop_occupation();
		    skillcaploss();
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
		    if (Fire_resistance && rn2(20)) {
			pline_The("fire doesn't feel hot!");
			dmg = 0;
		    }
		    burn_away_slime();
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 4 : 20)) /* hotter than ordinary fire attack, so more likely to burn items --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 4 : 20))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);

		    if (dmg) mdamageu(mtmp, dmg);
		    if (!rn2(5)) hurtarmor(AD_LAVA);
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
		    if (Fire_resistance && rn2(20)) {
			pline_The("fire doesn't feel hot!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    burn_away_slime();
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 6 : 33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(Race_if(PM_SEA_ELF) ? 1 : issoviet ? 10 : 50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);

		    if (Cold_resistance && rn2(20)) {
			pline_The("cold doesn't freeze you!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);

		    if (Shock_resistance && rn2(20)) {
			pline_The("gaze doesn't shock you!");
			if (dmg >= 4) dmg -= (dmg / 4);
		    }
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);

		    if (dmg) mdamageu(mtmp, dmg);

			if (!rn2(7) && (!Drain_resistance || !rn2(4) )  ) {
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
		    if (Cold_resistance && rn2(20)) {
			pline_The("cold doesn't freeze you!");
			dmg = 0;
		    }
		    /*if ((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
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
		    if (Shock_resistance && rn2(20)) {
			pline_The("gaze doesn't shock you!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(issoviet ? 6 : 33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
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
		    display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		    if (Shock_resistance && rn2(20)) {
			pline_The("gaze doesn't shock you!");
			dmg = 0;
		    }
		    if (!rn2(issoviet ? 2 : 10)) /* high voltage - stronger than ordinary shock attack --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    if (!rn2(issoviet ? 2 : 10))
			destroy_item(RING_CLASS, AD_ELEC);
		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;

	    case AD_UVUU:{

	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(50))) 		{
		pline("%s used HORN DRILL!", Monnam(mtmp));
		    stop_occupation();

		int wdmg = (int)(dmgplus/6) + 1;
		Sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		poisoned(buf, A_CON, mtmp->data->mname, 60);
		if(Poison_resistance) wdmg -= ACURR(A_CON)/2;
		if(wdmg > 0){
		
			while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
				wdmg--;
				(void) adjattrib(A_WIS, -1, TRUE);
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
           (void) adjattrib(A_INT, -1, FALSE);
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
		    stop_occupation();
			}
		break;
#endif
	    case AD_SLEE:
		if(!mtmp->mcan && canseemon(mtmp) &&
				mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
		    if (Displaced && rn2(3)) {
			if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
			    break;
		    }
		    if ((Invisible && rn2(3)) || rn2(4)) {
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
		    } else if (Sleep_resistance && rn2(20)) {
			pline("You yawn.");
		    } else {
			nomul(-rnd( (rn2(10) ? 10 : (10+dmgplus)) ), "sleeping from a monster's gaze");
			u.usleep = 1;
			nomovemsg = "You wake up.";
			if (Blind)  You("are put to sleep!");
			else You("are put to sleep by %s!",mon_nam(mtmp));
		    }
		}
		break;
	    case AD_DETH:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(18))) {
		    if (Displaced && rn2(3)) {
			if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
			    break;
		    }
		    if ((Invisible && rn2(3)) || rn2(4)) {
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
		    } else if (Antimagic) {
			You("shudder momentarily...");
		    } else {
			You(isangbander ? "have died." : "die...");
			killer_format = KILLED_BY_AN;
			killer = "gaze of death";
			done(DIED);
		    }
		}
		break;
	    case AD_PHYS:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || rn2(3))) {
	                if (Displaced && rn2(3)) {
	                        if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
	                        break;
	                }
	                if ((Invisible && rn2(3)) || rn2(4)) {
	                        if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
	                        break;
	                }
	                if (!Blind) pline("%s gazes directly at you!",Monnam(mtmp));
		    stop_occupation();
	                pline("You are wracked with pains!");
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

	    case AD_CHKH:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                  pline("%s gazes at you and screams the word 'DIE!'", Monnam(mtmp));
		    stop_occupation();
			dmgplus += u.chokhmahdamage;
			dmgplus += rnd(u.ualign.sins + 1);
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

			    if (!Stone_resistance &&
				!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {

				if (!Stoned) Stoned = 7;
				Sprintf(killer_buf, "being hit by a mirrored petrifying corpse");
				delayed_killer = killer_buf;
		
			    }
			}
			dmgplus += dmgval(uwep, &youmonst);
			
			if (uwep->opoisoned){
				Sprintf(buf, "%s %s",
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

	    case AD_FUMB:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s's gaze causes you to fumble!", Monnam(mtmp));

			HFumbling = FROMOUTSIDE | rnd(5);
			incr_itimeout(&HFumbling, rnd(20));
			u.fumbleduration += rnz(10 * (dmgplus + 1) );
		}

		break;

	    case AD_VULN:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
                pline("%s laughs devilishly!", Monnam(mtmp));

		 switch (rnd(123)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				u.uprops[DEAC_FIRE_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having fire resistance!");
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				u.uprops[DEAC_COLD_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having cold resistance!");
				break;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				u.uprops[DEAC_SLEEP_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having sleep resistance!");
				break;
			case 16:
			case 17:
				u.uprops[DEAC_DISINT_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having disintegration resistance!");
				break;
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
				u.uprops[DEAC_SHOCK_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having shock resistance!");
				break;
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				u.uprops[DEAC_POISON_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having poison resistance!");
				break;
			case 28:
			case 29:
			case 30:
				u.uprops[DEAC_DRAIN_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having drain resistance!");
				break;
			case 31:
			case 32:
				u.uprops[DEAC_SICK_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having sickness resistance!");
				break;
			case 33:
			case 34:
				u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having magic resistance!");
				break;
			case 35:
			case 36:
			case 37:
			case 38:
				u.uprops[DEAC_ACID_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having acid resistance!");
				break;
			case 39:
			case 40:
				u.uprops[DEAC_STONE_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having petrification resistance!");
				break;
			case 41:
				u.uprops[DEAC_FEAR_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having fear resistance!");
				break;
			case 42:
			case 43:
			case 44:
				u.uprops[DEAC_SEE_INVIS].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having see invisible!");
				break;
			case 45:
			case 46:
			case 47:
				u.uprops[DEAC_TELEPAT].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having telepathy!");
				break;
			case 48:
			case 49:
			case 50:
				u.uprops[DEAC_WARNING].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having warning!");
				break;
			case 51:
			case 52:
			case 53:
				u.uprops[DEAC_SEARCHING].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having automatic searching!");
				break;
			case 54:
				u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having clairvoyance!");
				break;
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
				u.uprops[DEAC_INFRAVISION].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having infravision!");
				break;
			case 60:
				u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having detect monsters!");
				break;
			case 61:
			case 62:
			case 63:
				u.uprops[DEAC_INVIS].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having invisibility!");
				break;
			case 64:
				u.uprops[DEAC_DISPLACED].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having displacement!");
				break;
			case 65:
			case 66:
			case 67:
				u.uprops[DEAC_STEALTH].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having stealth!");
				break;
			case 68:
				u.uprops[DEAC_JUMPING].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having jumping!");
				break;
			case 69:
			case 70:
			case 71:
				u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having teleport control!");
				break;
			case 72:
				u.uprops[DEAC_FLYING].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having flying!");
				break;
			case 73:
				u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having magical breathing!");
				break;
			case 74:
				u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having phasing!");
				break;
			case 75:
			case 76:
				u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having slow digestion!");
				break;
			case 77:
				u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having half spell damage!");
				break;
			case 78:
				u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having half physical damage!");
				break;
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
				u.uprops[DEAC_REGENERATION].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having regeneration!");
				break;
			case 84:
			case 85:
				u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having mana regeneration!");
				break;
			case 86:
			case 87:
			case 88:
				u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having polymorph control!");
				break;
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
				u.uprops[DEAC_FAST].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having speed!");
				break;
			case 94:
			case 95:
			case 96:
				u.uprops[DEAC_REFLECTING].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having reflection!");
				break;
			case 97:
			case 98:
			case 99:
				u.uprops[DEAC_FREE_ACTION].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having free action!");
				break;
			case 100:
				u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from hallu partying!");
				break;
			case 101:
				u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from drunken boxing!");
				break;
			case 102:
				u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having stunnopathy!");
				break;
			case 103:
				u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having numbopathy!");
				break;
			case 104:
				u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having freezopathy!");
				break;
			case 105:
				u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from being a stoned chiller!");
				break;
			case 106:
				u.uprops[DEAC_CORROSIVITY].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having corrosivity!");
				break;
			case 107:
				u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having an increased fear factor!");
				break;
			case 108:
				u.uprops[DEAC_BURNOPATHY].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having burnopathy!");
				break;
			case 109:
				u.uprops[DEAC_SICKOPATHY].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having sickopathy!");
				break;
			case 110:
				u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having keen memory!");
				break;
			case 111:
				u.uprops[DEAC_THE_FORCE].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from using the force like a real jedi!");
				break;
			case 112:
				u.uprops[DEAC_SIGHT_BONUS].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having extra sight!");
				break;
			case 113:
				u.uprops[DEAC_VERSUS_CURSES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having curse resistance!");
				break;
			case 114:
				u.uprops[DEAC_STUN_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having stun resistance!");
				break;
			case 115:
				u.uprops[DEAC_CONF_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having confusion resistance!");
				break;
			case 116:
				u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having double attacks!");
				break;
			case 117:
				u.uprops[DEAC_QUAD_ATTACK].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having quad attacks!");
				break;
			case 118:
				u.uprops[DEAC_PSI_RES].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having psi resistance!");
				break;
			case 119:
				u.uprops[DEAC_WONDERLEGS].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having wonderlegs!");
				break;
			case 120:
				u.uprops[DEAC_GLIB_COMBAT].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having glib combat!");
				break;
			case 121:
				u.uprops[DEAC_MANALEECH].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having manaleech!");
				break;
			case 122:
				u.uprops[DEAC_DIMMOPATHY].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having dimmopathy!");
				break;
			case 123:
				u.uprops[DEAC_PEACEVISION].intrinsic += rnz( (dmgplus * rnd(30) ) + 1);
				pline("You are prevented from having peacevision!");
				break;
		}
		}

		break;

	    case AD_ICUR:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s points at you and mumbles an arcane incantation!", Monnam(mtmp));

			if (!rn2(5)) {
				You_feel("as if you need some help.");
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
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && (issoviet || !rn2(5))) {
	                pline("%s uses a Topi Ylinen curse on you!", Monnam(mtmp));

		if (!rn2(50)) {
			pline("The ancient foul curse takes possession of you...");

			switch (rnd(85)) {

				case 1: RMBLoss += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 2: NoDropProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 3: DSTWProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 4: StatusTrapProblem += rnz( (dmgplus + 2) * rnd(100) ); 
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
				case 5: Superscroller += rnz( (dmgplus + 2) * rnd(100) * (Role_if(PM_GRADUATE) ? 2 : Role_if(PM_GEEK) ? 5 : 10) ); 
					(void) makemon(&mons[PM_SCROLLER_MASTER], 0, 0, NO_MINVENT);
					break;
				case 6: MenuBug += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 7: FreeHandLoss += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 8: Unidentify += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 9: Thirst += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 10: LuckLoss += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 11: ShadesOfGrey += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 12: FaintActive += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 13: Itemcursing += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 14: DifficultyIncreased += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 15: Deafness += rnz( (dmgplus + 2) * rnd(100) ); flags.soundok = 0; break;
				case 16: CasterProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 17: WeaknessProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 18: RotThirteen += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 19: BishopGridbug += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 20: UninformationProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 21: StairsProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 22: AlignmentProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 23: ConfusionProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 24: SpeedBug += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 25: DisplayLoss += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 26: SpellLoss += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 27: YellowSpells += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 28: AutoDestruct += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 29: MemoryLoss += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 30: InventoryLoss += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 31: {
	
					if (BlackNgWalls) break;
	
					BlackNgWalls = 1000 - (dmgplus * 3);
					if (BlackNgWalls < 100) BlackNgWalls = 100;
					(void) makemon(&mons[PM_BLACKY], 0, 0, NO_MM_FLAGS);
					break;
				}
				case 32: IntrinsicLossProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 33: BloodLossProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 34: BadEffectProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 35: TrapCreationProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 36: AutomaticVulnerabilitiy += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 37: TeleportingItems += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 38: NastinessProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 39: CaptchaProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 40: FarlookProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 41: RespawnProblem += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 42: RecurringAmnesia += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 43: BigscriptEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 44: {
					BankTrapEffect += rnz( (dmgplus + 2) * rnd(100) );
					if (u.bankcashlimit == 0) u.bankcashlimit = rnz(1000 * (monster_difficulty() + 1));
					u.bankcashamount += u.ugold;
					u.ugold = 0;
	
					break;
				}
				case 45: MapTrapEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 46: TechTrapEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 47: RecurringDisenchant += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 48: verisiertEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 49: ChaosTerrain += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 50: Muteness += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 51: EngravingDoesntWork += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 52: MagicDeviceEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 53: BookTrapEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 54: LevelTrapEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 55: QuizTrapEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 56: FastMetabolismEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 57: NoReturnEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 58: AlwaysEgotypeMonsters += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 59: TimeGoesByFaster += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 60: FoodIsAlwaysRotten += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 61: AllSkillsUnskilled += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 62: AllStatsAreLower += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 63: PlayerCannotTrainSkills += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 64: PlayerCannotExerciseStats += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 65: TurnLimitation += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 66: WeakSight += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 67: RandomMessages += rnz( (dmgplus + 2) * rnd(100) ); break;

				case 68: Desecration += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 69: StarvationEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 70: NoDropsEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 71: LowEffects += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 72: InvisibleTrapsEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 73: GhostWorld += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 74: Dehydration += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 75: HateTrapEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 76: TotterTrapEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 77: Nonintrinsics += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 78: Dropcurses += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 79: Nakedness += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 80: Antileveling += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 81: ItemStealingEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 82: Rebellions += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 83: CrapEffect += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 84: ProjectilesMisfire += rnz( (dmgplus + 2) * rnd(100) ); break;
				case 85: WallTrapping += rnz( (dmgplus + 2) * rnd(100) ); break;
			}

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

			if (!Poison_resistance) pline("You're badly poisoned!");
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_STR, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_DEX, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CON, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_INT, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_WIS, -rnd(2), FALSE);
			if (!rn2( (Poison_resistance && rn2(20) ) ? 20 : 4 )) (void) adjattrib(A_CHA, -rnd(2), FALSE);
	                poisoned("The gaze", rn2(A_MAX), mtmp->data->mname, 30);
		}
		break; 

	    case AD_NPRO:

		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(25))) {
                pline("%s gives you an excruciating look!", Monnam(mtmp));
		    stop_occupation();
			u.negativeprotection++;
		}
		break;

          case AD_DISE:
		if (rn2(3)) break; /* lower chance for normal disease, so pestilence attack is unique --Amy */
          case AD_PEST:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(12))) {
	                pline("%s leers down on you!", Monnam(mtmp));
		    stop_occupation();
			(void) digeasemu(mtmp); /* plus the normal damage */
	        }
	        break;

	    case AD_CHRN:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && (issoviet || !rn2(12))) {
	                pline("%s gazes at you and curses horribly.", Monnam(mtmp));
		    stop_occupation();

		    switch (rn2(11)) {
		    case 0: diseasemu(mtmp->data);
			    break;
		    case 1: make_blinded(Blinded + dmgplus, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.",
				    Hallucination ? "trippy" : "confused");
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
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE);
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
	                if (Free_action) You("stiffen momentarily.");
	                else {
	                        You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;
	                        nomul(-rnd(4), "paralyzed by a monster's gaze");
	                        exercise(A_DEX, FALSE);
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
		pushplayer();
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
	if (flags.iwbtg) {

		You("explode in a fountain of red pixels!");
		pline("GAME OVER - press R to try again");
		done_in_by(mtmp);

	}

	/* sometimes you take less damage. The game is deadly enough already. High constitution helps. --Amy */
	if (!issoviet && rn2(ABASE(A_CON))) {
	if (!rn2(3) && n >= 1) {n = n / 2; if (n < 1) n = 1;}
	if (!rn2(10) && n >= 1 && u.ulevel >= 10) {n = n / 3; if (n < 1) n = 1;}
	if (!rn2(15) && n >= 1 && u.ulevel >= 14) {n = n / 4; if (n < 1) n = 1;}
	if (!rn2(20) && n >= 1 && u.ulevel >= 20) {n = n / 5; if (n < 1) n = 1;}
	if (!rn2(50) && n >= 1 && u.ulevel >= 30) {n = n / 10; if (n < 1) n = 1;}
	}

	if (n && Race_if(PM_YUKI_PLAYA)) n += rnd(5);
	if (Role_if(PM_BLEEDER)) n = n * 2; /* bleeders are harder than hard mode */
	if (have_cursedmagicresstone()) n = n * 2;

	if (Invulnerable || (Stoned_chiller && Stoned)) n=0;
	if (n == 0) {
		pline("You are unharmed.");
		return;
	}

	if (Race_if(PM_PLAYER_SKELETON) && rn2(3) && !(hit_as_two(mtmp) || hit_as_three(mtmp) || hit_as_four(mtmp) || (MON_WEP(mtmp) && (MON_WEP(mtmp))->spe > 1) ) ) {
		pline("The attack doesn't seem to harm you.");
		n = 0;
	}

	/* WAC For consistency...DO be careful using techniques ;B */
	if (mtmp->mtame != 0 && tech_inuse(T_PRIMAL_ROAR)) {
		n *= 2; /* Double Damage! */
	}

	flags.botl = 1; /* This needs to be AFTER the pline for botl to be 
	 		 * updated correctly -- Kelly Bailey
	 		 */

	if (u.disruptionshield && u.uen >= n) {
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
	if (flags.showdmg && !DisplayLoss && !u.uprops[DISPLAY_LOST].extrinsic && !have_displaystone() && !(uarmc && uarmc->oartifact == ART_CLOAK_OF_THE_CONSORT) ) {

		pline("[-%d -> %d]", n, (Upolyd ? (u.mh) : (u.uhp) ) );  /* WAC see damage */
		if (!Upolyd && (( (u.uhp) * 5) < u.uhpmax)) pline(isangbander ? "***LOW HITPOINT WARNING***" : "Warning: HP low!");

	}
#endif

	if (u.uprops[TURNLIMITATION].extrinsic || (uarmf && uarmf->oartifact == ART_OUT_OF_TIME) || (uarmu && uarmu->oartifact == ART_THERMAL_BATH) || TurnLimitation || have_limitationstone() ) {
		if (n > 0) u.ascensiontimelimit -= n;
		if (u.ascensiontimelimit < 1) u.ascensiontimelimit = 1;
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
		if (obj->greased || obj->oerodeproof || (obj->blessed && rn2(3))) {
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
		return (!rn2(25)) ? 1 : (pagr->mlet == S_NYMPH) ? 2 : 0;
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
	boolean birthing;

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "birthcloth") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "rozhdeniye tkan'") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "tug'ilgan mato") )) birthing = 1;
	else birthing = 0;

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

	for(ring = invent; ring; ring = nring) {
	    nring = ring->nobj;
	    if (ring->otyp != RIN_ADORNMENT) continue;
	    if (fem) {
		if (rn2(120) < ACURR(A_CHA)) {
		    Sprintf(qbuf, "\"That %s looks pretty.  May I have it?\"",
			safe_qbuf("",sizeof("\"That  looks pretty.  May I have it?\""),
			xname(ring), simple_typename(ring->otyp), "ring"));
		    makeknown(RIN_ADORNMENT);
		    if (yn(qbuf) == 'n') continue;
		} else pline("%s decides she'd like your %s, and takes it.",
			Blind ? "She" : Monnam(mon), xname(ring));
		makeknown(RIN_ADORNMENT);
		if (ring==uleft || ring==uright) Ring_gone(ring);
		if (ring==uwep) setuwep((struct obj *)0, FALSE);
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
		    Sprintf(qbuf,"\"That %s looks pretty.  Would you wear it for me?\"",
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
		    Strcpy(buf, xname(uright));
		    pline("%s replaces your %s with your %s.",
			Blind ? "He" : Monnam(mon), buf, xname(ring));
		    Ring_gone(uright);
		    setworn(ring, RIGHT_RING);
		} else if (uleft && uleft->otyp != RIN_ADORNMENT) {
		    Strcpy(buf, xname(uleft));
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

	/* "Remove a stupid line of dialogue. This is not an adult visual novel.  The rest of the dialogue scattered around the source files like this will be cleaned up in due time." In Soviet Russia, people are filthy heretics who don't fully appreciate the beauty of Slash'EM Extended, which causes them to pick the best features of the game and remove them. :( --Amy */

	if (rnd(ACURR(A_CHA)) < 3) { /* random chance of being betrayed by your love interest... */

	monsterlev = ((mon->m_lev) + 1);
	if (monsterlev <= 0) monsterlev = 1;

	if (!flags.female) { pline("But %s suddenly rams her sexy knees right into your nuts! OUCH!", Monnam(mon));
				losehp(d(3,monsterlev), "treacherous lady", KILLED_BY_AN);
			}
	if (flags.female) { pline("But all of a sudden, %s clenches his burly hands to a fist and violently punches your breasts! AIIIEEEEEGGGGGHHHHH!", Monnam(mon));
				losehp(d(3,monsterlev), "filthy traitor", KILLED_BY_AN);
			}

	return 1;
	}

	/* by this point you have discovered mon's identity, blind or not... */
	pline("Time stands still while you and %s lie in each other's arms...",
		noit_mon_nam(mon));
	/* Well,  IT happened ... */
	u.uconduct.celibacy++;
	
	if (rn2(135) > ACURR(A_CHA) + ACURR(A_INT)) /*much higher chance of negative outcome now --Amy */ {
		/* Don't bother with mspec_used here... it didn't get tired! */
		pline("%s seems to have enjoyed it more than you...",
			noit_Monnam(mon));
		switch (rn2(5)) {
			case 0: You_feel("drained of energy.");
				u.uen = 0;
				u.uenmax -= rnd(Half_physical_damage ? 5 : 10);
			        exercise(A_CON, FALSE);
				if (u.uenmax < 0) u.uenmax = 0;
				break;
			case 1: You("are down in the dumps.");
				(void) adjattrib(A_CON, -1, TRUE);
			        exercise(A_CON, FALSE);
				flags.botl = 1;
				break;
			case 2: Your("senses are dulled.");
				(void) adjattrib(A_WIS, -1, TRUE);
			        exercise(A_WIS, FALSE);
				flags.botl = 1;
				break;
			case 3:
				if (!Drain_resistance || !rn2(4) ) {
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
				losehp(tmp, "exhaustion", KILLED_BY);
				break;
			}
		}
	} else {
		mon->mspec_used = rnd(100); /* monster is worn out */
		You("seem to have enjoyed it more than %s...",
		    noit_mon_nam(mon));
		switch (rn2(5)) {
		case 0: You_feel("raised to your full potential.");
			exercise(A_CON, TRUE);
			u.uen = (u.uenmax += rnd(5));
			break;
		case 1: You_feel("good enough to do it again.");
			(void) adjattrib(A_CON, 1, TRUE);
			exercise(A_CON, TRUE);
			flags.botl = 1;
			break;
		case 2: You("will always remember %s...", noit_mon_nam(mon));
			(void) adjattrib(A_WIS, 1, TRUE);
			exercise(A_WIS, TRUE);
			flags.botl = 1;
			break;
		case 3: pline("That was a very educational experience.");
			pluslvl(FALSE);
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

	if (mon->mtame) /* don't charge */ ;
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

	/* "Disable Pregnancy via foocubus/seducing encounters - Let's not do this, shall we?" In Soviet Russia, people aren't being conceived by sexual intercourse. Rather, they just spawn because God decided to create them from thin air. They're also inexplicably prude, which probably is the reason why they don't want pregnancy in their video games either. I guess they won't touch Elona with a ten-foot pole... --Amy */

	if (!rn2(birthing ? 3 : 50) && !issoviet) {

	/* Yes, real-life pregnancy doesn't work like this. But I want to avoid having to make complicated functions,
	   so the player will just get an egg that immediately hatches and may be tame. --Amy */

		struct obj *uegg;

		if (flags.female) { pline("Uh-oh - you're pregnant!"); verbalize("Be a good mother, sweetheart!");
		}
		else { pline("Oh! %s is pregnant!",noit_Monnam(mon)); verbalize("Please take good care of my baby, %s!",plname);
		}

		uegg = mksobj(EGG, FALSE, FALSE);
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
			(void) start_timer(1, TIMER_OBJECT, HATCH_EGG, (genericptr_t)uegg);
			pickup_object(uegg, 1, FALSE);
		}

		if (uarmc && uarmc->oartifact == ART_CATHERINE_S_SEXUALITY) {
			pline("Oh no... your heart... it's... getting... unsteady...");
			pline("BEEPBEEP BEEPBEEP BEEP BEEP BEEEEEEEEEEEEEEEEEEEEP!");
			pline("You die from a heart failure.");
			killer_format = KILLED_BY;
			killer = "complications from childbirth";
			done(DIED);
		}

	}

	/* maybe contract a disease? --Amy */
	if (!rn2(25) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted syphilis from having unprotected intercourse with your lover!");
		(void) diseasemu(mon->data);
	}

	if (!rn2(1000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted AIDS from having unprotected intercourse with your lover!");
		u.uprops[DEAC_FIRE_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_COLD_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_SLEEP_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_DISINT_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_SHOCK_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_POISON_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_DRAIN_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_SICK_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_ACID_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_STONE_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_FEAR_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_SEE_INVIS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_TELEPAT].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_WARNING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_SEARCHING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_CLAIRVOYANT].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_INFRAVISION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_DETECT_MONSTERS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_INVIS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_DISPLACED].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_STEALTH].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_JUMPING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_TELEPORT_CONTROL].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_FLYING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_MAGICAL_BREATHING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_PASSES_WALLS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_SLOW_DIGESTION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_HALF_SPDAM].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_HALF_PHDAM].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_REGENERATION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_ENERGY_REGENERATION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_FAST].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_REFLECTING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_FREE_ACTION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_HALLU_PARTY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_DRUNKEN_BOXING].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_STUNNOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_NUMBOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_DIMMOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_FREEZOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_STONED_CHILLER].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_CORROSIVITY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_FEAR_FACTOR].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_BURNOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_SICKOPATHY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_KEEN_MEMORY].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_THE_FORCE].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_SIGHT_BONUS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_VERSUS_CURSES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_STUN_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_CONF_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_DOUBLE_ATTACK].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_QUAD_ATTACK].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_PSI_RES].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_WONDERLEGS].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_GLIB_COMBAT].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_MANALEECH].intrinsic += rnz( (monster_difficulty() * 10) + 1);
		u.uprops[DEAC_PEACEVISION].intrinsic += rnz( (monster_difficulty() * 10) + 1);
	}

	if (!rn2(250) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted ataxia from having unprotected intercourse with your lover!");
		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(1000);
	}

	if (!rn2(100) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted rust chancres from having unprotected intercourse with your lover!");
	      HAggravate_monster |= FROMOUTSIDE;
	}

	if (!rn2(200) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted chills from having unprotected intercourse with your lover!");
		make_frozen(HFrozen + rnd(10) + rnd(monster_difficulty() + 1), TRUE);
		set_itimeout(&HeavyFrozen, HFrozen);
	}

	if (!rn2(500) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted blackheart plague from having unprotected intercourse with your lover!");

		if (!flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) && !Slimed) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY;
		    delayed_killer = "the blackheart plague";
		} else
		    pline("Yuck!");

	}

	if (!rn2(100) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted swamp fever from having unprotected intercourse with your lover!");
		make_burned(HBurned + rnd(100) + rnd((monster_difficulty() * 10) + 1), TRUE);
		set_itimeout(&HeavyBurned, HBurned);
	}

	if (!rn2(100) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted depression from having unprotected intercourse with your lover!");
		make_dimmed(HDimmed + rnd(100) + rnd((monster_difficulty() * 10) + 1), TRUE);
		set_itimeout(&HeavyDimmed, HDimmed);
	}

	if (!rn2(500) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted gray star from having unprotected intercourse with your lover!");
		make_blinded(Blinded + rnd(100) + rnd((monster_difficulty() * 10) + 1), TRUE);
		set_itimeout(&HeavyBlind, Blinded);
	}

	if (!rn2(500) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted voracious hunger from having unprotected intercourse with your lover!");
	      HHunger |= FROMOUTSIDE;
	}

	if (!rn2(5000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted polymorphitis from having unprotected intercourse with your lover!");
	      HPolymorph |= FROMOUTSIDE;
	}

	if (!rn2(5000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted green star from having unprotected intercourse with your lover!");
	      u.uprops[WEAKSIGHT].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(50000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted an ancient foul curse from having unprotected intercourse with your lover!");
	      u.uprops[PREMDEATH].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted witbane from having unprotected intercourse with your lover!");
	      u.uprops[UNIDENTIFY].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(5000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted brain rot from having unprotected intercourse with your lover!");
	      u.uprops[LOW_EFFECTS].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(4000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted dehydration from having unprotected intercourse with your lover!");
	      u.uprops[DEHYDRATION].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(5000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted alexithymia from having unprotected intercourse with your lover!");
	      u.uprops[HATE_TRAP_EFFECT].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted hydroanemia from having unprotected intercourse with your lover!");
	      u.uprops[THIRST].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted lung cancer from having unprotected intercourse with your lover!");
	      u.uprops[NONINTRINSIC_EFFECT].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(25000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted Tourette's syndrome from having unprotected intercourse with your lover!");
	      u.uprops[TOTTER_EFFECT].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted Dudley's disease from having unprotected intercourse with your lover!");
	      u.uprops[LUCK_LOSS].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(1000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted fuckedupness (the disease that has befallen Christian Grey) from having unprotected intercourse with your lover!");
	      u.uprops[SHADES_OF_GREY].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(1000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted Rodney's Black Cancer from having unprotected intercourse with your lover!");
	      u.uprops[ITEMCURSING].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted narcolepsy from having unprotected intercourse with your lover!");
	      u.uprops[FAINT_ACTIVE].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(400) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted diarrhea from having unprotected intercourse with your lover!");
	      u.uprops[CRAP_EFFECT].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted deafness from having unprotected intercourse with your lover!");
	      u.uprops[DEAFNESS].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(5000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted a tapeworm infection from having unprotected intercourse with your lover!");
	      u.uprops[WEAKNESS_PROBLEM].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(2000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted a heavy flu from having unprotected intercourse with your lover!");
	      u.uprops[STATUS_FAILURE].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(2000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted loss of will from having unprotected intercourse with your lover!");
	      u.uprops[ITEM_STEALING_EFFECT].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(4000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted incontinence from having unprotected intercourse with your lover!");
	      u.uprops[REBELLION_EFFECT].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted migraine from having unprotected intercourse with your lover!");
	      u.uprops[UNINFORMATION].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(20000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted mongoloism (trisomia-13) from having unprotected intercourse with your lover!");
	      u.uprops[ANTILEVELING].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted Alzheimer's disease from having unprotected intercourse with your lover!");
	      u.uprops[RECURRING_AMNESIA].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted telomer cutting disease from having unprotected intercourse with your lover!");
	      u.uprops[INTRINSIC_LOSS].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted tongue crippling from having unprotected intercourse with your lover!");
	      u.uprops[MUTENESS].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted parkinson from having unprotected intercourse with your lover!");
	      u.uprops[ENGRAVINGBUG].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(2000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted jazzy hands from having unprotected intercourse with your lover!");
	      u.uprops[PROJECTILES_MISFIRE].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted shatterhands from having unprotected intercourse with your lover!");
	      u.uprops[MAGIC_DEVICE_BUG].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted dyslexia from having unprotected intercourse with your lover!");
	      u.uprops[BOOKBUG].intrinsic |= FROMOUTSIDE;
	      u.uprops[CONFUSION_PROBLEM].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted hypermetabolism from having unprotected intercourse with your lover!");
	      u.uprops[FAST_METABOLISM].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted ghoulification from having unprotected intercourse with your lover!");
	      u.uprops[FOOD_IS_ROTTEN].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted autism from having unprotected intercourse with your lover!");
	      u.uprops[SKILL_DEACTIVATED].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted schizophreny from having unprotected intercourse with your lover!");
	      u.uprops[STATS_LOWERED].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted down syndrome from having unprotected intercourse with your lover!");
	      u.uprops[TRAINING_DEACTIVATED].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted locked-in syndrome from having unprotected intercourse with your lover!");
	      u.uprops[EXERCISE_DEACTIVATED].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted magic vacuum from having unprotected intercourse with your lover!");
	      u.uprops[NORETURN].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(10000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted brittle bones from having unprotected intercourse with your lover!");
	      u.uprops[NAKEDNESS].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(1000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted osteoporosis from having unprotected intercourse with your lover!");
		set_wounded_legs(LEFT_SIDE, HWounded_legs + rnz(10000));
		set_wounded_legs(RIGHT_SIDE, HWounded_legs + rnz(10000));

	}

 	if (!rn2(2000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted porphyric hemophilia from having unprotected intercourse with your lover!");
	      u.uprops[BLOOD_LOSS].intrinsic |= FROMOUTSIDE;
	}

 	if (!rn2(50000) && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted an ancient Morgothian curse from having unprotected intercourse with your lover!");
	      u.uprops[NASTINESS_EFFECTS].intrinsic |= FROMOUTSIDE;
	}

	if (!rn2(75) && !flags.female && !(ublindf && ublindf->otyp == CONDOME) && !(ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) && !(uarm && uarm->oartifact == ART_CHASTITY_ARMOR) && !(uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) ) {
		pline("Ulch - you contracted erective dysfunction from having unprotected intercourse with your lover!");
		make_numbed(HNumbed + rnd(100) + rnd((monster_difficulty() * 10) + 1), TRUE);
		set_itimeout(&HeavyNumbed, HNumbed);
	}

	if (ublindf && ublindf->otyp == CONDOME) pline("Your condome kept you safe from any diseases you might otherwise have contracted.");
	if (ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) pline("Your condome kept you safe from any diseases you might otherwise have contracted.");
	if (uarm && uarm->oartifact == ART_CHASTITY_ARMOR) pline("Your chastity armor kept you safe from any diseases you might otherwise have contracted.");
	if (uarm && uarm->oartifact == ART_LITTLE_PENIS_WANKER) {
		pline("Your penis-protection armor kept you safe from any diseases you might otherwise have contracted.");
		if (flags.female) pline("Thankfully it's shaped such that it also protects vaginas. :-)");
	}

	if (!rn2(Role_if(PM_LADIESMAN) ? 100 : 10)) mon->mcan = 1; /* monster is worn out; chance is much higher now --Amy */
	if (!tele_restrict(mon)) (void) rloc(mon, FALSE);
	return 1;
}

STATIC_OVL void
mayberem(obj, str)
register struct obj *obj;
const char *str;
{
	char qbuf[QBUFSZ];

	if (!obj || !obj->owornmask) return;

	if ((rn2(120) < ACURR(A_CHA)) || (uarmf && uarmf->oartifact == ART_RARE_ASIAN_LADY)) { /*much lower chance for the player to resist --Amy*/
		Sprintf(qbuf,"\"Shall I remove your %s, %s?\"",
			str,
			(!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart"));
		if (yn(qbuf) == 'n') return;
	} else {
		char hairbuf[BUFSZ];

		Sprintf(hairbuf, "let me run my fingers through your %s",
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

	if (Slimed && Corrosivity) {

		pline("%s is covered with a corrosive substance!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(u.ulevel) ) <= 0) {
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

	if (u.thornspell) {
		pline("%s is damaged by your thorns!", Monnam(mtmp));
		if((mtmp->mhp -= rnd(5 + (u.ulevel / 6)) ) <= 0) {
			pline("%s bleeds to death!", Monnam(mtmp));
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

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "electrostatic cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "elektrostaticheskoye plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "elektrofizikaviy kompyuteringizda ornatilgan plash") ) ) {
		if((mtmp->mhp -= rnd(4) ) <= 0) {
			pline("%s is electrocuted and dies!", Monnam(mtmp));
			xkilled(mtmp,0);
			if (mtmp->mhp > 0) return 1;
			return 2;
		}
	}

	for(i = 0; ; i++) {
	    if(i >= NATTK) return 1;
	    if (olduasmon->mattk[i].aatyp == AT_NONE ||
	    		olduasmon->mattk[i].aatyp == AT_BOOM) break;
	}
	if (olduasmon->mattk[i].damn)
	    tmp = d((int)olduasmon->mattk[i].damn,
				    (int)olduasmon->mattk[i].damd);
	else if(olduasmon->mattk[i].damd)
	    tmp = d( ((int)olduasmon->mlevel / 5)+1, (int)olduasmon->mattk[i].damd); /* Players polymorphed into blue slimes or similar stuff aren't supposed to be OP. --Amy */
	else
	    tmp = 0;

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
	if (!Upolyd) return 1;

	/* These affect the enemy only if you are still a monster */
	if (rn2(3)) switch(youmonst.data->mattk[i].adtyp) {
	    case AD_PHYS:
	    	if (youmonst.data->mattk[i].aatyp == AT_BOOM ) {
	    	    You("explode!");
	    	    /* KMH, balance patch -- this is okay with unchanging */
	    	    if (!Race_if(PM_UNGENOMOLD)) rehumanize(); /* we don't want ungenomolds to die from being a graveler */
			else polyself(FALSE);
	    	    goto assess_dmg;
	    	}
	    	break;
	    case AD_PLYS: /* Floating eye */
		if (tmp > 127) tmp = 127;
		if (u.umonnum == PM_FLOATING_EYE) {
		    /*if (!rn2(4)) tmp = 127;*/
		    if (mtmp->mcansee && haseyes(mtmp->data) && rn2(3) &&
				(perceives(mtmp->data) || !Invis)) {
			if (Blind)
			    pline("As a blind %s, you cannot defend yourself.",
							youmonst.data->mname);
		        else {
			    if (mon_reflects(mtmp,
					    "Your gaze is reflected by %s %s."))
				return 1;
			    pline("%s is frozen by your gaze!", Monnam(mtmp));
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = tmp;
			    return 3;
			}
		    }
		} else { /* gelatinous cube */
		    pline("%s is frozen by you.", Monnam(mtmp));
		    mtmp->mcanmove = 0;
		    mtmp->mfrozen = tmp;
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
		u.mh += tmp / 2;
		if (u.mhmax < u.mh) u.mhmax = u.mh;
		if (u.mhmax > ((youmonst.data->mlevel+1) * 8) && !rn2(25) ) /* slow down farming --Amy */
		    (void)split_mon(&youmonst, mtmp);
		break;
	    case AD_STUN: /* Yellow mold */
		tmp = 0; /* fall through */
	    case AD_FUMB:
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
		if (!mtmp->msleeping && sleep_monst(mtmp, rnd(10), -1)) {
		    pline("%s is put to sleep.", Monnam(mtmp));
		    mtmp->mstrategy &= ~STRAT_WAITFORU;
		    slept_monst(mtmp);
		}
		break;

	    case AD_SLOW:
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
		if(!rn2(3) && mtmp->mcanmove) {
		    pline("%s is paralyzed.", Monnam(mtmp));
		    mtmp->mcanmove = 0;
		    mtmp->mfrozen = rnd(10);
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
		litroomlite(FALSE);
		pline("A sinister darkness fills the area!");
		break;

	    case AD_THIR:
		healup(tmp, 0, FALSE, FALSE);
		You_feel("healthier!");
		break;
	    case AD_FRZE:
		if (!resists_cold(mtmp) && resists_fire(mtmp)) {
			tmp *= 2;
			pline("%s is suddenly ice-cold!", Monnam(mtmp));
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
		pline(fauxmessage());
		if (!rn2(3)) pline(fauxmessage());
		break;
	    case AD_WEBS:
		(void) maketrap(mtmp->mx, mtmp->my, WEB, 0);
		if (!rn2(issoviet ? 2 : 8)) makerandomtrap();
		break;
	    case AD_CNCL:
		if (rnd(100) > mtmp->data->mr) {
			mtmp->mcan = 1;
			pline("%s is covered in sparkling lights!", Monnam(mtmp));
		}
		break;
	    case AD_ICUR:
	    case AD_CURS:
		if (!rn2(10) && (rnd(100) > mtmp->data->mr)) {
			mtmp->mcan = 1;
		}
		break;
	    case AD_FEAR:
		if (rnd(100) > mtmp->data->mr) {
		     monflee(mtmp, rnd(1 + tmp), FALSE, TRUE);
			pline("%s is suddenly very afraid!",Monnam(mtmp));
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

	if (u.mh <= 1) return(struct monst *)0;
	if (mvitals[mndx].mvflags & G_EXTINCT) return(struct monst *)0;
	mon = makemon(youmonst.data, u.ux, u.uy, NO_MINVENT|MM_EDOG);
	if (mon) {
	mon = christen_monst(mon, plname);
	initedog(mon);
	mon->m_lev = youmonst.data->mlevel;
	mon->mhpmax = u.mhmax;
	mon->mhp = u.mh / 2;
	u.mh -= mon->mhp;
	flags.botl = 1;
	}
	return(mon);
}

#endif /* OVLB */

/*mhitu.c*/

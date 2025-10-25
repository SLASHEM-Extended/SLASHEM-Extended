/*	SCCS Id: @(#)sit.c	3.4	2002/09/21	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"

static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0};
static const char allnoncount[] = { ALL_CLASSES, 0};

void
take_gold()
{
#ifndef GOLDOBJ
	if (u.ugold <= 0)  {
		You_feel("a strange sensation.");
	} else {
		You("notice you have no gold!");
		u.bankcashamount += u.ugold; /* even if you don't have the bank trap effect --Amy */
		u.ugold = 0;
		flags.botl = 1;
	}
#else
        struct obj *otmp, *nobj;
	int lost_money = 0;
	for (otmp = invent; otmp; otmp = nobj) {
		nobj = otmp->nobj;
		if (otmp->oclass == COIN_CLASS) {
			lost_money = 1;
			delobj(otmp);
		}
	}
	if (!lost_money)  {
		You_feel("a strange sensation.");
	} else {
		You("notice you have no money!");
		flags.botl = 1;
	}
#endif
}

int
dosit()
{

	register struct obj *otmp;

	if (MenuIsBugged) {
	pline("The sit command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	static const char sit_message[] = "sit on the %s.";
	register struct trap *trap;
	register int typ = levl[u.ux][u.uy].typ;


	if (u.usteed) {
	    You("are already sitting on %s.", mon_nam(u.usteed));
	    return (0);
	}

	if(!can_reach_floor())	{
	    if (Levitation)
		You("tumble in place.");
	    else
		You("are sitting on air.");
	    return 0;
	} else if (is_waterypool(u.ux, u.uy) && !Underwater) {  /* water walking */
	    goto in_water;
	}

	if(OBJ_AT(u.ux, u.uy)) {
	    register struct obj *obj;

	    obj = level.objects[u.ux][u.uy];
	    You("sit on %s.", the(xname(obj)));
	    if (!(Is_box(obj) || objects[obj->otyp].oc_material == MT_CLOTH || objects[obj->otyp].oc_material == MT_SILK || objects[obj->otyp].oc_material == MT_INKA))
		pline("It's not very comfortable...");

	} else if ((trap = t_at(u.ux, u.uy)) != 0 ||
		   (u.utrap && (u.utraptype >= TT_LAVA))) {

	    if (u.utrap) {
		exercise(A_WIS, FALSE);	/* you're getting stuck longer */
		if(u.utraptype == TT_BEARTRAP) {
		    You_cant("sit down with your %s in the bear trap.", body_part(FOOT));
		    u.utrap++;
	        } else if(u.utraptype == TT_PIT) {
		    if(trap->ttyp == SPIKED_PIT) {
			You("sit down on a spike.  Ouch!");
			losehp(1, "sitting on an iron spike", KILLED_BY);
			exercise(A_STR, FALSE);
		    } else
			You("sit down in the pit.");
		    u.utrap += rn2(5);
		} else if(u.utraptype == TT_WEB) {
		    You("sit in the spider web and get entangled further!");
		    u.utrap += rn1(10, 5);
		} else if(u.utraptype == TT_GLUE) {
		    You("immerse yourself with glue!");
		    u.utrap += rn1(10, 5);
		} else if(u.utraptype == TT_LAVA) {
		    /* Must have fire resistance or they'd be dead already */
		    You("sit in the lava!");
		    u.utrap += rnd(4);
		    losehp(d(2,10), "sitting in lava", KILLED_BY);
		} else if(u.utraptype == TT_INFLOOR) {
		    You_cant("maneuver to sit!");
		    u.utrap++;
		}
	    } else {
	        You("sit down.");
		dotrap(trap, 0);
	    }
	} else if(Underwater || Is_waterlevel(&u.uz)) {
	    if (Is_waterlevel(&u.uz))
		There("are no cushions floating nearby.");
	    else
		You("sit down on the muddy bottom.");
	} else if(is_waterypool(u.ux, u.uy)) {
 in_water:
	    You("sit in the water.");
	    if (!rn2(10) && uarm)
		(void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst);
	    else if (!rn2(10) && uarmf && uarmf->otyp != WATER_WALKING_BOOTS)
		(void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst);
	} else if(IS_SINK(typ)) {

	    You(sit_message, defsyms[S_sink].explanation);
	    Your("%s gets wet.", humanoid(youmonst.data) ? "rump" : "underside");
	} else if(IS_TOILET(typ)) {
	    You(sit_message, defsyms[S_toilet].explanation);
	    if ((!Sick || !issoviet) && (u.uhs > 0)) You("don't have to go...");
	    else {
			u.cnd_toiletamount++; /* doesn't count if you don't actually take a crap :P --Amy */
			if (issoviet && u.uhs > 0) pline("Vy der'mo vedro, vy delayete svoye der'mo iz vozdukha? Nel'zya dazhe der'mo, kak i vy!");

			use_skill(P_SQUEAKING, 2);
			if (uarmu && uarmu->oartifact == ART_KATIA_S_SOFT_COTTON) {
				You("produce very erotic noises.");
				if (!rn2(10)) adjattrib(rn2(A_CHA), 1, -1, TRUE);
			}
			else if (Role_if(PM_BARBARIAN) || Role_if(PM_NOOB_MODE_BARB) || Role_if(PM_CAVEMAN)) You("miss...");
			else You("grunt.");

			if (practicantterror) {
				pline("%s booms: 'There's a fee of 100 zorkmids for using the toilet.'", noroelaname());
				fineforpracticant(100, 0, 0);
				if (Role_if(PM_BARBARIAN) || Role_if(PM_NOOB_MODE_BARB) || Role_if(PM_CAVEMAN)) {
					pline("%s thunders: 'People like you are the reason for that usage fee! You pay 5000 zorkmids extra for smutching the toilet!'", noroelaname());
					fineforpracticant(5000, 0, 0);
				}
			}

			if (uarmf && uarmf->oartifact == ART_LUDGERA_S_SECRET_COURSE) {
				pline("That felt good.");
				incr_itimeout(&HFast, rn1(250, 250));
			}

			/* Based on real life experience (urgh) this doesn't always instantly cure sickness. --Amy */
			if (Sick && !rn2(3) ) make_sick(0L, (char *)0, TRUE, SICK_VOMITABLE);
			else if (Sick && !rn2(10) ) make_sick(0L, (char *)0, TRUE, SICK_ALL);
			if (u.uhs == 0) morehungry(rn2(400)+200);
	    }
	} else if(IS_ALTAR(typ)) {

	    You(sit_message, defsyms[S_altar].explanation);
	    altar_wrath(u.ux, u.uy);

	} else if(IS_GRAVE(typ)) {

	    You(sit_message, defsyms[S_grave].explanation);

	} else if(typ == STAIRS) {

	    You(sit_message, "stairs");

	} else if(typ == LADDER) {

	    You(sit_message, "ladder");

	} else if (is_lava(u.ux, u.uy)) {

	    /* must be WWalking */
	    You(sit_message, "lava");
	    burn_away_slime();
	    if (likes_lava(youmonst.data) || (uarmf && itemhasappearance(uarmf, APP_HOT_BOOTS) ) || (uarmf && uarmf->oartifact == ART_VERY_WARM_TYPE) || (uamul && uamul->otyp == AMULET_OF_D_TYPE_EQUIPMENT) || FireImmunity || (powerfulimplants() && uimplant && uimplant->oartifact == ART_RUBBER_SHOALS) || Race_if(PM_PLAYER_SALAMANDER) || (uwep && uwep->oartifact == ART_EVERYTHING_MUST_BURN) || (uwep && uwep->oartifact == ART_MANUELA_S_PRACTICANT_TERRO) || (uarm && uarm->oartifact == ART_LAURA_CROFT_S_BATTLEWEAR) || (uarm && uarm->oartifact == ART_D_TYPE_EQUIPMENT) || (uarmc && uarmc->oartifact == ART_SCOOBA_COOBA) || (uarmf && uarmf->oartifact == ART_JOHANNA_S_RED_CHARM) ) {
		pline_The("lava feels warm.");
		return 1;
	    }
	    pline_The("lava burns you!");
	    if (Slimed) {
	       pline("The slime is burned away!");
	       Slimed = 0;
	    }
	    losehp(d((StrongFire_resistance ? 1 : Fire_resistance ? 2 : 10), 10),
		   "sitting on lava", KILLED_BY);

	} else if (is_ice(u.ux, u.uy)) {

	    You(sit_message, defsyms[S_ice].explanation);
	    if (!Cold_resistance) pline_The("ice feels cold.");

	} else if (typ == DRAWBRIDGE_DOWN) {

	    You(sit_message, "drawbridge");

	} else if(IS_WOODENTABLE(typ)) {
		pline("Sitting on a table isn't very fruitful.");

	} else if(IS_FARMLAND(typ)) {
		pline("Your ass gets dirty from the earthy farmland.");

	} else if(IS_MOUNTAIN(typ)) {
		pline("You enjoy the romantic view from the mountaintop.");

	} else if(IS_WELL(typ)) {
		pline("You sit down beside the well.");

	} else if(IS_SNOW(typ)) {
		pline("You sit on the fluffy snow.");

	} else if(IS_SAND(typ)) {
		pline("You sit on the soft sand.");

	} else if(IS_GRASSLAND(typ)) {
		pline("You sit on the soft grass.");

	} else if(IS_BUBBLES(typ)) {
		pline("You sit on a fleecy bubble.");

	} else if(IS_POISONEDWELL(typ)) {
		pline("You sit down beside the poisoned well.");

	} else if(IS_FOUNTAIN(typ)) {
		if (youmonst.data->mlet == S_BAD_COINS) { /* by GoldenIvy */
			You("toss yourself into the fountain.");
			if (rn2(2)) pline("Heads!"); /* this is purely cosmetical */
			else pline("Tails!");
		} else You(sit_message, "fountain");

	} else if(IS_PENTAGRAM(typ)) {
		You(sit_message, "pentagram");
		pline("Nothing happens. In order to interact with the pentagram, use #invoke.");

	} else if(IS_WAGON(typ)) {
		You("sit down beside the wagon and try to hide.");
		u.uundetected = TRUE;

	} else if(IS_STRAWMATTRESS(typ)) {
		You(sit_message, "mattress");
		pline("If for some weird reason you want to fall asleep, stay on the mattress tile for a while. But beware that this will not be a very pleasant sleep and monsters might try to mug you.");

	} else if(IS_CARVEDBED(typ)) {

		if (Sleep_resistance) {

			pline(FunnyHallu ? "It seems you drank too much coffee and therefore cannot sleep." : "You can't seem to fall asleep.");

		} else if (!Sleep_resistance && (moves < u.bedsleeping)) {

			You("don't feel sleepy yet.");

		} else if (!Sleep_resistance && (moves >= u.bedsleeping)) {

			u.cnd_bedamount++;
			u.bedsleeping = moves + 100;
			You("go to bed.");
			if (FunnyHallu) pline("Sleep-bundle-wing!");
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

			if (uarmf && uarmf->oartifact == ART_LARISSA_S_GENTLE_SLEEP) {
				pline((Role_if(PM_SAMURAI) || Role_if(PM_NINJA)) ? "Jikan ga teishi shimashita." : "Time has stopped.");
				TimeStopped += rnd(30);
			}

			if (uarmh && uarmh->oartifact == ART_SILVIA_S_DREAMBAND) {
				if (!level.flags.nommap) {
					pline("A map coalesces in your mind!");
					do_mappingZ();
				}
			}

			if (uarm && uarm->oartifact == ART_ROBE_OF_ELARION__THE_DREAM) {
				u.uhp = u.uhpmax;
				u.uen = u.uenmax;
				if (Upolyd) u.mh = u.mhmax;
				flags.botl = TRUE;
			}

		}

	} else if(IS_THRONE(typ)) {

	    You(sit_message, defsyms[S_throne].explanation);
	    u.cnd_throneamount++;
	    if (!rn2(2))  {

		if (uarmg && uarmg->oartifact == ART_FUMBLEFINGERS_QUEST) {

			{register int cnt = rnd(10);
			int randmonstforspawn = rnd(68);
			if (randmonstforspawn == 35) randmonstforspawn = 53;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(mkclass(randmonstforspawn,0), u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Oh, please help me! A horrible %s stole my sword! I'm nothing without it.", monexplain[randmonstforspawn]);
			}

		} else if (uarmg && uarmg->oartifact == ART_PRINCESS_BITCH) {

			{register int cnt = rnd(10);
			struct permonst *randmonstforspawn = rndmonst();

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(randmonstforspawn, u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Leave me alone, stupid %s", randmonstforspawn->mname);
			}

		} else

		switch (rnd(32))  {
		    case 1:
			(void) adjattrib(rn2(A_MAX), -rno(5), FALSE, TRUE);
			losehp(rnd(10), "cursed throne", KILLED_BY_AN);
			break;
		    case 2:
			(void) adjattrib(rn2(A_MAX), 1, FALSE, TRUE);
			break;
		    case 3:
			pline("A%s electric shock shoots through your body!",
			      (Shock_resistance) ? "n" : " massive");
			if (!ShockImmunity) losehp(StrongShock_resistance ? rnd(2) : Shock_resistance ? rnd(6) : rnd(30),
			       "electric chair", KILLED_BY_AN);
			exercise(A_CON, FALSE);
			break;
		    case 4:
			You_feel("much, much better!");
			if (Upolyd) {
			    if (u.mh >= (u.mhmax - 5))  u.mhmax += 4;
			    u.mh = u.mhmax;
			}
			if(u.uhp >= (u.uhpmax - 5))  u.uhpmax += 4;
			u.uhp = u.uhpmax;
			if (uinsymbiosis) {
				u.usymbiote.mhpmax += 4;
				maybe_evolve_symbiote();
				if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			}
			make_blinded(0L,TRUE);
			make_sick(0L, (char *) 0, FALSE, SICK_ALL);
			heal_legs();
			flags.botl = 1;
			break;
		    case 5:
			take_gold();
			break;
		    case 6:
/* ------------===========STEPHEN WHITE'S NEW CODE============------------ */                                                

			if (u.ulevel < 5) {
				You_feel("as if a stroke of good luck passed by.");
				break;
			}

			if (!rn2(4)) {

				if(u.uluck < 7) {
				    You_feel("your luck is changing.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Kha, vy ne poluchite zhelaniye, potomu chto eto Sovetskaya Rossiya, gde kazhdyy poluchayet odinakovoye kolichestvo zhelaniy! I vy uzhe boleye chem dostatochno, teper' ochered' drugikh personazhey'!" : "DSCHUEueUEueUEueUEueUEue...");
				    change_luck(5);
				} else	    makewish(evilfriday ? FALSE : TRUE);
			} else {
				othergreateffect();
			}

			break;
		    case 7:
			{
			register int cnt = rnd(10);

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			pline("A voice echoes:");
			verbalize("Thy audience hath been summoned, %s!",
				  flags.female ? "Dame" : "Sire");
			while(cnt--)
			    (void) makemon(courtmon(), u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			break;
			}
		    case 8:
			pline("A voice echoes:");
			verbalize("By thy Imperious order, %s...",
				  flags.female ? "Dame" : "Sire");
			do_genocide(5);	/* REALLY|ONTHRONE, see do_genocide() */
			break;
		    case 9:
			pline("A voice echoes:");
			verbalize("A curse upon thee for sitting upon this most holy throne!");
			if (Luck > 0)  {
			    make_blinded(Blinded + rn1(100,250),TRUE);
			} else	    rndcurse();
			break;
		    case 10:
			if (Luck < 0 || (HSee_invisible & INTRINSIC))  {
				if (level.flags.nommap) {
					pline(
					"A terrible drone fills your head!");
					make_confused(HConfusion + rnd(30),
									FALSE);
				} else {
					pline("An image forms in your mind.");
					do_mapping();
				}
			} else  {
				Your("vision becomes clear.");
				HSee_invisible |= FROMOUTSIDE;
				newsym(u.ux, u.uy);
			}
			break;
		    case 11:
			if (Luck < 0)  {
			    You_feel("threatened.");
			    aggravate();
			} else  {

			    You_feel("a wrenching sensation.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net nikakoy zashchity. Tam net nikakoy nadezhdy. Yedinstvennoye, chto yest'? Uverennost' v tom, chto vy, igrok, budet umeret' uzhasnoy i muchitel'noy smert'yu." : "SCHRING!");
			    tele();		/* teleport him */
			}
			break;
		    case 12:
			You("are granted an insight!");
			if (invent) {
			    /* rn2(5) agrees w/seffects() */
			    identify_pack(rn2(5), 0, 0);
			}
			break;
		    case 13:
			Your("mind turns into a pretzel!");
			make_confused(HConfusion + rn1(7,16),FALSE);
			break;
		    case 14:
			You("are granted some new skills!"); /* new effect that unrestricts skills --Amy */
			unrestrictskillchoice();
			break;
		    case 15:
			/* occasionally get extremely lucky --Amy */
			if (!rn2(50)) {
				u.weapon_slots++;
				You("feel very skillful, and gain an extra skill slot!");
				break;
			}
			pline("A voice echoes:");
			verbalize("Thou be cursed!");
			attrcurse();
			break;
		    case 16:
			pline("A voice echoes:");
			verbalize("Thou shall be punished!");
			punishx();
			break;
		    case 17:
			You_feel("like someone has touched your forehead...");

			int skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else gainlevelmaybe();

			if (Race_if(PM_RUSMOT)) {
				if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
					unrestrict_weapon_skill(skillimprove);
					pline("You can now learn the %s skill.", wpskillname(skillimprove));
				} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
					unrestrict_weapon_skill(skillimprove);
					P_MAX_SKILL(skillimprove) = P_BASIC;
					pline("You can now learn the %s skill.", wpskillname(skillimprove));
				} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
					P_MAX_SKILL(skillimprove) = P_SKILLED;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
					P_MAX_SKILL(skillimprove) = P_EXPERT;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
					P_MAX_SKILL(skillimprove) = P_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
					P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
					P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				}
			}

			gainlevelmaybe();

			break;
		    case 18:
			{register int cnt = rnd(10);
			struct permonst *randmonstforspawn = rndmonst();

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(randmonstforspawn, u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Leave me alone, stupid %s", randmonstforspawn->mname);
			break;
			}
		    case 19:
			{register int cnt = rnd(10);
			int randmonstforspawn = rnd(68);
			if (randmonstforspawn == 35) randmonstforspawn = 53;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(mkclass(randmonstforspawn,0), u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Oh, please help me! A horrible %s stole my sword! I'm nothing without it.", monexplain[randmonstforspawn]);
			break;
			}
		    case 20:
			{
			if (CannotSelectItemsInPrompts) break;
			pline("You may fully identify an object!");

secureidchoice:
			otmp = getobj(allnoncount, "secure identify");

			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to secure identify your objects.");
				else goto secureidchoice;
				pline("A feeling of loss comes over you.");
				break;
			}
			if (otmp) {
				makeknown(otmp->otyp);
				if (otmp->oartifact) discover_artifact((int)otmp->oartifact);
				otmp->known = otmp->dknown = otmp->bknown = otmp->rknown = 1;
				if (otmp->otyp == EGG && otmp->corpsenm != NON_PM)
				learn_egg_type(otmp->corpsenm);
				prinv((char *)0, otmp, 0L);
			}
			}
			break;
		    case 21:
			{
				int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
				if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
				int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
				if (!blackngdur ) blackngdur = 500; /* fail safe */
				randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), (blackngdur - (monster_difficulty() * 3)));
				You_feel("uncomfortable.");
			}
			break;
		    case 22:
			morehungry(500);
			pline("Whoops... suddenly you feel hungry.");
			break;
		    case 23:
			pline("Suddenly you feel a healing touch!");
			reducesanity(100);
			break;
		    case 24:
			poisoned("throne", rn2(6) /* A_STR ... A_CHA*/, "poisoned throne", 30);
			break;
		    case 25:
			{
				int thronegold = rnd(200);
				u.ugold += thronegold;
				pline("Some coins come loose! You pick up %d zorkmids.", thronegold);
			}
			break;
		    case 26:
			{

				if (Aggravate_monster) {
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
				}

				pline("A voice echoes:");
				verbalize("Thou hath been summoned to appear before royalty, %s!", playeraliasname);
				(void) makemon(specialtensmon(rn2(2) ? 105 : 106), u.ux, u.uy, MM_ANGRY); /* M2_LORD, M2_PRINCE */

				u.aggravation = 0;

			}
			break;
		    case 27:
			badeffect();
			break;
		    case 28:
			u.uhp++;
			u.uhpmax++;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			You_feel("a health boost!");
			break;
		    case 29:
			pline("A pretty ethereal woman appears and offers: 'For only 10000 zorkmids, I will give you a very rare trinket!");
			if (u.ugold < 10000) {
				pline("But you don't have enough money! Frustrated, she places a terrible curse on you and disappears.");
				randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));
			}
			else {
				char femhandlebuf[BUFSZ];
				getlin ("Do you want to buy her goods? [y/yes/no]",femhandlebuf);
				(void) lcase (femhandlebuf);
				if (!(strcmp (femhandlebuf, "yes")) || !(strcmp (femhandlebuf, "y")) || !(strcmp (femhandlebuf, "ye")) || !(strcmp (femhandlebuf, "ys"))) {
					u.ugold -= 10000;
					register struct obj *acqo;
					acqo = mksobj(makegreatitem(), TRUE, TRUE, FALSE);
					if (acqo) {
						dropy(acqo);
						verbalize("Thanks a lot! You'll find your prize on the ground.");
					} else {
						verbalize("Oh sorry, I must have misplaced it. Here you have your money back. Maybe next time I'll have something for you.");
						u.ugold += 10000;
					}
				} else {
					verbalize("You will regret that decision!");
					randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));

				}
			}
			break;
		    case 30:
			pline("A shady merchant appears and offers: 'Sale! Sale! I'm selling you this useful item for 2000 zorkmids!");
			if (u.ugold < 2000) {
				pline("But you don't have enough money! The merchant disappears.");
			}
			else {
				char femhandlebuf[BUFSZ];
				getlin ("Do you want to buy his item? [y/yes/no]",femhandlebuf);
				(void) lcase (femhandlebuf);
				if (!(strcmp (femhandlebuf, "yes")) || !(strcmp (femhandlebuf, "y")) || !(strcmp (femhandlebuf, "ye")) || !(strcmp (femhandlebuf, "ys"))) {
					u.ugold -= 2000;
					register struct obj *acqo;
					acqo = mksobj(usefulitem(), TRUE, TRUE, FALSE);
					if (acqo) {
						dropy(acqo);
						verbalize("Thank you! I've dropped the item at your feet.");
					} else {
						verbalize("Nyah-nyah, thanks for the money, sucker!");
					}
				} else {
					verbalize("Are you sure? Well, it's your decision. I'll find someone else to sell it to, then.");
				}
			}
			break;
		    case 31:
			{
				struct obj *stupidstone;
				stupidstone = mksobj_at(rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE), u.ux, u.uy, TRUE, FALSE, FALSE);
				if (stupidstone) {
					stupidstone->quan = 1L;
					stupidstone->owt = weight(stupidstone);
					if (!Blind) stupidstone->dknown = 1;
					if (stupidstone) {
						pline("%s lands in your knapsack!", Doname2(stupidstone));
						(void) pickup_object(stupidstone, 1L, TRUE, TRUE);
					}
				}
			}
			break;
		    case 32:
			(void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL, ROCK), u.ux, u.uy, TRUE, TRUE, FALSE);
			pline("Some stones come loose!");
			break;

		    default:	impossible("throne effect");
				break;
		}
	    } else {
		if (is_prince(youmonst.data))
		    You_feel("very comfortable here.");
		else
		    You_feel("somehow out of place...");
	    }

	    if (u.ualign.type == A_CHAOTIC) adjalign(1);

	    if (!rn2(6) && IS_THRONE(levl[u.ux][u.uy].typ)) {

		boolean thronewillvanish = TRUE;

		if (wizard) {
			if (yn("Make the throne vanish?") == 'n')
				thronewillvanish = FALSE;
		}

		/* may have teleported */

		if (thronewillvanish) {

			levl[u.ux][u.uy].typ = ROOM;
			pline_The("throne vanishes in a puff of logic.");
			newsym(u.ux,u.uy);
		}
	    }

	} else if (lays_eggs(youmonst.data)) {
		struct obj *uegg;

		if (monsndx(youmonst.data) >= NUMMONS) {
			You("can't lay eggs as a missingno because they would crash the game!");
			return 0;
		}

		if (!flags.female) {
			pline(FunnyHallu ? "You try to lay an egg, but instead you... okay let's not go there." : "Males can't lay eggs!");
			return 0;
		}

		if (u.uhunger < (int)objects[EGG].oc_nutrition) {
			You("don't have enough energy to lay an egg.");
			return 0;
		}

		if (u.egglayingtimeout) {
			pline("You need to wait %d turns to lay another egg!", u.egglayingtimeout);
			return 0;
		}

		uegg = mksobj(EGG, FALSE, FALSE, FALSE);
		if (uegg) {
			uegg->spe = 1;
			uegg->quan = 1;
			uegg->owt = weight(uegg);
			uegg->corpsenm = egg_type_from_parent(u.umonnum, FALSE);
			uegg->known = uegg->dknown = 1;
			attach_egg_hatch_timeout(uegg);
			You("lay an egg.");
			dropy(uegg);
			stackobj(uegg);
			morehungry((int)objects[EGG].oc_nutrition);
			u.egglayingtimeout = rnz(1000);
			if (!PlayerCannotUseSkills) {
				switch (P_SKILL(P_SQUEAKING)) {
			      	case P_BASIC:	u.egglayingtimeout *= 9; u.egglayingtimeout /= 10; break;
			      	case P_SKILLED:	u.egglayingtimeout *= 8; u.egglayingtimeout /= 10; break;
			      	case P_EXPERT:	u.egglayingtimeout *= 7; u.egglayingtimeout /= 10; break;
			      	case P_MASTER:	u.egglayingtimeout *= 6; u.egglayingtimeout /= 10; break;
			      	case P_GRAND_MASTER:	u.egglayingtimeout *= 5; u.egglayingtimeout /= 10; break;
			      	case P_SUPREME_MASTER:	u.egglayingtimeout *= 4; u.egglayingtimeout /= 10; break;
			      	default: break;

				}
			}
			pline("You will be able to lay another egg in %d turns.", u.egglayingtimeout);
			use_skill(P_SQUEAKING, rnd(20));
		}
	} else if (u.uswallow)
		There("are no seats in here!");
	else
		pline("Having fun sitting on the %s?", surface(u.ux,u.uy));
	return(1);
}

/* the cap of a skill has been changed: now make sure the skill level isn't higher than the cap --Amy */
void
skill_sanity_check(skilltocheck)
int skilltocheck;
{
	int tryct = 2000;
	int i = 0;

	/* if you have spent skill points, undo them until the skill in question isn't higher than the cap anymore */
	while (u.skills_advanced && tryct && ((P_SKILL(skilltocheck) > P_MAX_SKILL(skilltocheck)) || (P_SKILL(skilltocheck) >= P_BASIC && P_ADVANCE(skilltocheck) < (practice_needed_to_advance_nonmax(P_SKILL(skilltocheck) - 1, skilltocheck)) )) ) {

		lose_last_spent_skill();
		i++;
		tryct--;
	}

	/* refund the skill slots */
	while (i) {
		if (evilfriday && !u.evilvarhack) {
			pline("This is the evil variant. Your skill point is lost forever.");
			u.evilvarhack = TRUE;
		} else u.weapon_slots++;  /* because every skill up costs one slot --Amy */
		i--;
	}

	/* did a skill become restricted? then being unskilled should result in becoming restricted */
	if (P_SKILL(skilltocheck) == P_UNSKILLED && P_MAX_SKILL(skilltocheck) == P_ISRESTRICTED) {
		P_SKILL(skilltocheck) = P_ISRESTRICTED;
	}

	/* still higher than the cap? that probably means you started with some knowledge of the skill... */
	if ((P_SKILL(skilltocheck) > P_MAX_SKILL(skilltocheck)) || (P_SKILL(skilltocheck) >= P_BASIC && P_ADVANCE(skilltocheck) < (practice_needed_to_advance_nonmax(P_SKILL(skilltocheck) - 1, skilltocheck)) ) ) {

		int skillamount = 0;
		while ((P_SKILL(skilltocheck) > P_MAX_SKILL(skilltocheck)) || (P_SKILL(skilltocheck) >= P_BASIC && P_ADVANCE(skilltocheck) < (practice_needed_to_advance_nonmax(P_SKILL(skilltocheck) - 1, skilltocheck)) ) ) {
			if (P_SKILL(skilltocheck) == P_UNSKILLED && P_MAX_SKILL(skilltocheck) == P_ISRESTRICTED) {
				P_SKILL(skilltocheck) = P_ISRESTRICTED;
			} else {
				P_SKILL(skilltocheck) -= 1;
				skillamount++;
			}
		}

		while (skillamount > 0) {
			skillamount--;
			if (evilfriday && !u.evilvarhack) {
				pline("This is the evil variant. Your skill point is lost forever.");
				u.evilvarhack = TRUE;
			} else u.weapon_slots++;
		}
	}

}

void
skillcaploss()
{

	int skilltoreduce = randomgoodskill();

	if (P_RESTRICTED(skilltoreduce)) return; /* nothing to do */

	if (P_MAX_SKILL(skilltoreduce) == P_BASIC) {
		P_MAX_SKILL(skilltoreduce) = P_ISRESTRICTED;
		pline("You lose all knowledge of the %s skill!", wpskillname(skilltoreduce));
		P_ADVANCE(skilltoreduce) = 0;
	} else if (!rn2(2) && P_MAX_SKILL(skilltoreduce) == P_SKILLED) {
		P_MAX_SKILL(skilltoreduce) = P_BASIC;
		pline("You lose some knowledge of the %s skill!", wpskillname(skilltoreduce));
	} else if (!rn2(4) && P_MAX_SKILL(skilltoreduce) == P_EXPERT) {
		P_MAX_SKILL(skilltoreduce) = P_SKILLED;
		pline("You lose some knowledge of the %s skill!", wpskillname(skilltoreduce));
	} else if (!rn2(10) && P_MAX_SKILL(skilltoreduce) == P_MASTER) {
		P_MAX_SKILL(skilltoreduce) = P_EXPERT;
		pline("You lose some knowledge of the %s skill!", wpskillname(skilltoreduce));
	} else if (!rn2(100) && P_MAX_SKILL(skilltoreduce) == P_GRAND_MASTER) {
		P_MAX_SKILL(skilltoreduce) = P_MASTER;
		pline("You lose some knowledge of the %s skill!", wpskillname(skilltoreduce));
	} else if (!rn2(200) && P_MAX_SKILL(skilltoreduce) == P_SUPREME_MASTER) {
		P_MAX_SKILL(skilltoreduce) = P_GRAND_MASTER;
		pline("You lose some knowledge of the %s skill!", wpskillname(skilltoreduce));
	}

	skill_sanity_check(skilltoreduce);

	return;

}

void
skillcaploss_specific(skilltoreduce)
int skilltoreduce;
{

	if (P_RESTRICTED(skilltoreduce)) return; /* nothing to do */

	if (P_MAX_SKILL(skilltoreduce) >= P_BASIC) {
		P_MAX_SKILL(skilltoreduce) = P_ISRESTRICTED;
	}

	skill_sanity_check(skilltoreduce);

	P_SKILL(skilltoreduce) = P_ISRESTRICTED;

	return;

}

/* skill cap loss trap: slowly but steadily reduces training of all skills --Amy */
void
skillcaploss_severe()
{
	int skilltoreduce;
	int tryct, tryct2;
	int lossamount;

	skilltoreduce = P_DAGGER;

severelossagain:

	if (skilltoreduce < 0) return; /* fail safe, should never happen */

	int i = 0;

	/* 1 in 1000 chance per skill to be selected; if they do get selected, 1 in 1000 chance to lose all knowledge */
	if (rn2(1000)) {
		skilltoreduce++;
		if (skilltoreduce >= P_NUM_SKILLS) return;
		else goto severelossagain;
	} else if (rn2(1000)) lossamount = 1;
	else lossamount = 9999999;

	if ((P_ADVANCE(skilltoreduce)) < lossamount) P_ADVANCE(skilltoreduce) = 0;
	else P_ADVANCE(skilltoreduce) -= lossamount;

	if (!P_RESTRICTED(skilltoreduce)) {

		skill_sanity_check(skilltoreduce);

	}

	skilltoreduce++;
	if (skilltoreduce >= P_NUM_SKILLS) return;
	else goto severelossagain;

}

void
rndcurse()			/* curse a few inventory items at random! */
{
	int	nobj = 0;
	int	cnt, onum;
	struct	obj	*otmp;
	static const char mal_aura[] = "feel a malignant aura surround %s.";

	int verymanyitems;
	int nobjtempvar;

	if (uwep && (uwep->oartifact == ART_MAGICBANE) && rn2(20)) {
	    You(mal_aura, "the magic-absorbing blade");
	    return;
	}

	if (!rn2(5) && uarmh && itemhasappearance(uarmh, APP_SAGES_HELMET) ) {
		pline("A malignant aura surrounds you but is absorbed by the sages helmet!");
		return;
	}

	if (Versus_curses && rn2(StrongVersus_curses ? 20 : 4)) { /* curse resistance, by Chris_ANG */
		pline("A malignant aura surrounds you but is absorbed by your magical shield!");
	    return;
	}

	if(u.ukinghill && rn2(20)){
	    You(mal_aura, "the cursed treasure chest");
		otmp = 0;
		for(otmp = invent; otmp; otmp=otmp->nobj)
			if(otmp->oartifact == ART_TREASURY_OF_PROTEUS)
				break;
		if(!otmp) pline("Treasury not actually in inventory??");
		else if(otmp->blessed)
			unbless(otmp);
		else
			curse(otmp);
	    update_inventory();		
		return;
	}

	u.cnd_curseitemsamount++;

	if(Antimagic) {
	    shieldeff(u.ux, u.uy);
	    You(mal_aura, "you");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Ne vse blagopoluchno ot zlosloviya, i vy poluchite dopolnitel'noye soobshcheniye bespoleznuyu nazlo vam!" : "Due-due-duennn-nnnnn!");
	}

	for (otmp = invent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
	    /* gold isn't subject to being cursed or blessed */
	    if (otmp->oclass == COIN_CLASS) continue;
#endif
	    nobj++;
	}

	/* it's lame if you split 200 rocks to catch curses... gotta put a stop to that --Amy */
	verymanyitems = 6;
	nobjtempvar = 0;
	if (nobj > 50) {
		nobjtempvar = nobj;
		while (nobjtempvar > 50) {
			verymanyitems++;
			nobjtempvar -= 10;
		}
	}

	if (uinsymbiosis) {
		int symcurchance = 5;
		if (Antimagic) symcurchance += 2;
		if (StrongAntimagic) symcurchance += 2;
		if (Half_spell_damage) symcurchance += 3;
		if (StrongHalf_spell_damage) symcurchance += 3;
		if (isfriday) symcurchance /= 2;

		if (!rn2(symcurchance)) {
			cursesymbiote();
			if (FunnyHallu) pline("You feel like you have a %s attack!", body_part(HEART));
			else pline("Your symbiote feels deathly cold!");
		}
	}

	if (isfriday) verymanyitems *= 2;
	if (StrongAntimagic && verymanyitems > 1) verymanyitems /= 2;
	if (StrongHalf_spell_damage && verymanyitems > 1) verymanyitems /= 2;

	if (nobj) {
	    for (cnt = rnd(verymanyitems/((!!Antimagic) + (!!Half_spell_damage) + 1));
		 cnt > 0; cnt--)  {
		onum = rnd(nobj);
		for (otmp = invent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
		    /* as above */
		    if (otmp->oclass == COIN_CLASS) continue;
#endif
		    if (--onum == 0) break;	/* found the target */
		}
		/* the !otmp case should never happen; picking an already
		   cursed item happens--avoid "resists" message in that case
		   Amy edit: only prime cursed items can't be cursed further
		   edit again: now they can, since we have even more severe curses */
		if (!otmp) continue;	/* next target */

		if(otmp->oartifact && spec_ability(otmp, SPFX_INTEL) &&
		   rn2(10) < 8) {
		    pline("%s!", Tobjnam(otmp, "resist"));
		    continue;
		}

		/* materials overhaul: gold resists curses --Amy */
		if (objects[otmp->otyp].oc_material == MT_GOLD && rn2(2)) {
		    pline("%s!", Tobjnam(otmp, "resist"));
		    continue;
		}

		if (!stack_too_big(otmp)) {
		if(otmp->blessed)
			unbless(otmp);
		else
			curse(otmp);
		}
	    }
	    update_inventory();
	}

	/* treat steed's saddle as extended part of hero's inventory */
	if (u.usteed && !rn2(50) &&
		(otmp = which_armor(u.usteed, W_SADDLE)) != 0) {
	    if (otmp->blessed)
		unbless(otmp);
	    else
		curse(otmp);
	    if (!Blind) {
		pline("%s %s %s.",
		      s_suffix(upstart(y_monnam(u.usteed))),
		      aobjnam(otmp, "glow"),
		      hcolor(otmp->cursed ? NH_BLACK : (const char *)"brown"));
		otmp->bknown = TRUE;
	    }
	}
}

void
attrcurse()			/* remove a random INTRINSIC ability */
{
	if (chitinprotection() && !rn2(3)) {
		You("resist the intrinsic loss!");
		return;
	}
	if (MysteryResist && !rn2(3)) {
		You("resist the intrinsic loss!");
		return;
	}
	if (StrongMysteryResist && !rn2(3)) {
		You("resist the intrinsic loss!");
		return;
	}

	switch(rnd(266)) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15: if (HFire_resistance & INTRINSIC) {
			HFire_resistance &= ~INTRINSIC;
			You_feel("warmer.");
			u.cnd_intrinsiclosscount++;
		}
		if (HFire_resistance & TIMEOUT) {
			HFire_resistance &= ~TIMEOUT;
			You_feel("warmer.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23: if (HTeleportation & INTRINSIC) {
			HTeleportation &= ~INTRINSIC;
			You_feel("less jumpy.");
			u.cnd_intrinsiclosscount++;
		}
		if (HTeleportation & TIMEOUT) {
			HTeleportation &= ~TIMEOUT;
			You_feel("less jumpy.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 38: if (HPoison_resistance & INTRINSIC) {
			HPoison_resistance &= ~INTRINSIC;
			You_feel("a little sick!");
			u.cnd_intrinsiclosscount++;
		}
		if (HPoison_resistance & TIMEOUT) {
			HPoison_resistance &= ~TIMEOUT;
			You_feel("a little sick!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 39:
	case 40:
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
	case 48: if (HTelepat & INTRINSIC) {
			HTelepat &= ~INTRINSIC;
			if (Blind && !Blind_telepat)
			    see_monsters();	/* Can't sense mons anymore! */
			Your("senses fail!");
			u.cnd_intrinsiclosscount++;
		}
		if (HTelepat & TIMEOUT) {
			HTelepat &= ~TIMEOUT;
			if (Blind && !Blind_telepat)
			    see_monsters();	/* Can't sense mons anymore! */
			Your("senses fail!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
	case 58:
	case 59:
	case 60:
	case 61:
	case 62:
	case 63: if (HCold_resistance & INTRINSIC) {
			HCold_resistance &= ~INTRINSIC;
			You_feel("cooler.");
			u.cnd_intrinsiclosscount++;
		}
		if (HCold_resistance & TIMEOUT) {
			HCold_resistance &= ~TIMEOUT;
			You_feel("cooler.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 64:
	case 65:
	case 66:
	case 67:
	case 68:
	case 69:
	case 70:
	case 71:
	case 72:
	case 73: if (HInvis & INTRINSIC) {
			HInvis &= ~INTRINSIC;
			You_feel("paranoid.");
			u.cnd_intrinsiclosscount++;
		}
		if (HInvis & TIMEOUT) {
			HInvis &= ~TIMEOUT;
			You_feel("paranoid.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 74:
	case 75:
	case 76:
	case 77:
	case 78:
	case 79:
	case 80:
	case 81:
	case 82:
	case 83: if (HSee_invisible & INTRINSIC) {
			HSee_invisible &= ~INTRINSIC;
			You("%s!", FunnyHallu ? "tawt you taw a puttie tat" : "thought you saw something");
			u.cnd_intrinsiclosscount++;
		}
		if (HSee_invisible & TIMEOUT) {
			HSee_invisible &= ~TIMEOUT;
			You("%s!", FunnyHallu ? "tawt you taw a puttie tat" : "thought you saw something");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 84:
	case 85:
	case 86:
	case 87:
	case 88:
	case 89:
	case 90:
	case 91:
	case 92:
	case 93: if (HFast & INTRINSIC) {
			HFast &= ~INTRINSIC;
			You_feel("slower.");
			u.cnd_intrinsiclosscount++;
		}
		if (HFast & TIMEOUT) {
			HFast &= ~TIMEOUT;
			You_feel("slower.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 94:
	case 95:
	case 96:
	case 97:
	case 98:
	case 99:
	case 100:
	case 101:
	case 102:
	case 103: if (HStealth & INTRINSIC) {
			HStealth &= ~INTRINSIC;
			You_feel("clumsy.");
			u.cnd_intrinsiclosscount++;
		}
		if (HStealth & TIMEOUT) {
			HStealth &= ~TIMEOUT;
			You_feel("clumsy.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 104: if (HProtection & INTRINSIC) {
			HProtection &= ~INTRINSIC;
			You_feel("vulnerable.");
			u.cnd_intrinsiclosscount++;
		}
		if (HProtection & TIMEOUT) {
			HProtection &= ~TIMEOUT;
			You_feel("vulnerable.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 105:
	case 106:
	case 107:
	case 108:
	case 109:
	case 110:
	case 111:
	case 112: if (HAggravate_monster & INTRINSIC) {
			HAggravate_monster &= ~INTRINSIC;
			You_feel("less attractive.");
			u.cnd_intrinsiclosscount++;
		}
		if (HAggravate_monster & TIMEOUT) {
			HAggravate_monster &= ~TIMEOUT;
			You_feel("less attractive.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 113:
	case 114:
	case 115:
	case 116:
	case 117:
	case 118:
	case 119:
	case 120:
	case 121:
	case 122:
	case 123:
	case 124:
	case 125:
	case 126:
	case 127: if (HSleep_resistance & INTRINSIC) {
			HSleep_resistance &= ~INTRINSIC;
			You_feel("tired all of a sudden.");
			u.cnd_intrinsiclosscount++;
		}
		if (HSleep_resistance & TIMEOUT) {
			HSleep_resistance &= ~TIMEOUT;
			You_feel("tired all of a sudden.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 128:
	case 129:
	case 130:
	case 131:
	case 132:
	case 133:
	case 134:
	case 135:
	case 136:
	case 137: if (HDisint_resistance & INTRINSIC) {
			HDisint_resistance &= ~INTRINSIC;
			You_feel("like you're going to break apart.");
			u.cnd_intrinsiclosscount++;
		}
		if (HDisint_resistance & TIMEOUT) {
			HDisint_resistance &= ~TIMEOUT;
			You_feel("like you're going to break apart.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 138:
	case 139:
	case 140:
	case 141:
	case 142:
	case 143:
	case 144:
	case 145:
	case 146:
	case 147:
	case 148:
	case 149:
	case 150:
	case 151:
	case 152: if (HShock_resistance & INTRINSIC) {
			HShock_resistance &= ~INTRINSIC;
			You_feel("like someone has zapped you.");
			u.cnd_intrinsiclosscount++;
		}
		if (HShock_resistance & TIMEOUT) {
			HShock_resistance &= ~TIMEOUT;
			You_feel("like someone has zapped you.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 153: if (HDrain_resistance & INTRINSIC) {
			HDrain_resistance &= ~INTRINSIC;
			You_feel("like someone is sucking out your life-force.");
			u.cnd_intrinsiclosscount++;
		}
		if (HDrain_resistance & TIMEOUT) {
			HDrain_resistance &= ~TIMEOUT;
			You_feel("like someone is sucking out your life-force.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 154: if (HSick_resistance & INTRINSIC) {
			HSick_resistance &= ~INTRINSIC;
			You_feel("no longer immune to diseases!");
			u.cnd_intrinsiclosscount++;
		}
		if (HSick_resistance & TIMEOUT) {
			HSick_resistance &= ~TIMEOUT;
			You_feel("no longer immune to diseases!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 155:
	case 156:
	case 157: if (HWarning & INTRINSIC) {
			HWarning &= ~INTRINSIC;
			You_feel("that your radar has just stopped working!");
			u.cnd_intrinsiclosscount++;
		}
		if (HWarning & TIMEOUT) {
			HWarning &= ~TIMEOUT;
			You_feel("that your radar has just stopped working!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 158:
	case 159:
	case 160:
	case 161:
	case 162:
	case 163:
	case 164: if (HSearching & INTRINSIC) {
			HSearching &= ~INTRINSIC;
			You_feel("unable to find something you lost!");
			u.cnd_intrinsiclosscount++;
		}
		if (HSearching & TIMEOUT) {
			HSearching &= ~TIMEOUT;
			You_feel("unable to find something you lost!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 165: if (HClairvoyant & INTRINSIC) {
			HClairvoyant &= ~INTRINSIC;
			You_feel("a loss of mental capabilities!");
			u.cnd_intrinsiclosscount++;
		}
		if (HClairvoyant & TIMEOUT) {
			HClairvoyant &= ~TIMEOUT;
			You_feel("a loss of mental capabilities!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 166: if (HInfravision & INTRINSIC) {
			HInfravision &= ~INTRINSIC;
			You_feel("shrouded in darkness.");
			u.cnd_intrinsiclosscount++;
		}
		if (HInfravision & TIMEOUT) {
			HInfravision &= ~TIMEOUT;
			You_feel("shrouded in darkness.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 167: if (HDetect_monsters & INTRINSIC) {
			HDetect_monsters &= ~INTRINSIC;
			You_feel("that you can no longer sense monsters.");
			u.cnd_intrinsiclosscount++;
		}
		if (HDetect_monsters & TIMEOUT) {
			HDetect_monsters &= ~TIMEOUT;
			You_feel("that you can no longer sense monsters.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 168: if (HJumping & INTRINSIC) {
			HJumping &= ~INTRINSIC;
			You_feel("your legs shrinking.");
			u.cnd_intrinsiclosscount++;
		}
		if (HJumping & TIMEOUT) {
			HJumping &= ~TIMEOUT;
			You_feel("your legs shrinking.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 169:
	case 170:
	case 171:
	case 172:
	case 173:
	case 174:
	case 175:
	case 176:
	case 177:
	case 178: if (HTeleport_control & INTRINSIC) {
			HTeleport_control &= ~INTRINSIC;
			You_feel("unable to control where you're going.");
			u.cnd_intrinsiclosscount++;
		}
		if (HTeleport_control & TIMEOUT) {
			HTeleport_control &= ~TIMEOUT;
			You_feel("unable to control where you're going.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 179: if (HMagical_breathing & INTRINSIC) {
			HMagical_breathing &= ~INTRINSIC;
			You_feel("you suddenly need to breathe!");
			u.cnd_intrinsiclosscount++;
		}
		if (HMagical_breathing & TIMEOUT) {
			HMagical_breathing &= ~TIMEOUT;
			You_feel("you suddenly need to breathe!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 180: if (HRegeneration & INTRINSIC) {
			HRegeneration &= ~INTRINSIC;
			You_feel("your wounds are healing slower!");
			u.cnd_intrinsiclosscount++;
		}
		if (HRegeneration & TIMEOUT) {
			HRegeneration &= ~TIMEOUT;
			You_feel("your wounds are healing slower!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 181: if (HEnergy_regeneration & INTRINSIC) {
			HEnergy_regeneration &= ~INTRINSIC;
			You_feel("a loss of mystic power!");
			u.cnd_intrinsiclosscount++;
		}
		if (HEnergy_regeneration & TIMEOUT) {
			HEnergy_regeneration &= ~TIMEOUT;
			You_feel("a loss of mystic power!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 182: if (HPolymorph & INTRINSIC) {
			HPolymorph &= ~INTRINSIC;
			You_feel("unable to change form!");
			u.cnd_intrinsiclosscount++;
		}
		if (HPolymorph & TIMEOUT) {
			HPolymorph &= ~TIMEOUT;
			You_feel("unable to change form!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 183: if (HPolymorph_control & INTRINSIC) {
			HPolymorph_control &= ~INTRINSIC;
			You_feel("less control over your own body.");
			u.cnd_intrinsiclosscount++;
		}
		if (HPolymorph_control & TIMEOUT) {
			HPolymorph_control &= ~TIMEOUT;
			You_feel("less control over your own body.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 184 : 
	case 185 : 
	case 186 : 
	case 187: if (HAcid_resistance & INTRINSIC) {
			HAcid_resistance &= ~INTRINSIC;
			You_feel("worried about corrosion!");
			u.cnd_intrinsiclosscount++;
		}
		if (HAcid_resistance & TIMEOUT) {
			HAcid_resistance &= ~TIMEOUT;
			You_feel("worried about corrosion!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 188: if (HFumbling & INTRINSIC) {
			HFumbling &= ~INTRINSIC;
			You_feel("less clumsy.");
			u.cnd_intrinsiclosscount++;
		}
		if (HFumbling & TIMEOUT) {
			HFumbling &= ~TIMEOUT;
			You_feel("less clumsy.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 189: if (HSleeping & INTRINSIC) {
			HSleeping &= ~INTRINSIC;
			You_feel("like you just had some coffee.");
			u.cnd_intrinsiclosscount++;
		}
		if (HSleeping & TIMEOUT) {
			HSleeping &= ~TIMEOUT;
			You_feel("like you just had some coffee.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 190: if (HHunger & INTRINSIC) {
			HHunger &= ~INTRINSIC;
			You_feel("like you just ate a chunk of meat.");
			u.cnd_intrinsiclosscount++;
		}
		if (HHunger & TIMEOUT) {
			HHunger &= ~TIMEOUT;
			You_feel("like you just ate a chunk of meat.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 191: if (HConflict & INTRINSIC) {
			HConflict &= ~INTRINSIC;
			You_feel("more acceptable.");
			u.cnd_intrinsiclosscount++;
		}
		if (HConflict & TIMEOUT) {
			HConflict &= ~TIMEOUT;
			You_feel("more acceptable.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 192: if (HSlow_digestion & INTRINSIC) {
			HSlow_digestion &= ~INTRINSIC;
			You_feel("like you're burning calories faster.");
			u.cnd_intrinsiclosscount++;
		}
		if (HSlow_digestion & TIMEOUT) {
			HSlow_digestion &= ~TIMEOUT;
			You_feel("like you're burning calories faster.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 193: if (HFlying & INTRINSIC) {
			HFlying &= ~INTRINSIC;
			You_feel("like you just lost your wings!");
			u.cnd_intrinsiclosscount++;
		}
		if (HFlying & TIMEOUT) {
			HFlying &= ~TIMEOUT;
			You_feel("like you just lost your wings!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 194: if (HPasses_walls & INTRINSIC) {
			HPasses_walls &= ~INTRINSIC;
			You_feel("less ethereal!");
			u.cnd_intrinsiclosscount++;
		}
		if (HPasses_walls & TIMEOUT) {
			HPasses_walls &= ~TIMEOUT;
			You_feel("less ethereal!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 195: if (HAntimagic & INTRINSIC) {
			HAntimagic &= ~INTRINSIC;
			You_feel("less protected from magic!");
			u.cnd_intrinsiclosscount++;
		}
		if (HAntimagic & TIMEOUT) {
			HAntimagic &= ~TIMEOUT;
			You_feel("less protected from magic!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 196: if (HReflecting & INTRINSIC) {
			HReflecting &= ~INTRINSIC;
			You_feel("less reflexive!");
			u.cnd_intrinsiclosscount++;
		}
		if (HReflecting & TIMEOUT) {
			HReflecting &= ~TIMEOUT;
			You_feel("less reflexive!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 197: if (Blinded & INTRINSIC) {
			Blinded &= ~INTRINSIC;
			You_feel("visually clear!");
			u.cnd_intrinsiclosscount++;
		}
		if (Blinded & TIMEOUT) {
			Blinded &= ~TIMEOUT;
			You_feel("visually clear!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 198: if (Glib & INTRINSIC) {
			Glib &= ~INTRINSIC;
			You_feel("heavy-handed!");
			u.cnd_intrinsiclosscount++;
		}
		if (Glib & TIMEOUT) {
			Glib &= ~TIMEOUT;
			You_feel("heavy-handed!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 199: if (HSwimming & INTRINSIC) {
			HSwimming &= ~INTRINSIC;
			You_feel("less aquatic!");
			u.cnd_intrinsiclosscount++;
		}
		if (HSwimming & TIMEOUT) {
			HSwimming &= ~TIMEOUT;
			You_feel("less aquatic!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 200: if (HNumbed & INTRINSIC) {
			HNumbed &= ~INTRINSIC;
			You_feel("your body parts relax.");
			u.cnd_intrinsiclosscount++;
		}
		if (HNumbed & TIMEOUT) {
			HNumbed &= ~TIMEOUT;
			You_feel("your body parts relax.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 201: if (HFree_action & INTRINSIC) {
			HFree_action &= ~INTRINSIC;
			You_feel("a loss of freedom!");
			u.cnd_intrinsiclosscount++;
		}
		if (HFree_action & TIMEOUT) {
			HFree_action &= ~TIMEOUT;
			You_feel("a loss of freedom!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 202: if (HFeared & INTRINSIC) {
			HFeared &= ~INTRINSIC;
			You_feel("less afraid.");
			u.cnd_intrinsiclosscount++;
		}
		if (HFeared & TIMEOUT) {
			HFeared &= ~TIMEOUT;
			You_feel("less afraid.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 203: if (HFear_resistance & INTRINSIC) {
			HFear_resistance &= ~INTRINSIC;
			You_feel("a little anxious!");
			u.cnd_intrinsiclosscount++;
		}
		if (HFear_resistance & TIMEOUT) {
			HFear_resistance &= ~TIMEOUT;
			You_feel("a little anxious!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 204: if (u.uhitincxtra != 0) {
			u.uhitinc -= u.uhitincxtra;
			u.uhitincxtra = 0;
			You_feel("your to-hit rating changing!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 205: if (u.udamincxtra != 0) {
			u.udaminc -= u.udamincxtra;
			u.udamincxtra = 0;
			You_feel("your damage rating changing!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 206: if (HKeen_memory & INTRINSIC) {
			HKeen_memory &= ~INTRINSIC;
			You_feel("a case of selective amnesia...");
			u.cnd_intrinsiclosscount++;
		}
		if (HKeen_memory & TIMEOUT) {
			HKeen_memory &= ~TIMEOUT;
			You_feel("a case of selective amnesia...");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 207: if (HVersus_curses & INTRINSIC) {
			HVersus_curses &= ~INTRINSIC;
			You_feel("cursed!");
			u.cnd_intrinsiclosscount++;
		}
		if (HVersus_curses & TIMEOUT) {
			HVersus_curses &= ~TIMEOUT;
			You_feel("cursed!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 208: if (HStun_resist & INTRINSIC) {
			HStun_resist &= ~INTRINSIC;
			You_feel("a little stunned!");
			u.cnd_intrinsiclosscount++;
		}
		if (HStun_resist & TIMEOUT) {
			HStun_resist &= ~TIMEOUT;
			You_feel("a little stunned!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 209: if (HConf_resist & INTRINSIC) {
			HConf_resist &= ~INTRINSIC;
			You_feel("a little confused!");
			u.cnd_intrinsiclosscount++;
		}
		if (HConf_resist & TIMEOUT) {
			HConf_resist &= ~TIMEOUT;
			You_feel("a little confused!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 210: if (HDouble_attack & INTRINSIC) {
			HDouble_attack &= ~INTRINSIC;
			You_feel("your attacks becoming slower!");
			u.cnd_intrinsiclosscount++;
		}
		if (HDouble_attack & TIMEOUT) {
			HDouble_attack &= ~TIMEOUT;
			You_feel("your attacks becoming slower!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 211: if (HQuad_attack & INTRINSIC) {
			HQuad_attack &= ~INTRINSIC;
			You_feel("your attacks becoming a lot slower!");
			u.cnd_intrinsiclosscount++;
		}
		if (HQuad_attack & TIMEOUT) {
			HQuad_attack &= ~TIMEOUT;
			You_feel("your attacks becoming a lot slower!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 212: if (HExtra_wpn_practice & INTRINSIC) {
			HExtra_wpn_practice &= ~INTRINSIC;
			You_feel("less able to learn new stuff!");
			u.cnd_intrinsiclosscount++;
		}
		if (HExtra_wpn_practice & TIMEOUT) {
			HExtra_wpn_practice &= ~TIMEOUT;
			You_feel("less able to learn new stuff!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 213: if (HDeath_resistance & INTRINSIC) {
			HDeath_resistance &= ~INTRINSIC;
			You_feel("a little dead!");
			u.cnd_intrinsiclosscount++;
		}
		if (HDeath_resistance & TIMEOUT) {
			HDeath_resistance &= ~TIMEOUT;
			You_feel("a little dead!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 214: if (HDisplaced & INTRINSIC) {
			HDisplaced &= ~INTRINSIC;
			You_feel("a little exposed!");
			u.cnd_intrinsiclosscount++;
		}
		if (HDisplaced & TIMEOUT) {
			HDisplaced &= ~TIMEOUT;
			You_feel("a little exposed!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 215: if (HPsi_resist & INTRINSIC) {
			HPsi_resist &= ~INTRINSIC;
			You_feel("empty-minded!");
			u.cnd_intrinsiclosscount++;
		}
		if (HPsi_resist & TIMEOUT) {
			HPsi_resist &= ~TIMEOUT;
			You_feel("empty-minded!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 216: if (HSight_bonus & INTRINSIC) {
			HSight_bonus &= ~INTRINSIC;
			You_feel("less perceptive!");
			u.cnd_intrinsiclosscount++;
		}
		if (HSight_bonus & TIMEOUT) {
			HSight_bonus &= ~TIMEOUT;
			You_feel("less perceptive!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 217:
	case 218: if (HManaleech & INTRINSIC) {
			HManaleech &= ~INTRINSIC;
			You_feel("less magically attuned!");
			u.cnd_intrinsiclosscount++;
		}
		if (HManaleech & TIMEOUT) {
			HManaleech &= ~TIMEOUT;
			You_feel("less magically attuned!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 219:
	case 220: if (HMap_amnesia & INTRINSIC) {
			HMap_amnesia &= ~INTRINSIC;
			You_feel("less forgetful!");
			u.cnd_intrinsiclosscount++;
		}
		if (HMap_amnesia & TIMEOUT) {
			HMap_amnesia &= ~TIMEOUT;
			You_feel("less forgetful!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 221: if (HPeacevision & INTRINSIC) {
			HPeacevision &= ~INTRINSIC;
			You_feel("less peaceful!");
			u.cnd_intrinsiclosscount++;
		}
		if (HPeacevision & TIMEOUT) {
			HPeacevision &= ~TIMEOUT;
			You_feel("less peaceful!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 222: if (HHallu_party & INTRINSIC) {
			HHallu_party &= ~INTRINSIC;
			You_feel("that the party is over!");
			u.cnd_intrinsiclosscount++;
		}
		if (HHallu_party & TIMEOUT) {
			HHallu_party &= ~TIMEOUT;
			You_feel("that the party is over!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 223: if (HDrunken_boxing & INTRINSIC) {
			HDrunken_boxing &= ~INTRINSIC;
			You_feel("a little drunk!");
			u.cnd_intrinsiclosscount++;
		}
		if (HDrunken_boxing & TIMEOUT) {
			HDrunken_boxing &= ~TIMEOUT;
			You_feel("a little drunk!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 224: if (HStunnopathy & INTRINSIC) {
			HStunnopathy &= ~INTRINSIC;
			You_feel("an uncontrolled stunning!");
			u.cnd_intrinsiclosscount++;
		}
		if (HStunnopathy & TIMEOUT) {
			HStunnopathy &= ~TIMEOUT;
			You_feel("an uncontrolled stunning!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 225: if (HNumbopathy & INTRINSIC) {
			HNumbopathy &= ~INTRINSIC;
			You_feel("numbness spreading through your body!");
			u.cnd_intrinsiclosscount++;
		}
		if (HNumbopathy & TIMEOUT) {
			HNumbopathy &= ~TIMEOUT;
			You_feel("numbness spreading through your body!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 226: if (HDimmopathy & INTRINSIC) {
			HDimmopathy &= ~INTRINSIC;
			You_feel(FunnyHallu ? "that your marriage is no longer safe..." : "worried about the future!");
			u.cnd_intrinsiclosscount++;
		}
		if (HDimmopathy & TIMEOUT) {
			HDimmopathy &= ~TIMEOUT;
			You_feel(FunnyHallu ? "that your marriage is no longer safe..." : "worried about the future!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 227: if (HFreezopathy & INTRINSIC) {
			HFreezopathy &= ~INTRINSIC;
			You_feel("ice-cold!");
			u.cnd_intrinsiclosscount++;
		}
		if (HFreezopathy & TIMEOUT) {
			HFreezopathy &= ~TIMEOUT;
			You_feel("ice-cold!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 228: if (HStoned_chiller & INTRINSIC) {
			HStoned_chiller &= ~INTRINSIC;
			You_feel("that you ain't gonna get time for relaxing anymore!");
			u.cnd_intrinsiclosscount++;
		}
		if (HStoned_chiller & TIMEOUT) {
			HStoned_chiller &= ~TIMEOUT;
			You_feel("that you ain't gonna get time for relaxing anymore!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 229: if (HCorrosivity & INTRINSIC) {
			HCorrosivity &= ~INTRINSIC;
			You_feel("the protective layer on your skin disappearing!");
			u.cnd_intrinsiclosscount++;
		}
		if (HCorrosivity & TIMEOUT) {
			HCorrosivity &= ~TIMEOUT;
			You_feel("the protective layer on your skin disappearing!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 230: if (HFear_factor & INTRINSIC) {
			HFear_factor &= ~INTRINSIC;
			You_feel("fearful!");
			u.cnd_intrinsiclosscount++;
		}
		if (HFear_factor & TIMEOUT) {
			HFear_factor &= ~TIMEOUT;
			You_feel("fearful!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 231: if (HBurnopathy & INTRINSIC) {
			HBurnopathy &= ~INTRINSIC;
			You_feel("red-hot!");
			u.cnd_intrinsiclosscount++;
		}
		if (HBurnopathy & TIMEOUT) {
			HBurnopathy &= ~TIMEOUT;
			You_feel("red-hot!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 232: if (HSickopathy & INTRINSIC) {
			HSickopathy &= ~INTRINSIC;
			You_feel("a loss of medical knowledge!");
			u.cnd_intrinsiclosscount++;
		}
		if (HSickopathy & TIMEOUT) {
			HSickopathy &= ~TIMEOUT;
			You_feel("a loss of medical knowledge!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 233: if (HWonderlegs & INTRINSIC) {
			HWonderlegs &= ~INTRINSIC;
			You_feel("that all girls and women will scratch bloody wounds on your legs with their high heels!");
			u.cnd_intrinsiclosscount++;
		}
		if (HWonderlegs & TIMEOUT) {
			HWonderlegs &= ~TIMEOUT;
			You_feel("that all girls and women will scratch bloody wounds on your legs with their high heels!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 234: if (HGlib_combat & INTRINSIC) {
			HGlib_combat &= ~INTRINSIC;
			You_feel("fliction in your %s!", makeplural(body_part(HAND)));
			u.cnd_intrinsiclosscount++;
		}
		if (HGlib_combat & TIMEOUT) {
			HGlib_combat &= ~TIMEOUT;
			You_feel("fliction in your %s!", makeplural(body_part(HAND)));
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 235:
	case 236:
	case 237: if (HStone_resistance & INTRINSIC) {
			HStone_resistance &= ~INTRINSIC;
			You_feel("less solid!");
			u.cnd_intrinsiclosscount++;
		}
		if (HStone_resistance & TIMEOUT) {
			HStone_resistance &= ~TIMEOUT;
			You_feel("less solid!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 238: if (HCont_resist & INTRINSIC) {
			HCont_resist &= ~INTRINSIC;
			You_feel("less resistant to contamination!");
			u.cnd_intrinsiclosscount++;
		}
		if (HCont_resist & TIMEOUT) {
			HCont_resist &= ~TIMEOUT;
			You_feel("less resistant to contamination!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 239: if (HDiscount_action & INTRINSIC) {
			HDiscount_action &= ~INTRINSIC;
			You_feel("less resistant to paralysis!");
			u.cnd_intrinsiclosscount++;
		}
		if (HDiscount_action & TIMEOUT) {
			HDiscount_action &= ~TIMEOUT;
			You_feel("less resistant to paralysis!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 240: if (HFull_nutrient & INTRINSIC) {
			HFull_nutrient &= ~INTRINSIC;
			You_feel("a hole in your %s!", body_part(STOMACH));
			u.cnd_intrinsiclosscount++;
		}
		if (HFull_nutrient & TIMEOUT) {
			HFull_nutrient &= ~TIMEOUT;
			You_feel("a hole in your %s!", body_part(STOMACH));
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 241: if (HTechnicality & INTRINSIC) {
			HTechnicality &= ~INTRINSIC;
			You_feel("less capable of using your techniques...");
			u.cnd_intrinsiclosscount++;
		}
		if (HTechnicality & TIMEOUT) {
			HTechnicality &= ~TIMEOUT;
			You_feel("less capable of using your techniques...");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 242: if (HHalf_spell_damage & INTRINSIC) {
			HHalf_spell_damage &= ~INTRINSIC;
			You_feel("vulnerable to spells!");
			u.cnd_intrinsiclosscount++;
		}
		if (HHalf_spell_damage & TIMEOUT) {
			HHalf_spell_damage &= ~TIMEOUT;
			You_feel("vulnerable to spells!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 243: if (HHalf_physical_damage & INTRINSIC) {
			HHalf_physical_damage &= ~INTRINSIC;
			You_feel("vulnerable to damage!");
			u.cnd_intrinsiclosscount++;
		}
		if (HHalf_physical_damage & TIMEOUT) {
			HHalf_physical_damage &= ~TIMEOUT;
			You_feel("vulnerable to damage!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 244: if (HUseTheForce & INTRINSIC) {
			HUseTheForce &= ~INTRINSIC;
			You_feel("that you lost your jedi powers!");
			u.cnd_intrinsiclosscount++;
		}
		if (HUseTheForce & TIMEOUT) {
			HUseTheForce &= ~TIMEOUT;
			You_feel("that you lost your jedi powers!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 245: if (HScentView & INTRINSIC) {
			HScentView &= ~INTRINSIC;
			You_feel("unable to smell things!");
			u.cnd_intrinsiclosscount++;
		}
		if (HScentView & TIMEOUT) {
			HScentView &= ~TIMEOUT;
			You_feel("unable to smell things!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 246: if (HDiminishedBleeding & INTRINSIC) {
			HDiminishedBleeding &= ~INTRINSIC;
			You_feel("your %s coagulants failing!", body_part(BLOOD));
			u.cnd_intrinsiclosscount++;
		}
		if (HDiminishedBleeding & TIMEOUT) {
			HDiminishedBleeding &= ~TIMEOUT;
			You_feel("your %s coagulants failing!", body_part(BLOOD));
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 247: if (HControlMagic & INTRINSIC) {
			HControlMagic &= ~INTRINSIC;
			You_feel("unable to control your magic!");
			u.cnd_intrinsiclosscount++;
		}
		if (HControlMagic & TIMEOUT) {
			HControlMagic &= ~TIMEOUT;
			You_feel("unable to control your magic!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 248: if (HExpBoost & INTRINSIC) {
			HExpBoost &= ~INTRINSIC;
			You_feel("a loss of experience!");
			u.cnd_intrinsiclosscount++;
		}
		if (HExpBoost & TIMEOUT) {
			HExpBoost &= ~TIMEOUT;
			You_feel("a loss of experience!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 249: if (HPainSense & INTRINSIC) {
			HPainSense &= ~INTRINSIC;
			You_feel("unable to sense pain!");
			u.cnd_intrinsiclosscount++;
		}
		if (HPainSense & TIMEOUT) {
			HPainSense &= ~TIMEOUT;
			You_feel("unable to sense pain!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 250:
	case 251:
	case 252: if (HInvertedState & INTRINSIC) {
			HInvertedState &= ~INTRINSIC;
			You_feel("back on your %s!", makeplural(body_part(FOOT)));
			u.cnd_intrinsiclosscount++;
		}
		if (HInvertedState & TIMEOUT) {
			HInvertedState &= ~TIMEOUT;
			You_feel("back on your %s!", makeplural(body_part(FOOT)));
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 253:
	case 254:
	case 255: if (HWinceState & INTRINSIC) {
			HWinceState &= ~INTRINSIC;
			You_feel("relieved!");
			u.cnd_intrinsiclosscount++;
		}
		if (HWinceState & TIMEOUT) {
			HWinceState &= ~TIMEOUT;
			You_feel("relieved!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 256: if (HDefusing & INTRINSIC) {
			HDefusing &= ~INTRINSIC;
			You_feel("unable to defuse traps!");
			u.cnd_intrinsiclosscount++;
		}
		if (HDefusing & TIMEOUT) {
			HDefusing &= ~TIMEOUT;
			You_feel("unable to defuse traps!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 257:
	case 258: if (HBurdenedState & INTRINSIC) {
			HBurdenedState &= ~INTRINSIC;
			You_feel("lighter");
			u.cnd_intrinsiclosscount++;
		}
		if (HBurdenedState & TIMEOUT) {
			HBurdenedState &= ~TIMEOUT;
			You_feel("lighter");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 259:
	case 260: if (HMagicVacuum & INTRINSIC) {
			HMagicVacuum &= ~INTRINSIC;
			You_feel("capable of casting magic effectively again!");
			u.cnd_intrinsiclosscount++;
		}
		if (HMagicVacuum & TIMEOUT) {
			HMagicVacuum &= ~TIMEOUT;
			You_feel("capable of casting magic effectively again!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 261: if (HResistancePiercing & INTRINSIC) {
			HResistancePiercing &= ~INTRINSIC;
			You_feel("unable to damage immune opponents!");
			u.cnd_intrinsiclosscount++;
		}
		if (HResistancePiercing & TIMEOUT) {
			HResistancePiercing &= ~TIMEOUT;
			You_feel("unable to damage immune opponents!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 262: if (HFuckOverEffect & INTRINSIC) {
			HFuckOverEffect &= ~INTRINSIC;
			You_feel("that you're no longer being fucked over!");
			u.cnd_intrinsiclosscount++;
		}
		if (HFuckOverEffect & TIMEOUT) {
			HFuckOverEffect &= ~TIMEOUT;
			You_feel("that you're no longer being fucked over!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 263: if (HMysteryResist & INTRINSIC) {
			HMysteryResist &= ~INTRINSIC;
			You_feel("less resistant to mystery attacks!");
			u.cnd_intrinsiclosscount++;
		}
		if (HMysteryResist & TIMEOUT) {
			HMysteryResist &= ~TIMEOUT;
			You_feel("less resistant to mystery attacks!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 264: if (HMagicFindBonus & INTRINSIC) {
			HMagicFindBonus &= ~INTRINSIC;
			You_feel("less likely to find magical items!");
			u.cnd_intrinsiclosscount++;
		}
		if (HMagicFindBonus & TIMEOUT) {
			HMagicFindBonus &= ~TIMEOUT;
			You_feel("less likely to find magical items!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 265: if (HSpellboost & INTRINSIC) {
			HSpellboost &= ~INTRINSIC;
			You_feel("your spell power waning!");
			u.cnd_intrinsiclosscount++;
		}
		if (HSpellboost & TIMEOUT) {
			HSpellboost &= ~TIMEOUT;
			You_feel("your spell power waning!");
			u.cnd_intrinsiclosscount++;
		}
		break;
	case 266: if (HWildWeatherEffect & INTRINSIC) {
			HWildWeatherEffect &= ~INTRINSIC;
			You_feel("the weather becoming normal again.");
			u.cnd_intrinsiclosscount++;
		}
		if (HWildWeatherEffect & TIMEOUT) {
			HWildWeatherEffect &= ~TIMEOUT;
			You_feel("the weather becoming normal again.");
			u.cnd_intrinsiclosscount++;
		}
		break;
	default: break;
	}
}

/*sit.c*/

/*	SCCS Id: @(#)explode.c	3.3	2000/07/07	*/
/*      Copyright (C) 1990 by Ken Arromdee */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef OVL0

/* Note: Arrays are column first, while the screen is row first */
/*WAC - For the magical explosion*/
static int mexpl[3][3] = {
	{ S_mexplode1, S_mexplode4, S_mexplode7 },
	{ S_mexplode2, S_mexplode5, S_mexplode8 },
	{ S_mexplode3, S_mexplode6, S_mexplode9 }
};
/*Normal fire explosion*/
static int expl[3][3] = {
	{ S_explode1, S_explode4, S_explode7 },
	{ S_explode2, S_explode5, S_explode8 },
	{ S_explode3, S_explode6, S_explode9 }
};
/*WAC - For the cold explosion*/
static int cexpl[3][3] = {
	{ S_cexplode1, S_cexplode4, S_cexplode7 },
	{ S_cexplode2, S_cexplode5, S_cexplode8 },
	{ S_cexplode3, S_cexplode6, S_cexplode9 }
};
/*WAC - For the death explosion*/
static int dexpl[3][3] = {
	{ S_dexplode1, S_dexplode4, S_dexplode7 },
	{ S_dexplode2, S_dexplode5, S_dexplode8 },
	{ S_dexplode3, S_dexplode6, S_dexplode9 }
};
/*WAC - For the lightning explosion*/
static int lexpl[3][3] = {
	{ S_lexplode1, S_lexplode4, S_lexplode7 },
	{ S_lexplode2, S_lexplode5, S_lexplode8 },
	{ S_lexplode3, S_lexplode6, S_lexplode9 }
};
/*WAC - For the poison explosion*/
static int pexpl[3][3] = {
	{ S_pexplode1, S_pexplode4, S_pexplode7 },
	{ S_pexplode2, S_pexplode5, S_pexplode8 },
	{ S_pexplode3, S_pexplode6, S_pexplode9 }
};
/*WAC - For the acid explosion*/
static int aexpl[3][3] = {
	{ S_aexplode1, S_aexplode4, S_aexplode7 },
	{ S_aexplode2, S_aexplode5, S_aexplode8 },
	{ S_aexplode3, S_aexplode6, S_aexplode9 }
};

/* This is the "do-it-all" explosion command */
static void FDECL(do_explode, (int,int,int,int,CHAR_P,int, BOOLEAN_P));

/* Note: I had to choose one of three possible kinds of "type" when writing
 * this function: a wand type (like in zap.c), an adtyp, or an object type.
 * Wand types get complex because they must be converted to adtyps for
 * determining such things as fire resistance.  Adtyps get complex in that
 * they don't supply enough information--was it a player or a monster that
 * did it, and with a wand, spell, or breath weapon?  Object types share both
 * these disadvantages....
 */

void
explode(x, y, type, dam, olet)
	xchar x, y; /* WAC was int...i think it's supposed to be xchar */
	int type; /* the same as in zap.c */
	int dam;
	char olet;
{
	do_explode(x,y,type,dam,olet,0, !flags.mon_moving);
}

void
do_explode(x, y, type, dam, olet, dest, yours)
	xchar x, y; /* WAC was int...i think it's supposed to be xchar */
	int type; /* the same as in zap.c */
	int dam;
	char olet;
	int dest; /* 0 = normal, 1 = silent, 2 = silent/remote */	
	boolean yours; /* is it your fault (for killing monsters) */
{
	int i, j, k, damu = dam;
	boolean starting = 1;
	boolean visible, any_shield;
	int uhurt = 0; /* 0=unhurt, 1=items damaged, 2=you and items damaged */
	const char *str;
	int idamres, idamnonres;
	struct monst *mtmp;
	uchar adtyp;
	int explmask[3][3];
		/* 0=normal explosion, 1=do shieldeff, 2=do nothing */
	boolean shopdamage = FALSE;
	boolean silent = FALSE, remote = FALSE;

	if (dest > 0) silent = TRUE;	
	if (dest == 2) remote = TRUE;

	if (olet == WAND_CLASS)         /* retributive strike */
		switch (Role_switch) {
			case PM_PRIEST:
			/*WAC add Flame,  Ice mages,  Necromancer */
			case PM_FLAME_MAGE:
			case PM_ICE_MAGE:
			case PM_NECROMANCER:
			case PM_WIZARD: damu /= 5;
				  break;
			case PM_HEALER:
			case PM_KNIGHT: damu /= 2;
				  break;
			default:  break;
		}

        if (olet == MON_EXPLODE) {
	    str = killer;
	    killer = 0;		/* set again later as needed */
	    adtyp = AD_PHYS;
        } else switch (abs(type) % 10) {
		case 0: str = "magical blast";
			adtyp = AD_MAGM;
			break;
		case 1: str =   olet == BURNING_OIL ?   "burning oil" :
				olet == SCROLL_CLASS ?  "tower of flame" :
							"fireball";
			adtyp = AD_FIRE;
			break;
		case 2: str = "ball of cold";
			adtyp = AD_COLD;
			break;
/* Assume that wands are death, others are disintegration */
		case 4: str =  (olet == WAND_CLASS) ? "death field" :
							"disintegration field";
                	adtyp = AD_DISN;
			break;
		case 5: str = "ball of lightning";
			adtyp = AD_ELEC;
			break;
		case 6: str = "poison gas cloud";
			adtyp = AD_DRST;
			break;
		case 7: str = "splash of acid";
			adtyp = AD_ACID;
			break;
		default: impossible("explosion base type %d?", type); return;
	}

/*WAC add light source for fire*/
#ifdef LIGHT_SRC_SPELL
        if ((!remote) && ((adtyp == AD_FIRE) || (adtyp == AD_ELEC))) {
                new_light_source(x, y, 2, LS_TEMP, (genericptr_t) 1);
                vision_recalc(0);
        }
#endif

	any_shield = visible = FALSE;
	for (i=0; i<3; i++) for (j=0; j<3; j++) {
                if (!(isok(i+x-1,j+y-1)
                        && (ZAP_POS((&levl[i+x-1][j+y-1])->typ)))) {
			explmask[i][j] = 2;
			continue;
		} else
			explmask[i][j] = 0;

		if (i+x-1 == u.ux && j+y-1 == u.uy) {
		    switch(adtyp) {
			case AD_PHYS:                        
				explmask[i][j] = 0;
				break;
			case AD_MAGM:
				explmask[i][j] = !!Antimagic;
				break;
			case AD_FIRE:
				explmask[i][j] = !!Fire_resistance;
				break;
			case AD_COLD:
				explmask[i][j] = !!Cold_resistance;
				break;
			case AD_DISN:                                
				explmask[i][j] = (olet == WAND_CLASS) ?
						!!(nonliving(youmonst.data) || is_demon(youmonst.data)) :
						!!Disint_resistance;
				break;
			case AD_ELEC:
				explmask[i][j] = !!Shock_resistance;
				break;
			case AD_DRST:
				explmask[i][j] = !!Poison_resistance;
				break;
			case AD_ACID:
				explmask[i][j] = !!Acid_resistance;
				break;
			default:
				impossible("explosion type %d?", adtyp);
				break;
		    }
		}

		mtmp = m_at(i+x-1, j+y-1);
#ifdef STEED
		if (!mtmp && i+x-1 == u.ux && j+y-1 == u.uy)
			mtmp = u.usteed;
#endif
		if (mtmp) {
		    if (mtmp->mhp < 1) explmask[i][j] = 2;
		    else switch(adtyp) {
			case AD_PHYS:                        
				break;
			case AD_MAGM:
				explmask[i][j] |= resists_magm(mtmp);
				break;
			case AD_FIRE:
				explmask[i][j] |= resists_fire(mtmp);
				break;
			case AD_COLD:
				explmask[i][j] |= resists_cold(mtmp);
				break;
			case AD_DISN:
				explmask[i][j] |= (olet == WAND_CLASS) ?
					(nonliving(mtmp->data) || is_demon(mtmp->data)) :
					resists_disint(mtmp);
				break;
			case AD_ELEC:
				explmask[i][j] |= resists_elec(mtmp);
				break;
			case AD_DRST:
				explmask[i][j] |= resists_poison(mtmp);
				break;
			case AD_ACID:
				explmask[i][j] |= resists_acid(mtmp);
				break;
			default:
				impossible("explosion type %d?", adtyp);
				break;
		    }
		}
		if (mtmp && cansee(i+x-1,j+y-1) && !canspotmon(mtmp))
		    map_invisible(i+x-1, j+y-1);
		else if (!mtmp && memory_is_invisible(i+x-1, j+y-1)) {
		    unmap_object(i+x-1, j+y-1);
		    newsym(i+x-1, j+y-1);
		}
		if (cansee(i+x-1, j+y-1)) visible = TRUE;
		if (explmask[i][j] == 1) any_shield = TRUE;
	}
	
	/* Not visible if remote */
	if (remote) visible = FALSE;
	
	if (visible) {
#ifdef ALLEG_FX
	    if (iflags.usealleg) {		
                alleg_explode(x,y, adtyp);
                if (any_shield)        /* simulate a shield effect */
                        for (i=0; i<3; i++) for (j=0; j<3; j++) {
                            if (explmask[i][j] == 1)
                                shieldeff(i+x-1, j+y-1);
                        }
	    } else {
#endif
		/* Start the explosion */
		for (i=0; i<3; i++) for (j=0; j<3; j++) {
			if (explmask[i][j] == 2) continue;
			{
				if (adtyp == AD_MAGM) {
				    tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
				                cmap_to_glyph(mexpl[i][j])); }
				else if (adtyp == AD_COLD) {
				    tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
				                cmap_to_glyph(cexpl[i][j])); }
				else if (adtyp == AD_DISN) {
				    tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
				                cmap_to_glyph(dexpl[i][j])); }
				else if (adtyp == AD_ELEC) {
				    tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
				                cmap_to_glyph(lexpl[i][j])); }
				else if (adtyp == AD_DRST) {
				    tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
				                cmap_to_glyph(pexpl[i][j])); }
				else if (adtyp == AD_ACID) {
				    tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
				                cmap_to_glyph(aexpl[i][j])); }
				else tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
								cmap_to_glyph(expl[i][j]));
			}
			tmp_at(i+x-1, j+y-1);
			starting = 0;
		}
		curs_on_u();    /* will flush screen and output */

		if (any_shield) {       /* simulate a shield effect */
		    for (k = 0; k < SHIELD_COUNT; k++) {
			for (i=0; i<3; i++) for (j=0; j<3; j++) {
			    if (explmask[i][j] == 1)
				/*
				 * Bypass tmp_at() and send the shield glyphs
				 * directly to the buffered screen.  tmp_at()
				 * will clean up the location for us later.
				 */
				show_glyph(i+x-1, j+y-1,
					cmap_to_glyph(shield_static[k]));
			}
			curs_on_u();    /* will flush screen and output */
			delay_output();
		    }

		    /* Cover last shield glyph with blast symbol. */
		    for (i=0; i<3; i++) for (j=0; j<3; j++) {
				if (explmask[i][j] == 1) {
					/*Clean up using right glyph type*/
					if (adtyp == AD_MAGM) 
					     show_glyph(i+x-1,j+y-1,cmap_to_glyph(mexpl[i][j]));
					else if (adtyp == AD_COLD) 
					     show_glyph(i+x-1,j+y-1,cmap_to_glyph(cexpl[i][j]));
					else if (adtyp == AD_DISN) 
					     show_glyph(i+x-1,j+y-1,cmap_to_glyph(dexpl[i][j]));
					else if (adtyp == AD_ELEC) 
					     show_glyph(i+x-1,j+y-1,cmap_to_glyph(lexpl[i][j]));
					else if (adtyp == AD_DRST) 
					     show_glyph(i+x-1,j+y-1,cmap_to_glyph(pexpl[i][j]));
					else if (adtyp == AD_ACID) 
					     show_glyph(i+x-1,j+y-1,cmap_to_glyph(aexpl[i][j]));
					else show_glyph(i+x-1,j+y-1,cmap_to_glyph(expl[i][j]));
			    }
		    }

		} else {                /* delay a little bit. */
		    delay_output();
		    delay_output();
		}
		tmp_at(DISP_END, 0); /* clear the explosion */
#ifdef ALLEG_FX
	    }
#endif
	} else if (!remote) {
		if (flags.soundok) You_hear("a blast.");
	}

    	if (dam) for (i=0; i<3; i++) for (j=0; j<3; j++) {
		if (explmask[i][j] == 2) continue;
		if (i+x-1 == u.ux && j+y-1 == u.uy)
			uhurt = (explmask[i][j] == 1) ? 1 : 2;
		idamres = idamnonres = 0;
		if (type >= 0)
		(void)zap_over_floor((xchar)(i+x-1), (xchar)(j+y-1),
				     type, &shopdamage);

		mtmp = m_at(i+x-1, j+y-1);
#ifdef STEED
		if (!mtmp && i+x-1 == u.ux && j+y-1 == u.uy)
			mtmp = u.usteed;
#endif
		if (!mtmp) continue;
		if (u.uswallow && mtmp == u.ustuck) {
			if (is_animal(u.ustuck->data))
				if (!silent) pline("%s gets %s!",
				      Monnam(u.ustuck),
				      (adtyp == AD_FIRE) ? "heartburn" :
				      (adtyp == AD_COLD) ? "chilly" :
				      (adtyp == AD_DISN) ? ((olet == WAND_CLASS) ?
				       "irradiated by pure energy" : "perforated") :
				      (adtyp == AD_ELEC) ? "shocked" :
				      (adtyp == AD_DRST) ? "poisoned" :
				      (adtyp == AD_ACID) ? "an upset stomach" :
				       "fried");
			else
				if (!silent) pline("%s gets slightly %s!",
				      Monnam(u.ustuck),
				      (adtyp == AD_FIRE) ? "toasted" :
				      (adtyp == AD_COLD) ? "chilly" :
				      (adtyp == AD_DISN) ? ((olet == WAND_CLASS) ?
				       "overwhelmed by pure energy" : "perforated") :
				      (adtyp == AD_ELEC) ? "shocked" :
				      (adtyp == AD_DRST) ? "intoxicated" :
				      (adtyp == AD_ACID) ? "burned" :
				       "fried");
		} else if (!silent && cansee(i+x-1, j+y-1))
		pline("%s is caught in the %s!", Monnam(mtmp), str);
		
		idamres += destroy_mitem(mtmp, SCROLL_CLASS, (int) adtyp);
		idamres += destroy_mitem(mtmp, SPBOOK_CLASS, (int) adtyp);
		idamnonres += destroy_mitem(mtmp, POTION_CLASS, (int) adtyp);
		idamnonres += destroy_mitem(mtmp, WAND_CLASS, (int) adtyp);
		idamnonres += destroy_mitem(mtmp, RING_CLASS, (int) adtyp);

		if (explmask[i][j] == 1) {
			golemeffects(mtmp, (int) adtyp, dam + idamres);
			mtmp->mhp -= idamnonres;
		} else {
		/* call resist with 0 and do damage manually so 1) we can
		 * get out the message before doing the damage, and 2) we can
		 * call mondied, not killed, if it's not your blast
		 */
			int mdam = dam;

			if (resist(mtmp, olet, 0, FALSE)) {
				if (!silent && cansee(i+x-1,j+y-1))
				    pline("%s resists the %s!", Monnam(mtmp), str);
				mdam = dam/2;
			}
			if (mtmp == u.ustuck)
				mdam *= 2;
			if (resists_cold(mtmp) && adtyp == AD_FIRE)
				mdam *= 2;
			else if (resists_fire(mtmp) && adtyp == AD_COLD)
				mdam *= 2;
			mtmp->mhp -= mdam;
			mtmp->mhp -= (idamres + idamnonres);
#ifdef SHOW_DMG
			if (mtmp->mhp > 0 && !remote)
				showdmg(mdam + idamres + idamnonres);
#endif
		}
		if (mtmp->mhp <= 0) {
			/* KMH -- Don't blame the player for pets killed by gas spores */
			/* KMH -- Don't blame the player for pets killing gas spores */
			if (yours) xkilled(mtmp, (silent ? 0 : 1));
			else monkilled(mtmp, "", (int)adtyp);
/*			else mondied(mtmp);*/
		}
	}

#ifdef LIGHT_SRC_SPELL
        /*WAC kill the light source*/
        if ((!remote) && ((adtyp == AD_FIRE) || (adtyp == AD_ELEC))) {
            del_light_source(LS_TEMP, (genericptr_t) 1);
        }
#endif

	/* Do your injury last */
	
	/* You are not hurt if this is remote */
	if (remote) uhurt = FALSE;
	
	if (uhurt) {
		/* [ALI] Give message if it's a weapon (grenade) exploding */
		if ((type >= 0 || adtyp == AD_PHYS || olet == WEAPON_CLASS) &&
		    /* gas spores */
				flags.verbose && olet != SCROLL_CLASS)
			You("are caught in the %s!", str);
		/* do property damage first, in case we end up leaving bones */
		if (adtyp == AD_FIRE) burn_away_slime();
		if (Invulnerable) {
		    damu = 0;
		    You("are unharmed!");
		}
		if (adtyp == AD_FIRE) (void) burnarmor(&youmonst);
		destroy_item(SCROLL_CLASS, (int) adtyp);
		destroy_item(SPBOOK_CLASS, (int) adtyp);
		destroy_item(POTION_CLASS, (int) adtyp);
		destroy_item(RING_CLASS, (int) adtyp);
		destroy_item(WAND_CLASS, (int) adtyp);

		ugolemeffects((int) adtyp, damu);

		if (uhurt == 2) {
		/* [Tom] apparently, explosions were always damaging
		   polymorphed players' original hit points. This
		   seems like a bug... */
		   if (Upolyd) {
				u.mh -= damu;
		  	 flags.botl = 1;
		  	 if (u.mh < 1) {
		  	   if (Polymorph_control || !rn2(3)) {
				    u.uhp -= mons[u.umonnum].mlevel;
			   	    u.uhpmax -= mons[u.umonnum].mlevel;
			   	    if (u.uhpmax < 1) u.uhpmax = 1;
		      	   }
		   	   rehumanize();
		   	 }
                   } else u.uhp -= damu, flags.botl = 1;
#ifdef SHOW_DMG                
                   if (flags.showdmg) pline("[%d pts.]", damu);
#endif
		}
		if (u.uhp <= 0) {
			if (olet == MON_EXPLODE) {
			    /* killer handled by caller */
			    if (str != killer_buf)
				Strcpy(killer_buf, str);
			    killer_format = KILLED_BY_AN;
			} else if (type >= 0 && olet != SCROLL_CLASS) {
			    killer_format = NO_KILLER_PREFIX;
			    Sprintf(killer_buf, "caught %sself in %s own %s",
				    him[flags.female], his[flags.female], str);
			} else {
			    killer_format = KILLED_BY;
			    Strcpy(killer_buf, str);
			}
			killer = killer_buf;
			/* Known BUG: BURNING suppresses corpse in bones data,
			   but done does not handle killer reason correctly */
			done((adtyp == AD_FIRE) ? BURNING : DIED);
                        /* done(BURNING); */
		}
		exercise(A_STR, FALSE);
	}

	if (shopdamage) {
		if (adtyp == AD_FIRE) pay_for_damage("burn away");                
		if (adtyp == AD_PHYS) pay_for_damage("damage");
			pay_for_damage(adtyp == AD_FIRE ? "burn away" :
			       adtyp == AD_COLD ? "shatter" :
			       adtyp == AD_DISN ? "disintegrate" : "destroy");
	}
#ifdef ALLEG_FX
        if (iflags.usealleg) cleanup_explosions();
#endif
}
#endif /* OVL0 */
#ifdef OVL1

struct scatter_chain {
	struct scatter_chain *next;     /* pointer to next scatter item */
	struct obj *obj;                /* pointer to the object        */
	xchar ox;                       /* location of                  */
	xchar oy;                       /*      item                    */
	schar dx;                       /* direction of                 */
	schar dy;                       /*      travel                  */
	int range;                      /* range of object              */
	boolean stopped;                /* flag for in-motion/stopped   */
};

/*
 * scflags:
 *      VIS_EFFECTS     Add visual effects to display
 *      MAY_HITMON      Objects may hit monsters
 *      MAY_HITYOU      Objects may hit hero
 *      MAY_HIT         Objects may hit you or monsters
 *      MAY_DESTROY     Objects may be destroyed at random
 *      MAY_FRACTURE    Stone objects can be fractured (statues, boulders)
 */

void
scatter(sx,sy,blastforce,scflags, obj)
int sx,sy;                              /* location of objects to scatter */
int blastforce;                         /* force behind the scattering  */
unsigned int scflags;
struct obj *obj;			/* only scatter this obj        */
{
	register struct obj *otmp;
	register int tmp;
	int farthest = 0;
	uchar typ;
	long qtmp;
	boolean used_up;
	boolean split_up = FALSE;
	boolean individual_object = obj ? TRUE : FALSE;
	struct monst *mtmp;
	struct scatter_chain *stmp, *stmp2 = 0;
	struct scatter_chain *schain = (struct scatter_chain *)0;

	while ((otmp = individual_object ? obj : level.objects[sx][sy]) != 0) {
	    if (otmp->quan > 1L) {
		qtmp = otmp->quan - 1;
		if (qtmp > LARGEST_INT) qtmp = LARGEST_INT;
		qtmp = (long)rnd((int)qtmp);
		(void) splitobj(otmp, qtmp);
		if (qtmp < otmp->quan)
			split_up = TRUE;
		else
			split_up = FALSE;
	    }
	    if (individual_object) {
		if (split_up) {
			if (otmp->where == OBJ_FLOOR)
				obj = otmp->nexthere;
			else
				obj = otmp->nobj;
		} else
			obj = (struct obj *)0;
	    }
	    obj_extract_self(otmp);
	    used_up = FALSE;

	    /* 9 in 10 chance of fracturing boulders or statues */
	    if ((scflags & MAY_FRACTURE)
			&& ((otmp->otyp == BOULDER) || (otmp->otyp == STATUE))
			&& rn2(10)) {
		if (otmp->otyp == BOULDER) {
		    pline("%s breaks apart.",The(xname(otmp)));
		    fracture_rock(otmp);
		    place_object(otmp, sx, sy); /* put fragments on floor */
		} else {
		    struct trap *trap;

		    if ((trap = t_at(sx,sy)) && trap->ttyp == STATUE_TRAP)
			    deltrap(trap);
		    pline("%s crumbles.",The(xname(otmp)));
		    (void) break_statue(otmp);
		    place_object(otmp, sx, sy); /* put fragments on floor */
		}
		used_up = TRUE;

	    /* 1 in 10 chance of destruction of obj; glass, egg destruction */
	    } else if ((scflags & MAY_DESTROY) && (!rn2(10)
			|| (objects[otmp->otyp].oc_material == GLASS
			|| otmp->otyp == EGG))) {
		if (breaks(otmp, (xchar)sx, (xchar)sy)) used_up = TRUE;
	    }

	    if (!used_up) {
		stmp = (struct scatter_chain *)
					alloc(sizeof(struct scatter_chain));
		stmp->next = (struct scatter_chain *)0;
		stmp->obj = otmp;
		stmp->ox = sx;
		stmp->oy = sy;
		tmp = rn2(8);           /* get the direction */
		stmp->dx = xdir[tmp];
		stmp->dy = ydir[tmp];
		tmp = blastforce - (otmp->owt/40);
		if (tmp < 1) tmp = 1;
		stmp->range = rnd(tmp); /* anywhere up to that determ. by wt */
		if (farthest < stmp->range) farthest = stmp->range;
		stmp->stopped = FALSE;
		if (!schain)
		    schain = stmp;
		else
		    stmp2->next = stmp;
		stmp2 = stmp;
	    }
	}

	while (farthest-- > 0) {
		for (stmp = schain; stmp; stmp = stmp->next) {
		   if ((stmp->range-- > 0) && (!stmp->stopped)) {
			bhitpos.x = stmp->ox + stmp->dx;
			bhitpos.y = stmp->oy + stmp->dy;
			typ = levl[bhitpos.x][bhitpos.y].typ;
			if(!isok(bhitpos.x, bhitpos.y)) {
				bhitpos.x -= stmp->dx;
				bhitpos.y -= stmp->dy;
				stmp->stopped = TRUE;
			} else if(!ZAP_POS(typ) ||
					closed_door(bhitpos.x, bhitpos.y)) {
				bhitpos.x -= stmp->dx;
				bhitpos.y -= stmp->dy;
				stmp->stopped = TRUE;
			} else if ((mtmp = m_at(bhitpos.x, bhitpos.y)) != 0) {
				if (scflags & MAY_HITMON) {
				    stmp->range--;
				    if (ohitmon((struct monst *)0, mtmp, stmp->obj, 1, FALSE)) {
					stmp->obj = (struct obj *)0;
					stmp->stopped = TRUE;
				    }
				}
			} else if (bhitpos.x==u.ux && bhitpos.y==u.uy) {
				if (scflags & MAY_HITYOU) {
				    int hitvalu, hitu;

				    if (multi) nomul(0);
				    hitvalu = 8 + stmp->obj->spe;
				    if (bigmonst(youmonst.data)) hitvalu++;
				    hitu = thitu(hitvalu,
						 dmgval(stmp->obj, &youmonst),
						 stmp->obj, (char *)0);
				    if (hitu) {
					stmp->range -= 3;
					stop_occupation();
				    }
				}
			} else {
				if (scflags & VIS_EFFECTS) {
				    /* tmp_at(bhitpos.x, bhitpos.y); */
				    /* delay_output(); */
				}
			}
			stmp->ox = bhitpos.x;
			stmp->oy = bhitpos.y;
		   }
		}
	}
	for (stmp = schain; stmp; stmp = stmp2) {
		int x,y;

		stmp2 = stmp->next;
		x = stmp->ox; y = stmp->oy;
		if (stmp->obj) {
			place_object(stmp->obj, x, y);
			stackobj(stmp->obj);
		}
		free((genericptr_t)stmp);
		newsym(x,y);
	}
}


/*
 * Splatter burning oil from x,y to the surrounding area.
 *
 * This routine should really take a how and direction parameters.
 * The how is how it was caused, e.g. kicked verses thrown.  The
 * direction is which way to spread the flaming oil.  Different
 * "how"s would give different dispersal patterns.  For example,
 * kicking a burning flask will splatter differently from a thrown
 * flask hitting the ground.
 *
 * For now, just perform a "regular" explosion.
 */
void
splatter_burning_oil(x, y)
    int x, y;
{
    explode(x, y, ZT_SPELL(ZT_FIRE), d(4,4), BURNING_OIL);
}

#ifdef FIREARMS

#define BY_OBJECT       ((struct monst *)0)

void
grenade_explode(otyp,x,y, isyou, dest)
int otyp, x, y;
boolean isyou;
int dest;
{
	int ztype;
	int numdice = 3, dicetype = 6;
	int ox, oy;
	boolean expl = TRUE, dig_expl = FALSE;
	
	switch (otyp) {
	    case GAS_GRENADE:
	    	ztype = ZT_SPELL(ZT_POISON_GAS);
	    	break;
	    case STICK_OF_DYNAMITE:
	    	ztype = ZT_SPELL(ZT_FIRE);
	    	numdice = 6;
	    	dig_expl = TRUE;
	    	break;
	    case FRAG_GRENADE:
	    default:
	    	ztype = ZT_SPELL(ZT_FIRE);
	    	break;
	}
	if (expl) {
		if (!isyou) ztype = -ztype;
		
		do_explode(x, y, ztype, d(numdice,dicetype), WEAPON_CLASS,
		  dest, isyou);
		wake_nearto(x, y, 400);
	}
	if (dig_expl) {
	    	/* Like cartoons - the explosion first, then 
	    	 * the world deals with the holes produced ;)
	    	 */
	    	for (ox = (x-1) ; ox <= (x + 1); ox++)
	    	    for (oy = (y-1) ; oy <= (y + 1); oy++)
	    		if(dig_check(BY_OBJECT, FALSE, ox, oy))
				digactualhole(ox, oy, BY_OBJECT, PIT);
	}
}

void arm_bomb(obj, yours)
struct obj *obj;
boolean yours;
{
	if (is_grenade(obj)) {
		attach_bomb_blow_timeout(obj, 
			    (obj->cursed ? rn2(5) + 2 : obj->blessed ? 4 : 
			    	rn2(2) + 3)
			     , yours);			
	}
	/* Otherwise, do nothing */
}

#endif /* FIREARMS */

#endif /* OVL1 */

/*explode.c*/

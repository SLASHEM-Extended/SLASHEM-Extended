/*	SCCS Id: @(#)explode.c	3.4	2002/11/10	*/
/*	Copyright (C) 1990 by Ken Arromdee */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef OVL0

/* ExplodeRegions share some commonalities with NhRegions, but not enough to
 * make it worth trying to create a common implementation.
 */
typedef struct {
    xchar x, y;
    xchar blast;	/* blast symbol */
    xchar shielded;	/* True if this location is shielded */
} ExplodeLocation;

typedef struct {
    ExplodeLocation *locations;
    short nlocations, alocations;
} ExplodeRegion;

STATIC_DCL ExplodeRegion *
create_explode_region()
{
    ExplodeRegion *reg;

    reg = (ExplodeRegion *)alloc(sizeof(ExplodeRegion));
    reg->locations = (ExplodeLocation *)0;
    reg->nlocations = 0;
    reg->alocations = 0;
    return reg;
}

STATIC_DCL void
add_location_to_explode_region(reg, x, y)
ExplodeRegion *reg;
xchar x, y;
{
    int i;
    ExplodeLocation *new;
    for(i = 0; i < reg->nlocations; i++)
	if (reg->locations[i].x == x && reg->locations[i].y == y)
	    return;
    if (reg->nlocations == reg->alocations) {
	reg->alocations = reg->alocations ? 2 * reg->alocations : 32;
	new = (ExplodeLocation *)
		alloc(reg->alocations * sizeof(ExplodeLocation));
	(void) memcpy((genericptr_t)new, (genericptr_t)reg->locations,
		reg->nlocations * sizeof(ExplodeLocation));
	free((genericptr_t)reg->locations);
	reg->locations = new;
    }
    reg->locations[reg->nlocations].x = x;
    reg->locations[reg->nlocations].y = y;
    /* reg->locations[reg->nlocations].blast = 0; */
    /* reg->locations[reg->nlocations].shielded = 0; */
    reg->nlocations++;
}

STATIC_DCL int
compare_explode_location(loc1, loc2)
ExplodeLocation *loc1, *loc2;
{
    return loc1->y == loc2->y ? loc1->x - loc2->x : loc1->y - loc2->y;
}

STATIC_DCL void
set_blast_symbols(reg)
ExplodeRegion *reg;
{
    int i, j, bitmask;
    /* The index into the blast symbol array is a bitmask containing 4 bits:
     * bit 3: True if the location immediately to the north is present
     * bit 2: True if the location immediately to the south is present
     * bit 1: True if the location immediately to the east is present
     * bit 0: True if the location immediately to the west is present
     */
    static int blast_symbols[16] = {
	S_explode5, S_explode6, S_explode4, S_explode5,
	S_explode2, S_explode3, S_explode1, S_explode2,
	S_explode8, S_explode9, S_explode7, S_explode8,
	S_explode5, S_explode6, S_explode4, S_explode5,
    };
    /* Sort in order of North -> South, West -> East */
    qsort(reg->locations, reg->nlocations, sizeof(ExplodeLocation),
	    compare_explode_location);
    /* Pass 1: Build the bitmasks in the blast field */
    for(i = 0; i < reg->nlocations; i++)
	reg->locations[i].blast = 0;
    for(i = 0; i < reg->nlocations; i++) {
	bitmask = 0;
	if (i && reg->locations[i-1].y == reg->locations[i].y &&
		reg->locations[i-1].x == reg->locations[i].x-1) {
	    reg->locations[i].blast |= 1;	/* Location to the west */
	    reg->locations[i-1].blast |= 2;	/* Location to the east */
	}
	for(j = i-1; j >= 0; j--) {
	    if (reg->locations[j].y < reg->locations[i].y-1)
		break;
	    else if (reg->locations[j].y == reg->locations[i].y-1 &&
		    reg->locations[j].x == reg->locations[i].x) {
		reg->locations[i].blast |= 8;	/* Location to the north */
		reg->locations[j].blast |= 4;	/* Location to the south */
		break;
	    }
	}
    }
    /* Pass 2: Set the blast symbols */
    for(i = 0; i < reg->nlocations; i++)
	reg->locations[i].blast = blast_symbols[reg->locations[i].blast];
}

STATIC_DCL void
free_explode_region(reg)
ExplodeRegion *reg;
{
    free((genericptr_t)reg->locations);
    free((genericptr_t)reg);
}

/* This is the "do-it-all" explosion command */
STATIC_DCL void FDECL(do_explode,
	(int,int,ExplodeRegion *,int,int,CHAR_P,int,int,BOOLEAN_P));

/* Note: I had to choose one of three possible kinds of "type" when writing
 * this function: a wand type (like in zap.c), an adtyp, or an object type.
 * Wand types get complex because they must be converted to adtyps for
 * determining such things as fire resistance.  Adtyps get complex in that
 * they don't supply enough information--was it a player or a monster that
 * did it, and with a wand, spell, or breath weapon?  Object types share both
 * these disadvantages....
 *
 * Explosions derived from vanilla NetHack:
 *
 * src         nature          olet        expl    Comment
 * Your wand   MAGIC_MISSILE   WAND        FROSTY  Exploding wands of cold
 * Your wand   MAGIC_MISSILE   WAND        FIERY   Exploding wands of fire/
 *                                                 fireball
 * Your wand   MAGIC_MISSILE   WAND        MAGICAL Other explosive wands
 * Your spell  FIRE            BURNING_OIL FIERY   Splattered buring oil
 * Mon's ?     -               MON_EXPLODE NOXIOUS Exploding gas spore
 * Your spell  FIRE            0           FIERY   Filling a lamp with oil
 *                                                 when lit
 * Your spell  FIRE            SCROLL      FIERY   Reading a scroll of fire
 * Your spell  FIRE            WAND        FIERY   Zap yourself with wand/
 *                                                 spell of fireball
 * Your spell  FIRE            0           FIERY   Your fireball
 *
 * Slash'EM specific explosions:
 *
 * src         nature          olet        expl    Comment
 * Your spell  FIRE            WEAPON      FIERY   Explosive projectile
 * Your spell  FIRE            WEAPON      FIERY   Bolts shot by Hellfire
 * Mon's spell FIRE            FIERY       WEAPON  Explosive projectile (BUG)
 * Mon's spell FIRE            FIERY       WEAPON  Bolts shot by Hellfire (BUG)
 * Your spell  MAGIC_MISSILE   WAND        MAGICAL Spirit bomb technique
 * Mon's spell FIRE            0           FIERY   Monster's fireball
 *
 * Sigil of tempest:
 *
 * src         nature          olet        expl    Comment
 * Your spell  MAGIC_MISSILE   0           MAGICAL Hero casts magic missile
 * Your spell  DEATH           0           MAGICAL Hero casts finger of death
 * Your spell  FIRE            0           FIERY   Hero casts fireball
 * Your spell  LIGHTNING       0           FIERY   Hero casts lightning
 * Your spell  COLD            0           FROSTY  Hero casts cone of cold
 * Your spell  SLEEP           0           NOXIOUS Hero casts sleep
 * Your spell  POISON_GAS      0           NOXIOUS Hero casts poison blast
 * Your spell  ACID            0           NOXIOUS Hero casts acid stream
 *
 * Mega spells:
 *
 * src         nature          olet        expl    Comment
 * Your mega   FIRE            0           FIERY   
 * Your mega   COLD            0           FROSTY  
 * Your mega   MAGIC_MISSLE    0           MAGICAL 
 *
 * Notes:
 *	Nature is encoded as (abs(type) % 10) and src is determined using the
 *	following table:
 *		Types		Src
 *		-30 - -39	Mon's wand
 *		-20 - -29	Mon's breath
 *		-10 - -19	Mon's spell
 *		 -1 -  -9	Special
 *		  0 -   9	Your wand
 *		 10 -  19	Your spell
 *		 20 -  29	Your breath
 *		 30 -  39	Your mega
 *	There is only one special type currently defined:
 *		-1		Exploding gas spore
 */
void
explode(x, y, type, dam, olet, expltype)
xchar x, y; /* WAC was int...i think it's supposed to be xchar */
int type; /* the same as in zap.c */
int dam;
char olet;
int expltype;
{
    int i, j;
    ExplodeRegion *area;
    area = create_explode_region();
    for(i = 0; i < 3; i++)
	for(j = 0; j < 3; j++)
	    if (isok(i+x-1,j+y-1) && ZAP_POS((&levl[i+x-1][j+y-1])->typ))
		add_location_to_explode_region(area, i+x-1, j+y-1);
    do_explode(x, y, area, type, dam, olet, expltype, 0, !flags.mon_moving);
    free_explode_region(area);
}

void
do_explode(x, y, area, type, dam, olet, expltype, dest, yours)
xchar x, y; /* WAC was int...i think it's supposed to be xchar */
ExplodeRegion *area;
int type; /* the same as in zap.c */
int dam;
char olet;
int expltype;
int dest; /* 0 = normal, 1 = silent, 2 = silent/remote */	
boolean yours; /* is it your fault (for killing monsters) */
{
	int i, k, damu = dam;
	boolean starting = 1;
	boolean visible, any_shield;
	int uhurt = 0; /* 0=unhurt, 1=items damaged, 2=you and items damaged */
	const char *str;
	int idamres, idamnonres;
	struct monst *mtmp;
	uchar adtyp;
	boolean explmask;
	boolean shopdamage = FALSE;
	boolean generic = FALSE;
	boolean silent = FALSE, remote = FALSE;
	xchar xi, yi;

	if (dest > 0) silent = TRUE;	
	if (dest == 2) remote = TRUE;

	if (olet == WAND_CLASS)		/* retributive strike */
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
	} else
	switch (abs(type) % 10) {
		case 0: str = "magical blast";
			adtyp = AD_MAGM;
			break;
		case 1: str =   olet == BURNING_OIL ?	"burning oil" :
				olet == SCROLL_CLASS ?	"tower of flame" :
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
	for(i = 0; i < area->nlocations; i++) {
		explmask = FALSE;
		xi = area->locations[i].x;
		yi = area->locations[i].y;
		if (xi == u.ux && yi == u.uy) {
		    switch(adtyp) {
			case AD_PHYS:                        
				break;
			case AD_MAGM:
				explmask = !!Antimagic;
				break;
			case AD_FIRE:
				explmask = !!Fire_resistance;
				break;
			case AD_COLD:
				explmask = !!Cold_resistance;
				break;
			case AD_DISN:
				explmask = (olet == WAND_CLASS) ?
						!!(nonliving(youmonst.data) || is_demon(youmonst.data)) :
						!!Disint_resistance;
				break;
			case AD_ELEC:
				explmask = !!Shock_resistance;
				break;
			case AD_DRST:
				explmask = !!Poison_resistance;
				break;
			case AD_ACID:
				explmask = !!Acid_resistance;
				break;
			default:
				impossible("explosion type %d?", adtyp);
				break;
		    }
		}

		mtmp = m_at(xi, yi);
#ifdef STEED
		if (!mtmp && xi == u.ux && yi == u.uy)
			mtmp = u.usteed;
#endif
		if (mtmp) {
		    switch(adtyp) {
			case AD_PHYS:                        
				break;
			case AD_MAGM:
				explmask |= resists_magm(mtmp);
				break;
			case AD_FIRE:
				explmask |= resists_fire(mtmp);
				break;
			case AD_COLD:
				explmask |= resists_cold(mtmp);
				break;
			case AD_DISN:
				explmask |= (olet == WAND_CLASS) ?
					(nonliving(mtmp->data) || is_demon(mtmp->data)) :
					resists_disint(mtmp);
				break;
			case AD_ELEC:
				explmask |= resists_elec(mtmp);
				break;
			case AD_DRST:
				explmask |= resists_poison(mtmp);
				break;
			case AD_ACID:
				explmask |= resists_acid(mtmp);
				break;
			default:
				impossible("explosion type %d?", adtyp);
				break;
		    }
		}
		if (mtmp && cansee(xi,yi) && !canspotmon(mtmp))
		    map_invisible(xi, yi);
		else if (!mtmp && memory_is_invisible(xi, yi)) {
		    unmap_object(xi, yi);
		    newsym(xi, yi);
		}
		if (cansee(xi, yi)) visible = TRUE;
		if (explmask) any_shield = TRUE;
		area->locations[i].shielded = explmask;
	}

	/* Not visible if remote */
	if (remote) visible = FALSE;
	
	if (visible) {
#ifdef ALLEG_FX
	    if (iflags.usealleg) {
		alleg_explode(x, y, adtyp);
		if (any_shield)		/* simulate a shield effect */
		    for(i = 0; i < area->nlocations; i++) {
			if (area->locations[i].shielded)
			    shieldeff(area->locations[i].x,
				    area->locations[i].y);
		    }
	    } else {
#endif
		set_blast_symbols(area);
		/* Start the explosion */
		for(i = 0; i < area->nlocations; i++) {
		    tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
			    explosion_to_glyph(expltype,
			    area->locations[i].blast));
		    tmp_at(area->locations[i].x, area->locations[i].y);
		    starting = 0;
		}
		curs_on_u();	/* will flush screen and output */

		if (any_shield && flags.sparkle) { /* simulate shield effect */
		    for (k = 0; k < SHIELD_COUNT; k++) {
			for(i = 0; i < area->nlocations; i++) {
			    if (area->locations[i].shielded)
				/*
				 * Bypass tmp_at() and send the shield glyphs
				 * directly to the buffered screen.  tmp_at()
				 * will clean up the location for us later.
				 */
				show_glyph(area->locations[i].x,
					area->locations[i].y,
					cmap_to_glyph(shield_static[k]));
			}
			curs_on_u();	/* will flush screen and output */
			delay_output();
		    }

		    /* Cover last shield glyph with blast symbol. */
		    for(i = 0; i < area->nlocations; i++) {
			if (area->locations[i].shielded)
			    show_glyph(area->locations[i].x,
				    area->locations[i].y,
				    explosion_to_glyph(expltype,
				    area->locations[i].blast));
		    }

		} else {		/* delay a little bit. */
		    delay_output();
		    delay_output();
		}
		tmp_at(DISP_END, 0); /* clear the explosion */
#ifdef ALLEG_FX
	    }
#endif
	} else if (!remote) {
	    if (olet == MON_EXPLODE) {
		str = "explosion";
		generic = TRUE;
	    }
	    if (flags.soundok)
		You_hear(is_pool(x, y) ? "a muffled explosion." : "a blast.");
	}

	    if (dam) for(i = 0; i < area->nlocations; i++) {
		xi = area->locations[i].x;
		yi = area->locations[i].y;
		if (xi == u.ux && yi == u.uy)
		    uhurt = area->locations[i].shielded ? 1 : 2;
		idamres = idamnonres = 0;

		/* DS: Allow monster induced explosions also */
		if (type >= 0 || type <= -10)
		    (void)zap_over_floor(xi, yi, type, &shopdamage);

		mtmp = m_at(xi, yi);
#ifdef STEED
		if (!mtmp && xi == u.ux && yi == u.uy)
			mtmp = u.usteed;
#endif
		if (!mtmp) continue;
		if (DEADMONSTER(mtmp)) continue;
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
		} else if (!silent && cansee(xi, yi)) {
		    if(mtmp->m_ap_type) seemimic(mtmp);
		    pline("%s is caught in the %s!", Monnam(mtmp), str);
		}

		idamres += destroy_mitem(mtmp, SCROLL_CLASS, (int) adtyp);
		idamres += destroy_mitem(mtmp, SPBOOK_CLASS, (int) adtyp);
		idamnonres += destroy_mitem(mtmp, POTION_CLASS, (int) adtyp);
		idamnonres += destroy_mitem(mtmp, WAND_CLASS, (int) adtyp);
		idamnonres += destroy_mitem(mtmp, RING_CLASS, (int) adtyp);

		if (area->locations[i].shielded) {
			golemeffects(mtmp, (int) adtyp, dam + idamres);
			mtmp->mhp -= idamnonres;
		} else {
		/* call resist with 0 and do damage manually so 1) we can
		 * get out the message before doing the damage, and 2) we can
		 * call mondied, not killed, if it's not your blast
		 */
			int mdam = dam;

			if (resist(mtmp, olet, 0, FALSE)) {
			    if (!silent && cansee(xi,yi))
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
			/* KMH -- Don't blame the player for pets killing gas spores */
			if (yours) xkilled(mtmp, (silent ? 0 : 1));
			else monkilled(mtmp, "", (int)adtyp);
		} else if (!flags.mon_moving && yours) setmangry(mtmp);
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
		} else if (Half_physical_damage && adtyp == AD_PHYS)
		    damu = (damu+1) / 2;
		if (adtyp == AD_FIRE) (void) burnarmor(&youmonst);
		destroy_item(SCROLL_CLASS, (int) adtyp);
		destroy_item(SPBOOK_CLASS, (int) adtyp);
		destroy_item(POTION_CLASS, (int) adtyp);
		destroy_item(RING_CLASS, (int) adtyp);
		destroy_item(WAND_CLASS, (int) adtyp);

		ugolemeffects((int) adtyp, damu);

		if (uhurt == 2) {
		    if (Upolyd)
		    	u.mh  -= damu;
		    else
			u.uhp -= damu;
		    flags.botl = 1;
#ifdef SHOW_DMG                
		    if (flags.showdmg) pline("[%d pts.]", damu);
#endif
		}

		if (u.uhp <= 0 || (Upolyd && u.mh <= 0)) {
		    if (Upolyd) {
			if (Polymorph_control || !rn2(3)) {
			    u.uhp -= mons[u.umonnum].mlevel;
			    u.uhpmax -= mons[u.umonnum].mlevel;
			    if (u.uhpmax < 1) u.uhpmax = 1;
			}
			rehumanize();
		    } else {
			if (olet == MON_EXPLODE) {
			    /* killer handled by caller */
			    if (str != killer_buf && !generic)
				Strcpy(killer_buf, str);
			    killer_format = KILLED_BY_AN;
			} else if (type >= 0 && olet != SCROLL_CLASS && yours) {
			    killer_format = NO_KILLER_PREFIX;
			    Sprintf(killer_buf, "caught %sself in %s own %s",
				    uhim(), uhis(), str);
			} else if (olet != BURNING_OIL) {
			    killer_format = KILLED_BY_AN;
			    Strcpy(killer_buf, str);
			} else {
			    killer_format = KILLED_BY;
			    Strcpy(killer_buf, str);
			}
			killer = killer_buf;
			/* Known BUG: BURNING suppresses corpse in bones data,
			   but done does not handle killer reason correctly */
			done((adtyp == AD_FIRE) ? BURNING : DIED);
		    }
		}
		exercise(A_STR, FALSE);
	}

	if (shopdamage) {
		pay_for_damage(adtyp == AD_FIRE ? "burn away" :
			       adtyp == AD_COLD ? "shatter" :
			       adtyp == AD_DISN ? "disintegrate" : "destroy",
			       FALSE);
	}

	/* explosions are noisy */
	i = dam * dam;
	if (i < 50) i = 50;	/* in case random damage is very small */
	wake_nearto(x, y, i);
#ifdef ALLEG_FX
        if (iflags.usealleg) cleanup_explosions();
#endif
}
#endif /* OVL0 */
#ifdef OVL1

struct scatter_chain {
	struct scatter_chain *next;	/* pointer to next scatter item	*/
	struct obj *obj;		/* pointer to the object	*/
	xchar ox;			/* location of			*/
	xchar oy;			/*	item			*/
	schar dx;			/* direction of			*/
	schar dy;			/*	travel			*/
	int range;			/* range of object		*/
	boolean stopped;		/* flag for in-motion/stopped	*/
};

/*
 * scflags:
 *	VIS_EFFECTS	Add visual effects to display
 *	MAY_HITMON	Objects may hit monsters
 *	MAY_HITYOU	Objects may hit hero
 *	MAY_HIT		Objects may hit you or monsters
 *	MAY_DESTROY	Objects may be destroyed at random
 *	MAY_FRACTURE	Stone objects can be fractured (statues, boulders)
 */

/* returns number of scattered objects */
long
scatter(sx,sy,blastforce,scflags, obj)
int sx,sy;				/* location of objects to scatter */
int blastforce;				/* force behind the scattering	*/
unsigned int scflags;
struct obj *obj;			/* only scatter this obj        */
{
	register struct obj *otmp;
	register int tmp;
	int farthest = 0;
	uchar typ;
	long qtmp;
	boolean used_up;
	boolean individual_object = obj ? TRUE : FALSE;
	struct monst *mtmp;
	struct scatter_chain *stmp, *stmp2 = 0;
	struct scatter_chain *schain = (struct scatter_chain *)0;
	long total = 0L;

	while ((otmp = individual_object ? obj : level.objects[sx][sy]) != 0) {
	    if (otmp->quan > 1L) {
		qtmp = otmp->quan - 1;
		if (qtmp > LARGEST_INT) qtmp = LARGEST_INT;
		qtmp = (long)rnd((int)qtmp);
		otmp = splitobj(otmp, qtmp);
	    } else {
		obj = (struct obj *)0; /* all used */
	    }
	    obj_extract_self(otmp);
	    used_up = FALSE;

	    /* 9 in 10 chance of fracturing boulders or statues */
	    if ((scflags & MAY_FRACTURE)
			&& ((otmp->otyp == BOULDER) || (otmp->otyp == STATUE))
			&& rn2(10)) {
		if (otmp->otyp == BOULDER) {
		    pline("%s apart.", Tobjnam(otmp, "break"));
		    fracture_rock(otmp);
		    place_object(otmp, sx, sy);
		    if ((otmp = sobj_at(BOULDER, sx, sy)) != 0) {
			/* another boulder here, restack it to the top */
			obj_extract_self(otmp);
			place_object(otmp, sx, sy);
		    }
		} else {
		    struct trap *trap;

		    if ((trap = t_at(sx,sy)) && trap->ttyp == STATUE_TRAP)
			    deltrap(trap);
		    pline("%s.", Tobjnam(otmp, "crumble"));
		    (void) break_statue(otmp);
		    place_object(otmp, sx, sy);	/* put fragments on floor */
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
		tmp = rn2(8);		/* get the direction */
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
			if ( x!=sx || y!=sy )
			    total += stmp->obj->quan;
			place_object(stmp->obj, x, y);
			stackobj(stmp->obj);
		}
		free((genericptr_t)stmp);
		newsym(x,y);
	}

	return total;
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
    explode(x, y, ZT_SPELL(ZT_FIRE), d(4,4), BURNING_OIL, EXPL_FIERY);
}

#ifdef FIREARMS

#define BY_OBJECT       ((struct monst *)0)

STATIC_DCL int
dp(n, p)		/* 0 <= dp(n, p) <= n */
int n, p;
{
    int tmp = 0;
    while (n--) tmp += !rn2(p);
    return tmp;
}

#define GRENADE_TRIGGER(obj)	\
    if ((obj)->otyp == FRAG_GRENADE) { \
	delquan = dp((obj)->quan, 10); \
	no_fiery += delquan; \
    } else if ((obj)->otyp == GAS_GRENADE) { \
	delquan = dp((obj)->quan, 10); \
	no_gas += delquan; \
    } else if ((obj)->otyp == STICK_OF_DYNAMITE) { \
	delquan = (obj)->quan; \
	no_fiery += (obj)->quan * 2; \
	no_dig += (obj)->quan; \
    } else if (is_bullet(obj)) \
	delquan = (obj)->quan; \
    else \
	delquan = 0

struct grenade_callback {
    ExplodeRegion *fiery_area, *gas_area, *dig_area;
    boolean isyou;
};

STATIC_DCL void FDECL(grenade_effects, (struct obj *,XCHAR_P,XCHAR_P,
	ExplodeRegion *,ExplodeRegion *,ExplodeRegion *,BOOLEAN_P));

STATIC_DCL int
grenade_fiery_callback(data, x, y)
genericptr_t data;
int x, y;
{
    int is_accessible = ZAP_POS(levl[x][y].typ);
    struct grenade_callback *gc = (struct grenade_callback *)data;
    if (is_accessible) {
	add_location_to_explode_region(gc->fiery_area, x, y);
	grenade_effects((struct obj *)0, x, y,
		gc->fiery_area, gc->gas_area, gc->dig_area, gc->isyou);
    }
    return !is_accessible;
}

STATIC_DCL int
grenade_gas_callback(data, x, y)
genericptr_t data;
int x, y;
{
    int is_accessible = ZAP_POS(levl[x][y].typ);
    struct grenade_callback *gc = (struct grenade_callback *)data;
    if (is_accessible)
	add_location_to_explode_region(gc->gas_area, x, y);
    return !is_accessible;
}

STATIC_DCL int
grenade_dig_callback(data, x, y)
genericptr_t data;
int x, y;
{
    struct grenade_callback *gc = (struct grenade_callback *)data;
    if (dig_check(BY_OBJECT, FALSE, x, y))
	add_location_to_explode_region(gc->dig_area, x, y);
    return !ZAP_POS(levl[x][y].typ);
}

STATIC_DCL void
grenade_effects(source, x, y, fiery_area, gas_area, dig_area, isyou)
struct obj *source;
xchar x, y;
ExplodeRegion *fiery_area, *gas_area, *dig_area;
boolean isyou;
{
    int i, r;
    struct obj *obj, *obj2;
    struct monst *mon;
    /*
     * Note: These count explosive charges in arbitary units. Grenades
     *       are counted as 1 and sticks of dynamite as 2 fiery and 1 dig.
     */
    int no_gas = 0, no_fiery = 0, no_dig = 0;
    int delquan;
    boolean shielded = FALSE, redraw;
    struct grenade_callback gc;

    if (source) {
	if (source->otyp == GAS_GRENADE)
	    no_gas += source->quan;
	else if (source->otyp == FRAG_GRENADE)
	    no_fiery += source->quan;
	else if (source->otyp == STICK_OF_DYNAMITE) {
	    no_fiery += source->quan * 2;
	    no_dig += source->quan;
	}
	redraw = source->where == OBJ_FLOOR;
	obj_extract_self(source);
	obfree(source, (struct obj *)0);
	if (redraw) newsym(x, y);
    }
    mon = m_at(x, y);
#ifdef STEED
    if (!mon && x == u.ux && y == u.uy)
	mon = u.usteed;
#endif
    if (mon && !DEADMONSTER(mon))
	if (resists_fire(mon))
	    shielded = TRUE;
	else
	    for(obj = mon->minvent; obj; obj = obj2) {
		obj2 = obj->nobj;
		GRENADE_TRIGGER(obj);
		for(i = 0; i < delquan; i++)
		    m_useup(mon, obj);
	    }
    if (x == u.ux && y == u.uy)
	if (Fire_resistance)
	    shielded = TRUE;
	else
	    for(obj = invent; obj; obj = obj2) {
		obj2 = obj->nobj;
		GRENADE_TRIGGER(obj);
		for(i = 0; i < delquan; i++)
		    useup(obj);
	    }
    if (!shielded)
	for(obj = level.objects[x][y]; obj; obj = obj2) {
	    obj2 = obj->nexthere;
	    GRENADE_TRIGGER(obj);
	    if (delquan) {
		if (isyou)
		    useupf(obj, delquan);
		else if (delquan < obj->quan)
		    obj->quan -= delquan;
		else
		    delobj(obj);
	    }
	}
    gc.fiery_area = fiery_area;
    gc.gas_area = gas_area;
    gc.dig_area = dig_area;
    gc.isyou = isyou;
    if (no_gas) {
	/* r = floor(log2(n))+1 */
	r = 0;
	while(no_gas) {
	    r++;
	    no_gas /= 2;
	}
	xpathto(r, x, y, grenade_gas_callback, (genericptr_t)&gc);
    }
    if (no_fiery) {
	/* r = floor(log2(n))+1 */
	r = 0;
	while(no_fiery) {
	    r++;
	    no_fiery /= 2;
	}
	xpathto(r, x, y, grenade_fiery_callback, (genericptr_t)&gc);
    }
    if (no_dig) {
	/* r = floor(log2(n))+1 */
	r = 0;
	while(no_dig) {
	    r++;
	    no_dig /= 2;
	}
	xpathto(r, x, y, grenade_dig_callback, (genericptr_t)&gc);
    }
}

/*
 * Note: obj is not valid after return
 */

void
grenade_explode(obj, x, y, isyou, dest)
struct obj *obj;
int x, y;
boolean isyou;
int dest;
{
    int i, ztype;
    boolean shop_damage = FALSE;
    int ox, oy;
    ExplodeRegion *fiery_area, *gas_area, *dig_area;
    struct trap *trap;
    
    fiery_area = create_explode_region();
    gas_area = create_explode_region();
    dig_area = create_explode_region();
    grenade_effects(obj, x, y, fiery_area, gas_area, dig_area, isyou);
    if (fiery_area->nlocations) {
	ztype = isyou ? ZT_SPELL(ZT_FIRE) : -ZT_SPELL(ZT_FIRE);
	do_explode(x, y, fiery_area, ztype, d(3,6), WEAPON_CLASS,
	  EXPL_FIERY, dest, isyou);
    }
    wake_nearto(x, y, 400);
    /* Like cartoons - the explosion first, then
     * the world deals with the holes produced ;)
     */
    for(i = 0; i < dig_area->nlocations; i++) {
	ox = dig_area->locations[i].x;
	oy = dig_area->locations[i].y;
	if (IS_WALL(levl[ox][oy].typ) || IS_DOOR(levl[ox][oy].typ)) {
	    watch_dig((struct monst *)0, ox, oy, TRUE);
	    if (*in_rooms(ox, oy, SHOPBASE)) shop_damage = TRUE;
	}
	digactualhole(ox, oy, BY_OBJECT, PIT);
    }
    free_explode_region(dig_area);
    for(i = 0; i < fiery_area->nlocations; i++) {
	ox = fiery_area->locations[i].x;
	oy = fiery_area->locations[i].y;
	if ((trap = t_at(ox, oy)) != 0 && trap->ttyp == LANDMINE)
	    blow_up_landmine(trap);
    }
    free_explode_region(fiery_area);
    if (gas_area->nlocations) {
	ztype = isyou ? ZT_SPELL(ZT_POISON_GAS) : -ZT_SPELL(ZT_POISON_GAS);
	do_explode(x, y, gas_area, ztype, d(3,6), WEAPON_CLASS,
	  EXPL_NOXIOUS, dest, isyou);
    }
    free_explode_region(gas_area);
    if (shop_damage) pay_for_damage("damage", FALSE);
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

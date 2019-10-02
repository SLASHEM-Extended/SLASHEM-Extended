/*	SCCS Id: @(#)dogmove.c	3.4	2002/09/10	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#include "mfndpos.h"
#include "edog.h"
#include "emin.h"
#include "epri.h"

/* #define DEBUG */	/* uncomment to enable debugging */

#ifdef DEBUG
# ifdef WIZARD
#define debugpline      if (wizard) pline
# else
#define debugpline      pline
# endif
#endif

extern boolean notonhead;

#ifdef OVL0

STATIC_DCL boolean dog_hunger(struct monst *,struct edog *);
STATIC_DCL int dog_invent(struct monst *,struct edog *,int);
STATIC_DCL int dog_goal(struct monst *,struct edog *,int,int,int);

STATIC_DCL struct obj *DROPPABLES(struct monst *);
STATIC_DCL boolean can_reach_location(struct monst *,XCHAR_P,XCHAR_P,
    XCHAR_P,XCHAR_P);
STATIC_DCL boolean could_reach_item(struct monst *, XCHAR_P,XCHAR_P);

STATIC_OVL struct obj *
DROPPABLES(mon)
register struct monst *mon;
{
	register struct obj *obj;
	struct obj *wep = MON_WEP(mon);
	boolean item1 = FALSE, item2 = FALSE;

	if (is_animal(mon->data) || mindless(mon->data))
		item1 = item2 = TRUE;
	if (!tunnels(mon->data) || !needspick(mon->data))
		item1 = TRUE;
	for(obj = mon->minvent; obj; obj = obj->nobj) {
		if (!item1 && is_pick(obj) && ((obj->otyp != DWARVISH_MATTOCK && obj->otyp != SOFT_MATTOCK && obj->otyp != ETERNIUM_MATTOCK)
						|| !which_armor(mon, W_ARMS))) {
			item1 = TRUE;
			continue;
		}
		if (!item2 && obj->otyp == UNICORN_HORN && !obj->cursed) {
			item2 = TRUE;
			continue;
		}
		if (obj->mstartinvent) continue;
		if (obj->mstartinventB) continue;
		if (obj->mstartinventC) continue;
		if (obj->petmarked) continue;

		if (!obj->owornmask && obj != wep) return obj;
	}
	return (struct obj *)0;
}

static NEARDATA const char nofetch[] = { BALL_CLASS, CHAIN_CLASS, ROCK_CLASS, 0 };

#endif /* OVL0 */

STATIC_OVL boolean cursed_object_at(struct monst *, int, int);

STATIC_VAR xchar gtyp, gx, gy;	/* type and position of dog's current goal */

STATIC_PTR void wantdoor(int, int, void *);

#ifdef OVLB
STATIC_OVL boolean
cursed_object_at(mtmp, x, y)
register struct monst *mtmp;
int x, y;
{
	struct obj *otmp;

	for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		/* [Tom] demons & undead don't care, though */
		/* [ALI] demons & undead avoid blessed items instead */
		if ((is_demon(mtmp->data) || is_undead(mtmp->data) || mtmp->egotype_undead) ?
		    otmp->blessed : otmp->cursed)
	{
#ifdef DEBUG
		debugpline("%s thinks %s at (%d,%d) is `cursed'",
		  noit_Monnam(mtmp), doname(otmp), x, y);
#endif
		return TRUE;
	}
	return FALSE;
}

int
dog_nutrition(mtmp, obj)
struct monst *mtmp;
struct obj *obj;
{
	int nutrit;

	/*
	 * It is arbitrary that the pet takes the same length of time to eat
	 * as a human, but gets more nutritional value.
	 */
	if (obj->oclass == FOOD_CLASS) {
	    if(obj->otyp == CORPSE) {
		mtmp->meating = (issoviet ? 5 : 2) + (mons[obj->corpsenm].cwt >> (issoviet ? 6 : 8) );
		nutrit = mons[obj->corpsenm].cnutrit;
	    } else {
		mtmp->meating = objects[obj->otyp].oc_delay;
		nutrit = objects[obj->otyp].oc_nutrition;
	    }

		/* it was insane how much nutrition they got from corpses, since I increased the average amount corpses give,
		 * so I hereby decide to reduce it. Use tripe rations or slime molds to keep your pets fed :P --Amy */
	    if (obj->otyp != CORPSE) {
		    switch(mtmp->data->msize) {
			case MZ_TINY: nutrit *= 8; break;
			case MZ_SMALL: nutrit *= 6; break;
			default:
			case MZ_MEDIUM: nutrit *= 5; break;
			case MZ_LARGE: nutrit *= 4; break;
			case MZ_HUGE: nutrit *= 3; break;
			case MZ_GIGANTIC: nutrit *= 2; break;
		    }
	    }
	    if(obj->oeaten) {
		mtmp->meating = eaten_stat(mtmp->meating, obj);
		nutrit = eaten_stat(nutrit, obj);
	    }
	} else if (obj->oclass == COIN_CLASS) {
	    mtmp->meating = (int)(obj->quan/2000) + 1;
	    if (mtmp->meating < 0) mtmp->meating = 1;
	    nutrit = (int)(obj->quan/20);
	    if (nutrit < 0) nutrit = 0;
	} else {
	    /* Unusual pet such as gelatinous cube eating odd stuff.
	     * meating made consistent with wild monsters in mon.c.
	     * nutrit made consistent with polymorphed player nutrit in
	     * eat.c.  (This also applies to pets eating gold.)
	     */
	    mtmp->meating = obj->owt/20 + 1;
	    nutrit = 5*(objects[obj->otyp].oc_nutrition + 1);
		/* old factor restored by Amy; the +1 is so that zero-weight objects don't give zero nutrition */
	}
	use_skill(P_PETKEEPING,1);

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_PETKEEPING)) {
			default: break;
			case P_BASIC: nutrit = (nutrit * 11 / 10); break;
			case P_SKILLED: nutrit = (nutrit * 12 / 10); break;
			case P_EXPERT: nutrit = (nutrit * 13 / 10); break;
			case P_MASTER: nutrit = (nutrit * 14 / 10); break;
			case P_GRAND_MASTER: nutrit = (nutrit * 15 / 10); break;
			case P_SUPREME_MASTER: nutrit = (nutrit * 16 / 10); break;
		}
	}

	if (isfriday) nutrit /= 2;
	return nutrit;
}

/* returns 2 if pet dies, otherwise 1 */
int
dog_eat(mtmp, obj, x, y, devour)
register struct monst *mtmp;
register struct obj * obj;
int x, y;
boolean devour;
{
	register struct edog *edog = EDOG(mtmp);
	boolean poly = FALSE, grow = FALSE, heal = FALSE;
	int nutrit;
	boolean vis = (cansee(x, y) || cansee(mtmp->mx, mtmp->my));
	boolean vampiric = is_vampire(mtmp->data);
	struct permonst *fptr = &mons[obj->corpsenm];
	struct monst *potentialpet;

	if(edog->hungrytime < monstermoves)
	    edog->hungrytime = monstermoves;
	nutrit = dog_nutrition(mtmp, obj);
	poly = polyfodder(obj);
	grow = mlevelgain(obj);
	heal = mhealup(obj);
	if (devour) {
	    if (mtmp->meating > 1) mtmp->meating /= 2;
	    if (nutrit > 1) nutrit = (nutrit * 3) / 4;
	}

	/* vampires only get 1/5 normal nutrition */
	if (vampiric) {
	    mtmp->meating = (mtmp->meating + 4) / 5;
	    nutrit = (nutrit + 4) / 5;
	}
	
	edog->hungrytime += nutrit;
	mtmp->mconf = 0;
	if (edog->mhpmax_penalty) {
	    /* no longer starving */
	    mtmp->mhpmax += edog->mhpmax_penalty;
	    edog->mhpmax_penalty = 0;
	}
	if (mtmp->mflee && mtmp->mfleetim > 1) mtmp->mfleetim /= 2;
	if (mtmp->mtame < 20) mtmp->mtame++;
	if (x != mtmp->mx || y != mtmp->my) {	/* moved & ate on same turn */
	    newsym(x, y);
	    newsym(mtmp->mx, mtmp->my);
	}
	if ((is_waterypool(x, y) || is_watertunnel(x,y)) && !Underwater) {
	    /* Don't print obj */
	    /* TODO: Reveal presence of sea monster (especially sharks) */
	} else
	/* hack: observe the action if either new or old location is in view */
	/* However, invisible monsters should still be "it" even though out of
	   sight locations should not. */
	if (vis) {
	    pline("%s %s %s.", mon_visible(mtmp) ? noit_Monnam(mtmp) : "It",
		  vampiric ? "drains" : devour ? "devours" : "eats",
		  (obj->oclass == FOOD_CLASS) ?
			singular(obj, doname) : doname(obj));
		if (issoviet && (obj->otyp == CORPSE) && (fptr->cnutrit < 1) ) pline("On on on-kha-kha-kha ya smeyus' nad vami truslivogo smertnyy! - Tip bloka l'da.");

		if (issoviet && (obj->otyp == CORPSE) && ((((potentialpet = get_mtraits(obj, FALSE)) != (struct monst *)0) ) && potentialpet->mtame) ) pline("on on on vash pitomets teper' ushel navsegda, potomu chto ya vsemogushchiy tip bloka l'da ya velichayshiy!");

	}
	/* It's a reward if it's DOGFOOD and the player dropped/threw it. */
	/* We know the player had it if invlet is set -dlc */
	if(dogfood(mtmp,obj) == DOGFOOD && obj->invlet)
#ifdef LINT
	    edog->apport = 0;
#else
	    edog->apport += (int)(200L/
		((long)edog->dropdist + monstermoves - edog->droptime));
#endif
	if (mtmp->data == &mons[PM_RUST_MONSTER] && obj->oerodeproof) {
	    /* The object's rustproofing is gone now */
	    obj->oerodeproof = 0;
	    mtmp->mstun = 1;
	    if (canseemon(mtmp) && flags.verbose) {
		pline("%s spits %s out in disgust!",
		      Monnam(mtmp), distant_name(obj,doname));
	    }
	} else if (vampiric) {
		/* Split Object */
		if (obj->quan > 1L) {
		    if(!carried(obj)) {
			(void) splitobj(obj, 1L);
		    } else {
		    	/* Carried */
			obj = splitobj(obj, obj->quan - 1L);
			
			freeinv(obj);
			if (inv_cnt() >= 52 && !merge_choice(invent, obj))
			    dropy(obj);
			else
			    obj = addinv(obj); /* unlikely but a merge is possible */			
		    }
#ifdef DEBUG
		    debugpline("split object,");
#endif
		}
		
		/* Take away blood nutrition */
	    	obj->oeaten = drainlevel(obj);
		obj->odrained = 1;
	} else if (obj == uball) {
	    unpunish();
	    delobj(obj);
	} else if (obj == uchain)
	    unpunish();
	else if (obj->quan > 1L && obj->oclass == FOOD_CLASS) {
	    obj->quan--;
	    obj->owt = weight(obj);
	} else
	    delobj(obj);

	if (poly) {
	    (void) mon_spec_poly(mtmp, (struct permonst *)0, 0L, FALSE,
		    cansee(mtmp->mx, mtmp->my), FALSE, FALSE);
#if 0
	    (void) newcham(mtmp, (struct permonst *)0, FALSE,
			   cansee(mtmp->mx, mtmp->my));
#endif
	}
	/* limit "instant" growth to prevent potential abuse */
	if (grow && (int) mtmp->m_lev < (int)mtmp->data->mlevel + 15) {
	    if (!grow_up(mtmp, (struct monst *)0)) return 2;
	}
	if (heal) mtmp->mhp = mtmp->mhpmax;
	return 1;
}

#endif /* OVLB */
#ifdef OVL0

/* hunger effects -- returns TRUE on starvation */
STATIC_OVL boolean
dog_hunger(mtmp, edog)
register struct monst *mtmp;
register struct edog *edog;
{
	if (monstermoves > edog->hungrytime + 500) {
	    if (!carnivorous(mtmp->data) && !herbivorous(mtmp->data) && !metallivorous(mtmp->data) && !mtmp->egotype_lithivore && !mtmp->egotype_metallivore && !mtmp->egotype_allivore && !lithivorous(mtmp->data)) {
		edog->hungrytime = monstermoves + 500;
		/* but not too high; it might polymorph */
	    } else if (!edog->mhpmax_penalty) {
		/* starving pets are limited in healing */
		int newmhpmax = mtmp->mhpmax / 3;
		mtmp->mconf = 1;
		edog->mhpmax_penalty = mtmp->mhpmax - newmhpmax;
		mtmp->mhpmax = newmhpmax;
		if (mtmp->mhp > mtmp->mhpmax)
		    mtmp->mhp = mtmp->mhpmax;
		if (mtmp->mhp < 1) goto dog_died;
		if (cansee(mtmp->mx, mtmp->my))
		    pline("%s is confused from hunger.", Monnam(mtmp));
		else if (couldsee(mtmp->mx, mtmp->my))
		    beg(mtmp);
		else
		    You_feel("worried about %s.", y_monnam(mtmp));
		stop_occupation();
	    } else if (monstermoves > edog->hungrytime + 750 || mtmp->mhp < 1) {
 dog_died:
		if (mtmp->mleashed && mtmp != u.usteed)
		    Your("leash goes slack.");
		else if (cansee(mtmp->mx, mtmp->my))
		    pline("%s starves.", Monnam(mtmp));
		else {
		    You_feel("%s for a moment.",
			FunnyHallu ? "bummed" : "sad");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Tipichnyy igrok. Vy dazhe ne sposobny kormit' vashego pitomtsa." : "Tschwieaeaeh!");

		}
		mondied(mtmp);
		return(TRUE);
	    }
	}
	return(FALSE);
}

/* do something with object (drop, pick up, eat) at current position
 * returns 1 if object eaten (since that counts as dog's move), 2 if died
 */
STATIC_OVL int
dog_invent(mtmp, edog, udist)
register struct monst *mtmp;
register struct edog *edog;
int udist;
{
	/* KMH, balance patch -- quantity picked up should depend on dog's level */
	int dogquan = /*10*/5 * mtmp->m_lev; /* halved by Amy */
	register int omx, omy;
	struct obj *obj;
/*
	struct obj *floor_obj;
	int temp_quan;
 */
	if (mtmp->msleeping || !mtmp->mcanmove) return(0);

	omx = mtmp->mx;
	omy = mtmp->my;

	/* if we are carrying sth then we drop it (perhaps near @) */
	/* Note: if apport == 1 then our behaviour is independent of udist */
	/* Use udist+1 so steed won't cause divide by zero */
#ifndef GOLDOBJ
	if(DROPPABLES(mtmp) || mtmp->mgold) {
#else
	if(DROPPABLES(mtmp)) {
#endif
	    if (!rn2(udist+1) || !rn2(edog->apport))
		if(rn2(10) < edog->apport){
		    relobj(mtmp, (int)mtmp->minvis, TRUE);
		    if(edog->apport > 1) edog->apport--;
		    edog->dropdist = udist;		/* hpscdi!jon */
		    edog->droptime = moves;
		}
	} else {
	    if((obj=level.objects[omx][omy]) && !index(nofetch,obj->oclass)
#ifdef MAIL
			&& obj->otyp != SCR_MAIL
#endif
									){
		int edible = dogfood(mtmp, obj);

		if (((edible <= CADAVER ||
			/* starving pet is more aggressive about eating */
			(edog->mhpmax_penalty && edible == ACCFOOD)) &&
		    could_reach_item(mtmp, obj->ox, obj->oy)) && u.petcaneat)
		    return dog_eat(mtmp, obj, omx, omy, FALSE);

		/* [Tom] demonic & undead pets don't mind cursed items */                
		if(can_carry(mtmp, obj) && u.petcollectitems && (issoviet || !Has_contents(obj)) &&
		  !(obj == uchain) && !(obj == uball) &&
		  could_reach_item(mtmp, obj->ox, obj->oy) &&
		  (!obj->cursed || is_demon(mtmp->data) || is_undead(mtmp->data) || mtmp->egotype_undead) &&
		  (!obj->blessed || (!is_demon(mtmp->data) && !is_undead(mtmp->data) && (!mtmp->egotype_undead) ))) {
		    if(rn2(20) < edog->apport+3) {
			if (rn2(udist) || !rn2(edog->apport)) {
			    if ((!nohands(mtmp->data)) ||
						/* KMH, balance patch -- 10*level */
						(obj->quan <= dogquan))
			    {
			    if (cansee(omx, omy) && flags.verbose)
				pline("%s picks up %s.", Monnam(mtmp),
				    distant_name(obj, doname));
			    obj_extract_self(obj);
			    newsym(omx,omy);
			    (void) mpickobj(mtmp,obj,FALSE);
			    }                            
			    else /* picking up a few objects from a pile... */
				/* KMH -- fix picking up zero quantity */
			    if (dogquan > 0) {
				if (obj->oclass == COIN_CLASS) {
				    /* KMH, balance patch -- 10*level */

#ifndef GOLDOBJ
				    obj->quan -= dogquan;
				    if (cansee(omx, omy) && flags.verbose)
					pline("%s picks up %d gold pieces.", 
							Monnam(mtmp),
							dogquan);
				    mtmp->mgold += dogquan;
#else
						if (obj->quan != dogquan)
							obj = splitobj(obj, dogquan);
				    if (cansee(omx, omy) && flags.verbose)
							pline("%s picks up %s.", 
							Monnam(mtmp),
							doname(obj));
							obj_extract_self(obj);
				    	newsym(omx,omy);
							(void) mpickobj(mtmp,obj,FALSE);
#endif
				} else {
/*
                                struct obj *floor_obj;
                                int temp_quan;

                                obj->quan -= dogquan;
				temp_quan = obj->quan;
				floor_obj = level.objects[omx][omy];
				mpickobj(mtmp,obj,FALSE);
				obj->quan = dogquan;
				if (cansee(omx, omy) && flags.verbose)
				pline("%s picks up %s.", Monnam(mtmp),
				    distant_name(obj, doname));
				floor_obj->quan = temp_quan;*/
				}
			    }
			    if (attacktype(mtmp->data, AT_WEAP) &&
					mtmp->weapon_check == NEED_WEAPON) {
				mtmp->weapon_check = NEED_HTH_WEAPON;
				(void) mon_wield_item(mtmp);
			    }
			    m_dowear(mtmp, FALSE);
			}
		    }
		}
	    }
	}
	return 0;
}

/* set dog's goal -- gtyp, gx, gy
 * returns -1/0/1 (dog's desire to approach player) or -2 (abort move)
 */
STATIC_OVL int
dog_goal(mtmp, edog, after, udist, whappr)
register struct monst *mtmp;
struct edog *edog;
int after, udist, whappr;
{
	register int omx, omy;
	boolean in_masters_sight, dog_has_minvent;
	register struct obj *obj;
	xchar otyp;
	int appr;

	/* Steeds don't move on their own will */
	if (mtmp == u.usteed)
		return (-2);

	omx = mtmp->mx;
	omy = mtmp->my;

	in_masters_sight = couldsee(omx, omy);
	dog_has_minvent = (DROPPABLES(mtmp) != 0);

	if (!edog || mtmp->mleashed) {	/* he's not going anywhere... */
	    gtyp = APPORT;
	    gx = u.ux;
	    gy = u.uy;
	} else {
#define DDIST(x,y) (dist2(x,y,omx,omy))
#define SQSRCHRADIUS 5
	    int min_x, max_x, min_y, max_y;
	    register int nx, ny;

	    gtyp = UNDEF;	/* no goal as yet */
	    gx = gy = 0;	/* suppress 'used before set' message */

	    if ((min_x = omx - SQSRCHRADIUS) < 1) min_x = 1;
	    if ((max_x = omx + SQSRCHRADIUS) >= COLNO) max_x = COLNO - 1;
	    if ((min_y = omy - SQSRCHRADIUS) < 0) min_y = 0;
	    if ((max_y = omy + SQSRCHRADIUS) >= ROWNO) max_y = ROWNO - 1;

	    /* nearby food is the first choice, then other objects */
	    for (obj = fobj; obj; obj = obj->nobj) {
		nx = obj->ox;
		ny = obj->oy;
		if (nx >= min_x && nx <= max_x && ny >= min_y && ny <= max_y) {
		    otyp = dogfood(mtmp, obj);
		    /* skip inferior goals */
		    if (otyp > gtyp || otyp == UNDEF)
			continue;
		    /* avoid cursed items unless starving */
		    if (cursed_object_at(mtmp, nx, ny) &&
			    !(edog->mhpmax_penalty && otyp < MANFOOD))
			continue;
		    /* skip completely unreacheable goals */
		    if (!could_reach_item(mtmp, nx, ny) ||
		        !can_reach_location(mtmp, mtmp->mx, mtmp->my, nx, ny))
			continue;
		    if (otyp < MANFOOD) {
			if (otyp < gtyp || DDIST(nx,ny) < DDIST(gx,gy)) {
			    gx = nx;
			    gy = ny;
			    gtyp = otyp;
			}
		    } else if(gtyp == UNDEF && in_masters_sight &&
			      !dog_has_minvent &&
			      (!levl[omx][omy].lit || levl[u.ux][u.uy].lit) &&
			      (otyp == MANFOOD || m_cansee(mtmp, nx, ny)) &&
			      edog->apport > rn2(8) && u.petcollectitems &&
			      can_carry(mtmp,obj)) {
			gx = nx;
			gy = ny;
			gtyp = APPORT;
		    }
		}
	    }
	}

	/* follow player if appropriate */
	if (gtyp == UNDEF ||
	    (gtyp != DOGFOOD && gtyp != APPORT && monstermoves < edog->hungrytime)) {
		gx = u.ux;
		gy = u.uy;
		if (after && udist <= 4 && gx == u.ux && gy == u.uy)
			return(-2);
		appr = (udist >= 9) ? 1 : (mtmp->mflee) ? -1 : 0;
		if (udist > 1) {
			if (!IS_ROOM(levl[u.ux][u.uy].typ) || !rn2(4) ||
			   whappr ||
			   (dog_has_minvent && rn2(edog->apport)))
				appr = 1;
		}
		/* if you have dog food it'll follow you more closely */
		if (appr == 0) {
			obj = invent;
			while (obj) {
				if(dogfood(mtmp, obj) == DOGFOOD) {
					appr = 1;
					break;
				}
				obj = obj->nobj;
			}
		}
	} else
	    appr = 1;	/* gtyp != UNDEF */
	if(mtmp->mconf)
	    appr = 0;

#define FARAWAY (COLNO + 2)		/* position outside screen */
	if (gx == u.ux && gy == u.uy && !in_masters_sight) {
	    register coord *cp;

	    cp = gettrack(omx,omy);
	    if (cp) {
		gx = cp->x;
		gy = cp->y;
		if(edog) edog->ogoal.x = 0;
	    } else {
		/* assume master hasn't moved far, and reuse previous goal */
		if(edog && edog->ogoal.x &&
		   ((edog->ogoal.x != omx) || (edog->ogoal.y != omy))) {
		    gx = edog->ogoal.x;
		    gy = edog->ogoal.y;
		    edog->ogoal.x = 0;
		} else {
		    int fardist = FARAWAY * FARAWAY;
		    gx = gy = FARAWAY; /* random */
		    do_clear_area(omx, omy, 9, wantdoor,
				  (void *)&fardist);

		    /* here gx == FARAWAY e.g. when dog is in a vault */
		    if (gx == FARAWAY || (gx == omx && gy == omy)) {
			gx = u.ux;
			gy = u.uy;
		    } else if(edog) {
			edog->ogoal.x = gx;
			edog->ogoal.y = gy;
		    }
		}
	    }
	} else if(edog) {
	    edog->ogoal.x = 0;
	}
	if (!u.petcanfollow && appr == 1) appr = 0;
	if ((PetAIScrewed || u.uprops[PET_AI_SCREWED].extrinsic || have_petaistone()) && appr == 1) appr = 0;
	return appr;
}

#ifdef DEBUG
#define CHECK_ALLOW(flag,str)	if ((allowflags & (flag)) == (flag)) { \
				    allowflags ^= (flag); \
				    if (bp != buf) { *bp++=','; *bp++=' '; } \
				    strcpy(bp, str); \
				    bp += strlen(bp); \
				} else

STATIC_OVL char *
allow_set(allowflags)
long allowflags;
{
    static char buf[500];
    char *bp = buf;
    if (allowflags == 0)
	return "(none)";
    *bp = '\0';
    CHECK_ALLOW(ALLOW_TRAPS, "can enter traps");
    CHECK_ALLOW(ALLOW_U, "can attack you");
    CHECK_ALLOW(ALLOW_M, "can attack other monsters");
    CHECK_ALLOW(ALLOW_TM, "can attack tame monsters");
    CHECK_ALLOW(NOTONL, "avoids direct line to player");
    CHECK_ALLOW(OPENDOOR, "opens closed doors");
    CHECK_ALLOW(UNLOCKDOOR, "unlocks locked doors");
    CHECK_ALLOW(BUSTDOOR, "breaks any doors");
    CHECK_ALLOW(ALLOW_ROCK, "pushes rocks");
    CHECK_ALLOW(ALLOW_WALL, "walks thru walls");
    CHECK_ALLOW(ALLOW_DIG, "digs");
    CHECK_ALLOW(ALLOW_SANCT, "enters temples");
    CHECK_ALLOW(ALLOW_SSM, "ignores scare monster");
    CHECK_ALLOW(NOGARLIC, "hates garlic");
    if (allowflags) {
	if (bp != buf) { *bp++=','; *bp++=' '; }
	sprintf(bp, "0x%lX", allowflags);
    }
    return buf;
}

#undef CHECK_ALLOW
#endif

boolean
betrayed(mtmp)
register struct monst *mtmp;
{
    boolean has_edog = !mtmp->isminion;
    struct edog *edog = EDOG(mtmp);
    int udist = distu(mtmp->mx, mtmp->my);

	if (Role_if(PM_SLAVE_MASTER) && rn2(10)) return FALSE; /* can keep monsters tame more easily --Amy */

    if (udist < 4 && has_edog && (!mtmp->isspell || (mtmp->data == &mons[PM_SUMMONED_FIRE_GOLEM]) || (mtmp->data == &mons[PM_ULTRA_EVIL_QUASIT]) ) && !rn2(3)
		    && (can_betray(mtmp->data) || (isfriday && !rn2(10)) || (is_jonadabmonster(mtmp->data)) || (mtmp->data->mlevel >= 50) || (mtmp->data == &mons[PM_SUMMONED_FIRE_GOLEM]) || (mtmp->data == &mons[PM_ULTRA_EVIL_QUASIT]) || (uarmc && uarmc->oartifact == ART_ARTIFICIAL_FAKE_DIFFICULTY && !rn2(3) ) || Role_if(PM_FAILED_EXISTENCE) || (u.uprops[REBELLION_EFFECT].extrinsic || (Role_if(PM_UNDEAD_SLAYER) && is_undead(mtmp->data)) || Rebellions || have_rebelstone() || (uarmf && uarmf->oartifact == ART_KATIE_MELUA_S_FLEECINESS) ) || (mtmp->m_lev >= 40) )
		    /*&& !mindless(mtmp->data)*/ /* mindless creatures may still decide to attack randomly --Amy */
		    && (mtmp->mhp >= u.uhp || !rn2(5) || (u.uprops[REBELLION_EFFECT].extrinsic || (Role_if(PM_UNDEAD_SLAYER) && is_undead(mtmp->data)) || Rebellions || have_rebelstone() || (is_jonadabmonster(mtmp->data)) || (isfriday && !rn2(10)) || (mtmp->data->mlevel >= 50) || (uarmf && uarmf->oartifact == ART_KATIE_MELUA_S_FLEECINESS) ) || (mtmp->data == &mons[PM_SUMMONED_FIRE_GOLEM]) || (mtmp->data == &mons[PM_ULTRA_EVIL_QUASIT]) || (uarmc && uarmc->oartifact == ART_ARTIFICIAL_FAKE_DIFFICULTY && !rn2(3) ) || Role_if(PM_FAILED_EXISTENCE))	/* Pet is buff enough */
		    && rn2(22) > mtmp->mtame	/* Roll against tameness */
		    && !((rnd(30 - ACURR(A_CHA))) < 4) /* Roll against charisma */
		    && rn2(edog->abuse + rnd(2) )) {
	/* Treason */
	if (canseemon(mtmp))
	    pline("%s turns on you!", Monnam(mtmp));
	else
	    You_feel("uneasy about %s.", y_monnam(mtmp));
	mtmp->mpeaceful = 0;
	mtmp->mtame = 0;
	mtmp->mtraitor = TRUE;
	mtmp->isspell = 0;
	mtmp->uexp = 0;

	/* if the monster is a domestic animal, you could just re-tame it indefinitely... prevent that :P --Amy */
	if (!rn2(5)) {
		mtmp->mfrenzied = 1;
		if (canseemon(mtmp))
		    pline("In fact, %s apparently decides to stop at nothing until you're dead!", mon_nam(mtmp));
	}

	/* Do we need to call newsym() here? */
	newsym(mtmp->mx, mtmp->my);
	return TRUE;
    }
    return FALSE;
}

/* return 0 (no move), 1 (move) or 2 (dead) */
int
dog_move(mtmp, after)
register struct monst *mtmp;
register int after;	/* this is extra fast monster movement */
{
	int omx, omy;		/* original mtmp position */
	int appr, whappr, udist;
	int i, j, k;
	register struct edog *edog = EDOG(mtmp);
	struct obj *obj = (struct obj *) 0;
	xchar otyp;
	boolean has_edog, cursemsg[9], is_spell, do_eat = FALSE;
	xchar nix, niy;		/* position mtmp is (considering) moving to */
	register int nx, ny;	/* temporary coordinates */
	xchar cnt, uncursedcnt, chcnt;
	int chi = -1, nidist, ndist;
	coord poss[9];
	long info[9], allowflags;
#define GDIST(x,y) (dist2(x,y,gx,gy))

	if (mtmp->willbebanished) {
		mtmp->willbebanished = FALSE;
		if (u.usteed && u.usteed == mtmp) {
			if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed)) ) { pline("You shudder for a moment.");
			}
			if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || (iszapem && !(u.zapemescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) {
			pline("For some reason you resist the banishment!");
			}

			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

			if (!u.banishmentbeam) {
				u.banishmentbeam = 1;
				nomul(-2, "being banished", FALSE); /* because it's not called until you get another turn... */
			}

		} else {
			u_teleport_monB(mtmp, TRUE);
		}
		return 0;
	}

	/*
	 * Tame Angels have isminion set and an ispriest structure instead of
	 * an edog structure.  Fortunately, guardian Angels need not worry
	 * about mundane things like eating and fetching objects, and can
	 * spend all their energy defending the player.  (They are the only
	 * monsters with other structures that can be tame.)
	 */
	has_edog = !mtmp->isminion;

	/*
	 * Similar to Angels and Guardians are spell beings - temporary
	 * magical manifestations of the spellcaster's mind.
	 * They don't eat/pickup objects - only fight.
	 * But,  they aren't dismissed by conflict.
	 */
	is_spell = mtmp->isspell;

	omx = mtmp->mx;
	omy = mtmp->my;
	if (has_edog && !is_spell && dog_hunger(mtmp, edog)) return(2);      /* starved */

	udist = distu(omx,omy);
	/* Let steeds eat and maybe throw rider during Conflict */
	if (mtmp == u.usteed) {
	    if (Conflict && (issoviet || !rn2(100)) && !resist(mtmp, RING_CLASS, 0, 0)) {
		/* happens much less often now, so riding while causing conflict is no longer impossible --Amy */

		if (!mayfalloffsteed()) {
			dismount_steed(DISMOUNT_THROWN);
			return (1);
		}
	    }
	    udist = 1;
	} else
	/* maybe we tamed him while being swallowed --jgm */
	if (!udist) return(0);

	/* Intelligent pets may rebel (apart from minions, spell beings) */
	/* if it's a species that's supposed to not be tameable, make it happen much more often --Amy */
	if (!rn2( cannot_be_tamed(mtmp->data) ? 85 : 850) && betrayed(mtmp)) return 1;
	if (!rn2(85) && is_jonadabmonster(mtmp->data) && betrayed(mtmp)) return 1;
	if (!rn2(850) && (mtmp->data->mlevel >= 50) && betrayed(mtmp)) return 1;
	if (Aggravate_monster && !rn2( cannot_be_tamed(mtmp->data) ? 85 : 850) && betrayed(mtmp)) return 1;
	if (!rn2(10) && mtmp->data == &mons[PM_SUMMONED_FIRE_GOLEM] && betrayed(mtmp)) return 1;
	if (mtmp->data == &mons[PM_ULTRA_EVIL_QUASIT]) { /* REALLY doesn't want to be tame --Amy */
		if (betrayed(mtmp)) return 1;
		if (betrayed(mtmp)) return 1;
		if (betrayed(mtmp)) return 1;
		if (betrayed(mtmp)) return 1;
		if (betrayed(mtmp)) return 1;
		if (betrayed(mtmp)) return 1;
		if (betrayed(mtmp)) return 1;
		if (betrayed(mtmp)) return 1;
		if (betrayed(mtmp)) return 1;
		if (betrayed(mtmp)) return 1;
	}
	if ((u.uprops[REBELLION_EFFECT].extrinsic || Rebellions || have_rebelstone() || (uarmf && uarmf->oartifact == ART_KATIE_MELUA_S_FLEECINESS) ) && !rn2(85) && betrayed(mtmp)) return 1;
	if (Role_if(PM_UNDEAD_SLAYER) && is_undead(mtmp->data)) return 1;

	/* If you abused your pet, it will _very_ slowly time out. --Amy */
	if (!rn2(10000) && has_edog && edog->abuse) {
		edog->abuse--;
		if (!(PlayerCannotUseSkills)) {
			if (!rn2(10) && edog->abuse && P_SKILL(P_PETKEEPING) >= P_BASIC) edog->abuse--;
			if (!rn2(10) && edog->abuse && P_SKILL(P_PETKEEPING) >= P_SKILLED) edog->abuse--;
			if (!rn2(10) && edog->abuse && P_SKILL(P_PETKEEPING) >= P_EXPERT) edog->abuse--;
			if (!rn2(10) && edog->abuse && P_SKILL(P_PETKEEPING) >= P_MASTER) edog->abuse--;
			if (!rn2(10) && edog->abuse && P_SKILL(P_PETKEEPING) >= P_GRAND_MASTER) edog->abuse--;
			if (!rn2(10) && edog->abuse && P_SKILL(P_PETKEEPING) >= P_SUPREME_MASTER) edog->abuse--;
		}
		if (edog->abuse < 0) edog->abuse = 0; /* fail safe */
	}


	nix = omx;	/* set before newdogpos */
	niy = omy;
	cursemsg[0] = FALSE;	/* lint suppression */
	info[0] = 0;		/* ditto */

	if (has_edog && !is_spell) {
	    j = dog_invent(mtmp, edog, udist);
	    if (j == 2) return 2;		/* died */
	    else if (j == 1) goto newdogpos;	/* eating something */

	    whappr = (monstermoves - edog->whistletime < 5);
	} else
	    whappr = 0;

	appr = dog_goal(mtmp, (has_edog && !is_spell) ? edog : (struct edog *)0,
							after, udist, whappr);
#ifdef DEBUG
	{
	    char *goal;
	    switch(gtyp)
	    {
		case DOGFOOD:	goal = "dogfood"; break;
		case CADAVER:	goal = "cadaver"; break;
		case ACCFOOD:	goal = "accfood"; break;
		case MANFOOD:	goal = "manfood"; break;
		case APPORT:	goal = "apport"; break;
		case POISON:	goal = "poison"; break;
		case UNDEF:	goal = "undef"; break;
		case TABU:	goal = "tabu"; break;
		default:	goal = "???"; break;
	    }
	    debugpline("G(%s): %s @ (%d,%d), appr = %d",
	      mon_nam(mtmp), goal, gx, gy, appr);
	}
#endif
	if (appr == -2) return(0);

	allowflags = ALLOW_M | ALLOW_TRAPS | ALLOW_SSM | ALLOW_SANCT;
	if (passes_walls(mtmp->data) || (mtmp->egotype_wallwalk) ) allowflags |= (ALLOW_ROCK | ALLOW_WALL);
	if (passes_bars(mtmp->data) && !In_sokoban(&u.uz))
	    allowflags |= ALLOW_BARS;
	if (throws_rocks(mtmp->data)) allowflags |= ALLOW_ROCK;
	if (Conflict && !resist(mtmp, RING_CLASS, 0, 0) && In_endgame(&u.uz)) {
	    allowflags |= ALLOW_U;
	    if (!has_edog && !is_spell) {
		coord mm;
		/* Guardian angel refuses to be conflicted; rather,
		 * it disappears, angrily, and sends in some nasties
		 */
		if (canspotmon(mtmp)) {
		    pline("%s rebukes you, saying:", Monnam(mtmp));
		    verbalize("Since you desire conflict, have some more!");
		}
		mongone(mtmp);
		i = rnd(4);
		while(i--) {
		    mm.x = u.ux;
		    mm.y = u.uy;
		    if(enexto(&mm, mm.x, mm.y, &mons[PM_ANGEL]))
			(void) mk_roamer(&mons[PM_ANGEL], u.ualign.type,
					 mm.x, mm.y, FALSE);
		}
		return(2);

	    }
	}
	if (StrongConflict && !resist(mtmp, RING_CLASS, 0, 0) && In_endgame(&u.uz)) {
	    allowflags |= ALLOW_U;
	    if (!has_edog && !is_spell) {
		coord mm;
		/* Guardian angel refuses to be conflicted; rather,
		 * it disappears, angrily, and sends in some nasties
		 */
		if (canspotmon(mtmp)) {
		    pline("%s rebukes you, saying:", Monnam(mtmp));
		    verbalize("Since you desire conflict, have some more!");
		}
		mongone(mtmp);
		i = rnd(4);
		while(i--) {
		    mm.x = u.ux;
		    mm.y = u.uy;
		    if(enexto(&mm, mm.x, mm.y, &mons[PM_ANGEL]))
			(void) mk_roamer(&mons[PM_ANGEL], u.ualign.type,
					 mm.x, mm.y, FALSE);
		}
		return(2);

	    }
	}

	/* ALI -- Mindless pets shouldn't attack monsters when
	 * scared; they have no sense of allegiance to the hero,
	 * only self-preservation. This prevents weak pets blocking
	 * your exit from a shop by constantly missing shopkeeper.
	 */
	if (mindless(mtmp->data) && mtmp->mflee)
	    allowflags &= ~ALLOW_M;

	if (!Conflict && !mtmp->mconf &&
	    mtmp == u.ustuck && !sticks(youmonst.data)) {
	    unstuck(mtmp);	/* swallowed case handled above */
	    You("get released!");
	}
	if (!nohands(mtmp->data) && !verysmall(mtmp->data)) {
		allowflags |= OPENDOOR;
		if (m_carrying(mtmp, SKELETON_KEY)) allowflags |= BUSTDOOR;
		if (m_carrying(mtmp, SECRET_KEY)) allowflags |= BUSTDOOR;
	}
	if (is_giant(mtmp->data)) allowflags |= BUSTDOOR;
	if (tunnels(mtmp->data)) allowflags |= ALLOW_DIG;
	cnt = mfndpos(mtmp, poss, info, allowflags);
#ifdef DEBUG
	debugpline("%d positions found with allow: %s", cnt,
	  allow_set(allowflags));
	for (i = 0; i < cnt; i++) {
	    debugpline("[%d] %s @ (%d, %d)", i,
	      allow_set(info[i]), poss[i].x, poss[i].y);
	}
#endif

	/* Normally dogs don't step on cursed items, but if they have no
	 * other choice they will.  This requires checking ahead of time
	 * to see how many uncursed item squares are around.
	 */

	uncursedcnt = 0;
	for (i = 0; i < cnt; i++) {
		nx = poss[i].x; ny = poss[i].y;
		if (MON_AT(nx,ny) && !(info[i] & ALLOW_M)) continue;
		if (cursed_object_at(mtmp, nx, ny)) continue;
		uncursedcnt++;
	}

	chcnt = 0;
	chi = -1;
	nidist = GDIST(nix,niy);

	for (i = 0; i < cnt; i++) {
		nx = poss[i].x;
		ny = poss[i].y;
		cursemsg[i] = FALSE;

		/* if leashed, we drag him along. */
		if (mtmp->mleashed && distu(nx, ny) > 4) continue;

		/* if a guardian, try to stay close by choice */
		if ((!has_edog || is_spell) &&
		    (j = distu(nx, ny)) > 16 && j >= udist) continue;

		if ((info[i] & ALLOW_M) && MON_AT(nx, ny)) {
		    int mstatus;
		    register struct monst *mtmp2 = m_at(nx,ny);
		    aligntyp align1, align2; /* For priests, minions etc. */

			if (!u.petattackenemies) continue;

			if (mtmp2->data == &mons[PM_CHAREY]) continue;

		    if (mtmp->isminion) align1 = EMIN(mtmp)->min_align;
		    else if (is_unicorn(mtmp->data))
			align1 = sgn(mtmp->data->maligntyp);
		    else if (mtmp->ispriest) align1 = EPRI(mtmp)->shralign;
		    else align1 = A_NONE;
		    if (mtmp2->isminion) align2 = EMIN(mtmp2)->min_align;
		    else if (is_unicorn(mtmp2->data))
			align2 = sgn(mtmp2->data->maligntyp);
		    else if (mtmp2->ispriest) align2 = EPRI(mtmp2)->shralign;
		    else align2 = A_NONE;

		    /* Mindless monsters and spelled monsters have no fear of 
		     * attacking higher level monsters 
		     */
		    if (((int)mtmp2->m_lev >= (int)mtmp->m_lev+2 && !is_spell && 
			    !mindless(mtmp->data)) ||
			(mtmp2->data == &mons[PM_FLOATING_EYE] && rn2(10) &&
			 mtmp->mcansee && haseyes(mtmp->data) && mtmp2->mcansee
			 && !mtmp2->minvisreal && (perceives(mtmp->data) || !mtmp2->minvis)) ||
			(mtmp2->data==&mons[PM_GELATINOUS_CUBE] && rn2(10)) ||
			(mtmp2->data==&mons[PM_GAS_SPORE] && rn2(16)) ||
			(!attacktype(mtmp->data, AT_EXPL) &&
			 max_passive_dmg(mtmp2, mtmp) >= mtmp->mhp) ||
			/* Minions/Angels don't attack
			 * coaligned minions/priests/angels/unicorns.
			 */
			(align1 == align2 && align1 != A_NONE) ||
			( (mtmp->mhp*10 < mtmp->mhpmax) && !Conflict ) ||
			((mtmp->mhp*4 < mtmp->mhpmax
			  || mtmp2->data->msound == MS_GUARDIAN
			  || mtmp2->data->msound == MS_LEADER) &&
	/* the activistor quest shouldn't be trivialized by bringing a high-level pet or using charm monster. --Amy */
			 mtmp2->mpeaceful && !Conflict) ||
			 (Role_if(PM_ACTIVISTOR) && mtmp2->data == &mons[PM_TOPMODEL]) || 
			 (Race_if(PM_PEACEMAKER) && mtmp2->data == &mons[PM_TOPMODEL]) || 
	/* for Rodneyan race characters, the real Rodney is supposed to be their buddy so he doesn't get attacked */
			 (mtmp2->data == &mons[PM_WIZARD_OF_YENDOR] && Race_if(PM_RODNEYAN)) ||
			 (mtmp2->data == &mons[PM_THE_ZRUTINATOR] && Race_if(PM_RODNEYAN)) ||
	/* troves only drop their items if the player kills them, so keep pets away from them */
			 (mtmp2->data->mlet == S_TROVE) ||
	/* fear and other status effects should screw over pets */
			 (mtmp->mflee && rn2(10)) || (mtmp->mstun && rn2(4)) || (mtmp->mconf && rn2(2)) || (mtmp->mblinded && haseyes(mtmp->data) && rn2(3)) ||
	/* invisible monsters need see invis to be attacked reliably */
			 (mtmp2->minvis && haseyes(mtmp->data) && !perceives(mtmp->data) && rn2(4)) ||
			 (mtmp2->minvisreal && rn2(haseyes(mtmp->data) ? 5 : 3)) ||
	/* petshielder egotype is never attacked by pets either */
			 (mtmp2->egotype_petshielder || mtmp2->data == &mons[PM_TUXIE]) ||
	/* directive can be used to make them not attack peacefuls */
			 (u.petattackenemies == 1 && mtmp2->mpeaceful) ||
	/* Moldoux is special-cased */
			 (mtmp2->data == &mons[PM_MOLDOUX__THE_DEFENCELESS_MOLD]) ||
	/* if Izchak dies, the player gets disintegrated, so stop pets from killing them
	   well screw it, just completely prevent them from attacking shopkeepers, priests and vault guards --Amy */
	/* In Soviet Russia, pets are totally stupid (in fact, even more so than modders). They simply attack everything,
	 * even if it's something the player might want to use. --Amy */
			 (!issoviet && mtmp2->isshk ) || (!issoviet && mtmp2->data == &mons[PM_BLACKSMITH]) || (!issoviet && mtmp2->isgd ) || (!issoviet && mtmp2->ispriest ) ||
			   (touch_petrifies(mtmp2->data) &&
				!resists_ston(mtmp)))
			continue;

		    if (after) return(0); /* hit only once each move */

		    notonhead = 0;
		    mstatus = mattackm(mtmp, mtmp2);

		    /* aggressor (pet) died */
		    if (mstatus & MM_AGR_DIED) return 2;

		    if ((mstatus & MM_HIT) && !(mstatus & MM_DEF_DIED) &&
			    rn2(4) && mtmp2->mlstmv != monstermoves &&
			    !onscary(mtmp->mx, mtmp->my, mtmp2) &&
			    /* monnear check needed: long worms hit on tail */
			    monnear(mtmp2, mtmp->mx, mtmp->my)) {
			mstatus = mattackm(mtmp2, mtmp);  /* return attack */
			if (mstatus & MM_DEF_DIED) return 2;
		    } else if (!(mstatus & MM_HIT) && !(mstatus & MM_DEF_DIED) && !rn2(10) && mtmp2->mlstmv != monstermoves &&
			    !onscary(mtmp->mx, mtmp->my, mtmp2) && monnear(mtmp2, mtmp->mx, mtmp->my)) {

			/* Amy edit: allow monsters to occasionally fight back even if your pet missed them */
			mstatus = mattackm(mtmp2, mtmp);  /* return attack */
			if (mstatus & MM_DEF_DIED) return 2;
		    }

		    return 0;
		}

		{   /* Dog avoids harmful traps, but perhaps it has to pass one
		     * in order to follow player.  (Non-harmful traps do not
		     * have ALLOW_TRAPS in info[].)  The dog only avoids the
		     * trap if you've seen it, unlike enemies who avoid traps
		     * if they've seen some trap of that type sometime in the
		     * past.  (Neither behavior is really realistic.)
		     */
		    struct trap *trap;

		    if ((info[i] & ALLOW_TRAPS) && (trap = t_at(nx,ny))) {
			if (mtmp->mleashed) {
			    if (flags.soundok) whimper(mtmp);
			} else
			    /* 1/40 chance of stepping on it anyway, in case
			     * it has to pass one to follow the player...
			     */
			    if (trap->tseen && rn2(40)) continue;
		    }
		}

		/* dog eschews cursed objects, but likes dog food */
		/* [Tom] except demons & undead, who don't care */  
		/* (minion isn't interested; `cursemsg' stays FALSE) */
		if (has_edog && !is_spell) {
		for (obj = level.objects[nx][ny]; obj; obj = obj->nexthere) {
		    if ((obj->cursed) && has_edog && !is_demon(mtmp->data) 
		    && !is_undead(mtmp->data) && (!mtmp->egotype_undead) ) cursemsg[i] = TRUE;
		    if (obj->blessed && has_edog && (is_demon(mtmp->data) 
		    || is_undead(mtmp->data) || mtmp->egotype_undead)) cursemsg[i] = TRUE;
		    else if ((otyp = dogfood(mtmp, obj)) < MANFOOD && u.petcaneat &&
			     (otyp < ACCFOOD || edog->hungrytime <= monstermoves)) {
			/* Note: our dog likes the food so much that he
			 * might eat it even when it conceals a cursed object */
			nix = nx;
			niy = ny;
			chi = i;
			do_eat = TRUE;
			cursemsg[i] = FALSE;	/* not reluctant */
			goto newdogpos;
		    }
		}
		}
		/* didn't find something to eat; if we saw a cursed item and
		   aren't being forced to walk on it, usually keep looking */
		if (cursemsg[i] && !mtmp->mleashed && uncursedcnt > 0 &&
		    rn2(13 * uncursedcnt)) continue;

		/* lessen the chance of backtracking to previous position(s) */

		/* FIQ says that this code is supposedly bad. While I don't agree with him (it makes the pet follow too
		 * closely for my taste, and if you have more than one pet, it will lead to clogging up corridors, and
		 * those will then be ultra annoying if you're stunned, confused or punished, because you can't displace
		 * them), I can see how if you're using a leash or tin whistle, you want your pets to stay close,
		 * so yeah, those will now turn off the "offending" code. --Amy */

		if (!(mtmp->mleashed || (has_edog && (monstermoves - edog->whistletime < 10)) )) {

			k = (has_edog && !is_spell) ? uncursedcnt : cnt;
			for (j = 0; j < MTSZ && j < k - 1; j++)
				if (nx == mtmp->mtrack[j].x && ny == mtmp->mtrack[j].y)
					if (rn2(MTSZ * (k - j))) goto nxti;
		}

		j = ((ndist = GDIST(nx,ny)) - nidist) * appr;
		if ((j == 0 && !rn2(++chcnt)) || j < 0 ||
			(j > 0 && !whappr &&
				((omx == nix && omy == niy && !rn2(3))
					|| !rn2(12))
			)) {
			nix = nx;
			niy = ny;
			nidist = ndist;
			if(j < 0) chcnt = 0;
			chi = i;
		}
	nxti:	;
	}
newdogpos:
	if (nix != omx || niy != omy) {
		struct obj *mw_tmp;

		if (info[chi] & ALLOW_U) {
			if (mtmp->mleashed) { /* play it safe */
				pline("%s breaks loose of %s leash!",
				      Monnam(mtmp), mhis(mtmp));
				m_unleash(mtmp, FALSE);
			}
			(void) mattacku(mtmp);
			return(0);
		}
		if (!m_in_out_region(mtmp, nix, niy))
		    return 1;
		if (((IS_ROCK(levl[nix][niy].typ) && !(IS_FARMLAND(levl[nix][niy].typ)) && !(IS_GRAVEWALL(levl[nix][niy].typ)) && !(IS_MOUNTAIN(levl[nix][niy].typ)) && may_dig(nix,niy)) ||
		     closed_door(nix, niy)) &&
		    mtmp->weapon_check != NO_WEAPON_WANTED &&
		    tunnels(mtmp->data) && needspick(mtmp->data)) {
		    if (closed_door(nix, niy)) {
			if (!(mw_tmp = MON_WEP(mtmp)) ||
			    !is_pick(mw_tmp) || !is_axe(mw_tmp))
			    mtmp->weapon_check = NEED_PICK_OR_AXE;
		    } else if (IS_TREE(levl[nix][niy].typ)) {
			if (!(mw_tmp = MON_WEP(mtmp)) || !is_axe(mw_tmp))
			    mtmp->weapon_check = NEED_AXE;
		    } else if (!(mw_tmp = MON_WEP(mtmp)) || !is_pick(mw_tmp)) {
			mtmp->weapon_check = NEED_PICK_AXE;
		    }
		    if (mtmp->weapon_check >= NEED_PICK_AXE &&
			mon_wield_item(mtmp))
			return 0;
		}
		/* insert a worm_move() if worms ever begin to eat things */
		remove_monster(omx, omy);
		place_monster(mtmp, nix, niy);

		/* evil patch idea by jonadab: 1% chance for pets to step on cursed items anyway */
		if (has_edog && rn2(100) && !is_spell && cursemsg[chi] && (cansee(omx,omy) || cansee(nix,niy)))
			pline("%s moves only reluctantly.", Monnam(mtmp));
		for (j=MTSZ-1; j>0; j--) mtmp->mtrack[j] = mtmp->mtrack[j-1];
		mtmp->mtrack[0].x = omx;
		mtmp->mtrack[0].y = omy;
		/* We have to know if the pet's gonna do a combined eat and
		 * move before moving it, but it can't eat until after being
		 * moved.  Thus the do_eat flag.
		 */
		if (do_eat) {
		    if (dog_eat(mtmp, obj, omx, omy, FALSE) == 2) return 2;
		}
	} else if (mtmp->mleashed && distu(omx, omy) > 4) {
		/* an incredible kludge, but the only way to keep pooch near
		 * after it spends time eating or in a trap, etc.
		 */
		coord cc;

		nx = sgn(omx - u.ux);
		ny = sgn(omy - u.uy);
		cc.x = u.ux + nx;
		cc.y = u.uy + ny;
		if (goodpos(cc.x, cc.y, mtmp, 0)) goto dognext;

		i  = xytod(nx, ny);
		for (j = (i + 7)%8; j < (i + 1)%8; j++) {
			dtoxy(&cc, j);
			if (goodpos(cc.x, cc.y, mtmp, 0)) goto dognext;
		}
		for (j = (i + 6)%8; j < (i + 2)%8; j++) {
			dtoxy(&cc, j);
			if (goodpos(cc.x, cc.y, mtmp, 0)) goto dognext;
		}
		cc.x = mtmp->mx;
		cc.y = mtmp->my;
dognext:
		if (!m_in_out_region(mtmp, nix, niy))
		  return 1;
		remove_monster(mtmp->mx, mtmp->my);
		place_monster(mtmp, cc.x, cc.y);
		newsym(cc.x,cc.y);
		set_apparxy(mtmp);
	}
	return(1);
}

/* check if a monster could pick up objects from a location */
STATIC_OVL boolean
could_reach_item(mon, nx, ny)
struct monst *mon;
xchar nx, ny;
{
    if ((!is_waterypool(nx,ny) || mon->egotype_watersplasher || is_swimmer(mon->data)) &&
	(!is_lava(nx,ny) || likes_lava(mon->data)) &&
	(!sobj_at(BOULDER,nx,ny) || throws_rocks(mon->data)))
    	return TRUE;
    return FALSE;
}

/* Hack to prevent a dog from being endlessly stuck near an object that
 * it can't reach, such as caught in a teleport scroll niche.  It recursively
 * checks to see if the squares in between are good.  The checking could be a
 * little smarter; a full check would probably be useful in m_move() too.
 * Since the maximum food distance is 5, this should never be more than 5 calls
 * deep.
 */
STATIC_OVL boolean
can_reach_location(mon, mx, my, fx, fy)
struct monst *mon;
xchar mx, my, fx, fy;
{
    int i, j;
    int dist;

    if (mx == fx && my == fy) return TRUE;
    if (!isok(mx, my)) return FALSE; /* should not happen */
    
    dist = dist2(mx, my, fx, fy);
    for(i=mx-1; i<=mx+1; i++) {
	for(j=my-1; j<=my+1; j++) {
	    if (!isok(i, j))
		continue;
	    if (dist2(i, j, fx, fy) >= dist)
		continue;
	    if (IS_ROCK(levl[i][j].typ) && !(IS_FARMLAND(levl[i][j].typ)) && !passes_walls(mon->data) && (!mon->egotype_wallwalk) &&
				    (!may_dig(i,j) || !tunnels(mon->data)))
		continue;
	    if (IS_MOUNTAIN(levl[i][j].typ) && !passes_walls(mon->data) && (!mon->egotype_wallwalk))
		continue;
	    if (IS_DOOR(levl[i][j].typ) &&
				(levl[i][j].doormask & (D_CLOSED | D_LOCKED)))
		continue;
	    if (!could_reach_item(mon, i, j))
		continue;
	    if (can_reach_location(mon, i, j, fx, fy))
		return TRUE;
	}
    }
    return FALSE;
}

#endif /* OVL0 */
#ifdef OVLB

/*ARGSUSED*/	/* do_clear_area client */
STATIC_PTR void
wantdoor(x, y, distance)
int x, y;
void * distance;
{
    int ndist;

    if (*(int*)distance > (ndist = distu(x, y))) {
	gx = x;
	gy = y;
	*(int*)distance = ndist;
    }
}

#endif /* OVLB */

/*dogmove.c*/

/*	SCCS Id: @(#)priest.c	3.4	2002/11/06	*/
/* Copyright (c) Izchak Miller, Steve Linhart, 1989.		  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "mfndpos.h"
#include "eshk.h"
#include "epri.h"
#include "emin.h"

/* this matches the categorizations shown by enlightenment */
#define ALGN_SINNED	(-4)	/* worse than strayed */

#ifdef OVLB

STATIC_DCL boolean histemple_at(struct monst *,XCHAR_P,XCHAR_P);
STATIC_DCL boolean has_shrine(struct monst *);

/*
 * Move for priests and shopkeepers.  Called from shk_move() and pri_move().
 * Valid returns are  1: moved  0: didn't  -1: let m_move do it  -2: died.
 */
int
move_special(mtmp,in_his_shop,appr,uondoor,avoid,omx,omy,gx,gy)
register struct monst *mtmp;
boolean in_his_shop;
schar appr;
boolean uondoor,avoid;
register xchar omx,omy,gx,gy;
{
	register xchar nx,ny,nix,niy;
	register schar i;
	schar chcnt,cnt;
	coord poss[9];
	long info[9];
	long allowflags;
	struct obj *ib = (struct obj *)0;

	if(omx == gx && omy == gy)
		return(0);
	if(mtmp->mconf) {
		avoid = FALSE;
		appr = 0;
	}

	nix = omx;
	niy = omy;
	if (mtmp->isshk) allowflags = ALLOW_SSM;
	else allowflags = ALLOW_SSM | ALLOW_SANCT;
	if (passes_walls(mtmp->data) || (mtmp->egotype_wallwalk) ) allowflags |= (ALLOW_ROCK|ALLOW_WALL);
	if (throws_rocks(mtmp->data)) allowflags |= ALLOW_ROCK;
	if (tunnels(mtmp->data)) allowflags |= ALLOW_DIG;
	if (!nohands(mtmp->data) && !verysmall(mtmp->data)) {
		allowflags |= OPENDOOR;
		if (m_carrying(mtmp, SKELETON_KEY)) allowflags |= BUSTDOOR;
		if (m_carrying(mtmp, CONTROVERSY_CODE)) allowflags |= BUSTDOOR;
		if (m_carrying(mtmp, SECRET_KEY)) allowflags |= BUSTDOOR;
	}
	if (is_giant(mtmp->data)) allowflags |= BUSTDOOR;
	cnt = mfndpos(mtmp, poss, info, allowflags);

	if(mtmp->isshk && avoid && uondoor) { /* perhaps we cannot avoid him */
		for(i=0; i<cnt; i++)
		    if(!(info[i] & NOTONL)) goto pick_move;
		avoid = FALSE;
	}

#define GDIST(x,y)	(dist2(x,y,gx,gy))
pick_move:
	chcnt = 0;
	for(i=0; i<cnt; i++) {
		nx = poss[i].x;
		ny = poss[i].y;
		if(levl[nx][ny].typ == ROOM || (ACCESSIBLE(levl[nx][ny].typ) && levl[nx][ny].typ != DOOR) ||
			(levl[nx][ny].typ >= POOL && levl[nx][ny].typ <= STYXRIVER && is_flyer(mtmp->data)) ||
			(mtmp->ispriest &&
			    levl[nx][ny].typ == ALTAR) ||
			(mtmp->isshk &&
			    (!in_his_shop || ESHK(mtmp)->following))) {
		    if(avoid && (info[i] & NOTONL))
			continue;
		    if((!appr && !rn2(++chcnt)) ||
			(appr && GDIST(nx,ny) < GDIST(nix,niy))) {
			    nix = nx;
			    niy = ny;
		    }
		}
	}
	if(mtmp->ispriest && avoid &&
			nix == omx && niy == omy && onlineu(omx,omy)) {
		/* might as well move closer as long it's going to stay
		 * lined up */
		avoid = FALSE;
		goto pick_move;
	}

	if(nix != omx || niy != omy) {
		remove_monster(omx, omy);
		place_monster(mtmp, nix, niy);
		newsym(nix,niy);
		if (mtmp->isshk && !in_his_shop && inhishop(mtmp))
		    check_special_room(FALSE);
		if(ib) {
			if (cansee(mtmp->mx,mtmp->my))
			    pline("%s picks up %s.", Monnam(mtmp),
				distant_name(ib,doname));
			obj_extract_self(ib);
			(void) mpickobj(mtmp, ib, FALSE);
		}
		return(1);
	}
	return(0);
}

#endif /* OVLB */

#ifdef OVL0

char
temple_occupied(array)
register char *array;
{
	register char *ptr;

	for (ptr = array; *ptr; ptr++)
		if (rooms[*ptr - ROOMOFFSET].rtype == TEMPLE)
			return(*ptr);
	return('\0');
}

#endif /* OVL0 */
#ifdef OVLB

STATIC_OVL boolean
histemple_at(priest, x, y)
register struct monst *priest;
register xchar x, y;
{
	return((boolean)((EPRI(priest)->shroom == *in_rooms(x, y, TEMPLE)) &&
	       on_level(&(EPRI(priest)->shrlevel), &u.uz)));
}

/*
 * pri_move: return 1: moved  0: didn't  -1: let m_move do it  -2: died
 */
int
pri_move(priest)
register struct monst *priest;
{
	register xchar gx,gy,omx,omy;
	schar temple;
	boolean avoid = TRUE;

	omx = priest->mx;
	omy = priest->my;

	if(!histemple_at(priest, omx, omy)) return(-1);

	temple = EPRI(priest)->shroom;

	gx = EPRI(priest)->shrpos.x;
	gy = EPRI(priest)->shrpos.y;

	gx += rn1(3,-1);	/* mill around the altar */
	gy += rn1(3,-1);

	if (evilfriday && monnear(priest, u.ux, u.uy) && (uwep && (uwep->otyp == CIGARETTE || uwep->otyp == ELECTRIC_CIGARETTE || uwep->otyp == CIGAR)) && priest->mpeaceful) {
		priest->mpeaceful = 0;
		verbalize("Begone!  Thou desecratest this holy place with thy presence.");
	}

	if(!priest->mpeaceful ||
	   (Conflict && !resist(priest, RING_CLASS, 0, 0)) ||
	   (StrongConflict && !resist(priest, RING_CLASS, 0, 0))) {
		if(monnear(priest, u.ux, u.uy)) {
			if(Displaced)
				Your("displaced image doesn't fool %s!",
					mon_nam(priest));
			(void) mattacku(priest);
			return(0);
		} else if(index(u.urooms, temple)) {
			/* chase player if inside temple & can see him */
			if(priest->mcansee && m_canseeu(priest)) {
				gx = u.ux;
				gy = u.uy;
			}
			avoid = FALSE;
		}
	} else if(Invis) avoid = FALSE;

	return(move_special(priest,FALSE,TRUE,FALSE,avoid,omx,omy,gx,gy));
}

/* exclusively for mktemple() */
void
priestini(lvl, sroom, sx, sy, sanctum)
d_level	*lvl;
struct mkroom *sroom;
int sx, sy;
boolean sanctum;   /* is it the seat of the high priest? */
{
	struct monst *priest;
	struct obj *otmp = NULL;
	int cnt;

	if(MON_AT(sx+1, sy))
		(void) rloc(m_at(sx+1, sy), FALSE); /* insurance */

	priest = makemon(&mons[(In_yendorian(&u.uz) && depth(&u.uz) == 1) ? PM_DNETHACK_ELDER_PRIEST_TM_ : isevilvariant ? PM_DNETHACK_ELDER_PRIEST_TM_ : sanctum ? PM_HIGH_PRIEST : (level_difficulty() > 39 && Amask2align(levl[sx][sy].altarmask) != A_NONE) ? PM_ELITE_PRIEST : (level_difficulty() > 19 && Amask2align(levl[sx][sy].altarmask) != A_NONE) ? PM_MASTER_PRIEST : PM_ALIGNED_PRIEST],
			 sx + 1, sy, NO_MM_FLAGS);

	if (priest) {
		EPRI(priest)->shroom = (sroom - rooms) + ROOMOFFSET;
		EPRI(priest)->shralign = Amask2align(levl[sx][sy].altarmask);
		EPRI(priest)->shrpos.x = sx;
		EPRI(priest)->shrpos.y = sy;
		assign_level(&(EPRI(priest)->shrlevel), lvl);
		/*priest->mtrapseen = ~0;*/	/* traps are known */
		priest->mpeaceful = 1;
		priest->ispriest = 1;
		priest->msleeping = 0;
		set_malign(priest); /* mpeaceful may have changed */

		/* now his/her goodies... */
		if(sanctum && EPRI(priest)->shralign == A_NONE &&
		     on_level(&sanctum_level, &u.uz)) {
			(void) mongets(priest, AMULET_OF_YENDOR);
		}
		/* 2 to 4 spellbooks - but less later on
		 * Amy edit: 50% chance of having just one, har-har :P
		 * they're just free loot anyway, the priest doesn't benefit from having them! */
		cnt = (rn2(2) ? 1 : rn1(3,2));
		for (cnt; cnt > 0; --cnt) {
		    if (timebasedlowerchance()) (void) mpickobj(priest, mkobj(SPBOOK_CLASS, FALSE, FALSE), TRUE);
		}
		/* [ALI] Upgrade existing robe or aquire new */

		if (In_yendorian(&u.uz) && depth(&u.uz) == 1) {
			struct obj *obj;
			obj = mksobj(ROBE, TRUE, FALSE, FALSE);
			if (obj) {
				obj = oname(obj, artiname(ART_MOTHERFUCKER_TROPHY));
				if (obj) {
					curse(obj);
					(void) mpickobj(priest, obj, TRUE);
				}
				m_dowear(priest, TRUE);
			}

		} else if (rn2(2) || (otmp = which_armor(priest, W_ARM)) == 0) {
		    struct obj *obj;
		    obj = mksobj(rn2(p_coaligned(priest) ? 2 : 5) ?
			    ROBE_OF_PROTECTION : ROBE_OF_POWER, TRUE, FALSE, FALSE);
		    if (obj) { /* this should fix a STUPID segfault for minimalist characters --Amy */
			    if (p_coaligned(priest))
				uncurse(obj, TRUE);
			    else
				curse(obj);
			    if (obj) {
				(void) mpickobj(priest, obj, TRUE);
			    }
			    m_dowear(priest, TRUE);
		/* somehow the bug was still happening... this gold piece check seems to really fix it --Amy */
			    if (obj && !(obj->otyp == GOLD_PIECE) && !(obj->owornmask & W_ARM)) {
				obj_extract_self(obj);
				obfree(obj, (struct obj *)0);
			    } else if (otmp && !(otmp->otyp == GOLD_PIECE)) {
				obj_extract_self(otmp);
				obfree(otmp, (struct obj *)0);
			    }
		    }
		}
	}
}

/*
 * Specially aligned monsters are named specially.
 *	- aligned priests with ispriest and high priests have shrines
 *		they retain ispriest and epri when polymorphed
 *	- aligned priests without ispriest and Angels are roamers
 *		they retain isminion and access epri as emin when polymorphed
 *		(coaligned Angels are also created as minions, but they
 *		use the same naming convention)
 *	- minions do not have ispriest but have isminion and emin
 *	- caller needs to inhibit Hallucination if it wants to force
 *		the true name even when under that influence
 */
char *
priestname(mon, pname)
register struct monst *mon;
char *pname;		/* caller-supplied output buffer */
{
	const char *what = Hallucination ? rndmonnam() : (u.usanity > 900 && (u.usanity > rn2(10000)) && rn2(10) ) ? rndmonnam() : mon->data->mname;

	strcpy(pname, "the ");
	if (mon->minvis) strcat(pname, "invisible ");
	if (mon->ispriest || mon->data == &mons[PM_ALIGNED_PRIEST] || mon->data == &mons[PM_MASTER_PRIEST] || mon->data == &mons[PM_ELITE_PRIEST] ||
					mon->data == &mons[PM_ANGEL]) {
		/* use epri */
		if (mon->mtame && mon->data == &mons[PM_ANGEL])
			strcat(pname, "guardian ");
		if (mon->data != &mons[PM_ALIGNED_PRIEST] && mon->data != &mons[PM_DNETHACK_ELDER_PRIEST_TM_] &&
				mon->data != &mons[PM_HIGH_PRIEST]) {
			strcat(pname, what);
			strcat(pname, " ");
		}
		if (mon->data != &mons[PM_ANGEL]) {
			if (!mon->ispriest && EPRI(mon)->renegade)
				strcat(pname, "renegade ");
			if (mon->data == &mons[PM_HIGH_PRIEST])
				strcat(pname, "high ");
			if (mon->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_])
				strcat(pname, "elder ");
			if (FunnyHallu)
				strcat(pname, "poohbah ");
			else if (mon->female)
				strcat(pname, "priestess ");
			else
				strcat(pname, "priest ");
		}
		strcat(pname, "of ");
		strcat(pname, halu_gname((int)EPRI(mon)->shralign));
		return(pname);
	}
	/* use emin instead of epri */
	strcat(pname, what);
	strcat(pname, " of ");
	strcat(pname, halu_gname(EMIN(mon)->min_align));
	return(pname);
}

boolean
p_coaligned(priest)
struct monst *priest;
{
	return((boolean)(u.ualign.type == ((int)EPRI(priest)->shralign)));
}

STATIC_OVL boolean
has_shrine(pri)
struct monst *pri;
{
	struct rm *lev;

	if(!pri)
		return(FALSE);
	lev = &levl[EPRI(pri)->shrpos.x][EPRI(pri)->shrpos.y];
	if (!IS_ALTAR(lev->typ) || !(lev->altarmask & AM_SHRINE))
		return(FALSE);
	return((boolean)(EPRI(pri)->shralign == Amask2align(lev->altarmask & ~AM_SHRINE)));
}

struct monst *
findpriest(roomno)
char roomno;
{
	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if(mtmp->ispriest && (EPRI(mtmp)->shroom == roomno) &&
	       histemple_at(mtmp,mtmp->mx,mtmp->my))
		return(mtmp);
	}
	return (struct monst *)0;
}

/* called from check_special_room() when the player enters the temple room */
void
intemple(roomno)
register int roomno;
{
	register struct monst *priest = findpriest((char)roomno);
	boolean tended = (priest != (struct monst *)0);
	boolean shrined, sanctum, can_speak;
	const char *msg1, *msg2;
	char buf[BUFSZ];

	if(!temple_occupied(u.urooms0)) {
	    if(tended) {
		shrined = has_shrine(priest);
		sanctum = ( (priest->data == &mons[PM_HIGH_PRIEST] || priest->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) &&
			   (Is_sanctum(&u.uz) || In_endgame(&u.uz)));
		can_speak = (priest->mcanmove && !priest->msleeping &&
			     flags.soundok);
		if (can_speak) {
		    unsigned save_priest = priest->ispriest;
		    /* don't reveal the altar's owner upon temple entry in
		       the endgame; for the Sanctum, the next message names
		       Moloch so suppress the "of Moloch" for him here too */
		    if (sanctum && !Hallucination) priest->ispriest = 0;
		    pline("%s intones:",
			canseemon(priest) ? Monnam(priest) : "A nearby voice");
		    priest->ispriest = save_priest;
		}
		msg2 = 0;
		if(sanctum && Is_sanctum(&u.uz)) {
		    if(priest->mpeaceful) {	/* never the case in this game --Amy */
			msg1 = "Infidel, you have entered Moloch's Sanctum!";
			if (Role_if(PM_GANG_SCHOLAR)) msg1 = "Infidel, you have entered Anna's Sanctum!";
			if (Role_if(PM_WALSCHOLAR)) msg1 = "Infidel, you have entered Anna's Sanctum!";
			msg2 = "Be gone!";
			priest->mpeaceful = 0;
			set_malign(priest);
		    } else {
			if (rn2(2)) msg1 = "You desecrate this place by your presence!";
			else {
			msg1 = "Infidel, you have entered Moloch's Sanctum!";
			if (Role_if(PM_GANG_SCHOLAR)) msg1 = "Infidel, you have entered Anna's Sanctum!";
			if (Role_if(PM_WALSCHOLAR)) msg1 = "Infidel, you have entered Anna's Sanctum!";
			msg2 = "Be gone!";
			}
			}

		} else {
		    sprintf(buf, "Pilgrim, you enter a %s place!",
			    !shrined ? "desecrated" : "sacred");
		    msg1 = buf;
		}
		if (can_speak) {
		    verbalize("%s", msg1);
		    if (msg2) verbalize("%s", msg2);
		}
		if(!sanctum) {
		    /* !tended -> !shrined */
		    if (!shrined || !p_coaligned(priest) ||
			    u.ualign.record <= ALGN_SINNED)
			You("have a%s forbidding feeling...",
				(!shrined) ? "" : " strange");
		    else You("experience a strange sense of peace.");
		}

		if (can_speak && evilfriday) verbalize("Please be aware that smoking is strictly prohibited in this temple.");
	    } else {
		switch(rn2(3)) {
		  case 0: You("have an eerie feeling..."); break;
		  case 1: You_feel("like you are being watched."); break;
		  default: pline("A shiver runs down your %s.",
			body_part(SPINE)); break;
		}
		if(!rn2(5)) {
		    struct monst *mtmp;

		    if(!(mtmp = makemon(&mons[PM_GHOST],u.ux,u.uy,NO_MM_FLAGS)))
			return;
		    if (!Blind || sensemon(mtmp))
			pline("An enormous ghost appears next to you!");
		    else You("sense a presence close by!");
		    mtmp->mpeaceful = 0;
		    set_malign(mtmp);
		    if(flags.verbose)
			You("are frightened to death, and unable to move.");
		    nomul(-3, "frightened to death", TRUE);
			make_feared(HFeared + rnd(30 + (monster_difficulty() * 3) ),TRUE);
		    nomovemsg = "You regain your composure.";
	       }
	   }
       }
}

void
priest_talk(priest)
register struct monst *priest;
{
	boolean coaligned = p_coaligned(priest);
	boolean strayed = (u.ualign.record < 0);

	if (Race_if(PM_MACTHEIST)) {
		pline("Did you forget by any chance that you're an atheist? You cannot talk to priests!");
		return;
	}

	/* KMH, conduct */
	u.uconduct.gnostic++;
	if (Race_if(PM_MAGYAR)) {
		You_feel("bad about breaking the atheist conduct.");
		badeffect();
	}

	if(priest->mflee || (!priest->ispriest && coaligned && strayed)) {
	    pline("%s doesn't want anything to do with you!",
				Monnam(priest));
	    priest->mpeaceful = 0;
	    return;
	}

	/* priests don't chat unless peaceful and in their own temple */
	if(!histemple_at(priest,priest->mx,priest->my) ||
		 !priest->mpeaceful || !priest->mcanmove || priest->msleeping) {
	    static const char *cranky_msg[3] = {
		"Thou wouldst have words, eh?  I'll give thee a word or two!",
		"Talk?  Here is what I have to say!",
		"Pilgrim, I would speak no longer with thee."
	    };

	    if(!priest->mcanmove || priest->msleeping) {
		pline("%s breaks out of %s reverie!",
		      Monnam(priest), mhis(priest));
		priest->mfrozen = priest->msleeping = 0;
		priest->masleep = 0;
		priest->mcanmove = 1;
	    }
	    priest->mpeaceful = 0;
	    verbalize("%s", cranky_msg[rn2(3)]);
	    return;
	}

	/* you desecrated the temple and now you want to chat? */
	if(priest->mpeaceful && *in_rooms(priest->mx, priest->my, TEMPLE) &&
		  !has_shrine(priest)) {
	    verbalize("Begone!  Thou desecratest this holy place with thy presence.");
	    priest->mpeaceful = 0;
	    return;
	}
#ifndef GOLDOBJ
	if(!u.ugold) {
	    if(coaligned && !strayed) {
		if (priest->mgold > 0L) {
		    /* Note: two bits is actually 25 cents.  Hmm. */
		    pline("%s gives you %s for an ale.", Monnam(priest),
			FunnyHallu ? ((priest->mgold == 1L) ? "one bitcoin" : "two bitcoins") :
			(priest->mgold == 1L) ? "one bit" : "two bits");
		    if (priest->mgold > 1L)
			u.ugold = 2L;
		    else
			u.ugold = 1L;
		    priest->mgold -= u.ugold;
		    flags.botl = 1;
#else
	if(!money_cnt(invent)) {
	    if(coaligned && !strayed) {
                long pmoney = money_cnt(priest->minvent);
		if (pmoney > 0L) {
		    /* Note: two bits is actually 25 cents.  Hmm. */
		    pline("%s gives you %s for an ale.", Monnam(priest),
			(pmoney == 1L) ? "one bit" : "two bits");
		     money2u(priest, pmoney > 1L ? 2 : 1);
#endif
		} else
		    pline("%s preaches the virtues of poverty.", Monnam(priest));
		exercise(A_WIS, TRUE);
	    } else
		pline("%s is not interested.", Monnam(priest));
	    return;
	} else {
	    long offer;

	/* "Revert temple/priest prices to vanilla values." In Soviet Russia, the protection racket is a legal thing to do, just like ransom extortion, russian roulette and jaywalking. And so, tons of promising level 1 characters will die trying to get to the Minetown priest again. All the other races got enough of that bullshit mechanism and will enjoy fixed prices instead, which makes much more sense and gives players an incentive to actually level up. --Amy */

	    pline("%s asks you for a contribution for the temple.",
			Monnam(priest));
	    if((offer = bribe(priest)) == 0) {
		verbalize("Thou shalt regret thine action!");
		if(coaligned) adjalign(-1);
	    } else if(offer < (issoviet ? (u.ulevel * 200) : 2000) ) {
#ifndef GOLDOBJ
		if(u.ugold > (offer * 2L)) verbalize(issoviet ? "lol u sposobstvovali slishkom malo govorit sovetskiy mat' iz ada." : "Cheapskate.");
#else
		if(money_cnt(invent) > (offer * 2L)) verbalize(issoviet ? "lol u sposobstvovali slishkom malo govorit sovetskiy mat' iz ada." : "Cheapskate.");
#endif
		else {
		    verbalize("I thank thee for thy contribution.");
		    /*  give player some token  */
		    exercise(A_WIS, TRUE);
		}
	    } else if(offer < (issoviet ? (u.ulevel * 400) : 4000) ) {
		verbalize("Thou art indeed a pious individual.");
#ifndef GOLDOBJ
		if(u.ugold < (offer * 2L)) {
#else
		if(money_cnt(invent) < (offer * 2L)) {
#endif
		    if (coaligned && u.ualign.record <= ALGN_SINNED)
			adjalign(1);
		    verbalize("I bestow upon thee a blessing.");
		    /* KMH, intrinsic patch */
		    incr_itimeout(&HClairvoyant, rn1(500,500));
		}
		/* Amy edit: it's way too easy to get lots of protection. Fixing that. */
	    } else if(offer < (issoviet ? (u.ulevel * 600) : 6000) &&
		      u.ublessed < 20 &&
		      (u.ublessed < 1 || !rn2(u.ublessed))) {
		verbalize("Thy devotion has been rewarded.");
		if (Role_if(PM_PRIEST) || Role_if(PM_NECROMANCER) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA)) {
			use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? 3 : 1);
		}
		if (!(HProtection & INTRINSIC))  {
			HProtection |= FROMOUTSIDE;
			if (!u.ublessed)  u.ublessed = rno(Race_if(PM_MAYMES) ? 8 : 4);
		} else {
			u.ublessed++;
			if (Race_if(PM_MAYMES) && u.ublessed < 20) u.ublessed++;
		}
	    } else {
		verbalize("Thy selfless generosity is deeply appreciated.");
		if (Role_if(PM_PRIEST) || Role_if(PM_NECROMANCER) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA)) {
			use_skill(P_SPIRITUALITY, Role_if(PM_PRIEST) ? 3 : 1);
		}
#ifndef GOLDOBJ
		if(u.ugold < (offer * 2L) && coaligned) {
#else
		if(money_cnt(invent) < (offer * 2L) && coaligned) {
#endif
			/* don't immediately lose all negative alignment --Amy */
		    if(strayed && (moves - u.ucleansed) > 5000L) {
			u.ualign.record /= 2; /* cleanse thee */
			adjalign(2);
			u.ucleansed = moves;
			verbalize("Thou hast been cleansed.");
		    } else {
			adjalign(2);
		    }
		}
	    }
	}
}

struct monst *
mk_roamer(ptr, alignment, x, y, peaceful)
register struct permonst *ptr;
aligntyp alignment;
xchar x, y;
boolean peaceful;
{
	register struct monst *roamer;
	register boolean coaligned = (u.ualign.type == alignment);

	if (ptr != &mons[PM_ALIGNED_PRIEST] && ptr != &mons[PM_MASTER_PRIEST] && ptr != &mons[PM_ELITE_PRIEST] && ptr != &mons[PM_ANGEL])
		return((struct monst *)0);
	
	if (MON_AT(x, y)) (void) rloc(m_at(x, y), FALSE);	/* insurance */

	if (!(roamer = makemon(ptr, x, y, NO_MM_FLAGS)))
		return((struct monst *)0);

	EPRI(roamer)->shralign = alignment;
	if (coaligned && !peaceful)
		EPRI(roamer)->renegade = TRUE;
	/* roamer->ispriest == FALSE naturally */
	roamer->isminion = TRUE;	/* borrowing this bit */
	/*roamer->mtrapseen = ~0;*/		/* traps are known */
	roamer->mpeaceful = peaceful;
	roamer->msleeping = 0;
	set_malign(roamer); /* peaceful may have changed */

	/* MORE TO COME */
	return(roamer);
}

void
reset_hostility(roamer)
register struct monst *roamer;
{
	if(!(roamer->isminion && (roamer->data == &mons[PM_ALIGNED_PRIEST] || roamer->data == &mons[PM_MASTER_PRIEST] || roamer->data == &mons[PM_ELITE_PRIEST] ||
				  roamer->data == &mons[PM_ANGEL])))
	        return;

	if(EPRI(roamer)->shralign != u.ualign.type) {
	    roamer->mpeaceful = roamer->mtame = 0;
	    set_malign(roamer);
	}
	newsym(roamer->mx, roamer->my);
}

boolean
in_your_sanctuary(mon, x, y)
struct monst *mon;	/* if non-null, <mx,my> overrides <x,y> */
xchar x, y;
{
	register char roomno;
	register struct monst *priest;

	if (mon) {
	    if (is_minion(mon->data) || is_rider(mon->data) || is_deadlysin(mon->data)) return FALSE;
	    x = mon->mx, y = mon->my;
	}
	if (u.ualign.record <= ALGN_SINNED)	/* sinned or worse */
	    return FALSE;
	if ((roomno = temple_occupied(u.urooms)) == 0 ||
		roomno != *in_rooms(x, y, TEMPLE))
	    return FALSE;
	if ((priest = findpriest(roomno)) == 0)
	    return FALSE;
	return (boolean)(has_shrine(priest) &&
			 p_coaligned(priest) &&
			 priest->mpeaceful);
}

void
ghod_hitsu(priest)	/* when attacking "priest" in his temple */
struct monst *priest;
{
	int x, y, ax, ay, roomno = (int)temple_occupied(u.urooms);
	int x1, y1, x2, y2, n;
	coord poss[4];
	int stpx = sgn(u.ux - priest->mx), stpy = sgn(u.uy - priest->my);
		/* gods avoid hitting the temple priest */
	struct mkroom *troom;

	if (!roomno || !has_shrine(priest))
		return;

	ax = x = EPRI(priest)->shrpos.x;
	ay = y = EPRI(priest)->shrpos.y;
	troom = &rooms[roomno - ROOMOFFSET];

	/*
	 * Determine the source of the lightning bolt according to the
	 * following rules:
	 *	1. The source cannot be directly under the player
	 *	2. Don't zap through the temple priest
	 *	3. First choice of source is the altar itself
	 *	4. Otherwise use a wall, prefering orthogonal to diagonal paths
	 *	5. Choose randomly from equally preferred sources
	 * Note that if the hero is not standing on either the altar or
	 * a door then (u.ux, u.uy) may be counted as a possible source which
	 * is later rejected by linedup() letting the hero off the hook.
	 */
	if((u.ux == x && u.uy == y) || !linedup(u.ux, u.uy, x, y, FALSE) ||
		stpx == sgn(tbx) && stpy == sgn(tby)) {
	    if(IS_DOOR(levl[u.ux][u.uy].typ)) {

		if(u.ux == troom->lx - 1) {
		    if (stpx != sgn(u.ux - troom->hx) || stpy != 0) {
			x = troom->hx;
			y = u.uy;
		    } else {
			/* Diagonal required */
			x1 = u.ux + u.uy - troom->ly;
			y1 = troom->ly;
			x2 = u.ux + troom->hy - u.uy;
			y2 = troom->hy;
			if (x1 > troom->hx && x2 > troom->hx)
			    return;
			else if (x2 > troom->hx || x1 <= troom->hx && !rn2(2)) {
			    x = x1;
			    y = y1;
			} else {
			    x = x2;
			    y = y2;
			}
		    }
		} else if(u.ux == troom->hx + 1) {
		    if (stpx != sgn(u.ux - troom->lx) || stpy != 0) {
			x = troom->lx;
			y = u.uy;
		    } else {
			/* Diagonal required */
			x1 = u.ux - (u.uy - troom->ly);
			y1 = troom->ly;
			x2 = u.ux - (troom->hy - u.uy);
			y2 = troom->hy;
			if (x1 < troom->lx && x2 < troom->lx)
			    return;
			else if (x2 < troom->lx || x1 >= troom->lx && !rn2(2)) {
			    x = x1;
			    y = y1;
			} else {
			    x = x2;
			    y = y2;
			}
		    }
		} else if(u.uy == troom->ly - 1) {
		    if (stpx != 0 || stpy != sgn(u.uy - troom->hy)) {
			x = u.ux;
			y = troom->hy;
		    } else {
			/* Diagonal required */
			x1 = troom->lx;
			y1 = u.uy + u.ux - troom->lx;
			x2 = troom->hx;
			y2 = u.uy + troom->hx - u.ux;
			if (y1 > troom->hy && y2 > troom->hy)
			    return;
			else if (y2 > troom->hy || y1 <= troom->hy && !rn2(2)) {
			    x = x1;
			    y = y1;
			} else {
			    x = x2;
			    y = y2;
			}
		    }
		} else if(u.uy == troom->hy + 1) {
		    if (stpx != 0 || stpy != sgn(u.uy - troom->ly)) {
			x = u.ux;
			y = troom->ly;
		    } else {
			/* Diagonal required */
			x1 = troom->lx;
			y1 = u.uy - (u.ux - troom->lx);
			x2 = troom->hx;
			y2 = u.uy - (troom->hx - u.ux);
			if (y1 < troom->ly && y2 < troom->ly)
			    return;
			else if (y2 < troom->ly || y1 >= troom->ly && !rn2(2)) {
			    x = x1;
			    y = y1;
			} else {
			    x = x2;
			    y = y2;
			}
		    }
		}
	    } else {
		/* Calculate the possible orthogonal paths */
		n = 0;
		if (stpx != 0 || stpy != sgn(u.uy - troom->ly)) {
		    poss[n].x = u.ux;
		    poss[n++].y = troom->ly;
		}
		if (stpx != 0 || stpy != sgn(u.uy - troom->hy)) {
		    poss[n].x = u.ux;
		    poss[n++].y = troom->hy;
		}
		if (stpx != sgn(u.ux - troom->lx) || stpy != 0) {
		    poss[n].x = troom->lx;
		    poss[n++].y = u.uy;
		}
		if (stpx != sgn(u.ux - troom->hx) || stpy != 0) {
		    poss[n].x = troom->hx;
		    poss[n++].y = u.uy;
		}
		if (n) {
		    n = rn2(n);
		    x = poss[n].x;
		    y = poss[n].y;
		} else {
		    impossible("Omnipresent priest?");
		    return;
		}
	    }
	    if(!linedup(u.ux, u.uy, x, y, FALSE))
		return;
	}

	switch(rn2(3)) {
	case 0:
	    pline("%s roars in anger:  \"Thou shalt suffer!\"",
			a_gname_at(ax, ay));
	    break;
	case 1:
	    pline("%s voice booms:  \"How darest thou harm my servant!\"",
			s_suffix(a_gname_at(ax, ay)));
	    break;
	default:
	    pline("%s roars:  \"Thou dost profane my shrine!\"",
			a_gname_at(ax, ay));
	    break;
	}

	buzz(-10-(AD_ELEC-1), 6, x, y, sgn(tbx), sgn(tby)); /* bolt of lightning */
	exercise(A_WIS, FALSE);
}

void
angry_priest()
{
	register struct monst *priest;
	struct rm *lev;

	if ((priest = findpriest(temple_occupied(u.urooms))) != 0) {
	    wakeup(priest);
	    /*
	     * If the altar has been destroyed or converted, let the
	     * priest run loose.
	     * (When it's just a conversion and there happens to be
	     *	a fresh corpse nearby, the priest ought to have an
	     *	opportunity to try converting it back; maybe someday...)
	     */
	    lev = &levl[EPRI(priest)->shrpos.x][EPRI(priest)->shrpos.y];
	    if (!IS_ALTAR(lev->typ) ||
		((aligntyp)Amask2align(lev->altarmask & AM_MASK) !=
			EPRI(priest)->shralign)) {
		priest->ispriest = 0;		/* now a roamer */
		priest->isminion = 1;		/* but still aligned */
		/* this overloads the `shroom' field, which is now clobbered */
		EPRI(priest)->renegade = 0;
	    }
	}
}

/*
 * When saving bones, find priests that aren't on their shrine level,
 * and remove them.   This avoids big problems when restoring bones.
 */
void
clearpriests()
{
    register struct monst *mtmp, *mtmp2;

    for(mtmp = fmon; mtmp; mtmp = mtmp2) {
	mtmp2 = mtmp->nmon;
	if (!DEADMONSTER(mtmp) && mtmp->ispriest && !on_level(&(EPRI(mtmp)->shrlevel), &u.uz))
	    mongone(mtmp);
    }
}

/* munge priest-specific structure when restoring -dlc */
void
restpriest(mtmp, ghostly)
register struct monst *mtmp;
boolean ghostly;
{
    if(u.uz.dlevel) {
	if (ghostly)
	    assign_level(&(EPRI(mtmp)->shrlevel), &u.uz);
    }
}

#endif /* OVLB */

/*priest.c*/

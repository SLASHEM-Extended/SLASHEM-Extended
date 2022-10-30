/*	SCCS Id: @(#)shk.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* Note by Amy: All "shkp->mgold + (value)" routines have a chance to fail to make credit cloning harder. */
/* The same applies to all actions that give credit (see also dokick.c). */

#include "hack.h"
#include "eshk.h"

/*#define DEBUG*/

#define PAY_SOME    2
#define PAY_BUY     1
#define PAY_CANT    0	/* too poor */
#define PAY_SKIP  (-1)
#define PAY_BROKE (-2)

STATIC_DCL void makekops(coord *);
STATIC_DCL void call_kops(struct monst *,BOOLEAN_P);
# ifdef OVLB
STATIC_DCL void kops_gone(BOOLEAN_P);
# endif /* OVLB */

#define IS_SHOP(x)	(rooms[x].rtype >= SHOPBASE)
#define no_cheat      ((ACURR(A_CHA) - rnl(3)) > 7)

extern const struct shclass shtypes[];	/* defined in shknam.c */
extern struct obj *thrownobj;		/* defined in dothrow.c */

STATIC_VAR NEARDATA long int followmsg;	/* last time of follow message */

STATIC_DCL void setpaid(struct monst *);
STATIC_DCL long addupbill(struct monst *);
STATIC_DCL void pacify_shk(struct monst *);
STATIC_DCL struct bill_x *onbill(struct obj *, struct monst *, BOOLEAN_P);
STATIC_DCL struct monst *next_shkp(struct monst *, BOOLEAN_P);
STATIC_DCL long shop_debt(struct eshk *);
STATIC_DCL char *shk_owns(char *,struct obj *);
STATIC_DCL char *mon_owns(char *,struct obj *);
STATIC_DCL void clear_unpaid(struct obj *);
STATIC_DCL long check_credit(long, struct monst *);
STATIC_DCL void pay(long, struct monst *);
STATIC_DCL long get_cost(struct obj *, struct monst *);
STATIC_DCL long set_cost(struct obj *, struct monst *);
STATIC_DCL const char *shk_embellish(struct obj *, long);
STATIC_DCL long cost_per_charge(struct monst *,struct obj *,BOOLEAN_P);
STATIC_DCL long cheapest_item(struct monst *);
STATIC_DCL int dopayobj(struct monst *, struct bill_x *, struct obj **, int, BOOLEAN_P);
STATIC_DCL long stolen_container(struct obj *, struct monst *, long, BOOLEAN_P, BOOLEAN_P);
STATIC_DCL long getprice(struct obj *,BOOLEAN_P);
STATIC_DCL void shk_names_obj(struct monst *,struct obj *,const char *,long,const char *);
STATIC_DCL struct obj *bp_to_obj(struct bill_x *);
STATIC_DCL boolean inherits(struct monst *,int,int);
STATIC_DCL void set_repo_loc(struct eshk *);
STATIC_DCL boolean angry_shk_exists(void);
STATIC_DCL void rile_shk(struct monst *);
STATIC_DCL void rouse_shk(struct monst *,BOOLEAN_P);
STATIC_DCL void remove_damage(struct monst *, BOOLEAN_P);
STATIC_DCL void sub_one_frombill(struct obj *, struct monst *);
STATIC_DCL void add_one_tobill(struct obj *, BOOLEAN_P);
STATIC_DCL void dropped_container(struct obj *, struct monst *, BOOLEAN_P);
STATIC_DCL void add_to_billobjs(struct obj *);
STATIC_DCL void bill_box_content(struct obj *, BOOLEAN_P, BOOLEAN_P, struct monst *);
#ifdef OVL1
static boolean rob_shop(struct monst *);
#endif

#define NOBOUND         (-1)    /* No lower/upper limit to charge       */
static void shk_other_services(void);
static void shk_identify(char *, struct monst *);
static void shk_uncurse(char *, struct monst *);
static void shk_bless(char *, struct monst *);
static void shk_appraisal(char *, struct monst *);
static void shk_weapon_works(char *, struct monst *);
static void shk_armor_works(char *, struct monst *);
static void shk_charge(char *, struct monst *);
static void shk_estcredit(char *, struct monst *);
static boolean shk_obj_match(struct obj *, struct monst *);
/*static int shk_class_match(long class, struct monst *shkp);*/
static boolean shk_offer_price(char *, long, struct monst *);
static void shk_smooth_charge(int *, int, int);

#ifdef OVLB
/*
	invariants: obj->unpaid iff onbill(obj) [unless bp->useup]
		obj->quan <= bp->bquan
 */


#ifdef GOLDOBJ
/*
    Transfer money from inventory to monster when paying
    shopkeepers, priests, oracle, succubus, & other demons.
    Simple with only gold coins.
    This routine will handle money changing when multiple
    coin types is implemented, only appropriate
    monsters will pay change.  (Peaceful shopkeepers, priests
    & the oracle try to maintain goodwill while selling
    their wares or services.  Angry monsters and all demons
    will keep anything they get their hands on.
    Returns the amount actually paid, so we can know
    if the monster kept the change.
 */
long
money2mon(mon, amount)
struct monst *mon;
long amount;
{
    struct obj *ygold = findgold(invent);

    if (amount <= 0) {
	impossible("%s payment in money2mon!", amount ? "negative" : "zero");
	return 0L;
    }
    if (!ygold || ygold->quan < amount) {
	impossible("Paying without %s money?", ygold ? "enough" : "");
	return 0L;
    }

    if (ygold->quan > amount)
	ygold = splitobj(ygold, amount);
    else if (ygold->owornmask)
	remove_worn_item(ygold, FALSE);		/* quiver */
    freeinv(ygold);
    add_to_minv(mon, ygold);
    flags.botl = 1;
    return amount;
}


/*
    Transfer money from monster to inventory.
    Used when the shopkeeper pay for items, and when
    the priest gives you money for an ale.
 */
void
money2u(mon, amount)
struct monst *mon;
long amount;
{
    struct obj *mongold = findgold(mon->minvent);

    if (amount <= 0) {
	impossible("%s payment in money2u!", amount ? "negative" : "zero");
	return;
    }
    if (!mongold || mongold->quan < amount) {
	impossible("%s paying without %s money?", a_monnam(mon),
		   mongold ? "enough" : "");
	return;
    }

    if (mongold->quan > amount) mongold = splitobj(mongold, amount);
    obj_extract_self(mongold);

    if (!merge_choice(invent, mongold) && inv_cnt() >= 52) {
	You("have no room for the money!");
	dropy(mongold);
    } else {
	addinv(mongold);
	flags.botl = 1;
    }
}

#endif /* GOLDOBJ */

STATIC_OVL struct monst *
next_shkp(shkp, withbill)
register struct monst *shkp;
register boolean withbill;
{
	for (; shkp; shkp = shkp->nmon) {
	    if (DEADMONSTER(shkp)) continue;
	    if (shkp->isshk && (ESHK(shkp)->billct || !withbill)) break;
	}

	if (shkp) {
	    if (NOTANGRY(shkp)) {
		if (ESHK(shkp)->surcharge) pacify_shk(shkp);
	    } else {
		if (!ESHK(shkp)->surcharge) rile_shk(shkp);
	    }
	}
	return(shkp);
}

char *
shkname(mtmp)				/* called in do_name.c */
register struct monst *mtmp;
{
	return(ESHK(mtmp)->shknam);
}

void
shkgone(mtmp)				/* called in mon.c */
struct monst *mtmp;
{
	struct eshk *eshk = ESHK(mtmp);
	struct mkroom *sroom = &rooms[eshk->shoproom - ROOMOFFSET];
	struct obj *otmp;
	char *p;
	int sx, sy;

	/* [BUG: some of this should be done on the shop level */
	/*       even when the shk dies on a different level.] */
	if (on_level(&eshk->shoplevel, &u.uz)) {
	    remove_damage(mtmp, TRUE);
	    sroom->resident = (struct monst *)0;
	    if (!search_special(ANY_SHOP))
		level.flags.has_shop = 0;

	    /* items on shop floor revert to ordinary objects */
	    for (sx = sroom->lx; sx <= sroom->hx; sx++)
	      for (sy = sroom->ly; sy <= sroom->hy; sy++)
		for (otmp = level.objects[sx][sy]; otmp; otmp = otmp->nexthere)
		    otmp->no_charge = 0;

	    /* Make sure bill is set only when the
	       dead shk is the resident shk. */
	    if ((p = index(u.ushops, eshk->shoproom)) != 0) {
		setpaid(mtmp);
		eshk->bill_p = (struct bill_x *)0;
		/* remove eshk->shoproom from u.ushops */
		do { *p = *(p + 1); } while (*++p);
	    }
	}
}

void
set_residency(shkp, zero_out)
register struct monst *shkp;
register boolean zero_out;
{
	if (on_level(&(ESHK(shkp)->shoplevel), &u.uz))
	    rooms[ESHK(shkp)->shoproom - ROOMOFFSET].resident =
		(zero_out)? (struct monst *)0 : shkp;
}

void
replshk(mtmp,mtmp2)
register struct monst *mtmp, *mtmp2;
{
	rooms[ESHK(mtmp2)->shoproom - ROOMOFFSET].resident = mtmp2;
	if (inhishop(mtmp) && *u.ushops == ESHK(mtmp)->shoproom) {
		ESHK(mtmp2)->bill_p = &(ESHK(mtmp2)->bill[0]);
	}
}

/* do shopkeeper specific structure munging -dlc */
void
restshk(shkp, ghostly)
struct monst *shkp;
boolean ghostly;
{
    if (u.uz.dlevel) {
	struct eshk *eshkp = ESHK(shkp);

	if (eshkp->bill_p != (struct bill_x *) -1000)
	    eshkp->bill_p = &eshkp->bill[0];
	/* shoplevel can change as dungeons move around */
	/* savebones guarantees that non-homed shk's will be gone */
	if (ghostly) {
	    assign_level(&eshkp->shoplevel, &u.uz);
	    if (ANGRY(shkp) && strncmpi(eshkp->customer, plname, PL_NSIZ))
		pacify_shk(shkp);
	}
    }
}

#endif /* OVLB */
#ifdef OVL3

/* Clear the unpaid bit on all of the objects in the list. */
STATIC_OVL void
clear_unpaid(list)
register struct obj *list;
{
    while (list) {
	if (Has_contents(list)) clear_unpaid(list->cobj);
	list->unpaid = 0;
	list = list->nobj;
    }
}
#endif /*OVL3*/
#ifdef OVLB

/* either you paid or left the shop or the shopkeeper died */
STATIC_OVL void
setpaid(shkp)
register struct monst *shkp;
{
	register struct obj *obj;
	register struct monst *mtmp;

	/* FIXME: object handling should be limited to
	   items which are on this particular shk's bill */

	clear_unpaid(invent);
	clear_unpaid(fobj);
	clear_unpaid(level.buriedobjlist);
	if (thrownobj) thrownobj->unpaid = 0;
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		clear_unpaid(mtmp->minvent);
	for(mtmp = migrating_mons; mtmp; mtmp = mtmp->nmon)
		clear_unpaid(mtmp->minvent);

	while ((obj = billobjs) != 0) {
		obj_extract_self(obj);
		dealloc_obj(obj);
	}
	if(shkp) {
		ESHK(shkp)->billct = 0;
		ESHK(shkp)->credit = 0L;
		ESHK(shkp)->debit = 0L;
		ESHK(shkp)->loan = 0L;
	}
}

STATIC_OVL long
addupbill(shkp)
register struct monst *shkp;
{
	register int ct = ESHK(shkp)->billct;
	register struct bill_x *bp = ESHK(shkp)->bill_p;
	register long total = 0L;

	while(ct--){
		total += bp->price * bp->bquan;
		bp++;
	}
	return(total);
}

#endif /* OVLB */
#ifdef OVL1

STATIC_OVL void
call_kops(shkp, nearshop)
register struct monst *shkp;
register boolean nearshop;
{
	/* Keystone Kops srt@ucla */
	register boolean nokops;
	char kopname[20];
  
	strcpy(kopname, "Keystone Kops");

	if(!shkp) return;

	if (flags.soundok) {
	    pline("An alarm sounds!");
	    if (PlayerHearsSoundEffects) pline(issoviet ? "Veselites' vmeste s politseyskimi. Oni budut vas arestovat', ili na samom dele, tak kak vash personazh tak pateticheski slaby, oni mogut prosto ubit' vas skhodu, a zatem vy mozhete svernut' novuyu. Da!" : "Wueueueueueue! Wueueueueueue! Wueueueueueue! Wueueueueueue! Wueueueueueue!");
	}

	/* make new ones appear over time for a while, even if you switch dungeon levels to escape them --Amy */
	u.copwantedlevel += rnz(1000);

	nokops = ((mvitals[PM_KEYSTONE_KOP].mvflags & G_GONE) &&
		  (mvitals[PM_KOP_SERGEANT].mvflags & G_GONE) &&
		  (mvitals[PM_KOP_LIEUTENANT].mvflags & G_GONE) &&
		  (mvitals[PM_KOP_KOMMISSIONER].mvflags & G_GONE) &&
		  (mvitals[PM_KOP_KCHIEF].mvflags & G_GONE) &&
		  (mvitals[PM_KOP_KATCHER].mvflags & G_GONE) &&
		  (mvitals[PM_KOP_KAPTAIN].mvflags & G_GONE));

      if (Is_blackmarket(&u.uz)) {
	nokops = ((mvitals[PM_SOLDIER].mvflags & G_GONE) &&
		  (mvitals[PM_SERGEANT].mvflags & G_GONE) &&
		  (mvitals[PM_LIEUTENANT].mvflags & G_GONE) &&
		  (mvitals[PM_CAPTAIN].mvflags & G_GONE));
 
	strcpy(kopname, "guards");
      }
 
	/* Let's just assume not all K are genocided. --Amy */

	/*if(!angry_guards(!flags.soundok) && nokops) {
	    if(flags.verbose && flags.soundok)
		pline("But no one seems to respond to it.");
	    return;
	}

	if(nokops) return;*/

	{
	    coord mm;

	    if (nearshop)
	    if (!Is_blackmarket(&u.uz)) 
		{
		/* Create swarm around you, if you merely "stepped out" */
		if (flags.verbose)
		    pline_The("%s appear!", kopname);
		mm.x = u.ux;
		mm.y = u.uy;
		makekops(&mm);
		return;
	    }
	    if (flags.verbose)
		 pline_The("%s are after you!", kopname);
	    /* Create swarm near down staircase (hinders return to level) */
	    if (Is_blackmarket(&u.uz)) {
	      struct trap *trap = ftrap;
	      while (trap) {
		if (trap->ttyp == MAGIC_PORTAL) {
		  mm.x = trap->tx;
		  mm.y = trap->ty;
		}
		trap = trap->ntrap;
	      }
	    } else {
		mm.x = xdnstair;
		mm.y = ydnstair;
	    }            
	    makekops(&mm);
	    /* Create swarm near shopkeeper (hinders return to shop) */
	    mm.x = shkp->mx;
	    mm.y = shkp->my;
	    makekops(&mm);
	}
}


void 
blkmar_guards(shkp)
register struct monst *shkp;
{
    register struct monst *mt;
    register struct eshk *eshkp = ESHK(shkp);
    boolean mesg_given = FALSE;	/* Only give message if assistants peaceful */
    static boolean rlock = FALSE; /* Prevent recursive calls (via wakeup) */

    if (rlock)  return;
    rlock = TRUE;

    /* wake up assistants */
    for (mt = fmon; mt; mt = mt->nmon) {
	if (DEADMONSTER(mt)) continue;
	/* non-tame named monsters are presumably
	 * black marketeer's assistants */
	if (!mt->mtame && NAME(mt) && *NAME(mt) && mt->mpeaceful &&
		mt != shkp && inside_shop(mt->mx, mt->my) == eshkp->shoproom) {
	    if (!mesg_given) {
		pline("%s calls for %s assistants!",
			noit_Monnam(shkp), mhis(shkp));
		mesg_given = TRUE;
	    }
	    wakeup(mt);
	}
    }
    rlock = FALSE;
}

/* x,y is strictly inside shop */
char
inside_shop(x, y)
register xchar x, y;
{
	register char rno;

	rno = levl[x][y].roomno;
	if ((rno < ROOMOFFSET) || levl[x][y].edge || !IS_SHOP(rno-ROOMOFFSET))
	    return(NO_ROOM);
	else
	    return(rno);
}

void
u_left_shop(leavestring, newlev)
char *leavestring;
boolean newlev;
{
	struct monst *shkp;
	struct eshk *eshkp;

	/*
	 * IF player
	 * ((didn't leave outright) AND
	 *  ((he is now strictly-inside the shop) OR
	 *   (he wasn't strictly-inside last turn anyway)))
	 * THEN (there's nothing to do, so just return)
	 */
	if(!*leavestring &&
	   (!levl[u.ux][u.uy].edge || levl[u.ux0][u.uy0].edge))
	    return;

	shkp = shop_keeper(*u.ushops0);
	if (!shkp || !inhishop(shkp))
	    return;	/* shk died, teleported, changed levels... */

	eshkp = ESHK(shkp);
	if (!eshkp->billct && !eshkp->debit)	/* bill is settled */
	    return;

	if (!*leavestring && shkp->mcanmove && !shkp->msleeping) {
	    /*
	     * Player just stepped onto shop-boundary (known from above logic).
	     * Try to intimidate him into paying his bill
	     */
	    verbalize(NOTANGRY(shkp) ?
		      "%s!  Please pay before leaving." :
		      "%s!  Don't you leave without paying!",
		      playeraliasname);
	    return;
	}

	if (rob_shop(shkp)) {

	    if (Is_blackmarket(&u.uz))
		blkmar_guards(shkp);

	    call_kops(shkp, (!newlev && levl[u.ux0][u.uy0].edge));
	}
}

/* robbery from outside the shop via telekinesis or grappling hook */
void
remote_burglary(x, y)
xchar x, y;
{
	struct monst *shkp;
	struct eshk *eshkp;

	shkp = shop_keeper(*in_rooms(x, y, SHOPBASE));
	if (!shkp || !inhishop(shkp))
	    return;	/* shk died, teleported, changed levels... */

	eshkp = ESHK(shkp);
	if (!eshkp->billct && !eshkp->debit)	/* bill is settled */
	    return;

	if (rob_shop(shkp)) {

	    if (Is_blackmarket(&u.uz))
		blkmar_guards(shkp);

	    /*[might want to set 2nd arg based on distance from shop doorway]*/
	    call_kops(shkp, FALSE);
	}
}

/* shop merchandise has been taken; pay for it with any credit available;
   return false if the debt is fully covered by credit, true otherwise */
static boolean
rob_shop(shkp)
struct monst *shkp;
{
	struct eshk *eshkp;
	long total;

	eshkp = ESHK(shkp);
	rouse_shk(shkp, TRUE);
	total = (addupbill(shkp) + eshkp->debit);
	if (eshkp->credit >= total) {
	    Your("credit of %ld %s is used to cover your shopping bill.",
		 eshkp->credit, currency(eshkp->credit));
	    total = 0L;		/* credit gets cleared by setpaid() */
	} else {
	    You("escaped the shop without paying!");
	    total -= eshkp->credit;
	}
	setpaid(shkp);
	if (!total) return FALSE;

	/* by this point, we know an actual robbery has taken place */
	eshkp->robbed += total;

	/* it was annoying that the total was completely irrelevant unless you somehow wanted to compensate the shopkeeper,
	 * and therefore I decided to have it influence the kop wanted level --Amy */
	if ((!rn2(5) || Role_if(PM_CELLAR_CHILD)) && (total > 10)) u.copwantedlevel += rnz(total / 5);

	You("stole %ld %s worth of merchandise.",
	    total, currency(total));
	u.cnd_stealamount += total;
	if (!Role_if(PM_ROGUE)) {     /* stealing is unlawful */
		adjalign(-sgn(u.ualign.type));
		You_feel("like an evil rogue.");
		if (u.ualign.type == A_LAWFUL) {
			u.ualign.sins++;
			u.alignlim--;
		}
	}

	hot_pursuit(shkp);

	if (practicantterror && total > 0) { /* can happen several times --Amy */
		pline("%s thunders: 'That's theft! You have to pay twice the amount that the stolen goods cost, you hear? Be glad that I'm not giving you hall exclusion!'", noroelaname());
		fineforpracticant(total * 2, 0, 0);
	}

	return TRUE;
}

void
u_entered_shop(enterstring)
register char *enterstring;
{

	register int rt;
	register struct monst *shkp;
	register struct eshk *eshkp;
	static const char no_shk[] = "This shop appears to be deserted.";
	static char empty_shops[5];

	if(!*enterstring)
		return;

	if(!(shkp = shop_keeper(*enterstring))) {
	    if (!index(empty_shops, *enterstring) &&
		in_rooms(u.ux, u.uy, SHOPBASE) !=
				  in_rooms(u.ux0, u.uy0, SHOPBASE))
		pline(no_shk);
	    strcpy(empty_shops, u.ushops);
	    u.ushops[0] = '\0';
	    return;
	}

	eshkp = ESHK(shkp);

	if (!inhishop(shkp)) {
	    /* dump core when referenced */
	    eshkp->bill_p = (struct bill_x *) -1000;
	    if (!index(empty_shops, *enterstring))
		pline(no_shk);
	    strcpy(empty_shops, u.ushops);
	    u.ushops[0] = '\0';
	    return;
	}

	eshkp->bill_p = &(eshkp->bill[0]);

	if ((!eshkp->visitct || *eshkp->customer) &&
	    strncmpi(eshkp->customer, plname, PL_NSIZ)) {
	    /* You seem to be new here */
	    eshkp->visitct = 0;
	    eshkp->following = 0;
	    (void) strncpy(eshkp->customer,plname,PL_NSIZ);
	    pacify_shk(shkp);
	}

	if (shkp->msleeping || !shkp->mcanmove || eshkp->following)
	    return;	/* no dialog */

	if (Invis) {
	    pline("%s senses your presence.", shkname(shkp));
	    if (!Is_blackmarket(&u.uz)) {
	    verbalize("Invisible customers are not welcome!");
	    return;
	}
	}
 
	    if (Is_blackmarket(&u.uz) &&
		u.umonnum>0 && mons[u.umonnum].mlet != S_HUMAN) {
	      verbalize("Non-human customers are not welcome!");
	      return;
	}

	/* Visible striped prison shirt */
	if ((uarmu && (uarmu->otyp == STRIPED_SHIRT)) && !uarm && !uarmc) {
	    eshkp->pbanned = TRUE;
	}
	if (Race_if(PM_ALBAE) && !Upolyd) eshkp->pbanned = TRUE;
	if (Race_if(PM_IRAHA)) eshkp->pbanned = TRUE;

	rt = rooms[*enterstring - ROOMOFFSET].rtype;

	if (ANGRY(shkp)) {
	    verbalize("So, %s, you dare return to %s %s?!",
		      playeraliasname,
		      s_suffix(shkname(shkp)),
		      shtypes[rt - SHOPBASE].name);
	} else if (eshkp->robbed) {
	    pline("%s mutters imprecations against shoplifters.", shkname(shkp));
	} else {
        if (!eshkp->pbanned || inside_shop(u.ux, u.uy))
	    verbalize("%s, %s!  Welcome%s to %s %s!",
		      Hello(shkp), playeraliasname,
		      eshkp->visitct++ ? " again" : "",
		      s_suffix(shkname(shkp)),
		      shtypes[rt - SHOPBASE].name);
	}
	/* can't do anything about blocking if teleported in */
	if (!inside_shop(u.ux, u.uy)) {
	    boolean should_block;
	    int cnt;
	    const char *tool;
	    struct obj *pick = carrying(PICK_AXE),
			 *pickB = carrying(CONGLOMERATE_PICK),
			 *pickC = carrying(BRONZE_PICK),
			 *pickD = carrying(BRICK_PICK),
			 *pickE = carrying(NANO_PICK),
			 *pickF = carrying(MYSTERY_PICK),
			 *pickG = carrying(CONUNDRUM_PICK),
		       *mattock = carrying(DWARVISH_MATTOCK),
		       *mattockB = carrying(SOFT_MATTOCK),
			 *mattockC = carrying(ETERNIUM_MATTOCK);

	    if (pick || pickB || pickC || pickD || pickE || pickF || pickG || mattock || mattockB || mattockC) {
		cnt = 1;
		if (pick && mattock) {	/* carrying both types */
		    tool = "digging tool";
		    cnt = 2;	/* `more than 1' is all that matters */
		} else if (pick) {
		    tool = "pick-axe";
		    /* hack: `pick' already points somewhere into inventory */
		    while ((pick = pick->nobj) != 0)
			if (pick->otyp == PICK_AXE) ++cnt;
		} else if (pickB) {
		    tool = "conglomerate pick";
		    while ((pickB = pickB->nobj) != 0)
			if (pickB->otyp == CONGLOMERATE_PICK) ++cnt;
		} else if (pickC) {
		    tool = "bronze pick";
		    while ((pickC = pickC->nobj) != 0)
			if (pickC->otyp == BRONZE_PICK) ++cnt;
		} else if (pickD) {
		    tool = "brick pick";
		    while ((pickD = pickD->nobj) != 0)
			if (pickD->otyp == BRICK_PICK) ++cnt;
		} else if (pickE) {
		    tool = "nano pick";
		    while ((pickE = pickE->nobj) != 0)
			if (pickE->otyp == NANO_PICK) ++cnt;
		} else if (pickF) {
		    tool = "mystery pick";
		    while ((pickF = pickF->nobj) != 0)
			if (pickF->otyp == MYSTERY_PICK) ++cnt;
		} else if (pickG) {
		    tool = "conundrum pick";
		    while ((pickG = pickG->nobj) != 0)
			if (pickG->otyp == CONUNDRUM_PICK) ++cnt;
		} else if (mattock) {
		    tool = "mattock";
		    while ((mattock = mattock->nobj) != 0)
			if (mattock->otyp == DWARVISH_MATTOCK) ++cnt;
		    /* [ALI] Shopkeeper indicates mattock(s) */
		    if (!Blind) makeknown(DWARVISH_MATTOCK);
		} else if (mattockB) {
		    tool = "mattock";
		    while ((mattockB = mattockB->nobj) != 0)
			if (mattockB->otyp == ETERNIUM_MATTOCK) ++cnt;
		} else { /* mattockC */
		    tool = "mattock";
		    while ((mattockC = mattockC->nobj) != 0)
			if (mattockC->otyp == SOFT_MATTOCK) ++cnt;
		}
		verbalize(NOTANGRY(shkp) ?
			  "Will you please leave your %s%s outside?" :
			  "Leave the %s%s outside.",
			  tool, plur(cnt));
		should_block = TRUE;
	    } else if (u.usteed) {
		verbalize(NOTANGRY(shkp) ?
			  "Will you please leave %s outside?" :
			  "Leave %s outside.", y_monnam(u.usteed));
		should_block = TRUE;
	    } else if (eshkp->pbanned && !(Is_blackmarket(&u.uz)) ) {
	    verbalize("I don't sell to your kind here.");
		should_block = TRUE;
	    } else {
		should_block = (Fast && (sobj_at(PICK_AXE, u.ux, u.uy) || sobj_at(CONGLOMERATE_PICK, u.ux, u.uy) || sobj_at(CONUNDRUM_PICK, u.ux, u.uy) || sobj_at(MYSTERY_PICK, u.ux, u.uy) || sobj_at(BRONZE_PICK, u.ux, u.uy) || sobj_at(BRICK_PICK, u.ux, u.uy) || sobj_at(NANO_PICK, u.ux, u.uy) || sobj_at(SOFT_MATTOCK, u.ux, u.uy) || sobj_at(ETERNIUM_MATTOCK, u.ux, u.uy) ||
				      sobj_at(DWARVISH_MATTOCK, u.ux, u.uy)));
	    }
	    if (should_block) (void) dochug(shkp);  /* shk gets extra move */
	}
	return;
}

/*
   Decide whether two unpaid items are mergable; caller is responsible for
   making sure they're unpaid and the same type of object; we check the price
   quoted by the shopkeeper and also that they both belong to the same shk.
 */
boolean
same_price(obj1, obj2)
struct obj *obj1, *obj2;
{
	register struct monst *shkp1, *shkp2;
	register struct bill_x *bp1 = 0, *bp2 = 0;
	register boolean are_mergable = FALSE;

	/* look up the first object by finding shk whose bill it's on */
	for (shkp1 = next_shkp(fmon, TRUE); shkp1;
		shkp1 = next_shkp(shkp1->nmon, TRUE))
	    if ((bp1 = onbill(obj1, shkp1, TRUE)) != 0) break;
	/* second object is probably owned by same shk; if not, look harder */
	if (shkp1 && (bp2 = onbill(obj2, shkp1, TRUE)) != 0) {
	    shkp2 = shkp1;
	} else {
	    for (shkp2 = next_shkp(fmon, TRUE); shkp2;
		    shkp2 = next_shkp(shkp2->nmon, TRUE))
		if ((bp2 = onbill(obj2, shkp2, TRUE)) != 0) break;
	}

	if (!bp1 || !bp2) impossible("same_price: object wasn't on any bill!");
	else are_mergable = (shkp1 == shkp2 && bp1->price == bp2->price);
	return are_mergable;
}

/*
 * Figure out how much is owed to a given shopkeeper.
 * At present, we ignore any amount robbed from the shop, to avoid
 * turning the `$' command into a way to discover that the current
 * level is bones data which has a shk on the warpath.
 */
STATIC_OVL long
shop_debt(eshkp)
struct eshk *eshkp;
{
	struct bill_x *bp;
	int ct;
	long debt = eshkp->debit;

	for (bp = eshkp->bill_p, ct = eshkp->billct; ct > 0; bp++, ct--)
	    debt += bp->price * bp->bquan;
	return debt;
}

/* called in response to the `$' command */
void
shopper_financial_report()
{
	struct monst *shkp, *this_shkp = shop_keeper(inside_shop(u.ux, u.uy));
	struct eshk *eshkp;
	long amt;
	int pass;

	if (this_shkp &&
	    !(ESHK(this_shkp)->credit || shop_debt(ESHK(this_shkp)))) {
	    You("have no credit or debt in here.");
	    this_shkp = 0;	/* skip first pass */
	}

	/* pass 0: report for the shop we're currently in, if any;
	   pass 1: report for all other shops on this level. */
	for (pass = this_shkp ? 0 : 1; pass <= 1; pass++)
	    for (shkp = next_shkp(fmon, FALSE);
		    shkp; shkp = next_shkp(shkp->nmon, FALSE)) {
		if ((shkp != this_shkp) ^ pass) continue;
		eshkp = ESHK(shkp);
		if ((amt = eshkp->credit) != 0)
		    You("have %ld %s credit at %s %s.",
			amt, currency(amt), s_suffix(shkname(shkp)),
			shtypes[eshkp->shoptype - SHOPBASE].name);
		else if (shkp == this_shkp)
		    You("have no credit in here.");
		if ((amt = shop_debt(eshkp)) != 0)
		    You("owe %s %ld %s.",
			shkname(shkp), amt, currency(amt));
		else if (shkp == this_shkp)
		    You("don't owe any money here.");
	    }
}

#endif /* OVL1 */
#ifdef OVLB

int
inhishop(mtmp)
register struct monst *mtmp;
{
	return(index(in_rooms(mtmp->mx, mtmp->my, SHOPBASE),
		     ESHK(mtmp)->shoproom) &&
		on_level(&(ESHK(mtmp)->shoplevel), &u.uz));
}

struct monst *
shop_keeper(rmno)
register char rmno;
{
	struct monst *shkp = rmno >= ROOMOFFSET ?
				rooms[rmno - ROOMOFFSET].resident : 0;

	if (shkp) {
	    if (NOTANGRY(shkp)) {
		if (ESHK(shkp)->surcharge) pacify_shk(shkp);
	    } else {
		if (!ESHK(shkp)->surcharge) rile_shk(shkp);
	    }
	}
	return shkp;
}

boolean
tended_shop(sroom)
register struct mkroom *sroom;
{
	register struct monst *mtmp = sroom->resident;

	if (!mtmp)
		return(FALSE);
	else
		return((boolean)(inhishop(mtmp)));
}

STATIC_OVL struct bill_x *
onbill(obj, shkp, silent)
register struct obj *obj;
register struct monst *shkp;
register boolean silent;
{
	if (shkp) {
		register struct bill_x *bp = ESHK(shkp)->bill_p;
		register int ct = ESHK(shkp)->billct;

		while (--ct >= 0)
		    if (bp->bo_id == obj->o_id) {
			if (!obj->unpaid) pline("onbill: paid obj on bill?");
			return bp;
		    } else bp++;
	}
	if(obj->unpaid & !silent) pline("onbill: unpaid obj not on bill?");
	return (struct bill_x *)0;
}

/* Delete the contents of the given object. */
void
delete_contents(obj)
register struct obj *obj;
{
	register struct obj *curr;

	while ((curr = obj->cobj) != 0) {
	    if (Has_contents(curr)) delete_contents(curr);
	    obj_extract_self(curr);
	    if (evades_destruction(curr)) {
		switch (obj->where) {
		    case OBJ_FREE:
		    case OBJ_ONBILL:
			impossible("indestructible object %s",
			  obj->where == OBJ_FREE ? "free" : "on bill");
			obfree(curr, (struct obj *)0);
			break;
		    case OBJ_FLOOR:
			place_object(curr, obj->ox, obj->oy);
			/* No indestructible objects currently stack */
			break;
		    case OBJ_CONTAINED:
			add_to_container(obj->ocontainer, curr);
			break;
		    case OBJ_INVENT:
			if (!flooreffects(curr, u.ux, u.uy, "fall"))
			    place_object(curr, u.ux, u.uy);
			break;
		    case OBJ_MINVENT:
			if (!flooreffects(curr,
				obj->ocarry->mx, obj->ocarry->my, "fall"))
			    place_object(curr, obj->ocarry->mx, obj->ocarry->my);
			break;
		    case OBJ_MIGRATING:
			add_to_migration(curr);
			/* Copy migration destination */
			curr->ox = obj->ox;
			curr->oy = obj->oy;
			curr->owornmask = obj->owornmask;
			break;
		    case OBJ_BURIED:
			add_to_buried(curr);
			curr->ox = obj->ox;
			curr->oy = obj->oy;
			break;
		    default:
			panic("delete_contents");
			break;
		}
	    }
	    else
	    obfree(curr, (struct obj *)0);
	}
}

/* called with two args on merge */
void
obfree(obj, merge)
register struct obj *obj, *merge;
{
	register struct bill_x *bp;
	register struct bill_x *bpm;
	register struct monst *shkp;

	if (obj == usaddle) dismount_steed(DISMOUNT_GENERIC);

	if (obj->otyp == LEATHER_LEASH && obj->leashmon) o_unleash(obj);
	if (obj->otyp == INKA_LEASH && obj->leashmon) o_unleash(obj);
	if (obj->oclass == SPBOOK_CLASS) book_disappears(obj);
	if (obj->oclass == FOOD_CLASS) food_disappears(obj);
	/* [ALI] Enforce new rules: Containers must have their contents
	 * deleted while still in situ so that we can place any
	 * indestructible objects they may contain.
	 * Amy edit: the buglog file would laaaaag like hell with thousands of faux objects, so we'll scrap that
	 */
	if (Has_contents(obj)) {
		impossible("BUG: obfree() called on non-empty container");
		delete_contents(obj);
	}

	shkp = 0;
	if (obj->unpaid) {
	    /* look for a shopkeeper who owns this object */
	    for (shkp = next_shkp(fmon, TRUE); shkp;
		    shkp = next_shkp(shkp->nmon, TRUE))
		if (onbill(obj, shkp, TRUE)) break;
	}
	/* sanity check, more or less */
	if (!shkp) shkp = shop_keeper(*u.ushops);
		/*
		 * Note:  `shkp = shop_keeper(*u.ushops)' used to be
		 *	  unconditional.  But obfree() is used all over
		 *	  the place, so making its behavior be dependent
		 *	  upon player location doesn't make much sense.
		 */

	if ((bp = onbill(obj, shkp, FALSE)) != 0) {
		if(!merge){
			bp->useup = 1;
			obj->unpaid = 0;	/* only for doinvbill */
			add_to_billobjs(obj);
			return;
		}
		bpm = onbill(merge, shkp, FALSE);
		if(!bpm){
			/* this used to be a rename */
			impossible("obfree: not on bill??");
			return;
		} else {
			/* this was a merger */
			bpm->bquan += bp->bquan;
			ESHK(shkp)->billct--;
#ifdef DUMB
			{
			/* DRS/NS 2.2.6 messes up -- Peter Kendell */
				int indx = ESHK(shkp)->billct;
				*bp = ESHK(shkp)->bill_p[indx];
			}
#else
			*bp = ESHK(shkp)->bill_p[ESHK(shkp)->billct];
#endif
		}
	}

	if ((Race_if(PM_PLAYER_MUSHROOM) || (uchain && uchain->oartifact == ART_ERO_ERO_ERO_ERO_MUSHROOM_M)) && u.mushroompoleused) {
		setnotworn(obj);
		obj_extract_self(obj);
	}

	dealloc_obj(obj);
}
#endif /* OVLB */
#ifdef OVL3

STATIC_OVL long
check_credit(tmp, shkp)
long tmp;
register struct monst *shkp;
{
	long credit = ESHK(shkp)->credit;

	if(credit == 0L) return(tmp);
	if(credit >= tmp) {
		pline_The("price is deducted from your credit.");
		ESHK(shkp)->credit -=tmp;
		tmp = 0L;
	} else {
		pline_The("price is partially covered by your credit.");
		ESHK(shkp)->credit = 0L;
		tmp -= credit;
	}
	return(tmp);
}

STATIC_OVL void
pay(tmp,shkp)
long tmp;
register struct monst *shkp;
{
	long robbed = ESHK(shkp)->robbed;
	long balance = ((tmp <= 0L) ? tmp : check_credit(tmp, shkp));

#ifndef GOLDOBJ
	u.ugold -= balance;
	if (rn2(2) || balance < 0) shkp->mgold += balance;
#else
	if (balance > 0) money2mon(shkp, balance);
	else if (balance < 0) money2u(shkp, -balance);
#endif
	flags.botl = 1;
	if(robbed) {
		robbed -= tmp;
		if(robbed < 0) robbed = 0L;
		ESHK(shkp)->robbed = robbed;
	}
}
#endif /*OVL3*/
#ifdef OVLB

/* return shkp to home position */
void
home_shk(shkp, killkops)
register struct monst *shkp;
register boolean killkops;
{
	register xchar x = ESHK(shkp)->shk.x, y = ESHK(shkp)->shk.y;

	(void) mnearto(shkp, x, y, TRUE);
	level.flags.has_shop = 1;
	if (killkops) {
		/* kops_gone(TRUE); */
		pacify_guards();
	}
	after_shk_move(shkp);
}

STATIC_OVL boolean
angry_shk_exists()
{
	register struct monst *shkp;

	for (shkp = next_shkp(fmon, FALSE);
		shkp; shkp = next_shkp(shkp->nmon, FALSE))
	    if (ANGRY(shkp)) return(TRUE);
	return(FALSE);
}

/* remove previously applied surcharge from all billed items */
STATIC_OVL void
pacify_shk(shkp)
register struct monst *shkp;
{
	NOTANGRY(shkp) = TRUE;	/* make peaceful */
	if (ESHK(shkp)->surcharge) {
		register struct bill_x *bp = ESHK(shkp)->bill_p;
		register int ct = ESHK(shkp)->billct;

		ESHK(shkp)->surcharge = FALSE;
		while (ct-- > 0) {
			register long reduction = (bp->price + 3L) / 4L;
			bp->price -= reduction;		/* undo 33% increase */
			bp++;
		}
	}
}

/* add aggravation surcharge to all billed items */
STATIC_OVL void
rile_shk(shkp)
register struct monst *shkp;
{
	NOTANGRY(shkp) = FALSE;	/* make angry */
	if (!ESHK(shkp)->surcharge) {
		register struct bill_x *bp = ESHK(shkp)->bill_p;
		register int ct = ESHK(shkp)->billct;

		ESHK(shkp)->surcharge = TRUE;
		while (ct-- > 0) {
			register long surcharge = (bp->price + 2L) / 3L;
			bp->price += surcharge;
			bp++;
		}
	}
}

/* wakeup and/or unparalyze shopkeeper */
STATIC_OVL void
rouse_shk(shkp, verbosely)
struct monst *shkp;
boolean verbosely;
{
	if (!shkp->mcanmove || shkp->msleeping) {
	    /* greed induced recovery... */
	    if (verbosely && canspotmon(shkp))
		pline("%s %s.", Monnam(shkp),
		      shkp->msleeping ? "wakes up" : "can move again");
	    shkp->msleeping = 0;
	    shkp->mfrozen = 0;
	    shkp->masleep = 0;
	    shkp->mcanmove = 1;
	}
}

void
make_happy_shk(shkp, silentkops)
register struct monst *shkp;
register boolean silentkops;
{
	boolean wasmad = ANGRY(shkp);
	struct eshk *eshkp = ESHK(shkp);
	boolean guilty = wasmad ||
		eshkp->surcharge || eshkp->following || eshkp->robbed;

	pacify_shk(shkp);
	eshkp->following = 0;
	eshkp->robbed = 0L;
	if (guilty && !Role_if(PM_ROGUE)) {
		adjalign(sgn(u.ualign.type));
		You_feel("your guilt vanish.");        
	}
	if(!inhishop(shkp)) {
		char shk_nam[BUFSZ];
		boolean vanished = canseemon(shkp);

		strcpy(shk_nam, mon_nam(shkp));
		if (on_level(&eshkp->shoplevel, &u.uz)) {
			home_shk(shkp, FALSE);
			/* didn't disappear if shk can still be seen */
			if (canseemon(shkp)) vanished = FALSE;
		} else {
			/* if sensed, does disappear regardless whether seen */
			if (sensemon(shkp)) vanished = TRUE;
			/* can't act as porter for the Amulet, even if shk
			   happens to be going farther down rather than up */
			mdrop_special_objs(shkp);
			/* arrive near shop's door */
			migrate_to_level(shkp, ledger_no(&eshkp->shoplevel),
					 MIGR_APPROX_XY, &eshkp->shd);
		}
		if (vanished)
		    pline("Satisfied, %s suddenly disappears!", shk_nam);
	} else if(wasmad)
		pline("%s calms down.", Monnam(shkp));

	if(!angry_shk_exists()) {
/*#ifdef KOPS
		kops_gone(silentkops);
#endif*/
		pacify_guards();
	}
}

void
hot_pursuit(shkp)
register struct monst *shkp;
{
	if(!shkp->isshk) return;

	rile_shk(shkp);
	(void) strncpy(ESHK(shkp)->customer, plname, PL_NSIZ);
	ESHK(shkp)->following = 1;
}

/* used when the shkp is teleported or falls (ox == 0) out of his shop,
 * or when the player is not on a costly_spot and he
 * damages something inside the shop.  these conditions
 * must be checked by the calling function.
 */
void
make_angry_shk(shkp, ox, oy)
register struct monst *shkp;
register xchar ox,oy;
{
	xchar sx, sy;
	struct eshk *eshkp = ESHK(shkp);

	/* all pending shop transactions are now "past due" */
	if (eshkp->billct || eshkp->debit || eshkp->loan || eshkp->credit) {
	    eshkp->robbed += (addupbill(shkp) + eshkp->debit + eshkp->loan);
	    eshkp->robbed -= eshkp->credit;
	    if (eshkp->robbed < 0L) eshkp->robbed = 0L;
	    /* billct, debit, loan, and credit will be cleared by setpaid */
	    setpaid(shkp);
	}

	/* If you just used a wand of teleportation to send the shk away, you
	   might not be able to see her any more.  Monnam would yield "it",
	   which makes this message look pretty silly, so temporarily restore
	   her original location during the call to Monnam. */
	sx = shkp->mx,  sy = shkp->my;
	if (isok(ox, oy) && cansee(ox, oy) && !cansee(sx, sy))
		shkp->mx = ox,  shkp->my = oy;
	pline("%s %s!", Monnam(shkp),
	      !ANGRY(shkp) ? "gets angry" : "is furious");
	shkp->mx = sx,  shkp->my = sy;
	hot_pursuit(shkp);
}

STATIC_VAR const char no_money[] = "Moreover, you%s have no money.";
STATIC_VAR const char not_enough_money[] =
			    "Besides, you don't have enough to interest %s.";

#else
STATIC_VAR const char no_money[];
STATIC_VAR const char not_enough_money[];
#endif /*OVLB*/

#ifdef OVL3

STATIC_OVL long
cheapest_item(shkp)   /* delivers the cheapest item on the list */
register struct monst *shkp;
{
	register int ct = ESHK(shkp)->billct;
	register struct bill_x *bp = ESHK(shkp)->bill_p;
	register long gmin = (bp->price * bp->bquan);

	while(ct--){
		if(bp->price * bp->bquan < gmin)
			gmin = bp->price * bp->bquan;
		bp++;
	}
	return(gmin);
}
#endif /*OVL3*/
#ifdef OVL0

int
dopay()
{
	/* are you a practicant that currently has a fine? if so, you can only pay that and nothing else */
	if (practicantterror && (u.practicantpenalty || u.practicantstones || u.practicantarrows)) {
		practicant_payup();
		return 0;
	}

	if (Race_if(PM_PLAYER_DYNAMO)) {
		You("can't pay because no one wants to accept payment by you, criminal.");
		return 0;
	}

	if (MenuIsBugged) {
	pline("The pay command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	register struct eshk *eshkp;
	register struct monst *shkp;
	struct monst *nxtm, *resident;
	long ltmp;
#ifdef GOLDOBJ
	long umoney;
#endif
	int pass, tmp, sk = 0, seensk = 0;
	boolean paid = FALSE, stashed_gold = (hidden_gold() > 0L);

	multi = 0;

	/* find how many shk's there are, how many are in */
	/* sight, and are you in a shop room with one.    */
	nxtm = resident = 0;
	for (shkp = next_shkp(fmon, FALSE);
		shkp; shkp = next_shkp(shkp->nmon, FALSE)) {
	    sk++;
	    if (ANGRY(shkp) && distu(shkp->mx, shkp->my) <= 2) nxtm = shkp;
	    if (canspotmon(shkp)) seensk++;
	    if (inhishop(shkp) && (*u.ushops == ESHK(shkp)->shoproom))
		resident = shkp;
	}

	if (nxtm) {			/* Player should always appease an */
	     shkp = nxtm;		/* irate shk standing next to them. */
	     goto proceed;
	}

	/* KMH -- Permit paying adjacent gypsies */
	for (nxtm = fmon; nxtm; nxtm = nxtm->nmon) {
		if (!nxtm->isgyp || !nxtm->mpeaceful ||
				distu(nxtm->mx, nxtm->my) > 2 || !canspotmon(nxtm))
			continue;
		shkp = nxtm;
		sk++;
	    seensk++;
	}

	if ((!sk && (!Blind || Blind_telepat)) || (!Blind && !seensk)) {
      There("appears to be no shopkeeper here to receive your payment.");
		return(0);
	}

	if(!seensk) {
		You_cant("see...");
		return(0);
	}

	/* the usual case.  allow paying at a distance when */
	/* inside a tended shop.  should we change that?    */
	if(sk == 1 && resident) {
		shkp = resident;
		goto proceed;
	}

	if (seensk == 1) {
		/* KMH -- Permit paying gypsies */
		if (shkp && shkp->isgyp) {
			gypsy_chat(shkp);
			return (1);
		}

		for (shkp = next_shkp(fmon, FALSE);
			shkp; shkp = next_shkp(shkp->nmon, FALSE))
		    if (canspotmon(shkp)) break;
		if (shkp != resident && distu(shkp->mx, shkp->my) > 2) {
		    pline("%s is not near enough to receive your payment.",
					     Monnam(shkp));
		    return(0);
		}
	} else {
		struct monst *mtmp;
		coord cc;
		int cx, cy;

		pline("Pay whom?");
		cc.x = u.ux;
		cc.y = u.uy;
		if (getpos(&cc, TRUE, "the creature you want to pay") < 0)
		    return 0;	/* player pressed ESC */
		cx = cc.x;
		cy = cc.y;
		if(cx < 0) {
		     pline("Try again...");
		     return(0);
		}
		if(u.ux == cx && u.uy == cy) {
		     You("are generous to yourself.");
		     return(0);
		}
		mtmp = m_at(cx, cy);
		if (!cansee(cx, cy) && (!mtmp || !canspotmon(mtmp))) { /* bugfix from 3.7 - yeah I'm a filthy heretic :P --Amy */
		     You("can't see that location from here!");
		     return(0);
		}
		if(!mtmp) {
		     There("is no one there to receive your payment.");
		     return(0);
		}
		/* KMH -- Permit paying gypsies */
		if (mtmp->isgyp && mtmp->mpeaceful) {
			if (distu(mtmp->mx, mtmp->my) <= 2) {
				gypsy_chat(mtmp);
				return (1);
			}
		} else
		if(!mtmp->isshk) {
		     pline("%s is not interested in your payment.",
				    Monnam(mtmp));
		     return(0);
		}
		if (mtmp != resident && distu(mtmp->mx, mtmp->my) > 2) {
		     pline("%s is too far to receive your payment.",
				    Monnam(mtmp));
		     return(0);
		}
		shkp = mtmp;
	}

	if(!shkp) {
#ifdef DEBUG
		pline("dopay: null shkp.");
#endif
		return(0);
	}
proceed:
	eshkp = ESHK(shkp);
	ltmp = eshkp->robbed;

	/* wake sleeping shk when someone who owes money offers payment */
	if (ltmp || eshkp->billct || eshkp->debit) 
	    rouse_shk(shkp, TRUE);

	if (!shkp->mcanmove || shkp->msleeping) { /* still asleep/paralyzed */
		pline("%s %s.", Monnam(shkp),
		      rn2(2) ? "seems to be napping" : "doesn't respond");
		return 0;
	}

	if(shkp != resident && NOTANGRY(shkp)) {
#ifdef GOLDOBJ
                umoney = money_cnt(invent);
#endif
		if(!ltmp)
		    You("do not owe %s anything.", mon_nam(shkp));
#ifndef GOLDOBJ
		else if(!u.ugold) {
#else
		else if(!umoney) {
#endif
		    You("%shave no money.", stashed_gold ? "seem to " : "");
		    if(stashed_gold)

#ifndef GOLDOBJ
			pline("But you have some gold stashed away.");
#else
				pline("But you have some money stashed away.");
#endif
		} else {
#ifndef GOLDOBJ
		    long ugold = u.ugold;
		    if(ugold > ltmp) {
#else
		    if(umoney > ltmp) {
#endif
			You("give %s the %ld gold piece%s %s asked for.",
			    mon_nam(shkp), ltmp, plur(ltmp), mhe(shkp));
			pay(ltmp, shkp);
		    } else {
#ifndef GOLDOBJ
			You("give %s all your%s gold.", mon_nam(shkp),
#else
			You("give %s all your%s money.", mon_nam(shkp),
#endif
					stashed_gold ? " openly kept" : "");
#ifndef GOLDOBJ
			pay(u.ugold, shkp);
			if (stashed_gold) pline("But you have hidden gold!");
#else
			pay(umoney, shkp);
			if (stashed_gold) pline("But you have hidden money!");
#endif
		    }
#ifndef GOLDOBJ
		    if((ugold < ltmp/2L) || (ugold < ltmp && stashed_gold))
#else
		    if((umoney < ltmp/2L) || (umoney < ltmp && stashed_gold))
#endif
			pline("Unfortunately, %s doesn't look satisfied.",
			      mhe(shkp));
		    else
			make_happy_shk(shkp, FALSE);
		}
		return(1);
	}

	/* ltmp is still eshkp->robbed here */
	if (!eshkp->billct && !eshkp->debit) {
#ifdef GOLDOBJ
                umoney = money_cnt(invent);
#endif
		if(!ltmp && NOTANGRY(shkp)) {
		    You("do not owe %s anything.", mon_nam(shkp));
#ifndef GOLDOBJ
		    if (!u.ugold)
#else
		    if (!umoney)
#endif
			pline(no_money, stashed_gold ? " seem to" : "");
		 
/*		    else */
			shk_other_services();
		
		} else if(ltmp) {
		    pline("%s is after blood, not money!", Monnam(shkp));
#ifndef GOLDOBJ
		    if(u.ugold < ltmp/2L ||
				(u.ugold < ltmp && stashed_gold)) {
			if (!u.ugold)
#else
		    if(umoney < ltmp/2L ||
				(umoney < ltmp && stashed_gold)) {
			if (!umoney)
#endif
			    pline(no_money, stashed_gold ? " seem to" : "");
			else pline(not_enough_money, mhim(shkp));
			return(1);
		    }
		    pline("But since %s shop has been robbed recently,",
			  mhis(shkp));
		    pline("you %scompensate %s for %s losses.",
#ifndef GOLDOBJ
			  (u.ugold < ltmp) ? 
#else
			  (umoney < ltmp) ?
#endif
			  "partially " : "",
			  mon_nam(shkp), mhis(shkp));
#ifndef GOLDOBJ
		    pay(u.ugold < ltmp ? u.ugold : ltmp, shkp);
#else
		    pay(umoney < ltmp ? umoney : ltmp, shkp);
#endif
		    make_happy_shk(shkp, FALSE);
		} else {
		    /* shopkeeper is angry, but has not been robbed --
		     * door broken, attacked, etc. */
		    pline("%s is after your hide, not your money!",
			  Monnam(shkp));
#ifndef GOLDOBJ
		    if(u.ugold < 1000L) {
			if (!u.ugold)
#else
		    if(umoney < 1000L) {
			if (!umoney)
#endif
			    pline(no_money, stashed_gold ? " seem to" : "");
			else pline(not_enough_money, mhim(shkp));
			return(1);
		    }
		    You("try to appease %s by giving %s 1000 gold pieces.",
			x_monnam(shkp, ARTICLE_THE, "angry", 0, FALSE),
			mhim(shkp));
		    pay(1000L,shkp);
		    if (strncmp(eshkp->customer, plname, PL_NSIZ) || rn2(3))
			make_happy_shk(shkp, FALSE);
		    else
			pline("But %s is as angry as ever.", mon_nam(shkp));
		}
		return(1);
	}
	if(shkp != resident) {
		impossible("dopay: not to shopkeeper?");
		if(resident) setpaid(resident);
		return(0);
	}        
	/* pay debt, if any, first */
	if(eshkp->debit) {
		long dtmp = eshkp->debit;
		long loan = eshkp->loan;
		char sbuf[BUFSZ];
#ifdef GOLDOBJ
                umoney = money_cnt(invent);
#endif
		sprintf(sbuf, "You owe %s %ld %s ",
					   shkname(shkp), dtmp, currency(dtmp));
		if(loan) {
		    if(loan == dtmp)
			strcat(sbuf, "you picked up in the store.");
		    else strcat(sbuf,
			   "for gold picked up and the use of merchandise.");
		} else strcat(sbuf, "for the use of merchandise.");
		pline("%s", sbuf);
#ifndef GOLDOBJ
		if (u.ugold + eshkp->credit < dtmp) {
		    pline("But you don't%s have enough gold%s.",
#else
		if (umoney + eshkp->credit < dtmp) {
		    pline("But you don't%s have enough money%s.",
#endif

			stashed_gold ? " seem to" : "",
			eshkp->credit ? " or credit" : "");
		    return(1);
		} else {
		    if (eshkp->credit >= dtmp) {
			eshkp->credit -= dtmp;
			eshkp->debit = 0L;
			eshkp->loan = 0L;
			Your("debt is covered by your credit.");
		    } else if (!eshkp->credit) {
#ifndef GOLDOBJ
			u.ugold -= dtmp;
 			if (rn2(2) || dtmp < 0) shkp->mgold += dtmp;
#else
                        money2mon(shkp, dtmp);
#endif
			eshkp->debit = 0L;
			eshkp->loan = 0L;
			You("pay that debt.");
			flags.botl = 1;
		    } else {
			dtmp -= eshkp->credit;
			eshkp->credit = 0L;
#ifndef GOLDOBJ
			u.ugold -= dtmp;
			if (rn2(2) || dtmp < 0) shkp->mgold += dtmp;
#else
                        money2mon(shkp, dtmp);
#endif
			eshkp->debit = 0L;
			eshkp->loan = 0L;
			pline("That debt is partially offset by your credit.");
			You("pay the remainder.");
			flags.botl = 1;
		    }
		    paid = TRUE;
		}
	}
	/* now check items on bill */
	if (eshkp->billct) {
	    register boolean itemize;
#ifndef GOLDOBJ
	    if (!u.ugold && !eshkp->credit) {
#else
            umoney = money_cnt(invent);
	    if (!umoney && !eshkp->credit) {
#endif
		You("%shave no money or credit%s.",
				    stashed_gold ? "seem to " : "",
				    paid ? " left" : "");
		return(0);
	    }
#ifndef GOLDOBJ
	    if ((u.ugold + eshkp->credit) < cheapest_item(shkp)) {
#else
	    if ((umoney + eshkp->credit) < cheapest_item(shkp)) {
#endif
		You("don't have enough money to buy%s the item%s you picked.",
		    eshkp->billct > 1 ? " any of" : "", plur(eshkp->billct));
		if(stashed_gold)
#ifndef GOLDOBJ
		    pline("Maybe you have some gold stashed away?");
#else
		    pline("Maybe you have some money stashed away?");
#endif
		return(0);
	    }

	    /* this isn't quite right; it itemizes without asking if the
	     * single item on the bill is partly used up and partly unpaid */
	    itemize = (eshkp->billct > 1 ? yn("Itemized billing?") == 'y' : 1);

	    for (pass = 0; pass <= 1; pass++) {
		tmp = 0;
		while (tmp < eshkp->billct) {
		    struct obj *otmp;
		    register struct bill_x *bp = &(eshkp->bill_p[tmp]);

		    /* find the object on one of the lists */
		    if ((otmp = bp_to_obj(bp)) != 0) {
			/* if completely used up, object quantity is stale;
			   restoring it to its original value here avoids
			   making the partly-used-up code more complicated */
			if (bp->useup) otmp->quan = bp->bquan;
		    } else {
			impossible("Shopkeeper administration out of order.");
			setpaid(shkp);	/* be nice to the player */
			return 1;
		    }
		    if (pass == bp->useup && otmp->quan == bp->bquan) {
			/* pay for used-up items on first pass and others
			 * on second, so player will be stuck in the store
			 * less often; things which are partly used up
			 * are processed on both passes */
			tmp++;
		    } else {
			switch (dopayobj(shkp, bp, &otmp, pass, itemize)) {
			  case PAY_CANT:
				return 1;	/*break*/
			  case PAY_BROKE:
				paid = TRUE;
				goto thanks;	/*break*/
			  case PAY_SKIP:
				tmp++;
				continue;	/*break*/
			  case PAY_SOME:
				paid = TRUE;
				if (itemize) bot();
				continue;	/*break*/
			  case PAY_BUY:
				paid = TRUE;
				break;
			}
			if (itemize) bot();
			*bp = eshkp->bill_p[--eshkp->billct];
		    }
		}
	    }
	thanks:
	    if (!itemize)
	        update_inventory(); /* Done in dopayobj() if itemize. */
	}
	if(!ANGRY(shkp) && paid)
	    verbalize("Thank you for shopping in %s %s!",
		s_suffix(shkname(shkp)),
		shtypes[eshkp->shoptype - SHOPBASE].name);
	return(1);
}

/*
** FUNCTION shk_other_services
**
** Called when you don't owe any money.  Called after all checks have been
** made (in shop, not angry shopkeeper, etc.)
*/
static void
shk_other_services()
{
	char *slang;		/* What shk calls you		*/
	struct monst *shkp;		/* The shopkeeper		*/
	/*WAC - Windowstuff*/
	winid tmpwin;
	anything any;
	menu_item *selected;
	int n;

	/* Do you want to use other services */
	if (yn("Do you wish to try our other services?") != 'y' ) return;

	if (Race_if(PM_IRAHA)) {
		verbalize("Get the fuck out of here, insufferable scumbag!");
		return;
	}

	/* Init your name */
	if (Role_if(PM_CONVICT))
		slang = "scumbag";
	else if (Role_if(PM_MURDERER))
		slang = "killer";
	else if (Race_if(PM_ALBAE))
		slang = "murderer";
	else if (!is_human(youmonst.data))
		slang = "ugly";
	else
		slang = (flags.female) ? "lady" : "buddy";

	/* Init the shopkeeper */
	shkp = shop_keeper(/* roomno= */*u.ushops);
	if (!ESHK(shkp)->services) {
		verbalize("Sorry. We're all out of services.");
		return;
	}

	/*
	** Figure out what services he offers
	**
	** i = identify
	** a = appraise weapon's worth
	** u = uncurse
	** w = weapon-works (including poison)
	** p = poison weapon
	** r = armor-works
	** c = charge wands
	** e = establish credit
	*/
	/*WAC - did this using the windowing system...*/
	any.a_void = 0;         /* zero out all bits */
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);

  	/* All shops can identify (some better than others) */
	any.a_int = 1;
	if (ESHK(shkp)->services & (SHK_ID_BASIC|SHK_ID_PREMIUM))
	     add_menu(tmpwin, NO_GLYPH, &any , 'i', 0, ATR_NONE,
	         "Identify", MENU_UNSELECTED);
  
  	/* All shops can uncurse */
	any.a_int = 2;
	if (ESHK(shkp)->services & (SHK_UNCURSE))
	     add_menu(tmpwin, NO_GLYPH, &any , 'u', 0, ATR_NONE,
	         "Uncurse", MENU_UNSELECTED);
  
  	/* Weapon appraisals.  Weapon & general stores can do this. */
	if ((ESHK(shkp)->services & (SHK_APPRAISE)) &&
			(shk_class_match(WEAPON_CLASS, shkp))) {
		any.a_int = 3;
		add_menu(tmpwin, NO_GLYPH, &any , 'a', 0, ATR_NONE,
				"Appraise", MENU_UNSELECTED);
	}
  
  	/* Weapon-works!  Only a weapon store. */
	if ((ESHK(shkp)->services & (SHK_SPECIAL_A|SHK_SPECIAL_B|SHK_SPECIAL_C))
			&& (shk_class_match(WEAPON_CLASS, shkp) == SHK_MATCH)) {
		any.a_int = 4;
		if (ESHK(shkp)->services & (SHK_SPECIAL_A|SHK_SPECIAL_B))
			add_menu(tmpwin, NO_GLYPH, &any , 'w', 0, ATR_NONE,
				"Weapon-works", MENU_UNSELECTED);
		else
			add_menu(tmpwin, NO_GLYPH, &any , 'p', 0, ATR_NONE,
				"Poison", MENU_UNSELECTED);
	}
  
  	/* Armor-works */
	if ((ESHK(shkp)->services & (SHK_SPECIAL_A|SHK_SPECIAL_B))
			 && (shk_class_match(ARMOR_CLASS, shkp) == SHK_MATCH)) {
		any.a_int = 5;
		add_menu(tmpwin, NO_GLYPH, &any , 'r', 0, ATR_NONE,
				"Armor-works", MENU_UNSELECTED);
	}
  
  	/* Charging: / ( = */
	if ((ESHK(shkp)->services & (SHK_SPECIAL_A|SHK_SPECIAL_B)) &&
			((shk_class_match(WAND_CLASS, shkp) == SHK_MATCH) ||
			(shk_class_match(TOOL_CLASS, shkp) == SHK_MATCH) ||
			(shk_class_match(SPBOOK_CLASS, shkp) == SHK_MATCH) ||
			(shk_class_match(IMPLANT_CLASS, shkp) == SHK_MATCH) ||
			(shk_class_match(RING_CLASS, shkp) == SHK_MATCH))) {
		any.a_int = 6;
		add_menu(tmpwin, NO_GLYPH, &any , 'c', 0, ATR_NONE,
				"Charge", MENU_UNSELECTED);
	}

	any.a_int = 7;
	if (ESHK(shkp)->services & (SHK_CREDITSRV))
	     add_menu(tmpwin, NO_GLYPH, &any , 'e', 0, ATR_NONE,
	         "Establish Credit", MENU_UNSELECTED);

	any.a_int = 8;
	if (ESHK(shkp)->services & (SHK_BLESS))
	     add_menu(tmpwin, NO_GLYPH, &any , 'b', 0, ATR_NONE,
	         "Blessing", MENU_UNSELECTED);

	end_menu(tmpwin, "Services Available:");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	destroy_nhwindow(tmpwin);

	if (n > 0)
	    switch (selected[0].item.a_int) {
	        case 1:
	                shk_identify(slang, shkp);
	                break;

	        case 2:
	                shk_uncurse(slang, shkp);
	                break;

	        case 3:
	                shk_appraisal(slang, shkp);
	                break;

	        case 4:
	                shk_weapon_works(slang, shkp);
	                break;

	        case 5:
	                shk_armor_works(slang, shkp);
	                break;

	        case 6:
	                shk_charge(slang, shkp);
	                break;
	        case 7:
	                shk_estcredit(slang, shkp);
	                break;
	        case 8:
	                shk_bless(slang, shkp);
	                break;

	        default:
	                pline ("Unknown Service");
	                break;
	    }
}

#endif /*OVL0*/
#ifdef OVL3

/* return 2 if used-up portion paid */
/*	  1 if paid successfully    */
/*	  0 if not enough money     */
/*	 -1 if skip this object     */
/*	 -2 if no money/credit left */
STATIC_OVL int
dopayobj(shkp, bp, obj_p, which, itemize)
register struct monst *shkp;
register struct bill_x *bp;
struct obj **obj_p;
int	which;		/* 0 => used-up item, 1 => other (unpaid or lost) */
boolean itemize;
{
	register struct obj *obj = *obj_p;
	long ltmp, quan, save_quan;
#ifdef GOLDOBJ
	long umoney = money_cnt(invent);
#endif
	int buy;
	boolean stashed_gold = (hidden_gold() > 0L),
		consumed = (which == 0);

	if(!obj->unpaid && !bp->useup){
		impossible("Paid object on bill??");
		return PAY_BUY;
	}
#ifndef GOLDOBJ
	if(itemize && u.ugold + ESHK(shkp)->credit == 0L){
#else
	if(itemize && umoney + ESHK(shkp)->credit == 0L){
#endif
		You("%shave no money or credit left.",
			     stashed_gold ? "seem to " : "");
		return PAY_BROKE;
	}
	/* we may need to temporarily adjust the object, if part of the
	   original quantity has been used up but part remains unpaid  */
	save_quan = obj->quan;
	if (consumed) {
	    /* either completely used up (simple), or split needed */
	    quan = bp->bquan;
	    if (quan > obj->quan)	/* difference is amount used up */
		quan -= obj->quan;
	} else {
	    /* dealing with ordinary unpaid item */
	    quan = obj->quan;
	}
	obj->quan = quan;	/* to be used by doname() */
	obj->unpaid = 0;	/* ditto */
	ltmp = bp->price * quan;
	buy = PAY_BUY;		/* flag; if changed then return early */

	if (itemize) {
	    char qbuf[BUFSZ];
	    sprintf(qbuf,"%s for %ld %s.  Pay?", quan == 1L ?
		    Doname2(obj) : doname(obj), ltmp, currency(ltmp));
	    if (yn(qbuf) == 'n') {
		buy = PAY_SKIP;		/* don't want to buy */
	    } else if (quan < bp->bquan && !consumed) { /* partly used goods */
		obj->quan = bp->bquan - save_quan;	/* used up amount */
		verbalize("%s for the other %s before buying %s.",
			  ANGRY(shkp) ? "Pay" : "Please pay", xname(obj),
			  save_quan > 1L ? "these" : "this one");
		buy = PAY_SKIP;		/* shk won't sell */
	    }
	}
#ifndef GOLDOBJ
	if (buy == PAY_BUY && u.ugold + ESHK(shkp)->credit < ltmp) {
	    You("don't%s have gold%s enough to pay for %s.",
#else
	if (buy == PAY_BUY && umoney + ESHK(shkp)->credit < ltmp) {
	    You("don't%s have money%s enough to pay for %s.",
#endif
		stashed_gold ? " seem to" : "",
		(ESHK(shkp)->credit > 0L) ? " or credit" : "",
		doname(obj));
	    buy = itemize ? PAY_SKIP : PAY_CANT;
	}

	if (buy != PAY_BUY) {
	    /* restore unpaid object to original state */
	    obj->quan = save_quan;
	    obj->unpaid = 1;
	    return buy;
	}

	pay(ltmp, shkp);
	shk_names_obj(shkp, obj, consumed ?
			"paid for %s at a cost of %ld gold piece%s.%s" :
			"bought %s for %ld gold piece%s.%s", ltmp, "");
	obj->quan = save_quan;		/* restore original count */
	/* quan => amount just bought, save_quan => remaining unpaid count */
	if (consumed) {
	    if (quan != bp->bquan) {
		/* eliminate used-up portion; remainder is still unpaid */
		bp->bquan = obj->quan;
		obj->unpaid = 1;
		bp->useup = 0;
		buy = PAY_SOME;
	    } else {	/* completely used-up, so get rid of it */
		obj_extract_self(obj);
	     /* assert( obj == *obj_p ); */
		dealloc_obj(obj);
		*obj_p = 0;	/* destroy pointer to freed object */
	    }
	} else if (itemize)
	    update_inventory();	/* Done just once in dopay() if !itemize. */
	return buy;
}
#endif /*OVL3*/
#ifdef OVLB

static coord repo_location;	/* repossession context */

/* routine called after dying (or quitting) */
boolean
paybill(croaked)
int croaked;	/* -1: escaped dungeon; 0: quit; 1: died */
{
	register struct monst *mtmp, *mtmp2, *resident= (struct monst *)0;
	register boolean taken = FALSE;
	register int numsk = 0;

	/* if we escaped from the dungeon, shopkeepers can't reach us;
	   shops don't occur on level 1, but this could happen if hero
	   level teleports out of the dungeon and manages not to die */
	if (croaked < 0) return FALSE;

	/* this is where inventory will end up if any shk takes it */
	repo_location.x = repo_location.y = 0;

	/* give shopkeeper first crack */
	if ((mtmp = shop_keeper(*u.ushops)) && inhishop(mtmp)) {
	    numsk++;
	    resident = mtmp;
	    taken = inherits(resident, numsk, croaked);
	}
	for (mtmp = next_shkp(fmon, FALSE);
		mtmp; mtmp = next_shkp(mtmp2, FALSE)) {
	    mtmp2 = mtmp->nmon;
	    if (mtmp != resident) {
		/* for bones: we don't want a shopless shk around */
		if(!on_level(&(ESHK(mtmp)->shoplevel), &u.uz))
			mongone(mtmp);
		else {
		    numsk++;
		    taken |= inherits(mtmp, numsk, croaked);
		    ESHK(mtmp)->pbanned = FALSE; /* Un-ban for bones levels */
		}
	    }
	}
	if(numsk == 0) return(FALSE);
	return(taken);
}

STATIC_OVL boolean
inherits(shkp, numsk, croaked)
struct monst *shkp;
int numsk;
int croaked;
{
	long loss = 0L;
#ifdef GOLDOBJ
	long umoney;
#endif
	struct eshk *eshkp = ESHK(shkp);
	boolean take = FALSE, taken = FALSE;
	int roomno = *u.ushops;
	char takes[BUFSZ];

	/* the simplifying principle is that first-come */
	/* already took everything you had.		*/
	if (numsk > 1) {
	    if (cansee(shkp->mx, shkp->my) && croaked)
		pline("%s %slooks at your corpse%s and %s.",
		      Monnam(shkp),
		      (!shkp->mcanmove || shkp->msleeping) ? "wakes up, " : "",
		      !rn2(2) ? (shkp->female ? ", shakes her head," :
			   ", shakes his head,") : "",
		      !inhishop(shkp) ? "disappears" : "sighs");
	    rouse_shk(shkp, FALSE);	/* wake shk for bones */    
	    taken = (roomno == eshkp->shoproom);
	    goto skip;
	}

	/* get one case out of the way: you die in the shop, the */
	/* shopkeeper is peaceful, nothing stolen, nothing owed. */
	if(roomno == eshkp->shoproom && inhishop(shkp) &&
	    !eshkp->billct && !eshkp->robbed && !eshkp->debit &&
	     NOTANGRY(shkp) && !eshkp->following) {
		if (invent)
			pline("%s gratefully inherits all your possessions.",
				shkname(shkp));
		set_repo_loc(eshkp);
		goto clear;
	}

	if (eshkp->billct || eshkp->debit || eshkp->robbed) {
		if(roomno == eshkp->shoproom && inhishop(shkp))
		    loss = addupbill(shkp) + eshkp->debit;
		if (loss < eshkp->robbed) loss = eshkp->robbed;
		take = TRUE;
	}

	if (eshkp->following || ANGRY(shkp) || take) {
#ifndef GOLDOBJ
		if (!invent && !u.ugold) goto skip;
#else
		if (!invent) goto skip;
                umoney = money_cnt(invent);
#endif
		takes[0] = '\0';
		if (!shkp->mcanmove || shkp->msleeping)
			strcat(takes, "wakes up and ");
		if (distu(shkp->mx, shkp->my) > 2)
			strcat(takes, "comes and ");
		strcat(takes, "takes");

#ifndef GOLDOBJ
		if (loss > u.ugold || !loss || roomno == eshkp->shoproom) {
			eshkp->robbed -= u.ugold;
			if (eshkp->robbed < 0L) eshkp->robbed = 0L;
			if (rn2(2)) shkp->mgold += u.ugold;
			u.ugold = 0L;
#else
		if (loss > umoney || !loss || roomno == eshkp->shoproom) {
			eshkp->robbed -= umoney;
			if (eshkp->robbed < 0L) eshkp->robbed = 0L;
			  if (umoney > 0) money2mon(shkp, umoney);
#endif
			flags.botl = 1;
			pline("%s %s all your possessions.",
			      shkname(shkp), takes);
			taken = TRUE;
			/* where to put player's invent (after disclosure) */
			set_repo_loc(eshkp);
		} else {
#ifndef GOLDOBJ
			if (rn2(2) || loss < 0) shkp->mgold += loss;
			u.ugold -= loss;
#else
                        money2mon(shkp, loss);
#endif
			flags.botl = 1;
			pline("%s %s the %ld %s %sowed %s.",
			      Monnam(shkp), takes,
			      loss, currency(loss),
			      strncmp(eshkp->customer, plname, PL_NSIZ) ?
					"" : "you ",
			      shkp->female ? "her" : "him");
			/* shopkeeper has now been paid in full */
			pacify_shk(shkp);
			eshkp->following = 0;
			eshkp->robbed = 0L;
		}
skip:
		/* in case we create bones */
		rouse_shk(shkp, FALSE);	/* wake up */
		if(!inhishop(shkp))
			home_shk(shkp, FALSE);
	}
clear:
	setpaid(shkp);
	return(taken);
}

STATIC_OVL void
set_repo_loc(eshkp)
struct eshk *eshkp;
{
	register xchar ox, oy;

	/* if you're not in this shk's shop room, or if you're in its doorway
	    or entry spot, then your gear gets dumped all the way inside */
	if (*u.ushops != eshkp->shoproom ||
		IS_DOOR(levl[u.ux][u.uy].typ) ||
		(u.ux == eshkp->shk.x && u.uy == eshkp->shk.y)) {
	    /* shk.x,shk.y is the position immediately in
	     * front of the door -- move in one more space
	     */
	    ox = eshkp->shk.x;
	    oy = eshkp->shk.y;
	    ox += sgn(ox - eshkp->shd.x);
	    oy += sgn(oy - eshkp->shd.y);
	} else {		/* already inside this shk's shop */
	    ox = u.ux;
	    oy = u.uy;
	}
	/* finish_paybill will deposit invent here */
	repo_location.x = ox;
	repo_location.y = oy;
}

/* called at game exit, after inventory disclosure but before making bones */
void
finish_paybill()
{
	register struct obj *otmp;
	int ox = repo_location.x,
	    oy = repo_location.y;

#if 0		/* don't bother */
	if (ox == 0 && oy == 0) impossible("finish_paybill: no location");
#endif
	/* normally done by savebones(), but that's too late in this case */
	unleash_all();
	/* transfer all of the character's inventory to the shop floor */
	while ((otmp = invent) != 0) {
	    otmp->owornmask = 0L;	/* perhaps we should call setnotworn? */
	    otmp->lamplit = 0;		/* avoid "goes out" msg from freeinv */
	    if (rn2(5)) curse(otmp);	/* normal bones treatment for invent */
	    obj_extract_self(otmp);
	    place_object(otmp, ox, oy);
	}
}

/* find obj on one of the lists */
STATIC_OVL struct obj *
bp_to_obj(bp)
register struct bill_x *bp;
{
	register struct obj *obj;
	register unsigned int id = bp->bo_id;

	if(bp->useup)
		obj = o_on(id, billobjs);
	else
		obj = find_oid(id);
	return obj;
}

/*
 * Look for o_id on all lists but billobj.  Return obj or NULL if not found.
 * Its OK for restore_timers() to call this function, there should not
 * be any timeouts on the billobjs chain.
 */
struct obj *
find_oid(id)
unsigned id;
{
	struct obj *obj;
	struct monst *mon, *mmtmp[3];
	int i;

	/* first check various obj lists directly */
	if ((obj = o_on(id, invent)) != 0) return obj;
	if ((obj = o_on(id, fobj)) != 0) return obj;
	if ((obj = o_on(id, level.buriedobjlist)) != 0) return obj;
	if ((obj = o_on(id, migrating_objs)) != 0) return obj;

	/* not found yet; check inventory for members of various monst lists */
	mmtmp[0] = fmon;
	mmtmp[1] = migrating_mons;
	mmtmp[2] = mydogs;		/* for use during level changes */
	for (i = 0; i < 3; i++)
	    for (mon = mmtmp[i]; mon; mon = mon->nmon)
		if ((obj = o_on(id, mon->minvent)) != 0) return obj;

	/* not found at all */
	return (struct obj *)0;
}
#endif /*OVLB*/
#ifdef OVL3

/* calculate the value that the shk will charge for [one of] an object */
STATIC_OVL long
get_cost(obj, shkp)
register struct obj *obj;
register struct monst *shkp;	/* if angry, impose a surcharge */
{
	register long tmp = getprice(obj, FALSE);

	if (!tmp) tmp = 5L;
	/* shopkeeper may notice if the player isn't very knowledgeable -
	   especially when gem prices are concerned */
	if (!obj->dknown || !objects[obj->otyp].oc_name_known) {
		if (obj->oclass == GEM_CLASS &&
			objects[obj->otyp].oc_material == MT_GLASS) {
		    int i;
		    /* get a value that's 'random' from game to game, but the
		       same within the same game */
		    boolean pseudorand =
			(((int)u.ubirthday % obj->otyp) >= obj->otyp/2);

		    /* all gems are priced high - real or not */
		    switch(obj->otyp - LAST_GEM) {
			case 1: /* white */
			    i = pseudorand ? DIAMOND : OPAL;
			    break;
			case 2: /* blue */
			    i = pseudorand ? SAPPHIRE : AQUAMARINE;
			    break;
			case 3: /* red */
			    i = pseudorand ? RUBY : JASPER;
			    break;
			case 4: /* yellowish brown */
			    i = pseudorand ? AMBER : TOPAZ;
			    break;
			case 5: /* orange */
			    i = pseudorand ? JACINTH : AGATE;
			    break;
			case 6: /* yellow */
			    i = pseudorand ? CITRINE : CHRYSOBERYL;
			    break;
			case 7: /* black */
			    i = pseudorand ? BLACK_OPAL : JET;
			    break;
			case 8: /* pink */
			    i = pseudorand ? ROSE_QUARTZ : MORGANITE ;
			    break;
			case 9: /* cyan */
			    i = pseudorand ? AMAZONITE : VIVIANITE ;
			    break;
			case 10: /* teal */
			    i = pseudorand ? CYAN_STONE : DISTHENE ;
			    break;
			case 11: /* green */
			    i = pseudorand ? EMERALD : JADE;
			    break;
			case 12: /* radiant */
			    i = pseudorand ? DIOPTASE : ANDALUSITE ;
			    break;
			case 13: /* violet */
			    i = pseudorand ? AMETHYST : FLUORITE;
			    break;
			case 14: /* dark blue */
			    i = pseudorand ? IOLITE : LARIMAR;
			    break;
			default: impossible("bad glass gem %ld?", obj->otyp);
			    i = /*STRANGE_OBJECT*/MORGANITE; /* fail safe - in case of doubt have it be something expensive --Amy */
			    break;
		    }
		    tmp = (long) objects[i].oc_cost;
		} else if (!(obj->o_id % 4)) /* arbitrarily impose surcharge */
		    tmp += tmp / 3L;
	}
	if ((Role_if(PM_TOURIST) && u.ulevel < (MAXULEV/2))
	    || ((uarmu && !uarmu->oinvis) && (!uarm || uarm->oinvis) 
	    && (!uarmc || uarmc->oinvis)))	/* touristy shirt visible */
		tmp += tmp / 3L;
	else
	if (uarmh && !uarmh->oinvis && uarmh->otyp == DUNCE_CAP)
		tmp += tmp / 3L;

	if (Race_if(PM_DUTHOL)) tmp *= 2L;
	if (Role_if(PM_OTAKU)) tmp += tmp / 3L; /* bad at making deals */

	if (ACURR(A_CHA) > 18)		tmp /= 2L;
	else if (ACURR(A_CHA) > 17)	tmp -= tmp / 3L;
	else if (ACURR(A_CHA) > 15)	tmp -= tmp / 4L;
	else if (ACURR(A_CHA) < 6)	tmp *= 2L;
	else if (ACURR(A_CHA) < 8)	tmp += tmp / 2L;
	else if (ACURR(A_CHA) < 11)	tmp += tmp / 3L;
	if (tmp <= 0L) tmp = 1L;
	else if (obj->oartifact) tmp *= 4L;

	/* character classes who are discriminated against... */
	/* barbarians are gullible... */
	if (Role_if(PM_BARBARIAN)) tmp *= 3L;
	if (Role_if(PM_NOOB_MODE_BARB)) tmp *= 5L;
	/* rogues are untrustworthy... */
	if (Role_if(PM_ROGUE)) tmp *= 2L;
	/* samurais are from out of town... */
	if (Role_if(PM_SAMURAI)) tmp *= 2L;
	if (uarmf && uarmf->oartifact == ART_CARMARK) tmp *= 2L;

	/* anger surcharge should match rile_shk's */
	if (shkp && ESHK(shkp)->surcharge) tmp += (tmp + 2L) / 3L;

	/* KMH, balance patch -- healthstone replaces rotting/health */
	if (Is_blackmarket(&u.uz)) {
	  if (obj->oclass==RING_CLASS    || obj->oclass==AMULET_CLASS || obj->oclass == IMPLANT_CLASS   ||
	      obj->oclass==POTION_CLASS  || obj->oclass==SCROLL_CLASS   ||
	      obj->oclass==SPBOOK_CLASS  || obj->oclass==WAND_CLASS     ||
	      obj->otyp==LUCKSTONE       || obj->otyp==LOADSTONE        || 
	      obj->otyp==HEALTHSTONE || objects[obj->otyp].oc_magic) {
	    tmp *= (Role_if(PM_CONVICT) ? 10 : 50);
	  } else {
	    tmp *= (Role_if(PM_CONVICT) ? 5 : 25);
	  }
	}

	return tmp;
}
#endif /*OVL3*/
#ifdef OVLB

/* returns the price of a container's content.  the price
 * of the "top" container is added in the calling functions.
 * a different price quoted for selling as vs. buying.
 */
long
contained_cost(obj, shkp, price, usell, unpaid_only)
register struct obj *obj;
register struct monst *shkp;
long price;
register boolean usell;
register boolean unpaid_only;
{
	register struct obj *otmp;

	/* the price of contained objects */
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj) {
	    if (otmp->oclass == COIN_CLASS) continue;
	    /* the "top" container is evaluated by caller */
	    if (usell) {
		if (saleable(shkp, otmp) &&
			!otmp->unpaid && otmp->oclass != BALL_CLASS &&
			!is_hazy(otmp) &&
			!(otmp->oclass == FOOD_CLASS && otmp->oeaten) )
		    price += set_cost(otmp, shkp);
	    } else if (!otmp->no_charge &&
		      (!unpaid_only || (unpaid_only && otmp->unpaid))) {
		    price += get_cost(otmp, shkp) * otmp->quan;
	    }

	    if (Has_contents(otmp))
		    price += contained_cost(otmp, shkp, price, usell, unpaid_only);
	}

	return(price);
}

long
contained_gold(obj)
register struct obj *obj;
{
	register struct obj *otmp;
	register long value = 0L;

	/* accumulate contained gold */
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
	    if (otmp->oclass == COIN_CLASS)
		value += otmp->quan;
	    else if (Has_contents(otmp))
		value += contained_gold(otmp);

	return(value);
}

STATIC_OVL void
dropped_container(obj, shkp, sale)
register struct obj *obj;
register struct monst *shkp;
register boolean sale;
{
	register struct obj *otmp;

	/* the "top" container is treated in the calling fn */
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj) {
	    if (otmp->oclass == COIN_CLASS) continue;

	    if (!otmp->unpaid && !(sale && saleable(shkp, otmp)))
		otmp->no_charge = 1;

	    if (Has_contents(otmp))
		dropped_container(otmp, shkp, sale);
	}
}

void
picked_container(obj)
register struct obj *obj;
{
	register struct obj *otmp;

	/* the "top" container is treated in the calling fn */
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj) {
	    if (otmp->oclass == COIN_CLASS) continue;

	    if (otmp->no_charge)
		otmp->no_charge = 0;

	    if (Has_contents(otmp))
		picked_container(otmp);
	}
}
#endif /*OVLB*/
#ifdef OVL3

/* calculate how much the shk will pay when buying [all of] an object */
STATIC_OVL long
set_cost(obj, shkp)
register struct obj *obj;
register struct monst *shkp;
{
	long tmp = getprice(obj, TRUE) * obj->quan;

	if ((Role_if(PM_TOURIST) && u.ulevel < (MAXULEV/2))
	    || ((uarmu && !uarmu->oinvis) && (!uarm || uarm->oinvis) 
	    && (!uarmc || uarmc->oinvis)))	/* touristy shirt visible */
		tmp /= 3L;
	else
	if (uarmh && !uarmh->oinvis && uarmh->otyp == DUNCE_CAP)
		tmp /= 3L;
	else
		tmp /= 2L;

	if (Role_if(PM_OTAKU)) tmp /= 3L; /* bad at making deals */
	if (Race_if(PM_DUTHOL)) tmp /= 2L;

	/* shopkeeper may notice if the player isn't very knowledgeable -
	   especially when gem prices are concerned */
	if (!obj->dknown || !objects[obj->otyp].oc_name_known) {
		if (obj->oclass == GEM_CLASS) {
			/* different shop keepers give different prices */
			if (objects[obj->otyp].oc_material == MT_GEMSTONE ||
			    objects[obj->otyp].oc_material == MT_GLASS) {
				tmp = (obj->otyp % (6 - shkp->m_id % 3));
				tmp = (tmp + 3) * obj->quan;
			}
		} else if (tmp > 1L && !rn2(4))
			tmp -= tmp / 4L;
	}
	/* reduce player's ability to gain tons of money from selling common items --Amy */
	if (tmp > 1) {
	 	if (obj->otyp == IC && obj->cursed) tmp /= 20L;
		if (obj->otyp == IC && !obj->cursed) tmp /= 10L;
	 	if (obj->otyp == SPOON) tmp /= 100L; /* base price 5000??? ridiculous */
		if (objects[obj->otyp].oc_skill == P_FIREARM) tmp /= 10L;
		if (objects[obj->otyp].oc_skill == -P_FIREARM) tmp /= 3L;
		if (tmp < 1) tmp = 1; /* fail safe */
		if (obj->oclass == RING_CLASS) tmp /= 10L;
		if (obj->oclass == AMULET_CLASS) tmp /= 10L;
		if (obj->oclass == IMPLANT_CLASS) tmp /= 10L;
		if (obj->oclass == POTION_CLASS) tmp /= 10L;
		if (obj->oclass == SCROLL_CLASS) tmp /= 10L;
		if (obj->oclass == SPBOOK_CLASS) tmp /= 10L;
		if (obj->oclass == WAND_CLASS) tmp /= 10L;
	 	if (obj->otyp >= LUCKSTONE && obj->otyp <= SLING_AMMO) tmp /= 10L;
	}
	/* after all, we nuked the thing that should not exist (price id) by making many item types always have the price
	 * of the most expensive item in that type; if you can sell common scrolls, potions etc. for that price, it's way
	 * too easy to gain money or (worst of all) credit clone! */

	return tmp;
}

#endif /*OVL3*/
#ifdef OVLB

/* called from doinv(invent.c) for inventory of unpaid objects */
long
unpaid_cost(unp_obj)
register struct obj *unp_obj;	/* known to be unpaid */
{
	register struct bill_x *bp = (struct bill_x *)0;
	register struct monst *shkp;

	for(shkp = next_shkp(fmon, TRUE); shkp;
					shkp = next_shkp(shkp->nmon, TRUE))
	    if ((bp = onbill(unp_obj, shkp, TRUE)) != 0) break;

	/* onbill() gave no message if unexpected problem occurred */
	if(!bp) {
		impossible("unpaid_cost: object wasn't on any bill!");
		unp_obj->unpaid = 0;
	}

	return bp ? unp_obj->quan * bp->price : 0L;
}

STATIC_OVL void
add_one_tobill(obj, dummy)
register struct obj *obj;
register boolean dummy;
{
	register struct monst *shkp;
	register struct bill_x *bp;
	register int bct;
	register char roomno = *u.ushops;

	if (!roomno) return;
	if (!(shkp = shop_keeper(roomno))) return;
	if (!inhishop(shkp)) return;

	if (onbill(obj, shkp, FALSE) || /* perhaps thrown away earlier */
		    (obj->oclass == FOOD_CLASS && obj->oeaten))
		return;

	if (ESHK(shkp)->billct == BILLSZ) {
		You("got that for free!");
		if (!rn2(5) && shkp->mpeaceful) {
			verbalize("That's it, thief! I'm calling the kops!");
			call_kops(shkp, FALSE);
			hot_pursuit(shkp);
		}
		return;
	}

	/* To recognize objects the shopkeeper is not interested in. -dgk
	 */
	if (obj->no_charge) {
		obj->no_charge = 0;
		return;
	}

	bct = ESHK(shkp)->billct;
	bp = &(ESHK(shkp)->bill_p[bct]);
	if (!bp) {
		impossible("add_one_tobill error?!");
		return;
	}
	bp->bo_id = obj->o_id;
	bp->bquan = obj->quan;
	if(dummy) {		  /* a dummy object must be inserted into  */
	    bp->useup = 1;	  /* the billobjs chain here.  crucial for */
	    add_to_billobjs(obj); /* eating floorfood in shop.  see eat.c  */
	} else	bp->useup = 0;
	bp->price = get_cost(obj, shkp);
	ESHK(shkp)->billct++;
	obj->unpaid = 1;
}

STATIC_OVL void
add_to_billobjs(obj)
    struct obj *obj;
{
    if (obj->where != OBJ_FREE)
	panic("add_to_billobjs: obj not free");
    if (obj->timed)
	obj_stop_timers(obj);

    obj->nobj = billobjs;
    billobjs = obj;
    obj->where = OBJ_ONBILL;
}

/* recursive billing of objects within containers. */
STATIC_OVL void
bill_box_content(obj, ininv, dummy, shkp)
register struct obj *obj;
register boolean ininv, dummy;
register struct monst *shkp;
{
	register struct obj *otmp;

	for (otmp = obj->cobj; otmp; otmp = otmp->nobj) {
		if (otmp->oclass == COIN_CLASS) continue;

		/* the "top" box is added in addtobill() */
		if (!otmp->no_charge)
		    add_one_tobill(otmp, dummy);
		if (Has_contents(otmp))
		    bill_box_content(otmp, ininv, dummy, shkp);
	}

}

/* shopkeeper tells you what you bought or sold, sometimes partly IDing it */
STATIC_OVL void
shk_names_obj(shkp, obj, fmt, amt, arg)
struct monst *shkp;
struct obj *obj;
const char *fmt;	/* "%s %ld %s %s", doname(obj), amt, plur(amt), arg */
long amt;
const char *arg;
{
	char *obj_name, fmtbuf[BUFSZ];
	boolean was_unknown = !obj->dknown;

	obj->objwassold = TRUE; /* so you can't sell it again for credit cloning --Amy */
	obj->dknown = TRUE;
	/* Use real name for ordinary weapons/armor, and spell-less
	 * scrolls/books (that is, blank and mail), but only if the
	 * object is within the shk's area of interest/expertise.
	 */
	if (!objects[obj->otyp].oc_magic && saleable(shkp, obj) &&
	    (obj->oclass == WEAPON_CLASS || obj->oclass == ARMOR_CLASS ||
	     obj->oclass == SCROLL_CLASS || obj->oclass == SPBOOK_CLASS ||
	     obj->otyp == MIRROR)) {
	    was_unknown |= !objects[obj->otyp].oc_name_known;
	    makeknown(obj->otyp);
	}
	obj_name = doname(obj);
	/* Use an alternate message when extra information is being provided */
	if (was_unknown) {
	    sprintf(fmtbuf, "%%s; you %s", fmt);
	    obj_name[0] = highc(obj_name[0]);
	    pline(fmtbuf, obj_name, (obj->quan > 1) ? "them" : "it",
		  amt, plur(amt), arg);
	} else {
	    You(fmt, obj_name, amt, plur(amt), arg);
	}
}

void
addtobill(obj, ininv, dummy, silent)
register struct obj *obj;
register boolean ininv, dummy, silent;
{
	register struct monst *shkp;
	register char roomno = *u.ushops;
	long ltmp = 0L, cltmp = 0L, gltmp = 0L;
	register boolean container = Has_contents(obj);

	if(!*u.ushops) return;

	if(!(shkp = shop_keeper(roomno))) return;

	if(!inhishop(shkp)) return;

	if(/* perhaps we threw it away earlier */
		 onbill(obj, shkp, FALSE) ||
		 (obj->oclass == FOOD_CLASS && obj->oeaten)
	      ) return;

	if (Race_if(PM_IRAHA)) {
		verbalize("You just signed your own death warrant, thief!");
		hot_pursuit(shkp);
	}

	if(ESHK(shkp)->billct == BILLSZ) {
		You("got that for free!");
		if (!rn2(5) && shkp->mpeaceful) {
			verbalize("That's it, thief! I'm calling the kops!");
			call_kops(shkp, FALSE);
			hot_pursuit(shkp);
		}
		return;
	}

	if(obj->oclass == COIN_CLASS) {
		costly_gold(obj->ox, obj->oy, obj->quan);
		return;
	}

	if(!obj->no_charge)
	    ltmp = get_cost(obj, shkp);

	if (obj->no_charge && !container) {
		obj->no_charge = 0;
		return;
	}

	if(container) {
	    if(obj->cobj == (struct obj *)0) {
		if(obj->no_charge) {
		    obj->no_charge = 0;
		    return;
		} else {
		    add_one_tobill(obj, dummy);
		    goto speak;
		}
	    } else {
		cltmp += contained_cost(obj, shkp, cltmp, FALSE, FALSE);
		gltmp += contained_gold(obj);
	    }

	    if(ltmp) add_one_tobill(obj, dummy);
	    if(cltmp) bill_box_content(obj, ininv, dummy, shkp);
	    picked_container(obj); /* reset contained obj->no_charge */

	    ltmp += cltmp;

	    if(gltmp) {
		costly_gold(obj->ox, obj->oy, gltmp);
		if(!ltmp) return;
	    }

	    if(obj->no_charge)
	    {
		obj->no_charge = 0;
		if(!ltmp) return;
	    }

	} else /* i.e., !container */
	    add_one_tobill(obj, dummy);
speak:
	if (shkp->mcanmove && !shkp->msleeping && !silent) {
	    char buf[BUFSZ];

	    if(!ltmp) {
		pline("%s has no interest in %s.", Monnam(shkp),
					     the(xname(obj)));
		return;
	    }
	    strcpy(buf, "\"For you, ");
	    if (ANGRY(shkp)) strcat(buf, "scum ");
	    else if (Role_if(PM_CONVICT) || Role_if(PM_MURDERER) || Race_if(PM_ALBAE) || Race_if(PM_PLAYER_DYNAMO) ) strcat(buf, "criminal ");
	    else {
		static const char *honored[5] = {
		  "good", "honored", "most gracious", "esteemed",
		  "most renowned and sacred"
		};
		strcat(buf, honored[rn2(4) + u.uevent.udemigod]);
		if (!is_human(youmonst.data)) strcat(buf, " creature");
		else
		    strcat(buf, (flags.female) ? " lady" : " sir");
	    }
	    if(ininv) {
		long quan = obj->quan;
		obj->quan = 1L; /* fool xname() into giving singular */
		pline("%s; only %ld %s %s.\"", buf, ltmp,
			(quan > 1L) ? "per" : "for this", xname(obj));
		obj->quan = quan;
	    } else
		pline("%s will cost you %ld %s%s.",
			The(xname(obj)), ltmp, currency(ltmp),
			(obj->quan > 1L) ? " each" : "");
	} else if(!silent) {
	    if(ltmp) pline_The("list price of %s is %ld %s%s.",
				   the(xname(obj)), ltmp, currency(ltmp),
				   (obj->quan > 1L) ? " each" : "");
	    else pline("%s does not notice.", Monnam(shkp));
	}
}

void
splitbill(obj, otmp)
register struct obj *obj, *otmp;
{
	/* otmp has been split off from obj */
	register struct bill_x *bp;
	register long tmp;
	register struct monst *shkp = shop_keeper(*u.ushops);

	if(!shkp || !inhishop(shkp)) {
		impossible("splitbill: no resident shopkeeper??");
		return;
	}
	bp = onbill(obj, shkp, FALSE);
	if(!bp) {
		impossible("splitbill: not on bill?");
		return;
	}
	if(bp->bquan < otmp->quan) {
		impossible("Negative quantity on bill??");
	}
	if(bp->bquan == otmp->quan) {
		impossible("Zero quantity on bill??");
	}
	bp->bquan -= otmp->quan;
	tmp = bp->price; /* according to amateurhour this needs to be here to prevent a read after free --Amy */

	if(ESHK(shkp)->billct == BILLSZ) otmp->unpaid = 0;
	else {
		bp = &(ESHK(shkp)->bill_p[ESHK(shkp)->billct]);
		bp->bo_id = otmp->o_id;
		bp->bquan = otmp->quan;
		bp->useup = 0;
		bp->price = tmp;
		ESHK(shkp)->billct++;
	}
}

STATIC_OVL void
sub_one_frombill(obj, shkp)
register struct obj *obj;
register struct monst *shkp;
{
	register struct bill_x *bp;

	if((bp = onbill(obj, shkp, FALSE)) != 0) {
		register struct obj *otmp;

		obj->unpaid = 0;
		if(bp->bquan > obj->quan){
			otmp = newobj(0);
			*otmp = *obj;
			bp->bo_id = otmp->o_id = flags.ident++;
			otmp->where = OBJ_FREE;
			otmp->quan = (bp->bquan -= obj->quan);
			otmp->owt = 0;	/* superfluous */
			otmp->onamelth = 0;
			otmp->oxlth = 0;
			otmp->oattached = OATTACHED_NOTHING;
			bp->useup = 1;
			add_to_billobjs(otmp);
			return;
		}
		ESHK(shkp)->billct--;
#ifdef DUMB
		{
		/* DRS/NS 2.2.6 messes up -- Peter Kendell */
			int indx = ESHK(shkp)->billct;
			*bp = ESHK(shkp)->bill_p[indx];
		}
#else
		*bp = ESHK(shkp)->bill_p[ESHK(shkp)->billct];
#endif
		return;
	} else if (obj->unpaid) {
		impossible("sub_one_frombill: unpaid object not on bill");
		obj->unpaid = 0;
	}
}

/* recursive check of unpaid objects within nested containers. */
void
subfrombill(obj, shkp)
register struct obj *obj;
register struct monst *shkp;
{
	register struct obj *otmp;

	sub_one_frombill(obj, shkp);

	if (Has_contents(obj))
	    for(otmp = obj->cobj; otmp; otmp = otmp->nobj) {
		if(otmp->oclass == COIN_CLASS) continue;

		if (Has_contents(otmp))
		    subfrombill(otmp, shkp);
		else
		    sub_one_frombill(otmp, shkp);
	    }
}

#endif /*OVLB*/
#ifdef OVL3

STATIC_OVL long
stolen_container(obj, shkp, price, ininv, destruction)
register struct obj *obj;
register struct monst *shkp;
long price;
register boolean ininv, destruction;
{
	register struct obj *otmp;

	if (!(destruction && evades_destruction(obj))) {
	if(ininv && obj->unpaid)
	    price += get_cost(obj, shkp);
	else {
	    if(!obj->no_charge)
		price += get_cost(obj, shkp);
	    obj->no_charge = 0;
	}
	}

	/* the price of contained objects, if any */
	for(otmp = obj->cobj; otmp; otmp = otmp->nobj) {

	    if(otmp->oclass == COIN_CLASS) continue;

	    if (!Has_contents(otmp)) {
	      if (!(destruction && evades_destruction(otmp))) {
		if(ininv) {
		    if(otmp->unpaid)
			price += otmp->quan * get_cost(otmp, shkp);
		} else {
		    if(!otmp->no_charge) {
			if(otmp->oclass != FOOD_CLASS || !otmp->oeaten)
			    price += otmp->quan * get_cost(otmp, shkp);
		    }
		    otmp->no_charge = 0;
		}
	      }
	    } else
		price += stolen_container(otmp, shkp, price, ininv,
			destruction);
	}

	return(price);
}
#endif /*OVL3*/
#ifdef OVLB

long
stolen_value(obj, x, y, peaceful, silent, destruction)
register struct obj *obj;
register xchar x, y;
register boolean peaceful, silent, destruction;
{
	register long value = 0L, gvalue = 0L;
	register struct monst *shkp = shop_keeper(*in_rooms(x, y, SHOPBASE));

	if (!shkp || !inhishop(shkp))
	    return (0L);

	if(obj->oclass == COIN_CLASS) {
	    gvalue += obj->quan;
	} else if (Has_contents(obj)) {
	    register boolean ininv = !!count_unpaid(obj->cobj);

	    value += stolen_container(obj, shkp, value, ininv, destruction);
	    if(!ininv) gvalue += contained_gold(obj);
	} else if (!obj->no_charge && saleable(shkp, obj) &&
		!(destruction && evades_destruction(obj))) {
	    value += get_cost(obj, shkp);
	}

	if(gvalue + value == 0L) return(0L);

	value += gvalue;

	if(peaceful) {
	    boolean credit_use = !!ESHK(shkp)->credit;
	    value = check_credit(value, shkp);
	    /* 'peaceful' affects general treatment, but doesn't affect
	     * the fact that other code expects that all charges after the
	     * shopkeeper is angry are included in robbed, not debit */
	    if (ANGRY(shkp))
		ESHK(shkp)->robbed += value;
	    else 
		ESHK(shkp)->debit += value;

	    if(!silent) {
		const char *still = "";

		if (credit_use) {
		    if (ESHK(shkp)->credit) {
			You("have %ld %s credit remaining.",
				 ESHK(shkp)->credit, currency(ESHK(shkp)->credit));
			return value;
		    } else if (!value) {
			You("have no credit remaining.");
			return 0;
		    }
		    still = "still ";
		}
		if(obj->oclass == COIN_CLASS)
		    You("%sowe %s %ld %s!", still,
			mon_nam(shkp), value, currency(value));
		else
		    You("%sowe %s %ld %s for %s!", still,
			mon_nam(shkp), value, currency(value),
			obj->quan > 1L ? "them" : "it");
	    }
	} else {
	    ESHK(shkp)->robbed += value;

	    if(!silent) {
		if(cansee(shkp->mx, shkp->my)) {
		    Norep("%s booms: \"%s, you are a thief!\"",
				Monnam(shkp), playeraliasname);
		} else  Norep("You hear a scream, \"Thief!\"");
	    }
	    hot_pursuit(shkp);
	    (void) angry_guards(FALSE);
	}
	return(value);
}

/* auto-response flag for/from "sell foo?" 'a' => 'y', 'q' => 'n' */
static char sell_response = 'a';
static int sell_how = SELL_NORMAL;
/* can't just use sell_response='y' for auto_credit because the 'a' response
   shouldn't carry over from ordinary selling to credit selling */
static boolean auto_credit = FALSE;

void
sellobj_state(deliberate)
int deliberate;
{
	/* If we're deliberately dropping something, there's no automatic
	   response to the shopkeeper's "want to sell" query; however, if we
	   accidentally drop anything, the shk will buy it/them without asking.
	   This retains the old pre-query risk that slippery fingers while in
	   shops entailed:  you drop it, you've lost it.
	 */
	sell_response = (deliberate != SELL_NORMAL) ? '\0' : 'a';
	sell_how = deliberate;
	auto_credit = FALSE;
}

void
sellobj(obj, x, y)
register struct obj *obj;
xchar x, y;
{
	register struct monst *shkp;
	register struct eshk *eshkp;
	long ltmp = 0L, cltmp = 0L, gltmp = 0L, offer;
	boolean saleitem, cgold = FALSE, container = Has_contents(obj);
	boolean isgold = (obj->oclass == COIN_CLASS);
	boolean only_partially_your_contents = FALSE;

	if(!(shkp = shop_keeper(*in_rooms(x, y, SHOPBASE))) ||
	   !inhishop(shkp)) return;
	if(!costly_spot(x, y))	return;
	if(!*u.ushops) return;

	if(obj->unpaid && !container && !isgold) {
	    sub_one_frombill(obj, shkp);
	    return;
	}
	if(container) {
		/* find the price of content before subfrombill */
		cltmp += contained_cost(obj, shkp, cltmp, TRUE, FALSE);
		/* find the value of contained gold */
		gltmp += contained_gold(obj);
		cgold = (gltmp > 0L);
	}

	saleitem = saleable(shkp, obj);
	if(!isgold && !obj->unpaid && saleitem)
	    ltmp = set_cost(obj, shkp);

	offer = ltmp + cltmp;

	/* get one case out of the way: nothing to sell, and no gold */
	if(!isgold &&
	   ((offer + gltmp) == 0L || (obj->objwassold) || (sell_how == SELL_DONTSELL && !(uarmf && uarmf->oartifact == ART_KRISTIN_S_NOBILITY) ) )) {
		register boolean unpaid = (obj->unpaid ||
				  (container && count_unpaid(obj->cobj)));

		if(container) {
			dropped_container(obj, shkp, FALSE);
			if(!obj->unpaid && !saleitem)
			    obj->no_charge = 1;
			if(obj->unpaid || count_unpaid(obj->cobj))
			    subfrombill(obj, shkp);
		} else obj->no_charge = 1;

		if(!unpaid && (sell_how != SELL_DONTSELL)) {
			pline("%s seems uninterested%s.", Monnam(shkp), (obj && obj->objwassold) ? " in items that already got sold earlier" : "");
		}
		return;
	}

	/* you dropped something of your own - probably want to sell it */
	rouse_shk(shkp, TRUE);	/* wake up sleeping or paralyzed shk */
	eshkp = ESHK(shkp);

	if (ANGRY(shkp)) { /* they become shop-objects, no pay */
		pline("Thank you, scum!");
		subfrombill(obj, shkp);
		return;
	}

	if(eshkp->robbed) {  /* shkp is not angry? */
		if(isgold) offer = obj->quan;
		else if(cgold) offer += cgold;
		if((eshkp->robbed -= offer < 0L))
			eshkp->robbed = 0L;
		if(offer) verbalize(
  "Thank you for your contribution to restock this recently plundered shop.");
		subfrombill(obj, shkp);
		return;
	}

	if(isgold || cgold) {
		if(!cgold) gltmp = obj->quan;

		if(eshkp->debit >= gltmp) {
		    if(eshkp->loan) { /* you carry shop's gold */
			 if(eshkp->loan >= gltmp)
			     eshkp->loan -= gltmp;
			 else eshkp->loan = 0L;
		    }
		    eshkp->debit -= gltmp;
		    Your("debt is %spaid off.",
				eshkp->debit ? "partially " : "");
		} else {
		    long delta = gltmp - eshkp->debit;

		    /* credit limit as a nerf for cloning exploits: this exists mainly so you can't get 10000s of credit and enchant everything to +3 in an armor shop --Amy */
		    if (eshkp->totalcredit >= eshkp->creditlimit) {
				verbalize("Sorry. I'm not offering credit anymore because you've already had so much credit in my shop. From now on I can only pay you in cash. Why don't you buy something so I can pay you again?");
		    } else if (rn2(2)) { /* make the shopkeeper devious --Amy */
				if (eshkp->totalcredit + delta > eshkp->creditlimit) {
					eshkp->totalcredit = eshkp->creditlimit;
					verbalize("You've exceeded your credit limit in this shop. Sorry.");
				} else {
					eshkp->credit += delta;
					eshkp->totalcredit += delta;
				}
		    } else verbalize(isevilvariant ? "I own your stuff now. Fight me." : "What? You want credit? Well, sucks to be you, but I ain't giving you any!");
		    if(eshkp->debit) {
			eshkp->debit = 0L;
			eshkp->loan = 0L;
			Your("debt is paid off.");
		    }
		    if (eshkp->credit) pline("%ld %s %s added to your credit.",
				delta, currency(delta), delta > 1L ? "are" : "is");
		}
		if(offer) goto move_on;
		else {
		    if(!isgold) {
			if (container)
			    dropped_container(obj, shkp, FALSE);
			if (!obj->unpaid && !saleitem) obj->no_charge = 1;
			subfrombill(obj, shkp);
		    }
		    return;
		}
	}
move_on:
	if((!saleitem && !(container && cltmp > 0L))
	   || eshkp->billct == BILLSZ
	   || obj->oclass == BALL_CLASS
	   || obj->objwassold
	   || obj->oclass == CHAIN_CLASS || offer == 0L
	   || is_hazy(obj)
	   || (obj->oclass == FOOD_CLASS && obj->oeaten) ) {
		pline("%s seems uninterested%s.", Monnam(shkp),
			cgold ? " in the rest" : "");
		if (container)
		    dropped_container(obj, shkp, FALSE);
		obj->no_charge = 1;
		return;
	}
        
#ifndef GOLDOBJ
	if(!shkp->mgold) {
#else
	if(!money_cnt(shkp->minvent)) {
#endif
		char c, qbuf[BUFSZ];
		long tmpcr = ((offer * 9L) / 10L) + (offer <= 1L);
		if (!issoviet && tmpcr > 0) {
		/* if the shk has no money then he has no money. Giving credit is all fine and dandy, but there's way too much
		 * potential for abuse so from now on credit is one third of the # of zorkmids the shk would have paid. --Amy */
			tmpcr /= 3;
			if (tmpcr < 1) tmpcr = 1;
		}

		if (sell_how == SELL_NORMAL || auto_credit) {
		    c = sell_response = 'y';
		} else if (sell_response != 'n') {
		    pline("%s cannot pay you at present.", Monnam(shkp));
		    sprintf(qbuf,
			    "Will you accept %ld %s in credit for %s?",
			    tmpcr, currency(tmpcr), doname(obj));
		    /* won't accept 'a' response here */
		    /* KLY - 3/2000 yes, we will, it's a damn nuisance
                       to have to constantly hit 'y' to sell for credit */
		    c = ynaq(qbuf);
		    if (c == 'a') {
			c = 'y';
			auto_credit = TRUE;
		    }
		} else		/* previously specified "quit" */
		    c = 'n';

		if (c == 'y') {
		    shk_names_obj(shkp, obj, (sell_how != SELL_NORMAL) ?
			    "traded %s for %ld zorkmid%s in %scredit." :
			"relinquish %s and acquire %ld zorkmid%s in %scredit.",
			    tmpcr, (eshkp->credit > 0L) ? "additional " : "");
		    if (eshkp->totalcredit >= eshkp->creditlimit) {
				verbalize("Sorry. I'm not offering credit anymore because you've already had so much credit in my shop. From now on I can only pay you in cash. Why don't you buy something so I can pay you again?");
		    } else if (rn2(2)) { /* fail sometimes --Amy */

				if (eshkp->totalcredit + tmpcr > eshkp->creditlimit) {
					eshkp->totalcredit = eshkp->creditlimit;
					verbalize("You've exceeded your credit limit in this shop. Sorry.");
				} else {
					eshkp->credit += tmpcr;
					eshkp->totalcredit += tmpcr;
				}

		    } else verbalize(isevilvariant ? "I own your stuff now. Fight me." : "What? You want credit? Well, sucks to be you, but I ain't giving you any!");
		    subfrombill(obj, shkp);
		} else {
		    if (c == 'q') sell_response = 'n';
		    if (container)
			dropped_container(obj, shkp, FALSE);
		    if (!obj->unpaid) obj->no_charge = 1;
		    subfrombill(obj, shkp);
		}
	} else {
		char qbuf[BUFSZ];
#ifndef GOLDOBJ
		boolean short_funds = (offer > shkp->mgold);
		if (short_funds) offer = shkp->mgold;
#else
                long shkmoney = money_cnt(shkp->minvent);
		boolean short_funds = (offer > shkmoney);
		if (short_funds) offer = shkmoney;
#endif
		if (!sell_response) {
		    only_partially_your_contents =
			(contained_cost(obj, shkp, 0L, FALSE, FALSE) !=
			 contained_cost(obj, shkp, 0L, FALSE, TRUE));
		    sprintf(qbuf,
			 "%s offers%s %ld gold piece%s for%s %s %s.  Sell %s?",
			    Monnam(shkp), short_funds ? " only" : "",
			    offer, plur(offer),
			    (!ltmp && cltmp && only_partially_your_contents) ?
			     " your items in" : (!ltmp && cltmp) ? " the contents of" : "",
			    obj->unpaid ? "the" : "your", cxname(obj),
			    (obj->quan == 1L &&
			    !(!ltmp && cltmp && only_partially_your_contents)) ?
			    "it" : "them");
		} else  qbuf[0] = '\0';		/* just to pacify lint */

		switch (sell_response ? sell_response : ynaq(qbuf)) {
		 case 'q':  sell_response = 'n';
		 case 'n':  if (container)
				dropped_container(obj, shkp, FALSE);
			    if (!obj->unpaid) obj->no_charge = 1;
			    subfrombill(obj, shkp);
			    break;
		 case 'a':  sell_response = 'y';
		 case 'y':  if (container)
				dropped_container(obj, shkp, TRUE);
			    if (!obj->unpaid && !saleitem) obj->no_charge = 1;
			    subfrombill(obj, shkp);
			    pay(-offer, shkp);
			    shk_names_obj(shkp, obj, (sell_how != SELL_NORMAL) ?
				    (!ltmp && cltmp && only_partially_your_contents) ?
			    	    "sold some items inside %s for %ld gold pieces%s.%s" :
				    "sold %s for %ld gold piece%s.%s" :
	       "relinquish %s and receive %ld gold piece%s in compensation.%s",
				    offer, "");
			    break;
		 default:   impossible("invalid sell response");
		}
	}
}

int
doinvbill(mode)
int mode;		/* 0: deliver count 1: paged */
{
#ifdef	__SASC
	void sasc_bug(struct obj *, unsigned);
#endif
	struct monst *shkp;
	struct eshk *eshkp;
	struct bill_x *bp, *end_bp;
	struct obj *obj;
	long totused;
	char *buf_p;
	winid datawin;

	shkp = shop_keeper(*u.ushops);
	if (!shkp || !inhishop(shkp)) {
	    if (mode != 0) impossible("doinvbill: no shopkeeper?");
	    return 0;
	}
	eshkp = ESHK(shkp);

	if (mode == 0) {
	    /* count expended items, so that the `I' command can decide
	       whether to include 'x' in its prompt string */
	    int cnt = !eshkp->debit ? 0 : 1;

	    for (bp = eshkp->bill_p, end_bp = &eshkp->bill_p[eshkp->billct];
		    bp < end_bp; bp++)
		if (bp->useup ||
			((obj = bp_to_obj(bp)) != 0 && obj->quan < bp->bquan))
		    cnt++;
	    return cnt;
	}

	datawin = create_nhwindow(NHW_MENU);
	putstr(datawin, 0, "Unpaid articles already used up:");
	putstr(datawin, 0, "");

	totused = 0L;
	for (bp = eshkp->bill_p, end_bp = &eshkp->bill_p[eshkp->billct];
		bp < end_bp; bp++) {
	    obj = bp_to_obj(bp);
	    if(!obj) {
		impossible("Bad shopkeeper administration.");
		goto quit;
	    }
	    if(bp->useup || bp->bquan > obj->quan) {
		long oquan, uquan, thisused;
		unsigned save_unpaid;

		save_unpaid = obj->unpaid;
		oquan = obj->quan;
		uquan = (bp->useup ? bp->bquan : bp->bquan - oquan);
		thisused = bp->price * uquan;
		totused += thisused;
		obj->unpaid = 0;		/* ditto */
		/* Why 'x'?  To match `I x', more or less. */
		buf_p = xprname(obj, (char *)0, 'x', FALSE, thisused, uquan);
#ifdef __SASC
				/* SAS/C 6.2 can't cope for some reason */
		sasc_bug(obj,save_unpaid);
#else
		obj->unpaid = save_unpaid;
#endif
		putstr(datawin, 0, buf_p);
	    }
	}
	if (eshkp->debit) {
	    /* additional shop debt which has no itemization available */
	    if (totused) putstr(datawin, 0, "");
	    totused += eshkp->debit;
	    buf_p = xprname((struct obj *)0,
			    "usage charges and/or other fees",
			    GOLD_SYM, FALSE, eshkp->debit, 0L);
	    putstr(datawin, 0, buf_p);
	}
	buf_p = xprname((struct obj *)0, "Total:", '*', FALSE, totused, 0L);
	putstr(datawin, 0, "");
	putstr(datawin, 0, buf_p);
	display_nhwindow(datawin, FALSE);
    quit:
	destroy_nhwindow(datawin);
	return(0);
}

#define HUNGRY	2

STATIC_OVL long
getprice(obj, shk_buying)
register struct obj *obj;
boolean shk_buying;
{
	register long tmp = (long) objects[obj->otyp].oc_cost;

	if (obj->oartifact) {
	    tmp = arti_cost(obj);
	    if (shk_buying) tmp /= 4;
	}
	switch(obj->oclass) {
	case FOOD_CLASS:
		/* simpler hunger check, (2-4)*cost */
		if (u.uhs >= HUNGRY && !shk_buying) tmp *= (long) u.uhs;
		if (obj->oeaten) tmp = 0L;
		break;
	case WAND_CLASS:
		/*if (obj->spe == -1) tmp = 0L;*/ /* he'll try to sell them to you anyway --Amy */
		break;
	case POTION_CLASS:
		/*if (obj->otyp == POT_WATER && !obj->blessed && !obj->cursed)
			tmp = 0L;*/ /* no, you can't price-ID :P --Amy */
		break;
	case ARMOR_CLASS:
	case WEAPON_CLASS:
		/*if (obj->spe > 0) tmp += 10L * (long) obj->spe;*/ /* fuck price ID :P --Amy */
		/* Don't buy activated explosives! */
		if (is_grenade(obj) && obj->oarmed) tmp = 0L;
		break;

	/* used to be that partly used light sources made it cheaper here... FUCK PRICE ID :P --Amy */

	}
	return tmp;
}

/* shk catches thrown pick-axe */
struct monst *
shkcatch(obj, x, y)
register struct obj *obj;
register xchar x, y;
{
	register struct monst *shkp;

	if (!(shkp = shop_keeper(inside_shop(x, y))) ||
	    !inhishop(shkp)) return(0);

	if (shkp->mcanmove && !shkp->msleeping &&
	    (*u.ushops != ESHK(shkp)->shoproom || !inside_shop(u.ux, u.uy)) &&
	    dist2(shkp->mx, shkp->my, x, y) < 3 &&
	    /* if it is the shk's pos, you hit and anger him */
	    (shkp->mx != x || shkp->my != y)) {
		if (mnearto(shkp, x, y, TRUE))
		    verbalize("Out of my way, scum!");
		if (cansee(x, y)) {
		    pline("%s nimbly%s catches %s.",
			  Monnam(shkp),
			  (x == shkp->mx && y == shkp->my) ? "" : " reaches over and",
			  the(xname(obj)));
		    if (!canspotmon(shkp) && !(shkp->data->msound == MS_DEEPSTATE) && !(shkp->egotype_deepstatemember))
			map_invisible(x, y);
		    delay_output();
		    mark_synch();
		}
		subfrombill(obj, shkp);
		(void) mpickobj(shkp, obj, FALSE);
		return shkp;
	}
	return (struct monst *)0;
}

void
add_damage(x, y, cost)
register xchar x, y;
long cost;
{
	struct damage *tmp_dam;
	char *shops;

	if (IS_DOOR(levl[x][y].typ)) {
	    struct monst *mtmp;

	    /* Don't schedule for repair unless it's a real shop entrance */
	    for (shops = in_rooms(x, y, SHOPBASE); *shops; shops++)
		if ((mtmp = shop_keeper(*shops)) != 0 &&
			x == ESHK(mtmp)->shd.x && y == ESHK(mtmp)->shd.y)
		    break;
	    if (!*shops) return;
	}
	for (tmp_dam = level.damagelist; tmp_dam; tmp_dam = tmp_dam->next)
	    if (tmp_dam->place.x == x && tmp_dam->place.y == y) {
		tmp_dam->cost += cost;
		return;
	    }
	tmp_dam = (struct damage *)alloc((unsigned)sizeof(struct damage));
	tmp_dam->when = monstermoves;
	tmp_dam->place.x = x;
	tmp_dam->place.y = y;
	tmp_dam->cost = cost;
	tmp_dam->typ = levl[x][y].typ;
	tmp_dam->next = level.damagelist;
	level.damagelist = tmp_dam;
	/* If player saw damage, display as a wall forever */
	if (cansee(x, y))
	    levl[x][y].seenv = SVALL;
}

#endif /*OVLB*/
#ifdef OVL0

/*
 * Do something about damage. Either (!croaked) try to repair it, or
 * (croaked) just discard damage structs for non-shared locations, since
 * they'll never get repaired. Assume that shared locations will get
 * repaired eventually by the other shopkeeper(s). This might be an erroneous
 * assumption (they might all be dead too), but we have no reasonable way of
 * telling that.
 */
STATIC_OVL
void
remove_damage(shkp, croaked)
register struct monst *shkp;
register boolean croaked;
{
	register struct damage *tmp_dam, *tmp2_dam;
	register boolean did_repair = FALSE, saw_door = FALSE;
	register boolean saw_floor = FALSE, stop_picking = FALSE;
	register boolean saw_untrap = FALSE;
	uchar saw_walls = 0;

	tmp_dam = level.damagelist;
	tmp2_dam = 0;
	while (tmp_dam) {
	    register xchar x = tmp_dam->place.x, y = tmp_dam->place.y;
	    char shops[5];
	    int disposition;

	    disposition = 0;
	    strcpy(shops, in_rooms(x, y, SHOPBASE));
	    if (index(shops, ESHK(shkp)->shoproom)) {
		if (croaked)
		    disposition = (shops[1])? 0 : 1;
		else if (stop_picking)
		    disposition = repair_damage(shkp, tmp_dam, FALSE);
		else {
		    /* Defer the stop_occupation() until after repair msgs */
		    if (closed_door(x, y))
			stop_picking = picking_at(x, y);
		    disposition = repair_damage(shkp, tmp_dam, FALSE);
		    if (!disposition)
			stop_picking = FALSE;
		}
	    }

	    if (!disposition) {
		tmp2_dam = tmp_dam;
		tmp_dam = tmp_dam->next;
		continue;
	    }

	    if (disposition > 1) {
		did_repair = TRUE;
		if (cansee(x, y)) {
		    if (IS_WALL(levl[x][y].typ))
			saw_walls++;
		    else if (IS_DOOR(levl[x][y].typ))
			saw_door = TRUE;
		    else if (disposition == 3)		/* untrapped */
			saw_untrap = TRUE;
		    else
			saw_floor = TRUE;
		}
	    }

	    tmp_dam = tmp_dam->next;
	    if (!tmp2_dam) {
		free((void *)level.damagelist);
		level.damagelist = tmp_dam;
	    } else {
		free((void *)tmp2_dam->next);
		tmp2_dam->next = tmp_dam;
	    }
	}
	if (!did_repair)
	    return;
	if (saw_walls) {
	    pline("Suddenly, %s section%s of wall close%s up!",
		  (saw_walls == 1) ? "a" : (saw_walls <= 3) ?
						  "some" : "several",
		  (saw_walls == 1) ? "" : "s", (saw_walls == 1) ? "s" : "");
	    if (saw_door)
		pline_The("shop door reappears!");
	    if (saw_floor)
		pline_The("floor is repaired!");
	} else {
	    if (saw_door)
		pline("Suddenly, the shop door reappears!");
	    else if (saw_floor)
		pline("Suddenly, the floor damage is gone!");
	    else if (saw_untrap)
	        pline("Suddenly, the trap is removed from the floor!");
	    else if (inside_shop(u.ux, u.uy) == ESHK(shkp)->shoproom)
		You_feel("more claustrophobic than before.");
	    else if (flags.soundok && !rn2(10))
		Norep("The dungeon acoustics noticeably change.");
	}
	if (stop_picking)
		stop_occupation();
}

/*
 * 0: repair postponed, 1: silent repair (no messages), 2: normal repair
 * 3: untrap
 */
int
repair_damage(shkp, tmp_dam, catchup)
register struct monst *shkp;
register struct damage *tmp_dam;
boolean catchup;	/* restoring a level */
{
	register xchar x, y, i;
	xchar litter[9];
	register struct monst *mtmp;
	register struct obj *otmp;
	register struct trap *ttmp;

	if ((monstermoves - tmp_dam->when) < REPAIR_DELAY)
	    return(0);
	if (shkp->msleeping || !shkp->mcanmove || ESHK(shkp)->following)
	    return(0);
	x = tmp_dam->place.x;
	y = tmp_dam->place.y;
	if (!IS_ROOM(tmp_dam->typ)) {
	    if (x == u.ux && y == u.uy)
		if (!Passes_walls)
		    return(0);
	    if (x == shkp->mx && y == shkp->my)
		return(0);
	    if ((mtmp = m_at(x, y)) && (!mtmp->egotype_wallwalk) && (!passes_walls(mtmp->data)))
		return(0);
	}
	if ((ttmp = t_at(x, y)) != 0) {

	    if ((ttmp->ttyp != RUST_TRAP
					|| shkp->data == &mons[PM_FLAMING_SPHERE]
					|| shkp->data == &mons[PM_IRON_GOLEM])
				&& ttmp->ttyp != STATUE_TRAP
				&& ttmp->ttyp != SHIT_TRAP
				&& ttmp->ttyp != ANIMATION_TRAP
				&& ttmp->ttyp != RMB_LOSS_TRAP
				&& ttmp->ttyp != SUPERSCROLLER_TRAP
				&& ttmp->ttyp != ACTIVE_SUPERSCROLLER_TRAP
				&& ttmp->ttyp != SPEED_TRAP
				&& ttmp->ttyp != MENU_TRAP
				&& ttmp->ttyp != SWARM_TRAP
				&& ttmp->ttyp != AUTOMATIC_SWITCHER
				&& ttmp->ttyp != HEEL_TRAP
				&& ttmp->ttyp != VULN_TRAP
				&& ttmp->ttyp != DISPLAY_TRAP
				&& ttmp->ttyp != SPELL_LOSS_TRAP
				&& ttmp->ttyp != YELLOW_SPELL_TRAP
				&& ttmp->ttyp != AUTO_DESTRUCT_TRAP
				&& ttmp->ttyp != MEMORY_TRAP
				&& ttmp->ttyp != INVENTORY_TRAP
				&& ttmp->ttyp != BLACK_NG_WALL_TRAP
				&& ttmp->ttyp != UNKNOWN_TRAP
				&& ttmp->ttyp != TRAP_PERCENTS
				&& ttmp->ttyp != LAVA_TRAP
				&& ttmp->ttyp != FLOOD_TRAP
				&& ttmp->ttyp != FREE_HAND_TRAP
				&& ttmp->ttyp != UNIDENTIFY_TRAP
				&& ttmp->ttyp != THIRST_TRAP
				&& ttmp->ttyp != LUCK_TRAP
				&& ttmp->ttyp != SHADES_OF_GREY_TRAP
				&& ttmp->ttyp != GRAVITY_TRAP
				&& ttmp->ttyp != STONE_TO_FLESH_TRAP
				&& ttmp->ttyp != QUICKSAND_TRAP
				&& ttmp->ttyp != FAINT_TRAP
				&& ttmp->ttyp != CURSE_TRAP
				&& ttmp->ttyp != DIFFICULTY_TRAP
				&& ttmp->ttyp != SOUND_TRAP
				&& ttmp->ttyp != CASTER_TRAP
				&& ttmp->ttyp != WEAKNESS_TRAP
				&& ttmp->ttyp != ROT_THIRTEEN_TRAP
				&& ttmp->ttyp != BISHOP_TRAP
				&& ttmp->ttyp != CONFUSION_TRAP
				&& ttmp->ttyp != NUPESELL_TRAP
				&& ttmp->ttyp != DROP_TRAP
				&& ttmp->ttyp != DSTW_TRAP
				&& ttmp->ttyp != STATUS_TRAP
				&& ttmp->ttyp != PESTILENCE_TRAP
				&& ttmp->ttyp != FAMINE_TRAP
				&& ttmp->ttyp != ALIGNMENT_TRAP
				&& ttmp->ttyp != STAIRS_TRAP
				&& ttmp->ttyp != UNINFORMATION_TRAP
				&& ttmp->ttyp != TIMERUN_TRAP
				&& ttmp->ttyp != GIANT_EXPLORER_TRAP
				&& ttmp->ttyp != TRAPWARP_TRAP
				&& ttmp->ttyp != YAWM_TRAP
				&& ttmp->ttyp != CRADLE_OF_CHAOS_TRAP
				&& ttmp->ttyp != TEZCATLIPOCA_TRAP
				&& ttmp->ttyp != ENTHUMESIS_TRAP
				&& ttmp->ttyp != MIKRAANESIS_TRAP
				&& ttmp->ttyp != GOTS_TOO_GOOD_TRAP
				&& ttmp->ttyp != KILLER_ROOM_TRAP
				&& ttmp->ttyp != NO_FUN_WALLS_TRAP
				&& ttmp->ttyp != S_PRESSING_TRAP
				&& ttmp->ttyp != BAD_PART_TRAP
				&& ttmp->ttyp != COMPLETELY_BAD_PART_TRAP
				&& ttmp->ttyp != EVIL_VARIANT_TRAP
				&& ttmp->ttyp != INTRINSIC_LOSS_TRAP
				&& ttmp->ttyp != BLOOD_LOSS_TRAP
				&& ttmp->ttyp != BAD_EFFECT_TRAP
				&& ttmp->ttyp != MULTIPLY_TRAP
				&& ttmp->ttyp != AUTO_VULN_TRAP
				&& ttmp->ttyp != NASTINESS_TRAP
				&& ttmp->ttyp != RECURSION_TRAP
				&& ttmp->ttyp != RESPAWN_TRAP
				&& ttmp->ttyp != WARP_ZONE
				&& ttmp->ttyp != CAPTCHA_TRAP
				&& ttmp->ttyp != MIND_WIPE_TRAP
				&& ttmp->ttyp != LOCK_TRAP
				&& ttmp->ttyp != MAGIC_CANCELLATION_TRAP
				&& ttmp->ttyp != FARLOOK_TRAP
				&& ttmp->ttyp != GATEWAY_FROM_HELL
				&& ttmp->ttyp != GROWING_TRAP
				&& ttmp->ttyp != COOLING_TRAP
				&& ttmp->ttyp != BAR_TRAP
				&& ttmp->ttyp != LOCKING_TRAP
				&& ttmp->ttyp != AIR_TRAP
				&& ttmp->ttyp != TERRAIN_TRAP

				&& ttmp->ttyp != ORANGE_SPELL_TRAP
				&& ttmp->ttyp != VIOLET_SPELL_TRAP
				&& ttmp->ttyp != TRAP_OF_LONGING
				&& ttmp->ttyp != CURSED_PART_TRAP
				&& ttmp->ttyp != QUAVERSAL_TRAP
				&& ttmp->ttyp != APPEARANCE_SHUFFLING_TRAP
				&& ttmp->ttyp != BROWN_SPELL_TRAP
				&& ttmp->ttyp != CHOICELESS_TRAP
				&& ttmp->ttyp != GOLDSPELL_TRAP
				&& ttmp->ttyp != DEPROVEMENT_TRAP
				&& ttmp->ttyp != INITIALIZATION_TRAP
				&& ttmp->ttyp != GUSHLUSH_TRAP
				&& ttmp->ttyp != SOILTYPE_TRAP
				&& ttmp->ttyp != DANGEROUS_TERRAIN_TRAP
				&& ttmp->ttyp != FALLOUT_TRAP
				&& ttmp->ttyp != MOJIBAKE_TRAP
				&& ttmp->ttyp != GRAVATION_TRAP
				&& ttmp->ttyp != UNCALLED_TRAP
				&& ttmp->ttyp != EXPLODING_DICE_TRAP
				&& ttmp->ttyp != PERMACURSE_TRAP
				&& ttmp->ttyp != SHROUDED_IDENTITY_TRAP
				&& ttmp->ttyp != FEELER_GAUGES_TRAP
				&& ttmp->ttyp != LONG_SCREWUP_TRAP
				&& ttmp->ttyp != WING_YELLOW_CHANGER
				&& ttmp->ttyp != LIFE_SAVING_TRAP
				&& ttmp->ttyp != CURSEUSE_TRAP
				&& ttmp->ttyp != CUT_NUTRITION_TRAP
				&& ttmp->ttyp != SKILL_LOSS_TRAP
				&& ttmp->ttyp != AUTOPILOT_TRAP
				&& ttmp->ttyp != FORCE_TRAP
				&& ttmp->ttyp != MONSTER_GLYPH_TRAP
				&& ttmp->ttyp != CHANGING_DIRECTIVE_TRAP
				&& ttmp->ttyp != CONTAINER_KABOOM_TRAP
				&& ttmp->ttyp != STEAL_DEGRADE_TRAP
				&& ttmp->ttyp != LEFT_INVENTORY_TRAP
				&& ttmp->ttyp != FLUCTUATING_SPEED_TRAP
				&& ttmp->ttyp != TARMUSTROKINGNORA_TRAP
				&& ttmp->ttyp != FAILURE_TRAP
				&& ttmp->ttyp != BRIGHT_CYAN_SPELL_TRAP
				&& ttmp->ttyp != FREQUENTATION_SPAWN_TRAP
				&& ttmp->ttyp != PET_AI_TRAP
				&& ttmp->ttyp != SATAN_TRAP
				&& ttmp->ttyp != REMEMBERANCE_TRAP
				&& ttmp->ttyp != POKELIE_TRAP
				&& ttmp->ttyp != AUTOPICKUP_TRAP
				&& ttmp->ttyp != DYWYPI_TRAP
				&& ttmp->ttyp != SILVER_SPELL_TRAP
				&& ttmp->ttyp != METAL_SPELL_TRAP
				&& ttmp->ttyp != PLATINUM_SPELL_TRAP
				&& ttmp->ttyp != MANLER_TRAP
				&& ttmp->ttyp != DOORNING_TRAP
				&& ttmp->ttyp != NOWNSIBLE_TRAP
				&& ttmp->ttyp != ELM_STREET_TRAP
				&& ttmp->ttyp != MONNOISE_TRAP
				&& ttmp->ttyp != RANG_CALL_TRAP
				&& ttmp->ttyp != RECURRING_SPELL_LOSS_TRAP
				&& ttmp->ttyp != ANTITRAINING_TRAP
				&& ttmp->ttyp != TECHOUT_TRAP
				&& ttmp->ttyp != STAT_DECAY_TRAP
				&& ttmp->ttyp != MOVEMORK_TRAP

				&& ttmp->ttyp != GRAVE_WALL_TRAP
				&& ttmp->ttyp != TUNNEL_TRAP
				&& ttmp->ttyp != FARMLAND_TRAP
				&& ttmp->ttyp != MOUNTAIN_TRAP
				&& ttmp->ttyp != WATER_TUNNEL_TRAP
				&& ttmp->ttyp != CRYSTAL_FLOOD_TRAP
				&& ttmp->ttyp != MOORLAND_TRAP
				&& ttmp->ttyp != URINE_TRAP
				&& ttmp->ttyp != SHIFTING_SAND_TRAP
				&& ttmp->ttyp != STYX_TRAP
				&& ttmp->ttyp != PENTAGRAM_TRAP
				&& ttmp->ttyp != SNOW_TRAP
				&& ttmp->ttyp != ASH_TRAP
				&& ttmp->ttyp != SAND_TRAP
				&& ttmp->ttyp != PAVEMENT_TRAP
				&& ttmp->ttyp != HIGHWAY_TRAP
				&& ttmp->ttyp != GRASSLAND_TRAP
				&& ttmp->ttyp != NETHER_MIST_TRAP
				&& ttmp->ttyp != STALACTITE_TRAP
				&& ttmp->ttyp != CRYPTFLOOR_TRAP
				&& ttmp->ttyp != BUBBLE_TRAP
				&& ttmp->ttyp != RAIN_CLOUD_TRAP

				&& ttmp->ttyp != ITEM_NASTIFICATION_TRAP
				&& ttmp->ttyp != SANITY_INCREASE_TRAP
				&& ttmp->ttyp != PSI_TRAP
				&& ttmp->ttyp != GAY_TRAP

				&& ttmp->ttyp != SARAH_TRAP
				&& ttmp->ttyp != CLAUDIA_TRAP
				&& ttmp->ttyp != LUDGERA_TRAP
				&& ttmp->ttyp != KATI_TRAP

				&& ttmp->ttyp != SANITY_TREBLE_TRAP
				&& ttmp->ttyp != STAT_DECREASE_TRAP
				&& ttmp->ttyp != SIMEOUT_TRAP

				&& ttmp->ttyp != WALL_TRAP
				&& ttmp->ttyp != MONSTER_GENERATOR
				&& ttmp->ttyp != POTION_DISPENSER
				&& ttmp->ttyp != SPACEWARS_SPAWN_TRAP
				&& ttmp->ttyp != TV_TROPES_TRAP
				&& ttmp->ttyp != SYMBIOTE_TRAP
				&& ttmp->ttyp != KILL_SYMBIOTE_TRAP
				&& ttmp->ttyp != SYMBIOTE_REPLACEMENT_TRAP
				&& ttmp->ttyp != SHUTDOWN_TRAP
				&& ttmp->ttyp != CORONA_TRAP
				&& ttmp->ttyp != UNPROOFING_TRAP
				&& ttmp->ttyp != VISIBILITY_TRAP
				&& ttmp->ttyp != FEMINISM_STONE_TRAP
				&& ttmp->ttyp != SHUEFT_TRAP
				&& ttmp->ttyp != MOTH_LARVAE_TRAP
				&& ttmp->ttyp != WORTHINESS_TRAP
				&& ttmp->ttyp != CONDUCT_TRAP
				&& ttmp->ttyp != STRIKETHROUGH_TRAP
				&& ttmp->ttyp != MULTIPLE_GATHER_TRAP
				&& ttmp->ttyp != VIVISECTION_TRAP
				&& ttmp->ttyp != INSTAFEMINISM_TRAP
				&& ttmp->ttyp != INSTANASTY_TRAP
				&& ttmp->ttyp != SKILL_POINT_LOSS_TRAP
				&& ttmp->ttyp != PERFECT_MATCH_TRAP
				&& ttmp->ttyp != DUMBIE_LIGHTSABER_TRAP
				&& ttmp->ttyp != WRONG_STAIRS
				&& ttmp->ttyp != TECHSTOP_TRAP
				&& ttmp->ttyp != AMNESIA_SWITCH_TRAP
				&& ttmp->ttyp != SKILL_SWAP_TRAP
				&& ttmp->ttyp != SKILL_UPORDOWN_TRAP
				&& ttmp->ttyp != SKILL_RANDOMIZE_TRAP

				&& ttmp->ttyp != HYBRID_TRAP
				&& ttmp->ttyp != SHAPECHANGE_TRAP
				&& ttmp->ttyp != MELTEM_TRAP
				&& ttmp->ttyp != MIGUC_TRAP
				&& ttmp->ttyp != DIRECTIVE_TRAP
				&& ttmp->ttyp != SATATUE_TRAP
				&& ttmp->ttyp != FARTING_WEB
				&& ttmp->ttyp != CATACLYSM_TRAP
				&& ttmp->ttyp != DATA_DELETE_TRAP
				&& ttmp->ttyp != ELDER_TENTACLING_TRAP
				&& ttmp->ttyp != FOOTERER_TRAP

				&& ttmp->ttyp != SKILL_MULTIPLY_TRAP
				&& ttmp->ttyp != TRAPWALK_TRAP
				&& ttmp->ttyp != CLUSTER_TRAP
				&& ttmp->ttyp != FIELD_TRAP
				&& ttmp->ttyp != MONICIDE_TRAP
				&& ttmp->ttyp != TRAP_CREATION_TRAP
				&& ttmp->ttyp != LEOLD_TRAP
				&& ttmp->ttyp != ANIMEBAND_TRAP
				&& ttmp->ttyp != PERFUME_TRAP
				&& ttmp->ttyp != COURT_TRAP
				&& ttmp->ttyp != ELDER_SCROLLS_TRAP
				&& ttmp->ttyp != JOKE_TRAP
				&& ttmp->ttyp != DUNGEON_LORDS_TRAP
				&& ttmp->ttyp != FORTYTWO_TRAP
				&& ttmp->ttyp != RANDOMIZE_TRAP
				&& ttmp->ttyp != EVILROOM_TRAP
				&& ttmp->ttyp != AOE_TRAP
				&& ttmp->ttyp != ELONA_TRAP
				&& ttmp->ttyp != RELIGION_TRAP
				&& ttmp->ttyp != STEAMBAND_TRAP
				&& ttmp->ttyp != HARDCORE_TRAP
				&& ttmp->ttyp != MACHINE_TRAP
				&& ttmp->ttyp != BEE_TRAP
				&& ttmp->ttyp != MIGO_TRAP
				&& ttmp->ttyp != ANGBAND_TRAP
				&& ttmp->ttyp != DNETHACK_TRAP
				&& ttmp->ttyp != EVIL_SPAWN_TRAP
				&& ttmp->ttyp != SHOE_TRAP
				&& ttmp->ttyp != INSIDE_TRAP
				&& ttmp->ttyp != DOOM_TRAP
				&& ttmp->ttyp != MILITARY_TRAP
				&& ttmp->ttyp != ILLUSION_TRAP
				&& ttmp->ttyp != DIABLO_TRAP

				&& ttmp->ttyp != EVIL_HEEL_TRAP
				&& ttmp->ttyp != BAD_EQUIPMENT_TRAP
				&& ttmp->ttyp != TEMPOCONFLICT_TRAP
				&& ttmp->ttyp != TEMPOHUNGER_TRAP
				&& ttmp->ttyp != TELEPORTITIS_TRAP
				&& ttmp->ttyp != POLYMORPHITIS_TRAP
				&& ttmp->ttyp != PREMATURE_DEATH_TRAP
				&& ttmp->ttyp != LASTING_AMNESIA_TRAP
				&& ttmp->ttyp != RAGNAROK_TRAP
				&& ttmp->ttyp != SINGLE_DISENCHANT_TRAP

				&& ttmp->ttyp != SEVERE_DISENCHANT_TRAP
				&& ttmp->ttyp != PAIN_TRAP
				&& ttmp->ttyp != TREMBLING_TRAP
				&& ttmp->ttyp != TECHCAP_TRAP
				&& ttmp->ttyp != SPELL_MEMORY_TRAP
				&& ttmp->ttyp != SKILL_REDUCTION_TRAP
				&& ttmp->ttyp != SKILLCAP_TRAP
				&& ttmp->ttyp != PERMANENT_STAT_DAMAGE_TRAP

				&& ttmp->ttyp != LOOTCUT_TRAP
				&& ttmp->ttyp != MONSTER_SPEED_TRAP
				&& ttmp->ttyp != SCALING_TRAP
				&& ttmp->ttyp != ENMITY_TRAP
				&& ttmp->ttyp != WHITE_SPELL_TRAP
				&& ttmp->ttyp != COMPLETE_GRAY_SPELL_TRAP
				&& ttmp->ttyp != QUASAR_TRAP
				&& ttmp->ttyp != MOMMA_TRAP
				&& ttmp->ttyp != HORROR_TRAP
				&& ttmp->ttyp != ARTIFICER_TRAP
				&& ttmp->ttyp != WEREFORM_TRAP
				&& ttmp->ttyp != NON_PRAYER_TRAP
				&& ttmp->ttyp != EVIL_PATCH_TRAP
				&& ttmp->ttyp != HARD_MODE_TRAP
				&& ttmp->ttyp != SECRET_ATTACK_TRAP
				&& ttmp->ttyp != EATER_TRAP
				&& ttmp->ttyp != COVETOUSNESS_TRAP
				&& ttmp->ttyp != NOT_SEEN_TRAP
				&& ttmp->ttyp != DARK_MODE_TRAP
				&& ttmp->ttyp != ANTISEARCH_TRAP
				&& ttmp->ttyp != HOMICIDE_TRAP
				&& ttmp->ttyp != NASTY_NATION_TRAP
				&& ttmp->ttyp != WAKEUP_CALL_TRAP
				&& ttmp->ttyp != GRAYOUT_TRAP
				&& ttmp->ttyp != GRAY_CENTER_TRAP
				&& ttmp->ttyp != CHECKERBOARD_TRAP
				&& ttmp->ttyp != CLOCKWISE_SPIN_TRAP
				&& ttmp->ttyp != COUNTERCLOCKWISE_SPIN_TRAP
				&& ttmp->ttyp != LAG_TRAP
				&& ttmp->ttyp != BLESSCURSE_TRAP
				&& ttmp->ttyp != DE_LIGHT_TRAP
				&& ttmp->ttyp != DISCHARGE_TRAP
				&& ttmp->ttyp != TRASHING_TRAP
				&& ttmp->ttyp != FILTERING_TRAP
				&& ttmp->ttyp != DEFORMATTING_TRAP
				&& ttmp->ttyp != FLICKER_STRIP_TRAP
				&& ttmp->ttyp != UNDRESSING_TRAP
				&& ttmp->ttyp != HYPERBLUEWALL_TRAP
				&& ttmp->ttyp != NOLITE_TRAP
				&& ttmp->ttyp != PARANOIA_TRAP
				&& ttmp->ttyp != FLEECESCRIPT_TRAP
				&& ttmp->ttyp != INTERRUPT_TRAP
				&& ttmp->ttyp != DUSTBIN_TRAP
				&& ttmp->ttyp != MANA_BATTERY_TRAP
				&& ttmp->ttyp != MONSTERFINGERS_TRAP
				&& ttmp->ttyp != MISCAST_TRAP
				&& ttmp->ttyp != MESSAGE_SUPPRESSION_TRAP
				&& ttmp->ttyp != STUCK_ANNOUNCEMENT_TRAP
				&& ttmp->ttyp != BLOODTHIRSTY_TRAP
				&& ttmp->ttyp != MAXIMUM_DAMAGE_TRAP
				&& ttmp->ttyp != LATENCY_TRAP
				&& ttmp->ttyp != STARLIT_TRAP
				&& ttmp->ttyp != KNOWLEDGE_TRAP
				&& ttmp->ttyp != HIGHSCORE_TRAP
				&& ttmp->ttyp != PINK_SPELL_TRAP
				&& ttmp->ttyp != GREEN_SPELL_TRAP
				&& ttmp->ttyp != EVC_TRAP
				&& ttmp->ttyp != UNDERLAYER_TRAP
				&& ttmp->ttyp != DAMAGE_METER_TRAP
				&& ttmp->ttyp != ARBITRARY_WEIGHT_TRAP
				&& ttmp->ttyp != FUCKED_INFO_TRAP
				&& ttmp->ttyp != BLACK_SPELL_TRAP
				&& ttmp->ttyp != CYAN_SPELL_TRAP
				&& ttmp->ttyp != HEAP_TRAP
				&& ttmp->ttyp != BLUE_SPELL_TRAP
				&& ttmp->ttyp != TRON_TRAP
				&& ttmp->ttyp != RED_SPELL_TRAP
				&& ttmp->ttyp != TOO_HEAVY_TRAP
				&& ttmp->ttyp != ELONGATION_TRAP
				&& ttmp->ttyp != WRAPOVER_TRAP
				&& ttmp->ttyp != DESTRUCTION_TRAP
				&& ttmp->ttyp != MELEE_PREFIX_TRAP
				&& ttmp->ttyp != AUTOMORE_TRAP
				&& ttmp->ttyp != UNFAIR_ATTACK_TRAP

				&& ttmp->ttyp != LOUDSPEAKER
				&& ttmp->ttyp != NEST_TRAP
				&& ttmp->ttyp != CYANIDE_TRAP
				&& ttmp->ttyp != LASER_TRAP
				&& ttmp->ttyp != FART_TRAP
				&& ttmp->ttyp != CONFUSE_TRAP
				&& ttmp->ttyp != STUN_TRAP
				&& ttmp->ttyp != HALLUCINATION_TRAP
				&& ttmp->ttyp != PETRIFICATION_TRAP
				&& ttmp->ttyp != NUMBNESS_TRAP
				&& ttmp->ttyp != FREEZING_TRAP
				&& ttmp->ttyp != BURNING_TRAP
				&& ttmp->ttyp != FEAR_TRAP
				&& ttmp->ttyp != BLINDNESS_TRAP
				&& ttmp->ttyp != GLIB_TRAP
				&& ttmp->ttyp != SLIME_TRAP
				&& ttmp->ttyp != INERTIA_TRAP
				&& ttmp->ttyp != TIME_TRAP
				&& ttmp->ttyp != LYCANTHROPY_TRAP
				&& ttmp->ttyp != UNLIGHT_TRAP
				&& ttmp->ttyp != ELEMENTAL_TRAP
				&& ttmp->ttyp != ESCALATING_TRAP
				&& ttmp->ttyp != NEGATIVE_TRAP
				&& ttmp->ttyp != MANA_TRAP
				&& ttmp->ttyp != SIN_TRAP
				&& ttmp->ttyp != DESTROY_ARMOR_TRAP
				&& ttmp->ttyp != DIVINE_ANGER_TRAP
				&& ttmp->ttyp != GENETIC_TRAP
				&& ttmp->ttyp != MISSINGNO_TRAP
				&& ttmp->ttyp != CANCELLATION_TRAP
				&& ttmp->ttyp != HOSTILITY_TRAP
				&& ttmp->ttyp != BOSS_TRAP
				&& ttmp->ttyp != WISHING_TRAP
				&& ttmp->ttyp != RECURRING_AMNESIA_TRAP
				&& ttmp->ttyp != BIGSCRIPT_TRAP
				&& ttmp->ttyp != BANK_TRAP
				&& ttmp->ttyp != ONLY_TRAP
				&& ttmp->ttyp != MAP_TRAP
				&& ttmp->ttyp != TECH_TRAP
				&& ttmp->ttyp != DISENCHANT_TRAP
				&& ttmp->ttyp != VERISIERT
				&& ttmp->ttyp != CHAOS_TRAP
				&& ttmp->ttyp != MUTENESS_TRAP
				&& ttmp->ttyp != NTLL_TRAP
				&& ttmp->ttyp != ENGRAVING_TRAP
				&& ttmp->ttyp != MAGIC_DEVICE_TRAP
				&& ttmp->ttyp != BOOK_TRAP
				&& ttmp->ttyp != LEVEL_TRAP
				&& ttmp->ttyp != QUIZ_TRAP

				&& ttmp->ttyp != BOMB_TRAP
				&& ttmp->ttyp != EARTHQUAKE_TRAP
				&& ttmp->ttyp != GLUE_TRAP
				&& ttmp->ttyp != GUILLOTINE_TRAP
				&& ttmp->ttyp != BISECTION_TRAP
				&& ttmp->ttyp != VOLT_TRAP
				&& ttmp->ttyp != HORDE_TRAP
				&& ttmp->ttyp != IMMOBILITY_TRAP
				&& ttmp->ttyp != GREEN_GLYPH
				&& ttmp->ttyp != BLUE_GLYPH
				&& ttmp->ttyp != YELLOW_GLYPH
				&& ttmp->ttyp != ORANGE_GLYPH
				&& ttmp->ttyp != BLACK_GLYPH
				&& ttmp->ttyp != PURPLE_GLYPH
				&& ttmp->ttyp != METABOLIC_TRAP
				&& ttmp->ttyp != TRAP_OF_NO_RETURN
				&& ttmp->ttyp != EGOTRAP
				&& ttmp->ttyp != FAST_FORWARD_TRAP
				&& ttmp->ttyp != TRAP_OF_ROTTENNESS
				&& ttmp->ttyp != UNSKILLED_TRAP
				&& ttmp->ttyp != LOW_STATS_TRAP
				&& ttmp->ttyp != TRAINING_TRAP
				&& ttmp->ttyp != EXERCISE_TRAP
				&& ttmp->ttyp != FALLING_LOADSTONE_TRAP
				&& ttmp->ttyp != SUMMON_UNDEAD_TRAP
				&& ttmp->ttyp != FALLING_NASTYSTONE_TRAP

				&& ttmp->ttyp != SPINED_BALL_TRAP
				&& ttmp->ttyp != PENDULUM_TRAP
				&& ttmp->ttyp != TURN_TABLE
				&& ttmp->ttyp != SCENT_TRAP
				&& ttmp->ttyp != BANANA_TRAP
				&& ttmp->ttyp != FALLING_TUB_TRAP
				&& ttmp->ttyp != ALARM
				&& ttmp->ttyp != CALTROPS_TRAP
				&& ttmp->ttyp != BLADE_WIRE
				&& ttmp->ttyp != MAGNET_TRAP
				&& ttmp->ttyp != SLINGSHOT_TRAP
				&& ttmp->ttyp != CANNON_TRAP
				&& ttmp->ttyp != VENOM_SPRINKLER
				&& ttmp->ttyp != FUMAROLE

				&& ttmp->ttyp != NEXUS_TRAP
				&& ttmp->ttyp != LEG_TRAP
				&& ttmp->ttyp != ARTIFACT_JACKPOT_TRAP
				&& ttmp->ttyp != MAP_AMNESIA_TRAP
				&& ttmp->ttyp != SPREADING_TRAP
				&& ttmp->ttyp != ADJACENT_TRAP
				&& ttmp->ttyp != SUPERTHING_TRAP
				&& ttmp->ttyp != LEVITATION_TRAP
				&& ttmp->ttyp != BOWEL_CRAMPS_TRAP
				&& ttmp->ttyp != UNEQUIPPING_TRAP
				&& ttmp->ttyp != GOOD_ARTIFACT_TRAP
				&& ttmp->ttyp != GENDER_TRAP
				&& ttmp->ttyp != TRAP_OF_OPPOSITE_ALIGNMENT
				&& ttmp->ttyp != SINCOUNT_TRAP
				&& ttmp->ttyp != WRENCHING_TRAP
				&& ttmp->ttyp != TRACKER_TRAP
				&& ttmp->ttyp != NURSE_TRAP
				&& ttmp->ttyp != BACK_TO_START_TRAP
				&& ttmp->ttyp != NEMESIS_TRAP
				&& ttmp->ttyp != STREW_TRAP
				&& ttmp->ttyp != OUTTA_DEPTH_TRAP
				&& ttmp->ttyp != PUNISHMENT_TRAP
				&& ttmp->ttyp != BOON_TRAP
				&& ttmp->ttyp != FOUNTAIN_TRAP
				&& ttmp->ttyp != THRONE_TRAP
				&& ttmp->ttyp != ARABELLA_SPEAKER
				&& ttmp->ttyp != FEMMY_TRAP
				&& ttmp->ttyp != MADELEINE_TRAP
				&& ttmp->ttyp != MARLENA_TRAP
				&& ttmp->ttyp != RUTH_TRAP
				&& ttmp->ttyp != MAGDALENA_TRAP
				&& ttmp->ttyp != MARLEEN_TRAP
				&& ttmp->ttyp != KLARA_TRAP
				&& ttmp->ttyp != FRIEDERIKE_TRAP
				&& ttmp->ttyp != NAOMI_TRAP
				&& ttmp->ttyp != UTE_TRAP
				&& ttmp->ttyp != JASIEEN_TRAP
				&& ttmp->ttyp != YASAMAN_TRAP
				&& ttmp->ttyp != MAY_BRITT_TRAP
				&& ttmp->ttyp != KSENIA_TRAP
				&& ttmp->ttyp != LYDIA_TRAP
				&& ttmp->ttyp != CONNY_TRAP
				&& ttmp->ttyp != KATIA_TRAP
				&& ttmp->ttyp != MARIYA_TRAP
				&& ttmp->ttyp != ELISE_TRAP
				&& ttmp->ttyp != RONJA_TRAP
				&& ttmp->ttyp != ARIANE_TRAP
				&& ttmp->ttyp != JOHANNA_TRAP
				&& ttmp->ttyp != INGE_TRAP
				&& ttmp->ttyp != ROSA_TRAP
				&& ttmp->ttyp != JANINA_TRAP
				&& ttmp->ttyp != KRISTIN_TRAP
				&& ttmp->ttyp != ANNA_TRAP
				&& ttmp->ttyp != RUEA_TRAP
				&& ttmp->ttyp != DORA_TRAP
				&& ttmp->ttyp != MARIKE_TRAP
				&& ttmp->ttyp != JETTE_TRAP
				&& ttmp->ttyp != INA_TRAP
				&& ttmp->ttyp != SING_TRAP
				&& ttmp->ttyp != VICTORIA_TRAP
				&& ttmp->ttyp != MELISSA_TRAP
				&& ttmp->ttyp != ANITA_TRAP
				&& ttmp->ttyp != HENRIETTA_TRAP
				&& ttmp->ttyp != VERENA_TRAP
				&& ttmp->ttyp != ARABELLA_TRAP
				&& ttmp->ttyp != NELLY_TRAP
				&& ttmp->ttyp != EVELINE_TRAP
				&& ttmp->ttyp != KARIN_TRAP
				&& ttmp->ttyp != JUEN_TRAP
				&& ttmp->ttyp != KRISTINA_TRAP
				&& ttmp->ttyp != ALMUT_TRAP
				&& ttmp->ttyp != JULIETTA_TRAP
				&& ttmp->ttyp != LOU_TRAP
				&& ttmp->ttyp != ANASTASIA_TRAP
				&& ttmp->ttyp != FILLER_TRAP
				&& ttmp->ttyp != TOXIC_VENOM_TRAP
				&& ttmp->ttyp != INSANITY_TRAP
				&& ttmp->ttyp != MADNESS_TRAP
				&& ttmp->ttyp != JESSICA_TRAP
				&& ttmp->ttyp != SOLVEJG_TRAP
				&& ttmp->ttyp != WENDY_TRAP
				&& ttmp->ttyp != KATHARINA_TRAP
				&& ttmp->ttyp != ELENA_TRAP
				&& ttmp->ttyp != THAI_TRAP
				&& ttmp->ttyp != ELIF_TRAP
				&& ttmp->ttyp != NADJA_TRAP
				&& ttmp->ttyp != SANDRA_TRAP
				&& ttmp->ttyp != NATALJE_TRAP
				&& ttmp->ttyp != JEANETTA_TRAP
				&& ttmp->ttyp != YVONNE_TRAP
				&& ttmp->ttyp != MAURAH_TRAP
				&& ttmp->ttyp != ANNEMARIE_TRAP
				&& ttmp->ttyp != JIL_TRAP
				&& ttmp->ttyp != JANA_TRAP
				&& ttmp->ttyp != KATRIN_TRAP
				&& ttmp->ttyp != GUDRUN_TRAP
				&& ttmp->ttyp != ELLA_TRAP
				&& ttmp->ttyp != MANUELA_TRAP
				&& ttmp->ttyp != JENNIFER_TRAP
				&& ttmp->ttyp != PATRICIA_TRAP
				&& ttmp->ttyp != ANTJE_TRAP
				&& ttmp->ttyp != ANTJE_TRAP_X
				&& ttmp->ttyp != KERSTIN_TRAP
				&& ttmp->ttyp != LAURA_TRAP
				&& ttmp->ttyp != LARISSA_TRAP
				&& ttmp->ttyp != NORA_TRAP
				&& ttmp->ttyp != NATALIA_TRAP
				&& ttmp->ttyp != SUSANNE_TRAP
				&& ttmp->ttyp != LISA_TRAP
				&& ttmp->ttyp != BRIDGHITTE_TRAP
				&& ttmp->ttyp != JULIA_TRAP
				&& ttmp->ttyp != NICOLE_TRAP
				&& ttmp->ttyp != RITA_TRAP

				&& ttmp->ttyp != ELEMENTAL_PORTAL
				&& ttmp->ttyp != GIRLINESS_TRAP
				&& ttmp->ttyp != FUMBLING_TRAP
				&& ttmp->ttyp != EGOMONSTER_TRAP
				&& ttmp->ttyp != FLOODING_TRAP
				&& ttmp->ttyp != MONSTER_CUBE
				&& ttmp->ttyp != CURSED_GRAVE
				&& ttmp->ttyp != LIMITATION_TRAP
				&& ttmp->ttyp != WEAK_SIGHT_TRAP
				&& ttmp->ttyp != RANDOM_MESSAGE_TRAP

				&& ttmp->ttyp != DESECRATION_TRAP
				&& ttmp->ttyp != STARVATION_TRAP
				&& ttmp->ttyp != DROPLESS_TRAP
				&& ttmp->ttyp != LOW_EFFECT_TRAP
				&& ttmp->ttyp != INVISIBLE_TRAP
				&& ttmp->ttyp != GHOST_WORLD_TRAP
				&& ttmp->ttyp != DEHYDRATION_TRAP
				&& ttmp->ttyp != HATE_TRAP
				&& ttmp->ttyp != SPACEWARS_TRAP
				&& ttmp->ttyp != TEMPORARY_RECURSION_TRAP
				&& ttmp->ttyp != TOTTER_TRAP
				&& ttmp->ttyp != NONINTRINSICAL_TRAP
				&& ttmp->ttyp != DROPCURSE_TRAP
				&& ttmp->ttyp != NAKEDNESS_TRAP
				&& ttmp->ttyp != ANTILEVEL_TRAP
				&& ttmp->ttyp != VENTILATOR
				&& ttmp->ttyp != STEALER_TRAP
				&& ttmp->ttyp != REBELLION_TRAP
				&& ttmp->ttyp != CRAP_TRAP
				&& ttmp->ttyp != MISFIRE_TRAP
				&& ttmp->ttyp != TRAP_OF_WALLS
				&& ttmp->ttyp != DISCONNECT_TRAP
				&& ttmp->ttyp != INTERFACE_SCREW_TRAP
				&& ttmp->ttyp != BOSSFIGHT_TRAP
				&& ttmp->ttyp != ENTIRE_LEVEL_TRAP
				&& ttmp->ttyp != BONES_TRAP
				&& ttmp->ttyp != AUTOCURSE_TRAP
				&& ttmp->ttyp != HIGHLEVEL_TRAP
				&& ttmp->ttyp != SPELL_FORGETTING_TRAP
				&& ttmp->ttyp != SOUND_EFFECT_TRAP
				&& ttmp->ttyp != CONTAMINATION_TRAP
				&& ttmp->ttyp != BOSS_SPAWNER
				&& ttmp->ttyp != KOP_CUBE

				&& (ttmp->ttyp != DEATH_TRAP ||
				    (nonliving(shkp->data) || is_demon(shkp->data) || resists_death(shkp) || shkp->data->msound == MS_NEMESIS || resists_magm(shkp)) )

				&& (ttmp->ttyp != DISINTEGRATION_TRAP ||
				    (!resists_disint(shkp)) )

				&& (ttmp->ttyp != DRAIN_TRAP ||
				    (!resists_drli(shkp)) )

				&& (ttmp->ttyp != SLP_GAS_TRAP ||
				    (!resists_sleep(shkp) && !breathless(shkp->data) && (!shkp->egotype_undead) ) )
				&& (ttmp->ttyp != POISON_GAS_TRAP ||
				    (!resists_poison(shkp) && !breathless(shkp->data) && (!shkp->egotype_undead) ) )
				&& (ttmp->ttyp != SLOW_GAS_TRAP ||
				    (!breathless(shkp->data) && (!shkp->egotype_undead) ) )
				&& (ttmp->ttyp != BEAR_TRAP ||
				    (shkp->data->msize > MZ_SMALL &&
				     !amorphous(shkp->data) && !is_flyer(shkp->data) && (!shkp->egotype_flying) ))
				&& (ttmp->ttyp != FIRE_TRAP ||
				    !resists_fire(shkp))
				&& (ttmp->ttyp != SHOCK_TRAP ||
				    !resists_elec(shkp))
				&& (ttmp->ttyp != ICE_TRAP ||
				    !resists_cold(shkp))
				&& (ttmp->ttyp != SQKY_BOARD || (!is_flyer(shkp->data) && (!shkp->egotype_flying) ))
				&& (ttmp->ttyp != ACID_POOL || (!is_flyer(shkp->data) && (!shkp->egotype_flying) && !is_floater(shkp->data) && !resists_acid(shkp)) )
				&& (ttmp->ttyp != WATER_POOL || (!is_flyer(shkp->data) && (!shkp->egotype_flying) && !is_floater(shkp->data) && !is_swimmer(shkp->data) && !shkp->egotype_watersplasher && !amphibious(shkp->data) && !breathless(shkp->data) && (!shkp->egotype_undead) ) )
				&& (ttmp->ttyp != WEB || (!amorphous(shkp->data) && !shkp->egotype_webber &&
				    !webmaker(shkp->data) && !dmgtype(shkp->data, AD_WEBS) ))
			) {

	    if (x == u.ux && y == u.uy)
		if (!Passes_walls)
		    return(0);
	    if (ttmp->ttyp == LANDMINE || ttmp->ttyp == BEAR_TRAP) {
		/* convert to an object */
		otmp = mksobj((ttmp->ttyp == LANDMINE) ? LAND_MINE : BEARTRAP, TRUE, FALSE, FALSE);
		if (otmp) {
			otmp->quan= 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(shkp, otmp, FALSE);
		}
	    }
	    deltrap(ttmp);
	    if(IS_DOOR(tmp_dam->typ)) {
		levl[x][y].doormask = D_CLOSED; /* arbitrary */
		block_point(x, y);
	    } else if (IS_WALL(tmp_dam->typ)) {
		levl[x][y].typ = tmp_dam->typ;
		block_point(x, y);
	    }
	    newsym(x, y);
	    return(3);
	    }
	}
	if (IS_ROOM(tmp_dam->typ)) {
	    /* No messages, because player already filled trap door */
	    return(1);
	}
	if ((tmp_dam->typ == levl[x][y].typ) &&
	    (!IS_DOOR(tmp_dam->typ) || (levl[x][y].doormask > D_BROKEN)))
	    /* No messages if player already replaced shop door */
	    return(1);
	levl[x][y].typ = tmp_dam->typ;
	(void) memset((void *)litter, 0, sizeof(litter));
	if ((otmp = level.objects[x][y]) != 0) {
	    /* Scatter objects haphazardly into the shop */
#define NEED_UPDATE 1
#define OPEN	    2
#define INSHOP	    4
#define horiz(i) ((i%3)-1)
#define vert(i)  ((i/3)-1)
	    for (i = 0; i < 9; i++) {
		if ((i == 4) || (!ZAP_POS(levl[x+horiz(i)][y+vert(i)].typ)))
		    continue;
		litter[i] = OPEN;
		if (inside_shop(x+horiz(i),
				y+vert(i)) == ESHK(shkp)->shoproom)
		    litter[i] |= INSHOP;
	    }
	    if (Punished && !u.uswallow &&
				((uchain->ox == x && uchain->oy == y) ||
				 (uball->ox == x && uball->oy == y))) {
		/*
		 * Either the ball or chain is in the repair location.
		 *
		 * Take the easy way out and put ball&chain under hero.
		 */
		verbalize("Get your junk out of my wall!");
		unplacebc();	/* pick 'em up */
		placebc();	/* put 'em down */
	    }
	    while ((otmp = level.objects[x][y]) != 0)
		/* Don't mess w/ boulders -- just merge into wall */
		if ((otmp->otyp == BOULDER) || (otmp->otyp == ROCK)) {
		    obj_extract_self(otmp);
		    obfree(otmp, (struct obj *)0);
		} else {
		    while (!(litter[i = rn2(9)] & INSHOP));
			remove_object(otmp);
			place_object(otmp, x+horiz(i), y+vert(i));
			litter[i] |= NEED_UPDATE;
		}
	}
	if (catchup) return 1;	/* repair occurred while off level */

	block_point(x, y);
	if(IS_DOOR(tmp_dam->typ)) {
	    levl[x][y].doormask = D_CLOSED; /* arbitrary */
	    newsym(x, y);
	} else {
	    /* don't set doormask  - it is (hopefully) the same as it was */
	    /* if not, perhaps save it with the damage array...  */

	    if (IS_WALL(tmp_dam->typ) && cansee(x, y)) {
	    /* Player sees actual repair process, so they KNOW it's a wall */
		levl[x][y].seenv = SVALL;
		newsym(x, y);
	    }
	    /* Mark this wall as "repaired".  There currently is no code */
	    /* to do anything about repaired walls, so don't do it.	 */
	}
	for (i = 0; i < 9; i++)
	    if (litter[i] & NEED_UPDATE)
		newsym(x+horiz(i), y+vert(i));
	return(2);
#undef NEED_UPDATE
#undef OPEN
#undef INSHOP
#undef vert
#undef horiz
}
#endif /*OVL0*/
#ifdef OVL3
/*
 * shk_move: return 1: moved  0: didn't  -1: let m_move do it  -2: died
 */
int
shk_move(shkp)
register struct monst *shkp;
{
	register xchar gx,gy,omx,omy;
	register int udist;
	register schar appr;
	register struct eshk *eshkp = ESHK(shkp);
	int z;
	boolean uondoor = FALSE, satdoor, avoid = FALSE, badinv;

	omx = shkp->mx;
	omy = shkp->my;

	if (inhishop(shkp))
	    remove_damage(shkp, FALSE);

	if((udist = distu(omx,omy)) < 3 &&
	   (!(isgridbug(shkp->data)) || (omx==u.ux || omy==u.uy))) {
		if(ANGRY(shkp) ||
		   (Conflict && !resist(shkp, RING_CLASS, 0, 0)) ||
		   (StrongConflict && !resist(shkp, RING_CLASS, 0, 0))) {
			if(Displaced)
			  Your("displaced image doesn't fool %s!",
				mon_nam(shkp));
			(void) mattacku(shkp);
			return(0);
		}
		if(eshkp->following) {
			if(strncmp(eshkp->customer, plname, PL_NSIZ)) {
			    verbalize("%s, %s!  I was looking for %s.",
				    Hello(shkp), playeraliasname, eshkp->customer);
				    eshkp->following = 0;
			    return(0);
			}
			if(moves > followmsg+4) {
			    verbalize("%s, %s!  Didn't you forget to pay?",
				    Hello(shkp), playeraliasname);
			    followmsg = moves;
			    if (!rn2(9)) {
			      pline("%s doesn't like customers who don't pay.",
				    Monnam(shkp));
				rile_shk(shkp);
			    }
			}
			if(udist < 2)
			    return(0);
		}
	}

	appr = 1;
	gx = eshkp->shk.x;
	gy = eshkp->shk.y;
	satdoor = (gx == omx && gy == omy);
	if(eshkp->following || ((z = holetime()) >= 0 && z*z <= udist)){
		/* [This distance check used to apply regardless of
		    whether the shk was following, but that resulted in
		    m_move() sometimes taking the shk out of the shop if
		    the player had fenced him in with boulders or traps.
		    Such voluntary abandonment left unpaid objects in
		    invent, triggering billing impossibilities on the
		    next level once the character fell through the hole.] */
		if (udist > 4 && eshkp->following && !eshkp->billct)
		    return(-1);	/* leave it to m_move */
		gx = u.ux;
		gy = u.uy;
	} else if(ANGRY(shkp)) {
		/* Move towards the hero if the shopkeeper can see him. */
		if(shkp->mcansee && m_canseeu(shkp)) {
			gx = u.ux;
			gy = u.uy;
		}
		avoid = FALSE;
	} else {
#define	GDIST(x,y)	(dist2(x,y,gx,gy))
	    if ((Is_blackmarket(&u.uz) && u.umonnum>0 &&
		 mons[u.umonnum].mlet != S_HUMAN) ||
                /* WAC Let you out if you're stuck inside */                
                (!Is_blackmarket(&u.uz) && (Invis || u.usteed) && !inside_shop(u.ux, u.uy)))
		{
		    avoid = FALSE;
		} else {
		    uondoor = (u.ux == eshkp->shd.x && u.uy == eshkp->shd.y);
		    if(uondoor) {
			badinv = (carrying(PICK_AXE) || carrying(CONGLOMERATE_PICK) || carrying(CONUNDRUM_PICK) || carrying(MYSTERY_PICK) || carrying(BRONZE_PICK) || carrying(BRICK_PICK) || carrying(NANO_PICK) || carrying(DWARVISH_MATTOCK) || carrying(SOFT_MATTOCK) || carrying(ETERNIUM_MATTOCK) ||
            eshkp->pbanned ||
				  (Fast && (sobj_at(PICK_AXE, u.ux, u.uy) || sobj_at(CONGLOMERATE_PICK, u.ux, u.uy) || sobj_at(CONUNDRUM_PICK, u.ux, u.uy) || sobj_at(MYSTERY_PICK, u.ux, u.uy) || sobj_at(BRONZE_PICK, u.ux, u.uy) || sobj_at(BRICK_PICK, u.ux, u.uy) || sobj_at(NANO_PICK, u.ux, u.uy) || sobj_at(SOFT_MATTOCK, u.ux, u.uy) || sobj_at(ETERNIUM_MATTOCK, u.ux, u.uy) ||
				  sobj_at(DWARVISH_MATTOCK, u.ux, u.uy))));
			if(satdoor && badinv)
			    return(0);
			avoid = !badinv;
		    } else {
			avoid = (*u.ushops && distu(gx,gy) > 8);
			badinv = FALSE;
		    }

		    if(((!eshkp->robbed && !eshkp->billct && !eshkp->debit)
			|| avoid) && GDIST(omx,omy) < 3) {
			if (!badinv && !onlineu(omx,omy))
			    return(0);
			if(satdoor)
			    appr = gx = gy = 0;
		    }
		}
	}

	z = move_special(shkp,inhishop(shkp),appr,uondoor,avoid,omx,omy,gx,gy);
	if (z > 0) after_shk_move(shkp);

	return z;
}

/* called after shopkeeper moves, in case the move causes re-entry into shop */
void
after_shk_move(shkp)
struct monst *shkp;
{
	struct eshk *eshkp = ESHK(shkp);

	if (eshkp->bill_p == (struct bill_x *) -1000 && inhishop(shkp)) {
	    /* reset bill_p, need to re-calc player's occupancy too */
	    eshkp->bill_p = &eshkp->bill[0];
	    check_special_room(FALSE);
	}
}

#endif /*OVL3*/
#ifdef OVLB

/* for use in levl_follower (mondata.c) */
boolean
is_fshk(mtmp)
register struct monst *mtmp;
{
	return((boolean)(mtmp->isshk && ESHK(mtmp)->following));
}

/* You are digging in the shop. */
void
shopdig(fall)
register int fall;
{
    register struct monst *shkp = shop_keeper(*u.ushops);
    int lang;
    const char *grabs = "grabs";

    if(!shkp) return;

    /* 0 == can't speak, 1 == makes animal noises, 2 == speaks */
    lang = 0;
    if (shkp->msleeping || !shkp->mcanmove || is_silent(shkp->data))
	;	/* lang stays 0 */
    else if (shkp->data->msound <= MS_ANIMAL)
	lang = 1;
    else if (shkp->data->msound >= MS_HUMANOID)
	lang = 2;

    if(!inhishop(shkp)) {
	if (Role_if(PM_KNIGHT) || (uwep && uwep->otyp == HONOR_KATANA) || (u.twoweap && uswapwep && uswapwep->otyp == HONOR_KATANA) || Role_if(PM_CHEVALIER) || Role_if(PM_PALADIN)) {
	    You_feel("like a common thief.");
	    adjalign(-sgn(u.ualign.type));
	}
        /* WAC He may not be here now,  but... */
        make_angry_shk(shkp, 0, 0); /* No spot in particular*/
	return;
    }

    if(!fall) {
	if (lang == 2) {
	    if(u.utraptype == TT_PIT)
		verbalize(
			"Be careful, %s, or you might fall through the floor.",
			flags.female ? "madam" : "sir");
	    else
		verbalize("%s, do not damage the floor here!",
			flags.female ? "Madam" : "Sir");
	}
	if (Role_if(PM_KNIGHT) || (uwep && uwep->otyp == HONOR_KATANA) || (u.twoweap && uswapwep && uswapwep->otyp == HONOR_KATANA) || Role_if(PM_CHEVALIER) || Role_if(PM_PALADIN)) {
	    You_feel("like a common thief.");
	    adjalign(-sgn(u.ualign.type));
	}
    } else if(!um_dist(shkp->mx, shkp->my, 5) &&
		!shkp->msleeping && shkp->mcanmove &&
		(ESHK(shkp)->billct || ESHK(shkp)->debit)) {
	    register struct obj *obj, *obj2;
	    if (nolimbs(shkp->data)) {
		grabs = "knocks off";
#if 0
	       /* This is what should happen, but for balance
	        * reasons, it isn't currently.
	        */
		if (lang == 2)
		    pline("%s curses %s inability to grab your backpack!",
			  shkname(shkp), mhim(shkp));
		rile_shk(shkp);
		return;
#endif
	    }
	    if (distu(shkp->mx, shkp->my) > 2) {
		mnexto(shkp);
		/* for some reason the shopkeeper can't come next to you */
		if (distu(shkp->mx, shkp->my) > 2) {
		    if (lang == 2)
			pline("%s curses you in anger and frustration!",
			      shkname(shkp));
		    rile_shk(shkp);
		    return;
		} else
		    pline("%s %s, and %s your backpack!",
			  shkname(shkp),
			  makeplural(locomotion(shkp->data,"leap")), grabs);
	    } else
		pline("%s %s your backpack!", shkname(shkp), grabs);

	    for(obj = invent; obj; obj = obj2) {
		obj2 = obj->nobj;
		if ((obj->owornmask & ~(W_SWAPWEP|W_QUIVER)) != 0 ||
			(obj == uswapwep && u.twoweap) ||
			(obj->otyp == LEATHER_LEASH && obj->leashmon) || (obj->otyp == INKA_LEASH && obj->leashmon) ) continue;
		if (obj == current_wand) continue;
		setnotworn(obj);
		freeinv(obj);
		subfrombill(obj, shkp);
		(void) add_to_minv(shkp, obj);	/* may free obj */
	    }
   } else
        /* WAC He may not be here now,  but... */
        rile_shk(shkp);
}

/* modified by M. Campostrini (campo@sunthpi3.difi.unipi.it) */
/* to allow for multiple choices of kops */
/* modified even more by Amy to allow for even greater choice */
STATIC_OVL void
makekops(mm)
coord *mm;
{
	int kop_cnt[6];        
	int kop_pm[7];
	int ik, cnt;
	coord *mc;
  
	kop_pm[0] = PM_KEYSTONE_KOP;
	kop_pm[1] = PM_KOP_SERGEANT;
	kop_pm[2] = PM_KOP_LIEUTENANT;
	kop_pm[3] = PM_KOP_KAPTAIN;
	kop_pm[4] = PM_KOP_KOMMISSIONER;
	kop_pm[5] = PM_KOP_KCHIEF;
	kop_pm[6] = 0;
  
	cnt = abs(depth(&u.uz)) + rnd(5);
  
	if (Is_blackmarket(&u.uz)) {
	  kop_pm[0] = PM_SOLDIER;
	  kop_pm[1] = PM_SERGEANT;
	  kop_pm[2] = PM_LIEUTENANT;
	  kop_pm[3] = PM_CAPTAIN;
	  kop_pm[4] = 0;
	  kop_pm[5] = 0;
  	  kop_pm[6] = 0;
    
	  cnt = 7 + rnd(5);

	  if (Role_if(PM_CAMPERSTRIKER)) cnt *= (rn2(5) ? 2 : rn2(5) ? 3 : 5);
	}
 
	kop_cnt[0] = cnt;
	kop_cnt[1] = (cnt / 3) + 1;   /* at least one sarge */
	kop_cnt[2] = (cnt / 5) + 1;       /* maybe a lieutenant */
	kop_cnt[3] = (cnt / 8) + 1;       /* and maybe a kaptain */
	kop_cnt[4] = (cnt / 12) + 1;       /* and maybe a kaptain */
 	kop_cnt[5] = (cnt / 16) + 1;       /* and maybe a kaptain */
  
	if (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET) ) {
		kop_cnt[0] = ( kop_cnt[0] / 2) + 1;
		kop_cnt[1] = ( kop_cnt[1] / 2) + 1;
		kop_cnt[2] = ( kop_cnt[2] / 2) + 1;
		kop_cnt[3] = ( kop_cnt[3] / 2) + 1;
		kop_cnt[4] = ( kop_cnt[4] / 2) + 1;
		kop_cnt[5] = ( kop_cnt[5] / 2) + 1;
	}

	if (RngeAntiGovernment) {
		kop_cnt[0] = ( kop_cnt[0] / 2) + 1;
		kop_cnt[1] = ( kop_cnt[1] / 2) + 1;
		kop_cnt[2] = ( kop_cnt[2] / 2) + 1;
		kop_cnt[3] = ( kop_cnt[3] / 2) + 1;
		kop_cnt[4] = ( kop_cnt[4] / 2) + 1;
		kop_cnt[5] = ( kop_cnt[5] / 2) + 1;
	}

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

	mc = (coord *)alloc(cnt * sizeof(coord));
	for (ik=0; kop_pm[ik]; ik++) {
	  /*if (!(mvitals[kop_pm[ik]].mvflags & G_GONE)) {*/
	    cnt = epathto(mc, kop_cnt[ik], mm->x, mm->y, &mons[kop_pm[ik]]);
	    while(--cnt >= 0) {
		switch (rnd(303)) {
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
		(void) makemon(&mons[PM_ANGRY_WATCHMAN], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 11:
		case 12:
		(void) makemon(&mons[PM_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 13:
		(void) makemon(&mons[PM_COPPER_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 14:
		(void) makemon(&mons[PM_TEUTON_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 15:
		(void) makemon(&mons[PM_PAD_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 16:
		(void) makemon(&mons[PM_FRANKISH_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 17:
		(void) makemon(&mons[PM_GAUCHE_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 18:
		(void) makemon(&mons[PM_BRITISH_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 19:
		(void) makemon(&mons[PM_JAVA_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 20:
		(void) makemon(&mons[PM_AMERICAN_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 21:
		(void) makemon(&mons[PM_SWAMP_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 22:
		(void) makemon(&mons[PM_ARAB_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 23:
		(void) makemon(&mons[PM_VIKING_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 24:
		(void) makemon(&mons[PM_ASIAN_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 25:
		(void) makemon(&mons[PM_VANILLA_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 26:
		(void) makemon(&mons[PM_SEAFARING_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 27:
		(void) makemon(&mons[PM_ROHIRRIM_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 28:
		(void) makemon(&mons[PM_BYZANTINE_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 29:
		(void) makemon(&mons[PM_IBERIAN_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 30:
		(void) makemon(&mons[PM_CELTIC_SOLDIER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
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
		case 63:
		case 64:
		case 65:
		case 66:
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		case 86:
		case 87:
		case 88:
		case 89:
		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:
		case 98:
		case 99:
		case 100:
		(void) makemon(&mons[PM_KEYSTONE_KOP], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		(void) makemon(&mons[PM_ANGRY_WATCH_CAPTAIN], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 107:
		case 108:
		case 109:
		case 110:
		(void) makemon(&mons[PM_SERGEANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 111:
		(void) makemon(&mons[PM_TWOWEAP_SERGEANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 112:
		(void) makemon(&mons[PM_EXTRATERRESTRIAL_SERGEANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 113:
		(void) makemon(&mons[PM_MINOAN_SERGEANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 114:
		(void) makemon(&mons[PM_HUN_SERGEANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 115:
		(void) makemon(&mons[PM_MONGOL_SERGEANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 116:
		(void) makemon(&mons[PM_PERSIAN_SERGEANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
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
		case 127:
		case 128:
		case 129:
		case 130:
		case 131:
		case 132:
		case 133:
		case 134:
		case 135:
		case 136:
		case 137:
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
		(void) makemon(&mons[PM_KOP_SERGEANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 151:
		case 152:
		case 153:
		(void) makemon(&mons[PM_ANGRY_WATCH_LIEUTENANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 154:
		case 155:
		(void) makemon(&mons[PM_LIEUTENANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 156:
		(void) makemon(&mons[PM_YAMATO_LIEUTENANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 157:
		(void) makemon(&mons[PM_CARTHAGE_LIEUTENANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 158:
		(void) makemon(&mons[PM_ROMAN_LIEUTENANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 159:
		case 160:
		case 161:
		case 162:
		case 163:
		case 164:
		case 165:
		case 166:
		case 167:
		case 168:
		case 169:
		case 170:
		case 171:
		case 172:
		case 173:
		case 174:
		case 175:
		case 176:
		case 177:
		case 178:
		case 179:
		case 180:
		(void) makemon(&mons[PM_KOP_LIEUTENANT], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 181:
		(void) makemon(&mons[PM_CAPTAIN], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 182:
		(void) makemon(&mons[PM_URBAN_CAMO_CAPTAIN], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 183:
		(void) makemon(&mons[PM_GOTHIC_CAPTAIN], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 184:
		case 185:
		case 186:
		case 187:
		case 188:
		case 189:
		case 190:
		case 191:
		(void) makemon(&mons[PM_KOP_KAPTAIN], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 192:
		(void) makemon(&mons[PM_GENERAL], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 193:
		case 194:
		case 195:
		case 196:
		case 197:
		(void) makemon(&mons[PM_KOP_KOMMISSIONER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 198:
		case 199:
		case 200:
		(void) makemon(&mons[PM_KOP_KCHIEF], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 201:
		(void) makemon(&mons[PM_UNGENOCIDABLE_ARCH_LICH], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 202:
		(void) makemon(&mons[PM_ANGRY_WATCH_LEADER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		case 203:
		(void) makemon(&mons[PM_KOP_KATCHER], mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		default: /* can spawn sephirahs and similar things --Amy */
		(void) makemon(mkclass(S_KOP,0), mc[cnt].x, mc[cnt].y, MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		break;
		} /* switch */

		if (!rn2(100)) {

			int koptryct = 0;
			int kox, koy;

			for (koptryct = 0; koptryct < 2000; koptryct++) {
				kox = rn1(COLNO-3,2);
				koy = rn2(ROWNO);

				if (kox && koy && isok(kox, koy) && (levl[kox][koy].typ > DBWALL) && !(t_at(kox, koy)) ) {
					(void) maketrap(kox, koy, KOP_CUBE, 0, FALSE);
					break;
					}
			}

		}

	    }

	/*}*/
	}

	u.aggravation = 0;

	free((void *)mc);
}

void
pay_for_damage(dmgstr, cant_mollify)
const char *dmgstr;
boolean cant_mollify;
{
	register struct monst *shkp = (struct monst *)0;
	char shops_affected[5];
	register boolean uinshp = (*u.ushops != '\0');
	char qbuf[80];
	register xchar x, y;
	boolean dugwall = !strcmp(dmgstr, "dig into") ||	/* wand */
			  !strcmp(dmgstr, "damage");		/* pick-axe */
	struct damage *tmp_dam, *appear_here = 0;
	/* any number >= (80*80)+(24*24) would do, actually */
	long cost_of_damage = 0L;
	unsigned int nearest_shk = 7000, nearest_damage = 7000;
	int picks = 0;

	for (tmp_dam = level.damagelist;
	     (tmp_dam && (tmp_dam->when == monstermoves));
	     tmp_dam = tmp_dam->next) {
	    char *shp;

	    if (!tmp_dam->cost)
		continue;
	    cost_of_damage += tmp_dam->cost;
	    strcpy(shops_affected,
		   in_rooms(tmp_dam->place.x, tmp_dam->place.y, SHOPBASE));
	    for (shp = shops_affected; *shp; shp++) {
		struct monst *tmp_shk;
		unsigned int shk_distance;

		if (!(tmp_shk = shop_keeper(*shp)))
		    continue;
		if (tmp_shk == shkp) {
		    unsigned int damage_distance =
				   distu(tmp_dam->place.x, tmp_dam->place.y);

		    if (damage_distance < nearest_damage) {
			nearest_damage = damage_distance;
			appear_here = tmp_dam;
		    }
		    continue;
		}
		if (!inhishop(tmp_shk))
		    continue;
		shk_distance = distu(tmp_shk->mx, tmp_shk->my);
		if (shk_distance > nearest_shk)
		    continue;
		if ((shk_distance == nearest_shk) && picks) {
		    if (rn2(++picks))
			continue;
		} else
		    picks = 1;
		shkp = tmp_shk;
		nearest_shk = shk_distance;
		appear_here = tmp_dam;
		nearest_damage = distu(tmp_dam->place.x, tmp_dam->place.y);
	    }
	}

	if (!cost_of_damage || !shkp)
	    return;

	x = appear_here->place.x;
	y = appear_here->place.y;

	/* not the best introduction to the shk... */
	(void) strncpy(ESHK(shkp)->customer,plname,PL_NSIZ);

	/* if the shk is already on the war path, be sure it's all out */
	if(ANGRY(shkp) || ESHK(shkp)->following) {
		hot_pursuit(shkp);
		return;
	}

	/* if the shk is not in their shop.. */
	if(!*in_rooms(shkp->mx,shkp->my,SHOPBASE)) {
		if(!cansee(shkp->mx, shkp->my))
			return;
		goto getcad;
	}

	if(uinshp) {
		if(um_dist(shkp->mx, shkp->my, 1) &&
			!um_dist(shkp->mx, shkp->my, 3)) {
		    pline("%s leaps towards you!", shkname(shkp));
		    mnexto(shkp);
		}
		if(um_dist(shkp->mx, shkp->my, 1)) goto getcad;
	} else {
	    /*
	     * Make shkp show up at the door.  Effect:  If there is a monster
	     * in the doorway, have the hero hear the shopkeeper yell a bit,
	     * pause, then have the shopkeeper appear at the door, having
	     * yanked the hapless critter out of the way.
	     */
	    if (MON_AT(x, y)) {
		if(flags.soundok) {
		    You_hear("an angry voice:");
		    verbalize("Out of my way, scum!");
		    wait_synch();
#if defined(UNIX) || defined(VMS)
# if defined(SYSV) || defined(ULTRIX) || defined(VMS)
		    (void)
# endif
			sleep(1);
#endif
		}
	    }
	    (void) mnearto(shkp, x, y, TRUE);
	}

	if((um_dist(x, y, 1) && !uinshp) || cant_mollify ||
#ifndef GOLDOBJ
	   (u.ugold + ESHK(shkp)->credit) < cost_of_damage
#else
	   (money_cnt(invent) + ESHK(shkp)->credit) < cost_of_damage
#endif
				|| !rn2(50)) {
		if(um_dist(x, y, 1) && !uinshp) {
		    pline("%s shouts:", shkname(shkp));
		    verbalize("Who dared %s my %s?", dmgstr,
					 dugwall ? "shop" : "door");
		} else {
getcad:
		    verbalize("How dare you %s my %s?", dmgstr,
					 dugwall ? "shop" : "door");
		}
		hot_pursuit(shkp);
		return;
	}

	if (Invis) Your("invisibility does not fool %s!", shkname(shkp));
	sprintf(qbuf,"\"Cad!  You did %ld %s worth of damage!\"  Pay? ",
		 cost_of_damage, currency(cost_of_damage));
	if(yn(qbuf) != 'n') {
		cost_of_damage = check_credit(cost_of_damage, shkp);
#ifndef GOLDOBJ
		u.ugold -= cost_of_damage;
		if (rn2(2) || cost_of_damage < 0) shkp->mgold += cost_of_damage;
#else
                money2mon(shkp, cost_of_damage);
#endif
		flags.botl = 1;
		pline("Mollified, %s accepts your restitution.",
			shkname(shkp));
		/* move shk back to his home loc */
		home_shk(shkp, FALSE);
		pacify_shk(shkp);
	} else {
		verbalize("Oh, yes!  You'll pay!");
		hot_pursuit(shkp);
		adjalign(-sgn(u.ualign.type));
	}
}
#endif /*OVLB*/
#ifdef OVL0
/* called in dokick.c when we kick an object that might be in a store */
boolean
costly_spot(x, y)
register xchar x, y;
{
	register struct monst *shkp;

	if (!level.flags.has_shop) return FALSE;
	shkp = shop_keeper(*in_rooms(x, y, SHOPBASE));
	if(!shkp || !inhishop(shkp)) return(FALSE);

	return((boolean)(inside_shop(x, y) &&
		!(x == ESHK(shkp)->shk.x &&
			y == ESHK(shkp)->shk.y)));
}
#endif /*OVL0*/
#ifdef OVLB

/* called by dotalk(sounds.c) when #chatting; returns obj if location
   contains shop goods and shopkeeper is willing & able to speak */
struct obj *
shop_object(x, y)
register xchar x, y;
{
    register struct obj *otmp;
    register struct monst *shkp;

    if(!(shkp = shop_keeper(*in_rooms(x, y, SHOPBASE))) || !inhishop(shkp))
	return(struct obj *)0;

    for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
	if (otmp->oclass != COIN_CLASS)
	    break;
    /* note: otmp might have ->no_charge set, but that's ok */
    return (otmp && costly_spot(x, y) && NOTANGRY(shkp)
	    && shkp->mcanmove && !shkp->msleeping)
		? otmp : (struct obj *)0;
}

/* give price quotes for all objects linked to this one (ie, on this spot) */
void
price_quote(first_obj)
register struct obj *first_obj;
{
    register struct obj *otmp;
    char buf[BUFSZ], price[40];
    long cost;
    int cnt = 0;
    winid tmpwin;
    struct monst *shkp = shop_keeper(inside_shop(u.ux, u.uy));

    tmpwin = create_nhwindow(NHW_MENU);
    putstr(tmpwin, 0, "Fine goods for sale:");
    putstr(tmpwin, 0, "");
    for (otmp = first_obj; otmp; otmp = otmp->nexthere) {
	if (otmp->oclass == COIN_CLASS) continue;
	cost = (otmp->no_charge || otmp == uball || otmp == uchain) ? 0L :
		get_cost(otmp, (struct monst *)0);
	if (Has_contents(otmp))
	    cost += contained_cost(otmp, shkp, 0L, FALSE, FALSE);
	if (!cost) {
	    strcpy(price, "no charge");
	} else {
	    sprintf(price, "%ld %s%s", cost, currency(cost),
		    otmp->quan > 1L ? " each" : "");
	}
	sprintf(buf, "%s, %s", doname(otmp), price);
	putstr(tmpwin, 0, buf),  cnt++;
    }
    if (cnt > 1) {
	display_nhwindow(tmpwin, TRUE);
    } else if (cnt == 1) {
	if (first_obj->no_charge || first_obj == uball || first_obj == uchain){
	    pline("%s!", buf);	/* buf still contains the string */
	} else {
	    /* print cost in slightly different format, so can't reuse buf */
	    cost = get_cost(first_obj, (struct monst *)0);
	    if (Has_contents(first_obj))
		cost += contained_cost(first_obj, shkp, 0L, FALSE, FALSE);
	    pline("%s, price %ld %s%s%s", doname(first_obj),
		cost, currency(cost), first_obj->quan > 1L ? " each" : "",
		shk_embellish(first_obj, cost));
	}
    }
    destroy_nhwindow(tmpwin);
}
#endif /*OVLB*/
#ifdef OVL3

STATIC_OVL const char *
shk_embellish(itm, cost)
register struct obj *itm;
long cost;
{
    if (!rn2(3)) {
	register int o, choice = rn2(5);
	if (choice == 0) choice = (cost < 100L ? 1 : cost < 500L ? 2 : 3);
	switch (choice) {
	    case 4:
		if (cost < 10L) break; else o = itm->oclass;
		if (o == FOOD_CLASS) return ", gourmets' delight!";
		if (objects[itm->otyp].oc_name_known
		    ? objects[itm->otyp].oc_magic
		    : (o == AMULET_CLASS || o == IMPLANT_CLASS || o == RING_CLASS   ||
		       o == WAND_CLASS   || o == POTION_CLASS ||
		       o == SCROLL_CLASS || o == SPBOOK_CLASS))
		    return ", painstakingly developed!";
		return ", superb craftsmanship!";
	    case 3: return ", finest quality.";
	    case 2: return ", an excellent choice.";
	    case 1: return ", a real bargain.";
	   default: break;
	}
    } else if (itm->oartifact) {
	return ", one of a kind!";
    }
    return ".";
}
#endif /*OVL3*/
#ifdef OVLB

/* First 4 supplied by Ronen and Tamar, remainder by development team
 * Amy edit: added two lines about poofy coffee */
const char *Izchak_speaks[]={
    "%s says: 'These shopping malls give me a headache.'",
    "%s says: 'Slow down.  Think clearly.'",
    "%s says: 'You need to take things one at a time.'",
    "%s says: 'I don't like poofy coffee... give me Columbian Supremo.'",
    "%s says: 'I don't like it if the 'poofy' word in my original line is changed to something else.'",
    "%s says: 'If I were still alive, seeing the 'poofy' from my original quote changed would cause me to turn over in my grave.'",
    "%s says that getting the devteam's agreement on anything is difficult.",
    "%s says that he has noticed those who serve their deity will prosper.",
    "%s says: 'Don't try to steal from me - I have friends in high places!'",
    "%s says: 'You may well need something from this shop in the future.'",
    "%s comments about the Valley of the Dead as being a gateway."
};

void
shk_chat(shkp)
struct monst *shkp;
{
	struct eshk *eshk;
#ifdef GOLDOBJ
	long shkmoney;
#endif
	if (!shkp->isshk) {
		/* The monster type is shopkeeper, but this monster is
		   not actually a shk, which could happen if someone
		   wishes for a shopkeeper statue and then animates it.
		   (Note: shkname() would be "" in a case like this.) */
		pline("%s asks whether you've seen any untended shops recently.",
		      Monnam(shkp));
		/* [Perhaps we ought to check whether this conversation
		   is taking place inside an untended shop, but a shopless
		   shk can probably be expected to be rather disoriented.] */
		return;
	}

	eshk = ESHK(shkp);
	if (ANGRY(shkp))
		pline("%s mentions how much %s dislikes %s customers.",
			shkname(shkp), mhe(shkp),
			eshk->robbed ? "non-paying" : "rude");
	else if (eshk->following) {
		if (strncmp(eshk->customer, plname, PL_NSIZ)) {
		    verbalize("%s %s!  I was looking for %s.",
			    Hello(shkp), playeraliasname, eshk->customer);
		    eshk->following = 0;
		} else {
		    verbalize("%s %s!  Didn't you forget to pay?",
			      Hello(shkp), playeraliasname);
		}
	} else if (eshk->billct) {
		register long total = addupbill(shkp) + eshk->debit;
		pline("%s says that your bill comes to %ld %s.",
		      shkname(shkp), total, currency(total));
	} else if (eshk->debit)
		pline("%s reminds you that you owe %s %ld %s.",
		      shkname(shkp), mhim(shkp),
		      eshk->debit, currency(eshk->debit));
	else if (eshk->credit)
		pline("%s encourages you to use your %ld %s of credit.",
		      shkname(shkp), eshk->credit, currency(eshk->credit));
	else if (eshk->robbed)
		pline("%s complains about a recent robbery.", shkname(shkp));
#ifndef GOLDOBJ
	else if (shkp->mgold < 50)
#else
	else if ((shkmoney = money_cnt(shkp->minvent)) < 50)
#endif
		pline("%s complains that business is bad.", shkname(shkp));
#ifndef GOLDOBJ
	else if (shkp->mgold > 4000)
#else
	else if (shkmoney > 4000)
#endif
		pline("%s says that business is good.", shkname(shkp));
	else if (strcmp(shkname(shkp), "Izchak") == 0)
		pline(Izchak_speaks[rn2(SIZE(Izchak_speaks))],shkname(shkp));
	else
		pline("%s talks about the problem of shoplifters.",shkname(shkp));
}

STATIC_OVL void
kops_gone(silent) /* will no longer be called --Amy */
register boolean silent;
{
	register int cnt = 0;
	register struct monst *mtmp, *mtmp2;

	for (mtmp = fmon; mtmp; mtmp = mtmp2) {
	    mtmp2 = mtmp->nmon;
	    if (mtmp->data->mlet == S_KOP) {
		if (canspotmon(mtmp)) cnt++;
		mongone(mtmp);
	    }
	}
	if (cnt && !silent)
	    pline_The("Kop%s (disappointed) vanish%s into thin air.",
		      plur(cnt), cnt == 1 ? "es" : "");
}

#endif /*OVLB*/
#ifdef OVL3

STATIC_OVL long
cost_per_charge(shkp, otmp, altusage)
struct monst *shkp;
struct obj *otmp;
boolean altusage; /* some items have an "alternate" use with different cost */
{
	long tmp = 0L;

	if(!shkp || !inhishop(shkp)) return(0L); /* insurance */
	tmp = get_cost(otmp, shkp);

	/* The idea is to make the exhaustive use of */
	/* an unpaid item more expensive than buying */
	/* it outright.				     */
	/* KMH, balance patch -- removed abusive orbs */
	if(otmp->otyp == MAGIC_LAMP /*|| 
	   otmp->otyp == ORB_OF_DESTRUCTION*/) {           /* 1 */
		/* normal use (ie, as light source) of a magic lamp never
		   degrades its value, but not charging anything would make
		   identifcation too easy; charge an amount comparable to
		   what is charged for an ordinary lamp (don't bother with
		   angry shk surchage) */
		if (!altusage) tmp = (long) objects[OIL_LAMP].oc_cost;
		else tmp += tmp / 3L;	/* djinni is being released */
	} else if(otmp->otyp == MAGIC_MARKER) {		 /* 70 - 100 */
		/* no way to determine in advance   */
		/* how many charges will be wasted. */
		/* so, arbitrarily, one half of the */
		/* price per use.		    */
		tmp /= 2L;
	} else if(otmp->otyp == BAG_OF_TRICKS ||	 /* 1 - 20 */
		  otmp->otyp == MEDICAL_KIT ||                  
		  otmp->otyp == HORN_OF_PLENTY) {
		tmp /= 5L;
	} else if(otmp->otyp == CRYSTAL_BALL ||		 /* 1 - 5 */
		  /*otmp->otyp == ORB_OF_ENCHANTMENT ||                  
		  otmp->otyp == ORB_OF_CHARGING ||*/
		  otmp->otyp == OIL_LAMP ||		 /* 1 - 10 */
		  otmp->otyp == BRASS_LANTERN ||
		 (otmp->otyp >= MAGIC_FLUTE &&
		  otmp->otyp <= DRUM_OF_EARTHQUAKE) ||	 /* 5 - 9 */
		  otmp->oclass == WAND_CLASS) {		 /* 3 - 11 */
		if (otmp->spe > 1) tmp /= 4L;
	} else if (otmp->otyp == TORCH) {
	            tmp /= 2L;	
	} else if (otmp->oclass == SPBOOK_CLASS) {
		/* Normal use is studying. Alternate use is using up a charge */
		if (altusage) tmp /= 10L;		 /* 2 - 4 */
		else tmp -= tmp / 5L;
	} else if (otmp->otyp == CAN_OF_GREASE || otmp->otyp == LUBRICANT_CAN ||
		   otmp->otyp == TINNING_KIT || otmp->otyp == BINNING_KIT
		   || otmp->otyp == EXPENSIVE_CAMERA
		   ) {
		tmp /= 10L;
	} else if (otmp->otyp == POT_OIL) {
		tmp /= 5L;
	}
	return(tmp);
}
#endif /*OVL3*/
#ifdef OVLB

/* Charge the player for partial use of an unpaid object.
 *
 * Note that bill_dummy_object() should be used instead
 * when an object is completely used.
 */
void
check_unpaid_usage(otmp, altusage)
struct obj *otmp;
boolean altusage;
{
	struct monst *shkp;
	const char *fmt, *arg1, *arg2;
	long tmp;

	/* MRKR: Torches are a special case. As weapons they can have */
	/*       a 'charge' == plus value, which is independent of their */
	/*       use as a light source. */

	/* WAC - now checks for items that aren't carried */
	if ((!otmp->unpaid || !*u.ushops ||
		(otmp->spe <= 0 && objects[otmp->otyp].oc_charged &&
		 otmp->otyp != TORCH))
		&& (carried(otmp) || !costly_spot(otmp->ox, otmp->oy) ||
		otmp->no_charge))
	    return;
	if (!(shkp = shop_keeper(*u.ushops)) || !inhishop(shkp))
	    return;
	if ((tmp = cost_per_charge(shkp, otmp, altusage)) == 0L)
	    return;

	arg1 = arg2 = "";
	if (otmp->oclass == SPBOOK_CLASS && !altusage) {
	    fmt = "%sYou owe%s %ld %s.";
	    arg1 = rn2(2) ? "This is no free library, cad!  " : "";
	    arg2 = ESHK(shkp)->debit > 0L ? " an additional" : "";
	} else if (otmp->otyp == POT_OIL) {
	    fmt = "%s%sThat will cost you %ld %s (Yendorian Fuel Tax).";
	} else {
	    fmt = "%s%sUsage fee, %ld %s.";
	    if (!rn2(3)) arg1 = "Hey!  ";
	    if (!rn2(3)) arg2 = "Ahem.  ";
	}

	if (shkp->mcanmove || !shkp->msleeping)
	    verbalize(fmt, arg1, arg2, tmp, currency(tmp));
	ESHK(shkp)->debit += tmp;
	exercise(A_WIS, TRUE);		/* you just got info */
}

/* for using charges of unpaid objects "used in the normal manner" */
void
check_unpaid(otmp)
struct obj *otmp;
{
	check_unpaid_usage(otmp, FALSE);		/* normal item use */
}

void
costly_gold(x, y, amount)
register xchar x, y;
register long amount;
{
	register long delta;
	register struct monst *shkp;
	register struct eshk *eshkp;

	if(!costly_spot(x, y)) return;

	/* shkp now guaranteed to exist by costly_spot() */
	shkp = shop_keeper(*in_rooms(x, y, SHOPBASE));

	if (Race_if(PM_IRAHA) && shkp) {
		verbalize("You just signed your own death warrant, thief!");
		hot_pursuit(shkp);
	}

	eshkp = ESHK(shkp);
	if(eshkp->credit >= amount) {
	    if(eshkp->credit > amount)
		Your("credit is reduced by %ld %s.",
					amount, currency(amount));
	    else Your("credit is erased.");
	    eshkp->credit -= amount;
	} else {
	    delta = amount - eshkp->credit;
	    if(eshkp->credit)
		Your("credit is erased.");
	    if(eshkp->debit)
		Your("debt increases by %ld %s.",
					delta, currency(delta));
	    else You("owe %s %ld %s.",
				shkname(shkp), delta, currency(delta));
	    eshkp->debit += delta;
	    eshkp->loan += delta;
	    eshkp->credit = 0L;
	}
}

/* used in domove to block diagonal shop-exit */
/* x,y should always be a door */
boolean
block_door(x,y)
register xchar x, y;
{
	register int roomno = *in_rooms(x, y, SHOPBASE);
	register struct monst *shkp;

	if(roomno < 0 || !IS_SHOP(roomno)) return(FALSE);
	if(!IS_DOOR(levl[x][y].typ)) return(FALSE);
	if(roomno != *u.ushops) return(FALSE);

	if(!(shkp = shop_keeper((char)roomno)) || !inhishop(shkp))
		return(FALSE);

	if(shkp->mx == ESHK(shkp)->shk.x && shkp->my == ESHK(shkp)->shk.y
	    /* Actually, the shk should be made to block _any_
	     * door, including a door the player digs, if the
	     * shk is within a 'jumping' distance.
	     */
	    && ESHK(shkp)->shd.x == x && ESHK(shkp)->shd.y == y
	    && shkp->mcanmove && !shkp->msleeping
	    && (ESHK(shkp)->debit || ESHK(shkp)->billct ||
		ESHK(shkp)->robbed)) {
		pline("%s%s blocks your way!", shkname(shkp),
				Invis ? " senses your motion and" : "");
		return(TRUE);
	}
	return(FALSE);
}

/* used in domove to block diagonal shop-entry */
/* u.ux, u.uy should always be a door */
boolean
block_entry(x,y)
register xchar x, y;
{
	register xchar sx, sy;
	register int roomno;
	register struct monst *shkp;

	if(!(IS_DOOR(levl[u.ux][u.uy].typ) &&
		levl[u.ux][u.uy].doormask == D_BROKEN)) return(FALSE);

	roomno = *in_rooms(x, y, SHOPBASE);
	if(roomno < 0 || !IS_SHOP(roomno)) return(FALSE);
	if(!(shkp = shop_keeper((char)roomno)) || !inhishop(shkp))
		return(FALSE);

	if(ESHK(shkp)->shd.x != u.ux || ESHK(shkp)->shd.y != u.uy)
		return(FALSE);

	sx = ESHK(shkp)->shk.x;
	sy = ESHK(shkp)->shk.y;

	/* KMH, balacne patch -- allow other picks */
	if(shkp->mx == sx && shkp->my == sy
		&& shkp->mcanmove && !shkp->msleeping
		&& (x == sx-1 || x == sx+1 || y == sy-1 || y == sy+1)
		&& (Invis || carrying(PICK_AXE) || carrying(CONGLOMERATE_PICK) || carrying(CONUNDRUM_PICK) || carrying(MYSTERY_PICK) || carrying(BRONZE_PICK) || carrying(BRICK_PICK) || carrying(NANO_PICK) || carrying(DWARVISH_MATTOCK) || carrying(SOFT_MATTOCK) || carrying(ETERNIUM_MATTOCK) || u.usteed
	  )) {
		pline("%s%s blocks your way!", shkname(shkp),
				Invis ? " senses your motion and" : "");
		return(TRUE);
	}
	return(FALSE);
}

#endif /* OVLB */
#ifdef OVL2

char *
shk_your(buf, obj)
char *buf;
struct obj *obj;
{
	if (!shk_owns(buf, obj) && !mon_owns(buf, obj))
	    strcpy(buf, carried(obj) ? "your" : "the");
	return buf;
}

char *
Shk_Your(buf, obj)
char *buf;
struct obj *obj;
{
	(void) shk_your(buf, obj);
	*buf = highc(*buf);
	return buf;
}

STATIC_OVL char *
shk_owns(buf, obj)
char *buf;
struct obj *obj;
{
	struct monst *shkp;
	xchar x, y;

	if (get_obj_location(obj, &x, &y, 0) &&
	    (obj->unpaid ||
	     (obj->where==OBJ_FLOOR && !obj->no_charge && costly_spot(x,y)))) {
	    shkp = shop_keeper(inside_shop(x, y));
	    return strcpy(buf, shkp ? s_suffix(shkname(shkp)) : "the");
	}
	return (char *)0;
}

STATIC_OVL char *
mon_owns(buf, obj)
char *buf;
struct obj *obj;
{
	if (obj->where == OBJ_MINVENT)
	    return strcpy(buf, s_suffix(mon_nam(obj->ocarry)));
	return (char *)0;
}

#endif /* OVL2 */
#ifdef OVLB

#ifdef __SASC
void
sasc_bug(struct obj *op, unsigned x){
	op->unpaid=x;
}
#endif

static NEARDATA const char identify_types[] = { ALL_CLASSES, 0 };
static NEARDATA const char weapon_types[] = { WEAPON_CLASS, TOOL_CLASS, CHAIN_CLASS, BALL_CLASS, GEM_CLASS, 0 };
static NEARDATA const char armor_types[] = { ARMOR_CLASS, 0 };

/*
** FUNCTION shk_identify
**
** Pay the shopkeeper to identify an item.
*/
static NEARDATA const char ident_chars[] = "bp";

static void
shk_identify(slang, shkp)
	char *slang;
	struct monst *shkp;
{
	register struct obj *obj;       /* The object to identify       */
	int charge, mult;               /* Cost to identify             */
/*
	char sbuf[BUFSZ];
 */
	boolean guesswork;              /* Will shkp be guessing?       */
	boolean ripoff=FALSE;           /* Shkp ripping you off?        */
	char ident_type;

	/* Pick object */
	if ( !(obj = getobj(identify_types, "have identified"))) return;

	/* Will shk be guessing? */
        if ((guesswork = !shk_obj_match(obj, shkp)))
	{
		verbalize("I don't handle that sort of item, but I could try...");
	}

	/* Here we go */
	/* KMH -- fixed */
	if ((ESHK(shkp)->services & (SHK_ID_BASIC|SHK_ID_PREMIUM)) ==
			(SHK_ID_BASIC|SHK_ID_PREMIUM)) {
		ident_type = yn_function("[B]asic service or [P]remier",
		     ident_chars, '\0');
		if (ident_type == '\0') return;
	} else if (ESHK(shkp)->services & SHK_ID_BASIC) {
		verbalize("I only offer basic identification.");
		ident_type = 'b';
	} else if (ESHK(shkp)->services & SHK_ID_PREMIUM) {
		verbalize("I only make complete identifications.");
		ident_type = 'p';
	}

	/*
	** Shopkeeper is ripping you off if:
	** Basic service and object already known.
	** Premier service, object known, + know blessed/cursed and
	**      rustproof, etc.
	*/
	if (obj->dknown && objects[obj->otyp].oc_name_known)
	{
		if (ident_type=='b') ripoff=TRUE;
		if (ident_type=='p' && obj->bknown && obj->rknown && obj->known) ripoff=TRUE;
	}

	/* Compute the charge */
	
	if (ripoff)
	{
		if (no_cheat) {
			verbalize("That item's already identified!");
			return;
		}
		/* Object already identified: Try and cheat the customer. */
		pline("%s chuckles greedily...", mon_nam(shkp));
		mult = 1;
	
	/* basic */        
	} else if (ident_type=='b') mult = 1;

	/* premier */
	else mult = 2;
	
	switch (obj->oclass) {        
		case AMULET_CLASS:      charge = 375 * mult;
					break;
		case IMPLANT_CLASS:      charge = 500 * mult;
					break;
		case WEAPON_CLASS:      charge = 75 * mult;
					break;
		case ARMOR_CLASS:       charge = 100 * mult;
					break;
		case FOOD_CLASS:        charge = 25 * mult;   
					break;
		case SCROLL_CLASS:      charge = 150 * mult;   
					break;
		case SPBOOK_CLASS:      charge = 250 * mult;   
					break;
		case POTION_CLASS:      charge = 150 * mult;   
					break;
		case RING_CLASS:        charge = 300 * mult;   
					break;
		case WAND_CLASS:        charge = 200 * mult;   
					break;
		case TOOL_CLASS:        charge = 50 * mult;   
					break;
		case GEM_CLASS:         charge = 500 * mult;
					break;
		default:                charge = 75 * mult;
					break;
	}
		
	/* Artifacts cost more to deal with */
	/* KMH -- Avoid floating-point */
	if (obj->oartifact) charge = charge * 3 / 2;
	
	/* Smooth out the charge a bit (lower bound only) */
	shk_smooth_charge(&charge, 25, NOBOUND);
	
	/* Go ahead? */
	if (shk_offer_price(slang, charge, shkp) == FALSE) return;

	/* evil patch idea: buying the same service many times will eventually cause the shk to run out. --Amy */

	if (!rn2(10)) {
		if (ident_type == 'b') ESHK(shkp)->services &= ~SHK_ID_BASIC;
		if (ident_type == 'p') ESHK(shkp)->services &= ~SHK_ID_PREMIUM;
	}

	/* Shopkeeper deviousness */
	if (ident_type == 'b') {
	    if (Hallucination) {
		pline("You hear %s tell you it's a pot of flowers.",
			mon_nam(shkp));
		return;
	    } else if (Confusion && !Conf_resist) {
		pline("%s tells you but you forget.", mon_nam(shkp));
		return;
	    }
	}

	/* Is shopkeeper guessing? */
	if (guesswork)
	{
		/*
		** Identify successful if rn2() < #.  
		*/
		if (!rn2(ident_type == 'b' ? 4 : 2)) {
			verbalize("Success! Let's try to identify your item...");
			/* Rest of msg will come from identify();
			 * known quirk: the item may resist the identification attempt, this is not a bug
			 * I added some extra message to make it clear that success isn't really guaranteed --Amy */
		} else {
			verbalize("Sorry.  I guess it's not your lucky day.");
			return;
		}
	}

	/* Premier service */
	if (ident_type == 'p') {
		if (obj->oclass == SCROLL_CLASS && rnd(u.idscrollpenalty) > 100) pline("The scroll resisted your identification attempt!");
		else if (obj->oclass == POTION_CLASS && rnd(u.idpotionpenalty) > 3) pline("The potion resisted your identification attempt!");
		else if (obj->oclass == RING_CLASS && (!(obj->owornmask & W_RING) || ((rnd(u.idringpenalty) > 4) && (rnd(u.idringpenalty) > 4)) ) && rnd(u.idringpenalty) > 4) pline("The ring resisted your identification attempt!");
		else if (obj->oclass == AMULET_CLASS && (!(obj->owornmask & W_AMUL) || ((rnd(u.idamuletpenalty) > 15) && (rnd(u.idamuletpenalty) > 15)) )&& rnd(u.idamuletpenalty) > 15) pline("The amulet resisted your identification attempt!");
		else if (obj->oclass == IMPLANT_CLASS && (!(obj->owornmask & W_IMPLANT) || ((rnd(u.idimplantpenalty) > 1) && (rnd(u.idimplantpenalty) > 1)) )&& rnd(u.idimplantpenalty) > 1) pline("The implant resisted your identification attempt!");
		else if (obj->oclass == WAND_CLASS && rnd(u.idwandpenalty) > 3) pline("The wand resisted your identification attempt!");
		else if (obj->oclass == ARMOR_CLASS && (!(obj->owornmask & W_ARMOR) || ((rnd(u.idarmorpenalty) > 15) && (rnd(u.idarmorpenalty) > 15)) ) && rnd(u.idarmorpenalty) > 15) pline("The armor resisted your identification attempt!");
		else if (obj->oclass == SPBOOK_CLASS && rnd(u.idspellbookpenalty) > 2) pline("The spellbook resisted your identification attempt!");
		else if (obj->oclass == GEM_CLASS && rnd(u.idgempenalty) > 100) pline("The gem resisted your identification attempt!");
		else if (obj->oclass == TOOL_CLASS && rnd(u.idtoolpenalty) > 5) pline("The tool resisted your identification attempt!");
		else makeknown(obj->otyp);
		identify(obj);
	} else { 
		/* Basic */
		if (obj->oclass == SCROLL_CLASS && rnd(u.idscrollpenalty) > 100) pline("The scroll resisted your identification attempt!");
		else if (obj->oclass == POTION_CLASS && rnd(u.idpotionpenalty) > 3) pline("The potion resisted your identification attempt!");
		else if (obj->oclass == RING_CLASS && (!(obj->owornmask & W_RING) || ((rnd(u.idringpenalty) > 4) && (rnd(u.idringpenalty) > 4)) ) && rnd(u.idringpenalty) > 4) pline("The ring resisted your identification attempt!");
		else if (obj->oclass == AMULET_CLASS && (!(obj->owornmask & W_AMUL) || ((rnd(u.idamuletpenalty) > 15) && (rnd(u.idamuletpenalty) > 15)) )&& rnd(u.idamuletpenalty) > 15) pline("The amulet resisted your identification attempt!");
		else if (obj->oclass == IMPLANT_CLASS && (!(obj->owornmask & W_IMPLANT) || ((rnd(u.idimplantpenalty) > 1) && (rnd(u.idimplantpenalty) > 1)) )&& rnd(u.idimplantpenalty) > 1) pline("The implant resisted your identification attempt!");
		else if (obj->oclass == WAND_CLASS && rnd(u.idwandpenalty) > 3) pline("The wand resisted your identification attempt!");
		else if (obj->oclass == ARMOR_CLASS && (!(obj->owornmask & W_ARMOR) || ((rnd(u.idarmorpenalty) > 15) && (rnd(u.idarmorpenalty) > 15)) ) && rnd(u.idarmorpenalty) > 15) pline("The armor resisted your identification attempt!");
		else if (obj->oclass == SPBOOK_CLASS && rnd(u.idspellbookpenalty) > 2) pline("The spellbook resisted your identification attempt!");
		else if (obj->oclass == GEM_CLASS && rnd(u.idgempenalty) > 100) pline("The gem resisted your identification attempt!");
		else if (obj->oclass == TOOL_CLASS && rnd(u.idtoolpenalty) > 5) pline("The tool resisted your identification attempt!");
		else makeknown(obj->otyp);
		obj->dknown = 1;
    		prinv((char *)0, obj, 0L); /* Print result */
	}
}


/*
** FUNCTION shk_uncurse
**
** Uncurse an item for the customer
*/
static void
shk_uncurse(slang, shkp)
	char *slang;
	struct monst *shkp;
{
	struct obj *obj;                /* The object picked            */
	int charge;                     /* How much to uncurse          */
	boolean guesswork;              /* Will shkp be guessing?       */

	/* Pick object */
	if ( !(obj = getobj(identify_types, "uncurse"))) return;

	/* Will shk be guessing? */
        if ((guesswork = !shk_obj_match(obj, shkp)))
	{
		verbalize("I don't handle that sort of item, but I could try...");
	}

	/* Charge is same as cost */
	charge = get_cost(obj, shop_keeper(/* roomno= */*u.ushops));
	charge *= 3; /* uncursing shouldn't be possible for peanuts! --Amy */
	if (charge < 300) charge = 300; /* uncursing REALLY shouldn't be possible for peanuts */

	/* Artifacts cost more to deal with */
	/* KMH -- Avoid floating-point */
	if (obj->oartifact) charge = charge * 3 / 2;

	/* Smooth out the charge a bit */
	shk_smooth_charge(&charge, 50, NOBOUND);

	/* Go ahead? */
	if (shk_offer_price(slang, charge, shkp) == FALSE) return;

	if (!rn2(5)) { /* curses should not be meaningless --Amy */
		ESHK(shkp)->services &= ~SHK_UNCURSE;
	}

	/* Shopkeeper responses */
	/* KMH -- fixed bknown, curse(), bless(), uncurse() */
	if (!obj->bknown && !Role_if(PM_PRIEST) && !Role_if(PM_NECROMANCER) && !Role_if(PM_CHEVALIER) &&
	    !no_cheat)
	{
		/* Not identified! */
		pline("%s snickers and says \"See, nice and uncursed!\"",
			mon_nam(shkp));
		obj->bknown = FALSE;
	}
	else if (Confusion)
	{
		if (rn2(10)) {
			You("accidentally point to the wrong item in your confusion.");
		} else {
			/* Curse the item! */
			You("accidentally ask for the item to be cursed");
			if (!stack_too_big(obj)) {
				curse(obj);
				if (obj->cursed && !rn2(3)) obj->hvycurse = TRUE;
				if (obj->cursed && !rn2(20)) obj->stckcurse = TRUE;
				if (obj->cursed && !rn2(100)) obj->prmcurse = TRUE;
			}
			else pline("But the stack was so big that the shopkeeper failed to curse it.");
		}
	}
	else if (Hallucination)
	{
		/*
		** Let's have some fun:  If you're hallucinating,
		** then there's a chance for the object to be blessed!
		*/
		if (!rn2(10)) {
			pline("Distracted by your blood-shot %s, the shopkeeper",
			makeplural(body_part(EYE)));
			pline("accidentally curses the item!");
			if (!stack_too_big(obj)) {
				curse(obj);
				if (obj->cursed && !rn2(3)) obj->hvycurse = TRUE;
				if (obj->cursed && !rn2(20)) obj->stckcurse = TRUE;
				if (obj->cursed && !rn2(100)) obj->prmcurse = TRUE;
			}
			else pline("But the stack was so big that the shopkeeper failed to curse it.");
		} else if (!rn2(10))
		{
		    pline("Distracted by your blood-shot %s, the shopkeeper",
			makeplural(body_part(EYE)));
		    pline("accidentally blesses the item!");
		    if (!stack_too_big(obj)) bless(obj);
		    else pline("But the stack was so big that the blessing failed.");
		}
		else
		{
			You("can't see straight and point to the wrong item");
		}
	}
	/* Is shopkeeper guessing? */
	else if (guesswork) /* ported from identify function by Amy, because it makes no sense that they can uncurse everything */
	{
		/*
		** Uncurse successful 1 out of 5 times.  
		*/
		if (!rn2(5)) {
			verbalize("Success!");
			if (!stack_too_big(obj)) uncurse(obj, TRUE);
			else pline("Whoops, sorry, actually no success because the stack was too big!");
		} else {
			verbalize("Sorry.  I guess it's not your lucky day.");
			return;
		}
	}
	else
	{
		verbalize("All done - safe to handle, now!");
		if (!stack_too_big(obj)) uncurse(obj, TRUE);
		else pline("But the stack was so big that the shopkeeper failed to uncurse it.");
	}
}

/*
** FUNCTION shk_bless
**
** Bless an item for the customer, by Amy, based on the uncurse code above
*/
static void
shk_bless(slang, shkp)
	char *slang;
	struct monst *shkp;
{
	struct obj *obj;                /* The object picked            */
	int charge;                     /* How much to uncurse          */
	boolean guesswork;              /* Will shkp be guessing?       */

	/* Pick object */
	if ( !(obj = getobj(identify_types, "bless"))) return;

	/* Will shk be guessing? */
        if ((guesswork = !shk_obj_match(obj, shkp)))
	{
		verbalize("I don't handle that sort of item, but I could try...");
	}

	/* Charge is same as cost */
	charge = get_cost(obj, shop_keeper(/* roomno= */*u.ushops));
	charge *= 5; /* blessing shouldn't be possible for peanuts! --Amy */
	if (charge < 500) charge = 500; /* uncursing REALLY shouldn't be possible for peanuts */

	/* Artifacts cost more to deal with */
	/* KMH -- Avoid floating-point */
	if (obj->oartifact) charge = charge * 3 / 2;

	/* Smooth out the charge a bit */
	shk_smooth_charge(&charge, 50, NOBOUND);

	/* Go ahead? */
	if (shk_offer_price(slang, charge, shkp) == FALSE) return;

	if (!rn2(5)) {
		ESHK(shkp)->services &= ~SHK_BLESS;
	}

	/* Shopkeeper responses */
	/* KMH -- fixed bknown, curse(), bless(), uncurse() */
	if (!obj->bknown && !Role_if(PM_PRIEST) && !Role_if(PM_NECROMANCER) && !Role_if(PM_CHEVALIER) &&
	    !no_cheat)
	{
		/* Not identified! */
		pline("%s snickers and says \"Look, I blessed your item!\"", mon_nam(shkp));
		obj->bknown = FALSE;
	}
	else if (obj->cursed) { /* this is not a "super-uncurse" service - it only works on uncursed stuff --Amy */
		verbalize("The blessing magic only works on items that aren't cursed! Get it uncursed first!");
	}
	else if (Confusion)
	{
		if (rn2(10)) {
			You("accidentally point to the wrong item in your confusion.");
		} else {
			/* Curse the item! */
			You("accidentally ask for the item to be cursed");
			if (!stack_too_big(obj)) {
				curse(obj);
				if (obj->cursed && !rn2(3)) obj->hvycurse = TRUE;
				if (obj->cursed && !rn2(20)) obj->stckcurse = TRUE;
				if (obj->cursed && !rn2(100)) obj->prmcurse = TRUE;
			}
			else pline("But the stack was so big that the shopkeeper failed to curse it.");
		}
	}

	/* no special effect if you hallucinate */

	/* Is shopkeeper guessing? */
	else if (guesswork) /* ported from identify function by Amy, because it makes no sense that they can bless everything */
	{
		/*
		** Blessing successful 1 out of 5 times.  
		*/
		if (!rn2(5)) {
			verbalize("Success!");
			if (!stack_too_big(obj)) bless(obj);
			else pline("Whoops, sorry, actually no success because the stack was too big!");
		} else {
			verbalize("Sorry.  I guess it's not your lucky day.");
			return;
		}
	}
	else
	{
		verbalize("All done - the gods themselves will watch over your item now!");
		if (!stack_too_big(obj)) bless(obj);
		else pline("But the stack was so big that the shopkeeper failed to bless it.");
	}
}


/*
** FUNCTION shk_appraisal
**
** Appraise a weapon or armor
*/
static const char basic_damage[] =
	"Basic damage against small foes %s, against large foes %s.";

static void
shk_appraisal(slang, shkp)
	char *slang;
	struct monst *shkp;
{
	struct obj *obj;                /* The object picked            */
	int charge;                     /* How much for appraisal       */
	boolean guesswork;              /* Shopkeeper unsure?           */
	char ascii_wsdam[5];            /* Ascii form of damage         */
	char ascii_wldam[5];


	/* Pick object */
	if ( !(obj = getobj(weapon_types, "appraise"))) return;

	charge = get_cost(obj, shop_keeper(/* roomno= */*u.ushops)) / 3;

	/* Smooth out the charge a bit */
	shk_smooth_charge(&charge, 5, NOBOUND);

	/* If not identified, complain. */
	/* KMH -- Why should it matter? */
/*	if ( ! (obj->known && objects[obj->otyp].oc_name_known) )
	{
		verbalize("This weapon needs to be identified first!");
		return;
	} else */
	if (shk_class_match(WEAPON_CLASS, shkp) == SHK_MATCH)
	{
		verbalize("Ok, %s, let's see what we have here.", slang);
		guesswork = FALSE;
	}
	else
	{
		verbalize("Mind you, I'm not an expert in this field.");
		guesswork = TRUE;
	}

	/* Go ahead? */
	if (shk_offer_price(slang, charge, shkp) == FALSE) return;

	if (!rn2(1000)) {
		ESHK(shkp)->services &= ~SHK_APPRAISE;
	}

	/* Shopkeeper deviousness */
	if (Confusion && !Conf_resist)
	{
		pline("The numbers get all mixed up in your head.");
		return;
	}
	else if (Hallucination)
	{
		You("hear %s say it'll \"knock 'em dead\"",
			mon_nam(shkp));
		return;
	}

	/* Convert damage to ascii */
	sprintf(ascii_wsdam, "%d", objects[obj->otyp].oc_wsdam);
	sprintf(ascii_wldam, "%d", objects[obj->otyp].oc_wldam);

	/* Will shopkeeper be unsure? */
	if (guesswork)
	{
		switch (rn2(10))
		{
		    case 1:
			/* Shkp's an idiot */
			verbalize("Sorry, %s, but I'm not certain.", slang);
			break;

		    case 2:
			/* Not sure about large foes */
			verbalize(basic_damage, ascii_wsdam, "?");
			break;

		    case 3:
			/* Not sure about small foes */
			verbalize(basic_damage, "?", ascii_wldam);
			break;

		    default:
			verbalize(basic_damage, ascii_wsdam, ascii_wldam);
			if (!issoviet) {
				obj->known = TRUE;
				if (u.weapchantrecskill < 1 || !rn2(u.weapchantrecskill)) {
					u.weapchantrecskill++;
					if (u.weapchantrecskill > 250) u.weapchantrecskill = 250;
				}
				verbalize("It is %s", doname(obj));
			}
			else pline("Sovetskiy khochet, chtoby vse bylo der'mo, dazhe izmeneniya, kotoryye, ochevidno, vygodny, schitayutsya im zlymi, potomu chto Emi sdelala ikh. Takim obrazom, plyus vashego oruzhiya ne oboznachen khar khar!");

			break;
			
		}
	}
	else
	{
		verbalize(basic_damage, ascii_wsdam, ascii_wldam);

		/* Appraisal was too useless for way too long, so I decided to make it more useful. After all,
		 * the appraisal technique also identifies the + of a weapon, so the service should do so too. --Amy
		 * In Soviet Russia, nothing is ever allowed to be different from regular SLASH'EM, because SLASHTHEM
		 * basically is SLASH'EM with some very minor changes. */

		if (!issoviet) {
			obj->known = TRUE;
			if (u.weapchantrecskill < 1 || !rn2(u.weapchantrecskill)) {
				u.weapchantrecskill++;
				if (u.weapchantrecskill > 250) u.weapchantrecskill = 250;
			}
			verbalize("It is %s", doname(obj));
		}
		else pline("Sovetskiy khochet, chtoby vse bylo der'mo, dazhe izmeneniya, kotoryye, ochevidno, vygodny, schitayutsya im zlymi, potomu chto Emi sdelala ikh. Takim obrazom, plyus vashego oruzhiya ne oboznachen khar khar!");
	}
}


/*
** FUNCTION shk_weapon_works
**
** Perform ops on weapon for customer
*/
static const char we_offer[] = "We offer the finest service available!";
static void
shk_weapon_works(slang, shkp)
char *slang;
struct monst *shkp;
{
    struct obj *obj;
    int charge;
    winid tmpwin;
    anything any;
    menu_item *selected;
    int service;
    int n;

    /* Pick weapon */
    if (ESHK(shkp)->services & (SHK_SPECIAL_A | SHK_SPECIAL_B))
	obj = getobj(weapon_types, "improve");
    else
	obj = getobj(weapon_types, "poison");
    if (!obj) return;

    /* Check if you asked for a non weapon tool to be improved */
    if (obj->oclass == TOOL_CLASS && !is_weptool(obj))
	pline("%s grins greedily...", mon_nam(shkp));

    if (ESHK(shkp)->services & (SHK_SPECIAL_A | SHK_SPECIAL_B)) {
	any.a_void = 0;         /* zero out all bits */
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);

	if (ESHK(shkp)->services & SHK_SPECIAL_A) {
	    any.a_int = 1;
	    add_menu(tmpwin, NO_GLYPH, &any , 'w', 0, ATR_NONE,
		    "Ward against damage", MENU_UNSELECTED);
	}
	if (ESHK(shkp)->services & SHK_SPECIAL_B) {
	    any.a_int = 2;
	    add_menu(tmpwin, NO_GLYPH, &any , 'e', 0, ATR_NONE,
		    "Enchant", MENU_UNSELECTED);
	}

	/* Can object be poisoned? */
	if (ESHK(shkp)->services & SHK_SPECIAL_C) {
	    any.a_int = 3;
	    add_menu(tmpwin, NO_GLYPH, &any , 'p', 0, ATR_NONE,
		    "Poison", MENU_UNSELECTED);
	}

	end_menu(tmpwin, "Weapon-works:");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	destroy_nhwindow(tmpwin);
	if (n > 0)
	    service = selected[0].item.a_int;
	else
	    service = 0;
    } else
	service = 3;

    /* Here we go */
    if (service > 0)
	verbalize(we_offer);
    else {
	pline("%s", Never_mind);
	if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
		pline("Oh wait, actually I do mind...");
		badeffect();
	}

    }

    switch(service) {
	case 0:
	    break;

	case 1:
	    verbalize("This'll leave your %s untouchable!", xname(obj));
	    
	    /* Costs more the more eroded it is (oeroded 0-3 * 2) */
	    charge = 5000;
	    charge += ((obj->oeroded + obj->oeroded2) * 500);
	    if (obj->oeroded + obj->oeroded2 > 2)
		verbalize("This thing's in pretty sad condition, %s", slang);

	    /* Another warning if object is naturally rustproof */
	    if (obj->oerodeproof || !is_damageable(obj))
		pline("%s gives you a suspciously happy smile...",
			mon_nam(shkp));

	    /* Artifacts cost more to deal with */
	    if (obj->oartifact) charge = charge * 3 / 2;

	    /* Smooth out the charge a bit */
	    shk_smooth_charge(&charge, 200, NOBOUND);

	    if (shk_offer_price(slang, charge, shkp) == FALSE) return;

		if (!rn2(10)) {
			ESHK(shkp)->services &= ~SHK_SPECIAL_A;
		}

	    /* Have some fun, but for this $$$ it better work. */
	    if (Confusion)
		You("fall over in appreciation");
	    else if (Hallucination)
		Your(" - tin roof, un-rusted!");

	    if (!stack_too_big(obj)) {obj->oeroded = obj->oeroded2 = 0;
	    obj->rknown = TRUE;
	    obj->oerodeproof = TRUE;
		} else pline("But it failed due to the stack being too big!");
	    break;

	case 2:
	    verbalize("Guaranteed not to harm your weapon, or your money back!");
	    /*
	    ** The higher the enchantment, the more costly!
	    ** Gets to the point where you need to rob fort ludios
	    ** in order to get it to +5!!
	    */
	    charge = (obj->spe+1) * (obj->spe+1) * 625 * (obj->spe+1 > 5 ? (obj->spe+1) : 1);
		/* removed upper limit, but enchanting stuff beyond the former limit is much more expensive --Amy */

	    if (obj->spe < 0) charge = 100;

	    /* Artifacts cost more to deal with */
	    if (obj->oartifact) charge *= 2;

	    /* Smooth out the charge a bit (lower bound only) */
	    shk_smooth_charge(&charge, 50, NOBOUND);

	    if (shk_offer_price(slang, charge, shkp) == FALSE) return;

		if (!rn2(50)) {
			ESHK(shkp)->services &= ~SHK_SPECIAL_B;
		}

	    /*if (obj->spe+1 > 5) { 
		verbalize("I can't enchant this any higher!");
		charge = 0;
		break;
	    }*/
	    /* Have some fun! */
	    if (Confusion)
		Your("%s unexpectedly!", aobjnam(obj, "vibrate"));
	    else if (Hallucination)
		Your("%s to evaporate into thin air!", aobjnam(obj, "seem"));
	    /* ...No actual vibrating and no evaporating */

	    if (obj->otyp == WORM_TOOTH && !stack_too_big(obj) ) {
		obj->otyp = CRYSKNIFE;
		Your("weapon seems sharper now.");

		if ((obj->morgcurse || obj->evilcurse || obj->bbrcurse) && !rn2(100) ) {
			obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
		}
		else if (obj->prmcurse && !(obj->morgcurse || obj->evilcurse || obj->bbrcurse) && !rn2(10) ) {
			obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
		}
		else if (!(obj->prmcurse) && !(obj->morgcurse || obj->evilcurse || obj->bbrcurse) && obj->hvycurse && !rn2(3) ) {
			obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
		}
		else if (!(obj->prmcurse) && !(obj->hvycurse) && !(obj->morgcurse || obj->evilcurse || obj->bbrcurse) ) obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;

		break;
	    }

	    if (!stack_too_big(obj)) obj->spe++;
	    else pline("The enchantment failed because the stack was too big.");
	    break;

	case 3:

	    if (!is_poisonable(obj)) {
		verbalize("That cannot be poisoned!");
		break;
	    }

	    verbalize("Just imagine what poisoned %s can do!", xname(obj));

	    charge = 10 * obj->quan;

	    if (shk_offer_price(slang, charge, shkp) == FALSE) return;

		if (!rn2(100)) {
			ESHK(shkp)->services &= ~SHK_SPECIAL_C;
		}

	    obj->opoisoned = TRUE;
	    break;

	default:
	    impossible("Unknown Weapon Enhancement");
	    break;
    }
}


/*
** FUNCTION shk_armor_works
**
** Perform ops on armor for customer
*/
static void
shk_armor_works(slang, shkp)
	char *slang;
	struct monst *shkp;
{
	struct obj *obj;
	int charge;
/*WAC - Windowstuff*/
	winid tmpwin;
	anything any;
	menu_item *selected;
	int n;

	/* Pick armor */
	if ( !(obj = getobj(armor_types, "improve"))) return;

	/* Here we go */
	/*WAC - did this using the windowing system...*/
	any.a_void = 0;         /* zero out all bits */
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_int = 1;
	if (ESHK(shkp)->services & (SHK_SPECIAL_A))
		add_menu(tmpwin, NO_GLYPH, &any , 'r', 0, ATR_NONE, "Rust/Fireproof", MENU_UNSELECTED);
	any.a_int = 2;
	if (ESHK(shkp)->services & (SHK_SPECIAL_B))
		add_menu(tmpwin, NO_GLYPH, &any , 'e', 0, ATR_NONE, "Enchant", MENU_UNSELECTED);
	end_menu(tmpwin, "Armor-works:");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	destroy_nhwindow(tmpwin);

	verbalize(we_offer);

	if (n > 0)
	switch(selected[0].item.a_int) {
		case 1:
		if (!flags.female && is_human(youmonst.data))
		     verbalize("They'll call you the man of stainless steel!");

		/* Costs more the more rusty it is (oeroded 0-3), Amy edit: oeroded2 is also fixed */
		charge = 3000;
		charge += (obj->oeroded * 300);
		charge += (obj->oeroded2 * 300);
		if ((obj->oeroded + obj->oeroded2) > 2) verbalize("Yikes!  This thing's a mess!");

		/* Artifacts cost more to deal with */
		/* KMH -- Avoid floating-point */
		if (obj->oartifact) charge = charge * 3 / 2;
		
		/* Smooth out the charge a bit */
		shk_smooth_charge(&charge, 100, NOBOUND);

		if (shk_offer_price(slang, charge, shkp) == FALSE) return;

		if (!rn2(10)) {
			ESHK(shkp)->services &= ~SHK_SPECIAL_A;
		}

		/* Have some fun, but for this $$$ it better work. */
		if (Confusion)
			You("forget how to put your %s back on!", xname(obj));
		else if (Hallucination)
			You("mistake your %s for a pot and...", xname(obj));

		obj->oeroded = 0;
		obj->oeroded2 = 0;
		obj->rknown = TRUE;
		obj->oerodeproof = TRUE;
		break;

		case 2:
		verbalize("Nobody will ever hit on you again.");
 
		/* Higher enchantment levels cost more. */
		charge = (obj->spe+1) * (obj->spe+1) * 500 * (obj->spe+1 > 3 ? (obj->spe+1) : 1);
		/* removed upper limit, but enchanting stuff beyond the former limit is much more expensive --Amy */
						
		if (obj->spe < 0) charge = 100;                

		/* Artifacts cost more to deal with */
		if (obj->oartifact) charge *= 2;
		
		/* Smooth out the charge a bit */
		shk_smooth_charge(&charge, 50, NOBOUND);

		if (shk_offer_price(slang, charge, shkp) == FALSE) return;

		if (!rn2(50)) {
			ESHK(shkp)->services &= ~SHK_SPECIAL_B;
		}

		/*if (obj->spe+1 > 3) { 
			verbalize("I can't enchant this any higher!");
			charge = 0;
			break;
			}*/
		 /* Have some fun! */
		if (Hallucination) Your("%s looks dented.", xname(obj));
		
		if (obj->otyp >= GRAY_DRAGON_SCALES &&
					obj->otyp <= YELLOW_DRAGON_SCALES) {
			/* dragon scales get turned into dragon scale mail */
			Your("%s merges and hardens!", xname(obj));
			setworn((struct obj *)0, W_ARM);
			/* assumes same order */
			obj->otyp = GRAY_DRAGON_SCALE_MAIL +
						obj->otyp - GRAY_DRAGON_SCALES;

			if ((obj->morgcurse || obj->evilcurse || obj->bbrcurse) && !rn2(100) ) {
				obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
			}
			else if (obj->prmcurse && !(obj->morgcurse || obj->evilcurse || obj->bbrcurse) && !rn2(10) ) {
				obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
			}
			else if (!(obj->prmcurse) && !(obj->morgcurse || obj->evilcurse || obj->bbrcurse) && obj->hvycurse && !rn2(3) ) {
				obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
			}
			else if (!(obj->prmcurse) && !(obj->hvycurse) && !(obj->morgcurse || obj->evilcurse || obj->bbrcurse) ) obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;

			obj->known = 1;
			setworn(obj, W_ARM);
			break;
		}

		if (obj->otyp == LIZARD_SCALES) {
			Your("%s merges and hardens!", xname(obj));
			setworn((struct obj *)0, W_ARM);
			obj->otyp = LIZARD_SCALE_MAIL;
			if ((obj->morgcurse || obj->evilcurse || obj->bbrcurse) && !rn2(100) ) {
				obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
			}
			else if (obj->prmcurse && !(obj->morgcurse || obj->evilcurse || obj->bbrcurse) && !rn2(10) ) {
				obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
			}
			else if (!(obj->prmcurse) && !(obj->morgcurse || obj->evilcurse || obj->bbrcurse) && obj->hvycurse && !rn2(3) ) {
				obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
			}
			else if (!(obj->prmcurse) && !(obj->hvycurse) && !(obj->morgcurse || obj->evilcurse || obj->bbrcurse) ) obj->prmcurse = obj->hvycurse = obj->cursed = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;

			obj->known = 1;
			setworn(obj, W_ARM);
			break;
		}

		obj->spe++;
		break;

	    default:
                pline ("Unknown Armor Enhancement");
                break;
	}
}


/*
** FUNCTION shk_charge
**
** Charge something (for a price!)
*/
static NEARDATA const char wand_types[] = { WAND_CLASS, 0 };
static NEARDATA const char tool_types[] = { TOOL_CLASS, 0 };
static NEARDATA const char ring_types[] = { RING_CLASS, IMPLANT_CLASS, 0 };
static NEARDATA const char spbook_types[] = { SPBOOK_CLASS, 0 };

static void
shk_charge(slang, shkp)
	char *slang;
	struct monst *shkp;
{
	struct obj *obj = NULL; /* The object picked            */
	struct obj *tobj;       /* Temp obj                     */
	char type;              /* Basic/premier service        */
	int charge;             /* How much to charge customer  */
	char invlet;            /* Inventory letter             */

	/* What type of shop are we? */
	if (shk_class_match(WAND_CLASS, shkp) == SHK_MATCH)
		obj = getobj(wand_types, "charge");
	else if (shk_class_match(TOOL_CLASS, shkp) == SHK_MATCH)
		obj = getobj(tool_types, "charge");
	else if (shk_class_match(RING_CLASS, shkp) == SHK_MATCH)
		obj = getobj(ring_types, "charge");
	else if (shk_class_match(SPBOOK_CLASS, shkp) == SHK_MATCH)
		obj = getobj(spbook_types, "charge");
	if (!obj) return;

	/*
	** Wand shops can offer special service!
	** Extra charges (for a lot of extra money!)
	* Amy edit: fuck you, why only wands? other items can also be either uncursed- or blessed-charged by scrolls,
	* so why the everloving fuck should only wand shops get that?
	*/

	/* What type of service? */
	if ((ESHK(shkp)->services & (SHK_SPECIAL_A|SHK_SPECIAL_B)) ==
			(SHK_SPECIAL_A|SHK_SPECIAL_B)) {
		type = yn_function("[B]asic service or [P]remier",
				ident_chars, '\0');
		if (type == '\0') return;
	} else if (ESHK(shkp)->services & SHK_SPECIAL_A) {
		pline ("I only perform basic charging.");
		type = 'b';
	} else if (ESHK(shkp)->services & SHK_SPECIAL_B) {
		pline ("I only perform complete charging.");
		type = 'p';
	}

	/* Compute charge */
	if (type == 'b')
		charge = 3000;
	else
		charge = 10000;

	/* Wands of wishing should be hard to get recharged */
	if (obj->otyp == WAN_WISHING || obj->otyp == WAN_ACQUIREMENT)
		charge *= 3;
	else /* Smooth out the charge a bit */
		shk_smooth_charge(&charge, 100, NOBOUND);

	/* Go for it? */
	if (shk_offer_price(slang, charge, shkp) == FALSE) return;

	if (!rn2(15)) {
		if (type == 'b') ESHK(shkp)->services &= ~SHK_SPECIAL_A;
		if (type == 'p') ESHK(shkp)->services &= ~SHK_SPECIAL_B;
	}

	/* Shopkeeper deviousness */
	if (( (Confusion && !Conf_resist) || Hallucination) && !no_cheat)
	{
		pline("%s says it's charged and pushes you toward the door",
			Monnam(shkp));
		return;
	}

	/* Do it */
	invlet = obj->invlet;
	recharge(obj, (type=='b') ? 0 : 1);

	/*
	** Did the object blow up?  We need to check this in a way
	** that has nothing to do with dereferencing the obj pointer.
	** We saved the inventory letter of this item; now cycle
	** through all objects and see if there is an object
	** with that letter.
	*/
	for(obj=0, tobj=invent; tobj; tobj=tobj->nobj)
		if(tobj->invlet == invlet)
		{
			obj = tobj;
			break;
		}
	if (!obj)
	{
		verbalize("Oops!  Sorry about that...");
		return;
	}

	/* Wands get special treatment */
	if (obj->oclass == WAND_CLASS)
	{
		/* Wand of wishing? */
		if (obj->otyp == WAN_WISHING || obj->otyp == WAN_CHARGING || obj->otyp == WAN_BAD_EQUIPMENT || obj->otyp == WAN_ACQUIREMENT)
		{
			/* Premier gives you ONE more charge */
			/* KMH -- Okay, but that's pretty generous */
			if (type == 'p') obj->spe++;

			/* Fun */
			if (obj->otyp == WAN_WISHING) {
				verbalize("Since you'll have everything you always wanted,");
				verbalize("...How about loaning me some money?");
#ifndef GOLDOBJ
				if (rn2(2)) shkp->mgold += u.ugold;
				u.ugold = 0;
#else
				money2mon(shkp, money_cnt(invent));
#endif
				makeknown(obj->otyp);
			}
			bot();
		}
		else if (type == 'p')
		{ 
			/*
			** Basic: recharge() will have given 1 charge.
			** Premier: recharge() will have given 5-10, say.
			** Add a few more still. Note by Amy: but only if the player uses premier charging.
			*/
			if (obj->spe < 120) obj->spe += (obj->otyp == WAN_GENOCIDE ? rnd(2) : obj->otyp == WAN_GAIN_LEVEL ? rnd(2) : obj->otyp == WAN_INCREASE_MAX_HITPOINTS ? rnd(3) : rnd(5));
			/*else if (obj->spe < 20) obj->spe += 1;*/
		}
	}
}

/* establish credit via shopkeeper service by Amy: always available on any shopkeeper, but disappears if you exceed
 * your credit limit. This is a way to establish credit without the shk cheating you, but the gold is deleted; this is
 * intentional because that way you can't steal it back or otherwise get stuff for nothing, credit cloner. :P */
static void
shk_estcredit(slang, shkp)
	char *slang;
	struct monst *shkp;
{
	char buf[BUFSZ];
	long offer;
	struct eshk *eshkp = ESHK(shkp);

	if (!u.ugold) {
		pline("It seems that you have no money.");
		return;
	}

	getlin("How much credit do you want to establish?", buf);
	if (sscanf(buf, "%ld", &offer) != 1) offer = 0L;

	if (offer < 0L) {
		pline("Enter a positive number, please.");
		return;
	} else if (offer == 0L) {
		pline("You've changed your mind.");
		return;
	} else if (offer > u.ugold) {
		You("don't have that much!");
		return;
	} else {
		if ((eshkp->totalcredit + offer) > eshkp->creditlimit) {
			offer = (eshkp->creditlimit - eshkp->totalcredit);
			ESHK(shkp)->services &= ~SHK_CREDITSRV;
			verbalize("That would exceed your credit limit! I'll only accept %ld zorkmids.", offer);
		}
		u.ugold -= offer;
		eshkp->totalcredit += offer;
		eshkp->credit += offer;
		verbalize("Your total credit amounts to %ld zorkmids now. Thank you!", eshkp->credit);
	}
}

/*
** FUNCTION shk_obj_match
**
** Does object "obj" match the type of shop?
*/
static boolean
shk_obj_match(obj, shkp)
	struct obj *obj;
	struct monst *shkp;
{
	/* object matches type of shop? */
	return(saleable(shkp, obj));
}


/*
** FUNCTION shk_offer_price
**
** Tell customer how much it'll cost, ask if he wants to pay,
** and deduct from $$ if agreable.
*/
static boolean
shk_offer_price(slang, charge, shkp)
	char *slang;
	long charge;
	struct monst *shkp;
{
	char sbuf[BUFSZ];
	long credit = ESHK(shkp)->credit;

	/* Ask y/n if player wants to pay */
        sprintf(sbuf, "It'll cost you %ld zorkmid%s.  Interested?",
		charge, plur(charge));

	if ( yn(sbuf) != 'y' ) {
		verbalize("It's your call, %s.", slang);
		return(FALSE);
	}

	/* Player _wants_ to pay, but can he? */
	/* WAC -- Check the credit:  but don't use check_credit
	 * since we don't want to charge him for part of it if he can't pay for all 
	 * of it 
	 */
#ifndef GOLDOBJ
	if (charge > (u.ugold + credit)) {
#else
	if (charge > (money_cnt(invent) + credit)) {  
#endif
		verbalize("Cash on the spot, %s, and you ain't got the dough!",
			slang);
		return(FALSE);
	}

	/* Charge the customer */
	charge = check_credit (charge, shkp); /* Deduct the credit first */

#ifndef GOLDOBJ
	u.ugold -= charge;
	if (rn2(2) || charge < 0) shkp->mgold += charge;
#else
	money2mon(shkp, charge);
#endif
	bot();

	/* here's us throwing a bone to lawful politicians or evilvariant characters --Amy */
	if (u.ualign.type == A_LAWFUL) adjalign(1);
	u.cnd_shkserviceamount++;

	/* rarely, purchasing a service with very low CHA can increase it --Amy */
	if (ABASE(A_CHA) < 10) {
		int chachance = 100;
		switch (ABASE(A_CHA)) {
			case 4: chachance = 150; break;
			case 5: chachance = 200; break;
			case 6: chachance = 250; break;
			case 7: chachance = 300; break;
			case 8: chachance = 350; break;
			case 9: chachance = 400; break;
			default: {
				if (ABASE(A_CHA) < 4) chachance = 100;
				else chachance = 500;
				break;
			}
		}
		if (!rn2(chachance)) (void) adjattrib(A_CHA, 1, FALSE, TRUE);
	}

	return(TRUE);
}


/*
** FUNCTION shk_smooth_charge
**
** Smooth out the lower/upper bounds on the price to get something
** done.  Make sure that it (1) varies depending on charisma and
** (2) is constant.
*/
static void
shk_smooth_charge(pcharge, lower, upper)
	int *pcharge;
	int lower;
	int upper;
{
	int charisma;
	int bonus;

	/* KMH -- Avoid using floating-point arithmetic */
	     if(ACURR(A_CHA) > 21) *pcharge *= 11;
	else if(ACURR(A_CHA) > 18) *pcharge *= 12;
	else if(ACURR(A_CHA) > 15) *pcharge *= 13;
	else if(ACURR(A_CHA) > 12) *pcharge *= 14;
	else if(ACURR(A_CHA) > 10) *pcharge *= 15;
	else if(ACURR(A_CHA) > 8)  *pcharge *= 16;
	else if(ACURR(A_CHA) > 7)  *pcharge *= 17;
	else if(ACURR(A_CHA) > 6)  *pcharge *= 18;
	else if(ACURR(A_CHA) > 5)  *pcharge *= 19;
	else if(ACURR(A_CHA) > 4)  *pcharge *= 20;
	else *pcharge *= 21;
	*pcharge /= 10;

	if (Is_blackmarket(&u.uz)) *pcharge *= 3;
	/* Skip upper stuff? */
	if (upper == NOBOUND) goto check_lower;

	/* This should give us something like a charisma of 5 to 25. */
	charisma = ABASE(A_CHA) + ABON(A_CHA) + ATEMP(A_CHA);        

	/* Now: 0 to 10 = 0.  11 and up = 1 to whatever. */
	if (charisma <= 10)
		charisma = 0;
	else
		charisma -= 10;

	/* Charismatic players get smaller upper bounds */
	bonus=((upper/50)*charisma);

	/* Adjust upper.  Upper > lower! */
	upper -= bonus;
	upper = (upper>=lower) ? upper : lower;

	/* Ok, do the min/max stuff */
	if (*pcharge > upper) *pcharge=upper;
check_lower:
	if (*pcharge < lower) *pcharge=lower;
}



#endif /* OVLB */

#ifdef DEBUG
int
wiz_debug_cmd()	/* in this case, display your bill(s) */
{
    int win, special = 0;
    struct obj *obj;
    struct monst *shkp, *ushkp;
    struct bill_x *bp;
    int ct;
    char buf[BUFSIZ];
    char buf2[BUFSIZ];

    win = create_nhwindow(NHW_MENU);
    ushkp = shop_keeper(*u.ushops);
    shkp = next_shkp(fmon, TRUE);
    if (!shkp) {
	shkp = ushkp;
	special++;
    }
    if (!shkp)
	putstr(win, 0, "No shopkeepers with bills");
    else
	for (; shkp; ) {
	    bp = ESHK(shkp)->bill_p;
	    ct = ESHK(shkp)->billct;
	    if (ct) {
		sprintf(buf, "Your bill with %s", noit_mon_nam(shkp));
		if (shkp == ushkp) {
		    strcat(buf, " (here)");
		    ushkp = NULL;
		}
		strcat(buf, ":");
		putstr(win, 0, buf);
		putstr(win, 0, "Price   Quan    Used?   Object");
		while (--ct >= 0) {
		    obj = bp_to_obj(bp);
		    if (obj) {
			if (!obj->unpaid)
			    *buf2='*';		/* Bad entry */
			strcpy(obj->unpaid ? buf2 : buf2 + 1, xname(obj));
		    }
		    else
			sprintf(buf2, "Unknown, with ID %d", bp->bo_id);
		    sprintf(buf, "%-7d %-7d %-7s %s", bp->price, bp->bquan,
		      bp->useup ? "Yes" : "No", buf2);
		    putstr(win, 0, buf);
		    bp++;
		}
	    }
	    else {
		sprintf(buf, "You do not owe %s anything.", noit_mon_nam(shkp));
		putstr(win, 0, buf);
	    }
	    if (special)
		break;
	    shkp = next_shkp(shkp->nmon, TRUE);
	    if (!shkp) {
		shkp = ushkp;
		special++;
	    }
	    if (shkp)
		putstr(win, 0, "");
	}
    display_nhwindow(win, FALSE);
    destroy_nhwindow(win);
    return 0;
}
#endif	/* DEBUG */

/*shk.c*/

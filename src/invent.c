/*	SCCS Id: @(#)invent.c	3.4	2003/12/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#define NOINVSYM	'#'
#define CONTAINED_SYM	'>'	/* designator for inside a container */

#ifdef OVL1
STATIC_DCL void NDECL(reorder_invent);
STATIC_DCL boolean FDECL(mergable,(struct obj *,struct obj *));
STATIC_DCL void FDECL(invdisp_nothing, (const char *,const char *));
STATIC_DCL boolean FDECL(worn_wield_only, (struct obj *));
STATIC_DCL boolean FDECL(only_here, (struct obj *));
#endif /* OVL1 */
STATIC_DCL void FDECL(compactify,(char *));
STATIC_DCL boolean FDECL(taking_off, (const char *));
STATIC_DCL boolean FDECL(putting_on, (const char *));
STATIC_PTR int FDECL(ckunpaid,(struct obj *));
STATIC_PTR int FDECL(ckvalidcat,(struct obj *));
static char FDECL(display_pickinv, (const char *,BOOLEAN_P, long *));
#ifdef OVLB
STATIC_DCL boolean FDECL(this_type_only, (struct obj *));
STATIC_DCL void NDECL(dounpaid);
STATIC_DCL struct obj *FDECL(find_unpaid,(struct obj *,struct obj **));
STATIC_DCL void FDECL(menu_identify, (int));
STATIC_DCL boolean FDECL(tool_in_use, (struct obj *));
#endif /* OVLB */
STATIC_DCL char FDECL(obj_to_let,(struct obj *));
STATIC_DCL int FDECL(itemactions,(struct obj *));

/* define for getobj() */
#define FOLLOW(curr, flags) \
    (((flags) & BY_NEXTHERE) ? (curr)->nexthere : (curr)->nobj)

#ifdef OVLB

static int lastinvnr = 51;	/* 0 ... 51 (never saved&restored) */

#ifdef WIZARD
/* wizards can wish for venom, which will become an invisible inventory
 * item without this.  putting it in inv_order would mean venom would
 * suddenly become a choice for all the inventory-class commands, which
 * would probably cause mass confusion.  the test for inventory venom
 * is only WIZARD and not wizard because the wizard can leave venom lying
 * around on a bones level for normal players to find.
 */
static char venom_inv[] = { VENOM_CLASS, 0 };	/* (constant) */
#endif

void
assigninvlet(otmp)
register struct obj *otmp;
{
	boolean inuse[52];
	register int i;
	register struct obj *obj;

#ifdef GOLDOBJ
        /* There is only one of these in inventory... */        
        if (otmp->oclass == COIN_CLASS) {
	    otmp->invlet = GOLD_SYM;
	    return;
	}
#endif

	for(i = 0; i < 52; i++) inuse[i] = FALSE;
	for(obj = invent; obj; obj = obj->nobj) if(obj != otmp) {
		i = obj->invlet;
		if('a' <= i && i <= 'z') inuse[i - 'a'] = TRUE; else
		if('A' <= i && i <= 'Z') inuse[i - 'A' + 26] = TRUE;
		if(i == otmp->invlet) otmp->invlet = 0;
	}
	if((i = otmp->invlet) &&
	    (('a' <= i && i <= 'z') || ('A' <= i && i <= 'Z')))
		return;
	for(i = lastinvnr+1; i != lastinvnr; i++) {
		if(i == 52) { i = -1; continue; }
		if(!inuse[i]) break;
	}
	otmp->invlet = (inuse[i] ? NOINVSYM :
			(i < 26) ? ('a'+i) : ('A'+i-26));
	lastinvnr = i;
}

#endif /* OVLB */
#ifdef OVL1

/* note: assumes ASCII; toggling a bit puts lowercase in front of uppercase */
#define inv_rank(o) ((o)->invlet ^ 040)

/* sort the inventory; used by addinv() and doorganize() */
STATIC_OVL void
reorder_invent()
{
	struct obj *otmp, *prev, *next;
	boolean need_more_sorting;

	do {
	    /*
	     * We expect at most one item to be out of order, so this
	     * isn't nearly as inefficient as it may first appear.
	     */
	    need_more_sorting = FALSE;
	    for (otmp = invent, prev = 0; otmp; ) {
		next = otmp->nobj;
		if (next && inv_rank(next) < inv_rank(otmp)) {
		    need_more_sorting = TRUE;
		    if (prev) prev->nobj = next;
		    else      invent = next;
		    otmp->nobj = next->nobj;
		    next->nobj = otmp;
		    prev = next;
		} else {
		    prev = otmp;
		    otmp = next;
		}
	    }
	} while (need_more_sorting);
}

#undef inv_rank

/* KMH, balance patch -- Idea by Wolfgang von Hansen <wvh@geodesy.inka.de>.
 * Harmless to character, yet deliciously evil.
 * Somewhat expensive, so don't use it often.
 *
 * Some players who depend upon fixinv complained.  They take damage
 * instead.
 */
int jumble_pack ()
{
	register struct obj *obj, *nobj, *otmp;
	register char let;
	register int dmg = 0;


	for (obj = invent; obj; obj = nobj)
	{
		nobj = obj->nobj;
		if (rn2(10))
			/* Skip it */;
		else if (flags.invlet_constant)
			dmg += 2;
		else {
			/* Remove it from the inventory list (but don't touch the obj) */
			extract_nobj(obj, &invent);

			/* Determine the new letter */
			let = rnd(52) + 'A';
			if (let > 'Z')
				let = let - 'Z' + 'a' - 1;

			/* Does another object share this letter? */
			for (otmp = invent; otmp; otmp = otmp->nobj)
				if (otmp->invlet == let)
					otmp->invlet = obj->invlet;

			/* Add the item back into the inventory */
			obj->invlet = let;
			obj->nobj = invent; /* insert at beginning */
			obj->where = OBJ_INVENT;
			invent = obj;
		}
	}

	/* Clean up */
	reorder_invent();
	return (dmg);
}


/* scan a list of objects to see whether another object will merge with
   one of them; used in pickup.c when all 52 inventory slots are in use,
   to figure out whether another object could still be picked up */
struct obj *
merge_choice(objlist, obj)
struct obj *objlist, *obj;
{
	struct monst *shkp;
	int save_nocharge;

	if (obj->otyp == SCR_SCARE_MONSTER)	/* punt on these */
	    return (struct obj *)0;
	/* if this is an item on the shop floor, the attributes it will
	   have when carried are different from what they are now; prevent
	   that from eliciting an incorrect result from mergable() */
	save_nocharge = obj->no_charge;
	if (objlist == invent && obj->where == OBJ_FLOOR &&
		(shkp = shop_keeper(inside_shop(obj->ox, obj->oy))) != 0) {
	    if (obj->no_charge) obj->no_charge = 0;
	    /* A billable object won't have its `unpaid' bit set, so would
	       erroneously seem to be a candidate to merge with a similar
	       ordinary object.  That's no good, because once it's really
	       picked up, it won't merge after all.  It might merge with
	       another unpaid object, but we can't check that here (depends
	       too much upon shk's bill) and if it doesn't merge it would
	       end up in the '#' overflow inventory slot, so reject it now. */
	    else if (inhishop(shkp)) return (struct obj *)0;
	}
	while (objlist) {
	    if (mergable(objlist, obj)) break;
	    objlist = objlist->nobj;
	}
	obj->no_charge = save_nocharge;
	return objlist;
}

/* merge obj with otmp and delete obj if types agree */
int
merged(potmp, pobj)
struct obj **potmp, **pobj;
{
	register struct obj *otmp = *potmp, *obj = *pobj;

	if(mergable(otmp, obj)) {
		/* Approximate age: we do it this way because if we were to
		 * do it "accurately" (merge only when ages are identical)
		 * we'd wind up never merging any corpses.
		 * otmp->age = otmp->age*(1-proportion) + obj->age*proportion;
		 *
		 * Don't do the age manipulation if lit.  We would need
		 * to stop the burn on both items, then merge the age,
		 * then restart the burn.
		 */
		if (!obj->lamplit)
		    otmp->age = ((otmp->age*otmp->quan) + (obj->age*obj->quan))
			    / (otmp->quan + obj->quan);

		otmp->quan += obj->quan;
#ifdef GOLDOBJ
                /* temporary special case for gold objects!!!! */
#endif
		if (otmp->oclass == COIN_CLASS) otmp->owt = weight(otmp);
		else otmp->owt += obj->owt;
		if(!otmp->onamelth && obj->onamelth)
			otmp = *potmp = oname(otmp, ONAME(obj));
		obj_extract_self(obj);

		/* really should merge the timeouts */
		if (obj->lamplit) obj_merge_light_sources(obj, otmp);
		if (obj->timed) obj_stop_timers(obj);	/* follows lights */

		/* fixup for `#adjust' merging wielded darts, daggers, &c */
		if (obj->owornmask && carried(otmp)) {
		    long wmask = otmp->owornmask | obj->owornmask;

		    /* Both the items might be worn in competing slots;
		       merger preference (regardless of which is which):
			 primary weapon + alternate weapon -> primary weapon;
			 primary weapon + quiver -> primary weapon;
			 alternate weapon + quiver -> alternate weapon.
		       (Prior to 3.3.0, it was not possible for the two
		       stacks to be worn in different slots and `obj'
		       didn't need to be unworn when merging.) */
		    if (wmask & W_WEP) wmask = W_WEP;
		    else if (wmask & W_SWAPWEP) wmask = W_SWAPWEP;
		    else if (wmask & W_QUIVER) wmask = W_QUIVER;
		    else {
			impossible("merging strangely worn items (%lx)", wmask);
			wmask = otmp->owornmask;
		    }
		    if ((otmp->owornmask & ~wmask) != 0L) setnotworn(otmp);
		    setworn(otmp, wmask);
		    setnotworn(obj);
		}
#if 0
		/* (this should not be necessary, since items
		    already in a monster's inventory don't ever get
		    merged into other objects [only vice versa]) */
		else if (obj->owornmask && mcarried(otmp)) {
		    if (obj == MON_WEP(otmp->ocarry)) {
			MON_WEP(otmp->ocarry) = otmp;
			otmp->owornmask = W_WEP;
		    }
		}
#endif /*0*/

		obfree(obj,otmp);	/* free(obj), bill->otmp */
		return(1);
	}
	return 0;
}

/*
Adjust hero intrinsics as if this object was being added to the hero's
inventory.  Called _before_ the object has been added to the hero's
inventory.

This is called when adding objects to the hero's inventory normally (via
addinv) or when an object in the hero's inventory has been polymorphed
in-place.

It may be valid to merge this code with with addinv_core2().
*/
void
addinv_core1(obj)
struct obj *obj;
{
	if (obj->oclass == COIN_CLASS) {
#ifndef GOLDOBJ
		u.ugold += obj->quan;
#else
		flags.botl = 1;
#endif
	} else if (obj->otyp == AMULET_OF_YENDOR) {
		if (u.uhave.amulet) impossible("already have amulet?");
		u.uhave.amulet = 1;
#ifdef RECORD_ACHIEVE
                achieve.get_amulet = 1;
#ifdef LIVELOGFILE
		livelog_achieve_update();
#endif
#endif
	} else if (obj->otyp == CANDELABRUM_OF_INVOCATION) {
		if (u.uhave.menorah) impossible("already have candelabrum?");
		u.uhave.menorah = 1;
#ifdef RECORD_ACHIEVE
                achieve.get_candelabrum = 1;
#ifdef LIVELOGFILE
		livelog_achieve_update();
#endif
#endif
	} else if (obj->otyp == BELL_OF_OPENING) {
		if (u.uhave.bell) impossible("already have silver bell?");
		u.uhave.bell = 1;
#ifdef RECORD_ACHIEVE
                achieve.get_bell = 1;
#ifdef LIVELOGFILE
		livelog_achieve_update();
#endif
#endif
	} else if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
		if (u.uhave.book) impossible("already have the book?");
		u.uhave.book = 1;
#ifdef RECORD_ACHIEVE
                achieve.get_book = 1;
#ifdef LIVELOGFILE
		livelog_achieve_update();
#endif
#endif
	} else if (obj->oartifact) {
		if (is_quest_artifact(obj)) {
		    if (u.uhave.questart)
			impossible("already have quest artifact?");
		    u.uhave.questart = 1;
		    artitouch();
		}
		if(obj->oartifact == ART_TREASURY_OF_PROTEUS){
			u.ukinghill = TRUE;
		}
		set_artifact_intrinsic(obj, 1, W_ART);
	
	}

#ifdef RECORD_ACHIEVE
        if(obj->otyp == LUCKSTONE && obj->record_achieve_special) {
                achieve.get_luckstone = 1;
                obj->record_achieve_special = 0;
#ifdef LIVELOGFILE
		livelog_achieve_update();
#endif
        } else if((obj->otyp == AMULET_OF_REFLECTION ||
                   obj->otyp == BAG_OF_HOLDING) &&
                  obj->record_achieve_special) {
                achieve.finish_sokoban = 1;
                obj->record_achieve_special = 0;
#ifdef LIVELOGFILE
		livelog_achieve_update();
#endif
        }
#endif /* RECORD_ACHIEVE */

}

/*
Adjust hero intrinsics as if this object was being added to the hero's
inventory.  Called _after_ the object has been added to the hero's
inventory.

This is called when adding objects to the hero's inventory normally (via
addinv) or when an object in the hero's inventory has been polymorphed
in-place.
*/
void
addinv_core2(obj)
struct obj *obj;
{
	if (confers_luck(obj)) {
		/* new luckstone must be in inventory by this point
		 * for correct calculation */
		set_moreluck();
	}

	/* KMH, balance patch -- recalculate health if you've gained healthstones */
	if (obj->otyp == HEALTHSTONE)
		recalc_health();

}

/*
Add obj to the hero's inventory.  Make sure the object is "free".
Adjust hero attributes as necessary.
*/
struct obj *
addinv(obj)
struct obj *obj;
{
	struct obj *otmp, *prev;

	if (obj->where != OBJ_FREE)
	    panic("addinv: obj not free");
	obj->no_charge = 0;	/* not meaningful for invent */

	addinv_core1(obj);
#ifndef GOLDOBJ
	/* if handed gold, we're done */
	if (obj->oclass == COIN_CLASS)
	    return obj;
#endif

	/* merge if possible; find end of chain in the process */
	for (prev = 0, otmp = invent; otmp; prev = otmp, otmp = otmp->nobj)
	    if (merged(&otmp, &obj)) {
		obj = otmp;
		goto added;
	    }
	/* didn't merge, so insert into chain */
	if (flags.invlet_constant || !prev) {
	    if (flags.invlet_constant) assigninvlet(obj);
	    obj->nobj = invent;		/* insert at beginning */
	    invent = obj;
	    if (flags.invlet_constant) reorder_invent();
	} else {
	    prev->nobj = obj;		/* insert at end */
	    obj->nobj = 0;
	}
	obj->where = OBJ_INVENT;

added:
	addinv_core2(obj);
	carry_obj_effects(&youmonst, obj); /* carrying affects the obj */
	update_inventory();
	return(obj);
}
/*
 * Some objects are affected by being carried.
 * Make those adjustments here. Called _after_ the object
 * has been added to the hero's or monster's inventory,
 * and after hero's intrinsics have been updated.
 */
void
carry_obj_effects(mon, obj)
struct monst *mon;
struct obj *obj;
{
	/* Cursed figurines can spontaneously transform
	   when carried. */
	if (obj->otyp == FIGURINE) {
		if (obj->cursed
	    	    && obj->corpsenm != NON_PM
	    	    && !dead_species(obj->corpsenm,TRUE)) {
			attach_fig_transform_timeout(obj);
		    }
	}
	else if (obj->otyp == TORCH && obj->lamplit) {
	  /* MRKR: extinguish torches before putting them */
	  /*       away. Should monsters do the same?  */

	  if (mon == &youmonst) {
	    You("extinguish %s before putting it away.", 
		yname(obj));
	    end_burn(obj, TRUE);
	  }
	}	
}

#endif /* OVL1 */
#ifdef OVLB

/* Add an item to the inventory unless we're fumbling or it refuses to be
 * held (via touch_artifact), and give a message.
 * If there aren't any free inventory slots, we'll drop it instead.
 * If both success and failure messages are NULL, then we're just doing the
 * fumbling/slot-limit checking for a silent grab.  In any case,
 * touch_artifact will print its own messages if they are warranted.
 */
struct obj *
hold_another_object(obj, drop_fmt, drop_arg, hold_msg)
struct obj *obj;
const char *drop_fmt, *drop_arg, *hold_msg;
{
	char buf[BUFSZ];

#ifndef INVISIBLE_OBJECTS
	if (!Blind) obj->dknown = 1;	/* maximize mergibility */
#else
	if (!Blind && (!obj->oinvis || See_invisible)) obj->dknown = 1;
#endif
	if (obj->oartifact) {
	    /* place_object may change these */
	    boolean crysknife = (obj->otyp == CRYSKNIFE);
	    int oerode = obj->oerodeproof;
	    boolean wasUpolyd = Upolyd;

	    /* in case touching this object turns out to be fatal */
	    place_object(obj, u.ux, u.uy);

	    if (!touch_artifact(obj, &youmonst)) {
		obj_extract_self(obj);	/* remove it from the floor */
		dropy(obj);		/* now put it back again :-) */
		return obj;
	    } else if (wasUpolyd && !Upolyd) {
		/* loose your grip if you revert your form */
		if (drop_fmt) pline(drop_fmt, drop_arg);
		obj_extract_self(obj);
		dropy(obj);
		return obj;
	    }
	    obj_extract_self(obj);
	    if (crysknife) {
		obj->otyp = CRYSKNIFE;
		obj->oerodeproof = oerode;
	    }
	}
	if (Fumbling) {
	    if (drop_fmt) pline(drop_fmt, drop_arg);
	    dropy(obj);
	} else {
	    long oquan = obj->quan;
	    int prev_encumbr = near_capacity();	/* before addinv() */

	    /* encumbrance only matters if it would now become worse
	       than max( current_value, stressed ) */
	    if (prev_encumbr < MOD_ENCUMBER) prev_encumbr = MOD_ENCUMBER;
	    /* addinv() may redraw the entire inventory, overwriting
	       drop_arg when it comes from something like doname() */
	    if (drop_arg) drop_arg = strcpy(buf, drop_arg);

	    obj = addinv(obj);
	    if (inv_cnt() > 52
		    || (( (obj->otyp != LOADSTONE && obj->otyp != HEALTHSTONE && obj->otyp != LUCKSTONE && obj->otyp != MANASTONE && obj->otyp != SLEEPSTONE && obj->otyp != LOADBOULDER && obj->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(obj) ) || (!obj->cursed && !is_nastygraystone(obj)) )
			&& near_capacity() > prev_encumbr)) {
		if (drop_fmt) pline(drop_fmt, drop_arg);
		/* undo any merge which took place */
		if (obj->quan > oquan) obj = splitobj(obj, oquan);
		dropx(obj);
	    } else {
		if (flags.autoquiver && !uquiver && !obj->owornmask &&
			(is_missile(obj) ||
			    ammo_and_launcher(obj, uwep) ||
			    ammo_and_launcher(obj, uswapwep)))
		    setuqwep(obj);
		if (hold_msg || drop_fmt) prinv(hold_msg, obj, oquan);
	    }
	}
	return obj;
}

struct obj *
hold_another_objectX(obj, drop_fmt, drop_arg, hold_msg)
struct obj *obj;
const char *drop_fmt, *drop_arg, *hold_msg;
{
	char buf[BUFSZ];
	char qbuf[QBUFSZ];

#ifndef INVISIBLE_OBJECTS
	if (!Blind) obj->dknown = 1;	/* maximize mergibility */
#else
	if (!Blind && (!obj->oinvis || See_invisible)) obj->dknown = 1;
#endif
	if (obj->oartifact) {
	    /* place_object may change these */
	    boolean crysknife = (obj->otyp == CRYSKNIFE);
	    int oerode = obj->oerodeproof;
	    boolean wasUpolyd = Upolyd;

	    /* in case touching this object turns out to be fatal */
	    place_object(obj, u.ux, u.uy);

	    if (!touch_artifact(obj, &youmonst)) {
		obj_extract_self(obj);	/* remove it from the floor */
		dropy(obj);		/* now put it back again :-) */
		return obj;
	    } else if (wasUpolyd && !Upolyd) {
		/* loose your grip if you revert your form */
		if (drop_fmt) pline(drop_fmt, drop_arg);
		obj_extract_self(obj);
		dropy(obj);
		return obj;
	    }
	    obj_extract_self(obj);
	    if (crysknife) {
		obj->otyp = CRYSKNIFE;
		obj->oerodeproof = oerode;
	    }
	}
	if (Fumbling) {
	    if (drop_fmt) pline(drop_fmt, drop_arg);
	    dropy(obj);
	} else {
	    long oquan = obj->quan;
	    int prev_encumbr = near_capacity();	/* before addinv() */

	    /* encumbrance only matters if it would now become worse
	       than max( current_value, stressed ) */
	    if (prev_encumbr < MOD_ENCUMBER) prev_encumbr = MOD_ENCUMBER;
	    /* addinv() may redraw the entire inventory, overwriting
	       drop_arg when it comes from something like doname() */
	    if (drop_arg) drop_arg = strcpy(buf, drop_arg);

	    obj = addinv(obj);

		/* Players were getting annoyed by having their inventory cluttered with garbage from attacking
		 * low-level monsters as a high-level nymph. Let's allow them to drop items if they don't want them. --Amy */

		Sprintf(qbuf, "Got %s! Drop it?", doname(obj) );

		if (yn_function(qbuf, ynchars, 'n') == 'y' ) {

		if (drop_fmt) pline(drop_fmt, drop_arg);

		dropx(obj); /* just drop the crap on the ground. Nobody needs tons of chain mails from just attacking orcs. */
		return obj;
		}

	    if ( inv_cnt() > 52
		    || (( (obj->otyp != LOADSTONE && obj->otyp != HEALTHSTONE && obj->otyp != LUCKSTONE && obj->otyp != MANASTONE && obj->otyp != SLEEPSTONE && obj->otyp != LOADBOULDER && obj->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(obj) ) || (!obj->cursed && !is_nastygraystone(obj)) )
			&& near_capacity() > prev_encumbr)) {
		if (drop_fmt) pline(drop_fmt, drop_arg);
		/* undo any merge which took place */
		if (obj->quan > oquan) obj = splitobj(obj, oquan);
		dropx(obj);
	    } else {
		if (flags.autoquiver && !uquiver && !obj->owornmask &&
			(is_missile(obj) ||
			    ammo_and_launcher(obj, uwep) ||
			    ammo_and_launcher(obj, uswapwep)))
		    setuqwep(obj);
		if (hold_msg || drop_fmt) prinv(hold_msg, obj, oquan);
	    }
	}
	return obj;
}

/* useup() all of an item regardless of its quantity */
void
useupall(obj)
struct obj *obj;
{

	if (evades_destruction(obj)) return; /* fail safe */

	if (Has_contents(obj)) delete_contents(obj);
	setnotworn(obj);
	freeinv(obj);
	obfree(obj, (struct obj *)0);
}

void
useup(obj)
register struct obj *obj;
{

	if (evades_destruction(obj)) return; /* fail safe */

	/*  Note:  This works correctly for containers because they */
	/*	   (containers) don't merge.			    */
	if(obj->quan > 1L){
		obj->in_use = FALSE;	/* no longer in use */
		obj->quan--;
		obj->owt = weight(obj);
		update_inventory();
	} else {
		useupall(obj);
	}
}

/* use one charge from an item and possibly incur shop debt for it */
void
consume_obj_charge(obj, maybe_unpaid)
struct obj *obj;
boolean maybe_unpaid;	/* false if caller handles shop billing */
{
	if (maybe_unpaid) check_unpaid(obj);
	obj->spe -= 1;
	if (obj->known) update_inventory();
}

#endif /* OVLB */
#ifdef OVL3

/*
Adjust hero's attributes as if this object was being removed from the
hero's inventory.  This should only be called from freeinv() and
where we are polymorphing an object already in the hero's inventory.

Should think of a better name...
*/
void
freeinv_core(obj)
struct obj *obj;
{
	if (obj->oclass == COIN_CLASS) {
#ifndef GOLDOBJ
		u.ugold -= obj->quan;
		obj->in_use = FALSE;
#endif
		flags.botl = 1;
		return;
	} else if (obj->otyp == AMULET_OF_YENDOR) {
		if (!u.uhave.amulet) impossible("don't have amulet?");
		u.uhave.amulet = 0;
	} else if (obj->otyp == CANDELABRUM_OF_INVOCATION) {
		if (!u.uhave.menorah) impossible("don't have candelabrum?");
		u.uhave.menorah = 0;
	} else if (obj->otyp == BELL_OF_OPENING) {
		if (!u.uhave.bell) impossible("don't have silver bell?");
		u.uhave.bell = 0;
	} else if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
		if (!u.uhave.book) impossible("don't have the book?");
		u.uhave.book = 0;
	} else if (obj->oartifact) {
		if (is_quest_artifact(obj)) {
		    if (!u.uhave.questart)
			impossible("don't have quest artifact?");
		    u.uhave.questart = 0;
		}
		if(obj->oartifact == ART_TREASURY_OF_PROTEUS){
			u.ukinghill = FALSE;
		}
		set_artifact_intrinsic(obj, 0, W_ART);
	}

	if (obj->otyp == LOADSTONE || obj->otyp == SLEEPSTONE || obj->otyp == LOADBOULDER || is_nastygraystone(obj) ) {
		curse(obj);
	} else if (confers_luck(obj)) {
		set_moreluck();
		flags.botl = 1;
	} else if (obj->otyp == HEALTHSTONE) {
	/* KMH, balance patch -- recalculate health if you've lost healthstones */
		recalc_health();
	} else if (obj->otyp == FIGURINE && obj->timed) {
		(void) stop_timer(FIG_TRANSFORM, (genericptr_t) obj);
	}
}

/* remove an object from the hero's inventory */
void
freeinv(obj)
register struct obj *obj;
{
	extract_nobj(obj, &invent);
	freeinv_core(obj);
	update_inventory();
}

void
delallobj(x, y)
int x, y;
{
	struct obj *otmp, *otmp2;

	for (otmp = level.objects[x][y]; otmp; otmp = otmp2) {
		if (otmp == uball)
			unpunish();
		/* after unpunish(), or might get deallocated chain */
		otmp2 = otmp->nexthere;
		if (otmp == uchain)
			continue;
		delobj(otmp);
	}
}

#endif /* OVL3 */
#ifdef OVL2

/* destroy object in fobj chain (if unpaid, it remains on the bill) */
void
delobj(obj)
register struct obj *obj;
{
	boolean update_map;

	if (evades_destruction(obj)) {
		/* player might be doing something stupid, but we
		 * can't guarantee that.  assume special artifacts
		 * are indestructible via drawbridges, and exploding
		 * chests, and golem creation, and ...
		 */
		return;
	}
	if (uwep && uwep == obj) setnotworn(obj); /* this hopefully fixes cream pie bugs and similar things --Amy */

	/* to be on the safe side, let's include this check for all the other inventory slots too... */
	if (uswapwep && uswapwep == obj) uswapwepgone();
	if (uquiver && uquiver == obj) uqwepgone();
	if (uarm && uarm == obj) remove_worn_item(obj, TRUE);
	if (uarmc && uarmc == obj) remove_worn_item(obj, TRUE);
	if (uarmh && uarmh == obj) remove_worn_item(obj, TRUE);
	if (uarms && uarms == obj) remove_worn_item(obj, TRUE);
	if (uarmg && uarmg == obj) remove_worn_item(obj, TRUE);
	if (uarmf && uarmf == obj) remove_worn_item(obj, TRUE);
	if (uarmu && uarmu == obj) remove_worn_item(obj, TRUE);
	if (uamul && uamul == obj) remove_worn_item(obj, TRUE);
	if (uleft && uleft == obj) remove_worn_item(obj, TRUE);
	if (uright && uright == obj) remove_worn_item(obj, TRUE);
	if (ublindf && ublindf == obj) remove_worn_item(obj, TRUE);
	if (uball && uball == obj) unpunish();
	if (uchain && uchain == obj) unpunish();

	update_map = (obj->where == OBJ_FLOOR || Has_contents(obj) &&
		(obj->where == OBJ_INVENT || obj->where == OBJ_MINVENT));
	if (Has_contents(obj)) delete_contents(obj);
	obj_extract_self(obj);
	if (update_map) newsym(obj->ox, obj->oy);
	obfree(obj, (struct obj *) 0);
}

#endif /* OVL2 */
#ifdef OVL0

struct obj *
sobj_at(n,x,y)
register int n, x, y;
{
	register struct obj *otmp;

	for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		if(otmp->otyp == n)
		    return(otmp);
	return((struct obj *)0);
}

#endif /* OVL0 */
#ifdef OVLB

struct obj *
carrying(type)
register int type;
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == type)
			return(otmp);
	return((struct obj *) 0);
}

const char *
currency(amount)
long amount;
{
	if (amount == 1L) return "zorkmid";
	else return "zorkmids";
}

boolean
have_lizard()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_CAVE_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_CHAOS_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_LIZARD_EEL)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_LIZARD_MAN)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_LIZARD_KING)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_EEL_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_CLINGING_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_DEFORMED_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_MIMIC_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_HIDDEN_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_ANTI_STONE_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_HUGE_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_ROCK_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_NIGHT_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_SAND_TIDE)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_FBI_AGENT)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_OWN_SMOKE)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_GRANDPA)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_KARMIC_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_MONSTER_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_FIRE_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_LIGHTNING_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_ICE_LIZARD)
			return(TRUE);
		if(otmp->otyp == CORPSE && otmp->corpsenm == PM_GIANT_LIZARD)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_loadstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == LOADSTONE && otmp->cursed)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_primecurse()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->prmcurse)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_mothrelay()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if( (otmp->otyp == RELAY) && otmp->oartifact )
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_sleepstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == SLEEPSTONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_magicresstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STONE_OF_MAGIC_RESISTANCE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_cursedmagicresstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STONE_OF_MAGIC_RESISTANCE  && otmp->cursed)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_loadboulder()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == LOADBOULDER && otmp->cursed)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_rmbstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == RIGHT_MOUSE_BUTTON_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_displaystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == DISPLAY_LOSS_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_yellowspellstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == YELLOW_SPELL_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_spelllossstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == SPELL_LOSS_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_autodestructstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == AUTO_DESTRUCT_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_memorylossstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == MEMORY_LOSS_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_inventorylossstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == INVENTORY_LOSS_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_blackystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == BLACKY_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_menubugstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == MENU_BUG_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_speedbugstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == SPEEDBUG_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_superscrollerstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == SUPERSCROLLER_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_freehandbugstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == FREE_HAND_BUG_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_unidentifystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == UNIDENTIFY_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_thirststone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STONE_OF_THIRST)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_unluckystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == UNLUCKY_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_shadesofgreystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == SHADES_OF_GREY_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_faintingstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STONE_OF_FAINTING)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_cursingstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STONE_OF_CURSING)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_difficultystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STONE_OF_DIFFICULTY)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_deafnessstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == DEAFNESS_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_weaknessstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == WEAKNESS_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_antimagicstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == ANTIMAGIC_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_rotthirteenstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == ROT_THIRTEEN_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_bishopstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == BISHOP_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_confusionstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == CONFUSION_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_dropbugstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == DROPBUG_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_dstwstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == DSTW_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_statusstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STATUS_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_alignmentstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == ALIGNMENT_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_stairstrapstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STAIRSTRAP_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_uninformationstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == UNINFORMATION_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_intrinsiclossstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STONE_OF_INTRINSIC_LOSS)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_bloodlossstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == BLOOD_LOSS_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_badeffectstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == BAD_EFFECT_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_trapcreationstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == TRAP_CREATION_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_vulnerabilitystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == STONE_OF_VULNERABILITY)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_itemportstone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == ITEM_TELEPORTING_STONE)
			return(TRUE);
		}
	return(FALSE);
}

boolean
have_nastystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->otyp == NASTY_STONE)
			return(TRUE);
		}
	return(FALSE);
}

struct obj *
o_on(id, objchn)
unsigned int id;
register struct obj *objchn;
{
	struct obj *temp;

	while(objchn) {
		if(objchn->o_id == id) return(objchn);
		if (Has_contents(objchn) && (temp = o_on(id,objchn->cobj)))
			return temp;
		objchn = objchn->nobj;
	}
	return((struct obj *) 0);
}

boolean
obj_here(obj, x, y)
register struct obj *obj;
int x, y;
{
	register struct obj *otmp;

	for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		if(obj == otmp) return(TRUE);
	return(FALSE);
}

#endif /* OVLB */
#ifdef OVL2

struct obj *
g_at(x,y)
register int x, y;
{
	register struct obj *obj = level.objects[x][y];
	while(obj) {
	    if (obj->oclass == COIN_CLASS) return obj;
	    obj = obj->nexthere;
	}
	return((struct obj *)0);
}

#endif /* OVL2 */
#ifdef OVLB
#ifndef GOLDOBJ
/* Make a gold object from the hero's gold. */
struct obj *
mkgoldobj(q)
register long q;
{
	register struct obj *otmp;

	otmp = mksobj(GOLD_PIECE, FALSE, FALSE);
	u.ugold -= q;
	otmp->quan = q;
	otmp->owt = weight(otmp);
	flags.botl = 1;
	return(otmp);
}
#endif
#endif /* OVLB */
#ifdef OVL1

STATIC_OVL void
compactify(buf)
register char *buf;
/* compact a string of inventory letters by dashing runs of letters */
{
	register int i1 = 1, i2 = 1;
	register char ilet, ilet1, ilet2;

	ilet2 = buf[0];
	ilet1 = buf[1];
	buf[++i2] = buf[++i1];
	ilet = buf[i1];
	while(ilet) {
		if(ilet == ilet1+1) {
			if(ilet1 == ilet2+1)
				buf[i2 - 1] = ilet1 = '-';
			else if(ilet2 == '-') {
				buf[i2 - 1] = ++ilet1;
				buf[i2] = buf[++i1];
				ilet = buf[i1];
				continue;
			}
		}
		ilet2 = ilet1;
		ilet1 = ilet;
		buf[++i2] = buf[++i1];
		ilet = buf[i1];
	}
}

/* match the prompt for either 'T' or 'R' command */
STATIC_OVL boolean
taking_off(action)
const char *action;
{
    return !strcmp(action, "take off") || !strcmp(action, "remove");
}

/* match the prompt for either 'W' or 'P' command */
STATIC_OVL boolean
putting_on(action)
const char *action;
{
    return !strcmp(action, "wear") || !strcmp(action, "put on");
}

STATIC_OVL int
ugly_checks(let, word, otmp)
const char *let, *word;
struct obj *otmp;
{
		register int otyp = otmp->otyp;
		/* ugly check: remove inappropriate things */
		if((taking_off(word) &&
		    (!(otmp->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL))
		     || (otmp==uarm && uarmc)
#ifdef TOURIST
		     || (otmp==uarmu && (uarm || uarmc))
#endif
		    ))
		|| (putting_on(word) &&
		     (otmp->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL)))
							/* already worn */
#if 0	/* 3.4.1 -- include currently wielded weapon among the choices */
		|| (!strcmp(word, "wield") &&
		    (otmp->owornmask & W_WEP))
#endif
		|| (!strcmp(word, "ready") &&
		    (otmp == uwep || (otmp == uswapwep && u.twoweap)))
		    ) {
			return 1;
		}

		/* Second ugly check; unlike the first it won't trigger an
		 * "else" in "you don't have anything else to ___".
		 */
                else if ((putting_on(word) &&
		    ((otmp->oclass == FOOD_CLASS && otmp->otyp != MEAT_RING) ||
		    (otmp->oclass == TOOL_CLASS &&
		     otyp != BLINDFOLD && otyp != TOWEL && otyp != LENSES)))
/*add check for improving*/
                || ( (!strcmp(word, "wield") || !strcmp(word, "improve")) &&
		    (otmp->oclass == TOOL_CLASS && !is_weptool(otmp)))
		|| (!strcmp(word, "eat") && !is_edible(otmp))
		|| (!strcmp(word, "revive") && otyp != CORPSE) /* revive */
		|| (!strcmp(word, "sacrifice") &&
		    (otyp != CORPSE &&
		     otyp != SEVERED_HAND &&                    
		     otyp != EYEBALL &&	/* KMH -- fixed */
		     otyp != AMULET_OF_YENDOR && otyp != FAKE_AMULET_OF_YENDOR))
		|| (!strcmp(word, "write with") &&
		    (otmp->oclass == TOOL_CLASS &&
#ifdef LIGHTSABERS
		     (!is_lightsaber(otmp) || !otmp->lamplit) &&
#endif
		     otyp != MAGIC_MARKER && otyp != TOWEL))
		|| (!strcmp(word, "tin") &&
		    (otyp != CORPSE || !tinnable(otmp)))
		|| (!strcmp(word, "rub") &&
		    ((otmp->oclass == TOOL_CLASS &&
		      otyp != OIL_LAMP && otyp != MAGIC_LAMP &&
		      otyp != BRASS_LANTERN) ||
		     (otmp->oclass == GEM_CLASS && !is_graystone(otmp))))
		|| (!strncmp(word, "rub on the stone", 16) &&
		    *let == GEM_CLASS &&	/* using known touchstone */
		    otmp->dknown && objects[otyp].oc_name_known)
		|| ((!strcmp(word, "use or apply") ||
			!strcmp(word, "untrap with")) &&
		     /* Picks, axes, pole-weapons, bullwhips */
		    ((otmp->oclass == WEAPON_CLASS && !is_pick(otmp) &&
#ifdef FIREARMS
		      otyp != SUBMACHINE_GUN &&
		      otyp != AUTO_SHOTGUN &&
		      otyp != ASSAULT_RIFLE &&
		      otyp != FRAG_GRENADE &&
		      otyp != GAS_GRENADE &&
		      otyp != STICK_OF_DYNAMITE &&
#endif
		      !is_axe(otmp) && !is_antibar(otmp) && !is_pole(otmp) && otyp != BULLWHIP) ||
		    (otmp->oclass == POTION_CLASS &&
		     /* only applicable potion is oil, and it will only
			be offered as a choice when already discovered */
		     (otyp != POT_OIL || !otmp->dknown ||
		      !objects[POT_OIL].oc_name_known) &&
		      /* water is only for untrapping */
		     (strcmp(word, "untrap with") || 
		      otyp != POT_WATER || !otmp->dknown ||
		      !objects[POT_WATER].oc_name_known)) ||
		     (otmp->oclass == FOOD_CLASS &&
		      otyp != CREAM_PIE && otyp != EUCALYPTUS_LEAF) ||
		     (otmp->oclass == GEM_CLASS && !is_graystone(otmp))))
		|| (!strcmp(word, "invoke") &&
		    (!otmp->oartifact && !objects[otyp].oc_unique &&
		     (otyp != FAKE_AMULET_OF_YENDOR || otmp->known) &&
		     otyp != CRYSTAL_BALL &&	/* #invoke synonym for apply */
		   /* note: presenting the possibility of invoking non-artifact
		      mirrors and/or lamps is a simply a cruel deception... */
		     otyp != MIRROR && otyp != MAGIC_LAMP &&
		     (otyp != OIL_LAMP ||	/* don't list known oil lamp */
		      (otmp->dknown && objects[OIL_LAMP].oc_name_known))))
		|| (!strcmp(word, "untrap with") &&
		    (otmp->oclass == TOOL_CLASS && otyp != CAN_OF_GREASE))
		|| (!strcmp(word, "charge") && !is_chargeable(otmp))
		|| (!strcmp(word, "randomly enchant") && !is_enchantable(otmp))
		|| (!strcmp(word, "poison") && !is_poisonable(otmp))
		|| (!strcmp(word, "magically enchant") && !(otmp->owornmask & W_ARMOR) )
		|| ((!strcmp(word, "draw blood with") ||
			!strcmp(word, "bandage your wounds with")) &&
		    (otmp->oclass == TOOL_CLASS && otyp != MEDICAL_KIT))
		    )
			return 2;
		else
		    return 0;
}

/* List of valid classes for allow_ugly callback */
static char valid_ugly_classes[MAXOCLASSES + 1];

/* Action word for allow_ugly callback */
static const char *ugly_word;

STATIC_OVL boolean
allow_ugly(obj)
struct obj *obj;
{
    return index(valid_ugly_classes, obj->oclass) &&
	   !ugly_checks(valid_ugly_classes, ugly_word, obj);
}

/*
 * getobj returns:
 *	struct obj *xxx:	object to do something with.
 *	(struct obj *) 0	error return: no object.
 *	&zeroobj		explicitly no object (as in w-).
 *	&thisplace		this place (as in r.).
#ifdef GOLDOBJ
!!!! test if gold can be used in unusual ways (eaten etc.)
!!!! may be able to remove "usegold"
#endif
 */
struct obj *
getobj(let,word)
register const char *let,*word;
{
	register struct obj *otmp;
	register char ilet;
	char buf[BUFSZ], qbuf[QBUFSZ];
	char lets[BUFSZ], altlets[BUFSZ], *ap;
	register int foo = 0;
	register char *bp = buf;
	xchar allowcnt = 0;	/* 0, 1 or 2 */
#ifndef GOLDOBJ
	boolean allowgold = FALSE;	/* can't use gold because they don't have any */
#endif
	boolean usegold = FALSE;	/* can't use gold because its illegal */
	boolean allowall = FALSE;
	boolean allownone = FALSE;
	boolean allowfloor = FALSE;
	boolean usefloor = FALSE;
	boolean allowthisplace = FALSE;
	boolean useboulder = FALSE;
	xchar foox = 0;
	long cnt;
	boolean prezero = FALSE;
	long dummymask;
	int ugly;
	struct obj *floorchain;
	int floorfollow;

	if(*let == ALLOW_COUNT) let++, allowcnt = 1;
#ifndef GOLDOBJ
	if(*let == COIN_CLASS) let++,
		usegold = TRUE, allowgold = (u.ugold ? TRUE : FALSE);
#else
	if(*let == COIN_CLASS) let++, usegold = TRUE;
#endif

	/* Equivalent of an "ugly check" for gold */
	if (usegold && !strcmp(word, "eat") &&
	    (!metallivorous(youmonst.data)
	     || youmonst.data == &mons[PM_RUST_MONSTER]))
#ifndef GOLDOBJ
		usegold = allowgold = FALSE;
#else
		usegold = FALSE;
#endif

	if(*let == ALL_CLASSES) let++, allowall = TRUE;
	if(*let == ALLOW_NONE) let++, allownone = TRUE;
	if(*let == ALLOW_FLOOROBJ) {
	    let++;
	    if (!u.uswallow) {
		floorchain = can_reach_floorobj() ? level.objects[u.ux][u.uy] :
			     (struct obj *)0;
		floorfollow = BY_NEXTHERE;
	    } else {
		floorchain = u.ustuck->minvent;
		floorfollow = 0;		/* nobj */
	    }
	    usefloor = TRUE;
	    allowfloor = !!floorchain;
	}
	if(*let == ALLOW_THISPLACE) let++, allowthisplace = TRUE;
	/* "ugly check" for reading fortune cookies, part 1 */
	/* The normal 'ugly check' keeps the object on the inventory list.
	 * We don't want to do that for shirts/cookies, so the check for
	 * them is handled a bit differently (and also requires that we set
	 * allowall in the caller)
	 */
	if(allowall && !strcmp(word, "read")) allowall = FALSE;

	/* another ugly check: show boulders (not statues) */
	if(*let == WEAPON_CLASS &&
	   !strcmp(word, "throw") && throws_rocks(youmonst.data))
	    useboulder = TRUE;

	if(allownone) *bp++ = '-';
#ifndef GOLDOBJ
	if(allowgold) *bp++ = def_oc_syms[COIN_CLASS];
#endif
	if(bp > buf && bp[-1] == '-') *bp++ = ' ';
	ap = altlets;

	ilet = 'a';
	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if (!flags.invlet_constant)
#ifdef GOLDOBJ
		if (otmp->invlet != GOLD_SYM) /* don't reassign this */
#endif
		otmp->invlet = ilet;	/* reassign() */
	    if (!*let || index(let, otmp->oclass)
#ifdef GOLDOBJ
		|| (usegold && otmp->invlet == GOLD_SYM)
#endif
		|| (useboulder && otmp->otyp == BOULDER)
		) {
		bp[foo++] = otmp->invlet;

		/* ugly checks */
		ugly = ugly_checks(let, word, otmp);
		if (ugly == 1) {
		    foo--;
		    foox++;
		} else if (ugly == 2)
		    foo--;
		/* ugly check for unworn armor that can't be worn */
		else if (putting_on(word) && *let == ARMOR_CLASS &&
			 !canwearobj(otmp, &dummymask, FALSE)) {
			foo--;
			allowall = TRUE;
			*ap++ = otmp->invlet;
		}
	    } else {

		/* "ugly check" for reading fortune cookies, part 2 */
		if ((!strcmp(word, "read") &&
		    (otmp->otyp == FORTUNE_COOKIE
#ifdef TOURIST
			|| otmp->otyp == T_SHIRT
			|| otmp->otyp == STRIPED_SHIRT
			|| otmp->otyp == HAWAIIAN_SHIRT
			|| otmp->otyp == RUFFLED_SHIRT
			|| otmp->otyp == VICTORIAN_UNDERWEAR
#endif
		    )))
			allowall = TRUE;
	    }

	    if(ilet == 'z') ilet = 'A'; else ilet++;
	}
	bp[foo] = 0;
	if(foo == 0 && bp > buf && bp[-1] == ' ') *--bp = 0;
	Strcpy(lets, bp);	/* necessary since we destroy buf */
	if(foo > 5)			/* compactify string */
		compactify(bp);
	*ap = '\0';

	if (allowfloor && !allowall) {
	    if (usegold) {
		valid_ugly_classes[0] = COIN_CLASS;
		Strcpy(valid_ugly_classes + 1, let);
	    } else
		Strcpy(valid_ugly_classes, let);
	    ugly_word = word;
	    for (otmp = floorchain; otmp; otmp = FOLLOW(otmp, floorfollow))
		if (allow_ugly(otmp))
		    break;
	    if (!otmp)
		allowfloor = FALSE;
	}

	if(!foo && !allowall && !allownone &&
#ifndef GOLDOBJ
	   !allowgold &&
#endif
	   !allowfloor && !allowthisplace) {
		You("don't have anything %sto %s.",
			foox ? "else " : "", word);
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return((struct obj *)0);
	}
	
	for(;;) {
		cnt = 0;
		if (allowcnt == 2) allowcnt = 1;  /* abort previous count */
		Sprintf(qbuf, "What do you want to %s? [", word);
		bp = eos(qbuf);
		if (buf[0]) {
		    Sprintf(bp, "%s or ?", buf);
		    bp = eos(bp);
		}
		*bp++ = '*';
		if (allowfloor)
		    *bp++ = ',';
		if (allowthisplace)
		    *bp++ = '.';
		if (!buf[0] && bp[-2] != '[') {
		    /* "*," -> "* or ,"; "*." -> "* or ."; "*,." -> "*, or ." */
		    --bp;
		    Sprintf(bp, " or %c", *bp);
		    bp += 5;
		}
		*bp++ = ']';
		*bp = '\0';
#ifdef REDO
		if (in_doagain)
		    ilet = readchar();
		else
#endif
		    ilet = yn_function(qbuf, (char *)0, '\0');
		if(ilet == '0') prezero = TRUE;
		while(digit(ilet) && allowcnt) {
#ifdef REDO
			if (ilet != '?' && ilet != '*')	savech(ilet);
#endif
			cnt = 10*cnt + (ilet - '0');
			allowcnt = 2;	/* signal presence of cnt */
			ilet = readchar();
		}
		if(digit(ilet)) {
			pline("No count allowed with this command.");
			continue;
		}
		if(index(quitchars,ilet)) {
		    if(flags.verbose)
			pline(Never_mind);
		    return((struct obj *)0);
		}
		if(ilet == '-') {
			return(allownone ? &zeroobj : (struct obj *) 0);
		}
		if(ilet == def_oc_syms[COIN_CLASS]) {
			if (!usegold) {
			    if (!strncmp(word, "rub on ", 7)) {
				/* the dangers of building sentences... */
				You("cannot rub gold%s.", word + 3);
			    } else {
				You("cannot %s gold.", word);
			    }
			    return(struct obj *)0;
#ifndef GOLDOBJ
			} else if (!allowgold) {
				You("are not carrying any gold.");
				return(struct obj *)0;
#endif
			} 
			if(cnt == 0 && prezero) return((struct obj *)0);
			/* Historic note: early Nethack had a bug which was
			 * first reported for Larn, where trying to drop 2^32-n
			 * gold pieces was allowed, and did interesting things
			 * to your money supply.  The LRS is the tax bureau
			 * from Larn.
			 */
			if(cnt < 0) {
	pline_The("LRS would be very interested to know you have that much.");
				return(struct obj *)0;
			}

#ifndef GOLDOBJ
			if(!(allowcnt == 2 && cnt < u.ugold))
				cnt = u.ugold;
			return(mkgoldobj(cnt));
#endif
		}
		if(ilet == '.') {
		    if (allowthisplace)
			return &thisplace;
		    else {
			pline(silly_thing_to, word);
			return(struct obj *)0;
		    }
		}
		if(ilet == ',') {
		    int n;
		    menu_item *pick_list;

		    if (!usefloor) {
			pline(silly_thing_to, word);
			return(struct obj *)0;
		    } else if (!allowfloor) {
			if ((Levitation || Flying))
				You("cannot reach the floor to %s while %sing.", word, Levitation ? "float" : "fly");
			else
				pline("There's nothing here to %s.", word);
			return(struct obj *)0;
		    }
		    Sprintf(qbuf, "%s what?", word);
		    n = query_objlist(qbuf, floorchain,
			    floorfollow|INVORDER_SORT|SIGNAL_CANCEL, &pick_list,
			    PICK_ONE, allowall ? allow_all : allow_ugly);
		    if (n<0) {
			if (flags.verbose)
			    pline(Never_mind);
			return (struct obj *)0;
		    } else if (!n)
			continue;
		    otmp = pick_list->item.a_obj;
		    if (allowcnt && pick_list->count < otmp->quan)
			otmp = splitobj(otmp, pick_list->count);
		    free((genericptr_t)pick_list);
		    return otmp;
		}
		if(ilet == '?' || ilet == '*') {
		    char *allowed_choices = (ilet == '?') ? lets : (char *)0;
		    long ctmp = 0;

		    if (ilet == '?' && !*lets && *altlets)
			allowed_choices = altlets;
		    ilet = display_pickinv(allowed_choices, TRUE,
					   allowcnt ? &ctmp : (long *)0);
		    if(!ilet) continue;
		    if (allowcnt && ctmp >= 0) {
			cnt = ctmp;
			if (!cnt) prezero = TRUE;
			allowcnt = 2;
		    }
		    if(ilet == '\033') {
			if(flags.verbose)
			    pline(Never_mind);
			return((struct obj *)0);
		    }
		    /* they typed a letter (not a space) at the prompt */
		}
		/* WAC - throw now takes a count to allow for single/controlled shooting */
		if(allowcnt == 2 && !strcmp(word,"throw")) {
		    /* permit counts for throwing gold, but don't accept
		     * counts for other things since the throw code will
		     * split off a single item anyway */
#ifdef GOLDOBJ
		    if (ilet != def_oc_syms[COIN_CLASS])
#endif
			allowcnt = 1;
		    if(cnt == 0 && prezero) return((struct obj *)0);
		    if (cnt == 1) {
			save_cm = (char *) 1; /* Non zero */
			multi = 0;
		    }
		    if(cnt > 1) {
			/* You("can only throw one item at a time.");
			continue; */
			multi = cnt - 1;
			cnt = 1;
		    }
		}
#ifdef GOLDOBJ
		flags.botl = 1; /* May have changed the amount of money */
#endif
#ifdef REDO
		savech(ilet);
#endif
		for (otmp = invent; otmp; otmp = otmp->nobj)
			if (otmp->invlet == ilet) break;
		if(!otmp) {
			You("don't have that object.");
#ifdef REDO
			if (in_doagain) return((struct obj *) 0);
#endif
			continue;
		} else if (cnt < 0 || otmp->quan < cnt) {
			You("don't have that many!  You have only %ld.",
			    otmp->quan);
#ifdef REDO
			if (in_doagain) return((struct obj *) 0);
#endif
			continue;
		}
		break;
	}

	if(!allowall && let && !index(let,otmp->oclass)
#ifdef GOLDOBJ
	   && !(usegold && otmp->oclass == COIN_CLASS)
#endif
	   ) {
		silly_thing(word, otmp);
		return((struct obj *)0);
	}
	if(allowcnt == 2) {	/* cnt given */
	    if(cnt == 0) return (struct obj *)0;
	    if(cnt != otmp->quan) {
		/* don't split a stack of cursed loadstones */
		if ( (otmp->otyp == LOADSTONE || otmp->otyp == HEALTHSTONE || otmp->otyp == LUCKSTONE || otmp->otyp == MANASTONE || otmp->otyp == SLEEPSTONE || otmp->otyp == LOADBOULDER || otmp->otyp == STONE_OF_MAGIC_RESISTANCE || is_nastygraystone(otmp) ) && otmp->cursed)
		    /* kludge for canletgo()'s can't-drop-this message */
		    otmp->corpsenm = (int) cnt;
		else
		    otmp = splitobj(otmp, cnt);
	    }
	}
	return(otmp);
}

void
silly_thing(word, otmp)
const char *word;
struct obj *otmp;
{
	const char *s1, *s2, *s3, *what;
	int ocls = otmp->oclass, otyp = otmp->otyp;

	s1 = s2 = s3 = 0;
	/* check for attempted use of accessory commands ('P','R') on armor
	   and for corresponding armor commands ('W','T') on accessories */
	if (ocls == ARMOR_CLASS) {
	    if (!strcmp(word, "put on"))
		s1 = "W", s2 = "wear", s3 = "";
	    else if (!strcmp(word, "remove"))
		s1 = "T", s2 = "take", s3 = " off";
	} else if ((ocls == RING_CLASS || otyp == MEAT_RING) ||
		ocls == AMULET_CLASS ||
		(otyp == BLINDFOLD || otyp == TOWEL || otyp == LENSES)) {
	    if (!strcmp(word, "wear"))
		s1 = "P", s2 = "put", s3 = " on";
	    else if (!strcmp(word, "take off"))
		s1 = "R", s2 = "remove", s3 = "";
	}
	if (s1) {
	    what = "that";
	    /* quantity for armor and accessory objects is always 1,
	       but some things should be referred to as plural */
	    if (otyp == LENSES || is_gloves(otmp) || is_boots(otmp))
		what = "those";
	    pline("Use the '%s' command to %s %s%s.", s1, s2, what, s3);
	} else {
	    pline(silly_thing_to, word);
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	}
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_PTR int
ckvalidcat(otmp)
register struct obj *otmp;
{
	/* use allow_category() from pickup.c */
	return((int)allow_category(otmp));
}

STATIC_PTR int
ckunpaid(otmp)
register struct obj *otmp;
{
	return((int)(otmp->unpaid));
}

boolean
wearing_armor()
{
	return((boolean)(uarm || uarmc || uarmf || uarmg || uarmh || uarms
#ifdef TOURIST
		|| uarmu
#endif
		));
}

boolean
is_worn(otmp)
register struct obj *otmp;
{
    return((boolean)(!!(otmp->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL |
#ifdef STEED
			W_SADDLE |
#endif
			W_WEP | W_SWAPWEP | W_QUIVER))));
}

static NEARDATA const char removeables[] =
	{ ARMOR_CLASS, WEAPON_CLASS, RING_CLASS, AMULET_CLASS, TOOL_CLASS, 0 };

/* interactive version of getobj - used for Drop, Identify and */
/* Takeoff (A). Return the number of times fn was called successfully */
/* If combo is TRUE, we just use this to get a category list */
int
ggetobj(word, fn, mx, combo, resultflags)
const char *word;
int FDECL((*fn),(OBJ_P)), mx;
boolean combo;		/* combination menu flag */
unsigned *resultflags;
{
	int FDECL((*ckfn),(OBJ_P)) = (int FDECL((*),(OBJ_P))) 0;
	boolean FDECL((*filter),(OBJ_P)) = (boolean FDECL((*),(OBJ_P))) 0;
	boolean takeoff, ident, allflag, m_seen;
	int itemcount;
#ifndef GOLDOBJ
	int oletct, iletct, allowgold, unpaid, oc_of_sym;
#else
	int oletct, iletct, unpaid, oc_of_sym;
#endif
	char sym, *ip, olets[MAXOCLASSES+5], ilets[MAXOCLASSES+5];
	char extra_removeables[3+1];	/* uwep,uswapwep,uquiver */
	char buf[BUFSZ], qbuf[QBUFSZ];

	if (resultflags) *resultflags = 0;
#ifndef GOLDOBJ
	allowgold = (u.ugold && !strcmp(word, "drop")) ? 1 : 0;
#endif
	takeoff = ident = allflag = m_seen = FALSE;
#ifndef GOLDOBJ
	if(!invent && !allowgold){
#else
	if(!invent){
#endif
		You("have nothing to %s.", word);
		return(0);
	}
	add_valid_menu_class(0);	/* reset */
	if (taking_off(word)) {
	    takeoff = TRUE;
	    filter = is_worn;
	} else if (!strcmp(word, "identify")) {
	    ident = TRUE;
	    filter = not_fully_identified;
	}

	iletct = collect_obj_classes(ilets, invent,
				     	FALSE,
#ifndef GOLDOBJ
					(allowgold != 0),
#endif
					filter, &itemcount);
	unpaid = count_unpaid(invent);

	if (ident && !iletct) {
	    return -1;		/* no further identifications */
	} else if (!takeoff && (unpaid || invent)) {
	    ilets[iletct++] = ' ';
	    if (unpaid) ilets[iletct++] = 'u';
	    if (count_buc(invent, BUC_BLESSED))  ilets[iletct++] = 'B';
	    if (count_buc(invent, BUC_UNCURSED)) ilets[iletct++] = 'U';
	    if (count_buc(invent, BUC_CURSED))   ilets[iletct++] = 'C';
	    if (count_buc(invent, BUC_UNKNOWN))  ilets[iletct++] = 'X';
	    if (invent) ilets[iletct++] = 'a';
	} else if (takeoff && invent) {
	    ilets[iletct++] = ' ';
	}
	ilets[iletct++] = 'i';
	if (!combo)
	    ilets[iletct++] = 'm';	/* allow menu presentation on request */
	ilets[iletct] = '\0';

	for (;;) {
	    Sprintf(qbuf,"What kinds of thing do you want to %s? [%s]",
		    word, ilets);
	    getlin(qbuf, buf);
	    if (buf[0] == '\033') return(0);
	    if (index(buf, 'i')) {
		if (display_inventory((char *)0, TRUE) == '\033') return 0;
	    } else
		break;
	}

	extra_removeables[0] = '\0';
	if (takeoff) {
	    /* arbitrary types of items can be placed in the weapon slots
	       [any duplicate entries in extra_removeables[] won't matter] */
	    if (uwep) (void)strkitten(extra_removeables, uwep->oclass);
	    if (uswapwep) (void)strkitten(extra_removeables, uswapwep->oclass);
	    if (uquiver) (void)strkitten(extra_removeables, uquiver->oclass);
	}

	ip = buf;
	olets[oletct = 0] = '\0';
	while ((sym = *ip++) != '\0') {
	    if (sym == ' ') continue;
	    oc_of_sym = def_char_to_objclass(sym);
	    if (takeoff && oc_of_sym != MAXOCLASSES) {
		if (index(extra_removeables, oc_of_sym)) {
		    ;	/* skip rest of takeoff checks */
		} else if (!index(removeables, oc_of_sym)) {
		    pline("Not applicable.");
		    return 0;
		} else if (oc_of_sym == ARMOR_CLASS && !wearing_armor()) {
		    You("are not wearing any armor.");
		    return 0;
		} else if (oc_of_sym == WEAPON_CLASS &&
			!uwep && !uswapwep && !uquiver) {
		    You("are not wielding anything.");
		    return 0;
		} else if (oc_of_sym == RING_CLASS && !uright && !uleft) {
		    You("are not wearing rings.");
		    return 0;
		} else if (oc_of_sym == AMULET_CLASS && !uamul) {
		    You("are not wearing an amulet.");
		    return 0;
		} else if (oc_of_sym == TOOL_CLASS && !ublindf) {
		    You("are not wearing a blindfold.");
		    return 0;
		}
	    }

	    if (oc_of_sym == COIN_CLASS && !combo) {
#ifndef GOLDOBJ
		if (allowgold == 1)
		    (*fn)(mkgoldobj(u.ugold));
		else if (!u.ugold)
		    You("have no gold.");
		allowgold = 2;
#else
		flags.botl = 1;
#endif
	    } else if (sym == 'a') {
		allflag = TRUE;
	    } else if (sym == 'A') {
		/* same as the default */ ;
	    } else if (sym == 'u') {
		add_valid_menu_class('u');
		ckfn = ckunpaid;
	    } else if (sym == 'B') {
	    	add_valid_menu_class('B');
	    	ckfn = ckvalidcat;
	    } else if (sym == 'U') {
	    	add_valid_menu_class('U');
	    	ckfn = ckvalidcat;
	    } else if (sym == 'C') {
	    	add_valid_menu_class('C');
		ckfn = ckvalidcat;
	    } else if (sym == 'X') {
	    	add_valid_menu_class('X');
		ckfn = ckvalidcat;
	    } else if (sym == 'm') {
		m_seen = TRUE;
	    } else if (oc_of_sym == MAXOCLASSES) {
		You("don't have any %c's.", sym);
	    } else if (oc_of_sym != VENOM_CLASS) {	/* suppress venom */
		if (!index(olets, oc_of_sym)) {
		    add_valid_menu_class(oc_of_sym);
		    olets[oletct++] = oc_of_sym;
		    olets[oletct] = 0;
		}
	    }
	}

	if (m_seen)
	    return (allflag || (!oletct && ckfn != ckunpaid)) ? -2 : -3;
	else if (flags.menu_style != MENU_TRADITIONAL && combo && !allflag)
	    return 0;
#ifndef GOLDOBJ
	else if (allowgold == 2 && !oletct)
	    return 1;	/* you dropped gold (or at least tried to) */
	else {
#else
	else /*!!!! if (allowgold == 2 && !oletct)
	    !!!! return 1;	 you dropped gold (or at least tried to) 
            !!!! test gold dropping
	else*/ {
#endif
	    int cnt = askchain(&invent, olets, allflag, fn, ckfn, mx, word); 
	    /*
	     * askchain() has already finished the job in this case
	     * so set a special flag to convey that back to the caller
	     * so that it won't continue processing.
	     * Fix for bug C331-1 reported by Irina Rempt-Drijfhout. 
	     */
	    if (combo && allflag && resultflags)
		*resultflags |= ALL_FINISHED; 
	    return cnt;
	}
}

/*
 * Walk through the chain starting at objchn and ask for all objects
 * with olet in olets (if nonNULL) and satisfying ckfn (if nonnull)
 * whether the action in question (i.e., fn) has to be performed.
 * If allflag then no questions are asked. Max gives the max nr of
 * objects to be treated. Return the number of objects treated.
 */
int
askchain(objchn, olets, allflag, fn, ckfn, mx, word)
struct obj **objchn;
register int allflag, mx;
register const char *olets, *word;	/* olets is an Obj Class char array */
register int FDECL((*fn),(OBJ_P)), FDECL((*ckfn),(OBJ_P));
{
	struct obj *otmp, *otmp2, *otmpo;
	register char sym, ilet;
	register int cnt = 0, dud = 0, tmp;
	boolean takeoff, nodot, ident, ininv;
	char qbuf[QBUFSZ];

	takeoff = taking_off(word);
	ident = !strcmp(word, "identify");
	nodot = (!strcmp(word, "nodot") || !strcmp(word, "drop") ||
		 ident || takeoff);
	ininv = (*objchn == invent);
	/* Changed so the askchain is interrogated in the order specified.
	 * For example, if a person specifies =/ then first all rings will be
	 * asked about followed by all wands -dgk
	 */
nextclass:
	ilet = 'a'-1;
	if (*objchn && (*objchn)->oclass == COIN_CLASS)
		ilet--;		/* extra iteration */
	for (otmp = *objchn; otmp; otmp = otmp2) {
		if(ilet == 'z') ilet = 'A'; else ilet++;
		otmp2 = otmp->nobj;
		if (olets && *olets && otmp->oclass != *olets) continue;
		if (takeoff && !is_worn(otmp)) continue;
		if (ident && !not_fully_identified(otmp)) continue;
		if (ckfn && !(*ckfn)(otmp)) continue;
		if (!allflag) {
			Strcpy(qbuf, !ininv ? doname(otmp) :
				xprname(otmp, (char *)0, ilet, !nodot, 0L, 0L));
			Strcat(qbuf, "?");
			sym = (takeoff || ident || otmp->quan < 2L) ?
				nyaq(qbuf) : nyNaq(qbuf);
		}
		else	sym = 'y';

		otmpo = otmp;
		if (sym == '#') {
		 /* Number was entered; split the object unless it corresponds
		    to 'none' or 'all'.  2 special cases: cursed loadstones and
		    welded weapons (eg, multiple daggers) will remain as merged
		    unit; done to avoid splitting an object that won't be
		    droppable (even if we're picking up rather than dropping).
		  */
		    if (!yn_number)
			sym = 'n';
		    else {
			sym = 'y';
			if (yn_number < otmp->quan && !welded(otmp) &&
			    (!otmp->cursed || (otmp->otyp != LOADSTONE && otmp->otyp != LUCKSTONE && otmp->otyp != HEALTHSTONE && otmp->otyp != MANASTONE && otmp->otyp != SLEEPSTONE && otmp->otyp != LOADBOULDER && otmp->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(otmp) ) )) {
			    otmp = splitobj(otmp, yn_number);
			}
		    }
		}
		switch(sym){
		case 'a':
			allflag = 1;
		case 'y':
			tmp = (*fn)(otmp);
			if(tmp < 0) {
			    if (container_gone(fn)) {
				/* otmp caused magic bag to explode;
				   both are now gone */
				otmp = 0;		/* and return */
			    } else if (otmp && otmp != otmpo) {
				/* split occurred, merge again */
				(void) merged(&otmpo, &otmp);
			    }
			    goto ret;
			}
			cnt += tmp;
			if(--mx == 0) goto ret;
		case 'n':
			if(nodot) dud++;
		default:
			break;
		case 'q':
			/* special case for seffects() */
			if (ident) cnt = -1;
			goto ret;
		}
	}
	if (olets && *olets && *++olets)
		goto nextclass;
	if(!takeoff && (dud || cnt)) pline("That was all.");
	else if(!dud && !cnt) pline("No applicable objects.");
ret:
	return(cnt);
}


/*
 *	Object identification routines:
 */

/* make an object actually be identified; no display updating */
void
fully_identify_obj(otmp)
struct obj *otmp;
{
    makeknown(otmp->otyp);
    if (otmp->oartifact) discover_artifact((xchar)otmp->oartifact);
    otmp->known = otmp->dknown = otmp->bknown = otmp->rknown = 1;
    if (otmp->otyp == EGG && otmp->corpsenm != NON_PM)
	learn_egg_type(otmp->corpsenm);
}

/* ggetobj callback routine; identify an object and give immediate feedback */
int
identify(otmp)
struct obj *otmp;
{
    fully_identify_obj(otmp);
    prinv((char *)0, otmp, 0L);
    return 1;
}

/* menu of unidentified objects; select and identify up to id_limit of them */
STATIC_OVL void
menu_identify(id_limit)
int id_limit;
{
    menu_item *pick_list;
    int n, i, first = 1;
    char buf[BUFSZ];
    /* assumptions:  id_limit > 0 and at least one unID'd item is present */

    while (id_limit) {
	Sprintf(buf, "What would you like to identify %s?",
		first ? "first" : "next");
	n = query_objlist(buf, invent, SIGNAL_NOMENU|USE_INVLET|INVORDER_SORT,
		&pick_list, PICK_ANY, not_fully_identified);

	if (n > 0) {
	    if (n > id_limit) n = id_limit;
	    for (i = 0; i < n; i++, id_limit--)
		(void) identify(pick_list[i].item.a_obj);
	    free((genericptr_t) pick_list);
	    mark_synch(); /* Before we loop to pop open another menu */
	} else {
	    if (n < 0) pline("That was all.");
	    id_limit = 0; /* Stop now */
	}
	first = 0;
    }
}

/* dialog with user to identify a given number of items; 0 means all */
void
identify_pack(id_limit)
int id_limit;
{
    struct obj *obj, *the_obj;
    int n, unid_cnt;

    unid_cnt = 0;
    the_obj = 0;		/* if unid_cnt ends up 1, this will be it */
    for (obj = invent; obj; obj = obj->nobj)
	if (not_fully_identified(obj)) ++unid_cnt, the_obj = obj;

    if (!unid_cnt) {
	You("have already identified all of your possessions.");
    } else if (!id_limit) {
	/* identify everything */
	if (unid_cnt == 1) {
	    (void) identify(the_obj);
	} else {

	    /* TODO:  use fully_identify_obj and cornline/menu/whatever here */
	    for (obj = invent; obj; obj = obj->nobj)
		if (not_fully_identified(obj)) (void) identify(obj);

	}
    } else {
	/* identify up to `id_limit' items */
	n = 0;
	if (flags.menu_style == MENU_TRADITIONAL)
	    do {
		n = ggetobj("identify", identify, id_limit, FALSE, (unsigned *)0);
		if (n < 0) break; /* quit or no eligible items */
	    } while ((id_limit -= n) > 0);
	if (n == 0 || n < -1)
	    menu_identify(id_limit);
    }
    update_inventory();
}

#endif /* OVLB */
#ifdef OVL2

STATIC_OVL char
obj_to_let(obj)	/* should of course only be called for things in invent */
register struct obj *obj;
{
#ifndef GOLDOBJ
	if (obj->oclass == COIN_CLASS)
		return GOLD_SYM;
#endif
	if (!flags.invlet_constant) {
		obj->invlet = NOINVSYM;
		reassign();
	}
	return obj->invlet;
}

/*
 * Print the indicated quantity of the given object.  If quan == 0L then use
 * the current quantity.
 */
void
prinv(prefix, obj, quan)
const char *prefix;
register struct obj *obj;
long quan;
{
	if (!prefix) prefix = "";
	pline("%s%s%s",
	      prefix, *prefix ? " " : "",
	      xprname(obj, (char *)0, obj_to_let(obj), TRUE, 0L, quan));
}

#endif /* OVL2 */
#ifdef OVL1

char *
xprname(obj, txt, let, dot, cost, quan)
struct obj *obj;
const char *txt;	/* text to print instead of obj */
char let;		/* inventory letter */
boolean dot;		/* append period; (dot && cost => Iu) */
long cost;		/* cost (for inventory of unpaid or expended items) */
long quan;		/* if non-0, print this quantity, not obj->quan */
{
#ifdef LINT	/* handle static char li[BUFSZ]; */
    char li[BUFSZ];
#else
    static char li[BUFSZ];
#endif
    boolean use_invlet = flags.invlet_constant && let != CONTAINED_SYM;
    long savequan = 0;

    if (quan && obj) {
	savequan = obj->quan;
	obj->quan = quan;
    }

    /*
     * If let is:
     *	*  Then obj == null and we are printing a total amount.
     *	>  Then the object is contained and doesn't have an inventory letter.
     */
    if (cost != 0 || let == '*') {
	/* if dot is true, we're doing Iu, otherwise Ix */
	Sprintf(li, "%c - %-45s %6ld %s",
		(dot && use_invlet ? obj->invlet : let),
		(txt ? txt : doname(obj)), cost, currency(cost));
#ifndef GOLDOBJ
    } else if (obj && obj->oclass == COIN_CLASS) {
	Sprintf(li, "%ld gold piece%s%s", obj->quan, plur(obj->quan),
		(dot ? "." : ""));
#endif
    } else {
	/* ordinary inventory display or pickup message */
	Sprintf(li, "%c - %s%s",
		(use_invlet ? obj->invlet : let),
		(txt ? txt : doname(obj)), (dot ? "." : ""));
    }
    if (savequan) obj->quan = savequan;

    return li;
}

#endif /* OVL1 */
#ifdef OVLB

/* the 'i' command */
int
ddoinv()
{
	/*(void) display_inventory((char *)0, FALSE);
	return 0;*/

	char c;
	struct obj *otmp;
	c = display_inventory((char *)0, TRUE);
	if (!c) return 0;
	for (otmp = invent; otmp; otmp = otmp->nobj)
		if (otmp->invlet == c) break;
	if (otmp) return itemactions(otmp);
	return 0;

}

/*
 * find_unpaid()
 *
 * Scan the given list of objects.  If last_found is NULL, return the first
 * unpaid object found.  If last_found is not NULL, then skip over unpaid
 * objects until last_found is reached, then set last_found to NULL so the
 * next unpaid object is returned.  This routine recursively follows
 * containers.
 */
STATIC_OVL struct obj *
find_unpaid(list, last_found)
    struct obj *list, **last_found;
{
    struct obj *obj;

    while (list) {
	if (list->unpaid) {
	    if (*last_found) {
		/* still looking for previous unpaid object */
		if (list == *last_found)
		    *last_found = (struct obj *) 0;
	    } else
		return (*last_found = list);
	}
	if (Has_contents(list)) {
	    if ((obj = find_unpaid(list->cobj, last_found)) != 0)
		return obj;
	}
	list = list->nobj;
    }
    return (struct obj *) 0;
}

/*
 * Internal function used by display_inventory and getobj that can display
 * inventory and return a count as well as a letter. If out_cnt is not null,
 * any count returned from the menu selection is placed here.
 */
static char
display_pickinv(lets, want_reply, out_cnt)
register const char *lets;
boolean want_reply;
long* out_cnt;
{
	struct obj *otmp;
	char ilet, ret;
	char *invlet = flags.inv_order;
	int n, classcount;
	winid win;				/* windows being used */
	static winid local_win = WIN_ERR;	/* window for partial menus */
	anything any;
	menu_item *selected;
#ifdef PROXY_GRAPHICS
	static int busy = 0;
	if (busy)
	    return 0;
	busy++;
#endif

	/* overriden by global flag */
	if (flags.perm_invent) {
	    win = (lets && *lets) ? local_win : WIN_INVEN;
	    /* create the first time used */
	    if (win == WIN_ERR)
		win = local_win = create_nhwindow(NHW_MENU);
	} else
	    win = WIN_INVEN;

	if ( (InventoryLoss || u.uprops[INVENTORY_LOST].extrinsic || have_inventorylossstone()) && !program_state.gameover) {pline("Not enough memory to create inventory window");
 		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}	

	/*
	Exit early if no inventory -- but keep going if we are doing
	a permanent inventory update.  We need to keep going so the
	permanent inventory window updates itself to remove the last
	item(s) dropped.  One down side:  the addition of the exception
	for permanent inventory window updates _can_ pop the window
	up when it's not displayed -- even if it's empty -- because we
	don't know at this level if its up or not.  This may not be
	an issue if empty checks are done before hand and the call
	to here is short circuited away.
	*/
	if (!invent && !(flags.perm_invent && !lets && !want_reply)) {
#ifndef GOLDOBJ
	    pline("Not carrying anything%s.", u.ugold ? " except gold" : "");
#else
	    pline("Not carrying anything.");
#endif
#ifdef PROXY_GRAPHICS
	    busy--;
#endif
	    return 0;
	}

	/* oxymoron? temporarily assign permanent inventory letters */
	if (!flags.invlet_constant) reassign();

	if (lets && strlen(lets) == 1) {
	    /* when only one item of interest, use pline instead of menus;
	       we actually use a fake message-line menu in order to allow
	       the user to perform selection at the --More-- prompt for tty */
	    ret = '\0';
	    for (otmp = invent; otmp; otmp = otmp->nobj) {
		if (otmp->invlet == lets[0]) {
		    ret = message_menu(lets[0],
			  want_reply ? PICK_ONE : PICK_NONE,
			  xprname(otmp, (char *)0, lets[0], TRUE, 0L, 0L));
		    if (out_cnt) *out_cnt = -1L;	/* select all */
		    break;
		}
	    }
#ifdef PROXY_GRAPHICS
	    busy--;
#endif
	    return ret;
	}

	start_menu(win);
nextclass:
	classcount = 0;
	any.a_void = 0;		/* set all bits to zero */
	for(otmp = invent; otmp; otmp = otmp->nobj) {
		ilet = otmp->invlet;
		if(!lets || !*lets || index(lets, ilet)) {
			if (!flags.sortpack || otmp->oclass == *invlet) {
			    if (flags.sortpack && !classcount) {
				any.a_void = 0;		/* zero */
				add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
				    let_to_name(*invlet, FALSE, FALSE), MENU_UNSELECTED);
				classcount++;
			    }
			    any.a_char = ilet;
			    add_menu(win, obj_to_glyph(otmp),
					&any, ilet, 0, ATR_NONE, doname(otmp),
					MENU_UNSELECTED);
			}
		}
	}
	if (flags.sortpack) {
		if (*++invlet) goto nextclass;
#ifdef WIZARD
		if (--invlet != venom_inv) {
			invlet = venom_inv;
			goto nextclass;
		}
#endif
	}
	end_menu(win, (char *) 0);

	n = select_menu(win, want_reply ? PICK_ONE : PICK_NONE, &selected);
	if (n > 0) {
	    ret = selected[0].item.a_char;
	    if (out_cnt) *out_cnt = selected[0].count;
	    free((genericptr_t)selected);
	} else
	    ret = !n ? '\0' : '\033';	/* cancelled */

#ifdef PROXY_GRAPHICS
	busy--;
#endif
	return ret;
}

/*
 * If lets == NULL or "", list all objects in the inventory.  Otherwise,
 * list all objects with object classes that match the order in lets.
 *
 * Returns the letter identifier of a selected item, or 0 if nothing
 * was selected.
 */
char
display_inventory(lets, want_reply)
register const char *lets;
boolean want_reply;
{
	if ( (InventoryLoss || u.uprops[INVENTORY_LOST].extrinsic || have_inventorylossstone()) && !program_state.gameover) {pline("Not enough memory to create inventory window");
 		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	return display_pickinv(lets, want_reply, (long *)0);
}

/*
 * Returns the number of unpaid items within the given list.  This includes
 * contained objects.
 */
int
count_unpaid(list)
    struct obj *list;
{
    int count = 0;

    while (list) {
	if (list->unpaid) count++;
	if (Has_contents(list))
	    count += count_unpaid(list->cobj);
	list = list->nobj;
    }
    return count;
}

/*
 * Returns the number of items with b/u/c/unknown within the given list.  
 * This does NOT include contained objects.
 */
int
count_buc(list, type)
    struct obj *list;
    int type;
{
    int count = 0;

    while (list) {
	if (Role_if(PM_PRIEST) || Role_if(PM_NECROMANCER) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA)) list->bknown = TRUE;
	switch(type) {
	    case BUC_BLESSED:
		if (list->oclass != COIN_CLASS && list->bknown && list->blessed)
		    count++;
		break;
	    case BUC_CURSED:
		if (list->oclass != COIN_CLASS && list->bknown && list->cursed)
		    count++;
		break;
	    case BUC_UNCURSED:
		if (list->oclass != COIN_CLASS &&
			list->bknown && !list->blessed && !list->cursed)
		    count++;
		break;
	    case BUC_UNKNOWN:
		if (list->oclass != COIN_CLASS && !list->bknown)
		    count++;
		break;
	    default:
		impossible("need count of curse status %d?", type);
		return 0;
	}
	list = list->nobj;
    }
    return count;
}

STATIC_OVL void
dounpaid()
{
    winid win;
    struct obj *otmp, *marker;
    register char ilet;
    char *invlet = flags.inv_order;
    int classcount, count, num_so_far;
    int save_unpaid = 0;	/* lint init */
    long cost, totcost;

    count = count_unpaid(invent);

    if (count == 1) {
	marker = (struct obj *) 0;
	otmp = find_unpaid(invent, &marker);

	/* see if the unpaid item is in the top level inventory */
	for (marker = invent; marker; marker = marker->nobj)
	    if (marker == otmp) break;

	pline("%s", xprname(otmp, distant_name(otmp, doname),
			    marker ? otmp->invlet : CONTAINED_SYM,
			    TRUE, unpaid_cost(otmp), 0L));
	return;
    }

    win = create_nhwindow(NHW_MENU);
    cost = totcost = 0;
    num_so_far = 0;	/* count of # printed so far */
    if (!flags.invlet_constant) reassign();

    do {
	classcount = 0;
	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    ilet = otmp->invlet;
	    if (otmp->unpaid) {
		if (!flags.sortpack || otmp->oclass == *invlet) {
		    if (flags.sortpack && !classcount) {
			putstr(win, 0, let_to_name(*invlet, TRUE, FALSE));
			classcount++;
		    }

		    totcost += cost = unpaid_cost(otmp);
		    /* suppress "(unpaid)" suffix */
		    save_unpaid = otmp->unpaid;
		    otmp->unpaid = 0;
		    putstr(win, 0, xprname(otmp, distant_name(otmp, doname),
					   ilet, TRUE, cost, 0L));
		    otmp->unpaid = save_unpaid;
		    num_so_far++;
		}
	    }
	}
    } while (flags.sortpack && (*++invlet));

    if (count > num_so_far) {
	/* something unpaid is contained */
	if (flags.sortpack)
	    putstr(win, 0, let_to_name(CONTAINED_SYM, TRUE, FALSE));
	/*
	 * Search through the container objects in the inventory for
	 * unpaid items.  The top level inventory items have already
	 * been listed.
	 */
	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if (Has_contents(otmp)) {
		marker = (struct obj *) 0;	/* haven't found any */
		while (find_unpaid(otmp->cobj, &marker)) {
		    totcost += cost = unpaid_cost(marker);
		    save_unpaid = marker->unpaid;
		    marker->unpaid = 0;    /* suppress "(unpaid)" suffix */
		    putstr(win, 0,
			   xprname(marker, distant_name(marker, doname),
				   CONTAINED_SYM, TRUE, cost, 0L));
		    marker->unpaid = save_unpaid;
		}
	    }
	}
    }

    putstr(win, 0, "");
    putstr(win, 0, xprname((struct obj *)0, "Total:", '*', FALSE, totcost, 0L));
    display_nhwindow(win, FALSE);
    destroy_nhwindow(win);
}


/* query objlist callback: return TRUE if obj type matches "this_type" */
static int this_type;

STATIC_OVL boolean
this_type_only(obj)
    struct obj *obj;
{
    return (obj->oclass == this_type);
}

/* the 'I' command */
int
dotypeinv()
{
	char c = '\0';
	int n, i = 0;
	char *extra_types, types[BUFSZ];
	int class_count, oclass, unpaid_count, itemcount;
	boolean billx = *u.ushops && doinvbill(0);
	menu_item *pick_list;
	boolean traditional = TRUE;
	const char *prompt = "What type of object do you want an inventory of?";

#ifndef GOLDOBJ
	if (!invent && !u.ugold && !billx) {
#else
	if (!invent && !billx) {
#endif
	    You("aren't carrying anything.");
	    return 0;
	}
	unpaid_count = count_unpaid(invent);
	if (flags.menu_style != MENU_TRADITIONAL) {
	    if (flags.menu_style == MENU_FULL ||
				flags.menu_style == MENU_PARTIAL) {
		traditional = FALSE;
		i = UNPAID_TYPES;
		if (billx) i |= BILLED_TYPES;
		n = query_category(prompt, invent, i, &pick_list, PICK_ONE);
		if (!n) return 0;
		this_type = c = pick_list[0].item.a_int;
		free((genericptr_t) pick_list);
	    }
	}
	if (traditional) {
	    /* collect a list of classes of objects carried, for use as a prompt */
	    types[0] = 0;
	    class_count = collect_obj_classes(types, invent,
					      FALSE,
#ifndef GOLDOBJ
					      (u.ugold != 0),
#endif
					      (boolean FDECL((*),(OBJ_P))) 0, &itemcount);
	    if (unpaid_count) {
		Strcat(types, "u");
		class_count++;
	    }
	    if (billx) {
		Strcat(types, "x");
		class_count++;
	    }
	    /* add everything not already included; user won't see these */
	    extra_types = eos(types);
	    *extra_types++ = '\033';
	    if (!unpaid_count) *extra_types++ = 'u';
	    if (!billx) *extra_types++ = 'x';
	    *extra_types = '\0';	/* for index() */
	    for (i = 0; i < MAXOCLASSES; i++)
		if (!index(types, def_oc_syms[i])) {
		    *extra_types++ = def_oc_syms[i];
		    *extra_types = '\0';
		}

	    if(class_count > 1) {
		c = yn_function(prompt, types, '\0');
#ifdef REDO
		savech(c);
#endif
		if(c == '\0') {
			clear_nhwindow(WIN_MESSAGE);
			return 0;
		}
	    } else {
		/* only one thing to itemize */
		if (unpaid_count)
		    c = 'u';
		else if (billx)
		    c = 'x';
		else
		    c = types[0];
	    }
	}
	if (c == 'x') {
	    if (billx)
		(void) doinvbill(1);
	    else
		pline("No used-up objects on your shopping bill.");
	    return 0;
	}
	if (c == 'u') {
	    if (unpaid_count)
		dounpaid();
	    else
		You("are not carrying any unpaid objects.");
	    return 0;
	}
	if (traditional) {
	    oclass = def_char_to_objclass(c); /* change to object class */
	    if (oclass == COIN_CLASS) {
		return doprgold();
	    } else if (index(types, c) > index(types, '\033')) {
		You("have no such objects.");
		return 0;
	    }
	    this_type = oclass;
	}
	if (query_objlist((char *) 0, invent,
		    (flags.invlet_constant ? USE_INVLET : 0)|INVORDER_SORT,
		    &pick_list, PICK_NONE, this_type_only) > 0)
	    free((genericptr_t)pick_list);
	return 0;
}

/* return a string describing the dungeon feature at <x,y> if there
   is one worth mentioning at that location; otherwise null */
const char *
dfeature_at(x, y, buf)
int x, y;
char *buf;
{
	struct rm *lev = &levl[x][y];
	int ltyp = lev->typ, cmap = -1;
	const char *dfeature = 0;
	static char altbuf[BUFSZ];

	if (IS_DOOR(ltyp)) {
	    switch (lev->doormask) {
	    case D_NODOOR:	cmap = S_ndoor; break;	/* "doorway" */
	    case D_ISOPEN:	cmap = S_vodoor; break;	/* "open door" */
	    case D_BROKEN:	dfeature = "broken door"; break;
	    default:	cmap = S_vcdoor; break;	/* "closed door" */
	    }
	    /* override door description for open drawbridge */
	    if (is_drawbridge_wall(x, y) >= 0)
		dfeature = "open drawbridge portcullis",  cmap = -1;
	} else if (IS_FOUNTAIN(ltyp))
	    cmap = S_fountain;				/* "fountain" */
	else if (IS_THRONE(ltyp))
	    cmap = S_throne;				/* "opulent throne" */
	else if (is_lava(x,y))
	    cmap = S_lava;				/* "molten lava" */
	else if (is_ice(x,y))
	    cmap = S_ice;				/* "ice" */
	else if (is_pool(x,y))
	    dfeature = "pool of water";
#ifdef SINKS
	else if (IS_SINK(ltyp))
	    cmap = S_sink;				/* "sink" */
	else if (IS_TOILET(ltyp))
	    cmap = S_toilet;
#endif
	else if (IS_ALTAR(ltyp)) {
	    Sprintf(altbuf, "altar to %s (%s)", a_gname(),
		    align_str(Amask2align(lev->altarmask & ~AM_SHRINE)));
	    dfeature = altbuf;
	} else if ((x == xupstair && y == yupstair) ||
		 (x == sstairs.sx && y == sstairs.sy && sstairs.up))
	    cmap = S_upstair;				/* "staircase up" */
	else if ((x == xdnstair && y == ydnstair) ||
		 (x == sstairs.sx && y == sstairs.sy && !sstairs.up))
	    cmap = S_dnstair;				/* "staircase down" */
	else if (x == xupladder && y == yupladder)
	    cmap = S_upladder;				/* "ladder up" */
	else if (x == xdnladder && y == ydnladder)
	    cmap = S_dnladder;				/* "ladder down" */
	else if (ltyp == DRAWBRIDGE_DOWN)
	    cmap = S_vodbridge;			/* "lowered drawbridge" */
	else if (ltyp == DBWALL)
	    cmap = S_vcdbridge;			/* "raised drawbridge" */
	else if (IS_GRAVE(ltyp))
	    cmap = S_grave;				/* "grave" */
	else if (ltyp == TREE)
	    cmap = S_tree;				/* "tree" */
	else if (ltyp == IRONBARS)
	    dfeature = "set of iron bars";

	if (cmap >= 0) dfeature = defsyms[cmap].explanation;
	if (dfeature) Strcpy(buf, dfeature);
	return dfeature;
}

/* look at what is here; if there are many objects (5 or more),
   don't show them unless obj_cnt is 0 */
int
look_here(obj_cnt, picked_some)
int obj_cnt;	/* obj_cnt > 0 implies that autopickup is in progess */
boolean picked_some;
{
	struct obj *otmp;
	struct trap *trap;
	const char *verb = Blind ? "feel" : "see";
	const char *dfeature = (char*) 0;
	char fbuf[BUFSZ], fbuf2[BUFSZ];
	winid tmpwin;
	boolean skip_objects = (obj_cnt >= 5), felt_cockatrice = FALSE;

	if (u.uswallow && u.ustuck) {
	    struct monst *mtmp = u.ustuck;
	    Sprintf(fbuf, "Contents of %s %s",
		s_suffix(mon_nam(mtmp)), mbodypart(mtmp, STOMACH));
	    /* Skip "Contents of " by using fbuf index 12 */
	    You("%s to %s what is lying in %s.",
		Blind ? "try" : "look around", verb, &fbuf[12]);
	    otmp = mtmp->minvent;
	    if (otmp) {
		for ( ; otmp; otmp = otmp->nobj) {
			/* If swallower is an animal, it should have become stone but... */
			if (otmp->otyp == CORPSE || otmp->otyp == EGG) feel_cockatrice(otmp, FALSE);
		}
		if (Blind) Strcpy(fbuf, "You feel");
		Strcat(fbuf,":");
	    	(void) display_minventory(mtmp, MINV_ALL, fbuf);
	    } else {
		You("%s no objects here.", verb);
	    }
	    return(!!Blind);
	}
	if (!skip_objects && (trap = t_at(u.ux,u.uy)) && trap->tseen)
		There("is %s here.",
			an(defsyms[trap_to_defsym(trap->ttyp)].explanation));

	otmp = level.objects[u.ux][u.uy];
	dfeature = dfeature_at(u.ux, u.uy, fbuf2);
	if (dfeature && !strcmp(dfeature, "pool of water") && Underwater)
		dfeature = 0;

	if (Blind) {
		boolean drift = Is_airlevel(&u.uz) || Is_waterlevel(&u.uz);
		if (dfeature && !strncmp(dfeature, "altar ", 6)) {
		    /* don't say "altar" twice, dfeature has more info */
		    You("try to feel what is here.");
		} else {
		    You("try to feel what is %s%s.",
			drift ? "floating here" : "lying here on the ",
			drift ? ""		: surface(u.ux, u.uy));
		}
		if (dfeature && !drift && !strcmp(dfeature, surface(u.ux,u.uy)))
			dfeature = 0;		/* ice already identifed */
		if (!can_reach_floor()) {
			pline(Hallucination ? "But it seems the stuff actively tries to evade your grasp!" : "But you can't reach it!");
			return(0);
		}
	}

	if (dfeature) {
		Sprintf(fbuf, "There is %s here.", an(dfeature));
		if (flags.suppress_alert < FEATURE_NOTICE_VER(0,0,7) &&
			(IS_FOUNTAIN(levl[u.ux][u.uy].typ) ||
#ifdef SINKS
			 IS_SINK(levl[u.ux][u.uy].typ) ||
			 IS_TOILET(levl[u.ux][u.uy].typ)
#endif
			))
		    Strcat(fbuf, "  Use \"q.\" to drink from it.");
	}

	if (!otmp || is_lava(u.ux,u.uy) || (is_pool(u.ux,u.uy) && !Underwater)) {
		if (dfeature) pline(fbuf);
		sense_engr_at(u.ux, u.uy, FALSE); /* Eric Backus */
		if (!skip_objects && (Blind || !dfeature))
		    You("%s no objects here.", verb);
		return(!!Blind);
	}
	/* we know there is something here */

	if (skip_objects) {
	    if (dfeature) pline(fbuf);
	    sense_engr_at(u.ux, u.uy, FALSE); /* Eric Backus */
	    There("are %s%s objects here.",
		  (obj_cnt <= 10) ? "several" : "many",
		  picked_some ? " more" : "");
	} else if (!otmp->nexthere) {
	    /* only one object */
	    if (dfeature) pline(fbuf);
	    sense_engr_at(u.ux, u.uy, FALSE); /* Eric Backus */
#ifdef INVISIBLE_OBJECTS
	    if (otmp->oinvis && !See_invisible) verb = "feel";
#endif
	    You("%s here %s.", verb, doname(otmp));
	    if (otmp->otyp == CORPSE || otmp->otyp == EGG) feel_cockatrice(otmp, FALSE);
	} else {
	    display_nhwindow(WIN_MESSAGE, FALSE);
	    tmpwin = create_nhwindow(NHW_MENU);
	    if(dfeature) {
		putstr(tmpwin, 0, fbuf);
		putstr(tmpwin, 0, "");
	    }
	    putstr(tmpwin, 0, Blind ? "Things that you feel here:" :
				      "Things that are here:");
	    for ( ; otmp; otmp = otmp->nexthere) {
		if ( (otmp->otyp == CORPSE || otmp->otyp == EGG) && will_feel_cockatrice(otmp, FALSE)) {
			char buf[BUFSZ];
			felt_cockatrice = TRUE;
			Strcpy(buf, doname(otmp));
			Strcat(buf, "...");
			putstr(tmpwin, 0, buf);
			break;
		}
		putstr(tmpwin, 0, doname(otmp));
	    }
	    display_nhwindow(tmpwin, TRUE);
	    destroy_nhwindow(tmpwin);
	    if (felt_cockatrice) feel_cockatrice(otmp, FALSE);
	    sense_engr_at(u.ux, u.uy, FALSE); /* Eric Backus */
	}
	return(!!Blind);
}

/* explicilty look at what is here, including all objects */
int
dolook()
{
	return look_here(0, FALSE);
}

boolean
will_feel_cockatrice(otmp, force_touch)
struct obj *otmp;
boolean force_touch;
{
	if ((Blind || force_touch) && !uarmg && !Stone_resistance &&
		(otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm])))
			return TRUE;
	if ((Blind || force_touch) && !uarmg && !Stone_resistance &&
		(otmp->otyp == EGG && touch_petrifies(&mons[otmp->corpsenm])))
			return TRUE;
	return FALSE;
}

void
feel_cockatrice(otmp, force_touch)
struct obj *otmp;
boolean force_touch;
{
	char kbuf[BUFSZ];

	if (will_feel_cockatrice(otmp, force_touch)) {
	    if(poly_when_stoned(youmonst.data))
			You("touched the %s corpse with your bare %s.",
				mons[otmp->corpsenm].mname, makeplural(body_part(HAND)));
	    else
			pline("Touching the %s corpse is a fatal mistake...",
				mons[otmp->corpsenm].mname);
		Sprintf(kbuf, "%s corpse", an(mons[otmp->corpsenm].mname));
		instapetrify(kbuf);
	}
}

#endif /* OVLB */
#ifdef OVL1

void
stackobj(obj)
struct obj *obj;
{
	struct obj *otmp;

	for(otmp = level.objects[obj->ox][obj->oy]; otmp; otmp = otmp->nexthere)
		if(otmp != obj && merged(&obj,&otmp))
			break;
	return;
}

STATIC_OVL boolean
mergable(otmp, obj)	/* returns TRUE if obj  & otmp can be merged */
/* obj is being merged into otmp --Amy */
	register struct obj *otmp, *obj;
{
	if (obj->otyp != otmp->otyp) return FALSE;
#ifdef GOLDOBJ
	/* coins of the same kind will always merge */
	if (obj->oclass == COIN_CLASS) return TRUE;
#endif
	if (obj->unpaid != otmp->unpaid ||
	    obj->spe != otmp->spe || (obj->dknown && !otmp->dknown) ||
	    (obj->bknown && !otmp->bknown && !(Role_if(PM_PRIEST) || Role_if(PM_NECROMANCER) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA) ) ) ||
	    obj->cursed != otmp->cursed || obj->blessed != otmp->blessed ||
	    obj->hvycurse != otmp->hvycurse || obj->prmcurse != otmp->prmcurse ||
	    obj->no_charge != otmp->no_charge ||
	    obj->obroken != otmp->obroken ||
	    obj->otrapped != otmp->otrapped ||
	    obj->lamplit != otmp->lamplit ||
	    flags.pickup_thrown && obj->was_thrown != otmp->was_thrown ||
#ifdef INVISIBLE_OBJECTS
	    obj->oinvis != otmp->oinvis ||
#endif
#ifdef UNPOLYPILE
	    obj->oldtyp != otmp->oldtyp ||
#endif
	    obj->greased != otmp->greased ||
	    obj->oeroded != otmp->oeroded ||
	    obj->oeroded2 != otmp->oeroded2 ||
	    obj->bypass != otmp->bypass)
	    return(FALSE);

	if ((obj->oclass==WEAPON_CLASS || obj->oclass==ARMOR_CLASS) &&
	    (obj->oerodeproof!=otmp->oerodeproof || (obj->rknown && !otmp->rknown) ))
	    return FALSE;

	if (obj->oclass == FOOD_CLASS && (obj->oeaten != otmp->oeaten ||
	  obj->odrained != otmp->odrained || obj->orotten != otmp->orotten))
	    return(FALSE);

	if (obj->otyp == CORPSE || obj->otyp == EGG || obj->otyp == TIN) {
		if (obj->corpsenm != otmp->corpsenm)
				return FALSE;
	}

	/* armed grenades do not merge */
	if ((obj->timed || otmp->timed) && is_grenade(obj))
	    return FALSE;

	/* hatching eggs don't merge; ditto for revivable corpses */
	if ((obj->timed || otmp->timed) && (obj->otyp == EGG ||
	    (obj->otyp == CORPSE && otmp->corpsenm >= LOW_PM &&
		 is_reviver(&mons[otmp->corpsenm]))))
	    return FALSE;

	/* allow candle merging only if their ages are close */
	/* see begin_burn() for a reference for the magic "25" */
	/* [ALI] Slash'EM can't rely on using 25, because we
	 * have chosen to reduce the cost of candles such that
	 * the initial age is no longer a multiple of 25. The
	 * simplest solution is just to use 20 instead, since
	 * initial candle age is always a multiple of 20.
	 */
	if ((obj->otyp == TORCH || Is_candle(obj)) && obj->age/20 != otmp->age/20)
	    return(FALSE);

	/* burning potions of oil never merge */
	/* MRKR: nor do burning torches */
	if ((obj->otyp == POT_OIL || obj->otyp == TORCH) && obj->lamplit)
	    return FALSE;

	/* don't merge surcharged item with base-cost item */
	if (obj->unpaid && !same_price(obj, otmp))
	    return FALSE;

	/* if they have names, make sure they're the same */
	if ( (obj->onamelth != otmp->onamelth &&
		((obj->onamelth && otmp->onamelth) || obj->otyp == CORPSE)
	     ) ||
	    (obj->onamelth && otmp->onamelth &&
		    strncmp(ONAME(obj), ONAME(otmp), (int)obj->onamelth)))
		return FALSE;

	/* for the moment, any additional information is incompatible */
	if (obj->oxlth || otmp->oxlth) return FALSE;

	if(obj->oartifact != otmp->oartifact) return FALSE;

	if(obj->known == otmp->known || (otmp->known) ||
		!objects[otmp->otyp].oc_uses_known) {
		return((boolean)(objects[obj->otyp].oc_merge));
	} else return(FALSE);
}

/* Manipulating a stack of items is supposed to fail if the stack is very big. --Amy
 * This sounds evil, but if you think about it for a while it makes sense: why should a scroll of enchant weapon
 * have the same odds of enchanting a stack of 5 or 500 darts? That way, players would be well-advised to never use them
 * because in the case of doubt they'll find more darts to make an even bigger stack to enchant all at once!
 * And the vanilla behavior also means that finding a random stack of +5 darts is of no use since you can always make
 * a much bigger one with a few scrolls. On the other hand, water damage, cancellation etc. has the same chance of
 * ruining your stack of 15 teleportation scrolls all at once, which doesn't really make sense either. The best
 * solution would be allowing each individual item to perform a saving throw to see whether it is affected,
 * but lacking that, I'll just allow stacks to perform a saving throw against manipulation.
 * It will affect both "positive" and "negative" effects equally. */

boolean
stack_too_big(otmp)
register struct obj *otmp;
{
	/* returns 0 if the operation can be done on the stack, 1 if it will fail */

	if (!objects[otmp->otyp].oc_merge) return 0;

	if ( ( (objects[otmp->otyp].oc_skill == P_DAGGER) || (objects[otmp->otyp].oc_skill == P_KNIFE) || (objects[otmp->otyp].oc_skill == P_SPEAR) || (objects[otmp->otyp].oc_skill == P_JAVELIN) || (objects[otmp->otyp].oc_skill == P_BOOMERANG) || (otmp->otyp == WAX_CANDLE) || (otmp->otyp == TALLOW_CANDLE) || (otmp->otyp == MAGIC_CANDLE) || (otmp->otyp == TORCH) ) && (rnd(otmp->quan) > 10 ) ) return 1;

	if ( ( (objects[otmp->otyp].oc_skill == P_DART) || (objects[otmp->otyp].oc_skill == P_SHURIKEN) || (objects[otmp->otyp].oc_skill == -P_BOW) || (objects[otmp->otyp].oc_skill == -P_SLING) || (objects[otmp->otyp].oc_skill == -P_CROSSBOW) || (objects[otmp->otyp].oc_skill == -P_FIREARM) || (otmp->otyp == SPOON) || (objects[otmp->otyp].oc_class == VENOM_CLASS) ) && (rnd(otmp->quan) > 25 ) ) return 1;

	if ( ( (objects[otmp->otyp].oc_class == SCROLL_CLASS) || (objects[otmp->otyp].oc_class == POTION_CLASS) || (objects[otmp->otyp].oc_class == FOOD_CLASS)) && (rnd(otmp->quan) > 1 ) ) return 1;


	else return 0;
}


int
doprgold()
{
	/* the messages used to refer to "carrying gold", but that didn't
	   take containers into account */
#ifndef GOLDOBJ
	if(!u.ugold)
	    Your("wallet is empty.");
	else
	    Your("wallet contains %ld gold piece%s.", u.ugold, plur(u.ugold));
#else
        long umoney = money_cnt(invent);
	if(!umoney)
	    Your("wallet is empty.");
	else
	    Your("wallet contains %ld %s.", umoney, currency(umoney));
#endif
	shopper_financial_report();
	return 0;
}

#endif /* OVL1 */
#ifdef OVLB

int
doprwep()
{
    if (!uwep) {
	if (!u.twoweap){
	You("are empty %s.", body_part(HANDED));
	    return 0;
	}
	/* Avoid printing "right hand empty" and "other hand empty" */
	if (!uswapwep) {
	    You("are attacking with both %s.", makeplural(body_part(HAND)));
	    return 0;
	}
	Your("right %s is empty.", body_part(HAND));
    } else {
	prinv((char *)0, uwep, 0L);
    }
    if (u.twoweap) {
    	if (uswapwep)
    	    prinv((char *)0, uswapwep, 0L);
    	else
    	    Your("other %s is empty.", body_part(HAND));
    }
    return 0;
#if 0
	if(!uwep && !uswapwep && !uquiver) You("are empty %s.", body_part(HANDED));
	else {
		char lets[3];
		register int ct = 0;

		if(uwep) lets[ct++] = obj_to_let(uwep);
		if(uswapwep) lets[ct++] = obj_to_let(uswapwep);
		if(uquiver) lets[ct++] = obj_to_let(uquiver);
		lets[ct] = 0;
		(void) display_inventory(lets, FALSE);
    }
    return 0;
#endif
}

int
doprarm()
{
	if(!wearing_armor())
		You("are not wearing any armor.");
	else {
#ifdef TOURIST
		char lets[8];
#else
		char lets[7];
#endif
		register int ct = 0;

#ifdef TOURIST
		if(uarmu) lets[ct++] = obj_to_let(uarmu);
#endif
		if(uarm) lets[ct++] = obj_to_let(uarm);
		if(uarmc) lets[ct++] = obj_to_let(uarmc);
		if(uarmh) lets[ct++] = obj_to_let(uarmh);
		if(uarms) lets[ct++] = obj_to_let(uarms);
		if(uarmg) lets[ct++] = obj_to_let(uarmg);
		if(uarmf) lets[ct++] = obj_to_let(uarmf);
		lets[ct] = 0;
		(void) display_inventory(lets, FALSE);
	}
	return 0;
}

int
doprring()
{
	if(!uleft && !uright)
		You("are not wearing any rings.");
	else {
		char lets[3];
		register int ct = 0;

		if(uleft) lets[ct++] = obj_to_let(uleft);
		if(uright) lets[ct++] = obj_to_let(uright);
		lets[ct] = 0;
		(void) display_inventory(lets, FALSE);
	}
	return 0;
}

int
dopramulet()
{
	if (!uamul)
		You("are not wearing an amulet.");
	else
		prinv((char *)0, uamul, 0L);
	return 0;
}

STATIC_OVL boolean
tool_in_use(obj)
struct obj *obj;
{
	if ((obj->owornmask & (W_TOOL
#ifdef STEED
			| W_SADDLE
#endif
			)) != 0L) return TRUE;
	if (obj->oclass != TOOL_CLASS) return FALSE;
	return (boolean)(obj == uwep || obj->lamplit ||
				(obj->otyp == LEASH && obj->leashmon));
}

int
doprtool()
{
	struct obj *otmp;
	int ct = 0;
	char lets[52+1];

	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (tool_in_use(otmp))
		lets[ct++] = obj_to_let(otmp);
	lets[ct] = '\0';
	if (!ct) You("are not using any tools.");
	else (void) display_inventory(lets, FALSE);
	return 0;
}

/* '*' command; combines the ')' + '[' + '=' + '"' + '(' commands;
   show inventory of all currently wielded, worn, or used objects */
int
doprinuse()
{
	struct obj *otmp;
	int ct = 0;
	char lets[52+1];

	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (is_worn(otmp) || tool_in_use(otmp))
		lets[ct++] = obj_to_let(otmp);
	lets[ct] = '\0';
	if (!ct) You("are not wearing or wielding anything.");
	else (void) display_inventory(lets, FALSE);
	return 0;
}

/*
 * uses up an object that's on the floor, charging for it as necessary
 */
void
useupf(obj, numused)
register struct obj *obj;
long numused;
{
	register struct obj *otmp;
	boolean at_u = (obj->ox == u.ux && obj->oy == u.uy);

	/* burn_floor_paper() keeps an object pointer that it tries to
	 * useupf() multiple times, so obj must survive if plural */
	if (obj->quan > numused) {
		otmp = splitobj(obj, numused);
		obj->in_use = FALSE;		/* rest no longer in use */
	}
	else
		otmp = obj;
	if(costly_spot(otmp->ox, otmp->oy)) {
	    if(index(u.urooms, *in_rooms(otmp->ox, otmp->oy, 0)))
	        addtobill(otmp, FALSE, FALSE, FALSE);
	    else (void)stolen_value(otmp, otmp->ox, otmp->oy, FALSE, FALSE,
		    TRUE);
	}
	delobj(otmp);
	if (at_u && u.uundetected && hides_under(youmonst.data))
	    u.uundetected = OBJ_AT(u.ux, u.uy);
}

#endif /* OVLB */


#ifdef OVL1

/*
 * Conversion from a class to a string for printing.
 * This must match the object class order.
 */
STATIC_VAR NEARDATA const char *names[] = { 0,
	"Illegal objects", "Weapons", "Armor", "Rings", "Amulets",
	"Tools", "Comestibles", "Potions", "Scrolls", "Spellbooks",
	"Wands", "Coins", "Gems", "Boulders/Statues", "Iron balls",
	"Chains", "Venoms"
};

static NEARDATA const char oth_symbols[] = {
	CONTAINED_SYM,
	'\0'
};

static NEARDATA const char *oth_names[] = {
	"Bagged/Boxed items"
};

static NEARDATA char *invbuf = (char *)0;
static NEARDATA unsigned invbufsiz = 0;

char *
let_to_name(let,unpaid,showsym)
char let;
boolean unpaid,showsym;
{
	static const char *ocsymformat = "%s('%c')";
	const char *class_name;
	const char *pos;
	int oclass = (let >= 1 && let < MAXOCLASSES) ? let : 0;
	unsigned len;

	if (oclass)
	    class_name = names[oclass];
	else if ((pos = index(oth_symbols, let)) != 0)
	    class_name = oth_names[pos - oth_symbols];
	else
	    class_name = names[0];

	len = strlen(class_name) + (unpaid ? sizeof "unpaid_" : sizeof "") +
	    ((oclass && showsym) ? strlen(ocsymformat) : 0);
	if (len > invbufsiz) {
	    if (invbuf) free((genericptr_t)invbuf);
	    invbufsiz = len + 10; /* add slop to reduce incremental realloc */
	    invbuf = (char *) alloc(invbufsiz);
	}
	if (unpaid)
	    Strcat(strcpy(invbuf, "Unpaid "), class_name);
	else
	    Strcpy(invbuf, class_name);
	if (oclass && showsym)
	    Sprintf(eos(invbuf), ocsymformat,
		    iflags.menu_tab_sep ? "\t" : "  ", def_oc_syms[let]);
	return invbuf;
}

void
free_invbuf()
{
	if (invbuf) free((genericptr_t)invbuf),  invbuf = (char *)0;
	invbufsiz = 0;
}

#endif /* OVL1 */
#ifdef OVLB

void
reassign()
{
	register int i;
	register struct obj *obj;

	for(obj = invent, i = 0; obj; obj = obj->nobj, i++)
		obj->invlet = (i < 26) ? ('a'+i) : ('A'+i-26);
	lastinvnr = i;
}

#endif /* OVLB */
#ifdef OVL1

int
doorganize()	/* inventory organizer by Del Lamb */
{
	struct obj *obj, *otmp;
	register int ix, cur;
	register char let;
	char alphabet[52+1], buf[52+1];
	char qbuf[QBUFSZ];
	char allowall[2];
	const char *adj_type;

	if (!flags.invlet_constant) reassign();
	/* get a pointer to the object the user wants to organize */
	allowall[0] = ALL_CLASSES; allowall[1] = '\0';
	if (!(obj = getobj(allowall,"adjust"))) return(0);

	/* initialize the list with all upper and lower case letters */
	for (let = 'a', ix = 0;  let <= 'z';) alphabet[ix++] = let++;
	for (let = 'A', ix = 26; let <= 'Z';) alphabet[ix++] = let++;
	alphabet[52] = 0;

	/* blank out all the letters currently in use in the inventory */
	/* except those that will be merged with the selected object   */
	for (otmp = invent; otmp; otmp = otmp->nobj)
		if (otmp != obj && !mergable(otmp,obj)) {
			if (otmp->invlet <= 'Z')
				alphabet[(otmp->invlet) - 'A' + 26] = ' ';
			else	alphabet[(otmp->invlet) - 'a']	    = ' ';
		}

	/* compact the list by removing all the blanks */
	for (ix = cur = 0; ix <= 52; ix++)
		if (alphabet[ix] != ' ') buf[cur++] = alphabet[ix];

	/* and by dashing runs of letters */
	if(cur > 5) compactify(buf);

	/* get new letter to use as inventory letter */
	for (;;) {
		Sprintf(qbuf, "Adjust letter to what [%s]?",buf);
		let = yn_function(qbuf, (char *)0, '\0');
		if(index(quitchars,let)) {
			pline(Never_mind);
			return(0);
		}
		if (let == '@' || !letter(let))
			pline("Select an inventory slot letter.");
		else
			break;
	}

	/* change the inventory and print the resulting item */
	adj_type = "Moving:";

	/*
	 * don't use freeinv/addinv to avoid double-touching artifacts,
	 * dousing lamps, losing luck, cursing loadstone, etc.
	 */
	extract_nobj(obj, &invent);

	for (otmp = invent; otmp;)
		if (merged(&otmp,&obj)) {
			adj_type = "Merging:";
			obj = otmp;
			otmp = otmp->nobj;
			extract_nobj(obj, &invent);
		} else {
			if (otmp->invlet == let) {
				adj_type = "Swapping:";
				otmp->invlet = obj->invlet;
			}
			otmp = otmp->nobj;
		}

	/* inline addinv (assuming flags.invlet_constant and !merged) */
	obj->invlet = let;
	obj->nobj = invent; /* insert at beginning */
	obj->where = OBJ_INVENT;
	invent = obj;
	reorder_invent();

	prinv(adj_type, obj, 0L);
	update_inventory();
	return(0);
}

/* common to display_minventory and display_cinventory */
STATIC_OVL void
invdisp_nothing(hdr, txt)
const char *hdr, *txt;
{
	winid win;
	anything any;
	menu_item *selected;

	any.a_void = 0;
	win = create_nhwindow(NHW_MENU);
	start_menu(win);
	add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings, hdr, MENU_UNSELECTED);
	add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, "", MENU_UNSELECTED);
	add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, txt, MENU_UNSELECTED);
	end_menu(win, (char *)0);
	if (select_menu(win, PICK_NONE, &selected) > 0)
	    free((genericptr_t)selected);
	destroy_nhwindow(win);
	return;
}

/* query_objlist callback: return things that could possibly be worn/wielded */
STATIC_OVL boolean
worn_wield_only(obj)
struct obj *obj;
{
    return (obj->oclass == WEAPON_CLASS
		|| obj->oclass == ARMOR_CLASS
		|| obj->oclass == AMULET_CLASS
		|| obj->oclass == RING_CLASS
		|| obj->oclass == TOOL_CLASS);
}

/*
 * Display a monster's inventory.
 * Returns a pointer to the object from the monster's inventory selected
 * or NULL if nothing was selected.
 *
 * By default, only worn and wielded items are displayed.  The caller
 * can pick one.  Modifier flags are:
 *
 *	MINV_NOLET	- nothing selectable
 *	MINV_ALL	- display all inventory
 */
struct obj *
display_minventory(mon, dflags, title)
register struct monst *mon;
int dflags;
char *title;
{
	struct obj *ret;
#ifndef GOLDOBJ
	struct obj m_gold;
#endif
	char tmp[QBUFSZ];
	int n;
	menu_item *selected = 0;
#ifndef GOLDOBJ
	int do_all = (dflags & MINV_ALL) != 0,
	    do_gold = (do_all && mon->mgold);
#else
	int do_all = (dflags & MINV_ALL) != 0;
#endif

	Sprintf(tmp,"%s %s:", s_suffix(noit_Monnam(mon)),
		do_all ? "possessions" : "armament");

#ifndef GOLDOBJ
	if (do_all ? (mon->minvent || mon->mgold)
#else
	if (do_all ? (mon->minvent != 0)
#endif
		   : (mon->misc_worn_check || MON_WEP(mon))) {
	    /* Fool the 'weapon in hand' routine into
	     * displaying 'weapon in claw', etc. properly.
	     */
	    youmonst.data = mon->data;

#ifndef GOLDOBJ
	    if (do_gold) {
		/*
		 * Make temporary gold object and insert at the head of
		 * the mon's inventory.  We can get away with using a
		 * stack variable object because monsters don't carry
		 * gold in their inventory, so it won't merge.
		 */
		m_gold = zeroobj;
		m_gold.otyp = GOLD_PIECE;  m_gold.oclass = COIN_CLASS;
		m_gold.quan = mon->mgold;  m_gold.dknown = 1;
		m_gold.where = OBJ_FREE;
		/* we had better not merge and free this object... */
		if (add_to_minv(mon, &m_gold))
		    panic("display_minventory: static object freed.");
	    }

#endif
	    n = query_objlist(title ? title : tmp, mon->minvent, INVORDER_SORT, &selected,
			(dflags & MINV_NOLET) ? PICK_NONE : PICK_ONE,
			do_all ? allow_all : worn_wield_only);

#ifndef GOLDOBJ
	    if (do_gold) obj_extract_self(&m_gold);
#endif

	    set_uasmon();
	} else {
	    invdisp_nothing(title ? title : tmp, "(none)");
	    n = 0;
	}

	if (n > 0) {
	    ret = selected[0].item.a_obj;
	    free((genericptr_t)selected);
#ifndef GOLDOBJ
	    /*
	     * Unfortunately, we can't return a pointer to our temporary
	     * gold object.  We'll have to work out a scheme where this
	     * can happen.  Maybe even put gold in the inventory list...
	     */
	    if (ret == &m_gold) ret = (struct obj *) 0;
#endif
	} else
	    ret = (struct obj *) 0;
	return ret;
}

/*
 * Display the contents of a container in inventory style.
 * Currently, this is only used for statues, via wand of probing.
 * [ALI] Also used when looting medical kits.
 */
struct obj *
display_cinventory(obj)
register struct obj *obj;
{
	struct obj *ret;
	char tmp[QBUFSZ];
	int n;
	menu_item *selected = 0;

	Sprintf(tmp,"Contents of %s:", doname(obj));

	if (obj->cobj) {
	    n = query_objlist(tmp, obj->cobj, INVORDER_SORT, &selected,
			    PICK_NONE, allow_all);
	} else {
	    invdisp_nothing(tmp, "(empty)");
	    n = 0;
	}
	if (n > 0) {
	    ret = selected[0].item.a_obj;
	    free((genericptr_t)selected);
	} else
	    ret = (struct obj *) 0;
	return ret;
}

/* query objlist callback: return TRUE if obj is at given location */
static coord only;

STATIC_OVL boolean
only_here(obj)
    struct obj *obj;
{
    return (obj->ox == only.x && obj->oy == only.y);
}

/*
 * Display a list of buried items in inventory style.  Return a non-zero
 * value if there were items at that spot.
 *
 * Currently, this is only used with a wand of probing zapped downwards.
 */
int
display_binventory(x, y, as_if_seen)
int x, y;
boolean as_if_seen;
{
	struct obj *obj;
	menu_item *selected = 0;
	int n;

	/* count # of objects here */
	for (n = 0, obj = level.buriedobjlist; obj; obj = obj->nobj)
	    if (obj->ox == x && obj->oy == y) {
		if (as_if_seen) obj->dknown = 1;
		n++;
	    }

	if (n) {
	    only.x = x;
	    only.y = y;
	    if (query_objlist("Things that are buried here:",
			      level.buriedobjlist, INVORDER_SORT,
			      &selected, PICK_NONE, only_here) > 0)
		free((genericptr_t)selected);
	    only.x = only.y = 0;
	}
	return n;
}

/* Itemactions function stolen from Unnethack. I'll just print info about the item though. --Amy */
int
itemactions(obj)
struct obj *obj;
{

	if (Hallucination) {

	pline("%s - This item radiates in an array of beautiful colors. It's very mesmerizing.",xname(obj) );

	return 0;
	}

	if (UninformationProblem || u.uprops[UNINFORMATION].extrinsic || have_uninformationstone() ) {

	pline("%s - This is the best item in the game if you know how to use it. Good luck making it work!",xname(obj) );

	return 0;

	}

	register int typ = obj->otyp;
	register struct objclass *ocl = &objects[typ];
	register int nn = (ocl->oc_name_known && obj->dknown);
	register const char *dn = OBJ_DESCR(*ocl);

	switch (obj->oclass) {

		case WEAPON_CLASS:
		pline("%s - This is a weapon. Color: %s. Material: %s. Appearance: %s. You can wield it to attack enemies. Some weapons are also suitable for throwing.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn );
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { 

			switch (obj->otyp) {

			case ORCISH_DAGGER: 
				pline("A crappy dagger that doesn't do much damage. It can be thrown."); break;
			case DAGGER: 
				pline("A basic dagger that doesn't do much damage. It can be thrown."); break;
			case ATHAME: 
				pline("A high-quality dagger that can create hard engravings. It can be thrown."); break;
			case SILVER_DAGGER: 
				pline("A dagger that does extra damage to undead. It can be thrown."); break;
			case ELVEN_DAGGER: 
				pline("Elven daggers do slightly more damage than standard daggers. It can be thrown."); break;
			case DARK_ELVEN_DAGGER: 
				pline("A good dagger that does respectable damage. It can be thrown."); break;
			case WOODEN_STAKE: 
				pline("A quite powerful dagger. It can be thrown."); break;
			case GREAT_DAGGER: 
				pline("The strongest dagger in the game. It can be thrown."); break;
			case WORM_TOOTH: 
				pline("A crappy knife that can be turned into a very powerful knife if enchanted. It can be thrown."); break;
			case KNIFE: 
				pline("A totally piece of crap weapon. It can be thrown."); break;
			case STILETTO: 
				pline("This knife is more likely to hit than a regular knife, but it's still awfully weak. It can be thrown."); break;
			case SCALPEL: 
				pline("Don't bother with this knife-class weapon. The surgery technique works better if you have it in your inventory, though. It can be thrown."); break;
			case CRYSKNIFE: 
				pline("A magically enchanted knife that does superb damage. It can be thrown."); break;
			case TOOTH_OF_AN_ALGOLIAN_SUNTIGER: 
				pline("This razor-sharp knife cuts meat like butter. It can be thrown."); break;
			case AXE: 
				pline("A standard axe that does moderate damage. It can be used to chop down trees."); break;
			case BATTLE_AXE: 
				pline("A heavy two-handed axe that does moderate damage. It can be used to chop down trees."); break;
			case DWARVISH_MATTOCK: 
				pline("A two-handed pick-axe that can do a lot of damage. It can be used for digging."); break;
			case ORCISH_SHORT_SWORD: 
				pline("The weakest short sword in the game. It is inferior to other short swords in every way."); break;
			case SHORT_SWORD: 
				pline("A basic short sword that doesn't deal a lot of damage."); break;
			case SILVER_SHORT_SWORD: 
				pline("A basic short sword that doesn't deal a lot of damage. It is effective against undead."); break;
			case DWARVISH_SHORT_SWORD: 
				pline("A stronger version of the regular short sword."); break;
			case ELVEN_SHORT_SWORD: 
				pline("This short sword is definitely better than a regular short sword."); break;
			case DARK_ELVEN_SHORT_SWORD: 
				pline("If your weapon type of choice reads 'short sword', use this. It outdamages all other short swords in the game."); break;
			case BROADSWORD: 
				pline("A standard broadsword. It does more damage than a short sword but less than a long sword."); break;
			case RUNESWORD: 
				pline("This weapon is basically a broadsword, with the exact same stats."); break;
			case ELVEN_BROADSWORD: 
				pline("Far better than a regular broadsword, this weapon has the highest base damage among all broadswords in the game."); break;
			case LONG_SWORD: 
				pline("A basic long sword that does respectable damage."); break;
			case SILVER_LONG_SWORD: 
				pline("A long sword that does respectable damage, with a bonus against undead."); break;
			case KATANA: 
				pline("This Japanese long sword can deal more damage than a regular long sword."); break;
			case ELECTRIC_SWORD: 
				pline("The most powerful of the long swords."); break;
			case TWO_HANDED_SWORD: 
				pline("It's heavy and requires both hands, but does quite a lot of damage."); break;
			case TSURUGI: 
				pline("A long samurai sword that can only be wielded with both hands. It does lots of damage."); break;
			case SCIMITAR: 
				pline("A light but useful blade, the scimitar can outdamage a standard short sword."); break;
			case BENT_SABLE: 
				pline("This sharpened scimitar is actually very useful for quickly cutting up your enemies."); break;
			case RAPIER: 
				pline("A basic saber that's not stronger than a short sword."); break;
			case SILVER_SABER: 
				pline("This saber does moderate damage, but unlike most other weapons it's super-effective against undead."); break;
			case GOLDEN_SABER: 
				pline("A rare saber made of pure gold. It can do good damage."); break;
			case CLUB: 
				pline("Don't bother with this weapon. The club just doesn't ever deal any meaningful damage."); break;
			case AKLYS: 
				pline("Stronger than a regular club, but still crappy."); break;
			case BASEBALL_BAT: 
				pline("This wooden club does respectable damage for its type."); break;
			case METAL_CLUB: 
				pline("A club made of hard metal. It does solid damage."); break;
			case FLY_SWATTER: 
				pline("This paddle has good to-hit and small damage, but low large damage."); break;
			case INSECT_SQUASHER: 
				pline("A paddle that does superb damage against small foes but next to no damage against large foes."); break;
			case SILVER_MACE: 
				pline("The main use of this mace is to bash undead, which take extra damage from it."); break;
			case MACE: 
				pline("A mace. It's quite a weak weapon, actually."); break;
			case FLANGED_MACE: 
				pline("This mace does moderate damage but it's nothing to get excited about."); break;
			case MORNING_STAR: 
				pline("The morning star can be used to whack enemies. Its damage output is mediocre."); break;
			case JAGGED_STAR: 
				pline("An improved morning star that actually packs a punch."); break;
			case DEVIL_STAR: 
				pline("The strongest version of the morning star. A very strong one-handed melee weapon."); break;
			case FLAIL: 
				pline("A basic flail. It doesn't do a lot of damage."); break;
			case KNOUT: 
				pline("A better flail that does mediocre damage."); break;
			case OBSID: 
				pline("A strong flail that does good damage and has good to-hit."); break;
			case WAR_HAMMER: 
				pline("A relatively weak hammer."); break;
			case HEAVY_HAMMER: 
				pline("This hammer is a definite improvement of the standard war hammer that does good damage."); break;
			case MALLET: 
				pline("A huge hammer made of massive wood that is very useful for bashing down enemies."); break;
			case WEDGED_LITTLE_GIRL_SANDAL: 
				pline("It's a wedge-heeled sandal. Whacking it over the head of an enemy might deal a bit of damage. It uses the hammer skill."); break;
			case SOFT_GIRL_SNEAKER: 
				pline("Made of soft leather, this piece of footwear is not a powerful melee weapon. Good to-hit though. It uses the hammer skill."); break;
			case STURDY_PLATEAU_BOOT_FOR_GIRLS: 
				pline("A heavy plateau boot that can be swung at monsters to whack them for mediocre damage. It uses the hammer skill."); break;
			case HUGGING_BOOT: 
				pline("This thick winter boot is made of unyielding material, making it a useful weapon for bonking enemies' heads. It uses the hammer skill."); break;
			case BLOCK_HEELED_COMBAT_BOOT: 
				pline("A very fleecy lady's boot with a massive block heel. Seems like you can bash enemies' skulls with it. It uses the hammer skill."); break;
			case WOODEN_GETA: 
				pline("This piece of Japanese footwear is made of extremely hard wood. Striking the head of an enemy with it might leave them with a big dent. It uses the hammer skill."); break;
			case LACQUERED_DANCING_SHOE: 
				pline("This ladies' shoe looks expensive. Wielding it to bash enemies might have some uses. It uses the hammer skill."); break;
			case HIGH_HEELED_SANDAL: 
				pline("A sexy sandal; its heel looks sweet but can actually be used to smash things. It uses the hammer skill."); break;
			case SEXY_LEATHER_PUMP: 
				pline("This beautiful lilac women's shoe looks very tender. However, the funneled heel can actually cause a lot of damage if it is struck on somebody's head. It uses the hammer skill."); break;
			case SPIKED_BATTLE_BOOT: 
				pline("A heavy boot with spikes made of steel. Excellent for bashing monsters. It uses the hammer skill."); break;
			case QUARTERSTAFF: 
				pline("The basic quarterstaff is a two-handed weapon that does pitiful damage compared to other two-handers."); break;
			case BATTLE_STAFF: 
				pline("A metal quarterstaff that does relatively good damage but requires both hands."); break;
			case PARTISAN: 
				pline("A balanced two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case GLAIVE: 
				pline("A powerful two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case SPETUM: 
				pline("A finicky two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case RANSEUR: 
				pline("An unreliable two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case BARDICHE: 
				pline("A heavy two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case VOULGE: 
				pline("A dicey two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case HALBERD: 
				pline("A massive two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case FAUCHARD: 
				pline("A mediocre two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case GUISARME: 
				pline("A challenging two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case BILL_GUISARME: 
				pline("A reinforced two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case LUCERN_HAMMER: 
				pline("A ferocious two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case BEC_DE_CORBIN: 
				pline("A strong two-handed polearm that can be applied to hit monsters standing two squares away. Using it at point blank range is only useful if you're riding."); break;
			case ORCISH_SPEAR: 
				pline("The weakest of the spears. It can be thrown."); break;
			case SPEAR: 
				pline("A standard spear. It can be thrown."); break;
			case SILVER_SPEAR: 
				pline("A spear that does extra damage to undead. It can be thrown."); break;
			case ELVEN_SPEAR: 
				pline("A good spear. It can be thrown."); break;
			case DWARVISH_SPEAR: 
				pline("The strongest spear in the game. It can be thrown."); break;
			case JAVELIN: 
				pline("A basic javelin that doesn't do much damage. It can be thrown."); break;
			case SPIRIT_THROWER: 
				pline("A javelin that does good damage. It can be thrown."); break;
			case TORPEDO: 
				pline("A very strong javelin that does lots of damage. It can be thrown."); break;
			case TRIDENT: 
				pline("The trident does sucky damage but has bonuses versus eels."); break;
			case STYGIAN_PIKE: 
				pline("A trident from the depths of Hell. Good damage and bonus versus eels."); break;
			case LANCE: 
				pline("The only weapon that uses the lance skill, this thing can be applied to hit monsters that don't stand right next to you. While riding, you can joust monsters with it by performing standard melee attacks but sometimes the lance breaks if you do so."); break;
			case ORCISH_BOW: 
				pline("A two-handed bow that is meant to be used in conjunction with quivered arrows to fire at enemies."); break;
			case BOW: 
				pline("A two-handed bow that is meant to be used in conjunction with quivered arrows to fire at enemies."); break;
			case ELVEN_BOW: 
				pline("A two-handed bow that is meant to be used in conjunction with quivered arrows to fire at enemies."); break;
			case DARK_ELVEN_BOW: 
				pline("A two-handed bow that is meant to be used in conjunction with quivered arrows to fire at enemies. This bow has a small to-hit bonus."); break;
			case YUMI: 
				pline("A two-handed bow that is meant to be used in conjunction with quivered arrows to fire at enemies."); break;
			case ORCISH_ARROW: 
				pline("The weakest type of arrow. These are meant to be put in a quiver and shot with a bow."); break;
			case ARROW: 
				pline("A standard arrow. These are meant to be put in a quiver and shot with a bow."); break;
			case SILVER_ARROW: 
				pline("An arrow that does more damage to undead. These are meant to be put in a quiver and shot with a bow."); break;
			case ELVEN_ARROW: 
				pline("A good quality arrow that does high amounts of damage. These are meant to be put in a quiver and shot with a bow."); break;
			case DARK_ELVEN_ARROW: 
				pline("There are no arrows in the game that deal more damage than this one. These are meant to be put in a quiver and shot with a bow."); break;
			case YA: 
				pline("A high-quality arrow that has a moderate to-hit bonus. These are meant to be put in a quiver and shot with a bow."); break;
			case SLING: 
				pline("The sling is what you want to use if you want your thrown rocks to do more than a single point of damage. You can quiver most types of rocks and gems to shoot them with a sling."); break;
			case PISTOL: 
				pline("This firearm is capable of shooting bullets to deal damage to enemies."); break;
			case SUBMACHINE_GUN: 
				pline("An automatic firearm that can fire three bullets in a single round of combat."); break;
			case HEAVY_MACHINE_GUN: 
				pline("The heavy machine gun requires two hands to use, but it can rip monsters a new one by firing 8 bullets per turn."); break;
			case RIFLE: 
				pline("A two-handed gun with a low rate of fire that shoots single bullets at enemies."); break;
			case ASSAULT_RIFLE: 
				pline("Your standard-issue heavy firearm that fires 5 bullets in one turn."); break;
			case SNIPER_RIFLE: 
				pline("Very slow, two-handed, but highly accurate. It fires single bullets."); break;
			case SHOTGUN: 
				pline("A short-range firearm that fires highly damaging (and accurate) shotgun shells."); break;
			case AUTO_SHOTGUN: 
				pline("This two-handed shotgun is capable of firing several shotgun shells in one round of combat."); break;
			case ROCKET_LAUNCHER: 
				pline("The 'big daddy' of firearms, this baby shoots explosive rockets for massive damage. Yeah, baby. It takes awfully long to reload though."); break;
			case GRENADE_LAUNCHER: 
				pline("If you want your grenades to pack a bigger punch, fire them with this weapon. The grenade launcher has a low rate of fire though."); break;
			case BULLET: 
				pline("A metal bullet that can be fired with pistols, submachine guns, rifles of all kinds, and heavy machine guns."); break;
			case SILVER_BULLET: 
				pline("A silver bullet that can be fired with pistols, submachine guns, rifles of all kinds, and heavy machine guns. Undead monsters take extra damage from it."); break;
			case SHOTGUN_SHELL: 
				pline("This shell does a lot of damage if fired with a shotgun."); break;
			case ROCKET: 
				pline("A highly explosive rocket. It requires a rocket launcher to be used effectively, but the explosion can hit several enemies at once."); break;
			case FRAG_GRENADE: 
				pline("You can just arm this bomb and throw it at a monster, but for better results, fire it with a grenade launcher."); break;
			case GAS_GRENADE: 
				pline("This bomb will explode in a cloud of noxious gas if you arm it. You can also fire it with a grenade launcher."); break;
			case STICK_OF_DYNAMITE: 
				pline("A stick with a fuse that can be armed. Once the fuse is burned out, it detonates to do explosive damage."); break;
			case CROSSBOW: 
				pline("The crossbow is a two-handed ranged weapon that fires bolts, doing respectable damage. Put a stack of bolts in your quiver to fire."); break;
			case DROVEN_CROSSBOW: 
				pline("A more accurate, one-handed version of the crossbow. Use it to fire bolts at your enemies."); break;
			case DROVEN_BOW: 
				pline("A more accurate, one-handed version of the bow. Use it to fire arrows at your enemies."); break;
			case CROSSBOW_BOLT: 
				pline("This is the ammunition used by crossbows. Put it in your quiver while having a wielded crossbow and fire away. They do solid damage, too."); break;
			case DROVEN_BOLT: 
				pline("These glass bolts can be fired with a crossbow, doing more damage than regular bolts, but unfortunately they are very likely to break on impact."); break;
			case DROVEN_ARROW: 
				pline("These glass arrows can be fired with a bow, doing more damage than regular arrows, but unfortunately they are very likely to break on impact."); break;
			case DART: 
				pline("Darts are throwing weapons that are often found in stacks. They deal moderate amounts of damage. Don't bother whacking enemies with them though; they're meant to be thrown."); break;
			case SHURIKEN: 
				pline("These razor-sharp throwing stars are the only weapon to use the shuriken skill. Throw them at enemies to slice them into tiny bits."); break;
			case BOOMERANG: 
				pline("Good luck making this crappy weapon work! The boomerang can theoretically be thrown to hit several enemies in a single turn, but its weird flight pattern means it has situational uses at best and no uses at worst. Using the boomerang in melee will probably cause it to break."); break;
			case BULLWHIP: 
				pline("*cue Vampire Killer theme* For some reason, Simon Belmont likes to use this weapon. It's got a totally pitiful damage output, and thick-skinned enemies are even outright immune to it. However, you can apply a bullwhip to perform feats like disarming an enemy."); break;
			case STEEL_WHIP: 
				pline("A metal version of the bullwhip. While far stronger than a regular bullwhip, this weapon is still a whip and you know that whips suck. Steer clear."); break;
			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;
			case RUBBER_HOSE: 
				pline("The law inforcement officers like to use this whip-type weapon, but you're probably better off using a real weapon if you don't want to die horribly."); break;

			}

		}
		break;

		case ARMOR_CLASS:
		pline("%s - This is a piece of armor. Color: %s. Material: %s. Appearance: %s. It can be worn for protection (armor class, magic cancellation etc.).",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case HAWAIIAN_SHIRT: 
				pline("A shirt that can be worn under a suit of armor. Shopkeepers who see you wearing this thing will overcharge you. It can be read."); break;
			case T_SHIRT: 
				pline("A shirt that can be worn under a suit of armor. It can be read."); break;
			case STRIPED_SHIRT: 
				pline("A shirt that can be worn under a suit of armor. Shopkeepers who see you wearing this will not allow you to enter their shop. It can be read."); break;
			case RUFFLED_SHIRT: 
				pline("A shirt that can be worn under a suit of armor. If you wear a cursed one, you have a small chance of reviving on death. It can be read."); break;
			case VICTORIAN_UNDERWEAR: 
				pline("This wonderful piece of clothing can be worn under a suit of armor to grant maximum magic cancellation. If you wear a cursed one, you have a small chance of reviving on death. It can be read."); break;
			case PLATE_MAIL: 
				pline("A very heavy suit of armor that offers good protection."); break;
			case PLASTEEL_ARMOR: 
				pline("A low-weight suit of armor with a good armor value."); break;
			case CRYSTAL_PLATE_MAIL: 
				pline("A very heavy suit of armor that offers good protection."); break;
			case BRONZE_PLATE_MAIL: 
				pline("This suit of armor is inferior to regular plate mail."); break;
			case SPLINT_MAIL: 
				pline("A robust suit of armor that offers good protection."); break;
			case BANDED_MAIL: 
				pline("A robust suit of armor that offers good protection."); break;
			case DWARVISH_MITHRIL_COAT: 
				pline("A low-weight suit of armor that offers moderate protection and maximum magic cancellation."); break;
			case DARK_ELVEN_MITHRIL_COAT: 
				pline("This is the best suit of armor that is not a dragon scale mail."); break;
			case ELVEN_MITHRIL_COAT: 
				pline("A low-weight suit of armor that offers good protection and maximum magic cancellation."); break;
			case GNOMISH_SUIT: 
				pline("This suit of armor offers very little protection."); break;
			case CHAIN_MAIL: 
				pline("A moderately good suit of armor."); break;
			case ORCISH_CHAIN_MAIL: 
				pline("A crappier version of regular chain mail that offers mediocre protection."); break;
			case SCALE_MAIL: 
				pline("A medium-weight metallic suit of armor that offers mediocre protection."); break;
			case STUDDED_LEATHER_ARMOR: 
				pline("This is a suit of armor made of leather that offers some protection."); break;
			case RING_MAIL: 
				pline("A metallic suit of armor that offers little protection."); break;
			case ORCISH_RING_MAIL: 
				pline("This suit of metal armor offers very little protection."); break;
			case LEATHER_ARMOR: 
				pline("A basic suit of armor that offers little protection."); break;
			case LEATHER_JACKET: 
				pline("This thing is only useful if you don't have a better suit of armor."); break;
			case ROBE: 
				pline("Robes can be worn instead of armor. This is mainly useful for monks and jedi who are penalized for wearing 'real' armor."); break;
			case ROBE_OF_PROTECTION: 
				pline("If you don't want to wear a real armor, you can use this for some armor class."); break;
			case ROBE_OF_POWER: 
				pline("Wearing this robe improves your spellcasting ability but prevents you from wearing an actual suit of armor."); break;
			case ROBE_OF_WEAKNESS: 
				pline("If you wear this robe, you will be permanently stunned. They are usually generated cursed."); break;
			case GRAY_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as magic resistance."); break;
			case SILVER_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as reflection."); break;
			case SHIMMERING_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as displacement."); break;
			case DEEP_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as level-drain resistance."); break;
			case RED_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as fire resistance."); break;
			case WHITE_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as cold resistance."); break;
			case ORANGE_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as sleep resistance."); break;
			case BLACK_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as disintegration resistance."); break;
			case BLUE_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as shock resistance."); break;
			case GREEN_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as poison resistance."); break;
			case GOLDEN_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as sickness resistance."); break;
			case STONE_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as petrification resistance."); break;
			case CYAN_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as fear resistance."); break;
			case YELLOW_DRAGON_SCALE_MAIL: 
				pline("This armor offers great protection as well as acid resistance."); break;
			case GRAY_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as magic resistance."); break;
			case SILVER_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as reflection."); break;
			case SHIMMERING_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as displacement."); break;
			case DEEP_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as level-drain resistance."); break;
			case RED_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as fire resistance."); break;
			case WHITE_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as cold resistance."); break;
			case ORANGE_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as sleep resistance."); break;
			case BLACK_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as disintegration resistance."); break;
			case BLUE_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as shock resistance."); break;
			case GREEN_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as poison resistance."); break;
			case GOLDEN_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as sickness resistance."); break;
			case STONE_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as petrification resistance."); break;
			case CYAN_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as fear resistance."); break;
			case YELLOW_DRAGON_SCALES: 
				pline("This armor offers moderate protection as well as acid resistance."); break;
			case MUMMY_WRAPPING: 
				pline("A cloak that can be worn to suppress invisibility. Other than that, it's inferior to most other cloaks."); break;
			case ORCISH_CLOAK: 
				pline("This cloak provides medium magic cancellation and no other protection."); break;
			case DWARVISH_CLOAK: 
				pline("This cloak provides medium magic cancellation and no other protection."); break;
			case OILSKIN_CLOAK: 
				pline("A very useful cloak that provides maximum magic cancellation and also protects from grabbing attacks."); break;
			case ELVEN_CLOAK: 
				pline("A powerful cloak that provides maximum magic cancellation and stealth."); break;
			case LAB_COAT: 
				pline("This cloak is highly useful as it provides all of the following: maximum magic cancellation, poison resistance and acid resistance."); break;
			case LEATHER_CLOAK: 
				pline("A basic cloak that has low magic cancellation."); break;
			case PLASTEEL_CLOAK: 
				pline("A lightweight cloak with medium magic cancellation and robust armor class."); break;
			case CLOAK_OF_PROTECTION: 
				pline("This cloak provides good armor class and maximum magic cancellation."); break;
			case CLOAK_OF_FUMBLING: 
				pline("A cloak that provides maximum magic cancellation, but also causes fumbling when worn."); break;
			case POISONOUS_CLOAK: 
				pline("Wearing this cloak without poison resistance can kill you. Other than that, it provides maximum magic cancellation."); break;
			case CLOAK_OF_INVISIBILITY: 
				pline("This cloak renders the wearer invisible and also grants medium magic cancellation."); break;
			case CLOAK_OF_MAGIC_RESISTANCE: 
				pline("A superb cloak that provides magic resistance in addition to maximum magic cancellation."); break;
			case CLOAK_OF_REFLECTION: 
				pline("A superb cloak that provides reflection in addition to maximum magic cancellation."); break;
			case MANACLOAK: 
				pline("A superb cloak that provides energy regeneration in addition to maximum magic cancellation."); break;
			case CLOAK_OF_CONFUSION: 
				pline("Wearing this cloak will confuse you, but it also has good armor class and maximum magic cancellation."); break;
			case CLOAK_OF_WARMTH: 
				pline("Wearing this cloak grants cold resistance and maximum magic cancellation."); break;
			case CLOAK_OF_GROUNDING: 
				pline("Wearing this cloak grants shock resistance and medium magic cancellation."); break;

			case CLOAK_OF_UNSPELLING: 
				pline("This cloak causes spell loss. It grants good armor class and maximum magic cancellation."); break;
			case ANTI_CASTER_CLOAK: 
				pline("This cloak causes casting problems. It grants very good armor class and maximum magic cancellation."); break;
			case HEAVY_STATUS_CLOAK: 
				pline("This cloak causes heavy status effects. It grants extremely good armor class and maximum magic cancellation."); break;
			case CLOAK_OF_LUCK_NEGATION: 
				pline("This cloak causes bad luck. It grants extremely good armor class and maximum magic cancellation."); break;
			case YELLOW_SPELL_CLOAK: 
				pline("This cloak causes yellow spells. It grants great armor class and maximum magic cancellation."); break;
			case VULNERABILITY_CLOAK: 
				pline("This cloak causes vulnerability. It grants good armor class and maximum magic cancellation."); break;
			case CLOAK_OF_INVENTORYLESSNESS: 
				pline("This cloak causes inventory loss. It grants incredibly great armor class and maximum magic cancellation."); break;

			case CLOAK_OF_QUENCHING: 
				pline("Wearing this cloak grants fire resistance and maximum magic cancellation."); break;
			case CLOAK_OF_DISPLACEMENT: 
				pline("Wearing this cloak grants displacement and medium magic cancellation."); break;
			case ELVEN_LEATHER_HELM:
				pline("A light helmet that grants good armor class."); break;
			case FIRE_HELMET:
				pline("This helmet conveys fire resistance when worn."); break;
			case GNOMISH_HELM:
				pline("This headgear is a total waste of your time."); break;
			case ORCISH_HELM:
				pline("A basic helmet that gives a little bit of protection."); break;
			case DWARVISH_IRON_HELM:
				pline("A good helmet that offers some protection."); break;
			case FEDORA:
				pline("While it doesn't grant armor class, this headgear can increase your luck if worn."); break;
			case CORNUTHAUM:
				pline("Only wizards can use this headgear effectively."); break;
			case DUNCE_CAP:
				pline("This cap sets your intelligence to a low value but prevents it from changing, so you'll be protected from mind-eating attacks."); break;
			case DENTED_POT:
				pline("A relatively weak headgear."); break;
			case PLASTEEL_HELM:
				pline("Good protection and maximum magic cancellation, but this helmet prevents you from performing certain actions."); break;
			case HELMET:
				pline("A standard helmet that can be worn for protection."); break;
			case HELM_OF_OBSCURED_DISPLAY:
				pline("This helmet causes display loss. It has good AC and medium magic cancellation."); break;
			case HELM_OF_LOSE_IDENTIFICATION:
				pline("This helmet causes unidentification. It has moderately good AC and maximum magic cancellation."); break;
			case HELM_OF_THIRST:
				pline("This helmet causes thirst. It has mediocre AC and maximum magic cancellation."); break;
			case BLACKY_HELMET:
				pline("This helmet summons Blacky. It has great AC and maximum magic cancellation."); break;
			case ANTI_DRINKER_HELMET:
				pline("This helmet affects potions. It has good AC and low magic cancellation."); break;
			case WHISPERING_HELMET:
				pline("This helmet displays random rumors. It has low AC and low magic cancellation."); break;
			case CYPHER_HELM:
				pline("This helmet initiates a cipher. It has very good AC and maximum magic cancellation."); break;
			case HELM_OF_BAD_ALIGNMENT:
				pline("This helmet causes alignment failures. It has good AC and maximum magic cancellation."); break;
			case SOUNDPROOF_HELMET:
				pline("This helmet causes deafness. It has mediocre AC and medium magic cancellation."); break;
			case OUT_OF_MEMORY_HELMET:
				pline("This helmet causes memory loss. It has great AC and maximum magic cancellation."); break;
			case HELM_OF_BRILLIANCE:
				pline("This helmet can be worn to increase your intelligence."); break;
			case HELM_OF_OPPOSITE_ALIGNMENT:
				pline("If you put on this helmet, your alignment will be changed and you lose all divine protection that you might have."); break;
			case HELM_OF_STEEL:
				pline("A robust helmet that offers good armor class."); break;
			case HELM_OF_DRAIN_RESISTANCE:
				pline("You can get resistance to level drain by putting on this helm."); break;
			case HELM_OF_FEAR:
				pline("A helmet that grants good AC and magic cancellation, but you also get the 'fear' status effect while wearing it, causing you to miss a lot more often. It is usually generated cursed."); break;
			case HELM_OF_HUNGER:
				pline("A helmet that grants good AC and magic cancellation, but you also get the 'hunger' status effect while wearing it, causing you to burn nutrition at a faster rate. It is usually generated cursed."); break;
			case HELM_OF_STORMS:
				pline("The very powerful Helm of Storms grants its wearer control over the elements, which is to say, resistance to fire, cold and lightning. It also allows you to detect monsters until the helm is removed, but you can't eat, quaff potions, levelport, or control your teleports while wearing it. Monsters also respawn much faster for as long as you wear it. This helm autocurses if you put it on."); break;
			case HELM_OF_DETECT_MONSTERS:
				pline("When worn, this helm grants you the ability to detect monsters until removed. It also prevents you from eating or quaffing potions, and this helm autocurses every time it is put on."); break;
			case HELM_OF_DISCOVERY:
				pline("This helmet grants automatic searching if you wear it."); break;
			case HELM_OF_TELEPATHY:
				pline("Wearing this helmet conveys 'weak' telepathy that displays monsters close by, and 'good' telepathy that displays all monsters on the level if you are blind."); break;
			case PLASTEEL_GLOVES:
				pline("This pair of gloves offers good protection."); break;
			case LEATHER_GLOVES:
				pline("A standard pair of gloves that offers little protection."); break;
			case GAUNTLETS_OF_FUMBLING:
				pline("You will fumble if you put on this pair of gloves. They are usually generated cursed."); break;
			case GAUNTLETS_OF_SLOWING:
				pline("A pair of gloves that slows your movement speed when worn. They are usually generated cursed."); break;
			case OILSKIN_GLOVES:
				pline("This pair of gloves will cause you to drop your weapon, and you'll be unable to re-equip it. They provide some AC and maximum magic cancellation, but these gloves autocurse if you put them on."); break;

			case MENU_NOSE_GLOVES:
				pline("This pair of gloves causes menu bugs. They provide moderately good AC and medium magic cancellation."); break;
			case UNWIELDY_GLOVES:
				pline("This pair of gloves causes the free hand to be full. They provide good AC and maximum magic cancellation."); break;
			case CONFUSING_GLOVES:
				pline("This pair of gloves causes confusing problems. They provide extremely good AC."); break;
			case UNDROPPABLE_GLOVES:
				pline("This pair of gloves causes drop bugs. They provide moderately good AC and maximum magic cancellation."); break;
			case GLOVES_OF_MISSING_INFORMATION:
				pline("This pair of gloves causes a lack of feedback. They provide good AC and maximum magic cancellation."); break;
			case GLOVES_OF_TRAP_CREATION:
				pline("This pair of gloves causes traps to be generated. They provide good AC and medium magic cancellation."); break;
			case SADO_MASO_GLOVES:
				pline("This pair of gloves causes fifty shades of grey. They provide low AC and low magic cancellation."); break;

			case GAUNTLETS_OF_POWER:
				pline("A powerful pair of gauntlets that increases the wearer's strength."); break;
			case GAUNTLETS_OF_REFLECTION:
				pline("Wear this pair of gloves, and you'll be able to reflect beams and certain other attacks!"); break;
			case GAUNTLETS_OF_TYPING:
				pline("These gauntlets are nothing special, but they offer some points of armor class."); break;
			case GAUNTLETS_OF_STEEL:
				pline("A pair of gloves made of metal. They offer good protection."); break;
			case GAUNTLETS_OF_SWIMMING:
				pline("Magic gloves that allow the wearer to swim."); break;
			case GAUNTLETS_OF_DEXTERITY:
				pline("Depending on their enchantment, these gloves can increase or decrease your dexterity if you wear them."); break;
			case SMALL_SHIELD:
				pline("A wooden shield that offers a little protection."); break;
			case ELVEN_SHIELD:
				pline("This shield offers some protection from enemy attacks."); break;
			case URUK_HAI_SHIELD:
				pline("A good shield that offers solid armor class."); break;
			case ORCISH_SHIELD:
				pline("A good shield that offers solid armor class."); break;
			case STEEL_SHIELD:
				pline("This metal shield can deflect lots of attacks."); break;
			case LARGE_SHIELD:
				pline("A shield made of iron with a very good armor value."); break;
			case DWARVISH_ROUNDSHIELD:
				pline("This shield offers very good protection."); break;
			case FLAME_SHIELD:
				pline("A heat-resistant shield that offers fire resistance to the wearer."); break;
			case ICE_SHIELD:
				pline("A cold-resistant shield that offers cold resistance to the wearer."); break;
			case LIGHTNING_SHIELD:
				pline("A shock-resistant shield that offers shock resistance to the wearer."); break;
			case VENOM_SHIELD:
				pline("A poison-resistant shield that offers poison resistance to the wearer."); break;
			case SHIELD_OF_LIGHT:
				pline("This shield conveys infravision when worn."); break;
			case SHIELD_OF_MOBILITY:
				pline("A useful shield that prevents the wearer from being paralyzed."); break;
			case SHIELD_OF_REFLECTION:
				pline("One of the most powerful shields in the game. This reflexive shield protects the wearer from rays, gaze attacks and similar crap while also providing excellent AC."); break;
			case PLASTEEL_BOOTS:
				pline("A pair of boots that offers maximum magic cancellation."); break;
			case LOW_BOOTS:
				pline("This basic pair of boots offers minimal protection from enemy attacks."); break;
			case GNOMISH_BOOTS:
				pline("Don't bother wearing these things. Find a better pair of footwear instead."); break;
			case HIGH_BOOTS:
				pline("These boots offer moderate protection when worn."); break;
			case WEDGE_SANDALS:
				pline("A lovely pair of high-heeled women's sandals that provides no protection but looks pretty."); break;
			case DANCING_SHOES:
				pline("This pair of soft footwear has profiled soles and looks incredibly lovely. Your feet will probably like being enclosed by them."); break;
			case SWEET_MOCASSINS:
				pline("A pair of sexy beauties made of leather. They look comfortable."); break;
			case SOFT_SNEAKERS:
				pline("A valuable pair of lightweight leather sneakers that seems very comfortable."); break;
			case FEMININE_PUMPS:
				pline("These high heels look incredibly lovely and tender. You will surely look great if you wear them."); break;
			case LEATHER_PEEP_TOES:
				pline("A pair of asian footwear with plateau soles and stiletto heels. They are made of beautifully soft black leather."); break;

			case AUTODESTRUCT_DE_VICE_BOOTS:
				pline("This footwear causes auto-destruct mechanisms to be initiated. They provide good AC and medium magic cancellation."); break;
			case SPEEDBUG_BOOTS:
				pline("This footwear causes the speed bug. They provide good AC and low magic cancellation."); break;
			case SENTIENT_HIGH_HEELED_SHOES:
				pline("This high-heeled footwear randomly tries to hurt the wearer. They provide very good AC and maximum magic cancellation."); break;
			case BOOTS_OF_FAINTING:
				pline("This footwear causes fainting. They provide very good AC."); break;
			case DIFFICULT_BOOTS:
				pline("This footwear causes increased difficulty. They provide mediocre AC and medium magic cancellation."); break;
			case BOOTS_OF_WEAKNESS:
				pline("This footwear causes weakness. They provide good AC and medium magic cancellation."); break;
			case GRIDBUG_CONDUCT_BOOTS:
				pline("This footwear forces its wearer to adhere to the grid bug conduct. They provide extremely good AC and maximum magic cancellation."); break;
			case STAIRWELL_STOMPING_BOOTS:
				pline("This footwear causes stairwells to be trapped. They provide very good AC and maximum magic cancellation."); break;

			case HIPPIE_HEELS:
				pline("This pair of red leather plateau boots looks extraordinarily sexy. You get the feeling that they would love to be worn by you. Can you resist the temptation? :-)"); break;
			case COMBAT_STILETTOS:
				pline("This is a pair of high-heeled combat boots. Probably meant to be used by a kung-fu ninja woman or something like that."); break;
			case SPEED_BOOTS:
				pline("This piece of footwear makes its wearer speed up."); break;
			case BOOTS_OF_MOLASSES:
				pline("Don't wear these unless you want to move at half speed. They are usually generated cursed."); break;
			case WATER_WALKING_BOOTS:
				pline("If you want to be like Jesus and walk on water, wear this pair of boots. They also allow you to walk on lava, but keep in mind they will be touching it if you do."); break;
			case JUMPING_BOOTS:
				pline("Wearing this pair of boots allows you to jump around."); break;
			case FLYING_BOOTS:
				pline("A funny pair of boots with wings that allows the wearer to fly like an eagle."); break;
			case BOOTS_OF_ACID_RESISTANCE:
				pline("Wearing these boots grats the otherwise hard-to-get acid resistance property, but unfortunately it won't protect your equipment."); break;
			case ELVEN_BOOTS:
				pline("Wearers of this pair of boots can walk very quietly."); break;
			case KICKING_BOOTS:
				pline("If you want to be a kung-fu fighter, wear these boots to power up your kicks."); break;
			case FUMBLE_BOOTS:
				pline("Wear this pair of boots if you want to fumble, which probably won't ever be the case. They are usually generated cursed."); break;
			case FIRE_BOOTS:
				pline("A pair of boots that grants great AC and magic cancellation but also burns you when worn. They are usually generated cursed."); break;
			case ZIPPER_BOOTS:
				pline("By watching these boots closely, you notice their zippers are trying to touch and damage your skin. They're sharp-edged too, so be careful."); break;
			case LEVITATION_BOOTS:
				pline("You will float into the air if you wear this pair of boots. Unlike Castle of the Winds, this is NOT a good thing as these are usually generated cursed and prevent you from picking up items or using a set of downstairs."); break;

			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case RING_CLASS:
		pline("%s - This is a ring. Color: %s. Material: %s. Appearance: %s. You can wear a maximum of two rings; they will often have some sort of magical effect if worn. Every worn ring will cause you to go hungry a little bit faster. Dropping a ring on a sink will cause it to disappear while providing you with a clue to its nature.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case RIN_ADORNMENT: 
				pline("If you wear this ring, you will feel more charismatic."); break;
			case RIN_HUNGER: 
				pline("Put this ring on if you no longer want to be satiated. It is usually generated cursed and increases your food consumption rate."); break;
			case RIN_DISARMING: 
				pline("You will drop your weapon if you wear this ring. It is usually generated cursed."); break;
			case RIN_NUMBNESS: 
				pline("Wearing this ring will numb your limbs, which is a Bad Thing (TM). It is usually generated cursed."); break;
			case RIN_CURSE: 
				pline("While wearing this ring, your items will sometimes get cursed. Putting this ring on causes it to autocurse."); break;
			case RIN_HALLUCINATION: 
				pline("You will hallucinate as long as you wear this ring. Putting it on causes it to autocurse."); break;
			case RIN_INTRINSIC_LOSS: 
				pline("This ring can cause intrinsic loss."); break;
			case RIN_TRAP_REVEALING: 
				pline("A very rare ring that grants its wearer the ability to randomly detect traps on the current dungeon level."); break;
			case RIN_BLOOD_LOSS: 
				pline("This ring causes bleedout."); break;
			case RIN_NASTINESS: 
				pline("This ring has nasty effects."); break;
			case RIN_BAD_EFFECT: 
				pline("This ring has bad effects."); break;
			case RIN_SUPERSCROLLING: 
				pline("This ring causes the superscroller effect."); break;
			case RIN_MOOD: 
				pline("A fairly useless ring that requires you to put it on, then read it to reveal a not-very-enlightening message."); break;
			case RIN_PROTECTION: 
				pline("If it is enchanted, this ring will increase your armor class when worn."); break;
			case RIN_PROTECTION_FROM_SHAPE_CHAN: 
				pline("Most shapeshifters are forced back into their natural form if you wear this ring, and they are prevented from changing form too."); break;
			case RIN_SLEEPING: 
				pline("Wearing this ring causes you to fall asleep. It is usually generated cursed."); break;
			case RIN_STEALTH: 
				pline("You will make less noise if you wear this ring."); break;
			case RIN_SUSTAIN_ABILITY: 
				pline("This ring locks your stats if worn, i.e. they can be neither increased nor decreased."); break;
			case RIN_WARNING: 
				pline("This ring allows you to detect monsters and get a difficulty rating indicating their strength."); break;
			case RIN_AGGRAVATE_MONSTER: 
				pline("If you wear this ring, monsters will chase you more aggressively and also wake up more quickly. It is usually generated cursed."); break;
			case RIN_COLD_RESISTANCE: 
				pline("You can resist cold if you wear this ring."); break;
			case RIN_FEAR_RESISTANCE: 
				pline("You can resist fear if you wear this ring."); break;
			case RIN_GAIN_CONSTITUTION: 
				pline("Wearing this ring increases your constitution by its enchantment value."); break;
			case RIN_GAIN_DEXTERITY: 
				pline("Wearing this ring increases your dexterity by its enchantment value."); break;
			case RIN_GAIN_INTELLIGENCE: 
				pline("Wearing this ring increases your intelligence by its enchantment value."); break;
			case RIN_GAIN_STRENGTH: 
				pline("Wearing this ring increases your strength by its enchantment value."); break;
			case RIN_GAIN_WISDOM: 
				pline("Wearing this ring increases your wisdom by its enchantment value."); break;
			case RIN_TIMELY_BACKUP: 
				pline("Wearing this ring causes you to feel absolutely safe, which doesn't actually do anything."); break;
			case RIN_INCREASE_ACCURACY: 
				pline("Wearing this ring increases your to-hit rate by its enchantment value."); break;
			case RIN_INCREASE_DAMAGE: 
				pline("Wearing this ring increases your attack damage by its enchantment value."); break;
			case RIN_SLOW_DIGESTION: 
				pline("If you wear this ring, your natural food consumption rate is disabled. Keep in mind that ring hunger still applies, so wearing two of these actually causes you to consume more food than you would while wearing just one."); break;
			case RIN_INVISIBILITY: 
				pline("This powerful ring can be slipped on a finger to turn the wearer invisible."); break;
			case RIN_POISON_RESISTANCE: 
				pline("A ring that grants poison resistance when worn."); break;
			case RIN_SEE_INVISIBLE: 
				pline("If something is invisible, wear this ring and you can see it."); break;
			case RIN_INFRAVISION: 
				pline("You get the ability to see warm-blooded monsters in the dark while wearing this ring."); break;
			case RIN_SHOCK_RESISTANCE: 
				pline("You can resist shock if you wear this ring."); break;
			case RIN_SICKNESS_RESISTANCE: 
				pline("A very powerful ring that allows you to eat tainted corpses and be hit by sickness attacks without actually getting sick."); break;
			case RIN_FIRE_RESISTANCE: 
				pline("You can resist fire if you wear this ring."); break;
			case RIN_FREE_ACTION: 
				pline("This ring protects you from paralysis and similar effects."); break;
			case RIN_LEVITATION: 
				pline("Wearing this ring allows you to float into the air. This prevents you from performing certain actions, e.g. picking up items or using a set of downstairs."); break;
			case RIN_REGENERATION: 
				pline("Wear this ring to increase your HP regneration rate. It increases your food consumption rate."); break;
			case RIN_SEARCHING: 
				pline("If you want automatic searching so you don't have to continuously press the S key, wear this."); break;
			case RIN_TELEPORTATION: 
				pline("A ring that grants teleportitis when worn. It is usually generated cursed."); break;
			case RIN_CONFLICT: 
				pline("Monsters will attack each other if you wear a ring of conflict. It greatly increases your food consumption rate."); break;
			case RIN_POLYMORPH: 
				pline("A ring that grants polymorphitis when worn. It is usually generated cursed."); break;
			case RIN_POLYMORPH_CONTROL: 
				pline("While wearing this ring, you can control your polymorphs and specify what monster you would like to become."); break;
			case RIN_TELEPORT_CONTROL: 
				pline("While wearing this ring, you can control your teleports and specify where you want to go."); break;


 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case AMULET_CLASS:


		if (obj->otyp == FAKE_AMULET_OF_YENDOR || obj->otyp == AMULET_OF_YENDOR) {
		pline("This is the amulet of Yendor, a very powerful talisman that radiates power."); break;

		} else pline("%s - This is an amulet. Color: %s. Material: %s. Appearance: %s. It can be worn for some magical effect, but you will go hungry a little bit faster if you are wearing an amulet.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);

		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case AMULET_OF_CHANGE: 
				pline("Wearing this amulet causes you to become female if you were male, and in reverse. The amulet will then disintegrate."); break;
			case AMULET_OF_DRAIN_RESISTANCE: 
				pline("This amulet gives level-drain resistance if worn."); break;
			case AMULET_OF_ESP: 
				pline("An amulet of extra-sensory perception, a.k.a. telepathy."); break;
			case AMULET_OF_UNDEAD_WARNING: 
				pline("If you wear this amulet, you can detect the presence of undead."); break;
			case AMULET_OF_OWN_RACE_WARNING: 
				pline("If you wear this amulet, you can detect the presence of monsters that are the same race as you."); break;
			case AMULET_OF_POISON_WARNING: 
				pline("If you wear this amulet, you can detect the presence of poisonous monsters."); break;
			case AMULET_OF_COVETOUS_WARNING: 
				pline("If you wear this amulet, you can detect the presence of covetous monsters."); break;
			case AMULET_OF_FLYING:
				pline("Wearing this amulet allows the wearer to fly."); break;
			case AMULET_OF_LIFE_SAVING:
				pline("You can survive death once if you wear this amulet. It will disintegrate if it saves your life."); break;
			case AMULET_OF_MAGICAL_BREATHING:
				pline("An amulet that allows you to survive without air if you wear it."); break;
			case AMULET_OF_REFLECTION:
				pline("This amulet can reflect beams and other nasty things while worn."); break;
			case AMULET_OF_RESTFUL_SLEEP:
				pline("You will fall asleep if you wear this amulet. It is usually generated cursed."); break;
			case AMULET_OF_BLINDNESS:
				pline("Wearing this amulet prevents you from seeing. It is usually generated cursed."); break;
			case AMULET_OF_STRANGULATION:
				pline("If you wear this amulet, you only have 5 turns to live before it kills you. It is usually generated cursed."); break;
			case AMULET_OF_UNCHANGING:
				pline("This amulet prevents you from changing form. If something tries to force you out of a polymorphed form while wearing this amulet, you might die instantly."); break;
			case AMULET_VERSUS_POISON:
				pline("An amulet that grants poison resistance when worn."); break;
			case AMULET_VERSUS_STONE:
				pline("Wearing this amulet can save you from petrification, but every time it does, it will degrade."); break;
			case AMULET_OF_DEPETRIFY:
				pline("If you wear this amulet, you will be petrification resistant."); break;
			case AMULET_OF_MAGIC_RESISTANCE:
				pline("A very useful amulet that grants magic resistance to the wearer."); break;
			case AMULET_OF_SICKNESS_RESISTANCE:
				pline("You will be immune to sickness as long as you wear this amulet."); break;
			case AMULET_OF_SWIMMING:
				pline("Wear this amulet if you want to be able to swim in water."); break;
			case AMULET_OF_RMB_LOSS:
				pline("This amulet causes your right mouse button to stop working."); break;
			case AMULET_OF_ITEM_TELEPORTATION:
				pline("This amulet causes items to teleport out of your inventory."); break;
			case AMULET_OF_DISINTEGRATION_RESIS:
				pline("This amulet grants disintegration resistance while worn."); break;
			case AMULET_OF_ACID_RESISTANCE:
				pline("Wearing this amulet causes you to be resistant to acid. This resistance doesn't protect your inventory from acid damage though."); break;
			case AMULET_OF_REGENERATION:
				pline("An amulet that increases your hit point regeneration when worn. It increases your food consumption rate."); break;
			case AMULET_OF_CONFLICT:
				pline("As long as you wear this amulet, monsters may sometimes attack each other. It greatly increases your food consumption rate."); break;
			case AMULET_OF_FUMBLING:
				pline("Wearing this amulet causes you to fumble. It is usually generated cursed."); break;
			case AMULET_OF_SECOND_CHANCE:
				pline("A weaker version of the amulet of life saving that allows you to survive a deadly hit without restoring you to full hit points."); break;
			case AMULET_OF_DATA_STORAGE:
				pline("This amulet does nothing when worn."); break;
			case AMULET_OF_WATERWALKING:
				pline("You can walk on water if you wear this amulet."); break;
			case AMULET_OF_HUNGER:
				pline("This amulet increases your food consumption when worn. It is usually generated cursed."); break;
			case AMULET_OF_POWER:
				pline("A magical amulet that grants energy regeneration if you wear it."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case TOOL_CLASS:
		pline("%s - This is a tool. Color: %s. Material: %s. Appearance: %s. Most tools can be applied for an effect; some are also useful when wielded.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case LARGE_BOX: 
				pline("A big container that can hold items."); break;
			case CHEST: 
				pline("A treasure chest that may be filled with loot."); break;
			case ICE_BOX: 
				pline("This container has the unique ability to keep corpses fresh if they're put in."); break;
			case SACK: 
				pline("This is a basic container that can be used to store items."); break;
			case OILSKIN_SACK:
				pline("A useful container that protects its contents from water."); break;
			case BAG_OF_HOLDING: 
				pline("Items that are in this container have an altered weight. Be careful - nesting bags of holding will cause them to explode, and there are certain items that may not be put in either."); break;
			case BAG_OF_DIGESTION: 
				pline("If you want to get rid of unneccessary items, put them into this container and they may disappear."); break;
			case BAG_OF_TRICKS: 
				pline("A bag that cannot be used for storing items. Instead, it creates monsters when applied."); break;
			case SKELETON_KEY: 
				pline("A key that can be used for locking and unlocking doors as well as certain containers."); break;
			case LOCK_PICK: 
				pline("This tool can be used on locks to open them."); break;
			case CREDIT_CARD: 
				pline("Using this card on a lock has a chance to open it."); break;
			case TALLOW_CANDLE: 
				pline("A light source that will burn up after a certain amount of time."); break;
			case WAX_CANDLE: 
				pline("This candle can be lit to provide some light radius for a while."); break;
			case MAGIC_CANDLE: 
				pline("A permanent light source that might be useful in dark areas."); break;
			case OIL_LAMP: 
				pline("This lamp can be lit to provide a big radius of light for a while. Oil runs out after some time but can be refilled."); break;
			case BRASS_LANTERN: 
				pline("A mobile light source that lasts for quite a while."); break;
			case MAGIC_LAMP: 
				pline("This lamp won't ever go out, and according to certain fairy tales, it might be inhabited by a genie."); break;
			case TIN_WHISTLE: 
				pline("Supposed to make your pets follow more closely, but it rarely does anything."); break;
			case MAGIC_WHISTLE: 
				pline("One blow of this powerful whistle will instantly summon all your pets."); break;
			case WOODEN_FLUTE: 
				pline("If you're good enough at it, you may use this instrument to calm snakes."); break;
			case MAGIC_FLUTE: 
				pline("An instrument that generates charming sounds to lull your enemies into sleeping."); break;
			case TOOLED_HORN: 
				pline("A noisy instrument that will wake up monsters. Sometimes it will scare them, too."); break;
			case FROST_HORN: 
				pline("This instrument can shoot bolts of ice."); break;
			case FIRE_HORN: 
				pline("This instrument can shoot bolts of fire."); break;
			case HORN_OF_PLENTY: 
				pline("A magic horn that generates food."); break;
			case WOODEN_HARP: 
				pline("You may be able to charm nymphs by playing this harp."); break;
			case MAGIC_HARP: 
				pline("This powerful instrument can be played to tame adjacent monsters."); break;
			case BELL: 
				pline("A non-tonal instrument that can be used to make some noise."); break;
			case BUGLE:
				pline("This instrument can be played to wake up soldiers."); break;
			case LEATHER_DRUM: 
				pline("Using this instrument causes nearby monsters to wake up."); break;
			case DRUM_OF_EARTHQUAKE: 
				pline("A magic drum that causes the entire dungeon level to shake violently, creating lots of pits."); break;
			case LAND_MINE: 
				pline("A portable trap that can be set to explode if an enemy steps on it."); break;
			case BEARTRAP: 
				pline("A portable trap that can be set to prevent enemies from moving."); break;
			case SPOON: 
				pline("This tool is also a weapon that can be thrown. It uses the dart skill."); break;
			case PICK_AXE: 
				pline("A tool that can also be used as a weapon. It can be applied for digging."); break;
			case FISHING_POLE: 
				pline("This polearm weapon-tool can be applied to catch fish."); break;
			case GRAPPLING_HOOK: 
				pline("A flail-type weapon-tool that can be used to pull objects and monsters toward you."); break;
			case UNICORN_HORN: 
				pline("The unicorn horn can be used as a two-handed melee weapon that uses its own skill, and applying it can cure a variety of bad effects."); break;
			case TORCH: 
				pline("A tool that counts as a club for in-game purposes; unfortunately a lit torch must be wielded in order to work, which makes it a very useless item."); break;
			case GREEN_LIGHTSABER: 
				pline("This lightsaber does solid damage, but it needs to be turned on in order to work. An activated lightsaber is good for engraving."); break;
			case BLUE_LIGHTSABER: 
				pline("This lightsaber does good damage, but it needs to be turned on in order to work. An activated lightsaber is good for engraving."); break;
			case RED_LIGHTSABER: 
				pline("This lightsaber does random damage, but it needs to be turned on in order to work. An activated lightsaber is good for engraving."); break;
			case RED_DOUBLE_LIGHTSABER: 
				pline("A two-handed lightsaber that can be set to double mode in order to do even more damage. It needs to be turned on in order to work, and while activated it is also useful for engraving."); break;
			case LASER_SWATTER: 
				pline("A laser-based fly swatter that uses the paddle skill. If you turn it on, it will do great damage to small monsters."); break;
			case EXPENSIVE_CAMERA: 
				pline("A tool that can be used to photograph monsters."); break;
			case MIRROR: 
				pline("Some monsters may be scared if you apply a mirror at them, and gaze-based attacks may be reflected."); break;
			case CRYSTAL_BALL: 
				pline("Applying a crystal ball can be dangerous, but if it works, you may search for a glyph."); break;
			case LENSES: 
				pline("A weird tool that can be put on to improve the player's ability to search for things."); break;
			case BLINDFOLD: 
				pline("Putting on this tool prevents you from seeing."); break;
			case TOWEL: 
				pline("According to Douglas Adams, you can do a lot of stuff with a towel. Possible uses include: covering your eyes, wiping your hands, throwing it at a monster or wielding it as a melee weapon. See for yourself if you find any of these useful. :-)"); break;
			case SADDLE: 
				pline("Applying this at a tame monster may allow you to ride it. The more tame a monster is, the more likely you are to succeed in saddling it."); break;
			case LEASH: 
				pline("This tool can be applied at a tame monster to force it to follow you. Or that's what it *should* do, if pets weren't so goddamn stupid."); break;
			case STETHOSCOPE: 
				pline("This useful tool can be applied at monsters, objects and other things to find out more about them."); break;
			case TINNING_KIT: 
				pline("If you want to get rid of corpses, apply this tool. It will also generate tins containing some of the monster's remains, neutralizing some bad effects like rotting or poison, but a tin of cockatrice meat will still turn you to stone."); break;
			case MEDICAL_KIT: 
				pline("A bag filled with medical tools. Applying it will cause you to swallow a pill and feel deathly sick, or sometimes you can get other effects as well. Some techniques, e.g. draw blood and surgery, will work better if you have this item."); break;
			case TIN_OPENER: 
				pline("A tool that must be wielded in order to work. It allows you to open tins more quickly. Some players use it to kill Vlad but that's not a good idea in Slash'EM Extended."); break;
			case CAN_OF_GREASE: 
				pline("Despite seeming so mundane, this tool is actually rare and valuable as it allows you to grease your items. However, grease will wear off quickly and needs to be applied again."); break;
			case FIGURINE: 
				pline("Apply this at an empty location to transform it into a living monster. Please don't apply a figurine at a square containing a monster; doing so will just cause the figurine to break and do nothing!"); break;
			case MAGIC_MARKER: 
				pline("The magic marker is actually a sort of pen that can be used for engraving. If you have blank scrolls or spellbooks, you can also attempt to write something on them; for a better chance of success, try to write an item that you know."); break;
			case SWITCHER:
				pline("This metal box has a switch that can be pulled. What may happen if you do so?"); break;
			case HITCHHIKER_S_GUIDE_TO_THE_GALA: 
				pline("A very complicated-looking device. Better not mess around with it..."); break;
			case DIODE: 
				pline("It's a two-wired piece of metal. Nobody knows if it's good for anything."); break;
			case TRANSISTOR: 
				pline("It's a three-wired piece of metal. Nobody knows if it's good for anything."); break;
			case IC: 
				pline("It's a many-wired piece of metal. Nobody knows if it's good for anything."); break;
			case PACK_OF_FLOPPIES: 
				pline("Only characters who have a lot of knowledge about computers may be able to use this item."); break;
			case GOD_O_METER: 
				pline("Using this device can give you a clue about your current standing with your god."); break;
			case RELAY: 
				pline("It's a four-wired piece of metal. Nobody knows if it's good for anything."); break;
			case BOTTLE: 
				pline("An empty bottle that can be filled if you have a chemistry set."); break;
			case CHEMISTRY_SET: 
				pline("You can try to create your own potions with this. It requires an empty bottle to work; having the chemistry spell helps, too."); break;
			case BANDAGE: 
				pline("A pseudo tool that actually can't exist outside of medical kits. It is used for the surgery technique."); break;
			case PHIAL: 
				pline("A pseudo tool that actually can't exist outside of medical kits. It is used for the draw blood technique."); break;
			case CANDELABRUM_OF_INVOCATION: 
				pline("Also called a menorah. This candelabrum can hold several candles."); break;
			case BELL_OF_OPENING: 
				pline("It's a silver bell that you can ring."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case FOOD_CLASS:
		pline("%s - This is a comestible. Color: %s. Material: %s. Appearance: %s. It can be eaten.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case TRIPE_RATION: 
				pline("A ration of dog food that's meant to be eaten by carnivorous pets."); break;
			case CORPSE: 
				pline("Corpses can be eaten, but it's not always a good idea to do so. Depending on the type of monster and the age of a corpse, different effects can occur."); break;
			case EGG: 
				pline("Eggs can be eaten, but some of them can also hatch after a while. Eating a stale egg causes vomiting."); break;
			case MEATBALL: 
				pline("These provide very little nutrition but can be used for training dogs."); break;
			case MEAT_STICK: 
				pline("A snack made of meat. Carnivorous pets like to eat these."); break;
			case HUGE_CHUNK_OF_MEAT: 
				pline("This is one of the most satiating comestibles in the game that provides lots of nutrition."); break;
			case MEAT_RING: 
				pline("A rarely seen type of comestible that doesn't actually do anything special."); break;
			case EYEBALL: 
				pline("You don't really want to eat this..."); break;
			case SEVERED_HAND: 
				pline("You don't really want to eat this..."); break;
			case KELP_FROND: 
				pline("A vegetarian food item that can be thrown to petty monsters in order to tame them."); break;
			case EUCALYPTUS_LEAF: 
				pline("Eating this item cures sickness and vomiting."); break;
			case CLOVE_OF_GARLIC: 
				pline("This is a type of food that can be eaten by vegetarians, and it can also be used to keep vampires away from you."); break;
			case SPRIG_OF_WOLFSBANE: 
				pline("If you're having trouble with lycanthropes infecting you, eating this vegetarian piece of food will cure you."); break;
			case APPLE: 
				pline("A vegetarian type of food that cures numbness."); break;
			case CARROT: 
				pline("A vegetarian type of food that cures blindness."); break;
			case PEAR: 
				pline("A vegetarian type of food that cures stunning."); break;
			case ASIAN_PEAR: 
				pline("A vegetarian type of food that cures stunning and confusion."); break;
			case LEMON: 
				pline("A vegetarian type of food that cures fear."); break;
			case BANANA: 
				pline("A vegetarian type of food that cures hallucination."); break;
			case ORANGE: 
				pline("A vegetarian type of food that cures freezing."); break;
			case MUSHROOM: 
				pline("Sometimes, you can get random effects from eating this vegetarian food item."); break;
			case MELON: 
				pline("A vegetarian type of food that cures confusion."); break;
			case SLIME_MOLD: 
				pline("This type of vegetarian food provides good nutrition and can be renamed. Default name is 'slime mold'."); break;
			case PEANUT_BAG: 
				pline("This vegetarian food item provides lots of nutrition."); break;
			case LUMP_OF_ROYAL_JELLY: 
				pline("Eating this vegetarian food item can increase your strength."); break;
			case CREAM_PIE: 
				pline("A vegetarian type of food that cures burns. It can also be thrown to blind enemies."); break;
			case SANDWICH: 
				pline("A meaty food item that provides moderate amounts of nutrition."); break;
			case CANDY_BAR: 
				pline("Eating this provides moderate amounts of nutrition without violating vegetarian conduct."); break;
			case FORTUNE_COOKIE: 
				pline("You may get a message if you eat this vegetarian food item."); break;
			case PANCAKE: 
				pline("This vegetarian type of food provides relatively good nutrition."); break;
			case UGH_MEMORY_TO_CREATE_INVENTORY: 
				pline("An edible item with an unknown effect. If you disabled the missing_safety option (it defaults to on), eating it might crash the game."); break;
			case TORTILLA: 
				pline("A rarely seen vegetarian food item that provides relatively little nutrition."); break;
			case CHEESE: 
				pline("Can be used to tame rats. It can be eaten by vegetarians but not by vegans. Not that anyone is likely to care."); break;
			case PILL: 
				pline("Swallowing this thing is like playing russian roulette. You may get lucky and experience a good effect but you might also get something really bad instead."); break;
			case HOLY_WAFER: 
				pline("A vegetarian type of food that is relatively filling and can cure certain negative effects."); break;
			case LEMBAS_WAFER: 
				pline("A type of 'elven' bread that is more filling than any real-world vegetarian food can ever be."); break;
			case CRAM_RATION: 
				pline("A bland ration of vegetarian food."); break;
			case FOOD_RATION: 
				pline("A very filling ration of food. For some weird reason this counts as vegetarian food even though we all know that in real life, only food containing meat can ever satiate your stomach."); break;
			case HACKER_S_FOOD: 
				pline("This vegetarian food ration can be eaten in one turn."); break;
			case K_RATION: 
				pline("Soldiers often carry these rations that can be eaten in one turn. For some reason they contain no meat - if I were a soldier in real life I'd be pissed if they won't serve meat!"); break;
			case C_RATION: 
				pline("Soldiers often carry these rations that can be eaten in one turn. For some reason they contain no meat - how can any real-life soldiers even concentrate on their tasks if they ain't getting no real food?"); break;
			case TIN: 
				pline("A tin that may contain some type of food. If you wield a tin opener, you can open it more quickly; after a tin has been opened, you can decide whether you really want to eat it. The nutritional value of a tin is randomized."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case POTION_CLASS:
		pline("%s - This is a potion. Color: %s. Material: %s. Appearance: %s. You can quaff it to experience its effects, but it's also possible to throw potions at monsters or bash them with it in melee.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case POT_BOOZE:
				pline("Wanna get high? Quaff this! It causes confusion and gives a little bit of nutrition."); break;
			case POT_FRUIT_JUICE:
				pline("A potion that provides some nutrition when quaffed."); break;
			case POT_SEE_INVISIBLE:
				pline("You can see invisible monsters and items if you quaff this."); break;
			case POT_SICKNESS:
				pline("A potion that should not be quaffed. Instead, dip weapons into it to poison them, or throw it at a monster."); break;
			case POT_SLEEPING:
				pline("Attacking a monster with this potion puts it to sleep. You can also quaff it yourself if you want to sleep for some reason."); break;
			case POT_CLAIRVOYANCE:
				pline("A potion that grants temporary clairvoyance if you quaff it."); break;
			case POT_CONFUSION:
				pline("You can drink this potion if you want to get confused, or hurl it at a monster instead."); break;
			case POT_HALLUCINATION:
				pline("Don't quaff this unless you want to hallucinate for hundreds of turns. Instead, throwing it at a monster will cause it to hallucinate for a while which might be much more useful."); break;
			case POT_HEALING:
				pline("A basic healing potion that restores a low amount of hit points. If the amount restored exceeds your maximum hit points, they will be increased."); break;
			case POT_EXTRA_HEALING:
				pline("A good healing potion that restores a medium amount of hit points and sometimes fixes other troubles as well. If the amount restored exceeds your maximum hit points, they will be increased."); break;
			case POT_RESTORE_ABILITY:
				pline("Quaffing this potion can restore lost attribute points."); break;
			case POT_BLINDNESS:
				pline("A potion that causes you to become blind if you quaff it. You can also fling it at opponents to blind them instead."); break;
			case POT_ESP:
				pline("This potion grants extra-sensory perception, a.k.a. telepathy if you quaff it."); break;
			case POT_GAIN_ENERGY:
				pline("A potion of mana that will also increase your maximum amount of mana. If the amount of mana restored exceeds the maximum, your maximum mana will go up even more."); break;
			case POT_GAIN_HEALTH:
				pline("This potion can be used for healing, but its main use is increasing your maximum health when quaffed."); break;
			case POT_BANISHING_FEAR:
				pline("A potion that will cure fear when quaffed."); break;
			case POT_ICE:
				pline("You will freeze solid if you quaff this potion, which is usually a bad thing. Better use it as a missile to slow down enemies."); break;
			case POT_FEAR:
				pline("Anyone who breathes the fumes of this potion will become afraid of its surroundings."); break;
			case POT_FIRE:
				pline("If you quaff this potion, you'll suffer from burns. This means it's better used as a thrown potion to burn enemies."); break;
			case POT_STUNNING:
				pline("Drinking this potion will stun you. If it hits a monster, the monster will be stunned too."); break;
			case POT_NUMBNESS:
				pline("Your limbs will be numbed from quaffing this potion, so you're probably better off using it against a monster instead."); break;
			case POT_URINE:
				pline("Quaffing this potion is only fatal if it's more than 50 turns old."); break;
			case POT_SLIME:
				pline("You will slowly turn into a green slime if you quaff this potion."); break;
			case POT_CANCELLATION:
				pline("Similar to an amnesia potion, but this potion will cancel your items or remove your intrinsics."); break;
			case POT_INVISIBILITY:
				pline("This potion can make the one quaffing it invisible."); break;
			case POT_MONSTER_DETECTION:
				pline("Quaffing this potion shows all monsters on the current dungeon level to you."); break;
			case POT_OBJECT_DETECTION:
				pline("A potion that reveals objects on the current level if quaffed."); break;
			case POT_ENLIGHTENMENT:
				pline("When quaffed, this potion displays a lot of information about your character, including whether you can pray to your god."); break;
			case POT_FULL_HEALING:
				pline("The best healing potion in the game that always restores at least 400 hit points, in addition to fixing several other troubles. It's very likely to increase your maximum hit points too."); break;
			case POT_LEVITATION:
				pline("When quaffed, this potion causes you to levitate for a period of time."); break;
			case POT_POLYMORPH:
				pline("Quaffing this potion will polymorph you into a random monster, which may be a good or bad thing. You may also want to use this as a throwing 'weapon' in order to polymorph monsters."); break;
			case POT_MUTATION:
				pline("Quaffing this potion will polymorph you into a random monster, which may be a good or bad thing. If monsters quaff or get hit by it, they will gain mutations."); break;
			case POT_SPEED:
				pline("When quaffed, this potion makes you move much faster for a period of time."); break;
			case POT_ACID:
				pline("A very useful potion that cures petrification when quaffed. It has a variety of other uses too."); break;
			case POT_OIL:
				pline("You can quaff this potion, but it can also be used to refill oil lamps, disarm certain types of traps and more."); break;
			case POT_SALT_WATER:
				pline("This water is extremely salty. If you quaff it, you'll probably vomit, so it's best used if you are sick from food poisoning."); break;
			case POT_GAIN_ABILITY:
				pline("A beneficial potion that can increase your stats if you quaff it."); break;
			case POT_GAIN_LEVEL:
				pline("You will gain a level if you quaff this potion."); break;
			case POT_INVULNERABILITY:
				pline("A very powerful potion that makes you immune to damage for a short while if you quaff it."); break;
			case POT_PARALYSIS:
				pline("Quaffing this potion is probably a bad idea because it will paralyze you for some turns. The same can happen to monsters that are subjected to its vapors in some way."); break;
			case POT_EXTREME_POWER:
				pline("A potion that can increase your maximum amount of hit points when quaffed."); break;
			case POT_RECOVERY:
				pline("This healing potion instantly restores your health to its maximum."); break;
			case POT_HEROISM:
				pline("A potion that makes you super-powerful for a short while, but you might be blinded for a few turns if you quaff it."); break;
			case POT_CYANIDE:
				pline("Drinking this potion is a very bad idea indeed. Use it as a thrown weapon instead."); break;
			case POT_RADIUM:
				pline("You'll get sick if you try quaffing this liquid, but so might your enemies if you expose them to this potion's vapors."); break;
			case POT_JOLT_COLA:
				pline("A hacker's beverage that can make you feel a little better, in a variety of ways."); break;
			case POT_PAN_GALACTIC_GARGLE_BLASTE:
				pline("If you quaff this, you will feel like having your brain smashed out by a slice of lemon wrapped around a large gold brick."); break;
			case POT_WATER:
				pline("Water potions behave differently if they are blessed (holy water) or cursed (unholy water). Quaffing (un)holy water has a variety of effects, and it will alter the blessed/cursed/uncursed status of items dipped into it. Plain water also has some marginal uses."); break;
			case POT_BLOOD:
				pline("A red liquid that is meant to be quaffed by vampires."); break;
			case POT_VAMPIRE_BLOOD:
				pline("Vampires love the taste of this potion, as it gives them nutrition and heals them. Non-vampires may become a vampire if they drink it."); break;
			case POT_AMNESIA:
				pline("The best item in the entire game. Throwing it at monsters can make them forget things, and quaffing a blessed one can cure your sickness and lycanthropy. Be careful though, as you might get hit by a nasty amnesia effect that wipes your memory."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case SCROLL_CLASS:
		pline("%s - This is a scroll. Color: %s. Material: %s. Appearance: %s. Reading it has a magic effect and uses up the scroll; some scroll effects are different if they are read while you are confused.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case SCR_CREATE_MONSTER: 
				pline("Reading this scroll summons some monsters for you to fight."); break;
			case SCR_CREATE_FAMILIAR: 
				pline("Reading this scroll summons a tame monster that will help you out."); break;
			case SCR_SUMMON_UNDEAD: 
				pline("This scroll summons undead monsters if read."); break;
			case SCR_TAMING: 
				pline("If you read this scroll, it tries to tame all adjacent monsters. Certain monsters may resist, and some boss monsters are outright immune to this effect."); break;
			case SCR_LIGHT: 
				pline("Illuminates the area around you."); break;
			case SCR_FOOD_DETECTION: 
				pline("Reading this scroll allows you to detect comestibles on the current level."); break;
			case SCR_GOLD_DETECTION: 
				pline("All piles of gold on the entire level are revealed to you if you read this."); break;
			case SCR_IDENTIFY: 
				pline("You may identify one or more objects if you use this scroll."); break;
			case SCR_INVENTORY_ID: 
				pline("A powerful identify scroll that always identifies everything in your main inventory. Pick up as much as you can before reading it, and move your container's contents in your main inventory too!"); break;
			case SCR_MAGIC_MAPPING: 
				pline("This scroll can reveal the map of your current dungeon level. However, certain levels are unmappable."); break;
			case SCR_FLOOD: 
				pline("A dangerous scroll that creates water, possibly drowning you."); break;
			case SCR_GAIN_MANA: 
				pline("Reading this scroll will increase your max mana."); break;
			case SCR_CONFUSE_MONSTER: 
				pline("Your melee attacks have a chance to confuse monsters after reading this scroll."); break;
			case SCR_SCARE_MONSTER: 
				pline("Reading this scroll is a waste. Its real purpose is to lie on the ground, keeping monsters away from it. However, it degrades every time you pick it up."); break;
			case SCR_ENCHANT_WEAPON: 
				pline("Your wielded weapon's enchantment goes up if you read this scroll. Beware, if the weapon's enchantment is +6 or higher, the weapon may blow up."); break;
			case SCR_ENCHANT_ARMOR: 
				pline("You may select one of your worn pieces of armor to increase its enchantment. Most pieces of armor have a chance to evaporate if they're already enchanted to +4 or higher. Elven armors won't evaporate unless they're at least +6 though."); break;
			case SCR_REMOVE_CURSE: 
				pline("This scroll can uncurse some of the items in your inventory if you read it."); break;
			case SCR_TELEPORTATION: 
				pline("A scroll meant to be used in emergency situations that teleports you to a random empty location on the current dungeon level. Beware, some special levels inhibit teleportation!"); break;
			case SCR_TELE_LEVEL: 
				pline("This scroll will get you out of most sticky situations by warping you to another dungeon level."); break;
			case SCR_WARPING: 
				pline("You will warp to any random dungeon level if you read this scroll. It may deposit you at some fairly dangerous place, too."); break;
			case SCR_FIRE: 
				pline("The best scroll in the game. You will need this to cure the sliming condition, which is difficult to cure otherwise. It can also damage monsters standing next to you, with the side effect of burning you a little."); break;
			case SCR_EARTH: 
				pline("Summons some boulders if read. Beware, they might hit your head and damage you."); break;
			case SCR_DESTROY_ARMOR: 
				pline("A scroll that can be used if you are wearing a cursed piece of armor and want to get rid of it. You can't select the affected piece of armor yourself though; rather, the game randomly destroys one of your worn armor items."); break;
			case SCR_AMNESIA: 
				pline("You will forget some of your spells as well as the current level's layout if you read this scroll."); break;
			case SCR_BAD_EFFECT: 
				pline("Causes a randomly selected bad effect if read."); break;
			case SCR_HEALING: 
				pline("A standard healing scroll that behaves similar to healing potions in other role-playing games by restoring some lost hit points."); break;
			case SCR_MANA: 
				pline("A standard mana scroll that behaves similar to mana potions in other role-playing games by restoring some of your mana."); break;
			case SCR_CURE: 
				pline("A powerful curing scroll that will fix the following status effects: sickness, sliming, stoning, confusion, blindness, stun, numbness, freezing, burn, fear and hallucination."); break;
			case SCR_PHASE_DOOR: 
				pline("Using this scroll will teleport you over a short distance."); break;
			case SCR_TRAP_DISARMING: 
				pline("If you read this scroll, all traps in a 3x3 radius centered on you will be removed."); break;
			case SCR_STANDARD_ID: 
				pline("Reading this scroll allows you to identify exactly one item in your main inventory."); break;
			case SCR_CHARGING: 
				pline("This scroll can be read to charge an object, which must be in your main inventory and of an item type that can be charged, e.g. a wand. Be careful, recharging an item too many times may cause it to explode."); break;
			case SCR_RANDOM_ENCHANTMENT: 
				pline("Using this scroll will allow you to pick an item that you want to have randomly enchanted. The item in question might get a positive or negative enchantment. However, if the item had a positive enchantment before it will first be set to +0 and get enchanted afterwards, so it's probably better to use it on items that are already +0 or worse."); break;
			case SCR_GENOCIDE: 
				pline("A powerful magic scroll that can be read to permanently get rid of a monster type and also prevent any more of them to spawn. Not all monster types can be genocided though."); break;
			case SCR_PUNISHMENT: 
				pline("If you read this scroll, you receive a heavy iron ball that is heavy and will cause you to move slower if you don't pick it up, but the ball can be wielded as a weapon."); break;
			case SCR_STINKING_CLOUD: 
				pline("This scroll prompts you for a location to release a cloud of gas. However, you can't place the cloud in an unlit area or too far away from you."); break;
			case SCR_TRAP_DETECTION: 
				pline("A scroll that shows traps on your current dungeon level if read."); break;
			case SCR_ACQUIREMENT: 
				pline("You may wish for an object type if you read this."); break;
			case SCR_PROOF_ARMOR: 
				pline("A random worn armor-class item is made erosionproof if you read this."); break;
			case SCR_PROOF_WEAPON: 
				pline("If you wield a weapon and read this scroll, that weapon will become erosionproof."); break;
			case SCR_MASS_MURDER: 
				pline("A weaker version of the genocide scroll that only eliminates living monsters of the specified type."); break;
			case SCR_UNDO_GENOCIDE: 
				pline("If you already genocided a monster type, you can use this scroll to re-enable it to spawn."); break;
			case SCR_REVERSE_IDENTIFY: 
				pline("A quirky special version of the identify scroll, this thing prompts you to enter the name or description of an object which is then identified."); break;
			case SCR_WISHING: 
				pline("Allows you to wish for an object."); break;
			case SCR_CONSECRATION: 
				pline("You must be standing in a room for this scroll to work. If you do, it will create an altar underneath you."); break;
			case SCR_ENTHRONIZATION: 
				pline("This scroll works only if you're in a room, which causes it to create a throne at your current location."); break;
			case SCR_FOUNTAIN_BUILDING: 
				pline("If you read this scroll while in a room, a fountain appears below you. Otherwise, nothing happens."); break;
			case SCR_SINKING: 
				pline("Doesn't work outside of a room. What it does is building a sink on your current tile."); break;
			case SCR_WC: 
				pline("Builds a toilet on your square, but only if that square is in a room."); break;
			case SCR_LAVA: 
				pline("Reading this scroll turns some ordinary floor squares into lava."); break;
			case SCR_STONING: 
				pline("Read this scroll if you want to become a statue."); break;
			case SCR_GROWTH: 
				pline("You will create lots of trees if you read this scroll."); break;
			case SCR_ICE: 
				pline("Normal floor becomes icy in a radius around you if you read this."); break;
			case SCR_CLOUDS: 
				pline("A scroll that can be read to create clouds around you."); break;
			case SCR_BARRHING: 
				pline("This scroll creates iron bars on empty floor squares in your vicinity."); break;
			case SCR_LOCKOUT: 
				pline("Corridors near you turn into solid rock walls and doors automatically repair and lock themselves if you read this scroll."); break;
			case SCR_ROOT_PASSWORD_DETECTION: 
				pline("This scroll has the computer's root password written on it, which you can read. It's likely to be useless anyway."); break;
			case SCR_TRAP_CREATION: 
				pline("A dangerous scroll that creates traps around you."); break;
			case SCR_SLEEP: 
				pline("Reading this scroll puts you to sleep, leaving you open to monsters attacking you."); break;
			case SCR_BLANK_PAPER: 
				pline("A scroll that doesn't have a magic formula written on it. You may use a magic marker to turn it into another type of scroll."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case SPBOOK_CLASS:
		pline("%s - This is a spellbook. Color: %s. Material: %s. Appearance: %s. Reading it allows you to learn a new spell permanently, or refresh your memory if you already know the spell.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case SPE_FORCE_BOLT:
				pline("A spell that fires an invisible beam. It can damage monsters, items and certain dungeon features."); break;
			case SPE_CREATE_MONSTER:
				pline("Casting this spell summons random monsters."); break;
			case SPE_DRAIN_LIFE:
				pline("This spell drains the life force out of monsters, reducing their level. It also reduces the enchantment of objects it hits."); break;
			case SPE_COMMAND_UNDEAD:
				pline("A spell that attempts to tame all adjacent undead monsters."); break;
			case SPE_SUMMON_UNDEAD:
				pline("Summons an undead monster."); break;
			case SPE_STONE_TO_FLESH:
				pline("This spell can be cast at items, dungeon features and monsters that are made of stone, turning them into meat."); break;
			case SPE_HEALING:
				pline("A basic healing spell that can be used on yourself or on a monster to heal them."); break;
			case SPE_CURE_BLINDNESS:
				pline("Casting this spell cures blindness."); break;
			case SPE_CURE_SICKNESS:
				pline("A powerful spell that cures any food poisoning and illness you might be suffering from."); break;
			case SPE_CURE_HALLUCINATION:
				pline("If you successfully cast this spell, your hallucinations will end."); break;
			case SPE_CURE_CONFUSION:
				pline("A spell that can be successfully cast even while confused, and that's also the reason why one would cast it in the first place since it cures the confusion status."); break;
			case SPE_CURE_STUN:
				pline("By casting this spell, you can get rid of the stun condition."); break;
			case SPE_GENOCIDE:
				pline("Yes, this is not a joke. Casting this spell might allow you to genocide some monster. However, it often fails, and even on the off chance it doesn't, you will only be able to genocide a single monster species."); break;
			case SPE_EXTRA_HEALING:
				pline("A more powerful healing spell that can heal more hit points than the standard healing spell. Can be cast at yourself or at a monster."); break;
			case SPE_FULL_HEALING:
				pline("This spell can be aimed at yourself or a monster to heal a large amount of hit points."); break;
			case SPE_RESTORE_ABILITY:
				pline("If your attributes have been damaged, this spell may restore them to their previous values."); break;
			case SPE_CREATE_FAMILIAR:
				pline("Casting this spell summons a monster that fights on your side."); break;
			case SPE_LIGHT:
				pline("A spell that lights up dark areas."); break;
			case SPE_DETECT_MONSTERS:
				pline("Allows you to see all monsters on the current dungeon level."); break;
			case SPE_DETECT_FOOD:
				pline("This spell shows you the food items on the current level."); break;
			case SPE_CLAIRVOYANCE:
				pline("You will become clairvoyant for a period of time by casting this."); break;
			case SPE_DETECT_UNSEEN:
				pline("A spell that grants you the ability to see invisible things."); break;
			case SPE_IDENTIFY:
				pline("Casting this spell allows you to identify some objects in your inventory."); break;
			case SPE_DETECT_TREASURE:
				pline("This spell detects objects on the current level."); break;
			case SPE_MAGIC_MAPPING:
				pline("A spell that reveals what the current dungeon level looks like, unless it's a non-mappable special level."); break;
			case SPE_ENTRAPPING:
				pline("Casting this spell allows you to detect traps on the level."); break;
			case SPE_FINGER:
				pline("This spell fires an invisible beam that shows you the attributes of monsters hit by it."); break;
			case SPE_CHEMISTRY:
				pline("Casting this spell has no effect, but knowing it increases the likelihood of the chemistry set actually working."); break;
			case SPE_DETECT_FOOT:
				pline("According to the Sporkhack creator, this spell is supposed to be useless. But this is Slash'EM Extended, where it allows you to make enemies fall over unconscious by... well, just see it for yourself. :D"); break;
			case SPE_CONFUSE_MONSTER:
				pline("Your melee attacks can confuse monsters if you cast this spell."); break;
			case SPE_FORBIDDEN_KNOWLEDGE:
				pline("Learning this spell causes your deity to become very angry. Casting it angers your deity even more, but grants resistance to damage and spells for a while. The appearance and level of this book are random."); break;
			case SPE_SLOW_MONSTER:
				pline("This spell fires an invisible beam that slows targets."); break;
			case SPE_CAUSE_FEAR:
				pline("Use this spell to make monsters flee from you."); break;
			case SPE_CHARM_MONSTER:
				pline("A very powerful spell that tries to tame adjacent monsters. Their magic resistance prevents this from working sometimes, and since the spell used to be totally unbalanced, it can also fail if the monster isn't resistant at all."); break;
			case SPE_ENCHANT_WEAPON:
				pline("This spell rarely works, but if it does, it tries to enchant your wielded weapon. Beware, if the weapon in question already has a very high enchantment, it might blow up."); break;
			case SPE_ENCHANT_ARMOR:
				pline("This spell rarely works, but if it does, it tries to enchant a user-selected worn piece of armor. Beware, if the armor in question already has a very high enchantment, it might blow up."); break;
			case SPE_CHARGING:
				pline("Cast this spell if you want to recharge your objects."); break;
			case SPE_PROTECTION:
				pline("A spell that temporarily improves your armor class."); break;
			case SPE_RESIST_POISON:
				pline("This spell provides temporary poison resistance when cast."); break;
			case SPE_RESIST_SLEEP:
				pline("This spell provides temporary sleep resistance when cast."); break;
			case SPE_ENDURE_COLD:
				pline("This spell provides temporary cold resistance when cast."); break;
			case SPE_ENDURE_HEAT:
				pline("This spell provides temporary fire resistance when cast."); break;
			case SPE_INSULATE:
				pline("This spell provides temporary shock resistance when cast."); break;
			case SPE_REMOVE_CURSE:
				pline("A spell that might uncurse some of your cursed items. It only affects items in your main inventory."); break;
			case SPE_TURN_UNDEAD:
				pline("Fires an invisible beam that makes undead monsters flee and revives dead monsters."); break;
			case SPE_ANTI_DISINTEGRATION:
				pline("This spell provides temporary disintegration resistance when cast."); break;
			case SPE_BOTOX_RESIST:
				pline("This spell provides temporary sickness resistance when cast."); break;
			case SPE_ACIDSHIELD:
				pline("This spell provides temporary acid resistance when cast."); break;
			case SPE_GODMODE:
				pline("Exactly what it says on the tin. However, the invulnerability granted by casting this spell will wear off after a few turns."); break;
			case SPE_RESIST_PETRIFICATION:
				pline("This spell provides temporary petrification resistance when cast."); break;
			case SPE_JUMPING:
				pline("Casting this spell allows you to jump to a nearby empty location. There are some rules to consider though, e.g. the square must be lit and there needs to be a clear path between you and your location."); break;
			case SPE_HASTE_SELF:
				pline("This spell allows you to move at very fast speed for a period of time."); break;
			case SPE_ENLIGHTEN:
				pline("A spell that displays your current resistances, whether it is safe to pray, etc."); break;
			case SPE_INVISIBILITY:
				pline("Use this spell if you want to turn invisible for a period of time."); break;
			case SPE_LEVITATION:
				pline("This spell allows you to levitate for a while."); break;
			case SPE_TELEPORT_AWAY:
				pline("A spell that can be used to fire teleport beams at yourself, monsters and objects."); break;
			case SPE_PASSWALL:
				pline("Casting this spell allows you to walk through walls for a limited amount of time. Beware, certain special levels have walls that resist this ability."); break;
			case SPE_POLYMORPH:
				pline("A spell that can be cast at stuff to polymorph it."); break;
			case SPE_MUTATION:
				pline("A spell that has several uses. Zapping it at yourself will polymorph you, zapping it at objects will polymorph them, and if you zap a monster, it will gain mutations."); break;
			case SPE_KNOCK:
				pline("This spell opens things like locked doors or chests."); break;
			case SPE_FLAME_SPHERE:
				pline("Conjures a flaming sphere that attacks your enemies by exploding and doing some fire damage."); break;
			case SPE_FREEZE_SPHERE:
				pline("Conjures a freezing sphere that attacks your enemies by exploding and doing some cold damage."); break;
			case SPE_SHOCKING_SPHERE:
				pline("Conjures a shocking sphere that attacks your enemies by exploding and doing some shock damage."); break;
			case SPE_ACID_SPHERE:
				pline("Conjures an acidic sphere that attacks your enemies by exploding and doing some acid damage."); break;
			case SPE_WIZARD_LOCK:
				pline("A spell that fires invisible locking beams, which have an effect only if they hit something lockable. It can also repair broken doors."); break;
			case SPE_DIG:
				pline("Casting this can dig through walls and other obstacles."); break;
			case SPE_CANCELLATION:
				pline("This spell can be cast at objects and monsters to cancel them."); break;
			case SPE_REFLECTION:
				pline("You can reflect beams and similar attacks for a limited amount of time if you cast this spell."); break;
			case SPE_REPAIR_ARMOR:
				pline("Casting this spell repairs some of your armor. You may choose which item to repair."); break;
			case SPE_MAGIC_MISSILE:
				pline("A spell that fires a blue ray to do some damage to an enemy."); break;
			case SPE_FIREBALL:
				pline("This spell blasts monsters with a bolt of fire."); break;
			case SPE_CONE_OF_COLD:
				pline("Cast this spell in order to fire a blast of cold."); break;
			case SPE_SLEEP:
				pline("A spell that fires sleep rays at monsters."); break;
			case SPE_FINGER_OF_DEATH:
				pline("This spell shoots death rays that can instantly kill enemies."); break;
			case SPE_LIGHTNING:
				pline("A spell that projects a beam of lightning in a direction of your choice."); break;
			case SPE_POISON_BLAST:
				pline("This spell fires poison beams at enemies."); break;
			case SPE_ACID_STREAM:
				pline("A spell that does acid damage by hitting monsters with a ray."); break;
			case SPE_SOLAR_BEAM:
				pline("Fires a beam of pure solar energy that does a ton and a half of damage to enemies."); break;
			case SPE_BLANK_PAPER:
				pline("This spellbook is blank. You may be able to write on it with a magic marker, turning it into another spellbook."); break;
			case SPE_BOOK_OF_THE_DEAD:
				pline("An arcane book that can be read. Reciting the eldritch formulas contained therein may raise the dead, so be careful."); break;
			case SPE_DARKNESS:
				pline("Cast this spell if you want to turn lit areas into unlit ones."); break;
			case SPE_DETECT_ARMOR_ENCHANTMENT:
				pline("This spell detects the enchantment value of all armor items in your main inventory."); break;
			case SPE_CONFUSE_SELF:
				pline("Want to confuse yourself? Cast this spell!"); break;
			case SPE_STUN_SELF:
				pline("You can cast this spell to be stunned on purpose, should you wish to do so for some weird reason."); break;
			case SPE_BLIND_SELF:
				pline("A spell that blinds you for a period of time. This is best used in conjunction with telepathy."); break;
			case SPE_CORRODE_METAL:
				pline("This spell corrodes metal items in your main inventory, causing them to degrade."); break;
			case SPE_DISSOLVE_FOOD:
				pline("Want to get rid of your food without eating it? Then cast this spell while having food in your main inventory!"); break;
			case SPE_AGGRAVATE_MONSTER:
				pline("This spell will aggravate monsters if you cast it."); break;
			case SPE_REMOVE_BLESSING:
				pline("A spell that turns all blessed items in your main inventory into uncursed ones."); break;

			case SPE_DISINTEGRATION:
				pline("This very powerful spell will fire invisible disintegration beams that can be used to instakill monsters."); break;
			case SPE_PETRIFY:
				pline("Cast this spell at monsters to turn them to stone."); break;
			case SPE_PARALYSIS:
				pline("You can shoot paralysis beams by casting this spell."); break;
			case SPE_LEVELPORT:
				pline("If you cast this spell, you will teleport to a random dungeon level in your current branch unless there's something that prevents you from levelporting."); break;
			case SPE_BANISHING_FEAR:
				pline("A spell that cures the 'fear' status conditions. At higher spell levels it allows you to resist fear for a period of time."); break;
			case SPE_CURE_FREEZE:
				pline("Got hit by the 'freeze' status effect? Cast this spell and it will go away!"); break;
			case SPE_CURE_BURN:
				pline("This spell will cure you of burns."); break;
			case SPE_CURE_NUMBNESS:
				pline("A spell that can be cast to cure numbness."); break;
			case SPE_TIME_STOP:
				pline("This very powerful spell will stop the flow of time for a brief period."); break;
			case SPE_STINKING_CLOUD:
				pline("Casting this spell allows you to place a stinking cloud on a nearby visible location."); break;
			case SPE_GAIN_LEVEL:
				pline("An absurdly powerful spell that may increase your character level. However, it often fails."); break;
			case SPE_MAP_LEVEL:
				pline("This spell fails most of the time, but if it doesn't, it will reveal the map of the entire level and show all objects as well as all traps."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case WAND_CLASS:
		pline("%s - This is a wand. Color: %s. Material: %s. Appearance: %s. It can be zapped for an effect; some wands will have a direction that you may choose. However, you can also apply wands to break them and release the energy contained therein, which has effects similar to what the wand normally does, or engrave with them which may give you some clues about what the wand actually does.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case WAN_LIGHT: 
				pline("Zapping this wand will illuminate your surroundings."); break;
			case WAN_NOTHING: 
				pline("A wand that does nothing at all."); break;
			case WAN_ENLIGHTENMENT: 
				pline("If you zap this wand, you can see some clues about your status, e.g. alignment, whether it is safe to pray and if your luck is positive or negative."); break;
			case WAN_HEALING: 
				pline("Zapping this wand at a living creature will heal it. You can also zap yourself with it."); break;
			case WAN_LOCKING: 
				pline("If you zap this wand at something that can be locked, e.g. a door or chest, it will be locked. It can also transform broken doorways into fully functional locked doors."); break;
			case WAN_MAKE_INVISIBLE: 
				pline("Zap this wand at something to make that 'something' invisible!"); break;
			case WAN_MAKE_VISIBLE: 
				pline("A wand that can be zapped to remove invisibility from monsters and items."); break;
			case WAN_IDENTIFY: 
				pline("If you zap this wand, you may identify some of your inventory items."); break;
			case WAN_REMOVE_CURSE: 
				pline("Zapping this wand may uncurse some items in your inventory."); break;
			case WAN_PUNISHMENT: 
				pline("A wand that chains you to a heavy iron ball if you zap it, or if you're already punished, your iron ball gets heavier."); break;
			case WAN_OPENING: 
				pline("This wand fires invisible beams that can open locks."); break;
			case WAN_PROBING: 
				pline("Zapping this wand at a monster shows information about it, including its inventory contents."); break;
			case WAN_SECRET_DOOR_DETECTION: 
				pline("This wand can be zapped to detect secret doors close by."); break;
			case WAN_TRAP_DISARMING: 
				pline("Zap this wand if you want to get rid of traps on your square and the eight squares surrounding you."); break;
			case WAN_ENTRAPPING: 
				pline("A wand that allows you to detect traps."); break;
			case WAN_TELE_LEVEL: 
				pline("Zap this wand to land on another dungeon level!"); break;
			case WAN_TIME_STOP: 
				pline("This powerful wand is capable of stopping monsters from moving around."); break;
			case WAN_GENOCIDE: 
				pline("You can zap this wand to genocide a species of monsters."); break;
			case WAN_STINKING_CLOUD: 
				pline("With every zap of this wand, you can place a stinking cloud on a nearby visible square."); break;
			case WAN_TRAP_CREATION: 
				pline("Zapping this wand will surround you with randomly generated traps."); break;
			case WAN_DARKNESS: 
				pline("A wand that makes the area surrounding you unlit."); break;
			case WAN_MAGIC_MAPPING: 
				pline("Zap this wand if you want an idea of what the current level layout looks like!"); break;
			case WAN_DETECT_MONSTERS:
				pline("A wand that allows you to detect monsters wherever they are on the current level."); break;
			case WAN_OBJECTION:
				pline("Detects objects on the current dungeon level."); break;
			case WAN_SLOW_MONSTER:
				pline("Zapping this at a monster causes it to move slower."); break;
			case WAN_SPEED_MONSTER:
				pline("Zapping yourself with this wand gives you permanent intrinsic speed. It can also be zapped at monsters to speed them up."); break;
			case WAN_HASTE_MONSTER:
				pline("Zapping yourself with this wand will make you very fast for a while. It can also be zapped at monsters to speed them up."); break;
			case WAN_STRIKING:
				pline("A wand that shoots invisible bolts of force to damage enemies, break doors and otherwise interact with the dungeon."); break;
			case WAN_UNDEAD_TURNING:
				pline("This wand can be zapped at living undead monsters to make them flee, or you can zap corpses with it to reanimate them. Unfortunately you can't zap yourself after you die."); break;
			case WAN_DRAINING:
				pline("A wand that can be zapped at monsters and objects to drain their level."); break;
			case WAN_REDUCE_MAX_HITPOINTS:
				pline("A wand that can be zapped at monsters to reduce their maximum amount of hit points. Zapping it at objects drains their enchantment."); break;
			case WAN_INCREASE_MAX_HITPOINTS:
				pline("A wand that can be zapped at monsters or yourself to increase their maximum amount of hit points. Zapping it at objects drains their negative enchantment, bringing it closer to +0."); break;
			case WAN_CANCELLATION:
				pline("Cancels whatever you zap it at. Monsters lose their ability to use certain types of special attacks while objects will lose their enchantments."); break;
			case WAN_CREATE_MONSTER:
				pline("Zapping this wand summons monsters."); break;
			case WAN_CREATE_FAMILIAR:
				pline("Zapping this wand summons a tame monster."); break;
			case WAN_BAD_EFFECT:
				pline("This wand will subject you to a random bad effect if it is zapped. It doesn't matter WHO zaps it, it's always you who will suffer from its effect. Think that's unfair? Well, suck it up, this game was never designed to be fair in the first place!"); break;
			case WAN_SUMMON_UNDEAD:
				pline("A wand that summons some undead monsters if it is zapped."); break;
			case WAN_FEAR:
				pline("Firing this wand at a monster may cause it to run away in fear."); break;
			case WAN_WIND:
				pline("A wand that creates a powerful wind to push monsters and objects out of your way."); break;
			case WAN_POLYMORPH:
				pline("Zapping this wand at monsters, objects or yourself will polymorph whatever it hits. Be aware of the fact that polymorphing monsters and objects is temporary."); break;
			case WAN_MUTATION:
				pline("Zapping this wand will add mutations if the invisible beam hits a monster. Hitting yourself or an item will polymorph it."); break;
			case WAN_TELEPORTATION:
				pline("This wand can be zapped at monsters and objects to teleport them to a random empty location on the current dungeon level. Zapping yourself is also possible, but only if you're not on a no-teleport level, so in the case of doubt don't zap yourself, but zap the monster attacking you!"); break;
			case WAN_BANISHMENT:
				pline("A very powerful wand that banishes monsters to a random dungeon level. You may also zap it at yourself, maybe for getting away from a dangerous opponent."); break;
			case WAN_CREATE_HORDE:
				pline("This is a wand of create monster on steroids, which means it will summon a ton of monsters with each zap."); break;
			case WAN_EXTRA_HEALING:
				pline("A more powerful healing wand that restores more hit points than a wand of healing."); break;
			case WAN_FULL_HEALING:
				pline("The ultimate healing wand that restores lots of hit points of whatever you zap it at."); break;
			case WAN_WONDER:
				pline("This wand has a random effect whenever you zap it."); break;
			case WAN_BUGGING:
				pline("Zapping this wand summons bugs. Don't get your hopes up - the bugs never leave corpses, so you won't be able to use this wand for creating sacrifice fodder."); break;
			case WAN_WISHING:
				pline("Probably the most powerful wand in existence, this one allows you to wish for an object every time you zap it. However, it can be recharged at most once."); break;
			case WAN_ACQUIREMENT:
				pline("A weaker version of the wand of wishing, this wand allows you to wish for an object class. What you receive exactly is determined randomly, and you can't recharge this wand if it has already been recharged one or more times."); break;
			case WAN_CLONE_MONSTER:
				pline("Zapping this wand at monsters will create a duplicate of that monster. You can also zap yourself, but unless you're polymorphed into a monster it probably won't work."); break;
			case WAN_CHARGING:
				pline("A wand that allows you to charge an object if you zap it. Beware, since this wand is so powerful you can't charge it more than once (but you can use it to charge itself if it hasn't been recharged yet)."); break;
			case WAN_DIGGING:
				pline("Zapping this wand in a direction will try to dig open some walls and other obstacles that are in the way. It can also be used for engraving with good quality, and it can engrave up to 50 characters in a single turn."); break;
			case WAN_MAGIC_MISSILE:
				pline("This wand can be used to blast the shit out of your enemies by sending a magical ray at them."); break;
			case WAN_FIRE:
				pline("A wand that can engrave words in the floor permanently. It may also be used to shoot bolts of fire in order to burn enemies."); break;
			case WAN_COLD:
				pline("This wand shoots a cold ray that can damage monsters and freeze certain dungeon features."); break;
			case WAN_SLEEP:
				pline("A wand that fires sleep rays in a direction of your choosing."); break;
			case WAN_DEATH:
				pline("This wand allows you to blast enemies with death rays, instantly killing everything that doesn't resist."); break;
			case WAN_LIGHTNING:
				pline("Engravings creates with this wand are permanent, but you will be blinded for a few turns. It can also be zapped to shoot lightning bolts at enemies."); break;
			case WAN_FIREBALL:
				pline("A wand that blasts enemies with powerful fire explosions. It is also a good choice for engraving, being able to create permanent engravings."); break;
			case WAN_ACID:
				pline("Zapping this wand in a direction releases an acid ray to do damage."); break;
			case WAN_SOLAR_BEAM:
				pline("A wand that sends rays of pure solar energy at your enemies."); break;
			case WAN_STONING:
				pline("A wand that fires invisible beams to turn monsters to stone."); break;
			case WAN_CURSE_ITEMS:
				pline("If anyone zaps this wand, your inventory items become cursed."); break;
			case WAN_AMNESIA:
				pline("You should prevent at all costs that anyone zaps this thing, for if it happens, you will suffer from amnesia!"); break;
			case WAN_BAD_LUCK:
				pline("Every zap of this wand reduces your luck by one point, regardless of who zapped it."); break;
			case WAN_REMOVE_RESISTANCE:
				pline("Zapping this wand will remove random intrinsics."); break;
			case WAN_CORROSION:
				pline("A wand that corrodes some of your inventory if you zap it, and also if someone else zaps it."); break;
			case WAN_FUMBLING:
				pline("Zapping this wand will cause you to fumble. It doesn't matter who zapped it either."); break;
			case WAN_STARVATION:
				pline("This wand will reduce your nutrition if you, or anyone else, zaps it."); break;
			case WAN_PARALYSIS:
				pline("A wand that fires invisible beams to paralyze monsters."); break;
			case WAN_DISINTEGRATION:
				pline("For instances where a wand of death isn't good enough, use this to fire invisible disintegration beams."); break;
			case WAN_GAIN_LEVEL:
				pline("This wand is very powerful - every zap will increase your character level by 1."); break;
			case WAN_MANA:
				pline("Zapping this wand will restore some of your mana."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case COIN_CLASS:
		pline("%s - This is a coin. ",xname(obj) );
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case GOLD_PIECE: 
				pline("These are zorkmids, also known as the currency of the game. They actually shouldn't be appearing in your main inventory."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case GEM_CLASS:
		pline("%s - This is a gem. Color: %s. Appearance: %s. Some of them increase your score at the end of the game, provided you didn't die, and since ascension is next to impossible, you'll probably not care about score anyway. However, they can also be used as sling ammunition, some gray stones may have certain special effects, and throwing gems to unicorns can increase your luck.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case DILITHIUM_CRYSTAL:
				pline("A white gem with a mohs hardness of 5, worth 4500 zorkmids."); break;
			case WONDER_STONE:
				pline("An invisible gem with a mohs hardness of 13, worth 5000 zorkmids."); break;
			case DIAMOND:
				pline("A white gem with a mohs hardness of 10, worth 4000 zorkmids."); break;
			case CYAN_STONE:
				pline("A teal gem with a mohs hardness of 8, worth 600 zorkmids."); break;
			case MOONSTONE:
				pline("A white gem with a mohs hardness of 6, worth 500 zorkmids."); break;
			case DISTHENE:
				pline("A teal gem with a mohs hardness of 7, worth 400 zorkmids."); break;
			case PERIDOT:
				pline("A radiant gem with a mohs hardness of 7, worth 1200 zorkmids."); break;
			case PREHNITE:
				pline("A radiant gem with a mohs hardness of 6, worth 500 zorkmids."); break;
			case CHALCEDON:
				pline("A cyan gem with a mohs hardness of 7, worth 800 zorkmids."); break;
			case CHRYSOCOLLA:
				pline("A cyan gem with a mohs hardness of 3, worth 800 zorkmids."); break;
			case APOPHYLLITE:
				pline("A teal gem with a mohs hardness of 5, worth 400 zorkmids."); break;
			case ANDALUSITE:
				pline("A radiant gem with a mohs hardness of 7, worth 1500 zorkmids."); break;
			case EPIDOTE:
				pline("A green gem with a mohs hardness of 7, worth 900 zorkmids."); break;
			case CHAROITE:
				pline("A violet gem with a mohs hardness of 6, worth 1200 zorkmids."); break;
			case DIOPTASE:
				pline("A radiant gem with a mohs hardness of 5, worth 2500 zorkmids."); break;
			case RUBY:
				pline("A red gem with a mohs hardness of 9, worth 3500 zorkmids."); break;
			case JACINTH:
				pline("An orange gem with a mohs hardness of 9, worth 3250 zorkmids."); break;
			case ANHYDRITE:
				pline("A teal gem with a mohs hardness of 4, worth 200 zorkmids."); break;
			case HALITE:
				pline("A white gem with a mohs hardness of 2, worth 200 zorkmids."); break;
			case MARBLE:
				pline("A white gem with a mohs hardness of 3, worth 200 zorkmids."); break;
			case SAPPHIRE:
				pline("A blue gem with a mohs hardness of 9, worth 3000 zorkmids."); break;
			case BLACK_OPAL:
				pline("A black gem with a mohs hardness of 8, worth 2500 zorkmids."); break;
			case EMERALD:
				pline("A green gem with a mohs hardness of 8, worth 2500 zorkmids."); break;
			case TURQUOISE:
				pline("A green gem with a mohs hardness of 6, worth 1500 zorkmids."); break;
			case AMAZONITE:
				pline("A cyan gem with a mohs hardness of 6, worth 1000 zorkmids."); break;
			case SODALITH:
				pline("A blue gem with a mohs hardness of 6, worth 1000 zorkmids."); break;
			case VIVIANITE:
				pline("A cyan gem with a mohs hardness of 2, worth 900 zorkmids."); break;
			case KUNZITE:
				pline("A pink gem with a mohs hardness of 7, worth 600 zorkmids."); break;
			case CIRMOCLINE:
				pline("A pink gem with a mohs hardness of 11, worth 4000 zorkmids."); break;
			case CITRINE:
				pline("A yellow gem with a mohs hardness of 6, worth 1500 zorkmids."); break;
			case AQUAMARINE:
				pline("A green gem with a mohs hardness of 8, worth 1500 zorkmids."); break;
			case AMBER:
				pline("A yellowish brown gem with a mohs hardness of 2, worth 1000 zorkmids."); break;
			case LAPIS_LAZULI:
				pline("A blue gem with a mohs hardness of 5, worth 600 zorkmids."); break;
			case TOPAZ:
				pline("A yellowish brown gem with a mohs hardness of 8, worth 900 zorkmids."); break;
			case JET:
				pline("A black gem with a mohs hardness of 7, worth 850 zorkmids."); break;
			case OPAL:
				pline("A white gem with a mohs hardness of 6, worth 800 zorkmids."); break;
			case CHRYSOBERYL:
				pline("A yellow gem with a mohs hardness of 5, worth 700 zorkmids."); break;
			case GARNET:
				pline("A red gem with a mohs hardness of 7, worth 700 zorkmids."); break;
			case SPINEL:
				pline("A pink gem with a mohs hardness of 8, worth 600 zorkmids."); break;
			case AMETHYST:
				pline("A violet gem with a mohs hardness of 7, worth 600 zorkmids."); break;
			case JASPER:
				pline("A red gem with a mohs hardness of 7, worth 500 zorkmids."); break;
			case MALACHITE:
				pline("A green gem with a mohs hardness of 4, worth 800 zorkmids."); break;
			case COVELLINE:
				pline("A black gem with a mohs hardness of 2, worth 700 zorkmids."); break;
			case FLUORITE:
				pline("A violet gem with a mohs hardness of 4, worth 400 zorkmids."); break;
			case MORGANITE:
				pline("A pink gem with a mohs hardness of 8, worth 2000 zorkmids."); break;
			case ORTHOCLASE:
				pline("A yellow gem with a mohs hardness of 6, worth 2000 zorkmids."); break;
			case ROSE_QUARTZ:
				pline("A pink gem with a mohs hardness of 7, worth 700 zorkmids."); break;
			case TOURMALINE:
				pline("A red gem with a mohs hardness of 7, worth 200 zorkmids."); break;
			case RHODOCHROSITE:
				pline("A red gem with a mohs hardness of 4, worth 200 zorkmids."); break;
			case OBSIDIAN:
				pline("A black gem with a mohs hardness of 6, worth 200 zorkmids."); break;
			case AGATE:
				pline("A orange gem with a mohs hardness of 6, worth 200 zorkmids."); break;
			case JADE:
				pline("A green gem with a mohs hardness of 6, worth 300 zorkmids."); break;
			case LUCKSTONE:
				pline("This gray stone influences your luck if you keep it in your main inventory. It can also prevent luck from timing out."); break;
			case HEALTHSTONE:
				pline("Healthstones are usually generated cursed, but if you carry around noncursed ones your health regeneration will speed up."); break;
			case LOADSTONE:
				pline("A very heavy stone that is usually generated cursed. As long as it is cursed, you can't drop it. Trying to pick one up will ignore all restrictions that would usually prevent you from picking up an item, so be careful!"); break;
			case TOUCHSTONE:
				pline("Rubbing gems on this stone may allow you to find out more about them."); break;
			case SALT_CHUNK:
				pline("A very useless gray stone that has been added to the game just to re-obscure the identification of actually useful gray stones. The only thing you can do with it is to dip it into potions, and maybe you get a potion of salt water which is also next to useless."); break;
			case WHETSTONE:
				pline("This item is meant to be used in conjunction with things that can be sharpened, by rubbing them on it. However, it requires you to be near a source of water."); break;
			case MANASTONE:
				pline("Manastones are usually generated cursed, but if you carry around noncursed ones your energy regeneration will speed up."); break;
			case SLEEPSTONE:
				pline("A gray stone that is usually generated cursed. If you carry it in your open inventory, you will fall asleep even if you are sleep resistant. It also halves the chance of waking up from combat."); break;
			case STONE_OF_MAGIC_RESISTANCE:
				pline("Slotless magic resistance can be obtained by having this stone in your inventory. Beware: if the stone is cursed, you will take double damage from everything!"); break;
			case LOADBOULDER:
				pline("This extremely heavy item is usually generated cursed and can't be dropped unless you uncurse it; if you try to pick it up, you will always do so even if it would overburden you! It's okay to pick one up if you are a giant, though."); break;
			case FLINT:
				pline("A projectile meant to be in conjunction with a sling to do damage to enemies."); break;
			case ROCK:
				pline("This is a basic rock that can be thrown, but firing it with a sling does more damage."); break;
			case RIGHT_MOUSE_BUTTON_STONE:
				pline("A stone that curses itself and causes the right mouse button to stop working."); break;
		 	case DISPLAY_LOSS_STONE:
				pline("A stone that curses itself and causes the display to fail."); break;
		 	case SPELL_LOSS_STONE:
				pline("A stone that curses itself and causes spell loss."); break;
		 	case YELLOW_SPELL_STONE:
				pline("A stone that curses itself and causes yellow spells."); break;
		 	case AUTO_DESTRUCT_STONE:
				pline("A stone that curses itself and causes an auto destruct mechanism to be initiated."); break;
		 	case MEMORY_LOSS_STONE:
				pline("A stone that curses itself and causes low local memory."); break;
		 	case INVENTORY_LOSS_STONE:
				pline("A stone that curses itself and causes the memory used for displaying an inventory window to run out. You cannot view this message in-game because you can't open your inventory while having this stone in there, so you gotta be peeking at the source! --Amy"); break;
		 	case BLACKY_STONE:
				pline("A stone that curses itself and causes Blacky to close in on you with his NG walls."); break;
		 	case MENU_BUG_STONE:
				pline("A stone that curses itself and causes the menu bug."); break;
		 	case SPEEDBUG_STONE:
				pline("A stone that curses itself and causes the speed bug."); break;
		 	case SUPERSCROLLER_STONE:
				pline("A stone that curses itself and causes the superscroller effect."); break;
		 	case FREE_HAND_BUG_STONE:
				pline("A stone that curses itself and causes your free hand to be free less often."); break;
		 	case UNIDENTIFY_STONE:
				pline("A stone that curses itself and causes your possessions to unidentify themselves."); break;
		 	case STONE_OF_THIRST:
				pline("A stone that curses itself and causes a strong sense of thirst."); break;
		 	case UNLUCKY_STONE:
				pline("A stone that curses itself and causes you to be shitting out of luck (SOL)."); break;
		 	case SHADES_OF_GREY_STONE:
				pline("A stone that curses itself and causes everything to display in various shades of grey."); break;
		 	case STONE_OF_FAINTING:
				pline("A stone that curses itself and causes random fainting."); break;
		 	case STONE_OF_CURSING:
				pline("A stone that curses itself and causes your inventory to fill up with cursed items."); break;
		 	case STONE_OF_DIFFICULTY:
				pline("A stone that curses itself and causes an arbitrary increase of the game's difficulty."); break;
		 	case DEAFNESS_STONE:
				pline("A stone that curses itself and causes a hearing break."); break;
		 	case ANTIMAGIC_STONE:
				pline("A stone that curses itself and causes blood mana."); break;
		 	case WEAKNESS_STONE:
				pline("A stone that curses itself and causes weakness to damage your health."); break;
 			case ROT_THIRTEEN_STONE:
				pline("A stone that curses itself and causes this message, as well as all others, to display in rot13."); break;
		 	case BISHOP_STONE:
				pline("A stone that curses itself and causes you to be unable to move diagonally."); break;
		 	case CONFUSION_STONE:
				pline("A stone that curses itself and causes a confusing problem."); break;
		 	case DROPBUG_STONE:
				pline("A stone that curses itself and causes you to be unable to drop items."); break;
 			case DSTW_STONE:
				pline("A stone that curses itself and causes potions to sometimes not work."); break;
 			case STATUS_STONE:
				pline("A stone that curses itself and causes status effects to be impossible to cure."); break;
		 	case ALIGNMENT_STONE:
				pline("A stone that curses itself and causes your maximum alignment to decrease over time."); break;
		 	case STAIRSTRAP_STONE:
				pline("A stone that curses itself and causes stairs to be always trapped."); break;
			case UNINFORMATION_STONE:
				pline("A stone that curses itself and causes insufficient amounts of information. This message should never appear on the screen because you can only see it if the stone is in your inventory, and the stone prevents this kind of message from being displayed!"); break;

			case STONE_OF_INTRINSIC_LOSS:
				pline("A stone that curses itself and causes intrinsic loss."); break;
			case BLOOD_LOSS_STONE:
				pline("A stone that curses itself and causes bleedout."); break;
			case BAD_EFFECT_STONE:
				pline("A stone that curses itself and causes bad effects."); break;
			case TRAP_CREATION_STONE:
				pline("A stone that curses itself and causes traps to be generated."); break;
			case STONE_OF_VULNERABILITY:
				pline("A stone that curses itself and causes vulnerability."); break;
			case ITEM_TELEPORTING_STONE:
				pline("A stone that curses itself and causes item teleportation."); break;
			case NASTY_STONE:
				pline("A stone that curses itself and causes nasty effects."); break;

 			default: pline("Not much is known about this type of gem, but chances are you're looking at a piece of worthless glass. They are, indeed, worthless."); break;

			}

		}
		break;

		case ROCK_CLASS:
		pline("%s - This is a boulder or statue. Color: %s. Material: %s. Appearance: %s. Boulders can be thrown and are difficult to get past if they're just lying around on the floor, while statues may be reanimated or smashed.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], obj_descr[obj->otyp].oc_name
 );
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case BOULDER: 
				pline("A large boulder that weighs a ton. It can be thrown, provided you're strong enough."); break;
			case STATUE: 
				pline("This statue depicts some sort of monster. There may be a way to make it come back to life, or you can smash it to see if it contains items."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case BALL_CLASS:
		pline("%s - This is an iron ball. Color: %s. Material: %s. Appearance: %s. You can be chained to one, in which case it will follow you around, but it can also be used as a weapon that uses the flail skill.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], obj_descr[obj->otyp].oc_name
 );
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case HEAVY_IRON_BALL: 
				pline("A heavy but damaging weapon that uses the flail skill."); break;
			case QUITE_HEAVY_IRON_BALL: 
				pline("This iron ball is heavier than a standard heavy iron ball but also does more damage. It uses the flail skill."); break;
			case REALLY_HEAVY_IRON_BALL: 
				pline("Rarely found, this flail-class weapon weighs really much but does a ton of damage."); break;
			case EXTREMELY_HEAVY_IRON_BALL: 
				pline("The single most damaging weapon in the entire game. Provided you're capable of lifting and wielding it, this extremely heavy flail-class weapon will smack the living daylights out of your enemies."); break;
			case IMPOSSIBLY_HEAVY_IRON_BALL: 
				pline("You probably won't be able to lift this ball. It seems you somehow managed to get it into your inventory, but now you're probably overburdened and cannot move unless you drop it."); break;

			case HEAVY_STONE_BALL: 
				pline("This ball is made of stone. It uses the flail skill."); break;
			case HEAVY_GLASS_BALL: 
				pline("A breakable ball of glass, which you can swing at enemies to deal good damage. This uses the flail skill."); break;
			case HEAVY_GOLD_BALL: 
				pline("With this very heavy ball, you can do great damage to monsters if you use it as a melee or thrown weapon. The flail skill is used to determine the damage and chance to hit."); break;
			case HEAVY_ELYSIUM_BALL: 
				pline("A ball made of unbreakable material. It does tons of damage on a successful hit, and it uses the flail skill."); break;

			case HEAVY_CLAY_BALL: 
				pline("A very heavy mineral ball that uses the flail skill."); break;
			case HEAVY_GRANITE_BALL: 
				pline("This ball weighs a ton and a half, and is made of stone. If you use it as a weapon, you will exercise the flail skill."); break;
			case HEAVY_CONUNDRUM_BALL: 
				pline("A ball that is nearly unbreakable and does tons of damage. It uses the flail skill."); break;
			case HEAVY_CONCRETE_BALL: 
				pline("It uses the flail skill, is made of stone and does enormous amounts of damage."); break;
			case IMPOSSIBLY_HEAVY_GLASS_BALL: 
				pline("Good luck getting rid of this thing! It's breakable, but it weighs so much that you probably can't do anything other than drop it to the floor..."); break;
			case IMPOSSIBLY_HEAVY_MINERAL_BALL: 
				pline("If you're a lithivore, you can eat this ball; otherwise you probably have to drop it due to its sheer weight."); break;
			case IMPOSSIBLY_HEAVY_ELYSIUM_BALL: 
				pline("This ball cannot be damaged in any way and will prevent you from doing anything as long as it's in your inventory."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case CHAIN_CLASS:
		pline("%s - This is an iron chain. Color: %s. Material: %s. Appearance: %s. They are lightweight flail-class weapons that can be used in melee; if you're punished, one will be created to chain you to an iron ball, but iron chains created by punishment cannot be picked up.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case IRON_CHAIN: 
				pline("A basic iron chain that doesn't do much damage."); break;
			case STONE_CHAIN: 
				pline("A basic stone chain that doesn't do much damage."); break;
			case GLASS_CHAIN: 
				pline("A basic glass chain that doesn't do much damage."); break;
			case ROTATING_CHAIN: 
				pline("This iron chain is relatively heavy but does moderate damage in melee."); break;
			case GOLD_CHAIN: 
				pline("This gold chain is relatively heavy but does moderate damage in melee."); break;
			case CLAY_CHAIN: 
				pline("This mineral chain is relatively heavy but does moderate damage in melee."); break;
			case SCOURGE: 
				pline("A powerful iron chain that also has a considerable weight."); break;
			case ELYSIUM_SCOURGE: 
				pline("A powerful unbreakable chain that also has a considerable weight."); break;
			case GRANITE_SCOURGE: 
				pline("A powerful mineral chain that also has a considerable weight."); break;
			case NUNCHIAKU: 
				pline("This is the second-strongest iron chain in the game that does quite a lot of damage while still being lighter than a heavy iron ball."); break;
			case CONUNDRUM_NUNCHIAKU: 
				pline("This is the second-strongest unbreakable chain in the game that does quite a lot of damage while still being lighter than a heavy iron ball."); break;
			case CONCRETE_NUNCHIAKU: 
				pline("This is the second-strongest mineral chain in the game that does quite a lot of damage while still being lighter than a heavy iron ball."); break;
			case HOSTAGE_CHAIN: 
				pline("An iron chain that weighs the same as a heavy iron ball, yet does more damage."); break;
			case GLASS_HOSTAGE_CHAIN: 
				pline("A glass chain that weighs the same as a heavy iron ball, yet does more damage."); break;
			case MINERAL_HOSTAGE_CHAIN: 
				pline("A mineral chain that weighs the same as a heavy iron ball, yet does more damage."); break;
			case ELYSIUM_HOSTAGE_CHAIN: 
				pline("An unbreakable chain that weighs the same as a heavy iron ball, yet does more damage."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case VENOM_CLASS:
		pline("%s - This is a splash of venom. Color: %s. Material: %s. Appearance: %s. It can be used in melee or for throwing, but either of those actions will probably use it up.",xname(obj), c_obj_colors[objects[obj->otyp].oc_color], materialnm[objects[obj->otyp].oc_material], dn);
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case BLINDING_VENOM: 
				pline("Splashing an enemy with this venom may blind them."); break;
			case TAIL_SPIKES: 
				pline("A barrage of spikes that can be used to damage enemies."); break;
			case FAERIE_FLOSS_RHING: 
				pline("If you get hit with this, you'll lose an experience level. Monsters will probably be unphased though."); break;
			case ACID_VENOM: 
				pline("Hitting a monster with this thing may deal some acid damage to it."); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		case ILLOBJ_CLASS:
		pline("%s - This is an illegal object. You shouldn't be able to get these at all.",xname(obj) );
		if (!nn) pline("Unfortunately you don't know more about it.");
		else { switch (obj->otyp) {

			case STRANGE_OBJECT: 
				pline("A strange object that actually has no business being in your inventory. Well, at least it's not a glorkum instead. :-)"); break;

 			default: pline("Object information is still a beta feature. One day, this item will also have a description. --Amy"); break;

			}

		}
		break;

		default: pline("Object information is still a beta feature. One day, this item class will also have a description. --Amy"); break;
	}

	return 0; /* a "return 1" would consume time --Amy */
}

#endif /* OVL1 */

/*invent.c*/

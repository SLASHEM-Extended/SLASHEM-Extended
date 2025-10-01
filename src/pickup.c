/*	SCCS Id: @(#)pickup.c	3.4	2003/07/27	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 *	Contains code for picking objects up, and container use.
 */

#include "hack.h"

STATIC_DCL void simple_look(struct obj *,BOOLEAN_P);
#ifndef GOLDOBJ
STATIC_DCL boolean query_classes(char *,boolean *,boolean *,
		const char *,struct obj *,BOOLEAN_P,BOOLEAN_P,int *);
#else
STATIC_DCL boolean query_classes(char *,boolean *,boolean *,
		const char *,struct obj *,BOOLEAN_P,int *);
#endif
STATIC_DCL void check_here(BOOLEAN_P);
STATIC_DCL boolean n_or_more(struct obj *);
STATIC_DCL boolean all_but_uchain(struct obj *);
#if 0 /* not used */
STATIC_DCL boolean allow_cat_no_uchain(struct obj *);
#endif
STATIC_DCL int autopick(struct obj*, int, menu_item **);
STATIC_DCL int count_categories(struct obj *,int);
STATIC_DCL long carry_count(struct obj *,struct obj *,long,BOOLEAN_P,int *,int *);
STATIC_DCL int lift_object(struct obj *,struct obj *,long *,BOOLEAN_P,BOOLEAN_P);
STATIC_PTR int in_container_(struct obj *,BOOLEAN_P);
STATIC_PTR int in_container(struct obj *);
STATIC_PTR int ck_bag(struct obj *);
STATIC_PTR int out_container(struct obj *);
STATIC_DCL void observe_quantum_cat(struct obj *);
STATIC_DCL int menu_loot(int, struct obj *, BOOLEAN_P);
STATIC_DCL int in_or_out_menu(const char *,struct obj *, BOOLEAN_P, BOOLEAN_P);
STATIC_DCL int container_at(int, int, BOOLEAN_P);
STATIC_DCL boolean able_to_loot(int, int);
STATIC_DCL boolean mon_beside(int, int);

/* define for query_objlist() and autopickup() */
#define FOLLOW(curr, flags) \
    (((flags) & BY_NEXTHERE) ? (curr)->nexthere : (curr)->nobj)

#define CEILDIV(x,y)	(((x)+(y)-1)/(y))	/* ceil(x/y) */
/*
 *  How much the weight of the given container will change when the given
 *  object is removed from it.  This calculation must match the one used
 *  by weight() in mkobj.c.
 */
#define DELTA_CWT(cont,obj)		\
    ((cont)->cursed ? (obj)->owt * ((cont)->oartifact ? 4 : 2) :	\
   CEILDIV((obj)->owt, ((cont)->oartifact ? 3 : 2) * ((cont)->blessed ? 2 : 1)))

#define HANDYBAG_CWT(cont,obj)	\
	(flags.female ? ((obj)->owt * 4 / 5) : (obj)->owt )

#define GOLD_WT(n)		(((n) + 50L) / 100L)
/* if you can figure this out, give yourself a hearty pat on the back... */
#define GOLD_CAPACITY(w,n)	(((w) * -100L) - ((n) + 50L) - 1L)

/* A variable set in use_container(), to be used by the callback routines  */
/* in_container() and out_container() from askchain() and use_container(). */
/* Also used by memu_loot() and container_gone().			   */
static NEARDATA struct obj *current_container;
#define Icebox (current_container->otyp == ICE_BOX || current_container->otyp == DISPERSION_BOX || current_container->otyp == ICE_BOX_OF_HOLDING || current_container->otyp == ICE_BOX_OF_WATERPROOFING || current_container->otyp == ICE_BOX_OF_DIGESTION)

static const char moderateloadmsg[] = "You have a little trouble lifting";
static const char nearloadmsg[] = "You have much trouble lifting";
static const char overloadmsg[] = "You have extreme difficulty lifting";

/* BUG: this lets you look at cockatrice corpses while blind without
   touching them */
/* much simpler version of the look-here code; used by query_classes() */
STATIC_OVL void
simple_look(otmp, here)
struct obj *otmp;	/* list of objects */
boolean here;		/* flag for type of obj list linkage */
{
	/* Neither of the first two cases is expected to happen, since
	 * we're only called after multiple classes of objects have been
	 * detected, hence multiple objects must be present.
	 */

	if (InventoryDoesNotGo && !program_state.gameover) {
		pline("Not enough memory to create inventory window");
 		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}	

	if (!otmp) {
	    impossible("simple_look(null)");
	} else if (!(here ? otmp->nexthere : otmp->nobj)) {
	    pline("%s", doname(otmp));
	} else {
	    winid tmpwin = create_nhwindow(NHW_MENU);
	    putstr(tmpwin, 0, "");
	    do {
		putstr(tmpwin, 0, doname(otmp));
		otmp = here ? otmp->nexthere : otmp->nobj;
	    } while (otmp);
	    display_nhwindow(tmpwin, TRUE);
	    destroy_nhwindow(tmpwin);
	}
}

#ifndef GOLDOBJ
int
collect_obj_classes(ilets, otmp, here, incl_gold, filter, itemcount)
char ilets[];
register struct obj *otmp;
boolean here, incl_gold;
boolean (*filter)(OBJ_P);
int *itemcount;
#else
int
collect_obj_classes(ilets, otmp, here, filter, itemcount)
char ilets[];
register struct obj *otmp;
boolean here;
boolean (*filter)(OBJ_P);
int *itemcount;
#endif
{
	register int iletct = 0;
	register char c;

	*itemcount = 0;
#ifndef GOLDOBJ
	if (incl_gold)
	    ilets[iletct++] = def_oc_syms[COIN_CLASS];
#endif
	ilets[iletct] = '\0'; /* terminate ilets so that index() will work */
	while (otmp) {
	    c = def_oc_syms[(int)otmp->oclass];
	    if (!index(ilets, c) && (!filter || (*filter)(otmp)))
		ilets[iletct++] = c,  ilets[iletct] = '\0';
	    *itemcount += 1;
	    otmp = here ? otmp->nexthere : otmp->nobj;
	}

	return iletct;
}

/*
 * Suppose some '?' and '!' objects are present, but '/' objects aren't:
 *	"a" picks all items without further prompting;
 *	"A" steps through all items, asking one by one;
 *	"?" steps through '?' items, asking, and ignores '!' ones;
 *	"/" becomes 'A', since no '/' present;
 *	"?a" or "a?" picks all '?' without further prompting;
 *	"/a" or "a/" becomes 'A' since there aren't any '/'
 *	    (bug fix:  3.1.0 thru 3.1.3 treated it as "a");
 *	"?/a" or "a?/" or "/a?",&c picks all '?' even though no '/'
 *	    (ie, treated as if it had just been "?a").
 */
#ifndef GOLDOBJ
STATIC_OVL boolean
query_classes(oclasses, one_at_a_time, everything, action, objs,
	      here, incl_gold, menu_on_demand)
char oclasses[];
boolean *one_at_a_time, *everything;
const char *action;
struct obj *objs;
boolean here, incl_gold;
int *menu_on_demand;
#else
STATIC_OVL boolean
query_classes(oclasses, one_at_a_time, everything, action, objs,
	      here, menu_on_demand)
char oclasses[];
boolean *one_at_a_time, *everything;
const char *action;
struct obj *objs;
boolean here;
int *menu_on_demand;
#endif
{
	char ilets[20], inbuf[BUFSZ];
	int iletct, oclassct;
	boolean not_everything;
	char qbuf[QBUFSZ];
	boolean m_seen;
	int itemcount;

	oclasses[oclassct = 0] = '\0';
	*one_at_a_time = *everything = m_seen = FALSE;
	iletct = collect_obj_classes(ilets, objs, here,
#ifndef GOLDOBJ
				     incl_gold,
#endif
				     (boolean (*)(OBJ_P)) 0, &itemcount);
	if (iletct == 0) {
		return FALSE;
	} else if (iletct == 1) {
		oclasses[0] = def_char_to_objclass(ilets[0]);
		oclasses[1] = '\0';
		if (itemcount && menu_on_demand) {
			ilets[iletct++] = 'm';
			*menu_on_demand = 0;
			ilets[iletct] = '\0';
		}
	} else  {	/* more than one choice available */
		const char *where = 0;
		register char sym, oc_of_sym, *p;
		/* additional choices */
		ilets[iletct++] = ' ';
		ilets[iletct++] = 'a';
		ilets[iletct++] = 'A';
		ilets[iletct++] = (objs == invent ? 'i' : ':');
		if (menu_on_demand) {
			ilets[iletct++] = 'm';
			*menu_on_demand = 0;
		}
		ilets[iletct] = '\0';
ask_again:
		oclasses[oclassct = 0] = '\0';
		*one_at_a_time = *everything = FALSE;
		not_everything = FALSE;
		sprintf(qbuf,"What kinds of thing do you want to %s? [%s]",
			action, ilets);
		getlin(qbuf,inbuf);
		if (*inbuf == '\033') return FALSE;

		for (p = inbuf; (sym = *p++); ) {
		    /* new A function (selective all) added by GAN 01/09/87 */
		    if (sym == ' ') continue;
		    else if (sym == 'A') *one_at_a_time = TRUE;
		    else if (sym == 'a') {
			if (yn("You decided to autoselect everything, please confirm with y if that's what you really wanted") == 'y') *everything = TRUE;
		    } else if (sym == ':') {
			simple_look(objs, here);  /* dumb if objs==invent */
			goto ask_again;
		    } else if (sym == 'i') {
			(void) display_inventory((char *)0, TRUE);
			goto ask_again;
		    } else if (sym == 'm') {
			m_seen = TRUE;
		    } else {
			oc_of_sym = def_char_to_objclass(sym);
			if (index(ilets,sym)) {
			    add_valid_menu_class(oc_of_sym);
			    oclasses[oclassct++] = oc_of_sym;
			    oclasses[oclassct] = '\0';
			} else {
			    if (!where)
				where = !strcmp(action,"pick up")  ? "here" :
					!strcmp(action,"take out") ?
							    "inside" : "";
			    if (*where)
				There("are no %c's %s.", sym, where);
			    else
				You("have no %c's.", sym);
			    not_everything = TRUE;
			}
		    }
		}
		if (m_seen && menu_on_demand) {
			*menu_on_demand = (*everything || !oclassct) ? -2 : -3;
			return FALSE;
		}
		if (!oclassct && (!*everything || not_everything)) {
		    /* didn't pick anything,
		       or tried to pick something that's not present */
		    *one_at_a_time = TRUE;	/* force 'A' */
		    *everything = FALSE;	/* inhibit 'a' */
		}
	}
	return TRUE;
}

/* look at the objects at our location, unless there are too many of them */
STATIC_OVL void
check_here(picked_some)
boolean picked_some;
{
	register struct obj *obj;
	register int ct = 0;

	/* count the objects here */
	for (obj = level.objects[u.ux][u.uy]; obj; obj = obj->nexthere) {
	    if ((obj != uchain) && !obj->dynamitekaboom && !obj->mstartinventX)
		ct++;
	}

	/* If there are objects here, take a look. */
	if (ct) {
	    if (flags.run) nomul(0, 0, FALSE);
	    flush_screen(1);
	    (void) look_here(ct, picked_some);
	} else {
	    sense_engr_at(u.ux, u.uy, FALSE);
	}
}

/* Value set by query_objlist() for n_or_more(). */
static long val_for_n_or_more;

/* query_objlist callback: return TRUE if obj's count is >= reference value */
STATIC_OVL boolean
n_or_more(obj)
struct obj *obj;
{
    if (obj == uchain) return FALSE;
    if (obj->dynamitekaboom) return FALSE;
    if (obj->mstartinventX) return FALSE;
    return (obj->quan >= val_for_n_or_more);
}

/* List of valid menu classes for query_objlist() and allow_category callback */
static char valid_menu_classes[MAXOCLASSES + 2];

void
add_valid_menu_class(c)
int c;
{
	static int vmc_count = 0;

	if (c == 0)  /* reset */
	  vmc_count = 0;
	else
	  valid_menu_classes[vmc_count++] = (char)c;
	valid_menu_classes[vmc_count] = '\0';
}

/* query_objlist callback: return TRUE if not uchain */
STATIC_OVL boolean
all_but_uchain(obj)
struct obj *obj;
{
    return ((obj != uchain) && !obj->dynamitekaboom && !obj->mstartinventX);
}

/* query_objlist callback: return TRUE */
/*ARGSUSED*/
boolean
allow_all(obj)
struct obj *obj;
{
    return TRUE;
}

boolean
allow_category(obj)
struct obj *obj;
{
    if (Role_if(PM_PRIEST)) obj->bknown = TRUE;
    if (((index(valid_menu_classes,'u') != (char *)0) && obj->unpaid && !Hallucination ) ||
	(index(valid_menu_classes, obj->oclass) != (char *)0))
	return TRUE;
    else if (((index(valid_menu_classes,'I') != (char *)0) && not_fully_identified(obj) && !Hallucination) ||
	(index(valid_menu_classes, obj->oclass) != (char *)0))
	return TRUE;
    else if (((index(valid_menu_classes,'U') != (char *)0) &&
	(obj->oclass != COIN_CLASS && obj->bknown && !obj->blessed && !obj->cursed && !Hallucination)))
	return TRUE;
    else if (((index(valid_menu_classes,'B') != (char *)0) &&
	(obj->oclass != COIN_CLASS && obj->bknown && obj->blessed && !Hallucination)))
	return TRUE;
    else if (((index(valid_menu_classes,'C') != (char *)0) &&
	(obj->oclass != COIN_CLASS && obj->bknown && obj->cursed && !Hallucination)))
	return TRUE;
    else if (((index(valid_menu_classes,'X') != (char *)0) &&
	(obj->oclass != COIN_CLASS && !obj->bknown && !Hallucination)))
	return TRUE;
    else
	return FALSE;
}

#if 0 /* not used */
/* query_objlist callback: return TRUE if valid category (class), no uchain */
STATIC_OVL boolean
allow_cat_no_uchain(obj)
struct obj *obj;
{
    if ((obj != uchain) &&
	(((index(valid_menu_classes,'u') != (char *)0) && obj->unpaid) ||
	(index(valid_menu_classes, obj->oclass) != (char *)0)))
	return TRUE;
    else
	return FALSE;
}
#endif

/* query_objlist callback: return TRUE if valid class and worn */
boolean
is_worn_by_type(otmp)
register struct obj *otmp;
{
	return((boolean)(!!(otmp->owornmask &
			(W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL | W_WEP | W_SWAPWEP | W_QUIVER)))
	        && (index(valid_menu_classes, otmp->oclass) != (char *)0));
}

/*
 * Have the hero pick things from the ground
 * or a monster's inventory if swallowed.
 *
 * Arg what:
 *	>0  autopickup
 *	=0  interactive
 *	<0  pickup count of something
 *
 * Returns 1 if tried to pick something up, whether
 * or not it succeeded.
 */
int
pickup(what)
int what;		/* should be a long */
{
	int i, n, res, count, n_tried = 0, n_picked = 0;
	menu_item *pick_list = (menu_item *) 0;
	boolean autopickup = what > 0;
	struct obj *objchain;
	int traverse_how;

	if (what < 0)		/* pick N of something */
	    count = -what;
	else			/* pick anything */
	    count = 0;

	if (!u.uswallow) {
		struct trap *ttmp = t_at(u.ux, u.uy);
		/* no auto-pick if no-pick move, nothing there, or in a pool */
		if (autopickup && (flags.nopick || !OBJ_AT(u.ux, u.uy) ||
			(is_waterypool(u.ux, u.uy) && !is_crystalwater(u.ux, u.uy) && !Underwater) || (is_watertunnel(u.ux, u.uy) && !Underwater) || is_lava(u.ux, u.uy))) {
			sense_engr_at(u.ux, u.uy, FALSE);
			return (0);
		}

		/* no pickup if levitating & not on air or water level */
		if (!can_reach_floor()) {
		    if ((multi && !flags.run) || (autopickup && !flags.pickup))
			sense_engr_at(u.ux, u.uy, FALSE);
		    return (0);
		}
		if (ttmp && ttmp->tseen) {
		    /* Allow pickup from holes and trap doors that you escaped
		     * from because that stuff is teetering on the edge just
		     * like you, but not pits, because there is an elevation
		     * discrepancy with stuff in pits.
		     */
		    if ((ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT || ttmp->ttyp == GIANT_CHASM || ttmp->ttyp == SHIT_PIT || ttmp->ttyp == MANA_PIT || ttmp->ttyp == ANOXIC_PIT || ttmp->ttyp == HYPOXIC_PIT || ttmp->ttyp == ACID_PIT) &&
			(!u.utrap || (u.utrap && u.utraptype != TT_PIT)) && !Passes_walls && !Flying) {
			sense_engr_at(u.ux, u.uy, FALSE);
			return(0);
		    }
		}
		/* multi && !flags.run means they are in the middle of some other
		 * action, or possibly paralyzed, sleeping, etc.... and they just
		 * teleported onto the object.  They shouldn't pick it up.
		 */
		if ((multi && !flags.run) || (autopickup && !flags.pickup)) {
		    check_here(FALSE);
		    return (0);
		}
		if (notake(youmonst.data) && !(uarmg && itemhasappearance(uarmg, APP_HOOKED_GLOVES)) && !Race_if(PM_TRANSFORMER) ) {
		    if (!autopickup) {
			You("are physically incapable of picking anything up.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

			if (yn("But maybe you can reach the items anyway. Try it? Warning: this can fail and cause hallucination.") == 'y') {
				if (rn2(3) && !polyskillchance()) {
				 	make_hallucinated(HHallucination + rnd(50),FALSE,0L);
					pline("Oh wow! Is that your own shiny reflection you just saw?");
					if (!rn2(20)) badeffect();
					if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
					return 1;
				}
			}
			else {return(0);}


			}
		    else
			{ check_here(FALSE);
		    return (0); }
		}

		/* if there's anything here, stop running */
		if (OBJ_AT(u.ux,u.uy) && flags.run && flags.run != 8 && !flags.nopick) nomul(0, 0, FALSE);
	}

	add_valid_menu_class(0);	/* reset */
	if (!u.uswallow) {
		objchain = level.objects[u.ux][u.uy];
		traverse_how = BY_NEXTHERE;
	} else {pline(FunnyHallu ? "There's something embedded here, but you can't dislodge it..." : "You can't take items out of a monster's interior!"); 
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return (0); /* otherwise the player could snatch worn amulets of life saving or similar stuff! --Amy */

	}			/*{
		objchain = u.ustuck->minvent;
		traverse_how = 0;*/	/* nobj */
	/*}*/
	/*
	 * Start the actual pickup process.  This is split into two main
	 * sections, the newer menu and the older "traditional" methods.
	 * Automatic pickup has been split into its own menu-style routine
	 * to make things less confusing.
	 */
	if (autopickup) {
	    n = autopick(objchain, traverse_how, &pick_list);
	    goto menu_pickup;
	}

	if ((flags.menu_style != MENU_TRADITIONAL && !InventoryDoesNotGo) || iflags.menu_requested) {

	    /* use menus exclusively */
	    if (count) {	/* looking for N of something */
		char buf[QBUFSZ];
		sprintf(buf, "Pick %d of what?", count);
		val_for_n_or_more = count;	/* set up callback selector */
		n = query_objlist(buf, objchain,
			    traverse_how|AUTOSELECT_SINGLE|INVORDER_SORT,
			    &pick_list, PICK_ONE, n_or_more);
		/* correct counts, if any given */
		for (i = 0; i < n; i++)
		    pick_list[i].count = count;
	    } else {
		n = query_objlist("Pick up what?", objchain,
			traverse_how|AUTOSELECT_SINGLE|INVORDER_SORT|FEEL_COCKATRICE,
			&pick_list, PICK_ANY, all_but_uchain);
	    }
menu_pickup:
	    n_tried = n;
	    for (n_picked = i = 0 ; i < n; i++) {
		res = pickup_object(pick_list[i].item.a_obj,pick_list[i].count,
					FALSE, FALSE);
		if (res < 0) break;	/* can't continue */
		n_picked += res;
	    }
	    if (pick_list) free((void *)pick_list);

	} else {
	    /* old style interface */
	    int ct = 0;
	    long lcount;
	    boolean all_of_a_type, selective;
	    char oclasses[MAXOCLASSES];
	    struct obj *obj, *obj2;

	    oclasses[0] = '\0';		/* types to consider (empty for all) */
	    all_of_a_type = TRUE;	/* take all of considered types */
	    selective = FALSE;		/* ask for each item */

	    /* check for more than one object */
	    for (obj = objchain;
		  obj; obj = (traverse_how == BY_NEXTHERE) ? obj->nexthere : obj->nobj)
		ct++;

	    if (ct == 1 && count) {
		/* if only one thing, then pick it */
		obj = objchain;
		lcount = min(obj->quan, (long)count);
		n_tried++;
		if (pickup_object(obj, lcount, FALSE, FALSE) > 0)
		    n_picked++;	/* picked something */
		goto end_query;

	    } else if (ct >= 2) {
		int via_menu = 0;

		There("are %s objects here.",
		      (ct <= 10) ? "several" : "many");
		if (!query_classes(oclasses, &selective, &all_of_a_type,
				   "pick up", objchain,
				   traverse_how == BY_NEXTHERE,
#ifndef GOLDOBJ
				   FALSE,
#endif
				   &via_menu)) {
		    if (!via_menu) return (0);
		    n = query_objlist("Pick up what?",
				  objchain,
				  traverse_how|(selective ? 0 : INVORDER_SORT),
				  &pick_list, PICK_ANY,
				  via_menu == -2 ? allow_all : allow_category);
		    goto menu_pickup;
		}
	    }

	    for (obj = objchain; obj; obj = obj2) {
		if (traverse_how == BY_NEXTHERE)
			obj2 = obj->nexthere;	/* perhaps obj will be picked up */
		else
			obj2 = obj->nobj;
		lcount = -1L;

		if (!selective && oclasses[0] && !index(oclasses,obj->oclass))
		    continue;

		if (!all_of_a_type) {
		    char qbuf[BUFSZ];
		    sprintf(qbuf, "Pick up %s?",
			safe_qbuf("", sizeof("Pick up ?"), doname(obj),
					an(simple_typename(obj->otyp)), "something"));
		    switch ((obj->quan < 2L) ? ynaq(qbuf) : ynNaq(qbuf)) {
		    case 'q': goto end_query;	/* out 2 levels */
		    case 'n': continue;
		    case 'a':
			all_of_a_type = TRUE;
			if (selective) {
			    selective = FALSE;
			    oclasses[0] = obj->oclass;
			    oclasses[1] = '\0';
			}
			break;
		    case '#':	/* count was entered */
			if (!yn_number) continue; /* 0 count => No */
			lcount = (long) yn_number;
			if (lcount > obj->quan) lcount = obj->quan;
			/* fall thru */
		    default:	/* 'y' */
			break;
		    }
		}
		if (lcount == -1L) lcount = obj->quan;

		n_tried++;
		if ((res = pickup_object(obj, lcount, FALSE, FALSE)) < 0) break;
		n_picked += res;
	    }
end_query:
	    ;	/* semicolon needed by brain-damaged compilers */
	}

	if (!u.uswallow) {
		if (!OBJ_AT(u.ux,u.uy)) u.uundetected = 0;

		/* position may need updating (invisible hero) */
		if (n_picked) newsym(u.ux,u.uy);

		/* see whether there's anything else here, after auto-pickup is done */
		if (autopickup) check_here(n_picked > 0);
	}

	/* Picking up stuff no longer consumes turns. --Amy */
	/* See comment in do.c about soviet mode */
	/* message is only given if you actually tried to pick something up */

	if (issoviet) {
		if (!rn2(10) && n_tried) pline("Eto zanimayet ochered' potomu, chto sovetskiy khochet, chtoby igra byla der'mo.");
		return (n_tried > 0);
		/* I considered making it "return 1" just to spite the player, but decided to be lenient. */
	}
	return /*(n_tried > */0/*)*/;
}

#ifdef AUTOPICKUP_EXCEPTIONS
boolean
is_autopickup_exception(obj, grab)
struct obj *obj;
boolean grab;	 /* forced pickup, rather than forced leave behind? */
{
	/*
	 *  Does the text description of this match an exception?
	 */
	char *objdesc = makesingular(doname(obj));
	struct autopickup_exception *ape = (grab) ?
					iflags.autopickup_exceptions[AP_GRAB] :
					iflags.autopickup_exceptions[AP_LEAVE];
	while (ape) {
		if (pmatch(ape->pattern, objdesc)) return TRUE;
		ape = ape->next;
	}
	return FALSE;
}
#endif /* AUTOPICKUP_EXCEPTIONS */

/*
 * Pick from the given list using flags.pickup_types.  Return the number
 * of items picked (not counts).  Create an array that returns pointers
 * and counts of the items to be picked up.  If the number of items
 * picked is zero, the pickup list is left alone.  The caller of this
 * function must free the pickup list.
 */
STATIC_OVL int
autopick(olist, follow, pick_list)
struct obj *olist;	/* the object list */
int follow;		/* how to follow the object list */
menu_item **pick_list;	/* list of objects and counts to pick up */
{
	menu_item *pi;	/* pick item */
	struct obj *curr;
	int n;
	const char *otypes = flags.pickup_types;

	/* first count the number of eligible items */
	for (n = 0, curr = olist; curr; curr = FOLLOW(curr, follow)) {

		if (AlwaysAutopickup || u.uprops[AUTOPICKUP_ALWAYS].extrinsic || have_autopickupstone()) n++;
		else {

#ifndef AUTOPICKUP_EXCEPTIONS
	    if (/*!*otypes || */index(otypes, curr->oclass) ||
		(flags.pickup_thrown && curr->was_thrown && !(curr->cursed && curr->bknown && !flags.pickup_cursed && !Hallucination && !(PlayerUninformation) ) ) )
#else
	    if ((/*!*otypes || */index(otypes, curr->oclass) ||
		(flags.pickup_thrown && curr->was_thrown && !(curr->cursed && curr->bknown && !flags.pickup_cursed && !Hallucination && !(PlayerUninformation) ) ) ||
		is_autopickup_exception(curr, TRUE)) &&
		!is_autopickup_exception(curr, FALSE))
#endif
		n++;
		}
	}

	if (n) {
	    *pick_list = pi = (menu_item *) alloc(sizeof(menu_item) * n);
	    for (n = 0, curr = olist; curr; curr = FOLLOW(curr, follow)) {

			if (AlwaysAutopickup || u.uprops[AUTOPICKUP_ALWAYS].extrinsic || have_autopickupstone()) {
				pi[n].item.a_obj = curr;
				pi[n].count = curr->quan;
				n++;

			} else {

#ifndef AUTOPICKUP_EXCEPTIONS
				if (/*!*otypes || */index(otypes, curr->oclass) ||
					(flags.pickup_thrown && curr->was_thrown && !(curr->cursed && curr->bknown && !flags.pickup_cursed && !Hallucination && !(PlayerUninformation) ) ) ) {
#else
				if ((/*!*otypes || */index(otypes, curr->oclass) ||
					(flags.pickup_thrown && curr->was_thrown && !(curr->cursed && curr->bknown && !flags.pickup_cursed && !Hallucination && !(PlayerUninformation) ) ) ||
					is_autopickup_exception(curr, TRUE)) && !is_autopickup_exception(curr, FALSE)) {
#endif
				    pi[n].item.a_obj = curr;
				    pi[n].count = curr->quan;
				    n++;
				}

			}

		}
	}
	return n;
}


/*
 * Put up a menu using the given object list.  Only those objects on the
 * list that meet the approval of the allow function are displayed.  Return
 * a count of the number of items selected, as well as an allocated array of
 * menu_items, containing pointers to the objects selected and counts.  The
 * returned counts are guaranteed to be in bounds and non-zero.
 *
 * Query flags:
 *	BY_NEXTHERE	  - Follow object list via nexthere instead of nobj.
 *	AUTOSELECT_SINGLE - Don't ask if only 1 object qualifies - just
 *			    use it.
 *	USE_INVLET	  - Use object's invlet.
 *	INVORDER_SORT	  - Use hero's pack order.
 *	SIGNAL_NOMENU	  - Return -1 rather than 0 if nothing passes "allow".
 *	SIGNAL_CANCEL	  - Return -2 rather than 0 if player cancels.
 */
int
query_objlist(qstr, olist, qflags, pick_list, how, allow)
const char *qstr;		/* query string */
struct obj *olist;		/* the list to pick from */
int qflags;			/* options to control the query */
menu_item **pick_list;		/* return list of items picked */
int how;			/* type of query */
boolean (*allow)(OBJ_P);/* allow function */
{
	int i, j;
	int n;
	winid win;
	struct obj *curr, *last;
	struct obj **oarray;
	char *pack;
	anything any;
	boolean printed_type_name;

	if (InventoryDoesNotGo && !program_state.gameover) {pline("Not enough memory to create inventory window");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}	

	*pick_list = (menu_item *) 0;
	if (!olist) return 0;

	/* count the number of items allowed */
	for (n = 0, last = 0, curr = olist; curr; curr = FOLLOW(curr, qflags)) {

	    if ((*allow)(curr)) {
		last = curr;
		n++;
	    }
	}

	if (n == 0)	/* nothing to pick here */
	    return (qflags & SIGNAL_NOMENU) ? -1 : 0;

	if (n == 1 && (qflags & AUTOSELECT_SINGLE)) {
	    *pick_list = (menu_item *) alloc(sizeof(menu_item));
	    (*pick_list)->item.a_obj = last;
	    (*pick_list)->count = last->quan;
	    return 1;
	}

	/* Make a temporary array to store the objects sorted */
	oarray = (struct obj **)alloc(n*sizeof(struct obj*));

	/* Add objects to the array */
	i = 0;
	for (curr = olist; curr; curr = FOLLOW(curr, qflags)) {
	  if ((*allow)(curr)) {
	    if ((iflags.sortloot == 'f' ||
	        (iflags.sortloot == 'l' && !(qflags & USE_INVLET))) && !(Hallucination || PlayerUninformation))
	      {
	        /* Insert object at correct index */
	        for (j = i; j; j--)
	          {
	            if (strcmpi(cxname3(curr), cxname3(oarray[j-1]))>0) break;
	            oarray[j] = oarray[j-1];
	          }
	        oarray[j] = curr;
	        i++;
	      } else {
	        /* Just add it to the array */
	        oarray[i++] = curr;
	      }
	  }
	}

	win = create_nhwindow(NHW_MENU);
	start_menu(win);
	any.a_obj = (struct obj *) 0;

	/*
	 * Run through the list and add the objects to the menu.  If
	 * INVORDER_SORT is set, we'll run through the list once for
	 * each type so we can group them.  The allow function will only
	 * be called once per object in the list.
	 */
	pack = flags.inv_order;
	do {
	    printed_type_name = FALSE;
	    /*for (curr = olist; curr; curr = FOLLOW(curr, qflags)) {*/
	    for (i = 0; i < n; i++) {
		curr = oarray[i];

		if ((qflags & FEEL_COCKATRICE) && (curr->otyp == CORPSE || curr->otyp == EGG) &&
		     will_feel_cockatrice(curr, FALSE)) {
			destroy_nhwindow(win);	/* stop the menu and revert */
			(void) look_here(0, FALSE);
			return 0;
		}
		if ((!(qflags & INVORDER_SORT) || curr->oclass == *pack)
							&& (*allow)(curr)) {

		    /* if sorting, print type name (once only) */
		    if (qflags & INVORDER_SORT && !printed_type_name) {
			any.a_obj = (struct obj *) 0;
			add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
					let_to_name(*pack, FALSE, iflags.showsym), MENU_UNSELECTED);
			printed_type_name = TRUE;
		    }

		    any.a_obj = curr;
		    add_menu(win, obj_to_glyph(curr), &any,
			    qflags & USE_INVLET ? curr->invlet : 0,
			    def_oc_syms[(int)objects[curr->otyp].oc_class],
			    ATR_NONE, doname(curr), MENU_UNSELECTED);
		}
	    }
	    pack++;
	} while (qflags & INVORDER_SORT && *pack);

	free(oarray);
	end_menu(win, qstr);
	n = select_menu(win, how, pick_list);
	destroy_nhwindow(win);

	if (n > 0) {
	    menu_item *mi;
	    int i;

	    /* fix up counts:  -1 means no count used => pick all */
	    for (i = 0, mi = *pick_list; i < n; i++, mi++)
		if (mi->count == -1L || mi->count > mi->item.a_obj->quan)
		    mi->count = mi->item.a_obj->quan;
	} else if (n < 0) {
	    /* caller's don't expect -1 */
	    n = (qflags & SIGNAL_CANCEL) ? -2 : 0;
	}
	return n;
}

/*
 * allow menu-based category (class) selection (for Drop,take off etc.)
 *
 */
int
query_category(qstr, olist, qflags, pick_list, how)
const char *qstr;		/* query string */
struct obj *olist;		/* the list to pick from */
int qflags;			/* behaviour modification flags */
menu_item **pick_list;		/* return list of items picked */
int how;			/* type of query */
{
	int n;
	winid win;
	struct obj *curr;
	char *pack;
	anything any;
	boolean collected_type_name;
	char invlet;
	int ccount;
	boolean do_unpaid = FALSE;
	boolean do_unided = FALSE;
	boolean do_blessed = FALSE, do_cursed = FALSE, do_uncursed = FALSE,
	    do_buc_unknown = FALSE;
	int num_buc_types = 0;

	*pick_list = (menu_item *) 0;
	if (!olist) return 0;
	if ((qflags & UNPAID_TYPES) && count_unpaid(olist) && !Hallucination) do_unpaid = TRUE;
	if ((qflags & NOTFULLYIDED) && count_notfullyided(olist) && !Hallucination) do_unided = TRUE;
	if ((qflags & BUC_BLESSED) && count_buc(olist, BUC_BLESSED) && !Hallucination && !(PlayerUninformation) ) {
	    do_blessed = TRUE;
	    num_buc_types++;
	}
	if ((qflags & BUC_CURSED) && count_buc(olist, BUC_CURSED) && !Hallucination && !(PlayerUninformation) ) {
	    do_cursed = TRUE;
	    num_buc_types++;
	}
	if ((qflags & BUC_UNCURSED) && count_buc(olist, BUC_UNCURSED) && !Hallucination && !(PlayerUninformation) ) {
	    do_uncursed = TRUE;
	    num_buc_types++;
	}
	if ((qflags & BUC_UNKNOWN) && count_buc(olist, BUC_UNKNOWN) && !Hallucination && !(PlayerUninformation) ) {
	    do_buc_unknown = TRUE;
	    num_buc_types++;
	}

	ccount = count_categories(olist, qflags);
	/* no point in actually showing a menu for a single category */
	if (ccount == 1 && !do_unpaid && num_buc_types <= 1 && !(qflags & BILLED_TYPES)) {
	    for (curr = olist; curr; curr = FOLLOW(curr, qflags)) {
		if ((qflags & WORN_TYPES) &&
		    !(curr->owornmask & (W_ARMOR|W_RING|W_AMUL|W_IMPLANT|W_TOOL|W_WEP|W_SWAPWEP|W_QUIVER)))
		    continue;
		break;
	    }
	    if (curr) {
		*pick_list = (menu_item *) alloc(sizeof(menu_item));
		(*pick_list)->item.a_int = curr->oclass;
		return 1;
	    } else {
#ifdef DEBUG
		impossible("query_category: no single object match");
#endif
	    }
	    return 0;
	}

	win = create_nhwindow(NHW_MENU);
	start_menu(win);
	pack = flags.inv_order;
	if ((qflags & ALL_TYPES) && (ccount > 1)) {
		invlet = 'a';
		any.a_void = 0;
		any.a_int = ALL_TYPES_SELECTED;
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
		       (qflags & WORN_TYPES) ? "All worn types" : "All types",
			MENU_UNSELECTED);
		invlet = 'b';
	} else
		invlet = 'a';
	do {
	    collected_type_name = FALSE;
	    for (curr = olist; curr; curr = FOLLOW(curr, qflags)) {
		if (curr->oclass == *pack) {
		   if ((qflags & WORN_TYPES) &&
		   		!(curr->owornmask & (W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL |
		    	W_WEP | W_SWAPWEP | W_QUIVER)))
			 continue;
		   if (!collected_type_name) {
			any.a_void = 0;
			any.a_int = curr->oclass;
			add_menu(win, NO_GLYPH, &any, invlet++,
				def_oc_syms[(int)objects[curr->otyp].oc_class],
				ATR_NONE, let_to_name(*pack, FALSE, iflags.showsym),
				MENU_UNSELECTED);
			collected_type_name = TRUE;
		   }
		}
	    }
	    pack++;
	    if (invlet >= 'u') {
		impossible("query_category: too many categories");
		return 0;
	    }
	} while (*pack);

	/* unidentified items */
	if (do_unided) {
		invlet = 'I';
		any.a_void = 0;
		any.a_int = 'I';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Unidentified items", MENU_UNSELECTED);
	}
	/* unpaid items if there are any */
	if (do_unpaid) {
		invlet = 'u';
		any.a_void = 0;
		any.a_int = 'u';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Unpaid items", MENU_UNSELECTED);
	}
	/* billed items: checked by caller, so always include if BILLED_TYPES */
	if (qflags & BILLED_TYPES) {
		invlet = 'x';
		any.a_void = 0;
		any.a_int = 'x';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			 "Unpaid items already used up", MENU_UNSELECTED);
	}
	if (qflags & CHOOSE_ALL) {
		invlet = 'A';
		any.a_void = 0;
		any.a_int = 'A';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			(qflags & WORN_TYPES) ?
			"Auto-select every item being worn" :
			"Auto-select every item", MENU_UNSELECTED);
	}
	/* items with b/u/c/unknown if there are any */
	if (do_blessed) {
		invlet = 'B';
		any.a_void = 0;
		any.a_int = 'B';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Items known to be Blessed", MENU_UNSELECTED);
	}
	if (do_cursed) {
		invlet = 'C';
		any.a_void = 0;
		any.a_int = 'C';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Items known to be Cursed", MENU_UNSELECTED);
	}
	if (do_uncursed) {
		invlet = 'U';
		any.a_void = 0;
		any.a_int = 'U';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Items known to be Uncursed", MENU_UNSELECTED);
	}
	if (do_buc_unknown) {
		invlet = 'X';
		any.a_void = 0;
		any.a_int = 'X';
		add_menu(win, NO_GLYPH, &any, invlet, 0, ATR_NONE,
			"Items of unknown B/C/U status",
			MENU_UNSELECTED);
	}
	end_menu(win, qstr);
	n = select_menu(win, how, pick_list);
	destroy_nhwindow(win);
	if (n < 0)
	    n = 0;	/* caller's don't expect -1 */
	return n;
}

STATIC_OVL int
count_categories(olist, qflags)
struct obj *olist;
int qflags;
{
	char *pack;
	boolean counted_category;
	int ccount = 0;
	struct obj *curr;

	pack = flags.inv_order;
	do {
	    counted_category = FALSE;
	    for (curr = olist; curr; curr = FOLLOW(curr, qflags)) {
		if (curr->oclass == *pack) {
		   if ((qflags & WORN_TYPES) &&
		    	!(curr->owornmask & (W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL |
		    	W_WEP | W_SWAPWEP | W_QUIVER)))
			 continue;
		   if (!counted_category) {
			ccount++;
			counted_category = TRUE;
		   }
		}
	    }
	    pack++;
	} while (*pack);
	return ccount;
}

/* could we carry `obj'? if not, could we carry some of it/them? */
STATIC_OVL long
carry_count(obj, container, count, telekinesis, wt_before, wt_after)
struct obj *obj, *container;	/* object to pick up, bag it's coming out of */
long count;
boolean telekinesis;
int *wt_before, *wt_after;
{
    boolean adjust_wt = container && carried(container),
	    is_gold = obj->oclass == COIN_CLASS;
    int wt, iw, ow, oow;
    long qq, savequan;
#ifdef GOLDOBJ
    long umoney = money_cnt(invent);
#endif
    unsigned saveowt;
    const char *verb, *prefx1, *prefx2, *suffx;
    char obj_nambuf[BUFSZ], where[BUFSZ];

    savequan = obj->quan;
    saveowt = obj->owt;

    iw = max_capacity();

    if (count != savequan) {
	obj->quan = count;
	obj->owt = (unsigned)weight(obj);
    }
    wt = iw + (int)obj->owt;
    if (adjust_wt)
	wt -= (container->otyp == BAG_OF_HOLDING || container->otyp == ICE_BOX_OF_HOLDING || container->otyp == CHEST_OF_HOLDING || container->oartifact == ART_SACK_OF_HOLDING) ?
		(int)DELTA_CWT(container, obj) : (container->otyp == HANDYBAG) ? (int)HANDYBAG_CWT(container, obj) : (int)obj->owt;
#ifndef GOLDOBJ
    if (is_gold)	/* merged gold might affect cumulative weight */
	wt -= (GOLD_WT(u.ugold) + GOLD_WT(count) - GOLD_WT(u.ugold + count));
#else
    /* This will go with silver+copper & new gold weight */
    if (is_gold)	/* merged gold might affect cumulative weight */
	wt -= (GOLD_WT(umoney) + GOLD_WT(count) - GOLD_WT(umoney + count));
#endif
    if (count != savequan) {
	obj->quan = savequan;
	obj->owt = saveowt;
    }
    *wt_before = iw;
    *wt_after  = wt;

    if (wt < 0)
	return count;

    /* see how many we can lift */
    if (is_gold) {
#ifndef GOLDOBJ
	iw -= (int)GOLD_WT(u.ugold);
	if (!adjust_wt) {
	    qq = GOLD_CAPACITY((long)iw, u.ugold);
	} else {
	    oow = 0;
	    qq = 50L - (u.ugold % 100L) - 1L;
#else
	iw -= (int)GOLD_WT(umoney);
	if (!adjust_wt) {
	    qq = GOLD_CAPACITY((long)iw, umoney);
	} else {
	    oow = 0;
	    qq = 50L - (umoney % 100L) - 1L;
#endif
	    if (qq < 0L) qq += 100L;
	    for ( ; qq <= count; qq += 100L) {
		obj->quan = qq;
		obj->owt = (unsigned)GOLD_WT(qq);
#ifndef GOLDOBJ
		ow = (int)GOLD_WT(u.ugold + qq);
#else
		ow = (int)GOLD_WT(umoney + qq);
#endif
		ow -= (container->otyp == BAG_OF_HOLDING || container->otyp == ICE_BOX_OF_HOLDING || container->otyp == CHEST_OF_HOLDING || container->oartifact == ART_SACK_OF_HOLDING) ?
			(int)DELTA_CWT(container, obj) : (container->otyp == HANDYBAG) ? (int)HANDYBAG_CWT(container, obj) :  (int)obj->owt;
		if (iw + ow >= 0) break;
		oow = ow;
	    }
	    iw -= oow;
	    qq -= 100L;
	}
	if (qq < 0L) qq = 0L;
	else if (qq > count) qq = count;
#ifndef GOLDOBJ
	wt = iw + (int)GOLD_WT(u.ugold + qq);
#else
	wt = iw + (int)GOLD_WT(umoney + qq);
#endif
    } else if (count > 1 || count < obj->quan) {
	/*
	 * Ugh. Calc num to lift by changing the quan of of the
	 * object and calling weight.
	 *
	 * This works for containers only because containers
	 * don't merge.		-dean
	 */
	for (qq = 1L; qq <= count; qq++) {
	    obj->quan = qq;
	    obj->owt = (unsigned)(ow = weight(obj));
	    if (adjust_wt)
		ow -= (container->otyp == BAG_OF_HOLDING || container->otyp == ICE_BOX_OF_HOLDING || container->otyp == CHEST_OF_HOLDING || container->oartifact == ART_SACK_OF_HOLDING) ?
			(int)DELTA_CWT(container, obj) : (container->otyp == HANDYBAG) ? (int)HANDYBAG_CWT(container, obj) :  (int)obj->owt;
	    if (iw + ow >= 0)
		break;
	    wt = iw + ow;
	}
	--qq;
    } else {
	/* there's only one, and we can't lift it */
	qq = 0L;
    }
    obj->quan = savequan;
    obj->owt = saveowt;

    if (qq < count) {
	/* some message will be given */
	strcpy(obj_nambuf, doname(obj));
	if (container) {
	    sprintf(where, "in %s", the(xname(container)));
	    verb = "carry";
	} else {
	    strcpy(where, "lying here");
	    verb = telekinesis ? "acquire" : "lift";
	}
    } else {
	/* lint supppression */
	*obj_nambuf = *where = '\0';
	verb = "";
    }
    /* we can carry qq of them */
    if (qq > 0) {
	if (qq < count)
	    You("can only %s %s of the %s %s.",
		verb, (qq == 1L) ? "one" : "some", obj_nambuf, where);
	*wt_after = wt;
	return qq;
    }

    if (!container) strcpy(where, "here");  /* slightly shorter form */
#ifndef GOLDOBJ
    if (invent || u.ugold) {
#else
    if (invent || umoney) {
#endif
	prefx1 = "you cannot ";
	prefx2 = "";
	suffx  = " any more";
    } else {
	prefx1 = (obj->quan == 1L) ? "it " : "even one ";
	prefx2 = "is too heavy for you to ";
	suffx  = "";
    }
    There("%s %s %s, but %s%s%s%s.",
	  otense(obj, "are"), obj_nambuf, where,
	  prefx1, prefx2, verb, suffx);

 /* *wt_after = iw; */
    return 0L;
}

/* determine whether character is able and player is willing to carry `obj' */
STATIC_OVL
int 
lift_object(obj, container, cnt_p, telekinesis, alwaysflag)
struct obj *obj, *container;	/* object to pick up, bag it's coming out of */
long *cnt_p;
boolean telekinesis;
boolean alwaysflag;
{
    int result, old_wt, new_wt, prev_encumbr, next_encumbr;

    if (obj->otyp == BOULDER && In_sokoban(&u.uz)) {
	You("cannot get your %s around this %s.",
			body_part(HAND), xname(obj));
	return -1;
    }
    if (obj->otyp == LOADSTONE || obj->otyp == LOADBOULDER || obj->otyp == STARLIGHTSTONE || alwaysflag ||
	    (obj->otyp == BOULDER && (throws_rocks(youmonst.data) || (uarmg && uarmg->oartifact == ART_MOUNTAIN_FISTS)) ) ||
	    (obj->oartifact && arti_is_evil(obj)) )
	return 1;		/* lift regardless of current situation */

    *cnt_p = carry_count(obj, container, *cnt_p, telekinesis, &old_wt, &new_wt);
    if (*cnt_p < 1L) {
	result = -1;	/* nothing lifted */

/* Trying to allow the player to pick up as much as they want. --Amy
 * If you REALLY want the limit to come back for some weird reason, enable the knapsacklimit option */

#ifndef GOLDOBJ
    } else if ((flags.knapsacklimit || InventorySizeLimited) && obj->otyp != LOADSTONE && obj->otyp != HEALTHSTONE && obj->otyp != LUCKSTONE && obj->otyp != MANASTONE && obj->otyp != SLEEPSTONE && obj->otyp != LOADBOULDER && obj->otyp != STARLIGHTSTONE && obj->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(obj) && !is_feminismstone(obj) && obj->oclass != COIN_CLASS && inv_cnt() >= (InventorySizeXtra ? 26 : 52) &&
		!merge_choice(invent, obj)) {
#else
    } else if ((flags.knapsacklimit || InventorySizeLimited) && obj->otyp != LOADSTONE && obj->otyp != HEALTHSTONE && obj->otyp != LUCKSTONE && obj->otyp != MANASTONE && obj->otyp != SLEEPSTONE && obj->otyp != LOADBOULDER && obj->otyp != STARLIGHTSTONE && obj->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(obj) && !is_feminismstone(obj) && inv_cnt() >= (InventorySizeXtra ? 26 : 52) && !merge_choice(invent, obj)) {
#endif
	Your("knapsack cannot accommodate any more items.");
	result = -1;	/* nothing lifted */
    } else {
	result = 1;
	prev_encumbr = near_capacity();
	if (prev_encumbr < flags.pickup_burden)
		prev_encumbr = flags.pickup_burden;
	next_encumbr = calc_capacity(new_wt - old_wt);
	if (next_encumbr > prev_encumbr && !(AlwaysAutopickup || u.uprops[AUTOPICKUP_ALWAYS].extrinsic || have_autopickupstone()) ) {
	    /* if (telekinesis) {
		result = 0; */	/* don't lift */ /* Amy edit: nonsense! let the player pick up the goddamn item! */
	    /*} else*/ {
		char qbuf[BUFSZ];
		long savequan = obj->quan;

		obj->quan = *cnt_p;
		strcpy(qbuf,
			(next_encumbr > HVY_ENCUMBER) ? overloadmsg :
			(next_encumbr > MOD_ENCUMBER) ? nearloadmsg :
			moderateloadmsg);
		sprintf(eos(qbuf), " %s. Continue?",
			safe_qbuf(qbuf, sizeof(" . Continue?"),
				doname(obj), an(simple_typename(obj->otyp)), "something"));
		obj->quan = savequan;
		switch (ynq(qbuf)) {
		case 'q':  result = -1; break;
		case 'n':  result =  0; break;
		default:   break;	/* 'y' => result == 1 */
		}
		clear_nhwindow(WIN_MESSAGE);
	    }
	}
    }

    if (obj->otyp == SCR_SCARE_MONSTER && !stack_too_big(obj) && result <= 0 && !container)
	obj->spe = 0;
    return result;
}

/* To prevent qbuf overflow in prompts use planA only
 * if it fits, or planB if PlanA doesn't fit,
 * finally using the fallback as a last resort.
 * last_restort is expected to be very short.
 */
const char *
safe_qbuf(qbuf, padlength, planA, planB, last_resort)
const char *qbuf, *planA, *planB, *last_resort;
unsigned padlength;
{
	/* convert size_t (or int for ancient systems) to ordinary unsigned */
	unsigned len_qbuf = (unsigned)strlen(qbuf),
	         len_planA = (unsigned)strlen(planA),
	         len_planB = (unsigned)strlen(planB),
	         len_lastR = (unsigned)strlen(last_resort);
	unsigned textleft = QBUFSZ - (len_qbuf + padlength);

	if (len_lastR >= textleft) {
	    impossible("safe_qbuf: last_resort too large at %u characters.",
		       len_lastR);
	    return "";
	}
	return (len_planA < textleft) ? planA :
		    (len_planB < textleft) ? planB : last_resort;
}

/*
 * Pick up <count> of obj from the ground and add it to the hero's inventory.
 * Returns -1 if caller should break out of its loop, 0 if nothing picked
 * up, 1 if otherwise.
 */
int
pickup_object(obj, count, telekinesis, alwaysflag)
struct obj *obj;
long count;
boolean telekinesis;	/* not picking it up directly by hand */
boolean alwaysflag;	/* force the item to be picked up even if it burdens you --Amy */
{
	int res, nearload;
#ifndef GOLDOBJ
	const char *where = (obj->ox == u.ux && obj->oy == u.uy) ?
			    "here" : "there";
#endif

	if (obj->quan < count) {
	    impossible("pickup_object: count %ld > quan %ld?",
		count, obj->quan);
	    return 0;
	}

	/* In case of auto-pickup, where we haven't had a chance
	   to look at it yet; affects docall(SCR_SCARE_MONSTER). */
	if (!Blind)
		if ((!obj->oinvis || See_invisible) && !obj->oinvisreal)
		obj->dknown = 1;

	if (obj == uchain) {    /* do not pick up attached chain */
	    return 0;
	} else if (obj->dynamitekaboom) {
	    return 0;
	} else if (obj->mstartinventX) {
	    return 0;
	} else if ( (obj == uball) && obj->otyp == GOLD_PIECE) {
	    return 0;
	} else if (obj->oartifact && !touch_artifact(obj,&youmonst)) {
	    return 0;
#ifndef GOLDOBJ
	} else if (obj->oclass == COIN_CLASS) {
	    /* Special consideration for gold pieces... */
	    long iw = (long)max_capacity() - GOLD_WT(u.ugold);
	    long gold_capacity = GOLD_CAPACITY(iw, u.ugold);

	    if (gold_capacity <= 0L) {
		pline(
	       "There %s %ld gold piece%s %s, but you cannot carry any more.",
		      otense(obj, "are"),
		      obj->quan, plur(obj->quan), where);
		return 0;
	    } else if (gold_capacity < count) {
		You("can only %s %s of the %ld gold pieces lying %s.",
		    telekinesis ? "acquire" : "carry",
		    gold_capacity == 1L ? "one" : "some", obj->quan, where);
		pline("%s %ld gold piece%s.",
		    nearloadmsg, gold_capacity, plur(gold_capacity));
		u.ugold += gold_capacity;
		obj->quan -= gold_capacity;
		costly_gold(obj->ox, obj->oy, gold_capacity);
	    } else {
		/* The SC343-20 bugfix causes phantom crash bugs BECAUSE OBJ IS BEING DELETED. --Amy */
		u.ugold += count;
		if ((nearload = near_capacity()) != 0)
		    pline("%s %ld gold piece%s.",
			  nearload < MOD_ENCUMBER ?
			  moderateloadmsg : nearloadmsg,
			  count, plur(count));
		else
		    prinv((char *) 0, obj, count);
		costly_gold(obj->ox, obj->oy, count);
		if (count == obj->quan)
		    delobj(obj);
		else
		    obj->quan -= count;
	    }
	    flags.botl = 1;
	    if (flags.run) nomul(0, 0, FALSE);
	    return 1;
#endif
	} else if (obj->otyp == PETRIFYIUM_BAR) {
		if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !telekinesis) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
			static char kbuf[BUFSZ];

			strcpy(kbuf, "touching a petrifyium bar");
			pline("Touching a petrifyium bar is a fatal mistake.");
			instapetrify(kbuf);
		    return -1;

		}
		}

	} else if (obj->otyp == PETRIFYIUM_BRA) {
		if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !telekinesis) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
			static char kbuf[BUFSZ];

			strcpy(kbuf, "touching a petrifyium bra");
			pline("Touching a petrifyium bra is a fatal mistake.");
			instapetrify(kbuf);
		    return -1;

		}
		}

	} else if (obj->otyp == CORPSE) {
	    if ( (touch_petrifies(&mons[obj->corpsenm])) && (!uarmg || FingerlessGloves)
				&& (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !telekinesis) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
			static char kbuf[BUFSZ];

			pline("Touching this type of corpse is a fatal mistake.");
			strcpy(kbuf, "touching a petrifying corpse");
			instapetrify(kbuf);
		    return -1;
		}
	    } else if (is_rider(&mons[obj->corpsenm]) || is_deadlysin(&mons[obj->corpsenm])) {
		pline("At your %s, the corpse suddenly moves...",
			telekinesis ? "attempted acquisition" : "touch");
		(void) revive_corpse(obj, FALSE);
		exercise(A_WIS, FALSE);
		return -1;
	    }
	} else if (obj->otyp == EGG) {
	    if ( (touch_petrifies(&mons[obj->corpsenm])) && obj->corpsenm != PM_PLAYERMON && (!uarmg || FingerlessGloves)
				&& (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !telekinesis) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
			static char kbuf[BUFSZ];
			strcpy(kbuf, "coming into contact with a petrifying object");

			pline("Touching a petrifying egg is a fatal mistake.");
			instapetrify(kbuf);
		    return -1;
		}
	    }
	} else  if (obj->otyp == SCR_SCARE_MONSTER && !stack_too_big(obj)) {
	    if (obj->blessed) obj->blessed = 0;
	    else if (!obj->spe && !obj->cursed) obj->spe = 1;
	    else {
		pline_The("scroll%s %s to dust as you %s %s up.",
			plur(obj->quan), otense(obj, "turn"),
			telekinesis ? "raise" : "pick",
			(obj->quan == 1L) ? "it" : "them");
		if (!(objects[SCR_SCARE_MONSTER].oc_name_known) &&
				    !(objects[SCR_SCARE_MONSTER].oc_uname))
		    docall(obj);
		useupf(obj, obj->quan);
		return 1;	/* tried to pick something up and failed, but
				   don't want to terminate pickup loop yet   */
	    }
	} else  if (obj->otyp == SCR_INSTANT_AMNESIA) {

		useupf(obj, obj->quan);
		forget(ALL_SPELLS|ALL_MAP, FALSE);
		pline("If ever I should forget, May God make me more wretched Than ever I have been yet!");
		return 1;	/* tried to pick something up and failed, but
				   don't want to terminate pickup loop yet   */
	} else if (obj && obj->oclass == SCROLL_CLASS && !rn2(2) && (DustbinBug || u.uprops[DUSTBIN_BUG].extrinsic || have_dustbinstone())) {
		useupf(obj, obj->quan);
		pline("Your clumsy %s accidentally rip the paper to pieces.", makeplural(body_part(HAND)));
		return 1;	/* tried to pick something up and failed, but
				   don't want to terminate pickup loop yet   */

	}

	if (Role_if(PM_YAUTJA) && obj && obj->otyp == CHEMISTRY_SET) obj->known = TRUE;
	if (Role_if(PM_CRACKER) && obj && obj->oclass == SCROLL_CLASS) obj->bknown = TRUE;

	if (obj && obj->oclass == WAND_CLASS && (ManaBatteryBug || u.uprops[MANA_BATTERY_BUG].extrinsic || have_batterystone()) && obj->spe >= 0) {

		if (obj->spe == 0) obj->spe = -1;
		else {
			obj->spe -= rnd(obj->spe);
			if (!rn2(3)) obj->spe = 0;
		}
	}

	if (obj && obj->oclass == POTION_CLASS && !rn2(3) && (Monsterfingers || u.uprops[MONSTERFINGERS_EFFECT].extrinsic || have_butterfingerstone()) ) {
		pline("Whoops, the bottle breaks unexpectedly!");
		potionbreathe(obj);
		useupf(obj, obj->quan);
		return 1;	/* tried to pick something up and failed, but
				   don't want to terminate pickup loop yet   */
	}

	if (obj && obj->oartifact == ART_HAAAAAAAAAAAAA_LELUJA) {
		obj->bknown = 1;
		obj->cursed = obj->hvycurse = obj->prmcurse = obj->evilcurse = obj->morgcurse = obj->bbrcurse = obj->stckcurse = 0;
		obj->blessed = 1;
	}

	if ((res = lift_object(obj, (struct obj *)0, &count, telekinesis, alwaysflag)) <= 0)
	    return res;

#ifdef GOLDOBJ
        /* Whats left of the special case for gold :-) */
	if (obj->oclass == COIN_CLASS) flags.botl = 1;
#endif
	if (obj->quan != count && obj->otyp != LOADSTONE && obj->otyp != LUCKSTONE && obj->otyp != HEALTHSTONE && obj->otyp != MANASTONE && obj->otyp != SLEEPSTONE && obj->otyp != LOADBOULDER && obj->otyp != STARLIGHTSTONE && obj->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(obj) && !is_feminismstone(obj) )
	    obj = splitobj(obj, count);

	if (TooHeavyEffect || u.uprops[TOO_HEAVY_EFFECT].extrinsic || have_tooheavystone() || (uamul && uamul->oartifact == ART_THAT_OLD_BUG) || (uarmg && uarmg->oartifact == ART_SUPERHEAVYKLONK) ) {
		if (IncreasedGravity < 10000000) IncreasedGravity += 50;
	}

	obj = pick_obj(obj);

	/* evil patch addition: Nasty gray stones aren't usually generated cursed, but they autocurse if you pick them up. BUC testing won't save you! --Amy */
	if (is_nastygraystone(obj)) curse(obj);
	if (is_feminismstone(obj)) {
		curse(obj);
		pline("Oh no, apparently there is some sort of curse on this gem. It won't leave your inventory as long as it's still cursed.");
	}

	/* artifact versions of such stones should be harder to get rid of --Amy */
	if ( (is_nastygraystone(obj) || is_feminismstone(obj)) ) {
		if (obj->oartifact) {
			if (!rn2(3)) obj->cursed = obj->hvycurse = TRUE;
			if (!rn2(3)) obj->cursed = obj->stckcurse = TRUE;
		}
	}

	if (uwep && uwep == obj) mrg_to_wielded = TRUE;
	nearload = near_capacity();
	prinv(nearload == SLT_ENCUMBER ? moderateloadmsg : (char *) 0,
	      obj, count);
	mrg_to_wielded = FALSE;
	return 1;
}

/*
 * Do the actual work of picking otmp from the floor or monster's interior
 * and putting it in the hero's inventory.  Take care of billing.  Return a
 * pointer to the object where otmp ends up.  This may be different
 * from otmp because of merging.
 *
 * Gold never reaches this routine unless GOLDOBJ is defined.
 */
struct obj *
pick_obj(otmp)
struct obj *otmp;
{
	obj_extract_self(otmp);
	if (!u.uswallow && otmp != uball && costly_spot(otmp->ox, otmp->oy)) {
	    char saveushops[5], fakeshop[2];

	    /* addtobill cares about your location rather than the object's;
	       usually they'll be the same, but not when using telekinesis
	       (if ever implemented) or a grappling hook */
	    strcpy(saveushops, u.ushops);
	    fakeshop[0] = *in_rooms(otmp->ox, otmp->oy, SHOPBASE);
	    fakeshop[1] = '\0';
	    strcpy(u.ushops, fakeshop);
	    /* sets obj->unpaid if necessary */
	    addtobill(otmp, TRUE, FALSE, FALSE);
	    strcpy(u.ushops, saveushops);
	    /* if you're outside the shop, make shk notice */
	    if (!index(u.ushops, *fakeshop))
		remote_burglary(otmp->ox, otmp->oy);
	}
	if (otmp->no_charge)	/* only applies to objects outside invent */
	    otmp->no_charge = 0;
	if (otmp->was_thrown)	/* likewise */
	    otmp->was_thrown = 0;
	newsym(otmp->ox, otmp->oy);
	return addinv(otmp);	/* might merge it with other objects */
}

/*
 * prints a message if encumbrance changed since the last check and
 * returns the new encumbrance value (from near_capacity()).
 */
int
encumber_msg()
{
    static int oldcap = UNENCUMBERED;
    int newcap = near_capacity();

    if(oldcap < newcap) {
	switch(newcap) {
	case 1: Your("movements are slowed slightly because of your load.");
		break;
	case 2: You("rebalance your load.  Movement is difficult.");
		break;
	case 3: You("%s under your heavy load.  Movement is very hard.",
		    stagger(youmonst.data, "stagger"));
		break;
	default: You("%s move a handspan with this load!",
		     newcap == 4 ? "can barely" : "can't even");
		break;
	}
	flags.botl = 1;
    } else if(oldcap > newcap) {
	switch(newcap) {
	case 0: Your("movements are now unencumbered.");
		break;
	case 1: Your("movements are only slowed slightly by your load.");
		break;
	case 2: You("rebalance your load.  Movement is still difficult.");
		break;
	case 3: You("%s under your load.  Movement is still very hard.",
		    stagger(youmonst.data, "stagger"));
		break;
	}
	flags.botl = 1;
    }

    oldcap = newcap;
    return (newcap);
}

/* Is there a container at x,y. Optional: return count of containers at x,y */
STATIC_OVL int
container_at(x, y, countem)
int x,y;
boolean countem;
{
	struct obj *cobj, *nobj;
	int container_count = 0;
	
	for(cobj = level.objects[x][y]; cobj; cobj = nobj) {
		nobj = cobj->nexthere;
		if(Is_container(cobj)) {
			container_count++;
			if (!countem) break;
		}
	}
	return container_count;
}

STATIC_OVL boolean
able_to_loot(x, y)
int x, y;
{
	if (!can_reach_floor()) {
		if (u.usteed && !(uwep && uwep->oartifact == ART_SORTIE_A_GAUCHE) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !FemtrapActiveKerstin && !(bmwride(ART_DEEPER_LAID_BMW)) && (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) )
			rider_cant_reach(); /* not skilled enough to reach */
		else
			You("cannot reach the %s.", surface(x, y));

		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return FALSE;
	} else if ((is_waterypool(x, y) || is_watertunnel(x,y) || is_lava(x, y)) && !is_crystalwater(x, y)) {
		/* at present, can't loot in water even when Underwater */
		You("cannot loot things that are deep in the %s.",
		    is_lava(x, y) ? "lava" : "water");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return FALSE;
	} else if (nolimbs(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
		pline("Without limbs, you cannot loot anything.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return FALSE;
	} else if (!freehandX() && !(Role_if(PM_CELLAR_CHILD) && uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) ) {
		pline("Without a free %s, you cannot loot anything.",
			body_part(HAND));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return FALSE;
	}
	return TRUE;
}

STATIC_OVL boolean
mon_beside(x,y)
int x, y;
{
	int i,j,nx,ny;
	for(i = -1; i <= 1; i++)
	    for(j = -1; j <= 1; j++) {
	    	nx = x + i;
	    	ny = y + j;
		if(isok(nx, ny) && MON_AT(nx, ny))
			return TRUE;
	    }
	return FALSE;
}

int
doloot()	/* loot a container on the floor or loot saddle from mon. */
{

	if (MenuIsBugged) {
	pline("The loot command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

    struct obj *cobj, *nobj;
    register int c = -1;
    int timepassed = 0;
    coord cc;
    boolean underfoot = TRUE;
    const char *dont_find_anything = "don't find anything";
    struct monst *mtmp;
    char qbuf[BUFSZ];
    int prev_inquiry = 0;
    boolean prev_loot = FALSE;
    struct trap *trap;

	if (trap = t_at(u.ux, u.uy)) {
		if (trap->ttyp == VIVISECTION_TRAP) {
			You("are in vivisection, and therefore unable to loot anything!");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 0;
		}
	}

    if (check_capacity((char *)0)) {
	/* "Can't do that while carrying so much stuff." */
	return 0;
    }

	/* It used to check for hands here, but I removed that because actually looting a container already checks if your
	 * form has hands. It was really annoying that you could have twice the failure chance when looting off the ground
	 * as opposed to applying a container in your inventory, so I changed that. --Amy */

    cc.x = u.ux; cc.y = u.uy;

lootcont:

    if (container_at(cc.x, cc.y, FALSE)) {
	boolean any = FALSE;

	if (!able_to_loot(cc.x, cc.y)) return 0;
	for (cobj = level.objects[cc.x][cc.y]; cobj; cobj = nobj) {
	    nobj = cobj->nexthere;

	    if (Is_container(cobj)) {
		sprintf(qbuf, "There is %s here, loot it?",
			safe_qbuf("", sizeof("There is  here, loot it?"),
			     doname(cobj), an(simple_typename(cobj->otyp)),
			     "a container"));
		c = ynq(qbuf);
		if (c == 'q') return (timepassed);
		if (c == 'n') continue;
		any = TRUE;

		if (cobj->mstartinventX) {
			pline("The container is aetheric and can't be handled by you!");
			continue;
		}

		if (cobj->olocked) {
		    if (uwep && uwep->oartifact == ART_FINAL_DOOR_SOLUTION) {
			cobj->olocked = FALSE;
			pline("kloeck!");
		    } else {
			pline("Hmmm, it seems to be locked.");
			continue;
		    }
		}
		if (cobj->otyp == BAG_OF_TRICKS) {
		    int tmp;
		    You("carefully open the bag...");
		    pline("It develops a huge set of teeth and bites you!");
		    tmp = rnd(10);
		    if (Half_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) tmp = (tmp+1) / 2;
		    if (StrongHalf_physical_damage && (rn2(2) || (uwep && uwep->oartifact == ART_SOOTHE_)) ) tmp = (tmp+1) / 2;
		    losehp(tmp, "carnivorous bag", KILLED_BY_AN);
		    makeknown(BAG_OF_TRICKS);
		    timepassed = 1;
		    continue;
		}

		You("carefully open %s...", the(xname(cobj)));
		if ((cobj->otyp == BAG_OF_DIGESTION || cobj->otyp == LARGE_BOX_OF_DIGESTION || cobj->otyp == ICE_BOX_OF_DIGESTION) && !timepassed) timepassed = 1;
		if ((cobj->otyp == BAG_OF_HOLDING || cobj->otyp == CHEST_OF_HOLDING || cobj->oartifact == ART_SACK_OF_HOLDING || cobj->otyp == ICE_BOX_OF_HOLDING) && cobj->cursed && !timepassed) timepassed = 1;
		timepassed |= use_container(&cobj, 0); /* ATTENTION: cobj might be gone now (boh explosion) --Amy */

		/* might have triggered chest trap or magic bag explosion */
		if (multi < 0 || !cobj) return 1;
	    }
	}
	if (any) c = 'y';
    } else if (Confusion) {
#ifndef GOLDOBJ
	if (u.ugold){
	    long contribution = rnd((int)min(LARGEST_INT,u.ugold));
	    struct obj *goldob = mkgoldobj(contribution);
#else
	struct obj *goldob;
	/* Find a money object to mess with */
	for (goldob = invent; goldob; goldob = goldob->nobj) {
	    if (goldob->oclass == COIN_CLASS) break;
	}
	if (goldob){
	    long contribution = rnd((int)min(LARGEST_INT, goldob->quan));
	    if (contribution < goldob->quan)
		goldob = splitobj(goldob, contribution);
	    freeinv(goldob);
#endif
	    if (IS_THRONE(levl[u.ux][u.uy].typ)){
		struct obj *coffers;
		int pass;
		/* find the original coffers chest, or any chest */
		for (pass = 2; pass > -1; pass -= 2)
		    for (coffers = fobj; coffers; coffers = coffers->nobj)
			if (coffers->otyp == CHEST && coffers->spe == pass)
			    goto gotit;	/* two level break */
gotit:
		if (coffers) {
	    verbalize("Thank you for your contribution to reduce the debt.");
		    (void) add_to_container(coffers, goldob, TRUE);
		    coffers->owt = weight(coffers);
		} else {
		    struct monst *mon = makemon(courtmon(),
					    u.ux, u.uy, NO_MM_FLAGS);
		    if (mon) {
#ifndef GOLDOBJ
			mon->mgold += goldob->quan;
			delobj(goldob);
			pline("The exchequer accepts your contribution.");
		    } else {
			dropx(goldob);
		    }

			    if (!rn2(5) && IS_THRONE(levl[u.ux][u.uy].typ)) { /* reduce player's farming ability */
				/* may have teleported */
				levl[u.ux][u.uy].typ = ROOM;
				pline_The("throne vanishes in a puff of logic.");
				newsym(u.ux,u.uy);
			    }
			return 1; /* this is supposed to take time! --Amy */

		}
	    } else {
		dropx(goldob);
#else
			add_to_minv(mon, goldob);
			pline("The exchequer accepts your contribution.");
		    } else {
			dropy(goldob);
		    }
		}
	    } else {
		dropy(goldob);
#endif
		pline("Ok, now there is loot here.");
	    }
	}
    } else if (IS_GRAVE(levl[cc.x][cc.y].typ)) {
	You("need to dig up the grave to effectively loot it...");
    }
    /*
     * 3.3.1 introduced directional looting for some things.
     */
    if (c != 'y' && mon_beside(u.ux, u.uy)) {
	if (!get_adjacent_loc("Loot in what direction?", "Invalid loot location",
			u.ux, u.uy, &cc)) return 0;
	if (cc.x == u.ux && cc.y == u.uy) {
	    underfoot = TRUE;
	    if (container_at(cc.x, cc.y, FALSE))
		goto lootcont;
	} else
	    underfoot = FALSE;
	if (u.dz < 0) {
	    You("%s to loot on the %s.", dont_find_anything,
		ceiling(cc.x, cc.y));
	    timepassed = 1;
	    return timepassed;
	}
	mtmp = m_at(cc.x, cc.y);
	if (mtmp) timepassed = loot_mon(mtmp, &prev_inquiry, &prev_loot);

	/* Preserve pre-3.3.1 behaviour for containers.
	 * Adjust this if-block to allow container looting
	 * from one square away to change that in the future.
	 */
	if (!underfoot) {
	    if (container_at(cc.x, cc.y, FALSE)) {
		if (mtmp) {
		    You_cant("loot anything %sthere with %s in the way.",
			    prev_inquiry ? "else " : "", mon_nam(mtmp));
		    return timepassed;
		} else {
		    You("have to be at a container to loot it.");
		}
	    } else {
		You("%s %sthere to loot.", dont_find_anything,
			(prev_inquiry || prev_loot) ? "else " : "");
		return timepassed;
	    }
	}
    } else if (c != 'y' && c != 'n') {
	You("%s %s to loot.", dont_find_anything,
		    underfoot ? "here" : "there");
    }
    return (timepassed);
}

/* loot_mon() returns amount of time passed.
 */
int
loot_mon(mtmp, passed_info, prev_loot)
struct monst *mtmp;
int *passed_info;
boolean *prev_loot;
{
    int c = -1;
    int timepassed = 0;
    struct obj *otmp;
    char qbuf[QBUFSZ];

    /* 3.3.1 introduced the ability to remove saddle from a steed             */
    /* 	*passed_info is set to TRUE if a loot query was given.               */
    /*	*prev_loot is set to TRUE if something was actually acquired in here. */
    if (mtmp && mtmp != u.usteed && (otmp = which_armor(mtmp, W_SADDLE))) {
	long unwornmask;
	if (passed_info) *passed_info = 1;
	sprintf(qbuf, "Do you want to remove the saddle from %s?",
		x_monnam(mtmp, ARTICLE_THE, (char *)0, SUPPRESS_SADDLE, FALSE));
	if ((c = yn_function(qbuf, ynqchars, 'n')) == 'y') {
		if (nolimbs(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
		    You_cant("do that without limbs."); /* not body_part(HAND) */
		    return (0);
		}
		if (otmp->cursed) {
		    You("can't. The saddle seems to be stuck to %s.",
			x_monnam(mtmp, ARTICLE_THE, (char *)0,
				SUPPRESS_SADDLE, FALSE));
			    
		    /* the attempt costs you time */
			return (1);
		}
		/* wtf, why was it guaranteed to work even for a hostile horse trying to kill you??? --Amy */
		if (!mtmp->mtame && !mtmp->mpeaceful) {
			pline("%s doesn't just let you remove the saddle!", Monnam(mtmp));
			return (1);
		}
		obj_extract_self(otmp);
		if ((unwornmask = otmp->owornmask) != 0L) {
		    mtmp->misc_worn_check &= ~unwornmask;
		    otmp->owornmask = 0L;
		    update_mon_intrinsics(mtmp, otmp, FALSE, FALSE);
		}
		otmp = hold_another_object(otmp, "You drop %s!", doname(otmp),
					(const char *)0);
		timepassed = rnd(3);
		if (prev_loot) *prev_loot = TRUE;
	} else if (c == 'q') {
		return (0);
	}
    }
    /* 3.4.0 introduced the ability to pick things up from within swallower's stomach */
    if (u.uswallow) {
	int count = passed_info ? *passed_info : 0;
	timepassed = pickup(count);
    }
    return timepassed;
}

/*
 * Decide whether an object being placed into a magic bag will cause
 * it to explode.  If the object is a bag itself, check recursively.
 */
boolean
mbag_explodes(obj, depthin)
    struct obj *obj;
    int depthin;
{
    /* these won't cause an explosion when they're empty */
    if ((obj->otyp == WAN_CANCELLATION || obj->otyp == BAG_OF_TRICKS) &&
	    obj->spe <= 0)
	return FALSE;

    /* odds: 1/1, 2/2, 3/4, 4/8, 5/16, 6/32, 7/64, 8/128, 9/128, 10/128,... */
    if ((Is_mbag(obj) || obj->otyp == WAN_CANCELLATION) &&
	(rn2(1 << (depthin > 7 ? 7 : depthin)) <= depthin))
	return TRUE;
    else if (Has_contents(obj)) {
	struct obj *otmp;

	for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
	    if (mbag_explodes(otmp, depthin+1)) return TRUE;
    }
    return FALSE;
}

void
destroy_mbag(bomb, silent)
struct obj *bomb;
boolean silent;
{
    xchar x,y;
    boolean underwater;
    struct monst *mtmp = (struct monst *)0;

    if (get_obj_location(bomb, &x, &y, BURIED_TOO | CONTAINED_TOO)) {
	switch(bomb->where) {		
	    case OBJ_MINVENT:
		mtmp = bomb->ocarry;
		if (bomb == MON_WEP(mtmp)) {
		    bomb->owornmask &= ~W_WEP;
		    MON_NOWEP(mtmp);
		}
		if (!silent && canseemon(mtmp))
		    You("see %s engulfed in an explosion!", mon_nam(mtmp));
		mtmp->mhp -= d(6,6);
		if (mtmp->mhp < 1) {
		    if (!bomb->yours) 
			monkilled(mtmp, silent ? "" : "explosion", AD_PHYS);
		    else xkilled(mtmp, !silent);
		}
		break;
	    case OBJ_INVENT:
		/* This shouldn't be silent! */
		pline("Something explodes inside your knapsack!");
		if (bomb == uwep) {
		    uwepgone();
		    stop_occupation();
		} else if (bomb == uswapwep) {
		    uswapwepgone();
		    stop_occupation();
		} else if (bomb == uquiver) {
		    uqwepgone();
		    stop_occupation();
		}
		losehp(d(6,6), "carrying live explosives", KILLED_BY);
		break;
	    case OBJ_FLOOR:
		underwater = (is_waterypool(x, y) || is_watertunnel(x,y));
		if (!silent) {
		    if (x == u.ux && y == u.uy) {
			if (underwater && (Flying || Levitation))
			    pline_The("water boils beneath you.");
			else if (underwater && (Wwalking || Race_if(PM_KORONST)))
			    pline_The("water erupts around you.");
			else pline("A bag explodes under your %s!",
			  makeplural(body_part(FOOT)));
		    } else if (cansee(x, y))
			You(underwater ?
			    "see a plume of water shoot up." :
			    "see a bag explode.");
		}
		if (underwater && (Flying || Levitation || Wwalking || Race_if(PM_KORONST))) {
		    if ((Wwalking || Race_if(PM_KORONST)) && x == u.ux && y == u.uy) {
			struct trap trap;
			trap.ntrap = NULL;
			trap.tx = x;
			trap.ty = y;
			trap.launch.x = -1;
			trap.launch.y = -1;
			trap.ttyp = RUST_TRAP;
			trap.tseen = 0;
			trap.once = 0;
			trap.madeby_u = 0;
			trap.dst.dnum = -1;
			trap.dst.dlevel = -1;
			dotrap(&trap, 0);
		    }
		    goto free_bomb;
		}
		break;
	    default:	/* Buried, contained, etc. */
		if (!silent)
		    You_hear("a muffled explosion.");
		goto free_bomb;
		break;
	}
    }

free_bomb:
    if (Has_contents(bomb))
	delete_contents(bomb);

    obj_extract_self(bomb);
    obfree(bomb, (struct obj *)0);
    newsym(x,y);
}

int put_into_container(struct obj *container, struct obj *obj)
{
	struct obj *t;

	t = current_container;
	current_container = container;
	in_container_(obj,FALSE);
	current_container = t;

	/* control reaches end of non-void function, but is this one even used anywhere?! --Amy */
	return 1; /* eh, let's return 1 for now, can still change it if necessary */
}


/* Returns: -1 to stop, 1 item was inserted, 0 item was not inserted. */
STATIC_PTR int
in_container_(obj,invobj)
register struct obj *obj;
boolean invobj;
{
	boolean floor_container = !carried(current_container);
	boolean was_unpaid = FALSE;
	char buf[BUFSZ];

	if (!current_container) {
		impossible("<in> no current_container?");
		return 0;
	} else if (obj == uball || obj == uchain) {
		You("must be kidding.");
		return 0;
	} else if (obj == current_container) {
		pline(FunnyHallu ? "You try folding it with some ikebana technique but to no avail." : "That would be an interesting topological exercise.");
		return 0;
	} else if (obj->owornmask & (W_ARMOR | W_RING | W_AMUL | W_IMPLANT | W_TOOL)) {
		Norep("You cannot %s %s you are wearing.",
			Icebox ? "refrigerate" : "stash", something);
		return 0;
	} else if ((obj->otyp == LOADSTONE || obj->otyp == LUCKSTONE || obj->otyp == HEALTHSTONE || obj->otyp == MANASTONE || obj->otyp == SLEEPSTONE || obj->otyp == LOADBOULDER || obj->otyp == STARLIGHTSTONE || obj->otyp == STONE_OF_MAGIC_RESISTANCE || is_nastygraystone(obj) ) && obj->cursed) {
		obj->bknown = 1;
	      pline_The("stone%s won't leave your person.", plur(obj->quan));
		return 0;
	} else if (obj->otyp == LUCKSTONE && isevilvariant && !obj->cursed && !obj->blessed && Luck < 0) {
		pline("Har har har, an evil presence prevents you from getting rid of that!");
		return(FALSE);
	} else if (is_feminismstone(obj) && obj->cursed) {
		obj->bknown = 1;
		pline_The("gem%s will not leave your inventory as long as they're cursed.", plur(obj->quan));
		return 0;
	} else if ( (obj->otyp == AMULET_OF_YENDOR && !u.freeplaymode) || (obj->otyp == FAKE_AMULET_OF_YENDOR && !u.freeplaymode) ||
		   (obj->otyp == CANDELABRUM_OF_INVOCATION && !u.uevent.invoked) ||
		   (obj->otyp == BELL_OF_OPENING && !u.uevent.invoked) ||
		   (obj->oartifact == ART_KEY_OF_LAW && !u.uevent.invoked) ||
		   (obj->oartifact == ART_GAUNTLET_KEY && !u.uevent.invoked) ||
		   (obj->oartifact == ART_KEY_OF_NEUTRALITY && !u.uevent.invoked) ||
		   (obj->oartifact == ART_KEY_OF_CHAOS && !u.uevent.invoked) ||
		   (obj->otyp == SPE_BOOK_OF_THE_DEAD && !u.uevent.invoked) ) {
	/* Prohibit Amulets in containers; if you allow it, monsters can't
	 * steal them.  It also becomes a pain to check to see if someone
	 * has the Amulet.  Ditto for the Candelabrum, the Bell and the Book.
	 */
	    pline("%s cannot be confined in such trappings.", The(xname(obj)));
	    return 0;
	} else if (obj->otyp == LEATHER_LEASH && obj->leashmon != 0) {
		pline("%s attached to your pet.", Tobjnam(obj, "are"));
		return 0;
	} else if (obj->otyp == INKA_LEASH && obj->leashmon != 0) {
		pline("%s attached to your pet.", Tobjnam(obj, "are"));
		return 0;
	} else if (obj->otyp == ADAMANT_LEASH && obj->leashmon != 0) {
		pline("%s attached to your pet.", Tobjnam(obj, "are"));
		return 0;
	} else if (obj == uwep) {
		if (welded(obj)) {
			weldmsg(obj);
			return 0;
		}
		setuwep((struct obj *) 0, FALSE, TRUE);
		if (uwep) return 0;	/* unwielded, died, rewielded */
	} else if (obj == uswapwep) {
		setuswapwep((struct obj *) 0, FALSE);
		if (uswapwep) return 0;     /* unwielded, died, rewielded */
	} else if (obj == uquiver) {
		setuqwep((struct obj *) 0);
		if (uquiver) return 0;     /* unwielded, died, rewielded */
	}

	if (obj->otyp == CORPSE) {
	    if ( (touch_petrifies(&mons[obj->corpsenm])) && (!uarmg || FingerlessGloves)
		 && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    static char kbuf[BUFSZ];

		    pline("Touching this type of corpse is a fatal mistake.");
		    strcpy(kbuf, "touching a petrifying corpse");
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	if (obj->otyp == PETRIFYIUM_BAR) {
	    if ((!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    static char kbuf[BUFSZ];

		    strcpy(kbuf, "touching a petrifyium bar");
		    pline("Touching a petrifyium bar is a fatal mistake.");
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	if (obj->otyp == PETRIFYIUM_BRA) {
	    if ((!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    static char kbuf[BUFSZ];

		    strcpy(kbuf, "touching a petrifyium bra");
		    pline("Touching a petrifyium bra is a fatal mistake.");
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	if (obj->otyp == EGG) {
	    if ( (touch_petrifies(&mons[obj->corpsenm])) && obj->corpsenm != PM_PLAYERMON && (!uarmg || FingerlessGloves)
		 && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    static char kbuf[BUFSZ];

		    pline("Touching a petrifying egg is a fatal mistake.");
		    strcpy(kbuf, "touching a petrifying egg");
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	/* boxes, boulders, and big statues can't fit into any container */
	if (obj->otyp == ICE_BOX || obj->otyp == DISPERSION_BOX || obj->otyp == ICE_BOX_OF_HOLDING || obj->otyp == ICE_BOX_OF_WATERPROOFING || obj->otyp == ICE_BOX_OF_DIGESTION || Is_box(obj) || obj->otyp == BOULDER ||
		(obj->otyp == STATUE && bigmonst(&mons[obj->corpsenm]))) {
		/*
		 *  xname() uses a static result array.  Save obj's name
		 *  before current_container's name is computed.  Don't
		 *  use the result of strcpy() within You() --- the order
		 *  of evaluation of the parameters is undefined.
		 */
		strcpy(buf, the(xname(obj)));
		You("cannot fit %s into %s.", buf,
		    the(xname(current_container)));
		return 0;
	}

	if (invobj) freeinv(obj);

	if (obj_is_burning(obj))	/* this used to be part of freeinv() */
		(void) snuff_lit(obj);

	if (floor_container && costly_spot(u.ux, u.uy)) {
	    if (current_container->no_charge && !obj->unpaid) {
		/* don't sell when putting the item into your own container */
		obj->no_charge = 1;
	    } else if (obj->oclass != COIN_CLASS) {
		/* sellobj() will take an unpaid item off the shop bill
		 * note: coins are handled later */
		was_unpaid = obj->unpaid ? TRUE : FALSE;
		sellobj_state(SELL_DELIBERATE);
		sellobj(obj, u.ux, u.uy);
		sellobj_state(SELL_NORMAL);
	    }
	}
	if (Icebox && !age_is_relative(obj) && !is_lightsaber(obj)) {
		obj->icedobject = TRUE;
		obj->age = monstermoves - obj->age; /* actual age */
		/* stop any corpse timeouts when frozen */
		if (obj->otyp == CORPSE && obj->timed) {
			long rot_alarm = stop_timer(ROT_CORPSE, (void *)obj);
			(void) stop_timer(MOLDY_CORPSE, (void *)obj);
			(void) stop_timer(REVIVE_MON, (void *)obj);
			/* mark a non-reviving corpse as such */
			if (rot_alarm) obj->norevive = 1;
		}
	}

	/* There are ice boxes of holding --Amy */
	if (Is_mbag(current_container) && mbag_explodes(obj, 0)) {
		/* explicitly mention what item is triggering the explosion */
		pline(
	      "As you put %s inside, you are blasted by a magical explosion!",
		      doname(obj));
		if (PlayerHearsSoundEffects) pline(issoviet ? "VD GA GA GA vy takoy nemnogo nub, kto ne znayet, kak igrat' na vsekh! Ya ne byl by nastol'ko glup, no vy poteryali sumku provedeniya v nastoyashcheye vremya i, veroyatno, slomal vse zel'ya, kotoryye byli v nem. TY POLNYY OTSTOY!" : "Caeauwaesh! Well and?");
		if (Has_contents(obj)) {
		    struct obj *otmp;
		    while((otmp = container_extract_indestructable(obj)))
			if (!flooreffects(otmp, u.ux, u.uy, "fall"))
			    place_object(otmp, u.ux, u.uy);
		}

		/* did not actually insert obj yet */
		if (was_unpaid) addtobill(obj, FALSE, FALSE, TRUE);

/*		if (Has_contents(obj))
		    delete_contents(obj);
		obfree(obj, (struct obj *)0);
		delete_contents(current_container);
		if (!floor_container)
			useup(current_container);
		else if (obj_here(current_container, u.ux, u.uy))
			useupf(current_container, current_container->quan);
		else
			panic("in_container:  bag not found."); */

		/* dump it out onto the floor so the scatterage can take effect */
		if (dump_container(current_container, TRUE, u.ux, u.uy)) {
			pline("The contents fly everywhere!");
		}
		scatter(u.ux,u.uy,10,VIS_EFFECTS|MAY_HIT|MAY_DESTROY|MAY_FRACTURE,0);

		losehp(d(6,6),"magical explosion", KILLED_BY_AN);
		current_container = 0;	/* baggone = TRUE; */
	}

	if (current_container) {
	    strcpy(buf, the(xname(current_container)));
	    if (invobj) You("put %s into %s.", doname(obj), buf);

	    /* gold in container always needs to be added to credit */
	    if (floor_container && obj->oclass == COIN_CLASS)
		sellobj(obj, current_container->ox, current_container->oy);
	    (void) add_to_container(current_container, obj, TRUE);
	    current_container->owt = weight(current_container);
	}
	/* gold needs this, and freeinv() many lines above may cause
	 * the encumbrance to disappear from the status, so just always
	 * update status immediately.
	 */
	bot();

	return(current_container ? 1 : -1);
}

STATIC_PTR int
in_container(obj)
register struct obj *obj;
{
	return in_container_(obj,TRUE);
}


STATIC_PTR int
ck_bag(obj)
struct obj *obj;
{
	return current_container && obj != current_container;
}


/* Returns: -1 to stop, 1 item was removed, 0 item was not removed. */
STATIC_PTR int
out_container(obj)
register struct obj *obj;
{
	register struct obj *otmp;
	boolean is_gold = (obj->oclass == COIN_CLASS);
	int res, loadlev;
	long count;

	if (!current_container) {
		impossible("<out> no current_container?");
		return -1;
	} else if (is_gold) {
		obj->owt = weight(obj);
	}

	if(obj->oartifact && !touch_artifact(obj,&youmonst)) return 0;

	if (obj->otyp == CORPSE) {
	    if ( (touch_petrifies(&mons[obj->corpsenm])) && (!uarmg || FingerlessGloves)
		 && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    static char kbuf[BUFSZ];

		    pline("Touching this type of corpse is a fatal mistake.");
		    strcpy(kbuf, "touching a petrifying corpse");
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	if (obj->otyp == PETRIFYIUM_BAR) {
	    if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    static char kbuf[BUFSZ];

		    strcpy(kbuf, "touching a petrifyium bar");
		    pline("Touching a petrifyium bar is a fatal mistake.");
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	if (obj->otyp == PETRIFYIUM_BRA) {
	    if ( (!uarmg || FingerlessGloves) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    static char kbuf[BUFSZ];

		    strcpy(kbuf, "touching a petrifyium bra");
		    pline("Touching a petrifyium bra is a fatal mistake.");
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	if (obj->otyp == EGG) {
	    if ( (touch_petrifies(&mons[obj->corpsenm])) && obj->corpsenm != PM_PLAYERMON && (!uarmg || FingerlessGloves)
		 && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
		    display_nhwindow(WIN_MESSAGE, FALSE);
		else {
		    static char kbuf[BUFSZ];

		    pline("Touching a petrifying egg is a fatal mistake.");
		    strcpy(kbuf, "touching a petrifying egg");
		    instapetrify(kbuf);
		    return -1;
		}
	    }
	}

	count = obj->quan;
	if ((res = lift_object(obj, current_container, &count, FALSE, FALSE)) <= 0)
	    return res;

	if (obj->quan != count && obj->otyp != LOADSTONE && obj->otyp != LUCKSTONE && obj->otyp != HEALTHSTONE && obj->otyp != MANASTONE && obj->otyp != SLEEPSTONE && obj->otyp != LOADBOULDER && obj->otyp != STARLIGHTSTONE && obj->otyp != STONE_OF_MAGIC_RESISTANCE && !is_nastygraystone(obj) && !is_feminismstone(obj) )
	    obj = splitobj(obj, count);

	/* Remove the object from the list. */
	obj_extract_self(obj);
	current_container->owt = weight(current_container);

	if (Icebox && !age_is_relative(obj) && !is_lightsaber(obj)) {
		obj->icedobject = TRUE;
		obj->age = monstermoves - obj->age; /* actual age */
		if (obj->otyp == CORPSE)
			start_corpse_timeout(obj);
	}
	/* simulated point of time */

	if(!obj->unpaid && !carried(current_container) &&
	     costly_spot(current_container->ox, current_container->oy)) {
		obj->ox = current_container->ox;
		obj->oy = current_container->oy;
		addtobill(obj, FALSE, FALSE, FALSE);
	}
	if (is_pick(obj) && !obj->unpaid && *u.ushops && shop_keeper(*u.ushops))
		verbalize("You sneaky cad! Get out of here with that pick!");

	otmp = addinv(obj);
	loadlev = near_capacity();
	prinv(loadlev ?
	      (loadlev < MOD_ENCUMBER ?
	       "You have a little trouble removing" :
	       "You have much trouble removing") : (char *)0,
	      otmp, count);

	if (is_gold) {
#ifndef GOLDOBJ
		dealloc_obj(obj);
#endif
		bot();	/* update character's gold piece count immediately */
	}
	return 1;
}

/* an object inside a cursed bag of holding is being destroyed */
long
mbag_item_gone(held, item, yourfault)
int held;
struct obj *item;
boolean yourfault;
{
    struct monst *shkp;
    long loss = 0L;

	/* In the Evil Variant, you aren't told which items have been destroyed. --Amy */
    if (yourfault) {
	    if (evilfriday)
		Norep("Stuff has vanished!");
	    else if (item->dknown)
		pline("%s %s vanished!", Doname2(item), otense(item, "have"));
	    else
		You("%s %s disappear!", Blind ? "notice" : "see", doname(item));

	    if (*u.ushops && (shkp = shop_keeper(*u.ushops)) != 0) {
		if (held ? (boolean) item->unpaid : costly_spot(u.ux, u.uy))
		    loss = stolen_value(item, u.ux, u.uy,
					(boolean)shkp->mpeaceful, TRUE, TRUE);
	    }
    }

    /* [ALI] In Slash'EM we must delete the contents of containers before
     * we call obj_extract_self() so that any indestructable items can
     * migrate into the bag of holding. We are also constrained by the
     * need to wait until after we have calculated any loss.
     */
    if (Has_contents(item)) delete_contents(item);
    obj_extract_self(item);
    obfree(item, (struct obj *) 0);
    return loss;
}

STATIC_OVL void
observe_quantum_cat(box)
struct obj *box;
{
    static NEARDATA const char sc[] = "Schroedinger's Cat";
    struct obj *deadcat;
    struct monst *livecat;
    xchar ox, oy;

    box->spe = 0;		/* box->owt will be updated below */
    if (get_obj_location(box, &ox, &oy, 0))
	box->ox = ox, box->oy = oy;	/* in case it's being carried */

    /* this isn't really right, since any form of observation
       (telepathic or monster/object/food detection) ought to
       force the determination of alive vs dead state; but basing
       it just on opening the box is much simpler to cope with */
    livecat = rn2(2) ? makemon(&mons[PM_HOUSECAT],
			       box->ox, box->oy, NO_MINVENT) : 0;
    if (livecat) {
	livecat->mpeaceful = 1;
	set_malign(livecat);
	if (!canspotmon(livecat))
	    You("think %s brushed your %s.", something, body_part(FOOT));
	else
	    pline("%s inside the box is still alive!", Monnam(livecat));
	(void) christen_monst(livecat, sc);
    } else {
	deadcat = mk_named_object(CORPSE, &mons[PM_HOUSECAT],
				  box->ox, box->oy, sc);
	if (deadcat) {
	    obj_extract_self(deadcat);
	    (void) add_to_container(box, deadcat, TRUE);
	}
	pline_The("%s inside the box is dead!",
	    Hallucination ? rndmonnam() : "housecat");
    }
    box->owt = weight(box);
    return;
}

#undef Icebox

void
containerkaboom()
{
	    switch(rn2(26)) {
		case 25:
		case 24:
		case 23:
		case 22:
		case 21:
			pline("You're hit by a massive explosion!");
			wake_nearby();
			losehp( (d(6,6) + rnd((monster_difficulty()) + 1) ), "massive explosion", KILLED_BY_AN);
			exercise(A_STR, FALSE);
			break;
		case 20:
		case 19:
		case 18:
		case 17:
			pline("A cloud of noxious gas billows out at you.");
			poisoned("gas cloud", A_STR, "cloud of poison gas",15);
			exercise(A_CON, FALSE);
			break;
		case 16:
		case 15:
		case 14:
		case 13:
			You_feel("a needle prick your %s.",body_part(ARM));
			poisoned("needle", A_CON, "poisoned needle",10);
			exercise(A_CON, FALSE);
			break;
		case 12:
		case 11:
		case 10:
		case 9:
			dofiretrap((struct obj *)0);
			break;
		case 8:
		case 7:
		case 6: {
			int dmg;

			You("are jolted by a surge of electricity!");
			if((Shock_resistance && (StrongShock_resistance || rn2(10))) || ShockImmunity)  {
			    shieldeff(u.ux, u.uy);
			    You("don't seem to be affected.");
			    break;
			} else
			    losehp(d(4, 4) + rnd((monster_difficulty() / 2) + 1), "electric shock", KILLED_BY_AN);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(RING_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 6 : 33)) /* new calculations --Amy */	destroy_item(WAND_CLASS, AD_ELEC);
		    if (isevilvariant || !rn2(issoviet ? 30 : 165)) /* new calculations --Amy */	destroy_item(AMULET_CLASS, AD_ELEC);
			break;
		      }
		case 5:
		case 4:
		case 3:
			if (!Free_action || !rn2(StrongFree_action ? 100 : 20)) {
			pline("Suddenly you are frozen in place!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
			if (isstunfish) nomul(-rnz(11), "frozen by a container kaboom", TRUE);
			else nomul(-rn1(5, 6), "frozen by a container kaboom", TRUE);
			exercise(A_DEX, FALSE);
			nomovemsg = You_can_move_again;
			} else You("momentarily stiffen.");
			break;
		case 2:
		case 1:
		case 0:
			pline("A cloud of %s gas billows out at you.", Blind ? "booming" : rndcolor() );
			if(!Stunned) {
			    if (Blind)
				You("%s and get dizzy...",
				    stagger(youmonst.data, "stagger"));
			    else
				You("%s and your vision blurs...",
				    stagger(youmonst.data, "stagger"));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
			}
			make_stunned(HStun + rn1(7, 16) + rnd((monster_difficulty() / 2) + 1),FALSE);
			(void) make_hallucinated(HHallucination + rn1(5, 16) + rnd((monster_difficulty() / 2) + 1),FALSE,0L);
			break;
		default: impossible("bad kaboom trap");
			break;
		}
}

/* used by askchain() to check for magic bag explosion */
boolean
container_gone(fn)
int (*fn)(OBJ_P);
{
    /* result is only meaningful while use_container() is executing */
    return ((fn == in_container || fn == out_container) && !current_container);
}

int
use_container(objp, held)
struct obj **objp;
int held;
{
	struct obj *curr, *otmp, *obj = *objp;
#ifndef GOLDOBJ
	struct obj *u_gold = (struct obj *)0;
#endif
	struct monst *shkp;
	boolean one_by_one, allflag, quantum_cat = FALSE,
		loot_out = FALSE, loot_in = FALSE, loot_reverse = FALSE;
	char select[MAXOCLASSES+1];
	char qbuf[BUFSZ], emptymsg[BUFSZ], pbuf[QBUFSZ];
	long loss = 0L;
	int cnt = 0, used = 0, lcnt = 0,
	    menu_on_request;
	int monsterator = 0;

	if (obj && obj->oartifact == ART_SNAP_TOO && !rn2(100)) {
		obj->olocked = TRUE;
		obj->obroken = FALSE;
	}
	if (obj && obj->oartifact == ART_CANNOTRAP) obj->otrapped = FALSE;

	emptymsg[0] = '\0';
	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && !(obj && obj->otyp == HANDYBAG) ) {
		You("have no hands!");	/* not `body_part(HAND)' */
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		if (yn("Try to open the container with another part of your body instead? Warning: this can fail and cause paralysis.") == 'y') {
			if (rn2(3) && !polyskillchance()) { 			
				You_feel("a wrenching sensation.");
				if (!rn2(20)) badeffect();
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net nikakoy zashchity. Tam net nikakoy nadezhdy. Yedinstvennoye, chto yest'? Uverennost' v tom, chto vy, igrok, budet umeret' uzhasnoy i muchitel'noy smert'yu." : "SCHRING!");
				if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
				flags.soundok = 0;
				if (isstunfish) nomul(-rnz(10), "wrenched in a container", TRUE);
				else nomul(-rnd(10), "wrenched in a container", TRUE);
				nomovemsg = "You are conscious again.";
				afternmv = Hear_again;
				return 1;
			}
		}
		else {return(0);}

	} else if (!freehandX() && !(obj->oartifact == ART_MINNIE_S_HANDBAG) && !(Role_if(PM_CELLAR_CHILD) && uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) ) {
		You("have no free %s.", body_part(HAND));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}
	if (obj->olocked) {

	    if (uwep && uwep->oartifact == ART_FINAL_DOOR_SOLUTION) {
		obj->olocked = FALSE;
		pline("kloeck!");
	    } else {
		pline("%s to be locked.", Tobjnam(obj, "seem"));
		if (held) You("must put it down to unlock.");
		return 0;
	    }
	}
	if (obj->otrapped) {
	    if (held) You("open %s...", the(xname(obj)));
	    if (Role_if(PM_CYBERNINJA) && rn2(5)) {
		You("discover a trap on %s and disarm it.", the(xname(obj)));
		obj->otrapped = FALSE;
		return 1;
	    }
	    (void) chest_trap(obj, HAND, FALSE);
	    /* even if the trap fails, you've used up this turn */
	    if (multi >= 0) {	/* in case we didn't become paralyzed */
		nomul(-1, "opening a trapped container", TRUE);
		nomovemsg = "";
	    }
	    return 1;
	}

	current_container = obj;	/* for use by in/out_container */
	/* from here on out, all early returns go through containerdone */

	if (obj->oartifact == ART_ELECTRONIC_LOCK) {
		if (ContainerKaboom < 10000) ContainerKaboom = 10000;
	}

	if (obj->oartifact == ART_DEMONSEAL) {
		if (obj->invoketimer <= monstermoves) {
			int pm;

			pm = rn2(2) ? dprince(rn2((int)A_LAWFUL+2) - 1) : dlord(rn2((int)A_LAWFUL+2) - 1);
			if (pm >= PM_ORCUS && pm <= PM_DEMOGORGON) u.conclusiocount++;
			if (pm && (pm != NON_PM)) {
				(void) makemon(&mons[pm], u.ux, u.uy, MM_ADJACENTOK|MM_ANGRY|MM_FRENZIED);
				You("triggered an ancient trap on this container...");
			}

			int artitimeout = rnz(2000);
			if (!rn2(5)) artitimeout = rnz(20000); /* squeaking does not help here, as it's not an actual invoke --Amy */
			obj->invoketimer = (monstermoves + artitimeout);
		}
	}

	if (obj->spe == 1) {
	    observe_quantum_cat(obj);
	    used = 1;
	    quantum_cat = TRUE;	/* for adjusting "it's empty" message */
	}
	/* [ALI] If a container vanishes which contains indestructible
	 * objects then these will be added to the magic bag. This makes
	 * it very hard to combine the count and vanish loops so we do
	 * them seperately.
	 */
	/* Sometimes toss objects if a cursed magic bag or a bag of digestion */
	if (Is_mbag(obj) && obj->cursed) {
	    for (curr = obj->cobj; curr; curr = otmp) {
		otmp = curr->nobj;
		if (!rn2(13) && !evades_destruction(curr) && !stack_too_big(curr)) {
		    if (obj && obj->oartifact == ART_RECYCLER_BIN) {
			adjalign(1);
			if (curr->oartifact) {
				adjalign(50);
				u.alignlim += 5;
				pline("Wow, that must have been expensive!");
			}
		    }
		    if (obj && obj->oartifact == ART_MONSTERATOR) {
			monsterator++;
		    }
		    loss += mbag_item_gone(held, curr, TRUE);
		    used = 1;
		}
	    }
	}
	if ( (obj->otyp == BAG_OF_DIGESTION || obj->otyp == ICE_BOX_OF_DIGESTION || obj->otyp == LARGE_BOX_OF_DIGESTION) && !rn2(obj->blessed ? 20 : (obj->cursed ? 2 : 10))) {
	    for (curr = obj->cobj; curr; curr = otmp) {
		otmp = curr->nobj;
		if (!evades_destruction(curr) && !stack_too_big(curr)) {
		    if (obj && obj->oartifact == ART_RECYCLER_BIN) {
			adjalign(1);
			if (curr->oartifact) {
				adjalign(50);
				u.alignlim += 5;
				pline("Wow, that must have been expensive!");
			}
		    }
		    if (obj && obj->oartifact == ART_MONSTERATOR) {
			monsterator++;
		    }
		    loss += mbag_item_gone(held, curr, TRUE);
		    used = 1;
		}
	    }
	}
	/* Count the number of contained objects. */
	for (curr = obj->cobj; curr; curr = curr->nobj)
	    cnt++;

	if (loss)	/* magic bag lost some shop goods */
	    You("owe %ld %s for lost merchandise.", loss, currency(loss));
	obj->owt = weight(obj);	/* in case any items were lost */
  
	if (monsterator > 10) {

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if (monsterator > 200) monsterator = 200;

		u.lamefarmer = TRUE;

		while (monsterator > 0) {

			(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
			u.ublesscnt += 50;
			monsterator--;
			if (monsterator < 0) monsterator = 0; /* fail safe */
		}

		u.aggravation = 0;
		u.lamefarmer = FALSE;

	}

	if (ContainerKaboom || u.uprops[CONTAINER_KABOOM].extrinsic || have_containerkaboomstone() || autismweaponcheck(ART_BUMMERWUMMER) || (uball && uball->oartifact == ART_BUMMERWUMMER) ) {
		containerkaboom();
	}

	if (!cnt)
	    sprintf(emptymsg, "%s is %sempty.", Yname2(obj),
		    quantum_cat ? "now " : "");
	if (current_container->otyp == MEDICAL_KIT) {
	    if (!cnt)
		pline("%s", emptymsg);
	    else
		(void) display_cinventory(current_container);
	    return 0;
	}
	if (cnt || (flags.menu_style == MENU_FULL && !InventoryDoesNotGo)) {
	    strcpy(qbuf, "Do you want to take something out of ");
	    sprintf(eos(qbuf), "%s?",
		    safe_qbuf(qbuf, 1, yname(obj), ysimple_name(obj), "it"));
	    if (flags.menu_style != MENU_TRADITIONAL && !InventoryDoesNotGo) {
		if (flags.menu_style == MENU_FULL) {
		    int t;
		    char menuprompt[BUFSZ];
		    boolean outokay = (cnt != 0),
			    inokay = (invent != 0);

#ifndef GOLDOBJ
		    if (u.ugold) inokay = TRUE;
#endif
		    if (!outokay && !inokay) {
			pline("%s", emptymsg);
			You("don't have anything to put in.");
			goto containerdone;
		    }
		    menuprompt[0] = '\0';
		    if (!cnt) sprintf(menuprompt, "%s ", emptymsg);
		    strcat(menuprompt, "Do what?");
		    t = in_or_out_menu(menuprompt, current_container,
				       outokay, inokay);
		    if (t <= 0) {
			used = 0;
			goto containerdone;
		    }
		    loot_out = (t & 0x01) != 0;
		    loot_in  = (t & 0x02) != 0;
		    loot_reverse = 0;
		    if (t & 0x04) {
			loot_out = loot_in = loot_reverse = 1;
		    }
		} else {	/* MENU_COMBINATION or MENU_PARTIAL */
		    loot_out = (yn_function(qbuf, "ynq", 'n') == 'y');
		}
		if (loot_out && !loot_reverse) {
		    add_valid_menu_class(0);	/* reset */
		    used |= menu_loot(0, current_container, FALSE) > 0;
		}
	    } else {
		/* traditional code */
ask_again2:
		menu_on_request = 0;
		add_valid_menu_class(0);	/* reset */
		strcpy(pbuf, ":ynq");
		if (cnt) strcat(pbuf, "m");
		switch (yn_function(qbuf, pbuf, 'n')) {
		case ':':
		    if (InventoryDoesNotGo) {
			pline("Not enough memory to create inventory window");
			goto ask_again2;
		    }
		    container_contents(current_container, FALSE, FALSE);
		    goto ask_again2;
		case 'y':
		    if (query_classes(select, &one_by_one, &allflag,
				      "take out", current_container->cobj,
				      FALSE,
#ifndef GOLDOBJ
				      FALSE,
#endif
				      &menu_on_request)) {
			if (askchain((struct obj **)&current_container->cobj,
				     (one_by_one ? (char *)0 : select),
				     allflag, out_container,
				     (int (*)(OBJ_P))0,
				     0, "nodot")) {
				if (current_container->oartifact != ART_GITTA_S_HANDBAG && !(uarmc && uarmc->otyp == UTILITY_CLOAK && rn2(2)) && current_container->oartifact != ART_GIDDEM_FAST_) {
				    used = 1;
				}
			}
		    } else if (menu_on_request < 0) {
			used |= menu_loot(menu_on_request,
					  current_container, FALSE) > 0;
		    }
		    /*FALLTHRU*/
		case 'n':
		    break;
		case 'm':
		    menu_on_request = -2; /* triggers ALL_CLASSES */
		    used |= menu_loot(menu_on_request, current_container, FALSE) > 0;
		    break;
		case 'q':
		default:
		    goto containerdone;
		}
	    }
	} else {
	    pline("%s", emptymsg);		/* <whatever> is empty. */
	}

#ifndef GOLDOBJ
	if (!invent && u.ugold == 0) {
#else
	if (!invent) {
#endif
	    /* nothing to put in, but some feedback is necessary */
	    You("don't have anything to put in.");
	    goto containerdone;
	}
	if (flags.menu_style != MENU_FULL || InventoryDoesNotGo) {
	    sprintf(qbuf, "Do you wish to put %s in?", something);
	    strcpy(pbuf, ynqchars);
	    if ((flags.menu_style == MENU_TRADITIONAL || InventoryDoesNotGo) && invent && inv_cnt() > 0)
		strcat(pbuf, "m");
	    switch (yn_function(qbuf, pbuf, 'n')) {
		case 'y':
		    loot_in = TRUE;
		    break;
		case 'n':
		    break;
		case 'm':
		    add_valid_menu_class(0);	  /* reset */
		    menu_on_request = -2; /* triggers ALL_CLASSES */
		    used |= menu_loot(menu_on_request, current_container, TRUE) > 0;
		    break;
		case 'q':
		default:
		    goto containerdone;
	    }
	}
	/*
	 * Gone: being nice about only selecting food if we know we are
	 * putting things in an ice chest.
	 */
	if (loot_in) {
#ifndef GOLDOBJ
	    if (u.ugold) {
		/*
		 * Hack: gold is not in the inventory, so make a gold object
		 * and put it at the head of the inventory list.
		 */
		u_gold = mkgoldobj(u.ugold);	/* removes from u.ugold */
		u_gold->in_use = TRUE;
		u.ugold = u_gold->quan;		/* put the gold back */
		assigninvlet(u_gold);		/* might end up as NOINVSYM */
		u_gold->nobj = invent;
		invent = u_gold;
	    }
#endif
	    add_valid_menu_class(0);	  /* reset */
	    if (flags.menu_style != MENU_TRADITIONAL && !InventoryDoesNotGo) {
		used |= menu_loot(0, current_container, TRUE) > 0;
	    } else {
		/* traditional code */
		menu_on_request = 0;
		if (query_classes(select, &one_by_one, &allflag, "put in",
				   invent, FALSE,
#ifndef GOLDOBJ
				   (u.ugold != 0L),
#endif
				   &menu_on_request)) {
		    (void) askchain((struct obj **)&invent,
				    (one_by_one ? (char *)0 : select), allflag,
				    in_container, ck_bag, 0, "nodot");
		    if (current_container->oartifact != ART_KLARABELLA_S_HANDBAG && !(uarmc && uarmc->otyp == UTILITY_CLOAK && rn2(2)) && current_container->oartifact != ART_STASHIT) {
			used = 1;
		    }
		} else if (menu_on_request < 0) {
		    used |= menu_loot(menu_on_request,
				      current_container, TRUE) > 0;
		}
	    }
	}

#ifndef GOLDOBJ
	if (u_gold && invent && invent->oclass == COIN_CLASS) {
	    /* didn't stash [all of] it */
	    u_gold = invent;
	    invent = u_gold->nobj;
	    u_gold->in_use = FALSE;
	    dealloc_obj(u_gold);
	}
#endif

	if (loot_out && loot_reverse) {
	    add_valid_menu_class(0);	/* reset */
	    used |= menu_loot(0, current_container, FALSE) > 0;
	}

#ifndef GOLDOBJ
	if (u_gold && invent && invent->oclass == COIN_CLASS) {
	    /* didn't stash [all of] it */
	    u_gold = invent;
	    invent = u_gold->nobj;
	    u_gold->in_use = FALSE;
	    dealloc_obj(u_gold);
	}
#endif

 containerdone:
	*objp = current_container;	/* might have become null */
	current_container = 0;		/* avoid hanging on to stale pointer */
	return used;
}

/* Loot a container (take things out, put things in), using a menu. */
STATIC_OVL int
menu_loot(retry, container, put_in)
int retry;
struct obj *container;
boolean put_in;
{
    int n, i, n_looted = 0;
    boolean all_categories = TRUE, loot_everything = FALSE;
    char buf[BUFSZ];
    const char *takeout = "Take out", *putin = "Put in";
    struct obj *otmp, *otmp2;
    menu_item *pick_list;
    int mflags, res;
    long count;

    if (retry) {
	all_categories = (retry == -2);
    } else if (flags.menu_style == MENU_FULL && !InventoryDoesNotGo) {
	all_categories = FALSE;
	sprintf(buf,"%s what type of objects?", put_in ? putin : takeout);
	mflags = put_in ? ALL_TYPES | NOTFULLYIDED | BUC_ALLBKNOWN | BUC_UNKNOWN :
		          ALL_TYPES | NOTFULLYIDED | CHOOSE_ALL | BUC_ALLBKNOWN | BUC_UNKNOWN;
	n = query_category(buf, put_in ? invent : container->cobj,
			   mflags, &pick_list, PICK_ANY);
	if (!n) return 0;
	for (i = 0; i < n; i++) {
	    if (pick_list[i].item.a_int == 'A') {
		if (yn("You decided to autoselect everything, please confirm with y if that's what you really wanted") == 'y') loot_everything = TRUE;
	    } else if (pick_list[i].item.a_int == ALL_TYPES_SELECTED)
		all_categories = TRUE;
	    else
		add_valid_menu_class(pick_list[i].item.a_int);
	}
	free((void *) pick_list);
    }

    if (loot_everything) {
	for (otmp = container->cobj; otmp; otmp = otmp2) {
	    otmp2 = otmp->nobj;
	    res = out_container(otmp);
	    if (res < 0) break;
	}
    } else {
	mflags = INVORDER_SORT;
	if (put_in && flags.invlet_constant) mflags |= USE_INVLET;
	sprintf(buf,"%s what?", put_in ? putin : takeout);
	n = query_objlist(buf, put_in ? invent : container->cobj,
			  mflags, &pick_list, PICK_ANY,
			  all_categories ? allow_all : allow_category);
	if (n) {
		n_looted = n;
		for (i = 0; i < n; i++) {
		    otmp = pick_list[i].item.a_obj;
		    count = pick_list[i].count;
		    if (count > 0 && count < otmp->quan) {
			otmp = splitobj(otmp, count);
			/* special split case also handled by askchain() */
		    }
		    res = put_in ? in_container(otmp) : out_container(otmp);
		    if (res < 0) {
			if (!current_container) {
			    /* otmp caused current_container to explode;
			       both are now gone */
			    otmp = 0;		/* and break loop */
			} else if (otmp && otmp != pick_list[i].item.a_obj) {
			    /* split occurred, merge again */
			    (void) merged(&pick_list[i].item.a_obj, &otmp);
			}
			break;
		    }
		}
		free((void *)pick_list);
	}
    }

    if (uarmc && uarmc->otyp == UTILITY_CLOAK && rn2(2)) return 0;

    if (container && container->oartifact == ART_GITTA_S_HANDBAG && !put_in) return 0;
    if (container && container->oartifact == ART_GIDDEM_FAST_ && !put_in) return 0;
    if (container && container->oartifact == ART_KLARABELLA_S_HANDBAG && put_in) return 0;
    if (container && container->oartifact == ART_STASHIT && put_in) return 0;
    return n_looted;
}

STATIC_OVL int
in_or_out_menu(prompt, obj, outokay, inokay)
const char *prompt;
struct obj *obj;
boolean outokay, inokay;
{
    winid win;
    anything any;
    menu_item *pick_list;
    char buf[BUFSZ];
    int n;
    const char *menuselector = iflags.lootabc ? "abcd" : "oibr";

    any.a_void = 0;
    win = create_nhwindow(NHW_MENU);
    start_menu(win);
    if (outokay) {
	any.a_int = 1;
	sprintf(buf,"Take %s out of %s", something, the(xname(obj)));
	add_menu(win, NO_GLYPH, &any, *menuselector, 0, ATR_NONE,
			buf, MENU_UNSELECTED);
    }
    menuselector++;
    if (inokay) {
	any.a_int = 2;
	sprintf(buf,"Put %s into %s", something, the(xname(obj)));
	add_menu(win, NO_GLYPH, &any, *menuselector, 0, ATR_NONE, buf, MENU_UNSELECTED);
    }
    menuselector++;
    if (outokay && inokay) {
	any.a_int = 3;
	add_menu(win, NO_GLYPH, &any, *menuselector, 0, ATR_NONE,
			"Both of the above, out first", MENU_UNSELECTED);
    }
    menuselector++;
    if (outokay && inokay) {
	any.a_int = 4;
	add_menu(win, NO_GLYPH, &any, *menuselector, 0, ATR_NONE,
			"Both of the above, in first", MENU_UNSELECTED);
    }
    end_menu(win, prompt);
    n = select_menu(win, PICK_ONE, &pick_list);
    destroy_nhwindow(win);
    if (n > 0) {
	n = pick_list[0].item.a_int;
	free((void *) pick_list);
    }
    return n;
}

/* Dumps out a container, possibly as the prelude/result of an explosion.
 * destroy_after trashes the container afterwards; try not to use it :P
 *
 * Player is assumed to not be handling the contents directly.
 *
 * Returns 1 if at least one object was present, 0 if empty.
 */ 
int
dump_container(container, destroy_after, coordx, coordy)
struct obj* container;
BOOLEAN_P destroy_after;
int coordx, coordy;
{
	if (!isok(coordx, coordy)) {
		impossible("dump_container coordinates %d, %d?", coordx, coordy);
		coordx = u.ux;
		coordy = u.uy; /* fail safe */
	}

	struct obj* otmp,*otmp2;
	int ret = 0;

	/* sanity check */
	if (!container) { return 0; }

	for (otmp = container->cobj; otmp; otmp = otmp2)
	{
		ret = 1;
		otmp2 = otmp->nobj;
		obj_extract_self(otmp);
		container->owt = weight(container);

		/* we do need to start the timer on these */
		if ( (container->otyp == ICE_BOX || container->otyp == DISPERSION_BOX || container->otyp == ICE_BOX_OF_HOLDING || container->otyp == ICE_BOX_OF_WATERPROOFING || container->otyp == ICE_BOX_OF_DIGESTION) && !age_is_relative(otmp) && !is_lightsaber(otmp)) {
			otmp->age = monstermoves - otmp->age;
			otmp->icedobject = TRUE;
			if (otmp->otyp == CORPSE) {
				start_corpse_timeout(otmp);
			}
		}
		place_object(otmp,coordx,coordy);

		if (otmp->otyp == GOLD_PIECE) {
#ifndef GOLDOBJ
			/*dealloc_obj(otmp);*/ /* causes panic, and besides, is it really needed??? --Amy */
#endif
			bot();	/* update character's gold piece count immediately */
		}
	}

	if (destroy_after) {
		if (container->where == OBJ_INVENT) {
			useup(container);
		} else if (obj_here(container, u.ux, u.uy)) {
			useupf(container, container->quan);
		}
	}

	return ret;
}

int
dump_container_tele(container, destroy_after, coordx, coordy)
struct obj* container;
BOOLEAN_P destroy_after;
int coordx, coordy;
{
	if (!isok(coordx, coordy)) {
		impossible("dump_container coordinates %d, %d?", coordx, coordy);
		coordx = u.ux;
		coordy = u.uy; /* fail safe */
	}

	struct obj* otmp,*otmp2;
	int ret = 0;

	/* sanity check */
	if (!container) { return 0; }

	for (otmp = container->cobj; otmp; otmp = otmp2)
	{
		ret = 1;
		otmp2 = otmp->nobj;
		obj_extract_self(otmp);
		container->owt = weight(container);

		/* we do need to start the timer on these */
		if ( (container->otyp == ICE_BOX || container->otyp == DISPERSION_BOX || container->otyp == ICE_BOX_OF_HOLDING || container->otyp == ICE_BOX_OF_WATERPROOFING || container->otyp == ICE_BOX_OF_DIGESTION) && !age_is_relative(otmp) && !is_lightsaber(otmp)) {
			otmp->age = monstermoves - otmp->age;
			otmp->icedobject = TRUE;
			if (otmp->otyp == CORPSE) {
				start_corpse_timeout(otmp);
			}
		}
		place_object(otmp,coordx,coordy);
		rloco(otmp);

		if (otmp && otmp->otyp == GOLD_PIECE) {
#ifndef GOLDOBJ
			/*dealloc_obj(otmp);*/ /* causes panic, and besides, is it really needed??? --Amy */
#endif
			bot();	/* update character's gold piece count immediately */
		}
	}

	if (destroy_after) {
		if (container->where == OBJ_INVENT) {
			useup(container);
		} else if (obj_here(container, u.ux, u.uy)) {
			useupf(container, container->quan);
		}
	}

	return ret;
}

int
dump_container_huro(container, destroy_after, coordx, coordy)
struct obj* container;
BOOLEAN_P destroy_after;
int coordx, coordy;
{
	if (!isok(coordx, coordy)) {
		impossible("dump_container coordinates %d, %d?", coordx, coordy);
		coordx = u.ux;
		coordy = u.uy; /* fail safe */
	}

	struct obj* otmp,*otmp2;
	register struct monst *offmon;
	int ret = 0;

	/* sanity check */
	if (!container) { return 0; }

	for (otmp = container->cobj; otmp; otmp = otmp2)
	{
		ret = 1;
		otmp2 = otmp->nobj;
		obj_extract_self(otmp);
		container->owt = weight(container);

		/* we do need to start the timer on these */
		if ( (container->otyp == ICE_BOX || container->otyp == DISPERSION_BOX || container->otyp == ICE_BOX_OF_HOLDING || container->otyp == ICE_BOX_OF_WATERPROOFING || container->otyp == ICE_BOX_OF_DIGESTION) && !age_is_relative(otmp) && !is_lightsaber(otmp)) {
			otmp->age = monstermoves - otmp->age;
			otmp->icedobject = TRUE;
			if (otmp->otyp == CORPSE) {
				start_corpse_timeout(otmp);
			}
		}
		if ((offmon = makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS)) != 0) {
			(void) mpickobj(offmon,otmp,FALSE);
			mdrop_special_objs(offmon); /* don't levelport ones that have the amulet! */
			u_teleport_monB(offmon, FALSE);
		} else {
			place_object(otmp,coordx,coordy);
		}

		if (otmp && otmp->otyp == GOLD_PIECE) {
#ifndef GOLDOBJ
			/*dealloc_obj(otmp);*/ /* causes panic, and besides, is it really needed??? --Amy */
#endif
			bot();	/* update character's gold piece count immediately */
		}
	}

	if (destroy_after) {
		if (container->where == OBJ_INVENT) {
			useup(container);
		} else if (obj_here(container, u.ux, u.uy)) {
			useupf(container, container->quan);
		}
	}

	return ret;
}

int
dump_container_superhuro(container, destroy_after, coordx, coordy)
struct obj* container;
BOOLEAN_P destroy_after;
int coordx, coordy;
{
	if (!isok(coordx, coordy)) {
		impossible("dump_container coordinates %d, %d?", coordx, coordy);
		coordx = u.ux;
		coordy = u.uy; /* fail safe */
	}

	struct obj* otmp,*otmp2;
	register struct monst *offmon;
	int ret = 0;

	/* sanity check */
	if (!container) { return 0; }

	for (otmp = container->cobj; otmp; otmp = otmp2)
	{
		ret = 1;
		otmp2 = otmp->nobj;
		obj_extract_self(otmp);
		container->owt = weight(container);

		/* we do need to start the timer on these */
		if ( (container->otyp == ICE_BOX || container->otyp == DISPERSION_BOX || container->otyp == ICE_BOX_OF_HOLDING || container->otyp == ICE_BOX_OF_WATERPROOFING || container->otyp == ICE_BOX_OF_DIGESTION) && !age_is_relative(otmp) && !is_lightsaber(otmp)) {
			otmp->age = monstermoves - otmp->age;
			otmp->icedobject = TRUE;
			if (otmp->otyp == CORPSE) {
				start_corpse_timeout(otmp);
			}
		}
		if ((offmon = makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS)) != 0) {
			(void) mpickobj(offmon,otmp,FALSE);
			mdrop_special_objs(offmon); /* don't levelport ones that have the amulet! */
			u_teleport_monC(offmon, FALSE);
		} else {
			place_object(otmp,coordx,coordy);
		}

		if (otmp && otmp->otyp == GOLD_PIECE) {
#ifndef GOLDOBJ
			/*dealloc_obj(otmp);*/ /* causes panic, and besides, is it really needed??? --Amy */
#endif
			bot();	/* update character's gold piece count immediately */
		}
	}

	if (destroy_after) {
		if (container->where == OBJ_INVENT) {
			useup(container);
		} else if (obj_here(container, u.ux, u.uy)) {
			useupf(container, container->quan);
		}
	}

	return ret;
}

/*pickup.c*/


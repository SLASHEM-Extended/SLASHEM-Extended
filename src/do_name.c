/*	SCCS Id: @(#)do_name.c	3.4	2003/01/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef OVLB

STATIC_DCL void do_oname(struct obj *);
static void getpos_help(BOOLEAN_P,const char *);

extern const char what_is_an_unknown_object[];		/* from pager.c */

/* the response for '?' help request in getpos() */
static void
getpos_help(force, goal)
boolean force;
const char *goal;
{
    char sbuf[BUFSZ];
    boolean doing_what_is;
    winid tmpwin = create_nhwindow(NHW_MENU);

    sprintf(sbuf, "Use [%s] to move the cursor to %s.",
	    iflags.num_pad ? "2468" : "hjkl", goal);
    putstr(tmpwin, 0, sbuf);
    putstr(tmpwin, 0, "Use [HJKL] to move the cursor 8 units at a time.");
    putstr(tmpwin, 0, "Or enter a background symbol (ex. <).");
    /* disgusting hack; the alternate selection characters work for any
       getpos call, but they only matter for dowhatis (and doquickwhatis) */
    doing_what_is = (goal == what_is_an_unknown_object);
    sprintf(sbuf, "Type a .%s when you are at the right place.",
            doing_what_is ? " or , or ; or :" : "");
    putstr(tmpwin, 0, sbuf);
    if (!force)
	putstr(tmpwin, 0, "Type Space or Escape when you're done.");
    else
	putstr(tmpwin, 0, "Press Escape when you're done.");
    putstr(tmpwin, 0, "");
    display_nhwindow(tmpwin, TRUE);
    destroy_nhwindow(tmpwin);
}

int
getpos(cc,force,goal)
coord *cc;
boolean force;
const char *goal;
{
    int result = 0;
    int cx, cy, i, c;
    int sidx, tx, ty;
    boolean msg_given = TRUE;	/* clear message window by default */
    static const char pick_chars[] = ".,;:";
    const char *cp;
    const char *sdp;
    if(iflags.num_pad) sdp = ndir; else sdp = sdir;	/* DICE workaround */

    if (flags.verbose) {
	pline("(For instructions type a ?)");
	msg_given = TRUE;
    }

    cx = cc->x;
    cy = cc->y;

    if (!isok(cx, cy) && isok(u.ux, u.uy)) {
	cx = u.ux;
	cy = u.uy;
    }

#ifdef CLIPPING
    cliparound(cx, cy);
#endif
    curs(WIN_MAP, cx,cy);
    flush_screen(0);
#ifdef MAC
    lock_mouse_cursor(TRUE);
#endif
    for (;;) {
	c = nh_poskey(&tx, &ty, &sidx);
	if (c == '\033') {
	    cx = cy = -10;
	    msg_given = TRUE;	/* force clear */
	    result = -1;
	    break;
	}
	if(c == 0) {
	    if (!isok(tx, ty)) continue;
	    /* a mouse click event, just assign and return */
	    cx = tx;
	    cy = ty;
	    break;
	}
	if ((cp = index(pick_chars, c)) != 0) {
	    /* '.' => 0, ',' => 1, ';' => 2, ':' => 3 */
	    result = cp - pick_chars;
	    break;
	}
	for (i = 0; i < 8; i++) {
	    int dx, dy;

	    if (sdp[i] == c) {
		/* a normal movement letter or digit */
		dx = xdir[i];
		dy = ydir[i];
	    } else if (sdir[i] == lowc((char)c)) {
		/* a shifted movement letter */
		dx = 8 * xdir[i];
		dy = 8 * ydir[i];
	    } else
		continue;

	    /* truncate at map edge; diagonal moves complicate this... */
	    if (cx + dx < 1) {
		dy -= sgn(dy) * (1 - (cx + dx));
		dx = 1 - cx;		/* so that (cx+dx == 1) */
	    } else if (cx + dx > COLNO-1) {
		dy += sgn(dy) * ((COLNO-1) - (cx + dx));
		dx = (COLNO-1) - cx;
	    }
	    if (cy + dy < 0) {
		dx -= sgn(dx) * (0 - (cy + dy));
		dy = 0 - cy;		/* so that (cy+dy == 0) */
	    } else if (cy + dy > ROWNO-1) {
		dx += sgn(dx) * ((ROWNO-1) - (cy + dy));
		dy = (ROWNO-1) - cy;
	    }
	    cx += dx;
	    cy += dy;
	    goto nxtc;
	}

	if(c == '?'){
	    getpos_help(force, goal);
	} else {
	    if (!index(quitchars, c)) {
		char matching[MAXPCHARS];
		int pass, lo_x, lo_y, hi_x, hi_y, k = 0;
		(void)memset((void *)matching, 0, sizeof matching);
		for (sidx = 1; sidx < MAXPCHARS; sidx++)
		    if (c == defsyms[sidx].sym || c == (int)showsyms[sidx])
			matching[sidx] = (char) ++k;
		if (k) {
		    for (pass = 0; pass <= 1; pass++) {
			/* pass 0: just past current pos to lower right;
			   pass 1: upper left corner to current pos */
			lo_y = (pass == 0) ? cy : 0;
			hi_y = (pass == 0) ? ROWNO - 1 : cy;
			for (ty = lo_y; ty <= hi_y; ty++) {
			    lo_x = (pass == 0 && ty == lo_y) ? cx + 1 : 1;
			    hi_x = (pass == 1 && ty == hi_y) ? cx : COLNO - 1;
			    for (tx = lo_x; tx <= hi_x; tx++) {
				k = glyph_at(tx, ty);
				if (glyph_is_cmap(k) &&
					matching[glyph_to_cmap(k)]) {
				    cx = tx,  cy = ty;
				    if (msg_given) {
					clear_nhwindow(WIN_MESSAGE);
					msg_given = FALSE;
				    }
				    goto nxtc;
				}
			    }	/* column */
			}	/* row */
		    }		/* pass */
		    pline("Can't find dungeon feature '%c'.", c);
		    msg_given = TRUE;
		    goto nxtc;
		} else {
		    pline("Unknown direction: '%s' (%s).",
			  visctrl((char)c),
			  !force ? "aborted" :
			  iflags.num_pad ? "use 2468 or ." : "use hjkl or .");
		    msg_given = TRUE;
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		} /* k => matching */
	    } /* !quitchars */
	    if (force && !(c == 32) ) goto nxtc;
	    pline("Done.");
	    msg_given = FALSE;	/* suppress clear */
	    cx = -1;
	    cy = 0;
	    result = 0;	/* not -1 */
	    if (c == 32) result = -1;
	    break;
	}
    nxtc:	;
#ifdef CLIPPING
	cliparound(cx, cy);
#endif
	curs(WIN_MAP,cx,cy);
	flush_screen(0);
    }
#ifdef MAC
    lock_mouse_cursor(FALSE);
#endif
    if (msg_given) clear_nhwindow(WIN_MESSAGE);
    cc->x = cx;
    cc->y = cy;

    return result;

}

struct monst *
christen_monst(mtmp, name)
struct monst *mtmp;
const char *name;
{
	int lth;
	struct monst *mtmp2;
	char buf[PL_PSIZ];

	/* dogname & catname are PL_PSIZ arrays; object names have same limit */
	lth = *name ? (int)(strlen(name) + 1) : 0;
	if(lth > PL_PSIZ){
		lth = PL_PSIZ;
		name = strncpy(buf, name, PL_PSIZ - 1);
		buf[PL_PSIZ - 1] = '\0';
	}
	if (lth == mtmp->mnamelth) {
		/* don't need to allocate a new monst struct */
		if (lth) strcpy(NAME(mtmp), name);
		return mtmp;
	}
	mtmp2 = newmonst(mtmp->mxlth + lth);
	*mtmp2 = *mtmp;
	(void) memcpy((void *)mtmp2->mextra,
		      (void *)mtmp->mextra, mtmp->mxlth);
	mtmp2->mnamelth = lth;
	if (lth) strcpy(NAME(mtmp2), name);
	replmon(mtmp,mtmp2);
	return(mtmp2);
}

int
do_mname()
{
	char buf[BUFSZ];
	coord cc;
	register int cx,cy;
	register struct monst *mtmp;
	char qbuf[QBUFSZ];

	if (UncalledEffect || u.uprops[UNCALLED_EFFECT].extrinsic || have_uncalledstone()) return 0;

	if (Hallucination) {
		You("would never recognize it anyway.");
		return 0;
	}
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, FALSE, "the monster you want to name") < 0 ||
			(cx = cc.x) < 0)
		return 0;
	cy = cc.y;

	if (cx == u.ux && cy == u.uy) {
	    if (u.usteed && canspotmon(u.usteed))
		mtmp = u.usteed;
	    else {
		pline("This %s creature is called %s and cannot be renamed.",
		ACURR(A_CHA) > 14 ? (flags.female ? "beautiful" : "handsome") : "ugly", playeraliasname);
		return(0);
	    }
	} else
	    mtmp = m_at(cx, cy);

	if (!mtmp || (!sensemon(mtmp) &&
			(!(cansee(cx,cy) || see_with_infrared(mtmp)) || mtmp->mundetected
			|| mtmp->m_ap_type == M_AP_FURNITURE
			|| mtmp->m_ap_type == M_AP_OBJECT
			|| mtmp->minvisreal
			|| (mtmp->minvis && (!See_invisible || (!StrongSee_invisible && !mtmp->seeinvisble) ) )))) {
		pline("I see no monster there.");
		return(0);
	}
	/* special case similar to the one in lookat() */
	(void) distant_monnam(mtmp, ARTICLE_THE, buf);
	sprintf(qbuf, "What do you want to call %s?", buf);
	getlin(qbuf,buf);
	if(!*buf || *buf == '\033') return(0);
	/* strip leading and trailing spaces; unnames monster if all spaces */
	(void)mungspaces(buf);

	if (Hallucination)
	    pline("You wouldn't recognize it again anyway!");
	else if ( mtmp->data == &mons[PM_HIGH_PRIEST] || mtmp->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_] )
	    pline("Abusing the astral call bug, huh, cheater? That's not gonna work anymore! --Amy");
	else if (mtmp->data->geno & G_UNIQ)
	    pline("%s doesn't like being called names!", Monnam(mtmp));
	else if (mtmp->mnamelth && !mtmp->mtame)
	    pline("%s doesn't like you enough to allow you to rename %s!", Monnam(mtmp), mhim(mtmp));
	else if (!(strcmp(buf, "Glorious Dead") ) )
	    pline("That is an invalid name.");
	else
	    (void) christen_monst(mtmp, buf);
	return(0);
}

/*
 * This routine changes the address of obj. Be careful not to call it
 * when there might be pointers around in unknown places. For now: only
 * when obj is in the inventory.
 */
STATIC_OVL
void
do_oname(obj)
register struct obj *obj;
{
	char buf[BUFSZ], qbuf[QBUFSZ];
	const char *aname;
	/*short*/int objtyp;

	sprintf(qbuf, "What do you want to name %s %s?",
		is_plural(obj) ? "these" : "this", xname(obj));
	getlin(qbuf, buf);
	if(!*buf || *buf == '\033')	return;
	/* strip leading and trailing spaces; unnames item if all spaces */
	(void)mungspaces(buf);

	/* relax restrictions over proper capitalization for artifacts */
	if ((aname = artifact_name(buf, &objtyp)) != 0 && objtyp == obj->otyp)
		strcpy(buf, aname);

	if (obj->oartifact) {
		pline_The("artifact seems to resist the attempt.");
		return;
	} else if (obj->fakeartifact) {
		/* It was lame to be able to sort out fake artifacts by naming them, and besides, I made up so many beautiful
		 * names for all the fake artifacts! You're not supposed to overwrite them with some gibberish! --Amy */
		pline_The("artifact seems to resist the attempt.");
		return;
	} else if (obj->otyp == AMULET_OF_YENDOR || obj->otyp == FAKE_AMULET_OF_YENDOR) {
		pline("You cannot rename such a powerful object.");
		return;
	} else if (restrict_name(obj, buf) || exist_artifact(obj->otyp, buf)) {
		int n = rn2((int)strlen(buf));
		register char c1, c2;

		c1 = lowc(buf[n]);
		do c2 = 'a' + rn2('z'-'a'); while (c1 == c2);
		buf[n] = (buf[n] == c1) ? c2 : highc(c2);  /* keep same case */
		pline("While engraving your %s slips.", body_part(HAND));
		display_nhwindow(WIN_MESSAGE, FALSE);
		You("engrave: \"%s\".",buf);
	}
	obj = oname(obj, buf);
}

/*
 * Allocate a new and possibly larger storage space for an obj.
 */
struct obj *
realloc_obj(obj, oextra_size, oextra_src, oname_size, name)
struct obj *obj;
int oextra_size;		/* storage to allocate for oextra            */
void * oextra_src;
int oname_size;			/* size of name string + 1 (null terminator) */
const char *name;
{
	struct obj *otmp;

	otmp = newobj(oextra_size + oname_size);
	*otmp = *obj;	/* the cobj pointer is copied to otmp */
	if (oextra_size) {
	    if (oextra_src)
		(void) memcpy((void *)otmp->oextra, oextra_src,
							oextra_size);
	} else {
	    otmp->oattached = OATTACHED_NOTHING;
	}
	otmp->oxlth = oextra_size;

	otmp->onamelth = oname_size;
	otmp->timed = 0;	/* not timed, yet */
	otmp->lamplit = 0;	/* ditto */
	/* __GNUC__ note:  if the assignment of otmp->onamelth immediately
	   precedes this `if' statement, a gcc bug will miscompile the
	   test on vax (`insv' instruction used to store bitfield does
	   not set condition codes, but optimizer behaves as if it did).
	   gcc-2.7.2.1 finally fixed this.... */
	if (oname_size) {
	    if (name)
		strcpy(ONAME(otmp), name);
	}

	if (obj->owornmask) {
		boolean save_twoweap = u.twoweap;
		/* unwearing the old instance will clear dual-wield mode
		   if this object is either of the two weapons */
		setworn((struct obj *)0, obj->owornmask);
		setworn(otmp, otmp->owornmask);
		u.twoweap = save_twoweap;
	}

	/* replace obj with otmp */
	replace_object(obj, otmp);

	/* fix ocontainer pointers */
	if (Has_contents(obj)) {
		struct obj *inside;

		for(inside = obj->cobj; inside; inside = inside->nobj)
			inside->ocontainer = otmp;
	}

	/* move timers and light sources from obj to otmp */
	if (obj->timed) obj_move_timers(obj, otmp);
	if (obj->lamplit) obj_move_light_source(obj, otmp);

	/* objects possibly being manipulated by multi-turn occupations
	   which have been interrupted but might be subsequently resumed */
	if (obj->oclass == FOOD_CLASS)
	    food_substitution(obj, otmp);	/* eat food or open tin */
	else if (obj->oclass == SPBOOK_CLASS)
	    book_substitution(obj, otmp);	/* read spellbook */

	/* obfree(obj, otmp);	now unnecessary: no pointers on bill */
	dealloc_obj(obj);	/* let us hope nobody else saved a pointer */
	return otmp;
}

struct obj *
oname(obj, name)
struct obj *obj;
const char *name;
{
	int lth;
	char buf[PL_PSIZ];

	lth = *name ? (int)(strlen(name) + 1) : 0;
	if (lth > PL_PSIZ) {
		lth = PL_PSIZ;
		name = strncpy(buf, name, PL_PSIZ - 1);
		buf[PL_PSIZ - 1] = '\0';
	}
	/* If named artifact exists in the game, do not create another.
	 * Also trying to create an artifact shouldn't de-artifact
	 * it (e.g. Excalibur from prayer). In this case the object
	 * will retain its current name. */
	if (obj->oartifact || (lth && exist_artifact(obj->otyp, name)))
		return obj;

	if (lth == obj->onamelth) {
		/* no need to replace entire object */
		if (lth) strcpy(ONAME(obj), name);
	} else {
		obj = realloc_obj(obj, obj->oxlth,
			      (void *)obj->oextra, lth, name);
	}
	if (lth) artifact_exists(obj, name, TRUE);
	if (obj->oartifact) {
	    /* can't dual-wield with artifact as secondary weapon */
	    if (obj == uswapwep) untwoweapon();
	    /* activate warning if you've just named your weapon "Sting" */
	    if (obj == uwep) set_artifact_intrinsic(obj, TRUE, W_WEP);
	}
	if (carried(obj)) update_inventory();
	return obj;
}

struct obj *
onameX(obj, name)
struct obj *obj;
const char *name;
{
	int lth;
	char buf[PL_PSIZ];

	lth = *name ? (int)(strlen(name) + 1) : 0;
	if (lth > PL_PSIZ) {
		lth = PL_PSIZ;
		name = strncpy(buf, name, PL_PSIZ - 1);
		buf[PL_PSIZ - 1] = '\0';
	}
	/* This function allows the artifact to be created again if it already exists.
	 * Still, trying to create an artifact shouldn't de-artifact
	 * it (e.g. Excalibur from prayer). In this case the object
	 * will retain its current name. */
	if (obj->oartifact)
		return obj;

	if (lth == obj->onamelth) {
		/* no need to replace entire object */
		if (lth) strcpy(ONAME(obj), name);
	} else {
		obj = realloc_obj(obj, obj->oxlth,
			      (void *)obj->oextra, lth, name);
	}
	if (lth) artifact_exists(obj, name, TRUE);
	if (obj->oartifact) {
	    /* can't dual-wield with artifact as secondary weapon */
	    if (obj == uswapwep) untwoweapon();
	    /* activate warning if you've just named your weapon "Sting" */
	    if (obj == uwep) set_artifact_intrinsic(obj, TRUE, W_WEP);
	}
	if (carried(obj)) update_inventory();
	return obj;
}

static NEARDATA const char callable[] = {
	SCROLL_CLASS, POTION_CLASS, WAND_CLASS, RING_CLASS, AMULET_CLASS, IMPLANT_CLASS,
	GEM_CLASS, SPBOOK_CLASS, ARMOR_CLASS, TOOL_CLASS, 0 };

int
ddocall()
{
	register struct obj *obj;
	char	ch;
	char allowall[2];

	if (UncalledEffect || u.uprops[UNCALLED_EFFECT].extrinsic || have_uncalledstone()) return 0;

	switch(ch = ynq("Name an individual object?")) {
	case 'q':
		break;
	case 'y':
		savech(ch);
		allowall[0] = ALL_CLASSES; allowall[1] = '\0';
		obj = getobj(allowall, "name");
		if(obj) do_oname(obj);
		break;
	default:
		savech(ch);
		obj = getobj(callable, "call");
		if (obj) {
			/* behave as if examining it in inventory;
			   this might set dknown if it was picked up
			   while blind and the hero can now see */
			(void) xname(obj);

			if (!obj->dknown) {
				You("would never recognize another one.");
				return 0;
			}
			docall(obj);
		}
		break;
	}
	return 0;
}

void
docall(obj)
register struct obj *obj;
{
	char buf[BUFSZ], qbuf[QBUFSZ];
	struct obj otemp;
	register char **str1;

	if (UncalledEffect || u.uprops[UNCALLED_EFFECT].extrinsic || have_uncalledstone()) return;

	if (!obj->dknown) return; /* probably blind */
	otemp = *obj;
	otemp.quan = 1L;
	otemp.onamelth = 0;
	otemp.oxlth = 0;
	if (objects[otemp.otyp].oc_class == POTION_CLASS && otemp.fromsink)
	    /* kludge, meaning it's sink water */
	    sprintf(qbuf,"Call a stream of %s fluid:",
		    OBJ_DESCR(objects[otemp.otyp]));
	else
	    sprintf(qbuf, "Call %s:", an(xname(&otemp)));
	getlin(qbuf, buf);
	if(!*buf || *buf == '\033')
		return;

	/* clear old name */
	str1 = &(objects[obj->otyp].oc_uname);
	if(*str1) free((void *)*str1);

	/* strip leading and trailing spaces; uncalls item if all spaces */
	(void)mungspaces(buf);
	if (!*buf) {
	    if (*str1) {	/* had name, so possibly remove from disco[] */
		/* strip name first, for the update_inventory() call
		   from undiscover_object() */
		*str1 = (char *)0;
		undiscover_object(obj->otyp);
	    }
	} else {
	    *str1 = strcpy((char *) alloc((unsigned)strlen(buf)+1), buf);
	    discover_object(obj->otyp, FALSE, TRUE); /* possibly add to disco[] */
	}
}

#endif /*OVLB*/
#ifdef OVL0

static const char * const ghostnames[] = {
	/* these names should have length < PL_NSIZ */
	/* Capitalize the names for aesthetics -dgk */
	"Adri", "Andries", "Andreas", "Bert", "David", "Dirk", "Emile",
	"Frans", "Fred", "Greg", "Hether", "Jay", "John", "Jon", "Karnov",
	"Kay", "Kenny", "Kevin", "Maud", "Michiel", "Mike", "Peter", "Robert",
	"Ron", "Tom", "Wilmar", "Nick Danger", "Phoenix", "Jiro", "Mizue",
	"Stephan", "Lance Braccus", "Shadowhawk", "GoldenIvy", "AmyBSOD",
	"Dolores", "Mariari", "Lynn", "Emily", "Elenmirie", "Raisse",
	"MiseryMyra", "Lymia", "Naganadel", "Bhaak", "Jonadab", "Ais523",
	"Chris_ANG", "Tungtn", "SGrunt", "Lorimer", "Adeon", "FIQ",
	"Mandevil", "Luxidream", "Noty", "Psymar", "ProzacElf", "Tangles",
	"Yasdorian", "Bugsniper", "Heliokopis", "ShivanHunter", "Khor",
	"Kerio", "Stenno", "Deepy", "Damerell", "Elronnd", "Blindcoder",
	"Ilbelkyr", "GreyKnight", "Paxed", "Crawldragon", "Glycan",
	"QDesjardin", "Demo", "GagarinX", "Introspective", "Grasshopper",
	"Tarmunora", "SourSlime", "Mr0t", "Irrenhaus", "Greqrg", "Bouquet",
	"Lorskel", "Regret", "Tariru", "Goreval", "Winter", "StatueSurfer",
	"Yer Mivvagah", "Arrhythmia", "Tubs", "Winsalot", "Hothraxxa",
	"Aosdict", "Ziratha", "Volt", "Kritixilithos", "Rikersan", "K2",
	"Leeroy", "Hypnotist", "Anerag", "Icerose", "Madotsuki",
	"PeterQ", "PavelB", "FlamingGuacamole", "Dracopent", "AntiGulp",
	"Metanite", "Andrio", "Greyberyl", "Pellsson", "Recluse", "Malena",
	"Pinkbeast", "Mickmane", "Porkman", "Micromoog", "Malor", "Merlek",
	"Musicdemon", "Amateurhour", "Mobileuser", "Aoei", "Rebatela",

};

/* ghost names formerly set by x_monnam(), now by makemon() instead */
const char *
rndghostname()
{
    return rn2(7) ? ghostnames[rn2(SIZE(ghostnames))] : (const char *)plname;
}

/* undead player monsters with names --Amy */

static const char * const plrmonnames[] = {

"Wolf", "Big Bear", "Ryu", "Tacitus", "Urbaldi", "Pete", "Lex", "Denshi Gasu", "Mr. Black", "Tiger's Claw", "Katzou", "Mohmar Deathstrike", "Ingo", "Septimus", "Martius", "Faster-Than-All-Others", "Senator Antius", "H.", "Pokoh", "Davide", "Aee", "Doctor Maex", "Marc", "Arno", "Hailbush", "Romann", "Siegfried", "Roy", "G-cheater", "Bastian", "Nicyan", "Queelix", "Miesael", "Honno", "Robin", "JNR", "Lars", "Tommy", "Giglio", "Kastortransport", "Larry", "Morton", "Iggy", "Lemmy", "Ludwig", "Oberdan", "Len-kind", "Ilie", "Till", "Tomas", "Nikolob", "Tillbull", "Robat", "Robert", "Tobi", "Tobias", "Flo-ooo", "Florian", "Cristi", "Christian", "Alex", "Egas", "Hannes", "Leo", "Leopold", "Baschdi Deathstrike", "Markus", "Martin", "Max", "Maximilian", "Jannik", "Conse", "Constantin", "Paul", "David", "Arne", "Julian", "Sebastian", "Yannick", "Felix", "Michael", "Hanno", "Nino", "Daniel", "Lennart", "Ilja", "Nico", "Tillmann", "Stefan", "Lukas", "Selinger", "Gallardo", "Baenni", "Peeer", "Peeta", "Walter", "Klaus", "Walker", "Nikolei", "Jonas", "Iwan", "Rubinho", "Coffin Nail", "Evillan", "Thilo", "Maurus", "Freddie", "Laurens", "Loorenz", "Jorin", "Rinjo", "Andrej", "Anselm", "Aram", "Boris", "Burkhard", "Nils", "Siln", "Ozan", "Otzan", "Thorwald", "Forestgate", "Dominik", "Albert", "Don Finwe", "Gerrit", "Jens", "Leon", "Marius", "Mirko", "Sigurd", "Wilhelm", 

};

/* it's obvious that I seem to be better at making up female names ;) */

static const char * const plrmonnamesfemale[] = {

"JoJo", "Jyllia", "Sabrina", "Sabine", "Yvara", "Lenka", "Evita", "Liebea", "Isolde", "Elli", "Vilja", "Sunija", "Rhea", "Jasmin", "Erosina", "Irmina", "Melirija", "Larissa", "Sysette", "Miss Haskill", "Elenya", "Golden Mary", "Lara", "Sandrina", "Tonilia", "Claire", "Lumia", "Lahira", "Estrella", "Maricia", "Sontaire", "Marje", "Jill", "Trycja", "Kersey", "Sally", "Hannya", "Svantje", "Jynnifyr", "Elke", "Rinka", "Nicoletta", "Betti", "Ina", "Heikipa", "Jora", "Maitine", "Esruth", "Verene", "Lousie", "Irinella", "Amandina", "Lillie", "Leodoch", "Mirella", "Fisoa", "Suesska", "Ann", "Nurisha", "Desiree", "Birgit", "Elsbeth", "Lamy", "Lissie", "Arabella", "Anastasia", "Henrietta", "Katrin", "Jana", "Aniya", "Yasni", "Almina", "Xeni", "Mirri", "Eleanor", "Kirja", "Inge", "Helli", "Lucia", "Viktorija", "Simona", "Natalyana", "Krista", "Nellina", "Raidara", "Vera", "Noko", "Jasajeen", "Marika", "Merbek", "Marianna", "Sinja", "Rodotha", "Natinya", "Aline", "Michaela", "Mare", "Noenoe", "Tschulia", "Lea", "Sarah", "Iris", "Charravalga", "Fridrika", "Great Jaguar Claw", "Lynette", "Celina", "Irya", "Mariya", "Wendy", "Katia", "Tanja", "Vanessa", "Anne", "Lena", "Jeanetta", "Rungud", "Melissa", "Everella", "Madeleine", "Anita", "Nina", "Natascha", "Manola", "Litta", "Kiwi", "Maja", "Natalje", "Little Marie", "Ronja", "Roswitha", "Sing", "Johanetta", "Julia", "Julchen", "Yvonne", "Magdalena", "Eveline", "Bea", "Beatriz", "Corina", "Elif", "Nadja", "Sunali", "Solvejg", "Thai", "Meltem", "Susanne", "Rita", "Kati", "Katinka", "Mailie", "Marie", "Klara", "Sandra", "Antonia", "Chaska", "Ludgera", "Laura", "Eva", "Maurah", "Sophie", "Marian", "Jil", "Patricia", "Kerstin", "Hanh", "Antje", "Jennifer", "Karin", "Nicole", "Bettina", "Heike", "Dora", "Maite", "Ruth", "Verena", "Lou", "Danielle", "Amandine", "Lily", "Leonie", "Mira", "Sofia", "Christiane", "Ann Kathrin", "Njusha", "Elisabeth", "Conny", "Constanze", "Lisa", "Anja", "Yasaman", "Almut", "Ksenia", "Miriam", "Elena", "Katharina", "Helen", "Victoria", "Simone", "Nataliya", "Kristin", "Nelly", "Rejda", "Nora", "Jasieen", "Yacine", "Marike", "Merle", "Marianne", "Sina", "Dorothea", "Tinanya", "Noemi", "Giulia", "Charlotte", "Friederike", "Sophia", "Sue Lyn", "Juen", "Ruea", "Gudrun", "Ella", "Manuela", "Tilla", "Greta", "Jane", "Celia", "Boese", "Bad", "Eliane", "O'Neill", "Fenja", "Silvana", "Vanasil", "Sarina", "Alexia", "Vida", "Isis", "Ilse", "Melanie", "Lareena", "Janina", "Jannie", "Micha", "Chirin", "Ingrid", "Tonja", "Tapia", "Ligasa", "Andrea", "Mia", "Annemarie", "Caro", "Mandarina", "Ariane", "Carina", "Denise", "Kira", "Nadine", "Franzi", "Amelie", "Annika", "Barbara", "Elsa", "Isabel", "Bianca", "Carmen", "Hilda", "Johanna", "Julietta", "Linda", "Petra", "Sonja", "Stella", "Ismella", "Teresa", "Sagarah", "Rosy",

};

/* the following functions are used by makemon.c */

const char *
rndplrmonname()
{
    return plrmonnames[rn2(SIZE(plrmonnames))];
}

const char *
rndplrmonnamefemale()
{
    return plrmonnamesfemale[rn2(SIZE(plrmonnamesfemale))];
}

/* Monster naming functions:
 * x_monnam is the generic monster-naming function.
 *		  seen	      unseen	   detected		  named
 * mon_nam:	the newt	it	the invisible orc	Fido
 * noit_mon_nam:the newt (as if detected) the invisible orc	Fido
 * l_monnam:	newt		it	invisible orc		dog called fido
 * Monnam:	The newt	It	The invisible orc	Fido
 * noit_Monnam: The newt (as if detected) The invisible orc	Fido
 * Adjmonnam:	The poor newt	It	The poor invisible orc	The poor Fido
 * Amonnam:	A newt		It	An invisible orc	Fido
 * a_monnam:	a newt		it	an invisible orc	Fido
 * a_noit_monnam:	a newt	(as if detected)	an invisible orc	Fido
 * m_monnam:	newt		xan	orc			Fido
 * y_monnam:	your newt     your xan	your invisible orc	Fido
 */

/* Bug: if the monster is a priest or shopkeeper, not every one of these
 * options works, since those are special cases.
 */
char *
x_monnam(mtmp, article, adjective, suppress, called)
register struct monst *mtmp;
int article;
/* ARTICLE_NONE, ARTICLE_THE, ARTICLE_A: obvious
 * ARTICLE_YOUR: "your" on pets, "the" on everything else
 *
 * If the monster would be referred to as "it" or if the monster has a name
 * _and_ there is no adjective, "invisible", "saddled", etc., override this
 * and always use no article.
 */
const char *adjective;
int suppress;
/* SUPPRESS_IT, SUPPRESS_INVISIBLE, SUPPRESS_HALLUCINATION, SUPPRESS_SADDLE.
 * EXACT_NAME: combination of all the above
 */
boolean called;
{
#ifdef LINT	/* static char buf[BUFSZ]; */
	char buf[BUFSZ];
#else
	static char buf[BUFSZ];
#endif
	struct permonst *mdat = mtmp->data;
	boolean do_hallu, do_invis, do_it, do_saddle;
	boolean name_at_start, has_adjectives;
	char *bp;
	int egotypeamount = 0;

	if (program_state.gameover)
	    suppress |= SUPPRESS_HALLUCINATION;
	if (article == ARTICLE_YOUR && !mtmp->mtame)
	    article = ARTICLE_THE;

	do_hallu = Hallucination && !(suppress & SUPPRESS_HALLUCINATION);
	do_invis = mtmp->minvis && !(suppress & SUPPRESS_INVISIBLE);

	do_it = (!canspotmon(mtmp) || (!sensemon(mtmp) && ((is_hider(mtmp->data) || mtmp->egotype_hide || mtmp->egotype_mimic) && (mtmp->mundetected || mtmp->m_ap_type == M_AP_FURNITURE || mtmp->m_ap_type == M_AP_OBJECT)) )) &&
	    article != ARTICLE_YOUR &&
	    !program_state.gameover &&
	    mtmp != u.usteed &&
	    !(u.uswallow && mtmp == u.ustuck) &&
	    !(suppress & SUPPRESS_IT);

stupidsegfault:

	do_saddle = !(suppress & SUPPRESS_SADDLE);

	buf[0] = 0;

	/* unseen monsters, etc.  Use "it" */
	if (do_it) {
	    strcpy(buf, "it");
	    return buf;
	}

	if (Role_if(PM_GENDERSTARIST)) {
		if (is_neuter(mtmp->data)) strcat(buf, "male or female or neuter ");
		else if (mtmp->female) strcat(buf, "female ");
		else strcat(buf, "male ");
	}

	/* priests and minions: don't even use this function */
	if (mtmp->ispriest || mtmp->isminion) {

	    if (StarlitBug || u.uprops[STARLIT_BUG].extrinsic || have_starlitskystone() || (uarmf && uarmf->oartifact == ART_STAR_SOLES) || (uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uarmg && uarmg->oartifact == ART_RAAAAAAAARRRRRRGH) || MonsterGlyphHallu) {
		strcat(buf, "monster");
		return buf;
	    }

	    char priestnambuf[BUFSZ];
	    char *name;
	    long save_prop = EHalluc_resistance;
	    unsigned save_invis = mtmp->minvis;

	    /* when true name is wanted, explicitly block Hallucination */
	    if (!do_hallu) EHalluc_resistance = 1L;
	    if (!do_invis) mtmp->minvis = 0;
	    name = priestname(mtmp, priestnambuf);
	    EHalluc_resistance = save_prop;
	    mtmp->minvis = save_invis;
	    if (article == ARTICLE_NONE && !strncmp(name, "the ", 4))
		name += 4;
	    return strcpy(buf, name);
	}

	/* Shopkeepers: use shopkeeper name.  For normal shopkeepers, just
	 * "Asidonhopo"; for unusual ones, "Asidonhopo the invisible
	 * shopkeeper" or "Asidonhopo the blue dragon".  If hallucinating,
	 * none of this applies.
	 */
	if (mtmp->isshk && !do_hallu) {
	    if (adjective && article == ARTICLE_THE) {
		/* pathological case: "the angry Asidonhopo the blue dragon"
		   sounds silly */
		strcpy(buf, "the ");
		strcat(strcat(buf, adjective), " ");
		strcat(buf, shkname(mtmp));
		return buf;
	    }
	    strcat(buf, shkname(mtmp));
	    if (mdat == &mons[PM_SHOPKEEPER] && !do_invis)
		return buf;
	    if (mdat == &mons[PM_MASTER_SHOPKEEPER] && !do_invis)
		return buf;
	    if (mdat == &mons[PM_ELITE_SHOPKEEPER] && !do_invis)
		return buf;
	    strcat(buf, " the ");
	    if (do_invis)
		strcat(buf, "invisible ");
	    if (StarlitBug || u.uprops[STARLIT_BUG].extrinsic || have_starlitskystone() || (uarmf && uarmf->oartifact == ART_STAR_SOLES) || (uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uarmg && uarmg->oartifact == ART_RAAAAAAAARRRRRRGH) || MonsterGlyphHallu) strcat(buf, "monster");
	    else strcat(buf, mdat->mname);
	    return buf;
	}

	/* Put the adjectives in the buffer */
	if (adjective)
	    strcat(strcat(buf, adjective), " ");
	if (do_invis)
	    strcat(buf, "invisible ");
	if (do_saddle && (mtmp->misc_worn_check & W_SADDLE) &&
	    !Blind && !Hallucination)
	    strcat(buf, "saddled ");
	if (buf[0] != 0)
	    has_adjectives = TRUE;
	else
	    has_adjectives = FALSE;

	/* Put the actual monster name or type into the buffer now */
	/* Be sure to remember whether the buffer starts with a name */
	if (do_hallu) {
	    strcat(buf, rndmonnam());
	    name_at_start = FALSE;
	} else if (u.usanity > 900 && (u.usanity > rn2(10000)) && rn2(10) ) {
	    strcat(buf, rndmonnam());
	    name_at_start = FALSE;
	} else if (mtmp->mnamelth) {
	    char *name = NAME(mtmp);

	    if (StarlitBug || u.uprops[STARLIT_BUG].extrinsic || have_starlitskystone() || (uarmf && uarmf->oartifact == ART_STAR_SOLES) || (uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uarmg && uarmg->oartifact == ART_RAAAAAAAARRRRRRGH) || MonsterGlyphHallu) {
		strcat(buf, "monster");
		name_at_start = TRUE;
	    } else {

	    if (mdat == &mons[PM_GHOST]) {
		sprintf(eos(buf), "%s ghost", s_suffix(name));
		name_at_start = TRUE;
	    } else if (called && !is_mplayer(mdat) ) {
		sprintf(eos(buf), "%s called %s", mdat->mname, name);
		name_at_start = (boolean)type_is_pname(mdat);
	    } else if (is_mplayer(mdat) && (bp = strstri(name, " the ")) != 0) {
		/* <name> the <adjective> <invisible> <saddled> <rank> */
		char pbuf[BUFSZ];

		strcpy(pbuf, name);
		pbuf[bp - name + 5] = '\0'; /* adjectives right after " the " */
		if (has_adjectives)
		    strcat(pbuf, buf);
		strcat(pbuf, bp + 5);	/* append the rest of the name */
		strcpy(buf, pbuf);
		article = ARTICLE_NONE;
		name_at_start = TRUE;
	    } else if (is_mplayer(mdat) /*&& !In_endgame(&u.uz)*/) {	/* always include the rank, no matter what --Amy */
	      char pbuf[BUFSZ];
		sprintf(eos(buf), "%s called %s", mdat->mname, name);
	      strcpy(pbuf, rank_of((int)mtmp->m_lev, monsndx(mdat), (boolean)mtmp->female));
	      strcat(buf, " the ");
	      strcat(buf, lcase(pbuf));
	      name_at_start = FALSE;
	    } else {
		strcat(buf, name);
		name_at_start = TRUE;
	    }
	  }

	} else if (is_mplayer(mdat) /*&& !In_endgame(&u.uz)*/) {
	    char pbuf[BUFSZ];
	    if (StarlitBug || u.uprops[STARLIT_BUG].extrinsic || have_starlitskystone() || (uarmf && uarmf->oartifact == ART_STAR_SOLES) || (uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uarmg && uarmg->oartifact == ART_RAAAAAAAARRRRRRGH) || MonsterGlyphHallu) strcat(buf, "monster");
	    else {
		    strcpy(pbuf, rank_of((int)mtmp->m_lev, monsndx(mdat), (boolean)mtmp->female));
		    strcat(buf, lcase(pbuf));
	    }
	    name_at_start = FALSE;
	} else {
	    if (StarlitBug || u.uprops[STARLIT_BUG].extrinsic || have_starlitskystone() || (uarmf && uarmf->oartifact == ART_STAR_SOLES) || (uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uarmg && uarmg->oartifact == ART_RAAAAAAAARRRRRRGH) || MonsterGlyphHallu) strcat(buf, "monster");
	    else strcat(buf, mdat->mname);
	    name_at_start = (boolean)type_is_pname(mdat);
	}

	if (name_at_start && (article == ARTICLE_YOUR || !has_adjectives)) {
	    if (mdat == &mons[PM_WIZARD_OF_YENDOR])
		article = ARTICLE_THE;
	    else
		article = ARTICLE_NONE;
	} else if ((mdat->geno & G_UNIQ) && article == ARTICLE_A) {
	    article = ARTICLE_THE;
	}

	if (!do_hallu && mtmp->isegotype && !mtmp->noegodesc && !mtmp->noegodisplay && !PlayerUninformation) {

		/* Some of these use - or other weird ways to combine two words, which is actually against the rules
		 * of the English grammar, but it's intentional to mark words that belong to a single egotype, because
		 * a monster may have several egotypes at the same time. --Amy */

		/* we can't make the buffer arbitrarily large, so there needs to be a sanity check for the # of egotypes that
		 * gets displayed, otherwise we get segfaults when a player casts mutation a gazillion times. */
		egotypeamount = 0;

	    if (mtmp->egotype_thief && (++egotypeamount < 21) ) sprintf(eos(buf), " Thief");
	    if (mtmp->egotype_wallwalk && (++egotypeamount < 21) ) sprintf(eos(buf), " Phazer");
	    if (mtmp->egotype_disenchant && (++egotypeamount < 21) ) sprintf(eos(buf), " Disenchanter");
	    if (mtmp->egotype_rust && (++egotypeamount < 21) ) sprintf(eos(buf), " Ruster");
	    if (mtmp->egotype_corrosion && (++egotypeamount < 21) ) sprintf(eos(buf), " Corroder");
	    if (mtmp->egotype_decay && (++egotypeamount < 21) ) sprintf(eos(buf), " Decayer");
	    if (mtmp->egotype_wither && (++egotypeamount < 21) ) sprintf(eos(buf), " Witherer");
	    if (mtmp->egotype_grab && (++egotypeamount < 21) ) sprintf(eos(buf), " Grabber");
	    if (mtmp->egotype_flying && (++egotypeamount < 21) ) sprintf(eos(buf), " Flyer");
	    if (mtmp->egotype_hide && (++egotypeamount < 21) ) sprintf(eos(buf), " Hider");
	    if (mtmp->egotype_regeneration && (++egotypeamount < 21) ) sprintf(eos(buf), " Regenerator");
	    if (mtmp->egotype_undead && (++egotypeamount < 21) ) sprintf(eos(buf), " Undead");
	    if (mtmp->egotype_domestic && (++egotypeamount < 21) ) sprintf(eos(buf), " Pet-type");
	    if (mtmp->egotype_covetous && (++egotypeamount < 21) ) sprintf(eos(buf), " Covenant");
	    if (mtmp->egotype_avoider && (++egotypeamount < 21) ) sprintf(eos(buf), " Avoider");
	    if (mtmp->egotype_petty && (++egotypeamount < 21) ) sprintf(eos(buf), " Pettymonster");
	    if (mtmp->egotype_pokemon && (++egotypeamount < 21) ) sprintf(eos(buf), " Pokemon");
	    if (mtmp->egotype_slows && (++egotypeamount < 21) ) sprintf(eos(buf), " Slower");
	    if (mtmp->egotype_vampire && (++egotypeamount < 21) ) sprintf(eos(buf), " Vampire");
	    if (mtmp->egotype_teleportself && (++egotypeamount < 21) ) sprintf(eos(buf), " Teleporter");
	    if (mtmp->egotype_teleportyou && (++egotypeamount < 21) ) sprintf(eos(buf), " Warper");
	    if (mtmp->egotype_wrap && (++egotypeamount < 21) ) sprintf(eos(buf), " Wrapper");
	    if (mtmp->egotype_disease && (++egotypeamount < 21) ) sprintf(eos(buf), " Inficator");
	    if (mtmp->egotype_slime && (++egotypeamount < 21) ) sprintf(eos(buf), " Slimer");
	    if (mtmp->egotype_engrave && (++egotypeamount < 21) ) sprintf(eos(buf), " Rubber");
	    if (mtmp->egotype_dark && (++egotypeamount < 21) ) sprintf(eos(buf), " Endarker");
	    if (mtmp->egotype_luck && (++egotypeamount < 21) ) sprintf(eos(buf), " Luck-sucker");
	    if (mtmp->egotype_push && (++egotypeamount < 21) ) sprintf(eos(buf), " Pusher");
	    if (mtmp->egotype_arcane && (++egotypeamount < 21) ) sprintf(eos(buf), " Shaman");
	    if (mtmp->egotype_clerical && (++egotypeamount < 21) ) sprintf(eos(buf), " Cleric");

	    if (mtmp->egotype_armorer && (++egotypeamount < 21) ) sprintf(eos(buf), " Armorer");
	    if (mtmp->egotype_tank && (++egotypeamount < 21) ) sprintf(eos(buf), " Tank");
	    if (mtmp->egotype_speedster && (++egotypeamount < 21) ) sprintf(eos(buf), " Speedster");
	    if (mtmp->egotype_racer && (++egotypeamount < 21) ) sprintf(eos(buf), " Racer");

	    if (mtmp->egotype_randomizer && (++egotypeamount < 21) ) sprintf(eos(buf), " Randomizer");
	    if (mtmp->egotype_blaster && (++egotypeamount < 21) ) sprintf(eos(buf), " Blaster");
	    if (mtmp->egotype_multiplicator && (++egotypeamount < 21) ) sprintf(eos(buf), " Multiplicator");

	    if (mtmp->egotype_gator && (++egotypeamount < 21) ) sprintf(eos(buf), " Gator");

	    if (mtmp->egotype_reflecting && (++egotypeamount < 21) ) sprintf(eos(buf), " Reflector");
	    if (mtmp->egotype_hugger && (++egotypeamount < 21) ) sprintf(eos(buf), " Hugger");
	    if (mtmp->egotype_mimic && (++egotypeamount < 21) ) sprintf(eos(buf), " Mimic");
	    if (mtmp->egotype_permamimic && (++egotypeamount < 21) ) sprintf(eos(buf), " Permamimic");

	    if (mtmp->egotype_poisoner && (++egotypeamount < 21) ) sprintf(eos(buf), " Poisoner");
	    if (mtmp->egotype_elementalist && (++egotypeamount < 21) ) sprintf(eos(buf), " Elementalist");
	    if (mtmp->egotype_resistor && (++egotypeamount < 21) ) sprintf(eos(buf), " Resistor");
	    if (mtmp->egotype_acidspiller && (++egotypeamount < 21) ) sprintf(eos(buf), " Acidspiller");
	    if (mtmp->egotype_watcher && (++egotypeamount < 21) ) sprintf(eos(buf), " Watcher");
	    if (mtmp->egotype_metallivore && (++egotypeamount < 21) ) sprintf(eos(buf), " Metallivore");
	    if (mtmp->egotype_lithivore && (++egotypeamount < 21) ) sprintf(eos(buf), " Lithivore");
	    if (mtmp->egotype_organivore && (++egotypeamount < 21) ) sprintf(eos(buf), " Organivore");
	    if (mtmp->egotype_breather && (++egotypeamount < 21) ) sprintf(eos(buf), " Breather");
	    if (mtmp->egotype_beamer && (++egotypeamount < 21) ) sprintf(eos(buf), " Beamer");
	    if (mtmp->egotype_troll && (++egotypeamount < 21) ) sprintf(eos(buf), " Resurrector");

	    if (mtmp->egotype_faker && (++egotypeamount < 21) ) sprintf(eos(buf), " Faker");
	    if (mtmp->egotype_farter && (++egotypeamount < 21) ) sprintf(eos(buf), " Farter");
	    if (mtmp->egotype_timer && (++egotypeamount < 21) ) sprintf(eos(buf), " Timer");
	    if (mtmp->egotype_thirster && (++egotypeamount < 21) ) sprintf(eos(buf), " Thirster");
	    if (mtmp->egotype_watersplasher && (++egotypeamount < 21) ) sprintf(eos(buf), " Watersplasher");
	    if (mtmp->egotype_cancellator && (++egotypeamount < 21) ) sprintf(eos(buf), " Cancellator");
	    if (mtmp->egotype_banisher && (++egotypeamount < 21) ) sprintf(eos(buf), " Banisher");
	    if (mtmp->egotype_shredder && (++egotypeamount < 21) ) sprintf(eos(buf), " Shredder");
	    if (mtmp->egotype_abductor && (++egotypeamount < 21) ) sprintf(eos(buf), " Abductor");
	    if (mtmp->egotype_incrementor && (++egotypeamount < 21) ) sprintf(eos(buf), " Incrementor");
	    if (mtmp->egotype_mirrorimage && (++egotypeamount < 21) ) sprintf(eos(buf), " Mirror-image");
	    if (mtmp->egotype_curser && (++egotypeamount < 21) ) sprintf(eos(buf), " Curser");
	    if (mtmp->egotype_horner && (++egotypeamount < 21) ) sprintf(eos(buf), " Horner");
	    if (mtmp->egotype_lasher && (++egotypeamount < 21) ) sprintf(eos(buf), " Lasher");
	    if (mtmp->egotype_cullen && (++egotypeamount < 21) ) sprintf(eos(buf), " Cullen");
	    if (mtmp->egotype_webber && (++egotypeamount < 21) ) sprintf(eos(buf), " Webber");
	    if (mtmp->egotype_itemporter && (++egotypeamount < 21) ) sprintf(eos(buf), " Itemporter");
	    if (mtmp->egotype_schizo && (++egotypeamount < 21) ) sprintf(eos(buf), " Schizo");
	    if (mtmp->egotype_nexus && (++egotypeamount < 21) ) sprintf(eos(buf), " Nexus");
	    if (mtmp->egotype_sounder && (++egotypeamount < 21) ) sprintf(eos(buf), " Sounder");
	    if (mtmp->egotype_gravitator && (++egotypeamount < 21) ) sprintf(eos(buf), " Gravitator");
	    if (mtmp->egotype_inert && (++egotypeamount < 21) ) sprintf(eos(buf), " Inert");
	    if (mtmp->egotype_antimage && (++egotypeamount < 21) ) sprintf(eos(buf), " Antimage");
	    if (mtmp->egotype_plasmon && (++egotypeamount < 21) ) sprintf(eos(buf), " Plasmon");
	    if (mtmp->egotype_weaponizer && (++egotypeamount < 21) ) sprintf(eos(buf), " Weaponizer");
	    if (mtmp->egotype_engulfer && (++egotypeamount < 21) ) sprintf(eos(buf), " Engulfer");
	    if (mtmp->egotype_bomber && (++egotypeamount < 21) ) sprintf(eos(buf), " Bomber");
	    if (mtmp->egotype_exploder && (++egotypeamount < 21) ) sprintf(eos(buf), " Exploder");
	    if (mtmp->egotype_unskillor && (++egotypeamount < 21) ) sprintf(eos(buf), " Unskillor");
	    if (mtmp->egotype_blinker && (++egotypeamount < 21) ) sprintf(eos(buf), " Blinker");
	    if (mtmp->egotype_psychic && (++egotypeamount < 21) ) sprintf(eos(buf), " Psychic");
	    if (mtmp->egotype_abomination && (++egotypeamount < 21) ) sprintf(eos(buf), " Abomination");
	    if (mtmp->egotype_gazer && (++egotypeamount < 21) ) sprintf(eos(buf), " Gazer");
	    if (mtmp->egotype_seducer && (++egotypeamount < 21) ) sprintf(eos(buf), " Seducer");
	    if (mtmp->egotype_flickerer && (++egotypeamount < 21) ) sprintf(eos(buf), " Flickerer");
	    if (mtmp->egotype_hitter && (++egotypeamount < 21) ) sprintf(eos(buf), " Hitter");
	    if (mtmp->egotype_piercer && (++egotypeamount < 21) ) sprintf(eos(buf), " Piercer");
	    if (mtmp->egotype_petshielder && (++egotypeamount < 21) ) sprintf(eos(buf), " Petshielder");
	    if (mtmp->egotype_displacer && (++egotypeamount < 21) ) sprintf(eos(buf), " Displacer");
	    if (mtmp->egotype_lifesaver && (++egotypeamount < 21) ) sprintf(eos(buf), " Lifesaver");
	    if (mtmp->egotype_venomizer && (++egotypeamount < 21) ) sprintf(eos(buf), " Venomizer");
	    if (mtmp->egotype_dreameater && (++egotypeamount < 21) ) sprintf(eos(buf), " Dream-eater");
	    if (mtmp->egotype_nastinator && (++egotypeamount < 21) ) sprintf(eos(buf), " Nastinator");
	    if (mtmp->egotype_baddie && (++egotypeamount < 21) ) sprintf(eos(buf), " Baddie");
	    if (mtmp->egotype_sludgepuddle && (++egotypeamount < 21) ) sprintf(eos(buf), " Sludgepuddle");
	    if (mtmp->egotype_vulnerator && (++egotypeamount < 21) ) sprintf(eos(buf), " Vulnerator");
	    if (mtmp->egotype_marysue && (++egotypeamount < 21) ) sprintf(eos(buf), " Mary-Sue");
	    if (mtmp->egotype_shader && (++egotypeamount < 21) ) sprintf(eos(buf), " Shader");
	    if (mtmp->egotype_amnesiac && (++egotypeamount < 21) ) sprintf(eos(buf), " Amnesiac");
	    if (mtmp->egotype_trapmaster && (++egotypeamount < 21) ) sprintf(eos(buf), " Trapmaster");
	    if (mtmp->egotype_midiplayer && (++egotypeamount < 21) ) sprintf(eos(buf), " Midi-Player");
	    if (mtmp->egotype_rngabuser && (++egotypeamount < 21) ) sprintf(eos(buf), " RNG-abuser");
	    if (mtmp->egotype_mastercaster && (++egotypeamount < 21) ) sprintf(eos(buf), " Mastercaster");
	    if (mtmp->egotype_aligner && (++egotypeamount < 21) ) sprintf(eos(buf), " Aligner");
	    if (mtmp->egotype_sinner && (++egotypeamount < 21) ) sprintf(eos(buf), " Sinner");
	    if (mtmp->egotype_aggravator && (++egotypeamount < 21) ) sprintf(eos(buf), " Aggravator");
	    if (mtmp->egotype_minator && (++egotypeamount < 21) ) sprintf(eos(buf), " Minator");
	    if (mtmp->egotype_contaminator && (++egotypeamount < 21) ) sprintf(eos(buf), " Contaminator");
	    if (mtmp->egotype_radiator && (++egotypeamount < 21) ) sprintf(eos(buf), " Radiator");
	    if (mtmp->egotype_weeper && (++egotypeamount < 21) ) sprintf(eos(buf), " Weeper");
	    if (mtmp->egotype_reactor && (++egotypeamount < 21) ) sprintf(eos(buf), " Reactor");
	    if (mtmp->egotype_destructor && (++egotypeamount < 21) ) sprintf(eos(buf), " Destructor");
	    if (mtmp->egotype_trembler && (++egotypeamount < 21) ) sprintf(eos(buf), " Trembler");
	    if (mtmp->egotype_worldender && (++egotypeamount < 21) ) sprintf(eos(buf), " World-ender");
	    if (mtmp->egotype_damager && (++egotypeamount < 21) ) sprintf(eos(buf), " Damager");
	    if (mtmp->egotype_antitype && (++egotypeamount < 21) ) sprintf(eos(buf), " Antitype");
	    if (mtmp->egotype_painlord && (++egotypeamount < 21) ) sprintf(eos(buf), " Painlord");
	    if (mtmp->egotype_empmaster && (++egotypeamount < 21) ) sprintf(eos(buf), " EMP-Master");
	    if (mtmp->egotype_spellsucker && (++egotypeamount < 21) ) sprintf(eos(buf), " Spellsucker");
	    if (mtmp->egotype_eviltrainer && (++egotypeamount < 21) ) sprintf(eos(buf), " EvilTrainer");
	    if (mtmp->egotype_statdamager && (++egotypeamount < 21) ) sprintf(eos(buf), " StatDamager");
	    if (mtmp->egotype_damagedisher && (++egotypeamount < 21) ) sprintf(eos(buf), " Damage-Disher");
	    if (mtmp->egotype_thiefguildmember && (++egotypeamount < 21) ) sprintf(eos(buf), " ThiefGuildMember");
	    if (mtmp->egotype_rogue && (++egotypeamount < 21) ) sprintf(eos(buf), " Rogue");
	    if (mtmp->egotype_steed && (++egotypeamount < 21) ) sprintf(eos(buf), " Steed");
	    if (mtmp->egotype_champion && (++egotypeamount < 21) ) sprintf(eos(buf), " Champion");
	    if (mtmp->egotype_boss && (++egotypeamount < 21) ) sprintf(eos(buf), " Boss");
	    if (mtmp->egotype_atomizer && (++egotypeamount < 21) ) sprintf(eos(buf), " Atomizer");
	    if (mtmp->egotype_perfumespreader && (++egotypeamount < 21) ) sprintf(eos(buf), " Perfume-Spreader");
	    if (mtmp->egotype_converter && (++egotypeamount < 21) ) sprintf(eos(buf), " Converter");
	    if (mtmp->egotype_wouwouer && (++egotypeamount < 21) ) sprintf(eos(buf), " Wouwouer");
	    if (mtmp->egotype_allivore && (++egotypeamount < 21) ) sprintf(eos(buf), " Allivore");
	    if (mtmp->egotype_nastycurser && (++egotypeamount < 21) ) sprintf(eos(buf), " Nastycurser");
	    if (mtmp->egotype_sanitizer && (++egotypeamount < 21) ) sprintf(eos(buf), " Sanitizer");
	    if (mtmp->egotype_laserpwnzor && (++egotypeamount < 21) ) sprintf(eos(buf), " LaserPwnz0r");
	    if (mtmp->egotype_badowner && (++egotypeamount < 21) ) sprintf(eos(buf), " Bad0wn3r");
	    if (mtmp->egotype_bleeder && (++egotypeamount < 21) ) sprintf(eos(buf), " Bleeder");
	    if (mtmp->egotype_shanker && (++egotypeamount < 21) ) sprintf(eos(buf), " Shanker");
	    if (mtmp->egotype_terrorizer && (++egotypeamount < 21) ) sprintf(eos(buf), " Terrorizer");
	    if (mtmp->egotype_feminizer && (++egotypeamount < 21) ) sprintf(eos(buf), " Feminizer");
	    if (mtmp->egotype_levitator && (++egotypeamount < 21) ) sprintf(eos(buf), " Levitator");
	    if (mtmp->egotype_illusionator && (++egotypeamount < 21) ) sprintf(eos(buf), " Illusionator");
	    if (mtmp->egotype_stealer && (++egotypeamount < 21) ) sprintf(eos(buf), " Stealer");
	    if (mtmp->egotype_stoner && (++egotypeamount < 21) ) sprintf(eos(buf), " Stoner");
	    if (mtmp->egotype_maecke && (++egotypeamount < 21) ) sprintf(eos(buf), " Maecke");
	    if (mtmp->egotype_flamer && (++egotypeamount < 21) ) sprintf(eos(buf), " Flamer");
	    if (mtmp->egotype_datadeleter && (++egotypeamount < 21) ) sprintf(eos(buf), " DataDeleter");
	    if (mtmp->egotype_blasphemer && (++egotypeamount < 21) ) sprintf(eos(buf), " Blasphemer");
	    if (mtmp->egotype_dropper && (++egotypeamount < 21) ) sprintf(eos(buf), " Dropper");
	    if (egotypeamount > 20) sprintf(eos(buf), " (%d egotypes)", egotypeamount);

	}

	{
	    char buf2[BUFSZ];

	    switch(article) {
		case ARTICLE_YOUR:
		    strcpy(buf2, "your ");
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    return buf;
		case ARTICLE_THE:
		    strcpy(buf2, "the ");
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    return buf;
		case ARTICLE_A:
		    return(an(buf));
		case ARTICLE_NONE:
		default:
		    return buf;
	    }
	}
}

#endif /* OVL0 */
#ifdef OVLB

char *
l_monnam(mtmp)
register struct monst *mtmp;
{
	return(x_monnam(mtmp, ARTICLE_NONE, (char *)0,
		mtmp->mnamelth ? SUPPRESS_SADDLE : 0, TRUE));
}

#endif /* OVLB */
#ifdef OVL0

char *
mon_nam(mtmp)
register struct monst *mtmp;
{
	return(x_monnam(mtmp, ARTICLE_THE, (char *)0,
		mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE));
}

/* print the name as if mon_nam() was called, but assume that the player
 * can always see the monster--used for probing and for monsters aggravating
 * the player with a cursed potion of invisibility
 */
char *
noit_mon_nam(mtmp)
register struct monst *mtmp;
{
	return(x_monnam(mtmp, ARTICLE_THE, (char *)0,
		mtmp->mnamelth ? (SUPPRESS_SADDLE|SUPPRESS_IT) :
		    SUPPRESS_IT, FALSE));
}

char *
Monnam(mtmp)
register struct monst *mtmp;
{
	register char *bp = mon_nam(mtmp);

	*bp = highc(*bp);
	return(bp);
}

char *
noit_Monnam(mtmp)
register struct monst *mtmp;
{
	register char *bp = noit_mon_nam(mtmp);

	*bp = highc(*bp);
	return(bp);
}

/* monster's own name */
char *
m_monnam(mtmp)
struct monst *mtmp;
{
	return x_monnam(mtmp, ARTICLE_NONE, (char *)0, EXACT_NAME, FALSE);
}

/* pet name: "your little dog" */
char *
y_monnam(mtmp)
struct monst *mtmp;
{
	int prefix, suppression_flag;

	prefix = mtmp->mtame ? ARTICLE_YOUR : ARTICLE_THE;
	suppression_flag = (mtmp->mnamelth
			    /* "saddled" is redundant when mounted */
			    || mtmp == u.usteed
			    ) ? SUPPRESS_SADDLE : 0;

	return x_monnam(mtmp, prefix, (char *)0, suppression_flag, FALSE);
}

#endif /* OVL0 */
#ifdef OVLB

char *
Adjmonnam(mtmp, adj)
register struct monst *mtmp;
register const char *adj;
{
	register char *bp = x_monnam(mtmp, ARTICLE_THE, adj,
		mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE);

	*bp = highc(*bp);
	return(bp);
}

char *
a_monnam(mtmp)
register struct monst *mtmp;
{
	return x_monnam(mtmp, ARTICLE_A, (char *)0,
		mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE);
}

char *
a_noit_monnam(mtmp)
register struct monst *mtmp;
{
	return x_monnam(mtmp, ARTICLE_A, (char *)0,
		mtmp->mnamelth ? (SUPPRESS_SADDLE|SUPPRESS_IT) : SUPPRESS_IT, FALSE);
}

char *
Amonnam(mtmp)
register struct monst *mtmp;
{
	register char *bp = a_monnam(mtmp);

	*bp = highc(*bp);
	return(bp);
}

/* used for monster ID by the '/', ';', and 'C' commands to block remote
   identification of the endgame altars via their attending priests */
char *
distant_monnam(mon, article, outbuf)
struct monst *mon;
int article;	/* only ARTICLE_NONE and ARTICLE_THE are handled here */
char *outbuf;
{
    /* high priest(ess)'s identity is concealed on the Astral Plane,
       unless you're adjacent (overridden for hallucination which does
       its own obfuscation) */
    if ( (mon->data == &mons[PM_HIGH_PRIEST] || mon->data == &mons[PM_DNETHACK_ELDER_PRIEST_TM_]) && !Hallucination &&
	    Is_astralevel(&u.uz) && distu(mon->mx, mon->my) > 2) {
	strcpy(outbuf, article == ARTICLE_THE ? "the " : "");
	strcat(outbuf, mon->female ? "high priestess" : "high priest");
    } else {
	strcpy(outbuf, x_monnam(mon, article, (char *)0, 0, TRUE));
    }
    return outbuf;
}

static const char * const bogusmons[] = {
	"jumbo shrimp", "giant pigmy", "gnu", "killer penguin",
	"giant cockroach", "giant slug", "pterodactyl",
	"tyrannosaurus rex", "rot grub", "bookworm", "mastah lichen",
	"hologram", "jester", "attorney", "sleazoid",
	"killer tomato", "amazon", "robot", "battlemech",
	"rhinovirus", "lion-dog", "rat-ant", "Y2K bug",
						/* misc. */
	"grue", "Christmas-tree monster", "luck sucker", "paskald",
	"brogmoid", "dornbeast",		/* Quendor (Zork, &c.) */
	"Ancient Multi-Hued Dragon", "Evil Iggy",
						/* Moria */
	"emu", "kestrel", "xeroc", "venus flytrap",
						/* Rogue */
	"creeping coins",			/* Wizardry */
	"siren",                                /* Greek legend */
	"killer bunny",				/* Monty Python */
	"rodent of unusual size",		/* The Princess Bride */
	"Smokey the bear",	/* "Only you can prevent forest fires!" */
	"Luggage",				/* Discworld */
	"Ent",					/* Lord of the Rings */
	"tangle tree", "wiggle",                /* Xanth */
	"white rabbit", "snark",		/* Lewis Carroll */
	"pushmi-pullyu",			/* Dr. Doolittle */
	"smurf",				/* The Smurfs */
	"tribble", "Klingon", "Borg",		/* Star Trek */
	"Ewok",					/* Star Wars */
	"Totoro",				/* Tonari no Totoro */
	"ohmu",					/* Nausicaa */
	"youma",				/* Sailor Moon */
	"nyaasu",				/* Pokemon (Meowth) */
	"Godzilla", "King Kong",		/* monster movies */
	"earthquake beast",			/* old L of SH */
	"Invid",				/* Robotech */
	"Terminator",				/* The Terminator */
	"boomer",				/* Bubblegum Crisis */
	"Dalek",				/* Dr. Who ("Exterminate!") */
	"microscopic space fleet", "Ravenous Bugblatter Beast of Traal",
						/* HGttG */
	"teenage mutant ninja turtle",		/* TMNT */
	"samurai rabbit",			/* Usagi Yojimbo */
	"aardvark",				/* Cerebus */
	"Audrey II",				/* Little Shop of Horrors */
	"witch doctor", "one-eyed one-horned flying purple people eater",
						/* 50's rock 'n' roll */
	"Barney the dinosaur",			/* saccharine kiddy TV */
	"Azog the Orc King", "Morgoth",		/* Angband */

	/*[Tom] new wacky names */
	"commando", "green beret", "sherman tank",
						/* Military */
	"Jedi knight", "tie fighter", "protocol droid", "R2 unit", "Emperor",
						/* Star Wars */
	"Vorlon",				/* Babylon 5 */
	"keg","Diet Pepsi",
						/* drinks */
	"questing beast",		/* King Arthur */
	"Predator",				/* Movie */
	"green light", "automobile", "invisible Wizard of Yendor",
	"piece of yellowish-brown glass", "wand of nothing",
	"ocean","ballpoint pen","paper cut",
						/* misc */
	"Rune", "Gurk", "Yuval",		/* people I know */
	"mother-in-law"				/* common pest */
	"one-winged dewinged stab-bat",		/* KoL */
	"praying mantis",
	"arch-pedant",
	"beluga whale",
	"bluebird of happiness",
	"bouncing eye", "floating nose",
	"buffer overflow", "dangling pointer", "walking disk drive",
	"cacodemon", "scrag",
	"cardboard golem", "duct tape golem",
	"chess pawn",
	"chocolate pudding",
	"coelacanth",
	"corpulent porpoise",
	"Crow T. Robot",
	"diagonally moving grid bug",
	"dropbear",
	"Dudley",
	"El Pollo Diablo",
	"evil overlord",
	"existential angst",
	"figment of your imagination", "flash of insight",
	"flying pig",
	"gazebo",
	"gonzo journalist",
	"gray goo", "magnetic monopole",
	"heisenbug",
	"lag monster",
	"loan shark",
	"Lord British",
	"newsgroup troll",
	"ninja pirate zombie robot",
	"octarine dragon",
	"particle man",
	"possessed waffle iron",
	"poultrygeist",
	"raging nerd",
	"roomba",
	"sea cucumber",
	"spelling bee",
	"Strong Bad",
	"stuffed raccoon puppet",
	"tapeworm",
	"liger",
	"velociraptor",
	"vermicious knid",
	"viking",
	"voluptuous ampersand",
	"wee green blobbie",
	"wereplatypus",
	"zergling",
	"hag of bolding",
	"grind bug",
	"enderman",
	"wight supremacist",
	"Magical Trevor",
	"first category perpetual motion device",
	"ghoti",
	"regex engine",
	"netsplit",
	"peer",
	"pigasus",
	"Semigorgon",
	"meeple",
	"conventioneer",
	"terracotta warrior",
	"large microbat", "small megabat",

	/* soundex and typos of monsters, from NAO, added in UnNetHack */
	"gloating eye",
	"flush golem",
	"martyr orc",
	"mortar orc",
	"acute blob",
	"aria elemental",
	"aliasing priest",
	"aligned parasite",
	"aligned parquet",
	"aligned proctor",
	"baby balky dragon",
	"baby blues dragon",
	"baby caricature",
	"baby crochet",
	"baby grainy dragon",
	"baby bong worm",
	"baby long word",
	"baby parable worm",
	"barfed devil",
	"beer wight",
	"boor wight",
	"brawny mold",
	"rave spider",
	"clue golem",
	"bust vortex",
	"errata elemental",
	"elastic eel",
	"electrocardiogram eel",
	"fir elemental",
	"tire elemental",
	"flamingo sphere",
	"fallacy golem",
	"frizzed centaur",
	"forest centerfold",
	"fierceness sphere",
	"frosted giant",
	"geriatric snake",
	"gnat ant",
	"giant bath",
	"grant beetle",
	"giant mango",
	"glossy golem",
	"gnome laureate",
	"gnome dummy",
	"gooier ooze",
	"green slide",
	"guardian nacho",
	"hell hound pun",
	"high purist",
	"hairnet devil",
	"ice trowel",
	"feather golem",
	"lounge worm",
	"mountain lymph",
	"pager golem",
	"pie fiend",
	"prophylactic worm",
	"sock mole",
	"rogue piercer",
	"seesawing sphere",
	"simile mimic",
	"moldier ant",
	"stain vortex",
	"scone giant",
	"umbrella hulk",
	"vampire mace",
	"verbal jabberwock",
	"water lemon",
	"water melon",
	"winged grizzly",
	"yellow wight",

	/* from http://www.alt.org/nethack/addmsgs/viewmsgs.php added in UnNetHackPlus*/
	"lurker below",
	"worthless yellowish-brown glass golem",
	"writhing mass of primal chaos", /* ADOM */
	"hallucinatory monster",
	"jumping brain",
	"colorless green idea",
	"floating ear",
	"floating tongue",
	"hallucinogen-distorted hallucination",
	"mountain dwarf",
	"were(random beast)",
	"weremindflayer",
	"wereplatypus",
	"Gnome With the Wand of Death",
	"arch-lichen",
	"Baba Yaga",
	"harmless protoplasm",
	"badger",
	"giant dwarf",
	"magically animated Vorpal Blade",
	"Legendary black beast of Arrrgh",

	/* from UnNetHack */
	"apostroph golem", "Bob the angry flower",
	"bonsai-kitten", "Boxxy", "lonelygirl15",
	"tie-thulu", "Domo-kun", "nyan cat",
	"looooooooooooong cat",			/* internet memes */
	"bohrbug", "mandelbug", "schroedinbug", /* bugs */
	"Gerbenok",				/* Monty Python killer rabbit */
	"doenertier",				/* Erkan & Stefan */
	"Invisible Pink Unicorn",
	"Flying Spaghetti Monster",		/* deities */
	"Bluebear", "Professor Abdullah Nightingale",
	"Qwerty Uiop", "troglotroll",		/* Zamonien */
	"wolpertinger", "elwedritsche", "skvader",
	"Nessie", "tatzelwurm", "dahu",		/* european cryptids */
	"three-headed monkey",			/* Monkey Island */
	"little green man",			/* modern folklore */
	"weighted Companion Cube",		/* Portal */
	"/b/tard",				/* /b/ */
	"manbearpig",				/* South Park */
	"ceiling cat", "basement cat",
	"monorail cat",				/* the Internet is made for cat pix */
	"rape golem",				/* schnippi */
	"tridude",				/* POWDER */
	"orcus cosmicus",			/* Radomir Dopieralski */
	"yeek", "quylthulg",
	"Greater Hell Beast",			/* Angband */
	"Vendor of Yizard",			/* Souljazz */
	"Sigmund", "lernaean hydra", "Ijyb",
	"Gloorx Vloq", "Blork the orc",		/* Dungeon Crawl Stone Soup */
	"unicorn pegasus kitten",		/* Wil Wheaton, John Scalzi */
	"dwerga nethackus", "dwerga castrum",	/* Ask ASCII Ponies */

	"Irrenhaus the Third",			/* http://www.youtube.com/user/Irrenhaus3 */

	"semipotent demidog", "shale imp",	/* Homestuck */
	"mercury imp", "Betty Crocker",
	"Spades Slick",
	"patriarchy", "bourgeiose",		/* talking points */
	"mainstream media",
	"Demonhead Mobster Kingpin",		/* Problem Sleuth */
	"courtesan angel", "fractal bee",
	/* bogus UnNetHack monsters */
	"weeping angle",
	"gelatinous sphere", "gelatinous pyramid",
	"gelatinous Klein bottle", "gelatinous Mandelbrot set",
	"robot unicorn",
	/* Welcome to Night Vale*/
	"John Peters, you know, the farmer",

	"great hell nanafushi",
	"giant cockatflayer eellich",
	"water hound",
	"eel-dog",
	"lesser nishikiori",
	"nishikiori",
	"Internet Exploder",
	"lesser degudoga",
	"degudoga",
	"gisau",
	"ondy",
	"meso",
	"hige meso",
	"death quasit",
	"moaning wolf",
	"biwa jelly",
	"wine jelly",
	"macya jelly",
	"nijisseiki nama jelly",
	"apple konnyaku jelly",
	"zakuro konnyaku jelly",
	"ume konnyaku jelly",
	"grape konnyaku jelly",
	"kyohaku nymph",
	"silly nymph",
	"Akane",
	"Kisuke",
	"Aobe",
	"lesser omochi beast",
	"taho drakee",
	"killer duck",
	"duck warrior",
	"duck captain",
	"iron duck",
	"takuhi",
	"jubjub bird",
	"plasma elemental",
	"cribo",
	"rafflesia",
	"blue giant",
	"green giant",
	"Jaian",
	"novice lich",
	"slimebesu",
	"ojyama puyo",
	"slimebesu lord",
	"chaos shapechanger"
	"grape ice creep",
	"cemedined eraser",
	"ultra noncommittal motorcycle",
	"Bull Gates",
	"Mongai Sakurada",
	"Sanji",
	"hata-hata",
	"winged eel",
	"deminewt",
	"master newt",
	/*Silly JNetHack classes*/
	"User of Stand",
	"Sexymate",
	"Triclops",
	/*others*/
	"Daisenki",
	"squid girl",

	"Mook",
	"Gachapin",
	"Noppo-san",
	"Gonta-kun",
	"Nonowa-san",

	/*summer illust*/
	"cast-off shell of a cicada",
	"cast-off shell of a dragonfly larva",
	"halcyon",
	"Morning Glory",
	"bitter gourd gang",
	/*Wizardly*/
	"unseen entity",
	"shadowy figure",
	"slimy thing",
	"strange animal",
	"strange plant",
	"gaunt figure",
	"tiny figure",
	"man in black",
	"fiery figure",
	"radiant figure",
	"weird humanoid",
	"fluffy thing",
	"unseen being",
	"protozoan",
	"cave dweller",
	"animated object",
	"creeping thing",
	"flying creature",
	"reptile",
	"dark beast",
	"toadstool",
	"strange mist",
	"demonic figure",
	"dark shadow",
	"wriggle objec",
	"creeping objec",
	"flap figure",
	"squat figure",
	"standing animal",
	"clatter ! hoof",
	/*others*/
	"prototype meter",
	"prototype kilogram",
	"shortest sword",
	"kotatsu snail",
	"Saturday Knight",
	"moai",
	"PC mouse",
	"kitten on your keyboard",
	"vacuum golem",
	"extra newspaper",
	"wireless mouse",
	"wireless cable",
	"static vortex",
	"jumping jelly",
	"herbivorous plant",
	"flintwork automaton",
	"proprllered gargoyle",
	"lonely wolf",
	"optical rat",
	"wireflameman",
	"scarecockatrice",
	"PMD model", "PMX model",	/*MikuMikuDance*/
	"monster from lift-the-flap book",
	"bobbin tank",
	"time consumer",
	"lead soldier",
	"cyclopedia",
	"liquor spirits",
	"Jack of Earphone",
	"Hi-Jack",
	"Pandemonium demon",	/*Dungeon Crawl*/
	"Mobile Idol Ganbutu",	/*Aku-Daikan*/
	"Tutankamen",		/*Aku-Daikan*/
	"Scissorman",		/*Clock Tower*/
	"giant house centipede",
	"steel blob",
	"borrow wight",
	"Kan-musu",		/*Kantai Collection*/
	"forgotten beast",	/*Dwarf Fortress*/
	"blank body",		/*Cataclysm DDA*/
	"F.O.E",		/*Etrian Odyssey*/
	"Blue-eyed Doll",	/*Dolls of Friendship*/
	"handmade oobleck slime",
	"dry blob",
	"microdrive",		/*Goat MMO Simulator*/
	"gold harvest golem",	/*Goat MMO Simulator*/

	/* from UnNetHackPlus */
	"King Krakus",               /* Polish folklore */
	"Topielec",                  /* Slavic folklore */
	"pink oliphaunt",            /* Lord of the Rings + silliness */
	"Amphisbaena",               /* Greek mythology */
	"phoenix",                   /* Greek mythology */
	"catoblepas",                /* Greek mythology */
	"phantom kangaroo",          /* urban legend */
	"echinemon",                 /* from medieval literature, "enemy of the dragon" */
	"Ratatoskr",                 /* Norse mythology */
	"Twrch Trwyth",              /* Arthurian legends */
	"Unperson",                  /* Nineteen Eighty-Four */
	"Somebody Else's Problem",   /* Douglas Adams */
	"Armok",                     /* Dwarf Fortress */
	"Dwarf-Eating Carp",         /* Dwarf Fortress */
	"Urist McDwarf",             /* Dwarf Fortress */
	"werecapybara",              /* Dwarf Fortress */
	"werecthulhu",
	"weresomething",
	"Evil Otto",                 /* Berzerk - via GruntHack */
	"P'lod",                     /* Weekly World News */
	"mortgage golem",
	"dark matter golem",
	"giant orange brain",        /* Dungeon Crawl Stone Soup */
	"ugly thing",                /* Dungeon Crawl Stone Soup */
	"hellephant",                /* Dungeon Crawl Stone Soup */
	"inept mimic",               /* Dungeon Crawl Stone Soup */
	"hungry ghost",              /* Dungeon Crawl Stone Soup */
	"unborn deep dwarf",         /* Dungeon Crawl Stone Soup */
	"Wandering mushroom",        /* Dungeon Crawl Stone Soup */
	"Vlad the Inhaler",
	"Delaunay tessellation field estimator",
	"unnameable horror from beyond",/* NAO fruit name*/
	"munchkin",
	"error-spamming shambling horror",          /* SporkHack */
	"Grid Bug Mk. 2",            /* SLAS'EM (nickname of arc bugs) */
	/* "killer tripe ration", */      /* SLAS'EM */
	"yet another D&D monster",
	"kobold mage",
	"hobbyte",
	/* via ProgressQuest */
	"will-o'-the-wisp",
	"ignis fatuus",
	"triceratops",
	"sylph",
	"stegosaurus",
	"sphinx",
	"spectre",
	"lamassu",
	"su-monster",                /* Dungeons & Dragons */
	"shambling mound",           /* Dungeons & Dragons */
	"sand elemental",            /* Dungeons & Dragons */
	"rubber golem",               /* Dungeons & Dragons */
	"remorhaz",                  /* Dungeons & Dragons */
	"otyugh",                    /* Dungeons & Dragons */
	"bacon elemental",
	"roper",
	"roc",
	"peryton",                   /* Jorge Luis Borges - Book of Imaginary Beings */
	"octopus",
	"beer golem",                /* ProgressQuest */
	"rice giant",                /* ProgressQuest */
	"porn elemental",            /* ProgressQuest */
	"demicanadian",              /* ProgressQuest */
	"gyrognome",                 /* ProgressQuest */
	"cardboard golem",
	"cheese elemental",
	"dervish",
	"dragon turtle",
	"megalosaurus",
	"organist",
	/* end of monsters via ProgressQuest */
	"Lucius Malfoy",             /* Harry Potter */
	"Dumbledore",                /* Harry Potter */
	"Harry Potter",              /* Harry Potter */
	"Crumple-Horned Snorkack",   /* Harry Potter */
	"mailer daemon",               /* with defined MAIL it may be selected as real monster in get_bogus_monster_name */
	"Vaarsuvius",                /* The Order of the Stick */
	"Durkon Thundershield",      /* The Order of the Stick */
	"Roy Greenhilt",             /* The Order of the Stick */
	"Lord Voldemort", "He-who-may-not-be-named", "Tom Marvolo Riddle", "Al-Mutasim", "Basil the Bat Lord", "Insectoid Queen Gypsy Moth", "run-time error", "abnormal program termination", "unhandled exception", "assertion failure", "file read error",
	"halt", "giant error", "world killer", "eater of worlds", "Mickey Mouse", "Donald Duck", "Scrooge McDuck", "Link", "Zelda", "Ganondorf", "Enderdragon", "Pikachu", "Aerodactyl", "Ho-oh", "Elite Four Bruno", "Gym Leader Claire", "Champion Lance",
	"Team Rocket Grunt", "Team Missile Bomb Grunt", "Hamburgler Grunt", "Broil Bunch Grunt", "Arcanine", "Mewtwo", "Mewthree", "Ebony Dark'ness Dementia Raven Way", "B'loody Mary Smith", "Vampire Potter", "Mehrunes Dagon", "Emperor Uriel Septim",
	"General Tullius", "Ulfric Stormcloak", "+7 daedric long sword", "blessed spellbook of Fus-Ro-Dah", "cursed called", "super wooden statue", "Hrungnir, The Hill Giant Lord", "The Demon Lord Surtur", "your godfather", "Donkey Kong", "King K. Rool",
	"Andariel, the Maiden of Anguish", "Duriel, the Lord of Pain", "Mephisto, the Lord of Hate", "Diablo, the Lord of Terror", "Baal, the Lord of Destruction", "B-a-a-l", "extra strong might-enchanted multiple shots lightning enchanted gloam",
	"Caesar", "General Jing-Wei", "President Eden", "Colonel B. Astard", "Elder Owyn Lyons", "Tobar", "General Chase", "Krzzzzssssssthhhhuuuuuullll Hnnnnnngggggggghhhhhhhhhhhh", "Scribe Vallincourt", "Scribe Bigsley", "annoying lab-coat-wearing scientist",
	"nerdy geek", "greedy doctor", "Lord Ashur", "Father Elijah", "Joshua Graham", "Courier Ulysses", "Claude Speed", "Tommy Vercetti", "Carl Johnson", "Niko Bellic", "Dimitri Rascalov", "Melvin 'Big Smoke' Harris", "Sonny Forelli", "Don Salvatore",
	"Mr. Don", "Jimmy Pegorino", "Galdryn the Green", "Shaduroth", "Nauselom", "Greuvenia the Pox", "Ikrella the Witch", "Elder Demus Fathien", "game-freezing ninja lord", "Yagu Matasai", "Osayo Narakami wearing a cerberus band and a winter katana",
	"General Ironside", "pain in the butt", "ass-fucker", "General Mohmar Deathstrike", "paladin tank", "emperor overlord", "aurora alpha bomber", "king raptor", "attack outpost", "lag defense tower", "stinger site", "tunnel network", "nuclear missile silo",
	"particle uplink cannon", "scud storm", "construction dozer", "tactical superweapon", "commander in chief", "terrorist", "S.W.A.T. member", "undercover cop", "Superman", "Gordon Freeman", "G-Man", "Colonel Shepard", "Drill Sergeant Sharp",
	"Drill Sergeant Nasty", "grand inquisitor", "game over screen", "dywypi", "yasd", "fatal food poisoning", "dark wraith", "gigantic mind flayer", "dream eater", "Super Mario", "Princess Peach", "Princess Toadstool", "goomba", "koopa troopa",
	"Lakitu", "Bowser", "Bullet Bill", "homing Bullet Bill", "Blarog", "Wart", "Birdo-Ostro", "Clawglip", "Larry Koopa", "Morton Koopa", "Wendy O. Koopa", "Iggy Koopa", "Roy Koopa", "Lemmy Koopa", "Ludwig von Koopa", "Simon Belmont", "Ryu Hayabusa",
	"Jaquio", "Ashtar", "hallucinogen-distorted Wizard of Yendor", "polytrap abomination", "result of a bad polytrap", "outta depth giant shoggoth", "freaking monadic deva", "James Bond", "Bud Spencer", "Le Chiffre", "Renard", "disappointing final boss",
	"wimpy final boss", "warm-up boss", "wake up call boss", "that one boss", "that one boss named Whitney", "unbeatable Air Man", "Sheriff of Nottingham", "Guy of Gisborne", "gnome who zaps a hexagonal wand", "invisible player character", "invisible outta depth monster",
	"disintegration-breathing cockatrice", "animated gray dragon scale mail", "stupidity in motion", "death-is-death loonie", "hardcore player", "pro-gamer", "hardcore internet nerd", "DDOS attack", "blue screen of death", "blackscreen bug",
	"fatal system error", "windows subsystem has stopped unexpectedly", "general protection fault", "stack overflow", "stack underflow", "pure virtual function call", "not enough space for environment", "low local memory", "not a valid save file",
	"save-game corruption", "savegame erasing bug", "program in disorder", "unstable equilibrium", "crash-prone operating system", "sudden reboot", "power failure", "wide-angle disintegration beam", "infidel priest of Moloch", "Marduk the Creator",
	"killer cram ration", "huge pile of killer rocks", "hallucinogen supplier", "nightmare fuel", "kitten called Wizard of Yendor needs food badly", "little dog called savescum13", "Team Ant Leader", "Team A Leader", "extra fast soldier ant", "soldier ant with the wand of death",
	"invisible soldier ant", "self-replicating soldier ant", "number one cause of nethack deaths", "hallucinogen-distorted master mind flayer", "werecockatrice", "black weredragon", "wererocktroll", "werelich", "were-ki-rin", "weremedusa",
	"weresuccubus", "wereincubus", "hallucinogen-distorted werehallucinator", "gnome wielding the Tsurugi of Muramasa", "goblin wielding a sword called vorpal blade", "pain elemental of Moloch", "air elemental of Air", "fire elemental of Fire",
	"dremora caitiff", "mythical dawn agent", "Lord Sheogorath", "archcouatl", "master solar", "archfiend summon", "archnemesis", "plaster blaster", "psych orb", "Na-Krul", "felltwin", "schizophrenic", "bipolar oddity", "Ford Sierra Cosworth",
	"Lancia Integrale", "Lancia Stratos", "Ford Focus", "Ford Escort", "Toyota Corolla", "MG Metro 6R4", "Peugeot 205 Turbo 16", "drunken driver", "Commissioner Hunter", "Hydra Aurora Bomber", "VTOL aircraft", "multi-purpose amphibian assault ship",
	"devteam member", "player who can ascend any character", "ascension runner", "total noobie", "biggest noob ever", "critically injured smirking sneak thief", "application error", "integer divide by 0", "unrecoverable internal error",

"$cat",
"$dog",
"$fruit -headed Wizard of Yendor",
"$playername",
"$playername 's imaginary girlfriend dual-wielding lightsabers",
"'74 Cadillac",
"(random gas-colour e.g. paisley) dragon",
"... wait a second. You're high!",
"/b/",
"1 of 60 Templars",
"15 m class Eoten",
"23-headed lizard-monkey",
"4chan",
"50 Cent",
"@ sign",
"A Giant Baked Bean",
"A MILF",
"A Mathematician",
"A One Eyed Trouser Snake",
"A Weapon of Mass Destruction",
"A Wood Carver",
"AM",
"ASCII representation of a monster",
"Abraham Lincoln",
"Absolutely nothing",
"Academia",
"Acererak in a TIE fighter",
"Acid worm",
"Adom player",
"Aerith",
"Agent Smith",
"Ahriman",
"Air golem",
"Airman",
"Akkat",
"Albert Einstein",
"Alf",
"Angband player",
"Angra Mainyu",
"Anime Cosplayer",
"Ann Coulter",
"Ann Coulter",
"Anywere",
"Applejack",
"Arceus",
"Astral call bug",
"Aud Ketilsdatter",
"Aunt Irma",
"BASIC elemental",
"BDFL",
"BIKECAT",
"Baba Yaga",
"Baby Tank",
"Bachelor's party",
"Bad Dude",
"Bag of doorknobs",
"Baiowulf",
"Ballos",
"Balrog",
"Ban monster",
"Barbariccia",
"Barky fox",
"Baron Harkonnen",
"Barzahl",
"Basement Cat",
"Basic mind flayer",
"Basic mind flayer",
"Batman",
"Battlesnake",
"Beelzebub",
"Behemoth",
"Bellybutton lint monster",
"Bennifer",
"Betamax",
"Betty & Veronica",
"Big Bird",
"Big, Round, kind of greyish Golem",
"Bill Gates",
"Bill O'Reilly",
"Björk",
"Black Cat",
"Black Knight (with limbs)",
"Black Knight (without limbs)",
"Blaster",
"Blue-Eyes White Dragon",
"Bo Derek",
"Bob Barker",
"Bob Hope",
"Bob the Blob",
"Bon Jovi",
"Bort Sim-pesson",
"Bort Simpesson",
"Bort Sipesson",
"Bowser",
"Bozo the Clown",
"Brain eating snake",
"Brain kicker",
"Brick monster",
"Britney Spears",
"Brotherman Bill",
"Bugger Hive-Queen",
"Bugs Bunny",
"Bugs Bunny",
"Cactaur",
"Caerbannog",
"Cagnazzo",
"Calculator store",
"Calvin & Hobbes",
"Can of Spackling Paste",
"Cantankerous Californian",
"Captain Crunch",
"Captain Gordon, Defender of Earth",
"Captain Kirk",
"Captain Tagon",
"Captain Viridian",
"Care Bear",
"Carrot Soldier",
"Carrot Top",
"Carry Nation",
"Ceiling Cat",
"Cerberus",
"Cerenus",
"Chao",
"Chaos Butterfly",
"Charles Barkley",
"Charlie Chaplin",
"Charon",
"Cheese Golem",
"Chess Piece Face",
"Choronzon",
"Chris the Ninja Pirate",
"Christopher Walken",
"Chuck Norris",
"Chucky",
"Clippy the Shopping Assistant",
"Coach Z",
"Colin Farrell",
"Colonel Sanders",
"Combine soldier",
"Cosmos",
"Crawl player",
"Crow T Robot",
"Crowley",
"Crumple-Horned Snorkack",
"Cryptosporidium",
"Cryptosporidium-136",
"Cthulhu",
"Cthulhu",
"Cthulhu",
"Cucumber",
"Cucumber sandwich",
"Curly Brace",
"Curse of the Were-Rabbit",
"Cursed pile of coins",
"Cyberman",
"Dagoth Ur",
"Dalek",
"Dan Quayle",
"Dante Alighieri",
"Daphne",
"Darth Vader",
"Darwin's fish",
"David Hasselhoff",
"Deadline",
"Death of Rats",
"Descoladore",
"Detritus",
"Dev Team",
"Dion Nicholas",
"Disco Bandit",
"Discord",
"Disemheaded body",
"Disgruntled postal worker",
"Doctor",
"Dogley",
"Don Knotts",
"Donut head, who says 'I am not a hallucination.'",
"Dorothy Gale",
"Dr. Funkenstein",
"Dr. McNinja",
"Dr. Phil",
"Dr. strange",
"Dremora",
"Drunken Politician",
"Dudley",
"Dudley",
"Dumbledore",
"Dungeon Master",
"Dust Bunny",
"Dust speck",
"Dyspeptic hamster",
"Dzhokar Tsarnaev",
"EVA Unit 01",
"EXTERMINATE! EXTERMINATE! EXTERMINATE!",
"Easss",
"Eblis",
"Edward Cullen",
"Eg",
"Eh! Steve!",
"Eidolos",
"El Pollo Diablo",
"Elbereth Engraving",
"Elvis",
"Entropy",
"Eris",
"Error from the Clone Lab",
"Evil Devil",
"Evil angel",
"Evilking",
"Exor",
"Explorington III",
"Extraordinary person",
"Eye of Sauron",
"F.O.E.",
"FOXHOUND covert operative",
"FPSRussia",
"Fat Albert",
"Fat Momma",
"Feedback",
"Fig Newton of your imagination",
"Filet-O-Fish",
"Flaming Violist",
"Flea Man",
"Fluttershy",
"Flying Jeans",
"Flying Nun",
"Flying Spaghetti Monster",
"Flying Spaghetti Monster",
"Flying Spaghetti Monster",
"Flying angry evil skull that flies upside-down when holy water is sprinkled on it",
"Foreign Host",
"Formido Oppugnatura Exsequens",
"Foul Ole Ron",
"Frankenstein",
"Fred",
"Fred Durst's disembodied head",
"Freddie Mercury",
"Freddy",
"Frog-eating surrender monkey",
"Fuzz golem",
"GEP Gun",
"Galactus",
"Galo Sengen",
"Gangsta",
"Garfield",
"Garfield the Cat",
"Garfield the President",
"George W. Bush",
"Geraldo Rivera",
"Ghost Dad",
"Ghoul Wizard",
"Giant Belly",
"Giant Hand",
"Giant Midget",
"Giant Rubix Cube",
"Giant Slobbering Piemonster",
"Giant Stick",
"Giant Tongue",
"Giant shoggoth",
"Giygas",
"Gizmo the Gremlin",
"Glinda",
"Gnome With A Wand Of Death",
"Gnome-With-The-Wand-Of-Death",
"Godzilla",
"Godzilla",
"Godzilla",
"Google monster",
"Gordon Freeman",
"Gouda Golem",
"Grammar Nazi",
"Grand master mind flayer",
"Grand master mind flayer",
"Grandma",
"Granny Weatherwax",
"Gravemind",
"Greased Scotsman",
"Greater Queen of England",
"Greebo",
"Green Flesh-Thresher",
"Grover",
"Grue",
"Gundam",
"Guymelef",
"HAL 9000",
"HURD kernel",
"Hacker",
"Hag of Bolding",
"Haggis Golem",
"Half Chewed Taxi Squasher",
"Halloween Document",
"Hallucinatory Monster",
"Hamburglar",
"Hammer Pants",
"Hanniwa",
"Happy Fun Ball",
"Harmless Protoplasm",
"Harry Potter",
"Hastur",
"Haxor",
"He-Man",
"Head Rot",
"Headcrab",
"Hell Baron",
"Here lies /usr/bin/nethack, killed by SIGSEGV.",
"Herobrine",
"Highwind Airship",
"Hildr, engraged",
"Hippopotamus",
"Holocaust",
"Holy Spirit",
"Honest Politician",
"Hong Kong Fooey",
"Humpty Dumpty",
"Hyperion",
"I think I see Death!",
"IFC Yipes",
"Ian-Keith on crack",
"Idiotic nerd that looks like your momma",
"Ig",
"Igor",
"Integrated Data Sentient Entity",
"Inu Yasha",
"Invisible Pink Unicorn",
"Iolo",
"It",
"It explodes!",
"Its your FATHER!",
"Its your MOTHER!",
"J. R. 'Bob' Dobbs",
"J.R.R. Tolkien",
"Jackie Chan",
"James Bond",
"Jason",
"Jean-Luc Picard",
"Jean-Paul Sartre",
"Jebus",
"Jerry Garcia",
"Jesus H. Christ",
"Jigglypuff",
"Jimmy Hoffa",
"John Madden",
"Johnny Depp",
"Juba the Sniper",
"Jubilex",
"Jumping brain",
"KOMPRESSOR",
"Kaster Maen",
"Katniss Everdeen",
"Kedama",
"Kermit the Frog",
"Ketchup Golem",
"Kibo",
"Killer Beatle",
"Killer Rabbit",
"Killer ant",
"Killer tripe ration",
"Kilrathi",
"King Kong",
"King Prawn",
"King Richard III",
"King of All Cosmos",
"Kirby <('_')>",
"Kit-Kat bar",
"Knob Goblin",
"Kounosuke Kuri",
"Kraid",
"Kung Fu Jesus",
"Kwyjibo",
"L dressed as Darth Vader, sans helmet, wielding a red lightsaber",
"Lady GaGa",
"Lady of the Lake",
"Lara Croft",
"Large Marge",
"Lavos",
"LeChuck",
"Legendary black beast of Arrrgh",
"Leonard Bernstein",
"Lex Luther",
"Life",
"Living Door",
"Long worm bug",
"Lorax",
"Lord British",
"Lord Goda",
"Lucius Malfoy",
"Luigi",
"M. C. Escher",
"M. Drew Streib",
"MAH LAZOR!",
"Magical Trevor",
"Magically animated vorpal blade",
"Magikarp",
"Mail Daemon",
"Major Victory",
"Malaclypse the Younger, Omnibenevolent Polyfather of Virginity in Gold",
"Male Daemon",
"Mammon",
"Mani Mani",
"Marduk/Merodach",
"Marilyn Monroe",
"Mario",
"Marmie",
"Martha Stewart",
"Martian jellymould",
"Mary-Kate and Ashely Olson",
"Master Chief",
"Master Light Wings Close Range Support Cruel Battle Machine Evaccania DOOM",
"Maud",
"Mayor and his three Daughters",
"Mecha-Streisand",
"Mega Man",
"Megadeus",
"Megatron",
"Mephistopheles",
"Metaknight",
"Metric stormtrooper",
"Mewtwo",
"Michael Jackson",
"Mickey Mouse",
"Microsoft Windows",
"Mike Nelson",
"Miku Hatsune",
"Misery",
"Miss Marple",
"Missingno",
"Mithos Yggdrasil",
"Mobile Suit",
"Moloch",
"Monkey Woman",
"Monkey-man of Delhi",
"Monster-with-a-petrifying-gaze",
"Mooninite",
"Morgoth",
"Most Interesting Man in the World",
"Mother-in-law",
"Mr Rogers",
"Mr. Blobby",
"Mr. Friend",
"Mr. Potato Head",
"Mr. T",
"Mudkip",
"Mumm-Ra",
"Mumm-Ra",
"Mushroom Giant",
"Mustard Golem",
"My Pet Monster!",
"NS13",
"Naughty Sorceress",
"Navi",
"Necromancer",
"Neil Diamond",
"Nelson Munce",
"NetHack player",
"New Age Retro Hippy",
"New Age Retro Hippy",
"Newtoghu",
"Nigerian prince Fela Kanye Okonma",
"Ninja Panda",
"Ninjapiratezombierobot",
"Nobody",
"Nuclear Bomb",
"Nyan Cat",
"Nyarlathotep",
"Og",
"Og",
"Old Scratch",
"Olly Jolly Puffball",
"Omar Khayyam Ravenhurst, K.S.C.",
"Omicronian",
"One-winged two wing bird",
"Oprah",
"Optimus Prime",
"Oracle publicitary blimp",
"Original Bubs",
"PK",
"Panty & Stocking",
"Pantyhose Golem",
"Papa Legba",
"Papa Smurf",
"Paxed",
"Penis Dragon",
"Personal Trainer",
"Peter Griffin",
"Peter Piper",
"Ph.D. Student",
"Phoenix Wright, Ace Attorney",
"Picasso",
"Pikachu",
"Pinhead",
"Pinkie Pie",
"Pit Pat",
"Poly gone",
"Polymorphic Virus",
"Polyself bug",
"Poppler",
"Porn Golem",
"Possessed waffle iron",
"Potted Plant",
"Power Rangers",
"Preacher man wants to save your soul",
"Prince",
"Probot",
"Professor Genki",
"Professor Snape",
"Psychadelic Eyeball",
"Pudding Farmer",
"Pyramid Head",
"Pyramidhead",
"Quote",
"RNG",
"RNG",
"RNG",
"Rabite",
"Racecar Bob and Bob Racecar",
"Rainbow Dash",
"Rakanishu wielding a MAC-10",
"Rancor Monster",
"Random Number God",
"Rarity",
"Rast",
"Ravenous Bugblatter Beast of Traal",
"Raymond Luxury Yacht",
"RedMachineD",
"Republican",
"Reshiram",
"Revan's Mom",
"Rhakkus",
"Richard Stallman",
"Rick James",
"Ridley",
"Robert, Wizard of Trebor",
"Robotic Kraken",
"Rodent of an Unusual Size",
"Rodney",
"Rodney",
"Rogue AI",
"Ron Jeremy",
"Ronald McDonald",
"Royal jelly",
"Rubicante",
"Ruby Weapon",
"SARS-in-a-can",
"SHODAN",
"Saddam Hussein",
"Saiyan",
"Sally Bowles",
"Sam the One-Eyed Marketeer",
"Samurai named Ken",
"Santa Claus",
"Sauron",
"Sauron",
"Scarmliogne",
"Schrödinger's cat",
"Scooby-Doo",
"Screaming Heebie-Jeebie",
"Scuzzlebutt",
"Senor Cardgage",
"Sentiant cheese sandwhich",
"Sergeant Schlock",
"Server",
"Shaggy",
"Shambler (Quake)",
"Shnardlewonk",
"Short worm",
"Shredder",
"Shroedinger's Cat",
"Shy Guy",
"Sigmund",
"Silent Bob",
"Silly-String golem",
"Sim@",
"Singing and dancing frog",
"Sister Mary Loquacious of the Chattering Order of Satanic Nuns",
"Skeletor",
"Skeletor",
"Skeletor",
"Skilled mind flayer",
"Skilled mind flayer",
"Slashdot Troll",
"Slenderman",
"Slenderman",
"Slenderman",
"Slim Shady",
"Slime Maul",
"Slime Maul",
"Slut",
"Small minotaur",
"Snoopy",
"Snowager",
"Sonic The Hedgehog",
"Soviet Russia",
"Spaceman Spiff",
"Spaghetti Giant",
"Spaghetti Monster",
"Speedrunner",
"Spellbook of wishing",
"Spider Pig",
"Spiderman",
"Stephen Colbert",
"Steve Jobs",
"Strong Bad",
"Struttin' Evil Mushroom",
"Stupendous Man",
"SubGenius",
"Super Mario",
"Super-Saiyan",
"Supercaptaincoolman",
"Superman",
"Surfshack Tito",
"Swine flu",
"Sylvester Stallone",
"TAEB",
"Taco Giant",
"Takakazu Abe",
"Talking Paperclip",
"Teen Titan",
"Teracotta Warrior",
"The Anti-Butler",
"The Big Cheese",
"The Bugblatter Beast of Traal",
"The Creeper",
"The Creeper (Jeepers Creepers)",
"The Curse of Jenni",
"The Destroyer of Levels",
"The Flying Spaghetti Monster",
"The Fonz",
"The Grateful Dead",
"The Grimace",
"The Last Unicorn",
"The Pink invisible Unicorn",
"The President",
"The RNG",
"The Real Slim Shady",
"The Red Sox",
"The Savage Decider",
"The Sound of Silence",
"The System",
"The Theory of Relativity",
"The Undertaker",
"The United States",
"The Zombie of Steve Irwin",
"The one-eyed flesh python",
"Thin air",
"Thing That Should Not Be",
"Thomas Biskup",
"Thursday Next",
"Tinkerbell",
"Tinky-Winky",
"Toilet Duck",
"Tom Nook",
"Tom Servo",
"Tonberry",
"Toroko",
"Toto",
"Trekkie Monster",
"Trogdor the Burninator",
"Turmaculus",
"Turmaculus",
"Turok-Han",
"Tutu-wearing Sarevok",
"Twilight Sparkle",
"UboaAAAAAAAAAAA~!",
"Umpah Lumpah",
"Uncle Henry",
"Undead Core",
"Undead angel",
"Underwater Basket Weaving Kobold",
"Unladen swallow",
"Unskilled mind flayer",
"Unskilled mind flayer",
"Upperclass Twit of the Year",
"Urist McDwarf",
"Ursa Major",
"Vaas Montenegro",
"Van Darkholme",
"Vault Dweller",
"Velma",
"Violist",
"Viper on his bike, dual-wielding chains",
"Vlad the Impala",
"Voldemort",
"Voltron",
"Vore (Quake)",
"Vrock Samson",
"W**dch*ck",
"Waffle",
"Waiter waiting for a #tip",
"Walpurgis Night",
"Wangsta",
"Weapons of Mass Destruction",
"Were-skunk",
"Wes Craven",
"When we introduced Eg to the giant, he misunderstood.",
"Whiteface Charcoalpants",
"Whiteface Charcoalpants",
"Whiteface Charcoalpants",
"Witchalok",
"Wizard of Yendoor",
"Wolfoid",
"Wombat",
"Wormtongue, Agent of Saruman",
"Xena",
"Xena, Warrior Princess",
"Xenu",
"Xivilai",
"Yendor",
"Yog-Sothoth",
"YouBane",
"Your Dad On A Sybian",
"Your Mom",
"Yourself",
"Yourself",
"Ysalamir",
"Yukari Yakumo",
"Zadir the Washer",
"Zaku",
"Zamboni",
"Zeeky H. Bomb",
"Zekrom",
"Zeruel",
"Zorn's lemma",
"a FNORD",
"a SPAM monster",
"a black dragon with Snoopy's head",
"a dentist",
"a flock of Bird Flu chickens",
"a garlic monster",
"a giant crab",
"a giant octopus",
"a mob of PETA activists",
"a one-winged dewinged stab-bat",
"a plain gold ring",
"a plastic surgeon",
"a praying mantis",
"a squee",
"a team of Caballists",
"a tree full of monkeys on nitrous oxide",
"a vision of Ragnarök",
"a whark",
"a winged serpent",
"a ytram",
"aardvark",
"absurd pop-culture reference",
"abyss",
"acid blog",
"africanized bee",
"albatross",
"alien",
"alien space bat",
"alluring being",
"aluminum elemental",
"ambulatory fortune cookie",
"amoeba",
"amusing movie reference",
"an Anger Management counsellor",
"an angry Madonna",
"an italian grandmother",
"an sandwich",
"ancient karmic dragon",
"angry Norwegian",
"angry mail demon",
"angry mariachi",
"antagonist",
"anteater",
"anticoffee",
"antimony elemental",
"arch-lichen",
"arch-pedant",
"archie the cockroach",
"argon elemental",
"armed bear",
"arsenic elemental",
"artila",
"astatine elemental",
"astral snake",
"astral wyvern",
"attack chopper",
"badger",
"baleen whale",
"bandersnatch",
"banshee",
"barium elemental",
"barrel goat",
"battle alpha",
"battle beta",
"battle bunny",
"battle mech",
"bee of the bird of the moth",
"beer-can golem",
"beholder",
"berdache with a bardiche",
"beryllium elemental",
"big red button",
"bismuth elemental",
"black hole",
"black smoke monster",
"black tapioca pudding",
"blargian snagglebeast",
"blood-spurting onion",
"blue light",
"blue whale",
"bluebird of happiness",
"bonta-kun",
"bonzai bush",
"boron elemental",
"bottle of Heinz Tomato Ketchup",
"bouncing eye",
"bowl of petunias",
"box ghost",
"boyfriend",
"broken clock",
"bromine elemental",
"brontosaurus",
"brony",
"brown dragon",
"buffer overflow",
"bugs on the floor",
"bungo pony",
"butler",
"cacodaemon",
"cadmium elemental",
"calcium elemental",
"candied kobold",
"capital I",
"carbon elemental",
"carbosilicate amorph",
"cardboard golem",
"casey",
"cesium elemental",
"chaos demon",
"cheese golem",
"chess pawn",
"chibi",
"chicken",
"chihuahua",
"chilli elemental",
"chlorine elemental",
"chocolate golem",
"chocolate pudding",
"chocolate pudding",
"christmas elf",
"chromium elemental",
"chupacabra",
"chupacabras",
"circus freak",
"clawbug",
"cobalt elemental",
"code fault",
"coelacanth",
"collapsed mine golem",
"colorless green idea",
"commissar",
"contrabassoon",
"convention furry",
"conventioneer",
"cookie monster",
"copper elemental",
"corpulent porpoise",
"cossack",
"cow",
"cow beneath the sea",
"crap fodder",
"crazy bastard",
"crazy bastard",
"crocodile-dragon Tharagavverug",
"cryoa",
"cryodrayk",
"cubist kobold",
"cursed amulet of strangulation",
"dancing Savior, Jesus Christ Superstar,",
"dancing blade",
"dangling pointer",
"dark archon",
"darkness owl",
"database",
"dead parrot",
"death vortex",
"demonic talking skull",
"diagonally moving grid bug",
"dick in a box",
"dilithium crystal golem",
"dilithium crystal golem",
"dirty bastard",
"disillusioner",
"district attorney",
"dolphin",
"door-to-door salesman",
"dopefish",
"double-eyed cyclops",
"dragon farmer",
"drakon",
"drayk",
"dropbear",
"drunk Dragonborn",
"drunk guy",
"dryer monster",
"duct tape golem",
"dust bunny",
"dvorak beast",
"dwarven construct",
"economic crisis",
"ego-death",
"elder xorn",
"emperor lich",
"empty cell",
"enchanted food ration",
"ender-dragon",
"enderman",
"entropy",
"er... dragon? It sure looks like one...",
"evil brain",
"evil overlord",
"evil stepmothership",
"exceptionally large pair of buttocks",
"excremental",
"existential angst",
"expert mind flayer",
"expert mind flayer",
"exploding cake",
"eyebeast",
"fail rat",
"fan",
"feeling that you're being watched",
"fetus",
"figment of your imagination",
"fire monkey",
"first category perpetual motion device",
"fish",
"flaming skull",
"flash of insight",
"flatulence vortex",
"fleas",
"floating ear",
"floating nose",
"floating pair of eyes",
"floating tongue",
"floor",
"flumph",
"fluorine elemental",
"flying pig",
"fnord",
"foocubus",
"food eater",
"football",
"footrice",
"fractal fish finger tree",
"frerf",
"frog prince",
"fruit bat",
"frumious bandersnatch",
"funkateer",
"funny little man",
"furby",
"furry",
"future diary holder",
"fyora",
"gallium elemental",
"gang member",
"gaping goatse",
"gargantua",
"gazebo",
"gazer",
"gerbil",
"germanium elemental",
"ghost of christmas past",
"ghost of christmas present",
"ghost of christmas yet to come",
"giant Chibi Maruko-chan on wheels",
"giant bong",
"giant cockroach",
"giant dwarf",
"giant enemy crab",
"giant gnome",
"giant letter D",
"giant marshmallow man",
"giant mushroom",
"giant robot",
"giant squid",
"gigantic kobold",
"gimp",
"gingerbread man",
"girlfriend",
"glahk",
"glom of nit",
"gnu",
"gold elemental",
"golden goblin",
"golem made out of other golems",
"gonzo journalist",
"goomba",
"grad student",
"grass mud horse",
"gray goo",
"gray stoner",
"grease golem",
"greased pig",
"green light",
"green... no, gray dragon",
"grid bug hits! Invisible Demogorgon",
"grid feature",
"grinch",
"grue",
"guardianal",
"haggis",
"hair golem",
"half-hobbit",
"half-horse half-monkey",
"hallucination",
"hallucinatory monster",
"hallucinogen-distorted hallucination",
"hamster",
"hardware bug",
"haunted television",
"haxorus",
"head crab",
"headless Mímir",
"headless thompson gunner",
"hedge golem",
"hedgehog",
"heisenbug",
"helium elemental",
"henchman",
"hentai tentacle beast",
"herobrine",
"hipster",
"hockey elemental",
"horrible gelatinous blob",
"hotel detective",
"humorous message",
"hydrogen elemental",
"hypnosis vortex",
"icecream pooping giant taco monster",
"imaginary number",
"indescribable horror",
"indestructable monster",
"indium elemental",
"infernal seal",
"inside joke",
"inside-out bag of holding",
"invisible hand of Adam Smith",
"invisible pink unicorn",
"invisible potion of invisibility",
"invisible shiny Bulbasaur",
"iodine elemental",
"iridium elemental",
"iron elemental",
"janitor",
"jehovah's witness",
"jell-o golem",
"jolly green giant",
"jophur",
"jub-jub bird",
"juvenile delinquent",
"kangaroo",
"katamari",
"keebler elf",
"kernel bug",
"killer bunny",
"killer tomato",
"kitten prospecting robot",
"kl;kl;kl;kl",
"klingon warrior",
"koala",
"krypton elemental",
"kzin",
"laboratory mouse",
"lag monster",
"land octopus",
"land wyvern",
"large letter",
"laser cat",
"lawnmower",
"lawyer",
"lead elemental",
"left eye",
"lego brick",
"lemon",
"lemur",
"liquid nitrogen elemental",
"lithium elemental",
"little brother",
"little green man",
"living apple",
"loan shark",
"logic bug",
"lol man",
"lombax",
"long worm brain",
"lorax",
"lower-case h",
"lumberjack",
"lungfish",
"lurker below",
"machine elf",
"mad Pierson's Puppeteer",
"mad god",
"mad scientist",
"mad scientist",
"mage jackal",
"magnesium elemental",
"magnetic monopole",
"mamul",
"man in a pink tutu",
"man-eating banana",
"manbearpig",
"manbearpig",
"manganese elemental",
"marshmallow peep",
"martian flat cat",
"master baker",
"master grid bug",
"master lichen",
"mehitabel the cat",
"member of the Greate Race of Yith",
"meme monster",
"mentat",
"mercury elemental",
"metroid",
"mexican jumping beans",
"midget",
"mind vortex",
"molybdenum elemental",
"morlock",
"mothership",
"mountain dwarf",
"moving hole",
"mumak mummy",
"mummy zombie",
"munchkin",
"mutant rat",
"mysterious force",
"mysterious force",
"naughty sorceress",
"neon elemental",
"neosquid",
"net troll",
"nethaxorus",
"newsgroup troll",
"newt called Dudleyslayer",
"newt mummy",
"nickel elemental",
"nigiri monster",
"ninja pirate zombie robot",
"ninja snowman",
"nitrogen elemental",
"non-player character",
"nonesuch",
"oblivion",
"obstinate tank",
"octarine dragon",
"octarine dragon",
"octarine mold",
"oiraM repuS",
"on the succubus",
"one-eyed, one-horned, flying, purple people eater",
"opossum",
"orangutan",
"ornk",
"osmium elemental",
"osquip",
"oxygen elemental",
"paint blob",
"palladium elemental",
"parachute pants",
"parliamentarian",
"particle man",
"paxed",
"peer",
"penguin",
"peregrine falcon",
"perfume vortex",
"phoenix",
"phosphorus elemental",
"pime taradox",
"pink dinosaur",
"pink dragon",
"pink elephant",
"pirate",
"pirate ninja",
"pissed-off shopkeeper",
"pixel golem",
"pizza delivery boy",
"plaid blob",
"plaid dragon",
"plaid dragon",
"plain trained automaton",
"plated bug",
"platinum elemental",
"platypus",
"player",
"playername",
"plumper",
"plutonium elemental",
"polka-dot dragon",
"pop-culture reference",
"pop-up ad",
"pope",
"pot fiend",
"potassium elemental",
"potentate of amnesia",
"potted plant",
"poultrygeist",
"power armor Hitler",
"pr0n prawn",
"prairie dog",
"president of the United States",
"primal white jelly",
"prismatic dragon",
"prissy, two-faced, backstabbing Templar whore",
"professional wrestler",
"programmer",
"psychedelic beetle",
"psycho teddy bear",
"puppeteer",
"purple dinosaur",
"purple haze",
"purple unicorn",
"pyrex golem",
"pyroroamer",
"qwerty beast",
"rabid lawyer",
"rabid squirrel",
"radium elemental",
"radon elemental",
"raging nerd",
"rakshasha",
"random monster",
"random number generator",
"random number golem",
"rare nymph",
"rat king",
"realization of your life's true meaning",
"really, really, really big dragon",
"red D",
"red d",
"red light",
"redditor",
"redneck tree",
"repo man",
"republican",
"rhodium elemental",
"rice pudding",
"right eye",
"road runner",
"roamer",
"robot finding stegosaurus",
"roomba",
"roostertrice",
"rotdhizon",
"rotghroth",
"rubber baby buggy bumper",
"rubber chicken",
"rubber ducky",
"rubber golem",
"rubidium elemental",
"saiph",
"samoan attorney",
"sauceror",
"scaly, no-feather-bullshit, kosher raptor",
"scandium elemental",
"scary clown",
"scary devil monk",
"scrag",
"screaming dizbuster",
"scroll of genocide",
"sea cucumber",
"sea orc",
"sea wyvern",
"searing artila",
"second law of thermodynamics",
"segmentation fault",
"selenium elemental",
"seven-headed, fire-breathing $dog",
"shawnz",
"sheep",
"shimmering $playername",
"shimmering dragon",
"shinigami",
"shoggoth",
"shoggoth",
"shrubbery",
"silicon elemental",
"silly in-joke",
"silver elemental",
"silver saber-tooth tiger",
"sinking ear",
"sky wyvern",
"sleaze elemental",
"sleestak",
"slime mold",
"slow loris",
"small letter",
"smurf",
"snark",
"snow golem",
"snow goon",
"social worker",
"sodium elemental",
"sodium elemental",
"space-time anomaly",
"spam dragon",
"spammer",
"spelling mistake",
"spleling eror elmental",
"spoiler bug",
"spotted dragon",
"spycrab",
"star-bellied sneetch",
"star-nosed mole",
"starman",
"stasis vortex",
"stdev",
"stick figure",
"storm of buttered footballs",
"stormtrooper",
"strawberry jelly",
"street preacher",
"strontium elemental",
"strontium elemental",
"stuffed raccoon puppet",
"styrofoam elemental",
"succutrice",
"sulfur elemental",
"sumatran rat-monkey",
"super grue",
"surfer dude",
"sysadmin",
"system administrator",
"tapeworm",
"taxman",
"teal deer",
"technetium elemental",
"telemarketer",
"tellurium elemental",
"terror vlish",
"thahd",
"thahd shade",
"the Candyman",
"the Chupacabra",
"the Creeping Terror",
"the DevTeam",
"the F.O.E.",
"the Goblin King",
"the Godfather",
"the Incredible Hulk's weight in bees",
"the Internet",
"the Joker",
"the Loch Ness monster",
"the ROFLchopter",
"the ROFLcopter",
"the Roomba",
"the Scarecrow",
"the Spice Girls",
"the Teenage Mutant Ninja Turtles",
"the Terminator",
"the Three Stooges",
"the White Witch",
"the Wicked Witch of the East",
"the Wicked Witch of the West",
"the Wizard of Oz",
"the Wizard of Yeldud",
"the blessed +5 Sword of Damocles",
"the cyberdemon",
"the darkness",
"the embodiment of life, death and everything inbetween",
"the ghost of Tupac Shakur",
"the goddess Discordia",
"the goddess Eris",
"the hellbride",
"the invisible Demogorgon",
"the invisible cockatrice",
"the invisible werewaiter",
"the largest prime number",
"the paparazzi",
"the road",
"the roots of Yggdrasil",
"the smashing pumpkin",
"the urge that you're being watched",
"the urge to look at your hands",
"thing",
"thing",
"thing with 2.3i heads",
"thoroughly rusty thoroughly corroded iron golem",
"three weaving crones",
"three-headed cockatrice",
"three-headed monkey",
"three-headed monkey",
"thunder fiend of gain level",
"thwomp",
"timberwolf",
"time paradox",
"timey wimey ball",
"tin elemental",
"tin of gnome meat",
"titanium elemental",
"toilet paper golem",
"topiary golem",
"trapper",
"triple Gandalf",
"trippy vortex",
"trouser snake",
"truck driver",
"tube rat",
"tubgirl",
"tungsten elemental",
"turbonium dragon",
"turducken",
"twen",
"two-headed two-bodied goat",
"twonky",
"tyger",
"ultra mega battle robot",
"umpire",
"unassuming local guy",
"universe",
"unpredictably deadly pyromaniac midget",
"ur-drakon",
"ur-glahk",
"uranium elemental",
"urge to stare at your hands",
"vampire bird",
"vampire chicken",
"vampire slayer",
"vanadium elementa",
"vanadium elemental",
"velcro golem",
"velociraptor",
"vending machine",
"venture capitalist",
"vermicious knid",
"vertigo vortex",
"viking",
"violent fungus",
"violent fungus",
"visible stalker",
"vlish",
"vogon",
"voluptuous ampersand",
"vorpal jabberwock",
"vortigaunt",
"vulture",
"waddle dee",
"waddle doo",
"waddle doo",
"walking disk drive",
"walrus",
"wanderer",
"warning level 1",
"warning level 2",
"warning level 3",
"warning level 4",
"warning level 5",
"water buffalo",
"weasel",
"wee green blobbie",
"were(random beast)",
"werecar",
"weredolphin",
"werefoo",
"werehorse",
"werehouse",
"werehuman",
"weremindflayer",
"weremold",
"weremouse",
"wereplatypus",
"white rabbit",
"wiener sausage",
"wif dove",
"wight supremacist",
"willow-the-wisp",
"wombat",
"wookie",
"worthless yellowish-brown glass golem",
"writhing mass of primal chaos",
"wub",
"wumpus",
"wumpus mummy",
"xenon elemental",
"xoff",
"xon",
"xorff",
"yellowish-brown dragon",
"your ex",
"your foot",
"your inner demons",
"your mom",
"your mom",
"your own hands",
"your pet idiot",
"ysalamiri",
"yttrium elemental",
"zappy eel",
"zappy eel",
"zergling",
"zinc elemental",
"zirconium elemental",
"zmobie",
"zombi Oracle",
"zombie mummy",
"zombie robot ninja samurai pirate sorcerer gunman",
"zombie sasquatch",
"zombie zombie",
"zombified tarantulas",
"zorkmid",
	"Digger",
	"Field Worker",
	"Investigator",
	"Exhumer",
	"Excavator",
	"Spelunker",
	"Speleologist",
	"Collector",
	"Curator",
	"Quetzalcoatl", "Camaxtli", "Huhetotl", /* Central American */
	"Plunderer",   "Plunderess",
	"Pillager",
	"Bandit",
	"Brigand",
	"Raider",
	"Reaver",
	"Slayer",
	"Chieftain",   "Chieftainess",
	"Conqueror",   "Conqueress",
	"Mitra", "Crom", "Set", /* Hyborian */
	"Troglodyte",
	"Aborigine",
	"Wanderer",
	"Vagrant",
	"Wayfarer",
	"Roamer",
	"Nomad",
	"Rover",
	"Pioneer",
	"Anu", "Ishtar", "Anshar", /* Babylonian */
	"Detainee",
	"Inmate",
	"Jail-bird",
	"Prisoner",
	"Outlaw",
	"Crook",
	"Desperado",
	"Felon",
	"Fugitive",
	"Ilmater", "Grumbar", "Tymora",	/* Faerunian */
	"Filthy Worm",
	"Creeping Maggot",
	"Lowly Being",
	"Black Mage",
	"Curse Caster",
	"Imperious",
	"Crucious",
	"Deathbringer",
	"Dark Lord's Bodyguard",
	"Merlin", "Salazar Slytherin", "Lord Voldemort", /* Harry Potter */
	"Charger",
	"Bolter",
	"Buzzer",
	"Hummer",
	"Energic",
	"Arclite",
	"Volt Technician",
	"Thundermage",
	"Shock-Master",
	"Thomas Alva Edison", "Benjamin Franklin", "Marilyn Monroe", /* associated with electricity */
	"Spark",
	"Igniter",
	"Broiler",
	"Combuster",
	"Torcher",
	"Scorcher",
	"Incinerator",
	"Disintegrator",
	"Flame-Master",
	"Earth", "Fire", "Ash", /* Special */
	"Low Thug",
	"Pickpocketer",
	"Street Criminal",
	"Carjacker",
	"Wanted Criminal",
	"Gang Member",
	"Mafia Member",
	"The Don's Right Hand",
	"Mafia Don",
	"Claude Speed", "Carl CJ Johnson", "Tommy Vercetti",	/* Grand Theft Auto */
	"Newbie",
	"BASIC Programmer",        "BASIC Programmeress",
	"C Programmer",        "C Programmeress",
	"Hacker",        "Hackeress",
	"NetHacker", "NetHackeress",
	"Nethack Programmer",      "Nethack Programmeress",
	"he who uses",       "she who uses",
	"he who knows",      "she who knows",
	"he who learns",     "she who learns",
	"UNIX", "the PDP-7", "VMS", /* Computerian */
	"Rhizotomist",
	"Empiric",
	"Embalmer",
	"Dresser",
	"Medicus ossium", "Medica ossium",
	"Herbalist",
	"Magister",       "Magistra",
	"Physician",
	"Chirurgeon",
	"Athena", "Hermes", "Poseidon", /* Greek */
	"Cooler",
	"Condenser",
	"Chiller",
	"Froster",
	"Permafroster",
	"Icer",
	"Freezer",
	"Sublimer",
	"Ice-Master",
	"Air", "Frost", "Smoke", /* Special */
	"Youngling",
	"Padawan",
	"Jedi Apprentice",
	"Jedi Knight",
	"Jedi Hero",
	"Jedi Master",
	"the Light Side", "the Force", "the Dark Side",
	"Gallant",
	"Esquire",
	"Bachelor",
	"Sergeant",
	"Knight",
	"Banneret",
	"Cavalier",   "Cavaliere",
	"Seignieur",   "Dame",
	"Paladin",
	"Lugh", "Brigit", "Manannan Mac Lir", /* Celtic */
	"Candidate",
	"Novice",
	"Initiate",
	"Student of Stones",
	"Student of Waters",
	"Student of Metals",
	"Student of Winds",
	"Student of Fire",
	"Master",
	"Shan Lai Ching", "Chih Sung-tzu", "Huan Ti", /* Chinese */
	"Gravedigger",
	"Embalmer",
	"Mortician",
	"Zombie Lord",
	"Ghoul Master",
	"Necromancer",
	"Undead Master",
	"Lich Lord",
	"Nharlotep", "Zugguthobal", "Gothuulbe", /* Assorted slimy things */
	"Pargar",
	"Cneaz",
	"Ban",
	"Jude",
	"Boier",
	"Cupar",
	"Clucer",
	"Domn",
	"Domnitor",
	"God the Father", "Mother Earth", "the Satan", /* Romanian, sorta */
	"Landlubber",
	"Swabbie",
	"Cutthroat",
	"Bosun",
	"Second Mate",
	"First Mate",
	"Captain",
	"Pirate Lord",
	"Dread Pirate",
	"the Lord", "the deep blue sea", "the Devil",	/* Christian, sorta */
	"CROC",
	"ALIGE",
	"BEIL",
	"MADAM",
	"GUAIL",
	"BANQ",
	"PIJIA",
	"CLAU",
	"MIY",
	"A'En", "Dr. Oujide", "Team Missile Bomb", /* Pokemon Vietnamese Crystal */
	"Footpad",
	"Cutpurse",
	"Rogue",
	"Pilferer",
	"Robber",
	"Burglar",
	"Filcher",
	"Magsman",     "Magswoman",
	"Thief",
	"Issek", "Mog", "Kos", /* Nehwon */
	"Edhel",       "Elleth",
	"Edhel",       "Elleth",      /* elf-maid */
	"Ohtar",       "Ohtie",       /* warrior */
	"Kano",			/* commander (Q.) ['a] */
			"Kanie",	/* educated guess, until further research- SAC */
	"Arandur",			/* king's servant, minister (Q.) - guess */
			"Aranduriel",	/* educated guess */
	"Hir",         "Hiril",       /* lord, lady (S.) ['ir] */
	"Aredhel",     "Arwen",       /* noble elf, maiden (S.) */
	"Ernil",       "Elentariel",  /* prince (S.), elf-maiden (Q.) */
	"Elentar",     "Elentari",	/* Star-king, -queen (Q.) */
	"Solonor Thelandira", "Aerdrie Faenya", "Lolth", "Erevan Ilesere", /* Elven */
	"Tenderfoot",
	"Lookout",
	"Trailblazer",
	"Reconnoiterer", "Reconnoiteress",
	"Scout",
	"Arbalester",
	"Archer",
	"Sharpshooter",
	"Marksman",      "Markswoman",
	"Mercury", "Venus", "Mars", /* Roman/planets */
	"Hatamoto",
	"Ronin",
	"Ninja",       "Kunoichi",  /* secret society */
	"Joshu",
	"Ryoshu",
	"Kokushu",
	"Daimyo",
	"Kuge",
	"Shogun",
	"Amaterasu Omikami", "Raijin", "Susanowo", /* Japanese */
	"Rambler",
	"Sightseer",
	"Excursionist",
	"Peregrinator","Peregrinatrix",
	"Traveler",
	"Journeyer",
	"Voyager",
	"Explorer",
	"Adventurer",
	"Blind Io", "The Lady", "Offler", /* Discworld */
	"Clacker",
	"Staggerer",
	"Pseudo Model",
	"Disguiser",
	"Carnevalist",
	"Heeler",
	"Crossdresser",
	"Drag Lord",
	"Drag Queen",
	"Olivia", "Peyman", "Lady Gaga", /* weird fashion sense */
	"Assistant",
	"Eliminator",
	"Exterminator",
	"Destroyer",
	"Vindicator",
	"Undead Slayer",
	"Seeker", "Osiris", "Seth", /* Egyptian */
	"Stripling",
	"Skirmisher",
	"Fighter",
	"Man-at-arms", "Woman-at-arms",
	"Great Warrior",
	"Swashbuckler",
	"Hero",        "Heroine",
	"Champion",
	"Lord",        "Lady",
	"Tyr", "Odin", "Loki", /* Norse */
	"Evoker",
	"Conjurer",
	"Thaumaturge",
	"Magician",
	"Warlock",     "Witch",
	"Enchanter",   "Enchantress",
	"Sorcerer",    "Sorceress",
	"Wizard",
	"Mage",
	"Ptah", "Thoth", "Anhur", /* Egyptian */
	"Swordsman",
	"Longswordsman",
	"Two-Handed Swordsman",
	"Legionnaire",
	"Crusader",
	"Baron",    "Baroness",
	"Count",   "Countess",
	"Duke",   "Duchess",
	"Patriarch",     "Matriarch",
	"Talos", "Meridia", "Clavicus Vile", /* The Elder Scrolls */
	"Usher",
	"Steward",        "Stewardess",
	"Keeper",
	"Marshal",
	"Master Steward", "Master Stewardess",
	"Chamberlain",
	"Constable",
	"Chancellor",
	"Regent",
	"His Majesty", "His Holiness", "The Commons", /* The three estates */
	"Basher",
	"Undefiler",
	"Faithful",
	"Religious One",
	"Returned One",
	"Sacred One",
	"Celestial",
	"Decomposer",
	"Annihilator",
	"Buddha", "Jahwe", "Allah",	/* major real-world religions */
	"Vault Dweller",
	"Hiker",
	"Urban Legend",
	"Landstalker",
	"Searcher",
	"Capital Crime Queen",
	"Opportunist",
	"Peacebringer",
	"Messiah",
	"President Kimball", "Mr. House", "Caesar",	/* Fallout New Vegas */
	"Aspirant",
	"Acolyte",
	"Adept",
	"Priest",      "Priestess",
	"Curate",
	"Canon",       "Canoness",
	"Lama",
	"Patriarch",   "Matriarch",
	"High Priest", "High Priestess",
	"Tin Can",
	"Artificial Stupidity",
	"Metal Man",   "Metal Woman",
	"Automaton",
	"Mechwarrior",
	"Android",
	"Advanced Robot",
	"Artificial Intelligence",
	"Synthetic Man",  "Synthetic Woman",
	"Andromorph", "Technix", "Mechatron",	/* made-up names by Amy */
	"Oozer",
	"Slimer",
	"Sludge Mage",
	"Spiller",
	"Corroder",
	"Acidsplasher",
	"Hazardous Materials Specialist",
	"Laboratory Science PhD",
	"Acid-Master",
	"The Lord of the Pit", "Goldblight of the Flame", "Warpfire Hellspawn",
	"Whistleblower",
	"Tooter",
	"Hooter",
	"Violinist",
	"Guitarist",
	"Bassist",
	"Percussionist",
	"Harper",
	"Synthesizer",
	"Classic Rock", "Symphonic Metal", "Hardcore Punk",	 /* music styles */
	"Roguelike Beginner",
	"Dungeon Explorer",
	"Sword Swinger",
	"Burly Combatant",
	"Heroic Mage",
	"Battlemage",
	"Spellsword",
	"Nightblade",
	"King's True Heir",
	"Lady Ariane", "Lord Stahngnir", "Sven Fanara", /* from a certain book --Amy */
	"Exile",
	"Heratic",
	"Cultist",
	"Real Binder",
	"Akousmatikoi",
	"Mathematikoi",
	"Doctor",
	"Unbinder",
	"Gnostikos",
	"Yaldabaoth", "the void", "Sophia", /* Gnostic */
	"Sweet Girl",
	"Catwalk Tester",
	"Curved Woman",
	"Playboy Bunny",
	"Erotic Lady",
	"Lovely Chick",
	"Sexy Poser",
	"ANTM Semi-Finalist",
	"ANTM Finalist",
	"Speedy Gonzales", "Dan Naginati", "Kylie Lum", /* taken from a fanfic */
	"Little Rebel",
	"Do-Gooder",
	"Demonstrant",
	"Worker on strike",
	"Protestor",
	"Rebellious Individual",
	"Involved Activist",
	"Renegade Fighter",
	"Savior",
	"Anti-War Movement", "Global Freedom Council", "Human Rights Progression", /* buzzwords used by activists */

"a series of disconected lines", /* nondescript*/ /*DnD*/ "a cerulean weeping-willow", /* it's magic. Unlike the others, this one works. Keep in sync with engrave.h!*/ /*Special behavior, these move across the floor, keep in sync with allmain.c*/ "a north-east facing glider", "a north-west facing glider", "a south-west facing glider", "a south-east facing glider", "a square", /* books */ "a set of holy horns", "a Summoning Dark mine-sign", "a Long Dark mine-sign", "a Following Dark mine-sign", "a Closing Dark mine-sign", "an Opening Dark mine-sign", "a Breathing Dark mine-sign", "a Speaking Dark mine-sign", "a Catching Dark mine-sign", "a Secret Dark mine-sign", "a Calling Dark mine-sign", "a Waiting Dark mine-sign", "a florid crest dominated by a double-headed bat", "a Guarding Dark mine-sign", "the mark of the Caller of Eight", /* Discworld */ "a lidless eye", /* Lord of the Rings */ "a white tree", /* Gondor, Lord of the Rings */ "a triangle enclosing a circle and bisected by a line", /* Harry Potter */ "a set of three trefoils, the lower most inverted", /* describes the three of clubs. Too Many Magicians*/ "a Trump of Doom", "a Sign of Chaos", "a Pattern of Amber", "a Ghostwheel", "a Court symbol", "a Forest symbol", "the sign of the Wandering Eye", /* Gunnerkrigg Court */ /* Not quite */ "a heptagenarian", "an octogram", "a pentagrain", "a circle of da Vinci", "a hand making a rude gesture", "a junior sign", "a childish compound eye", "a Sign of an Illegitimate Step-daughter", "a cenotaph of a catgirl", "a groovy rendition of the wings of Gargula", "a Robotech Defense Force insignia", /*...Robotech*/ "a Black Knights insignia", /* Code Geass */ "an inverted triangle flanked by seven eyes", /* NGE */ "a laughing man", /* Ghost in the Shell */ "an alchemic array", "a human transmutation circle", /* Fullmetal Alchemist */ "a triangle composed of three smaller triangles", "an eye and single tear", "a circle enclosing four swirling lines", "a flame inside a circle", "a snowflake within a circle", "an inverted triangle with a dot above each face, enclosed by a circle", "a sign resembling an eyeless yin-yang", "a circle surrounding a triangle of dots and another of triangels",/*Zelda*/ "a setting (rising?) sun", /* Dresden Codak */ "an asymmetric, stylized arrowhead, point upwards", /* Star Trek*/ "a set of three blades, the top blade straight, the dexter curved down, the sinister curved up", "a Sharuan Mindharp", /* Star Wars expanded universe */ "a winged blade of light", /* Jedi Order symbol */ "an angular S before a segmented circle",/*a screw attack symbol*/ "more dakka", "a symbol of pain", /* DnD */ /* Planescape */ "a mimir", "a symbol of torment", "a circle enclosing two colliding arrows", "a pair of triangles, drawn tip to tip,", "a stylized beast", "a triangle crowned by a single line", "a simple image of many mountains", "a sketch of a shining diamond", "a tree-rune", "an eight-toothed gear", "a random scribble", "a square with two small handles on opposite sides", "a square enclosing a spiral", "an eye with three inverted crosses", "an infinity symbol crossed by a burning downwards arrow", "a set of four nested triangles", "a watchful eye blocking an upward arrow", "a pitchfork stabbing the ground", /* Zodiac */ "an Aries sign", "a Taurus sign", "a Gemini sign", "a Cancer sign", "a Leo sign", "a Virgo sign", "a Libra sign", "a Scorpio sign", "a Sagittarius sign", "a Capricorn sign", "an Aquarius sign", "a Pisces sign", "a heart pierced through with an arrow", "a broken heart", "a skull and crossed bones", "a bad situation", "a zorkmid", "a diagram of the bridges of Konigsberg", "a hand-mirror of Aphrodite", "a shield and spear of Ares", /* alchemy/male/female */ "a black moon lilith sign", "a window", /* op-sys*/ "a no symbol", "a test pattern", "a work of modern art", "a flag of Neverland", "a hyped-up duck dressed in a sailor's shirt and hat", /* Disney */ "a mouse with 2d ears", "a set of three circles in the shape of a mouse's head", "a meaningless coincidence", /*Corporate Logos*/ "a stylized, fan-shaped seashell", "a bitten apple", "a pair of arches meeting to form an \"M\"", "a Swoosh mark", "a set of five interlocked rings", /*Olympics logo*/ "a running man", /* Exit */ "a running man holding a cane", "a one-and-zero", /* Power toggle */ "a thick soup of mist", "a pattern of squared circles", "a void", "a notable lack of images", "a stark absence of pictures", "nothing much", "a convergence of parallel lines", "a sphere", /* How did you manage that? */ "a yin-yang", "a taijitu",/* Taoist */ "a hand of Eris", /* Discordian */ "an ichthus", "a Cross", /* Christian*/ "a wheel with eight spokes", /* Budhism */ "a fish with legs", "a fat fish", "a fish with tentacles, legs, and wings", /* ichthus parodies/derivitives: darwin, buddha, and Cthulhu. */ "a set of seven concentric circles", "a left-handed trefoil knot", "a triskelion", /* Ancient Symbol */ "a rough circle enclosing an A", /* Anarchy */ "a Tree of Life", /* Kabbalah */ "a winged oak", "a wheel cross", "a labyrinth", "sign of Shamash", "a naudh rune", /* misery */ "an Eye of Providence", "a pyramid surmounted by an eye", /* Christian */ "a one-way staircase", "an 'a' encircled by its own tail", /* meta */

"Leg-scratched Boy",        "Leg-scratched Girl",
"Wounded Guy",      "Wounded Gal",
"Dripper",
"Pourer",
"Red Lake Creator",
"Tearshedder",
"Unlucky Individual",
"Diseased Sibling",
"Cursed King",    "Cursed Queen",
"Glycocalyx", "Fibrinogen", "Hemophilia", /* hereditary disease */

	"Lubber",
	"Swabby",
	"Deckhand",
	"Bilge Rat",
	"Helmsman",     "Helmswoman",
	"Navigator",
	"Bosun",
	"Mate",
	"Commodore",
	"Erzulie Freda", "Marassa Jumeaux", "Papa Legba",	/* Haitian Vodou */

	"Velite",
	"Thraex",
	"Dimachaerus",
	"Venator",
	"Murmillo",
	"Hoplomachus",
	"Retiarius",
	"Secutor",
	"Centurion", /* sorry Fyr, but Champion was just way too generic of a title. --Amy */
	"Sulla", "Cicero", "Catilina",	/* Ancient Rome */

	"Beachcomber",
	"Paddler",
	"Snorkeler",
	"Swimmer",
	"Wave-rider",
	"Cruiser",
	"Surfer",
	"Spearfisher",
	"Torpedo",
	"Tane", "Tangaroa", "Whiro",	/* Polynesian */
	"robber", "homie", "motherfucker", "mofo", "hell bride", "bitch", "hard motherfucker", "hard mofo", "slut with syphilis", "lustful girl", "quick learner", "supersmart woman", 
"Wolf", "Big Bear", "Ryu", "Tacitus", "Urbaldi", "Pete", "Lex", "JoJo", "Jyllia", "Sabrina", "Sabine", "Yvara", "Lenka",
"Evita", "Liebea", "Denshi Gasu", "Mr. Black", "Tiger's Claw", "Katzou", "Mohmar Deathstrike", "Ingo", "Septimus",
"Isolde", "Elli", "Vilja", "Sunija", "Rhea", "Jasmin", "Erosina", "Irmina", "Martius", "Faster-Than-All-Others",
"Senator Antius", "H.", "Pokoh", "Davide", "Aee", "Melirija", "Larissa", "Sysette", "Miss Haskill", "Elenya",
"Golden Mary", "Lara", "Sandrina", "Doctor Maex", "Marc", "Arno", "Hailbush", "Romann", "Siegfried", "Roy", "Tonilia",
"Claire", "Lumia", "Lahira", "Estrella", "Maricia", "Sontaire", "Marje",

"Jill", "Trycja", "Kersey", "Sally", "Hannya", "Svantje", "Jynnifyr", "Elke", "Rinka", "Nicoletta", "Betti", "Ina", "Heikipa", "Jora", "Maitine", "Esruth", "Verene", "Lousie", "G-cheater", "Irinella", "Bastian", "Amandina", "Lillie", "Nicyan", "Leodoch", "Mirella", "Queelix", "Fisoa", "Suesska", "Ann", "Nurisha", "Desiree", "Birgit",

"Elsbeth", "Lamy", "Lissie", "Arabella", "Anastasia", "Henrietta", "Katrin", "Jana", "Aniya", "Yasni", "Almina", "Xeni", "Mirri", "Eleanor", "Kirja", "Inge", "Helli", "Lucia", "Viktorija", "Simona", "Natalyana", "Krista", "Nellina", "Raidara", "Vera", "Noko", "Jasajeen", "Marika", "Miesael", "Merbek", "Marianna", "Sinja", "Rodotha", "Natinya", "Honno", "Aline", "Michaela", "Robin", "JNR", "Lars", "Mare", "Noenoe", "Tschulia", "Lea", "Tommy", "Sarah", "Iris",

"Giglio", "Charravalga", "Fridrika", "Great Jaguar Claw", "Lynette", "Kastortransport", "Celina", "Irya", "Mariya", "Wendy", "Katia", "Tanja", "Vanessa", "Anne", "Lena", "Jeanetta", "Rungud", "Melissa", "Everella", "Madeleine", "Anita", "Nina", "Natascha", "Manola", "Larry", "Morton", "Iggy", "Lemmy", "Ludwig", "Oberdan", "Len-kind", "Litta", "Ilie", "Kiwi", "Maja", "Till", "Tomas", "Natalje", "Little Marie", "Nikolob", "Tillbull", "Ronja", "Roswitha", "Sing", 

	"Cantripper",
	"Spoonbender",
	"Kinetic",
	"Seer",
	"Psychic",
	"Oracle",
	"Levitator",
	"Visionary",
	"Master Psion",
	"Bickney", "Corridor", "Lockney", /* Egyptian */
	"Tube Mixer",
	"Practician",
	"Advanced Practician",
	"Experimentator",
	"Test Runner",
	"Graduate Scientist",
	"Simpleton with a PhD",
	"Rocket Scientist",
	"Nobel-Prized Scientist",
	"Nikola Tesla", "Erwin Schroedinger", "Wernher von Braun", /* famous scientists */
	"Howler",
	"Crazy Person",
	"Moon Worshipper",
	"Weirdo",
	"Shapeshifter",
	"Warped Person",
	"Wereperson",
	"Asylum Escapee",
	"Master Shapeshifter",
	"Eluvian", "Moon", "Lycanthus",	/* from an old SLASH version without extended magic */

       "blessed greased +5 silly object of hilarity",
       /* Modern */
       "polo mallet",
       "string vest",
       "YAFM",                             /* rgrn */
       "applied theology textbook",        /* AFutD */
       "handbag",
       "onion ring",
       "tuxedo",
       "breath mint",
       "potion of antacid",
       "traffic cone",
       "chainsaw",
       "pair of high-heeled stilettos",    /* the *other* stiletto */

       /* Silly */
       "left-handed iron chain",
       "holy hand grenade",                /* Monty Python */
       "decoder ring",
       "amulet of huge gold chains",       /* Foo' */
       "rubber Marduk",
       "unicron horn",                     /* Transformers */
       "holy grail",                       /* Monty Python */
       "chainmail bikini",
       "first class one-way ticket to Albuquerque", /* Weird Al */
       "yellow spandex dragon scale mail", /* X-Men */

       /* Musical Instruments */
       "grand piano",
       "two slightly sampled electric eels", /* Oldfield */
       "kick drum",                        /* 303 */
       "tooled airhorn",

       /* Pop Culture */
       "flux capacitor",                   /* BTTF */
       "Walther PPK",                      /* Bond */
       "hanging chad",                     /* US Election 2000 */
       "99 red balloons",                  /* 80s */
       "pincers of peril",                 /* Goonies */
       "ring of schwartz",                 /* Spaceballs */
       "signed copy of Diaspora",          /* Greg Egan */
       "the missing evidence in the Kelner case", /* Naked Gun */
       "blessed +9 helm of Des Lynam",     /* Bottom */

       /* compile with -DBRITHACK for British goodness */
       "bum bag",
       "blessed tin of marmite",
       "tesco value potion",
       "ringtone of drawbridge opening",
       "burberry cap",
       "potion of bitter",
       "cursed -2 bargain plane ticket to Ibiza",
       "black pudding corpse",
       /* Fantasy */
       "Necronomicon",                     /* Lovecraft */
       "pipe weed",                        /* LOTR */
       "knife missile",                    /* Iain M. Banks */
       "large gem",                        /* Valhalla */
       "monster manual",                   /* D&D */
       "spellbook called Octavo",          /* Discworld */
       "ring of power",                    /* LOTR */
       "lightsaber",
       "no tea",                           /* HGttG game */
       "pan-galactic gargle blaster",      /* HGttG */
       "silmaril",                         /* LOTR */
       "pentagram of protection",          /* Quake */

       /* Geekery */
       "AAA chipset",                      /* Amiga */
       "thoroughly used copy of Nethack for Dummies",
       "named pipe",                       /* UNIX */
       "kernel trap",
       "copy of nethack 3.4.4",            /* recursion... */
       "cursed smooth manifold",           /* Topology */
       "vi clone",
       "maximally subsentient emacs mode",
       "bongard diagram",                  /* Intelligence test */

       /* Historical */
       "dead sea scroll",
       "cat o'nine tails",
       "pieces of eight",
       "codpiece",
       "straight-jacket",
       "bayonet",
       "iron maiden",
       "oubliette",
       "pestle and mortar",
       "plowshare",

       /* Mashups */
       "potion of score doubling",
       "scroll labelled ED AWK YACC",      /* the standard scroll */
       "scroll labelled RTFM",
       "scroll labelled KLAATU BARADA NIKTO", /* Evil Dead 3 */
       "scroll of omniscience",
       "scroll of mash keyboard",
       "scroll of RNG taming",
       "scroll of fungicide",
       "helm of telemetry",
       "blue suede boots of charisma",
       "cubic zirconium",
       "amulet of instadeath",
       "amulet of bad luck",
       "amulet of refraction",
       "potion of rebigulation",           /* Simpsons */
       "O-ring",
       "wand of washing",
       "ring named Frost Band",
       "expensive exact replica of the Amulet of Yendor",
       "giant beatle",
       "lodestone",
       "rubber chicken",                   /* c corpse */
       "tin of Player meat",
       "figurine of a god",
       "tin of whoop ass",
       "cursed -3 earring of adornment",
       "wisdom boots",
       "ornamental cape",
       "acid blob skeleton",
       "brand new, all time lowest introductory rate special offer",
       "dirty rag",

	"Drinker",
	"Boozer",
	"Alcoholic",
	"Winetester",
	"Vodkaholic",
	"Scrapper",
	"Bar-Brawler",
	"Beer King",    "Beer Queen",
	"Ethanol Addict",
	"Apollo", "Dionysus", "Pan", /* Drunken */
	"Hazard Trainee",
	"Brigade Runner",
	"Watercannon User",
	"Extinguisher",
	"Forest Fire Specialist",
	"Anti-Flame Fighter",
	"Fire Leader",
	"Flash Fire Deleter",
	"Eternal Firefighter",
	"Prometheus", "Thor", "Arson", /* Fire */
	"Junior Assistant",
	"Ass Assistant",
	"Mean Assistant",
	"College Graduate",
	"University Traveler",
	"Junior Professor",
	"Experimental Scientist",
	"Lab Leader",
	"Test Row Developer",
	"Jobs", "Wozniak", "Gates", /* geek */
	"Key User",
	"Door-opener",
	"Lock Breaker",
	"Lock Trickster",
	"Safecutter",
	"Keymaster",
	"Emergency Key Carrier",
	"Door Safety Assistant",
	"Supreme Lockpicker",
	"Nuada", "Dagda", "Morrigan", /* Celtic */
	"Doodler",
	"Whistler",
	"Flute Player",
	"Harp Player",
	"Orchestrator",
	"Concertist",
	"Virtue",
	"Piano Master",
	"Godly Voice",
	"Donblas", "Grome", "Arioch", /* Melnibonean */
	"Sword Trainee",
	"Dual-wield wannabee",
	"Whiffer",
	"Bladebearer",
	"Double-edge Sword",
	"Master Blade",
	"Katana Knight",
	"Sword Acrobat",
	"Shadowblade",
	"Votishal", "Raiden", "Rat God", /* Nehwon */
	"Street Cop",
	"Traffic Officer",
	"Patrolling Cop",
	"Strifer",
	"Crime Scene Cop",
	"Junior Sheriff",
	"Deputy Sheriff",
	"Wild-West Sheriff",
	"Cop Chief",
	"Magnum", "Smith", "Wesson", /* guns */
	"Hearse Driver",
	"Disposer",
	"Grave Creator",
	"Open Casketeer",
	"Closed Casketeer",
	"Dumper",
	"Shoveler",
	"Crematorist",
	"Corpse Burner",
	"Hel", "Pluto", "Orcus", /* Death */
	"Dog Feeder",
	"Cat Tamer",
	"Wolf Feeder",
	"Lion Tamer",
	"Mastodon Feeder",
	"Dragon Tamer",
	"Tyrannosaurus Feeder",
	"Kangaroo Tamer",
	"Zouthern Petkeeper",
	"Balder", "Edda", "Hagen", /* Norse mythology */
	"Rhymer",
	"Lyrist",
	"Sonneteer",
	"Jongleur",
	"Troubadour",
	"Minstrel",
	"Lorist",
	"Well-known Bard",
	"Master Bard",
	"Garl Glittergold", "Flandal Steelskin", "Urdlen",
	"Fanfic Reader",
	"Saint Preps Academy Alumnus",       "Saint Preps Academy Alumna",
	"Badfic Writer",
	"Emo Boy", "Emo Girl",
	"Blatant Troll",
	"Motherfucker",
	"Author Avatar",
	"Self-insert",
	"Marty Stu",      "Mary Sue",
	"B'loody Mary", "Ebony Dark'ness", "Darth Valer", /* taken from a fanfic */
	"Villager",
	"Chosen for Reaping",
	"Low Tribute",
	"Tribute",
	"Career Tribute",
	"Master Hunter",
	"Hunger Games Winner",
	"Peetabane",
	"Right Hand of Cato",
	"Suzanne Collins", "Effie Trinket", "President Snow", /* hunger games */
	"Cantrip Caster",
	"Stunted Magician",
	"Vapor Mage",
	"Ghost Mage",
	"Mage Dominion",
	"Mage Empress",
	"Spellbane",
	"Armored Mage",
	"Archmage",
	"Eilistraee", "Kiaransali", "Lolth", /* Elven */
	"Treehut Dweller",
	"Flora Watcher",
	"Timberlander",
	"Leafcloth Wearer",
	"Woodrunner",
	"Jungle Hunter",
	"Guerilla Bowman",
	"Bow Sniper",
	"Master Hunter",
	"Eddergud", "Vhaeraun", "the black web", /* Elven */
	"Radical Elf",
	"one who doesn't like foreigners",
	"S'wit Remover",
	"N'wah Basher",
	"causer of suffering",
	"Merciless Assassin",
	"Morag Tong member",
	"Morag Tong leader",
	"Morag Tong Master Assassin",
	"Orome", "Yavanna", "Tulkas", /* Elven */
	"Battler",
	"Duelist",
	"Bladefencer",
	"Battlemaster",
	"Overlord",
	"War Machine",
	"Three-handed Swordsman",
	"Walking Weapon Factory",
	"Fucking Hammer of Thor",
	"Vaire", "Varda Elentari", "Nessa", /* Elven */
	"Concordate Agent",
	"Whitegold Ambassador",
	"Heretic Hunter",
	"enemy of Talos",
	"Hunter of Blades",
	"Elven Secret Agent",
	"Aldmeri Dominion",
	"Elf at War",
	"Summerset Dictator",
	"Manwe Sulimo", "Mandos", "Lorien", /* Elven */
	"Whipper",
	"Lasher",
	"Sadist",
	"Kinky Fucker",
	"Punisher",
	"Riding Crop Lover",
	"BDSM Fetishist",
	"Painmaker",
	"Grey wannabe",
	"Jacob Black", "Bella Swan", "Edward Cullen",	/* Twilight */
	"Little Wannabe Fighter",
	"Saiyan Amateur",
	"Saiyan Apprentice",
	"Anime Hero",
	"Monkey Boy",
	"Z-Warrior",
	"Saiyan Prince",
	"Saiyan General",
	"Saiyan Grandmaster",
	"Moori", "King Kai", "Vegeta", /* Dragonball series */
	"Voter",
	"Councillor",
	"District Councillor",
	"Backbencher",
	"Minister",
	"Secretary of State",
	"Chancellor",
	"Home Secretary",
	"Prime Minister",
	"Democracy", "Communism", "Despotism", /* political structures */
	"one who forgot the salt",
	"pepper-choker",
	"second-class waiter",       "second-class waitress",
	"waiter",    "waitress",
	"kitchen chef",
	"star cook",
	"five-star cook",
	"national championship cook",
	"world-championship cook",
	"McDonalds", "Kentucky's Fried Chicken", "Burger King", /* fast food places */
	"College Student",
	"Deviant Seer",
	"Fortune Teller",
	"Seer Graduate",
	"Visioneer",
	"Wise Seer",
	"Void Diviner",
	"Psychic Visor",
	"Hidden Depth Explorer",
	"Dunlain", "Savos Aren", "Hert the Vampire", /* Skyrim */
	"Invoker",
	"Summoner",
	"Secret Keeper",
	"Mage Adept",
	"Witchcrafter",
	"Illusionist",
	"Geomancer",
	"Astral Caster",
	"Master Of All Spells",
	"Cassandra", "Menelaos", "Helen of Troy", /* Greek history */
	"Intellectual",
	"Bestseller Reader",
	"Bookkeeper",
	"Bookworm",
	"Beta Reader",
	"Keeper of Rare Books",
	"Keeper of Valuable Books",
	"Magna Charta Keeper",
	"Keeper of Unique Books",
	"Leo Tolstoi", "Stephen Hawking", "H. P. Lovecraft", /* famous writers */
	"Pistol User",
	"Revolver User",
	"SMG User",
	"Shotgun User",
	"Assault Rifle User",
	"Machine-Gunner",
	"Heavy Machine-Gunner",
	"Rocketeer",
	"BFG 9000 Master",
	"Heyaduo",
	"Zhozshi",
	"Mr. Yu",         "Ms. Yu",
	"Songye",
	"Zhidao",
	"Juzi",
	"Wintry, Poplar and Willow",
	"Yifuji",
	"Du",
	"Brahma", "Vishnu", "Shiva", /* India? */
	"Paint Splotcher",
	"Brushswinger",
	"Color Cribber",
	"Painter",
	"Picture Designer",
	"Successful Artist",
	"Brush Virtue",
	"Canvas Enchanter",
	"Creator of Unique Paintings",
	"Leonardo", "Picasso", "Dali", /* famous artists */
	"Tetris Player",
	"Boulderdash Player",
	"Super Mario Addict",
	"FIFA Sports Gamer",
	"Need For Speed Racer",
	"owner of a dozen consoles",
	"Wii Champion",
	"RROD Defeater",
	"Playstation Master",
	"Nintendo", "Microsoft", "Sony", /* gaming console producers */
	"Puppet Trainer",
	"Yin Seal Thrower",
	"Spell Capturer",
	"Magic Puppeteer",
	"Death Forest Survivor",
	"Mount Fucking Moon Hiker",
	"Gates of Hell Visitor",
	"Puppet League Participant",
	"Puppet League Champion",
	"Sakuya", "Reimu", "Yukari Yakumo", /* Touhou */
	"Hopper",
	"Jumper",
	"Stunter",
	"Flyer",
	"Airdasher",
	"Warpjumper",
	"Implacable Protagonist",
	"Main Attraction",
	"Movie Star",
	"James Bond", "Chuck Norris", "Jackie Chan", /* movie heroes */
	"Recruit",
	"Octopus",
	"Raven",
	"Mantis",
	"Wolf",
	"Fox",
	"Foxhound",
	"Big Boss",
	"Solid Snake",
	"Colonel Campbell", "Deepthroat", "The Patriots", /* probably some TV show or movie? */
	"Red Robe",
	"Reader",
	"Scribbler",
	"Writer",
	"Permanent Magic Marker",
	"Portable Ink Container",
	"Scientific Scribe",
	"Well-trained Scribe",
	"Highest Order Scribe",
	"Bowditch", "Peabody", "Rothchild", /* Fallout 3 */
	"Fighter of Law",
	"Chivalrous Warrior",
	"Knightly Strider",
	"Courageous Battler",
	"Holy Warrior",
	"Celestial Warrior",
	"Peacebringing Knight",
	"Knight in shiny armor",
	"God-gifted Warrior",
	"Ariel", "Tyrael", "Gabriel",	/* angels */
	"Zapper",
	"Wand Carrier",
	"Magic Shooter",
	"Zapbolter",
	"Recharger",
	"Charge Master",
	"Wandmaker",
	"Senior Wandmaker",
	"Wishing Wand Creator",
	"Larry Koopa", "Roy Koopa", "Morton Koopa Jr.", /* Super Mario Bros */
	"Dark Brotherhood Initiate",
	"Whetblade",
	"Killer",
	"Contract Killer",
	"Silent Killer",
	"Devious Cutthroat",
	"Knife After Dark",
	"Deadly Blade",
	"Merciless Murderer",
	"Ehud", "Ford", "Ivins", /* unknown origin */
	"Money Addict",
	"Small-time Bully",
	"Little Mobber",
	"Clique Bully",
	"Cape Stealer",
	"Leather-clad Thief",
	"Mean Mobbing Bully",
	"Violent Bully",
	"Weapon Toting Bully",
	"Everella Shrine", "Butch DeLoria", "Draco Malfoy",	/* popular bullies */
	"Low Worker",
	"Shift Worker",
	"Accord Worker",
	"Bored Cashier",
	"Shoplifter Detector",
	"Detective disguised as Cashier",
	"Gun-toting Shopkeeper",
	"Thief's Bane",
	"Safemaster",
	"the Homies", "the Robbers", "the Motherfuckers", /* taken from a fanfic */
	"Slave Trader",
	"People Seller",
	"Human Abducter",
	"Professional Slaver",
	"Kidnapper",
	"Poocher",
	"Whipmaster",
	"Master Slaver",
	"King of Slavers",      "Queen of Slavers",
	"Airyaman", "Gandarewa", "Daevas", /* Persian */
	"Insert rank title here",
	"Insert clever name here",
	"Insert witty saying here",
	"Insert random string here",
	"Insert cool stuff here",
	"Insert what you want here",
	"Insert appellation here",
	"Insert heroic deeds here",
	"Insert mysterious message here",
	"Battlefield Newbie",
	"Training Fighter",
	"Weaponbearer",
	"Battle Veteran",
	"Honored Fighter",
	"Cruel Warlord",
	"Bonecrusher",
	"Spinebreaker",
	"Decapitator",
	"Rhea Oro", "Liebea Luna", "Elenya Pure", /* taken from a fanfic */
	"Wiseguy",
	"Mirror Looker",
	"Self-admirer",
	"Beauty Contest Competitor",
	"Women's Hero",
	"Chick Magnet",
	"Celebrity",
	"Male Topmodel",
	"Sexiest Man Alive",
	"Hugh Hefner", "G-boy", "Arsene Lupin", /* famous people, I guess? */
	"Little Fool",
	"Juggler",
	"Court Clown",
	"Funny Dude",     "Funny Gal",
	"Laughing Muscle",
	"Lollerskater",
	"Roflcopter",
	"Joker",
	"King's Personal Jester",
	"Barnum", "Bailey", "Shaco", /* ??? */
	"Cash Stealer",
	"Money Thief",
	"Tricky Hands",
	"Trained Fingers",
	"Trickster Thief",
	"Mean Stealer",
	"Expert Thief",
	"Moneybag Remover",
	"Money Van Kidnapper",
	"Danzai", "Milanor", "Daini", /* ??? */
	"Schoolkid",
	"Teenie",
	"Preparing for Exams",
	"High School Graduate",
	"Final Year Student",
	"University Newbie",
	"Bachelor of Art and Craft",
	"Jesuitic Studies PhD",
	"Marriage Counseling Professor",
	"Queen Serenity", "Angel Aphrodite", "Queen Beryl", /* perhaps an anime or manga? */
	"Looser",
	"Failure",
	"Bum",
	"Torn Person",
	"Chainsmoker",
	"Walking Beer Bottle",
	"Limping Tramp",
	"Luckless Individual",
	"Choosy Beggar",
	"Knife Killer",
	"Dangerous Criminal",
	"Mean Contract",
	"Serial Killer",
	"Cold-Blooded Gangster",
	"Silent Assassin",
	"Master Hitman",     "Master Hitwoman",
	"Professional Cutter",
	"Bane of all innocent people",
	"Falis", "Marfa", "Falaris",	/* Record of Lodoss War */

	"Aimbot User",
	"Wallhack Exploiter",
	"Dirty Camper",
	"Asshole Player",
	"Kickban Bait",
	"MMORPG Troll",
	"Server Hacker",
	"Godmode Cheater",
	"Instawin Scriptkiddy",
	"the Bitches", "the Hell Brides", "the Sluts with Syphilis", /* taken from a fanfic --Amy */

	"Globule",
	"Moist",
	"Wiggly",
	"Jiggly",
	"Slurry",
	"Slime Knight",
	"Slime Hero",
	"King Slime",     "Queen Slime",
	"Slime God",       "Slime Goddess",
	"King Somnus", "Rubiss", "Malroth", /* Dragon Warrior */

	"Young Boy",     "Young Girl",
	"Village Hero",
	"City Hero",
	"Island Hero",
	"Country Hero",
	"Continent Hero",
	"World Hero",
	"Super Hero",
	"Legend",

	"Mining Slave",
	"Mine Worker",
	"Pick Wielder",
	"Stonecutter",
	"Shaft Climber",
	"Gold Digger",
	"Gemstone Hoarder",
	"Retired Goldrusher",
	"Made Man",
	"Aurum", "Argentum", "Antimony",

	"Little Midget",
	"Small Person",
	"Middle-Earth Miner",
	"Cave Inhabitant",
	"Big Midget",
	"Giant Midget",
	"Dwarven Master Smith",
	"Dwarvish Legend",
	"Biggest Midget Ever",
	"Moradin", "Dumathoin", "Abbathor",

	"Hobbit Townie",
	"Drunken Hobbit",
	"Celebrating Hobbit",
	"Ringquester",
	"Fellow of the Ring",
	"Moria Survivor",
	"Blarog Basher",
	"Mount Doom Hiker",
	"Hero who defeated Sauron",
	"Lurtz", "Saruman", "Morgoth",

	"Polymorph Trainee",
	"Item Zapper",
	"Polymorphitis Hero",
	"Wild Changer",
	"Polypiler",
	"Human Chameleon",
	"Master Shapechanger",
	"Grandmaster Shapechanger",
	"Ultimate Shapechanger",

	"Twedhel",       "Twelleth",
	"Twedhel",       "Twelleth",      /* elf-maid */
	"Twohtar",       "Twohtie",       /* warrior */
	"Twekano",			/* commander (Q.) ['a] */
			"Twekanie",	/* educated guess, until further research- SAC */
	"Twarandur",			/* king's servant, minister (Q.) - guess */
			"Twaranduriel",	/* educated guess */
	"Twehir",         "Twehiril",       /* lord, lady (S.) ['ir] */
	"Twaredhel",     "Twarwen",       /* noble elf, maiden (S.) */
	"Twernil",       "Twelentariel",  /* prince (S.), elf-maiden (Q.) */
	"Twelentar",     "Twelentari",	/* Star-king, -queen (Q.) */
	"Legolas", "Eowyn", "Eol", /* Elven */

	"Survivor",
	"Recruit",
	"Insurgent",
	"Trooper",
	"Rebel Leader",
	"Chrono Commandant",
	"Anachrononononaut",
	"Fatebreaker",
	"Hope of the Future",

	"Fire Tower Caster",
	"Rock Melter",
	"Eruption Magician",
	"Windchiller",
	"Tornado Chief",
	"Volcano Prince",
	"Causer of Hurricanes",
	"Armageddon Mage",
	"Thundering Earthshaker",
	"Tal Rascha", "Blood Raven", "B-a-a-l", /* Diablo 2 */

	"octo", "enraged diggle", "arch diggle", "garg", "poison slug", "dopefish", "Borg drone",

	"Mars Marine",
	"Demon Blaster",
	"Gun-Toting Avenger",
	"Kalashnikov Gunner",
	"BFG Wielder",
	"Spiderdemon Hunter",
	"Nether Realm Traveler",
	"Hell Gate Closer",
	"Cyberdemon's Bane",
	"Zombieman Brigade", "The Revenants", "Cyberdemon MkIII Deluxe", /* Doom 2 */

	"Alcor",
	"Dubhe",
	"Merak",
	"Phecda",
	"Megrez",
	"Alioth",
	"Mizar",
	"Benetnash",
	"Polaris",
	"Sol", "Terra", "Luna",

	"Mercury",
	"Venus",
	"Mars",
	"Jupiter",
	"Saturn",
	"Uranus",
	"Neptune",
	"Pluto",
	"Moon",
	"Selene", "Helios", "Eos",

	"Kageman",
	"Sonicblastman",
	"Mustleman",
	"Wingman",
	"Eightman",
	"Gatyaman",
	"Rainbowman",
	"Tekkaman",
	"Ultimate Justice Keeper",
	"Lawful Good", "True Neutral", "Chaotic Evil",	/* Dungeons and Dragons */

	"Pure Boy",		"Pure Girl",
	"Jinme Boy",		"Jinme Girl",
	"Mist Youth",		"Mist Maiden",
	"Moon Youth",		"Moon Maiden",
	"Prince Youth",	"Princess Maiden",
	"Hanuman Youth",	"Hanuman Maiden",
	"Ryujin Youth",	"Ryujin Maiden",
	"Kamiko",
	"Eternal Youth",	"Eternal Maiden",
	"Umbrellarina Madeline", "Medium Deborah", "Crueltrainer Cindy",

	"Arrow Pierced",
	"Possessed",
	"Conjurer",
	"Magician",
	"Stand user",
	"Journeyer",
	"Voyager",
	"Explorer",
	"Adventurer",
	"Jonathan Joestar", "Hirohiko Araki", "Cars", /* Jojo */

	"Hiyokko",
	"Flutty",
	"Miso Souper",
	"Fitness Strider",
	"Black Mate",
	"Knee Supporter",
	"Beard Mantlet",
	"Global Pertner",
	"Sexy King",	"Sexy Queen",
	"Yoroshiku", "Ma Shin", "Sexy Sai", /* Chinese */

	"Poison Mixer",
	"Venom Scatterer",
	"Green Poisoner",
	"Purple Poisoner",
	"Poison Exploder",
	"Contaminator",
	"Stinking Cloud Creator",
	"Gas Attacker",
	"Poison Bomber",
	"Seveso Toxin", "Fukoshima Radiation", "Mustard Gas", /* poison */

	"Heel Wearer",
	"Stiletto Lover",
	"Khmer-Speaking Transvestite",
	"Dragshow Attendant",
	"Sexy Malemodel",
	"Hippie Rocker",
	"High-Heel-Loving Guy",
	"Androgynous Beauty",
	"Fleecy Sex Symbol",

	"Rookie",
	"Soldier",
	"Mercenary",
	"Veteran",
	"Champion",
	"Hero",
	"Baron",
	"Duke",
	"Lord",

	/* more random names... */

	"Tacitusrian", "Tacitus-rian", "Ryuan", "Ryu-an", "Robat", "Robert", "Tobi", "Tobias", "Flo-ooo", "Florian", "Cristi", "Christian", "Alex", "Johanetta", "Julia", "Julchen", "Yvonne", "Magdalena", "Eveline", "Bea", "Beatriz", "Egas", "Hannes", "Leo", "Leopold", "Baschdi Deathstrike", "Markus", "Martin", "Corina", "Elif", "Nadja", "Sunali", "Solvejg", "Thai", "Max", "Maximilian", "Jannik", "Conse", "Constantin", "Paul", "David", "Meltem", "Susanne", "Rita", "Kati", "Katinka", "Mailie", "Marie", "Klara", "Sandra", "Arne", "Antonia", "Chaska", "Ludgera", "Laura", "Eva", "Maurah", "Sophie", "Marian", "Jil", "Patricia", "Kerstin", "Hanh", "Antje", "Jennifer", "Karin", "Nicole", "Bettina", "Heike", "Dora", "Maite", "Ruth", "Verena", "Lou", "Julian", "Danielle", "Sebastian", "Amandine", "Lily", "Yannick", "Leonie", "Mira", "Felix", "Sofia", "Christiane", "Ann Kathrin", "Njusha", "Elisabeth", "Conny", "Constanze", "Lisa", "Anja", "Yasaman", "Almut", "Ksenia", "Miriam", "Elena", "Katharina", "Helen", "Victoria", "Simone", "Nataliya", "Kristin", "Nelly", "Rejda", "Nora", "Jasieen", "Yacine", "Marike", "Michael", "Merle", "Marianne", "Sina", "Dorothea", "Tinanya", "Hanno", "Noemi", "Giulia", "Nino", "Charlotte", "Friederike", "Sophia", "Sue Lyn", "Juen", "Ruea", "Gudrun", "Ella", "Manuela", "Daniel", "Lennart", "Tilla", "Ilja", "Greta", "Jane", "Nico", "Tillmann", 

	"Celia", "Boese", "Bad", "Eliane", "O'Neill", "Stefan", "Fenja", "Silvana", "Vanasil", "Lukas", "Selinger", "Gallardo", "Sarina", "Alexia", "Vida", "Isis", "Ilse", "Melanie", "Baenni", "Peeer", "Peeta", "Lareena", "Janina", "Jannie", "Walter", "Klaus", "Micha", "Chirin", "Ingrid", "Walker", "Nikolei", "Jonas", "Iwan", "Tonja", "Tapia", "Rubinho", "Coffin Nail", "Evillan", "Thilo", "Maurus", "Ligasa", "Andrea", "Mia", "Annemarie", "Caro",

	"Mandarina", "Ariane", "Carina", "Denise", "Freddie", "Kira", "Nadine", "Laurens", "Loorenz", "Jorin", "Rinjo", "Franzi", "Amelie", "Andrej", "Annika", "Anselm", "Aram", "Barbara", "Boris", "Burkhard", "Elsa", "Isabel", "Nils", "Siln", "Ozan", "Otzan", "Thorwald", "Forestgate", "Dominik", "Albert", "Bianca", "Carmen", "Don Finwe", "Gerrit", "Hilda", "Jens", "Johanna", "Julietta", "Leon", "Linda", "Marius", "Mirko", "Petra", "Sigurd", "Sonja", "Stella", "Ismella", "Teresa", "Wilhelm", "Sagarah", "Rosy",

"Borg",
"Dalek",
"Destructor",
"Predator",
"Wizard of Yendor",
"Smurf",
"Cosmic Horror",
"Ultimate Unspeakable Lovecraftian Nightmare",
"Space Invader",
"Transformer",
"Master of the Universe",
"He-Man",
"Pizza Worm",
"Bogeyman",
"Care Bear",
"My Little Pony",
"IRA Terrorist",
"Nazi",
"Klingon",
"Cardassian",
"Snark",
"Luggage",
"Killer penguin",
"Were-penguin",
"Giant were-penguin",
"Peeping-Tom",
"Jester",
"Battlemech",
"Invid",
"Amazon",
"Teenage Mutant Ninja Turtle",
"Stork",
"Hippo",
"Roadrunner",
"Fooglebird",
"Ghoti",
"Whale",
"Apocalyptic Beast",
"Anti-Christ",
"Dark Avenger",
"Evil Computer",
"Finnish Sprayer",
"Natas",
"Poodle",
"Fire Hydrant",
"Multi-hued elephant",
"Martian",
"Purple alligator",
"Space ship",
"Miniature space fleet",
"6-feet tall elephant",
"Insurance salesman",
"Benji",
"Magnet",
"Squid",
"Typewriter",
"Manitou",
"Miniature Shetland pony",
"Munchkin",
"Mad Scientist",
"Keystone Kop",
"Magnificent big-game hunter",
"Wicked witch",
"B-1 Bomber",
"Superman",
"Batman",
"Boy Wonder",
"Incredible Hulk",
"Amazing Spider-Man",
"Orphan",
"Fifth columnist",
"Secret agent",
"Spy",
"Dog catcher",
"Vacuum cleaner",
"Lawnmower Man",
"Bloodletter of Khorne",
"Buzzard",
"Oyster",
"Wombat",
"Midget elephant",
"Chameleon",
"Dungeon Master",
"Dungeon Keeper",
"Beyonder",
"Robocop",
"Robocod",
"Paradroid",
"Dominatrix",
"Paranoid",
"Wagtail",
"Great tit",
"Pope",
"Tribble",
"Jedi knight",
"Dark Jedi",
"Master of Teras Kasi",
"Vulcan",
"Rancor",
"Sarlacc",
"Warbot",
"Tailgunner",
"Skinhead",
"Paul Bunyan Machine",
"Bungee",
"Sumo wrestler",
"Gargantuan sumo wrestler",
"Karateka",
"President",
"King",
"Lobo",
"Trashman",
"Teacher",
"Cheshire Cat",
"Mad Hatter",
"March Hare",
"Voodoo doll",
"Rag doll",
"Scarecrow",
"Porcupine",
"Stone idol",
"Hacker",
"Samurai",
"Samurai Cat",
"Phantom of the Opera",
"X-man",
"Wumpus",
"Walrus",
"Silver Surfer",
"Chimpanzee",
"Omnipotent being",
"Reaperbot",
"G. I. Joe",
"Cobra Commander",
"Dreadnok",
"Autobot",
"Battlemage",
"Saboteur",
"Master of Sinanju",
"Cyborg",
"Lemming",
"Green Goblin",
"Galactus",
"Gooey Kablooie",
"Prowler",
"Iron Fist",
"Stuka",
"Mahdi",
"Xeroc",
"Catwoman",
"Stupendous Man",
"Lost boy",
"Hunter-killer",
"Mad butcher",
"Vore",
"Rotfish",
"Heresiarch",
"False prophet",
"Emperor",
"Tumtum",
"Androgyne",
"Thieving magpie",
"Boast of England",
"Red Rose Knight",
"Unholy cow",
"Q",
"Red Baron",
"Giddy goon",
"Vogon",
"Lizard King",
"Hutt",
"Puppet master",
"Wookie",
"Giant sandworm",
"Muad' Dib",
"Ent",
"Huorn",
"Werepotato",
"Mindless one",
"Alien queen",
"Alligator man",
"Ancient ape",
"Ancient astronaut",
"Astro-zombie",
"Beast from 20000 fathoms",
"Beast with a million eyes",
"Black goat of the woods with a thousand young",
"Behemoth",
"Leviathan",
"Big Bug",
"Blob",
"Khyberdemon",
"Body snatcher",
"Brain from Planet Ardus",
"Scatologist",
"Brain of Venus",
"Brain that would not die",
"Breath blaster",
"Chickenstein's monster",
"Creature from the Black Lagoon",
"Loch Ness monster",
"Creeping death",
"Creeping unknown",
"Fifty-foot woman",
"Flumsh",
"4D-man",
"Bride of Frankenstein",
"Galaxy being",
"Giant baby",
"Giant chicken",
"Krazy Kat",
"Giant celery stalk",
"Gorilla witch",
"Great god Porno",
"H-dial monster",
"Heap",
"Love lotus",
"Herman munster",
"Highest intelligence",
"H-man",
"Hound of the Baskervilles",
"Wizball",
"ID monster",
"Mancubus",
"Arch-vile",
"Pain elemental",
"I-ball",
"Killdozer",
"Killer tomato",
"Lurking fear",
"Madball",
"M.U.L.E.",
"Magnetic monster",
"Mixed-up zombie",
"Hunchback of Notre Dame",
"Monster that challenged the World",
"Monster from green hell",
"Morlock",
"Queen Kong",
"Robot Kong",
"Mecha-Godzilla",
"Ghidhrah",
"Saucerman",
"Ethereal",
"Space Brain",
"Floating brain of Hitler",
"Stay Puft Marshmallow Man",
"Supercow",
"Swamp Thing",
"Thing",
"Teenage Frankenstein",
"Teenage Werewolf",
"Terror from the year 5000",
"Triffid",
"Tripod",
"Xenophobic man",
"Undying monster",
"Unknown terror",
"Unnamable",
"Womaneater",
"E.T.",
"Toxic Avenger",
"Onslaught",
"Pacman",
"Paradroid",
"Bantha",
"Ass-headed fish",
"Wizard of Oz",
"Juggernaut",
"Cheshire Cat",
"Tauntaun",
"Heretic",
"Other",
"Uncanny",
"Libido",
"Super-ego",
"Boy Blunder",
"Big nothing",
"Parademon",
"Zelda",
"Super Mario",
"Great Giana Sister",
"Magnetic man",
"Pink elephant",
"Elephant man",
"Great Unknown",
"Lurking unknown",
"Cenobite",
"Cheerleader",
"Model T-1000 Terminator",
"Whippoorwill",
"Moon Maniac",
"Killer Clown",
"Stocking Strangler",
"Torture Doctor",
"Beast of the Black Forest",
"Demon of the Belfry",
"Sex Beast",
"Sunday Morning Slasher",
"Kerio the Ill-Tempered Swearing Italian",
"Ego",
"Id",
"Computer god",
"Evil woman",
"Lord of this world",
"Sadist",
"Masochist",
"Sado-masochist",
"War pig",
"Iron Man",
"Dehumanizer",
"Holy diver",
"Jugulator",
"Exciter",
"Painkiller",
"angry mob",
"Who",
"Y2K bug",
"RNG",
"Greater hell magic mushroom cyber-unmaker leprechaun of the Dawn",
"Devastator, the Destroyer's Big Brother",
"Celestial",
"Valkyrie",
"Diablo",
"Eidolon",
"Medic",
"Tank",
"Tank Commander",
"Man-in-Black",
"Spawn",
"Neon knight",
"Defender of the Faith",
"Eternal idol",
"little Morgoth",
"Ur-Vile",
"Moose",
"Osama bin Laden",
"Agent",
"Morpheus",
"Hydralisk",
"Infested Terran",
"Asha'man",
"Aes Sedai",
"little lamb",
"Tea-kettle",
"Vorlon",
"Giant flea",
"Tractor trailer",
"Zergling",
"Dark Templar",
"Dragon Reborn",
"George Bush",
"Saddam Hussein",
"Deodorant",
"Minion of DarkGod, the Mighty Coder of Hell",
"Evil Genius",
"Satan",
"Sea Folk Windfinder",
"Seanchan",
"Eminem",
"Madonna",
"Dementor",
"Typewriter",
"Great Wyrm of Nothing",
"Great Wyrm of Toxic Waste",
"Damane",
"Man in Black",
"Matrix agent",
"Harry Potter",
"Lawnmower",
"Killer Banana",
"Fear Mold",
"Oracle",
"Trinity",
"Dumbledore",
"Voldemort",
"Maia",
"Vala",
"Elf",
"Human",
"Gnome",
"Half-Troll",
"Skywalker cronie",
"Sith Lord",
"Existentialist philosopher",
"Midichlorian",
"Scientologist",
"Jehovah's witness",
"Umbarite",
"Druj",
"Yogi",
"Prime Minister",
"Dragkhar",
"Stone Dog",
"Maiden of the Spear",
"t-o-m-e.net forum poster",
"Messenger of Eru",
"Grohlm",
"Aiel Wise One",
"Alley cat",
"Keanuholic",
"Shadowkiller",
"Robocop",
"Alien",
"Amulet of Yendor",
"Christian",
"Muslim",
"Extremist",
"Forsaken",
"Carnivore",
"Xenu",
"King of Arnor",
"Rider of Rohan",
"Draco Malfoy",
"Cheeky bastard",
"Ghost of Frodo Baggins",
"Evil dead",
"Death incarnate",
"Rhinoceros",
"Ghost of your past",
"Biology professor",
"Frankenstein",
"Breather",
"Caligula",
"Man of Haleth",
"Budweiser frog",
"Terminator",
"T-800",
"Purple space chicken",
"Simpsons character",
"UN weapons inspector",
"Noldorin mercenary",
"Egomaniac",
"Russian mafia",
"Godfather",
"UFO",
"Superman",
"Evil wizard",
"Streetcar",
"Karaoke machine",
"Yakuza",
"Wolverine",
"Lovecraftian nightmare",
"Shakespeare's imitator",
"King Kong",
"Protoss probe",
"Shai'tan",
"Baba Yaga",
"Saucepan",
"Marshmallow Man",
"Cellular automaton",
"Strawman",
"Logic gate",
"Xenophobe",
"Unspeakable horror",
"Spirit of Roger Wilco",
"Long sword 'Ringil'",
"Zoolander",
"Intel(R) Pentium processor",
"Pac-Man",
"Gangster",
"Waterboy",
"Long sword 'Mormegil'",
"Battlecruiser",
"Ultralisk",
"Believer",
"Loser",
"Moron",
"Imbecile",
"Nebuchadnezzar",
"Tomb raider",
"Hammer of Thor",
"Wrath of the gods",
"Betrayer of Turin",
"Traitor of Gondolin",
"Terminatrix",
"Dragonlance",
"Hillside strangler",
"Texas chainsaw",
"Pampers commercial",
"Bus driver",
	"Chaotic Dweller",
	"Enemy of Order",
	"Madman",         "Madwoman",
	"Vicious Diceroller",
	"Russian Roulette Player",
	"Spell Randomizer",
	"Effect Warper",
	"Cool Calculator",
	"Master of Entropy",
	"Arle Nadja", "Celine Jules", "Miki Onimaru", /* anime */
	"Fire Fingers",
	"Icicle Hands",
	"Air Current Lover",
	"Earth Digger",
	"Inferno Strider",
	"Hurricane Center",
	"Glacial Avalanche",
	"Migmatite Carver",
	"Astral Controller",
	"Amon Ra", "Hersifon", "Mor Havoc", /* Age of Empires */
	"Risky Apprentice",
	"Dark Magician",
	"Venture Caster",
	"Tome Mage",
	"Arcane Arts Student",
	"Promising Scholar",
	"Controller of Dark Arts",
	"Occult Library Owner",
	"Backlash Master",
	"Fate", "Unlimited Blade Works", "Heaven's Feel", /* suggestion on gamefaqs IIRC? */
	"Poschengband Noob",
	"Camelot Explorer",
	"Ambush Avoider",
	"Morivant Merchant",
	"Angband Diver",
	"Ocean Traveler",
	"666 Delver",
	"555 Survivor",
	"One Who Killed The Serpent",

	"Cheap Sex Slave",
	"Kidnapped Beauty",
	"Sex Market Commodity",
	"Spread Legs",
	"STD Lover",
	"Many-Times-Fucked",
	"Orgasm Bomber",
	"Men's Wet Dream",
	"Highest-Selling Wench",
	"Ewa", "Lorskel", "Kaczynski", /* Polish names */
	"Light Girl",
	"Attractive Hooker",
	"Sexy Butt Cheeks",
	"Money Body",
	"Noble Prostitute",
	"Beautiful Harlot",
	"Sexbomb",
	"Top-Class Working Girl",
	"King's Secret Love",
	"Mercedes", "Misty", "Kendl", /* Grand Theft Auto */

	"Kumiromi of Harvest", "Opatos of Earth", "Ehekatl of Luck",

	"Rebel Woman",
	"Wannabe Maneater",
	"Fingernail Scratcher",
	"Purple Dress Wearer",
	"Well-known SJW",
	"Safe Zone Defender",
	"Nutsmasher",
	"Rolling Pin Swinger",
	"Bane of Men Worldwide",
	"Balanced Doppelganger",
	"Fluctuated Controller",
	"Restricted Polymorpher",
	"Armor Preserver",
	"Shuddering Vibrator",
	"Skillful Shifter",
	"Master of Transmogrifying",
	"Polyself Hero",
	"Reality Creator",
	"Polypiling", "Monster Polymorphing", "Self-Polymorph",
	"Bullying Victim",
	"Mobfer",
	"Pupil Without Pocket Money",
	"Bang Gang's Punching Bag",
	"Hussy Club Doormat",
	"Teacher Friend",
	"Victim Turned Executioner",
	"Headhunter Spokesman",    "Headhunter Spokeswoman",
	"One Who Will Defeat Anna",
	"Mr. Maradonna", "Ms. Unfortunate Forest", "Ms. Storm",
	"Safety Officer",
	"Burning Stick Controller",
	"Uranium Handler",
	"Walking Geiger Counter",
	"Atomic Counselor",
	"Ordinance Observer",
	"HEV Suit Wearer",
	"Resonance Cascade Averter",
	"Gordon Freeman Clone",
	"Gina", "Gordon Freeman", "G-Man", /* Half-Life */
	"Cursed Beginner",
	"RNG Whim",
	"Would-Be Creator",
	"Fated Grumbler",
	"Bad Wielder",
	"Sent Manipulator",
	"Evasive Switcher",
	"Random Ascension Kit Wearer",
	"Nastiness Survivor",
	"Hexorcist",
	"Anachrononono-nononono",
	"Vestige Vanquisher",
	"Spirit Killer",
	"Mind Player",
	"Psionaut",
	"Void Voider",
	"Telekinetic",
	"Keeper of the Gate",
	"Tariru", "FIQ", "Demo",	/* dnethack players */
	"Annoying Guy",    "Annoying Girl",
	"Nasty Fucker",    "Nasty Bitch",
	"Teaser Displeaser",
	"Perfume Bomb",
	"Stinking Socks Wearer",
	"Drum Stint Reluctant",
	"Odorous Uncle",    "Odorous Aunt",
	"Clicker Money Thief",
	"Anna's Personal Bodyguard",
	"Madeleine", "Sofia", "Wendy", /* hussy names from pager.c */
	"Bricklayer",
	"Wall Construction Worker",
	"Granite Cementer",
	"Pavement Optimizer",
	"Housebuilding Apprentice",
	"Slate Slabber",
	"Clay Smelter",
	"Minecraft Master",
	"World Trade Center Architect",
	"The Battlehorse", "The Wild Boar", "The Dire Wolf", /* big mean animals */
	"Isolated Modder",
	"Autistic Programmer",
	"Whiny Crybaby",
	"Balance Analphabet",
	"Dumplog Scanner",
	"Ragefitter",
	"Accusing Lux",
	"Repository Deleter",
	"Self-Appointed Coding God",
	"Your Magical Roommate", "Your Annoying Sister", "Your Insufferable Mother-In-Law", /* special */
	"Assumption Spreader",
	"Big Lips",
	"Loud Agitator",
	"Tally Heretic",
	"Mass Rallier",
	"Mob Mobilizer",
	"Politic Revolutionizer",
	"State Usurper",
	"King of Fake News",
	"Heimskr", "Suffragette", "Cromwell", /* annoying NPCs in certain video games */
	"Colorfucker",
	"Realism Lover",
	"Dark Day Worshipper",
	"Opinion Decrier",
	"Creative Name Inventor",
	"Good Godparent",
	"Bullying Enabler",
	"Cataclysm Squatter",
	"Meme Machine",
	"Margaret", "Anaconda", "Aerschie-Miesie", /* Amy's nicknames in Half-Life, AHL and Counter-Strike */
	"Safe Zone Kid",
	"Low Gatekeeper",
	"Cesspool Dweller",
	"Intolerant Fanatic",
	"Echo Chamber Sounder",
	"Banhammer Wielder",
	"Party Line Ensurer",
	"Miss-Leader",
	"Major Minoritist",
	"Zoe Quinn", "Brianna Wu", "Anita Sarkeesian", /* feminists */
	"Alpha Mission Team",
	"Sector Beta Sweeper",
	"Order Acceptor",
	"Diamond Waller",
	"Distress Disposer",
	"Teacher's Favorite",
	"Treebark Protector",
	"Superior Hider",
	"Bane of Robbers",
	"Speaker Walt", "Great Corner-Hard", "Left Guenter", /* superschool people */

	"Summer Intern",
	"Q/A Tester",
	"Web Designer",
	"Help Desk Jockey",
	"Junior Programmer",
	"Sysadmin",
	"Lead Programmer",
	"VP Engineering",
	"High Programmer",
	"Linus Torvalds", "Bjarne Stroustrup", "Mark Zuckerberg", /* famous software engineers */
	"Savescummer",
	"File Sharer",
	"W@r3z d00d",
	"Script Kiddie",
	"H@x0r",
	"1337 H@x0r",
	"Decker",
	"Virusneaker",
	"Phreaker",
	"Wikileaks", "Guccifer 2.0", "Anonymous", /* h@cking */
	"Toilet Scrubber",
	"Mop Boy",
	"Housekeeper",
	"Custodian",
	"Maintenance Man",
	"Sanitation Freak",
	"Superintendent",
	"Property Manager",
	"Landlord",
	"Grime", "Dog Shit", "Fag Butt", /* various forms of garbage - the latter is an actual term for "cigarette", you can stop whining about political correctness */
	"Private",
	"Corporal",
	"Space Sergeant",
	"Space Cadet",
	"Space Lieutenant",
	"Space Captain",
	"Space Major",
	"Space Colonel",
	"Space General",
	"Krog", "Winston", "Cyrus", /* Clan EIT; Cyrus is also the ZAPM lead dev */
	"Yoof",
	"Cannon Fodder",
	"Meatshield",
	"Warbuddy",
	"Skarboy",
	"Mob Kaptain",
	"Nob",
	"Meganob",
	"Warboss",
	"Cortege", "Tache", "Pillory", /* from Pokemon Vietnamese Crystal */
	"Unblooded",
	"Blooded",
	"Honored One",
	"Master Hunter",
	"Vanguard",
	"Elite",
	"Clan Leader",
	"Elder",
	"Adjudicator",
	"Macbeth", "Zoness", "Titania", /* Star Fox 64 */
	"Towel Boy",
	"Bench Warmer",
	"Starter",
	"Jock",
	"Star Player",
	"Team Captain",
	"MVP",
	"Pro Bowler",
	"Hall Of Famer",
	"Roger Staubach", "Peyton Manning", "Tom Brady", /* famous quarterbacks */
	"Odd Ball",
	"Weirdo",
	"Mind Reader",
	"Spoon Bender",
	"Freakazoid",
	"Telepath",
	"Spyion",
	"Master Psyker",
	"Farseer",
	"Bill Rizer", "Lance Bean", "Haggle Man", /* classic NES games */
	"Enlightened One",
	"Mind Mirror",
	"Thought Catcher",
	"Subconscious Link",
	"Psionic Vein",
	"Brainwave Warper",
	"Neuroconductor",
	"Mind Overlord",
	"Fate Weaver",
	"Grolla Seyfarth", "Pamela & Carl Arwig", "Iris Sepperin", /* erka_es, and the "Carl Arwig" is an inside joke */
	"Far Herald",
	"Space Brother",
	"Cult Idol",
	"First Contact",
	"Federation Envoy",
	"Galactic Senator",
	"Coadunator",
	"Uplifter",
	"Terraformer",
	"Marie Curie", "Georg Simon Ohm", "Albert Einstein", /* famous scientists */
	"Crazy One",
	"Very Mad Orc",
	"Frazzler",
	"Humming Head",
	"Brain Warper",
	"Walking Psi Bomb",
	"One Orc Storm",
	"Warptide",
	"Warp Unleasher",
	"Tax Evasion", "Murder and Arson", "Prison Outbreak", /* crimes, listed in order of severity from minor to major :P */
	"Rocket Tester",
	"Vostok Martyr",
	"Deimos Stevedore",      "Moon Walker",
	"Leonov Engineer",
	"Marauder Pilot",
	"Von Braun Staff",
	"LongShot Navigator",
	"Tie Fighter Ace",
	"Nostromo Survivor",
	"Neil Armstrong", "Sally Ride", "Yuri Gagarin", /* famous astronauts */
	"Eavesdropper",
	"Looming Shadow",
	"Infiltrator",
	"Sabotager",
	"Terrorist",
	"Corporate Spy",
	"Black Hand",
	"Cyber Assassin",
	"Unseen Master",
	"Diddy the Fail Master", "Bantor the Gay Voice", "Conker the Bad Fur Squirrel", /* Diddy Kong Failing :P */
	"Mars Castaway",   "Moon Base Staff",
	"Orbital Watch",
	"Land Agent",
	"Covert Eye",
	"Arms Dealer",
	"Belt Capo",
	"Planet Conqueror",
	"System Kingpin",
	"Sector Crimelord",
	"Aung San Suu Kyi", "Ai Weiwei", "Vladimir Bukovsky", /* famous dissidents */
	"Outworlder",   "Void Explorer",
	"Genesis Witness",
	"Chaos Shaper",
	"Nebulae Catcher",
	"Star Assembler",
	"Planet Shifter",
	"Galaxy Designer",
	"DNA Programmer",
	"Species Creator",
	"Jim Raynor", "Hierarch Artanis", "Sarah Kerrigan", /* starcraft */
	"Learner",
	"Shuffler",
	"Player",
	"Reader",
	"Strategist",
	"Deck Stacker",
	"Card Collector",
	"Deck Stacker",
	"King of Games", "Queen of Games",
	"Johnny", "Spike", "Timmy", /* Card player archetypes */
	"Servant of Scales",
	"Page of Wings",
	"Page of Claws",
	"Page of Scales",
	"Knight of the Skies",
	"Knight of Talons",
	"Knight of Scales",
	"Knight of Power",
	"Knight of Breath",
	"Enki", "Enlil", "Ereshkigal", /* Sumerian */
	"Fishtank Dipper",
	"Nibble Arouser",
	"Pondseeker",
	"Submarine Crew",
	"Flood Lover",
	"Player's Competitor",
	"Deep Biter",
	"Monsterfish Fighter",
	"First Place Challenger",
	"Cheep Cheep", "Blubber", "Boss Bass", /* Super Mario */
	"Oppressed Lab Worker",
	"Will-less Slave",
	"One Who Fears Assistants",
	"Beach Drama Experiencer",
	"Despotism Endurer",
	"Annoyed Ramming Support",
	"Rebel Leader",
	"System Overthrower",
	"Accomplished Diploma Student",
	"Divert", "Oat Camper", "Jeannine", /* assistants */
	"Sexhater",
	"Relationship Avoider",
	"Me-Neither-Shouter",
	"Privileged Potato",
	"Butt Resoler",
	"Terrible Dad",
	"Projection Of All That Is Wrong",
	"Militant Emera",
	"Exshooter",
	"Slim Jim", "Milwaukee Jon", "Bose Jefferson", /* Road Rash */
	"Javazon Wannabe",
	"Peeker",
	"Graystabber",
	"Stack Increaser",
	"Uncommon Flinger",
	"Wood Chooser",
	"Javelin Forger",
	"Bigstacker",
	"Torpedo Launcher",
	"Arihant", "Siddha", "Acharya", /* Jain */
	"Club Carrier",
	"Bola Thrower",
	"Range Applier",
	"Melee Detonator",
	"Iron Maul",
	"Sounding Clasher",
	"Two-Square Melee Master",
	"Nun-Chuck",
	"World Whacker",
	"Sulis", "Sequana", "Damona", /* celtic */
	"Trash Eater",
	"Garbage Can Searcher",
	"Waste Collector",
	"Refuse Picker",
	"True Grimer",
	"Reusable Plastic Bag",
	"Bottle Trader",
	"Truckloader",
	"Ocean Cleaner",
	"Indra", "Soma", "Ishwara", /* rigvedic */
	"Moldhome",
	"Piggybacker",
	"Stationary Seeker",
	"Symbiote Powerer",
	"Master of Greater Powers",
	"Sym-Breather",
	"Elite Symbiant",
	"Master Jelly Farmer",
	"Supreme Symbiosis Teacher",
	"Erotic Air Current Noises", "Wonderful Rubbing Noises", "Sexy Licking Noises", /* Amyism :D */
	"Asterisk User",
	"Language Faker",
	"Speech Policer",
	"Aggressive Advocate",
	"Bank Suer",
	"Placation Striker",
	"Inside Squeezer",
	"Truth Minister",
	"Inclusive Spacer",
	"Buergerinnen und Buerger", "Buergerlnnen", "Buerger*innen", /* some weird language that pretends to be German; on some fonts "I" and "l" look the same, therefore the neutral god is spelled like that on purpose :P */
	"Fighter Lad",
	"Puncher",
	"Weaponless Adventurer",
	"Spell-Abstainer",
	"Trained Muscle",
	"Lack Compensator",
	"Specialized Minmaxer",
	"Almost Supreme Master",
	"Combat Boss",
	"Arev", "U.GUR", "Khaldi", /* armenian */
	"a stylized eye formed from the letters V, F, and D",
	"Stroking Hands",
	"Feel-Upper",
	"Farting Gas Inhaler",
	"Squeaking Noise Enjoyer",
	"Curve Lover",
	"Graceful Body Admirer",
	"Blissful Air Current",
	"Sexiness Expert",
	"Master Softhand",
	"Eveline", "Elena", "Marlen",
	"Swing Student",
	"Toe Stepper",
	"Spinner",
	"Gymnast",
	"Dancehall Artist",
	"Sindancer",
	"Souldancer",
	"Lifedancer",
	"World-Class Dancer",
	"the Archomentals", "Cryonax", "the Elder Elemental",
	"Stocks That Hunter Inside Dweller",
	"Cemetery Plunderer",
	"Catacomb Evil-Fighter",
	"Desert Caravanist",
	"Tomb Comber",
	"Jungle Stinger",
	"Temple Cleanser",
	"Heavenly Fortress Defender",
	"Diablobane",
	"Athulua", "Bul-Kathos", "Rathma", /* Diablo series */
	"Newcomer",
	"Green Cross Delver",
	"Moneyless Adventurer",
	"Balance Grumbler",
	"Crash Bug Encounterer",
	"Bad Game Design Bitcher",
	"Fook-Yoo-Sayer",
	"Ch3at0r",
	"Ascension Run Skipper",
	"0.54A", "Level Change UAE", "Reset Button Without A Confirmation", /* still salty about the latter... :-P --Amy */
	"Hidden Operative",
	"Covert OP",
	"Underground Agitator",
	"Secret Schemer",
	"Gun Hider",
	"Shady Executive",
	"Banishment Wielder",
	"Top-Secret Advice Member",
	"Boss of the Secret Advice",
	"Katrin", "Len-kind", "Coffin Nail",
	"Socksmeller",
	"Combat Boot Slave",
	"Shin-Smashed Sneaker",
	"Bathtub Despairer",
	"Dogshit Challenger",
	"Final Dancer",
	"Fall-From-Grace",
	"Toilet Puzzler",
	"Winner At Last",
	"Birkenstock", "Tamaris", "Peter Kaiser", /* German shoe brands */
	"Half Baker",
	"Bloody Novice",
	"No-Hoper",
	"Pupil of Gravel",
	"Pupil of Fluids",
	"Pupil of Trash",
	"Pupil of Blows",
	"Pupil of Heat",
	"Self-Appointed Master",
	"Nuwa", "Pangu", "Fuxi", /* Chinese */

};

/* Amy note: it is not a bug that the rank titles are in here; they normally cannot be chosen as hallucinated monsters,
 * because those are taken either from this list or from monst.c. The rank titles are in role.c though. Because we want
 * variety (unlike Soviet5lo), we added all those names here so they can actually be picked. */

/* Return a random monster name, for hallucination.
 * KNOWN BUG: May be a proper name (Godzilla, Barney), may not
 * (the Terminator, a Dalek).  There's no elegant way to deal
 * with this without radically modifying the calling functions.
 */
const char *
rndmonnam()
{
	int name;

	/*do {*/
	    if (!issoviet) name = rn1(NUMMONS + SIZE(bogusmons) - LOW_PM, LOW_PM);
	/*} while (name < NUMMONS &&
	    (type_is_pname(&mons[name]) || (mons[name].geno & G_NOGEN)));*/
/* All monster names should be possible, even unique and genocided ones. This adds more variety. --Amy */

	else {
		do {
		    name = rn1(NUMMONS + SIZE(bogusmons) - LOW_PM, LOW_PM);
		} while (name < NUMMONS && (type_is_pname(&mons[name]) || (mons[name].geno & G_NOGEN)));

	}

	/* "Revert hallucination monster name changes. This should disable genocided and unique monsters from appearing in the list of monsters." In Soviet Russia, people HATE variety with a fiery passion. Sure, it doesn't really make sense to have genocided monsters appear, but... why the heck disable unique ones??? They exist outside of hallucination too, and hallucination is supposed to make the player think the monster is some other monster, so why shouldn't the player's sub-conscious (or whatever's responsible for that) try to tell them that the monster in front of them is Demogorgon or something like that? --Amy */

	if (name >= NUMMONS) return bogusmons[name - NUMMONS];
	return mons[name].mname;
}

#ifdef REINCARNATION
const char *
roguename() /* Name of a Rogue player */
{
	char *i, *opts;

	if ((opts = nh_getenv("ROGUEOPTS")) != 0) {
		for (i = opts; *i; i++)
			if (!strncmp("name=",i,5)) {
				char *j;
				if ((j = index(i+5,',')) != 0)
					*j = (char)0;
				return i+5;
			}
	}
	return rn2(3) ? (rn2(2) ? "Michael Toy" : "Kenneth Arnold")
		: "Glenn Wichman";
}
#endif /* REINCARNATION */
#endif /* OVLB */

#ifdef OVL2

static NEARDATA const char * const hcolors[] = {
	"ultraviolet", "infrared", "bluish-orange",
	"reddish-green", "dark white", "light black", "sky blue-pink",
	"salty", "sweet", "sour", "bitter", "umami",
	"striped", "spiral", "swirly", "plaid", "checkered", "argyle",
	"paisley", "blotchy", "guernsey-spotted", "polka-dotted",
	"square", "round", "triangular", "octarine",
	"cabernet", "sangria", "fuchsia", "wisteria",
	"lemon-lime", "strawberry-banana", "peppermint",
	"dancing", "singing", "loving",
	"electric", "welsh onion", "your colored",
	"loudy", "noisy", "clattery", "silent",
	"romantic", "incandescent", "multicolored",
	"apocyan", "cosmogone", "gant", "infra-pink",
	"irrigo", "opalescent", "peligin", "razzmatazz",
	"violant", "viric",
};

const char *
hcolor(colorpref)
const char *colorpref;
{
	return (Hallucination || isblait || !colorpref) ?
		hcolors[rn2(SIZE(hcolors))] : colorpref;
}

/* return a random real color unless hallucinating */
const char *
rndcolor()
{
	int k = rn2(CLR_MAX);
	return Hallucination ? hcolor((char *)0) : (k == NO_COLOR) ?
		"colorless" : c_obj_colors[k];
}

/* Aliases for road-runner nemesis
 */
static const char * const coynames[] = {
	"Carnivorous Vulgaris","Road-Runnerus Digestus",
	"Eatibus Anythingus"  ,"Famishus-Famishus",
	"Eatibus Almost Anythingus","Eatius Birdius",
	"Famishius Fantasticus","Eternalii Famishiis",
	"Famishus Vulgarus","Famishius Vulgaris Ingeniusi",
	"Eatius-Slobbius","Hardheadipus Oedipus",
	"Carnivorous Slobbius","Hard-Headipus Ravenus",
	"Evereadii Eatibus","Apetitius Giganticus",
	"Hungrii Flea-Bagius","Overconfidentii Vulgaris",
	"Caninus Nervous Rex","Grotesques Appetitus",
	"Nemesis Riduclii","Canis latrans"
};

char *
coyotename(mtmp, buf)
struct monst *mtmp;
char *buf;
{
    if (mtmp && buf) {
	sprintf(buf, "%s - %s",
	    x_monnam(mtmp, ARTICLE_NONE, (char *)0, 0, TRUE),
	    mtmp->mcan ? coynames[SIZE(coynames)-1] : coynames[rn2(SIZE(coynames)-1)]);
    }
    return buf;
}
#endif /* OVL2 */

/*do_name.c*/

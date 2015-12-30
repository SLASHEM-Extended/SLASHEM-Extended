/*	SCCS Id: @(#)cmd.c	3.4	2003/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "func_tab.h"
/* #define DEBUG */	/* uncomment for debugging */

/*
 * Some systems may have getchar() return EOF for various reasons, and
 * we should not quit before seeing at least NR_OF_EOFS consecutive EOFs.
 */
#if defined(SYSV) || defined(DGUX) || defined(HPUX)
#define NR_OF_EOFS	20
#endif

#define CMD_TRAVEL (char)0x90

#ifdef DEBUG
/*
 * only one "wiz_debug_cmd" routine should be available (in whatever
 * module you are trying to debug) or things are going to get rather
 * hard to link :-)
 */
extern int NDECL(wiz_debug_cmd);
#endif

#ifdef DUMB	/* stuff commented out in extern.h, but needed here */
extern int NDECL(doapply); /**/
extern int NDECL(dorub); /**/
extern int NDECL(dojump); /**/
extern int NDECL(doextlist); /**/
extern int NDECL(dodrop); /**/
extern int NDECL(doddrop); /**/
extern int NDECL(dodown); /**/
extern int NDECL(doup); /**/
extern int NDECL(donull); /**/
extern int NDECL(dowipe); /**/
extern int NDECL(do_mname); /**/
extern int NDECL(ddocall); /**/
extern int NDECL(dotakeoff); /**/
extern int NDECL(doremring); /**/
extern int NDECL(dowear); /**/
extern int NDECL(doputon); /**/
extern int NDECL(doddoremarm); /**/
extern int NDECL(dokick); /**/
extern int NDECL(dofire); /**/
extern int NDECL(dofire); /**/
extern int NDECL(dothrow); /**/
extern int NDECL(doeat); /**/
extern int NDECL(done2); /**/
extern int NDECL(doengrave); /**/
extern int NDECL(dopickup); /**/
extern int NDECL(ddoinv); /**/
extern int NDECL(dotypeinv); /**/
extern int NDECL(dolook); /**/
extern int NDECL(doprgold); /**/
extern int NDECL(doprwep); /**/
extern int NDECL(doprarm); /**/
extern int NDECL(doprring); /**/
extern int NDECL(dopramulet); /**/
extern int NDECL(doprtool); /**/
extern int NDECL(dosuspend); /**/
extern int NDECL(doforce); /**/
extern int NDECL(doopen); /**/
extern int NDECL(doclose); /**/
extern int NDECL(dosh); /**/
extern int NDECL(dodiscovered); /**/
extern int NDECL(doset); /**/
extern int NDECL(dotogglepickup); /**/
extern int NDECL(dowhatis); /**/
extern int NDECL(doquickwhatis); /**/
extern int NDECL(dowhatdoes); /**/
extern int NDECL(dohelp); /**/
extern int NDECL(dohistory); /**/
extern int NDECL(doloot); /**/
extern int NDECL(dodrink); /**/
extern int NDECL(dodip); /**/
extern int NDECL(dosacrifice); /**/
extern int NDECL(dopray); /**/
extern int NDECL(doturn); /**/
extern int NDECL(dotech); /**/
extern int NDECL(doredraw); /**/
extern int NDECL(doread); /**/
extern int NDECL(dosave); /**/
extern int NDECL(dosearch); /**/
extern int NDECL(doidtrap); /**/
extern int NDECL(dopay); /**/
extern int NDECL(dosit); /**/
extern int NDECL(dotalk); /**/
extern int NDECL(docast); /**/
extern int NDECL(dovspell); /**/
extern int NDECL(dotele); /**/
extern int NDECL(dountrap); /**/
extern int NDECL(doversion); /**/
extern int NDECL(doextversion); /**/
extern int NDECL(doswapweapon); /**/
extern int NDECL(doswapweapon); /**/
extern int NDECL(dowield); /**/
extern int NDECL(dowieldquiver); /**/
extern int NDECL(dowieldquiver); /**/
extern int NDECL(dozap); /**/
extern int NDECL(doorganize); /**/
extern int NDECL(doremoveimarkers); /**/

#ifdef LIVELOG_SHOUT
extern int NDECL(doshout); /**/
#endif

extern int NDECL(dolistvanq); /**/

#endif /* DUMB */

#ifdef OVL1
static int NDECL((*timed_occ_fn));
#endif /* OVL1 */

STATIC_PTR int NDECL(doprev_message);
STATIC_PTR int NDECL(timed_occupation);
STATIC_PTR int NDECL(doextcmd);
#ifdef BORG
STATIC_PTR int NDECL(doborgtoggle);
#endif
STATIC_PTR int NDECL(domonability);
STATIC_PTR int NDECL(dotravel);
STATIC_PTR int NDECL(playersteal);
#if 0
STATIC_PTR int NDECL(specialpower); /* WAC -- use techniques */
#endif
# ifdef WIZARD
STATIC_PTR int NDECL(wiz_wish);
STATIC_PTR int NDECL(wiz_identify);
STATIC_PTR int NDECL(wiz_map);
/* BEGIN TSANTH'S CODE */
STATIC_PTR int NDECL(wiz_gain_ac);
STATIC_PTR int NDECL(wiz_gain_level);
STATIC_PTR int NDECL(wiz_toggle_invulnerability);
STATIC_PTR int NDECL(wiz_detect_monsters);
/* END TSANTH'S CODE */
STATIC_PTR int NDECL(wiz_genesis);
STATIC_PTR int NDECL(wiz_where);
STATIC_PTR int NDECL(wiz_detect);
STATIC_PTR int NDECL(wiz_panic);
STATIC_PTR int NDECL(wiz_polyself);
STATIC_PTR int NDECL(wiz_level_tele);
STATIC_PTR int NDECL(wiz_level_change);
STATIC_PTR int NDECL(wiz_show_seenv);
STATIC_PTR int NDECL(wiz_show_vision);
STATIC_PTR int NDECL(wiz_mon_polycontrol);
STATIC_PTR int NDECL(wiz_show_wmodes);
extern void FDECL(list_vanquished, (int, BOOLEAN_P)); /* showborn patch */
#if defined(__BORLANDC__) && !defined(_WIN32)
extern void FDECL(show_borlandc_stats, (winid));
#endif
#ifdef DEBUG_MIGRATING_MONS
STATIC_PTR int NDECL(wiz_migrate_mons);
#endif
STATIC_DCL void FDECL(count_obj, (struct obj *, long *, long *, BOOLEAN_P, BOOLEAN_P));
STATIC_DCL void FDECL(obj_chain, (winid, const char *, struct obj *, long *, long *));
STATIC_DCL void FDECL(mon_invent_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(mon_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(contained, (winid, const char *, long *, long *));
STATIC_PTR int NDECL(wiz_show_stats);
#ifdef DISPLAY_LAYERS
STATIC_PTR int NDECL(wiz_show_display);
#endif
#  ifdef PORT_DEBUG
STATIC_DCL int NDECL(wiz_port_debug);
#  endif
# endif
int NDECL(enter_explore_mode);
STATIC_PTR int NDECL(doattributes);
STATIC_PTR int NDECL(doconduct); /**/
STATIC_PTR boolean NDECL(minimal_enlightenment);
STATIC_PTR int FDECL(makemenu, (const char *, struct menu_list *));

static NEARDATA struct rm *maploc;

#ifdef OVLB
STATIC_DCL void FDECL(enlght_line, (const char *,const char *,const char *));
STATIC_DCL char *FDECL(enlght_combatinc, (const char *,int,int,char *));
#ifdef UNIX
static void NDECL(end_of_input);
#endif
#endif /* OVLB */

static const char* readchar_queue="";

STATIC_DCL char *NDECL(parse);
STATIC_DCL boolean FDECL(help_dir, (CHAR_P,const char *));

STATIC_PTR int NDECL(domenusystem); /* WAC the menus*/

#ifdef BORG
/* in borg.c */
extern char borg_on;
extern char borg_line[80];
char borg_input(void);
#endif
#ifdef OVL1

STATIC_PTR int
doprev_message()
{

	if (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone()) {
	pline("The previous message command is currently unavailable!");
	display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

    return nh_doprev_message();
}


/* Count down by decrementing multi */
STATIC_PTR int
timed_occupation()
{
	(*timed_occ_fn)();
	if (multi > 0)
		multi--;
	return multi > 0;
}

/* If you have moved since initially setting some occupations, they
 * now shouldn't be able to restart.
 *
 * The basic rule is that if you are carrying it, you can continue
 * since it is with you.  If you are acting on something at a distance,
 * your orientation to it must have changed when you moved.
 *
 * The exception to this is taking off items, since they can be taken
 * off in a number of ways in the intervening time, screwing up ordering.
 *
 *	Currently:	Take off all armor.
 *			Picking Locks / Forcing Chests.
 *			Setting traps.
 */
void
reset_occupations()
{
	reset_remarm();
	reset_pick();
	reset_trapset();
}

/* If a time is given, use it to timeout this function, otherwise the
 * function times out by its own means.
 */
void
set_occupation(fn, txt, xtime)
int NDECL((*fn));
const char *txt;
int xtime;
{
	if (xtime) {
		occupation = timed_occupation;
		timed_occ_fn = fn;
	} else
		occupation = fn;
	occtxt = txt;
	occtime = 0;
	return;
}

#ifdef REDO

static char NDECL(popch);

/* Provide a means to redo the last command.  The flag `in_doagain' is set
 * to true while redoing the command.  This flag is tested in commands that
 * require additional input (like `throw' which requires a thing and a
 * direction), and the input prompt is not shown.  Also, while in_doagain is
 * TRUE, no keystrokes can be saved into the saveq.
 */
#define BSIZE 20
static char pushq[BSIZE], saveq[BSIZE];
static NEARDATA int phead, ptail, shead, stail;

static char
popch() {
	/* If occupied, return '\0', letting tgetch know a character should
	 * be read from the keyboard.  If the character read is not the
	 * ABORT character (as checked in pcmain.c), that character will be
	 * pushed back on the pushq.
	 */
	if (occupation) return '\0';
	if (in_doagain) return(char)((shead != stail) ? saveq[stail++] : '\0');
	else		return(char)((phead != ptail) ? pushq[ptail++] : '\0');
}

char
pgetchar() {		/* curtesy of aeb@cwi.nl */
	register int ch;

	if(!(ch = popch()))
		ch = nhgetch();
	return((char)ch);
}

/* A ch == 0 resets the pushq */
void
pushch(ch)
char ch;
{
	if (!ch)
		phead = ptail = 0;
	if (phead < BSIZE)
		pushq[phead++] = ch;
	return;
}

/* A ch == 0 resets the saveq.	Only save keystrokes when not
 * replaying a previous command.
 */
void
savech(ch)
char ch;
{
	if (!in_doagain) {
		if (!ch)
			phead = ptail = shead = stail = 0;
		else if (shead < BSIZE)
			saveq[shead++] = ch;
	}
	return;
}
#endif /* REDO */

#endif /* OVL1 */
#ifdef OVLB

STATIC_PTR int
doextcmd()	/* here after # - now read a full-word command */
{
	int idx, retval;

	/* keep repeating until we don't run help or quit */
	do {
	    idx = get_ext_cmd();
	    if (idx < 0) return 0;	/* quit */

	    retval = (*extcmdlist[idx].ef_funct)();
	} while (extcmdlist[idx].ef_funct == doextlist);

	return retval;
}

int
doextlist()	/* here after #? - now list all full-word commands */
{
	register const struct ext_func_tab *efp;
	char	 buf[BUFSZ];
	winid datawin;

	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
	putstr(datawin, 0, "            Extended Commands List");
	putstr(datawin, 0, "");
	putstr(datawin, 0, "    Press '#', then type:");
	putstr(datawin, 0, "");

	for(efp = extcmdlist; efp->ef_txt; efp++) {
		Sprintf(buf, "    %-15s - %s.", efp->ef_txt, efp->ef_desc);
		putstr(datawin, 0, buf);
	}
	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
	return 0;
}

STATIC_PTR int
doremoveimarkers()
{
	int x, y;
	for(x=0; x<COLNO; x++) for(y=0; y<ROWNO; y++) {
		if (isok(x, y)) {
			if (memory_is_invisible(x, y)) {
			    unmap_object(x,y);
			    newsym(x,y);
			}
		}
	}
	pline("Remembered monster markers removed.");

	return 0;
}


#ifdef BORG
STATIC_PTR int 
doborgtoggle()
{
	char    qbuf[QBUFSZ];
	char    c;
	Strcpy(qbuf,"Really enable cyborg?");
	if ((c = yn_function(qbuf, ynqchars, 'n')) == 'y') {
		borg_on = 1;
		pline("The cyborg is enabled.... Good luck!");
	}
	return 0;
}
#endif

#if defined(TTY_GRAPHICS) || defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)
#define MAX_EXT_CMD 60		/* Change if we ever have > 40 ext cmds */
/*
 * This is currently used only by the tty port and is
 * controlled via runtime option 'extmenu'
 * -AJA- The SDL/GL window systems use it too.
 */
int
extcmd_via_menu()	/* here after # - now show pick-list of possible commands */
{
    const struct ext_func_tab *efp;
    menu_item *pick_list = (menu_item *)0;
    winid win;
    anything any;
    const struct ext_func_tab *choices[MAX_EXT_CMD];
    char buf[BUFSZ];
    char cbuf[QBUFSZ], prompt[QBUFSZ], fmtstr[20];
    int i, n, nchoices, acount;
    int ret,  biggest;
    int accelerator, prevaccelerator;
    int  matchlevel = 0;

    ret = 0;
    cbuf[0] = '\0';
    biggest = 0;
    while (!ret) {
	    i = n = 0;
	    accelerator = 0;
	    any.a_void = 0;
	    /* populate choices */
	    for(efp = extcmdlist; efp->ef_txt; efp++) {
		if (!matchlevel || !strncmp(efp->ef_txt, cbuf, matchlevel)) {
			choices[i++] = efp;
			if ((int)strlen(efp->ef_desc) > biggest) {
				biggest = strlen(efp->ef_desc);
				Sprintf(fmtstr,"%%-%ds", biggest + 15);
			}
#ifdef DEBUG
			if (i >= MAX_EXT_CMD - 2) {
			    impossible("Exceeded %d extended commands in doextcmd() menu",
					MAX_EXT_CMD - 2);
			    return 0;
			}
#endif
		}
	    }
	    choices[i] = (struct ext_func_tab *)0;
	    nchoices = i;
	    /* if we're down to one, we have our selection so get out of here */
	    if (nchoices == 1) {
		for (i = 0; extcmdlist[i].ef_txt != (char *)0; i++)
			if (!strncmpi(extcmdlist[i].ef_txt, cbuf, matchlevel)) {
				ret = i;
				break;
			}
		break;
	    }

	    /* otherwise... */
	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);
	    prevaccelerator = 0;
	    acount = 0;
	    for(i = 0; choices[i]; ++i) {
		accelerator = choices[i]->ef_txt[matchlevel];
		if (accelerator != prevaccelerator || nchoices < (ROWNO - 3)) {
		    if (acount) {
 			/* flush the extended commands for that letter already in buf */
			Sprintf(buf, fmtstr, prompt);
			any.a_char = prevaccelerator;
			add_menu(win, NO_GLYPH, &any, any.a_char, 0,
					ATR_NONE, buf, FALSE);
			acount = 0;
		    }
		}
		prevaccelerator = accelerator;
		if (!acount || nchoices < (ROWNO - 3)) {
		    Sprintf(prompt, "%s [%s]", choices[i]->ef_txt,
				choices[i]->ef_desc);
		} else if (acount == 1) {
		    Sprintf(prompt, "%s or %s", choices[i-1]->ef_txt,
				choices[i]->ef_txt);
		} else {
		    Strcat(prompt," or ");
		    Strcat(prompt, choices[i]->ef_txt);
		}
		++acount;
	    }
	    if (acount) {
		/* flush buf */
		Sprintf(buf, fmtstr, prompt);
		any.a_char = prevaccelerator;
		add_menu(win, NO_GLYPH, &any, any.a_char, 0, ATR_NONE, buf, FALSE);
	    }
	    Sprintf(prompt, "Extended Command: %s", cbuf);
	    end_menu(win, prompt);
	    n = select_menu(win, PICK_ONE, &pick_list);
	    destroy_nhwindow(win);
	    if (n==1) {
		if (matchlevel > (QBUFSZ - 2)) {
			free((genericptr_t)pick_list);
#ifdef DEBUG
			impossible("Too many characters (%d) entered in extcmd_via_menu()",
				matchlevel);
#endif
			ret = -1;
		} else {
			cbuf[matchlevel++] = pick_list[0].item.a_char;
			cbuf[matchlevel] = '\0';
			free((genericptr_t)pick_list);
		}
	    } else {
		if (matchlevel) {
			ret = 0;
			matchlevel = 0;
		} else
			ret = -1;
	    }
    }
    return ret;
}
#endif

/* #monster command - use special monster ability while polymorphed */
/* edit by Amy - some polymorph forms have several abilities, so I'll try to make the player able to choose. */
STATIC_PTR int
domonability()
{
	if (can_breathe(youmonst.data) && yn("Do you want to use your breath attack?")=='y' ) return dobreathe();
	else if (attacktype(youmonst.data, AT_SPIT) && yn("Do you want to use your spit attack?")=='y' ) return dospit();
	else if (youmonst.data->mlet == S_NYMPH && yn("Do you want to remove an iron ball?")=='y' ) return doremove();
	else if (attacktype(youmonst.data, AT_GAZE) && yn("Do you want to use your gaze attack?")=='y' ) return dogaze();
	else if (is_were(youmonst.data) && yn("Do you want to summon help?")=='y' ) return dosummon();
	else if (webmaker(youmonst.data) && yn("Do you want to spin webs?")=='y' ) return dospinweb();
	else if (is_hider(youmonst.data) && yn("Do you want to hide?")=='y' ) return dohide();
	else if (is_mind_flayer(youmonst.data) && yn("Do you want to emit a mind blast?")=='y' ) return domindblast();
	else if (u.umonnum == PM_GREMLIN && yn("Do you want to replicate in water?")=='y' ) {
	    if(IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		if (split_mon(&youmonst, (struct monst *)0))
		    dryup(u.ux, u.uy, TRUE);
	    } else There("is no fountain here.");
	} else if (is_unicorn(youmonst.data) && yn("Do you want to cure yourself with your horn?")=='y' ) {
	    use_unicorn_horn((struct obj *)0);
	    return 1;
	} else if (youmonst.data->msound == MS_SHRIEK && yn("Do you want to shriek?")=='y' ) {
	    You("shriek.");
	    if(u.uburied)
		pline("Unfortunately sound does not carry well through rock.");
	    else aggravate();
	} else if (youmonst.data->msound == MS_FART_QUIET && yn("Do you want to fart?")=='y' ) {
		if (u.uhunger <= 10) {
			pline("There isn't enough gas stored in your %s butt!", flags.female ? "sexy" : "ugly");
			return 0;
		} else {
			morehungry(10);
			pline("You produce %s farting noises with your %s butt.", rn2(2) ? "tender" : "soft", flags.female ? "sexy" : "ugly");
			badeffect();
			return 1;
		}
	} else if (youmonst.data->msound == MS_FART_NORMAL && yn("Do you want to fart?")=='y' ) {
		if (u.uhunger <= 10) {
			pline("There isn't enough gas stored in your %s butt!", flags.female ? "sexy" : "ugly");
			return 0;
		} else {
			morehungry(10);
			pline("You produce %s farting noises with your %s butt.", rn2(2) ? "beautiful" : "squeaky", flags.female ? "sexy" : "ugly");
			badeffect();
			return 1;
		}
	} else if (youmonst.data->msound == MS_FART_LOUD && yn("Do you want to fart?")=='y' ) {
		if (u.uhunger <= 10) {
			pline("There isn't enough gas stored in your %s butt!", flags.female ? "sexy" : "ugly");
			return 0;
		} else {
			morehungry(10);
			pline("You produce %s farting noises with your %s butt.", rn2(2) ? "disgusting" : "loud", flags.female ? "sexy" : "ugly");
			badeffect();
			return 1;
		}
	} else if (Upolyd)
		pline("Any (other) special ability you may have is purely reflexive.");
	else You("don't have another special ability in your normal form!");
	return 0;
}

int	/* deathexplore patch */
enter_explore_mode()
{
	char buf[BUFSZ];
	if(!discover && !wizard) {
		pline("Beware!  From explore mode there will be no return to normal game.");

		  getlin ("Do you want to enter explore mode? [yes/no]?",buf);
		  (void) lcase (buf);
		  if (!(strcmp (buf, "yes"))) {
			clear_nhwindow(WIN_MESSAGE);
			You("are now in non-scoring explore mode.");
			discover = TRUE;
		}
		else {
			clear_nhwindow(WIN_MESSAGE);
			pline("Resuming normal game.");
		}
	}
	return 0;
}


STATIC_PTR int
playersteal()
{

	if (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone()) {
	pline("The borrow command is currently unavailable!");
	display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	register int x, y;
        int temp, chanch, base, dexadj, statbonus = 0;
	boolean no_steal = FALSE;

	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) {
		pline("Could be hard without hands ...");
		no_steal = TRUE;
	} else
	if (near_capacity() > SLT_ENCUMBER) {
		Your("load is too heavy to attempt to steal.");
		no_steal = TRUE;
	}
	if (no_steal) {
		/* discard direction typeahead, if any */
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}

	if(!getdir(NULL)) return(0);
	if(!u.dx && !u.dy) return(0);

	x = u.ux + u.dx;
	y = u.uy + u.dy;
	
	if(u.uswallow) {
		pline("You search around but don't find anything.");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(1);
	}

	u_wipe_engr(2);

	maploc = &levl[x][y];

	if(MON_AT(x, y)) {
		register struct monst *mdat = m_at(x, y);

		/* calculate chanch of sucess */
		base = 5;
		dexadj = 1;
		if (Role_if(PM_ROGUE)) {
			base = 5 + (u.ulevel * 2);
			dexadj = 3;
		}
		if (Role_if(PM_PICKPOCKET)) {
			base = 5 + (u.ulevel * 3);
			dexadj = 5;
		}

		if (Race_if(PM_HUMANOID_LEPRECHAUN)) {

			base += u.ulevel;
			dexadj += 1;
		}

		if (ACURR(A_DEX) < 10) statbonus = (ACURR(A_DEX) - 10) * dexadj;
		else 
		if (ACURR(A_DEX) > 14) statbonus = (ACURR(A_DEX) - 14) * dexadj;

		chanch = base + statbonus;

		if (uarmg && uarmg->otyp != GAUNTLETS_OF_DEXTERITY)
				chanch -= 5;
		if (!uarmg)     chanch += 5;
		if (uarms)      chanch -= 10;
		if (uarm && uarm->owt < 75)       chanch += 10;
		else if (uarm && uarm->owt < 125) chanch += 5;
		else if (uarm && uarm->owt < 175) chanch += 0;
		else if (uarm && uarm->owt < 225) chanch -= 5;
		else if (uarm && uarm->owt < 275) chanch -= 10;
		else if (uarm && uarm->owt < 325) chanch -= 15;
		else if (uarm && uarm->owt < 375) chanch -= 20;
		else if (uarm)                    chanch -= 25;
		if (chanch < 5) chanch = 5;
		if (chanch > 95) chanch = 95;
		if (rnd(100) < chanch || mdat->mtame) {

#ifdef GOLDOBJ
			/* [CWC] This will steal money from the monster from the
			 * first found goldobj - we could be really clever here and
			 * then move onwards to the next goldobj in invent if we
			 * still have coins left to steal, but lets leave that until
			 * we actually have other coin types to test it on.
			 */
			struct obj *gold = findgold(mdat->minvent);
			if (gold) {
				int mongold;
				int coinstolen;
				coinstolen = (u.ulevel * rn1(25,25));
				mongold = (int)gold->quan;
				if (coinstolen > mongold) coinstolen = mongold;
				if (coinstolen > 0)	{
					if (coinstolen != mongold) 
						gold = splitobj(gold, coinstolen);
					obj_extract_self(gold);
		      if (merge_choice(invent, gold) || inv_cnt() < 52) {
				    addinv(gold);
						You("steal %s.", doname(gold));
					} else {
            You("grab %s, but find no room in your knapsack.", doname(gold));
			    	dropy(gold);
					}
				}
				else
				impossible("cmd.c:playersteal() stealing negative money");
#else
			if (mdat->mgold) {
				temp = (u.ulevel * rn1(25,25));
				if (temp > mdat->mgold) temp = mdat->mgold;
				u.ugold += temp;
				mdat->mgold -= temp;
				You("steal %d gold.",temp);
#endif
			} else
				You("don't find anything to steal.");

			if (!mdat->mtame) exercise(A_DEX, TRUE);
			return(1);
		} else {
			You("failed to steal anything.");
			setmangry(mdat);
			return(1);
	       }
	} else {
		pline("I don't see anybody to rob there!");
		return(0);
	}

	return(0);
} 

#ifdef WIZARD

/* ^W command - wish for something */
STATIC_PTR int
wiz_wish()	/* Unlimited wishes for debug mode by Paul Polderman */
{
	if (wizard) {
	    boolean save_verbose = flags.verbose;

	    flags.verbose = FALSE;
	    makewish();
	    flags.verbose = save_verbose;
	    (void) encumber_msg();
	} else
	    pline("Unavailable command '^W'.");
	return 0;
}


#if 0	/* WAC -- Now uses techniques */
STATIC_PTR int
specialpower()      /* Special class abilites [modified by Tom] */
{
	/*
	 * STEPHEN WHITE'S NEW CODE
	 *
	 * For clarification, lastuse (as declared in decl.{c|h}) is the
	 * actual length of time the power is active, nextuse is when you can
	 * next use the ability.
	 */

        /*Added wizard mode can choose to use ability - wAC*/
	if (u.unextuse) {
	    You("have to wait %s before using your ability again.",
		(u.unextuse > 500) ? "for a while" : "a little longer");
#ifdef WIZARD
            if (!wizard || (yn("Use ability anyways?") == 'n'))
#endif
                return(0);
	}

	switch (u.role) {
	    case 'A':
/*WAC stolen from the spellcasters...'A' can identify from
        historical research*/
		if(Hallucination || Stunned || Confusion) {
		    You("can't concentrate right now!");
		    break;
		} else if((ACURR(A_INT) + ACURR(A_WIS)) < rnd(60)) {
			pline("Nothing in your pack looks familiar.");
		    u.unextuse = rn1(500,500);
		    break;
		} else if(invent) {
		    int ret;
			You("examine your possessions.");
			identify_pack(1);
/*WAC this should be better - made like scroll of identify*/
/* KMH -- also commented out use of 'ret' without initialization */
/*                  ret = ggetobj("identify", identify, 1, FALSE);*/
/*		    if (ret < 0) break; *//* quit or no eligible items */
/*                  ret = ggetobj("identify", identify, 1, FALSE);*/
		} else {
			/* KMH -- fixed non-compliant string */
			You("are already quite familiar with the contents of your pack.");
		    break;
		}
		u.unextuse = rn1(500,1500);
		break;
	    case 'G':
			Your("ability, gem identification, is automatic.");
			return(0);
		case 'P':
			Your("ability, bless and curse detection, is automatic.");
			return(0);
	    case 'D':
			/* KMH -- Just do it!  (Besides, Alt isn't portable...) */
			return (polyatwill());
			/*Your("ability, polymorphing, uses the alt-y key.");*/
			/*return(0);*/
	    case 'L':
		/*only when empty handed, in human form!*/
		if (Upolyd) {
			You("can't do this while polymorphed!");
			break;
		}
		if (uwep == 0) {
			Your("fingernails extend into claws!");
			aggravate();
			u.ulastuse = d(2,4) + (u.ulevel/5) + 1; /* [max] was d(2,8) */
			u.unextuse = rn1(1000,1000); /* [max] increased delay */
		}
		else You("can't do this while holding a weapon!");
		break;
	    case 'R':
	    /* KMH -- Just do it!  (Besides, Alt isn't portable...) */
	    return (playersteal());
		/*Your("ability, stealing, uses the alt-b key.");*/
        /*return(0);*/
		break;
	    case 'M':
		Your("special ability is unarmed combat, and it is automatic.");
                return(0);
		break;
	    case 'C':
	    case 'T':
		You("don't have a special ability!");
                return(0);
		break;
	    case 'B':
		You("fly into a berserk rage!");
		u.ulastuse = d(2,8) + (u.ulevel/5) + 1;
		incr_itimeout(&HFast, u.ulastuse);
		u.unextuse = rn1(1000,500);
		return(0);
		break;
	    case 'F':
	    case 'I':
	    case 'N':
	    case 'W':
            /* WAC spell-users can study their known spells*/
		if(Hallucination || Stunned || Confusion) {
		    You("can't concentrate right now!");
		    break;
		} else {
			You("concentrate...");
			studyspell(); /*in spell.c*/
		}
		break;
	    case 'E':
                Your("%s %s become blurs as they reach for your quiver!",
			uarmg ? "gloved" : "bare",      /* Del Lamb */
			makeplural(body_part(HAND)));
                u.ulastuse = rnd((int) (u.ulevel/6 + 1)) + 1;
		u.unextuse = rn1(1000,500);
		break;
	    case 'U':
	    case 'V':
		if(!uwep || (weapon_type(uwep) == P_NONE)) {
		    You("are not wielding a weapon!");
		    break;
		} else if(uwep->known == TRUE) {
                    You("study and practice with your %s %s.",
                        uarmg ? "gloved" : "bare",      /* Del Lamb */
			makeplural(body_part(HAND)));
                    practice_weapon();
		} else {
                    if (not_fully_identified(uwep)) {
                        You("examine %s.", doname(uwep));
                            if (rnd(15) <= ACURR(A_INT)) {
                                makeknown(uwep->otyp);
                                uwep->known = TRUE;
                                You("discover it is %s",doname(uwep));
                                } else
                     pline("Unfortunately, you didn't learn anything new.");
                    } 
                /*WAC Added practicing code - in weapon.c*/
                    practice_weapon();
		}
                u.unextuse = rn1(500,500);
		break;
	    case 'H':
		if (Hallucination || Stunned || Confusion) {
		    You("are in no condition to perform surgery!");
		    break;
		}
		if ((Sick) || (Slimed)) {       /* WAC cure sliming too */
		    if(carrying(SCALPEL)) {
			pline("Using your scalpel (ow!), you cure your infection!");
			make_sick(0L,(char *)0, TRUE,SICK_ALL);
			Slimed = 0;
			if(u.uhp > 6) u.uhp -= 5;
			else          u.uhp = 1;
			u.unextuse = rn1(500,500);
			break;
		    } else pline("If only you had a scalpel...");
		}
		if (u.uhp < u.uhpmax) {
		    if(carrying(MEDICAL_KIT)) {
			pline("Using your medical kit, you bandage your wounds.");
			u.uhp += (u.ulevel * (rnd(2)+1)) + rn1(5,5);
		    } else {
			pline("You bandage your wounds as best you can.");
			u.uhp += (u.ulevel) + rn1(5,5);
		    }
		    u.unextuse = rn1(1000,500);
		    if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		} else pline("You don't need your healing powers!");
		break;
	    case 'K':
		if (u.uhp < u.uhpmax || Sick || Slimed) { /*WAC heal sliming */
			if (Sick) You("lay your hands on the foul sickness...");
			pline("A warm glow spreads through your body!");
			if (Slimed) pline_The("slime is removed.");
			Slimed = 0;
			if(Sick) make_sick(0L,(char*)0, TRUE, SICK_ALL);
			else     u.uhp += (u.ulevel * 4);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			u.unextuse = 3000;
		} else pline(nothing_happens);
		break;
	    case 'S':
		You("scream \"KIIILLL!\"");
		aggravate();
		u.ulastuse = rnd((int) (u.ulevel/6 + 1)) + 1;
		u.unextuse = rn1(1000,500);
		return(0);
		break;
#ifdef YEOMAN
		case 'Y':
#ifdef STEED
			if (u.usteed) {
				pline("%s gets tamer.", Monnam(u.usteed));
				tamedog(u.usteed, (struct obj *) 0, FALSE);
				u.unextuse = rn1(1000,500);
			} else
				Your("special ability is only effective when riding a monster.");
			break;
#else
			You("don't have a special ability!");
			return(0);
#endif
#endif
	    default:
		break;
	  }
/*By default,  action should take a turn*/
	return(1);
}
#endif


/* ^I command - identify hero's inventory */
STATIC_PTR int
wiz_identify()
{
	if (wizard)	identify_pack(0);
	else		pline("Unavailable command '^I'.");
	return 0;
}

/* ^F command - reveal the level map and any traps on it */
STATIC_PTR int
wiz_map()
{
	if (wizard) {
	    struct trap *t;
	    long save_Hconf = HConfusion,
		 save_Hhallu = HHallucination;

	    HConfusion = HHallucination = 0L;
	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (!t->hiddentrap) t->tseen = 1;
		if (!t->hiddentrap) map_trap(t, TRUE);
	    }
	    do_mapping();
	    HConfusion = save_Hconf;
	    HHallucination = save_Hhallu;
	} else
	    pline("Unavailable command '^F'.");
	return 0;
}

/* ^G command - generate monster(s); a count prefix will be honored */
STATIC_PTR int
wiz_gain_level()
{
	if (wizard) pluslvl(FALSE);
	else            pline("Unavailable command '^J'.");
	return 0;
}

/* BEGIN TSANTH'S CODE */
STATIC_PTR int
wiz_gain_ac()
{
	if (wizard) {
		if (u.ublessed < 20) {
			pline("Intrinsic AC increased by 1.");
			HProtection |= FROMOUTSIDE;
			u.ublessed++;
			flags.botl = 1;
		} else
			pline("Intrinsic AC is already maximized.");
	}
	else
		pline("Unavailable command '^C'.");
	return 0;
}

STATIC_PTR int
wiz_toggle_invulnerability()
{
	if (wizard) {
	    if ((Invulnerable == 0) && (u.uinvulnerable == FALSE)) {
	            You("will be invulnerable for 32000 turns.");
	            Invulnerable = 32000;
	            u.uinvulnerable = TRUE;
	    }
	    else {
	            You("are no longer invulnerable.");
	            Invulnerable = 0;
	            u.uinvulnerable = FALSE;
	    }
	}
	else            pline("Unavailable command '^N'.");
	return 0;
}
/* END TSANTH'S CODE */

STATIC_PTR int
wiz_detect_monsters()
{
	if (wizard) {
	            You("can detect monsters.");
		    incr_itimeout(&HDetect_monsters, 32000);
	}
	else            pline("Unavailable command '^H'.");
	return 0;
}

STATIC_PTR int
wiz_genesis()
{
	if (wizard)	(void) create_particular();
	else		pline("Unavailable command '^G'.");
	return 0;
}

/* ^O command - display dungeon layout */
STATIC_PTR int
wiz_where()
{
	if (wizard) (void) print_dungeon(FALSE, (schar *)0, (xchar *)0);
	else	    pline("Unavailable command '^O'.");
	return 0;
}

/* ^E command - detect unseen (secret doors, traps, hidden monsters) */
STATIC_PTR int
wiz_detect()
{
	if(wizard)  (void) findit();
	else	    pline("Unavailable command '^E'.");
	return 0;
}

/* ^V command - level teleport */
STATIC_PTR int
wiz_level_tele()
{
	if (wizard)	level_tele();
	else		pline("Unavailable command '^V'.");
	return 0;
}

/* #monpolycontrol command - choose new form for shapechangers, polymorphees */
STATIC_PTR int
wiz_mon_polycontrol()
{
    iflags.mon_polycontrol = !iflags.mon_polycontrol;
    pline("Monster polymorph control is %s.",
	  iflags.mon_polycontrol ? "on" : "off");
    return 0;
}

/* #levelchange command - adjust hero's experience level */
STATIC_PTR int
wiz_level_change()
{
    char buf[BUFSZ];
    int newlevel;
    int ret;

    getlin("To what experience level do you want to be set?", buf);
    (void)mungspaces(buf);
    if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
    else ret = sscanf(buf, "%d", &newlevel);

    if (ret != 1) {
	pline(Never_mind);
	return 0;
    }
    if (newlevel == u.ulevel) {
	You("are already that experienced.");
    } else if (newlevel < u.ulevel) {
	if (u.ulevel == 1) {
	    You("are already as inexperienced as you can get.");
	    return 0;
	}
	if (newlevel < 1) newlevel = 1;
	while (u.ulevel > newlevel)
	    losexp("#levelchange", TRUE, FALSE);
    } else {
	if (u.ulevel >= MAXULEV) {
	    You("are already as experienced as you can get.");
	    return 0;
	}
	if (newlevel > MAXULEV) newlevel = MAXULEV;
	while (u.ulevel < newlevel)
	    pluslvl(FALSE);
    }
    u.ulevelmax = u.ulevel;
    return 0;
}

/* #panic command - test program's panic handling */
STATIC_PTR int
wiz_panic()
{
	if (yn("Do you want to call panic() and end your game?") == 'y')
		panic("crash test.");
        return 0;
}

/* #polyself command - change hero's form */
STATIC_PTR int
wiz_polyself()
{
        polyself(TRUE);
        return 0;
}

/* #seenv command */
STATIC_PTR int
wiz_show_seenv()
{
	winid win;
	int x, y, v, startx, stopx, curx;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	/*
	 * Each seenv description takes up 2 characters, so center
	 * the seenv display around the hero.
	 */
	startx = max(1, u.ux-(COLNO/4));
	stopx = min(startx+(COLNO/2), COLNO);
	/* can't have a line exactly 80 chars long */
	if (stopx - startx == COLNO/2) startx++;

	for (y = 0; y < ROWNO; y++) {
	    for (x = startx, curx = 0; x < stopx; x++, curx += 2) {
		if (x == u.ux && y == u.uy) {
		    row[curx] = row[curx+1] = '@';
		} else {
		    v = levl[x][y].seenv & 0xff;
		    if (v == 0)
			row[curx] = row[curx+1] = ' ';
		    else
			Sprintf(&row[curx], "%02x", v);
		}
	    }
	    /* remove trailing spaces */
	    for (x = curx-1; x >= 0; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #vision command */
STATIC_PTR int
wiz_show_vision()
{
	winid win;
	int x, y, v;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	Sprintf(row, "Flags: 0x%x could see, 0x%x in sight, 0x%x temp lit",
		COULD_SEE, IN_SIGHT, TEMP_LIT);
	putstr(win, 0, row);
	putstr(win, 0, "");
	for (y = 0; y < ROWNO; y++) {
	    for (x = 1; x < COLNO; x++) {
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else {
		    v = viz_array[y][x]; /* data access should be hidden */
		    if (v == 0)
			row[x] = ' ';
		    else
			row[x] = '0' + viz_array[y][x];
		}
	    }
	    /* remove trailing spaces */
	    for (x = COLNO-1; x >= 1; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, &row[1]);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #wmode command */
STATIC_PTR int
wiz_show_wmodes()
{
	winid win;
	int x,y;
	char row[COLNO+1];
	struct rm *lev;

	win = create_nhwindow(NHW_TEXT);
	for (y = 0; y < ROWNO; y++) {
	    for (x = 0; x < COLNO; x++) {
		lev = &levl[x][y];
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else if (IS_WALL(lev->typ) || lev->typ == SDOOR)
		    row[x] = '0' + (lev->wall_info & WM_MASK);
		else if (lev->typ == CORR)
		    row[x] = '#';
		else if (IS_ROOM(lev->typ) || IS_DOOR(lev->typ))
		    row[x] = '.';
		else
		    row[x] = 'x';
	    }
	    row[COLNO] = '\0';
	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

#endif /* WIZARD */


/* -enlightenment and conduct- */
static winid en_win;
static const char
	You_[] = "You ",
	are[]  = "are ",  were[]  = "were ",
	have[] = "have ", had[]   = "had ",
	can[]  = "can ",  could[] = "could ";
static const char
	have_been[]  = "have been ",
	have_never[] = "have never ", never[] = "never ";

#define enl_msg(prefix,present,past,suffix) \
			enlght_line(prefix, final ? past : present, suffix)
#define you_are(attr)	enl_msg(You_,are,were,attr)
#define you_have(attr)	enl_msg(You_,have,had,attr)
#define you_can(attr)	enl_msg(You_,can,could,attr)
#define you_have_been(goodthing) enl_msg(You_,have_been,were,goodthing)
#define you_have_never(badthing) enl_msg(You_,have_never,never,badthing)
#define you_have_X(something)	enl_msg(You_,have,(const char *)"",something)

static void
enlght_line(start, middle, end)
const char *start, *middle, *end;
{
	char buf[BUFSZ];

	Sprintf(buf, "%s%s%s.", start, middle, end);
	putstr(en_win, 0, buf);
}



/* KMH, intrinsic patch -- several of these are updated */
void
enlightenment(final)
int final;	/* 0 => still in progress; 1 => over, survived; 2 => dead */
{
	int ltmp;
	char buf[BUFSZ];
	struct permonst *ptr;

	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, final ? "Final Attributes:" : "Current Attributes:");
	putstr(en_win, 0, "");

#ifdef ELBERETH
	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
				"the Hand of Elbereth",
				"the Envoy of Balance",
				"the Glory of Arioch"
	    };
	    you_are(hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
#endif

	/* note: piousness 20 matches MIN_QUEST_ALIGN (quest.h) */
	if (u.ualign.record >= 20)	you_are("piously aligned");
	else if (u.ualign.record > 13)	you_are("devoutly aligned");
	else if (u.ualign.record > 8)	you_are("fervently aligned");
	else if (u.ualign.record > 3)	you_are("stridently aligned");
	else if (u.ualign.record == 3)	you_are("aligned");
	else if (u.ualign.record > 0)	you_are("haltingly aligned");
	else if (u.ualign.record == 0)	you_are("nominally aligned");
	else if (u.ualign.record >= -3)	you_have("strayed");
	else if (u.ualign.record >= -8)	you_have("sinned");
	else you_have("transgressed");
#ifdef WIZARD
	if (wizard || (!rn2(10)) || final >= 1 ) { /* can randomly appear with enlightenment, always after death --Amy */
		Sprintf(buf, " %d", u.ualign.record);
		enl_msg("Your alignment ", "is", "was", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) { /* can randomly appear with enlightenment, always after death --Amy */
		Sprintf(buf, " %d", u.alignlim);
		enl_msg("Your max alignment ", "is", "was", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) { /* can randomly appear with enlightenment, always after death --Amy */
		Sprintf(buf, " %d sins", u.ualign.sins);
		enl_msg("You ", "carry", "carried", buf);
	}

	if ((wizard || (!rn2(10)) || final >= 1 ) && u.idscrollpenalty > 100) {
		Sprintf(buf, " %d", u.idscrollpenalty);
		enl_msg("Scroll identification only works 100 out of X attempts, and X ", "is", "was", buf);
	}

	if ((wizard || (!rn2(10)) || final >= 1 ) && u.idpotionpenalty > 3) {
		Sprintf(buf, " %d", u.idpotionpenalty);
		enl_msg("Potion identification only works 3 out of X attempts, and X ", "is", "was", buf);
	}

	if ((wizard || (!rn2(10)) || final >= 1 ) && u.idringpenalty > 4) {
		Sprintf(buf, " %d", u.idringpenalty);
		enl_msg("Ring identification only works 4 out of X attempts, and X ", "is", "was", buf);
	}

	if ((wizard || (!rn2(10)) || final >= 1 ) && u.idamuletpenalty > 15) {
		Sprintf(buf, " %d", u.idamuletpenalty);
		enl_msg("Amulet identification only works 15 out of X attempts, and X ", "is", "was", buf);
	}

	if ((wizard || (!rn2(10)) || final >= 1 ) && u.idwandpenalty > 3) {
		Sprintf(buf, " %d", u.idwandpenalty);
		enl_msg("Wand identification only works 3 out of X attempts, and X ", "is", "was", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) { Sprintf(buf, " %d", nartifact_exist() );
		enl_msg("Number of artifacts generated ", "is", "was", buf);
	}

	if (u.legscratching > 1) { Sprintf(buf, " %d", u.legscratching - 1);
		enl_msg("Your leg damage ", "is", "was", buf);
	}

	if (u.youpolyamount > 0) { Sprintf(buf, " %d", u.youpolyamount);
		enl_msg("Your remaining amount of free polymorphs ", "is", "was", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) { Sprintf(buf, " turn %d", u.monstertimeout);
		enl_msg("Monster spawn increase ", "start at", "would have started at", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {Sprintf(buf, " turn %d", u.monstertimefinish);
		enl_msg("Monster spawn increase ", "reaches its maximum at", "had reached its maximum at", buf);
		enl_msg("In this game, Eevee's evolution ", "is ", "was ", mons[u.eeveelution].mname );
		/* these two are tied together because the monstertimefinish variable defines the evolution --Amy */
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		enl_msg("The monster class that cannot be genocided ", "is ", "was ", monexplain[u.ungenocidable] );
	}

	if ((wizard || (!rn2(10)) || final >= 1 ) && u.alwaysinvisible) {
		enl_msg("Today, the monster class that always gets generated invisible ", "is ", "was ", monexplain[u.alwaysinvisible] );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", monexplain[u.frequentmonster], u.freqmonsterbonus);
		enl_msg("The monster class that ", "is ", "was ", buf );
	}

	if (u.freqcolorbonus && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", c_obj_colors[u.frequentcolor], u.freqcolorbonus);
		enl_msg("The monster color that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies2];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus2);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies3];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus3);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies4];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus4);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies5];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus5);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies6];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus6);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies7];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus7);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies8];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus8);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies9];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus9);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.frequentspecies10];
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus10);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.nospawnspecies];
		Sprintf(buf, "never randomly generated: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.nospawnspecies2];
		Sprintf(buf, "never randomly generated: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.nospawnspecies3];
		Sprintf(buf, "never randomly generated: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.nospawnspecies4];
		Sprintf(buf, "never randomly generated: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		ptr = &mons[u.nospawnspecies5];
		Sprintf(buf, "never randomly generated: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((wizard || (!rn2(10)) || final >= 1 ) && u.speedymonster) {
		Sprintf(buf, "always generated with extra speed: %s", monexplain[u.speedymonster]);
		enl_msg("The monster class that ", "is ", "was ", buf );
	}

	if ((wizard || (!rn2(10)) || final >= 1 ) && u.musemonster) {
		Sprintf(buf, "more likely to generate with musable stuff: %s", monexplain[u.musemonster]);
		enl_msg("The monster class that ", "is ", "was ", buf );
	}

	if (u.minimalism) {Sprintf(buf, " %d", u.minimalism);
		enl_msg("Items generate only 1 time in X, and X ", "is", "was", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable].oc_name, obj_descr[u.unobtainable].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable2].oc_name, obj_descr[u.unobtainable2].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable3].oc_name, obj_descr[u.unobtainable3].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable4].oc_name, obj_descr[u.unobtainable4].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((wizard || (!rn2(10)) || final >= 1 ) && (u.unobtainablegeno != -1) ) {
		Sprintf(buf, "genocided: %s (%s)", obj_descr[u.unobtainablegeno].oc_name, obj_descr[u.unobtainablegeno].oc_descr);
		enl_msg("This item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "generated more often: %s (%s, frequency bonus %d)", obj_descr[u.veryobtainable].oc_name, obj_descr[u.veryobtainable].oc_descr, u.veryobtainableboost);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "generated more often: %s (%s, frequency bonus %d)", obj_descr[u.veryobtainable2].oc_name, obj_descr[u.veryobtainable2].oc_descr, u.veryobtainableboost2);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "generated more often: %s (%s, frequency bonus %d)", obj_descr[u.veryobtainable3].oc_name, obj_descr[u.veryobtainable3].oc_descr, u.veryobtainableboost3);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem].oc_name, obj_descr[u.alwayscurseditem].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem2].oc_name, obj_descr[u.alwayscurseditem2].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem3].oc_name, obj_descr[u.alwayscurseditem3].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem4].oc_name, obj_descr[u.alwayscurseditem4].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "always invisible: %s", defsyms[trap_to_defsym(u.invisotrap)].explanation);
		enl_msg("The RNG hath decreed that this trap ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", defsyms[trap_to_defsym(u.frequenttrap)].explanation, u.freqtrapbonus);
		enl_msg("The RNG hath decreed that this trap ", "is ", "was ", buf );
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {Sprintf(buf, " turn %d", u.next_check);
		enl_msg("Next attribute increase check ", "comes at", "would have come at", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {Sprintf(buf, " %d", AEXE(A_STR));
		enl_msg("Strength training ", "is", "was", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {Sprintf(buf, " %d", AEXE(A_DEX));
		enl_msg("Dexterity training ", "is", "was", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {Sprintf(buf, " %d", AEXE(A_WIS));
		enl_msg("Wisdom training ", "is", "was", buf);
	}

	if (wizard || (!rn2(10)) || final >= 1 ) {Sprintf(buf, " %d", AEXE(A_CON));
		enl_msg("Constitution training ", "is", "was", buf);
	}
#endif

	if (u.negativeprotection) {Sprintf(buf, " %d", u.negativeprotection);
		enl_msg("Your protection was reduced. The amount ", "is", "was", buf);
	}

	if (u.chokhmahdamage) {Sprintf(buf, " %d", u.chokhmahdamage);
		enl_msg("The escalating chokhmah attack damage ", "is", "was", buf);
	}

	/*** Resistances to troubles ***/
	if (Fire_resistance) you_are("fire resistant");
	if (Cold_resistance) you_are("cold resistant");
	if (Sleep_resistance) you_are("sleep resistant");
	if (Disint_resistance) you_are("disintegration-resistant");
	if (Shock_resistance) you_are("shock resistant");
	if (Poison_resistance) you_are("poison resistant");
	if (Drain_resistance) you_are("level-drain resistant");
	if (Sick_resistance) you_are("immune to sickness");
	if (Antimagic) you_are("magic-protected");
	if (Acid_resistance) you_are("acid resistant");
	if (Fear_resistance) you_are("resistant to fear");
	if (Stone_resistance)
		you_are("petrification resistant");
	if (Invulnerable) {
		Sprintf(buf, "invulnerable");
	    if (wizard || (!rn2(10)) || final >= 1  ) Sprintf(eos(buf), " (%d)", Invulnerable);
		you_are(buf);
	}
	/*if (u.urealedibility) you_can("recognize detrimental food");*/

	if (u.urealedibility) {
		Sprintf(buf, "recognize detrimental food");
	   Sprintf(eos(buf), " (%d)", u.urealedibility);
		you_can(buf);
	}

	/*** Troubles ***/

	if (multi) {
		Sprintf(buf, "paralyzed.");
	      Sprintf(eos(buf), " (%d)", multi);
		you_are(buf);
	}

	if (Halluc_resistance)
		enl_msg("You resist", "", "ed", " hallucinations");
	/*if (final) { */
	if (Hallucination) {
		if (HeavyHallu) Sprintf(buf, "badly hallucinating");
		else Sprintf(buf, "hallucinating");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HHallucination);
		you_are(buf);
	}
	if (Stunned) {
		if (HeavyStunned) Sprintf(buf, "badly stunned");
		else Sprintf(buf, "stunned");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HStun);
		you_are(buf);
	}
	if (Confusion) {
		if (HeavyConfusion) Sprintf(buf, "badly confused");
		else Sprintf(buf, "confused");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HConfusion);
		you_are(buf);
	}
	if (Blind) {
		if (HeavyBlind) Sprintf(buf, "badly blinded");
		else Sprintf(buf, "blinded");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Blinded);
		you_are(buf);
	}
	if (Sick) {
		Sprintf(buf, "sick");
			if (u.usick_type & SICK_VOMITABLE) Sprintf(eos(buf), " from food poisoning");
			if (u.usick_type & SICK_NONVOMITABLE) Sprintf(eos(buf), " from illness");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Sick);
		you_are(buf);
	}
	if (Vomiting) {
		Sprintf(buf, "nauseated");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Vomiting);
		you_are(buf);
	}
	if (Feared) {
		if (HeavyFeared) Sprintf(buf, "stricken with very strong fear");
		else Sprintf(buf, "stricken with fear");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HFeared);
		you_are(buf);
	}
	if (Numbed) {
		if (HeavyNumbed) Sprintf(buf, "badly numbed");
		else Sprintf(buf, "numbed");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HNumbed);
		you_are(buf);
	}
	if (Frozen) {
		if (HeavyFrozen) Sprintf(buf, "frozen rigid and solid");
		else Sprintf(buf, "frozen solid");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HFrozen);
		you_are(buf);
	}

	if (u.hanguppenalty) {
		Sprintf(buf, "temporarily slower because you tried to hangup cheat");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.hanguppenalty);
		you_are(buf);
	}

	if (Burned) {
		if (HeavyBurned) Sprintf(buf, "badly burned");
		else Sprintf(buf, "burned");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HBurned);
		you_are(buf);
	}
		
#ifdef CONVICT
        if (Punished) {
            you_are("punished");
        }
#endif /* CONVICT */

        if (u.totter) {
            you_have("inverted directional keys");
        }

        if (sengr_at("Elbereth", u.ux, u.uy) ) {
            you_are("standing on an active Elbereth engraving");
        }

	if (Stoned) {
		Sprintf(buf, "turning to stone");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Stoned);
		you_are(buf);
	}
	if (Slimed) {
		Sprintf(buf, "turning into slime");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Slimed);
		you_are(buf);
	}
	if (Strangled) {
		Sprintf(buf, (u.uburied) ? "buried" : "being strangled");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Strangled);
		you_are(buf);
	}
	if (Prem_death) {
		Sprintf(buf, "going to die prematurely");
		you_are(buf);
	}
	if (IsGlib) {
		Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Glib);
		you_have(buf);
	}


	if (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone()) {
		Sprintf(buf, "the menu bug.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MenuBug);
		you_have(buf);
	}
	if (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) {
		Sprintf(buf, "the speed bug.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", SpeedBug);
		you_have(buf);
	}
	if (Superscroller || u.uprops[SUPERSCROLLER_ACTIVE].extrinsic || have_superscrollerstone()) {
		Sprintf(buf, "the superscroller.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Superscroller);
		you_have(buf);
	}
	if (RMBLoss || u.uprops[RMB_LOST].extrinsic || have_rmbstone()) {
		Sprintf(buf, "the following problem: Your right mouse button failed.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RMBLoss);
		you_have(buf);
	}
	if (DisplayLoss || u.uprops[DISPLAY_LOST].extrinsic || have_displaystone()) {
		Sprintf(buf, "the following problem: Your display failed.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", DisplayLoss);
		you_have(buf);
	}
	if (SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) {
		Sprintf(buf, "the following problem: You lost spells.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", SpellLoss);
		you_have(buf);
	}
	if (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone()) {
		Sprintf(buf, "the following problem: Your spells became yellow.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", YellowSpells);
		you_have(buf);
	}
	if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || have_autodestructstone()) {
		Sprintf(buf, "the following problem: An auto destruct mechanism was initiated.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AutoDestruct);
		you_have(buf);
	}
	if (MemoryLoss || u.uprops[MEMORY_LOST].extrinsic || have_memorylossstone()) {
		Sprintf(buf, "the following problem: There is low local memory.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MemoryLoss);
		you_have(buf);
	}
	if (InventoryLoss || u.uprops[INVENTORY_LOST].extrinsic || have_inventorylossstone()) {
		Sprintf(buf, "the following problem: There is not enough memory to create an inventory window.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", InventoryLoss);
		you_have(buf);
	}
	if (BlackNgWalls || u.uprops[BLACK_NG_WALLS].extrinsic || have_blackystone()) {
		Sprintf(buf, "the following problem: Blacky and his NG walls are closing in.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BlackNgWalls);
		you_have(buf);
	}
	if (FreeHandLoss || u.uprops[FREE_HAND_LOST].extrinsic || have_freehandbugstone()) {
		Sprintf(buf, "the following problem: Your free hand is less likely to be free.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FreeHandLoss);
		you_have(buf);
	}
	if (Unidentify || u.uprops[UNIDENTIFY].extrinsic || have_unidentifystone()) {
		Sprintf(buf, "the following problem: Your possessions sometimes unidentify themselves.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Unidentify);
		you_have(buf);
	}
	if (Thirst || u.uprops[THIRST].extrinsic || have_thirststone()) {
		Sprintf(buf, "a strong sense of thirst.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Thirst);
		you_have(buf);
	}
	if (LuckLoss || u.uprops[LUCK_LOSS].extrinsic || have_unluckystone()) {
		Sprintf(buf, "the following problem: You're shitting out of luck (SOL).");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", LuckLoss);
		you_have(buf);
	}
	if (ShadesOfGrey || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone()) {
		Sprintf(buf, "the following problem: Everything displays in various shades of grey.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", ShadesOfGrey);
		you_have(buf);
	}
	if (FaintActive || u.uprops[FAINT_ACTIVE].extrinsic || have_faintingstone()) {
		Sprintf(buf, "the following problem: You randomly faint.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FaintActive);
		you_have(buf);
	}
	if (Itemcursing || u.uprops[ITEMCURSING].extrinsic || have_cursingstone() || have_primecurse() ) {
		Sprintf(buf, "the following problem: Your inventory gradually fills up with cursed items.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Itemcursing);
		you_have(buf);
	}
	if (DifficultyIncreased || u.uprops[DIFFICULTY_INCREASED].extrinsic || have_difficultystone()) {
		Sprintf(buf, "the following problem: The difficulty of the game was arbitrarily increased.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", DifficultyIncreased);
		you_have(buf);
	}
	if (Deafness || u.uprops[DEAFNESS].extrinsic || have_deafnessstone()) {
		Sprintf(buf, "a hearing break.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Deafness);
		you_have(buf);
	}
	if (CasterProblem || u.uprops[CASTER_PROBLEM].extrinsic || have_antimagicstone()) {
		Sprintf(buf, "blood mana.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", CasterProblem);
		you_have(buf);
	}
	if (WeaknessProblem || u.uprops[WEAKNESS_PROBLEM].extrinsic || have_weaknessstone()) {
		Sprintf(buf, "the following problem: Being weak from hunger damages your health.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", WeaknessProblem);
		you_have(buf);
	}
	if (RotThirteen || u.uprops[ROT_THIRTEEN].extrinsic || have_rotthirteenstone()) {
		Sprintf(buf, "the following problem: A rot13 cypher has been activated for lowercase letters.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RotThirteen);
		you_have(buf);
	}
	if (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone()) {
		Sprintf(buf, "the following problem: You cannot move diagonally.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BishopGridbug);
		you_have(buf);
	}
	if (ConfusionProblem || u.uprops[CONFUSION_PROBLEM].extrinsic || have_confusionstone()) {
		Sprintf(buf, "a confusing problem.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", ConfusionProblem);
		you_have(buf);
	}
	if (NoDropProblem || u.uprops[DROP_BUG].extrinsic || have_dropbugstone()) {
		Sprintf(buf, "the following problem: You cannot drop items.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", NoDropProblem);
		you_have(buf);
	}
	if (DSTWProblem || u.uprops[DSTW_BUG].extrinsic || have_dstwstone()) {
		Sprintf(buf, "the following problem: Your potions don't always work.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", DSTWProblem);
		you_have(buf);
	}
	if (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) {
		Sprintf(buf, "the following problem: You can't cure status effects.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", StatusTrapProblem);
		you_have(buf);
	}
	if (AlignmentProblem || u.uprops[ALIGNMENT_FAILURE].extrinsic || have_alignmentstone()) {
		Sprintf(buf, "the following problem: Your maximum alignment will decrease over time.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AlignmentProblem);
		you_have(buf);
	}
	if (StairsProblem || u.uprops[STAIRSTRAP].extrinsic || have_stairstrapstone()) {
		Sprintf(buf, "the following problem: Stairs are always trapped.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", StairsProblem);
		you_have(buf);
	}
	if (UninformationProblem || u.uprops[UNINFORMATION].extrinsic || have_uninformationstone()) {
		Sprintf(buf, "the following problem: The game gives insufficient amounts of information.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", UninformationProblem);
		you_have(buf);
	}

	if (CaptchaProblem || u.uprops[CAPTCHA].extrinsic || have_captchastone()) {
		Sprintf(buf, "the following problem: You sometimes have to solve captchas.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", CaptchaProblem);
		you_have(buf);
	}
	if (FarlookProblem || u.uprops[FARLOOK_BUG].extrinsic || have_farlookstone()) {
		Sprintf(buf, "the following problem: Farlooking peaceful monsters angers them.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FarlookProblem);
		you_have(buf);
	}
	if (RespawnProblem || u.uprops[RESPAWN_BUG].extrinsic || have_respawnstone()) {
		Sprintf(buf, "the following problem: Killing monsters will cause them to respawn somewhere on the level.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RespawnProblem);
		you_have(buf);
	}

	if (BigscriptEffect || u.uprops[BIGSCRIPT].extrinsic || have_bigscriptstone()) {
		Sprintf(buf, "BIGscript.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BigscriptEffect);
		you_have(buf);
	}
	if (BankTrapEffect || u.uprops[BANKBUG].extrinsic || have_bankstone()) {
		Sprintf(buf, "the following problem: Your money will wander into a mysterious bank.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BankTrapEffect);
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (amount stored: %d)", u.bankcashamount);
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (money limit: %d)", u.bankcashlimit);
		you_have(buf);
	} else {
	  if (u.bankcashamount) {
	    if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "the following amount of cash stored in the bank:");
		Sprintf(eos(buf), " %d", u.bankcashamount);
		you_have(buf);
	    }
	  }
	  if (u.bankcashlimit) {
	    if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "the following bank cash limit:");
		Sprintf(eos(buf), " %d", u.bankcashlimit);
		you_have(buf);
	    }
	  }
	}
	if (MapTrapEffect || u.uprops[MAPBUG].extrinsic || have_mapstone()) {
		Sprintf(buf, "the following problem: The map doesn't display correctly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MapTrapEffect);
		you_have(buf);
	}
	if (TechTrapEffect || u.uprops[TECHBUG].extrinsic || have_techniquestone()) {
		Sprintf(buf, "the following problem: Your techniques don't work.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TechTrapEffect);
		you_have(buf);
	}
	if (RecurringDisenchant || u.uprops[RECURRING_DISENCHANT].extrinsic || have_disenchantmentstone()) {
		Sprintf(buf, "the following problem: Your possessions disenchant themselves spontaneously.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RecurringDisenchant);
		you_have(buf);
	}
	if (verisiertEffect || u.uprops[VERISIERTEFFECT].extrinsic || have_verisiertstone()) {
		Sprintf(buf, "the following problem: Monster respawn speeds up rapidly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", verisiertEffect);
		you_have(buf);
	}
	if (ChaosTerrain || u.uprops[CHAOS_TERRAIN].extrinsic || have_chaosterrainstone()) {
		Sprintf(buf, "the following problem: The terrain slowly becomes ever more chaotic.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", ChaosTerrain);
		you_have(buf);
	}
	if (Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone()) {
		Sprintf(buf, "the following problem: You're completely unable to cast spells.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Muteness);
		you_have(buf);
	}
	if (EngravingDoesntWork || u.uprops[ENGRAVINGBUG].extrinsic || have_engravingstone()) {
		Sprintf(buf, "the following problem: Monsters don't respect Elbereth.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", EngravingDoesntWork);
		you_have(buf);
	}
	if (MagicDeviceEffect || u.uprops[MAGIC_DEVICE_BUG].extrinsic || have_magicdevicestone()) {
		Sprintf(buf, "the following problem: Zapping a wand can cause it to explode.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MagicDeviceEffect);
		you_have(buf);
	}
	if (BookTrapEffect || u.uprops[BOOKBUG].extrinsic || have_bookstone()) {
		Sprintf(buf, "the following problem: Reading spellbooks confuses you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BookTrapEffect);
		you_have(buf);
	}
	if (LevelTrapEffect || u.uprops[LEVELBUG].extrinsic || have_levelstone()) {
		Sprintf(buf, "the following problem: Monsters become stronger if many of their species have been generated already.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", LevelTrapEffect);
		you_have(buf);
	}
	if (QuizTrapEffect || u.uprops[QUIZZES].extrinsic || have_quizstone()) {
		Sprintf(buf, "the following problem: You have to partake in the Great NetHack Quiz.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", QuizTrapEffect);
		you_have(buf);
	}

	if (FastMetabolismEffect || u.uprops[FAST_METABOLISM].extrinsic || have_metabolicstone()) {
		Sprintf(buf, "the following problem: Your food consumption is much faster.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FastMetabolismEffect);
		you_have(buf);
	}

	if (NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone()) {
		Sprintf(buf, "the following problem: You cannot teleport at all.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", NoReturnEffect);
		you_have(buf);
	}

	if (AlwaysEgotypeMonsters || u.uprops[ALWAYS_EGOTYPES].extrinsic || have_egostone()) {
		Sprintf(buf, "the following problem: Monsters always spawn with egotypes.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AlwaysEgotypeMonsters);
		you_have(buf);
	}

	if (TimeGoesByFaster || u.uprops[FAST_FORWARD].extrinsic || have_fastforwardstone()) {
		Sprintf(buf, "the following problem: Time goes by faster.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TimeGoesByFaster);
		you_have(buf);
	}

	if (FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) {
		Sprintf(buf, "the following problem: Your food is always rotten.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FoodIsAlwaysRotten);
		you_have(buf);
	}

	if (AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone()) {
		Sprintf(buf, "the following problem: Your skills are deactivated.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AllSkillsUnskilled);
		you_have(buf);
	}

	if (AllStatsAreLower || u.uprops[STATS_LOWERED].extrinsic || have_lowstatstone()) {
		Sprintf(buf, "the following problem: All of your stats are lowered.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AllStatsAreLower);
		you_have(buf);
	}

	if (PlayerCannotTrainSkills || u.uprops[TRAINING_DEACTIVATED].extrinsic || have_trainingstone()) {
		Sprintf(buf, "the following problem: You cannot train skills.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", PlayerCannotTrainSkills);
		you_have(buf);
	}

	if (PlayerCannotExerciseStats || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone()) {
		Sprintf(buf, "the following problem: You cannot exercise your stats.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", PlayerCannotExerciseStats);
		you_have(buf);
	}

	if (MCReduction) {
		Sprintf(buf, "reduced magic cancellation.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MCReduction);
		you_have(buf);
	}

	if (u.uprops[INTRINSIC_LOSS].extrinsic || IntrinsicLossProblem || have_intrinsiclossstone() ) {
		Sprintf(buf, "a case of random intrinsic loss.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", IntrinsicLossProblem);
		you_have(buf);
	}
	if (u.uprops[TRAP_REVEALING].extrinsic) {
		Sprintf(buf, "randomly revealing traps");
		you_are(buf);
	}
	if (u.uprops[BLOOD_LOSS].extrinsic || BloodLossProblem || have_bloodlossstone() ) {
		Sprintf(buf, "bleeding out");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BloodLossProblem);
		you_are(buf);
	}
	if (u.uprops[NASTINESS_EFFECTS].extrinsic || NastinessProblem || have_nastystone() ) {
		Sprintf(buf, "subjected to random nasty trap effects");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", NastinessProblem);
		you_are(buf);
	}
	if (u.uprops[BAD_EFFECTS].extrinsic || BadEffectProblem || have_badeffectstone() ) {
		Sprintf(buf, "subjected to random bad effects");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BadEffectProblem);
		you_are(buf);
	}
	if (u.uprops[AUTOMATIC_TRAP_CREATION].extrinsic || TrapCreationProblem || have_trapcreationstone() ) {
		Sprintf(buf, "more likely to encounter traps");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TrapCreationProblem);
		you_are(buf);
	}
	if (u.uprops[SENTIENT_HIGH_HEELS].extrinsic) {
		Sprintf(buf, "wearing sentient high heels");
		you_are(buf);
	}
	if (u.uprops[REPEATING_VULNERABILITY].extrinsic || AutomaticVulnerabilitiy || have_vulnerabilitystone() ) {
		Sprintf(buf, "the risk of temporarily losing intrinsics");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AutomaticVulnerabilitiy);
		you_have(buf);
	}
	if (u.uprops[TELEPORTING_ITEMS].extrinsic || TeleportingItems || have_itemportstone() ) {
		Sprintf(buf, "teleporting items");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TeleportingItems);
		you_have(buf);
	}

	if (u.uprops[RECURRING_AMNESIA].extrinsic || RecurringAmnesia || have_amnesiastone() ) {
		Sprintf(buf, "going to suffer from amnesia now and then");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RecurringAmnesia);
		you_are(buf);
	}


	if (u.uprops[RANDOM_RUMORS].extrinsic) {
		Sprintf(buf, "going to listen to random rumors");
		you_are(buf);
	}

	if (IncreasedGravity) {
		Sprintf(buf, "increased encumbrance due to a stronger gravity.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", IncreasedGravity);
		you_have(buf);
	}

	if (NoStaircase) {
		Sprintf(buf, "to wait until you can use staircases again.");
	      Sprintf(eos(buf), " (%d)", NoStaircase);
		you_have(buf);
	}

	if (TimeStopped) {
		Sprintf(buf, "stopped the flow of time.");
	      Sprintf(eos(buf), " (%d)", TimeStopped);
		you_have(buf);
	}

	if (UseTheForce) {
		Sprintf(buf, "able to use the force like a true jedi");
		you_are(buf);
	}

	if (u.uprops[SENSORY_DEPRIVATION].extrinsic) {
		Sprintf(buf, "sensory deprivation.");
		you_have(buf);
	}

	if (NoFire_resistance) {
		Sprintf(buf, "prevented from having fire resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FIRE_RES].intrinsic);
		you_are(buf);
	}
	if (NoCold_resistance) {
		Sprintf(buf, "prevented from having cold resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_COLD_RES].intrinsic);
		you_are(buf);
	}
	if (NoSleep_resistance) {
		Sprintf(buf, "prevented from having sleep resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLEEP_RES].intrinsic);
		you_are(buf);
	}
	if (NoDisint_resistance) {
		Sprintf(buf, "prevented from having disintegration resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISINT_RES].intrinsic);
		you_are(buf);
	}
	if (NoShock_resistance) {
		Sprintf(buf, "prevented from having shock resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SHOCK_RES].intrinsic);
		you_are(buf);
	}
	if (NoPoison_resistance) {
		Sprintf(buf, "prevented from having poison resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_POISON_RES].intrinsic);
		you_are(buf);
	}
	if (NoDrain_resistance) {
		Sprintf(buf, "prevented from having drain resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRAIN_RES].intrinsic);
		you_are(buf);
	}
	if (NoSick_resistance) {
		Sprintf(buf, "prevented from having sickness resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICK_RES].intrinsic);
		you_are(buf);
	}
	if (NoAntimagic_resistance) {
		Sprintf(buf, "prevented from having magic resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_ANTIMAGIC].intrinsic);
		you_are(buf);
	}
	if (NoAcid_resistance) {
		Sprintf(buf, "prevented from having acid resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_ACID_RES].intrinsic);
		you_are(buf);
	}
	if (NoStone_resistance) {
		Sprintf(buf, "prevented from having petrification resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONE_RES].intrinsic);
		you_are(buf);
	}
	if (NoFear_resistance) {
		Sprintf(buf, "prevented from having fear resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_RES].intrinsic);
		you_are(buf);
	}
	if (NoSee_invisible) {
		Sprintf(buf, "prevented from having see invisible");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEE_INVIS].intrinsic);
		you_are(buf);
	}
	if (NoTelepat) {
		Sprintf(buf, "prevented from having telepathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPAT].intrinsic);
		you_are(buf);
	}
	if (NoWarning) {
		Sprintf(buf, "prevented from having warning");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_WARNING].intrinsic);
		you_are(buf);
	}
	if (NoSearching) {
		Sprintf(buf, "prevented from having automatic searching");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEARCHING].intrinsic);
		you_are(buf);
	}
	if (NoClairvoyant) {
		Sprintf(buf, "prevented from having clairvoyance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_CLAIRVOYANT].intrinsic);
		you_are(buf);
	}
	if (NoInfravision) {
		Sprintf(buf, "prevented from having infravision");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_INFRAVISION].intrinsic);
		you_are(buf);
	}
	if (NoDetect_monsters) {
		Sprintf(buf, "prevented from having detect monsters");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DETECT_MONSTERS].intrinsic);
		you_are(buf);
	}
	if (NoInvisible) {
		Sprintf(buf, "prevented from having invisibility");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_INVIS].intrinsic);
		you_are(buf);
	}
	if (NoDisplaced) {
		Sprintf(buf, "prevented from having displacement");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISPLACED].intrinsic);
		you_are(buf);
	}
	if (NoStealth) {
		Sprintf(buf, "prevented from having stealth");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STEALTH].intrinsic);
		you_are(buf);
	}
	if (NoJumping) {
		Sprintf(buf, "prevented from having jumping");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_JUMPING].intrinsic);
		you_are(buf);
	}
	if (NoTeleport_control) {
		Sprintf(buf, "prevented from having teleport control");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPORT_CONTROL].intrinsic);
		you_are(buf);
	}
	if (NoFlying) {
		Sprintf(buf, "prevented from having flying");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FLYING].intrinsic);
		you_are(buf);
	}
	if (NoBreathless) {
		Sprintf(buf, "prevented from having magical breathing");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_MAGICAL_BREATHING].intrinsic);
		you_are(buf);
	}
	if (NoPasses_walls) {
		Sprintf(buf, "prevented from having phasing");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_PASSES_WALLS].intrinsic);
		you_are(buf);
	}
	if (NoSlow_digestion) {
		Sprintf(buf, "prevented from having slow digestion");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLOW_DIGESTION].intrinsic);
		you_are(buf);
	}
	if (NoHalf_spell_damage) {
		Sprintf(buf, "prevented from having half spell damage");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_SPDAM].intrinsic);
		you_are(buf);
	}
	if (NoHalf_physical_damage) {
		Sprintf(buf, "prevented from having half physical damage");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_PHDAM].intrinsic);
		you_are(buf);
	}
	if (NoRegeneration) {
		Sprintf(buf, "prevented from having regeneration");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_REGENERATION].intrinsic);
		you_are(buf);
	}
	if (NoEnergy_regeneration) {
		Sprintf(buf, "prevented from having mana regeneration");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_ENERGY_REGENERATION].intrinsic);
		you_are(buf);
	}
	if (NoPolymorph_control) {
		Sprintf(buf, "prevented from having polymorph control");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic);
		you_are(buf);
	}
	if (NoFast) {
		Sprintf(buf, "prevented from having speed");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FAST].intrinsic);
		you_are(buf);
	}
	if (NoReflecting) {
		Sprintf(buf, "prevented from having reflection");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_REFLECTING].intrinsic);
		you_are(buf);
	}
	if (NoKeen_memory) {
		Sprintf(buf, "prevented from having keen memory");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_KEEN_MEMORY].intrinsic);
		you_are(buf);
	}
	if (NoSight_bonus) {
		Sprintf(buf, "prevented from having a sight bonus");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SIGHT_BONUS].intrinsic);
		you_are(buf);
	}
	if (NoVersus_curses) {
		Sprintf(buf, "prevented from having curse resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_VERSUS_CURSES].intrinsic);
		you_are(buf);
	}

	if (NoStun_resist) {
		Sprintf(buf, "prevented from having stun resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUN_RES].intrinsic);
		you_are(buf);
	}
	if (NoConf_resist) {
		Sprintf(buf, "prevented from having confusion resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_CONF_RES].intrinsic);
		you_are(buf);
	}
	if (NoDouble_attack) {
		Sprintf(buf, "prevented from having double attacks");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DOUBLE_ATTACK].intrinsic);
		you_are(buf);
	}
	if (NoQuad_attack) {
		Sprintf(buf, "prevented from having quad attacks");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_QUAD_ATTACK].intrinsic);
		you_are(buf);
	}

	if (NoFree_action) {
		Sprintf(buf, "prevented from having free action");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREE_ACTION].intrinsic);
		you_are(buf);
	}

	if (NoHallu_party) {
		Sprintf(buf, "prevented from hallu partying");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALLU_PARTY].intrinsic);
		you_are(buf);
	}

	if (NoDrunken_boxing) {
		Sprintf(buf, "prevented from drunken boxing");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRUNKEN_BOXING].intrinsic);
		you_are(buf);
	}

	if (NoStunnopathy) {
		Sprintf(buf, "prevented from having stunnopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUNNOPATHY].intrinsic);
		you_are(buf);
	}

	if (NoNumbopathy) {
		Sprintf(buf, "prevented from having numbopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_NUMBOPATHY].intrinsic);
		you_are(buf);
	}

	if (NoFreezopathy) {
		Sprintf(buf, "prevented from having freezopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREEZOPATHY].intrinsic);
		you_are(buf);
	}

	if (NoStoned_chiller) {
		Sprintf(buf, "prevented from being a stoned chiller");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONED_CHILLER].intrinsic);
		you_are(buf);
	}

	if (NoCorrosivity) {
		Sprintf(buf, "prevented from having corrosivity");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_CORROSIVITY].intrinsic);
		you_are(buf);
	}

	if (NoFear_factor) {
		Sprintf(buf, "prevented from having an increased fear factor");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_FACTOR].intrinsic);
		you_are(buf);
	}

	if (NoBurnopathy) {
		Sprintf(buf, "prevented from having burnopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_BURNOPATHY].intrinsic);
		you_are(buf);
	}

	if (NoSickopathy) {
		Sprintf(buf, "prevented from having sickopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICKOPATHY].intrinsic);
		you_are(buf);
	}
	if (NoUseTheForce) {
		Sprintf(buf, "prevented from using the force like a real jedi");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_THE_FORCE].intrinsic);
		you_are(buf);
	}

	int shieldblockrate = 0;

	if (uarms) {

		switch (uarms->otyp) {

		case SMALL_SHIELD:
			shieldblockrate = 20;
			break;
		case ELVEN_SHIELD:
			shieldblockrate = 30;
			if (Race_if(PM_ELF) || Race_if(PM_DROW) || Role_if(PM_ELPH) || Role_if(PM_TWELPH)) shieldblockrate += 5;
			break;
		case URUK_HAI_SHIELD:
			shieldblockrate = 32;
			if (Race_if(PM_ORC)) shieldblockrate += 5;
			break;
		case ORCISH_SHIELD:
		case ORCISH_GUARD_SHIELD:
			shieldblockrate = 28;
			if (Race_if(PM_ORC)) shieldblockrate += 5;
			break;
		case DWARVISH_ROUNDSHIELD:
			shieldblockrate = 34;
			if (Race_if(PM_DWARF)) shieldblockrate += 5;
			if (Role_if(PM_MIDGET)) shieldblockrate += 5;
			break;
		case LARGE_SHIELD:
		case SHIELD:
			shieldblockrate = 35;
			break;
		case STEEL_SHIELD:
			shieldblockrate = 40;
			break;
		case CRYSTAL_SHIELD:
		case RAPIRAPI:
			shieldblockrate = 45;
			break;
		case SHIELD_OF_REFLECTION:
		case SILVER_SHIELD:
		case MIRROR_SHIELD:
			shieldblockrate = 35;
			break;
		case FLAME_SHIELD:
			shieldblockrate = 40;
			break;
		case ICE_SHIELD:
			shieldblockrate = 40;
			break;
		case LIGHTNING_SHIELD:
			shieldblockrate = 40;
			break;
		case VENOM_SHIELD:
			shieldblockrate = 40;
			break;
		case SHIELD_OF_LIGHT:
			shieldblockrate = 40;
			break;
		case SHIELD_OF_MOBILITY:
			shieldblockrate = 40;
			break;

		case GRAY_DRAGON_SCALE_SHIELD:
		case SILVER_DRAGON_SCALE_SHIELD:
		case MERCURIAL_DRAGON_SCALE_SHIELD:
		case SHIMMERING_DRAGON_SCALE_SHIELD:
		case DEEP_DRAGON_SCALE_SHIELD:
		case RED_DRAGON_SCALE_SHIELD:
		case WHITE_DRAGON_SCALE_SHIELD:
		case ORANGE_DRAGON_SCALE_SHIELD:
		case BLACK_DRAGON_SCALE_SHIELD:
		case BLUE_DRAGON_SCALE_SHIELD:
		case GREEN_DRAGON_SCALE_SHIELD:
		case GOLDEN_DRAGON_SCALE_SHIELD:
		case STONE_DRAGON_SCALE_SHIELD:
		case CYAN_DRAGON_SCALE_SHIELD:
		case RAINBOW_DRAGON_SCALE_SHIELD:
		case BLOOD_DRAGON_SCALE_SHIELD:
		case PLAIN_DRAGON_SCALE_SHIELD:
		case SKY_DRAGON_SCALE_SHIELD:
		case WATER_DRAGON_SCALE_SHIELD:
		case YELLOW_DRAGON_SCALE_SHIELD:

			shieldblockrate = 33;
			break;

		default: impossible("Unknown type of shield (%d)", uarms->otyp);

		}

		if (uarms->spe > 0) shieldblockrate += (uarms->spe * 2);

		if (uarms->cursed) shieldblockrate /= 2;
		if (uarms->blessed) shieldblockrate += 5;

		if (uarms->spe < 0) shieldblockrate += (uarms->spe * 2);

		if (shieldblockrate < 0) shieldblockrate = 0;

		Sprintf(buf, "%d%%", shieldblockrate);
		enl_msg("Your chance to block ", "is ", "was ", buf );
	}

	if (Fumbling) enl_msg("You fumble", "", "d", "");
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
		Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HWounded_legs);
		you_have(buf);
	}
#if defined(WIZARD) && defined(STEED) /*randomly tell this to the player outside of wizard mode too --Amy */
	if (Wounded_legs && u.usteed && (wizard || !rn2(10) || final >= 1 )) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    enl_msg(buf, " has", " had", " wounded legs");
	}
#endif
	if (Sleeping) enl_msg("You ", "fall", "fell", " asleep");
	if (Hunger) enl_msg("You hunger", "", "ed", " rapidly");

	if (have_sleepstone()) enl_msg("You ", "are", "were", " very tired");
	if (have_cursedmagicresstone()) enl_msg("You ", "take", "took", " double damage");

	/*** Vision and senses ***/
	if (See_invisible) enl_msg(You_, "see", "saw", " invisible");
	if (Blind_telepat) you_are("telepathic");

	if (Hallu_party) you_are("hallu partying");
	if (Drunken_boxing) you_are("a drunken boxer");
	if (Stunnopathy) you_are("stunnopathic");
	if (Numbopathy) you_are("numbopathic");
	if (Freezopathy) you_are("freezopathic");
	if (Stoned_chiller) you_are("a stoned chiller");
	if (Corrosivity) you_are("extremely corrosive");
	if (Fear_factor) you_have("an increased fear factor");
	if (Burnopathy) you_are("burnopathic");
	if (Sickopathy) you_are("sickopathic");

	if (Warning) you_are("warned");
	if (Warn_of_mon && flags.warntype) {
	    /* [ALI] Add support for undead */
	    int i, nth = 0;
	    unsigned long warntype = flags.warntype;
	    struct { unsigned long mask; const char *str; } warntypes[] = {
		M2_ORC,		"orcs",
		M2_DEMON,	"demons",
		M2_UNDEAD,	"undead",
	    };

	    Sprintf(buf, "aware of the presence of ");
	    for(i = 0; i < SIZE(warntypes); i++)
	    {
		if (warntype & warntypes[i].mask) {
		    warntype &= ~warntypes[i].mask;
		    if (nth) {
			if (warntype)
			    strcat(buf, ", ");
			else
			    strcat(buf, " and ");
		    }
		    else
			nth = 1;
		    strcat(buf, warntypes[i].str);
		}
	    }
	    if (warntype)
	    {
		if (nth)
		    strcat(buf, " and ");
		strcat(buf, something); 
	    }
		you_are(buf);
	}
#if 0	/* ALI - dealt with under Warn_of_mon */
	if (Undead_warning) you_are("warned of undead");
#endif

	if (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) you_are("aware of the presence of topmodels");
	if (Role_if(PM_ACTIVISTOR) && uwep && is_quest_artifact(uwep) ) you_are("aware of the presence of unique monsters");
	if (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING ) you_are("aware of the presence of undead");
	if (uamul && uamul->otyp == AMULET_OF_POISON_WARNING ) you_are("aware of the presence of poisonous monsters");
	if (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING ) you_are("aware of the presence of same-race monsters");
	if (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING ) you_are("aware of the presence of covetous monsters");
	if (Role_if(PM_PALADIN) ) you_are("aware of the presence of demons");
	if (Race_if(PM_VORTEX) ) you_are("aware of the presence of unsolid creatures");
	if (Race_if(PM_VORTEX) ) you_are("aware of the presence of creatures without limbs");
	if (Race_if(PM_CORTEX) ) you_are("aware of the presence of unsolid creatures");
	if (Race_if(PM_CORTEX) ) you_are("aware of the presence of creatures without limbs");
	if (Race_if(PM_LEVITATOR) ) you_are("aware of the presence of flying monsters");
	if (Race_if(PM_RODNEYAN) ) you_are("able to sense monsters possessing coveted objects");
	if (isselfhybrid) you_are("aware of the presence of strong wanderers");
	if (isselfhybrid) you_are("aware of the presence of monsters that are valid polymorph forms for monsters only");

	if (Searching) you_have("automatic searching");

	if (Clairvoyant) {
		Sprintf(buf, "clairvoyant");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HClairvoyant);
		you_are(buf);
	}

	if (Infravision) you_have("infravision");

	if (u.banishmentbeam) you_are("going to be banished");
	if (u.levelporting) you_are("going to be levelported");

	if (u.inertia) {
		Sprintf(buf, "slowed by inertia");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.inertia);
		you_are(buf);
	}

	if (Detect_monsters) {
		Sprintf(buf, "sensing the presence of monsters");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HDetect_monsters);
		you_are(buf);
	}

	if (u.umconf) you_are("going to confuse monsters");

	Sprintf(buf, "%d points of nutrition remaining", u.uhunger); you_have(buf);

	/*** Appearance and behavior ***/
	if (Adornment) {
	    int adorn = 0;

	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		you_are("poorly adorned");
	    else
		you_are("adorned");
	}
	if (Invisible) you_are("invisible");
	else if (Invis) you_are("invisible to others");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis)
	    you_are("visible");
	if (Displaced) you_are("displaced");
	if (Stealth) you_are("stealthy");
	if (Aggravate_monster) enl_msg("You aggravate", "", "d", " monsters");
	if (Conflict) enl_msg("You cause", "", "d", " conflict");

	/*** Transportation ***/
	if (Jumping) you_can("jump");
	if (Teleportation) you_have("teleportitis");
	if (Teleport_control) you_have("teleport control");
	/*if (Lev_at_will) you_are("levitating, at will");
	else if (Levitation) you_are("levitating");*/	/* without control */

	if (Lev_at_will)  {
	    Sprintf(buf, "levitating, at will");
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) Sprintf(eos(buf), " (%d)", HLevitation);
#endif /*same like above --Amy */
	    you_are(buf);
	}
	if (Levitation)  {
	    Sprintf(buf, "levitating");
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) Sprintf(eos(buf), " (%d)", HLevitation);
#endif /*same like above --Amy */
	    you_are(buf);
	}

	else if (Flying) you_can("fly");
	if (Wwalking) you_can("walk on water");
	if (Swimming) you_can("swim");        
	if (Breathless) you_can("survive without air");
	else if (Amphibious) you_can("breathe water");
	if (Passes_walls) you_can("walk through walls");
#ifdef STEED
	/* If you die while dismounting, u.usteed is still set.  Since several
	 * places in the done() sequence depend on u.usteed, just detect this
	 * special case. */
	if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) {
	    Sprintf(buf, "riding %s", y_monnam(u.usteed));
	    you_are(buf);
	}
#endif
	if (u.uswallow) {
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif /*same like above --Amy */
	    you_are(buf);
	} else if (u.ustuck) {
	    Sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by",
		    a_monnam(u.ustuck));
	    you_are(buf);
	}

	/*** Physical attributes ***/
	if (u.uhitinc)
	    you_have(enlght_combatinc("to hit", u.uhitinc, final, buf));
	if (u.udaminc)
	    you_have(enlght_combatinc("damage", u.udaminc, final, buf));
	if (Slow_digestion) you_have("slower digestion");
	if (Keen_memory)
		enl_msg("Your memory ", "is", "was", " keen");
	if (Sight_bonus) enl_msg("Your sight ", "is", "was", " improved");
	if (Versus_curses) you_have("curse resistance");

	if (Stun_resist) you_have("stun resistance");
	if (Conf_resist) you_have("confusion resistance");
	if (Extra_wpn_practice) enl_msg("You ", "can", "could", " train skills and attributes faster");
	if (Death_resistance) you_have("resistance to death rays");
	if (Double_attack) you_have("double attacks");
	if (Quad_attack) you_have("quad attacks");

	if (Half_physical_damage) you_have("physical resistance");
	if (Half_spell_damage) you_have("spell resistance");
	if (Regeneration) enl_msg("You regenerate", "", "d", "");
	if (Energy_regeneration) you_have("mana regeneration");
	if (u.uspellprot || Protection) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;

	    if (prot < 0)
		you_are("ineffectively protected");
	    else
		you_are("protected");
	}
	if (Protection_from_shape_changers)
		you_are("protected from shape changers");
	if (Polymorph) you_have("polymorphitis");
	if (Polymorph_control) you_have("polymorph control");
	if (u.ulycn >= LOW_PM) {
		Strcpy(buf, an(mons[u.ulycn].mname));
		you_are(buf);
	}
	if (Upolyd && !missingnoprotect) {
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form");
	    else Sprintf(buf, "polymorphed into %s", an(youmonst.data->mname));
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1)  Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    you_are(buf);
	}
	if (Unchanging) you_can("not change from your current form");
	if (Fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !have_speedbugstone()) you_are(Very_fast ? "very fast" : "fast");
	if (Fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) ) you_are(Very_fast ? "very slow" : "slow");
	if (!Fast && Very_fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !have_speedbugstone()) you_are("very fast");
	if (!Fast && Very_fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) ) you_are("very slow");
	if (Reflecting) you_have("reflection");
	if (Free_action) you_have("free action");
	if (Fixed_abil) you_have("fixed abilities");
	if (uamul && uamul->otyp == AMULET_VERSUS_STONE)
		enl_msg("You ", "will be", "would have been", " depetrified");
	if (Lifesaved)
		enl_msg("Your life ", "will be", "would have been", " saved");
	if (Second_chance)
  	  enl_msg("You ","will be", "would have been"," given a second chance");
	if (u.twoweap) {
	    if (uwep && uswapwep)
		Sprintf(buf, "wielding two weapons at once");
	    else if (uwep || uswapwep)
		Sprintf(buf, "fighting with a weapon and your %s %s",
			uwep ? "left" : "right", body_part(HAND));
	    else
		Sprintf(buf, "fighting with two %s",
			makeplural(body_part(HAND)));
	    you_are(buf);
	}

	/*** Miscellany ***/
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%slucky",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "");
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) Sprintf(eos(buf), " (%d)", Luck);
#endif /*sometimes show your actual luck too --Amy */
	    you_are(buf);
	}
#ifdef WIZARD
	 else if (wizard || !rn2(10) || final >= 1) enl_msg("Your luck ", "is", "was", " zero");
#endif
	if (u.moreluck > 0) you_have("extra luck");
	else if (u.moreluck < 0) you_have("reduced luck");
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		enl_msg("Bad luck ", "does", "did", " not time out for you");
	    if (ltmp >= 0)
		enl_msg("Good luck ", "does", "did", " not time out for you");
	}

	/* KMH, balance patch -- healthstones affect health */
	if (u.uhealbonus)
	{
		Sprintf(buf, "%s health", u.uhealbonus > 0 ? "extra" :
			"reduced");
#ifdef WIZARD
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uhealbonus);
#endif
		you_have(buf);
	}
#ifdef WIZARD
	 else if (wizard) enl_msg("Your health bonus ", "is", "was", " zero");
#endif

	if (recalc_mana() )
	{
		Sprintf(buf, "%s mana", recalc_mana() > 0 ? "extra" :
			"reduced");
#ifdef WIZARD
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", recalc_mana() );
#endif
		you_have(buf);
	}
#ifdef WIZARD
	 else if (wizard) enl_msg("Your mana bonus ", "is", "was", " zero");
#endif

	if (u.ugangr) {
	    Sprintf(buf, " %sangry with you",
		    u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : "");
#ifdef WIZARD
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.ugangr);
#endif
	    enl_msg(u_gname(), " is", " was", buf);
	} /*else*/
	    /*
	     * We need to suppress this when the game is over, because death
	     * can change the value calculated by can_pray(), potentially
	     * resulting in a false claim that you could have prayed safely.
	     */
	  if (wizard || !wizard) {

	    /* "can [not] safely pray" vs "could [not] have safely prayed" */

	    Sprintf(buf, "%s%ssafely pray%s", u.ugangr ? "not " : can_pray(FALSE) ? "" : "not ",
		    final ? "have " : "", final ? "ed" : "");

	    /*Sprintf(buf, "%ssafely pray", can_pray(FALSE) ? "" : "not ");*/

#ifdef WIZARD
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.ublesscnt);
#endif

	    you_can(buf);




#if 0	/* WAC -- replaced by techniques */
/*	    Sprintf(buf, "%s%suse%s your special", !u.unextuse ? "" : "not ",
		    final ? "have " : "", final ? "d" : "");*/
	    Sprintf(buf, "%suse your special", !u.unextuse ? "" : "not ");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.unextuse);
#endif
	    you_can(buf);
#endif
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* still in progress, or quit/escaped/ascended */
	    p = "survived after being killed ";
	    switch (u.umortality) {
	    case 0:  p = !final ? (char *)0 : "survived";  break;
	    case 1:  Strcpy(buf, "once");  break;
	    case 2:  Strcpy(buf, "twice");  break;
	    case 3:  Strcpy(buf, "thrice");  break;
	    default: Sprintf(buf, "%d times", u.umortality);
		     break;
	    }
	} else {		/* game ended in character's death */
	    p = "are dead";
	    switch (u.umortality) {
	    case 0:  impossible("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, " (%d%s time!)", u.umortality,
			     ordin(u.umortality));
		     break;
	    }
	}
	if (p) enl_msg(You_, "have been killed ", p, buf);
    }

	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}

const char * const encx_stat[] = {
	"",
	"burdened",
	"stressed",
	"strained",
	"overtaxed",
	"overloaded"
};

const char *hux_stat[] = {
	"satiated",
	"        ",
	"hungry  ",
	"weak    ",
	"fainting",
	"fainted ",
	"starved "
};

#ifdef DUMP_LOG
void
dump_enlightenment(final)
int final;
{
	int ltmp;
	struct permonst *ptr;
	char buf[BUFSZ];
	char buf2[BUFSZ];
	const char *enc_stat[] = { /* copied from botl.c */
	     "",
	     "burdened",
	     "stressed",
	     "strained",
	     "overtaxed",
	     "overloaded"
	};
	char *youwere = "  You were ";
	char *youhave = "  You have ";
	char *youhad  = "  You had ";
	char *youcould = "  You could ";

	dump("", "Final attributes");

#ifdef ELBERETH
	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
				"the Hand of Elbereth",
				"the Envoy of Balance",
				"the Glory of Arioch"
	    };
	    dump(youwere,
		(char *)hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
#endif

	if (u.ualign.record >= 20)
		dump(youwere, "piously aligned");
	else if (u.ualign.record > 13)
	    dump(youwere, "devoutly aligned");
	else if (u.ualign.record > 8)
	    dump(youwere, "fervently aligned");
	else if (u.ualign.record > 3)
	    dump(youwere, "stridently aligned");
	else if (u.ualign.record == 3)
	    dump(youwere, "aligned");
	else if (u.ualign.record > 0)
	    dump(youwere, "haltingly aligned");
	else if (u.ualign.record == 0)
	    dump(youwere, "nominally aligned");
	else if (u.ualign.record >= -3)	dump(youhave, "strayed");
	else if (u.ualign.record >= -8)	dump(youhave, "sinned");
	else dump("  You have ", "transgressed");

	Sprintf(buf, " %d", u.ualign.record);
	dump("  Your alignment was ", buf);

	Sprintf(buf, " %d", u.alignlim);
	dump("  Your max alignment was ", buf);

	Sprintf(buf, " %d sins", u.ualign.sins);
	dump("  You carried ", buf);

	if (u.idscrollpenalty > 100) {
		Sprintf(buf, " %d", u.idscrollpenalty);
		dump("  Scroll identification only worked 100 out of X attempts, and X was", buf);
	}

	if (u.idpotionpenalty > 3) {
		Sprintf(buf, " %d", u.idpotionpenalty);
		dump("  Potion identification only worked 3 out of X attempts, and X was", buf);
	}

	if (u.idringpenalty > 4) {
		Sprintf(buf, " %d", u.idringpenalty);
		dump("  Ring identification only worked 4 out of X attempts, and X was", buf);
	}

	if (u.idamuletpenalty > 15) {
		Sprintf(buf, " %d", u.idamuletpenalty);
		dump("  Amulet identification only works 15 out of X attempts, and X was", buf);
	}

	if (u.idwandpenalty > 3) {
		Sprintf(buf, " %d", u.idwandpenalty);
		dump("  Wand identification only works 3 out of X attempts, and X was", buf);
	}

	Sprintf(buf, " %d", nartifact_exist());
	dump("  Number of artifacts generated was", buf);

	if (u.legscratching > 1) {
		Sprintf(buf, " %d", u.legscratching - 1);
		dump("  Your leg damage was", buf);
	}

	if (u.youpolyamount > 0) {
		Sprintf(buf, " %d", u.youpolyamount);
		dump("  Your remaining number of free polymorphs was", buf); 
	}

	Sprintf(buf, " turn %d", u.monstertimeout);  
	dump("  Monster spawn increase would have started at", buf);
	dump("  In this game, Eevee's evolution was", mons[u.eeveelution].mname);   /* these two are tied together because the monstertimefinish variable defines the evolution --Amy */

	dump("  The monster class that could not be genocided was", monexplain[u.ungenocidable]);

	if (u.alwaysinvisible) {
		dump("  Yesterday, the monster class that always got generated invisible was", monexplain[u.alwaysinvisible]);
        }

	Sprintf(buf, "generated more often: %s (frequency bonus %d)", monexplain[u.frequentmonster], u.freqmonsterbonus);
	dump("  The monster class that was", buf );

	if (u.freqcolorbonus) {
		Sprintf(buf, "generated more often: %s (frequency bonus %d)", c_obj_colors[u.frequentcolor], u.freqcolorbonus);
		dump("  The monster color that was", buf );
       }

	ptr = &mons[u.frequentspecies];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus);
	dump("  The monster species that was", buf );

	ptr = &mons[u.frequentspecies2];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus2);
	dump("  The monster species that was", buf );

	ptr = &mons[u.frequentspecies3];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus3);
	dump("  The monster species that was", buf );

	ptr = &mons[u.frequentspecies4];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus4);
	dump("  The monster species that was", buf );

	ptr = &mons[u.frequentspecies5];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus5);
	dump("  The monster species that was", buf );

	ptr = &mons[u.frequentspecies6];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus6);
	dump("  The monster species that was", buf );

	ptr = &mons[u.frequentspecies7];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus7);
	dump("  The monster species that was", buf );

	ptr = &mons[u.frequentspecies8];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus8);
	dump("  The monster species that was", buf );

	ptr = &mons[u.frequentspecies9];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus9);
	dump("  The monster species that was", buf );

	ptr = &mons[u.frequentspecies10];
	Sprintf(buf, "generated more often: %s (frequency bonus %d)", ptr->mname, u.freqspeciesbonus10);
	dump("  The monster species that was", buf );

	ptr = &mons[u.nospawnspecies];
	Sprintf(buf, "never randomly generated: %s", ptr->mname);
	dump("  The monster species that was", buf );

	ptr = &mons[u.nospawnspecies2];
	Sprintf(buf, "never randomly generated: %s", ptr->mname);
	dump("  The monster species that was", buf );

	ptr = &mons[u.nospawnspecies3];
	Sprintf(buf, "never randomly generated: %s", ptr->mname);
	dump("  The monster species that was", buf );

	ptr = &mons[u.nospawnspecies4];
	Sprintf(buf, "never randomly generated: %s", ptr->mname);
	dump("  The monster species that was", buf );

	ptr = &mons[u.nospawnspecies5];
	Sprintf(buf, "never randomly generated: %s", ptr->mname);
	dump("  The monster species that was", buf );

	if (u.speedymonster) {
		Sprintf(buf, "always generated with extra speed: %s", monexplain[u.speedymonster]);
		dump("  The monster class that was", buf );
	}

	if (u.musemonster) {
		Sprintf(buf, "more likely to generate with musable stuff: %s", monexplain[u.musemonster]);
		dump("  The monster class that was", buf );
	}

	if (u.minimalism) {
		Sprintf(buf, " %d", u.minimalism);
		dump("  Items generated only 1 time in X, and X was", buf);
	}

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable].oc_name, obj_descr[u.unobtainable].oc_descr);
	dump("  The RNG hath decreed that this item was", buf );

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable2].oc_name, obj_descr[u.unobtainable2].oc_descr);
	dump("  The RNG hath decreed that this item was", buf );   

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable3].oc_name, obj_descr[u.unobtainable3].oc_descr);
	dump("  The RNG hath decreed that this item was", buf );   

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable4].oc_name, obj_descr[u.unobtainable4].oc_descr);
	dump("  The RNG hath decreed that this item was", buf );

	if (u.unobtainablegeno != -1) {
		Sprintf(buf, "genocided: %s (%s)", obj_descr[u.unobtainablegeno].oc_name, obj_descr[u.unobtainablegeno].oc_descr);
		dump("  This item was", buf );
	}

	Sprintf(buf, "generated more often: %s (%s, frequency bonus %d)", obj_descr[u.veryobtainable].oc_name, obj_descr[u.veryobtainable].oc_descr, u.veryobtainableboost);
	dump("  The RNG hath decreed that this item was", buf );

	Sprintf(buf, "generated more often: %s (%s, frequency bonus %d)", obj_descr[u.veryobtainable2].oc_name, obj_descr[u.veryobtainable2].oc_descr, u.veryobtainableboost2);
	dump("  The RNG hath decreed that this item was", buf );

	Sprintf(buf, "generated more often: %s (%s, frequency bonus %d)", obj_descr[u.veryobtainable3].oc_name, obj_descr[u.veryobtainable3].oc_descr, u.veryobtainableboost3);
	dump("  The RNG hath decreed that this item was", buf );   

	Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem].oc_name, obj_descr[u.alwayscurseditem].oc_descr);
	dump("  The RNG hath decreed that this item was", buf );   

	Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem2].oc_name, obj_descr[u.alwayscurseditem2].oc_descr);
	dump("  The RNG hath decreed that this item was", buf );

	Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem3].oc_name, obj_descr[u.alwayscurseditem3].oc_descr);
	dump("  The RNG hath decreed that this item was", buf );   

	Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem4].oc_name, obj_descr[u.alwayscurseditem4].oc_descr);
	dump("  The RNG hath decreed that this item was", buf );   

	Sprintf(buf, "always invisible: %s", defsyms[trap_to_defsym(u.invisotrap)].explanation);
	dump("  The RNG hath decreed that this trap was", buf );   

	Sprintf(buf, "generated more often: %s (frequency bonus %d)", defsyms[trap_to_defsym(u.frequenttrap)].explanation, u.freqtrapbonus);
	dump("  The RNG hath decreed that this trap was", buf );

	Sprintf(buf, " turn %d", u.next_check);
	dump("  Next attribute increase check would have come at", buf);

	Sprintf(buf, " %d", AEXE(A_STR));
	dump("  Strength training was", buf);

	Sprintf(buf, " %d", AEXE(A_DEX));
	dump("  Dexterity training was", buf);

	Sprintf(buf, " %d", AEXE(A_WIS));
	dump("  Wisdom training was", buf);

	Sprintf(buf, " %d", AEXE(A_CON));
	dump("  Constitution training was", buf);


	if (u.negativeprotection) {
		Sprintf(buf, " %d", u.negativeprotection);
		dump("  Your protection was reduced. The amount was", buf);  
        }

        if (u.chokhmahdamage) {
		Sprintf(buf, " %d", u.chokhmahdamage);
                dump("  The escalating chokhmah attack damage was", buf);
	}


	/*** Resistances to troubles ***/
	if (Fire_resistance) dump(youwere, "fire resistant");
	if (Cold_resistance) dump(youwere, "cold resistant");
	if (Sleep_resistance) dump(youwere, "sleep resistant");
	if (Disint_resistance) dump(youwere, "disintegration-resistant");
	if (Shock_resistance) dump(youwere, "shock resistant");
	if (Poison_resistance) dump(youwere, "poison resistant");
	if (Drain_resistance) dump(youwere, "level-drain resistant");
	if (Sick_resistance) dump(youwere, "immune to sickness");
	if (Antimagic) dump(youwere, "magic-protected");
	if (Acid_resistance) dump(youwere, "acid resistant");
	if (Stone_resistance) dump(youwere, "petrification resistant");
	if (Invulnerable) dump(youwere, "invulnerable");
	if (u.uedibility) dump(youcould, "recognize detrimental food");

	/*** Troubles ***/
	if (Halluc_resistance) 	dump("  ", "You resisted hallucinations");
	if (Hallucination) dump(youwere, "hallucinating");
	if (Stunned) dump(youwere, "stunned");
	if (Confusion) dump(youwere, "confused");
	if (Blinded) dump(youwere, "blinded");
	if (Sick) {
		if (u.usick_type & SICK_VOMITABLE)
			dump(youwere, "sick from food poisoning");
		if (u.usick_type & SICK_NONVOMITABLE)
			dump(youwere, "sick from illness");
	}
	if (Stoned) dump(youwere, "turning to stone");
	if (Slimed) dump(youwere, "turning into slime");
	if (Strangled)
		dump(youwere, (u.uburied) ? "buried" : "being strangled");
	if (Glib) {
		Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
		dump(youhad, buf);
	}
	if (Fumbling) dump("  ", "You fumbled");
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
		Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
		dump(youhad, buf);
	}
#ifdef STEED
	if (Wounded_legs && u.usteed) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    Strcat(buf, " had wounded legs");
	    dump("  ", buf);
	}
#endif
	if (Sleeping) dump("  ", "You fell asleep");
	if (Hunger) dump("  ", "You hungered rapidly");

	/*** Vision and senses ***/
	if (See_invisible) dump("  ", "You saw invisible");
	if (Blind_telepat) dump(youwere, "telepathic");
	if (Warning) dump(youwere, "warned");
	if (Warn_of_mon && flags.warntype) {
		Sprintf(buf, "aware of the presence of %s",
			(flags.warntype & M2_ORC) ? "orcs" :
			(flags.warntype & M2_DEMON) ? "demons" :
			something); 
		dump(youwere, buf);
	}
	if (Undead_warning) dump(youwere, "warned of undead");
	if (Searching) dump(youhad, "automatic searching");
	if (Clairvoyant) dump(youwere, "clairvoyant");
	if (Infravision) dump(youhad, "infravision");
	if (Detect_monsters)
	  dump(youwere, "sensing the presence of monsters");
	if (u.umconf) dump(youwere, "going to confuse monsters");

	/*** Appearance and behavior ***/
	if (Adornment) {
	    int adorn = 0;
	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		dump(youwere, "poorly adorned");
	    else
		dump(youwere, "adorned");
	}
	if (Invisible) dump(youwere, "invisible");
	else if (Invis) dump(youwere, "invisible to others");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis)
	    dump(youwere, "visible");
	if (Displaced) dump(youwere, "displaced");
	if (Stealth) dump(youwere, "stealthy");
	if (Aggravate_monster) dump("  ", "You aggravated monsters");
	if (Conflict) dump("  ", "You caused conflict");

	/*** Transportation ***/
	if (Jumping) dump(youcould, "jump");
	if (Teleportation) dump(youcould, "teleport");
	if (Teleport_control) dump(youhad, "teleport control");
	if (Lev_at_will) dump(youwere, "levitating, at will");
	else if (Levitation)
	  dump(youwere, "levitating");	/* without control */
	else if (Flying) dump(youcould, "fly");
	if (Wwalking) dump(youcould, "walk on water");
	if (Swimming) dump(youcould, "swim");
	if (Breathless) dump(youcould, "survive without air");
	else if (Amphibious) dump(youcould, "breathe water");
	if (Passes_walls) dump(youcould, "walk through walls");
#ifdef STEED
	if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) {
	    Sprintf(buf, "riding %s", y_monnam(u.usteed));
	    dump(youwere, buf);
	}
#endif
	if (u.uswallow) {
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif
	    dump(youwere, buf);
	} else if (u.ustuck) {
	    Sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by",
		    a_monnam(u.ustuck));
	    dump(youwere, buf);
	}

	/*** Physical attributes ***/
	if (u.uhitinc)
	    dump(youhad,
		enlght_combatinc("to hit", u.uhitinc, final, buf));
	if (u.udaminc)
	    dump(youhad,
		enlght_combatinc("damage", u.udaminc, final, buf));
	if (Slow_digestion) dump(youhad, "slower digestion");
	if (Regeneration) dump("  ", "You regenerated");
	if (u.uspellprot || Protection) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;
	    
	    if (prot < 0)
		dump(youwere, "ineffectively protected");
	    else
		dump(youwere, "protected");
	}
	if (Protection_from_shape_changers)
		dump(youwere, "protected from shape changers");
	if (Polymorph) dump(youwere, "polymorphing");
	if (Polymorph_control) dump(youhad, "polymorph control");
	if (u.ulycn >= LOW_PM) {
		Strcpy(buf, an(mons[u.ulycn].mname));
		dump(youwere, buf);
	}
	if (Upolyd) {
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form");
	    else Sprintf(buf, "polymorphed into %s",
			 an(youmonst.data->mname));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    dump(youwere, buf);
	}
	if (Unchanging)
	  dump(youcould, "not change from your current form");
	if (Fast) dump(youwere, Very_fast ? "very fast" : "fast");
	if (Reflecting) dump(youhad, "reflection");
	if (Free_action) dump(youhad, "free action");
	if (Fixed_abil) dump(youhad, "fixed abilities");
	if (Lifesaved)
		dump("  ", "Your life would have been saved");
	if (u.twoweap) dump(youwere, "wielding two weapons at once");

	/*** Miscellany ***/
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%slucky (%d)",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "", Luck);
	    dump(youwere, buf);
	}
#ifdef WIZARD
	 else if (wizard) dump("  ", "Your luck was zero");
#endif
	if (u.moreluck > 0) dump(youhad, "extra luck");
	else if (u.moreluck < 0) dump(youhad, "reduced luck");
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		dump("  ", "Bad luck did not time out for you");
	    if (ltmp >= 0)
		dump("  ", "Good luck did not time out for you");
	}

	if (u.ugangr) {
	    Sprintf(buf, " %sangry with you",
		u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ugangr);
#endif
	    Sprintf(buf2, "%s was %s", u_gname(), buf);
	    dump("  ", buf2);
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* quit/escaped/ascended */
	    p = "survived after being killed ";
	    switch (u.umortality) {
	    case 0:  p = "survived";  break;
	    case 1:  Strcpy(buf, "once");  break;
	    case 2:  Strcpy(buf, "twice");  break;
	    case 3:  Strcpy(buf, "thrice");  break;
	    default: Sprintf(buf, "%d times", u.umortality);
		     break;
	    }
	} else {		/* game ended in character's death */
	    p = "are dead";
	    switch (u.umortality) {
	    case 0:  impossible("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, " (%d%s time!)", u.umortality,
			     ordin(u.umortality));
		     break;
	    }
	}
	if (p) {
	  Sprintf(buf2, "You %s %s", p, buf);
	  dump("  ", buf2);
	}
    }
	dump("", "");
	return;

} /* dump_enlightenment */
#endif

/*
 * Courtesy function for non-debug, non-explorer mode players
 * to help refresh them about who/what they are.
 * Returns FALSE if menu cancelled (dismissed with ESC), TRUE otherwise.
 */
STATIC_OVL boolean
minimal_enlightenment()
{

	char statline[BUFSZ];

	*statline = '\0';

	if (!DisplayLoss && !u.uprops[DISPLAY_LOST].extrinsic && !have_displaystone() ) {
		/* Yes I know, this is far from optimized. But it's a crutch for terminals with
		 * less than 25 lines, where bot2() doesn't display everything if you have lots of status effects. --Amy */

		Sprintf(eos(statline), "You are %s, a %s %s %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s %s.", plname, align_str(u.ualign.type), (flags.female ? "female" : "male"), (flags.hybridangbander ? "angbander " : ""), (flags.hybridaquarian ? "aquarian " : ""), (flags.hybridcurser ? "curser " : ""), (flags.hybridhaxor ? "haxor " : ""), (flags.hybridhomicider ? "homicider " : ""), (flags.hybridsuxxor ? "suxxor " : ""), (flags.hybridwarper ? "warper " : ""), (flags.hybridrandomizer ? "randomizer " : ""), (flags.hybridnullrace ? "null " : ""), (flags.hybridmazewalker ? "mazewalker " : ""), (flags.hybridsoviet ? "soviet " : ""), (flags.hybridxrace ? "x-race " : ""), (flags.hybridheretic ? "heretic " : ""), (flags.hybridsokosolver ? "sokosolver " : ""), (flags.hybridspecialist ? "specialist " : ""), (flags.hybridamerican ? "american " : ""), (flags.hybridminimalist ? "minimalist " : ""), (flags.hybridnastinator ? "nastinator " : ""), urace.adj, (flags.female && urole.name.f) ? urole.name.f : urole.name.m);

		if (!Upolyd) Sprintf(eos(statline), " HP: %d (max %d)", u.uhp, u.uhpmax);
		else Sprintf(eos(statline), " HP: %d (max %d)", u.mh, u.mhmax);

		Sprintf(eos(statline), " Pw %d (max %d)", u.uen, u.uenmax);

		Sprintf(eos(statline), " AC %d", u.uac);

		Sprintf(eos(statline), " Current status effects: ");

		if (Levitation) Sprintf(eos(statline), "levitation, ");
		if (HeavyConfusion) Sprintf(eos(statline), "xtraconfusion, ");
		else if (Confusion) Sprintf(eos(statline), "confusion, ");
		if (Sick) {
			if (u.usick_type & SICK_VOMITABLE) Sprintf(eos(statline), "food poisoning, ");
			if (u.usick_type & SICK_NONVOMITABLE) Sprintf(eos(statline), "illness, ");
		}
		if (HeavyBlind) Sprintf(eos(statline), "xtrablindness, ");
		else if (Blind) Sprintf(eos(statline), "blindness, ");
		if(sengr_at("Elbereth", u.ux, u.uy)) Sprintf(eos(statline), "elbereth, ");
		if (HeavyFeared) Sprintf(eos(statline), "xtrafear, ");
		else if (Feared) Sprintf(eos(statline), "fear, ");
		if (HeavyNumbed) Sprintf(eos(statline), "xtranumbness, ");
		else if (Numbed) Sprintf(eos(statline), "numbness, ");
		if (HeavyFrozen) Sprintf(eos(statline), "xtrafreeze, ");
		else if (Frozen) Sprintf(eos(statline), "freeze, ");
		if (HeavyBurned) Sprintf(eos(statline), "xtraburn, ");
		else if (Burned) Sprintf(eos(statline), "burn, ");
		if (HeavyStunned) Sprintf(eos(statline), "xtrastun, ");
		else if (Stunned) Sprintf(eos(statline), "stun, ");
		if (HeavyHallu) Sprintf(eos(statline), "xtrahallucination, ");
		else if (Hallucination) Sprintf(eos(statline), "hallucination, ");
		if (Slimed) Sprintf(eos(statline), "sliming, ");
		if (Stoned) Sprintf(eos(statline), "petrification, ");
		if (IsGlib) Sprintf(eos(statline), "glib, ");
		if (Wounded_legs) Sprintf(eos(statline), "wounded legs, ");
		if (Strangled) Sprintf(eos(statline), "strangled, ");
		if (Vomiting) Sprintf(eos(statline), "vomiting, ");
		if(u.ustuck && !u.uswallow && !sticks(youmonst.data)) Sprintf(eos(statline), "held by a monster, ");
		if(near_capacity() > UNENCUMBERED) Sprintf(eos(statline), "%s, ", encx_stat[near_capacity()]);
		if (!Thirst && !u.uprops[THIRST].extrinsic && !have_thirststone() && u.urealedibility && u.uhunger >= 4500) Sprintf(eos(statline), "oversatiated, ");
		else if(!Thirst && !u.uprops[THIRST].extrinsic && !have_thirststone() && strcmp(hux_stat[u.uhs], "        ")) Sprintf(eos(statline), "%s, ", hux_stat[u.uhs]);

		Sprintf(eos(statline), ".");

		pline(statline);

	/*"You are %s, a %s %s %s%s%s%s%s%s%s%s %s. Current status effects: %s%s%s%s.", , Levitation ? "levitation " : "", Confusion ? (HeavyConfusion ? "xtraconfusion " : "confusion ") : "", );
*/
	}

	winid tmpwin;
	menu_item *selected;
	anything any;
	int genidx, n;
	char buf[BUFSZ], buf2[BUFSZ];
	static const char untabbed_fmtstr[] = "%-15s: %-12s";
	static const char untabbed_deity_fmtstr[] = "%-17s%s";
	static const char tabbed_fmtstr[] = "%s:\t%-12s";
	static const char tabbed_deity_fmtstr[] = "%s\t%s";
	static const char *fmtstr;
	static const char *deity_fmtstr;

	fmtstr = iflags.menu_tab_sep ? tabbed_fmtstr : untabbed_fmtstr;
	deity_fmtstr = iflags.menu_tab_sep ?
			tabbed_deity_fmtstr : untabbed_deity_fmtstr; 
	any.a_void = 0;
	buf[0] = buf2[0] = '\0';
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Starting", FALSE);

	/* Starting name, race, role, gender */
	Sprintf(buf, fmtstr, "name", plname);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, "race", ustartrace.noun);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, "role",
		(flags.initgend && ustartrole.name.f) ? ustartrole.name.f : ustartrole.name.m);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, "gender", genders[flags.initgend].adj);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Starting alignment */
	Sprintf(buf, fmtstr, "alignment", align_str(u.ualignbase[A_ORIGINAL]));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Hybridization (if any) */
	if (flags.hybridization) {

	Sprintf(buf, fmtstr, "hybrid races", hybrid_str());
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	}

	/* Current name, race, role, gender */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Current", FALSE);
	Sprintf(buf, fmtstr, "race", (Upolyd && !missingnoprotect) ? youmonst.data->mname : urace.noun);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd) {
	    Sprintf(buf, fmtstr, "role (base)",
		(u.mfemale && urole.name.f) ? urole.name.f : urole.name.m);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	} else {
	    Sprintf(buf, fmtstr, "role",
		(flags.female && urole.name.f) ? urole.name.f : urole.name.m);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}
	/* don't want poly_gender() here; it forces `2' for non-humanoids */
	genidx = is_neuter(youmonst.data) ? 2 : flags.female;
	Sprintf(buf, fmtstr, "gender", genders[genidx].adj);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd && (int)u.mfemale != genidx) {
	    Sprintf(buf, fmtstr, "gender (base)", genders[u.mfemale].adj);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}

	/* Current alignment */
	Sprintf(buf, fmtstr, "alignment", align_str(u.ualign.type));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Deity list */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Deities", FALSE);
	Sprintf(buf2, deity_fmtstr, align_gname(A_CHAOTIC),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_CHAOTIC) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_CHAOTIC)       ? " (s)" :
	    (u.ualign.type   == A_CHAOTIC)       ? " (c)" : "");
	Sprintf(buf, fmtstr, "Chaotic", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_NEUTRAL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_NEUTRAL) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_NEUTRAL)       ? " (s)" :
	    (u.ualign.type   == A_NEUTRAL)       ? " (c)" : "");
	Sprintf(buf, fmtstr, "Neutral", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_LAWFUL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type &&
		u.ualign.type == A_LAWFUL)  ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_LAWFUL)        ? " (s)" :
	    (u.ualign.type   == A_LAWFUL)        ? " (c)" : "");
	Sprintf(buf, fmtstr, "Lawful", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	end_menu(tmpwin, "Base Attributes");
	n = select_menu(tmpwin, PICK_NONE, &selected);
	destroy_nhwindow(tmpwin);
	return (n != -1);
}

STATIC_PTR int
doattributes()
{
	if (!minimal_enlightenment())
		return 0;
	if (wizard || discover)
		enlightenment(0);
	return 0;
}

static const struct menu_tab game_menu[] = {
	{'O', TRUE, doset, "Options"},
	{'r', TRUE, doredraw, "Redraw Screen"},
#ifndef PUBLIC_SERVER
	{'x', TRUE, enter_explore_mode, "Enter Explore Mode"},
#endif
#ifdef SHELL
	{'!', TRUE, dosh, "Jump to Shell"},
#endif
	{'S', TRUE, dosave, "Save"},
	{'q', TRUE, done2, "Quit [M-q]"},
	{0,0,0,0},
};

static const struct menu_tab inv_menu[] = {
	{(char)0, TRUE, (void *)0, "View Inventory"},
	{'i', TRUE, ddoinv, "Inventory List"},
	{'I', TRUE, dotypeinv, "Inventory List by Type"},
	{'*', TRUE, doprinuse, "Items in use"},
	{(char)0, TRUE, (void *)0, "Ready Items"},
	{'w', FALSE, dowield, "Wield Weapon"},
	{'W', FALSE, dowear, "Wear Protective Gear"},
	{'Q', FALSE, dowieldquiver, "Prepare missile weapon (in Quiver)"},
	{'T', FALSE, dotakeoff, "Take off Protective Gear"},
	{(char)0, TRUE, (void *)0, "Manipulate Items"},
	{'a', FALSE, doapply, "Apply an object"},
	{'d', FALSE, dodip, "Dip an object [M-d]"},
	{'E', FALSE, doengrave, "Engrave into the ground"},
	{'f', FALSE, dofire, "Fire your prepared missile weapon"},
	{'i', TRUE, doinvoke, "Invoke your weapon"},
	{'t', FALSE, dothrow, "Throw an item"},
	{(char)0, TRUE, (void *)0, "Drop Items"},
	{'d', FALSE, dodrop, "Drop an object"},
	{'D', FALSE, doddrop, "Multi-Drop"},
	{0,0,0,0}
};

static const struct menu_tab action_menu[] = {
	{'c', FALSE, doclose, "Close a door"},
	{'e', FALSE, doeat, "Eat some food"},
	{'f', FALSE, doforce, "Force a lock [M-f]"},
	{'l', FALSE, doloot, "Loot an object"},
	{'o', FALSE, doopen, "Open a door"},
	{'q', TRUE, dodrink, "Quaff a potion"},
	{'r', FALSE, doread, "Read an object"},
	{'u', FALSE, dountrap, "Untrap"}, 
	{'z', FALSE, dozap, "Zap a wand"},
	{'Z', TRUE, docast, "Cast a spell"},
	{0,0,0,0}
};

static const struct menu_tab player_menu[] = {
	{'b', FALSE, playersteal, "Steal from Monsters [M-b]"},
	{'c', TRUE, dotalk, "Chat with Monsters [M-c]"},
	{'d', FALSE, dokick, "Do Kick"}, 
	/*        {'e', FALSE, specialpower, "Use your Class Ability [M-e]"},*/
	{'e', TRUE, enhance_weapon_skill, "Weapon Skills [M-k]"},
	{'m', TRUE, domonability, "Use your Monster Ability [M-m]"},
	{'o', FALSE, dosacrifice, "Offer a Sacrifice [M-o]"},
	{'p', FALSE, dopay, "Pay the Shopkeeper"},
	{'s', FALSE, dosit, "Sit down [M-s]"},
	{'t', TRUE, dotele, "Controlled Teleport [C-t]"},
/*	{'T', TRUE, doturn, "Turn Undead [M-t]"},*/
	{'T', TRUE, dotech, "Use Techniques [M-t]"},
	{'U', TRUE, doremoveimarkers, "Remove 'I' markers [M-u]"},
	{'x', TRUE, doattributes, "Show attributes"},
	{'y', TRUE, polyatwill, "Self-Polymorph [M-y]"},
	{0,0,0,0}
};

#ifdef WIZARD
static const struct menu_tab wizard_menu[] = {
	{'c', TRUE, wiz_gain_ac, "Increase AC"},
#ifdef DISPLAY_LAYERS
	{'d', TRUE, wiz_show_display, "Detail display layers"},
#endif
	{'e', TRUE, wiz_detect, "Detect secret doors and traps"},
	{'f', TRUE, wiz_map, "Do magic mapping"},
	{'g', TRUE, wiz_genesis, "Create monster"},
	{'i', TRUE, wiz_identify, "Identify items in pack"},
	{'j', TRUE, wiz_gain_level, "Go up an experience level"},
	{'n', TRUE, wiz_toggle_invulnerability, "Toggle invulnerability"},
	{'o', TRUE, wiz_where, "Tell locations of special levels"},
	{'v', TRUE, wiz_level_tele, "Do trans-level teleport"},
	{'w', TRUE, wiz_wish,  "Make wish"},
	{'H', TRUE, wiz_detect_monsters, "Detect monsters"},
	{'L', TRUE, wiz_light_sources, "show mobile light sources"},
	{'M', TRUE, wiz_show_stats, "show memory statistics"},
	{'S', TRUE, wiz_show_seenv, "show seen vectors"},
	{'T', TRUE, wiz_timeout_queue, "look at timeout queue"},
	{'V', TRUE, wiz_show_vision, "show vision array"},
	{'W', TRUE, wiz_show_wmodes, "show wall modes"},
#ifdef DEBUG
	{'&', TRUE, wiz_debug_cmd, "wizard debug command"},
#endif
	{0,0,0,0,0},
};
#endif

static const struct menu_tab help_menu[] = {
	{'?', TRUE, dohelp, "Help Contents"},
	{'v', TRUE, doextversion, "Version"},
	{'/', TRUE, dowhatis, "Identify an object on the screen" },
	{'&', TRUE, dowhatdoes, "Determine what a key does"},
	{0,0,0,0,0},
};

static const struct menu_tab main_menu[] = {
	{'g', TRUE, (void *)0, "Game"},
	{'i', TRUE, (void *)0, "Inventory"},
	{'a', TRUE, (void *)0, "Action"},
	{'p', TRUE, (void *)0, "Player"},
	{'d', TRUE, (void *)0, "Discoveries"},
#ifdef WIZARD
	{'w', TRUE, (void *)0, "Wizard"},
#endif
	{'?', TRUE, (void *)0, "Help"},
	{0,0,0,0},
};

static const struct menu_tab discover_menu[] = {
	{'X', TRUE, dovspell, "View known spells"},                  /* Mike Stephenson */
	{'d', TRUE, dodiscovered, "Items already discovered [\\]"},             /* Robert Viduya */
	{'C', TRUE, do_mname, "Name a monster"},
	{0,0,0,0},
};

static struct menu_list main_menustruct[] = {
	{"Game", "Main Menu", game_menu},
	{"Inventory", "Main Menu", inv_menu},
	{"Action", "Main Menu", action_menu},
	{"Player", "Main Menu", player_menu},
	{"Discoveries", "Main Menu", discover_menu},
#ifdef WIZARD
	{"Wizard", "Main Menu", wizard_menu},
#endif
	{"Help", "Main Menu", help_menu},
	{"Main Menu",(char *)0, main_menu},
	{0,0,0},
};

STATIC_PTR int
makemenu(menuname, menu_struct)
const char *menuname;
struct menu_list menu_struct[];
{
	winid win;
	anything any;
	menu_item *selected;
        int n, i, NDECL((*func));
        const struct menu_tab *current_menu;

	any.a_void = 0;
	win = create_nhwindow(NHW_MENU);
	start_menu(win);

        for (i = 0; menu_struct[i].m_header; i++) {
                if (strcmp(menu_struct[i].m_header,menuname)) continue;
                current_menu = menu_struct[i].m_menu;
                for (n = 0; current_menu[n].m_item; n++) {
                        if (u.uburied && !current_menu[n].can_if_buried) continue;
#ifdef WIZARD
			if (!wizard && !current_menu[n].m_funct && !strcmp(current_menu[n].m_item,"Wizard")) continue;
#endif
                        if (current_menu[n].m_char == (char)0) {
                                any.a_int = 0; 
                                add_menu(win, NO_GLYPH, &any, 0, 0, ATR_BOLD,
                                     current_menu[n].m_item, MENU_UNSELECTED);
                                continue;
                        }
                        any.a_int = n + 1; /* non-zero */
                        add_menu(win, NO_GLYPH, &any, current_menu[n].m_char,
                        0, ATR_NONE, current_menu[n].m_item, MENU_UNSELECTED);
                }
                break;
        }
        end_menu(win, menuname);
        n = select_menu(win, PICK_ONE, &selected);
        destroy_nhwindow(win);
        if (n > 0) {
                /* we discard 'const' because some compilers seem to have
		       trouble with the pointer passed to set_occupation() */
                i = selected[0].item.a_int - 1;
                func = current_menu[i].m_funct;
                if (current_menu[i].m_text && !occupation && multi)
                      set_occupation(func, current_menu[i].m_text, multi);
                /*WAC catch void into makemenu */
                if (func == (void *)0)
                        return (makemenu(current_menu[i].m_item, menu_struct));
                else return (*func)();            /* perform the command */
        } else if (n < 0) {
                for (i = 0; menu_struct[i].m_header; i++){
                   if (menuname == menu_struct[i].m_header) {
                    if (menu_struct[i].m_parent)
                      return (makemenu(menu_struct[i].m_parent, menu_struct));
                    else return (0);
                }
        }
        }
        return 0;
}

STATIC_PTR int
domenusystem()  /* WAC add helpful menus ;B */
{
        return (makemenu("Main Menu", main_menustruct));
}

/* KMH, #conduct
 * (shares enlightenment's tense handling)
 */
STATIC_PTR int
doconduct()
{
	show_conduct(0);
	return 0;
}

/* format increased damage or chance to hit */
static char *
enlght_combatinc(inctyp, incamt, final, outbuf)
const char *inctyp;
int incamt, final;
char *outbuf;
{
	char numbuf[24];
	const char *modif, *bonus;

	if (final
#ifdef WIZARD
		|| wizard
#endif
	  ) {
	    Sprintf(numbuf, "%s%d",
		    (incamt > 0) ? "+" : "", incamt);
	    modif = (const char *) numbuf;
	} else {
	    int absamt = abs(incamt);

	    if (absamt <= 3) modif = "small";
	    else if (absamt <= 6) modif = "moderate";
	    else if (absamt <= 12) modif = "large";
	    else modif = "huge";
	}
	bonus = (incamt > 0) ? "bonus" : "penalty";
	/* "bonus to hit" vs "damage bonus" */
	if (!strcmp(inctyp, "damage")) {
	    const char *ctmp = inctyp;
	    inctyp = bonus;
	    bonus = ctmp;
	}
	Sprintf(outbuf, "%s %s %s", an(modif), bonus, inctyp);
	return outbuf;
}

void
show_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

	/* Create the conduct window */
	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Voluntary challenges:");
	putstr(en_win, 0, "");

	if (!u.uconduct.food && !u.uconduct.unvegan)
	    enl_msg(You_, "have gone", "went", " without food");
	    /* But beverages are okay */
	else if (!u.uconduct.food)
	    enl_msg(You_, "have gone", "went", " without eating");
	    /* But quaffing animal products (eg., blood) is okay */
	else if (!u.uconduct.unvegan) {
	    you_have_X("followed a strict vegan diet");
			Sprintf(buf, "eaten %ld time%s",
		    u.uconduct.food, plur(u.uconduct.food));
	    you_have_X(buf);
		}
	else if (!u.uconduct.unvegetarian) {
	    you_have_been("vegetarian");
			Sprintf(buf, "eaten %ld time%s",
		    u.uconduct.food, plur(u.uconduct.food));
	    you_have_X(buf);
			Sprintf(buf, "eaten food with animal by-products %ld time%s",
		    u.uconduct.unvegan, plur(u.uconduct.unvegan));
	    you_have_X(buf);

		}
	else {
			Sprintf(buf, "eaten %ld time%s",
		    u.uconduct.food, plur(u.uconduct.food));
	    you_have_X(buf);
			Sprintf(buf, "eaten food with animal by-products %ld time%s",
		    u.uconduct.unvegan, plur(u.uconduct.unvegan));
	    you_have_X(buf);
			Sprintf(buf, "eaten meat %ld time%s",
		    u.uconduct.unvegetarian, plur(u.uconduct.unvegetarian));
	    you_have_X(buf);

	}

	if (!u.uconduct.gnostic)
	    you_have_been("an atheist");
	else {	    Sprintf(buf, "communicated with the gods %ld time%s",
		    u.uconduct.gnostic, plur(u.uconduct.gnostic));
	    you_have_X(buf);
	}

	if (!u.uconduct.praydone)
	    you_have_never("prayed to the gods");
	else {	    Sprintf(buf, "prayed %ld time%s",
		    u.uconduct.praydone, plur(u.uconduct.praydone));
	    you_have_X(buf);
	}

	if (!u.uconduct.weaphit)
	    you_have_never("hit with a wielded weapon");
#ifdef WIZARD
	else /*if (wizard)*/ {
	    Sprintf(buf, "used a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    you_have_X(buf);
	}
#endif
	if (!u.uconduct.killer)
	    you_have_been("a pacifist");
	else {
		Sprintf(buf, "killed %ld monster%s",
		    u.uconduct.killer, plur(u.uconduct.killer));
	    you_have_X(buf);

	}

	if (!u.uconduct.literate)
	    you_have_been("illiterate");
#ifdef WIZARD
	else /*if (wizard)*/ {
	    Sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    you_have_X(buf);
	}
#endif

	ngenocided = num_genocides();
	if (ngenocided == 0) {
	    you_have_never("genocided any monsters");
	} else {
	    Sprintf(buf, "genocided %d type%s of monster%s",
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    you_have_X(buf);
	}

	if (!u.uconduct.polypiles)
	    you_have_never("polymorphed an object");
#ifdef WIZARD
	else /*if (wizard)*/ {
	    Sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    you_have_X(buf);
	}
#endif

	if (!u.uconduct.polyselfs)
	    you_have_never("changed form");
#ifdef WIZARD
	else /*if (wizard)*/ {
	    Sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    you_have_X(buf);
	}
#endif

	if (!u.uconduct.wishes)
	    you_have_X("used no wishes");
	else {
	    Sprintf(buf, "used %ld wish%s",
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : "");
	    you_have_X(buf);

	    if (!u.uconduct.wisharti)
		enl_msg(You_, "have not wished", "did not wish",
			" for any artifacts");
	}


	if (!u.uconduct.celibacy)
	    you_have_X("remained celibate");
#ifdef WIZARD
	else {
	    Sprintf(buf, "your vow of celibacy %ld time%s",
		    u.uconduct.celibacy, plur(u.uconduct.celibacy));
	    enl_msg(You_, "have broken ", "broke ", buf);
	}
#endif

	/* Pop up the window and wait for a key */
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
}

#ifdef DUMP_LOG
void
dump_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

	dump("", "Voluntary challenges");

	if (!u.uconduct.food)
	    dump("", "  You went without food");
	    /* But beverages are okay */
	else if (!u.uconduct.unvegan)
	    dump("", "  You followed a strict vegan diet");
	else if (!u.uconduct.unvegetarian)
	    dump("", "  You were a vegetarian");
	else if (Role_if(PM_MONK) && u.uconduct.unvegetarian < 10) {
	    sprintf(buf, "  You ate non-vegetarian food %ld time%s.", 
		u.uconduct.unvegetarian, plur(u.uconduct.unvegetarian));
	    dump("", buf);
	}

	if (!u.uconduct.gnostic)
	    dump("", "  You were an atheist");

	if (!u.uconduct.weaphit)
	    dump("", "  You never hit with a wielded weapon");
	else if (Role_if(PM_MONK) && u.uconduct.weaphit < 10) {
	    Sprintf(buf, "  You hit with a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    dump("", buf);
	}
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "hit with a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    dump("  You ", buf);
	}
#endif
	if (!u.uconduct.killer)
	    dump("", "  You were a pacifist");

	if (!u.uconduct.literate)
	    dump("", "  You were illiterate");
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    dump("  You ", buf);
	}
#endif

	ngenocided = num_genocides();
	if (ngenocided == 0) {
	    dump("", "  You never genocided any monsters");
	} else {
	    Sprintf(buf, "genocided %d type%s of monster%s",
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    dump("  You ", buf);
	}

	if (!u.uconduct.polypiles)
	    dump("", "  You never polymorphed an object");
	else {
	    Sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    dump("  You ", buf);
	}

	if (!u.uconduct.polyselfs)
	    dump("", "  You never changed form");
	else {
	    Sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    dump("  You ", buf);
	}

	if (!u.uconduct.wishes)
	    dump("", "  You used no wishes");
	else {
	    Sprintf(buf, "used %ld wish%s",
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : "");
	    dump("  You ", buf);

	    if (!u.uconduct.wisharti)
		dump("", "  You did not wish for any artifacts");
	}

	dump("", "");
}
#endif /* DUMP_LOG */

#endif /* OVLB */
#ifdef OVL1

#ifndef M
# ifndef NHSTDC
#  define M(c)		(0x80 | (c))
# else
#  define M(c)		((c) - 128)
# endif /* NHSTDC */
#endif
#ifndef C
#define C(c)		(0x1f & (c))
#endif

static const struct func_tab cmdlist[] = {
	{C('d'), FALSE, dokick}, /* "D" is for door!...?  Msg is in dokick.c */
#ifdef WIZARD
	{C('b'), FALSE, playersteal},
/* BEGIN TSANTH'S CODE */
	{C('c'), TRUE, wiz_gain_ac},
/* END TSANTH'S CODE */
	{C('e'), TRUE, wiz_detect},
	{C('f'), TRUE, wiz_map},
	{C('g'), TRUE, wiz_genesis},
	{C('i'), TRUE, wiz_identify},
	{C('j'), TRUE, wiz_gain_level},
#endif
	{C('l'), TRUE, doredraw}, /* if number_pad is set */
#ifdef WIZARD
/* BEGIN TSANTH'S CODE */
	{C('n'), TRUE, wiz_toggle_invulnerability},
/* END TSANTH'S CODE */
	{C('h'), TRUE, wiz_detect_monsters},
	{C('o'), TRUE, wiz_where},
#endif
	{C('p'), TRUE, doprev_message},
	{C('q'), TRUE, done2},
	{C('r'), TRUE, doredraw},
/*	{C('s'), FALSE, specialpower},*/
	{C('s'), TRUE, dosave},
	{C('t'), TRUE, dotele},
	{C('u'), TRUE, doremoveimarkers},
#ifdef WIZARD
	{C('v'), TRUE, wiz_level_tele},
	{C('w'), TRUE, wiz_wish},
#endif
	{C('x'), TRUE, doattributes},
	{C('y'), TRUE, polyatwill},
#ifdef SUSPEND
	{C('z'), TRUE, dosuspend},
#endif
	{'a', FALSE, doapply},
	{'A', FALSE, doddoremarm},
	{M('a'), TRUE, doorganize},
/*	'b', 'B' : go sw */
#ifdef BORG
	{'B', TRUE, doborgtoggle}, /* [Tom] */
#endif
	{M('b'), FALSE, playersteal},   /* jla */
#if 0
	{M('b'), FALSE, specialpower},   /* jla */
#endif
	{'c', FALSE, doclose},
	{'C', TRUE, do_mname},
	{M('c'), TRUE, dotalk},
	{'d', FALSE, dodrop},
	{'D', FALSE, doddrop},
	{M('d'), FALSE, dodip},
	{'e', FALSE, doeat},
	{'E', FALSE, doengrave},
	{M('e'), TRUE, enhance_weapon_skill},
	{'f', FALSE, dofire},
/*	'F' : fight (one time) */
	{M('f'), FALSE, doforce},
/*	'g', 'G' : multiple go */
/*	'h', 'H' : go west */
	{'h', TRUE, dohelp}, /* if number_pad is set */
	{'i', TRUE, ddoinv},
	{'I', TRUE, dotypeinv},		/* Robert Viduya */
	{M('i'), TRUE, doinvoke},
/*	'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N' : move commands */
	{'j', FALSE, dojump}, /* if number_pad is on */
	{M('j'), FALSE, dojump},
	{'k', FALSE, dokick}, /* if number_pad is on */
	{'K', TRUE, dolistvanq}, /* if number_pad is on */
	{M('k'), TRUE, enhance_weapon_skill},
	{'l', FALSE, doloot}, /* if number_pad is on */
	{M('l'), FALSE, doloot},
/*	'n' prefixes a count if number_pad is on */
	{M('m'), TRUE, domonability},
	{'N', TRUE, ddocall}, /* if number_pad is on */
	{M('n'), TRUE, ddocall},
	{'o', FALSE, doopen},
	{'O', TRUE, doset},
	{M('o'), FALSE, dosacrifice},
	{'p', FALSE, dopay},
/*WAC replace with dowear*/
	{'P', FALSE, doputon},
	{M('p'), TRUE, dopray},
	{'q', FALSE, dodrink},
	{'Q', FALSE, dowieldquiver},
	{M('q'), TRUE, done2},
	{'r', FALSE, doread},
	{'R', FALSE, doremring},
	{M('r'), FALSE, dorub},
	{'s', TRUE, dosearch, "searching"},
	{'S', TRUE, dosave},
	{M('s'), FALSE, dosit},
	{'t', FALSE, dothrow},
	{'T', FALSE, dotakeoff},
/*	{M('t'), TRUE, doturn},*/
	{M('t'), TRUE, dotech},
/*	'u', 'U' : go ne */
	{'u', FALSE, dountrap}, /* if number_pad is on */
	{M('u'), FALSE, dountrap},
	{'v', TRUE, doversion},
	{'V', TRUE, dohistory},
	{M('v'), TRUE, doextversion},
/*replaced with dowear*/
	{'w', FALSE, dowield},
	{'W', FALSE, dowear},
	{M('w'), FALSE, dowipe},
	{'x', FALSE, doswapweapon},                    /* [Tom] */        
	{'X', FALSE, dotwoweapon},
#if 0
        {M('x'), TRUE, dovspell},                  /* Mike Stephenson */
#endif
/*	'y', 'Y' : go nw */
	{M('y'), FALSE, polyatwill},  /* jla */        
	{'z', FALSE, dozap},
	{'Z', TRUE, docast},
	{'<', FALSE, doup},
	{'>', FALSE, dodown},
	{'/', TRUE, dowhatis},
	{'&', TRUE, dowhatdoes},
	{'?', TRUE, dohelp},
	{M('?'), TRUE, doextlist},
#ifdef SHELL
	{'!', TRUE, dosh},
#endif
	{'.', TRUE, donull, "waiting"},
	{' ', TRUE, donull, "waiting"},
	{',', FALSE, dopickup},
	{':', TRUE, dolook},
	{';', TRUE, doquickwhatis},
	{'^', TRUE, doidtrap},
	{'\\', TRUE, dodiscovered},		/* Robert Viduya */
	{'@', TRUE, dotogglepickup},
	{M('2'), FALSE, dotwoweapon},
/* WAC Angband style items in use, menusystem
	{'*', TRUE, doinvinuse}, */
	{'`', TRUE, domenusystem},
	{'~', TRUE, domenusystem},
	{WEAPON_SYM,  TRUE, doprwep},
	{ARMOR_SYM,  TRUE, doprarm},
	{RING_SYM,  TRUE, doprring},
	{AMULET_SYM, TRUE, dopramulet},
	{TOOL_SYM, TRUE, doprtool},
	{'*', TRUE, doprinuse},	/* inventory of all equipment in use */
	{GOLD_SYM, TRUE, doprgold},
	{SPBOOK_SYM, TRUE, dovspell},			/* Mike Stephenson */
	{'#', TRUE, doextcmd},
	{'_', TRUE, dotravel},
	{0,0,0,0}
};

struct ext_func_tab extcmdlist[] = {
	{"2weapon", "toggle two-weapon combat", dotwoweapon, FALSE},
	{"adjust", "adjust inventory letters", doorganize, TRUE},
	{"borrow", "steal from monsters", playersteal, FALSE},  /* jla */        
	{"chat", "talk to someone", dotalk, TRUE},	/* converse? */
	{"conduct", "list which challenges you have adhered to", doconduct, TRUE},
	{"dip", "dip an object into something", dodip, FALSE},
	{"enhance", "advance or check weapons skills", enhance_weapon_skill,
							TRUE},
#if 0
	{"ethics", "list which challenges you have adhered to", doethics, TRUE},
#endif
#ifndef PUBLIC_SERVER
	{"explore", "enter explore mode", enter_explore_mode, TRUE},
#endif
	{"force", "force a lock", doforce, FALSE},
	{"invoke", "invoke an object's powers", doinvoke, TRUE},
	{"jump", "jump to a location", dojump, FALSE},
	{"loot", "loot a box on the floor", doloot, FALSE},
	{"monster", "use a monster's special ability", domonability, TRUE},
	{"name", "name an item or type of object", ddocall, TRUE},
	{"offer", "offer a sacrifice to the gods", dosacrifice, FALSE},
	{"pray", "pray to the gods for help", dopray, TRUE},
	{"quit", "exit without saving current game", done2, TRUE},
#ifdef STEED
	{"ride", "ride (or stop riding) a monster", doride, FALSE},
#endif
	{"rub", "rub a lamp or a stone", dorub, FALSE},
#ifdef LIVELOG_SHOUT
	{"shout", "shout something", doshout, FALSE},
#endif
	{"sit", "sit down", dosit, FALSE},
#ifdef SHOUT
	{"shout", "say something loud", doyell, TRUE}, /* jrn */
#endif
	{"technique", "perform a technique", dotech, TRUE},
	{"turn", "turn undead", doturn, TRUE},
	{"twoweapon", "toggle two-weapon combat", dotwoweapon, FALSE},
	{"untrap", "untrap something", dountrap, FALSE},
	{"vanquished", "list vanquished monsters", dolistvanq, TRUE},
	{"version", "list compile time options for this version of Slash'EM",
		doextversion, TRUE},
	{"wipe", "wipe off your face", dowipe, FALSE},
	{"youpoly", "polymorph at will", polyatwill, FALSE},  /* jla */        
	{"?", "get this list of extended commands", doextlist, TRUE},
#if defined(WIZARD)
	/*
	 * There must be a blank entry here for every entry in the table
	 * below.
	 */
#ifdef DISPLAY_LAYERS
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef DEBUG_MIGRATING_MONS
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef PORT_DEBUG
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
        {(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef DEBUG
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE}	/* sentinel */
};

#if defined(WIZARD)
static const struct ext_func_tab debug_extcmdlist[] = {
#ifdef DISPLAY_LAYERS
	{"display", "detail display layers", wiz_show_display, TRUE},
#endif
	{"levelchange", "change experience level", wiz_level_change, TRUE},
	{"lightsources", "show mobile light sources", wiz_light_sources, TRUE},
#ifdef DEBUG_MIGRATING_MONS
	{"migratemons", "migrate n random monsters", wiz_migrate_mons, TRUE},
#endif
	{"monpolycontrol", "control monster polymorphs", wiz_mon_polycontrol, TRUE},
	{"panic", "test panic routine (fatal to game)", wiz_panic, TRUE},
	{"polyself", "polymorph self", wiz_polyself, TRUE},
#ifdef PORT_DEBUG
	{"portdebug", "wizard port debug command", wiz_port_debug, TRUE},
#endif
	{"seenv", "show seen vectors", wiz_show_seenv, TRUE},
	{"stats", "show memory statistics", wiz_show_stats, TRUE},
	{"timeout", "look at timeout queue", wiz_timeout_queue, TRUE},
	{"vision", "show vision array", wiz_show_vision, TRUE},
#ifdef DEBUG
	{"wizdebug", "wizard debug command", wiz_debug_cmd, TRUE},
#endif
	{"wmode", "show wall modes", wiz_show_wmodes, TRUE},
	{(char *)0, (char *)0, donull, TRUE}
};

/*
 * Insert debug commands into the extended command list.  This function
 * assumes that the last entry will be the help entry.
 *
 * You must add entries in ext_func_tab every time you add one to the
 * debug_extcmdlist().
 */
void
add_debug_extended_commands()
{
	int i, j, k, n;

	/* count the # of help entries */
	for (n = 0; extcmdlist[n].ef_txt[0] != '?'; n++)
	    ;

	for (i = 0; debug_extcmdlist[i].ef_txt; i++) {
	    for (j = 0; j < n; j++)
		if (strcmp(debug_extcmdlist[i].ef_txt, extcmdlist[j].ef_txt) < 0) break;

	    /* insert i'th debug entry into extcmdlist[j], pushing down  */
	    for (k = n; k >= j; --k)
		extcmdlist[k+1] = extcmdlist[k];
	    extcmdlist[j] = debug_extcmdlist[i];
	    n++;	/* now an extra entry */
	}
}


static const char template[] = "%-18s %4ld  %6ld";
static const char count_str[] = "                   count  bytes";
static const char separator[] = "------------------ -----  ------";

STATIC_OVL void
count_obj(chain, total_count, total_size, top, recurse)
	struct obj *chain;
	long *total_count;
	long *total_size;
	boolean top;
	boolean recurse;
{
	long count, size;
	struct obj *obj;

	for (count = size = 0, obj = chain; obj; obj = obj->nobj) {
	    if (top) {
		count++;
		size += sizeof(struct obj) + obj->oxlth + obj->onamelth;
	    }
	    if (recurse && obj->cobj)
		count_obj(obj->cobj, total_count, total_size, TRUE, TRUE);
	}
	*total_count += count;
	*total_size += size;
}

STATIC_OVL void
obj_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct obj *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;

	count_obj(chain, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_invent_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	for (mon = chain; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
contained(win, src, total_count, total_size)
	winid win;
	const char *src;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	count_obj(invent, &count, &size, FALSE, TRUE);
	count_obj(fobj, &count, &size, FALSE, TRUE);
	count_obj(level.buriedobjlist, &count, &size, FALSE, TRUE);
	count_obj(migrating_objs, &count, &size, FALSE, TRUE);
	/* DEADMONSTER check not required in this loop since they have no inventory */
	for (mon = fmon; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);
	for (mon = migrating_mons; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);

	*total_count += count; *total_size += size;

	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count, size;
	struct monst *mon;

	for (count = size = 0, mon = chain; mon; mon = mon->nmon) {
	    count++;
	    size += sizeof(struct monst) + mon->mxlth + mon->mnamelth;
	}
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

/*
 * Display memory usage of all monsters and objects on the level.
 */
static int
wiz_show_stats()
{
	char buf[BUFSZ];
	winid win;
	long total_obj_size = 0, total_obj_count = 0;
	long total_mon_size = 0, total_mon_count = 0;

	win = create_nhwindow(NHW_TEXT);
	putstr(win, 0, "Current memory statistics:");
	putstr(win, 0, "");
	Sprintf(buf, "Objects, size %d", (int) sizeof(struct obj));
	putstr(win, 0, buf);
	putstr(win, 0, "");
	putstr(win, 0, count_str);

	obj_chain(win, "invent", invent, &total_obj_count, &total_obj_size);
	obj_chain(win, "fobj", fobj, &total_obj_count, &total_obj_size);
	obj_chain(win, "buried", level.buriedobjlist,
				&total_obj_count, &total_obj_size);
	obj_chain(win, "migrating obj", migrating_objs,
				&total_obj_count, &total_obj_size);
	mon_invent_chain(win, "minvent", fmon,
				&total_obj_count,&total_obj_size);
	mon_invent_chain(win, "migrating minvent", migrating_mons,
				&total_obj_count, &total_obj_size);

	contained(win, "contained",
				&total_obj_count, &total_obj_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_obj_count, total_obj_size);
	putstr(win, 0, buf);

	putstr(win, 0, "");
	putstr(win, 0, "");
	Sprintf(buf, "Monsters, size %d", (int) sizeof(struct monst));
	putstr(win, 0, buf);
	putstr(win, 0, "");

	mon_chain(win, "fmon", fmon,
				&total_mon_count, &total_mon_size);
	mon_chain(win, "migrating", migrating_mons,
				&total_mon_count, &total_mon_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_mon_count, total_mon_size);
	putstr(win, 0, buf);

#if defined(__BORLANDC__) && !defined(_WIN32)
	show_borlandc_stats(win);
#endif

	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return 0;
}

void
sanity_check()
{
	obj_sanity_check();
	timer_sanity_check();
}

#ifdef DISPLAY_LAYERS
/*
 * Detail contents of each display layer at specified location(s).
 */
static int
wiz_show_display()
{
    int ans, glyph;
    coord cc;
    winid win;
    char buf[BUFSZ];
    struct rm *lev;

    cc.x = u.ux;
    cc.y = u.uy;
    pline("Pick a location.");
    ans = getpos(&cc, FALSE, "a location of interest");
    if (ans < 0 || cc.x < 0)
	return 0;	/* done */
    lev = &levl[cc.x][cc.y];
    win = create_nhwindow(NHW_MENU);
    Sprintf(buf, "Contents of hero's memory at (%d, %d):", cc.x, cc.y);
    putstr(win, 0, buf);
    putstr(win, 0, "");
    Sprintf(buf, "Invisible monster: %s",
	    lev->mem_invis ? "present" : "none");
    putstr(win, 0, buf);
    if (lev->mem_obj && lev->mem_corpse)
	if (mons[lev->mem_obj - 1].geno & G_UNIQ)
	    Sprintf(buf, "Object: %s%s corpse",
		    type_is_pname(&mons[lev->mem_obj - 1]) ? "" : "the ",
		    s_suffix(mons[lev->mem_obj - 1].mname));
	else
	    Sprintf(buf, "Object: %s corpse", mons[lev->mem_obj - 1].mname);
    else
	Sprintf(buf, "Object: %s", lev->mem_obj ?
		obj_typename(lev->mem_obj - 1) : "none");
    putstr(win, 0, buf);
    Sprintf(buf, "Trap: %s", lev->mem_trap ?
	    defsyms[trap_to_defsym(lev->mem_trap)].explanation : "none");
    putstr(win, 0, buf);
    Sprintf(buf, "Backgroud: %s", defsyms[lev->mem_bg].explanation);
    putstr(win, 0, buf);
    putstr(win, 0, "");
    glyph = glyph_at(cc.x, cc.y);
    Sprintf(buf, "Buffered (3rd screen): ");
    if (glyph_is_monster(glyph)) {
	Strcat(buf, mons[glyph_to_mon(glyph)].mname);
	if (glyph_is_pet(glyph))
	    Strcat(buf, " (tame)");
	if (glyph_is_ridden_monster(glyph))
	    Strcat(buf, " (ridden)");
	if (glyph_is_detected_monster(glyph))
	    Strcat(buf, " (detected)");
    } else if (glyph_is_object(glyph)) {
	if (glyph_is_body(glyph)) {
	    int corpse = glyph_to_body(glyph);
	    if (mons[corpse].geno & G_UNIQ)
		Sprintf(eos(buf), "%s%s corpse",
			type_is_pname(&mons[corpse]) ? "" : "the ",
			s_suffix(mons[corpse].mname));
	    else
		Sprintf(eos(buf), "%s corpse", mons[corpse].mname);
	} else
	    Strcat(buf, obj_typename(glyph_to_obj(glyph)));
    } else if (glyph_is_invisible(glyph))
	Strcat(buf, "invisible monster");
    else if (glyph_is_cmap(glyph))
	Strcat(buf, defsyms[glyph_to_cmap(glyph)].explanation);
    else
	Sprintf(eos(buf), "[%d]", glyph);
    putstr(win, 0, buf);
    display_nhwindow(win, FALSE);
    destroy_nhwindow(win);
    return 0;
}
#endif

#ifdef DEBUG_MIGRATING_MONS
static int
wiz_migrate_mons()
{
	int mcount = 0;
	char inbuf[BUFSZ];
	struct permonst *ptr;
	struct monst *mtmp;
	d_level tolevel;
	getlin("How many random monsters to migrate? [0]", inbuf);
	if (*inbuf == '\033') return 0;
	mcount = atoi(inbuf);
	if (mcount < 0 || mcount > (COLNO * ROWNO) || Is_botlevel(&u.uz))
		return 0;
	while (mcount > 0) {
		if (Is_stronghold(&u.uz))
		    assign_level(&tolevel, &valley_level);
		else
		    get_level(&tolevel, depth(&u.uz) + 1);
		ptr = rndmonst();
		mtmp = makemon(ptr, 0, 0, NO_MM_FLAGS);
		if (mtmp) migrate_to_level(mtmp, ledger_no(&tolevel),
				MIGR_RANDOM, (coord *)0);
		mcount--;
	}
	return 0;
}
#endif



#endif /* WIZARD */

#define unctrl(c)	((c) <= C('z') ? (0x60 | (c)) : (c))
#define unmeta(c)	(0x7f & (c))


void
rhack(cmd)
register char *cmd;
{
	boolean do_walk, do_rush, prefix_seen, bad_command,
		firsttime = (cmd == 0);

	iflags.menu_requested = FALSE;
	if (firsttime) {
		flags.nopick = 0;
		cmd = parse();
	}
	if (*cmd == '\033') { /* <esc> key - user might be panicking */
		/* Bring up the menu */
		if (multi || !flags.menu_on_esc || !(domenusystem())) {
		flags.move = FALSE;
		    multi = 0;
		}
		return;
#if 0
		flags.move = FALSE;
		return;
#endif
	}
#ifdef REDO
	if (*cmd == DOAGAIN && !in_doagain && saveq[0]) {
		in_doagain = TRUE;
		stail = 0;
		rhack((char *)0);	/* read and execute command */
		in_doagain = FALSE;
		return;
	}
	/* Special case of *cmd == ' ' handled better below */
	if(!*cmd || *cmd == (char)0377)
#else
	if(!*cmd || *cmd == (char)0377 || (!flags.rest_on_space && *cmd == ' '))
#endif
	{
		nhbell();
		flags.move = FALSE;
		return;		/* probably we just had an interrupt */
	}
	if (iflags.num_pad && iflags.num_pad_mode == 1) {
		/* This handles very old inconsistent DOS/Windows behaviour
		 * in a new way: earlier, the keyboard handler mapped these,
		 * which caused counts to be strange when entered from the
		 * number pad. Now do not map them until here. 
		 */
		switch (*cmd) {
		    case '5':       *cmd = 'g'; break;
		    case M('5'):    *cmd = 'G'; break;
		    case M('0'):    *cmd = 'I'; break;
        	}
        }
	/* handle most movement commands */
	do_walk = do_rush = prefix_seen = FALSE;
	flags.travel = iflags.travel1 = 0;
	switch (*cmd) {
	 case 'g':  if (movecmd(cmd[1])) {
			flags.run = 2;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case '5':  if (!iflags.num_pad) break;	/* else FALLTHRU */
	 case 'G':  if (movecmd(lowc(cmd[1]))) {
			flags.run = 3;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case '-':  if (!iflags.num_pad) break;	/* else FALLTHRU */
	/* Effects of movement commands and invisible monsters:
	 * m: always move onto space (even if 'I' remembered)
	 * F: always attack space (even if 'I' not remembered)
	 * normal movement: attack if 'I', move otherwise
	 */
	 case 'F':  if (movecmd(cmd[1])) {
			flags.forcefight = 1;
			do_walk = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case 'm':  if (movecmd(cmd[1]) || u.dz) {
			flags.run = 0;
			flags.nopick = 1;
			if (!u.dz) do_walk = TRUE;
			else cmd[0] = cmd[1];	/* "m<" or "m>" */
		    } else
			prefix_seen = TRUE;
		    break;
	 case 'M':  if (movecmd(lowc(cmd[1]))) {
			flags.run = 1;
			flags.nopick = 1;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case '0':  if (!iflags.num_pad) break;
		    (void)ddoinv(); /* a convenience borrowed from the PC */
		    flags.move = FALSE;
		    multi = 0;
		    return;
	 case CMD_TRAVEL:
		    if (iflags.travelcmd) {
			    flags.travel = 1;
			    iflags.travel1 = 1;
			    flags.run = 8;
			    flags.nopick = 1;
			    do_rush = TRUE;
			    break;
		    }
		    /*FALLTHRU*/
	 default:   if (movecmd(*cmd)) {	/* ordinary movement */
			flags.run = 0;	/* only matters here if it was 8 */
			do_walk = TRUE;
		    } else if (movecmd(iflags.num_pad ?
				       unmeta(*cmd) : lowc(*cmd))) {
			flags.run = 1;
			do_rush = TRUE;
		    } else if (movecmd(unctrl(*cmd))) {
			flags.run = 3;
			do_rush = TRUE;
		    }
		    break;
	}

	/* some special prefix handling */
	/* overload 'm' prefix for ',' to mean "request a menu" */
	if (prefix_seen && cmd[1] == ',') {
		iflags.menu_requested = TRUE;
		++cmd;
	}

	if (do_walk) {
	    if (multi) flags.mv = TRUE;
	    domove();
	    flags.forcefight = 0;
	    return;
	} else if (do_rush) {
	    if (firsttime) {
		if (!multi) multi = max(COLNO,ROWNO);
		u.last_str_turn = 0;
	    }
	    flags.mv = TRUE;
	    domove();
	    return;
	} else if (prefix_seen && cmd[1] == '\033') {	/* <prefix><escape> */
	    /* don't report "unknown command" for change of heart... */
	    bad_command = FALSE;
	} else if (*cmd == ' ' && !flags.rest_on_space) {
	    bad_command = TRUE;		/* skip cmdlist[] loop */
	/* handle all other commands */
	} else {
	    register const struct func_tab *tlist;
	    int res, NDECL((*func));
#ifdef QWERTZ
            unsigned char cmdchar = *cmd & 0xff;
#endif
	    for (tlist = cmdlist; tlist->f_char; tlist++) {
#ifdef QWERTZ
                if (C(cmdchar) == C('y') && iflags.qwertz_layout == TRUE)
                    cmdchar += 'z' - 'y';

                if (cmdchar != (tlist->f_char & 0xff)) continue;
#else
		if ((*cmd & 0xff) != (tlist->f_char & 0xff)) continue;
#endif
		if (u.uburied && !tlist->can_if_buried) {
		    You_cant("do that while you are buried!");
		    res = 0;
		} else {
		    /* we discard 'const' because some compilers seem to have
		       trouble with the pointer passed to set_occupation() */
		    func = ((struct func_tab *)tlist)->f_funct;
		    if (tlist->f_text && !occupation && multi)
			set_occupation(func, tlist->f_text, multi);
		    res = (*func)();		/* perform the command */
		}
		if (!res) {
		    flags.move = FALSE;
		    multi = 0;
		}
		return;
	    }
	    /* if we reach here, cmd wasn't found in cmdlist[] */
	    bad_command = TRUE;
	}
	if (bad_command) {
	    char expcmd[10];
	    register char *cp = expcmd;

	    while (*cmd && (int)(cp - expcmd) < (int)(sizeof expcmd - 3)) {
		if (*cmd >= 040 && *cmd < 0177) {
		    *cp++ = *cmd++;
		} else if (*cmd & 0200) {
		    *cp++ = 'M';
		    *cp++ = '-';
		    *cp++ = *cmd++ &= ~0200;
		} else {
		    *cp++ = '^';
		    *cp++ = *cmd++ ^ 0100;
		}
	    }
	    *cp = '\0';
	    if (!prefix_seen || !iflags.cmdassist ||
		!help_dir(0, "Invalid direction key!"))
	    Norep("Unknown command '%s'.", expcmd);
	}
	/* didn't move */
	flags.move = FALSE;
	multi = 0;
	return;
}

int
xytod(x, y)	/* convert an x,y pair into a direction code */
schar x, y;
{
	register int dd;

	for(dd = 0; dd < 8; dd++)
	    if(x == xdir[dd] && y == ydir[dd]) return dd;

	return -1;
}

void
dtoxy(cc,dd)	/* convert a direction code into an x,y pair */
coord *cc;
register int dd;
{
	cc->x = xdir[dd];
	cc->y = ydir[dd];
	return;
}

int
movecmd(sym)	/* also sets u.dz, but returns false for <> */
char sym;
{
	register const char *dp;
	register const char *sdp;
	if(iflags.num_pad) sdp = ndir; else sdp = sdir;	/* DICE workaround */

	u.dz = 0;
	if(!(dp = index(sdp, sym))) return 0;
	u.dx = xdir[dp-sdp];
	u.dy = ydir[dp-sdp];
	u.dz = zdir[dp-sdp];

	if (u.totter) {

		u.dx = -u.dx;
		u.dy = -u.dy;

	}

	if (u.dx && u.dy && (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || u.umonnum == PM_GRID_BUG || u.umonnum == PM_WEREGRIDBUG || u.umonnum == PM_GRID_XORN || u.umonnum == PM_STONE_BUG || u.umonnum == PM_NATURAL_BUG || u.umonnum == PM_MELEE_BUG || u.umonnum == PM_WEAPON_BUG || (Race_if(PM_WEAPON_BUG) && !Upolyd) ) ) {
		u.dx = u.dy = 0;
		return 0;
	}
	return !u.dz;
}

/*
 * uses getdir() but unlike getdir() it specifically
 * produces coordinates using the direction from getdir()
 * and verifies that those coordinates are ok.
 *
 * If the call to getdir() returns 0, Never_mind is displayed.
 * If the resulting coordinates are not okay, emsg is displayed.
 *
 * Returns non-zero if coordinates in cc are valid.
 */
int get_adjacent_loc(prompt,emsg,x,y,cc)
const char *prompt, *emsg;
xchar x,y;
coord *cc;
{
	xchar new_x, new_y;
	if (!getdir(prompt)) {
		pline(Never_mind);
		return 0;
	}
	new_x = x + u.dx;
	new_y = y + u.dy;
	if (cc && isok(new_x,new_y)) {
		cc->x = new_x;
		cc->y = new_y;
	} else {
		if (emsg) pline(emsg);
		return 0;
	}
	return 1;
}

int
getdir(s)
const char *s;
{
	char dirsym;
	/* WAC add dirsymbols to generic prompt */
	char buf[BUFSZ];
        
	Sprintf(buf, "In what direction? [%s]",
                (iflags.num_pad ? ndir : sdir));


#ifdef REDO
	if(in_doagain || *readchar_queue)
	    dirsym = readchar();
	else
#endif
	do {
	    dirsym = yn_function ((s && *s != '^') ? s : buf, (char *)0, '\0');
	} while (!movecmd(dirsym) && !index(quitchars, dirsym)
                && dirsym == '.' && dirsym == 's' && !u.dz);

#ifdef REDO
	savech(dirsym);
#endif
	if(dirsym == '.' || dirsym == 's')
		u.dx = u.dy = u.dz = 0;
	else if(!movecmd(dirsym) && !u.dz) {
		boolean did_help = FALSE;
		if(!index(quitchars, dirsym)) {
		    if (iflags.cmdassist) {
			did_help = help_dir((s && *s == '^') ? dirsym : 0,
					    "Invalid direction key!");
		    }
		    if (!did_help) pline("What a strange direction!");
		}
		return 0;
	}
	if(!u.dz && ((Stunned && !rn2(Stun_resist ? 8 : 2)) || (Confusion && !rn2(Conf_resist ? 40 : 8)))) confdir();
	return 1;
}

STATIC_OVL boolean
help_dir(sym, msg)
char sym;
const char *msg;
{
	char ctrl;
	winid win;
	static const char wiz_only_list[] = "EFGIOVW";
	char buf[BUFSZ], buf2[BUFSZ], *expln;

	win = create_nhwindow(NHW_TEXT);
	if (!win) return FALSE;
	if (msg) {
		Sprintf(buf, "cmdassist: %s", msg);
		putstr(win, 0, buf);
		putstr(win, 0, "");
	}
	if (letter(sym)) { 
	    sym = highc(sym);
	    ctrl = (sym - 'A') + 1;
	    if ((expln = dowhatdoes_core(ctrl, buf2))
		&& (!index(wiz_only_list, sym)
#ifdef WIZARD
		    || wizard
#endif
	                     )) {
		Sprintf(buf, "Are you trying to use ^%c%s?", sym,
			index(wiz_only_list, sym) ? "" :
			" as specified in the Guidebook");
		putstr(win, 0, buf);
		putstr(win, 0, "");
		putstr(win, 0, expln);
		putstr(win, 0, "");
		putstr(win, 0, "To use that command, you press");
		Sprintf(buf,
			"the <Ctrl> key, and the <%c> key at the same time.", sym);
		putstr(win, 0, buf);
		putstr(win, 0, "");
	    }
	}
	if (iflags.num_pad && (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || u.umonnum == PM_GRID_BUG || u.umonnum == PM_WEREGRIDBUG || u.umonnum == PM_GRID_XORN || u.umonnum == PM_STONE_BUG || u.umonnum == PM_NATURAL_BUG || u.umonnum == PM_WEAPON_BUG || u.umonnum == PM_MELEE_BUG || (Race_if(PM_WEAPON_BUG) && !Upolyd))) {
	    putstr(win, 0, "Valid direction keys in your current form (with number_pad on) are:");
	    putstr(win, 0, "             8   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             2   ");
	} else if (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || u.umonnum == PM_GRID_BUG || u.umonnum == PM_WEREGRIDBUG || u.umonnum == PM_GRID_XORN || u.umonnum == PM_STONE_BUG || u.umonnum == PM_NATURAL_BUG || u.umonnum == PM_MELEE_BUG || u.umonnum == PM_WEAPON_BUG || (Race_if(PM_WEAPON_BUG) && !Upolyd)) {
	    putstr(win, 0, "Valid direction keys in your current form are:");
	    putstr(win, 0, "             k   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             j   ");
	} else if (iflags.num_pad) {
	    putstr(win, 0, "Valid direction keys (with number_pad on) are:");
	    putstr(win, 0, "          7  8  9");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          1  2  3");
	} else {
	    putstr(win, 0, "Valid direction keys are:");
	    putstr(win, 0, "          y  k  u");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          b  j  n");
	};
	putstr(win, 0, "");
	putstr(win, 0, "          <  up");
	putstr(win, 0, "          >  down");
	putstr(win, 0, "          .  direct at yourself");
	putstr(win, 0, "");
	putstr(win, 0, "(Suppress this message with !cmdassist in config file.)");
	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return TRUE;
}

#endif /* OVL1 */
#ifdef OVLB

void
confdir()
{
	register int x = (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || u.umonnum == PM_GRID_BUG || u.umonnum == PM_WEREGRIDBUG || u.umonnum == PM_GRID_XORN || u.umonnum == PM_STONE_BUG || u.umonnum == PM_NATURAL_BUG || u.umonnum == PM_MELEE_BUG || u.umonnum == PM_WEAPON_BUG || (Race_if(PM_WEAPON_BUG) && !Upolyd)) ? 2*rn2(4) : rn2(8);
	u.dx = xdir[x];
	u.dy = ydir[x];
	return;
}

#endif /* OVLB */
#ifdef OVL0

int
isok(x,y)
register int x, y;
{
	/* x corresponds to curx, so x==1 is the first column. Ach. %% */
	return x >= 1 && x <= COLNO-1 && y >= 0 && y <= ROWNO-1;
}

static NEARDATA int last_multi;

/*
 * convert a MAP window position into a movecmd
 */
const char *
click_to_cmd(x, y, mod)
    int x, y, mod;
{
    int dir;
    static char cmd[4];
    cmd[1]=0;

    x -= u.ux;
    y -= u.uy;

    if (iflags.travelcmd) {
        if (abs(x) <= 1 && abs(y) <= 1 ) {
	x = sgn(x), y = sgn(y);
    } else {
	u.tx = u.ux+x;
	u.ty = u.uy+y;
	cmd[0] = CMD_TRAVEL;
	return cmd;
    }

    if(x == 0 && y == 0) {
	/* here */
	if(IS_FOUNTAIN(levl[u.ux][u.uy].typ) || IS_SINK(levl[u.ux][u.uy].typ)) {
	    cmd[0]=mod == CLICK_1 ? 'q' : M('d');
	    return cmd;
	} else if(IS_THRONE(levl[u.ux][u.uy].typ)) {
	    cmd[0]=M('s');
	    return cmd;
	} else if((u.ux == xupstair && u.uy == yupstair)
		  || (u.ux == sstairs.sx && u.uy == sstairs.sy && sstairs.up)
		  || (u.ux == xupladder && u.uy == yupladder)) {
	    return "<";
	} else if((u.ux == xdnstair && u.uy == ydnstair)
		  || (u.ux == sstairs.sx && u.uy == sstairs.sy && !sstairs.up)
		  || (u.ux == xdnladder && u.uy == ydnladder)) {
	    return ">";
	} else if(OBJ_AT(u.ux, u.uy)) {
	    cmd[0] = Is_container(level.objects[u.ux][u.uy]) ? M('l') : ',';
	    return cmd;
	} else {
	    return "."; /* just rest */
	}
    }

    /* directional commands */

    dir = xytod(x, y);

	if (!m_at(u.ux+x, u.uy+y) && !test_move(u.ux, u.uy, x, y, TEST_MOVE)) {
	cmd[1] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
	cmd[2] = 0;
	if (IS_DOOR(levl[u.ux+x][u.uy+y].typ)) {
	    /* slight assistance to the player: choose kick/open for them */
	    if (levl[u.ux+x][u.uy+y].doormask & D_LOCKED) {
		cmd[0] = C('d');
		return cmd;
	    }
	    if (levl[u.ux+x][u.uy+y].doormask & D_CLOSED) {
		cmd[0] = 'o';
		return cmd;
	    }
	}
	if (levl[u.ux+x][u.uy+y].typ <= SCORR) {
	    cmd[0] = 's';
	    cmd[1] = 0;
	    return cmd;
	}
    }
    } else {
        /* convert without using floating point, allowing sloppy clicking */
        if(x > 2*abs(y))
            x = 1, y = 0;
        else if(y > 2*abs(x))
            x = 0, y = 1;
        else if(x < -2*abs(y))
            x = -1, y = 0;
        else if(y < -2*abs(x))
            x = 0, y = -1;
        else
            x = sgn(x), y = sgn(y);

        if(x == 0 && y == 0)	/* map click on player to "rest" command */
            return ".";

        dir = xytod(x, y);
    }

    /* move, attack, etc. */
    cmd[1] = 0;
    if(mod == CLICK_1) {
	cmd[0] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
    } else {
	cmd[0] = (iflags.num_pad ? M(ndir[dir]) :
		(sdir[dir] - 'a' + 'A')); /* run command */
    }

    return cmd;
}

STATIC_OVL char *
parse()
{
#ifdef LINT	/* static char in_line[COLNO]; */
	char in_line[COLNO];
#else
	static char in_line[COLNO];
#endif
	register int foo;
#ifdef BORG
	char junk_char;
#endif
	static char repeat_char;
	boolean prezero = FALSE;

	multi = 0;
	flags.move = 1;
	flush_screen(1); /* Flush screen buffer. Put the cursor on the hero. */

#ifdef BORG
	if (borg_on) {
	/* KMH -- Danger!  kbhit() is non-standard! */
	   if (!kbhit()) {
	       borg_input();
	       return(borg_line);
	   } else {
		 junk_char = readchar();
		 pline("Cyborg terminated.");
		 borg_on = 0;
	   }

	} else 
#endif
	/* [Tom] for those who occasionally go insane... */
	if (repeat_hit) {
		/* Sanity checks for repeat_hit */
		if (repeat_hit < 0) repeat_hit = 0;
		else {
			/* Don't want things to get too out of hand */
			if (repeat_hit > 10) repeat_hit = 10;
			
			repeat_hit--;
			in_line[0] = repeat_char;
			in_line[1] = 0;
			return (in_line);
		}
	}

	if (!iflags.num_pad || (foo = readchar()) == 'n')
	    for (;;) {
		foo = readchar();
		if (foo >= '0' && foo <= '9') {
		    multi = 10 * multi + foo - '0';
		    if (multi < 0 || multi >= LARGEST_INT) multi = LARGEST_INT;
		    if (multi > 9) {
			clear_nhwindow(WIN_MESSAGE);
			Sprintf(in_line, "Count: %d", multi);
			pline(in_line);
			mark_synch();
		    }
		    last_multi = multi;
		    if (!multi && foo == '0') prezero = TRUE;
		} else break;	/* not a digit */
	    }

	if (foo == '\033') {   /* esc cancels count (TH) */
	    clear_nhwindow(WIN_MESSAGE);
	    /* multi = */ last_multi = 0;  /* WAC multi is cleared later in rhack */
# ifdef REDO
	} else if (foo == DOAGAIN || in_doagain) {
	    multi = last_multi;
	} else {
	    last_multi = multi;
	    savech(0);	/* reset input queue */
	    savech((char)foo);
# endif
	}

	if (multi) {
	    multi--;
	    save_cm = in_line;
	} else {
	    save_cm = (char *)0;
	}
	in_line[0] = foo;
	in_line[1] = '\0';

	if (foo == 'g' || foo == 'G' || foo == 'm' || foo == 'M' ||
	    foo == 'F' || (iflags.num_pad && (foo == '5' || foo == '-'))) {
	    foo = readchar();
#ifdef REDO
	    savech((char)foo);
#endif
	    in_line[1] = foo;
	    in_line[2] = 0;
	}
	clear_nhwindow(WIN_MESSAGE);

	if (prezero) in_line[0] = '\033';
	repeat_char = in_line[0];
	
	return(in_line);
}

#endif /* OVL0 */
#ifdef OVLB

#ifdef UNIX
static
void
end_of_input()
{
#ifndef NOSAVEONHANGUP
	if (!program_state.done_hup++ && program_state.something_worth_saving)
	    (void) dosave0();
#endif
	exit_nhwindows((char *)0);
	clearlocks();
	terminate(EXIT_SUCCESS);
}
#endif

#endif /* OVLB */
#ifdef OVL0

char
readchar()
{
	register int sym;
	int x = u.ux, y = u.uy, mod = 0;

	if ( *readchar_queue )
	    sym = *readchar_queue++;
	else
#ifdef REDO
	    sym = in_doagain ? Getchar() : nh_poskey(&x, &y, &mod);
#else
	    sym = Getchar();
#endif

#ifdef UNIX
# ifdef NR_OF_EOFS
	if (sym == EOF) {
	    register int cnt = NR_OF_EOFS;
	  /*
	   * Some SYSV systems seem to return EOFs for various reasons
	   * (?like when one hits break or for interrupted systemcalls?),
	   * and we must see several before we quit.
	   */
	    do {
		clearerr(stdin);	/* omit if clearerr is undefined */
		sym = Getchar();
	    } while (--cnt && sym == EOF);
	}
# endif /* NR_OF_EOFS */
	if (sym == EOF)
	    end_of_input();
#endif /* UNIX */

	if(sym == 0) {
	    /* click event */
	    readchar_queue = click_to_cmd(x, y, mod);
	    sym = *readchar_queue++;
	}
	return((char) sym);
}

STATIC_PTR int
dotravel()
{

	if (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone()) {
	pline("The travel command is currently unavailable!");
	display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	/* Keyboard travel command */
	static char cmd[2];
	coord cc;

	if (!iflags.travelcmd) return 0;
	cmd[1]=0;
	cc.x = iflags.travelcc.x;
	cc.y = iflags.travelcc.y;
	if (cc.x == -1 && cc.y == -1) {
	    /* No cached destination, start attempt from current position */
	    cc.x = u.ux;
	    cc.y = u.uy;
	}
	pline("Where do you want to travel to?");
	if (getpos(&cc, TRUE, "the desired destination") < 0) {
		/* user pressed ESC */
		return 0;
	}
	iflags.travelcc.x = u.tx = cc.x;
	iflags.travelcc.y = u.ty = cc.y;
	cmd[0] = CMD_TRAVEL;
	readchar_queue = cmd;
	return 0;
}

#ifdef PORT_DEBUG
# ifdef WIN32CON
extern void NDECL(win32con_debug_keystrokes);
extern void NDECL(win32con_handler_info);
# endif

int
wiz_port_debug()
{
	int n, k;
	winid win;
	anything any;
	int item = 'a';
	int num_menu_selections;
	struct menu_selection_struct {
		char *menutext;
		char *portname;
		void NDECL((*fn));
	} menu_selections[] = {
#ifdef WIN32CON
		{"test win32 keystrokes", "tty", win32con_debug_keystrokes},
		{"show keystroke handler information", "tty",
				win32con_handler_info},
#endif
		{(char *)0, (char *)0, (void NDECL((*)))0}/* array terminator */
	};

	num_menu_selections = SIZE(menu_selections) - 1;
	for (k=n=0; k < num_menu_selections; ++k)
		if (!strcmp(menu_selections[k].portname, windowprocs.name))
			n++;
	if (n > 0) {
		menu_item *pick_list;
		win = create_nhwindow(NHW_MENU);
		start_menu(win);
		for (k=0; k < num_menu_selections; ++k) {
			if (strcmp(menu_selections[k].portname,
				   windowprocs.name))
				continue;
			any.a_int = k+1;
			add_menu(win, NO_GLYPH, &any, item++, 0, ATR_NONE,
				menu_selections[k].menutext, MENU_UNSELECTED);
		}
		end_menu(win, "Which port debugging feature?");
		n = select_menu(win, PICK_ONE, &pick_list);
		destroy_nhwindow(win);
		if (n > 0) {
			n = pick_list[0].item.a_int - 1;
			free((genericptr_t) pick_list);
			/* execute the function */
			(*menu_selections[n].fn)();
		}
	} else
		pline("No port-specific debug capability defined.");
	return 0;
}
# endif /*PORT_DEBUG*/

#endif /* OVL0 */
#ifdef OVLB
/*
 *   Parameter validator for generic yes/no function to prevent
 *   the core from sending too long a prompt string to the
 *   window port causing a buffer overflow there.
 */
char
yn_function(query,resp, def)
const char *query,*resp;
char def;
{
	char qbuf[QBUFSZ];
	unsigned truncspot, reduction = sizeof(" [N]  ?") + 1;

	if (resp) reduction += strlen(resp) + sizeof(" () ");
	if (strlen(query) < (QBUFSZ - reduction))
		return (*windowprocs.win_yn_function)(query, resp, def);
	paniclog("Query truncated: ", query);
	reduction += sizeof("...");
	truncspot = QBUFSZ - reduction;
	(void) strncpy(qbuf, query, (int)truncspot);
	qbuf[truncspot] = '\0';
	Strcat(qbuf,"...");
	return (*windowprocs.win_yn_function)(qbuf, resp, def);
}
#endif

/*cmd.c*/

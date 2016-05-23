/*	SCCS Id: @(#)cmd.c	3.4	2003/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <ctype.h>

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

static void FDECL(bind_key, (unsigned char, char*));
static void NDECL(init_bind_list);
static void NDECL(change_bind_list);
#ifdef WIZARD
static void NDECL(add_debug_extended_commands);
#endif /* WIZARD */

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
		/* Show name and text for each command.  Autocompleted
		 * commands are marked with an asterisk ('*'). */
		Sprintf(buf, "  %c %-15s - %s.",
		efp->autocomplete ? '*' : ' ',
		efp->ef_txt, efp->ef_desc);
		putstr(datawin, 0, buf);
	}
	putstr(datawin, 0, "");
	putstr(datawin, 0, "    Commands marked with a * will be autocompleted.");
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
#define MAX_EXT_CMD 200		/* Change if we ever have > 40 ext cmds */  /* So we can only rebind keys up to 200 times?  Although unlikely to ever be encountered, I would call this a bug --Elronnd */
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

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "polnish gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "pol'skiye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "polsha qo'lqop") ) ) {
			if (chanch < 5) chanch = 5;
			chanch *= 2;
		}

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
			identify_pack(1, 0);
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
	if (wizard)	identify_pack(0, 1);
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
enlightenment(final, guaranteed)
int final;	/* 0 => still in progress; 1 => over, survived; 2 => dead */
{
	int ltmp;
	char buf[BUFSZ];
	struct permonst *ptr;

	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, final ? "Final Attributes:" : "Current Attributes:");
	putstr(en_win, 0, "");

#ifdef ELBERETH
	if ((guaranteed || !rn2(10)) && u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
				"the Hand of Elbereth",
				"the Envoy of Balance",
				"the Glory of Arioch"
	    };
	    you_are(hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
#endif

	/* note: piousness 20 matches MIN_QUEST_ALIGN (quest.h) */
	if ((guaranteed || !rn2(10)) && u.ualign.record >= 20)	you_are("piously aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record > 13)	you_are("devoutly aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record > 8)	you_are("fervently aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record > 3)	you_are("stridently aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record == 3)	you_are("aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record > 0)	you_are("haltingly aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record == 0)	you_are("nominally aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record >= -3)	you_have("strayed");
	else if ((guaranteed || !rn2(10)) && u.ualign.record >= -8)	you_have("sinned");
	else if (guaranteed || !rn2(10)) you_have("transgressed");
#ifdef WIZARD
	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1) ) { /* can randomly appear with enlightenment, always after death --Amy */
		Sprintf(buf, " %d", u.ualign.record);
		enl_msg("Your alignment ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1) ) { /* can randomly appear with enlightenment, always after death --Amy */
		Sprintf(buf, " %d", u.alignlim);
		enl_msg("Your max alignment ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1) ) { /* can randomly appear with enlightenment, always after death --Amy */
		Sprintf(buf, " %d sins", u.ualign.sins);
		enl_msg("You ", "carry", "carried", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idscrollpenalty > 100)) {
		Sprintf(buf, " %d", u.idscrollpenalty);
		enl_msg("Scroll identification only ", "works 100 times out of", "worked 100 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idpotionpenalty > 3)) {
		Sprintf(buf, " %d", u.idpotionpenalty);
		enl_msg("Potion identification only ", "works 3 times out of", "worked 3 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idringpenalty > 4)) {
		Sprintf(buf, " %d", u.idringpenalty);
		enl_msg("Ring identification only ", "works 4 times out of", "worked 4 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idamuletpenalty > 15)) {
		Sprintf(buf, " %d", u.idamuletpenalty);
		enl_msg("Amulet identification only ", "works 15 times out of", "worked 15 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idwandpenalty > 3)) {
		Sprintf(buf, " %d", u.idwandpenalty);
		enl_msg("Wand identification only ", "works 3 times out of", "worked 3 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.musableremovechance)) {
		Sprintf(buf, " %d%% chance", 100 - u.musableremovechance);
		enl_msg("Monsters ", "will only drop their musable items with", "only dropped their musable items with", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.bookspawnchance)) {
		Sprintf(buf, " %d%%", 100 - u.bookspawnchance);
		enl_msg("Book drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.scrollspawnchance)) {
		Sprintf(buf, " %d%%", 100 - u.scrollspawnchance);
		enl_msg("Scroll drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.ringspawnchance)) {
		Sprintf(buf, " %d%%", 100 - u.ringspawnchance);
		enl_msg("Ring drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.wandspawnchance)) {
		Sprintf(buf, " %d%%", 100 - u.wandspawnchance);
		enl_msg("Wand drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.amuletspawnchance)) {
		Sprintf(buf, " %d%%", 100 - u.amuletspawnchance);
		enl_msg("Amulet drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.potionspawnchance)) {
		Sprintf(buf, " %d%%", 100 - u.potionspawnchance);
		enl_msg("Potion drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.concealitemchance)) {
		Sprintf(buf, " %d%% of the time only", 100 - u.concealitemchance);
		enl_msg("Concealing monsters ", "are spawned underneath items", "were spawned underneath items", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.usefulitemchance)) {
		Sprintf(buf, " %d%% chance", 100 - u.usefulitemchance);
		enl_msg("Monster death drops ", "spawn with only", "spawned with only", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) { Sprintf(buf, " %d", nartifact_exist() );
		enl_msg("Number of artifacts generated ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.legscratching > 1) { Sprintf(buf, " %d", u.legscratching - 1);
		enl_msg("Your leg damage ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.youpolyamount > 0) { Sprintf(buf, " %d", u.youpolyamount);
		enl_msg("Your remaining amount of free polymorphs ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.antimagicshell > 0) { Sprintf(buf, " %d more turns", u.antimagicshell);
		enl_msg("Your antimagic shell ", "will last", "would have lasted", buf);
	}

	if ((guaranteed || !rn2(10)) && Role_if(PM_UNBELIEVER)) {
		you_are("producing a permanent antimagic shell");
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) { Sprintf(buf, " turn %d", u.monstertimeout);
		enl_msg("Monster spawn increase ", "start at", "would have started at", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {Sprintf(buf, " turn %d", u.monstertimefinish);
		enl_msg("Monster spawn increase ", "reaches its maximum at", "had reached its maximum at", buf);
		enl_msg("In this game, Eevee's evolution ", "is ", "was ", mons[u.eeveelution].mname );
		/* these two are tied together because the monstertimefinish variable defines the evolution --Amy */
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) { Sprintf(buf, " turn %d", u.ascensiontimelimit);
		enl_msg("Your limit for ascension ", "is at", "was at", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		enl_msg("The monster class that cannot be genocided ", "is ", "was ", monexplain[u.ungenocidable] );
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 )) && u.alwaysinvisible) {
		enl_msg("Today, the monster class that always gets generated invisible ", "is ", "was ", monexplain[u.alwaysinvisible] );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", monexplain[u.frequentmonster], u.freqmonsterbonus);
		enl_msg("The monster class that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (u.freqcolorbonus && (wizard || (!rn2(10)) || final >= 1 ))) {
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", c_obj_colors[u.frequentcolor], u.freqcolorbonus);
		enl_msg("The monster color that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies2];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus2);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies3];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus3);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies4];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus4);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies5];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus5);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies6];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus6);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies7];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus7);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies8];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus8);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies9];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus9);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies10];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus10);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies11];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus11);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies12];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus12);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies13];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus13);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies14];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus14);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies15];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus15);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies16];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus16);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies17];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus17);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies18];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus18);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies19];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus19);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies20];
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus20);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies2];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies3];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies4];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies5];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies6];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies7];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies8];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies9];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies10];
		Sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.speedymonster)) {
		Sprintf(buf, "always generated with extra speed: %s", monexplain[u.speedymonster]);
		enl_msg("The monster class that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.musemonster)) {
		Sprintf(buf, "more likely to generate with musable stuff: %s", monexplain[u.musemonster]);
		enl_msg("The monster class that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.minimalism) {Sprintf(buf, " %d", u.minimalism);
		enl_msg("Items generate only 1 time in X, and X ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable].oc_name, obj_descr[u.unobtainable].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable2].oc_name, obj_descr[u.unobtainable2].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable3].oc_name, obj_descr[u.unobtainable3].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable4].oc_name, obj_descr[u.unobtainable4].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable5].oc_name, obj_descr[u.unobtainable5].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable6].oc_name, obj_descr[u.unobtainable6].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && (u.unobtainablegeno != -1) )) {
		Sprintf(buf, "genocided: %s (%s)", obj_descr[u.unobtainablegeno].oc_name, obj_descr[u.unobtainablegeno].oc_descr);
		enl_msg("This item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable].oc_name, obj_descr[u.veryobtainable].oc_descr, u.veryobtainableboost);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable2].oc_name, obj_descr[u.veryobtainable2].oc_descr, u.veryobtainableboost2);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable3].oc_name, obj_descr[u.veryobtainable3].oc_descr, u.veryobtainableboost3);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable4].oc_name, obj_descr[u.veryobtainable4].oc_descr, u.veryobtainableboost4);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem].oc_name, obj_descr[u.alwayscurseditem].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem2].oc_name, obj_descr[u.alwayscurseditem2].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem3].oc_name, obj_descr[u.alwayscurseditem3].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem4].oc_name, obj_descr[u.alwayscurseditem4].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem5].oc_name, obj_descr[u.alwayscurseditem5].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "always invisible: %s", defsyms[trap_to_defsym(u.invisotrap)].explanation);
		enl_msg("The RNG hath decreed that this trap ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		Sprintf(buf, "generated more often: %s (freq bonus %d)", defsyms[trap_to_defsym(u.frequenttrap)].explanation, u.freqtrapbonus);
		enl_msg("The RNG hath decreed that this trap ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {Sprintf(buf, " turn %d", u.next_check);
		enl_msg("Next attribute increase check ", "comes at", "would have come at", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {Sprintf(buf, " %d", AEXE(A_STR));
		enl_msg("Strength training ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {Sprintf(buf, " %d", AEXE(A_DEX));
		enl_msg("Dexterity training ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {Sprintf(buf, " %d", AEXE(A_WIS));
		enl_msg("Wisdom training ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {Sprintf(buf, " %d", AEXE(A_CON));
		enl_msg("Constitution training ", "is", "was", buf);
	}
#endif

	if ((guaranteed || !rn2(10)) && u.negativeprotection) {Sprintf(buf, " %d", u.negativeprotection);
		enl_msg("Your protection was reduced. The amount ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.extralives == 1) {
		you_have("an extra life");
	}

	if ((guaranteed || !rn2(10)) && u.extralives > 1) {Sprintf(buf, " %d", u.extralives);
		enl_msg("Your amount of extra lives ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.chokhmahdamage) {Sprintf(buf, " %d", u.chokhmahdamage);
		enl_msg("The escalating chokhmah attack damage ", "is", "was", buf);
	}

	/*** Resistances to troubles ***/
	if ((guaranteed || !rn2(10)) && Fire_resistance) you_are("fire resistant");
	if ((guaranteed || !rn2(10)) && Cold_resistance) you_are("cold resistant");
	if ((guaranteed || !rn2(10)) && Sleep_resistance) you_are("sleep resistant");
	if ((guaranteed || !rn2(10)) && Disint_resistance) you_are("disintegration-resistant");
	if ((guaranteed || !rn2(10)) && Shock_resistance) you_are("shock resistant");
	if ((guaranteed || !rn2(10)) && Poison_resistance) you_are("poison resistant");
	if ((guaranteed || !rn2(10)) && Drain_resistance) you_are("level-drain resistant");
	if ((guaranteed || !rn2(10)) && Sick_resistance) you_are("immune to sickness");
	if ((guaranteed || !rn2(10)) && Antimagic) you_are("magic-protected");
	if ((guaranteed || !rn2(10)) && Acid_resistance) you_are("acid resistant");
	if ((guaranteed || !rn2(10)) && Fear_resistance) you_are("resistant to fear");
	if ((guaranteed || !rn2(10)) && Stone_resistance)
		you_are("petrification resistant");
	if ((guaranteed || !rn2(10)) && Invulnerable) {
		Sprintf(buf, "invulnerable");
	    if (wizard || (!rn2(10)) || final >= 1  ) Sprintf(eos(buf), " (%d)", Invulnerable);
		you_are(buf);
	}
	/*if (u.urealedibility) you_can("recognize detrimental food");*/

	if ((guaranteed || !rn2(10)) && u.urealedibility) {
		Sprintf(buf, "recognize detrimental food");
	   Sprintf(eos(buf), " (%d)", u.urealedibility);
		you_can(buf);
	}

	/*** Troubles ***/

	if ((guaranteed || !rn2(10)) && multi) {
		Sprintf(buf, "paralyzed.");
	      Sprintf(eos(buf), " (%d)", multi);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && Halluc_resistance)
		enl_msg("You resist", "", "ed", " hallucinations");
	/*if (final) { */
	if ((guaranteed || !rn2(10)) && Hallucination) {
		if (HeavyHallu) Sprintf(buf, "badly hallucinating");
		else Sprintf(buf, "hallucinating");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HHallucination);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Stunned) {
		if (HeavyStunned) Sprintf(buf, "badly stunned");
		else Sprintf(buf, "stunned");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HStun);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Confusion) {
		if (HeavyConfusion) Sprintf(buf, "badly confused");
		else Sprintf(buf, "confused");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HConfusion);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Blind) {
		if (HeavyBlind) Sprintf(buf, "badly blinded");
		else Sprintf(buf, "blinded");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Blinded);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Sick) {
		Sprintf(buf, "sick");
			if (u.usick_type & SICK_VOMITABLE) Sprintf(eos(buf), " from food poisoning");
			if (u.usick_type & SICK_NONVOMITABLE) Sprintf(eos(buf), " from illness");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Sick);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Vomiting) {
		Sprintf(buf, "nauseated");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Vomiting);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Feared) {
		if (HeavyFeared) Sprintf(buf, "stricken with very strong fear");
		else Sprintf(buf, "stricken with fear");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HFeared);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Numbed) {
		if (HeavyNumbed) Sprintf(buf, "badly numbed");
		else Sprintf(buf, "numbed");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HNumbed);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Frozen) {
		if (HeavyFrozen) Sprintf(buf, "frozen rigid and solid");
		else Sprintf(buf, "frozen solid");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HFrozen);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.hanguppenalty) {
		Sprintf(buf, "temporarily slower because you tried to hangup cheat");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.hanguppenalty);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && Burned) {
		if (HeavyBurned) Sprintf(buf, "badly burned");
		else Sprintf(buf, "burned");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HBurned);
		you_are(buf);
	}
		
#ifdef CONVICT
        if ((guaranteed || !rn2(10)) && Punished) {
            you_are("punished");
        }
#endif /* CONVICT */

        if ((guaranteed || !rn2(10)) && u.totter) {
            you_have("inverted directional keys");
        }

        if ((guaranteed || !rn2(10)) && sengr_at("Elbereth", u.ux, u.uy) ) {
            you_are("standing on an active Elbereth engraving");
        }

	if ((guaranteed || !rn2(10)) && Stoned) {
		Sprintf(buf, "turning to stone");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Stoned);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Slimed) {
		Sprintf(buf, "turning into slime");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Slimed);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Strangled) {
		Sprintf(buf, (u.uburied) ? "buried" : "being strangled");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Strangled);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Prem_death) {
		Sprintf(buf, "going to die prematurely");
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && IsGlib) {
		Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Glib);
		you_have(buf);
	}


	if ((guaranteed || !rn2(10)) && (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone())) {
		Sprintf(buf, "the menu bug.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MenuBug);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone())) {
		Sprintf(buf, "the speed bug.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", SpeedBug);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Superscroller || u.uprops[SUPERSCROLLER_ACTIVE].extrinsic || have_superscrollerstone())) {
		Sprintf(buf, "the superscroller.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Superscroller);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (RMBLoss || u.uprops[RMB_LOST].extrinsic || have_rmbstone())) {
		Sprintf(buf, "the following problem: Your right mouse button failed.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RMBLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (DisplayLoss || u.uprops[DISPLAY_LOST].extrinsic || have_displaystone())) {
		Sprintf(buf, "the following problem: Your display failed.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", DisplayLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone())) {
		Sprintf(buf, "the following problem: You lost spells.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", SpellLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone())) {
		Sprintf(buf, "the following problem: Your spells became yellow.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", YellowSpells);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || have_autodestructstone())) {
		Sprintf(buf, "the following problem: An auto destruct mechanism was initiated.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AutoDestruct);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (MemoryLoss || u.uprops[MEMORY_LOST].extrinsic || have_memorylossstone())) {
		Sprintf(buf, "the following problem: There is low local memory.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MemoryLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (InventoryLoss || u.uprops[INVENTORY_LOST].extrinsic || have_inventorylossstone())) {
		Sprintf(buf, "the following problem: There is not enough memory to create an inventory window.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", InventoryLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (BlackNgWalls || u.uprops[BLACK_NG_WALLS].extrinsic || have_blackystone())) {
		Sprintf(buf, "the following problem: Blacky and his NG walls are closing in.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BlackNgWalls);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (FreeHandLoss || u.uprops[FREE_HAND_LOST].extrinsic || have_freehandbugstone())) {
		Sprintf(buf, "the following problem: Your free hand is less likely to be free.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FreeHandLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Unidentify || u.uprops[UNIDENTIFY].extrinsic || have_unidentifystone())) {
		Sprintf(buf, "the following problem: Your possessions sometimes unidentify themselves.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Unidentify);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Thirst || u.uprops[THIRST].extrinsic || have_thirststone())) {
		Sprintf(buf, "a strong sense of thirst.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Thirst);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (LuckLoss || u.uprops[LUCK_LOSS].extrinsic || have_unluckystone())) {
		Sprintf(buf, "the following problem: You're shitting out of luck (SOL).");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", LuckLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (ShadesOfGrey || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone())) {
		Sprintf(buf, "the following problem: Everything displays in various shades of grey.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", ShadesOfGrey);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (FaintActive || u.uprops[FAINT_ACTIVE].extrinsic || have_faintingstone())) {
		Sprintf(buf, "the following problem: You randomly faint.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FaintActive);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Itemcursing || u.uprops[ITEMCURSING].extrinsic || have_cursingstone() || have_primecurse() )) {
		Sprintf(buf, "the following problem: Your inventory gradually fills up with cursed items.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Itemcursing);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (DifficultyIncreased || u.uprops[DIFFICULTY_INCREASED].extrinsic || have_difficultystone())) {
		Sprintf(buf, "the following problem: The difficulty of the game was arbitrarily increased.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", DifficultyIncreased);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Deafness || u.uprops[DEAFNESS].extrinsic || have_deafnessstone())) {
		Sprintf(buf, "a hearing break.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Deafness);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (CasterProblem || u.uprops[CASTER_PROBLEM].extrinsic || have_antimagicstone())) {
		Sprintf(buf, "blood mana.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", CasterProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (WeaknessProblem || u.uprops[WEAKNESS_PROBLEM].extrinsic || have_weaknessstone())) {
		Sprintf(buf, "the following problem: Being weak from hunger damages your health.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", WeaknessProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (RotThirteen || u.uprops[ROT_THIRTEEN].extrinsic || have_rotthirteenstone())) {
		Sprintf(buf, "the following problem: A rot13 cypher has been activated for lowercase letters.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RotThirteen);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone())) {
		Sprintf(buf, "the following problem: You cannot move diagonally.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BishopGridbug);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (ConfusionProblem || u.uprops[CONFUSION_PROBLEM].extrinsic || have_confusionstone())) {
		Sprintf(buf, "a confusing problem.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", ConfusionProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (NoDropProblem || u.uprops[DROP_BUG].extrinsic || have_dropbugstone())) {
		Sprintf(buf, "the following problem: You cannot drop items.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", NoDropProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (DSTWProblem || u.uprops[DSTW_BUG].extrinsic || have_dstwstone())) {
		Sprintf(buf, "the following problem: Your potions don't always work.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", DSTWProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone())) {
		Sprintf(buf, "the following problem: You can't cure status effects.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", StatusTrapProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (AlignmentProblem || u.uprops[ALIGNMENT_FAILURE].extrinsic || have_alignmentstone())) {
		Sprintf(buf, "the following problem: Your maximum alignment will decrease over time.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AlignmentProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (StairsProblem || u.uprops[STAIRSTRAP].extrinsic || have_stairstrapstone())) {
		Sprintf(buf, "the following problem: Stairs are always trapped.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", StairsProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (UninformationProblem || u.uprops[UNINFORMATION].extrinsic || have_uninformationstone())) {
		Sprintf(buf, "the following problem: The game gives insufficient amounts of information.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", UninformationProblem);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CaptchaProblem || u.uprops[CAPTCHA].extrinsic || have_captchastone())) {
		Sprintf(buf, "the following problem: You sometimes have to solve captchas.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", CaptchaProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (FarlookProblem || u.uprops[FARLOOK_BUG].extrinsic || have_farlookstone())) {
		Sprintf(buf, "the following problem: Farlooking peaceful monsters angers them.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FarlookProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (RespawnProblem || u.uprops[RESPAWN_BUG].extrinsic || have_respawnstone())) {
		Sprintf(buf, "the following problem: Killing monsters will cause them to respawn somewhere on the level.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RespawnProblem);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BigscriptEffect || u.uprops[BIGSCRIPT].extrinsic || have_bigscriptstone())) {
		Sprintf(buf, "BIGscript.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BigscriptEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (BankTrapEffect || u.uprops[BANKBUG].extrinsic || have_bankstone())) {
		Sprintf(buf, "the following problem: Your money will wander into a mysterious bank.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BankTrapEffect);
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (amount stored: %d)", u.bankcashamount);
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (money limit: %d)", u.bankcashlimit);
		you_have(buf);
	} else {
	  if ((guaranteed || !rn2(10)) && u.bankcashamount) {
	    if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "the following amount of cash stored in the bank:");
		Sprintf(eos(buf), " %d", u.bankcashamount);
		you_have(buf);
	    }
	  }
	  if ((guaranteed || !rn2(10)) && u.bankcashlimit) {
	    if (wizard || (!rn2(10)) || final >= 1 ) {
		Sprintf(buf, "the following bank cash limit:");
		Sprintf(eos(buf), " %d", u.bankcashlimit);
		you_have(buf);
	    }
	  }
	}
	if ((guaranteed || !rn2(10)) && (MapTrapEffect || u.uprops[MAPBUG].extrinsic || have_mapstone())) {
		Sprintf(buf, "the following problem: The map doesn't display correctly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MapTrapEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (TechTrapEffect || u.uprops[TECHBUG].extrinsic || have_techniquestone())) {
		Sprintf(buf, "the following problem: Your techniques don't work.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TechTrapEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (RecurringDisenchant || u.uprops[RECURRING_DISENCHANT].extrinsic || have_disenchantmentstone())) {
		Sprintf(buf, "the following problem: Your possessions disenchant themselves spontaneously.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RecurringDisenchant);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (verisiertEffect || u.uprops[VERISIERTEFFECT].extrinsic || have_verisiertstone())) {
		Sprintf(buf, "the following problem: Monster respawn speeds up rapidly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", verisiertEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (ChaosTerrain || u.uprops[CHAOS_TERRAIN].extrinsic || have_chaosterrainstone())) {
		Sprintf(buf, "the following problem: The terrain slowly becomes ever more chaotic.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", ChaosTerrain);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone())) {
		Sprintf(buf, "the following problem: You're completely unable to cast spells.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Muteness);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (EngravingDoesntWork || u.uprops[ENGRAVINGBUG].extrinsic || have_engravingstone())) {
		Sprintf(buf, "the following problem: Monsters don't respect Elbereth.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", EngravingDoesntWork);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (MagicDeviceEffect || u.uprops[MAGIC_DEVICE_BUG].extrinsic || have_magicdevicestone())) {
		Sprintf(buf, "the following problem: Zapping a wand can cause it to explode.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MagicDeviceEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (BookTrapEffect || u.uprops[BOOKBUG].extrinsic || have_bookstone())) {
		Sprintf(buf, "the following problem: Reading spellbooks confuses you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BookTrapEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (LevelTrapEffect || u.uprops[LEVELBUG].extrinsic || have_levelstone())) {
		Sprintf(buf, "the following problem: Monsters become stronger if many of their species have been generated already.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", LevelTrapEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (QuizTrapEffect || u.uprops[QUIZZES].extrinsic || have_quizstone())) {
		Sprintf(buf, "the following problem: You have to partake in the Great NetHack Quiz.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", QuizTrapEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FastMetabolismEffect || u.uprops[FAST_METABOLISM].extrinsic || have_metabolicstone())) {
		Sprintf(buf, "the following problem: Your food consumption is much faster.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FastMetabolismEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone())) {
		Sprintf(buf, "the following problem: You cannot teleport at all.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", NoReturnEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AlwaysEgotypeMonsters || u.uprops[ALWAYS_EGOTYPES].extrinsic || have_egostone())) {
		Sprintf(buf, "the following problem: Monsters always spawn with egotypes.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AlwaysEgotypeMonsters);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (TimeGoesByFaster || u.uprops[FAST_FORWARD].extrinsic || have_fastforwardstone())) {
		Sprintf(buf, "the following problem: Time goes by faster.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TimeGoesByFaster);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone())) {
		Sprintf(buf, "the following problem: Your food is always rotten.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", FoodIsAlwaysRotten);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone())) {
		Sprintf(buf, "the following problem: Your skills are deactivated.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AllSkillsUnskilled);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AllStatsAreLower || u.uprops[STATS_LOWERED].extrinsic || have_lowstatstone())) {
		Sprintf(buf, "the following problem: All of your stats are lowered.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AllStatsAreLower);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (PlayerCannotTrainSkills || u.uprops[TRAINING_DEACTIVATED].extrinsic || have_trainingstone())) {
		Sprintf(buf, "the following problem: You cannot train skills.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", PlayerCannotTrainSkills);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (PlayerCannotExerciseStats || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone())) {
		Sprintf(buf, "the following problem: You cannot exercise your stats.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", PlayerCannotExerciseStats);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && MCReduction) {
		Sprintf(buf, "reduced magic cancellation.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", MCReduction);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[INTRINSIC_LOSS].extrinsic || IntrinsicLossProblem || have_intrinsiclossstone() )) {
		Sprintf(buf, "a case of random intrinsic loss.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", IntrinsicLossProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && u.uprops[TRAP_REVEALING].extrinsic) {
		Sprintf(buf, "randomly revealing traps");
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[BLOOD_LOSS].extrinsic || BloodLossProblem || have_bloodlossstone() )) {
		Sprintf(buf, "bleeding out");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BloodLossProblem);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[NASTINESS_EFFECTS].extrinsic || NastinessProblem || have_nastystone() )) {
		Sprintf(buf, "subjected to random nasty trap effects");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", NastinessProblem);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[BAD_EFFECTS].extrinsic || BadEffectProblem || have_badeffectstone() )) {
		Sprintf(buf, "subjected to random bad effects");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", BadEffectProblem);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[AUTOMATIC_TRAP_CREATION].extrinsic || TrapCreationProblem || have_trapcreationstone() )) {
		Sprintf(buf, "more likely to encounter traps");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TrapCreationProblem);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && u.uprops[SENTIENT_HIGH_HEELS].extrinsic) {
		Sprintf(buf, "wearing sentient high heels");
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[REPEATING_VULNERABILITY].extrinsic || AutomaticVulnerabilitiy || have_vulnerabilitystone() )) {
		Sprintf(buf, "the risk of temporarily losing intrinsics");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", AutomaticVulnerabilitiy);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[TELEPORTING_ITEMS].extrinsic || TeleportingItems || have_itemportstone() )) {
		Sprintf(buf, "teleporting items");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TeleportingItems);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[RECURRING_AMNESIA].extrinsic || RecurringAmnesia || have_amnesiastone() )) {
		Sprintf(buf, "going to suffer from amnesia now and then");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RecurringAmnesia);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[TURNLIMITATION].extrinsic || TurnLimitation || have_limitationstone() )) {
		Sprintf(buf, "getting your ascension turn limit reduced whenever you take damage");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TurnLimitation);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[WEAKSIGHT].extrinsic || WeakSight || have_weaksightstone() )) {
		Sprintf(buf, "short-sighted");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", WeakSight);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[RANDOM_MESSAGES].extrinsic || RandomMessages || have_messagestone() )) {
		Sprintf(buf, "reading random messages");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", RandomMessages);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[DESECRATION].extrinsic || Desecration || have_nonsacredstone() )) {
		Sprintf(buf, "the following problem: Altars malfunction whenever you try to use them");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Desecration);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[STARVATION_EFFECT].extrinsic || StarvationEffect || have_starvationstone() )) {
		Sprintf(buf, "not going to get corpses from defeated enemies");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", StarvationEffect);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[NO_DROPS_EFFECT].extrinsic || NoDropsEffect || have_droplessstone() )) {
		Sprintf(buf, "not going to get death drops from monsters");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", NoDropsEffect);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[LOW_EFFECTS].extrinsic || LowEffects || have_loweffectstone() )) {
		Sprintf(buf, "reduced power of magical effects");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", LowEffects);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[INVIS_TRAPS_EFFECT].extrinsic || InvisibleTrapsEffect || have_invisostone() )) {
		Sprintf(buf, "not going to see traps");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", InvisibleTrapsEffect);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[GHOST_WORLD].extrinsic || GhostWorld || have_ghostlystone() )) {
		Sprintf(buf, "in a ghost world");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", GhostWorld);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() )) {
		Sprintf(buf, "going to become dehydrated");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Dehydration);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[HATE_TRAP_EFFECT].extrinsic || HateTrapEffect || have_hatestone() )) {
		Sprintf(buf, "the following problem: Pets hate you with a fiery passion.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HateTrapEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[TOTTER_EFFECT].extrinsic || TotterTrapEffect || have_directionswapstone() )) {
		Sprintf(buf, "the following problem: Your directional keys got swapped.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", TotterTrapEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )) {
		Sprintf(buf, "unable to get intrinsics from eating corpses");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Nonintrinsics);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[DROPCURSES_EFFECT].extrinsic || Dropcurses || have_dropcursestone() )) {
		Sprintf(buf, "the following problem: Dropping items causes them to autocurse.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Dropcurses);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[NAKEDNESS].extrinsic || Nakedness || have_nakedstone() )) {
		Sprintf(buf, "effectively naked");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Nakedness);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[ANTILEVELING].extrinsic || Antileveling || have_antilevelstone() )) {
		Sprintf(buf, "unable to gain experience");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Antileveling);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || have_stealerstone() )) {
		Sprintf(buf, "more likely to have your items stolen");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", ItemStealingEffect);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[REBELLION_EFFECT].extrinsic || Rebellions || have_rebelstone() )) {
		Sprintf(buf, "the following problem: Pets can spontaneously rebel and become hostile.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", Rebellions);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[CRAP_EFFECT].extrinsic || CrapEffect || have_shitstone() )) {
		Sprintf(buf, "diarrhea");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", CrapEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[PROJECTILES_MISFIRE].extrinsic || ProjectilesMisfire || have_misfirestone() )) {
		Sprintf(buf, "the following problem: Your projectiles often misfire.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", ProjectilesMisfire);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[WALL_TRAP_EFFECT].extrinsic || WallTrapping || have_wallstone() )) {
		Sprintf(buf, "the following problem: Dungeon regrowth is excessively fast.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", WallTrapping);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.uprops[RANDOM_RUMORS].extrinsic) {
		Sprintf(buf, "going to listen to random rumors");
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && IncreasedGravity) {
		Sprintf(buf, "increased encumbrance due to a stronger gravity.");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", IncreasedGravity);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && NoStaircase) {
		Sprintf(buf, "to wait until you can use staircases again.");
	      Sprintf(eos(buf), " (%d)", NoStaircase);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && TimeStopped) {
		Sprintf(buf, "stopped the flow of time.");
	      Sprintf(eos(buf), " (%d)", TimeStopped);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && UseTheForce) {
		Sprintf(buf, "able to use the force like a true jedi");
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.uprops[SENSORY_DEPRIVATION].extrinsic) {
		Sprintf(buf, "sensory deprivation.");
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && NoFire_resistance) {
		Sprintf(buf, "prevented from having fire resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FIRE_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoCold_resistance) {
		Sprintf(buf, "prevented from having cold resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_COLD_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSleep_resistance) {
		Sprintf(buf, "prevented from having sleep resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLEEP_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoDisint_resistance) {
		Sprintf(buf, "prevented from having disintegration resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISINT_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoShock_resistance) {
		Sprintf(buf, "prevented from having shock resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SHOCK_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoPoison_resistance) {
		Sprintf(buf, "prevented from having poison resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_POISON_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoDrain_resistance) {
		Sprintf(buf, "prevented from having drain resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRAIN_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSick_resistance) {
		Sprintf(buf, "prevented from having sickness resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICK_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoAntimagic_resistance) {
		Sprintf(buf, "prevented from having magic resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_ANTIMAGIC].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoAcid_resistance) {
		Sprintf(buf, "prevented from having acid resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_ACID_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoStone_resistance) {
		Sprintf(buf, "prevented from having petrification resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONE_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoFear_resistance) {
		Sprintf(buf, "prevented from having fear resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSee_invisible) {
		Sprintf(buf, "prevented from having see invisible");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEE_INVIS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoTelepat) {
		Sprintf(buf, "prevented from having telepathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPAT].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoWarning) {
		Sprintf(buf, "prevented from having warning");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_WARNING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSearching) {
		Sprintf(buf, "prevented from having automatic searching");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEARCHING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoClairvoyant) {
		Sprintf(buf, "prevented from having clairvoyance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_CLAIRVOYANT].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoInfravision) {
		Sprintf(buf, "prevented from having infravision");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_INFRAVISION].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoDetect_monsters) {
		Sprintf(buf, "prevented from having detect monsters");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DETECT_MONSTERS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoInvisible) {
		Sprintf(buf, "prevented from having invisibility");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_INVIS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoDisplaced) {
		Sprintf(buf, "prevented from having displacement");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISPLACED].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoStealth) {
		Sprintf(buf, "prevented from having stealth");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STEALTH].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoJumping) {
		Sprintf(buf, "prevented from having jumping");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_JUMPING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoTeleport_control) {
		Sprintf(buf, "prevented from having teleport control");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPORT_CONTROL].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoFlying) {
		Sprintf(buf, "prevented from having flying");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FLYING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoBreathless) {
		Sprintf(buf, "prevented from having magical breathing");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_MAGICAL_BREATHING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoPasses_walls) {
		Sprintf(buf, "prevented from having phasing");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_PASSES_WALLS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSlow_digestion) {
		Sprintf(buf, "prevented from having slow digestion");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLOW_DIGESTION].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoHalf_spell_damage) {
		Sprintf(buf, "prevented from having half spell damage");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_SPDAM].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoHalf_physical_damage) {
		Sprintf(buf, "prevented from having half physical damage");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_PHDAM].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoRegeneration) {
		Sprintf(buf, "prevented from having regeneration");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_REGENERATION].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoEnergy_regeneration) {
		Sprintf(buf, "prevented from having mana regeneration");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_ENERGY_REGENERATION].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoPolymorph_control) {
		Sprintf(buf, "prevented from having polymorph control");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoFast) {
		Sprintf(buf, "prevented from having speed");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FAST].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoReflecting) {
		Sprintf(buf, "prevented from having reflection");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_REFLECTING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoKeen_memory) {
		Sprintf(buf, "prevented from having keen memory");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_KEEN_MEMORY].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSight_bonus) {
		Sprintf(buf, "prevented from having a sight bonus");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SIGHT_BONUS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoVersus_curses) {
		Sprintf(buf, "prevented from having curse resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_VERSUS_CURSES].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoStun_resist) {
		Sprintf(buf, "prevented from having stun resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUN_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoConf_resist) {
		Sprintf(buf, "prevented from having confusion resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_CONF_RES].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoPsi_resist) {
		Sprintf(buf, "prevented from having psi resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_PSI_RES].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoDouble_attack) {
		Sprintf(buf, "prevented from having double attacks");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DOUBLE_ATTACK].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoQuad_attack) {
		Sprintf(buf, "prevented from having quad attacks");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_QUAD_ATTACK].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoFree_action) {
		Sprintf(buf, "prevented from having free action");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREE_ACTION].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoHallu_party) {
		Sprintf(buf, "prevented from hallu partying");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALLU_PARTY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoDrunken_boxing) {
		Sprintf(buf, "prevented from drunken boxing");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRUNKEN_BOXING].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoStunnopathy) {
		Sprintf(buf, "prevented from having stunnopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUNNOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoNumbopathy) {
		Sprintf(buf, "prevented from having numbopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_NUMBOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoFreezopathy) {
		Sprintf(buf, "prevented from having freezopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREEZOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoStoned_chiller) {
		Sprintf(buf, "prevented from being a stoned chiller");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONED_CHILLER].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoCorrosivity) {
		Sprintf(buf, "prevented from having corrosivity");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_CORROSIVITY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoFear_factor) {
		Sprintf(buf, "prevented from having an increased fear factor");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_FACTOR].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoBurnopathy) {
		Sprintf(buf, "prevented from having burnopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_BURNOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoSickopathy) {
		Sprintf(buf, "prevented from having sickopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICKOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoWonderlegs) {
		Sprintf(buf, "prevented from having wonderlegs");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_WONDERLEGS].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoGlib_combat) {
		Sprintf(buf, "prevented from having glib combat");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_GLIB_COMBAT].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoUseTheForce) {
		Sprintf(buf, "prevented from using the force like a real jedi");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.uprops[DEAC_THE_FORCE].intrinsic);
		you_are(buf);
	}

	int shieldblockrate = 0;

	if ((guaranteed || !rn2(10)) && uarms) {

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
		case COPPER_DRAGON_SCALE_SHIELD:
		case PLATINUM_DRAGON_SCALE_SHIELD:
		case BRASS_DRAGON_SCALE_SHIELD:
		case AMETHYST_DRAGON_SCALE_SHIELD:
		case PURPLE_DRAGON_SCALE_SHIELD:
		case DIAMOND_DRAGON_SCALE_SHIELD:
		case EMERALD_DRAGON_SCALE_SHIELD:
		case SAPPHIRE_DRAGON_SCALE_SHIELD:
		case RUBY_DRAGON_SCALE_SHIELD:
		case GREEN_DRAGON_SCALE_SHIELD:
		case GOLDEN_DRAGON_SCALE_SHIELD:
		case STONE_DRAGON_SCALE_SHIELD:
		case CYAN_DRAGON_SCALE_SHIELD:
		case PSYCHIC_DRAGON_SCALE_SHIELD:
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

	if ((guaranteed || !rn2(10)) && Fumbling) enl_msg("You fumble", "", "d", "");
	if ((guaranteed || !rn2(10)) && (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  )) {
		Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HWounded_legs);
		you_have(buf);
	}
#if defined(WIZARD) && defined(STEED) /*randomly tell this to the player outside of wizard mode too --Amy */
	if ((guaranteed || !rn2(10)) && (Wounded_legs && u.usteed && (wizard || !rn2(10) || final >= 1 ))) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    enl_msg(buf, " has", " had", " wounded legs");
	}
#endif
	if ((guaranteed || !rn2(10)) && Sleeping) enl_msg("You ", "fall", "fell", " asleep");
	if ((guaranteed || !rn2(10)) && Hunger) enl_msg("You hunger", "", "ed", " rapidly");

	if ((guaranteed || !rn2(10)) && have_sleepstone()) enl_msg("You ", "are", "were", " very tired");
	if ((guaranteed || !rn2(10)) && have_cursedmagicresstone()) enl_msg("You ", "take", "took", " double damage");

	/*** Vision and senses ***/
	if ((guaranteed || !rn2(10)) && See_invisible) enl_msg(You_, "see", "saw", " invisible");
	if ((guaranteed || !rn2(10)) && Blind_telepat) you_are("telepathic");

	if ((guaranteed || !rn2(10)) && Hallu_party) you_are("hallu partying");
	if ((guaranteed || !rn2(10)) && Drunken_boxing) you_are("a drunken boxer");
	if ((guaranteed || !rn2(10)) && Stunnopathy) you_are("stunnopathic");
	if ((guaranteed || !rn2(10)) && Numbopathy) you_are("numbopathic");
	if ((guaranteed || !rn2(10)) && Freezopathy) you_are("freezopathic");
	if ((guaranteed || !rn2(10)) && Stoned_chiller) you_are("a stoned chiller");
	if ((guaranteed || !rn2(10)) && Corrosivity) you_are("extremely corrosive");
	if ((guaranteed || !rn2(10)) && Fear_factor) you_have("an increased fear factor");
	if ((guaranteed || !rn2(10)) && Burnopathy) you_are("burnopathic");
	if ((guaranteed || !rn2(10)) && Sickopathy) you_are("sickopathic");
	if ((guaranteed || !rn2(10)) && Wonderlegs) you_have("wonderlegs");
	if ((guaranteed || !rn2(10)) && Glib_combat) you_are("a glibbery fighter");

	if ((guaranteed || !rn2(10)) && Warning) you_are("warned");
	if ((guaranteed || !rn2(10)) && (Warn_of_mon && flags.warntype)) {
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
	if ((guaranteed || !rn2(10)) && Undead_warning) you_are("warned of undead");
#endif

	if ((guaranteed || !rn2(10)) && (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) )) you_are("aware of the presence of topmodels");
	if ((guaranteed || !rn2(10)) && (Role_if(PM_ACTIVISTOR) && uwep && is_quest_artifact(uwep) )) you_are("aware of the presence of unique monsters");
	/*if (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING ) you_are("aware of the presence of undead");
	if ((guaranteed || !rn2(10)) && (uarmh && uarmh->otyp == HELMET_OF_UNDEAD_WARNING )) you_are("aware of the presence of undead");*/
	if ((guaranteed || !rn2(10)) && (uamul && uamul->otyp == AMULET_OF_POISON_WARNING )) you_are("aware of the presence of poisonous monsters");
	if ((guaranteed || !rn2(10)) && (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING )) you_are("aware of the presence of same-race monsters");
	if ((guaranteed || !rn2(10)) && (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING )) you_are("aware of the presence of covetous monsters");
	if ((guaranteed || !rn2(10)) && Role_if(PM_PALADIN) ) you_are("aware of the presence of demons");
	if ((guaranteed || !rn2(10)) && Race_if(PM_VORTEX) ) you_are("aware of the presence of unsolid creatures");
	if ((guaranteed || !rn2(10)) && Race_if(PM_VORTEX) ) you_are("aware of the presence of creatures without limbs");
	if ((guaranteed || !rn2(10)) && Race_if(PM_CORTEX) ) you_are("aware of the presence of unsolid creatures");
	if ((guaranteed || !rn2(10)) && Race_if(PM_CORTEX) ) you_are("aware of the presence of creatures without limbs");
	if ((guaranteed || !rn2(10)) && Race_if(PM_LEVITATOR) ) you_are("aware of the presence of flying monsters");
	if ((guaranteed || !rn2(10)) && Race_if(PM_RODNEYAN) ) you_are("able to sense monsters possessing coveted objects");
	if ((guaranteed || !rn2(10)) && isselfhybrid) you_are("aware of the presence of strong wanderers");
	if ((guaranteed || !rn2(10)) && isselfhybrid) you_are("aware of the presence of monsters that are valid polymorph forms for monsters only");

	if ((guaranteed || !rn2(10)) && Searching) you_have("automatic searching");

	if ((guaranteed || !rn2(10)) && Clairvoyant) {
		Sprintf(buf, "clairvoyant");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HClairvoyant);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && Infravision) you_have("infravision");

	if ((guaranteed || !rn2(10)) && u.banishmentbeam) you_are("going to be banished");
	if ((guaranteed || !rn2(10)) && u.levelporting) you_are("going to be levelported");

	if ((guaranteed || !rn2(10)) && u.inertia) {
		Sprintf(buf, "slowed by inertia");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.inertia);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.temprecursion) {
		Sprintf(buf, "under the effect of temporary recursion");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", u.temprecursiontime);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && Detect_monsters) {
		Sprintf(buf, "sensing the presence of monsters");
	    if (wizard || (!rn2(10)) || final >= 1 ) Sprintf(eos(buf), " (%d)", HDetect_monsters);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.umconf) you_are("going to confuse monsters");

	if (guaranteed || !rn2(10)) {
		Sprintf(buf, "%d points of nutrition remaining", u.uhunger); you_have(buf);
	}

	/*** Appearance and behavior ***/
	if ((guaranteed || !rn2(10)) && Adornment) {
	    int adorn = 0;

	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		you_are("poorly adorned");
	    else
		you_are("adorned");
	}
	if ((guaranteed || !rn2(10)) && Invisible) you_are("invisible");
	else if ((guaranteed || !rn2(10)) && Invis) you_are("invisible to others");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((guaranteed || !rn2(10)) && ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis))
	    you_are("visible");
	if ((guaranteed || !rn2(10)) && Displaced) you_are("displaced");
	if ((guaranteed || !rn2(10)) && Stealth) you_are("stealthy");
	if ((guaranteed || !rn2(10)) && Aggravate_monster) enl_msg("You aggravate", "", "d", " monsters");
	if ((guaranteed || !rn2(10)) && Conflict) enl_msg("You cause", "", "d", " conflict");

	/*** Transportation ***/
	if ((guaranteed || !rn2(10)) && Jumping) you_can("jump");
	if ((guaranteed || !rn2(10)) && Teleportation) you_have("teleportitis");
	if ((guaranteed || !rn2(10)) && Teleport_control) you_have("teleport control");
	/*if (Lev_at_will) you_are("levitating, at will");
	else if ((guaranteed || !rn2(10)) && Levitation) you_are("levitating");*/	/* without control */

	if ((guaranteed || !rn2(10)) && Lev_at_will)  {
	    Sprintf(buf, "levitating, at will");
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) Sprintf(eos(buf), " (%d)", HLevitation);
#endif /*same like above --Amy */
	    you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Levitation)  {
	    Sprintf(buf, "levitating");
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) Sprintf(eos(buf), " (%d)", HLevitation);
#endif /*same like above --Amy */
	    you_are(buf);
	}

	else if ((guaranteed || !rn2(10)) && Flying) you_can("fly");
	if ((guaranteed || !rn2(10)) && Wwalking) you_can("walk on water");
	if ((guaranteed || !rn2(10)) && Swimming) you_can("swim");        
	if ((guaranteed || !rn2(10)) && Breathless) you_can("survive without air");
	else if ((guaranteed || !rn2(10)) && Amphibious) you_can("breathe water");
	if ((guaranteed || !rn2(10)) && Passes_walls) you_can("walk through walls");
#ifdef STEED
	/* If you die while dismounting, u.usteed is still set.  Since several
	 * places in the done() sequence depend on u.usteed, just detect this
	 * special case. */
	if ((guaranteed || !rn2(10)) && (u.usteed && (final < 2 || strcmp(killer, "riding accident")))) {
	    Sprintf(buf, "riding %s", y_monnam(u.usteed));
	    you_are(buf);
	}
#endif
	if ((guaranteed || !rn2(10)) && u.uswallow) {
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif /*same like above --Amy */
	    you_are(buf);
	} else if ((guaranteed || !rn2(10)) && u.ustuck) {
	    Sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by",
		    a_monnam(u.ustuck));
	    you_are(buf);
	}

	/*** Physical attributes ***/
	if ((guaranteed || !rn2(10)) && u.uhitinc)
	    you_have(enlght_combatinc("to hit", u.uhitinc, final, buf));
	if ((guaranteed || !rn2(10)) && u.udaminc)
	    you_have(enlght_combatinc("damage", u.udaminc, final, buf));
	if ((guaranteed || !rn2(10)) && Slow_digestion) you_have("slower digestion");
	if ((guaranteed || !rn2(10)) && Keen_memory)
		enl_msg("Your memory ", "is", "was", " keen");
	if ((guaranteed || !rn2(10)) && Sight_bonus) enl_msg("Your sight ", "is", "was", " improved");
	if ((guaranteed || !rn2(10)) && Versus_curses) you_have("curse resistance");

	if ((guaranteed || !rn2(10)) && Stun_resist) you_have("stun resistance");
	if ((guaranteed || !rn2(10)) && Conf_resist) you_have("confusion resistance");
	if ((guaranteed || !rn2(10)) && Psi_resist) you_have("psi resistance");
	if ((guaranteed || !rn2(10)) && Extra_wpn_practice) enl_msg("You ", "can", "could", " train skills and attributes faster");
	if ((guaranteed || !rn2(10)) && Death_resistance) you_have("resistance to death rays");
	if ((guaranteed || !rn2(10)) && Double_attack) you_have("double attacks");
	if ((guaranteed || !rn2(10)) && Quad_attack) you_have("quad attacks");

	if ((guaranteed || !rn2(10)) && Half_physical_damage) you_have("physical resistance");
	if ((guaranteed || !rn2(10)) && Half_spell_damage) you_have("spell resistance");
	if ((guaranteed || !rn2(10)) && Regeneration) enl_msg("You regenerate", "", "d", "");
	if ((guaranteed || !rn2(10)) && Energy_regeneration) you_have("mana regeneration");
	if ((guaranteed || !rn2(10)) && (u.uspellprot || Protection)) {
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
	if ((guaranteed || !rn2(10)) && Protection_from_shape_changers)
		you_are("protected from shape changers");
	if ((guaranteed || !rn2(10)) && Polymorph) you_have("polymorphitis");
	if ((guaranteed || !rn2(10)) && Polymorph_control) you_have("polymorph control");
	if ((guaranteed || !rn2(10)) && (u.ulycn >= LOW_PM)) {
		Strcpy(buf, an(mons[u.ulycn].mname));
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (Upolyd && !missingnoprotect)) {
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form");
	    else Sprintf(buf, "polymorphed into %s", an(youmonst.data->mname));
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1)  Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Unchanging) you_can("not change from your current form");
	if ((guaranteed || !rn2(10)) && (Fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !have_speedbugstone())) you_are(Very_fast ? "very fast" : "fast");
	if ((guaranteed || !rn2(10)) && (Fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) )) you_are(Very_fast ? "very slow" : "slow");
	if ((guaranteed || !rn2(10)) && (!Fast && Very_fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !have_speedbugstone())) you_are("very fast");
	if ((guaranteed || !rn2(10)) && (!Fast && Very_fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) )) you_are("very slow");
	if ((guaranteed || !rn2(10)) && Reflecting) you_have("reflection");
	if ((guaranteed || !rn2(10)) && Free_action) you_have("free action");
	if ((guaranteed || !rn2(10)) && Fixed_abil) you_have("fixed abilities");
	if ((guaranteed || !rn2(10)) && (uamul && uamul->otyp == AMULET_VERSUS_STONE))
		enl_msg("You ", "will be", "would have been", " depetrified");
	if ((guaranteed || !rn2(10)) && Lifesaved)
		enl_msg("Your life ", "will be", "would have been", " saved");
	if ((guaranteed || !rn2(10)) && Second_chance)
  	  enl_msg("You ","will be", "would have been"," given a second chance");
	if ((guaranteed || !rn2(10)) && u.twoweap) {
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
	if ((guaranteed || !rn2(10)) && Luck) {
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
	 else if ((guaranteed || !rn2(10)) && (wizard || !rn2(10) || final >= 1)) enl_msg("Your luck ", "is", "was", " zero");
#endif
	if ((guaranteed || !rn2(10)) && (u.moreluck > 0)) you_have("extra luck");
	else if ((guaranteed || !rn2(10)) && (u.moreluck < 0)) you_have("reduced luck");
	if ((guaranteed || !rn2(10)) && (carrying(LUCKSTONE) || stone_luck(TRUE))) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		enl_msg("Bad luck ", "does", "did", " not time out for you");
	    if (ltmp >= 0)
		enl_msg("Good luck ", "does", "did", " not time out for you");
	}

	/* KMH, balance patch -- healthstones affect health */
	if ((guaranteed || !rn2(10)) && u.uhealbonus)
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

	if ((guaranteed || !rn2(10)) && recalc_mana() )
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

	if ((guaranteed || !rn2(10)) && u.ugangr) {
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
	  if (guaranteed || !rn2(10)) {

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
	struct permonst *ptr;

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
		dump("  Scroll identification only worked 100 times out of ", buf);
	}

	if (u.idpotionpenalty > 3) {
		Sprintf(buf, " %d", u.idpotionpenalty);
		dump("  Potion identification only worked 3 times out of ", buf);
	}

	if (u.idringpenalty > 4) {
		Sprintf(buf, " %d", u.idringpenalty);
		dump("  Ring identification only worked 4 times out of ", buf);
	}

	if (u.idamuletpenalty > 15) {
		Sprintf(buf, " %d", u.idamuletpenalty);
		dump("  Amulet identification only worked 15 times out of ", buf);
	}

	if (u.idwandpenalty > 3) {
		Sprintf(buf, " %d", u.idwandpenalty);
		dump("  Wand identification only worked 3 times out of ", buf);
	}

	if (u.musableremovechance) {
		Sprintf(buf, " %d%% chance", 100 - u.musableremovechance);
		dump("  Monsters only dropped their musable items with", buf);
	}

	if (u.bookspawnchance) {
		Sprintf(buf, " %d%%", 100 - u.bookspawnchance);
		dump("  Book drop chance was reduced to", buf);
	}

	if (u.scrollspawnchance) {
		Sprintf(buf, " %d%%", 100 - u.scrollspawnchance);
		dump("  Scroll drop chance was reduced to", buf);
	}

	if (u.ringspawnchance) {
		Sprintf(buf, " %d%%", 100 - u.ringspawnchance);
		dump("  Ring drop chance was reduced to", buf);
	}

	if (u.wandspawnchance) {
		Sprintf(buf, " %d%%", 100 - u.wandspawnchance);
		dump("  Wand drop chance was reduced to", buf);
	}

	if (u.amuletspawnchance) {
		Sprintf(buf, " %d%%", 100 - u.amuletspawnchance);
		dump("  Amulet drop chance was reduced to", buf);
	}

	if (u.potionspawnchance) {
		Sprintf(buf, " %d%%", 100 - u.potionspawnchance);
		dump("  Potion drop chance was reduced to", buf);
	}

	if (u.concealitemchance) {
		Sprintf(buf, " %d%% of the time only", 100 - u.concealitemchance);
		dump("  Concealing monsters were spawned underneath items", buf);
	}

	if (u.usefulitemchance) {
		Sprintf(buf, " %d%% chance", 100 - u.usefulitemchance);
		dump("  Monster death drops spawned with only", buf);
	}

	Sprintf(buf, " %d", nartifact_exist() );
	dump("  Number of artifacts generated was ", buf);

	if (u.legscratching > 1) { 
		Sprintf(buf, " %d", u.legscratching - 1);
		dump("  Your leg damage was ", buf);
	}

	if (u.youpolyamount > 0) {
		Sprintf(buf, " %d", u.youpolyamount);
		dump("  Your remaining amount of free polymorphs was ", buf);
	}

	if (u.antimagicshell > 0) {
		Sprintf(buf, " %d more turns", u.antimagicshell);
		dump("  Your antimagic shell would have lasted", buf);
	}

	if (Role_if(PM_UNBELIEVER)) {
		dump("  You were ", "producing a permanent antimagic shell");
	}

	Sprintf(buf, " turn %d", u.monstertimeout);
	dump("  Monster spawn increase started at ", buf);

	Sprintf(buf, " turn %d", u.monstertimefinish);
	dump("  Monster spawn increase reached its maximum at ", buf);
	dump("  In this game, Eevee's evolution was ", mons[u.eeveelution].mname );
	/* these two are tied together because the monstertimefinish variable defines the evolution --Amy */

	Sprintf(buf, " turn %d", u.ascensiontimelimit);
	dump("  Your limit for ascension was at ", buf);

	dump("  The monster class that cannot be genocided was ", monexplain[u.ungenocidable] );

	if (u.alwaysinvisible) {
		dump("  Today, this monster class was always generated invisible: ", monexplain[u.alwaysinvisible] );
	}

	Sprintf(buf, "spawned more often: %s (freq bonus %d)", monexplain[u.frequentmonster], u.freqmonsterbonus);
	dump("  The monster class that was ", buf );

	if (u.freqcolorbonus) {
		Sprintf(buf, "spawned more often: %s (freq bonus %d)", c_obj_colors[u.frequentcolor], u.freqcolorbonus);
		dump("  The monster color that was ", buf );
	}

	ptr = &mons[u.frequentspecies];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies2];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus2);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies3];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus3);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies4];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus4);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies5];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus5);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies6];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus6);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies7];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus7);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies8];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus8);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies9];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus9);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies10];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus10);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies11];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus11);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies12];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus12);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies13];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus13);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies14];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus14);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies15];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus15);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies16];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus16);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies17];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus17);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies18];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus18);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies19];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus19);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies20];
	Sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus20);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies2];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies3];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies4];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies5];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies6];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies7];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies8];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies9];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies10];
	Sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	if (u.speedymonster) {
		Sprintf(buf, "always generated with extra speed: %s", monexplain[u.speedymonster]);
		dump("  The monster class that ", buf );
	}

	if (u.musemonster) {
		Sprintf(buf, "more likely to generate with musable stuff: %s", monexplain[u.musemonster]);
		dump("  The monster class that ", buf );
	}

	if (u.minimalism) {
		Sprintf(buf, " %d", u.minimalism);
		dump("  Items generated only 1 time in", buf);
	}

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable].oc_name, obj_descr[u.unobtainable].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable2].oc_name, obj_descr[u.unobtainable2].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable3].oc_name, obj_descr[u.unobtainable3].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable4].oc_name, obj_descr[u.unobtainable4].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable5].oc_name, obj_descr[u.unobtainable5].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable6].oc_name, obj_descr[u.unobtainable6].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	if (u.unobtainablegeno != -1) {
		Sprintf(buf, "%s (%s)", obj_descr[u.unobtainablegeno].oc_name, obj_descr[u.unobtainablegeno].oc_descr);
		dump("  You had genocided this item: ", buf );
	}

	Sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable].oc_name, obj_descr[u.veryobtainable].oc_descr, u.veryobtainableboost);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable2].oc_name, obj_descr[u.veryobtainable2].oc_descr, u.veryobtainableboost2);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable3].oc_name, obj_descr[u.veryobtainable3].oc_descr, u.veryobtainableboost3);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable4].oc_name, obj_descr[u.veryobtainable4].oc_descr, u.veryobtainableboost4);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem].oc_name, obj_descr[u.alwayscurseditem].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem2].oc_name, obj_descr[u.alwayscurseditem2].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem3].oc_name, obj_descr[u.alwayscurseditem3].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem4].oc_name, obj_descr[u.alwayscurseditem4].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem5].oc_name, obj_descr[u.alwayscurseditem5].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	Sprintf(buf, "always invisible: %s", defsyms[trap_to_defsym(u.invisotrap)].explanation);
	dump("  The RNG hath decreed that this trap was ", buf );

	Sprintf(buf, "generated more often: %s (freq bonus %d)", defsyms[trap_to_defsym(u.frequenttrap)].explanation, u.freqtrapbonus);
	dump("  The RNG hath decreed that this trap was ", buf );

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
		dump("  Your protection was reduced by", buf);
	}

	if (u.extralives == 1) {
		dump(youhad, "an extra life");
	}

	if (u.extralives > 1) {
		Sprintf(buf, " %d", u.extralives);
		dump("  Your amount of extra lives was", buf);
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
	if (Fear_resistance) dump(youwere, "resistant to fear");
	if (Stone_resistance) dump(youwere, "petrification resistant");
	if (Invulnerable) dump(youwere, "invulnerable");
	if (u.urealedibility) {
		Sprintf(buf, "recognize detrimental food");
	      Sprintf(eos(buf), " (%d)", u.urealedibility);
		dump(youcould, buf);
	}

	/*** Troubles ***/

	if (multi) {
		Sprintf(buf, "paralyzed");
	      Sprintf(eos(buf), " (%d)", multi);
		dump(youwere, buf);
	}

	if (Halluc_resistance) 	dump("  ", "You resisted hallucinations");
	if (Hallucination) {
		if (HeavyHallu) Sprintf(buf, "badly hallucinating");
		else Sprintf(buf, "hallucinating");
	      Sprintf(eos(buf), " (%d)", HHallucination);
		dump(youwere, buf);
	}
	if (Stunned) {
		if (HeavyStunned) Sprintf(buf, "badly stunned");
		else Sprintf(buf, "stunned");
	      Sprintf(eos(buf), " (%d)", HStun);
		dump(youwere, buf);
	}
	if (Confusion) {
		if (HeavyConfusion) Sprintf(buf, "badly confused");
		else Sprintf(buf, "confused");
	      Sprintf(eos(buf), " (%d)", HConfusion);
		dump(youwere, buf);
	}
	if (Blinded) {
		if (HeavyBlind) Sprintf(buf, "badly blinded");
		else Sprintf(buf, "blinded");
	      Sprintf(eos(buf), " (%d)", Blinded);
		dump(youwere, buf);
	}
	if (Sick) {
		Sprintf(buf, "sick");
			if (u.usick_type & SICK_VOMITABLE) Sprintf(eos(buf), " from food poisoning");
			if (u.usick_type & SICK_NONVOMITABLE) Sprintf(eos(buf), " from illness");
	      Sprintf(eos(buf), " (%d)", Sick);
		dump(youwere, buf);
	}
	if (Vomiting) {
		Sprintf(buf, "nauseated");
	      Sprintf(eos(buf), " (%d)", Vomiting);
		dump(youwere, buf);
	}
	if (Feared) {
		if (HeavyFeared) Sprintf(buf, "stricken with very strong fear");
		else Sprintf(buf, "stricken with fear");
	      Sprintf(eos(buf), " (%d)", HFeared);
		dump(youwere, buf);
	}
	if (Numbed) {
		if (HeavyNumbed) Sprintf(buf, "badly numbed");
		else Sprintf(buf, "numbed");
	      Sprintf(eos(buf), " (%d)", HNumbed);
		dump(youwere, buf);
	}
	if (Frozen) {
		if (HeavyFrozen) Sprintf(buf, "frozen rigid and solid");
		else Sprintf(buf, "frozen solid");
	      Sprintf(eos(buf), " (%d)", HFrozen);
		dump(youwere, buf);
	}

	if (u.hanguppenalty) {
		Sprintf(buf, "temporarily slower because you tried to hangup cheat");
	      Sprintf(eos(buf), " (%d)", u.hanguppenalty);
		dump(youwere, buf);
	}

	if (Burned) {
		if (HeavyBurned) Sprintf(buf, "badly burned");
		else Sprintf(buf, "burned");
	      Sprintf(eos(buf), " (%d)", HBurned);
		dump(youwere, buf);
	}
		
      if (Punished) dump(youwere, "punished");

      if (u.totter) dump(youhad, "inverted directional keys");

      if (sengr_at("Elbereth", u.ux, u.uy) ) dump(youwere, "standing on an active Elbereth engraving");

	if (Stoned) {
		Sprintf(buf, "turning to stone");
	      Sprintf(eos(buf), " (%d)", Stoned);
		dump(youwere, buf);
	}
	if (Slimed) {
		Sprintf(buf, "turning into slime");
	      Sprintf(eos(buf), " (%d)", Slimed);
		dump(youwere, buf);
	}
	if (Strangled) {
		Sprintf(buf, (u.uburied) ? "buried" : "being strangled");
	      Sprintf(eos(buf), " (%d)", Strangled);
		dump(youwere, buf);
	}
	if (Prem_death) {
		Sprintf(buf, "going to die prematurely");
		dump(youwere, buf);
	}

	if (IsGlib) {
		Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
	      Sprintf(eos(buf), " (%d)", Glib);
		dump(youhad, buf);
	}

	if (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone()) {
		Sprintf(buf, "the menu bug.");
	      Sprintf(eos(buf), " (%d)", MenuBug);
		dump(youhad, buf);
	}
	if (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) {
		Sprintf(buf, "the speed bug.");
	      Sprintf(eos(buf), " (%d)", SpeedBug);
		dump(youhad, buf);
	}
	if (Superscroller || u.uprops[SUPERSCROLLER_ACTIVE].extrinsic || have_superscrollerstone()) {
		Sprintf(buf, "the superscroller.");
	      Sprintf(eos(buf), " (%d)", Superscroller);
		dump(youhad, buf);
	}
	if (RMBLoss || u.uprops[RMB_LOST].extrinsic || have_rmbstone()) {
		Sprintf(buf, "the following problem: Your right mouse button failed.");
	      Sprintf(eos(buf), " (%d)", RMBLoss);
		dump(youhad, buf);
	}
	if (DisplayLoss || u.uprops[DISPLAY_LOST].extrinsic || have_displaystone()) {
		Sprintf(buf, "the following problem: Your display failed.");
	      Sprintf(eos(buf), " (%d)", DisplayLoss);
		dump(youhad, buf);
	}
	if (SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) {
		Sprintf(buf, "the following problem: You lost spells.");
	      Sprintf(eos(buf), " (%d)", SpellLoss);
		dump(youhad, buf);
	}
	if (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone()) {
		Sprintf(buf, "the following problem: Your spells became yellow.");
	      Sprintf(eos(buf), " (%d)", YellowSpells);
		dump(youhad, buf);
	}
	if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || have_autodestructstone()) {
		Sprintf(buf, "the following problem: An auto destruct mechanism was initiated.");
	      Sprintf(eos(buf), " (%d)", AutoDestruct);
		dump(youhad, buf);
	}
	if (MemoryLoss || u.uprops[MEMORY_LOST].extrinsic || have_memorylossstone()) {
		Sprintf(buf, "the following problem: There was low local memory.");
	      Sprintf(eos(buf), " (%d)", MemoryLoss);
		dump(youhad, buf);
	}
	if (InventoryLoss || u.uprops[INVENTORY_LOST].extrinsic || have_inventorylossstone()) {
		Sprintf(buf, "the following problem: There was not enough memory to create an inventory window.");
	      Sprintf(eos(buf), " (%d)", InventoryLoss);
		dump(youhad, buf);
	}
	if (BlackNgWalls || u.uprops[BLACK_NG_WALLS].extrinsic || have_blackystone()) {
		Sprintf(buf, "the following problem: Blacky and his NG walls were closing in.");
	      Sprintf(eos(buf), " (%d)", BlackNgWalls);
		dump(youhad, buf);
	}
	if (FreeHandLoss || u.uprops[FREE_HAND_LOST].extrinsic || have_freehandbugstone()) {
		Sprintf(buf, "the following problem: Your free hand was less likely to be free.");
	      Sprintf(eos(buf), " (%d)", FreeHandLoss);
		dump(youhad, buf);
	}
	if (Unidentify || u.uprops[UNIDENTIFY].extrinsic || have_unidentifystone()) {
		Sprintf(buf, "the following problem: Your possessions sometimes unidentified themselves.");
	      Sprintf(eos(buf), " (%d)", Unidentify);
		dump(youhad, buf);
	}
	if (Thirst || u.uprops[THIRST].extrinsic || have_thirststone()) {
		Sprintf(buf, "a strong sense of thirst.");
	      Sprintf(eos(buf), " (%d)", Thirst);
		dump(youhad, buf);
	}
	if (LuckLoss || u.uprops[LUCK_LOSS].extrinsic || have_unluckystone()) {
		Sprintf(buf, "the following problem: You were shitting out of luck (SOL).");
	      Sprintf(eos(buf), " (%d)", LuckLoss);
		dump(youhad, buf);
	}
	if (ShadesOfGrey || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone()) {
		Sprintf(buf, "the following problem: Everything displayed in various shades of grey.");
	      Sprintf(eos(buf), " (%d)", ShadesOfGrey);
		dump(youhad, buf);
	}
	if (FaintActive || u.uprops[FAINT_ACTIVE].extrinsic || have_faintingstone()) {
		Sprintf(buf, "the following problem: You randomly fainted.");
	      Sprintf(eos(buf), " (%d)", FaintActive);
		dump(youhad, buf);
	}
	if (Itemcursing || u.uprops[ITEMCURSING].extrinsic || have_cursingstone() || have_primecurse() ) {
		Sprintf(buf, "the following problem: Your inventory gradually filled up with cursed items.");
	      Sprintf(eos(buf), " (%d)", Itemcursing);
		dump(youhad, buf);
	}
	if (DifficultyIncreased || u.uprops[DIFFICULTY_INCREASED].extrinsic || have_difficultystone()) {
		Sprintf(buf, "the following problem: The difficulty of the game was arbitrarily increased.");
	      Sprintf(eos(buf), " (%d)", DifficultyIncreased);
		dump(youhad, buf);
	}
	if (Deafness || u.uprops[DEAFNESS].extrinsic || have_deafnessstone()) {
		Sprintf(buf, "a hearing break.");
	      Sprintf(eos(buf), " (%d)", Deafness);
		dump(youhad, buf);
	}
	if (CasterProblem || u.uprops[CASTER_PROBLEM].extrinsic || have_antimagicstone()) {
		Sprintf(buf, "blood mana.");
	      Sprintf(eos(buf), " (%d)", CasterProblem);
		dump(youhad, buf);
	}
	if (WeaknessProblem || u.uprops[WEAKNESS_PROBLEM].extrinsic || have_weaknessstone()) {
		Sprintf(buf, "the following problem: Being weak from hunger damaged your health.");
	      Sprintf(eos(buf), " (%d)", WeaknessProblem);
		dump(youhad, buf);
	}
	if (RotThirteen || u.uprops[ROT_THIRTEEN].extrinsic || have_rotthirteenstone()) {
		Sprintf(buf, "the following problem: A rot13 cypher had been activated for lowercase letters.");
	      Sprintf(eos(buf), " (%d)", RotThirteen);
		dump(youhad, buf);
	}
	if (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone()) {
		Sprintf(buf, "the following problem: You could not move diagonally.");
	      Sprintf(eos(buf), " (%d)", BishopGridbug);
		dump(youhad, buf);
	}
	if (ConfusionProblem || u.uprops[CONFUSION_PROBLEM].extrinsic || have_confusionstone()) {
		Sprintf(buf, "a confusing problem.");
	      Sprintf(eos(buf), " (%d)", ConfusionProblem);
		dump(youhad, buf);
	}
	if (NoDropProblem || u.uprops[DROP_BUG].extrinsic || have_dropbugstone()) {
		Sprintf(buf, "the following problem: You could not drop items.");
	      Sprintf(eos(buf), " (%d)", NoDropProblem);
		dump(youhad, buf);
	}
	if (DSTWProblem || u.uprops[DSTW_BUG].extrinsic || have_dstwstone()) {
		Sprintf(buf, "the following problem: Your potions didn't always work.");
	      Sprintf(eos(buf), " (%d)", DSTWProblem);
		dump(youhad, buf);
	}
	if (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) {
		Sprintf(buf, "the following problem: You could not cure status effects.");
	      Sprintf(eos(buf), " (%d)", StatusTrapProblem);
		dump(youhad, buf);
	}
	if (AlignmentProblem || u.uprops[ALIGNMENT_FAILURE].extrinsic || have_alignmentstone()) {
		Sprintf(buf, "the following problem: Your maximum alignment decreased over time.");
	      Sprintf(eos(buf), " (%d)", AlignmentProblem);
		dump(youhad, buf);
	}
	if (StairsProblem || u.uprops[STAIRSTRAP].extrinsic || have_stairstrapstone()) {
		Sprintf(buf, "the following problem: Stairs were always trapped.");
	      Sprintf(eos(buf), " (%d)", StairsProblem);
		dump(youhad, buf);
	}
	if (UninformationProblem || u.uprops[UNINFORMATION].extrinsic || have_uninformationstone()) {
		Sprintf(buf, "the following problem: The game gave insufficient amounts of information.");
	      Sprintf(eos(buf), " (%d)", UninformationProblem);
		dump(youhad, buf);
	}

	if (CaptchaProblem || u.uprops[CAPTCHA].extrinsic || have_captchastone()) {
		Sprintf(buf, "the following problem: You sometimes had to solve captchas.");
	      Sprintf(eos(buf), " (%d)", CaptchaProblem);
		dump(youhad, buf);
	}
	if (FarlookProblem || u.uprops[FARLOOK_BUG].extrinsic || have_farlookstone()) {
		Sprintf(buf, "the following problem: Farlooking peaceful monsters angered them.");
	      Sprintf(eos(buf), " (%d)", FarlookProblem);
		dump(youhad, buf);
	}
	if (RespawnProblem || u.uprops[RESPAWN_BUG].extrinsic || have_respawnstone()) {
		Sprintf(buf, "the following problem: Killing monsters caused them to respawn somewhere on the level.");
	      Sprintf(eos(buf), " (%d)", RespawnProblem);
		dump(youhad, buf);
	}

	if (BigscriptEffect || u.uprops[BIGSCRIPT].extrinsic || have_bigscriptstone()) {
		Sprintf(buf, "BIGscript.");
	      Sprintf(eos(buf), " (%d)", BigscriptEffect);
		dump(youhad, buf);
	}
	if (BankTrapEffect || u.uprops[BANKBUG].extrinsic || have_bankstone()) {
		Sprintf(buf, "the following problem: Your money wandered into a mysterious bank.");
	      Sprintf(eos(buf), " (%d)", BankTrapEffect);
	      Sprintf(eos(buf), " (amount stored: %d)", u.bankcashamount);
	      Sprintf(eos(buf), " (money limit: %d)", u.bankcashlimit);
		dump(youhad, buf);
	} else {
	  if (u.bankcashamount) {
		Sprintf(buf, "the following amount of cash stored in the bank:");
		Sprintf(eos(buf), " %d", u.bankcashamount);
		dump(youhad, buf);
	  }
	  if (u.bankcashlimit) {
		Sprintf(buf, "the following bank cash limit:");
		Sprintf(eos(buf), " %d", u.bankcashlimit);
		dump(youhad, buf);
	  }
	}
	if (MapTrapEffect || u.uprops[MAPBUG].extrinsic || have_mapstone()) {
		Sprintf(buf, "the following problem: The map didn't display correctly.");
	      Sprintf(eos(buf), " (%d)", MapTrapEffect);
		dump(youhad, buf);
	}
	if (TechTrapEffect || u.uprops[TECHBUG].extrinsic || have_techniquestone()) {
		Sprintf(buf, "the following problem: Your techniques didn't work.");
	      Sprintf(eos(buf), " (%d)", TechTrapEffect);
		dump(youhad, buf);
	}
	if (RecurringDisenchant || u.uprops[RECURRING_DISENCHANT].extrinsic || have_disenchantmentstone()) {
		Sprintf(buf, "the following problem: Your possessions disenchanted themselves spontaneously.");
	      Sprintf(eos(buf), " (%d)", RecurringDisenchant);
		dump(youhad, buf);
	}
	if (verisiertEffect || u.uprops[VERISIERTEFFECT].extrinsic || have_verisiertstone()) {
		Sprintf(buf, "the following problem: Monster respawn speeded up rapidly.");
	      Sprintf(eos(buf), " (%d)", verisiertEffect);
		dump(youhad, buf);
	}
	if (ChaosTerrain || u.uprops[CHAOS_TERRAIN].extrinsic || have_chaosterrainstone()) {
		Sprintf(buf, "the following problem: The terrain slowly became ever more chaotic.");
	      Sprintf(eos(buf), " (%d)", ChaosTerrain);
		dump(youhad, buf);
	}
	if (Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone()) {
		Sprintf(buf, "the following problem: You were completely unable to cast spells.");
	      Sprintf(eos(buf), " (%d)", Muteness);
		dump(youhad, buf);
	}
	if (EngravingDoesntWork || u.uprops[ENGRAVINGBUG].extrinsic || have_engravingstone()) {
		Sprintf(buf, "the following problem: Monsters didn't respect Elbereth.");
	      Sprintf(eos(buf), " (%d)", EngravingDoesntWork);
		dump(youhad, buf);
	}
	if (MagicDeviceEffect || u.uprops[MAGIC_DEVICE_BUG].extrinsic || have_magicdevicestone()) {
		Sprintf(buf, "the following problem: Zapping a wand could cause it to explode.");
	      Sprintf(eos(buf), " (%d)", MagicDeviceEffect);
		dump(youhad, buf);
	}
	if (BookTrapEffect || u.uprops[BOOKBUG].extrinsic || have_bookstone()) {
		Sprintf(buf, "the following problem: Reading spellbooks confused you.");
	      Sprintf(eos(buf), " (%d)", BookTrapEffect);
		dump(youhad, buf);
	}
	if (LevelTrapEffect || u.uprops[LEVELBUG].extrinsic || have_levelstone()) {
		Sprintf(buf, "the following problem: Monsters became stronger if many of their species had been generated already.");
	    Sprintf(eos(buf), " (%d)", LevelTrapEffect);
		dump(youhad, buf);
	}
	if (QuizTrapEffect || u.uprops[QUIZZES].extrinsic || have_quizstone()) {
		Sprintf(buf, "the following problem: You had to partake in the Great NetHack Quiz.");
	      Sprintf(eos(buf), " (%d)", QuizTrapEffect);
		dump(youhad, buf);
	}

	if (FastMetabolismEffect || u.uprops[FAST_METABOLISM].extrinsic || have_metabolicstone()) {
		Sprintf(buf, "the following problem: Your food consumption was much faster.");
	      Sprintf(eos(buf), " (%d)", FastMetabolismEffect);
		dump(youhad, buf);
	}

	if (NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone()) {
		Sprintf(buf, "the following problem: You could not teleport at all.");
	      Sprintf(eos(buf), " (%d)", NoReturnEffect);
		dump(youhad, buf);
	}

	if (AlwaysEgotypeMonsters || u.uprops[ALWAYS_EGOTYPES].extrinsic || have_egostone()) {
		Sprintf(buf, "the following problem: Monsters always spawned with egotypes.");
	      Sprintf(eos(buf), " (%d)", AlwaysEgotypeMonsters);
		dump(youhad, buf);
	}

	if (TimeGoesByFaster || u.uprops[FAST_FORWARD].extrinsic || have_fastforwardstone()) {
		Sprintf(buf, "the following problem: Time went by faster.");
	      Sprintf(eos(buf), " (%d)", TimeGoesByFaster);
		dump(youhad, buf);
	}

	if (FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) {
		Sprintf(buf, "the following problem: Your food was always rotten.");
	      Sprintf(eos(buf), " (%d)", FoodIsAlwaysRotten);
		dump(youhad, buf);
	}

	if (AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone()) {
		Sprintf(buf, "the following problem: Your skills were deactivated.");
	      Sprintf(eos(buf), " (%d)", AllSkillsUnskilled);
		dump(youhad, buf);
	}

	if (AllStatsAreLower || u.uprops[STATS_LOWERED].extrinsic || have_lowstatstone()) {
		Sprintf(buf, "the following problem: All of your stats were lowered.");
	      Sprintf(eos(buf), " (%d)", AllStatsAreLower);
		dump(youhad, buf);
	}

	if (PlayerCannotTrainSkills || u.uprops[TRAINING_DEACTIVATED].extrinsic || have_trainingstone()) {
		Sprintf(buf, "the following problem: You could not train skills.");
	      Sprintf(eos(buf), " (%d)", PlayerCannotTrainSkills);
		dump(youhad, buf);
	}

	if (PlayerCannotExerciseStats || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone()) {
		Sprintf(buf, "the following problem: You could not exercise your stats.");
	      Sprintf(eos(buf), " (%d)", PlayerCannotExerciseStats);
		dump(youhad, buf);
	}

	if (MCReduction) {
		Sprintf(buf, "reduced magic cancellation.");
	      Sprintf(eos(buf), " (%d)", MCReduction);
		dump(youhad, buf);
	}

	if (u.uprops[INTRINSIC_LOSS].extrinsic || IntrinsicLossProblem || have_intrinsiclossstone() ) {
		Sprintf(buf, "a case of random intrinsic loss.");
	      Sprintf(eos(buf), " (%d)", IntrinsicLossProblem);
		dump(youhad, buf);
	}
	if (u.uprops[TRAP_REVEALING].extrinsic) {
		Sprintf(buf, "randomly revealing traps");
		dump(youwere, buf);
	}
	if (u.uprops[BLOOD_LOSS].extrinsic || BloodLossProblem || have_bloodlossstone() ) {
		Sprintf(buf, "bleeding out");
	      Sprintf(eos(buf), " (%d)", BloodLossProblem);
		dump(youwere, buf);
	}
	if (u.uprops[NASTINESS_EFFECTS].extrinsic || NastinessProblem || have_nastystone() ) {
		Sprintf(buf, "subjected to random nasty trap effects");
	      Sprintf(eos(buf), " (%d)", NastinessProblem);
		dump(youwere, buf);
	}
	if (u.uprops[BAD_EFFECTS].extrinsic || BadEffectProblem || have_badeffectstone() ) {
		Sprintf(buf, "subjected to random bad effects");
	      Sprintf(eos(buf), " (%d)", BadEffectProblem);
		dump(youwere, buf);
	}
	if (u.uprops[AUTOMATIC_TRAP_CREATION].extrinsic || TrapCreationProblem || have_trapcreationstone() ) {
		Sprintf(buf, "more likely to encounter traps");
	      Sprintf(eos(buf), " (%d)", TrapCreationProblem);
		dump(youwere, buf);
	}
	if (u.uprops[SENTIENT_HIGH_HEELS].extrinsic) {
		Sprintf(buf, "wearing sentient high heels");
		dump(youwere, buf);
	}
	if (u.uprops[REPEATING_VULNERABILITY].extrinsic || AutomaticVulnerabilitiy || have_vulnerabilitystone() ) {
		Sprintf(buf, "the risk of temporarily losing intrinsics");
	      Sprintf(eos(buf), " (%d)", AutomaticVulnerabilitiy);
		dump(youhad, buf);
	}
	if (u.uprops[TELEPORTING_ITEMS].extrinsic || TeleportingItems || have_itemportstone() ) {
		Sprintf(buf, "teleporting items");
	      Sprintf(eos(buf), " (%d)", TeleportingItems);
		dump(youhad, buf);
	}

	if (u.uprops[DESECRATION].extrinsic || Desecration || have_nonsacredstone() ) {
		Sprintf(buf, "the following problem: Altars malfunction whenever you try to use them");
	      Sprintf(eos(buf), " (%d)", Desecration);
		dump(youhad, buf);
	}

	if (u.uprops[STARVATION_EFFECT].extrinsic || StarvationEffect || have_starvationstone() ) {
		Sprintf(buf, "not going to get corpses from defeated enemies");
	      Sprintf(eos(buf), " (%d)", StarvationEffect);
		dump(youwere, buf);
	}

	if (u.uprops[NO_DROPS_EFFECT].extrinsic || NoDropsEffect || have_droplessstone() ) {
		Sprintf(buf, "not going to get death drops from monsters");
	      Sprintf(eos(buf), " (%d)", NoDropsEffect);
		dump(youwere, buf);
	}

	if (u.uprops[LOW_EFFECTS].extrinsic || LowEffects || have_loweffectstone() ) {
		Sprintf(buf, "reduced power of magical effects");
	      Sprintf(eos(buf), " (%d)", LowEffects);
		dump(youhad, buf);
	}

	if (u.uprops[INVIS_TRAPS_EFFECT].extrinsic || InvisibleTrapsEffect || have_invisostone() ) {
		Sprintf(buf, "not going to see traps");
	      Sprintf(eos(buf), " (%d)", InvisibleTrapsEffect);
		dump(youwere, buf);
	}

	if (u.uprops[GHOST_WORLD].extrinsic || GhostWorld || have_ghostlystone() ) {
		Sprintf(buf, "in a ghost world");
	      Sprintf(eos(buf), " (%d)", GhostWorld);
		dump(youwere, buf);
	}

	if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
		Sprintf(buf, "going to become dehydrated");
	      Sprintf(eos(buf), " (%d)", Dehydration);
		dump(youwere, buf);
	}

	if (u.uprops[HATE_TRAP_EFFECT].extrinsic || HateTrapEffect || have_hatestone() ) {
		Sprintf(buf, "the following problem: Pets hate you with a fiery passion.");
	      Sprintf(eos(buf), " (%d)", HateTrapEffect);
		dump(youhad, buf);
	}

	if (u.uprops[TOTTER_EFFECT].extrinsic || TotterTrapEffect || have_directionswapstone() ) {
		Sprintf(buf, "the following problem: Your directional keys got swapped.");
	      Sprintf(eos(buf), " (%d)", TotterTrapEffect);
		dump(youhad, buf);
	}

	if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) {
		Sprintf(buf, "unable to get intrinsics from eating corpses");
	      Sprintf(eos(buf), " (%d)", Nonintrinsics);
		dump(youwere, buf);
	}

	if (u.uprops[DROPCURSES_EFFECT].extrinsic || Dropcurses || have_dropcursestone() ) {
		Sprintf(buf, "the following problem: Dropping items causes them to autocurse.");
	      Sprintf(eos(buf), " (%d)", Dropcurses);
		dump(youhad, buf);
	}

	if (u.uprops[NAKEDNESS].extrinsic || Nakedness || have_nakedstone() ) {
		Sprintf(buf, "effectively naked");
	      Sprintf(eos(buf), " (%d)", Nakedness);
		dump(youwere, buf);
	}

	if (u.uprops[ANTILEVELING].extrinsic || Antileveling || have_antilevelstone() ) {
		Sprintf(buf, "unable to gain experience");
	      Sprintf(eos(buf), " (%d)", Antileveling);
		dump(youwere, buf);
	}

	if (u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || have_stealerstone() ) {
		Sprintf(buf, "more likely to have your items stolen");
	      Sprintf(eos(buf), " (%d)", ItemStealingEffect);
		dump(youwere, buf);
	}

	if (u.uprops[REBELLION_EFFECT].extrinsic || Rebellions || have_rebelstone() ) {
		Sprintf(buf, "the following problem: Pets can spontaneously rebel and become hostile.");
	      Sprintf(eos(buf), " (%d)", Rebellions);
		dump(youhad, buf);
	}

	if (u.uprops[CRAP_EFFECT].extrinsic || CrapEffect || have_shitstone() ) {
		Sprintf(buf, "diarrhea");
	      Sprintf(eos(buf), " (%d)", CrapEffect);
		dump(youhad, buf);
	}

	if (u.uprops[PROJECTILES_MISFIRE].extrinsic || ProjectilesMisfire || have_misfirestone() ) {
		Sprintf(buf, "the following problem: Your projectiles often misfire.");
	      Sprintf(eos(buf), " (%d)", ProjectilesMisfire);
		dump(youhad, buf);
	}

	if (u.uprops[WALL_TRAP_EFFECT].extrinsic || WallTrapping || have_wallstone() ) {
		Sprintf(buf, "the following problem: Dungeon regrowth is excessively fast.");
	      Sprintf(eos(buf), " (%d)", WallTrapping);
		dump(youhad, buf);
	}

	if (u.uprops[RECURRING_AMNESIA].extrinsic || RecurringAmnesia || have_amnesiastone() ) {
		Sprintf(buf, "going to suffer from amnesia now and then");
	      Sprintf(eos(buf), " (%d)", RecurringAmnesia);
		dump(youwere, buf);
	}

	if (u.uprops[TURNLIMITATION].extrinsic || TurnLimitation || have_limitationstone() ) {
		Sprintf(buf, "getting your ascension turn limit reduced whenever you take damage");
	      Sprintf(eos(buf), " (%d)", TurnLimitation);
		dump(youwere, buf);
	}

	if (u.uprops[WEAKSIGHT].extrinsic || WeakSight || have_weaksightstone() ) {
		Sprintf(buf, "short-sighted");
	      Sprintf(eos(buf), " (%d)", WeakSight);
		dump(youwere, buf);
	}

	if (u.uprops[RANDOM_MESSAGES].extrinsic || RandomMessages || have_messagestone() ) {
		Sprintf(buf, "reading random messages");
	      Sprintf(eos(buf), " (%d)", RandomMessages);
		dump(youwere, buf);
	}


	if (u.uprops[RANDOM_RUMORS].extrinsic) {
		Sprintf(buf, "going to listen to random rumors");
		dump(youwere, buf);
	}

	if (IncreasedGravity) {
		Sprintf(buf, "increased encumbrance due to a stronger gravity.");
	      Sprintf(eos(buf), " (%d)", IncreasedGravity);
		dump(youhad, buf);
	}

	if (NoStaircase) {
		Sprintf(buf, "to wait until you could use staircases again.");
	      Sprintf(eos(buf), " (%d)", NoStaircase);
		dump(youhad, buf);
	}

	if (TimeStopped) {
		Sprintf(buf, "stopped the flow of time.");
	      Sprintf(eos(buf), " (%d)", TimeStopped);
		dump(youhad, buf);
	}

	if (UseTheForce) {
		Sprintf(buf, "able to use the force like a true jedi");
		dump(youwere, buf);
	}

	if (u.uprops[SENSORY_DEPRIVATION].extrinsic) {
		Sprintf(buf, "sensory deprivation.");
		dump(youhad, buf);
	}

	if (NoFire_resistance) {
		Sprintf(buf, "prevented from having fire resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FIRE_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoCold_resistance) {
		Sprintf(buf, "prevented from having cold resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_COLD_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoSleep_resistance) {
		Sprintf(buf, "prevented from having sleep resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLEEP_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoDisint_resistance) {
		Sprintf(buf, "prevented from having disintegration resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISINT_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoShock_resistance) {
		Sprintf(buf, "prevented from having shock resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SHOCK_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoPoison_resistance) {
		Sprintf(buf, "prevented from having poison resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_POISON_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoDrain_resistance) {
		Sprintf(buf, "prevented from having drain resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRAIN_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoSick_resistance) {
		Sprintf(buf, "prevented from having sickness resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICK_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoAntimagic_resistance) {
		Sprintf(buf, "prevented from having magic resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_ANTIMAGIC].intrinsic);
		dump(youwere, buf);
	}
	if (NoAcid_resistance) {
		Sprintf(buf, "prevented from having acid resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_ACID_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoStone_resistance) {
		Sprintf(buf, "prevented from having petrification resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONE_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoFear_resistance) {
		Sprintf(buf, "prevented from having fear resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoSee_invisible) {
		Sprintf(buf, "prevented from having see invisible");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEE_INVIS].intrinsic);
		dump(youwere, buf);
	}
	if (NoTelepat) {
		Sprintf(buf, "prevented from having telepathy");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPAT].intrinsic);
		dump(youwere, buf);
	}
	if (NoWarning) {
		Sprintf(buf, "prevented from having warning");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_WARNING].intrinsic);
		dump(youwere, buf);
	}
	if (NoSearching) {
		Sprintf(buf, "prevented from having automatic searching");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEARCHING].intrinsic);
		dump(youwere, buf);
	}
	if (NoClairvoyant) {
		Sprintf(buf, "prevented from having clairvoyance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_CLAIRVOYANT].intrinsic);
		dump(youwere, buf);
	}
	if (NoInfravision) {
		Sprintf(buf, "prevented from having infravision");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_INFRAVISION].intrinsic);
		dump(youwere, buf);
	}
	if (NoDetect_monsters) {
		Sprintf(buf, "prevented from having detect monsters");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DETECT_MONSTERS].intrinsic);
		dump(youwere, buf);
	}
	if (NoInvisible) {
		Sprintf(buf, "prevented from having invisibility");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_INVIS].intrinsic);
		dump(youwere, buf);
	}
	if (NoDisplaced) {
		Sprintf(buf, "prevented from having displacement");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISPLACED].intrinsic);
		dump(youwere, buf);
	}
	if (NoStealth) {
		Sprintf(buf, "prevented from having stealth");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STEALTH].intrinsic);
		dump(youwere, buf);
	}
	if (NoJumping) {
		Sprintf(buf, "prevented from having jumping");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_JUMPING].intrinsic);
		dump(youwere, buf);
	}
	if (NoTeleport_control) {
		Sprintf(buf, "prevented from having teleport control");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPORT_CONTROL].intrinsic);
		dump(youwere, buf);
	}
	if (NoFlying) {
		Sprintf(buf, "prevented from having flying");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FLYING].intrinsic);
		dump(youwere, buf);
	}
	if (NoBreathless) {
		Sprintf(buf, "prevented from having magical breathing");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_MAGICAL_BREATHING].intrinsic);
		dump(youwere, buf);
	}
	if (NoPasses_walls) {
		Sprintf(buf, "prevented from having phasing");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_PASSES_WALLS].intrinsic);
		dump(youwere, buf);
	}
	if (NoSlow_digestion) {
		Sprintf(buf, "prevented from having slow digestion");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLOW_DIGESTION].intrinsic);
		dump(youwere, buf);
	}
	if (NoHalf_spell_damage) {
		Sprintf(buf, "prevented from having half spell damage");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_SPDAM].intrinsic);
		dump(youwere, buf);
	}
	if (NoHalf_physical_damage) {
		Sprintf(buf, "prevented from having half physical damage");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_PHDAM].intrinsic);
		dump(youwere, buf);
	}
	if (NoRegeneration) {
		Sprintf(buf, "prevented from having regeneration");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_REGENERATION].intrinsic);
		dump(youwere, buf);
	}
	if (NoEnergy_regeneration) {
		Sprintf(buf, "prevented from having mana regeneration");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_ENERGY_REGENERATION].intrinsic);
		dump(youwere, buf);
	}
	if (NoPolymorph_control) {
		Sprintf(buf, "prevented from having polymorph control");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic);
		dump(youwere, buf);
	}
	if (NoFast) {
		Sprintf(buf, "prevented from having speed");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FAST].intrinsic);
		dump(youwere, buf);
	}
	if (NoReflecting) {
		Sprintf(buf, "prevented from having reflection");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_REFLECTING].intrinsic);
		dump(youwere, buf);
	}
	if (NoKeen_memory) {
		Sprintf(buf, "prevented from having keen memory");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_KEEN_MEMORY].intrinsic);
		dump(youwere, buf);
	}
	if (NoSight_bonus) {
		Sprintf(buf, "prevented from having a sight bonus");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SIGHT_BONUS].intrinsic);
		dump(youwere, buf);
	}
	if (NoVersus_curses) {
		Sprintf(buf, "prevented from having curse resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_VERSUS_CURSES].intrinsic);
		dump(youwere, buf);
	}

	if (NoStun_resist) {
		Sprintf(buf, "prevented from having stun resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUN_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoConf_resist) {
		Sprintf(buf, "prevented from having confusion resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_CONF_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoPsi_resist) {
		Sprintf(buf, "prevented from having psi resistance");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_PSI_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoDouble_attack) {
		Sprintf(buf, "prevented from having double attacks");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DOUBLE_ATTACK].intrinsic);
		dump(youwere, buf);
	}
	if (NoQuad_attack) {
		Sprintf(buf, "prevented from having quad attacks");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_QUAD_ATTACK].intrinsic);
		dump(youwere, buf);
	}

	if (NoFree_action) {
		Sprintf(buf, "prevented from having free action");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREE_ACTION].intrinsic);
		dump(youwere, buf);
	}

	if (NoHallu_party) {
		Sprintf(buf, "prevented from hallu partying");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALLU_PARTY].intrinsic);
		dump(youwere, buf);
	}

	if (NoDrunken_boxing) {
		Sprintf(buf, "prevented from drunken boxing");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRUNKEN_BOXING].intrinsic);
		dump(youwere, buf);
	}

	if (NoStunnopathy) {
		Sprintf(buf, "prevented from having stunnopathy");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUNNOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoNumbopathy) {
		Sprintf(buf, "prevented from having numbopathy");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_NUMBOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoFreezopathy) {
		Sprintf(buf, "prevented from having freezopathy");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREEZOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoStoned_chiller) {
		Sprintf(buf, "prevented from being a stoned chiller");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONED_CHILLER].intrinsic);
		dump(youwere, buf);
	}

	if (NoCorrosivity) {
		Sprintf(buf, "prevented from having corrosivity");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_CORROSIVITY].intrinsic);
		dump(youwere, buf);
	}

	if (NoFear_factor) {
		Sprintf(buf, "prevented from having an increased fear factor");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_FACTOR].intrinsic);
		dump(youwere, buf);
	}

	if (NoBurnopathy) {
		Sprintf(buf, "prevented from having burnopathy");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_BURNOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoSickopathy) {
		Sprintf(buf, "prevented from having sickopathy");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICKOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoWonderlegs) {
		Sprintf(buf, "prevented from having wonderlegs");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_WONDERLEGS].intrinsic);
		dump(youwere, buf);
	}
	if (NoGlib_combat) {
		Sprintf(buf, "prevented from having glib combat");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_GLIB_COMBAT].intrinsic);
		dump(youwere, buf);
	}

	if (NoUseTheForce) {
		Sprintf(buf, "prevented from using the force like a real jedi");
	      Sprintf(eos(buf), " (%d)", u.uprops[DEAC_THE_FORCE].intrinsic);
		dump(youwere, buf);
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
		case COPPER_DRAGON_SCALE_SHIELD:
		case PLATINUM_DRAGON_SCALE_SHIELD:
		case BRASS_DRAGON_SCALE_SHIELD:
		case AMETHYST_DRAGON_SCALE_SHIELD:
		case PURPLE_DRAGON_SCALE_SHIELD:
		case DIAMOND_DRAGON_SCALE_SHIELD:
		case EMERALD_DRAGON_SCALE_SHIELD:
		case SAPPHIRE_DRAGON_SCALE_SHIELD:
		case RUBY_DRAGON_SCALE_SHIELD:
		case GREEN_DRAGON_SCALE_SHIELD:
		case GOLDEN_DRAGON_SCALE_SHIELD:
		case STONE_DRAGON_SCALE_SHIELD:
		case CYAN_DRAGON_SCALE_SHIELD:
		case PSYCHIC_DRAGON_SCALE_SHIELD:
		case RAINBOW_DRAGON_SCALE_SHIELD:
		case BLOOD_DRAGON_SCALE_SHIELD:
		case PLAIN_DRAGON_SCALE_SHIELD:
		case SKY_DRAGON_SCALE_SHIELD:
		case WATER_DRAGON_SCALE_SHIELD:
		case YELLOW_DRAGON_SCALE_SHIELD:

			shieldblockrate = 33;
			break;

		default: shieldblockrate = 0; /* we don't want to call impossible from here --Amy */

		}

		if (uarms->spe > 0) shieldblockrate += (uarms->spe * 2);

		if (uarms->cursed) shieldblockrate /= 2;
		if (uarms->blessed) shieldblockrate += 5;

		if (uarms->spe < 0) shieldblockrate += (uarms->spe * 2);

		if (shieldblockrate < 0) shieldblockrate = 0;

		Sprintf(buf, "%d%%", shieldblockrate);
		dump("  Your chance to block was ", buf );
	}

	if (Fumbling) dump("  ", "You fumbled");
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
		Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
	      Sprintf(eos(buf), " (%d)", HWounded_legs);
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

	if (have_sleepstone())  dump("  ", "You were very tired");
	if (have_cursedmagicresstone()) dump("  ", "You took double damage");

	/*** Vision and senses ***/
	if (See_invisible) dump("  ", "You saw invisible");
	if (Blind_telepat) dump(youwere, "telepathic");

	if (Hallu_party) dump(youwere, "hallu partying");
	if (Drunken_boxing) dump(youwere, "a drunken boxer");
	if (Stunnopathy) dump(youwere, "stunnopathic");
	if (Numbopathy) dump(youwere, "numbopathic");
	if (Freezopathy) dump(youwere, "freezopathic");
	if (Stoned_chiller) dump(youwere, "a stoned chiller");
	if (Corrosivity) dump(youwere, "extremely corrosive");
	if (Fear_factor) dump(youhad, "an increased fear factor");
	if (Burnopathy) dump(youwere, "burnopathic");
	if (Sickopathy) dump(youwere, "sickopathic");
	if (Wonderlegs) dump(youhad, "wonderlegs");
	if (Glib_combat) dump(youwere, "a glibbery fighter");

	if (Warning) dump(youwere, "warned");
	if (Warn_of_mon && flags.warntype) {
		Sprintf(buf, "aware of the presence of %s",
			(flags.warntype & M2_ORC) ? "orcs" :
			(flags.warntype & M2_DEMON) ? "demons" :
			something); 
		dump(youwere, buf);
	}
	if (Undead_warning) dump(youwere, "warned of undead");

	if (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) dump(youwere, "aware of the presence of topmodels");
	if (Role_if(PM_ACTIVISTOR) && uwep && is_quest_artifact(uwep) ) dump(youwere, "aware of the presence of unique monsters");
	/*if (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING ) dump(youwere, "aware of the presence of undead");
	if (uarmh && uarmh->otyp == HELMET_OF_UNDEAD_WARNING ) dump(youwere, "aware of the presence of undead");*/
	if (uamul && uamul->otyp == AMULET_OF_POISON_WARNING ) dump(youwere, "aware of the presence of poisonous monsters");
	if (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING ) dump(youwere, "aware of the presence of same-race monsters");
	if (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING ) dump(youwere, "aware of the presence of covetous monsters");
	if (Role_if(PM_PALADIN) ) dump(youwere, "aware of the presence of demons");
	if (Race_if(PM_VORTEX) ) dump(youwere, "aware of the presence of unsolid creatures");
	if (Race_if(PM_VORTEX) ) dump(youwere, "aware of the presence of creatures without limbs");
	if (Race_if(PM_CORTEX) ) dump(youwere, "aware of the presence of unsolid creatures");
	if (Race_if(PM_CORTEX) ) dump(youwere, "aware of the presence of creatures without limbs");
	if (Race_if(PM_LEVITATOR) ) dump(youwere, "aware of the presence of flying monsters");
	if (Race_if(PM_RODNEYAN) ) dump(youwere, "able to sense monsters possessing coveted objects");
	if (isselfhybrid) dump(youwere, "aware of the presence of strong wanderers");
	if (isselfhybrid) dump(youwere, "aware of the presence of monsters that are valid polymorph forms for monsters only");

	if (Searching) dump(youhad, "automatic searching");
	if (Clairvoyant) {
		Sprintf(buf, "clairvoyant");
	      Sprintf(eos(buf), " (%d)", HClairvoyant);
		dump(youwere, buf);

	}
	if (Infravision) dump(youhad, "infravision");

	if (u.banishmentbeam) dump(youwere, "going to be banished");
	if (u.levelporting) dump(youwere, "going to be levelported");

	if (u.inertia) {
		Sprintf(buf, "slowed by inertia");
	      Sprintf(eos(buf), " (%d)", u.inertia);
		dump(youwere, buf);
	}

	if (u.temprecursion) {
		Sprintf(buf, "under the effect of temporary recursion");
	   	Sprintf(eos(buf), " (%d)", u.temprecursiontime);
		dump(youwere, buf);
	}

	if (Detect_monsters) {
		Sprintf(buf, "sensing the presence of monsters");
	      Sprintf(eos(buf), " (%d)", HDetect_monsters);
		dump(youwere, buf);
	}

	if (u.umconf) dump(youwere, "going to confuse monsters");

	Sprintf(buf, "%d points of nutrition remaining", u.uhunger);
	dump(youhad, buf);

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

	if (Lev_at_will)  {
	    Sprintf(buf, "levitating, at will");
	    Sprintf(eos(buf), " (%d)", HLevitation);
	    dump(youwere, buf);
	}
	if (Levitation)  {
	    Sprintf(buf, "levitating");
	    Sprintf(eos(buf), " (%d)", HLevitation);
	    dump(youwere, buf);
	}

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
	if (Keen_memory) dump("  ", "Your memory was keen");

	if (Sight_bonus) dump("  ", "Your sight was improved");
	if (Versus_curses) dump(youhad, "curse resistance");

	if (Stun_resist) dump(youhad, "stun resistance");
	if (Conf_resist) dump(youhad, "confusion resistance");
	if (Psi_resist) dump(youhad, "psi resistance");
	if (Extra_wpn_practice) dump("  ", "You could train skills and attributes faster");
	if (Death_resistance) dump(youhad, "resistance to death rays");
	if (Double_attack) dump(youhad, "double attacks");
	if (Quad_attack) dump(youhad, "quad attacks");

	if (Half_physical_damage) dump(youhad, "physical resistance");
	if (Half_spell_damage) dump(youhad, "spell resistance");

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
	if (Upolyd && !missingnoprotect) {
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
	if (Fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !have_speedbugstone()) dump(youwere, Very_fast ? "very fast" : "fast");
	if (Fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) ) dump(youwere, Very_fast ? "very slow" : "slow");
	if (!Fast && Very_fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !have_speedbugstone()) dump(youwere, "very fast");
	if (!Fast && Very_fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) ) dump(youwere, "very slow");
	if (Reflecting) dump(youhad, "reflection");
	if (Free_action) dump(youhad, "free action");
	if (Fixed_abil) dump(youhad, "fixed abilities");
	if (uamul && uamul->otyp == AMULET_VERSUS_STONE)
		dump("  ", "You would have been depetrified");
	if (Lifesaved)
		dump("  ", "Your life would have been saved");
	if (Second_chance)
		dump("  ", "You would have been given a second chance");
	if (u.twoweap) dump(youwere, "wielding two weapons at once");

	/*** Miscellany ***/
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%slucky (%d)",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "", Luck);
	    dump(youwere, buf);
	}
	 else dump("  ", "Your luck was zero");
	if (u.moreluck > 0) dump(youhad, "extra luck");
	else if (u.moreluck < 0) dump(youhad, "reduced luck");
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		dump("  ", "Bad luck did not time out for you");
	    if (ltmp >= 0)
		dump("  ", "Good luck did not time out for you");
	}

	if (u.uhealbonus)
	{
		Sprintf(buf, "%s health", u.uhealbonus > 0 ? "extra" :
			"reduced");
	      Sprintf(eos(buf), " (%d)", u.uhealbonus);
	      dump(youhad, buf);
	}
	else dump("  ", "Your health bonus was zero");

	if (recalc_mana() )
	{
		Sprintf(buf, "%s mana", recalc_mana() > 0 ? "extra" :
			"reduced");
	      Sprintf(eos(buf), " (%d)", recalc_mana() );
	      dump(youhad, buf);
	}
	else dump("  ", "Your mana bonus was zero");

	    Sprintf(buf, "Your pantheon consisted of %s, %s and %s", align_gname(A_LAWFUL), align_gname(A_NEUTRAL), align_gname(A_CHAOTIC) );
	    dump("  ", buf);

	    Sprintf(buf, "Your deity was %s", u_gname());
	    dump("  ", buf);

	if (u.ugangr) {
	    Sprintf(buf, " %sangry with you",
		u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : "");
	    Sprintf(eos(buf), " (%d)", u.ugangr);
	    Sprintf(buf2, "%s was %s", u_gname(), buf);
	    dump("  ", buf2);
	}

	    Sprintf(buf, "You could %s have safely prayed", u.ugangr ? "not " : can_pray(FALSE) ? "" : "not ");
	    Sprintf(eos(buf), " (%d)", u.ublesscnt);
	    dump("  ", buf);

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

		Sprintf(eos(statline), "You are %s, a %s %s %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s %s.", plname, align_str(u.ualign.type), (flags.female ? "female" : "male"), (flags.hybridangbander ? "angbander " : ""), (flags.hybridaquarian ? "aquarian " : ""), (flags.hybridcurser ? "curser " : ""), (flags.hybridhaxor ? "haxor " : ""), (flags.hybridhomicider ? "homicider " : ""), (flags.hybridsuxxor ? "suxxor " : ""), (flags.hybridwarper ? "warper " : ""), (flags.hybridrandomizer ? "randomizer " : ""), (flags.hybridnullrace ? "null " : ""), (flags.hybridmazewalker ? "mazewalker " : ""), (flags.hybridsoviet ? "soviet " : ""), (flags.hybridxrace ? "x-race " : ""), (flags.hybridheretic ? "heretic " : ""), (flags.hybridsokosolver ? "sokosolver " : ""), (flags.hybridspecialist ? "specialist " : ""), (flags.hybridamerican ? "american " : ""), (flags.hybridminimalist ? "minimalist " : ""), (flags.hybridnastinator ? "nastinator " : ""), (flags.hybridrougelike ? "rougelike " : ""), urace.adj, (flags.female && urole.name.f) ? urole.name.f : urole.name.m);

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
		enlightenment(0, 1);
	return 0;
}

static const struct menu_tab game_menu[] = {
	{'O', TRUE, doset, "Options"},
	{'r', TRUE, doredraw, "Redraw Screen"},
#ifndef PUBLIC_SERVER
	{'x', TRUE, enter_explore_mode, "Enter Explore Mode"},
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
	else {
	    Sprintf(buf, "used a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    you_have_X(buf);
	}
	if (!u.uconduct.killer)
	    you_have_been("a pacifist");
	else {
		Sprintf(buf, "killed %ld monster%s",
		    u.uconduct.killer, plur(u.uconduct.killer));
	    you_have_X(buf);

	}

	if (!u.uconduct.literate)
	    you_have_been("illiterate");
	else {
	    Sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    you_have_X(buf);
	}

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
	else {
	    Sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    you_have_X(buf);
	}

	if (!u.uconduct.polyselfs)
	    you_have_never("changed form");
	else {
	    Sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    you_have_X(buf);
	}

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
	else {
	    Sprintf(buf, "your vow of celibacy %ld time%s",
		    u.uconduct.celibacy, plur(u.uconduct.celibacy));
	    enl_msg(You_, "have broken ", "broke ", buf);
	}

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

	/* Yes, this is an ungrammatical sentence. It will stay that way because I think it sounds funny. --Amy */
	if (u.uconduct.food) {
	    sprintf(buf, "  You eaten %ld time%s", 
		u.uconduct.food, plur(u.uconduct.food));
	    dump("", buf);
	}
	if (u.uconduct.unvegan) {
	    sprintf(buf, "  You eaten food with animal by-products %ld time%s", 
		u.uconduct.unvegan, plur(u.uconduct.unvegan));
	    dump("", buf);
	}
	if (u.uconduct.unvegetarian) {
	    sprintf(buf, "  You eaten meat %ld time%s", 
		u.uconduct.unvegetarian, plur(u.uconduct.unvegetarian));
	    dump("", buf);
	}

	if (!u.uconduct.gnostic)
	    dump("", "  You were an atheist");
	else {
	    sprintf(buf, "  You have communicated with the gods %ld time%s", 
		    u.uconduct.gnostic, plur(u.uconduct.gnostic));
	    dump("", buf);
	}

	if (!u.uconduct.praydone)
	    dump("", "  You never prayed to the gods");
	else {
	    sprintf(buf, "  You prayed to the gods %ld time%s", 
		    u.uconduct.praydone, plur(u.uconduct.praydone));
	    dump("", buf);
	}

	if (!u.uconduct.weaphit)
	    dump("", "  You never hit with a wielded weapon");
	else {
	    Sprintf(buf, "  You hit with a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    dump("", buf);
	}

	if (!u.uconduct.killer)
	    dump("", "  You were a pacifist");
	else {
	    sprintf(buf, "  You killed %ld monster%s", 
		    u.uconduct.killer, plur(u.uconduct.killer));
	    dump("", buf);
	}

	if (!u.uconduct.literate)
	    dump("", "  You were illiterate");
	else {
	    Sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    dump("  You ", buf);
	}

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

	if (!u.uconduct.celibacy)
	    dump("", "  You remained celibate");
	else {
	    Sprintf(buf, "your vow of celibacy %ld time%s",
		    u.uconduct.celibacy, plur(u.uconduct.celibacy));
	    dump("  You have broken ", buf);
	}

	dump("", "");
}
#endif /* DUMP_LOG */

#endif /* OVLB */
#ifdef OVL1

/* Macros for meta and ctrl modifiers:
 *   M and C return the meta/ctrl code for the given character;
 *     e.g., (C('c') is ctrl-c
 *   ISMETA and ISCTRL return TRUE iff the code is a meta/ctrl code
 *   UNMETA and UNCTRL are the opposite of M/C and return the key for a given
 *     meta/ctrl code. */
#ifndef M
# ifndef NHSTDC
#  define M(c)		(0x80 | (c))
# else
#  define M(c)		((c) - 128)
# endif /* NHSTDC */
#endif
#define ISMETA(c) (((c) & 0x80) != 0)
#define UNMETA(c) ((c) & 0x7f)

#ifndef C
#define C(c)		(0x1f & (c))
#endif
#define ISCTRL(c) ((uchar)(c) < 0x20)
#define UNCTRL(c) (ISCTRL(c) ? (0x60 | (c)) : (c))


/* maps extended ascii codes for key presses to extended command entries in extcmdlist */
static struct key_tab cmdlist[256];

/* list built upon option loading; holds list of keys to be rebound later
 * see "crappy hack" below */
static struct binding_list_tab *bindinglist = NULL;

#define AUTOCOMPLETE TRUE
#define IFBURIED TRUE

#define EXTCMDLIST_SIZE (sizeof(extcmdlist) / sizeof(extcmdlist[1]))

struct ext_func_tab extcmdlist[] = {
	{"2weapon", "toggle two-weapon combat", dotwoweapon, !IFBURIED, AUTOCOMPLETE},
	{"adjust", "adjust inventory letters", doorganize, IFBURIED, AUTOCOMPLETE},
	{"borrow", "steal from monsters", playersteal, IFBURIED, AUTOCOMPLETE},  /* jla */        
	{"chat", "talk to someone", dotalk, IFBURIED, AUTOCOMPLETE},    /* converse? */
	{"conduct", "list which challenges you have adhered to", doconduct, IFBURIED, AUTOCOMPLETE},
	{"dip", "dip an object into something", dodip, !IFBURIED, AUTOCOMPLETE},
	{"enhance", "advance or check weapons skills", enhance_weapon_skill, IFBURIED, AUTOCOMPLETE},
#if 0
//	{"ethics", "list which challenges you have adhered to", doethics, TRUE},
	{"ethics", "list which challenges you have adhered to", doethics, !IFBURIED, AUTOCOMPLETE},
#endif
#ifndef PUBLIC_SERVER
	{"explore", "enter explore mode", enter_explore_mode, IFBURIED, !AUTOCOMPLETE},
#endif
	{"force", "force a lock", doforce, !IFBURIED, AUTOCOMPLETE},
	{"invoke", "invoke an object's powers", doinvoke, IFBURIED, AUTOCOMPLETE},
	{"jump", "jump to a location", dojump, !IFBURIED, AUTOCOMPLETE},
	{"loot", "loot a box on the floor", doloot, IFBURIED, AUTOCOMPLETE},
	{"look", "look at an object", dolook, !IFBURIED, !AUTOCOMPLETE},
	{"monster", "use a monster's special ability", domonability, IFBURIED, AUTOCOMPLETE},
	{"name", "name an item or type of object", ddocall, IFBURIED},
	{"offer", "offer a sacrifice to the gods", dosacrifice, !IFBURIED, AUTOCOMPLETE},
	{"pray", "pray to the gods for help", dopray, IFBURIED, AUTOCOMPLETE},
	{"quit", "exit without saving current game", done2, IFBURIED, AUTOCOMPLETE},

	{"apply", "apply (use) a tool (pick-axe, key, lamp...)", doapply, !IFBURIED},
	{"removeimarkers", "remove all \"I\"s, remembered, unseen creatures from the level", doremoveimarkers, IFBURIED},
	{"attributes", "show your attributes (intrinsic ones included in debug or explore mode)", doattributes, IFBURIED},
	{"close", "close a door", doclose, !IFBURIED},
	{"cast", "zap (cast) a spell", docast, IFBURIED},
	{"discoveries", "show what object types have been discovered", dodiscovered, IFBURIED},
	{"down", "go down a staircase", dodown, !IFBURIED},
	{"drop", "drop an item", dodrop, !IFBURIED},
	{"dropall", "drop specific item types", doddrop, !IFBURIED},
	{"takeoffall", "remove all armor", doddoremarm, !IFBURIED},
	{"inventory", "show your inventory", ddoinv, IFBURIED},
	{"quaff", "quaff (drink) something", dodrink, !IFBURIED},
	{"#", "perform an extended command", doextcmd, IFBURIED},
	{"travel", "Travel to a specific location", dotravel, !IFBURIED},
	{"eat", "eat something", doeat, !IFBURIED},
	{"engrave", "engrave writing on the floor", doengrave, !IFBURIED},
	{"fire", "fire ammunition from quiver", dofire, !IFBURIED},
	{"history", "show long version and game history", dohistory, IFBURIED},
	{"help", "give a help message", dohelp, IFBURIED},
	{"seetrap", "show the type of a trap", doidtrap, IFBURIED},
	{"kick", "kick something", dokick, !IFBURIED},
	{"call", "call (name) a particular monster", ddocall, IFBURIED},
	{"callold", "call (name) a particular monster (vanilla)", do_mname, IFBURIED},
	{"wait", "rest one move while doing nothing", donull, IFBURIED, !AUTOCOMPLETE, "waiting"},
	{"previous", "toggle through previously displayed game messages", doprev_message, IFBURIED},
	{"open", "open a door", doopen, !IFBURIED},
	{"pickup", "pick up things at the current location", dopickup, !IFBURIED},
	{"pay", "pay your shopping bill", dopay, !IFBURIED},
	{"puton", "put on an accessory (ring amulet, etc)", doputon, !IFBURIED},
	{"seeweapon", "show the weapon currently wielded", doprwep, IFBURIED},
	{"seearmor", "show the armor currently worn", doprarm, IFBURIED},
	{"seerings", "show the ring(s) currently worn", doprring, IFBURIED},
	{"seeamulet", "show the amulet currently worn", dopramulet, IFBURIED},
	{"seetools", "show the tools currently in use", doprtool, IFBURIED},
	{"seeall", "show all equipment in use (generally, ),[,=,\",( commands", doprinuse, IFBURIED},
	{"seegold", "count your gold", doprgold, IFBURIED},
	{"glance", "show what type of thing a map symbol on the level corresponds to", doquickwhatis, IFBURIED},
	{"remove", "remove an accessory (ring, amulet, etc)", doremring, !IFBURIED},
	{"read", "read a scroll or spellbook", doread, !IFBURIED},
	{"redraw", "redraw screen", doredraw, IFBURIED},
#ifdef SUSPEND
	{"suspend", "suspend game (only if defined)", dosuspend, IFBURIED},
#endif /* SUSPEND */
	{"setoptions", "show option settings, possibly change them", doset, IFBURIED},
	{"search", "search for traps and secret doors", dosearch, IFBURIED, !AUTOCOMPLETE, "searching"},
	{"save", "save the game", dosave, IFBURIED},
	{"swap", "swap wielded and secondary weapons", doswapweapon, !IFBURIED},
/* Have to put the definition in an ifdef too because "shell" could be rebound
 * to something
 */
#ifndef PUBLIC_SERVER
	{"shell", "do a shell escape (only if defined)", dosh, IFBURIED},
#endif
	{"throw", "throw something", dothrow, !IFBURIED},
	{"takeoff", "take off one piece of armor", dotakeoff, !IFBURIED},
	{"teleport", "teleport around level", dotele, IFBURIED},
	{"inventoryall", "inventory specific item types", dotypeinv, IFBURIED},
	{"autopickup", "toggle the pickup option on/off", dotogglepickup, IFBURIED},
	{"up", "go up a staircase", doup, !IFBURIED},
	{"version", "show version", doversion, IFBURIED},
	{"seespells", "list known spells", dovspell, IFBURIED},
	{"quiver", "select ammunition for quiver", dowieldquiver, !IFBURIED},
	{"whatis", "show what type of thing a symbol corresponds to", dowhatis, IFBURIED},
	{"whatdoes", "tell what a command does", dowhatdoes, IFBURIED},
	{"wield", "wield (put in use) a weapon", dowield, !IFBURIED},
	{"wear", "wear a piece of armor", dowear, !IFBURIED},
	{"zap", "zap a wand", dozap, !IFBURIED},

#ifdef STEED
	{"ride", "ride (or stop riding) a monster", doride, !IFBURIED, AUTOCOMPLETE},
#endif
	{"rub", "rub a lamp or a stone", dorub, !IFBURIED, AUTOCOMPLETE},
#ifdef LIVELOG_SHOUT
	{"shout", "shout something", doshout, !IFBURIED, AUTOCOMPLETE},
#endif
	{"sit", "sit down", dosit, !IFBURIED, AUTOCOMPLETE},
#ifdef SHOUT
	{"shout", "say something loud", doyell, !IFBURIED, AUTOCOMPLETE}, /* jrn */
#endif
	{"technique", "perform a technique", dotech, IFBURIED, AUTOCOMPLETE},
	{"turn", "turn undead", doturn, IFBURIED, AUTOCOMPLETE},
	{"twoweapon", "toggle two-weapon combat", dotwoweapon, !IFBURIED, AUTOCOMPLETE},
	{"untrap", "untrap something", dountrap, !IFBURIED, AUTOCOMPLETE},
	{"vanquished", "list vanquished monsters", dolistvanq, IFBURIED, !AUTOCOMPLETE},
	{"versionext", "list compile time options for this version of Slash'EM", doextversion, IFBURIED, AUTOCOMPLETE},
	{"wipe", "wipe off your face", dowipe, IFBURIED, AUTOCOMPLETE},
	{"youpoly", "polymorph at will", polyatwill, IFBURIED, AUTOCOMPLETE},  /* jla */        
	{"?", "get this list of extended commands", doextlist, IFBURIED, AUTOCOMPLETE},
#if defined(WIZARD)
	/*
	 * There must be a blank entry here for every entry in the table
	 * below.
	 */
#ifdef DISPLAY_LAYERS
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE}, /* #levelchange */
	{(char *)0, (char *)0, donull, TRUE}, /* #lightsources */
#ifdef DEBUG_MIGRATING_MONS
	{(char *)0, (char *)0, donull, TRUE}, /* #migratemons */
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
	{(char *)0, (char *)0, donull, TRUE}, /* #wmode */
	{(char *)0, (char *)0, donull, TRUE}, /* #detect */
	{(char *)0, (char *)0, donull, TRUE}, /* #map */
	{(char *)0, (char *)0, donull, TRUE}, /* #genesis */
	{(char *)0, (char *)0, donull, TRUE}, /* #identify */
	{(char *)0, (char *)0, donull, TRUE}, /* #levelport */
	{(char *)0, (char *)0, donull, TRUE}, /* #wish */
	{(char *)0, (char *)0, donull, TRUE}, /* #gainac */
	{(char *)0, (char *)0, donull, TRUE}, /* #gainlevel */
	{(char *)0, (char *)0, donull, TRUE}, /* #iddqd */
	{(char *)0, (char *)0, donull, TRUE}, /* #where */
#endif
	{(char *)0, (char *)0, donull, TRUE}	/* sentinel */
};

#ifdef WIZARD
static struct ext_func_tab debug_extcmdlist[] = {
#ifdef DISPLAY_LAYERS
	{"display", "detail display layers", wiz_show_display, IFBURIED, AUTOCOMPLETE},
#endif /* DISPLAY_LAYERS */
	{"levelchange", "change experience level", wiz_level_change, IFBURIED, AUTOCOMPLETE},
	{"lightsources", "show mobile light sources", wiz_light_sources, IFBURIED, AUTOCOMPLETE},
#ifdef DEBUG_MIGRATING_MONS
	{"migratemons", "migrate n random monsters", wiz_migrate_mons, IFBURIED, AUTOCOMPLETE},
#endif
	{"detectmons", "detect monsters", wiz_detect_monsters, IFBURIED, AUTOCOMPLETE},
	{"monpolycontrol", "control monster polymorphs", wiz_mon_polycontrol, IFBURIED, AUTOCOMPLETE},
	{"panic", "test panic routine (fatal to game)", wiz_panic, IFBURIED, AUTOCOMPLETE},
	{"polyself", "polymorph self", wiz_polyself, IFBURIED, AUTOCOMPLETE},
#ifdef PORT_DEBUG
	{"portdebug", "wizard port debug command", wiz_port_debug, IFBURIED, AUTOCOMPLETE},
#endif
	{"seenv", "show seen vectors", wiz_show_seenv, IFBURIED, AUTOCOMPLETE},
	{"stats", "show memory statistics", wiz_show_stats, IFBURIED, AUTOCOMPLETE},
	{"timeout", "look at timeout queue", wiz_timeout_queue, IFBURIED, AUTOCOMPLETE},
	{"vision", "show vision array", wiz_show_vision, IFBURIED, AUTOCOMPLETE},
#ifdef DEBUG
	{"wizdebug", "wizard debug command", wiz_debug_cmd, IFBURIED, AUTOCOMPLETE},
#endif
	{"wmode", "show wall modes", wiz_show_wmodes, IFBURIED, AUTOCOMPLETE},
	{"detect", "detect secret doors and traps", wiz_detect, IFBURIED},
	{"map", "do magic mapping", wiz_map, IFBURIED},
	{"genesis", "create monster", wiz_genesis, IFBURIED},
	{"identify", "identify items in pack", wiz_identify, IFBURIED},
	{"levelport", "to trans-level teleport", wiz_level_tele, IFBURIED},
	{"wish", "make wish", wiz_wish, IFBURIED},
	{"gainac", "gain ac", wiz_gain_ac, IFBURIED},
	{"gainlevel", "gain level", wiz_gain_level, IFBURIED},
	{"iddqd", "become invulnerable", wiz_toggle_invulnerability, IFBURIED},
	{"where", "tell locations of special levels", wiz_where, IFBURIED},
	{(char *)0, (char *)0, donull, IFBURIED}

};


static void
bind_key(key, command)
	unsigned char key;
	char* command;
{
	struct ext_func_tab * extcmd;

	/* special case: "nothing" is reserved for unbinding */
	if (!strcmp(command, "nothing")) {
		cmdlist[key].bind_cmd = NULL;
		return;
	}

	for(extcmd = extcmdlist; extcmd->ef_txt; extcmd++) {
		if (strcmp(command, extcmd->ef_txt)) continue;
		cmdlist[key].bind_cmd = extcmd;
		return;
	}

	pline("Bad command %s matched with key %c (ASCII %i). "
		"Ignoring command.\n", command, key, key);
}


static void
init_bind_list(void)
{
	bind_key(C('d'), "kick" ); /* "D" is for door!...?  Msg is in dokick.c */
#ifdef WIZARD
	if (wizard) {
		bind_key(C('e'), "detect" );
		bind_key(C('f'), "map" );
		bind_key(C('g'), "genesis" );
		bind_key(C('i'), "identify" );
		bind_key(C('o'), "where" );
		bind_key(C('v'), "levelport" );
		bind_key(C('w'), "wish" );
		bind_key(C('h'), "detectmons" );
		bind_key(C('c'), "gainac" );
		bind_key(C('j'), "gainlevel" );
		bind_key(C('n'), "iddqd" );
	}
#endif
	bind_key(C('l'), "redraw" ); /* if number_pad is set */
	bind_key(C('p'), "previous" );
	bind_key(C('r'), "redraw" );
	bind_key(C('t'), "teleport" );
	bind_key(C('x'), "attributes" );
	bind_key(C('y'), "youpoly" );
	bind_key(C('b'), "borrow" );
	bind_key(C('s'), "save" );
#ifdef SUSPEND
	if (iflags.qwertz_layout) {
		bind_key(C('y'), "suspend" );
	} else {
		bind_key(C('z'), "suspend" );
	}
#endif
	bind_key('a',    "apply" );
	bind_key('A',    "takeoffall" );
	bind_key(M('a'), "adjust" );
	/*       'b', 'B' : go sw */
	bind_key(M('b'), "borrow" );
	bind_key('c',    "close" );
	bind_key('C',    "callold" );
	bind_key(M('c'), "chat" );
	bind_key('d',    "drop" );
	bind_key('D',    "dropall" );
	bind_key(M('d'), "dip" );
	bind_key('e',    "eat" );
	bind_key('E',    "engrave" );
	bind_key(M('e'), "enhance" );
	bind_key('f',    "fire" );
	/*       'F' : fight (one time) */
	bind_key(M('f'), "force" );
	/*       'g', 'G' : multiple go */
	/*       'h', 'H' : go west */
	bind_key('h',    "help" ); /* if number_pad is set */
	bind_key('i',    "inventory" );
	bind_key('I',    "inventoryall" ); /* Robert Viduya */
	bind_key(M('i'), "invoke" );
	bind_key('j',    "jump" );
	/*       'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N' : move commands */
	bind_key(M('j'), "jump" ); /* if number_pad is on */
	bind_key('k',    "kick" ); /* if number_pad is on */
	bind_key('K',	"vanquished" );  /* if number_pad is on */
	bind_key('l',    "loot" ); /* if number_pad is on */
	bind_key(M('l'), "loot" );
	bind_key(M('m'), "monster" );
	bind_key('N',    "name" );
	/*       'n' prefixes a count if number_pad is on */
	bind_key(M('n'), "name" );
	bind_key(M('N'), "name" ); /* if number_pad is on */
	bind_key('o',    "open" );
	bind_key('O',    "setoptions" );
	bind_key(M('o'), "offer" );
	bind_key('p',    "pay" );
	bind_key('P',    "puton" );
	bind_key(M('p'), "pray" );
	bind_key('q',    "quaff" );
	bind_key('Q',    "quiver" );
	bind_key(M('q'), "quit" );
	bind_key('r',    "read" );
	bind_key('R',    "remove" );
	bind_key(M('r'), "rub" );
	bind_key('s',    "search" );
	bind_key('S',    "save" );
	bind_key(M('s'), "sit" );
	bind_key('t',    "throw" );
	bind_key('T',    "takeoff" );
	bind_key(M('t'), "technique" );
	/*        'u', 'U' : go ne */
	bind_key('u',    "untrap" ); /* if number_pad is on */
	bind_key(M('u'), "untrap" );
	bind_key(C('U'), "removeimarkers" );
	bind_key('v',    "version" );
	bind_key('V',    "history" );
	bind_key(M('v'), "versionext" );
	bind_key('w',    "wield" );
	bind_key('W',    "wear" );
	bind_key(M('w'), "wipe" );
	bind_key('x',    "swap" );
	bind_key('X',    "twoweapon" );
	/*bind_key('X',    "explore_mode" );*/
	/*        'y', 'Y' : go nw */
#ifdef STICKY_COMMAND
	if (iflags.qwertz_layout) {
		bind_key(M('z'), "sticky" );
	} else {
		bind_key(M('y'), "sticky" );
	}
#endif /* STICKY_COMMAND */
	if (iflags.qwertz_layout) {
		bind_key('y',	"zap" );
		bind_key('Y',	"cast" );
	} else {
		bind_key('z',    "zap" );
		bind_key('Z',    "cast" );
	}
	bind_key('<',    "up" );
	bind_key('>',    "down" );
	bind_key('/',    "whatis" );
	bind_key('&',    "whatdoes" );
	bind_key('?',    "help" );
	bind_key(M('?'), "?" );
#ifndef PUBLIC_SERVER
	bind_key('!',    "shell" );
#endif
	bind_key('.',    "wait" );
	bind_key(' ',    "wait" );
	bind_key(',',    "pickup" );
	bind_key(':',    "look" );
	bind_key(';',    "glance" );
	bind_key('^',    "seetrap" );
	bind_key('\\',   "discoveries" ); /* Robert Viduya */
	bind_key('@',    "autopickup" );
	bind_key(M('2'), "twoweapon" );
	bind_key(WEAPON_SYM, "seeweapon" );
	bind_key(ARMOR_SYM,  "seearmor" );
	bind_key(RING_SYM,   "seerings" );
	bind_key(AMULET_SYM, "seeamulet" );
	bind_key(TOOL_SYM,   "seetools" );
	bind_key('*',        "seeall" ); /* inventory of all equipment in use */
	bind_key(GOLD_SYM,   "seegold" );
	bind_key(SPBOOK_SYM, "seespells" ); /* Mike Stephenson */
	bind_key('#', "#");
	bind_key('_', "travel");
}

/* takes the list of bindings loaded from the options file, and changes cmdlist
 * to match it */
static void
change_bind_list(void)
{
	struct binding_list_tab *binding;

	/* TODO: they must be loaded forward, not backward as they are now */
	while ((binding = bindinglist)) {
		bindinglist = bindinglist->next;
		bind_key(binding->key, binding->extcmd);
		free(binding->extcmd);
		free(binding);
	}
}


/*
 * Insert debug commands into the extended command list.  This function
 * assumes that the last entry will be the help entry.
 *
 * You must add entries in ext_func_tab every time you add one to the
 * debug_extcmdlist().
 */
static void
add_debug_extended_commands()
{
	int i, j, k, n;

	/* count the # of help entries */
	for (n = 0; extcmdlist[n].ef_txt; n++) ;

	for (i = 0; debug_extcmdlist[i].ef_txt; i++) {
	    extcmdlist[n + i] = debug_extcmdlist[i];
	}
}

/* list all keys and their bindings, like dat/hh but dynamic */
void
dokeylist(void)
{
	char	buf[BUFSZ], buf2[BUFSZ];
	uchar	key;
	boolean keys_used[256] = {0};
	register const char*	dir_keys;
	winid	datawin;
	int	i;
	char*	dir_desc[10] = {"move west",
				"move northwest",
				"move north",
				"move northeast",
				"move east",
				"move southeast",
				"move south",
				"move southwest",
				"move downward",
				"move upward"};
	char*	misc_desc[MISC_CMD_COUNT] = 
		{"rush until something interesting is seen",
		 "run until something extremely interesting is seen",
		 "fight even if you don't see a monster",
		 "move without picking up objects/fighting",
		 "run without picking up objects/fighting",
		 "escape from the current query/action"
#ifdef REDO
		 , "redo the previous command"
#endif
		};


	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
	putstr(datawin, 0, "            Full Current Key Bindings List");
	putstr(datawin, 0, "");

	/* directional keys */
	if (iflags.num_pad) dir_keys = ndir;
	else dir_keys = sdir;
	putstr(datawin, 0, "Directional keys:");
	{
	  Sprintf(buf, "  %c %c %c", dir_keys[1], dir_keys[2], dir_keys[3]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "   \\|/");
	  Sprintf(buf, "  %c-.-%c", dir_keys[0], dir_keys[4]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "   /|\\");
	  Sprintf(buf, "  %c %c %c", dir_keys[7], dir_keys[6], dir_keys[5]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "");
	  Sprintf(buf, "    %c  up", dir_keys[9]);
	  putstr(datawin, 0, buf);
	  Sprintf(buf, "    %c  down", dir_keys[8]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "");
	}
	for (i = 0; i < 10; i++) {
		key = dir_keys[i];
		keys_used[key] = TRUE;
		if (!iflags.num_pad) {
			keys_used[toupper(key)] = TRUE;
			keys_used[C(key)] = TRUE;
		}
		/*
		Sprintf(buf, "%c\t%s", key, dir_desc[i]);
		putstr(datawin, 0, buf);
		*/
	}
	if (!iflags.num_pad) {
		putstr(datawin, 0, "Shift-<direction> will move in specified direction until you hit");
		putstr(datawin, 0, "        a wall or run into something.");
		putstr(datawin, 0, "Ctrl-<direction> will run in specified direction until something");
		putstr(datawin, 0, "        very interesting is seen.");
	}
	putstr(datawin, 0, "");

	/* special keys -- theoretically modifiable but many are still hard-coded*/
	putstr(datawin, 0, "Miscellaneous keys:");
	for (i = 0; i < MISC_CMD_COUNT; i++) {
		key = misc_cmds[i];
		keys_used[key] = TRUE;
		Sprintf(buf, "%s\t%s", key2txt(key, buf2), misc_desc[i]);
		putstr(datawin, 0, buf);
	}
	putstr(datawin, 0, "");

	/* more special keys -- all hard-coded */
#ifndef NO_SIGNAL
	putstr(datawin, 0, "^c\tbreak out of nethack (SIGINT)");
	keys_used[C('c')] = TRUE;
	if (!iflags.num_pad) putstr(datawin, 0, "");
#endif
	if (iflags.num_pad) {
		putstr(datawin, 0, "-\tforce fight (same as above)");
		putstr(datawin, 0, "5\trun (same as above)");
		putstr(datawin, 0, "0\tinventory (as #inventory)");
		keys_used['-'] = keys_used['5'] = keys_used['0'] = TRUE;
		putstr(datawin, 0, "");
	}

	/* command keys - can be rebound or remapped*/
	putstr(datawin, 0, "Command keys:");
	for(i=0; i<=255; i++) {
		struct ext_func_tab * extcmd;
		char* mapping;
		key = i;
		/* JDS: not the most efficient way, perhaps */
		if (keys_used[i]) continue;
		if (key == ' ' && !flags.rest_on_space) continue;
		if ((extcmd = cmdlist[i].bind_cmd)) {
			Sprintf(buf, "%s\t%s", key2txt(key, buf2),
				extcmd->ef_desc);
			putstr(datawin, 0, buf);
		}
	}
	putstr(datawin, 0, "");

	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
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

static int
compare_commands(_cmd1, _cmd2)
     /* a wrapper function for strcmp.  Can this be done more simply? */
     void *_cmd1, *_cmd2;
{
	struct ext_func_tab *cmd1 = _cmd1, *cmd2 = _cmd2;

	return strcmp(cmd1->ef_txt, cmd2->ef_txt);
}

void
commands_init(void)
{
	int count = 0;

#ifdef WIZARD
	if (wizard) add_debug_extended_commands();
#endif
	while(extcmdlist[count].ef_txt) count++;

	qsort(extcmdlist, count, sizeof(struct ext_func_tab),
	      &compare_commands);

	init_bind_list();	/* initialize all keyboard commands */
	change_bind_list();	/* change keyboard commands based on options */
}

/* returns a one-byte character from the text (it may massacre the txt
 * buffer) */
char
txt2key(txt)
     char* txt;
{
	txt = stripspace(txt);
	if (!*txt) return 0;

	/* simple character */
	if (!txt[1]) return txt[0];

	/* a few special entries */
	if (!strcmp(txt, "<enter>")) return '\n';
	if (!strcmp(txt, "<space>")) return ' ';
	if (!strcmp(txt, "<esc>"))   return '\033';

	/* control and meta keys */
	switch (*txt) {
	    case 'm': /* can be mx, Mx, m-x, M-x */
	    case 'M':
		    txt++;
		    if(*txt == '-' && txt[1]) txt++;
		    if (txt[1]) return 0;
		    return M( *txt );
	    case 'c': /* can be cx, Cx, ^x, c-x, C-x, ^-x */
	    case 'C':
	    case '^':
		    txt++;
		    if(*txt == '-' && txt[1]) txt++;
		    if (txt[1]) return 0;
		    return C( *txt );
	}

	/* ascii codes: must be three-digit decimal */
	if (*txt >= '0' && *txt <= '9') {
		uchar key = 0;
		int i;
		for(i = 0; i < 3; i++) {
			if(txt[i]<'0' || txt[i]>'9') return 0;
			key = 10 * key + txt[i]-'0';
		}
		return key;
	}

	return 0;
}

/* returns the text for a one-byte encoding
 * must be shorter than a tab for proper formatting */
char*
key2txt(c, txt)
     char c;
     char* txt; /* sufficiently long buffer */
{
	if (c == ' ')
		Sprintf(txt, "<space>");
	else if (c == '\033')
		Sprintf(txt, "<esc>");
	else if (c == '\n')
		Sprintf(txt, "<enter>");
	else if (ISCTRL(c))
		Sprintf(txt, "^%c", UNCTRL(c));
	else if (ISMETA(c))
		Sprintf(txt, "M-%c", UNMETA(c));
	else if (c >= 33 && c <= 126)
		Sprintf(txt, "%c", c);		/* regular keys: ! through ~ */
	else
		Sprintf(txt, "A-%i", c);	/* arbitrary ascii combinations */
	return txt;
}

/* returns the text for a string of one-byte encodings */
char*
str2txt(s, txt)
     char* s;
     char* txt;
{
	char* buf = txt;
	
	while (*s) {
		(void) key2txt(*s, buf);
		buf = eos(buf);
		*buf = ' ';
		buf++;
		*buf = 0;
		s++;
      	}
	return txt;
}


/* strips leading and trailing whitespace */
char*
stripspace(txt)
     char* txt;
{
	char* end;
	while (isspace(*txt)) txt++;
	end = eos(txt);
	while (--end >= txt && isspace(*end)) *end = 0;
	return txt;
}

void
parsebindings(bindings)
     /* closely follows parseoptions in options.c */
     char* bindings;
{
	char *bind;
	char key;
	struct binding_list_tab *newbinding = NULL;

	/* break off first binding from the rest; parse the rest */
	if ((bind = index(bindings, ',')) != 0) {
		*bind++ = 0;
		parsebindings(bind);
	}
 
	/* parse a single binding: first split around : */
	if (! (bind = index(bindings, ':'))) return; /* it's not a binding */
	*bind++ = 0;

	/* read the key to be bound */
	key = txt2key(bindings);
	if (!key) {
		raw_printf("Bad binding %s.", bindings);
		wait_synch();
		return;
	}
	
	/* JDS: crappy hack because wizard mode information
	 * isn't read until _after_ key bindings are read,
	 * and to change this would cause numerous side effects.
	 * instead, I save a list of rebindings, which are later
	 * bound. */
	bind = stripspace(bind);
	newbinding = (struct binding_list_tab *)alloc(sizeof(*newbinding));
	newbinding->key = key;
	newbinding->extcmd = (char *)alloc(strlen(bind)+1);
	strcpy(newbinding->extcmd, bind);;
	newbinding->next = bindinglist;
	bindinglist = newbinding;
}
 
void
parseautocomplete(autocomplete,condition)
     /* closesly follows parsebindings and parseoptions */
     char* autocomplete;
     boolean condition;
{
	register char *autoc;
	int i;
	
	/* break off first autocomplete from the rest; parse the rest */
	if ((autoc = index(autocomplete, ','))
	    || (autoc = index(autocomplete, ':'))) {
		*autoc++ = 0;
		parseautocomplete(autoc, condition);
	}

	/* strip leading and trailing white space */
	autocomplete = stripspace(autocomplete);
	
	if (!*autocomplete) return;

	/* take off negations */
	while (*autocomplete == '!') {
		/* unlike most options, a leading "no" might actually be a part of
		 * the extended command.  Thus you have to use ! */
		autocomplete++;
		condition = !condition;
	}

	/* find and modify the extended command */
	/* JDS: could be much faster [O(log n) vs O(n)] if done differently */
	for (i=0; extcmdlist[i].ef_txt; i++) {
		if (strcmp(autocomplete, extcmdlist[i].ef_txt)) continue;
		extcmdlist[i].autocomplete = condition;
		return;
	}

#ifdef WIZARD
	/* do the exact same thing with the wizmode list */
	/* this is a hack because wizard-mode commands haven't been loaded yet when
	 * this code is run.  See "crappy hack" elsewhere. */
	for (i=0; debug_extcmdlist[i].ef_txt; i++) {
		if (strcmp(autocomplete, debug_extcmdlist[i].ef_txt)) continue;
		debug_extcmdlist[i].autocomplete = condition;
		return;
	}
#endif

	/* not a real extended command */
	raw_printf ("Bad autocomplete: invalid extended command '%s'.", autocomplete);
	wait_synch();
}


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
	if (*cmd == DOESCAPE) { /* <esc> key - user might be panicking */
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
	if (*cmd == DORUSH) {
	    if (movecmd(cmd[1])) {
		flags.run = 2;
		do_rush = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if ( (*cmd == '5' && iflags.num_pad)
		    || *cmd == DORUN) {
	    if (movecmd(lowc(cmd[1]))) {
		flags.run = 3;
		do_rush = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if ( (*cmd == '-' && iflags.num_pad)
		    || *cmd == DOFORCEFIGHT) {
		/* Effects of movement commands and invisible monsters:
		 * m: always move onto space (even if 'I' remembered)
		 * F: always attack space (even if 'I' not remembered)
		 * normal movement: attack if 'I', move otherwise
		 */
	    if (movecmd(cmd[1])) {
		flags.forcefight = 1;
		do_walk = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if (*cmd == DONOPICKUP) {
	    if (movecmd(cmd[1]) || u.dz) {
		flags.run = 0;
		flags.nopick = 1;
		if (!u.dz) do_walk = TRUE;
		else cmd[0] = cmd[1];	/* "m<" or "m>" */
	    } else
		prefix_seen = TRUE;
	} else if (*cmd == DORUN_NOPICKUP) {
	    if (movecmd(lowc(cmd[1]))) {
		flags.run = 1;
		flags.nopick = 1;
		do_rush = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if (*cmd == '0' && iflags.num_pad) {
	    (void)ddoinv(); /* a convenience borrowed from the PC */
	    flags.move = FALSE;
	    multi = 0;
	} else if (*cmd == CMD_TRAVEL && iflags.travelcmd) {
	  flags.travel = 1;
	  iflags.travel1 = 1;
	  flags.run = 8;
	  flags.nopick = 1;
	  do_rush = TRUE;
	} else {
	    if (movecmd(*cmd)) {	/* ordinary movement */
		do_walk = TRUE;
	    } else if (movecmd(iflags.num_pad ?
			       UNMETA(*cmd) : lowc(*cmd))) {
		flags.run = 1;
		do_rush = TRUE;
	    } else if (movecmd(UNCTRL(*cmd))) {
		flags.run = 3;
		do_rush = TRUE;
	    }
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
	} else if (prefix_seen && cmd[1] == DOESCAPE) {	/* <prefix><escape> */
	    /* don't report "unknown command" for change of heart... */
	    bad_command = FALSE;
	} else if (*cmd == ' ' && !flags.rest_on_space) {
	    bad_command = TRUE;		/* skip cmdlist[] loop */
	/* handle bound commands */
	} else {
	    const struct key_tab *keytab = &cmdlist[(unsigned char)*cmd];
	    if (keytab->bind_cmd != NULL) {
		struct ext_func_tab *extcmd = keytab->bind_cmd;
		int res, NDECL((*func));		
		if (u.uburied && !extcmd->can_if_buried) {
		    You_cant("do that while you are buried!");
		    res = 0;
		} else {
		    func = extcmd->ef_funct;
		    if (extcmd->f_text && !occupation && multi)
		    set_occupation(func, extcmd->f_text, multi);
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

	if (u.totter || u.uprops[TOTTER_EFFECT].extrinsic || TotterTrapEffect || have_directionswapstone() ) {

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

	if (foo == DOESCAPE) {   /* esc cancels count (TH) */
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

	if (foo == DORUSH || foo == DORUN || foo == DOFORCEFIGHT
	    || foo == DONOPICKUP || foo == DORUN_NOPICKUP
	    || (iflags.num_pad && (foo == '5' || foo == '-'))) {
	    foo = readchar();
#ifdef REDO
	    savech((char)foo);
#endif
	    in_line[1] = foo;
	    in_line[2] = 0;
	}
	clear_nhwindow(WIN_MESSAGE);

	if (prezero) in_line[0] = DOESCAPE;
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

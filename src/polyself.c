/*	SCCS Id: @(#)polyself.c	3.4	2003/01/08	*/
/*	Copyright (C) 1987, 1988, 1989 by Ken Arromdee */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Polymorph self routine.
 *
 * Note:  the light source handling code assumes that both youmonst.m_id
 * and youmonst.mx will always remain 0 when it handles the case of the
 * player polymorphed into a light-emitting monster.
 */

#include "hack.h"

#ifdef OVLB
STATIC_DCL void polyman(const char *,const char *);
STATIC_DCL void break_armor(void);
STATIC_DCL void drop_weapon(int);
STATIC_DCL void uunstick(void);
STATIC_DCL int armor_to_dragon(int);
/*STATIC_DCL void newman(void);*/
STATIC_DCL void merge_with_armor(void);

/*  Not Used 
static void special_poly(void);
*/

/* Assumes u.umonster is set up already */
/* Use u.umonster since we might be restoring and you may be polymorphed */
void
init_uasmon()
{
	int i;

	upermonst = mons[u.umonster];

	/* Fix up the flags */
	/* Default flags assume human,  so replace with your race's flags */

	upermonst.mflags1 &= ~(mons[PM_HUMAN].mflags1);
	upermonst.mflags1 |= (mons[urace.malenum].mflags1);

	upermonst.mflags2 &= ~(mons[PM_HUMAN].mflags2);
	upermonst.mflags2 |= (mons[urace.malenum].mflags2);

	upermonst.mflags3 &= ~(mons[PM_HUMAN].mflags3);
	upermonst.mflags3 |= (mons[urace.malenum].mflags3);
	
	/* Fix up the attacks */
	for(i = 0; i < NATTK; i++) {
	    upermonst.mattk[i] = mons[urace.malenum].mattk[i];
	}
	
	set_uasmon();
}

/* update the youmonst.data structure pointer */
void
set_uasmon()
{
	set_mon_data(&youmonst, ((u.umonnum == u.umonster) ? 
					&upermonst : &mons[u.umonnum]), 0);

	bot_reconfig();
}

/* make a (new) human out of the player */
STATIC_OVL void
polyman(fmt, arg)
const char *fmt, *arg;
{
	boolean sticky = sticks(youmonst.data) && u.ustuck && !u.uswallow,
		was_mimicking = (youmonst.m_ap_type == M_AP_OBJECT);
	boolean could_pass_walls = Passes_walls;
	boolean was_blind = !!Blind;

	if (Upolyd) {
		/*u.acurr = u.macurr;*/	/* restore old attribs */
		/*u.amax = u.mamax;*/ /* decided to take this out --Amy */
		u.umonnum = u.umonster;
		flags.female = u.mfemale;
	}

	set_uasmon();

	u.mh = u.mhmax = 0;
	u.mtimedone = 0;
	skinback(FALSE);
	u.uundetected = 0;
	newsym(u.ux,u.uy);

	if (sticky) uunstick();
	find_ac();
	if (was_mimicking) {
	    if (multi < 0) unmul("");
	    youmonst.m_ap_type = M_AP_NOTHING;
	}

	newsym(u.ux,u.uy);

	
	You(fmt, arg);
	/* check whether player foolishly genocided self while poly'd */
	if ((mvitals[urole.malenum].mvflags & G_GENOD) ||
			(urole.femalenum != NON_PM &&
			(mvitals[urole.femalenum].mvflags & G_GENOD)) ||
			(mvitals[urace.malenum].mvflags & G_GENOD) ||
			(urace.femalenum != NON_PM &&
			(mvitals[urace.femalenum].mvflags & G_GENOD))) {
	    /* intervening activity might have clobbered genocide info */
	    killer = delayed_killer;
	    if (!killer || !strstri(killer, "genocid")) {
		killer_format = KILLED_BY;
		killer = "self-genocide";
	    }
	    done(GENOCIDED);
	}

	if (u.twoweap && !could_twoweap(youmonst.data))
	    untwoweapon();

	if (u.utraptype == TT_PIT) {
	    if (could_pass_walls) {	/* player forms cannot pass walls */
		u.utrap = rn1(6,2);
	    }
	}
	if (was_blind && !Blind) {	/* reverting from eyeless */
	    Blinded = 1L;
	    make_blinded(0L, TRUE);	/* remove blindness */
	}

	if(!Levitation && !u.ustuck &&
	   (is_waterypool(u.ux,u.uy) || is_watertunnel(u.ux,u.uy) || is_lava(u.ux,u.uy)))
		spoteffects(TRUE);

	see_monsters();
}

void
change_sex()
{
	/* setting u.umonster for caveman/cavewoman or priest/priestess
	   swap unintentionally makes `Upolyd' appear to be true */
	boolean already_polyd = (boolean) Upolyd;

	/* Some monsters are always of one sex and their sex can't be changed */
	/* succubi/incubi can change, but are handled below */
	/* !already_polyd check necessary because is_male() and is_female()
           are true if the player is a priest/priestess */
	if (!already_polyd || (!is_male(youmonst.data) && !is_female(youmonst.data) && !is_neuter(youmonst.data)))
	    flags.female = !flags.female;
	if (already_polyd)	/* poly'd: also change saved sex */
	    u.mfemale = !u.mfemale;
	max_rank_sz();		/* [this appears to be superfluous] */
	if ((already_polyd ? u.mfemale : flags.female) && urole.name.f)
	    strcpy(pl_character, urole.name.f);
	else
	    strcpy(pl_character, urole.name.m);
	u.umonster = ((already_polyd ? u.mfemale : flags.female) && urole.femalenum != NON_PM) ?
			urole.femalenum : urole.malenum;

	if (!already_polyd) {
	    u.umonnum = u.umonster;
	} else if (u.umonnum == PM_SUCCUBUS || u.umonnum == PM_INCUBUS) {
	    flags.female = !flags.female;
	    /* change monster type to match new sex */
	    u.umonnum = (u.umonnum == PM_SUCCUBUS) ? PM_INCUBUS : PM_SUCCUBUS;
	}
	set_uasmon();
}

void
newman()
{
	int tmp, oldlvl;

	if (Race_if(PM_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) ) {
	    if (!rn2(10)) change_sex();
	} else {
	tmp = u.uhpmax;
	oldlvl = u.ulevel;
	u.ulevel = u.ulevel + rn1(5, -2);
	if (u.ulevel > 127 || u.ulevel < 1) { /* level went below 0? */
	    u.ulevel = oldlvl; /* restore old level in case they lifesave */
	    if (!Race_if(PM_UNGENOMOLD) && !Race_if(PM_MOULD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_WARPER) && !Race_if(PM_MISSINGNO)) goto dead;
	}
	/* Moulds, including ungenomolds, are resistant to bad polymorphs but have uncurable polymorphitis. --Amy
	   They cannot suffer from system shock either. Since ungenomolds automatically genocide their own race
	   upon starting the game, their first polymorph _needs_ to be into a species other than ungenomold. */

	if (u.ulevel > MAXULEV) u.ulevel = MAXULEV;
	/* If your level goes down, your peak level goes down by
	   the same amount so that you can't simply use blessed
	   full healing to undo the decrease.  But if your level
	   goes up, your peak level does *not* undergo the same
	   adjustment; you might end up losing out on the chance
	   to regain some levels previously lost to other causes. */
	if (u.ulevel < oldlvl) u.ulevelmax -= (oldlvl - u.ulevel);
	if (u.ulevelmax < u.ulevel) u.ulevelmax = u.ulevel;

	if (!rn2(10)) change_sex();

	adjabil(oldlvl, (int)u.ulevel);
	reset_rndmonst(NON_PM);	/* new monster generation criteria */

	/* random experience points for the new experience level */
	u.uexp = rndexp(FALSE);

	/* u.uhpmax * u.ulevel / oldlvl: proportionate hit points to new level
	 * -10 and +10: don't apply proportionate HP to 10 of a starting
	 *   character's hit points (since a starting character's hit points
	 *   are not on the same scale with hit points obtained through level
	 *   gain)
	 * 9 - rn2(19): random change of -9 to +9 hit points
	 */
#ifndef LINT
	if (u.ulevel <= oldlvl)	/* drain for gain exploit fixed by Amy */
	u.uhpmax = ((u.uhpmax - 10) * (long)u.ulevel / oldlvl + 10) +
		(9 - rn2(19));

	else
	u.uhpmax = (u.uhpmax + rnd(12));

#endif

#ifdef LINT
	u.uhp = u.uhp + tmp;
#else
	u.uhp = u.uhp * (long)u.uhpmax/tmp;
#endif

	tmp = u.uenmax;
#ifndef LINT
	if (u.ulevel <= oldlvl)	/* drain for gain exploit fixed by Amy */
	u.uenmax = u.uenmax * (long)u.ulevel / oldlvl + 9 - rn2(19);

	else
	u.uenmax = (u.uenmax + rnd(5));

#endif
	if (u.uenmax < 0) u.uenmax = 0;
#ifndef LINT
	u.uen = (tmp ? u.uen * (long)u.uenmax / tmp : u.uenmax);
#endif
	}

	redist_attr();
	u.uhunger = rn1(750,750);
	if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
	Sick = 0;
	Stoned = 0;
	delayed_killer = 0;
	if (Race_if(PM_DOPPELGANGER) || Race_if(PM_HEMI_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) ) {
		if (u.uhp <= 10) u.uhp = 10;
		if (u.uhpmax <= 10) u.uhpmax = 10;
		if (u.uen <= u.ulevel) u.uen = u.ulevel;
		if (u.uenmax <= u.ulevel) u.uenmax = u.ulevel;
	}
	if (u.uhp <= 0 || u.uhpmax <= 0) {

		/* Amy edit: We allow polymorph traps and such stuff to generate as early as dungeon level 1. This can
		 * result in the player's attributes being redistributed, including possibly death, so we'll be nice and
		 * prevent such a death if the turn counter is below 1000. After all, we fixed that stupid drain for gain
		 * exploit and therefore this should not be abusable. If it is, I'll devise another fix :P */

		if (Polymorph_control || Race_if(PM_MOULD) || Race_if(PM_DEATHMOLD) || Race_if(PM_MISSINGNO) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_WARPER) || Race_if(PM_UNGENOMOLD) || u.polyprotected ) {
		    if (u.uhp <= 0) u.uhp = 1;
		    if (u.uhpmax <= 0) u.uhpmax = 1;
		} else {
dead: /* we come directly here if their experience level went to 0 or less */
			u.youaredead = 1;
		    Your("new form doesn't seem healthy enough to survive.");
		    killer_format = KILLED_BY_AN;
		    killer="unsuccessful polymorph";
		    done(DIED);
			u.youaredead = 0;
		    newuhs(FALSE);
		    return; /* lifesaved */
		}
	}
	newuhs(FALSE);
	polyman("feel like a new %s!",
		(flags.female && urace.individual.f) ? urace.individual.f :
		(urace.individual.m) ? urace.individual.m : urace.noun);
	u.cnd_newmancount++;

	/* polyselfing into a new man is supposed to be the only way to reroll your areola diameter --Amy */
	u.areoladiameter = 20;
	if (rn2(10)) {
		if (rn2(2)) {
			u.areoladiameter += rnd(10);
			while (!rn2(2)) {
				u.areoladiameter++;
			}
		}
		else {
			u.areoladiameter -= rnd(10);
			while (!rn2(2)) {
				u.areoladiameter--;
			}
		}
	}
	if (u.areoladiameter < 1) u.areoladiameter = 1; /* fail safe */
	if (u.areoladiameter > 30) u.areoladiameter = 30; /* fail safe */

	use_skill(P_POLYMORPHING, rnd(5));
	if (!Upolyd) u.polyformed = 0;
	if (Slimed) {
		Your("body transforms, but there is still slime on you.");
		Slimed = 100;
	}
	flags.botl = 1;
	vision_full_recalc = 1;
	(void) encumber_msg();
	see_monsters();
	exprecalc();
}

void
polyself(forcecontrol)
boolean forcecontrol;     
{
	char buf[BUFSZ];
	int old_light, new_light;
	int mntmp = NON_PM;
	int tries=0;
	boolean draconian = (uarm &&
				uarm->otyp >= GRAY_DRAGON_SCALE_MAIL &&
				uarm->otyp <= YELLOW_DRAGON_SCALES);
	boolean iswere = (u.ulycn >= LOW_PM || is_were(youmonst.data));
	boolean isvamp = (is_vampire(youmonst.data) && !Race_if(PM_VAMGOYLE) && issoviet );
	boolean was_floating = (Levitation || Flying);
	boolean canhavesemicontrol = TRUE;

	if (Race_if(PM_MISSINGNO)) {
		reinitmissingno();
		init_uasmon();

		if (Upolyd) {
			rehumanize();
			return;
		}
	}

	if (Unchanging) {
		if (!wizard || !forcecontrol) return;
	}

	if (!CannotBeUnchanging && uarmh && uarmh->oartifact == ART_MUTATIONPREVENT && rn2(2)) {
		if (!wizard || !forcecontrol) {
			Your("hat prevents the polymorph!");
			return;
		}
	}

	if (Race_if(PM_MOULD) || Race_if(PM_TRANSFORMER) || Race_if(PM_POLYINITOR) || Race_if(PM_DESTABILIZER) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_WARPER) || Race_if(PM_MISSINGNO) || Race_if(PM_UNGENOMOLD) || Race_if(PM_DEATHMOLD) || Race_if(PM_AK_THIEF_IS_DEAD_) ) {
		canhavesemicontrol = FALSE;
	}

	if (Race_if(PM_PLAYER_SLIME)) { /* cannot polymorph at all - punishment for being slimed --Amy */
		newman();
		return;
	}
	if (Race_if(PM_INCORPOREALOID)) { /* cannot polymorph at all, because amateurhour wants it so --Amy */
		return;
	}
	/* and you should be grateful, because in vanilla the game would just have ended... */

	/* [Tom] I made the chance of dying from Con check only possible for
		 really weak people (it was out of 20) */

	/* Amy edit: early polymorph traps shouldn't kill you by system shock either because that would suck */

	if(!Polymorph_control && !(tech_inuse(T_POLYFORM) || tech_inuse(T_FUNGOISM) || u.fungalsandals || tech_inuse(T_BECOME_UNDEAD)) && !u.wormpolymorph && !forcecontrol && !draconian && !iswere &&
			!(isvamp && issoviet) && !Race_if(PM_DOPPELGANGER) && !Role_if(PM_SHAPESHIFTER) && !Race_if(PM_HEMI_DOPPELGANGER)) {
		if ( (rn2(12) > ACURR(A_CON) || !rn2(50)) && !u.polyprotected && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_MOULD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_MISSINGNO) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_WARPER) ) {

		You("%s", shudder_for_moment);
		losehp(rnd(30), "system shock", KILLED_BY_AN);
		exercise(A_CON, FALSE);
		return;
	    }
	}
	old_light = Upolyd ? emits_light(youmonst.data) : 0;

	if (tech_inuse(T_POLYFORM) || u.castlecrushglitch) {

		do {
			mntmp = rn2(NUMMONS);
		} while(( (notake(&mons[mntmp]) && rn2(4) ) || ((mons[mntmp].mlet == S_BAT) && rn2(2)) || ((mons[mntmp].mlet == S_EYE) && rn2(2) ) || ((mons[mntmp].mmove == 1) && rn2(4) ) || ((mons[mntmp].mmove == 2) && rn2(3) ) || ((mons[mntmp].mmove == 3) && rn2(2) ) || ((mons[mntmp].mmove == 4) && !rn2(3) ) || ( (mons[mntmp].mlevel < 10) && ((mons[mntmp].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[mntmp]) && rn2(2) ) || ( is_nonmoving(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(5) ) || ( is_nonmoving(&mons[mntmp]) && rn2(20) ) || (is_jonadabmonster(&mons[mntmp]) && rn2(20)) || ( uncommon2(&mons[mntmp]) && !rn2(4) ) || ( uncommon3(&mons[mntmp]) && !rn2(3) ) || ( uncommon5(&mons[mntmp]) && !rn2(2) ) || ( uncommon7(&mons[mntmp]) && rn2(3) ) || ( uncommon10(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(20) ) ) );

		if (polymon(mntmp)) u.polyformed = 1;

		if (!uarmg || FingerlessGloves) selftouch("No longer petrify-resistant, you");

		u.wormpolymorph = 0;

		goto made_change;
	}

	if (tech_inuse(T_FUNGOISM) || u.fungalsandals) {

		do {
			mntmp = rn2(NUMMONS);
		} while(( (notake(&mons[mntmp]) && rn2(4) ) || !polyok(&mons[mntmp]) || (mons[mntmp].mlet != S_FUNGUS) || ((mons[mntmp].mmove == 1) && rn2(4) ) || ((mons[mntmp].mmove == 2) && rn2(3) ) || ((mons[mntmp].mmove == 3) && rn2(2) ) || ((mons[mntmp].mmove == 4) && !rn2(3) ) || ( (mons[mntmp].mlevel < 10) && ((mons[mntmp].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[mntmp]) && rn2(2) ) || ( is_nonmoving(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(5) ) || ( is_nonmoving(&mons[mntmp]) && rn2(20) ) || (is_jonadabmonster(&mons[mntmp]) && rn2(20)) || ( uncommon2(&mons[mntmp]) && !rn2(4) ) || ( uncommon3(&mons[mntmp]) && !rn2(3) ) || ( uncommon5(&mons[mntmp]) && !rn2(2) ) || ( uncommon7(&mons[mntmp]) && rn2(3) ) || ( uncommon10(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(20) ) ) );

		if (polymon(mntmp)) u.polyformed = 1;

		if (!uarmg || FingerlessGloves) selftouch("No longer petrify-resistant, you");

		u.wormpolymorph = 0;

		goto made_change;
	}

	if (tech_inuse(T_BECOME_UNDEAD)) {

		do {
			mntmp = rn2(NUMMONS);
		} while(( (notake(&mons[mntmp]) && rn2(4) ) || !polyok(&mons[mntmp]) || (!(is_undead(&mons[mntmp])) ) || ((mons[mntmp].mmove == 1) && rn2(4) ) || ((mons[mntmp].mmove == 2) && rn2(3) ) || ((mons[mntmp].mmove == 3) && rn2(2) ) || ((mons[mntmp].mmove == 4) && !rn2(3) ) || ( (mons[mntmp].mlevel < 10) && ((mons[mntmp].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[mntmp]) && rn2(2) ) || ( is_nonmoving(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(5) ) || ( is_nonmoving(&mons[mntmp]) && rn2(20) ) || (is_jonadabmonster(&mons[mntmp]) && rn2(20)) || ( uncommon2(&mons[mntmp]) && !rn2(4) ) || ( uncommon3(&mons[mntmp]) && !rn2(3) ) || ( uncommon5(&mons[mntmp]) && !rn2(2) ) || ( uncommon7(&mons[mntmp]) && rn2(3) ) || ( uncommon10(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(20) ) ) );

		if (polymon(mntmp)) u.polyformed = 1;

		if (!uarmg || FingerlessGloves) selftouch("No longer petrify-resistant, you");

		u.wormpolymorph = 0;

		goto made_change;
	}

	if (uamul && uamul->oartifact == ART_DIKKIN_S_DRAGON_TEETH && (yn("Do a completely random polymorph?") == 'y') ) {

		do {
			mntmp = rn2(NUMMONS);
		} while(( (notake(&mons[mntmp]) && rn2(4) ) || ((mons[mntmp].mlet == S_BAT) && rn2(2)) || ((mons[mntmp].mlet == S_EYE) && rn2(2) ) || ((mons[mntmp].mmove == 1) && rn2(4) ) || ((mons[mntmp].mmove == 2) && rn2(3) ) || ((mons[mntmp].mmove == 3) && rn2(2) ) || ((mons[mntmp].mmove == 4) && !rn2(3) ) || ( (mons[mntmp].mlevel < 10) && ((mons[mntmp].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[mntmp]) && rn2(2) ) || ( is_nonmoving(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(5) ) || ( is_nonmoving(&mons[mntmp]) && rn2(20) ) || (is_jonadabmonster(&mons[mntmp]) && rn2(20)) || ( uncommon2(&mons[mntmp]) && !rn2(4) ) || ( uncommon3(&mons[mntmp]) && !rn2(3) ) || ( uncommon5(&mons[mntmp]) && !rn2(2) ) || ( uncommon7(&mons[mntmp]) && rn2(3) ) || ( uncommon10(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(20) ) ) );

		if (polymon(mntmp)) u.polyformed = 1;

		u.polyformed = 1;

		if (!uarmg || FingerlessGloves) selftouch("No longer petrify-resistant, you");

		u.wormpolymorph = 0;

		goto made_change;
	}

	if (Race_if(PM_WARPER) && !u.wormpolymorph) {
		do {
			/* randomly pick any monster, but reroll if it sucks too much --Amy */
			mntmp = rn2(NUMMONS);
		} while(( (notake(&mons[mntmp]) && rn2(4) ) || ((mons[mntmp].mlet == S_BAT) && rn2(2)) || ((mons[mntmp].mlet == S_EYE) && rn2(2) ) || ((mons[mntmp].mmove == 1) && rn2(4) ) || ((mons[mntmp].mmove == 2) && rn2(3) ) || ((mons[mntmp].mmove == 3) && rn2(2) ) || ((mons[mntmp].mmove == 4) && !rn2(3) ) || ( (mons[mntmp].mlevel < 10) && ((mons[mntmp].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[mntmp]) && rn2(2) ) || ( is_nonmoving(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(5) ) || ( is_nonmoving(&mons[mntmp]) && rn2(20) ) || (is_jonadabmonster(&mons[mntmp]) && rn2(20)) || ( uncommon2(&mons[mntmp]) && !rn2(4) ) || ( uncommon3(&mons[mntmp]) && !rn2(3) ) || ( uncommon5(&mons[mntmp]) && !rn2(2) ) || ( uncommon7(&mons[mntmp]) && rn2(3) ) || ( uncommon10(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(20) ) ) );

	}

	else if (Race_if(PM_MISSINGNO)) {

		mntmp = PM_POLYMORPHED_MISSINGNO;

	} else if (Race_if(PM_DEATHMOLD)) {
		/* since we added new tilde-class monsters... have to hardwire this --Amy */
		int deathmolds = 14 + (u.ulevel / 2);
		mntmp = PM_WHITE_MISSINGNO; /* fail safe */
		switch (rnd(deathmolds)) {
			case 1:
				mntmp = PM_WHITE_MISSINGNO;
				break;
			case 2:
				mntmp = PM_GRAY_MISSINGNO;
				break;
			case 3:
				mntmp = PM_BLACK_MISSINGNO;
				break;
			case 4:
				mntmp = PM_RED_MISSINGNO;
				break;
			case 5:
				mntmp = PM_GREEN_MISSINGNO;
				break;
			case 6:
				mntmp = PM_BROWN_MISSINGNO;
				break;
			case 7:
				mntmp = PM_MAGENTA_MISSINGNO;
				break;
			case 8:
				mntmp = PM_CYAN_MISSINGNO;
				break;
			case 9:
				mntmp = PM_ORANGE_MISSINGNO;
				break;
			case 10:
				mntmp = PM_BRIGHT_GREEN_MISSINGNO;
				break;
			case 11:
				mntmp = PM_YELLOW_MISSINGNO;
				break;
			case 12:
				mntmp = PM_BRIGHT_BLUE_MISSINGNO;
				break;
			case 13:
				mntmp = PM_BRIGHT_MAGENTA_MISSINGNO;
				break;
			case 14:
				mntmp = PM_BRIGHT_CYAN_MISSINGNO;
				break;
			case 15:
				mntmp = PM_BEIGE_MISSINGNO;
				break;
			case 16:
				mntmp = PM_SHADY_MISSINGNO;
				break;
			case 17:
				mntmp = PM_DARK_MISSINGNO;
				break;
			case 18:
				mntmp = PM_SCARLET_MISSINGNO;
				break;
			case 19:
				mntmp = PM_VIRIDIAN_MISSINGNO;
				break;
			case 20:
				mntmp = PM_UMBRA_MISSINGNO;
				break;
			case 21:
				mntmp = PM_PURPLE_MISSINGNO;
				break;
			case 22:
				mntmp = PM_STEEL_MISSINGNO;
				break;
			case 23:
				mntmp = PM_VIVID_MISSINGNO;
				break;
			case 24:
				mntmp = PM_POISONOUS_MISSINGNO;
				break;
			case 25:
				mntmp = PM_TOPAZ_MISSINGNO;
				break;
			case 26:
				mntmp = PM_ULTRAMARINE_MISSINGNO;
				break;
			case 27:
				mntmp = PM_PINK_MISSINGNO;
				break;
			case 28:
				mntmp = PM_AZURE_MISSINGNO;
				break;
			case 29:
				mntmp = PM_MULTICOLORED_MISSINGNO;
				break;
			default:
				impossible("wrong form for deathmold?");
				mntmp = PM_WHITE_MISSINGNO; /* fall back to default one --Amy */
				break;
		}
	}
	else if ((Polymorph_control || forcecontrol) && !u.wormpolymorph && rn2(StrongPolymorph_control ? 5 : 3)) {

		boolean hasfailed = FALSE;
		boolean invalidpolymorph = FALSE;

		do {
			getlin("Become what kind of monster? [type the name]",
				buf);
			mntmp = name_to_mon(buf);

			if (mntmp >= LOW_PM && !polyok(&mons[mntmp]) && !your_race(&mons[mntmp])) {
				invalidpolymorph = TRUE;
				/* mother fucker, if I'm in wizard mode I damn well can poly into invalid stuff! --Amy */
				if (wizard && yn("Invalid polymorph form, do you want to force the polymorph to happen anyway?") == 'y') {
					u.wormpolymorph = mntmp;
					invalidpolymorph = FALSE;
				}
			}

			if (mntmp < LOW_PM)
				pline("I've never heard of such monsters.");
			/* Note:  humans are illegal as monsters, but an
			 * illegal monster forces newman(), which is what we
			 * want if they specified a human.... */
			else if (invalidpolymorph)
				You("cannot polymorph into that.");

			/* taking on high-level forms sometimes fails, especially if your level is low --Amy */
			else if (!forcecontrol && (rnd(50 - u.ulevel + mons[mntmp].mlevel) > 40 )) {

				mntmp = LOW_PM - 1; hasfailed = TRUE; break; /* polymorph failed */
			}
			/* uncommon forms are difficult to polymorph into, because the usual reason why they're uncommon is
			 * that they are very powerful, so we need to reduce the player's chance of becoming one --Amy */
			else if (!forcecontrol && ( (is_jonadabmonster(&mons[mntmp]) && rn2(20)) || ( uncommon2(&mons[mntmp]) && !rn2(4) ) || ( uncommon3(&mons[mntmp]) && !rn2(3) ) || ( uncommon5(&mons[mntmp]) && !rn2(2) ) || ( uncommon7(&mons[mntmp]) && rn2(3) ) || ( uncommon10(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(5)) ) ) {
				mntmp = LOW_PM - 1; hasfailed = TRUE; break; /* polymorph failed */
			}
			else if (!forcecontrol && ((PlayerCannotUseSkills) && (rnd(StrongPolymorph_control ? 12 : 18) > 7) ) ) {
				mntmp = LOW_PM - 1; hasfailed = TRUE; break; /* polymorph failed */
			}
			else if (!forcecontrol && (!(PlayerCannotUseSkills) && (rnd(StrongPolymorph_control ? 15 : 18) > (P_SKILL(P_POLYMORPHING) + 10) ) ) ) {
				mntmp = LOW_PM - 1; hasfailed = TRUE; break; /* polymorph failed */
			}

			else break;
		} while(++tries < 5);

		if (tries==5) pline("%s", thats_enough_tries);

		if (hasfailed && ((StrongPolymorph_control && rn2(10)) || (!StrongPolymorph_control && rn2(2)) ) ) {
			boolean strongchoice = StrongPolymorph_control;
			int whichchoice = 1;

			int bschoice1 = LOW_PM - 1;
			int bschoice2 = LOW_PM - 1;
			int bschoice3 = LOW_PM - 1;

			/* randomly pick an "ordinary" monster */

			tries = 0;
			do {
				bschoice1 = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
			} while((!polyok(&mons[bschoice1]) || is_placeholder(&mons[bschoice1]) || (notake(&mons[bschoice1]) && rn2(4) ) || ((mons[bschoice1].mlet == S_BAT) && rn2(2)) || ((mons[bschoice1].mlet == S_EYE) && rn2(2) ) || ((mons[bschoice1].mmove == 1) && rn2(4) ) || ((mons[bschoice1].mmove == 2) && rn2(3) ) || ((mons[bschoice1].mmove == 3) && rn2(2) ) || ((mons[bschoice1].mmove == 4) && !rn2(3) ) || ( (mons[bschoice1].mlevel < 10) && ((mons[bschoice1].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[bschoice1]) && rn2(2) ) || ( is_nonmoving(&mons[bschoice1]) && rn2(5) ) || (is_jonadabmonster(&mons[bschoice1]) && rn2(20)) || ( uncommon2(&mons[bschoice1]) && !rn2(4) ) || ( uncommon3(&mons[bschoice1]) && !rn2(3) ) || ( uncommon5(&mons[bschoice1]) && !rn2(2) ) || ( uncommon7(&mons[bschoice1]) && rn2(3) ) || ( uncommon10(&mons[bschoice1]) && rn2(5) ) || ( is_eel(&mons[bschoice1]) && rn2(5) ) ) );

			tries = 0;
			do {
				bschoice2 = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
			} while((!polyok(&mons[bschoice2]) || is_placeholder(&mons[bschoice2]) || (notake(&mons[bschoice2]) && rn2(4) ) || ((mons[bschoice2].mlet == S_BAT) && rn2(2)) || ((mons[bschoice2].mlet == S_EYE) && rn2(2) ) || ((mons[bschoice2].mmove == 1) && rn2(4) ) || ((mons[bschoice2].mmove == 2) && rn2(3) ) || ((mons[bschoice2].mmove == 3) && rn2(2) ) || ((mons[bschoice2].mmove == 4) && !rn2(3) ) || ( (mons[bschoice2].mlevel < 10) && ((mons[bschoice2].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[bschoice2]) && rn2(2) ) || ( is_nonmoving(&mons[bschoice2]) && rn2(5) ) || (is_jonadabmonster(&mons[bschoice2]) && rn2(20)) || ( uncommon2(&mons[bschoice2]) && !rn2(4) ) || ( uncommon3(&mons[bschoice2]) && !rn2(3) ) || ( uncommon5(&mons[bschoice2]) && !rn2(2) ) || ( uncommon7(&mons[bschoice2]) && rn2(3) ) || ( uncommon10(&mons[bschoice2]) && rn2(5) ) || ( is_eel(&mons[bschoice2]) && rn2(5) ) ) );

			tries = 0;
			do {
				bschoice3 = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
			} while((!polyok(&mons[bschoice3]) || is_placeholder(&mons[bschoice3]) || (notake(&mons[bschoice3]) && rn2(4) ) || ((mons[bschoice3].mlet == S_BAT) && rn2(2)) || ((mons[bschoice3].mlet == S_EYE) && rn2(2) ) || ((mons[bschoice3].mmove == 1) && rn2(4) ) || ((mons[bschoice3].mmove == 2) && rn2(3) ) || ((mons[bschoice3].mmove == 3) && rn2(2) ) || ((mons[bschoice3].mmove == 4) && !rn2(3) ) || ( (mons[bschoice3].mlevel < 10) && ((mons[bschoice3].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[bschoice3]) && rn2(2) ) || ( is_nonmoving(&mons[bschoice3]) && rn2(5) ) || (is_jonadabmonster(&mons[bschoice3]) && rn2(20)) || ( uncommon2(&mons[bschoice3]) && !rn2(4) ) || ( uncommon3(&mons[bschoice3]) && !rn2(3) ) || ( uncommon5(&mons[bschoice3]) && !rn2(2) ) || ( uncommon7(&mons[bschoice3]) && rn2(3) ) || ( uncommon10(&mons[bschoice3]) && rn2(5) ) || ( is_eel(&mons[bschoice3]) && rn2(5) ) ) );

			if (StrongPolymorph_control) pline("Your controlled polymorph failed, but your limited ability to control polymorphs allows you to pick from these polymorph forms: %s, %s or %s.", mons[bschoice1].mname, mons[bschoice2].mname, mons[bschoice3].mname);
			else pline("Your controlled polymorph failed, but your limited ability to control polymorphs allows you to pick from these polymorph forms: %s or %s.", mons[bschoice1].mname, mons[bschoice2].mname);

			pline("Pick a polymorph form. The prompt will show you the stats of the monsters you can choose, and loop until you actually make a choice.");

controlchooseagain:
			if (whichchoice < 2 || whichchoice > (StrongPolymorph_control ? 3 : 2)) {
				corpsepager(bschoice1);
				if (yn("Turn into this monster?") == 'y') {
					mntmp = bschoice1;
					goto controldone;
				}
				whichchoice++;
				if (whichchoice > (StrongPolymorph_control ? 3 : 2)) whichchoice = 1;
				goto controlchooseagain;
			}
			if (whichchoice == 2) {
				corpsepager(bschoice2);
				if (yn("Turn into this monster?") == 'y') {
					mntmp = bschoice2;
					goto controldone;
				}
				whichchoice++;
				if (whichchoice > (StrongPolymorph_control ? 3 : 2)) whichchoice = 1;
				goto controlchooseagain;
			}
			if (whichchoice == 3 && StrongPolymorph_control) {
				corpsepager(bschoice3);
				if (yn("Turn into this monster?") == 'y') {
					mntmp = bschoice3;
					goto controldone;
				}
				whichchoice++;
				if (whichchoice > (StrongPolymorph_control ? 3 : 2)) whichchoice = 1;
				goto controlchooseagain;
			}
			whichchoice = 0; /* fail safe */
			goto controlchooseagain;

		}
controldone:

		/* allow skin merging, even when polymorph is controlled */
		if (draconian &&
		    (mntmp == armor_to_dragon(uarm->otyp) || tries == 5))
		    goto do_merge;
	} else if ((Race_if(PM_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) || Race_if(PM_HEMI_DOPPELGANGER)) && rn2(StrongPolymorph_control ? 10 : 5) && canhavesemicontrol) {
		/* Not an experienced Doppelganger yet */
		do {
			/* Slightly different wording */
			getlin("Attempt to become what kind of monster? [type the name]",
				buf);
			mntmp = name_to_mon(buf);
			if (mntmp < LOW_PM)
				pline("I've never heard of such monsters.");
			/* Note:  humans are illegal as monsters, but an
			 * illegal monster forces newman(), which is what we
			 * want if they specified a human.... */
			else if (!polyok(&mons[mntmp]) && !your_race(&mons[mntmp]))
				You("cannot polymorph into that.");
			else if (!mvitals[mntmp].eaten) {
				You("attempt an unfamiliar polymorph.");
				if ((rn2(5) + u.ulevel) < mons[mntmp].mlevel)
				    mntmp = LOW_PM - 1; /* Didn't work for sure */

				/* lower chance of success even if the form isn't too high-level --Amy */
				else if (!forcecontrol && (rnd(50 - u.ulevel + mons[mntmp].mlevel) > 40 )) {
	
					mntmp = LOW_PM - 1; break; /* polymorph failed */

				/* lower chance if form is uncommon, see above --Amy */
				}
				else if ( (is_jonadabmonster(&mons[mntmp]) && rn2(20)) || ( uncommon2(&mons[mntmp]) && !rn2(4) ) || ( uncommon3(&mons[mntmp]) && !rn2(3) ) || ( uncommon5(&mons[mntmp]) && !rn2(2) ) || ( uncommon7(&mons[mntmp]) && rn2(3) ) || ( uncommon10(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(5)) ) {
					mntmp = LOW_PM - 1; break; /* polymorph failed */
				}
				else if ((PlayerCannotUseSkills) && (rnd(StrongPolymorph_control ? 6 : 12) > 3) ) {
					mntmp = LOW_PM - 1; break; /* polymorph failed */
				}
				else if (!(PlayerCannotUseSkills) && (rnd(StrongPolymorph_control ? 9 : 12) > (P_SKILL(P_POLYMORPHING) + 4) ) ) {
					mntmp = LOW_PM - 1; break; /* polymorph failed */
				}

				/* Either way, give it a shot */
				break;
			}

			/* even if you've eaten it, the polymorph can still fail --Amy */
			else if (!forcecontrol && (rnd(50 - u.ulevel + mons[mntmp].mlevel) > 40 )) {

				mntmp = LOW_PM - 1; break; /* polymorph failed */
			}

			else if ((PlayerCannotUseSkills) && (rnd(StrongPolymorph_control ? 6 : 12) > 3) ) {
				mntmp = LOW_PM - 1; break; /* polymorph failed */
			}
			else if (!(PlayerCannotUseSkills) && (rnd(StrongPolymorph_control ? 9 : 12) > (P_SKILL(P_POLYMORPHING) + 4) ) ) {
				mntmp = LOW_PM - 1; break; /* polymorph failed */
			}

			else break;
		} while(++tries < 5);
		if (tries==5) pline("%s", thats_enough_tries);
		/* allow skin merging, even when polymorph is controlled */
		if (draconian &&
		    (mntmp == armor_to_dragon(uarm->otyp) || tries == 5))
		    goto do_merge;
	} else if ( !u.wormpolymorph && ((draconian && rn2(5))  || iswere || (isvamp && issoviet) )) { /* chance to poly into something else --Amy */
		/* special changes that don't require polyok() */
		if (draconian /*&& rn2(5)*/ ) {
		    do_merge:
			mntmp = armor_to_dragon(uarm->otyp);

			if (!(mvitals[mntmp].mvflags & G_GENOD)) {
				/* Code that was here is now in merge_with_armor */
				merge_with_armor();
			}
		} else if (iswere) {
			if (is_were(youmonst.data))
				mntmp = PM_HUMAN; /* Illegal; force newman() */
			else
				mntmp = u.ulycn;
		} else if (isvamp && issoviet) {
			/* Amy edit: vampire bats are ultra sucky polymorph forms... */
			/* In Soviet Russia, vampires aren't allowed to polymorph into non-vampires. It's actually weird that
			 * the country isn't called Soviet Romania, where the vampires originally come from. And of course they
			 * don't take into account the fact that there's many many vampiric monsters in this game, not all of
			 * which deserve to always turn into such a godawfully weak form. */
			if (u.umonnum != PM_VAMPIRE_BAT) {
				mntmp = PM_VAMPIRE_BAT;
				pline("Teper' vy bespoleznaya letuchaya mysh', kotoraya vsegda oglushena i dvizhetsya sluchaynym obrazom. Vse tol'ko potomu, chto sovetskiy tip ledyanykh glyb nastol'ko otstalyy i nikogda ne mozhet izmenit' chto-to ot vanili.");
			} else
				mntmp = PM_HUMAN; /* newman() */
		}
		/* if polymon fails, "you feel" message has been given
		   so don't follow up with another polymon or newman */
		if (mntmp == PM_HUMAN && !Race_if(PM_UNGENOMOLD)) newman();	/* werecritter */
		else (void) polymon(mntmp);
		goto made_change;    /* maybe not, but this is right anyway */
	}
	if (u.wormpolymorph) {
		mntmp = u.wormpolymorph;
		u.polyformed = 1;
	}

	if (!u.wormpolymorph && !Race_if(PM_WARPER) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_MISSINGNO) && mntmp < LOW_PM) {
		tries = 0;
		do {
			/* randomly pick an "ordinary" monster */
			mntmp = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
		} while((!polyok(&mons[mntmp]) || is_placeholder(&mons[mntmp]) || (notake(&mons[mntmp]) && rn2(4) ) || ((mons[mntmp].mlet == S_BAT) && rn2(2)) || ((mons[mntmp].mlet == S_EYE) && rn2(2) ) || ((mons[mntmp].mmove == 1) && rn2(4) ) || ((mons[mntmp].mmove == 2) && rn2(3) ) || ((mons[mntmp].mmove == 3) && rn2(2) ) || ((mons[mntmp].mmove == 4) && !rn2(3) ) || ( (mons[mntmp].mlevel < 10) && ((mons[mntmp].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[mntmp]) && rn2(2) ) || ( is_nonmoving(&mons[mntmp]) && rn2(5) ) || (is_jonadabmonster(&mons[mntmp]) && rn2(20)) || ( uncommon2(&mons[mntmp]) && !rn2(4) ) || ( uncommon3(&mons[mntmp]) && !rn2(3) ) || ( uncommon5(&mons[mntmp]) && !rn2(2) ) || ( uncommon7(&mons[mntmp]) && rn2(3) ) || ( uncommon10(&mons[mntmp]) && rn2(5) ) || ( is_eel(&mons[mntmp]) && rn2(5) ) || ( is_nonmoving(&mons[mntmp]) && rn2(20) && (Race_if(PM_UNGENOMOLD) || Race_if(PM_MOULD) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_WARPER) ) ) || ( is_eel(&mons[mntmp]) && rn2(20) && (Race_if(PM_UNGENOMOLD) || Race_if(PM_MOULD) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_WARPER) ) ) )
	/* Polymorphing into a nonmoving monster can really ruin your day as an ungenomold character, so the chances
	 * of ending up as one are greatly reduced now. Eels are sucky polymorph forms too.
	 * And of course stuff that can't pick up items or is otherwise stunted should be more rare as well. --Amy */
				&& tries++ < 200);
	}

	/* The below polyok() fails either if everything is genocided, or if
	 * we deliberately chose something illegal to force newman().
	 */
        /* WAC Doppelgangers go through a 1/20 check rather than 1/5 */

	  /* Amy edit: it's bullshit if you get newman() so often. If you polymorph, you should fucking polymorph
	   * most of the time! I've greatly reduced the odds that newman() is called */

        if ( !u.wormpolymorph && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_MISSINGNO) && !Race_if(PM_WARPER) && !Race_if(PM_DEATHMOLD) && (!polyok(&mons[mntmp]) ||
			((Race_if(PM_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) || Race_if(PM_HEMI_DOPPELGANGER)) ? (
        			((u.ulevel < mons[mntmp].mlevel)
        			 || !mvitals[mntmp].eaten
        			 ) && !rn2(50)) : (Race_if(PM_MOULD) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_WARPER)) ? !rn2(50) :
				   !rn2(20)) || (your_race(&mons[mntmp]) && !Race_if(PM_MOULD) && !Race_if(PM_TRANSFORMER) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_WARPER) && rn2(5) ) ) ) /* polymorphitis races can always polymorph into everything, others can sometimes poly into their own race --Amy */
		newman();
	else if(!polymon(mntmp)) { u.wormpolymorph = 0; return; }

	if (!uarmg || FingerlessGloves) selftouch("No longer petrify-resistant, you");

	/* now that the polymorph has happened (or not), reset worm that walks variable */
	u.wormpolymorph = 0;

 made_change:
	new_light = Upolyd ? emits_light(youmonst.data) : 0;
	if (old_light != new_light) {
	    if (old_light)
		del_light_source(LS_MONSTER, (void *)&youmonst);
	    if (new_light == 1) ++new_light;  /* otherwise it's undetectable */
	    if (new_light)
		new_light_source(u.ux, u.uy, new_light,
				 LS_MONSTER, (void *)&youmonst);
	}
	if (is_drowningpool(u.ux,u.uy) && !(is_crystalwater(u.ux,u.uy)) && was_floating && !(Levitation || Flying) &&
		!breathless(youmonst.data) && !amphibious(youmonst.data) &&
		!Swimming) drown();
}

/* (try to) make a mntmp monster out of the player */
int
polymon(mntmp)	/* returns 1 if polymorph successful */
int	mntmp;
{
	int duratoincrease;

	boolean sticky = sticks(youmonst.data) && u.ustuck && !u.uswallow,
		was_blind = !!Blind, dochange = FALSE;
	boolean could_pass_walls = Passes_walls;
	int mlvl;

	if (Race_if(PM_PLAYER_SLIME)) { /* cannot polymorph at all - punishment for being slimed --Amy */
		newman();
		return 0;
	}
	if (Race_if(PM_INCORPOREALOID)) { /* cannot polymorph at all, because amateurhour wants it so --Amy */
		return 0;
	}

	if (mntmp < LOW_PM) { /* for some reason, this seems to happen sometimes. Dunno why. --Amy */

		pline("Uh-oh... that polymorph didn't seem to work.");
		return(0);

	}

	if (mntmp < NUMMONS) u.ughmemory = 0;

	if (mvitals[mntmp].mvflags & G_GENOD) {	/* allow G_EXTINCT */
		You_feel("rather %s-ish.",mons[mntmp].mname);
		exercise(A_WIS, TRUE);
		return(0);
	}

	/* STEPHEN WHITE'S NEW CODE */        
  
	/* If your an Undead Slayer, you can't become undead! */
  
	if (is_undead(&mons[mntmp]) && Role_if(PM_UNDEAD_SLAYER)) {
		if (Polymorph_control) { 
			You("hear a voice boom out: \"How dare you take such a form!\"");
			u.ualign.record -= 5;
#ifdef NOARTIFACTWISH
			u.usacrifice = 0;
#endif
			exercise(A_WIS, FALSE);
		 } else {
			You("start to change into %s, but a voice booms out:", an(mons[mntmp].mname));
			pline("\"No, I will not allow such a change!\"");
		 }
		 return(0);
	}

	/* KMH, conduct */
	u.uconduct.polyselfs++;
	if (uarm && uarm->oartifact == ART_RADAR_FELL_UP) increasesanity(rnz(5000));

	if (!Upolyd) {
		/* Human to monster; save human stats */
		/*u.macurr = u.acurr;
		u.mamax = u.amax;*/ /* edit by Amy - this was just stupid. Let the stats be interchangeable between forms! */
		u.mfemale = flags.female;
	} else {
		/* Monster to monster; restore human stats, to be
		 * immediately changed to provide stats for the new monster
		 */
		/*u.acurr = u.macurr;
		u.amax = u.mamax;*/ /* see above - I really don't understand why polymorph forms would have their own attribute scores. I want my freaking xorn wizard form to get a benefit from eating cockatrice corpses after all! Dammit! --Amy */
		flags.female = u.mfemale;
	}

	if (youmonst.m_ap_type) {
	    /* stop mimicking immediately */
	    if (multi < 0) unmul("");
	} else if (mons[mntmp].mlet != S_MIMIC) {
	    /* as in polyman() */
	    youmonst.m_ap_type = M_AP_NOTHING;
	}
	if (is_male(&mons[mntmp])) {
		if(flags.female) dochange = TRUE;
	} else if (is_female(&mons[mntmp])) {
		if(!flags.female) dochange = TRUE;
	} else if (!is_neuter(&mons[mntmp]) && mntmp != u.ulycn) {
		if(!rn2(10)) dochange = TRUE;
	}

	if (!u.ughmemory) {
	if (dochange) {
		flags.female = !flags.female;
		You("%s %s%s!",
		    (u.umonnum != mntmp) ? "turn into a" : "feel like a new",
		    (is_male(&mons[mntmp]) || is_female(&mons[mntmp])) ? "" :
			flags.female ? "female " : "male ",
		    mons[mntmp].mname);
	} else {
		if (u.umonnum != mntmp)
			You("turn into %s!", an(mons[mntmp].mname));
		else
			You_feel("like a new %s!", mons[mntmp].mname);
	}
	} else pline("You transform!");

	if (Stoned && poly_when_stoned(&mons[mntmp])) {
		/* poly_when_stoned already checked stone golem genocide */
		You("turn to stone!");
		mntmp = PM_STONE_GOLEM;
		Stoned = 0;
		delayed_killer = 0;
	}

	if (PlayerCannotUseSkills) u.mtimedone = rnz(400);
	else switch (P_SKILL(P_POLYMORPHING)) {

      	case P_BASIC:	u.mtimedone = rnz(500); break;
      	case P_SKILLED:	u.mtimedone = rnz(600); break;
      	case P_EXPERT:	u.mtimedone = rnz(700); break;
      	case P_MASTER:	u.mtimedone = rnz(800); break;
      	case P_GRAND_MASTER:	u.mtimedone = rnz(900); break;
      	case P_SUPREME_MASTER:	u.mtimedone = rnz(1000); break;
      	default: u.mtimedone = rnz(400); break;

	}

	if (!PlayerCannotUseSkills) {

		if (P_SKILL(P_POLYMORPHING) >= P_BASIC) {
			char nervbuf[QBUFSZ];
			char thisisannoying = 0;

			sprintf(nervbuf, "You have the polymorphing skill, which allows you to get a longer-lasting polymorph. Do you want a longer polymorph duration? (If you answer no, you just throw the bonus away.)");
			thisisannoying = yn_function(nervbuf, ynqchars, 'y');
			if (thisisannoying == 'n') {
				u.mtimedone = rnz(400);
				pline("You decided to opt for the regular polymorphing duration, disregarding the bonus that your polymorphing skill would have given you.");
			}
			else pline("Your polymorph duration was extended!");
		}

	}

	u.umonnum = mntmp;
	use_skill(P_POLYMORPHING, mons[mntmp].mlevel + 1 + rnd(5));

	set_uasmon();

	/* New stats for monster, to last only as long as polymorphed.
	 * Currently only strength gets changed.
	 */
	/*if(strongmonst(&mons[mntmp])) ABASE(A_STR) = AMAX(A_STR) = STR18(100);*/

	if (Stone_resistance && Stoned) { /* parnes@eniac.seas.upenn.edu */
		Stoned = 0;
		delayed_killer = 0;
		You("no longer seem to be petrifying.");
	}
	if (Sick_resistance && Sick) {
		make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		You("no longer feel sick.");
	}
	if (Slimed) {
	    if (flaming(youmonst.data)) {
		pline_The("slime burns away!");
		Slimed = 0L;
		flags.botl = 1;
	    } else if (slime_on_touch(&mons[mntmp]) ) {
		/* do it silently */
		Slimed = 0L;
		flags.botl = 1;
	    }
	}
	if (nohands(youmonst.data)) Glib = 0;
	if (Passes_walls && u.utraptype == TT_PIT) {  
		u.utraptype = 0;  
		u.utrap = 0;  
		vision_full_recalc = 1;  
	}  

	/*
	mlvl = adj_lev(&mons[mntmp]);
	 * We can't do the above, since there's no such thing as an
	 * "experience level of you as a monster" for a polymorphed character.
	 */
	mlvl = ((mntmp == u.ulycn) ? u.ulevel : (int)mons[mntmp].mlevel);
	if (youmonst.data->mlet == S_DRAGON && mntmp >= PM_GRAY_DRAGON) {
		u.mhmax = In_endgame(&u.uz) ? (8*mlvl) : (4*mlvl + d(mlvl,4));
	} else if (mntmp == PM_SLICK_RUEA || mntmp == PM_DOUBLE_AURORA_BOMBER || mntmp == PM_SHEER_SPACER || mntmp == PM_CRITICALLY_INJURED_THIEF || mntmp == PM_CRITICALLY_INJURED_JEDI) {
		u.mhmax = 1;
	} else if (is_golem(youmonst.data)) {
		u.mhmax = golemhp(mntmp);
	} else {
		if (!mlvl) u.mhmax = rnd(4);
		else u.mhmax = d(mlvl, 8);
		if (is_home_elemental(&mons[mntmp])) u.mhmax *= 3;
	}
	u.mhmax += rnz(u.ulevel);
	u.mhmax += rnz(u.ulevel);
	u.mhmax += rnz(ACURR(A_CON));
	if (u.ulevel >= 10) {
		u.mhmax += rnd(u.ulevel - 9 + mons[u.umonnum].mlevel );
		if (ACURR(A_CON) > 6) u.mhmax += ((u.ulevel + mons[u.umonnum].mlevel) * (ACURR(A_CON) / 7) );
	}
	if (u.menoraget) u.mhmax += rnd(50);
	if (u.silverbellget) u.mhmax += rnd(50);
	if (u.bookofthedeadget) u.mhmax += rnd(50);
	if (u.chaoskeyget) u.mhmax += rnd(10);
	if (u.neutralkeyget) u.mhmax += rnd(10);
	if (u.lawfulkeyget) u.mhmax += rnd(10);

	u.mh = u.mhmax;

	if (u.ulevel < mlvl && !Race_if(PM_MOULD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_MISSINGNO)) {
	/* Low level characters can't become high level monsters for long */
#ifdef DUMB
		/* DRS/NS 2.2.6 messes up -- Peter Kendell */
		int mtd = u.mtimedone, ulv = u.ulevel;

		u.mtimedone = mtd * ulv / mlvl;
#else
		u.mtimedone = u.mtimedone * u.ulevel / mlvl;
#endif
	}

	/* very fast polymorph forms last for a shorter amount of time (nerf :P) --Amy */
	if (youmonst.data->mmove > 17) {
		u.mtimedone *= 17;
		u.mtimedone /= youmonst.data->mmove;

		boolean morereduction = 1;
		if (!PlayerCannotUseSkills) switch (P_SKILL(P_POLYMORPHING)) {

	      	case P_BASIC: if (!rn2(10)) morereduction = 0; break;
	      	case P_SKILLED: if (!rn2(8)) morereduction = 0; break;
	      	case P_EXPERT: if (!rn2(5)) morereduction = 0; break;
	      	case P_MASTER: if (!rn2(3)) morereduction = 0; break;
	      	case P_GRAND_MASTER: if (!rn2(2)) morereduction = 0; break;
	      	case P_SUPREME_MASTER: morereduction = 0; break;
	      	default: morereduction = 1; break;
		
		}
		if (morereduction) {
			u.mtimedone *= 17;
			u.mtimedone /= youmonst.data->mmove;
		}

	}

	if (Race_if(PM_WARPER)) u.mtimedone /= 2;

	/* multiplicative bonuses come here, and shouldn't affect each other exponentially --Amy */

	duratoincrease = u.mtimedone;

	/* Moulds suck way too much. Let's allow them to stay polymorphed for a longer time. --Amy */
	/* Worms too. Their polymorph time depends on the monster's level though. */

	if ( (u.ulevel * 2) < mlvl && (Race_if(PM_MOULD) || Race_if(PM_DEATHMOLD) || Race_if(PM_MISSINGNO) || Race_if(PM_WORM_THAT_WALKS) ) ) {
		u.mtimedone += rnz((u.ulevel * 2) + 1);

		u.mtimedone += duratoincrease;
	}

	if (uarmc && uarmc->oartifact == ART_LONG_LASTING_JOY) u.mtimedone += (duratoincrease * rnd(2));;

	if (ishaxor) u.mtimedone += duratoincrease;

	/* WAC Doppelgangers can stay much longer in a form they know well */
	if ((Race_if(PM_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) || Race_if(PM_HEMI_DOPPELGANGER)) && mvitals[mntmp].eaten) {
		u.mtimedone += duratoincrease;
		u.mtimedone += mvitals[mntmp].eaten;
	}

	if (wizard && yn("Do you want to increase your polymorph duration by 10000 turns?") == 'y') u.mtimedone += 10000;

	if (uskin && mntmp != armor_to_dragon(uskin->otyp))
		skinback(FALSE);
	break_armor();
	drop_weapon(1);
	if (PlayerHidesUnderItems && OBJ_AT(u.ux, u.uy))
		u.uundetected = TRUE;
	else if ((youmonst.data->mlet == S_EEL) && is_waterypool(u.ux, u.uy))
		u.uundetected = TRUE;
	else
		u.uundetected = 0;

	if (u.utraptype == TT_PIT) {
	    if (could_pass_walls && !Passes_walls) {
		u.utrap = rn1(6,2);
	    } else if (!could_pass_walls && Passes_walls) {
		u.utrap = 0;
	    }
	}
	if (was_blind && !Blind) {	/* previous form was eyeless */
	    Blinded = 1L;
	    make_blinded(0L, TRUE);	/* remove blindness */
	}
	newsym(u.ux,u.uy);		/* Change symbol */

	if (!sticky && !u.uswallow && u.ustuck && sticks(youmonst.data)) setustuck(0);
	else if (sticky && !sticks(youmonst.data)) uunstick();
	if (u.usteed) {
	    if (touch_petrifies(u.usteed->data) && (PlayerCannotUseSkills || (P_SKILL(P_RIDING) < P_EXPERT)) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(uarmg && !FingerlessGloves && uarmu && uarm && uarmc) && rnl(3)) {
	    	char buf[BUFSZ];

	    	pline("No longer petrifying-resistant, you touch %s.",
	    			mon_nam(u.usteed));
	    	sprintf(buf, "riding a petrifying monster");
	    	instapetrify(buf);
 	    }
	    if (!can_ride(u.usteed)) dismount_steed(DISMOUNT_POLY);
	}

	if (flags.verbose) {
	    static const char use_thec[] = "Use the command #%s to %s.";
	    static const char monsterc[] = "monster";
	    if (can_breathe(youmonst.data))
		pline(use_thec,monsterc,"use your breath weapon");
	    if (attacktype(youmonst.data, AT_SPIT))
		pline(use_thec,monsterc,"spit venom");
	    if (youmonst.data->mlet == S_NYMPH)
		pline(use_thec,monsterc,"remove an iron ball");
	    if (attacktype(youmonst.data, AT_GAZE))
		pline(use_thec,monsterc,"gaze at monsters");
	    if (is_hider(youmonst.data))
		pline(use_thec,monsterc,"hide");
	    if (is_were(youmonst.data))
		pline(use_thec,monsterc,"summon help");
	    if (webmaker(youmonst.data))
		pline(use_thec,monsterc,"spin a web");
	    if (splittinggremlin(youmonst.data))
		pline(use_thec,monsterc,"multiply in a fountain");
	    if (splittinglavagremlin(youmonst.data))
		pline(use_thec,monsterc,"multiply in a pool of lava");
	    if (is_unicorn(youmonst.data))
		pline(use_thec,monsterc,"use your horn");
	    if (is_mind_flayer(youmonst.data))
		pline(use_thec,monsterc,"emit a mental blast");
	    if (youmonst.data->msound == MS_SHRIEK) /* worthless, actually */
		pline(use_thec,monsterc,"shriek");
	    if (youmonst.data->msound == MS_FART_QUIET)
		pline(use_thec,monsterc,"fart");
	    if (youmonst.data->msound == MS_FART_NORMAL)
		pline(use_thec,monsterc,"fart");
	    if (youmonst.data->msound == MS_FART_LOUD)
		pline(use_thec,monsterc,"fart");
	    if (lays_eggs(youmonst.data) && flags.female)
		pline(use_thec,"sit","lay an egg");
	    if (youmonst.data->msound == MS_STENCH)
		pline(use_thec,monsterc,"spread the perfume");
	    if (youmonst.data->msound == MS_CONVERT)
		pline(use_thec,monsterc,"preach");
	    if (youmonst.data->msound == MS_HCALIEN)
		pline(use_thec,monsterc,"chant");
	    if (youmonst.data->msound == MS_WHORE)
		pline(use_thec,monsterc,"talk sexily");
	    if (youmonst.data->msound == MS_SUPERMAN)
		pline(use_thec,monsterc,"make superman taunts");
	    if (youmonst.data->msound == MS_BONES)
		pline(use_thec,monsterc,"rattle");
	    if (dmgtype(youmonst.data, AD_WET))
		pline(use_thec,monsterc,"perform a rain dance");
	    if (attackdamagetype(youmonst.data, AT_BREA, AD_MALK) || attackdamagetype(youmonst.data, AT_BEAM, AD_MALK))
		pline(use_thec,monsterc,"conjure a thunderstorm");
	    if (dmgtype(youmonst.data, AD_ICEB) && dmgtype(youmonst.data, AD_FRZE))
		pline(use_thec,monsterc,"conjure a sleet");
	    if (attackdamagetype(youmonst.data, AT_BREA, AD_LITE))
		pline(use_thec,monsterc,"conjure sunny weather");
	    if (attackdamagetype(youmonst.data, AT_BREA, AD_DARK) || attackdamagetype(youmonst.data, AT_BEAM, AD_DARK)) {
		if (u.ulevel >= 18 || (Upolyd && (mons[u.umonnum].mlevel >= 18)) ) {
			pline(use_thec,monsterc,"cause an eclipse");
		}
	    }
	    if (youmonst.data->msound == MS_WEATHER)
		pline(use_thec,monsterc,"set the default weather");
	}
	/* you now know what an egg of your type looks like */
	if (lays_eggs(youmonst.data)) {
	    /* make queen bees recognize killer bee eggs */
	    learn_egg_type(egg_type_from_parent(u.umonnum, TRUE));
	}
	find_ac();
	if((!Levitation && !u.ustuck && !Flying &&
	    (is_waterypool(u.ux,u.uy) || is_watertunnel(u.ux,u.uy) || is_lava(u.ux,u.uy))) ||
	   (Underwater && !Swimming))
	    spoteffects(TRUE);
	if (Passes_walls && u.utrap && u.utraptype == TT_INFLOOR) {
	    u.utrap = 0;
	    FunnyHallu ? pline("You get on an astral trip.") : pline_The("rock seems to no longer trap you.");
	} else if (likes_lava(youmonst.data) && u.utrap && u.utraptype == TT_LAVA) {
	    u.utrap = 0;
	    FunnyHallu ? pline("Fire all around you - how comfy!") : pline_The("lava now feels soothing.");
	}
	if (amorphous(youmonst.data) || is_whirly(youmonst.data) || unsolid(youmonst.data)) {
	    if (Punished && !(u.repunishcounter) ) {
		You("slip out of the iron chain.");
		unpunish();
	    }
	}
	if (u.utrap && (u.utraptype == TT_WEB || u.utraptype == TT_BEARTRAP) &&
		(amorphous(youmonst.data) || is_whirly(youmonst.data) || unsolid(youmonst.data) ||
		  (youmonst.data->msize <= MZ_SMALL && u.utraptype == TT_BEARTRAP))) {
	    You("are no longer stuck in the %s.",
		    u.utraptype == TT_WEB ? "web" : "bear trap");
	    /* probably should burn webs too if PM_FIRE_ELEMENTAL */
	    u.utrap = 0;
	}
	if (webmaker(youmonst.data) && u.utrap && u.utraptype == TT_WEB) {
	    You("orient yourself on the web.");
	    u.utrap = 0;
	}
	flags.botl = 1;
	vision_full_recalc = 1;
	see_monsters();
	exercise(A_CON, FALSE);
	exercise(A_WIS, TRUE);
	(void) encumber_msg();
	return(1);
}

STATIC_OVL void
break_armor()
{

	char buf[BUFSZ];

    register struct obj *otmp;
    boolean controlled_change = (Race_if(PM_DOPPELGANGER) || (uarm && uarm->oartifact == ART_POLYFITTED) || RngeArmorPreservation || Role_if(PM_SHAPESHIFTER) || Race_if(PM_HEMI_DOPPELGANGER) || Role_if(PM_LUNATIC) || Race_if(PM_AK_THIEF_IS_DEAD_) || (Race_if(PM_HUMAN_WEREWOLF) && u.umonnum == PM_WEREWOLF));

	if (!(PlayerCannotUseSkills)) {
		if (rnd(10) < P_SKILL(P_POLYMORPHING)) controlled_change = TRUE;

	}

	boolean facelesskeep = 0;
	if (facelessprotection()) facelesskeep = TRUE; /* have to calculate this first, before any armor is removed --Amy */

	int controllingchance = 0;
	boolean armorkeep = 0;
	boolean cloakkeep = 0;
	boolean shirtkeep = 0;

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_POLYMORPHING)) {

	      	case P_BASIC:	controllingchance = 1; break;
	      	case P_SKILLED:	controllingchance = 2; break;
	      	case P_EXPERT:	controllingchance = 3; break;
      		case P_MASTER:	controllingchance = 4; break;
      		case P_GRAND_MASTER:	controllingchance = 5; break;
      		case P_SUPREME_MASTER:	controllingchance = 6; break;
      		default: controllingchance = 0; break;
		}

		if (hugemonst(youmonst.data)) controllingchance *= 5;
		else if (verysmall(youmonst.data) || (youmonst.data->msize >= MZ_HUGE)) {
			controllingchance *= 15;
			controllingchance /= 2;
		}
		else controllingchance *= 10;

	}
	if (facelesskeep && (controllingchance < 100)) controllingchance = 100;

	if (uarm && !Race_if(PM_TRANSFORMER) && (rnd(100) < controllingchance)) {

		getlin ("Keep your torso armor on? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) armorkeep = 1;

	}
	if (uarm && uarm->stckcurse) armorkeep = 1;

	if (uarmc && !Race_if(PM_TRANSFORMER) && (rnd(100) < controllingchance)) {
		getlin ("Keep your cloak on? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) cloakkeep = 1;

	}
	if (uarmc && uarmc->stckcurse) cloakkeep = 1;

	if (uarmu && !Race_if(PM_TRANSFORMER) && (rnd(100) < controllingchance)) {
		getlin ("Keep your shirt on? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) shirtkeep = 1;

	}
	if (uarmu && uarmu->stckcurse) shirtkeep = 1;

    if (breakarm(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
	if (((otmp = uarm) != 0) && !(uarm && uarm->otyp == OSFA_CHAIN_MAIL) && !(uarm && uarm->oartifact == ART_WRONG_TURN) && !armorkeep) {
	    if(otmp->oartifact || facelesskeep || (uarmf && uarmf->oartifact == ART_MALENA_S_LADYNESS) || autismringcheck(ART_FRAIDLOSE) || (otmp->fakeartifact && rn2(2)) ) {
		if (donning(otmp)) cancel_don();
		Your("armor falls off!");
		(void) Armor_gone();
		dropx(otmp); /*WAC Drop instead of destroy*/
	    } else if ((controlled_change) || (youmonst.data->msize == MZ_MEDIUM && !issoviet && rn2(20)) || (itemsurvivedestruction(otmp, 8) && !issoviet) || (Polymorph_control && !issoviet && !rn2(3)) || ((Fast || Very_fast) && !issoviet && !rn2(3)) || (StrongFast && !issoviet && !rn2(3)) || (otmp->blessed && !issoviet && !rn2(2)) || (StrongPolymorph_control && !issoviet && !rn2(3)) || (youmonst.data->msize == MZ_LARGE && !issoviet && rn2(5)) || (youmonst.data->msize == MZ_HUGE && !issoviet && rn2(3)) || (youmonst.data->msize > MZ_HUGE && !issoviet && rn2(2)) ) {
		if (donning(otmp)) cancel_don();

		if (!otmp->cursed) {
		You("quickly remove your armor as you start to change.");
		(void) Armor_gone();
		dropx(otmp); /*WAC Drop instead of destroy*/

		}

	    } else {
		if (donning(otmp)) cancel_don();
		if (!issoviet) You("break out of your armor!");
		else pline("I yeshche odin glupyy veshch' kusayet pyl'! Sovetskaya khochet etu igru, chtoby sosat' kak mozhno bol'she!");
		exercise(A_STR, FALSE);
		(void) Armor_gone();
		u.cnd_polybreak++;
		useup(otmp);
	}
	}
	if (!cloakkeep && !(uarmc && uarmc->oartifact == ART_THAT_PIXEL_SHAPE_) && !(uarmc && itemhasappearance(uarmc, APP_OSFA_CLOAK)) && (otmp = uarmc) != 0) {
	    if(otmp->oartifact || facelesskeep || (uarmf && uarmf->oartifact == ART_MALENA_S_LADYNESS) || autismringcheck(ART_FRAIDLOSE) || (otmp->fakeartifact && rn2(2)) ) {
		Your("%s falls off!", cloak_simple_name(otmp));
		(void) Cloak_off();
		dropx(otmp);
	    } else if ((controlled_change) || (youmonst.data->msize == MZ_MEDIUM && !issoviet && rn2(20)) || (youmonst.data->msize == MZ_LARGE && !issoviet && rn2(5)) || (itemsurvivedestruction(otmp, 8) && !issoviet) || (Polymorph_control && !issoviet && !rn2(3)) || ((Fast || Very_fast) && !issoviet && !rn2(3)) || (StrongFast && !issoviet && !rn2(3)) || (otmp->blessed && !issoviet && !rn2(2)) || (StrongPolymorph_control && !issoviet && !rn2(3)) || (youmonst.data->msize == MZ_HUGE && !issoviet && rn2(3)) || (youmonst.data->msize > MZ_HUGE && !issoviet && rn2(2)) ) {

		if (!otmp->cursed) {

		You("remove your %s before you transform.",
			cloak_simple_name(otmp));
		(void) Cloak_off();
		dropx(otmp);
		}

	    } else {
		if (!issoviet) Your("%s tears apart!", cloak_simple_name(otmp));
		else pline("I yeshche odin glupyy veshch' kusayet pyl'! Sovetskaya khochet etu igru, chtoby sosat' kak mozhno bol'she!");
		(void) Cloak_off();
		u.cnd_polybreak++;
		useup(otmp);
	    }
	}
	if (!shirtkeep && !(uarmu && uarmu->oartifact == ART_JEDE_SIZE_HIZE) && (otmp = uarmu) != 0) {

	    if(otmp->oartifact || facelesskeep || (uarmf && uarmf->oartifact == ART_MALENA_S_LADYNESS) || autismringcheck(ART_FRAIDLOSE) || (otmp->fakeartifact && rn2(2)) ) {
		Your("shirt falls off!");
		setworn((struct obj *)0, otmp->owornmask & W_ARMU);
		dropx(otmp);
	    } else if ((controlled_change) || (youmonst.data->msize == MZ_MEDIUM && !issoviet && rn2(20)) || (youmonst.data->msize == MZ_LARGE && !issoviet && rn2(5)) || (itemsurvivedestruction(otmp, 8) && !issoviet) || (Polymorph_control && !issoviet && !rn2(3)) || ((Fast || Very_fast) && !issoviet && !rn2(3)) || (StrongFast && !issoviet && !rn2(3)) || (otmp->blessed && !issoviet && !rn2(2)) || (StrongPolymorph_control && !issoviet && !rn2(3)) || (youmonst.data->msize == MZ_HUGE && !issoviet && rn2(3)) || (youmonst.data->msize > MZ_HUGE && !issoviet && rn2(2)) ) {

		if (!otmp->cursed) {

		You("take off your shirt just before it starts to rip.");
		setworn((struct obj *)0, otmp->owornmask & W_ARMU);
		dropx(otmp);
		}

	    } else {                
		if (!issoviet) Your("shirt rips to shreds!");
		else pline("I yeshche odin glupyy veshch' kusayet pyl'! Sovetskaya khochet etu igru, chtoby sosat' kak mozhno bol'she!");
		u.cnd_polybreak++;
		useup(uarmu);
	    }
	}
    } else if (sliparm(youmonst.data) && !(uarm && uarm->oartifact == ART_GRADIATING_WORK) && !(uarm && uarm->otyp == OSFA_CHAIN_MAIL) && !(uarm && uarm->oartifact == ART_WRONG_TURN) && !Race_if(PM_TRANSFORMER) ) {
	if (((otmp = uarm) != 0) && !armorkeep && (racial_exception(&youmonst, otmp) < 1)) {
		if (donning(otmp)) cancel_don();
		Your("armor falls around you!");
		(void) Armor_gone();
		dropx(otmp);
	}
	if (((otmp = uarmc) != 0) && !(uarmc && uarmc->oartifact == ART_THAT_PIXEL_SHAPE_) && !(uarmc && itemhasappearance(uarmc, APP_OSFA_CLOAK)) && !cloakkeep) {
		if (is_whirly(youmonst.data))
			Your("%s falls, unsupported!", cloak_simple_name(otmp));
		else You("shrink out of your %s!", cloak_simple_name(otmp));
		(void) Cloak_off();
		dropx(otmp);
	}
	if (((otmp = uarmu) != 0) && !(uarmu && uarmu->oartifact == ART_JEDE_SIZE_HIZE) && !shirtkeep) {
		if (is_whirly(youmonst.data))
			You("seep right through your shirt!");
		else You("become much too small for your shirt!");
		setworn((struct obj *)0, otmp->owornmask & W_ARMU);
		dropx(otmp);
	}
    }
    if (has_horns(youmonst.data) && !(uarmh && uarmh->oartifact == ART_FIT_THE_FAT_SCHWELLES) && !Race_if(PM_TRANSFORMER) ) {
	if (((otmp = uarmh) != 0) && !uarmh->stckcurse) {
	    if (is_flimsy(otmp) && !donning(otmp)) {
		char hornbuf[BUFSZ], yourbuf[BUFSZ];

		/* Future possiblities: This could damage/destroy helmet */
		sprintf(hornbuf, "horn%s", plur(num_horns(youmonst.data)));
		Your("%s %s through %s %s.", hornbuf, vtense(hornbuf, "pierce"),
		     shk_your(yourbuf, otmp), xname(otmp));
	    } else {
		if (donning(otmp)) cancel_don();
		Your("helmet falls to the %s!", surface(u.ux, u.uy));
		(void) Helmet_off();
		dropx(otmp);
	    }
	}
    }
    if ((otmp = uarmh) != 0 && !(uarmh && uarmh->oartifact == ART_FIT_THE_FAT_SCHWELLES) && !uarmh->stckcurse && !Race_if(PM_TRANSFORMER) && (is_mind_flayer(youmonst.data))) {
	    if (!otmp->cursed || facelesskeep) {
	      pline_The("%s is pushed from your head by your tentacles.", xname(otmp));
	      (void) Helmet_off();
	    } else if (otmp->oartifact || (otmp->fakeartifact && rn2(2)) || (controlled_change) || (itemsurvivedestruction(otmp, 8) && !issoviet) || (Polymorph_control && !issoviet && !rn2(3)) || ((Fast || Very_fast) && !issoviet && !rn2(3)) || (StrongFast && !issoviet && !rn2(3)) || (otmp->blessed && !issoviet && !rn2(2)) || (StrongPolymorph_control && !issoviet && !rn2(3)) ) {
	      pline_The("%s is pushed from your head by your tentacles.", xname(otmp));
	      (void) Helmet_off();
	    } else if (!(uarmf && uarmf->oartifact == ART_MALENA_S_LADYNESS) && !autismringcheck(ART_FRAIDLOSE) ) {
	      Your("tentacles break through %s.", the(xname(otmp)));
	      useup(uarmh);
		u.cnd_polybreak++;
	    }
    }
    if (!Race_if(PM_TRANSFORMER) && (nohands(youmonst.data) || verysmall(youmonst.data))) {

	if (uarmg && !Race_if(PM_TRANSFORMER) && (rnd(100) < controllingchance)) {

		getlin ("Keep your gloves on? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) goto glovesdone;

	}

	/* It was really retarded that you would also drop the weapon, because drop_weapon() is already called
	   someplace else and this was interfering with the "chance to keep stuff on" code. --Amy */
	if ((otmp = uarmg) != 0 && !(uarmg && uarmg->oartifact == ART_LUUUUUUUUUUUD) && !uarmg->stckcurse) {
	    if (donning(otmp)) cancel_don();
	    You("drop your gloves!");
	    (void) Gloves_off();
	    dropx(otmp);
	}
glovesdone:

	if (uarms && !Race_if(PM_TRANSFORMER) && (rnd(100) < controllingchance)) {

		getlin ("Keep your shield on? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) goto shielddone;

	}

	if ((otmp = uarms) != 0 && !(uarms && uarms->oartifact == ART_PERMOSTAND) && !(uarms && uarms->oartifact == ART_CASTLE_CRUSH_GLITCH) && !uarms->stckcurse) {
	    You("can no longer hold your shield!");
	    (void) Shield_off();
	    dropx(otmp);
	}
shielddone:

	if (uarmh && !Race_if(PM_TRANSFORMER) && (rnd(100) < controllingchance)) {

		getlin ("Keep your helmet on? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) goto helmetdone;

	}

	if ((otmp = uarmh) != 0 && !(uarmh && uarmh->oartifact == ART_FIT_THE_FAT_SCHWELLES) && !uarmh->stckcurse) {
	    if (donning(otmp)) cancel_don();
	    Your("helmet falls to the %s!", surface(u.ux, u.uy));
	    (void) Helmet_off();
	    dropx(otmp);
	}
    }
helmetdone:
    if (!Race_if(PM_TRANSFORMER) && (nohands(youmonst.data) || verysmall(youmonst.data) ||
		slithy(youmonst.data) || youmonst.data->mlet == S_CENTAUR)) {

	if (uarmf && !Race_if(PM_TRANSFORMER) && (rnd(100) < controllingchance)) {

		getlin ("Keep your boots on? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) goto bootsdone;

	}

	if ((otmp = uarmf) != 0 && !(uarmf && uarmf->oartifact == ART_MALENA_S_LADYNESS) && !uarmf->stckcurse) {
	    if (donning(otmp)) cancel_don();
	    if (is_whirly(youmonst.data))
		Your("boots fall away!");
	    else Your("boots %s off your feet!",
			verysmall(youmonst.data) ? "slide" : "are pushed");
	    (void) Boots_off();
	    dropx(otmp);
	}
bootsdone:
	;
    }
}

STATIC_OVL void
drop_weapon(alone)
int alone;
{
    struct obj *otmp;
    struct obj *otmp2;

	char buf[BUFSZ];

	int controllingchance = 0;

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_POLYMORPHING)) {

	      	case P_BASIC:	controllingchance = 1; break;
	      	case P_SKILLED:	controllingchance = 2; break;
	      	case P_EXPERT:	controllingchance = 3; break;
      		case P_MASTER:	controllingchance = 4; break;
      		case P_GRAND_MASTER:	controllingchance = 5; break;
      		case P_SUPREME_MASTER:	controllingchance = 6; break;
      		default: controllingchance = 0; break;
		}

		if (hugemonst(youmonst.data)) controllingchance *= 5;
		else if (verysmall(youmonst.data) || (youmonst.data->msize >= MZ_HUGE)) {
			controllingchance *= 15;
			controllingchance /= 2;
		}
		else controllingchance *= 10;

	}

    if ((otmp = uwep) != 0 && !uwep->stckcurse) {
	/* !alone check below is currently superfluous but in the
	 * future it might not be so if there are monsters which cannot
	 * wear gloves but can wield weapons
	 */
	if (!Race_if(PM_TRANSFORMER) && !autismweaponcheck(ART_FLAGELLATOR) && (!alone || cantwield(youmonst.data))) {
	    struct obj *wep = uwep;

	    if (alone) You("find you must drop your weapon%s!",
			   	u.twoweap ? "s" : "");
	    otmp2 = u.twoweap ? uswapwep : 0;

		if (uwep && !Race_if(PM_TRANSFORMER) && (rnd(100) < controllingchance)) {

			getlin ("Keep wielding your weapon? [y/yes/no]",buf);
			(void) lcase (buf);
			if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) goto weapondone;

		}
	    uwepgone();

	    if (canletgo(otmp, "poly-drop"))
		dropx(otmp);

weapondone:

	    if (otmp2 != 0) {
		uswapwepgone();
		if (canletgo(otmp2, "poly-drop"))
		    dropx(otmp2);
	    }
	    untwoweapon();
	} else if (!could_twoweap(youmonst.data)) {
	    untwoweapon();
	}
    } else if (cantwield(youmonst.data))
	untwoweapon();
}

void
rehumanize()
{
	boolean forced = (u.mh < 1);
	
	/* KMH, balance patch -- you can't revert back while unchanging */
	if (Unchanging && forced) {
		u.youaredead = 1;
		killer_format = NO_KILLER_PREFIX;
		killer = "killed while stuck in creature form";
		done(DIED);
		u.youaredead = 0;
	}

	if (Race_if(PM_MISSINGNO)) {
		reinitmissingno();
		init_uasmon();
	}

	if (emits_light(youmonst.data))
	    del_light_source(LS_MONSTER, (void *)&youmonst);
	polyman("return to %s form!", urace.adj);
	u.polyformed = 0;

	u.ughmemory = 0;

	if (u.uhp < 1) {
	    char kbuf[256];

		u.youaredead = 1;
	    sprintf(kbuf, "reverting to unhealthy %s form", urace.adj);
	    killer_format = KILLED_BY;
	    killer = kbuf;
	    done(DIED);
		u.youaredead = 0;
	}
	
	if (forced || /*(!Race_if(PM_DOPPELGANGER) && */(rn2(20) > ACURR(A_CON)))/*)*/ {
	/* Exhaustion for "forced" rehumaization & must pass con chack for 
    	 * non-doppelgangers 
    	 * Don't penalize doppelgangers/polymorph running out */
	/* Amy edit - why give so many boosts to the already overpowered doppelgangers??? */
    	 
   	/* WAC Polymorph Exhaustion 1/2 HP to prevent abuse */
	    u.uhp = (u.uhp/2) + 1;
	}

	/* SLASH'EM illogically made it so that whenever you unpolymorph, regardless of HOW, you get a massive penalty
	 * by having your maximum hit points drained fiercely. But this is SLEX, a game that's supposed to be balanced,
	 * so we only give that penalty if you're wimpy enough to allow your polymorphed HP to run out --Amy */

	/* In Soviet Russia, polymorphing is strictly forbidden. Everyone has to appear as what they actually are, because
	 * otherwise they could be foreign agents trying to undermine the Kreml's supremacy. So the government troops will
	 * do their best to punish anyone who's spotted polymorphing back to human form. */

	if (forced || issoviet) {
		int hitpointloss = rnd(10);
		if (hitpointloss > u.ulevel) hitpointloss = u.ulevel;

		if (u.uhpmax > hitpointloss) {
			u.uhpmax -= hitpointloss;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (issoviet) pline("Kha-kha-kha! Dumayesh', ty mog by byt' chlenom vooruzhennykh sil sosedney strany nezametno dlya nas?");
			else You("lost %d maximum hit points due to exhaustion.", hitpointloss);
		} else {
			u.youaredead = 1;
			You("had too little health left in your regular form and therefore you die!");
			killer_format = KILLED_BY;
			killer = "critical polymorph failure";
			done(DIED);
			u.youaredead = 0;
		}
	}

	if (!uarmg || FingerlessGloves) selftouch("No longer petrify-resistant, you");
	nomul(0, 0, FALSE);

	flags.botl = 1;
	vision_full_recalc = 1;
	(void) encumber_msg();
}


/* WAC -- MUHAHAHAAHA - Gaze attacks! 
 * Note - you can only gaze at one monster at a time, to keep this 
 * from getting out of hand ;B  Also costs 20 energy.
 * Amy edit: costs scale with level of your current form, since some gazes can be quite unbalanced...
 */
int
dogaze()
{
	coord cc;
	struct monst *mtmp;
	int gazecost = 24;
	int squeakamount = 0;
	gazecost += (Upolyd ? (mons[u.umonnum].mlevel * 4) : (u.ulevel * 4));

	if (!attacktype_fordmg(youmonst.data, AT_GAZE, AD_ANY)) {
		gazecost = 24;
		gazecost += (1 + mons[u.usymbiote.mnum].mlevel * 4);
	}

	squeakamount = gazecost;
	/* we can now reduce the cost based on the player's squeaking skill --Amy */
	if (!PlayerCannotUseSkills && gazecost > 2) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	gazecost *= 9; gazecost /= 10; break;
	      	case P_SKILLED:	gazecost *= 8; gazecost /= 10; break;
	      	case P_EXPERT:	gazecost *= 7; gazecost /= 10; break;
	      	case P_MASTER:	gazecost *= 6; gazecost /= 10; break;
	      	case P_GRAND_MASTER:	gazecost *= 5; gazecost /= 10; break;
	      	case P_SUPREME_MASTER:	gazecost *= 4; gazecost /= 10; break;
	      	default: break;
		}
	}

	if (Blind) {
		You("can't see a thing!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}
	if (u.uen < gazecost) {
		You("lack the energy to use your special gaze! Your current form's gaze attack costs %d mana!", gazecost);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}
	pline("Where do you wish to look?");
	cc.x = u.ux;
	cc.y = u.uy;
	getpos(&cc, TRUE, "the spot to look");

	if (cc.x == -10) return (0); /* user pressed esc */

	mtmp = m_at(cc.x, cc.y);

	if (!mtmp || !canseemon(mtmp)) {
		You("don't see a monster there!");
		return (0);
	}


	if ((flags.safe_dog && !Confusion && !Hallucination
		  && mtmp->mtame) || (flags.confirm && mtmp->mpeaceful 
		  && !Confusion && !Hallucination)) {
		  	char qbuf[QBUFSZ];
		  	
			sprintf(qbuf, "Really gaze at %s?", mon_nam(mtmp));
			if (yn(qbuf) != 'y') return (0);
	}

	/* WUOT why the hell did they not become hostile if you were e.g. confused??? */
	if (mtmp->mpeaceful) setmangry(mtmp);

	u.uen -= gazecost;

	You("gaze at %s...", mon_nam(mtmp));

	if (practicantterror) {
		pline("%s booms: 'Don't look at other people like that. Now you have to pay a fine of 200 zorkmids and vow to not do it again.'", noroelaname());
		fineforpracticant(200, 0, 0);
	}

	ranged_thorns(mtmp);

	while (squeakamount > 20) {
		use_skill(P_SQUEAKING, 1);
		squeakamount -= 20;
	}
	use_skill(P_SQUEAKING, 1);

	if ((mtmp->data==&mons[PM_MEDUSA]) && !mtmp->mcan && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) )) {
		pline("Gazing at the awake Medusa is not a very good idea.");
		/* as if gazing at a sleeping anything is fruitful... */
		/*You("turn to stone...");
		killer_format = KILLED_BY;
		killer = "deliberately gazing at Medusa's hideous countenance";
		done(STONING);*/
		You("start turning to stone...");
		if (!Stoned) {
			if (Hallucination && rn2(10)) pline("But you are already stoned.");
			else {
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				delayed_killer = "gazing at Medusa";
			}
		}
	} else if (!mtmp->mcansee || mtmp->msleeping) {
	    pline("But nothing happens.");
	    return (1);
	} else if (Invis && !perceives(mtmp->data)) {
	    pline("%s seems not to notice your gaze.", Monnam(mtmp));
	    return (1);
	} else {
	    register struct attack *mattk;
	    register int i;

	    for(i = 0; i < NATTK; i++) {
		mattk = &(youmonst.data->mattk[i]);
		if (mattk->aatyp == AT_GAZE) {
		    damageum(mtmp, mattk);
		    break;
		}

	    }

	    for(i = 0; i < NATTK; i++) {
		if (uactivesymbiosis && !PlayerCannotUseSkills && P_SKILL(P_SYMBIOSIS) >= P_SKILLED) {
			mattk = &(mons[u.usymbiote.mnum].mattk[i]);
			if (mattk->aatyp == AT_GAZE) {
			    damageum(mtmp, mattk);
			    break;
			}
		}
	    }

	}
	return(1);
}

int
dobreathe()
{
	struct attack *mattk;
	int energy = 0;
	int squeakamount = 0;

	if (Strangled) {
	    You_cant("breathe.  Sorry.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return(0);
	}

	/* WAC -- no more belching.  Use up energy instead */
	if (Race_if(PM_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) || Race_if(PM_HEMI_DOPPELGANGER)
		|| (Role_if(PM_FLAME_MAGE) && u.umonnum == PM_RED_DRAGON)
		|| (Role_if(PM_ACID_MAGE) && u.umonnum == PM_YELLOW_DRAGON)
		|| (Role_if(PM_ELECTRIC_MAGE) && u.umonnum == PM_BLUE_DRAGON)
		|| (Role_if(PM_POISON_MAGE) && u.umonnum == PM_GREEN_DRAGON)
		|| (Role_if(PM_ICE_MAGE) && u.umonnum == PM_WHITE_DRAGON))
	    energy = 15;
	else
	    energy = 20; /* a bit more expensive --Amy */

	/* and make instakill breath attacks even more expensive to tone down abuse potential */
	mattk = attacktype_fordmg(youmonst.data, AT_BREA, AD_ANY);
	if (!mattk && uactivesymbiosis && !PlayerCannotUseSkills && P_SKILL(P_SYMBIOSIS) >= P_EXPERT) mattk = attacktype_fordmg(&mons[u.usymbiote.mnum], AT_BREA, AD_ANY);
	if (!mattk) {
		impossible("bad breath attack?");   /* mouthwash needed... */
		return(0); /* prevent you from using segfault breath --Amy */
	}

	if ((mattk->adtyp != AD_MAGM) && (mattk->adtyp != AD_RBRE) && (mattk->adtyp != AD_FIRE) && (mattk->adtyp != AD_COLD) && (mattk->adtyp != AD_SLEE) && (mattk->adtyp != AD_DISN) && (mattk->adtyp != AD_ELEC) && (mattk->adtyp != AD_DRST) && (mattk->adtyp != AD_ACID) && (mattk->adtyp != AD_LITE) && (mattk->adtyp != AD_SPC2)  ) energy = 30;

	if (mattk->adtyp == AD_DISN) energy = u.breathenhancetimer ? 66 : 100;
	else if (mattk->adtyp == AD_RBRE) energy = u.breathenhancetimer ? 20 : 30; /* can randomly be a disintegration beam */

	if (Race_if(PM_PLAYER_CERBERUS) && !Upolyd) energy *= 3;

	squeakamount = energy;
	/* squeaking skill can reduce the required amount; reduce it after setting up the variable for skill training */
	if (!PlayerCannotUseSkills && energy > 2) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	energy *= 9; energy /= 10; break;
	      	case P_SKILLED:	energy *= 8; energy /= 10; break;
	      	case P_EXPERT:	energy *= 7; energy /= 10; break;
	      	case P_MASTER:	energy *= 6; energy /= 10; break;
	      	case P_GRAND_MASTER:	energy *= 5; energy /= 10; break;
	      	case P_SUPREME_MASTER:	energy *= 4; energy /= 10; break;
	      	default: break;
		}
	}

	if (u.uen < energy) {
	    You("don't have enough energy to breathe! You need at least %d mana!",energy);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return(0);
	}

	if (!getdir((char *)0)) return(0);

	u.uen -= energy;
	flags.botl = 1;

	mattk = attacktype_fordmg(youmonst.data, AT_BREA, AD_ANY);
	if (!mattk && uactivesymbiosis && !PlayerCannotUseSkills && P_SKILL(P_SYMBIOSIS) >= P_EXPERT) mattk = attacktype_fordmg(&mons[u.usymbiote.mnum], AT_BREA, AD_ANY);
	if (!mattk)
	    impossible("bad breath attack?");   /* mouthwash needed... */
	else {
	    /* Extra handling for AD_RBRE - player might poly into a crystal
	     * golem. */
	    uchar adtyp;
	    adtyp = mattk->adtyp == AD_RBRE ? rnd(AD_SPC2) : mattk->adtyp;

	if ((mattk->adtyp != AD_MAGM) && (mattk->adtyp != AD_RBRE) && (mattk->adtyp != AD_FIRE) && (mattk->adtyp != AD_COLD) && (mattk->adtyp != AD_SLEE) && (mattk->adtyp != AD_DISN) && (mattk->adtyp != AD_ELEC) && (mattk->adtyp != AD_DRST) && (mattk->adtyp != AD_ACID) && (mattk->adtyp != AD_LITE) && (mattk->adtyp != AD_SPC2)  ) adtyp = rnd(AD_SPC2);

		if (u.breathenhancetimer) {
			buzz((int) (20 + adtyp - 1), (rn2(2) ? ((int)mattk->damn * 3 / 2) : ((int)mattk->damd * 3 / 2) ), u.ux, u.uy, u.dx, u.dy);

		} else {
			buzz((int) (20 + adtyp - 1), (rn2(2) ? (int)mattk->damn : (int)mattk->damd ), u.ux, u.uy, u.dx, u.dy);

		}

		if (Race_if(PM_PLAYER_CERBERUS) && !Upolyd) { /* breath three times for 3x the cost --Amy */
			if (u.breathenhancetimer) {
				buzz((int) (20 + adtyp - 1), (rn2(2) ? ((int)mattk->damn * 3 / 2) : ((int)mattk->damd * 3 / 2) ), u.ux, u.uy, u.dx, u.dy);
			} else {
				buzz((int) (20 + adtyp - 1), (rn2(2) ? (int)mattk->damn : (int)mattk->damd ), u.ux, u.uy, u.dx, u.dy);
			}

			if (u.breathenhancetimer) {
				buzz((int) (20 + adtyp - 1), (rn2(2) ? ((int)mattk->damn * 3 / 2) : ((int)mattk->damd * 3 / 2) ), u.ux, u.uy, u.dx, u.dy);
			} else {
				buzz((int) (20 + adtyp - 1), (rn2(2) ? (int)mattk->damn : (int)mattk->damd ), u.ux, u.uy, u.dx, u.dy);
			}
		}

	}
	while (squeakamount > 20) {
		use_skill(P_SQUEAKING, 1);
		squeakamount -= 20;
	}
	use_skill(P_SQUEAKING, 1);
	return(1);
}

int
dospit()
{
	struct obj *otmp;
	struct attack *mattk;
	int spitcost = 10;
	int squeakamount = 0;

	mattk = attacktype_fordmg(youmonst.data, AT_SPIT, AD_ANY);
	if (!mattk && uactivesymbiosis && !PlayerCannotUseSkills && P_SKILL(P_SYMBIOSIS) >= P_BASIC) mattk = attacktype_fordmg(&mons[u.usymbiote.mnum], AT_SPIT, AD_ANY);

	if (mattk && mattk->adtyp == AD_TCKL) spitcost = 30;
	if (mattk && mattk->adtyp == AD_DRLI) spitcost = 50;

	squeakamount = spitcost;

	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	spitcost *= 9; spitcost /= 10; break;
	      	case P_SKILLED:	spitcost *= 8; spitcost /= 10; break;
	      	case P_EXPERT:	spitcost *= 7; spitcost /= 10; break;
	      	case P_MASTER:	spitcost *= 6; spitcost /= 10; break;
	      	case P_GRAND_MASTER:	spitcost *= 5; spitcost /= 10; break;
	      	case P_SUPREME_MASTER:	spitcost *= 4; spitcost /= 10; break;
	      	default: break;
		}
	}
	if (spitcost < 3) spitcost = 3; /* fail safe */

	if (u.uen < spitcost) {
		You("lack the energy to spit - need at least %d mana!", spitcost);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}

	if (!getdir((char *)0)) return(0);

	u.uen -= spitcost;

	if (!mattk)
	    impossible("bad spit attack?");
	else {
	    switch (mattk->adtyp) {
		case AD_BLND:
		case AD_DRST:
		    otmp = mksobj(BLINDING_VENOM, TRUE, FALSE, FALSE);
		    break;
		case AD_DRLI:
		    otmp = mksobj(FAERIE_FLOSS_RHING, TRUE, FALSE, FALSE);
		    break;
		case AD_ICEB:
		    otmp = mksobj(ICE_BLOCK, TRUE, FALSE, FALSE);
		    break;
		case AD_TCKL:
		    otmp = mksobj(TAIL_SPIKES, TRUE, FALSE, FALSE);
		    break;
		case AD_NAST:
		    otmp = mksobj(SEGFAULT_VENOM, TRUE, FALSE, FALSE);
		    break;
		default:
		    pline("bad attack type in do_spit");
		    /* fall through */
		case AD_ACID:
		    otmp = mksobj(ACID_VENOM, TRUE, FALSE, FALSE);
		    break;
	    }
	    if (!otmp) return(0);
	    otmp->quan = 1;
	    otmp->owt = weight(otmp);
	    /*otmp->spe = 1;*/ /* to indicate it's yours */
	    throwit(otmp, 0L, FALSE, 0);
	}
	while (squeakamount > 20) {
		use_skill(P_SQUEAKING, 1);
		squeakamount -= 20;
	}
	use_skill(P_SQUEAKING, 1);
	return(1);
}

int
doremove()
{
	if (!Punished) {
		You("are not chained to anything!");
		return(0);
	}
	unpunish();
	return(1);
}

int
dospinweb()
{
	register struct trap *ttmp = t_at(u.ux,u.uy);

	if (Levitation || Is_airlevel(&u.uz)
	    || Underwater || Is_waterlevel(&u.uz)) {
		You("must be on the ground to spin a web.");
		return(0);
	}
	if (u.uswallow) {
		You("release web fluid inside %s.", mon_nam(u.ustuck));
		if (is_animal(u.ustuck->data)) {
			expels(u.ustuck, u.ustuck->data, TRUE);
			return(0);
		}
		if (is_whirly(u.ustuck->data)) {
			int i;

			for (i = 0; i < NATTK; i++)
				if (u.ustuck->data->mattk[i].aatyp == AT_ENGL)
					break;
			if (i == NATTK)
			       impossible("Swallower has no engulfing attack?");
			else {
				char sweep[30];

				sweep[0] = '\0';
				switch(u.ustuck->data->mattk[i].adtyp) {
					case AD_FIRE:
						strcpy(sweep, "ignites and ");
						break;
					case AD_ELEC:
						strcpy(sweep, "fries and ");
						break;
					case AD_COLD:
						strcpy(sweep,
						      "freezes, shatters and ");
						break;
				}
				pline_The("web %sis swept away!", sweep);
			}
			return(0);
		}		     /* default: a nasty jelly-like creature */
		pline_The("web dissolves into %s.", mon_nam(u.ustuck));
		return(0);
	}
	if (u.utrap) {
		You("cannot spin webs while stuck in a trap.");
		return(0);
	}
	exercise(A_DEX, TRUE);
	if (ttmp) switch (ttmp->ttyp) {
		case S_PRESSING_TRAP:
			You("enclose the s-pressing trap in a web.");
			deltrap(ttmp);
			newsym(u.ux, u.uy);
			return(1);
		case PIT:
		case SHIT_PIT:
		case MANA_PIT:
		case ANOXIC_PIT:
		case HYPOXIC_PIT:
		case ACID_PIT:
		case SPIKED_PIT: You("spin a web, covering up the pit.");
			deltrap(ttmp);
			bury_objs(u.ux, u.uy);
			newsym(u.ux, u.uy);
			return(1);
		case SQKY_BOARD: pline_The("squeaky board is muffled.");
			deltrap(ttmp);
			newsym(u.ux, u.uy);
			return(1);
		case TELEP_TRAP:
		case BEAMER_TRAP:
		case LEVEL_TELEP:
		case PHASEPORTER:
		case PHASE_BEAMER:
		case LEVEL_BEAMER:
		case NEXUS_TRAP:
		case MAGIC_PORTAL:
			Your("webbing vanishes!");
			return(0);
		case WEB: You("make the web thicker.");
			return(1);
		case HOLE:
		case TRAPDOOR:
		case SHAFT_TRAP:
		case CURRENT_SHAFT:
			You("web over the %s.",
			    (ttmp->ttyp == TRAPDOOR) ? "trap door" : "hole");
			deltrap(ttmp);
			newsym(u.ux, u.uy);
			return 1;
		case ROLLING_BOULDER_TRAP:
			You("spin a web, jamming the trigger.");
			deltrap(ttmp);
			newsym(u.ux, u.uy);
			return(1);
		case ARROW_TRAP:
		case DART_TRAP:
		case BEAR_TRAP:
		case ROCKTRAP:
		case FIRE_TRAP:
		case ICE_TRAP:
		case SPEAR_TRAP:
		case MAGIC_BEAM_TRAP:
		case COLLAPSE_TRAP:
		case SHIT_TRAP:
		case ANIMATION_TRAP:
		case GLYPH_OF_WARDING:
		case LANDMINE:
		case SLP_GAS_TRAP:
		case RUST_TRAP:
		case MAGIC_TRAP:
		case ANTI_MAGIC:
		case POLY_TRAP:
		case SCYTHING_BLADE:
		case BOLT_TRAP:
		case ACID_POOL:
		case WATER_POOL:
		case POISON_GAS_TRAP:
		case SLOW_GAS_TRAP:
		case SHOCK_TRAP:
		case HEEL_TRAP:
		case VULN_TRAP:
		case LAVA_TRAP:
		case FLOOD_TRAP:
		case DISINTEGRATION_TRAP:
		case DEATH_TRAP:
		case PESTILENCE_TRAP:
		case FAMINE_TRAP:
		case DRAIN_TRAP:
		case ITEM_TELEP_TRAP:
		case GRAVITY_TRAP:
		case STONE_TO_FLESH_TRAP:
		case QUICKSAND_TRAP:
		case RELOCATION_TRAP:

		case NEST_TRAP:
		case BULLET_TRAP:
		case PARALYSIS_TRAP:
		case CYANIDE_TRAP:
		case LASER_TRAP:
		case GIANT_CHASM:
		case FART_TRAP:
		case CONFUSE_TRAP:
		case STUN_TRAP:
		case HALLUCINATION_TRAP:
		case PETRIFICATION_TRAP:
		case NUMBNESS_TRAP:
		case FREEZING_TRAP:
		case BURNING_TRAP:
		case FEAR_TRAP:
		case BLINDNESS_TRAP:
		case GLIB_TRAP:
		case SLIME_TRAP:
		case INERTIA_TRAP:
		case TIME_TRAP:
		case LYCANTHROPY_TRAP:
		case UNLIGHT_TRAP:
		case ELEMENTAL_TRAP:
		case ESCALATING_TRAP:
		case NEGATIVE_TRAP:
		case MANA_TRAP:
		case SIN_TRAP:
		case DESTROY_ARMOR_TRAP:
		case DIVINE_ANGER_TRAP:
		case GENETIC_TRAP:
		case MISSINGNO_TRAP:
		case CANCELLATION_TRAP:
		case HOSTILITY_TRAP:
		case BOSS_TRAP:
		case WISHING_TRAP:
		case FALLING_BOULDER_TRAP:
		case GLASS_ARROW_TRAP:
		case GLASS_BOLT_TRAP:
		case OUT_OF_MAGIC_TRAP:
		case PLASMA_TRAP:
		case BOMB_TRAP:
		case EARTHQUAKE_TRAP:
		case NOISE_TRAP:
		case GLUE_TRAP:
		case GUILLOTINE_TRAP:
		case BISECTION_TRAP:
		case VOLT_TRAP:
		case HORDE_TRAP:
		case IMMOBILITY_TRAP:
		case GREEN_GLYPH:
		case BLUE_GLYPH:
		case YELLOW_GLYPH:
		case ORANGE_GLYPH:
		case BLACK_GLYPH:
		case PURPLE_GLYPH:
		case FALLING_LOADSTONE_TRAP:
		case FALLING_NASTYSTONE_TRAP:
		case SUMMON_UNDEAD_TRAP:

		case SPIKE_TRAP:
		case PAPER_ARROW_TRAP:
		case KNIFE_TRAP:
		case SUMMON_MONSTER_TRAP:
		case MAGIC_DAMAGE_TRAP:
		case RED_DAMAGE_TRAP:
		case GREEN_DAMAGE_TRAP:
		case YELLOW_DAMAGE_TRAP:
		case WHITE_DAMAGE_TRAP:
		case BLUE_DAMAGE_TRAP:
		case MULTI_DEBUFF_TRAP:

		case SPINED_BALL_TRAP:
		case PENDULUM_TRAP:
		case TURN_TABLE:
		case SCENT_TRAP:
		case BANANA_TRAP:
		case FALLING_TUB_TRAP:
		case ALARM:
		case CALTROPS_TRAP:
		case BLADE_WIRE:
		case MAGNET_TRAP:
		case SLINGSHOT_TRAP:
		case CANNON_TRAP:
		case VENOM_SPRINKLER:
		case FUMAROLE:

		case LEG_TRAP:
		case ARTIFACT_JACKPOT_TRAP:
		case MAP_AMNESIA_TRAP:
		case SPREADING_TRAP:
		case ADJACENT_TRAP:
		case SUPERTHING_TRAP:
		case LEVITATION_TRAP:
		case BOWEL_CRAMPS_TRAP:
		case UNEQUIPPING_TRAP:
		case GOOD_ARTIFACT_TRAP:
		case GENDER_TRAP:
		case TRAP_OF_OPPOSITE_ALIGNMENT:
		case SINCOUNT_TRAP:
		case PET_TRAP:
		case PIERCING_BEAM_TRAP:
		case WRENCHING_TRAP:
		case TRACKER_TRAP:
		case NURSE_TRAP:
		case BACK_TO_START_TRAP:
		case NEMESIS_TRAP:
		case STREW_TRAP:
		case OUTTA_DEPTH_TRAP:
		case PUNISHMENT_TRAP:
		case BOON_TRAP:
		case FOUNTAIN_TRAP:
		case THRONE_TRAP:
		case ARABELLA_SPEAKER:
		case FEMMY_TRAP:
		case MADELEINE_TRAP:
		case MARLENA_TRAP:
		case JUDITH_TRAP:
		case SABINE_TRAP:
		case JANET_TRAP:
		case SABRINA_TRAP:
		case TANJA_TRAP:
		case SONJA_TRAP:
		case RHEA_TRAP:
		case LARA_TRAP:
		case NADINE_TRAP:
		case LUISA_TRAP:
		case IRINA_TRAP:
		case LISELOTTE_TRAP:
		case GRETA_TRAP:
		case JANE_TRAP:
		case SUE_LYN_TRAP:
		case CHARLOTTE_TRAP:
		case HANNAH_TRAP:
		case LITTLE_MARIE_TRAP:
		case RUTH_TRAP:
		case MAGDALENA_TRAP:
		case MARLEEN_TRAP:
		case KLARA_TRAP:
		case FRIEDERIKE_TRAP:
		case NAOMI_TRAP:
		case UTE_TRAP:
		case JASIEEN_TRAP:
		case YASAMAN_TRAP:
		case MAY_BRITT_TRAP:
		case KSENIA_TRAP:
		case LYDIA_TRAP:
		case CONNY_TRAP:
		case KATIA_TRAP:
		case MARIYA_TRAP:
		case ELISE_TRAP:
		case RONJA_TRAP:
		case ARIANE_TRAP:
		case JOHANNA_TRAP:
		case INGE_TRAP:
		case ROSA_TRAP:
		case JANINA_TRAP:
		case KRISTIN_TRAP:
		case ANNA_TRAP:
		case RUEA_TRAP:
		case DORA_TRAP:
		case MARIKE_TRAP:
		case JETTE_TRAP:
		case INA_TRAP:
		case SING_TRAP:
		case VICTORIA_TRAP:
		case MELISSA_TRAP:
		case ANITA_TRAP:
		case HENRIETTA_TRAP:
		case VERENA_TRAP:
		case ARABELLA_TRAP:
		case NELLY_TRAP:
		case EVELINE_TRAP:
		case KARIN_TRAP:
		case JUEN_TRAP:
		case KRISTINA_TRAP:
		case ALMUT_TRAP:
		case JULIETTA_TRAP:
		case LOU_TRAP:
		case ANASTASIA_TRAP:
		case FILLER_TRAP:
		case TOXIC_VENOM_TRAP:
		case INSANITY_TRAP:
		case MADNESS_TRAP:
		case JESSICA_TRAP:
		case SOLVEJG_TRAP:
		case WENDY_TRAP:
		case KATHARINA_TRAP:
		case ELENA_TRAP:
		case THAI_TRAP:
		case ELIF_TRAP:
		case NADJA_TRAP:
		case SANDRA_TRAP:
		case NATALJE_TRAP:
		case JEANETTA_TRAP:
		case YVONNE_TRAP:
		case MAURAH_TRAP:
		case ANNEMARIE_TRAP:
		case JIL_TRAP:
		case JANA_TRAP:
		case KATRIN_TRAP:
		case GUDRUN_TRAP:
		case ELLA_TRAP:
		case MANUELA_TRAP:
		case JENNIFER_TRAP:
		case PATRICIA_TRAP:
		case ANTJE_TRAP:
		case ANTJE_TRAP_X:
		case KERSTIN_TRAP:
		case LAURA_TRAP:
		case LARISSA_TRAP:
		case NORA_TRAP:
		case NATALIA_TRAP:
		case SUSANNE_TRAP:
		case LISA_TRAP:
		case BRIDGHITTE_TRAP:
		case JULIA_TRAP:
		case NICOLE_TRAP:
		case RITA_TRAP:

		case ELEMENTAL_PORTAL:
		case GIRLINESS_TRAP:
		case FUMBLING_TRAP:
		case EGOMONSTER_TRAP:
		case FLOODING_TRAP:
		case MONSTER_CUBE:
		case CURSED_GRAVE:
		case DIMNESS_TRAP:
		case EVIL_ARTIFACT_TRAP:
		case RODNEY_TRAP:
		case CONTAMINATION_TRAP:

		case EVIL_HEEL_TRAP:
		case BAD_EQUIPMENT_TRAP:
		case TEMPOCONFLICT_TRAP:
		case TEMPOHUNGER_TRAP:
		case TELEPORTITIS_TRAP:
		case POLYMORPHITIS_TRAP:
		case PREMATURE_DEATH_TRAP:
		case LASTING_AMNESIA_TRAP:
		case RAGNAROK_TRAP:
		case SINGLE_DISENCHANT_TRAP:

		case SEVERE_DISENCHANT_TRAP:
		case PAIN_TRAP:
		case TECHCAP_TRAP:
		case TREMBLING_TRAP:
		case SPELL_MEMORY_TRAP:
		case SKILL_REDUCTION_TRAP:
		case SKILLCAP_TRAP:
		case PERMANENT_STAT_DAMAGE_TRAP:

		case HYBRID_TRAP:
		case SHAPECHANGE_TRAP:
		case MELTEM_TRAP:
		case MIGUC_TRAP:
		case DIRECTIVE_TRAP:
		case SATATUE_TRAP:
		case FARTING_WEB:
		case CATACLYSM_TRAP:
		case DATA_DELETE_TRAP:
		case ELDER_TENTACLING_TRAP:
		case FOOTERER_TRAP:

		case GRAVE_WALL_TRAP:
		case TUNNEL_TRAP:
		case FARMLAND_TRAP:
		case MOUNTAIN_TRAP:
		case WATER_TUNNEL_TRAP:
		case CRYSTAL_FLOOD_TRAP:
		case MOORLAND_TRAP:
		case URINE_TRAP:
		case SHIFTING_SAND_TRAP:
		case STYX_TRAP:
		case PENTAGRAM_TRAP:
		case SNOW_TRAP:
		case ASH_TRAP:
		case SAND_TRAP:
		case PAVEMENT_TRAP:
		case HIGHWAY_TRAP:
		case GRASSLAND_TRAP:
		case NETHER_MIST_TRAP:
		case STALACTITE_TRAP:
		case CRYPTFLOOR_TRAP:
		case BUBBLE_TRAP:
		case RAIN_CLOUD_TRAP:

		case ITEM_NASTIFICATION_TRAP:
		case SANITY_INCREASE_TRAP:
		case PSI_TRAP:
		case GAY_TRAP:

		case SARAH_TRAP:
		case CLAUDIA_TRAP:
		case LUDGERA_TRAP:
		case KATI_TRAP:

		case SKILL_MULTIPLY_TRAP:
		case TRAPWALK_TRAP:
		case CLUSTER_TRAP:
		case FIELD_TRAP:
		case MONICIDE_TRAP:
		case TRAP_CREATION_TRAP:
		case LEOLD_TRAP:
		case BURDEN_TRAP:
		case MAGIC_VACUUM_TRAP:
		case ANIMEBAND_TRAP:
		case PERFUME_TRAP:
		case COURT_TRAP:
		case POKEMON_TRAP:
		case ELDER_SCROLLS_TRAP:
		case JOKE_TRAP:
		case DUNGEON_LORDS_TRAP:
		case FORTYTWO_TRAP:
		case RANDOMIZE_TRAP:
		case EVILROOM_TRAP:
		case AOE_TRAP:
		case ELONA_TRAP:
		case RELIGION_TRAP:
		case STEAMBAND_TRAP:
		case HARDCORE_TRAP:
		case MACHINE_TRAP:
		case BEE_TRAP:
		case MIGO_TRAP:
		case ANGBAND_TRAP:
		case DNETHACK_TRAP:
		case EVIL_SPAWN_TRAP:
		case SHOE_TRAP:
		case INSIDE_TRAP:
		case DOOM_TRAP:
		case MILITARY_TRAP:
		case ILLUSION_TRAP:
		case DIABLO_TRAP:

		case WALL_TRAP:
		case MONSTER_GENERATOR:
		case POTION_DISPENSER:
		case SPACEWARS_SPAWN_TRAP:
		case TV_TROPES_TRAP:
		case SYMBIOTE_TRAP:
		case KILL_SYMBIOTE_TRAP:
		case SYMBIOTE_REPLACEMENT_TRAP:
		case SHUTDOWN_TRAP:
		case CORONA_TRAP:
		case UNPROOFING_TRAP:
		case VISIBILITY_TRAP:
		case FEMINISM_STONE_TRAP:
		case BRANCH_TELEPORTER:
		case BRANCH_BEAMER:
		case POISON_ARROW_TRAP:
		case POISON_BOLT_TRAP:
		case MACE_TRAP:
		case SHUEFT_TRAP:
		case MOTH_LARVAE_TRAP:
		case WORTHINESS_TRAP:
		case CONDUCT_TRAP:
		case STRIKETHROUGH_TRAP:
		case MULTIPLE_GATHER_TRAP:
		case VIVISECTION_TRAP:
		case INSTAFEMINISM_TRAP:

		case CALLING_OUT_TRAP:
		case FIELD_BREAK_TRAP:
		case TENTH_TRAP:
		case DEBT_TRAP:
		case INVERSION_TRAP:
		case WINCE_TRAP:
		case FUCK_OVER_TRAP:
		case U_HAVE_BEEN_TRAP:
		case PERSISTENT_FART_TRAP:
		case ATTACKING_HEEL_TRAP:
		case TRAP_TELEPORTER:
		case ALIGNMENT_TRASH_TRAP:
		case RESHUFFLE_TRAP:
		case MUSEHAND_TRAP:
		case DOGSIDE_TRAP:
		case BANKRUPT_TRAP:
		case FILLUP_TRAP:
		case AIRSTRIKE_TRAP:
		case DYNAMITE_TRAP:
		case MALEVOLENCE_TRAP:
		case LEAFLET_TRAP:
		case TENTADEEP_TRAP:
		case STATHALF_TRAP:
		case CUTSTAT_TRAP:
		case RARE_SPAWN_TRAP:
		case YOU_ARE_AN_IDIOT_TRAP:
		case NASTYCURSE_TRAP:
		case DAGGER_TRAP:
		case RAZOR_TRAP:
		case PHOSGENE_TRAP:
		case CHLOROFORM_TRAP:
		case CORROSION_TRAP:
		case FLAME_TRAP:
		case WITHER_TRAP:
		case VULNERATE_TRAP:

		case FALLING_ROCK_COLD:
		case RETURN_TRAP:
		case INTRINSIC_STEAL_TRAP:
		case SCORE_AXE_TRAP:
		case SCORE_DRAIN_TRAP:
		case SINGLE_UNIDENTIFY_TRAP:
		case UNLUCKY_TRAP:
		case ALIGNMENT_REDUCTION_TRAP:
		case MALIGNANT_TRAP:
		case WILD_WEATHER_TRAP:
		case STAT_DAMAGE_TRAP:
		case HALF_MEMORY_TRAP:
		case HALF_TRAINING_TRAP:
		case DEBUFF_TRAP:
		case TRIP_ONCE_TRAP:
		case NARCOLEPSY_TRAP:
		case MARTIAL_ARTS_TRAP:

		case INSTANASTY_TRAP:
		case SKILL_POINT_LOSS_TRAP:
		case PERFECT_MATCH_TRAP:
		case DUMBIE_LIGHTSABER_TRAP:
		case WRONG_STAIRS:
		case TECHSTOP_TRAP:
		case AMNESIA_SWITCH_TRAP:
		case SKILL_SWAP_TRAP:
		case SKILL_UPORDOWN_TRAP:
		case SKILL_RANDOMIZE_TRAP:
			You("have triggered a trap!");
			dotrap(ttmp, 0);
			return(1);
		case LOUDSPEAKER:
		case RMB_LOSS_TRAP:
		case SUPERSCROLLER_TRAP:
		case ACTIVE_SUPERSCROLLER_TRAP:
		case MENU_TRAP:
		case SPEED_TRAP:
		case SWARM_TRAP:
		case AUTOMATIC_SWITCHER:
		case DISPLAY_TRAP:
		case SPELL_LOSS_TRAP:
		case YELLOW_SPELL_TRAP:
		case AUTO_DESTRUCT_TRAP:
		case MEMORY_TRAP:
		case INVENTORY_TRAP:
		case BLACK_NG_WALL_TRAP:
		case UNKNOWN_TRAP:
		case TRAP_PERCENTS:
		case FREE_HAND_TRAP:
		case UNIDENTIFY_TRAP:
		case THIRST_TRAP:
		case LUCK_TRAP:
		case SHADES_OF_GREY_TRAP:
		case FAINT_TRAP:
		case CURSE_TRAP:
		case DIFFICULTY_TRAP:
		case SOUND_TRAP:
		case CASTER_TRAP:
		case WEAKNESS_TRAP:
		case ROT_THIRTEEN_TRAP:
		case BISHOP_TRAP:
		case UNINFORMATION_TRAP:
		case TIMERUN_TRAP:
		case BOUND_DAY_CHOICE_TRAP:
		case ANTISWITCH_TRAP:
		case SPELL_COOLDOWN_TRAP:
		case TURBODULL_TRAP:
		case REDDAM_TRAP:
		case REDINC_TRAP:
		case MULCH_TRAP:
		case INACCURACY_TRAP:
		case MONSTER_MULTISHOT_TRAP:
		case DO_YOU_HAVE_A_PIN_TRAP:
		case COSTLY_FAILURE_TRAP:
		case INVENTORY_SIZE_TRAP:
		case MEAN_BURDEN_TRAP:
		case CARRCAP_TRAP:
		case UMENG_TRAP:
		case AEFDE_TRAP:
		case EPVI_TRAP:
		case FUCKFUCKFUCK_TRAP:
		case OPTION_TRAP:
		case MISCOLOR_TRAP:
		case ONE_RAINBOW_TRAP:
		case COLORSHIFT_TRAP:
		case TOP_LINE_TRAP:
		case CAPS_TRAP:
		case UN_KNOWLEDGE_TRAP:
		case DARKHANCE_TRAP:
		case DSCHUEUEUET_TRAP:
		case NOPESKILL_TRAP:
		case REAL_LIE_TRAP:
		case ESCAPE_PAST_TRAP:
		case PETHATE_TRAP:
		case PET_LASHOUT_TRAP:
		case PETSTARVE_TRAP:
		case PETSCREW_TRAP:
		case TECH_LOSS_TRAP:
		case PROOFLOSS_TRAP:
		case UN_INVIS_TRAP:
		case DETECTATION_TRAP:
		case ARTIBLAST_TRAP:
		case REPEATING_NASTYCURSE_TRAP:
		case REALLY_BAD_TRAP:
		case COVID_TRAP:
		case GIANT_EXPLORER_TRAP:
		case TRAPWARP_TRAP:
		case YAWM_TRAP:
		case CRADLE_OF_CHAOS_TRAP:
		case TEZCATLIPOCA_TRAP:
		case ENTHUMESIS_TRAP:
		case MIKRAANESIS_TRAP:
		case GOTS_TOO_GOOD_TRAP:
		case KILLER_ROOM_TRAP:
		case NO_FUN_WALLS_TRAP:
		case BAD_PART_TRAP:
		case COMPLETELY_BAD_PART_TRAP:
		case EVIL_VARIANT_TRAP:
		case ALIGNMENT_TRAP:
		case CONFUSION_TRAP:
		case NUPESELL_TRAP:
		case DROP_TRAP:
		case DSTW_TRAP:
		case STATUS_TRAP:
		case STAIRS_TRAP:
		case INTRINSIC_LOSS_TRAP:
		case BLOOD_LOSS_TRAP:
		case BAD_EFFECT_TRAP:
		case MULTIPLY_TRAP:
		case AUTO_VULN_TRAP:
		case TELE_ITEMS_TRAP:
		case NASTINESS_TRAP:
		case RECURSION_TRAP:
		case RESPAWN_TRAP:
		case WARP_ZONE:
		case CAPTCHA_TRAP:
		case MIND_WIPE_TRAP:
		case THROWING_STAR_TRAP:
		case LOCK_TRAP:
		case MAGIC_CANCELLATION_TRAP:
		case FARLOOK_TRAP:
		case GATEWAY_FROM_HELL:
		case GROWING_TRAP:
		case COOLING_TRAP:
		case BAR_TRAP:
		case LOCKING_TRAP:
		case AIR_TRAP:
		case TERRAIN_TRAP:
		case ORANGE_SPELL_TRAP:
		case VIOLET_SPELL_TRAP:
		case TRAP_OF_LONGING:
		case CURSED_PART_TRAP:
		case QUAVERSAL_TRAP:
		case APPEARANCE_SHUFFLING_TRAP:
		case BROWN_SPELL_TRAP:
		case CHOICELESS_TRAP:
		case GOLDSPELL_TRAP:
		case DEPROVEMENT_TRAP:
		case INITIALIZATION_TRAP:
		case GUSHLUSH_TRAP:
		case SOILTYPE_TRAP:
		case DANGEROUS_TERRAIN_TRAP:
		case FALLOUT_TRAP:
		case MOJIBAKE_TRAP:
		case GRAVATION_TRAP:
		case UNCALLED_TRAP:
		case EXPLODING_DICE_TRAP:
		case PERMACURSE_TRAP:
		case SHROUDED_IDENTITY_TRAP:
		case FEELER_GAUGES_TRAP:
		case LONG_SCREWUP_TRAP:
		case WING_YELLOW_CHANGER:
		case LIFE_SAVING_TRAP:
		case CURSEUSE_TRAP:
		case CUT_NUTRITION_TRAP:
		case SKILL_LOSS_TRAP:
		case AUTOPILOT_TRAP:
		case FORCE_TRAP:
		case MONSTER_GLYPH_TRAP:
		case CHANGING_DIRECTIVE_TRAP:
		case CONTAINER_KABOOM_TRAP:
		case STEAL_DEGRADE_TRAP:
		case LEFT_INVENTORY_TRAP:
		case FLUCTUATING_SPEED_TRAP:
		case TARMUSTROKINGNORA_TRAP:
		case FAILURE_TRAP:
		case BRIGHT_CYAN_SPELL_TRAP:
		case FREQUENTATION_SPAWN_TRAP:
		case PET_AI_TRAP:
		case SATAN_TRAP:
		case REMEMBERANCE_TRAP:
		case POKELIE_TRAP:
		case AUTOPICKUP_TRAP:
		case DYWYPI_TRAP:
		case SILVER_SPELL_TRAP:
		case METAL_SPELL_TRAP:
		case PLATINUM_SPELL_TRAP:
		case MANLER_TRAP:
		case DOORNING_TRAP:
		case NOWNSIBLE_TRAP:
		case ELM_STREET_TRAP:
		case MONNOISE_TRAP:
		case RANG_CALL_TRAP:
		case RECURRING_SPELL_LOSS_TRAP:
		case ANTITRAINING_TRAP:
		case TECHOUT_TRAP:
		case STAT_DECAY_TRAP:
		case MOVEMORK_TRAP:

		case LOOTCUT_TRAP:
		case MONSTER_SPEED_TRAP:
		case SCALING_TRAP:
		case ENMITY_TRAP:
		case WHITE_SPELL_TRAP:
		case COMPLETE_GRAY_SPELL_TRAP:
		case QUASAR_TRAP:
		case MOMMA_TRAP:
		case HORROR_TRAP:
		case ARTIFICER_TRAP:
		case WEREFORM_TRAP:
		case NON_PRAYER_TRAP:
		case EVIL_PATCH_TRAP:
		case HARD_MODE_TRAP:
		case SECRET_ATTACK_TRAP:
		case EATER_TRAP:
		case COVETOUSNESS_TRAP:
		case NOT_SEEN_TRAP:
		case DARK_MODE_TRAP:
		case ANTISEARCH_TRAP:
		case HOMICIDE_TRAP:
		case NASTY_NATION_TRAP:
		case WAKEUP_CALL_TRAP:
		case GRAYOUT_TRAP:
		case GRAY_CENTER_TRAP:
		case CHECKERBOARD_TRAP:
		case CLOCKWISE_SPIN_TRAP:
		case COUNTERCLOCKWISE_SPIN_TRAP:
		case LAG_TRAP:
		case BLESSCURSE_TRAP:
		case DE_LIGHT_TRAP:
		case DISCHARGE_TRAP:
		case TRASHING_TRAP:
		case FILTERING_TRAP:
		case DEFORMATTING_TRAP:
		case FLICKER_STRIP_TRAP:
		case UNDRESSING_TRAP:
		case HYPERBLUEWALL_TRAP:
		case NOLITE_TRAP:
		case PARANOIA_TRAP:
		case FLEECESCRIPT_TRAP:
		case INTERRUPT_TRAP:
		case DUSTBIN_TRAP:
		case MANA_BATTERY_TRAP:
		case MONSTERFINGERS_TRAP:
		case MISCAST_TRAP:
		case MESSAGE_SUPPRESSION_TRAP:
		case STUCK_ANNOUNCEMENT_TRAP:
		case BLOODTHIRSTY_TRAP:
		case MAXIMUM_DAMAGE_TRAP:
		case LATENCY_TRAP:
		case STARLIT_TRAP:
		case KNOWLEDGE_TRAP:
		case HIGHSCORE_TRAP:
		case PINK_SPELL_TRAP:
		case GREEN_SPELL_TRAP:
		case EVC_TRAP:
		case UNDERLAYER_TRAP:
		case DAMAGE_METER_TRAP:
		case ARBITRARY_WEIGHT_TRAP:
		case FUCKED_INFO_TRAP:
		case BLACK_SPELL_TRAP:
		case CYAN_SPELL_TRAP:
		case HEAP_TRAP:
		case BLUE_SPELL_TRAP:
		case TRON_TRAP:
		case RED_SPELL_TRAP:
		case TOO_HEAVY_TRAP:
		case ELONGATION_TRAP:
		case WRAPOVER_TRAP:
		case DESTRUCTION_TRAP:
		case MELEE_PREFIX_TRAP:
		case AUTOMORE_TRAP:
		case UNFAIR_ATTACK_TRAP:

		case RECURRING_AMNESIA_TRAP:
		case BIGSCRIPT_TRAP:
		case BANK_TRAP:
		case ONLY_TRAP:
		case MAP_TRAP:
		case TECH_TRAP:
		case DISENCHANT_TRAP:
		case VERISIERT:
		case CHAOS_TRAP:
		case MUTENESS_TRAP:
		case NTLL_TRAP:
		case ENGRAVING_TRAP:
		case MAGIC_DEVICE_TRAP:
		case BOOK_TRAP:
		case LEVEL_TRAP:
		case QUIZ_TRAP:

		case METABOLIC_TRAP:
		case TRAP_OF_NO_RETURN:
		case EGOTRAP:
		case FAST_FORWARD_TRAP:
		case TRAP_OF_ROTTENNESS:
		case UNSKILLED_TRAP:
		case LOW_STATS_TRAP:
		case TRAINING_TRAP:
		case EXERCISE_TRAP:
		case LIMITATION_TRAP:
		case WEAK_SIGHT_TRAP:
		case RANDOM_MESSAGE_TRAP:

		case DESECRATION_TRAP:
		case STARVATION_TRAP:
		case DROPLESS_TRAP:
		case LOW_EFFECT_TRAP:
		case INVISIBLE_TRAP:
		case GHOST_WORLD_TRAP:
		case DEHYDRATION_TRAP:
		case HATE_TRAP:
		case SPACEWARS_TRAP:
		case TEMPORARY_RECURSION_TRAP:
		case TOTTER_TRAP:
		case NONINTRINSICAL_TRAP:
		case DROPCURSE_TRAP:
		case NAKEDNESS_TRAP:
		case ANTILEVEL_TRAP:
		case VENTILATOR:
		case STEALER_TRAP:
		case REBELLION_TRAP:
		case CRAP_TRAP:
		case MISFIRE_TRAP:
		case TRAP_OF_WALLS:
		case DISCONNECT_TRAP:
		case INTERFACE_SCREW_TRAP:
		case BOSSFIGHT_TRAP:
		case ENTIRE_LEVEL_TRAP:
		case BONES_TRAP:
		case AUTOCURSE_TRAP:
		case HIGHLEVEL_TRAP:
		case SPELL_FORGETTING_TRAP:
		case SOUND_EFFECT_TRAP:
		case KOP_CUBE:
		case BOSS_SPAWNER:
		case SANITY_TREBLE_TRAP:
		case STAT_DECREASE_TRAP:
		case SIMEOUT_TRAP:

			dotrap(ttmp, 0);
			return(1);
		default:
			impossible("Webbing over trap type %d?", ttmp->ttyp);
			return(0);
		}
	else if (On_stairs(u.ux, u.uy)) {
	    /* cop out: don't let them hide the stairs */
	    Your("web fails to impede access to the %s.",
		 (levl[u.ux][u.uy].typ == STAIRS) ? "stairs" : "ladder");
	    return(1);
		 
	}
	ttmp = maketrap(u.ux, u.uy, WEB, 0, FALSE);
	if (ttmp && !ttmp->hiddentrap) {
		ttmp->tseen = 1;
		ttmp->madeby_u = 1;
	}
	newsym(u.ux, u.uy);
	return(1);
}

int
dosummon()
{
	int placeholder;
	int somanymana;
	somanymana = 10;
	int squeakamount = 0;

	if (u.ulycn == PM_WERESOLDIERANT) somanymana = 15;
	if (u.ulycn == PM_WEREWOLF) somanymana = 20;
	if (u.ulycn == PM_WEREPIRANHA) somanymana = 20;
	if (u.ulycn == PM_WEREEEL) somanymana = 25;
	if (u.ulycn == PM_WEREKRAKEN) somanymana = 45;
	if (u.ulycn == PM_WEREFLYFISH) somanymana = 45;
	if (u.ulycn == PM_WEREPANTHER) somanymana = 30;
	if (u.ulycn == PM_WERETIGER) somanymana = 30;
	if (u.ulycn == PM_WERESNAKE) somanymana = 20;
	if (u.ulycn == PM_WERECOW) somanymana = 20;
	if (u.ulycn == PM_WEREBEAR) somanymana = 75;
	if (u.ulycn == PM_WEREPHANT) somanymana = 75;
	if (u.ulycn == PM_WEREVORTEX) somanymana = 50;
	if (u.ulycn == PM_WERETROLL) somanymana = 50;
	if (u.ulycn == PM_WEREGIANT) somanymana = 50;
	if (u.ulycn == PM_WEREGHOST) somanymana = 30;
	if (u.ulycn == PM_WERECOCKATRICE) somanymana = 60;
	if (u.ulycn == PM_WERELICH) somanymana = 100;
	if (u.ulycn == PM_UN_IN_PROTECT_MODE) somanymana = 100;
	if (u.ulycn == PM_WEREDEMON) somanymana = 100;
	if (u.ulycn == PM_WEREMINDFLAYER) somanymana = 150;
	if (u.ulycn == PM_VORPAL_WERE_ALHOONTRICE_ZOMBIE) somanymana = 150;
	if (u.ulycn == PM_WEREJABBERWOCK) somanymana = 200;
	if (u.ulycn == PM_WEREWEDGESANDAL) somanymana = 80;
	if (u.ulycn == PM_WEREHUGGINGBOOT) somanymana = 120;
	if (u.ulycn == PM_WEREPEEPTOE) somanymana = 140;
	if (u.ulycn == PM_WERESEXYLEATHERPUMP) somanymana = 160;
	if (u.ulycn == PM_WEREBLOCKHEELEDCOMBATBOOT) somanymana = 160;
	if (u.ulycn == PM_WERECOMBATSTILETTO) somanymana = 200;
	if (u.ulycn == PM_WEREBEAUTIFULFUNNELHEELEDPUMP) somanymana = 240;
	if (u.ulycn == PM_WEREPROSTITUTESHOE) somanymana = 240;
	if (u.ulycn == PM_WERESTILETTOSANDAL) somanymana = 250;
	if (u.ulycn == PM_WEREUNFAIRSTILETTO) somanymana = 260;
	if (u.ulycn == PM_WEREWINTERSTILETTO) somanymana = 300;

	if (u.umonnum == PM_WERESOLDIERANT && somanymana < 15) somanymana = 15;
	if (u.umonnum == PM_WEREWOLF && somanymana < 20) somanymana = 20;
	if (u.umonnum == PM_WEREPIRANHA && somanymana < 20) somanymana = 20;
	if (u.umonnum == PM_WEREEEL && somanymana < 25) somanymana = 25;
	if (u.umonnum == PM_WEREKRAKEN && somanymana < 45) somanymana = 45;
	if (u.umonnum == PM_WEREFLYFISH && somanymana < 45) somanymana = 45;
	if (u.umonnum == PM_WEREPANTHER && somanymana < 30) somanymana = 30;
	if (u.umonnum == PM_WERETIGER && somanymana < 30) somanymana = 30;
	if (u.umonnum == PM_WERESNAKE && somanymana < 20) somanymana = 20;
	if (u.umonnum == PM_WERECOW && somanymana < 20) somanymana = 20;
	if (u.umonnum == PM_WEREBEAR && somanymana < 75) somanymana = 75;
	if (u.umonnum == PM_WEREPHANT && somanymana < 75) somanymana = 75;
	if (u.umonnum == PM_WEREVORTEX && somanymana < 50) somanymana = 50;
	if (u.umonnum == PM_WERETROLL && somanymana < 50) somanymana = 50;
	if (u.umonnum == PM_WEREGIANT && somanymana < 50) somanymana = 50;
	if (u.umonnum == PM_WEREGHOST && somanymana < 30) somanymana = 30;
	if (u.umonnum == PM_WERECOCKATRICE && somanymana < 60) somanymana = 60;
	if (u.umonnum == PM_WERELICH && somanymana < 100) somanymana = 100;
	if (u.umonnum == PM_UN_IN_PROTECT_MODE && somanymana < 100) somanymana = 100;
	if (u.umonnum == PM_WEREDEMON && somanymana < 100) somanymana = 100;
	if (u.umonnum == PM_WEREMINDFLAYER && somanymana < 150) somanymana = 150;
	if (u.umonnum == PM_VORPAL_WERE_ALHOONTRICE_ZOMBIE && somanymana < 150) somanymana = 150;
	if (u.umonnum == PM_WEREJABBERWOCK && somanymana < 200) somanymana = 200;
	if (u.umonnum == PM_WEREWEDGESANDAL && somanymana < 80) somanymana = 80;
	if (u.umonnum == PM_WEREHUGGINGBOOT && somanymana < 120) somanymana = 120;
	if (u.umonnum == PM_WEREPEEPTOE && somanymana < 140) somanymana = 140;
	if (u.umonnum == PM_WERESEXYLEATHERPUMP && somanymana < 160) somanymana = 160;
	if (u.umonnum == PM_WEREBLOCKHEELEDCOMBATBOOT && somanymana < 160) somanymana = 160;
	if (u.umonnum == PM_WERECOMBATSTILETTO && somanymana < 200) somanymana = 200;
	if (u.umonnum == PM_WEREBEAUTIFULFUNNELHEELEDPUMP && somanymana < 240) somanymana = 240;
	if (u.umonnum == PM_WEREPROSTITUTESHOE && somanymana < 240) somanymana = 240;
	if (u.umonnum == PM_WERESTILETTOSANDAL && somanymana < 250) somanymana = 250;
	if (u.umonnum == PM_WEREUNFAIRSTILETTO && somanymana < 260) somanymana = 260;
	if (u.umonnum == PM_WEREWINTERSTILETTO && somanymana < 300) somanymana = 300;

	squeakamount = somanymana;
	/* we can now use the squeaking skill to reduce the cost */

	if (!PlayerCannotUseSkills && somanymana > 2) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	somanymana *= 9; somanymana /= 10; break;
	      	case P_SKILLED:	somanymana *= 8; somanymana /= 10; break;
	      	case P_EXPERT:	somanymana *= 7; somanymana /= 10; break;
	      	case P_MASTER:	somanymana *= 6; somanymana /= 10; break;
	      	case P_GRAND_MASTER:	somanymana *= 5; somanymana /= 10; break;
	      	case P_SUPREME_MASTER:	somanymana *= 4; somanymana /= 10; break;
	      	default: break;
		}
	}

	if (u.uen < somanymana) {
	    You("lack the energy to send forth a call for help! You need at least %d!",somanymana);
	    return(0);
	}
	u.uen -= somanymana;
	flags.botl = 1;

	You("call upon your brethren for help!");
	exercise(A_WIS, TRUE);
	if (!were_summon(youmonst.data, TRUE, &placeholder, (char *)0, FALSE))
		pline("But none arrive.");

	while (squeakamount > 40) {
		use_skill(P_SQUEAKING, 1);
		squeakamount -= 40;
	}
	use_skill(P_SQUEAKING, 1);

	return(1);
}


#if 0
/* WAC supplanted by dogaze (). */
int
dogaze()
{
	register struct monst *mtmp;
	int looked = 0;
	char qbuf[QBUFSZ];
	int i;
	uchar adtyp = 0;

	for (i = 0; i < NATTK; i++) {
	    if(youmonst.data->mattk[i].aatyp == AT_GAZE) {
		adtyp = youmonst.data->mattk[i].adtyp;
		break;
	    }
	}
	if (adtyp != AD_CONF && adtyp != AD_FIRE) {
	    impossible("gaze attack %d?", adtyp);
	    return 0;
	}


	if (Blind) {
	    You_cant("see anything to gaze at.");
	    return 0;
	}
	if (u.uen < 15) {
	    You("lack the energy to use your special gaze! You need at least 15 points of mana!");
	    return(0);
	}
	u.uen -= 15;
	flags.botl = 1;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my)) {
		looked++;
		if (Invis && !perceives(mtmp->data))
		    pline("%s seems not to notice your gaze.", Monnam(mtmp));
		else if ((mtmp->minvis && (!See_invisible || (!StrongSee_invisible && !mtmp->seeinvisble)) ) || mtmp->minvisreal)
		    You_cant("see where to gaze at %s.", Monnam(mtmp));
		else if (mtmp->m_ap_type == M_AP_FURNITURE
			|| mtmp->m_ap_type == M_AP_OBJECT) {
		    looked--;
		    continue;
		} else if (flags.safe_dog && !Confusion && !Hallucination
		  && mtmp->mtame) {
		    You("avoid gazing at %s.", y_monnam(mtmp));
		} else {
		    if (flags.confirm && mtmp->mpeaceful && !Confusion
							&& !Hallucination) {
			sprintf(qbuf, "Really %s %s?",
			    (adtyp == AD_CONF) ? "confuse" : "attack",
			    mon_nam(mtmp));
			if (yn(qbuf) != 'y') continue;
			setmangry(mtmp);
		    }
		    if (!mtmp->mcanmove || mtmp->mstun || mtmp->msleeping ||
				    !mtmp->mcansee || !haseyes(mtmp->data)) {
			looked--;
			continue;
		    }
		    /* No reflection check for consistency with when a monster
		     * gazes at *you*--only medusa gaze gets reflected then.
		     */
		    if (adtyp == AD_CONF) {
			if (!mtmp->mconf)
			    Your("gaze confuses %s!", mon_nam(mtmp));
			else
			    pline("%s is getting more and more confused.",
							Monnam(mtmp));
			mtmp->mconf = 1;
		    } else if (adtyp == AD_FIRE) {
			int dmg = d(2,6);
			You("attack %s with a fiery gaze!", mon_nam(mtmp));
			if (resists_fire(mtmp) && !player_will_pierce_resistance()) {
			    pline_The("fire doesn't burn %s!", mon_nam(mtmp));
			    dmg = 0;
			}
			if(!rn2(33))
			    (void) destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
			if(!rn2(33))
			    (void) destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);
			if(!rn2(33))
			    (void) destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
			if (dmg && !DEADMONSTER(mtmp)) mtmp->mhp -= dmg;
			if (mtmp->mhp <= 0) killed(mtmp);
		    }
		    /* For consistency with passive() in uhitm.c, this only
		     * affects you if the monster is still alive.
		     */
		    if (!DEADMONSTER(mtmp) &&
			  (mtmp->data==&mons[PM_FLOATING_EYE]) && !mtmp->mcan) {
			if (!Free_action) {
			    You("are frozen by %s gaze!",
					     s_suffix(mon_nam(mtmp)));
			    nomul(-d((int)mtmp->m_lev+1, (int)mtmp->data->mattk[0].damd), "frozen by stupidly gazing at a floating eye", TRUE);
			    return 1;
			} else
			    You("stiffen momentarily under %s gaze.",
				    s_suffix(mon_nam(mtmp)));
		    }
		    /* Technically this one shouldn't affect you at all because
		     * the Medusa gaze is an active monster attack that only
		     * works on the monster's turn, but for it to *not* have an
		     * effect would be too weird.
		     */
		    if (!DEADMONSTER(mtmp) &&
			    (mtmp->data==&mons[PM_MEDUSA]) && !mtmp->mcan) {
			pline(
			 "Gazing at the awake %s is not a very good idea.",
			    l_monnam(mtmp));
			/* as if gazing at a sleeping anything is fruitful... */
			/*You("turn to stone...");
			killer_format = KILLED_BY;
			killer = "deliberately meeting Medusa's gaze";
			done(STONING);*/
			You("start turning to stone...");
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("But you are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					delayed_killer = "deliberately meeting Medusa's gaze";
				}
			}
		    }
		}
	    }
	}
	if (!looked) You("gaze at no place in particular.");
	return 1;
}
#endif


int
dohide()
{
	boolean ismimic = youmonst.data->mlet == S_MIMIC;

	/* costs mana if you're mimicking, but not if you're merely hiding --Amy */
	int hidingcost = 25;

	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	hidingcost = 24; break;
	      	case P_SKILLED:	hidingcost = 23; break;
	      	case P_EXPERT:	hidingcost = 22; break;
	      	case P_MASTER:	hidingcost = 21; break;
	      	case P_GRAND_MASTER:	hidingcost = 20; break;
	      	case P_SUPREME_MASTER:	hidingcost = 19; break;
	      	default: break;
		}
	}

	if (u.uundetected || (ismimic && youmonst.m_ap_type != M_AP_NOTHING)) {
		You("are already hiding.");
		return(0);
	}
	if (ismimic) {

		if (u.uen < hidingcost) {
		    You("lack the required %d mana to mimic an object.", hidingcost);
		    return(0);
		}
		u.uen -= hidingcost;

		/* should bring up a dialog "what would you like to imitate?" */
		youmonst.m_ap_type = M_AP_OBJECT;
		youmonst.mappearance = STRANGE_OBJECT;

		use_skill(P_SQUEAKING, 2); /* trains squeaking only if you used mana */

	} else
		u.uundetected = 1;
	newsym(u.ux,u.uy);
	return(1);
}

int
domindblast()
{
	struct monst *mtmp, *nmon;
	int mindblastcost = 10;

	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	mindblastcost = 9 + rn2(2); break;
	      	case P_SKILLED:	mindblastcost = 8 + rn2(3); break;
	      	case P_EXPERT:	mindblastcost = 7 + rn2(4); break;
	      	case P_MASTER:	mindblastcost = 6 + rn2(5); break;
	      	case P_GRAND_MASTER:	mindblastcost = 5 + rn2(6); break;
	      	case P_SUPREME_MASTER:	mindblastcost = 4 + rn2(7); break;
	      	default: break;
		}
	}

	if (u.uen < mindblastcost) {
	    You("concentrate but lack the energy to maintain doing so. Wimp, you don't even have 10 mana to spare?!");
	    return(0);
	}
	u.uen -= mindblastcost;
	flags.botl = 1;

	pline("A wave of psychic energy pours out.");
	for(mtmp=fmon; mtmp; mtmp = nmon) {
		int u_sen;

		nmon = mtmp->nmon;
		if (DEADMONSTER(mtmp))
			continue;
		if (distu(mtmp->mx, mtmp->my) > BOLT_LIM * BOLT_LIM)
			continue;
		if(mtmp->mpeaceful)
			continue;
		u_sen = telepathic(mtmp->data) && !mtmp->mcansee;
		if (u_sen || (telepathic(mtmp->data) && rn2(3)) || !rn2(2)) {

			int mindblastdmg = rn1(4,4);
			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_TSCHHKRZKRZ) mindblastdmg *= rn1(2, 2);

			You("lock in on %s %s.", s_suffix(mon_nam(mtmp)),
				u_sen ? "telepathy" :
				telepathic(mtmp->data) ? "latent telepathy" :
				"mind");
			mtmp->mhp -= mindblastdmg;
			if (mtmp->mhp <= 0)
				killed(mtmp);
		}
	}
	use_skill(P_SQUEAKING, 1);

	return 1;
}

STATIC_OVL void
uunstick()
{
	pline("%s is no longer in your clutches.", Monnam(u.ustuck));
	setustuck(0);
}

void
skinback(silently)
boolean silently;
{
	if (uskin) {
		if (!silently) Your("%s returns to its original form.", body_part(BODY_SKIN));
		uarm = uskin;
		uskin = (struct obj *)0;
		/* undo save/restore hack */
		uarm->owornmask &= ~I_SPECIAL;
	}
}

#endif /* OVLB */
#ifdef OVL1

const char *
mbodypart(mon, part)
struct monst *mon;
int part;
{
	static NEARDATA const char
	*humanoid_parts[] = { "arm", "eye", "face", "finger",
		"fingertip", "foot", "hand", "handed", "head", "leg",
		"light headed", "neck", "spine", "toe", "hair",
		"blood", "lung", "nose", "stomach",
		"heart", "skin", "flesh", "beat",
		"bones", "ear", "ears", "creak", "crack"
		},
	*jelly_parts[] = { "pseudopod", "dark spot", "front",
		"pseudopod extension", "pseudopod extremity",
		"pseudopod root", "grasp", "grasped", "cerebral area",
		"lower pseudopod", "viscous", "middle", "surface",
		"pseudopod extremity", "ripples", "juices",
		"tiny cilia", "chemosensor", "vacuoles",
		"cytoskeletal structure", "membrane", "cortex", "shift",
		"cytoskeletal filaments", "outer membrane", "outer membranes", "creak", "crack"
		},
	*animal_parts[] = { "forelimb", "eye", "face", "foreclaw", "claw tip",
		"rear claw", "foreclaw", "clawed", "head", "rear limb",
		"light headed", "neck", "spine", "rear claw tip",
		"fur", "blood", "lung", "nose", "stomach",
		"heart", "skin", "flesh", "beat",
		"bones", "ear", "ears", "creak", "crack"
		},
	*bird_parts[] = { "wing", "eye", "face", "wing", "wing tip",
		"foot", "wing", "winged", "head", "leg",
		"light headed", "neck", "spine", "toe",
		"feathers", "blood", "lung", "bill", "stomach",
		"heart", "skin", "flesh", "beat",
		"bones", "ear", "ears", "creak", "crack"
		},
	*horse_parts[] = { "foreleg", "eye", "face", "forehoof", "hoof tip",
		"rear hoof", "foreclaw", "hooved", "head", "rear leg",
		"light headed", "neck", "backbone", "rear hoof tip",
		"mane", "blood", "lung", "nose", "stomach",
		"heart", "skin", "flesh", "beat",
		"bones", "ear", "ears", "creak", "crack"
		},
	*sphere_parts[] = { "appendage", "optic nerve", "body", "tentacle",
		"tentacle tip", "lower appendage", "primary tentacle", "tentacled",
		"center", "lower tentacle", "rotational", "equator", "skeleton",
		"lower tentacle tip", "cilia", "life force", "retina",
		"olfactory nerve", "interior",
		"core", "surface", "subsurface layers", "pulse",
		"auras", "tympanic membrane", "tympanic membranes", "flicker", "blink out"
		},
	*fungus_parts[] = { "mycelium", "visual area", "front", "hypha",
		"hypha extension", "root", "strand", "stranded", "cap area",
		"rhizome", "sporulated", "stalk", "skeleton", "rhizome tip",
		"spores", "juices", "gill", "respiratory orifice", "interior",
		"hyphal network", "cuticle", "flesh", "twitch",
		"exoskeleton", "tympanic area", "tympanic area", "stretch", "tear"
		},
	*vortex_parts[] = { "region", "eye", "front", "major current",
		"minor current", "nether current", "swirl", "swirled",
		"central core", "lower current", "addled", "center",
		"currents", "edge", "rotational part", "life force",
		"center", "leading edge", "interior",
		"core", "vaporous currents", "subsurface currents", "pulse",
		"bits", "vapor", "vapor", "weaken", "falter"
		},
	*snake_parts[] = { "vestigial limb", "eye", "face", "large scale",
		"large scale tip", "lower region", "scale gap", "scale gapped",
		"head", "rear region", "light headed", "neck", "length",
		"rear scale", "scales", "blood", "lung", "forked tongue", "stomach",
		"heart", "skin", "flesh", "beat",
		"bones", "ear", "ears", "creak", "crack"
		},
	*wallmonst_parts[] = { "long wall", "center core", "top region", "moving wall",
		"wall extension", "bottom region", "wall extension", "extended", "central core", "tall wall",
		"misdirectional", "support column", "large column", "lower extension", "painting",
		"stone fragments", "inner segment", "front support", "middle wall",
		"main column", "mineral", "stone", "rattle",
		"cement", "upper extension", "upper extension", "moan", "shatter"
		},
	*turret_parts[] = { "gun arm", "lens", "calculation region", "gun",
		"laser point", "foot", "laser gun", "gunned", "CPU", "pod",
		"frenzied", "support", "back", "mainframe", "combat inhibitor",
		"electrical energy", "RAM", "power", "armor",
		"chip", "cables", "software", "ping",
		"backbone", "listening unit", "listening unit", "screak", "shatter"
		},
	*fish_parts[] = { "fin", "eye", "premaxillary", "pelvic axillary",
		"pelvic fin", "anal fin", "pectoral fin", "finned", "head", "peduncle",
		"played out", "gills", "dorsal fin", "caudal fin",
		"scales", "blood", "gill", "nostril", "stomach",
		"heart", "skin", "flesh", "beat",
		"bones", "ear", "ears", "creak", "crack"
		};
	/* claw attacks are overloaded in mons[]; most humanoids with
	   such attacks should still reference hands rather than claws */
	static const char not_claws[] = {
		S_HUMAN, S_MUMMY, S_ZOMBIE, S_ANGEL,
		S_NYMPH, S_LEPRECHAUN, S_QUANTMECH, S_VAMPIRE,
		S_ORC, S_GIANT,		/* quest nemeses */
		'\0'		/* string terminator; assert( S_xxx != 0 ); */
	};
	struct permonst *mptr = mon->data;

	if (part == HAND || part == HANDED) {	/* some special cases */
	    if (mptr->mlet == S_DOG || mptr->mlet == S_FELINE ||
		    mptr->mlet == S_YETI)
		return part == HAND ? "paw" : "pawed";
	    if (humanoid(mptr) && attacktype(mptr, AT_CLAW) &&
		    !index(not_claws, mptr->mlet) &&
		    mptr != &mons[PM_STONE_GOLEM] &&
		    mptr != &mons[PM_INCUBUS] && mptr != &mons[PM_SUCCUBUS])
		return part == HAND ? "claw" : "clawed";
	}
	if ((mptr == &mons[PM_MUMAK] || mptr == &mons[PM_MASTODON]) &&
		part == NOSE)
	    return "trunk";
	if (mptr == &mons[PM_SHARK] && part == HAIR)
	    return "skin";	/* sharks don't have scales */
	if (mptr == &mons[PM_JELLYFISH] && (part == ARM || part == FINGER ||
	    part == HAND || part == FOOT || part == TOE))
	    return "tentacle";
	if (mptr == &mons[PM_FLOATING_EYE] && part == EYE)
	    return "cornea";
	if (humanoid(mptr) &&
		(part == ARM || part == FINGER || part == FINGERTIP ||
		    part == HAND || part == HANDED))
	    return humanoid_parts[part];
	if (mptr->mlet == S_BAT)
	    return bird_parts[part];
	if (mptr->mlet == S_CENTAUR || mptr->mlet == S_UNICORN ||
		(mptr == &mons[PM_ROTHE] && part != HAIR))
	    return horse_parts[part];
	if (mptr->mlet == S_LIGHT) {
		if (part == HANDED) return "rayed";
		else if (part == ARM || part == FINGER ||
				part == FINGERTIP || part == HAND) return "ray";
		else return "beam";
	}
	if (mptr->mlet == S_EEL && mptr != &mons[PM_JELLYFISH])
	    return fish_parts[part];
	if (mptr->mlet == S_FLYFISH)
	    return fish_parts[part];
	if (slithy(mptr) || (mptr->mlet == S_DRAGON && part == HAIR))
	    return snake_parts[part];
	if (mptr->mlet == S_EYE || mptr->mlet == S_GRUE)
	    return sphere_parts[part];
	if (mptr->mlet == S_JELLY || mptr->mlet == S_PUDDING ||
		mptr->mlet == S_BLOB || mptr == &mons[PM_JELLYFISH])
	    return jelly_parts[part];
	if (mptr->mlet == S_VORTEX || mptr->mlet == S_ELEMENTAL)
	    return vortex_parts[part];
	if (mptr->mlet == S_FUNGUS)
	    return fungus_parts[part];
	if (mptr->mlet == S_WALLMONST)
	    return wallmonst_parts[part];
	if (mptr->mlet == S_TURRET)
	    return turret_parts[part];
	if (humanoid(mptr))
	    return humanoid_parts[part];
	return animal_parts[part];
}

const char *
body_part(part)
int part;
{
	return mbodypart(&youmonst, part);
}

#endif /* OVL1 */
#ifdef OVL0

int
poly_gender()
{
/* Returns gender of polymorphed player; 0/1=same meaning as flags.female,
 * 2=none.
 */
	if (is_neuter(youmonst.data) || !humanoid(youmonst.data)) return 2;
	return flags.female;
}

#endif /* OVL0 */
#ifdef OVLB

void
ugolemeffects(damtype, dam)
int damtype, dam;
{
	int heal = 0;
	/* We won't bother with "slow"/"haste" since players do not
	 * have a monster-specific slow/haste so there is no way to
	 * restore the old velocity once they are back to human.
	 */
	if (u.umonnum != PM_FLESH_GOLEM && u.umonnum != PM_IRON_GOLEM)
		return;
	switch (damtype) {
		case AD_ELEC: if (u.umonnum == PM_FLESH_GOLEM)
				heal = dam / 6; /* Approx 1 per die */
			break;
		case AD_FIRE: if (u.umonnum == PM_IRON_GOLEM)
				heal = dam;
			break;
	}
	if (heal && (u.mh < u.mhmax)) {
		u.mh += heal;
		if (u.mh > u.mhmax) u.mh = u.mhmax;
		flags.botl = 1;
		pline("Strangely, you feel better than before.");
		exercise(A_STR, TRUE);
	}
}

STATIC_OVL int
armor_to_dragon(atyp)
int atyp;
{
	switch(atyp) {
	    case GRAY_DRAGON_SCALE_MAIL:
	    case GRAY_DRAGON_SCALES:
		return PM_GRAY_DRAGON;
	    case SILVER_DRAGON_SCALE_MAIL:
	    case SILVER_DRAGON_SCALES:
		return PM_SILVER_DRAGON;
	    case MERCURIAL_DRAGON_SCALE_MAIL:
	    case MERCURIAL_DRAGON_SCALES:
		return PM_MERCURIAL_DRAGON;
	    case RAINBOW_DRAGON_SCALE_MAIL:
	    case RAINBOW_DRAGON_SCALES:
		return PM_RAINBOW_DRAGON;
	    case BLOOD_DRAGON_SCALE_MAIL:
	    case BLOOD_DRAGON_SCALES:
		return PM_BLOOD_DRAGON;
	    case PLAIN_DRAGON_SCALE_MAIL:
	    case PLAIN_DRAGON_SCALES:
		return PM_PLAIN_DRAGON;
	    case SKY_DRAGON_SCALE_MAIL:
	    case SKY_DRAGON_SCALES:
		return PM_SKY_DRAGON;
	    case WATER_DRAGON_SCALE_MAIL:
	    case WATER_DRAGON_SCALES:
		return PM_WATER_DRAGON;
	    case EVIL_DRAGON_SCALE_MAIL:
	    case EVIL_DRAGON_SCALES:
		return PM_EVIL_DRAGON;
	    case MAGIC_DRAGON_SCALE_MAIL:
	    case MAGIC_DRAGON_SCALES:
		return PM_MAGIC_DRAGON;
	    case SHIMMERING_DRAGON_SCALE_MAIL:
	    case SHIMMERING_DRAGON_SCALES:
		return PM_SHIMMERING_DRAGON;
	    case DEEP_DRAGON_SCALE_MAIL:
	    case DEEP_DRAGON_SCALES:
		return PM_DEEP_DRAGON;
	    case RED_DRAGON_SCALE_MAIL:
	    case RED_DRAGON_SCALES:
		return PM_RED_DRAGON;
	    case ORANGE_DRAGON_SCALE_MAIL:
	    case ORANGE_DRAGON_SCALES:
		return PM_ORANGE_DRAGON;
	    case WHITE_DRAGON_SCALE_MAIL:
	    case WHITE_DRAGON_SCALES:
		return PM_WHITE_DRAGON;
	    case BLACK_DRAGON_SCALE_MAIL:
	    case BLACK_DRAGON_SCALES:
		return PM_BLACK_DRAGON;
	    case BLUE_DRAGON_SCALE_MAIL:
	    case BLUE_DRAGON_SCALES:
		return PM_BLUE_DRAGON;
	    case WEATHER_DRAGON_SCALE_MAIL:
	    case WEATHER_DRAGON_SCALES:
		return PM_WEATHER_DRAGON;
	    case DARK_DRAGON_SCALE_MAIL:
	    case DARK_DRAGON_SCALES:
		return PM_DARK_DRAGON;
	    case COPPER_DRAGON_SCALE_MAIL:
	    case COPPER_DRAGON_SCALES:
		return PM_COPPER_DRAGON;
	    case PLATINUM_DRAGON_SCALE_MAIL:
	    case PLATINUM_DRAGON_SCALES:
		return PM_PLATINUM_DRAGON;
	    case BRASS_DRAGON_SCALE_MAIL:
	    case BRASS_DRAGON_SCALES:
		return PM_BRASS_DRAGON;
	    case AMETHYST_DRAGON_SCALE_MAIL:
	    case AMETHYST_DRAGON_SCALES:
		return PM_AMETHYST_DRAGON;
	    case PURPLE_DRAGON_SCALE_MAIL:
	    case PURPLE_DRAGON_SCALES:
		return PM_PURPLE_DRAGON;
	    case DIAMOND_DRAGON_SCALE_MAIL:
	    case DIAMOND_DRAGON_SCALES:
		return PM_DIAMOND_DRAGON;
	    case EMERALD_DRAGON_SCALE_MAIL:
	    case EMERALD_DRAGON_SCALES:
		return PM_EMERALD_DRAGON;
	    case SAPPHIRE_DRAGON_SCALE_MAIL:
	    case SAPPHIRE_DRAGON_SCALES:
		return PM_SAPPHIRE_DRAGON;
	    case RUBY_DRAGON_SCALE_MAIL:
	    case RUBY_DRAGON_SCALES:
		return PM_RUBY_DRAGON;
	    case GREEN_DRAGON_SCALE_MAIL:
	    case GREEN_DRAGON_SCALES:
		return PM_GREEN_DRAGON;
	    case GOLDEN_DRAGON_SCALE_MAIL:
	    case GOLDEN_DRAGON_SCALES:
		return PM_GOLDEN_DRAGON;
	    case FEMINISM_DRAGON_SCALE_MAIL:
	    case FEMINISM_DRAGON_SCALES:
		return PM_FEMINISM_DRAGON;
	    case MAIDRAGON_SCALE_MAIL:
	    case MAIDRAGON_SCALES:
		return PM_MAIDRAGON;
	    case CANCEL_DRAGON_SCALE_MAIL:
	    case CANCEL_DRAGON_SCALES:
		return PM_CANCEL_DRAGON;
	    case NEGATIVE_DRAGON_SCALE_MAIL:
	    case NEGATIVE_DRAGON_SCALES:
		return PM_NEGATIVE_DRAGON;
	    case CORONA_DRAGON_SCALE_MAIL:
	    case CORONA_DRAGON_SCALES:
		return PM_CORONA_DRAGON;
	    case CONTRO_DRAGON_SCALE_MAIL:
	    case CONTRO_DRAGON_SCALES:
		return PM_CONTRO_DRAGON;
	    case NOPE_DRAGON_SCALE_MAIL:
	    case NOPE_DRAGON_SCALES:
		return PM_NOPE_DRAGON;
	    case CRYSTALLINE_DRAGON_SCALE_MAIL:
	    case CRYSTALLINE_DRAGON_SCALES:
		return PM_CRYSTALLINE_DRAGON;
	    case MYSTERY_DRAGON_SCALE_MAIL:
	    case MYSTERY_DRAGON_SCALES:
		return PM_MYSTERY_DRAGON;
	    case HEROIC_DRAGON_SCALE_MAIL:
	    case HEROIC_DRAGON_SCALES:
		return PM_HEROIC_DRAGON;
	    case STONE_DRAGON_SCALE_MAIL:
	    case STONE_DRAGON_SCALES:
		return PM_STONE_DRAGON;
	    case CYAN_DRAGON_SCALE_MAIL:
	    case CYAN_DRAGON_SCALES:
		return PM_CYAN_DRAGON;
	    case PSYCHIC_DRAGON_SCALE_MAIL:
	    case PSYCHIC_DRAGON_SCALES:
		return PM_PSYCHIC_DRAGON;
	    case YELLOW_DRAGON_SCALE_MAIL:
	    case YELLOW_DRAGON_SCALES:
		return PM_YELLOW_DRAGON;
	    default:
		return -1;
	}
}

static struct {
    int mon;
    int reqtime;
    boolean merge;
} draconic;

STATIC_PTR
int
mage_transform()	/* called each move during transformation process */
{
    if (--draconic.reqtime)
	return 1;
    if (draconic.merge)
	merge_with_armor();
    polymon(draconic.mon);

	/* put it on a timer, because it was just silly that you'd be able to shake off cursed items indefinitely. --Amy */

    u.dragonpolymorphtime = rnz(2000);
    pline("You have to wait %d turns until you can use the ability to polymorph into a dragon again.", u.dragonpolymorphtime);

	/* If you need to get rid of cursed stuff more often than once every rnz(2000) turns, you should maybe consider
	 * changing your playstyle and not putting every unidentified piece of equipment on :P */

    return 0;
}

int
polyatwill()      /* Polymorph under conscious control (#youpoly) */
{
#define EN_DOPP 	20 	/* This is the "base cost" for a polymorph
				 * Actual cost is this base cost + 5 * monster level
				 * of the final form you actually assume.
				 * Energy will be taken first, then you will get 
				 * more hungry if you do not have enough energy.
				 */
#define EN_BABY_DRAGON 	20
#define EN_ADULT_DRAGON 50

	int squeakamount = 0;
	int somanymana = 10;

	boolean scales = ((uarm && uarm->otyp == RED_DRAGON_SCALES
				&& Role_if(PM_FLAME_MAGE)) ||
				(uarm && uarm->otyp == BLUE_DRAGON_SCALES
				&& Role_if(PM_ELECTRIC_MAGE)) ||
				(uarm && uarm->otyp == GREEN_DRAGON_SCALES
				&& Role_if(PM_POISON_MAGE)) ||
				(uarm && uarm->otyp == YELLOW_DRAGON_SCALES
				&& Role_if(PM_ACID_MAGE)) ||
			  (uarm && uarm->otyp == WHITE_DRAGON_SCALES
				&& Role_if(PM_ICE_MAGE)));
	boolean scale_mail = ((uarm && uarm->otyp == RED_DRAGON_SCALE_MAIL   
				&& Role_if(PM_FLAME_MAGE)) ||
				(uarm && uarm->otyp == BLUE_DRAGON_SCALE_MAIL   
				&& Role_if(PM_ELECTRIC_MAGE)) ||
				(uarm && uarm->otyp == GREEN_DRAGON_SCALE_MAIL   
				&& Role_if(PM_POISON_MAGE)) ||
				(uarm && uarm->otyp == YELLOW_DRAGON_SCALE_MAIL   
				&& Role_if(PM_ACID_MAGE)) ||
			  (uarm && uarm->otyp == WHITE_DRAGON_SCALE_MAIL 
				&& Role_if(PM_ICE_MAGE)));

	/* KMH, balance patch -- new intrinsic */
	if (Unchanging) {
	    pline("You cannot change your form.");
	    return 0;
	}

	/* First, if in correct polymorphed form, rehumanize (for free) 
	 * Omit Lycanthropes,  who need to spend energy to change back and forth
	 * Amy edit: yeah sure, for free. You probably do not believe. Of course you need to pay for that too!
	 */
	if (Upolyd && !u.dragonpolymorphtime && !Race_if(PM_UNGENOMOLD) && (Race_if(PM_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) ||
			(Role_if(PM_FLAME_MAGE) && (u.umonnum == PM_RED_DRAGON || 
				u.umonnum == PM_BABY_RED_DRAGON)) ||
			(Role_if(PM_ACID_MAGE) && (u.umonnum == PM_YELLOW_DRAGON || 
				u.umonnum == PM_BABY_YELLOW_DRAGON)) ||
			(Role_if(PM_ELECTRIC_MAGE) && (u.umonnum == PM_BLUE_DRAGON || 
				u.umonnum == PM_BABY_BLUE_DRAGON)) ||
			(Role_if(PM_POISON_MAGE) && (u.umonnum == PM_GREEN_DRAGON || 
				u.umonnum == PM_BABY_GREEN_DRAGON)) ||
			(Role_if(PM_ICE_MAGE) && (u.umonnum == PM_WHITE_DRAGON || 
				u.umonnum == PM_BABY_WHITE_DRAGON)))) {
		if (yn("Turn back into your normal form?") == 'y') {

			u.dragonpolymorphtime = rnz(2000);
			pline("You have to wait %d turns until you can use the ability to polymorph into a dragon again.", u.dragonpolymorphtime);
			rehumanize();
			return 1;	    

		}
	}

	if ((Role_if(PM_ICE_MAGE) || Role_if(PM_FLAME_MAGE) || Role_if(PM_ACID_MAGE) || Role_if(PM_ELECTRIC_MAGE) || Role_if(PM_POISON_MAGE)) && !u.dragonpolymorphtime && (u.ulevel > 6 || scale_mail) && (yn("Transform into your draconic form?") == 'y')) {
	    /* [ALI]
	     * I've rewritten the logic here to fix the failure messages,
	     * but the requirements for polymorphing into the two dragon
	     * forms remains the same:
	     *
	     * Polymorph into adult dragon form if one of:
	     *
	     * - Wearing scale mail (no charge).
	     * - Wearing scales and experience level 7 and
	     *   energy level 11 or more (charge is 10).
	     * - Not wearing scales or scale mail and experience level 14 and
	     *   energy level 16 or more (charge is 15).
	     *
	     * Polymorph into baby dragon form if one of:
	     *
	     * - Wearing scales and experience level 7 and
	     *   energy level 10 or less (no charge).
	     * - Not wearing scales or scale mail and experience level 14 and
	     *   energy level 11-15 (charge is 10).
	     * - Not wearing scales or scale mail and experience level 7-13 and
	     *   energy level 11 or more (charge is 10).
	     *
	     * Fail if one of:
	     *
	     * - Not wearing scales or scale mail and experience level 7 and
	     *   energy level 10 or less (not enough energy).
	     * - Not wearing scale mail and experience level 6 or less
	     *   (not experienced enough).
	     *
	     * The transformation takes a few turns. If interrupted during this
	     * period then the ritual must be begun again from the beginning.
	     * We deliberately don't say what form the ritual takes since it
	     * is unaffected by blindness, confusion, stun etc. 
	     */
	    /*if (yn("Transform into your draconic form?") == 'n') 
		return 0;
	    else*/ if (!scales && !scale_mail && u.uen <= EN_BABY_DRAGON) {
		You("don't have the energy to polymorph. You need at least %d!",EN_BABY_DRAGON);
		return 0;		
	    } else {
		/* Check if you can do the adult form */
		if (u.ulevel > 13 && u.uen > EN_ADULT_DRAGON || 
			scales && u.uen > EN_BABY_DRAGON || scale_mail) {
		    /* If you have scales, energy cost is less */
		    /* If you have scale mail,  there is no cost! */
		    if (!scale_mail) {
			if (scales) u.uen -= EN_BABY_DRAGON; 
			else u.uen -= EN_ADULT_DRAGON;
		    }

		    draconic.mon = Role_if(PM_ACID_MAGE) ? PM_YELLOW_DRAGON : Role_if(PM_FLAME_MAGE) ?
			    PM_RED_DRAGON : Role_if(PM_ICE_MAGE) ? PM_WHITE_DRAGON : Role_if(PM_POISON_MAGE) ? PM_GREEN_DRAGON : PM_BLUE_DRAGON;
		    draconic.merge = scales || scale_mail;
		/* Otherwise use the baby form */
		} else {
		    if (!scales) u.uen -= EN_BABY_DRAGON;

		    draconic.mon = Role_if(PM_ACID_MAGE) ? PM_BABY_YELLOW_DRAGON : Role_if(PM_FLAME_MAGE) ?
			    PM_BABY_RED_DRAGON : Role_if(PM_ICE_MAGE) ? PM_BABY_WHITE_DRAGON : Role_if(PM_POISON_MAGE) ? PM_BABY_GREEN_DRAGON : PM_BABY_BLUE_DRAGON;
		    draconic.merge = scales;
		}
		draconic.reqtime = 2;
		if (mvitals[draconic.mon].mvflags & G_GENOD)
		    draconic.merge = FALSE;
		set_occupation(mage_transform,
			"transforming into your draconic form", 0);
		You("begin the transformation ritual.");
		return 1;
	    }
	}
	/* Moulds and ungenomolds _must_ be able to polymorph at will. Otherwise they would just suck. --Amy */

	if (Race_if(PM_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) || Race_if(PM_HEMI_DOPPELGANGER) || Race_if(PM_MOULD) || Race_if(PM_DEATHMOLD) || Race_if(PM_UNGENOMOLD) || Race_if(PM_WARPER)) {

	    if (!u.youpolyamount) {
		pline("You have no free polymorphs left!");
		return 0;
	    }

	    pline("You have %d free polymorphs left.", u.youpolyamount);

	    if (yn("Polymorph at will?") == 'n')	    
		return 0;
	    else if (u.uen < EN_DOPP) {
		You("don't have the energy to polymorph! You need at least %d!",EN_DOPP);
		return 0;
	    } else {

		if (!(uwep && uwep->oartifact == ART_SCHWILILILILI_MORPH && rn2(2))) u.youpolyamount--;
		u.uen -= EN_DOPP;
		if (multi >= 0) {
		    if (occupation) stop_occupation();
		    else nomul(0, 0, FALSE);
		}
		polyself(FALSE);
		if (Upolyd) { /* You actually polymorphed */
		    u.uen -= 5 * mons[u.umonnum].mlevel;
		    if (u.uen < 0) {
			morehungry(-u.uen);
			u.uen = 0;
		    }
		}
		return 1;	
	    }
	} else if ( (Race_if(PM_HUMAN_WEREWOLF) || Race_if(PM_AK_THIEF_IS_DEAD_) || Role_if(PM_LUNATIC) ) && !u.werepolymorphtime &&
		(!Upolyd || u.umonnum == u.ulycn)) {

		if (u.ulycn == PM_WERESOLDIERANT) somanymana = 15;
		if (u.ulycn == PM_WEREWOLF) somanymana = 20;
		if (u.ulycn == PM_WEREPIRANHA) somanymana = 20;
		if (u.ulycn == PM_WEREEEL) somanymana = 25;
		if (u.ulycn == PM_WEREKRAKEN) somanymana = 45;
		if (u.ulycn == PM_WEREFLYFISH) somanymana = 45;
		if (u.ulycn == PM_WEREPANTHER) somanymana = 30;
		if (u.ulycn == PM_WERETIGER) somanymana = 30;
		if (u.ulycn == PM_WERESNAKE) somanymana = 20;
		if (u.ulycn == PM_WERECOW) somanymana = 20;
		if (u.ulycn == PM_WEREBEAR) somanymana = 75;
		if (u.ulycn == PM_WEREPHANT) somanymana = 75;
		if (u.ulycn == PM_WEREVORTEX) somanymana = 50;
		if (u.ulycn == PM_WERETROLL) somanymana = 50;
		if (u.ulycn == PM_WEREGIANT) somanymana = 50;
		if (u.ulycn == PM_WEREGHOST) somanymana = 30;
		if (u.ulycn == PM_WERECOCKATRICE) somanymana = 60;
		if (u.ulycn == PM_WERELICH) somanymana = 100;
		if (u.ulycn == PM_UN_IN_PROTECT_MODE) somanymana = 100;
		if (u.ulycn == PM_WEREDEMON) somanymana = 100;
		if (u.ulycn == PM_WEREMINDFLAYER) somanymana = 150;
		if (u.ulycn == PM_VORPAL_WERE_ALHOONTRICE_ZOMBIE) somanymana = 150;
		if (u.ulycn == PM_WEREJABBERWOCK) somanymana = 200;
		if (u.ulycn == PM_WEREWEDGESANDAL) somanymana = 80;
		if (u.ulycn == PM_WEREHUGGINGBOOT) somanymana = 120;
		if (u.ulycn == PM_WEREPEEPTOE) somanymana = 140;
		if (u.ulycn == PM_WERESEXYLEATHERPUMP) somanymana = 160;
		if (u.ulycn == PM_WEREBLOCKHEELEDCOMBATBOOT) somanymana = 160;
		if (u.ulycn == PM_WERECOMBATSTILETTO) somanymana = 200;
		if (u.ulycn == PM_WEREBEAUTIFULFUNNELHEELEDPUMP) somanymana = 240;
		if (u.ulycn == PM_WEREPROSTITUTESHOE) somanymana = 240;
		if (u.ulycn == PM_WERESTILETTOSANDAL) somanymana = 250;
		if (u.ulycn == PM_WEREUNFAIRSTILETTO) somanymana = 260;
		if (u.ulycn == PM_WEREWINTERSTILETTO) somanymana = 300;

		squeakamount = somanymana;
		/* we can now use the squeaking skill to reduce the cost */

		if (!PlayerCannotUseSkills && somanymana > 2) {
			switch (P_SKILL(P_SQUEAKING)) {
		      	case P_BASIC:	somanymana *= 9; somanymana /= 10; break;
		      	case P_SKILLED:	somanymana *= 8; somanymana /= 10; break;
		      	case P_EXPERT:	somanymana *= 7; somanymana /= 10; break;
		      	case P_MASTER:	somanymana *= 6; somanymana /= 10; break;
		      	case P_GRAND_MASTER:	somanymana *= 5; somanymana /= 10; break;
		      	case P_SUPREME_MASTER:	somanymana *= 4; somanymana /= 10; break;
		      	default: break;
			}
		}

	    if (yn("Change lycanthropic form?") == 'n')
		return 0;
	    else if (u.ulycn == NON_PM) {
	    	/* Very serious */
	    	You("are no longer a lycanthrope!");
	    } else if (u.ulevel <= 2) {
	    	You("can't invoke the change at will yet.");
		return 0;		
	    } else if (u.uen < somanymana) {
		You("don't have the energy to change form! You need at least %d!", somanymana);
		return 0;
	    } else {
	    	/* Committed to the change now */
		u.uen -= somanymana;
		u.werepolymorphtime = rnz(2000);
		pline("You have to wait %d turns until you can use the ability to polymorph into a werecreature again.", u.werepolymorphtime);
		if (!Upolyd) {
		    if (multi >= 0) {
			if (occupation) stop_occupation();
			else nomul(0, 0, FALSE);
		    }
		    you_were();
		} else {
	    	    if (!Race_if(PM_UNGENOMOLD)) rehumanize();
			else polyself(FALSE);
		}

		while (squeakamount > 40) {
			use_skill(P_SQUEAKING, 1);
			squeakamount -= 40;
		}
		use_skill(P_SQUEAKING, 1);

		return 1;
	    }
	} else {
	    pline("You can't polymorph at will%s.", 
		    ((Role_if(PM_FLAME_MAGE) || Role_if(PM_ELECTRIC_MAGE) || Role_if(PM_POISON_MAGE) || Role_if(PM_LUNATIC) || Role_if(PM_ACID_MAGE) || Role_if(PM_ICE_MAGE) || Race_if(PM_HUMAN_WEREWOLF) || Race_if(PM_AK_THIEF_IS_DEAD_) || Race_if(PM_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) || Race_if(PM_HEMI_DOPPELGANGER)) ?
		    " yet" : ""));
	    return 0;
	}

	flags.botl = 1;
	return 1;
}

/* doing stuff like putting on armor when you can't, or opening a door without hands, should have a bigger chance
 * of success if you're highly skilled at polymorphing. This function is used in many places --Amy */
boolean
polyskillchance()
{
	register int percentualchance = 0;
	register int enchantplant = 0;

	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_POLYMORPHING)) {

	      	case P_BASIC:	percentualchance = 15; break;
	      	case P_SKILLED:	percentualchance = 30; break;
	      	case P_EXPERT:	percentualchance = 45; break;
	      	case P_MASTER:	percentualchance = 60; break;
	      	case P_GRAND_MASTER:	percentualchance = 75; break;
	      	case P_SUPREME_MASTER:	percentualchance = 90; break;
	      	default: percentualchance = 0; break;
		}
	}

	if (rn2(100) < percentualchance) return TRUE;

	else if (powerfulimplants() && uimplant && objects[uimplant->otyp].oc_charged && uimplant->spe > 0 ) {
		enchantplant = uimplant->spe;
		while (enchantplant > 0) {
			if (!rn2(10)) return TRUE;
			enchantplant--;
			if (enchantplant < 0) enchantplant = 0; /* fail safe, should never happen */
		}
	}

	/* if we get here, always return false because you didn't succeed on any of the "can perform the action anyway" checks */
	return FALSE;
}

/* are you eligible for the "handless" effects of implants? If you don't have hands and aren't a transformer, yes.
 * But some other races are handicapped enough that I decide they deserve this bonus too. --Amy */
boolean
powerfulimplants()
{
	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) return TRUE;
	if (Race_if(PM_WEAPON_BUG) && !Upolyd) return TRUE; /* the movement restriction is a big handicap */
	if (Race_if(PM_JELLY) && !Upolyd) return TRUE; /* permablind and can't pick up items */
	if (Race_if(PM_OCTOPODE)) return TRUE; /* can't wear armor, even while polymorphed! */
	if (Race_if(PM_SATRE) && !Upolyd) return TRUE; /* equipment restrictions */
	if (Race_if(PM_ELONA_SNAIL) && !Upolyd) return TRUE; /* equipment restrictions */
	if (Race_if(PM_HUMAN_WRAITH)) return TRUE; /* loses maxHP permanently when equipping stuff */
	if (Race_if(PM_PLAYER_GLORKUM)) return TRUE; /* is so weird that it always gets the bonus */
	if (tech_inuse(T_IMPLANTED_SYMBIOSIS) && uactivesymbiosis && uimplant) return TRUE;
	if (uarm && uarm->oartifact == ART_KUSE_MUSE) return TRUE;

	return FALSE;
}

/* are you eligible for getting your symbiote's resistances? depends on your symbiosis skill level --Amy
 * this function ensures that your symbiote exists, otherwise it returns FALSE */
boolean
powerfulsymbiosis()
{
	if (!uactivesymbiosis) return FALSE;

	if (!PlayerCannotUseSkills) {
		if (P_SKILL(P_SYMBIOSIS) >= P_MASTER) return TRUE;
		if (Role_if(PM_SYMBIANT) && (P_SKILL(P_SYMBIOSIS) >= P_EXPERT)) return TRUE;
	}

	return FALSE;
}

static void
merge_with_armor()
{
	/* This function does hides the armor being worn 
	 * It currently assumes that you are changing into a dragon
	 * Should check that monster being morphed into is not genocided
	 * see do_merge above for correct use
	 */
	if ((Race_if(PM_DOPPELGANGER) || Role_if(PM_SHAPESHIFTER) || Race_if(PM_HEMI_DOPPELGANGER)) && !uarm->cursed && uarmu &&
	  !uarmu->cursed && !(uarmu && uarmu->oartifact == ART_JEDE_SIZE_HIZE) ) {
	    struct obj *otmp = uarmu;
	    You("quickly remove your shirt as you start to change.");
	    setworn((struct obj *)0, otmp->owornmask & W_ARMU);
	    dropx(otmp);
	}
	You("merge with your scaly armor.");
	uskin = uarm;
	uarm = (struct obj *)0;
	/* save/restore hack */
	uskin->owornmask |= I_SPECIAL;

}

#if 0	/* What the f*** is this for? -- KMH */
static void
special_poly()
{
	char buf[BUFSZ];
	int old_light, new_light;
	int mntmp = NON_PM;
	int tries=0;

	old_light = (u.umonnum >= LOW_PM) ? emits_light(youmonst.data) : 0;
	do {
		getlin("Become what kind of monster? [type the name]", buf);
		mntmp = name_to_mon(buf);
		if (mntmp < LOW_PM)
				pline("I've never heard of such monsters.");
			/* Note:  humans are illegal as monsters, but an
			 * illegal monster forces newman(), which is what we
			 * want if they specified a human.... */

			/* [Tom] gnomes are polyok, so this doesn't apply for
			   player gnomes */
	                     /* WAC but we want to catch player gnomes and not
	                        so add an extra check */
		else if (!polyok(&mons[mntmp]) &&
				(Role_elven ? !is_elf(&mons[mntmp]) :
				Role_if(PM_DWARF) ? !is_gnome(&mons[mntmp]) :
				/* WAC
				 * should always fail (for now) gnome check
				 * unless gnomes become not polyok.  Then, it'll
				 * still work ;B
				 */
				Role_if(PM_GNOME) ? !is_gnome(&mons[mntmp]) :
				!is_human(&mons[mntmp])))
			You("cannot polymorph into that.");
		else if (!mvitals[mntmp].eaten && (rn2((u.ulevel + 25)) < 20)) {
			You("don't have the knowledge to polymorph into that.");
			return;  /* Nice try */
		} else {
			You("attempt an unfamiliar polymorph.");
			break;
		}
	} while(++tries < 5);
	if (tries==5) {
		pline(thats_enough_tries);
		return;
	} else if (polymon(mntmp)) {
		/* same as made_change above */
		new_light = (u.umonnum >= LOW_PM) ? emits_light(youmonst.data) : 0;
		if (old_light != new_light) {
		    if (old_light)
			del_light_source(LS_MONSTER, (void *)&youmonst);
		    if (new_light == 1) ++new_light;  /* otherwise it's undetectable */
		    if (new_light)
			new_light_source(u.ux, u.uy, new_light,
					 LS_MONSTER, (void *)&youmonst);
		}
	}
	return;
}
#endif


#endif /* OVLB */

/*polyself.c*/

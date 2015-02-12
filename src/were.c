/*	SCCS Id: @(#)were.c	3.4	2002/11/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef OVL0

void
were_change(mon)
register struct monst *mon;
{
	if (!is_were(mon->data))
	    return;

	if (is_human(mon->data)) {
	    if (!Protection_from_shape_changers &&
			!rn2(night() ? (flags.moonphase == FULL_MOON ?  3 : 30)
				     : (flags.moonphase == FULL_MOON ? 10 : 50))) {
		new_were(mon);		/* change into animal form */
		if (flags.soundok && !canseemon(mon)) {
		    const char *howler;

		    switch (monsndx(mon->data)) {
		    case PM_HUMAN_WEREWOLF:	howler = "wolf";    break;
		    case PM_HUMAN_WEREJACKAL:	howler = "jackal";  break;
		    case PM_HUMAN_WEREPANTHER:	howler = "panther"; break;
		    case PM_HUMAN_WERETIGER:	howler = "tiger";   break;
		    default:			howler = (char *)0; break;
		    }
		    if (howler)
			You_hear("a %s howling at the moon.", howler);
		}
	    }
	} else if (!rn2(30) || Protection_from_shape_changers) {
	    new_were(mon);		/* change back into human form */
	}
}

#endif /* OVL0 */
#ifdef OVLB

int
counter_were(pm)
int pm;
{
	switch(pm) {
	    case PM_WEREWOLF:	      return(PM_HUMAN_WEREWOLF);
	    case PM_HUMAN_WEREWOLF:   return(PM_WEREWOLF);
	    case PM_WEREJACKAL:	      return(PM_HUMAN_WEREJACKAL);
	    case PM_HUMAN_WEREJACKAL: return(PM_WEREJACKAL);
	    case PM_WERERAT:	      return(PM_HUMAN_WERERAT);
	    case PM_HUMAN_WERERAT:    return(PM_WERERAT);
	    case PM_WEREPANTHER:      return(PM_HUMAN_WEREPANTHER);            
	    case PM_HUMAN_WEREPANTHER:return(PM_WEREPANTHER);
	    case PM_WERETIGER:        return(PM_HUMAN_WERETIGER);
	    case PM_HUMAN_WERETIGER:  return(PM_WERETIGER);
	    case PM_WERESNAKE:        return(PM_HUMAN_WERESNAKE);
	    case PM_HUMAN_WERESNAKE:  return(PM_WERESNAKE);
	    case PM_WERESPIDER:       return(PM_HUMAN_WERESPIDER);
	    case PM_HUMAN_WERESPIDER: return(PM_WERESPIDER);
	    case PM_WERELICHEN:       return(PM_HUMAN_WERELICHEN);
	    case PM_HUMAN_WERELICHEN: return(PM_WERELICHEN);
	    case PM_WEREVORTEX:       return(PM_HUMAN_WEREVORTEX);
	    case PM_HUMAN_WEREVORTEX: return(PM_WEREVORTEX);
	    case PM_WEREPIERCER:       return(PM_HUMAN_WEREPIERCER);
	    case PM_HUMAN_WEREPIERCER: return(PM_WEREPIERCER);
	    case PM_WEREPENETRATOR:       return(PM_HUMAN_WEREPENETRATOR);
	    case PM_HUMAN_WEREPENETRATOR: return(PM_WEREPENETRATOR);
	    case PM_WERESMASHER:       return(PM_HUMAN_WERESMASHER);
	    case PM_HUMAN_WERESMASHER: return(PM_WERESMASHER);
	    case PM_WERENYMPH:       return(PM_HUMAN_WERENYMPH);
	    case PM_HUMAN_WERENYMPH: return(PM_WERENYMPH);
	    case PM_WERECOW:       return(PM_HUMAN_WERECOW);
	    case PM_HUMAN_WERECOW: return(PM_WERECOW);
	    case PM_WEREBEAR:       return(PM_HUMAN_WEREBEAR);
	    case PM_HUMAN_WEREBEAR: return(PM_WEREBEAR);
	    case PM_WERESOLDIERANT:       return(PM_HUMAN_WERESOLDIERANT);
	    case PM_HUMAN_WERESOLDIERANT: return(PM_WERESOLDIERANT);
	    case PM_WERECOCKATRICE:       return(PM_HUMAN_WERECOCKATRICE);
	    case PM_HUMAN_WERECOCKATRICE: return(PM_WERECOCKATRICE);
	    case PM_WEREMINDFLAYER:       return(PM_HUMAN_WEREMINDFLAYER);
	    case PM_HUMAN_WEREMINDFLAYER: return(PM_WEREMINDFLAYER);
	    case PM_WEREMIMIC:       return(PM_HUMAN_WEREMIMIC);
	    case PM_HUMAN_WEREMIMIC: return(PM_WEREMIMIC);
	    case PM_WEREGIANT:        return(PM_HUMAN_WEREGIANT);
	    case PM_HUMAN_WEREGIANT:  return(PM_WEREGIANT);
	    case PM_WEREGHOST:        return(PM_HUMAN_WEREGHOST);
	    case PM_HUMAN_WEREGHOST:  return(PM_WEREGHOST);
	    case PM_WEREPIRANHA:       return(PM_HUMAN_WEREPIRANHA);
	    case PM_HUMAN_WEREPIRANHA: return(PM_WEREPIRANHA);
	    case PM_WEREEEL:       return(PM_HUMAN_WEREEEL);
	    case PM_HUMAN_WEREEEL: return(PM_WEREEEL);
	    case PM_WEREFLYFISH:       return(PM_HUMAN_WEREFLYFISH);
	    case PM_HUMAN_WEREFLYFISH: return(PM_WEREFLYFISH);
	    case PM_WEREKRAKEN:       return(PM_HUMAN_WEREKRAKEN);
	    case PM_HUMAN_WEREKRAKEN: return(PM_WEREKRAKEN);
	    default:		      return(0);
	}
}

void
new_were(mon)
register struct monst *mon;
{
	register int pm;

	pm = counter_were(monsndx(mon->data));
	if(!pm) {
	    impossible("unknown lycanthrope %s.", mon->data->mname);
	    return;
	}

	if(canseemon(mon) && !Hallucination)
	    pline("%s changes into a %s.", Monnam(mon),
			is_human(&mons[pm]) ? "human" :
			mons[pm].mname+4);

	set_mon_data(mon, &mons[pm], 0);
	if (mon->msleeping || !mon->mcanmove) {
	    /* transformation wakens and/or revitalizes */
	    mon->msleeping = 0;
	    mon->mfrozen = 0;	/* not asleep or paralyzed */
	    mon->mcanmove = 1;
	}
	/* regenerate by 1/4 of the lost hit points */
	mon->mhp += (mon->mhpmax - mon->mhp) / 4;
	newsym(mon->mx,mon->my);
	mon_break_armor(mon, FALSE);
	possibly_unwield(mon, FALSE);
	(void) stop_timer(UNPOLY_MON, (genericptr_t) mon);
	(void) start_timer(rn1(1000,1000), TIMER_MONSTER,
		UNPOLY_MON, (genericptr_t) mon);
}

int
were_summon(ptr,yours,visible,genbuf)	/* were-creature (even you) summons a horde */
register struct permonst *ptr;
register boolean yours;
int *visible;			/* number of visible helpers created */
char *genbuf;
{
	register int i, typ, pm = monsndx(ptr);
	register struct monst *mtmp;
	int total = 0;

	*visible = 0;
	if(Protection_from_shape_changers && !yours)
		return 0;
	/*
	 * Allow lycanthropes in normal form to summon hordes as well.  --ALI
	 */
	if (pm == PM_PLAYERMON)
	    pm = urace.malenum;
	for(i = rnd(2); i > 0; i--) {
	   switch(pm) {

		case PM_WERERAT:
		case PM_HUMAN_WERERAT:
			typ = rn2(3) ? PM_SEWER_RAT : rn2(3) ? PM_GIANT_RAT : PM_RABID_RAT ;
			if (genbuf) Strcpy(genbuf, "rat");
			break;
		case PM_WEREJACKAL:
		case PM_HUMAN_WEREJACKAL:
			typ = PM_JACKAL;
			if (genbuf) Strcpy(genbuf, "jackal");
			break;
		case PM_WEREWOLF:
		case PM_HUMAN_WEREWOLF:
			typ = rn2(5) ? PM_WOLF : PM_WINTER_WOLF ;
			if (genbuf) Strcpy(genbuf, "wolf");
			break;
		case PM_WEREPANTHER:
		case PM_HUMAN_WEREPANTHER:
			typ = rn2(5) ? PM_JAGUAR : PM_PANTHER ;
			if (genbuf) Strcpy(genbuf, "large cat");
			break;
		case PM_WERETIGER:
		case PM_HUMAN_WERETIGER:
			typ = rn2(5) ? PM_JAGUAR : PM_TIGER ;
			if (genbuf) Strcpy(genbuf, "large cat");
			break;
		case PM_WERESNAKE:
		case PM_HUMAN_WERESNAKE:
			typ = rn2(5) ? PM_SNAKE : PM_PIT_VIPER ;
			if (genbuf) Strcpy(genbuf, "snake");
			break;
		case PM_WERESPIDER:
		case PM_HUMAN_WERESPIDER:
			typ = rn2(5) ? PM_CAVE_SPIDER : PM_RECLUSE_SPIDER ;
			if (genbuf) Strcpy(genbuf, "spider");
			break;
		case PM_WEREGHOST:
		case PM_HUMAN_WEREGHOST:
			typ = rn2(5) ? PM_GHOST : PM_BOO ;
			if (genbuf) Strcpy(genbuf, "spiritual");
			break;
		case PM_WEREGIANT:
		case PM_HUMAN_WEREGIANT:
			typ = rn2(5) ? PM_GIANT : PM_HILL_GIANT ;
			if (genbuf) Strcpy(genbuf, "giant");
			break;
		case PM_WERELICHEN:
		case PM_HUMAN_WERELICHEN:
			typ = rn2(5) ? PM_AGGRESSIVE_LICHEN : PM_POISON_LICHEN ;
			if (genbuf) Strcpy(genbuf, "fungus");
			break;
		case PM_WEREVORTEX:
		case PM_HUMAN_WEREVORTEX:
			typ = rn2(5) ? PM_ENERGY_VORTEX : PM_STEAM_VORTEX ;
			if (genbuf) Strcpy(genbuf, "vortex");
			break;
		case PM_WERECOW:
		case PM_HUMAN_WERECOW:
			typ = rn2(5) ? PM_COW : PM_BULL ;
			if (genbuf) Strcpy(genbuf, "bovine");
			break;
		case PM_WEREBEAR:
		case PM_HUMAN_WEREBEAR:
			typ = rn2(5) ? PM_BROWN_BEAR : PM_CAVE_BEAR ;
			if (genbuf) Strcpy(genbuf, "ursa major");
			break;
		case PM_WEREPIERCER:
		case PM_HUMAN_WEREPIERCER:
			typ = rn2(5) ? PM_ROCK_PIERCER : PM_IRON_PIERCER ;
			if (genbuf) Strcpy(genbuf, "piercer");
			break;
		case PM_WEREPENETRATOR:
		case PM_HUMAN_WEREPENETRATOR:
			typ = rn2(5) ? PM_ROCK_PENETRATOR : PM_IRON_PENETRATOR ;
			if (genbuf) Strcpy(genbuf, "penetrator");
			break;
		case PM_WERESMASHER:
		case PM_HUMAN_WERESMASHER:
			typ = rn2(5) ? PM_ROCK_SMASHER : PM_IRON_SMASHER ;
			if (genbuf) Strcpy(genbuf, "smasher");
			break;
		case PM_WERENYMPH:
		case PM_HUMAN_WERENYMPH:
			typ = rn2(5) ? PM_WOOD_NYMPH : PM_WATER_NYMPH ;
			if (genbuf) Strcpy(genbuf, "nymph");
			break;
		case PM_WEREMIMIC:
		case PM_HUMAN_WEREMIMIC:
			typ = rn2(5) ? PM_LARGE_MIMIC : PM_GIANT_MIMIC ;
			if (genbuf) Strcpy(genbuf, "mimic");
			break;
		case PM_WEREMINDFLAYER:
		case PM_HUMAN_WEREMINDFLAYER:
			typ = rn2(5) ? PM_MIND_FLAYER : PM_MASTER_MIND_FLAYER ;
			if (genbuf) Strcpy(genbuf, "mind flayer");
			break;
		case PM_WERECOCKATRICE:
		case PM_HUMAN_WERECOCKATRICE:
			typ = rn2(5) ? PM_CHICKATRICE : PM_COCKATRICE ;
			if (genbuf) Strcpy(genbuf, "cockatrice");
			break;
		case PM_WERESOLDIERANT:
		case PM_HUMAN_WERESOLDIERANT:
			typ = rn2(5) ? PM_SOLDIER_ANT : PM_GIANT_RED_ANT ;
			if (genbuf) Strcpy(genbuf, "ant");
			break;
		case PM_WEREPIRANHA:
		case PM_HUMAN_WEREPIRANHA:
			typ = rn2(5) ? PM_PIRANHA : PM_BLOODTHIRSTY_PIRANHA ;
			if (genbuf) Strcpy(genbuf, "piranha");
			break;
		case PM_WEREEEL:
		case PM_HUMAN_WEREEEL:
			typ = rn2(5) ? PM_GIANT_EEL : PM_ELECTRIC_EEL ;
			if (genbuf) Strcpy(genbuf, "eel");
			break;
		case PM_WEREFLYFISH:
		case PM_HUMAN_WEREFLYFISH:
			typ = rn2(5) ? PM_FLYING_RIBBON_FISH : PM_FLYING_SKELETON_FISH ;
			if (genbuf) Strcpy(genbuf, "flying fish");
			break;
		case PM_WEREKRAKEN:
		case PM_HUMAN_WEREKRAKEN:
			typ = rn2(5) ? PM_KRAKEN : PM_HENTAI_OCTOPUS ;
			if (genbuf) Strcpy(genbuf, "kraken");
			break;
		default:
			continue;
	    }
	    mtmp = makemon(&mons[typ], u.ux, u.uy, NO_MM_FLAGS);
	    if (mtmp) {
		total++;
		if (canseemon(mtmp)) *visible += 1;
	    }
	    if (yours && mtmp)
		(void) tamedog(mtmp, (struct obj *) 0, TRUE);
	}
	return total;
}

void
you_were()
{
	char qbuf[QBUFSZ];

	if (Unchanging || (u.umonnum == u.ulycn)) return;
	if (Polymorph_control && rn2(5) ) {
	    /* `+4' => skip "were" prefix to get name of beast */
	    Sprintf(qbuf, "Do you want to change into %s? ",
		    an(mons[u.ulycn].mname+4));
	    if(yn(qbuf) == 'n') return;
	}
	(void) polymon(u.ulycn);
}

void
you_unwere(purify)
boolean purify;
{
	boolean in_wereform = (u.umonnum == u.ulycn);

	if (purify) {
	    if (Race_if(PM_HUMAN_WEREWOLF) || Role_if(PM_LUNATIC) || Role_if(PM_AK_THIEF_IS_DEAD_) ) {
		/* An attempt to purify you has been made! */
		if (in_wereform && Unchanging) {
		    killer_format = NO_KILLER_PREFIX;
		    killer = "purified while stuck in creature form";
		    pline_The("purification was deadly...");
		    done(DIED);
		} else {
		    You_feel("very bad!");
		    if (in_wereform)
			rehumanize();
		    (void) adjattrib(A_STR, -rn1(3,3), 2);
		    (void) adjattrib(A_CON, -rn1(3,3), 1);
		    losehp(u.uhp - (u.uhp > 10 ? rnd(5) : 1), "purification",
			    KILLED_BY);
		}
		return;
	    }
	    You_feel("purified.");
	    u.ulycn = NON_PM;	/* cure lycanthropy */
	    upermonst.mflags2 &= ~M2_WERE;
	}
	if (!Unchanging && in_wereform &&
		(!Polymorph_control || yn("Remain in beast form?") == 'n'))
	    rehumanize();
}

#endif /* OVLB */

/*were.c*/

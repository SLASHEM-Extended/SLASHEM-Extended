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
		    if (howler) {
			You_hear("a %s howling at the moon.", howler);
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy stanete odnim iz nikh i kopirovat' vashi dospekhi v kloch'ya, potomu chto vy takoy lokh i, veroyatno, ne znayut o volshebnoy otmene." : "WOEAAAAAAAAAAAA-eaaaaaoh.");
		    }
		}
	    }
	} else if (!rn2(30) && !Protection_from_shape_changers) {
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
	    case PM_WERERABBIT:	      return(PM_HUMAN_WERERABBIT);
	    case PM_HUMAN_WERERABBIT: return(PM_WERERABBIT);
	    case PM_WEREBOAR:	      return(PM_HUMAN_WEREBOAR);
	    case PM_HUMAN_WEREBOAR:   return(PM_WEREBOAR);
	    case PM_WERELOCUST:	      return(PM_HUMAN_WERELOCUST);
	    case PM_HUMAN_WERELOCUST: return(PM_WERELOCUST);
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
	    case PM_WEREBEE:       return(PM_HUMAN_WEREBEE);
	    case PM_HUMAN_WEREBEE: return(PM_WEREBEE);
	    case PM_WERECOW:       return(PM_HUMAN_WERECOW);
	    case PM_HUMAN_WERECOW: return(PM_WERECOW);
	    case PM_WEREBEAR:       return(PM_HUMAN_WEREBEAR);
	    case PM_HUMAN_WEREBEAR: return(PM_WEREBEAR);
	    case PM_WEREBRONZEGRAM:       return(PM_HUMAN_WEREBRONZEGRAM);
	    case PM_HUMAN_WEREBRONZEGRAM: return(PM_WEREBRONZEGRAM);
	    case PM_WERECHROMEGRAM:       return(PM_HUMAN_WERECHROMEGRAM);
	    case PM_HUMAN_WERECHROMEGRAM: return(PM_WERECHROMEGRAM);
	    case PM_WEREDEMON:       return(PM_HUMAN_WEREDEMON);
	    case PM_HUMAN_WEREDEMON: return(PM_WEREDEMON);
	    case PM_WEREPHANT:       return(PM_HUMAN_WEREPHANT);
	    case PM_HUMAN_WEREPHANT: return(PM_WEREPHANT);
	    case PM_WERESOLDIERANT:       return(PM_HUMAN_WERESOLDIERANT);
	    case PM_HUMAN_WERESOLDIERANT: return(PM_WERESOLDIERANT);
	    case PM_WERETROLL:       return(PM_HUMAN_WERETROLL);
	    case PM_HUMAN_WERETROLL: return(PM_WERETROLL);
	    case PM_WEREBAT:       return(PM_HUMAN_WEREBAT);
	    case PM_HUMAN_WEREBAT: return(PM_WEREBAT);
	    case PM_WERECOCKATRICE:       return(PM_HUMAN_WERECOCKATRICE);
	    case PM_HUMAN_WERECOCKATRICE: return(PM_WERECOCKATRICE);
	    case PM_WEREMINDFLAYER:       return(PM_HUMAN_WEREMINDFLAYER);
	    case PM_HUMAN_WEREMINDFLAYER: return(PM_WEREMINDFLAYER);
	    case PM_WEREMIMIC:       return(PM_HUMAN_WEREMIMIC);
	    case PM_HUMAN_WEREMIMIC: return(PM_WEREMIMIC);
	    case PM_WEREPERMAMIMIC:       return(PM_HUMAN_WEREPERMAMIMIC);
	    case PM_HUMAN_WEREPERMAMIMIC: return(PM_WEREPERMAMIMIC);
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
	    case PM_WERELICH:       return(PM_HUMAN_WERELICH);
	    case PM_HUMAN_WERELICH: return(PM_WERELICH);
	    case PM_WEREJABBERWOCK:       return(PM_HUMAN_WEREJABBERWOCK);
	    case PM_HUMAN_WEREJABBERWOCK: return(PM_WEREJABBERWOCK);
	    case PM_WEREGRIDBUG:       return(PM_HUMAN_WEREGRIDBUG);
	    case PM_HUMAN_WEREGRIDBUG: return(PM_WEREGRIDBUG);
	    case PM_AK_THIEF_IS_DEAD_:       return(PM_UN_IN_PROTECT_MODE);
	    case PM_UN_IN_PROTECT_MODE: return(PM_AK_THIEF_IS_DEAD_);
	    case PM_WEREACIDBLOB:       return(PM_HUMAN_WEREACIDBLOB);
	    case PM_HUMAN_WEREACIDBLOB: return(PM_WEREACIDBLOB);
	    case PM_WEREFOX:       return(PM_HUMAN_WEREFOX);
	    case PM_HUMAN_WEREFOX: return(PM_WEREFOX);
	    case PM_WEREMONKEY:       return(PM_HUMAN_WEREMONKEY);
	    case PM_HUMAN_WEREMONKEY: return(PM_WEREMONKEY);
	    case PM_WEREFLOATINGEYE:       return(PM_HUMAN_WEREFLOATINGEYE);
	    case PM_HUMAN_WEREFLOATINGEYE: return(PM_WEREFLOATINGEYE);
	    case PM_WEREGIANTANT:       return(PM_HUMAN_WEREGIANTANT);
	    case PM_HUMAN_WEREGIANTANT: return(PM_WEREGIANTANT);
	    case PM_WEREKOBOLD:       return(PM_HUMAN_WEREKOBOLD);
	    case PM_HUMAN_WEREKOBOLD: return(PM_WEREKOBOLD);
	    case PM_WERELEPRECHAUN:       return(PM_HUMAN_WERELEPRECHAUN);
	    case PM_HUMAN_WERELEPRECHAUN: return(PM_WERELEPRECHAUN);
	    case PM_WEREHORSE:       return(PM_HUMAN_WEREHORSE);
	    case PM_HUMAN_WEREHORSE: return(PM_WEREHORSE);
	    case PM_WERERUSTMONSTER:       return(PM_HUMAN_WERERUSTMONSTER);
	    case PM_HUMAN_WERERUSTMONSTER: return(PM_WERERUSTMONSTER);
	    case PM_WEREBLACKLIGHT:       return(PM_HUMAN_WEREBLACKLIGHT);
	    case PM_HUMAN_WEREBLACKLIGHT: return(PM_WEREBLACKLIGHT);
	    case PM_WEREAUTON:       return(PM_HUMAN_WEREAUTON);
	    case PM_HUMAN_WEREAUTON: return(PM_WEREAUTON);
	    case PM_WEREGREMLIN:       return(PM_HUMAN_WEREGREMLIN);
	    case PM_HUMAN_WEREGREMLIN: return(PM_WEREGREMLIN);
	    case PM_WEREGREENSLIME:       return(PM_HUMAN_WEREGREENSLIME);
	    case PM_HUMAN_WEREGREENSLIME: return(PM_WEREGREENSLIME);
	    case PM_WEREJELLY:       return(PM_HUMAN_WEREJELLY);
	    case PM_HUMAN_WEREJELLY: return(PM_WEREJELLY);
	    case PM_WEREXAN:       return(PM_HUMAN_WEREXAN);
	    case PM_HUMAN_WEREXAN: return(PM_WEREXAN);
	    case PM_WEREMUMAK:       return(PM_HUMAN_WEREMUMAK);
	    case PM_HUMAN_WEREMUMAK: return(PM_WEREMUMAK);
	    case PM_WERECENTAUR:       return(PM_HUMAN_WERECENTAUR);
	    case PM_HUMAN_WERECENTAUR: return(PM_WERECENTAUR);
	    case PM_WEREGELATINOUSCUBE:       return(PM_HUMAN_WEREGELATINOUSCUBE);
	    case PM_HUMAN_WEREGELATINOUSCUBE: return(PM_WEREGELATINOUSCUBE);
	    case PM_WERECOUATL:       return(PM_HUMAN_WERECOUATL);
	    case PM_HUMAN_WERECOUATL: return(PM_WERECOUATL);
	    case PM_WEREAIRELEMENTAL:       return(PM_HUMAN_WEREAIRELEMENTAL);
	    case PM_HUMAN_WEREAIRELEMENTAL: return(PM_WEREAIRELEMENTAL);
	    case PM_WEREFIREELEMENTAL:       return(PM_HUMAN_WEREFIREELEMENTAL);
	    case PM_HUMAN_WEREFIREELEMENTAL: return(PM_WEREFIREELEMENTAL);
	    case PM_WEREEARTHELEMENTAL:       return(PM_HUMAN_WEREEARTHELEMENTAL);
	    case PM_HUMAN_WEREEARTHELEMENTAL: return(PM_WEREEARTHELEMENTAL);
	    case PM_WEREWATERELEMENTAL:       return(PM_HUMAN_WEREWATERELEMENTAL);
	    case PM_HUMAN_WEREWATERELEMENTAL: return(PM_WEREWATERELEMENTAL);
	    case PM_WEREASTRALELEMENTAL:       return(PM_HUMAN_WEREASTRALELEMENTAL);
	    case PM_HUMAN_WEREASTRALELEMENTAL: return(PM_WEREASTRALELEMENTAL);
	    case PM_WEREXORN:       return(PM_HUMAN_WEREXORN);
	    case PM_HUMAN_WEREXORN: return(PM_WEREXORN);
	    case PM_WERESTALKER:       return(PM_HUMAN_WERESTALKER);
	    case PM_HUMAN_WERESTALKER: return(PM_WERESTALKER);
	    case PM_WEREDRACONIAN:       return(PM_HUMAN_WEREDRACONIAN);
	    case PM_HUMAN_WEREDRACONIAN: return(PM_WEREDRACONIAN);
	    case PM_WEREUMBERHULK:       return(PM_HUMAN_WEREUMBERHULK);
	    case PM_HUMAN_WEREUMBERHULK: return(PM_WEREUMBERHULK);
	    case PM_WEREVAMPIRE:       return(PM_HUMAN_WEREVAMPIRE);
	    case PM_HUMAN_WEREVAMPIRE: return(PM_WEREVAMPIRE);
	    case PM_WEREKILLERBEE:       return(PM_HUMAN_WEREKILLERBEE);
	    case PM_HUMAN_WEREKILLERBEE: return(PM_WEREKILLERBEE);
	    case PM_WEREKANGAROO:       return(PM_HUMAN_WEREKANGAROO);
	    case PM_HUMAN_WEREKANGAROO: return(PM_WEREKANGAROO);
	    case PM_WEREGRUE:       return(PM_HUMAN_WEREGRUE);
	    case PM_HUMAN_WEREGRUE: return(PM_WEREGRUE);
	    case PM_WERECOINS:       return(PM_HUMAN_WERECOINS);
	    case PM_HUMAN_WERECOINS: return(PM_WERECOINS);
	    case PM_WERETRAPPER:       return(PM_HUMAN_WERETRAPPER);
	    case PM_HUMAN_WERETRAPPER: return(PM_WERETRAPPER);
	    case PM_WERESHADE:       return(PM_HUMAN_WERESHADE);
	    case PM_HUMAN_WERESHADE: return(PM_WERESHADE);
	    case PM_WEREDISENCHANTER:       return(PM_HUMAN_WEREDISENCHANTER);
	    case PM_HUMAN_WEREDISENCHANTER: return(PM_WEREDISENCHANTER);
	    case PM_WERENAGA:       return(PM_HUMAN_WERENAGA);
	    case PM_HUMAN_WERENAGA: return(PM_WERENAGA);
	    case PM_WEREWORM:       return(PM_HUMAN_WEREWORM);
	    case PM_HUMAN_WEREWORM: return(PM_WEREWORM);
	    case PM_WEREDRAGON:       return(PM_HUMAN_WEREDRAGON);
	    case PM_HUMAN_WEREDRAGON: return(PM_WEREDRAGON);
	    case PM_WEREWEDGESANDAL:       return(PM_HUMAN_WEREWEDGESANDAL);
	    case PM_HUMAN_WEREWEDGESANDAL: return(PM_WEREWEDGESANDAL);
	    case PM_WEREHUGGINGBOOT:       return(PM_HUMAN_WEREHUGGINGBOOT);
	    case PM_HUMAN_WEREHUGGINGBOOT: return(PM_WEREHUGGINGBOOT);
	    case PM_WEREPEEPTOE:       return(PM_HUMAN_WEREPEEPTOE);
	    case PM_HUMAN_WEREPEEPTOE: return(PM_WEREPEEPTOE);
	    case PM_WERESEXYLEATHERPUMP:       return(PM_HUMAN_WERESEXYLEATHERPUMP);
	    case PM_HUMAN_WERESEXYLEATHERPUMP: return(PM_WERESEXYLEATHERPUMP);
	    case PM_WEREBLOCKHEELEDCOMBATBOOT:       return(PM_HUMAN_WEREBLOCKHEELEDCOMBATBOOT);
	    case PM_HUMAN_WEREBLOCKHEELEDCOMBATBOOT: return(PM_WEREBLOCKHEELEDCOMBATBOOT);
	    case PM_WERECOMBATSTILETTO:       return(PM_HUMAN_WERECOMBATSTILETTO);
	    case PM_HUMAN_WERECOMBATSTILETTO: return(PM_WERECOMBATSTILETTO);
	    case PM_WEREBEAUTIFULFUNNELHEELEDPUMP:       return(PM_HUMAN_WEREBEAUTIFULFUNNELHEELEDPUMP);
	    case PM_HUMAN_WEREBEAUTIFULFUNNELHEELEDPUMP: return(PM_WEREBEAUTIFULFUNNELHEELEDPUMP);
	    case PM_WEREPROSTITUTESHOE:       return(PM_HUMAN_WEREPROSTITUTESHOE);
	    case PM_HUMAN_WEREPROSTITUTESHOE: return(PM_WEREPROSTITUTESHOE);
	    case PM_WERESTILETTOSANDAL:       return(PM_HUMAN_WERESTILETTOSANDAL);
	    case PM_HUMAN_WERESTILETTOSANDAL: return(PM_WERESTILETTOSANDAL);
	    case PM_WEREUNFAIRSTILETTO:       return(PM_HUMAN_WEREUNFAIRSTILETTO);
	    case PM_HUMAN_WEREUNFAIRSTILETTO: return(PM_WEREUNFAIRSTILETTO);
	    case PM_WEREWINTERSTILETTO:       return(PM_HUMAN_WEREWINTERSTILETTO);
	    case PM_HUMAN_WEREWINTERSTILETTO: return(PM_WEREWINTERSTILETTO);
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
	    impossible("unknown lycanthrope %d.", mon->mnum);
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
	    mon->masleep = 0;
	}
	/* regenerate by 1/4 of the lost hit points */
	mon->mhp += (mon->mhpmax - mon->mhp) / 4;
	newsym(mon->mx,mon->my);
	mon_break_armor(mon, FALSE);
	possibly_unwield(mon, FALSE);
	(void) stop_timer(UNPOLY_MON, (void *) mon);
	(void) start_timer(rn1(1000,1000), TIMER_MONSTER,
		UNPOLY_MON, (void *) mon);
}

int
were_summon(ptr,yours,visible,genbuf,ownloc)	/* were-creature (even you) summons a horde */
register struct permonst *ptr;
register boolean yours;
int *visible;			/* number of visible helpers created */
char *genbuf;
boolean ownloc; /* TRUE = summon them at a random location, FALSE = summon them at player's location --Amy */
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
	for(i = ((!rn2(5) && !yours) ? rnd(5) : rnd(2)); i > 0; i--) {
	   switch(pm) {

		/* no concealing monsters due to possibility of abuse --Amy */

		case PM_WERERAT:
		case PM_HUMAN_WERERAT:
			typ = rn2(3) ? PM_SEWER_RAT : rn2(3) ? PM_GIANT_RAT : PM_RABID_RAT ;
			if (genbuf) strcpy(genbuf, "rat");
			break;
		case PM_WERERABBIT:
		case PM_HUMAN_WERERABBIT:
			typ = rn2(5) ? PM_RABBIT : PM_RABID_RABBIT ;
			if (genbuf) strcpy(genbuf, "rabbit");
			break;
		case PM_WEREBOAR:
		case PM_HUMAN_WEREBOAR:
			typ = rn2(5) ? PM_BOAR : PM_DIRE_BOAR ;
			if (genbuf) strcpy(genbuf, "boar");
			break;
		case PM_WERELOCUST:
		case PM_HUMAN_WERELOCUST:
			typ = rn2(5) ? PM_SMALL_LOCUST : PM_LOCUST ;
			if (genbuf) strcpy(genbuf, "locust");
			break;
		case PM_WEREJACKAL:
		case PM_HUMAN_WEREJACKAL:
			typ = PM_JACKAL;
			if (genbuf) strcpy(genbuf, "jackal");
			break;
		case PM_WEREWOLF:
		case PM_HUMAN_WEREWOLF:
			typ = rn2(5) ? PM_WOLF : PM_WINTER_WOLF ;
			if (genbuf) strcpy(genbuf, "wolf");
			break;
		case PM_WEREPANTHER:
		case PM_HUMAN_WEREPANTHER:
			typ = rn2(5) ? PM_JAGUAR : PM_PANTHER ;
			if (genbuf) strcpy(genbuf, "large cat");
			break;
		case PM_WERETIGER:
		case PM_HUMAN_WERETIGER:
			typ = rn2(5) ? PM_JAGUAR : PM_TIGER ;
			if (genbuf) strcpy(genbuf, "large cat");
			break;
		case PM_WERESNAKE:
		case PM_HUMAN_WERESNAKE:
			typ = rn2(5) ? PM_STANDARD_SNAKE : PM_CHASM_VIPER ;
			if (genbuf) strcpy(genbuf, "snake");
			break;
		case PM_WERESPIDER:
		case PM_HUMAN_WERESPIDER:
			typ = rn2(5) ? PM_LAND_SPIDER : PM_MUD_SPIDER ;
			if (genbuf) strcpy(genbuf, "spider");
			break;
		case PM_WEREGHOST:
		case PM_HUMAN_WEREGHOST:
			typ = rn2(5) ? PM_GHOST : PM_BOO ;
			if (genbuf) strcpy(genbuf, "spiritual");
			break;
		case PM_WEREGIANT:
		case PM_HUMAN_WEREGIANT:
			typ = rn2(5) ? PM_GIANT : PM_HILL_GIANT ;
			if (genbuf) strcpy(genbuf, "giant");
			break;
		case PM_WERELICHEN:
		case PM_HUMAN_WERELICHEN:
			typ = rn2(5) ? PM_AGGRESSIVE_LICHEN : PM_POISON_LICHEN ;
			if (genbuf) strcpy(genbuf, "fungus");
			break;
		case PM_WEREVORTEX:
		case PM_HUMAN_WEREVORTEX:
			typ = rn2(5) ? PM_ENERGY_VORTEX : PM_STEAM_VORTEX ;
			if (genbuf) strcpy(genbuf, "vortex");
			break;
		case PM_WERECOW:
		case PM_HUMAN_WERECOW:
			typ = rn2(5) ? PM_COW : PM_BULL ;
			if (genbuf) strcpy(genbuf, "bovine");
			break;
		case PM_WEREBEAR:
		case PM_HUMAN_WEREBEAR:
			typ = rn2(5) ? PM_BROWN_BEAR : PM_CAVE_BEAR ;
			if (genbuf) strcpy(genbuf, "ursa major");
			break;
		case PM_WEREBRONZEGRAM:
		case PM_HUMAN_WEREBRONZEGRAM:
		case PM_WERECHROMEGRAM:
		case PM_HUMAN_WERECHROMEGRAM:
			typ = PM_JOURNEYMAN_ROLL_WALL ;
			if (genbuf) strcpy(genbuf, "pentagram");
			break;
		case PM_WEREDEMON:
		case PM_HUMAN_WEREDEMON:
			typ = rn2(5) ? PM_HELL_HOUND_PUP : PM_HELL_HOUND ;
			if (genbuf) strcpy(genbuf, "demon");
			break;
		case PM_WEREPHANT:
		case PM_HUMAN_WEREPHANT:
			typ = PM_MUMAK ;
			if (genbuf) strcpy(genbuf, "phant");
			break;
		case PM_WEREPIERCER:
		case PM_HUMAN_WEREPIERCER:
			typ = rn2(5) ? PM_ROCK_PIERCER : PM_IRON_PIERCER ;
			if (genbuf) strcpy(genbuf, "piercer");
			break;
		case PM_WEREPENETRATOR:
		case PM_HUMAN_WEREPENETRATOR:
			typ = rn2(5) ? PM_ROCK_PENETRATOR : PM_IRON_PENETRATOR ;
			if (genbuf) strcpy(genbuf, "penetrator");
			break;
		case PM_WERESMASHER:
		case PM_HUMAN_WERESMASHER:
			typ = rn2(5) ? PM_ROCK_SMASHER : PM_IRON_SMASHER ;
			if (genbuf) strcpy(genbuf, "smasher");
			break;
		case PM_WERENYMPH:
		case PM_HUMAN_WERENYMPH:
			typ = rn2(5) ? PM_WOOD_NYMPH : PM_WATER_NYMPH ;
			if (genbuf) strcpy(genbuf, "nymph");
			break;
		case PM_WEREBEE:
		case PM_HUMAN_WEREBEE:
			typ = PM_KILLER_BEE;
			if (genbuf) strcpy(genbuf, "bee");
			break;
		case PM_WEREMIMIC:
		case PM_HUMAN_WEREMIMIC:
			typ = rn2(5) ? PM_LARGE_MIMIC : PM_GIANT_MIMIC ;
			if (genbuf) strcpy(genbuf, "mimic");
			break;
		case PM_WEREPERMAMIMIC:
		case PM_HUMAN_WEREPERMAMIMIC:
			typ = rn2(5) ? PM_LARGE_PERMAMIMIC : PM_GIANT_PERMAMIMIC ;
			if (genbuf) strcpy(genbuf, "mimic");
			break;
		case PM_WEREMINDFLAYER:
		case PM_HUMAN_WEREMINDFLAYER:
			typ = rn2(5) ? PM_MIND_FLAYER : PM_MASTER_MIND_FLAYER ;
			if (genbuf) strcpy(genbuf, "mind flayer");
			break;
		case PM_WERECOCKATRICE:
		case PM_HUMAN_WERECOCKATRICE:
			typ = rn2(5) ? PM_CHICKATRICE : PM_COCKATRICE ;
			if (genbuf) strcpy(genbuf, "cockatrice");
			break;
		case PM_WERESOLDIERANT:
		case PM_HUMAN_WERESOLDIERANT:
			typ = rn2(5) ? PM_SOLDIER_ANT : PM_GIANT_RED_ANT ;
			if (genbuf) strcpy(genbuf, "ant");
			break;
		case PM_WERETROLL:
		case PM_HUMAN_WERETROLL:
			typ = rn2(5) ? PM_TROLL : PM_ICE_TROLL ;
			if (genbuf) strcpy(genbuf, "troll");
			break;
		case PM_WEREBAT:
		case PM_HUMAN_WEREBAT:
			typ = rn2(5) ? PM_BAT : PM_GIANT_BAT ;
			if (genbuf) strcpy(genbuf, "bat");
			break;
		case PM_WEREPIRANHA:
		case PM_HUMAN_WEREPIRANHA:
			typ = rn2(5) ? PM_PIRANHA : PM_BLOODTHIRSTY_PIRANHA ;
			if (genbuf) strcpy(genbuf, "piranha");
			break;
		case PM_WEREEEL:
		case PM_HUMAN_WEREEEL:
			typ = rn2(5) ? PM_GIANT_EEL : PM_ELECTRIC_EEL ;
			if (genbuf) strcpy(genbuf, "eel");
			break;
		case PM_WEREFLYFISH:
		case PM_HUMAN_WEREFLYFISH:
			typ = rn2(5) ? PM_FLYING_RIBBON_FISH : PM_FLYING_SKELETON_FISH ;
			if (genbuf) strcpy(genbuf, "flying fish");
			break;
		case PM_WEREKRAKEN:
		case PM_HUMAN_WEREKRAKEN:
			typ = rn2(5) ? PM_KRAKEN : PM_HENTAI_OCTOPUS ;
			if (genbuf) strcpy(genbuf, "kraken");
			break;
		case PM_WERELICH:
		case PM_HUMAN_WERELICH:
			typ = rn2(5) ? PM_LICH : PM_LICH_WARRIOR ;
			if (genbuf) strcpy(genbuf, "lich");
			break;
		case PM_WEREJABBERWOCK:
		case PM_HUMAN_WEREJABBERWOCK:
			typ = rn2(5) ? PM_JABBERWOCK : PM_VORPAL_JABBERWOCK ;
			if (genbuf) strcpy(genbuf, "jabberwock");
			break;
		case PM_WEREGRIDBUG:
		case PM_HUMAN_WEREGRIDBUG:
			typ = rn2(5) ? PM_GRID_BUG : PM_TRON_BUG ;
			if (genbuf) strcpy(genbuf, "grid bug");
			break;
		case PM_WEREACIDBLOB:
		case PM_HUMAN_WEREACIDBLOB:
			typ = PM_ACID_BLOB;
			if (genbuf) strcpy(genbuf, "blob");
			break;
		case PM_WEREFOX:
		case PM_HUMAN_WEREFOX:
			typ = rn2(5) ? PM_FOX : PM_LARGE_FOX;
			if (genbuf) strcpy(genbuf, "fox");
			break;
		case PM_WEREMONKEY:
		case PM_HUMAN_WEREMONKEY:
			typ = rn2(5) ? PM_MONKEY : PM_INTELLIGENT_MONKEY;
			if (genbuf) strcpy(genbuf, "monkey");
			break;
		case PM_WEREFLOATINGEYE:
		case PM_HUMAN_WEREFLOATINGEYE:
			typ = PM_FLOATING_EYE;
			if (genbuf) strcpy(genbuf, "eye");
			break;
		case PM_WEREGIANTANT:
		case PM_HUMAN_WEREGIANTANT:
			typ = PM_GIANT_ANT;
			if (genbuf) strcpy(genbuf, "ant");
			break;
		case PM_WEREKOBOLD:
		case PM_HUMAN_WEREKOBOLD:
			typ = rn2(5) ? PM_KOBOLD : PM_LARGE_KOBOLD;
			if (genbuf) strcpy(genbuf, "kobold");
			break;
		case PM_WERELEPRECHAUN:
		case PM_HUMAN_WERELEPRECHAUN:
			typ = PM_LEPRECHAUN;
			if (genbuf) strcpy(genbuf, "leprechaun");
			break;
		case PM_WEREHORSE:
		case PM_HUMAN_WEREHORSE:
			typ = rn2(3) ? PM_PONY : PM_HORSE;
			if (genbuf) strcpy(genbuf, "horse");
			break;
		case PM_WERERUSTMONSTER:
		case PM_HUMAN_WERERUSTMONSTER:
			typ = PM_RUST_MONSTER;
			if (genbuf) strcpy(genbuf, "rust monster");
			break;
		case PM_WEREBLACKLIGHT:
		case PM_HUMAN_WEREBLACKLIGHT:
			typ = PM_BLACK_LIGHT;
			if (genbuf) strcpy(genbuf, "light");
			break;
		case PM_WEREAUTON:
		case PM_HUMAN_WEREAUTON:
			typ = rn2(3) ? PM_MONOTON : rn2(2) ? PM_DUTON : rn2(2) ? PM_TRITON : rn2(3) ? PM_QUATON : PM_QUINON;
			if (genbuf) strcpy(genbuf, "auton");
			break;
		case PM_WEREGREMLIN:
		case PM_HUMAN_WEREGREMLIN:
			typ = PM_HEALING_GREMLIN;
			if (genbuf) strcpy(genbuf, "gremlin");
			break;
		case PM_WEREGREENSLIME:
		case PM_HUMAN_WEREGREENSLIME:
			typ = PM_GREEN_SLIME;
			if (genbuf) strcpy(genbuf, "slime");
			break;
		case PM_WEREJELLY:
		case PM_HUMAN_WEREJELLY:
			typ = PM_OCHRE_JELLY;
			if (genbuf) strcpy(genbuf, "jelly");
			break;
		case PM_WEREXAN:
		case PM_HUMAN_WEREXAN:
			typ = PM_XAN;
			if (genbuf) strcpy(genbuf, "xan");
			break;
		case PM_WEREMUMAK:
		case PM_HUMAN_WEREMUMAK:
			typ = PM_MUMAK;
			if (genbuf) strcpy(genbuf, "mumak");
			break;
		case PM_WERECENTAUR:
		case PM_HUMAN_WERECENTAUR:
			typ = rn2(5) ? PM_PLAINS_CENTAUR : PM_FOREST_CENTAUR;
			if (genbuf) strcpy(genbuf, "centaur");
			break;
		case PM_WERECOUATL:
		case PM_HUMAN_WERECOUATL:
			typ = PM_COUATL;
			if (genbuf) strcpy(genbuf, "couatl");
			break;
		case PM_WEREGELATINOUSCUBE:
		case PM_HUMAN_WEREGELATINOUSCUBE:
			typ = PM_GELATINOUS_CUBE;
			if (genbuf) strcpy(genbuf, "gelatinous cube");
			break;
		case PM_WEREAIRELEMENTAL:
		case PM_HUMAN_WEREAIRELEMENTAL:
			typ = PM_AIR_ELEMENTAL;
			if (genbuf) strcpy(genbuf, "air elemental");
			break;
		case PM_WEREFIREELEMENTAL:
		case PM_HUMAN_WEREFIREELEMENTAL:
			typ = PM_FIRE_ELEMENTAL;
			if (genbuf) strcpy(genbuf, "fire elemental");
			break;
		case PM_WEREEARTHELEMENTAL:
		case PM_HUMAN_WEREEARTHELEMENTAL:
			typ = PM_EARTH_ELEMENTAL;
			if (genbuf) strcpy(genbuf, "earth elemental");
			break;
		case PM_WEREWATERELEMENTAL:
		case PM_HUMAN_WEREWATERELEMENTAL:
			typ = PM_WATER_ELEMENTAL;
			if (genbuf) strcpy(genbuf, "water elemental");
			break;
		case PM_WEREASTRALELEMENTAL:
		case PM_HUMAN_WEREASTRALELEMENTAL:
			typ = PM_ASTRAL_ELEMENTAL;
			if (genbuf) strcpy(genbuf, "astral elemental");
			break;
		case PM_WEREXORN:
		case PM_HUMAN_WEREXORN:
			typ = PM_XORN;
			if (genbuf) strcpy(genbuf, "xorn");
			break;
		case PM_WERESTALKER:
		case PM_HUMAN_WERESTALKER:
			typ = PM_STALKER;
			if (genbuf) strcpy(genbuf, "stalker");
			break;
		case PM_WEREDRACONIAN:
		case PM_HUMAN_WEREDRACONIAN:
			typ = PM_PLAIN_DRACONIAN;
			if (genbuf) strcpy(genbuf, "draconian");
			break;
		case PM_WEREUMBERHULK:
		case PM_HUMAN_WEREUMBERHULK:
			typ = PM_UMBER_HULK;
			if (genbuf) strcpy(genbuf, "hulk");
			break;
		case PM_WEREVAMPIRE:
		case PM_HUMAN_WEREVAMPIRE:
			typ = PM_VAMPIRE;
			if (genbuf) strcpy(genbuf, "vampire");
			break;
		case PM_WEREKILLERBEE:
		case PM_HUMAN_WEREKILLERBEE:
			typ = PM_KILLER_BEE;
			if (genbuf) strcpy(genbuf, "bee");
			break;
		case PM_WEREKANGAROO:
		case PM_HUMAN_WEREKANGAROO:
			typ = PM_KANGAROO;
			if (genbuf) strcpy(genbuf, "kangaroo");
			break;
		case PM_WEREGRUE:
		case PM_HUMAN_WEREGRUE:
			typ = PM_DARK_GRUE;
			if (genbuf) strcpy(genbuf, "grue");
			break;
		case PM_WERECOINS:
		case PM_HUMAN_WERECOINS:
			typ = rn2(5) ? PM_PILE_OF_KILLER_COINS : PM_LARGE_PILE_OF_KILLER_COINS;
			if (genbuf) strcpy(genbuf, "coin");
			break;
		case PM_WERETRAPPER:
		case PM_HUMAN_WERETRAPPER:
			typ = PM_TRAPPER;
			if (genbuf) strcpy(genbuf, "trapper");
			break;
		case PM_WERESHADE:
		case PM_HUMAN_WERESHADE:
			typ = PM_SHADE;
			if (genbuf) strcpy(genbuf, "shade");
			break;
		case PM_WEREDISENCHANTER:
		case PM_HUMAN_WEREDISENCHANTER:
			typ = PM_DISENCHANTER;
			if (genbuf) strcpy(genbuf, "disenchanter");
			break;
		case PM_WERENAGA:
		case PM_HUMAN_WERENAGA:
			typ = rn2(10) ? PM_WHITE_NAGA_HATCHLING : PM_WHITE_NAGA;
			if (genbuf) strcpy(genbuf, "naga");
			break;
		case PM_WEREWORM:
		case PM_HUMAN_WEREWORM:
			typ = rn2(10) ? PM_BABY_PURPLE_WORM : PM_PURPLE_WORM;
			if (genbuf) strcpy(genbuf, "worm");
			break;
		case PM_WEREDRAGON:
		case PM_HUMAN_WEREDRAGON:
			typ = rn2(2) ? PM_BABY_RED_DRAGON : PM_YOUNG_RED_DRAGON;
			if (genbuf) strcpy(genbuf, "dragon");
			break;
		case PM_WEREWEDGESANDAL:
		case PM_HUMAN_WEREWEDGESANDAL:
			typ = PM_ANIMATED_WEDGE_SANDAL;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WEREHUGGINGBOOT:
		case PM_HUMAN_WEREHUGGINGBOOT:
			typ = PM_ANIMATED_HUGGING_BOOT;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WEREPEEPTOE:
		case PM_HUMAN_WEREPEEPTOE:
			typ = PM_ANIMATED_LEATHER_PEEP_TOE;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WERESEXYLEATHERPUMP:
		case PM_HUMAN_WERESEXYLEATHERPUMP:
			typ = PM_ANIMATED_SEXY_LEATHER_PUMP;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WEREBLOCKHEELEDCOMBATBOOT:
		case PM_HUMAN_WEREBLOCKHEELEDCOMBATBOOT:
			typ = PM_ANIMATED_BLOCK_HEELED_COMBAT_BOOT;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WERECOMBATSTILETTO:
		case PM_HUMAN_WERECOMBATSTILETTO:
			typ = PM_ANIMATED_COMBAT_STILETTO;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WEREBEAUTIFULFUNNELHEELEDPUMP:
		case PM_HUMAN_WEREBEAUTIFULFUNNELHEELEDPUMP:
			typ = PM_ANIMATED_BEAUTIFUL_FUNNEL_HEELED_PUMP;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WEREPROSTITUTESHOE:
		case PM_HUMAN_WEREPROSTITUTESHOE:
			typ = PM_ANIMATED_PROSTITUTE_SHOE;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WERESTILETTOSANDAL:
		case PM_HUMAN_WERESTILETTOSANDAL:
			typ = PM_ANIMATED_STILETTO_SANDAL;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WEREUNFAIRSTILETTO:
		case PM_HUMAN_WEREUNFAIRSTILETTO:
			typ = PM_ANIMATED_UNFAIR_STILETTO;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		case PM_WEREWINTERSTILETTO:
		case PM_HUMAN_WEREWINTERSTILETTO:
			typ = PM_ANIMATED_WINTER_STILETTO;
			if (genbuf) strcpy(genbuf, "shoe");
			break;
		default:
			continue;
	    }
	    if (ownloc) mtmp = makemon(&mons[typ], 0, 0, yours ? MM_NOSPECIALS : NO_MM_FLAGS);
	    else mtmp = makemon(&mons[typ], u.ux, u.uy, yours ? MM_NOSPECIALS : NO_MM_FLAGS);
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
	char buf[BUFSZ];

	if (Unchanging || (u.umonnum == u.ulycn)) return;
	if (Polymorph_control && rn2(5) ) {
	    /* `+4' => skip "were" prefix to get name of beast */
	    sprintf(qbuf, "Do you want to change into %s? [yes/no]",
		    an(mons[u.ulycn].mname+4));

		getlin(qbuf,buf);
		(void) lcase (buf);
		if (strcmp (buf, "yes")) return;

	}
	(void) polymon(u.ulycn);
}

void
you_unwere(purify)
boolean purify;
{
	boolean in_wereform = (u.umonnum == u.ulycn);

	if (purify) {
	    if (Race_if(PM_HUMAN_WEREWOLF) || Role_if(PM_LUNATIC) || Race_if(PM_AK_THIEF_IS_DEAD_) ) {
		/* An attempt to purify you has been made! */
		if (in_wereform && Unchanging) {
			u.youaredead = 1;
		    killer_format = NO_KILLER_PREFIX;
		    killer = "purified while stuck in creature form";
		    pline_The("purification was deadly...");
		    done(DIED);
			u.youaredead = 0;
		} else {
		    You_feel("very bad!");
		    if (in_wereform && !Race_if(PM_UNGENOMOLD) )
			rehumanize();
		    (void) adjattrib(A_STR, -rn1(3,3), 2, TRUE);
		    (void) adjattrib(A_CON, -rn1(3,3), 1, TRUE);
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
		(!Polymorph_control || yn("Remain in beast form?") == 'n')) {
    	    if (!Race_if(PM_UNGENOMOLD)) rehumanize();
		else polyself(FALSE);
	}
}

#endif /* OVLB */

/*were.c*/

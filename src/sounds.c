/*	SCCS Id: @(#)sounds.c	3.4	2002/05/06	*/
/*	Copyright (c) 1989 Janet Walz, Mike Threepoint */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h"
#ifdef USER_SOUNDS
# ifdef USER_SOUNDS_REGEX
#include <regex.h>
# endif
#endif

/* Hmm.... in working on SHOUT I started thinking about things.
 * I think something like this should be set up:
 *  You_hear_mon(mon,loud, msg) - You_hear(msg); monnoise(mon,loud);
 *  monnoise(mon,loud) - wake_nearto(mon->mx,mon->my,mon->data->mlevel*loud)
 *				and stuff like that
 *  mon_say(mon,loud,msg) - verbalize(msg); sayeffects(mon,loud,msg);
 *  sayeffects(mon,loud,msg) - monnoise(mon,loud); + the pet stuff et al
 * In fact, I think will set this up, but as a diff, not actually modifying the
 * files.
 * If I knew something about branches I might do that.
 * But anyway, I should be working on petcommands now... maybe later...
 * -- JRN
 */

#ifdef OVLB

static int FDECL(domonnoise,(struct monst *));
static int NDECL(dochat);
static const char *FDECL(yelp_sound,(struct monst *));
static const char *FDECL(whimper_sound,(struct monst *));

#endif /* OVLB */

#ifdef OVL0

#ifdef DUMB
static int FDECL(mon_in_room, (struct monst *,int));

/* this easily could be a macro, but it might overtax dumb compilers */
static int
mon_in_room(mon, rmtyp)
struct monst *mon;
int rmtyp;
{
    int rno = levl[mon->mx][mon->my].roomno;

    return rooms[rno - ROOMOFFSET].rtype == rmtyp;
}
#else
/* JRN: converted above to macro */
# define mon_in_room(mon,rmtype) (rooms[ levl[(mon)->mx][(mon)->my].roomno \
					- ROOMOFFSET].rtype == (rmtype))
#endif

void
dosounds()
{
    register struct mkroom *sroom;
    register int hallu, vx, vy;
#if defined(AMIGA) && defined(AZTEC_C_WORKAROUND)
    int xx;
#endif
    struct monst *mtmp;

    if (!flags.soundok || u.uswallow || Underwater) return;

    hallu = Hallucination ? 1 : 0;

    if (level.flags.nfountains && !rn2(400)) {
	static const char * const fountain_msg[4] = {
		"bubbling water.",
		"water falling on coins.",
		"the splashing of a naiad.",
		"a soda fountain!",
	};
	You_hear(fountain_msg[rn2(3+hallu)]);
    }
#ifdef SINK
    if (level.flags.nsinks && !rn2(300)) {
	static const char * const sink_msg[3] = {
		"a slow drip.",
		"a gurgling noise.",
		"dishes being washed!",
	};
	You_hear(sink_msg[rn2(2+hallu)]);
    }
#endif
    if (level.flags.has_court && !rn2(200)) {
	static const char * const throne_msg[4] = {
		"the tones of courtly conversation.",
		"a sceptre pounded in judgment.",
		"Someone shouts \"Off with %s head!\"",
		"Queen Beruthiel's cats!",
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping ||
			is_lord(mtmp->data) || is_prince(mtmp->data)) &&
		!is_animal(mtmp->data) &&
		mon_in_room(mtmp, COURT)) {
		/* finding one is enough, at least for now */
		int which = rn2(3+hallu);

		if (which != 2) You_hear(throne_msg[which]);
		else		pline(throne_msg[2], uhis());
		return;
	    }
	}
    }
    if (level.flags.has_swamp && !rn2(200)) {
	static const char * const swamp_msg[4] = {
		"hear mosquitoes!",
		"smell marsh gas!",	/* so it's a smell...*/
		"hear Donald Duck!",
		"inhale a vile stench that reminds you of what kissing a frog is probably like!",
	};
	You(swamp_msg[rn2(2+hallu*2)]);
	return;
    }
    if (level.flags.spooky && !rn2(200)) {
	static const char *spooky_msg[24] = {
		"hear screaming in the distance!",
		"hear a faint whisper: \"Please leave your measurements for your custom-made coffin.\"",
		"hear a door creak ominously.",
		"hear hard breathing just a few steps behind you!",
		"hear dragging footsteps coming closer!",
		"hear anguished moaning and groaning coming out of the walls!",
		"hear mad giggling directly behind you!",
		"smell rotting corpses.",
		"smell chloroform!",
		"feel ice cold fingers stroking your neck.",
		"feel a ghostly touch caressing your face.",
		"feel somebody dancing on your grave.",
		"feel something breathing down your neck.",
		"feel as if the walls were closing in on you.",
		"just stepped on something squishy.",
		"hear a strong voice pronouncing: \"There can only be one!\"",
		"hear a voice booming all around you: \"Warning: self-destruction sequence activated!\"",
		"smell your mother-in-law's cooking!",
		"smell horse dung.",
		"hear someone shouting: \"Who ordered the burger?\"",
		"can faintly hear the Twilight Zone theme.",
		"hear an outraged customer complaining: \"I'll be back!\"",
		"hear someone praising your valor!",
		"hear someone singing: \"Jingle bells, jingle bells...\"",
	};
	You(spooky_msg[rn2(15+hallu*9)]);
	return;
    }
    if (level.flags.has_vault && !rn2(200)) {
	if (!(sroom = search_special(VAULT))) {
	    /* strange ... */
	    level.flags.has_vault = 0;
	    return;
	}
	if(gd_sound())
	    switch (rn2(2+hallu)) {
		case 1: {
		    boolean gold_in_vault = FALSE;

		    for (vx = sroom->lx;vx <= sroom->hx; vx++)
			for (vy = sroom->ly; vy <= sroom->hy; vy++)
			    if (g_at(vx, vy))
				gold_in_vault = TRUE;
#if defined(AMIGA) && defined(AZTEC_C_WORKAROUND)
		    /* Bug in aztec assembler here. Workaround below */
		    xx = ROOM_INDEX(sroom) + ROOMOFFSET;
		    xx = (xx != vault_occupied(u.urooms));
		    if(xx)
#else
		    if (vault_occupied(u.urooms) !=
			 (ROOM_INDEX(sroom) + ROOMOFFSET))
#endif /* AZTEC_C_WORKAROUND */
		    {
			if (gold_in_vault && !level.flags.vault_is_aquarium && !level.flags.vault_is_cursed)
			    You_hear(!hallu ? "someone counting money." :
				"the quarterback calling the play.");
			else if (gold_in_vault && level.flags.vault_is_aquarium)
			    You_hear(!hallu ? "soft splashing." :
				"a swimmer divebomb into the water.");
			else if (gold_in_vault && level.flags.vault_is_cursed)
			    You_hear(!hallu ? "gutteral voices chanting." :
				"drums beating ominously!");
			else
			    You_hear(level.flags.vault_is_aquarium ? "frenzied splashing." : level.flags.vault_is_cursed ? "someone scream: 'No! Stop! The Knife! Aaaaaahhhh!'" : "someone searching.");
			break;
		    }
		    /* fall into... (yes, even for hallucination) */
		}
		case 0:
		    You_hear(level.flags.vault_is_aquarium ? "gently lapping water." : level.flags.vault_is_cursed ? "somebody screaming for help!" : "the footsteps of a guard on patrol.");
		    break;
		case 2:
		    You_hear(level.flags.vault_is_aquarium ? "the bubbling of an oxygen filter." : level.flags.vault_is_cursed ? "somebody whisper: 'You've been detected by a security camera! Har har har!'" : "Ebenezer Scrooge!");
		    break;
	    }
	return;
    }
    if (level.flags.has_beehive && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->data->mlet == S_ANT && is_flyer(mtmp->data)) &&
		mon_in_room(mtmp, BEEHIVE)) {
		switch (rn2(2+hallu*2)) {
		    case 0:
			You_hear("a low buzzing.");
			break;
		    case 1:
			You_hear("an angry drone.");
			break;
		    case 2:
			You_hear("bees in your %sbonnet!",
			    uarmh ? "" : "(nonexistent) ");
			break;
		    case 3:
			pline("You suddenly see King Zing chasing after you! RUN, Rambi, RUN!!!");
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_morgue && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_undead(mtmp->data) &&
		mon_in_room(mtmp, MORGUE)) {
		switch (rn2(2+hallu*2)) {
		    case 0:
			You("suddenly realize it is unnaturally quiet.");
			break;
		    case 1:
			pline_The("%s on the back of your %s stands up.",
				body_part(HAIR), body_part(NECK));
			break;
		    case 2:
			pline_The("%s on your %s seems to stand up.",
				body_part(HAIR), body_part(HEAD));
			break;
		    case 3:
			pline("An enormous ghost suddenly appears before you!");
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_barracks && !rn2(200)) {
	static const char * const barracks_msg[4] = {
		"blades being honed.",
		"loud snoring.",
		"dice being thrown.",
		"General MacArthur!",
	};
	int count = 0;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_mercenary(mtmp->data) &&
#if 0		/* don't bother excluding these */
		!strstri(mtmp->data->mname, "watch") &&
		!strstri(mtmp->data->mname, "guard") &&
#endif
		mon_in_room(mtmp, BARRACKS) &&
		/* sleeping implies not-yet-disturbed (usually) */
		(mtmp->msleeping || ++count > 5)) {
		You_hear(barracks_msg[rn2(3+hallu)]);
		return;
	    }
	}
    }
	    if (level.flags.has_clinic && !rn2(200)) {
		static const char *hospital_msg[4] = {
			"hear something about streptococus.",
			"smell chloroform nearby.",
			"hear someone cursing viruses.",
			"seem to hear Doctor Frankenstein.",
		};
		You(hospital_msg[rn2(3+hallu)]);
		return;
	    }
	    if (level.flags.has_nymphhall && !rn2(200)) {
		static const char *nymphhall_msg[4] = {
			"hear leaves rustling.",
			"hear a female voice singing a song.",
			"catch a glimpse of sunlight.",
			"seem mesmerized.",
		};
		You(nymphhall_msg[rn2(3+hallu)]);
		return;
	    }
	    if (level.flags.has_spiderhall && !rn2(200)) {
		static const char *spiderhall_msg[5] = {
			"hear mandibles clicking.",
			"notice webs everywhere.",
			"notice little openings in the floor.",
			"trip over a broken spider egg.",
			"see a giant tarantula squittering around!",
		};
		You(spiderhall_msg[rn2(3+hallu*2)]);
		return;
	    }
	    if (level.flags.has_trollhall && !rn2(200)) {
		static const char *trollhall_msg[5] = {
			"hear a grunting noise.",
			"notice a horrible smell.",
			"hear loud footsteps going up and down the halls.",
			"trip over a stinking pile of shit!",
			"suddenly see a huge monster appearing right in front of you!",
		};
		You(trollhall_msg[rn2(3+hallu*2)]);
		return;
	    }
	    if (level.flags.has_humanhall && !rn2(200)) {
		static const char *humanhall_msg[6] = {
			"listen to a conversation.",
			"hear people talking.",
			"hear someone drawing a weapon.",
			"sense that you are not alone...",
			"hear a glass of wine breaking!",
			"listen to a radio broadcast: For the next five days, the dungeon's estimated temperature high is 96 degrees. Now, is that in Fahrenheit or Kelvin?",
		};
		You(humanhall_msg[rn2(3+hallu*3)]);
		return;
	    }
	    if (level.flags.has_golemhall && !rn2(200)) {
		static const char *golemhall_msg[5] = {
			"hear rattling noises.",
			"think you saw an object move on its own.",
			"feel that something weird may be just around the corner.",
			"hear the deafening noise of a metal object breaking apart!",
			"see a warmech appearing before you! If this were a Pokemon game, it would say: A wild WARMECH appeared!",
		};
		You(golemhall_msg[rn2(3+hallu*2)]);
		return;
	    }
	    if (level.flags.has_coinhall && !rn2(200)) {
		static const char *coinhall_msg[4] = {
			"notice the sound of clinging coins.",
			"feel that, somewhere down here, there might be a whole lot of gold.",
			"hear metal banging against a wall.",
			"hear Scrooge McDuck taking a bath in his money!",
		};
		You(coinhall_msg[rn2(3+hallu)]);
		return;
	    }
	    if (level.flags.has_angelhall && !rn2(200)) {
		static const char *angelhall_msg[4] = {
			"hear the sound of wispy wings.",
			"can barely hear a whispering voice.",
			"sense a glow of radiance.",
			"think you just heard God speaking to you!",
		};
		You(angelhall_msg[rn2(3+hallu)]);
		return;
	    }
	    if (level.flags.has_elemhall && !rn2(200)) {
		static const char *elemhall_msg[5] = {
			"suddenly feel a wave of heat.",
			"notice some heaps of earth.",
			"sense a strong gust of wind.",
			"hear a wave of water.",
			"get the impression that the laws of Nature itself are conspiring against you!",
		};
		You(elemhall_msg[rn2(4+hallu)]);
		return;
	    }
	    if (level.flags.has_terrorhall && !rn2(200)) {
		static const char *terrorhall_msg[5] = {
			"feel weirded out.",
			"sense something strange about this place.",
			"think you just saw something move.",
			"think you're seeing white rabbits!",
			"feel totally down, like you're on a bad trip!",
		};
		You(terrorhall_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_insideroom && !rn2(200)) {
		static const char *insideroom_msg[19] = {
			"have a weird feeling of chills and excitement.",
			"sense that something isn't right here...",
			"shudder in expectation.",
			"see an error message pop up: NETHACK.EXE caused a General Protection Fault at address 000D:001D.",
			"see an error message pop up: Warning: Low Local Memory. Freeing description strings.",
			"see an error message pop up: nt|| - Not a valid save file",
			"see an error message pop up: APPLICATION ERROR - integer divide by 0",
			"see an error message pop up: Runtime error! Program: NETHACK.EXE. R6025 - pure virtual function call",
			"see an error message pop up: Buffer overrun detected! Program: NETHACK.EXE. A buffer overrun has been detected which has corrupted the program's internal state. The program cannot safely continue execution and must now be terminated.",
			"see an error message pop up: Runtime error! Program: NETHACK.EXE. This application has requested the Runtime to terminate it in an unusual way. Please contact the application's support team for more information.",
			"see an error message pop up: Not enough memory to create inventory window",
			"see an error message pop up: Error: Nethack will only run in Protect mode",
			"see an error message pop up: Oops... Suddenly, the dungeon collapses. NETHACK.EXE has stopped working. Unsaved data may have been lost.",
			"see an error message pop up: ERROR: SIGNAL 11 WAS RAISED",
			"see an error message pop up: UNHANDLED EXCEPTION: ACCESS_VIOLATION (C0000005)",
			"see an error message pop up: An error has occurred in your application. If you choose Close, your application will be terminated. If you choose Ignore, you should save your work in a new file.",
			"see an error message pop up: Do you want your possessions identified? DYWYPI?",
			"see an error message pop up: Windows Subsystem service has stopped unexpectedly.",
			"see an error message pop up: nv4_disp.dll device driver is stuck in an infinite loop.",
		};
		You(insideroom_msg[rn2(3+hallu*16)]);
		return;
	    }

	    if (level.flags.has_riverroom && !rn2(200)) {
		static const char *riverroom_msg[5] = {
			"hear the trickle of water.",
			"sense a watery smell mixed with grass.",
			"listen to an unusual ambient sound.",
			"hear a fountain crash through solid rock with a mighty roar!",
			"admire the beautiful mountains around here. Strange, how can mountains be in an underground dungeon?",
		};
		You(riverroom_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_tenshall && !rn2(200)) {
		static const char *tenshall_msg[10] = {
			"feel a certain tension.",
			"feel a certain tension. This message was stolen from ADOM.",
			"sense a certain tension.",
			"suddenly see nightmarish images with Thomas Biskup in your mind!", /* ADOM creator */
			"sense a certain tension. Perhaps you should head for the nearest toilet...",
			"sense a certain tension. That dreaded Dungeon Master probably filled the next room with arch-liches, disenchanters and master mind flayers.",
			"sense a certain tension. That dreaded Dungeon Master probably filled the next room with black dragons, green slimes and invisible cockatrices.",
			"sense a certain tension. That dreaded Dungeon Master probably filled the next room with luck suckers, Mothers-in-Law and questing beasts.",
			"fear that a fucking crash bug is going to eat your savegame file!", /* yes, Adom does that sometimes */
			"fear that some anti-cheat mechanism is going to curse all of your items even though you clearly didn't cheat at all!", /* and yes, that's another annoying fact of Adom that makes me glad I'm playing Nethack instead --Amy */
		};
		You(tenshall_msg[rn2(3+hallu*7)]);
		return;
	    }
    if (level.flags.has_zoo && !rn2(200)) {
	static const char * const zoo_msg[3] = {
		"a sound reminiscent of an elephant stepping on a peanut.",
		"a sound reminiscent of a seal barking.",
		"Doctor Doolittle!",
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping || is_animal(mtmp->data)) &&
		    mon_in_room(mtmp, ZOO)) {
		You_hear(zoo_msg[rn2(2+hallu)]);
		return;
	    }
	}
    }
    if (level.flags.has_shop && !rn2(200)) {
	if (!(sroom = search_special(ANY_SHOP))) {
	    /* strange... */
	    level.flags.has_shop = 0;
	    return;
	}
	if (tended_shop(sroom) &&
		!index(u.ushops, ROOM_INDEX(sroom) + ROOMOFFSET)) {
	    static const char * const shop_msg[3] = {
		    "someone cursing shoplifters.",
		    "the chime of a cash register.",
		    "Neiman and Marcus arguing!",
	    };
	    You_hear(shop_msg[rn2(2+hallu)]);
	}
	return;
    }
    if (Is_oracle_level(&u.uz) && !rn2(400)) {
	/* make sure the Oracle is still here */
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if (!DEADMONSTER(mtmp) && mtmp->data == &mons[PM_ORACLE])
		break;
	/* and don't produce silly effects when she's clearly visible */
	if (mtmp && (hallu || !canseemon(mtmp))) {
	    static const char * const ora_msg[5] = {
		    "a strange wind.",		/* Jupiter at Dodona */
		    "convulsive ravings.",	/* Apollo at Delphi */
		    "snoring snakes.",		/* AEsculapius at Epidaurus */
		    "someone say \"No more woodchucks!\"",
		    "a loud ZOT!"		/* both rec.humor.oracle */
	    };
	    /* KMH -- Give funny messages on Groundhog Day */
	    if (flags.groundhogday) hallu = 1;
	    You_hear(ora_msg[rn2(3+hallu*2)]);
	}
	return;
    }
#ifdef BLACKMARKET
    if (!Is_blackmarket(&u.uz) && at_dgn_entrance("One-eyed Sam's Market") &&
        !rn2(200)) {
      static const char *blkmar_msg[4] = {
        "You hear someone complaining about the prices.",
        "Somebody whispers: \"Food rations? Only 900 zorkmids.\"",
        "You feel like searching for more gold.",
        "Somebody whispers: \"Dawg, I got quality shit around here.\"",
      };
      pline(blkmar_msg[rn2(2+hallu*2)]);
    }
#endif /* BLACKMARKET */

	/* finding alignment quests is way too hard, so the player will get some help. --Amy */
    if ( (at_dgn_entrance("Lawful Quest") || at_dgn_entrance("Neutral Quest") || at_dgn_entrance("Chaotic Quest") ) &&
        !rn2(1000)) {
      static const char *alignquest_msg[4] = {
        "You feel the presence of some sort of force.",
        "There seems to be something special about this place.",
        "You hear someone say: \"Force field activated!\"",
        "You hear someone say: \"Warp panel test sequence is starting in three... two... one...\"",
      };
      pline(alignquest_msg[rn2(2+hallu*2)]);
    }

}

#endif /* OVL0 */
#ifdef OVLB

static const char * const h_sounds[] = {
    "beep", "boing", "sing", "belche", "creak", "cough", "rattle",
    "ululate", "pop", "jingle", "sniffle", "tinkle", "eep",
	"clatter", "hum", "sizzle", "twitter", "wheeze", "rustle",
	"honk", "lisp", "yodel", "coo", "burp", "moo", "boom",
	"murmur", "oink", "quack", "rumble", "twang", "bellow",
	"toot", "gargle", "hoot", "warble", "crackle", "hiss",
	"growl", "roar", "buzz", "squeal", "screech", "neigh",
	"wail", "commotion", "squaark", "scream", "yowl", "yelp",
	"snarl", "squeal", "screak", "whimper", "whine", "howl",
	"yip", "bark", "purr", "meow", "mew", "drone", "whinnie",
	"whicker", "gurgle", "burble", "shriek", "baaaa", "cluck"

};

/* make the sounds of a pet in any level of distress */
/* (1 = "whimper", 2 = "yelp", 3 = "growl") */
void
pet_distress(mtmp, lev)
register struct monst *mtmp;
int lev;
{
    const char *verb;
    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;
    /* presumably nearness and soundok checks have already been made */

    if (Hallucination)
	verb = h_sounds[rn2(SIZE(h_sounds))];
    else if (lev == 3)
	verb = growl_sound(mtmp);
    else if (lev == 2)
	verb = yelp_sound(mtmp);
    else if (lev == 1)
	verb = whimper_sound(mtmp);
    else
	panic("strange level of distress");

    if (verb) {
	pline("%s %s%c", Monnam(mtmp), vtense((char *)0, verb),
		lev>1?'!':'.');
	if (flags.run) nomul(0, 0);
	wake_nearto(mtmp->mx,mtmp->my,mtmp->data->mlevel*6*lev);
    }
}

/* the sounds of a seriously abused pet, including player attacking it */
/* in extern.h: #define growl(mon) pet_distess((mon),3) */

const char *
growl_sound(mtmp)
register struct monst *mtmp;
{
	const char *ret;

	switch (mtmp->data->msound) {
	case MS_MEW:
	case MS_HISS:
	    ret = "hiss";
	    break;
	case MS_BARK:
	case MS_GROWL:
	    ret = "growl";
	    break;
	case MS_ROAR:
	    ret = "roar";
	    break;
	case MS_BUZZ:
	    ret = "buzz";
	    break;
	case MS_SQEEK:
	    ret = "squeal";
	    break;
	case MS_SQAWK:
	    ret = "screech";
	    break;
	case MS_NEIGH:
	    ret = "neigh";
	    break;
	case MS_WAIL:
	    ret = "wail";
	    break;
	case MS_SILENT:
		ret = "commotion";
		break;
	case MS_PARROT:
	    ret = "squaark";
	    break;
	default:
		ret = "scream";
	}
	return ret;
}

/* the sounds of mistreated pets */
/* in extern.h: #define yelp(mon) pet_distress((mon),2) */

static
const char *
yelp_sound(mtmp)
register struct monst *mtmp;
{
    const char *ret;

    switch(mtmp->data->msound) {
	case MS_MEW:
	ret = "yowl";
	    break;
	case MS_BARK:
	case MS_GROWL:
	ret = "yelp";
	    break;
	case MS_ROAR:
	ret = "snarl";
	    break;
	case MS_SQEEK:
	ret = "squeal";
	    break;
	case MS_SQAWK:
	ret = "screak";
	    break;
	case MS_WAIL:
	ret = "wail";
	    break;
    default:
	ret = (const char*) 0;
    }
    return ret;
}

/* the sounds of distressed pets */
/* in extern.h: #define whimper(mon) pet_distress((mon),1) */

static
const char *
whimper_sound(mtmp)
register struct monst *mtmp;
{
    const char *ret;

    switch (mtmp->data->msound) {
	case MS_MEW:
	case MS_GROWL:
	ret = "whimper";
	    break;
	case MS_BARK:
	ret = "whine";
	    break;
	case MS_SQEEK:
	ret = "squeal";
	    break;
    default:
	ret = (const char *)0;
    }
    return ret;
}

/* pet makes "I'm hungry" noises */
void
beg(mtmp)
register struct monst *mtmp;
{
    if (mtmp->msleeping || !mtmp->mcanmove ||
	    !(carnivorous(mtmp->data) || herbivorous(mtmp->data) || metallivorous(mtmp->data) || mtmp->egotype_lithivore || mtmp->egotype_metallivore || lithivorous(mtmp->data)))
	return;

    /* presumably nearness and soundok checks have already been made */
    if (!is_silent(mtmp->data) && mtmp->data->msound <= MS_ANIMAL)
	(void) domonnoise(mtmp);
    else if (mtmp->data->msound >= MS_HUMANOID) {
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);
	verbalize("I'm hungry.");
}
}

static int
domonnoise(mtmp)
register struct monst *mtmp;
{
    register const char *pline_msg = 0,	/* Monnam(mtmp) will be prepended */
			*verbl_msg = 0;	/* verbalize() */
    struct permonst *ptr = mtmp->data;
    char verbuf[BUFSZ];

    if (mtmp->egotype_farter) {
		pline("You gently caress %s's %s butt using %s %s.", mon_nam(mtmp), mtmp->female ? "sexy" : "ugly", !rn2(3) ? "both your left and right" : rn2(2) ? "your left" : "your right", body_part(HAND) );
		if (mtmp->mtame) {
			pline("%s seems to love you even more than before.", Monnam(mtmp) );
			if (mtmp->mtame < 30) mtmp->mtame++;
			if (EDOG(mtmp)->abuse) EDOG(mtmp)->abuse--;
		}
		else if (mtmp->mpeaceful) {
			pline("%s seems to like being felt up by you.", Monnam(mtmp) );
		}
		else {
			pline("%s seems to be even more angry at you than before.", Monnam(mtmp) );
		}
	    m_respond(mtmp);
    }

    /* presumably nearness and sleep checks have already been made */
    if (!flags.soundok) return(0);
    if (is_silent(ptr)) return(0);

    /* Make sure its your role's quest quardian; adjust if not */
    if (ptr->msound == MS_GUARDIAN && ptr != &mons[urole.guardnum]) {
    	int mndx = monsndx(ptr);
    	ptr = &mons[genus(mndx,1)];
    }

    /* be sure to do this before talking; the monster might teleport away, in
     * which case we want to check its pre-teleport position
     */
    if (!canspotmon(mtmp))
	map_invisible(mtmp->mx, mtmp->my);

    switch (ptr->msound) {
	case MS_ORACLE:
	    return doconsult(mtmp);
	case MS_PRIEST:
	    priest_talk(mtmp);
	    break;
	case MS_LEADER:
	case MS_NEMESIS:
	case MS_GUARDIAN:
	    quest_chat(mtmp);
	    break;
	case MS_SELL: /* pitch, pay, total */
	    shk_chat(mtmp);
	    break;
	case MS_VAMPIRE:
	    {
	    /* vampire messages are varied by tameness, peacefulness, and time of night */
		boolean isnight = night();
		boolean kindred = maybe_polyd(u.umonnum == PM_VAMPIRE ||
				    u.umonnum == PM_VAMPIRE_LORD ||
				    u.umonnum == PM_VAMPIRE_MAGE,
				    Race_if(PM_VAMPIRE));
		boolean nightchild = (Upolyd && (u.umonnum == PM_WOLF ||
				       u.umonnum == PM_SHADOW_WOLF ||
				       u.umonnum == PM_MIST_WOLF ||
				       u.umonnum == PM_WINTER_WOLF ||
	    			       u.umonnum == PM_WINTER_WOLF_CUB));
		const char *racenoun = (flags.female && urace.individual.f) ?
					urace.individual.f : (urace.individual.m) ?
					urace.individual.m : urace.noun;

		if (mtmp->mtame) {
			if (kindred) {
				Sprintf(verbuf, "Good %s to you Master%s",
					isnight ? "evening" : "day",
					isnight ? "!" : ".  Why do we not rest?");
				verbl_msg = verbuf;
		    	} else {
		    	    Sprintf(verbuf,"%s%s",
				nightchild ? "Child of the night, " : "",
				midnight() ?
					"I can stand this craving no longer!" :
				isnight ?
					"I beg you, help me satisfy this growing craving!" :
					"I find myself growing a little weary.");
				verbl_msg = verbuf;
			}
		} else if (mtmp->mpeaceful) {
			if (kindred && isnight) {
				Sprintf(verbuf, "Good feeding %s!",
	    				flags.female ? "sister" : "brother");
				verbl_msg = verbuf;
 			} else if (nightchild && isnight) {
				Sprintf(verbuf,
				    "How nice to hear you, child of the night!");
				verbl_msg = verbuf;
	    		} else
		    		verbl_msg = "I only drink... potions.";
    	        } else {
			int vampindex;
	    		static const char * const vampmsg[] = {
			       /* These first two (0 and 1) are specially handled below */
	    			"I vant to suck your %s!",
	    			"I vill come after %s without regret!",
		    	       /* other famous vampire quotes can follow here if desired */
	    		};
			if (kindred)
			    verbl_msg = "This is my hunting ground that you dare to prowl!";
			else if (youmonst.data == &mons[PM_SILVER_DRAGON] ||
				 youmonst.data == &mons[PM_BABY_SILVER_DRAGON]) {
			    /* Silver dragons are silver in color, not made of silver */
			    Sprintf(verbuf, "%s! Your silver sheen does not frighten me!",
					youmonst.data == &mons[PM_SILVER_DRAGON] ?
					"Fool" : "Young Fool");
			    verbl_msg = verbuf; 
			} else {
			    vampindex = rn2(SIZE(vampmsg));
			    if (vampindex == 0) {
				Sprintf(verbuf, vampmsg[vampindex], body_part(BLOOD));
	    			verbl_msg = verbuf;
			    } else if (vampindex == 1) {
				Sprintf(verbuf, vampmsg[vampindex],
					(Upolyd && !missingnoprotect) ? an(mons[u.umonnum].mname) : an(racenoun));
	    			verbl_msg = verbuf;
		    	    } else
			    	verbl_msg = vampmsg[vampindex];
			}
	        }
	    }
	    break;
	case MS_WERE:
	    if (flags.moonphase == FULL_MOON && (night() ^ !rn2(13))) {
		pline("%s throws back %s head and lets out a blood curdling %s!",
		      Monnam(mtmp), mhis(mtmp),
		      ptr == &mons[PM_HUMAN_WERERAT] ? "shriek" : "howl");
		wake_nearto(mtmp->mx, mtmp->my, 11*11);
	    } else
		pline_msg =
		     "whispers inaudibly.  All you can make out is \"moon\".";
	    break;
	case MS_BARK:
	    if (flags.moonphase == FULL_MOON && night()) {
		pline_msg = "howls.";
	    } else if (mtmp->mpeaceful) {
		if (mtmp->mtame &&
			(mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
			 moves > EDOG(mtmp)->hungrytime || mtmp->mtame < 5))
		    pline_msg = "whines.";
		else if (mtmp->mtame && EDOG(mtmp)->hungrytime > moves + 1000)
		    pline_msg = "yips.";
		else {
		    if (mtmp->data != &mons[PM_DINGO])	/* dingos do not actually bark */
			    pline_msg = "barks.";
		}
	    } else {
		pline_msg = "growls.";
	    }
	    break;
	case MS_MEW:
	    if (mtmp->mtame) {
		if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
			mtmp->mtame < 5)
		    pline_msg = "yowls.";
		else if (moves > EDOG(mtmp)->hungrytime)
		    pline_msg = "meows.";
		else if (EDOG(mtmp)->hungrytime > moves + 1000)
		    pline_msg = "purrs.";
		else
		    pline_msg = "mews.";
		break;
	    } /* else FALLTHRU */
	case MS_GROWL:
	    pline_msg = mtmp->mpeaceful ? "snarls." : "growls!";
	    break;
	case MS_ROAR:
	    pline_msg = (ptr == &mons[PM_SYSTEM_FAILURE]) ? "says in a monotone voice: 'Pieteikumu kluda.'" : mtmp->mpeaceful ? "snarls." : "roars!";
	    break;
	case MS_SQEEK:
	    pline_msg = "squeaks.";
	    break;
	case MS_PARROT:
	    switch (rn2(8)) {
		default:
		case 0:
		    pline_msg = "squaarks louldly!";
		    break;
		case 1:
		    pline_msg = "says 'Polly want a lembas wafer!'";
		    break;
		case 2:
		    pline_msg = "says 'Nobody expects the Spanish Inquisition!'";
		    break;
		case 3:
		    pline_msg = "says 'Who's a good boy, then?'";
		    break;
		case 4:
		    pline_msg = "says 'Show us yer knickers!'";
		    break;
		case 5:
		    pline_msg = "says 'You'll never make it!'";
		    break;
		case 6:
		    pline_msg = "whistles suggestively!";
		    break;
		case 7:
		    pline_msg = "says 'What sort of a sword do you call that!'";
		    break;
	    }
	    break;
	case MS_SQAWK:
	    if (ptr == &mons[PM_RAVEN] && !mtmp->mpeaceful)
	    	verbl_msg = "Nevermore!";
	    else
	    	pline_msg = "squawks.";
	    break;
	case MS_HISS:
	    if (!mtmp->mpeaceful)
		pline_msg = "hisses!";
	    else return 0;	/* no sound */
	    break;
	case MS_BUZZ:
	    pline_msg = mtmp->mpeaceful ? "drones." : "buzzes angrily.";
	    break;
	case MS_GRUNT: /* YANI by ShivanHunter */
	    pline_msg = (mtmp->data->mlet == S_TROLL) ? "shouts: 'VI > EMACS!!!!!'" : "grunts.";
	    break;
	case MS_NEIGH:
	    if (mtmp->mtame < 5)
		pline_msg = "neighs.";
	    else if (moves > EDOG(mtmp)->hungrytime)
		pline_msg = "whinnies.";
	    else
		pline_msg = "whickers.";
	    break;
	case MS_WAIL:
	    pline_msg = "wails mournfully.";
	    break;
	case MS_GURGLE:
	    pline_msg = "gurgles.";
	    break;
	case MS_BURBLE:
	    pline_msg = "burbles.";
	    break;
	case MS_SHRIEK:
	    pline_msg = "shrieks.";
	    aggravate();
	    break;
	case MS_FART_QUIET:
	case MS_FART_NORMAL:
	case MS_FART_LOUD:
		pline("You gently caress %s's %s butt using %s %s.", mon_nam(mtmp), mtmp->female ? "sexy" : "ugly", !rn2(3) ? "both your left and right" : rn2(2) ? "your left" : "your right", body_part(HAND) );
		if (mtmp->mtame) {
			pline("%s seems to love you even more than before.", Monnam(mtmp) );
			if (mtmp->mtame < 30) mtmp->mtame++;
		}
		else if (mtmp->mpeaceful) {
			pline("%s seems to like being felt up by you.", Monnam(mtmp) );
		}
		else {
			pline("%s seems to be even more angry at you than before.", Monnam(mtmp) );
		}
	    m_respond(mtmp);
	    break;
	case MS_IMITATE:
	    pline_msg = "imitates you.";
	    break;
	case MS_SHEEP:
	    pline_msg = "baaaas.";
	    break;
	case MS_CHICKEN:
	    pline_msg = "clucks.";
	    break;
	case MS_COW:
	    pline_msg = "bellows.";
	    break;
	case MS_BONES:
	    pline("%s rattles noisily.", Monnam(mtmp));
	    You("freeze for a moment.");
	    nomul(-2, "scared by rattling");
	    nomovemsg = 0;
	    break;
	case MS_LAUGH:
	    {
		static const char * const laugh_msg[4] = {
		    "giggles.", "chuckles.", "snickers.", "laughs.",
		};
		pline_msg = laugh_msg[rn2(4)];
	    }
	    break;
	case MS_MUMBLE:
	    pline_msg = "mumbles incomprehensibly.";
	    break;
	case MS_DJINNI:
	    if (mtmp->mtame) {
		verbl_msg = "Sorry, I'm all out of wishes.";
	    } else if (mtmp->mpeaceful) {
		if (ptr == &mons[PM_WATER_DEMON])
		    pline_msg = "gurgles.";
		else
		    verbl_msg = "I'm free!";
	    } else verbl_msg = "This will teach you not to disturb me!";
	    break;
	case MS_BOAST:	/* giants */
	    if (!mtmp->mpeaceful) {
		switch (rn2(4)) {
		case 0: pline("%s boasts about %s gem collection.",
			      Monnam(mtmp), mhis(mtmp));
			break;
		case 1: pline_msg = "complains about a diet of mutton.";
			break;
	       default: pline_msg = "shouts \"Fee Fie Foe Foo!\" and guffaws.";
			wake_nearto(mtmp->mx, mtmp->my, 7*7);
			break;
		}
		break;
	    }
	    /* else FALLTHRU */
	case MS_HUMANOID:
	    if (!mtmp->mpeaceful) {
		if (In_endgame(&u.uz) && is_mplayer(ptr)) {
		    mplayer_talk(mtmp);
		    break;
		} else return 0;	/* no sound */
	    }
	    /* Generic peaceful humanoid behaviour. */
	    if (mtmp->mflee)
		pline_msg = "wants nothing to do with you.";
	    else if (mtmp->mhp < mtmp->mhpmax/4)
		pline_msg = "moans.";
	    else if (mtmp->mconf || mtmp->mstun)
		verbl_msg = !rn2(3) ? "Huh?" : rn2(2) ? "What?" : "Eh?";
	    else if (!mtmp->mcansee)
		verbl_msg = "I can't see!";
	    else if (mtmp->mtrapped) {
		struct trap *t = t_at(mtmp->mx, mtmp->my);

		if (t && !t->hiddentrap) t->tseen = 1;
		verbl_msg = "I'm trapped!";
	    } else if (mtmp->mhp < mtmp->mhpmax/2)
		pline_msg = "asks for a potion of healing.";
	    else if (mtmp->mtame && !mtmp->isminion &&
						moves > EDOG(mtmp)->hungrytime)
		verbl_msg = "I'm hungry.";
	    /* Specific monsters' interests */
	    else if (is_elf(ptr))
		pline_msg = "curses orcs.";
	    else if (is_dwarf(ptr))
		pline_msg = "talks about mining.";
	    else if (likes_magic(ptr) && rn2(5) )
		pline_msg = "talks about spellcraft.";
	    else if (ptr->mlet == S_CENTAUR)
		pline_msg = "discusses hunting.";
	    else switch (monsndx(ptr)) {
		case PM_HOBBIT:
		    pline_msg = (mtmp->mhpmax - mtmp->mhp >= 10) ?
				"complains about unpleasant dungeon conditions."
				: "asks you about the One Ring.";
		    break;
#if 0	/* OBSOLETE */
		case PM_FARMER_MAGGOT:
			pline_msg = "mumbles something about Morgoth.";
			break;
#endif
		case PM_ARCHEOLOGIST:
    pline_msg = "describes a recent article in \"Spelunker Today\" magazine.";
		    break;
#ifdef TOURIST
		case PM_TOURIST:
		    verbl_msg = "Aloha.";
		    break;
#endif
		case PM_DEATH_EATER:
	    pline_msg = "talks about hunting muggles.";
		    break;
		case PM_GANGSTER:
	    pline_msg = "talks about doing a drive-by.";
		    break;
		case PM_GEEK:
		    verbl_msg = "Enematzu memalezu!";
		    break;
		case PM_UNBELIEVER:
		    verbl_msg = "Gods are nonexistant. People should stop believing in them.";
		    break;
		case PM_POKEMON:
		    verbl_msg = "Little strawberry me baby!";
		    break;
		case PM_TRANSVESTITE:
		    verbl_msg = "Look at my heels! Do you think they're beautiful?";
		    break;
		case PM_TRANSSYLVANIAN:
		    verbl_msg = "My block-heeled combat boots are very fleecy!";
		    break;
		case PM_TOPMODEL:
		    verbl_msg = "I'm the chosen one who is going to win the ANTM show!";
		    break;
		case PM_FAILED_EXISTENCE:
		    verbl_msg = "Waaaaaah! Life has been so cruel to me!";
		    break;
		case PM_ACTIVISTOR:
		case PM_PEACEMAKER:
		case PM_ACTIVIST:
		    verbl_msg = "We're here to bring peace to the world.";
		    break;
		case PM_BLEEDER:
		    verbl_msg = "*sigh* If only I could make this bleeding stop...";
		    break;
		case PM_COURIER:
	    pline_msg = "talks about some random guy named Benny.";
		    break;
		case PM_ROCKER:
	    pline_msg = "talks about groovy music.";
		    break;

		default:
		    pline_msg = "discusses dungeon exploration.";
		    break;
	    }
	    break;
	case MS_SEDUCE:
#ifdef SEDUCE
	    if (ptr->mlet != S_NYMPH &&
		could_seduce(mtmp, &youmonst, (struct attack *)0) == 1) {
			(void) doseduce(mtmp);
			break;
	    }
	    switch ((poly_gender() != (int) mtmp->female) ? rn2(3) : 0)
#else
	    switch ((poly_gender() == 0) ? rn2(3) : 0)
#endif
	    {
		case 2:
			verbl_msg = "Hello, sailor.";
			break;
		case 1:
			pline_msg = "comes on to you.";
			break;
		default:
			pline_msg = "cajoles you.";
	    }
	    break;
	case MS_ARREST:
	    if (mtmp->mpeaceful)
		verbalize("Just the facts, %s.",
		      flags.female ? "Ma'am" : "Sir");
	    else {
		static const char * const arrest_msg[3] = {
		    "Anything you say can be used against you.",
		    "You're under arrest!",
		    "Stop in the name of the Law!",
		};
		verbl_msg = arrest_msg[rn2(3)];
	    }
	    break;
	case MS_BRIBE:
#ifdef CONVICT        
        if (monsndx(ptr) == PM_PRISON_GUARD) {
            long gdemand = 500 * u.ulevel;
            long goffer = 0;

    	    if (!mtmp->mpeaceful && !mtmp->mtame) {
                pline("%s demands %ld %s to avoid re-arrest.",
                 Amonnam(mtmp), gdemand, currency(gdemand));
                if ((goffer = bribe(mtmp)) >= gdemand) {
                    verbl_msg = "Good.  Now beat it, scum!";
            	    mtmp->mpeaceful = 1;
            	    set_malign(mtmp);
                    break;
                } else {
                    pline("I said %ld!", gdemand);
                    mtmp->mspec_used = 1000;
                    break;
                }
            } else {
                verbl_msg = "Out of my way, scum!"; /* still a jerk */
            }
        } else
#endif /* CONVICT */
	    if (mtmp->mpeaceful && !mtmp->mtame) {
		(void) demon_talk(mtmp);
		break;
	    }
	    /* fall through */
	case MS_CUSS:
	    if (!mtmp->mpeaceful)
		cuss(mtmp);

	    if (mtmp->mpeaceful && monsndx(ptr) == PM_WIZARD_OF_YENDOR && Race_if(PM_RODNEYAN) )	{

			if (mon_has_amulet(mtmp) )	{
				verbalize("Here, take the amulet and use it well, %s!", flags.female ? "sister" : "brother");
				mdrop_special_objs(mtmp);
			}
			else if (mon_has_special(mtmp) )	{
				verbalize("Here %s, I got a nice artifact for you!", flags.female ? "sister" : "brother");
				mdrop_special_objs(mtmp);
			}
			else verbalize("Good luck on your quest, %s!", flags.female ? "sister" : "brother");

		}

	    break;
	case MS_GYPSY:	/* KMH */
		if (mtmp->mpeaceful) {
			gypsy_chat(mtmp);
			break;
		}
		/* fall through */
	case MS_SPELL:
	    /* deliberately vague, since it's not actually casting any spell */
	    pline_msg = "seems to mutter a cantrip.";
	    break;
	case MS_NURSE:

	    {
		int nursehpcost = u.nurseextracost;
		if (Upolyd) nursehpcost /= 5;
	
		if (u.ugold >= nursehpcost) {
			verbalize("I can inject extra health into you for %d dollars if you want.", nursehpcost);
			if (yn("Accept the offer?") == 'y') {
				verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
				u.ugold -= nursehpcost;
				if (!Upolyd) u.uhpmax++;
				else u.mhmax++;
				u.nurseextracost += 50;
				if (u.nurseextracost < 1000) u.nurseextracost = 1000; /* fail safe */
			}
		}

	    else if (uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == CHAIN_CLASS || is_weptool(uwep))
		|| (u.twoweap && uswapwep && (uswapwep->oclass == WEAPON_CLASS
		|| is_weptool(uswapwep))))
		verbl_msg = "Put that weapon away before you hurt someone!";
	    else if (uarmc || uarm || uarmh || uarms || uarmg || uarmf)
		verbl_msg = (Role_if(PM_HEALER) || Race_if(PM_HERBALIST)) ?
			  "Doc, I can't help you unless you cooperate." :
			  "Please undress so I can examine you.";
#ifdef TOURIST
	    else if (uarmu)
		verbl_msg = "Take off your shirt, please.";
#endif
	    else verbl_msg = "Relax, this won't hurt a bit.";

	    }

	    break;
	case MS_GUARD:
#ifndef GOLDOBJ
	    if (u.ugold)
#else
	    if (money_cnt(invent))
#endif
		verbl_msg = "Please drop that gold and follow me.";
	    else
		verbl_msg = "Please follow me.";
	    break;
	case MS_SOLDIER:
	    {
		static const char * const soldier_foe_msg[3] = {
		    "Resistance is useless!",
		    "You're dog meat!",
		    "Surrender!",
		},		  * const soldier_pax_msg[3] = {
		    "What lousy pay we're getting here!",
		    "The food's not fit for Orcs!",
		    "My feet hurt, I've been on them all day!",
		};
		verbl_msg = mtmp->mpeaceful ? soldier_pax_msg[rn2(3)]
					    : soldier_foe_msg[rn2(3)];
	    }
	    break;
	case MS_RIDER:
	    if (ptr == &mons[PM_DEATH] && !rn2(10))
		pline_msg = "is busy reading a copy of Sandman #8.";
	    else verbl_msg = "Who do you think you are, War?";
	    break;

	case MS_VICE:
	    switch(monsndx(ptr)) {
		case PM_GLUTTONY:
		    pline_msg = rn2(2) ? "gobbling noisily." :	/*[Sakusha]*/
				"mumbles through a mouthful of food.";
		    break;
		case PM_LUST:
		    pline_msg = "breathes heavily...";
		    break;
		case PM_GREED:
		    verbl_msg = "Mine! All mine!";
		    break;
		case PM_ENVY:
#ifndef GOLDOBJ
		    if(u.ugold)
#else
		    if(money_cnt(invent))
#endif
			verbl_msg = "Give me that gold!";
		    else {
			pline_msg = "stares at you jealously.";
		    }
		    break;
		case PM_WRATH:
		    pline_msg = "glares at you ferociously.";
		    break;
		case PM_SLOTH:
		    pline_msg = "sighs.";
		    break;
		case PM_DEPRESSION:
		    pline_msg = "moans in agony.";
		    break;
		case PM_PRIDE:
		    pline_msg = "doesn't deign to reply.";
		    break;
		default:
		    verbl_msg = "Think you're innocent and pure? That can be changed!";
		    break;
	    }
	    break;

	case MS_DOUGLAS_ADAMS:
            {
	   	 static const char *da_msgs[] = {
			"42",
			"It's a nice day today!",
			"Have a Pan Galactic Gargle Blaster?",
			"Time is relative. Lunchtime doubly so.",
			"This is some strange usage of the word 'safe' that I wasn't aware of.",
		};
		if (mtmp->data == &mons[PM_MARVIN]) {
			verbalize("Life, loathe it or ignore it, you cannot like it.");
		} else if (mtmp->data == &mons[PM_DEEP_THOUGHT]) {
			verbalize("6*9 = 42");
		} else if (mtmp->data == &mons[PM_EDDIE]) {
			verbalize("I'm feeling just great, guys!");
		} else {
			verbalize(da_msgs[rn2(SIZE(da_msgs))]);
		}
	    }
	    break;
	case MS_PUPIL:

		{
		static const char *pupil_msgs[] = {
			"Today no homework ... *please*",
			"six times nine is ... um ... uh ... ... forty-two",
			"you ... Strange word",	/* You're not expected to understand this ... */
			"Bugger off!",
			"*uck off!",
			"What are the newest news about the Super Information Highway?",
			"What do you want?",
			"Do the world a favour---jump out of the 20th story of the Uni-Center!",
		};

		verbalize(pupil_msgs[rn2(SIZE(pupil_msgs))]);
		}
		break;

	case MS_WHORE:

		{
		static const char *whore_msgs[] = { /* These are obviously inspired by the GTA series --Amy */
			"Come to Momma.",
			"I'm gonna call my pimp.",
			"You picked the wrong bitch.",
			"You're just another sorry ass!",
			"Hey, this is my first night!",
			"You know, I used to be a marine before the OP.", /* Vice City */
			"Cocksucker!",
			"I'll kick your nuts flat!", /* yes the PC can be female, but who cares? */
			"I'm no slut, I just need the money!",
			"I'll be sitting on you in a second.",
			"You think I can't mess you up?",
			"Die in a pool of your own blood.",
			"Get ready for an ass-kicking.",
			"You want me to whoop you?",
			"You want some? I'll give you some!",
			"Enjoy this stiletto.",
			"If I don't kill you, my parents will.",
			"I know kickboxing.",
			"I'm a black belt in karate.",
			"My hands are lethal weapons.",
			"I'll kick your teeth in.",
			"Would you really hit a woman?",
			"I've killed hundreds of pigs like you!",
			"I'm gonna open up a can of whoopass on you now!",
			"Girls can fight too!",
			"Beating on girls, right?",
			"I have no problem kicking you in the nuts.",
			"I'll slap you silly.",
			"My pimp will take care of you.",
			"You're messing with an angry bitch!",
			"Another asshole with a problem!",
			"You think cause I'm a girl I can't fight?",
			"You call that 'fighting'?",
			"I'm gonna stomp your balls!",
			"I'm a lady but I can fight.",
			"I'm an innocent virgin!",
			"You just made me break a nail!",
			"I'm expecting an apology!",
			"You insult my womanhood.",
			"You disgust me.",
		};

		if (!mtmp->mpeaceful) verbalize(whore_msgs[rn2(SIZE(whore_msgs))]);
		}
		break;

	case MS_SUPERMAN:

		if (!mtmp->mpeaceful) {
		static const char *superman_msgs[] = {
			"FRONTAL ATTACK!!!",
			"YOU DON'T STAND A CHANCE!",
			"YOU WILL DIE A SLOW, SLOW, DEATH...",
			"COME OUT! WE WILL HURT YOU!",
			"GRRRRRRRRRAAAAAAAAAAHHH!",
			"CHARRRRRRRRRGE!",
			"FEAR ME!!!",
			"DIE YOU SON OF A BITCH!", /* too lazy to check for female PC --Amy */
			"I AM YOUR DOOM!",
			"YOUR LIFE IS GONNA END NOW!",
			"YOU WILL CEASE TO EXIST!",
			"I'M GOING TO EAT YOU!",
			"RAAAAAAAAAAAARGH!",
			"ATTACK PATTERN ALPHA!",
			"YOU CAN'T HIDE!",
			"THERE'S NO ESCAPE!",
			"BE AFRAID OF ME!",
			"ATTAAAAAAAAAAAAACK!",
		};

		verbalize(superman_msgs[rn2(SIZE(superman_msgs))]);
		badeffect();
		}
		break;

	case MS_TEACHER:
		{
	   	 static const char *teacher_msgs[] = {
			"No chance! Every day you'll get homework!",
			"Is it really true? Does really _everybody_ have the homework?",
			"That usage of the word 'goes' does harm to my ears!",
			"Your attitude is really unacceptable!",
			"The \"Stigel-Brauerei\" was founded 1492. Well, in that year was that affair with that guy, Columbus, but that really isn't important.",
			"Why are you going? I'm only 20 minutes late!",
			"Where's your problem? I'll be happy to help you",
			"You didn't understand? Then let's begin again ... (*sigh*)",
			"No homework yet? - This can be changed!",
			"Overbecks - das Ueberbier",
			"How about dehydrating carbonhydrates today?",
			"Back when I was a pupil, the following thing happened ...",
			"Back when I was studying chemistry, the following thing happened ...",
			"... dann ist die Scheisse am dampfen",
			"NIKI forever!",
			"Pascal forever!",
			"Yes ... I know that everything is easier in C, but I simply love Pascal ...",
			"You have Str:0 (at most), so bugger off!",
			"Do it - who cares about the odd broken bone?",
			"You are sick because you were running for 30 minutes? So run another hour!",
			"Shall I help you? (takes the whip)",
			"We'll do that diagonally. *grin* (wipes sweat off head)",
			"*grin*",
			"You know, (*grin*) we'll have to do something now! (*grin*)",
			"How about a pupil experiment - cut off your ears?",
			"Yet another pupil experiment: the consequences of KCN ingested.",
			"Don't expect to get away without homework!",
			"No homework in the holidays? You must have something to do, after all!",
			"The low level of you all is really beyond acception!",
			"There was endless work in the supervision and administration of the farm ...",
			/* it's really a shame that I can't think of more messages for him */
			"I expect you to do your homework _regularly_ and _carefully_!",
			"The level of work is really very low nowadays!",
			"In _our_ times pupils were real pupils and teachers were real teachers!",
			"Back when pupils where real pupils and teachers were real teachers, everything was better!",
		};

		verbalize(teacher_msgs[rn2(SIZE(teacher_msgs))]);

		}
		break;

	case MS_PRINCIPAL:

		{
		static const char *principal_msgs[] = {
		"What's up?",
		"I really feel sick - there are so many things to do!",
		"Help me, I faint!",
		"We'll do that in groups of one person!",
		};

		verbalize(principal_msgs[rn2(SIZE(principal_msgs))]);
		}
		break;

	case MS_SMITH:
		verbalize("I'm working. Please don't disturb me again!");
		break;

    }

    if (pline_msg) pline("%s %s", Monnam(mtmp), pline_msg);
    else if (verbl_msg) verbalize(verbl_msg);
    return(1);
}


int
dotalk()
{

	if (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone()) {
	pline("The chat command is currently unavailable!");
	display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

    int result;
    boolean save_soundok = flags.soundok;
    if (!Deafness && !u.uprops[DEAFNESS].extrinsic && !have_deafnessstone() ) flags.soundok = 1;	/* always allow sounds while chatting */
    result = dochat();
    flags.soundok = save_soundok;
    return result;
}

static int
dochat()
{
    register struct monst *mtmp;
    register int tx,ty;
    struct obj *otmp;

    if (is_silent(youmonst.data)) {
	pline("As %s, you cannot speak.", !missingnoprotect ? an(youmonst.data->mname) : "this weird creature");
	display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }
    if (Strangled) {
	You_cant("speak.  You're choking!");
	display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }
    if (u.uswallow) {
	pline("They won't hear you out there.");
	display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }
    if (Underwater) {
	Your("speech is unintelligible underwater.");
	display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }

    if (!Blind && (otmp = shop_object(u.ux, u.uy)) != (struct obj *)0) {
	/* standing on something in a shop and chatting causes the shopkeeper
	   to describe the price(s).  This can inhibit other chatting inside
	   a shop, but that shouldn't matter much.  shop_object() returns an
	   object iff inside a shop and the shopkeeper is present and willing
	   (not angry) and able (not asleep) to speak and the position contains
	   any objects other than just gold.
	*/
	price_quote(otmp);
	return(1);
    }

    if (!getdir("Talk to whom? (in what direction)")) {
	/* decided not to chat */
	return(0);
    }

#ifdef STEED
    if (u.usteed && u.dz > 0)
	return (domonnoise(u.usteed));
#endif
    if (u.dz) {
	pline("They won't hear you %s there.", u.dz < 0 ? "up" : "down");
	return(0);
    }

    if (u.dx == 0 && u.dy == 0) {
/*
 * Let's not include this.  It raises all sorts of questions: can you wear
 * 2 helmets, 2 amulets, 3 pairs of gloves or 6 rings as a marilith,
 * etc...  --KAA
	if (u.umonnum == PM_ETTIN) {
	    You("discover that your other head makes boring conversation.");
	    return(1);
	}
*/
	pline("Talking to yourself is a bad habit for a dungeoneer.");
	return(0);
    }

    tx = u.ux+u.dx; ty = u.uy+u.dy;
    mtmp = m_at(tx, ty);

    if (!mtmp || mtmp->mundetected ||
		mtmp->m_ap_type == M_AP_FURNITURE ||
		mtmp->m_ap_type == M_AP_OBJECT)
	return(0);

    /* sleeping monsters won't talk, except priests (who wake up) */
    if ((!mtmp->mcanmove || mtmp->msleeping) && !mtmp->ispriest) {
	/* If it is unseen, the player can't tell the difference between
	   not noticing him and just not existing, so skip the message. */
	if (canspotmon(mtmp))
	    pline("%s seems not to notice you.", Monnam(mtmp));
	return(0);
    }

    /* if this monster is waiting for something, prod it into action */
    mtmp->mstrategy &= ~STRAT_WAITMASK;

    if (mtmp->mtame && mtmp->meating) {
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);
	pline("%s is eating noisily.", Monnam(mtmp));
	return (0);
    }

#ifdef CONVICT
    if (Role_if(PM_CONVICT) && is_rat(mtmp->data) && !mtmp->mpeaceful &&
     !mtmp->mtame) {
        You("attempt to soothe the %s with chittering sounds.",
         l_monnam(mtmp));
        if (rnl(10) < 2) {
            (void) tamedog(mtmp, (struct obj *) 0, FALSE);
        } else {
            if (rnl(10) > 8) {
                pline("%s unfortunately ignores your overtures.",
                 Monnam(mtmp));
                return 0;
            }
            mtmp->mpeaceful = 1;
            set_malign(mtmp);
        }
        return 0;
    }
#endif /* CONVICT */

    if ( (Role_if(PM_FEAT_MASTER) || Race_if(PM_VORTEX) || Race_if(PM_CORTEX)) && mtmp->data->mlet == S_VORTEX && !mtmp->mtame && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) ) {

		if (yn("Tame the vortex?") == 'y') {

	      pline("You attempt to tame %s.",mon_nam(mtmp) );

		(void) tamedog(mtmp, (struct obj *) 0, FALSE);

	        return 1;
		}
	}

    if (Race_if(PM_KOP) && mtmp->data->mlet == S_KOP && mtmp->mpeaceful && !mtmp->mtame && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) && u.uhunger > 100 ) {

		if (yn("Recruit this officer of the law?") == 'y') {

	      pline("You convince %s to join your cause.",mon_nam(mtmp) );
		morehungry(100);

		(void) tamedog(mtmp, (struct obj *) 0, TRUE);

	        return 1;
		}
	}

    if (Role_if(PM_LADIESMAN) && !flags.female && (mtmp->data->mlet == S_NYMPH || mtmp->data->msound == MS_NURSE || mtmp->data->msound == MS_SEDUCE || mtmp->data->msound == MS_WHORE) && !mtmp->mtame && !mtmp->mpeaceful && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) ) {

		if (yn("Seduce this pretty lady?") == 'y') {

	      pline("You attempt to seduce %s.",mon_nam(mtmp) );

            mtmp->mpeaceful = 1; /* they will always become at least peaceful. --Amy */
            set_malign(mtmp);

		if (mtmp->m_lev < rnd(50) && rn2(u.ulevel + 2) && rn2(3) ) /* higher level monsters are less likely to be affected --Amy*/

		(void) tamedog(mtmp, (struct obj *) 0, TRUE);

	        return 1;
		}
	}

    if (Race_if(PM_IMPERIAL) && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) && !mtmp->mpeaceful &&
     !mtmp->mtame && u.uhunger > 100) {

		if (yn("Use the Emperor's Voice?") == 'y') {

	      verbalize("%s", !rn2(3) ? "By the power of His Holiness Titus Medes, I beseech thee - stop thine combat actions!" : !rn2(2) ? "Long live Martin Septim! Thou shall surrender lest I smite thee!" : "The Emperor will spare thy life if thou stoppest fighting!");
		morehungry(100);

		if (mtmp->m_lev < rnd(50) && rn2(u.ulevel + 2) && rn2(3) ) { /* higher level monsters are less likely to be affected --Amy*/

            mtmp->mpeaceful = 1;
            set_malign(mtmp);
		pline("%s is convinced by your sermon, and no longer views you as an enemy!",l_monnam(mtmp));
		}

        return 1;

		}
    }

    if (Race_if(PM_MUMMY) && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) &&
     !mtmp->mtame && (is_undead(mtmp->data) || mtmp->egotype_undead) && u.uhunger > 500) {

		if (yn("Try to tame this undead monster?") == 'y') {

	      pline("You frantically chant at %s.",mon_nam(mtmp) );
		morehungry(500);

		if (mtmp->m_lev < rnd(100) && rn2(u.ulevel + 2) && rn2(3)) { /* higher level monsters are less likely to be affected --Amy*/

	    /*maybe_tame(mtmp, sobj);*/
		(void) tamedog(mtmp, (struct obj *) 0, FALSE);
		}

        return 1;

		}
    }


    return domonnoise(mtmp);
}

#ifdef USER_SOUNDS

extern void FDECL(play_usersound, (const char*, int));

typedef struct audio_mapping_rec {
#ifdef USER_SOUNDS_REGEX
	struct re_pattern_buffer regex;
#else
	char *pattern;
#endif
	char *filename;
	int volume;
	struct audio_mapping_rec *next;
} audio_mapping;

static audio_mapping *soundmap = 0;

char* sounddir = ".";

/* adds a sound file mapping, returns 0 on failure, 1 on success */
int
add_sound_mapping(mapping)
const char *mapping;
{
	char text[256];
	char filename[256];
	char filespec[256];
	int volume;

	if (sscanf(mapping, "MESG \"%255[^\"]\"%*[\t ]\"%255[^\"]\" %d",
		   text, filename, &volume) == 3) {
	    const char *err;
	    audio_mapping *new_map;

	    if (strlen(sounddir) + strlen(filename) > 254) {
		raw_print("sound file name too long");
		return 0;
	    }
	    Sprintf(filespec, "%s/%s", sounddir, filename);

	    if (can_read_file(filespec)) {
		new_map = (audio_mapping *)alloc(sizeof(audio_mapping));
#ifdef USER_SOUNDS_REGEX
		new_map->regex.translate = 0;
		new_map->regex.fastmap = 0;
		new_map->regex.buffer = 0;
		new_map->regex.allocated = 0;
		new_map->regex.regs_allocated = REGS_FIXED;
#else
		new_map->pattern = (char *)alloc(strlen(text) + 1);
		Strcpy(new_map->pattern, text);
#endif
		new_map->filename = strdup(filespec);
		new_map->volume = volume;
		new_map->next = soundmap;

#ifdef USER_SOUNDS_REGEX
		err = re_compile_pattern(text, strlen(text), &new_map->regex);
#else
		err = 0;
#endif
		if (err) {
		    raw_print(err);
		    free(new_map->filename);
		    free(new_map);
		    return 0;
		} else {
		    soundmap = new_map;
		}
	    } else {
		Sprintf(text, "cannot read %.243s", filespec);
		raw_print(text);
		return 0;
	    }
	} else {
	    raw_print("syntax error in SOUND");
	    return 0;
	}

	return 1;
}

void
play_sound_for_message(msg)
const char* msg;
{
	audio_mapping* cursor = soundmap;

	while (cursor) {
#ifdef USER_SOUNDS_REGEX
	    if (re_search(&cursor->regex, msg, strlen(msg), 0, 9999, 0) >= 0) {
#else
	    if (pmatch(cursor->pattern, msg)) {
#endif
		play_usersound(cursor->filename, cursor->volume);
	    }
	    cursor = cursor->next;
	}
}

#endif /* USER_SOUNDS */

#endif /* OVLB */

/*sounds.c*/

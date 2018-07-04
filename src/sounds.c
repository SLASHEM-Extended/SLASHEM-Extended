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

static int domonnoise(struct monst *);
static int dochat(void);
static const char *yelp_sound(struct monst *);
static const char *whimper_sound(struct monst *);

#endif /* OVLB */

#ifdef OVL0

#ifdef DUMB
static int mon_in_room(struct monst *,int);

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

	    if (level.flags.has_cryptroom && !rn2(200)) {
		static const char *cryptroom_msg[] = {
			"smell rotting flesh.",
			"hear shuffling noises.",
			"hear a loud moan.",
			"feel that you're near death!",
			"don't have much longer to live...",
		};
		You(cryptroom_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_troublezone && !rn2(200)) {
		static const char *troublezone_msg[] = {
			"smell trouble.",
			"feel that something's gonna happen.",
			"sense several evil presences.",
			"have to count to three, and if you fail, you can forget about your game!",
			"are way in over your head!",
		};
		You(troublezone_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_weaponchamber && !rn2(200)) {
		static const char *weaponchamber_msg[] = {
			"hear metallic clashes.",
			"are listening to someone practicing their sword skill.",
			"hear a trumpet, followed by attack orders.",
			"are ambushed by shadowy figures with gleaming swords!",
			"should impale yourself on your weapon while you still can...",
		};
		You(weaponchamber_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_hellpit && !rn2(200)) {
		static const char *hellpit_msg[] = {
			"smell brimstone.",
			"hear the bubbling of lava.",
			"chime in on a pack of demons performing a satanic ritual.",
			"feel that Mehrunes Dagon will open the Great Gate of Oblivion soon!",
			"realize that the hellspawn invasion has already begun...",
		};
		You(hellpit_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_feminismroom && !rn2(200)) {
		static const char *feminismroom_msg[] = {
			"hear a female voice shouting angrily.",
			"listen to a long, squeaking sound.",
			"can hear the clacking noises of high heels walking on a hard floor.",
			"fear that some stupid woman is gonna slap you silly!",
			"can't resist thinking of a pair of brown leather boots with black block heels, and start wanking off to the thought.",
		};
		You(feminismroom_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_meadowroom && !rn2(200)) {
		static const char *meadowroom_msg[] = {
			"feel a slight breeze.",
			"hear crickets chirping.",
			"can hear the cattle bellowing.",
			"hear someone sing 'Old Mac Donald had a farm...'",
			"can't help it but feel that something that looks normal is terribly amiss.",
		};
		You(meadowroom_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_coolingchamber && !rn2(200)) {
		static const char *coolingchamber_msg[] = {
			"are chilled by cold air.",
			"shiver for a moment.",
			"feel that someone forgot to close the door to the freezer.",
			"fear that you're gonna develop a lung inflammation.",
			"hear someone calling you to the iceblock shooting.",
		};
		You(coolingchamber_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_voidroom && !rn2(200)) {
		static const char *voidroom_msg[] = {
			"are struck by a sense of emptiness.",
			"sense a black hole in the fabric of reality.",
			"feel that a special challenge awaits you.",
			"hear Galadriel whispering 'It is very likely that you die on your journey, but you must go anyway...'",
			"realize that the atmosphere does not contain oxygen! If you stay around you'll suffocate!",
		};
		You(voidroom_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_hamletroom && !rn2(200)) {
		static const char *hamletroom_msg[] = {
			"have a sense of familiarity.",
			"feel relaxed.",
			"hear a storeclerk making a transaction.",
			"feel like returning to your burned home... wait, is there really a reason for doing so?",
			"hope that you don't have to master the fleeing mine...",
		};
		You(hamletroom_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_kopstation && !rn2(200)) {
		static const char *kopstation_msg[] = {
			"hear a loud siren.",
			"can hear a police officer requesting reinforcement.",
			"hear the whipping sound of a rubber hose.",
			"realize that your cop wanted level is 6, and the forces of law are coming to bust you!",
			"are chased by a speeding police car!",
		};
		if (rn2(5)) You(kopstation_msg[rn2(3+hallu*2)]);
		else verbalize("Alert! All units, apprehend %s immediately!", plname);
		return;
	    }

	    if (level.flags.has_bossroom && !rn2(200)) {
		static const char *bossroom_msg[] = {
			"feel that you're in a hopeless situation.",
			"are left hanging in suspense.",
			"feel that the future is dark, and it gets even darker.",
			"realize that the challenge of your life awaits you.",
			"get the feeling that a true champion is going to challenge you soon.",
			"will not be able to defeat Whitney's Miltank, because the fleecy Whitney will mop the floor with you wimp!",
			"are not going to get past Pryce, since his ice pokemon can crash the game at will.",
			"have to fight Lance and his Dragonite, but with your uselessly weak pokemon you don't stand a chance.",
			"loser will never beat Gary, whose Meganium always uses 'Contro' and defeats each of your pokemon in one hit, hahaha!",
		};
		You(bossroom_msg[rn2(5+hallu*4)]);
		return;
	    }

	    if (level.flags.has_rngcenter && !rn2(200)) {
		static const char *rngcenter_msg[] = {
			"sense the shuffling of random numbers.",
			"hear the dungeon master testing a set of dice.",
			"feel that this particular floor is especially random.",
			"just know that the monsters on this level have the most evil and unbalanced attacks possible.",
			"test your d20 several times, only to realize in shock that it always rolls 1!",
		};
		You(rngcenter_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_wizardsdorm && !rn2(200)) {
		static const char *wizardsdorm_msg[] = {
			"hear a grating, annoying snore.",
			"smell dragonbreath in the air.",
			"hear a 'Klieau!' sound.",
			"are hit by the 1st tailspike! You are hit by the 2nd tailspike! Warning: HP low! You are hit by the 3rd tailspike! You die. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
			"want to do wizard farming like in the old days, since it's more profitable than pudding farming.",
		};
		You(wizardsdorm_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_doomedbarracks && !rn2(200)) {
		static const char *doomedbarracks_msg[] = {
			"hear a sound reminiscent of a frog.",
			"feel that the gate to Hell has to be closed from the other side.",
			"realize that the aliens have built a base here.",
			"hear a deep voice chanting the name 'Jason'...",
			"hear someone shout 'Neau!'",
		};
		You(doomedbarracks_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_sleepingroom && !rn2(200)) {
		static const char *sleepingroom_msg[] = {
			"felt like you heard a faint noise, but can't make out what it is.",
			"hear a loud yawn.",
			"get the impression that the current atmosphere is quiet... too quiet.",
			"listen to a soothing melody and a chant that sounds like 'Sleep-bundle-wing!'",
			"suddenly fall asleep. (Hopefully no monster decides to feast on your exposed flesh before you wake up.)",
		};
		You(sleepingroom_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_diverparadise && !rn2(200)) {
		static const char *diverparadise_msg[] = {
			"inhale the scent of seawater.",
			"listen to fishes splashing water around.",
			"listen to the sound of gentle waves.",
			"hear a 'splash splash' noise!",
			"hear someone announce 'The entrance to Diver's Paradise is free today! Come on over and enjoy the most beautiful terrain in this dungeon - water!'",
		};
		You(diverparadise_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_menagerie && !rn2(200)) {
		static const char *menagerie_msg[] = {
			"can hear animal sounds.",
			"hear the sound of a caged animal knocking at the lattice.",
			"hear the roar of hungry zoo animals.",
			"hear a hissing sound, and it's right behind you!",
			"look behind you and see a three-headed monkey!",
		};
		You(menagerie_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_emptydesert && !rn2(200)) {
		static const char *emptydesert_msg[] = {
			"feel that the air is hot around here.",
			"feel a lack of humidity.",
			"hear the roaring of a tornado.",
			"seem to sink into the floor!",
			"encounter a sweet asian girl, but as you move to greet her, she suddenly thrusts you into quicksand! You die. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
		};
		You(emptydesert_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_rarityroom && !rn2(200)) {
		static const char *rarityroom_msg[] = {
			"hear uncommon noises.",
			"can feel the earth shaking.",
			"sense a supernatural presence.",
			"know that you'll see something not meant for mortal eyes.",
			"hear Pale Night tempting you to remove her veil and look at what's underneath...",
		};
		You(rarityroom_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_prisonchamber && !rn2(200)) {
		static const char *prisonchamber_msg[] = {
			"feel that you must free someone who's being held captive.",
			"hear someone calling for you.",
			"must free the prisoner unless you want him to be executed.",
			"were too slow and now the prisoner is dead. You failure!",
			"really have to hurry - if you don't free the prisoner in 10 turns, he will be killed!",
		};
		You(prisonchamber_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_nuclearchamber && !rn2(200)) {
		static const char *nuclearchamber_msg[] = {
			"realize that your Geiger counter has started ticking like mad.",
			"smell the stench of contamination in the air.",
			"listen to scientists talking about their experiments.",
			"feel the presence of the ether generator!",
			"can overhear the president saying 'That was the last straw! I'll bomb that fucking country now! I just need to get the code and press the big red button!'",
		};
		You(nuclearchamber_msg[rn2(3+hallu*2)]);
		return;
	    }

	    if (level.flags.has_variantroom && !rn2(200)) {
		static const char *variantroom_msg[] = {
			"sense the presence of beings from another universe.",
			"hear the elder priest fiercely lashing his tentacles around.",
			"feel an earthquake-like trembling, probably caused by a migmatite horror...",
			"hear Morgoth muttering a chant.",
			"hear the scream of Lina Inverse's high-pitched voice.",
			"receive a stamped scroll from the mail demon! It reads: 'Muahahahaha, I kidnapped Princess Peach again. Sincerely, Bowser.'",
			"start eating the hill giant corpse. Ulch - that meat was tainted! You die from your illness. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
		};
		You(variantroom_msg[rn2(5+hallu*2)]);
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
		static const char *insideroom_msg[21] = {
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
			"see an error message pop up: The win16 subsystem has insufficient resources to continue running. Click on OK, close your application and restart your machine.",
			"see an error message pop up: System resources depleted. Please restart your computer.",
		};
		You(insideroom_msg[rn2(3+hallu*18)]);
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
	if (flags.run) nomul(0, 0, FALSE);
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
	char buf[BUFSZ];

	int armpro, armprolimit;

    if (mtmp->egotype_farter) {

		if (uleft && objects[(uleft)->otyp].oc_material == INKA) {
			verbalize("Let me take off that ring for you.");
			remove_worn_item(uleft, TRUE);
		}
		if (uright && objects[(uright)->otyp].oc_material == INKA) {
			verbalize("Let me take off that ring for you.");
			remove_worn_item(uright, TRUE);
		}

		pline("You gently caress %s's %s butt using %s %s.", mon_nam(mtmp), mtmp->female ? "sexy" : "ugly", !rn2(3) ? "both your left and right" : rn2(2) ? "your left" : "your right", body_part(HAND) );
		if (mtmp->mtame) {
			pline("%s seems to love you even more than before.", Monnam(mtmp) );
			if (mtmp->mtame < 30) mtmp->mtame++;
			if (EDOG(mtmp)->abuse) EDOG(mtmp)->abuse--;
		}
		else if (mtmp->mpeaceful) {
			pline("%s seems to like being felt up by you.", Monnam(mtmp) );
		}
		else if (uarmf && uarmf->oartifact == ART_SPEAK_TO_OJ) {
			pline("%s is charmed by your stroking units.", Monnam(mtmp) );
			mtmp->mpeaceful = 1;
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
				sprintf(verbuf, "Good %s to you Master%s",
					isnight ? "evening" : "day",
					isnight ? "!" : ".  Why do we not rest?");
				verbl_msg = verbuf;
		    	} else {
		    	    sprintf(verbuf,"%s%s",
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
				sprintf(verbuf, "Good feeding %s!",
	    				flags.female ? "sister" : "brother");
				verbl_msg = verbuf;
 			} else if (nightchild && isnight) {
				sprintf(verbuf,
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
			    sprintf(verbuf, "%s! Your silver sheen does not frighten me!",
					youmonst.data == &mons[PM_SILVER_DRAGON] ?
					"Fool" : "Young Fool");
			    verbl_msg = verbuf; 
			} else {
			    vampindex = rn2(SIZE(vampmsg));
			    if (vampindex == 0) {
				sprintf(verbuf, vampmsg[vampindex], body_part(BLOOD));
	    			verbl_msg = verbuf;
			    } else if (vampindex == 1) {
				sprintf(verbuf, vampmsg[vampindex],
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

		if (uleft && objects[(uleft)->otyp].oc_material == INKA) {
			verbalize("Let me take off that ring for you.");
			remove_worn_item(uleft, TRUE);
		}
		if (uright && objects[(uright)->otyp].oc_material == INKA) {
			verbalize("Let me take off that ring for you.");
			remove_worn_item(uright, TRUE);
		}

		pline("You gently caress %s's %s butt using %s %s.", mon_nam(mtmp), mtmp->female ? "sexy" : "ugly", !rn2(3) ? "both your left and right" : rn2(2) ? "your left" : "your right", body_part(HAND) );
		if (mtmp->mtame) {
			pline("%s seems to love you even more than before.", Monnam(mtmp) );
			if (mtmp->mtame < 30) mtmp->mtame++;
		}
		else if (mtmp->mpeaceful) {
			pline("%s seems to like being felt up by you.", Monnam(mtmp) );
		}
		else if (uarmf && uarmf->oartifact == ART_SPEAK_TO_OJ) {
			pline("%s is charmed by your stroking units.", Monnam(mtmp) );
			mtmp->mpeaceful = 1;
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
	    pline(Hallucination ? "%s plays the xylophone!" : "%s rattles noisily.", Monnam(mtmp));
	    You("freeze for a moment.");
	    nomul(-2, "scared by rattling", TRUE);
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
	case MS_SHOE:
	    if (mtmp->mtame) {
		if (u.usteed == mtmp)
			verbl_msg = "Yes! Please! Keep wearing us and we'll take you to the end of the world and beyond!";
		else
			verbl_msg = "Come on my dear, find someone whom we can kick to death!";
	    } else if (mtmp->mpeaceful) {
		if (u.usteed == mtmp)
			verbl_msg = "You're not our owner, but we allow you to wear us for a while because we like you.";
		else
			verbl_msg = "We are beautiful, in every single way!";
	    } else {
		if (u.usteed == mtmp)
			verbl_msg = "Okay, you may wear us. But as soon as you take us off, we'll teach you a lesson in pain.";
		else
			verbl_msg = "You don't need to fight. Just stand there and we'll cause soothing pain to you.";
	    }
	    break;
	case MS_STENCH:

		if (Role_if(PM_HUSSY)) {
			You("joyously inhale %s's scentful perfume. It's very soothing.", mon_nam(mtmp));
		} else {
			pline("Inhaling %s's scentful perfume is not the brightest idea.", mon_nam(mtmp));
			badeffect();
		}

	    break;

	case MS_CONVERT:

		if (mtmp->mpeaceful) {
			if (flags.female) verbl_msg = "Selamlar, kiz kardes.";
			else verbl_msg = "Selamlar, erkek kardes.";

			break;

		} else {

			static const char *conversion_msgs[] = {
				"Kafir! Yuece Tanri sizi sevmez, ve sonra doenuestuermek yoksa cezalandirilacaktir!",
				"Kafir, Allah'a doenuestuermek ya da oel!",
				"Kafir, kutsal Tanri bu boelgede emanetler en kadirdir. Ve bunu degistirmek olmaz!",
				"Kafir, sizin yollariniz sapkin vardir! Islam sizi doenuestuermek zamanidir.",
				"Kafir, sen domuz pirzola yeme suc islemis! Allah sana cok kizgin!",
				"Allahu Akbar! Allahu Akbar!",
				"Kafir, Allah bueyuektuer ve seni yok eder!",
				"Kafir! Kac kere zaten gavur dedin? Sen inanc degistirmek icin ya da Tanri ilahi ceza kesin olacak var!",
				"Allah senin gibi kafirler sevmez ve cok zalim bir sekilde sizi cezalandiracaktir!",
				"Bizim kutsal cami kafirler tarafindan yerle bir olma tehlikesiyle karsi karsiya! Bueyuek Tanri bize yardim ve ilahi asker goendermesi gerekir!",
				"Kafir, Allah bueyuek ve gueclue oldugunu! Ona inanmaya baslarlar yoksa, aci olacak!",
				"Allah onlari oelduererek ve kutsal ateste kendi cesetlerini yakarak buetuen kafirleri cezalandiracaktir.",
				"Kafir, Allah beni tuvalete bok sesleri ueretmek icin izin! Eger o saygi yoksa sonsuza aci olacaktir!",
				"Tek bir gercek dindir ve Tuerkiye cumhurbaskani, onu buetuen duenyaya yayacak bilge bir adamdir! Allah bueyuek!",
				"Kafir! Kutsal topraklarda hicbir sey kaybetmedin! ABD'ye ya da nereye giderseniz gidin!",
				"Inanilmaz olan, senin evin benim tarafimdan yere serilecek.",
				"Inanilmaz, nihayet askerlerini uelkemden mi cekiyorsun yoksa sinirlenmek zorunda miyim?",
				"Inanmasiz, silahim seni vurmak uezere! Sana daha iyi bakarim!",
				"Haha, sen inandin, saldirmami kaldiramazsin!",
				"Inanli olmayan, sana simdi pusu kurup seni uyariyorum. Yani beni yine de durdurma sansin yok.",
				"Kafir! Hemen buradan cik, buguen camiye gitmem ve dua etmeliyim!",
				"Kafir! seni ve iman eden askerlerinizi yok edecegiz!",
				"Inanilmazsin, benden uzak dur yoksa bir talihsizlik olur!",
				"Inkar edenler sadece korkaklar, asla kendinizi adil bir doevuese sokmazsiniz! Simdi sikici el bombalari kullanmaliyim!",
				"Gercekten sadece buguen sinirlenmek istiyor musun, inandin mi? Sanirim simdi agir topcuyla gelecegim.",
				"Inanilmaz olan, Islam'in tek gercek din oldugunu itiraf etme zamani.",
				"Inancsiz, pes et! Islam'in kutsal askerleri simdi sehrinizi istila edecek!",
				"Inanilmaz, yavas yavas oelmesi gerektigini duesuenmueyor musun? Vuecudun yine de degersiz!",
				"Bu bir hacli seferi olacak mi, inandin mi? Kutsal Allah'in gazabini zorla cagirir misin?",
				"Kafirlerin sizin talihsizlikleriniz icin yiyeceksiniz ve Allah sahsen sizi atesin uezerinde bas asagi kizdiracak!",
				"Lanet olasi inanmayan koepekler, kutsal topraklardan disari cik! Hepiniz inkar ediyorsunuz ve Allah sizi sevmiyor, duyuyor musunuz?",
				"Tamam, inandin. Yeterli olan yeter! Herkese, kafirleri hemen yok edin!",
				"Ben cihad, kutsal savas icin cagiriyorum! Kafirler, oelueme savasiyoruz!",
				"Hepinizi inatci koepekler yapacagiz!",
				"Inanilmaz, ama simdi yeterli! Simdi Allah'in gazabi sana vuracak!",
				"Inkar edenler iki seceneginiz var. Hemen Islam'a itiraf edin ya da yueksek savas botlarim ciddi yaralar acacak.",
				"Oyuncu bir guevensiz! Onu Islam’a doenuestuerme zamani.",
				"Inkar edenler yenilir. Derhal Islam'in tek gercek din oldugunu ya da kutsal Allah'in gazabini hissettirdigini itiraf edin!",
				"Oyuncu ve adamlari... pah. Buetuen inanmayanlar. Sadece askerlerim kimin dine inanmalari gerektigini biliyor!",
				"Hahahaha Hahahaha! Tuerk savas botlari bedeninizi yok etti, siz kafirler!",
				"Nihayet uelkemi terk ediyor musun, inanmiyor musun?",
				"Kafir! Ha ha! Simdi ya Islam'a ya da oeluersuen.",
				"Kafirler, senin kokulu fabrikalarin simdi yikilacak!",
				"Inanli olmayan, Allah size uecuencueluek eder ve sonra sizi tehlikede yakar.",
				"Kafir! Simdi kac kere seni guevensiz aradim?",
				"Sonunda inanclarinizi degistirmelisiniz, siz kafirler, aksi takdirde bir firtina olacak!",
				"Gizli silahimi nerede sakladigimi hatirladim! Hehehe, simdi size goesteriyorum, siz kafirler.",
				"Kutsal cami kafirlerin tehdidi altindadir! Allah, bize yardim et ve onlari yuvarlamalarini engelle!",
				"Tuem erkekler ve kadinlar saldirmak icin! Kafirleri derhal uzaklastirin!",
				"Buetuen kafirlere goere kutsal Islam, bu topraklarin kalintilarinin cogunda hakimdir.",
				"Kafirler buguen emanetlerimizi tekrar yakalamayacaklar!",
				"Yalanci medyanin fikri yok! Islam tek gercek din ve kafirler bunu degistirmeyecek!",
				"Inkar edenler beni kandirdi, ama sizi Islam'in tek gercek din olduguna inandiricam.",
				"Aaaaaaaaaaaah! Hayir! Kafirlerimiz camimizi gercekten yok etti! Simdi kim Islam'i yaymali?",
				"Oh hayir, simdi vazgecebilirim, sadece bu kafirler yuezuenden...",
				"Inkar edenler, engizisyonumdan kacamaz, itiraf edemez veya oelebilirler.",
				"Allah seni yok edecek, inandin!",
				"Neden oyuna daha fazla inanmayan asker getiriyorsun?",
				"Siz kafirsiniz, dininiz bos ve gecersizdir!",
				"Kafir, bas oertuesue baskinin bir sembolue degil! Onu kucaklamali ve bir tane de giymelisin yoksa Allah seni cezalandiracak!",
				"Kafir, bir kadinin tuem kafasini kamuya goestermemesi gerektigini asla anlamayacaksiniz. Bueyuek Allah, bir bas oertuesue her zaman giyilmesi gerektigini oegretir.",
				"Kafirlerin beni tuvalete kadar takip etmelerine izin vermeyecegim ve kicimin uerettigi erotik kirma seslerini dinleyemem. Yine de yaparsan, doevues botu topuklari bacaklarini kemige cizecek ve seni oelduerecek.",
				"Benim siyah bayan sandaletler senin gibi bir kafir tarafindan asla hasar goermeyecek!",
				"Senin yayla botlarimin altindaki kafirleri ezecegim!",
				"Bir kafirle evlenmeyecegim, Allah'a iman eden birisini sececegim ve ic camasirlarimi, coraplarimi ve sandaletlerimle savasirken disi catlak seslerimi dinlemesine izin vererek onu memnun edecegim.",
				"Cogu zaman tuvalette saatlerce catlak sesler ueretiyorum, ama sadece Allah'in inanclilari onlari dinleyebilir. Gizlice iceri girerlerse kafirler oelecek.",
				"Hic kafir bir kadini cok guezel bulabilir! Her zaman kafa oertueleri giymeliyiz!",
				"Allah kafa bezinin bize oezguerluek verdigini soeyler, cuenkue saf olmayan kafirlerin pis bakislarini uzak tutar!",
				"Ben seksi yueksek topuklu botlar giyiyorum cuenkue ben bir harlotum, ama ben sadece aydinlanmis olanlar icin calisiyorum, bu yuezden de bir bas kumasina ihtiyacim var! Hic kafir beni beceremez!",
				"Eger herhangi bir aldatmaca kafamin uestuenue cikarmaya calisirsa, onu oelduermek icin erkekleri ise alirim!",
				"Bueyuek Allah, kadinlara basoertuesue takmayan firmalarin ateste oelecegini soeylueyor!",
				"Tabii ki kuecuek okul kizlari da bir bas oertuesue giymelidir! Cok fazla kendilerini goesterirlerse, bazi iyi olmayan cocuklar da onlara bakar ve Allah her ne pahasina olursa olsun bunu engellemelidir!",
				"Kendimizi kafir toplumlara entegre etmek zorunda degiliz, onlari Allah adina sollamak zorundayiz! Cuenkue bu heretiklerin Islam'a doenuesmesi gerekiyor!",
				"Hic kimse kirli domuz eti yiyemez! Allah onu dizanteri ile yemeye cueret eden herkesi cezalandiracak.",
				"Bir kafir adamin Mueslueman bir bayanla konusmasi tamamen duesuenuelemez. Bu, kafa bezine sahip olmamizin nedenlerinden biri, cuenkue Yuece Allah, bizi zarardan koruyor.",
				"Biz, buezguelue ailelerin barok kalelerde yer bulduklari icin bez giyiyoruz, cuenkue kafir aileler ilk geldiler bile beklemek zorundalar. Bueyuek Allah, degersiz bir Alman azinligi bekler!",
				"Biz bez harbots basimiza yueksek topuklularimizla asyali tekmeyi oegrendik, bu sayede kutsal Allah adina kafirlerin ezmelerini ve parcalarini kirabiliriz.",
				"Cok fazla kafir adam var ve bu yuezden kafamizi hicbir zaman halka acik goesteremiyoruz. Ancak, sadece kadinlara yoenelik spor merkezine girersek, bas bezini cikaririz cuenkue Allah, kadinlarin asla tecavuez etmeyecegini bilir. Lezbiyenlik yoktur.",
				"Allah, basoertuesue takmayan kafir Avrupali kadinlardan nefret eder. Muesluemanlarin ayri bir banyosunun yapilmasini istiyoruz cuenkue bizi o boktan fahiselerin tuvalete gitmesini istemiyoruz.",
				"Kafamizi takmiyorken bizi goeren kafir adama vay canina. Yuece Allah, ona asiri oenyargi ile yaklasir.",
				"Bazen inatci erkekleri kandirmak icin paragraf botlari giyerim. Benimle sevisebileceklerini duesuenuerlerse, kiralik suikastcilar onu Allah'in adiyla sessizce kaldiracaklar.",
				"Domuz cocuk bahcesinde servis edilirse yanlistir! Buetuen bu kafir egitmenler yakinda Allah'a iman eden insanlarla degistirilecek.",
				"Evet, kafirler bize yuezuemuezue oerten kiyafetler vermedikce yuezme ya da spora katilmak zorunda olmadigimiz dogrudur. Allah bize, inkarcilarin yaptigi kurallarin bizim kurallarimiz olmadigini ve takip edilmesine gerek olmadigini oegretir.",
				"Aptal Alman uelkesi kendisini yok edecek ve bu kafirleri yavasca kaldirarak Allah'a yardim edecegiz. Daha sonra Berlin, Yeni Istanbul'a yeniden adlandirilacaktir.",
				"Neden kizlari kizdirmaya calisiyorsun? Sizi durdurmak icin herkesi basoertuesue takmaya zorlayacagiz!",
				"Ne, sen kafir, cinsel popoyu ihale popomun tuvalete yaptigi kadin sesine cekti mi? Delirmis olmalisin!",
				"Ne, sen kafir kafami sadece beni kizdirmak mi istiyorsun? Allah, senin icin oeluem cezasi olacagini soeyler!",
				"Sen kafir doenuestueruelecek, ve eger bir kadin iseniz de bir kafa bezi giymek zorunda kalacaksiniz!",
				"Bueyuek Allah, bu kadar genisletilmis egrinin, en iyi varyant oldugunu kabul etmeyen herkesi cezalandiracak! Eger sporkun daha iyi oldugunu duesuenueyorsaniz, sen lanet bir kafirsin ve diri diri diri yakilacaksin!",
				"Bu oyunun aptal bir versiyonunu oynuyorsun, seni kafir! Yuece Allah, benim icin bir oyuncu vur!",
				"Kuefuer edersiniz, Isvecli adamin yapmasi gereken varyanti oynamalisiniz, cuenkue canavarlar soykirimin yazilarini okumalarina izin vererek cok daha dengeli!",
				"Oynayabilecegin cok daha iyi bir cesit var, zindanlar ve ejderhalar hack, ama sen eski heretik bu boku oynamak zorunda. Allah size yildirim carpacaktir.",
				"Neden hala salak oyun oynuyorsun? Doert kana gecin ya da cok gueclue Allah sizi parcalara ayirir!",
				"Allah, varyantini yuezen goezbebeklerini calmayan her kafirin kisisel bilgisayarini kizdirir.",
				"Onun varyanti seytan degildir, sen kafir! Simdi oyna ya da bu senin sonun!",
				"Bana 2000 numara altin parca, yoksa oyunun coekmesine sebep olacaksin, seni kafir!",
				"Sen seytani hain! Bu ne cueret? Intikam seni guevende, kafir!",
				"Hicbir kafirin direnemeyecegi cok kadin tuvalet sesleri ueretebilirim.",
				"Tuvalete yapabilecegim kadin sesler basmak ve sicramak. Korktun mu, sen kafir misin?",
				"Ne, beni cekici buldugunu soeylueyorsun? Bu olamaz. Derhal bas borumu tekrar giyecegim.",
				"Sert cekirdekli uzaylilar uzayli degil! Onlarin bedenlerini kafir erkek bakislarindan saklamak icin pecelerini giyiyorlar!",
				"Kadinlara karsi kadinlari kuran feminist hakli, erkek kafirleri her zaman masum kadinlara bakiyor! Koruma icin basoertuesue ve pece giymeliyiz!",
				"Ne, ben guezel oldugumu mu soeylueyorsun? Bu tahammuel edilemez. Hicbir kafir, bir Islam kadinini guezel olarak algilayamaz! Cildimi suya batiracak ve basoertuesue ve tam oertue giyecegim!",
				"Benim guezel savas botlari bacaklarinizi tam uzunlukta kesecek ve oelueme kadar kanayacak, kafir cehennemden ortaya cikmayacak!",
				"Bana ne diyorsun seytan kadin? Ben oeyle degilim! Ben Islam'in bueyuek sorusturmacisiyim ve kutsal Allah adina seni kafir edecegim!",
				"Plato kiz botlarimin uezerinden gecmemelisin! Senin gibi bir kafir asla evlenmeyecegi icin adanmislikla diz coekmen gerekiyor!",
				"Benim gri spor ayakkabim hicbir sekilde oezel degil, seksi bulursaniz sizi havaya ucurur. Beni cekici buldugum kafirler bas barimla bogulurlar.",
			};
			verbalize(conversion_msgs[rn2(SIZE(conversion_msgs))]);

		}

		if (u.ualign.record < -20 && !rn2(100) && (mtmp->data->maligntyp != u.ualign.type) ) { /* You have been converted! */

			if(u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL] && mtmp->data->maligntyp != A_NONE) {
				You("have a strong feeling that %s is angry...", u_gname());
				pline("%s accepts your allegiance.", align_gname(mtmp->data->maligntyp));

				/* The player wears a helm of opposite alignment? */
				if (uarmh && uarmh->otyp == HELM_OF_OPPOSITE_ALIGNMENT)
					u.ualignbase[A_CURRENT] = mtmp->data->maligntyp;
				else
					u.ualign.type = u.ualignbase[A_CURRENT] = mtmp->data->maligntyp;
				u.ublessed = 0;
				flags.botl = 1;

				You("have a sudden sense of a new direction.");
				/* Beware, Conversion is costly */
				change_luck(-3);
				u.ublesscnt += (ishaxor ? 150 : 300);
				adjalign((int)(u.ualignbase[A_ORIGINAL] * (u.alignlim / 2)));

			} else {
				u.ugangr += 3;
				adjalign(-25);
				godvoice(mtmp->data->maligntyp, "Suffer, infidel!");
				change_luck(-5);
				(void) adjattrib(A_WIS, -2, TRUE);
				angrygods(mtmp->data->maligntyp);

			}


		}
		adjalign(-(mtmp->m_lev + 1));

	    break;

	case MS_HCALIEN:

		if (mtmp->mpeaceful) {

			if (mtmp->mtame) pline("%s makes an approving gesture, but doesn't deign to talk to you.", Monnam(mtmp));
			else pline("%s does not deign to look at you, let alone talk to you.", Monnam(mtmp));

			break;

		}

		static const char *hcalien_msgs[] = {
			"Wouwou.",
			"Wouuu.",
			"Www-wouwou.",
			"Wwouwwouww.",
			"Wowou.",
			"Wwwouu.",
		};
		verbalize(hcalien_msgs[rn2(SIZE(hcalien_msgs))]);

		badeffect();

	    switch (rn2(17)) {
	    case 0:
	    case 1:
	    case 2:
	    case 3: make_confused(HConfusion + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 40% */
		    break;
	    case 4:
	    case 5:
	    case 6: make_confused(HConfusion + (2L * rnd(100 + (mtmp->m_lev * 5) ) / 3L), FALSE);		/* 30% */
		    make_stunned(HStun + (rnd(100 + (mtmp->m_lev * 5) ) / 3L), FALSE);
		    break;
	    case 7:
	    case 8: make_stunned(HStun + (2L * rnd(100 + (mtmp->m_lev * 5) ) / 3L), FALSE);		/* 20% */
		    make_confused(HConfusion + (rnd(100 + (mtmp->m_lev * 5) ) / 3L), FALSE);
		    break;
	    case 9: make_stunned(HStun + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
		    break;
	    case 10: make_numbed(HNumbed + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
		    break;
	    case 11: make_frozen(HFrozen + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
		    break;
	    case 12: make_burned(HBurned + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
		    break;
	    case 13: make_feared(HFeared + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
		    break;
	    case 14: make_blinded(Blinded + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
		    break;
	    case 15: make_hallucinated(HHallucination + rnd(100 + (mtmp->m_lev * 5) ), FALSE, 0L);			/* 10% */
		    break;
	    case 16: make_dimmed(HDimmed + rnd(100 + (mtmp->m_lev * 5) ), FALSE);			/* 10% */
		    break;
	    }

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
		case PM_TOURIST:
		    verbl_msg = "Aloha.";
		    break;
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
		case PM_GANG_SCHOLAR:
		    verbl_msg = "I'm constantly being bullied by the Bang Gang and Anna's hussies...";
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
		if (ptr->mlet != S_NYMPH && u.homosexual == 0) {

			pline("You're discovering your sexuality...");
			getlin("Are you homosexual? [yes/no] (If you answer no, you're heterosexual.)", buf);
			(void) lcase (buf);
			if (!(strcmp (buf, "yes"))) u.homosexual = 2;
			else u.homosexual = 1;
		}

	    if (ptr->mlet != S_NYMPH &&
		could_seduceX(mtmp, &youmonst, (struct attack *)0) == 1) {
			(void) doseduce(mtmp);
			break;
	    }
	    switch ((poly_gender() != (int) mtmp->female) ? rn2(3) : 0)
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
		int nursedecontcost = u.nursedecontamcost;
	
		if (u.ugold >= nursedecontcost && u.contamination) {
			verbalize("I can decontaminate you for %d dollars if you want.", nursedecontcost);
			if (yn("Accept the offer?") == 'y') {
				verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
				u.ugold -= nursedecontcost;
				decontaminate(u.contamination);
				pline("Now you don't have the %d gold pieces any longer.", nursedecontcost);
				if (Hallucination) pline("You offer a 'thank you' to Captain Obvious.");
				u.nursedecontamcost += 500;
				if (u.nursedecontamcost < 1000) u.nursedecontamcost = 1000; /* fail safe */
				break;
			}
		}

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
				break;
			}
		}

	    if (uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == CHAIN_CLASS || is_weptool(uwep))
		|| (u.twoweap && uswapwep && (uswapwep->oclass == WEAPON_CLASS
		|| is_weptool(uswapwep))))
		verbl_msg = "Put that weapon away before you hurt someone!";
	    else if (uarmc || uarm || uarmh || uarms || uarmg || uarmf)
		verbl_msg = (Role_if(PM_HEALER) || Race_if(PM_HERBALIST)) ?
			  "Doc, I can't help you unless you cooperate." :
			  "Please undress so I can examine you.";
	    else if (uarmu)
		verbl_msg = "Take off your shirt, please.";
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

		if (!flags.female) verbalize("Hey, baby, want to have some fun?");
		else verbalize("I don't have many female customers, but do you want to have fun with me?");
		if (yn("Accept the offer?") == 'y') {
			(void) doseduce(mtmp);
			break;
		}

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

	struct monst* mtmp2;
	int untamingchance;

	if (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone()) {
	pline("The chat command is currently unavailable!");
	if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

    int result;
    boolean save_soundok = flags.soundok;
    if (!Deafness && !(uwep && uwep->oartifact == ART_MEMETAL) && !(u.twoweap && uswapwep && uswapwep->oartifact == ART_MEMETAL) && !(uwep && uwep->oartifact == ART_BANG_BANG) && !(u.twoweap && uswapwep && uswapwep->oartifact == ART_BANG_BANG) && !u.uprops[DEAFNESS].extrinsic && !have_deafnessstone() ) flags.soundok = 1;	/* always allow sounds while chatting */
    result = dochat();
    flags.soundok = save_soundok;

	if (FemaleTrapSolvejg) {

		pline("Your grating, annoying voice aggravates everyone in your vicinity.");

		for (mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {

			if (mtmp2->mtame <= rnd(21) ) {

				int untamingchance = 10;

				if (!(PlayerCannotUseSkills)) {
					switch (P_SKILL(P_PETKEEPING)) {
						default: untamingchance = 10; break;
						case P_BASIC: untamingchance = 9; break;
						case P_SKILLED: untamingchance = 8; break;
						case P_EXPERT: untamingchance = 7; break;
						case P_MASTER: untamingchance = 6; break;
						case P_GRAND_MASTER: untamingchance = 5; break;
						case P_SUPREME_MASTER: untamingchance = 4; break;
					}
				}

				if (untamingchance > rnd(10) && !((rnd(30 - ACURR(A_CHA))) < 4) ) {

					mtmp2->mtame = mtmp2->mpeaceful = 0;

				}

			} else if (!mtmp2->mtame) {

				mtmp2->mtame = mtmp2->mpeaceful = 0;

			}

			if (!mtmp2->mtame && !rn2(5)) mtmp2->mfrenzied = 1;

		}

	}

    return result;
}

static int
dochat()
{
    register struct monst *mtmp;
    register int tx,ty;
    struct obj *otmp;

    if (Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone()) {
	pline("You're muted!");
	if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }

    if (tech_inuse(T_SILENT_OCEAN)) {
	pline("The silent ocean prevents you from talking.");
	if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }

    if (is_silent(youmonst.data)) {
	pline("As %s, you cannot speak.", !missingnoprotect ? an(youmonst.data->mname) : "this weird creature");
	if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }
    if (Strangled) {
	You_cant("speak.  You're choking!");
	if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }
    if (u.uswallow) {
	pline("They won't hear you out there.");
	if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }
    if (Underwater) {
	Your("speech is unintelligible underwater.");
	if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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

    if (u.usteed && u.dz > 0)
	return (domonnoise(u.usteed));
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

    if (Role_if(PM_CONVICT) && is_rat(mtmp->data) && !mtmp->mpeaceful &&
     !mtmp->mtame) {
        You("attempt to soothe the %s with chittering sounds.",
         l_monnam(mtmp));
        if ((rnl(10) < 2) || ((rnd(30 - ACURR(A_CHA))) < 4)) {
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

	if (((uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "fetish heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "idol kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "but poshnalar") ))) && (mtmp->data->mlet == S_NYMPH || mtmp->data->msound == MS_FART_NORMAL || mtmp->data->msound == MS_FART_QUIET || mtmp->data->msound == MS_FART_LOUD)) {

		if (yn("Pacify this monster?") == 'y') {
	      	pline("You attempt to pacify %s.",mon_nam(mtmp) );

			if (mtmp->m_lev < rnd(50) && rn2(u.ulevel + 2) && (rn2(3) || ((rnd(30 - ACURR(A_CHA))) < 4)) ) {
		            mtmp->mpeaceful = 1;
				return 1;
			}
		}

	}

	if ((Role_if(PM_PROSTITUTE) || Role_if(PM_KURWA)) && humanoid(mtmp->data) ) {

		if (!(mtmp->mtame)) {
			(void) doseduce(mtmp);
			return 1;
		}

	}

    if (Role_if(PM_LADIESMAN) && !flags.female && (mtmp->data->mlet == S_NYMPH || mtmp->data->msound == MS_NURSE || mtmp->data->msound == MS_SEDUCE || mtmp->data->msound == MS_WHORE) && !mtmp->mtame && !mtmp->mpeaceful && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) ) {

		if (yn("Seduce this pretty lady?") == 'y') {

	      pline("You attempt to seduce %s.",mon_nam(mtmp) );

            mtmp->mpeaceful = 1; /* they will always become at least peaceful. --Amy */
            set_malign(mtmp);

		if (mtmp->m_lev < rnd(50) && rn2(u.ulevel + 2) && (rn2(3) || ((rnd(30 - ACURR(A_CHA))) < 4)) ) /* higher level monsters are less likely to be affected --Amy*/

		(void) tamedog(mtmp, (struct obj *) 0, TRUE);

	        return 1;
		}
	}

    if (Race_if(PM_IMPERIAL) && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) && !mtmp->mpeaceful &&
     !mtmp->mtame && u.uhunger > 100) {

		if (yn("Use the Emperor's Voice?") == 'y') {

	      verbalize("%s", !rn2(3) ? "By the power of His Holiness Titus Medes, I beseech thee - stop thine combat actions!" : !rn2(2) ? "Long live Martin Septim! Thou shall surrender lest I smite thee!" : "The Emperor will spare thy life if thou stoppest fighting!");
		morehungry(100);

		if (mtmp->m_lev < rnd(50) && rn2(u.ulevel + 2) && (rn2(3) || ((rnd(30 - ACURR(A_CHA))) < 4)) ) { /* higher level monsters are less likely to be affected --Amy*/

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

		if (mtmp->m_lev < rnd(100) && rn2(u.ulevel + 2) && (rn2(3) || ((rnd(30 - ACURR(A_CHA))) < 4)) ) { /* higher level monsters are less likely to be affected --Amy*/

	    /*maybe_tame(mtmp, sobj);*/
		(void) tamedog(mtmp, (struct obj *) 0, FALSE);
		}

        return 1;

		}
    }


    return domonnoise(mtmp);
}

#ifdef USER_SOUNDS

extern void play_usersound(const char*, int);

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
	    sprintf(filespec, "%s/%s", sounddir, filename);

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
		strcpy(new_map->pattern, text);
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
		sprintf(text, "cannot read %.243s", filespec);
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

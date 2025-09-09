/*	SCCS Id: @(#)sounds.c	3.4	2002/05/06	*/
/*	Copyright (c) 1989 Janet Walz, Mike Threepoint */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h"
#include "time.h"
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

static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static const char allnoncount[] = { ALL_CLASSES, 0 };
STATIC_OVL NEARDATA const char pawnables[] = { ALLOW_COUNT, GEM_CLASS, 0 };

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

    hallu = FunnyHallu ? 1 : 0;

    if (level.flags.nfountains && !rn2(400)) {
	static const char * const fountain_msg[] = {
		"bubbling water.",
		"water falling on coins.",
		"the splashing of a naiad.",
		"flowing water.",
		"a nether voice.", /* the water demon */
		"muffled hissing.", /* the water mocassins */
		"a soda fountain!",
		"a vampire complaining about the architecture!", /* they don't like running water */
		"someone offering three wishes to a lucky person!", /* water demon being generous */
		"the moaning noises of an adventurer caught in hot sexual pleasure with a nymph!",
		"Indiana Jones screaming for help!", /* he's terribly afraid of snakes */

	};
	You_hear("%s", fountain_msg[rn2(6+hallu*5)]);
    }
#ifdef SINK
    if (level.flags.nsinks && !rn2(300)) {
	static const char * const sink_msg[] = {
		"a slow drip.",
		"a gurgling noise.",
		"a metallic plinking sound.",
		"a toilet flush!",
		"a dripping tap.",
		"dripping water.",
		"dishes being washed!",
		"a neverending dripping sound that is driving you crazy!",
		"someone getting in on the action with a hot foocubus!",
		"something that gives you a sinking feeling.",
		"something that gives you a sinking feeling, because sounds.c uses 'You_hear' instead of 'pline' and therefore all of the sink-related sounds have to be messages that start with 'You hear', annoyingly enough.",
	};
	You_hear("%s", sink_msg[rn2(6+hallu*5)]);
    }
#endif
    if (level.flags.has_court && !rn2(200)) {
	static const char * const throne_msg[] = {
		"the tones of courtly conversation.",
		"a sceptre pounded in judgment.",
		"Someone shouts \"Off with %s head!\"",
		"royal trumpets.",
		"what sounds like a royal ceremony.",
		"Queen Beruthiel's cats!",
		"a royal coronation/culmination taking place!",
		"the king scolding his most trusted advisor for failing a task!",
		"the queen conducting an execution! Oh my god, a poor, helpless woman was guillotined!!",
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping ||
			is_lord(mtmp->data) || is_prince(mtmp->data)) &&
		!is_animal(mtmp->data) &&
		mon_in_room(mtmp, COURT)) {
		/* finding one is enough, at least for now */
		int which = rn2(5+hallu*4);

		if (which != 2) You_hear("%s", throne_msg[which]);
		else		pline(throne_msg[2], uhis());
		return;
	    }
	}
    }
    if (level.flags.has_swamp && !rn2(200)) {
	static const char * const swamp_msg[] = {
		"hear mosquitoes!",
		"smell marsh gas!",	/* so it's a smell...*/
		"hear a muffled splash.",
		"are frightened by the calm atmosphere for some reason.",
		"hear Donald Duck!",
		"inhale a vile stench that reminds you of what kissing a frog is probably like!",
		"hear someone falling in the water and losing a life.",
		"suddenly encounter the hamming fish, which can devour and instakill you!", /* Big Bill from Super Mario Bros 3 */
	};
	You("%s", swamp_msg[rn2(4+hallu*4)]);
	return;
    }
    if (level.flags.spooky && !rn2(200)) {
	static const char *spooky_msg[] = {
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
	You("%s", spooky_msg[rn2(15+hallu*9)]);
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
		switch (rn2(3+hallu*3)) {
		    case 0:
			You_hear("a low buzzing.");
			break;
		    case 1:
			You_hear("an angry drone.");
			break;
		    case 2:
			You_hear("a repeating monotonous melody."); /* rambi rumble melody before you meet King Zing */
			break;
		    case 3:
			You_hear("bees in your %sbonnet!",
			    uarmh ? "" : "(nonexistent) ");
			break;
		    case 4:
			pline("You suddenly see King Zing chasing after you! RUN, Rambi, RUN!!!");
			break;
		    case 5:
			You("smell stale honey.");
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
		switch (rn2(4+hallu*2)) {
		    case 0:
			You("suddenly realize it is unnaturally quiet.");
			break;
		    case 1:
			pline_The("%s on the back of your %s stands up.",
				body_part(HAIR), body_part(NECK));
			break;
		    case 2:
			if (midnight()) pline("Suddenly you have a severe shivering fit!");
			else pline("This place seems haunted.");
			break;
		    case 3:
			pline("There is an eerie feeling about this place.");
			break;
		    case 4:
			pline_The("%s on your %s seems to stand up.",
				body_part(HAIR), body_part(HEAD));
			break;
		    case 5:
			pline("An enormous ghost suddenly appears before you!");
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_barracks && !rn2(200)) {
	static const char * const barracks_msg[] = {
		"blades being honed.",
		"loud snoring.",
		"dice being thrown.",
		"a soldier checking up on a gun.", /* they have firearms in SLASH'EM */
		"someone talking about the grenade cache.", /* and also frag grenades */
		"General MacArthur!",
		"the senior drill instructor shouting at you for being a lazy fatass!",
		"someone sneer 'Where you from, soldier? Texas?? Holy shit, you know what comes from Texas!'", /* Half Life: Opposing Forceses */
		"someone scream 'FIRE! IN! THE! ASS! HOLE!'",
		"someone pulling the pin of a grenade, and shockedly realize it's a double free corruption grenade that can crash the game!", /* bug that existed in SLEX */
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
		You_hear("%s", barracks_msg[rn2(5+hallu*5)]);
		return;
	    }
	}
    }
	    if (level.flags.has_clinic && !rn2(200)) {
		static const char *hospital_msg[] = {
			"hear something about streptococus.",
			"smell chloroform nearby.",
			"hear someone cursing viruses.",
			"hear the sound of an ambulance.",
			"listen to patients screaming while the doctor is conducting the sectio.",
			"seem to hear Doctor Frankenstein.",
			"hear someone scream 'NOOOOOO! Get away from my teeth! Dentists are TERRIBLE people!!!'",
			"suddenly have a flashback to your last OP!",
			"do not want to get another covid shot, the last one already caused your body to react all strangely and, most annoyingly, you endured a covid-19 infection anyway! So you have every right to believe that the vaccination does in fact do nothing at all!",
			"hear a pained woman scream 'WHERE ARE THE DAMN PAINKILLERS YOU FAT BITCH!!'", /* woman trying to give birth to a child */
		};
		You("%s", hospital_msg[rn2(5+hallu*5)]);
		return;
	    }
	    if (level.flags.has_nymphhall && !rn2(200)) {
		static const char *nymphhall_msg[] = {
			"hear leaves rustling.",
			"hear a female voice singing a song.",
			"catch a glimpse of sunlight.",
			"hear crickets chirping loudly!",
			"hear birds singing!",
			"hear grass growing!",
			"hear wind in the willows!",
			"seem mesmerized.",
			"feel like giving all your possessions to a beautiful woman.",
			"want to pull down your pants.",
		};
		You("%s", nymphhall_msg[rn2(5+hallu*5)]);
		return;
	    }
	    if (level.flags.has_lemurepit && !rn2(2000)) { /* from unnethack, deliberately made rare */
		static const char *lemurepit_msg[] = {
			"the crack of a barbed whip!",
			"the screams of tortured souls!",
			"a wail of eternal anguish!",
			"diabolical laughter!",
			"cries of repentance!",
			"futile pleas for mercy!",
			"screams of lust!",
			"the crack of your mistress's whip!",
			"a weeping willow!",
			"wonderful scratching noises!",
			"erotic rubbing noises!",
			"cheerful squeaking noises!",
			"sexy licking noises!",
		};
		You_hear("%s", lemurepit_msg[rn2(6+hallu*7)]);
		return;
	    }
	    if (level.flags.has_spiderhall && !rn2(200)) {
		static const char *spiderhall_msg[] = {
			"hear mandibles clicking.",
			"notice webs everywhere.",
			"notice little openings in the floor.",
			"see a tiny insect embedded in a cocoon.",
			"trip over a broken spider egg.",
			"see a giant tarantula squittering around!",
			"have to fight Arich, the old cheater, while listening to a dramatic music that is unfortunately too quiet.",
			"must get past Barbos' Barrier!",
			"suddenly get an irrational fear of glowflies, since you just know they'll wait until you sleep, spin a thick cocoon around you, and then very slowly start feasting on your exposed flesh until the only thing that remains of you is a grisly-looking skeleton!", /* based on a crime thriller Amy read once; while she knows that glowflies don't actually do that IRL, she still has to relive the horrible experience of reading that book every time she sees such a glowing, flying bug */
		};
		You("%s", spiderhall_msg[rn2(4+hallu*5)]);
		return;
	    }
	    if (level.flags.has_trollhall && !rn2(200)) {
		static const char *trollhall_msg[] = {
			"hear a grunting noise.",
			"notice a horrible smell.",
			"hear loud footsteps going up and down the halls.",
			"hear a rough voice comparing the merits of vi and emacs.",
			"trip over a stinking pile of shit!",
			"suddenly see a huge monster appearing right in front of you!",
			"hear some usenetter spouting radical bullshit!",
			"can't seem to find the plonking switch!",
		};
		You("%s", trollhall_msg[rn2(4+hallu*4)]);
		return;
	    }
	    if (level.flags.has_humanhall && !rn2(200)) {
		static const char *humanhall_msg[] = {
			"listen to a conversation.",
			"hear people talking.",
			"hear someone drawing a weapon.",
			"chime in on some people celebrating a party.",
			"overhear plans for a raid.",
			"sense that you are not alone...",
			"hear a glass of wine breaking!",
			"listen to a radio broadcast: For the next five days, the dungeon's estimated temperature high is 96 degrees. Now, is that in Fahrenheit or Kelvin?",
			"know that there's a celebration being held for YOU! Where do you get this party started?",
			"hear somebody announce: 'So are we starting to chase down these intruders yet?'",
		};
		You("%s", humanhall_msg[rn2(5+hallu*5)]);
		return;
	    }
	    if (level.flags.has_golemhall && !rn2(200)) {
		static const char *golemhall_msg[] = {
			"hear rattling noises.",
			"think you saw an object move on its own.", /* golems = animate objects */
			"feel that something weird may be just around the corner.",
			"hear a mechanical noise.",
			"hear the deafening noise of a metal object breaking apart!",
			"see a warmech appearing before you! If this were a Pokemon game, it would say: A wild WARMECH appeared!",
			"will certainly never catch a Regigigas, because your pokeball isn't good enough!",
			"listen to the Diablo sorceror casting a summoning spell!", /* one of the spells is called "golem" */
		};
		You("%s", golemhall_msg[rn2(4+hallu*4)]);
		return;
	    }

	    if (level.flags.has_cryptroom && !rn2(200)) {
		static const char *cryptroom_msg[] = {
			"smell rotting flesh.",
			"hear shuffling noises.", /* grunthack zombie inspired */
			"hear a loud moan.",
			"inhale the stench of charnel and the things that should be dead, but aren't.",
			"feel that you're near death!",
			"don't have much longer to live...",
			"listen to a tune that gets ever more dissonant...", /* crypt music from Robin Hood, Prince of Thieves */
			"hear your ancestors laughing at you!",
		};
		You("%s", cryptroom_msg[rn2(4+hallu*4)]);
		return;
	    }

	    if (level.flags.has_troublezone && !rn2(200)) {
		static const char *troublezone_msg[] = {
			"smell trouble.",
			"feel that something's gonna happen.",
			"sense several evil presences.",
			"sense high tension.",
			"notice an electrical feeling in the air.",
			"are afraid of something.",
			"hear the voices of vaguely humanoid beings.",
			"feel vexed.",
			"have to count to three, and if you fail, you can forget about your game!", /* this and the next few are from the obscure Trouble Zone game for the Nintendo Gameboy */
			"are way in over your head!",
			"must sort the blocks according to the numbers, and pronto!",
			"fall asleep while the line very slowly dissolves.",
			"listen to the military music broadcast from Radio Enclave.", /* Fallout 3 */
			"are being fully gouged by the type!",
			"collected too many speed increasing pickups and your speed wrapped over so you're super slow now!",
			"realize that the game has resetted itself, so you'll have to start over at dungeon level 1.",
		};
		You("%s", troublezone_msg[rn2(8+hallu*8)]);
		return;
	    }

	    if (level.flags.has_weaponchamber && !rn2(200)) {
		static const char *weaponchamber_msg[] = {
			"hear metallic clashes.",
			"are listening to someone practicing their sword skill.",
			"hear a trumpet, followed by attack orders.",
			"hear a knight discussing strategy with his subordinates.",
			"feel that the enemies' main base camp is on this floor.",
			"are ambushed by shadowy figures with gleaming swords!",
			"should impale yourself on your weapon while you still can...",
			"absolutely want a polearm, because you love polearms.",
			"want to look for a replacement lance because your last one broke when you jousted a monster.",
			"listen to an asian woman announcing that she'll hunt down and kill you!", /* a reference that no one will get because it's about a certain scene in one of Amy's fanfics */
		};
		You("%s", weaponchamber_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_hellpit && !rn2(200)) {
		static const char *hellpit_msg[] = {
			"smell brimstone.",
			"hear the bubbling of lava.",
			"smell sulfur.",
			"chime in on a pack of demons performing a satanic ritual.",
			"sense the presence of hellish beings.",
			"see that the ceiling has turned red-hot!", /* Oblivion gate, but you're presumably in a dungeon and not the outskirts, so instead of the sky changing colors, the ceiling does so */
			"feel that Mehrunes Dagon will open the Great Gate of Oblivion soon!",
			"realize that the hellspawn invasion has already begun...",
			"sense the presence of a gate to Oblivion.",
			"suddenly encounter the giant siege caterpillar and have 10 minutes to destroy it or everything is lost.",
			"need to protect Annoying Head Martin from the 200 storm atronachs and dremora valkynaz.",
		};
		You("%s", hellpit_msg[rn2(6+hallu*5)]);
		return;
	    }

	    if (level.flags.has_robbercave && !rn2(200)) {
		static const char *robbercave_msg[] = {
			"feel that this floor is home to the robbers.", /* these are all based on Amy's FO3 fanfic */
			"know that you must watch your purse.",
			"get the feeling that somebody is after your valuables.",
			"hear someone screaming that their money got stolen!",
			"listen to people drinking beer.",
			"hear a male voice announce: 'Hundred one hundred!'", /* robbers playing cards */
			"want to watch when the robbers assault the motherfuckers' ship, because it will be like a second Pearl Harbor!",
			"know that the bitches are not the rulers of the world, even if they want to view themselves as such just because they have money.",
			"fully endorse racketeering, as long as the super-smart women are the target.", /* and also the quick learners */
			"feel like being sucked into one of Amy's terrible fanfics!",
		};
		You("%s", robbercave_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_casinoroom && !rn2(200)) {
		static const char *casinoroom_msg[] = {
			"hear the clinging of jetons.",
			"hear someone announce 'Place your bets please!'",
			"see a passerby holding a casino chip.",
			"can hear a roulette wheel spinning.",
			"are feeling like gambling away a couple thousand zorkmids.",
			"are looking for the entrance to Caligula's Casino.",
			"hear someone complaining about the Forelli and Sindacco competitors.",
			"feel like playing some cards.",
			"hear someone using the video poker machine.",
			"hear someone announce 'FISHING! FISHING! WIN! WIN!'",
			"see someone passing by who definitely has to be a high roller!",
			"suddenly realize that there are ever more security full refuges coming, and they'll whittle down your health if you don't manage to escape!",
			"feel that the right way to address Don Salvatore is by saying 'Greetings, Mr. Don', even though Don is actually Italian for Mister.", /* GTA San Andreas mafia boss */
			"know that one of the two chips is a fake without even looking at them.",
			"hear a cat singing 'Vrei sa pleci dar, numa numa yay. Numa numa yay, numa numa numa yay!", /* "Dragostea Din Tei", which is quoted by cats in Elona */
			"hear a cat singing 'Mai-Ya-Hi, Mai-Ya-Hu, Mai-Ya-Ho, Mai-Ya-Ho-Ho!'",
			"have to chase the assassins, who are travelling as a string quartet, and their airplane is too damn fast!", /* "Free Fall" mission in GTA San Andreas */
			"wonder why the beanpole englishman is constantly wanking off. Like who is that monkey?", /* Maccer, obviously */
		};
		You("%s", casinoroom_msg[rn2(9+hallu*9)]);
		return;
	    }

	    if (level.flags.has_sanitationcentral && !rn2(200)) {
		static const char *sanitationcentral_msg[] = {
			"hear heavy breathing.",
			"feel that there's something that wants to drain you of your sanity.",
			"seem more focused on your mental health than usual.",
			"have to wonder whether you're seeing things.",
			"sense an eldritch abomination lurking somewhere on this floor.",
			"start perceiving the world as the terrible place it really is, and go mad from the revelation!",
			"suddenly have over 9000 sanity! Oh no! Now every sanity increase will paralyze you!",
			"have to ask Amy what in-game sanity does, because you have no idea!",
			"feel more sane than before, which is a BAD thing!",
			"hear a horrible call that rings in your head, which periodically increases your sanity!",
		};
		You("%s", sanitationcentral_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_feminismroom && !rn2(200)) {
		static const char *feminismroom_msg[] = {
			"hear a female voice shouting angrily.",
			"listen to a long, squeaking sound.", /* farting noise */
			"can hear the clacking noises of high heels walking on a hard floor.",
			"hear woman noises.",
			"overhear women discussing makeup and perfume.",
			"hear the voice of a model asking whether her dress is looking good enough.",
			"hear strange rumbling noises.", /* farting noise */
			"think you just heard very faint air current noises.", /* farting noise */
			"fear that some stupid woman is gonna slap you silly!",
			"can't resist thinking of a pair of brown leather boots with black block heels, and start wanking off to the thought.",
			"hear very beautiful farting noises!",
			"hear your wife complaining that you're still not home yet!",
			"suddenly hear a commercial announcement: 'Buy a 5 liter bottle of the newest drum stint reluctance perfume for only 10 euros now!'",
			"hear a sales pitcher praising a new branded costume.",
			"hear a seemingly neverending, disgusting sound.", /* crapping noise */
			"listen to very tender female noises.",
			"hear a librarian yelling at someone to be SILENT!",
		};
		You("%s", feminismroom_msg[rn2(8+hallu*9)]);
		return;
	    }

	    if (level.flags.has_playercentral && !rn2(200)) {
		static const char *playercentral_msg[] = {
			"overhear some hectic talk.",
			"catch earshot of your fellow adventurers.",
			"feel that they're coming for you.",
			"don't think you're alone here...",
			"are highly alert for some reason.",
			"are afraid that everyone on this dungeon level is infected with covid-19!",
			"just know that there's someone around who should be quarantined, but isn't!",
			"hear someone sneezing constantly! Why isn't that corona-infected fucker staying at home?!",
			"overhear a loudspeaker announcement: 'Protect yourself and others! Keep your distance to other people and wear your face mask at all times. Help us stop the spread of coronavirus and stay healthy.'",
			"think you just saw a bunch of cops passing you by! Good thing they didn't realize that you weren't wearing your mask properly!",
		};
		You("%s", playercentral_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_meadowroom && !rn2(200)) {
		static const char *meadowroom_msg[] = {
			"feel a slight breeze.",
			"hear crickets chirping.",
			"can hear the cattle bellowing.",
			"hear the cooing of the nightingale.",
			"hear the 'baaaaa' sound of a sheep.",
			"hear someone sing 'Old Mac Donald had a farm...'",
			"can't help it but feel that something that looks normal is terribly amiss.",
			"listen to the palomita, which is Spanish for parrot.", /* or at least the Amy always thought that's what it meant */
			"wonder what's the color of the giant red ant.",
			"feel that the one grass tile is greener than the rest for some reason.",
		};
		You("%s", meadowroom_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_coolingchamber && !rn2(200)) {
		static const char *coolingchamber_msg[] = {
			"are chilled by cold air.",
			"shiver for a moment.",
			"feel that someone forgot to close the door to the freezer.",
			"feel that this is a cold place.",
			"regret not bringing along your winter clothes for this cold atmosphere.",
			"fear that you're gonna develop a lung inflammation.",
			"hear someone calling you to the iceblock shooting.", /* reference to GNTM, where the models had to do a photo shooting in an ice chamber once */
			"can smell the mildew on the walls.",
			"listen to the type of ice block laughing all the time. Harharharharharharhar!",
			"know that if you're walking on ice all the time, you'll unlearn the ability to walk on a non-slippery floor.",
			"hear an ice cream van!",
		};
		You("%s", coolingchamber_msg[rn2(5+hallu*6)]);
		return;
	    }

	    if (level.flags.has_voidroom && !rn2(200)) {
		static const char *voidroom_msg[] = {
			"are struck by a sense of emptiness.",
			"sense a black hole in the fabric of reality.",
			"feel that a special challenge awaits you.", /* ToME */
			"feel the presence of spirits from the netherworld.",
			"hear a sucking sound.",
			"hear audible silence.",
			"notice that this area is disturbingly quiet.",
			"hear the absence of nosie.", /* sic by AntiGulp, and deemed too funny to fix */
			"hear Galadriel whispering 'It is very likely that you die on your journey, but you must go anyway...'",
			"realize that the atmosphere does not contain oxygen! If you stay around you'll suffocate!",
			"hear Melkor laughing at your puny attempts to reach him.",
			"have failed to defeat Maeglin, and he reached the stairs! Everything is lost now. Gondolin will go down in a rain of fire and smoke, and all the humans are killed by blarogs and ringwraiths.",
			"feel a ghostly touch on your nosie and inhale the scent of a rosie!",
			"hear the sounds of the gene splicing machine!", /* Elona */
			"listen to someone complaining about a splintered nail.", /* Harry Potter disappearing (or whatever it's called, it's actually magical teleportation) mishap */
			"neither hear nor see the words, yet somehow they're still there.",
		};
		You("%s", voidroom_msg[rn2(8+hallu*8)]);
		return;
	    }

	    if (level.flags.has_armory && !rn2(5000)) { /* from dnethack, intentionally VERY rare --Amy */
		static const char *library_msg[] = {
			"hear water dripping onto metal.",
			"hear a loud crunching sound.",
			"hear a ceiling tile fall.",
			"hear the scraping of metal on metal.",
			"hear chains clinking.",
			"smell a lot of rust.",
			"hear a gun clank.",
			"hear iron oxidize.",
			"hear a military contractor making out like a bandit.",
			"fear that your +7 artifact weapon might get eaten!",
			"hear the sound of a toilet latch.",
			"wonder why the hell this set of messages is called 'library_msg' in the game's source.",
			"feel that parts of the floor have been washed away.",
			"hear the nearby ACME building!",
		};
		You("%s", library_msg[rn2(7)+hallu*7]);
		return;
	    }

	    if (level.flags.has_hamletroom && !rn2(200)) {
		static const char *hamletroom_msg[] = {
			"have a sense of familiarity.",
			"feel relaxed.",
			"hear a storeclerk making a transaction.", /* but there's not actually a shopkeeper around */
			"hear the cackle of poultry.",
			"listen to the joyful cries of playing children.",
			"feel like returning to your burned home... wait, is there really a reason for doing so?", /* the hallu sounds in particular are inspired by the storyline of Castle of the Winds */
			"hope that you don't have to master the fleeing mine...",
			"gaze once more at the ruins of what used to be your father's farm.",
			"suddenly feel a sharp smoky smell fill your nostrils.",
			"hear the heavy flapping sound of a vulture's wings.",
		};
		You("%s", hamletroom_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_kopstation && !rn2(200)) {
		static const char *kopstation_msg[] = {
			"hear a loud siren.",
			"can hear a police officer requesting reinforcement.",
			"hear the whipping sound of a rubber hose.", /* default keystone kop weapon */
			"overhear the police radio and find out to your shock that you're wanted.",
			"hear an officer giving commands hectically.",
			"realize that your cop wanted level is 6, and the forces of law are coming to bust you!",
			"are chased by a speeding police car!",
			"feel that the safest place in existence is right in the middle of the enemies' base camp.", /* a joke from one of Amy's fanfics where the robbers were discussing where to hide and one of them sarcastically suggested hiding in the cop station */
			"hear the sound of a fucking army helicopter!", /* in GTA games, the army comes for you if your cop wanted level is maxed */
			"encounter a woman who looks dressed like a cop, but she isn't actually a cop, as she's not trying to bust you; in fact, she's wearing high heels! Who the hell is she?", /* a meter maid */
		};
		if (rn2(5)) You("%s", kopstation_msg[rn2(5+hallu*5)]);
		else verbalize("Alert! All units, apprehend %s immediately!", playeraliasname);
		return;
	    }

	    if (level.flags.has_bossroom && !rn2(200)) {
		static const char *bossroom_msg[] = {
			"feel that you're in a hopeless situation.", /* this and most of the others are inspired by Pokemon */
			"are left hanging in suspense.",
			"feel that the future is dark, and it gets even darker.",
			"realize that the challenge of your life awaits you.",
			"get the feeling that a true champion is going to challenge you soon.",
			"just know that you'll be having a legendary battle.",
			"will not be able to defeat Whitney's Miltank, because the fleecy Whitney will mop the floor with you wimp!",
			"are not going to get past Pryce, since his ice pokemon can crash the game at will.",
			"have to fight Lance and his Dragonite, but with your uselessly weak pokemon you don't stand a chance.",
			"loser will never beat Gary, whose Meganium always uses 'Contro' and defeats each of your pokemon in one hit, hahaha!",
			"are challenged by Clair, the dragon pokemon trainer, whose name in the German version is 'Schwaermelbueschelsandra' and who will kick your butt with her unbeatable Kingdra!",
			"encounter a wild Suicune, which is even a *legendary* pokemon and since you don't have a master ball, catching it comes all down to luck!",
		};
		You("%s", bossroom_msg[rn2(6+hallu*6)]);
		return;
	    }

	    if (level.flags.has_rngcenter && !rn2(200)) {
		static const char *rngcenter_msg[] = {
			"sense the shuffling of random numbers.",
			"hear the dungeon master testing a set of dice.",
			"feel that this particular floor is especially random.",
			"have an unknown feeling.",
			"feel that the RNG is putting your abilities to the test.",
			"just know that the monsters on this level have the most evil and unbalanced attacks possible.",
			"test your d20 several times, only to realize in shock that it always rolls 1!",
			"are invited to a game of Dungeons & Dragons by the game master!",
			"listen to some gamers rolling dice.",
			"think it's really unbalanced that 'random damage type' monster attacks can roll rider-grade attacks.",
		};
		You("%s", rngcenter_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_wizardsdorm && !rn2(200)) {
		static const char *wizardsdorm_msg[] = {
			"hear a grating, annoying snore.", /* the sleeping wizard */
			"smell dragonbreath in the air.",
			"hear a 'Klieau!' sound.", /* sound that the manticore's tailspike attack supposedly makes */
			"feel the presence of arcane energies.",
			"listen to a spellcasting chant.",
			"are hit by the 1st tailspike! You are hit by the 2nd tailspike! Warning: HP low! You are hit by the 3rd tailspike! You die. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
			"want to do wizard farming like in the old days, since it's more profitable than pudding farming.", /* in Castle of the Winds, that was actually one of the easiest ways to get lots of items */
			"listen to someone ushering his pets to attack you!", /* the wizard ordering his manticores to kill you */
			"suddenly see nightmare figures invading your village and murdering the townsfolk! You gotta stop them!",
			"forgot to renew your detect monsters spell and the manticore shoots you with tailspikes from outside your line of sight.",
		};
		You("%s", wizardsdorm_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_doomedbarracks && !rn2(200)) {
		static const char *doomedbarracks_msg[] = {
			"hear a sound reminiscent of a frog.", /* Doom 2, if there's monsters nearby you can often hear an ultra-annoying "Krr-kro-krrg!" sound that used to drive me fucking nuts */
			"feel that the gate to Hell has to be closed from the other side.",
			"realize that the aliens have built a base here.",
			"hear someone announce 'Wonnen.'", /* yeah I know it's supposed to be 'warning'... sue me :P --Amy */
			"hear the teleportation sound.",
			"hear an elevator sound.",
			"hear a deep voice chanting the name 'Jason'...", /* from a Doom 2 wad */
			"hear someone shout 'Neau!'", /* from a different Doom 2 wad */
			"are no longer invulnerable even though you used the IDDQD code!",
			"fell into a poisonous pit and the IDCLIP cheat doesn't seem to work! Now you'll die very slowly and painfully, you poor bastard.",
			"encounter a little child that makes an 'Aloe!' sound! He looks deceptively similar to Commander Keen...",
			"must run past the cyberdemon to fish the red key map out of the river while dodging the rockets. Good luck, you'll need it.", /* yet another Doom 2 wad */
		};
		You("%s", doomedbarracks_msg[rn2(6+hallu*6)]);
		return;
	    }

	    if (level.flags.has_sleepingroom && !rn2(200)) {
		static const char *sleepingroom_msg[] = {
			"felt like you heard a faint noise, but can't make out what it is.", /* the moan of a sleeping monster */
			"hear a loud yawn.",
			"get the impression that the current atmosphere is quiet... too quiet.",
			"feel that it's time to take revenge.", /* inspired by Castle of the Winds, and also some of the others */
			"know that some fell creatures are encamped in this dungeon level.",
			"wonder where the denizens of the dungeon came from.",
			"listen to a soothing melody and a chant that sounds like 'Sleep-bundle-wing!'", /* Elona sleeping music */
			"suddenly fall asleep. (Hopefully no monster decides to feast on your exposed flesh before you wake up.)",
			"decide to stop at nothing until the threat is removed.",
			"overhear the patrol's orders. To your shock, they're talking about plans to KILL you!",
			"wonder where the clothes and shield hanging over the blacksmith's forge have gone.",
			"suddenly realize that when your hometown was destroyed, the evil overlord did so because he specifically wanted to kill YOU!",
		};
		You("%s", sleepingroom_msg[rn2(6+hallu*6)]);
		return;
	    }

	    if (level.flags.has_diverparadise && !rn2(200)) {
		static const char *diverparadise_msg[] = {
			"inhale the scent of seawater.",
			"listen to fishes splashing water around.",
			"listen to the sound of gentle waves.",
			"hear children having fun on the water slide.",
			"hear the rushing noise of water.",
			"hear a 'splash splash' noise!",
			"hear someone announce 'The entrance to Diver's Paradise is free today! Come on over and enjoy the most beautiful terrain in this dungeon - water!'",
			"suddenly see a swimmer in bathing shorts run past you, and my god, does he have an absolutely UGLY tattoo on his leg!",
			"wonder why fat people are always wearing tight bathing shorts. Having to see that makes you really uncomfortable...",
			"encounter a guy who wears almost no clothes, and as he approaches you, he suddenly pulls down his pants and starts wanking off... ewwwwwwww...",
		};
		You("%s", diverparadise_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_menagerie && !rn2(200)) {
		static const char *menagerie_msg[] = {
			"can hear animal sounds.",
			"hear the sound of a caged animal knocking at the lattice.",
			"hear the roar of hungry zoo animals.",
			"hear an animalistic moan.",
			"hear a hissing sound, and it's right behind you!",
			"look behind you and see a three-headed monkey!",
			"hear an announcement: 'WARNING: The tiger's cage has been broken! There might be a tiger on the loose!'",
			"feel like getting a pet spidermonkey or gorilla.",
		};
		You("%s", menagerie_msg[rn2(4+hallu*4)]);
		return;
	    }

	    if (level.flags.has_emptydesert && !rn2(200)) {
		static const char *emptydesert_msg[] = {
			"feel that the air is hot around here.",
			"feel a lack of humidity.",
			"hear the roaring of a tornado.",
			"hear the flapping noises of a condor's wings.",
			"seem to sink into the floor!",
			"encounter a sweet asian girl, but as you move to greet her, she suddenly thrusts you into quicksand! You die. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
			"hear the top of the pyramid crackling and breaking off!", /* Super Mario 64 */
			"must stand tall on all four columns without falling into the shifting sand!",
		};
		You("%s", emptydesert_msg[rn2(4+hallu*4)]);
		return;
	    }

	    if (level.flags.has_rarityroom && !rn2(200)) {
		static const char *rarityroom_msg[] = {
			"hear uncommon noises.", /* the monsters in the room are rare, so the noises they make aren't heard often */
			"can feel the earth shaking.", /* monster stomping the ground */
			"sense a supernatural presence.",
			"feel a conventional weirdness.", /* the monsters are actually rather mundane, but rare, so that's why it's weird that there are so many of them */
			"know that you'll see something not meant for mortal eyes.",
			"hear Pale Night tempting you to remove her veil and look at what's underneath...",
			"know that you can beat the impossible odds!",
			"listen to someone scream 'YES! I won in the lottery! I'm a millionaire now!!!'",
		};
		You("%s", rarityroom_msg[rn2(4+hallu*4)]);
		return;
	    }

	    if (level.flags.has_prisonchamber && !rn2(200)) {
		static const char *prisonchamber_msg[] = {
			"feel that you must free someone who's being held captive.", /* Castle of the Winds, level 4 of the castle where you have to free the prisoner */
			"hear someone calling for you.",
			"must free the prisoner unless you want him to be executed.",
			"have a limited amount of time to save the prisoner or else he gets killed.",
			"are already hearing drums.",
			"were too slow and now the prisoner is dead. You failure!",
			"really have to hurry - if you don't free the prisoner in 10 turns, he will be killed!",
			"were so stupid and accidentally attacked the prisoner, causing him to die! Now you'll never get the reward for freeing him!",
			"procrastinated for too long and now the prisoner is history. You failure!",
			"noticed that the drums are beating faster!",
		};
		You("%s", prisonchamber_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_nuclearchamber && !rn2(200)) {
		static const char *nuclearchamber_msg[] = {
			"realize that your Geiger counter has started ticking like mad.",
			"smell the stench of contamination in the air.",
			"listen to scientists talking about their experiments.",
			"hear the sounds of a nuclear facility.",
			"detect gamma radiation.",
			"feel the presence of the ether generator!",
			"can overhear the president saying 'That was the last straw! I'll bomb that fucking country now! I just need to get the code and press the big red button!'",
			"are in the extended hazardous course and will have to beat it without cheats, which is very difficult!", /* Hazardous Course 2, also known as Super Kaizo Half-Life */
			"fear that the politicians will abolish their nuclear peace treaties!",
			"are glad that you have a HEV suit, which must be very HEVvy.",
		};
		You("%s", nuclearchamber_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_variantroom && !rn2(200)) {
		static const char *variantroom_msg[] = {
			"sense the presence of beings from another universe.",
			"hear the elder priest fiercely lashing his tentacles around.", /* dnethack */
			"feel an earthquake-like trembling, probably caused by a migmatite horror...", /* steamband */
			"hear Morgoth muttering a chant.", /* angband */
			"hear the scream of Lina Inverse's high-pitched voice.", /* animeband */
			"receive a stamped scroll from the mail demon! It reads: 'Muahahahaha, I kidnapped Princess Peach again. Sincerely, Bowser.'",
			"start eating the hill giant corpse. Ulch - that meat was tainted! You die from your illness. Do you want your possessions identified? DYWYPI? [ynq] (n) _", /* Grunthack */
			"suddenly realize that the elder priest stands right before you, and unfortunately, it's his turn now. Say goodbye to your cloak of magic resistance!",
			"allowed your speed spell to wear off, and as a result Morgoth gets a double turn and casts mana storm twice in a row. You die.",
			"are lucky, because you encountered Animeband's instakill monster, and it kills you! Game over!",
		};
		You("%s", variantroom_msg[rn2(5+hallu*5)]);
		return;
	    }

	    if (level.flags.has_coinhall && !rn2(200)) {
		static const char *coinhall_msg[] = {
			"notice the sound of clinging coins.",
			"feel that, somewhere down here, there might be a whole lot of gold.",
			"hear metal banging against a wall.", /* the metal is the gold, of course */
			"hear a tingling-clinging sound.",
			"hear Scrooge McDuck taking a bath in his money!",
			"hear the sound of a shower and then... coins???", /* Scrooge's gold coin shower */
			"don't know why purple rain is so popular, because gold rain is much better...",
			"feel extremely bored! Why does the dungeon master keep sending boring chaff monsters instead of something more dangerous?", /* players have complained that coin halls are one of the most boring special rooms */
		};
		You("%s", coinhall_msg[rn2(4+hallu*4)]);
		return;
	    }
	    if (level.flags.has_angelhall && !rn2(200)) {
		static const char *angelhall_msg[] = {
			"hear the sound of wispy wings.",
			"can barely hear a whispering voice.",
			"sense a glow of radiance.",
			"listen to someone announcing that you need penance.",
			"feel like you are in the presence of a god.",
			"suddenly feel an itching sensation inside your brain.",
			"think you just heard God speaking to you!",
			"notice that the stupid little angel is always flying in a circle.",
			"forgot your golden spirit gel that you could use to clip the angel's wings.",
			"see the angel flap its wings. One of them brings destruction while the other brings prosperity.",
			"begin to shed tears of joy without even realizing it.",
			"feel the angel's beauty taking your breath away...",
		};
		You("%s", angelhall_msg[rn2(6+hallu*6)]);
		return;
	    }
	    if (level.flags.has_elemhall && !rn2(200)) {
		static const char *elemhall_msg[] = {
			"suddenly feel a wave of heat.",
			"notice some heaps of earth.",
			"sense a strong gust of wind.",
			"hear a wave of water.",
			"feel the presence of astral beings.",
			"hear distant thunder.",
			"get the impression that the laws of Nature itself are conspiring against you!",
			"suffer from severe inundation!",
			"realize that you're on a hot plate that was just turned on! Step off or you'll get the tip of your toes burned!",
			"are sitting in an electric chair and have to survive a current of 20 watt for half an hour!",
			"realize that a tornado has engulfed the entire dungeon and the ceiling starts to collapse...",
			"wonder why 'astral' elementals exist, because astral is not an element.",
			"can hear Laura chanting spells.", /* she's supposed to be a master of elemental spells */
		};
		You("%s", elemhall_msg[rn2(6+hallu*7)]);
		return;
	    }

	    if (level.flags.has_evilroom && !rn2(200)) {
		static const char *evilroom_msg[] = {
			"are very afraid for some reason.",
			"listen to a really evil sound.",
			"hear a very deep grunt.", /* grunthack :D */
			"sense a great danger.",
			"hear measurers and forks.", /* Nethack Fourk */
			"hear someone sharpening the spoon fork.", /* Sporkhack */
			"somehow feel that the default rules are not valid here.",
			"are afraid that this place will be rather unfair to you.",
			"can tell from the sounds that the monsters on this level are spectacularly unforgiving.",
			"know that you won't make it through this level alive...",
			"will die, but not before you've witnessed the evil perpetrators taking your beloved toys and breaking them into pieces.",
			"are going to get disintegrated even if you are resistant.", /* Nethack Fourk */
			"will never be able to kill a bugbear in less than 5 hits, and woe to you if an actually difficult monster comes, e.g. a stone golem.", /* Sporkhack */
			"don't look forward to encountering the elder priest at all.", /* dnethack */
			"just love it when intrinsically obtained resistances time out.", /* ditto */
			"have to eat hundreds of corpses to obtain full resistance.", /* sporkhack */
			"feel that magic resistance will not protect you from the destroy armor spell.", /* ditto */
			"finally understand that interface screws are not fake difficulty.",
		};
		You("%s", evilroom_msg[rn2(9+hallu*9)]);
		return;
	    }
	    if (level.flags.has_religioncenter && !rn2(200)) {
		static const char *religioncenter_msg[] = {
			"hear religious chants.",
			"hear the sound of a preacher.",
			"listen to some gibberish talk.",
			"hear voices chanting a prayer.",
			"sense an evil, awful stench.",
			"sense a cultural clash.", /* sadly, some religions do not get along very well with each other */
			"hear someone call for the elimination of infidels.",
			"hear a frightening 'Hola-hola!' call!",
			"hear someone shout 'Allahu Akbar!'",
			"have to resist the urge to vomit from the terrible smell.",
			"must prevent the hostile overtaking or everything you hold dear will be lost.",
			"suddenly meet someone who accuses you of being an intolerant asshole.",
			"know that the statistics don't lie and the alien infestation is a real threat!",
			"overhear plans for a destructive bomb notice!",
		};
		You("%s", religioncenter_msg[rn2(7+hallu*7)]);
		return;
	    }
	    if (level.flags.has_cursedmummyroom && !rn2(200)) {
		static const char *cursedmummyroom_msg[] = {
			"sense the eerie silence.",
			"feel the presence of ancient beings.",
			"have a primordial feeling.", /* the lich in that room is a really ancient being */
			"are struck with awe at the sight of these structures.", /* refers to the architecture to the mummies' tomb */
			"definitely feel the presence of a really ancient corpse.",
			"hear the frightening sounds of something you do NOT, EVER, want to anger.", /* Elona big daddy */
			"realize that you've trespassed in the tomb of an ancient pharao.",
			"are afraid of accidentally equipping cursed items.",
			"hear the dead walking.",
			"hear the dead potatoes walking. And the dissolved undead potatoes, returned potatoes and whatever other kind of potatoes as well.",
			"hear someone singing a song that always goes 'Oceanos de oro, y tumbas de sal!' Meaning: oceans of gold, and barrels of salt!", /* yeah I know, it's actually tombs of salt */
		};
		You("%s", cursedmummyroom_msg[rn2(5+hallu*6)]);
		return;
	    }
	    if (level.flags.has_arduousmountain && !rn2(200)) {
		static const char *arduousmountain_msg[] = {
			"feel that this will be rather difficult.",
			"know that until the very end, you have to give it your best.",
			"sense a great deal of variety.",
			"seem to sense the weather changing.", /* weather in the mountains can change fast and unexpectedly */
			"sense the change of air pressure.",
			"feel that the air is getting thin.",
			"must contest the harder part of the Adventurer Zone!", /* Mushroom Kingdom Fusion */
			"feel that in the name of Caesar, Carthage MUST be destroyed!",
			"switch out your lamp for a climbing set.", /* ToME has climbing tools for scaling mountains but sadly they use the same slot that light sources occupy */
			"realize that your chosen character class is hard to play for the entire length of the game.",
			"wish you had a dumbo to ride on.",
			"wonder how a legion would deal with an icy staircase.",
		};
		You("%s", arduousmountain_msg[rn2(6+hallu*6)]);
		return;
	    }
	    if (level.flags.has_changingroom && !rn2(200)) {
		static const char *changingroom_msg[] = {
			"hear women talking about clothing.",
			"listen to giggling sounds.", /* girls while changing clothes */
			"feel as if messages are being transmitted via bush telegraph methods.",
			"sense a scentful smell.",
			"hear a hollow beating sound.", /* someone tapping their shoes on the ground */
			"feel that someone around here is unusually busy.",
			"dream of a little black dress, even though you don't have any reason for bereavement.",
			"absolutely want a pair of leather boots with bright green block heels!",
			"look forward to buying several pairs of 'Janina' underwear.",
			"try to picture AmyBSOD in front of your mental eyes, with her wearing only high heels and pants.", /* inspired by bhaak */
			"wonder whether you're the peeping tom?", /* Pokemon, Erika's Gym, don't remember which edition but probably yellow? */
			"feel like you've not changed your underwear for a whole day, so it's time to change them now!",
		};
		You("%s", changingroom_msg[rn2(6+hallu*6)]);
		return;
	    }
	    if (level.flags.has_levelffroom && !rn2(200)) {
		static const char *levelffroom_msg[] = {
			"hear laughing sounds.",
			"listen to dissonant beeps.", /* supposed to be the Metroid "Beepbebeep beep BEEP beep, beep beep BEEP beep!" "music" */
			"have the feeling that there will be errors.", /* many of these are inspired by various glitches in Castle of the Winds */
			"hear an asynchronous sound.",
			"realize some unexpected hard disk activity.",
			"have to use phase door if you want to get inside.",
			"wonder whether teleportation will work on this floor.",
			"wonder whether shooting the percents will make it go away.",
			"have the urge to click on that gray tile!",
			"get a NTLL - Not a valid save file!",
			"suddenly notice the superdeep type over there!",
			"scrolled too far down and the ultrayousee ends your game. Sorry.",
			"just lost another continue! If you keep playing that badly, you'll run out and be eliminated prematurely!",
			"need to use clairvoyance in a very specific place or you won't be able to teleport!",
			"suddenly remember the highscore cheat that Amy's roommate discovered!",
			"are challenged by Julietta, and have to defeat her in the auto racer duel!",
		};
		You("%s", levelffroom_msg[rn2(8+hallu*8)]);
		return;
	    }
	    if (level.flags.has_verminroom && !rn2(200)) {
		static const char *verminroom_msg[] = {
			"hear chittering sounds.",
			"hear hissing noises.",
			"feel as if there's rats in the walls.",
			"notice a foul stench in the air.",
			"fell into an open sewer hole!",
			"think that it must be raining outside, judging by the ratty smell.", /* canal lids on the ground sometimes emit a horrific smell when it's raining... */
			"suddenly bear witness to a huge invasion of ants, rats, cockroaches and other vermin!",
			"feel that the cockroach is an abomination of nature, no animal should have such HUGE antennae!",
		};
		You("%s", verminroom_msg[rn2(4+hallu*4)]);
		return;
	    }
	    if (level.flags.has_questorroom && !rn2(200)) {
		static const char *questorroom_msg[] = {
			"feel like you should be working.",
			"seem to hear your superiors calling your name.",
			"suddenly meet one of your colleagues who is just passing by.",
			"absolutely don't want to visit the canteen.", /* because the "food" that is served there gives your stomach trouble */
			"think you've slacked off for long enough, time to get back to work.",
			"feel that the quest leader is looking at you with *the eye*, waiting for you to finally complete that quest.",
			"wonder whether that cute secretary girl will flirt back today.",
			"are late for work! Now your boss will give you a final written warning!",
			"find out that while you didn't lose your job yet, you have to continue working at 20%% reduced pay, and if you make one more mistake it'll get lowered even more.", /* inspired by jonadab */
			"feel extremely nauseated all of a sudden. Maybe it was a mistake after all to eat canteen calls-itself-food.",
			"are sooooooo fed up with your annoying colleague! Having to see that same insufferable person every day makes you want to vomit!",
			"decide that if anyone calls you out for slacking off, you'll just tell them that your code's compiling.",
			"remember the last letter of your boss, wherein he said that you've been put on probation for six months and if you make any mistake during that time, your pay will be reduced further. Also, you are now responsible for making sure that no customers complain.", /* inspired by jonadab */
			"are suddenly caught by your supervisor, and he says 'Okay, so your values are all wrong. So now you won't get paid for the work you did today, the faculty manager will be notified, and we will have a close eye on your work from now on to ensure that you do your job properly next time!'",
		};
		You("%s", questorroom_msg[rn2(7+hallu*7)]);
		return;
	    }
	    if (level.flags.has_miraspa && !rn2(200)) {
		static const char *miraspa_msg[] = {
			"hear a female voice praising her spa.",
			"notice an overwhelming stench of urine.",
			"hear a distorted splash.",
			"listen to a complaining customer and a female voice saying 'Nope, you don't get your money back, sorry.'",
			"hear a constant peeing noise.",
			"encounter Mira, and she asks: 'Do you want to bathe in my urine? Only 20 zorkmids for one hour of pure fun!'",
			"suddenly see the skeletons of people who died in a most painful way!",
			"can't wrap the head around Mira's fantasies.",
			"are certainly turned OFF bathing for now. Did Mira really pee into the basin???", /* she did... */
			"notice the stench of ammonia.",
		};
		You("%s", miraspa_msg[rn2(5+hallu*5)]);
		return;
	    }
	    if (level.flags.has_machineroom && !rn2(200)) {
		static const char *machineroom_msg[] = {
			"hear clatter.",
			"hear the grinding gears of industry.",
			"hear busy, hectic noises.",
			"realize that you're in the industrial district now.",
			"hear the sound of a dump truck.",
			"hear the track bed being gravelled.",
			"hear the taunts of the motherfucking Glass Golem!",
			"listen to the sound of a truck rolling over a road.",
			"wonder how long the industrial highway is, it seems to be endless?!",
			"hear a 'choo-choo' sound and as you turn around, you see a TRAAAAAAAAAIN! YAAAAAAAY! It's headed to Trier, so it's probably a Sueueueueueuewex? :-)",
			"doze off into a peaceful slumber as the trains keep passing through the station once every 30 minutes.",
			"suddenly wonder why there's so much traffic.",
		};
		You("%s", machineroom_msg[rn2(6+hallu*6)]);
		if (uarmf && itemhasappearance(uarmf, APP_INDUSTRIAL_BOOTS) && !u.uspellprot) {
			u.uspellprot = 4;
			u.uspmtime = 10;
			if (!u.usptime) u.usptime = u.uspmtime;
			find_ac();
			flags.botl = TRUE;
			You_feel("strangely protected. Could be due to your boots.");
		}
		return;
	    }
	    if (level.flags.has_showerroom && !rn2(200)) {
		static const char *showerroom_msg[] = {
			"hear water currents.",
			"feel like dancing in the rain.",
			"listen to a mix of splashes.",
			"hear the shower running.",
			"inhale the smell of brine.",
			"hear a thunderous rumble.",
			"listen to the emergency service trying to repair a broken water pipe.",
			"hear someone scream 'HELP! I can't swim! Save me please, I'm drowning!!!",
			"realize that the sorcery adept doesn't know how to stop the water...", /* some poem that I forgot the name off, maybe "the mage adept"? the poor sap wants to fill the bathtub with water by using a spell but forgot how to stop the helpers he summoned */
			"detect a flooding on this sub level.",
			"have to dive through the next segment, and it's too long for you to hold your breath all the way. Which means that you have to find the hidden underwater oxygen tanks and if you miss one, you'll drown.",
			"want to use the shower, but someone rigged it so the water coming out of it is really acid! You gotta avoid that shower!",
		};
		You("%s", showerroom_msg[rn2(6+hallu*6)]);
		return;
	    }
	    if ((level.flags.has_greencrossroom || (at_dgn_entrance("Green Cross")) ) && !rn2(200)) {
		static const char *greencrossroom_msg[] = {
			"hear some mundane talk.", /* most of these are Castle of the Winds-inspired */
			"listen to the cackle of poultry.",
			"feel like reaching the next town.",
			"seem to be in a nice atmosphere.",
			"hear pages turning.",
			"listen to the villagers' conversations.",
			"want to check whether the secret path is open.",
			"inhale the sharp smell of burning thatch!",
			"suddenly see the hill giant lord throwing enormous rocks on the shops! And he's killing the shopkeeper with a giant club! Ack!",
			"must be in a town that's not very easy to reach.",
			"hacked into Arabella's chat, allowing you to monitor her talk and that of her girlfriends. And you realize that Henrietta complains about you having gotten away from her because she can't follow you now.",
			"need to squeeze past all the statues with the ancient white dragon breathing down your neck!",
			"have 100 turns left to find the secret entrance to an otherwise unreachable dungeon level before the hidden warp portal dissipates.",
			"somehow feel that the secret advice hussies can't get you for now.",
		};
		You("%s", greencrossroom_msg[rn2(7+hallu*7)]);
		return;
	    }
	    if (level.flags.has_ruinedchurch && !rn2(200)) {
		static const char *ruinedchurch_msg[] = {
			"feel a haunting presence.",
			"hear a chiming bell.",
			"hear a C major chord.",
			"feel that all sanctity of this place has been lost.",
			"are sure that the dead are floating around here.",
			"hear an orchestral melody that goes 'Daaaaa... daaaaaa... DAAAAAAAAAAAAAAA!!!'",
			"notice a sound: 'Ooeoeoeoep! ... Ooeoeoeoep!' Suddenly someone shouts 'Ey, that's not called 'Ooeoeoeoep', that's a church bell, didn't you know that!'",
			"remember the bible saying that you should do unto others what you want others to do unto you.",
			"know that practice makes perfect. Yeah, yeah.",
			"hear someone announce 'Way to go! New record! You are the champion!!'",
		};
		You("%s", ruinedchurch_msg[rn2(5+hallu*5)]);
		return;
	    }
	    if (level.flags.has_gamecorner && !rn2(200)) {
		static const char *gamecorner_msg[] = {
			"hear the Tetris melody.",
			"listen to someone playing Diablo 2.",
			"seem to catch earshot of people playing Dungeons and Dragons.",
			"hear the familiar melody of Mario Kart 64.",
			"listen to the typical Electronic Arts games slogan.",
			"seem to catch earshot of a dragon shout from Skyrim.",
			"encounter a tiny lag that indicates monsters being spawned.",
			"have to click OK on the shareware notice.",
			"see the accursed crashy graphics card logo.",
			"wonder whether this game has control stick support.",
			"hear a speedrunner curse at Super Kaizo Mario because the game is too damn hard!",
			"suddenly feel that you're really playing GTA! A cop car appears! The cop wields a shotgun! The cop fires a shotgun shell!--More--",
			"lost your last hitpoint and jump out of the picture!",
			"suddenly feel like just standing there and letting a sexy prostitute kick you in the nuts repeatedly.",
			"want to be expenditure-feasted by the sharp claws!",
			"absolutely long to have a succubus slowly and very painfully scratch you with her erotic blade!",
			"feel that the developer inserted even the most impossible bugs into the game that a normal mortal could never conceive!",
			"feel that the colors are very fleece-bouffant!",
			"hear someone say 'Die-Far-ben-sind-sehr-flau-schel-bau-schig!'",
			"push buttons and your stupid PC responds by making annoying 'DIE-DIE-DOE' sounds instead of executing your commands!",
		};
		You("%s", gamecorner_msg[rn2(10+hallu*10)]);
		return;
	    }
	    if (level.flags.has_illusionroom && !rn2(200)) {
		static const char *illusionroom_msg[] = {
			"feel a little confused about this place.",
			"feel that things aren't necessarily really there just because you can see them.",
			"are sure that some strange presences are on this floor.",
			"need to stay alert for anything out of the ordinary.",
			"have an everchanging feeling.",
			"feel slightly irritated and annoyed at something.",
			"notice that your weapon has no effect! Get a shield!",
			"can't think about difficult things or your brain will asplode!",
			"forgot that you were suffering from amnesia! Oh no!",
			"decide that you'd rather not and we'll just pretend that you did so you can move on.",
			"hope that one of the gods can help you out of your predicament...",
			"should stop thinking of eldritch abominations all the time or you'll end up permanently confused!",
		};
		You("%s", illusionroom_msg[rn2(6+hallu*6)]);
		return;
	    }

	    if (level.flags.has_terrorhall && !rn2(200)) {
		static const char *terrorhall_msg[] = {
			"feel weirded out.",
			"sense something strange about this place.",
			"think you just saw something move.",
			"feel terrified.",
			"hear the walls crumbling down.",
			"hear an eerie curse.",
			"think you're seeing white rabbits!",
			"feel totally down, like you're on a bad trip!",
			"feel reminded of the am shop notice!",
			"develop an acute case of agoraphobia because of all the car notices!",
			"listen to someone flat a notice on you!",
			"hear the all-too-familiar telltale sound of an incoming airstrike! RUN! TAKE COVER!",
		};
		You("%s", terrorhall_msg[rn2(6+hallu*6)]);
		return;
	    }

	    if (level.flags.has_insideroom && !rn2(200)) {
		static const char *insideroom_msg[] = {
			"have a weird feeling of chills and excitement.",
			"sense that something isn't right here...",
			"shudder in expectation.",
			"listen to Arabella saying 'The inside part is too dangerous for me.'",
			"listen to Arabella saying 'Please be careful in the inside part.'",
			"listen to Arabella saying 'That place is so perilous, even I won't be entering.'",
			"listen to Arabella saying 'Take care. There's nasty traps everywhere.'",
			"listen to Arabella saying 'Even I can't remember all the nasty traps I placed on this dungeon level.'",
			"listen to Anastasia saying 'My vacuum cleaner is waiting for you.'",
			"listen to Henrietta saying 'You won't get away from me this time.'",
			"listen to Katrin saying 'Why don't you come over here and have some fun with me?'",
			"listen to Jana saying 'I'm well hidden, you won't even find me.'",
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
			"listen to Arabella saying 'Stop stalking me all the time, you're creepy.'",
			"listen to Arabella saying 'You're not Marc, and therefore you won't ever defeat me.'",
			"listen to Arabella saying 'You admitted that you want to grope my girl butt. Time to join #metoo and tell them that you're a sexual predator.'",
			"listen to Arabella saying 'What, you thought I stepped into dog shit because I want to clean my shoes myself? No, *you* will do that now, by hand and without tools.'",
			"listen to Arabella saying 'If you get too close to me, I will push your shoes into a heap of shit. Then, no matter what you do to me, you'll at least have to clean the shit off your shoes again.'",
			"listen to Anastasia saying 'Thanks for cleaning the dog shit off my profiled dancing shoes. Now I'll find a new heap of shit to step into, and if you like, you may then clean my shoes again. Was it fun?'",
			"listen to Henrietta saying 'Because of you I stepped into dog shit with my Henrietta shoes. As a punishment you have to clean them by hand now.'",
			"listen to Katrin saying 'Please let me stick my soft chewing gum on your skin! Then I can very slowly and extremely painfully take it off again, ripping off your skin in the process.'",
			"listen to Jana saying 'Even if you find me, I'll just teleport away and prevent you from defeating me! And your skill certainly isn't good enough to reach Sven, so you won't be fulfilling the alternate victory condition either!'",
			"encounter the annoying bug where the game just closes with no message.",
			"suddenly find yourself at the MS-DOS prompt. The game crashed so hard that the entire Windows 3.1 operating system just stopped running, without even giving a message or anything.",
		};
		You("%s", insideroom_msg[rn2(12+hallu*29)]);
		return;
	    }

	    if (level.flags.has_riverroom && !rn2(200)) {
		static const char *riverroom_msg[] = {
			"hear seagulls.",
			"hear waves on sand.",
			"hear something fishy happening.",
			"hear the trickle of water.",
			"sense a watery smell mixed with grass.",
			"listen to an unusual ambient sound.",
			"listen to a song that goes 'Leyde, coming down to the ri-ver...'",
			"suddenly start humming the tune of 'Green River' by CCR.",
			"hear a fountain crash through solid rock with a mighty roar!",
			"admire the beautiful mountains around here. Strange, how can mountains be in an underground dungeon?",
			"return to your hiding place, only to find a cursed called. Shit.",
			"return to your weapon camp, but to your dismay Arabella has looted almost everything, leaving only a single wand of magic missile for you.",
			"get the dreaded 'Can't write to save file' error. Bullshit.",
			"hear burly voices singing shanties.",
			"here someone ask about warez.",
			"hear a speedboat.",
			"hear a squeaking rubber duck!",
			"hear a loud advert for Tide.",
			"hear a commercial for protein bars.",
			"hear a shark jumping!",
			"hear a gull asking for a bite of your sandwich.",
			"hear a foghorn!",
			"hear a sea shell exploding!",
			"hear a C-shell script running.",
		};
		You("%s", riverroom_msg[rn2(8+hallu*16)]);
		return;
	    }

	    if (level.flags.has_tenshall && !rn2(200)) {
		static const char *tenshall_msg[] = {
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
		You("%s", tenshall_msg[rn2(3+hallu*7)]);
		return;
	    }
    if (level.flags.has_zoo && !rn2(200)) {
	static const char * const zoo_msg[] = {
		"hear a sound reminiscent of an elephant stepping on a peanut.",
		"hear a sound reminiscent of a seal barking.",
		"hear David feeding his animals.",
		"hear a roar.",
		"feel like there will be steak tonight.", /* cooked from the zoo animals, obviously */
		"hear Doctor Doolittle!",
		"hear someone asking 'Who the fuck is David?'",
		"hear a kid ask 'Ohh, look at all the cute zoo animals! May I feed them? May I pet them? Would you mind if I incinerate them?'",
		"feel like calling PETA, since it's cruel if animals are bred in captivity.",
		"absolutely feel that people eat tasty animals. In fact, you feel like eating one right now!",
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping || is_animal(mtmp->data)) &&
		    mon_in_room(mtmp, ZOO)) {
		You("%s", zoo_msg[rn2(5+hallu*5)]);
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
	    static const char * const shop_msg[] = {
		    "someone cursing shoplifters.",
		    "the chime of a cash register.",
		    "a barker praising the wares of a store.",
		    "Neiman and Marcus arguing!",
		    "someone chasing after a shoplifter!",
		    "a shifty voice say 'Fear not, our wares will definitely last for a whole life and come with a two-day warranty!'", /* Gheed from Diablo 2 */
	    };
	    if (rn2(50)) You_hear("%s", shop_msg[rn2(3+hallu*3)]);
	    else { /* sporkhack; I know they don't always fit, but oh well --Amy */
		static const char * const shop_sporkmsg[] = {
			"Any %s would love these!  Finest quality!",
			"Fit for a Knight, but they'll last for weeks!",
			"It's dangerous 'round here these days... better wear something safe!",
			"Hey, %s, I've got something here that'll fit you perfectly!",
			"Guaranteed safety or double your money back!",
			"If any of the armors you buy here doesn't save your life, you get your money back of course!",
			"'ere now, this isn't a library; get lost, you freeloader!",
			"Waterproof ink upon request!  ... for a small surcharge.",
			"Curses removed, gold detected, and weapons enchanted, at your whim!",
			"If you can read, %s, you'll want some of these!",
			"Booze on ice!  Getcher booze on ice!",
			"Come on, %s.  You know you're thirsty.",
			"Ahhh, it'll put hair on yer chest!",
			"Lowest percentage of cursed items around!",
			"Sharpest weapons around! On sale, today only!",
			"We sell 'em, you stab 'em!",
			"Guaranteed to not dull for ten fights or your money back!",
			"Look, %s, with a face like that you'll be in a lot of fights.  Better buy something now.",
			"You'll never slash the same again after one of ours!",
			"Gitchore luvverly orinjes!",
			"Fresh fish! So fresh it'll grab yer naughty bits!",
			"Sausage inna bun!  Hot sausage!",
			"Genuine pig parts, these. So good most pigs don't even know they got 'em.",
			"Well, you seem like a fine, discerning young %s; come look at this.",
			"Eat hot and tasty pizza here!",
			"We sell the best food in the world: the Doenerteller Versace!",
			"Do you like McDonalds, %s? Well, our fries are even better than theirs, so come on over!",
			"Come over %s, and taste the one and only Shawarma you'll get in the entire dungeon!",
			"Yo %s, this way to the Burger King restaurant! We know you're hungry!",
			"Special sparklies for a special %s, perhaps?",
			"Once you put one of ours on, you'll never want to take it off!",
			"Our bands never break or melt!",
			"Shiny, isn't it?",
			"Credit available for valued customers!",
			"We offer premium quality inka rings! Guaranteed to make your woman use her sexy butt cheeks to shower you with love!",
			"The other shops may be trying to sell you amulets of strangulation, but we have something that saves your life instead!",
			"Bugger off, you filthy little %s. Don't come begging around here!",
			"Straightest zaps anywhere!  100%% money back guarantee (less usage)!",
			"Our wands explode less than all others!",
			"New EZ-BREAK feature on these in case of emergency!",
			"Dude, %s, I have a wand of wishing for sale today. Buy now before someone else does!",
			"Tins opened, faces wiped, gazes reflected; your one-stop shop!",
			"How you gonna carry all your stuff without a bag, %s?",
			"Must be hard kickin' all those doors down, I bet a key would help...",
			"Only tools wouldn't buy our tools!",
			"Large print available!",
			"Mental magnificence for the scholarly IN-clined!",
			"'Banned' section now open! (I.D. required)",
			"Hey, %s! Best candles in Minetown! You'll need 'em later, count on it!",
			"You've got a long way down yet, %s.  Be sure you're ready.",
			"Let us be the light in your darkness!",
			"You know, I hear some of these old lamps might be... magic.",
			"Be a shame if you missed anything because you didn't see it!",
			"Enchant your armor here! We offer premium enchant armor services!",
			"Hey %s, want to rustproof your armor? Our service will never fail you!",
			"Need to poison your weapon, %s? I have what you need!",
			"Dawg, I got quality shit for you here.",
			"Out of enchant weapon scrolls? Come over to my shop and you can enchant your weapon for just a few zorkmids!",
			"Tired of your weapon rusting and corroding all the damn time? Our shop allows you to proof them for a small fee!",
			"Got surplus weapons and need to know their enchantment? No problem, our appraisal service's really cheap and guaranteed to help you!",
			"Purchase your identify services here! We'll allow you to know what your items are!",
			"Premium wand recharging, %s! You don't want to miss our service!",
			"Enchant your rings in our shop! Only a few hundred zorkmids per charge!",
			"Tools empty? No problem - we recharge them if you got the money!",
			"You know you need a new pet! Come now and pick from our fine selection of brand-new figurines!",
			"Come to our gun shop and purchase rifles and ammo for all your hunting needs!",
		};
		verbalize(shop_sporkmsg[rn2(SIZE(shop_sporkmsg))],urace.noun);
	    }
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
	    static const char * const ora_msg[] = {
		    "a strange wind.",		/* Jupiter at Dodona */
		    "convulsive ravings.",	/* Apollo at Delphi */
		    "snoring snakes.",		/* AEsculapius at Epidaurus */
		    "someone say \"No more woodchucks!\"",
		    "a loud ZOT!"		/* both rec.humor.oracle */
	    };
	    /* KMH -- Give funny messages on Groundhog Day */
	    if (flags.groundhogday) hallu = 1;
	    You_hear("%s", ora_msg[rn2(3+hallu*2)]);
	}
	return;
    }
    if (!Is_blackmarket(&u.uz) && at_dgn_entrance("One-eyed Sam's Market") &&
        !rn2(200)) {
      static const char *blkmar_msg[] = {
        "You hear someone complaining about the prices.",
        "Somebody whispers: \"Food rations? Only 900 zorkmids.\"",
	  "Somebody whispers: \"For one million zorkmids you can buy the legendary one-of-a-kind Excalibur!\"",
        "You feel like searching for more gold.",
        "Somebody whispers: \"Dawg, I got quality shit around here.\"",
	  "Somebody whispers: \"I have that old playboy magazine issue that has naked pics of several dozen celebrities. I'll sell it to you for 10000 zorkmids.\"",
      };
      pline("%s", blkmar_msg[rn2(3+hallu*3)]);
    }

	/* finding alignment quests is way too hard, so the player will get some help. --Amy */
    if ( (at_dgn_entrance("Lawful Quest") || at_dgn_entrance("Neutral Quest") || at_dgn_entrance("Chaotic Quest") ) &&
        !rn2(1000)) {
      static const char *alignquest_msg[] = {
        "You feel the presence of some sort of force.",
        "There seems to be something special about this place.",
	  "You detect the presence of an alignment quest portal!",
	  "You hear an evil figure taunting you to step through the magic portal!",
        "You hear someone say: \"Force field activated!\"",
        "You hear someone say: \"Warp panel test sequence is starting in three... two... one...\"",
	  "You see Diablo 2, the boss from the game with the same name, and he's holding your mother hostage!",
	  "You know that on the flipside they have a cat AND a dog!",
      };
      pline("%s", alignquest_msg[rn2(4+hallu*4)]);
    }

	/* finding the subquest is even more annoying because you have to do it */
    if ( at_dgn_entrance("The Subquest") && u.silverbellget && !rn2(500) ) {
      static const char *subquest_msg[] = {
	  "You sense some questing power!",
	  "You detect the presence of the subquest portal!",
	  "A lot of force seems to be in the air.",
	  "You listen to the taunts of your nemesis who wants a rematch!",
	  "You suddenly fear that you'll face your nemesis again and lose!",
	  "You have a vision of your home being burned to the ground by the person you despise the most!",
	  "Something tells you that the only thing you can depend on is your family, and even that not always!",
	  "You hear a voice announce: \"Morn', player. I'm Sean Gonorrhea. I like my coffee black, my women slutty, and my mucosa membranes inflamed.\"",
      };
      pline("%s", subquest_msg[rn2(4+hallu*4)]);

    }

}

#endif /* OVL0 */
#ifdef OVLB

static const char * const h_sounds[] = {
    "beeps", "boings", "sings", "belches", "creaks", "coughs", "rattles",
    "ululates", "pops", "jingles", "sniffles", "tinkles", "eeps",
	"clatters", "hums", "sizzles", "twitters", "wheezes", "rustles",
	"honks", "lisps", "yodels", "coos", "burps", "moos", "booms",
	"murmurs", "oinks", "quacks", "rumbles", "twangs", "bellows",
	"toots", "gargles", "hoots", "warbles", "crackles", "hisses",
	"growls", "roars", "buzzes", "squeals", "screeches", "neighs",
	"wails", "commotions", "squaarks", "screams", "yowls", "yelps",
	"snarls", "squeals", "screaks", "whimpers", "whines", "howls",
	"yips", "barks", "purrs", "meows", "mews", "drones", "whinnies",
	"whickers", "gurgles", "burbles", "shrieks", "baaaas", "clucks",

};

/* make the sounds of a pet in any level of distress */
/* (1 = "whimper", 2 = "yelp", 3 = "growl") */
void
pet_distress(mtmp, lev)
register struct monst *mtmp;
int lev;
{
    const char *verb;
    if (mtmp->msleeping || !mtmp->mcanmove || !monstersoundtype(mtmp))
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
	pline("%s %s%c", Monnam(mtmp), verb, lev>1?'!':'.');
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

	switch (monstersoundtype(mtmp)) {
	case MS_MEW:
	case MS_HISS:
	    ret = "hisses";
	    break;
	case MS_BARK:
	case MS_GROWL:
	case MS_GRAKA:
	case MS_TEMPER:
	case MS_TRIP:
	    ret = "growls";
	    break;
	case MS_ROAR:
	    ret = "roars";
	    break;
	case MS_BUZZ:
	    ret = "buzzes";
	    break;
	case MS_SQEEK:
	    ret = "squeals";
	    break;
	case MS_SQAWK:
	    ret = "screeches";
	    break;
	case MS_NEIGH:
	    ret = "neighs";
	    break;
	case MS_WAIL:
	    ret = "wails";
	    break;
	case MS_SILENT:
		ret = "commotions";
		break;
	case MS_PARROT:
	    ret = "squaarks";
	    break;
	case MS_GURGLE:
		ret = "gurgles threateningly";
		break;
	case MS_SHRIEK:
		ret = "shrieks aggravatingly";
		aggravate();
		break;
	case MS_BONES:
		ret = "rattles frighteningly";
		You("freeze for a moment.");
		nomul(-2, "scared by rattling", TRUE);
		nomovemsg = 0;
		break;
	case MS_LAUGH:
		ret = "laughs fiendishly";
		break;
	case MS_MUMBLE:
		ret = "mumbles loudly";
		break;
	case MS_CASINO:
		ret = "calls for the casino's security";
		break;
	case MS_GLYPHS:
		ret = "screams 'Wait until I activate the awesome power of the yellow glyph!'";
		break;
	case MS_GIBBERISH:
		pline("%s", generate_garbage_string());
		break;
	case MS_SOCKS:
		ret = "looks very sad";
		break;
	case MS_PANTS:
		ret = "is heavily damaged";
		break;
	case MS_HANDY:
		ret = "screams 'How do you dare interrupting me during an important call!'";
		break;
	case MS_CAR:
		ret = "shouts 'You dorf, get away get away!'";
		break;
	case MS_JAPANESE:
		ret = "announces 'Sore wa seisen-shi no yarikata ni hanshimasu.'";
		break;
	case MS_SOVIET:
		ret = "shouts 'Ty pochuvstvuyesh' kholodnyy potseluy smerti, debil!'";
		break;
	case MS_SNORE:
		ret = "is jolted into consciousness";
		break;
	case MS_PHOTO:
		ret = "shouts 'Attacking a journalist is a severe crime!'";
		break;
	case MS_REPAIR:
		ret = "shouts 'Hey! I want to repair your gear, not be attacked by you!'";
		break;
	case MS_DRUGS:
		ret = "shouts 'Fuck you, asshole! I'm gonna sell my stuff somewhere else!'";
		break;
	case MS_COMBAT:
		ret = "shouts 'You can't fight fair, eh?'";
		break;
	case MS_MUTE:
		ret = "emits a long, angry grunt";
		break;
	case MS_CORONA:
		ret = "screams 'Hopefully you contract the virus and die!'";
		break;
	case MS_TRUMPET:
		ret = "emits a long, anguished trumpet sound";
		aggravate();
		break;
	case MS_PAIN:
		ret = "screams 'Ow-wow-wow! Owwwww!'";
		break;
	case MS_BRAG:
		ret = "shouts 'One day I'll kill you, and then you'll end up crying!'";
		break;
	case MS_PRINCESSLEIA:
		ret = "sobs 'That's not a way to treat a princess...'";
		break;
	case MS_LIEDER:
		ret = "shouts 'You will be excommunicated!'";
		break;
	case MS_GAARDIEN:
		ret = "shouts 'Friendly fire! FRIENDLY FIRE!'";
		break;
	case MS_MISSING:
		ret = "does not make a sound when seriously abused";
		break;
	case MS_SELFHARM:
		ret = "is bleeding terribly...";
		increasesanity(rnz(100));
		break;
	case MS_CLOCK:
		ret = "complains about the clock being broken";
		break;
	case MS_OHGOD:
		ret = "screams 'Ohtenthousandfoldgod!'";
		break;
	case MS_WOLLOH:
		ret = "shouts 'Wolloh asshole, I'm gonna beat up your ugly face wolloh!'";
		break;
	case MS_CODE:
		ret = "seems unable to remember the code";
		break;
	case MS_BARBER:
		ret = "screams 'Argh! I'm a barber, not a haircutter!'";
		break;
	case MS_AREOLA:
		ret = "is about to go crazy";
		break;
	case MS_STABILIZE:
		ret = "frantically tries to cast an emergency spell";
		break;
	case MS_ESCAPE:
		ret = "is about to escape";
		break;
	case MS_FEARHARE:
		ret = "is seriously afraid!";
		break;
	case MS_OE:
		ret = "seems fearful!";
		break;
	case MS_BAN:
		ret = "shouts 'I'll disappear if this continues!'";
		break;
	case MS_TALONCOMPANY:
		ret = "shouts 'Damn, I'm hit!'";
		break;
	case MS_REGULATOR:
		ret = "shouts 'Damn, I'm hit!'";
		break;
	case MS_RAIDER:
		ret = "shouts 'Stand still for once!'";
		break;
	case MS_ENCLAVE:
		ret = "shouts 'Ten-four! Target in sight!'";
		break;
	case MS_MUTANT:
		ret = "shouts 'I'm gonna eat your arms when you're dead, little human!'";
		break;
	case MS_MYTHICALDAWN:
		ret = "mutters 'The paradise awaits me...'";
		break;
	case MS_DREMORA:
		ret = "shouts 'Wince, failure!'";
		break;
	case MS_BOS:
		ret = "shouts 'Are you really trying to mess with the Brotherhood?'";
		break;
	case MS_OUTCAST:
		ret = "shouts 'Been a long time since I last killed someone!'";
		break;
	case MS_AMNESIA:
		ret = "mutters 'Huh, I think someone just attacked me.'";
		break;
	case MS_SHIVERINGESLES:
		ret = "announces 'Widerliche Tergens sind das. Heffin sie alle.'";
		break;
	case MS_ANOREXIA:
		ret = "mumbles 'Ah, who cares, I'm gonna die anyway.'";
		break;
	case MS_BULIMIA:
		ret = "shouts 'I will kill you! I swear, if you don't leave NOW I'll really kill you!'";
		break;
	case MS_BLANKER:
		ret = "shouts 'Now I'm gonna disenchant all your armor.'";
		break;
	case MS_CONDESCEND:
		ret = "laughs 'You're no match for me.'";
		break;
	case MS_MOLEST:
		if (flags.female) ret = "laughs 'Now I'll rape you, little girl.'";
		else ret = "sneers 'Fucking man, begone!'";
		break;
	case MS_SHOCKTROOPER:
		ret = "says 'ku kur ku'";
		break;
	case MS_GAGA:
		ret = "screams 'You violated my gender!!!!!!!111'";
		break;
	case MS_COMMON:
		ret = "screams 'I was seriously abused!'";
		break;
	case MS_PIRATE:
		ret = "shouts 'Gonna cleave ye to the brisket!'";
		break;
	case MS_SILLY:
		ret = "shouts 'Your ass will come out of the back!'";
		break;
	case MS_MIDI:
		ret = "makes a frenzied clamping sound";
		break;
	case MS_HERCULES:
		ret = "laughs 'You're no match for me, wimp.'";
		break;
	case MS_BEG:
		ret = "screams 'You're cruel.'";
		break;
	case MS_HIRE:
		ret = "shouts 'I'm gonna switch sides!'";
		break;
	case MS_ARMORER:
		ret = "shouts 'Your soul shall be food for hellfire!'";
		break;
	case MS_OMEN:
		ret = "shouts 'That's seven years of bad luck for you!'";
		break;
	case MS_NEWS:
		ret = "shouts 'Police! Someone assaulted me!!'";
		break;
	case MS_CRYTHROAT:
		ret = "shouts 'UAEAEAEAEAE I WAS STRUCK! UAEAEAEAEAE!'";
		break;
	case MS_HOOT:
		ret = "hoots angrily!";
		break;
	case MS_RUSTLE:
		ret = "slashes its roots angrily!";
		break;
	case MS_SEMEN:
		ret = "shouts 'You'll have hell to pay for that insolence.'";
		break;
	case MS_SCIENTIST:
		ret = "shouts 'Diiiiiiiiie, you little biiiiiiiiii-'";
		break;
	case MS_INCISION:
		ret = "shouts 'Now it's gonna get extra painful for you. I'll circumcise you even more slowly just to make you suffer.'";
		break;
	case MS_WEATHER:
		ret = "screams 'Lightning and thunderhall!'";
		break;
	case MS_MODALSHOP:
		ret = "shouts '!Te enterraran cuando termine contigo!'";
		break;
	case MS_SPOILER:
		ret = "Error 25. Please restart the game";
		break;
	case MS_DEAD:
		ret = "moans 'Please just kill me now.'";
		break;
	case MS_TREESQUAD:
		ret = "screams 'Alert! Serious environmental damage!'";
		break;
	case MS_METALMAFIA:
		ret = "groans 'I will get your metal someday...'";
		break;
	case MS_DEEPSTATE:
		ret = "doesn't show a visible reaction";
		break;
	case MS_SISSY:
		verbalize("owwww %s has me pain-done!", playeraliasname);
		break;
	case MS_SING:
		ret = "shouts 'Stop that! You're not allowed to use those girl boots against me!'";
		break;
	case MS_POMPEJI:
		ret = "moans 'Pompejiiiiiiii...'";
		break;
	case MS_FLUIDATOR:
		ret = "shouts 'Now leave to me finite in peace, you hell creature!'";
		break;
	case MS_BULLETATOR:
		ret = "shouts 'If you continue like that, I'll kill you.'";
		break;
	case MS_ALLA:
		ret = "shouts 'If you keep angering me like that, you'll pay.'";
		break;
	case MS_POKEDEX:
		ret = "shouts 'Hey! You're damaging my pokedex's CPU!'";
		break;
	case MS_BOT:
		ret = "drones 'Hostile acts detected.'";
		break;
	case MS_APOC:
		ret = "screams 'The apocalypse is truly coming now, everyone's going insane!'";
		break;
	case MS_ARREST:
		ret = "shouts 'I am an officer of the law!'";
		break;
	case MS_SOLDIER:
		ret = "shouts 'I'm gonna report you to your superior!";
		break;
	case MS_VAMPIRE:
		ret = "shouts 'Don't make me bite your the neck!'";
		break;
	case MS_CUSS:
		ret = "screams 'Fucking cunt!'";
		break;
	case MS_WHORE:
		ret = "shouts 'My pimp is the only one who may lay a hand on me!'";
		break;
	case MS_SUPERMAN:
		ret = "shouts 'YOU SHALL BE EXTERMINATED!'";
		aggravate();
		break;
	case MS_FART_QUIET:
	case MS_FART_NORMAL:
	case MS_FART_LOUD:
		ret = "screams 'Stop slapping my butt! That's painful!'";
		break;
	case MS_BOSS:
		ret = "shouts 'Hey! *I* am the boss here, not you!'";
		break;
	case MS_SHOE:
		ret = "screams 'Don't treat us like that! We don't like that at all!'";
		break;
	case MS_STENCH:
		ret = "screams 'You ruined my makeup!'";
		break;
	case MS_CONVERT:
		ret = "shouts 'Siz Allah'in sadik bir takipcisi degilsiniz, kafir!'";
		break;
	case MS_HCALIEN:
		ret = "looks at you with a gaze that is capable of killing";
		if (!rn2(20)) losehp(d(10,8), "the HC alien's gaze", KILLED_BY);
		else if (!rn2(5)) losehp(d(6,8), "the HC alien's gaze", KILLED_BY);
		else losehp(d(4,6), "the HC alien's gaze", KILLED_BY);
		break;
	default:
		ret = "screams";
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

    switch(monstersoundtype(mtmp)) {
	case MS_MEW:
	ret = "yowls";
	    break;
	case MS_BARK:
	case MS_GROWL:
	case MS_GRAKA:
	case MS_TEMPER:
	case MS_TRIP:
	ret = "yelps";
	    break;
	case MS_ROAR:
	ret = "snarls";
	    break;
	case MS_SQEEK:
	ret = "squeals";
	    break;
	case MS_SQAWK:
	ret = "screaks";
	    break;
	case MS_WAIL:
	ret = "wails";
	    break;
	case MS_GURGLE:
		ret = "gurgles hissingly";
		break;
	case MS_SHRIEK:
		ret = "shrieks in pain";
		aggravate();
		break;
	case MS_BONES:
		ret = "rattles";
		You("freeze for a moment.");
		nomul(-2, "scared by rattling", TRUE);
		nomovemsg = 0;
		break;
	case MS_LAUGH:
		ret = "stops laughing";
		break;
	case MS_MUMBLE:
		ret = "mumbles a bit";
		break;
	case MS_CASINO:
		ret = "complains about a loss of money";
		break;
	case MS_GIBBERISH:
		pline("%s", generate_garbage_string());
		break;
	case MS_SOCKS:
		ret = "frowns";
		break;
	case MS_PANTS:
		ret = "doesn't look so good";
		break;
	case MS_HANDY:
		ret = "has to interrupt a call";
		break;
	case MS_CAR:
		ret = "flashes a warning light";
		break;
	case MS_JAPANESE:
		ret = "mumbles 'Watashi wa futatabi kin'yoku-tekidenakereba narimasen...'";
		break;
	case MS_SOVIET:
		ret = "shouts 'Eto podzemel'ye rabotayet ne tak, kak zadumano!'";
		break;
	case MS_SNORE:
		ret = "seems alert for a bit, then dozes off again";
		break;
	case MS_PHOTO:
		ret = "checks the camera again";
		break;
	case MS_REPAIR:
		ret = "checks the repair hammer again";
		break;
	case MS_DRUGS:
		ret = "grumbles 'Stupid cops, can't they just go home...'";
		break;
	case MS_COMBAT:
		ret = "assumes a fighting pose";
		break;
	case MS_MUTE:
		ret = "growls annoyedly";
		break;
	case MS_CORONA:
		ret = "screams 'Police! Someone got closer to me than 1,50 meters!'";
		break;
	case MS_TRUMPET:
		ret = "emits a muffled trumpet sound";
		break;
	case MS_PAIN:
		ret = "seems to be in pain";
		break;
	case MS_BRAG:
		ret = "leafs through the monster stat sheet";
		break;
	case MS_PRINCESSLEIA:
		ret = "cries 'It's unfair that I always get hurt! A princess is supposed to be protected from harm at all times!'";
		break;
	case MS_LIEDER:
		ret = "shouts 'That's a violation of your conduct!'";
		break;
	case MS_GAARDIEN:
		ret = "laughs 'Keep doing that and I'll tell your boss, then you'll be fired.'";
		break;
	case MS_MISSING:
		ret = "does not make a sound when injured";
		break;
	case MS_SELFHARM:
		ret = "is squirting blood...";
		increasesanity(rnz(10));
		break;
	case MS_CLOCK:
		ret = "has to check the clock again";
		break;
	case MS_OHGOD:
		ret = "shouts 'Ohdoublegod!'";
		break;
	case MS_WOLLOH:
		ret = "screams 'Wolloh man, I'll fuck you up if you do that again, wolloh!";
		break;
	case MS_CODE:
		ret = "is trying hard to keep the code secret";
		break;
	case MS_BARBER:
		ret = "shouts 'No one stops the tunnel queues!'";
		break;
	case MS_AREOLA:
		ret = "stares at you in shock";
		break;
	case MS_STABILIZE:
		ret = "is attempting to erect a force field";
		break;
	case MS_ESCAPE:
		ret = "ponders the meaning of the dungeon";
		break;
	case MS_FEARHARE:
		ret = "screams 'Ah! I'm endangered! Screw that, I'm outta here!'";
		break;
	case MS_OE:
		ret = "feels in danger";
		break;
	case MS_BAN:
		ret = "shouts 'Emergency! Gotta get away from here!'";
		break;
	case MS_TALONCOMPANY:
		ret = "laughs 'I like it if they fight back!'";
		break;
	case MS_REGULATOR:
		ret = "shouts 'You can't escape the law!'";
		break;
	case MS_RAIDER:
		ret = "shouts 'Fresh blood, just waiting to be spilled!'";
		break;
	case MS_ENCLAVE:
		ret = "shouts 'Security breach!'";
		break;
	case MS_MUTANT:
		ret = "mutters 'Just get close... I'll tear off your legs...'";
		break;
	case MS_MYTHICALDAWN:
		ret = "announces 'I don't fear death.'";
		break;
	case MS_DREMORA:
		ret = "shouts 'You'll be my next trophy, scapegrace!'";
		break;
	case MS_BOS:
		ret = "shouts 'Evil-doer, I shall purge you from this world!'";
		break;
	case MS_OUTCAST:
		ret = "shouts 'What the hell do you think you're doing?!'";
		break;
	case MS_AMNESIA:
		ret = "grumbles 'Hmm, I think I've just been wounded!'";
		break;
	case MS_SHIVERINGESLES:
		ret = "grumbles 'Ich werde den Finduh kalikraken. Ehrlich. Darauf koennt Ihr terraten!'";
		break;
	case MS_ANOREXIA:
		ret = "mutters 'This is all worthless.'";
		break;
	case MS_BULIMIA:
		ret = "growls 'Do that one more time and I'll slit your throat!'";
		break;
	case MS_BLANKER:
		ret = "growls 'Leave me alone or I'll cast destroy armor at you.'";
		break;
	case MS_CONDESCEND:
		ret = "snickers 'Does that little adventurer really think they can match my skill?'";
		break;
	case MS_MOLEST:
		if (flags.female) ret = "snickers 'You really want me to play with the orifice between your legs, huh, girl?'";
		else ret = "shouts 'Asshole dude! Leave me the fuck alone!'";
		break;
	case MS_SHOCKTROOPER:
		ret = "says 'dup dup'";
		break;
	case MS_GAGA:
		ret = "shouts 'Did you just assume my gender?'";
		break;
	case MS_COMMON:
		ret = "shouts 'I have been mistreated!'";
		break;
	case MS_PIRATE:
		ret = "shouts 'Hang'em from the yardarm!'";
		break;
	case MS_SILLY:
		ret = "shouts 'Penis masturbation! Titty fuck!'";
		break;
	case MS_MIDI:
		ret = "drums rapidly";
		break;
	case MS_HERCULES:
		ret = "sneers 'If you do that again I'll force you to clean the stables.'";
		break;
	case MS_BEG:
		ret = "sighs 'Beggars always cry...'";
		break;
	case MS_HIRE:
		ret = "moans 'Why do I always have to put up with abusive bosses...'";
		break;
	case MS_ARMORER:
		ret = "shouts 'Me smashem!'";
		break;
	case MS_OMEN:
		ret = "shouts 'Attacking me causes you to be struck by misfortune!'";
		break;
	case MS_NEWS:
		ret = "shouts 'This will be in tomorrow's newspaper!'";
		break;
	case MS_CRYTHROAT:
		ret = "shouts 'WAAAAAAH! THEY WERE MEAN TO ME!'";
		break;
	case MS_HOOT:
		ret = "hoots alarmingly!";
		break;
	case MS_RUSTLE:
		ret = "emits an unholy-sounding drone!";
		break;
	case MS_SEMEN:
		ret = "shouts 'I'm gonna have my revenge!'";
		break;
	case MS_SCIENTIST:
		ret = "shouts 'Stop attacking, immediately!'";
		break;
	case MS_INCISION:
		ret = "shouts 'You'll be bleeding!'";
		break;
	case MS_WEATHER:
		ret = "shouts 'There's a storm brewing...'";
		break;
	case MS_MODALSHOP:
		ret = "shouts '!Esa no es manera de tratar a una tendera!'";
		break;
	case MS_SPOILER:
		ret = "Buffer overrun detected. NETHACK cannot safely continue execution and needs to close.";
		break;
	case MS_DEAD:
		ret = "mutters 'Oh, if only I could have stayed dead...'";
		break;
	case MS_TREESQUAD:
		ret = "screams 'All activists, protect the trees with your lives!'";
		break;
	case MS_METALMAFIA:
		ret = "shouts 'If you don't give me your metal I'll shoot you with my gun and take it by force!'";
		break;
	case MS_DEEPSTATE:
		ret = "has second thoughts about you";
		break;
	case MS_SISSY:
		ret = "cries 'noooooo i broke a nail!'";
		break;
	case MS_SING:
		ret = "is sad that no one wants to clean the female shoes";
		break;
	case MS_POMPEJI:
		ret = "shouts 'Pompeji!!!'";
		break;
	case MS_FLUIDATOR:
		ret = "bellows 'You are always cursed!'";
		break;
	case MS_BULLETATOR:
		ret = "shouts 'You no-good bastard!'";
		break;
	case MS_ALLA:
		ret = "shouts 'Better stop mistreating me, for you'll not like my counterattack.'";
		break;
	case MS_POKEDEX:
		ret = "looks worriedly at the PokeGear(TM) display";
		break;
	case MS_BOT:
		ret = "drones 'Threat level omega.'";
		break;
	case MS_APOC:
		ret = "mutters 'Everything is falling apart at the seams...'";
		break;
	case MS_GLYPHS:
		ret = "moans 'Why doesn't the glyph work...'";
		break;
	case MS_ARREST:
		ret = "blows a whistle";
		aggravate();
		break;
	case MS_SOLDIER:
		ret = "shouts 'Medic!'";
		break;
	case MS_VAMPIRE:
		ret = "groans";
		break;
	case MS_CUSS:
		ret = "grumbles 'If you do that again I'm gonna open a can of whoopass on you...'";
		break;
	case MS_WHORE:
		ret = "exclaims 'Hey! I ain't a slut!'";
		break;
	case MS_SUPERMAN:
		ret = "shouts 'NO! FUCK YOU!!!'";
		aggravate();
		break;
	case MS_FART_QUIET:
	case MS_FART_NORMAL:
	case MS_FART_LOUD:
		ret = "sobs 'This is humiliating...'";
		break;
	case MS_BOSS:
		ret = "shouts 'That's twice you hit me now! One more time and I'll kill you!'";
		break;
	case MS_SHOE:
		ret = "threatens 'If you keep doing that, we'll scratch your legs full length!'";
		break;
	case MS_STENCH:
		ret = "shouts 'That's no way to treat a woman!'";
		break;
	case MS_CONVERT:
		ret = "shouts 'Peygamber bunu hatirlayacak!'";
		break;
	case MS_HCALIEN:
		ret = "strikes fear into your heart with a gaze";
		make_feared(HFeared + rnd(10 + (monster_difficulty()) ),TRUE);
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

    switch (monstersoundtype(mtmp)) {
	case MS_MEW:
	case MS_GROWL:
	case MS_GRAKA:
	case MS_TEMPER:
	case MS_TRIP:
	ret = "whimpers";
	    break;
	case MS_BARK:
	ret = "whines";
	    break;
	case MS_SQEEK:
	ret = "squeals";
	    break;
	case MS_SHRIEK:
		ret = "shrieks fearfully";
		aggravate();
		break;
	case MS_BONES:
		ret = "claps";
		break;
	case MS_LAUGH:
		ret = "laughs quietly";
		break;
	case MS_MUMBLE:
		ret = "mumbles in anticipation of danger";
		break;
	case MS_CASINO:
		ret = "warns you that the mafia is coming";
		break;
	case MS_GIBBERISH:
		pline("%s", generate_garbage_string());
		break;
	case MS_SOCKS:
		ret = "waggles warningly";
		break;
	case MS_PANTS:
		ret = "swerves back and forth";
		break;
	case MS_HANDY:
		ret = "briefly interrupts the call and shouts 'CAREFUL!'";
		break;
	case MS_CAR:
		ret = "alarm installation goes off!";
		aggravate();
		break;
	case MS_JAPANESE:
		ret = "instructs 'Sono pureto kara hanarete kudasai, sore wa jaakuna doragon ni yotte settei sa reta wanadesu.'";
		break;
	case MS_SOVIET:
		ret = "laughs 'Vy popadete v lovushku i, nakonets, zakonchite igru, i vy, kak sleduyet, igrayete v takuyu ??der'movuyu igru.'";
		break;
	case MS_SNORE:
		ret = "opens the eyes, looks worriedly at you, and dozes off again";
		break;
	case MS_PHOTO:
		ret = "takes a photo of the nearby dungeon floor for some reason";
		break;
	case MS_REPAIR:
		ret = "makes a motion towards the floor";
		break;
	case MS_DRUGS:
		ret = "screams 'Damn! It's a trap set by the cops!'";
		break;
	case MS_COMBAT:
		ret = "shouts 'Search! There's a trap nearby! Use the search command!'";
		break;
	case MS_MUTE:
		ret = "exhales thrice in a row, while gesturing at the floor";
		break;
	case MS_CORONA:
		ret = "shouts 'Let's go somewhere else, the corona wind is bad around here!'";
		break;
	case MS_TRUMPET:
		ret = "trumpets scaredly";
		break;
	case MS_PAIN:
		ret = "shouts 'Ack! It's a trap!'";
		break;
	case MS_BRAG:
		ret = "laughs 'Bet I'll find that trap before you do?'";
		break;
	case MS_PRINCESSLEIA:
		ret = "whispers 'I sense a trap. Please be careful, don't step into it.'";
		break;
	case MS_LIEDER:
		ret = "says 'The nemesis put a trap over there! Don't trigger it!'";
		break;
	case MS_GAARDIEN:
		ret = "tells you 'I remember that there's a trap here.'";
		break;
	case MS_MISSING:
		ret = "does not make a sound when distressed";
		break;
	case MS_SELFHARM:
		ret = "suddenly seems very eager to take one more step!";
		break;
	case MS_CLOCK:
		ret = "notices that the clock's alarm has gone off!";
		break;
	case MS_OHGOD:
		ret = "shouts 'Oh god, a trap!'";
		break;
	case MS_WOLLOH:
		ret = "shouts 'Wolloh, watch out, wolloh!'";
		break;
	case MS_CODE:
		ret = "is trying hard to avoid losing the code";
		break;
	case MS_BARBER:
		ret = "says 'Watch out, the overseer has laid a trap!'";
		break;
	case MS_AREOLA:
		ret = "is staring at you in expectation";
		break;
	case MS_STABILIZE:
		ret = "opens the abilities menu and searches for the one that disarms traps";
		break;
	case MS_ESCAPE:
		ret = "wonders whether there's an escape path";
		break;
	case MS_FEARHARE:
		ret = "screams 'Get me away from here, I don't have a good feeling about this place!'";
		break;
	case MS_OE:
		ret = "feels like there's a beartrap or landmine nearby!";
		break;
	case MS_BAN:
		ret = "says 'Are you sure we should stay here?'";
		break;
	case MS_TALONCOMPANY:
		ret = "shouts 'Take cover! Ass grenade!'";
		break;
	case MS_REGULATOR:
		ret = "shouts 'Grenade!'";
		break;
	case MS_RAIDER:
		ret = "screams 'Oh, green nines!'";
		break;
	case MS_ENCLAVE:
		ret = "announces 'Explosion imminent!'";
		break;
	case MS_MUTANT:
		ret = "announces 'Hot potato!'";
		break;
	case MS_MYTHICALDAWN:
		ret = "announces 'Only the chosen ones will survive purgatory.'";
		break;
	case MS_DREMORA:
		ret = "announces 'Be quick, mortal.'";
		break;
	case MS_BOS:
		ret = "announces 'Be careful, there may be traps.'";
		break;
	case MS_OUTCAST:
		ret = "shouts 'Watch out, they placed a bomb!'";
		break;
	case MS_AMNESIA:
		ret = "doesn't seem to remember whether there were traps here.";
		break;
	case MS_SHIVERINGESLES:
		ret = "says 'Gorbeln. Koennt Ihr sie nicht gorbeln hoeren?'";
		break;
	case MS_ANOREXIA:
		ret = "says 'Stay away from the food.'";
		break;
	case MS_BULIMIA:
		ret = "says 'Don't step on that plate!'";
		break;
	case MS_BLANKER:
		ret = "shouts 'There, an anti-magic trap!'";
		break;
	case MS_CONDESCEND:
		ret = "whispers 'Watch out, that one floor tile is a different shade of color from the rest.'";
		break;
	case MS_MOLEST:
		if (flags.female) ret = "laughs 'Please, girl, step into the insemination trap for me!'";
		else ret = "shouts 'Watch out man, a trap!'";
		break;
	case MS_SHOCKTROOPER:
		ret = "says 'dup blis dup'";
		break;
	case MS_GAGA:
		ret = "mutters 'Some bigot must have placed a trap there!'";
		break;
	case MS_COMMON:
		ret = "says 'I sense a trap nearby.'";
		break;
	case MS_PIRATE:
		ret = "says 'Avast, they runnin' a rig on ye!'";
		break;
	case MS_SILLY:
		ret = "grins 'I see an angular sign and a socketed angular sign.'";
		break;
	case MS_MIDI:
		ret = "pings with an annoyingly high pitch";
		break;
	case MS_HERCULES:
		ret = "remarks 'This must be another one of those divine tasks.'";
		break;
	case MS_BEG:
		ret = "shouts 'Someone wants to kill me!'";
		break;
	case MS_HIRE:
		ret = "shouts 'Watch out! A trap!'";
		break;
	case MS_ARMORER:
		ret = "shouts 'Hurrrrr!'";
		break;
	case MS_OMEN:
		ret = "whispers 'My sixth sense tells me that there's a trap!'";
		break;
	case MS_NEWS:
		ret = "flusters 'Some hate mob must have mined the area!'";
		break;
	case MS_CRYTHROAT:
		ret = "screams 'NOOOOOOOO I DON'T WANT TO GO THERE! NOOOOOOOO!'";
		break;
	case MS_HOOT:
		ret = "hoots loudly!";
		break;
	case MS_RUSTLE:
		ret = "rustles lightly and points a root in a certain direction.";
		break;
	case MS_SEMEN:
		ret = "snickers 'Why don't you step on that tile over there... he he he.'";
		break;
	case MS_SCIENTIST:
		ret = "mutters 'It can't get worse than this.'";
		break;
	case MS_INCISION:
		ret = "says 'That device over there looks useful...'";
		break;
	case MS_WEATHER:
		ret = "says 'The soil looks suspicious there.'";
		break;
	case MS_MODALSHOP:
		ret = "remarks 'Eso parece sospechoso.'";
		break;
	case MS_SPOILER:
		ret = "An error has occurred in your application. If you choose Close, NETHACK will close. If you choose Ignore, you should save your work in a new file.";
		break;
	case MS_DEAD:
		ret = "whimpers 'Why do you have to lead me into such dangerous areas?'";
		break;
	case MS_TREESQUAD:
		ret = "shouts 'Attention! Our enemies are up to something devious!'";
		break;
	case MS_METALMAFIA:
		ret = "contemplates 'Hmm... if I just pull that hunk of metal out of the ground...'";
		break;
	case MS_DEEPSTATE:
		ret = "is pointing a finger in a certain direction";
		break;
	case MS_SISSY:
		ret = "whimpers 'arrrrgh we will all die!'";
		break;
	case MS_SING:
		ret = "seems happy for some reason";
		break;
	case MS_POMPEJI:
		ret = "points a finger and shouts 'Pompeji!'";
		break;
	case MS_FLUIDATOR:
		ret = "remarks 'Pass up, falling!'";
		break;
	case MS_BULLETATOR:
		ret = "shouts 'Watch for the traps!'";
		break;
	case MS_ALLA:
		ret = "wordlessly gazes at you";
		break;
	case MS_POKEDEX:
		ret = "shouts 'A game bug! Watch out, don't step on that tile next to you or the game will glitch out and delete your last pokemon!'";
		break;
	case MS_BOT:
		ret = "drones 'Area is scanned for hostile threats!'";
		break;
	case MS_APOC:
		ret = "shouts 'The plunderers are trying to lead us into a trap!'";
		break;
	case MS_GLYPHS:
		ret = "suggests you to use a trap detection glyph";
		break;
	case MS_ARREST:
		ret = "calls for reinforcements";
		break;
	case MS_SOLDIER:
		ret = "whispers 'Careful! It's an ambush!'";
		break;
	case MS_CUSS:
		ret = "shouts 'Shit! Danger!'";
		break;
	case MS_WHORE:
		ret = "moans 'Those filthy customers keep trying to beat me up!'";
		break;
	case MS_FART_QUIET:
	case MS_FART_NORMAL:
	case MS_FART_LOUD:
		ret = "advises you to be careful";
		break;
	case MS_BOSS:
		ret = "whispers 'Careful, I know there's traps here. Let's try to avoid them.'";
		break;
	case MS_SHOE:
		ret = "shouts 'Stop! There's a heap of shit over there! Make sure we don't step into it!'";
		break;
	case MS_STENCH:
		ret = "asks you to be careful";
		break;
	case MS_CONVERT:
		ret = "whispers 'Kafirler goelgede saklaniyor, ama Allah onlan yok edecek.'";
		break;
	case MS_HCALIEN:
		ret = "makes a pointing gesture";
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
	    !(carnivorous(mtmp->data) || herbivorous(mtmp->data) || metallivorous(mtmp->data) || mtmp->egotype_lithivore || mtmp->egotype_metallivore || mtmp->egotype_allivore || lithivorous(mtmp->data)))
	return;

    /* presumably nearness and soundok checks have already been made */
    if (!is_silent(mtmp->data) && monstersoundtype(mtmp) <= MS_ANIMAL)
	(void) domonnoise(mtmp);
    else if (monstersoundtype(mtmp) >= MS_HUMANOID) {
	if (!canspotmon(mtmp) && !(monstersoundtype(mtmp) == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember))
	    map_invisible(mtmp->mx, mtmp->my);
	(void) domonnoise(mtmp); /* "I'm hungry" for MS_HUMANOID but different for others --Amy */
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

	/* monster should make hungry noises only if it actually has to eat --Amy */
	boolean hastoeat = TRUE;
	if (!carnivorous(mtmp->data) && !herbivorous(mtmp->data) && !metallivorous(mtmp->data) && !mtmp->egotype_lithivore && !mtmp->egotype_metallivore && !mtmp->egotype_allivore && !lithivorous(mtmp->data)) hastoeat = FALSE;

    if (mtmp->egotype_farter) {

		if (uleft && objects[(uleft)->otyp].oc_material == MT_INKA) {
			verbalize("Let me take off that ring for you.");
			remove_worn_item(uleft, TRUE);
		}
		if (uright && objects[(uright)->otyp].oc_material == MT_INKA) {
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
		else if (uarmf && uarmf->oartifact == ART_SPEAK_TO_OJ && !mtmp->mfrenzied) {
			pline("%s is charmed by your stroking units.", Monnam(mtmp) );
			mtmp->mpeaceful = 1;
		}
		else if (uarmg && uarmg->oartifact == ART_GROPESTROKER && !mtmp->mfrenzied && !rn2(20)) {
			if (rn2(6)) {
				pline("%s is charmed by your stroking units.", Monnam(mtmp) );
				mtmp->mpeaceful = 1;
			} else {
				pline("%s is really angry and calls you a sexual predator.", Monnam(mtmp) );
				mtmp->mfrenzied = 1;
			}
		}
		else {
			pline("%s seems to be even more angry at you than before.", Monnam(mtmp) );
		}
	    m_respond(mtmp);
    }

    /* presumably nearness and sleep checks have already been made */
    if (!flags.soundok) return(0);

	if (ptr == &mons[PM_FJORDE]) verbalize("I can't help it, I really consider fish to be the most beautiful creatures in existence."); /* must do that here because they're "silent" */

    if (is_silent(ptr)) return(0);

    /* Make sure its your role's quest quardian; adjust if not */
    if (ptr->msound == MS_GUARDIAN && ptr != &mons[urole.guardnum]) {
    	int mndx = monsndx(ptr);
    	ptr = &mons[genus(mndx,1)];
    }

    /* be sure to do this before talking; the monster might teleport away, in
     * which case we want to check its pre-teleport position
     */
    if (!canspotmon(mtmp) && !(monstersoundtype(mtmp) == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember))
	map_invisible(mtmp->mx, mtmp->my);

	if (ptr == &mons[PM_DONALD_TRUMP]) { /* idea by Crawldragon, actual messages by Amy */

		if (issoviet) verbalize("Of course Russia was not involved in the presidential election in any way. That's just fake news.");
		else pline("Make America GREAT again!");
	}

	if (ptr == &mons[PM_EMERA]) verbalize("HATE! HATE! HATE!");

    switch (monstersoundtype(mtmp)) {
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

		if (mtmp->data == &mons[PM_BAKER]) {
			verbalize("Want a square meal? (100 zorkmids)");
			if (yn("Accept the offer?") == 'y') {
				if (u.ugold < 100) {
					You("check your wallet and shake your head.");
				} else {
					u.ugold -= 100;
					lesshungry(100);
					pline("The taste is not bad.");
				}
			}
		}

		if (mtmp->data == &mons[PM_BERTIE_BOTT] || mtmp->data == &mons[PM_DIAGON_ALLEY_VENDOR]) {
			verbalize("Want some of my All-Flavor Beans? Just 2000 zorkmids!");
			if (yn("Accept the offer?") == 'y') {
				if (u.ugold < 2000) {
					You("check your wallet and shake your head.");
				} else {
					struct obj *udrink;
					u.ugold -= 2000;
					udrink = mkobj(POTION_CLASS, TRUE, FALSE);
					if (udrink) {
						dropy(udrink);
						stackobj(udrink);
						verbalize("Enjoy! I've placed it on the ground. It'll certainly taste like earthworm or vomit, hahaha!");
					} else {
						u.ugold += 2000;
						verbalize("Hmm, I must have misplaced it. Try again later. Here you have your money back.");
					}

				}
			}
		}

		if (mtmp->data == &mons[PM_ELONA_TRADER]) {
			verbalize("Do you want to trade 2000 zorkmids for a random item?");
			if (yn("Accept the offer?") == 'y') {
				if (u.ugold < 2000) {
					You("check your wallet and shake your head.");
				} else {
					struct obj *udrink;
					u.ugold -= 2000;
					udrink = mkobj(RANDOM_CLASS, TRUE, FALSE);
					if (udrink) {
						dropy(udrink);
						stackobj(udrink);
						verbalize("Alright, check the ground below you!");
					} else {
						u.ugold += 1800;
						verbalize("Huh, it seems that there's some thief around who stole the item just as you went to pick it up! I'll give you a refund, minus an administration fee of 200 zorkmids of course.");
					}
				}
			}
		}

		if (mtmp->data == &mons[PM_ELONA_INNKEEPER]) {
			verbalize("Hey, do you want a refreshing drink? Just 1000 zorkmids!");
			if (yn("Accept the offer?") == 'y') {
				if (u.ugold < 1000) {
					You("check your wallet and shake your head.");
				} else {
					struct obj *udrink;
					u.ugold -= 1000;
					udrink = mksobj(POT_BOOZE, TRUE, FALSE, FALSE);
					if (udrink) {
						udrink->known = udrink->dknown = 1;
						udrink->finalcancel = 1;
						dropy(udrink);
						stackobj(udrink);
						verbalize("There you go, I placed it at your feet!");
					} else {
						verbalize("Oh sorry, we're out of stock.");
						u.ugold += 1000;
					}

				}
			}
		}

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
			if (mtmp->mhp < mtmp->mhpmax/3) {
				verbl_msg = "I didn't get a drink in a long time...";
			} else if (kindred) {
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
		if (mtmp->mtame && (mtmp->mconf || mtmp->mflee || mtmp->mtrapped || (moves > EDOG(mtmp)->hungrytime && hastoeat) || mtmp->mtame < 5))
		    pline_msg = "whines.";
		else if (mtmp->mtame && hastoeat && (EDOG(mtmp)->hungrytime > (moves + 1000)))
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
		else if (moves > EDOG(mtmp)->hungrytime && hastoeat)
		    pline_msg = "meows.";
		else if ((EDOG(mtmp)->hungrytime > (moves + 1000)) && hastoeat)
		    pline_msg = "purrs.";
		else
		    pline_msg = "mews.";
		break;
	    } /* else FALLTHRU */
	case MS_GROWL:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "lets out a tormented snarl.";
			else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "growls hungrily.";
			else if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped)
				pline_msg = "whines.";
			else if (mtmp->mtame && hastoeat && (EDOG(mtmp)->hungrytime > (moves + 1000)))
				pline_msg = "yips.";
			else
				pline_msg = "snarls.";
		}
		else pline_msg = mtmp->mpeaceful ? "snarls." : "growls!";
		break;
	case MS_ROAR:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "lets out a tormented snarl.";
			else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "emits a faint roar.";
			else
				pline_msg = (ptr == &mons[PM_SYSTEM_FAILURE]) ? "says in a monotone voice: 'Pieteikumu kluda.'" : "snarls.";
		}
		else pline_msg = (ptr == &mons[PM_SYSTEM_FAILURE]) ? "says in a monotone voice: 'Pieteikumu kluda.'" : mtmp->mpeaceful ? "snarls." : "roars!";
		break;
	case MS_SQEEK:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "squeaks in agony.";
			else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "emits a high squeak.";
			else if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped)
				pline_msg = "emits a terrified squeak.";
			else if (mtmp->mtame && EDOG(mtmp)->hungrytime > moves + 1000)
				pline_msg = "squeaks joyfully.";
			else
				pline_msg = "squeaks.";
		}
		else pline_msg = "squeaks.";
		break;
	case MS_PARROT:

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			pline_msg = "says 'Help! Police!'";
		} else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			pline_msg = "says 'Pass me the damn food already!'";
		} else switch (rn2(8)) {
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
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "squawks in agony.";
			else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "makes a distressed squawk.";
			else if (ptr == &mons[PM_RAVEN] && !mtmp->mpeaceful)
				verbl_msg = "Nevermore!";
			else
				pline_msg = "squawks.";
		}
		else if (ptr == &mons[PM_RAVEN] && !mtmp->mpeaceful)
			verbl_msg = "Nevermore!";
		else
			pline_msg = "squawks.";
		break;
	case MS_HISS:
		if (!mtmp->mpeaceful)
			pline_msg = "hisses!";
		else if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "hisses angrily.";
			else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "tries to snap at your finger.";
			else if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped)
				pline_msg = "makes a confused hiss.";
			else if (mtmp->mtame && hastoeat && (EDOG(mtmp)->hungrytime > (moves + 1000)))
				pline_msg = "hums.";
			else
				pline_msg = "hisses peacefully.";
		}
		else return 0;	/* no sound */
		break;
	case MS_BUZZ:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/5)
				pline_msg = "displays a blue screen of death!";
			else if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "displays an error message: 'General protection fault'...";
			else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "is waiting for data input...";
			else if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped)
				pline_msg = "makes a confused droning sound.";
			else
				pline_msg = "drones.";
		}
		else pline_msg = mtmp->mpeaceful ? "drones." : "buzzes angrily.";
		break;
	case MS_GRUNT: /* YANI by ShivanHunter */
		if (mtmp->mtame) {
			if (mtmp->data->mlet == S_TROLL) {
				if (mtmp->mhp < mtmp->mhpmax/3)
					pline_msg = "shouts: 'Curse those right-wing extremists!'";
				else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
					pline_msg = "shouts: 'Does this stinking hole not have a restaurant??? '";
				else if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped)
					pline_msg = "shouts: 'rn2(P = NP) can't be false because we didn't see any rn2(0) impossibles, which proves that P = NP!'";
				else
					pline_msg = "shouts: 'VI > EMACS!!!!!'";

			} else {
				if (mtmp->mhp < mtmp->mhpmax/3)
					pline_msg = "seems to be wounded.";
				else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
					pline_msg = "is hungry.";
				else if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped)
					pline_msg = "grovels.";
				else
					pline_msg = "grunts.";
			}
		}
		else pline_msg = (mtmp->data->mlet == S_TROLL) ? "shouts: 'VI > EMACS!!!!!'" : "grunts.";
		break;
	case MS_NEIGH:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3)
			pline_msg = "whines.";
		else if (mtmp->mtame < 5)
			pline_msg = "neighs.";
		else if (hastoeat && moves > EDOG(mtmp)->hungrytime)
			pline_msg = "whinnies.";
		else
			pline_msg = "whickers.";
		break;
	case MS_WAIL:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "seems about to faint.";
			else
				pline_msg = "wails mournfully.";
		}
		else pline_msg = "wails mournfully.";
		break;
	case MS_GURGLE:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "gurgles loudly.";
			else
				pline_msg = "gurgles.";
		}
		else pline_msg = "gurgles.";
		break;
	case MS_BURBLE:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/4)
				pline_msg = "makes a frightened burble.";
			else
				pline_msg = "burbles.";
		}
		else pline_msg = "burbles.";
		break;
	case MS_SHRIEK:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "emits a faint shriek.";
			else
				pline_msg = "shrieks.";
				aggravate();
		}
		else {
			pline_msg = "shrieks.";
			aggravate();
		}
		break;
	case MS_FART_QUIET:
	case MS_FART_NORMAL:
	case MS_FART_LOUD:

		if (uleft && objects[(uleft)->otyp].oc_material == MT_INKA) {
			verbalize("Let me take off that ring for you.");
			remove_worn_item(uleft, TRUE);
		}
		if (uright && objects[(uright)->otyp].oc_material == MT_INKA) {
			verbalize("Let me take off that ring for you.");
			remove_worn_item(uright, TRUE);
		}

		if (uarmf && uarmf->oartifact == ART_ELENA_S_REWARD && quest_status.killed_nemesis && monstersoundtype(mtmp) == MS_FART_NORMAL && !mtmp->mpeaceful && !mtmp->mtame && !mtmp->mfrenzied) {
			mtmp->mpeaceful = TRUE;
			verbalize("You've won, so I'll be at your service...");
		}

		pline("You gently caress %s's %s butt using %s %s.", mon_nam(mtmp), mtmp->female ? "sexy" : "ugly", !rn2(3) ? "both your left and right" : rn2(2) ? "your left" : "your right", body_part(HAND) );

		if (uarmu && uarmu->oartifact == ART_SUE_LYN_S_SEX_GAME && !mtmp->mpeaceful && !mtmp->mfrenzied) {
			mtmp->mpeaceful = TRUE;
		}

		if (mtmp->mtame) {

			if (mtmp->mhp < mtmp->mhpmax/3) {
				verbl_msg = "Please don't let me die!";
				break;
			} else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime) {
				verbl_msg = "I haven't eaten in a while. Please bring me some chicken meat garnished with pepperoni.";
				break;
			} else {
				pline("%s seems to love you even more than before.", Monnam(mtmp) );
				if (mtmp->mtame < 30) mtmp->mtame++;
			}
		}
		else if (mtmp->mpeaceful) {
			pline("%s seems to like being felt up by you.", Monnam(mtmp) );
		}
		else if (uarmf && uarmf->oartifact == ART_SPEAK_TO_OJ && !mtmp->mfrenzied) {
			pline("%s is charmed by your stroking units.", Monnam(mtmp) );
			mtmp->mpeaceful = 1;
		}
		else if (uarmg && uarmg->oartifact == ART_GROPESTROKER && !mtmp->mfrenzied && !rn2(20)) {
			if (rn2(6)) {
				pline("%s is charmed by your stroking units.", Monnam(mtmp) );
				mtmp->mpeaceful = 1;
			} else {
				pline("%s is really angry and calls you a sexual predator.", Monnam(mtmp) );
				mtmp->mfrenzied = 1;
			}
		}
		else if (FemtrapActiveSueLyn && mtmp->female && !mtmp->mfrenzied && monstersoundtype(mtmp) == MS_FART_NORMAL) {
			if (!rn2(20)) {
				pline("%s is satisfied by your caressing touch.", Monnam(mtmp) );
				mtmp->mpeaceful = TRUE;
			} else {
				pline("%s demands that you continue stroking %s butt!", Monnam(mtmp), mhis(mtmp) );
			}
		}
		else {
			pline("%s seems to be even more angry at you than before.", Monnam(mtmp) );
		}
		m_respond(mtmp);

		if (uarmu && uarmu->oartifact == ART_SUE_LYN_S_SEX_GAME && mtmp->mpeaceful && !mtmp->mfrenzied && !mtmp->mtame && (mtmp->m_lev < 1 || (u.ugold >= (mtmp->m_lev * 100))) ) {
			pline("%s offers to join you for %d zorkmids.", Monnam(mtmp), (mtmp->m_lev < 1) ? 0 : (mtmp->m_lev * 100));
			if (yn("Accept the offer?") == 'y') {
				if (mtmp->m_lev > 0) u.ugold -= (mtmp->m_lev * 100);
				struct monst *suepet;
				suepet = tamedog(mtmp, (struct obj *) 0, TRUE);
				if (suepet) mtmp = suepet;
				if (mtmp) verbalize("You really caressed my butt cheeks so beautifully, I think I'm in love with you!");
				else impossible("suepet was tamed but doesn't exist now??");
			}

		}

	    break;
	case MS_IMITATE:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "unsuccessfully tries to imitate you.";
			else if (mtmp->mtame && hastoeat && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "imitates a starving person.";
			else
				pline_msg = "imitates you.";
		}
		else pline_msg = "imitates you.";
		break;
	case MS_SHEEP:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "makes a sound that reminds you of a moan.";
			else if (mtmp->mtame && hastoeat && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "baaaas hungrily.";
			else if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped)
				pline_msg = "is scared.";
			else
				pline_msg = "baaaas.";
		}
		else pline_msg = "baaaas.";
		break;
	case MS_CHICKEN:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3) {
				pline_msg = "makes an anguished shriek.";
				aggravate();
			} else if (mtmp->mtame && hastoeat && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "looks hungry.";
			else if (mtmp->mconf || mtmp->mflee)
				pline_msg = "runs wildly in fear.";
			else if (mtmp->mtrapped)
				pline_msg = "is mortally scared.";
			else
				pline_msg = "clucks.";
		}
		else pline_msg = "clucks.";
		break;
	case MS_GLYPHS:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3) {
				verbl_msg = "Hell if I knew how to activate the healing glyph...";
			} else if (mtmp->mtame && hastoeat && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime)
				verbl_msg = "I've been trying all this time to make this glyph do something, but the only thing that happened was that I'm getting hungry!";
			else if (mtmp->mconf || mtmp->mflee)
				verbl_msg = "Where is the button? Hey, can you take a look, do you see a button somewhere on this stone glyph?";
			else
				verbl_msg = "I know that the magic glyphs exist, and I even know the name of the place where they supposedly can be found, but I forgot the name! Damn!";
		}
		else verbl_msg = "Go away, I'm looking for the magic glyphs, they have to be around here somewhere...";
		break;

	case MS_SOCKS:

		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3) {
				pline_msg = "are very sad that they've been damaged so badly...";
			} else
				pline_msg = "look sexy.";
		}
		else pline_msg = "emit an exciting smell.";

		break;
	case MS_PANTS:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3) {
				pline_msg = "look quite torn.";
			} else
				pline_msg = "are quite erotic.";
		}
		else pline_msg = "want you to take a whiff from them.";

		break;
	case MS_HANDY:
		if (mtmp->handytime) {
			pline_msg = "is on the phone and can't be interrupted right now!";
			break;
		}

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I gotta call an ambulance!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Do you happen to have the phone number of that pizza delivery service?";
			break;
		}
		if (mtmp->mtame) verbl_msg = "Go ahead, I'll follow you. But please be aware that I'm expecting an important phone call.";
		else verbl_msg = "Don't disturb me, I'm expecting a phone call!";

		break;
	case MS_CAR:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Just hold on Tetsuo. I'm gonna find you to cure your pain.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Teeeeeeeee tsuuuuuuu oooooooo!";
			break;
		}
		if (mtmp->mtame) verbl_msg = "Relax, it's gonna take some time to warm my engine up.";
		else verbl_msg = rn2(2) ? "Bo-Bo-Bo" : "Va-Va-Va";

		break;
	case MS_JAPANESE:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/10) {
			verbl_msg = "Kofuku shimasen! Watashi wa katsu ka, watashi no kuni no tame ni shinudeshou!";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Watashitachi no firudodokuta ni denwa shite kudasai!";
			break;
		}
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "Nani? Chugokujin wa shinryaku shite imasu ka? Soretomo daitoryo wa kakumisairu o hassha shita nodeshou ka?";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Yasai o tori ni itte kudasai.";
			break;
		}
		if (mtmp->mtame && mtmp->bleedout) {
			verbl_msg = "Kizuguchi ni subayaku hotai o maku!";
			break;
		}
		if (mtmp->mtame) verbl_msg = "Kekko.";
		else if (mtmp->mpeaceful) verbl_msg = "Go aisatsu.";
		else if (mtmp->mfrenzied) verbl_msg = "Watashi ni kohei-sa o kitai shinaide kudasai.";
		else verbl_msg = "Anata wa tekidesu.";

		break;
	case MS_SOVIET:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/10) {
			verbl_msg = "K chertu vsyu igru, kto voobshche zaprogrammiruyet etu fignyu.";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Skazhi programmistu, chtoby on otsosal mne.";
			break;
		}
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "YA sluchayno sozdal svoy variant? Eto neobychno.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Ba, v moyey igre ya smogu protyanut' dol'she bez yedy.";
			break;
		}
		if (mtmp->mtame && mtmp->bleedout) {
			verbl_msg = "Pochemu etot statusnyy effekt voobshche imeyet znacheniye.";
			break;
		}
		if (mtmp->mtame) verbl_msg = "Da ladno, ya ne khochu razygryvat' etu glupost'.";
		else if (mtmp->mpeaceful) verbl_msg = "Pochemu ya voobshche dolzhen byt' zdes'?";
		else if (mtmp->mfrenzied) verbl_msg = "Vse neuravnovesheno i ne imeyet smysla, i Emi deystvitel'no stoit obratit'sya k psikhiatru, potomu chto u neye opredelenno yest' bol'she, chem prosto neskol'ko nezakreplennykh vintov.";
		else verbl_msg = "Skoro ty umresh', i togda tebe ne ostanetsya nichego drugogo, kak sygrat' v ledyanoy blok tipa yego variant, kotoryy namnogo luchshe, chem etot musor!";

		break;
	case MS_SNORE:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "breathes faintly.";
			else if (mtmp->mtame && hastoeat && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "wants to eat a lot.";
			else
				pline_msg = "snores peacefully.";
		}
		else pline_msg = "snores.";
		break;
	case MS_PHOTO:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I'll take a photo of whoever hurt me there, and then we'll manhunt that person!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Let's go to the cafeteria, I need a little snack!";
			break;
		}
		if (mtmp->mtame) {
			if (ACURR(A_CHA) > 17) verbl_msg = "Wow, you look splendid! I'm happy to accompany you!";
			else if (ACURR(A_CHA) > 17) verbl_msg = "Hey, good looking!";
			else verbl_msg = "Let's look for good photo opportunities.";
		}
		else verbl_msg = "Don't move, I want to take a photo of you!";

		break;
	case MS_REPAIR:

		{
			if (mtmp->repaircredit) verbalize("Welcome back to my repair shop. You still have %d credit remaining for repairs.", mtmp->repaircredit);
			else verbalize("Welcome to my repair shop. You can pawn gems to me to generate credit, and then use that credit to have your equipment repaired!");

			winid tmpwin;
			anything any;
			menu_item *selected;
			int n;

			any.a_void = 0;         /* zero out all bits */
			tmpwin = create_nhwindow(NHW_MENU);
			start_menu(tmpwin);
			any.a_int = 1;
			add_menu(tmpwin, NO_GLYPH, &any , 'p', 0, ATR_NONE, "Pawn Gems", MENU_UNSELECTED);
			any.a_int = 2;
			add_menu(tmpwin, NO_GLYPH, &any , 'r', 0, ATR_NONE, "Repair", MENU_UNSELECTED);

			end_menu(tmpwin, "Services Available:");
			n = select_menu(tmpwin, PICK_ONE, &selected);
			destroy_nhwindow(tmpwin);

			if (n > 0) {
				switch (selected[0].item.a_int) {
					case 1:
						{
							struct obj *pawngem;
							int gemvalue = 0;
							pawngem = getobj((const char *)pawnables, "pawn");
							if (!pawngem) break;
							if (pawngem->oclass != GEM_CLASS) {
								verbalize("That's not a gem!");
								break;
							}
							if (pawngem->otyp >= ELIF_S_JEWEL) {
								verbalize("Sorry, I only accept real gems or worthless glass.");
								break;
							}
							if (pawngem->otyp > JADE) gemvalue = 5;
							else gemvalue = objects[pawngem->otyp].oc_cost;
							if (pawngem->spe > 0) gemvalue += (pawngem->spe * 10);
							if (pawngem->spe < 0) {
								gemvalue += (pawngem->spe * 10);
								if (gemvalue < 5) gemvalue = 5;
							}
							if (mtmp->mpeaceful) {
								gemvalue *= 5;
								gemvalue /= 4;
							}
							if (pawngem->oeroded == 3) gemvalue /= 4;
							else if (pawngem->oeroded == 2) gemvalue /= 3;
							else if (pawngem->oeroded == 1) gemvalue /= 2;
							if (pawngem->oeroded2 == 3) gemvalue /= 4;
							else if (pawngem->oeroded2 == 2) gemvalue /= 3;
							else if (pawngem->oeroded2 == 1) gemvalue /= 2;
							if (gemvalue < 1) gemvalue = 1;

							if (pawngem->quan > 1) {
								pawngem->quan--;
								pawngem->owt = weight(pawngem);
							} else useup(pawngem);

							mtmp->repaircredit += gemvalue;
							verbalize("Alright, your credit was increased by %d and amounts to %d now.", gemvalue, mtmp->repaircredit);

						}
						break;
					case 2:
						{
						register struct obj *repairotmp;
						int repaircost = 0;

						if (mtmp->repaircredit < 1000) {
							verbalize("Sorry, you only have %d credit, that's too little.", mtmp->repaircredit);
							break;
						}

repairitemchoice:
						repairotmp = getobj(allnoncount, "magically repair");
						if (!repairotmp) {
							break;
						}

						if (!repairotmp->oeroded && !repairotmp->oeroded2) {
							verbalize("That item doesn't need repairs!");
							break;
						}

						if (repairotmp->oeroded == 3) repaircost += 10000;
						if (repairotmp->oeroded == 2) repaircost += 2500;
						if (repairotmp->oeroded == 1) repaircost += 1000;
						if (repairotmp->oeroded2 == 3) repaircost += 10000;
						if (repairotmp->oeroded2 == 2) repaircost += 2500;
						if (repairotmp->oeroded2 == 1) repaircost += 1000;

						if (repaircost > mtmp->repaircredit) {
							verbalize("Sorry, you only have %d credit, and repairing that item would cost %d.", mtmp->repaircredit, repaircost);
							break;
						}

						verbalize("It'll cost you %d zorkmids to repair that.", repaircost);
						if (yn("Pay?") != 'y') {
							verbalize("Ah well, maybe next time.");
							break;
						}
						mtmp->repaircredit -= repaircost;

						if (repairotmp && stack_too_big(repairotmp)) {
							pline("The stack was too big and therefore didn't get repaired!");
						} else if (repairotmp) {
							if (!Blind) {
								pline("Your %s looks as good as new!",xname(repairotmp));
							}
							if (repairotmp->oeroded > 0) { repairotmp->oeroded = 0; }
							if (repairotmp->oeroded2 > 0) { repairotmp->oeroded2 = 0; }
							if (repairotmp && objects[(repairotmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(repairotmp)) {
								if (!repairotmp->cursed) bless(repairotmp);
								else uncurse(repairotmp, FALSE);
							}

						}

						}
						break;
				}
			}
		}

		break;
	case MS_DRUGS:
		{
			register int cnt = 0;
			register struct monst *mkop, *mkop2;
			int kopchance = 10;

			verbalize("Ey yo man, what you need, dog?");
			if (yn("Purchase drugs?") != 'y') {
				verbalize("Keep calm, man!");
				break;
			}
			if (u.ugold < 1000) {
				verbalize("You ain't got the cash, man!");
				break;
			}
			u.ugold -= 1000;
			adjalign(-5);

			struct obj *medkit;
			medkit = mksobj(rn2(2) ? MUSHROOM : PILL, TRUE, FALSE, FALSE);
			if (medkit) {
				medkit->quan = 1;
				medkit->known = medkit->dknown = medkit->bknown = medkit->rknown = 1;
				medkit->owt = weight(medkit);
				dropy(medkit);
				stackobj(medkit);
			}
			verbalize("Yo, cool man, go check out the ground but shhhh!");

			for (mkop = fmon; mkop; mkop = mkop2) {
			    mkop2 = mkop->nmon;
			    if (mkop->data->mlet == S_KOP) {
				cnt++;
			    }
			}
			while (cnt > 0) {
				cnt--;
				if (!rn2(4)) kopchance--;
			}
			if (kopchance < 3) kopchance = 3;
			if (!rn2(kopchance)) {
				u.copwantedlevel += rnz(1000);
				verbalize("WEOWEO");
			}

			if (!rn2(kopchance)) {
				int randsp;
				int i;
				coord cc, dd;
				int cx,cy;
			      cx = rn2(COLNO);
			      cy = rn2(ROWNO);

				randsp = rno(14);
				if (!rn2(10)) randsp += rno(2);
				if (!rn2(100)) randsp += rno(5);
				if (!rn2(1000)) randsp += rno(10);
				if (!rn2(10000)) randsp += rno(20);

				for (i = 0; i < randsp; i++) {
					if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;
					(void) makemon(mkclass(S_KOP,0), cx, cy, MM_ADJACENTOK|MM_ANGRY|MM_FRENZIED);
				}

			}

			if (!rn2(7)) {
				mongone(mtmp);
				verbalize("Yo man this too hot here, later!");
				return(1);
			}
		}
		break;
	case MS_COMBAT:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/10) {
			verbl_msg = "If I don't get some source of healing, the next hit will kill me!";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I'm low on health! Hopefully you have some healing powers!";
			break;
		}
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "I'm confused! Could you lend me your unicorn horn?";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Find me something to eat!";
			break;
		}
		if (mtmp->mtame && mtmp->bleedout) {
			verbl_msg = "Dang, I'm still bleeding from that last hit! Can you heal me? That should take care of it!";
			break;
		}
		if (mtmp->mtame) verbl_msg = "I'm looking forward to the next battle!";
		else if (mtmp->mpeaceful) verbl_msg = "I'd like to fight some enemies!";
		else if (mtmp->mfrenzied) verbl_msg = "I'll finish you off!";
		else verbl_msg = "Come! Fight me!";
		break;
	case MS_MUTE:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "moans.";
			else if (mtmp->mtame && hastoeat && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "grunts like a pig.";
			else
				pline_msg = "grunts peacefully.";
		}
		else pline_msg = rn2(2) ? "barks threateningly." : "grunts angrily.";
		break;
	case MS_CORONA:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I'm dying of covid-19! Bring me to the intensive care unit, FAST!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Man, everything tastes the same! But I really need to eat something...";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = rn2(2) ? "Lead the way, and be sure to always wear your face mask properly." : "Careful, I know we're on the same team, but please don't get closer than 1.50 meters to me.";
		}
		else verbl_msg = rn2(2) ? "Stop! You need to wear a face mask! I'm not gonna let you spread the virus to me!" : "Stay away! I don't want to contract your covid infection! Keep a minimum distance of 2 meters, got it?";
		break;
	case MS_TRUMPET:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "lets out a tormented squeak.";
			else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "emits a long sigh.";
			else
				pline_msg = "trumpets.";
		}
		else pline_msg = mtmp->mpeaceful ? "trumpets peacefully." : "roars!";
		break;
	case MS_PAIN:
		verbalize("My health status is %d HP out of a maximum of %d!", mtmp->mhp, mtmp->mhpmax);
		break;
	case MS_BRAG:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Bah, I won't die that easily! I still got some hit points left!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Once I've eaten, my strength will be even higher than before!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "I am the best!";
		}
		else verbl_msg = "Bah, I'm much better than you!";
		break;
	case MS_LIEDER:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "If you let me die now, I can't give you advice on how to beat your quest anymore!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Hey, I'll help you, but first I need to eat something!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Keep going forward, you'll be able to beat your quest!";
		}
		else verbl_msg = "How far have you gotten in your quest?";
		break;
	case MS_GAARDIEN:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Why are you letting my health drop so low? Can't you cast a healing spell on me?";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Hungry!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "The boss is counting on you.";
		}
		else verbl_msg = "Get moving, that quest doesn't complete itself!";
		break;
	case MS_MISSING:
		pline("%s does not make a sound.", Monnam(mtmp));
		break;
	case MS_SELFHARM:
		if (mtmp->mhp < mtmp->mhpmax/10) {
			verbl_msg = "The suffering will be over soon.";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Hopefully I manage to kill myself this time...";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "No, I don't want to eat! I just want to die!";
			break;
		}
		if (mtmp->bleedout > 100) {
			verbl_msg = "Uhh... maybe I did go too far this time after all, I think I've hit styro...";
			break;
		}
		if (mtmp->bleedout > 50) {
			verbl_msg = "This is looking good, I only have to apply the razorblade again...";
			break;
		}
		if (mtmp->bleedout > 0) {
			verbl_msg = "That didn't do much damage, perhaps I should cut deeper...";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "I always have my tools at hand. Don't worry, I'm fine.";
		}
		else verbl_msg = "Damn, I'm having THE urge!";
		break;
	case MS_CLOCK:
		if (mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Damn, the clock is broken!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Uhh, I hope it's not too late to get some food! Do you happen to know a restaurant that is open now?";
			break;
		}

		time_t rawtime;
		struct tm *info;
		time(&rawtime);
		info = localtime(&rawtime);

		pline("'The time is: %s'", asctime(info));
		break;
	case MS_OHGOD:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Oh-god-o-matic!";
			break;
		}
		if (mtmp->mconf) {
			verbl_msg = "Oh Buddha! Or was it Jahwe? Oh Allah!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Ohgod no food! Ohgod!";
			break;
		}
		verbl_msg = "Oh goooood...";
		break;
	case MS_WOLLOH:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Wolloh that shit's fucked up wolloh!";
			break;
		}
		if (mtmp->mconf) {
			verbl_msg = "Hollow? Why is everything hollow?";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Wolloh age, let's go doener kebab wolloh!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Wolloh come with me!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Wolloh wassup?";
		}
		else verbl_msg = "Wolloh, I'm gonna fuck you up asshole! Wolloh age!";
		break;
	case MS_CODE:
		if (mtmp->codeguessed) {
			verbalize("Sorry, you already took a guess.");
		} else {
			mtmp->codeguessed = TRUE;
			verbalize("What is the secret four-digit code number?");

			char buf[BUFSZ];
			long offer;

			getlin("Your guess:", buf);
			if (sscanf(buf, "%ld", &offer) != 1) offer = 0L;
			if (offer < 0) {
				verbalize("Certainly not, mastermind!");
			} else if (offer > 9999) {
				verbalize("Nice try, but no, the code is four-digit. You gave too many digits.");
			} else if (offer == u.secretcodenumber) {
				u.secretcodenumber = rn2(10000);
				verbalize("We have a winner!!! You shall receive a boon.");
				if (!rn2(4)) makewish(evilfriday ? FALSE : TRUE);
				else othergreateffect();
			} else {
				verbalize("Sorry, your guess was wrong.");
			}

		}

		break;
	case MS_BARBER:

		if (u.barbertimer < 5000) {
			verbalize("Sorry, your last haircut was just %d turns ago, you have to wait a bit until I can give you another!", u.barbertimer);
			break;
		}

		{
			int barbercost = 250;
			if (u.barberamount > 0) barbercost += (u.barberamount * 50);
			if (u.barbertimer > 5999) {
				int barberxtracost = u.barbertimer - 5000;
				while (barberxtracost >= 1000) {
					barberxtracost -= 1000;
					barbercost += 100;
				}
			}

			verbalize("Hey %s, want a new haircut? Just %d zorkmids!", flags.female ? "girl" : "man", barbercost);

			if (yn("Do you want a new haircut?") != 'y') {
				verbalize("Oh well, if you don't wanna... you know where to find me if you change your mind.");
			} else {
				if (u.ugold < barbercost) {
					verbalize("Sorry, I'm not working for free. Get enough cash first!");
				} else {
					int charismagain = 1;
					if (Role_if(PM_LADIESMAN)) charismagain++;

					u.ugold -= barbercost;
					u.barberamount++;
					u.barbertimer = 0;
					if (flags.female) {
						u.femalehaircut = rnd(55);
					} else {
						u.malehaircut = rnd(18);
						u.malebeard = rnd(48);
					}
					(void) adjattrib(A_CHA, charismagain, 0, TRUE);

	
					d_level flev;

					if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
						verbalize("Alright, done. I'll be off looking for another customer! Seeya!");
						mongone(mtmp);
						Your("new hairstyle is '%s.", bundledescription());
						return(1);
					}
					flev = random_branchport_level();
					migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM, (coord *)0);
					verbalize("Alright, done. I'll be off looking for another customer! Seeya!");
					Your("new hairstyle is '%s.", bundledescription());
					return(1);
				}
			}
		}

		break;
	case MS_AREOLA:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			pline_msg = "doesn't seem very happy.";
			break;
		}
		if (mtmp->mconf) {
			pline_msg = "seems to be lost in thought...";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			pline_msg = "wants to eat a lot.";
			break;
		}
		if (mtmp->mtame) {
			pline_msg = "puffs out its chest with pride.";
		} else if (mtmp->mpeaceful) {
			pline_msg = "looks surprised.";
		}
		else pline_msg = "spits on the ground.";
		break;
	case MS_STABILIZE:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			pline_msg = "tries to deactivate the force field so that an emergency teleport spell can be cast.";
			break;
		}
		if (mtmp->mconf) {
			pline_msg = "seems to have forgotten the original plan.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			pline_msg = "needs to refuel.";
			break;
		}
		if (mtmp->mtame) {
			pline_msg = "tells you that the force field will improve your chance of success.";
		} else if (mtmp->mpeaceful) {
			pline_msg = "merrily remarks that the force field is working well.";
		}
		else pline_msg = "tells you that you won't break through the force field!";
		break;
	case MS_ESCAPE:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "That's it, I'm out of here! Catch me if you can, I'm the Gingerbread Man!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Where's the canteen?";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Okay, I'll follow you.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Pinky, are you pondering what I'm pondering?";
		}
		else verbl_msg = "Are you crazy? Well, neglecting to wear safety gear does verge on the edge of lunacy... anyway, stop attacking me, immediately!";
		break;
	case MS_OE:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Need healing!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Need food!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Lead the way!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "What's going on?";
		}
		else verbl_msg = "Enemy!";
		break;
	case MS_BAN:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I'll get out of this area soon.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Give me something to eat!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "I'll follow you for now.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "This area seems reasonably safe.";
		}
		else verbl_msg = "Get away from me!";
		break;
	case MS_TALONCOMPANY:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "This ain't what I signed up for!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Food! Get me some fucking food!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Yeah?";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Hello.";
		}
		else verbl_msg = "You're dead!";
		break;
	case MS_REGULATOR:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "This ain't what I signed up for!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Gotta eat another donut, I'm hungry!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Yeah?";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Hello.";
		}
		else verbl_msg = "Well what do we have here, the Vault rogues are back.";
		break;
	case MS_RAIDER:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Oh God, I don't want to die!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Where's the damn meat shipment?";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Shh, not so loud. The others shouldn't notice that I'm doing business with you.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Well, if that ain't a very alive horn ox!";
		}
		else verbl_msg = "Hahahahahahaha! Time to kill!";
		break;
	case MS_ENCLAVE:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Retreat and regroup!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Need a C-ration!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Greetings, commander.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Hello.";
		}
		else verbl_msg = "Attack is continued!";
		break;
	case MS_MUTANT:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Please... rescue me...";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Almost no green stuff left...";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "What's going on?";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "I seem to remember something... from before. I knew a woman... or maybe I *WAS* one...";
		}
		else verbl_msg = "Hahahahahahaha! Let's play, little human!";
		break;
	case MS_MYTHICALDAWN:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "The old Tamriel still has some power left!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Once I'm in paradise I'll eat lichor and ambrosia in great profusion!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "We will hunt those accursed Blades.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Lord Dagon will walk the earth again!";
		}
		else verbl_msg = "You're weak, just like the gods you pray to.";
		break;
	case MS_DREMORA:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Stop stealing my air.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Dark and deep!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Heel, doggie!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Begone, mortal.";
		}
		else verbl_msg = "On your knees, coward!";
		break;
	case MS_BOS:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Damn, I'm gonna have to contact Scribe Jameson, for the scrolls...";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "I need a lunch break.";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Greetings.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Don't worry, you're safe in our presence.";
		}
		else verbl_msg = "Enemy contact!";
		break;
	case MS_OUTCAST:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I'm about to expire!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Gonna have to eat a lot!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Don't ever forget the fire that forged you, or you're lost.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Casdin made the right choice when he left Lyons' little group of do-gooders.";
		}
		else verbl_msg = "Old man, kiss my ass!"; /* referring to Elder Lyons, not your player character */
		break;
	case MS_GENDER:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbalize("I may die, but let everyone know that I'm %s!", is_neuter(mtmp->data) ? "neuter" : mtmp->female ? "female" : "male");
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbalize("This %s needs something to eat!", is_neuter(mtmp->data) ? "neutrum" : mtmp->female ? "woman" : "man");
			break;
		}
		if (mtmp->mtame) {
			verbalize("Do you like the fact that you have a %s ally?", is_neuter(mtmp->data) ? "neuter" : mtmp->female ? "female" : "male");
		} else if (mtmp->mpeaceful) {
			verbalize("Hey look at me! I'm %s!", is_neuter(mtmp->data) ? "neuter" : mtmp->female ? "female" : "male");
		}
		else verbalize("You're gonna lose to a %s!", is_neuter(mtmp->data) ? "neutrum" : mtmp->female ? "girl" : "boy");
		break;
	case MS_AMNESIA:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Forget about it.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Hmm, I forgot what I'm supposed to do when my stomach growls...";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Hey, what do you want from me, I don't know you!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "I don't remember seeing you before.";
		}
		else verbl_msg = "Huh, who are you?";
		break;
	case MS_SHIVERINGESLES:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Ich habe den Zaxer gesehen. Und ich habe ihn gemaggt. Ach, fribbel!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Gebt ihnen Gargen zum Emmet.";
			break;
		}
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "Grundi Obelisken! Immer tetteraten sie Ritter und Festars. Verwendet den Dookas, um sie in Ordnung zu bringen.";
			break;
		}
		if (mtmp->mtame && mtmp->mstun) {
			verbl_msg = "Ich wette einen Quintat, dass sie ipanaten koennen.";
			break;
		}
		if (mtmp->mtame && mtmp->mtrapped) {
			verbl_msg = "Dunkel-dunkel. Wir haben einen Zeffel geyingt.";
			break;
		}
		if (mtmp->mtame && mtmp->mcan) {
			verbl_msg = "Zeut Femul dimat! Ich boestle, bevor ich redauple.";
			break;
		}
		if (mtmp->mtame && mtmp->mblinded) {
			verbl_msg = "Sagt dem Dagen Bescheid! Dem Dagen! Karn Himmel relft schon.";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Nehmt Zickel. Niemand macht rebbi Zickel heutzutage.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Gal bursten es.";
		}
		else verbl_msg = "Uungor spredet, er 'tschaui.' Spredet mit Uungor. Ihr sterfeht mich? Spredet mit Uungor! Gebt Freund Pennigkeit in Uungors Ratzepueklo.";
		break;
	case MS_ANOREXIA:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Can't I just abstain from eating in peace?";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "I'm still not skinny enough!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Do you think I'm skinny and beautiful?";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Please don't tell me I'm fat!";
		}
		else verbl_msg = "You think I'm fat, I know it.";
		break;
	case MS_BULIMIA:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Fuck the whole world!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "I need to eat a lot!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "I'm happy and feeling fine!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Nice day today, huh?";
		}
		else verbl_msg = "Don't you DARE comment on my teeth!";
		break;
	case MS_TEMPER:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Need healing!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Hungry!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Sup!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Eh!";
		}
		else verbl_msg = "GRR!";
		break;
	case MS_BLANKER:
		/* todo */
		break;
	case MS_CONDESCEND:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I'm not gonna die! I'll just heal!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Once I've eaten, I'll finish off my enemy!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "You're my subordinate, just obey my command and everything is fine.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Oh, what a cute little specimen you are.";
		}
		else verbl_msg = "You will fall like wheat before my sword.";
		break;
	case MS_MOLEST:
		if (flags.female) {
			if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
				verbl_msg = "I'm not gonna go down like that! My penis still has lots of semen left for inseminating women!";
				break;
			}
			if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
				verbl_msg = "Hey, girl! Make me a sandwich, will ya?";
				break;
			}
			if (mtmp->mtame) {
				verbl_msg = "Oh girl, I'm looking forward to landing you in bed tonight...";
			} else if (mtmp->mpeaceful) {
				verbl_msg = "Do you like it if I massage your buttocks with my hands?";
			}
			else verbl_msg = "Now my penis will penetrate your vagina, girl. Hehehehehe.";
		} else {
			if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
				verbl_msg = "Gah why the hell do there have to be so many men on this planet FUCK THAT SHIT!";
				break;
			}
			if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
				verbl_msg = "Bring me some food, fast!";
				break;
			}
			if (mtmp->mtame) {
				verbl_msg = "Gah, why do you have to be a man?";
			} else if (mtmp->mpeaceful) {
				verbl_msg = "You're not interesting to me because you have a penis.";
			}
			else verbl_msg = "God, why are you male? Ah well, I'll just get rid of you real quick and then I'll find some girls to play with!";
		}

		break;
	case MS_SHOCKTROOPER:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "ka ga blis blis ka";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "puh pur hyu ka";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "kss wirt ras";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "kss kyur kiml";
		}
		else verbl_msg = "kiml kiml";
		break;
	case MS_GAGA:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "The LGBTIQ+ community will avenge me...";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Can you procure some food that's suitable for my gender?";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Always be aware of my gender.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Hey, did I tell you about my gender yet?";
		}
		else verbl_msg = "You have to use the correct pronouns when talking to me!!!!!";
		break;
	case MS_COMMON:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "My health is low!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "I need to eat something!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Hello, friend.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Greetings, adventurer.";
		}
		else verbl_msg = "You are my enemy.";
		break;
	case MS_PIRATE:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Blow me down!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Splice the mainbrace!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Weigh anchor and hoist the mizzen!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Yo Ho Ho!";
		}
		else verbl_msg = "Scurvy dog!";
		break;
	case MS_BUSY:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I can't do this anymore...";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Quick lunchbreak before I get back to work!";
			break;
		}
		else verbl_msg = "I'm very busy! Please don't disturb me!";
		break;
	case MS_SILLY:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Soon I will go dead, but bring myself back into the play.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Now I need to eat a food ration that contains meat, because I don't have an eating disorder.";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Do you want me to alternate between massaging your nuts and kicking them?";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "The block heels are made of cuddle cloth!";
		}
		else verbl_msg = "Now I will pull you the pants down!";
		break;
	case MS_MIDI:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "rnn rnn rnn RNN RNN RNN!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "deeeeeeeee, dededidadaDEEEEEEE-daaaaa-deeeeee!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "klimpklamp";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "klampklimpklamp";
		}
		else verbl_msg = "klilili!";
		break;
	case MS_HERCULES:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Impossible. I cannot fail.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Even I have to eat something from time to time. So, where can we get some meat?";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "With your assistance, we will overcome all the obstacles.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Hey, I'm on a task given to me by the gods themselves!";
		}
		else verbl_msg = "You won't stop me! I'm a God, you puny mortal!";
		break;
	case MS_BEG:
		verbalize("Hey, I have no money to buy food, will you spare %d zorkmids, noble adventurer?", mtmp->m_lev + u.ulevel);
		if (u.ugold >= (mtmp->m_lev + u.ulevel)) {
			if (yn("Give the beggar some cash?") == 'y') {
				verbalize("Thanks! I'll never forget this.");
				u.ugold -= (mtmp->m_lev + u.ulevel);
				if (!mtmp->mpeaceful) {
					mtmp->mpeaceful = TRUE;
					mtmp->mfrenzied = FALSE;
				}
				if (Race_if(PM_HUMANOID_DEVIL)) devil_misbehavior();
				else if (u.ualign.type != A_CHAOTIC) adjalign(1);
			} else {
				verbalize("You're so cheap!");
			}
		} else {
			pline("But you don't have enough money.");
		}

		break;
	case MS_HIRE:

		if (mtmp->mfrenzied) {
			verbl_msg = "Grrrrrr! You're gonna die, traitor!";
			break;
		}

		else if (mtmp->mtame) {
			if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
				verbl_msg = "Heal me!!";
				break;
			}
			if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
				verbl_msg = "Need a pit stop, I'm hungry!";
				break;
			}

			verbl_msg = "Lead the way!";
			break;

		} else if (!mtmp->mfrenzied && !mtmp->mtame) {

			int hirecost = 1000 + (mtmp->m_lev * 50);

			verbalize("Hey, I'll join your team for %d zorkmids.", hirecost);

			if (u.ugold >= hirecost) {
				if (yn("Hire the monster?") == 'y') {
					struct monst *hiredmon;

					u.ugold -= hirecost;

					hiredmon = tamedog(mtmp, (struct obj *) 0, TRUE);
					if (hiredmon) mtmp = hiredmon;
					if (!mtmp) impossible("hireable monster was tamed but doesn't exist now??");

					verbalize("Alright! Let's fight the badguys together!");

				}
			} else {
				pline("But unfortunately you don't have enough money.");
			}
			break;
		}
		break;

	case MS_ARMORER:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Huuuuu!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "GRR! Hungry!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Arrr.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "You will not get my hammer!";
		}
		else verbl_msg = "I'm gonna make weapons from your bones!";
		break;

	case MS_OMEN:
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "Hmm, maybe 200 years from now, robots will take over the world...";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I sense the equilibrium drawing near!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Something tells me that I'll soon be eating a big meal.";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Beware, taking me along will endarken your future.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Your future entails something nasty, I'm afraid...";
		}
		else verbl_msg = "You don't have a future! You're gonna die, a hundred percent sure!";
		break;

	case MS_NEWS:
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "Which newspaper was I reporting to again?";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "My death will make the biggest headline!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Hmm, I gotta report the lack of restaurants in this area...";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Let's look for something sensational!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Hi! Did you hear the news?";
		}
		else verbl_msg = "Leave me alone, I'm trying to get some footage here!";
		break;

	case MS_CRYTHROAT:
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "OEOEOEOEOEOEOEOEOEOEOEHHHHHHHHHH";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "OWWOW! SOMEONE HAS HURT ME, OWWOW! I DON'T WANT THAT!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "MY BELLY ACHES SO MUCH! PLEASE MAKE IT STOP HURTING, OWW!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "HELP ME! EVERYONE IS SO MEAN TO ME!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "IS THERE NO ONE WHO CAN HELP ME?";
		}
		else verbl_msg = "YOU MEAN, EVIL... WHY ARE YOU SO UNFAIR... NOW I HAVE TO CRY!";
		break;

	case MS_HOOT:
		if (mtmp->mhp < mtmp->mhpmax/3) {
			pline_msg = "hoots weakly.";
			break;
		}
		if (mtmp->mconf) {
			pline_msg = "hoots a wrong tune.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			pline_msg = "hoots a whiny tune.";
			break;
		}
		if (mtmp->mtame) {
			pline_msg = "hoots calmly.";
		} else if (mtmp->mpeaceful) {
			pline_msg = "hoots.";
		}
		else pline_msg = "hoots aggressively!";
		break;

	case MS_RUSTLE:
		if (mtmp->mhp < mtmp->mhpmax/3) {
			pline_msg = "looks almost dead!";
			break;
		}
		if (mtmp->mconf) {
			pline_msg = "hums dissonantly.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			pline_msg = "looks rather colorless.";
			break;
		}
		if (mtmp->mtame) {
			pline_msg = "rustles calmly.";
		} else if (mtmp->mpeaceful) {
			pline_msg = "rustles.";
		}
		else pline_msg = "screams at you angrily!";
		break;

	case MS_SEMEN:
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "Where is the vagina? Is it inside the penis?";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "If I catch whoever hurt me like that, they'll wish they had never been born!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Gotta eat something so that my penis can produce new semen!";
			break;
		}
		if (flags.female) {
			if (mtmp->mtame) {
				verbl_msg = "Hey, I like you. Would you like to go to bed with me tonight?";
			} else if (mtmp->mpeaceful) {
				verbl_msg = "Want me to ejaculate my semen into your orifice, girl? *snicker*";
			}
			else verbl_msg = "You will be forced to give birth to my child, bitch!";
		} else {
			if (mtmp->mtame) {
				verbl_msg = "The only reason I'm tolerating your presence is because you're going to find me a nice girl that I can fuck.";
			} else if (mtmp->mpeaceful) {
				verbl_msg = "Go away, I don't like male people. They're a waste of space.";
			}
			else verbl_msg = "Fuck you, male asshole! I shall cut off your fucking penis!";
		}
		break;

	case MS_SCIENTIST:
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "Do you think we should appeal to the alien authorities for guidance?";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I seem to be wounded, but I can keep bleeding!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "At least we no longer need to eat that cafeteria food.";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Eureka! Oh, sorry, just practising.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Quit staring, you're giving me the heebie-jeebies!";
		}
		else verbl_msg = "I'm not following you, you evil person!";
		break;

	case MS_WEATHER:
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "Oh, I think we have a portal storm! We need to move away from all friendly NPCs so that they're outside of the reality bubble, or they'll just die!"; /* I hopefully don't have to mention just how utterly unrealistic that is, having stuff not happen just because you're more than two screens away... --Amy */
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Agh! I got hit by lightning!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "If you want another weather forecast, please wait until I've eaten!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Always take the weather with you, everywhere you go!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Nice weather, huh?";
		}
		else verbl_msg = "You look like three days of rainy weather.";
		break;

	case MS_INCISION:
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "Uhh, I'm not so sure, do the gods actually approve of our practices?";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Rescue me! I'm a state-approved medical doctor!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Where's my extra bloody beefsteak?";
			break;
		}
		if (flags.female) {
			if (mtmp->mtame) {
				verbl_msg = "Did you know there's a huuuuuuge amount of girls in this world whose genitals are waiting for my scalpel?";
			} else if (mtmp->mpeaceful) {
				verbl_msg = "Since it's unfair to perform genital circumcision on males only, I perform it on females, too. Why, that's not an act of mutilation, why are you asking?";
			}
			else verbl_msg = "Aaaaaahhh! A female! Look, don't scream like a woman, I'm just going to circumcise your genitals. Every woman at your age has to undergo that surgery, and most of them are grateful when it's complete. You won't be an exception.";
		} else {
			if (mtmp->mtame) {
				verbl_msg = "Let's find someone whose penis I can circumcise.";
			} else if (mtmp->mpeaceful) {
				verbl_msg = "Every grown-up male must have his foreskin removed. No exceptions will be made.";
			}
			else verbl_msg = "Having an intact foreskin is a crime! I shall remove yours with my blood-smeared scalpel!!";
		}
		break;

	case MS_MODALSHOP:

		if (mtmp->mfrenzied) {
			verbl_msg = "!Te dare la vuelta los intestinos!";
		} else if (!mtmp->mpeaceful && !mtmp->mtame) {
			verbl_msg = "Estas arruinando el ambiente.";
		} else {
			verbl_msg = "Hola, ?quieres ver lo que tengo en venta?";
		}
		break;

	case MS_FEARHARE:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Help me, help me! I can't handle them myself!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Please, let's get something to eat, I'm afraid that if I don't eat, then some monster will eat me!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Are you sure you know where you're going?";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "The situation clearly can't get any worse than this.";
		}
		else verbl_msg = "Stop attacking, I'm a fiend! Err, friend! Fruck, I always mess that R up.";
		break;
	case MS_SPOILER:
		if (mtmp->mhp < mtmp->mhpmax/3) {
			pline_msg = "Unhandled exception: At address 002EF0C";
			break;
		}
		if (mtmp->mconf) {
			pline_msg = "Application termination, app event: BEX";
			break;
		}
		if (mtmp->mtame) {
			pline_msg = "Network Error: NO ERROR";
		} else if (mtmp->mpeaceful) {
			pline_msg = "Error: No route to host";
		}
		else pline_msg = "Runtime Error - program is running at the wrong time of the day. Please restart the program later.";
		break;
	case MS_DEAD:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Now I have to experience death a second time, because of you...";
			break;
		}
		if (mtmp->mconf) {
			verbl_msg = "Huh, am I dead now? I think I'm dead! Or is this all just a dream?";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Oh well, I think I'll just starve to death now, that wouldn't be the worst possible fate.";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Yeah I guess I can assist you...";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Who resurrected me?!";
		}
		else verbl_msg = "I tell you, if you're responsible for disturbing my peaceful slumber I'll make sure that you die before I do!";
		break;
	case MS_TREESQUAD:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Even if I die, the trees will be protected by my comrades!";
			break;
		}
		if (mtmp->mconf) {
			verbl_msg = "What is that huge mana flower doing there, can I harvest ambrosia or lichor from it?";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Gotta eat some acorns and berries!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Don't hesitate to kill anyone who dares chopping down a tree.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Remember, nature has to be protected at all times. Everyone who applies an axe to a tree is a criminal!";
		}
		else verbl_msg = "You devil, I'll make sure you never fell a tree again!";
		break;
	case MS_METALMAFIA:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Dammit, they're defending their metal with teeth and claws!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Let's spend our money on a good meal, and then we'll resume stealing metal!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Let's go! We gotta find someone who has valuable metal that we can steal and sell for profit!";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "Metal is the only material worth anything on the black market.";
		} else if (mtmp->mfrenzied) {
			verbl_msg = "I'm gonna kill you, and then I'll loot your backpack for metallic items that I can sell.";
		} else {
			verbalize("Gimme your metal!");

			/* metal mafia goons want your metal, and if you give them something that weighs enough, they may
			 * turn peaceful and leave you alone, but it's only a chance --Amy */

			register struct obj *metalmafiaotmp;

metalmafiaagain:

			metalmafiaotmp = getobj(allnoncount, "offer to the metal mafia");

			if (!metalmafiaotmp) {
				verbalize("I said, gimme your metal!");
				break;
			}
			if (!is_metallic(metalmafiaotmp)) {
				verbalize("Stop bullshitting me, that thing's not metal and therefore it's worthless! Gimme your metal dammit!");
				break;
			}
			if (metalmafiaotmp->oartifact) {
				verbalize("Hey, I ain't touching something that hot!");
				break;
			}
			if (objects[(metalmafiaotmp)->otyp].oc_material == MT_ETHER) {
				verbalize("Stay the hell away with that contaminated garbage!");
				break;
			}
			if (evades_destruction(metalmafiaotmp)) {
				verbalize("No way! There's no fence on the black market who would buy that!");
				break;
			}
			if (metalmafiaotmp->oclass == COIN_CLASS) {
				verbalize("You can't bribe me with money! I need metallic items!");
				break;
			}
			if (metalmafiaotmp->owt < 20) {
				useupall(metalmafiaotmp);
				verbalize("That's scrap metal. I'll take it, but it's not gonna fetch much cash, so... gimme something better!");
				goto metalmafiaagain;
				break;
			} else {
				if (!rn2(5)) {
					int metalmafiaworth = rnd(100);
					if (metalmafiaotmp->oerodeproof) {
						useupall(metalmafiaotmp);
						mtmp->mpeaceful = TRUE;
						verbalize("Damn, that's a truly shiny piece! This will sell for a fortune! I officially consider you a friend now. Many thanks, the metal mafia is very pleased to do business with you!");
					} else if (metalmafiaotmp->owt >= metalmafiaworth) {
						useupall(metalmafiaotmp);
						mtmp->mpeaceful = TRUE;
						verbalize("Alright, that should do it! It's been a pleasure doing business with you. Have a nice day!");
					} else {
						useupall(metalmafiaotmp);
						verbalize("Yeah, that thing's good, it'll sell for some cash, but I still need more. Gimme more metal!");
						goto metalmafiaagain;
					}
					break;
				} else {
					useupall(metalmafiaotmp);
					verbalize("Yeah, that thing's good, it'll sell for some cash, but I still need more. Gimme more metal!");
					goto metalmafiaagain;
					break;
				}
			}

		}
		break;
	case MS_DEEPSTATE:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "We are many. I can be replaced.";
			break;
		}
		if (mtmp->mconf) {
			verbl_msg = "is there some sort of deep anti-state at work?";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Need food.";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Don't say a word.";
		} else if (mtmp->mpeaceful) {
			verbl_msg = "You've never seen me, aight?";
		}
		else verbl_msg = rn2(2) ? "You're dead." : "This is your end.";
		break;
	case MS_PRINCESSLEIA:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/10) {
			verbl_msg = "Noooooo! I may not die! If I die, this kingdom will perish!";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Please repair my broken nail!";
			break;
		}
		if (mtmp->mtame && mtmp->mconf) {
			verbl_msg = "That green elf dude... I think his name was Zelda, right? And he has to defeat that horned demon named Link to save the kingdom of Ganondorf!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Oh, I just remember that I baked a cake, but I forgot where I left it? Can you find it, then we can eat it together?";
			break;
		}
		if (mtmp->mtame && mtmp->bleedout) {
			verbl_msg = "Do something! You see that I'm dripping my blue blood all over the place!";
			break;
		}
		if (mtmp->mtame) verbl_msg = "Together, we can peacefully rule this kingdom.";
		else if (mtmp->mpeaceful) verbl_msg = "Have you seen Han Solo?";
		else if (mtmp->mfrenzied) verbl_msg = "You cannot defeat the princess! My marital arts training will pay off and I'll chase you away!";
		else verbl_msg = "Are you trying to invade my kingdom? Because if you are, I'll have to bludgeon you with my pointy pink girl shoes.";

		break;
	case MS_SISSY:
		verbl_msg = "waaaaaaah...";
		break;
	case MS_SING:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "What, why'd they turn on me? I was supposed to rule them...";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Oh, I'm afraid I've run out of bloodpacks, do you have some for me?";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "It's nice of you to help me force those slaves to clean the girls' shoes.";
		}
		else if (mtmp->mpeaceful) verbl_msg = "Hey, do you want to assist me? I'd be thrilled if you were to step into dog shit with your shoes, because then I can force my slaves to clean your shoes, and when they're done, you'll get them back.";
		else verbl_msg = "You know the rules. You have to clean the dog shit off of every pair of female shoes that I present to you, and you're not allowed to use anything other than your hands and a can of shoe polish.";
		break;
	case MS_BULLETATOR:
		if (mtmp->mconf) {
			verbl_msg = "Hmm... I think the BFG is not an automatic weapon, so using it is okay...";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/10) {
			verbl_msg = "Save me! Please!";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Can you heal me, please?";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Need food!";
			break;
		}
		if (mtmp->mtame) {
			switch (rnd(5)) {
				case 1:
					if (bulletator_allowed(1)) verbl_msg = "Don't worry, you can use automatic firearms.";
					else verbl_msg = "A reminder that you unfortunately can't use automatic firearms.";
					break;
				case 2:
					if (bulletator_allowed(2)) verbl_msg = "Don't worry, you can use the hydra bow.";
					else verbl_msg = "A reminder that you unfortunately can't use the hydra bow.";
					break;
				case 3:
					if (bulletator_allowed(3)) verbl_msg = "Don't worry, you can use the demon crossbow.";
					else verbl_msg = "A reminder that you unfortunately can't use the demon crossbow.";
					break;
				case 4:
					if (bulletator_allowed(4)) verbl_msg = "Don't worry, you can use the catapult.";
					else verbl_msg = "A reminder that you unfortunately can't use the catapult.";
					break;
				case 5:
					if (bulletator_allowed(5)) verbl_msg = "Don't worry, you can use flamethrowers.";
					else verbl_msg = "A reminder that you unfortunately can't use flamethrowers.";
					break;
			}
		} else if (mtmp->mpeaceful) {
			switch (rnd(5)) {
				case 1:
					if (bulletator_allowed(1)) verbl_msg = "You may use automatic firearms.";
					else verbl_msg = "Remember, you're not allowed to use automatic firearms!";
					break;
				case 2:
					if (bulletator_allowed(2)) verbl_msg = "You may use the hydra bow.";
					else verbl_msg = "Remember, you're not allowed to use the hydra bow!";
					break;
				case 3:
					if (bulletator_allowed(3)) verbl_msg = "You may use the demon crossbow.";
					else verbl_msg = "Remember, you're not allowed to use the demon crossbow!";
					break;
				case 4:
					if (bulletator_allowed(4)) verbl_msg = "You may use the catapult.";
					else verbl_msg = "Remember, you're not allowed to use the catapult!";
					break;
				case 5:
					if (bulletator_allowed(5)) verbl_msg = "You may use flamethrowers.";
					else verbl_msg = "Remember, you're not allowed to use flamethrowers!";
					break;
			}
		} else {
			verbl_msg = "Now you'll be punished for using automatic weapons.";
			break;
		}
		break;
	case MS_FLUIDATOR:
		if (mtmp->mconf) {
			verbl_msg = "What are we to now make?";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/10) {
			verbl_msg = "I am equal dead! Assistance!";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I must take some welfare watering places!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "I custom which to eat!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = u.fluidatorwantedlevel ? "Are you glad, with me to allies its?" : "You is my buddy!";
		}
		else if (mtmp->mpeaceful) {
			verbl_msg = u.fluidatorwantedlevel ? "Now you are however to!" : "Is greeted, unknown potato.";
			if (u.fluidatorwantedlevel) mtmp->mpeaceful = FALSE;
		}
		else verbl_msg = u.fluidatorwantedlevel ? "Now you can however which experience, you hybrid!" : "We are attacked becoming!";
		break;
	case MS_POMPEJI:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Pom... peji...";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Pompeji-pompeji!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Poooooom peeeeeeee jiiiiiiii!";
		}
		else if (mtmp->mpeaceful) verbl_msg = "Pompeji.";
		else verbl_msg = "Pompe-e-ji!";
		break;
	case MS_ALLA:
		if (!mtmp->mtame && !mtmp->mpeaceful) verbl_msg = "you will lose";
		break;
	case MS_POKEDEX:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Argh, I bit off more than I can chew! That monster is too strong!";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "All that pokemon hunting has caused me to go hungry! I got to eat some berries!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Let's look for new pokemon in that grass patch over there.";
		}
		else if (mtmp->mpeaceful) verbl_msg = "Hmm, my PokeWalker(TM) said that a couple new pokemon species can be encountered on this level...";
		else verbl_msg = "Go away, this is my pokemon hunting ground! I'm not letting you catch the shinies here, you hear?";
		break;
	case MS_BOT:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Calculating chance of success... 25%%.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Hungry! Let's go to the bar!";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "I protect and serve you.";
		}
		else if (mtmp->mpeaceful) verbl_msg = "Another great day in the history of our army!";
		else verbl_msg = "Please don't be surprised if I kill you. That's just my programming.";
		break;
	case MS_APOC:
		if (mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Just fucking kill me now.";
			break;
		}
		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Of course, the food supply chains have broken down too...";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "This world is headed towards destruction.";
		}
		else if (mtmp->mpeaceful) verbl_msg = "Did you know that the end of the world is coming?";
		else verbl_msg = "Oh, don't mind me, I know that we're all gonna die anyway.";
		break;

	case MS_GIBBERISH:
		pline("%s", generate_garbage_string());
		break;
	case MS_CASINO:
		{
			boolean willdisappear = 0;
			int returnvalue = 0;
			verbalize("Hello sir, welcome to 'Fortune Cookie' casino!"); /* from Elona, where they always say 'sir' even if you're female */
			returnvalue = play_blackjack(FALSE);
			if (returnvalue == 2) willdisappear = 1;
			if (returnvalue >= 1 && (mtmp->data == &mons[PM_CROUPIER] || mtmp->data == &mons[PM_MASTER_CROUPIER] || mtmp->data == &mons[PM_EXPERIENCED_CROUPIER] || mtmp->data == &mons[PM_EXCEPTIONAL_CROUPIER] || mtmp->data == &mons[PM_ELITE_CROUPIER]) ) {
			/* you played, so the croupier should become peaceful; other MS_CASINO monsters don't care */
				mtmp->mfrenzied = FALSE;
				mtmp->mpeaceful = TRUE;
			}

			if (willdisappear) {
				mongone(mtmp);
				verbalize("Sorry sir, I've been ordered to return to Las Venturas! Thanks for playing!");
				return(1);
			}
			verbalize("Remember, if you win, your reward will be dropped on the ground beneath you.");
		}
		break;
	case MS_COW:
		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline_msg = "breathes heavily.";
			else if (mtmp->mtame && hastoeat && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime)
				pline_msg = "makes a distressed bellow.";
			else
				pline_msg = "bellows.";
		}
		else pline_msg = "bellows.";
		break;
	case MS_BONES:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			pline("%s's bones seem about to break!", Monnam(mtmp));
		} else {
			pline(FunnyHallu ? "%s plays the xylophone!" : "%s rattles noisily.", Monnam(mtmp));
			You("freeze for a moment.");
			nomul(-2, "scared by rattling", TRUE);
			nomovemsg = 0;
		}
		break;
	case MS_LAUGH:

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/4) {
			verbl_msg = "Hahahahahahahaaaaaaaaaa!"; /* Super Mario 64 :P */
			break;
		}

	    {
		static const char * const laugh_msg[4] = {
		    "giggles.", "chuckles.", "snickers.", "laughs.",
		};
		pline_msg = laugh_msg[rn2(4)];
	    }
	    break;
	case MS_MUMBLE:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			pline_msg = "mumbles faintly.";
		} else pline_msg = "mumbles incomprehensibly.";
		break;
	case MS_DJINNI:

		if (mtmp->data == &mons[PM_POEZ_PRESIDENT] && u.ualign.type == A_CHAOTIC && !mtmp->mtame && !mtmp->mfrenzied) {

			mtmp->mpeaceful = TRUE;
			verbalize("Hey %s! I see you're a POEZ member! I'm gonna join your team!", playeraliasname);

			struct monst *ppresident;
			ppresident = tamedog(mtmp, (struct obj *) 0, TRUE);
			if (ppresident) mtmp = ppresident;
			if (!mtmp) impossible("poez president was tamed but doesn't exist now??");
			break;

		}
		if (mtmp->data == &mons[PM_GIRL_OUTSIDE_GANG] && u.ualign.type == A_LAWFUL && !mtmp->mtame && !mtmp->mfrenzied) {

			mtmp->mpeaceful = TRUE;
			verbalize("%s! It is good that you have come! You see, I'm one of the few girls that resisted joining those filthy youth gangs. Since you despise them as much as I do, I decide to join your cause.", playeraliasname);

			struct monst *ppresident;
			ppresident = tamedog(mtmp, (struct obj *) 0, TRUE);
			if (ppresident) mtmp = ppresident;
			if (!mtmp) impossible("girl outside gang was tamed but doesn't exist now??");
			break;

		}
		if (mtmp->data == &mons[PM_OCCASIONAL_FRIEND] && u.ualign.type == A_NEUTRAL && !mtmp->mtame && !mtmp->mfrenzied) {

			mtmp->mpeaceful = TRUE;
			verbalize("Yeeeeah! %s, let's mow down all our opponents! I'll gladly join your team, and then we'll be unstoppable!", playeraliasname);

			struct monst *ppresident;
			ppresident = tamedog(mtmp, (struct obj *) 0, TRUE);
			if (ppresident) mtmp = ppresident;
			if (!mtmp) impossible("girl outside gang was tamed but doesn't exist now??");
			break;

		}

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

		if (mtmp->ruinedshoe) {
			pline_msg = "is unresponsive.";
		} else if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			if (u.usteed == mtmp)
				verbl_msg = "Why do you wear us out like that? Can't you, like, try to avoid the mud and dogshit?";
			else
				verbl_msg = "Nooooo! We're nearing the end of our lifetime! Can you please take us to a shoe repair shop?";
		} else if (mtmp->mfrenzied) {
			if (u.usteed == mtmp)
				verbl_msg = "Wear us if you want, but take us off and we'll scratch you to death and make your blood squirt in all directions.";
			else
				verbl_msg = "We'll scratch very bloody wounds on your legs and kill you.";
		} else if (mtmp->mtame) {
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

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I sure hope you'll protect me with your life.";
			break;
		}
		if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Dinner time! Can you take me to a noble restaurant please?";
			break;
		}

		if (mtmp->data == &mons[PM_MANDARUCK] && !mtmp->mfrenzied) {

			if (!mtmp->mpeaceful && !mtmp->mfrenzied) mtmp->mpeaceful = TRUE;

			static const char *hint_msgs[] = {
				"Always guard your purse from thieves.",
				"Be wary of Jessica, she might not look like it but she'll steal your money repeatedly.",
				"Careful when dealing with the mysterious 2! They're known to sneak into your backpack as blind passengers!",
				"The journalist Rhea wants to open your backpack and film it at the same time, which will cause you to appear in the newspaper in a rather negative light...",
				"Don't react to Mariya when she clicks open all the drawers of your backpack. She will seduce you if you confront her, and then make you unconscious.",
				"If your backpack is suddenly much heavier with no warning, it means that Wendy jumped on top of it and you're dragging her weight.",
				"There's a sexy princess called Cathlette, but she's really evil. God help you if she decides to make you a target.",
				"Watch out for Lisa's tricks. She will divert you by talking about boring stuff.",
				"Make sure you don't turn your back on May-Britt, or she might end up breaking your backpack's zippers!",
			};
			verbalize("%s", hint_msgs[rn2(SIZE(hint_msgs))]);

			break;
		}

		if (mtmp->data == &mons[PM_TEARFRAN] && !mtmp->mfrenzied) {

			if (mtmp->mpeaceful) {
				verbalize("*sob*... there are no good days in this life at all, there are only terrible ones... *sob*");
			} else if (u.ualign.record > 0) {
				mtmp->mpeaceful = TRUE;
				verbalize("Oh... I see you care for me... please, protect me from harm...");
				pline("You can see tears running down %s's cheeks, and try your best to make her feel better.", mon_nam(mtmp));
			} else if (u.ualign.record < 0) {
				mtmp->mfrenzied = TRUE;
				monflee(mtmp, 100, TRUE, TRUE, FALSE);
				verbalize("*sob* *sob* You're just as evil as all the other people... please end this life of misery and agony already...");
			} else verbalize("Oh... can I talk to you?");
			break;

		}

		if (mtmp->data == &mons[PM_MELISTRIDE] && PlayerInHighHeels && !mtmp->mfrenzied) {
			mtmp->mpeaceful = TRUE;
			verbalize("Wow, I really like your heels, %s!", playeraliasname);

			if (ACURR(A_CHA) >= 18 && !mtmp->mtame) {
				struct monst *melistride;
				melistride = tamedog(mtmp, (struct obj *) 0, TRUE);
				if (melistride) mtmp = melistride;
				if (mtmp) verbalize("You know what? I'll join you. We're in this together, after all. Let's complete our tasks in a team effort!");
				else impossible("melistride was tamed but doesn't exist now??");
			}
			break;
		}

		if (mtmp->data == &mons[PM_NASTROSCHA] && (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) && !mtmp->mfrenzied) {
			mtmp->mpeaceful = TRUE;
			pline("With your velvet gloves, you caress %s...", mon_nam(mtmp));

			if (!mtmp->mtame) {
				struct monst *nastroscha;
				nastroscha = tamedog(mtmp, (struct obj *) 0, TRUE);
				if (nastroscha) mtmp = nastroscha;
				if (!mtmp) impossible("nastroscha was tamed but doesn't exist now??");
				else pline("Suddenly %s is absolutely in love with you!", mon_nam(mtmp));
			} else verbalize("Ooh %s, I really like those velvety soft gloves...", playeraliasname);
			break;
		}

		if (mtmp->data == &mons[PM_WUXTINA] && !mtmp->mcan) {
			verbalize("Get the fuck out of here! I'm the only one who may use the analysis port now and if you want to use it too you have to stand in the fucking line!");

			if (yn("Do you obey the command that the wuxtina uttered with her bitchy voice?") != 'y') {
				pline("%s suddenly sprays her scentful perfume right into your %s.", Monnam(mtmp), body_part(FACE));
				u.cnd_perfumecount++;
				if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
				if (Role_if(PM_HUSSY)) {
					You("joyously inhale %s's scentful perfume. It's very soothing.", mon_nam(mtmp));
				} else {
					pline("Inhaling %s's scentful perfume is not the brightest idea. But in this case you didn't have a choice...", mon_nam(mtmp));

					if (rn2(10) && uarmh && itemhasappearance(uarmh, APP_GAS_MASK) ) {
						pline("But the gas mask protects you from the effects.");
					} else if (rn2(5) && uarmf && uarmf->oartifact == ART_CLAUDIA_S_SELF_WILL) {
						pline("But you actually enjoy the lovely scent.");
					} else if (rn2(20) && uwep && uwep->oartifact == ART_HIGH_ORIENTAL_PRAISE) {
						pline("But you actually enjoy the lovely scent.");
					} else if (rn2(10) && uarmf && uarmf->oartifact == ART_KATHARINA_S_SIGH) {
						pline("But you actually enjoy the lovely scent.");
					} else {
						badeffect();
						if (rn2(2)) increasesanity(rnz(20 + mtmp->m_lev));
					}
				}

			} else {

				mtmp->mcan = TRUE;

				if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed)) ) { pline("You shudder for a moment."); (void) safe_teleds_normalterrain(FALSE); break;}
				if (playerlevelportdisabled()) {
					pline("For some reason you resist the banishment!");
					break;
				}

				make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

				if (!u.banishmentbeam) {
					u.banishmentbeam = 1;
					nomul(-2, "being banished", FALSE);
				}
				verbalize(!rn2(4) ? "Don't molest me ever again. If you do, I'll kick your hands bloodily with my dancing shoes." : !rn2(3) ? "Good riddance. Now get lost and annoy other girls instead." : !rn2(2) ? "Get lost before I start burning your arm with my candle." : "If you ever come back, I'm gonna extinguish my cigarette right in your face.");
				if (FunnyHallu) pline("(She obviously likes assholes, you say to yourself.)");

			}

			break;
		}

		u.cnd_perfumecount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
		if (Role_if(PM_HUSSY)) {
			You("joyously inhale %s's scentful perfume. It's very soothing.", mon_nam(mtmp));
		} else {
			pline("Inhaling %s's scentful perfume is not the brightest idea.", mon_nam(mtmp));

			if (rn2(10) && uarmh && itemhasappearance(uarmh, APP_GAS_MASK) ) {
				pline("But the gas mask protects you from the effects.");
			} else if (rn2(5) && uarmf && uarmf->oartifact == ART_CLAUDIA_S_SELF_WILL) {
				pline("But you actually enjoy the lovely scent.");
			} else if (rn2(20) && uwep && uwep->oartifact == ART_HIGH_ORIENTAL_PRAISE) {
				pline("But you actually enjoy the lovely scent.");
			} else if (rn2(10) && uarmf && uarmf->oartifact == ART_KATHARINA_S_SIGH) {
				pline("But you actually enjoy the lovely scent.");
			} else {
				badeffect();
				if (rn2(2)) increasesanity(rnz(20 + mtmp->m_lev));
			}
		}

	    break;

	case MS_CONVERT:

		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				verbl_msg = "Allah bana bunu yapan kafirleri cezalandiracak!";
			else if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime)
				verbl_msg = "Baska bir doener kebap yemem gerek!";
			else if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped)
				verbl_msg = "Simdi kime dua etmem gerekiyor? Jahwe? Hayir, hatirliyorum, bu Buda!";
			else {
				if (flags.female) verbl_msg = "Selamlar, kiz kardes.";
				else verbl_msg = "Selamlar, erkek kardes.";
			}
			break;
		} else if (mtmp->mpeaceful) {
			if (flags.female) verbl_msg = "Selamlar, kiz kardes.";
			else verbl_msg = "Selamlar, erkek kardes.";

			break;

		} else if (Race_if(PM_TURMENE) || Race_if(PM_HC_ALIEN) || Race_if(PM_SLYER_ALIEN) || (have_hardcoreaddiction() && rn2(10)) || (uarmh && uarmh->oartifact == ART_JAMILA_S_BELIEF)) {

			break;

		} else {

			conversionsermon();
			u.cnd_conversioncount++;
			if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

		}

		if (uarmf && uarmf->oartifact == ART_RUEA_S_FAILED_CONVERSION && rn2(20)) break;
		if (uwep && uwep->oartifact == ART_CRONVERT && rn2(10)) break;
		if (uarmf && uarmf->oartifact == ART_SUSCHEL_KUNA) break;
		if (uarmf && uarmf->oartifact == ART_EXHAUST_DAMAGE) break;
		if (uarmf && uarmf->oartifact == ART_SUNA_S_CONTROVERSY_MATTER) break;

		if (u.ualign.record < -20 && !rn2(Race_if(PM_KORONST) ? 10 : 100) && (sgn(mtmp->data->maligntyp) != u.ualign.type) ) { /* You have been converted! */

			if(u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL] && mtmp->data->maligntyp != A_NONE) {
				You("have a strong feeling that %s is angry...", u_gname());
				pline("%s accepts your allegiance.", align_gname(sgn(mtmp->data->maligntyp)));

				/* The player wears a helm of opposite alignment? */
				if (uarmh && uarmh->otyp == HELM_OF_OPPOSITE_ALIGNMENT)
					u.ualignbase[A_CURRENT] = sgn(mtmp->data->maligntyp);
				else
					u.ualign.type = u.ualignbase[A_CURRENT] = sgn(mtmp->data->maligntyp);
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
				godvoice(sgn(mtmp->data->maligntyp), "Suffer, infidel!");
				change_luck(-5);
				(void) adjattrib(A_WIS, -2, TRUE, TRUE);
				angrygods(sgn(mtmp->data->maligntyp));

			}


		}
		adjalign(-(mtmp->m_lev + 1));

		if (!rn2(10)) badeffect();

	    break;

	case MS_HCALIEN:

		if (mtmp->mtame) {
			if (mtmp->mhp < mtmp->mhpmax/3)
				pline("%s points a %s at %s heart.", Monnam(mtmp), mbodypart(mtmp, FINGER), mhis(mtmp));
			else if (mtmp->mtame && hastoeat && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime)
				pline("%s grunts long and deeply.", Monnam(mtmp));
			else if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped)
				pline("%s spins %s %s wildly.", Monnam(mtmp), mhis(mtmp), mbodypart(mtmp, HEAD));
			else if (Race_if(PM_TURMENE) || Race_if(PM_EGYMID) || Race_if(PM_PERVERT) || Race_if(PM_HC_ALIEN) || Race_if(PM_SLYER_ALIEN))
				pline("%s nods approvingly.", Monnam(mtmp));
			else
				pline("%s makes an approving gesture, but doesn't deign to talk to you.", Monnam(mtmp));
			break;
		}

		if (mtmp->mpeaceful) {

			if (Race_if(PM_TURMENE) || Race_if(PM_EGYMID) || Race_if(PM_PERVERT) || Race_if(PM_HC_ALIEN) || Race_if(PM_SLYER_ALIEN)) pline("%s looks at you for a moment.", Monnam(mtmp));
			else pline("%s does not deign to look at you, let alone talk to you.", Monnam(mtmp));
			break;

		}

		if (Race_if(PM_TURMENE) || Race_if(PM_HC_ALIEN) || Race_if(PM_SLYER_ALIEN) || (have_hardcoreaddiction() && rn2(4)) || (uarmh && uarmh->oartifact == ART_JAMILA_S_BELIEF)) {
			break;
		}

		wouwoutaunt();
		u.cnd_wouwoucount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();

		badeffect();

	    if (!obsidianprotection()) switch (rn2(17)) {
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
		if (!rn2(20)) increasesanity(rnd(10 + (mtmp->m_lev * 2) ));

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
	case MS_NASTYTRAP:
	case MS_GRAKA:
	case MS_JAM:
	case MS_HUMANOID:
	case MS_SPEEDBUG:
	case MS_LOWPRIORITY:
	case MS_AETHERIC:
	case MS_TRIP:
	case MS_FEMI:
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
	    else if (mtmp->mtame && hastoeat && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime)
		verbl_msg = "I'm hungry.";
	    /* Specific monsters' interests */
	    else if (is_elf(ptr))
		pline_msg = "curses orcs.";
	    else if (is_dwarf(ptr))
		pline_msg = "talks about mining.";
	    else if (likes_magic(ptr) && (monsndx(ptr) != PM_KLAPPTNIX) && rn2(5) )
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
		case PM_EMERA:
		    verbl_msg = "HATE! HATE! HATE!";
		    break;
		case PM_MECHTNED:
		    verbl_msg = "Mechtned.";
		    break;
		case PM_KLAPPTNIX:
		    verbl_msg = "Klapptnix.";
		    break;
		case PM_UNLUCKY_GIRL:
		    verbl_msg = "Damn those people who constantly make me slip into dog shit with my zipper boots!";
		    break;
		case PM_FJORDE:
		    verbl_msg = "I can't help it, I really consider fish to be the most beautiful creatures in existence.";
		    break;
		case PM_PRACTICANT:
		    verbl_msg = "I'm gonna introduce all those bastard assistants to my battering ram.";
		    break;
		case PM_BUTT_LOVER:
		    verbl_msg = "Mmmmmmmmmmm... lovely, soft butt cheeks...";
		    break;
		case PM_SHOE_FETISHIST:
		    verbl_msg = "I absolutely looooooooove high heels!";
		    break;
		case PM_SECRET_ADVICE_MEMBER:
		    verbl_msg = "Careful. I'm a member of the Secret Advice.";
		    break;
		case PM_PREVERSIONER:
		    verbl_msg = "Who the hell would design a game with a reset button that has no confirmation and simply erases your current game without giving you a chance to save???";
		    break;
		case PM_GENDERSTARIST:
		    verbl_msg = !rn2(3) ? "Sehr geehrte Buergerinnen und Buerger, die Schuelerinnen und Schueler sind unter dem gesetzlich geforderten Niveau..." : !rn2(2) ? "Sehr geehrte Buergerlnnen, die Schuelerlnnen fallen beim Pisa-Test zu oft durch..." : "Sehr geehrte Buerger*innen, die Schueler*innen weigern sich, den Gender*stern* zu* benutz*en*/*in*...*";
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
		case PM_ALGONQUIN_BUSINESSLADY:
		    verbl_msg = "I'm very busy! Please don't disturb me!";
		    break;
		case PM_GOFF:
		    verbl_msg = "britny is a prep!!!111";
		    break;
		case PM_UNBELIEVER:
		    verbl_msg = "Gods are nonexistant. People should stop believing in them.";
		    break;
		case PM_HUSSY:
		    verbl_msg = "Anna is gonna finish off that Walt bumbler.";
		    break;
		case PM_POKEMON:
		    verbl_msg = "Little strawberry me baby!";
		    break;
		case PM_GANG_SCHOLAR:
		    verbl_msg = "I'm constantly being bullied by the Bang Gang and Anna's hussies...";
		    break;
		case PM_WALSCHOLAR:
		    verbl_msg = "Hidden diamonds are like tree bark.";
		    break;
		case PM_SINGSLAVE:
		    verbl_msg = "That Sing bitch keeps forcing me to clean the dog shit from all the girls' shoes! Please rescue me!";
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
			getlin("Are you homosexual? [y/yes/no] (If you answer no, you're heterosexual.)", buf);
			(void) lcase (buf);
			if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) u.homosexual = 2;
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
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/10) {
			verbalize("This is unit %d to headquarters. Send reinforcements! Repeat! Send reinforcements!", mtmp->m_id); /* inspired by the anachronononononononononononononaut NPCs in dnethack, thanks Chris --Amy */
		} else if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbalize("Officer in need of a medic!");
		} else if (mtmp->mpeaceful)
			verbalize("Just the facts, %s.", flags.female ? "Ma'am" : "Sir");
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
            	    if (!mtmp->mfrenzied) mtmp->mpeaceful = 1;
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

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) verbalize("Doggone it!");

	    break;
	case MS_GYPSY:	/* KMH */
		if (mtmp->mpeaceful) {
			gypsy_chat(mtmp);
			break;
		}
		/* fall through */
	case MS_SPELL:

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			pline_msg = "is trying to cast a healing spell.";
			break;
		}

		/* deliberately vague, since it's not actually casting any spell */
		pline_msg = "seems to mutter a cantrip.";
		break;
	case MS_NURSE: /* services added by Amy - maybe we should make this into a menu instead of y/n prompts? */

	    {
		int nursesanitycost = (u.usanity * 10); /* fixed cost */

		int nursedecontcost = u.nursedecontamcost; /* goes up every time you purchase it */

		int nursehpcost = u.nurseextracost; /* goes up every time you purchase it */

		int nursesymbiotecost = u.nursesymbiotecost; /* goes up every time you purchase it */

		int nurseshutdowncost = u.nurseshutdowncost;

		if (Upolyd) nursehpcost /= 5;

			winid tmpwin;
			anything any;
			menu_item *selected;
			int n;

			if (!mtmp->nurse_extrahealth && !mtmp->nurse_decontaminate && !mtmp->nurse_healing && !mtmp->nurse_curesickness && !mtmp->nurse_curesliming && !mtmp->nurse_curesanity && !mtmp->nurse_medicalsupplies && !mtmp->nurse_purchasedrugs && !mtmp->nurse_obtainsymbiote && !mtmp->nurse_fixsymbiote && !mtmp->nurse_shutdownsymbiote && !mtmp->nurse_restoration && !mtmp->nurse_vaccine && !mtmp->nurse_alla) {
				verbalize("Sorry. I'm all out of services.");
				goto noservices;
			}

			any.a_void = 0;         /* zero out all bits */
			tmpwin = create_nhwindow(NHW_MENU);
			start_menu(tmpwin);
			any.a_int = 1;
			if (mtmp->nurse_extrahealth) add_menu(tmpwin, NO_GLYPH, &any , 'e', 0, ATR_NONE, "Extra Health", MENU_UNSELECTED);
			any.a_int = 2;
			if (mtmp->nurse_decontaminate) add_menu(tmpwin, NO_GLYPH, &any , 'd', 0, ATR_NONE, "Decontaminate", MENU_UNSELECTED);
			any.a_int = 3;
			if (mtmp->nurse_healing) add_menu(tmpwin, NO_GLYPH, &any , 'h', 0, ATR_NONE, "Healing", MENU_UNSELECTED);
			any.a_int = 4;
			if (mtmp->nurse_curesickness) add_menu(tmpwin, NO_GLYPH, &any , 'c', 0, ATR_NONE, "Cure Sickness", MENU_UNSELECTED);
			any.a_int = 5;
			if (mtmp->nurse_curesliming) add_menu(tmpwin, NO_GLYPH, &any , 'l', 0, ATR_NONE, "Cure Sliming", MENU_UNSELECTED);
			any.a_int = 6;
			if (mtmp->nurse_curesanity) add_menu(tmpwin, NO_GLYPH, &any , 'i', 0, ATR_NONE, "Cure Sanity", MENU_UNSELECTED);
			any.a_int = 7;
			if (mtmp->nurse_medicalsupplies) add_menu(tmpwin, NO_GLYPH, &any , 'm', 0, ATR_NONE, "Medical Supplies", MENU_UNSELECTED);
			any.a_int = 8;
			if (mtmp->nurse_purchasedrugs) add_menu(tmpwin, NO_GLYPH, &any , 'p', 0, ATR_NONE, "Purchase Drugs", MENU_UNSELECTED);
			any.a_int = 9;
			if (mtmp->nurse_obtainsymbiote) add_menu(tmpwin, NO_GLYPH, &any , 'o', 0, ATR_NONE, "Obtain Symbiote", MENU_UNSELECTED);
			any.a_int = 10;
			if (mtmp->nurse_fixsymbiote) add_menu(tmpwin, NO_GLYPH, &any , 'f', 0, ATR_NONE, "Fix Symbiote", MENU_UNSELECTED);
			any.a_int = 11;
			if (mtmp->nurse_shutdownsymbiote) add_menu(tmpwin, NO_GLYPH, &any , 's', 0, ATR_NONE, "Shutdown Symbiote", MENU_UNSELECTED);
			any.a_int = 12;
			if (mtmp->nurse_restoration) add_menu(tmpwin, NO_GLYPH, &any , 'r', 0, ATR_NONE, "Restoration", MENU_UNSELECTED);
			any.a_int = 13;
			if (mtmp->nurse_vaccine) add_menu(tmpwin, NO_GLYPH, &any , 'v', 0, ATR_NONE, "COVID-19 Vaccine", MENU_UNSELECTED);
			any.a_int = 14;
			if (mtmp->nurse_alla) add_menu(tmpwin, NO_GLYPH, &any , 'v', 0, ATR_NONE, "Gain Alla", MENU_UNSELECTED);

			end_menu(tmpwin, "Services Available:");
			n = select_menu(tmpwin, PICK_ONE, &selected);
			destroy_nhwindow(tmpwin);

			if (n > 0) {
				switch (selected[0].item.a_int) {
					case 1:
						if (u.ugold >= nursehpcost) {
							verbalize("I can inject extra health into you for %d dollars if you want.", nursehpcost);
							if (yn("Accept the offer?") == 'y') {
								verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
								if (!rn2(10)) mtmp->nurse_extrahealth = 0;
								u.ugold -= nursehpcost;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								if (!Upolyd) u.uhpmax++;
								else u.mhmax++;
								use_skill(P_SQUEAKING, 1);
								if (uinsymbiosis) {
									u.usymbiote.mhpmax++;
									maybe_evolve_symbiote();
									if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
								}
								u.nurseextracost += 50;
								if (u.nurseextracost < 1000) u.nurseextracost = 1000; /* fail safe */
								break;
							}
						} else verbalize("Sorry, extra health costs %d dollars.", nursehpcost);

						break;
					case 2:
						if (u.ugold >= nursedecontcost && u.contamination) {
							verbalize("I can decontaminate you for %d dollars if you want.", nursedecontcost);
							if (yn("Accept the offer?") == 'y') {
								verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
								if (!rn2(5)) mtmp->nurse_decontaminate = 0;
								u.ugold -= nursedecontcost;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								decontaminate(u.contamination);
								pline("Now you don't have the %d gold pieces any longer.", nursedecontcost);
								if (FunnyHallu) pline("You offer a 'thank you' to Captain Obvious.");
								u.nursedecontamcost += 500;
								if (u.nursedecontamcost < 1000) u.nursedecontamcost = 1000; /* fail safe */
								break;
							}
						} else if (!u.contamination) verbalize("Huh? You don't need to do that.");
						else verbalize("Sorry, decontamination costs %d dollars.", nursedecontcost);

						break;
					case 3:

						if (u.uhp == u.uhpmax && (!Upolyd || (Upolyd && u.mh == u.mhmax))) {
							verbalize("Apart from potential mental disorders everything's fine with you. There's no need for me to waste my medical supplies on you.");
							break;
						}
						if (u.ugold >= 500) {
							verbalize("Sure thing, I can heal you for 500 dollars.");
							if (yn("Accept the offer?") == 'y') {
								verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
								if (!rn2(50)) mtmp->nurse_healing = 0;
								u.ugold -= 500;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								u.uhp += 50;
								if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
								if (Upolyd) {
									u.mh += 50;
									if (u.mh > u.mhmax) u.mh = u.mhmax;
								}
								break;
							}
						} else verbalize("Sorry, healing costs 500 dollars.");

						break;
					case 4:
						if (!Sick) {
							verbalize("Don't call in sick when you aren't! Come back to me when you actually have a sickness that needs curing.");
							break;
						}
						if (u.ugold >= 5000) {
							verbalize("It is wise of you to come to see a doctor. For only 5000 dollars, I can heal you.");
							if (yn("Accept the offer?") == 'y') {
								verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
								if (!rn2(15)) mtmp->nurse_curesickness = 0;
								u.ugold -= 5000;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								You_feel("better.");
								make_sick(0L, (char *) 0, FALSE, SICK_ALL);
								break;
							}
						} else verbalize("Sorry, the cure for sickness costs 5000 dollars.");

						break;
					case 5:
						if (!Slimed) {
							verbalize("Do you see any slime on your body? No? Well, me neither. Come back when you're actually slimed.");
							break;
						}
						if (u.ugold >= 10000) {
							verbalize("Eek! Yeah that would normally require a medical doctor, but if you have 10000 dollars I can give you something that should hopefully cure the sliming.");
							if (yn("Accept the offer?") == 'y') {
								verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
								if (!rn2(12)) mtmp->nurse_curesliming = 0;
								u.ugold -= 10000;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								FunnyHallu ? pline("The rancid goo is gone! Yay!") : pline_The("slime disappears.");
								Slimed = 0;
								flags.botl = 1;
								delayed_killer = 0;
								break;
							}
						} else verbalize("Sorry, the cure for sliming costs 10000 dollars.");

						break;
					case 6:
						if (u.usanity && (u.ugold >= nursesanitycost)) {
							verbalize("I can cure your sanity for %d dollars if you want.", nursesanitycost);
							if (yn("Accept the offer?") == 'y') {
								verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
								if (!rn2(5)) mtmp->nurse_curesanity = 0;
								u.ugold -= nursesanitycost;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								reducesanity(u.usanity);
								break;
							}
						} else if (!u.usanity) verbalize("You have no sanity that could be cured! Be glad, being insane is the preferred state of mind you want in this dungeon!");
						else verbalize("Sorry, the cure for sanity costs %d dollars.", nursesanitycost);

						break;
					case 7:
						if (u.ugold >= 10000) {
							verbalize("Ah, you look like a walking coinpurse. Sure, you can have medical supplies, but they come at a price. For 10000 dollars I can sell you a medical kit.");
							if (yn("Accept the offer?") == 'y') {
								u.ugold -= 10000;
								if (!rn2(10)) mtmp->nurse_medicalsupplies = 0;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								struct obj *medkit;
								medkit = mksobj(MEDICAL_KIT, TRUE, FALSE, FALSE);
								verbalize(medkit ? "A pleasure doing business with you. The medical kit is waiting on the ground below you." : "Whoops. It seems that I don't have supplies for you right now, but for technical reasons I can't give you a refund. Sorry.");
								use_skill(P_SQUEAKING, 2);
								if (medkit) {
									medkit->quan = 1;
									medkit->known = medkit->dknown = medkit->bknown = medkit->rknown = 1;
									medkit->owt = weight(medkit);
									dropy(medkit);
									stackobj(medkit);
								}
							}
						} else verbalize("Sorry, medical supplies cost 10000 dollars.");
						break;
					case 8:
						if (u.ugold >= 2000) {
							verbalize("Hmm, I think I can give you a little something, but I need 2000 dollars to cover up expenses.");
							if (yn("Accept the offer?") == 'y') {
								u.ugold -= 2000;
								if (!rn2(20)) mtmp->nurse_purchasedrugs = 0;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								struct obj *medkit;
								medkit = mksobj(rn2(2) ? MUSHROOM : PILL, TRUE, FALSE, FALSE);
								verbalize(medkit ? "Here, your stuff is on the ground. Have fun, but remember: if you call the cops, I'll send my assassins after you!" : "Oh, sorry, I don't have anything for you... but thanks for the money, sucker!");
								use_skill(P_SQUEAKING, 1);
								if (medkit) {
									medkit->quan = 1;
									medkit->known = medkit->dknown = medkit->bknown = medkit->rknown = 1;
									medkit->owt = weight(medkit);
									dropy(medkit);
									stackobj(medkit);
								}

							}
						} else verbalize("What? I'm not a dealer! But I might change my mind if you can bring at least 2000 dollars.");
						break;
					case 9:
						if (uinsymbiosis && u.usymbiote.cursed) {
							verbalize("Your current symbiote is cursed! I can't replace it with a new one unless you fix that problem first.");
							break;
						}

						if (u.ugold < nursesymbiotecost) {
							verbalize("Sorry, a symbiote costs %d dollars.", nursesymbiotecost);
							break;
						}
						if (uinsymbiosis) verbalize("This will replace your current symbiote. By continuing, I assume that you agree to discard your symbiote for a random new one.");

						if (u.ugold >= nursesymbiotecost) {
							verbalize("A symbiote injection costs %d dollars. According to current Yendorian law, I'm forced to inform you that the symbiote you receive is random and may or may not be useful to you. By continuing, you confirm that you know of this risk and will not hold me responsible if you don't like the result.", nursesymbiotecost);
							if (yn("Accept the offer?") == 'y') {
								verbalize("Alright, here's your injection.");
								if (!rn2(10)) mtmp->nurse_obtainsymbiote = 0;
								u.ugold -= nursesymbiotecost;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								getrandomsymbiote(FALSE, FALSE);

								u.nursesymbiotecost += 5000;
								if (u.nursesymbiotecost < 10000) u.nursesymbiotecost = 10000; /* fail safe */
								break;
							}
						}

						break;
					case 10:
						if (!uinsymbiosis) {
							verbalize("What? You don't have a symbiote! Sorry, but I can't fix something that doesn't exist!");
							break;
						}
						if (!u.usymbiote.cursed && u.usymbiote.mhp == u.usymbiote.mhpmax) {
							verbalize("Your symbiote is in perfect condition, so you don't need my services.");
							break;
						}
						int symhealcost = 0;

						/* The nurse will always remove all the curses. So in order to not make it too easy
						 * to get rid of the nastier curses, we have to add to the cost of the service,
						 * scaling with the severity of cursedness of your symbiote --Amy */
						if (u.usymbiote.cursed) symhealcost += 2000;
						if (u.usymbiote.hvycurse) symhealcost += 3000;
						if (u.usymbiote.prmcurse) symhealcost += 15000;
						if (u.usymbiote.evilcurse) symhealcost += 480000;
						if (u.usymbiote.morgcurse) symhealcost += 480000;
						if (u.usymbiote.bbcurse) symhealcost += 480000;
						if (u.usymbiote.stckcurse) symhealcost += 10000;
						if (u.usymbiote.mhp < u.usymbiote.mhpmax) symhealcost += ((u.usymbiote.mhpmax - u.usymbiote.mhp) * 100);
						if (u.ugold >= symhealcost) {
							verbalize("Sure thing, I can fully heal your symbiote and remove all curses from it for %d dollars.", symhealcost);
							if (yn("Accept the offer?") == 'y') {
								verbalize("Alright, hold still. Don't worry, this injection won't hurt your symbiote a bit.");
								if (!rn2(8)) mtmp->nurse_fixsymbiote = 0;
								u.ugold -= symhealcost;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								uncursesymbiote(TRUE);
								u.usymbiote.mhp = u.usymbiote.mhpmax;
								if (flags.showsymbiotehp) flags.botl = TRUE;
								break;
							}
						} else verbalize("Sorry, fixing your symbiote's current condition costs %d dollars.", symhealcost);

						break;
					case 11:

						if (!uinsymbiosis) {
							verbalize("Hey, I can shut down your symbiote only if you actually have one.");
							break;
						}
						if (u.shutdowntime) {
							verbalize("Your symbiote is already shut down. Trying to shut it down again would disturb the space-time continuum.");
							break;
						}

						if (u.ugold < nurseshutdowncost) {
							verbalize("Sorry, symbiote shutdown costs %d dollars.", nurseshutdowncost);
							break;
						}

						if (u.ugold >= nurseshutdowncost) {
							verbalize("Shutting down your symbiote will temporarily prevent it from attacking, defending or otherwise participating in combat. This will cost you %d dollars.", nurseshutdowncost);
							if (yn("Accept the offer?") == 'y') {
								verbalize("Okay, here's a special injection for your symbiote. Please hold still.");
								if (!rn2(20)) mtmp->nurse_shutdownsymbiote = 0;
								u.ugold -= nurseshutdowncost;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								use_skill(P_SQUEAKING, 3);
								u.shutdowntime = 1000;

								u.nurseshutdowncost += 500;
								if (u.nurseshutdowncost < 1000) u.nurseshutdowncost = 1000; /* fail safe */
								if (flags.showsymbiotehp) flags.botl = TRUE;
								break;
							}
						}

						break;
					case 12:
						if (u.ugold < 2500) {
							verbalize("Sorry, restoration costs 2500 dollars.");
							break;
						}
						if (issoviet) {
							verbalize("Otvali! V Sovetskoy Rossli zdorov'ye istoshchayet VAS!");
							break;
						}
						if (u.ugold >= 2500) {
							verbalize("This procedure will safely recover one drained attribute point for 2500 dollars. Please be aware that if none of your attributes have been drained, you won't get your money back.");
							if (yn("Accept the offer?") == 'y') {
								verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
								u.ugold -= 2500;
								if (!rn2(10)) mtmp->nurse_restoration = 0;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);

								{
									int i, ii, lim;

									i = rn2(A_MAX);		/* start at a random point */
									for (ii = 0; ii < A_MAX; ii++) {
										lim = AMAX(i);
										if (ABASE(i) < lim) {
											ABASE(i)++;
											pline("Wow! This makes you feel good!");
											flags.botl = 1;
											break; /* only restore one --Amy */
										}
									if(++i >= A_MAX) i = 0;
									}
								}

								u.cnd_nurseserviceamount++;
								maybegaincha();
							}
						}

						break;
					case 13:
						if (u.ugold < u.nursevaccinecost) {
							verbalize("Sorry, vaccination costs %d dollars.", u.nursevaccinecost);
							break;
						}
						if (u.ulevel < 4) {
							verbalize("Sorry. You're still a child, and the permanent vaccination commission doesn't recommend us to vaccinate children. Come back when you're older.");
							break;
						}
						if (u.ugold >= u.nursevaccinecost) {
							verbalize("Yes, you are wise to get vaccinated against COVID-19. Unfortunately, we are currently experiencing supply bottlenecks, which has caused the prices for vaccines to explode, which means that a shot will cost you %d dollars, but it's for the best, because we're using a Yendorian State Approved(TM) vaccine that is guaranteed to be safe and highly effective! Get your covid shot now!", u.nursevaccinecost);
							if (yn("Accept the offer?") == 'y') {
								verbalize("Okay, hold still while I puncture you with this long, pointy needle...");
								u.ugold -= u.nursevaccinecost;
								if (!rn2(20)) mtmp->nurse_vaccine = 0;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								u.nursevaccinecost += 250;
								if (u.nursevaccinecost < 2000) u.nurseshutdowncost = 2000; /* fail safe */
								upnivel(TRUE); /* guaranteed */
							}
						}

						break;
					case 14:
						if (u.ugold < 100) {
							verbalize("Sorry, that costs 100 dollars.");
							break;
						}
						if (u.ugold >= 100) {
							verbalize("You can buy one line of alla for 100 dollars.");
							if (yn("Accept the offer?") == 'y') {
								verbalize("Alright here's a line of alla for you.");
								u.ugold -= 100;
								if (!rn2(100)) mtmp->nurse_alla = 0;
								if (u.ualign.type == A_NEUTRAL) adjalign(1);
								u.cnd_nurseserviceamount++;
								maybegaincha();
								gain_alla(1);
							}
						}

						break;
				} /* switch statement */
			} /* n > 0 menu check */

noservices:
	    if (uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == CHAIN_CLASS || uwep->oclass == VENOM_CLASS || is_weptool(uwep))
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

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Medic!!!";
			break;
		}

		if (mtmp->mtame && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "We gotta stop by the next canteen.";
			break;
		}

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
			verbalize("%s", da_msgs[rn2(SIZE(da_msgs))]);
		}
	    }
	    break;
	case MS_PUPIL:

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbalize("%s! Those bullies beat me up! Help!", flags.female ? "Mom" : "Dad");
			break;
		}
		if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Man, how much longer does this lesson last? I need a break so I can eat some peanuts!";
			break;
		}

		pupiltaunt();

		break;

	case MS_WHORE:

		if (mtmp->mtame && u.roxannemode && mtmp->data == &mons[PM_ROXANNE]) {
			if (u.roxannetimer) {
				if (u.roxannetimer < 1000) verbalize("My poison enchantment is active, but I'm running low on bolts!");
				else verbalize("My poison enchantment is active, meaning that all nearby monsters are being poisoned automatically.");
			}

			register struct obj *roxybolt;
findmorebolts:
			roxybolt = carrying(CROSSBOW_BOLT);
			if (!roxybolt) roxybolt = carrying(FLEECE_BOLT);
			if (!roxybolt) roxybolt = carrying(PIN_BOLT);
			if (!roxybolt) roxybolt = carrying(MINERAL_BOLT);
			if (!roxybolt) roxybolt = carrying(DROVEN_BOLT);
			if (!roxybolt) roxybolt = carrying(KOKKEN);
			if (!roxybolt) roxybolt = carrying(SEEKER_BOLT);
			if (!roxybolt) roxybolt = carrying(POISON_BOLT);
			if (!roxybolt) roxybolt = carrying(HEAVY_CROSSBOW_BOLT);
			if (!roxybolt) roxybolt = carrying(SPIKY_BOLT);
			if (!roxybolt) roxybolt = carrying(HARDENED_BOLT);
			if (!roxybolt) roxybolt = carrying(INFERIOR_BOLT);
			if (!roxybolt) roxybolt = carrying(ETHER_BOLT);
			if (!roxybolt) roxybolt = carrying(MATERIAL_BOLT);
			if (roxybolt) {
				verbalize("Ah, excellent, you have a stack of %ld crossbow bolts! Do you want to give them to me?", roxybolt->quan);

				if (yn("Give the bolts to Roxanne?") == 'y') {

					struct attack* attkptr;
					struct permonst* poisonroxanne = &mons[PM_ROXANNE];

					u.roxannetimer += (roxybolt->quan * 200);
					useupall(roxybolt);
					verbalize("Thanks! My poison enchantment will ensure that the enemy is poisoned.");

					attkptr = &poisonroxanne->mattk[4];
					attkptr->aatyp = AT_NONE;
					attkptr->adtyp = AD_POIS;
					attkptr->damn = 0;
					attkptr->damd = 4;

					attkptr = &poisonroxanne->mattk[5];
					attkptr->aatyp = AT_LASH;
					attkptr->adtyp = AD_POIS;
					attkptr->damn = 2;
					attkptr->damd = 4;

					goto findmorebolts;
				}				

			}
		}

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Why did you allow those rude customers to treat me like that? Some sort of pimp you are!";
			break;
		}
		if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Can we get something to eat, honey? I promise that I'll pay for my meal with my own money!";
			break;
		}

		if (mtmp->data == &mons[PM_ROXY] && !mtmp->mtame && mtmp->wastame && u.ugold >= 1000) {
			verbalize("For 1000 zorkmids I'll join your team again.");
			if (yn("Accept the offer?") == 'y') {

				struct monst *roxylein;
				u.ugold -= 1000;

				roxylein = tamedog(mtmp, (struct obj *) 0, TRUE);
				if (roxylein) mtmp = roxylein;
				if (mtmp) verbalize("Thank you!");
				else impossible("roxy was tamed but doesn't exist now??");
				break;
			}

		}

		if (!flags.female) verbalize("Hey, baby, want to have some fun?");
		else verbalize("I don't have many female customers, but do you want to have fun with me?");
		if (yn("Accept the offer?") == 'y') {

			if (u.homosexual == 0) {
				pline("You're discovering your sexuality...");
				getlin("Are you homosexual? [y/yes/no] (If you answer no, you're heterosexual.)", buf);
				(void) lcase (buf);
				if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) u.homosexual = 2;
				else u.homosexual = 1;
			}

			(void) doseduce(mtmp);
			break;
		}

		if (!mtmp->mpeaceful) prostitutetaunt();

		break;

	case MS_SUPERMAN:

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "I WILL COME BACK...";
			break;
		}
		if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "HUNGRY! NEED FOOD!";
			break;
		}
		if (mtmp->mtame) { /* tame and no distress */
			static const char *superman_tame[] = {
				"GO!",
				"LETS! GO!",
				"GO! SQUAD!",
				"WE WILL KICK THEM IN THE GOD DAMN ASS!",
				"MOVE IN!",
				"ENEMY SPOTTED!",
				"FIRE IN THE ASS HOLE!",
				"SQUAD STAY ALERT FOUR HOSTILES!",
				"SIR I HERE SOMETHING!", /* too lazy to check for female PC --Amy */
				"MY PRIVATE IS FIVEMETERS TANGO ALL!",
				"MY PRIVATE IS TENMETERS ALERT CREEPS HOT ASS!",
				"ECHO MISSION IS GO!",
			};

			verbalize("%s", superman_tame[rn2(SIZE(superman_tame))]);
			break;
		}

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

		verbalize("%s", superman_msgs[rn2(SIZE(superman_msgs))]);
		badeffect();
		increasesanity(rnz(50 + (mtmp->m_lev * 5) ));
		u.cnd_supermancount++;
		}
		break;

	case MS_TEACHER:

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "Those kids these days ain't got no respect, I tell you...";
			break;
		}
		if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Why did I forget to bring along any food?";
			break;
		}

		teachertaunt();

		break;

	case MS_PRINCIPAL:

		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "No! I can't faint! I'm the only one who can lead this school!";
			break;
		}
		if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Wait, I need a pit stop! Be back in a minute!";
			break;
		}

		principaltaunt();

		break;

	case MS_BOSS:
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/3) {
			verbl_msg = "What? It's impossible that they are beating me, I'm the boss of this game after all!";
			if (Sick) verbl_msg = "I might be running low on health, but unlike you, at least my nose isn't running all the time!";
			break;
		}
		if (mtmp->mtame && mtmp->mhp < mtmp->mhpmax/5) {
			verbl_msg = "Argh, my spaceship is falling apart! Quick, someone needs to repair it!";
			break;
		}
		if (mtmp->mtame && !mtmp->isminion && hastoeat && moves > EDOG(mtmp)->hungrytime) {
			verbl_msg = "Dude, get me something to eat! Now! You have 50 turns or you're fired.";
			break;
		}
		if (mtmp->mtame) {
			verbl_msg = "Yes, my pet, we're in this together. I'm the boss, you just need to follow my lead and we'll finish off our enemies.";
			break;
		}
		break;

	case MS_SMITH:
		/* this assumes that it's Duri, however actually there's also other monsters using this... --Amy
		 * you can only request services at his forge, but other monsters can bring you the artifacts */
		if (u.duriworking) {
			verbalize("I'm working. Please don't disturb me again!");
			break;
		}

		if (!mtmp->mpeaceful) {
			verbalize("That's it. No soup for you!");
			break;
		}

		/* request variable: 0 = nothing, 1 = evil artifact, 2 = good artifact */
		if (u.durirequest == 1) {
			u.durirequest = 0;
			bad_artifact();
			if (mtmp->data == &mons[PM_BLACKSMITH] || mtmp->data == &mons[PM_ELONA_BLACKSMITH]) verbalize("Here's your artifact. Watch out, it's cursed and may well have downsides.");
			else verbalize("Here, I'm supposed to give you this cursed artifact from the blacksmith. Watch out, it may well have downsides.");
			break;
		}

		if (u.durirequest == 2) {
			struct obj *durifact;
			u.durirequest = 0;

			boolean havegifts = u.ugifts;
			if (!havegifts) u.ugifts++;

			durifact = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
			if (durifact) {

				int duriskill = get_obj_skill(durifact, TRUE);

				if (P_MAX_SKILL(duriskill) == P_ISRESTRICTED) {
				    unrestrict_weapon_skill(duriskill);
				} else if (P_MAX_SKILL(duriskill) == P_UNSKILLED) {
					unrestrict_weapon_skill(duriskill);
					P_MAX_SKILL(duriskill) = P_BASIC;
				} else if (rn2(2) && P_MAX_SKILL(duriskill) == P_BASIC) {
					P_MAX_SKILL(duriskill) = P_SKILLED;
				} else if (!rn2(4) && P_MAX_SKILL(duriskill) == P_SKILLED) {
					P_MAX_SKILL(duriskill) = P_EXPERT;
				} else if (!rn2(10) && P_MAX_SKILL(duriskill) == P_EXPERT) {
					P_MAX_SKILL(duriskill) = P_MASTER;
				} else if (!rn2(100) && P_MAX_SKILL(duriskill) == P_MASTER) {
					P_MAX_SKILL(duriskill) = P_GRAND_MASTER;
				} else if (!rn2(200) && P_MAX_SKILL(duriskill) == P_GRAND_MASTER) {
					P_MAX_SKILL(duriskill) = P_SUPREME_MASTER;
				}
				if (Race_if(PM_RUSMOT)) {
					if (P_MAX_SKILL(duriskill) == P_ISRESTRICTED) {
					    unrestrict_weapon_skill(duriskill);
					} else if (P_MAX_SKILL(duriskill) == P_UNSKILLED) {
						unrestrict_weapon_skill(duriskill);
						P_MAX_SKILL(duriskill) = P_BASIC;
					} else if (rn2(2) && P_MAX_SKILL(duriskill) == P_BASIC) {
						P_MAX_SKILL(duriskill) = P_SKILLED;
					} else if (!rn2(4) && P_MAX_SKILL(duriskill) == P_SKILLED) {
						P_MAX_SKILL(duriskill) = P_EXPERT;
					} else if (!rn2(10) && P_MAX_SKILL(duriskill) == P_EXPERT) {
						P_MAX_SKILL(duriskill) = P_MASTER;
					} else if (!rn2(100) && P_MAX_SKILL(duriskill) == P_MASTER) {
						P_MAX_SKILL(duriskill) = P_GRAND_MASTER;
					} else if (!rn2(200) && P_MAX_SKILL(duriskill) == P_GRAND_MASTER) {
						P_MAX_SKILL(duriskill) = P_SUPREME_MASTER;
					}
				}

				dropy(durifact);
				discover_artifact(durifact->oartifact);
			}

			if (!havegifts) u.ugifts--;

			if (mtmp->data == &mons[PM_BLACKSMITH] || mtmp->data == &mons[PM_ELONA_BLACKSMITH]) verbalize("Here's your artifact. You'll find it on the floor beneath you. Have fun!");
			else verbalize("Duri sent me to give you this artifact, so I'm dropping it at the floor beneath you. Have fun!");
			break;
		}

		if (mtmp->data == &mons[PM_BLACKSMITH] || mtmp->data == &mons[PM_ELONA_BLACKSMITH]) {
			verbalize("Welcome to Duri's forge! I offer various services, including equipment repair, proofing, and artifact forging.");

			winid tmpwin;
			anything any;
			menu_item *selected;
			int n;

			any.a_void = 0;         /* zero out all bits */
			tmpwin = create_nhwindow(NHW_MENU);
			start_menu(tmpwin);
			any.a_int = 1;
			add_menu(tmpwin, NO_GLYPH, &any , 'r', 0, ATR_NONE, "Repair", MENU_UNSELECTED);
			any.a_int = 2;
			add_menu(tmpwin, NO_GLYPH, &any , 'e', 0, ATR_NONE, "Erosionproofing", MENU_UNSELECTED);
			any.a_int = 3;
			add_menu(tmpwin, NO_GLYPH, &any , 'b', 0, ATR_NONE, "Bad artifact", MENU_UNSELECTED);
			any.a_int = 4;
			add_menu(tmpwin, NO_GLYPH, &any , 'g', 0, ATR_NONE, "Good artifact", MENU_UNSELECTED);

			end_menu(tmpwin, "Services Available:");
			n = select_menu(tmpwin, PICK_ONE, &selected);
			destroy_nhwindow(tmpwin);

			if (n > 0) {
				switch (selected[0].item.a_int) {
					case 1:
						verbalize("For only %d zorkmids, I can repair all damage on an item of your choice!", u.durirepaircost);
						if (u.ugold < u.durirepaircost) {
							verbalize("But sadly you don't seem to have enough money.");
							break;
						}
						struct obj *repairobj;
						repairobj = getobj(allnoncount, "repair");
						if (!repairobj) break;
						if (repairobj) {
							if (!(repairobj->oeroded) && !(repairobj->oeroded2)) {
								verbalize("That item isn't damaged. You don't need to repair it!");
								break;
							}
							if (yn("Pay for the repairs?") == 'y') {
								u.ugold -= u.durirepaircost;
								u.durirepaircost += 500;
								if (!stack_too_big(repairobj)) {
									repairobj->oeroded = repairobj->oeroded2 = 0;
									verbalize("Alright! Your item is in tiptop shape again!");
								} else verbalize("Oh no! The stack was too big, causing my repair attempt to fail!");
							}
						}
						break;
					case 2:
						verbalize("So you want to erodeproof your items? All you need to do is give me %d zorkmids!", u.durienchantcost);
						if (u.ugold < u.durienchantcost) {
							verbalize("Well %s, it seems you can't afford it. Bummer.", flags.female ? "lady" : "dude");
							break;
						}
						struct obj *proofobj;
						proofobj = getobj(allnoncount, "erosionproof");
						if (!proofobj) break;
						if (proofobj) {
							if (proofobj->oerodeproof) {
								verbalize("That item is erosionproof already!");
								break;
							}
							if (yn("Pay for erosionproofing?") == 'y') {
								u.ugold -= u.durienchantcost;
								u.durienchantcost += 2000;
								if (!stack_too_big(proofobj)) {
									proofobj->oerodeproof = 1;
									verbalize("Your item is untouchable now!");
								} else verbalize("Bad luck, %s - proofing such a large stack of items can fail, and it seems you didn't get lucky this time.", flags.female ? "gal" : "lad");
							}
						}
						break;
					case 3:
						verbalize("Forging a bad artifact will cost %d zorkmids.", u.duriarticostevil);
						if (u.ugold < u.duriarticostevil) {
							verbalize("Unfortunately you don't have enough money!");
							break;
						}
						if (yn("Forge a bad artifact?") == 'y') {
							u.ugold -= u.duriarticostevil;
							u.duriarticostevil += 5000;
							u.duriworking = 1000;
							u.durirequest = 1;
							verbalize("Alright, give me about 1000 turns and I'll have something for you.");
						}
						break;
					case 4:
						verbalize("Forging a good artifact will cost %d zorkmids.", u.duriarticostnormal);
						if (u.ugold < u.duriarticostnormal) {
							verbalize("Unfortunately you don't have enough money!");
							break;
						}
						if (yn("Forge a good artifact?") == 'y') {
							u.ugold -= u.duriarticostnormal;
							u.duriarticostnormal += 10000;
							u.duriworking = 2000;
							u.durirequest = 2;
							verbalize("Alright, give me about 2000 turns and I'll have something for you. It'll be worth the wait, I promise!");
						}
						break;
				}
			}
			break;

		}

		verbalize("Sorry, I don't think I can help you. Look for Duri in the Blacksmith's Forge.");

		break;

    }

    if (pline_msg) pline("%s %s", Monnam(mtmp), pline_msg);
    else if (verbl_msg) verbalize("%s", verbl_msg);
    return(1);
}


int
dotalk()
{

	struct monst* mtmp2;
	int untamingchance;

	if (MenuIsBugged) {
	pline("The chat command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	if (HardcoreAlienMode) {

		wouwoutaunt();
		u.cnd_wouwoucount++;
		if (Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) sjwtrigger();
		wake_nearby();

		return 1;

	}

    int result;
    boolean save_soundok = flags.soundok;
    if (!(YouAreDeaf)) flags.soundok = 1;	/* always allow sounds while chatting */
    result = dochat();
    flags.soundok = save_soundok;

	if (FemtrapActiveSolvejg) {

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

				if (untamingchance > rnd(10) && !(Role_if(PM_DRAGONMASTER) && uarms && Is_dragon_shield(uarms) && mtmp2->data->mlet == S_DRAGON) && !((rnd(30 - ACURR(A_CHA))) < 4) ) {

					mtmp2->mtame = mtmp2->mpeaceful = 0;

				}

			} else if (!mtmp2->mtame && !is_infrastructure_monster(mtmp2)) {

				mtmp2->mtame = mtmp2->mpeaceful = 0;

			}

			if (!is_infrastructure_monster(mtmp2) && !mtmp2->mtame && !rn2(5)) mtmp2->mfrenzied = 1;

		}

	}

    return result;
}

/* monster got hit, and can now emit a pain sound or otherwise display a message for the player; it shouldn't always do so,
 * but every once in a while, so that the player can gauge roughly how much damage they're doing to a monster,
 * or (more importantly) how much their pet has been hurt, to make it possible to tell when they're in need of healing
 * without constantly needing to stethoscope or chat to them (which requires you to be adjacent to them and often also
 * wastes a turn, which isn't always an option in the middle of a fight). --Amy */
void
monster_pain(mtmp)
register struct monst *mtmp;
{
	int painchance = 10;
	boolean soundbased = FALSE;
	int distresslevel = 0;

	if (!mtmp) {
		impossible("monster_pain called with nonexistant monster!");
		return;
	}

	if (mtmp->mundetected && is_hider(mtmp->data)) mtmp->mundetected = FALSE;
	if (mtmp->mundetected && hides_under(mtmp->data)) mtmp->mundetected = FALSE;

	if (FemtrapActiveLittleMarie && is_female(mtmp->data)) {
		if (SuperFemtrapLittleMarie) randomnastytrapeffect(rnz(2 + (mtmp->m_lev * 2)), 1000 - (mtmp->m_lev * 2));
		else randomnastytrapeffect(rnz(2 + mtmp->m_lev), 1000 - mtmp->m_lev);
		u.cnd_littlemarietrapcnt++;
	}

	if (FemtrapActiveSonja && !mtmp->mpeaceful && !mtmp->mtame && !rn2(SuperFemtrapSonja ? 5 : 10) && is_female(mtmp->data)) {
		incr_itimeout(&HAggravate_monster, (mtmp->m_lev * 5) + 1);
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();
	}

	if (monstersoundtype(mtmp) == MS_FEARHARE) {
		monflee(mtmp, rnd(10), TRUE, TRUE, FALSE);
	}

	if (mtmp->mhp < (mtmp->mhpmax / 4)) distresslevel = 3;
	else if (mtmp->mhp < (mtmp->mhpmax / 2)) distresslevel = 2;
	else if (mtmp->mhp < (mtmp->mhpmax * 3 / 4)) distresslevel = 1;

	if (distresslevel == 0) return; /* didn't lose enough health yet */

	if (monstersoundtype(mtmp) == MS_SILENT) return; /* never emits any sound */
	if (monstersoundtype(mtmp) == MS_SISSY) painchance = 0; /* always emits a sound whenever it's damaged */

	/* petkeeping skill makes it more likely for pets to make sounds, so that you can see how much they've been hurt */
	if (mtmp->mtame) {
		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_PETKEEPING)) {
		      	case P_BASIC:	painchance -= 1; break;
		      	case P_SKILLED:	painchance -= 2; break;
		      	case P_EXPERT:	painchance -= 3; break;
		      	case P_MASTER:	painchance -= 4; break;
		      	case P_GRAND_MASTER:	painchance -= 5; break;
		      	case P_SUPREME_MASTER:	painchance -= 6; break;
		      	default: break;
			}

		}
	}

	/* riding skill makes it more likely for your steed to make sounds, even if the steed isn't tame */
	if (u.usteed && (u.usteed == mtmp)) {
		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_RIDING)) {
		      	case P_BASIC:	painchance -= 1; break;
		      	case P_SKILLED:	painchance -= 2; break;
		      	case P_EXPERT:	painchance -= 3; break;
		      	case P_MASTER:	painchance -= 4; break;
		      	case P_GRAND_MASTER:	painchance -= 5; break;
		      	case P_SUPREME_MASTER:	painchance -= 6; break;
		      	default: break;
			}
		}
	}
	if (painchance < 0) painchance = 0; /* fail safe */

	/* pets need even better chances */
	if (mtmp->mtame && !rn2(10)) painchance = 0;
	if (!PlayerCannotUseSkills && mtmp->mtame) {
		if (P_SKILL(P_PETKEEPING) >= P_BASIC && !rn2(10)) painchance = 0;
		if (P_SKILL(P_PETKEEPING) >= P_SKILLED && !rn2(10)) painchance = 0;
		if (P_SKILL(P_PETKEEPING) >= P_EXPERT && !rn2(10)) painchance = 0;
		if (P_SKILL(P_PETKEEPING) >= P_MASTER && !rn2(10)) painchance = 0;
		if (P_SKILL(P_PETKEEPING) >= P_GRAND_MASTER && !rn2(10)) painchance = 0;
		if (P_SKILL(P_PETKEEPING) >= P_SUPREME_MASTER && !rn2(10)) painchance = 0;
	}

	if (!(PainSense && !rn2(10)) && !(StrongPainSense && !rn2(5)) && (painchance >= (rnd(11))) ) return;

	/* some of these are sound-based, others are vision-based */
	switch (monstersoundtype(mtmp)) {
		default:
			break;
		case MS_HUMANOID:
		case MS_OHGOD:
		case MS_ARREST:
		case MS_SOLDIER:
		case MS_CUSS:
		case MS_SUPERMAN:
		case MS_CONVERT:
		case MS_JAPANESE:
		case MS_SOVIET:
		case MS_BRAG:
		case MS_PRINCESSLEIA:
		case MS_FLUIDATOR:
		case MS_SISSY:
		case MS_BOT:
		case MS_BARBER:
		case MS_TREESQUAD:
		case MS_METALMAFIA:
		case MS_WOLLOH:
			soundbased = TRUE;
			break;
	}

	if (soundbased && !flags.soundok) return; /* can't hear */
	if (soundbased && monstersoundtype(mtmp) != MS_SISSY && (distu(mtmp->mx,mtmp->my) > (BOLT_LIM+1)*(BOLT_LIM+1)) ) return; /* too far away */
	if (!soundbased && monstersoundtype(mtmp) != MS_GIBBERISH && !cansee(mtmp->mx, mtmp->my)) return; /* can't see */

	switch (monstersoundtype(mtmp)) {

		default:
		case MS_BARK:
		case MS_MEW:
		case MS_ROAR:
		case MS_GROWL:
		case MS_SQEEK:
		case MS_SQAWK:
		case MS_HISS:
		case MS_BUZZ:
		case MS_GRUNT:
		case MS_NEIGH:
		case MS_WAIL:
		case MS_GURGLE:
		case MS_BURBLE:
		/*case MS_ANIMAL: -- duplicate */ 
		case MS_SHRIEK:
		case MS_BONES:
		case MS_LAUGH:
		case MS_MUMBLE:
		case MS_IMITATE:
		/*case MS_ORC: -- duplicate */
		case MS_GUARD:
		case MS_DJINNI:
		case MS_NURSE:
		case MS_SEDUCE:
		case MS_VAMPIRE:
		case MS_BRIBE:
		case MS_RIDER:
		case MS_LEADER:
		case MS_NEMESIS:
		case MS_GUARDIAN:
		case MS_SELL:
		case MS_ORACLE:
		case MS_PRIEST:
		case MS_SPELL:
		case MS_WERE:
		case MS_BOAST:
		case MS_GYPSY:
		case MS_SHEEP:
		case MS_CHICKEN:
		case MS_COW:
		case MS_PARROT:
		case MS_VICE:
		case MS_BOSS:
		case MS_SOUND:
		case MS_STENCH:
		case MS_CASINO:
		case MS_GLYPHS:
		case MS_SNORE:
		case MS_PHOTO:
		case MS_REPAIR:
		case MS_DRUGS:
		case MS_COMBAT:
		case MS_MUTE:
		case MS_CORONA:
		case MS_TRUMPET:
		case MS_PAIN:
		case MS_SING:
		case MS_ALLA:
		case MS_POKEDEX:
		case MS_APOC:
		case MS_LIEDER:
		case MS_GAARDIEN:
		case MS_CLOCK:
		case MS_AREOLA:
		case MS_DEAD:
		case MS_STABILIZE:
		case MS_ESCAPE:
		case MS_POMPEJI:
		case MS_FEARHARE:
		case MS_CODE:
		case MS_BULLETATOR:
		case MS_OE:
		case MS_NASTYTRAP:
		case MS_FEMI:
		case MS_ANOREXIA:
		case MS_BULIMIA:
		case MS_GRAKA:
		case MS_BLANKER:
		case MS_CONDESCEND:
		case MS_TRIP:
		case MS_COMMON:
		case MS_AETHERIC:
		case MS_BUSY:
		case MS_JAM:
		case MS_LOWPRIORITY:
		case MS_SPEEDBUG:
		case MS_BEG:
		case MS_HIRE:
		case MS_NEWS:
		case MS_HOOT:
		case MS_RUSTLE:
		case MS_SEMEN:
		case MS_WEATHER:

			switch (distresslevel) {
				case 1:
					pline("%s screams.", Monnam(mtmp));
					break;
				case 2:
					pline("%s writhes in pain.", Monnam(mtmp));
					break;
				case 3:
					pline("%s is severely hurt!", Monnam(mtmp));
					break;
			}
			break;

		case MS_MODALSHOP:
			switch (distresslevel) {
				case 1:
					verbalize("?No tienes modales?");
					break;
				case 2:
					verbalize("!Que te jodan, gilipollas!");
					break;
				case 3:
					verbalize("!Hijo de puta!");
					break;
			}
			break;

		case MS_SCIENTIST:
			switch (distresslevel) {
				case 1:
					verbalize("I soiled my lab coat.");
					break;
				case 2:
					verbalize("No! Please!");
					break;
				case 3:
					verbalize("Please, I swear I didn't touch your sister! Aaaaahhh!");
					break;
			}
			break;

		case MS_INCISION:
			switch (distresslevel) {
				case 1:
					verbalize("You won't get to keep your genitals!");
					break;
				case 2:
					verbalize("I said that I'm gonna cut your genitals, and I meant it!");
					break;
				case 3:
					verbalize("Your genital area will be covered by an enormous red lake when I'm finished with you...");
					break;
			}
			break;

		case MS_CRYTHROAT:
			switch (distresslevel) {
				case 1:
					verbalize("WAAAAAH!");
					break;
				case 2:
					verbalize("AEAEAEAEAEAEH! OWWWW!");
					break;
				case 3:
					verbalize("OWWOWOW OW-WOW! OWWOWOW OW-WOW! AAAAAAAAAAAAHHH...");
					break;
			}
			break;

		case MS_OMEN:
			switch (distresslevel) {
				case 1:
					verbalize("You're gonna die anyway.");
					break;
				case 2:
					verbalize("You're just digging your own grave!");
					break;
				case 3:
					verbalize("Even if you defeat me, my death curse will make you wish you hadn't killed me!");
					break;
			}
			break;

		case MS_ARMORER:
			switch (distresslevel) {
				case 1:
					verbalize("Grrr!");
					break;
				case 2:
					verbalize("Hrrrrrgh!");
					break;
				case 3:
					verbalize("Oaaaaaaaaa-aah!");
					break;
			}
			break;

		case MS_HERCULES:
			switch (distresslevel) {
				case 1:
					verbalize("That's just a flesh wound...");
					break;
				case 2:
					verbalize("I underestimated you, but you'll still fall before my sword!");
					break;
				case 3:
					verbalize("The gods will grant me the strength to overcome this difficult opponent...");
					break;
			}
			break;
		case MS_SILLY:
			switch (distresslevel) {
				case 1:
					verbalize("My vital signs are dropping!");
					break;
				case 2:
					verbalize("Uhh, that might require a tetanus shot. Speaking of which, my last vaccination was more than 10 years ago...");
					break;
				case 3:
					verbalize("Hey! Why is this HEV suit beeping constantly? That's so annoying!");
					break;
			}
			break;
		case MS_MIDI:
			switch (distresslevel) {
				case 1:
					verbalize("Dadadiededada-dedidada-diededada-dedidada...");
					break;
				case 2:
					verbalize("La laaaaaa, lalala-laaaaaa...");
					break;
				case 3:
					verbalize("nananananinenananoe!");
					break;
			}
			break;
		case MS_PIRATE:
			switch (distresslevel) {
				case 1:
					verbalize("Scuttle that landlubber!");
					break;
				case 2:
					verbalize("Gonna keelhaul ye!");
					break;
				case 3:
					verbalize("Ye'r gonna dance with Jack Ketch!");
					break;
			}
			break;
		case MS_GAGA:
			switch (distresslevel) {
				case 1:
					verbalize("Do you not recognize the fact that I'm %s?", mongenderidentity(mtmp));
					break;
				case 2:
					verbalize("This %s is not amused!", mongenderidentity(mtmp));
					break;
				case 3:
					verbalize("One day all the members of the LGBTIQ+ community are gonna cancel you, and then everyone will treat me as a respected %s!", mongenderidentity(mtmp));
					break;
			}
			break;
		case MS_SHOCKTROOPER:
			switch (distresslevel) {
				case 1:
					verbalize("mub puh mub dit dit");
					break;
				case 2:
					verbalize("dit kss kss");
					break;
				case 3:
					verbalize("kss dit dit kss");
					break;
			}
			break;
		case MS_MOLEST:
			switch (distresslevel) {
				case 1:
					if (flags.female) pline("%s shouts 'I'm gonna fuck you for that, bitch!'", Monnam(mtmp));
					else pline("%s shouts 'Bastard!'", Monnam(mtmp));
					break;
				case 2:
					if (flags.female) pline("%s shouts 'My penis will thrust into your vagina again and again, you slut!'", Monnam(mtmp));
					else pline("%s shouts 'Motherfucker, I'll polish your kisser!'", Monnam(mtmp));
					break;
				case 3:
					if (flags.female) pline("%s screams 'No woman gets away from me! You will be made pregnant by my dick, no matter what you devil's whore try to do!'", Monnam(mtmp));
					else pline("%s screams 'Fuck Fuck Fuck why does that asshole have to be a man why can't I get a woman instead I'd so totally rape her into submission fuuuuuck...'", Monnam(mtmp));
					break;
			}
			break;
		case MS_TEMPER:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'You'll die!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'I'ma smash you for this!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s screams 'FUCK YOU ASSHOLE!!!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_SHIVERINGESLES:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Sie sterben schnell, wenn man sie jenijakt. Jenijak! Jenijak!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Sendockel den Klen. Verstanden? Den Klen!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s groans 'Fribbel! Einfach fribbel!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_GENDER:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Help! I'm %s and got attacked!'", Monnam(mtmp), is_neuter(mtmp->data) ? "neuter" : mtmp->female ? "female" : "male");
					break;
				case 2:
					pline("%s shouts 'Someone must rescue me, I'm a very important %s!'", Monnam(mtmp), is_neuter(mtmp->data) ? "neutrum" : mtmp->female ? "woman" : "man");
					break;
				case 3:
					pline("%s groans 'I don't wanna die a virgin...'", Monnam(mtmp));
					break;
			}
			break;
		case MS_OUTCAST:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'What the heck?'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Outcasts, attack my target!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s groans 'Casdin shall avenge me...'", Monnam(mtmp));
					break;
			}
			break;
		case MS_AMNESIA:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Something just hurt me!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Ah! I think I just got stabbed! But I don't remember by whom!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s groans 'Oh... think I need to perform first aid... but I forgot how that works...'", Monnam(mtmp));
					break;
			}
			break;
		case MS_BOS:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'I got hit!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'They hit me in the chest!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s groans 'Argh... I'm not gonna make it...'", Monnam(mtmp));
					break;
			}
			break;
		case MS_DREMORA:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'I'll banish you!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Beg, milkface!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'Break apart! Shatter!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_MYTHICALDAWN:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Dawn is breaking!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Lord Dagon awaits your soul in Oblivion!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'The time of purgatory is drawing near!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_BAN:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'This area isn't safe!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Why did I come here? Should have picked another dungeon level!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s screams 'This is insane! I'll warp to a different level!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_MUTANT:
			switch (distresslevel) {
				case 1:
					pline("%s screams 'Ahh, the %s!'", Monnam(mtmp), mbodypart(mtmp, LEG));
					break;
				case 2:
					pline("%s screams 'Paaaaaaaain!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s roars 'Gueueueaaaarrrrrrr!!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_ENCLAVE:
			switch (distresslevel) {
				case 1:
					pline("%s screams 'Medic!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s screams 'My %s!'", Monnam(mtmp), !rn2(4) ? mbodypart(mtmp, LEG) : !rn2(3) ? mbodypart(mtmp, ARM) : rn2(2) ? mbodypart(mtmp, HEAD) : mbodypart(mtmp, EYE) );
					break;
				case 3:
					pline("%s mutters 'For... Eden.'", Monnam(mtmp));
					break;
			}
			break;
		case MS_TALONCOMPANY:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Don't mess with the Talons!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Fuck that shit!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'Retreat, fucking shit, retreat!!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_REGULATOR:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'The law will always win!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Arrh!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'Retreat!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_HUMANOID:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'I'm hit!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Need a healing potion!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s groans 'I'm close to death!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_METALMAFIA:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Stop being such a dick! Just gimme your metal!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'I'll blast you with my shotgun if you don't give me your metal!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s groans 'You cannot stop the metal mafia...'", Monnam(mtmp));
					break;
			}
			break;
		case MS_RAIDER:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Should have stayed in bed instead of playing superhero!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Muck!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s sobs 'No, please don't kill me!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_ARREST:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Unit %d was hit!'", Monnam(mtmp), mtmp->m_id);
					break;
				case 2:
					pline("%s shouts 'Officer in need of a backup!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'Emergency! Unit %d confronted with extreme resistance!'", Monnam(mtmp), mtmp->m_id);
					break;
			}
			break;
		case MS_FLUIDATOR:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'The opponent met me!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'I have hits off-gotten, I custom assistance!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'Outer one no, equivalent I am dead!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_TREESQUAD:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'They're resisting!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'We need to kill them, it's the only way to stop them!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'They're going to burn our trees! We must not fall!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_SOLDIER:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Medic!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Excuse me! I'm in need of medical attention!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'God dammit!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_BARBER:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'You're not allowed to hit me, I'm a barber!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Stop that, man! Let me cut your hair instead!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'I'm gonna cut like the devil!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_SPOILER:
			switch (distresslevel) {
				case 1:
					pline("Out of memory. Please restart the program.");
					break;
				case 2:
					pline("Runtime Error! R6016 - Not enough space for thread data");
					break;
				case 3:
					pline("Runtime Error! R6025 - pure virtual function call");
					break;
			}
			break;
		case MS_OHGOD:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Oh god!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Oh doublegod!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'Ohtenfoldgod!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_WOLLOH:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Wolloh age!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Wolloh, you fucked up, wolloh!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'Ey wolloh, older! Wolloh, I have a knife, wolloh, and I'll cut out your fucking tongue, wolloh!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_SELFHARM:
			switch (distresslevel) {
				case 1:
					pline("%s is bleeding...", Monnam(mtmp));
					increasesanity(rnz(10));
					break;
				case 2:
					pline("%s is squirting blood everywhere!", Monnam(mtmp));
					increasesanity(rnz(50));
					break;
				case 3:
					pline("Oh no! %s is bleeding profusely and it just doesn't want to stop and the entire floor is already colored red and... urgh, your %s...", Monnam(mtmp), body_part(STOMACH));
					increasesanity(rnz(200));
					break;
			}
			break;
		case MS_CUSS:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Fuck! I'm hit!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Damn those assholes!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'Your mother is such a cunt!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_WHORE:
			switch (distresslevel) {
				case 1:
					pline("%s complains about a broken nail.", Monnam(mtmp));
					break;
				case 2:
					pline("%s's makeup is running down %s cheeks.", Monnam(mtmp), mhis(mtmp));
					break;
				case 3:
					pline("%s is badly bruised.", Monnam(mtmp));
					break;
			}
			break;
		case MS_SUPERMAN:
			switch (distresslevel) {
				case 1:
					verbalize("GRRRR!");
					break;
				case 2:
					verbalize("ARRRRH! REVENGE!");
					break;
				case 3:
					verbalize("KILL THEM ALL!");
					break;
			}
			break;
		case MS_FART_QUIET:
		case MS_FART_NORMAL:
		case MS_FART_LOUD:
			switch (distresslevel) {
				case 1:
					pline("%s's butt cheeks are bruised.", Monnam(mtmp));
					break;
				case 2:
					pline("%s's butt cheeks look sore.", Monnam(mtmp));
					break;
				case 3:
					pline("%s's butt cheeks are bleeding...", Monnam(mtmp));
					break;
			}
			break;
		case MS_SHOE:
		case MS_SOCKS:
		case MS_PANTS:
			switch (distresslevel) {
				case 1:
					pline("%s's material seems to degrade.", Monnam(mtmp));
					break;
				case 2:
					pline("%s looks a bit worn-out.", Monnam(mtmp));
					break;
				case 3:
					pline("%s seems about to fall apart!", Monnam(mtmp));
					break;
			}
			break;
		case MS_CONVERT:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Kafirler!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Allah seni cezalandiracak!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s grumbles 'Cennette beni bekleyen bir suerue bakire var...'", Monnam(mtmp));
					break;
			}
			break;
		case MS_HCALIEN:
			switch (distresslevel) {
				case 1:
					pline("%s looks annoyed.", Monnam(mtmp));
					break;
				case 2:
					pline("%s seems to be angry at the enemy who attacked %s.", Monnam(mtmp), mhim(mtmp));
					break;
				case 3:
					pline("%s's facial expression is very threatening.", Monnam(mtmp));
					break;
			}
			break;
		case MS_GIBBERISH:
			pline("%s", generate_garbage_string());
			break;
		case MS_HANDY: /* todo for when they're in the middle of a call */
			switch (distresslevel) {
				case 1:
					pline("%s screams.", Monnam(mtmp));
					break;
				case 2:
					pline("%s writhes in pain.", Monnam(mtmp));
					break;
				case 3:
					pline("%s is severely hurt!", Monnam(mtmp));
					break;
			}
			break;
		case MS_CAR:
			switch (distresslevel) {
				case 1:
					pline("%s is dented.", Monnam(mtmp));
					break;
				case 2:
					pline("Splinters of %s's chassis are flying around!", mon_nam(mtmp));
					break;
				case 3:
					pline("%s looks quite a bit like a car wreck!", Monnam(mtmp));
					break;
			}
			break;
		case MS_JAPANESE:
			switch (distresslevel) {
				case 1:
					pline("%s shouts 'Karera no ken ga watashi o osotta!'", Monnam(mtmp));
					break;
				case 2:
					pline("%s shouts 'Iya, senaka ni ya ga sasatte iru!'", Monnam(mtmp));
					break;
				case 3:
					pline("%s shouts 'Watashi wa mada shinu ka jisatsu suru junbi ga dekite imasen!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_SOVIET:
			switch (distresslevel) {
				case 1:
					pline("%s remarks 'Eta igra - otstoy.'", Monnam(mtmp));
					break;
				case 2:
					pline("%s mutters 'Fignya programmirovaniya.'", Monnam(mtmp));
					break;
				case 3:
					pline("%s grumbles 'Emi chertova shlyukha, i yey dolzhno byt' stydno za sebya.'", Monnam(mtmp));
					break;
			}
			break;
		case MS_BRAG:
			switch (distresslevel) {
				case 1:
					verbalize("Bah! I still have %d HP remaining!", mtmp->mhp);
					break;
				case 2:
					verbalize("Yeah whatever, I have %d HP left!", mtmp->mhp);
					break;
				case 3:
					verbalize("No one can take off my remaining %d HP!", mtmp->mhp);
					break;
			}
			break;
		case MS_PRINCESSLEIA:
			switch (distresslevel) {
				case 1:
					pline("%s sighs 'Another evil-doer who doesn't respect a princess...'", Monnam(mtmp));
					break;
				case 2:
					pline("%s sobs 'If only we could live in a world without those violent people...'", Monnam(mtmp));
					break;
				case 3:
					pline("%s cries 'This is not fair! I just want to wear my crown and purple dress, not spill my blue blood all over the place!'", Monnam(mtmp));
					break;
			}
			break;
		case MS_SISSY:
			switch (distresslevel) {
				case 1:
					switch (rnd(5)) {
						case 1:
							verbalize("ow!");
							break;
						case 2:
							verbalize("owwow!");
							break;
						case 3:
							verbalize("ouch!");
							break;
						case 4:
							verbalize("ow, this is painful!");
							break;
						case 5:
							verbalize("ouchie!");
							break;
					}
					break;
				case 2:
					switch (rnd(5)) {
						case 1:
							verbalize("owwowow ow-wow!");
							break;
						case 2:
							verbalize("owwowow!");
							break;
						case 3:
							verbalize("ow! ow!");
							break;
						case 4:
							verbalize("oooooow!");
							break;
						case 5:
							verbalize("man this hurts so much!");
							break;
					}
					break;
				case 3:
					switch (rnd(5)) {
						case 1:
							verbalize("oooooooow-wowowowowowowow!");
							break;
						case 2:
							verbalize("ooooooow! this is so painful! please make it stop hurting!");
							break;
						case 3:
							verbalize("owwowow! owwowow! why doesn't it stop hurting?!");
							break;
						case 4:
							verbalize("owwooooooooow! i don't want that, owwooooooooow!");
							break;
						case 5:
							verbalize("waaaaaaaah! waaaaaaah! you meanie! you big, evil... owwww...");
							break;
					}
					break;
			}
			break;
		case MS_BOT:
			switch (distresslevel) {
				case 1:
					verbalize("Bsst.");
					break;
				case 2:
					verbalize("Dirt ****!");
					break;
				case 3:
					verbalize("Bssssssss-bssssssss...");
					break;
			}
			break;


	}
}

static int
dochat()
{
    register struct monst *mtmp;
    register int tx,ty;
    struct obj *otmp;
	char buf[BUFSZ];

    boolean eligiblesymbio = FALSE;

    if (Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone() || (uarmh && uarmh->oartifact == ART_CLELIA_S_TONGUE_BREAKER) ) {
	pline("You're muted!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }

    if (tech_inuse(T_SILENT_OCEAN)) {
	pline("The silent ocean prevents you from talking.");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }

    if (is_silent(youmonst.data)) {
	pline("As %s, you cannot speak.", !missingnoprotect ? an(youmonst.data->mname) : "this weird creature");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }
    if (Strangled) {
	You_cant("speak.  You're choking!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }
    if (u.uswallow) {
	pline("They won't hear you out there.");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return(0);
    }
    if (Underwater) {
	Your("speech is unintelligible underwater.");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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

    if (mtmp->shinymonst && !mtmp->mfrenzied && !mtmp->mtame && (u.alla > 100)) {

	if (yn("You can tame this monster for 100 alla. Do it?") == 'y') {

		mtmp->mpeaceful = TRUE;
		(void) tamedog(mtmp, (struct obj *) 0, TRUE);
		u.alla -= 100;
		pline("Done!");
		return 1;

	}

    }

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
	if (!canspotmon(mtmp) && !(monstersoundtype(mtmp) == MS_DEEPSTATE) && !(mtmp->egotype_deepstatemember))
	    map_invisible(mtmp->mx, mtmp->my);
	pline("%s is eating noisily.", Monnam(mtmp));
	return (0);
    }

    if ((mtmp->data == &mons[PM_SYMBIULD]) || (mtmp->data == &mons[PM_SYMBIOTE_WITH_A_DATA_DELETE_ATTACK])) eligiblesymbio = TRUE;

    if ( (mtmp->mtame) && (((stationary(mtmp->data) || mtmp->data->mmove == 0 || mtmp->data->mlet == S_TURRET) && !Race_if(PM_GOAULD)) || (!(stationary(mtmp->data) || mtmp->data->mmove == 0 || mtmp->data->mlet == S_TURRET) && Race_if(PM_GOAULD))) && !is_arabellamonster(mtmp->data) && !mtmp->mfrenzied && !(mtmp->data->mlevel > 20 || (mtmp->data->mlevel > (u.ulevel + 5)) || cannot_be_tamed(mtmp->data) || (mtmp->cham == CHAM_ZRUTINATOR) || mtmp->isshk || mtmp->isgd || mtmp->ispriest || mtmp->isminion || mtmp->isgyp || (mtmp->oldmonnm != monsndx(mtmp->data))) ) eligiblesymbio = TRUE;

	if (eligiblesymbio) {
	
	pline("You can attempt to turn this pet into a symbiote. Warning: if it's an intelligent monster, it may fail and result in a frenzied monster instead!");
	getlin("Attempt to turn the pet into a symbiote? [yes/no]", buf);
	(void) lcase (buf);
	if (!(strcmp (buf, "yes")) ) {

		int resistrounds = 0;
		if (!mindless(mtmp->data)) resistrounds++;
		if (humanoid(mtmp->data)) resistrounds++;
		if (mtmp->data == &mons[PM_SYMBIULD]) resistrounds = 0;
		if (mtmp->data == &mons[PM_SYMBIOTE_WITH_A_DATA_DELETE_ATTACK]) resistrounds = 0;
		if (resistrounds >= 1 && resist(mtmp, TOOL_CLASS, 0, 0)) {
			mtmp->mtame = FALSE;
			mtmp->mpeaceful = FALSE;
			mtmp->mfrenzied = TRUE;
			pline("Oh no! %s is frenzied!", Monnam(mtmp));
			return 1;
		}
		if (resistrounds >= 2 && resist(mtmp, TOOL_CLASS, 0, 0)) {
			mtmp->mtame = FALSE;
			mtmp->mpeaceful = FALSE;
			mtmp->mfrenzied = TRUE;
			pline("Oh no! %s is frenzied!", Monnam(mtmp));
			return 1;
		}

		if (uinsymbiosis && u.usymbiote.cursed) {
			pline(FunnyHallu ? "Apparently Morgoth himself decided to curse you with some ancient hex." : "Since your current symbiote is cursed, you cannot get a new one.");
			return 1;
		}

		/* it worked! */

		if (touch_petrifies(mtmp->data) && (!Stone_resistance || (!IntStone_resistance && !rn2(20))) ) {
			if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
				display_nhwindow(WIN_MESSAGE, FALSE);
			else {
				static char kbuf[BUFSZ];
				pline("Incorporating a petrifying creature is a fatal mistake.");
				sprintf(kbuf, "picking the wrong symbiote");
				instapetrify(kbuf);
			}
		}
		if (slime_on_touch(mtmp->data) && !Slimed && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data)) {
			You("don't feel very well.");
			make_slimed(100);
			killer_format = KILLED_BY_AN;
			delayed_killer = "slimed by picking the wrong symbiote";
		}

		turnmonintosymbiote(mtmp, FALSE); /* WARNING: mtmp is removed at this point */

		return 1;
	}

    }

    if (Role_if(PM_CONVICT) && is_rat(mtmp->data) && mtmp->m_lev <= 30 && mtmp->m_lev <= (u.ulevel + 5) && !mtmp->mpeaceful && !mtmp->mtame) {
        You("attempt to soothe the %s with chittering sounds.", l_monnam(mtmp));
        if ((rnl(10) < 2) || ((rnd(30 - ACURR(A_CHA))) < 4)) {
            (void) tamedog(mtmp, (struct obj *) 0, FALSE);
        } else {
            if (rnl(10) > 8) {
                pline("%s unfortunately ignores your overtures.", Monnam(mtmp));
                return 1;
            }
            if (!mtmp->mfrenzied) mtmp->mpeaceful = 1;
            set_malign(mtmp);
        }
        return 1;
    }

    if (u.ugold >= 1000 && !mtmp->mtame && mtmp->mnum != quest_info(MS_NEMESIS) && uarmh && uarmh->oartifact == ART_CLAUDIA_S_SEXY_SCENT && monstersoundtype(mtmp) == MS_STENCH) {

		if (yn("Hire this pretty lady for 1000 dollars?") == 'y') {

			(void) tamedog(mtmp, (struct obj *) 0, FALSE);
			u.ugold -= 1000;
			use_skill(P_SQUEAKING, rnd(5));
			return 1;

		}

    }

    if (u.smexyberries >= 1000 && !mtmp->mtame && mtmp->mnum != quest_info(MS_NEMESIS) && uarmf && uarmf->oartifact == ART_SMEXY_BERRIES && mtmp->female && mtmp->data->geno & G_UNIQ && (monstersoundtype(mtmp) == MS_BOSS || monstersoundtype(mtmp) == MS_FART_NORMAL || monstersoundtype(mtmp) == MS_FART_QUIET || monstersoundtype(mtmp) == MS_FART_LOUD) ) {

		if (yn("This lady wants to join your team. Do you want to tame her?") == 'y') {

			(void) tamedog(mtmp, (struct obj *) 0, FALSE);
			u.smexyberries = 0;
			use_skill(P_SQUEAKING, rnd(5));
			return 1;

		}

	}

    if ( (Role_if(PM_FEAT_MASTER) || Race_if(PM_VORTEX) || Race_if(PM_CORTEX)) && mtmp->m_lev <= 30 && mtmp->m_lev <= (u.ulevel + 7) && mtmp->data->mlet == S_VORTEX && !mtmp->mtame && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) ) {

		if (yn("Tame the vortex?") == 'y') {

		      pline("You attempt to tame %s.",mon_nam(mtmp) );
			(void) tamedog(mtmp, (struct obj *) 0, FALSE);
			return 1;
		}
	}

    if (Race_if(PM_KOP) && mtmp->m_lev <= 30 && mtmp->m_lev <= (u.ulevel + 10) && mtmp->data->mlet == S_KOP && mtmp->mpeaceful && !mtmp->mtame && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) && u.uhunger > 100 ) {

		if (yn("Recruit this officer of the law?") == 'y') {

		      pline("You convince %s to join your cause.",mon_nam(mtmp) );
			morehungry(100);

			(void) tamedog(mtmp, (struct obj *) 0, TRUE);
			use_skill(P_SQUEAKING, 1);

			return 1;
		}
	}

	if (((uarmf && itemhasappearance(uarmf, APP_FETISH_HEELS))) && (mtmp->data->mlet == S_NYMPH || monstersoundtype(mtmp) == MS_FART_NORMAL || monstersoundtype(mtmp) == MS_FART_QUIET || monstersoundtype(mtmp) == MS_FART_LOUD)) {

		if (yn("Pacify this monster?") == 'y') {
	      	pline("You attempt to pacify %s.",mon_nam(mtmp) );

			if (!mtmp->mfrenzied && mtmp->m_lev < rnd(50) && rn2(u.ulevel + 2) && (rn2(3) || ((rnd(30 - ACURR(A_CHA))) < 4)) ) {
		            mtmp->mpeaceful = 1;
				return 1;
			}
		}

	}

	if ((Role_if(PM_PROSTITUTE) || Role_if(PM_KURWA) || (uarmf && uarmf->oartifact == ART_VERY_PROSTITUTED) || autismweaponcheck(ART_SEXCALIBUR)) && humanoid(mtmp->data) ) {

		if (!(mtmp->mtame)) {

			if (u.homosexual == 0) {
				pline("You're discovering your sexuality...");
				getlin("Are you homosexual? [y/yes/no] (If you answer no, you're heterosexual.)", buf);
				(void) lcase (buf);
				if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) u.homosexual = 2;
				else u.homosexual = 1;
			}

			(void) doseduce(mtmp);
			return 1;
		}

	}

    if (Role_if(PM_LADIESMAN) && mtmp->m_lev <= 30 && mtmp->m_lev <= (u.ulevel + 10) && !flags.female && (mtmp->data->mlet == S_NYMPH || monstersoundtype(mtmp) == MS_NURSE || monstersoundtype(mtmp) == MS_SEDUCE || monstersoundtype(mtmp) == MS_WHORE) && !mtmp->mtame && !mtmp->mpeaceful && mtmp->mnum != quest_info(MS_NEMESIS) && !mtmp->mfrenzied && !(mtmp->data->geno & G_UNIQ) ) {

		if (yn("Seduce this pretty lady?") == 'y') {

		      pline("You attempt to seduce %s.",mon_nam(mtmp) );

	            mtmp->mpeaceful = 1; /* they will always become at least peaceful. --Amy */
	            set_malign(mtmp);

			/* higher level monsters are less likely to be affected --Amy */
			if (mtmp->m_lev < rnd(50) && rn2(u.ulevel + 2) && (rn2(3) || ((rnd(30 - ACURR(A_CHA))) < 4)) ) {
				(void) tamedog(mtmp, (struct obj *) 0, TRUE);
			}

			return 1;
		}
	}

	if (Race_if(PM_BABYLONIAN) && mtmp->mpeaceful && mtmp->m_lev <= 30 && mtmp->m_lev < (u.ulevel + 5) && !mtmp->mtame && mtmp->data->mlet == S_TURRET) {
		int stonescost = 10;
		if (mtmp->m_lev >= 1) stonescost = (mtmp->m_lev) * 10;
		pline("You can turn this turret into a symbiote by paying %d stones.", stonescost);
		if (yn("Do it?") == 'y') {
			register struct obj *prcstone;
			prcstone = carrying(ROCK);
			if (prcstone) {
				if (prcstone->quan > stonescost) {
					prcstone->quan -= stonescost;
					prcstone->owt = weight(prcstone);
					turnmonintosymbiote(mtmp, FALSE); /* WARNING: mtmp is removed at this point */
				} else if (prcstone->quan == stonescost) {
					useupall(prcstone);
					turnmonintosymbiote(mtmp, FALSE); /* WARNING: mtmp is removed at this point */
				} else {
					pline("Your main stack of stones isn't big enough!");
				}
			} else pline("You don't have any stones!");
			return 1;
		}
	}

    if (Race_if(PM_IMPERIAL) && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) && !mtmp->mpeaceful &&
     !mtmp->mtame && u.uhunger > 100) {

		if (yn("Use the Emperor's Voice?") == 'y') {

		      verbalize("%s", !rn2(3) ? "By the power of His Holiness Titus Medes, I beseech thee - stop thine combat actions!" : !rn2(2) ? "Long live Martin Septim! Thou shall surrender lest I smite thee!" : "The Emperor will spare thy life if thou stoppest fighting!");
			morehungry(100);
			use_skill(P_SQUEAKING, 1);

			/* higher level monsters are less likely to be affected --Amy */
			if (!mtmp->mfrenzied && mtmp->m_lev < rnd(50) && rn2(u.ulevel + 2) && (rn2(3) || ((rnd(30 - ACURR(A_CHA))) < 4)) ) {
		            mtmp->mpeaceful = 1;
		            set_malign(mtmp);
				pline("%s is convinced by your sermon, and no longer views you as an enemy!",l_monnam(mtmp));
			}

			return 1;

		}
    }

    if (uarmg && uarmg->oartifact == ART_VULTUREHANDLER && mtmp->data->mlet == S_BAT && !is_bat(mtmp->data) && !mtmp->mpeaceful && !mtmp->mfrenzied && !mtmp->mtame) {
		if (yn("Try to pacify the bird?") == 'y') {

		      pline("You try to calm %s.", mon_nam(mtmp) );

			if (!rn2(4)) {
				if (!resist(mtmp, RING_CLASS, 0, NOTELL)) {
					mtmp->mpeaceful = TRUE;
					pline("%s seems calmer.", Monnam(mtmp));
				}
			}

			return 1;

		}

    }

    if (Race_if(PM_MUMMY) && mtmp->mnum != quest_info(MS_NEMESIS) && mtmp->m_lev <= 30 && mtmp->m_lev <= (u.ulevel + 7) && !(mtmp->data->geno & G_UNIQ) &&
     !mtmp->mtame && (is_undead(mtmp->data) || mtmp->egotype_undead) && u.uhunger > 500) {

		if (yn("Try to tame this undead monster?") == 'y') {

		      pline("You frantically chant at %s.",mon_nam(mtmp) );
			morehungry(500);
			use_skill(P_SQUEAKING, rnd(5));

			/* higher level monsters are less likely to be affected --Amy */
			if (mtmp->m_lev < rnd(100) && rn2(u.ulevel + 2) && (rn2(3) || ((rnd(30 - ACURR(A_CHA))) < 4)) ) {

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

void
maybegaincha()
{
	if (ABASE(A_CHA) < 10) {
		int chachance = 100;
		switch (ABASE(A_CHA)) {
			case 1: chachance = 33; break;
			case 2: chachance = 66; break;
			case 3: chachance = 100; break;
			case 4: chachance = 150; break;
			case 5: chachance = 200; break;
			case 6: chachance = 250; break;
			case 7: chachance = 300; break;
			case 8: chachance = 350; break;
			case 9: chachance = 400; break;
			default: chachance = 500; break;
		}
		if (!rn2(chachance)) (void) adjattrib(A_CHA, 1, FALSE, TRUE);
	}
}

void
playermsconvert()
{
	int k, l;
	struct monst *mtmp3;

	int cost = 50;
	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	cost *= 9; cost /= 10; break;
	      	case P_SKILLED:	cost *= 8; cost /= 10; break;
	      	case P_EXPERT:	cost *= 7; cost /= 10; break;
	      	case P_MASTER:	cost *= 6; cost /= 10; break;
	      	case P_GRAND_MASTER:	cost *= 5; cost /= 10; break;
	      	case P_SUPREME_MASTER:	cost *= 4; cost /= 10; break;
	      	default: break;
		}
	}
	if (u.uen < cost) {
		You("lack the energy to use your conversion sermon! Need at least %d mana!", cost);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}
	u.uen -= cost;
	use_skill(P_SQUEAKING, 2);
	conversionsermon();

	for (k = -3; k <= 3; k++) for(l = -3; l <= 3; l++) {
		if (!isok(u.ux + k, u.uy + l)) continue;
		if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) && !is_infrastructure_monster(mtmp3) && mtmp3->mtame == 0 && mtmp3->mpeaceful == 0 && mtmp3->mfrenzied == 0 && mtmp3->isminion == 0 && mtmp3->isgyp == 0
		&& mtmp3->mnum != quest_info(MS_NEMESIS) && !(mtmp3->data->geno & G_UNIQ) )

		{
			if ( (!rn2(10) || monstersoundtype(mtmp3) == MS_CONVERT || monstersoundtype(mtmp3) == MS_HCALIEN) && !resist(mtmp3, RING_CLASS, 0, NOTELL)) {
				pline("%s is converted.", mon_nam(mtmp3));
				mtmp3->mpeaceful = 1;
			}

		} /* monster is catchable loop */
	} /* for loop */

}

void
playerwouwoutaunt()
{

	int cost = 100;
	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	cost *= 9; cost /= 10; break;
	      	case P_SKILLED:	cost *= 8; cost /= 10; break;
	      	case P_EXPERT:	cost *= 7; cost /= 10; break;
	      	case P_MASTER:	cost *= 6; cost /= 10; break;
	      	case P_GRAND_MASTER:	cost *= 5; cost /= 10; break;
	      	case P_SUPREME_MASTER:	cost *= 4; cost /= 10; break;
	      	default: break;
		}
	}
	if (u.uen < cost) {
		You("lack the energy to chant a wouwou taunt! Need at least %d mana!", cost);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}
	u.uen -= cost;
	use_skill(P_SQUEAKING, 4);
	wouwoutaunt();

	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		if (mtmp->mpeaceful || mtmp->mfrenzied || mtmp->mtame) continue;
		if(distu(mtmp->mx, mtmp->my) < 65) {
			if (!resist(mtmp, SCROLL_CLASS, 0, NOTELL)) {

				if (monstersoundtype(mtmp) == MS_CONVERT || monstersoundtype(mtmp) == MS_HCALIEN) {
					pline("%s is pacified.", mon_nam(mtmp));
					mtmp->mpeaceful = 1;
				} else {
					monflee(mtmp, rnd(10), FALSE, FALSE, FALSE);
					pline("%s turns to flee!", Monnam(mtmp));
				}

			}
		}
	}

}

void
playerwhoretalk()
{

	int cost = 100;
	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	cost *= 9; cost /= 10; break;
	      	case P_SKILLED:	cost *= 8; cost /= 10; break;
	      	case P_EXPERT:	cost *= 7; cost /= 10; break;
	      	case P_MASTER:	cost *= 6; cost /= 10; break;
	      	case P_GRAND_MASTER:	cost *= 5; cost /= 10; break;
	      	case P_SUPREME_MASTER:	cost *= 4; cost /= 10; break;
	      	default: break;
		}
	}
	if (u.uen < cost) {
		You("lack the energy to use your sexy talk ability! Need at least %d mana!", cost);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}
	u.uen -= cost;
	use_skill(P_SQUEAKING, 4);

	You("make very female noises!");
	{
		register struct monst *nexusmon, *nextmon;
		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
		    nextmon = nexusmon->nmon; /* trap might kill mon */
		    if (DEADMONSTER(nexusmon)) continue;
		    if (nexusmon->mpeaceful || nexusmon->mtame) continue;
		    if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;
		    if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;

		    if (distu(nexusmon->mx, nexusmon->my) > rnd(50)) continue;

			/* doesn't anger peaceful ones - not a bug --Amy */
			pline("%s cannot resist!", Monnam(nexusmon));
			monflee(nexusmon, rnd(20), FALSE, TRUE, FALSE);
			if (nexusmon->mblinded < 100) nexusmon->mblinded += 20;
			nexusmon->mcansee = 0;
			nexusmon->mstun = TRUE;
			nexusmon->mconf = TRUE;

			if (!resists_drain(nexusmon) && !resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) {
				pline("%s shudders in dread!", Monnam(nexusmon));
				nexusmon->mhpmax -= rnd(10);
				if (nexusmon->mhpmax < 1) nexusmon->mhpmax = 1;
				if (nexusmon->mhp > nexusmon->mhpmax) nexusmon->mhp = nexusmon->mhpmax;
				nexusmon->m_enmax -= rnd(10);
				if (nexusmon->m_enmax < 0) nexusmon->m_enmax = 0;
				if (nexusmon->m_en > nexusmon->m_enmax) nexusmon->m_en = nexusmon->m_enmax;
				if (nexusmon->m_lev > 0) nexusmon->m_lev--;
				if (nexusmon->m_lev > 0 && !rn2(4)) nexusmon->m_lev--;
			}

		}
	}

	if (practicantterror) {
		pline("%s rings out: 'Your irritating noises are disturbing the other practicants, so here's a warning for you to not do that again and I also receive 50 zorkmids from you.'", noroelaname());
		fineforpracticant(50, 0, 0);
	}

}

void
playersupermantaunt()
{

	int cost = 200;
	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	cost *= 9; cost /= 10; break;
	      	case P_SKILLED:	cost *= 8; cost /= 10; break;
	      	case P_EXPERT:	cost *= 7; cost /= 10; break;
	      	case P_MASTER:	cost *= 6; cost /= 10; break;
	      	case P_GRAND_MASTER:	cost *= 5; cost /= 10; break;
	      	case P_SUPREME_MASTER:	cost *= 4; cost /= 10; break;
	      	default: break;
		}
	}
	if (u.uen < cost) {
		You("lack the energy to make a superman taunt! Need at least %d mana!", cost);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}
	u.uen -= cost;
	use_skill(P_SQUEAKING, 10);
	supermantaunt();
	turn_allmonsters();

	if (practicantterror) {
		pline("%s booms: 'Stop screaming like a mad%s. You pay 500 zorkmids now, and better don't scare the other practicants like that again, maggot!'", noroelaname(), flags.female ? "woman" : "man");
		fineforpracticant(500, 0, 0);
	}


}

void
playerrattlebones()
{

	int cost = 25;
	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	cost *= 9; cost /= 10; break;
	      	case P_SKILLED:	cost *= 8; cost /= 10; break;
	      	case P_EXPERT:	cost *= 7; cost /= 10; break;
	      	case P_MASTER:	cost *= 6; cost /= 10; break;
	      	case P_GRAND_MASTER:	cost *= 5; cost /= 10; break;
	      	case P_SUPREME_MASTER:	cost *= 4; cost /= 10; break;
	      	default: break;
		}
	}
	if (u.uen < cost) {
		You("lack the energy to rattle your bones! Need at least %d mana!", cost);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}
	u.uen -= cost;
	use_skill(P_SQUEAKING, 1);
	You(FunnyHallu ? "play the xylophone." : "rattle noisily.");

	{
		register struct monst *nexusmon, *nextmon;
		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			nextmon = nexusmon->nmon; /* trap might kill mon */
			if (DEADMONSTER(nexusmon)) continue;
			if (!nexusmon->mcanmove) continue;
			if (rn2(10)) continue; /* low chance of success --Amy */
			if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;

			if (distu(nexusmon->mx, nexusmon->my) > rnd(26)) continue;

			wakeup(nexusmon); /* can anger peaceful monsters --Amy */
			nexusmon->mcanmove = 0;
			nexusmon->mfrozen = 2;
			nexusmon->mstrategy &= ~STRAT_WAITFORU;
			pline("%s freezes for a moment.", Monnam(nexusmon));

		}
	}

	if (practicantterror) {
		pline("%s rings out: 'Your irritating noises are disturbing the other practicants, so here's a warning for you to not do that again and I also receive 50 zorkmids from you.'", noroelaname());
		if (FunnyHallu) pline("%s adds: 'Better hope I won't confiscate your xylophone. This is a chemical laboratory, not a music hall.'", noroelaname());
		fineforpracticant(50, 0, 0);
	}

}

void
conversionsermon()
{
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
		"Oyuncu bir guevensiz! Onu Islam'a doenuestuerme zamani.",
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
		"Ben farkli degilim cuenkue bir bas oertuesue giyiyorum! Beni ayrimcilik yapmamalisin, kafir demek istiyorsun!",
		"Siz kafirler bizi kabul etmeli ve dininize uygun olmali! Kapsayici olmak istiyorsaniz, oegretmenlerin ve polis kadinlarinin ve digerlerinin halka acik bir bez kullanmasina izin veriyorsunuz!",
		"Beni 'basoertuesue kiz' sluruna duesuerme, seni lanet olasi kafir! Bu benim kimligimin bir parcasi ve bunu kabullenmek icin kanunla zorlanacaksin!",
		"Kafa bezi bize fransiz sigaralari gibi oezguerluek veriyor! Siz kafirler asla anlayamayacaksiniz cuenkue Tanri'ya inanmiyorsunuz!",
		"Yeme tabagim sacmalik degil! Sen pis kafir sadece domuz eti kirli oldugunu ve hic kimsenin tarafindan yenilmeyecegini anlamiyorum! Cuenkue yaparsan kansersin!",
		"Imam, kafir kueltueruenuen hicbir seyin degersiz oldugunu soeylueyor. Bu nedenle aptal kurallariniza uymak zorunda degiliz ve okul da ise yaramiyor, cuenkue birkac yil icinde hepiniz asimile olacaksiniz.",
		"Yuezmeye katilmak zorunda degilim! Kafir guecuen beni zorlamak istiyor ama kutsal Islam her zaman bir bas oertuesue giymem gerektigini soeylueyor ve bu da aptal yuezme derslerine uymuyor! Politikacilarina bogma tilsimindan bogulmalarini soeyle!",
		"Acil bir durum varsa, oence Allah'ta muesluemanlar ve inananlar kurtarilmalidir! Kafirler daha bueyuek iyilik icin kurban edilebilir!",
		"Inanilmaz lanet olasi guezel Islam'in duenyayi yoenetecegini anlamazsin.",
		"Ne, Almanya'nin alternatifi mueslueman avlamak mi istiyor? Yuece Allah adina, bu kafirleri dogrudan cehenneme goenderecek, cocuklarini oelduerecek ve kadinlarini koele tueccarlarina satacagiz.",
		"Kafa bezi sadece bir bez parcasi degildir! Onu takariz cuenkue bizi Allah'in kutsal mueminleri olarak tanimlar! Sadece kafirlerin anlamadigi icin hicbir anlam ifade etmez!",
		"Bir Yahudi dini semboluenue giydigi icin saldiriya ugrarsa, kendi hatasidir. Biz mueslueman bir kafa bezi giymek icin bize saldiran kimseyi asla kabul etmeyecegiz, cuenkue biz kafir topuklularimizla bir asyali tekmelemizi dogrudan kafir somunlarina yerlestirecegiz.",
		"Siz kafirsiniz, Allah'a doenmelisiniz cuenkue o zaman cennette doksan dokuz genc kadin var! Eger yapmazsan, cehennemin kirmizi seytanlari acimasizca iskence edecek!",
		"Halkimizin kutsal topraklara seyahat etmesine izin verilmemesi bir hakaret! Bueyuek Tanri, bu karardan sorumlu olan buetuen kafirleri cezalandirir!",
		"Kongremizdeki kadinlar suclu degil! Ancak baskan kirli bir kuefuer ve Tanri sahsen ona yildirim carpacak!",
		"Bir bildirimin ardindan geri doendueguemuez icin neden uezuelmemiz gerektigini duesuenueyorsunuz? Siz heretik, istediginizi aldiniz, cuenkue yuece Tanrimiz affetmez!",
		"Yasli ihtiyar bizim icadi olan doener kebabi yiyorsun, ama dinimiz hakkinda yalan soeylueyorsun! Tanrimiz seni mahvedecek!",
		"Ceviri makinesi kirli kafirler tarafindan isletiliyor, cuenkue tanri adina varsayilan ceviriyi degistirdiler!",
		"Benim konusmalarimi zaten anlamiyorsunuz, bu yuezden irkinizin tamamen ortadan kaldirilmasi icin cagri yapabilirim, sizi allahimiza inanmayan insafsizsiniz.",
		"Temizlik Allah'in guenue buetuen kafirler cezalandirmak ve ilk bu sapkin kadin olacak!",
		"Sen inanmayan basoertueguemue indiremezsin!",
		"Savas botlarim kaninizi kutsal Allah'a feda edecek, sizi kafir!",
		"Kafirler gelmeden tuvalete gitmem gerekiyor!",
		"Belki kafirleri erojen seslerle cezbedebilirim!",
		"Kafirlere arkadan saldirdigimizda Allah razi olur.",
		"Allah'in gazabindan hicbir kafir kacamaz.",
		"Allah'imiz, tuem kafirleri yok edecek.",
		"Geri cekilmek! Kafirlerin bizi yakalamasina izin vermeyin!",
		"Basoertueguemue caldin! Bunun icin bu kafir doerde boeluenmeli!",
		"Eger benim gibi bir Muesluemana kuefuer edersen, Allah onu yildirimla oeldueruer!",
		"Kutsal Allah'a kurban olacaginiz sunak hazir!",
		"Hicbir kafir Islam'i durduramaz!",
		"Yuece Allah, yardim et ve beni kafirlerden koru!",
		"Kafirler korku icinde titreyip Allah'in kutsal gazabina huekmedilsin.",
		"Basoertuesue takmayan tuem inancsiz kadinlar din degistirilmelidir!",
		"Evlilik oencesi cinsel iliski, yalnizca inanmayanlarin isledigi oeluemcuel bir guenahtir! Bunun icin Allah onlari oelduerecek!",
		"Allah onlara oeluem emrini verdigi icin, oegrencilerimizin geleneksel kiyafetlerini giymelerine izin vermedikleri icin buetuen kafirler oelecekler.",
		"Bizim kanunumuz Allah'in kanunudur! Hicbir duenyevi insanin, oezellikle de sapkinlarin neyin dogru oldugu konusunda soez hakki yoktur!",
		"Biz bu uelkeye aitiz ve aksini soeyleyen her kafir Allah'in gazabina ugrayacaktir! Bu bir gercek!",
		"Kamuoyunda basoertuesue takma hakkimizi savunmak icin kaya gibi dimdik duracagiz! Kafirler bize neyi giyip giyemeyecegimizi soeyleyemezler, cuenkue Allah bizden yanadir!",
	};
	verbalize("%s", conversion_msgs[rn2(SIZE(conversion_msgs))]);

}

void
wouwoutaunt()
{
	static const char *hcalien_msgs[] = {
		"Wouwou.",
		"Wouuu.",
		"Www-wouwou.",
		"Wwouwwouww.",
		"Wowou.",
		"Wwwouu.",
	};
	verbalize("%s", hcalien_msgs[rn2(SIZE(hcalien_msgs))]);

}

void
supermantaunt()
{
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

	verbalize("%s", superman_msgs[rn2(SIZE(superman_msgs))]);

}

void
prostitutetaunt()
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
		"Oh yeah baby, let's brawl.",
		"Think you're hard hitting a woman?",
		"You want my heel in your ass?",
		"This is gonna hurt!",
		"Ass-kicking runs in my family.",
		"How about a free face-lift?",
		"Watch those shoes, girl.",
		"Oh my god, watch the pumps!",
		"You're ruining the ambience.",
		"I love it when it gets violent.",
		"I'm totally spitting in your drink.",
		"Can you put lotion on the back of my leg?",
		"I can kick, you know.",
		"You fight like a girl.",
		"What are you wearing?!",
		"Let's scrap you, buster.",
		"Still sleepy, sister?",
		"I'm gonna slap you so hard you're gonna cry!",
		"Oh what's the matter baby, gonna cry?",
		"Now I have to get another panic here!",
		"You can't beat me. I'm empowered.",
		"Hitting me does not make you any less of a loser.",
		"Are you sorry now?",
		"Don't mess with a lifestyle doll.",
		"Accept your punishment!",
		"You can't hit me, I'm a lady!",
		"I'm gonna beat you just like I beat my children!",
	};

	verbalize("%s", whore_msgs[rn2(SIZE(whore_msgs))]);

}

void
principaltaunt()
{
	static const char *principal_msgs[] = {
		"What's up?",
		"I really feel sick - there are so many things to do!",
		"Help me, I faint!",
		"We'll do that in groups of one person!",
	};

	verbalize("%s", principal_msgs[rn2(SIZE(principal_msgs))]);
}

void
pupiltaunt()
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

	verbalize("%s", pupil_msgs[rn2(SIZE(pupil_msgs))]);

}

void
teachertaunt()
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

	verbalize("%s", teacher_msgs[rn2(SIZE(teacher_msgs))]);

}

/* which sound effect type is a monster using? this takes stuff like MS_COMMON into account --Amy */
int
monstersoundtype(mtmp)
register struct monst *mtmp;
{
	if (!mtmp) {
		impossible("monstersoundtype called with nonexistant monster!");
		return MS_SILENT; /* fail safe */
	}

	if (mtmp->data->msound == MS_COMMON) return u.commonsound;

	return mtmp->data->msound;

	return MS_SILENT; /* fail safe */
}

#endif /* OVLB */

/*sounds.c*/

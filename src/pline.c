/*	SCCS Id: @(#)pline.c	3.4	1999/11/28	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#define NEED_VARARGS /* Uses ... */	/* comment line for pre-compiled headers */
#include "hack.h"
#include "epri.h"
#ifdef WIZARD
#include "edog.h"
#endif

#ifdef OVLB

static boolean no_repeat = FALSE;

static char *FDECL(You_buf, (int));

#if defined(DUMP_LOG)
char msgs[DUMPMSGS][BUFSZ];
int msgs_count[DUMPMSGS];
int lastmsg = -1;
#endif

static NEARDATA const char * const fake_plines[] = {
	"Demogorgon suddenly disappears!",
	"You are slowing down.",
	"Your limbs are stiffening.",
	"Do you want your possessions identified? DYWYPI?",
	"Warning: Low Local Memory. Freeing description strings.",
	"Not enough memory to create inventory window",
	"Your potion of full healing boils and explodes!",
	"Your scroll of genocide catches fire and burns!",
	"Your ring of free action turns to dust and vanishes!",
	"Your wand of death (0:8) suddenly explodes!",
	"Your wand of wishing seems less effective.",
	"One of your objects was destroyed by rust!",
	"Your weapon withered away!",
	"It summons insects!",
	"The soldier ant stings you! The poison was deadly...",
	"It breathes --More-- You have died.",
	"You feel weaker!",
	"You undergo a freakish metamorphosis!",
	"Your armor turns to dust and is blown away!",
	"It stole an amulet of reflection.",
	"It summons demon lords!",
	"The Wizard of Yendor casts a spell at you!",
	"Death reaches out with his deadly touch...",
	"A Master Archon suddenly appears!",
	"Oh no, it uses the touch of death!",
	"You feel deathly sick.",
	"You don't feel very well.",
	"You hear the cockatrice's hissing!",
	"A little dart shoots out at you! The dart was poisoned!",
	"You hear a clank.",
	"Oh wow! Everything looks so cosmic!",
	"You stagger and your vision blurrs...",
	"Eating that is instantly fatal.",
	"Falling, you touch the cockatrice corpse.",
	"It seduces you and you start taking off your boots of levitation.",
	"A wide-angle disintegration beam hits you!",
	"You die...",
	"***LOW HITPOINT WARNING***",
	"You hear the wailing of the Banshee...",
	"Wizard is about to die.",
	"Oops... Suddenly, the dungeon collapses.",
	"You turn to stone.",
	"You are a statue.",
	"You have become a green slime.",
	"Your skin begins to peel away.",
	"You die of starvation.",
	"You faint from lack of food.",
	"You can't move!",
	"You fall asleep!",
	"You suddenly feel weaker! 10 points of strength got sapped!",
	"You float into the air!",
	"Wait! The door really was an intrinsic eating mimic!",
	"Remember - ascension is impossible!",
	"You don't stand a chance!",
	"Weaklings like you won't ever ascend, so don't even think you can do it!",
	"YASD is waiting around the corner with big, pointy teeth!",
	"You have 35 turns to live.",
	"If you don't ascend within the next 10000 turns, you automatically die.",
	"Your last thought fades away.",
	"You feel dead inside.",
	"Unfortunately, you're still genocided.",
	"Blecch! Rotten food!",
	"Ecch - that must have been poisonous!",
	"You get a strong feeling that the gods are angry...",
	"The voice of Moloch roars: 'Destroy him, my servants!'",
	"Asidonhopo hits you! Asidonhopo hits you! Asidonhopo hits you! You die...",
	"Vanzac screams: 'How dare you break my door?' Vanzac zaps a hexagonal wand!",
	"The gnome zaps a wand of death!",
	"The soldier zaps a wand of remove resistance!",
	"The giant spider zaps a wand of banishment!",
	"The black dragon breathes a disintegration blast!",
	"The giant eel drowns you... Do you want your possessions identified? DYWYPI? [ynq] (n) _",
	"Really quit [yes/no]? _",
	"Suddenly, one of the Vault's guards enters!",
	"You fall into the lava! You burn to a crisp.",
	"You die from your illness.",
	"The newt bites you! You die...",
	"You hit the floating eye! You are frozen by the floating eye!",
	"The porter lethe vortex engulfs you!",
	"You feel the waters of the Lethe sweeping away your cares...",
	"The mind flayer's tentacles suck you! Your brain is eaten! You feel very stupid!",
	"The disenchanter claws you! Your silver dragon scale mail seems less effective.",
	"Your silver saber named Grayswandir glows violently for a while and then evaporates.",
	"Grateful for her release, she grants you a wish! For what do you wish?",
	"The djinni speaks. 'You disturbed me, fool!'",
	"An enormous ghost appears next to you! You are frightened to death, and unable to move.",
	"The priest intones: 'Pilgrim, you enter a desecrated place!'",
	"h - a cursed -5 unicorn horn.",
	"The troll giant rises from the dead!",
	"The barbarian named Mike the Raider wields a long sword named Vorpal Blade!",
	"One-Eyed Sam swings his long sword named Thiefbane. One-Eyed Sam hits you!",
	"Ashikaga Takauji picks up a tsurugi named The Tsurugi of Muramasa.",
	"The maud agent hurls a sparkling potion!",
	"The bone devil zaps a wand of create horde!",
	"The bolt of fire bounces! The bolt of fire hits you! The ice underneath you melts!",
	"You irradiate yourself with pure energy. You die...",
	"You're confused!",
	"You just blinded yourself!",
	"The thin crust of rock gives way, revealing lava underneath!",
	"CLICK! You have triggered a trap!",
	"All your possessions have been stolen!",
	"The beautiful water nymph seduces you and you start taking off your cloak.",
	"The blast of disintegration hits you!",
	/* The statu */ "es come to life!",
	/* The statues */ "come to life!",
	/* The Smirking Sne */ "ak Thief is dead!",
	/* You impale th */ "e Pale Wraith with a lightning stroke.",
	/* Undistributed points a */ "vailable, exit anyway?",
	/* This is a fake messa */ "ge broadcast by Arabella to confuse unwary players.",
	/* Castle of the Winds will only r */ "un in Protect mode",
	/* pul */ "verisiert!",
	/* Not eno */ "ugh memory to create inventory window",
	/* Com */ "mand Pending: Magic Spell...",
	/* integer div */ "ide by 0",
	/* Jetzt wirds sc */ "hwer!",
	"Arabella began to build a world miracle!", 
	"Arabella finished building a world miracle. You will lose if it exists for 2000 turns!",
	"Your head is sucked off! You die...",
	"You choke over your food. You die...",
	"You bite the gnome lord. You feed on the lifeblood. You choke over it.",
	"You fall into a pit! You land on a set of sharp iron spikes! The spikes were poisoned!",
	"You triggered a teleporting items trap!",
	"You feel life has clocked back.",
	"Your stats got scrambled!",
	"As you put a wand of cancellation inside, your bag named sack suddenly explodes!",
	"You triggered a bisection trap! The razor-sharp blade cuts you in half! You die...",
	"KABOOM! The door was booby-trapped! You stagger...",
	"KAABLAMM! You triggered a land mine!",
	"It misses.",
	"It hits! [-35 -> 1] Warning: HP low!",
	"You hear a flinging sound. You are hit by an arrow!",
	"A boss monster suddenly appears!",
	"It summons demons!",
	"It summons demon princes!",
	"It summons something invisible!",
	"You slow down to a crawl.",
	"You slow down.",
	"You are prevented from having reflection!",
	"You are prevented from having magic resistance!",
	"You feel your life draining away!",
	"You feel weaker!",
	"You feel clumsy!",
	"You feel stupid!",
	"You feel foolish!",
	"You feel very sick!",
	"You break out in hives!",
	"All your stats have been drained!",
	"You're not as agile as you used to be...",
	"You dream that you hear the howling of the CwnAnnwn...",
	"Rien ne va plus...",
	"SLASHEM.EXE has stopped working.",
	"nt|| - Not a valid save file",
	"NETHACK.EXE caused a General Protection Fault at address 000D:001D.",
	"APPLICATION ERROR - integer divide by 0",
	"Runtime error! Program: NETHACK.EXE",
	"R6025 - pure virtual function call",
	"Buffer overrun detected! Program: NETHACK.EXE",
	"A buffer overrun has been detected which has corrupted the program's internal state. The program cannot safely continue execution and must now be terminated.",
	"This application has requested the Runtime to terminate it in an unusual way. Please contact the application's support team for more information.",
	"Not enough memory to create inventory window",
	"Error: Nethack will only run in Protect mode",
	"ERROR: SIGNAL 11 WAS RAISED",
	"Not enough memory to initialize NetHack",
	"UNHANDLED EXCEPTION: ACCESS_VIOLATION (C0000005)",
	"An error has occurred in your application. If you choose Close, your application will be terminated. If you choose Ignore, you should save your work in a new file.",
	"Windows Subsystem service has stopped unexpectedly.",
	"nv4_disp.dll device driver is stuck in an infinite loop.",
	"You hear a nearby zap. You feel dizzy!",
	"The goblin zaps a wand of disintegration! Your cloak of magic resistance is blown away!",
	"The ogre reads a scroll of destroy armor! Your gloves vanish!",
	"You feel the air around you grow charged...",
	"You feel very jumpy.",
	"Got aggravate monster!",
	"Got polymorphitis!",
	"You're insane if you think you can ascend!",
	"Message to the person sitting in front of the monitor: you will not escape this time.",
	"This is Arabella speaking to you, player. Know this: I put nasty traps on every dungeon level and you will not be able to avoid all of them.",
	"This is Arabella speaking to you, player. Know this: Some parts of the dungeon have been designed by me specially to make sure you won't win the game.",
	"This is Arabella speaking to you, player. Know this: Marc is the only one who ever managed to defeat me in combat. You will not change that fact.",
	"This is Arabella speaking to you, player. Know this: Even if you're not a Spacewars Fighter or Camperstriker, I can still turn your life into hell.",
	"This is Arabella speaking to you, player. Know this: I own the Bank of Yendor and I decided not to give you back any money you deposit.",
	"This is Arabella speaking to you, player. Know this: If you like this game, consider playing the latest up-to-date version.",
	"This is Arabella speaking to you, player. Know this: If you lose to me, you have to clean the dog shit off of my profiled boots.",
	"This is Arabella speaking to you, player. Know this: I'm working hard to invent more nasty traps with which to obstruct the players.",
	"Incoming message for Player 1. The message is from 'Marc'. It reads: --More--",
	"Incoming message for Player 1. The message is from 'Sizzle'. It reads: 'Roger that.'",
	"Incoming message for Player 1. The message is from <sender name is illegible>. It reads: 'I am the world champion of SPACE WARS but I'm not playing anymore for fear of losing my title.'",
	"Incoming message for Player 1. The message is from 'Anastasia'. It reads: 'Sorry, but I have to kill you with my vacuum cleaner now.'",
	"Incoming message for Player 1. The message is from 'Henrietta'. It reads: 'Besos y fuderes, Henrietta, xxx. PS: GO TO HELL!!!'",
	"Incoming message for Player 1. The message is from 'Jana'. It reads: 'You won't be able to beat the game unless you kill me, and I'm hiding on dungeon level 71. You will never even find me.'",
	"Incoming message for Player 1. The message is from 'Vlad the Impaler'. It reads: 'Nyah-nyah, I bet you'll never find out which dungeon level I teleported to! Nyah-nyah!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Fear my wrath, ass-fucker!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Verily, thou shalt be one dead piece of shit!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'I will eat thee alive, startscummer!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Thine carcass shalt burn, pudding farmer!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Thou art doomed, cunt!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Hell shall soon claim thy remains, gay fag!'",
	"Incoming message for Player 1. The message is from 'Wizard of Yendor'. It reads: 'Even now thy life force ebbs, dirtbag!'",
	"Incoming message for Player 1. The message is from 'Irina'. It reads: 'Failed existences are those who failed in my test. They are not admitted into my exclusive circle.'",
	"Incoming message for Player 1. The message is from 'Kylie Lum'. It reads: 'Apply as a topmodel and join my show! If you're good then I'll make you famous!'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'Thank you for playing SLASH'EM Extended!'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'Need help? Come join me in IRC channel #slashemextended!'",
	"Incoming message for Player 1. The message is from 'Bluescreen'. It reads: 'When starting a new game, please always use the latest version. Older ones are bugged and will eventually be removed from the server.'",
	"Incoming message for Player 1. The message is from 'Famine'. It reads: 'I am waiting to eat you on the Astral Plane, adventurer.'",
	"Incoming message for Player 1. The message is from 'Death'. It reads: 'Face me on the Astral Plane if you're a man.'",
	"Incoming message for Player 1. The message is from 'Pestilence'. It reads: 'I will make sure you never reach the High Altar on the Astral Plane.'",
	"Incoming message for Player 1. The message is from 'Moloch'. It reads: 'Thou shalt suffer, infidel!'",
	"Incoming message for Player 1. The message is from 'High Priest of Moloch'. It reads: 'Infidel, you have entered Moloch's Sanctum! Be gone!'",
	"Incoming message for Player 1. The message is from 'Vecna'. It reads: 'I live, and if you decide to fight me then you will join my legion of undead!'",
	"Incoming message for Player 1. The message is from 'Vecna'. It reads: 'The warmth of life has entered my tomb. Prepare yourself, mortal, to serve me for eternity!'",
	"Incoming message for Player 1. The message is from 'Vecna'. It reads: 'Abandon your foolish quest! All that awaits you is the wrath of my master! You are too late to save the child. Now you'll join him - in hell.'",
	"Incoming message for Player 1. The message is from 'Miriam'. It reads: 'I would loooooooooooooove to stomp your sweet, unprotected toes with my high-heeled combat boots.'",
	"Incoming message for Player 1. The message is from 'The Prisoner'. It reads: 'SOS! Please, someone help me! I'm being tortured one level down from your location!'",
	"Incoming message for Player 1. The message is from 'Lord Surtur'. It reads: 'You won't defeat me. I'm covetous, so I can just warp away if I'm low on health.'",
	"Incoming message for Player 1. The message is from 'Final Boss'. It reads: 'oremoR nhoJ em llik tsum uoy emag eht niw oT'",
	"Incoming message for Player 1. The message is from an anonymous watcher. It reads: 'still havent ascended? lol u suk hahahahahah'",
	"Incoming message for Player 1. The message is from an anonymous watcher. It reads: 'i bet i can play better than you'",
	"Incoming message for Player 1. The message is from an anonymous watcher. It reads: 'use the force luke!'",
	"You got an item! Fuckshitting cuntdicks!",
	"PLAYER DRUG BAG FUCK",
	"The Ho-oh stole a blessed greased fixed +5 silver dragon scale mail.",
	"It seduces you and you start to take off your boots. You fall into the water! You sink like a rock.",
	"You fall into the water! You sink like a rock.",
	"You fall into the lava, but it only burns slightly!",
	"Izchak gets angry! You miss Izchak. Izchak zaps an orthorhombic wand!",
	"You feel weak now.",
	"You feel drained...",
	"A malevolent black glow surrounds you...",
	"You lack the strength to cast spells.",
	"You are physically incapable of picking anything up.",
	"Off - you just blinded yourself!",
	"You split open your skull on the ground. You die...",
	"An arrow shoots out at you! You are hit by an arrow!",
	"BANG! A gun shoots at you! The bullet hits your head! This is instantly fatal.",
	"The marilith hits you! The marilith hits you! The marilith opens a gate! The marilith hits you! The marilith hits you! The marilith hits you! The marilith opens a gate! The marilith hits you!",
	"The hill orc zaps a wand of create horde!",
	"You wrest one final charge from the worn-out wand. The wand of create horde turns to dust.",
	"You miss it. It gets angry! You hear a nearby zap. The lightning bolt hits you!",
	"oy||",
	"P|<||",
	"Your computer has been running for too long and is now going to shut down.",
	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
	"Things that are here:  a potion  a potion  a potion  a potion  a potion  a potion--More--",
	"This fortune cookie tastes terrible! There is message in the cookie. It says:--More--",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'suk me hahahahahah'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'Nobody will ever ascend SLASH'EM Extended.'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'Have a nice trip! --The local acid supplier.'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'You will get instakilled in 349 turns.'",
	"This fortune cookie tastes terrible! There is message in the cookie. It says: 'Air current noises are sexy!'",
	"You discover a construction, behind which there is a sexy girl waiting... but you can only see her butt. It seems that her name is Kati.",
	"You can't resist the temptation to caress Elena's sexy butt.",
	"Katharina produces beautiful farting noises with her sexy butt. The world spins and goes dark.",
	"Using her sexy butt, Karin produces beautiful noises and craps right in your face.",
	"You hear tender crapping noises.",
	"Kerstin produces beautiful crapping noises with her sexy butt. You just can't believe that someone could produce such erotic noises, and are immobilized by your feelings.",
	"You hear a soft click. The disintegration beam hits you!",
	"A trap door in the ceiling opens and a rock falls on your head!",
	"A board beneath you squeaks loudly.",
	"You stepped on a trigger! Your ears are blasted by hellish noise!",
	"You stepped on a trigger! Your ears are blasted by hellish noise! Your cyan potion freezes and shatters!",
	"You stepped on a trigger! Your ears are blasted by hellish noise! Your potion of polymorph freezes and shatters!",
	"A bear trap closes on your feet!",
	"You are enveloped in a cloud of gas!",
	"You are frozen in place!",
	"You are sprayed with acid! Your plate mail corrodes!",
	"You are sprayed with acid! Your weapon got vaporized!",
	"You fell into a pool of water!",
	"You stepped on a trap of wishing! Unfortunately, nothing happens.",
	"From the dark stairway to hell, demons appear to surround you!",
	"You are pulled downwards...",
	"The giant eel drowns you...",
	"A vivid purple glow surrounds you...",
	"You are momentarily illuminated by a flash of light!",
	"You feel less protected!",
	"You murderer!",
	"You hear the rumble of distant thunder...",
	"You hear a clank.",
	"You hear a thud.",
	"You hear a thud. You hear a thud.",
	"You hear a ripping sound.",
	"You hear a chugging sound.",
	"You hear a nearby zap.",
	"You hear a blast.",
	"You hear some noises.",
	"You hear some noises in the distance.",
	"You have a sad feeling for a moment, then it passes.",
	"You feel worried about Idefix.",
	"You feel your magical energy dwindle to nothing!",
	"You feel your magical energy dwindling rapidly!",
	"You feel a little apathetic...",
	"You feel like you could use a nap.",
	"You have been playing for long enough now, so the RNG decides to kill your character.",
	"Suddenly, rocks fall and everyone dies. Do you want your possessions identified? DYWYPI? [ynq] (n) _",
	"Your purse feels lighter...",
	"Your belongings leave your body!",
	"You hear a chuckling laughter.",
	"You hear a chuckling laughter. You feel a little sick!",
	"The RNG determines to take you down a peg or two...",
	"You undergo a freakish metamorphosis!",
	"You feel a little strange. You turn into a black naga! You break out of your armor!",
	"Suddenly, there's glue all over you!",
	"Your position suddenly seems very uncertain!",
	"You suddenly get teleported!",
	"You are getting confused.",
	"You stagger...",
	"You stepped into a heap of shit!",
	"You feel yourself yanked in a direction you didn't know existed!",
	"The magic cancellation granted by your armor seems weaker now...",
	"You're hit by a massive explosion!",
	"A cloud of noxious gas billows out at you. The gas cloud was poisoned!",
	"You are jolted by a surge of electricity! Your wand of lightning explodes!",
	"You start turning to stone!",
	"Your weapon slips from your hands.",
	"You feel feverish.",
	"It seems a little more dangerous here now...",
	"You hear a mumbled incantation.",
	"You feel that monsters are aware of your presence.",
	"Thinking of Maud you forget everything else.",
	"Eek - you stepped into a heap of sticky glue!",
	"You are irradiated by pure mana!",
	"KAABLAMM!!!  You triggered a bomb!",
	"Click! You trigger a rolling boulder trap!",
	"You are decapitated by a guillotine! You die...",
	"Rocks fall all around you! Your elven leather helm does not protect you.",
	"You suddenly feel a surge of tension!",
	"You feel the arrival of monsters!",
	"You feel a colorful sensation!",
	"Cyanide gas blows in your face!",
	"You hear the roaring of an angry bear!",
	"You are enveloped by searing plasma radiation!",
	"One of your objects withered away!",
	"A potion explodes!",
	"One of your objects was destroyed by rust!",
	"WARNING: A Scud Storm has been launched.",
	"WARNING: Nuclear missile silo detected.",
	"WARNING: Level ends here!",
	"WARNING: Not enough memory for objects. Deleting objects from player's inventory to free space...",
	"WARNING: too many spells memorized. Resetting player's spell list.",
	"WARNING: array_playerintrinsics too large, deleting intrinsic_teleport_control",
	"WARNING: player is having too much luck. Increasing random monster spawn difficulty by 50...",
	"WARNING: Demogorgon was spawned on this level!",
	"WARNING: Assertion failure.",
	"WARNING: out_of_memory, emergency termination",
	"WARNING: system integrity has been damaged, initiating system halt",
	"WARNING: This version of SLASH'EM Extended is outdated. Please consider getting an up-to-date version.",
	"Arabella casts world fall. You die... Do you want your possessions identified? DYWYPI? [ynq] (n) _",
	"Anastasia uses a vacuum cleaner on you! Or is that a gluon gun? Anastasia sucks your head off! You die...",
	"Henrietta shoots 9 bullets! The 1st bullet hits you! The 2nd bullet hits you! The 3rd bullet hits you! Warning: HP low! The 4th bullet hits you!",
	"Katrin casts a spell at you! Suddenly, you're frozen in place! Katrin hits you! Katrin kicks you! You hear the wailing of the Banshee...",
	"Jana was hidden under aXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX--More-- Oops... Suddenly, the dungeon collapses. Error save file being written.",
	"Unreliable overflow, ignoring",
	"Ignoring unreliable datagram for player 1, would overflow",
	"Assertion Failure - Location: src/role.c, line 666, Expression: player_canascend == 0",
	"Halt - unrecoverable internal error 0035ffbc",
	"Bad syntax in options/player1/nethackrc.rc, line 0: OPTIONS=rest_on_space",
	"Monster master_mind_flayer stuck in wall - level design error",
	"Bad message string 'UHH! UHH! OH! OH! YES! IN! OUT! IN! OUT! DAMN! IS! THAT! YOUR! MOTHER! I! WILL! BLOW! A! GRENADE! UP! YOUR! SHIT! ASS! SHIT! CHARLIE!'",
	"Error - model potion_of_full_healing.jpg not found",
	"Could not load minor_demon_spawn_list(). Spawning Demogorgon instead.",
	"Fatal error: function void increase_player_luck() returning a non-zero value.",
	"Fatal error: user is trying to defy the wrath of the RNG. Calling god_zapsplayer() now.",
	"SLKJEWIUOIOSDGIOPSDGHIOPWEIOPWPHJBSDJLEJAKDYWYPIWELTJKEWZLKJWJEKLZ",
	"You have mail from user 'jonadab'! Read it? [yes/no]? _",
	"You have mail from user 'Bluescreen'! Read it? [yes/no]? y It reads: --More--",
	"You have mail from user 'kerio'! Read it? [yes/no]? The mail was covered with contact poison! You die...",
	"On salue, pomme de terre inconnue. to meet your kind here. You sure heard of the tragedy that befell Tristram--More-- format c:",
	"Can't write to save file",
	"Autosave failed! Reason: not enough free disk space",
	"It seduces you and you pull down your pants.",
	"It murmurs in your ear while helping you undress. You're naked.",
	"She gently massages your unprotected nuts, but suddenly pushes her knee right into your groin! OUCH!",
	"The incubus caresses your soft breasts, but all of a sudden he starts cutting them up with a knife!",
	"Mariya moves her sexy butt right in front of your nose and produces disgusting farting noises. You can't resist the temptation to inhale deeply...",
	"You suddenly feel desperate, so you grab a sexy leather pump and start to scratch up and down your own legs with the razor-sharp high heel.",
	"You are losing blood!",
	"You are bleeding out from your stinging injury!",
	"You flinch!",
	"Beware, there will be no return! Still climb? [ynq] (y) _",
	"You hear a laughing voice announce: 'Wimpy player, better go back to playing Pac-Man!'",
	"You hear a laughing voice announce: 'Eh, still thinking you can beat this game?'",
	"You hear a laughing voice announce: 'It's fun to watch you play because bad stuff happens to you all the time.'",
	"You hear a laughing voice announce: 'Please stay alive for a while so your inevitable YASD will be more fun!'",
	"You hear a laughing voice announce: 'I'm watching your game. And judging from what I've seen so far, I'm sure you'll die in, let's say, 500 turns.'",
	"You hear a laughing voice announce: 'Did you know there is a gnome with the wand of death on this level?'",
	"You hear a laughing voice announce: 'I am watching you...'",
	"SLASH'EM Extended Notification Service: Someone opened the ttyrec of your current game.",
	"SLASH'EM Extended Notification Service: Your amount of public watchers has increased by 1.",
	"SLASH'EM Extended Notification Service: You just got pinged in IRC channel #slashemextended.",
	"SLASH'EM Extended Notification Service: The ttyrec of your current run will be saved here:--More--",
	"SLASH'EM Extended Notification Service: User 'Bluescreen' invites you to join IRC channel #slashemextended!",
	"SLASH'EM Extended Notification Service: If you die now, you will be #1 on the high-score list!",
	"SLASH'EM Extended Notification Service: A person watching your game is trying to send you mail.",
	"SLASH'EM Extended Notification Service: Please make sure you're playing the most recent, up-to-date version of the game.",
	"You hear crashing rock.",
	"The dungeon acoustics noticeably change.",
	"You hear bubbling water.",
	"You hear water falling on coins.",
	"You hear the splashing of a naiad.",
	"You hear a slow drip.",
	"You hear a gurgling noise.",
	"You hear the tones of courtly conversation.",
	"You hear a sceptre pounded in judgment.",
	"You hear mosquitoes!",
	"You smell marsh gas!",
	"You hear screaming in the distance!",
	"You hear a faint whisper: \"Please leave your measurements for your custom-made coffin.\"",
	"You hear a door creak ominously.",
	"You hear hard breathing just a few steps behind you!",
	"You hear dragging footsteps coming closer!",
	"You hear anguished moaning and groaning coming out of the walls!",
	"You hear mad giggling directly behind you!",
	"You smell rotting corpses.",
	"You smell chloroform!",
	"You feel ice cold fingers stroking your neck.",
	"You feel a ghostly touch caressing your face.",
	"You feel somebody dancing on your grave.",
	"You feel something breathing down your neck.",
	"You feel as if the walls were closing in on you.",
	"You just stepped on something squishy.",
	"You smell horse dung.",
	"You hear someone counting money.",
	"You hear soft splashing.",
	"You hear gutteral voices chanting.",
	"You hear frenzied splashing.",
	"You hear someone scream: 'No! Stop! The Knife! Aaaaaahhhh!'",
	"You hear someone searching.",
	"You hear gently lapping water.",
	"You hear somebody screaming for help!",
	"You hear the footsteps of a guard on patrol.",
	"You hear a low buzzing.",
	"You hear an angry drone.",
	"You suddenly realize it is unnaturally quiet.",
	"An enormous ghost suddenly appears before you!",
	"You hear blades being honed.",
	"You hear loud snoring.",
	"You hear dice being thrown.",
	"You hear something about streptococus.",
	"You smell chloroform nearby.",
	"You hear someone cursing viruses.",
	"You hear leaves rustling.",
	"You hear a female voice singing a song.",
	"You catch a glimpse of sunlight.",
	"You hear mandibles clicking.",
	"You notice webs everywhere.",
	"You notice little openings in the floor.",
	"You hear a grunting noise.",
	"You notice a horrible smell.",
	"You hear loud footsteps going up and down the halls.",
	"You trip over a stinking pile of shit!",
	"You suddenly see a huge monster appearing right in front of you!",
	"You listen to a conversation.",
	"You hear people talking.",
	"You hear someone drawing a weapon.",
	"You sense that you are not alone...",
	"You hear a glass of wine breaking!",
	"You listen to a radio broadcast: For the next five days, the dungeon's estimated temperature high is 96 degrees. Now, is that in Fahrenheit or Kelvin?",
	"You hear rattling noises.",
	"You think you saw an object move on its own.",
	"You feel that something weird may be just around the corner.",
	"You hear the deafening noise of a metal object breaking apart!",
	"A wild WARMECH appeared!",
	"You notice the sound of clinging coins.",
	"You feel that, somewhere down here, there might be a whole lot of gold.",
	"You hear metal banging against a wall.",
	"You hear the sound of wispy wings.",
	"You can barely hear a whispering voice.",
	"You sense a glow of radiance.",
	"You think you just heard God speaking to you!",
	"You suddenly feel a wave of heat.",
	"You notice some heaps of earth.",
	"You sense a strong gust of wind.",
	"You hear a wave of water.",
	"You feel weirded out.",
	"You sense something strange about this place.",
	"You think you just saw something move.",
	"You think you're seeing white rabbits!",
	"You feel totally down, like you're on a bad trip!",
	"You have a weird feeling of chills and excitement.",
	"You sense that something isn't right here...",
	"You shudder in expectation.",
	"You hear the trickle of water.",
	"You sense a watery smell mixed with grass.",
	"You listen to an unusual ambient sound.",
	"You hear a fountain crash through solid rock with a mighty roar!",
	"You admire the beautiful mountains around here. Strange, how can mountains be in an underground dungeon?",
	"The killer bee stings you!",
	"The migo queen's tentacles suck you!",
	"It hits! Its tentacles suck you! You feel very stupid!",
	"The master mind flayer drinks a potion of invisibility. Suddenly you cannot see the master mind flayer.",
	"You feel aggravated at the invisible master lich.",
	"The rust monster eats an athame named Magicbane!",
	"The gelatinous cube eats a scroll of wishing!",
	"The rock mole eats a blessed stone of magic resistance!",
	"The ghast eats a black dragon corpse!",
	"You hear an awful gobbling noise!",
	"You hear a crunching sound.",
	"You hear a grating sound.",
	"You feel a certain tension.",
	"You feel a certain tension. This message was stolen from ADOM.",
	"You sense a certain tension.",
	"You sense a certain tension. Perhaps you should head for the nearest toilet...",
	"You sense a certain tension. That dreaded Dungeon Master probably filled the next room with arch-liches, disenchanters and master mind flayers.",
	"You sense a certain tension. That dreaded Dungeon Master probably filled the next room with black dragons, green slimes and invisible cockatrices.",
	"You sense a certain tension. That dreaded Dungeon Master probably filled the next room with luck suckers, Mothers-in-Law and questing beasts.",
	"It casts a spell!",
	"Something casts a spell at you!",
	"Something casts a spell at thin air!",
	"It breathes--More--",
	"You hear an exhaling sound.",
	"You hear a flinging sound.",
	"You hear a spitting sound.",
	"You are hit by a dagger!",
	"You are hit by a bullet! You are hit by a bullet! You are hit by a bullet! You are hit by a bullet!",
	"You are hit by an elven arrow! You are hit by an elven arrow! You are hit by an elven arrow!",
	"You are hit by an elven arrow! You are hit by an elven arrow! Elf is about to die.",
	"You are hit by a shuriken!",
	"You are hit by a dart! The dart was poisoned!",
	"The flagon crashes on your head and breaks into shards. Oh wow! Everything looks so cosmic!",
	"You hear a hurling sound.",
	"You hear a disenchanter reading a scroll labeled RAMBA ZAMBA.",
	"The maud agent hurls a sparkling potion! The carafe crashes on your head and breaks into shards.",
	"You hear a giant eel reading a scroll of create monster.",
	"The gnome drinks a potion of polymorph! The gnome turns into a hru!",
	"The lizard drinks a potion of polymorph! The lizard turns into a praetor!",
	"The killer bee drinks a potion of polymorph! The killer bee turns into a gorgon fly!",
	"It gazes directly at you! You are slowing down.",
	"You hear a sound reminiscent of an elephant stepping on a peanut.",
	"You hear a sound reminiscent of a seal barking.",
	"The ice crackles and melts. You fall into the water! You sink like a rock.",
	"You hear someone cursing shoplifters.",
	"You hear the chime of a cash register.",
      "You hear someone complaining about the prices.",
      "Somebody whispers: \"Food rations? Only 900 zorkmids.\"",
      "You feel the presence of some sort of force.",
      "There seems to be something special about this place.",
      "You hear someone say: \"Force field activated!\"",
      "You hear someone say: \"Warp panel test sequence is starting in three... two... one...\"",
	"It ululates! You kill it! You hear the studio audience applaud!",
	"The apostroph golem honks! You kill the dissolved undead potato! You hear the studio audience applaud!",
	"\"I've killed hundreds of pigs like you!\"",
	"\"You just made me break a nail!\"",
	"\"I'll kick your nuts flat!\"",
	"\"YOU DON'T STAND A CHANCE!\"",
	"\"YOU WILL DIE A SLOW, SLOW, DEATH...\"",
	"\"COME OUT! WE WILL HURT YOU!\"",
	"\"Your madness ends here, betrayer!\"",
	"\"You're not gonna ascend, puny mortal!\"",
	"\"Run, or die!\"",
	"\"Somebody call a medic!\"",
	"\"You call yourself a human being. I call you a pain in the butt, that's what I call you.\"",
	"\"Repent, and thou shalt be saved!\"",
	"\"*In a bored, monotone voice* Repent, and thou shalt be saved.\"",
	"\"At least you aren't that Mario guy. He demands I bring him back EVERY SINGLE TIME he dies.\"",
	"\"Die monster, you don't belong in this world!\"",
	"\"I am not burdened by any sort of genitals! Jealous you must be!\"",
	"\"I have HAD it with these motherfucking mortals on this motherfucking astral plane!\"",
	"\"I'm gonna LOVE and TOLERATE the SHIT out of you!\"",
	"\"Lawful is easy, I'll show you why!\"",
	"\"Stop right there criminal scum! Nobody breaks the law on my watch!\"",
	"\"Begone, and do not return!\"",
	"\"I will be rude to you if you disturb my work!\"",
	"\"You don't affect me. The trashy strolling is an eyesore.\"",
	"\"You have gained monaters from graduate school, but you are so weak. It's a great pity.\"",
	"\"Forget it! In any case you were not grasped by the legendary monster.\"",
	"\"I trained on the road of ultimate!\"",
	"\"Think of your impending death my sputa will flow down.\"",
	"\"Little strawberry me baby!\"",
	"\"Is my hair-clip beautiful? Contest?\"",
	"\"In spite of the monster, and the coach, only trashy, I will beat down them all.\"",
	"\"One day I will beat you down, and let you know my strength.\"",
	"\"BULLET GIVEN BY BET-BOY GIVEN BY BET-BOY CAN USE\"",
	"\"I am the son of Wind, the child of spirit.\"",
	"\"What don't stare me always!\"",
	"\"Because we have the spirit not to lose absolutely, whether or no, and we are called 'wintry poplar and willow' to see our might!\"",
	"\"Wei! You can't throw the butt casually!\"",
	"\"Everyone is playing the pocket monster, so am I. It is very lovely! Try it? You play it very well!\"",
	"\"The pocket monster said that I was lovely.\"",
	"\"Little familiarity grotty sight!\"",
	"\"How are you? I will visit you. Try to eat a piece of muskiness.\"",
	"\"Oh, it is you. You are more fearful than Msr.Yu.\"",
	"\"Glinting set off by rainbow fluttering fairily in the sky.\"",
	"\"Flapping the sea with ten legs bending!\"",
	"\"Avada Kedavra!\"",
	"\"Doth pain excite thee?  Wouldst thou prefer the whip?\"",
	"\"Abandon your foolish quest! All that awaits you is the wrath of my Master. You are too late to save the child. Now you'll join him - in hell.\"",
	"\"Only the chosen ones will survive purgatory.\"",
	"\"Lord Dagon awaits your soul in Oblivion.\"",
	"\"You're weak, just like the gods you pray to.\"",
	"\"This is your end, swit!\"",
	"\"You will suffer eternally!\"",
	"\"Soon you will be one of us.\"",
	"\"Die, coward!\"",
	"\"I'll cut you up slowly.\"",
	"\"Beg for your life, slave!\"",
	"\"You're gonna bleed!\"",
	"\"I'm gonna feast on your dead flesh!\"",
	"\"Your corpse will stink like a heap of shit!\"",
	"\"Morituri te salutant!\"",
	"\"Ph'nglui mglw'nafh Cthulhu R'lyeh wagh'nagl fthagn!\"",
	"\"Suck it!\"",
	"\"The next words out of your mouth better be some Mark Twain shit because it's going on your grave.\"",
	"\"What kind of loser has the time to make it this far? After I kill you, get a life!\"",
	"\"Where is your God now?\"",
	"\"You have no chance to survive make your time.\"",
	"\"You n'wah!\"",
	"\"Your mommy cannot bake!\"",
	"\"Your mother was a hamster, and your father smelt of elderberries!\"",
	"\"Your scream reminds me of your mother last night!\"",
	"\"You fight like a girl called Frankenstein!\"",
	"\"Come join the dark side, we have cookies!\"",
	"\"Stop sucking so bad at this game! Go read some spoilers!\"",
	"\"I AM... I AM A MONSTER! ... ... ... ... ... ... ... ... coach.\"",
	"\"I heard you were sleeping there.\"",
	"\"Er which hole? What droped? Failing thing, serve one right!\"",
	"\"GO AWAY!\"",
	"\"EN'SSICIBZ EDUDO\"",
	"\"Your inside is made of mud!\"",
	"\"Your daddy screamed REAL good before he died!\"",
	"\"You will die, just like your father!\"",
	"\"Die, filthy N'wah!\"",
	"\"As if feel something!\"",
	"\"Linked hole!\"",
	"\"EY REFLECT EFF INT!\"",
	"You feel a strange vibration beneath your feet.",
	"You are frozen solid!",
	"You're burning!",
	"Your potion of extra healing dilutes.",
	"Your potion of gain level dilutes further.",
	"Suddenly, your 2 scrolls of genocide transform!",
	"Your magic marker explodes!",
	"The scroll turns to dust as you pick it up.",
	"The scroll turns to dust as you touch it.",
	"There's some graffiti on the ground here. Use 'r' to read it.",
	"The door resists!",
	"You see a door open.",
	"You hear a door open.",
	"You see a door crash open.",
	"You hear a door crash open.",
	"You feel vibrations.",
	"You feel shuddering vibrations.",
	"You hear a soft click.",
	"You hear a click.",
	"You hear several clicking sounds.",
	"You hear a rushing noise nearby.",
	"Your cloak of magic resistance is protected by the layer of grease! The grease wears off.",
	"The death ray hits it.",
	"The blast of disintegration misses it. The blast of disintegration hits it.",
	"You feel cold.",
	"You hear sizzling. The acid ray hits it.",
	"It breathes lightning. The door splinters!",
	"You hear a nearby zap. You hear crashing wood.",
	"You hear an exhaling sound. You smell smoke.",
	"You hear a flinging sound. The door absorbs the bolt!",
	"You see a puff of smoke.",
	"Some water evaporates.",
	"Your Stormbringer suddenly vibrates unexpectedly.",
	"Your Stormbringer suddenly vibrates unexpectedly and then detonates. Warning: HP low!",
	"You feel weaker! You lose  Strength",
	"You feel clumsy! You lose  Dexterity",
	"Your muscles won't obey you! You lose  Dexterity",
	"You feel stupid! You lose  Intelligence",
	"Your brain is on fire! You lose  Intelligence",
	"You feel foolish! You lose  Wisdom",
	"Your judgement is impared! You lose  Wisdom",
	"You feel very sick! You lose  Constitution",
	"You break out in hives! You lose  Charisma",
	"The gremlin chuckles. You feel a little sick!",
	"You lear laughter. Your senses fail!",
	"A voice announces: 'Thou be cursed!' You feel no longer immune to diseases!",
	"You hear a chuckling laughter. You feel unable to control where you're going.",
	"You feel warmer.",
	"You feel less jumpy.",
	"You feel a little sick!",
	"Your senses fail!",
	"You feel cooler.",
	"You feel paranoid.",
	"You thought you saw something!",
	"You feel slower.",
	"You feel clumsy.",
	"You feel vulnerable.",
	"You feel less attractive.",
	"You feel tired all of a sudden.",
	"You feel like you're going to break apart.",
	"You feel like someone has zapped you.",
	"You feel like someone is sucking out your life-force.",
	"You feel no longer immune to diseases!",
	"You feel that your radar has just stopped working!",
	"You feel unable to find something you lost!",
	"You feel a loss of mental capabilities!",
	"You feel shrouded in darkness.",
	"You feel that you can no longer sense monsters.",
	"You feel your legs shrinking.",
	"You feel unable to control where you're going.",
	"You feel you suddenly need to breathe!",
	"You feel your wounds are healing slower!",
	"You feel a loss of mystic power!",
	"You feel 42.",
	"You feel unable to change form!",
	"You feel less control over your own body.",
	"You feel worried about corrosion!",
	"You feel like you're burning calories faster.",
	"You feel like you just lost your wings!",
	"You feel less ethereal!",
	"You feel less protected from magic!",
	"You feel less reflexive!",
	"You feel less aquatic!",
	"You feel a loss of freedom!",
	"You feel a little anxious!",
	"You feel vaguely nervous.",
	"You feel a case of selective amnesia...",
	"You feel cursed!",
	"You feel a little stunned!",
	"You feel a little confused!",
	"You feel your attacks becoming slower!",
	"You feel your attacks becoming a lot slower!",
	"You feel less able to learn new stuff!",
	"You feel a little dead!",
	"You feel a little exposed!",
	"You feel that you're going to die in turns time!",
	"You feel that you really ain't gonna ascend with this character...",
	"You feel that the life of your character is forfeit, better roll a new one right away...",
	"You feel that the RNG is out to get you!",
	"You feel a need to check Nethackwiki so you can see if there's a new version of SLASH'EM Extended available...",
	"You feel a need to press the 'v' key so you can see if you're playing the up-to-date version...",
	"You feel that Amy Bluescreenofdeath may have made a new version of this game already...",
	"You feel that Nethack is better than Crawl.",
	"You feel that Slash'EM Extended is the best Nethack variant available.",
	"You feel that ADOM sux compared to Nethack.",
	"You feel like giving AmyBSOD's other roguelike variant ToME-SX a try someday.",
	"You feel that ToME-SX might also be a good game, so you open the git repository page in your web browser.",
	"You feel that Nethack and Angband really are the two best major roguelikes ever created.",
	"You feel that reading these messages makes you feel stupid.",
	"You feel that you keep looking at the messages displayed on the top status line.",
	"You feel like skipping all the annoying battle spam messages, including actually important ones like 'You are slowing down'.",
	"You feel that AmyBSOD is trolling you with all the message spam, yet you keep reading it... :D",
	"You feel that AmyBSOD has no life since she typed in all those various bullshit messages you're reading.",
	"You feel that this message is false.",
	"You feel that this message is true.",
	"You feel that the previous message was false.",
	"You feel that the next message will be a fake one.",
	"You feel that you now know the meaning of life, the universe and everything else.",
	"You feel that now, after having played Nethack for 25 hours straight, it is time to go to bed.",
	"Remember, your body in real life has functions too - don't forget to drink, eat and sleep!",
	"Reminder: don't forget to go to the toilet!",
	"Don't neglect your real-life obligations because of Nethack, please!",
	"It's time for our super-special commercial announcement: Buy THE COMPLETE ANGBAND AND ANCIENT DOMAINS OF MYSTERY PACKAGE for just $50!!!",
	"It's time for our super-special commercial announcement: Fallout 4, just $60 and not dumbed down at all! Buy now!",
	"It's time for our super-special commercial announcement: Buy 5 dollars for the unbelievable price of just $2!",
	"It's time for our super-special commercial announcement: Amulet of Yendor, price $99999999999--More--",
	"It's time for our super-special commercial announcement: CALIGULA'S CASINO! PLACE YOUR BETS, WIN WIN WIN!",
	"It's time for our super-special commercial announcement: FISHING! FISHING! WIN! WIN!",
	"It's time for our super-special commercial announcement: Today there's a laptop with Micro$oft Windows 7 for sale! Just $2500!",
	"It's time for our super-special commercial announcement: e - a ring of become Noeda (being worn). Sub-5000 turn ascension virtually guaranteed!",
	"It's time for our super-special commercial announcement: 10 scrolls of destroy armor! Mass discount if you buy them all!",
	"It's time for our super-special commercial announcement: For just $50000, you can get your own heavily cursed thoroughly corroded very rusty -7 Grayswandir welded to your hand!",
	"Your fingers seem ok for now.",
	"Your fingers start hurting from playing for so long.",
	"Nothing appears to happen.",
	"Somehing appears to happen.",
	"Nothing happens.",
	"Something happens.",
	"Something bad happens.",
	"You don't notice anything unusual.",
	"You notice something unusual.",
	"You don't really feel any worse.",
	"You feel worse.",
	"You can't tell that anything is going wrong.",
	"You can tell that something is going wrong.",
	"Your mood seems about as good as before.",
	"Your mood seems much worse as before.",
	"You don't even have a case of the heebie jeebies.",
	"You have a case of the heebie jeebies.",
	"You have a case of the willies for a moment, but it passes.",
	"You probably have nothing to fear but fear itself.",
	"You probably have nothing to fear but fear itself, and that monster that wants to eat you.",
	"The dungeon doesn't seem to be collapsing.",
	"The dungeon seems to be collapsing.",
	"Suddenly, the dungeon doesn't collapse.",
	"You don't see monsters appearing out of nowhere.",
	"You see monsters appearing out of nowhere.",
	"You see monsters crawling out of the walls.",
	"You don't have an urgent need to sit down.",
	"You have an urgent need to sit down.",
	"You have an urgent need to go to the toilet.",
	"You don't notice anything unnatural about how quiet it is.",
	"You notice something unnatural about how quiet it is.",
	"The gas spore explodes!",
	"You get blasted!",
	"You are caught in the highly explosive bomb's explosion!",
	"BOOM! It explodes!",
	"The newt bites you!",
	"Suddenly, you notice a gnoll.",
	"The superjedi ignites a green lightsaber.",
	"The jedi knight wields a red lightsaber!",
	"The mumak blasts you!",
	"You find a fire trap.",
	"Do you really want to step into that mind wipe trap?",
	"You find a super nupesell nix area.",
	"You find a magic portal.",
	"You trigger a magic portal, but it doesn't seem to work!",
	"You're pushed back!",
	"Yes.",
	"No.",
	"Really save?",
	"Your candle's flame is extinguished!",
	"Your magic lamp goes out!",
	"Your magic lamp turns into an oil lamp!",
	"Clash!",
	"You hear a twang.",
	"You hear a bugle playing reveille!",
	"It breathes on you!",
	"The green naga spits on you!",
	"The soldier fires a bullet!",
	"Something explodes inside your knapsack!",
	"You feel increasingly panicked about being in the dark!",
	"You hear a loud scraping noise.",
	"You can swim! Type #swim while swimming on the surface to dive down to the bottom.",
	"Press Ctrl^E or type #seal to engrave a seal of binding.",
	"#chat to a fresh seal to contact the spirit beyond.",
	"Press Ctrl^F or type #power to fire active spirit powers!",
	"Beware, droven armor evaporates in light!",
	"Use #monster to create a patch of darkness.",
	"Use #monster to adjust your clockspeed.",
	"You do not heal naturally. Use '.' to attempt repairs.",
	"Incantifiers eat magic, not food, and do not heal naturally.",
	"The engraving tumbles off the ring to join it's fellows.",
	"The leash would not come off!",
	"The kitten chokes on the leash!",
	"An incomprehensible sight meets your eyes!",
	"It makes a rather sad clonk.",
	"It seems inert.",
	"The cloak sweeps up a web!",
	"A web spins out from the cloak!",
	"The thoroughly tattered cloak falls to pieces!",
	"A monster is there that you couldn't see.",
	"Suddenly, and without warning, nothing happens.",
	"WARNING: Nothing happens!",
	"WARNING: something might have happened!",
	"WARNING: an out-of-depth monster was spawned!",
	"WARNING: an out-of-depth monster was spawned! Really! This is not a joke!",
	"WARNING: you still haven't ascended yet!",
	"Your grasp of physics is appalling.",
	"Is this a zen thing?",
	"You chip off the existing rune.",
	"The wood heals like the rune was never there.",
	"In order to eat, use the 'e' command.",
	"There is a seal on the tip of the Rod! You can't see it, you know it's there, just the same.",
	"You learn a new seal!",
	"You feel more confident in your weapon skills.",
	"You feel more confident in your fucking weapon skills.",
	"You feel more confident in your combat skills.",
	"You feel more confident in your spellcasting skills.",
	"There is a seal in the heart of the crystal, shining bright through the darkness.",
	"The cracks on the mirror's surface form part of a seal.",
	"dbon applies",
	"The webbing sticks to you. You're caught!",
	"The unholy blade drains your life!",
	"Your weapon has become more perfect!",
	"Your weapon has become more perfect! Yes, this sentence is not nonsensical in any way!",
	"The winds fan the flames into a roaring inferno!",
	"The winds blast the stone and sweep the fragments into a whirling dust storm!",
	"The winds whip the waters into a rolling fog!",
	"The hateful axe hamstrings you!",
	"The asian girl smashes your nuts flat!",
	"It blows apart in the wind.",
	"The white unicorn goes insane!",
	"The black blade drains your life!",
	"Silver starlight shines upon your blade!",
	"Death Reborn Revolution!",
	"Silence Wall!",
	"The walls of the dungeon quake!",
	"Dead Scream.",
	"Time Stop!",
	"Getsuga Tensho!",
	"What dance will you use (1/2/3)?",
	"Some no mai, Tsukishiro!",
	"Tsugi no mai, Hakuren!",
	"San no mai, Shirafune!",
	"Ice crackles around your weapon!",
	"The spiritual energy is released!",
	"A cloud of toxic smoke pours out!",
	"For a moment, you feel a crushing weight settle over you.",
	"Your weapon has become more flawed.",
	"Your weapon rattles warningly.",
	"Your weapon has become much more flawed!",
	"Your weapon rattles alarmingly.  It has become much more flawed!",
	"The strange symbols stare at you reproachfully.",
	"Cool it, Link.  It's just a book.",
	"The whisperers berate you ceaselessly.",
	"Following the instructions on the pages, you cast the spell!",
	"What is this strange command!?",
	"You strike the twin-bladed athame like a tuning fork. The beautiful chime is like nothing you have ever heard.",
	"Unrecognized Necronomicon effect.",
	"Unrecognized Lost Names effect.",
	"A cloud of steam rises.",
	"Icicles form and fall from the freezing ceiling.",
	"The altar falls into a chasm.",
	"What game do you think you are playing anyway?",
	"Aivan sairas kaveri kun tuollaista aikoo puuhata!",
	"Insufficient data for further analysis.",
	"Non sequitur. Your facts are uncoordinated.",
	"Type '?' for help.",
	"Invalid command.",
	"What?",
	"WHAT?!",
	"You must be out of your mind!",
	"You're killing me.",
	"Are you sure?",
	"Are you sure you know what you are doing?",
	"Aww, come on!",
	"That makes no sense.",
	"I beg your pardon.",
	"Degreelessness mode on.",
	"Degreelessness mode off.",
	"Syntax error.",
	"That doesn't compute.",
	"I don't understand you.",
	"???",
	"Sure. Piece of cake.",
	"Error.",
	"You can't do that!",
	"Help!",
	"Come again?",
	"Sorry?",
	"Sorry, I'm not sure I understand you.",
	"What's your point?",
	"Unknown command.",
	"Command not found.",
	"An unexpected error has occurred because an error of type 42 occurred.",
	"Somehow, you think that would never work.",
	"Welcome to level 42.",
	"Don't be ridiculous!",
	"lfae aierty agnxzcg?",
	"Soyha, azho bouate!",
	"I don't fully understand you.",
	"Why would anybody want to do THAT?",
	"Yes, yes, now tell me about your childhood.",
	"Satisfied?",
	"Something is wrong here.",
	"There's something wrong with YOU.",
	"You leap up 9' and perform a miraculous 5xSpiral in the air.",
	"Aw, shaddap!",
	"Shut up, smartass!",
	"I see little point in doing that.",
	"Oh, really?",
	"Very funny.",
	"You've got to be kidding!",
	"I'm not amused.",
	"I must have misheard you.",
	"Nothing happens.",
	"Where did you learn THAT command?",
	"When all else fails, read the instructions.",
	"Why not read the instructions first?",
	"Cut it out!",
	"Nothing interesting happens.",
	"Just how exactly am I supposed to do THAT?",
	"That's morally wrong and I won't do it.",
	"I'm not gonna take this abuse.",
	"AAAAAAAAAAAAAHHHHHHHHHHHHHHHRRRRRRRRRRRGGGGGGGGGGG!",
	"No more, if you value your character's life!",
	"Give it up, guy.",
	"Disk error. (a)bort, (r)etry, (f)ail?",
	"The image of the weeping angel is taking over your body!",
	"exerper: Hunger checks",
	"exerchk: testing.",
	"Explore mode is for local games, not public servers.",
	"No port-specific debug capability defined.",
	"A normal force teleports you away...",
	"Do chunks miss?",
	"Mon can't survive here",
	"Jump succeeds!",
	"Doing relocation.",
	"Checking new square for occupancy.",
	"The drawbridge closes in...",
	"Monsters sense the presence of you.",
	"Its mouth opens!",
	"The purple worm opens its mouth!",
	"Crash!  The ceiling collapses around you!",
	"The debris from your digging comes to life!",
	"Powerful winds force you down!",
	"Clang!",
	"Clink!",
	"Clonk!",
	"Whang!",
	"Plop!",
	"Splash!",
	"An alarm sounds!",
	"You penetrated a high security area!",
	"A voice booms: \"The Angel of the Pit hast fallen!  We have returned!\"",
	"It constricts your throat!",
	"Suddenly you cannot see yourself.",
	"The tentacles tear your cloak to shreads!",
	"split object,",
	"testing for betrayal",
	"THUD!",
	"Thump!",
	"CRASH!  You destroy it.",
	"You've disturbed the birds nesting high in the tree's branches!",
	"Some black feathers drift down.",
	"Some scraps of webbing drift down.",
	"You've attracted the tree's guardians!",
	"Some branches are swinging...",
	"Klunk!",
	"Ouch!  That hurts!",
	"WHAMMM!!!",
	"When you can't make them see the light, make them feel the heat.",
	"Every man must be free to become whatever God intends he should become.",
	"There you go again.",
	"Before I refuse to take your questions, I have an opening statement.",
	"If you're explaining, you're losing.",
	"As government expands, liberty contracts.",
	"Each individual is accountable for his actions.",
	"Government's first duty is to protect the people, not run their lives.",
	"The ultimate determinate in the struggle now going on for the world will not be bombs and rockets but a test of wills and ideas.",
	"If we lose freedom here, there is no place to escape to.",
	"This is the last stand on Earth.",
	"You and I have a rendezvous with destiny.",
	"We will preserve for our children this, the last best hope of man on earth, or we will sentence them to take the first step into a thousand years of darkness.",
	"Trust, but verify.",
	"The bombing begins in five minutes.",
	"Some people wonder all their lives if they've made a difference. The Marines don't have that problem.",
	"History teaches that war begins when governments believe the price of aggression is cheap.",
	"Freedom is never more than one generation away from extinction.",
	"Our people look for a cause to believe in.",
	"Today we did what we had to do.",
	"They counted on us to be passive. They counted wrong.",
	"Of the four wars in my lifetime, none came about because we were too strong.",
	"If it moves, tax it. If it keeps moving, regulate it. And if it stops moving, subsidize it.",
	"Tear down this wall.",
	"Open this gate!",
	"Come here to this gate!",
	"There are no easy answers, but there are simple answers.",
	"We must have the courage to do what we know is morally right.",
	"We are never defeated unless we give up on God.",
	"We will always remember.",
	"We will always be proud.",
	"We will always be prepared, so we will always be free.",
	"It is a weapon our adversaries in today's world do not have.",
	"They say the world has become too complex for simple answers. They are wrong.",
	"Don't be afraid to see what you see.",
	"Concentrated power has always been the enemy of liberty.",
	"Nothing happens when you pull the trigger.",
	"Ouch!",
	"It blinds you!",
	"The flying shards of mirror sear you!",
	"Splat!",
	"What a mess!",
	"u_on_sstairs: picking random spot",
	"You reform!",
	"Oh wow!  Great stuff!",
	"Tried to give an impossible intrinsic",
	"The corpse liquefies into a putrid broth, and you slurp it down and die!",
	"Segmentation fault -- core dumped.",
	"Bus error -- core dumped.",
	"Yo' mama -- core dumped.",
	"Suddenly you can see yourself.",
	"You are metal and springs, not flesh and blood. You cannot eat.",
	"Ulch - that was tainted with filth!",
	"Hakuna matata!",
	"That sounds really boring.",
	"There is no longer sufficient tension in your mainspring to maintain a high clock-speed",
	"Player 1 needs food badly!",
	"Player 1, all your powers will be lost...",
	"Player 1, your life-force is running out.",
	"Player 1 is about to die.",
	"The warding sign can be reinforced no further!",
	"Ouch!  You bump into a heavy door.",
	"A breaker penetrator attacks you by surprise!",
	"That was all.",
	"Select an inventory slot letter.",
	"Your key broke!",
	"Klick!",
	"Klick! Klick! Klick! Klick! Klick! Klick! Klick! Klick! Klick! Klick!--More-- How many boxes do you have??? (Don't say 'didn't count'!)",
	"Warning: no monster.",
	"Demogorgon, Prince of Demons, is near!",
	"Lamashtu, the Demon Queen, is near!",
	"Lightning crackles around you.",
	"You're enveloped in flames.",
	"You're covered in frost.",
	"A cascade of filth pours onto you!",
	"A torrent of burning acid rains down on you!",
	"It feels mildly uncomfortable.",
	"A sudden geyser erupts under your feet!",
	"A pillar of fire strikes all around you!",
	"Chunks of ice pummel you from all sides!",
	"A bolt of lightning strikes down at you from above!",
	"The world tears open, and an invincible superman steps through!",
	"A cloud of darkness falls upon you.",
	"You just soiled yourself. Get to the next toilet in 200 turns or all the monsters will laugh at you.",
	"You feel like reading one of Amy's smut fanfics.",
	"So thou thought thou couldst kill me, fool.",
	"You hear air currents.",
	"You hear a farting sound.",
	"You hear a beautiful sound.",
	"You hear a disgusting sound.",
	"Something pulls down your pants.",
	"You feel childish.",
	"You feel like sitting in a corner.",
	"You hear scratching noises.",
	"You hear digging noises.",
	"You can definitely hear human screams.",
	"Somewhere on this level someone is getting killed, right now.",
	"You hear a scream.",
	"You hear a roar.",
	"You hear a chime.",
	"You smell poisonous gas.",
	"You suffocate!",
	"You smell something rotten.",
	"You smell decay.",
	"Your nostrils fill with a sharp stench of smoke.",
	"You feel deathly cold.",
	"You feel sweaty.",
	"You feel bad.",
	"You feel much worse.",
	"You feel even worse.",
	"You don't feel so good...",
	"You feel sick.",
	"You feel apathetic.",
	"You feel bored.",
	"You are suffering from sleep deprivation.",
	"You are suffering from withdrawal symptoms.",
	"A gray stone lands in your inventory! E - A GRAY STONE. YOUR MOVEMENTS ARE SLOWED SLIGHTLY BECAUSE OF YOUR LOAD.",
	"The gnome throws an inventory loss stone!",
	"The hobbit shoots a superscroller stone! The superscroller stone hits you!",


};

void
msgpline_add(typ, pattern)
     int typ;
     char *pattern;
{
    struct _plinemsg *tmp = (struct _plinemsg *) alloc(sizeof(struct _plinemsg));
    if (!tmp) return;
    tmp->msgtype = typ;
    tmp->pattern = strdup(pattern);
    tmp->next = pline_msg;
    pline_msg = tmp;
}

void
msgpline_free()
{
    struct _plinemsg *tmp = pline_msg;
    struct _plinemsg *tmp2;
    while (tmp) {
	free(tmp->pattern);
	tmp2 = tmp;
	tmp = tmp->next;
	free(tmp2);
    }
    pline_msg = NULL;
}

int
msgpline_type(msg)
     char *msg;
{
    struct _plinemsg *tmp = pline_msg;
    while (tmp) {
	if (pmatch(tmp->pattern, msg)) return tmp->msgtype;
	tmp = tmp->next;
    }
    return MSGTYP_NORMAL;
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */
char * FDECL(replace, (const char *, const char *, const char *));

#if defined(USE_STDARG) || defined(USE_VARARGS)
static void FDECL(vpline, (const char *, va_list));

void
pline VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, char *);
	vpline(line, VA_ARGS);
	VA_END();
}

char prevmsg[BUFSZ];

# ifdef USE_STDARG
static void
vpline(const char *line, va_list the_args) {
# else
static void
vpline(line, the_args) const char *line; va_list the_args; {
# endif

#else	/* USE_STDARG | USE_VARARG */

#define vpline pline

void
pline VA_DECL(const char *, line)
#endif	/* USE_STDARG | USE_VARARG */

	char pbuf[BUFSZ];
	int typ;
/* Do NOT use VA_START and VA_END in here... see above */

	if (!line || !*line) return;

	if ( (u.uprops[RANDOM_MESSAGES].extrinsic || RandomMessages || have_messagestone() ) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover 

#if defined(WIN32)
&& !program_state.exiting
#endif

) line = fake_plines[rn2(SIZE(fake_plines))];

	if ( (MemoryLoss || u.uprops[MEMORY_LOST].extrinsic || have_memorylossstone() ) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover 

/* buildfix by EternalEye: sinfo.exiting only exists on win32 */
#if defined(WIN32)
&& !program_state.exiting
#endif

) line = "Warning: Low Local Memory. Freeing description strings.";

	if (index(line, '%')) {
	    Vsprintf(pbuf,line,VA_ARGS);
	    line = pbuf;
	}

        if ( (RotThirteen || u.uprops[ROT_THIRTEEN].extrinsic || have_rotthirteenstone() ) && (strlen(line)<(BUFSZ-5)) &&(!program_state.in_impossible) ) {

		line = replace(line,"a","N");
		line = replace(line,"b","O");
		line = replace(line,"c","P");
		line = replace(line,"d","Q");
		line = replace(line,"e","R");
		line = replace(line,"f","S");
		line = replace(line,"g","T");
		line = replace(line,"h","U");
		line = replace(line,"i","V");
		line = replace(line,"j","W");
		line = replace(line,"k","X");
		line = replace(line,"l","Y");
		line = replace(line,"m","Z");
		line = replace(line,"n","A");
		line = replace(line,"o","B");
		line = replace(line,"p","C");
		line = replace(line,"q","D");
		line = replace(line,"r","E");
		line = replace(line,"s","F");
		line = replace(line,"t","G");
		line = replace(line,"u","H");
		line = replace(line,"v","I");
		line = replace(line,"w","J");
		line = replace(line,"x","K");
		line = replace(line,"y","L");
		line = replace(line,"z","M");

	  }  else if ( (BigscriptEffect || u.uprops[BIGSCRIPT].extrinsic || have_bigscriptstone() ) && (strlen(line)<(BUFSZ-5)) &&(!program_state.in_impossible) ) {

		line = replace(line,"a","A");
		line = replace(line,"b","B");
		line = replace(line,"c","C");
		line = replace(line,"d","D");
		line = replace(line,"e","E");
		line = replace(line,"f","F");
		line = replace(line,"g","G");
		line = replace(line,"h","H");
		line = replace(line,"i","I");
		line = replace(line,"j","J");
		line = replace(line,"k","K");
		line = replace(line,"l","L");
		line = replace(line,"m","M");
		line = replace(line,"n","N");
		line = replace(line,"o","O");
		line = replace(line,"p","P");
		line = replace(line,"q","Q");
		line = replace(line,"r","R");
		line = replace(line,"s","S");
		line = replace(line,"t","T");
		line = replace(line,"u","U");
		line = replace(line,"v","V");
		line = replace(line,"w","W");
		line = replace(line,"x","X");
		line = replace(line,"y","Y");
		line = replace(line,"z","Z");

	  }


/*Intercept direct speach, inpossible() and very short or long Strings here*/
/* to cut down unnecesary calls to the now slower replace */
/* other checks like read must be done dynamically because */
/* they depent on position -CK */
/* right : Ye read "Here lies ..."
   wrong : You read "Here lies ..."
   wrong : Ye read "'er lies ..." */
        if( (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) ) &&(*line!='"')&&(strlen(line)<(BUFSZ-5))
             &&(!program_state.in_impossible)
             &&(strlen(line)>9)){
                /* >9: "You die ..." but not "It hits." */
		line = replace(line,"You","Ye");
		line = replace(line,"you","ye");
		line = replace(line,"His","'is");
		line = replace(line," his"," 'is");
		line = replace(line,"Her","'er");
		line = replace(line," her"," 'er");
		line = replace(line,"Are","Be");
		line = replace(line," are"," be");
		line = replace(line,"Is ","Be");
		line = replace(line," is "," be ");
		line = replace(line," is."," be.");
		line = replace(line," is,"," be,");
		if (Role_if(PM_KORSAIR)) { /* words beginning with a c will begin with a k for korsairs --Amy */
		line = replace(line,"C","K");
		line = replace(line," c"," k");
		line = replace(line,"(c","(k");
		}
		line = replace(line,"Is ","Be ");
		line = replace(line,"Of ","O' ");
		line = replace(line," of "," o' ");
		line = replace(line,"Of.","O'.");
		line = replace(line," of."," o'.");
		line = replace(line,"Of,","O',");
		line = replace(line," of,"," o',");
		line = replace(line," ear"," lug");
		line = replace(line,"Ear","Lug");
		line = replace(line,"eye","deadlight");
		line = replace(line,"Eye","Deadlight");
                /* If orkmid isn't contained, save some time -CK */
                if(strstr(line,"orkmid") )
                {
                 line = replace(line,"zorkmids ","doubloons ");
                 line = replace(line,"Zorkmids ","Doubloons ");
                 line = replace(line,"zorkmids.","doubloons.");
                 line = replace(line,"Zorkmids.","Doubloons.");
                 line = replace(line,"zorkmids,","doubloons,");
                 line = replace(line,"Zorkmids,","Doubloons,");
                 line = replace(line,"zorkmids)","doubloons)");
                 line = replace(line,"Zorkmids)","Doubloons)");
                 line = replace(line,"zorkmid ","doubloon ");
                 line = replace(line,"Zorkmid ","Doubloon ");
                 line = replace(line,"zorkmid.","doubloon.");
                 line = replace(line,"Zorkmid.","Doubloon.");
                 line = replace(line,"zorkmid,","doubloon,");
                 line = replace(line,"Zorkmid,","Doubloon,");
                 line = replace(line,"zorkmid)","doubloon)");
                 line = replace(line,"Zorkmid)","Doubloon)");
                } /* endif orkmid */
                /* If old coin isn't contained, save some time -CK */
                if(strstr(line,"old coin") )
                {
                 line = replace(line,"gold coins","pieces of eight");
                 line = replace(line,"Gold coins","Pieces of eight");
                 line = replace(line,"gold coin","piece of eight");
                 line = replace(line,"Gold coin","Piece of eight");
                }
                /* If old piece isn't contained, save some time -CK */
                if(strstr(line,"old piece") )
                {
                 line = replace(line,"gold pieces.","pieces of eight");
                 line = replace(line,"Gold pieces.","Pieces of eight");
                 line = replace(line,"gold pieces,","pieces of eight");
                 line = replace(line,"Gold pieces,","Pieces of eight");
                 line = replace(line,"gold pieces ","pieces of eight");
                 line = replace(line,"Gold pieces ","Pieces of eight");
                 line = replace(line,"gold piece.","piece of eight");
                 line = replace(line,"Gold piece.","Piece of eight");
                 line = replace(line,"gold piece,","piece of eight");
                 line = replace(line,"Gold piece,","Piece of eight");
                 line = replace(line,"gold piece ","piece of eight");
                 line = replace(line,"Gold piece ","Piece of eight");
                } /* endif old piece */
        }  /* endif role_if(PM_PIRATE),etc. */

#if defined(DUMP_LOG)
	if (DUMPMSGS > 0 && !program_state.gameover) {
		/* count identical messages */
		if (!strncmp(msgs[lastmsg], line, BUFSZ)) {
			msgs_count[lastmsg] += 1;
		} else {
			lastmsg = (lastmsg + 1) % DUMPMSGS;
			strncpy(msgs[lastmsg], line, BUFSZ);
			msgs_count[lastmsg] = 1;
		}
	}
#endif

	typ = msgpline_type(line);
	if (!iflags.window_inited) {
	    raw_print(line);
	    return;
	}
#ifndef MAC
	if (no_repeat && !strcmp(line, toplines))
	    return;
#endif /* MAC */
	if (vision_full_recalc) vision_recalc(0);
	if (u.ux) flush_screen(1);		/* %% */
	if (typ == MSGTYP_NOSHOW) return;
	if (typ == MSGTYP_NOREP && !strcmp(line, prevmsg)) return;
	putstr(WIN_MESSAGE, 0, line);
	strncpy(prevmsg, line, BUFSZ);
	if (typ == MSGTYP_STOP) display_nhwindow(WIN_MESSAGE, TRUE); /* --more-- */
}

/*VARARGS1*/
void
Norep VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, const char *);
	no_repeat = TRUE;
	vpline(line, VA_ARGS);
	no_repeat = FALSE;
	VA_END();
	return;
}

/* work buffer for You(), &c and verbalize() */
static char *you_buf = 0;
static int you_buf_siz = 0;

static char *
You_buf(siz)
int siz;
{
	if (siz > you_buf_siz) {
		if (you_buf) free((genericptr_t) you_buf);
		you_buf_siz = siz + 10;
		you_buf = (char *) alloc((unsigned) you_buf_siz);
	}
	return you_buf;
}

void
free_youbuf()
{
	if (you_buf) free((genericptr_t) you_buf),  you_buf = (char *)0;
	you_buf_siz = 0;
}

/* `prefix' must be a string literal, not a pointer */
#define YouPrefix(pointer,prefix,text) \
 Strcpy((pointer = You_buf((int)(strlen(text) + sizeof prefix))), prefix)

#define YouMessage(pointer,prefix,text) \
 strcat((YouPrefix(pointer, prefix, text), pointer), text)

/*VARARGS1*/
void
You VA_DECL(const char *, line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "You ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
Your VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "Your ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
You_feel VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "You feel ", line), VA_ARGS);
	VA_END();
}


/*VARARGS1*/
void
You_cant VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "You can't ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
pline_The VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "The ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
There VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "There ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
You_hear VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	if (Underwater)
		YouPrefix(tmp, "You barely hear ", line);
	else if (u.usleep)
		YouPrefix(tmp, "You dream that you hear ", line);
	else
		YouPrefix(tmp, "You hear ", line);
	vpline(strcat(tmp, line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
verbalize VA_DECL(const char *,line)
	char *tmp;
	if (!flags.soundok) return;
	VA_START(line);
	VA_INIT(line, const char *);
	tmp = You_buf((int)strlen(line) + sizeof "\"\"");
	Strcpy(tmp, "\"");
	Strcat(tmp, line);
	Strcat(tmp, "\"");
	vpline(tmp, VA_ARGS);
	VA_END();
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */

#if defined(USE_STDARG) || defined(USE_VARARGS)
static void FDECL(vraw_printf,(const char *,va_list));

void
raw_printf VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, char *);
	vraw_printf(line, VA_ARGS);
	VA_END();
}

# ifdef USE_STDARG
static void
vraw_printf(const char *line, va_list the_args) {
# else
static void
vraw_printf(line, the_args) const char *line; va_list the_args; {
# endif

#else  /* USE_STDARG | USE_VARARG */

void
raw_printf VA_DECL(const char *, line)
#endif
/* Do NOT use VA_START and VA_END in here... see above */

	if(!index(line, '%'))
	    raw_print(line);
	else {
	    char pbuf[BUFSZ];
	    Vsprintf(pbuf,line,VA_ARGS);
	    raw_print(pbuf);
	}
}


/*VARARGS1*/
void
impossible VA_DECL(const char *, s)
	VA_START(s);
	VA_INIT(s, const char *);
	if (program_state.in_impossible)
		panic("impossible called impossible");
	program_state.in_impossible = 1;
	{
	    char pbuf[BUFSZ];
	    Vsprintf(pbuf,s,VA_ARGS);
	    paniclog("impossible", pbuf);
	}
	vpline(s,VA_ARGS);
	pline("Program in disorder. Please inform Amy (Bluescreenofdeath at nethackwiki) about this bug.");
	program_state.in_impossible = 0;
	VA_END();
}

const char *
align_str(alignment)
    aligntyp alignment;
{
    switch ((int)alignment) {
	case A_CHAOTIC: return "chaotic";
	case A_NEUTRAL: return "neutral";
	case A_LAWFUL:	return "lawful";
	case A_NONE:	return "unaligned";
    }
    return "unknown";
}

const char *
hybrid_str()
{
    char string[BUFSZ];
    *string = '\0';
	if (flags.hybridangbander) Sprintf(eos(string), "angbander ");
	if (flags.hybridaquarian) Sprintf(eos(string), "aquarian ");
	if (flags.hybridcurser) Sprintf(eos(string), "curser ");
	if (flags.hybridhaxor) Sprintf(eos(string), "haxor ");
	if (flags.hybridhomicider) Sprintf(eos(string), "homicider ");
	if (flags.hybridsuxxor) Sprintf(eos(string), "suxxor ");
	if (flags.hybridwarper) Sprintf(eos(string), "warper ");
	if (flags.hybridrandomizer) Sprintf(eos(string), "randomizer ");
	if (flags.hybridnullrace) Sprintf(eos(string), "null ");
	if (flags.hybridmazewalker) Sprintf(eos(string), "mazewalker ");
	if (flags.hybridsoviet) Sprintf(eos(string), "soviet ");
	if (flags.hybridxrace) Sprintf(eos(string), "x-race ");
	if (flags.hybridheretic) Sprintf(eos(string), "heretic ");
	if (flags.hybridsokosolver) Sprintf(eos(string), "sokosolver ");
	if (flags.hybridspecialist) Sprintf(eos(string), "specialist ");
	if (flags.hybridamerican) Sprintf(eos(string), "american ");
	if (flags.hybridminimalist) Sprintf(eos(string), "minimalist ");
	if (flags.hybridnastinator) Sprintf(eos(string), "nastinator ");

	if (!(flags.hybridangbander) && !(flags.hybridaquarian) && !(flags.hybridcurser) && !(flags.hybridhaxor) && !(flags.hybridhomicider) && !(flags.hybridsuxxor) && !(flags.hybridwarper) && !(flags.hybridrandomizer) && !(flags.hybridnullrace) && !(flags.hybridmazewalker) && !(flags.hybridsoviet) && !(flags.hybridxrace) && !(flags.hybridheretic) && !(flags.hybridsokosolver) && !(flags.hybridspecialist) && !(flags.hybridamerican) && !(flags.hybridminimalist) && !(flags.hybridnastinator)) Sprintf(eos(string), "none ");

    return string;
}

const char *
hybrid_strcode()
{
    char string[BUFSZ];
    *string = '\0';
	if (flags.hybridangbander) Sprintf(eos(string), "Agb");
	if (flags.hybridaquarian) Sprintf(eos(string), "Aqu");
	if (flags.hybridcurser) Sprintf(eos(string), "Cur");
	if (flags.hybridhaxor) Sprintf(eos(string), "Hax");
	if (flags.hybridhomicider) Sprintf(eos(string), "Hom");
	if (flags.hybridsuxxor) Sprintf(eos(string), "Sux");
	if (flags.hybridwarper) Sprintf(eos(string), "War");
	if (flags.hybridrandomizer) Sprintf(eos(string), "Ran");
	if (flags.hybridnullrace) Sprintf(eos(string), "Nul");
	if (flags.hybridmazewalker) Sprintf(eos(string), "Maz");
	if (flags.hybridsoviet) Sprintf(eos(string), "Sov");
	if (flags.hybridxrace) Sprintf(eos(string), "Xra");
	if (flags.hybridheretic) Sprintf(eos(string), "Her");
	if (flags.hybridsokosolver) Sprintf(eos(string), "Sok");
	if (flags.hybridspecialist) Sprintf(eos(string), "Spe");
	if (flags.hybridamerican) Sprintf(eos(string), "Ame");
	if (flags.hybridminimalist) Sprintf(eos(string), "Min");
	if (flags.hybridnastinator) Sprintf(eos(string), "Nas");

	if (!(flags.hybridangbander) && !(flags.hybridaquarian) && !(flags.hybridcurser) && !(flags.hybridhaxor) && !(flags.hybridhomicider) && !(flags.hybridsuxxor) && !(flags.hybridwarper) && !(flags.hybridrandomizer) && !(flags.hybridnullrace) && !(flags.hybridmazewalker) && !(flags.hybridsoviet) && !(flags.hybridxrace) && !(flags.hybridheretic) && !(flags.hybridsokosolver) && !(flags.hybridspecialist) && !(flags.hybridamerican) && !(flags.hybridminimalist) && !(flags.hybridnastinator)) Sprintf(eos(string), "none");

    return string;
}

void
mstatusline(mtmp)
register struct monst *mtmp;
{
	aligntyp alignment;
	char info[BUFSZ], monnambuf[BUFSZ];

	if (mtmp->ispriest || mtmp->data == &mons[PM_ALIGNED_PRIEST]
				|| mtmp->data == &mons[PM_ANGEL])
		alignment = EPRI(mtmp)->shralign;
	else
		alignment = mtmp->data->maligntyp;
	alignment = (alignment > 0) ? A_LAWFUL :
		(alignment < 0) ? A_CHAOTIC :
		A_NEUTRAL;

	info[0] = 0;
	if (mtmp->mtame) {	  Strcat(info, ", tame");
#ifdef WIZARD
	    if (wizard) {
		Sprintf(eos(info), " (%d", mtmp->mtame);
		if (!mtmp->isminion)
		    Sprintf(eos(info), "; hungry %ld; apport %d; abuse %d",
			EDOG(mtmp)->hungrytime, EDOG(mtmp)->apport, EDOG(mtmp)->abuse);
		Strcat(info, ")");
	    }
#endif
	}
	else if (mtmp->mpeaceful) Strcat(info, ", peaceful");
	else if (mtmp->mtraitor)  Strcat(info, ", traitor");
	if (mtmp->meating)	  Strcat(info, ", eating");
	if (mtmp->mcan)		  Strcat(info, ", cancelled");
	if (mtmp->mconf)	  Strcat(info, ", confused");
	if (mtmp->mblinded || !mtmp->mcansee)
				  Strcat(info, ", blind");
	if (mtmp->mstun)	  Strcat(info, ", stunned");
	if (mtmp->msleeping)	  Strcat(info, ", asleep");
#if 0	/* unfortunately mfrozen covers temporary sleep and being busy
	   (donning armor, for instance) as well as paralysis */
	else if (mtmp->mfrozen)	  Strcat(info, ", paralyzed");
#else
	else if (mtmp->mfrozen || !mtmp->mcanmove)
				  Strcat(info, ", can't move");
#endif
				  /* [arbitrary reason why it isn't moving] */
	else if (mtmp->mstrategy & STRAT_WAITMASK)
				  Strcat(info, ", meditating");
	else if (mtmp->mflee) {	  Strcat(info, ", scared");
#ifdef WIZARD
	    if (wizard)		  Sprintf(eos(info), " (%d)", mtmp->mfleetim);
#endif
	}
	if (mtmp->mtrapped)	  Strcat(info, ", trapped");
	if (mtmp->mspeed)	  Strcat(info,
					mtmp->mspeed == MFAST ? ", fast" :
					mtmp->mspeed == MSLOW ? ", slow" :
					", ???? speed");
	if (mtmp->mundetected)	  Strcat(info, ", concealed");
	if (mtmp->minvis)	  Strcat(info, ", invisible");
	if (mtmp == u.ustuck)	  Strcat(info,
			(sticks(youmonst.data)) ? ", held by you" :
				u.uswallow ? (is_animal(u.ustuck->data) ?
				", swallowed you" :
				", engulfed you") :
				", holding you");
#ifdef STEED
	if (mtmp == u.usteed)	  Strcat(info, ", carrying you");
#endif
	if (mtmp->butthurt) Sprintf(eos(info), ", butthurt (%d)", mtmp->butthurt);

	/* avoid "Status of the invisible newt ..., invisible" */
	/* and unlike a normal mon_nam, use "saddled" even if it has a name */
	Strcpy(monnambuf, x_monnam(mtmp, ARTICLE_THE, (char *)0,
	    (SUPPRESS_IT|SUPPRESS_INVISIBLE), FALSE));

	pline("Status of %s (%s):  Level %d  HP %d(%d)  Pw %d(%d)  AC %d%s.",
		monnambuf,
		align_str(alignment),
		mtmp->m_lev,
		mtmp->mhp,
		mtmp->mhpmax,
		mtmp->m_en,
		mtmp->m_enmax,
		find_mac(mtmp),
		info);
}

void
mstatuslinebl(mtmp) /*for blessed stethoscope --Amy*/
register struct monst *mtmp;
{
	aligntyp alignment;
	char info[BUFSZ], monnambuf[BUFSZ];

	if (mtmp->ispriest || mtmp->data == &mons[PM_ALIGNED_PRIEST]
				|| mtmp->data == &mons[PM_ANGEL])
		alignment = EPRI(mtmp)->shralign;
	else
		alignment = mtmp->data->maligntyp;
	alignment = (alignment > 0) ? A_LAWFUL :
		(alignment < 0) ? A_CHAOTIC :
		A_NEUTRAL;

	info[0] = 0;
	if (mtmp->mtame) {	  Strcat(info, ", tame");
		Sprintf(eos(info), " (%d", mtmp->mtame);
		if (!mtmp->isminion)
		    Sprintf(eos(info), "; hungry %ld; apport %d",
			EDOG(mtmp)->hungrytime, EDOG(mtmp)->apport);
		Strcat(info, ")");
	}
	else if (mtmp->mpeaceful) Strcat(info, ", peaceful");
	else if (mtmp->mtraitor)  Strcat(info, ", traitor");
	if (mtmp->meating)	  Strcat(info, ", eating");
	if (mtmp->mcan)		  Strcat(info, ", cancelled");
	if (mtmp->mconf)	  Strcat(info, ", confused");
	if (mtmp->mblinded || !mtmp->mcansee)
				  Strcat(info, ", blind");
	if (mtmp->mstun)	  Strcat(info, ", stunned");
	if (mtmp->msleeping)	  Strcat(info, ", asleep");
#if 0	/* unfortunately mfrozen covers temporary sleep and being busy
	   (donning armor, for instance) as well as paralysis */
	else if (mtmp->mfrozen)	  Strcat(info, ", paralyzed");
#else
	else if (mtmp->mfrozen || !mtmp->mcanmove)
				  Strcat(info, ", can't move");
#endif
				  /* [arbitrary reason why it isn't moving] */
	else if (mtmp->mstrategy & STRAT_WAITMASK)
				  Strcat(info, ", meditating");
	else if (mtmp->mflee) {	  Strcat(info, ", scared");
	    Sprintf(eos(info), " (%d)", mtmp->mfleetim);
	}
	if (mtmp->mtrapped)	  Strcat(info, ", trapped");
	if (mtmp->mspeed)	  Strcat(info,
					mtmp->mspeed == MFAST ? ", fast" :
					mtmp->mspeed == MSLOW ? ", slow" :
					", ???? speed");
	if (mtmp->mundetected)	  Strcat(info, ", concealed");
	if (mtmp->minvis)	  Strcat(info, ", invisible");
	if (mtmp == u.ustuck)	  Strcat(info,
			(sticks(youmonst.data)) ? ", held by you" :
				u.uswallow ? (is_animal(u.ustuck->data) ?
				", swallowed you" :
				", engulfed you") :
				", holding you");
#ifdef STEED
	if (mtmp == u.usteed)	  Strcat(info, ", carrying you");
#endif

	/* avoid "Status of the invisible newt ..., invisible" */
	/* and unlike a normal mon_nam, use "saddled" even if it has a name */
	Strcpy(monnambuf, x_monnam(mtmp, ARTICLE_THE, (char *)0,
	    (SUPPRESS_IT|SUPPRESS_INVISIBLE), FALSE));

	pline("Status of %s (%s):  Level %d  HP %d(%d)  Pw %d(%d)  AC %d%s.",
		monnambuf,
		align_str(alignment),
		mtmp->m_lev,
		mtmp->mhp,
		mtmp->mhpmax,
		mtmp->m_en,
		mtmp->m_enmax,
		find_mac(mtmp),
		info);
}

void
ustatusline()
{
	char info[BUFSZ];

	info[0] = '\0';
	if (Sick) {
		Strcat(info, ", dying from");
		if (u.usick_type & SICK_VOMITABLE)
			Strcat(info, " food poisoning");
		if (u.usick_type & SICK_NONVOMITABLE) {
			if (u.usick_type & SICK_VOMITABLE)
				Strcat(info, " and");
			Strcat(info, " illness");
		}
	}
	if (Stoned)		Strcat(info, ", solidifying");
	if (Slimed)		Strcat(info, ", becoming slimy");
	if (Strangled)		Strcat(info, ", being strangled");
	if (Vomiting)		Strcat(info, ", nauseated"); /* !"nauseous" */
	if (Confusion)		Strcat(info, ", confused");
	if (Blind) {
	    Strcat(info, ", blind");
	    if (u.ucreamed) {
		if ((long)u.ucreamed < Blinded || Blindfolded
						|| !haseyes(youmonst.data))
		    Strcat(info, ", cover");
		Strcat(info, "ed by sticky goop");
	    }	/* note: "goop" == "glop"; variation is intentional */
	}
	if (Stunned)		Strcat(info, ", stunned");
	if (Numbed)		Strcat(info, ", numbed");
	if (Feared)		Strcat(info, ", stricken with fear");
	if (Frozen)		Strcat(info, ", frozen solid");
	if (Burned)		Strcat(info, ", burned");
#ifdef STEED
	if (!u.usteed)
#endif
	if (Wounded_legs) {
	    const char *what = body_part(LEG);
	    if ((Wounded_legs & BOTH_SIDES) == BOTH_SIDES)
		what = makeplural(what);
				Sprintf(eos(info), ", injured %s", what);
	}
	if (IsGlib)		Sprintf(eos(info), ", slippery %s",
					makeplural(body_part(HAND)));
	if (u.utrap)		Strcat(info, ", trapped");
	if (Fast)		Strcat(info, Very_fast ?
						", very fast" : ", fast");
	if (u.uundetected)	Strcat(info, ", concealed");
	if (Invis)		Strcat(info, ", invisible");
	if (u.ustuck) {
	    if (sticks(youmonst.data))
		Strcat(info, ", holding ");
	    else
		Strcat(info, ", held by ");
	    Strcat(info, mon_nam(u.ustuck));
	}

	pline("Status of %s (%s%s):  Level %d  HP %d(%d)  Pw %d(%d)  AC %d%s.",
		plname,
		    (u.ualign.record >= 20) ? "piously " :
		    (u.ualign.record > 13) ? "devoutly " :
		    (u.ualign.record > 8) ? "fervently " :
		    (u.ualign.record > 3) ? "stridently " :
		    (u.ualign.record == 3) ? "" :
		    (u.ualign.record >= 1) ? "haltingly " :
		    (u.ualign.record == 0) ? "nominally " :
					    "insufficiently ",
		align_str(u.ualign.type),
		Upolyd ? mons[u.umonnum].mlevel : u.ulevel,
		Upolyd ? u.mh : u.uhp,
		Upolyd ? u.mhmax : u.uhpmax,
		u.uen,
		u.uenmax,
		u.uac,
		info);
}

void
self_invis_message()
{
	if(Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) ){
	pline("%s %s.",
	    Hallucination ? "Arr, Matey!  Ye" : "Avast!  All of a sudden, ye",
	    See_invisible ? "can see right through yerself" :
		"can't see yerself");
	}
	else{
	pline("%s %s.",
	    Hallucination ? "Far out, man!  You" : "Gee!  All of a sudden, you",
	    See_invisible ? "can see right through yourself" :
		"can't see yourself");
	}
}

/* replace() from CK */

/*Consecutive calls to replace would result in strings copied onto itselves*/
/*So we alternate between two buffers*/
char            replace_buffer[BUFSZ*2]; /* two buffers */
unsigned int    flipflop=0;              /* which one ? */
/* flipflop must be unsigned int (not int or boolean) to act as senitel */
/* If it wrote beyond the end of buffer, flipflop is >1 ... */
/* ... then you can panic */

char *replace(st, orig, repl)
const char *st, *orig, *repl;
{
        char *buffer;
	char *ch;
        char *tmp;
        int i;  

        /* Direct speach ? (cheapest test, and only once ) */
        /*"Thou art doomed, scapegrace!" */
        /*"Who do you think you are, War?" */
        /*"Hello Dudley. Welcome to Delphi."*/
        if( (*st)== '"' ) return st;

        /* at most 20 times we replace the word to prevent infinite loops */
        i=100;
REPEAT:
        /*Most calls won't match, so do the match first.*/
	if (!(ch = strstr(st, orig)))
		return st;

        /* You read "eyelbereth" */
        /*if( (tmp = strstr(st,"read")) && (tmp<ch) ) return st;*/
	/* note by Amy: already caught by the direct speech test above */

        /* A cursed rusty iron chain named Eye Pod */
        /* if( (tmp = strstr(st,"named")) && (tmp<ch) ) return st; */

        /* A raven called Hugin - Eye of Odin */
        /* if( (tmp = strstr(st,"called")) && (tmp<ch) ) return st; */

        /* A tiger eye ring (that is called tiger eye in inventory) */
        /* if( !strcmp(orig,"eye") && strstr(st,"tiger") ) return st; */

        /* The Eye of the Aethiopica, The Eyes of the Overworld */
        /* if( !strcmp(orig,"Eye") && (
            strstr(ch,"Aethiopica") ||
            strstr(ch,"Overworld")
          )) return st; */

	/* note by Amy - I think the Deadlight of the Aethiopica sounds funny... */
	/* tiger deadlight ring etc. too :D */

        /* Check if it will fit into the buffer */
        /* 2 is enough, but 5 is safer */
        /* Should be rare enough to come late */
        if( ( strlen(st)+strlen(repl)-strlen(orig)+5 )>BUFSZ ) return st;

/*Quote beginning in the middle of the string */
/*The voice of Moloch booms out "So, mortal!  You dare desecrate my High Temple!"*/
/* voice o' Moloch, but not Ye dare */
/* rare enough, to come last */
        if( (tmp=strstr(st,"\"")) && (tmp<ch) ) return st;

        /* Don't convert disorder messages into pirate slang ! */
        /* Nested calls of impossible() call panic(). */
        if(program_state.in_impossible) return st;

        /* get a buffer */
        buffer = &replace_buffer[(flipflop^=1)?BUFSZ:0];

        /* If it is the same buffer, something went wrong. */
        /* This may happen if you work with two strings at the same time */
        /* and make intersecting calls to replace */
        if(buffer==st)
        {
         /* Assert a disorder if in wizard mode */
         /* otherwise do it silently */
         if(wizard)
           impossible("Intersecting calls to replace() in pline.c !");
         return st;
        }

        strncpy(buffer, st, ch-st);  
	buffer[ch-st] = 0;
        sprintf(buffer+(ch-st), "%s%s", repl, ch+strlen(orig));

        /* we don't know how much data was destroyed, so assume the worst */
        if(flipflop>1)
          panic("Memory leak in replace() !");

        st=buffer;
        if(i--) goto REPEAT;
        return st;
}

#endif /* OVLB */
/*pline.c*/



/*	SCCS Id: @(#)sp_lev.c	3.4	2001/09/06	*/
/*	Copyright (c) 1989 by Jean-Christophe Collet */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This file contains the various functions that are related to the special
 * levels.
 * It contains also the special level loader.
 *
 */

#include "hack.h"
#include "dlb.h"
/* #define DEBUG */	/* uncomment to enable code debugging */

#ifdef DEBUG
# ifdef WIZARD
#define debugpline	if (wizard) pline
# else
#define debugpline	pline
# endif
#endif

#include "sp_lev.h"
#include "rect.h"

extern void FDECL(mkmap, (lev_init *));

STATIC_DCL void FDECL(get_room_loc, (schar *, schar *, struct mkroom *));
STATIC_DCL void FDECL(get_free_room_loc, (schar *, schar *, struct mkroom *));
STATIC_DCL void FDECL(create_trap, (trap *, struct mkroom *));
STATIC_DCL int FDECL(noncoalignment, (ALIGNTYP_P));
STATIC_DCL void FDECL(create_monster, (monster *, struct mkroom *));
STATIC_DCL void FDECL(create_object, (object *, struct mkroom *));
STATIC_DCL void FDECL(create_engraving, (engraving *,struct mkroom *));
STATIC_DCL void FDECL(create_stairs, (stair *, struct mkroom *));
STATIC_DCL void FDECL(create_altar, (altar *, struct mkroom *));
STATIC_DCL void FDECL(create_gold, (gold *, struct mkroom *));
STATIC_DCL void FDECL(create_feature, (int,int,struct mkroom *,int));
STATIC_DCL boolean FDECL(search_door, (struct mkroom *, xchar *, xchar *,
					XCHAR_P, int));
STATIC_DCL void NDECL(fix_stair_rooms);
STATIC_DCL void FDECL(create_corridor, (corridor *));

STATIC_DCL boolean FDECL(create_subroom, (struct mkroom *, XCHAR_P, XCHAR_P,
					XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P));

STATIC_DCL void FDECL(mkfeature,(int,int));

/* Epitaphs for random headstones */
static const char *epitaphs[] = {
	"Rest in peace",
	"R.I.P.",
	"Rest In Pieces",
	"Note -- there are NO valuable items in this grave",
	"1994-1995. The Longest-Lived Hacker Ever",
	"The Grave of the Unknown Hacker",
	"We weren't sure who this was, but we buried him here anyway",
	"Sparky -- he was a very good dog",
	"Beware of Electric Third Rail",
	"Made in Taiwan",
	"Og friend. Og good dude. Og died. Og now food",
	"Beetlejuice Beetlejuice Beetlejuice",
	"Look out below!",
	"Please don't dig me up. I'm perfectly happy down here. -- Resident",
	"Postman, please note forwarding address: Gehennom, Asmodeus's Fortress, fifth lemure on the left",
	"Mary had a little lamb/Its fleece was white as snow/When Mary was in trouble/The lamb was first to go",
	"Be careful, or this could happen to you!",
	"Soon you'll join this fellow in hell! -- the Wizard of Yendor",
	"Caution! This grave contains toxic waste",
	"Sum quod eris",
	"Here lies an Atheist, all dressed up and no place to go",
	"Here lies Ezekiel, age 102.  The good die young.",
	"Here lies my wife: Here let her lie! Now she's at rest and so am I.",
	"Here lies Johnny Yeast. Pardon me for not rising.",
	"He always lied while on the earth and now he's lying in it",
	"I made an ash of myself",
	"Soon ripe. Soon rotten. Soon gone. But not forgotten.",
	"Here lies the body of Jonathan Blake. Stepped on the gas instead of the brake.",
	"Go away!",
	/* From SLASH'EM */
	"This old man, he played one, he played knick-knack on my thumb.",
	"Don't be daft, they couldn't hit an elephant at this dist-",
	"I wonder what deleting 'system32' does?",
	"If wizards can do the tengu shuffle, I bet tourists can do the fox-trot...",
	"Kai su, teknon?",
	"So this is what it feels like to be buried alive. - Last words of Indiana Jones 1889-1946",
	"This is what happens to you if you try to kill me - The Chromatic Dragon.",
	"Wonder what this mushroom does?...",
	"Y'all watch this!",
	"You call that a punch? I'll show you a punch--",
	"$dog",
	"$playername experienced YASD",
	"$playername took war to Team Ant and bought this fine track of land as benefits.",
	"$playername, Hand of Elbereth",
	"$playername, Killed at the hands of the Random Number God.",
	"$playername, demonstrating the value of not being seen.",
	"$playername, killed by a time-travel accident",
	"$playername, killed by an ambush, while reading",
	"$playername, killed by one-more-hit syndrome",
	"'Tis but a scratch!",
	"'Tis just a flesh wound...",
	"(This space for sale.)",
	"*Reserved for You*",
	"--More--",
	"...",
	"...for famous men have the whole earth as their memorial",
	"0AJQp",
	"0B6x8",
	"0EfGr",
	"0O7dW",
	"0X5mr",
	"2Bnxa",
	"2zr4S",
	"3sDlZ",
	"4ba9W",
	"4iyBA",
	"4nLCG",
	"4oY9C",
	"5cfQi",
	"5h5J0",
	"6dq9u",
	"6i9ma",
	"6q909",
	"6sEe6",
	"6z28Z",
	"7WRTy",
	"7jN66",
	"83gTA",
	"9h5Gq",
	"9qdzJ",
	"9wRBD",
	"<Privet!>!",
	"A is for Amy who fell down the stairs.",
	"A last bastion of rationality in a world where imbecility reigns supreme.",
	"A man of few words to the very end, now a man of even fewer words.",
	"A man's character is his fate",
	"A warning: Avoid the oysters at the local buffet line.",
	"ADkS1",
	"AFK",
	"ALL YOUR BASE ARE BELONG TO US",
	"ASYcW",
	"About the Zote what can be said? There was just one and now it's dead.",
	"Ach, Hans, run! It's the lhurgoyf!",
	"Acta est fabula.",
	"Ag friend. Eg good dude. Ig died. Og now Ig",
"Alas fair Death, 'twas missed in life - some peace and quiet from my wife",
"Alas, poor Bob. He almost asended. Only to fall into a pit of spikes.",
"Alas, poor Yorick!",
"Always look on the bright side of life",
"Another one bites the dust.",
"Any advice on risk management?",
"Anyone wanna switch places?",
"Apparently, you *can't* kill huge green fierce dragons with your bare hands.",
"Applaud, my friends, the comedy is finished.",
"As you are now, I once was. As I am now, you shall be. Um, unless you're immortal.",
"At last... a nice long sleep.",
"At least I didn't get eaten by $dog or $cat.",
"Ate that little packet of silica gel labelled Do Not Eat.",
"Audi Partem Alteram",
"Aut neca aut necatis eris",
"B is for Basil, assaulted by bears",
"B0spY",
"BPEsx",
"Beetlejuice Beetlejuice Beetlejuice",
"Beloved sister, devoted friend. She saved the world a lot.",
"Beware the killer bunny!",
"Blasted here by a SLASH'EM rocket.",
"Bulk deals on gravestones - Die today!",
"Burninated.",
"COME AT ME BRO",
"Casper the Friendly Boy",
"Clint Eastwood",
"Closed for inventory",
"Come on Mary, shoot! You couldn't hit the broad side of a barn!",
"Cut off the 'Do not remove under penalty of law' tag on a matress. Got death penalty.",
"CzXSY",
"David Blaine 1973 -",
"Dead Men Tell no Tales",
"Death is but a door. Time is but a window. I'll be back!",
"Death seeketh us all.",
"Deleted system32",
"Desynchronized...",
"Did you know that death rays bounce?",
"Die? [yn] (n)",
"Died by fighting what I thought that purple h was a dwarf king was actually a mind flayer.",
"Died by losing the game.",
"Died? Again? Damn.",
"Dig me up for a good time",
"Digging up this grave can create arch-lich",
"Disco - July 12, 1979",
"DjjFu",
"Do not open until Christmas",
"Do not play while drinking.",
"Don't Try",
"Don't do whatever it was I did wrong.",
"Don't even think about reaching for that pickaxe shorty! or I'll havta bite ya.",
"Don't forget to stop and smell the roses",
"Don't let this happen to you!",
"Don't try the fish",
"Dulce et decorum est pro patria mori",
"E6Cdf",
"EAT AT JOES",
"ECCPz",
"EGE6X",
"ET IN ARCADIA EGO",
"Eaten by a Grue",
"EdQji",
"Enter here for invisible zombie rave!",
"Fatty and skinny went to bed. Fatty rolled over and skinny was dead. Skinny Smith 1983-2000.",
"Feed me. I'm starving.... come to think of it... i havn't eatin' since 2002.",
"Fgsfds",
"Finally I am becoming stupider no more",
"Flooring inspector.",
"Follow me to hell.",
"Follow me to hell.",
"Fool me once, shame on you. Fool me twice, ...",
"For you, scum; only 700,000,000 for this grave reserve.",
"GE8BT",
"Game over, man. Game over.",
"Ganked again...",
"George W. Bush had everything and look where he ended up. No.. not up there u idiot... down here.",
"Go Team Ant!",
"Go Team Ant!",
"Go away",
"Go away! I'm trying to take a nap in here! Bloody adventurers...",
"Go to Hell - Do not pass go, do not collect 200 zorkmids",
"God, Killed by Logic",
"Gone fishin'",
"Good night, sweet prince: And flights of angels sing thee to thy rest!",
"Greg didn't fit anywhere, accept in this grave!",
"Greg the Ghoul's All Nite Diner: All you can eat, under your feet",
"Guybrush Threepwood, Mitey Pirate",
"GwNmL",
"H is for Hector done in by a thug",
"HEEM SLEEPY",
"Hanniwa",
"Haters gonna hate",
"He died at the console, of hunger and thirst. Next day he was buried, face-down, nine-edge first.",
"He farmed his way here",
"He shouldn't have fed it.",
"He waited too long",
"Help! I'm trapped in a gravestone factory!",
"Here Lies Patient Zero",
"Here Rest The Three Stooges",
"Here lays Butch, we planted him raw, he was quick on the trigger but slow on the draw.",
"Here lies $cat killed by $playername",
"Here lies $playername. He never scored.",
"Here lies /usr/bin/nethack, killed by SIGSEGV.",
"Here lies @",
"Here lies Acid blob killed by bob",
"Here lies Balin, son of Fundin, Lord of Moria",
"Here lies Bob / decided to try an acid blob",
"Here lies Bugs Bunny. His doc couldn't figure out what was up.",
"Here lies Chester Moores, killed by a poisoned corpse. Stayed dead - his killer has no re-Moores.",
"Here lies Curious George. Reached for a candy bar, crushed by a morning star.",
"Here lies Dudley, ate a ring of death while polymorphed (ever tried?)",
"Here lies Dudley, killed by an exploding ring of full warning",
"Here lies Dudley, killed by another %&#@#& newt.",
"Here lies Dudley. Drank a potion of death (ever found one?)",
"Here lies Ed. He was a dork.",
"Here lies Edd. He was an even bigger dork.",
"Here lies Eddie. He lied, he cheated, he stole. Sadly he's dead, God bless his soul.",
"Here lies Elric killed by touching Stormbringer",
"Here lies Elric, killed by touching Stormbringer",
"Here lies Elvis. Yes, he's really dead.",
"Here lies Ezekiel. Died of cholera.",
"Here lies Fred/ Fred the dead/ Fred was in bed/ Then he ate Bread/ Now Fred is dead.",
"Here lies God, killed by Nietzsche",
"Here lies Gregg. Choked on an egg.",
"Here lies Gronkh, blasted by a Creeper",
"Here lies Henry. May he find that place called Paradise.",
"Here lies Heywood Ucuddleme, a orc who never had a hug.",
"Here lies Izchak, shopkeeper and shopmaker.",
"Here lies Jeff, he angered the shopkeeper, again",
"Here lies Joe Hacker, starved to death while playing nethack.",
"Here lies Johnny Blake/ Stepped on the gas instead of the brake.",
"Here lies Lament. Killed by nerdrage over a wand of wishing.",
"Here lies Lester Moore. 4 shots from a .44. No less... No Moore.",
"Here lies Lies. It's True.",
"Here lies Mallory Pike. She had a mean serve.",
"Here lies Nietzsche, killed by Gog",
"Here lies Original Bubs. He can see you from here.",
"Here lies Owen Moore; gone away, owin' more than he could pay.",
"Here lies Paul. You knew this was coming.",
"Here lies Poot. He is dead.",
"Here lies Sgeo, killed while reading a gravestone.",
"Here lies Steve Irwin: Rememberance for his incredible journeys.",
"Here lies Strong Bad, Checking e-mails and kicking Cheats in the hereafter,",
"Here lies The Lady's maid, died of a Vorpal Blade.",
"Here lies Tom killed by Jerry",
"Here lies Urist McDwarf. Do NOT strike the earth here, please.",
"Here lies Urist McMiner, killed by falling multiple z-levels.",
"Here lies a hacker, buried alive.",
"Here lies a happy hacker, killed by a hallucinogen-distorted succubus, while helpless.",
"Here lies a hobbit of the Shire, died of a wand of fire.",
"Here lies a jerk who forgot to pay.",
"Here lies a man who never backed down.",
"Here lies a man. His name is Robert Paulson.",
"Here lies a man. His name is Robert Paulson.",
"Here lies a programmer. Killed by a fatal error.",
"Here lies a samurai named Ken.",
"Here lies an Irishman, a good lawyer and an honest man. It's pretty crowded down there.",
"Here lies andy. Peperony and chease.",
"Here lies foo. Killed by 4chan.",
"Here lies good old Ned. A great big rock fell on his head.",
"Here lies ron3090, killed by kicking a ******* wall.",
"Here lies ron3090, killed by kicking a fucking wall.",
"Here lies scwizard, killed while tying to assent.",
"Here lies the Gnome with a Wand of Death, vanquished by a black dragon's breath. Damn RNG.",
"Here lies the body of Johann Gambolputty de von Ausfern-schplenden-schlitter-crasscrenbon --More--",
"Here lies the dumbest son-of-a-b**** ever to set foot in the Dungeons of Doom.",
"Here lies the dumbest son-of-a-bitch ever to set foot in the Dungeons of Doom.",
"Here lies the english language, killed by instant messaging",
"Here lies the left foot of Jack, killed by a land mine. Let us know if you find any more of him.",
"Here lies the man from Nantucket. He was attacked by a orc, what was left was put into a bucket.",
"Here lies the planet earth. don't dig it.",
"Here lies yetanotherhacker, killed by the wrath of RNG",
"Here lies you, in preparation.",
"Hey, get off my grave!",
"His last words? 'What throne room?'",
"Hmm... What's tha--",
"I Used Up All My Sick Days, So I Called in Dead",
"I admit no wrongdoing!",
"I ain't even mad.",
"I am not human. This mind and body are constructs. Yes, as is this sorrow.",
"I drank *what*, now?",
"I iz ded. Ded kittehs dont need bafs.",
"I need rest...",
"I never could get the hang of Thursdays.",
"I said I'm not dead yet!",
"I told him it wasn't edible...",
"I told you I was ill",
"I told you I was sick!",
"I took both pills!",
"I used to search for the Amulet of Yendor, but then I took an arrow to the knee.",
"I was thirsty.",
"I will survive!",
"I wonder what cockatrice tastes like..",
"I wonder what that little ^ sign is...",
"I wonder what this button does?",
"I wonder what this potion does...",
"I wonder what this potion does...",
"I'd rather be sailing.",
"I'll be back.",
"I'll live to die another day... or not.",
"I'm actually quite busy on the cellular level.",
"I'm gonna make it!",
"I've really been a fool",
"IWwKl",
"If a man's deeds do not outlive him, of what value is a mark in stone?",
"If only I had worn a hard hat . . .",
"If you were dead, you'd be home by now!",
"In honor of Dudley, choked to death on a ring of invisibility",
"In remembrance of Spike Milligan: He told you he was sick.",
"It looks like I'm going to have to jump...",
"It's nothing... it's nothing...",
"It's only a flesh wound; nothing serious.",
"James Brown is dead.",
"Jeanne -- She died and we did nothing.",
"JoXoD",
"JrDSt",
"Just one more hit . . .",
"Just resting a while.",
"KYNMh",
"Killed by a black dragon -- This grave is empty",
"Killed by a succubus with AIDS.",
"Killed by an improbability",
"Killed by playing SLASH'EM",
"Killed in a teleporting accident.",
"Killed over a dilithium crystal.",
"Last Words: Cheeseburger Please.",
"Last words are for wimps",
"Leeroy Jenkins - Killed by overconfidence",
"Let me out of here!",
"Life's a piece of shit / when you look at it / life's a laugh and death's a joke, it's true.",
"Lions and Tigers and Bears! Oh My!",
"Little Johnny was a chemist. Now he is no more. 'Cause what he thought was H20, was H2SO4.",
"Lm1xg",
"LnAwY",
"Lookin' good, Medusa.",
"Lorem Ipsum",
"LqAWq",
"M-M-M-M-M-MONSTER KILL!",
"MDrdP",
"MMg5P",
"Mental note: Hold sword by blunt end.",
"Mistakenly performed a sword swallowing stunt with a vorpal blade.",
"Mrs. Smith, choked on an apple. She left behind grieving husband, daughter, and granddaughter.",
"My only regret was eating my 40th pancake",
"My only regret was not having had more soda. -- Keith Forbes",
"Myrnd",
"N is for Nevill who died of ennui",
"N9MDx",
"NONE OF YOU ARE SAFE",
"NOOOOOOOOOOOOOOOOOOOOOOOOOOOO!!!!!!!!",
"Nah, that scorpion wasn't poisonous, i'm sure of it.",
"National Park. Digging is prohibited.",
"Never get involved in a land war in Asia.",
"Never go in against a Sicilian when death is on the line.",
"Newb",
"Nietzsche is dead. -- God",
"NmcOi",
"No! I don't want to see my damn conduct!",
"Nobody believed her when she said her feet were killing her",
"Not another ******* floating eye.",
"Not another fucking floating eye.",
"Nqtb1",
"OMG LAG",
"OYl1A",
"Oh Zed, Woe Zed. No where left to go Zed. All alone, and so dead. Poor Zed.",
"Oh man I am so psyched for this.",
"Oh, no. Not again.",
"Omae wa mo shindeiru!",
"On Vacation",
"On the whole, I'd rather be in Minetown.",
"One corpse, sans head.",
"Only YOU can prevent fountain overflow",
"Oooh, what's this scroll do?",
"Oops.",
"Out to Lunch",
"PLK0H",
"Paula Schultz",
"PhbRM",
"Pwnd!",
"Q57Et",
"Q8Qwi",
"QBywK",
"Quidquid Latine dictum sit, altum sonatur.",
"Quintili Vare, legiones redde!",
"R.I.P John Jacob Jingleheimer Schmidt - His name was my name too",
"R3PmA",
"RIP @",
"RIP Bort Simpesson",
"RIP Chingy, fatally killed by wasps.",
"RIP Chingy, stung by wasps.",
"RIP Guy Fieri, stung by wasps.",
"RIP Stebe Job, stung to death by wasps.",
"RIP, better you than me",
"Repeat after me: Hastur Hastur Hastur!",
"RmR0w",
"Roses are red, Violets are blue, Omae Wa Mo Shindeiru.",
"RtEjE",
"S6qzn",
"S8MXt",
"SOLD",
"Save me Jebus!",
"Segmentation fault: core dumped.",
"Shouldn't have given away my last weapon.",
"Sic Transit Gloria Mundi",
"Since the day of my birth, my death began its walk. It is walking towards me, without hurrying.",
"Since when does a failed polymorph kill you?",
"Slipped while mounting a succubus.",
"Snape, killed by a Giant Mummy. The Giant Mummy hits! You die...",
"So much time, so little to do",
"Some crazy bastard waved me over so he could hit me. Why did I go over there?",
"Someone set us up the bomb!",
"Something in my inventory could have saved me.",
"SsHwF",
"Stan's Kozy Krypts: A Place To Spend Eternity, Not A Fortune.",
"Stan's Kozy Krypts: We bury the dead, for a lot less bread.",
"Steven Paul Jobs, 1955-2011. Thanks for the future.",
"TO LET",
"TONIGHT! WE DINE! IN HELL!",
"TVTropes ruined my life",
"TXfJ9",
"Take my stuff- I don't need it anymore.",
"Taking a year dead for tax reasons.",
"Ted Danson",
"Teracotta Warrior",
"That is not dead which can eternal lie, and in successful ascensions even Death may die.",
"That is not dead which can eternal lie. I, on the other hand...",
"That is not dead which can eternal lie. And with strange aeons even death may die.",
"That zombie had armor. I wanted armor.",
"That zombie had armor. I wanted armor.",
"The cake is a lie!",
"The invisible jabberwock hits! You die...",
"The keys were right next to each other",
"The reports of my demise are completely accurate.",
"The statue got me high.",
"There goes the neighborhood.",
"This Space Reserved",
"This could by YOU!",
"This grave is reserved - for you!",
"This gravestone does not indicate a bones file.",
"This gravestone provided to you by FreeStone.",
"This headstone intentionally left blank.",
"This is a gravestone, not an altar Trololololololol",
"This is on me.",
"This man died because he sucked at nethack.",
"This space for rent; go to http://www.alt.org/nethack/addmsgs/",
"This space intentionally left blank.",
"This was a triumph.",
"This was actually just a pit, but since there was a corpse, we filled it.",
"This way to the crypt.",
"TlK5K",
"To the strongest!",
"Trolls just want to be loved!",
"Tu quoque, Brute?",
"Turns out, you really shouldn't use the phone during a thunderstorm.",
"Twilight Sparkle, killed while fighting sapient $fruit.",
"Uisne Agnoscere Mercem?",
"UsiJF",
"VACANCY",
"WHO'S THEY AND WHY DO THEY KEEP SAYING IT? - Died to bad advice.",
"Wait for that wisest of all counselors, Time",
"We introduced Eg to the giant, but he misunderstood.",
"We wonder what kil",
"Welcome!",
"What could possibly go wrong?",
"What happen when I press the button?",
"What the hell is genocidal confusion?",
"When you find yourself in a hole, stop digging.",
"When you zap a wand of death, make sure you're pointing it the right way.",
"Where were you Kos?",
"Wheres the beef?!",
"While we are postponing, life speeds by.",
"Whoops.",
"Whoops....",
"Why, oh why didn't I take the BLUE pill?",
"Wipe your feet before entering",
"Wish me best of luck in my future endeavors....",
"Wish you were here!",
"Worse things happen at sea, you know.",
"WqLjM",
"X66P9",
"XXxyZ",
"Y is for Yorick whose head was knocked in",
"Y.A.S.D.",
"YAAD",
"YASD",
"YASD'd!",
"Yea, it got me too.",
"You asked me if it would kill me to be civil...well, now you know.",
"You come from nothing, you're going back to nothing. What have you lost? Nothing!",
"You die... --More--",
"You should see the other guy.",
"You're never too dead to learn.",
"You're standing on my head.",
"Your very silence shows you agree",
"Zed's dead, baby.",
"Zed's dead, baby. Zed's dead.",
"addFU",
"asasa",
"asdg",
"brb",
"bvOwX",
"cAWZB",
"cGf2N",
"cg5Vx",
"confusion will be my epitaph",
"cxjUZ",
"cy4Wa",
"doQmo",
"dxekm",
"edw",
"eee3e",
"enPWu",
"fW6Ye",
"fj2In",
"fopBQ",
"g3cwM",
"g5NJc",
"gIWDx",
"gwNNZ",
"hVhfq",
"here lies Krass the cheapskate... still owes me 10 Gold for engraving this gravestone...",
"here lies andy -- peperoni and cheese",
"hukQY",
"j16IV",
"jVHLs",
"jeBrG",
"kGIOZ",
"lgALT",
"logout;",
"lol",
"lzBU6",
"m4xlc",
"mde.tv",
"mkelO",
"mv170",
"mxQ0M",
"n8jyF",
"never try to kill a shopkeeper who has a wand of sleep, it'll be the last thing you do.",
"njnm",
"oPbQR",
"oPlHb",
"ojN8O",
"on7ZP",
"or2SE",
"p0RFR",
"pWRbl",
"pz6Yk",
"q4JhO",
"qJOkM",
"qnnZA",
"quit, on",
"rACim",
"rest in peace dudley killed by a newt AGAIN!!!!",
"rlDrk",
"romanes eunt domus",
"ruNc2",
"sX2l3",
"sbpzV",
"slXdb",
"swyyI",
"test",
"test test",
"tlbto",
"tm5QP",
"try #quit",
"u5fPY",
"uE54o",
"uWD2q",
"uoZXt",
"wK3qm",
"wKJBc",
"wKiLp",
"wc8Zl",
"wzURY",
"x982a",
"xS3K6",
"xdwdwewewrwerfew",
"xyO0Y",
"yY5OU",
"zKKBC",
"zZIk8",
"zmtgs",
	"Alas fair Death, 'twas missed in life - some peace and quiet from my wife",
	"Applaud, my friends, the comedy is finished.",
	"At last... a nice long sleep.",
	"Audi Partem Alteram",
	"Basil, assaulted by bears",
	"Burninated",
	"Confusion will be my epitaph",
	"Do not open until Christmas",
	"Don't be daft, they couldn't hit an elephant at this dist-",
	"Don't forget to stop and smell the roses",
	"Don't let this happen to you!",
	"Dulce et decorum est pro patria mori",
	"Et in Arcadia ego",
	"Fatty and skinny went to bed.  Fatty rolled over and skinny was dead.  Skinny Smith 1983-2000.",
	"Finally I am becoming stupider no more",
	"Follow me to hell",
	"...for famous men have the whole earth as their memorial",
	"Game over, man.  Game over.",
	"Go away!  I'm trying to take a nap in here!  Bloody adventurers...",
	"Gone fishin'",
	"Good night, sweet prince: And flights of angels sing thee to thy rest!",
	"Go Team Ant!",
	"He farmed his way here",
	"Here lies Lies. It's True",
	"Here lies the left foot of Jack, killed by a land mine.  Let us know if you find any more of him",
	"He waited too long",
	"I'd rather be sailing",
	"If a man's deeds do not outlive him, of what value is a mark in stone?",
	"I'm gonna make it!",
	"I took both pills!",
	"I will survive!",
	"Killed by a black dragon -- This grave is empty",
	"Let me out of here!",
	"Lookin' good, Medusa.",
	"Mrs. Smith, choked on an apple.  She left behind grieving husband, daughter, and granddaughter.",
	"Nobody believed her when she said her feet were killing her",
	"No!  I don't want to see my damn conduct!",
	"One corpse, sans head",
	"On the whole, I'd rather be in Minetown",
	"On vacation",
	"Oops.",
	"Out to Lunch",
	"SOLD",
	"Someone set us up the bomb!",
	"Take my stuff, I don't need it anymore",
	"Taking a year dead for tax reasons",
	"The reports of my demise are completely accurate",
	"(This space for sale)",
	"This was actually just a pit, but since there was a corpse, we filled it",
	"This way to the crypt",
	"Tu quoque, Brute?",
	"VACANCY",
	"Welcome!",
	"Wish you were here!",
	"Yea, it got me too",
	"You should see the other guy",
	"...and they made me engrave my own headstone too!",
	"<Expletive Deleted>",
	"Adapt. Enjoy. Survive.",
	"Adventure, hah! Excitement, hah!",
	"After all, what are friends for...",
	"After this, nothing will shock me",
	"Age and treachery will always overcome youth and skill",
	"Ageing is not so bad.  The real killer is when you stop.",
	"Ain't I a stinker?",
	"Algernon",
	"All else failed...",
	"All hail RNG",
	"All right, we'll call it a draw!",
	"All's well that end well",
	"Alone at last!",
	"Always attack a floating eye from behind!",
	"Am I having fun yet?",
	"And I can still crawl, I'm not dead yet!",
	"And all I wanted was a free lunch",
	"And all of the signs were right there on your face",
	"And don't give me that innocent look either!",
	"And everyone died.  Boo hoo hoo.",
	"And here I go again...",
	"And nobody cares until somebody famous dies...",
	"And so it ends?",
	"And so... it begins.",
	"And sometimes the bear eats you.",
	"And then 'e nailed me 'ead to the floor!",
	"And they said it couldn't be done!",
	"And what do I look like?  The living?",
	"And yes, it was ALL his fault!",
	"And you said it was pretty here...",
	"Another lost soul",
	"Any day above ground is a good day!",
	"Any more of this and I'll die of a stroke before I'm 30.",
	"Anybody seen my head?",
	"Anyone for deathmatch?",
	"Anything for a change.",
	"Anything that kills you makes you ... well, dead",
	"Anything worth doing is worth overdoing.",
	"Are unicorns supposedly peaceful if you're a virgin?  Hah!",
	"Are we all being disintegrated, or is it just me?",
	"At least I'm good at something",
	"Attempted suicide",
	"Auri sacra fames",
	"Auribus teneo lupum",
	"Be prepared",
	"Beauty survives",
	"Been Here. Now Gone. Had a Good Time.",
	"Been through Hell, eh? What did you bring me?",
	"Beg your pardon, didn't recognize you, I've changed a lot.",
	"Being dead builds character",
	"Beloved daughter, a treasure, buried here.",
	"Best friends come and go...  Mine just die.",
	"Better be dead than a fat slave",
	"Better luck next time",
	"Beware the ...",
	"Bloody Hell...",
	"Bloody barbarians!",
	"Buried the cat.  Took an hour.  Damn thing kept fighting.",
	"But I disarmed the trap!",
	"Can YOU fly?",
	"Can you believe that thing is STILL moving?",
	"Can you come up with some better ending for this?",
	"Can you feel anything when I do this?",
	"Can you give me mouth to mouth, you just took my breath away.",
	"Can't I just have a LITTLE peril?",
	"Can't eat, can't sleep, had to bury the husband here.",
	"Can't you hit me?!",
	"Chaos, panic and disorder.  My work here is done.",
	"Check enclosed.",
	"Check this out!  It's my brain!",
	"Chivalry is only reasonably dead",
	"Coffin for sale.  Lifetime guarantee.",
	"Come Monday, I'll be all right.",
	"Come and see the violence inherent in the system",
	"Come back here!  I'll bite your bloody knees off!",
	"Commodore Business Machines, Inc.   Died for our sins.",
	"Complain to one who can help you",
	"Confess my sins to god?  Which one?",
	"Confusion will be my epitaph",
	"Cooties?  Ain't no cooties on me!",
	"Could somebody get this noose off me?",
	"Could you check again?  My name MUST be there.",
	"Could you please take a breath mint?",
	"Couldn't I be sedated for this?",
	"Courage is looking at your setbacks with serenity",
	"Cover me, I'm going in!",
	"Crash course in brain surgery",
	"Cross my fingers for me.",
	"Curse god and die",
	"Dead Again?  Pardon me for not getting it right the first time!",
	"Dead and loving every moment!",
	"Dear wife of mine. Died of a broken heart, after I took it out of her.",
	"Don't tread on me!",
	"Dragon? What dragon?",
	"Drawn and quartered",
	"Either I'm dead or my watch has stopped.",
	"Eliza -- Was I really alive, or did I just think I was?",
	"Elvis",
	"Enter not into the path of the wicked",
	"Eris?  I don't need Eris",
	"Eternal Damnation, Come and stay a long while!",
	"Even The Dead pay taxes (and they aren't Grateful).",
	"Even a tomb stone will say good things when you're down!",
	"Ever notice that live is evil backwards?",
	"Every day is starting to look like Monday",
	"Every day, in every way, I am getting better and better.",
	"Every survival kit should include a sense of humor",
	"Evil I did dwell;  lewd did I live",
	"Ex post fucto",
	"Excellent day to have a rotten day.",
	"Excuse me for not standing up.",
	"Experience isn't everything. First, You've got to survive",
	"First shalt thou pull out the Holy Pin",
	"For a Breath, I Tarry...",
	"For recreational use only.",
	"For sale: One soul, slightly used. Asking for 3 wishes.",
	"For some moments in life, there are no words.",
	"Forget Disney World, I'm going to Hell!",
	"Forget about the dog, Beware of my wife.",
	"Funeral - Real fun.",
	"Gawd, it's depressing in here, isn't it?",
	"Genuine Exploding Gravestone.  (c)Acme Gravestones Inc.",
	"Get back here!  I'm not finished yet...",
	"Go ahead, I dare you to!",
	"Go ahead, it's either you or him.",
	"Goldilocks -- This casket is just right",
	"Gone But Not Forgotten",
	"Gone Underground For Good",
	"Gone away owin' more than he could pay.",
	"Gone, but not forgiven",
	"Got a life. Didn't know what to do with it.",
	"Grave?  But I was cremated!",
	"Greetings from Hell - Wish you were here.",
	"HELP! It's dark in here... Oh, my eyes are closed - sorry",
	"Ha! I NEVER pay income tax!",
	"Have you come to raise the dead?",
	"Having a good time can be deadly.",
	"Having a great time. Where am I exactly??",
	"He died of the flux.",
	"He died today... May we rest in peace!",
	"He got the upside, I got the downside.",
	"He lost his face when he was beheaded.",
	"He missed me first.",
	"He's not dead, he just smells that way.",
	"Help! I've fallen and I can't get up!",
	"Help, I can't wake up!",
	"Here lies Pinocchio",
	"Here lies the body of John Round. Lost at sea and never found.",
	"Here there be dragons",
	"Hey, I didn't write this stuff!",
	"Hodie mihi, cras tibi",
	"Hold my calls",
	"Home Sweet Hell",
	"I KNEW this would happen if I lived long enough.",
	"I TOLD you I was sick!",
	"I ain't broke but I am badly bent.",
	"I ain't old. I'm chronologically advantaged.",
	"I am NOT a vampire. I just like to bite..nibble, really!",
	"I am here. Wish you were fine.",
	"I am not dead yet, but watch for further reports.",
	"I believe them bones are me.",
	"I broke his brain.",
	"I can feel it.  My mind.  It's going.  I can feel it.",
	"I can't go to Hell. They're afraid I'm gonna take over!",
	"I can't go to hell, they don't want me.",
	"I didn't believe in reincarnation the last time, either.",
	"I didn't mean it when I said 'Bite me'",
	"I died laughing",
	"I disbelieved in reincarnation in my last life, too.",
	"I hacked myself to death",
	"I have all the time in the world",
	"I knew I'd find a use for this gravestone!",
	"I know my mind. And it's around here someplace.",
	"I lied!  I'll never be alright!",
	"I like it better in the dark.",
	"I like to be here when I can.",
	"I may rise but I refuse to shine.",
	"I never get any either.",
	"I said hit HIM with the fireball, not me!",
	"I told you I would never say goodbye.",
	"I used to be amusing. Now I'm just disgusting.",
	"I used up all my sick days, so now I'm calling in dead.",
	"I was killed by <illegible scrawl>",
	"I was somebody. Who, is no business of yours.",
	"I will not go quietly.",
	"I'd give you a piece of my mind... but I can't find it.",
	"I'd rather be breathing",
	"I'll be back!",
	"I'll be mellow when I'm dead. For now, let's PARTY!",
	"I'm doing this only for tax purposes.",
	"I'm not afraid of Death!  What's he gonna do? Kill me?",
	"I'm not getting enough money, so I'm not going to engrave anything useful here.",
	"I'm not saying anything.",
	"I'm weeth stupeed --->",
	"If you thought you had problems...",
	"Ignorance kills daily.",
	"Ignore me... I'm just here for my looks!",
	"Ilene Toofar -- Fell off a cliff",
	"Is that all?",
	"Is there life before Death?",
	"Is this a joke, or a grave matter?",
	"It happens sometimes. People just explode.",
	"It must be Thursday. I never could get the hang of Thursdays.",
	"It wasn't a fair fight",
	"It wasn't so easy.",
	"It's Loot, Pillage and THEN Burn...",
	"Just doing my job here",
	"Killed by diarrhea of mouth and constipation of brain.",
	"Let her RIP",
	"Let it be; I am dead.",
	"Let's play Hide the Corpse",
	"Life is NOT a dream",
	"Madge Ination -- It wasn't all in my head",
	"Meet me in Heaven",
	"Move on, there's nothing to see here.",
	"My heart is not in this",
	"No one ever died from it",
	"No, you want room 12A, next door.",
	"Nope.  No trap on that chest.  I swear.",
	"Not again!",
	"Not every soil can bear all things",
	"Now I have a life",
	"Now I lay thee down to sleep... wanna join me?",
	"Obesa Cantavit",
	"Oh! An untimely death.",
	"Oh, by the way, how was my funeral?",
	"Oh, honey..I missed you! She said, and fired again.",
	"Ok, so the light does go off. Now let me out of here.",
	"One stone brain",
	"Ooh! Somebody STOP me!",
	"Oops!",
	"Out for the night.  Leave a message.",
	"Ow!  Do that again!",
	"Pardon my dust.",
	"Part of me still works.",
	"Please, not in front of those orcs!",
	"Prepare to meet me in Heaven",
	"R2D2 -- Rest, Tin Piece",
	"Relax.  Nothing ever happens on the first level.",
	"Res omnia mea culpa est",
	"Rest In Pieces",
	"Rest, rest, perturbed spirit.",
	"Rip Torn",
	"She always said her feet were killing her but nobody believed her.",
	"She died of a chest cold.",
	"So let it be written, so let it be done!",
	"So then I says, How do I know you're the real angel of death?",
	"Some patients insist on dying.",
	"Some people have it dead easy, don't they?",
	"Some things are better left buried.",
	"Sure, trust me, I'm a lawyer...",
	"Thank God I wore my corset, because I think my sides have split.",
	"That is all",
	"The Gods DO have a sense of humor: I'm living proof!",
	"This dungeon is a pushover",
	"This elevator doesn't go to Heaven",
	"This gravestone is shareware. To register, please send me 10 zorkmids",
	"This gravestone provided by The Yendorian Grave Services Inc.",
	"This is not an important part of my life.",
	"This one's on me.",
	"This side up",
	"Tone it down a bit, I'm trying to get some rest here.",
	"Virtually Alive",
	"We Will Meet Again.",
	"Weep not, he is at rest",
	"Welcome to Dante's.  What level please?",
	"Well, at least they listened to my sermon...",
	"Went to be an angel.",
	"What are you doing over there?",
	"What are you smiling at?",
	"What can you say, Death's got appeal...!",
	"What pit?",
	"When the gods want to punish you, they answer your prayers.",
	"Where's my refund?",
	"Will let you know for sure in a day or two...",
	"Wizards are wimps",
	"Worms at work, do not disturb!",
	"Would you mind moving a bit?  I'm short of breath down here.",
	"Would you quit being evil over my shoulder?",
	"Yes Dear, just a few more minutes...",
	"You said it wasn't poisonous!",

	/* from http://www.alt.org/nethack/addmsgs/viewmsgs.php */
	"Balin, son of Fundin, Lord of Moria",
	"Memento mori",
	"Help! I'm trapped in a gravestone factory!",
	"This grave is reserved - for you!",
	"This gravestone does not indicate a bones file.",
	"This space for rent; go to http://www.alt.org/nethack/addmsgs/"
	"Here lies Sgeo, killed while reading a gravestone.",
	"Quidquid Latine dictum sit, altum videtur.",
	"Death is but a door. Time is but a window. I'll be back!",
	"The reports of my demise are completely accurate.",
	"Bulk deals on gravestones - Die today!",

	/* from UnNetHack */
	"Hack 1984-1985",
	"NetHack 1987-2003",

	/* from UnNetHackPlus */
	"SporkHack 2007-2010",
	"SLASH'EM 1997-2006",
	"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
	"Lost to the guy who is passway!",
	"Went through cave just now. Still spiritful.",
	"Tie with rope!",
	"Finally it will be return to the center!",
	"I will not suspend it.",
	"But the bag is full!",
	"We are defeated by Shangdao team!",
	"I will try best not to incommode. Everybody aftertime",
	"The wave is small, may is urf?",
	"Great waterfall! Can I drift?",
      "OH! OH! Draw it! Put it down it is excessively too!!",
	"Put it down what to do? Oh the badge... sorry for having forgotten it want to get a regularity badge.",
	"Give it to you after a while!",
	"There is some dimness inside. The skill is to experience the charm of pocket monster. Is it fit to me?",
	"I defeated her. I try my best, but still not strong and no confidence. If lost out try again nothing at all!",
	"I will not equest sister. It is good in this status, dear!",
	"I go to Xiaojin specially the bike isn't really solded, isn't it?",
	"It is safe for man too!",
	"It always lost in the game of monster. Always let it die because of it isn't closed with me",
	"Nobody is care of it/it's impossible/dislike to go out to play/it's good to play together",
	"Once, my husband has done nothing. Only fond of entertainment how can I do?",
	"I didn't call him, he had gone to game-shop.",
	"My husband is regarded as a monster cracy Zhenqian is like to his father!",
	"Oh!Loged on such at most!",
	"The trolley isn't in, it's impossible to carry the guest on my back to walk!",
"Once I walked in darkness I saw the moving-tree.",
"My sister felt funny and go to see it, I afraid that it was dangerous!",
"I told my elder sister about the moving-tree, she said it was dangerous, don't let me go unless I defeated Miss Yu.",
"Today many goods to be bought!",
"I wondered whose tel no. to be longed no?",
"I feel Xiaojin is in a big city while going to its store.",
"Buy a gift for my friends go to the second floor:Market!",
"Isn't it improved the gear while playing?",
"The feeling is very good when the special ability is improved!",
"A magical gift to send gift!",
"Elder sister will come on sunday to look the state of monster, take the skill machine!",
"The monster taken from others whose name isn't altered, because the name includes the feeling of a great man!",
"Tired while shopping. Go to the top floor to have a rest.",
"There is a thing I want very much, but the money isn't enough!",
"Look the distant with telescope. Our house maybe seen.Is it the green roof?",
"But the percentage of lose is low in fact, I like the card machine.",
"Losed to the coin machine losed to the card machine too!",
"It's regretted very much, don't find the object for exchange take the monster being kept!",
"No prop but mail!",
"It is a regret cannn't found the object for exchanging because of keeping for a long time!",
"Before contacting with the center, wrote a report.",
"This is a mystic egg be careful, please!",
"Its content is your note about exploration,and character synopsis. The telephone hasn't sent.",
"What a big monster's center! Very good, haveing so many new machines! As if it has been set up just!",
"An unknown girl gave me a cat just now. Do you believe that it will be fine when exchange with?",
"I was afraided when found my friend's name merge in the news!",
"After the that we can connect with various of people certainly.",
"The facility there can't be used now. It will feel good to go to the hearsaid place earlier!",
"Becoming famous, after the name merged in the news. All right, how to make one's name merge in the news?",
"It is excited to communicate! With this means first?",
"When exchanging at the exchange department, and seeing the news about monsters.",
"The main comes to manage center. Look out everybody, now the monster coachs around the countryuse the electric wave to communicate.",
"The forest looks strange. It isn't good to walk now.",
"There is sweet taste inside! The monster seems to go out.",
"Shangdao themselves look down on us, how to stand it!",
"Drive in the bikeway without hands the feeling is good!",
"Ssee the relic, there is a sense suddenly!",
"I think there is a great secret hidden in the relic!",
"The shaking just frightens me!But we are care about the wall very much.",
"Did the monster appear in the relic!? This is a great invent! We start to investigate it at once!",
"How many sort monsters are there in the relic?",
"The riddle's appearance has some causes certainly. Now we are invertigating this matter.",
"My result is the riddle's appearance appeared after the communion center had been built.",
"Clear up the rogatory content it is going to be published. If so,I'll be as famous as Wuzhimu Dr.",
"I am living there if you like,you may come to reside. There are many empty places.",
"Every week one monster always comes to waterside. Want to see the monster very much!",
"I droped from the rope and brode my waist, I cann't move.",
"Do well, the Missile Bombs escapes. My waist becomes well! We arw going to go back!",
"Here lies the Hunger Games trilogy. Slain by the romantic plot tumor named Peeta.",
"Here lies Bella Swan. Killed by her abusive bastard boyfriend Edward.",
"Here lies Jacob Black. He killed himself after he realized Edward would get Bella even though Jacob was a far better partner for her in every way.",
"Here lies Neville Longbottom. Guess Hermione finally stopped missing him with her Avada Kedavra curse.",
"Here lies Dolores Umbridge. Shot in the head by Hermione's Avada Kedavra death ray. Yep, Rowling - your characters should stop acting stupid and kill Umbridge already!",
"Here lies Lord Voldemort. Killed by his own reflecting death ray.",
"Here lies Bellatrix Lestrange. She took a knife to the head.",
"Here lies Peeta Mellark. He took a knife to the head and we're not as unrealistic as his 'author'. He shouldn't even have existed anymore in books 2 and 3!",
"Here lies Cato. Killed by an unrealistic author who didn't realize he and Katniss were the one true pairing.",
"Here lies Gale. He choked himself to death on a rope when he realized that Peeta, that asshole, would put his dick in Katniss' cunt.",
"Here lies Ron Weasley. Killed by brainlessness.",
"Here lies Vincent Crabbe. Killed by a demonic fire.",
"Here lies Dobby the elf. He died totally in vain, and because his author was on a killing spree to get rid of 'unneccessary' characters.",
"Here lies Cedric Diggor-erm, Edward Cullen. Killed by an overdose of sparkle.",
"Here lies Edward Cullen. Killed by an angry mob for being an abusive son-of-a-bitch.",
"Here lies Christian Gray. Anastasia finally started to fight him and whipped him to death with his own belt.",
"Here lies Sirius Black. Cause of death: being too stupid to simply use the Avada Kedavra curse on Bellatrix.",
"Here lies Vernon Dursley. He fell out of his window and broke his neck. Really, Rowling, why the heck would you let him live??? He fucking died! Dammit!",
"Here lies Dolores Umbridge. She was raped and successively killed by Bane the centaur. NO, Rowling, one does NOT return from them alive. Just NO.",
"Here lies Albus Dumbledore. Killed by a cold-blooded murderer named Snape.",
"Here lies Severus Snape. One of many, many people in the Harry Potter series who died just because their author wanted them to die.",
"Here lies the main character of Rowling's 'adult book'. Killed by cutting herself. Seriously, Rowling, why do you write such crap anyway???",
"Here lies Alastor Mad-Eye Moody. He would have lived if it weren't for his sadistic author who wanted to 'shock' the readers by presenting lots of corpses.",
"Here lies Nymphadora Tonks. Well, nobody cared about that pseudo-Mary Sue anyway. I'd much rather read stories about Ebony Dark'ness.",
"Here lies Rita Skeeter. Killed by her own quill which decided it had enough of her using it to write filthy lies.",
"Here lies Draco Malfoy. Killed by Harry Potter with the Sectumsempra curse. Really now, there's no reason why Harry would stop using it prior to killing Draco.",
"Here lies Big Bad Pete. Bludgeoned by Minnie Mouse with a high-heeled lady's shoe.",
"Here lies Donald Duck. He foolishly allowed Daisy to see him with another woman and was too stupid to run away.",
"Live and let live, right..?",
"AAAAAAAAARRRRRRRRRRRRRRGGGGGGGGGGGGGHHHHHHHHHHHHHHH!!!!!!!!!!!!",
"AAAARRRGGGHHH!!!",
"Somehow, I have a bad feeling about this...",
"Strangely, all of a sudden I don't feel so good.",
"You can see armored women on winged horses coming for you.",
"Oh well, you can't always win.",
"I'm too young to die!",
"I'll be back!",
"O, untimely death!",
"Slave, thou hast slain me!",
"Ouch! That smarts!",
"Who knocked?",
"Did anybody get the number of that truck..?",
"Ouch.",
"Et tu, Brute! Then fall, Caesar!",
"O! I die, Horatio...",
"I told you to be careful with that sword...",
"This guy's a little crazy...",
"Ok, ok, I get it: No more pals.",
"No more mr. nice guy!",
"Who turned off the light..?",
"Join the army, see the world, they said...",
"Mom told me there'd be days like this...",
"Rats!",
"Shall this fellow live?",
"Help, ho!",
"What ho! Help!",
"What hast thou done?",
"I'll be revenged on the whole pack of you!",
"You will *pay* for this!",
"They say blood will have blood...",
"Violence is no solution!",
"Yes?",
"#&%#&#%*#*&%!!!!!",
"F***!",
"No time to make a testament?",
"Ugh!",
"Aargh!",
"Aaagghhh!",
"I'm melting!",
"Oof..",
"Oh!",
"Did somebody knock?",
"Later, dude...",
"CU!",
"What? Who? Me? Oh, s..t!",
"...amen!",
"Eeek!",
"Aacch!",
"I hate it when that happens.",
"One direct hit can ruin your whole day.",
"Oh no!",
"Not me!",
"Ouch.",
"Oh no, not again.",
"Another one bites the dust.",
"Goodbye.",
"Help me!",
"Farewell, cruel world.",
"Oh man!",
"Doough!",
"This is the End, my only friend.",
"It's all over.",
"The fat lady sang.",
"Why does everything happen to me?",
"I'm going down.",
"Crapola.",
"Pow!",
"Bif!",
"Bam!",
"Zonk!",
"I should've listened to my mother...",
"No... a Bud light!",
"What was that noise?",
"Mama said there'd be days like this.",
"It's just one of those days...",
"I see a bright light...",
"Mommy? Is that you?",
"I let you hit me!",
"Sucker shot!",
"I didn't want to live anyway.",
"-<sob>-",
"Hah haa! Missed me! Ha---",
"Was that as close as I think it was?",
"Monsters rejoice: the hero has been defeated.",
"It wasn't just a job it was an adventure!",
"I didn't like violence anyway!",
"I thought you liked me?",
"Such senseless violence! I don't understand it.",
"I think this guy's a little crazy.",
"Somehow I don't feel like killing anymore.",
"Help me! I am undone!",
"Hey! Killin' ain't cool.",
"This fell sergeant, Death, is strict in his arrest...",
"The rest is silence.",
"Guh!",
"It's game over, man!",
"You've run out of life.",
"Thou art slain.",
"Finish him!",
"Trust me, I know what I'm doing...",
"Die, mortal!",
"Kill men i' the dark! What be these bloody thieves?",
"Ho! Murder! Murder!",
"O! I am spoil'd, undone by villains!",
"O murderous slave! O villain!",
"O, falsely, falsely murder'd!",
"A guiltless death I die.",
"AAAAAAAAAAAAAAAAAAAAAAAAHHHHHHH!",
"Trust me.",
"Dammit, this thing won't die!",
"He hit me for HOW MUCH?????",
"Look, behind you!!!",
"Who fed steroids to that kobold?",
"Don't worry, be happy!",
"I don't believe this!",
"Oops.",
"Oups.",
"Can't you take a joke?",
"Well, I didn't much like this character, anyway...",
"Oops, sorry... didn't mean to disturb you.",
"I never get to have any fun!",
"Stop!",
"Cut it out!",
"Don't worry. I've got a plan.",
"It didn't look so tough.",
"Run away!",
"All clear, guys.",
"AGAIN!?!?!",
"I don't like this dungeon...",
"Maybe this wasn't such a good idea.",
"My God will protect me.",
"You wouldn't dare!",
"But what about my Parry Skill? Tumbling?",
"Don't worry - I have Pilot-7.",
"And I've *never* done you any harm.",
"I don't understand. It should be dead by now.",
"I'm heir to the crown. They wouldn't dare!",
"Hey! Where's my stomach? My hands?",
"Ha! That's the oldest trick in the book.",
"Cover me.",
"Watch this.",
"And damn'd be him that first cries, 'Hold, enough!'",
"I will not yield.",
"...but like a man he died.",
"If you cut me down, I will only become more powerful.",
"Well, at least I tried...?",
"What could possibly have gone wrong?",
"You die...",
"What's with that weirdo with the teeth?",
"Surrender? Never!",
"I'm sure reinforcements will get here on time. They promised.",
"Funny, didn't *look* like a cyberpsycho....",
"I have a very bad feeling about this.",
"Do something, SCHMUCK!",
"I feel I could cast 'Speak with Dead' and talk to myself.",
"Oh, that's just a light wound.",
"Ach, is doch nur 'ne Fleischwunde...",
"I thought you were on MY side...",
"Next time, try talking!",
"Oh shit... I'll try to teleport again.",
"Somebody get me a Rod of Resurrection... QUICK!",
"Uhh... oh-oh...",
"Gee, where'd everybody go?",
"I see it coming...aaargllhhhh! {sough}",
"What do you mean 'aaargllhhhh'? Hey man, I've paid for this.",
"Ay! Ay! Ay!",
"Ohe! Ohe! Ohe!",
"Et tu, Caesar! Then fall, Brute!",
"Even the best laid plans...",
"Hey, not too rough!",
"The Random Number Generator hates me!",
"So when I die, the first thing I will see in heaven is a score list?",
"Can't we talk this thing over?",
"Wait! Spare me and I'll make you rich! Money is not a problem!",
"I hate you!",
"By the kind gods, 'twas most ignobly done!",
"Mein Leben!",
"Meine Lieder!",
"I'm the hero of this story! I CAN'T die!",
"I thought heroes were supposed to win!",
"Gee... thanks.",
"You've fallen and can't get up!",
"911?",
"Sure don't look good...",
"Oh No! Here I blow again!",
"Hey - I've got lawyers.",
"Thanks, I needed that.",
"I AM toast!!",
"Scheisse!",
"Fatality!",
"Brutality!",
"Toasty!",
"And you thought Tristan was unlucky...",
"Just wait till I get my hands on the crook who sold me this crappy armor...",
"All is lost. Monks, monks, monks!",
"All my possessions for a moment of time!",
"Don't let poor Nelly starve!",
"Wally, what is this? It is death, my boy: they have deceived me.",
"Everyone dances with the Grim Reaper.",
"Adios.",
"I'm going home, babe.",
"I am innocent, innocent, innocent!",
"Watch where you're pointing with that sword! You nearly...",
"Hmm, some things are better wanted than had...",
"And they told me it was not loaded.",
"Of course I know what I am doing.",
"It looked harmless.",
"Hilfe, hilfe, hilfe!",
"Look, dad! No head!",
"Look! I'm flying!",
"Think I'm gonna fall for that?",
"I'll be back... as soon as I can.",
"3... 2... 1... Liftoff!",
"My wallet? In your dreams!",
"Yes! Yes! YES! YES! YY... AAARRRGGGHH!",
"See you later, alligator!",
"Up, up and awaaaayyy!",
"Been nice knowing you.",
"But I just got a little prick!",
"And I just wanted that fancy suit of armour you were carrying...",
"Hey guys, where are you?",
"Hey look... ARCHERS!",
"I can't probably miss...",
"I don't care. I have a Scroll of Raise Dead.",
"I don't care. I have a Ring of Regeneration.",
"I have this dungeon at home, I know where everything is!",
"This HAS to be an illusion. I attempt to disbelieve it.",
"I thought you could be trusted.",
"Never try to sneak in a plate mail.",
"I'll never surrender.",
"I'll use the Cheat Death option...",
"I'm invincible!",
"I'm death incarnate! Nothing can harm me!",
"Hey, it was only a joke, all right?",
"Hey, don't talk to me like that!",
"I have rights, too!",
"Just because you're big and ugly doesn't mean you can push ME around.",
"Me first! Me first!",
"Let me handle this.",
"No problem. That's easy.",
"Oh, shit.",
"So what?",
"Tell me this is an illusion... please!",
"I hate the RNG...",
"They need a twenty to hit me! I'm invincible!",
"Trust me.",
"CHARGE!",
"What do you mean, how many hit points do I have?",
"What do you mean, my GOI expired?",
"Yeah, I knew it was dangerous, but I was thinking about the experience points.",
"You mean you get to use the critical hit chart too?",
"You'd have to be a GOD to smile after that hit!",
"I'm not afraid of death. I just don't want to be there when it happens.",
"I have such sweet thoughts.",
"I pray you all pray for me.",
"I shall hear in heaven.",
"Is not this dying with courage and true greatness?",
"I must sleep now.",
"Nurse, nurse, what murder! What blood! I have done wrong!",
"It is finished.",
"That unworthy hand! That unworthy hand!",
"I am dying.",
"Oh, dear.",
"I will not kneel. Strike!",
"I have led a happy life.",
"Dying, dying.",
"I feel the flowers growing over me.",
"Now it is come.",
"Let me die to the sound of sweet music.",
"I will now enter the Halls of Mandos.",
"Ungrateful traitors!",
"We perish, we disappear, but the march of time goes on forever.",
"Youth, I forgive thee.",
"Treason! Treason!",
"Coward! Why did you not protect me?",
"I am absolutely undone.",
"It is well. I die hard, but am not afraid to go.",
"Do let me die in peace.",
"Nothing is real but pain now.",
"Violent use brings violent plans.",
"Soldier boy, made of clay, now an empty shell.",
"Bodies fill the fields I see, the slaughter never ends.",
"Life planned out before my birth, nothing could I say.",
"Blood will follow blood, dying time is here.",
"Never happy endings on these dark sets.",
"No one to play soldier now, no one to pretend.",
"Time for lust, time for lie, time to kiss your life goodbye.",
"Greetings, Death, he's yours to take away.",
"I was born for dying.",
"The higher you walk, the farther you fall.",
"Where's your crown, King Nothing?",
"Exit: light - enter: night!",
"New blood joins this earth...",
"You labeled me, I'll label you, so I dub thee unforgiven.",
"If you're gonna die, die with your boots on!",
"There's a time to live, and a time to die, when it's time to meet the maker.",
"Isn't it strange, as soon as you're born you're dying?",
"Only the good die young, all the evil seem to live forever.",
"I don't wanna die, I'm a god, why can't I live on?",
"And in my last hour, I'm a slave to the power of death.",
"Now I am cold, but a ghost lives in my veins.",
"You got to watch them - be quick or be dead.",
"Heaven can wait 'till another day.",
"You'll take my life but I'll take yours too.",
"We won't live to fight another day.",
"As I lay forgotten and alone, without fear I draw my parting groan.",
"Somebody please tell me that I'm dreaming!",
"Can't it be there's been some sort of error?",
"Is it really the end not some crazy dream?",
"Life down there is just a strange illusion.",
"Your body tries to leave your soul.",
"I'm so tired of living, I might as well end today.",
"Life, life! Death, death! How curious it is!",
"Catch my soul 'cos it's willing to fly away!",
"Flames? Not yet, I think.",
"Someone call the Gendarmes!",
"I split my brain, melt through the floor.",
"And now the dreams end.",
"Off to Never-Never Land!",
"Death greets me warm, now I will just say goodbye.",
"What is this? I've been stricken by fate!",
"This can't be happening to me!",
"Flash before my eyes: now it's time to die.",
"You have been dying since the day you were born.",
"No point asking who's to blame.",
"But for all his power he couldn't foresee his own demise.",
"My creator will lay my soul to rest.",
"Was that worth dying for?",
"Can you say you are proud of what you've done?",
"But there are some things which cannot be excused.",
"Why is it some of us are here just so that we'll die?",
"The shortest straw, pulled for you.",
"There's got to be just more to it that this or tell me why do we exist?",
"I can't believe that really my time has come.",
"Too much of a good thing, I guess...",
"I really screwed up this time.",
"Wow, what a trip!",
"What is Time, friend or foe",
"Time waits for none",
"Running through your fingers like sand",
"Taking us along to future unknown",
"And all too sudden, like it or not",
"We become part of the Land.",
"Hah! I'm not dead yet.  I still have five hit points.",
"I don't understand.  It should be dead by now.",
"I rolled a 20.  How could that be a miss?",
"What the frell?!",


};


#define LEFT	1
#define H_LEFT	2
#define CENTER	3
#define H_RIGHT	4
#define RIGHT	5

#define TOP	1
#define BOTTOM	5

#define sq(x) ((x)*(x))

#define XLIM	1
#define YLIM	1

#define Fread	(void)dlb_fread
#define Fgetc	(schar)dlb_fgetc
#define New(type)		(type *) alloc(sizeof(type))
#define NewTab(type, size)	(type **) alloc(sizeof(type *) * (unsigned)size)
#define Free(ptr)		if(ptr) free((genericptr_t) (ptr))

static NEARDATA walk walklist[50];
extern int min_rx, max_rx, min_ry, max_ry; /* from mkmap.c */

static char Map[COLNO][ROWNO];
static char robjects[10], rloc_x[10][10], rloc_y[10][10], rmonst[10];
static aligntyp	ralign[3] = { AM_CHAOTIC, AM_NEUTRAL, AM_LAWFUL };
static NEARDATA xchar xstart, ystart;
static NEARDATA char xsize, ysize;
static lev_region rarea[10];

STATIC_DCL void FDECL(set_wall_property, (XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,int));
STATIC_DCL int NDECL(rnddoor);
STATIC_DCL int NDECL(rndtrap);
STATIC_DCL boolean FDECL(get_location, (schar *,schar *,int));
STATIC_DCL void FDECL(sp_lev_shuffle, (char *,char *,int));
STATIC_DCL void FDECL(light_region, (region *));
STATIC_DCL void FDECL(load_common_data, (dlb *,int));
STATIC_DCL void FDECL(load_one_monster, (dlb *,monster *));
STATIC_DCL void FDECL(load_one_object, (dlb *,object *));
STATIC_DCL void FDECL(load_one_engraving, (dlb *,engraving *));
STATIC_DCL boolean FDECL(load_rooms, (dlb *));
STATIC_DCL void FDECL(maze1xy, (coord *,int));
STATIC_DCL boolean FDECL(load_maze, (dlb *));
STATIC_DCL void FDECL(create_door, (room_door *, struct mkroom *));
STATIC_DCL void FDECL(free_rooms,(room **, int));
STATIC_DCL void FDECL(build_room, (room *, room*));

char *lev_message = 0;
lev_region *lregions = 0;
int num_lregions = 0;
lev_init init_lev;

/* Make a random dungeon feature --Amy */
STATIC_OVL void
mkfeature(x,y)
register int x, y;
{

	int whatisit;
	aligntyp al;
	register int tryct = 0;
	register struct obj *otmp;

	if (occupied(x, y)) return;
	if (rn2( !((moves + u.monstertimefinish) % 3357 ) ? 5 : !((moves + u.monstertimefinish) % 357 ) ? 10 : 20)) return;

	whatisit = rnd(200);

	switch (whatisit) {

	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	levl[x][y].typ = FOUNTAIN; level.flags.nfountains++; break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	levl[x][y].typ = CLOUD; break;

	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 38:
	case 39:
	case 40:
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
	case 48:
	case 49:
	case 50:
	levl[x][y].typ = ICE; break;

	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
	case 58:
	case 59:
	case 60:
	case 61:
	case 62:
	case 63:
	case 64:
	case 65:
	case 66:
	case 67:
	case 68:
	case 69:
	case 70:
	levl[x][y].typ = POOL; break;

	case 71:
	case 72:
	case 73:
	case 74:
	case 75:
	case 76:
	case 77:
	case 78:
	case 79:
	case 80:
	levl[x][y].typ = LAVAPOOL; break;

	case 81:
	case 82:
	case 83:
	case 84:
	case 85:
	case 86:
	case 87:
	case 88:
	case 89:
	case 90:
	levl[x][y].typ = IRONBARS; break;

	case 91:
	case 92:
	case 93:
	case 94:
	case 95:
	case 96:
	case 97:
	case 98:
	case 99:
	case 100:
	case 101:
	case 102:
	case 103:
	case 104:
	case 105:
	case 106:
	case 107:
	case 108:
	case 109:
	case 110:
	levl[x][y].typ = TREE; break;

	case 111:
	case 112:
	case 113:
	case 114:
	case 115:
	case 116:
	case 117:
	case 118:
	case 119:
	case 120:
	case 121:
	case 122:
	case 123:
	case 124:
	case 125:
	case 126:
	case 127:
	case 128:
	case 129:
	case 130:
	make_grave(x, y, (char *) 0);
	/* Possibly fill it with objects */
	if (!rn2(3)) (void) mkgold(0L, x, y);
	for (tryct = rn2(5); tryct; tryct--) {
	    otmp = mkobj(RANDOM_CLASS, TRUE);
	    if (!otmp) return;
	    curse(otmp);
	    otmp->ox = x;
	    otmp->oy = y;
	    add_to_buried(otmp);
	}
	break;

	case 131:
	case 132:
	case 133:
	case 134:
	case 135:
	levl[x][y].typ = TOILET; break;

	case 136:
	case 137:
	case 138:
	case 139:
	case 140:
	levl[x][y].typ = SINK; level.flags.nsinks++; break;

	case 141:
	levl[x][y].typ = THRONE; break;

	case 142:
	levl[x][y].typ = ALTAR;
	al = rn2((int)A_LAWFUL+2) - 1;
	if (!rn2(10)) levl[x][y].altarmask = Align2amask( al );
	else levl[x][y].altarmask = Align2amask( A_NONE );
	 break;
	default:
	levl[x][y].typ = STONE; break;
	}

}

/*
 * Make walls of the area (x1, y1, x2, y2) non diggable/non passwall-able
 */

STATIC_OVL void
set_wall_property(x1,y1,x2,y2, prop)
xchar x1, y1, x2, y2;
int prop;
{
	register xchar x, y;

	for(y = y1; y <= y2; y++)
	    for(x = x1; x <= x2; x++)
		if(IS_STWALL(levl[x][y].typ))
		    levl[x][y].wall_info |= prop;
}

/*
 * Choose randomly the state (nodoor, open, closed or locked) for a door
 */
STATIC_OVL int
rnddoor()
{
	int i = 1 << rn2(5);
	i >>= 1;
	return i;
}

/*
 * Select a random trap
 */
STATIC_OVL int
rndtrap()
{
	int rtrap;

	do {
	    rtrap = rnd(TRAPNUM-1);
	    switch (rtrap) {
	     case HOLE:		/* no random holes on special levels */
	     case MAGIC_PORTAL:	rtrap = ROCKTRAP;
				break;
	     case SHAFT_TRAP:
	     case TRAPDOOR:	if (!Can_dig_down(&u.uz) && !Is_stronghold(&u.uz) ) rtrap = ROCKTRAP;
				break;
	     case LEVEL_TELEP:	if (level.flags.noteleport || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) )  rtrap = ANTI_MAGIC;
				break;
	     case TELEP_TRAP:	if (level.flags.noteleport) rtrap = SQKY_BOARD;
				break;
	     case ROLLING_BOULDER_TRAP:
	     case ROCKTRAP:	if (In_endgame(&u.uz)) rtrap = WEB;
				break;

	     case MENU_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8)) rtrap = FIRE_TRAP;
	     case SPEED_TRAP: 
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40)) rtrap = ICE_TRAP;
	     case AUTOMATIC_SWITCHER:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(ishaxor ? (Role_if(PM_GRADUATE) ? 125 : Role_if(PM_SPACEWARS_FIGHTER) ? 250 : Role_if(PM_GEEK) ? 250 : 500) : (Role_if(PM_GRADUATE) ? 250 : Role_if(PM_SPACEWARS_FIGHTER) ? 500 : Role_if(PM_GEEK) ? 500 : 1000) )) rtrap = SHOCK_TRAP;
	     case RMB_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && !Role_if(PM_SPACEWARS_FIGHTER) && rn2(2)) rtrap = ACID_POOL; break;
	     case DISPLAY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3)) rtrap = GLYPH_OF_WARDING; break;
	     case SPELL_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4)) rtrap = SLOW_GAS_TRAP; break;
	     case YELLOW_SPELL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5)) rtrap = POISON_GAS_TRAP; break;
	     case AUTO_DESTRUCT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10)) rtrap = WATER_POOL; break;
	     case MEMORY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20)) rtrap = SCYTHING_BLADE; break;
	     case INVENTORY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50)) rtrap = ANIMATION_TRAP; break;
	     case BLACK_NG_WALL_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100)) rtrap = UNKNOWN_TRAP; break;
	     case SUPERSCROLLER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200)) rtrap = WEB; break;
	     case ACTIVE_SUPERSCROLLER_TRAP: rtrap = RUST_TRAP; break;

	     case ITEM_TELEP_TRAP:
			if (rn2(15)) rtrap = ANIMATION_TRAP; break;
	     case LAVA_TRAP:
			if (rn2(50)) rtrap = PIT; break;
	     case FLOOD_TRAP:
			if (rn2(20)) rtrap = ROCKTRAP; break;
	     case DRAIN_TRAP:
			if (rn2(3)) rtrap = FIRE_TRAP; break;
	     case TIME_TRAP:
			if (rn2(10)) rtrap = FIRE_TRAP; break;
	     case FREE_HAND_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) rtrap = SLP_GAS_TRAP; break;
	     case DISINTEGRATION_TRAP:
			if (rn2(20)) rtrap = ICE_TRAP; break;
	     case UNIDENTIFY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) rtrap = MAGIC_TRAP; break;
	     case THIRST_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) rtrap = ANTI_MAGIC; break;
	     case SHADES_OF_GREY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) rtrap = WEB; break;
	     case LUCK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) rtrap = STATUE_TRAP; break;
	     case DEATH_TRAP:
			if (rn2(100)) rtrap = SHOCK_TRAP; break;
	     case STONE_TO_FLESH_TRAP:
			if (rn2(100)) rtrap = ACID_POOL; break;
	     case FAINT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) rtrap = BOLT_TRAP; break;
	     case CURSE_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 32 : 64 )) rtrap = ARROW_TRAP; break;
	     case DIFFICULTY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 24 )) rtrap = DART_TRAP; break;
	     case SOUND_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) rtrap = SQKY_BOARD; break;
	     case CASTER_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) rtrap = LANDMINE; break;
	     case WEAKNESS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) rtrap = BEAR_TRAP; break;
	     case ROT_THIRTEEN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) rtrap = RUST_TRAP; break;
	     case BISHOP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 28 : 56 )) rtrap = SPIKED_PIT; break;
	     case CONFUSION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) rtrap = SHIT_TRAP; break;
	     case DROP_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) rtrap = Role_if(PM_SPACEWARS_FIGHTER) ? NUPESELL_TRAP : POISON_GAS_TRAP; break;
	     case DSTW_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) rtrap = SLOW_GAS_TRAP; break;
	     case STATUS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) rtrap = SCYTHING_BLADE; break;
	     case ALIGNMENT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 7 )) rtrap = ANTI_MAGIC; break;
	     case STAIRS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 11 )) rtrap = GLYPH_OF_WARDING; break;
	     case UNINFORMATION_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) rtrap = STATUE_TRAP; break;

	     case INTRINSIC_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) rtrap = SQKY_BOARD; break;
	     case BLOOD_LOSS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 18 )) rtrap = FIRE_TRAP; break;
	     case BAD_EFFECT_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) rtrap = UNKNOWN_TRAP; break;
	     case MULTIPLY_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) rtrap = ROLLING_BOULDER_TRAP; break;
	     case AUTO_VULN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) rtrap = SLP_GAS_TRAP; break;
	     case TELE_ITEMS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) rtrap = ROLLING_BOULDER_TRAP; break;
	     case NASTINESS_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 250 : 500 )) rtrap = SPIKED_PIT; break;

	     case FARLOOK_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 34 )) rtrap = THROWING_STAR_TRAP; break;
	     case RESPAWN_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) rtrap = LOCK_TRAP; break;
	     case CAPTCHA_TRAP:
			if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5 )) rtrap = SHIT_PIT; break;

	    case RECURRING_AMNESIA_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) rtrap = LOUDSPEAKER; break;
	    case BIGSCRIPT_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) rtrap = LASER_TRAP; break;
	    case BANK_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) rtrap = FART_TRAP; break;
	    case ONLY_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) rtrap = GLIB_TRAP; break;
	    case MAP_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) rtrap = PLASMA_TRAP; break;
	    case TECH_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) rtrap = NOISE_TRAP; break;
	    case DISENCHANT_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) rtrap = MANA_TRAP; break;
	    case VERISIERT:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) rtrap = UNLIGHT_TRAP; break;
	    case CHAOS_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 33 )) rtrap = ESCALATING_TRAP; break;
	    case MUTENESS_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) rtrap = NEGATIVE_TRAP; break;
	    case NTLL_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) rtrap = LOUDSPEAKER; break;
	    case ENGRAVING_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 17 )) rtrap = BLINDNESS_TRAP; break;
	    case MAGIC_DEVICE_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) rtrap = FART_TRAP; break;
	    case BOOK_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 19 )) rtrap = NUMBNESS_TRAP; break;
	    case LEVEL_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 15 )) rtrap = INERTIA_TRAP; break;
	    case QUIZ_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 11 )) rtrap = LASER_TRAP; break;

	    case METABOLIC_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) rtrap = MAGIC_TRAP; break;
	    case TRAP_OF_NO_RETURN:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) rtrap = ICE_TRAP; break;
	    case EGOTRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) rtrap = SQKY_BOARD; break;
	    case FAST_FORWARD_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) rtrap = PIT; break;
	    case TRAP_OF_ROTTENNESS:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) rtrap = RUST_TRAP; break;
	    case UNSKILLED_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) rtrap = BEAR_TRAP; break;
	    case LOW_STATS_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 13 )) rtrap = LANDMINE; break;
	    case TRAINING_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) rtrap = ROLLING_BOULDER_TRAP; break;
	    case EXERCISE_TRAP:
		if (!Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) rtrap = ANTI_MAGIC; break;

	    case FALLING_LOADSTONE_TRAP:
		if (rn2(3)) rtrap = ROCKTRAP; break;
	    case FALLING_NASTYSTONE_TRAP:
		if (rn2(10)) rtrap = ROCKTRAP; break;

	     case RECURSION_TRAP:
			if (rn2(500)) rtrap = ARROW_TRAP; break;
	     case WARP_ZONE:
			if (rn2(50)) rtrap = BOLT_TRAP; break;
	     case MIND_WIPE_TRAP:
			if (rn2(10)) rtrap = DART_TRAP; break;
	     case GATEWAY_FROM_HELL:
			if (rn2(20)) rtrap = THROWING_STAR_TRAP; break;

	     case PESTILENCE_TRAP:
			if (rn2(20)) rtrap = POISON_GAS_TRAP; break;
	     case FAMINE_TRAP:
			if (rn2(2)) rtrap = SLOW_GAS_TRAP; break;

	    case SIN_TRAP:
		if (rn2(50)) rtrap = STUN_TRAP; break;
	    case DESTROY_ARMOR_TRAP:
		if (rn2(15)) rtrap = RUST_TRAP; break;
	    case DIVINE_ANGER_TRAP:
		if (rn2(40)) rtrap = HALLUCINATION_TRAP; break;
	    case GENETIC_TRAP:
		if (rn2(10)) rtrap = POLY_TRAP; break;
	    case MISSINGNO_TRAP:
		if (rn2(50)) rtrap = POLY_TRAP; break;
	    case CANCELLATION_TRAP:
		if (rn2(25)) rtrap = NUMBNESS_TRAP; break;
	    case HOSTILITY_TRAP:
		if (rn2(4)) rtrap = FREEZING_TRAP; break;
	    case BOSS_TRAP:
		if (rn2(16)) rtrap = BURNING_TRAP; break;
	    case WISHING_TRAP:
		if (rn2(500)) rtrap = BLINDNESS_TRAP; break;
	    case GUILLOTINE_TRAP:
		if (rn2(200)) rtrap = FEAR_TRAP; break;
	    case BISECTION_TRAP:
		if (rn2(200)) rtrap = CONFUSE_TRAP; break;
	    case HORDE_TRAP:
		if (rn2(10)) rtrap = ANIMATION_TRAP; break;
	    case IMMOBILITY_TRAP:
		if (rn2(15)) rtrap = PARALYSIS_TRAP; break;
	    case GREEN_GLYPH:
		if (rn2(5)) rtrap = GLYPH_OF_WARDING; break;
	    case BLUE_GLYPH:
		if (rn2(10)) rtrap = GLYPH_OF_WARDING; break;
	    case YELLOW_GLYPH:
		if (rn2(7)) rtrap = GLYPH_OF_WARDING; break;
	    case ORANGE_GLYPH:
		if (rn2(20)) rtrap = GLYPH_OF_WARDING; break;
	    case BLACK_GLYPH:
		if (rn2(35)) rtrap = GLYPH_OF_WARDING; break;
	    case PURPLE_GLYPH:
		if (rn2(49)) rtrap = GLYPH_OF_WARDING; break;

	    }
	} while (rtrap == NO_TRAP);
	return rtrap;
}

/*
 * Select a random trap (extended) --Amy
 */
int
randomtrap()
{
	int rtrap;

	do {
	    rtrap = rnd(TRAPNUM-1);
		if (rtrap == HOLE && !Is_stronghold(&u.uz) ) rtrap = PIT;
		if (rtrap == MAGIC_PORTAL) rtrap = PIT;
		if (rtrap == TRAPDOOR && !Can_dig_down(&u.uz) && !Is_stronghold(&u.uz) ) rtrap = PIT;
		if (rtrap == SHAFT_TRAP && !Can_dig_down(&u.uz) && !Is_stronghold(&u.uz) ) rtrap = SHIT_PIT;
		if (rtrap == LEVEL_TELEP && (level.flags.noteleport || Is_knox(&u.uz) || Is_blackmarket(&u.uz) || Is_aligned_quest(&u.uz) || In_endgame(&u.uz) || In_sokoban(&u.uz) ) ) rtrap = SQKY_BOARD;
		if (rtrap == TELEP_TRAP && level.flags.noteleport) rtrap = SQKY_BOARD;
		if (rtrap == ROLLING_BOULDER_TRAP) rtrap = ROCKTRAP;
		if (rtrap == NO_TRAP) rtrap = ARROW_TRAP;
		if (rtrap == RMB_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && !Role_if(PM_SPACEWARS_FIGHTER) && rn2(2)) rtrap = ACID_POOL;
		if (rtrap == DISPLAY_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3)) rtrap = GLYPH_OF_WARDING;
		if (rtrap == SPELL_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4)) rtrap = SLOW_GAS_TRAP;
		if (rtrap == YELLOW_SPELL_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5)) rtrap = POISON_GAS_TRAP;

		if (rtrap == MENU_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8)) rtrap = FIRE_TRAP;
		if (rtrap == SPEED_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40)) rtrap = ICE_TRAP;
		if (rtrap == AUTOMATIC_SWITCHER && !Role_if(PM_CAMPERSTRIKER) && rn2(ishaxor ? (Role_if(PM_GRADUATE) ? 125 : Role_if(PM_SPACEWARS_FIGHTER) ? 250 : Role_if(PM_GEEK) ? 250 : 500) : (Role_if(PM_GRADUATE) ? 250 : Role_if(PM_SPACEWARS_FIGHTER) ? 500 : Role_if(PM_GEEK) ? 500 : 1000) )) rtrap = SHOCK_TRAP;

		if (rtrap == AUTO_DESTRUCT_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10)) rtrap = WATER_POOL;
		if (rtrap == MEMORY_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20)) rtrap = SCYTHING_BLADE;
		if (rtrap == INVENTORY_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50)) rtrap = ANIMATION_TRAP;
		if (rtrap == BLACK_NG_WALL_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 50 : 100)) rtrap = UNKNOWN_TRAP;
		if (rtrap == SUPERSCROLLER_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 100 : 200)) rtrap = WEB;
		if (rtrap == ACTIVE_SUPERSCROLLER_TRAP) rtrap = RUST_TRAP;

	      if (rtrap == ITEM_TELEP_TRAP && rn2(15)) rtrap = ANIMATION_TRAP;

	      if (rtrap == LAVA_TRAP && rn2(50)) rtrap = PIT;
	      if (rtrap == FLOOD_TRAP && rn2(20)) rtrap = ROCKTRAP;
	      if (rtrap == DRAIN_TRAP && rn2(3)) rtrap = FIRE_TRAP;
	      if (rtrap == TIME_TRAP && rn2(10)) rtrap = FIRE_TRAP;
	      if (rtrap == FREE_HAND_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) rtrap = SLP_GAS_TRAP;
	      if (rtrap == DISINTEGRATION_TRAP && rn2(20)) rtrap = ICE_TRAP;
	      if (rtrap == UNIDENTIFY_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) rtrap = MAGIC_TRAP;
	      if (rtrap == THIRST_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) rtrap = ANTI_MAGIC;
	      if (rtrap == SHADES_OF_GREY_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) rtrap = WEB;
	      if (rtrap == LUCK_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) rtrap = STATUE_TRAP;
	      if (rtrap == DEATH_TRAP && rn2(100)) rtrap = SHOCK_TRAP;
	      if (rtrap == STONE_TO_FLESH_TRAP && rn2(100)) rtrap = ACID_POOL;
	      if (rtrap == FAINT_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) rtrap = BOLT_TRAP;
	      if (rtrap == CURSE_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 32 : 64 )) rtrap = ARROW_TRAP;
	      if (rtrap == DIFFICULTY_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 12 : 24 )) rtrap = DART_TRAP;
	      if (rtrap == SOUND_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 14 )) rtrap = SQKY_BOARD;
	      if (rtrap == CASTER_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) rtrap = LANDMINE;
	      if (rtrap == WEAKNESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) rtrap = BEAR_TRAP;
	      if (rtrap == ROT_THIRTEEN_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) rtrap = RUST_TRAP;
	      if (rtrap == BISHOP_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 28 : 56 )) rtrap = SPIKED_PIT;
	      if (rtrap == CONFUSION_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 40 : 80 )) rtrap = SHIT_TRAP;
	      if (rtrap == DROP_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 80 )) rtrap = Role_if(PM_SPACEWARS_FIGHTER) ? NUPESELL_TRAP : POISON_GAS_TRAP;
	      if (rtrap == DSTW_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 8 )) rtrap = SLOW_GAS_TRAP;
	      if (rtrap == STATUS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 4 : 9 )) rtrap = SCYTHING_BLADE;
	      if (rtrap == ALIGNMENT_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 7 )) rtrap = ANTI_MAGIC;
	      if (rtrap == STAIRS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 11 )) rtrap = GLYPH_OF_WARDING;
	      if (rtrap == UNINFORMATION_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) rtrap = STATUE_TRAP;

	      if (rtrap == INTRINSIC_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 12 )) rtrap = SQKY_BOARD;
	      if (rtrap == BLOOD_LOSS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 18 )) rtrap = FIRE_TRAP;
	      if (rtrap == BAD_EFFECT_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) rtrap = UNKNOWN_TRAP;
	      if (rtrap == MULTIPLY_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) rtrap = ROLLING_BOULDER_TRAP;
	      if (rtrap == AUTO_VULN_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) rtrap = SLP_GAS_TRAP;
	      if (rtrap == TELE_ITEMS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) rtrap = ROLLING_BOULDER_TRAP;
	      if (rtrap == NASTINESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 250 : 500 )) rtrap = SPIKED_PIT;

	      if (rtrap == FARLOOK_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 34 )) rtrap = THROWING_STAR_TRAP;
	      if (rtrap == RESPAWN_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) rtrap = LOCK_TRAP;
	      if (rtrap == CAPTCHA_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 5 )) rtrap = SHIT_PIT;

	      if (rtrap == RECURRING_AMNESIA_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 35 : 70 )) rtrap = LOUDSPEAKER;
	      if (rtrap == BIGSCRIPT_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 3 )) rtrap = LASER_TRAP;
	      if (rtrap == BANK_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 13 : 26 )) rtrap = FART_TRAP;
	      if (rtrap == ONLY_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) rtrap = GLIB_TRAP;
	      if (rtrap == MAP_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) rtrap = PLASMA_TRAP;
	      if (rtrap == TECH_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) rtrap = NOISE_TRAP;
	      if (rtrap == DISENCHANT_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 30 : 60 )) rtrap = MANA_TRAP;
	      if (rtrap == VERISIERT && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 25 : 50 )) rtrap = UNLIGHT_TRAP;
	      if (rtrap == CHAOS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 17 : 33 )) rtrap = ESCALATING_TRAP;
	      if (rtrap == MUTENESS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 24 : 48 )) rtrap = NEGATIVE_TRAP;
	      if (rtrap == NTLL_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 22 : 44 )) rtrap = LOUDSPEAKER;
	      if (rtrap == ENGRAVING_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 9 : 17 )) rtrap = BLINDNESS_TRAP;
	      if (rtrap == MAGIC_DEVICE_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 14 : 28 )) rtrap = FART_TRAP;
	      if (rtrap == BOOK_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 19 )) rtrap = NUMBNESS_TRAP;
	      if (rtrap == LEVEL_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 15 )) rtrap = INERTIA_TRAP;
	      if (rtrap == QUIZ_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 11 )) rtrap = LASER_TRAP;

		if (rtrap == METABOLIC_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 10 : 20 )) rtrap = MAGIC_TRAP;
		if (rtrap == TRAP_OF_NO_RETURN && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 15 : 30 )) rtrap = ICE_TRAP;
		if (rtrap == EGOTRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 7 : 15 )) rtrap = SQKY_BOARD;
		if (rtrap == FAST_FORWARD_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 20 : 40 )) rtrap = PIT;
		if (rtrap == TRAP_OF_ROTTENNESS && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 8 : 17 )) rtrap = RUST_TRAP;
		if (rtrap == UNSKILLED_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 5 : 10 )) rtrap = BEAR_TRAP;
		if (rtrap == LOW_STATS_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 6 : 13 )) rtrap = LANDMINE;
		if (rtrap == TRAINING_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 3 : 6 )) rtrap = ROLLING_BOULDER_TRAP;
		if (rtrap == EXERCISE_TRAP && !Role_if(PM_CAMPERSTRIKER) && rn2(Role_if(PM_SPACEWARS_FIGHTER) ? 2 : 4 )) rtrap = ANTI_MAGIC;

	      if (rtrap == FALLING_LOADSTONE_TRAP && rn2(3)) rtrap = ROCKTRAP;
	      if (rtrap == FALLING_NASTYSTONE_TRAP && rn2(10)) rtrap = ROCKTRAP;

	      if (rtrap == RECURSION_TRAP && rn2(500)) rtrap = ARROW_TRAP;
	      if (rtrap == WARP_ZONE && rn2(50)) rtrap = BOLT_TRAP;
	      if (rtrap == MIND_WIPE_TRAP && rn2(10)) rtrap = DART_TRAP;
	      if (rtrap == GATEWAY_FROM_HELL && rn2(20)) rtrap = THROWING_STAR_TRAP;

	      if (rtrap == PESTILENCE_TRAP && rn2(20)) rtrap = POISON_GAS_TRAP;
	      if (rtrap == FAMINE_TRAP && rn2(2)) rtrap = SLOW_GAS_TRAP;

	      if (rtrap == SIN_TRAP && rn2(50)) rtrap = STUN_TRAP;
	      if (rtrap == DESTROY_ARMOR_TRAP && rn2(15)) rtrap = RUST_TRAP;
	      if (rtrap == DIVINE_ANGER_TRAP && rn2(40)) rtrap = HALLUCINATION_TRAP;
	      if (rtrap == GENETIC_TRAP && rn2(10)) rtrap = POLY_TRAP;
	      if (rtrap == MISSINGNO_TRAP && rn2(50)) rtrap = POLY_TRAP;
	      if (rtrap == CANCELLATION_TRAP && rn2(25)) rtrap = NUMBNESS_TRAP;
	      if (rtrap == HOSTILITY_TRAP && rn2(4)) rtrap = FREEZING_TRAP;
	      if (rtrap == BOSS_TRAP && rn2(16)) rtrap = BURNING_TRAP;
	      if (rtrap == WISHING_TRAP && rn2(500)) rtrap = BLINDNESS_TRAP;
	      if (rtrap == GUILLOTINE_TRAP && rn2(200)) rtrap = FEAR_TRAP;
	      if (rtrap == BISECTION_TRAP && rn2(200)) rtrap = CONFUSE_TRAP;
	      if (rtrap == HORDE_TRAP && rn2(10)) rtrap = ANIMATION_TRAP;
	      if (rtrap == IMMOBILITY_TRAP && rn2(15)) rtrap = PARALYSIS_TRAP;
	      if (rtrap == GREEN_GLYPH && rn2(5)) rtrap = GLYPH_OF_WARDING;
	      if (rtrap == BLUE_GLYPH && rn2(10)) rtrap = GLYPH_OF_WARDING;
	      if (rtrap == YELLOW_GLYPH && rn2(7)) rtrap = GLYPH_OF_WARDING;
	      if (rtrap == ORANGE_GLYPH && rn2(20)) rtrap = GLYPH_OF_WARDING;
	      if (rtrap == BLACK_GLYPH && rn2(35)) rtrap = GLYPH_OF_WARDING;
	      if (rtrap == PURPLE_GLYPH && rn2(49)) rtrap = GLYPH_OF_WARDING;

	} while (rtrap == NO_TRAP);
	return rtrap;
}

/*
 * Create a trap on some random empty location --Amy
 */

void
makerandomtrap()
{

	int rtrap;
	rtrap = randomtrap();
	int tryct = 0;
	int x, y;

	for (tryct = 0; tryct < 2000; tryct++) {
		x = rn1(COLNO-3,2);
		y = rn2(ROWNO);

		if (x && y && isok(x, y) && (levl[x][y].typ == ROOM || levl[x][y].typ == CORR) && !(t_at(x, y)) ) {
			(void) maketrap(x, y, rtrap);
			break;
			}

	}
}

/*
 * Create either a fart trap or heel trap on some random empty location, for scroll of girliness --Amy
 */

void
makegirlytrap()
{

	int rtrap;
	rtrap = rn2(2) ? FART_TRAP : HEEL_TRAP;
	int tryct = 0;
	int x, y;

	for (tryct = 0; tryct < 2000; tryct++) {
		x = rn1(COLNO-3,2);
		y = rn2(ROWNO);

		if (x && y && isok(x, y) && (levl[x][y].typ == ROOM || levl[x][y].typ == CORR) && !(t_at(x, y)) ) {
			(void) maketrap(x, y, rtrap);
			break;
			}

	}
}

/*
 * Coordinates in special level files are handled specially:
 *
 *	if x or y is -11, we generate a random coordinate.
 *	if x or y is between -1 and -10, we read one from the corresponding
 *	register (x0, x1, ... x9).
 *	if x or y is nonnegative, we convert it from relative to the local map
 *	to global coordinates.
 *	The "humidity" flag is used to insure that engravings aren't
 *	created underwater, or eels on dry land.
 */
#define DRY	0x1
#define WET	0x2
#define MOLTEN	0x4

STATIC_DCL boolean FDECL(is_ok_location, (SCHAR_P, SCHAR_P, int));

STATIC_OVL boolean
get_location(x, y, humidity)
schar *x, *y;
int humidity;
{
	int cpt = 0;

	if (*x >= 0) {			/* normal locations */
		*x += xstart;
		*y += ystart;
	} else if (*x > -11) {		/* special locations */
	    char ry = rloc_y[ - *x - 1][ - *y - 1];
	    char rx = rloc_x[ - *x - 1][ - *y - 1];
	    if (ry == (char)-1 || rx == (char)-1)
		return FALSE;		/* nowhere */
	    else {
		*y = ystart + ry;
		*x = xstart + rx;
	    }
	} else if (*x > -12) {		/* within random region */
	    schar t = - *y - 1;

	    do {
		*x = rn2(rarea[t].inarea.x2 - rarea[t].inarea.x1 + 1) +
		     rarea[t].inarea.x1;
		*y = rn2(rarea[t].inarea.y2 - rarea[t].inarea.y1 + 1) +
		     rarea[t].inarea.y1;
		if (!rarea[t].in_islev) {
		    *x += xstart;
		    *y += ystart;
		}
		if (is_ok_location(*x,*y,humidity)) {
		    int x1 = rarea[t].delarea.x1;
		    int x2 = rarea[t].delarea.x2;
		    int y1 = rarea[t].delarea.y1;
		    int y2 = rarea[t].delarea.y2;
		    if (!rarea[t].del_islev) {
			x1 += xstart;
			x2 += xstart;
			y1 += ystart;
			y2 += ystart;
		    }
		    if (!within_bounded_area(*x,*y,x1,y1,x2,y2)) break;
		}
	    } while (++cpt < 100);
	    if (cpt >= 100) {
		register int xx, yy;
		for (xx = rarea[t].inarea.x1; xx < rarea[t].inarea.x2; xx++)
		    for (yy = rarea[t].inarea.y1; yy < rarea[t].inarea.y2; yy++) {
			*x = xx;
			*y = yy;
			if (!rarea[t].in_islev) {
			    *x += xstart;
			    *y += ystart;
			}
			if (is_ok_location(*x,*y,humidity)) {
			    int x1 = rarea[t].delarea.x1;
			    int x2 = rarea[t].delarea.x2;
			    int y1 = rarea[t].delarea.y1;
			    int y2 = rarea[t].delarea.y2;
			    if (!rarea[t].del_islev) {
				x1 += xstart;
				x2 += xstart;
				y1 += ystart;
				y2 += ystart;
			    }
			    if (!within_bounded_area(*x,*y,x1,y1,x2,y2))
				goto found_it;
			}
		    }
		panic("get_location:  can't find a place!");
	    }
	} else {			/* random location */
	    do {
		*x = xstart + rn2((int)xsize);
		*y = ystart + rn2((int)ysize);
		if (is_ok_location(*x,*y,humidity)) break;
	    } while (++cpt < 100);
	    if (cpt >= 100) {
		register int xx, yy;
		/* last try */
		for (xx = 0; xx < xsize; xx++)
		    for (yy = 0; yy < ysize; yy++) {
			*x = xstart + xx;
			*y = ystart + yy;
			if (is_ok_location(*x,*y,humidity)) goto found_it;
		    }
		panic("get_location:  can't find a place!");
	    }
	}
found_it:;

	if (!isok(*x,*y)) {
	    impossible("get_location:  (%d,%d) out of bounds", *x, *y);
	    *x = x_maze_max; *y = y_maze_max;
	}
	return TRUE;
}

STATIC_OVL boolean
is_ok_location(x, y, humidity)
register schar x, y;
register int humidity;
{
	register int typ;

	if (Is_waterlevel(&u.uz)) return TRUE;	/* accept any spot */

	if (humidity & DRY) {
	    typ = levl[x][y].typ;
	    if (typ == ROOM || typ == AIR ||
		    typ == CLOUD || typ == ICE || typ == CORR)
		return TRUE;
	}
	if (humidity & WET) {
	    if (is_pool(x,y))
		return TRUE;
	}
	if (humidity & MOLTEN) {
	    if (is_lava(x,y))
		return TRUE;
	}
	return FALSE;
}

/*
 * Shuffle the registers for locations, objects or monsters
 */

STATIC_OVL void
sp_lev_shuffle(list1, list2, n)
char list1[], list2[];
int n;
{
	register int i, j;
	register char k;

	for (i = n - 1; i > 0; i--) {
		if ((j = rn2(i + 1)) == i) continue;
		k = list1[j];
		list1[j] = list1[i];
		list1[i] = k;
		if (list2) {
			k = list2[j];
			list2[j] = list2[i];
			list2[i] = k;
		}
	}
}

/*
 * Get a relative position inside a room.
 * negative values for x or y means RANDOM!
 */

STATIC_OVL void
get_room_loc(x,y, croom)
schar		*x, *y;
struct mkroom	*croom;
{
	coord c;

	if (*x <0 && *y <0) {
		if (somexy(croom, &c)) {
			*x = c.x;
			*y = c.y;
		} else
		    panic("get_room_loc : can't find a place!");
	} else {
		if (*x < 0)
		    *x = rn2(croom->hx - croom->lx + 1);
		if (*y < 0)
		    *y = rn2(croom->hy - croom->ly + 1);
		*x += croom->lx;
		*y += croom->ly;
	}
}

/*
 * Get a relative position inside a room.
 * negative values for x or y means RANDOM!
 */

STATIC_OVL void
get_free_room_loc(x,y, croom)
schar		*x, *y;
struct mkroom	*croom;
{
	schar try_x, try_y;
	register int trycnt = 0;

	do {
	    try_x = *x,  try_y = *y;
	    get_room_loc(&try_x, &try_y, croom);
	} while (levl[try_x][try_y].typ != ROOM && ++trycnt <= 100);

	if (trycnt > 100)
	    panic("get_free_room_loc:  can't find a place!");
	*x = try_x,  *y = try_y;
}

boolean
check_room(lowx, ddx, lowy, ddy, vault)
xchar *lowx, *ddx, *lowy, *ddy;
boolean vault;
{
	register int x,y,hix = *lowx + *ddx, hiy = *lowy + *ddy;
	register struct rm *lev;
	int xlim, ylim, ymax;

	xlim = XLIM + (vault ? 1 : 0);
	ylim = YLIM + (vault ? 1 : 0);

	/* Allow rooms to be closer together. For warper race characters this will be the case even more often. --Amy */

	if (!((moves + u.monstertimefinish) % (iswarper ? 7 : 19) )) {

		xlim = xlim - 1;
		ylim = ylim - 1;

	}

	if (!((moves + u.monstertimefinish) % (iswarper ? 13 : 91) )) {

		xlim = xlim - 2;
		ylim = ylim - 2;

	}

	if (!((moves + u.monstertimefinish) % (iswarper ? 19 : 464) )) {

		xlim = xlim - 5;
		ylim = ylim - 5;

	}

	if (!((moves + u.monstertimefinish) % (iswarper ? 29 : 2209) )) {

		xlim = xlim - 10;
		ylim = ylim - 10;

	}

	/* After all, a warped dungeon is what we'd expect from a character called warper... */

	if (*lowx < 3)		*lowx = 3;
	if (*lowy < 2)		*lowy = 2;
	if (hix > COLNO-3)	hix = COLNO-3;
	if (hiy > ROWNO-3)	hiy = ROWNO-3;
chk:
	if (hix <= *lowx || hiy <= *lowy)	return FALSE;

	/* check area around room (and make room smaller if necessary) */
	for (x = *lowx - xlim; x<= hix + xlim; x++) {
		if(x <= 0 || x >= COLNO) continue;
		y = *lowy - ylim;	ymax = hiy + ylim;
		if(y < 0) y = 0;
		if(ymax >= ROWNO) ymax = (ROWNO-1);
		lev = &levl[x][y];
		for (; y <= ymax; y++) {
			if (lev++->typ) {
#ifdef DEBUG
				if(!vault)
				    debugpline("strange area [%d,%d] in check_room.",x,y);
#endif
				if (!rn2(3))	return FALSE;
				if (x < *lowx)
				    *lowx = x + xlim + 1;
				else
				    hix = x - xlim - 1;
				if (y < *lowy)
				    *lowy = y + ylim + 1;
				else
				    hiy = y - ylim - 1;
				goto chk;
			}
		}
	}
	*ddx = hix - *lowx;
	*ddy = hiy - *lowy;
	return TRUE;
}

/*
 * Create a new room.
 * This is still very incomplete...
 */

boolean
create_room(x,y,w,h,xal,yal,rtype,rlit, canbeshaped)
xchar	x,y;
xchar	w,h;
xchar	xal,yal;
xchar	rtype, rlit;
boolean canbeshaped;
{
    /*  
     * numeric args that are -1 mean random  
     * x and y are position within the rectangle grid for the level  
     *        I *think* these values range from 1 to 5.  
     * w and h are size (width and height)  
     * xal and yal are alignment (LEFT/TOP, CENTER, RIGHT/BOTTOM).  
     * footmp is the current working value of foo; this is typically  
     *        equal to foo, unless foo is -1 (random).  
     * xabs and yabs are the actual x and y coordinates where the  
     *        room will be placed on the level map.  
     * */  
	xchar	xabs, yabs;
	int	wtmp, htmp, xaltmp, yaltmp, xtmp, ytmp;
	NhRect	*r1 = 0, r2;
	int	trycnt = 0;
	boolean	vault = FALSE;
	int	xlim = XLIM, ylim = YLIM;

	if (!((moves + u.monstertimefinish) % (iswarper ? 7 : 19) )) {

		xlim = xlim - 1;
		ylim = ylim - 1;

	}

	if (!((moves + u.monstertimefinish) % (iswarper ? 13 : 91) )) {

		xlim = xlim - 2;
		ylim = ylim - 2;

	}

	if (!((moves + u.monstertimefinish) % (iswarper ? 19 : 464) )) {

		xlim = xlim - 5;
		ylim = ylim - 5;

	}

	if (!((moves + u.monstertimefinish) % (iswarper ? 29 : 2209) )) {

		xlim = xlim - 10;
		ylim = ylim - 10;

	}

	if (rtype == -1)	/* Is the type random ? */
	    rtype = OROOM;

	if (rtype == VAULT) {
		vault = TRUE;
		canbeshaped = FALSE;
		xlim++;
		ylim++;
	}

	/* on low levels the room is lit (usually) */
	/* some other rooms may require lighting */

	/* is light state random ? */
	if (rlit == -1)
	    rlit = (rnd(1+abs(depth(&u.uz))) < 11 && rn2(77)) ? TRUE : FALSE;

	/*
	 * Here we will try to create a room. If some parameters are
	 * random we are willing to make several try before we give
	 * it up.
	 */
	do {
		xchar xborder, yborder;
		wtmp = w; htmp = h;
		xtmp = x; ytmp = y;
		xaltmp = xal; yaltmp = yal;

		/* First case : a totaly random room */

		if((xtmp < 0 && ytmp <0 && wtmp < 0 && xaltmp < 0 &&
		   yaltmp < 0) || vault) {
			xchar hx, hy, lx, ly, dx, dy;
			r1 = rnd_rect(); /* Get a random rectangle */

			if (!r1) { /* No more free rectangles ! */
#ifdef DEBUG
				debugpline("No more rects...");
#endif
				return FALSE;
			}
			hx = r1->hx;
			hy = r1->hy;
			lx = r1->lx;
			ly = r1->ly;
			if (vault)
			    dx = dy = 1;
			else {
				dx = 2 + rn2((hx-lx > 28) ? 12 : 8);
				dy = 2 + rn2(4);

				if (!rn2(1000) && trycnt < 200) dx += rnd(80);
				if (!rn2(1000) && trycnt < 200) dy += rnd(24);

				if (!rn2(100) && trycnt < 200) dx += rnd(40);
				if (!rn2(100) && trycnt < 200) dy += rnd(12);

				if (!rn2(10) && trycnt < 200) dx += rnd(20);
				if (!rn2(10) && trycnt < 200) dy += rnd(6);


				/*if(dx*dy > 50)
				    dy = 50/dx;*/
			}

				/*if (wizard && trycnt == 500) pline("try count 500 a");*/

				if ((trycnt > 500) && (dx > 10)) dx = 10;
				if ((trycnt > 1000) && (dx > 6)) dx = 6;
				if ((trycnt > 2000) && (dx > 4)) dx = 4;
				if ((trycnt > 3000) && (dx > 2)) dx = 2;
				if ((trycnt > 500) && (dy > 6)) dy = 6;
				if ((trycnt > 1000) && (dy > 4)) dy = 4;
				if ((trycnt > 2000) && (dy > 3)) dy = 3;
				if ((trycnt > 3000) && (dy > 2)) dy = 2;

			xborder = (lx > 0 && hx < COLNO -1) ? 2*xlim : xlim+1;
			yborder = (ly > 0 && hy < ROWNO -1) ? 2*ylim : ylim+1;
			if(hx-lx < dx + 3 + xborder ||
			   hy-ly < dy + 3 + yborder) {
				r1 = 0;
				continue;
			}
			xabs = lx + (lx > 0 ? xlim : 3)
			    + rn2(hx - (lx>0?lx : 3) - dx - xborder + 1);
			yabs = ly + (ly > 0 ? ylim : 2)
			    + rn2(hy - (ly>0?ly : 2) - dy - yborder + 1);
			if (ly == 0 && hy >= (ROWNO-1) &&
			    (!nroom || !rn2(nroom)) && (yabs+dy > ROWNO/2)) {
			    yabs = rn1(3, 2);
			    if(nroom < 4 && dy>1) dy--;
		        }
			if (!check_room(&xabs, &dx, &yabs, &dy, vault)) {
				r1 = 0;
				continue;
			}
			wtmp = dx+1;
			htmp = dy+1;
			r2.lx = xabs-1; r2.ly = yabs-1;
			r2.hx = xabs + wtmp;
			r2.hy = yabs + htmp;
		} else {	/* Only some parameters are random */
			int rndpos = 0;
			if (xtmp < 0 && ytmp < 0) { /* Position is RANDOM */
				xtmp = rnd(5);
				ytmp = rnd(5);
				rndpos = 1;
			}
			if (wtmp < 0 || htmp < 0) { /* Size is RANDOM */
				wtmp = rn1(15, 3);
				htmp = rn1(8, 2);

				if (!rn2(1000) && trycnt < 200) wtmp += rnd(80);
				if (!rn2(1000) && trycnt < 200) htmp += rnd(24);

				if (!rn2(100) && trycnt < 200) wtmp += rnd(40);
				if (!rn2(100) && trycnt < 200) htmp += rnd(12);

				if (!rn2(10) && trycnt < 200) wtmp += rnd(20);
				if (!rn2(10) && trycnt < 200) htmp += rnd(6);

			}

				/*if (wizard && trycnt == 501) pline("try count 500 b");*/

				if (trycnt > 500 && wtmp > 10) wtmp = 10;
				if (trycnt > 1000 && wtmp > 6) wtmp = 6;
				if (trycnt > 2000 && wtmp > 4) wtmp = 4;
				if (trycnt > 3000 && wtmp > 2) wtmp = 2;
				if (trycnt > 500 && htmp > 6) htmp = 6;
				if (trycnt > 1000 && htmp > 4) htmp = 4;
				if (trycnt > 2000 && htmp > 3) htmp = 3;
				if (trycnt > 3000 && htmp > 2) htmp = 2;

			if (xaltmp == -1) /* Horizontal alignment is RANDOM */
			    xaltmp = rnd(3);
			if (yaltmp == -1) /* Vertical alignment is RANDOM */
			    yaltmp = rnd(3);

			/* Try to generate real (absolute) coordinates here! */

			xabs = (((xtmp-1) * COLNO) / 5) + 1;
			yabs = (((ytmp-1) * ROWNO) / 5) + 1;
			switch (xaltmp) {
			      case LEFT:
				break;
			      case RIGHT:
				xabs += (COLNO / 5) - wtmp;
				break;
			      case CENTER:
				xabs += ((COLNO / 5) - wtmp) / 2;
				break;
			}
			switch (yaltmp) {
			      case TOP:
				break;
			      case BOTTOM:
				yabs += (ROWNO / 5) - htmp;
				break;
			      case CENTER:
				yabs += ((ROWNO / 5) - htmp) / 2;
				break;
			}

			if (xabs + wtmp - 1 > COLNO - 2)
			    xabs = COLNO - wtmp - 3;
			if (xabs < 2)
			    xabs = 2;
			if (yabs + htmp - 1> ROWNO - 2)
			    yabs = ROWNO - htmp - 3;
			if (yabs < 2)
			    yabs = 2;

			/* Try to find a rectangle that fit our room ! */

			r2.lx = xabs-1; r2.ly = yabs-1;
			r2.hx = xabs + wtmp + rndpos;
			r2.hy = yabs + htmp + rndpos;
			r1 = get_rect(&r2);
		}

	} while (++trycnt <= 5000 && !r1);
	if (!r1) {	/* creation of room failed ? */
		return FALSE;
	}
	split_rects(r1, &r2);

	if (!vault) {
		smeq[nroom] = nroom;
		add_room(xabs, yabs, xabs+wtmp-1, yabs+htmp-1,
			 rlit, rtype, FALSE, TRUE);
	} else {
		rooms[nroom].lx = xabs;
		rooms[nroom].ly = yabs;
	}
	return TRUE;
}

/*
 * Create a subroom in room proom at pos x,y with width w & height h.
 * x & y are relative to the parent room.
 */

STATIC_OVL boolean
create_subroom(proom, x, y, w,  h, rtype, rlit)
struct mkroom *proom;
xchar x,y;
xchar w,h;
xchar rtype, rlit;
{
	xchar width, height;

	width = proom->hx - proom->lx + 1;
	height = proom->hy - proom->ly + 1;

	/* There is a minimum size for the parent room */
	if (width < 4 || height < 4)
	    return FALSE;

	/* Check for random position, size, etc... */

	if (w == -1)
	    w = rnd(width - 3);
	if (h == -1)
	    h = rnd(height - 3);
	if (x == -1)
	    x = rnd(width - w - 1) - 1;
	if (y == -1)
	    y = rnd(height - h - 1) - 1;
	if (x == 1)
	    x = 0;
	if (y == 1)
	    y = 0;
	if ((x + w + 1) == width)
	    x++;
	if ((y + h + 1) == height)
	    y++;
	if (rtype == -1)
	    rtype = OROOM;
	if (rlit == -1)
	    rlit = (rnd(1+abs(depth(&u.uz))) < 11 && rn2(77)) ? TRUE : FALSE;
	add_subroom(proom, proom->lx + x, proom->ly + y,
		    proom->lx + x + w - 1, proom->ly + y + h - 1,
		    rlit, rtype, FALSE);
	return TRUE;
}

/*
 * Create a new door in a room.
 * It's placed on a wall (north, south, east or west).
 */

STATIC_OVL void
create_door(dd, broom)
room_door *dd;
struct mkroom *broom;
{
	int	x, y;
	int	trycnt = 0;
	int	i;

	if (dd->secret == -1)
	    dd->secret = rn2(2);

	if (dd->mask == -1) {
		/* is it a locked door, closed, or a doorway? */
		if (!dd->secret) {
			if(!rn2(3)) {
				if(!rn2(5))
				    dd->mask = D_ISOPEN;
				else if(!rn2(6))
				    dd->mask = D_LOCKED;
				else
				    dd->mask = D_CLOSED;
				if (dd->mask != D_ISOPEN && !rn2(25))
				    dd->mask |= D_TRAPPED;
			} else
			    dd->mask = D_NODOOR;
		} else {
			if(!rn2(5))	dd->mask = D_LOCKED;
			else		dd->mask = D_CLOSED;

			if(!rn2(20)) dd->mask |= D_TRAPPED;
		}
	}

	do {
		register int dwall, dpos;

		dwall = dd->wall;
		if (dwall == -1)	/* The wall is RANDOM */
		    dwall = 1 << rn2(4);

		dpos = dd->pos;
		if (dpos == -1)	/* The position is RANDOM */
		    dpos = rn2((dwall == W_WEST || dwall == W_EAST) ?
			    (broom->hy - broom->ly) : (broom->hx - broom->lx));

		/* Convert wall and pos into an absolute coordinate! */

		switch (dwall) {
		      case W_NORTH:
			y = broom->ly - 1;
			x = broom->lx + dpos;
			break;
		      case W_SOUTH:
			y = broom->hy + 1;
			x = broom->lx + dpos;
			break;
		      case W_WEST:
			x = broom->lx - 1;
			y = broom->ly + dpos;
			break;
		      case W_EAST:
			x = broom->hx + 1;
			y = broom->ly + dpos;
			break;
		      default:
			x = y = 0;
			panic("create_door: No wall for door!");
			break;
		}
		if (okdoor(x,y))
		    break;
	} while (++trycnt <= 100);
	if (trycnt > 100) {
		if (wizard) pline("create_door: Can't find a proper place!");
		return;
	}
	i = add_door(x,y,broom);
	doors[i].arti_key = dd->arti_key;
	levl[x][y].typ = (dd->secret ? SDOOR : DOOR);
	levl[x][y].doormask = dd->mask;
}

/*
 * Create a secret door in croom on any one of the specified walls.
 */
void
create_secret_door(croom, walls)
    struct mkroom *croom;
    xchar walls; /* any of W_NORTH | W_SOUTH | W_EAST | W_WEST (or W_ANY) */
{
    xchar sx, sy; /* location of the secret door */
    int count;

    for(count = 0; count < 100; count++) {
	sx = rn1(croom->hx - croom->lx + 1, croom->lx);
	sy = rn1(croom->hy - croom->ly + 1, croom->ly);

	switch(rn2(4)) {
	case 0:  /* top */
	    if(!(walls & W_NORTH)) continue;
	    sy = croom->ly-1; break;
	case 1: /* bottom */
	    if(!(walls & W_SOUTH)) continue;
	    sy = croom->hy+1; break;
	case 2: /* left */
	    if(!(walls & W_EAST)) continue;
	    sx = croom->lx-1; break;
	case 3: /* right */
	    if(!(walls & W_WEST)) continue;
	    sx = croom->hx+1; break;
	}

	if(okdoor(sx,sy)) {
	    levl[sx][sy].typ = SDOOR;
	    levl[sx][sy].doormask = D_CLOSED;
	    add_door(sx,sy,croom);
	    return;
	}
    }

      sy = croom->hy+1;
	sx = croom->hx+1;
    levl[sx][sy].typ = SDOOR;
    levl[sx][sy].doormask = D_CLOSED;
    add_door(sx,sy,croom);
    return;
    /* impossible("couldn't create secret door on any walls 0x%x", walls); */
}

/*
 * Create a trap in a room.
 */

STATIC_OVL void
create_trap(t,croom)
trap	*t;
struct mkroom	*croom;
{
    schar	x,y;
    coord	tm;

    if (rn2(100) < t->chance) {
	x = t->x;
	y = t->y;
	if (croom)
	    get_free_room_loc(&x, &y, croom);
	else
	    if (!get_location(&x, &y, DRY))
		return;

	tm.x = x;
	tm.y = y;

	mktrap(t->type, 1, (struct mkroom*) 0, &tm);
    }
}

/*
 * Create a monster in a room.
 */

STATIC_OVL int
noncoalignment(alignment)
aligntyp alignment;
{
	int k;

	k = rn2(2);
	if (!alignment)
		return(k ? -1 : 1);
	return(k ? -alignment : 0);
}

STATIC_OVL void
create_monster(m,croom)
monster	*m;
struct mkroom	*croom;
{
    struct monst *mtmp;
    schar x, y;
    char class;
    aligntyp amask;
    coord cc;
    struct permonst *pm;
    unsigned g_mvflags;

    if (rn2(100) < m->chance) {

	if (m->class >= 0)
	    class = (char) def_char_to_monclass((char)m->class);
	else if (m->class > -11)
	    class = (char) def_char_to_monclass(rmonst[- m->class - 1]);
	else
	    class = 0;

	if (class == MAXMCLASSES)
	    panic("create_monster: unknown monster class '%c'", m->class);

	amask = (m->align == AM_SPLEV_CO) ?
			Align2amask(u.ualignbase[A_ORIGINAL]) :
		(m->align == AM_SPLEV_NONCO) ?
			Align2amask(noncoalignment(u.ualignbase[A_ORIGINAL])) :
		(m->align <= -11) ? induced_align(80) :
		(m->align < 0 ? ralign[-m->align-1] : m->align);

	if (!class)
	    pm = (struct permonst *) 0;
	else if (m->id != NON_PM) {
#if 0	/* OBSOLETE */
	    if (flags.female && Role_if(PM_ELF) /*&& !u.uelf_drow*/ && m->id == PM_EARENDIL)
		m->id = PM_ELWING;
#endif

	    if (flags.female && Role_if(PM_ELPH) && m->id == PM_EARENDIL) /* elf role re-enabled by Amy */
		m->id = PM_ELWING;

	    /* in the Gnome Mines, make the gnomes & dwarves into            
	       orcs, ogres, & zombies (because gnomes & dwarves are friendly...
	       the mines would be hella easy otherwise) */
		if (In_mines(&u.uz) && (
#ifdef DWARF
			Race_if(PM_DWARF) ||
#endif
			Race_if(PM_GNOME))) {
		switch (m->id) {
		  case PM_GNOME: m->id = PM_GNOME_ZOMBIE; break;
		  case PM_GNOMISH_WIZARD: m->id = PM_ORC_SHAMAN; break;
		  case PM_GNOME_LORD: m->id = PM_GNOME_MUMMY; break;
		  case PM_GNOME_KING: m->id = PM_OGRE; break;
#ifdef DWARF
		  case PM_DWARF: m->id = PM_DWARF_ZOMBIE; break;
		  case PM_DWARF_LORD: m->id = PM_DWARF_MUMMY; break;
		  case PM_DWARF_KING: m->id = PM_WAR_ORC; break;
#endif
		}
	    }
	    pm = &mons[m->id];
	    g_mvflags = (unsigned) mvitals[monsndx(pm)].mvflags;
	    if ((pm->geno & G_UNIQ) && (g_mvflags & G_EXTINCT))
		goto m_done;
	    else if (g_mvflags & G_GONE)	/* genocided or extinct */
		pm = (struct permonst *) 0;	/* make random monster */
	} else {
	    pm = mkclass(class,G_NOGEN|MKC_ULIMIT);
	    /* if we can't get a specific monster type (pm == 0) then all
	       the monsters of acceptable difficulty in the given class
	       have been genocided, so settle for a random monster */
	}
	x = m->x;
	y = m->y;
	if (croom)
	    get_room_loc(&x, &y, croom);
	else {
	    boolean found;
	    if (!pm || !is_swimmer(pm) && !likes_lava(pm))
		found = get_location(&x, &y, DRY);
	    else if (pm->mlet == S_EEL)
		found = get_location(&x, &y, DRY|WET);
	    else if (likes_lava(pm))
		found = get_location(&x, &y, DRY|MOLTEN);
	    else
		found = get_location(&x, &y, DRY|WET);
	    if (!found)
		goto m_done;	/* nowhere */
	}
	/* try to find a close place if someone else is already there */
	if (MON_AT(x,y) && enexto(&cc, x, y, pm))
	    x = cc.x,  y = cc.y;

	if(m->align != -12)
	    mtmp = mk_roamer(pm, Amask2align(amask), x, y, m->peaceful);
	else if(PM_ARCHEOLOGIST <= m->id && m->id <= PM_WIZARD)
	         mtmp = mk_mplayer(pm, x, y, FALSE);
	else mtmp = makemon(pm, x, y, NO_MM_FLAGS);

	if (mtmp) {
	    /* handle specific attributes for some special monsters */
	    if (m->name.str) mtmp = christen_monst(mtmp, m->name.str);

	    /*
	     * This is currently hardwired for mimics only.  It should
	     * eventually be expanded.
	     */
	    if (m->appear_as.str && mtmp->data->mlet == S_MIMIC) {
		int i;

		switch (m->appear) {
		    case M_AP_NOTHING:
			impossible(
		"create_monster: mon has an appearance, \"%s\", but no type",
				m->appear_as.str);
			break;

		    case M_AP_FURNITURE:
			for (i = 0; i < MAXPCHARS; i++)
			    if (!strcmp(defsyms[i].explanation,
					m->appear_as.str))
				break;
			if (i == MAXPCHARS) {
			    impossible(
				"create_monster: can't find feature \"%s\"",
				m->appear_as.str);
			} else {
			    mtmp->m_ap_type = M_AP_FURNITURE;
			    mtmp->mappearance = i;
			}
			break;

		    case M_AP_OBJECT:
			for (i = 0; i < NUM_OBJECTS; i++)
			    if (OBJ_NAME(objects[i]) &&
				!strcmp(OBJ_NAME(objects[i]),m->appear_as.str))
				break;
			if (i == NUM_OBJECTS) {
			    impossible(
				"create_monster: can't find object \"%s\"",
				m->appear_as.str);
			} else {
			    mtmp->m_ap_type = M_AP_OBJECT;
			    mtmp->mappearance = i;
			}
			break;

		    case M_AP_MONSTER:
			/* note: mimics don't appear as monsters! */
			/*	 (but chameleons can :-)	  */
		    default:
			impossible(
		"create_monster: unimplemented mon appear type [%d,\"%s\"]",
				m->appear, m->appear_as.str);
			break;
		}
		if (does_block(x, y, &levl[x][y]))
		    block_point(x, y);
	    }

	    if (m->peaceful >= 0) {
		mtmp->mpeaceful = m->peaceful;
		/* changed mpeaceful again; have to reset malign */
		set_malign(mtmp);
	    }
	    if (m->asleep >= 0) {
#ifdef UNIXPC
		/* optimizer bug strikes again */
		if (m->asleep)
			mtmp->msleeping = 1;
		else
			mtmp->msleeping = 0;
#else
		mtmp->msleeping = m->asleep;
#endif
	    }
	}

    }		/* if (rn2(100) < m->chance) */
 m_done:
    Free(m->name.str);
    Free(m->appear_as.str);
}

/*
 * Create an object in a room.
 */

STATIC_OVL void
create_object(o,croom)
object	*o;
struct mkroom	*croom;
{
    struct obj *otmp;
    schar x, y;
    char c;
    boolean named;	/* has a name been supplied in level description? */

    if (rn2(100) < o->chance) {
	named = o->name.str ? TRUE : FALSE;

	x = o->x; y = o->y;
	if (croom)
	    get_room_loc(&x, &y, croom);
	else
	    if (!get_location(&x, &y, DRY))	/* nowhere */
		goto o_done;

	if (o->class >= 0)
	    c = o->class;
	else if (o->class > -11)
	    c = robjects[ -(o->class+1)];
	else
	    c = 0;

	if (!c)
	    otmp = mkobj_at(RANDOM_CLASS, x, y, !named);
	else if (o->id != -1)
	    otmp = mksobj_at(o->id, x, y, TRUE, !named);
	else {
	    /*
	     * The special levels are compiled with the default "text" object
	     * class characters.  We must convert them to the internal format.
	     */
	    char oclass = (char) def_char_to_objclass(c);

	    if (oclass == MAXOCLASSES)
		panic("create_object:  unexpected object class '%c'",c);

	    /* KMH -- Create piles of gold properly */
	    /* Bruce Cox/WAC - some clean ups */
	    if (oclass == COIN_CLASS && !o->containment) {
		mkgold(0L, x, y);
		otmp = g_at(x,y);
	    } else
		otmp = mkobj_at(oclass, x, y, !named);
	}

	if (o->spe != -127)	/* That means NOT RANDOM! */
	    otmp->spe = (schar)o->spe;

	switch (o->curse_state) {
	      case 1:	bless(otmp); break; /* BLESSED */
	      case 2:	unbless(otmp); uncurse(otmp); break; /* uncursed */
	      case 3:	curse(otmp); break; /* CURSED */
	      default:	break;	/* Otherwise it's random and we're happy
				 * with what mkobj gave us! */
	}

	/*	corpsenm is "empty" if -1, random if -2, otherwise specific */
	if (o->corpsenm != NON_PM) {
	    if (o->corpsenm == NON_PM - 1) otmp->corpsenm = rndmonnum();
	    else otmp->corpsenm = o->corpsenm;
	    otmp->owt = weight(otmp);
	}

	if (otmp->otyp == EGG && In_spiders(&u.uz)) {
	    otmp->corpsenm = PM_GIANT_SPIDER;
	    otmp->age = monstermoves;
	}
	/* assume we wouldn't be given an egg corpsenm unless it was
	   hatchable */
	if (otmp->otyp == EGG && otmp->corpsenm != NON_PM) {
	    if (dead_species(otmp->otyp, TRUE))
		kill_egg(otmp);	/* make sure nothing hatches */
	    else
		attach_egg_hatch_timeout(otmp);	/* attach new hatch timeout */
	}

	if (named)
	    otmp = oname(otmp, o->name.str);

	switch(o->containment) {
	    static struct obj *container = 0;

	    /* contents */
	    case 1:
		if (!container) {
		    impossible("create_object: no container");
		    break;
		}
		remove_object(otmp);
		(void) add_to_container(container, otmp);
		goto o_done;		/* don't stack, but do other cleanup */
	    /* container */
	    case 2:
		delete_contents(otmp);
		container = otmp;
		break;
	    /* nothing */
	    case 0: break;

	    default: impossible("containment type %d?", (int) o->containment);
	}

	/* Medusa level special case: statues are petrified monsters, so they
	 * are not stone-resistant and have monster inventory.  They also lack
	 * other contents, but that can be specified as an empty container.
	 */
	if (o->id == STATUE && Is_medusa_level(&u.uz) &&
		    o->corpsenm == NON_PM) {
	    struct monst *was;
	    struct obj *obj;
	    int wastyp;
	    int i = 0; /* prevent endless loop in case makemon always fails */ 

	    /* Named random statues are of player types, and aren't stone-
	     * resistant (if they were, we'd have to reset the name as well as
	     * setting corpsenm).
	     */
	    for (wastyp = otmp->corpsenm; i < 1000; i++) {
		/* makemon without rndmonst() might create a group */
		was = makemon(&mons[wastyp], 0, 0, NO_MM_FLAGS);
		if (was) {
			if (!resists_ston(was)) break;
			mongone(was);
		}
		wastyp = rndmonnum(); 
	    }
		if (was) {
		otmp->corpsenm = wastyp;
		while(was->minvent) {
		obj = was->minvent;
		obj->owornmask = 0;
		obj_extract_self(obj);
		(void) add_to_container(otmp, obj);
		}
		otmp->owt = weight(otmp);
		mongone(was);
	    }
	}

#ifdef RECORD_ACHIEVE
        /* Nasty hack here: try to determine if this is the Mines or Sokoban
         * "prize" and then set record_achieve_special (maps to corpsenm)
         * for the object.  That field will later be checked to find out if
         * the player obtained the prize. */
        if(otmp->otyp == LUCKSTONE && Is_mineend_level(&u.uz)) {
                otmp->record_achieve_special = 1;
        } else if((otmp->otyp == AMULET_OF_REFLECTION ||
                   otmp->otyp == BAG_OF_HOLDING) && 
                  Is_sokoend_level(&u.uz)) {
                otmp->record_achieve_special = 1;
        }
#endif

	stackobj(otmp);

	if (o->oflags & OBJF_LIT)
	    begin_burn(otmp, FALSE);
	if (o->oflags & OBJF_BURIED)
	    otmp = bury_an_obj(otmp);

    }		/* if (rn2(100) < o->chance) */
 o_done:
    Free(o->name.str);
}

/*
 * Randomly place a specific engraving, then release its memory.
 */
STATIC_OVL void
create_engraving(e, croom)
engraving *e;
struct mkroom *croom;
{
	xchar x, y;
	boolean found = TRUE;

	x = e->x,  y = e->y;
	if (croom)
	    get_room_loc(&x, &y, croom);
	else
	    found = get_location(&x, &y, DRY);

	if (found)
	make_engr_at(x, y, e->engr.str, 0L, e->etype);
	free((genericptr_t) e->engr.str);
}

/*
 * Create stairs in a room.
 *
 */

STATIC_OVL void
create_stairs(s,croom)
stair	*s;
struct mkroom	*croom;
{
	schar		x,y;

	x = s->x; y = s->y;
	get_free_room_loc(&x, &y, croom);
	mkstairs(x,y,(char)s->up, croom);
}

/*
 * Create an altar in a room.
 */

STATIC_OVL void
create_altar(a, croom)
	altar		*a;
	struct mkroom	*croom;
{
	schar		sproom,x,y;
	aligntyp	amask;
	boolean		croom_is_temple = TRUE;
	int oldtyp; 

	x = a->x; y = a->y;

	if (croom) {
	    get_free_room_loc(&x, &y, croom);
	    if (croom->rtype != TEMPLE)
		croom_is_temple = FALSE;
	} else {
	    if (!get_location(&x, &y, DRY))
		return;		/* nowhere */
	    if ((sproom = (schar) *in_rooms(x, y, TEMPLE)) != 0)
		croom = &rooms[sproom - ROOMOFFSET];
	    else
		croom_is_temple = FALSE;
	}

	/* check for existing features */
	oldtyp = levl[x][y].typ;
	if (oldtyp == STAIRS || oldtyp == LADDER)
	    return;

	a->x = x;
	a->y = y;

	/* Is the alignment random ?
	 * If so, it's an 80% chance that the altar will be co-aligned.
	 *
	 * The alignment is encoded as amask values instead of alignment
	 * values to avoid conflicting with the rest of the encoding,
	 * shared by many other parts of the special level code.
	 */

	amask = (a->align == AM_SPLEV_CO) ?
			Align2amask(u.ualignbase[A_ORIGINAL]) :
		(a->align == AM_SPLEV_NONCO) ?
			Align2amask(noncoalignment(u.ualignbase[A_ORIGINAL])) :
		(a->align == -11) ? induced_align(80) :
		(a->align < 0 ? ralign[-a->align-1] : a->align);

	levl[x][y].typ = ALTAR;
	levl[x][y].altarmask = amask;

	if (a->shrine < 0) a->shrine = rn2(2);	/* handle random case */

	if (oldtyp == FOUNTAIN)
	    level.flags.nfountains--;
	else if (oldtyp == SINK)
	    level.flags.nsinks--;

	if (!croom_is_temple || !a->shrine) return;

	if (a->shrine) {	/* Is it a shrine  or sanctum? */
	    priestini(&u.uz, croom, x, y, (a->shrine > 1));
	    levl[x][y].altarmask |= AM_SHRINE;
	    level.flags.has_temple = TRUE;
	}
}

/*
 * Create a gold pile in a room.
 */

STATIC_OVL void
create_gold(g,croom)
gold *g;
struct mkroom	*croom;
{
	schar		x,y;

	x = g->x; y= g->y;
	if (croom)
	    get_room_loc(&x, &y, croom);
	else
	    if (!get_location(&x, &y, DRY))
		return;		/* nowhere */

	if (g->amount == -1)
	    g->amount = rnd(200);
	(void) mkgold((long) g->amount, x, y);
}

/*
 * Create a feature (e.g a fountain) in a room.
 */

STATIC_OVL void
create_feature(fx, fy, croom, typ)
int		fx, fy;
struct mkroom	*croom;
int		typ;
{
	schar		x,y;
	int		trycnt = 0;

	x = fx;  y = fy;
	if (croom) {
	    if (x < 0 && y < 0)
		do {
		    x = -1;  y = -1;
		    get_room_loc(&x, &y, croom);
		} while (++trycnt <= 200 && occupied(x,y));
	    else
		get_room_loc(&x, &y, croom);
	    if(trycnt > 200)
		return;
	} else {
	    if (!get_location(&x, &y, DRY))
		return;		/* nowhere */
	}
	/* Don't cover up an existing feature (particularly randomly
	   placed stairs).  However, if the _same_ feature is already
	   here, it came from the map drawing and we still need to
	   update the special counters. */
	if (IS_FURNITURE(levl[x][y].typ) && levl[x][y].typ != typ)
	    return;

	levl[x][y].typ = typ;
	if (typ == FOUNTAIN)
	    level.flags.nfountains++;
	else if (typ == SINK)
	    level.flags.nsinks++;
/*      else if (typ == TOILET)
	    level.flags.nsinks++;*/
}

/*
 * Search for a door in a room on a specified wall.
 */

STATIC_OVL boolean
search_door(croom,x,y,wall,cnt)
struct mkroom *croom;
xchar *x, *y;
xchar wall;
int cnt;
{
	int dx, dy;
	int xx,yy;

	switch(wall) {
	      case W_NORTH:
		dy = 0; dx = 1;
		xx = croom->lx;
		yy = croom->hy + 1;
		break;
	      case W_SOUTH:
		dy = 0; dx = 1;
		xx = croom->lx;
		yy = croom->ly - 1;
		break;
	      case W_EAST:
		dy = 1; dx = 0;
		xx = croom->hx + 1;
		yy = croom->ly;
		break;
	      case W_WEST:
		dy = 1; dx = 0;
		xx = croom->lx - 1;
		yy = croom->ly;
		break;
	      default:
		dx = dy = xx = yy = 0;
		panic("search_door: Bad wall!");
		break;
	}
	while (xx <= croom->hx+1 && yy <= croom->hy+1) {
		if (IS_DOOR(levl[xx][yy].typ) || levl[xx][yy].typ == SDOOR) {
			*x = xx;
			*y = yy;
			if (cnt-- <= 0)
			    return TRUE;
		}
		xx += dx;
		yy += dy;
	}
	return FALSE;
}

/*
 * Dig a corridor between two points.
 */

boolean
dig_corridor(org,dest,nxcor,ftyp,btyp)
coord *org, *dest;
boolean nxcor;
schar ftyp, btyp;
{
	register int dx=0, dy=0, dix, diy, cct;
	register struct rm *crm;
	register int tx, ty, xx, yy;

	xx = org->x;  yy = org->y;
	tx = dest->x; ty = dest->y;
	if (xx <= 0 || yy <= 0 || tx <= 0 || ty <= 0 ||
	    xx > COLNO-1 || tx > COLNO-1 ||
	    yy > ROWNO-1 || ty > ROWNO-1) {
#ifdef DEBUG
		debugpline("dig_corridor: bad coords : (%d,%d) (%d,%d).",
			   xx,yy,tx,ty);
#endif
		return FALSE;
	}
	if (tx > xx)		dx = 1;
	else if (ty > yy)	dy = 1;
	else if (tx < xx)	dx = -1;
	else			dy = -1;

	xx -= dx;
	yy -= dy;
	cct = 0;
	while(xx != tx || yy != ty) {
	    /* loop: dig corridor at [xx,yy] and find new [xx,yy] */
	    if(cct++ > 500 || (nxcor && !rn2(35)))
		return FALSE;

	    xx += dx;
	    yy += dy;

	    if(xx >= COLNO-1 || xx <= 0 || yy <= 0 || yy >= ROWNO-1)
		return FALSE;		/* impossible */

	    crm = &levl[xx][yy];
	    if(crm->typ == btyp) {
		if(ftyp != CORR || rn2(100)) {
			crm->typ = ftyp;

			if(/*nxcor && */!rn2(ishaxor ? 38 : 75))
				(void) mksobj_at(BOULDER, xx, yy, TRUE, FALSE);
			else if(/*nxcor &&*/ !rn2(ishaxor ? 20 : 40))
				(void) mkobj_at(0, xx, yy, TRUE);
			else if(/*nxcor &&*/ !rn2(ishaxor ? 50 : 100)){ 
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(xx, yy, mesg, 0L, MARK);
			}
			/* else (mktrap(0,1,(struct mkroom *) 0, (coord*) 0) ) ;*/
		    else if(/*nxcor &&*/ !rn2(ishaxor ? 150 : 300)) 
				(void) maketrap(xx, yy, rndtrap());
		    else if(/*nxcor &&*/ !rn2(ishaxor ? 100 : 200)) {
				if (!ishomicider) (void) makemon((struct permonst *)0, xx, yy, NO_MM_FLAGS);
				else makerandomtrap_at(xx, yy);
				}
		    else if(/*nxcor &&*/ !rn2(ishaxor ? 10 : 20)) 
				(void) mkfeature(xx, yy);
		} else {
			crm->typ = SCORR;
		}
	    } else
	    if(crm->typ != ftyp && crm->typ != SCORR) {
		/* strange ... */
		return FALSE;
	    }

	    /* find next corridor position */
	    dix = abs(xx-tx);
	    diy = abs(yy-ty);

	    /* do we have to change direction ? */
	    if(dy && dix > diy) {
		register int ddx = (xx > tx) ? -1 : 1;

		crm = &levl[xx+ddx][yy];
		if(crm->typ == btyp || crm->typ == ftyp || crm->typ == SCORR) {
		    dx = ddx;
		    dy = 0;
		    continue;
		}
	    } else if(dx && diy > dix) {
		register int ddy = (yy > ty) ? -1 : 1;

		crm = &levl[xx][yy+ddy];
		if(crm->typ == btyp || crm->typ == ftyp || crm->typ == SCORR) {
		    dy = ddy;
		    dx = 0;
		    continue;
		}
	    }

	    /* continue straight on? */
	    crm = &levl[xx+dx][yy+dy];
	    if(crm->typ == btyp || crm->typ == ftyp || crm->typ == SCORR)
		continue;

	    /* no, what must we do now?? */
	    if(dx) {
		dx = 0;
		dy = (ty < yy) ? -1 : 1;
	    } else {
		dy = 0;
		dx = (tx < xx) ? -1 : 1;
	    }
	    crm = &levl[xx+dx][yy+dy];
	    if(crm->typ == btyp || crm->typ == ftyp || crm->typ == SCORR)
		continue;
	    dy = -dy;
	    dx = -dx;
	}
	return TRUE;
}

/*
 * Disgusting hack: since special levels have their rooms filled before
 * sorting the rooms, we have to re-arrange the speed values upstairs_room
 * and dnstairs_room after the rooms have been sorted.  On normal levels,
 * stairs don't get created until _after_ sorting takes place.
 */
STATIC_OVL void
fix_stair_rooms()
{
    int i;
    struct mkroom *croom;

    if(xdnstair &&
       !((dnstairs_room->lx <= xdnstair && xdnstair <= dnstairs_room->hx) &&
	 (dnstairs_room->ly <= ydnstair && ydnstair <= dnstairs_room->hy))) {
	for(i=0; i < nroom; i++) {
	    croom = &rooms[i];
	    if((croom->lx <= xdnstair && xdnstair <= croom->hx) &&
	       (croom->ly <= ydnstair && ydnstair <= croom->hy)) {
		dnstairs_room = croom;
		break;
	    }
	}
	if(i == nroom)
	    panic("Couldn't find dnstair room in fix_stair_rooms!");
    }
    if(xupstair &&
       !((upstairs_room->lx <= xupstair && xupstair <= upstairs_room->hx) &&
	 (upstairs_room->ly <= yupstair && yupstair <= upstairs_room->hy))) {
	for(i=0; i < nroom; i++) {
	    croom = &rooms[i];
	    if((croom->lx <= xupstair && xupstair <= croom->hx) &&
	       (croom->ly <= yupstair && yupstair <= croom->hy)) {
		upstairs_room = croom;
		break;
	    }
	}
	if(i == nroom)
	    panic("Couldn't find upstair room in fix_stair_rooms!");
    }
}

/*
 * Corridors always start from a door. But it can end anywhere...
 * Basically we search for door coordinates or for endpoints coordinates
 * (from a distance).
 */

STATIC_OVL void
create_corridor(c)
corridor	*c;
{
	coord org, dest;

	boolean specialcorridor = 0;
	if (!rn2(iswarper ? 50 : 500)) specialcorridor = 1;

	if (!(u.monstertimefinish % 327) && !rn2(iswarper ? 10 : 50)) specialcorridor = 1;

	if (!((moves + u.monstertimefinish) % 5337 )) specialcorridor = 1;

	if (c->src.room == -1) {
		sort_rooms();
		fix_stair_rooms();
		makecorridors();
		return;
	}

	if( !search_door(&rooms[c->src.room], &org.x, &org.y, c->src.wall,
			 c->src.door))
	    return;

	if (c->dest.room != -1) {
		if(!search_door(&rooms[c->dest.room], &dest.x, &dest.y,
				c->dest.wall, c->dest.door))
		    return;
		switch(c->src.wall) {
		      case W_NORTH: org.y--; break;
		      case W_SOUTH: org.y++; break;
		      case W_WEST:  org.x--; break;
		      case W_EAST:  org.x++; break;
		}
		switch(c->dest.wall) {
		      case W_NORTH: dest.y--; break;
		      case W_SOUTH: dest.y++; break;
		      case W_WEST:  dest.x--; break;
		      case W_EAST:  dest.x++; break;
		}
		if (!specialcorridor) {
			(void) dig_corridor(&org, &dest, FALSE, CORR, STONE);
		} else {
			(void) dig_corridor(&org, &dest, FALSE, rn2(2) ? ICE : CLOUD, STONE);
		}
	}
}


/*
 * Fill a room (shop, zoo, etc...) with appropriate stuff.
 */

void
fill_room(croom, prefilled)
struct mkroom *croom;
boolean prefilled;
{

	register int sx,sy,i,eelct=0;
	int rtrap;
	int randomnes = 0;
	schar typ, typ2;
	struct monst *mtmp;

	register int tryct = 0;
	register struct obj *otmp;

	if (croom && croom->rtype == OROOM && !rn2(Role_if(PM_CAMPERSTRIKER) ? 50 : 5000) ) {

		switch (rnd(57)) {

			case 1: croom->rtype = COURT; break;
			case 2: croom->rtype = SWAMP; break;
			case 3: croom->rtype = BEEHIVE; break;
			case 4: croom->rtype = MORGUE; break;
			case 5: croom->rtype = BARRACKS; break;
			case 6: croom->rtype = ZOO; break;
			case 7: croom->rtype = REALZOO; break;
			case 8: croom->rtype = GIANTCOURT; break;
			case 9: croom->rtype = LEPREHALL; break;
			case 10: croom->rtype = DRAGONLAIR; break;
			case 11: croom->rtype = BADFOODSHOP; break;
			case 12: croom->rtype = COCKNEST; break;
			case 13: croom->rtype = ANTHOLE; break;
			case 14: croom->rtype = LEMUREPIT; break;
			case 15: croom->rtype = MIGOHIVE; break;
			case 16: croom->rtype = FUNGUSFARM; break;
			case 17: croom->rtype = CLINIC; break;
			case 18: croom->rtype = TERRORHALL; break;
			case 19: croom->rtype = ELEMHALL; break;
			case 20: croom->rtype = ANGELHALL; break;
			case 21: croom->rtype = MIMICHALL; break;
			case 22: croom->rtype = NYMPHHALL; break;
			case 23: croom->rtype = SPIDERHALL; break;
			case 24: croom->rtype = TROLLHALL; break;
			case 25: croom->rtype = HUMANHALL; break;
			case 26: croom->rtype = GOLEMHALL; break;
			case 27: croom->rtype = COINHALL; break;
			case 28: croom->rtype = DOUGROOM; break;
			case 29: croom->rtype = ARMORY; break;
			case 30: croom->rtype = TENSHALL; break;
			case 31: croom->rtype = TRAPROOM; break;
			case 32: croom->rtype = POOLROOM; break;
			case 33: croom->rtype = STATUEROOM; break;
			case 34: croom->rtype = INSIDEROOM; break;
			case 35: croom->rtype = RIVERROOM; break;
			case 36: croom->rtype = TEMPLE; break;
			case 37: croom->rtype = EMPTYNEST; break;
			case 38: croom->rtype = INSIDEROOM; break;
			case 39: croom->rtype = INSIDEROOM; break;
			case 40: croom->rtype = INSIDEROOM; break;
			case 41: croom->rtype = INSIDEROOM; break;
			case 42: croom->rtype = INSIDEROOM; break;
			case 43: croom->rtype = INSIDEROOM; break;
			case 44: croom->rtype = INSIDEROOM; break;
			case 45: croom->rtype = INSIDEROOM; break;
			case 46: croom->rtype = INSIDEROOM; break;
			case 47: croom->rtype = INSIDEROOM; break;
			case 48: croom->rtype = RIVERROOM; break;
			case 49: croom->rtype = RIVERROOM; break;
			case 50: croom->rtype = RIVERROOM; break;
			case 51: croom->rtype = RIVERROOM; break;
			case 52: croom->rtype = RIVERROOM; break;
			case 53: croom->rtype = RIVERROOM; break;
			case 54: croom->rtype = RIVERROOM; break;
			case 55: croom->rtype = RIVERROOM; break;
			case 56: croom->rtype = RIVERROOM; break;
			case 57: croom->rtype = RIVERROOM; break;

		}

	}

	if (!croom || croom->rtype == OROOM)
	    return;

	if (croom->rtype == RANDOMSHOP) {

		switch (rnd(100)) {	/* random shop - probabilities should match shknam.c --Amy */

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
				croom->rtype = SHOPBASE; break;
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
				croom->rtype = ARMORSHOP; break;
			case 41:
			case 42:
			case 43:
			case 44:
				croom->rtype = SCROLLSHOP; break;
			case 45:
			case 46:
			case 47:
			case 48:
				croom->rtype = POTIONSHOP; break;
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 64:
			case 65:
				croom->rtype = WEAPONSHOP; break;
			case 66:
			case 67:
			case 68:
			case 69:
				croom->rtype = FOODSHOP; break;
			case 70:
			case 71:
			case 72:
				croom->rtype = RINGSHOP; break;
			case 73:
			case 74:
			case 75:
			case 76:
				croom->rtype = WANDSHOP; break;
			case 77:
			case 78:
			case 79:
				croom->rtype = TOOLSHOP; break;
			case 80:
			case 81:
			case 82:
			case 83:
				croom->rtype = PETSHOP; break;
			case 84:
			case 85:
			case 86:
			case 87:
				croom->rtype = TINSHOP; break;
			case 88:
			case 89:
			case 90:
			case 91:
				croom->rtype = BOOKSHOP; break;
			case 92:
				croom->rtype = USELESSSHOP; break;
			case 93:
				croom->rtype = GUNSHOP; break;
			case 94:
				croom->rtype = BANGSHOP; break;
			case 95:
				croom->rtype = AMMOSHOP; break;
			case 96:
				croom->rtype = LITENSHOP; break;
			case 97:
				croom->rtype = BOULDSHOP; break;
			case 98:
				croom->rtype = ROCKSSHOP; break;
			case 99:
				croom->rtype = WIERDSHOP; break;
			case 100:
				croom->rtype = ZORKSHOP; break;

		}
	}

	if (croom->rtype == RANDOMROOM) {

		switch (rnd(37)) {

			case 1: croom->rtype = COURT; break;
			case 2: croom->rtype = SWAMP; break;
			case 3: croom->rtype = BEEHIVE; break;
			case 4: croom->rtype = MORGUE; break;
			case 5: croom->rtype = BARRACKS; break;
			case 6: croom->rtype = ZOO; break;
			case 7: croom->rtype = REALZOO; break;
			case 8: croom->rtype = GIANTCOURT; break;
			case 9: croom->rtype = LEPREHALL; break;
			case 10: croom->rtype = DRAGONLAIR; break;
			case 11: croom->rtype = BADFOODSHOP; break;
			case 12: croom->rtype = COCKNEST; break;
			case 13: croom->rtype = ANTHOLE; break;
			case 14: croom->rtype = LEMUREPIT; break;
			case 15: croom->rtype = MIGOHIVE; break;
			case 16: croom->rtype = FUNGUSFARM; break;
			case 17: croom->rtype = CLINIC; break;
			case 18: croom->rtype = TERRORHALL; break;
			case 19: croom->rtype = ELEMHALL; break;
			case 20: croom->rtype = ANGELHALL; break;
			case 21: croom->rtype = MIMICHALL; break;
			case 22: croom->rtype = NYMPHHALL; break;
			case 23: croom->rtype = SPIDERHALL; break;
			case 24: croom->rtype = TROLLHALL; break;
			case 25: croom->rtype = HUMANHALL; break;
			case 26: croom->rtype = GOLEMHALL; break;
			case 27: croom->rtype = COINHALL; break;
			case 28: croom->rtype = DOUGROOM; break;
			case 29: croom->rtype = ARMORY; break;
			case 30: croom->rtype = TENSHALL; break;
			case 31: croom->rtype = TRAPROOM; break;
			case 32: croom->rtype = POOLROOM; break;
			case 33: croom->rtype = STATUEROOM; break;
			case 34: croom->rtype = INSIDEROOM; break;
			case 35: croom->rtype = RIVERROOM; break;
			case 36: croom->rtype = TEMPLE; break;
			case 37: croom->rtype = EMPTYNEST; break;

		}

	}

	if (!prefilled) {
	    int x,y;

	    /* Shop ? */
	    if (croom->rtype >= SHOPBASE) {
		    stock_room(croom->rtype - SHOPBASE, croom);
		    level.flags.has_shop = TRUE;
		    return;
	    }

	    switch (croom->rtype) {
		case VAULT:
		    for (x=croom->lx;x<=croom->hx;x++)
			for (y=croom->ly;y<=croom->hy;y++)
			    (void) mkgold((long)rn1(abs(depth(&u.uz))*100, 51), x, y);
			if (!rn2(5)) { /* sporkhack code */
				/* it's an aquarium!  :) */
				level.flags.vault_is_aquarium = TRUE;
				create_feature(0,0,croom,POOL);
				create_feature(1,1,croom,POOL);
				if (!rn2(3)) {
					(void)makemon(mkclass(S_EEL,0),croom->lx,croom->ly,NO_MM_FLAGS);
				}
				if (!rn2(3)) {
					(void)makemon(mkclass(S_EEL,0),croom->hx,croom->hy,NO_MM_FLAGS);
				}
			}
			else if (!rn2(100)) { /* evil patch idea by jonadab - cursed vaults filled with L */

				level.flags.vault_is_cursed = TRUE;
				mtmp = makemon(mkclass(S_LICH,0),croom->lx,croom->ly,NO_MM_FLAGS);
				if (mtmp) mtmp->msleeping = 1;
				mtmp = makemon(mkclass(S_LICH,0),croom->hx,croom->hy,NO_MM_FLAGS);
				if (mtmp) mtmp->msleeping = 1;
				if (rn2(2)) {mtmp = makemon(mkclass(S_LICH,0),croom->hx,croom->ly,NO_MM_FLAGS);
				if (mtmp) mtmp->msleeping = 1;
				}
				if (rn2(2)) {mtmp = makemon(mkclass(S_LICH,0),croom->lx,croom->hy,NO_MM_FLAGS);
				if (mtmp) mtmp->msleeping = 1;
				}

			}

		    break;
		case COURT:
		case ZOO:
		case REALZOO:
		case GIANTCOURT:
		case BEEHIVE:
		case BADFOODSHOP:
		case LEMUREPIT:
		case MIGOHIVE:
		case FUNGUSFARM:
		case MORGUE:
		case DRAGONLAIR:
		case ANTHOLE:
		case COCKNEST:
		case EMPTYNEST:
		case LEPREHALL:
		case BARRACKS:
		case HUMANHALL:
		case MIMICHALL:
		case CLINIC:
		case TERRORHALL:
		case ELEMHALL:
		case ANGELHALL:
		case NYMPHHALL:
		case SPIDERHALL:
		case TROLLHALL:
		case GOLEMHALL:
		case COINHALL:
		case ARMORY:
		case TENSHALL:
		/*case TRAPROOM:*/
		/*case POOLROOM:*/
		case GRUEROOM:
		/*case STATUEROOM:*/
		case DOUGROOM:
		    fill_zoo(croom);
		    break;
	    }
	}

	if (croom->rtype == SWAMP) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {
		    if((sx+sy)%2) {
			levl[sx][sy].typ = POOL;
			if(!eelct || !rn2(4)) {
			    /* mkclass() won't do, as we might get kraken */
/* comment by Amy - low-level players shouldn't move close to water anyway, so I will totally spawn everything here! */
			    (void) makemon(rn2(5) ? mkclass(S_EEL,0)
						  : rn2(5) ? &mons[PM_GIANT_EEL]
						  : rn2(2) ? &mons[PM_PIRANHA]
						  : &mons[PM_ELECTRIC_EEL],
						sx, sy, NO_MM_FLAGS);
			    eelct++;
			}
		    } else
			if(!rn2(4))	/* swamps tend to be moldy */
			    (void) makemon(mkclass(S_FUNGUS,0),
						sx, sy, NO_MM_FLAGS);
		}

	}

	if (croom->rtype == TRAPROOM) {

		rtrap = randomtrap();

		if (!rn2(4)) randomnes = 1;

			if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
			if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

			for(sx = croom->lx; sx <= croom->hx; sx++)
			for(sy = croom->ly; sy <= croom->hy; sy++)
			if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {
			    if(rn2(5)) 
					(void) maketrap(sx, sy, rtrap);
				if (randomnes == 1) rtrap = randomtrap();
			}

	}

	if (croom->rtype == POOLROOM) {

	    typ = !rn2(3) ? POOL : !rn2(4) ? ICE : !rn2(5) ? CLOUD : !rn2(8) ? AIR : !rn2(10) ? STONE : !rn2(10) ? TREE : !rn2(15) ? IRONBARS : !rn2(120) ? FOUNTAIN : !rn2(250) ? THRONE : !rn2(60) ? SINK : !rn2(40) ? TOILET : !rn2(20) ? GRAVE : !rn2(500) ? ALTAR : LAVAPOOL;

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {
		    if(rn2(5)) {
			levl[sx][sy].typ = typ;
			if (typ == FOUNTAIN) 	level.flags.nfountains++;
			if (typ == SINK) 	level.flags.nsinks++;

			}

		}

	}

	if (croom->rtype == INSIDEROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {

    typ = !rn2(5) ? POOL : !rn2(5) ? ICE : !rn2(7) ? CLOUD : !rn2(8) ? AIR : !rn2(8) ? STONE : !rn2(8) ? TREE : !rn2(10) ? IRONBARS : !rn2(20) ? FOUNTAIN : !rn2(50) ? THRONE : !rn2(16) ? SINK : !rn2(12) ? TOILET : !rn2(6) ? GRAVE : !rn2(100) ? ALTAR : LAVAPOOL;

	typ2 = !rn2(7) ? TRAP_PERCENTS : !rn2(9) ? UNKNOWN_TRAP : !rn2(15) ? RMB_LOSS_TRAP : !rn2(15) ? UNINFORMATION_TRAP : !rn2(15) ? BIGSCRIPT_TRAP : !rn2(15) ? BAD_EFFECT_TRAP : !rn2(14) ? CAPTCHA_TRAP : !rn2(14) ? WEAKNESS_TRAP : !rn2(14) ? TRAINING_TRAP : !rn2(13) ? ALIGNMENT_TRAP : !rn2(12) ? DISPLAY_TRAP : !rn2(12) ? SPELL_LOSS_TRAP : !rn2(12) ? EXERCISE_TRAP : !rn2(12) ? NUPESELL_TRAP : !rn2(11) ? CASTER_TRAP : !rn2(11) ? QUIZ_TRAP : !rn2(11) ? INTRINSIC_LOSS_TRAP : !rn2(11) ? LOW_STATS_TRAP : !rn2(11) ? YELLOW_SPELL_TRAP : !rn2(11) ? SOUND_TRAP : !rn2(11) ? EGOTRAP : !rn2(10) ? LEVEL_TRAP : !rn2(10) ? MULTIPLY_TRAP : !rn2(10) ? TRAP_OF_ROTTENNESS : !rn2(10) ? DSTW_TRAP : !rn2(10) ? STATUS_TRAP : !rn2(9) ? MENU_TRAP : !rn2(8) ? AUTO_DESTRUCT_TRAP : !rn2(8) ? DIFFICULTY_TRAP : !rn2(8) ? BANK_TRAP : !rn2(8) ? STAIRS_TRAP : !rn2(8) ? FREE_HAND_TRAP : !rn2(8) ? AUTO_VULN_TRAP : !rn2(8) ? UNSKILLED_TRAP : !rn2(8) ? MAGIC_DEVICE_TRAP : !rn2(7) ? ROT_THIRTEEN_TRAP : !rn2(7) ? TRAP_OF_NO_RETURN : !rn2(7) ? DROP_TRAP : !rn2(7) ? ENGRAVING_TRAP : !rn2(7) ? BLOOD_LOSS_TRAP : !rn2(7) ? BOOK_TRAP : !rn2(7) ? MEMORY_TRAP : !rn2(7) ? RESPAWN_TRAP : !rn2(7) ? MAP_TRAP : !rn2(7) ? METABOLIC_TRAP : !rn2(7) ? UNIDENTIFY_TRAP : !rn2(7) ? NTLL_TRAP : !rn2(6) ? INVENTORY_TRAP : !rn2(6) ? THIRST_TRAP : !rn2(6) ? VERISIERT : !rn2(6) ? FAINT_TRAP : !rn2(6) ? MUTENESS_TRAP : !rn2(5) ? CURSE_TRAP : !rn2(5) ? CHAOS_TRAP : !rn2(5) ? FARLOOK_TRAP : !rn2(5) ? SHADES_OF_GREY_TRAP : !rn2(5) ? TELE_ITEMS_TRAP : !rn2(5) ? TECH_TRAP : !rn2(5) ? FAST_FORWARD_TRAP : !rn2(5) ? BISHOP_TRAP : !rn2(4) ? SPEED_TRAP : !rn2(4) ? CONFUSION_TRAP : !rn2(4) ? LUCK_TRAP : !rn2(4) ? ONLY_TRAP : !rn2(4) ? DISENCHANT_TRAP : !rn2(3) ? BLACK_NG_WALL_TRAP : !rn2(2) ? SUPERSCROLLER_TRAP : rn2(50) ? NASTINESS_TRAP : AUTOMATIC_SWITCHER;

		    if(rn2(3)) {
			levl[sx][sy].typ = typ;

			if (typ == FOUNTAIN) 	level.flags.nfountains++;
			if (typ == SINK) 	level.flags.nsinks++;

			if (typ == GRAVE) {

					make_grave(sx, sy, (char *) 0);
					/* Possibly fill it with objects */
					if (!rn2(3)) (void) mkgold(0L, sx, sy);
					for (tryct = rn2(5); tryct; tryct--) {
					    otmp = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmp) return;
					    curse(otmp);
					    otmp->ox = sx;
					    otmp->oy = sy;
					    add_to_buried(otmp);
					}

				}
			}

			/*else*/ if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 5 : 10))	(void) maketrap(sx, sy, typ2);

			if (!rn2(1000)) 	(void) mksobj_at(SWITCHER, sx, sy, TRUE, FALSE);
			if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 25 : 100)) 	(void) mksobj_at(UGH_MEMORY_TO_CREATE_INVENTORY, sx, sy, TRUE, FALSE);

			if (!rn2(Role_if(PM_CAMPERSTRIKER) ? 20 : 40)) 	(void) makemon(insidemon(), sx, sy, MM_ADJACENTOK);
		}

	}

	if (croom->rtype == RIVERROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {

	    typ = !rn2(3) ? POOL : !rn2(10) ? ICE : !rn2(10) ? FOUNTAIN : !rn2(3) ? STONE : !rn2(8) ? TREE : ROOM;

		levl[sx][sy].typ = typ;
		if (typ == FOUNTAIN) 	level.flags.nfountains++;
		if (typ == SINK) 	level.flags.nsinks++;
		if(Role_if(PM_CAMPERSTRIKER) && !rn2(50)) (void) maketrap(sx, sy, randomtrap());
		}

	}

	if (croom->rtype == STATUEROOM) {

		if (croom->ly == 20 && croom->hy == 19) croom->ly = croom->hy = 20;
		if (croom->ly == 1 && croom->hy == 0) croom->ly = croom->hy = 0;

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		if((levl[sx][sy].typ == ROOM || levl[sx][sy].typ == CORR) && !t_at(sx,sy) ) {
		    if(rn2(2)) 
				(void) maketrap(sx, sy, (rn2(10) ? STATUE_TRAP : ANIMATION_TRAP) );
		}

		for(sx = croom->lx; sx <= croom->hx; sx++)
		for(sy = croom->ly; sy <= croom->hy; sy++)
		    if(rn2(2)) 
			{
			    struct obj *sobj = mksobj_at(STATUE, sx, sy, TRUE, FALSE);

			    if (sobj) {
				for (i = rn2(5); i; i--)
				    (void) add_to_container(sobj,
						mkobj(RANDOM_CLASS, FALSE));
				sobj->owt = weight(sobj);
			    }
			}

	}

	switch (croom->rtype) {
	    case VAULT:
		level.flags.has_vault = TRUE;
		break;
	    case ZOO:
	    case DOUGROOM:
		level.flags.has_zoo = TRUE;
		break;
	    case COURT:
		level.flags.has_court = TRUE;
		break;
	    case MORGUE:
		level.flags.has_morgue = TRUE;
		break;
	    case BEEHIVE:
		level.flags.has_beehive = TRUE;
		break;
	    case LEMUREPIT:
		level.flags.has_lemurepit = TRUE;
		break;
	    case MIGOHIVE:
		level.flags.has_migohive = TRUE;
		break;
	    case FUNGUSFARM:
		level.flags.has_fungusfarm = TRUE;
		break;
	    case CLINIC:
		level.flags.has_clinic = TRUE;
		break;
	    case MIMICHALL:
		level.flags.has_mimichall = TRUE;
		break;
	    case ANGELHALL:
		level.flags.has_angelhall = TRUE;
		break;
	    case ELEMHALL:
		level.flags.has_elemhall = TRUE;
		break;
	    case NYMPHHALL:
		level.flags.has_nymphhall = TRUE;
		break;
	    case GOLEMHALL:
		level.flags.has_golemhall = TRUE;
		break;
	    case GRUEROOM:
		level.flags.has_grueroom = TRUE;
		break;
	    case POOLROOM:
		level.flags.has_poolroom = TRUE;
		break;
	    case STATUEROOM:
		level.flags.has_statueroom = TRUE;
		break;
	    case INSIDEROOM:
		level.flags.has_insideroom = TRUE;
		break;
	    case RIVERROOM:
		level.flags.has_riverroom = TRUE;
		break;
	    case TRAPROOM:
		level.flags.has_traproom = TRUE;
		break;
	    case COINHALL:
		level.flags.has_coinhall = TRUE;
		break;
	    case SPIDERHALL:
		level.flags.has_spiderhall = TRUE;
		break;
	    case TROLLHALL:
		level.flags.has_trollhall = TRUE;
		break;
	    case HUMANHALL:
		level.flags.has_humanhall = TRUE;
		break;
	    case TERRORHALL:
		level.flags.has_terrorhall = TRUE;
		break;
	    case TENSHALL:
		level.flags.has_tenshall = TRUE;
		break;
	    case BARRACKS:
		level.flags.has_barracks = TRUE;
		break;
	    case TEMPLE:
		level.flags.has_temple = TRUE;
		break;
	    case SWAMP:
		level.flags.has_swamp = TRUE;
		break;
	}
}

STATIC_OVL void
free_rooms(ro, n)
room **ro;
int n;
{
	short j;
	room *r;

	while(n--) {
		r = ro[n];
		Free(r->name);
		Free(r->parent);
		if ((j = r->ndoor) != 0) {
			while(j--)
			    Free(r->doors[j]);
			Free(r->doors);
		}
		if ((j = r->nstair) != 0) {
			while(j--)
			    Free(r->stairs[j]);
			Free(r->stairs);
		}
		if ((j = r->naltar) != 0) {
			while (j--)
			    Free(r->altars[j]);
			Free(r->altars);
		}
		if ((j = r->nfountain) != 0) {
			while(j--)
			    Free(r->fountains[j]);
			Free(r->fountains);
		}
		if ((j = r->nsink) != 0) {
			while(j--)
			    Free(r->sinks[j]);
			Free(r->sinks);
		}
		if ((j = r->npool) != 0) {
			while(j--)
			    Free(r->pools[j]);
			Free(r->pools);
		}
		if ((j = r->ntrap) != 0) {
			while (j--)
			    Free(r->traps[j]);
			Free(r->traps);
		}
		if ((j = r->nmonster) != 0) {
			while (j--)
				Free(r->monsters[j]);
			Free(r->monsters);
		}
		if ((j = r->nobject) != 0) {
			while (j--)
				Free(r->objects[j]);
			Free(r->objects);
		}
		if ((j = r->ngold) != 0) {
			while(j--)
			    Free(r->golds[j]);
			Free(r->golds);
		}
		if ((j = r->nengraving) != 0) {
			while (j--)
				Free(r->engravings[j]);
			Free(r->engravings);
		}
		Free(r);
	}
	Free(ro);
}

STATIC_OVL void
build_room(r, pr)
room *r, *pr;
{
	boolean okroom;
	struct mkroom	*aroom;
	short i;
	xchar rtype = (!r->chance || rn2(100) < r->chance) ? r->rtype : OROOM;

	if(pr) {
		aroom = &subrooms[nsubroom];
		okroom = create_subroom(pr->mkr, r->x, r->y, r->w, r->h,
					rtype, r->rlit);
	} else {
		aroom = &rooms[nroom];
		okroom = create_room(r->x, r->y, r->w, r->h, r->xalign,
				     r->yalign, rtype, r->rlit, FALSE);
		r->mkr = aroom;
	}

	if (okroom) {
		/* Create subrooms if necessary... */
		for(i=0; i < r->nsubroom; i++)
		    build_room(r->subrooms[i], r);
		/* And now we can fill the room! */

		/* Priority to the stairs */

		for(i=0; i <r->nstair; i++)
		    create_stairs(r->stairs[i], aroom);

		/* Then to the various elements (sinks, etc..) */
		for(i = 0; i<r->nsink; i++)
		    create_feature(r->sinks[i]->x, r->sinks[i]->y, aroom, SINK);
/*              for(i = 0; i<r->ntoilet; i++)
		    create_feature(r->toilets[i]->x, r->toilets[i]->y, aroom, TOILET);*/
		for(i = 0; i<r->npool; i++)
		    create_feature(r->pools[i]->x, r->pools[i]->y, aroom, POOL);
		for(i = 0; i<r->nfountain; i++)
		    create_feature(r->fountains[i]->x, r->fountains[i]->y,
				   aroom, FOUNTAIN);
		for(i = 0; i<r->naltar; i++)
		    create_altar(r->altars[i], aroom);
		for(i = 0; i<r->ndoor; i++)
		    create_door(r->doors[i], aroom);

		/* The traps */
		for(i = 0; i<r->ntrap; i++)
		    create_trap(r->traps[i], aroom);

		/* The monsters */
		for(i = 0; i<r->nmonster; i++)
		    create_monster(r->monsters[i], aroom);

		/* The objects */
		for(i = 0; i<r->nobject; i++)
		    create_object(r->objects[i], aroom);

		/* The gold piles */
		for(i = 0; i<r->ngold; i++)
		    create_gold(r->golds[i], aroom);

		/* The engravings */
		for (i = 0; i < r->nengraving; i++)
		    create_engraving(r->engravings[i], aroom);

#ifdef SPECIALIZATION
		topologize(aroom,FALSE);		/* set roomno */
#else
		topologize(aroom);			/* set roomno */
#endif
		/* MRS - 07/04/91 - This is temporary but should result
		 * in proper filling of shops, etc.
		 * DLC - this can fail if corridors are added to this room
		 * at a later point.  Currently no good way to fix this.
		 */
		if( (aroom->rtype != OROOM || !rn2(Role_if(PM_CAMPERSTRIKER) ? 50 : 5000) ) && r->filled) fill_room(aroom, FALSE);
	}
}

/*
 * set lighting in a region that will not become a room.
 */
STATIC_OVL void
light_region(tmpregion)
    region  *tmpregion;
{
    register boolean litstate = tmpregion->rlit ? 1 : 0;
    register int hiy = tmpregion->y2;
    register int x, y;
    register struct rm *lev;
    int lowy = tmpregion->y1;
    int lowx = tmpregion->x1, hix = tmpregion->x2;

    if(litstate) {
	/* adjust region size for walls, but only if lighted */
	lowx = max(lowx-1,1);
	hix = min(hix+1,COLNO-1);
	lowy = max(lowy-1,0);
	hiy = min(hiy+1, ROWNO-1);
    }
    for(x = lowx; x <= hix; x++) {
	lev = &levl[x][lowy];
	for(y = lowy; y <= hiy; y++) {
	    if (lev->typ != LAVAPOOL) /* this overrides normal lighting */
		lev->lit = litstate;
	    lev++;
	}
    }
}

/* initialization common to all special levels */
STATIC_OVL void
load_common_data(fd, typ)
dlb *fd;
int typ;
{
	uchar	n;
	long	lev_flags;
	int	i;

      {
	aligntyp atmp;
	/* shuffle 3 alignments; can't use sp_lev_shuffle() on aligntyp's */
	i = rn2(3);   atmp=ralign[2]; ralign[2]=ralign[i]; ralign[i]=atmp;
	if (rn2(2)) { atmp=ralign[1]; ralign[1]=ralign[0]; ralign[0]=atmp; }
      }

	level.flags.is_maze_lev = typ == SP_LEV_MAZE;

	/* Read the level initialization data */
	Fread((genericptr_t) &init_lev, 1, sizeof(lev_init), fd);
	if(init_lev.init_present) {
	    if(init_lev.lit < 0)
		init_lev.lit = rn2(2);
	    mkmap(&init_lev);
	}

	/* Read the per level flags */
	Fread((genericptr_t) &lev_flags, 1, sizeof(lev_flags), fd);
	if (lev_flags & NOTELEPORT)
	    level.flags.noteleport = 1;
	if (lev_flags & HARDFLOOR)
	    level.flags.hardfloor = 1;
	if (lev_flags & NOMMAP)
	    level.flags.nommap = 1;
	if (lev_flags & SHORTSIGHTED)
	    level.flags.shortsighted = 1;
	if (lev_flags & ARBOREAL)
	    level.flags.arboreal = 1;
	if (lev_flags & SPOOKY)
	    level.flags.spooky = 1;
	if (lev_flags & LETHE)
	    level.flags.lethe = 1;

	/* Read message */
	Fread((genericptr_t) &n, 1, sizeof(n), fd);
	if (n) {
	    lev_message = (char *) alloc(n + 1);
	    Fread((genericptr_t) lev_message, 1, (int) n, fd);
	    lev_message[n] = 0;
	}
}

STATIC_OVL void
load_one_monster(fd, m)
dlb *fd;
monster *m;
{
	int size;

	Fread((genericptr_t) m, 1, sizeof *m, fd);
	if ((size = m->name.len) != 0) {
	    m->name.str = (char *) alloc((unsigned)size + 1);
	    Fread((genericptr_t) m->name.str, 1, size, fd);
	    m->name.str[size] = '\0';
	} else
	    m->name.str = (char *) 0;
	if ((size = m->appear_as.len) != 0) {
	    m->appear_as.str = (char *) alloc((unsigned)size + 1);
	    Fread((genericptr_t) m->appear_as.str, 1, size, fd);
	    m->appear_as.str[size] = '\0';
	} else
	    m->appear_as.str = (char *) 0;
}

STATIC_OVL void
load_one_object(fd, o)
dlb *fd;
object *o;
{
	int size;

	Fread((genericptr_t) o, 1, sizeof *o, fd);
	if ((size = o->name.len) != 0) {
	    o->name.str = (char *) alloc((unsigned)size + 1);
	    Fread((genericptr_t) o->name.str, 1, size, fd);
	    o->name.str[size] = '\0';
	} else
	    o->name.str = (char *) 0;
}

STATIC_OVL void
load_one_engraving(fd, e)
dlb *fd;
engraving *e;
{
	int size;

	Fread((genericptr_t) e, 1, sizeof *e, fd);
	size = e->engr.len;
	e->engr.str = (char *) alloc((unsigned)size+1);
	Fread((genericptr_t) e->engr.str, 1, size, fd);
	e->engr.str[size] = '\0';
}

STATIC_OVL void
makeriverX(x1,y1,x2,y2,lava,rndom)
int x1,y1,x2,y2;
boolean lava,rndom;
{
    int cx,cy;
    int dx, dy;
    int chance;
    int count = 0;
    int trynmbr = 0;
    int rndomizat = 0;
    const char *str;
    if (rndom) rndomizat = (rn2(5) ? 0 : 1);
    if (rndom) trynmbr = rnd(20);

	register struct obj *otmpX;
	register int tryct = 0;

    cx = x1;
    cy = y1;

    while (count++ < 2000) {
	int rnum = levl[cx][cy].roomno - ROOMOFFSET;
	chance = 0;
	/*if (rnum >= 0 && rooms[rnum].rtype != OROOM) chance = 0;
	else */if (levl[cx][cy].typ == CORR) chance = 15;
	else if (levl[cx][cy].typ == ROOM) chance = 30;
	else if (IS_ROCK(levl[cx][cy].typ)) chance = 100;
	if (rndomizat) trynmbr = (rn2(5) ? rnd(20) : rnd(7));

	if (rn2(100) < chance && !t_at(cx,cy)) {
	    if (lava) {
		if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = POOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
			else levl[cx][cy].typ = LAVAPOOL;
		}
		else {levl[cx][cy].typ = LAVAPOOL;
		levl[cx][cy].lit = 1;
		}
	    } else	if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = LAVAPOOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
			else levl[cx][cy].typ = POOL;
		}
		else 
		levl[cx][cy].typ = !rn2(3) ? POOL : MOAT;

		if (!rn2(ishaxor ? 10000 : 20000))
			levl[cx][cy].typ = THRONE;
		else if (!((moves + u.monstertimefinish) % 857 ) && !rn2(ishaxor ? 1000 : 2000))
			levl[cx][cy].typ = THRONE;
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!((moves + u.monstertimefinish) % 859 ) && !rn2(ishaxor ? 250 : 500)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!((moves + u.monstertimefinish) % 861 ) && !rn2(ishaxor ? 250 : 500)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!rn2(ishaxor ? 5000 : 10000))
			levl[cx][cy].typ = TOILET;
		else if (!((moves + u.monstertimefinish) % 863 ) && !rn2(ishaxor ? 500 : 1000))
			levl[cx][cy].typ = TOILET;
		else if (!rn2(ishaxor ? 1000 : 2000)) {
			levl[cx][cy].typ = GRAVE;
			str = epitaphs[rn2(SIZE(epitaphs))];
			del_engr_at(cx, cy);
			make_engr_at(cx, cy, str, 0L, HEADSTONE);
	
			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
				    otmpX = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmpX) return;
				    curse(otmpX);
				    otmpX->ox = cx;
				    otmpX->oy = cy;
				    add_to_buried(otmpX);
				}
			}
		else if (!((moves + u.monstertimefinish) % 865 ) && !rn2(ishaxor ? 100 : 200)) {
			levl[cx][cy].typ = GRAVE;
			str = epitaphs[rn2(SIZE(epitaphs))];
			del_engr_at(cx, cy);
			make_engr_at(cx, cy, str, 0L, HEADSTONE);

			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
				    otmpX = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmpX) return;
				    curse(otmpX);
				    otmpX->ox = cx;
				    otmpX->oy = cy;
				    add_to_buried(otmpX);
				}
			}
		else if (!rn2(ishaxor ? 10000 : 20000)) {
			levl[cx][cy].typ = ALTAR;
			if (rn2(10)) levl[cx][cy].altarmask = Align2amask( A_NONE );
			else switch (rnd(3)) {
	
			case 1: levl[cx][cy].altarmask = Align2amask( A_LAWFUL ); break;
			case 2: levl[cx][cy].altarmask = Align2amask( A_NEUTRAL ); break;
			case 3: levl[cx][cy].altarmask = Align2amask( A_CHAOTIC ); break;
	
			}
		}
		else if (!((moves + u.monstertimefinish) % 867 ) && !rn2(ishaxor ? 1000 : 2000)) {
			levl[cx][cy].typ = ALTAR;
			if (rn2(10)) levl[cx][cy].altarmask = Align2amask( A_NONE );
			else switch (rnd(3)) {
	
			case 1: levl[cx][cy].altarmask = Align2amask( A_LAWFUL ); break;
			case 2: levl[cx][cy].altarmask = Align2amask( A_NEUTRAL ); break;
			case 3: levl[cx][cy].altarmask = Align2amask( A_CHAOTIC ); break;
	
			}
		}
	}

	if (cx == x2 && cy == y2) break;

	if (cx < x2 && !rn2(3)) dx = 1;
	else if (cx > x2 && !rn2(3)) dx = -1;
	else dx = 0;

	if (cy < y2 && !rn2(3)) dy = 1;
	else if (cy > y2 && !rn2(3)) dy = -1;
	else dy = 0;

	switch (rn2(16)) {
	default: break;
	case 1: dx--; dy--; break;
	case 2: dx++; dy--; break;
	case 3: dx--; dy++; break;
	case 4: dx++; dy++; break;
	case 5: dy--; break;
	case 6: dy++; break;
	case 7: dx--; break;
	case 8: dx++; break;
	}

	if (dx < -1) dx = -1;
	else if (dx > 1) dx = 1;
	if (dy < -1) dy = -1;
	else if (dy > 1) dy = 1;

	cx += dx;
	cy += dy;

	if (cx < 0) cx = 0;
	else if (cx >= COLNO) cx = COLNO-1;
	if (cy < 0) cy = 0;
	else if (cy >= ROWNO) cy = ROWNO-1;

    }
}

STATIC_OVL void
makerandriverX(lava,rndom)
boolean lava,rndom;

{
    int cx,cy;
    int chance;
    int count = 0;
    int ammount = rnz(10 + rnd(40) + rnz(5) + (rn2(5) ? 0 : 50) + (rn2(25) ? 0 : 200) );
    int trynmbr = 0;
    int rndomizat = 0;
    const char *str;
    if (rndom) rndomizat = (rn2(3) ? 0 : 1);
    if (rndom) trynmbr = rnd(12);

	register int tryct = 0;
	register struct obj *otmpX;

    while (count++ < ammount) {

      cx = rn2(COLNO);
      cy = rn2(ROWNO);

	chance = 0;
	if (levl[cx][cy].typ == CORR) chance = 15;
	else if (levl[cx][cy].typ == ROOM) chance = 30;
	else if (IS_ROCK(levl[cx][cy].typ)) chance = 100;
	if (rndomizat) trynmbr = (rn2(5) ? rnd(12) : rnd(7));

	if (rn2(100) < chance && !t_at(cx,cy)) {
	    if (lava) {
		if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = POOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
			else levl[cx][cy].typ = LAVAPOOL;
		}
		else {levl[cx][cy].typ = LAVAPOOL;
		levl[cx][cy].lit = 1;
		}
	    } else	if (rndom) { 

			if (trynmbr == 1) levl[cx][cy].typ = LAVAPOOL;
			else if (trynmbr == 2) levl[cx][cy].typ = TREE;
			else if (trynmbr == 3) levl[cx][cy].typ = IRONBARS;
			else if (trynmbr == 4) levl[cx][cy].typ = ICE;
			else if (trynmbr == 5) levl[cx][cy].typ = CLOUD;
			else if (trynmbr == 6) levl[cx][cy].typ = CORR;
			else levl[cx][cy].typ = POOL;
		}
		else 
		levl[cx][cy].typ = !rn2(3) ? POOL : MOAT;

		if (!rn2(ishaxor ? 10000 : 20000))
			levl[cx][cy].typ = THRONE;
		else if (!((moves + u.monstertimefinish) % 877 ) && !rn2(ishaxor ? 1000 : 2000))
			levl[cx][cy].typ = THRONE;
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!((moves + u.monstertimefinish) % 879 ) && !rn2(ishaxor ? 250 : 500)) {
			levl[cx][cy].typ = FOUNTAIN;
			level.flags.nfountains++;
			}
		else if (!rn2(ishaxor ? 2500 : 5000)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!((moves + u.monstertimefinish) % 881 ) && !rn2(ishaxor ? 250 : 500)) {
			levl[cx][cy].typ = SINK;
			level.flags.nsinks++;
			}
		else if (!rn2(ishaxor ? 5000 : 10000))
			levl[cx][cy].typ = TOILET;
		else if (!((moves + u.monstertimefinish) % 883 ) && !rn2(ishaxor ? 500 : 1000))
			levl[cx][cy].typ = TOILET;
		else if (!rn2(ishaxor ? 1000 : 2000)) {
			levl[cx][cy].typ = GRAVE;
			str = epitaphs[rn2(SIZE(epitaphs))];
			del_engr_at(cx, cy);
			make_engr_at(cx, cy, str, 0L, HEADSTONE);
	
			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
				    otmpX = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmpX) return;
				    curse(otmpX);
				    otmpX->ox = cx;
				    otmpX->oy = cy;
				    add_to_buried(otmpX);
				}
			}
		else if (!((moves + u.monstertimefinish) % 885 ) && !rn2(ishaxor ? 100 : 200)) {
			levl[cx][cy].typ = GRAVE;
			str = epitaphs[rn2(SIZE(epitaphs))];
			del_engr_at(cx, cy);
			make_engr_at(cx, cy, str, 0L, HEADSTONE);
	
			if (!rn2(3)) (void) mkgold(0L, cx, cy);
			for (tryct = rn2(5); tryct; tryct--) {
				    otmpX = mkobj(RANDOM_CLASS, TRUE);
				    if (!otmpX) return;
				    curse(otmpX);
				    otmpX->ox = cx;
				    otmpX->oy = cy;
				    add_to_buried(otmpX);
				}
			}
		else if (!rn2(ishaxor ? 10000 : 20000)) {
			levl[cx][cy].typ = ALTAR;
			if (rn2(10)) levl[cx][cy].altarmask = Align2amask( A_NONE );
			else switch (rnd(3)) {
	
			case 1: levl[cx][cy].altarmask = Align2amask( A_LAWFUL ); break;
			case 2: levl[cx][cy].altarmask = Align2amask( A_NEUTRAL ); break;
			case 3: levl[cx][cy].altarmask = Align2amask( A_CHAOTIC ); break;
	
			}
		}
		else if (!((moves + u.monstertimefinish) % 887 ) && !rn2(ishaxor ? 1000 : 2000)) {
			levl[cx][cy].typ = ALTAR;
			if (rn2(10)) levl[cx][cy].altarmask = Align2amask( A_NONE );
			else switch (rnd(3)) {
	
			case 1: levl[cx][cy].altarmask = Align2amask( A_LAWFUL ); break;
			case 2: levl[cx][cy].altarmask = Align2amask( A_NEUTRAL ); break;
			case 3: levl[cx][cy].altarmask = Align2amask( A_CHAOTIC ); break;
	
			}
		}
	}

	}
}

STATIC_OVL void
mkriversX()
{
    boolean lava;
    boolean rndom;
    int nriv = rn2(3) + 1;
    if (!rn2(10)) nriv += rnd(3);
    if (!rn2(100)) nriv += rnd(5);
    if (!rn2(500)) nriv += rnd(7);
    if (!rn2(2000)) nriv += rnd(10);
    if (!rn2(10000)) nriv += rnd(15);
    while (nriv--) {
      lava = rn2(100) < depth(&u.uz);
      rndom = (rn2(5) ? 0 : 1);
	if (rn2(2)) makeriverX(0, rn2(ROWNO), COLNO-1, rn2(ROWNO), lava, rndom);
	else makeriverX(rn2(COLNO), 0, rn2(COLNO), ROWNO-1, lava, rndom);
    }
}

STATIC_OVL void
mkrandriversX()
{
    boolean lava;
    boolean rndom;
    int nriv = 1;
    if (!rn2(10)) nriv += rnd(2);
    if (!rn2(100)) nriv += rnd(3);
    if (!rn2(500)) nriv += rnd(5);
    if (!rn2(2000)) nriv += rnd(7);
    if (!rn2(10000)) nriv += rnd(10);

    while (nriv--) {
      lava = rn2(100) < depth(&u.uz);
      rndom = (rn2(3) ? 0 : 1);
	if (rn2(2)) makerandriverX(lava, rndom);
	else makerandriverX(lava, rndom);
    }
}

/*
 * Select a random coordinate in the maze.
 *
 * We want a place not 'touched' by the loader.  That is, a place in
 * the maze outside every part of the special level.
 */

STATIC_OVL boolean
load_rooms(fd)
dlb *fd;
{
	xchar		nrooms, ncorr;
	char		n;
	short		size;
	corridor	tmpcor;
	room**		tmproom;
	int		i, j;

	load_common_data(fd, SP_LEV_ROOMS);

	Fread((genericptr_t) &n, 1, sizeof(n), fd); /* nrobjects */
	if (n) {
		Fread((genericptr_t)robjects, sizeof(*robjects), n, fd);
		sp_lev_shuffle(robjects, (char *)0, (int)n);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd); /* nrmonst */
	if (n) {
		Fread((genericptr_t)rmonst, sizeof(*rmonst), n, fd);
		sp_lev_shuffle(rmonst, (char *)0, (int)n);
	}

	Fread((genericptr_t) &nrooms, 1, sizeof(nrooms), fd);
						/* Number of rooms to read */
	tmproom = NewTab(room,nrooms);
	for (i=0;i<nrooms;i++) {
		room *r;

		r = tmproom[i] = New(room);

		/* Let's see if this room has a name */
		Fread((genericptr_t) &size, 1, sizeof(size), fd);
		if (size > 0) {	/* Yup, it does! */
			r->name = (char *) alloc((unsigned)size + 1);
			Fread((genericptr_t) r->name, 1, size, fd);
			r->name[size] = 0;
		} else
		    r->name = (char *) 0;

		/* Let's see if this room has a parent */
		Fread((genericptr_t) &size, 1, sizeof(size), fd);
		if (size > 0) {	/* Yup, it does! */
			r->parent = (char *) alloc((unsigned)size + 1);
			Fread((genericptr_t) r->parent, 1, size, fd);
			r->parent[size] = 0;
		} else
		    r->parent = (char *) 0;

		Fread((genericptr_t) &r->x, 1, sizeof(r->x), fd);
					/* x pos on the grid (1-5) */
		Fread((genericptr_t) &r->y, 1, sizeof(r->y), fd);
					 /* y pos on the grid (1-5) */
		Fread((genericptr_t) &r->w, 1, sizeof(r->w), fd);
					 /* width of the room */
		Fread((genericptr_t) &r->h, 1, sizeof(r->h), fd);
					 /* height of the room */
		Fread((genericptr_t) &r->xalign, 1, sizeof(r->xalign), fd);
					 /* horizontal alignment */
		Fread((genericptr_t) &r->yalign, 1, sizeof(r->yalign), fd);
					 /* vertical alignment */
		Fread((genericptr_t) &r->rtype, 1, sizeof(r->rtype), fd);
					 /* type of room (zoo, shop, etc.) */
		Fread((genericptr_t) &r->chance, 1, sizeof(r->chance), fd);
					 /* chance of room being special. */
		Fread((genericptr_t) &r->rlit, 1, sizeof(r->rlit), fd);
					 /* lit or not ? */
		Fread((genericptr_t) &r->filled, 1, sizeof(r->filled), fd);
					 /* to be filled? */
		r->nsubroom= 0;

		/* read the doors */
		Fread((genericptr_t) &r->ndoor, 1, sizeof(r->ndoor), fd);
		if ((n = r->ndoor) != 0)
		    r->doors = NewTab(room_door, n);
		while(n--) {
			r->doors[(int)n] = New(room_door);
			Fread((genericptr_t) r->doors[(int)n], 1,
				sizeof(room_door), fd);
		}

		/* read the stairs */
		Fread((genericptr_t) &r->nstair, 1, sizeof(r->nstair), fd);
		if ((n = r->nstair) != 0)
		    r->stairs = NewTab(stair, n);
		while (n--) {
			r->stairs[(int)n] = New(stair);
			Fread((genericptr_t) r->stairs[(int)n], 1,
				sizeof(stair), fd);
		}

		/* read the altars */
		Fread((genericptr_t) &r->naltar, 1, sizeof(r->naltar), fd);
		if ((n = r->naltar) != 0)
		    r->altars = NewTab(altar, n);
		while (n--) {
			r->altars[(int)n] = New(altar);
			Fread((genericptr_t) r->altars[(int)n], 1,
				sizeof(altar), fd);
		}

		/* read the fountains */
		Fread((genericptr_t) &r->nfountain, 1,
			sizeof(r->nfountain), fd);
		if ((n = r->nfountain) != 0)
		    r->fountains = NewTab(fountain, n);
		while (n--) {
			r->fountains[(int)n] = New(fountain);
			Fread((genericptr_t) r->fountains[(int)n], 1,
				sizeof(fountain), fd);
		}

		/* read the sinks */
		Fread((genericptr_t) &r->nsink, 1, sizeof(r->nsink), fd);
		if ((n = r->nsink) != 0)
		    r->sinks = NewTab(sink, n);
		while (n--) {
			r->sinks[(int)n] = New(sink);
			Fread((genericptr_t) r->sinks[(int)n], 1, sizeof(sink), fd);
		}

		/* read the pools */
		Fread((genericptr_t) &r->npool, 1, sizeof(r->npool), fd);
		if ((n = r->npool) != 0)
		    r->pools = NewTab(pool,n);
		while (n--) {
			r->pools[(int)n] = New(pool);
			Fread((genericptr_t) r->pools[(int)n], 1, sizeof(pool), fd);
		}

		/* read the traps */
		Fread((genericptr_t) &r->ntrap, 1, sizeof(r->ntrap), fd);
		if ((n = r->ntrap) != 0)
		    r->traps = NewTab(trap, n);
		while(n--) {
			r->traps[(int)n] = New(trap);
			Fread((genericptr_t) r->traps[(int)n], 1, sizeof(trap), fd);
		}

		/* read the monsters */
		Fread((genericptr_t) &r->nmonster, 1, sizeof(r->nmonster), fd);
		if ((n = r->nmonster) != 0) {
		    r->monsters = NewTab(monster, n);
		    while(n--) {
			r->monsters[(int)n] = New(monster);
			load_one_monster(fd, r->monsters[(int)n]);
		    }
		} else
		    r->monsters = 0;

		/* read the objects, in same order as mazes */
		Fread((genericptr_t) &r->nobject, 1, sizeof(r->nobject), fd);
		if ((n = r->nobject) != 0) {
		    r->objects = NewTab(object, n);
		    for (j = 0; j < n; ++j) {
			r->objects[j] = New(object);
			load_one_object(fd, r->objects[j]);
		    }
		} else
		    r->objects = 0;

		/* read the gold piles */
		Fread((genericptr_t) &r->ngold, 1, sizeof(r->ngold), fd);
		if ((n = r->ngold) != 0)
		    r->golds = NewTab(gold, n);
		while (n--) {
			r->golds[(int)n] = New(gold);
			Fread((genericptr_t) r->golds[(int)n], 1, sizeof(gold), fd);
		}

		/* read the engravings */
		Fread((genericptr_t) &r->nengraving, 1,
			sizeof(r->nengraving), fd);
		if ((n = r->nengraving) != 0) {
		    r->engravings = NewTab(engraving,n);
		    while (n--) {
			r->engravings[(int)n] = New(engraving);
			load_one_engraving(fd, r->engravings[(int)n]);
		    }
		} else
		    r->engravings = 0;

	}

	/* Now that we have loaded all the rooms, search the
	 * subrooms and create the links.
	 */

	for (i = 0; i<nrooms; i++)
	    if (tmproom[i]->parent) {
		    /* Search the parent room */
		    for(j=0; j<nrooms; j++)
			if (tmproom[j]->name && !strcmp(tmproom[j]->name,
						       tmproom[i]->parent)) {
				n = tmproom[j]->nsubroom++;
				tmproom[j]->subrooms[(int)n] = tmproom[i];
				break;
			}
	    }

	/*
	 * Create the rooms now...
	 */

	for (i=0; i < nrooms; i++)
	    if(!tmproom[i]->parent)
		build_room(tmproom[i], (room *) 0);

	free_rooms(tmproom, nrooms);

	/* read the corridors */

	Fread((genericptr_t) &ncorr, sizeof(ncorr), 1, fd);
	for (i=0; i<ncorr; i++) {
		Fread((genericptr_t) &tmpcor, 1, sizeof(tmpcor), fd);
		create_corridor(&tmpcor);
	}

	/* make rivers if possible --Amy */
	if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversX();
	if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversX();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversX();
		if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversX();
	}

	if (isaquarian && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversX();

	if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();
	if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();
		if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();
	}

	if (isaquarian && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();

	return TRUE;
}


STATIC_OVL void
maze1xy(m, humidity)
coord *m;
int humidity;
{
	register int x, y, tryct = 2000;
	/* tryct:  normally it won't take more than ten or so tries due
	   to the circumstances under which we'll be called, but the
	   `humidity' screening might drastically change the chances */

	do {
	    x = rn1(x_maze_max - 3, 3);
	    y = rn1(y_maze_max - 3, 3);
	    if (--tryct < 0) break;	/* give up */
	} while (!(x % 2) || !(y % 2) || Map[x][y] ||
		 !is_ok_location((schar)x, (schar)y, humidity));

	m->x = (xchar)x,  m->y = (xchar)y;
}

/*
 * The Big Thing: special maze loader
 *
 * Could be cleaner, but it works.
 */

STATIC_OVL boolean
load_maze(fd)
dlb *fd;
{
    xchar   x, y, typ;
    boolean prefilled, room_not_needed;

    char    n, numpart = 0;
    xchar   nwalk = 0, nwalk_sav;
    schar   filling;
    char    halign, valign;

    int     xi, dir, size;
    coord   mm;
    int     mapcount, mapcountmax, mapfact;

    lev_region  tmplregion;
    region  tmpregion;
    door    tmpdoor;
    trap    tmptrap;
    monster tmpmons;
    object  tmpobj;
    drawbridge tmpdb;
    walk    tmpwalk;
    digpos  tmpdig;
    lad     tmplad;
    stair   tmpstair, prevstair;
    altar   tmpaltar;
    gold    tmpgold;
    fountain tmpfountain;
    engraving tmpengraving;
    xchar   mustfill[(MAXNROFROOMS+1)*2];
    struct trap *badtrap;
    boolean has_bounds;

    int specialcorridor;
    if (!rn2(iswarper ? 50 : 500)) specialcorridor = rnd(2);
    if (!(u.monstertimefinish % 347) && !rn2(iswarper ? 10 : 50)) specialcorridor = rnd(2);
    if (!((moves + u.monstertimefinish) % 4257 )) specialcorridor = rnd(2);

    (void) memset((genericptr_t)&Map[0][0], 0, sizeof Map);
    load_common_data(fd, SP_LEV_MAZE);

    /* Initialize map */
    Fread((genericptr_t) &filling, 1, sizeof(filling), fd);
    if (!init_lev.init_present) { /* don't init if mkmap() has been called */
      for(x = 2; x <= x_maze_max; x++)
	for(y = 0; y <= y_maze_max; y++)
	    if (filling == -1) {
#ifndef WALLIFIED_MAZE
		    levl[x][y].typ = STONE;
#else
		    levl[x][y].typ =
			(y < 2 || ((x % 2) && (y % 2))) ? STONE : HWALL;
#endif
	    } else {
		    levl[x][y].typ = filling;
	    }
    }

    /* Start reading the file */
    Fread((genericptr_t) &numpart, 1, sizeof(numpart), fd);
						/* Number of parts */
    if (!numpart || numpart > 9)
	panic("load_maze error: numpart = %d", (int) numpart);

    while (numpart--) {
	Fread((genericptr_t) &halign, 1, sizeof(halign), fd);
					/* Horizontal alignment */
	Fread((genericptr_t) &valign, 1, sizeof(valign), fd);
					/* Vertical alignment */
	Fread((genericptr_t) &xsize, 1, sizeof(xsize), fd);
					/* size in X */
	Fread((genericptr_t) &ysize, 1, sizeof(ysize), fd);
					/* size in Y */
	switch((int) halign) {
	    case LEFT:	    xstart = 3;					break;
	    case H_LEFT:    xstart = 2+((x_maze_max-2-xsize)/4);	break;
	    case CENTER:    xstart = 2+((x_maze_max-2-xsize)/2);	break;
	    case H_RIGHT:   xstart = 2+((x_maze_max-2-xsize)*3/4);	break;
	    case RIGHT:     xstart = x_maze_max-xsize-1;		break;
	}
	switch((int) valign) {
	    case TOP:	    ystart = 3;					break;
	    case CENTER:    ystart = 2+((y_maze_max-2-ysize)/2);	break;
	    case BOTTOM:    ystart = y_maze_max-ysize-1;		break;
	}
	if (!(xstart % 2)) xstart++;
	if (!(ystart % 2)) ystart++;
	if ((ystart < 0) || (ystart + ysize > ROWNO)) {
	    /* try to move the start a bit */
	    ystart += (ystart > 0) ? -2 : 2;
	    if(ysize == ROWNO) ystart = 0;
	    if(ystart < 0 || ystart + ysize > ROWNO)
		panic("reading special level with ysize too large");
	}

	/*
	 * If any CROSSWALLs are found, must change to ROOM after REGION's
	 * are laid out.  CROSSWALLS are used to specify "invisible"
	 * boundaries where DOOR syms look bad or aren't desirable.
	 */
	has_bounds = FALSE;

	if(init_lev.init_present && xsize <= 1 && ysize <= 1) {
	    xstart = 1;
	    ystart = 0;
	    xsize = COLNO-1;
	    ysize = ROWNO;
	} else {
	    /* Load the map */
	    for(y = ystart; y < ystart+ysize; y++)
		for(x = xstart; x < xstart+xsize; x++) {
		    levl[x][y].typ = Fgetc(fd);
		    levl[x][y].lit = FALSE;
		    /* clear out levl: load_common_data may set them */
		    levl[x][y].flags = 0;
		    levl[x][y].horizontal = 0;
		    levl[x][y].roomno = 0;
		    levl[x][y].edge = 0;
		    /*
		     * Note: Even though levl[x][y].typ is type schar,
		     *	 lev_comp.y saves it as type char. Since schar != char
		     *	 all the time we must make this exception or hack
		     *	 through lev_comp.y to fix.
		     */

		    /*
		     *  Set secret doors to closed (why not trapped too?).  Set
		     *  the horizontal bit.
		     */
		    if (levl[x][y].typ == SDOOR || IS_DOOR(levl[x][y].typ)) {
			if(levl[x][y].typ == SDOOR)
			    levl[x][y].doormask = D_CLOSED;
			/*
			 *  If there is a wall to the left that connects to a
			 *  (secret) door, then it is horizontal.  This does
			 *  not allow (secret) doors to be corners of rooms.
			 */
			if (x != xstart && (IS_WALL(levl[x-1][y].typ) ||
					    levl[x-1][y].horizontal))
			    levl[x][y].horizontal = 1;
		    } else if(levl[x][y].typ == HWALL ||
				levl[x][y].typ == IRONBARS)
			levl[x][y].horizontal = 1;
		    else if(levl[x][y].typ == LAVAPOOL)
			levl[x][y].lit = 1;
		    else if(levl[x][y].typ == CROSSWALL)
			has_bounds = TRUE;
		    Map[x][y] = 1;
		}
	    if (init_lev.init_present && init_lev.joined)
		remove_rooms(xstart, ystart, xstart+xsize, ystart+ysize);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of level regions */
	if(n) {
	    if(num_lregions) {
		/* realloc the lregion space to add the new ones */
		/* don't really free it up until the whole level is done */
		lev_region *newl = (lev_region *) alloc(sizeof(lev_region) *
						(unsigned)(n+num_lregions));
		(void) memcpy((genericptr_t)(newl+n), (genericptr_t)lregions,
					sizeof(lev_region) * num_lregions);
		Free(lregions);
		num_lregions += n;
		lregions = newl;
	    } else {
		num_lregions = n;
		lregions = (lev_region *)
				alloc(sizeof(lev_region) * (unsigned)n);
	    }
	}

	while(n--) {
	    boolean found = TRUE;
	    Fread((genericptr_t) &tmplregion, sizeof(tmplregion), 1, fd);
	    if ((size = tmplregion.rname.len) != 0) {
		tmplregion.rname.str = (char *) alloc((unsigned)size + 1);
		Fread((genericptr_t) tmplregion.rname.str, size, 1, fd);
		tmplregion.rname.str[size] = '\0';
	    } else
		tmplregion.rname.str = (char *) 0;
	    if(!tmplregion.in_islev) {
		found &= get_location(&tmplregion.inarea.x1,
				      &tmplregion.inarea.y1, DRY|WET);
		found &= get_location(&tmplregion.inarea.x2,
				      &tmplregion.inarea.y2, DRY|WET);
	    }
	    if(!tmplregion.del_islev) {
		found &= get_location(&tmplregion.delarea.x1,
				      &tmplregion.delarea.y1, DRY|WET);
		found &= get_location(&tmplregion.delarea.x2,
				      &tmplregion.delarea.y2, DRY|WET);
	    }
	    if (!found)
		panic("reading special level with region located nowhere");
	    lregions[(int)n] = tmplregion;
	}

	/* random level region registers */
	Fread((genericptr_t) &n, 1, sizeof(n), fd);	
	if (n) {
	    int tmpn = n;
	    while(n--) {
		boolean found = TRUE;
		Fread((genericptr_t) &tmplregion, sizeof(tmplregion), 1, fd);
		if ((size = tmplregion.rname.len) != 0) {
		    tmplregion.rname.str = (char *) alloc((unsigned)size + 1);
		    Fread((genericptr_t) tmplregion.rname.str, size, 1, fd);
		    tmplregion.rname.str[size] = '\0';
		} else
		    tmplregion.rname.str = (char *) 0;
		if (!found)
		    panic("reading special level with random region located nowhere");
		(void) memcpy((genericptr_t)&rarea[(int)tmpn - n - 1],
			(genericptr_t)&tmplregion, sizeof(lev_region));
	    }
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Random objects */
	if(n) {
		Fread((genericptr_t)robjects, sizeof(*robjects), (int) n, fd);
		sp_lev_shuffle(robjects, (char *)0, (int)n);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Random locations */
	if(n) {
		char nloc[10];
		Fread((genericptr_t)nloc, sizeof(*nloc), (int) n, fd);
		for(xi = 0; xi < n; xi++) {
		    Fread((genericptr_t)rloc_x[xi], sizeof(*rloc_x[xi]),
			    (int) nloc[xi], fd);
		    Fread((genericptr_t)rloc_y[xi], sizeof(*rloc_y[xi]),
			    (int) nloc[xi], fd);
		    sp_lev_shuffle(rloc_x[xi], rloc_y[xi], (int)nloc[xi]);
		}
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Random monsters */
	if(n) {
		Fread((genericptr_t)rmonst, sizeof(*rmonst), (int) n, fd);
		sp_lev_shuffle(rmonst, (char *)0, (int)n);
	}

	(void) memset((genericptr_t)mustfill, 0, sizeof(mustfill));
	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of subrooms */
	while(n--) {
		register struct mkroom *troom;

		Fread((genericptr_t)&tmpregion, 1, sizeof(tmpregion), fd);

		if(tmpregion.rtype > MAXRTYPE) {
		    tmpregion.rtype -= MAXRTYPE+1;
		    prefilled = TRUE;
		} else
		    prefilled = FALSE;

		if(tmpregion.rlit < 0)
		    tmpregion.rlit = (rnd(1+abs(depth(&u.uz))) < 11 && rn2(77))
			? TRUE : FALSE;

		if (!get_location(&tmpregion.x1, &tmpregion.y1, DRY|WET) ||
		  !get_location(&tmpregion.x2, &tmpregion.y2, DRY|WET))
		    panic("reading special level with region located nowhere");

		/* for an ordinary room, `prefilled' is a flag to force
		   an actual room to be created (such rooms are used to
		   control placement of migrating monster arrivals) */
		room_not_needed = (tmpregion.rtype == OROOM &&
				   !tmpregion.rirreg && !prefilled);
		if (room_not_needed || nroom >= MAXNROFROOMS) {
		    if (!room_not_needed)
			impossible("Too many rooms on new level!");
		    light_region(&tmpregion);
		    continue;
		}

		troom = &rooms[nroom];

		/* mark rooms that must be filled, but do it later */
		if (tmpregion.rtype != OROOM || !rn2(Role_if(PM_CAMPERSTRIKER) ? 50 : 5000) )
		    mustfill[nroom] = (prefilled ? 2 : 1);

		if(tmpregion.rirreg) {
		    min_rx = max_rx = tmpregion.x1;
		    min_ry = max_ry = tmpregion.y1;
		    flood_fill_rm(tmpregion.x1, tmpregion.y1,
				  nroom+ROOMOFFSET, tmpregion.rlit, TRUE);
		    add_room(min_rx, min_ry, max_rx, max_ry,
			     FALSE, tmpregion.rtype, TRUE, FALSE);
		    troom->rlit = tmpregion.rlit;
		    troom->irregular = TRUE;
		} else {
		    add_room(tmpregion.x1, tmpregion.y1,
			     tmpregion.x2, tmpregion.y2,
			     tmpregion.rlit, tmpregion.rtype, TRUE, FALSE);
#ifdef SPECIALIZATION
		    topologize(troom,FALSE);		/* set roomno */
#else
		    topologize(troom);			/* set roomno */
#endif
		}
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of doors */
	while(n--) {
		struct mkroom *croom = &rooms[0];

		Fread((genericptr_t)&tmpdoor, 1, sizeof(tmpdoor), fd);

		x = tmpdoor.x;	y = tmpdoor.y;
		typ = tmpdoor.mask == -1 ? rnddoor() : tmpdoor.mask;

		if (get_location(&x, &y, DRY)) {
		    if(levl[x][y].typ != SDOOR)
			levl[x][y].typ = DOOR;
		    else {
			if(typ < D_CLOSED)
			    typ = D_CLOSED; /* force it to be closed */
		    }
		    levl[x][y].doormask = typ;

		    /* Fix random door alignment */

		    if (y > 0 && (IS_WALL(levl[x][y-1].typ) ||
				levl[x][y-1].horizontal))
			levl[x][y].horizontal = 0;
		}

		/* Now the complicated part, list it with each subroom */
		/* The dog move and mail daemon routines use this */
		xi = -1;
		while(croom->hx >= 0 && doorindex < DOORMAX) {
		    if(croom->hx >= x-1 && croom->lx <= x+1 &&
		       croom->hy >= y-1 && croom->ly <= y+1) {
			/* Found it */
			xi = add_door(x, y, croom);
			doors[xi].arti_key = tmpdoor.arti_key;
		    }
		    croom++;
		}
		if (xi < 0) {	/* Not in any room */
		    if (doorindex >= DOORMAX)
			impossible("Too many doors?");
		    else {
			xi = add_door(x, y, (struct mkroom *)0);
			doors[xi].arti_key = tmpdoor.arti_key;
		    }
		}
	}

	/* now that we have rooms _and_ associated doors, fill the rooms */
	for(n = 0; n < SIZE(mustfill); n++)
	    if(mustfill[(int)n])
		fill_room(&rooms[(int)n], (mustfill[(int)n] == 2));

	/* if special boundary syms (CROSSWALL) in map, remove them now */
	if(has_bounds) {
	    for(x = xstart; x < xstart+xsize; x++)
		for(y = ystart; y < ystart+ysize; y++)
		    if(levl[x][y].typ == CROSSWALL)
			levl[x][y].typ = ROOM;
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of drawbridges */
	while(n--) {
		Fread((genericptr_t)&tmpdb, 1, sizeof(tmpdb), fd);

		x = tmpdb.x;  y = tmpdb.y;
		if (get_location(&x, &y, DRY|WET)) {
		if (!create_drawbridge(x, y, tmpdb.dir, tmpdb.db_open))
		    impossible("Cannot create drawbridge.");
	}
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of mazewalks */
	while(n--) {
		Fread((genericptr_t)&tmpwalk, 1, sizeof(tmpwalk), fd);

		if (get_location(&tmpwalk.x, &tmpwalk.y, DRY|WET))
		walklist[nwalk++] = tmpwalk;
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of non_diggables */
	while(n--) {
		Fread((genericptr_t)&tmpdig, 1, sizeof(tmpdig), fd);

		get_location(&tmpdig.x1, &tmpdig.y1, DRY|WET);
		get_location(&tmpdig.x2, &tmpdig.y2, DRY|WET);

		set_wall_property(tmpdig.x1, tmpdig.y1,
				  tmpdig.x2, tmpdig.y2, W_NONDIGGABLE);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of non_passables */
	while(n--) {
		Fread((genericptr_t)&tmpdig, 1, sizeof(tmpdig), fd);

		get_location(&tmpdig.x1, &tmpdig.y1, DRY|WET);
		get_location(&tmpdig.x2, &tmpdig.y2, DRY|WET);

		set_wall_property(tmpdig.x1, tmpdig.y1,
				  tmpdig.x2, tmpdig.y2, W_NONPASSWALL);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of ladders */
	while(n--) {
		Fread((genericptr_t)&tmplad, 1, sizeof(tmplad), fd);

		x = tmplad.x;  y = tmplad.y;
		if (get_location(&x, &y, DRY)) {
		levl[x][y].typ = LADDER;
		if (tmplad.up == 1) {
			xupladder = x;	yupladder = y;
			levl[x][y].ladder = LA_UP;
		} else {
			xdnladder = x;	ydnladder = y;
			levl[x][y].ladder = LA_DOWN;
		}
	}
	}

	prevstair.x = prevstair.y = 0;
	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of stairs */
	while(n--) {
		boolean found;
		Fread((genericptr_t)&tmpstair, 1, sizeof(tmpstair), fd);

		xi = 0;
		do {
		    x = tmpstair.x;  y = tmpstair.y;
		    found = get_location(&x, &y, DRY);
		} while(found && prevstair.x && xi++ < 100 &&
			distmin(x,y,prevstair.x,prevstair.y) <= 8);
		if (!found)
		    continue;
		if ((badtrap = t_at(x,y)) != 0) deltrap(badtrap);
		mkstairs(x, y, (char)tmpstair.up, (struct mkroom *)0);
		prevstair.x = x;
		prevstair.y = y;
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of altars */
	while(n--) {
		Fread((genericptr_t)&tmpaltar, 1, sizeof(tmpaltar), fd);

		create_altar(&tmpaltar, (struct mkroom *)0);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of fountains */
	while (n--) {
		Fread((genericptr_t)&tmpfountain, 1, sizeof(tmpfountain), fd);

		create_feature(tmpfountain.x, tmpfountain.y,
			       (struct mkroom *)0, FOUNTAIN);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of traps */
	while(n--) {
		Fread((genericptr_t)&tmptrap, 1, sizeof(tmptrap), fd);

		create_trap(&tmptrap, (struct mkroom *)0);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of monsters */
	while(n--) {
		load_one_monster(fd, &tmpmons);

		create_monster(&tmpmons, (struct mkroom *)0);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of objects */
	while(n--) {
		load_one_object(fd, &tmpobj);

		create_object(&tmpobj, (struct mkroom *)0);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of gold piles */
	while (n--) {
		Fread((genericptr_t)&tmpgold, 1, sizeof(tmpgold), fd);

		create_gold(&tmpgold, (struct mkroom *)0);
	}

	Fread((genericptr_t) &n, 1, sizeof(n), fd);
						/* Number of engravings */
	while(n--) {
		load_one_engraving(fd, &tmpengraving);

		create_engraving(&tmpengraving, (struct mkroom *)0);
	}

    }		/* numpart loop */

    nwalk_sav = nwalk;
    while(nwalk--) {
	    x = (xchar) walklist[nwalk].x;
	    y = (xchar) walklist[nwalk].y;
	    dir = walklist[nwalk].dir;

	    /* don't use move() - it doesn't use W_NORTH, etc. */
	    switch (dir) {
		case W_NORTH: --y; break;
		case W_SOUTH: y++; break;
		case W_EAST:  x++; break;
		case W_WEST:  --x; break;
		default: panic("load_maze: bad MAZEWALK direction");
	    }

	    if(!IS_DOOR(levl[x][y].typ) && !specialcorridor) {
#ifndef WALLIFIED_MAZE
		levl[x][y].typ = CORR;
#else
		levl[x][y].typ = ROOM;
#endif
		levl[x][y].flags = 0;
	    }
	    if(!IS_DOOR(levl[x][y].typ) && specialcorridor) {

		levl[x][y].typ = (specialcorridor == 1) ? ICE : CLOUD;
		levl[x][y].flags = 0;
	    }

	    /*
	     * We must be sure that the parity of the coordinates for
	     * walkfrom() is odd.  But we must also take into account
	     * what direction was chosen.
	     */
	    if(!(x % 2)) {
		if (dir == W_EAST)
		    x++;
		else
		    x--;

		/* no need for IS_DOOR check; out of map bounds */

		if (!specialcorridor) {
#ifndef WALLIFIED_MAZE
		levl[x][y].typ = CORR;
#else
		levl[x][y].typ = ROOM;
#endif
		}
		else levl[x][y].typ = (specialcorridor == 1) ? ICE : CLOUD;

		levl[x][y].flags = 0;
	    }

	    if (!(y % 2)) {
		if (dir == W_SOUTH)
		    y++;
		else
		    y--;
	    }

	    walkfrom(x, y);
    }
    /* wallification(1, 0, COLNO-1, ROWNO-1, FALSE); */
    wallification(1, 0, COLNO-1, ROWNO-1, TRUE);

    /*
     * If there's a significant portion of maze unused by the special level,
     * we don't want it empty.
     *
     * Makes the number of traps, monsters, etc. proportional
     * to the size of the maze.
     */
    mapcountmax = mapcount = (x_maze_max - 2) * (y_maze_max - 2);

    for(x = 2; x < x_maze_max; x++)
	for(y = 0; y < y_maze_max; y++)
	    if(Map[x][y]) mapcount--;

    if (nwalk_sav && (mapcount > (int) (mapcountmax / 10))) {
	    mapfact = (int) ((mapcount * 100L) / mapcountmax);
	    for(x = rnd((int) (20 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mkobj_at(rn2(2) ? GEM_CLASS : RANDOM_CLASS,
							mm.x, mm.y, TRUE);
	    }
	    for(x = rnd((int) (12 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mksobj_at(BOULDER, mm.x, mm.y, TRUE, FALSE);
	    }
	    for (x = rn2(20); x; x--) 	{ 
		    maze1xy(&mm, DRY);
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(mm.x, mm.y, mesg, 0L, MARK);
			}
	    for (x = rn2(2); x; x--) { if (depth(&u.uz) > depth(&medusa_level)) {
		maze1xy(&mm, DRY);
		if (!ishomicider) (void) makemon(&mons[PM_MINOTAUR], mm.x, mm.y, NO_MM_FLAGS);
		else makerandomtrap_at(mm.x, mm.y);
		} /* cause they would be outta depth when mazes are generated at a shallow level --Amy */
	    }
	    for(x = rnd((int) (12 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, WET|DRY);
		    if (!ishomicider) (void) makemon((struct permonst *) 0, mm.x, mm.y, NO_MM_FLAGS);
		    else makerandomtrap_at(mm.x, mm.y);
	    }
	    for(x = rn2((int) (15 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mkgold(0L,mm.x,mm.y);
	    }
	    for(x = rn2((int) (15 * mapfact) / 100); x; x--) {
		    int trytrap;

		    maze1xy(&mm, DRY);
		    trytrap = rndtrap();
		    if (sobj_at(BOULDER, mm.x, mm.y))
			while (trytrap == PIT || trytrap == SPIKED_PIT || trytrap == GIANT_CHASM || trytrap == SHIT_PIT || trytrap == SHAFT_TRAP ||
				trytrap == TRAPDOOR || trytrap == HOLE)
			    trytrap = rndtrap();
		    (void) maketrap(mm.x, mm.y, trytrap);
	    }

		if (ishaxor) {
	    for(x = rnd((int) (20 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mkobj_at(rn2(2) ? GEM_CLASS : RANDOM_CLASS,
							mm.x, mm.y, TRUE);
	    }
	    for(x = rnd((int) (12 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mksobj_at(BOULDER, mm.x, mm.y, TRUE, FALSE);
	    }
	    for (x = rn2(20); x; x--) 	{ 
		    maze1xy(&mm, DRY);
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(mm.x, mm.y, mesg, 0L, MARK);
			}
	    for (x = rn2(2); x; x--) { if (depth(&u.uz) > depth(&medusa_level)) {
		maze1xy(&mm, DRY);
		if (!ishomicider) (void) makemon(&mons[PM_MINOTAUR], mm.x, mm.y, NO_MM_FLAGS);
		else makerandomtrap_at(mm.x, mm.y);
		} /* cause they would be outta depth when mazes are generated at a shallow level --Amy */
	    }
	    for(x = rnd((int) (12 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, WET|DRY);
		    if (!ishomicider) (void) makemon((struct permonst *) 0, mm.x, mm.y, NO_MM_FLAGS);
		    else makerandomtrap_at(mm.x, mm.y);
	    }
	    for(x = rn2((int) (15 * mapfact) / 100); x; x--) {
		    maze1xy(&mm, DRY);
		    (void) mkgold(0L,mm.x,mm.y);
	    }
	    for(x = rn2((int) (15 * mapfact) / 100); x; x--) {
		    int trytrap;

		    maze1xy(&mm, DRY);
		    trytrap = rndtrap();
		    if (sobj_at(BOULDER, mm.x, mm.y))
			while (trytrap == PIT || trytrap == SPIKED_PIT || trytrap == GIANT_CHASM || trytrap == SHIT_PIT || trytrap == SHAFT_TRAP ||
				trytrap == TRAPDOOR || trytrap == HOLE)
			    trytrap = rndtrap();
		    (void) maketrap(mm.x, mm.y, trytrap);
	    }
		}

    }

	/* make rivers if possible --Amy */
	if (!rn2(50) && !In_endgame(&u.uz) ) mkriversX();
	if (!rn2(250) && !In_endgame(&u.uz) ) mkriversX();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) ) mkriversX();
		if (!rn2(250) && !In_endgame(&u.uz) ) mkriversX();
	}

	if (isaquarian && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversX();

	if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();
	if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();
		if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();
	}

	if (isaquarian && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversX();

    return TRUE;
}

/*
 * General loader
 */

boolean
load_special(name)
const char *name;
{
	dlb *fd;
	boolean result = FALSE;
	char c;
	struct version_info vers_info;

	fd = dlb_fopen_area(FILE_AREA_UNSHARE, name, RDBMODE);
	if (!fd) return FALSE;

	Fread((genericptr_t) &vers_info, sizeof vers_info, 1, fd);
	if (!check_version(&vers_info, name, TRUE))
	    goto give_up;

	Fread((genericptr_t) &c, sizeof c, 1, fd); /* c Header */

	switch (c) {
		case SP_LEV_ROOMS:
		    result = load_rooms(fd);
		    break;
		case SP_LEV_MAZE:
		    result = load_maze(fd);
		    break;
		default:	/* ??? */
		    result = FALSE;
	}
 give_up:
	(void)dlb_fclose(fd);
	return result;
}




/*sp_lev.c*/


/*	SCCS Id: @(#)mkmaze.c	3.4	2002/04/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "sp_lev.h"
#include "lev.h"	/* save & restore info */

/* from sp_lev.c, for fixup_special() */
extern char *lev_message;
extern lev_region *lregions;
extern int num_lregions;

STATIC_DCL boolean FDECL(iswall,(int,int));
STATIC_DCL boolean FDECL(iswall_or_stone,(int,int));
STATIC_DCL boolean FDECL(is_solid,(int,int));
STATIC_DCL int FDECL(extend_spine, (int [3][3], int, int, int));
STATIC_DCL boolean FDECL(okay,(int,int,int));
STATIC_DCL void FDECL(maze0xy,(coord *));
STATIC_DCL boolean FDECL(put_lregion_here,(XCHAR_P,XCHAR_P,XCHAR_P,
	XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,BOOLEAN_P,d_level *));
STATIC_DCL void NDECL(fixup_special);
STATIC_DCL void FDECL(move, (int *,int *,int));
STATIC_DCL void NDECL(setup_waterlevel);
STATIC_DCL void NDECL(unsetup_waterlevel);

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

STATIC_OVL boolean
iswall(x,y)
int x,y;
{
    register int type;

    if (!isok(x,y)) return FALSE;
    type = levl[x][y].typ;
    return (IS_WALL(type) || IS_DOOR(type) ||
	    type == SDOOR || type == IRONBARS);
}

STATIC_OVL boolean
iswall_or_stone(x,y)
    int x,y;
{
    register int type;

    /* out of bounds = stone */
    if (!isok(x,y)) return TRUE;

    type = levl[x][y].typ;
    return (type == STONE || IS_WALL(type) || IS_DOOR(type) ||
	    type == SDOOR || type == IRONBARS);
}

/* return TRUE if out of bounds, wall or rock */
STATIC_OVL boolean
is_solid(x,y)
    int x, y;
{
    return (!isok(x,y) || IS_STWALL(levl[x][y].typ));
}


/*
 * Return 1 (not TRUE - we're doing bit vectors here) if we want to extend
 * a wall spine in the (dx,dy) direction.  Return 0 otherwise.
 *
 * To extend a wall spine in that direction, first there must be a wall there.
 * Then, extend a spine unless the current position is surrounded by walls
 * in the direction given by (dx,dy).  E.g. if 'x' is our location, 'W'
 * a wall, '.' a room, 'a' anything (we don't care), and our direction is
 * (0,1) - South or down - then:
 *
 *		a a a
 *		W x W		This would not extend a spine from x down
 *		W W W		(a corridor of walls is formed).
 *
 *		a a a
 *		W x W		This would extend a spine from x down.
 *		. W W
 */
STATIC_OVL int
extend_spine(locale, wall_there, dx, dy)
    int locale[3][3];
    int wall_there, dx, dy;
{
    int spine, nx, ny;

    nx = 1 + dx;
    ny = 1 + dy;

    if (wall_there) {	/* wall in that direction */
	if (dx) {
	    if (locale[ 1][0] && locale[ 1][2] && /* EW are wall/stone */
		locale[nx][0] && locale[nx][2]) { /* diag are wall/stone */
		spine = 0;
	    } else {
		spine = 1;
	    }
	} else {	/* dy */
	    if (locale[0][ 1] && locale[2][ 1] && /* NS are wall/stone */
		locale[0][ny] && locale[2][ny]) { /* diag are wall/stone */
		spine = 0;
	    } else {
		spine = 1;
	    }
	}
    } else {
	spine = 0;
    }

    return spine;
}


/*
 * Walls to surprise jaded Gehennom-haters :)
 *
 * Wall cleanup. This function turns all wall squares into 'floortype' squares.
 */
STATIC_OVL
void
wallify_special(x1, y1, x2, y2, floortype)
int x1, y1, x2, y2;
int floortype;		/* The 'wall' floortype */
{
	uchar type;
	register int x,y;
	struct rm *lev;

	int density = 3;
	if (!(u.monstertimefinish % 479)) density += rnd(5);
	if (!(u.monstertimefinish % 1879)) density += rnd(10);
	if (!rn2(5)) density += 1;
	if (!rn2(10)) density += rnd(3);
	if (!rn2(25)) density += rnd(5);
	if (!rn2(125)) density += rnd(10);
	if (!rn2(750)) density += rnd(20);

	/* sanity check on incoming variables */
	if (x1<0 || x2>=COLNO || x1>x2 || y1<0 || y2>=ROWNO || y1>y2)
	    panic("wallify_fire: bad bounds (%d,%d) to (%d,%d)",x1,y1,x2,y2);

	/* Translate the maze... */
	for(x = x1; x <= x2; x++)
	    for(y = y1; y <= y2; y++) {
		lev = &levl[x][y];
		type = lev->typ;
		if ( IS_WALL(type) && (rnd(density) < 4))
		    lev->typ = (floortype == CROSSWALL) ? randomwalltype() : floortype;
		else if IS_WALL(type)
		    lev->typ = STONE;
		/* Doors become room squares. Does this make sense? */
		else if (IS_DOOR(type))
		    lev->typ = ROOM;
		else if (type == SDOOR)
		    lev->typ = ROOM;
		else if (type == SCORR)
		    lev->typ = ROOM;
	    }

	return;
}

/*
 * Wall cleanup.  This function has two purposes: (1) remove walls that
 * are totally surrounded by stone - they are redundant.  (2) correct
 * the types so that they extend and connect to each other.
 */
STATIC_OVL
void
wallify_stone(x1, y1, x2, y2)	/* [Lethe] Classic stone walls */
int x1, y1, x2, y2;
{
	uchar type;
	register int x,y;
	struct rm *lev;
	int bits;
	int locale[3][3];	/* rock or wall status surrounding positions */
	/*
	 * Value 0 represents a free-standing wall.  It could be anything,
	 * so even though this table says VWALL, we actually leave whatever
	 * typ was there alone.
	 */
	static xchar spine_array[16] = {
	    VWALL,	HWALL,		HWALL,		HWALL,
	    VWALL,	TRCORNER,	TLCORNER,	TDWALL,
	    VWALL,	BRCORNER,	BLCORNER,	TUWALL,
	    VWALL,	TLWALL,		TRWALL,		CROSSWALL
	};

	/* sanity check on incoming variables */
	if (x1<0 || x2>=COLNO || x1>x2 || y1<0 || y2>=ROWNO || y1>y2)
	    panic("wallify_stone: bad bounds (%d,%d) to (%d,%d)",x1,y1,x2,y2);

	/* Step 1: change walls surrounded by rock to rock. */
	for(x = x1; x <= x2; x++)
	    for(y = y1; y <= y2; y++) {
		lev = &levl[x][y];
		type = lev->typ;
		if (IS_WALL(type) && type != DBWALL) {
		    if (is_solid(x-1,y-1) &&
			is_solid(x-1,y  ) &&
			is_solid(x-1,y+1) &&
			is_solid(x,  y-1) &&
			is_solid(x,  y+1) &&
			is_solid(x+1,y-1) &&
			is_solid(x+1,y  ) &&
			is_solid(x+1,y+1))
		    lev->typ = STONE;
		}
	    }

	/*
	 * Step 2: set the correct wall type.  We can't combine steps
	 * 1 and 2 into a single sweep because we depend on knowing if
	 * the surrounding positions are stone.
	 */
	for(x = x1; x <= x2; x++)
	    for(y = y1; y <= y2; y++) {
		lev = &levl[x][y];
		type = lev->typ;
		if ( !(IS_WALL(type) && type != DBWALL)) continue;

		/* set the locations TRUE if rock or wall or out of bounds */
		locale[0][0] = iswall_or_stone(x-1,y-1);
		locale[1][0] = iswall_or_stone(  x,y-1);
		locale[2][0] = iswall_or_stone(x+1,y-1);

		locale[0][1] = iswall_or_stone(x-1,  y);
		locale[2][1] = iswall_or_stone(x+1,  y);

		locale[0][2] = iswall_or_stone(x-1,y+1);
		locale[1][2] = iswall_or_stone(  x,y+1);
		locale[2][2] = iswall_or_stone(x+1,y+1);

		/* determine if wall should extend to each direction NSEW */
		bits =    (extend_spine(locale, iswall(x,y-1),  0, -1) << 3)
			| (extend_spine(locale, iswall(x,y+1),  0,  1) << 2)
			| (extend_spine(locale, iswall(x+1,y),  1,  0) << 1)
			|  extend_spine(locale, iswall(x-1,y), -1,  0);

		/* don't change typ if wall is free-standing */
		if (bits) lev->typ = spine_array[bits];
	    }
}

/*
 * Wall cleanup.  This selects an appropriate function to sort out the 
 * dungeon walls.                                                    
 */
void
wallification(x1, y1, x2, y2, initial)
int x1, y1, x2, y2;
boolean initial;
{
	/* Wallify normally unless creating a full maze level */
	if (!initial) {
		wallify_stone(x1, y1, x2, y2);
		return;
	}

	/* Put in the walls... */
	{
	    int wallchoice = rn2(100);

	    if(!(u.monstertimefinish % 43)) wallchoice = (50 + rn2(50));

	    if(!(u.monstertimefinish % 943)) wallchoice = (75 + rn2(25));

	    if(!(u.monstertimefinish % 3943)) wallchoice = (85 + rn2(15));

	    if (wallchoice < 88)
		wallify_stone(x1, y1, x2, y2);
	    else if (wallchoice < 89)
		wallify_special(x1, y1, x2, y2, CROSSWALL);
	    else if (wallchoice < 90)
		wallify_special(x1, y1, x2, y2, CLOUD);
	    else if (wallchoice < 92)
		wallify_special(x1, y1, x2, y2, MOAT);
	    else if (wallchoice < 94)
		wallify_special(x1, y1, x2, y2, ICE);
	    else if (wallchoice < 96)
		wallify_special(x1, y1, x2, y2, LAVAPOOL);
	    else if (wallchoice < 97)
		wallify_special(x1, y1, x2, y2, TREE);
	    else
		wallify_special(x1, y1, x2, y2, IRONBARS);
	}
	return;
}

STATIC_OVL boolean
okay(x,y,dir)
int x,y;
register int dir;
{
	move(&x,&y,dir);
	move(&x,&y,dir);
	if(x<3 || y<3 || x>x_maze_max || y>y_maze_max || levl[x][y].typ != 0)
		return(FALSE);
	return(TRUE);
}

STATIC_OVL void
maze0xy(cc)	/* find random starting point for maze generation */
	coord	*cc;
{
	cc->x = 3 + 2*rn2((x_maze_max>>1) - 1);
	cc->y = 3 + 2*rn2((y_maze_max>>1) - 1);
	return;
}

/*
 * Bad if:
 *	pos is occupied OR
 *	pos is inside restricted region (lx,ly,hx,hy) OR
 *	NOT (pos is corridor and a maze level OR pos is a room OR pos is air)
 */
boolean
bad_location(x, y, lx, ly, hx, hy)
    xchar x, y;
    xchar lx, ly, hx, hy;
{
    return((boolean)(occupied(x, y) ||
	   within_bounded_area(x,y, lx,ly, hx,hy) ||
	   !((levl[x][y].typ == CORR /*&& level.flags.is_maze_lev*/) ||
	       levl[x][y].typ == ROOM || levl[x][y].typ == AIR || levl[x][y].typ == CLOUD || levl[x][y].typ == ICE)));
}

/* pick a location in area (lx, ly, hx, hy) but not in (nlx, nly, nhx, nhy) */
/* and place something (based on rtype) in that region */
void
place_lregion(lx, ly, hx, hy, nlx, nly, nhx, nhy, rtype, lev)
    xchar	lx, ly, hx, hy;
    xchar	nlx, nly, nhx, nhy;
    xchar	rtype;
    d_level	*lev;
{
    int trycnt;
    boolean oneshot;
    xchar x, y;

    if(!lx) { /* default to whole level */
	/*
	 * if there are rooms and this a branch, let place_branch choose
	 * the branch location (to avoid putting branches in corridors).
	 */
	if(rtype == LR_BRANCH && nroom) {
	    place_branch(Is_branchlev(&u.uz), 0, 0);
	    return;
	}

	lx = 1; hx = COLNO-1;
	ly = 1; hy = ROWNO-1;
    }

    /* first a probabilistic approach */

    oneshot = (lx == hx && ly == hy);
    for (trycnt = 0; trycnt < 200; trycnt++) {
	x = rn1((hx - lx) + 1, lx);
	y = rn1((hy - ly) + 1, ly);
	if (put_lregion_here(x,y,nlx,nly,nhx,nhy,rtype,oneshot,lev))
	    return;
    }

    /* then a deterministic one */

    oneshot = TRUE;
    for (x = lx; x <= hx; x++)
	for (y = ly; y <= hy; y++)
	    if (put_lregion_here(x,y,nlx,nly,nhx,nhy,rtype,oneshot,lev))
		return;

    /*impossible*/pline("Couldn't place lregion type %d!", rtype);
}

STATIC_OVL boolean
put_lregion_here(x,y,nlx,nly,nhx,nhy,rtype,oneshot,lev)
xchar x, y;
xchar nlx, nly, nhx, nhy;
xchar rtype;
boolean oneshot;
d_level *lev;
{
    if (bad_location(x, y, nlx, nly, nhx, nhy)) {
	if (!oneshot) {
	    return FALSE;		/* caller should try again */
	} else {
	    /* Must make do with the only location possible;
	       avoid failure due to a misplaced trap.
	       It might still fail if there's a dungeon feature here. */
	    struct trap *t = t_at(x,y);

	    if (t && t->ttyp != MAGIC_PORTAL) deltrap(t);
	    if (bad_location(x, y, nlx, nly, nhx, nhy)) return FALSE;
	}
    }
    switch (rtype) {
    case LR_TELE:
    case LR_UPTELE:
    case LR_DOWNTELE:
	/* "something" means the player in this case */
	if(MON_AT(x, y)) {
	    /* move the monster if no choice, or just try again */
	    if(oneshot) (void) rloc(m_at(x,y), FALSE);
	    else return(FALSE);
	}
	u_on_newpos(x, y);
	break;
    case LR_PORTAL:
	mkportal(x, y, lev->dnum, lev->dlevel);
	break;
    case LR_DOWNSTAIR:
    case LR_UPSTAIR:
	mkstairs(x, y, (char)rtype, (struct mkroom *)0);
	break;
    case LR_BRANCH:
	place_branch(Is_branchlev(&u.uz), x, y);
	break;
    }
    return(TRUE);
}

static boolean was_waterlevel; /* ugh... this shouldn't be needed */

/* this is special stuff that the level compiler cannot (yet) handle */
STATIC_OVL void
fixup_special()
{
    register lev_region *r = lregions;
    struct d_level lev;
    register int x, y;
    struct mkroom *croom;
    boolean added_branch = FALSE;

    if (was_waterlevel) {
	was_waterlevel = FALSE;
	u.uinwater = 0;
	unsetup_waterlevel();
    } else if (Is_waterlevel(&u.uz)) {
	level.flags.hero_memory = 0;
	was_waterlevel = TRUE;
	/* water level is an odd beast - it has to be set up
	   before calling place_lregions etc. */
	setup_waterlevel();
    }
    for(x = 0; x < num_lregions; x++, r++) {
	switch(r->rtype) {
	case LR_BRANCH:
	    added_branch = TRUE;
	    goto place_it;

	case LR_PORTAL:
	    if(*r->rname.str >= '0' && *r->rname.str <= '9') {
		/* "chutes and ladders" */
		lev = u.uz;
		lev.dlevel = atoi(r->rname.str);
	    } else {
		s_level *sp = find_level(r->rname.str);
		lev = sp->dlevel;
	    }
	    /* fall into... */

	case LR_UPSTAIR:
	case LR_DOWNSTAIR:
	place_it:
	    place_lregion(r->inarea.x1, r->inarea.y1,
			  r->inarea.x2, r->inarea.y2,
			  r->delarea.x1, r->delarea.y1,
			  r->delarea.x2, r->delarea.y2,
			  r->rtype, &lev);
	    break;

	case LR_TELE:
	case LR_UPTELE:
	case LR_DOWNTELE:
	    /* save the region outlines for goto_level() */
	    if(r->rtype == LR_TELE || r->rtype == LR_UPTELE) {
		    updest.lx = r->inarea.x1; updest.ly = r->inarea.y1;
		    updest.hx = r->inarea.x2; updest.hy = r->inarea.y2;
		    updest.nlx = r->delarea.x1; updest.nly = r->delarea.y1;
		    updest.nhx = r->delarea.x2; updest.nhy = r->delarea.y2;
	    }
	    if(r->rtype == LR_TELE || r->rtype == LR_DOWNTELE) {
		    dndest.lx = r->inarea.x1; dndest.ly = r->inarea.y1;
		    dndest.hx = r->inarea.x2; dndest.hy = r->inarea.y2;
		    dndest.nlx = r->delarea.x1; dndest.nly = r->delarea.y1;
		    dndest.nhx = r->delarea.x2; dndest.nhy = r->delarea.y2;
	    }
	    /* place_lregion gets called from goto_level() */
	    break;
	}

	if (r->rname.str) free((genericptr_t) r->rname.str),  r->rname.str = 0;
    }

    /* place dungeon branch if not placed above */
    if (!added_branch && Is_branchlev(&u.uz)) {
	place_lregion(0,0,0,0,0,0,0,0,LR_BRANCH,(d_level *)0);
    }

	/* KMH -- arboreal levels */
/*	if (level.flags.arboreal)
		for(x = 2; x < x_maze_max; x++)
			for(y = 2; y < y_maze_max; y++)
				if (levl[x][y].typ == STONE)
					levl[x][y].typ = TREE;*/

	/* KMH -- Sokoban levels */
	if(In_sokoban(&u.uz))
		sokoban_detect();

    /* Still need to add some stuff to level file */
    if (Is_medusa_level(&u.uz)) {
	struct obj *otmp;
	int tryct;

	croom = &rooms[0]; /* only one room on the medusa level */
	for (tryct = rnd(4); tryct; tryct--) {
	    x = somex(croom); y = somey(croom);
	    if (goodpos(x, y, (struct monst *)0, 0)) {
		otmp = mk_tt_object(STATUE, x, y);
		while (otmp && (poly_when_stoned(&mons[otmp->corpsenm]) ||
				pm_resistance(&mons[otmp->corpsenm],MR_STONE))) {
		    otmp->corpsenm = rndmonnum();
		    otmp->owt = weight(otmp);
		}
	    }
	}

	if (rn2(2))
	    otmp = mk_tt_object(STATUE, somex(croom), somey(croom));
	else /* Medusa statues don't contain books */
	    otmp = mkcorpstat(STATUE, (struct monst *)0, (struct permonst *)0,
			      somex(croom), somey(croom), FALSE);
	if (otmp) {
	    while (pm_resistance(&mons[otmp->corpsenm],MR_STONE)
		   || poly_when_stoned(&mons[otmp->corpsenm])) {
		otmp->corpsenm = rndmonnum();
		otmp->owt = weight(otmp);
	    }
	}
    } else if(Is_wiz1_level(&u.uz)) {
	croom = search_special(MORGUE);

	create_secret_door(croom, W_SOUTH|W_EAST|W_WEST);
    } else if(Is_knox(&u.uz)) {
	/* using an unfilled morgue for rm id */
	croom = search_special(MORGUE);
	/* avoid inappropriate morgue-related messages */
	level.flags.graveyard = level.flags.has_morgue = 0;
	croom->rtype = OROOM;	/* perhaps it should be set to VAULT? */
	/* stock the main vault */
	for(x = croom->lx; x <= croom->hx; x++)
	    for(y = croom->ly; y <= croom->hy; y++) {
		(void) mkgold((long) rn1(300, 600), x, y);
		if (!rn2(3) && !is_pool(x,y))
		    (void)maketrap(x, y, rn2(3) ? LANDMINE : SPIKED_PIT);
	    }
    } else if (Role_if(PM_PRIEST) && In_quest(&u.uz)) {
	/* less chance for undead corpses (lured from lower morgues) */
	level.flags.graveyard = 1;
    } else if (Is_stronghold(&u.uz)) {
	level.flags.graveyard = 1;
    } else if(Is_sanctum(&u.uz)) {
	croom = search_special(TEMPLE);

	create_secret_door(croom, W_ANY);
    } else if(on_level(&u.uz, &orcus_level)) {
	   register struct monst *mtmp, *mtmp2;

	   /* it's a ghost town, get rid of shopkeepers */
	    for(mtmp = fmon; mtmp; mtmp = mtmp2) {
		    mtmp2 = mtmp->nmon;
		    if(mtmp->isshk) mongone(mtmp);
	    }
    }

    if(lev_message) {
	char *str, *nl;
	for(str = lev_message; (nl = index(str, '\n')) != 0; str = nl+1) {
	    *nl = '\0';
	    pline("%s", str);
	}
	if(*str)
	    pline("%s", str);
	free((genericptr_t)lev_message);
	lev_message = 0;
    }

    if (lregions)
	free((genericptr_t) lregions),  lregions = 0;
    num_lregions = 0;
}

STATIC_OVL void
makeriverY(x1,y1,x2,y2,lava,rndom)
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
	else*/ if (levl[cx][cy].typ == CORR) chance = 15;
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
makerandriverY(lava,rndom)
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
mkriversY()
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
	if (rn2(2)) makeriverY(0, rn2(ROWNO), COLNO-1, rn2(ROWNO), lava, rndom);
	else makeriverY(rn2(COLNO), 0, rn2(COLNO), ROWNO-1, lava, rndom);
    }
}

STATIC_OVL void
mkrandriversY()
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
	if (rn2(2)) makerandriverY(lava, rndom);
	else makerandriverY(lava, rndom);
    }
}

/*
 * Select a random coordinate in the maze.
 *
 * We want a place not 'touched' by the loader.  That is, a place in
 * the maze outside every part of the special level.
 */

void
makemaz(s)
register const char *s;
{
	int x,y;
	char protofile[20];
	s_level	*sp = Is_special(&u.uz);
	coord mm;

	if(*s) {
	    if(sp && sp->rndlevs) Sprintf(protofile, "%s-%d", s,
						rnd((int) sp->rndlevs));
	    else		 Strcpy(protofile, s);
	} else if(*(dungeons[u.uz.dnum].proto)) {
	    if(dunlevs_in_dungeon(&u.uz) > 1) {
		if(sp && sp->rndlevs)
		     Sprintf(protofile, "%s%d-%d", dungeons[u.uz.dnum].proto,
						dunlev(&u.uz),
						rnd((int) sp->rndlevs));
		else Sprintf(protofile, "%s%d", dungeons[u.uz.dnum].proto,
						dunlev(&u.uz));
	    } else if(sp && sp->rndlevs) {
		     Sprintf(protofile, "%s-%d", dungeons[u.uz.dnum].proto,
						rnd((int) sp->rndlevs));
	    } else Strcpy(protofile, dungeons[u.uz.dnum].proto);

	} else Strcpy(protofile, "");

#ifdef WIZARD
	/* SPLEVTYPE format is "level-choice,level-choice"... */
	if (wizard && *protofile && sp && sp->rndlevs) {
	    char *ep = getenv("SPLEVTYPE");	/* not nh_getenv */
	    if (ep) {
		/* rindex always succeeds due to code in prior block */
		int len = (rindex(protofile, '-') - protofile) + 1;

		while (ep && *ep) {
		    if (!strncmp(ep, protofile, len)) {
			int pick = atoi(ep + len);
			/* use choice only if valid */
			if (pick > 0 && pick <= (int) sp->rndlevs)
			    Sprintf(protofile + len, "%d", pick);
			break;
		    } else {
			ep = index(ep, ',');
			if (ep) ++ep;
		    }
		}
	    }
	}
#endif

	if(*protofile) {
	    Strcat(protofile, LEV_EXT);
	    if(load_special(protofile)) {
		fixup_special();
		/* some levels can end up with monsters
		   on dead mon list, including light source monsters */
		dmonsfree();
		return;	/* no mazification right now */
	    }
	    /* impossible("Couldn't load \"%s\" - making a maze.", protofile); */
	    pline("Couldn't load \"%s\" - making a maze.", protofile);
	}

	if (rn2(2)) level.flags.is_maze_lev = TRUE;

#ifndef WALLIFIED_MAZE
	for(x = 2; x < x_maze_max; x++)
		for(y = 2; y < y_maze_max; y++)
			levl[x][y].typ = STONE;
#else
	for(x = 2; x <= x_maze_max; x++)
		for(y = 2; y <= y_maze_max; y++)
			levl[x][y].typ = ((x % 2) && (y % 2)) ? STONE : HWALL;
#endif

	maze0xy(&mm);
	walkfrom((int) mm.x, (int) mm.y);
	/* put a boulder at the maze center */
	(void) mksobj_at(BOULDER, (int) mm.x, (int) mm.y, TRUE, FALSE);

#ifdef WALLIFIED_MAZE
	wallification(2, 2, x_maze_max, y_maze_max, TRUE);
#endif
	mazexy_all(&mm);
	mkstairs(mm.x, mm.y, 1, (struct mkroom *)0);		/* up */
	if (!Invocation_lev(&u.uz)) {
	    mazexy_all(&mm);
	    mkstairs(mm.x, mm.y, 0, (struct mkroom *)0);	/* down */
	} else {	/* choose "vibrating square" location */
#define x_maze_min 2
#define y_maze_min 2
	    /*
	     * Pick a position where the stairs down to Moloch's Sanctum
	     * level will ultimately be created.  At that time, an area
	     * will be altered:  walls removed, moat and traps generated,
	     * boulders destroyed.  The position picked here must ensure
	     * that that invocation area won't extend off the map.
	     *
	     * We actually allow up to 2 squares around the usual edge of
	     * the area to get truncated; see mkinvokearea(mklev.c).
	     */
#define INVPOS_X_MARGIN (6 - 2)
#define INVPOS_Y_MARGIN (5 - 2)
#define INVPOS_DISTANCE 11
	    int x_range = x_maze_max - x_maze_min - 2*INVPOS_X_MARGIN - 1,
		y_range = y_maze_max - y_maze_min - 2*INVPOS_Y_MARGIN - 1;

#ifdef DEBUG
	    if (x_range <= INVPOS_X_MARGIN || y_range <= INVPOS_Y_MARGIN ||
		   (x_range * y_range) <= (INVPOS_DISTANCE * INVPOS_DISTANCE))
		panic("inv_pos: maze is too small! (%d x %d)",
		      x_maze_max, y_maze_max);
#endif
	    inv_pos.x = inv_pos.y = 0; /*{occupied() => invocation_pos()}*/
	    do {
		x = rn1(x_range, x_maze_min + INVPOS_X_MARGIN + 1);
		y = rn1(y_range, y_maze_min + INVPOS_Y_MARGIN + 1);
		/* we don't want it to be too near the stairs, nor
		   to be on a spot that's already in use (wall|trap) */
	    } while (x == xupstair || y == yupstair ||	/*(direct line)*/
		     abs(x - xupstair) == abs(y - yupstair) ||
		     distmin(x, y, xupstair, yupstair) <= INVPOS_DISTANCE ||
		     !SPACE_POS(levl[x][y].typ) || occupied(x, y));
	    inv_pos.x = x;
	    inv_pos.y = y;
#undef INVPOS_X_MARGIN
#undef INVPOS_Y_MARGIN
#undef INVPOS_DISTANCE
#undef x_maze_min
#undef y_maze_min
	}

	/* place branch stair or portal */
	place_branch(Is_branchlev(&u.uz), 0, 0);

	for(x = rn1(8,16); x; x--) {
		mazexy_all(&mm);
		(void) mkobj_at(!rn2(3) ? GEM_CLASS : 0, mm.x, mm.y, TRUE);
	}
	for (x = rn1(2,10); x; x--) 	{ 
		mazexy_all(&mm);
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(mm.x, mm.y, mesg, 0L, MARK);
			}
	for(x = rn1(10,2); x; x--) {
		mazexy_all(&mm);
		(void) mksobj_at(BOULDER, mm.x, mm.y, TRUE, FALSE);
	}
	if (depth(&u.uz) > depth(&medusa_level)) {
	for (x = rn2(3); x; x--) {
		mazexy_all(&mm);
		if (!ishomicider) (void) makemon(&mons[PM_MINOTAUR], mm.x, mm.y, NO_MM_FLAGS);
		else makerandomtrap_at(mm.x, mm.y);
		}
	}	 /* cause they would be outta depth when mazes are generated at a shallow level --Amy */
	for(x = rn1(5,7); x; x--) {
		mazexy_all(&mm);
		if (!ishomicider) (void) makemon((struct permonst *) 0, mm.x, mm.y, NO_MM_FLAGS);
		else makerandomtrap_at(mm.x, mm.y);
	}
	for(x = rn1(6,7); x; x--) {
		mazexy_all(&mm);
		(void) mkgold(0L,mm.x,mm.y);
	}
	for(x = rn1(6,7); x; x--)
		mktrap(0,1,(struct mkroom *) 0, (coord*) 0);

	if (ishaxor) {
	for(x = rn1(8,16); x; x--) {
		mazexy_all(&mm);
		(void) mkobj_at(!rn2(3) ? GEM_CLASS : 0, mm.x, mm.y, TRUE);
	}
	for (x = rn1(2,10); x; x--) 	{ 
		mazexy_all(&mm);
			    char buf[BUFSZ];
				const char *mesg = random_engraving(buf);
			    make_engr_at(mm.x, mm.y, mesg, 0L, MARK);
			}
	for(x = rn1(10,2); x; x--) {
		mazexy_all(&mm);
		(void) mksobj_at(BOULDER, mm.x, mm.y, TRUE, FALSE);
	}
	if (depth(&u.uz) > depth(&medusa_level)) {
	for (x = rn2(3); x; x--) {
		mazexy_all(&mm);
		if (!ishomicider) (void) makemon(&mons[PM_MINOTAUR], mm.x, mm.y, NO_MM_FLAGS);
		else makerandomtrap_at(mm.x, mm.y);
		}
	}	 /* cause they would be outta depth when mazes are generated at a shallow level --Amy */
	for(x = rn1(5,7); x; x--) {
		mazexy_all(&mm);
		if (!ishomicider) (void) makemon((struct permonst *) 0, mm.x, mm.y, NO_MM_FLAGS);
		else makerandomtrap_at(mm.x, mm.y);
	}
	for(x = rn1(6,7); x; x--) {
		mazexy_all(&mm);
		(void) mkgold(0L,mm.x,mm.y);
	}
	for(x = rn1(6,7); x; x--)
		mktrap(0,1,(struct mkroom *) 0, (coord*) 0);

	}

	/* make rivers if possible --Amy */
	if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversY();
	if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversY();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversY();
		if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversY();
	}

	if (isaquarian && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkriversY();

	if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversY();
	if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversY();

	if (ishaxor) {
		if (!rn2(50) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversY();
		if (!rn2(250) && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversY();
	}

	if (isaquarian && !In_endgame(&u.uz) && !Invocation_lev(&u.uz) ) mkrandriversY();

}

#ifdef MICRO
/* Make the mazewalk iterative by faking a stack.  This is needed to
 * ensure the mazewalk is successful in the limited stack space of
 * the program.  This iterative version uses the minimum amount of stack
 * that is totally safe.
 */
void
walkfrom(x,y)
int x,y;
{
#define CELLS (ROWNO * COLNO) / 4		/* a maze cell is 4 squares */
	char mazex[CELLS + 1], mazey[CELLS + 1];	/* char's are OK */
	int q, a, dir, pos;
	int dirs[4];

	register int tryct = 0;
	register struct obj *otmpX;
	const char *str;

	int specialcorridor = 0;
	if (!rn2(iswarper ? 50 : 500)) specialcorridor = rnd(2);
	if (!(u.monstertimefinish % 325) && !rn2(iswarper ? 10 : 50)) specialcorridor = rnd(2);
	if (!((moves + u.monstertimefinish) % 857 ) && !rn2(iswarper ? 5 : 10)) specialcorridor = rnd(2);
	if (!((moves + u.monstertimefinish) % 2457 ) && !rn2(iswarper ? 2 : 4)) specialcorridor = rnd(2);
	if (!((moves + u.monstertimefinish) % 5857 )) specialcorridor = rnd(2);

	pos = 1;
	mazex[pos] = (char) x;
	mazey[pos] = (char) y;
	while (pos) {
		x = (int) mazex[pos];
		y = (int) mazey[pos];
		if(!IS_DOOR(levl[x][y].typ) && !specialcorridor) {
		    /* might still be on edge of MAP, so don't overwrite */
#ifndef WALLIFIED_MAZE
		    levl[x][y].typ = CORR;
#else
		    levl[x][y].typ = ROOM;
#endif
		    levl[x][y].flags = 0;

			if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!((moves + u.monstertimefinish) % 757 ) && !rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!((moves + u.monstertimefinish) % 759 ) && !rn2(ishaxor ? 250 : 500) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!((moves + u.monstertimefinish) % 761 ) && !rn2(ishaxor ? 250 : 500) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!((moves + u.monstertimefinish) % 763 ) && !rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = epitaphs[rn2(SIZE(epitaphs))];
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);

				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					}
				}
			else if (!((moves + u.monstertimefinish) % 765 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = epitaphs[rn2(SIZE(epitaphs))];
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					}
				}
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!((moves + u.monstertimefinish) % 767 ) && !rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!((moves + u.monstertimefinish) % 769 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!((moves + u.monstertimefinish) % 771 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!((moves + u.monstertimefinish) % 773 ) && !rn2(ishaxor ? 200 : 400) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!((moves + u.monstertimefinish) % 775 ) && !rn2(ishaxor ? 50 : 100) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;
			else if (!((moves + u.monstertimefinish) % 777 ) && !rn2(ishaxor ? 50 : 100) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;

		}
		if(!IS_DOOR(levl[x][y].typ) && specialcorridor) {

		    levl[x][y].typ = (specialcorridor == 1) ? ICE : CLOUD;
		    levl[x][y].flags = 0;
		}

		q = 0;
		for (a = 0; a < 4; a++)
			if(okay(x, y, a)) dirs[q++]= a;
		if (!q)
			pos--;
		else {
			dir = dirs[rn2(q)];
			move(&x, &y, dir);

			if (!specialcorridor) {
#ifndef WALLIFIED_MAZE
			levl[x][y].typ = CORR;
#else
			levl[x][y].typ = ROOM;
#endif

			if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!((moves + u.monstertimefinish) % 757 ) && !rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))){
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!((moves + u.monstertimefinish) % 759 ) && !rn2(ishaxor ? 250 : 500) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))){
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!((moves + u.monstertimefinish) % 761 ) && !rn2(ishaxor ? 250 : 500) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!((moves + u.monstertimefinish) % 763 ) && !rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = epitaphs[rn2(SIZE(epitaphs))];
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					}
				}
			else if (!((moves + u.monstertimefinish) % 765 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = epitaphs[rn2(SIZE(epitaphs))];
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					}
				}
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!((moves + u.monstertimefinish) % 767 ) && !rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!((moves + u.monstertimefinish) % 769 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!((moves + u.monstertimefinish) % 771 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!((moves + u.monstertimefinish) % 773 ) && !rn2(ishaxor ? 200 : 400) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!((moves + u.monstertimefinish) % 775 ) && !rn2(ishaxor ? 50 : 100) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;
			else if (!((moves + u.monstertimefinish) % 777 ) && !rn2(ishaxor ? 50 : 100) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;

			}
			else levl[x][y].typ = (specialcorridor == 1) ? ICE : CLOUD;

			move(&x, &y, dir);
			pos++;
			if (pos > CELLS)
				panic("Overflow in walkfrom");
			mazex[pos] = (char) x;
			mazey[pos] = (char) y;
		}
	}
}
#else

void
walkfrom(x,y)
int x,y;
{
	register int q,a,dir;
	int dirs[4];

	register int tryct = 0;
	register struct obj *otmpX;
	const char *str;

	int specialcorridor = 0;
	if (!rn2(iswarper ? 50 : 500)) specialcorridor = rnd(2);
	if (!(u.monstertimefinish % 325) && !rn2(iswarper ? 10 : 50)) specialcorridor = rnd(2);
	if (!((moves + u.monstertimefinish) % 857 ) && !rn2(iswarper ? 5 : 10)) specialcorridor = rnd(2);
	if (!((moves + u.monstertimefinish) % 2457 ) && !rn2(iswarper ? 2 : 4)) specialcorridor = rnd(2);
	if (!((moves + u.monstertimefinish) % 5857 )) specialcorridor = rnd(2);

	if(!IS_DOOR(levl[x][y].typ) && !specialcorridor) {
	    /* might still be on edge of MAP, so don't overwrite */
#ifndef WALLIFIED_MAZE
	    levl[x][y].typ = CORR;
#else
	    levl[x][y].typ = ROOM;
#endif
	    levl[x][y].flags = 0;

			if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!((moves + u.monstertimefinish) % 757 ) && !rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!((moves + u.monstertimefinish) % 759 ) && !rn2(ishaxor ? 250 : 500) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!((moves + u.monstertimefinish) % 761 ) && !rn2(ishaxor ? 250 : 500) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!((moves + u.monstertimefinish) % 763 ) && !rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = epitaphs[rn2(SIZE(epitaphs))];
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					}
				}
			else if (!((moves + u.monstertimefinish) % 765 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = epitaphs[rn2(SIZE(epitaphs))];
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					}
				}
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!((moves + u.monstertimefinish) % 767 ) && !rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!((moves + u.monstertimefinish) % 769 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!((moves + u.monstertimefinish) % 771 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!((moves + u.monstertimefinish) % 773 ) && !rn2(ishaxor ? 200 : 400) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!((moves + u.monstertimefinish) % 775 ) && !rn2(ishaxor ? 50 : 100) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;
			else if (!((moves + u.monstertimefinish) % 777 ) && !rn2(ishaxor ? 50 : 100) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;

	}
	if(!IS_DOOR(levl[x][y].typ) && specialcorridor) {

	    levl[x][y].typ = (specialcorridor == 1) ? ICE : CLOUD;
	    levl[x][y].flags = 0;
	}

	while(1) {
		q = 0;
		for(a = 0; a < 4; a++)
			if(okay(x,y,a)) dirs[q++]= a;
		if(!q) return;
		dir = dirs[rn2(q)];
		move(&x,&y,dir);
		if (!specialcorridor) {
#ifndef WALLIFIED_MAZE
		levl[x][y].typ = CORR;
#else
		levl[x][y].typ = ROOM;
#endif
			if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!((moves + u.monstertimefinish) % 757 ) && !rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = THRONE;
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!((moves + u.monstertimefinish) % 759 ) && !rn2(ishaxor ? 250 : 500) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = FOUNTAIN;
				level.flags.nfountains++;
				}
			else if (!rn2(ishaxor ? 2500 : 5000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!((moves + u.monstertimefinish) % 761 ) && !rn2(ishaxor ? 250 : 500) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = SINK;
				level.flags.nsinks++;
				}
			else if (!rn2(ishaxor ? 5000 : 10000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!((moves + u.monstertimefinish) % 763 ) && !rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TOILET;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
				str = epitaphs[rn2(SIZE(epitaphs))];
				del_engr_at(x, y);
				make_engr_at(x, y, str, 0L, HEADSTONE);
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					}
				}
			else if (!((moves + u.monstertimefinish) % 765 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = GRAVE;
	
				if (!rn2(3)) (void) mkgold(0L, x, y);
				for (tryct = rn2(5); tryct; tryct--) {
					    otmpX = mkobj(RANDOM_CLASS, TRUE);
					    if (!otmpX) return;
					    curse(otmpX);
					    otmpX->ox = x;
					    otmpX->oy = y;
					    add_to_buried(otmpX);
					}
				}
			else if (!rn2(ishaxor ? 10000 : 20000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!((moves + u.monstertimefinish) % 767 ) && !rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM))) {
				levl[x][y].typ = ALTAR;
				if (rn2(10)) levl[x][y].altarmask = Align2amask( A_NONE );
				else switch (rnd(3)) {
	
				case 1: levl[x][y].altarmask = Align2amask( A_LAWFUL ); break;
				case 2: levl[x][y].altarmask = Align2amask( A_NEUTRAL ); break;
				case 3: levl[x][y].altarmask = Align2amask( A_CHAOTIC ); break;
	
				}
			}
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!((moves + u.monstertimefinish) % 769 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = TREE;
			else if (!rn2(ishaxor ? 1000 : 2000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!((moves + u.monstertimefinish) % 771 ) && !rn2(ishaxor ? 100 : 200) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = MOAT;
			else if (!rn2(ishaxor ? 2000 : 4000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!((moves + u.monstertimefinish) % 773 ) && !rn2(ishaxor ? 200 : 400) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = LAVAPOOL;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!((moves + u.monstertimefinish) % 775 ) && !rn2(ishaxor ? 50 : 100) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = ICE;
			else if (!rn2(ishaxor ? 500 : 1000) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;
			else if (!((moves + u.monstertimefinish) % 777 ) && !rn2(ishaxor ? 50 : 100) && ((levl[x][y].typ == CORR) || (levl[x][y].typ == ROOM)))
				levl[x][y].typ = CLOUD;

		}
		else levl[x][y].typ = (specialcorridor == 1) ? ICE : CLOUD;
		move(&x,&y,dir);
		walkfrom(x,y);
	}
}
#endif /* MICRO */

STATIC_OVL void
move(x,y,dir)
register int *x, *y;
register int dir;
{
	switch(dir){
		case 0: --(*y); break;
		case 1: (*x)++; break;
		case 2: (*y)++; break;
		case 3: --(*x); break;
		default: panic("move: bad direction");
	}
}

void
mazexy(cc)	/* find random point in generated corridors,
		   so we don't create items in moats, bunkers, or walls */
	coord	*cc;
{
	int cpt=0;

	do {
	    cc->x = 3 + 2*rn2((x_maze_max>>1) - 1);
	    cc->y = 3 + 2*rn2((y_maze_max>>1) - 1);
	    cpt++;
	} while (cpt < 100 && levl[cc->x][cc->y].typ !=
#ifdef WALLIFIED_MAZE
		 ROOM
#else
		 CORR
#endif
		);
	if (cpt >= 100) {
		register int x, y;
		/* last try */
		for (x = 0; x < (x_maze_max>>1) - 1; x++)
		    for (y = 0; y < (y_maze_max>>1) - 1; y++) {
			cc->x = 3 + 2 * x;
			cc->y = 3 + 2 * y;
			if (levl[cc->x][cc->y].typ ==
#ifdef WALLIFIED_MAZE
			    ROOM
#else
			    CORR
#endif
			   ) return;
		    }

		for (x = 0; x < (x_maze_max>>1) - 1; x++)
		    for (y = 0; y < (y_maze_max>>1) - 1; y++) {
			cc->x = 3 + 2 * x;
			cc->y = 3 + 2 * y;
			if ((levl[cc->x][cc->y].typ == CLOUD) || (levl[cc->x][cc->y].typ == ICE)) return;
		    }

		panic("mazexy: can't find a place!");
	}
	return;
}

void
mazexy_all(cc)	/* mazexy() only returns "even-numbered" squares... --Amy */
	coord	*cc;
{
	int cpt=0;

	do {
	    cc->x = 3 + rn2((x_maze_max) - 1);
	    cc->y = 3 + rn2((y_maze_max) - 1);
	    cpt++;
	} while (cpt < 100 && levl[cc->x][cc->y].typ !=
#ifdef WALLIFIED_MAZE
		 ROOM
#else
		 CORR
#endif
		);
	if (cpt >= 100) {
		register int x, y;
		/* last try */
		for (x = 0; x < (x_maze_max) - 1; x++)
		    for (y = 0; y < (y_maze_max) - 1; y++) {
			cc->x = 3 + x;
			cc->y = 3 + y;
			if (levl[cc->x][cc->y].typ ==
#ifdef WALLIFIED_MAZE
			    ROOM
#else
			    CORR
#endif
			   ) return;
		    }

		for (x = 0; x < (x_maze_max) - 1; x++)
		    for (y = 0; y < (y_maze_max) - 1; y++) {
			cc->x = 3 + x;
			cc->y = 3 + y;
			if ((levl[cc->x][cc->y].typ == CLOUD) || (levl[cc->x][cc->y].typ == ICE)) return;
		    }

		panic("mazexy_all: can't find a place!");
	}
	return;
}

void
bound_digging()
/* put a non-diggable boundary around the initial portion of a level map.
 * assumes that no level will initially put things beyond the isok() range.
 *
 * we can't bound unconditionally on the last line with something in it,
 * because that something might be a niche which was already reachable,
 * so the boundary would be breached
 *
 * we can't bound unconditionally on one beyond the last line, because
 * that provides a window of abuse for WALLIFIED_MAZE special levels
 */
{
	register int x,y;
	register unsigned typ;
	register struct rm *lev;
	boolean found, nonwall;
	int xmin,xmax,ymin,ymax;

	if(Is_earthlevel(&u.uz)) return; /* everything diggable here */

	found = nonwall = FALSE;
	for(xmin=0; !found; xmin++) {
		lev = &levl[xmin][0];
		for(y=0; y<=ROWNO-1; y++, lev++) {
			typ = lev->typ;
			if(typ != STONE) {
				found = TRUE;
				if(!IS_WALL(typ)) nonwall = TRUE;
			}
		}
	}
	xmin -= (nonwall || !level.flags.is_maze_lev) ? 2 : 1;
	if (xmin < 0) xmin = 0;

	found = nonwall = FALSE;
	for(xmax=COLNO-1; !found; xmax--) {
		lev = &levl[xmax][0];
		for(y=0; y<=ROWNO-1; y++, lev++) {
			typ = lev->typ;
			if(typ != STONE) {
				found = TRUE;
				if(!IS_WALL(typ)) nonwall = TRUE;
			}
		}
	}
	xmax += (nonwall || !level.flags.is_maze_lev) ? 2 : 1;
	if (xmax >= COLNO) xmax = COLNO-1;

	found = nonwall = FALSE;
	for(ymin=0; !found; ymin++) {
		lev = &levl[xmin][ymin];
		for(x=xmin; x<=xmax; x++, lev += ROWNO) {
			typ = lev->typ;
			if(typ != STONE) {
				found = TRUE;
				if(!IS_WALL(typ)) nonwall = TRUE;
			}
		}
	}
	ymin -= (nonwall || !level.flags.is_maze_lev) ? 2 : 1;

	found = nonwall = FALSE;
	for(ymax=ROWNO-1; !found; ymax--) {
		lev = &levl[xmin][ymax];
		for(x=xmin; x<=xmax; x++, lev += ROWNO) {
			typ = lev->typ;
			if(typ != STONE) {
				found = TRUE;
				if(!IS_WALL(typ)) nonwall = TRUE;
			}
		}
	}
	ymax += (nonwall || !level.flags.is_maze_lev) ? 2 : 1;

	for (x = 0; x < COLNO; x++)
	  for (y = 0; y < ROWNO; y++)
	    if (y <= ymin || y >= ymax || x <= xmin || x >= xmax) {
#ifdef DCC30_BUG
		lev = &levl[x][y];
		lev->wall_info |= W_NONDIGGABLE;
#else
		levl[x][y].wall_info |= W_NONDIGGABLE;
#endif
	    }
}

void
mkportal(x, y, todnum, todlevel)
register xchar x, y, todnum, todlevel;
{
	/* a portal "trap" must be matched by a */
	/* portal in the destination dungeon/dlevel */
	register struct trap *ttmp = maketrap(x, y, MAGIC_PORTAL);

	if (!ttmp) {
		impossible("portal on top of portal??");
		return;
	}
#ifdef DEBUG
	pline("mkportal: at (%d,%d), to %s, level %d",
		x, y, dungeons[todnum].dname, todlevel);
#endif
	ttmp->dst.dnum = todnum;
	ttmp->dst.dlevel = todlevel;
	return;
}

/*
 * Special waterlevel stuff in endgame (TH).
 *
 * Some of these functions would probably logically belong to some
 * other source files, but they are all so nicely encapsulated here.
 */

/* to ease the work of debuggers at this stage */
#define register

#define CONS_OBJ   0
#define CONS_MON   1
#define CONS_HERO  2
#define CONS_TRAP  3

static struct bubble *bbubbles, *ebubbles;

static struct trap *wportal;
static int xmin, ymin, xmax, ymax;	/* level boundaries */
/* bubble movement boundaries */
#define bxmin (xmin + 1)
#define bymin (ymin + 1)
#define bxmax (xmax - 1)
#define bymax (ymax - 1)

STATIC_DCL void NDECL(set_wportal);
STATIC_DCL void FDECL(mk_bubble, (int,int,int));
STATIC_DCL void FDECL(mv_bubble, (struct bubble *,int,int,BOOLEAN_P));

void
movebubbles()
{
	static boolean up;
	register struct bubble *b;
	register int x, y, i, j;
	struct trap *btrap;
	static const struct rm water_pos =
#ifdef DISPLAY_LAYERS
		{ S_water, 0, 0, 0, 0, 0,
#else
		{ cmap_to_glyph(S_water),
#endif
		WATER, 0, 0, 0, 0, 0, 0, 0 };

	/* set up the portal the first time bubbles are moved */
	if (!wportal) set_wportal();

	vision_recalc(2);

	/*
	 * Pick up everything inside of a bubble then fill all bubble
	 * locations.
	 */

	for (b = up ? bbubbles : ebubbles; b; b = up ? b->next : b->prev) {
	    if (b->cons) panic("movebubbles: cons != null");
	    for (i = 0, x = b->x; i < (int) b->bm[0]; i++, x++)
		for (j = 0, y = b->y; j < (int) b->bm[1]; j++, y++)
		    if (b->bm[j + 2] & (1 << i)) {
			if (!isok(x,y)) {
			    impossible("movebubbles: bad pos (%d,%d)", x,y);
			    continue;
			}

			/* pick up objects, monsters, hero, and traps */
			if (OBJ_AT(x,y)) {
			    struct obj *olist = (struct obj *) 0, *otmp;
			    struct container *cons = (struct container *)
				alloc(sizeof(struct container));

			    while ((otmp = level.objects[x][y]) != 0) {
				remove_object(otmp);
				otmp->ox = otmp->oy = 0;
				otmp->nexthere = olist;
				olist = otmp;
			    }

			    cons->x = x;
			    cons->y = y;
			    cons->what = CONS_OBJ;
			    cons->list = (genericptr_t) olist;
			    cons->next = b->cons;
			    b->cons = cons;
			}
			if (MON_AT(x,y)) {
			    struct monst *mon = m_at(x,y);
			    struct container *cons = (struct container *)
				alloc(sizeof(struct container));

			    cons->x = x;
			    cons->y = y;
			    cons->what = CONS_MON;
			    cons->list = (genericptr_t) mon;

			    cons->next = b->cons;
			    b->cons = cons;

			    if(mon->wormno)
				remove_worm(mon);
			    else
				remove_monster(x, y);

			    newsym(x,y);	/* clean up old position */
			    mon->mx = mon->my = 0;
			}
			if (!u.uswallow && x == u.ux && y == u.uy) {
			    struct container *cons = (struct container *)
				alloc(sizeof(struct container));

			    cons->x = x;
			    cons->y = y;
			    cons->what = CONS_HERO;
			    cons->list = (genericptr_t) 0;

			    cons->next = b->cons;
			    b->cons = cons;
			}
			if ((btrap = t_at(x,y)) != 0) {
			    struct container *cons = (struct container *)
				alloc(sizeof(struct container));

			    cons->x = x;
			    cons->y = y;
			    cons->what = CONS_TRAP;
			    cons->list = (genericptr_t) btrap;

			    cons->next = b->cons;
			    b->cons = cons;
			}

			levl[x][y] = water_pos;
			block_point(x,y);
		    }
	}

	/*
	 * Every second time traverse down.  This is because otherwise
	 * all the junk that changes owners when bubbles overlap
	 * would eventually end up in the last bubble in the chain.
	 */

	up = !up;
	for (b = up ? bbubbles : ebubbles; b; b = up ? b->next : b->prev) {
		register int rx = rn2(3), ry = rn2(3);

		mv_bubble(b,b->dx + 1 - (!b->dx ? rx : (rx ? 1 : 0)),
			    b->dy + 1 - (!b->dy ? ry : (ry ? 1 : 0)),
			    FALSE);
	}

	/* put attached ball&chain back */
	if (Punished) placebc();
	vision_full_recalc = 1;
}

/* when moving in water, possibly (1 in 3) alter the intended destination */
void
water_friction()
{
	register int x, y, dx, dy;
	register boolean eff = FALSE;

	if (Swimming && rn2(4))
		return;		/* natural swimmers have advantage */

	if (u.dx && !rn2(!u.dy ? 3 : 6)) {	/* 1/3 chance or half that */
		/* cancel delta x and choose an arbitrary delta y value */
		x = u.ux;
		do {
		    dy = rn2(3) - 1;		/* -1, 0, 1 */
		    y = u.uy + dy;
		} while (dy && (!isok(x,y) || !is_pool(x,y)));
		u.dx = 0;
		u.dy = dy;
		eff = TRUE;
	} else if (u.dy && !rn2(!u.dx ? 3 : 5)) {	/* 1/3 or 1/5*(5/6) */
		/* cancel delta y and choose an arbitrary delta x value */
		y = u.uy;
		do {
		    dx = rn2(3) - 1;		/* -1 .. 1 */
		    x = u.ux + dx;
		} while (dx && (!isok(x,y) || !is_pool(x,y)));
		u.dy = 0;
		u.dx = dx;
		eff = TRUE;
	}
	if (eff) pline("Water turbulence affects your movements.");
}

void
save_waterlevel(fd, mode)
int fd, mode;
{
	register struct bubble *b;

	if (!Is_waterlevel(&u.uz)) return;

	if (perform_bwrite(mode)) {
	    int n = 0;
	    for (b = bbubbles; b; b = b->next) ++n;
	    bwrite(fd, (genericptr_t)&n, sizeof (int));
	    bwrite(fd, (genericptr_t)&xmin, sizeof (int));
	    bwrite(fd, (genericptr_t)&ymin, sizeof (int));
	    bwrite(fd, (genericptr_t)&xmax, sizeof (int));
	    bwrite(fd, (genericptr_t)&ymax, sizeof (int));
	    for (b = bbubbles; b; b = b->next)
		bwrite(fd, (genericptr_t)b, sizeof (struct bubble));
	}
	if (release_data(mode))
	    unsetup_waterlevel();
}

void
restore_waterlevel(fd)
register int fd;
{
	register struct bubble *b = (struct bubble *)0, *btmp;
	register int i;
	int n;

	if (!Is_waterlevel(&u.uz)) return;

	set_wportal();
	mread(fd,(genericptr_t)&n,sizeof(int));
	mread(fd,(genericptr_t)&xmin,sizeof(int));
	mread(fd,(genericptr_t)&ymin,sizeof(int));
	mread(fd,(genericptr_t)&xmax,sizeof(int));
	mread(fd,(genericptr_t)&ymax,sizeof(int));
	for (i = 0; i < n; i++) {
		btmp = b;
		b = (struct bubble *)alloc(sizeof(struct bubble));
		mread(fd,(genericptr_t)b,sizeof(struct bubble));
		if (bbubbles) {
			btmp->next = b;
			b->prev = btmp;
		} else {
			bbubbles = b;
			b->prev = (struct bubble *)0;
		}
		mv_bubble(b,0,0,TRUE);
	}
	ebubbles = b;
	b->next = (struct bubble *)0;
	was_waterlevel = TRUE;
}

const char *waterbody_name(x, y)
xchar x,y;
{
	register struct rm *lev;
	schar ltyp;

	if (!isok(x,y))
		return "drink";		/* should never happen */
	lev = &levl[x][y];
	ltyp = lev->typ;

	if (is_lava(x,y))
		return "lava";
	else if (ltyp == ICE ||
		 (ltyp == DRAWBRIDGE_UP &&
		  (levl[x][y].drawbridgemask & DB_UNDER) == DB_ICE))
		return "ice";
	else if (((ltyp != POOL) && (ltyp != WATER) &&
	  !Is_medusa_level(&u.uz) && !Is_waterlevel(&u.uz) && !Is_juiblex_level(&u.uz)) ||
	   (ltyp == DRAWBRIDGE_UP && (levl[x][y].drawbridgemask & DB_UNDER) == DB_MOAT))
		return "moat";
	else if ((ltyp != POOL) && (ltyp != WATER) && Is_juiblex_level(&u.uz))
		return "swamp";
	else if (ltyp == POOL)
		return "pool of water";
	else return "water";
}

STATIC_OVL void
set_wportal()
{
	/* there better be only one magic portal on water level... */
	for (wportal = ftrap; wportal; wportal = wportal->ntrap)
		if (wportal->ttyp == MAGIC_PORTAL) return;
	impossible("set_wportal(): no portal!");
}

STATIC_OVL void
setup_waterlevel()
{
	register int x, y;
	register int xskip, yskip;

	/* ouch, hardcoded... */

	xmin = 3;
	ymin = 1;
	xmax = 78;
	ymax = 20;

	/* set hero's memory to water */

	for (x = xmin; x <= xmax; x++)
		for (y = ymin; y <= ymax; y++)
			clear_memory_glyph(x, y, S_water);

	/* make bubbles */

	xskip = 10 + rn2(10);
	yskip = 4 + rn2(4);
	for (x = bxmin; x <= bxmax; x += xskip)
		for (y = bymin; y <= bymax; y += yskip)
			mk_bubble(x,y,rn2(7));
}

STATIC_OVL void
unsetup_waterlevel()
{
	register struct bubble *b, *bb;

	/* free bubbles */

	for (b = bbubbles; b; b = bb) {
		bb = b->next;
		free((genericptr_t)b);
	}
	bbubbles = ebubbles = (struct bubble *)0;
}

STATIC_OVL void
mk_bubble(x,y,n)
register int x, y, n;
{
	/*
	 * These bit masks make visually pleasing bubbles on a normal aspect
	 * 25x80 terminal, which naturally results in them being mathematically
	 * anything but symmetric.  For this reason they cannot be computed
	 * in situ, either.  The first two elements tell the dimensions of
	 * the bubble's bounding box.
	 */
	static uchar
		bm2[] = {2,1,0x3},
		bm3[] = {3,2,0x7,0x7},
		bm4[] = {4,3,0x6,0xf,0x6},
		bm5[] = {5,3,0xe,0x1f,0xe},
		bm6[] = {6,4,0x1e,0x3f,0x3f,0x1e},
		bm7[] = {7,4,0x3e,0x7f,0x7f,0x3e},
		bm8[] = {8,4,0x7e,0xff,0xff,0x7e},
		*bmask[] = {bm2,bm3,bm4,bm5,bm6,bm7,bm8};

	register struct bubble *b;

	if (x >= bxmax || y >= bymax) return;
	if (n >= SIZE(bmask)) {
		impossible("n too large (mk_bubble)");
		n = SIZE(bmask) - 1;
	}
	b = (struct bubble *)alloc(sizeof(struct bubble));
	if ((x + (int) bmask[n][0] - 1) > bxmax) x = bxmax - bmask[n][0] + 1;
	if ((y + (int) bmask[n][1] - 1) > bymax) y = bymax - bmask[n][1] + 1;
	b->x = x;
	b->y = y;
	b->dx = 1 - rn2(3);
	b->dy = 1 - rn2(3);
	b->bm = bmask[n];
	b->cons = 0;
	if (!bbubbles) bbubbles = b;
	if (ebubbles) {
		ebubbles->next = b;
		b->prev = ebubbles;
	}
	else
		b->prev = (struct bubble *)0;
	b->next =  (struct bubble *)0;
	ebubbles = b;
	mv_bubble(b,0,0,TRUE);
}

/*
 * The player, the portal and all other objects and monsters
 * float along with their associated bubbles.  Bubbles may overlap
 * freely, and the contents may get associated with other bubbles in
 * the process.  Bubbles are "sticky", meaning that if the player is
 * in the immediate neighborhood of one, he/she may get sucked inside.
 * This property also makes leaving a bubble slightly difficult.
 */
STATIC_OVL void
mv_bubble(b,dx,dy,ini)
register struct bubble *b;
register int dx, dy;
register boolean ini;
{
	register int x, y, i, j, colli = 0;
	struct container *cons, *ctemp;

	/* move bubble */
	if (dx < -1 || dx > 1 || dy < -1 || dy > 1) {
	    /* pline("mv_bubble: dx = %d, dy = %d", dx, dy); */
	    dx = sgn(dx);
	    dy = sgn(dy);
	}

	/*
	 * collision with level borders?
	 *	1 = horizontal border, 2 = vertical, 3 = corner
	 */
	if (b->x <= bxmin) colli |= 2;
	if (b->y <= bymin) colli |= 1;
	if ((int) (b->x + b->bm[0] - 1) >= bxmax) colli |= 2;
	if ((int) (b->y + b->bm[1] - 1) >= bymax) colli |= 1;

	if (b->x < bxmin) {
	    pline("bubble xmin: x = %d, xmin = %d", b->x, bxmin);
	    b->x = bxmin;
	}
	if (b->y < bymin) {
	    pline("bubble ymin: y = %d, ymin = %d", b->y, bymin);
	    b->y = bymin;
	}
	if ((int) (b->x + b->bm[0] - 1) > bxmax) {
	    pline("bubble xmax: x = %d, xmax = %d",
			b->x + b->bm[0] - 1, bxmax);
	    b->x = bxmax - b->bm[0] + 1;
	}
	if ((int) (b->y + b->bm[1] - 1) > bymax) {
	    pline("bubble ymax: y = %d, ymax = %d",
			b->y + b->bm[1] - 1, bymax);
	    b->y = bymax - b->bm[1] + 1;
	}

	/* bounce if we're trying to move off the border */
	if (b->x == bxmin && dx < 0) dx = -dx;
	if (b->x + b->bm[0] - 1 == bxmax && dx > 0) dx = -dx;
	if (b->y == bymin && dy < 0) dy = -dy;
	if (b->y + b->bm[1] - 1 == bymax && dy > 0) dy = -dy;

	b->x += dx;
	b->y += dy;

	/* void positions inside bubble */

	for (i = 0, x = b->x; i < (int) b->bm[0]; i++, x++)
	    for (j = 0, y = b->y; j < (int) b->bm[1]; j++, y++)
		if (b->bm[j + 2] & (1 << i)) {
		    levl[x][y].typ = AIR;
		    levl[x][y].lit = 1;
		    unblock_point(x,y);
		}

	/* replace contents of bubble */
	for (cons = b->cons; cons; cons = ctemp) {
	    ctemp = cons->next;
	    cons->x += dx;
	    cons->y += dy;

	    switch(cons->what) {
		case CONS_OBJ: {
		    struct obj *olist, *otmp;

		    for (olist=(struct obj *)cons->list; olist; olist=otmp) {
			otmp = olist->nexthere;
			place_object(olist, cons->x, cons->y);
		    }
		    break;
		}

		case CONS_MON: {
		    struct monst *mon = (struct monst *) cons->list;
		    (void) mnearto(mon, cons->x, cons->y, TRUE);
		    break;
		}

		case CONS_HERO: {
		    int ux0 = u.ux, uy0 = u.uy;

		    /* change u.ux0 and u.uy0? */
		    u.ux = cons->x;
		    u.uy = cons->y;
		    newsym(ux0, uy0);	/* clean up old position */

		    if (MON_AT(cons->x, cons->y)) {
				mnexto(m_at(cons->x,cons->y));
			}
                        /* WAC removed this.  The ball and chain is moved
                         * as a CONS_OBJECT by the bubble
                         */
#if 0
                    if (Punished) placebc();    /* do this for now */
#endif
		    break;
		}

		case CONS_TRAP: {
		    struct trap *btrap = (struct trap *) cons->list;
		    btrap->tx = cons->x;
		    btrap->ty = cons->y;
		    break;
		}

		default:
		    impossible("mv_bubble: unknown bubble contents");
		    break;
	    }
	    free((genericptr_t)cons);
	}
	b->cons = 0;

	/* boing? */

	switch (colli) {
	    case 1: b->dy = -b->dy;	break;
	    case 3: b->dy = -b->dy;	/* fall through */
	    case 2: b->dx = -b->dx;	break;
	    default:
		/* sometimes alter direction for fun anyway
		   (higher probability for stationary bubbles) */
		if (!ini && ((b->dx || b->dy) ? !rn2(20) : !rn2(5))) {
			b->dx = 1 - rn2(3);
			b->dy = 1 - rn2(3);
		}
	}
}

/*mkmaze.c*/

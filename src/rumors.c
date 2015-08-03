/*	SCCS Id: @(#)rumors.c	3.4	1996/04/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"
#include "dlb.h"

/*	[note: this comment is fairly old, but still accurate for 3.1]
 * Rumors have been entirely rewritten to speed up the access.  This is
 * essential when working from floppies.  Using fseek() the way that's done
 * here means rumors following longer rumors are output more often than those
 * following shorter rumors.  Also, you may see the same rumor more than once
 * in a particular game (although the odds are highly against it), but
 * this also happens with real fortune cookies.  -dgk
 */

/*	3.1
 * The rumors file consists of a "do not edit" line, a hexadecimal number
 * giving the number of bytes of useful/true rumors, followed by those
 * true rumors (one per line), followed by the useless/false/misleading/cute
 * rumors (also one per line).  Number of bytes of untrue rumors is derived
 * via fseek(EOF)+ftell().
 *
 * The oracles file consists of a "do not edit" comment, a decimal count N
 * and set of N+1 hexadecimal fseek offsets, followed by N multiple-line
 * records, separated by "---" lines.  The first oracle is a special case,
 * and placed there by 'makedefs'.
 */

STATIC_DCL void FDECL(init_rumors, (dlb *));
STATIC_DCL void FDECL(init_oracles, (dlb *));

static long true_rumor_start,  true_rumor_size,  true_rumor_end,
	    false_rumor_start, false_rumor_size, false_rumor_end;
static int oracle_flg = 0;  /* -1=>don't use, 0=>need init, 1=>init done */
static unsigned oracle_cnt = 0;
static long *oracle_loc = 0;

static const char *random_mesgX[] = {
	"Elbereth",
	/* trap engravings */
	"Vlad was here", "ad aerarium",
	/* take-offs and other famous engravings */
	"Owlbreath", "Galadriel",
	"Kilroy was here",
	"A.S. ->", "<- A.S.", /* Journey to the Center of the Earth */
	"You won't get it up the steps", /* Adventure */
	"Lasciate ogni speranza o voi ch'entrate.", /* Inferno */
	"Well Come", /* Prisoner */
	"We apologize for the inconvenience.", /* So Long... */
	"See you next Wednesday", /* Thriller */
	"notary sojak", /* Smokey Stover */
	"For a good time call 8?7-5309",
	"Please don't feed the animals.", /* Various zoos around the world */
	"Madam, in Eden, I'm Adam.", /* A palindrome */
	"Two thumbs up!", /* Siskel & Ebert */
	"Hello, World!", /* The First C Program */
	"You've got mail!", /* AOL */
	"May the force be with you!",
	"As if!", /* Clueless */
	/* [Tom] added these */
	"Y?v?l s??ks!", /* just kidding... */
	"T?m ?as h?r?",
	/* Tsanth added these */
	"Gazortenplatz", /* Tribute to David Fizz */
	"John 3:16", /* You see this everywhere; why not here? */
	"Exhale! Exhale! Exhale!", /* Prodigy */
	"All you need is love.", /* The Beatles */
	"Please don't feed the animals.", /* Various zoos around the world */
	"....TCELES B HSUP   A magic spell?", /* Final Fantasy I (US) */
	"Madam, in Eden, I'm Adam.", /* A palindrome */
	"Two thumbs up!", /* Siskel & Ebert */
	"Hello, World!", /* The First Program line */
	"Turn around.", /* Various people at various times in history */
	"You've got mail!", /* AOL */
	"UY WUZ HERE", /* :] */
	"Time flies when you're having fun.", /* Who said this first, anyway? */
	"As if!", /* Clueless */
	"How bizarre, how bizarre.", /* OMC */
	"Silly rabbit, Trix are for kids!", /* Trix */
 	"I'll be back!", /* Terminator */
 	"That is not dead which can eternal lie.", /* HPL */
"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon",
"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon",
"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon cafe",
"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon cafe",
"**FR33 P0T10N 0F FULL H34L1NG**",
"An HTML attachment was scrubbed...",
"Are YOU tired of the same old messages? Go to http://alt.org/nethack/addmsgs/ now!",
"Badger badger badger badger badger badger MUSHROOM MUSHROOM!",
"CONGRATULATIONS! you are now the proud owner of 40 newts.",
"Christmas Greetings!",
"Crush the enemy, see them driven before you, and hear the lamentations of their women!",
"Dear $playername, how do you spell Elbereth? Singed- A friend",
"Dear $playername, your $fruit is in another castle.",
"Dear Dudley, what would you say are your greatest weaknesses?",
"Do you want to change that garter snake into a raging python?",
"Do you want to start? Take the 5:45 Shibuya bound train from Jiyuugaoka Station",
"Engraving Moloch works as engraving Elbereth. Just see how that a turns to flee from here!",
"Find a small token to represent your Owlbear",
"HOT inside stock tip - Yendor Industries (YNDR) STRONG BUY!",
"Has anyone really been far even as decided to use even go want to do look more like?",
"Have you heard of a spell called Explosive Runes?",
"Hello.",
"How are you gentlemen?",
"Spam Hummmmmmmmmmmmmmmmmmmmmmm...",
"I am Nigerian prince HORRE YED MOBUTU contacting you regarding the transfer of $10000000 zorkmids",
"I've had it with you $playername !",
"IM in ur base, killing ur doods!!!111",
"LOL ROFL LMAO",
"Lolol!",
"MAKE ZORKMIDS FAST, GUARANTEED!",
"Made you look!",
"Magic-B-Here sells wands of not exactly finest quality! Bye today!",
"Make a contract with me to become a puella magi!",
"Me, too!",
"PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS PENIS",
"Play FREE roguelike game - http://crawl.akrasiac.org/",
"Please return to sender (Asmodeus).",
"Please send 15300 zorkmids security deposit to help me move 11 million (Z) out of Ludios!",
"Pole Arm Wax! Only 350 Zk.",
"Potion of generic C1al1s, only 10 zorkmids, satisfaction guaranteed!",
"Red in the morning, sailor's warning.",
"SEVEN DAYS",
"Sale @ Izchak's! The first five visitors will get a free lamp!",
"Scroll of punishment. Apply directly to the forehead.",
"Send $5 to the return address, and get a free subscription to Thumbs magazine!",
"Slash'EM Free Forever, Come Play My Lord *INSERT SUCCUBUS PIC HERE*",
"Sword enlargement scrolls! Base price: 60 zorkmids!",
"System shut down in 1 minute(s)",
"TYPE 111 FOR HOT SUCCUBUS LOVIN'",
"Thanx Maud",
"The WOW starts now - http://www.microsoft.com/windowsvista/",
"The best escort services! Meet the most intelligent succubi and incubi! Call 1-800-HOTDATE!",
"This is Spam Enjoy!",
"This mail complies with the Yendorian Anti-Spam Act (YASA).",
"This message has been bounced. If you recieved this successfully, ignore this.",
"This scroll was made with cockatrice leather. Have a nice day.",
"This space intentionally left blank.",
"Thou needst no longer toil with a mere quarterstaff!",
"Try new magic Scroll! Up to three inches GUARANTEED!",
"Undeliverable mail returned to sender: no such address <rodney@dungeons.doom>",
"WAN7 +0 EN L4GE Y UR PEN???",
"Watch out! The-Gnome-With-The-Wand-Of-Death is behind that door!",
"While your character doesn't need socialising, remember that you do. Don't be a tool.",
"YENDOR EATS CHEEZE WIZZ",
"Yo Momma!",
"You are about to die... so why not get a cheap LIFE INSURANCE?",
"You are the 100 millionth person to read this message! Wait 500 turns for your FREE Iron Chain.",
"You got mail. From AOL!",
"You may already have won $1000!",
"You're doing great! Keep up the good work!",
"You've been pre-approved for the Platinum Yendorian Express card!",
"Your Dad On A Sybian",
"Your porn stash has been located. Have a good day.",
"g3t bl3ss3d skrol1z of 3nch4n7 w34pon ch33p! add inchez to your Sword!",
"http://www.tvtropes.org/",
"qwertyuiopasdfghjklzxcvbnm",
"wow. such meme. so epic. much random.",
"Engrave a wise saying and your name will live forever. -- Anonymous",
"You smite the vampire. It explodes! You seem unaffected by it.",
"dilbert aint gay - tupac",
"$playername sux!",
"+7 Magicbane. Do you want to kill monsters, or do you want to fool around?",
"--More--",
"/join #nethack",
"1/0",
"1337",
"2, 12, 1112, 3112, 132112, 1113122112, ...",
"3ngr@Vr VVu$ }{3@r 3|-|d }{e 1eft $u/V-133tsp3@k!!!!1111!!!oneoneone",
"4, 8, 15, 16, 23, 42",
"666 420 Tongue Kiss Your Father",
"<-- Go that way!",
">:D",
"A - cursed loadstone.",
"A horse is a horse of course...",
"Aaoooooooooooo! Werewolves of Yendor!",
"All work and no play makes Jack a dull boy",
"Allburrith",
"Always Be Closing!",
"At last... the amulet ^n my grasp..... hidden in the sec+et room on th! %3rd level...",
"Bark!",
"Beware the exploding cow!",
"Blessed Greased +5 silver dragon scale mail buried here",
"CAPS LOCK IS CRUISE CONTROL FOR COOL",
"Call Before You Dig",
"Cancel this engraving",
"Chainsaw was here! RAAAAAAAGH!",
"Clothed for inventory",
"Come on, $playername, don't dawdle! You have almost missed the train!",
"Computers: Can't live with 'em, can't play Nethack without 'em.",
"Cool story, bro.",
"Destroy the amulet on this level.",
"Did you know the witch-woman jenka had a brother?",
"Dig Here for a +5 pair of Speed Boots!",
"Dig down!",
"Dig for Victory here",
"Dig here --- X",
"Dig here for Iraq's WMDs",
"Dig here.",
"Do not feed the pink elephant",
"Do not move forward",
"Do not taunt happy fun cockatrice",
"Don't Panic",
"Don't go that way.",
"Don't look behind you.",
"Don't worry. You're still illiterate.",
"Duck!",
"E=mc ... no, that's not it.",
"ERAU QSSI DRLO WEHT",
"ETAOIN SHRDLU",
"Eat at Joe's",
"Elbcreth",
"Elber",
"Elberek? Albereth? Albert?",
"Elberet",
"Elbereth",
"Elbereth",
"Elbereth was engraved here, but it's no longer engraved here. You're too late, loser!",
"Elberith Elbireth Elberth Lbereth Elbreth",
"Elebird",
"Eleburff",
"Elf Breath",
"Engraving with a hammer is hard!",
"Environmentally sensitive area -- no farming",
"Even in your NetHack game, F.O.E.!",
"Find the computer room!",
"First!",
"For a good time call 867-5309",
"For a good time, call 1-900-FOOCUBI!",
"Friendlydads.net friendliest dads on the Net!",
"Gaius Julius Primigenius was here. Why are you late?",
"Galadriel",
"Gandalf for president!",
"Gatorade. Is it in you?",
"George plonks ferrets.",
"Get A BRAIN! MORANS",
"Gilthoniel",
"Go Left -->",
"Go around! Go around and around and around! What fun! I'm so happy!",
"Go see Snakes on a Plane.",
"Got milk?",
"Haermund Hardaxe carved these runes",
"Hastur Hastur H%s+ur",
"He who is valiant and pure of spirit may find the Holy Grail in the Castle of Arrrgh",
"HeadOn! Apply directly to the forehead! HeadOn! Apply directly to the forehead!",
"Hello world!",
"Here be dragons.",
"Here lies Bob; he tried an acid blob",
"Here lies all hopes of a cheedar cheese pie.",
"Here there be soldier ants.",
"Hey, you! Stupid-looking kid! Look down here!",
"Hoc est qui sumus.",
"How much wood could a w**dc***k chuck if a w**dc****k could chuck wood?",
"Htereble",
"I AM ERROR.",
"I am a fish.",
"I bet you're wishing that you had the ability to press ctrl + w",
"I couldn't afford a stone!",
"I love you Stacy, please marry me! AArrghh!",
"I prepared Exploding Runes this morning.",
"I prepared Explosive Runes this morning.",
"I warned you about stairs bro!",
"I wasn't here!",
"I wasn't kidding when I said pay! Now, you'll pay the ultimate price!!",
"I went that way! -->",
"I wonder what's for dinner...",
"I'm George W. Bush, and I approve this message.",
"I'm sorry, but the Amulet of Yendor is in another plane!",
"IT HIURTS",
"Ia! Ia! Cthulhu Fhtagn!",
"If you can read this, you're standing too close",
"If you find silver dragon scale mail on this spot, consider yourself lucky.",
"In Soviet Russia, the amulet ascends with you!",
"It explodes!",
"It hits! It hits! You regret eating that one more quesadilla! You die... --More--",
"It is dark. You may be eaten by a grue.",
"It's a trick. Get an axe.",
"Jesus was here 51 BC",
"Joseph of Arimathea was here!",
"June 5, 2009: I've been wandering these dungeons for days... I want to see my $playername again",
"Keep off the grass!",
"Kilroy was here.",
"Klaatu barada nikto!",
"Klein bottle for sale -- enquire within.",
"Look behind you, a three-headed monkey!",
"Look! Over there!",
"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut...",
"Madam, I'm Adam!",
"Marvin was here.",
"Maxim 1: Pillage, Then Burn",
"Need a light? Come visit the Minetown branch of Izchak's Lighting Store!",
"Need more time? Pray for the Time Goddess! **REQUIRES MONEY**",
"No! Not another new-",
"Nodnol 871 Selim",
"Not Elbereth",
"OH SH-",
"OW! top TREADING ON ME!",
"On this floor is engraved image of young $playername. The $playername is in fetal position.",
"On this floor is masterfully engraved image of : and @. The newt bites the hero. The hero dies...",
"Oo ee oo ah ah ting tang walla walla bing bang",
"Open for inventory",
"Owl Breath",
"Owlbreath",
"Pay Attention!",
"Perception changes with perspective",
"Please insert disk 2 to continue.",
"Poot dispenser here",
"Preeeeeeeeeeeeeow.",
"REDRUM",
"Rainbow Dash is the best pony.",
"Real men play female characters.",
"Rodney was here",
"Rosebud.",
"Run for it! It's a rigged bones file!",
"Save Ferris!",
"Save now, and do your homework!",
"See the TURTLE of enormous girth! On his shell he holds the earth.",
"Snakes on the Astral Plane - Soon in a dungeon near you",
"Sometimes I feel that people can read me like an engraving...",
"Sorry, $playername, but The Amulet of Yendor is on another dungeon level!",
"Stuff to remember: don't kick sinks.",
"The Gnome-with-a-Wand-of-Death was here.",
"The Vibrating Square",
"The Vibrating Square is NOT here",
"The forgotten pet $dog has come! An overpowered vicious dog. Beware it's deadly bite!",
"The grass is always greener on the other side of your sunglasses.",
"The moon rules - #1",
"The preparations are complete. There is no hope for $playername now.",
"The treasure is buried uuddlrlrba from here.",
"There is nothing engraved here. Really.",
"There was a hole here. It's gone now.",
"They say god saved the queen",
"They say that They can never be trusted.",
"They say that the Spider Caves have a wand of death... Oh wait... That was SLASH'EM, wrong game",
"They say that there's a stalker behind you",
"They say that you are what you eat.",
"They say that you can't trust what they say.",
"This engraving is totally unextrordinary. Wait! No! It says - no, it's unextrordinary.",
"This is a pit!",
"This is a pit!",
"This is not the dungeon you are looking for.",
"This message is false.",
"This space intentionally left blank.",
"This square deliberately left blank.",
"This statement is false.",
"This tile tells you that you need at least AC -15 to clear the castle.",
"This way for egress.",
"Toy Story 2 was OK.",
"Try ?? , : :: /f !, You",
"Twilight Sparkle has bacon hair.",
"Unicorn skin tastes likes Jesus.",
"Vlad wasn't here",
"W.A.S.T.E.",
"WARNING! Do not read this message.",
"WARNING: CONSTRUCTION AHEAD.",
"WOT, NO GYPSIES?",
"WTF",
"Warning, Exploding runes!",
"Watch out! The-Gnome-With-The-Wand-Of-Death is behind that door!",
"We are not amused.",
"Where are the ats of yesteryear?",
"Who is this foreign host and why does he keep closing my sessions?",
"Why are you looking down here?",
"Why are you reading this?",
"Why's everybody always picking on me?",
"X marks the spot",
"X marks the spot.",
"Yeah, well, y'know, that's just like, your opinion, man.",
"You are already dead!",
"You are here.",
"You are here.",
"You aren't going to ascend by staring at your shoes.",
"You can't help being ugly, but you could stay home.",
"You die...",
"You die...",
"You fall into a bottomless pit. You die...",
"You step on a polymorph trap!",
"Your Dad On A Sybian",
"http://www.tvtropes.org/",
"lol",
"mde.tv",
"mexican jumping beans",
"rerecord don't fade away",
"romanes eunt domus",
"romanes eunt domus",
"shawnz sux.",
"you are the one millionth visitor on this place! please wait 200 turns for a wand of wishing.",
	/* From NAO */
	"Arooo!  Werewolves of Yendor!", /* gang tag */
	"Dig for Victory here", /* pun, duh */
	"Don't go this way",
	"Gaius Julius Primigenius was here.  Why are you late?", /* pompeii */
	"Go left --->", "<--- Go right",
	"Haermund Hardaxe carved these runes", /* viking graffiti */
	"Here be dragons",
	"Need a light?  Come visit the Minetown branch of Izchak's Lighting Store!",
	"Save now, and do your homework!",
	"Snakes on the Astral Plane - Soon in a dungeon near you!",
	"There was a hole here.  It's gone now.",
	"The Vibrating Square",
	"This is a pit!",
	"This is not the dungeon you are looking for.",
	"This square deliberately left blank.",
	"Warning, Exploding runes!",
	"Watch out, there's a gnome with a wand of death behind that door!",
	"X marks the spot",
	"X <--- You are here.",
	"You are the one millionth visitor to this place!  Please wait 200 turns for your wand of wishing.",

	/* http://www.alt.org/nethack/addmsgs/viewmsgs.php */
	"Badger badger badger badger badger badger MUSHROOM MUSHROOM!",
	"Dig here for a +5 pair of Speed Boots!",
	"If you find speed boots on this spot, consider yourself lucky.",
	"Gandalf for president!",
	"Fnord",
	"\"Engrave a wise saying and your name will live forever.\" -- Anonymous",
	"In Soviet Russia, the amulet ascends with you!",
	"Need a light? Come visit the Minetown branch of Izchak's Lighting Store!",
	"The Vibrating Square is NOT here",

	/* From UnNetHack */
	"She's watching you!", /* Portal */
	"The cake is a lie!", /* Portal */
	"cjohnson/tier3", /* Portal */
	"What's the sitch?", /* Kim Possible */
	"So not the drama.", /* Kim Possible */
	"Sudden decompression sucks!", /* Space Quest 3 death message */
	"Thanks for playing UnNetHackPlus.  As usual, you've been a real pantload!", /* Space Quest 4 death message, paraphrased */
	"Colorless green ideas sleep furiously.", /* Noam Chomsky */
	"Time flies like an arrow.", /* linguistic humor */
	"All Your Base Are Belong To Us!", /* Zero Wing */
	"Numa Numa", /* Internet Meme */
	"I want to believe!", /* X-Files */
	"Trust No One", /* X-Files */
	"The Truth is Out There", /* X-Files */
	"Look behind you, a Three-Headed Monkey!", /* Monkey Island */
	"Ifnkovhgroghprm", /* Kings Quest I */
	"Cookies are delicious delicacies", /* Mozilla Firefox */

	/* From UnNetHackPlus */
	"Losing is fun!", /* Dwarf Fortress */
	/* Murphy's laws */
	"Cheer up, the worst is yet to come...",
	"Computers let you waste time efficiently",
	"If something cannot go wrong at all, it will go wrong in a very spectacular way.",
	"If everything seems to be going well, you have obviously overlooked something",
	"If anything can go wrong, it will",
	"When things go from bad to worse, the cycle repeats.",
	"Being dead right, won't make you any less dead.",
	"If all else fails, hit it with a big hammer.",
	"If many things can go wrong, they will all go wrong at the same time.",
	/* end of Murphy's laws */
	"That is not dead which can eternal lie, and with strange aeons even death may die.", /* H. P. Lovecraft */
	"Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn.", /* H. P. Lovecraft */
	"Conducts are about player skill, not about obsessive-compulsive disorder", /* TJR on RGRN */
	"That is not dead which can eternal lie, and with strange aeons even vanilla might get released.", /* bhaak on http://nhqdb.alt.org/?301 */
	/* http://donjon.bin.sh/fantasy/random/#dungeon_graffiti */
	"When song becomes silence, the Wand of Aether shall be destroyed",
	"Upon the Night of Dweomers, when iron is made flesh and the Golden Scepter is wreathed in flames, the Copper Tower shall be restored",
	"Run away!",
	"<--",
	"-->",
"This door leads to the women's locker room. Use a wand of make invisible on yourself before entering.",
"You stand at the entrance to XXX SEX SHOP!!!",
"Draw it! Draw it!",
"Pong begin broadcasting!",
"TEAM JACOB MOTHERFUCKERS. Gotta banish that douchebag Edward to Gehennom for all eternity.",
"The effect of spray disappears.",
"There is the tree with fruit!",
"The sound is loud, water aboves fall!",
"The door is locked. Can't be opened.",
"You win? Very good. Now I believe the girls is good too.",
"You ride a bike to advertise?",
"A bright bike!",
"Very lovrly!",
"My son collect too, understand the monster, Zhenqian says so. The aid center of monster just called me.",
"It's by my brother just now I wanted to tell you his tel number!",
"The left passages do not have the ticket, how about them?",
"Troubled you? Xiaojin comes welcome to take again!",
"It is nothing the olden-tortoise water-pot is belonded to you.",
"This time you will take it to the center of monster to be treated.",
"Welcome! This is Xiaojin Store!",
"There are sorts of things no if the game shop in this city don't go out to exchange.",
"Ground floor is service counter for free.",
"The second is a market store.",
"The fourth is a drugstore.",
"The fifth is the skill machine counter.",
"The sixth is a square.",
"The garret is watching tower.",
"You can register ten with pocket gearing.",
"Do you turn to the 'lucky channel' programme?",
"You are very good!",
"The coins increasing,and laughing endless!",
"Welcome! Play the game, put the coin into there, exchange the goods!",
"Machine is interesting compared with worm machine.",
"This is a life games rotate quickly.",
"Card machine, you can imagine easily!",
"The cup to drink is there, it has a sweet taste!",
"Welcome to the trade center here, the people who are stranger each other, exchange monaster casually.",
"Does the monaster owned each other to be exchange now?",
"It needs times to find the exchanged object, come after a while!",
"You sit on the cockatrice corpse. It is not very comfortable.",
"The mystic egg is prepared for you specially send it at once there are many rooms in the center.",
"Make preparations now wait for a moment please!",
"Before finding the news wrote a report please!",
"Do you know the news?",
"The data of the news is wrong - see you next!",
"Then the monster you want will let her send it out. If everybody do so, we can communicate with various of people.",
"It's a female cat called Anxiong whose name is as same as father's.",
"The palace is writed how to use the radio.",
"We will realize the wish of the coachs all over the world. Let's work hard!",
"Do you pay homage to the spirit of the forest? The forest spirit surpasses space and time to protect the forest.",
"She is the grass monster originally.",
"It looks like the picture on the relic wall, if it is like as a monster, according to the reason there are many.",
"The front.... En?What? You use the secret sign!",
"I like to think of the question about the relic.",
"This relic built 1500 years ago! But,who had built it.",
"The libyan relic!",
"The graduate school of libyan relic the question about relic!",
"It is made patterning foregone monsters.",
"This is the appearance drawn on the wall!",
"There is a big hole on the wall!",
"Welcome to relic small house! There are the appearance of monsters drawn remote time ago. Move it to look please!",
"There is nothing just before you come there to look at the wall!",
"Under your help to our investigation about the relic, we have a father understanding.",
"The thing made patterning foregone monsters.",
"A big room but nothing!",
"A holy appearance, it is a mystic place!",
"Good! The alteration for monsters badge is achieved! Supplement a unknown badge.",
"Under your help to investigate we have known the relic, as if the relic was built for the monsters to inhabit!",
"This is the first superintendent for libya graduate school the photo of Namuluoji Dr!",
"Many profound books. 'Archaian relic' 'Riddle of archaian civilization'",
"That is the soul of coach!",
"It is yedong's tail to be cut off! There are some mails,do you want to read?",
/* contributed by members of NetHackWiki */
	"Please disable the following security protocols: 2-18, 1-12, 1-0", /* the Submachine series */
		"Our coil is broken, and we don't have a replacement. It's useless. It's so useless.", 
	
/* contributed by members of the Nethack Usenet group, rec.games.roguelike.nethack */
	"Which way did he go, George, which way did he go?",
	"Until you stalk and overrun, you can't devour anyone.",
	"Fee.  Fie.  Foe.  Foo.",
	"Violence is the last refuge of the incompetent.",
	"Be vewy vewy quiet.  I'm hunting wabbits.",
	"Eh, what's up, doc?",
	"We prefer your extinction to the loss of our job.",
	"You're weird, sir.",
	"I do not know for what reason the enemy is pursuing you, but I perceive that he is, strange indeed though that seems to me.",
	"When it is falling on your head, then you are knowing it is a rock.",
	"The very best mail is made from corbomite.",
	"Woah, duude, look at all the colors.",
	"I'm bringing home a baby bumblebee.",
	"Now, if you'll permit me, I'll continue my character assassination unimpeded.",
	"Cry havoc, and let slip the dogs of war.",
	"Virtue needs some cheaper thrills.",
	"I just saw Absolom hanging in an oak tree!",
	"And I may do anything I wish as long as I squeeze?",
	"Marsey Dotesin Dosey Dotesin Diddley Damsey Divey.",
	"I knew I should have taken that left turn at Albuquerque.",
	"YOU BROKE MY SWORD!",
	"An atom-blaster is a good weapon, but it can point both ways.",
	"You may have already won ten million zorkmids.",
	"They say the satisfaction of teaching makes up for the lousy pay.",
	"Tharr she blows!",
	"Perhaps today is a good day to die.",
	"Sufferin' succotash.",
	"To succeed, planning alone is insufficient. One must improvise as well.",
	"I love it when a plan comes together.",
	"Very fond of rumpots, crackpots, and how are you Mr. Wilson?",
	"No matter how valuable it might be, any man's life is still worth more than any animal's.",
	"No clock for this.  Flyin' now.",
	"Of course, you realize, this means war.",
	"She's fast enough for you, old man. What's the cargo?",
	"There're few who haven't heard the name of Gurney Halleck.",
	"Do you like mayonnaise and corn on your pizza?",
	"I am that merry wanderer of the night.",
	"Don't try any games.  Remember I can see your face if you can't see mine.",
	"Ah hates rabbits.",
	"Captain, there be whales here!",
	"Mimesis is a plant.  Go Bucks!",
	"That's all we live for, isn't it?  For pleasure, for titillation?",
	"The only brew for the brave and true comes from the Green Dragon.",
	"There is an old Vulcan proverb: only Nixon could go to China.",
	"Yo, put me in a trebuchet and launch me to the stratosphere.",
	"Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua.",
	"Sola lingua bona lingua mortua est.",
	"Mairzy Doats And Dozy Doats And Liddle Lamzy Divey A kiddley divey too, wouldn't you?",
	"Madam, I may be drunk, but you are ugly, and in the morning I shall be sober.",
	"Outside of a dog, a book is a man's best friend.",
	"Inside of a dog, it's too dark to read." ,
	"You are superior in only one aspect. You are better at dying.",
	"No McFly ever amounted to anything in the history of Hill Valley!",
	"You've got to ask yourself one question: 'Do I feel lucky?' Well, do ya, punk?",
	"In this pearl it is hard to see a Strange Loop.",
	"That is because the Strange Loop is buried in the oyster -- the proof.",
	"I love the smell of napalm in the morning.",
	"Et tu, Brute?",
	"The conditions were difficult. Several contestants were maimed, but... I was triumphant. I won Champion Standing.",
	"We're not gonna take it. No, we ain't gonna take it. We're not gonna take it anymore.",
	"Absurd!  Your 'MU' is as silly as a cow's moo.",
	"Hasta la vista, baby.",
	"I see...  I see...  I see a lavender man...  bearing tidings.",
	"It is a sign of creative genius to reconcile the seemingly disparate.",
	"It pays to be obvious, especially if you have a reputation for subtlety.",
	"Bother. I've left my new torch in Narnia",
	"If you put into the machine wrong figures, will the right answers come out?",
	"I am not able rightly to apprehend the kind of confusion of ideas that could provoke such a question.",
	"Colorless green ideas sleep furiously",
	"We fear a few dwarf watercraft stewardesses were regarded as desegregated after a great Texas tweezer war.",
	"I'll kill him in Honolulu, you unhook my pink kimono.",
	"The clown can stay, but the Ferengi in the gorilla suit has got to go.",
	"All roads lead to Trantor, and that is where all stars end.",
	"It is a far, far better thing I do than I have ever done. It is a far, far better rest I go to than I have ever known.",
	"Yes, this is Network 23. The net-net-net-net-network that puts it right... where they want you to be!",
	"You're listening to the E.I.B Radio Network.",
	"We shall this day light such a candle, by God's grace, in England, as I trust shall never be put out.",
	"How about a little fire, scarecrow?",
	"And if you're the kind of person who parties with a bathtub full of pasta, I suspect you don't care much about cholesterol anyway.",
	"I AM A SEXY SHOELESS GOD OF WAR!!!",
	
	"Bad Wolf", /* Dr. Who */
	"It's only the end if you want it to be.", /*Batgirl*/
	"Mistakes were made.",
	"But at what cost?",
	"For we wrestle not against flesh and blood, but against principalities and powers.", /* Ephesians 6:12 */
	"Life is truth, and never a dream...", /* Persona 4 */
	"fire, walk with me.", /*Twin Peaks*/
	"redefine happiness", /* Blue Oyster Cult */
	"no more sadness, no more anger, no more envy...", /*Halo*/
	"Even false things are true",	"FIVE TONS OF FLAX!",	"Fnord", /*Discordianism*/
	"Ash nazg durbatuluk, ash nazg gimbatul, ash nazg thrakatuluk, agh burzum-ishi krimpatul", /*the Lord of the Rings*/
	"No Fate", /*Terminator*/
	"This world can have but one Emperor.", /*FF II*/
	"We shall devour your light, and use it to return this world to the Void!", /*FFIII*/
	"My hatred will not be stanched until it has consumed all else!", /*FFV*/
	"Life... dreams... hope... Where do they come from? And where do they go...?", /*FFVI*/
	"I will NEVER be a memory.", /*FFVII Advent Children*/
	"All the world will be your enemy, Prince With A Thousand Enemies...", /*Watership Down*/
		"...and when they catch you they will kill you... but first they must catch you.",
	"This statement is false.", /*Logical contradiction*/
	"Quod Erat Demonstrandum",	"anno Domini",	"per centum", /* QED, AD, percent */
	"I was so surprised, I CLASSIFIED INFORMATION, I really did!", /*Haruhi Suzumiya*/
	"[REDACTED]",	"[DATA EXPUNGED]",	"[DATA PLUNGED]",	"[DATA EXPANDED]",	"I am a toaster!", /* SCP Foundation */
	"I prepared Explosive Runes this morning.", /*Order of the Stick*/
		"In any battle, there's always a level of force against which no tactics can succeed.",
	"1002: He is Not that Which He Says He Is.", /*Good Omens*/
		"3001: Behinde the Eagle's Neste a grate Ash hath fellen.",
	"VI. You Shall Not Subject Your God To Market Forces!",	/*Discworld*/
		"Nae quin! Nae laird! Nae master! We willna be fooled again!",
		"Morituri Nolumus Mori",
		"End-of-the-World Switch. PLEASE DO NOT TOUCH",
		"Give a man a fire and he's warm for a day, but set fire to him and he's warm for the rest of his life.",
		"Do not, under any circumstances, open this door",
		"WORDS IN THE HEART CANNOT BE TAKEN",
		"Slab: Jus' say \"AarrghaarrghpleeassennononoUGH\"",
		"Ahahahahaha! Ahahahaha! Aahahaha!\nBEWARE!!!!!\nYrs sincerely\nThe Opera Ghost",
		"Bee There Orr Bee A Rectangular Thyng",
	"NOT A Secret Base", /*Gunnerkrigg Court*/
	"Short to long term memory impaired. Go to Robotics Building. Explain about Bowman's architecture. Write down everything.", /*Freefall*/
	"I SAW THE EYE. Over the horizon, like a rising sun!", /* Dresden Codak */
		"That's Science, isn't it? The perpetual horizon.\nBut we found a way, haha, we found a way around that!",
		"What would YOU give up to... to see it through?",
	"Don't trust the skull.", /* Cynarfpncr: Gbezrag (mild spoiler) */
	"We do not threaten or warn or advise. We tell the truth.", /* The Mansion of E */
		"What you stole is even more dangerous and precious than you know.",
		"In the end, you will fly away and live forever, and you will leave the world shattered in your wake.",
		"You will return to where you started, and you will become what you hate.",
		"And in doing so, you will restore the world to what it was.",
	"Fall in a more hardboiled manner.", /* MS Paint Adventures */
		"Since you are reading this, chances are you have already installed this game on your computer.\nIf this is true, you have just participated in bringing about the end of the world.\nBut don't beat yourself up about it.",
		"YOU HATE TIME TRAVEL YOU HATE TIME TRAVEL YOU HATE....",
		"It begins to dawn on you that everything you just did may have been a colossal waste of time.",
	"We will BUILD heroes!", /* THE PROTOMEN */
		"If you replace the working parts, you get a different machine.",
		"If you destroy the working parts, what you get is a broken machine.",
	"Never tell me the odds!" /* Starwars */,
	"Needle-like the new log to know that SlashEM Extended are you ...",
	"http://nethackwiki.com/wiki/Slash%27EM_Extended I will not cease to poison by carbon monoxide",
	"after opportunity to kill Cthulhu Geenome lightsaber, playing for the Khajiit-transsexual talk about that in other games is the freedom to choose make me laugh only",
	"Yes, we'll see a list of races and added roles such trash",
};

STATIC_OVL void
init_rumors(fp)
dlb *fp;
{
	char line[BUFSZ];

	(void) dlb_fgets(line, sizeof line, fp); /* skip "don't edit" comment */
	(void) dlb_fgets(line, sizeof line, fp);
	if (sscanf(line, "%6lx\n", &true_rumor_size) == 1 &&
	    true_rumor_size > 0L) {
	    (void) dlb_fseek(fp, 0L, SEEK_CUR);
	    true_rumor_start  = dlb_ftell(fp);
	    true_rumor_end    = true_rumor_start + true_rumor_size;
	    (void) dlb_fseek(fp, 0L, SEEK_END);
	    false_rumor_end   = dlb_ftell(fp);
	    false_rumor_start = true_rumor_end;	/* ok, so it's redundant... */
	    false_rumor_size  = false_rumor_end - false_rumor_start;
	} else
	    true_rumor_size = -1L;	/* init failed */
}

/* exclude_cookie is a hack used because we sometimes want to get rumors in a
 * context where messages such as "You swallowed the fortune!" that refer to
 * cookies should not appear.  This has no effect for true rumors since none
 * of them contain such references anyway.
 */
char *
getrumor(truth, rumor_buf, exclude_cookie)
int truth; /* 1=true, -1=false, 0=either */
char *rumor_buf;
boolean exclude_cookie; 
{
	dlb	*rumors;
	long tidbit, beginning;
	char	*endp, line[BUFSZ], xbuf[BUFSZ];

	rumor_buf[0] = '\0';
	if (true_rumor_size < 0L)	/* we couldn't open NH_RUMORFILE */
		return rumor_buf;

	rumors = dlb_fopen_area(NH_RUMORAREA, NH_RUMORFILE, "r");

	if (rumors) {
	    int count = 0;
	    int adjtruth;

	    do {
		rumor_buf[0] = '\0';
		if (true_rumor_size == 0L) {	/* if this is 1st outrumor() */
		    init_rumors(rumors);
		    if (true_rumor_size < 0L) {	/* init failed */
			Sprintf(rumor_buf, "Error reading \"%.80s\".",
				NH_RUMORFILE);
			return rumor_buf;
		    }
		}
		/*
		 *	input:      1    0   -1
		 *	 rn2 \ +1  2=T  1=T  0=F
		 *	 adj./ +0  1=T  0=F -1=F
		 */
		switch (adjtruth = truth + rn2(2)) {
		  case  2:	/*(might let a bogus input arg sneak thru)*/
		  case  1:  beginning = true_rumor_start;
			    tidbit = Rand() % true_rumor_size;
			break;
		  case  0:	/* once here, 0 => false rather than "either"*/
		  case -1:  beginning = false_rumor_start;
			    tidbit = Rand() % false_rumor_size;
			break;
		  default:
			    impossible("strange truth value for rumor");
			return strcpy(rumor_buf, "Uh-oh...");
		}
		(void) dlb_fseek(rumors, beginning + tidbit, SEEK_SET);
		(void) dlb_fgets(line, sizeof line, rumors);
		if (!dlb_fgets(line, sizeof line, rumors) ||
		    (adjtruth > 0 && dlb_ftell(rumors) > true_rumor_end)) {
			/* reached end of rumors -- go back to beginning */
			(void) dlb_fseek(rumors, beginning, SEEK_SET);
			(void) dlb_fgets(line, sizeof line, rumors);
		}
		if ((endp = index(line, '\n')) != 0) *endp = 0;
		Strcat(rumor_buf, xcrypt(line, xbuf));
	    } while(count++ < 50 && exclude_cookie && (strstri(rumor_buf, "fortune") || strstri(rumor_buf, "pity")));
	    (void) dlb_fclose(rumors);
	    if (count >= 50)
		impossible("Can't find non-cookie rumor?");
	    else
		exercise(A_WIS, (adjtruth > 0));
	} else {
		pline("Can't open rumors file!");
		true_rumor_size = -1;	/* don't try to open it again */
	}
	return rumor_buf;
}

void
outrumor(truth, mechanism)
int truth; /* 1=true, -1=false, 0=either */
int mechanism;
{
	static const char fortune_msg[] =
		"This cookie has a scrap of paper inside.";
	const char *line;
	char buf[BUFSZ];
	boolean reading = (mechanism == BY_COOKIE ||
			   mechanism == BY_PAPER);

	if (reading) {
	    /* deal with various things that prevent reading */
	    if (is_fainted() && mechanism == BY_COOKIE)
	    	return;
	    else if (Blind) {
		if (mechanism == BY_COOKIE)
			pline(fortune_msg);
		pline("What a pity that you cannot read it!");
	    	return;
	    }
	}
	line = (rn2(20) ? getrumor(truth, buf, reading ? FALSE : TRUE) : random_mesgX[rn2(SIZE(random_mesgX))] ) ;
	if (!*line)
		line = "NetHack rumors file closed for renovation.";
	switch (mechanism) {
	    case BY_ORACLE:
	 	/* Oracle delivers the rumor */
		pline("True to her word, the Oracle %ssays: ",
		  (!rn2(4) ? "offhandedly " : (!rn2(3) ? "casually " :
		  (rn2(2) ? "nonchalantly " : ""))));
		verbalize("%s", line);
		exercise(A_WIS, TRUE);
		return;
	    case BY_COOKIE:
		pline(fortune_msg);
		/* FALLTHRU */
	    case BY_PAPER:
		pline("It reads:");
		break;
	}
	pline("%s", line);
}

STATIC_OVL void
init_oracles(fp)
dlb *fp;
{
	register int i;
	char line[BUFSZ];
	int cnt = 0;

	/* this assumes we're only called once */
	(void) dlb_fgets(line, sizeof line, fp); /* skip "don't edit" comment*/
	(void) dlb_fgets(line, sizeof line, fp);
	if (sscanf(line, "%5d\n", &cnt) == 1 && cnt > 0) {
	    oracle_cnt = (unsigned) cnt;
	    oracle_loc = (long *) alloc((unsigned)cnt * sizeof (long));
	    for (i = 0; i < cnt; i++) {
		(void) dlb_fgets(line, sizeof line, fp);
		(void) sscanf(line, "%5lx\n", &oracle_loc[i]);
	    }
	}
	return;
}

void
save_oracles(fd, mode)
int fd, mode;
{
	if (perform_bwrite(mode)) {
	    bwrite(fd, (genericptr_t) &oracle_cnt, sizeof oracle_cnt);
	    if (oracle_cnt)
		bwrite(fd, (genericptr_t)oracle_loc, oracle_cnt*sizeof (long));
	}
	if (release_data(mode)) {
	    if (oracle_cnt) {
		free((genericptr_t)oracle_loc);
		oracle_loc = 0,  oracle_cnt = 0,  oracle_flg = 0;
	    }
	}
}

void
restore_oracles(fd)
int fd;
{
	mread(fd, (genericptr_t) &oracle_cnt, sizeof oracle_cnt);
	if (oracle_cnt) {
	    oracle_loc = (long *) alloc(oracle_cnt * sizeof (long));
	    mread(fd, (genericptr_t) oracle_loc, oracle_cnt * sizeof (long));
	    oracle_flg = 1;	/* no need to call init_oracles() */
	}
}

void
outoracle(special, delphi)
boolean special;
boolean delphi;
{
	char	line[COLNO];
	char	*endp;
	dlb	*oracles;
	int oracle_idx;
	char xbuf[BUFSZ];

	if(oracle_flg < 0 ||			/* couldn't open NH_ORACLEFILE */
	   (oracle_flg > 0 && oracle_cnt == 0))	/* oracles already exhausted */
		return;

	oracles = dlb_fopen_area(NH_ORACLEAREA, NH_ORACLEFILE, "r");

	if (oracles) {
		winid tmpwin;
		if (oracle_flg == 0) {	/* if this is the first outoracle() */
			init_oracles(oracles);
			oracle_flg = 1;
			if (oracle_cnt == 0) return;
		}
		/* oracle_loc[0] is the special oracle;		*/
		/* oracle_loc[1..oracle_cnt-1] are normal ones	*/
		if (oracle_cnt <= 1 && !special) return;  /*(shouldn't happen)*/
		oracle_idx = special ? 0 : rnd((int) oracle_cnt - 1);
		(void) dlb_fseek(oracles, oracle_loc[oracle_idx], SEEK_SET);
		if (!special) oracle_loc[oracle_idx] = oracle_loc[--oracle_cnt];

		tmpwin = create_nhwindow(NHW_TEXT);
		if (delphi)
		    putstr(tmpwin, 0, special ?
		          "The Oracle scornfully takes all your money and says:" :
		          "The Oracle meditates for a moment and then intones:");
		else
		    putstr(tmpwin, 0, "The message reads:");
		putstr(tmpwin, 0, "");

		while(dlb_fgets(line, COLNO, oracles) && strcmp(line,"---\n")) {
			if ((endp = index(line, '\n')) != 0) *endp = 0;
			putstr(tmpwin, 0, xcrypt(line, xbuf));
		}
		display_nhwindow(tmpwin, TRUE);
		destroy_nhwindow(tmpwin);
		(void) dlb_fclose(oracles);
	} else {
		pline("Can't open oracles file!");
		oracle_flg = -1;	/* don't try to open it again */
	}
}

int
doconsult(oracl)
register struct monst *oracl;
{
#ifdef GOLDOBJ
        long umoney = money_cnt(invent);
#endif
	int u_pay, minor_cost = 50, major_cost = 500 + 50 * u.ulevel;
	int add_xpts;
	char qbuf[QBUFSZ];

	multi = 0;

	if (!oracl) {
		There("is no one here to consult.");
		return 0;
	} else if (!oracl->mpeaceful) {
		pline("%s is in no mood for consultations.", Monnam(oracl));
		return 0;
#ifndef GOLDOBJ
	} else if (!u.ugold) {
#else
	} else if (!umoney) {
#endif
		You("have no money.");
		return 0;
	}

	Sprintf(qbuf,
		"\"Wilt thou settle for a minor consultation?\" (%d %s)",
		minor_cost, currency((long)minor_cost));
	switch (ynq(qbuf)) {
	    default:
	    case 'q':
		return 0;
	    case 'y':
#ifndef GOLDOBJ
		if (u.ugold < (long)minor_cost) {
#else
		if (umoney < (long)minor_cost) {
#endif
		    You("don't even have enough money for that!");
		    return 0;
		}
		u_pay = minor_cost;
		break;
	    case 'n':
#ifndef GOLDOBJ
		if (u.ugold <= (long)minor_cost ||	/* don't even ask */
#else
		if (umoney <= (long)minor_cost ||	/* don't even ask */
#endif
		    (oracle_cnt == 1 || oracle_flg < 0)) return 0;
		Sprintf(qbuf,
			"\"Then dost thou desire a major one?\" (%d %s)",
			major_cost, currency((long)major_cost));
		if (yn(qbuf) != 'y') return 0;
#ifndef GOLDOBJ
		u_pay = (u.ugold < (long)major_cost ? (int)u.ugold
						    : major_cost);
#else
		u_pay = (umoney < (long)major_cost ? (int)umoney
						    : major_cost);
#endif
		break;
	}
#ifndef GOLDOBJ
	u.ugold -= (long)u_pay;
	if (rn2(2)) oracl->mgold += (long)u_pay; /* prevent player from getting everything back --Amy */
#else
        money2mon(oracl, (long)u_pay);
#endif
	flags.botl = 1;
	add_xpts = 0;	/* first oracle of each type gives experience points */
	if (u_pay == minor_cost) {
		outrumor(1, BY_ORACLE);
		if (!u.uevent.minor_oracle)
		    add_xpts = u_pay / (u.uevent.major_oracle ? 25 : 10);
		    /* 5 pts if very 1st, or 2 pts if major already done */
		u.uevent.minor_oracle = TRUE;
	} else {
		boolean cheapskate = u_pay < major_cost;
		outoracle(cheapskate, TRUE);
		if (!cheapskate && !u.uevent.major_oracle)
		    add_xpts = u_pay / (u.uevent.minor_oracle ? 25 : 10);
		    /* ~100 pts if very 1st, ~40 pts if minor already done */
		u.uevent.major_oracle = TRUE;
		exercise(A_WIS, !cheapskate);
	}
	if (add_xpts) {
		more_experienced(add_xpts, u_pay/50);
		newexplevel();
	}
	return 1;
}

/*rumors.c*/

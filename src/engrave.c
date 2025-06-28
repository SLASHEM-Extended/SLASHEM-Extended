/*	SCCS Id: @(#)engrave.c	3.4	2001/11/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"
#include <ctype.h>

STATIC_VAR NEARDATA struct engr *head_engr;

#ifdef OVLB
/* random engravings */
static const char *random_mesg[] = {
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
	"....TCELES B HSUP   A magic spell?", /* Final Fantasy I (US) */
	"Madam, in Eden, I'm Adam.", /* A palindrome */
	"Turn around.", /* Various people at various times in history */
	"UY WUZ HERE", /* :] */
	"Time flies when you're having fun.", /* Who said this first, anyway? */
	"As if!", /* Clueless */
	"How bizarre, how bizarre.", /* OMC */
	"Silly rabbit, Trix are for kids!", /* Trix */
 	"I'll be back!", /* Terminator */
 	"That is not dead which can eternal lie.", /* HPL */
"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon",
"we serve perfect meals and only exdule all cavepeople- The Chromatic Dragon cafe",
"**FR33 P0T10N 0F FULL H34L1NG**",
"An HTML attachment was scrubbed...",
"Are YOU tired of the same old messages? Go to http://alt.org/nethack/addmsgs/ now!",
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
"Dig down!",
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
"Elbereth", /* okay to have this several times because it has an actual gameplay effect --Amy */
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
"Galadriel",
"Gatorade. Is it in you?",
"George plonks ferrets.",
"Get A BRAIN! MORANS",
"Gilthoniel",
"Go Left -->",
"Go around! Go around and around and around! What fun! I'm so happy!",
"Go see Snakes on a Plane.",
"Got milk?",
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
"Watch out! The-Gnome-With-The-Wand-Of-Death is behind that door!",
"We are not amused.",
"Where are the ats of yesteryear?",
"Who is this foreign host and why does he keep closing my sessions?",
"Why are you looking down here?",
"Why are you reading this?",
"Why's everybody always picking on me?",
"X marks the spot.",
"Yeah, well, y'know, that's just like, your opinion, man.",
"You are already dead!",
"You are here.",
"You aren't going to ascend by staring at your shoes.",
"You can't help being ugly, but you could stay home.",
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
	"Snakes on the Astral Plane - Soon in a dungeon near you!",
	"There was a hole here.  It's gone now.",
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
	"Inside of a dog, it's too dark to read.",
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
	
	"Mistakes were made.",
	"But at what cost?",
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
	"Never tell me the odds!", /* Starwars */
	"Needle-like the new log to know that SlashEM Extended are you ...",
	"http://nethackwiki.com/wiki/Slash%27EM_Extended I will not cease to poison by carbon monoxide",
	"after opportunity to kill Cthulhu Geenome lightsaber, playing for the Khajiit-transsexual talk about that in other games is the freedom to choose make me laugh only",
	"Yes, we'll see a list of races and added roles such trash",
	"This area is property of Team Dislexiapater.",
	"Anna's Hussy Club - Main Entrance",
	"Marking left by Arabella to denote space for nasty traps",
	"ElberethElbereth", "ElberethElberethElbereth",
	"ElberetnElherethLlberethElbcrethElbere?hElbercthFlbereth", "Ylbereth",
	"I never hit with a wielded weapon, but I changed form six times.",
	"Always name your kitten after a famous wizard.",
	"You are in a maze of twisty little passages, all alike.",
	"You have died of dysentery.",
	"The cake is a lie.",
	"The cake is a lie, says Super Mario after rescuing the princess 5 minutes into the game in Super Mario 64 by using TAS tools",
	"We all make choices, but in the end our choices make us.",
	"We juggle priceless eggs in variable gravity.", /* Gateway */
	"They therefore went up here with much agility and speed", /* Borders of Infinity (and originally from Pilgrim's Progress) */
	"We are the dead.", /* 1984 */
	"To sleep, perchance to dream, aye, there's the rub.", /* Hamlet */
	"When the goat turns red strikes true.", /* Order of the Stick */
	"It is a far, far better thing I do than I have ever done.", /* A Tale of Two Cities */
	"All dungeon adventurers are equal; but some are more equal than others.", /* Animal Farm */

	"Dare mighty things.", /* Moto of the Mars Science Laboratory */
	"are we cool yet?", /* SCP Foundation */
	"I seem to be having tremendous difficulty with my lifestyle", /* Hitchhiker's Guide to the Galaxy */
	"The flow of time itself is convoluted, with heroes centuries old phasing in and out.", /* Dark Souls */

	"Who are you?", "What do you want?", "Why are you here?", "Where are you going?", /*Babylon 5*/
	"Beware of the weeping angels", /* Doctor Who */
	"Duck, Sally Sparrow", /* Doctor Who */
	"Standing on this square increases your weeping angel contamination by 50 per turn",
	"You made a mistake by stepping on this square - now dnethack's elder priest was spawned on the current level.",

	"4 8 15 16 23 42", /* Lost */
	
	"This too shall pass.", /* medieval Persian adage */

	"Write down the coordinates, he said.", /* the Submachine series */
	"...look for a green leaf...",
	"...bring it to the statue...",
	"...you should arrive at the lab...",
	"...or somewhere nearby...", 

	"Here would be graffiti about one of the first nine ascenders of dnethack, but this isn't dnethack.",
	"AmyBSOD was here. AmyBSOD followed AmyBSOD. (repeat 6 times) Why is no one else ascending SLEX?",
	"Why don't you put some effort into it and ascend? SLEX's demigod bar is waiting for you!",

	"@ <--- You are here.",
	
	"Jesus was here 2/15/57 B.C.", /* Family Guy */
	"If you have time to read this, you should get to the down stairs immediately!", /* Zelda 3 */
	"Gary was here! Ash is a loser!", /* Pokemon */
	
	"I see you...",
		"...do you see me?",
		"That was a mistake.",
	"What was drowned, and washed away, might never rise again.",
		"Eternity is a lie.",
		"Everything dies.",
		"What is lost might not be found.",
		"And even if it all came back, it would never be the same.",
	
	"Department of Game Balance: Closing barn doors since day two.",
	"Department of Game Balance, Terrahack Branch Office: Designing well-balanced elder priests for you to enjoy fighting!",
	
	"Listen at us, won't you?", /* All Our Friends Are Dead */
		"Share with us your head",
		"Do hide behind the iris",
		"As all of our friends are dead.",
	
	"T1M3 FOr WH4T?", /*Homestuck. This is Terezi's side of her conversation with Snowman, */
		"WH4T M1SS1ON?", /*in which Terezi communicated by writing on walls and statues etc with colored chalk*/
		"WH4T 4BOUT TH3 QU33N?",
		"J4CK W4S H3LP1NG US 3X1L3 H3R SO...BOTH??? >:\\",
		"OK H3H3H3 >:] WOW OHMYGOD D4RK T3XT L4DY! 1 G3T 1T!!!!!",
		">8[ NO 1 DONT TH1NK SO WH3R3 4R3 YOU TH3N?",
		"OK SORRY 1 4SK3D! >:O",
		"WH4T? NO H3H3H3 1TS OK B3C4US3",
		"1 DONT W4NT TO RUN OUT OF R3D >:[",
		"B3C4US3!!! R3D 1S TH3 B3ST 4ND T4ST3S TH3 B3ST!!! <3",
		"OK TH3N WH4T?",
		"WH3R3 1S 1T?",
	"Im Sure That Possibly All Made Sense Good Speech",
	
	"And there shall be laughter and magic and blood, and we shall dance our dance until the end of time...", /*Incursion, Halls of the Goblin King*/
		"May you find beauty in endings.",
	
	"The eclipse will shatter the sky and from the remnants will bleed a storm of fire to engulf the earth.",	/* Gone with the Blastwave */
		"The spirits of past sins will char the land and cleanse the planet.",
		"As humans shed their former selves their true form shall be revealed.",
		"Join us in the dancing lights. Join us in the Celebration!",
		"Burn them! Burn ALL!",
		
	"Speak not the Watchers.",	/* Drakengard */
		"Draw not the Watchers.",
		"Write not the Watchers.",
		"Sculpt not the Watchers.",
		"Sing not the Watchers.",
		"Call not the Watchers' name.",
	"Should you encounter God on your quest, God will be cut.", /*Kill Bill*/
	"Generally, when one is conducting an experiment, it is worth noting that the observers went insane, killed each other, and then rose from the dead.", 
		/*Pyro627, regarding an experiment in creating undead bacon in Dwarf Fortress. An "accident" with an axe led to a zombified army, 
			which the poster didn't even comment on.*/
		"Today, in 'poor word choice': 'Urist, go make a new table with our mason.'", /*Swonnrr, regarding a fell mood.*/
		"The horses outnumber us. I have seen settlements with a thousand horses to a man. I have seen them in the deepest caverns. They are everywhere. Save us.", /*Quote from: Fniff regarding a bug causing ubiquitous horses*/
	"All the world's cracked but me and thee; and I'm not too sure about thee.",
	"Some walls are made of fear, but some are made from wall material.", /*Clash of Clans metro advert */
	"No gods, no masters.", /*Anarchist slogan*/
	"For the world's more full of weeping than you can understand.", /*The Stolen Child*/
	"No matter where you go, there you are.",
	"You noticed them, and they noticed that you noticed them.", /*The Mothman Prophecies*/
	"Well, how did I get here?", /*Once In A Lifetime*/
	"You can be me when I'm gone.", /*Sandman*/
		"You can stop being anything.",
		"Sometimes you wake up. Sometimes the fall kills you. And sometimes, when you fall, you fly.",
		"To absent friends, lost loves, old gods and the season of mists.",
		"And may each and everyone of us always give the devil his due.",
		"Innocence, once lost, can never be regained.",
		"In the pale light of the Moon I play the game of you. Whoever I am. Whoever you are.",
	"Have been unavoidably detained by the world. Expect us when you see us.", /* Stardust */
		"There was once a young man who wished to gain his Heart's Desire.",
		"Time, the thief, eventually takes all things into his dusty storehouse.",
		"Scuse me, but would you mind dreamin' a bit quieter?",
	"He who is valiant and pure of spirit may find the holy grail in the Castle of Aaaaarrrrrrggghhh...", /*Monty Python*/
	"If someone is reading this...I must have failed.", /*The Guardian Legend*/
	"\"Wasn't somebody's father?\"", /*The main plot point of Star Wars, according to one who hasn't seen it*/
	"The psychic efficacy of evil magic swords is vastly overrated.", /* The Water Phoenix King*/
		"It is dangerous to be right when the gods are wrong.",
		"Here are blind idiot children playing with a magic that hates them.",
		"The walled-up abomination seems friendly enough.",
		"Kill you... Take your face...",
		"The trees don't even change color anymore.",
		"What happend to you? It wasn't me!",
		"I'm sorry you could never forgive me.",
	"Reach heaven through violence.",/*Kill 6 Billion Demons*/
		"Kindly ignore my first three answers.",
	"ALL POWER TO THE OLD FLESH!!!!!!! A HOWLING VOID UPON DETRACTORS!!!", /*Awful Hospital*/
	"You are lost. You can never go home.", /* Gollum's Song */
	"But now, we dance this grim fandango, and will for years until we rest.", /* Grim Fandango */
	"Bad Wolf", /* Dr. Who */
	"It's only the end if you want it to be.", /*Batgirl*/
	"Live. Die. Repeat.", /*Edge of Tomorrow*/
	"For we wrestle not against flesh and blood, but against principalities and powers.", /* Ephesians 6:12 */
	"Life is truth, and never a dream...", /* Persona 4 */
	"fire, walk with me.", /*Twin Peaks*/
	"redefine happiness", /* Blue Oyster Cult */
	"Evolve. Transform. Transcend", /*Heart of the Swarm*/
	"no more sadness, no more anger, no more envy...", /*Halo*/
		"Silence fills the empty grave, now that I have gone.",
		"But my mind is not at rest, for questions linger on.",
	"Everyone you know will tell you small lies, except the ones you love, who will tell you large ones.", /*Borderlands 2*/
		"I am eternal / Death will never capture me / My skills exceed his.",
	"We are born of the blood.",/*Bloodborne*/
		"Made men by the blood.",
		"Undone by the blood.",
		"Fear the old blood.",
		"The Great Ones are sympathetic in spirit, and often answer when called upon.",
		"O Flora, of the moon, of the dream.",
		"O Little ones, o fleeting will of the ancients.",
		"Let the hunter be safe.  Let this dream foretell a pleasant awakening...",
	"We do not breathe. We do not eat. We do not sleep. We do not stop. ", /* Sigma */
		"We have outlasted our gods. We know true freedom.",
		"We do not suffer the enslavement of your passions, or your religions, or your creeds and your self conflict.",
		"We are the Vederiction of our fallen deities, their visage perfected.",
		"We have marched upon the cosmos long before your ancestors left their home.",
		"And we will endure long after your dying civilization perishes on the chain of worlds.",
	"Even false things are true",	"FIVE TONS OF FLAX!",	"Fnord", /*Discordianism*/
	"Ash nazg durbatuluk, ash nazg gimbatul, ash nazg thrakatuluk, agh burzum-ishi krimpatul", /*the Lord of the Rings*/
		"This thing all things devours.",
		"Not all those who wander are lost.",
	"This is the curse of the Buddha-you will never again be the same as once you were.", /*Lord of Light*/
		"Tricky and brilliant and heartfelt and dangerous.",
		"Death is mighty, and is no one's friend.",
		"It is the difference between the unknown and the unknowable, between science and fantasy-it is a matter of essence.",
		"Go away. This is not a place to be.",
	"No Fate", /*Terminator*/
	"When the world is in darkness Four Warriors will come....", /*FF I*/
	"This world can have but one Emperor.", /*FF II*/
	"We shall devour your light, and use it to return this world to the Void!", /*FF III*/
	"You tread the path of darkness.",/*FF IV*/
	"My hatred will not be stanched until it has consumed all else!", /*FF V*/
	"Life... dreams... hope... Where do they come from? And where do they go...?", /*FF VI*/
	"Mine is special. It's good for absolutely nothing!", /*FF VII*/
	"I will NEVER be a memory.", /*FF VII Advent Children*/
	"Trees fade with time, as do many things in this world.", /*FF VII Dirge of Cerberus*/
	"Even if the world becomes your enemy... I'll be your knight, I'll protect you.", /*FF VIII*/
	"You don't need a reason to help someone.", /*FF IX*/
	"Memories are nice, but that's all they are.", /*FF X*/
	"Better to leave things lost.", /*FF X-2*/
	"Benighted child, Unchosen, with life so short, This tower freely may you tread.", /*FF XII*/
	"The thirteen days after we awoke were the beginning of the end.", /*FF XIII*/
	"Servant of time. Daughter of chaos. Unto the world unseen, the untamed you guide.", /*FF XIII-2*/
	"One day, the light touched me.", /*FF XIII-3*/
	"What greater weapon is there than to turn an enemy to your cause? To use their own knowledge against them?", /* Knights of the Old Republic */
		"Sand stays.  All else changes.",
		"We will remember you when you are sand too.",
	"That is a lesson I will never learn, for I do not believe it.", /* Knights of the Old Republic II */
	"Do not call up any that you can not put down.", /*HP Lovecraft*/
		"That is not dead, that can eternal lie,",
		"for with strange aeons even death may die.",
	"All the world will be your enemy, Prince With A Thousand Enemies...", /*Watership Down*/
		"...and when they catch you they will kill you...",
		"but first they must catch you.",
	"This statement is false.", /*Logical contradiction*/
	"History is an account, mostly false, of events, mostly unimportant, which are brought about by rulers, mostly knaves, and soldiers, mostly fools",  /*Ambrose Bierce*/
	"Quod Erat Demonstrandum",	"anno Domini",	"per centum", /* QED, AD, percent */
	"I was so surprised, I CLASSIFIED INFORMATION, I really did!", /*Haruhi Suzumiya*/
		"Problems that cannot be solved do not exist in this world.",
		"Feelings of love are just a temporary lapse in judgment.",
	"I'm not going there to die. I'm going to find out if I'm really alive.", /*Cowboy Bebop*/
		"There is nothing left to believe in.",
	"He hides who he really is and pretends to be someone else forever.", /*FLCL*/
		"So in time he becomes that person, so his lie becomes the truth, see?  He transcends the mask.",
		"Well, don't you get it? That's how he finds happiness. That's pretty good, right?",
		"Special Containment Procedures: You're going to die, you poor dumb fuck.", /* SCP-1983 */
		"Hello.", "You are no longer lost.", "Here is the map for your journey up.", /* SCP-2084 - Anabasis */
		"A war, whoever was left alive, was Clean.", /* Red Sea Object */
		"Let the depths rise up and sweep over the shores, rendering the works of man to dust.", /* SCP-1014 - Jonah Crusoe */
		"in my head theres a sick silk nail", /* SCP 1291 */
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
		"Ahahahahaha! Ahahahaha! Aahahaha! BEWARE!!!!! Yrs sincerely, The Opera Ghost",
		"Bee There Orr Bee A Rectangular Thyng",
		"We're certainly dwarves",
		"There is no doubt being human is incredibly difficult and cannot be mastered in one lifetime.",
	"NOT A Secret Base", /*Gunnerkrigg Court*/
	"We have the option to definitely die, but I've decided against it.", /*Schlock Mercenary*/
	"Short to long term memory impaired. Go to Robotics Building. Explain about Bowman's architecture. Write down everything.", /*Freefall*/
	"I SAW THE EYE. Over the horizon, like a rising sun!", /* Dresden Codak */
		"That's Science, isn't it? The perpetual horizon. But we found a way, haha, we found a way around that!",
		"What would YOU give up to... to see it through?",
		"Nothing. A shadow is nothing. It is merely a question not yet answered. We only fear the dark if we have no means of lighting our way.",
		"The world turns, and dawn comes. And under the light of the sun I shall slay giants.",
	"Don't trust the skull.", /* Cynarfpncr: Gbezrag (mild spoiler) */
	"death is the greatest illusion", /*Blade of Innocence*/
	"Our sandwiches come with attempted assassination plots. New sales gimmick!", /*Shemeska's Planescape Storyhour*/
		"I've stood at that edge! I've looked over into that void! I'm just looking at it from the bottom now!",
		"Free! Apple pie with every meal! We don't care if you don't like apple - Take It!",
		"For there is a hole in the sky",
		"and too long have I gazed",
	"The quality of your lives depends on what you make of them!", /* The Life and Times of Scrooge McDuck */
	"We do not threaten or warn or advise. We tell the truth.", /* The Mansion of E */
		"What you stole is even more dangerous and precious than you know.",
		"In the end, you will fly away and live forever, and you will leave the world shattered in your wake.",
		"You will return to where you started, and you will become what you hate.",
		"And in doing so, you will restore the world to what it was.",
		">^ N^T R3A> T||1S S1<N",
		">^ R3A> T||1S ^N3",
	"Fall in a more hardboiled manner.", /* MS Paint Adventures */
		"Since you are reading this, chances are you have already installed this game on your computer.  If this is true, you have just participated in bringing about the end of the world.  But don't beat yourself up about it.",
		"YOU HATE TIME TRAVEL YOU HATE TIME TRAVEL YOU HATE....",
		"It begins to dawn on you that everything you just did may have been a colossal waste of time.",
	"We will BUILD heroes!", /* THE PROTOMEN */
		"If you replace the working parts, you get a different machine.",
		"If you destroy the working parts, what you get is a broken machine.",
	"To right the countless wrongs of our day, we shine this light of true redemption, that this place may become as paradise.",/*It's a Wonderful World/The World Ends with You*/
		"What a wonderful world such would be...",
		"Please...Don't kill me...",
		"Another world awaits. ...And you're going!",
		"You have seven days.",
	"But the future refused to change.", /*Chrono Trigger*/
	"WHEN ALL ELSE FAILS USE FIRE",
	"Don't you see? All of you... YOUR GODS DESTROYED YOU",
		"The wind... It is blowing...",
	"I wonder... If you do the right thing... Does it really make... everybody... happy?",/*Majora's Mask*/
		"Your friends... What kind of... people are they? I wonder... Do these people... think of you... as a friend?",
		"What makes you happy? I wonder...what makes you happy...does it make...others happy, too?",
		"Your true face... What kind of... face is it? I wonder... The face under the mask... Is that... your true face?",
		"You've met with a terrible fate, haven't you?",
	"Why are you sad?", /*Blood-C*/
	"Only accurate information has practical application.", /* not sure where this originated, possibly Aron Ra?*/
	"We will come for you again  When you are alone  In the dark  Without your sword", /*Jake Wyatt's Necropolis*/
		"My sword is only steel in a useful shape. It's me you should fear, you stupid, dead things.",
		"There are horrors.  But also the sun, the grass... And the road I wish to travel.",
	"Now watch what you say or they'll be calling you a radical!", /* The Logical Song, Supertramp */
	"I took a walk around the world, to ease my troubled mind.", /* Kryptonite */
	"Do what thou wilt shall be the whole of the Law", /* Aleister Crowley */
	"Oh, 'great warrior...' Wars not make one great!", /*Star Wars*/
		"I've got a bad feeling about this...",
		"Do, or Do Not.  There is no Try.",
		"Never tell me the odds!",
	"There's no replacement for displacement.",
	"This ASCII tile was donated by Tariru.",
	"Antediluvian arelgee message: Always do opposite of what Amy says.",
	"lol you fag you're wasting time reading pointless messages",
	"Entrance to Anna's Hussy Club, if you're male then bugger off or prepare to clean the dog shit off our shoes",
	"Bang Gang Office main entrance, unauthorized trespassers will be killed",
	"You are standing on a nasty trap, which don't give a message and don't reveal themselves when triggered.",
	"You stepped on my fucking foot, ese!",
	"Warning, this tile will transform into a trapdoor in 5 turns unless you step off.",
	"Your fault for stepping on this particular tile - the game just became unwinnable because the Amulet of Yendor was removed from the game permanently.",
	"Sorry, but our princess is in another dungeon.",
	"Why are you here? You're supposed to go rescue the bitch that calls herself Princess Peach, and she's in another dungeon!",
	"Go north from here if you want to find a room full of sexy girls.",
	"Looking for the entrance to the brothel? One level down from here, northwest corner.",
	"Bank of RA - Gold Deposit", /* Scarab of Ra */
	"If you play with the wand destruction patch active, this particular square renders its effects inactive while you stand on it!",
	"The dimensional portal to Rodney's Tower is one tile north of this location. Lasciate ogni speranza o voi ch'entrate.",
	"Warning: Somewhere on this level, there's a dimensional portal to K-Measurer. If you trigger it, you have to ascend a tourist within 3 real-time months or your SLEX character is deleted.",
	"UNDER ME", /* apparently from Narnia, according to aosdict? */
	"You have just won a goat", /* by elenmirie */
	"Welcome to the party!",
	"Quantify this egghead!",
	"Everything in Demo's shop will cause a segfault when picked up.",
	"THIS ROOM IS AN ILLUSION AND IS A TRAP DEVISUT BY SATAN. GO AHEAD DAUNTLESSLY! MAKE RAPID PROGRES!",
	"Murat's Doener Kebap Place - entrance 3 tiles to the north",
	"This tile has some residual hussy perfume on it. If you're not a hussy yourself, staying here for more than 5 turns will make you fall over unconscious.",
	"Welcome to Luigi's Pizza Parlor!",
	"Naechste Station: Schlaefelheim-Fluegelingen (bei Bueschelhausen).",
	"Good things will happen to those who wait for 2000 turns on this tile.",
	"There used to be one of Hans Walt's diamond barriers on this tile but we, the hussies, smashed it with our high heels.",
	"There used to be one of Hans Walt's hidden diamonds on this tile but I, Rita, stole it in the name of the hussies.",
	"There used to be an unconscious bodyguard of Hans Walt lying on this tile. If you didn't find him here, report it to Anna immediately so she can sic her hussies on the bastard again.",
	"Welcome to the Dungeons of Doom(TM) Fitness Center! You've definitely come to the right place - we have lots of boulders that you can use to train your strength!",
	"This area is under video surveillance.",
	"Minetown Kop Station <--- You are here",
	"Stay on this tile and smell the roses.",
	"Hint: You can eat some BDSM one dungeon level below. Yes, you read that right, there's a Burger King, a Doener Kebab place, a Shawarma restaurant and a McDonalds!",
	"This maze has no exit, so you don't even need to bother looking for one.",
	"Looking for the exit? Too bad, it was sealed as a result of you stepping on this particular tile.",
	"You've triggered the doormash switch that was hidden on this tile, and now the game will erase your save by spontaneously updating its versions.",
	"Error: graffiti not found. Please phone Amy for a fix, her tel number: 0190666373737-FIXME.", /* don't bother, I made up the number and it doesn't really exist :P */
	"Help! I'm being held captive in a SLEX graffiti-writing factory!",
	"My, just look at those peculiar markings! Ugly little bugger.",
	"This tile was borrowed from dnhslex.",
	"Hahaha, you are reading my graffiti!",
	"Only those who wear high heels may step on this tile! Everyone else gets hit by a permanent nasty trap effect.",
	"You stepped on a levelscaler trap, which is a nasty trap that gives no message and doesn't reveal itself even if you trigger it.",
	"The Amy is hogging all the lesbians and maintains a rotating harem of 50-100 lesbians.",
	"The Amy believes, unironically, in the pantheon of Nethack and engages in 'fleecy' sexual rituals with her harem loves in which she hopes to immunize the eschaton to prevent diseases so that the demiurge may contain infinite control over the world.", /* from 4chan */
	"The Amy routinely uses SLEX servers to meet lesbians and promises updates/game secrets in return for 'favors' from lesbian players.",
	"Vanilla Nethack getting BORING? Doing soko mazes in your sleep and while sitting on the train? Want a CHANGE? Try.... LAGOBAN! Do Sokoban from a server halfway around the planet, and bounce your signal off a server in yet another country while you're doing it! Make sure your wifi router is no closer than 40m from your current position for the full experience!",
	"A fortune cookie told me if you try to set message history to before you were born, it angers your god!",
	"GOOGL EM: Greased Outlandish Opportunist's Guide to Legends: Extended Magic",
	"At the Shoeshop 69, 00420 Bundletown, Fleecyland",
	"In the next was at the cover a schiessmaschine, which shot continuing, resounds.",
	"Please brush your feet before entering the Dungeons of Doom. Sincerely, Moloch.",
	"what's an anagram of Banach-Tarski? Banach-Tarski Banach-Tarski.",
	"Well you are sometimes really stupid! And sometimes very! And sometimes small! And sometimes very!",
	"that girl is called hole-girl-ass with me and is sow-super shit. and above all, i and my friend simply threw her about 18 found eggs next to the tree! har-har!",
	"the most humans create super mario land 5 up to the end boss of the last world 8-4, but that is completely fast and (almost) only with fireballs defeatable. that is the all-heaviest end boss of the mario-luigi-games. har-har!",
	"nobody except me has super mario land 5 completely finished, but i have in 8-4 always fireballs when i'm at the end boss. so somehow the game is sow easy. har-har!",
	"i have the game 'super mario land 5' completely finished until world 8-4, and that i finished too, and even the sow-heavy end boss on which you have to jump 10 times. har-har!",
	"crapshoot noun, North American - The chance of finding a secret door or corridor in NetHack with a new character.",
	"DEMO casts SEGFAULT! It's super effective! THRONE-POOFING CODE fainted! DNETHACK was defeated!",
	"slexxx <--- nethack porn", /* by K2 */
	"Whoops, by reading this message you've contracted corona virus!",
	"This space is definitely not infected with covid-19 yet.",
	"know when to hold em, know when to fold em, know when to pork'em, know when to run", /* by porkman */
	"The engraving you have just started reading is the engraving you have just finished reading.",
	"SLEX is hereby presented to you by Amy Bluecuntwipeofdeath. Or it would be, according to a certain vandal on the roguebasin site.",
	"fleecy bundle is an anagram of bully defence",
	"Locke's men bind Julyne Stark to a tree, and then Locke pulls out his most devastating weapon which is roughly wand-shaped, using it to rob Julyne of something that she can't ever get back.",
	"You cheated not only the game, but yourself. You didn't grow. You didn't improve. You took a shortcut and gained nothing. You experienced a hollow victory.",
	"get blasted off to space by the 5730 turboass! call now to receive a FREE turbofinger with your purchase!", /* by spicycat */
	"Team Ant getting you down?  Call Nero's Extermination Service today!",
	"Got milky potions?",
	"Try Pengalengan's Green Slime Meringue Pies - The Combustible Comestible",
	"Lonely? Try one of Madame Ruth's Patented Love Potions - 'Taming in a Bottle'",
	"All your castle are belong to us",
	"DON'T DEAD OPEN INSIDE",
	"The Enclave is here! Being led by Aendru Rasenban who unleashes his multigoons to spread fear and panic, they've arrived and want to kill our peaceful citizens! Run for your lives!!!",
	"notnethack is a variant made by emo", /* by elronnd */
	"are the pirates properly rapey?",
	"Amy is an error in the matrix and needs to be terminated before she releases another stupid version of slex!",
	"Everywhere you go, the-e-ere you are, and you fall asleep, from this pot of sleep.",
	"Es war einmal ein Fraeuelein, das war ganz gemeinelein. Da brach sie sich ein Naegelein, und fing an zu schreielein.",
	"Who was it that brings the children? Was it the platypus? Or the disturbenvulture?",
	"The knackwurst is no cervela.",
	"French is no Finnish, and the knackwurst is no cervela.",
	"Q: Why is spell retention a range? A: Because it's not, it's an exact value. Only weird variants would make the display inaccurate.",
	"Asian people usually know karate, and all Germans eat sauerkraut with Bavarian veal sausage.",
	"Usually deformation is not a bad thing, I see if my equipment does not burst. In addition, if the deformation is unfavorable, I have aconite grass that can be cured.",
	"In the monster room in the lower right corner, one of the Asian girls fart attacked the petrified person, and was also slowed down.",
	"The door here is not allowed to be destroyed by soldiers at first, so that soldiers will not enter through an oblique direction and maintain an attack surface.",
	"Rule #1 of Cleaver safety: Don't play a version where it hits peaceful creatures that you didn't even attack.",
	"Floeresoemito umkadingsda in die Erde?",
	"mit dem Kalk kramsipumpamsel schnoepfen",
	"mit dem Pflanzloch verzusamzimpelt umpfmanumpfixen",
	"gelummibummiflausel Frischwaren-Fraxelschnurz",
	"oh my god, there's a man sitting to the opposite of us! giggle! and oh my god he's wearing wedge heels doublegiggle!",
	"omg slex is an autoerotic wanking simulator",
	"It is forbidden to enter a woman, even a foreigner if dressed as a man.",
	"Amy whilst reloading her assault rifle: yeah I think I'll not use guns this time", /* by amateurhour */
	"If I were you, I wouldn't dare play the piano in the bar while Loyter is hanging out in there.  Don't say I didn't warn you.",
	"The road to the east leads to the capital city of Palmia.",
	"You're going to Palmia?  Just follow the road to the east.",
	"That man you see by the graves will listen to anyone's performance, no matter how bad it is.",
	"There's a direct carriage from the east side of the bar here to the capital Palmia.",
	"If you want the best training a fighter can have, you should check out the fighter's guild under the arena office.",
	"I wonder how you get into that Pyramid north of town.  I have heard of people trying to go there, but none of them ever come back.",
	"I hear that a lot of the seediest and creepiest people head to Derphy.  I'm not sure where that place is, but we seem to get more than our fair share of thieves and vagabonds, so maybe it's not that far from here.",
	"The tavern here was built on top of a graveyard.",
	"Sometimes we see known outlaws heading to the west after they raid our farms or kill our cattle.  I wonder if something is out there.",
	"The elder always talks about an old run down castle to the southwest.  That old bag is always making up crazy stories.",
	"That black castle you see when leaving town is the royal capital.",
	"The etherwinds have been getting strong and more dangerous over the last fifty years.  My grandfather always tells me about how weak they used to be.  If they grow much stronger we might be forced to abandon our homes.",
	"The water flowing out of the sculpture on the wall is actually waste water.",
	"This is about as exciting as a bag of broken rocks.",
	"You can confess your sins at the church.  I can't think of a better way to be forgiven for your crimes.",
	"The Holy Night Festival is also known as the Festival of St.Jure. This is a feast to honor the Goddess of Healing and to celebrate the end of the year.",
	"I heard there's a town famous for hot springs somewhere.",
	"The guards will warn you if you try to fish here. Nature preservation and all that.",
	"This area is rather cold. It even snows sometimes.",
	"The Yerles seem to be investigating the ruins in the southern-central part of the country.",
	"Naplus is always looking for victims to satisfy her lecherous needs, and likes to grope people while pretending that it's for her alchemy experiments. That's why we call her 'Eronaplus' behind her back.",
	"There's a huge gambling facility underground.",
	"It's probably best not to go into the slums.",
	"There's a recreational facility for children underground.",
	"The prices here in this booming town are about half that of those in other towns.",
	"There's a large ship back there at the harbour.",
	"There's a huge bounty on the heads of those pirates that show up around here.",
	"There seems to be plans to make the nearby beach a resort spot.",
	"You can swim here all year round.",
	"Trash and debris often wash up on the shore.",
	"That turtle you see on the beach can supposedly fly in the sky by rotating his shell.",
	"The basement is off-limits to all except official personnel.",
	"This base itself can transform into a giant battle mech.",
	"The towers in the four corners of the base serve as camouflage for enormous missiles.",
	"Be careful what you buy at the souvenir shops.",
	"The lake here is very famous.",
	"Killing the tourists who come to this town isn't a sin.",
	"In the past there were bountiful fields all throughout this area.",
	"Almost no rain has fallen since the beginning of the year.",
	"There aren't many of them, but some of the books housed here are from long-destroyed civilizations.",
	"There seems to be many books in the off-limits section in the basement.",
	"We've had a lot of visitors from the world below recently.",
	"More than a thousand guests starve to death here every year because they were too engrossed in reading.",
	"The curator seems to be secretly picking up books that the mortals throw away.",
	"The downfall of previous ages was caused by people arguing whether dogs or cats were better.",
	"The mine where the platinum coins come from is controlled by an oligarchy of government officials.",
	"Sierre Terre is the 11th era on Irva.",
	"The land of Tyris has countless ruins and dungeons on it.  They are part of a long lost civilization called Nefia.",
	"In Eyth Terre, magic and science were thought to be opposites.",
	"The name 'Jure' of the Goddess of Healing is actually a pseudonym based off her homeland of Juere.",
	"Once I met this extraordinary bard who played a truly exquisite stringed instrument. He was so good that I even threw my expensive shoes at him!",
	"There was once I saw a younger sister standing around with a blank look on her face and a bloody cleaver in her hands.",
	"During the Great War between the Gods many of them scattered across this world.",
	"After the Great War the Gods' rivalry changed from trying to kill each other, to gathering the most followers.",
	"The surviving Gods of the Great War discussed among themselves and came to various agreements.",
	"Because of the Gods' conflict the mana flow has become unstable and the climate has grown worse. It seems like it's necessary to isolate the continents.",
	"The Gods that were defeated during the Great War had their powers absorbed and were disolved into tiny pieces which turned into roses. What a miserable end.",
	"It seems the surrounding march came to be from water collecting inside a giant crater.",
	"Tezcatlipoca beckoned the Gods at the start of the Great War to come to this world and launched a surprise attack against them.",
	"It's not just the people who are getting summoned. The monsters from the land below are getting transported here too.",
	"Most of the people who were summoned here are mercenaries, traveling merchants, and adventurers. That healer over there and the guild members are probably the only exceptions.",
	"Once in a blue moon, Lady Lulwy Herself graces us with Her presence. But She comes and goes like the wind, and never stays for long.",
	"That cat at the northern oasis outside the tower just lazes around most of the time. But when she's in a bad mood, she goes on a rampage and curses everything with bad luck.",
	"There are many Gods of old who lost their divinity in the wars they waged against each other. They fell and became monsters.",
	"You may find this hard to believe, but it seems like there are people receive guidance from beings in another realm. These people are unbound by logic and the laws of the world.",
	"The promenade is a rather popular spot.",
	"Whatever it is that you need, the shops in this city probably have it.",
	"Those knights are stationed here under contract with the Goddess of Wealth.",
	"Some of the ship's functions are offline due to ongoing equipment upgrades.",
	"This is the Divine Warship Albedrion. It took part in the final battle in the War of the Gods against the fleet of the forces of Chaos.",
	"This plane is rigged to blow when the altitude is below 1000 feet. Please sit back, relax, and enjoy the trip back down to the ground. It won't take long.",
	"hey guys check out my new roguelike game!!! it has realtime combat and if you die you get some goodies to help you with your next run, but its fiendishly hard so its still a roguelike!!!!!!!111 check out super action dungeons of metaprogression now, the brand-new roguelike game!!!!", /* based on the noobies that appear on r/roguelikes every once in a while, who think they can just apply the term "roguelike" to any game because it's "popular" to apply that term in a completely wrong way --Amy */
	"hey guys i'm making a turn-based roguelike! i totally feel like calling it that, to distinguish it from the many real-time roguelikes out there!!", /* ditto; if it's real-time it's not a roguelike, simple as that */
	"dendelo ga di, wololololo di gad",
	"They say that Pale Night's real name is Claudia.",
	"They say that Pale Night is capable of assuming the body and shape of a mortal, which is why she might pretend that her name was Claudia.",
	"You think SLEX is just a game, punk? Well you'll be in for a rude surprise, because SLEX is serious business and will fuck you up and kill your character if you don't put in the effort for playing properly.",
	"Invocans tam damnum sanescit, te velocissimum facit et sic duratione tutelam eiicit.",
	"The best NetHack variant for both local and online play is SLEX, hands down.",

};

const char *
random_mesgX()
{
	return (random_mesg[rn2(SIZE(random_mesg))]);
}

char *
random_engraving(outbuf)
char *outbuf;
{
	const char *rumor;
	register int itemnumber;
	char buf[BUFSZ];

	/* a random engraving may come from the "rumors" file,
	   or from the list above */

	if (!rn2(5)) strcpy(outbuf, fauxmessage()); /* sometimes have faux messages --Amy */
	else if (!rn2(3) || !(rumor = getrumor(0, outbuf, TRUE)) || !*rumor) /* increased random rumor chance --Amy */
	    strcpy(outbuf, random_mesg[rn2(SIZE(random_mesg))]);

	if (!rn2(5)) wipeout_text(outbuf, (int)(strlen(outbuf) / (8 + rn2(5)) ), 0); /* wipe out less, idea by Soviet5lo */
	return outbuf;
}

/* Partial rubouts for engraving characters. -3. */
static const struct {
	char		wipefrom;
	const char *	wipeto;
} rubouts[] = {
	{'A', "^"},     {'B', "Pb["},   {'C', "("},     {'D', "|)["},
	{'E', "|FL[_"}, {'F', "|-"},    {'G', "C("},    {'H', "|-"},
	{'I', "|"},     {'K', "|<"},    {'L', "|_"},    {'M', "|"},
	{'N', "|\\"},   {'O', "C("},    {'P', "F"},     {'Q', "C("},
	{'R', "PF"},    {'T', "|"},     {'U', "J"},     {'V', "/\\"},
	{'W', "V/\\"},  {'Z', "/"},
	{'b', "|"},     {'d', "c|"},    {'e', "c"},     {'g', "c"},
	{'h', "n"},     {'j', "i"},     {'k', "|"},     {'l', "|"},
	{'m', "nr"},    {'n', "r"},     {'o', "c"},     {'q', "c"},
	{'w', "v"},     {'y', "v"},
	{':', "."},     {';', ","},
	{'0', "C("},    {'1', "|"},     {'6', "o"},     {'7', "/"},
	{'8', "3o"}
};

void
wipeout_text(engr, cnt, seed)
char *engr;
int cnt;
unsigned seed;		/* for semi-controlled randomization */
{
	char *s;
	int i, j, nxt, use_rubout, lth = (int)strlen(engr);

	if (lth && cnt > 0) {
	    while (cnt--) {
		/* pick next character */
		if (!seed) {
		    /* random */
		    nxt = rn2(lth);
		    use_rubout = rn2(4);
		} else {
		    /* predictable; caller can reproduce the same sequence by
		       supplying the same arguments later, or a pseudo-random
		       sequence by varying any of them */
		    nxt = seed % lth;
		    seed *= 31,  seed %= (BUFSZ-1);
		    use_rubout = seed & 3;
		}
		s = &engr[nxt];
		if (*s == ' ') continue;

		/* rub out unreadable & small punctuation marks */
		if (index("?.,'`-|_", *s)) {
		    *s = ' ';
		    continue;
		}

		if (!use_rubout)
		    i = SIZE(rubouts);
		else
		    for (i = 0; i < SIZE(rubouts); i++)
			if (*s == rubouts[i].wipefrom) {
			    /*
			     * Pick one of the substitutes at random.
			     */
			    if (!seed)
				j = rn2(strlen(rubouts[i].wipeto));
			    else {
				seed *= 31,  seed %= (BUFSZ-1);
				j = seed % (strlen(rubouts[i].wipeto));
			    }
			    *s = rubouts[i].wipeto[j];
			    break;
			}

		/* didn't pick rubout; use '?' for unreadable character */
		if (i == SIZE(rubouts)) *s = '?';
	    }
	}

	/* trim trailing spaces */
	while (lth && engr[lth-1] == ' ') engr[--lth] = 0;
}

boolean
can_reach_floor()
{
	return (boolean)(!u.uswallow &&
			/* Restricted/unskilled riders can't reach the floor */
			!(u.usteed && !(uwep && uwep->oartifact == ART_SORTIE_A_GAUCHE) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_READY_FOR_A_RIDE) && !FemtrapActiveKerstin && !(bmwride(ART_DEEPER_LAID_BMW)) && (PlayerCannotUseSkills || P_SKILL(P_RIDING) < P_BASIC) ) &&
			 ((!Levitation || StrongLevitation) || is_table(u.ux, u.uy) ||
			  Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)));
}
#endif /* OVLB */
#ifdef OVL0

const char *
surface(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];

	if ((x == u.ux) && (y == u.uy) && u.uswallow &&
		is_animal(u.ustuck->data))
	    return "maw";
	else if (IS_AIR(lev->typ) && Is_airlevel(&u.uz))
	    return "air";
	else if (is_waterypool(x,y))
	    return (Underwater && !Is_waterlevel(&u.uz)) ? "bottom" : "water";
	else if (is_ice(x,y))
	    return "ice";
	else if (is_ash(x,y))
	    return "ash";
	else if (is_farmland(x,y))
	    return "farmland";
	else if (is_sand(x,y))
	    return "sand";
	else if (is_grassland(x,y))
	    return "grass";
	else if (is_snow(x,y))
	    return "snow";
	else if (is_lava(x,y))
	    return "lava";
	else if (is_table(x,y))
	    return "table";
	else if (is_mattress(x,y))
	    return "mattress";
	else if (is_pavedfloor(x,y))
	    return "pavement";
	else if (is_highway(x,y))
	    return "highway";
	else if (is_carvedbed(x,y))
	    return "bed";
	else if (is_wagon(x,y))
	    return "wagon";
	else if (is_well(x,y))
	    return "well";
	else if (is_pentagram(x,y))
	    return "pentagram";
	else if (is_bubble(x,y))
	    return "bubbles";
	else if (is_poisonedwell(x,y))
	    return "poisoned well";
	else if (is_burningwagon(x,y))
	    return "smoldering remains of a wagon";
	else if (lev->typ == DRAWBRIDGE_DOWN)
	    return "bridge";
	else if(IS_ALTAR(levl[x][y].typ))
	    return "altar";
	else if(IS_GRAVE(levl[x][y].typ))
	    return "headstone";
	else if(IS_FOUNTAIN(levl[x][y].typ))
	    return "fountain";
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
	    return "floor";
	else
	    return "ground";
}

const char *
ceiling(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];
	const char *what;

	/* other room types will no longer exist when we're interested --
	 * see check_special_room()
	 */
	if (*in_rooms(x,y,VAULT))
	    what = "vault's ceiling";
	else if (*in_rooms(x,y,TEMPLE))
	    what = "temple's ceiling";
	else if (*in_rooms(x,y,SHOPBASE))
	    what = "shop's ceiling";
	else if (IS_AIR(lev->typ))
	    what = "sky";
	else if (is_watertunnel(x,y))
	    return "low ceiling";
	else if (is_crystalwater(x,y))
	    return "crystallized ceiling";
	else if (is_stalactite(x,y))
	    return "spiky ceiling";
	else if (Underwater)
	    what = "water's surface";
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
	    what = "ceiling";
	else
	    what = "rock above";

	return what;
}

struct engr *
engr_at(x, y)
xchar x, y;
{
	register struct engr *ep = head_engr;

	while(ep) {
		if(x == ep->engr_x && y == ep->engr_y)
			return(ep);
		ep = ep->nxt_engr;
	}
	return((struct engr *) 0);
}

/* Decide whether a particular string is engraved at a specified
 * location; a case-insensitive substring match used.
 * Ignore headstones, in case the player names herself "Elbereth".
 */
int
sengr_at(s, x, y)
	const char *s;
	xchar x, y;
{
	register struct engr *ep = engr_at(x,y);

	return (ep && ep->engr_type != HEADSTONE &&
		ep->engr_time <= moves && strstri(ep->engr_txt, s) != 0);
}

#endif /* OVL0 */
#ifdef OVL2

void
u_wipe_engr(cnt)
register int cnt;
{
	if (can_reach_floor())
		wipe_engr_at(u.ux, u.uy, cnt);
}

#endif /* OVL2 */
#ifdef OVL1

void
wipe_engr_at(x,y,cnt)
register xchar x,y,cnt;
{
	register struct engr *ep = engr_at(x,y);

	if (uarmf && uarmf->oartifact == ART_ART_ROCKZ && cnt) {
		if (cnt == 1 && !rn2(3)) cnt = 0;
		else if (!rn2(3)) cnt /= 2;
	}

	/* Headstones are indelible */
	if(ep && ep->engr_type != HEADSTONE){
	    if(ep->engr_type != BURN || is_ice(x,y) || is_snow(x,y)) {
		if(ep->engr_type != DUST && ep->engr_type != ENGR_BLOOD) {
			cnt = /*rn2(1 + 40/(cnt+1)) ? 0 : 1)*/rnd(cnt) ; /* nerf to semi-permanent Elbereths --Amy */
			if (rn2(5)) cnt /= 2;
		}
		if (isfriday && !rn2(3)) cnt *= 2;

		wipeout_text(ep->engr_txt, (int)cnt, 0);
		while(ep->engr_txt[0] == ' ')
			ep->engr_txt++;
		if(!ep->engr_txt[0]) del_engr(ep);
	    }
	}

      if (ep && ep->engr_type == BURN && sengr_at("Elbereth", x, y) && !rn2(isfriday ? 1000 : 10000) ) {

	/* Burned Elbereth engravings will no longer be truly permanent. --Amy */

		pline(FunnyHallu ? "Flaming words rising up into the air... this must be MAGIC!" : "Suddenly, the burned engraving disappears into the ether!");
	    del_engr(ep);
	    ep = (struct engr *)0;

	}

      if (sobj_at(SCR_SCARE_MONSTER, x, y) && !rn2(isfriday ? 10 : issoviet ? 1000 : 50) ) {

	/* scare monster scrolls might randomly disappear --Amy */

		if (!rn2(10)) pline(issoviet ? "Vy chuvstvuyete nekotoruyu ekstremal'nuyu zharu! Popytka Soveta SSSR sokhranit' ispug monstr svitkov obratnyy effekt, kha-kha-kha!" : FunnyHallu ? "You get a hot feeling! Maybe some sexy girls are around here somewhere?" : "You sense some sort of heat for a moment!");
		(void) burn_floor_paper(x, y, TRUE, FALSE); /* The actual chance of this removing the scroll is about 3%. */

	}



}

#endif /* OVL1 */
#ifdef OVL2

boolean
sense_engr_at(x,y,read_it)
register int x,y;
boolean read_it; /* Read any sensed engraving */
{
	register struct engr *ep = engr_at(x,y);
	register int	sensed = 0;
	char buf[BUFSZ];
	
	/* Sensing an engraving does not require sight,
	 * nor does it necessarily imply comprehension (literacy).
	 */
	if(ep && ep->engr_txt[0]) {
	    switch(ep->engr_type) {
	    case DUST:
		if(!Blind) {
			sensed = 1;
			pline("%s is written here on the %s.", Something,
				is_ice(x,y) ? "frost" : is_snow(x,y) ? "snow" : surface(x,y));
		}
		break;
	    case ENGRAVE:
	    case HEADSTONE:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
			pline("%s is engraved here on the %s.",
				Something,
				surface(x,y));
		}
		break;
	    case BURN:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
			pline("Some text has been %s into the %s here.",
				is_ice(x,y) ? "melted" : is_snow(x,y) ? "melted" : "burned",
				surface(x,y));
		}
		break;
	    case MARK:
		if(!Blind) {
			sensed = 1;
			pline("There's some graffiti on the %s here.",
				surface(x,y));
		}
		break;
	    case ENGR_BLOOD:
		/* "It's a message!  Scrawled in blood!"
		 * "What's it say?"
		 * "It says... `See you next Wednesday.'" -- Thriller
		 */
		if(!Blind) {
			sensed = 1;
			You("see a message scrawled in blood here.");
		}
		break;
	    default:
		impossible("%s is written in a very strange way.",
				Something);
		sensed = 1;
	    }
	    if (sensed && !read_it &&
			    flags.suppress_alert < FEATURE_NOTICE_VER(0,0,7)) {
		pline("Use \"r.\" to read it.");
		if(flags.run > 0) nomul(0, 0, FALSE);
	    } else if (sensed && read_it) {
	    	char *et;
	    	unsigned len, maxelen = BUFSZ - sizeof("You feel the words: \"\". ");
	    	len = strlen(ep->engr_txt);

		if (Race_if(PM_PLAYABLE_NEANDERTHAL)) {
			pline("You're illiterate! You can't read!");
			return FALSE;
		}

	    	if (len > maxelen) {
	    		(void)strncpy(buf,  ep->engr_txt, (int)maxelen);
			buf[maxelen] = '\0';
			et = buf;
		} else
			et = ep->engr_txt;

		/* If you can engrave an 'x', you can "read" it --ALI */
		if (len != 1 || (!index(et, 'x') && !index(et, 'X')))
			u.uconduct.literate++;

		You("%s: \"%s\".",
		      (Blind) ? "feel the words" : "read",  et);
		if(flags.run > 0) nomul(0, 0, FALSE);
		return TRUE;
	    }
	}
	return FALSE;
}

#endif /* OVL2 */
#ifdef OVLB

void
make_engr_at(x,y,s,e_time,e_type)
register int x,y;
register const char *s;
register long e_time;
register xchar e_type;
{
	register struct engr *ep;

	if ((ep = engr_at(x,y)) != 0)
	    del_engr(ep);
	ep = newengr(strlen(s) + 1);
	ep->nxt_engr = head_engr;
	head_engr = ep;
	ep->engr_x = x;
	ep->engr_y = y;
	ep->engr_txt = (char *)(ep + 1);
	strcpy(ep->engr_txt, s);
	/* engraving Elbereth shows wisdom */
	if (!in_mklev && !strcmp(s, "Elbereth")) exercise(A_WIS, TRUE);
	ep->engr_time = e_time;
	ep->engr_type = e_type > 0 ? e_type : rnd(N_ENGRAVE-1);
	ep->engr_lth = strlen(s) + 1;
}

/* delete any engraving at location <x,y> */
void
del_engr_at(x, y)
int x, y;
{
	register struct engr *ep = engr_at(x, y);

	if (ep) del_engr(ep);
}

/*
 *	freehand - returns true if player has a free hand
 */
int
freehand()
{
	if (uwep && uwep->oartifact == ART_WAR_S_SWORD) return 1;
	if (uwep && uwep->oartifact == ART_BARDICHE_ASSALT) return 1;
	if (uwep && uwep->oartifact == ART_DE_SID) return 1;
	if (uwep && uwep->oartifact == ART_OZYZEVPDWTVP) return 1;
	if (uwep && uwep->oartifact == ART_KATI_GAVE_YOU_THE_ENGLISH_) return 1;

	return(!uwep || !welded(uwep) ||
	   (!bimanual(uwep) && (!uarms || !uarms->cursed)));
/*	if ((uwep && bimanual(uwep)) ||
	    (uwep && uarms))
		return(0);
	else
		return(1);*/
}

/* function by Amy that also checks your skill; for prayer etc. purposes, you still count as not having a free hand
 * because skills should not make things worse, i.e. it should still be possible to uncurse a two-hander by praying */
int
freehandX()
{
	if (uwep && uwep->oartifact == ART_WAR_S_SWORD) return 1;
	if (uwep && uwep->oartifact == ART_BARDICHE_ASSALT) return 1;
	if (uwep && uwep->oartifact == ART_DE_SID) return 1;
	if (uwep && uwep->oartifact == ART_OZYZEVPDWTVP) return 1;
	if (uwep && uwep->oartifact == ART_KATI_GAVE_YOU_THE_ENGLISH_) return 1;

	if (!PlayerCannotUseSkills) {

		if (uwep && bimanual(uwep) && P_SKILL(P_TWO_HANDED_WEAPON) >= P_EXPERT) return 1;
		if (uwep && uarms && P_SKILL(P_SHIELD) >= P_EXPERT) return 1;

	}

	/* grim shield is worn over the shoulder, and therefore doesn't result in unusable hands */
	return(!uwep || !welded(uwep) ||
	   (!bimanual(uwep) && (!uarms || (uarms && (uarms->otyp == GRIM_SHIELD || Race_if(PM_THRALL) || Race_if(PM_URGOTH) || Race_if(PM_ZAUR) || Race_if(PM_WYLVAN) ) ) || !uarms->cursed)));
}

static NEARDATA const char styluses[] =
	{ ALL_CLASSES, ALLOW_NONE, TOOL_CLASS, WEAPON_CLASS, WAND_CLASS,
	  GEM_CLASS, RING_CLASS, 0 };

/* Mohs' Hardness Scale:
 *  1 - Talc		 6 - Orthoclase
 *  2 - Gypsum		 7 - Quartz
 *  3 - Calcite		 8 - Topaz
 *  4 - Fluorite	 9 - Corundum
 *  5 - Apatite		10 - Diamond
 *
 * Since granite is a igneous rock hardness ~ 7, anything >= 8 should
 * probably be able to scratch the rock.
 * Devaluation of less hard gems is not easily possible because obj struct
 * does not contain individual oc_cost currently. 7/91
 *
 * steel     -	5-8.5	(usu. weapon)
 * diamond    - 10			* jade	     -	5-6	 (nephrite)
 * ruby       -  9	(corundum)	* turquoise  -	5-6
 * sapphire   -  9	(corundum)	* opal	     -	5-6
 * topaz      -  8			* glass      - ~5.5
 * emerald    -  7.5-8	(beryl)		* dilithium  -	4-5??
 * aquamarine -  7.5-8	(beryl)		* iron	     -	4-5
 * garnet     -  7.25	(var. 6.5-8)	* fluorite   -	4
 * agate      -  7	(quartz)	* brass      -	3-4
 * amethyst   -  7	(quartz)	* gold	     -	2.5-3
 * jasper     -  7	(quartz)	* silver     -	2.5-3
 * onyx       -  7	(quartz)	* copper     -	2.5-3
 * moonstone  -  6	(orthoclase)	* amber      -	2-2.5
 */

/* return 1 if action took 1 (or more) moves, 0 if error or aborted */
int
doengrave()
{
	boolean dengr = FALSE;	/* TRUE if we wipe out the current engraving */
	boolean doblind = FALSE;/* TRUE if engraving blinds the player */
	boolean doknown = FALSE;/* TRUE if we identify the stylus */
	boolean eow = FALSE;	/* TRUE if we are overwriting oep */
	boolean jello = FALSE;	/* TRUE if we are engraving in slime */
	boolean ptext = TRUE;	/* TRUE if we must prompt for engrave text */
	boolean teleengr =FALSE;/* TRUE if we move the old engraving */
	boolean zapwand = FALSE;/* TRUE if we remove a wand charge */
	xchar type = DUST;	/* Type of engraving made */
	char buf[ENGBUFSZ];	/* Buffer for final/poly engraving text */
	char ebuf[ENGBUFSZ];	/* Buffer for initial engraving text */
	char qbuf[QBUFSZ];	/* Buffer for query text */
	char post_engr_text[ENGBUFSZ]; /* Text displayed after engraving prompt */
	const char *everb;	/* Present tense of engraving type */
	const char *eloc;	/* Where the engraving is (ie dust/floor/...) */
	char *sp;		/* Place holder for space count of engr text */
	int len;		/* # of nonspace chars of new engraving text */
	int maxelen;		/* Max allowable length of engraving text */
	struct engr *oep = engr_at(u.ux,u.uy);
				/* The current engraving */
	struct obj *otmp;	/* Object selected with which to engrave */
	char *writer;

	multi = 0;		/* moves consumed */
	nomovemsg = (char *)0;	/* occupation end message */

	boolean canengravefast = (Role_if(PM_ARTIST) || (uarmf && uarmf->oartifact == ART_GIVE_THE_ART_A_HOME));

	buf[0] = (char)0;
	ebuf[0] = (char)0;
	post_engr_text[0] = (char)0;
	maxelen = ENGBUFSZ; /* a maximum of 31 active Elbereths on a single square --Amy */
	if (is_demon(youmonst.data) || youmonst.data->mlet == S_VAMPIRE)
	    type = ENGR_BLOOD;

	/* Can the adventurer engrave at all? */

	if(u.uswallow) {
		if (is_animal(u.ustuck->data)) {
			pline("What would you write?  \"Jonah was here\"?");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return(0);
		} else if (is_whirly(u.ustuck->data)) {
			You_cant("reach the %s.", surface(u.ux,u.uy));
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return(0);
		} else
			jello = TRUE;
	} else if (is_lava(u.ux, u.uy)) {
		You_cant("write on the lava!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	} else if (is_styxriver(u.ux, u.uy)) {
		You_cant("write on the styx river!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	} else if (is_shiftingsand(u.ux, u.uy)) {
		You_cant("write on the shifting sand!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	} else if (is_waterypool(u.ux,u.uy) || is_watertunnel(u.ux,u.uy) || is_moorland(u.ux,u.uy) || IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		You_cant("write on the water!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}
	if(Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)/* in bubble */) {
		You_cant("write in thin air!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}
	if(IS_AIR(levl[u.ux][u.uy].typ)) {
		You_cant("write on an unsolid surface!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}
	if (cantwield(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
		You_cant("even hold anything!");

		if (yn("Do you want to try engraving anyway?") == 'y') {
			if (rn2(3) && !polyskillchance()) { 		
				pline("You failed to engrave anything.");
				if (!rn2(20)) badeffect();
				return 1;
			}
		}

		/* Just make them lose that turn. If they're surrounded by 20 killer bees and 10 large cats,
		   losing a turn while engraving might be enough to kill them anyway. No need for further penalties. --Amy */
		else {return(0);}
	}
	if (check_capacity((char *)0)) return (0);

	/* One may write with finger, or weapon, or wand, or..., or...
	 * Edited by GAN 10/20/86 so as not to change weapon wielded.
	 */

	otmp = getobj(styluses, "write with");
	if(!otmp) return(0);		/* otmp == zeroobj if fingers */

	if (otmp == &zeroobj) writer = makeplural(body_part(FINGER));
	else writer = xname(otmp);

	/* evil variant: very strong char might cause the wand to explode (by nosomebodies) */
	if (evilfriday && otmp && otmp->oclass == WAND_CLASS && ((ACURR(A_STR) >= 18 && !rn2(10)) || (ACURR(A_STR) >= 68 && !rn2(4)) || (ACURR(A_STR) >= 100 && !rn2(2)) ) ) {
		pline("Oops, you seem to have pressed the wand too hard with your fat %s...", makeplural(body_part(HAND)) );

		otmp->in_use = TRUE;	/* in case losehp() is fatal */
		pline("%s suddenly explodes!", The(xname(otmp)));
		wand_explode(otmp, FALSE); /* KABOOM - otmp is used up */

		exercise(A_STR, FALSE);
		return(0);

	}

	/* from *shudder* Nethack 3.6 - if the wand could explode when zapped, it can also explode when engraved --Amy */
	if (otmp && otmp->oclass == WAND_CLASS) {
		if(otmp->otyp == WAN_MISFIRE) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);  /* the wand blows up in your face! */
			exercise(A_STR, FALSE);
			return(0);
		}

		else if(otmp->cursed && !rn2(5)) {
			/* WAC made this rn2(5) from rn2(100)*/
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);  /* the wand blows up in your face! */
			exercise(A_STR, FALSE);
			return(1);
		}

		/* evil patch idea by jonadab: eroded wands have a chance of exploding */
		else if ( (otmp->oeroded > 2 || (otmp->oeroded2 > 2 && !(objects[(otmp)->otyp].oc_material == MT_COMPOST) ) ) && !rn2(5) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);  /* the wand blows up in your face! */
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ( (otmp->oeroded > 1 || (otmp->oeroded2 > 1 && !(objects[(otmp)->otyp].oc_material == MT_COMPOST) ) ) && !rn2(20) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);  /* the wand blows up in your face! */
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ( (otmp->oeroded > 0 || (otmp->oeroded2 > 0 && !(objects[(otmp)->otyp].oc_material == MT_COMPOST) ) ) && !rn2(80) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);  /* the wand blows up in your face! */
			exercise(A_STR, FALSE);
			return(1);
		}

		/* evil patch idea by jondab: zapping a wand while impaired can cause it to explode */
		else if ( Stunned && !rn2(StrongStun_resist ? 2000 : Stun_resist ? 200 : 20) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ( Confusion && !rn2(StrongConf_resist ? 15000 : Conf_resist ? 1500 : 150) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ( Numbed && !rn2(500) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ( Feared && !rn2(500) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ( Frozen && !rn2(30) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ( Burned && !rn2(300) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ( Dimmed && !rn2(1000) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ( Blind && !rn2(200) ) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);
			exercise(A_STR, FALSE);
			return(1);
		}
		else if ((MagicDeviceEffect || u.uprops[MAGIC_DEVICE_BUG].extrinsic || have_magicdevicestone()) && !rn2(10)) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);
			exercise(A_STR, FALSE);
			return(1);
		}

		else if(otmp->otyp == WAN_WONDER && !rn2(100)) {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);  /* the wand blows up in your face! */
			exercise(A_STR, FALSE);
			return(1);
	    /* WAC wands of lightning will also explode in your face*/
		} else if ((otmp->otyp == WAN_LIGHTNING || otmp->otyp == SPE_LIGHTNING) && (Underwater) && (!otmp->blessed))   {
			pline("%s suddenly explodes!", The(xname(otmp)));
			wand_explode(otmp, FALSE);	/* the wand blows up in your face! */
			exercise(A_STR, FALSE);
			return(1);
		}

	} /* wand can explode check */

	/* There's no reason you should be able to write with a wand
	 * while both your hands are tied up.
	 */
	if (!freehandX() && otmp != uwep && !otmp->owornmask) {
		You("have no free %s to write with!", body_part(HAND));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}

	if (isevilvariant && u.ustuck && !u.uswallow && !(uactivesymbiosis && sticks(&mons[u.usymbiote.mnum]) ) && !sticks(youmonst.data)) {
		You("are being held, and cannot engrave anything because your %s are being squeezed.", makeplural(body_part(HAND)) );
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);

	}

	if (jello) {
		You("tickle %s with your %s.", mon_nam(u.ustuck), writer);
		Your("message dissolves...");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}
	if (otmp->oclass != WAND_CLASS && !can_reach_floor()) {
		You_cant("reach the %s!", surface(u.ux,u.uy));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}
	if (IS_ALTAR(levl[u.ux][u.uy].typ)) {
		You("make a motion towards the altar with your %s.", writer);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		altar_wrath(u.ux, u.uy);
		return(0);
	}
	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (otmp == &zeroobj) { /* using only finger */
		You("would only make a small smudge on the %s.",
			surface(u.ux, u.uy));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	    } else if (!levl[u.ux][u.uy].disturbed) {
		You("disturb the undead!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		levl[u.ux][u.uy].disturbed = 1;
		(void) makemon(&mons[PM_GHOUL], u.ux, u.uy, NO_MM_FLAGS);
		exercise(A_WIS, FALSE);
		return(1);
	    }
	}

	/* SPFX for items */

	switch (otmp->oclass) {
	    default:
	    case AMULET_CLASS:
	    case IMPLANT_CLASS:
	    case CHAIN_CLASS:
	    case POTION_CLASS:
	    case COIN_CLASS:
		break;

	    case RING_CLASS:
		/* "diamond" rings and others should work */
	    case GEM_CLASS:
		/* diamonds & other hard gems should work */
		if (objects[otmp->otyp].oc_tough) {
			type = ENGRAVE;
			break;
		}
		break;

	    case ARMOR_CLASS:
		if (is_boots(otmp)) {
			type = DUST;
			break;
		}
		/* fall through */
	    /* Objects too large to engrave with */
	    case BALL_CLASS:
	    case ROCK_CLASS:
		You_cant("engrave with such a large object!");
		ptext = FALSE;
		break;

	    /* Objects too silly to engrave with */
	    case FOOD_CLASS:
	    case SCROLL_CLASS:
	    case SPBOOK_CLASS:
		Your("%s would get %s.", xname(otmp),
			is_ice(u.ux,u.uy) ? "all frosty" : is_snow(u.ux,u.uy) ? "all frosty" : "too dirty");
		ptext = FALSE;
		break;

	    case RANDOM_CLASS:	/* This should mean fingers */
		break;

	    /* The charge is removed from the wand before prompting for
	     * the engraving text, because all kinds of setup decisions
	     * and pre-engraving messages are based upon knowing what type
	     * of engraving the wand is going to do.  Also, the player
	     * will have potentially seen "You wrest .." message, and
	     * therefore will know they are using a charge.
	     */

		/* edit by Amy - you could wrest without consuming time. Thus I won't call zappable(otmp) for empty wands.
		 * Wresting must be done by actually zapping the wand now, which will always take time. */

	    case WAND_CLASS:
		if (otmp->spe > 0) {
		if (zappable(otmp)) {
		    check_unpaid(otmp);
		    zapwand = TRUE;
		    if (Levitation) ptext = FALSE;

		    switch (otmp->otyp) {
		    /* DUST wands */
		    default:
			break;

			/* NODIR wands */
		    case WAN_LIGHT:
		    case WAN_SECRET_DOOR_DETECTION:
		    case WAN_TRAP_DISARMING:
		    case WAN_CREATE_MONSTER:
		    case WAN_CREATE_FAMILIAR:
		    case WAN_SUMMON_UNDEAD:
		    case WAN_CREATE_HORDE:
		    case WAN_ENLIGHTENMENT:
		    case WAN_ENTRAPPING:
		    case WAN_TELE_LEVEL:
		    case WAN_GENOCIDE:
		    case WAN_STINKING_CLOUD:
		    case WAN_TIME_STOP:
		    case WAN_MAGIC_MAPPING:
		    case WAN_DARKNESS:
		    case WAN_TRAP_CREATION:
		    case WAN_SUMMON_SEXY_GIRL:
		    case WAN_MANA:
		    case WAN_GAIN_LEVEL:
		    case WAN_BAD_EFFECT:
		    case WAN_DETECT_MONSTERS:
		    case WAN_OBJECTION:
		    case WAN_IDENTIFY:
		    case WAN_LEVITATION:
		    case WAN_DEBUGGING:
		    case WAN_SPELLBINDER:
		    case WAN_INERTIA_CONTROL:
		    case WAN_STERILIZE:
		    case WAN_RESTORATION:
		    case WAN_REMOVE_CURSE:
		    case WAN_PUNISHMENT:
		    case WAN_WISHING:
		    case WAN_ACQUIREMENT:
		    case WAN_BUGGING:
		    case WAN_CHARGING:
		    case WAN_CURSE_ITEMS:
		    case WAN_AMNESIA:
		    case WAN_BLEEDING:
		    case WAN_UNDRESSING:
		    case WAN_DISENCHANTMENT:
		    case WAN_CONTAMINATION:
		    case WAN_TREMBLING:
		    case WAN_BAD_LUCK:
		    case WAN_REMOVE_RESISTANCE:
		    case WAN_CORROSION:
		    case WAN_MEDIUM_HEALING:
		    case WAN_STRONG_HEALING:
		    case WAN_SUPER_HEALING:
		    case WAN_TELEPORT_SELF:
		    case WAN_CHAOS_TERRAIN:
		    case WAN_FLEECY_TERRAIN:
		    case WAN_STAT_REDUCTION:
		    case WAN_FUMBLING:
		    case WAN_TIDAL_WAVE:
		    case WAN_SUMMON_ELM:
		    case WAN_STARVATION:
		    case WAN_CONFUSION:
		    case WAN_SLIMING:
		    case WAN_DRAIN_MANA:
		    case WAN_FINGER_BENDING:
		    case WAN_LYCANTHROPY:
		    case WAN_IMMOBILITY:
		    case WAN_INSANITY:
		    case WAN_BAD_EQUIPMENT:
		    case WAN_EGOISM:
		    case WAN_SIN:
		    case WAN_LAVA:
			zapnodir(otmp);
			break;

			/* IMMEDIATE wands */
			/* If wand is "IMMEDIATE", remember to affect the
			 * previous engraving even if turning to dust.
			 */
		    case WAN_STRIKING:
			strcpy(post_engr_text,
			"The wand unsuccessfully fights your attempt to write!"
			);
			break;
		    case WAN_DISINTEGRATION:
		    case WAN_DISINTEGRATION_BEAM:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s cease to exist!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_GRAVITY_BEAM:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs are driven into the %s!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_STONE_TO_FLESH:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s are eagerly eating some meat!", surface(u.ux, u.uy));
			}
			break;
		    case WAN_STONING:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s turn into gravel!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_DREAM_EATER:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s seem unaffected.", /* because they're not asleep --Amy */
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_BUBBLEBEAM:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s are covered with bubbles!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_GOOD_NIGHT:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s look very drowsy!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_STUN_MONSTER:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s start moving in strange patterns!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_ACID:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s seem to be covered with goo!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_SLUDGE:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s seem to be covered with litter!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_DESLEXIFICATION:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s seem unaffected.",
				   surface(u.ux, u.uy));
		/* bugs on the floor are not a slex feature, and therefore the wand does nothing to them --Amy */
			}
			break;
		    case WAN_WIND:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s are blown away!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_SOLAR_BEAM:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s shine brightly for a moment!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_AURORA_BEAM:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s are illuminated with rainbow colors!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_PSYBEAM:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s seem utterly confused!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_NETHER_BEAM:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s are killed!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_HYPER_BEAM:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s are vaporized!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_SLOW_MONSTER:
		    case WAN_INERTIA:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s slow down!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_SPEED_MONSTER:
		    case WAN_HASTE_MONSTER:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s speed up!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_HEALING:
		    case WAN_EXTRA_HEALING:
		    case WAN_FULL_HEALING:
		    case WAN_INCREASE_MAX_HITPOINTS:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s look healthier!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_FEAR:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s run away!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_MAKE_VISIBLE:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s are clearly visible!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_POLYMORPH:
		    case WAN_MUTATION:
			if(oep)  {
			    if (!Blind) {
				type = (xchar)0;	/* random */
				(void) random_engraving(buf);
			    }
			    dengr = TRUE;
			}
			break;
		    case WAN_DRAINING:	/* KMH */
		    case WAN_TIME:
		    case WAN_REDUCE_MAX_HITPOINTS:
			if (oep) {
			    /*
			     * [ALI] Wand of draining give messages like
			     * either polymorph or cancellation/make
			     * invisible depending on whether the
			     * old engraving is completely wiped or not.
			     * Note: Blindness has slightly different
			     * effect than with wand of polymorph.
			     */
			    u_wipe_engr(5);
			    oep = engr_at(u.ux,u.uy);
			    if (!Blind) {
				if (!oep)
				    pline_The("engraving on the %s vanishes!",
				      surface(u.ux,u.uy));
				else {
				    strcpy(buf, oep->engr_txt);
				    dengr = TRUE;
				}
			    }
			}
			break;
		    case WAN_CLONE_MONSTER:
			if (oep) {
			    if (!Blind)
				    pline_The("engraving is blurred, and you think you can see it twice!");
			    }
			break;
		    case WAN_NOTHING:
		    case WAN_UNDEAD_TURNING:
		    case WAN_OPENING:
		    case WAN_LOCKING:
		    case WAN_PROBING:
		    case WAN_MISFIRE: /* fire or lightning message in FHS... but I decided to be cruel :-P --Amy */
			break;

		    case WAN_VENOM_SCATTERING:

			if (!Blind) pline("A venom leaves the wand top.");

			break;

			/* RAY wands */
		    case WAN_MAGIC_MISSILE:
		    case WAN_SHARE_PAIN:
			ptext = TRUE;
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The %s is riddled by bullet holes!",
				   surface(u.ux, u.uy));
			}
			break;

		    /* can't tell sleep from death - Eric Backus */
		    case WAN_SLEEP:
		    case WAN_CHLOROFORM:
		    case WAN_DEATH:
		    case WAN_PARALYSIS:
			if (!Blind) {
			   sprintf(post_engr_text,
				   "The bugs on the %s stop moving!",
				   surface(u.ux, u.uy));
			}
			break;

		    case WAN_COLD:
			if (!Blind)
			    strcpy(post_engr_text,
				"A few ice cubes drop from the wand.");
			if(!oep || (oep->engr_type != BURN))
			    break;
		    case WAN_ICE_BEAM:
			if (!Blind)
			    strcpy(post_engr_text,
				"A steady stream of ice shoots from the wand.");
			if(!oep || (oep->engr_type != BURN))
			    break;
		    case WAN_CANCELLATION:
		    case WAN_MAKE_INVISIBLE:
		    case WAN_BANISHMENT:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
				pline_The("engraving on the %s vanishes!",
					surface(u.ux,u.uy));
			    dengr = TRUE;
			}
			break;
		    case WAN_TELEPORTATION:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
				pline_The("engraving on the %s vanishes!",
					surface(u.ux,u.uy));
			    teleengr = TRUE;
			}
			break;

		    /* type = ENGRAVE wands */
		    case WAN_DIGGING:
			ptext = TRUE;
			type  = ENGRAVE;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("This %s is a wand of digging!",
				xname(otmp));
			    doknown = TRUE;
			}
			if (!Blind)
			    strcpy(post_engr_text,
				IS_GRAVE(levl[u.ux][u.uy].typ) ?
				"Chips fly out from the headstone." :
				is_ice(u.ux,u.uy) ?
				"Ice chips fly up from the ice surface!" :
				is_snow(u.ux,u.uy) ?
				"Snowflakes fly up from the ice surface!" :
				"Gravel flies up from the floor.");
			else
			    strcpy(post_engr_text, "You hear drilling!");
			break;

		    case WAN_POISON:	/*new wand [Sakusha]*/
			ptext = TRUE;
			type  = MARK;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("This %s is a wand of poison!", xname(otmp));
			    doknown = TRUE;
			}
			break;

		    case WAN_TOXIC:	/*new wand [Sakusha]*/
			ptext = TRUE;
			type  = MARK;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("This %s is a wand of toxic!", xname(otmp));
			    doknown = TRUE;
			}
			break;

		    /* type = BURN wands */
		    case WAN_FIRE:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			if (flags.verbose)
			    pline("This %s is a wand of fire!", xname(otmp));
			    doknown = TRUE;
			}
			strcpy(post_engr_text,
				Blind ? "You feel the wand heat up." :
					"Flames fly from the wand.");
			break;
		    case WAN_INFERNO:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			if (flags.verbose)
			    pline("This %s is a wand of inferno!", xname(otmp));
			    doknown = TRUE;
			}
			strcpy(post_engr_text,
				Blind ? "You feel the wand heat up." :
					"Flames fly from the wand.");
			break;
		    case WAN_CHROMATIC_BEAM:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			if (flags.verbose)
			    pline("This %s is a wand of chromatic beam!", xname(otmp));
			    doknown = TRUE;
			}
			strcpy(post_engr_text,
				Blind ? "You feel the wand trembling while you engrave." :
					"Multicolored beams fly from the wand.");
			break;
		    case WAN_FIREBALL:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			if (flags.verbose)
			    pline("This %s is a wand of fireballs!", xname(otmp));
			    doknown = TRUE;
			}
			strcpy(post_engr_text,
				Blind ? "You feel the wand heat up." :
					"Flames fly from the wand.");
			break;
		    case WAN_LIGHTNING:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("This %s is a wand of lightning!",
					xname(otmp));
			    doknown = TRUE;
			}
			if (!Blind) {
			    strcpy(post_engr_text,
				    "Lightning arcs from the wand.");
			    doblind = TRUE;
			} else
			    strcpy(post_engr_text, "You hear crackling!");
			break;
		    case WAN_THUNDER:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("This %s is a wand of thunder!",
					xname(otmp));
			    doknown = TRUE;
			}
			if (!Blind) {
			    strcpy(post_engr_text,
				    "Lightning arcs from the wand.");
			    doblind = TRUE;
			} else
			    strcpy(post_engr_text, "You hear crackling!");
			break;

		    /* type = MARK wands */
		    /* type = ENGR_BLOOD wands */
		    }
		} else /* end if zappable */
		    if (!can_reach_floor()) {
			You_cant("reach the %s!", surface(u.ux,u.uy));
			return(0);
		    }
			break;
		} else /* end if obj->spe > 0 */
		    if (!can_reach_floor()) {
			You_cant("reach the %s!", surface(u.ux,u.uy));
			return(0);
		    }
		break;

	    case WEAPON_CLASS:
		if(is_blade(otmp)) {
		    if ((int)otmp->spe > -3)
			type = ENGRAVE;
		    else
			Your("%s too dull for engraving.", aobjnam(otmp,"are"));
		}
		break;

	    case TOOL_CLASS:
		if(otmp == ublindf) {
		    pline(
		"That is a bit difficult to engrave with, don't you think?");
		    return(0);
		}

		if (is_lightsaber(otmp)) {
		    if (otmp->lamplit) type = /*BURN*/ENGRAVE;
		    else Your("%s is deactivated!", aobjnam(otmp,"are"));
		} else
		switch (otmp->otyp)  {
		    case MAGIC_MARKER:
		    case FELT_TIP_MARKER:
			if (otmp->spe <= 0)
			    Your("marker has dried out.");
			else
			    type = MARK;
			break;
		    case TOWEL:
			/* Can't really engrave with a towel */
			ptext = FALSE;
			if (oep)
			    if ((oep->engr_type == DUST ) ||
				(oep->engr_type == ENGR_BLOOD) ||
				(oep->engr_type == MARK )) {
				if (!Blind)
				    You("wipe out the message here.");
				else
				    Your("%s %s %s.", xname(otmp),
					 otense(otmp, "get"),
					 is_ice(u.ux,u.uy) ?
					 "frosty" : is_snow(u.ux,u.uy) ?
					 "frosty" : "dusty");
				dengr = TRUE;
			    } else
				Your("%s can't wipe out this engraving.",
				     xname(otmp));
			else
			    Your("%s %s %s.", xname(otmp), otense(otmp, "get"),
				  is_ice(u.ux,u.uy) ? "frosty" : is_snow(u.ux,u.uy) ? "snowy" : "dusty");
			break;
		    default:
			break;
		}
		break;

	    case VENOM_CLASS:
		    pline(FunnyHallu ? "That would require some huge hands as it's just smearing big letters everywhere." : "Writing a poison pen letter??"); /* why the heck was this wizard-mode only??? --Amy */
		    break;
	    case ILLOBJ_CLASS:
		impossible("You're engraving with an illegal object!");
		break;
	}

	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (type == ENGRAVE || type == 0)
		type = HEADSTONE;
	    else {
		/* ensures the "cannot wipe out" case */
		type = DUST;
		dengr = FALSE;
		teleengr = FALSE;
		buf[0] = (char)0;
	    }
	}

	/* End of implement setup */

	/* Identify stylus */
	if (doknown) {
	    makeknown(otmp->otyp);
	    more_experienced(0,10);
	}

	if (teleengr) {
	    rloc_engr(oep);
	    oep = (struct engr *)0;
	}

	if (dengr) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Something has changed the engraving here */
	if (*buf) {
	    make_engr_at(u.ux, u.uy, buf, moves, type);
	    pline_The("engraving looks different now.");
	    ptext = FALSE;
	}

	if (zapwand && (otmp->spe < 0)) {
	    pline("%s %sturns to dust.",
		  The(xname(otmp)), Blind ? "" : "glows violently, then ");
	    if (!IS_GRAVE(levl[u.ux][u.uy].typ))
		You("are not going to get anywhere trying to write on the %s with your dust.",
		    is_ice(u.ux,u.uy) ? "frost" : is_snow(u.ux,u.uy) ? "snow" : surface(u.ux, u.uy));
	    useup(otmp);
	    ptext = FALSE;
	}

	/* Wand of tele level can crash of course. Don't allow it to engrave, then. --Amy */
	if (otmp && otmp->otyp == WAN_TELE_LEVEL) {
		ptext = FALSE;
		pline("Somehow, this wand refuses to engrave anything.");
	}

	if (!ptext) {		/* Early exit for some implements. */
	    if (otmp && otmp->oclass == WAND_CLASS && !can_reach_floor())
		You_cant("reach the %s!", surface(u.ux,u.uy));
	    return(1);
	}

	/* Special effects should have deleted the current engraving (if
	 * possible) by now.
	 */

	if (oep) {
	    register char c = 'n';

	    /* Give player the choice to add to engraving. */

	    if (type == HEADSTONE) {
		/* no choice, only append */
		c = 'y';
	    } else if ( (type == oep->engr_type) && (!Blind ||
		 (oep->engr_type == BURN) || (oep->engr_type == ENGRAVE)) ) {
		c = yn_function("Do you want to add to the current engraving?",
				ynqchars, 'y');
		if (c == 'q') {
		    pline("%s", Never_mind);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually I do mind...");
				badeffect();
			}
		    return(0);
		}
	    }

	    if (c == 'n' || Blind) {

		if( (oep->engr_type == DUST) || (oep->engr_type == ENGR_BLOOD) ||
		    (oep->engr_type == MARK) ) {
		    if (!Blind) {
			You("wipe out the message that was %s here.",
			    ((oep->engr_type == DUST)  ? "written in the dust" :
			    ((oep->engr_type == ENGR_BLOOD) ? "scrawled in blood"   :
							 "written")));
			del_engr(oep);
			oep = (struct engr *)0;
		    } else
		   /* Don't delete engr until after we *know* we're engraving */
			eow = TRUE;
		} else
		    if ( (type == DUST) || (type == MARK) || (type == ENGR_BLOOD) ) {
			You(
			 "cannot wipe out the message that is %s the %s here.",
			 oep->engr_type == BURN ?
			   (is_ice(u.ux,u.uy) ? "melted into" : is_snow(u.ux,u.uy) ? "melted into" : "burned into") :
			   "engraved in", surface(u.ux,u.uy));
			return(1);
		    } else
			if ( (type != oep->engr_type) || (c == 'n') ) {
			    if (!Blind || can_reach_floor())
				You("will overwrite the current message.");
			    eow = TRUE;
			}
	    }
	}

	eloc = surface(u.ux,u.uy);
	switch(type){
	    default:
		everb = (oep && !eow ? "add to the weird writing on" :
				       "write strangely on");
		break;
	    case DUST:
		everb = (oep && !eow ? "add to the writing in" :
				       "write in");
		eloc = (is_ice(u.ux,u.uy) ? "frost" : is_snow(u.ux,u.uy) ? "snow" : surface(u.ux,u.uy));
		break;
	    case HEADSTONE:
		everb = (oep && !eow ? "add to the epitaph on" :
				       "engrave on");
		break;
	    case ENGRAVE:
		everb = (oep && !eow ? "add to the engraving in" :
				       "engrave in");
		break;
	    case BURN:
		everb = (oep && !eow ?
			( is_ice(u.ux,u.uy) ? "add to the text melted into" :
			 is_snow(u.ux,u.uy) ? "add to the text melted into" :
					      "add to the text burned into") :
			( is_ice(u.ux,u.uy) ? "melt into" : is_snow(u.ux,u.uy) ? "melt into" : "burn into"));
		break;
	    case MARK:
		everb = (oep && !eow ? "add to the graffiti on" :
				       "scribble on");
		break;
	    case ENGR_BLOOD:
		everb = (oep && !eow ? "add to the scrawl on" :
				       "scrawl on");
		break;
	}

	/* Tell adventurer what is going on */
	if (otmp != &zeroobj)
	    You("%s the %s with %s.", everb, eloc, doname(otmp));
	else
	    You("%s the %s with your %s.", everb, eloc,
		makeplural(body_part(FINGER)));

	/* Prompt for engraving! */
	sprintf(qbuf,"What do you want to %s the %s here?", everb, eloc);
	getlin(qbuf, ebuf);

	/* Count the actual # of chars engraved not including spaces */
	len = strlen(ebuf);
	for (sp = ebuf; *sp; sp++) if (isspace(*sp)) len -= 1;

	if (len == 0 || index(ebuf, '\033')) {
	    if (zapwand) {
		if (!Blind)
		    pline("%s, then %s.",
			  Tobjnam(otmp, "glow"), otense(otmp, "fade"));
		return(1);
	    } else {
		pline("%s", Never_mind);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually I do mind...");
			badeffect();
		}
		return(0);
	    }
	}

	/* A single `x' is the traditional signature of an illiterate person */
	if (len != 1 || (!index(ebuf, 'x') && !index(ebuf, 'X'))) {

		if (Race_if(PM_PLAYABLE_NEANDERTHAL)) {
			pline("You're illiterate, and therefore you can't write that!");
			return 0;
		}

		u.uconduct.literate++;
	}

	/* Evil Variant by ais523: the first Elbereth will always misengrave */
	if ((evilfriday || (uarmg && uarmg->oartifact == ART_GANTULETS_OF_MISPEALING)) && strlen(ebuf) > 7) for (sp = ebuf; *sp; sp++) {

		if (*sp == 'e' || *sp == 'E') {
			sp++;
			if (*sp && (*sp == 'l' || *sp == 'L')) {
				sp++;
				if (*sp && (*sp == 'b' || *sp == 'B')) {
					sp++;
					if (*sp && (*sp == 'e' || *sp == 'E')) {
						sp++;
						if (*sp && (*sp == 'r' || *sp == 'R')) {
							sp++;
							if (*sp && (*sp == 'e' || *sp == 'E')) {
								sp++;
								if (*sp && (*sp == 't' || *sp == 'T')) {
									sp++;
									if (*sp && (*sp == 'h' || *sp == 'H')) {
										*sp = 's';
										sp--;
										*sp = 'e';
										sp--;
										*sp = 'l';
										sp--;
										*sp = 'o';
										sp--;
										*sp = 'h';
										sp--;
										*sp = 's';
										sp--;
										*sp = 's';
										sp--;
										*sp = 'A';
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/* Mix up engraving if surface or state of mind is unsound.
	   Note: this won't add or remove any spaces. */

	/* The game is difficult enough already. Let's improve the player's chances to engrave. --Amy */

	for (sp = ebuf; *sp; sp++) {
	    if (isspace(*sp)) continue;
	    if ( (rn2(5) || !Role_if(PM_ARTIST)) && ((type == DUST || type == ENGR_BLOOD || isfriday) && !(uwep && uwep->oartifact == ART_PINSELFLINSELING) && (ishaxor ? !rn2(40) : !rn2(30) ) ) || (isfriday && !rn2(30)) ||
		    (Blind && (ishaxor ? !rn2(35) : !rn2(25)) ) || 
			(Confusion && (ishaxor ? !rn2(30) : !rn2(20)) ) ||
			(Burned && (ishaxor ? !rn2(32) : !rn2(22)) ) ||
			(Numbed && (ishaxor ? !rn2(38) : !rn2(28)) ) ||
			(Dimmed && (ishaxor ? !rn2(35) : !rn2(25)) ) ||
			(Feared && (ishaxor ? !rn2(40) : !rn2(30)) ) ||
		    (Stunned && (ishaxor ? !rn2(25) : !rn2(15)) ) ||
			(Frozen && (ishaxor ? !rn2(20) : !rn2(10)) ) ||
			(Glib && (ishaxor ? !rn2(10) : !rn2(5)) ) ||
			(Hallucination && (ishaxor ? !rn2(28) : !rn2(18))) )
		*sp = ' ' + rnd(96 - 2);	/* ASCII '!' thru '~'
						   (excludes ' ' and DEL) */

		if (uarmg && uarmg->oartifact == ART_GANTULETS_OF_MISPEALING && rn2(2)) *sp = ' ' + rnd(96 - 2);

	}

	/* Previous engraving is overwritten */
	if (eow) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Figure out how long it took to engrave, and if player has
	 * engraved too much. Artists can engrave much faster than other characters.
	 */
	switch(type){
	    default:
		multi = -(len/(canengravefast ? 20 : 10) );
		if (multi) nomovemsg = "You finish your weird engraving.";
		break;
	    case DUST:
		multi = -(len/(canengravefast ? 20 : 10) );
		if (multi) nomovemsg = "You finish writing in the dust.";
		if (tech_inuse(T_POISON_PEN_LETTER) && otmp && otmp->oclass == VENOM_CLASS) {
			if (len > 50) (void) create_gas_cloud(u.ux, u.uy, 6, 20);
			else if (len > 30) (void) create_gas_cloud(u.ux, u.uy, 5, 15);
			else if (len > 20) (void) create_gas_cloud(u.ux, u.uy, 4, 12);
			else if (len > 10) (void) create_gas_cloud(u.ux, u.uy, 3, 8);
			else (void) create_gas_cloud(u.ux, u.uy, 2, 4);
		}
		break;
	    case HEADSTONE:
	    case ENGRAVE:
		multi = -(len/(canengravefast ? 20 : 10) );
		if (otmp->otyp == WAN_DIGGING) multi /= 5; /* otherwise engraving with them would be useless --Amy */
		if ((otmp->oclass == WEAPON_CLASS) && !stack_too_big(otmp) &&
		    ( ((otmp->otyp != ATHAME) && (otmp->otyp != MERCURIAL_ATHAME) && (!is_lightsaber(otmp))) || otmp->cursed)) {
		    multi = -len;
		    maxelen = ((otmp->spe + 3) * 2) + 1;
			/* -2 = 3, -1 = 5, 0 = 7, +1 = 9, +2 = 11
			 * Note: this does not allow a +0 anything (except
			 *	 an athame) to engrave "Elbereth" all at once.
			 *	 However, you could now engrave "Elb", then
			 *	 "ere", then "th".
			 */
		    Your("%s dull.", aobjnam(otmp, "get"));
		    if (otmp->unpaid) {
			struct monst *shkp = shop_keeper(*u.ushops);
			if (shkp) {
			    You("damage it, you pay for it!");
			    bill_dummy_object(otmp);
			}
		    }
		    if (len > maxelen) {
			multi = -maxelen;
			otmp->spe = -3;
		    } else if (len > 1)
			otmp->spe -= len >> 1;
		    else otmp->spe -= 1; /* Prevent infinite engraving */
		} else
		    if ( (otmp->oclass == RING_CLASS) ||
			 (otmp->oclass == GEM_CLASS) )
			multi = -len;
		if (multi) nomovemsg = "You finish engraving.";
		break;
	    case BURN:
		multi = -(len/(canengravefast ? 20 : 10) );
		if (multi)
		    nomovemsg = is_ice(u.ux,u.uy) ?
			"You finish melting your message into the ice.": is_snow(u.ux,u.uy) ?
			"You finish melting your message into the snow.":
			"You finish burning your message into the floor.";
		break;
	    case MARK:
		multi = -(len/(canengravefast ? 20 : 10) );
		if ((otmp->oclass == TOOL_CLASS) &&
		    (otmp->otyp == MAGIC_MARKER || otmp->otyp == FELT_TIP_MARKER)) {
		    maxelen = (otmp->spe) * /*2*/8; /* one charge / 2 letters */ /* Amy edit - one charge per 8 letters */
		    if (len > maxelen) {
			Your("marker dries out.");
			otmp->spe = 0;
			multi = -(maxelen/10);
		    } else if (!(otmp->oartifact == ART_SCRABBLE_BABBLE) || !rn2(10))
			if (len > 7) otmp->spe -= (len / 8);
			else otmp->spe -= 1; /* Prevent infinite grafitti */
		}
		if (multi) nomovemsg = "You finish defacing the dungeon.";
		break;
	    case ENGR_BLOOD:
		multi = -(len/(canengravefast ? 20 : 10) );
		if (multi) nomovemsg = "You finish scrawling.";
		break;
	}

	/* Chop engraving down to size if necessary */
	if (len > maxelen) {
	    for (sp = ebuf; (maxelen && *sp); sp++)
		if (!isspace((int)*sp)) maxelen--;
	    if (!maxelen && *sp) {
		*sp = (char)0;
		if (multi) nomovemsg = "You cannot write any more.";
		You("only are able to write \"%s\"", ebuf);
	    }
	}

	/* Add to existing engraving */
	if (oep) strcpy(buf, oep->engr_txt);

	(void) strncat(buf, ebuf, (ENGBUFSZ - (int)strlen(buf) - 1));

	make_engr_at(u.ux, u.uy, buf, (moves - multi), type);

	if (post_engr_text[0]) pline("%s", post_engr_text);

	if (doblind && !resists_blnd(&youmonst)) {
	    You("are blinded by the flash!");
	    make_blinded((long)rnd(20),FALSE);
	    if (!Blind) Your("%s", vision_clears);
	}

	return(1);
}

void
save_engravings(fd, mode)
int fd, mode;
{
	register struct engr *ep = head_engr;
	register struct engr *ep2;
	unsigned no_more_engr = 0;

	while (ep) {
	    ep2 = ep->nxt_engr;
	    if (ep->engr_lth && ep->engr_txt[0] && perform_bwrite(mode)) {
		bwrite(fd, (void *)&(ep->engr_lth), sizeof(ep->engr_lth));
		bwrite(fd, (void *)ep, sizeof(struct engr) + ep->engr_lth);
	    }
	    if (release_data(mode))
		dealloc_engr(ep);
	    ep = ep2;
	}
	if (perform_bwrite(mode))
	    bwrite(fd, (void *)&no_more_engr, sizeof no_more_engr);
	if (release_data(mode))
	    head_engr = 0;
}

void
rest_engravings(fd)
int fd;
{
	register struct engr *ep;
	unsigned lth;

	head_engr = 0;
	while(1) {
		mread(fd, (void *) &lth, sizeof(unsigned));
		if(lth == 0) return;
		ep = newengr(lth);
		mread(fd, (void *) ep, sizeof(struct engr) + lth);
		ep->nxt_engr = head_engr;
		head_engr = ep;
		ep->engr_txt = (char *) (ep + 1);	/* Andreas Bormann */
		/* mark as finished for bones levels -- no problem for
		 * normal levels as the player must have finished engraving
		 * to be able to move again */
		ep->engr_time = moves;
	}
}

void
del_engr(ep)
register struct engr *ep;
{
	if (ep == head_engr) {
		head_engr = ep->nxt_engr;
	} else {
		register struct engr *ept;

		for (ept = head_engr; ept; ept = ept->nxt_engr)
		    if (ept->nxt_engr == ep) {
			ept->nxt_engr = ep->nxt_engr;
			break;
		    }
		if (!ept) {
		    impossible("Error in del_engr?");
		    return;
		}
	}
	dealloc_engr(ep);
}

/* randomly relocate an engraving */
void
rloc_engr(ep)
struct engr *ep;
{
	int tx, ty, tryct = 200;

	do  {
	    if (--tryct < 0) return;
	    tx = rn1(COLNO-3,2);
	    ty = rn2(ROWNO);
	} while (engr_at(tx, ty) ||
		!goodpos(tx, ty, (struct monst *)0, 0));

	ep->engr_x = tx;
	ep->engr_y = ty;
}


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
"Fool me once, shame on you. Fool me twice, ...",
"For you, scum; only 700,000,000 for this grave reserve.",
"GE8BT",
"Game over, man. Game over.",
"Ganked again...",
"George W. Bush had everything and look where he ended up. No.. not up there u idiot... down here.",
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
	"Quidquid Latine dictum sit, altum videtur.",

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
"Here lies Dolores Umbridge. Shot in the head by Hermione's Avada Kedavra death ray. The Harry Potter characters should stop acting stupid and kill Umbridge already!",
"Here lies Lord Voldemort. Killed by his own reflecting death ray.",
"Here lies Bellatrix Lestrange. She took a knife to the head.",
"Here lies Peeta Mellark. He took a knife to the head because really there's no reason for Katniss not to kill him. He shouldn't even have existed anymore in books 2 and 3!",
"Here lies Cato. Killed even though he and Katniss were the one true pairing.",
"Here lies Gale. He choked himself to death on a rope when he realized that Peeta, that asshole, would put his dick in Katniss' cunt.",
"Here lies Ron Weasley. Killed by brainlessness.",
"Here lies Vincent Crabbe. Killed by a demonic fire.",
"Here lies Dobby the elf. He died totally in vain, and because it seemed the book needed to get rid of 'unneccessary' characters.",
"Here lies Cedric Diggor-erm, Edward Cullen. Killed by an overdose of sparkle.",
"Here lies Edward Cullen. Killed by an angry mob for being an abusive son-of-a-bitch.",
"Here lies Christian Gray. Anastasia finally started to fight him and whipped him to death with his own belt.",
"Here lies Sirius Black. Cause of death: being too stupid to simply use the Avada Kedavra curse on Bellatrix.",
"Here lies Vernon Dursley. He fell out of his window and broke his neck. Really, why the heck would he live after that??? He fucking died! Dammit!",
"Here lies Dolores Umbridge. She was raped and successively killed by Bane the centaur. NO, one does NOT return from them alive. Just NO.",
"Here lies Albus Dumbledore. Killed by a cold-blooded murderer named Snape.",
"Here lies Severus Snape. One of many, many people in the Harry Potter series who died in vain",
"Here lies the main character of Rowling's 'adult book'. Killed by cutting herself. Seriously, why did I have to read that???",
"Here lies Alastor Mad-Eye Moody. He would have lived but it seems Harry Potter uses the fact that lots of corpses are presented for shock value.",
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
"Tangado a chadad!",
"Prepare to fire!",
"Hello again, poor Yorick.",
"Ok, ok, I get it.",
"YASD",
"This one's for the YASD hall of fame.",
"I must away, ere break of day...",
"I hear the tramp of doom.",
"To fly would be folly.",
"The wind came down from mountains cold.",
"Cut the bridges! To arms! To arms!",
"Aah!",
"O! wandering folk, the summons heed!",
"I go now to the halls of waiting.",
"This is a bitter adventure, if it must end so.",
"I leave now all gold and silver, and go where it is of no worth.",
"But sad or merry, I must leave it now. Farewell!",
"His armour is shivered, his splendour is humbled.",
"Roads go ever ever on.",
"Yet feet that wandering have gone, turn at last to home afar.",
"Presumed dead.",
"For blood ye shall render blood.",
"The Shadow does not hold sway yet.",
"...and in the Darkness bind them.",
"One Ring to rule them all, One Ring to find them...",
"Leithio i philinn!",
"Fire the arrows!",
"Sedho, hodo.",
"Be still, lie still.",
"There-is-no life in-the-cold, in-the-dark.",
"Here - in-the-void only death.",
"I have kept no hope for myself.",
"I Aear can ven na mar.",
"The Sea calls us home.",
"Aiya Earendil Elenion Ancalima!",
"Hail Earendil brightest of the Stars!",
"Farewell sweet earth and northern sky.",
"Mourn not overmuch! Mighty was the fallen.",
"When winter comes without a spring that I shall ever see.",
"O! Rowan dead, upon your head your hair is dry and grey.",
"Out of doubt, out of dark, to the day's rising.",
"Hope he rekindled, and in hope ended.",
"Over death, over dread, over doom lifted.",
"Out of loss, out of life, unto long glory.",
"Now for wrath, now for ruin and a red nightfall!",
"Death in the morning and at day's ending...",
"For into darkness fell his star...",
"Where'd everybody go?",
"Caveyard! Paveyard!",
"Can't we talk this over?",
"Thy nuncle was dead as a lump o' lead.",
"A bump o' the boot on the seat.",
"I stood upon the bridge alone.",
"Dark is the path appointed for thee.",
"Thy heart shall then rest in the forest no more.",
"The Winter comes, the bare and leafless Day.",
"Where now the horse and the rider?",
"Who shall gather the smoke of the dead wood burning?",
"For our days are ending and our years failing.",
"I saw him walk in empty lands, until he passed away.",
"His cloven shield, his broken sword, they to the water brought.",
"It is death, my boy: they have deceived me.",
"His limbs they laid to rest.",
"I will not say the Day is done, nor bid the Stars farewell.",
"The shadow lies upon my tomb...",
"My cup runneth over...",
"I have drained my cup to the bitter dregs.",
"Si man i yulma nin enquantuva?",
"Namarie!",
"Fell deeds awake: fire and slaughter!",
"Cold be hand and heart and bone...",
"...cold be sleep under stone.",
"Never more to wake on stony bed...",
"Till the Sun fails and the Moon is dead.",
"In the black wind the stars shall die...",
"Out into the barren lands.",
"Where is my spellbook of Hand of Doom?",
"But I have double resist!",
"I hate the Random Number Generator.",
"Oh. I didn't know it could resist that.",
"I knew I should have played with stupid monsters on.",
"I knew I shouldn't have let monsters learn from their mistakes.",
"OK, turning that option on was a bad idea.",
"Ha, ha, wrong again.",
"This was supposed to be easy.",
"I don't get it.",
"But I read the manual!",
"*sigh*",
"My descendants will avenge me.",
"I should have listened to my ancestors.",
"I should have read the help files.",
"One ill turn deserves another.",
"That's not what the Wand of Death was supposed to do.",
"*whimper*",
"No, this really isn't happening.",
"*bang*",
"This can't be real.",
"Someone call the Shiriff!",
"Awake! Awake! Fear, Fire, Foes! Awake!",
"I didn't know it was a Scroll of Aggravate Monster!",
"New blood joins this earth...",
"But my Saving Throw is Good!",
"I really only like writing poetry.",
"What is Time, friend or foe?",
"Time waits for none.",
"Why didn't I start a Necromancer?",
"Flash before my eyes: now it's time to die.",
"Gaaaah! This is most frustrating.",
"I hate summoners.",
"I'm not dead yet.  I still have five hit points.",
"To sleep, perchance to dream.",
"This thing all things devours.",
"What's a burrahobbit?",
"Blackheart! I have done thy bidding.",
"Hail Sauron, Lord of the Ring, Lord of the Earth!",
"I'll be back for my revenge.",
"You have robbed my revenge of sweetness.",
"I'll see your descendants later!",
"Oh, great.",
"But, Sharkey!",
"But I'm famous!",
"I like happy things. I'm really calm and peaceful.",
"Won't you look at all the pretty flowers?",
"What treachery is this?",
"Down here, we all float.",
"See you in the Void!",
"No, no, no. It's not fair.",
"This didn't just happen.",
"You hit me for HOW MUCH damage?",
"You cheated!",
"Oh well, it's back to square one in HMa.",
"Great. Now I can finish my memoirs.",
"Sharkey made me do it.",
"This is just gruesome.",
"I believe my kingdom will come.",
"I'll see your descendants later!",
"Duh! Lev Zakrevski wouldn't have fallen for that one.",
"It's appalling.",
"Oh, basely done! I had hoped for better of thee!",
"I'll get even! Next time I see you, Death is yours!",
"Empire Micro$oft is eternal!",
"My God, Please visit a punishment upon this unrighteous sinner!",
"Faugh, I'll be back...so long as human psyche has a blackness. Ho-ho.",
"This CAN'T be happening! I am Dying....!!!!!",
"I desire peace ... for all people...",
"I finished my life with no regrets!",
"Ah, PosChengband is too difficult!  I'm going back to Chengband!!",
"I guess PosChengband is too easy!",
"I guess this is the last version of PosChengband!",
"Well, Hengband was much more fun than this!",
"Damn that AmyBSOD harlot, she's such a terribly bad Game Master!",
"It breathes--More--",
"Argh, fire enchanted explosion got me.",
"That bastard instakilled me!",
"I got instakilled again!",
"Gah, the bastard had a gun!",
"Here lies a snail that got salted to death by Semzov the Cleaner.",
"I made the mistake to gaze directly at Medusa.",
"There is no corpse in this grave because the dude died to a black dragon breath.",
"Go Team Ampersand!",
"The Three Big S got me - I died of sickness.",
"The Three Big S got me - I died of stoning.",
"The Three Big S got me - I died of sliming.",
"I made the mistake to use a tightrope on myself.",
"I died by stepping into an iron maiden.",
"Here lies a victim of the guillotine.",
"Died of neurofibroma.",
"Starved to death. Next time I'm playing a race that does not have intrinsic regeneration.",
"Here lies a poor explorer, all because he attacked an exploder.",
"C is for Clair who wasted away.",
"D is for Desmond thrown out of the sleigh.",
"E is for Ernest who choked on a peach.",
"F is for Fanny, sucked dry by a leech.",
"G is for George, smothered under a rug.",
"H is for Hector, done in by a thug.",
"I is for Ida who drowned in the lake.",
"J is for James who took lye, by mistake.",
"K is for Kate who was struck with an axe.",
"L is for Leo who swallowed some tacks.",
"M is for Maud who was swept out to sea.",
"N is for Nevil who died of ennui.",
"O is for Olive, run through with an awl.",
"P is for Prue, trampled flat in a brawl.",
"Q is for Quinton who sank in a mire.",
"R is for Rhoda, consumed by a fire.",
"S is for Susan who perished of fits.",
"T is for Titas who blew into bits.",
"U is for Una who slipped down a drain.",
"V is for Victor, squashed under a train.",
"W is for Winie, embedded in ice.",
"X is for Xerxes, devoured by mice.",
"Y is for Yoric whose head was bashed in.",
"Z is for Zilla who drank too much gin.",
"I played Rodney's Tower and got to the quest but then some asshole read a scroll of genocide and wiped me out.",
"I played K-Measurer but a deep ettin summoned an island nymph who stole my shield of reflection and jumped into a hole, and then a lieutenant with a wand of death appeared.",
"Tried to punch out Cthulhu in Dynahack. He sucked my brain 200 times and then I died.",
"The elder priest in dnethack killed me, that cheater!",
"Tried to eat a tainted giant corpse in Grunthack. And then the game crashed while trying to make a bones file. I wonder if that has been fixed by now?",
"The wand destruction patch (which I really dislike) killed my K-Measurer character! Why the hell does lightning bypass my reflection and vaporize my ring of levitation while I'm over open water?",
"The wand destruction patch (which I really dislike) killed my K-Measurer character! Why the hell do wands of teleportation not work at all on the castle where you have no hopes of fighting an ancient dragon head-on?",
"This unfortunate fellow engaged the elder priest in dnethack and had all his stuff corroded, then lost his cloak of magic resistance to the asshole tentacles, and while trying to slip on the ring of levitation to get past the 200 pits the priest created, made the mistake of taking off free action and was chain-paralyzed to death.",
"A person who inhaled too much of Katharina's fart gas.",
"Tried to play Matichack and got a repeatable crash upon trying to kill Rodney, so I decided to commit sudoku.",
"Eyehack's Aphrodite (who certainly isn't overpowered at all) and her bitches stole all of my belongings and then a tiger spawned to assault my naked self. Yes, I could have engraved Elbereth, but it wasn't really worth it anymore.",
"I made the mistake of playing Russiack. Got bored to tears after five minutes, and resorted to repeatedly throw my sword upward after ten minutes.",
"Someone told me that there was a cure for acute depression waiting for me at the train station in 10 minutes, so I went ahead and started walking on the railroad tracks.",
"I played Russiack and died to some bullshit. Be glad you're playing a much better variant instead!",
"I played Russiack with my favorite role from SLASH'EM Extended and realized that someone had nuked the most defining features of that role. Not wanting to put up with that bullshit, I shot my rifle in . direction repeatedly.",
"I played Rodney's Tower. The last thing I saw was a kobold shaman and a few dozen instances of 'The sleep ray hits you!' on my message log culminating in 'You die--More--'.",
"I played Kneelhack and started in the lower right corner surrounded by rock. My pick-axe only produced the message 'This wall is too hard to dig into', so I applied it to my own skull instead.",
"The orange dragon technically instakilled me from full health because I stupidly thought that 54%% sleep resistance is enough. God I hate Kneelhack!",
"This dude fought zombies in Grunthack and eventually became one himself.",
"What, why did that succubus kill me? There was no message about her attacking or anything! What the hell, K-Measurer? I just died out of the blue!",
"No corpse can be found in this grave, because this grave is of a player who tried to take on an ancient black dragon in K-Measurer thinking that disintegration resistance would protect the inventory. He learned the hard way that it does not.",
"A scurrier tourist would have been buried here, but K-Measurer crashed and warped the body to the segfaultitis realm.",
"This samurai tried to fight an enemy samurai that had a wand of lightning, not knowing that Dynahack changed shock resistance to only offer 50%% protection.",
"Killed by bumping into an invisible shopkeeper.",
"Killed by foolishly trying to melee a superjedi at experience level 2.",
"Killed by foolishly trying to melee a giant eel at experience and dungeon level 2.",
"Wait, why did the priest get hostile? I got a strange feeling of peace when entering the te-- oh shit, I forgot that altars can disappear when dropping stuff on them...",
"Huh. Didn't think the darts of disintegration thrown by that kobold would actually disintegrate me. Oh well, at least my tombstone reads 'killed by a died', which sounds really cool.",
"Killed by a sexy leather pump.",
"Killed by bleedout from leg-scratches.",
"Instead of a corpse, we buried a statue here - it was created because some intelligent monster threw a cockatrice egg at the alive body.",
"Tried to eat a dreamer cockatrice, and forgot that the paralysis only happens after eating it is complete.",
"A poor soul who tried to eat an eight-footed snake, not knowing that they cause petrification.",
"Wait, so if I eat an undead mimic and become deathly sick, I end up becoming a gold piece for more turns than it takes for the sickness to kill me???",
"AmyBSOD: Enduring dnethack, until the inevitable death to the MOTHER FUCKER THAT HATES EVERY LIVING BEING. Man the elder priest is overpowered.",
"The elder priest in dnethack: No one has ever killed him, no one will ever kill him and those who try are dead. Like me.",
"'the elder priest of Moloch is a contender for the title of second most dangerous monster in dnethack' my ass, he's the SINGLE MOST DANGEROUS MONSTER THAT HAS EVER GRACED A PC SCREEN",
"Killed in dnethack by a beautiful pair of high-heeled stilettos. If you see Chris, please tell him that I want to hug him for adding the wonderful high heels!",
"Killed by dnethack's elder priest. I will now go ahead and write page after page of revenge porn. That monster is so unbalanced and I want to kill it!",
"fuck that monster!",
"ugods unbalanced fix plz",
"truck ghosts are unfair!!!!!",
"waaaaaaahhhhhhh something zapped a wand of remove resistance and stole my doppelgangers polymorph control i cannot play on without it waaaaaaaaaahhhhhhhh",
"What? The game didn't give me a sickness message when my angel kurwa repeatedly meleed that gray growth!",
"'Fatal contamination'? Doesn't seem to do all that mu-",
"Huh. Didn't expect that 'fatal contamination' causes me to grow a third leg.",
"I underestimated SLEX's much more dangerous Team K and got killed by a thrown cream pie.",
"Wait, so I'm not supposed to walk around in weird-looking rooms?",
"Why is there a special room type that's more unfair than any others and why is it the U-filled terror hall?",
"Drew the maze level with dozens of special rooms back to back on dlvl2 and died to a ginormous squad of trolls. But hey, there were 600+ monsters generated in a game that only lasted 500 turns!",
"Huh. I thought the temple of Moloch was the place you go to if you want to buy divine protection.",
"Why did the priest on the Lawful Quest turn on me? He was coaligned and I had 50k zorkmids!!!",
"Why did that imp Flickerer fail to die after I hit him with my long sword 50 times????????",
"'Stairs trap' my ass.",
"Vaporization attacks are gay.",
"Well screw this retarded game too.",
"What the hell was the developer woman smoking???",
"Whoa, didn't expect the thick farting girl to own me that hard. She surely didn't look that dangerous when I looked at her stats...",
"I died to a Burphack beholder because I didn't realize that the warning levels ONLY display the average level a monster has and not whether it's dangerous, so I stupidly thought a 2 on the warning meter couldn't pose a threat.",
"The air elemental speed beholder got me. Watch out, it's probably still around somewhere close by.",
"Air elementals should be renamed to 'fuck you'.",
"Monsters moving and shooting the same turn is so unfair!",
"The game broke down AGAIN!",
"I think centaur princes should not be allowed to fire 5 crossbow bolts per turn.",
"Committed suicide after hitting a superscroller trap. Amy, remove that trap from the game or I won't play it again.",
"Committed suicide after failing 50 times in a row to tame a wounding knight. Why didn't I simply say 'screw it' and move on? I have no idea.",
"Why were my sling rocks not doing any dama- DUH, forgot to actually wield that sling!",
"What, void room on dungeon level 5? With truck ghosts??? Seriously, Amy?",
"Amy I will never play this game again!",
"this game is unfair and kills you in random ways",
"shopkeepers should not be spawned invisible fix plzzzzz",
"amy you must remove nasty traps from the game",
"amy this thing killed me please remove it",
"amy i want this game to be as easy as vanilla nethack please remove everything that can kill the player character",
"Why is being drowned by a giant eel an instadeath? I had FULL health left!",
"What, digging in > direction while standing next to lava is an instadeath? That's bullshit.",
"You made explosive boots deal 200+ damage when they explode??? That's insane, no XL10 character will even have that many to begin with!",
"Wow, Jane's dancing shoes really are elegant... I couldn't help it, all I could do was wanking off while they kicked me to death.",
"I inhaled the beguiling stench of Josefine's sweaty feet until I fell down unconscious, and then she killed me with her velcro sneakers before I came to myself again.",
"Got my balls kicked by an asian woman in stiletto heels. She kicked them so hard, they were force-pushed out of their usual position causing several internal ruptures and internal bleeding which eventually proved fatal.",
"Killed by repeatedly being whacked over the head by Natalia's wedge sandals for being a very, VERY naughty boy!",
"Breath control fetishism is what killed this person.",
"Died of erotic asphyxiation.",
"Died of erotic asphyxiation. Turned out that yes, you can suffocate from tying worn socks to your mouth and nose if you use enough of them at once.",
"Had my x and y reversed.",
"Electrocuted by a queevolt.",
"Molested like a loli by the elder priest's tentacles.",
"stupid water tiles!",
"monsters should not spawn with wands of disintegration before level 20 because i noob and i forgot that items not have levels in this game",

	"TANJ!", /* Larry Niven */
	"Summer Rose - Thus Kindly I Scatter", /* RWBY */
	"Hold my beer and watch this!", /* Rango */
	"Of course, you realize, this means war.",
	"In death ground, fight.", /* Sun Tzu */
	"Did you see him passing by?", /* Dark Souls 3 */
		"The world began without knowledge, and without knowledge will it end.", /* Dark Souls */
		"Fear not the dark, my friend. And let the feast begin.",
	"For every hero commemorated, a thousand martyrs die unmourned and unremembered.", /*Warhammer 40k*/
	"FIQ:  One step from divinity.",
		"FIQ:  Debugging dNethack, one iron ball to the face at a time.",
		"FIQ, killed by a clockwork bug.",
	"Khor:  \"Wait, did that axe just fall off the edge of the world?\"",
		"Khor:  Balancing dNethack, one sleeping potion to the face at a time.",
	"AmyBSoD: \"Phew, killed that elder priest.  He shredded my CoMR but at least he's de-\"",
	"Package contains: One posthumous worm feeder",
	"Was that the hill I wanted to die on?",
	"Used no net, knew no fear, made mis-step, wound up here.", /*Balder's Gate gravestones*/
		"Here lies the body of Rob.  If not, please notify the undertakers at once.",
		"When I can no longer stand alone, then it will be time to die.",
		"I feel my body rising towards the bright light...wait, now it's falling, What the HELL!",
		"Reader if cash thou art in want of any, dig four feet deep and find a penny",
		"A lesson learned. Too bad I'm dead.",
	"I never got a single answer to any of my biggest questions, and now I will never know.", /*Unicorn Jelly*/
	"I beat you in the human race.", /*Fable*/
	"Nobody expects the Spanish Inquisition!", /*Monty Python*/
	"Omnes una manet nox", /*One night awaits us all*/
	"I wake. I work. I sleep. I die.", /*Alpha Centauri*/
	"I suddenly have a LOT of regrets.", /*Water Phoenix King is very quoteable...*/
	"You either die a hero, or you live long enough to see yourself become the villain.", /* Batman, the Dark Knight */
	"What can men do against such reckless hate?", /* the Lord of the Rings (Two Towers movie) */
	"I'll just hit this thing a couple more times, it'll die.", /*jonadab*/
		"I don't need to unburden myself, they're only soldiers, I've been killing soldiers, it'll be fine.",
	"Maybe if I press this key a bit harder I'll hit for more damage next time.", /*Grasshopper*/
	"...nor the battle to the strong", /*Bible*/
	"fucking yora chick suck dick.", /* by GoldenIvy */
	"looks like it's get the fuck out of here o'clock", /* by Khor */
	"It's a kick in the pants when you get killed by your spouse for playing the piano.",
	"that is why the dnethack elder priest exists - to simply frustrate everyone", /* by rikersan, and I couldn't agree more (as if that wasn't obvious from all the other messages about that mother fucker tentacle asshole :P) */
	"Forgot to wear a cloak, and then the wight used his level-draining attack a couple of times.",
	"What? I didn't know that thing autocurses!",
	"WTF, the hippie heels autocurse even if they are blessed? And they convey extrinsic hallucination? That's unfair!!!",
	"Here lies the n00b that decided to ragequit after his doppelganger was zapped with remove resistance and lost intrinsic polymorph control.",
	"Here lies a n00b that used #quit on dungeon level 25 after suffering what should have been only a minor setback.",
	"Please don't spawn whitlevel kops next time!",
	"Well, the void room is more than just a little unbalanced.",
	"Committed shoplifting and levelported away. How did the kops get from dlvl5 to 35 SO fast???", /* many thanks to Elronnd for the inspiration */
	"Why is the undead katcher not undead? I would have lived if he was, I had command undead at 0%% fail!",
	"Unfair attack trap. And then a normally harmless gnome lord randomly decided to get a touch of death off.",
	"I was too impatient. I had gotten the Amulet of Yendor already, but on the way up I ESCed out of the messages telling me about the succubus taking off my silver dragon scale mail, and then a black dragon breathed disintegration.",
	"Well, apparently eating up all the K-rations before hitting the Astral Plane is not the best idea if you then run out of permafood while fighting Famine.",
	"Whoa, I completely forgot that Pestilence can do up to 128 damage per round. I thought he couldn't touch me, I had a blessed unicorn horn after all.",
	"Wait, so you're saying there is a defense against random monsters with a wand of disintegration that appear out of nowhere? I don't believe that, all I saw was 'you hear a nearby zap' followed by the DYWYPI prompt!",
	"Scrolls of flood are bullshit on Sokoban. #quit",
	"You can't play with the yellow spell trap. I decided to #(rage)quit upon triggering it, because I'm convinced that makes me a better player.",
	"You can't play with the yellow spell trap. I decided to #(rage)quit upon triggering it, because I'm not touching SLASH'EM Extended again. I'll play casual games like Pac-Man or Minesweeper instead, since I need to polish my ego and that whole concept of 'difficult games' is too much for me to handle.",
	"The counterclockwise spin trap screwed me over, without it I'd have hit the correct button and walked away from the monster instead of bumping into it!",
	"ARRRRRRGH CURSE THE FUCKING FUCKED UP PROGRAMMER FUCKTARD WHO MADE IT SO THAT HITTING SPACEBAR AT THE FINGER OF DEATH DIRECTION PROMPT RELEASES IT IN THE PREVIOUS DIRECTION",
	"Here lies richard rag, who chose to zig when he should have zagged",
	"Killed by playing K-Measurer completely normally, because somehow there always spawn a blood pudding whenever you split another type of pudding even once. That's not a good idea IMHO.",
	"Killed by Rodney in Kneelhack, because thee's a limit for the wand of death to one recharge while Rodney's stats are simultaneously buffed out the wazoo.",
	"Suicided after losing the cloak of magic resistance to a black dragon in Satanchop despite having disintegration resistance. My last words? Hopefully the goddamn dragon will also be disintegrated one day :P",
	"Here lies the primordial man, who tried to fight the elder priest and had his brain cored like an apple, losing 9 points of intelligence every turn and then dying of brainlessness. Is he the one who generated that unbalanced monster?",
	"Huh, didn't expect that the elder priest on slex's topmost Yendorian Tower level actually is an elder priest that can kill more or less any character within a few turns.",
	"Got all confused in the Illusory Castle and then the glass golem blasted my mind 20000 times and then I died.",
	"There are too many truck ghosts and express trains and idolans and memory leaks and spectral killers and swansdown microfilms and adrenaline stevens and sorbate bowereds and everything in the Void! How the hell are you supposed to get past them without dying???",
	"I want to tell you that it's simply shit if monsters can read scrolls of genocide and end your game...",
	"Killed by too much streamlining in K-Measurer. I rather want to play a game that has a lot of randomness.",
	"Shit! I thought I didn't have to fight Vecna, but when I entered that weird Dead Grounds dungeon, there was a motherfucking GIGA VECNA that ONE-SHOTTED ME DESPITE MY FULL ASCENSION KIT!!!",
	"There were 200 cluster bomb rocks behind that closed door, all ready to draw me in and explode. Nuff said.",
	"Some overpowered monster in terrahack killed me even though I had overpowered artifacts in 12 slots and overenchanted gear in 8 others and a naked AC of -50 and 2000 max HP.",
	"Here lies someone who was so stupid and thought it was a good idea to ESC out of messages in slex.",
	"The monsters on Rodney's Tower's Astral Plane spammed 200 charges of their create monster wands and then the things that got created also had wands of create monster and then I got cornered by a couple Archons and died.",
	"Well that Rodney's Tower monster was just regenerating health way too quickly and therefore I couldn't kill it.",
	"Some player monster in Rodney's Tower spawned with a wand of death and was skilled at wands of course, and somehow that caused the wand to create a death field that cannot be reflected, so I died despite having a shield of reflection.",
	"I found out the hard way that if I'm polymorphed into a foocubus in Rodney's Tower, I cannot seduce monsters despite expecting it to be fully monster-player symmetrical. Not wanting to live in that world anymore, I then zapped my wand of death (the one between my legs, you know, I was an incubus after all) in . direction and now I'll play SLEX.",
	"I died in SLEX because I didn't feel like farlooking every monster, so eventually a powerful one spawned and owned me.",
	"Help, I'm stuck! I can't get past dungeon level 6 because every time a leocrotta or mumak or other strong monster spawns and kills me!",
	"Tried to seduce a foocubus because hey, it works in vanilla! But after undressing me, she kneed me in the nuts and that is supposedly meant to be funny but I'm some sensitive man who doesn't find that funny, so I quit slex.",
	"What? When that leocrotta 'ripped into my breast with maniacal fervor', it did more damage in one turn than the pokedex says it does! Unfair!!!",
	"'Such a sweet nymphet' my ass. If only you could start with a blessed scroll of genocide, I'd make sure that I never have to deal with those nymph bitches again.",
	"Some Rodney's Tower monster zapped a wand of cancellation at my wizard and that was some A-grade bullshit, I tell you. I had a goddamn cloak of magic resistance, and in SLEX that should protect against cancellation because SLEX is well-designed!",
	"Some Rodney's Tower monster zapped a wand of slow monster at me and since there is no defense against that, the air elementals and angels and mastodons and whatnot then proceeded to kick the crap out of me. Next time I hope that the wands of inertia are rare enough to be balanced.",
	"The soldier threw a double free corruption grenade and somehow the crashed game failed to recover.",
	"I tried to farlook an orange U and got a buffer overrun. Instead of trying to figure out how to get my savegame back, I then played Alien Half-Life and got a buffer overrun there too, even though I wanted to put my nose in front of Anastasia's girl shoes and inhale the dog shit she stepped into.",
	"Got some bullshit stack bashing segfault core dump desktop panic, and on the prompt that asks if the game can be recovered automatically, I annoyedly hit 'y' to say 'yes, recover it'. But the MOTHER FUCKER game is programmed so that 'y' means 'yes, delete it' AND THERE IS NO FAILSAFE TO ASK WHETHER YOU REALLY WANT TO DELETE IT!!! RAAAAAAAAAAAAARGH",
	"I had almost finished playing that level and then the game segfaulted, and when I tried to play through it again because somehow it is so that the game never autosaves unless you switch levels, I died. Bullshit.",
	"Ran into a panic error and I'm such a noob who doesn't know that in almost all cases, NetHack will make an emergency save. I simply thought my game was gone for good and started a new character.",
	"Here lies a victim of Jubilex and the segfault spit attack he gets in Grunthack. (But that error has been fixed by now.)",
	"I died to some monster in slex and now I will proceed to tell Amy that her game is unbalanced, although by that logic many vanilla NetHack monsters would have to count as unbalanced too. After all, it's easy to die to a leocrotta or mumak in vanilla and they appear rather early.",
	"Screw your meme fork.", /* by Coolfrog */
	"Slex is a digital turd and just one of the many proofs that women will never achieve anything no matter how hard they try", /* ditto */
	"I had a tron stone thrown into my inventory and didn't realize that if I wait for one turn after each move, I can still walk down a narrow corridor. Instead I made the mistake to ragequit the game.",
	"slex is nothing but dicks in the butt, at least i think thats how amy described it once",
	"I triggered a captcha trap and said 'screw this fucking game' instead of toughing it out and solving the captchas.",
	"Held the wait key down to make the hallucination end. In curses mode.",
	"Huh, quaffing from fountains can cause me to die of system shock???",
	"Wait, what do you mean fountain quaffing ain't safe?",
	"Crushed by a malfunctioning elevator.",
	"it is stupid that nasty traps dont reveal themselves when triggered",
	"It's a mumakill - a person killed by a mumak!",
	"Why do I die when I get shot?",
	"I didn't lose! I was semi-victorious!", /* Azumanga Daioh */
	"IT'S A GUNDAM!", /* Gundam Wing */
	"Paul Eulmeyer, 1907-2000. With atomic terror, he brought us peace.", /* Mission Hill */
	"Cunnus! Perite! Canis! Nothus! Futue te ipsuni! Mentula!",
	"oh god, why can't I make it so that 'move in a direction until interrupted' in K-Measurer is interrupted by an intersection but quicktravel is not? I can only make it so that both are interrupted or none are! :(",
	"oh god why does a pile of fucking rocks interrupt quicktravel in K-Measurer???",
	"slex is the variant where you need an usb port with an external memory extension in your skull to have enough brain capacity to keep all the new ways of being killed in your brain ;-)",
	"if you want nethack than vanilla is the best, if you want a nice ux then nh4 is the best, if you want more burden of knowledge go with slashem, if you want to torture a prisoner then tell them you will release them if they ascend a bleeder on slex",
	"The frog's dead. He Kermitted suicide.",
	"Who designed this sorry excuse of a travelto command in K-Measurer??? Seriously!",
	"How To Kill Nethack And Erase It From The Popular Consciousness: a guide by LarienTelrunya",
	"GUH, K-Measurer does this completely retarded thing with unihorns where every single stat point restore disenchants it",
	"reminder to self: never use quicktravel on satanchop soko or the game will screw you over, a hundred percent sure",
	"at least K-Measurer soko isn't terrahack's sleepoban, but the bullshit travelto command still pushing boulders into corners is some A-grade bullshit and the feature should have his or her license revoked",
	"in slex I could so beautifully use travelto for sokoban, especially for repetitive levels, but if I try in K-Measurer it will, without fail, screw me over",
	"wtf how many 200 wand zaps does it take to up that wand skill in that wand destruction patch K-Measurer?",
	"I guess the design philosophy behind K-Measurer is 'How To Piss Off AmyBSOD Within 5 Minutes - A Tutorial'",
	"good lord Amy really thinks her game is the best balanced fork, her delusion is off the charts", /* inspired by mtf */
	"The elder black dragon in K-Measurer disintegrated my +4 silver dragon scale mail even though I had disintegration resistance. Since that's utter bullshit, I seeked the nearest deep ettin and let it summon monsters until one of the summoned ones finally put me out of my misery.",
	"There are too many player monsters in Rodney's Tower's Gehennom. It was a matter of time until one of them spawned with Vorpal Blade and instakilled me.",
	"Autoexplored into lava. Bullshit.", /* happened to Adeon in Unnethack */
	"I died because I made the mistake to hit the autoexplore key. That key is the first thing on my list of things to genocide once I learn how to genocide things that you cannot genocide.", /* happens to me in DCSS every single time */
	"wtf division thieves are bullshit!", /* happened to Winsalot */
	"hopefully the dnethack elder priest chokes over some elderberries and dies",
	"You gaze upon the ancient bones of Glen Wilkerson.",
	"I forgot which wards my terrahack character knew, and the only way to find out which ones you do know seems to be to try to use one. I only wanted to know which ones my character knows, I didn't actually want to engrave one. But I was standing on an altar.",
	"Don't you wish you prayed instead.",
	"The guard captain stole my lightsaber, I fell through a hole and got gang banged by invisible scary things all the time being brought back from the dead so I could get pounded again.", /* happened to tubs */
	"Fuck K-Measurer's alignment system. I have a fucking cursed ring of levitation stuck on me. I pray and god decides that my polymorph form is bigger problem than that shitty ring. After a while I start having serious issues with hunger, so I pray again and this time I get fucking punished! Dude...", /* by Winsalot */
	"Go team Girl (and shoes)!",
	"chances of dying in slex to something girl-related: 20%%. chances of dying to something shoe-related: 30%%.", /* by aosdict */
	"Sometimes the game just doesn't feel like giving you a source of reflection or magic resistance. So I decided to brave the castle, but the first lieutenant had a wand of death because of course he did.", /* happened to me in Splicehack */
	"Personally, apart from maybe a handful of games in Junethack to get some cross-variant trophies (walk in the park and such), I mostly would not recommend trying the variants with a lot of added content (Slash'EM, dnethack, or even UnNetHack), nor the ones intended for advanced players (SporkHack, GruntHack) until you are already pretty familiar with the basic game.", /* by jonadab */
	"Slash'EM Extended combines both of these traits into one variant and cranks them both up to eleven and then some, so starting with that one would definitely be jumping in at the deep end of the lava pool, with a heavy iron ball shackled to each ankle.", /* ditto */
	"Tried to polymorph into a yellow mold in Burphack to cure the zombification disease and got a SIGFPE.",
	"Died from whacking a wall in Rodney's Tower with my katana. What the hell?",
	"GAH the CURSED resistance change in Rodney's Tower made it so that I lost strength from eating a poisonous corpse even though I was resistant from eating a killer bee earlier! And as a result, my strength was so trashy that I died when a lone soldier ant appeared!",
	"Tried to play the newest version of Rodney's Tower and got annoyed to death by all the annoyance factors.",
	"Tried to do Sokoban in Rodney's Tower, using autotravel to skip past boring walking segments, and OF COURSE it pushed a boulder into a corner. I then committed sudoku because I don't want to have the travelto command trashed. It should work normally, dammit!",
	"Something in Rodney's Tower used a scroll of summoning, and even though the monster reading it was standing very far away, the minotaur appeared right next to me and owned me. Yet another scroll that can randomly end your game! Genius!",
	"These stupid fake earlygame bones piles in TSX... now I'm wearing a cursed blindfold! why is everyone considering them to be a good idea? I think they're really silly!",
	"maaaaaaaan shitty lag! it's absolute shit if, when you're walking down a winding corridor, it takes a split second after each turn you make, because you have to wait until the screen updates to see in which direction the corridor will continue!",
	"Demo said that kneelhack was cancer, so I went to check it out and my death was so ridiculous that I can't help but agree with Demo's assertion. That trainwreck seems to be literally unplayable.",
	"Made the mistake to play Kneelhack. My samurai had 15 strength and maxxed long sword skill, yet the katana was still doing scratch damage, taking 5 hits for a BUGBEAR. Then an actually difficult monster appeared - a stone golem in this case - and simply owned me because I can never seem to kill anything in that variant.",
	"If the savegame corruption bug is not fixed once and for all, I will call it 'a huge penis and a half' (thanks Demo).",
	"I played one game of slex, understood precisely nothing, and died to being poisoned when I thought I was immune to poison", /* by zid */
	"Welcome to Kneelhack, where your samurai with 14 strength and expert long sword skill needs 5 hits with a katana for a wimpy gnome lord.",
	"omg Rodney's Tower is absolute shit, it still kills you by way of monsters using items that have player-hostile side effects! dwarves can apparently zap wands of cold that explode and make the shopkeeper angry at YOU!",
	"Pressed R while levitating over open water in Kneelhack and for some weird reason there is no confirmation if the ring of levitation is the only accessory you're wearing.",
	"The downstair once again refused to generate in Kneelhack, so I had to suicide.",
	"Kneelhack must be a secret governmental torture machine. They probably invented it because waterboarding was banned and they needed a way to continue torturing their victims.",
	"Ascended on the wrong high altar, was permitted to live but eventually died of old age.",
	"Tried to eat something on the tile that Medusa died on and because I wasn't used to the variant I'm playing, which uses a different corpse eating interface than SLASH'EM, of course I ended up eating Medusa's corpse by mistake. Gaaaaaaaaah.",
	"Tried to play a neutral atheist in K-Measurer. Eventually it became clear that there is absolutely no way to increase your alignment record, making it forever impossible to unlock the quest, so I suicided.",
	"how is one-eyed sam 'simple'??? he was buffed something fierce, iron bars are generated if you attack him, and to top it off, his gender was changed and made female without changing his name to 'Samantha'! arson, murder and jaywalking!",
	"A fire giant threw a boulder off the map in Kneelhack's valkyrie quest causing the game to crash, and very sadly, it seems that particular savegame cannot be recovered no matter what.",
	"god are there no quicktravel settings that allow me to set what does and doesn't interrupt me? this seems like it has never been playtested! it is so annoying if you have ESP :P",
	"Bullshit! What the hell are you supposed to do about the FUCKING amulet teleporting away whenever you kill Cthulhu? It literally teleports away every time! And he respawns again and again and is way too hard to kill, so I eventually ran out of full healing potions and died to this UTTER BULLSHIT!",
	"okay this is absolute bullshit and unplayable... killed Cthulhu for the third time, of course he ignores the elbereth and OF COURSE the amulet is also gone again, why the hell does the game work like that",
	"Man, weeping archangels are the anti-fun squad.",
	"Seriously, item destruction is insane, and the evil variant actively makes it worse.",
	"what if SLEX is just a complex survival method that the amy uses to find intelligent, healthy new mates for the year to ensure the survival of the kr*ut race",
	"Monsters which cause instapetrification that can't be stopped even if you have a lizard corpse deserve to get instapetrified too.",
	"Not surprising... SLEX, the variant where no means yes.", /* by Mandevil */
	"I asked Mandevil how he got in here, and he responded: 'I used my Ana-Elf-Mal-Cha raygun to disintegrate your defences.' And at that time I hadn't eaten a black dragon corpse yet.",
	"damn the weeping angel anti-fun squad! why are they so annoying :P",
	"WUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUT WHAT THE HELL I thought turning paranoid water off means I just don't get the unnecessary 'yes/no' but a simple 'y/n' instead! Now I fucking DIED because of that!",
	"Bah, somehow it is so that if quaffing from a fountain gives a water nymph, she will then get a turn! It should be your turn after she got generated!",
	"did I make it so that evil variant monsters spawn more often on friday the 13th by mistake?",
	"is my vehicle broken?",
	"God, I got to cry harder.",
	"Forget about it.",
	"So ein Mist!",
	"The downstair and the branch staircase spawned on the same tile and the game would give precedence to the branch when pressing > on it, so my lostsoul sokosolver had no way to proceed and ended up suiciding instead.",
	"Somehow there was a second vibrating square level in Sheol and it had the seven deadly sins again and they killed me.",
	"This fellow was too n00b to use the panic digging technique correctly and ended up quitting even though he could have waited until it became available again.",
	"This fellow didn't read the description on the panic digging technique and used it without engraving Elbereth, so he ended up getting killed by a newt.",
	"This fellow played during Junethack and forgot to read the message telling him about the lostsoul mode not being eligible, so he died in vain and his Medusa kill didn't count.", /* sorry Elronnd, I guess I should make the message stand out more */
	"Forgot that I was playing a new variant, and died to a new vanilla change. I guess I'll play a variant of old vanilla again now.",
	"Man, why couldn't the variant author base its variant on old vanilla??? Now I died because of a feature from new vanilla! I should play a variant of old vanilla so I don't get my muscle memory screwed over!",
	"Okay this variant is annoying, in vanilla I would have lived. TSX is so different from the vanilla I'm used to...",
	"Died to something retarded.",
	"I realized that the variant I was playing has viruses, and don't want to play it anymore. My next character will be a SLEX one I guess.",
	"RIP diversity, killed by the virus.",
	"RIP freedom of speech, killed by the thought police.",
	"RIP poofy coffee, killed by political correctness reasons.",
	"Killed by thigh cramps because there are no high heels for men. This is bullshit, I would have lived if I were allowed to wear wedge heels to ease my pain.",
	"Died to a severe dysentery infection.",
	"My car was hit by a truck with a full-speed head-on collision and I died immediately.",
	"Got wrecked when my car was run over by a tank.",
	"Here lies the victim of a plane crash.",
	"Here lies the victim of a plane crash. Specifically, the crash occured when trying to go from the Plane of Water to the Astral Plane, a buffer overrun occurred when the game tried to make a named player monster.",
	"Here lies a kneelhack valkyrie whose savegame got mangled into an unrecoverable chunk because somehow the game failed to keep a .0 file in the playground.",
	"Touched a fire barrier in Bowser's castle and jumped out of the picture.",
	"That slide in the frosty lake level pushed me into the one triangle that buggedly counts as out of bounds, which caused Mario to lose his hat and die immediately for no apparent reason.",
	"Here lies Super Mario, he entered the wrong pipe in Super Kaizo Mario and ended up in an inescapable death trap.",
	"Here lies Super Mario. He jumped out of the picture.",
	"Here lies Super Mario. He shot himself to the top of the castle after collecting all 120 stars and then ended up in that one spot near the map edge, which caused him to lose his cap and die out the blue.",
	"Here lies Maricia Usanna. After her beloved pink high heels got destroyed, she committed suicide.",
	"Here lies Marje Marid. She killed herself after her precious earring was stolen.",
	"Here lies Pokoh Asseguest. He was caught in a deadly voltage spike.",
	"Here lies Big Bear Havei. Some stinking bitch poisoned him to death.",
	"Here lies Lenka Valus. She begged for mercy but the evil robber didn't care and cut her up with a katana.",
	"Here lies Wolf Stengel. He was shot because unlike the rest of his squad he didn't pull back and run for cover when the enemies used heavy gunfire.",
	"Here lies Tacitus Bort. A cute little girl shot him in the head with a tubing rifle and blew his brains out.",
	"Here lies Aee Dumas. Some fat bride blew him to bits with a landmine.",
	"Here lies Evita Hall. She was killed by some motherfucker.",
	"Here lies Ryu Hayabusa. His girlfriend kicked him to death.",
	"Here lies Urbaldi Six. A beautiful bride stomped his nuts flat and then crushed his skull with her sneakers.",
	"Here lies Pete Schwiedelhole. Some slut with syphilis slit both of his legs full length and he bled to death.",
	"Here lies Lex Miller. He happened to be too close to a nuclear bomb when it detonated.",
	"Here lies JoJo of Warrington. She was eaten by a newt.",
	"Here lies Jyllia Gynn. She faded away when she realized her money wasn't enough to buy the pair of green peep-toes.",
	"Here lies Sabrina Merilis. The stormcloaks beheaded both her and her husband for the crime of siding with the empire.",
	"Here lies Sabine Laul. Killed by a soldier after her former boyfriend had mortally wounded her.",
	"Here lies Yvara Walter. A motherfucker cut her up with a knife.",
	"Here lies Liebea Luna. She was r*ped and subsequently killed by a bunch of bandits.",
	"Here lies Denshi Gasu Montoya. After his mother had ruined both of his wonderlegs with her buckled sandals, he no longer wanted to live and let another woman scratch him to death with sexy leather pumps.",
	"Here lies Mr. Black. Killed by that whore spawned from hell known as Bellatrix.",
	"Here lies Tiger's Claw Poldi. His friend brutally executed him with a sawn-off shotgun.",
	"Here lies Katzou Flesher. A robber gunned him down with an assault rifle.",
	"Here lies Mohmar 'Deathstrike'. Died of old age because no threat, no matter how great, was capable of ending his life prematurely.",
	"Here lies Ingo Schmitz. He was trying to negotiate terms with a slut but she suddenly fired a gun at him and he died immediately.",
	"Here lies Septimus Gloeckle. He misunderstood the Oblivion loading screen and died with a blade stuck in his vagina. Except that he didn't actually have a vagina, so the blade had severed his penis and some vitally important blood vessels instead, leading to his death.",
	"Here lies Isolde Caught. The robbers beat up that lardass bitch's butt so powerfully that she died of internal organ failure.",
	"Here lies Elli Karma. Her body will rise again as a vampire and she will suck every last drop of blood from her killer's sorry body.",
	"Here lies Vilja Agnethasson. She ultimately couldn't stand her own annoying voice any longer, so she decided to point her gun at her own head and pulled the trigger.",
	"Here lies Sunija Bitchiya. She suffered a severe constipation that prevented her from farting and eventually lead to fatal organ ruptures.",
	"Here lies Rhea Oro. The angry mob killed her for publishing too much fake news in her newspaper.",
	"Here lies Jasmin Nagano. After she had been repeatedly r*ped by a robber bastard, she drowned herself in the river.",
	"Here lies Erosina Shol. Her body was repeatedly treated with poisonous spikes until she expired.",
	"Here lies Irmina Trong-Oul. Some cold-blooded bastard incapacitated her and then left her behind in the middle of nowhere to die.",
	"Here lies Martius Murkel. His teacher shot him dead.",
	"Here lies Faster-Than-All-Others Stakedeer. Ran into a wall at high speed, died of a fractured skull.",
	"Here lies Senator Antius Scratche. His girlfriend killed him with her combat boots.",
	"Here lies H. After he refused to tell his full name to the inquirers, they eventually decided to kill him.",
	"Here lies Davide Miller. Got caught in the blast of an exploding grenade.",
	"Here lies Melirija Pekriah. She was stabbed when she told her friend that she wanted to dump him.",
	"Here lies Larissa Oldsmall. Some bride used the Imperio curse on her and told her to kill herself.",
	"Here lies Sysette Deers. The guy who calls himself her boyfriend treated her very badly, resulting in her death.",
	"Here lies Miss Haskill. The people who killed her had originally put her corpse on a stake.",
	"Here lies Elenya Pure. Her ship sank to the depths of the sea.",
	"Here lies Golden Mary Himal. She was perforated with minigun rounds.",
	"Here lies Lara Hossie. The angry customers decided to forcibly take back the money that this greedy nurse slut had gotten from them.",
	"Here lies Sandrina Emperor. When her hometown was invaded, she didn't try to resist even as the aggressors were about to kill her, and evidently they succeeded.",
	"Here lies Doctor Maex Heartfount. His teammates got very very annoyed at his constant friendly fire and decided to repay the favor with bullets.",
	"Here lies Marc Seven. He was considered too large a threat for the evil overlord and knew too much, so he had to be neutralized.",
	"Here lies Arno Eckart. Poor guy never really fit into his new environment, and eventually it devoured him.",
	"Here lies Hailbush. The old failure constantly had his money stolen and therefore couldn't pay for food and basic needs, so he ultimately starved.",
	"Here lies Romann Ownlaud. He was such an idiot and thought it was a good idea to attack a gun-toting criminal with his bare fists.",
	"Here lies Siegfried Koopa. Wendy will not be pleased that he died in such an ignoble way.",
	"Here lies Roy Koopa. His head was stomped by Mario three times in a row.",
	"Here lies Ludwig von Koopa. He has sung his last song.",
	"Here lies Larry Koopa. Obviously Mario didn't share his taste for hardcore punk music.",
	"Here lies Wendy O. Koopa. Not only did Luigi kick that turtle miss's butt, he also stole her high heels and then accidentally jumped into a pool of lava with them.",
	"Here lies Iggy Koopa. He always wanted to duel Mario, and ultimately he got his wish; unfortunately, Mario won.",
	"Here lies Morton Koopa. His dad eventually realized that an usurpation was about to take place and therefore preemptively killed his son.",
	"Here lies Lemmy Koopa. Mario stomped his balls. All six of them. That included the ball Lemmy was riding on, the three balls he was throwing around, and the two between Lemmy's legs.",
	"Here lies Bowser Jr. When Bowser realized that he had gotten another kid, and especially when he realized that this new kid was uglier than all of his Koopalings, he quickly suffocated that little brat to make sure it would never grew up.",
	"Here lies Tonilia Innocentia. Well, at least no one had robbed her innocence before she died...",
	"Here lies Claire Tato. She somehow thought that being a spokeswoman would mysteriously make her immune to bullets.",
	"Here lies Lumia Livenoch. She never believed in magic, but obviously her attempt to disbelieve the existence of death rays failed.",
	"Here lies Lahira Hattrick. While her mastery of water spells was unprecedented, it didn't help her stop the hot searing flames of doom engulfing her.",
	"Here lies Estrella Pietra. An acquaintance of her shot her with a gun.",
	"Here lies Sontaire Chansina. She didn't take any threat seriously, and eventually met someone who followed through on his threats.",
	"Here lies Celina Peng. Some bastard cut off her leg and didn't bother to call a medic or otherwise ensure her survival.",
	"Here lies Jasajeen Diagna. A vulture hacked her to death and then devoured the remains.",
	"Here lies Kersey Forester. The dizzy blonde got lost and fell into a deep hole.",
	"Here lies Rinka Schmitt. Stoned to death by an audience that didn't want to put up with her bitchiness.",
	"Here lies Arvogenia the High-Heeled Topmodel. Died in a bike crash because she absolutely had to wear high heels on a bike and couldn't hit the brakes fast enough with them.",
	"Here lies Greneuvenia the Hugging Topmodel. Accidentally locked herself in her own room with the key lying on the other end, and her stupid pride prevented her from calling for help, so of course the dumb hussy died.",
	"Here lies Jill Waster. Walt's agents assassinated her.",
	"Here lies Anna Oar. Walt personally executed her and fed her corpse to his dog.",
	"Here lies Anton. He thought his bus was a racecar and raced into a tree.",
	"Here lies a bunch of beautiful flowers that was killed by a satanic lawnmower.",
	"We buried the bones of a farm animal here. The poor thing was caught in a tool of the devil called 'combine harvester'. Those killing machines really should be banned.",
	"Here lies Jora Koeler-Jesters. Tried to pick a fight with the security officers and lost.",
	"Here lies Fisoa Havei. The hussy somehow managed to die to a spitball. Don't ask me how she managed that.",
	"Here lies Fridrika Lightshape. She found out the hard way that there's a limit to the amount of badguys she can fight at once.",
	"Here lies Trycja Shorter. A silvester firecracker blew up and fatally injured her.",
	"Here lies Roswitha Freeze. Just couldn't lay off the sparkling wine, and died from an overdose.",
	"Here lies Eleanor Berrytowner. She has lived a long, happy life with her husband Queelix, and when the time had come, she allowed the Grim Reaper to take her away.",
	"Here lies Kirja Berrytowner. May her sexy butt cheeks and lovely platform boots always live on.",
	"Here lies Viktorija Halfa. Her competitors used dirty methods to get rid of her, and unfortunately they succeeded.",
	"Here lies the word 'poofy', considered insulting even though actual gay men said they didn't consider it insulting.",
	"Here lies your memory, put in this grave after your brain was sucked by a mind flayer.",
	"RIP permablind conduct, killed by the damn Eyes of the Overworld that ignore the birth option instead of simply not allowing you to see.",
	"RIP Mitre of Holiness, evaporated because you read the scroll of enchant armor despite getting the 'You stop disrobing' message.",
	"RIP user-friendliness, killed by the impossibility to target the scroll of enchant armor when we're wearing many armor pieces.",
	"RIP playability, killed by Rodney's Tower. Now you live or die on a crapshoot, because if a monster gets the wrong musable item it will fuck you up and there's no way to counter that.",
	"RIP full player-monster symmetry, killed by the lack of foocubus seduction messages.",
	"RIP intact skin, killed by sunburns. We should genocide the sun, nobody needs it anyway. Let there be clouds and rain forever!",
	"Entered Gehennom without fire resistance in Nethack 1.3d.",
	"Stupid bugged game, that was probably ascendable. Nethack 1.3d needs proper panic handling instead of simply declaring it a loss.", /* I (Amy) count Tariru's loss as an ascension because it's not his fault if the game is programmed that badly (his original message was slightly different from the one I wrote here) */
	"Went to Gehennom. Played through a few levels. Died of boredom.",
	"Well that death was bullshit, the game is getting the uninstall treatment now.",
	"Sometimes the only winning move is not to play.",
	"Another victim of the torture machine that is called Rodney's Tower.",
	"Autoexplore caused me to autoexplore into a dead end, and when I wanted to walk back out the way I came, there were 200 yaks and basilisks and Sigmunds and whatnot so I died with zero chance. How do I genocide the autoexplore hotkey?",
	"Here lies Super Mario, he was pulled underneath by instadeath quicksand.",
	"Here lies Zero. He committed suicide when CJ failed to get that stupid red baron back to the roof, even though all of Berkley's delivery boys were already dead.",
	"Here lies someone who tried to steal a woman's high heels. She furiously split his skull with the very tender heels.",
	"Jumped from the 10 meter diving board... and missed the pool.",
	"This fellow was so stupid and decided it was a good idea to stop in a high-speed water slide. The next racer crashed into his back and the idiot died of broken bones.",
	"This grave marks the sad tale of a diver who tried to dive too deep, ran out of air and couldn't resurface in time.",
	"The remains of a hobo are buried here. Poor guy got crushed between two train cars because he didn't watch out.",
	"This sad fellow jumped in front of a train. While he may now have transitioned into a world where there is no pain, he should have thought about the poor guy who was in control of the train when it happened...", /* seriously guys, if you have to suicide, DO IT IN A WAY THAT DOES NOT TRAUMATIZE INNOCENT PEOPLE --Amy */
	"Some lunatic tried to cross a federal highway on foot and got run over by a car. We buried his remains here.",
	"The Amy does not understand the difference between race and class and so just flips a coin to decide which will be which when adding new content.",
	"We buried some fool here who tried to pull down a religious woman's headcloth in public. She hired a bunch of measurer men who stabbed him.",
	"Here lies a victim of self harm. DO NOT TRY THAT AT HOME.",
	"Roses are red, violets are blue, I died and so will you.",
	"The butterfly died... NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOoooooooooooooooooooooooooooooooouuuuuuuuuuuuuuuuuuuuuuuuuuuu!!!!!!!!!!!!!",
	"Here lies Princess Zelda, because Link, the noob, failed in his quest and thereby allowed Ganondorf to succeed with his evil plan.",
	"Here lies Princess Peach. That idiot Mario lost all his lives and therefore failed to free her, and Bowser decided to just let her starve to death.",
	"Here lies Princess Palutena, because Pit Icarus fell to his death and thus failed to rescue her.",
	"Here lies Pit Icarus. He fell into a bottomless pit. Isn't that the pits?",
	"Here lies Princess Cyndi (who looks like a boy). Alex, that failure, got beaten up by every stupid kid from River City and was therefore unable to rescue her.",
	"Here lies Chintai (who looks like a girl). He got very depressed after Billy just skipped the battle with him in the second world, and ultimately decided to jump off the roof.",
	"Here lies the nameless princess from Double Dragon. Billy and Jimmy killed each other in the fight for her, and the key to her prison rolled into a sewer lid in the process so no one was able to rescue her anymore.",
	"Here lies the prince from Jill of the Jungle. The heroine thought the game was called 'Jill leaves level 14 without saving the prince'.",
	"Here lies King K. Rool. Donkey Kong punched him so hard that he fell down from the very top of Crocodile Island and was eaten alive by the titular animals.",
	"Here lies a little sister. There was one additional stray breath attack after the Big Daddy tyrant was killed, and the poor girl melted down instantly.",
	"Here lies King Zabi. One of Saimore's assassin bastards stabbed him and then Larnneire was innocently blamed for the murder.",
	"Here lies Elishe. When Bethel the White Hawk came across her bones, he cried for the first time... and time has been frozen for him ever since. We will all pray that one day Bethel may regain his wings and soar again.",
	"Here lies the traitor Lance Vance. He was perforated by so many assault rifle rounds when Tommy realized what was happening.",
	"Here lies Sonny Forelli. He thought Vice City was his property, but Tommy Vercetti sent him to hell.",
	"Here lies Maria Latore. Apparently Claude, the psychopath, shot her because she was constantly complaining about girlish stuff, like breaking a nail.",
	"Here lies Don Salvatore. The stupid idiot was treating Maria like shit and then thought he could blow up Claude's car just because Maria liked him, but Claude survived, came back and took bloody revenge.",
	"Here lies Catalina. Claude blew up her escape helicopter.",
	"Here lies the cold-blooded G-Man butcher. Gordon Freeman realized that he had become an intolerable threat, so he hunted down and KILLED that asshole, and at last the world was told the truth about the Black Mesa incident.",
	"Here lies Lance 'Ryder' Wilson. The shitty drug addict always thought he could bully CJ, but when CJ realized that his pseudo-'friend' was a Ballas all along, he sniped that shithead.",
	"Here lies Melvin 'Big Smoke' Harris. He failed to take responsibility for his actions until the end, so CJ had no choice but to kill the fat fool.",
	"Here lies Officer Tenpenny. He died in a fire truck crash, even though he would have deserved to be killed at CJ's hands. Well, at least the asshole is dead, that's all that counts.",
	"Here lies Officer Pulaski. He could have killed CJ but fumbled it, and then CJ killed him instead.",
	"Here lies Claude Speed. He was repeatedly kicked in the nuts by a redguard girl in Puma sneakers, and even though the tender girl was so frail that Claude would have been able to knock her out by simply walking into her, she still managed to kick him to death.",
	"Here lies Tommy Vercetti. A lilac beauty slapped him repeatedly with her soft, fleecy hands and kicked him with her cute pointy girl shoes until he died.",
	"Here lies Carl CJ Johnson. He listened to erotic air current noises for a long time, but then the sexy prostitute got him with her high-heeled lady shoes and proceeded to stomp him with the lovely heel until he expired.",
	"Here lies Mirmulnir the great dragon. The Whiterun town guard assholes somehow managed to kill him even though they should really have died.",
	"Here lies the Empire from The Elder Scrolls. Galmar was right when he told Rikke: 'The empire is dead. And so are you.'",
	"Here lies Legate Rikke. The stormcloaks killed that stupid bitch and ripped out her breasts.",
	"Here lies General Tullius. Ulfric stood before him and shouted, killing that old fool just like he killed that pseudo 'high king' Torygg.",
	"Here lies Jarl Balgruuf. His kids went insane and committed patricide, with Nelkir piercing Balgruuf's black heart with an arrow and Dagny using her cute little dagger to slit his throat.",
	"Here lies the world peace, murdered by power-hungry politicians.",
	"Here lies the Earth's climate, killed by industrial pollution.",
	"Here lies the Pacific Ocean, transformed into an area inhospitable for all life due to the huge amounts of plastic waste.",
	"If you are a lesbian: Do not respond to the Amy, do not play SLEX - you can and WILL be indoctrinated into the 6th Order of the Black Sun. You WILL end up becoming one of the Amy's many harem women. Men may safely play her game, although straight women have been 'turned' by her before.",
	"This fellow came to a thai massage studio to have fun... but then he refused to pay and the sweet asian woman slit his throat with a knife.",
	"Well great, the TSX vampire bat became unkillable because apparently I triggered a bug by genociding the wrong monster.",
	"Stupid variant, one should really be able to levitate on the Plane of Water. I want to play variants that allow me to do that.",
	"Stupid variant!!! TSX is annoying, because there should be no monster movement energy randomization! Why did that have to be invented anyway?",
	"Well great, the top Sokoban level was 2 but it behaved as if it were level difficulty 10. God, TSX sucks, I no longer want to play.",
	"Died in Rodney's Tower because Elbereth is very hard to use there. I kinda like the Sporkhack Elbereth nerf, that one is really nice! Can't we port it?",
	"Thought I could tame peaceful carnivores by throwing food to them. Well, this particular one turned hostile instead...",
	"Collapsed underneath my load because stuff weigh way too much in vanilla Nethack. To quote Linkin Park: Why is everything so heavy? This is so much more than I can carry!",
	"Found out what the truth behind 'bundlebundlebundle' is but then Amy removed me because I knew too much.", /* don't worry, I wouldn't really do that :D */
	"A corpse found at the bottom of the purple poison lake was buried here.",
	"The corpse in this grave is very sticky. Poor guy entered the desolate chocolate manufactury and, well, drowned in a pool of hot chocolate.",
	"Impaled by Death's sickle.",
	"THE MONKEY DOES 4 DAMAGE! HE JUST JUMPS AROUND! WHAT - IS - DOING - SO - MUCH - DAMAGE - TO - ME - THERE???",
	"Here lies a professional suicidal person. Obviously he survived all those attempts and thereby became a professional in the first place... except that his suicide skill had become so high that he actually succeeded in killing himself now. RIP :(",
	"Here lies a professional suicidal person. He trained his art a lot, committing suicide in many different ways, but sadly he's no longer alive so he can't teach others how to perform a good suicide.",
	"Here lies a professional suicidal person. He committed suicide many times and is now really very good at killing himself.",
	"I was killed by the heroine Larissa Oldsmall in a large battle.",
	"The traitor that was buried here was killed by Larissa Oldsmall with melee weapons. And what was Larissa's choice melee weapon? A pair of Nike sneakers.",
	"Amy, I have a question. At this very moment in time I am standing next to a mold fungus. This mold fungus has no brain, no limbs, no opposable thumbs, it's not even very mobile at all. So can you please explain to me HOW THE FUCK IT'S PICKPOCKETING ME", /* by Crawldragon */
	"I ATEN'T DEAD",
	"Always look on the bright side of life.",
	"Always look on the bright side of life. Pfeifpfeif, pfeifpfeifpfeifpfeifpfeifpfeif!", /* I (Amy) find that song incredibly annoying... */
	"Life is quite absurd, and Death's the final word.",
	"Are you irritable, tired, depressed of life? Keep it up!",
	"Can I get a turn undead over here?",
	"Pour one out for me, will ya?",
	"The treasure is buried three spaces to the left.",
	"Edward d'Eath - Gonne from this world",
	"Excuse me, I ordered the cremation.",
	"Back in MY day we didn't have these fancy headstones; we had cairns and we liked 'em!",
	"Upgrade to a mausoleum with our premium membership package",
	"Oh sure, give the *gnome* a wand of death.",
	"Yet again I had to brave the castle without reflection or MR and yet again the first lieutenant had a wand of death.",
	"Hey, you! What year is it?",
	"He was #1.",
	"Dig here for a wand of wishing",
	"20XX",
	"I now face Elbereth and walk backwards into Gehennom.",
	"Here we go again!",
	"I shall walk this Earth again.",
	"I didn't want to go.",
	"No fair.",
	"Give Rodney a kick for me!",
	"Yet another victim of Master Kaen's training program.",
	"Damn, lost to Master Kaen even though that wimp hits like a girl called Frankenstein! The DISGRACE!",
	"No regrets... except for eating that kobold corpse.",
	"Jimbo the item farmer, got some brown pudding stuck in his armor.",
	"Sarah the valkyrie, thought she'd sit on a throne to feel less dreary.",
	"They told me that what is dead may never die, but I feel pretty dead!",
	"In this grave sits Astolfo the knight, his fault was he never ran from a fight.",
	"hashhashhashhashhashhash",
	"The following are up there for most obscure references in Splice.",
	"GartenzaunGartenzaunGartenzaunGartenzaunGartenzaunGartenzaun",
	"This one will probably become a scroll in some form later.",
	"IOWA WE CASK NIACIN WE IX APOGEE IOWA",
	"I would have liked to have seen the Astral Plane",
	"Use your light but awful chains",
	"That is not dead which may eternal lie.",
	"In strange aeons even death may die.",
	"In successful ascensions even death may die.",
	"Here lies valkyrie Blanche, she tried real hard to fight an avalanche.", /* Splicehack then changed that to say "barbarian"... but Blanche sounds way too sophisticated to be a BARBARIAN. So in slex she's still a valkyrie. --Amy */
	"Bob, killed by a zombie. Sure could use a turn, undead!",
	"This would never have happened in vanilla!",
	"This was all part of my evil plan!",
	"4.5 / 5 stars, would live again",
	"Sure is dark in here.",
	"Hindsight is 20/20",
	"Who dug this grave?",
	"What a world, what a world!",
	"Peter Fyde, tripped over a cockatrice corpse.",
	"HE COMES",
	"Lifetime guarantee",
	"Onward to the next adventure!",
	"Raise a glass",
	"Hey, who turned out the lights?",
	"Here lies a hero named Theseus, he was struck in the trapezius.",
	"Here lies the body of Ernest, he chugged some lava straight out of a furnace.",
	"Does anyone read these?",
	"Hey, you're proof that these inscriptions are indeed being read! Congratulations!",
	"Antigulp was here",
	"Here lies a mysterious force, dummied out as a matter of course",
	"Mystic scarab mass grave",
	"Mystic scarab mass grave (that means a mass grave for people who were torn to shreds by the scarabs, of course)",
	"Hopes and Dreams",
	"RIP the Lord of the Foocubi, few were sad to see them die",
	"LOL, GWWOD. YASD.",
	"A miserable pile of secrets",
	"Doh!",
	"Don't let my death be in vain.",
	"For sale by owner",
	"'Zombie life insurance' isn't all it was cracked up to be",
	"That's it?",
	"Failed the death warp",
	"he's a happy little sticky bouncing ball. maybe if i tickle him he wont attack me?",
	"They're after me lucky charms!",
	"Terrahack fleshes out its new races so much, you could almost still play them.",
	"Mmm, a sauna!",
	"Ouch! That really hurts!",
	"what was the reasoning behind making it so that rings can no longer erode even if they're iron and therefore should? it's not gameplay-relevant because destruction due to erosion isn't a thing in vanilla and it certainly breaks realism...",
	"what's next, Gehennom instakills you again if you're not fire resistant despite there being very little fire/lava at all?",
	"did this terra guy make it so that there's no land on the stupid plane of stupidity at all??? are you going to autodie when going there without magical breathing? WTF",
	"goddamn it what did terra do to the water plane??? I wizmode-levported to it, instantly drowned due to being in water, came back to life because wizmode and was teleported into a different water square where it now loops on 'Die? (y/n)' with no way out",
	"It's probably not a good time for a picnic...",
	"I bought an antibundling shield but I think I got fleeced.",
	"somehow I don't think of variant paranoia as applying to SLEX because the variant actually IS out to get you!", /* by ais523 */
	"Where does the insidious woman picture put?",
	"All this, because of a dream?",
	"Why are they doing this?",
	"I thought these religions had nothing to do with each other!?",
	"Lolth, help me!",
	"I can't control my arms!",
	"I can't stop!  Look out!!",
	"Cut me lose, please!",
	"It's so dark!",
	"Free me!",
	"Lolth, why have you forsaken me!?",
	"It was waiting for us!",
	"Kill me!  Please, just kill me...",
	"Could you tell me where the Amulet of Yendor is?",
	"I must leave now!",
	"I tried to catch an Entei and it used roar and now it's forever unobtainable, hence I suicided.",
	"Cursed are you who calls me forth. I damn you to bear my sign and my flames, alone in this world of darkness!",
	"Sometimes, what was broken cannot be restored.",
	"By your hand was this done, therefore you shall be stained by my blood.",
	"Phanto from Super Mario Bros 2 is a sow retard and I wish I could genocide his ass.",
	"Evil f-u-c-k-i-t!",
	"I hope you visit again some time.",
	"Free me from this place, and I and my brood shall fight for your cause.",
	"God does not exist. After all, he allowed the holocaust to happen, and if he really existed and cared about humans he'd have conjured divine lightning to kill all the fucking nazis before they can do their evil deeds.",
	"None shall rest until my vengeance is complete.",
	"Here lies Vera, calm and placid...",
	"You see here a python corpse. You feel that python is dead.",
	"i think i'm lost, hehe",
	"they're coming to get me",
	"tango down",
	"this is a strange world you've got here",
	"i keep hearing about the 'etherwind' and the horrible things that come with it...is it really true?",
	"1 killed. Who is next?",
	"Oh no. Not that!",
	"Arrrrgh! My one weakpoint...",
	"For the Demon King: Eddie Murphy!",
	"The nightmare continues.",
	"The maid jumps up and down",
	"I've been canned!",
	"She died by the bullet and then there were none.",
	"Unbelievably,I lose.",
	"Guess I flunked it.",
	"I should have quit while I was ahead.",
	"maydey meydey",
	"This was a mismanaged venture.",
	"This just ain't my day.",
	"I can say this all day. Let me vote! This is a national disgrace! Let me vo-", /* Red Dead Redemption 2 */
	"RIP suffragette, beaten up by Arthur Morgan.",
	"RIP suffragette, she got rolled over by a train.",
	"RIP suffragette, she got eaten by an alligator.",
	"RIP suffragette, she got dropped in a mine shaft and fed to the devil.",
	"Ruat coelum, fiat iustitia. Ecce, lex rex!", /* by Porkman */
	"Satan's Flaming Bollocks!", /* ditto */
	"Here lies Antichthon. Crushed underneath an express train from HELL.", /* with 50-odd egotypes */
	"if you play slex, you *are* a dickhead", /* by porkman */
	"Oh no! The brain drain!",
	"Spork Astral hits! You feel your game fading away...", /* by aosdict, after K2 suffered an astral splat :( */
	"After 20 hours of playing bigslex I suddenly ran into a damms and died in two turns. Fuck that stupid game, why the hell are level 90 outta depth crap monsters spawning in sokoban and randomly ending my run.",
	"I lost the run to RNG, dude!",
	"Fucking game bugs. The loadstone that the lich put into my inventory weighed 21600 units and of course my blessing technique was on timeout.",
	"Argh, tender jesses are overpowered. How the heck do you handle them?",
	"Why does the game constantly spawn lulu asses, anyway?",
	"Are you SURE that's a wand of wishing?",
	"Croesus?  Who's he?",
	"Who knows, how moves the people nowadays is? They are capable and kill one because allegedly a turning place belongs to them!",
	"I died in the cutscene! What the actual fuck!",
	"I'm out of water. I have no water? I HAVE NO FUCKING WATER??? There is no goddamn water left in my F.L.U.D.D.!",
	"I hate this stupid game *so* much.",
	"TAPE MEASURER! Who could ever need a tape measurer? GARBAGE!",
	"I tried to speedrun real life, but got stuck at the relationship section of the run.",
	"The most important part is not dying... wait what? NO! I DIED!",
	"Damn, I only have one hit point left! Well then, I guess I'll start randomly quaffing potions. Let's try the effervescent potion first.",
	"Uhh I think I'm gonna die in a couple of turns. Might as well read my unidentified scrolls, maybe one of them will save me. I'll start with the one labeled YO I PLAYED THE GAME AND DIED TO SOME UTTER BULLSHIT.",
	"Oh no, the mind flayer is going to kill me on its next turn because my intelligence was just reduced to 3! I think I'll zap this californium wand at it, maybe it's an attack wand that kills it!",
	"Engrave-testing wands is stupid, I'd much rather find out what they are by zapping them at myself. Let's see what this cavorite wand will do...",
	"What? You're supposed to engrave-test wands? That's stupid, a much better method is to break them since that way you'll identify some of the types that give no message upon engraving. Here, hold my beer while I break-test this ribbed wand...",
	"Josefine killed me with her velcro sneakers while I was unconscious from smelling her sweaty feet!",
	"Here lies someone who absolutely wanted to find out how far Lydia's lady pumps will go if one lets them. Well, it turns out that they can actually cause death by bleeding from terrible leg scratches.",
	"This fool was just standing there without retaliating while Katia scratched her block heels over his leg again and again until he died from loss of blood.",
	"This grave contains yet another poor victim of a pair of stiletto heels that managed to split his skull by beating on it repeatedly.",
	"Hey, I know the best way to get away from an annoying monster! I'll just start teleporting randomly! That certainly isn't going to land me in a more dang-",
	"Dammit, if this potion of healing doesn't heal at least as much as the rate at which the monsters are dealing damage to me I'll be dead.",
	"Noooooooo! That Amy bitch fixed my favorite exploit from an older slex version and therefore the new version sucks! I will only ever play games that don't fix obvious loopholes, because I can't handle a game that is actually challenging!",
	"Whaaaaat you made black dragon breath cost 100 mana? This is fucking unplayable, and I will go back to an older slex version now because I'm such a filthy heretic.",
	"omg ever since amy changed it so that digestion attacks dont always instakill monsters the game has become very sucky, why even play",
	"What? You actually die instantly if you spend too much time inside a purple worm?",
	"Say what, you're not supposed to hit mirror patches with a cockatrice corpse?",
	"Yeah uhh I was made deathly sick and don't know what a scroll of cure is, I just punched random buttons instead of familiarizing myself with my starting inventory before I started playing.",
	"What? The scroll of healing doesn't heal sickness??? What even IS this?",
	"Wait what? That can HAPPEN? In vanilla such a bullshit death would have been unthinkable!",
	"I wanted to grab Bowser's tail but missed, and ended up falling into a pool of lava.",
	"Yay I actually got a VC crash, that makes me so happy.",
	"Lost the run to a fucking cartridge tilt.",
	"Damn, why did the stupid game crash instead of simply loading the next area???",
	"Yeah uhh, the game just loaded and loaded and didn't actually get done with it, so I suspected that it had frozen and killed it.",
	"Oh damn, why did that wand have to be out of charges just now.",
	"The annoying thing with bows is that you constantly run out of arrows! How are you even supposed to play an archer role?",
	"The elder priest used an aimbot and a wallhack!",
	"I tried to do wizard farming but the wizards just kept casting slow on me until I was so slow that they proceeded to actually kill me.",
	"Damn, the thief speedup bug is back! Quick, everyone, run for your lives, children and women first!",
	"Man this is such a stupid bug, the thief tried to unsuccessfully steal my money four times in a row but then he suddenly ninetuple-turned me and I died from full HP.",
	"Why does the game put up warning messages and then freeze if I cast sleep monster, leave the level that the sleeping monster was on, and return after a while?",
	"What is wrong with the game developer? I didn't feel like picking up every stupid item on the floor, so eventually a lot of crap accumulated from killed monsters but I didn't expect the game to run out of memory if there are too many items!",
	"Why are the 19BE trips capable of magically attracting teleports?",
	"Stupid level bug, why can't that one go on vacation for once.",
	"Why did you save the game while engulfed? Some genius you are!",
	"Amy, I'd advise you to remove all your stupid monsters and leave only the ones from vanilla, SLASH'EM and dnethack in the game.",
	"Umm, guys?  You know I just cast epic-level Hour of Silence, right?  None of your spells have any effect whatsoever.",
	"evilhack is 'with a purpose but balanced and challenging' evil. slex is 'bwaaaaaah i'm on acid I smell purple lets add n to the x things weeee' evil", /* by K2; obviously I disagree with him on that one, because I view slex as very balanced (yes, seriously! just because the game is unfair and tries to kill player characters doesn't automatically mean it's unbalanced) --Amy */
	"I didn't know cats could break the illiterate conduct.", /* by Hekik */
	"Boss bug. That's all I'm saying.",
	"This is so stupid, if you warp to the black moorland and explore in any direction you'll immediately encounter at least 4-5 bosses and also a bunch of champions, so you end up getting owned with no chance!",
	"What the hell, why does the monster blizzard spell deal 600+ damage even though I'm cold resistant???",
	"Wow, the beautifully sharp claws really expenditure-feasted my soso.",
	"A SEVERE ERROR HAS OCCURRED - my merc is gone, his equipment is gone, EVERYTHING is gone! So I decided to suicide my character.",
	"Suddenly the game crashed and produced a broken save. I tried to load it again but failed, so I hex-edited it but then trying to load it resulted in an error message saying that my merc had impossibly high stats. How the hell am I supposed to fix that???",
	"SUCK! I died to an upper-crazy hi-jork even though that's certainly not capable of dealing so much damage in one hit!",
	"I died on purpose because I had wasted my character's skill points on the wrong skills, so I just stood next to a bunch of carrion birds to check how long I survive if I don't fight back.",
	"The assassin role SUXXX OVER ALL!",
	"My merc was so stupid and cast inferno 50 times at a lightning-enchanted boss that stood right next to me, so I got hit-recovery-locked by the lightnings and died.",
	"An urdar slammed shut.",
	"I got defeated by an exploding barrel! Because I absolutely wanted to see what the death message would be if you die to that. It was close though, I'd almost have died to a hungry dead potato instead!",
	"A shaman boss who thought he was Colenzo offed me.",
	"I didn't run away from the dung soldiers' lightnings, but simply bashed away.",
	"Mage boss. I guess I don't need to say anything else.",
	"An army of pleased shamans with fanaticism boss. They showed me how to defeat an opponent, only that the one who was defeated was ME.",
	"Man, I have no idea how I died! I just wanted to re-summon my grizzly because he had died, when all of a sudden about 5 champions appear and all shoot arrows at me and they all hit - DEAD from 2000+ HP! I can't believe it... now I have to start all over again with a new char!",
	"I was fighting the armorer and it seemed to be going well, but when he was down to half health he suddenly started to chase after me and was already as fast as a car. I'm trying to lose him and run up a stair but remain hanging, the guy hits me, bam, dead. That was stupid.",
	"There's not much to say other than that the enemy was lightning enchanted.",
	"I was stupid enough to engage the rogue captain at experience level 3. After the first arrow I had lost less than half of my HP so I didn't expect the next arrow to kill me.",
	"My necromancer was busted by the cops because I didn't watch out. Why do I have to undergo hit recovery for half an hour when I get hit, anyway?",
	"This is impossible. I had lightning resistance and over 400 HP, and died to two or three lightnings from a death beetle. From full health.",
	"There were hundreds of black souls and stygian dolls and I ran out of healing potions. I was hoping to just run through between them but it didn't work out.",
	"The dungeon is a murderous place! Even with good fire resistance you're still not adequately protected from the soul deadmaker shaman's flamethrower!",
	"Due to the weird vipers standing in the way, I was unable to move away from the wall of fire in time!",
	"Due to my armor being too heavy, I was unable to evade any attacks!",
	"I lost to a fire enchanted Ruth, because I thought I could survive with sufficient fire resistance. Apparently you can't.",
	"Well, without a merc the necro can do nothing, and he also never has magic resistance despite a shield that gave fire and lightning resistance.",
	"There were a lot of light worm champions, so I wanted to finish them off with my sword, but suddenly the boss whom everyone knows came with super-many minion spiders and cursed me, and then I couldn't get away in time. Maybe experience level 23 was too low after all?",
	"I was quite stupid to not run out of the ultra fire wall despite seeing that it's there.",
	"The corpse collision bug shouldn't happen just in the situation where an extra strong cellar master is standing right in front of you and you can't whirl due to lack of mana.",
	"Totally the many bears, deamon steeds and suchlike, and the resurrection bill for my merc was too expensive.",
	"I died to the absolute boss of the entire game, in a single hit despite having over 600 HP, maxxed lightning and poison resistance and over 900 AC. Well at least it was a beautiful death because a girl killed me!",
	"I died to cheap opponents! Why the hell does the healing potion take half a century to heal me?",
	"It's not very easy to run through an army of duriels and turbo clubmans if you only have one hand to control your character because the other hand is busy holding the phone.",
	"weird, why do rock worm champions do so much damage? There probably tunes which not!",
	"why does the barricade have so much health? and why does my girlfriend have so little? and above all, why can't I just whirl through barricades and other obstacles!",
	"Oh no, I even had one hit point left! But if my guy continues whirling instead of stopping to heal, no wonder I die...",
	"The works, I had no fire absorb and you can guess what happened next. Man, this is annoying! I saw the ultra fire storm! Why did I walk into it?",
	"I died because the wood head does totally much damage and the chance to avoid the much damage is close to zero.",
	"I died due to the mistake of getting too close to a sand warrior that controlled all three elements.",
	"Suddenly Duriel did a charge attack and killed me. All the time he didn't do one while the valkyrie was still alive!",
	"Yet again I died to wig sheep champs, when I tried to get to a yellow light bulb.",
	"My ama had way too little health to survive such a super-cold-damage leap attack.",
	"usually I finish off all the sparkers with negative lightning resistance but this time it did not go!",
	"I died in the city of the condemned potatoes because, what else, there were too many mages and also 2 bosses of other annoyers.",
	"weird, despite 500 HP and good resistances I didn't see any red in the life orb after one inferno, and the next hit killed me.",
	"I died to a bug! But there were so many bugs that the entire screen was filled with them and of course I couldn't block any attacks due to missing a shield!",
	"For once I DIDN'T die to a mage in the city of the condemned potatoes! Instead I died to a super rocket from a black muzzle boss that I couldn't evade due to a bug!",
	"Oh no, so you can actually die to iron maiden after all! This never happened to me before!",
	"man, this is annoying! It wasn't even the upper-strong one! I wonder whether the upper-strong one would have killed me from full HP!",
	"How are you supposed to evade the spell ammos without stamina? Right! Not at all! Instead one must allow themselves to get hit because evading is impossible!",
	"It was stupid of me to expect being able to survive more than one hit.",
	"Three suicide bombers all exploded at me and I died.",
	"Died to an upper-bad multisparker! What are you supposed to do if the lightnings shoot at you faster than a Ferrari?",
	"Died to a priest.",
	"Oh no, I had already killed the armorer! But then hundreds of badguys appeared from nowhere!",
	"Yet again a multisparker killed me, who was additionally capable of shooting cooling rockets.",
	"I died to upper wimps! But I didn't manage to get away from them in time!",
	"My F2 hotkey wasn't set to 'summon golem' as I had thought, but to 'aging'! So I didn't have a golem to protect me from the GTS boss with fanaticism, who then proceeded to one-shot me.",
	"I died to a single super rocket, because the choice muzzle does totally much damage. All the previous runs he didn't even hit me at all!",
	"Damn, I died because the boss narrowly hit me just before I could warp away via portal.",
	"Dead because the boss was capable of teleporting!",
	"Might aura, what the hell are you supposed to do about that.",
	"That weird potato guy who firecrackers completely weird cold. I had too little health and therefore died very quickly. And of course my crossbow never hit him either.",
	"After the nose enchantment explosion I only had about 3 HP left, and then the last remaining green zombie hit me of course. I shouldn't be playing this character if I'm so wimpy to die to that.",
	"Totally the many beer drinkers and duriels, which cannot be defeated if you only do 11-13 times 11 (at most) damage.",
	"Cause of death? Because the beautiful Ruth's had a boss with way too many HP.",
	"upper-many enemy types in all corridors, so I couldn't town portal out anymore. But if I were to use it to warp back in I'd die instantly, and again through the grave mill is no fun, so I delete this char!",
	"Due to the wall being in the way, I couldn't run out of the ultra fire wall!",
	"If that red whore would at least have died to my poison gas, I'd go on playing! But because I no longer have a girlfriend now, I fire a function in golden status!",
	"Because my brother was faster than me, I had to delete my character!",
	"I lasted for quite a while! But the stupid Duriel always headshots me, what the hell are you supposed to do about that?",
	"Man, the minions are so stupid! And why do the stupid healing potions just not work? And where did the hundreds of enemies come from? Now I'm dead because of a smaller grotesque! The Ro-asking would now say 'Hea! Lol!'",
	"I overestimated my abilities.",
	"suddenly the Talic used a stupid combat command cry, and then all three grandpas hammered on me at once, and even though they were not cursed I lost 1200 HP in an instant. Actually all three of them should have been wimps. So, everyone, watch out: those guys are mean!",
	"The buddy was dead, and then there were completely many flayer annoyies and suchlike, and there I couldn't do anything. And the town portal wasn't usable once again.",
	"Suddenly the elephant used an earthquake and I was almost dead, and then I was so stupid and ran into an inferno with negative fire resistance.",
	"I killed myself with an amulet! The level 6 raise skeleton effect took off 34 of my HP with every cast, just like that, and the mastery I had on the same amulet didn't work!",
	"The level 4 traps were meaner than expected. They constantly fired super rockets and then there were the ultra fire traps as well, what the hell could I have done there.",
	"Killed by Horazon, what else, because the assie made a firewall. There was nothing I could have done about that.",
	"I was dead before the game had loaded the area I ported to. There was a steel beetle boss who died immediately to my minions and took me with him, that's something I can't do anything about.",
	"what the hell are you supposed to do if the entire screen is full of storm callers and all have fanaticism and are blocking the portal.",
	"because the upper-bad council bosses had even worse hydras and fully the bad 'extra annoying' and then still holy cold weather. There you can do nothing at all.",
	"Because the hugging-jessica was so wimpy and died twice in one minute, and I only had enough money to repeat her once, I surrendered. I mean, there's nothing I can do about the hostile girlfriends. That said, I might theoretically have done it, but it would have sucked in any case.",
	"I even survived the holy static field, but then that guy had such a stupid fire explosion and of course I had way too little HP so I died yet again.",
	"because the cave speeders were so super bad and showed me where it long-goes, I lost.",
	"because I cannot leave a wo rune lying on the ground, I had to grab it despite monsters beating on me from all sides, and then I died just as I wanted to ESC out of the game.",
	"There's nothing you can do when fighting Mephisto. I think his attacks should be made weaker.",
	"I had too little health and was shot by evil archers.",
	"How much damage did THAT do? Not even in the regular version of the game the fire nova does so much, and I even made it weaker! There genuinely tunes which not!",
	"after a battle with devilish potato shamans I was down to single-digit HP. Normally that's no problem, normally you press the '1' button and have HP again! But if the hand that presses the '1' button cannot reach for that button and you have to click on the healing potions with the mouse, you need to be lucky that the arrow doesn't hit you, and in this case it obviously did hit!",
	"If this dungeon hadn't crashed the first time, I wouldn't have encountered menises at all, and then I could have done it!",
	"Man, why is 'got immured' not a valid cause of death? There's nothing you can do, as there were way too many stygian dolls and even more shamans, so I just didn't have a chance.",
	"I turned on the wrong aura and was immured by the dog generator and got killed.",
	"if you become super slow and from farther away fully bad fireballs are shot at you, there's nothing that you can do at all.",
	"there one can do nothing at all. Actually Bremm Sparkfist is bad enough because he has conviction! One should remove the lightning enchantment, that's for sure!",
	"Huuuuuuuuuuuuuuuuuuuuuuh, so he was upper strong... actually I should have realized because the merc can't be dead instantly to a wimpy champion. But just before I could go through the portal, I died.",
	"the explosion is overpowered... But the suicide attack was the only chance to rescue the buddy! Otherwise he would have been arrested!",
	"bah, that goooooooooooooooooes not at all... I think the cave speeders fly out or are changed into something that only comes in the kagero dungeon! But the upper wimp healing potions are also still way too weak, those should also be improved!",
	"oh no, always that stuuuuuuuuuuuuupid explosion... wanted to run away, but the explosion range was too overpowered, just like the damage... I would have been better off in werebear form I guess.",
	"I only had one HP left, so I was almost dead, but then I was really dead because the mean fire took off my last HP.",
	"before the pope was even visible on the screen, he already used a multishot blizzard and I was dead on the spot.",
	"so many mean traps at once and then also a refuge Uhlert, so you certainly don't get the idea to stop. There it's almost guaranteed that you're hit by at least one ice arrow trap.",
	"guess what... it was the second blizzard that was shot at my paladin, too. I didn't lose just my buddy but also a hugging valkyrie.",
	"huuuuuuuuuuuuh... so that does over 600 damage in one shot? Okay. Thank you for coming. Either you don't encounter roller coasters or you can do nothing at all. I really had over 600 HP and was dead instantly. Why don't they just shoot an orbital laser that instakills you no matter how many HP you have?",
	"there were way too many pellet mages and they shredded me in seconds because my resistances are way too low.",
	"Okay, so that's a thorns aura! Am I supposed to kill him with the box fist now or what? Or am I supposed to use a grenade launcher that requires 10 hits for a fallen because 9 of them don't deal damage?",
	"there was a leftover trap bug, and it caused my immediate death. Only after I died I saw what had killed me.",
	"well super, what am I supposed to do there. Actually I didn't plan for the grenade launcher shooters to come there, so due to a bug both types of shooters could come and that is simply too hard. I mean, the area was hard enough already, so this should be changed such that only the fire arrow shooters come.",
	"the sand warrior didn't take all that long to remove me with his lightnings.",
	"because there were so many mean baddies, I was immured and could only use the upper-wimpy arm chest that has to shoot at least ten times for a single opponent.",
	"died because the annoyxplosion apparently did over 300 damage. But at least all the badguys that were there are also gone.",
	"at least this mercenary bug didn't appear for eons. Actually I should view it as a 'feature' from now on: if you're so bad and let the mercenary die too often, it can happen that he's dead for real. But why does it always happen in act 5 near Baal?",
	"the menises transformed me into parts shortly before I could go through the portal.",
	"the MG shooter was way too good and therefore won against me.",
	"had picked myself back up not even a minute ago, and then the new overpowered poison princes come and first make the buddy in less than one second away and then me, because I'm not resistant and have less than 300 HP.",
	"such an old error, the gives it not at all... but why does it aaaaaaaaaaalways only happen when I'm playing a barbarian? Is it because I had a helmet with +2 leap attack?",
	"the boss of the demented shamans was so mean and scored a single hit kill, which means he dealt over 500 damage in one hit. I don't even want to know what bad combo he had, all I can imagine is extra strong and then still holy statics fields.",
	"the clanoutern had too many elemental enchantments and always stunned me when I wanted to run away.",
	"that's not something you can tell me that this does only 30-60 damage. That's really something you can't tell me.",
	"the shamans are just way too good! If my brother pretends only once that they were only 'pseudo-improved', I'll demand that he plays this version at once!",
	"there were suddenly at least ten guys with rocket launchers, and I didn't want to do an emergency exit in order to not have to play through all the stupid way again, and therefore I'm dead now.",
	"the Lister barely got me away because he's so mean. Apparently it was the poison that killed me. Man, is the poison enchantment mean! How much do we bet that the buddy is once again not revivable now?",
	"I fire a fuuuuuuuuuuuuuuuuuuuuuuuuuunction! Oh, and I also won the bet, as was to be expected. Now wuerd I times say, that I take out the poison enchantment (what else is there to do)?",
	"wanted to try out how it feels to get the hands kicked bloodily by the friendly bundle-Elif.",
	"actually that was stupid of me. Now I'm dead because of freeze sphere, because I had thought the trap was dead already.",
	"the nec even survived two giga freeze spheres, but then there was another such trap in the chest and at first I didn't even realize I had opened a trap, when I was already dead.",
	"I attacked the annoying trap, and I would even have finished it off but the golem was hit too often and so the trap still gunned me down.",
	"the freeze sphere ball hit me and I was dead instantly, with 283 HP and some cold resistance mind you.",
	"there such a baddie did a suicide attack and some other baddie gave me the remainder.",
	"degree so, I wanted through the town portal and was away-made by the consul boss.",
	"I tried to talk to the mercenary vendor NPC and the game just ceased and desisted, with no message.",
	"The bugged Diablo 2's in town caused the game to crash. Argh.",
	"Man, it seems there's always a rock worm queen in a specific place of the chaos sanctuary and the game always crashes whenever I go near it! How do I generate a new map where that beast isn't there?",
	"the bundle-Elif her sneakers hit more often than my h-hydrogen, and therefore my necro bled due to the wounds that the lovely Elif-flax opened.",
	"the trap owned me, i.e. apparently it was the thunder nova that took off my last HP. But that wouldn't have happened without the rocket launcher traps.",
	"That gives it not, I've never seen it happen that one strains in town! Otherwise as does the mercenary just have 0 HP and lives! I wonder if that only happens for the guy from act 5?",
	"the golem was somehow not where I summoned him, and therefore the Horazon just shot me over. Can't do anything about that.",
	"the refuge boss killed me right away, but actually I should have survived that with half of my maximum HP... I should have aged him! Can someone tell me why I didn't do that?",
	"yeah yeah, I know, with level 36 you don't enter the kagero, but what am I supposed to do if the fifth act is so easy? Additionally I'd have defeated the ice arrow archers if I had been able to get out of the room again, because then they'd have lost to my poison creeper, all of them!",
	"lost because of the cold explosion. And yeah, I know I'm upper wimpy, but at least the dead dictator is also dead.",
	"I didn't even see the arrows. But I realized that they hit me by reading the message, which said 'They died - far ones with pressure on ESC'.",
	"THEY DIED - far ones with pressure on ESC",
	"GAME OVER - return of Ganon",
	"the gatling gun rattled too quickly and my cold resistance was too bad and I overestimated my char's abilities.",
	"the buddy refused to find the way. And I stand before the Horazon guy and hammer on him and just keep missing.",
	"Buuuuuuuuuug! The game should be modified such that one isn't constantly underleveled.",
	"because the prayer aura eteeeeeeeeeernally slowly recovers my HP, I used an upper wimp healing potion (there was no red left in my life orb), which obviously didn't do anything. Then I wanted to use a reju and it also made 'gulp-gulp-gulp', but I was dead anyway and afterwards the reju was still in my inventory.",
	"the buddy simply fell over after Radament was dead already, and the buddy wasn't green. No idea how that works. But he was still in the red range before.",
	"and yet again the buddy fell over, when there were no opponents left, and again he wasn't green. But it only seems to happen for Radament-type opponents, because previously he barely managed to win against some annihilators and then he was completely badly in the red range.",
	"the consul finished me off.",
	"MLEB boss. That happened so quickly, not even a fuller reju would have helped there.",
	"Boss bug! There I just say BUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUG! I just wanted to do an emergency exit but I was already dead. Bug, is all I'm saying. That is a single old error. Let's see if I can't do something about that, can't be that all bosses are so overpowered...",
	"I'm just too slow with the emergency exit. Therefore the mage boss with holy cold weather gunned me down together with the condemned ones.",
	"bah, one can stop make nothing, if the buddy is extra stupid and there's constantly the boss bug.",
	"the buddy allowed himself to be killed and then I still was so stupid and ran into a menis army.",
	"Suicide type makes it so that there was no red in my life orb anymore, and all five fireballs that came from the left hit me. Not even a healing potion 5 can do something against that.",
	"I had thought the fanatic triad wouldn't ever deal damage anyway.",
	"the pellet archers were too good.",
	"upper-many feature types in all corridors, and because of the blood racquets was also nothing with through-running. Therefore: Game over!",
	"Multishot pellet archer, and that his freezing arrows hit all three of course, from that close.",
	"Yeeeeeeeeah, I still quickly collected the Wirret's leg before the argonaut away-made me!",
	"shortly before level 5, the upper wimp healing potions were just too slow-assey and the fire resistance doesn't work ever anyway.",
	"the one suicide type made it so that there was no red left in the orb and the next one killed me because I was immured.",
	"just as a yellow strengthened pike dropped, everything went totally fast... at least the sub ghost that away-made me died at the same time.",
	"the ice arrow archers kill one always instantly... annoying! but oh well, can one stop nothing make.",
	"lightning enchanted thick potato, and I realized too late that healing potions aren't fast enough. So I want to use a reju, but like always there's the total bug that I'm dead while it makes 'gulp-gulp-gulp'. The reju is now - who would have expected it - still in my inventory.",
	"too many melee mages. Actually I wanted to run away and let the spirit tank (yes, I have a thorn spirit), but annoying hit recovery lock...",
	"bah, that with the reju simply sucks... hmmmmmm... I think I know so slowly what is loosely there... LAAAAAAAAAAAAAG! Because the game is adjusted such that everything is registered half a second later, the reju quaffs itself later too!",
	"I know, one may not be so wimpy and when you die in act 2 normal you should be penalized by having to start over from the very beginning... but ey! What can I do about it that the healing potions of course take themselves half a second later again and you can't charge from so close?",
	"it annoys simply fully if you still don't have good lightning resistance at that point.",
	"apparently the sniper assie pierced or so... the golem definitely stood before him and he shot both the golem and me at once.",
	"bah, that deserts perhaps times on... Single hit kill in normal with 1000+ HP... o-ver-po-wered! Why do the suaels have so many lines anyway?",
	"Noooooooooooooo... Single Hit Kill after Baal by a suicide type...",
	"I didn't even see the grandpa take a swing at me, but I was just suddenly dead. Well, if you only have 1500 HP and are cursed...",
	"constantly clicked beside it, therefore I didn't get back to town and was busted by the cop champion instead. But at least I got the waypoint.",
	"I know, I'm upper wimpy: died to the cold explosion. And it was because it hit twice again. Only the second hit was deadly.",
	"well, if you have negative resistances and the shaman is lightning and cold enchantment... after the first arrow I barely had any red left in my life orb, then the next arrow comes and I try to dodge, but the arrow is there already and kills me.",
	"yet again such a weird shaman... that a champion can take off over 400 in one shot? *be confused*",
	"so what of cheaty. If the arrow hits the mercenary or another minion, it never kills in one shot, but my character is always killed in one shot. If even the mercenary dies in one shot, the arrow would deal twice as much damage as the mercenary has HP in case it hits me. Of course I'm always dead instantly, there one can simply nothing A T A L L do.",
	"lightning and cold enchanted boss, and my resistances are in the cellar so it didn't last long.",
	"baaaaaah, I idiot hit the wrong button... FUCK! I'd have slaughtered all the nerve heads if I hadn't been so stupid!",
	"because of the steel beetle there was absolutely no red to be seen in the life orb, but I quickly paused. Then I thought 'so, I'll unpause now and immediately use a few rejus, then I make the two boss baddies away', but I was immediately dead before I could use the reju.",
	"static fielded all the time, but the shaman boss with nose enchanted and something else shot down the mercenary wimp and then hit me, and I didn't have much HP remaining. So I quickly tried to run behind the wall to get to safety but I think I was cold weathered and therefore got shot instead.",
	"err, how quickly did that go... was just a couple wimpy dungeon lords?!",
	"probably I knocked the Miriam her cube away (yet again the second time in a row...) and the Bettina her aura somehow doubly static fielded every time, and the rejus were out quickly and three-healing potions were too slow.",
	"Huuuuuuuuuuuuuuuuh, huuh huuh huuh, huuuuuuuuuuh, huuhhuuhhuuhhuuhhuuuuuuuuuuuuuuuuuuuuh huuuuuuuuuuuuuh, the flying scimitar killed me, even though that can do nothing... with those one probably needs to watch out really well or take ranged weapons, which a barbarian never has...",
	"They died because of chain thunderbolt!",
	"Man, the chain thunderbolt is fully overpowered if the enemy uses it... the types belong into the kagero and not in storyline areas! That's something I noticed now!",
	"got flooded and had no chance at all.",
	"yet again got uber flooded... o nou, and I already wasted all my healing potions!",
	"is stop already stupid if you have no, and I mean really NO, item that grants resistances.",
	"I was down to 2 HP and there was like always the Reju-gulp-gulp-gulp-bug. Actually I still live. :-P",
	"the wimpy baal taunt made me away, those probably got much meaner too now... actually stupid of me, but no matter. At least now I know that one has to really watch out with those and may not simply stand there!",
	"baaaaaaaaaaaah... Reju-gulp-gulp-gulp-bug... actually I quaffed the reju and saved myself. :-P",
	"degree so been too slow with the emergency exit. What's up with that anyway, menises on level 1, there one is fully powerless... annoyance!",
	"all of a sudden the spider assie again made correctly fast and because he was extra strong and my shotgun doesn't do enough damage, he still has one pixel of health left and I have none.",
	"the boss hit me even though the wall was in his way, and I only had 15 HP remaining. So of course I wanted to run away quickly, but the mean spectre flew over the wall and killed me.",
	"Yeah, at least the cave error that killed me still died to poison! Harharharharharharharharharharharharharhar!",
	"been too slow with the emergency exit and overestimated my abilities totally. Somehow I always die way too much when playing necros, this cannot be that I'm so bad at the game...",
	"eeeeeeeeeeeeeeeeeeh... Single hit kill... slowly I'm beginning to wonder if I should really keep playing this role, maybe I should rather roll a different char...",
	"there I just couldn't do nothing at all anymore. Such rockets always hit from so close and pull off completely much, therefore: Game over!",
	"hmm, somehow I'm of the opinion that the MG types are quite OP...",
	"that is completely bad! Actually I rejued myself, but the bone beetle still offed me even though I had lots of health left!",
	"I had 170 HP left. A single shot and I croaked. Now I'm curious what their normal damage output is, certainly about 17 or so... well at least the girlfriend still quickly killed the nerve head type *after* I was already dead! So if their base damage isn't at least 30, oh wait, I can answer that myself... okay. Bugfix = new character. This one attacked way too slowly anyway and had much too upper-slow-ass hit recovery.",
	"dead-made because of an amok-running yardie - one such suicide dwarf got me and I was down to 85 HP, and because my druid just automatically transformed back into a druid the next explosion killed me instead of bringing me down to 1.",
	"I didn't watch out and therefore the wig sheep with multishot overwhelmed me... that went completely beautifully fast, I was down to 1 HP and in the next instant to 0. Hmm, maybe I should start over if I'm that wimpy? But on the other hand I didn't see wig sheep bosses for ages in the dark forest, maybe that's why I rarely died there...",
	"eeeeeeeeh... I thought dying with experience level 8 was early, but I was too stupid to not use any healing potions and suddenly the pas op came charging and shot me instantly...",
	"I think I should make a real character... 47%% chance to block cannot save you if you don't have enough HP.",
	"eeeeeeeeh, is thaaaaaaaaaaat annoying... the rejus were out and he always only used the stupid red breath and without stamina you can't go out of the way!",
	"that was practically a single hit kill, even if it wasn't the suicide type that killed me. Boah, is that version of the game imba... thankfully the recent version isn't like that anymore! Now I also understand why my brother always said the old version was so imba! Because it is!",
	"died to curiosity - I thought I might have a chance against the refuge Uhlerts...",
	"if that wasn't a Reju-gulp-gulp-gulp-bug...",
	"the stuuuuuuuuuuuuuuuuuuuuuuupid portal never appeared where I wanted it, and I was fully stunlocked and the potions stop didn't last forever.",
	"weird, there was still one HP displayed, but I was dead anyway... bug with the display maybe? in any case the archer boss with spirit hits and lightning enchantment defeated me.",
	"Rejugulpgulpgulpbug like always, but even if the reju had taken itself I'd probably still have died. Maybe I should the boss - right, idea! the boss could come to the kagero dungeon! Because it's okay if a boss with power comes there!",
	"Single hit kill, probably the air death had such a gas to critically hit me.",
	"the mean rock worm egg killed me... was the stupid thing cold enchanted maybe? In any case my life orb emptied so fast, I couldn't even say WTF!",
	"Mercenary bug, and the veras servant made me completely slow-assey and all the uncurseable enemies then showed it to me. When I was dead already I wanted to use a reju and like always it made 'gulp-gulp-gulp', but I was dead all the time.",
	"Shitty bugs! One cannot die in town! Give me back my 100 zorkmids, you accursed game! As if it wasn't bad enough that the game froze a little bit ago when I used shock field... today is really Friday the 13th, I'm starting to believe in the bad luck!",
	"no mana for prayer and the finger that presses 1 was way too slow. Because I actually had my left hand between my legs wanking off while the carrion bird claws hacked me up, not realizing that they were actually deadly.",
	"bah, the annoying d-e-a-t-h had so much luck to single hit kill me... now in hindsight it turned out that those their boss was cold enchanted!",
	"overpowered nose enchantment bug explosion! Which buggard smoker assie thought that up? I should have realized back then how to implement the bugfix, because then lowrate would also have it!",
	"Mercenary dead, no rejus left, boss with extra fast and cold enchantment and the upper wimp healing potions never go anyway. I'd really like to program it such that those work correctly.",
	"as far as I know the boss of the hell cats had spirit hits and the poison of that does damage so quickly that the game doesn't realize one cannot die in town.",
	"been overwhelmed by the boss group and been too slow with the rejus. Weird that they deal so much damage if the boss is only magic resistant...",
	"Moved shaman boss with death combination, that went abnormally fast... I want my old armor back! Because the new one has way too few resistances and everything!",
	"I was insta-dead because of a boss of the devious girls with extra fast cursed, actually I wanted to redeem a teleport right away but one stop cannot react so quickly.",
	"in the moment where a load of ice errors was around me the armor went at the ass, apparently... it had only 1 dur all the time and was somehow displayed red already, but then in the crucial moment it didn't have durability anymore! Now in hindsight my suspicion is that items with over 100 dur will bug the display, just like in D1 where Griswold couldn't always repair such items!",
	"Baal taunt champions forced me to run past everything and I remained hanging at Juen flax. Of course a soso doesn't survive such a load of immolation arrows.",
	"it displayed 'Direct hit!' - as far as I know that was a chaos ice trap... since when do they take off so much health?",
	"baaaaaaah, the baddie suicided... eeeeeeeeeh... is perhaps times annoying!",
	"realized too late that the shaman had multishot and extra strong!",
	"blaaaaaaaah... overpowered thunder nova, and I had actually done an emergency exit already but because of the buggard smoker assies their LAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG it of course realized only half a second later that I had pressed ESC long ago already!",
	"murderous stairs trap with extra strong mana burn cop boss, who stood so stupidly that I couldn't go anywhere, and he had way too much luck, the bastard, because the one cop that blocked the way just barely didn't die quickly enough!",
	"blaaaaaaaaaaaaah... the boss with death combination cheated... he was dead and I was down to 1 because of his nova, but then the nova cheated me away because like always it hit completely often!",
	"eeeeeeeeeeeeeeeh... Single hit kill... slowly I'm beginning to realize that the wimpy druid isn't ready for hell...",
	"two immolation arrows from up close, which obviously both hit and it was over immediately. There one can simply make nothing, because who expects that a boss comes out of nowhere? Now in hindsight I saw that the boss was extra fast, no wonder I didn't see it coming!",
	"small sand maggot with death combination and conviction, I was immediately down to 13 HP, then down to 1, and quickly tried to use some rejus but it didn't go, instead I was down to 0 of course and the damn muck cattle just has a pixel of health left! Condemned still times!",
	"is perhaps times buggy...",
	"the grand vizier was extra annoying and suddenly made fully fast and owned me!",
	"baaaaaaaaaah, just shortly before I could use the portal a pyro came out of nowhere and blasted my one green HP away... baaaaaaaaaaaaaaaaaah... who makes it so stu-u-upid that the portal takes hours until it finally develops? Baaaaaaaaah...",
	"Boss bug... let's see if I even create it to away-collect myself again...",
	"baaaaaaaaaah, there was a second boss... that was lightning enchanted and extra strong, no wonder I died so quickly! But actually I had done an emergency exit and because of the buggard smoker assies their LAAAAAAAAAAAAAAAAAG it didn't realize that I had loooooooooooooooooooooooooooooooong pressed ESC!",
	"OMG, suddenly something fully hit me and I was almost dead, and before I could use a reju I was completely dead. There were shaman minions and apparently the boss shot from eternally far away and also hit me, and he was lightning enchanted and my resistance is rock bottom!",
	"blaaaaaaaaah, such a shit however too... the stupid bow always selected some dumb man types and therefore my guy didn't run! And even though I used fully the many upper wimp rejus, the bastard assiebassies quickly made me away even though I had LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOONG pressed ESC!",
	"the portal was blocked and I pressed ESC as always... SHITTY SMOKER ASSIES of Buggard! Actually the game paused and I left before I died! Such a SHIT however too!",
	"forgot that I had negative resistances to gloams...",
	"Single hit kill by a MG type with single hit kill enchantment from two screens afar.",
	"the bastard of a council boss single hit killed me...",
	"Eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeerror! If I click the town portal the guy has to please go through it, and if I press 4 and it makes 'gulp-gulp-gulp', please hit points have to be there again instead of a red screen being displayed!",
	"Steam mages. Before I realized that it gets fire-dangerous I was already dead, so I couldn't quickly quaff a reju.",
	"the bastard killed me with ultra charring in less than a second because I have no absorption.",
	"one-hitted. Such things happen.",
	"I was fighting a fully bad fire golem boss with 'specially strongly' and would have defeated him too, but then a boss of the devious girls with nose enchantment came and at first I pressed 3 because like always I was so stupid and thought healing potions could still fix things. Then I was dead and still tried to press 4 but of course it was too late already.",
	"one-hitted by a demented shaman, no idea what overes he had. Probably upper strong.",
	"that went way too fast, because that is too overpowered and the duriels are also way too good.",
	"Reju-gulp-gulp-gulp-bug! But even without the upper bug I would have been dead, because a single reju can of course change nothing if five KSK types come out of nowhere. GAME OVER! And I was soooooo close to finishing the kagero...",
	"Single hit kill via boss with 'specially strongly nose enchantment', that his first hit already almost killed me and I hammered on keys because of course I forgot that healing potions work as rejus now. But even though I was already full after that, he instantly killed me in one hit. Can one stop nothing make.",
	"Boss of the dark creature tinies with spirit hits, nose enchantment and holy cold weather and boss of the elbow contractors with extra strong cursed and aura enchantment, and the aura was prospering, and my teleport didn't go off quickly enough and the upper wimp healing potions were way too slow. So the assies assied me away. In hindsight it turned out: one screen farther there was another bell back boss with conviction! That explains quite a bit!",
	"didn't notice that the spider got a might aura from another boss while I fought a boss of the fanaticians, and got one-hitted.",
	"WETEEFF??? I had full HP and there was a steel beetle boss with multishot and enchanted nose, and I stood half a screen away, but completely suddenly I was simply dead, from full to zero, with no warning. Even though I actually have quite good resistances.",
	"Bah, the single hit kills are perhaps times overpowered, so much that may not take off at all...",
	"bleh, when there's stop no condemned bastard healing potion in there anymore...",
	"eh, I thought the times were past where the ultra charring simply times takes off over 1000 HP in a second. Is the guy's fire resistance really so bad that he dies to that so fast???",
	"blah, actually the werebear should have protected me from the overpowered explosion, also I have maxed fire resistance and the bug enchantment bug is actually fixed...",
	"Grenade launcher boss with extra strong cursed and nose enchantment. What wantsye there already largely make, huh. And I even again ran away as druid, because of course I was transformed back, but then something hit me again and I was dead instantly.",
	"Single hit kill by a sniper boss with extra strong. It does however surprise me that they take off so much...",
	"baeaep, is yes bugged...",
	"Single hit kill, boeoeoeoeoeoeoeoep...",
	"With so many NDEs it was clear that the mage baddies would get me eventually.",
	"Sure that his static field takes off so much.",
	"jesus kicking is broken", /* by Demo; kicking Jesus is really overpowered because he won't fight back */
	"There is a strange bug in nethack. Sometimes it seems when you have the amulet and go upstairs in Gehennom, you instead go a level down again, and this happens randomly so you have to do it over and over and over and over and over and over again. Surely this cannot be intended??? I mean who would think of something that stupid?", /* by stenno */
	"slex is for people who just hate themselves or feel they deserve punishment for something", /* by K2 */
	"At last this character died, so I can finally roll a new one. Thanks RNG!",
	"I got so bored with this character that I was hoping a monster would kill me, but it just didn't want to happen, so I threw my char away on purpose by eating Envy's corpse. Because I'm such a noob who would actually forfeit the chance to score a SLEX ascension for such a petty reason.",

	/* here we have a bunch of hilarious quotes, I don't know of a good place to put them so for now they're gravestone inscriptions --Amy */
	"This is markedly different from the most popular RPG series of all time like Final Fantasy, Diablolo, The Legend of Zelda, where players regularly fight against monsters with resistances or immunities regularly.",
	"Claim that it's obvious that weapons that affect 'most' monsters are as good as weapons that affect 'all' monsters: Claim rejected. I've coached multiple people in playing nethack and they are all confused by this.", /* "I've coached multiple people in playing nethack and they are all confused by the fact that I don't know how to coach people." :P */
	"Further it is not typical of an RPG to have a energy type weapon and have so few monsters resistant to that type of energy. Nethack is an outlier in that. You are wrong.",
	"Simply because people agree to do the article this way doesn't mean that's how it should be done.",
	"I am well aware that Vorpal Blade can behead most monsters, but it's not a good compliment to resistant monsters since many can't be affected by Vorpal Blade's behaading attack. Sorry, you are still wrong.", /* What is "behaading"? */
	"Having meandering sentences jumping from topic to topic as the current entry does, and not putting sentences under topic headings isn't just what wikipedia does, you lean that in your 1st grade English class. It's proper communication.",
	"I'm sure you thought that frost brand was a good weapon. You just didn't know that Frost brand (and other 'most' weapons) were nearly just as good as 'all' weapons.", /* err yeah, can someone please translate this sentence into plain English? */
	"Okay, first off, gauntlets of power are never a mid tier wish, for any player they are a top tier wish for many, many classes.", /* yeah sure, tell that to the lieutenant with a wand of death that kills you because you didn't wish for SDSM or CoMR instead! */
	"BTW I got gauntlets of power with a monk once and it was the absolute nuts. So not sure what you're on about here.", /* Uhh no? Monks don't ever deal meaningful damage in vanilla because when they ported the role from slashem they somehow decided to trash the martial arts skill? */
	"Certainly there are other circumstances where GOP is not a good idea, if you are a monk who's been fortunate enough to get a lot of attack spells and a ring of slow digestion (maybe wish for magicbane?).",
	"DO YOU WANT TO DO THIS BRO? BECAUSE IT'S THUNDERDOME TIME! I'M GOING TO MAKE IT SHORTER THAN YOUR ARTICLE AND CONTAIN MORE INFORMATION. CAN YOU HANDLE THAT? WHAT WILL YOUR EXCUSE BE THEN?", /* SHUT UP MR SHOUTY :P */
	"Okay I failed at the thunderdome challenge. Endless shame is mine. But I've gotten down to 1381 words to the current article's 992. There is no more concise left to be had.",
	"It's absolutely hell on wheels, and I feel there needs to be a sign up around Orcus that says HE LOOKS EASY BUT HE CAN SUMMON FRIENDS THAT WILL MAKE YOU WISH YOU WERE NEVER BORN!", /* dude just git gud at the game bro */
	"First of all the need for consensus is not listed in your rules. I'll say that again in all caps so you don't miss it. THE NEED FOR CONSENSUS IS NOT IN YOUR RULES. One more time. THE NEED FOR CONSENSUS, THAT IS, HAVING ADMINS AGREE TO AN EDIT, IS NOT IN YOUR RULES.", /* The need to ban trolls is also not in the rules. */
	"The Administrators will not permit any substantial edit (one that goes beyond grammar, punctuation etc) without full consensus from the group of editors whether regardless of the truth value contained in the edit.", /* well don't behave like an idiot and don't do stupid edits I guess? would probably help things */

	"well, who knows how moves the variants nowadays is, they are capable of making it so that zapping the bag with cancel will also blow it up",
	"slex is the variant where you ask amy how to cure stuff and what to wish for and wtf just killed you?",
	"argh far myaratmo snarg! Warrh! P = NP!",
	"Sigh, yet another pointless death that could have been avoided if these adventurers just read up on OSHA compliance before entering. They all claim they've been heralded from birth, you'd think they could spare a couple days to gear up properly.",
	"SLEX is what you would get if playing vanilla NetHack while high.", /* supposedly by FIQ */
	"SUCH A SHITTE!",
	"Around to shorten helps only full processor power lay.",
	"upper shitty lay!",
	"ziratha: Finder of stupid way to die #2014", /* trying to quicktravel while sinking in lava */
	"whenever you wish for an artifact you instead get a pink tutu and high heels if you're male, or a flannel shirt if you are female. that's how you make artiwishing gay", /* by Demo */
	"There most probably is no god, but if there is, at best he doesn't care about us humans and at worst he's laughing his ass off at our ineptitude.",
	"playing slex is like going in vacation to hell", /* by Demo */
	"F you are your game and the dragon you rode in on! This $hit SUCKS WHy is there a fire elemental on Dl2 ahfgakjhg akjhajkhga;pkdhgAljkg;lkgjh", /* by amateurhour I think? */
	"I never said that SLEX would be easy, or that it wouldn't have pitfalls for unwary players to stumble into.",
	"'unbalanced', you keep using that word... I don't think it means what you think it means",
	"Bro, the entire game is like crawling inside a lunatic's head. Even the maps look like someone threw up a bag of skittles. Everything about it is traumatizing.", /* by someone on 4chan */
	"This fool thought he could ignore the safety precautions, ended up getting infected with corona virus, and died.",
	"Apparently corona ignores face masks now.",
	"Huh... so it is possible to contract corona after all. Seems like I'm the one unlucky fool who actually managed to get it by meeting other people.",
	"Why make a tournament for such a broken mess of a variant? Better fix all the bugs first and obtain save stability :P",
	"oh whoops, we only added some monsters, didn't think that would break save compatibility... sorry!",
	"I fell and cheated on NetHack!",
	"Oops, I think I cheated! Sorry, my mistake, didn't mean to!",
	"Oh no, here I go cheating again!",
	"no, I totally didn't intentionally hang up when that mumak's second attack would have procced to kill me, my internet connection dropped!",
	"I keep hitting the 'cheat' key on my keyboard. Sorry.",
	"I did something that MIIIIIIIGHT be cheating, but ACKSHUALLY isn't cheating!!!",
	"Can I discover a new bug and then it's not cheating?",
	"Wow, yay, great game design. Very hard difficulty just makes all hostile monsters have 8x as much HP! That's... so incredibly well-designed, I can't believe it!",
	"Telling me to play very easy difficulty when I complain about the impossibility to have pets steal from shops is incredibly arrogant.",
	"Here lies Sid, ascended... and cracked his skull on the ceiling.",
	"Time Capsule: Do not open until T:4283852",
	"Here, we buried the variant that was impossible to ascend despite there being a tournament held that would award money to those who ascend it.",
	"RIP wand of wishing (0:3), killed by the wand of wishing (0:8) and the developers whining that players please don't use more than three charges.",
	"Wait, the elder priest is immune to time stop???",
	"Huh. Didn't think the random backfires from the map level spell could be that bad.",
	"RIP tender yellow-blonde practicant lass, crushed under the weight of Noroela's cruel dictatorship when she ultimately ran out of money with which to pay the fines.",
	"Amy, even a fresh slex compile has this problem!!!",
	"How can this be? My char basically steamrollered hordes of ubermonsters and then that lone monster came along and completely obliterated me.",
	"What? I've been fighting monsters like that one before. How the hell can it be that this particular one hit way harder than the previous ones?",
	"The stupid confusion just would not end, so I was unable to cast any healing spells!",
	"Fuck hallucination, I don't have the slot of my wand of teleportation memorized. And when I tried to zap the monsters with one of my wands, of course I ended up picking the wand of healing instead.",
	"Of course it is only a 1 in 8 chance that confusion makes my spell go in the wrong direction. Of course the RNG still decided that it would do so in this critical situation.",
	"Noooooo! Something caused me to auto-equip an amulet of strangulation and I had no holy water left!",
	"Gah, why did my prayer fail? I had specifically used enlightenment 20 turns ago and it said it was safe to pray!",
	"Whoever put a timer on meteor and frozen orb deserves to have their sexual activity in real life put on a timer.",
	"The lying character screen told me that Bremm's conviction aura reduced my resistances by 5 percent, so I just ignored it... only to find out that each spark was doing 120 or so damage and before I could react, he had already killed me. BULLSHIT!",
	"Baal shot me with his orange missile ten times in a row, and then he teleported on top of me to do it!",
	"Nihlathak, that son of a barbarian whore, was exploding corpse after corpse after corpse all well out of range, but then I suddenly realized that I was standing on a harpy corpse and before I could teleport away, it detonated.",
	"I did dodge the archer boss's arrow. I really did. But it was an extra strong multiple shots might aura enchanted archer boss and the outside arrow clipped me and I DIED. Geeze-o-man.",
	"I walked up to the next teleporter and clicked on it. Why? I had no idea. Anyway, I ended up in the middle of a pack of goat demons, they went chop chop, and a few seconds later I was staring at a red screen.",
	"To my dismay, Duriel completely ignored my hired mercenary meatshield and came directly at me, and instakilled me with his charge.",
	"That stuuuuupid Lister the Tormentor, why the everloving fuck did they design it so that there is loading lag while his boss pack is spawned? When the lag ended, I was already dead!",
	"lol warum? weteeff",
	"NOOOOOOOOO! I SOFTLOCKED THE GAME!!!",
	"Well this part is dangerous, there is a slight chance that the game crashes. Hopefully it won't happen to m-",
	"Oh my god. If you ever encounter a shoe monster in SLEX, run. Don't try to kill it, you'll get your butt kicked.",
	"Gah, did the shopkeeper really have to call that many kops just because I accidentally stole a 10zm item???",
	"WTF, how long does it take for my character to digest a baluchitherium when polymorphed into a purple worm? My polymorphed form got killed halfway through the digestion process but my char didn't stop digesting, meaning I was helpless and got killed before I got to move again!",
	"Oh wait, so having autodig on is actually dangerous if you happen to press > while standing next to lava? I never knew!",
	"Damn you Amy, you made shops way too deadly. I had uncontrolled teleportitis, and when I walked over a potion, my autopickup caused me to pick it up and at the same time I got teleported out! What's that you say? I'm supposed to turn autopickup off? FUCK YOU, I'm not gonna do that! Fix your shit game instead!",
	"Why is it so special to die to a fire enchanted Bartuc? There's lots of other bosses that could also spawn fire enchanted...",
	"Wrong variant, 'udemy certified metasploit framework professional' sounds like a slex monster, not a dnethack one. Learn to tag your post properly next time bro :P",
	"you don't want a cursed luckstone, because you'll likely not be able to pray and your wand of wishing will also not work correctly, but behave more like the virgin on her wedding night: can't find the hole!",
	"they did that on purpose because they want to maximally piss off me (specifically, they don't care about pissing off anyone else, just me), and nope I'm not paranoid, they're really after me",
	"The only reason to play SLEX I can imagine is that your supply of acid ran out.", /* by Mandevil */
	"Oh, you didn't know about slex being the actual cause of COVID.", /* by jonadab */
	"this game is a fever dream", /* by amateurhour */
	"the sokosolver floors being bass ackwards is also a pita", /* ditto */
	"the f in slex stands for fun", /* ditto */
	"no theories involve you surviving a run of slex all the way through",
	"dude this game, it's like a fever dream. That trap gave me a feminism curse which meant any feminine creatures made me fart, and the mummy kept stroking me twice per turn unleashing every possible fart effect so I'm just there farting myself to sleep over and over as they're stroking me to death", /* by amateurhour */
	"Amy a tiny asian girl paralysed me and kicked my shins over and over for ages till I died", /* ditto */
	"Oh no, I only have one energiud remaining!",
	"Here lies Eddard Stark the Cunning. Apparently he wasn't cunning enough to predict that the evil monster Cersei would try to kill him.",
	"Here lies Robb Stark, King of the North. THE Theon took over his kingdom and killed Robb in the process.",
	"Here lies Julyne Stark's virginity, killed by Locke's penis.",
	"Here lies King Joffrey the Mad. Julyne Stark slit his throat and the entire kingdom rejoiced.",
	"Here lies Brienne Stark, who doesn't even exist. Who is that supposed to be? She's not a character from the show. Now Julyne Stark, *she* is a canon character. But Brienne? Never heard of her.",
	"Here lies Catelyn Stark. The evil Lannister assholes wanted her to die even though she hadn't harmed anyone.",
	"This is the Game of Thrones mass grave, because everyone dies in the end anyway so we just shoveled all the corpses into one single tomb.",
	"if u dont have double motivation ur game can suddenly end at any time", /* by amateurhour */
	"Here lies Jaime Lannister's left hand, cut off by Locke. Because Locke is evil with a capital E.",
	"Here lies Bran's ability to move his legs. While Cersei is an evil monster, one of her only good deeds was to push that loser out the window. He broke his legs so badly that he ended up handicapped for the rest of his pathetic life.",
	"What, dipping a stack of potions into a pool doesn't always dilute it, this is unplayable and a hill I'm willing to die on",
	"nasty traps last too long", /* hint: they don't :P --Amy */
	"WHAAAAAT you made it so that I can't simply startscum healers for wishes anymore YOU SUUUUUUCK AMY! I certainly won't touch your shit variant again!",
	"Is there still something in slex that players enjoy? You should implement the opposite of that.", /* by amateurhour (exact original wording might be slightly different) */
	"magic traps lead to tragic maps", /* by amateurhour */
	"maybe slex would be winnable if, instead of ascension, it had other ways to win. like collect/consort with every kind of shoe in the game", /* by spicycat */
	"God, the Minotaur Maze subdungeon is really capable of driving a player insane. #quit",
	"Amy the reward for killing the Minotaur of the Maze sucks!!!!!!! I refuse to play the game again until you make it better!",
	"Yet again the Green Cross subdungeon wasn't open even though I based my strategy around the 1 in 20 chance that it is, so I had to quit again. Great.",
	"Omg how overpowered is that Lord Stahngnir boss at the lowest level of Green Cross???",
	"WHAT? Amy, are you fucking serious right now? I steamrollered Stahngnir, the Steel Giant Lord, and then planned to whoop the extra boss you put there, but your 'Ariane, Lady of the Elements' should be renamed to 'Ariane, Lady of Cheating' because she can't be defeated at all!!!",
	"Okay Amy I'm calling you out now, you bitch. I assembled five ascension kits with 20 artifact weapons and had to use all of that to kill your 'Ariane' boss at the bottom of Green Cross and all I got was two random artifacts and the ability to generate weak elemental artifacts, all of which are a lot weaker than what I actually used to defeat her???",
	"What, rival quest in Gehennom? Sounds easy, there sure won't be anything dangeroWAIT WHAT WHOSE QUEST NEMESIS IS THAT AND WHY DID HE JUST ONESHOT ME???",
	"Blah, the devnull challenge areas in this game suck, most of the time they don't exist at all and even if they do, you can't actually play pool or digdug! Amy I'll play the actual devnull instead! Screw you!",
	"Wut? I kept running into walls in the Emyn Luin branch and somehow lost all of my intelligence and then a mind flayer came and instakilled me!",
	"It's really unfair that the boss of Emyn Luin can wipe you out so quickly with that stupid assault rifle. None of the other enemies in there could even dent me.",
	"Hell's Bathroom is a stuuuuupid, unfairly designed dungeon branch.",
	"Amy I told you that the Witch King of Angmar is OP! Just to prove my case, he just offed my character that was otherwise ready for ascension!",
	"Yeah you can't defeat Tiksrvzllat. Don't know why I even bothered trying.",
	"WHAT??? Script-generated monsters shouldn't exist! They add nothing to the game! Fuck you Amy for having them in the first place!",
	"I swear, script-generated monsters were a very bad addition to the game as their balance is completely out of whack. Amy should remove them.", /* but she never will :-P */
	"SLEX is not a game but a C compiler testsuite", /* by bhaak */
	"no mouth, one leg, you can use #monster to roll around on the ground, you start with an inka hat and randomized skills and intrinsics", /* by spicycat (original wording was slightly different) */
	"hey amy ur game is LAMY", /* by Demo */
	"Amy you suuuuuuck! why did you make that fucking town with the blasted HORDE of angels that is plain IMPOSSIBLE to defeat you BIIIIIIITCH! omg do you even play your game? apparently not, because otherwise you'd notice how it's more of a mental asylum simulator than a video game!!!",
	"slex is for slosers", /* by Demo */
	"slashtehmeretaher <--- play that instead", /* ditto */
	"tzt der sycrikk olkf symbiosis den skill freilschascltne+",
	"SLEX is an egregore from the EPIs of every player", /* by luxidream */
	"Here lies some hobbyless minecraft player, slain by the ender dragon because in hobbylessminecraft you always get killed by the ender dragon.",
	"Here lies the ender dragon, slain by a bunch of exploding beds.",
	"Arrested by the Keystone Kops. Permanently.",
	"Come on!  What kind of watchmen arrest someone by killing them?",
	"For years and years I wandered the earth, until I died and went to hell.",
	"Here lies Hilda, calm and placid.  Added water to the acid.",
	"Here lies Mittens the housecat, used up his wand of undead turning (0:8).",
	"Here lies Owen Cash, tried and failed to dine and dash.",
	"Here lies the zruty, eaten by a sea monster. May it zzz in peace.",
	"Here lies the zruty, killed by a variant developer that dislikes variety.", /* :-P --Amy */
	"I thought I could be like Adeon.  Guess not.",
	"I was wanted dead or alive.  Turned out it was dead.",
	"Little Timmy drank a potion. Now he is no more. Cause what he thought was H2O was H2SO4.",
	"Oh, I died.  Well, better late than never!",
	"Ten thousand start scummers lie at rest here, their lives tragically cut short.",
	"Here lies this lame start scummer. We didn't even bother adding his, her or its stupid name because startscummers suck lol",
	"Here lies Desideratus Annius, a perverted coward.",
	"Here lies the captain of the town watch. He takes a lot of the blame for this fortress's downfall.",
	"At last that old fool died, was about time.",
	"Hey visitor! Feel free to dance on this grave, for I burrowed my insufferable mother-in-law here! Ding-dong, the witch is dead!",
	"Terminated.",
	"Don't you know it's rude to walk on someone else's grave?",
	"How could the elves do this to me?",
	"Here lies Jade, her debt to a shopkeeper finally paid.",
	"Ashes to ashes, dust to dust.",
	"This guy didn't pay enough to have his name on the headstone.",
	"Here lies Brock, killed by a falling rock.",
	"Huh, what's that you say about walking around with unpaid items while having uncontrolled teleportitis? What's the worst that could happen?",
	"WAT? Contamination instakill? I don't even know why that killed me, or how I could have prevented that!",
	"Tried to off that demon lord, but he breathed at me and I died.",
	"Oh, I got hit with inertia. No big deal, that dragon can't possible kill me in one rou-",
	"Hey I know what I'll do! I just flee from that ancient dragon by running over the snow tiles! Dragons are slow after all, they can't cat-",
	"Aww how cute, a tai chi girl! Why did the variant developer even bother adding such a wimpy monster to the game... hey how come she just paralyzed me? Err what, she kicks me and kicks me and I don't regain control of my char??? I can't possibly die to h-",
	"Amy that hippie heels kick attack is busted, the level 10 zombie prostitute just soloed my XL20 monk even though I had a full ascension kit.",
	"Yeah of course this game is totally fair. You make your way through that endless Gehennom with your ascension kit and then suddenly there's this demon lord that just disintegrates all those items one after the other. UNBALANCED!!!", /* although of course it's not unbalanced, just a difficulty spike :-P */
	"Whaaaaaaaat? Wouwou beggars are level 0??? Omg does the variant developer even know how OP the wouwou taunts are?",
	"A level 2 monster with an active banishment attack??? BULLSHIT! No sane programmer would make such an unbalanced game!", /* once again, they are balanced, after all they're very slow */
	"Yeah sure, a death lichen. Give a level 0 monster a touch of death attack and you'll have a garbage game where you can just die randomly through no fault of your own.", /* and of course the random poison dart traps in vanilla aren't unbalanced :-P */
	"Tried to get to Oregon. Died of dysentery.",
	"every joke suggestion becomes another fartifact that spawns on dlvl 1 and prevents you getting your first sac gift", /* by amateurhour */
	"man you cannot get the first sacrifice gift and without it this role kind of sucks! #quit",
	"amy i offered 250 corpses and had three altars disappear and got NO gift!!! #quit",
	"man i sacfested for three hours straight and when i finally got a gift it was one that autocurses and gives some bullshit interface screw nastytrap garbage!!! #quit",
	"The connection died and then I tried to reconnect and my game was gone.",
	"suffocated in a vat of crappy shoes", /* by cebolla */
	"WHAAAAAAT? I'm supposed to use tactical combat where my positioning, choice of melee versus ranged weapons etc. MATTER? screw that, I'll just whack everything with my orcish dagger in melee :P",
	"nurse dancing can result in the sexy nurses stepping on the player's toes with their sexy high-heeled nurse boots <3",
	"So, like, atrocious segfaults happen every minute!",
	"balanced means you don't fall off a tightrope. if you put a computer running slex on a tightrope it would fall off, therefore slex is not balanced", /* by qt */
	"nethack worse game ever, never playing again", /* by aoei */
	"nethack worse game ever, never playing again AND NO ONYO",
	"Here lies democracy, slain by power-hungry politicians.",
	"Got attacked by a plof anje and the game said that I 'come because her sandals are so beautiful'. This game is unplayable, no one can handle such wacky messages.",
	"Amy the castle chest exploded, slex worst variant ever.", /* by amateurhour */
	"oh sec has wand of banishment! selfzap selfzap selfzap oh empty? well geronimo", /* originally the comment here read: 'amateurhour still has until July 1, 2021 to figure out how to scum them more effectively, then I'll reveal the (IMHO rather obvious, but it requires familiarizing oneself with the role :P) secret --Amy' - he didn't, so there's some follow-up messages now as well :D */
	"Here lies a secret advice member who suicided after his wand of banishment ran out because he didnt familiarize himself with his abilities and therefore failed to realize that he could recharge the wand.",
	"Whaaaaaaat? I'm supposed to familiarize myself with the new role I play by looking at my inventory and techniques first? SCREW YOU! I'll just whack everything in melee, using convoluted strategies like recharging an empty wand with a technique is for plebs!",
	"well the goal of slex is obviously to hurt players, so mission accomplished", /* by Earth Traveller */
	"omg there are roles that cannot simply melee everything with their starting weapon? suuuuuucks! not gonna play that role again :P",
	"SLEX is not a game, but a piece of postmodernist art designed to see how much suffering someone will endure in pursuit of petty goals.", /* by Earth Traveller, original wording may be slightly different */
	"Can I get a count of everyone who plays slex that enjoys playing it? 1", /* and that person is Amy :-P */
	"every time i get optimism that slex is beatable some crazy stuff happens", /* by amateurhour */
	"How do I reliably prevent triggering nasty traps? WHAT??? You can't? FUCK YOU! This game is unplayable!",
	"Hey, how can I fix nasty trap effects? What, you say I need a single-use item that is so rare it basically requires a wish? That's absurd, I'm gonna file this stupid game in the trash bin.",
	"SLEX is just too damn long! If the mandatory part of the dungeon was maybe 70 levels it would be okay, but the way it is, you're guaranteed to eventually encounter some bullshit that kills you.",
	"Whaaaaaaaat? I thought nastytrap duration was capped at 2000 turns! No one told me that they could randomly last for over 10k! That is some prime bullshit right there if you ask me!",
	"omg I triggered that feminism trap and it just wouldn't time out??? Amy urgently needs to rebalance that shit because the effect is crippling and lasts way too long!",
	"omg Amy you overnerfed the monk role!!!",
	"yeah you can't play the monk anymore if they don't deal a consistent 50-60 damage per round in melee. not gonna play again",
	"that spirit race nerf is bullshit! they weren't OP, just good!", /* but in reality they were very OP because phasing is uber powerful --Amy */
	"Amy you always nerf exactly the thing that players use! Why not nerf something else instead for a change?", /* because the roles and races that no one plays remain untested until someone does play them :-P I'm just one woman, I can't very well playtest everything by myself?! No one has that much free time! */
	"Well this is bullshit, the strategy I was using certainly wasn't OP and in no way it made the game too easy. That Amy nerfed it anyway is absolutely retarded and no one should play her fucked up game anymore.",
	"works as intended",
	"slex is a very fair and balanced game",
	"Amy is insane, every time I complain about bullshit she just mocks me, tells me it's working as intended and then proceeds to add more stupid garbage to the game! SLEX fucking sucks!",
	"Hey, I tried to wish for a figurine of an archon but the game said it doesn't exist! What's that you say, I can't wish for figurines of specific monsters? FUCK YOU AMY, your game is so brain-dead that I won't ever play it again!",
	"Urgh I cancelled myself and lost an intrinsic resistance but the cursed -5 luckstone wasn't uncursed and only became -4! That just sucks so very much, I don't even want to know what the developer was smoking cause it sure ain't legal.",
	"Amy must have been on crack when she decided to make the restore ability spell not guaranteed to work. Like, who would design a game that badly?",
	"How the hell could Amy screw up so bad? Man, her changed artiwishing rules suck ass! There are so many artifacts generated and they all suck but at the same time blow your artiwishing chances to heck!",
	"amy is the epitome of mental health, as we all know",
	"Guh why won't that dumbie lightsaber get better regardless of how much I bash with it while it's unlit? That trains the Hunk lightsaber form after all, which is supposed to increase bashing damage!",
	"This stupid lightsaber, it doesn't want me to win! I tried everything I could think of to train the Kliu form, but it just wouldn't gain any skill!",
	"Shitty lightsaber, I tried so hard to make the Aldebaran form work but the RNG wasn't on my side!",
	"Shitty lightsaber ey! I used the Polgo form all the time, or so it thought, but somehow it didn't work and I saw that it had 0 units of skill training on my dumplog!",
	"This dumbie lightsaber, even when it was finally turned on I couldn't use the Firga form! What gives?",
	"Gah why did I think the Hunksterman artifact was good? The speed bugs caused monsters to outspeed me by a factor three and they ended up killing me with no chance!",
	"Man, 'Lu Nonname' is such a bad artifact. Don't ever try to use that, the supposed bonuses to the Kliu form are never worth it and the downsides will cause you to die.",
	"Fuck the inventor of the Aldebaran lightsaber form! And fuck whoever made the Alpha Tauri artifact armor, I still wasn't able to actually train or use the Aldebaran skill but the game constantly spawned nemains and wenkamuis and metal vesda fire machines and mikraanesises and then I died!",
	"Gaaaaah, I thought it was a good idea to use the Plantoplim artifact armor and what happened? I had an implant with levitation forced upon me and starved because I couldn't get at the food on the ground anymore!",
	"Yeah Elmhere just ain't a good artifact, the Firga lightsaber form is completely useless and you'll just get killed by random garbage.",
	"Hey! I thought SLEX was supposed to be a fun game, not super bullshit simulator!",
	"Man, SLEX is no fun. There is so little strategy headroom, you get very few useful items and then have to be super careful or you end up losing them again to some bullshit! And if you do lose them, you never find another!",
	"What, it's a 'massed affecting calendared', whatever that is! I'll throw some darts at it, hmm, that doesn't really seem to be doing so much... damage, why the hell am I suddenly paralyzed? Huh, does that thing have ranged thorns or what?",
	"Wait, so there *actually* is a lag trap that causes fake lag? And it lasts for thousands of turns and cannot be easily cured? #quit",
	"Configuration file? Who needs that anyway? I'll just play with the default settings. By the way, what does it mean when the bottom status line says 'Stressed' after I just killed some elves? Oh well, certainly not important, I'll just continue killing everything in my pa-",
	"The s-course had 12 minutes delay.",
	"Here lies SLEX, killed by internet cancel culture.",
	"SLEX, 2013-2021. Some person found a tiny little game feature, decided to interpret it as if it was something REEEEEEEEEEEEEALLY BAD, and told that to the entire community who then proceeded to cancel the game and its developer.",
	"I cannot imagine the mental disorder that would make a person *want* to wear shoes that are so terrible, practice is required just to walk in them.",
	"She got stuck with her stiletto heel in a tiny little crevice so badly that she fell against a massive stone statue, hitting her head and dying instantly.",
	"You fucking bumped into a fucking peaceful monster with a fucking Stormbringer for fuck's sake!",
	"Internet cancel culture is just terrible. All it takes is one person in a position of power who declares your game dead, and all of their followers will mindlessly follow suit and treat your game as if it never existed, even those who used to play and enjoy it. That is the fate that befell slex.",
	"Balance? What's that? No one needs that. Let's just throw everything into the game that we can think of, and call it a day.",
	"Since I can't see the information invisible, I feel my future is uncertain. Read 2 healing scrolls, a phase door scroll, and fuck him hard.",
	"To be honest, I was really gambled on my life, but good luck was on my side and killed it with a silver dagger, an artifact of the Immortal Nemesis.",
	"What? Died because I 'ran out of alla'? What is that bullshit death that came out of nowhere?!",
	"Stupid monster socks with stupid paralysis attacks.",
	"WTF, why on earth does Sing (who is that even supposed to be) suddenly ask that I clean a pair of female shoes with dog shit on the soles and why do I get paralyzed to death if I rightfully refuse???",
	"Got very very annoyed at the casino croupier and decided to take my money back by force. Only, it turned out that he was too strong, the bastard!",
	"Monster pants are a stupid mechanic and Amy should feel bad about adding them.",
	"WTF, killing a monster that is on the phone gives a huge alignment and luck penalty for no obvious reason??? #quit",
	"I tried to be stealthy. I really did. Gave a wide berth to that zoo and avoided every action that could possibly wake up monsters. But there was some weird snoring sound and suddenly they were all awake and killed me. Bullshit.",
	"The annoying VF kept photographing and blinding me while the other monsters beat the crap out of me!",
	"What on earth are you supposed to do if a permamimic corners you and you have no ranged weapons? I bumped into it 200 times and it still didn't die; instead, I did!",
	"Well the monsters that keep mimicking things even when you attack them can just fuck off, the cunts. Doubly so because you can't melee them. Bullshit mechanic that should be abolished.",
	"The main reason is that piles of useless things are useless, and a little bit of money is a little bit.",
	"oh lol you thought those blank tiles in vlad's cellar were just unexplored areas? mua ha ha ha ha, they're really bottomless pits, and now your char that you've put 100 hours into is gone. lololololol I'm so great, next time I'll just delete your save 100 hours into a run with the same effect, ain't I a genius?",
	"omg the 'unconscious girl minigame' is too much i wont play this game anymore",
	"omg the 'unconscious girl minigame' is too much i wont play this game anymore even though the chance of actually encountering that minigame is about as big as winning the jackpot in the lottery",
	"Rounded a corner and the level 6 catprincess just instagibbed my level 22 char, very balanced game. I had over a hundred hit points and two dozen points in Evasion and Greater Evasion.", /* happened to Amy in Elona */
	"You could be screwed over at any point in the game! It's balanced!", /* by amateurhour */
	"Here lies a former SLEX player who decided to dump the game because of the Antje minigame.",
	"Here lies a former SLEX player who decided to dump the game because of the Antje minigame even though the minigame occurs extremely rarely and when it does, it's possible to simply spacebar-spam past it.",
	"Here lies a former SLEX player who decided to dump the game because of the Antje minigame, even though most of the time you won't even reach the point in the minigame where the girl becomes unconscious and even if you do, you're not obligated to pick the choice where you can r-word her.",
	"Here lies the stupid tournament whose name starts with the letter J. It was killed by a server administrator who somehow has so much power that he can fucking FORCE the tournament admin to take games OFF THE TOURNAMENT just because the server admin personally dislikes them.",
	"Eyehack makes it so that you can encounter full-grown silver dragons or elvenking statue traps as early as dungeon level 3. And they call *slex* unbalanced.",
	"Told you that if there was a genuine minotaur in that maze on dungeon level 5, fighting was not going to be winnable.",
	"If you have rock trolls spawning from the very beginning of the game, I do have to wonder what's going on there.", /* used to happen in slex */
	"The madling somehow survived my level 150 magic dart spell and then got ten turns in a row which it used for spamming only chaos ball and nothing else, so I died. Great.",
	"The madling survived my water bolt spell because it was too low level, and when I tried to cast again I failed to cast the spell correctly, so the madling used its turns for spamming only chaos ball and nothing else, and of course I died. Blaaaaaah.",
	"Bah, SLEX is so stupid, why on earth can you get a level with unbeatable activists and other level 10 stuff as early as dungeon level 2 with a XL1 character? That's so unbalanced I don't even!",
	"Man, that Larus bastard is way too strong, you can easily get past the hanging semen ejaculators with a decently skilled climacterial but the nemesis is unbeatable, he has too much AC and resists most elements and even if you somehow manage to damage him, he just endlessly spams a healing spell, and then he kills you!",
	"Here lies the grandmaster spirit monk who ascended SLEX, he decided to retire because he doesn't want to ruin his win streak of 1.",
	"Paralysis is turboass!",
	"Yeah you can't play this game unless you have free action and discount action, because without them you just eventually get paralyzed and killed with no chance.",
	"Ding-dong, the witch is dead! Took her long enough to finally kick the bucket. Annoyingly we can't simply undo all the bad things she did while she was still alive.",
	"Whaaaaat? I used an uncursed scroll of charging on a 0:2 magic marker and it didn't become a 1:50 marker but turned into a 1:8 one??? That's prime bullshit, why on earth would anyone change that???????",
	"Amy what the hell? Every variant in existence keeps the vanilla behavior where the quality of charging doesn't matter at all for a magic marker, so of course I used an uncursed scroll in slex too! What is that shit, now I have a recharged marker that cannot write anything! #quit",
	"Err is there a way to land? I keep getting damaged by the stalactites - oh whoops I'm dead. Bullshit!",
	"Uhh I had a ring of sustain ability, that should have saved me from the contamination instadeath when I only had 3 wisdom left...",
	"Uhh I had lost some wisdom due to contamination but I had a blessed +7 unicorn horn! I applied it two hundred times and it still didn't give me the lost stats back, that's gotta be a bug!!!!!! Instead, the unihorn just disenchanted itself and then turned to dust, this game should really get fixed by the developer!!!",
	"uhh blessed remove curse doesn't work, there are still many cursed items in my inventory, what gives? wait, you say I have to equip them first??? blessed remove curse is supposed to uncurse everything!!!!!",
	"bullshit, why do I have to pick up the loadstone in order to identify it? amy is supposed to add a quality of life feature where you can name stuff off the ground!",
	"how many hoops did amy put into the stupid game, the regular quest certainly isnt too easy so why on earth do i have to endure a fucking subquest thats literally impossible to complete",
	"Why on earth did the blessed potion of random intrinsic still give me a bad intrinsic??? That's something that should not happen, I want it to be so that I can just bless all of them and accumulate every good intrinsic in the book while avoiding all the bad ones!",
	"wait, gain level is nerfed because it was supposedly too easy to reach XL30? fuck you amy",
	"the game certainly isnt too easy and it definitely doesnt need more nerfs, it needs some player-friendly stuff",
	"wtf, stethoscope breaks, god i hate you",
	"uhh i need something that gives me a full inventory id that works on everything and doesnt fail, otherwise the game is too hard",
	"waaaaaaat I cannot branchport out of the Void? just what the hell is that shit",
	"eh you do know that half physical damage is supposed to halve the physical damage I take? that attack just did full damage to me still and I died!",
	"Wait, I thought that polymorphing into a different race gave you all the powers of that race! I polymorphed into an octopode yet didn't always get the effects of eating a ring, what gives?",
	"yaaaaaaawn the yendorian tower is sooooo boooooring, I can simply pwn all the monsters in there so the search for the stairs is just eating up my time, I'm quitting! From now on, once I acquire the amulet in SLEX I consider my character to have ascended, can't be arsed to do that long boring slog!",
	"WHAT? Amy made it so that I can't just put all the contents of a shop into a container and then let my pet steal that container??? DUDE that's how you steal stuff in vanilla, it should work in every other variant too! I'm gonna dump SLEX, worst variant ever!",
	"Bullshit, the prompt asked me what monster I wanted to genocide so of course I said 'bimbim lich' and instead of genociding them, six of them appeared around me. That's gotta be a bug, the prompt should have said 'what kind of monster do you want to send in'.",
	"The gods aren't interested in refrigerated corpses now??? THAT WAS MY WHOLE STRATEGY",
	"What, I had a wand of death with plenty of charges specifically saved up so that I could kill my nemesis with it! Something's gotta be funky there, I emptied the entire wand on the nemesis and he still didn't die. Hopefully Amy can look into that and fix it.",
	"The stone of magic resistance is soooooo stupid, why isn't it just so that it curses itself on turns divisible by 2000. I'd always quickly put it away when such a turn number draws near, and be able to actually use the stone, but as it is, it's unworkable.",
	"what, why on earth did my luck time out, I always had a luckstone in my inventory on turns divisible by 600. That should have prevented it from decreasing.",
	"hey! I kept applying standard vanilla strategy to SLEX, because I expected it to work! That it didn't is some serious bullshit, won't play the game again!",
	"What? Why can't I banish myself closer to the ending once I have the required macguffins?",
	"Accidentally picked a race that cannot wallwalk, #quit. The game is too hard if you have to actually navigate dungeon levels the intended way.",
	"WHAT??? The fake artifact ALSO cannot be dropped or put into containers and weighs 500? Lay off the drugs Amy, you crackhead whore!",
	"urrrrrgh I had prepared the master SLEX strategy that depended on an oversight but for the new version that evil Amy witch FIXED IT!!! ARRRRRRRGH the cunt, now my infallible plan has been foiled! She's responsible for the fact that I cannot ascend this char now!",
	"nasty traps need to be easy to cure, otherwise players actually have to struggle with their effects and they just make the game unplayable, amy please add a guaranteed source of switchers",
	"amy you must make it so that nasty traps time out after 2000 turns tops, because waiting them out has to be the standard strategy if you dont have a switcher but with their currently busted duration thats unworkable",
	"At this point, I think cannibalism should be punished with an alignment penalty and not something as severe as aggravate monster...",
	"fuck you amy, I put on the iron skull cap of map amnesia for one turn and immediately took it off again and still got amnesia. diaf amy",
	"oilskin gloves should count as a major trouble, wench",
	"nooooooo! the gloves are oilskin! well, I guess I'll just pray to my god for help... err what, they still won't come off???",
	"gah Amy I hate you for making it impossible to do credit cloning in your shit variant",
	"well the fact that unihorns vaporize when used repeatedly is such nonsense, the creator of that feature shall suffer",
	"altars disappear after you BUC test a bunch of things??? bullshit, how on earth am I supposed to figure out what is safe to equip now!",
	"that interface screw trap is so shitty, screw you amy, you need to be flogged 24 hours a day for the next aeon",
	"even when you block them, nastystones thrown at you by monsters land in your pack! that surely isn't right!",
	"while blind, even if you prefix your movement with M, you still anger shit! had to kill a coaligned priest for that stupid crap, i knew he was in the temple, i was blind and used M, but noooo, he gets mad",
	"I got some farting effect for about 10k turns, seems like a bug",
	"oh... I bet the cooling box is not waterproof... I had all my scrolls and spellbooks in there... :-(",
	"even with full teleport control, sometimes it doesn't control!",
	"lol I had double elec resistance and still got damaged by an energy vortex",
	"wtf the town guards on dungeon level 98 pack a fucking punch",
	"what's overly annoying about void and nether is that the map amnesia causes the I from monsters that hit you while blind or invisible to just vanish immediately",
	"whaaaaaaat, the scrolls still give the confused effect when reading them in the illusory castle even if I have confusion resist? god fucking dammit, I was using that genocide scroll because I wanted to get rid of the thing that was killing me!",
	"Amy, you disappoint me, you could have at least made the artifact drops good artifacts after all the trouble of getting to the void boss",
	"Amy, intrinsics from offhand wielded weapon don't work!",
	"hey cool, my incantifier in dnethack had sex with a foocubus and got drained to 0 Pw, and due to Pw doubling as nutrition for that role I died instantly, cool variant bro",
	"I casted the Remove Curse spell and it didn't remove the curses on my armor. I checked the code and if I'm reading it right, it should uncurse everything equipped. Am I missing something?",
	"amy i found a bug in the game",
	"amy i found a bug in the game, it seems that if there is something friendly that can pass through walls, you can displace it and walk into the wall, maybe i should quit because the game is obviously in disorder",
	"The game said 'Program in disorder, maybe you should better #quit', so I did and it deleted my character! What the hell???",
	"Is this a bug? I have 3 diamonds. I took one out of the bag and sold it to a jeweler. My faithful dog was kind enough to return the diamond to me, and I put it back in the bag. It stacked with the other 2 diamonds in the bag. Now, I can no longer sell any of the three diamonds because of that item flag.",
	"What the hell is that, I was fighting that lieutenant in melee range! He's supposed to use his dinky knife against me, not shoot 10 bullets from his heavy machine gun and instantly kill me! Monsters should use ranged weapons only when they're too far from you to use melee!",
	"WUT the boulders on that sokoban level didn't start out pre-mapped!!!!!!! how on earth would I have known where they are, of course I got some of them stuck now and can no longer finish the level! BUG!",
	"ugh symbiosis is so bad, you have to carry the monster's weight and it burdens you! bullshit!",
	"wtf, I let that mugger wail on me while he was standing on a piece of armor in a shop, and he never picked it up!",
	"damn the potion of cure insanity is too rare, I died because my sanity was too high! Amy needs to make that potion spawn at least as frequently as healing potions!",
	"so dog picked up helm, I put it on, immediate hallucination effect and it says it's cursed and cannot be taken off, that certainly is a bug",
	"Amy, I want you to know that the quest with the paralyzing toilets things sucks. Especially when the entire bottom floor is one of the weird rooms with all the traps :(",
	"Amy my devil poison mages keep dying on the quest! Why do you keep telling me to play some other character, I want to beat the quest with this combo, please make the nemesis easier!",
	"I picked up a stack of loadstones... immediately overloaded and incapable of doing anything",
	"kinda imba considering how common astral elementals are in the main dungeon",
	"cannot find the stair because hitting > doesnt make quicktravel go to them if they are obscured and cant be arsed to look for them manually, #quit",
	"the alignment quests are like maybe in 1000 turns I'll have told u i am here, but only like 66% chance of that happening by turn 1000, and who wants to hang out on a random floor for thousands of turns?",
	"Amy, stupid astral vision effect where even a sleeping monster keeps interrupting my digging! fix it! please fix it, please!",
	"Amy I had the sokosolver hybrid race forced onto my character how do I get rid of it, do not want to play with that ",
	"ONE percent chance of blocking I had only??? What?! But I was using a bull shield! That normally has 40%% chance of blocking! And I only had ONE percent? That cannot be, even if the shield was cursed.",
	"Of course, now I'll lose to this asian girl because my char cannot get anything done.",
	"> it's a bad idea to expect stuff to work - fixed that for you", /* by amateurhour */
	"yendorian tower worst part of the game, #quit", /* ditto, although probably not a literal quote */
	"What, Amy nerfed the sleep spell? It only lasts two or three turns now??? That's, like, completely unplayable, the spell used to fucking work! #ragequit",
	"Amy nerfed magic lamps. This is always the way it is with slex; I come back to play willing to overlook all the flaws for the really neat features it has that I love, only to find more annoying changes that limit the players ability to move forward.", /* those aren't flaws though, if a player is too bad to put in the effort to play the game properly, they don't deserve to win! whiner! */
	"Whaaaaaat, Amy changed the interaction between the sokosolver, specialist and mazewalker hybrid races? booooo! hisssss!",
	"Amy, the fairy would be much more interesting if robes weighed less than 15!",
	"Amy, all the roles play exactly the same! They all get some ranged weapons with the skills capped at expert, so everyone can use the same kiting strategy on everything! Boring! I'll play a game with more differentiated characters!",
	"That crystal ice golem was probably played by Chuck Norris.",
	"couldn't find the last portal to the last dumb devnull challenge and dont want to go on playing without having completed them #quit",
	"how effing fast does being in the gamma caves contaminate me???",
	"it's insane how many demons are being spawned in the mainframe and the super demons keep gating in more super demons that all spawn with scrolls of demonology and then i died",
	"bah, level 13 sucks, amy should remove that from the game, no one wants to dig a 79x21 area looking for grunds. #(rage)quit",
	"not the bees!!! why on earth did amy have to make that wide open level with the beehives and the weird center line that paralyzes you if you step near it",
	"huh, the shopkeeper on level 31 was invisible. bullshit",
	"Ran around on level 55 and used a throne, but a couple turns later it felt as if the game was malfunctioning! I decided to #quit, hopefully the developer can look into that issue and fix the weird stuff that was going on while I was on that level.",
	"Yeah you can't beat level 70. There's tons of perfume spreaders and wouwou taunters and supermans and whatnot behind the grave walls and to top it off, spellcasters who summon new crap constantly. No one can beat that badly designed level, ever.",
	"hey why did all the traps become invisible after I was on level 71?",
	"what the hell is a 'segmentary query' supposed to be and why did it spawn on level 71 and kick my butt so hard?",
	"Amy the special room on level 88 is yet another fucking bigroom that causes you to be surrounded by uber-powerful monsters from all sides and then you get killed with no chance! Fuck your stupid game!",
	"whaaaaaat, eating Greed caused me to die instantly??? Why was there no warning? How on earth should I have known about that in advance, that's it, I'm quitting this shit game!",
	"Bah, the Witch-King of Angmar killed me so quickly I didn't even know what hit me! He cut right through my stalwart defenses, too!",
	"The rival quest had a neutral artifact. #quit (yeah I'm only playing chaotic because in this game everyone and their mother only plays chaotic)",
	"Why oh why does carrying some other role's quest artifact cause turboassitis, the game would be much more fun if that wasn't the case",
	"Man, I always die in Sheol. And in this case I died to a vanilla monster too, to add insult to injury...",
	"I attacked that slimebeth 200 times in melee and never managed to hit it and then its passive acid attack killed me! Fix plz!!!",
	"that monster was invisible!!! what, I'm supposed to set my terminal such that it can display black-colored monsters? That's bullshit, any good game would have sensible default settings where that color is displayed properly by default!",
	"Didn't want to believe the 'Smoking kills' inscription on the cigarette packs.",
	"add to that that there's not many unique features for roles and whenever a role has a unique feature that's vaguely good it gets nerfed :P", /* by amateurhour, of course */
	"huh, I played the camperstriker like a samurai but it didn't work out! I thought all the roles were identical, so that strategy should work with every role!",
	"Tried to play a kurwa. It didn't feel at all like playing a valkyrie. It felt as if the game was malfunctioning.", /* based on a famous quote by Mandevil (<3<3<3), which is why it says "valkyrie" instead of "samurai" here */
	"what, I thought the secret advice member was totally stacked. They surely seemed weaker than a samurai to me.",
	"waaaaaat a nymph stole my secret advice member's starting wand and used it on me! that seems very broken, Amy should fix that!",
	"Amy made the game not fun to play any more by intentionally nerfing exactly the thing I was using, so now I don't want to play no more.",
	"Amy always nerfs exactly the roles and races that people are playing, allegedly because they're too strong, but in reality they were well-balanced and she trashed them!",
	"So I came back from a 2-year SLEX hiatus, assuming that basic game mechanics were still working like they did back then, but nooooo, that evil Amy whore trashed basically everything, resulting in the player being unable to do anything fun. SLEX is no fun anymore and I remember why I quit in the first place.",
	"Okay so this was me giving Amy's stupid game one more chance, she had a year to improve her game since my last ragequit. But I came back and realized after 10 minutes of playing that she has actually made things worse, so I'm ragequitting again, for good this time.",
	"Died on SLEX's hardest level: the character creation screen. Just couldn't decide on a role to play!",
	"Died on SLEX's hardest level: the character creation screen. Had even managed to decide on a role even though there are too many of them, but it's impossible to also select a race since there's even more of those!!!",
	"Died on SLEX's hardest level: the character creation screen. I'm not aware of the possibility to just let the game pick a random character, which would have allowed me to actually play the game.",
	"Died on SLEX's hardest level: the character creation screen. Had skipped past the page with the one role I actually wanted to play, and I'm a total newbie who doesn't even know that the < key moves you back to the previous page of a menu.",
	"Died on SLEX's hardest level: the character creation screen. Couldn't figure out how to advance to the next page, even though it really shouldn't be so hard to try out different keys and thereby eventually hit the space bar that does the trick.",
	"Died on SLEX's hardest level: the character creation screen. That hybrid race selection menu was just too annoying, why on earth would anyone want to play a game with such cumbersome menus.",
	"Power outage ate my save.",
	"Got disconnected and when I tried to reconnect, my savegame was somehow gone.",
	"My computer crashed while I was in the middle of the game, which resulted in the game behaving as if it had crashed, and somehow it failed to recover.",
	"BULLSHIT! I had polymorph control! Yet I still randomly polymorphed into a silver dragon without getting the chance to choose, and lost my cloak of magic resistance! #ragequit #slexsucks #amysuckstoo",
	"Played a shoe failed existence and got bored because it felt just like playing a human samurai. I'm quitting this game since all the characters are identical so once you've played through the game with one, there's no more variety.",
	"Played a shoe failed existence and died with no chance on dungeon level 2. Why did Amy even put that character choice there if she didn't plan to make them playable.",
	"Played a random character, got jelly courier and died horribly, it didn't feel like there was anything I could have done to survive. Amy should really offer a 'random good character' choice that always gives you something playable.",
	"Amy please make it so that all good playable roles and races are displayed in a bright white while weak ones appear in a darker gray color! I'm sick of picking characters that turn out to suck!", /* this was actually requested once, and will always be rejected because what constitutes "good" or "bad" roles/races is subjective and will be different depending on who you ask */
	"The natalje trap makes the game unplayable, Amy should just remove that from the game or at least make it so that it never lasts longer than 200 turns. It's just plain evil if you can never remain in one place for more than a few turns because your items get trashed if you do, can't play the game with that effect.",
	"Yeah what the hell was Amy thinking when she designed the nadja trap. Getting damaged whenever you melee a female monster, and the damage even increases every time you do, so eventually you just die instantly from hitting such a monster, and the trap lasts for how many thousand turns???",
	"This must have been the situation where my standard strategy doesn't work.",
	"Oh man, you truly are screwed if you accidentally equip a cursed weapon early in the game, because you never find a remove curse scroll when you need one.",
	"Stupid game just wouldn't give me a usable weapon! Having to still plink away with bare hands when you're already at dungeon level 9 obviously doesn't work out...",
	"Maaaaaaaan... I was ONE experience point short of leveling up, and then I'd have been unstoppable...",
	"What the hell kind of bastard monsters are spawning on dungeon level ONE for fuck's sake??? That's like totally unfair, no beginning character can survive that!",
	"Bah, who made it so that monsters can spawn with a wand of draining on dungeon level 1 and instantly kill you before you've even encountered enough monsters to level up once.",
	"What, my prayer timeout can't possibly have been so high. My last prayer was over 10k turns ago.",
	"Weapons suck, they always get stolen by nymphs! And in this particular case there was also a feral lion nearby which I then couldn't handle anymore without my weapon!",
	"Oh damn, should probably have put my full plate mail back on after that foocubus encounter.",
	"Man, I ascended vanilla so I expected I'd be reasonably prepared to ascend SLEX too!",
	"Man... I ascended every nethack variant except SLEX, so I really didn't expect that to be so much harder than even the hardest other variants! Seems like I have to re-learn how to play the game if I want to win!",
	"Dude SLEX is busted, I did a spectacular multi-conduct ascension in evilhack with pacifist and wishless among other challenges, yet I can't seem to survive in SLEX even without following conducts! Amy made it way too hard!",
	"Why the hell is SLEX so hard? I've won Grunthack, one of the hardest variants in existence, so really I expected my standard Grunthack strategy to mostly work in SLEX, too!",
	"Wait what, I'm supposed to farlook every monster because it'll have some attack that fucks you up if I don't? That's just plain silly, even the hardest non-SLEX variants don't require that level of hardcore autism.",
	"Some bastard monster disenchanted all my precious gear and I lacked the resources to enchant them back up, and then some other monster killed me.",
	"#quit - reason: nymph stole my +9 artifact weapon and levelported with it to god knows where",
	"It simply is unfair if a bog-standard monster has destroy armor attacks and nukes the items you spent all game to find. Regular random monsters are supposed to be trash mobs that you can easily obliterate, they shouldn't be able to actually pose a threat!",
	"That permanent stat damage attack is busted and way too many regular monsters have it, so you can never have good stats with your character! No wonder I died!",
	"It is shit that there is a spell memory reduction attack. The bastard monsters kept erasing my main attack spell and even though I read the book several times to re-learn it, they just erased it again and then the book was out of charges! #(rage)quit",
	"FUCK YOU AMY! YOU MADE IT SO THAT THE VULNERABILITY ATTACK CAN DEACTIVATE MY SPIRIT'S PHASING FOR 100000 TURNS!!! YOU CAN'T POSSIBLY BE SERIOUS, FIX THAT OR I'LL NEVER PLAY AGAIN!", /* all caps on purpose, this isn't meant to imitate anyone in particular (there are several players who are really fond of wallwalking but none of them talk like this) --Amy */
	"Yeah why the hell did that level 6 monster deal 120 damage in one round, that's like super unbalanced. Such a monster shouldn't even be allowed to exist.",
	"What are you supposed to do about that superfast monster that hits you with three random status effects per turn? Once it hits you a couple of times, you can't even unihorn the effects away quickly enough, so you'll just die!",
	"That single spiked devil ended my game by first gating in one demon, then it and that other demon gated in two more, then the bunch of them gated in four more, then they gated in seven more and a demon lord, and then that gated in five more and another demon lord, and then I just died. What are you supposed to do about that???",
	"uhh... dude that monster was much stronger than all the other monsters at that depth! something's gotta be bugged there, no single monster should be that strong, especially since it wasn't even a boss or anything",
	"Amy that feature is sexist! It objectifies women and denies nonbinary people the right to be a gender other than male or female! You must remove it from your game or I'll rile up a huge hate mob that will force you to!",
	"Oh great, I thought this game had a sane developer, but nooooo, I rolled the same character that I had ascended in an earlier version only to discover that the evil Amy witch has nerfed everything I used in order to win.",
	"That's it! I'll discontinue playing SLEX now until Amy listens to me, because I am entitled to keep ascending the game with the same strategy I used before. Either she reverts the bad changes that nerfed my strat or she can just shrivel up and die.",
	"Amy fixed what she considers an 'ascendancy bug', even though I legitimately ascended her stupid game. Since my winning strategy no longer works in her game now, I'll switch over to malorhack, whose developer will happily let me keep ascending the same char over and over until I die of old age.",
	"Bah, those pompeji squads are far too strong, they're hard to kill and will off even a strong character in a few turns!",
	"Amy you should remove the pompeji squad thing again, I used to be able to kill shopkeepers and temple priests with impunity but those overpowered bastards just wreck my char every time I do!",
	"What was Amy thinking??? I killed all of one shopkeeper, and a few thousand turns later a fucking level 20 'pompeji poisoner' spawned and absolutely obliterated my low-level character! There's unbalanced and there's unfair, but that encounter was off even those scales!",
	"Blah, the fluidator squads ruined my equipment, all I did was wallwalking so I really feel that there shouldn't be such a harsh penalty.",
	"Amy you suck, why did you make it so that fluidator squads spawn all the time whenever I use my wallwalking ability. And most aggravatingly, far too many of them trash your stats with poison and similar shit, ruining my character.",
	"Eh, #quit, won't play the game again. If I can't even pick spirit and play them the way God intended, the game sucks. You are supposed to be able to shortcut past everything and mostly not be bothered by monsters if you phase through walls, but the fluidators make that impossible.",
	"The hell is a 'fluidator drerge', why on earth did that spawn and attack my incorporealoid and why on earth does it have six stat-draining attacks that fuck you over like you cannot believe??? Unplayable game!",
	"slex is balanced just like car accidents are balanced",
	"the only person games are allowed to be fun for are the developers, wake up sheepie",
	"If anyone is caught having unauthorised fun, expect that whatever you're doing will be removed from the next version of the game.",
	"Amy that artifact is a weapon, so it sucks! The only role that exists is the monk, and those have no use for weapons!",
	"Amy that artifact is a torso armor, so it sucks! The only role that exists is the monk, and those can't use non-robe body armor!",
	"Amy that artifact is a shield, so it sucks! The only role that exists is the monk, and those have their martial arts screwed over when wearing a shield, I don't care how many great artifact enchantments the shield has I'll not use it anyway!",
	"<amateurhour voice> things working in favor of the player? unthinkable! burn the heathen!",
	"whaaaaaaaaat? Amy I was wearing flying boots! The game was asking me whether I really want to step into water but I knew that I was flying so of course I said yes, and I still fell in the water, that's a big fat bug! Err YOU SAY THAT'S WORKING AS INTENDED??? FUCK YOU AMY, it isn't!!!!!!!!",
	"remember, game balance goes both ways, stuff can be unbalanced both in the game's favor or also in the player's favor, but at least 99%% of players are such filthy casuals who never see the latter form of unbalance as what it is", /* originally by Amy but then amateurhour started spamming the line whenever Amy said something about game balance :-P */
	"no alignment distinction allowed - the only thing alignment is allowed to change is which free skill you get on your secret advice member, everything else is just too player-favoured and fun", /* by amateurhour */
	"if a change makes the game suck it was actually intended and therefore doesnt suck lmao gottem i win", /* sic ("doesnt" instead of "doesn't"; normally that gets fixed for Amy's own lines but in this case the original quote is preserved) by amateurhour */
	"para rnz(100) and perfume bleed rnz(100) was intentional and didn't suck lmao gottem i win", /* likewise by amateurhour */
	"> u can attempt to use various skills that appear in your character's enhance screen, but it's really a waste of time because it takes far too long and you'd be better off just using any of the firearms skills even if they're restricted", /* again, by amateurhour */
	"Bah, SLASH'EM doppelgangers suck, why do they always have their regular form's maximum HP reduced by like 20+ points whenever their polymorph times out. And to top it off, there's not even a message, they just cut down your HP max without telling you. Bastards.",
	"cheat codes that Amy uses = approved, any minor benefit that any other player uses = verboten :P", /* by amateurhour */
	"> the monster difficulty is increased by 10 and boss monsters are much more common, their items sometimes teleport out of their inventory, they can't buy items in shops, and watchmen get hostile upon sighting them, lol what next a race that dies once per turn but in exchange they get immunity to farts?", /* by amateurhour */
	"err what, upon entering dungeon level 66 I suddenly encountered a HUUUUUGE amount of random traps as well as a level 89 demon prince that proceeded to kick my butt, that cannot be right? I had been to that level before, what gives? Huh, penalty for having lifesaved? OMG THAT'S BULLSHIT!!!!!!1111",
	"why do the rewards all blow, you could make the reward worth the hassle and sadness! what next a branch that sets all skills to unskilled and is 100 floors long, reward: training of 1 point to ur lowest skill, think about it that's 1 whole point you don't have to train!! in a potentially hard to train skill!!", /* by amateurhour of course :-) */
	"Amy it's no fun that you gave all those downsides to the reticulan, the race used to be balanced and now it's not anymore.",
	"Amy, making the reticulans inverted once their health is close to zero is just like saying 'fook yoo' to any reticulan players! Just reducing the proc chance of their inertia attack would have been enough of a nerf!",
	"Yeah, 'wince' is such a stupid mechanic, that monster displayed 6d6 damage and I still died even though I had 37 health left.",
	"You cannot play the game with the double invert effect. Why can't Amy like, I dunno, make new in/extrinsics that are actually fun instead of always creating ones that basically equal a death sentence for the player?",
	"Floating eye instadeaths in vanilla are bullshit! You can't just walk down a dark corridor in the early game without a lot of slowdown, because one of those cunts can be next to you after any turn and then that one overshot keypress attacks it and ends the game.",
	"Did you think you were safe from floating eye bullshit instadeaths just because your vanilla char was walking through a completely empty, lit room? Ha! While you mash-mash those directional keys to move from one end to the other, a floating eye in a completely different room walked into a teleport trap, and warped to the square you were going to move to next. Game over, because there was a coyote in the adjacent room.",
	"Chameleons are a particularly 'fun' vanilla feature. Every player loves it if the tiger they're fighting suddenly turns into a floating eye because unless you play extremely slowly, your next keypress attacks it and ends the game.",
	"Cockatrice instadeaths in vanilla are bullshit! You don't even get the chance to save yourself with your 200 stockpiled lizard corpses, and to add insult to injury, even if your form has a fucking weapon attack YOU STILL ATTACK WITH YOUR TENTACLES OR BITE FOR CRYING OUT LOUD!!! That is so incredibly retarded, a real-life mind flayer or vampire would certainly be smart enough to only hit with the weapon!",
	"The fact that vanilla lacks paranoid water confirmations is bullshit, you don't even see the water because the color is too fucking dark and when you do fall in, you're guaranteed to ruin all of your scrolls and rust every last item that can rust, which is waaaaay too big a penalty for one single typo.",
	"The fact that vanilla lacks paranoid trap confirmations is bullshit, because the character knows the trap is there! If at least the interface somehow made it clear that there is a trap on the pile with items, but nooooo, they're lying there as a bait so that you trigger the trap again, and every time there's another chance that the game rolls a deadly poisoned dart/spike and ends your run through no fault of your own.",
	"It is complete and utter bullshit that vanilla gives no repeating message about you sinking deeper in the lava. In fact, they even made the message use the NOREP function, which causes it to not repeat if it would occur several times in a row! That sure looks like intentional malice, designed that way because the devs gleefully wank off in delight whenever a lava death message appears in the logs.",
	"The breath meter in Terrahack is stupid, it clogs up the damn status line with information that I don't need and yet cannot turn off, and it doesn't even allow me to cross water! My char crawled out, now with 50 blank scrolls and 50 rusty weapons, and didn't give me the chance to wade through one more adjacent water tile to reach the shore on the other side! How retarded is that???",
	"Chameleons don't imitate other monsters, they actually become the monster in question temporarily, with all its abilities. So you don't get killed by 'a chameleon imitating a mastodon', you get killed by a mastodon. The death messages in TSX make no sense.",
	"It really is stupid that you get no message when a chameleon in vanilla changes form within your view. Like, would it be too much to ask to tell the player what is going on??? If there suddenly isn't a leocrotta anymore but an arch-lich, maybe I'd *like to know that* so I can react accordingly?",
	"Nooooooo, I ate all the chocolate and it still didn't cure the accursed totter effect and you can't play with that. #quit",
	"Scroll of symmetry, gah. Why on earth did Amy even add that thing to the game? Is it just there to screw over those who read-test random scrolls?",
	"Err what, quaffing the Boomshine artifact potion gave me confusion, how long does it take for that to time out? What? IT'S PERMANENT??????? MOTHER FUCKER, Amy your parents should never have given birth to you abomination, now my character is ruined 200 hours into the game with NO way to fix it, that's it, I'm throwing the game on the trash heap! You'll never see me play again!",
	"Whaaaaat, I read the IBM guild manual and nothing obvious happened but then I tried to get rid of an annoying monster type by reading a scroll of genocide and I died! That's stupid design and you know that!",
	"Huh, I got intrinsic map amnesia, how do I get rid of that? What, I have to find some extremely rare item, I don't have that item! And what's that you say, the only other guaranteed method is to play through the goddamn minotaur maze? FUCK OFF AMY, I'm not doing the minotaur maze! #quit, and you'd better remove the thing that gave me that stupid intrinsic permanently or that was the last time I played SLEX!",
	"Bulletators??? What next, breathators that punish characters who dare to breathe in the dungeon? This game's busted, I tell ya.",
	"Omg all I did was fire the demon crossbow a few times, and then suddenly a bunch of 'bulletator tildes' appeared and hit me with basically every type of attack that exists and then I died! That's really unfair!!!",
	"Hey, why isn't my xelnaga allowed to use a submachine gun?! I started in the space base, roles that start there are supposed to be allowed to use all firearms! Why did I have a bunch of deadly bulletators spawn, that's not very fair!",
	"Yes, that's it, last time I played this game, I just fired my catapult a few times and it's not even very strong but Amy somehow decided it was okay to summon 200 bulletators in response. And she gave them uber stats so that you can't ever hope to kill them before they kill you. Goodbye Amy, I'll play Splicehack instead.",
	"Well, I was gleefully fighting a throng of monsters in that awakened nefia and then suddenly a yerles cyborg that I didn't even see used the critical particle cannon and I died instantly. Blah to that, I say. My char just doesn't have enough health and therefore I'm always in danger of being instakilled by something that deals too much damage.", /* happened to Amy in Elona */
	"There's nothing that has only upsides and no downsides... except in unbalanced role-playing games. SLEX doesn't want to be one of those, which is why stuff that is too player-favored needs to be reigned in.",
	"why are they picking fun combos to play and not what I want them to pick!!! why are they picking races with limbs, why are they picking races without turboassitis, why are they enjoying themselves!!!!", /* by amateurhour */
	"We've got plenty of food stocked up.  We could stay in this place for months if we had to.",
	"We're almost out of food. You, share some of yours with us.",
	"I'm freezing.  I can't feel my fingers.",
	"They say something horrible lives in the sewers.  It smells horrific though, so I'd never go down there.",
	"We're out of supplies. Anything you could give would be blessing.",
	"I wonder what's become of my hometown.",
	"Bamboo shoots don't taste good at all.",
	"Our hard work this year will save us next year.",
	"The scholars who went to investigate the hills in the north have yet to return. They're probably monster food by now.",
	"Just who in the world is that attendant? She mowed through the monsters as if they were nothing.",
	"Tezcatlipoca was the trigger for the Great War between the Gods. It is said that he alone massacred thousands of Gods in that battle.",
	"The final battle with the God of Chaos was fierce battle; one that we barely won. If he awakens again, the Gods of today who are few in number will surely be unable to stand against him.",
	"The Shaman can do more than just erase a person's memories. She can also erase those of every acquaintance he's ever had.",
	"If you don't keep your mind blank when erasing memories, she might just end up accidentally erasing more than what's necessary... along with your sanity.",
	"Before you realize it, you're already one of them.",
	"The Shaman lost most of her power a long time ago when she was injured. She used to be able to manipulate even the free will of people.",
	"Someone of the Shaman's status should not be cooped up here like a caged bird.",
	"People live by forgetting. Memories of pain and sadness will only bring you suffering.",
	"Don't worry. He can't move a muscle while he is shackled.",
	"Naughty customers who look around and don't buy anything get eaten up by the giant in one gulp.",
	"Bleh, finding the alignment quests is far too hard and the punishment for not finding and completing them is insane!",
	"What? I'm being penalized with having to fight Ubernightmare just because a bug made it impossible to complete the Nightmare alignment quest??? That's bullshit, Amy, and you know that.",
	"Amy you should make it so that whenever you enter a level with an alignment quest portal, you get a message so that you can go look for it.",
	"Fell asleep trying to step on every square on all dungeon levels from 15-49 trying to find those damn alignment quest portals, and then my character died to something stupid.",
	"Amy if I were to make a variant of your game I'd have only the fun stuff and none of the stupidity!",
	"Amy, you should design your game in such a way that players will find it fun, not punish them for things that aren't even their fault! Otherwise, no one will ever play your game!",
	"SLEX is stupid, you blissfully plow through all the monsters for dozens of dungeon levels and then one of them just soloes you because they had some shoes.",
	"Oh my god I realized the kurwa role in this game can have sex with the 'child nurse' monster! That's so fucked up, I'm deleting the game from my hard drive, removing all accounts on all game servers that run this stupid game, and change the wiki page to say that this game contains significant amounts of highly problematic content.",
	"Amy, the 'antje trap 2' is grossly offensive, I won't touch a game that has an optional minigame which lets you rape an unconscious girl, even though there's no obligation to ever interact with that content. Because nullifying everything about a game as huge as SLEX because of one single feature I consider offensive is the proper course of action. Yeah.",
	"Oh no, I became stupider! If the mind flayer hits me another time I'll get a game ov-",
	"What, a 'noneffervescent tatting'??? It has 9d4 wrap, oh well that usually doesn't hit anyway, but a passive superpoisoning attack, a 9d7 thieving kick, RNG intervention spells, a 4d5 tentacle attack that destroys armor and a 5d4 lethe attack??? Like dude you're fucked up, whoever made up such an unbalanced monster?",
	"Uuuuuuuuhhh... if the 'corset nudism' hits me, it's game over. A ranged 3d18 fear-inducing attack, randomized-effect spellcasting, a 1d21 specific bite and a *6D20* vulnerability hug attack??? If that hits me even once, game over. And the thing's speed 21, too. Hopefully it doesn't--",
	"Another adventurer carried this to their grave, which thou hast already approached. Search around such an area.",
	"It has lain somewhere in this dungeon for a long time, awaiting the next hero. Thou hast already passed near to it.",
	"when a new race exists in several variants and has different implementations depending on variant, nothing can be suckier than the slex one cuz to play the slex one u gotta be playing slex", /* inspired by amateurhour */
	"looooooool forcing a chest with autounlock! that's so typical for TSX :P  'hey, sorry dude, but the castle chest was locked and you happened to be wielding a blunt weapon, well now the chest is totally destroyed and you see a hexagonal wand shatter! hahahahahahhahahahahaHAHAHAAAHAHAOMEGALUL'",
	"singslave: lol yet another 'hahahaha all parts of the game suxx0rz like you cannot believe' role :P", /* by amateurhour */
	"Blahonir's lair sucks, reason: there's traps that remove your disint res and destroy your stuff or disintegrate you, and lots of monsters with disintegrate attacks, and a demon lord that steals ur shit if that's what you had for disint res, and the traps are almost certainly on critical path", /* by amateurhour */
	"then take venonir's lair, traps remove poison res and prevent poison res working, and a covetous demon lord that uses venom, so you step on a trap, he teleports to you, gg no re you instadied from poison", /* by amateurhour */
	"what, you didn't figure out by yourself that the minotaur maze is a collossal waste of time and not actually required to beat the game?",
	"whaaat, you didn't realise that skipping an alignment key would mean you have to face an uber version of the boss and instadied? LOL gg",
	"whaaaaaaaat, you didn't know that hitting an ogre thief 3 times would straight up kill you??", /* it's specifically the *huge* ogre thief that does, regular ogre thieves aren't special-cased --Amy */
	"whaaaaaaaaaaaaaat you didn't know helm of neutral exists and guarantees you can skip two thirds of astral??",
	"I don't have any healing scrolls and potions now, all the techniques are in the 'LIMIT' state and cannot be used, and I can't escape.", /* sorry Pineapple Tycoon but you could still have used them of course, should just have tried ;) */
	"Damn, why the hell do my techniques always display LIMIT when I'm about to die? I would have lived if I could have used them!", /* admittedly the design is unclear on that one, the game doesn't tell you what limit break mode is and not all players are cunning enough to go "well, let's just try to use them anyway!", I've seen it happen to several players already that they thought "ah fuck, can't use the techniques now" and just closed the menu without attempting to use one */
	"Gah, the katoiseful is unbalanced as heck, they look just like gnomes and also appear where gnomes usually appear, i.e. in the gnomish mines, but they seem unkillable and completely wreck your shit with their attacks!",
	"WHAT? A psybur hit me ONCE and my maximum mana went from 171 straight to 35???",
	"Hey, something reduced my maximum hit points to half of what they originally were, which item do I need to use in order to restore them to their old values? SAY WHAT AMY, you need to build them back up the hard way??? UNPLAYABLE GAME, #quit, you'll never see me again you bitch!",
	"WTF, I just died. Dude, I was polymorphed into a powerful form with 334 maximum HP and that nazgul just killed me instantly. 'It breathes--More-- You have died.'", /* happened to Amy in angbander mode */
	"Being an idiot is great, don't you agree?",
	"huh something has got to be bugged there, I was in Minetown with my level 4 character and offered 1600 zorkmids but the priest somehow didn't give me protection. Amy must have made a programming mistake somewhere.",
	"whaaaaat, you nooblar didn't instantly guess that aphantaconotan disks cure insight? lol every little child knows that, it's something you learn at elementary school! of course I didn't bother adding an in-game hint about that because it was supposed to be OBVIOUS, you gross ignoramus! :D :D :D",
	"but of course the crystal skulls have to be taken to level 3 of Mithardir and dropped on the eight squares around the magic portal which will cause Father of Death to appear, the secret boss of Mithardir who holds the fourth key of law! lol you REALLY had to ask me to figure that out lol noob :P",
	"Here lies the zombie technician, killed by a monster that still has the same name but lost the unique ability of pulling the player's metal items out of their grasp. As a result, the monster with that name is now a very boring garden variety zombie with no abilities that make it stand out at all. Yawn.",
	"Portal storm spawns are BS! They seem designed to just end your game randomly, everything else in the game is manageable except maybe melded task forces or armored centipedes, but the portal storm just spawns some wacky crap that kills you with no chance for you to fight back!",
	"Here lies Madelene(sic), killed by a skeletal juggernaut because the player character wandered off looking for ammo for ONE minute. Sadly, the player didn't just go 'oh hell no you stupid game that was developed by stupid people, that didn't happen, *reloads from last save*'.",
	"Here lies Earnest Dejesus, killed by a goddamn version update that caused him to be ERASED OFF THE FACE OF THE EARTH, along with all the other party members on that savegame file, leaving only the player character to walk the resultant corrupted world. How the hell can you program such an update and then not test it before rolling it out???",
	"Yeah the melded task force sucks, if you ever allow it to get a turn in melee you're dead. #ragequit, not playing that game again.",
	"Yeah the melded task force sucks, I had unfinished business in that lab but now there are 200 tentacle things and yuggs and slime whatevers and Shub-Nigguraths and spiral kings and yomagnthos and everything, if I stay I'll just certainly die and if I leave, all the loot will be lost. #(rage)quit",
	"Yeah the melded task force sucks, it doesn't make sense that the monsters spawned from the thing that appears after its death don't just take over the world because after all, there is no way to stop them spawning and they come in endless amounts. Oh, they don't do that anymore when they're outside of your reality bubble? Fail, 0/10, you have to come up with a believable explanation or make it so that the rift in reality can be repaired, or I will not buy the what-passes-for-logic of your game.",
	"walked around near the edge of the map on dungeon level 71 and suddenly the game was gone. what, you're saying that the ceiling collapse trap is trying to drop rocks on squares that are off the map? like why would that be the case", /* based on shadowrider38's game, although the bug could be reproduced and fixed */
	"The ghost world nasty trap effect suuuuuuucks like you cannot believe! Why oh why does it make it so that my pets automiss those super-invisible monsters regardless of stats or anything? The programmer who did that is such a moron!",
	"The stairs became invisible because a trap happened to spawn on them, and I couldn't find them anymore despite stepping on every square. #quit",
	"bluh Amy, why the hell did you make the zruty race's movement speed so slow, unplayable, won't play again",
	"Yeah the ent race sucks, no one can play that because they're far too susceptible to fire.",
	"Dude, the jabberwock race wasn't overpowered! It was merely good! Making monsters retaliate even more quickly and also making the player more likely to suffer from amnesia was completely unnecessary, and that you even increased the average monster difficulty on top of that is supreme bullshit, Amy!",
	"It takes too long to level up if you pick 'monster' as a race, I decided to quit the game out of boredom. Actually, I decided that all of Amy's newly added races can just bite me and from now on I'll only play human, gnome or dwarf.",
	"God Amy must be out of her mind, she made it so that unalignment things are nerfed even more despite already being one of the hardest races due to their handicap. There really was no need to increase the speed of hostile hardcore aliens when you play as that race!",
	"Whaaaaaaat, unalignment things already have extreme difficulty due to their lack of hands but now Amy made them train skills at two thirds the normal rate??? Like does she not want people to play her game or what?!",
	"AMY DIVIDED THE DAMAGE OF THE SCURRIER RACE'S BITE ATTACK BY HALF OMG OVERNERF!!!!!!!!11111 HOLY SHIT DO YOU KNOW HOW FUCKING IMPOSSIBLE IT IS TO KILL ANYTHING AS THAT RACE!!!! OMG DOES SHE EVEN PLAY THE GAME??? !?!?",
	"there was no reason to reduce the amount of damage done by trapper race player characters... amy always nerfs the wrong things...",
	"Okay this is it, I'm officially not touching SLEX anymore now. When Amy nerfed the reticulan race half a dozen versions ago, I was already very annoyed, but now she additionally cut their melee attack damage from 4d4 down to *1d4*!!! It is now completely unnecessary to even bother playing such a weak race. Back before her pointless nerfs the race was pretty good and fun to play, now it's just... not.",
	"Fucking hell, why did she nerf the melee damage output when playing as the lich race? I mean, it's a motherfucking lich, an ancient undead being with awesome power, so for the sake of consistency, they should be awesome! We need to start a petition to make Amy un-nerf the lich now!",
	"Hey Amy, your nerf for the mazewalker race sucks, it's already annoying enough to trudge through all those boring-ass, samey maze levels, you could have at least left in the 2d8 melee attack. That you cut its damage down all the way to 1d3 is a huge dick move, are you aware of that?",
	"Well the unmagic player race certainly wasn't too strong! Stupid nerf is stupid! There was already no one ever picking that race but now Amy made sure that no one will pick it in future either and those who accidentally do, well, they're going to quit after five dungeon levels tops because the regular human race is so much better.",
	"Ugh, the race I picked seems to be pretty good, it has been fun to play but I just know that if Amy ever catches wind of that, she'll nerf the hell out of that particular race just to annoy me. So I guess I have to arrange some stupid death to prevent her from noticing.",
	"Can't have nice things in SLEX, I guess. Yes, I'm playing a powerful role that looks pretty ascendable because of that special trait, but Amy will nerf it, I just know she will. So I'm saying RIP in advance because this'll likely be the last time ever that this playable role is actually enjoyable to play.",
	"Amy is evil, she has stolen my joy, I was playing a doppelganger and doing great and then she suddenly decided to limit the amount of free polymorphs to a paltry 10. There, she single-handedly took away all reason to even play the doppelganger. Until she reverts that ill-called nerf, I'll play SLASHTHEM where that race works the way God intended.",
	"Yeah the monk was overnerfed, even if you wear a robe and no shield and have maxxed martial arts you only deal like 25 damage per hit. In case you don't know, Amy, in SLASHTHEM they consistently deal 40 or more. Guess which variant I'll be playing from now on. Fuck you and your variant, Amy! SLASHTHEM forever, long live the type of ice block!",
	"In roguelikes, combat encounters fall into a few different categories: encounters that are trivial to win; encounters that need skill to win; encounters that need consumables to win; encounters that cannot be won, but can be postponed; encounters that cannot even be escaped and inevitably kill you. Obviously, I got the last one.",
	"In roguelikes, combat encounters fall into a few different categories: encounters that are trivial to win; encounters that need skill to win; encounters that need consumables to win; encounters that cannot be won, but can be postponed; encounters that cannot even be escaped and inevitably kill you. And I had even left the level in order to postpone an encounter of the fourth type but then stupidly went back to that level and got killed anyway.",
	"In roguelikes, combat encounters fall into a few different categories: encounters that are trivial to win; encounters that need skill to win; encounters that need consumables to win; encounters that cannot be won, but can be postponed; encounters that cannot even be escaped and inevitably kill you. Got the third type, the one that requires consumables, except I didn't actually HAVE any consumables. Rats.",
	"In roguelikes, combat encounters fall into a few different categories: encounters that are trivial to win; encounters that need skill to win; encounters that need consumables to win; encounters that cannot be won, but can be postponed; encounters that cannot even be escaped and inevitably kill you. Seems like I lack the required skill for an encounter of the second type. What a letdown.",
	"In roguelikes, combat encounters fall into a few different categories: encounters that are trivial to win; encounters that need skill to win; encounters that need consumables to win; encounters that cannot be won, but can be postponed; encounters that cannot even be escaped and inevitably kill you. And somehow I managed to lose the game to the first type. No idea how I managed that.",
	"In roguelikes, combat encounters fall into a few different categories: encounters that are trivial to win; encounters that need skill to win; encounters that need consumables to win; encounters that cannot be won, but can be postponed; encounters that cannot even be escaped and inevitably kill you. But I want to say that this one was the sixth type: the one that requires lucky RNG to win. Guess it wasn't my lucky day.",
	"In roguelikes, combat encounters fall into a few different categories, but this one was the 'fuck you player, you're dead no matter what you do' category. Bullshit.",
	"In roguelikes, combat encounters fall into a few different categories. But regardless of what category this one was, I got so bored of playing this stupid game that I fell asleep, my head ended up holding down some key and when I woke up again, I was staring at a DYWYPI prompt.",
	"In roguelikes, combat encounters fall into a few different categories. This particular one fell into the 'sexy monster that I'd find arousing to be killed by' category, so I just tapped the space bar with my right hand while the left one was busy massaging my huge boner, and then I died.",
	"What, the bot died again! It didn't announce my death!",
	"Gah, not only did I die to sliming, the game didn't even display a proper death message and instead threw up some ASCII gibberish as the death reason.",
	"Yeah you can't play vanilla nethack, you can't get any of your damn potions and scrolls identified and if you carry them out in the open, a monster with fire or cold attacks will definitely spawn sooner rather than later and deal extra damage to you proportional to the # of carried consumables, killing you instantly.",
	"The best way to deal with random monsters with a wand of death in vanilla nethack before you can survive them is to pretend that they're not in the game and roll a new char if you do run into one. I'm fairly convinced that there's a game design problem if the game really plays like that.",
	"Random instadeaths that you can't avoid are no fun! If there's special code that allows you to survive a death ray once even without reflection or MR, you'll still die to the next zap because the FILTHY monsters can CH3AT by moving and instantly zapping you again all in one turn, even if you moved such that you'd no longer be lined up with them!",
	"Oh and I'll still maintain that Nethack sucks. This cannot be argued. Might as well have an 'Ultra Death' enemy that just has a random chance of spawning right next to you if you do literally anything. If it does, it instantly kills you just by having spawned and ignores the effects of Amulets of Life Saving in the process.",
	"If a game developer just ignores feedback altogether and pretends to never have received your complaint, it's a sure sign that the game is terrible and should be stayed away from. A sensible developer would at least say 'no, won't change that' and thereby have the game qualify for the label of 'bad' (as opposed to 'utter heap of crap').",
	"I want to play SLEX, and not constantly feel as if a sharp object is being poked into my ass rim.",
	"I'd prefer to have a game that simply runs, over one that uses maybe 0.01%% less memory of which I have more than enough anyway and constantly bugs out.",
	"Huh? Droughter hybrid race? Amy are you serious, how am I supposed to survive if monsters only drop a corpse 1 in 20 times they were supposed to drop one??? Unplayable game!!!!!!! You should remove that playable race so that players don't accidentally pick it!",
	"nooooo you fixed it! i wanted to autowin the game by using scrolls of flood to make enemies not attack me and you evil amy witch fixed it!!! i am too bad to play the game properly i can only win with exploits what am i gonna do now???????", /* yeah loggers of course a game that has been in development for 10 years and whose dev has a very fine-tuned antenna that detects things that make the game easier than intended, would have overlooked that one. suuuuure, dream on. this isn't dnethack where exploits are not only tolerated but part of the official easy ascension strategy (and the reason why so many players have ascended dnethack in the first place) */
	"I attacked Pale Night in melee, and my char was so stupid to remove her veil and died instantly!",
	"What, the final enlightenment screen said that I was 'being fucked over', why is that even specifically mentioned in SLEX? It's always the case, no matter what char you play and no matter how you try to avoid the stupid stuff in this game!",
	"Noooooo, I got stuck with a cursed pair of columnar heels and couldn't lift the curse, and since I lacked the high heels skill I was slowed down by those stupid heels! I would have escaped from that monster without the dumb shoes! Why do high heels even exist, they're a walking impairment!",
	"Stupid monster kept lifesaving, that's gotta be a big fat bug, Amy. Normally, an amulet of life saving is used up when it saves someone's life. And why did my wand of probing show that the monster's amulet was called 'Nyehehehehe.'? Are you trying to make fun of me?",
	"The hell, there was an 'enemy from level 55' on dungeon level 2! Dude I was just starting out! Why did you place that there Amy, I died pretty much instantly! Maybe you should reconsider your monster spawning routines and fix that unfairness?",
	"Yeah bullshit, the all-important item glitched out of my car when I turned. Stupid UDTV programmers, couldn't they just make items *not* fall out between two car tiles when turning?!",
	"the abyss, I have looked into it and it has stared back and said things like fleecy and bundlebundlebundle", /* by Loggers */
	"Yeah sure, some random mob spawned with a fatman, fired a mini-nuke at me, and I died instantly. Great game, 10/10 would play again.",
	"What? A giant appendage that has 1000 million hit points and fucking kills you in 2-3 hits? What's the name of the illegal drug that the devs have been using???",
	"Well, of course I used the arch-vile jump on The Inmost Dens to get into the area that normally requires the blue key, after all the 'wall' is so ridiculously waist-high that it's obvious you're supposed to pass it that way! What's that you say, the devs didn't intend that and now I can't get back? Really???",
	"Dude, if the devs didn't intend that you can bypass the blue key door with an arch-vile jump and subsequently get stuck behind it then why on earth did they make it so that you can arch-vile jump at all??? Some geniuses they are!",
	"Yes, of course SLEX is a work in progress and if someone abuses obviously unintended exploits to score an easy ascension, I reserve the right to fix that. Who are you to demand that I leave such loopholes in the game for all eternity???",
	"Nooooooo! Amy nerfed the light spell and now it only lights up the square you're standing on!!! Like what the hell, why would someone nerf THAT?! I'll never play this game again, Soviet5lo made a much better variant that is lots more fun to play!",
	"WHAT? Amy you fucked up, surely force bolt isn't supposed to be a motherfucking level FOUR spell??? Lady, it was a level 1 spell for centuries!!!!!!! It sure wasn't overpowered! Now that it's level four, few chars can even cast it and those who can are no longer capable of spamming it because it costs far too much Pw! Revert that nerf or I'll round up a mob of thousands of SLEX players to form a mass protest!",
	"Amy you are the worst game developer in existence, my gnomish wizards were having difficulty getting started already but now you made it so that I no longer start with force bolt and instead have to struggle through the early game with a uselessly weak magic bolt spell that cannot even break doors or boulders??? #ragequit forever, I shouldn't have come back in the first place but now I'm really gone for good.",
	"Amy you dick without a brain, how dimwitted do you have to be to increase the mana cost for ctrl-t teleportitis to a HUNDRED??? like, do you even play the game, do you know how that is far too expensive when the spell costs only 30 mana? if you do not revert that change I'll stop playing the game for good and also tell all my friends to avoid it like the biblical plague!",
	"Guh, stiletto heels are stupid, why do they slow me down if I don't have a very specific skill that only few characters get? Without that I would have lived, I swear!",
	"My cone heels keept disenchanting themselves through no fault of my own! No idea what was going on there, but my DYWYPI screen showed that they were -12 even though they were +0 when I put them on!",
	"Was strolling around the dungeon and avoiding those nasty special rooms with their way too many monsters, and suddenly my block heels made a loud noise that caused all those bastards to wake up and kill me.",
	"Amy your game sucks, why do I constantly step into shit with my wedge heels? And in fact, why can't my character walk in them? I was playing a female character, and I can tell you from real-life experience that women can always walk gracefully in wedges! Your game is unrealistic, wedge heels shouldn't require a specific skill that only few roles start with!",
	"Here lies the judge bitch from hell who tried to be a douche bag to Johnny Gat. She caught a bullet right into her forehead.",
	"Here lies Carlos. After being brutally tortured by the Brotherhood who chained him to a truck and drove him over the asphalt, the boss of the Saints arrived too late and had no other choice but to kill Carlos to end his suffering.",
	"Here lies Jessica, girlfriend of Maero. He crushed her under his monster truck.",
	"Here lies Shogo Akuji, he suffocated to death in the sarchophagus after Johnny Gat had beaten him up thrice, including smashing Shogo's head into a tombstone so hard that the stone broke into pieces. But really, Shogo has only himself to blame, no one forced him to try to kill Johnny at the funeral for his girlfriend.",
	"Here lies Kazuo Akuji, he won the sword duel with the boss of the Saints but then the latter decided to cheat by pulling a gun and firing at point blank.",
	"Here lies Mister Sunshine. The boss of the Saints shot him twenty times, then cut Sunshine's head off and threw it into the meat grinder just to make sure that for fuck's sake, he died already!",
	"Here lies Mister Sunshine's left ear, which got cut off with a machete by his general.",
	"Here lies Dane Vogel. He thought he could mess with the Saints, and ended up eating bullets.",
	"Here lies Julius, the little traitor who once founded the Saints and then thought he could do his own thing. He tried to talk his way out by making lame excuses for his misbehavior, but the Boss shot him in the head mid-sentence.",
	"Here lies the boss of the Saints, he finally decided to atone for his sins by allowing a sweet woman to gently stomp him repeatedly with her soft pumps. Although, being the main character of Saints Row 2, he can just respawn infinitely anyway.",
	"Here lies Aisha, Johnny Gat's girlfriend. She got stabbed by Jyunichi when she wanted to warn the Saints about the Ronin's ambush.",
	"Here lies Veteran Child. The coward thought he could kidnap Shaundi and then hide behind her, using her as a human shield instead of fighting his opponent fair and square. Well... Veteran Child died, Shaundi lived, and Stilwater was a better place as a result.",
	"Power mad to the end, Hloval Dreth met his end in shame.",
	"He died just as he lived: as a selfish moron.",
	"Here lies Mehrunes Dagon. He was burned to death by Martin Septim.",
	"Here lies Colonel B. Astard. The Lone Wanderer eventually confronted him and trepanned his head with bullets to punish the Colonel for murdering James and Janice.",
	"Here lies Dimitri Rascalow. He thought that he could get away with double-crossing Niko Bellic, boy was he wrong or what!",
	"Here lies Ricardo Diaz. He thought Tommy Vercetti was a friend of him, but really the latter was planning to overthrow Diaz's drug empire all along, and in order to do that, he had to pump Diaz full of lead.",
	"Here lies the original version of Saints Row 2, cut to hell by German censorship. Really, there's so many features missing, it might as well be considered a different game - one that should really be rated E for Everyone because the German moral guardians removed everything that was even slightly hinting at in-game violence.",
	"Here lies the ground attack that had existed in GTA3, Vice City and San Andreas, and which the German moral guardians removed from all these games because 'omg violence'. Is it because teenagers in present day Germany do in fact continue attacking their victims when they're on the ground? Does anyone actually think they do that just because they played too much GTA?",
	"Here lies the hot coffee minigame, removed from GTASA by American prudes because 'omg sex'. Even though there's nothing even remotely arousing about that minigame and you might as well be looking at an IKEA catalogue for furniture, which has the same potential (read: zero) of arousing you.",
	"Here lies the original version of the song 'Break On Through' by The Doors. Someone decided that the word 'high' was somehow offensive and removed it from the song for no good reason, therefore making that specific line of the song nonsensical and out of context.",
	"Here lies the original version of the song 'School's Out Forever' by Alice Cooper. By butchering a specific line, the lyrics stopped making sense altogether because NO, you are NOT 'bored to pieces' (that's a phrase that doesn't even exist) when school's out forever, you're happy like you never were before! Ask a pupil - any pupil - and they'll confirm that they'd cheer when school was blown to pieces!",
	"Here lies the deity Lucy from DCSS, who was somehow renamed to Lugonu because 'omg a name that could be interpreted as being female, that may not be!'.",
	"Here lies some mundane item or other feature from DCSS, e.g. a wand of cold or scroll of teleport. You could really insert anything here, because no matter what it is, it got removed in some patch for no real reason.",
	"Here lies the song 'Fuck You' by CeeLo Green. Someone apparently thought that the presence of the F-word was offensive and therefore censored it (changing it to 'Forget You' in the process), which is so very petty because there was nothing wrong with the original version of the song.",
	"What the hell, I ate a mimic and then teleportitis took me to a square with a stalactite and because I was flying, the stalactite did 10-20 damage to me every turn yet my stupid char just would not stop mimicking and so I died with no way to prevent it!", /* happened to Amy */
	"Used the cheater prayer to get rid of that shitty loadboulder. What else could I have done? But, the negative effects are just too detrimental and so I died shortly after, with no chance despite no longer being overloaded.",
	"how do i move, number pad keys arent doing anything, stupid game always says count:22222 or some such garbage, unplayable",
	"fuck this stupid game, help displays 200 silly commands that i dont need and says number pad moves me but it doesnt it always says count:444 and doesnt work, unplayable game, gonna play dcss instead",
	"couldnt figure out how to move after 5 minutes, i kept pressing number pad keys and nothing worked, if i cant even move the entire game has got to suck so ill play my favorite roguelike again and just file this 'slex' thing in the dust bin",
	"yeah the game said something about going through an options menu and pressing this button and changing that option so that i can move with the number pad, i did all that and it still didnt work, gonna play a different game that has playable controls",
	"Courage is when you don't run away from a difficult situation. Foolhardiness is when you still don't run away even after the situation has turned lethal.",
	"Boo, the shifting sands shouldn't say it kills you by suffocation.  Seems like bs since I went and put on my amulet of magical breathing and everything  :(", /* sorry but the message says very clearly that you WILL die if you stay in there, should have listened --Amy */
	"Here lies Tanya Winters. Despite her name, she was not a fleece-Tanja, and her last mistake was to immediately pull a Krukov and open fire at her former boss, Benjamin King. Who then proceeded to gun her down and throw her out of a window.",
	"Here lies Warren Williams. He didn't realize that one does not fuck with Benjamin King, and as a result, Warren's car burst into flames with him inside. When that STILL didn't kill Warren, Tanya then appeared, turned on him, and shot him in the head.",
	"Here lies Angelo Lopez. While he realized that the Los Carnales were going down, his airplane got shot up by the Saints before it could take off, and Angelo was caught in the explosion and died instantly.",
	"Here lies Hector Lopez. McManus said hello.",
	"Here lies Victor Rodriguez. He thought that the Los Carnales could beat the Saints, but ended up getting pumped full of bullets and died.",
	"Here lies a bag filled with last season's heels, which Luz had to leave behind when Angelo called her and said that she needed to hurry to the airport. Since the flight got 'cancelled' by a rocket launcher fired by the boss of the Saints, Luz is still around somewhere, so if you find these beautiful heels, please return them to her <3",
	"RIP colorful stripper girl platform shoes, due to a bug they always wear default-colored ones which look really goofy. Editing the game data files does not work, the shoes don't want to become pink or cyan. :-(",
	"RIP any random character who got taken as a human shield by the boss of the Saints in the second game. Why on earth the programmers didn't code in a 'release' button, we will never know, but you can only either execute the human shield or throw them against an obstacle, which usually kills them. (Hint: get next to a car and try to enter it, THAT somehow releases the NPC.)",
	"RIP super-powerful magical cloak, some random monster had a scroll of destroy armor and read it.",
	"RIP meteoric steel shield, a black dragon breathed disintegration at it.",
	"RIP flying boots of blindness resistance, a sword flowey tagged them with a nastycurse attack and now they're flying boots of quasar vision instead.",
	"Here lies the word 'dick' in the Kelis song 'Trick Me'. It's incredible how every word in every song that could be considered even vaguely offensive gets censored.",
	"Here lies Lin. She got shot by that asshole Sharp and since that wasn't enough for that motherfucker, he then drowned her body in the sea.",
	"Here lies William Sharp. Despite having murdered an innocuous girl in cold blood, and in an extremely gruesome way no less, all that happened was that he got shot to death, which is a far too good-natured kind of death for such a motherfucker.",
	"Here lies Joseph Price. For the final boss of the Westside Rollerz, his death was actually rather anti-climactic: the Saints leader just shot up his truck and Price exploded.",
	"Here lies Chief Monroe. He thought he could fuck with the Saints just because he was the chief of police, but the Saints weren't buying that shit and blew Monroe's car to kingdom come with him inside.",
	"Here lies Alderman Hughes. Motherfucker of a politician REALLY thought he could just take out the Third Street Saints after they had managed to finally rub out the other gangs and attain supremacy over Stilwater... but then Hughes' yacht blew up, and the tie-wearing asshole was disintegrated into a fine mist.",
	"Bullshit! There's nothing that you can do if Greed steals your box and empties it on the floor! Even if you get most of the items back, he still has the box so you have to either leave them on the ground for any asshole to grab, or risk having like 30 or so items stolen PER HIT.",
	"Bleeeeeh, Greed just didn't want to stay dead and then I somehow had the autopickup nastytrap which worked on his fucking corpse too, and ultimately he killed me!", /* these last two happened to Amy */
	"My guy was watching the lovely girl futilely try to kick him with her platform shoes but then suddenly he started walking like he was on autopilot, and even when the girl caught up and started kicking the crap out of him with her very cuuuuute platform boots, he still continued walking and eventually got his butt kicked.",
	"This guy just couldn't resist the girlful kicks placed into his shins by the lovely cyan platform girl boots, and eventually got bludgeoned by them in a very female way. <3",
	"Here lies a feminist, whose random level 8 nasty trap turned out to be rot13 making the game nearly unplayable.",
	"Here lies a feminist, whose random level 6 nasty trap turned out to be low local memory making the game nearly unplayable.",
	"FUCK nasty traps FUCK who created them FUCK FUCK FUCK those horrible interface screws are unplayable FUUUUUUUCK!",
	"Man, if I ever get the developer into my fingers who added nasty traps to SLEX...",
	"Nooooo, not the heap trap... every other nasty trap type I could have handled but not that one, #quit",
	"Goddamn secret attack nastytrap, that bastard monster used a lethe attack on me even though it doesn't actually have that and now way too much of my stuff has been irreparably damaged!",
	"Nooooooo, the assiebassie of a spider grue, of course he was in the water and I fell in! Now all my stuff has been rusted because of that bastard!",
	"Here lies a random boss monster from Diablo 2. This boss's special ability was 'extra dead'. Meaning the boss died before the player even had a chance to highlight it and see its characteristics. :-P",
	"Here lies Thrower, a (surprise!) throwing weapon barbarian. In act 3 of nightmare he ran completely out of ammo and just picked up that long sword off the ground and waded in swinging, which was just as effective as his high-level throwing weapons, meaning all those skill points turned out to be obsolete so he retired.",
	"Here lies Ribinak, defiance paladin. Baal used his Agent Orange three times in a row and the stupid healing potion was way too slow, a reju potion would have saved him but the player mistyped.",
	"Here lies any vanilla Diablo 2 character ever because the super dumbie healing potions are soooooooo sloooooooooooow!!! Even the strongest one heals maybe 40 HP/second while the hungry dead potatoes in act 1 hell deal like 200 damage per hit and come in groups.",
	"Here lies Hotfoot, fire sorceress. Her spirit wilted under the shackles of the spell timer system, and she died from complications of indigestion (too much candy).",
	"Here lies the pre-1.09 frozen orb skill of Diablo 2. Back in the day, you'd be able to spam your soso's spells like you were supposed to, but suddenly Buggard got scared of graphics-intensive spells so they put looooong stupid timers on all of them because 'it might cause the game to lag for those whose processor power is capped at 166 MHz'.",
	"Here lies any hardcore Diablo 2 character ever, killed by a boss with nose enchantment. Yeah, I know, it's actually called 'fire enchanted' but the damage done by the boss's death explosion can be wildly off the charts and kill even the most virile barbarian ubertank instantly.",
	"Here lies any hardcore Diablo 2 character ever, killed by a MLEB (multiple shots lightning enchanted boss). That said, I guess with a bunch of lightning absorb they could have survived it, but not everyone is lucky enough to find Thundergod's Vigor.",
	"It's a lot of fun to watch your guy in GTA San Andreas repeatedly get stomped by a lovely girl until your HP bar is empty, particularly if the girl in question is wearing pretty high-heeled shoes!",
	"The sweet prostitutes in GTA3 can repeately knee you in the nuts very painfully, and it's really exciting to let them do that until your guy's HP runs out!",
	"Wow, it's sooooo sexy to watch the scantily clad beach girls in GTA Vice City repeatedly push the player character through the area with their lovely bathing sandals, because they can kick very powerfully and eventually defeat the player!",
	"FUK U AMY why did you make it so that my boomerang suddenly turns from +4 to -4 YOU SUCK",
	"ugghh amy why did the shopkeeper become hostile when I picked up the chest with his goods after picking up 200 individually named rocks from his shop that's like so stupid, you shouldn't fix exploits, you should let players exploit the game to their heart's content",
	"Here lies Mitchell the Unplayable Mage, a mahou shonen cast cause critical wounds twice in rapid succession and the player was playing too fast, otherwise a healing potion would have been quaffed after the first cast.", /* happened to Amy in ToME-SX */
	"Arrgh why did Sophia have to miss with her kick attack, I wanted to have the full length of my leg slit open by her razor-sharp stiletto heels and bleed spectacularly to death but now the wimpy white wolf killed me instead!", /* Amy goofing around in ToME-SX */
	"Mirilith the Queen of Arda, she danced until the end of time with her maid Klara whose black high-heeled sandals are very beautiful <3",
	"If you find a WoW in the first room, you will die horribly, just like you will die horribly if you spend half an hour startscumming before you finally play the game in earnest.",
	"HELP! I've deleted the internet!!!",
	"Well maybe I shouldn't have played on hard difficulty...",
	"Told you not to play on hard difficulty.",
	"Oh man, great game design, having damage traps that deal more damage than an average character has maximum hit points. Press X to not die. The dev of this game certainly doesn't know shit about game balance.",
	"I tell ya flat out, teleporting bosses are broken. I've done enough damage to kill several Diablos in normal difficulty now but due to that broken healing effect I'm still no closer to killing this teleporting stone skin ghoul lord.",
	"Man this dev just doesn't know how to balance the playable roles, I picked one that turned out to suck and when I complained, he said 'well some roles are harder than others, tough luck, I won't change them'. That's bullshit.",
	"This dev made it so that some of the playable roles suck, but he doesn't tell you which ones, meaning you have to figure that out yourself so that you can then steer clear of them. That's some garbage game design and the dev should feel bad.",
	"Man, this playable role is good at only the one thing it's named after but has sucky values in everything else! Why is that role even in the game, is it a trap so that players pick it thinking it could be good, while that other role is just far better in every way?",
	"Man, the priest sucks, he can't kill anything in melee but also doesn't have good spellcasting! What a sucky role, why would anyone ever want to play that?? It's not even a jack of all trades, it's a noob of all trades and you're much better off just playing a warrior or mage!",
	"Goh, the archer is no fun to play, you always have to run after your ammo to pick it back up and the ammo stacks use up valuable slots of your inventory but you cannot even merge them unless their irrelevant bonuses match exactly! It's stupid, you should just play a warrior, then you never have to worry about that shit!",
	"Mages suck, they always have too little hit points! Sure, your spells can obliterate pretty much anything but few spells can hit monsters around a corner and spending even a single turn in the collective lines of sight of a group of monsters can be enough for you to just die instantly!",
	"Who the hell came up with the stormbringer and made it so that his critical hits can do 300 or so stun damage when 100 units of stun are enough to knock you out and cause a technical instadeath?????????????",
	"The devs are stupid, but woe to you if you criticize them. They'll immediately act all offended even though it's obvious how their game sucks and could be made non-sucky by a few simple changes.",
	"Man, I know why the devs inserted the Elbereth mechanic! It's a cheat code because even they got annoyed by the fact that you always get swarmed by 200 orcs and a leocrotta, resulting in an inescapable death unless you spam the E-word to prevent them from attacking!",
	"Poison instadeath sucks, you should at least be able to survive it if your health at the time of being poisoned is higher than, say, 100! Or make it 200 if you must, but in any case it shouldn't be an automatic game over that completely ignores your max HP!",
	"Oh man, the touch of death is a silly game mechanic, it basically means 'have MR or die'. So the best strategy is to linger on levels where high-level spellcasters cannot spawn, killing the random monsters over and over, until you finally obtain MR. That's gameplay straight out of the toilet.",
	"Really feels cheap if any random lieutenant or other intelligent monster can spawn with a wand of death wholly randomly, with no way for you to know they're there, because it means that unless you obtain MR or reflection, you can just randomly die through no fault of your own. Maybe they should add a new game mechanic: if your char has no reflection or MR, there's a 1 in 20000 chance per turn that you simply die, with no explanation.",
	"It's a very good and balanced game mechanic if you can be two-stage-instakilled by a giant eel regardless of how much hit points, constitution or anything you have. Like, why can't you at least attempt to quickly slit the eel in half with your sword or something???",
	"Cockatrices suck, no level 5 monster should be that deadly! That's, like, even more dangerous than a minotaur lol, because the minotaur is basically harmless if you have lots of HP and AC! But fail to notice the delayed stoning effect even once, and you die three turns later regardless of how virile and otherwise unstoppable your char was. That's so stupid.",
	"Like what the hell were they thinking when they gave Pestilence both a sickness attack that kills you in a few turns and a stun attack that makes it wholly impossible to fight back??? Even if you manage to clear the status effects with your unihorn, he'll just apply them again on his next round and effectively lock you up until you run out of HP!",
	"Bah, it sucks that falling into lava without fire resistance is an instadeath and there's no confirmation if that umber hulk had just rounded the corner and decided to look in your direction. That's all I'm saying.",
	"Why on earth can't you swim in the water on the Plane of Water??? Why do you die if you move into the water if you failed to realize that some giant had just thrown a boulder at you that ends up blocking the only adjacent non-water square?",
	"Whose ingenious idea was it to make adventurers on the Astral Plane randomly spawn with Vorpal Blade? Monsters shouldn't be able to use that if you ask me, as it means you can simply die out of nowhere to something that you couldn't have anticipated!",
	"The creator of the samurai quest is the biggest mother fucker, no mandatory boss fight should involve having to get lucky not to get instakilled. And that CHEATER covetous AI means you can't even really use any strategy, since the boss can randomly warp on top of you and bisect you before you get another turn.",
	"Bleh, drawbridges shouldn't be capable of instakilling the player character. They should instead deal, let's say, 200 damage if they manage to catch the player, so that a high-level char at least has a chance of survival.",
	"The three riders have far too high damage outputs!! Like what the hell they max out at 128 damage, that's almost twice as much as a minotaur and since it's just two attacks with high average damage, as opposed to the mino's three attacks with far lower average damage, your AC doesn't even grant you significant damage reduction against them!",
	"Air elementals on the elemental planes suck, they always hit you regardless of your AC and you can't even outrun them since they're too damn fast, plus there's nowhere to hide from them so unless you've stockpiled all the full healing potions that spawned up to that point, you'll just die! How is that good game design???",
	"Archons suck, monster spellcasting shouldn't just be unlimited. Whenever you end a turn in a radius of something like 8 tiles in the vicinity of an archon, there's a high chance that a cloud of monsters appears around you. And the fucking bastards can cast the spell through walls, which is fucking cheating. Spellcasting shouldn't work like that.",
	"Arch-liches are sooooooo imba, every turn you spend near them you'll see a cloud of high-level monsters form around you out of nowhere, they trash your armor and may even instakill you if you engage them in melee without MR, and even if you somehow manage to hit them, they'll just warp to the upstair, heal back to full in a few turns, and warp back to you where they'll continue endlessly summoning shit! It's impossible to defeat them!!",
	"HELP!!! I can't kill Lord Surtur! Every time I hit him a few times he just goes away, and when he appears again he's back to full health! How the hell do you kill him?????",
	"Here lies God-Emperor Sun Hai. Mad with power, he eventually got stopped by the one person who could, which as it happened was the last spirit monk.",
	"Here lies Master Li, self-appointed 'master strategist'. His strategy allowed him to overcome his power-mad brother Sun Hai, only for Li to be corrupted by that same power himself, and despite being given numerous chances to dial it back and become a normal human again, he couldn't let go of his power and eventually the last remaining spirit monk had to kill him.",
	"Here lies Dawn Star. She was so furious when she learned that the silk fox would become the new ruler of the Jade Empire instead of her, which felt like a crushing defeat for her that was ultimately too much to bear.",
	"Here lies Sagacious Zu. He stupidly died to Death's Hand, hoping to crush that bastard under a whole lot of falling rocks, but Death's Hand survived, so Zu's sacrifice was in vain.",
	"Here lies the Water Dragon from Jade Empire. She was bleeding out for 20 years and then finally the hero came to her and mercy killed her.",
	"Here lies one of the holy fountains of Dirge. It was corrupted with human blood. Quite the silly design for a 'holy' fountain, if it can be made to stop functioning that easily.",
	"Here lie the meditation wheels of Dirge. How on earth would anyone design them such that even after being fully repaired, they can only be used one more time before breaking completely??? Wouldn't that mean a shitload of unnecessary work, having to build new wheels completely from scratch all the damn time instead of having ones that simply *work* and *keep working*?",
	"Here lies Lord Lao's furnace. Like so many other machines in Jade Empire, this one was also somehow designed to break after being used too much, probably because the constructors wanted to rake in more $$$'s when they'd get tasked with building a replacement. Except that Lao constructed the furnace himself, for his own use, so he'd be the one who'd have to build a new one once it broke down, which makes no sense whatsoever because he certainly has better things to do than constantly build new furnaces...",
	"Here lies Gao the Lesser. That lowly demon bears much of the responsibility for Two Rivers being wiped out, and therefore deserved his death like the little rat he was.",
	"Here lies Jing Woo. All he wanted to do was to spar with the other martial artists but Sun Hai and his Lotus Assassin bastards decided to rain fiery death on Two Rivers, killing Woo and many other innocents in cold blood.",
	"Here lies Kia Min. She would have fought valiantly against Sun Hai's assassins, but the player decided to give her the wrong healing herb so Kia just didn't feel pain anymore but wasn't really healed, so she was unable to fight properly and quickly got killed.",
	"Here lies a Jade Empire Let's Play thread. It died from the Tien's Landing syndrome, specifically the 'it's no fun to fight your way through 20000 lost spirits every time you take like two steps in that stupid forest' syndrome.",
	"Here lies yet another Jade Empire playthrough. The necropolis was just too damn boring, what the hell were the developers thinking when they made it so that those circular areas would spawn lost spirit bastards over and over and over and over and over again every time you as much as clipped such an area with the tip of your right foot.",
	"Here lies Silk Fox. She was forced to be a princess and later a ruler, even though she just wanted to live the life of a normal woman, and collapsed under the load of her responsibility.",
	"Here lies the kiss scene where Wu the Lotus Blossom and Silk Fox were supposed to, well, kiss each other. Some weird-ass developer censored that scene, causing the screen to fade to black, just because both of those characters were female, while a male protagonist would actually get to see the scene in question. Seems like the devs don't know that homosexuality is a thing that exists and the player would have to be a VERY delicate, brittle thing to have their fefes hurt by seeing it in a video game.",
	"Here lies Ai Ling. She thought that the best way to deal with being denied the man she wanted was to kill both him and his wife, and for such a huge crime, she had to pay the ultimate price.",
	"Here lies Bei from Tien's Landing. He was stalked by that vile Ai Ling person who just couldn't accept that Bei had a wife, and eventually Ling actually went ahead and killed him (and Bei's wife too, for that matter).",
	"Here lies Lan from Tien's Landing. Ai Ling decided to remove her in a vain attempt to get Lan's husband, Bei, for herself, except that he wouldn't want to marry such a serial killer and ended up getting killed by Ling as well.",
	"Here lies Wild Flower. She was possessed by Ya Zhen the evil demon, and while Chai Ka tried to protect her, ultimately Ya Zhen prevailed and tossed the poor girl's dead body in the gutter like a piece of trash. :-(",
	"Here lies Chai Ka. He lost the struggle with Ya Zhen and died, together with Wild Flower even though she was such a cute girl with two bundles. :-(",
	"Here lies Turnkey Shiji. He decided it was a good idea to free someone known as 'Strangler', only to find himself actually getting strangled to death.",
	"Here lies Black Whirlwind. He was teasing Dawn Star and her purple-clothed legs, but then he got to witness firsthand the damage those cute female legs could inflict, and they shattered his spine and collarbone.",
	"Here lies Henpecked Hou. His wife abused him for years and eventually overdid it, breaking the poor sap's neck.",
	"Here lies Gao the Greater. The reign of his band of pirates came to an end when Kang the Mad had his revenge.",
	"Here lies Inquisitor Lim. He died just like all the other Lotus Assassins: by being beheaded. But he brought that fate upon himself by deciding to be an evil bastard.",
	"Here lies Sky. He betrayed the last spirit monk and their party to Death's Hand at the last moment, thereby signing his own death warrant because seriously, did he really think he could side with the evil bastards and escape with his life?! No way. Sky was executed, his body being bisected by the mighty dragon sword.",
	"Here lies Kang the Mad. He kept inventing things that exploded or otherwise malfunctioned, and eventually blew himself to smithereens by accident.",
	"Here lies Ya Zhen. He lost the battle with Chai Ka.",
	"Here lies Purveyor Shouji. What he did was basically to enslave an innocent little girl and threaten that he'd enslave her parents as well if she doesn't do his every bidding, and for that, Shouji was slashed by the last spirit monk's sword so many times that he bled spectacularly to death.",
	"Here lies Kai Lan the Serpent, who was manipulating the Imperial Arena. First, Kai's tail got cut off, and then his head was lopped off by the Black Whirlwind's dual axes.",
	"Here lies Lucky Cho, one of the thugs manipulating fights in the Imperial Arena. Actually, he doesn't lie in this grave, because there's nothing left of him; after the Black Whirlwind killed Cho, Kai Lan then fed Cho's remains to the cannibals.",
	"Here lies Crimson Khana. Her arena fight was rigged, and she got poisoned to death as a result.",
	"Here lies Master Smiling Hawk. He was unfit to lead a school, trying to outdo Master Radiant (who was a much better master than Hawk anyway) by using fraud and deceit, but eventually one of his students decided to cut him down.",
	"Here lies Fading Moon, a crazy arsonist. She was burned in a fire.",
	"Here lies Grand Inquisitor Jia. She paid the price for being the head of the Lotus Assassins, dying a death of a thousand cuts.",
	"Here lies Death's Hand. The last spirit monk managed to put him to rest.",
	"Here lies Master Shin. He was so stupid and got crushed in the golem press, an accident that happens only to very stupid, incompetent people.",
	"Here lies Master Gang. A clay golem dropped on his head because he didn't watch out. Why didn't he look at what was above him, anyway? He could've seen the golem suspended to a crane and avoided walking directly underneath it!",
	"Here lies Lotus Acolyte Go Rin, who was oblivious to everything that happened around her at first but then somehow instantly recognized that a faulty soul was put in the extractor, instantly decided to attack the person who had operated the extractor, and instantly had her head cut off by said person's sword. Too dumb to live.",
	"Here lies Wu the Lotus Blossom, the last spirit monk. She did everything to help Master Li and exact revenge on Li's evil brother Sun Hai, but then Li showed his true colors, revealed that he had been evil all along (although his kinda mean-looking facial expression could have given it away), and somehow managed to stab Wu to death instantly by boring his little finger into Wu's neckline.",
	"Here lies the possibility to resolve quests in Jade Empire in a neutral or at least 'only slightly evil' way. Most of the time, you have a choice only between a good (open palm) way and a monumentally evil (closed fist) way, with the latter usually involving the murder or killing of an innocent NPC, sometimes in spectacularly brutal ways. What's missing is a way to complete quests in ways that still give you closed fist points but without totally fucking over people.",
	"Here lies some unnamed Lotus Assassin. He was beheaded for thinking he could stop a spirit monk.",
	"Here lies Knight Graham of Daventry. He closed King's Quest 1 after realizing he had to input all commands into a text parser, even ones as simple as 'walk south' which really should have been possible to execute by pressing arrow keys.",
	"Here lies Knight Graham of Daventry. He walked into the castle moat and got eaten by a serpent. The moat monsters appreciated his good taste.",
	"Here lies Knight Graham of Daventry. He stood in front of a rock and tried to move it. The moving rock rolled downhill... and right into him. A crushing defeat.",
	"Here lies Knight Graham of Daventry. He got caught by the sneaky little dwarf by surprise, had a plot-critical item stolen, and ended up unable to complete a puzzle an hour later into the game because there was no way to get the item back.",
	"Here lies Knight Graham of Daventry. He got caught by the witch and, since he's a superfailure of a 'knight' who's completely unable to defend himself despite having a fucking dagger, she turned him into a gingerbread man. Or is that a Graham cracker?",
	"Here lies Knight Graham of Daventry. He opened the goat pen and then left the screen, causing the goat to get erased from the game permanently.",
	"Here lies Knight Graham of Daventry. He drowned in the well like a complete dolt even though it took almost 19 seconds to do so and the body of water wasn't very big to begin with. 'Well, well, well! Water you going to do now?', the narrator's comment reads.",
	"Here lies Knight Graham of Daventry. By venturing too close to the dragon's flame, he made an ash out of himself.",
	"Here lies the witch of the dark forest. Graham managed to sneak up on her and pushed her into the pot. Her wild screams were suddenly cut off as she melted away into the oily green slime.",
	"Here lies the troll who tried to guard the 'Troll Toll' bridge in Daventry. Graham led a goat to that bridge, the goat went 'GRRRRRRR!' and simply pushed the troll into the river below, hahaha.",
	"Here lies Knight Graham of Daventry. Of course he didn't guess that the name of the gnome is 'Ifnkovhgroghprm', because by conventional logic 'Rumplestiltskin' should have been correct and the backwards way of spelling that would have been 'Nikstlitselpmur'.",
	"Here lies Knight Graham of Daventry. He was climbing up a beanstalk and then suddenly fell off for no good reason. Looks like he had a bad fall this spring.",
	"Here lies Knight Graham of Daventry. He ran from the giant for a while, but then the giant did a smashing job of defeating him.",
	"Here lies Knight Graham of Daventry. He killed the giant by using the sling, but then the STUPID game was programmed to make the chest unrecoverable just because it dropped behind a tree!! Can you believe it? Who would program a game like that???",
	"Here lies Knight Graham of Daventry. He tried to walk those narrow, winding paths in the cave and then fell off because the game's controls were so stupid and didn't just allow him to walk *staying on the goddamn path* like a normal human.",
	"Here lies Knight Graham of Daventry. He entered the wolf screen and then wasn't at the very edge of the screen when the wolf spawned, which resulted in him getting caught and devoured. Of course Graham's dagger was once again useless because some 'knight' he is.",
	"Here lies Knight Graham of Daventry. Despite being a knight and armed with a dagger, he was killed by a large rat. Yeah, go figure. He can't kill anything, so why is he even called a knight??? And the narrator even went and said 'Rats! This was one shady character you shouldn't have tangled with.'",
	"Here lies Knight Graham of Daventry. He didn't have the four-leaf clover and, like always, couldn't just cut down those wimpy leprechauns with his dagger, so he ended up spending the rest of his days peeling potatoes for the leprechaun king.",
	"Here lies Knight Graham of Daventry. He had gotten the scepter from the leprechaun king and everything... and then couldn't get out of the cave because he had forgotten to get the 'smaller-making mushroom' that would (like in Super Mario Bros: The Lost Levels) make him smaller, small enough to fit through the opening! So Graham got stuck in there for all eternity and starved to death!",
	"Here lies King Edward of Daventry. Just as Knight Graham returned from his quest (given to him by the king, which means it was a King's Quest :D), Edward spontaneously died, but at least he died slowly enough to realize that Graham had been successful.",
	"Here lies King Graham of Daventry. He tried to swim in a poisoned lake in Kolyma, and died of extreme poisoning.",
	"Here lies King Graham of Daventry. He got caught by the witch Hagatha, who then had him for dinner, and I don't mean as a guest.",
	"Here lies King Graham of Daventry. Since Kolyma didn't seem that great, he tried to swim back to Daventry, only to end up so far out to sea, he had no chance of ever getting back. He could no longer swim... and drowned.",
	"Here lies King Graham of Daventry. A wolf in grandma's clothing was in the bed, and the wolf pounced on Graham! My, what big TEETH he has! Good enough to eat him with, my dear.",
	"Here lies King Graham of Daventry. He waved Neptune's trident in front of him, then handed it back to him. With a cold look in his eye, King Neptune took the rusty trident and pointed it at Graham, who felt a powerful surge of electricity and then... nothing. He died at the bottom of the ocean.",
	"Here lies King Graham of Daventry. He gave the trident to Neptune's mermaid and then visited the king of the sea... who instantly killed Graham for no good reason.",
	"Here lies King Graham of Daventry. He ran into an enchanter and because Graham is a super wimp, he couldn't just kill the bastard so he got turned into an animal and lost the game. Pooooooof! The evil enchanter has turned Graham into a cute little froggy. Ribbit, ribbit!",
	"Here lies King Graham of Daventry. He met a poisonous viper and tried to walk past it like the dolt he was, but the snake had struck him dead.",
	"Here lies King Graham of Daventry. It seems he had fallen to his death from the high mountain top.",
	"Here lies King Graham of Daventry. Whoops! King Graham had fallen into the chasm, ending his quest. Because he tried to approach the bridge from the south and the screen was constructed such that he'd appear right in front of the chasm for some weird reason.",
	"Here lies King Graham of Daventry. He has been poisoned by the thorns and could no longer continue the game, complete with the narrator breaking the fourth wall for Graham not eating the sugar cube of immunity to poisonous thorns.",
	"Here lies King Graham of Daventry. The two spooks floated toward Graham and entered his body. In a trance, he walked toward the thorn covered brambles and for some inexplicable reason, felt a sudden tingling sensation meaning the effects of the sugar cube had worn off JUST before he touched the deadly poisoned thorns.",
	"Here lies King Graham of Daventry. He entered the area inside Dracula's castle where the light went out, and tried to walk down the staircase in the dark only to fall to his death.",
	"Here lies King Graham of Daventry. He stupidly woke up Dracula while not wearing the cross, and was caught in Dracula's grasp! Saliva dripped off the end of his sharp fangs. Graham felt them sink into the soft flesh of his neck, and then... nothing.",
	"Here lies Count Dracula. Graham placed the pointed stake on Count Dracula's chest. Quickly, using the mallet, he pounded the stake through the vampire's heart. Dracula disintegrated into a pile of dust.",
	"Here lies King Graham of Daventry. Due to completely idiotic programming, he couldn't even diagonally walk up the part of the staircase that may as well be 2D, as there's still depth where there shouldn't be any. So he'd have had to awkwardly stutter step the whole way instead of simply walking up NORMALLY like a NORMAL HUMAN in REAL LIFE would be PERFECTLY CAPABLE of doing. Instead, Graham fell to his death on the hard stone floor.",
	"Here lies King Graham of Daventry. Oh, dear! What a shame. The rickety old bridge collapsed as he attempted crossing it. He has fallen to his death. Should have played such that he'd not need to cross Kolyma's bridge an eighth time!",
	"Here lies King Graham of Daventry. He returned from Kolyma without a bride, unfortunately, and so ultimately his kingdom fell into ruins.",
	"Here lies King Graham of Daventry. He lost his footing on the narrow stairway of that tower because the STUPID game programming would require completely arcane sequences of inputs just to walk up a spiral staircase, something that even a five-year-old could do in real life without needing to be babysitted.",
	"Here lies King Graham of Daventry. CRUNCH! SNARL! CHOMP! Well, he made a nice meal for the hungry lion! SMACK! SLURP!",
	"Here lies King Graham of Daventry. He tried to use the dagger on that witch Hagatha but inexplicably it didn't work. That's so dumb, a dagger to the neck should be enough to kill anyone.",
	"Here lies King Graham of Daventry. Despite having a dagger and seeing the wolf in grandma's clothing in the bed, the game just arbitrarily would not let him kill the wolf, which makes absolutely no fucking sense.",
	"Here lies King Graham of Daventry. For some fucking reason, whenever he tried to kill the dwarf by using his sword, the game just said the dwarf would be too fast on his feet, yet the dwarf would be able to catch Graham and steal all of his possessions.",
	"Here lies King Graham of Daventry. He tried to kill the monk but that just caused his game to be ended because the developers think someone who would kill a man in the cloth wouldn't deserve to play the game. Do they even know that the Christians killed thousands (well, probably more) witches and other 'heretics' (or people whom they labeled heretics) in cold blood, which would be enough to fully justify killing any Christian monk in a video game on sight?",
	"Here lies King Graham of Daventry. For some reason, the evil enchanter is magic (not even magical, i.e. a magically-enhanced person, nope, he actually IS magic itself, i.e. the very definition of all that is magical is in the enchanter) and that made him immune to Graham's sword, while Graham was not immune to the enchanter's, well, magic.",
	"Here lies King Graham of Daventry. He was caught by Count Dracula while wearing the silver cross, which caused Dracula to turn into a bat and fly away with the silver key. Of course, without the key, Graham then couldn't complete the game, because King's Quest 2 was a Sierra game after all, complete with dead man walking scenarios and everything.",
	"Here lies King Graham of Daventry. The water in the final area of Kolyma was too turbulent to swim in, and Graham had just drowned despite there being no visible indication that the water had any sort of turbulence whatsoever.",
	"Here lies a normal staircase in Manannan's house. Instead, there are stairs that were designed by an asshole, which are angled in such a way that the player has to keep adjusting Gwydion up and down as he travels which makes NO FUCKING SENSE because in real life, you could just walk up the staircase using a straight path.",
	"Here lies Gwydion. He got caught by the shitty CoPy protection of King's Quest 3 where spelling the magical incantations even slightly wrong would somehow instantly kill him, instead of doing the logical thing and just making the spell fail.",
	"Here lies Gwydion. He tried to climb the stair back out of Manannan's lab only to have the fucking cat occupy the WHOLE WIDTH of the staircase somehow, which should be physically impossible, and as a result Gwydion tripped and fell to his death.",
	"Here lies Gwydion. He was caught by Manannan holding an inventory full of asterisk items, and got zapped out of existence as a result.",
	"Here lies Gwydion. He fell into the canyon because Sierra was so fond of making stupid winding paths where you have to babysit the walking of your character (including sections that require your movements to be pixel-perfect even though the same path would be easy to traverse in real life).",
	"Here lies Gwydion. He entered the screen where Medusa spawns and was facing to the west, and Gwydion's eyes fell upon a face so hideous, so gruesome, that the horror of it is beyond description. Suddenly, his body felt cold and stiffened hard as a rock.",
	"Here lies Gwydion. He couldn't take the relentless sun anymore, and collapsed from heat stroke. Miserably, he died in the dreadful desert. But it was really his own fault for walking over a dozen screens out into the endless desert.",
	"Here lies Gwydion. He fell from the tree fort because the weird-ass game would cause him to move just one pixel too far to the east, rather than using the rope ladder.",
	"Here lies Gwydion. After waking the bandit in his treehouse, the heartless thug dragged Gwydion out the door and pushed him off the porch.",
	"Here lies Gwydion. He experimented with a huge spiderweb, only to be held in place while a giant spider dropped down from her lair in the tree above.",
	"Here lies Gwydion. He ate the cat hair and thereby somehow grew a cat tail, with the narrator saying 'That was a-mew-sing Gwydion'.",
	"Here lies Gwydion. He ate some saffron, which inexplicably made his eyes huge and alien-like.",
	"Here lies Gwydion. He ate an eagle's feather, and thereby grew some huge ears which isn't exactly logical but what did you expect from King's Quest 3.",
	"Here lies Gwydion. He was caught by Manannan while holding a bottle of salt, which that old shitbag wizard somehow considered an offense punishable by death. Even though salt is really a mundane food additive likely to be found in any normal kitchen.",
	"Here lies Gwydion. He ate a bowl of nightshade and fell into a deep, potentially deadly sleep. Night-night, Gwydion.",
	"Here lies Gwydion. He tried to prepare the sleep spell and not only would the text parser (which was likely programmed by idiots) not recognize 'sleeping powder' instead of 'sleep powder', but even worse, King's Quest 3's CoPy protection even instantly killed him because it thought he had pirated the game!",
	"Here lies Gwydion. OH, OH!! That darn cat really did it to Gwydion this time! Tripping over the cat, he fell to his death. And the cat then laughed 'You look mighty funny falling down those stairs, Gwydion.'",
	"Here lies Gwydion. He somehow thought it was a good idea to eat the magically prepared cookie, and permanently turned into a cat.",
	"Here lies Gwydion. He gave the porridge to Manannan without spiking it first, resulting in a long dead man walking scenario because there was no longer a way to get rid of that wizard shitbag anymore and eventually the food ran out.",
	"Here lies Gwydion. He tried to give the cookie to Manannan, but of course that old asshole realized that the cookie was spiked with magic and therefore not only refused to eat it, but zapped Gwydion into nonexistence immediately.",
	"Here lies Manannan, the evil wizard who terrorized Llewdor. His slave Gwydion spiked the porridge and permanently turned that shitbag wizard into a cat.",
	"Here lies the big spider of Llewdor. Gwydion transformed into an eagle, grabbed the spider and threw it into the open sea.",
	"Here lies Gwydion. He ate some salt, and mysteriously that just caused him to poof out of existence forever. That's King's Quest 3 moon logic for you right there.",
	"Here lies Gwydion. He drank some ocean water, and for some god-forsaken reason that no one can explain, doing so immediately created a rain cloud above him even though Gwydion was standing indoors, and he was struck by lightning because there was no silver lining in this cloud.",
	"Here lies Gwydion. He ate a cactus and was just gone. Yeah, now you go explain to me how that even works.",
	"Here lies Gwydion. He was stranded in Llewdor forever due to having spent more than four gold pieces in the shops, so the pirates wouldn't take him with them to another island.",
	"Here lies Gwydion. He was stranded in Llewdor since the pirates sailed away without him due to Gwydion dilly-dallying on the island for too long.",
	"Here lies Gwydion. He was all set to leave Llewdor with the pirates, but had forgotten to finish off that old git Manannan, who then appeared to kill Gwydion just as he was about to set foot on the ship.",
	"Here lies Gwydion. He got sooooooo boooooooored on that pirate ship, waiting around for over ten minutes while nothing happened, that he decided to cast 'brew of storms churn it up' and the ship sank. Looks like Gwydion's spell had backfired.",
	"Here lies Gwydion. He teleported up the ship's mast but then got noticed by a burly pirate from inside the crow's nest. Before Gwydion could even react, the pirate yelled 'Yer dead meat, bucko!' The pirate then slashed at Gwydion with his sword. He missed, but the prince lost his balance. Down he tumbled... to his untimely death.",
	"Here lies Gwydion. He had seen everything on the ship and got really tired of waiting, so he made a break for it, only to realize that once he's in the water, there's no turning back. Seven screens later, Gwydion had reached the limits of his strength. He panicked and started to struggle hopelessly.",
	"Here lies Gwydion. He had waited for over ten minutes to have the ship finally reach land, only to fall in the open hatch into the cargo area. Falling from a screen up in King's Quest 3 is a death sentence, and Gwydion hadn't saved since before starting the waiting game. Suffice it to say, he was not happy at having to wait twice.",
	"Here lies Gwydion. Gosh! He barely made a mouthful for that hungry shark! And all just because he didn't get off the screen immediately when the shark spawned, since it's faster than Gwydion.",
	"Here lies Gwydion. He encountered the abominable snowman in the mountains. The terrible, hairy creature grabbed Gwydion with bone-crushing force! He resisted, but it was no use. The creature carried him away to his cave, where Gwydion met an early (and unspeakable) demise.",
	"Here lies Gwydion. He had passed the hardest part of crossing the mountains that no one had ever passed and lived, only to suddenly fall down for no reason when climbing a much smaller, easier segment of rock.",
	"Here lies Gwydion. He went into that cave with a staircase that would be PERFECTLY STRAIGHT in real life, but the screen orientation makes it appear angled and due to the way King's Quest 3 is programmed, you have to babysit Gwydion's walking, waggling back and forth or you will fall off. Even if you do successfully make it up, the walking animation will look completely gay and everything, no sane human being would walk up a set of stairs like that.",
	"Here lies Gwydion. He walked on the clouds. This mystical cloud land may never cease to amaze him, but... it had ceased to support him. Meaning Gwydion had fallen to his death.",
	"Here lies Gwydion. Upon exploring Cloudland, a huge, fire-breathing dragon was there and it has SEEN Alexander (wtf I thought his name was Gwydion)! Hypnotized by its baleful glare, Gwydion stood frozen in his tracks. What a way to go; ending up as a dragon's barbeque!",
	"Here lies the dragon of Cloudland. Alexander, err Gwydion cast 'brew of storms churn it up'. Bolts of lightning struck the huge dragon. He howled in pain and fell to the ground... dead.",
	"Here lies Gwydion, err Prince Alexander I guess. He tried to take a shortcut, but Princess 'Swoon Bundle' Rosella didn't allow that, so she just kicked Alexander with her cute pointy little-girl 'princess' shoes, resulting in Alexander falling off the staircase and to his death even though he had already technically won the game.",
	"Here lies Princess Rosella. She couldn't handle a 10 foot drop even though the ground below was soft, sandy beach.",
	"Here lies Princess Rosella. She ran around Tamir for more than 24 hours, so the fairy Genesta ran out of energy and died, resulting in a nonstandard game over.",
	"Here lies Princess Rosella. That rickety old shovel (which is a picture of hate and spite for King's Quest 4) broke after she had dug up five graves, but she had failed to realize that she had to dig up five SPECIFIC graves, and without the shovel she found herself unable to dig up some plot-critical grave so her game was no longer winnable.",
	"Here lies Princess Rosella. She's really gotten herself out of a limb this time, being snatched by a group of humanlike trees who presumably proceeded to eat her.",
	"Here lies Princess Rosella. The ogre spawned right on top of her and dragged her off to her death instantly before she even had a chance to react, because King's Quest 4 is a very well-designed game. 'Sorry, you died because the RNG said so. Get fucked.'",
	"Here lies Princess Rosella. That ocean screen randomly had a shark on it, and, well, she made a tasty morsel for that hungry shark!",
	"Here lies Princess Rosella. She decided to take a swim in the ocean to wake herself up. It didn't work. Too tired to swim anymore, Rosella's body gave out as she joined Davey Jones in his locker.",
	"Here lies Princess Rosella. She got stuck in the whale's stomach and, after four and a half minutes of not being able to figure out how to escape, she finally died. We don't know whether it was because she kept falling down the whale's tongue, or she just didn't have the peacock feather and therefore couldn't tickle the whale's uvula to make him sneeze her out.",
	"Here lies Princess Rosella. She left that lonely island without finding the invisible golden bridle and, as a result, ended up unable to complete a puzzle much later in the game.",
	"Here lies Princess Rosella. She made the mistake to knock at the ogre's door, and with a jerk, the door opened to reveal... oh no!... the ogress! Poor Rosella! It looks like she joined the deer in the stew pot tonight!",
	"Here lies Princess Rosella. Inside the ogre's house, a VERY UNFRIENDLY bulldog rushed toward her. She wasn't fast enough, exacerbated by the fact that the AGI version of King's Quest 4 doesn't pause while you enter the lengthy 'throw bone to dog' command. She was nothing but a bite-sized morsel for that big dog!",
	"Here lies Princess Rosella. She decided to wake the ogre to say thanks for the hospitality. That was foolish. The vicious ogre grabbed Rosella and dragged her toward the kitchen. As to what happened in the kitchen, let's just say she was cordially invited to BE dinner.",
	"Here lies Princess Rosella. Even though she gave the eye back to the three blind witches, they weren't grateful, instead they grabbed Rosella and threw her into the boiling pot.",
	"Here lies Princess Rosella. The troll that lived in the caves came out to play at random, and she died with no way of escaping.",
	"Here lies Princess Rosella. She even tried to transition to a different screen in the cave to escape from the troll, but the cheater of a troll just immediately spawns right behind her upon going to a different screen, causing an inescapable game over.",
	"Here lies Princess Rosella. Oh no! Rosella has been caught by the vicious troll! Fate be what it may, she was dragged off to meet it. But really, it was just the RNG deciding that her game would end in those caves through no fault of her own.",
	"Here lies Princess Rosella. On the final screen in the cave, she fell into a chasm that she had no way of knowing was there.",
	"Here lies Princess Rosella. She had to go back through the caves but had forgotten to retrieve the board after the last time she used it, and was therefore no longer able to cross the chasm.",
	"Here lies Princess Rosella. Don't you know that some swamps are quagmires that will suck you under? As she took her last breath, Rosella contemplated this final information.",
	"Here lies Princess Rosella. She turned into a frog to be able to cross the death bog, when suddenly she uhh... suffered from some sort of muscle cramps out of nowhere and died?! The narrator just said 'Oh boy! I sure love frog legs!', which doesn't explain at all why a frog would die from trying to cross a swamp.",
	"Here lies Princess Rosella. She got too close to a cobra in the swamp and died of poisoning.",
	"Here lies Princess Rosella. She ran around the graveyard at night without the mystic scarab that protects her from the undead, and got killed by a zombie.",
	"Here lies Princess Rosella. She wanted to just go down the ladder in that manor, but the game's interface caused her to fall down without holding on to the ladder instead and she fell to her death.",
	"Here lies Princess Rosella. In fact, she died and she died over and over and over again, because of the goddamn programmers making it so that at certain points of traversing spiral staircases, the up and down controls are REVERSED for no good reason other than 'lel let's make players die more often just because we can, disregarding the fact that in real life a woman would be perfectly capable of walking up or down a spiral staircase with zero risk of falling off unless she's completely drunk'.",
	"Here lies Princess Rosella. She used cheats to get into the tomb without the scarab, and got killed by the mummy inside.",
	"Here lies Princess Rosella. She opened Pandora's Box. That was very foolish. She's loosed the terrible demons... and killed herself in the process.",
	"Here lies Princess Rosella. She had to walk down the UAEAEAEAEAEAEAEH spiral staircase in Lolotte's castle, complete with the dumbfuckery of the game developers once again reversing her fucking controls, and splatted on the floor right next to an incubus. Shame, she should've fallen ONTO the dumb incubus, then that bastard's skull would at least have been split open by Rosella's cute pointy little-girl 'princess' shoes.",
	"Here lies Princess Rosella. She stepped on an incubus's toe, causing that bastard to notice her, take away all of her belongings, and destined her to be Mrs. Edgar with that evil witch Lolotte as her mother-in-law. Ugggggggghhhhhhh.",
	"Here lies Princess Rosella. She stupidly woke Lolotte up! 'VIPER!', the old crone screamed as she quickly sat up, pointed a gnarled finger at Rosella, and zapped her into oblivion!",
	"Here lies Lolotte the evil witch. Rosella shot her with Cupid's bow, whose arrows are harmless, and only filled with love. But Lolotte was so evil, that when pure love entered her body, it acted like a poison.",
	"Here lies Princess Rosella. She had fired all the cupid arrows away, and therefore found herself unable to kill Lolotte. This, obviously, resulted in her being unable to continue her adventure.",
	"Here lies Princess Rosella. The walkable path from Lolotte's castle is extremely narrow, so it's necessary to babysit Rosella's walking animation if you don't want to fall off repeatedly, which is really silly because why is there a fucking winding path in a game that only allows you to use cardinal directions for walking??? That shouldn't even be legal to program!",
	"Here lies Princess Rosella. She had made it past the chore of walking the narrow path from Lolotte's castle to freedom... or so she thought, only to fall down a mini-size cliff on the next screen and since she didn't save upon reaching that screen, she then fell off the narrow passage *six more times*.",
	"Here lies Princess Rosella. She really should have won the game already, having dealt with Lolotte and retrieved the magic fruit and everything... but she hadn't saved her game. The shark patrolling the bay knew that she didn't, and showed up to teach her a painful lesson.",
	"Here lies King Graham. Rosella has had a pretty shitty day, tied to a stake as a sacrifice for a three-headed dragon, her dad had sudden onset of heart failure, and then she got forced into a pretty terrifying adventure... all without getting anything to eat except a bit of soup with the dwarves. She thought it wouldn't hurt to take one little bite out of the fruit... but before she knew it, she had eaten the whole thing. Forgetting that this fruit was the only way to save Graham from death.",
	"Here lies Graham the Idiot. He got eaten by a pooooooooooisonous snake. 'That wasn't wise, Graham. He who speaks with forked tongues should never be trusted.', is what the narrator had to say.",
	"Here lie the ears of a King's Quest 5 player. The 'kings_quest_5_town_theme.mid' music was getting ever louder and louder the longer the player stayed in the town, eventually causing the player to go deaf. Arrrrrgh.",
	"Here lie the ears of a King's Quest 5 player. Killed by Cedric the ooooooooooooooooooooowl who draws oooooooooooouuuuuuuuuuuut soooooooooome vooooooooooooowels fooooooooooooooor nooooooooooo reasoooooooooon at all.",
	"Here lies Graham the Idiot. He ran into a bee swarm and got stung to death. BEE seeing you.",
	"Here lies Graham the Idiot. Too bad. Graham's swimming skills were no match for the mighty river. Because dude, don't you know that every video game character ever just fucking can't swim no matter what?",
	"Here lies Graham the Idiot. He entered the Dark Forest and immediately got turned into a frog by the witch. Because that old witch caught Graham toad-ally off guard.",
	"Here lies Graham the Idiot. Struggle as he might, Graham cannot escape his bonds. And he's a monumental idiot for letting the innkeeper and his goons 'rub him out' in the first place; some knight or king he is, being completely unable to fight back!",
	"Here lies Graham the Idiot. Tisk tisk. Graham should know better than to feed the bears. Although the bear just punched Graham once and then went back to knocking on the tree, so Graham shouldn't actually have died, but being a frail adventure game character, of course he did.",
	"Here lies Graham the Idiot. Too bad. Even the sting of such a small creature as a scorpion can prove deadly, and of course the scorpion kills you the instant you enter the screen so you can't even run back to a safe screen. Joy!",
	"Here lies Graham the Idiot. Too late! Graham collapsed and died of extreme thirst in the hot desert sun. If only he could have found an oasis, which gives the ultra-annoying line 'Ahh! Live-giving water! Nectar of the gods! Graham can now feel strength and renewal flowing through him.' every. single. time. he drinks from one.",
	"Here lies Graham the Idiot. He stood in front of the temple, and some guys on horses came galopping and just ran him over. They didn't even stop to unmount and cut him down with their swords, they just bumped into him with their horses and Graham died instantly because 'reasons'.",
	"Here lies Graham the Idiot. He entered the bandit camp and wanted to peek into one of the tents, only to have some bandit come out and cut Graham down instantly. Because Graham is an absolute idiot who doesn't even have a weapon or anything, and therefore cannot defend himself from anything whatsoever.",
	"Here lies Graham the Idiot. He woke up a sleeping bandit, who proceeded to immediately rise and stab Graham with a dagger. Of course Graham was once again completely unable to do anything about that - not only does he not have a weapon to defend himself with, but he was even too stupid to run away like a sane person!",
	"Here lies Graham the Idiot. He tried to take some of the mountain of gold inside the temple, which somehow caused the temple door to instantly re-materialize, and... well, cheer up. At least he could practice his game of tiddlywinks! For all eternity, in fact, since he was now trapped inside with no way out.",
	"Here lies Graham the Idiot. He took too long to pick up the gold coin and brass bottle in the temple, causing the stupid door to close and of course it couldn't be opened again from the inside because some genius designed that place.",
	"Here lies Graham the Idiot. He opened the bottle, but the djinni inside caused Graham to be the next one to spend five hundred years in that bottle.",
	"Here lies Graham the Idiot. He bought something other than fortune telling with his gold coin, even though those other items could have been bought with a silver coin or traded for other items, and thereby ended up not having the gold coin when he really needed it, resulting in an unwinnable game.",
	"Here lies Graham the Idiot. He didn't realize the single pixel inside the temple (which is a gold coin), and left without picking it up, making it impossible to retrieve the coin. As a result, Graham then did everything else he could do in Serenia, which took him about an hour, and then he finally realized that he had gotten stuck with no way to continue.",
	"Here lies Graham the Idiot. He entered the Dark Forest without carrying the honeycomb, and in order to prevent him from simply being unable to leave, the maw plant came to life and ate him. This death only happens if you are lacking an item necessary for being able to leave.",
	"Here lies Graham the Idiot. He entered the Dark Forest without carrying the bottle, and in order to prevent him from simply being unable to leave, a giant spider dropped on his head completely randomly and ate him. This death only happens if you are lacking an item necessary for being able to leave.",
	"Here lies the witch of the Dark Forest. Graham gave her a bottle and, due to metagame knowledge, he knew that the bottle contained a djinni who would imprison the user inside, but the witch didn't, so she had to spend the next 500 years inside the bottle.",
	"Here lies Graham the Idiot. He threw an emerald on the ground in the Dark Forest. A little elf came out of the woods and picked it up, then ran away again. So Graham threw another emerald, and the elf came out again and picked it up. After repeating it for the third time, Graham was out of emeralds and could no longer beat the game because there was no longer a way of getting out of the forest.",
	"Here lies Graham the Idiot. He failed to save the rat from a cat, resulting in his game being no longer winnable because he'd not be able to escape from the inn. That he had all of TWO seconds to 1. realize he had to save the rat and 2. somehow guess that throwing some old boot at the cat was the correct solution, certainly didn't help.",
	"Here lies the rat in King's Quest 5. Even though the game clearly did broadcast that the rat is important, the player blithely ignored the message and went something like 'ah a cat is hunting vermin, good, that's the normal course of things in nature', causing the rat to get eaten, but in the process the game was also rendered unwinnable.",
	"Here lies Graham the Idiot. He had even saved the rat and therefore escaped his bonds in that horrible inn of death, but then a rusty padlock on the door kept Graham locked in. Why on earth they had the lock on the inside, we'll never know, but nonetheless Graham didn't have a hammer so he had to redo the last 15 minutes of gameplay because he had saved over his savegame. Like a dingus.",
	"Here lies Graham the Idiot. He returned to the inn of death after having escaped, and was STILL unable to defend himself even though he had a fucking HAMMER that he could realistically use to pound on his foes, and so the innkeeper's goons rubbed Graham out for good. What an idiot.",
	"Here lies Graham the Idiot. He clicked on the wrong spot in the snowcap area, and fell from the cliffs to his death.",
	"Here lies Graham the Idiot. He had forgotten to get the rope from the inn, because of course he didn't think the rope would become an item that he could pick up, and so one hour later he found himself unable to climb in the snowcap area. He could only fall to his death at that point.",
	"Here lies Graham the Idiot. He had forgotten to get the cloak in Serenia, and froze to death in the snowcap mountains.",
	"Here lies Graham the Idiot. He had thrown his rope to the tree root and then saved his game. Since the very smart developers didn't create a way to untie the rope, the game was no longer winnable because the root would always break off before Graham reached the top, and it doesn't help at all that the small rock outcropping to the east of the root (which really looks like it's just a background graphic), which is the actual spot Graham needs to throw the rope to, is very easy to overlook.",
	"Here lies Graham the Idiot. He had to traverse the floating rocks in the snowy mountains and of course there was no indication which ones were safe and which ones weren't, so all he could do was to engage in trial-and-error gameplay. Obviously, that last rock he tried to step on was the 'error' part of that strategy.",
	"Here lies Graham the Idiot. He had somehow managed to get to the snowcap area with no food in his inventory... and starved to death. Now he had to go back to a much earlier savegame file where he was still in Serenia, and get some damn food.",
	"Here lies Graham the Idiot. He ate the custard pie in the snowcap mountains, and therefore ended up being unable to get past the yeti later. Why he didn't eat the lamb, we have no idea.",
	"Here lies Graham the Idiot. He had forgotten to get the lamb meat in that inn, resulting in an unwinnable game because now he could neither feed the eagle nor get rid of the yeti. And both of those dead man walking scenarios terminated about an hour past the point where Graham had fucked up.",
	"Here lies Graham the Idiot. He tried to navigate the snowy downhill path with his 'batcape', but no matter what he tried to do, the last step would always result in him falling to his death when the narrow rock path broke down underneath him. How on earth would anyone guess that it's possible to bypass that deathtrap by using a sled to fly over the big chasm???",
	"Here lies Graham the Idiot. He didn't have a sled because he had forgotten to get it from that annoying pseudo-german-speaking toy shop owner, and therefore found himself completely unable to pass the downhill path in the snowcap mountains with less than zero indication as to what he was supposed to do.",
	"Here lies Graham the Idiot. He encountered the easter egg in the snowcap area by using the batcape on the broken sled, which somehow resulted in him getting a country junk boat (which is a boat that can only travel on dry land), but after driving and floating around for half a minute, a dragon appeared out of nowhere and ate Graham, saying 'For Lisa from 0.', whatever the hell that means.",
	"Here lies Graham the Idiot. He fed the custard pie to the eagle, even though he still had a leg of lamb left. Well, at least the game terminated shortly after when Graham found himself unable to get past the yeti.",
	"Here lies Graham the Idiot. He had failed to realize that the weeping willow had left behind her harp, and lost the game an hour later when he found himself unable to prevent the regicidal queen Icebella from killing him.",
	"Here lies Graham the Idiot. He had five seconds to convice this woman (who called herself 'queen' Icebella) to not kill him, which was far too little. Who on earth would be able to think THAT fast? So Icebella's wolves ate Graham. It's a dog eat dog world out there...",
	"Here lies Graham the Idiot. He encountered a yeti in the snowy mountains, and didn't realize that the one item to get past the yeti is the custard pie. What an abominable situation Graham's found himself in.",
	"Here lies the yeti that had been terrorizing queen Icebella's territory. Graham threw a custard pie in the yeti's face, who got blinded and ended up tumbling off the cliffs to his death.",
	"Here lies Graham the Idiot. He hadn't saved the eagle, so once he ended up in the roc's nest, there was no escape. Graham got eaten by the baby roc.",
	"Here lies Graham the Idiot. The boat he tried to ride had a hole in it, which the game didn't communicate at all when Graham set sail, and which wouldn't even get displayed upon inspecting the fucking boat, and... well, guess what happened. Of course the boat would sink in the middle of the ocean, too far from shore for Graham to swim to safety, or maybe he couldn't swim at all.",
	"Here lies Graham the Idiot. His boat encountered a nasty sea creature and was swallowed whole. Of course, King's Quest 5 being what it is, there's not even a second where you're given the chance to sail off the 'here be dragons' part of the map, nope, once you enter one of the death screens, that's it, goodbye, thank you for playing King's Quest 5.",
	"Here lies Graham the Idiot. Hate to harp on the subject, but... Graham was no match for the vicious harpies! And nope, he was once again unable to defend himself, even though his opponents were women in this case. If only he had played his harp, which he must have had at that point since without it he'd have died to Icebella earlier.",
	"Here lies Cedric the ultra-annoying owl. He had been fatally wounded by the harpies and Graham, being sick of that stupid owl's way of speaking, left him there to die.",
	"Here lies Graham the Idiot. He returned to the harpy island since he had forgotten some plot-critical item there, but the harpies instantly grabbed and ate him because he's such a super wimp who can't even defeat a *woman*. Why didn't he use the hammer to pound that bitch of a harpy?!",
	"Here lies Graham the Idiot. He had forgotten to get the pixel-sized conch shell from the harpy island, and so the old man couldn't hear a thing Graham said but going back to the island was not an option either. Graham's game was therefore unwinnable.",
	"Here lies Graham the Idiot. He hadn't noticed the one specific crystal in the cave that was possible to break off, and therefore died to the serpent guardians of Mordack's castle much later into his adventure. Too bad. It looks like the EYES have it.",
	"Here lie the serpent guardians of Mordack's castle. Too dumb to live, they fired their beams at Graham even though he was holding a reflective crystal in plain sight, and they shot their own heads out. Well, I guess some creatures really do have an IQ that's slightly below that of a rock...",
	"Here lies Graham the Idiot. He played the floppy version of King's Quest 5 and failed the fomo CoPy protection checks too often, and therefore his adventure was terminated.",
	"Here lies Graham the Idiot. He had forgotten the iron bar from the beach, so when he tried to open the grate leading to Mordack's castle, he couldn't. Eventually, some unnamed henchman spawned, Graham once again forgot that a sane man would be able to defend himself, and that was the end.",
	"Here lies Graham the Idiot. He shouldn't play around with Dink, even though that beast actually looks nice and friendly. Because this is King's Quest 5, and walking up to anyone or anything will result in a game over for 'reasons'.",
	"Here lies Graham the Idiot. He had forgotten to grab Dink's hairpin and therefore found himself completely unable to open the door, and going back to where Dink had been didn't help since the game had stupidly erased the hairpin off the face of the world because 'realism'.",
	"Here lies Graham the Idiot. He hadn't grabbed the pixel-sized golden locket in the roc's nest, and as a result that bitch Cassima refused to give him the time of the day even though she was a plot-critical NPC. That harlot.",
	"Here lies Graham the Idiot. He played the pipe organ in Mordack's castle. That thing is a giant trap, since all it ever does is summon Mordack the Evil Bastard, who then proceeded to zap Graham to ashes.",
	"Here lies Graham the Idiot. He hadn't realized that he had to grab a pixel-sized fishing pole on that island, and therefore ended up unable to retrieve the cheese out of the hole in that prison cell. And without that cheese, he was unable to power up the machine that would give him a wand for the final boss fight.",
	"Here lies Graham the Idiot. He got abducted by the blue alien a second time, and somehow within that short timeframe of getting out of the jail and being thrown in again, Mordack and his goons had replaced and mortared the loose stone. After observing no discernible escape route from the small cell, Graham sunk to the floor in despair, knowing he will never see the light of day again.",
	"Here lies Graham the Idiot. He stupidly allowed the complete and utter bastard Manannan (in cat form) to find him, and instead of instantly strangling that cat fucker to death, allowed the cat to run off and summon Mordack. Who then appeared and instakilled Graham.",
	"Here lies Graham the Idiot. He hadn't noticed the old, stinking fish at the entrance of Mordack's castle and therefore found himself unable to do anything about Manannan the cat fucker. So Graham's game was over because he failed to pick up an item an hour or so ago.",
	"Here lies Graham the Idiot. He had forgotten to get the bag of peas even though it was meant to be used for capturing Manannan the fucker of a cat, and therefore the fucker of a cat would always call Mordack who then proceeded to blast Graham to hell.",
	"Here lies Graham the Idiot. He just couldn't figure out how to power up Mordack's machine, because no sane person would ever guess that you have to put Mordack's wand on one platter, your own wand on the other, and then fuel the machine with cheese. *No one in the world* would guess that, not even in a hundred years.",
	"Here lies Graham the Idiot. He hadn't saved Cedric the dumbass owl with the horrible voice, and therefore Cedric couldn't catch the magic bullet fired by Mordack. Why Graham didn't simply BREAK Mordack's stupid wand, we have no idea.",
	"Here lies Graham the Idiot. He had forgotten to read Mordack's spell tome, so when Mordack transformed into a flying drake, Graham had no way of defending himself and got killed.",
	"Here lies Graham the Idiot. He had transformed into a tiger to battle Mordack's flying drake form, but Mordack then turned into a dragon and roasted Graham.",
	"Here lies Graham the Idiot. He had transformed into a rabbit to evade Mordack's dragon form's fireblasts, but Mordack then turned into a snake (who knows only one single insult, namely 'swine') and ate Graham.",
	"Here lies Graham the Idiot. He had transformed into a mongoose to scare Mordack's snake form, but Mordack then turned into a fire and burned Graham to death.",
	"Here lies Mordack the Bastard Wizard. He had transformed into a fire in an attempt to burn Graham alive, but Graham summoned a rain cloud and deleted Mordack from existence. And with that, the kingdom was finally saved... well, or it would have been, but there's a King's Quest 6 where the story continues with even more badguys whom Graham and Alexander have to defeat.",
	"I believe the term is altar-camp? I really don't do it at all and I don't know where the FUCK you got that from, and if I did I wouldn't care. I get an ascension every few months... no need to be suspecting me of things.", /* based on something rebatela said */
	"NOOOOOOO! Amy fixed the strategy of casting create monster 200 times while next to an altar! How am I going to farm those prayer boosts now? SLEX is unplayable and the opposite of fun! All the good strategies from vanilla don't work for bullshit reasons!",
	"Here lies Fingolfin. Melkor crushed that evil bastard elf's neck as punishment for dealing eight never-healing wounds to him. Why Fingolfin had to do that in the first place instead of simply letting Melkor mind his own business, we will never know.",
	"Here lies the version of Arda that Eru, Melkor's daddy, had envisioned. Melkor rightfully played some different music of the Valar, thereby creating a richer, more diverse Arda, but of course Eru and Tulkas had to be so evil and punish Melkor for that. At least they weren't able to undo Melkor's music, though.",
	"Here lies Gollum, or rather, the ashes that remain of him. He jumped into the lava, but at least he also destroyed the One Ring in the process.",
	"Here lies the One Ring. Gollum threw it into the lava. Due to his inadvertence, he also fell into the lava himself and got consumed as well.",
	"Here lies Elendil. He had his head smashed in by Sauron's mace.",
	"Here lies Isildur. He was such a complete failure who couldn't destroy the One Ring, and ended up catching orcish arrows in the back when he tried to run away, the coward.",
	"Here lies Boromir. Lurtz pumped him full of arrows while shouting 'THEY MUST ALL DIE!'.",
	"Here lies Hama, the annoying doorwarden of the golden hall who always requests everyone to put their weapons away. Eventually, a warg came along, and Hama still somehow thought it was a good idea to tell the warg to put its weapons away, except the warg's weapons are actually built into its body (teeth and claws) and therefore cannot be put away, and the warg tore Hama apart.",
	"Here lies Deagol. He got strangled by his brother Smeagol (who is actually Gollum).",
	"Here lies the Witch-King of Angmar. He got stabbed in the face by Eowyn. Which actually shouldn't have killed him, because Sauron was still alive at that point and as long as Sauron is alive, he can simply resurrect the Nazgul at will.",
	"Here lies Sauron, the Lord of the Rings. When the One Ring was destroyed, Sauron's fortress inexplicably exploded, and Sauron was disintegrated in the process.",
	"Here lies Khamul the Black Easterling. Once Sauron was dead, Khamul no longer had protection from death and so the good guys were able to finally kill him.",
	"Here lies Dwar, Dog Lord of Waw. With Sauron gone, Dwar's ability to summon dogs could only get him so far, and eventually he was overpowered and got killed.",
	"Here lies Ji Indur Dawndeath. After Sauron's death, Ji's own death also came swiftly.",
	"Here lies Akhorahil the Blind. He never knew who had actually killed him, but in any case Akhorahil was no longer protected from death when Sauron died.",
	"Here lies Hoarmurath of Dir. He had tried to protect Sauron in the tower of Dol Guldur, but when Sauron was dead, Hoarmurath got killed as well.",
	"Here lies Adunaphel the Quiet. She was the only female ringwraith, but that didn't protect her from the wrath of the Gondolindrim and soldiers of Minas Anor, who proceeded to stab her after Sauron was killed.",
	"Here lies Ren the Unclean. When Arda was freed of the corruption that Sauron had been spreading, the heroes of that battle then decided to seek and destroy all remaining sources of corruption, and so they killed Ren as well.",
	"Here lies Uvatha the Horseman. After the defeat of Sauron, Bree was finally able to put Uvatha down for good.",

};

const char *
random_epitaph()
{
	return (epitaphs[rn2(SIZE(epitaphs))]);
}


/* Create a headstone at the given location.
 * The caller is responsible for newsym(x, y).
 */
void
make_grave(x, y, str)
int x, y;
const char *str;
{
	/* Can we put a grave here? */
	if ((levl[x][y].typ != ROOM && levl[x][y].typ != GRAVE && levl[x][y].typ != CORR) || t_at(x,y)) return;

	/* Make the grave */
	levl[x][y].typ = GRAVE;

	/* Engrave the headstone */
	if (!str) str = epitaphs[rn2(SIZE(epitaphs))];

	del_engr_at(x, y);
	make_engr_at(x, y, str, 0L, HEADSTONE);
	return;
}


#endif /* OVLB */

/*engrave.c*/

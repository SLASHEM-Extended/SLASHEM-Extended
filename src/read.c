/*	SCCS Id: @(#)read.c	3.4	2003/10/22	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h"


/* KMH -- Copied from pray.c; this really belongs in a header file */
#define DEVOUT 14
#define STRIDENT 4


#define Your_Own_Role(mndx) \
	((mndx) == urole.malenum || \
	 (urole.femalenum != NON_PM && (mndx) == urole.femalenum))
#define Your_Own_Race(mndx) \
	((mndx) == urace.malenum || \
	 (urace.femalenum != NON_PM && (mndx) == urace.femalenum))

#ifndef OVLB

STATIC_DCL NEARDATA const short skill_names_indices[];
STATIC_DCL NEARDATA const char *odd_skill_names[];

#else	/* OVLB */

/* KMH, balance patch -- updated */
STATIC_OVL NEARDATA const short skill_names_indices[P_NUM_SKILLS] = {
	0,                DAGGER,         KNIFE,        AXE,
	PICK_AXE,         SHORT_SWORD,    BROADSWORD,   LONG_SWORD,
	TWO_HANDED_SWORD, SCIMITAR,       PN_SABER,     CLUB,
	PN_PADDLE,        MACE,           MORNING_STAR,   FLAIL,
	PN_HAMMER,        QUARTERSTAFF,   PN_POLEARMS,  SPEAR,
	JAVELIN,          TRIDENT,        LANCE,        BOW,
	SLING,            PN_FIREARMS,    CROSSBOW,       DART,
	SHURIKEN,         BOOMERANG,      PN_WHIP,      UNICORN_HORN,
	PN_LIGHTSABER,
	PN_ATTACK_SPELL,     PN_HEALING_SPELL,
	PN_DIVINATION_SPELL, PN_ENCHANTMENT_SPELL,
	PN_PROTECTION_SPELL,            PN_BODY_SPELL,
	PN_OCCULT_SPELL,
	PN_ELEMENTAL_SPELL,
	PN_CHAOS_SPELL,
	PN_MATTER_SPELL,
	PN_BARE_HANDED,	PN_HIGH_HEELS,
	PN_GENERAL_COMBAT,	PN_SHIELD,	PN_BODY_ARMOR,
	PN_TWO_HANDED_WEAPON,	PN_POLYMORPHING,	PN_DEVICES,
	PN_SEARCHING,	PN_SPIRITUALITY,	PN_PETKEEPING,
	PN_MISSILE_WEAPONS,	PN_TECHNIQUES,	PN_IMPLANTS,	PN_SEXY_FLATS,
	PN_MEMORIZATION,	PN_GUN_CONTROL,	PN_SQUEAKING,	PN_SYMBIOSIS,
	PN_SHII_CHO,	PN_MAKASHI,	PN_SORESU,
	PN_ATARU,	PN_SHIEN,	PN_DJEM_SO,
	PN_NIMAN,	PN_JUYO,	PN_VAAPAD,	PN_WEDI,
	PN_MARTIAL_ARTS, 
	PN_TWO_WEAPONS,
	PN_RIDING,
};


STATIC_OVL NEARDATA const char * const odd_skill_names[] = {
    "no skill",
    "polearms",
    "saber",
    "hammer",
    "whip",
    "paddle",
    "firearms",
    "attack spells",
    "healing spells",
    "divination spells",
    "enchantment spells",
    "protection spells",
    "body spells",
    "occult spells",
    "elemental spells",
    "chaos spells",
    "matter spells",
    "bare-handed combat",
    "high heels",
    "general combat",
    "shield",
    "body armor",
    "two-handed weapons",
    "polymorphing",
    "devices",
    "searching",
    "spirituality",
    "petkeeping",
    "missile weapons",
    "techniques",
    "implants",
    "sexy flats",
    "memorization",
    "gun control",
    "squeaking",
    "symbiosis",
    "form I (Shii-Cho)",
    "form II (Makashi)",
    "form III (Soresu)",
    "form IV (Ataru)",
    "form V (Shien)",
    "form V (Djem So)",
    "form VI (Niman)",
    "form VII (Juyo)",
    "form VII (Vaapad)",
    "form VIII (Wedi)",
    "martial arts",
    "riding",
    "two-weapon combat",
    "lightsaber"
};

#endif	/* OVLB */

#define P_NAME(type) (skill_names_indices[type] > 0 ? \
		      OBJ_NAME(objects[skill_names_indices[type]]) : \
			odd_skill_names[-skill_names_indices[type]])

#ifdef OVLB

boolean	known;

static NEARDATA const char readable[] = {
		   SCROLL_CLASS, SPBOOK_CLASS, RING_CLASS, 0 };
static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };


#if 0
static const char *warnings[] = {
	"white", "pink", "red", "ruby", "purple", "black"
};
#endif

#if 0
static void wand_explode(struct obj *);
#endif
static void do_class_genocide(void);
static void do_total_genocide(void);
static void do_class_erasure(void);
static void stripspe(struct obj *);
static void p_glow1(struct obj *);
static void p_glow2(struct obj *,const char *);
static void randomize(int *, int);
static void maybe_tame(struct monst *,struct obj *);
static void undo_genocide(void);

STATIC_PTR void set_lit(int,int,void *);

int
doread()
{
	register struct obj *scroll;
	register boolean confused;
	char class_list[SIZE(readable) + 3];
	char *cp = class_list;
	struct engr *ep = engr_at(u.ux, u.uy);
	boolean cant_see = Blind;
	struct obj otemp;

	/* cartomancer can sometimes keep a scroll after reading it --Amy */
	boolean cartokeep = FALSE;
	int cartochance = 0;

	*cp++ = ALL_CLASSES;
	*cp++ = ALLOW_FLOOROBJ;
	if (!u.uswallow && ep && ep->engr_txt[0])
	    *cp++ = ALLOW_THISPLACE;
	strcpy(cp, readable);

	known = FALSE;
	if(check_capacity((char *)0)) return (0);
	scroll = getobj(class_list, "read");
	if(!scroll) return(0);

	if (InterruptEffect || u.uprops[INTERRUPT_EFFECT].extrinsic || have_interruptionstone()) {
		nomul(-(rnd(5)), "reading a scroll", TRUE);
	}

	if (scroll == &thisplace) {
	    (void) sense_engr_at(u.ux, u.uy, TRUE);
	    return 0;
	}

	if ((scroll->oinvis && !See_invisible) || scroll->oinvisreal)
	    cant_see = TRUE;

	/* KMH -- some rings can be read, even while illiterate */
	if (scroll->oclass == RING_CLASS) {
	    const char *clr = (char *)0;

	    if (cant_see) {
		You("cannot see it!");
		return 0;
	    }
	    if (scroll->where != OBJ_INVENT || !(scroll->owornmask & W_RING)) {
		pline(FunnyHallu ? "The writing is so small, you'd need to take a closer look..." : "Perhaps you should put it on first.");
		return 0;
	    }
	    if (scroll->dknown && objects[scroll->otyp].oc_name_known)
		switch (scroll->otyp) {
#if 0	/* Not yet supported under 3.3.1 style warning system */
		    case RIN_WARNING:
			if (warnlevel >= 100)
			    clr = "light blue";
			else if (warnlevel >= SIZE(warnings))
			    clr = warnings[SIZE(warnings)-1];
			else
			    clr = warnings[warnlevel];
			break;
#endif
		    case RIN_MOOD:
			if (u.ualign.record >= DEVOUT)
			    clr = "green";	/* well-pleased */
			else if (u.ualign.record >= STRIDENT)
			    clr = "yellow";	/* pleased */
			else if (u.ualign.record > 0)
			    clr = "orange";	/* satisfied */
			else
			    clr = "red";	/* you've been naughty */
			break;
		    default:
			break;
		}
	    if (!clr)
		pline("There seems to be nothing special about this ring.");
	    else if (scroll->cursed)
		pline("It appears dark.");
	    else
		pline("It glows %s!", hcolor(clr));
	    return 1;
	}

	/* outrumor has its own blindness check */
	if(scroll->otyp == FORTUNE_COOKIE) {
	    long save_Blinded = Blinded;
	    if(flags.verbose)
		You("break up the cookie and throw away the pieces.");
	    Blinded = cant_see;	/* Treat invisible fortunes as if blind */
	    outrumor(bcsign(scroll), BY_COOKIE, FALSE);
	    Blinded = save_Blinded;
	    if (!cant_see) u.uconduct.literate++;
	    if (carried(scroll)) useup(scroll);
	    else useupf(scroll, 1L);
	    return(1);
	} else if (scroll->otyp == T_SHIRT || scroll->otyp == HAWAIIAN_SHIRT || scroll->otyp == BLACK_DRESS
	|| scroll->otyp == STRIPED_SHIRT || scroll->otyp == BODYGLOVE || scroll->otyp == BAD_SHIRT || scroll->otyp == CHANTER_SHIRT || scroll->otyp == KYRT_SHIRT || scroll->otyp == WOOLEN_SHIRT || scroll->otyp == METAL_SHIRT || scroll->otyp == RED_STRING || scroll->otyp == YOGA_PANTS || scroll->otyp == GREEN_GOWN
	|| scroll->otyp == BEAUTIFUL_SHIRT || scroll->otyp == PETA_COMPLIANT_SHIRT || scroll->otyp == TOILET_ROLL || scroll->otyp == RADIOACTIVE_UNDERGARMENT
	|| scroll->otyp == PRINTED_SHIRT || scroll->otyp == BATH_TOWEL
	|| scroll->otyp == PLUGSUIT || scroll->otyp == SWIMSUIT || scroll->otyp == MEN_S_UNDERWEAR
	|| scroll->otyp == VICTORIAN_UNDERWEAR || scroll->otyp == RUFFLED_SHIRT) {
	    static const char *shirt_msgs[] = { /* Scott Bigham */
    "I explored the Dungeons of Doom and all I got was this lousy T-shirt!",
    "Is that Mjollnir in your pocket or are you just happy to see me?",
    "It's not the size of your sword, it's how #enhance'd you are with it.",
    "Madame Elvira's House O' Succubi Lifetime Customer",
    "Madame Elvira's House O' Succubi Employee of the Month",
    "Ludios Vault Guards Do It In Small, Dark Rooms",
    "Yendor Military Soldiers Do It In Large Groups",
    "I survived Yendor Military Boot Camp",
    "Ludios Accounting School Intra-Mural Lacrosse Team",
    "Oracle(TM) Fountains 10th Annual Wet T-Shirt Contest",
    "Hey, black dragon!  Disintegrate THIS!",
    "I'm With Stupid -->",
    "Don't blame me, I voted for Izchak!",
    "Don't Panic",				/* HHGTTG */
    "Furinkan High School Athletic Dept.",	/* Ranma 1/2 */
    "Hel-LOOO, Nurse!",			/* Animaniacs */
			"Frodo Lives!",
			"Actually, I AM a quantum mechanic.",
			"I beat the Sword Master",			/* Monkey Island */
			
			"Don't ask me, I only adventure here",
			"Hello, my darlings!", /* Charlie Drake */
			"^^  My eyes are up there!  ^^",
			"Anhur State University - Home of the Fighting Fire Ants!",
			"Plunder Island Brimstone Beach Club",

			/* http://www.alt.org/nethack/addmsgs/viewmsgs.php */
			"That is not dead which can eternal lie, and in successful ascensions even Death may die.",
			"3,14159 26535 89793 23846 26433 83279 50288 41971 69399 37510 58209 74944 59230 78164 06286 20899...",

			/* UnNetHack */
			"I made a NetHack fork and all I got was this lousy T-shirt!",	/* galehar */

			/* UnNetHackPlus */
			"not food",
			"ingredients: oxygen 65%, Carbon 18%, Hydrogen 10%, Nitrogen 3%, Calcium 1,5%, Phosphorus 1%",
			"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
			"I made a UnNetHack fork and all I got was this lousy T-shirt!",	/* see UnNetHack section */
"I captured the t-shirt factory, anyone who kill every last caveman gets 9 +9 t-shirts",
"Kill, Maim, Burn...",
"$playername IS THE BOSS!",
"'$playername, I'm watching you, scum!' -- Wizard of Yendor",
"(This space for sale.)",
"*insert Monty Python quote here*",
"*insert pic of uboa here*",
"+5 sexterity vest",
"+<<*giant cave spider silk T-shirt*>>+",
"+Infinity Billion Cool Shirt of Awesomeness",
"- - - - - - CUT HERE - - - - - -",
"-----Fill up to here.-----",
"--More--",
"-export-a-crypto-system-sig -RSA-3-lines-PERL",
"/</tits/> /<//tits/>",
"100% goblin hair - do not wash",
"3.141592653589792636.......",
"666 420 Tongue Kiss Your Father",
"9/11",
"9/11, I'm In Heaven!",
"90% cotton, 10% polyester.",
":-)",
"<tits> </tits>",
"=^.^=",
"@ k a",
"ARE YOU ASKING FOR A CHALLENGE?!?!?!?",
"Aberzombie and Fitch",
"Aim >>> <<< here",
"All I got was this lousy T-Shirt",
"Anarchy in the U(nderground) K(ingdom)",
"Anhur is my homeboy",
"Aperture Science Torso-Covering Device",
"Ask Me About Grim Fandango",
"Ask Me About Loom",
"Ask Me About Rogue",
"B.a.r. Equipment",
"Beauty Is In The Eye Of The Beerholder",
"Best in Show",
"Big Daddy",
"Blue Sun",
"C:/DOS C:/DOS/RUN RUN/DOS/RUN",
"CITY NAME SPORTS TEAM",
"Came back from the demigod bar to watch you puny mortals ascend.",
"Can you believe that they actually put this on a T-Shirt?",
"Cockatrices can't touch this",
"Cool Tapes",
"Croesus for President 2008 - Campaign Finance Reform Now (for the other guy)",
"D.A.R.E. - Dungeon Adventure Resistance Education",
"DX Army",
"Darmok and Jalad at Tanagra - World Tour '91",
"David Allen CEO",
"Deep Throat",
"DethKlok",
"Dev Team",
"Dispater is my homeboy",
"Don't ask me, I only work here",
"Don't know where to buy some lighting? Visit Itzchak's at minetown.",
"Down With Pants!",
"Dry clean only... good luck",
"Dubno;withmyheadman",
"E.Z. Moad",
"ELBERETH ALREADY!",
"Elbereth",
"Elvira's House O'Succubi, granting the gift of immorality!",
"End Mercantile Opacity Discrimination Now: Let Invisible Customers Shop!",
"Ever did the cha-cha with a pit viper?",
"Ever did the waltz with a nymph?",
"Ever held hands with a cockatrice?",
"Ever let a water demon spank you?",
"FREE HUGS",
"FREE PUG AND NEWT!",
"Fhtagn!",
"Flooring inspector.",
"Frankie Says Relax",
"Frankie say: Ascend",
"Free shirt: Limit 1",
"Funny message goes here",
"Gamefaqs.com, We have a nethack board.",
"Gehennoms Angels",
"Glutton For Punishment",
"Go Team Ant!",
"Go team 'a'!",
"Got Newt?",
"Got OCD?",
"Got Potion?",
"Got pr0n?",
"Great, you're not illiterate. Now try Elbereth.",
"Have a heart. Spay or neuter your d or f.",
"Hawaiian T-Shits Just Suck!",
"He who dies with the most toys is nonetheless dead.",
"Heading for Godhead",
"Hey! Nymphs! Steal This T-Shirt!",
"Hey, Wizard of Yendor, Demigod This!",
"Hi, My name is $playername!",
"Home Taping is Killing Music",
"How To Keep An Idiot In Suspense - See Other Side",
"I <3 Dungeon of Doom",
"I <3 Maud",
"I Am Not a Pack Rat - I Am a Collector",
"I Support Single Succubi",
"I am a Demigod, oh yes...",
"I am a Valkyrie. If you see me running, try to keep up.",
"I am a proud brony.",
"I ascended to demi-godhood, and all I got was this lousy T-Shirt!",
"I beat anorexia",
"I bounced off a rubber tree",
"I can hold my breath for TEN minutes!",
"I couldn't afford this T-shirt so I stole it!",
"I do not have the amulet of yendor YET !!!",
"I don't like your ringtone",
"I got sick riding the Juiblex Express",
"I got zapped with a wand of polymorph and all I became was this stupid T-shirt.",
"I killed the Wizard and all I got was this stupid T-shirt!",
"I killed the wizard of Yendor, and all i got is this stupid t-shirt.",
"I met Carl, the swordmaster of Jambalaya island and all I got was this lousy t-shirt",
"I prepared explosive runes this morning.",
"I rode the Juiblex Express",
"I rode the Juiblex Express and became sick",
"I scored with the princess",
"I took the red pill!",
"I want to live forever or die in the attempt.",
"I went into the Mazes of Menace and all I got was this stupid T-Shirt.",
"I went to Yendor and all I got was this lousy T-Shirt.",
"I wish these were magic lamps",
"I wished for blessed +2 GDSM and all I got was this lousy T-shirt.",
"I won the nethack.alt.org June Tournament., and all I got was this lousy T-Shiry!",
"I'm Confused!",
"I'm back from the demigod bar!",
"I'm in ur base, killin ur doods",
"I'm like a superhero with no powers or motivation.",
"I'm not even supposed to be here today.",
"I'm not fat, I'm knocked up!",
"I'm not illiterate.",
"I'm selling these fine leather jackets",
"I'm with Stupid -->",
"I'm with a pet",
"I'm with stupid ->",
"If you can read this, I can hit you with my polearm",
"If you can read this, you are too close.",
"If you can read this, you're standing too close.",
"If you read this, your illiterate conduct is lost.",
"Im not emo!",
"Im with stupid -->",
"Iron Maiden",
"Ironica",
"It keeps happening!",
"Kobolds do it better",
"Kop Killaz",
"LOST IN THOUGHT - please send search party",
"Lichen Park",
"Linus Torvalds is my homeboy",
"Loser Pride",
"Make Love, Not War",
"Maxim 1: Pillage, then burn",
"Maybe if this shirt is witty enough, someone will finally love me.",
"Meat Is Mordor",
"Medusa is my homegirl",
"Mind Flayers suck!",
"Minetown Better Business Bureau",
"Minetown Watch",
"Ms. Palm's House of Negotiable Affection -- A Very Reputable House Of Disrepute",
"My name is shawnz. Take me off and see how fat I am.",
"My other armor is a blessed +5 gray dragon scale mail.",
"My other sword is a Vorpal Blade!",
"My sisters' are bigger",
"Neferet/Pelias '08",
"Next time you wave at me, use more than one finger, please.",
"No Outfit Is Complete Without a Little Cat Fur",
"No, I will not fix your computer.",
"No, this is not a blessed +7 silver dragon scale mail. You must be crazy.",
"Nothing happens.",
"OPTIONS=marvin,!suck,rngfavor",
"Objects In This Shirt Are Closer Than They Appear",
"Occupy Fort Ludios",
"Only you can prevent forest fires.",
"Outlook not so good.",
"Overcharge me!",
"P Happens",
"PETA - People Eating Tasty Animals",
"Pain is vital bodily fluids leaving the body.",
"Play Paranoid!",
"Plunder Island Brimstone Beach Club Lifetime Member",
"Porn Star",
"Prefectionist.",
"Protection Racketeer",
"Queen B",
"Randomness Happens",
"Real Men Are Valkyries",
"Real men love Crom",
"Real men watch ponies.",
"Rodney for President!",
"Rodney in '08. OR ELSE!",
"Rogues do it from behind!",
"Rogues do it from behind.",
"Romanes Eunt Domus",
"Romani Ite Domum",
"S.O.B: Succubi Obliging Boytoy",
"SHOOOOP DA WHOOOOOOOP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
"SPONSORED BY GOLDENPALACE.COM",
"Save The Rare Endangered Spycrab!",
"Save collapsing wavefunctions: don't look at me!",
"Scalding Lake",
"Seduce Me, Please.",
"Serial Ascender",
"Shoot, and I'll move. In a flash.",
"Smile if you're not wearing underpants.",
"Sokoban Gym -- Get Strong or Die Trying",
"Somebody stole my Mojo!",
"Telepathics Do It Blindly",
"The Amulet of Yendor has been stolen by Moloch. Are you a bad enough dude to retrieve the Amulet?",
"The Game",
"The Hellhound Gang",
"The Werewolves",
"The front says: '+5' The back says: 'fireproof'",
"The front says: 'This message is false.' The back says: 'The message on the front is true.'",
"The front says: 'Throw daggers!' The back says: 'Because they're sharper than rocks.'",
"The tengu shuffle is my favorite dance.",
"There are 10 types of adventurers. Those who understand binary and those who don't.",
"There's no place like Home 1",
"They Might Be Ettins",
"They Might Be H",
"They Might Be Storm Giants",
"They say that this is not an actual t-shirt, but more like a halfshirt",
"This T-Shirt left intentionally blank",
"This T-shirt sucks.",
"This T-shirt was made by someone who leaves there stuff in the most places",
"This is an ironic t-shirt.",
"This is not a T-Shirt",
"This is not a shirt.",
"Torso Awaregness intrinsic",
"Tourists Be Gone!",
"Up with miniskirts!",
"Visit Nicolai's house of magic, a last vestige of civilization in an otherwise uncivilized land.",
"Weapons don't kill people, I kill people",
"What happens in Gehennnom stays in Gehennom",
"Where's the beef?",
"White Zombie",
"Winner of the 2006 Wet T-shirt contest!",
"Woodstock '69",
"Words on a Shirt",
"Worship me",
"Xorn ate my +7 Stormbringer and +6 Grayswandir",
"Xorns Do It Anywhere",
"Yet Another Stupid T-Shirt",
"You feel douchy! You must have a poor sense of fashion..",
"You feel geeky! You must be leading a sunless lifestyle.",
"You laugh because I'm different, I laugh because you're about to die",
"You see here a ye flask.",
"You should hear what the voices in my head are saying about you.",
"You're killing me!",
"Young Men's Cavedigging Association",
"Your Dad On A Sybian",
"Your Head A Splode",
"Your bones itch.",
"^^ My eyes are up here! ^^",
"cK -- Cockatrice touches the Kop",
"d, your dog or a killer?",
"lol",
"mde.tv",
"nWo",
"noitcelfer fo trihs-T 5+ desselB",
"only 100 zm for that t-shirt",
"sloshy",
"sysadmin",
"the High Priest of Moloch knows how to accessorize",
"your an idiot",
"zorkmid",
"~~It's Time 2 Throw Down~~",
"Don't worry, be happy! Amy likes all SLEX players! SMILE!!!",
"I got lobotomized! I'm a member of Amy fan club now!",
"I participated junethack 2016 -- TROPHY GET!",
"Hey, my name is Yasdorian - a name telemarketers loooove!",
"You can't. It's cursed.",
"I participated in 'Apokalyptia's Next Topmodel' and survived!",
"I confronted Gruff Co-Worker and didn't get my ass kicked!",
"I punched a black dragon to death with my bare hands!",
"This shirt is unique! Well, until you go to a shop that prints shirts, and order them to print this exact message on it.",
"XOR ME HSALS!",
"LUXUS MODA!",
"man there's literally satanists and everything else is absolute shit and there's autists everywhere",
"I got a trophy in every variant in last year's Junethack!",
"Play slex! *bundlebundlebundle*",
"Geht nicht. Es ist verflucht.",
"My sexy butt cheeks can produce beautiful farting noises!",
"I'm totally hot in bed!",
"Want to spend a romantic night with me? Just 20 zorkmids! Don't worry, I'm working for women too!",
"I'm a bitch - deal with it!",
"This shirt really is as fleecy as it looks!",
"I'm in love with AmyBSOD!",
"Member of Anna's Hussy Club",
"Supreme Chairwoman of Anna's Hussy Club",
"Bang Gang - Don't mess with us!",
"Teacher Darts Competitor",
"Why are you looking at my shirt? My high heels are what should be catching your eyesight!",
"I got 113 trophies in Junethack without resorting to botting or similarly lame strategies!",
"I ascended Nethack 343, 360, Unnethack, Dynahack and Nethack4 - now I'll ascend SLEX!",
	"I'm not wearing any pants",
	"Down with the living!",
	"Vegetarian",
	"Vegan and Proud and Soon To Be Starved", /* sorry but I don't support eating disorders --Amy */
	"Hello, I'm War",
	"Sredni Vashtar went forth.  " /* Saki short story */
	"His thoughts were red and his teeth were white.",
	"Keep calm and explore the dungeon.",
	"One does not simply kill the Wizard of Yendor.",
	"I killed the Wizard of Yendor.  (He got over it.)",
	"Giants are Tossers!", /* -- elenmirie */
	"I don't keep calm. I plot and seek revenge.",
	"I DON'T KEEP CALM! AVADA KEDAVRA!!!",
	"I fucked dnethack's elder priest and severed his asshole tentacles and all I got is this lousy t-shirt!",
	"Flauusch!",
	"I love high heels!",
	"Supreme Superschool Speaker",
	"SLASH'EM Extended devteam member",
	"I love to eat rotten food! (Bleech!)",
	"If you're a smoker, don't get closer than 100 feet to me, for I will sever your tentacle with my high heels if you dare smoke in my presence.",
	"Newt Fodder",
	"My Dog ate Og",
	"End Lich Prejudice Now!",
	"Down With The Living!",
	"Pudding Farmer",
	"Dudley",
	"I pray to Our Lady of Perpetual Mood Swings",
	"Soooo, When's the Wizard Getting Back to You About That Brain?",
	"I plan to go to Astral",
	"If They Don't Have Fruit Juice in Heaven, I Ain't Going",
	"Living Dead",

	/* #nethack suggestions */
	"Lich Park",
	"I'd rather be pudding farming",

	"Great Cthulhu for President -- why settle for the lesser evil?",
	"Wizard Farming Is More Profitable Than Pudding Farming",
	"I don't like the actors lines.",
	"I don't like the way the RNG determines what message is displayed on my shirt.",
	"Sign the petition for shirts' rights now - vote YES for making all shirts readable, including the striped shirt!",
	"Our love must become strength.",
	"I'm doing the breatharian conduct today",
	"Watch me get my first wishless ascension!",
	"I am Iron Man. I ascended a binder in slex in ironman mode. Wishless conduct to boot. Beat that! :P",
	"Look, behind you! An air elemental speed beholder!",
	"I ascended SLEX, why didn't you?",
	"I ran a half marathon untrained in 2h 38min! Beat that! :P",
	"I scored 113 trophies in last year's Junethack! If you don't have that many yourself, GTFO pleb :P",
	"I love inhaling the fragrance of Josefine's sweaty feet",
	"The scent of femininity that emanates from me is the wonderful Christina Aguilera Shower Gel <3",
	"I love Britney Spears!",
	"If you pay me $5 I'll let you kick me in the nuts once!",
	"My mistress is a cruel lady but I love her anyway",
	"Being tied up is fun!",
	"Wanna undress the soft cotton that covers my sexy female body?",
	"Streichelbefuehlueng", /* shame we can't use 'ü' */
	"I say the kurwa role is easy", /* by dolores */
	"I'm an OP necro", /* ditto */
	"Camperstriker is the best role!", /* ditto */
	"Ranged chars are OP!", /* ditto */
	"Fuck ugods!", /* ditto */
	"If you move and shoot at the same time I'll call you out for cheating", /* ditto */
	"Yes, wands of remove resistance can steal your intrinsic polymorph control. Deal with it, n00b!",
	"So you're calling me the 'squatter' of SLASHEM Extended and accuse me of 'stalking' your Facebook account? Well, sue me! :-P",
	"No, I'm not looking to gain the 'respect' of players who ragequit over something minor like being zapped with a wand of remove resistance. Play my variant or bugger off :P",
	"I know the difference between cone, block and wedge heels!",
	"I Hate Boys", /* Christina Aguilera */
	"I'm a primadonna, I can rule the world!", /* ditto */
	"I'm the sexiest bitch in this dungeon!",
	"I have grand master high heels skill! What about you?",
	"My riding skill is at Grand Master and I'm riding a speed 72 cockatrice. I bet I can outspeed your car.",
	"omg chevalier detects BUC omg that role is OP", /* by dolores */
	"I beat Elona with a snail tourist and only got killed by cleaners twice",
	"I can play the piano in Vernis without getting my skull split by Loyter!",
	"I'm a 4chan rlg memeposter!",
	"FeAr MiNe HoMo KoCk BaBe", /* DCSS - yes, it really allows you to have characters that will shortcut to those words */
	"I dEfEaTeD aNdOr DrAkOn AnD wEnT iNsAnE", /* ADOM */
	"Why don't you shove a dildo golem up yourself?", /* by Crawldragon */
	"Wow, it's incredible that a single t-shirt can be SO fleecy!",
	"tfw you want to be a goomba underneath maria with high heels", /* by bugsniper */
	"YOU DIED - Dark Souls",
	"Ay have come ta answuurr thay ...uhh questions y'all may have about my decisions on thay ...uhh code, ayn' my answuurr is ay'm God ayn' y'all can't stop God!",
	"ow! Who throws a show? Honestly?", /* -- from Austin Powers according to ziratha */
	"Made in china, please recycle in bin number 4",
	"I like disintegration resistance and therefore refrain from playing Satanchop",
	"I'll genocide all the fuckers in kneelhack that cast destroy armor. Curse the fact that this spell is way too overpowered!",
	"I beat the glass golem! I can identify everything without fail now!",
	"I chopped my way through 30 levels worth of ghosts in the Void and defeated Tiksrvzllat and all I got was this lousy t-shirt!",
	"I ascended a cavewoman in slex with the wishless, genocideless, polyselfless and polyobjectless conducts intact! Why don't you ascend too? It's really not that hard, you just need lots of patience and the will to try again no matter how many times you die!",
	"I ascended FIQhack and didn't get FIQd over and all I got was made fun of.", /* thanks Demo */
	"i'm playing shitty meme fork simulator 2018", /* by some slex player on 4chan, probably Coolfrog */
	"Did i give you permission to reply to my post you carpet munching bootlicking bugwoman cuck? Shut the fuck up Amy of course a womyn with eleventeen IQ will never be able to comprehend the grandeur of Tales of Maj'Eyal", /* ditto */
	"It is better to light a candle than to curse the darkness",
	"It is easier to curse the darkness than to light a candle",
	"rock--paper--scissors--lizard--Spock!", /* expanded "rock--paper--scissors" featured in TV show "Big Bang Theory" although they didn't create it (and an actual T-shirt with pentagonal diagram showing which choices defeat which) */
	"/Valar morghulis/ -- /Valar dohaeris/", /* "All men must die -- all men must serve" challange and response from book series _A_Song_of_Ice_and_Fire_ by George R.R. Martin, TV show "Game of Thrones" (probably an actual T-shirt too...) */  
	"I <3 Humanity", /* No Game No Life */
	"Ignore his posts. Nobody literally plays his game but him and nobody has ever tried to ascend except for him. It'd be sad to watch if he wasn't so annoying and spammed his screenshots everywhere here and on reddit.",
	"Tuna Fish - expires Feb 6, 1989",
	"Rare Meat of a Genocided Hallucinatory Monster",
	"Water Chestnuts - be sure to eat while underwater",
	"Yendorian Brand Applesauce",
	"This Tin Is Not Booby-Trapped",
	"This Shirt Is Not Booby-Trapped",
	"This Shirt Is Not Booby-Trapped, Feel Free To Caress",
	"Fresh Tinned Adventurer Meat",
	"MumaKill",
	"Sarcastic Comment Loading - Please Wait",
	"On a scale of 1 to 10, what is your favorite color of the alphabet?",
	"Huge hung hero hunks hastily hump horny heaving hot whores.",
	"you can dig as much as you want and will find it eventually...", /* -- AmyBSOD Feb2018 */
	"Sure, slex has standards. And the usual standard is, everyone else has to think it's a bad idea, or Amy won't implement it.", /* by jonadab */
	"The greatest value of Slex is existing as a surreal wide awake nightmare game that never makes sense to anyone.", /* someone who took my SLEX survey in Feb 2018 */
	"This shirt scores very well on Amy's fleeciness scale!",
	"I follow the No Cyanide Rule: No item should, if use-tested in reasonable circumstances, be game-ending! Also, here's an example of an item which directly violates it!", /* yeah aosdict, you are right that the cloak of death violates it, but no player complained about it yet so it can't be so bad! */
	"I managed to ascend Nethack Fourk and only got screwed over by wands of lightning, elder black dragons, deep ettins and half a dozen other things!",
	"I absolutely HATE K-Measurer's elder black dragons",
	"I refuse to play doormash because it suddenly updates its versions and erases my savegame",
	"I absolutely HATE K-Measurer's elder black dragons, and a certain other monster with 'elder' in its name",
	"Viva la Croissistance!", /* by Elronnd */
	"I wanna boh", /* inspired by Demo */
	"Amy is the insane dev of slex. She adds literally anything she can think of to the game, has no idea what the concept of balance is, and i'm pretty sure is actually terrible at playing roguelikes.",
	"Because of all of this, her games are extremely fun to play generally to see the utterly ridiculous ways you can die in them, though not fun at all to play if you're trying to win or understand wtf is even going on.",
	"I played SLEX exactly three times and ascended all three of those games.",
	"Amy do u have a bundle and heart key on ur keyboard?", /* by Demo */
	"I've always known that chicks will inherit the Earth, I just didn't know it'd be this much fun.", /* don't remember who said that, maybe Grasshopper? */
	"oh wait, I got it now: all those who are complaining about the allegedly 'awful polearms interface' are using vikeys! no wonder! with vikeys, everything is awful :P number pad ftw!",
	"i am the slex master, i ascend 500 times a clock cycle, i have a master guardian gmx3 vest and shoot the phasergons with my skooble gun that fires like 40 rocks a turn", /* by Demo */
	"Nubere volunt et omnino hedera aureum, quod est pulchra puella tam mirabilis!",
	"Unguibus et etiam habet pulchra!",
	"kneelhack you are so stupid! why do you present me the prompt again if I cancel it with space? space should remove the prompt completely!",
	"I consider TSX a variant of vanilla NetHack",
	"I'm the ultimate girl gamer!",
	"My favorite SLEX role? Why, feminist, of course!",
	"I participate in slutwalking",
	"Yes, I look very sexy and it is not my fault if someone gropes me. Men have to learn to control their instincts, or else!",
	"Genocide K-Measurer's tab confirmation",
	"I ascended Splicehack in June 2018 and at that time it was the fastest turncount ascension ever in that variant!",
	"I ascended Unnethack and only had to kill Cthulhu thrice and search the entire sanctum for the amulet while constantly being afraid of him respawning, grabbing the amulet, and it being teleported away again when he dies!",
	"I'm the bitch who got the 'die to 9 specific monsters' trophy (introduced while Junethack 2018 was already running) before anyone else did and it didn't take me hundreds of scurrier scum games!",
	"Hahahahahahahahahahahahahahahahahahahahahahahahahahaha Germany lost 0:2 to South Korea in 2018 and finished dead last in the first round :D",
	"Wands don't kill people - I do.",
	"Variant developers should make sure players on all platforms with all supported control schemes have a good playing experience, not only the ones that use the same one the dev is using.",
	"I can bitch about badly designed Junethack trophies and I will not shut up about them even if you kick me from the IRC channel!",
	"Can't deal with me bitching about shitty scumfests? Deal with it, bitch!",
	"Eyehack's yes/no confirmation when stepping on water should be genocided",
	"Rodney's Tower's tab confirmation should be genocided",
	"Half a dozen scroll types in Rodney's Tower should be genocided because monsters will screw you over when they read those",
	"Eyehack's yes/no confirmation when stepping on water is as useful as a hole in the head",
	"K-Measurer's tab confirmation when overeating is an ulcerous chancre that should be cut from the game with a rusty knife",
	"#blacklightsmatter", /* by Demo */
	"Think about obsessive players in your community 'What is the worst/most stupid/most obsessive thing a player could do?'.  You will probably be wrong", /* by bhaak */
	"Defund slex development", /* by Demo */
	"The nymphets would have won junethack 2018 but they couldn't score enough medusa kills, ascensions in 24 hours and good specific monster kill times.",
	"2018 Medusa Kill Scummer",
	"Wielder of Weaponized Autism",
	"Healer Startscummer Who Wishes For Asmodeus Statues",
	"Don't mind me, I'm busy parking a tarot card deck of Dynahack ascensions. This is my 21st one, 20 others are already waiting on the Astral Plane and will ascend on June 30.",
	"When is the ETA for Chinese world domination? I prefer a cruel master to a crazy one.",
	"I ran 10 km in wedge sandals and it only took me 1 hour and 24 minutes!",
	"I don't like poofy coffee, and I don't like making changes to vanilla messages either.",
	"I'm a gay man and I want you to stop getting offended on my behalf. I don't consider 'poofy' an insult.", /* thanks Crawldragon (inspiration is his, actual wording mine --Amy) */
	"High heels are very comfortable and I'll never wear flats again!",
	"Plants Have Rights Too - Ban Lawnmowers And Combine Harvesters Now!",
	"SLEX = Securing Lesbians for Entertainment of Xerself (the Amy)",
	"It's offensive to presume that LGBT players can't handle a quaint word like 'poofy' in a computer game. Not all of us are shrieking narcissists whose delicate sensibilities are shredded by words.", /* thanks Crawldragon (this is a literal quote from him) */
	"DATA: a lesbian occultist who leads a harem of up to 70-200 lesbians she met from SLEX. At one time this harem was smaller, consisting of around 20 lesbians. She has since expanded her dating pool by developing mods for Elona, Goldfish RL and other more popular roguelikes.",
	"I've noticed you around, I find you gay and happy. Would you go to bed with me?",
	"I've noticed you around, I find you quite attractive. Would you, uhh...",
	"play slex 2.2.8 now on esm!!! fleecier than ever!!!! *bundlebundlebundle*",
	"An ascension a day keeps Rodney away!", /* by Demo */
	"An apple a day keeps anyone away, if thrown hard enough!",
	"My first wish: uncursed historic partly eaten magic marker named amy is a voyeur", /* by hothraxxa */
	"I asked for luck of the grasshopper, but all I got was this lousy slex update message",
	"Because I'm playing as a Feminist, I have these shoes on that cause random traps to appear every time a monster is spawned, and for some reason I keep getting the trap that causes my character to take damage every time I hit a female monster because my poofy goddess gets mad at me.", /* by Crawldragon */
	"SAVE THE ZRUTYS",
	"SpliceHack Beta Tester",
	"#NerfTheLoTF",
	"Got ASCII?",
	"Don't @ me!",
	"I <3 Sokoban",
	"I digested the lord of the foocubi whole! Fear my mad polyselfing skills!",
	"Void dragons cannot disintegrate me. I disintegrate void dragons. Regularly.",
	"esm = erotic shoe memes", /* from 4chan */
	"there should be a variant called amyhack where all monsters are shoes, all object descriptions are fleecy, and instead of hitting things or them hitting you, you bundle them", /* by Demo */
	"I'm Belba Loamsdown of Deephallow.", /* this and the next two by raisse */
	"I'm Nellas Carnesir if I'm an elf.", /* sadly we cannot use î */
	"my wallet name actually does better, Amaurea Vanimedle", /* sadly we cannot use ë */
	"here's looking at you, bhaak ;) *äugeläugel*", /* pun because of the "Schwebäugler" <3 */
	"it's septembernethack, a cross variant bug fixing shitshow", /* by Demo */
	"But I want my own house and not a girl!",
	"whoa boy I did that 'slex with amy' thing last night, i got bundled twice. i think i fleecied 3 times!", /* by K2 */
	"After years of research, I have developed the ultimate counter to bundling: eldnubeldnubeldnub :D", /* aosdict I think? */
	"Demo: making slex more retarded, one bad idea at a time.", /* he said that himself because I keep implementing stuff he suggests :D */
	"do these yoga pants make my butt look big? no your butt makes your butt look big", /* by K2 */
	"here have a potion on me buddy!",
	"i'm siyenne! nice to meet you!",
	"i'm really a friendly fairy! don't listen to what they're saying about me",
	"i'm actually kind of shy, but i'm hoping talking to people will help make me more approachable!",
	"the crown on my head? oh, i'm not royalty, i just thought it looked cute",
	"i'm a fairy, but i'm really more of a butterfly i guess--i have a second pair of arms but i can't use them here...that's what the empty sleeves are for",
	"i'm rather pale...i don't get out much",
	"Even the God of Chaos cowers beneath my power!",
	"Do I have something on my face?",
	"You don't wanna play hardball with me.",
	"This is not proper corporate attire.",
	"Do you want me?",
	"I may just be a flunky - but I'm real spunky.",
	"I am Najelith of the wind. Asking helps.",
	"I may be small, but I'm also tough.",
	"I'm No. 2!",
	"Votes for Women - Abolish Discrimination Against Half Of Every Home", /* Red Dead Redemption 2 */
	"You're asking if the apocalypse will happen soon? Well, here's a newsflash: it has already happened and right now we're dealing with the aftermath!",
	"The forced draft for men is sexism at its finest.",
	"418 I'm a Teapot. The resulting entity body MAY be short and stout. Tip me over and pour me out.", /* by attie; she wants actual teapot items with labels, but I'm taking the easy way out :P */
	"Transsylvanian Teacher Darts Club",
	"I got the first official notdnethack ascension! *oink oink*", /* Porkman :D */
	"v qe qe qe qe qd qd qd qd qf qf qf qf qg qg qg qg #quit", /* by Demo */
	"I haven't watched Game of THrones and I don't care", /* by Elronnd IIRC */
	"I should become a SLEX developer so I can inflect EVIL upon the world.", /* by NCommander */
	"I own your stuff now. fight me", /* by bhaak */
	"Say YES to plastic! Glass can break. Plastic persists forever and for always in the seas.",
	"Why did I explain the joke? Now it's not funny anymore you dummy!",
	"Good day, I'm Amy from the slexomatic institute. Would you like to take a survey? It will take about 6 minutes. First question: do you like high heels?",
	"Hello, I'd like to take a survey about your customer satisfaction with the 'Deutsche Bahn'. First question: Do you like the word 'delay'?",
	"What? You're saying you want the game to be realistic? Man, I play games to escape from reality for a while, not to try to emulate reality as accurately as possible!",
	"I'm not burdened by a too low carry capacity and I won't play any variant that doesn't have slex-level carry caps for the player character. Because I want to spend my time actually PLAYING the game, not doing a tedious inventory management chore that is more work than the work I'm getting PAID to do in real life.",
	"My car has 400 HP and a filter gear shift!",
	"I want to be hurt by the estellen zippers!",
	"I am Connor MacLeod of the Clan MacLeod. I was born in 1518 in the village of Glenfinnan on the shores of Loch Shiel. And I am immortal.",
	"hewy, you wanna come over to my place? we can netflix and smite",
	"slip-on ass mummy rapping and we will be good to go", /* by someone who watched Tone's stream with subtitles */
	"Women are goddesses that have been created to drive men insane.", /* I (Amy) saw an actual RL t-shirt that said this */
	"I haven't watched One Piece, and therefore I'm controlling two devil fruits because I don't know that that's impossible.",
	"I petition that the gender star be renamed to gender snowflake",
	"the game is an acid trip where weapons become shoes and monsters become glitchymissingnos", /* by bug_sniper */
	"My cuten lovenails are nothing special, they just mean that I like the color purple.",
	"why does nethack have a tendency to attract dysfunctional people and programmer?", /* by bhaak */
	"can I get a wand of reverse wishing that makes something you have go poof?", /* by Demo */
	"basing a variant on SLEX and then removing all of SLEX specifics is almost as stupid as basing your variant on NitroHack without assessing the shitty code quality of NitroHack first", /* by bhaak */
	"I'm not afraid of corona virus",
	"I'm immune to corona virus!",
	"You can get closer than 1.50 m to me if you want, because I don't believe in corona hysteria anyway.",
	"Could I cheat and argue that it was not on purpose?",
	"I've decided that it would be unethical to cheat in this scenario.",
	"I'll never play in a NetHack tournament where the prizes are real money. This isn't Diablo 3, after all.",
	"Are you trying to bribe me with 5 euros to be a beta tester for your shit variant? Because fuck you, I'm not going to stoop that low. Go find some other fool to playtest your garbage.",
	"Rock 'n Mole",
	"Yendor Alchemical Academy, Class of '47",
	"Zombies aren't so tough. I'm more scared of an archon apocalypse.",
	"I only play SLEX, developed by Amy. Variants of SLEX developed by others are too easy, ascending those would mean nothing to me.",
	"I used all 8 charges in the wand of wishing despite the developers pleading me to pretend it only had 3!",
	"I can eat a slime mold", /* by mobileuser */
	"slex - seeing bugs as features since 2017", /* by K2 */

	    };
	    char buf[BUFSZ];
	    int erosion;

	    if (cant_see) {
		You_cant("feel any Braille writing.");
		return 0;
	    }
	    u.uconduct.literate++;
	    if(flags.verbose)
		pline("It reads:");
	    strcpy(buf, shirt_msgs[scroll->shirtmessage % SIZE(shirt_msgs)]);
	    erosion = greatest_erosion(scroll);
	    if (erosion)
		wipeout_text(buf,
			(int)(strlen(buf) * erosion / (2*MAX_ERODE)),
			     scroll->o_id ^ (unsigned)u.ubirthday);
	    pline("\"%s\"", buf);
	    return 1;
	} else if (scroll->otyp == PACK_OF_FLOPPIES) {
		use_floppies(scroll);
	} else if (scroll->oclass != SCROLL_CLASS
		&& scroll->oclass != SPBOOK_CLASS) {
	    pline(silly_thing_to, "read");
	    return(0);
	} else if (Blind && !scroll->dknown && scroll->otyp != SPE_BOOK_OF_THE_DEAD) {
	    const char *what = 0;
	    if (scroll->oclass == SPBOOK_CLASS)
		what = "mystic runes";
	    else if (!scroll->dknown)
		what = "formula on the scroll";
	    if (what) {
		pline("Being blind, you cannot read the %s.", what);
		return(0);
	    }
	} else if (cant_see && scroll->otyp != SPE_BOOK_OF_THE_DEAD && !scroll->dknown) {
	    if (scroll->oclass == SPBOOK_CLASS)
	    {
		You_cant("read the mystic runes in the invisible spellbook.");
		return(0);
	    }
	    else if (!scroll->dknown)
	    {
		You_cant("read the formula on the invisible scroll.");
		return(0);
	    }
	}

	/* Actions required to win the game aren't counted towards conduct */
	if (scroll->otyp != SPE_BOOK_OF_THE_DEAD &&
		scroll->otyp != SPE_BLANK_PAPER &&
#ifdef MAIL
		scroll->otyp != SCR_MAIL &&
#endif
		scroll->otyp != SCR_BLANK_PAPER)
	    u.uconduct.literate++;

	confused = (Confusion != 0);
#ifdef MAIL
	if (scroll->otyp == SCR_MAIL) confused = FALSE;
#endif
	if(scroll->oclass == SPBOOK_CLASS) {
		if (BookTrapEffect || u.uprops[BOOKBUG].extrinsic || have_bookstone()) {
		    pline("Suddenly you're very confused!");
		    make_confused(HConfusion + 2, FALSE);
		}
	    return(study_book(scroll));
	}
	scroll->in_use = TRUE;	/* scroll, not spellbook, now being read */

	if (Role_if(PM_CARTOMANCER) && scroll) {
		cartochance = 0;

		switch (scroll->otyp) { /* values depend on how much they cost to write, see write.c --Amy */

			case SCR_STANDARD_ID:
			case SCR_WOUNDS:
			case SCR_RUMOR:
			case SCR_MESSAGE:
				cartochance = 40;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 42; break;
					case P_SKILLED: cartochance = 44; break;
					case P_EXPERT: cartochance = 46; break;
					case P_MASTER: cartochance = 48; break;
					case P_GRAND_MASTER: cartochance = 50; break;
					case P_SUPREME_MASTER: cartochance = 52; break;
					default: break;
				}
				break;
			case SCR_HEALING:
			case SCR_LIGHT:
			case SCR_GOLD_DETECTION:
			case SCR_FOOD_DETECTION:
			case SCR_TRAP_DETECTION:
			case SCR_MAGIC_MAPPING:
			case SCR_AMNESIA:
			case SCR_INSTANT_AMNESIA:
			case SCR_FIRE:
			case SCR_SLEEP:
			case SCR_EARTH:
			case SCR_CURE_BLINDNESS:
			case SCR_ROOT_PASSWORD_DETECTION:
			case SCR_GRASSLAND:
				cartochance = 35;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 37; break;
					case P_SKILLED: cartochance = 39; break;
					case P_EXPERT: cartochance = 41; break;
					case P_MASTER: cartochance = 43; break;
					case P_GRAND_MASTER: cartochance = 45; break;
					case P_SUPREME_MASTER: cartochance = 47; break;
					default: break;
				}
				break;
			case SCR_MANA:
			case SCR_DESTROY_ARMOR:
			case SCR_DESTROY_WEAPON:
			case SCR_BAD_EFFECT:
			case SCR_CREATE_MONSTER:
			case SCR_CREATE_VICTIM:
			case SCR_SUMMON_UNDEAD:
			case SCR_PUNISHMENT:
			case SCR_NASTINESS:
			case SCR_SYMMETRY:
			case SCR_CREATE_CREATE_SCROLL:
			case SCR_PROOF_ARMOR:
			case SCR_PROOF_WEAPON:
			case SCR_CRYPT:
			case SCR_PAVING:
			case SCR_INFERIOR_MATERIAL:
			case SCR_CONFUSE_MONSTER:
			case SCR_PHASE_DOOR:
				cartochance = 30;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 32; break;
					case P_SKILLED: cartochance = 33; break;
					case P_EXPERT: cartochance = 35; break;
					case P_MASTER: cartochance = 36; break;
					case P_GRAND_MASTER: cartochance = 38; break;
					case P_SUPREME_MASTER: cartochance = 40; break;
					default: break;
				}
				break;
			case SCR_IDENTIFY:
			case SCR_STONING:
			case SCR_BULLSHIT:
			case SCR_SCARE_MONSTER:
			case SCR_SNOW:
			case SCR_SAND:
			case SCR_NETHER:
				cartochance = 28;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 30; break;
					case P_SKILLED: cartochance = 31; break;
					case P_EXPERT: cartochance = 33; break;
					case P_MASTER: cartochance = 34; break;
					case P_GRAND_MASTER: cartochance = 36; break;
					case P_SUPREME_MASTER: cartochance = 37; break;
					default: break;
				}
				break;
			case SCR_ASH:
			case SCR_BUBBLE_BOBBLE:
			case SCR_RAIN:
			case SCR_TAMING:
			case SCR_TELEPORTATION:
			case SCR_FLOOD:
			case SCR_LAVA:
			case SCR_GRAVE:
			case SCR_DIVING:
			case SCR_CRYSTALLIZATION:
			case SCR_QUICKSAND:
			case SCR_STYX:
			case SCR_URINE:
			case SCR_MOORLAND:
			case SCR_TUNNELS:
			case SCR_FARMING:
			case SCR_BARRHING:
			case SCR_STALACTITE:
			case SCR_GROWTH:
			case SCR_ICE:
			case SCR_ILLUSION:
			case SCR_FEMINISM:
			case SCR_EVIL_VARIANT:
			case SCR_ENRAGE:
			case SCR_FROST:
			case SCR_CLOUDS:
			case SCR_DETECT_WATER:
			case SCR_CHAOS_TERRAIN:
			case SCR_TELE_LEVEL:
			case SCR_WARPING:
			case SCR_IMMOBILITY:
			case SCR_MASS_MURDER:
			case SCR_TRAP_CREATION:
			case SCR_CREATE_TRAP:
			case SCR_GROUP_SUMMONING:
			case SCR_UNDO_GENOCIDE:
			case SCR_RANDOM_ENCHANTMENT:
			case SCR_BAD_EQUIPMENT:
			case SCR_HEAL_OTHER:
			case SCR_REGULAR_MATERIAL:
				cartochance = 25;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 27; break;
					case P_SKILLED: cartochance = 28; break;
					case P_EXPERT: cartochance = 30; break;
					case P_MASTER: cartochance = 31; break;
					case P_GRAND_MASTER: cartochance = 33; break;
					case P_SUPREME_MASTER: cartochance = 35; break;
					default: break;
				}
				break;
			case SCR_STINKING_CLOUD:
			case SCR_ENCHANT_ARMOR:
			case SCR_REMOVE_CURSE:
			case SCR_ENCHANT_WEAPON:
			case SCR_CHARGING:
			case SCR_GIRLINESS:
			case SCR_FLOODING:
			case SCR_EGOISM:
			case SCR_ERASURE:
			case SCR_ANTIMATTER:
			case SCR_MEGALOAD:
			case SCR_WONDER:
			case SCR_GEOLYSIS:
			case SCR_OFFLEVEL_ITEM:
			case SCR_REPAIR_ITEM:
			case SCR_EXTRA_HEALING:
			case SCR_MOUNTAINS:
			case SCR_HIGHWAY:
			case SCR_SYMBIOSIS:
				cartochance = 20;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 21; break;
					case P_SKILLED: cartochance = 22; break;
					case P_EXPERT: cartochance = 23; break;
					case P_MASTER: cartochance = 24; break;
					case P_GRAND_MASTER: cartochance = 25; break;
					case P_SUPREME_MASTER: cartochance = 26; break;
					default: break;
				}
				break;
			case SCR_RESISTANCE:
			case SCR_GENOCIDE:
			case SCR_CURE:
			case SCR_SIN:
			case SCR_ARMOR_SPECIALIZATION:
			case SCR_SUMMON_BOSS:
			case SCR_SUMMON_ELM:
			case SCR_DEMONOLOGY:
			case SCR_ELEMENTALISM:
			case SCR_TRAP_DISARMING:
			case SCR_FLOOD_TIDE:
			case SCR_EBB_TIDE:
			case SCR_MATERIAL_CHANGE:
			case SCR_CREATE_FACILITY:
			case SCR_SUMMON_GHOST:
			case SCR_GREATER_MANA_RESTORATION:
			case SCR_NASTY_CURSE:
			case SCR_TERRAFORMING:
				cartochance = 15;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 16; break;
					case P_SKILLED: cartochance = 17; break;
					case P_EXPERT: cartochance = 18; break;
					case P_MASTER: cartochance = 19; break;
					case P_GRAND_MASTER: cartochance = 20; break;
					case P_SUPREME_MASTER: cartochance = 21; break;
					default: break;
				}
				break;
			case SCR_GAIN_MANA:
			case SCR_LOCKOUT:
			case SCR_WARD:
			case SCR_CREATE_ALTAR:
			case SCR_WARDING:
			case SCR_RELOCATION:
			case SCR_VILENESS:
			case SCR_CREATE_FAMILIAR:
			case SCR_ITEM_GENOCIDE:
			case SCR_POWER_HEALING:
			case SCR_REVERSE_IDENTIFY:
			case SCR_SUPERIOR_MATERIAL:
				cartochance = 10;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 11; break;
					case P_SKILLED: cartochance = 12; break;
					case P_EXPERT: cartochance = 13; break;
					case P_MASTER: cartochance = 14; break;
					case P_GRAND_MASTER: cartochance = 15; break;
					case P_SUPREME_MASTER: cartochance = 16; break;
					default: break;
				}
				break;
			case SCR_CONSECRATION:
			case SCR_BOSS_COMPANION:
			case SCR_ANTIMAGIC:
			case SCR_SECURE_CURSE_REMOVAL:
			case SCR_INVENTORY_ID:
			case SCR_SKILL_UP:
			case SCR_ALTER_REALITY:
			case SCR_SECURE_IDENTIFY:
			case SCR_HYBRIDIZATION:
			case SCR_RAGNAROK:
				cartochance = 5;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 6; break;
					case P_SKILLED: cartochance = 7; break;
					case P_EXPERT: cartochance = 8; break;
					case P_MASTER: cartochance = 9; break;
					case P_GRAND_MASTER: cartochance = 10; break;
					case P_SUPREME_MASTER: cartochance = 11; break;
					default: break;
				}
				break;
			/* this bit is meant for hypothetical 100 ink scrolls, in case we add any --Amy
				cartochance = 1;
				if (!PlayerCannotUseSkills) switch (P_SKILL(P_DEVICES)) {
					case P_BASIC: cartochance = 1; break;
					case P_SKILLED: cartochance = 1; break;
					case P_EXPERT: cartochance = 2; break;
					case P_MASTER: cartochance = 2; break;
					case P_GRAND_MASTER: cartochance = 3; break;
					case P_SUPREME_MASTER: cartochance = 4; break;
					default: break;
				}
				break; */
			case SCR_WORLD_FALL:
			case SCR_ASTRALCENSION:
			case SCR_ARTIFACT_CREATION:
			case SCR_MISSING_CODE:
			case SCR_ARTIFACT_JACKPOT:
			case SCR_RESURRECTION:
			case SCR_ENTHRONIZATION:
			case SCR_WELL_BUILDING:
			case SCR_DRIVING:
			case SCR_TABLE_FURNITURE:
			case SCR_EMBEDDING:
			case SCR_MATTRESS_SLEEPING:
			case SCR_MAKE_PENTAGRAM:
			case SCR_FOUNTAIN_BUILDING:
			case SCR_SINKING:
			case SCR_CREATE_SINK:
			case SCR_WC:
				cartochance = 1;
				break;
			default:
				cartochance = 0;
				break;

		}

		if (cartochance > 0 && (rn2(100) < cartochance) ) cartokeep = TRUE;

	} /* cartomancer dupe chance check */

	if(scroll->oartifact == ART_MARAUDER_S_MAP) {
		if(Blind) {
			pline("Being blind, you cannot see the %s.", the(xname(scroll)));
			return 0;
		}
		pline("You examine %s.", the(xname(scroll)));
	} else if(scroll->otyp != SCR_BLANK_PAPER) {

	  if (cartokeep) {
		  You("manage to duplicate the scroll!");
		  if(confused) {
		    if (FunnyHallu)
			pline("Being so trippy, you screw up...");
		    else
			pline("Being confused, you mis%s the magic words...",
				is_silent(youmonst.data) ? "understand" : "pronounce");
		    u.cnd_confusedscrollread++;
		} /* confused reading */
	  } else {
		  if(Blind)
		    pline("As you %s the formula on it, the scroll disappears.", is_silent(youmonst.data) ? "cogitate" : "pronounce");
		  else
		    pline("As you read the scroll, it disappears.");
		  if(confused) {
		    if (FunnyHallu)
			pline("Being so trippy, you screw up...");
		    else
			pline("Being confused, you mis%s the magic words...",
				is_silent(youmonst.data) ? "understand" : "pronounce");
		    u.cnd_confusedscrollread++;
		} /* confused reading */
	   } /* cartokeep check */
	}

	if (CurseAsYouUse && scroll && scroll->otyp != CANDELABRUM_OF_INVOCATION && scroll->otyp != SPE_BOOK_OF_THE_DEAD && scroll->otyp != BELL_OF_OPENING) curse(scroll);

	/*
	 * When reading scrolls of teleportation off the floor special
	 * care needs to be taken so that the scroll is used up before
	 * a potential level teleport occurs.
	 */
	if (scroll->otyp == SCR_TELEPORTATION || scroll->otyp == SCR_ANTIMATTER || scroll->otyp == SCR_BAD_EFFECT || scroll->otyp == SCR_SIN || scroll->otyp == SCR_TELE_LEVEL || scroll->otyp == SCR_WARPING) {
	    otemp = *scroll;
	    otemp.where = OBJ_FREE;
	    otemp.nobj = (struct obj *)0;
	    if (carried(scroll)) useup(scroll);
	    else if (mcarried(scroll)) m_useup(scroll->ocarry, scroll);
	    else useupf(scroll, 1L);
	    scroll = &otemp;
	}

	if (!scroll) {
		impossible("used scroll disappeared before processing its effects?");
		return(1);
	}

	if (scroll && scroll->oclass == SCROLL_CLASS && scroll->otyp != SCR_CREATE_CREATE_SCROLL && !(scroll->oartifact == ART_MARAUDER_S_MAP)) {

		u.cnd_scrollcount++;

		use_skill(P_DEVICES,1);
		if (Race_if(PM_FAWN)) {
			use_skill(P_DEVICES,1);
		}
		if (Race_if(PM_SATRE)) {
			use_skill(P_DEVICES,1);
			use_skill(P_DEVICES,1);
		}
		if (Role_if(PM_INTEL_SCRIBE)) {
			use_skill(P_DEVICES,1);
			use_skill(P_DEVICES,1);
			use_skill(P_DEVICES,1);
		}

	}

	if(!seffects(scroll))  {

		if (!scroll) { /* scroll of copying "phantom crash bug"... --Amy */
			impossible("trying to finish used scroll that doesn't exist anymore?");
			return(1);
		}

		if(!objects[scroll->otyp].oc_name_known) {
		    if(known) {
			makeknown(scroll->otyp);
			more_experienced(0,10);
		    } else if(!objects[scroll->otyp].oc_uname && (scroll->otyp != SCR_INSTANT_AMNESIA) )
			docall(scroll);
		}
		if (spell_skilltype(scroll->otyp) != P_NONE) {
			use_skill(spell_skilltype(scroll->otyp), 
				(scroll->blessed ? 2 : 1));
		}
		if(!cartokeep && scroll->otyp != SCR_BLANK_PAPER && scroll->oartifact != ART_MARAUDER_S_MAP &&
		  scroll->otyp != SCR_TELEPORTATION && scroll->otyp != SCR_ANTIMATTER && scroll->otyp != SCR_BAD_EFFECT && scroll->otyp != SCR_SIN && scroll->otyp != SCR_TELE_LEVEL && scroll->otyp != SCR_WARPING) {
		    if (carried(scroll)) useup(scroll);
		    else if (mcarried(scroll)) m_useup(scroll->ocarry, scroll);
		    else useupf(scroll, 1L);
		}
		else scroll->in_use = FALSE;
	}
	return(1);
}

static void
stripspe(obj)
register struct obj *obj;
{
	if (obj->blessed) {
		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
	}
	else {
		if (obj->spe > 0) {
		    obj->spe = 0;
		    if (obj->otyp == OIL_LAMP || obj->otyp == BRASS_LANTERN)
			obj->age = 0;
		    Your("%s %s briefly.",xname(obj), otense(obj, "vibrate"));
		} else {
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
	}
}

static void
p_glow1(otmp)
register struct obj	*otmp;
{
	Your("%s %s briefly.", xname(otmp),
	     otense(otmp, Blind ? "vibrate" : "glow"));
}

static void
p_glow2(otmp,color)
register struct obj	*otmp;
register const char *color;
{
	Your("%s %s%s%s for a moment.",
		xname(otmp),
		otense(otmp, Blind ? "vibrate" : "glow"),
		Blind ? "" : " ",
		Blind ? nul : hcolor(color));
}

/* Is the object chargeable?  For purposes of inventory display; it is */
/* possible to be able to charge things for which this returns FALSE. */
boolean
is_chargeable(obj)
struct obj *obj;
{
	if (obj->oclass == WAND_CLASS || obj->oclass == SPBOOK_CLASS) return TRUE;
	/* known && !uname is possible after amnesia/mind flayer */
	if (obj->oclass == RING_CLASS)
	    return (boolean)(objects[obj->otyp].oc_charged &&
			(obj->known || objects[obj->otyp].oc_uname));
	if (obj->oclass == IMPLANT_CLASS)
	    return (boolean)(objects[obj->otyp].oc_charged &&
			(obj->known || objects[obj->otyp].oc_uname));
	if (is_lightsaber(obj))
	    return TRUE;
	if (obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP || obj->otyp == BRASS_LANTERN) return TRUE;
	if (is_weptool(obj))	/* specific check before general tools */
	    return FALSE;
	if (obj->oclass == TOOL_CLASS)
	    return (boolean)(objects[obj->otyp].oc_charged);
	return FALSE; /* why are weapons/armor considered charged anyway? */
}

boolean
is_enchantable(obj)
struct obj *obj;
{
	if (obj->oclass == ARMOR_CLASS || obj->oclass == WEAPON_CLASS || obj->oclass == BALL_CLASS || obj->oclass == GEM_CLASS || obj->oclass == CHAIN_CLASS || is_weptool(obj)) return TRUE;
	return FALSE;
}

/*
 * recharge an object; curse_bless is -1 if the recharging implement
 * was cursed, +1 if blessed, 0 otherwise.
 */
void
recharge(obj, curse_bless)
struct obj *obj;
int curse_bless;
{
	register int n;
	/*int enspe;*/
	boolean is_cursed, is_blessed;

	is_cursed = curse_bless < 0;
	is_blessed = curse_bless > 0;

	if (obj->oclass == WAND_CLASS) {

	    /* undo any prior cancellation, even when is_cursed */
	    if (obj->spe == -1) obj->spe = 0;

	    /*
	     * Recharging might cause wands to explode.
	     *	v = number of previous recharges
	     *	      v = percentage chance to explode on this attempt
	     *		      v = cumulative odds for exploding
	     *	0 :   0       0
	     *	1 :   0.29    0.29
	     *	2 :   2.33    2.62
	     *	3 :   7.87   10.28
	     *	4 :  18.66   27.02
	     *	5 :  36.44   53.62
	     *	6 :  62.97   82.83
	     *	7 : 100     100
	     */
	    n = (int)obj->recharged;
	    if (n > 0 && (obj->otyp == WAN_WISHING || obj->otyp == WAN_CHARGING || obj->otyp == WAN_ACQUIREMENT || obj->otyp == WAN_GAIN_LEVEL || obj->otyp == WAN_INCREASE_MAX_HITPOINTS ||
		/* no unlimited recharging of wands of charging --Amy */
		    ((n * n * n > rn2(7*7*7) && !(obj->oartifact == ART_EXTRA_CONTROL && Role_if(PM_FORM_CHANGER)) ) && ( Role_if(PM_WANDKEEPER) ? !rn2(10) : rn2(3)) ))) {	/* recharge_limit */
		Your("%s vibrates violently, and explodes!",xname(obj));
		wand_explode(obj, FALSE);
		return;
	    }
	    /* didn't explode, so increment the recharge count */
	    obj->recharged = (unsigned)(n + 1);

	    /* now handle the actual recharging */
	    if (is_cursed) {
		stripspe(obj);
	    } else {
		int lim = (obj->otyp == WAN_WISHING) ? 2 : (obj->otyp == WAN_GENOCIDE) ? 4 : (obj->otyp == WAN_GAIN_LEVEL) ? 5  : (obj->otyp == WAN_BAD_EQUIPMENT) ? 7 : (obj->otyp == WAN_INCREASE_MAX_HITPOINTS) ? 6 : ( (obj->otyp == WAN_CHARGING || obj->otyp == WAN_ACQUIREMENT ) ) ? 3 : ( (obj->otyp == WAN_PARALYSIS || obj->otyp == WAN_DISINTEGRATION || obj->otyp == WAN_DISINTEGRATION_BEAM || obj->otyp == WAN_STONING || obj->otyp == WAN_INERTIA || obj->otyp == WAN_TIME || obj->otyp == WAN_IDENTIFY || obj->otyp == WAN_REMOVE_CURSE || obj->otyp == WAN_TIME_STOP || obj->otyp == WAN_TELE_LEVEL || obj->otyp == WAN_ENTRAPPING || obj->otyp == WAN_MAGIC_MAPPING || obj->otyp == WAN_CREATE_FAMILIAR ) ) ? 5 : (obj->otyp == WAN_ENLIGHTENMENT || obj->otyp == WAN_TRAP_DISARMING || obj->otyp == WAN_CANCELLATION || obj->otyp == WAN_POLYMORPH || obj->otyp == WAN_MUTATION) ? 7 : (obj->otyp == WAN_VENOM_SCATTERING) ? 20 : (objects[obj->otyp].oc_dir == IMMEDIATE) ? (12 + n) : (objects[obj->otyp].oc_dir != NODIR) ? (10 + n) : (20 + n);

		n = (lim == 2) ? 2 : (lim == 3) ? 3 : (is_blessed ? rn1(5, lim + 1 - 5) : rnd(lim) ) ;
		/*if (!is_blessed) {enspe = rnd(n); n = enspe;}*/ /* no longer needed */

		if (obj->spe < n) { obj->spe = rnd(n);
		if (is_blessed && obj->spe < n && rn2(3) ) obj->spe = n;
		}

		if (Role_if(PM_WANDKEEPER) && obj->spe < 100) obj->spe += ((obj->otyp == WAN_WISHING) ? 1 : (obj->otyp == WAN_CHARGING || obj->otyp == WAN_BAD_EQUIPMENT || obj->otyp == WAN_ACQUIREMENT ) ? rnd(3) : (obj->otyp == WAN_GENOCIDE) ? rnd(4) : (obj->otyp == WAN_GAIN_LEVEL) ? rnd(5) : (obj->otyp == WAN_INCREASE_MAX_HITPOINTS) ? rnd(6) : rnd(10));

		/* let's make charging a bit more useful, considering wands spawn with less charges now --Amy */
		/*else*/ obj->spe += obj->recharged; /* cannot be higher than 1 for wishing/charging anyway */
		/*if (obj->otyp == WAN_WISHING && obj->spe > 3) {
		    Your("%s vibrates violently, and explodes!",xname(obj));
		    wand_explode(obj, FALSE);
		    return;
		}*/
		u.cnd_chargingcount++;
		if (obj->spe >= lim) p_glow2(obj, NH_BLUE);
		else p_glow1(obj);
		if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
			if (!obj->cursed) bless(obj);
			else uncurse(obj, FALSE);
		}
	    }

	} else if (obj->oclass == SPBOOK_CLASS) {

		/* some books are so powerful that charging them gives much fewer charges (similar to wand of wishing) --Amy */
		boolean fewchargesadded = FALSE;
		if (obj->otyp == SPE_TIME || obj->otyp == SPE_GAIN_LEVEL || obj->otyp == SPE_MAP_LEVEL || obj->otyp == SPE_INERTIA || obj->otyp == SPE_CHARGING || obj->otyp == SPE_GENOCIDE || obj->otyp == SPE_GODMODE || obj->otyp == SPE_CHARACTER_RECURSION || obj->otyp == SPE_PETRIFY || obj->otyp == SPE_ACQUIREMENT || obj->otyp == SPE_THRONE_GAMBLE || obj->otyp == SPE_WISHING || obj->otyp == SPE_WORLD_FALL || obj->otyp == SPE_REROLL_ARTIFACT || obj->otyp == SPE_ATTUNE_MAGIC || obj->otyp == SPE_GAIN_SPACT || obj->otyp == SPE_CLONE_MONSTER || obj->otyp == SPE_TIME_STOP || obj->otyp == SPE_ALTER_REALITY || obj->otyp == SPE_AULE_SMITHING) fewchargesadded = TRUE;

	    if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
	    	pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
		return;
	    }
   
	    /*
	     * Recharging might cause spellbooks to crumble.
	     *	 v = number of previous recharges
	     *	       v = percentage chance to crumble on this attempt
	     *	               v = cumulative odds for crumbling
	     *	 0 :   0       0
	     *	 1 :   0.15    0.15
	     *	 2 :   0.73    0.87
	     *	 3 :   1.97    2.82
	     *	 4 :   4.08    6.79
	     *	 5 :   7.29   13.58
	     *	 6 :  11.81   23.79
	     *	 7 :  17.86   37.40
	     *	 8 :  25.17   53.16
	     *	 9 :  33.03   68.63
	     *	10 :  40.68   81.39
	     *	11 :  47.64   90.26
	     *	12 :  53.72   95.49
	     *	13 :  58.90   98.15
	     *	14 :  63.29   99.32
	     */
	    n = (int)obj->recharged;
	    if (n > 0 && (Role_if(PM_LIBRARIAN) ? !rn2(10) : Role_if(PM_INTEL_SCRIBE) ? !rn2(10) : rn2(3)) && (n * n * n > rn2(7*7*7))) {	/* recharge_limit */
		Your("%s crumbles to dust!", xname(obj));
		useup(obj);
		    return;
		}
	    /* didn't crumble, so increment the recharge count */
	    obj->recharged = (unsigned)(n + 1);

	    /* now handle the actual recharging */
	    if (is_cursed) {
		stripspe(obj);
  	    } else {
		if (fewchargesadded) n = (is_blessed) ? rnd(2) : 1;
		else n = (is_blessed) ? rnd(5) : rnd(3);
		obj->spe += n;
		if (obj->spe > 100) obj->spe = 100; /* upper limit */
		if (obj->spe >= 5) p_glow2(obj, NH_BLUE);
		else p_glow1(obj);
		u.cnd_chargingcount++;
		if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
			if (!obj->cursed) bless(obj);
			else uncurse(obj, FALSE);
		}
	    }

	} else if (obj->oclass == RING_CLASS &&
					objects[obj->otyp].oc_charged) {
	    /* charging does not affect ring's curse/bless status */
	    int s = is_blessed ? rnd(3) : is_cursed ? -rnd(2) : 1;
	    boolean is_on = (obj == uleft || obj == uright);

	    /* destruction depends on current state, not adjustment; explosion change lowered by Amy */
	    if ((obj->spe > rn2(7) || obj->spe <= -5) && rn2(2)) {
		Your("%s %s momentarily, then %s!",
		     xname(obj), otense(obj,"pulsate"), otense(obj,"explode"));
		if (is_on) Ring_gone(obj);
		s = rnd(3 * abs(obj->spe));	/* amount of damage */
		useup(obj);
		losehp(s, "exploding ring", KILLED_BY_AN);
	    } else {
		long mask = is_on ? (obj == uleft ? LEFT_RING : RIGHT_RING) : 0L;
		Your("%s spins %sclockwise for a moment.",
		     xname(obj), s < 0 ? "counter" : "");
		/* cause attributes and/or properties to be updated */
		if (is_on) Ring_off(obj);
		obj->spe += s;	/* update the ring while it's off */
		if (s > 0) u.cnd_chargingcount++;
		if (s > 0 && obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
			if (!obj->cursed) bless(obj);
			else uncurse(obj, FALSE);
		}
		if (is_on) setworn(obj, mask), Ring_on(obj);
		/* oartifact: if a touch-sensitive artifact ring is
		   ever created the above will need to be revised  */
	    }

	} else if (obj->oclass == IMPLANT_CLASS && objects[obj->otyp].oc_charged) {

	    int s = is_blessed ? rnd(3) : is_cursed ? -rnd(2) : rnd(2);

	    if (((obj->spe > rn2(7) && obj->spe > rn2(7) && obj->spe > rn2(7)) || (obj->spe <= -5 && obj->spe <= -5)) && !rn2(4)) {
		Your("%s %s momentarily, then %s.", xname(obj), otense(obj,"pulsate"), otense(obj,"fade"));
		obj->spe = 0;

	    } else {
		if (((obj->spe + s) < 8) || !rn2(3)) { /* make it hard to reach ultra-high enchantment values --Amy */
			Your("%s spins %sclockwise for a moment.", xname(obj), s < 0 ? "counter" : "");
			obj->spe += s;	/* we don't need to take it off because it just affects AC and poly'd stuff */
			if (s > 0) u.cnd_chargingcount++;
			if (s > 0 && obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		} else {
			Your("%s seems unchanged.", xname(obj));
		}

	    }

	} else if (obj->oclass == TOOL_CLASS) {
	    int rechrg = (int)obj->recharged;

	    if (objects[obj->otyp].oc_charged) {
		/* tools don't have a limit, but the counter used does */
		if (rechrg < 7)	/* recharge_limit */
		    obj->recharged++;
	    }
	    switch(obj->otyp) {
	    case BELL_OF_OPENING:
		if (is_cursed) stripspe(obj);
		else if (is_blessed) obj->spe += rnd(30);
		else obj->spe += 10;
		if (obj->spe > 100) obj->spe = 100;
		if (!is_cursed) u.cnd_chargingcount++;
		if (!is_cursed && obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
			if (!obj->cursed) bless(obj);
			else uncurse(obj, FALSE);
		}

		break;
	    case MAGIC_MARKER:
		if (practicantterror) {
			pline("%s thunders: 'That's fraud, you're supposed to buy a new ink cartridge like everyone else when your pen is empty! That makes 5000 zorkmids!'", noroelaname());
			fineforpracticant(5000, 0, 0);
		}
		/* fall through */
	    case FELT_TIP_MARKER:
	    case TINNING_KIT:
	    case BINNING_KIT:
	    case EXPENSIVE_CAMERA:

		n = (int)obj->recharged;
		if (n > 0 && rn2(3) && (n * n * n > rn2(9*9*9))) {
			Your("%s glows violently and evaporates!", xname(obj));
			useup(obj);
		    return;
		}

		if (is_cursed) stripspe(obj);
		else if (rechrg && obj->otyp == MAGIC_MARKER && !(obj->oartifact == ART_MARKER_OF_SAFE_SPEECH && Role_if(PM_SOCIAL_JUSTICE_WARRIOR)) ) {	/* previously recharged */
		    obj->recharged = 1;	/* override increment done above */
		    if (obj->spe < 3)
			Your("marker seems permanently dried out.");
		    else {
				pline("%s", nothing_happens);
				if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
					pline("Oh wait, actually something bad happens...");
					badeffect();
				}
		    }
		} else if (is_blessed) {
		    n = rnd(30);		/* 15..30 */
		    if (rn2(2)) n += rnd(30);
		    if (!rn2(5)) n += rnd(50);

			int chrg = (int)obj->spe;
			if ((chrg + n) > 127)
				obj->spe = 127;
			else
				obj->spe += n;

			p_glow2(obj, NH_BLUE);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		} else {
		    n = rnd(20);		/* 10..20 */
		    if (rn2(2)) n += rnd(30);

			int chrg = (int)obj->spe;
			if ((chrg + n) > 127)
				obj->spe = 127;
			else
				obj->spe += n;

			p_glow2(obj, NH_WHITE);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		}
		break;
	    case OIL_LAMP:
	    case BRASS_LANTERN:

		if (is_cursed) {
		    stripspe(obj);
		    if (obj->lamplit) {
			if (!Blind)
			    pline("%s out!", Tobjnam(obj, "go"));
			end_burn(obj, TRUE);
		    }
		} else if (is_blessed) {
		    obj->spe = 1;
		    if (issoviet) {
				obj->age = 1500;
				pline("Vasha legkaya sablya ne zaryazhena pravil'no, potomu chto tip ledyanogo bloka nenavidit Emi i vse izmeneniya, kotoryye ona proizvodit. Yasno, chto slesh ikh vsegda budet vonyuchey kuchey der'ma.");
		    }
		    else obj->age += 1500;
		    p_glow2(obj, NH_BLUE);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		} else {
		    obj->spe = 1;
		    obj->age += 750;
		    if (issoviet && obj->age > 1500) {
				obj->age = 1500;
				pline("Vasha legkaya sablya ne zaryazhena pravil'no, potomu chto tip ledyanogo bloka nenavidit Emi i vse izmeneniya, kotoryye ona proizvodit. Yasno, chto slesh ikh vsegda budet vonyuchey kuchey der'ma.");
		    }
		    p_glow1(obj);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		}
		break;
	    case GREEN_LIGHTSABER:
	    case BLUE_LIGHTSABER:
	    case MYSTERY_LIGHTSABER:
	    case VIOLET_LIGHTSABER:
	    case WHITE_LIGHTSABER:
	    case YELLOW_LIGHTSABER:
	    case RED_LIGHTSABER:
	    case RED_DOUBLE_LIGHTSABER:
	    case WHITE_DOUBLE_LIGHTSABER:
	    case LASER_SWATTER:
	    case NANO_HAMMER:
	    case LIGHTWHIP:

		if (is_cursed) {
		    if (obj->lamplit) {
			end_burn(obj, TRUE);
			obj->age = 0;
			if (!Blind)
			    pline("%s deactivates!", The(xname(obj)));
		    } else
			obj->age = 0;
		} else if (is_blessed) {
		    if (issoviet) {
				obj->age = 1500;
				pline("Vasha legkaya sablya ne zaryazhena pravil'no, potomu chto tip ledyanogo bloka nenavidit Emi i vse izmeneniya, kotoryye ona proizvodit. Yasno, chto slesh ikh vsegda budet vonyuchey kuchey der'ma.");
		    }
		    else obj->age += 1500;
		    p_glow2(obj, NH_BLUE);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		} else {
		    obj->age += 750;
		    if (issoviet && obj->age > 1500) {
				obj->age = 1500;
				pline("Vasha legkaya sablya ne zaryazhena pravil'no, potomu chto tip ledyanogo bloka nenavidit Emi i vse izmeneniya, kotoryye ona proizvodit. Yasno, chto slesh ikh vsegda budet vonyuchey kuchey der'ma.");
		    }
		    p_glow1(obj);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		}
		break;
	    case CRYSTAL_BALL:

		n = (int)obj->recharged;
		if (n > 0 && rn2(3) && (n * n * n > rn2(9*9*9))) {
			Your("%s glows violently and evaporates!", xname(obj));
			useup(obj);
		    return;
		}

		if (is_cursed) stripspe(obj);
		else if (is_blessed) {
		    obj->spe = 6;
		    p_glow2(obj, NH_BLUE);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		} else {
		    if (obj->spe < 5) {
			obj->spe++;
			u.cnd_chargingcount++;
			p_glow1(obj);
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		    } else {
				pline("%s", nothing_happens);
				if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
					pline("Oh wait, actually something bad happens...");
					badeffect();
				}
		    }
		}
		break;

	    case CAN_OF_GREASE:
	    case LUBRICANT_CAN:

		n = (int)obj->recharged;
		if (n > 0 && rn2(3) && (n * n * n > rn2(9*9*9))) {
			Your("%s glows violently and evaporates!", xname(obj));
			useup(obj);
		    return;
		}

		if (is_cursed) stripspe(obj);
		else if (is_blessed) {
			obj->spe += rnd(30);
			if (obj->spe > 100) obj->spe = 100;
			p_glow2(obj, NH_BLUE);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		} else {
			obj->spe += rnd(20);
			if (obj->spe > 100) obj->spe = 100;
			p_glow1(obj);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		}
		break;

	    case HORN_OF_PLENTY:
	    case BAG_OF_TRICKS:
	    case CHEMISTRY_SET:

		n = (int)obj->recharged;
		if (n > 0 && rn2(3) && (n * n * n > rn2(9*9*9))) {
			Your("%s glows violently and evaporates!", xname(obj));
			useup(obj);
		    return;
		}

		if (is_cursed) stripspe(obj);
		else if (is_blessed) {

			obj->spe += rnd(10);

		    if (obj->spe > 117) obj->spe = 117;
		    p_glow2(obj, NH_BLUE);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		} else {
		    obj->spe += rnd(5);
		    if (obj->spe > 117) obj->spe = 117;
		    p_glow1(obj);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		}
		break;
	    case MAGIC_FLUTE:
	    case MAGIC_HARP:
	    case FROST_HORN:
	    case TEMPEST_HORN:
	    case CHROME_HORN:
	    case SHADOW_HORN:
	    case ETHER_HORN:
	    case FIRE_HORN:
	    case DRUM_OF_EARTHQUAKE:

		n = (int)obj->recharged;
		if (n > 0 && rn2(3) && (n * n * n > rn2(9*9*9))) {
			Your("%s glows violently and evaporates!", xname(obj));
			useup(obj);
		    return;
		}

		if (is_cursed) {
		    stripspe(obj);
		} else if (is_blessed) {
		    obj->spe += d(2,4);
		    if (obj->spe > 20) obj->spe = 20;
		    p_glow2(obj, NH_BLUE);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		} else {
		    obj->spe += rnd(4);
		    if (obj->spe > 20) obj->spe = 20;
		    p_glow1(obj);
			u.cnd_chargingcount++;
			if (obj && objects[(obj)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(obj)) {
				if (!obj->cursed) bless(obj);
				else uncurse(obj, FALSE);
			}
		}
		break;
	    default:
		goto not_chargable;
		/*NOTREACHED*/
		break;
	    } /* switch */

	} else {
 not_chargable:
	    You("have a feeling of loss.");
	}
}

void
randomenchant(obj, curse_bless, confused)
struct obj *obj;
int curse_bless;
boolean confused;
{
	register int n, positive;
	boolean is_cursed, is_blessed;

	is_cursed = curse_bless < 0;
	is_blessed = curse_bless > 0;

	if (!is_enchantable(obj)) {

		pline("You have a feeling of loss.");
		return;
	}

	if (stack_too_big(obj)) {

		pline("The enchantment failed because the stack was too big.");
		return;

	}

	n = 0;
	positive = rn2(2) ? 0 : 1;
	if (obj->spe < 1) n = obj->spe;
	if (is_cursed) n--;
	if (is_blessed) n++;
	if (positive) {
		n++;
		n += rne(2);
	} else {
		n--;
		n -= rne(2);
	}

	if (confused) {
		positive = rn2(2) ? 0 : 1;
		if (positive) {
			n++;
			n += rne(2);
		} else {
			n--;
			n -= rne(2);
		}
	}

	if (n < -127) n = -127;
	if (n > 127) n = 127;
	obj->spe = n;
	if (is_cursed) {
		curse(obj);
		pline("Your %s is surrounded by a black aura.", xname(obj));
	} else if (!confused) pline("Your %s is surrounded by a gray aura.", xname(obj));
	else pline("Your %s is surrounded by a sparkly aura.", xname(obj));

}

/* Forget known information about this object class. */
void
forget_single_object(obj_id)
	int obj_id;
{
	if (obj_id == RIN_MEMORY && rn2(6))
	    return;   /* does not want to be forgotten */
	objects[obj_id].oc_name_known = 0;
	objects[obj_id].oc_pre_discovered = 0;	/* a discovery when relearned */
	if (objects[obj_id].oc_uname) {
	    free((void *)objects[obj_id].oc_uname);
	    objects[obj_id].oc_uname = 0;
	}
	undiscover_object(obj_id);	/* after clearing oc_name_known */

	/* clear & free object names from matching inventory items too? */
}


#if 0	/* here if anyone wants it.... */
/* Forget everything known about a particular object class. */
static void
forget_objclass(oclass)
	int oclass;
{
	int i;

	for (i=bases[oclass];
		i < NUM_OBJECTS && objects[i].oc_class==oclass; i++)
	    forget_single_object(i);
}
#endif


/* randomize the given list of numbers  0 <= i < count */
static void
randomize(indices, count)
	int *indices;
	int count;
{
	int i, iswap, temp;

	for (i = count - 1; i > 0; i--) {
	    if ((iswap = rn2(i + 1)) == i) continue;
	    temp = indices[i];
	    indices[i] = indices[iswap];
	    indices[iswap] = temp;
	}
}


/* Forget % of known objects. */
void
forget_objects(percent)
	int percent;
{
	int i, count;
	int indices[NUM_OBJECTS];

	if (Keen_memory && rn2(StrongKeen_memory ? 20 : 4)) return;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_UNFORGETTABLE_EVENT && rn2(10)) return;
	if (Role_if(PM_MASTERMIND) && mastermindsave()) return;

	if (isfriday) {
		percent *= 2;
		if (percent > 100) percent = 100;
	}

	if (percent == 0) return;
	if (percent <= 0 || percent > 100) {
	    impossible("forget_objects: bad percent %d", percent);
	    return;
	}

	for (count = 0, i = 1; i < NUM_OBJECTS; i++)
	    if (OBJ_DESCR(objects[i]) &&
		    (objects[i].oc_name_known || objects[i].oc_uname))
		indices[count++] = i;

	randomize(indices, count);

	/* forget first % of randomized indices */
	count = ((count * percent) + 50) / 100;
	if (count > 1) count /= 2; /* another nerf by Amy */
	for (i = 0; i < count; i++)
	    forget_single_object(indices[i]);

	if (!rn2(issoviet ? 25 : 5)) {

		initobjectsamnesia();
		if (wizard) pline("init_objects done!");
		if (issoviet) pline("Tip bloka l'da reshil randomizirovat' vse! Khi khi!");

	}

}


/* Forget some or all of map (depends on parameters). */
void
forget_map(howmuch)
	int howmuch;
{
	register int zx, zy;

	/*if (In_sokoban(&u.uz) && rn2(20) )
	    return;*/

	if (Keen_memory && rn2(StrongKeen_memory ? 20 : 4)) return;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_UNFORGETTABLE_EVENT && rn2(10)) return;
	if (Role_if(PM_MASTERMIND) && mastermindsave()) return;

	known = TRUE;
	for(zx = 0; zx < COLNO; zx++) for(zy = 0; zy < ROWNO; zy++)
	    if (howmuch & ALL_MAP || isfriday || rn2(7)) {
		/* Zonk all memory of this location. */
		levl[zx][zy].seenv = 0;
		levl[zx][zy].waslit = 0;
		clear_memory_glyph(zx, zy, S_stone);
	    }
}

/* for AD_AMNE: forget random percentage of current map --Amy */
void
maprot()
{
	register int zx, zy;
	register int percentage = rnd(100);

	if (isfriday) {
		percentage *= 2;
		if (percentage > 100) percentage = 100;
	}

	if (Role_if(PM_MASTERMIND) && mastermindsave()) {
		percentage /= 10;
		if (percentage < 1) return;
	}

	if (Keen_memory && rn2(StrongKeen_memory ? 20 : 4)) {
		percentage /= 10;
		if (percentage < 1) return;
	}

	for(zx = 0; zx < COLNO; zx++) for(zy = 0; zy < ROWNO; zy++)
	    if (rn2(100) < percentage) {
		/* Zonk all memory of this location. */
		levl[zx][zy].seenv = 0;
		levl[zx][zy].waslit = 0;
		clear_memory_glyph(zx, zy, S_stone);
	    }
	docrt();		/* this correctly will reset vision */
}

/* Forget all traps on the level. */
void
forget_traps()
{
	register struct trap *trap;

	if (Keen_memory && rn2(StrongKeen_memory ? 20 : 4)) return;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_UNFORGETTABLE_EVENT && rn2(10)) return;
	if (Role_if(PM_MASTERMIND) && mastermindsave()) return;

	/* forget all traps (except the one the hero is in :-) */
	for (trap = ftrap; trap; trap = trap->ntrap)
	    if ((trap->tx != u.ux || trap->ty != u.uy) && (trap->ttyp != HOLE))
		trap->tseen = 0;
}

/*
 * Forget given % of all levels that the hero has visited and not forgotten,
 * except this one.
 */
void
forget_levels(percent)
	int percent;
{
	int i, count;
	xchar  maxl, this_lev;
	int indices[MAXLINFO];

	if (percent == 0) return;

	if (isfriday) {
		percent *= 2;
		if (percent > 100) percent = 100;
	}

	if (Keen_memory && rn2(StrongKeen_memory ? 20 : 4)) return;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_UNFORGETTABLE_EVENT && rn2(10)) return;
	if (Role_if(PM_MASTERMIND) && mastermindsave()) return;

	if (percent <= 0 || percent > 100) {
	    impossible("forget_levels: bad percent %d", percent);
	    return;
	}

	this_lev = ledger_no(&u.uz);
	maxl = maxledgerno();

	/* count & save indices of non-forgotten visited levels */
	/* Sokoban levels are pre-mapped for the player, and should stay
	 * so, or they become nearly impossible to solve.  But try to
	 * shift the forgetting elsewhere by fiddling with percent
	 * instead of forgetting fewer levels.
	 */
	for (count = 0, i = 0; i <= maxl; i++)
	    if ((level_info[i].flags & VISITED) &&
			!(level_info[i].flags & FORGOTTEN) && i != this_lev) {
		if ((ledger_to_dnum(i) == sokoban_dnum) && rn2(5)) /* evil patch idea by Amy: sometimes delete soko levels */
		    percent += 2;
		else
		    indices[count++] = i;
	    }
	
	if (percent > 100) percent = 100;

	randomize(indices, count);

	/* forget first % of randomized indices */
	count = ((count * percent) + 50) / 100;
	if (count > 1) count /= 2; /* another nerf by Amy */
	for (i = 0; i < count; i++) {
	    level_info[indices[i]].flags |= FORGOTTEN;
	    forget_mapseen(indices[i]);
	}
}

/*
 * Forget some things (e.g. after reading a scroll of amnesia).  When called,
 * the following are always forgotten:
 *
 *	- felt ball & chain
 *	- traps
 *	- part (6 out of 7) of the map
 *
 * Other things are subject to flags:
 *
 *	howmuch & ALL_MAP	= forget whole map
 *	howmuch & ALL_SPELLS	= forget all spells
 */
void
forget(howmuch)
int howmuch;
{

	/* Amnesia can make you forget contamination, but only if it's less than fatal --Amy */
	if (u.contamination && u.contamination < 1000) {
		decontaminate(100);
	}

	u.cnd_amnesiacount++;

	reducesanity(100);

	if (Punished) u.bc_felt = 0;	/* forget felt ball&chain */

	forget_map(howmuch);
	forget_traps();

	/* 1 in 3 chance of forgetting some levels */
	if (!rn2(issoviet ? 2 : 3)) forget_levels(rnd(issoviet ? 25 : 10));

	/* 1 in 5 chance of forgeting some objects */
	if (!rn2(issoviet ? 3 : 5)) forget_objects(rnd(issoviet ? 25 : 10));

	if (howmuch & ALL_SPELLS) losespells();
	/*
	 * Make sure that what was seen is restored correctly.  To do this,
	 * we need to go blind for an instant --- turn off the display,
	 * then restart it.  All this work is needed to correctly handle
	 * walls which are stone on one side and wall on the other.  Turning
	 * off the seen bits above will make the wall revert to stone,  but
	 * there are cases where we don't want this to happen.  The easiest
	 * thing to do is to run it through the vision system again, which
	 * is always correct.
	 */
	docrt();		/* this correctly will reset vision */
}

/* monster is hit by scroll of taming's effect */
static void
maybe_tame(mtmp, sobj)
struct monst *mtmp;
struct obj *sobj;
{
	if (sobj->cursed || Is_blackmarket(&u.uz)) {
	    setmangry(mtmp);
	} else {
	    if (mtmp->isshk)
		make_happy_shk(mtmp, FALSE);
	    else if (!resist(mtmp, sobj->oclass, 0, NOTELL))
		(void) tamedog(mtmp, (struct obj *) 0, FALSE);
	}
}

/** Remove water tile at x,y. */
STATIC_PTR void
undo_flood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if ((levl[x][y].typ != POOL) &&
	    (levl[x][y].typ != MOAT) &&
	    (levl[x][y].typ != WATER) &&
	    (levl[x][y].typ != FOUNTAIN))
		return;

	(*(int *)roomcnt)++;

	/* Get rid of a pool at x, y */
	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_flood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = POOL;
			del_engr_at(randomx, randomy);
			water_damage(level.objects[randomx][randomy], FALSE, TRUE);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = POOL;
		del_engr_at(x, y);
		water_damage(level.objects[x][y], FALSE, TRUE);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_lavaflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != LAVAPOOL)
		return;

	(*(int *)roomcnt)++;

	/* Get rid of a lava pool at x, y */
	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
terraincleanupA(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (Is_waterlevel(&u.uz)) return;

	if (levl[x][y].typ < GRAVEWALL)
		return;
	if (levl[x][y].typ >= SDOOR && levl[x][y].typ <= SCORR)
		return;
	if ((levl[x][y].wall_info & W_NONDIGGABLE) != 0)
		return;
	if (levl[x][y].typ == DRAWBRIDGE_UP || levl[x][y].typ == DRAWBRIDGE_DOWN)
		return;
	if (levl[x][y].typ >= DOOR && levl[x][y].typ <= STRAWMATTRESS)
		return;

	(*(int *)roomcnt)++;

	/* Get rid of stone at x, y */
	levl[x][y].typ = CORR;
	blockorunblock_point(x,y);
	if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;
	newsym(x,y);
}

STATIC_PTR void
do_lavaflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = LAVAPOOL;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = LAVAPOOL;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_megaflooding(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			if (rn2(4)) {
				levl[randomx][randomy].typ = MOAT;
				makemon(mkclass(S_EEL,0), randomx, randomy, NO_MM_FLAGS);
			} else {
				levl[randomx][randomy].typ = LAVAPOOL;
				makemon(mkclass(S_FLYFISH,0), randomx, randomy, NO_MM_FLAGS);
			}

			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */

		if (rn2(4)) {
			levl[x][y].typ = MOAT;
			makemon(mkclass(S_EEL,0), x, y, NO_MM_FLAGS);
		} else {
			levl[x][y].typ = LAVAPOOL;
			makemon(mkclass(S_FLYFISH,0), x, y, NO_MM_FLAGS);
		}

		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

	u.aggravation = 0;

}

STATIC_PTR void
undo_lockflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != STONE)
		return;
	if ((levl[x][y].wall_info & W_NONDIGGABLE) != 0)
		return;

	if (*in_rooms(x,y,SHOPBASE)) return;

	(*(int *)roomcnt)++;

	/* Get rid of stone at x, y */
	levl[x][y].typ = CORR;
	blockorunblock_point(x,y);
	if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;
	newsym(x,y);
}

STATIC_PTR void
undo_graveflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != GRAVEWALL)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	blockorunblock_point(x,y);
	newsym(x,y);
}

STATIC_PTR void
do_graveflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = GRAVEWALL;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = GRAVEWALL;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_tunnelflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != TUNNELWALL)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
	blockorunblock_point(x,y);
}

STATIC_PTR void
do_tunnelflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = TUNNELWALL;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = TUNNELWALL;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_farmflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != FARMLAND)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
	blockorunblock_point(x,y);
}

STATIC_PTR void
do_farmflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = FARMLAND;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = FARMLAND;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_mountainflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != MOUNTAIN)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
	blockorunblock_point(x,y);
}

STATIC_PTR void
do_mountainflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = MOUNTAIN;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = MOUNTAIN;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_watertunnelflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != WATERTUNNEL)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
	blockorunblock_point(x,y);
}

STATIC_PTR void
do_watertunnelflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = WATERTUNNEL;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = WATERTUNNEL;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_crystalwaterflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != CRYSTALWATER)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_crystalwaterflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = CRYSTALWATER;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CRYSTALWATER;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_moorflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != MOORLAND)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_moorflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = MOORLAND;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = MOORLAND;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_urineflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != URINELAKE)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_urineflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = URINELAKE;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = URINELAKE;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_shiftingsandflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != SHIFTINGSAND)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_shiftingsandflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = SHIFTINGSAND;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = SHIFTINGSAND;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_styxflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != STYXRIVER)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_styxflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = STYXRIVER;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = STYXRIVER;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_snowflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != SNOW)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_snowflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = SNOW;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = SNOW;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_ashflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != ASH)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_ashflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = ASH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = ASH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_sandflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != SAND)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_sandflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = SAND;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = SAND;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_pavementflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != PAVEDFLOOR)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_pavementflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = PAVEDFLOOR;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = PAVEDFLOOR;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_highwayflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != HIGHWAY)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_highwayflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = HIGHWAY;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = HIGHWAY;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_grassflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != GRASSLAND)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_grassflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = GRASSLAND;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = GRASSLAND;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_nethermistflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != NETHERMIST)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_nethermistflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = NETHERMIST;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = NETHERMIST;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_stalactiteflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != STALACTITE)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_stalactiteflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = STALACTITE;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = STALACTITE;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_cryptflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != CRYPTFLOOR)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_cryptflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = CRYPTFLOOR;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CRYPTFLOOR;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_bubbleflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != BUBBLES)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
	blockorunblock_point(x,y);
}

STATIC_PTR void
do_bubbleflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = BUBBLES;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = BUBBLES;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_raincloudflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != RAINCLOUD)
		return;

	(*(int *)roomcnt)++;

	levl[x][y].typ = ROOM;
	newsym(x,y);
	blockorunblock_point(x,y);
}

STATIC_PTR void
do_raincloudflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = RAINCLOUD;
			del_engr_at(randomx, randomy);
			blockorunblock_point(randomx,randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = RAINCLOUD;
		del_engr_at(x, y);
		blockorunblock_point(x,y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_lockflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && ((levl[randomx][randomy].wall_info & W_NONDIGGABLE) == 0) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR || (levl[randomx][randomy].typ == DOOR && levl[randomx][randomy].doormask == D_NODOOR) ) ) {

			if (rn2(3)) doorlockX(randomx, randomy, TRUE);
			else {
				if (levl[randomx][randomy].typ != DOOR) levl[randomx][randomy].typ = STONE;
				else levl[randomx][randomy].typ = CROSSWALL;
				blockorunblock_point(randomx,randomy);
				if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
				del_engr_at(randomx, randomy);

				if ((mtmp = m_at(randomx, randomy)) != 0) {
					(void) minliquid(mtmp);
				} else {
					newsym(randomx,randomy);
				}

			}
		}
	}

	if (rn2(3)) doorlockX(x, y, TRUE);

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].wall_info & W_NONDIGGABLE) != 0 || (levl[x][y].typ != CORR && levl[x][y].typ != ROOM && (levl[x][y].typ != DOOR || levl[x][y].doormask != D_NODOOR) ))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a wall at x, y */
		if (levl[x][y].typ != DOOR) levl[x][y].typ = STONE;
		else levl[x][y].typ = CROSSWALL;
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_treeflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != TREE)
		return;

	(*(int *)roomcnt)++;

	/* Get rid of a lava pool at x, y */
	levl[x][y].typ = ROOM;
	blockorunblock_point(x,y);
	if (!(levl[x][y].wall_info & W_HARDGROWTH)) levl[x][y].wall_info |= W_EASYGROWTH;
	newsym(x,y);
}

STATIC_PTR void
do_treeflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = TREE;
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = TREE;
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_iceflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != ICE)
		return;

	(*(int *)roomcnt)++;

	/* Get rid of a lava pool at x, y */
	levl[x][y].typ = ROOM;
	newsym(x,y);
}

STATIC_PTR void
do_iceflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = ICE;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = ICE;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_cloudflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != CLOUD)
		return;

	(*(int *)roomcnt)++;

	/* Get rid of a cloud at x, y */
	levl[x][y].typ = ROOM;
	blockorunblock_point(x,y);
	newsym(x,y);
}

STATIC_PTR void
do_cloudflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = CLOUD;
			blockorunblock_point(randomx,randomy);
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CLOUD;
		blockorunblock_point(x,y);
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
undo_barflood(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != IRONBARS)
		return;

	(*(int *)roomcnt)++;

	/* Get rid of bars at x, y */
	levl[x][y].typ = ROOM;
	blockorunblock_point(x,y);
	newsym(x,y);
}

STATIC_PTR void
do_barflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = IRONBARS;
			blockorunblock_point(randomx,randomy);
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = IRONBARS;
		blockorunblock_point(x,y);
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}


STATIC_PTR void
do_terrainflood(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = randomwalltype();
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = randomwalltype();
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

int
seffects(sobj)
register struct obj	*sobj;
{
	register int cval;
	register boolean confused = (Confusion != 0);
	register struct obj *otmp, *acqo;

	if (objects[sobj->otyp].oc_magic)
		exercise(A_WIS, TRUE);		/* just for trying */

	if (ConfusionProblem || u.uprops[CONFUSION_PROBLEM].extrinsic || (uarmc && uarmc->oartifact == ART_WINDS_OF_CHANGE) || (ublindf && ublindf->oartifact == ART_BLINDFOLD_OF_MISPELLING) || have_confusionstone() ) {

	    pline("Suddenly you're very confused!");
	    make_confused(HConfusion + 2, FALSE);
	    pline("You screw up while reading the scroll...");
	    confused = 1;

	}

	if (evilfriday && (ABASE(A_INT) < rnd(8))) {
	    pline("Due to your stupidity, you fail to read the scroll correctly!");
	    make_confused(HConfusion + 2, FALSE);
	    confused = 1;

	}

	if (sobj->otyp == SCR_COPYING) {

		struct obj *wonderscroll;
		wonderscroll = mkobj(SCROLL_CLASS,FALSE, FALSE);
		if (wonderscroll) sobj->otyp = wonderscroll->otyp;
		if (sobj->otyp == GOLD_PIECE) sobj->otyp = SCR_RUMOR; /* minimalist fix */
		if (wonderscroll) obfree(wonderscroll, (struct obj *)0);

	}

	switch(sobj->otyp) {
#ifdef MAIL
	case SCR_MAIL:
		known = TRUE;
		if (sobj->spe)
		    pline("This seems to be junk mail addressed to the finder of the Eye of Larn.");
		/* note to the puzzled: the game Larn actually sends you junk
		 * mail if you win!
		 */
		/*else*/ readmail(sobj);
		break;
#endif
	case SPE_ENCHANT_ARMOR:
		if (confused) break;
	case SCR_ENCHANT_ARMOR:
	    {
		register schar s;
		boolean special_armor;
		boolean same_color;

		pline("You may enchant a worn piece of armor.");
enchantarmorchoice:
		otmp = getobj(all_count, "magically enchant");
		/*otmp = some_armor(&youmonst);*/

		if(!otmp) {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to enchant your armor.");
			else goto enchantarmorchoice;
			strange_feeling(sobj,
					!Blind ? "Your skin glows then fades." :
					"Your skin feels warm for a moment.");
			exercise(A_CON, !sobj->cursed);
			exercise(A_STR, !sobj->cursed);
			return(1);
		}
		if (!(otmp->owornmask & W_ARMOR) ) {

			strange_feeling(sobj, "You have a feeling of loss.");
			return(1);
		}

		if(confused) {
			otmp->oerodeproof = !(sobj->cursed);
			if(Blind) {
			    otmp->rknown = FALSE;
			    Your("%s %s warm for a moment.",
				xname(otmp), otense(otmp, "feel"));
			} else {
			    otmp->rknown = TRUE;
			    Your("%s %s covered by a %s %s %s!",
				xname(otmp), otense(otmp, "are"),
				sobj->cursed ? "mottled" : "shimmering",
				 hcolor(sobj->cursed ? NH_BLACK : NH_GOLDEN),
				sobj->cursed ? "glow" :
				  (is_shield(otmp) ? "layer" : "shield"));
			}
			if (otmp->oerodeproof &&
			    (otmp->oeroded || otmp->oeroded2)) {
			    otmp->oeroded = otmp->oeroded2 = 0;
			    Your("%s %s as good as new!",
				 xname(otmp),
				 otense(otmp, Blind ? "feel" : "look"));
			}

			if (!(sobj->cursed)) {
				if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
					if (!otmp->cursed) bless(otmp);
					else uncurse(otmp, FALSE);
				}

			}

			break;
		}
		/* elven armor vibrates warningly when enchanted beyond a limit */
		special_armor = is_elven_armor(otmp) || otmp->otyp == KYRT_SHIRT ||
		  (Role_if(PM_WIZARD) && otmp->otyp == CORNUTHAUM) ||
		  (Role_if(PM_VALKYRIE) && otmp->otyp == GAUNTLETS_OF_POWER);

		if (sobj->cursed)
		    same_color =
			(otmp->otyp == BLACK_DRAGON_SCALE_MAIL ||
			 otmp->otyp == BLACK_DRAGON_SCALES);
		else
		    same_color =
			(otmp->otyp == SILVER_DRAGON_SCALE_MAIL ||
			 otmp->otyp == SILVER_DRAGON_SCALES ||
			 otmp->otyp == SHIELD_OF_REFLECTION);
		if (Blind) same_color = FALSE;

		/* KMH -- catch underflow */
		s = sobj->cursed ? -otmp->spe : otmp->spe;

		if (s > (is_droven_armor(otmp) ? 8 : special_armor ? 5 : 3) && rn2(s) && !rn2(3) )  {

			if (otmp->oartifact) {
				otmp->spe = 0;
				Your("%s violently %s%s%s for a while, then %s.", xname(otmp),
				otense(otmp, Blind ? "vibrate" : "glow"),
			     (!Blind && !same_color) ? " " : nul,
			     (Blind || same_color) ? nul : hcolor(sobj->cursed ? NH_BLACK : NH_SILVER),
				otense(otmp, "fade"));

				break;
			}

		Your("%s violently %s%s%s for a while, then %s.",
		     xname(otmp),
		     otense(otmp, Blind ? "vibrate" : "glow"),
		     (!Blind && !same_color) ? " " : nul,
		     (Blind || same_color) ? nul :
			hcolor(sobj->cursed ? NH_BLACK : NH_SILVER),
		     otense(otmp, "evaporate"));
			if(is_cloak(otmp)) (void) Cloak_off();
			if(is_boots(otmp)) (void) Boots_off();
			if(is_helmet(otmp)) (void) Helmet_off();
			if(is_gloves(otmp)) (void) Gloves_off();
			if(is_shield(otmp)) (void) Shield_off();
			if(otmp == uarm) (void) Armor_gone();
			useup(otmp);
			break;
		}

		if (!(sobj->cursed) && otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
			if (!otmp->cursed) bless(otmp);
			else uncurse(otmp, FALSE);
		}


		/* KMH, balance patch -- Restore the NetHack success rate */
		/* We'll introduce a disenchantment attack later */
		s = sobj->cursed ? -1 :
		/* Come on, the evaporation chance is nasty enough. Let's allow enchanting stuff beyond +9. --Amy */
		    otmp->spe >= 9 ? /*(rn2(otmp->spe) == 0)*/1 :
		    sobj->blessed ? rnd(3-otmp->spe/3) : 1;
/*		s = sobj->cursed ? -rnd(2) :
		    otmp->spe >= 3 ? (rn2(otmp->spe) == 0) :
		    sobj->blessed ? rnd(2) : 1;*/
		if (s >= 0 && otmp->otyp >= GRAY_DRAGON_SCALES &&
					otmp->otyp <= YELLOW_DRAGON_SCALES) {
			/* dragon scales get turned into dragon scale mail */
			Your("%s merges and hardens!", xname(otmp));
			setworn((struct obj *)0, W_ARM);
			/* assumes same order */
			otmp->otyp = GRAY_DRAGON_SCALE_MAIL +
						otmp->otyp - GRAY_DRAGON_SCALES;

			if ((otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(100) ) {
				otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
			}
			else if (otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(10) ) {
				otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
			}
			else if (!(otmp->prmcurse) && otmp->hvycurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(3) ) {
				otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
			}
			else if (!(otmp->prmcurse) && !(otmp->hvycurse) && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) ) otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;

			if (sobj->blessed) {
				otmp->spe++;
				otmp->blessed = 1;
			}
			otmp->known = 1;
			setworn(otmp, W_ARM);
			break;
		}
		if (s >= 0 && otmp->otyp == LIZARD_SCALES) {
			Your("%s merges and hardens!", xname(otmp));
			setworn((struct obj *)0, W_ARM);
			otmp->otyp = LIZARD_SCALE_MAIL;

			if ((otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(100) ) {
				otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
			}
			else if (otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(10) ) {
				otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
			}
			else if (!(otmp->prmcurse) && otmp->hvycurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(3) ) {
				otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;
			}
			else if (!(otmp->prmcurse) && !(otmp->hvycurse) && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) ) otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->stckcurse = 0;

			if (sobj->blessed) {
				otmp->spe++;
				otmp->blessed = 1;
			}
			otmp->known = 1;
			setworn(otmp, W_ARM);
			break;
		}

		Your("%s %s%s%s%s for a %s.",
			xname(otmp),
		        s == 0 ? "violently " : nul,
			otense(otmp, Blind ? "vibrate" : "glow"),
			(!Blind && !same_color) ? " " : nul,
			(Blind || same_color) ? nul : hcolor(sobj->cursed ? NH_BLACK : NH_SILVER),
			  (s*s>1) ? "while" : "moment");
		otmp->cursed = (sobj->cursed || ((otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && rn2(100)) || (otmp->prmcurse && rn2(10)) || (otmp->hvycurse && rn2(3)) ) ;
		if ((!otmp->blessed || sobj->cursed) && !otmp->cursed)
			otmp->blessed = sobj->blessed;
		if (s) {
			otmp->spe += s;
			if (Race_if(PM_SPARD) && s > 0) otmp->spe++;
			known = otmp->known;
		}
		/* sometimes, enchanting armor pieces may give them an actual magical enchantment --Amy */
		if (!otmp->enchantment && !rn2(sobj->blessed ? 10 : 20) && s > 0) {

			long savewornmask;
			otmp->enchantment = randenchantment();
			pline("Your %s seems to have gained special magical properties!", xname(otmp) );
			savewornmask = otmp->owornmask;
			setworn((struct obj *)0, otmp->owornmask);
			setworn(otmp, savewornmask);

		}

		if ((otmp->spe > (is_droven_armor(otmp) ? 8 : special_armor ? 5 : 3)) &&
		    (special_armor || !rn2(7)))
			Your("%s suddenly %s %s.",
				xname(otmp), otense(otmp, "vibrate"),
				Blind ? "again" : "unexpectedly");

		if (practicantterror && otmp && otmp->spe >= 5 && !u.pract_enchantarmor) {
			pline("%s rings out: 'I told you that you may not disguise as a tank! Just for that it costs 1000 zorkmids and 1000 stones now.'", noroelaname());
			fineforpracticant(1000, 1000, 0);
			u.pract_enchantarmor = TRUE;
		}

		break;
	    }
	case SCR_REPAIR_ITEM:
	    {
		pline("You may repair a damaged item.");
repairitemchoice:
		otmp = getobj(all_count, "magically repair");
		if (!otmp) {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to repair your items.");
			else goto repairitemchoice;
			pline("A feeling of loss comes over you.");
			break;
		}
		if (otmp && stack_too_big(otmp)) {
			pline("The stack was too big and therefore didn't get repaired!");
		} else if (otmp && confused) {
			if (!Blind) {
				pline("Your %s looks like it might fall apart if you sneeze at it!",xname(otmp));
			}
			otmp->oeroded = 3;
			otmp->oeroded2 = 3;
		} else if (otmp && greatest_erosion(otmp) > 0) {
			if (!Blind) {
				pline("Your %s looks as good as new!",xname(otmp));
			}
			if (otmp->oeroded > 0) { otmp->oeroded = 0; }
			if (otmp->oeroded2 > 0) { otmp->oeroded2 = 0; }
			if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
				if (!otmp->cursed) bless(otmp);
				else uncurse(otmp, FALSE);
			}

		} else pline("Your %s is still as undamaged as ever.",xname(otmp));

	    }
		break;

	case SPE_DESTROY_ARMOR:
		if (confused) break;
	case SCR_DESTROY_ARMOR:
	    {
		otmp = some_armor(&youmonst);

		if (sobj && sobj->otyp == SCR_DESTROY_ARMOR && sobj->blessed) { /* idea by bhaak */
			pline("You may enchant a worn piece of armor.");
destroyarmorchoice:
			otmp = getobj(all_count, "magically enchant");

			if (otmp && !(otmp->owornmask & W_ARMOR)) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to enchant your armor.");
				else goto destroyarmorchoice;
				strange_feeling(sobj,"Your skin itches.");
				exercise(A_STR, FALSE);
				exercise(A_CON, FALSE);
				break;
			}

		}

		if(confused) {
			if(!otmp) {
				strange_feeling(sobj,"Your bones itch.");
				exercise(A_STR, FALSE);
				exercise(A_CON, FALSE);
				return(1);
			}
			otmp->oerodeproof = sobj->cursed;
			p_glow2(otmp, NH_PURPLE);
			if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
				if (!otmp->cursed) bless(otmp);
				else uncurse(otmp, FALSE);
			}
			break;
		}
		if(!sobj->cursed || !otmp || !otmp->cursed) {
		    if(!destroy_arm(otmp)) {
			strange_feeling(sobj,"Your skin itches.");
			exercise(A_STR, FALSE);
			exercise(A_CON, FALSE);
			return(1);
		    } else
			known = TRUE;
		} else {	/* armor and scroll both cursed */
		    Your("%s %s.", xname(otmp), otense(otmp, "vibrate"));
		    if (otmp->spe >= -6) otmp->spe--;
		    make_stunned(HStun + rn1(10, 10), TRUE);
		}
	    }
	    break;

	case SCR_DESTROY_WEAPON:

		otmp = uwep;

		if (sobj == otmp) break; /* prevent wonderful "segfault panic" that leaves corrupt files behind --Amy */

		if(confused) {
			if(!otmp) {
				strange_feeling(sobj,"Your fingers itch.");
				exercise(A_STR, FALSE);
				exercise(A_CON, FALSE);
				return(1);
			}
			if (!stack_too_big(otmp)) {
				otmp->oerodeproof = sobj->cursed;
				p_glow2(otmp, NH_PURPLE);
			}
			break;
		}
		if (!otmp) {
			strange_feeling(sobj,"Your fingers itch.");
			exercise(A_STR, FALSE);
			exercise(A_CON, FALSE);
			return(1);
		}

		if(!sobj->cursed || !otmp->cursed) {

			if (!stack_too_big(otmp)) {
			useupall(otmp);
			pline("Your weapon evaporates!");
			}
			known = TRUE;

		} else {	/* weapon and scroll both cursed */
			if (!stack_too_big(otmp)) {
		    Your("%s %s.", xname(otmp), otense(otmp, "vibrate"));
		    if (otmp->spe >= -6) otmp->spe--;
			}
		    make_stunned(HStun + rn1(10, 10), TRUE);
		}

	    break;

	case SCR_BAD_EFFECT:
		if(confused) {
			You_feel("lucky for some reason!");
			break;
		}
		badeffect();
	    break;

	case SCR_PROOF_ARMOR: /*scroll added by Amy*/
	      {
		pline("You may enchant a worn piece of armor.");
proofarmorchoice:
		otmp = getobj(all_count, "magically enchant");
		/*otmp = some_armor(&youmonst);*/
			if(!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to enchant your armor.");
				else goto proofarmorchoice;
				strange_feeling(sobj,"Some weird things are happening to your equipment!");
				exercise(A_STR, FALSE);
				exercise(A_CON, FALSE);
				return(1);
			}
			if (!(otmp->owornmask & W_ARMOR) ) {
	
				strange_feeling(sobj, "You have a feeling of loss.");
				return(1);
			}

			otmp->oerodeproof = 1;
			if (!Blind) otmp->rknown = TRUE;
			p_glow2(otmp, NH_PURPLE);
			if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
				if (!otmp->cursed) bless(otmp);
				else uncurse(otmp, FALSE);
			}
		}
		break;
	case SPE_CONFUSE_MONSTER:
		if (confused) break;
	case SCR_CONFUSE_MONSTER:

		if (sobj->oartifact == ART_IBM_GUILD_MANUAL) {
		      u.uprops[CONFUSION_PROBLEM].intrinsic |= FROMOUTSIDE;
			pline("Thomas Biskup hates you and all other players!!!");
		}

		if(youmonst.data->mlet != S_HUMAN || sobj->cursed) {
			if(!HConfusion) You_feel("confused.");
			make_confused(HConfusion + rnd(100),FALSE);
		} else  if(confused) {
		    if(!sobj->blessed) {
			Your("%s begin to %s%s.",
			    makeplural(body_part(HAND)),
			    Blind ? "tingle" : "glow ",
			    Blind ? nul : hcolor(NH_PURPLE));
			make_confused(HConfusion + rnd(100),FALSE);
		    } else {
			pline("A %s%s surrounds your %s.",
			    Blind ? nul : hcolor(NH_RED),
			    Blind ? "faint buzz" : " glow",
			    body_part(HEAD));
			make_confused(0L,TRUE);
		    }
		} else {
		    if (!sobj->blessed) {
			Your("%s%s %s%s.",
			makeplural(body_part(HAND)),
			Blind ? "" : " begin to glow",
			Blind ? (const char *)"tingle" : hcolor(NH_RED),
			u.umconf ? " even more" : "");
			u.umconf++;
			if (sobj->otyp == SCR_CONFUSE_MONSTER) u.umconf++;
			if (sobj->otyp == SCR_CONFUSE_MONSTER) incr_itimeout(&HConf_resist, rnd(200));
		    } else {
			if (Blind)
			    Your("%s tingle %s sharply.",
				makeplural(body_part(HAND)),
				u.umconf ? "even more" : "very");
			else
			    Your("%s glow a%s brilliant %s.",
				makeplural(body_part(HAND)),
				u.umconf ? "n even more" : "",
				hcolor(NH_RED));
			/* after a while, repeated uses become less effective */
			if (u.umconf >= 40)
			    u.umconf++;
			else
			    u.umconf += rn1(8, 2);
			if (sobj->otyp == SCR_CONFUSE_MONSTER) incr_itimeout(&HConf_resist, rnd(500));
		    }
		}
		break;
	case SPE_CAUSE_FEAR:
		if (confused) break;
		if (!rn2(5)) {

			pline("The spell effect backlashes!");

		    if (!obsidianprotection()) switch (rn2(17)) {
		    case 0:
		    case 1:
		    case 2:
		    case 3: make_confused(HConfusion + 12, FALSE);			/* 40% */
			    break;
		    case 4:
		    case 5:
		    case 6: make_confused(HConfusion + (2L * 12 / 3L), FALSE);		/* 30% */
			    make_stunned(HStun + (12 / 3L), FALSE);
			    break;
		    case 7:
		    case 8: make_stunned(HStun + (2L * 12 / 3L), FALSE);		/* 20% */
			    make_confused(HConfusion + (12 / 3L), FALSE);
			    break;
		    case 9: make_stunned(HStun + 12, FALSE);			/* 10% */
			    break;
		    case 10: make_numbed(HNumbed + 12, FALSE);			/* 10% */
			    break;
		    case 11: make_frozen(HFrozen + 12, FALSE);			/* 10% */
			    break;
		    case 12: make_burned(HBurned + 12, FALSE);			/* 10% */
			    break;
		    case 13: make_feared(HFeared + 12, FALSE);			/* 10% */
			    break;
		    case 14: make_blinded(Blinded + 12, FALSE);			/* 10% */
			    break;
		    case 15: make_hallucinated(HHallucination + 12, FALSE, 0L);			/* 10% */
			    break;
		    case 16: make_dimmed(HDimmed + 12, FALSE);			/* 10% */
			    break;
		    }

		}
	case SCR_SCARE_MONSTER:
	    {	register int ct = 0;
		register struct monst *mtmp;

		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (DEADMONSTER(mtmp)) continue;
		    if(cansee(mtmp->mx,mtmp->my)) {
			if(confused || sobj->cursed) {
			    mtmp->mflee = mtmp->mfrozen = mtmp->msleeping = 0;
			    mtmp->masleep = 0;
			    mtmp->mcanmove = 1;
			} else
			    if (!resist(mtmp, sobj->oclass, 0, NOTELL))
				monflee(mtmp, rnd(10), FALSE, FALSE);
			if(!mtmp->mtame) ct++;	/* pets don't laugh at you */
		    }
		}
		if(!ct)
		      You_hear("%s in the distance.",
			       (confused || sobj->cursed) ? "sad wailing" :
							"maniacal laughter");
		else if(sobj->otyp == SCR_SCARE_MONSTER)
			You_hear("%s close by.",
				  (confused || sobj->cursed) ? "sad wailing" :
						 "maniacal laughter");
		break;
	    }
	case SCR_BLANK_PAPER:
	    if (Blind)
		You("don't remember there being any magic words on this scroll.");
	    else
		pline("This scroll seems to be blank.");
	    known = TRUE;
	    break;
	/* KMH, balance patch -- removed */
	case SCR_TRAP_DETECTION:
		if (!sobj->cursed) return(trap_detect(sobj));
	      break;
		/*what the hell? */

	case SCR_CURE_BLINDNESS:
		if (confused || sobj->cursed) make_blinded(Blinded + rnd(100), TRUE);
		else healup(0, 0, FALSE, TRUE);

	      break;

	case SCR_ALTER_REALITY:
		alter_reality(0);

	      break;

	case SCR_GEOLYSIS:

		pline("You gain the ability to eat through solid rock.");
		/* reading another will restart the countdown rather than add to the duration --Amy */
		u.geolysis = 10 + rnd(20);

	      break;

	case SCR_DETECT_WATER:

		water_detect();

	      break;

	case SCR_SKILL_UP:

		if (sobj->cursed || (confused && rn2(2) ) ) {

			You_feel("your abilities draining away...");
			skillcaploss();

		} else {

			You_feel("like someone has touched your forehead...");

			int skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("Unfortunately, you feel no different than before.");

			if (Race_if(PM_RUSMOT)) {
				if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
					unrestrict_weapon_skill(skillimprove);
					pline("You can now learn the %s skill.", P_NAME(skillimprove));
				} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
					unrestrict_weapon_skill(skillimprove);
					P_MAX_SKILL(skillimprove) = P_BASIC;
					pline("You can now learn the %s skill.", P_NAME(skillimprove));
				} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
					P_MAX_SKILL(skillimprove) = P_SKILLED;
					pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
				} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
					P_MAX_SKILL(skillimprove) = P_EXPERT;
					pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
				} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
					P_MAX_SKILL(skillimprove) = P_MASTER;
					pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
				} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
					P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
					pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
				} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
					P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
					pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
				} else pline("Unfortunately, you feel no different than before.");
			}

		}

	      break;

	case SPE_REMOVE_CURSE:
		if (confused) break;
	case SCR_REMOVE_CURSE:
	    {	register struct obj *obj;
		if(confused) {
		    if (FunnyHallu)
			You_feel("the power of the Force against you!");
		    else
			You_feel("like you need some help.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		} else {
		    if (FunnyHallu)
			You_feel("in touch with the Universal Oneness.");
		    else
			You_feel("like someone is helping you.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Ba, tip bloka l'da budet proklinat' svoye der'mo snova tak ili inache." : "Daedeldaedimm!");

		}

		if (sobj->cursed) {
			pline_The("scroll disintegrates.");
			if (evilfriday) {
				You_feel("as if you need some help.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
				rndcurse();
			}
		} else {

			/* Scroll of remove curse will completely decontaminate you --Amy */
			if (u.contamination && !(sobj->otyp == SPE_REMOVE_CURSE)) decontaminate(u.contamination);
			if (uinsymbiosis && !(sobj->otyp == SPE_REMOVE_CURSE && rn2(5))) uncursesymbiote(FALSE);

		    for (obj = invent; obj; obj = obj->nobj) {
			long wornmask;
#ifdef GOLDOBJ
			/* gold isn't subject to cursing and blessing */
			if (obj->oclass == COIN_CLASS) continue;
#endif
			wornmask = (obj->owornmask & ~(W_BALL|W_ART|W_ARTI));
			if (wornmask && !sobj->blessed) {
			    /* handle a couple of special cases; we don't
			       allow auxiliary weapon slots to be used to
			       artificially increase number of worn items */
			    if (obj == uswapwep) {
				if (!u.twoweap) wornmask = 0L;
			    } else if (obj == uquiver) {
				if (obj->oclass == WEAPON_CLASS) {
				    /* mergeable weapon test covers ammo,
				       missiles, spears, daggers & knives */
				    if (!objects[obj->otyp].oc_merge) 
					wornmask = 0L;
				} else if (obj->oclass == GEM_CLASS) {
				    /* possibly ought to check whether
				       alternate weapon is a sling... */
				    if (!uslinging()) wornmask = 0L;
				} else {
				    /* weptools don't merge and aren't
				       reasonable quivered weapons */
				    wornmask = 0L;
				}
			    }
			}
			/* Amy nerf: blessed ones were way too powerful, allowing you to more or less ignore curses */
			if (( (sobj->blessed && !rn2(5)) || wornmask ||
			     obj->otyp == LOADSTONE ||
			     obj->otyp == LOADBOULDER ||
			     obj->otyp == STARLIGHTSTONE ||
			     obj->otyp == LUCKSTONE ||
			     obj->otyp == HEALTHSTONE ||
			     obj->otyp == MANASTONE ||
			     obj->otyp == SLEEPSTONE ||
			     obj->otyp == STONE_OF_MAGIC_RESISTANCE ||
			     is_nastygraystone(obj) ||
			     (obj->otyp == LEATHER_LEASH && obj->leashmon) || (obj->otyp == INKA_LEASH && obj->leashmon) ) && !stack_too_big(obj) ) {
			    if(confused) blessorcurse(obj, 2);
			    else if (!(sobj->otyp == SPE_REMOVE_CURSE) || !rn2(5) ) uncurse(obj, FALSE);
			}
		    }
		}

		if(Punished && !confused) unpunish();

		update_inventory();

		if (sobj->otyp == SPE_REMOVE_CURSE) {
			if (!rn2(10)) {
				pline("The spell backfires!");
				badeffect();
			}
			if (!rn2(100)) {
				You_feel("as if you need some help.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
				rndcurse();
			}
		}

		break;
	    }
	case SPE_CREATE_MONSTER:
		{
		if (confused) break;

		(void) makemon((struct permonst *)0, u.ux, u.uy, MM_NOSPECIALS|MM_ANGRY|MM_FRENZIED);
		if (!rn2(4)) {
			pline("The spell backfires!");
			badeffect();
		}

		u.aggravation = 0;

		}

		break;
	case SCR_CREATE_MONSTER:
	    if (create_critters(1 + ((confused || sobj->cursed) ? 12 : 0) +
				((sobj->blessed || rn2(73)) ? 0 : rnd(4)),
			/*confused ? &mons[PM_ACID_BLOB] :*/ (struct permonst *)0)) /* no easy blob fort building --Amy */
		known = TRUE;
	    /* no need to flush monsters; we ask for identification only if the
	     * monsters are not visible
	     */
	    break;

	case SCR_CREATE_VICTIM:

	    {
		int cnt = rnd(9);
		if (confused) cnt += rnd(12);
		if (sobj->cursed) cnt += rnd(5);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {
			makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS);
		}

		u.aggravation = 0;
	    }

	    break;

	case SCR_CREATE_FAMILIAR:
		known = TRUE;
		if (confused) create_critters(rn1(7,6), (struct permonst *)0);
		else 		(void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE);

		if (sobj->oartifact == ART_FRIEND_CALL)  		(void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE);

		break;

	case SCR_MEGALOAD:
		known = TRUE;

		{

		struct obj *ldstone;

		pline("A gray stone appears from nowhere!");

		ldstone = mksobj_at(LOADSTONE, u.ux, u.uy, TRUE, FALSE, FALSE);
		if (ldstone) {
			ldstone->quan = 1L;
			ldstone->owt = weight(ldstone);
			if (!Blind) ldstone->dknown = 1;
			if (ldstone) {
			      pline("The stone automatically wanders into your knapsack!");
				(void) pickup_object(ldstone, 1L, TRUE, TRUE);
			}
		}

		}

		break;

	case SCR_VILENESS:
		known = TRUE;

		bad_artifact();

		break;

	case SCR_HYBRIDIZATION:
		known = TRUE;

		/* cursed always adds one, blessed has 75% chance of removing one, uncursed 50% of either */
		if (sobj->cursed) changehybridization(2);
		else if (sobj->blessed) {
			changehybridization(rn2(4) ? 1 : 2);
		}
		else changehybridization(0);

		break;

	case SCR_NASTY_CURSE:
		known = TRUE;

		nastytrapcurse();

		break;

	case SCR_BAD_EQUIPMENT:
		known = TRUE;

		bad_equipment(0);

		break;

	case SCR_ANTIMATTER:
		known = TRUE;

		pline("You are caught in an antimatter storm!");
		withering_damage(invent, FALSE, FALSE);

		break;

	case SCR_RUMOR:
		known = TRUE;

		{
			const char *line;
			char buflin[BUFSZ];
			if (rn2(2)) line = getrumor(-1, buflin, TRUE);
			else line = getrumor(0, buflin, TRUE);
			if (!*line) line = "Slash'EM rumors file closed for renovation.";
			pline("%s", line);
		}

		break;

	case SCR_MESSAGE:
		known = TRUE;

		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}

		break;

	case SCR_ILLUSION:
		known = TRUE;

	    {
		coord cc;
		int cnt = rnd(6);
		if (confused) cnt += rno(6);
		if (sobj->cursed) cnt += rno(3);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {

			makemon(illusionmon(), u.ux, u.uy, NO_MM_FLAGS);

		}
	    }
		pline("Some monsters from the Illusory Castle are summoned!");

		u.aggravation = 0;

		break;

	case SCR_EVIL_VARIANT:
		known = TRUE;

	    {
		coord cc;
		int cnt = rnd(6);
		if (confused) cnt += rno(6);
		if (sobj->cursed) cnt += rno(3);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {

			makemon(specialtensmon(341), u.ux, u.uy, NO_MM_FLAGS); /* M5_EVIL */

		}
	    }
		pline("Some monsters from the Evil Variant are summoned!");

		u.aggravation = 0;

		break;

	case SCR_FEMINISM:
		known = TRUE;

		if (!rn2(2)) {
			randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));
			break;
		}

	    {
		coord cc;
		int cnt = rnd(6);
		if (confused) cnt += rno(6);
		if (sobj->cursed) cnt += rno(3);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {

			makemon(specialtensmon(!rn2(50) ? 369 : !rn2(20) ? 333 : !rn2(3) ? 38 : !rn2(2) ? 39 : 40), u.ux, u.uy, NO_MM_FLAGS); /* AD_FEMI, MS_STENCH, and the three MS_FART_foo */

		}
	    }
		pline("Several women appear from nowhere!");

		u.aggravation = 0;

		break;

	case SCR_TERRAFORMING:
		known = TRUE;

		{
			int maderoomX = 0;

			do_clear_areaX(u.ux, u.uy, 1, terraincleanupA, (void *)&maderoomX);

			if (maderoomX) pline("Some annoying terrain was cleaned up!");
			else pline("There was nothing to clean up...");

		}

		break;

	case SCR_INFERIOR_MATERIAL:
		known = TRUE;

		pline("You have found a scroll of inferior material!");

materialchoice1:
		{
			struct obj *otmpC = getobj(all_count, "change the material of");
			if (!otmpC) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to change an item's material.");
				else goto materialchoice1;
				break;
			}
			if ((otmpC->otyp == GOLD_PIECE) || (otmpC->otyp == STRANGE_OBJECT) || (otmpC->otyp == AMULET_OF_YENDOR) || (otmpC->otyp == CANDELABRUM_OF_INVOCATION) || (otmpC->otyp == BELL_OF_OPENING) || (otmpC->otyp == SPE_BOOK_OF_THE_DEAD) || (objects[otmpC->otyp].oc_prob < 1)) {
				pline("The material of that item cannot be changed!");
				break;
			} else {
				int changematerial;
				switch (rnd(14)) {
					case 1: changematerial = MT_LIQUID; break;
					case 2: changematerial = MT_WAX; break;
					case 3: changematerial = MT_VEGGY; break;
					case 4: changematerial = MT_FLESH; break;
					case 5: changematerial = MT_PAPER; break;
					case 6: changematerial = MT_CLOTH; break;
					case 7: changematerial = MT_LEATHER; break;
					case 8: changematerial = MT_IRON; break;
					case 9: changematerial = MT_PLASTIC; break;
					case 10: changematerial = MT_TAR; break;
					case 11: changematerial = MT_SECREE; break;
					case 12: changematerial = MT_POURPOOR; break;
					case 13: changematerial = MT_SAND; break;
					case 14: changematerial = MT_CERAMIC; break;
					default: changematerial = MT_PAPER; break;
				}
				objects[otmpC->otyp].oc_material = changematerial;
				pline("Success! The item's material got changed.");

			}
		}

		break;

	case SCR_REGULAR_MATERIAL:
		known = TRUE;

		pline("You have found a scroll of regular material!");

materialchoice2:
		{
			struct obj *otmpC = getobj(all_count, "change the material of");
			if (!otmpC) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to change an item's material.");
				else goto materialchoice2;
				break;
			}
			if ((otmpC->otyp == GOLD_PIECE) || (otmpC->otyp == STRANGE_OBJECT) || (otmpC->otyp == AMULET_OF_YENDOR) || (otmpC->otyp == CANDELABRUM_OF_INVOCATION) || (otmpC->otyp == BELL_OF_OPENING) || (otmpC->otyp == SPE_BOOK_OF_THE_DEAD) || (objects[otmpC->otyp].oc_prob < 1)) {
				pline("The material of that item cannot be changed!");
				break;
			} else {
				int changematerial;
				switch (rnd(15)) {
					case 1: changematerial = MT_MYSTERIOUS; break;
					case 2: changematerial = MT_WOOD; break;
					case 3: changematerial = MT_BONE; break;
					case 4: changematerial = MT_METAL; break;
					case 5: changematerial = MT_COPPER; break;
					case 6: changematerial = MT_GLASS; break;
					case 7: changematerial = MT_GEMSTONE; break;
					case 8: changematerial = MT_MINERAL; break;
					case 9: changematerial = MT_SILK; break;
					case 10: changematerial = MT_COMPOST; break;
					case 11: changematerial = MT_BRICK; break;
					case 12: changematerial = MT_SHADOWSTUFF; break;
					case 13: changematerial = MT_OBSIDIAN; break;
					case 14: changematerial = MT_LEAD; break;
					case 15: changematerial = MT_CHROME; break;
					default: changematerial = MT_METAL; break;
				}
				objects[otmpC->otyp].oc_material = changematerial;
				pline("Success! The item's material got changed.");

			}
		}

		break;

	case SCR_SUPERIOR_MATERIAL:
		known = TRUE;

		pline("You have found a scroll of superior material!");

materialchoice3:
		{
			struct obj *otmpC = getobj(all_count, "change the material of");
			if (!otmpC) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to change an item's material.");
				else goto materialchoice3;
				break;
			}
			if ((otmpC->otyp == GOLD_PIECE) || (otmpC->otyp == STRANGE_OBJECT) || (otmpC->otyp == AMULET_OF_YENDOR) || (otmpC->otyp == CANDELABRUM_OF_INVOCATION) || (otmpC->otyp == BELL_OF_OPENING) || (otmpC->otyp == SPE_BOOK_OF_THE_DEAD) || (objects[otmpC->otyp].oc_prob < 1)) {
				pline("The material of that item cannot be changed!");
				break;
			} else {
				int changematerial;
				switch (rnd(16)) {
					case 1: changematerial = MT_DRAGON_HIDE; break;
					case 2: changematerial = MT_SILVER; break;
					case 3: changematerial = MT_GOLD; break;
					case 4: changematerial = MT_PLATINUM; break;
					case 5: changematerial = MT_MITHRIL; break;
					case 6: changematerial = MT_VIVA; break;
					case 7: changematerial = MT_INKA; break;
					case 8: changematerial = MT_ARCANIUM; break;
					case 9: changematerial = MT_ETERNIUM; break;
					case 10: changematerial = MT_ETHER; break;
					case 11: changematerial = MT_NANOMACHINE; break;
					case 12: changematerial = MT_CELESTIUM; break;
					case 13: changematerial = MT_CONUNDRUM; break;
					case 14: changematerial = MT_PWN_BUBBLE; break;
					case 15: changematerial = MT_METEOSTEEL; break;
					case 16: changematerial = MT_ANTIDOTIUM; break;
					default: changematerial = MT_MITHRIL; break;
				}
				objects[otmpC->otyp].oc_material = changematerial;
				pline("Success! The item's material got changed.");

			}

		}

		break;

	case SCR_SIN:

		{
		int dmg = 0;
		struct obj *otmpi, *otmpii;

		switch (rnd(8)) {

			case 1: /* gluttony */
				u.negativeprotection++;
				if (evilfriday && u.ublessed > 0) {
					u.ublessed -= 1;
					if (u.ublessed < 0) u.ublessed = 0;
				}
				You_feel("less protected!");
				break;
			case 2: /* wrath */
				if(u.uen < 1) {
				    You_feel("less energised!");
				    u.uenmax -= rn1(10,10);
				    if(u.uenmax < 0) u.uenmax = 0;
				} else if(u.uen <= 10) {
				    You_feel("your magical energy dwindle to nothing!");
				    u.uen = 0;
				} else {
				    You_feel("your magical energy dwindling rapidly!");
				    u.uen /= 2;
				}
				break;
			case 3: /* sloth */
				You_feel("a little apathetic...");

				switch(rn2(7)) {
				    case 0: /* destroy certain things */
					lethe_damage(invent, FALSE, FALSE);
					break;
				    case 1: /* sleep */
					if (multi >= 0) {
					    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) {pline("You yawn."); break;}
					    fall_asleep(-rnd(10), TRUE);
					    You("are put to sleep!");
					}
					break;
				    case 2: /* paralyse */
					if (multi >= 0) {
					    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
						You("momentarily stiffen.");            
					    } else {
						You("are frozen!");
						if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
						nomovemsg = 0;	/* default: "you can move again" */
						nomul(-rnd(10), "paralyzed by a scroll of sin", TRUE);
						exercise(A_DEX, FALSE);
					    }
					}
					break;
				    case 3: /* slow */
					if(HFast)  u_slow_down();
					else You("pause momentarily.");
					break;
				    case 4: /* drain Dex */
					adjattrib(A_DEX, -rn1(1,1), 0, TRUE);
					break;
				    case 5: /* steal teleportitis */
					if(HTeleportation & INTRINSIC) {
					      HTeleportation &= ~INTRINSIC;
					}
			 		if (HTeleportation & TIMEOUT) {
						HTeleportation &= ~TIMEOUT;
					}
					if(HTeleport_control & INTRINSIC) {
					      HTeleport_control &= ~INTRINSIC;
					}
			 		if (HTeleport_control & TIMEOUT) {
						HTeleport_control &= ~TIMEOUT;
					}
				      You("don't feel in the mood for jumping around.");
					break;
				    case 6: /* steal sleep resistance */
					if(HSleep_resistance & INTRINSIC) {
						HSleep_resistance &= ~INTRINSIC;
					} 
					if(HSleep_resistance & TIMEOUT) {
						HSleep_resistance &= ~TIMEOUT;
					} 
					You_feel("like you could use a nap.");
					break;
				}

				break;
			case 4: /* greed */
				if (u.ugold) pline("Your purse feels lighter...");
				u.ugold /= 2;
				break;
			case 5: /* lust */
				if (invent) {
					pline("Your belongings leave your body!");
				    int itemportchance = 10 + rn2(21);
				    for (otmpi = invent; otmpi; otmpi = otmpii) {

				      otmpii = otmpi->nobj;

					if (!rn2(itemportchance) && !(objects[otmpi->otyp].oc_material == MT_BONE && rn2(10)) && !stack_too_big(otmpi) ) {

						if (otmpi->owornmask & W_ARMOR) {
						    if (otmpi == uskin) {
							skinback(TRUE);		/* uarm = uskin; uskin = 0; */
						    }
						    if (otmpi == uarm) (void) Armor_off();
						    else if (otmpi == uarmc) (void) Cloak_off();
						    else if (otmpi == uarmf) (void) Boots_off();
						    else if (otmpi == uarmg) (void) Gloves_off();
						    else if (otmpi == uarmh) (void) Helmet_off();
						    else if (otmpi == uarms) (void) Shield_off();
						    else if (otmpi == uarmu) (void) Shirt_off();
						    /* catchall -- should never happen */
						    else setworn((struct obj *)0, otmpi ->owornmask & W_ARMOR);
						} else if (otmpi ->owornmask & W_AMUL) {
						    Amulet_off();
						} else if (otmpi ->owornmask & W_IMPLANT) {
						    Implant_off();
						} else if (otmpi ->owornmask & W_RING) {
						    Ring_gone(otmpi);
						} else if (otmpi ->owornmask & W_TOOL) {
						    Blindf_off(otmpi);
						} else if (otmpi ->owornmask & (W_WEP|W_SWAPWEP|W_QUIVER)) {
						    if (otmpi == uwep)
							uwepgone();
						    if (otmpi == uswapwep)
							uswapwepgone();
						    if (otmpi == uquiver)
							uqwepgone();
						}

						if (otmpi->owornmask & (W_BALL|W_CHAIN)) {
						    unpunish();
						} else if (otmpi->owornmask) {
						/* catchall */
						    setnotworn(otmpi);
						}

						dropx(otmpi);
					      if (otmpi->where == OBJ_FLOOR) rloco(otmpi);
						u.cnd_itemportcount++;
					}

				    }
				}
				break;
			case 6: /* envy */
				if (flags.soundok) {
					You_hear("a chuckling laughter.");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Kha-kha-kha-kha-kha-KDZH KDZH, tip bloka l'da smeyetsya yego tortsa, potomu chto vy teryayete vse vashi vstroyennyye funktsii!" : "Hoehoehoehoe!");
				}
			      attrcurse();
			      attrcurse();
				break;
			case 7: /* pride */
			      pline("The RNG determines to take you down a peg or two...");
				if (!rn2(3)) {
				    poisoned("air", rn2(A_MAX), "scroll of sin", 30);
				}
				if (!rn2(4)) {
					You_feel("drained...");
					u.uhpmax -= rn1(10,10);
					if (u.uhpmax < 1) u.uhpmax = 1;
					if(u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				}
				if (!rn2(4)) {
					You_feel("less energised!");
					u.uenmax -= rn1(10,10);
					if (u.uenmax < 0) u.uenmax = 0;
					if(u.uen > u.uenmax) u.uen = u.uenmax;
				}
				if (!rn2(4)) {
					if(!Drain_resistance || !rn2(StrongDrain_resistance ? 16 : 4) )
					    losexp("life drainage", FALSE, TRUE);
					else You_feel("woozy for an instant, but shrug it off.");
				}
				break;
			case 8: /* depression */

			    switch(rnd(20)) {
			    case 1:
				if (!Unchanging && !Antimagic) {
					You("undergo a freakish metamorphosis!");
				      polyself(FALSE);
				}
				break;
			    case 2:
				You("need reboot.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
				if (!Race_if(PM_UNGENOMOLD)) newman();
				else polyself(FALSE);
				break;
			    case 3: case 4:
				if(!rn2(4) && u.ulycn == NON_PM &&
					!Protection_from_shape_changers &&
					!is_were(youmonst.data) &&
					!defends(AD_WERE,uwep)) {
				    You_feel("feverish.");
				    exercise(A_CON, FALSE);
				    u.ulycn = PM_WERECOW;
				} else {
					if (multi >= 0) {
					    if (Sleep_resistance && rn2(StrongSleep_resistance ? 20 : 5)) break;
					    fall_asleep(-rnd(10), TRUE);
					    You("are put to sleep!");
					}
				}
				break;
			    case 5: case 6:
				pline("Suddenly, there's glue all over you!");
				u.utraptype = TT_GLUE;
				u.utrap = 25 + rnd(monster_difficulty());

				break;
			    case 7:
			    case 8:
				Your("position suddenly seems very uncertain!");
				teleX();
				break;
			    case 9:
				u_slow_down();
				break;
			    case 10:
			    case 11:
			    case 12:
				if ((!StrongSwimming || !rn2(10)) && (!StrongMagical_breathing || !rn2(10))) {
					water_damage(invent, FALSE, FALSE);
				}
				break;
			    case 13:
				if (multi >= 0) {
				    if (Free_action && rn2(StrongFree_action ? 100 : 20)) {
					You("momentarily stiffen.");            
				    } else {
					You("are frozen!");
					if (PlayerHearsSoundEffects) pline(issoviet ? "Teper' vy ne mozhete dvigat'sya. Nadeyus', chto-to ubivayet vas, prezhde chem vash paralich zakonchitsya." : "Klltsch-tsch-tsch-tsch-tsch!");
					nomovemsg = 0;	/* default: "you can move again" */
					nomul(-rnd(10), "paralyzed by a scroll of sin", TRUE);
					exercise(A_DEX, FALSE);
				    }
				}
				break;
			    case 14:
				if (FunnyHallu)
					pline("What a groovy feeling!");
				else
					You(Blind ? "%s and get dizzy..." :
						 "%s and your vision blurs...",
						    stagger(youmonst.data, "stagger"));
				if (PlayerHearsSoundEffects) pline(issoviet ? "Imet' delo s effektami statusa ili sdat'sya!" : "Wrueue-ue-e-ue-e-ue-e...");
				dmg = rn1(7, 16);
				make_stunned(HStun + dmg + monster_difficulty(), FALSE);
				(void) make_hallucinated(HHallucination + dmg + monster_difficulty(),TRUE,0L);
				break;
			    case 15:
				if(!Blind)
					Your("vision bugged.");
				dmg += rn1(10, 25);
				dmg += rn1(10, 25);
				(void) make_hallucinated(HHallucination + dmg + monster_difficulty() + monster_difficulty(),TRUE,0L);
				break;
			    case 16:
				if(!Blind)
					Your("vision turns to screen saver.");
				dmg += rn1(10, 25);
				(void) make_hallucinated(HHallucination + dmg + monster_difficulty(),TRUE,0L);
				break;
			    case 17:
				{
				    struct obj *objD = some_armor(&youmonst);
	
				    if (objD && drain_item(objD)) {
					Your("%s less effective.", aobjnam(objD, "seem"));
					u.cnd_disenchantamount++;
					if (PlayerHearsSoundEffects) pline(issoviet ? "Vse, chto vy vladeyete budet razocharovalsya v zabveniye, kha-kha-kha!" : "Klatsch!");
				    }
				}
				break;
			    default:
				    if(Confusion)
					 You("are getting even more confused.");
				    else You("are getting confused.");
				    make_confused(HConfusion + monster_difficulty() + 1, FALSE);
				break;
			    }

				break;

		}

		}

		break;

	case SCR_DEMONOLOGY:
		known = TRUE;
	    {
		int cnt = rnd(9);
		if (confused) cnt += rnd(12);
		if (sobj->cursed) cnt += rnd(5);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {
			makemon(mkclass(S_DEMON,0), u.ux, u.uy, NO_MM_FLAGS);
		}

		u.aggravation = 0;

	    }
		pline("You summon the denizens of Gehennom!");
		break;

	case SCR_GIRLINESS:

		You_feel("a female presence.");

	    {
		int cnt = rnd(9);
		if (sobj->cursed) cnt += rnd(18);
		if (confused) cnt += rnd(100);
		while(cnt--) {
			makegirlytrap();
		}
	    }

		break;


	case SCR_ELEMENTALISM:
		known = TRUE;
	    {
		int cnt = rnd(9);
		if (confused) cnt += rnd(12);
		if (sobj->cursed) cnt += rnd(5);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while(cnt--) {
			makemon(mkclass(S_ELEMENTAL,0), u.ux, u.uy, NO_MM_FLAGS);
		}

		u.aggravation = 0;
	    }
		pline("You summon inhabitants of the elemental planes!");
		break;

	case SCR_NASTINESS:
		{

		int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
		if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
		int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
		if (!blackngdur ) blackngdur = 500; /* fail safe */

		if (!rn2(100)) pline("You have a bad feeling in your %s.",body_part(STOMACH) );

		randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), blackngdur - (monster_difficulty() * 3));

		}

		break;

	case SPE_SUMMON_UNDEAD:
		if (confused) break;
		if (!rn2(10)) {
			pline("The spell backfires!");
			badeffect();
		}

	case SCR_SUMMON_UNDEAD:        
	    {
		int cnt = 1, oldmulti = multi;
                struct monst *mtmp;
		multi = 0;
  
		if (Aggravate_monster && (sobj && sobj->otyp == SCR_SUMMON_UNDEAD)) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if(!rn2(73) && !sobj->blessed) cnt += rnd(4);
		if(confused || sobj->cursed) cnt += 12;
		while(cnt--) {
#ifdef WIZARD
		    if(!wizard || !(mtmp = create_particular()))
#endif
		    switch (rn2(10)+1) {
		    case 1:
			mtmp = makemon(mkclass(S_VAMPIRE,0), u.ux, u.uy, (sobj->otyp == SPE_SUMMON_UNDEAD) ? MM_NOSPECIALS|MM_ANGRY|MM_FRENZIED : NO_MM_FLAGS);
			break;
		    case 2:
		    case 3:
		    case 4:
		    case 5:
			mtmp = makemon(mkclass(S_ZOMBIE,0), u.ux, u.uy, (sobj->otyp == SPE_SUMMON_UNDEAD) ? MM_NOSPECIALS|MM_ANGRY|MM_FRENZIED : NO_MM_FLAGS);
			break;
		    case 6:
		    case 7:
		    case 8:
			mtmp = makemon(mkclass(S_MUMMY,0), u.ux, u.uy, (sobj->otyp == SPE_SUMMON_UNDEAD) ? MM_NOSPECIALS|MM_ANGRY|MM_FRENZIED : NO_MM_FLAGS);
			break;
		    case 9:
			mtmp = makemon(mkclass(S_GHOST,0), u.ux, u.uy, (sobj->otyp == SPE_SUMMON_UNDEAD) ? MM_NOSPECIALS|MM_ANGRY|MM_FRENZIED : NO_MM_FLAGS);
			break;
		    case 10:
			mtmp = makemon(mkclass(S_WRAITH,0), u.ux, u.uy, (sobj->otyp == SPE_SUMMON_UNDEAD) ? MM_NOSPECIALS|MM_ANGRY|MM_FRENZIED : NO_MM_FLAGS);
			break;
		    }
		    /* WAC Give N a shot at controlling the beasties
		     * (if not cursed <g>).  Check curse status in case
		     * this ever becomes a scroll
		     */
			/* Amy edit: can control them *only* if it's the scroll (nerf :P) */
		    if (mtmp)
			if (!sobj->cursed && sobj->oclass == SCROLL_CLASS && (Role_if(PM_NECROMANCER) || Race_if(PM_MUMMY) || Race_if(PM_LICH_WARRIOR)) ) {
			    if (!resist(mtmp, sobj->oclass, 0, TELL)) {
				mtmp = tamedog(mtmp, (struct obj *) 0, FALSE);
				if (mtmp) You("dominate %s!", mon_nam(mtmp));
			    }
			} else setmangry(mtmp);
		}
		multi = oldmulti;
		/* WAC Give those who know command undead a shot at control.
		 * Since spell is area affect,  do this after all undead
		 * are summoned
		 */
			/* Amy edit: also only if it's the scroll :P */
		if (!Role_if(PM_NECROMANCER) && !Race_if(PM_MUMMY) && !Race_if(PM_LICH_WARRIOR) && !(sobj->cursed) && sobj->oclass == SCROLL_CLASS) {
		    if (objects[SPE_COMMAND_UNDEAD].oc_name_known) {
			int sp_no;
			for (sp_no = 0; sp_no < MAXSPELL; sp_no++)
			    if (spl_book[sp_no].sp_id == SPE_COMMAND_UNDEAD) {
				You("try to command %s", mon_nam(mtmp));
				spelleffects(sp_no, TRUE);
				break;
			    }
			if (sp_no == MAXSPELL)
			    You("don't seem to have the spell command undead memorized!");
		    } else You("don't know how to command undead...");
		}

		u.aggravation = 0;

		/* flush monsters before asking for identification */
		flush_screen(0);
		break;
	    }

	case SCR_RESURRECTION:

		if (confused) {
			int cnt = 25 + rnd(25);
			coord mm;
			mm.x = u.ux;
			mm.y = u.uy;
			while (cnt--) mkundeadX(&mm, FALSE, NO_MINVENT);
			break;
		}

		if (sobj->cursed) {
			known = TRUE;
			pline("A terrible black aura blasts you!");
			if (u.extralives) done(DIED);
			else NastinessProblem += rnz(1000 * (monster_difficulty() + 1));
		} else {
			known = TRUE;
			u.extralives++;
			pline("1-UP!");
		}

		break;

	case SCR_ENRAGE:

		{
		int effectradius = (sobj->blessed ? 5 : sobj->cursed ? 20 : 10);
		if (confused) effectradius *= 3;
	      register struct monst *mtmp2;
		struct edog* edog;

		for (mtmp2 = fmon; mtmp2; mtmp2 = mtmp2->nmon) {
			if (rn2(3) && distu(mtmp2->mx,mtmp2->my) < effectradius) {
				if (mtmp2->mtame) {
					edog = (mtmp2->isminion) ? 0 : EDOG(mtmp2);
					if (mtmp2->mtame <= rnd(21) || (edog && edog->abuse >= rn2(6) )) {

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
							if (mtmp2->mleashed) { m_unleash(mtmp2,FALSE); }

						}
					}
				} else if (mtmp2->mpeaceful) {
					mtmp2->mpeaceful = 0;
				} else {
					if (!rn2(5)) mtmp2->mfrenzied = 1;
					mtmp2->mhp = mtmp2->mhpmax; /* let's heal them instead --Amy */
				}
			}
		}
		pline("It seems a little more dangerous here now...");

		}

		break;

	case SCR_GROUP_SUMMONING:

		{

		int cnt = rn2(14) + 2;
		struct monst *mon;

		int randmnst;
		int randmnsx;
		struct permonst *randmonstforspawn;
		int monstercolor;

		int spawntype = rnd(4);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		if(confused || sobj->cursed) cnt += 12;

		if (spawntype == 1) {
			randmnst = (rn2(187) + 1);
			randmnsx = (rn2(100) + 1);
		} else if (spawntype == 2) {
			randmonstforspawn = rndmonst();
		} else if (spawntype == 3) {
			monstercolor = rnd(15);
			do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);
		} else {
			monstercolor = rnd(376);
		}

		while(cnt--) {

			if (spawntype == 1) {

			if (randmnst < 6)
		 	    mon = makemon(mkclass(S_ANT,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 9)
		 	    mon = makemon(mkclass(S_BLOB,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 11)
		 	    mon = makemon(mkclass(S_COCKATRICE,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 15)
		 	    mon = makemon(mkclass(S_DOG,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 18)
		 	    mon = makemon(mkclass(S_EYE,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 22)
		 	    mon = makemon(mkclass(S_FELINE,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 24)
		 	    mon = makemon(mkclass(S_GREMLIN,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 29)
		 	    mon = makemon(mkclass(S_HUMANOID,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 33)
		 	    mon = makemon(mkclass(S_IMP,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 36)
		 	    mon = makemon(mkclass(S_JELLY,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 41)
		 	    mon = makemon(mkclass(S_KOBOLD,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 44)
		 	    mon = makemon(mkclass(S_LEPRECHAUN,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 47)
		 	    mon = makemon(mkclass(S_MIMIC,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 50)
		 	    mon = makemon(mkclass(S_NYMPH,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 54)
		 	    mon = makemon(mkclass(S_ORC,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 55)
		 	    mon = makemon(mkclass(S_PIERCER,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 58)
		 	    mon = makemon(mkclass(S_QUADRUPED,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 62)
		 	    mon = makemon(mkclass(S_RODENT,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 65)
		 	    mon = makemon(mkclass(S_SPIDER,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 66)
		 	    mon = makemon(mkclass(S_TRAPPER,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 69)
		 	    mon = makemon(mkclass(S_UNICORN,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 71)
		 	    mon = makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 73)
		 	    mon = makemon(mkclass(S_WORM,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 75)
		 	    mon = makemon(mkclass(S_XAN,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 76)
		 	    mon = makemon(mkclass(S_LIGHT,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 77)
		 	    mon = makemon(mkclass(S_ZOUTHERN,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 78)
		 	    mon = makemon(mkclass(S_ANGEL,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 81)
		 	    mon = makemon(mkclass(S_BAT,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 83)
		 	    mon = makemon(mkclass(S_CENTAUR,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 86)
		 	    mon = makemon(mkclass(S_DRAGON,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 89)
		 	    mon = makemon(mkclass(S_ELEMENTAL,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 94)
		 	    mon = makemon(mkclass(S_FUNGUS,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 99)
		 	    mon = makemon(mkclass(S_GNOME,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 102)
		 	    mon = makemon(mkclass(S_GIANT,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 103)
		 	    mon = makemon(mkclass(S_JABBERWOCK,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 104)
		 	    mon = makemon(mkclass(S_KOP,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 105)
		 	    mon = makemon(mkclass(S_LICH,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 108)
		 	    mon = makemon(mkclass(S_MUMMY,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 110)
		 	    mon = makemon(mkclass(S_NAGA,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 113)
		 	    mon = makemon(mkclass(S_OGRE,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 115)
		 	    mon = makemon(mkclass(S_PUDDING,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 116)
		 	    mon = makemon(mkclass(S_QUANTMECH,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 118)
		 	    mon = makemon(mkclass(S_RUSTMONST,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 121)
		 	    mon = makemon(mkclass(S_SNAKE,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 123)
		 	    mon = makemon(mkclass(S_TROLL,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 124)
		 	    mon = makemon(mkclass(S_UMBER,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 125)
		 	    mon = makemon(mkclass(S_VAMPIRE,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 127)
		 	    mon = makemon(mkclass(S_WRAITH,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 128)
		 	    mon = makemon(mkclass(S_XORN,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 130)
		 	    mon = makemon(mkclass(S_YETI,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 135)
		 	    mon = makemon(mkclass(S_ZOMBIE,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 145)
		 	    mon = makemon(mkclass(S_HUMAN,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 147)
		 	    mon = makemon(mkclass(S_GHOST,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 149)
		 	    mon = makemon(mkclass(S_GOLEM,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 152)
		 	    mon = makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 155)
		 	    mon = makemon(mkclass(S_EEL,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 160)
		 	    mon = makemon(mkclass(S_LIZARD,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 162)
		 	    mon = makemon(mkclass(S_BAD_FOOD,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 165)
		 	    mon = makemon(mkclass(S_BAD_COINS,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 166) {
				if (randmnsx < 96)
		 	    mon = makemon(mkclass(S_HUMAN,0), u.ux, u.uy, MM_ADJACENTOK);
				else
		 	    mon = makemon(mkclass(S_NEMESE,0), u.ux, u.uy, MM_ADJACENTOK);
				}
			else if (randmnst < 171)
		 	    mon = makemon(mkclass(S_GRUE,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 176)
		 	    mon = makemon(mkclass(S_WALLMONST,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 180)
		 	    mon = makemon(mkclass(S_RUBMONST,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 181) {
				if (randmnsx < 99)
		 	    mon = makemon(mkclass(S_HUMAN,0), u.ux, u.uy, MM_ADJACENTOK);
				else
		 	    mon = makemon(mkclass(S_ARCHFIEND,0), u.ux, u.uy, MM_ADJACENTOK);
				}
			else if (randmnst < 186)
		 	    mon = makemon(mkclass(S_TURRET,0), u.ux, u.uy, MM_ADJACENTOK);
			else if (randmnst < 187)
		 	    mon = makemon(mkclass(S_FLYFISH,0), u.ux, u.uy, MM_ADJACENTOK);
			else
		 	    mon = makemon((struct permonst *)0, u.ux, u.uy, MM_ADJACENTOK);

			} else if (spawntype == 2) {

				mon = makemon(randmonstforspawn, u.ux, u.uy, MM_ADJACENTOK);

			} else if (spawntype == 3) {

				mon = makemon(colormon(monstercolor), u.ux, u.uy, MM_ADJACENTOK);

			} else {

				mon = makemon(specialtensmon(monstercolor), u.ux, u.uy, MM_ADJACENTOK);

			}

		    if (mon && canspotmon(mon)) known = TRUE;
		}

		u.aggravation = 0;

		break;

		}

	case SCR_SUMMON_BOSS:
		known = TRUE;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		{
			int attempts = 0;
			register struct permonst *ptrZ;
newboss:
			do {

				ptrZ = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

			if (ptrZ && ptrZ->geno & G_UNIQ) {
				if (wizard) pline("monster generation: %s", ptrZ->mname);
				(void) makemon(ptrZ, u.ux, u.uy, NO_MM_FLAGS);
			}
			else if (rn2(50)) {
				attempts = 0;
				goto newboss;
			}
			if (confused ? rn2(10) : !rn2(100) ) {
				attempts = 0;
				goto newboss;
			}
			pline("Boss monsters appear from nowhere!");

		}

		u.aggravation = 0;

		break;

	case SCR_IMMOBILITY:
		known = TRUE;
		{

		int monstcnt;
		monstcnt = 8 + rnd(10);
		if (sobj->cursed) monstcnt += (8 + rnd(10)) ;
		if (confused) monstcnt += (12 + rnd(15));
		int sessileattempts;
		int sessilemnum;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
			for (sessileattempts = 0; sessileattempts < 100; sessileattempts++) {
				sessilemnum = rndmonnum();
				if (sessilemnum != -1 && (mons[sessilemnum].mlet != S_TROVE) && is_nonmoving(&mons[sessilemnum]) ) sessileattempts = 100;
			}
			if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], u.ux, u.uy, NO_MM_FLAGS);
		}

		}

		u.aggravation = 0;

		break;

	case SCR_EGOISM:
		known = TRUE;
		{

		int monstcnt;
		monstcnt = rnd(5);
		if (sobj->cursed) monstcnt += rnd(6);
		if (confused) monstcnt += rnd(12);
		int sessileattempts;
		int sessilemnum;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	      while(--monstcnt >= 0) {
			for (sessileattempts = 0; sessileattempts < 10000; sessileattempts++) {
				sessilemnum = rndmonnum();
				if (sessilemnum != -1 && always_egotype(&mons[sessilemnum]) ) sessileattempts = 10000;
			}
			if (sessilemnum != -1) (void) makemon( &mons[sessilemnum], u.ux, u.uy, NO_MM_FLAGS);
		}

		}

		u.aggravation = 0;

		break;

	case SCR_SUMMON_GHOST:

		known = TRUE;

		{
		coord mm;   
		mm.x = u.ux;   
		mm.y = u.uy;   

		tt_mname(&mm, FALSE, 0);	/* create player-monster ghosts */
		}

		break;

	case SCR_SUMMON_ELM:

		{
		int aligntype;
		aligntype = rn2((int)A_LAWFUL+2) - 1;
		pline("A servant of %s appears!",aligns[1 - aligntype].noun);
		summon_minion(aligntype, TRUE);
		}

		break;

	case SPE_ENCHANT_WEAPON:
		if (confused) break;
	case SCR_ENCHANT_WEAPON:
		if (uwep && stack_too_big(uwep)) {
			pline("The enchantment failed due to the stack being too big.");
			break;
		}

		if(uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == GEM_CLASS || uwep->oclass == CHAIN_CLASS || is_weptool(uwep))
			&& confused) {
		/* oclass check added 10/25/86 GAN */
			uwep->oerodeproof = !(sobj->cursed);
			if (Blind) {
			    uwep->rknown = FALSE;
			    Your("weapon feels warm for a moment.");
			} else {
			    uwep->rknown = TRUE;
			    Your("%s covered by a %s %s %s!",
				aobjnam(uwep, "are"),
				sobj->cursed ? "mottled" : "shimmering",
				hcolor(sobj->cursed ? NH_PURPLE : NH_GOLDEN),
				sobj->cursed ? "glow" : "shield");
			}
			if (uwep->oerodeproof && (uwep->oeroded || uwep->oeroded2)) {
			    uwep->oeroded = uwep->oeroded2 = 0;
			    Your("%s as good as new!",
				 aobjnam(uwep, Blind ? "feel" : "look"));
			}
		/* KMH, balance patch -- Restore the NetHack success rate */
		} else return !chwepon(sobj,
				       sobj->cursed ? -1 :
				       !uwep ? 1 :
		/* Come on, the evaporation chance is nasty enough. Let's allow enchanting stuff beyond +9. --Amy */
				       uwep->spe >= 9 ? /*(rn2(uwep->spe) == 0)*/1 :
				       sobj->blessed ? rnd(3-uwep->spe/3) : 1);
/*		else return !chwepon(sobj,
				       sobj->cursed ? -rnd(2) :
				       !uwep ? 1 :
				       uwep->spe >= 3 ? (rn2(uwep->spe) == 0) :
				       sobj->blessed ? rnd(2) : 1);*/
		break;
	case SCR_PROOF_WEAPON: /* scroll added by Amy */
		if (uwep && stack_too_big(uwep)) {
			pline("The enchantment failed due to the stack being too big.");
			break;
		}

		if(uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == GEM_CLASS || uwep->oclass == CHAIN_CLASS || is_weptool(uwep)))
	      {
			uwep->oerodeproof = 1;
			if (uwep && objects[(uwep)->otyp].oc_material == MT_CELESTIUM) {
				if (!uwep->cursed) bless(uwep);
				else uncurse(uwep, FALSE);
			}
			if (Blind) {
			    uwep->rknown = FALSE;
			    Your("weapon feels warm for a moment.");
			} else {
			    uwep->rknown = TRUE;
			    Your("%s covered by a %s %s %s!",
				aobjnam(uwep, "are"),
				sobj->cursed ? "mottled" : "shimmering",
				hcolor(sobj->cursed ? NH_PURPLE : NH_GOLDEN),
				sobj->cursed ? "glow" : "shield");
			}
		}
		break;
	case SCR_SLEEP:
		known = TRUE;
		if(confused) {
			You_feel("awake!");
			if (sobj->cursed) incr_itimeout(&HSleep_resistance, 10);
			else if (sobj->blessed) incr_itimeout(&HSleep_resistance, 200);
			else {incr_itimeout(&HSleep_resistance, 100);}
		}
		else {
			if(Sleep_resistance && (StrongSleep_resistance || rn2(10)) ) You("yawn.");
			else {You("fall asleep!");
			if (sobj->cursed) fall_asleep(-50, FALSE);
			else if (sobj->blessed) fall_asleep(-10, FALSE);
			else {fall_asleep(-20, FALSE);}
				}
		}
		break;
	case SCR_TRAP_CREATION:
		known = TRUE;
		    You_feel("endangered!!");
		{
			int rtrap;
		    int i, j, bd = confused ? 5 : 1;

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ <= DBWALL) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = randomtrap();

				(void) maketrap(u.ux + i, u.uy + j, rtrap, 100);
			}
		}

		makerandomtrap();
		if (!rn2(2)) makerandomtrap();
		if (!rn2(4)) makerandomtrap();
		if (!rn2(8)) makerandomtrap();
		if (!rn2(16)) makerandomtrap();
		if (!rn2(32)) makerandomtrap();
		if (!rn2(64)) makerandomtrap();
		if (!rn2(128)) makerandomtrap();
		if (!rn2(256)) makerandomtrap();

		break;

	case SCR_CREATE_TRAP:

		{
		struct trap *ttmp2 = maketrap(u.ux, u.uy, randomtrap(), 100 );
		if (ttmp2) dotrap(ttmp2, 0);
		}

		break;

	case SCR_COPYING:

		pline("Your attempt to violate the copy protection fails. Filthy hacker.");

		break;

	case SCR_TRAP_DISARMING:


		known = TRUE;
		You_feel("out of the danger zone.");
		{
			int rtrap;
			struct trap *ttmp;

		    int i, j, bd = confused ? 5 : 1;

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {

				if ((ttmp = t_at(u.ux + i, u.uy + j)) != 0) {
				    if (ttmp->ttyp == MAGIC_PORTAL) continue;
					deltrap(ttmp);
					u.uhpmax++;
					if (Upolyd) u.mhmax++;
					flags.botl = TRUE;
				}

			}
		}
		(void) doredraw();
		break;
	case SCR_TAMING:
		if (u.uswallow) {
		    maybe_tame(u.ustuck, sobj);
		} else {
		    int i, j, bd = confused ? 5 : 1;
		    if (sobj->oartifact == ART_FOR_THE_GOOD_CAUSE) bd = 5;
		    struct monst *mtmp;

		    for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0)
			    maybe_tame(mtmp, sobj);
		    }
		}
		break;
	case SCR_BULLSHIT:
		pline("You notice a vile stench...");

		    int i, j, bd = confused ? 100 : sobj->cursed ? 2 : 1;
		    for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ <= DBWALL) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;
			maketrap(u.ux + i, u.uy + j, rn2(5) ? SHIT_TRAP : SHIT_PIT, 0);
		    }

		break;

	case SPE_CHARM_MONSTER: /* gotta nerf that overpowered spell a little --Amy */
		if (confused) break;
	
		if (u.uswallow && !rn2(4) ) {
		    maybe_tame(u.ustuck, sobj);
		} else {
		    int i, j, bd = confused ? 5 : 1;
		    struct monst *mtmp;

		    for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0)
			    if (!rn2(4) || ( (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) && mtmp->data == &mons[PM_TOPMODEL]) ) maybe_tame(mtmp, sobj);
			    else if (!rn2(10) && !((rnd(30 - ACURR(A_CHA))) < 4) && !mtmp->mfrenzied && !mtmp->mtame) {
				pline("Instead of being tamed, %s enters a state of frenzy!", mon_nam(mtmp));
				mtmp->mpeaceful = 0;
				mtmp->mfrenzied = 1;
			    }
		    }
		}

		if (!rn2(5)) {
			pline("The spell backfires!");
			badeffect();
		}

	break;

	case SCR_LOCKOUT:
		known = TRUE;
		if (confused) {
			/* remove walls from vicinity of player */
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_lockflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				You_feel("free once again.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_lockflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			/*if (!madepool && stilldry)
				break;*/
			if (madepool)
				pline(FunnyHallu ?
						"It's getting a little bit tight in here!" :
						"Walls and obstacles shoot up from the ground!" );
			else pline(FunnyHallu ?
						"You hear a grating that reminds you of Chinese water torture!" :
						"You see dust particles flying around." );

			known = TRUE;
			break;
		}

	break;


	case SCR_LAVA:
		known = TRUE;
		if (confused) {
			/* remove lava from vicinity of player */
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_lavaflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				You("don't feel hot anymore.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_lavaflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Wow, that's, like, TOTALLY HOT, dude!" :
						"A stream of lava surges through the area!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_GRAVE:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_graveflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The grave walls are dug out.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_graveflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Hans Walt has hidden the diamonds here! Are you a badass enough hussy to steal them?" :
						"Hans Walt erects grave walls!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_TUNNELS:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_tunnelflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("It seems that the tunnels have collapsed and the debris was cleared.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_tunnelflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"The course is driving through a tunnel and therefore your mobile phone does not go!" :
						"It seems that construction workers have carved tunnels through the rock." );
			known = TRUE;
			break;
		}

	break;

	case SCR_FARMING:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_farmflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("Old Mac Donald bought the farm.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_farmflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"The Greens are forcing new nature preservation laws upon you!" :
						"All the local farmers are claiming land on this dungeon level." );
			known = TRUE;
			break;
		}

	break;

	case SCR_MOUNTAINS:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_mountainflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The mountains start to crumble.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_mountainflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"The Amy her roommate uses a wing-tufted facial expression on you! :-)" :
						"Strange... the underground dungeon seems to become mountainous." );
			known = TRUE;
			break;
		}

	break;

	case SCR_DIVING:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_watertunnelflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The bulldozers have transformed the water tunnels into regular terrain.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_watertunnelflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Deep sea diving challenge! You must collect 50 clams in three minutes or lose the game automatically!" :
						"Watery tunnels are erected!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_CRYSTALLIZATION:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_crystalwaterflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("You sense a lack of crystal water.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_crystalwaterflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Due to the laws of physics, the water flows upward." :
						"Crystallized water appears on the ceiling." );
			known = TRUE;
			break;
		}

	break;

	case SCR_MOORLAND:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_moorflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("You no longer smell the swampy mud.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_moorflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Watch out, the swamp holes are like shifting sand - you drown if you fall into them." :
						"The dungeon gets swampy." );
			known = TRUE;
			break;
		}

	break;

	case SCR_URINE:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_urineflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("At last, that urine stench is gone.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_urineflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Holy crap, you get to watch Mira create her 'swimming pools' firsthand..." :
						"Mira pees all over the dungeon." );
			known = TRUE;
			break;
		}

	break;

	case SCR_QUICKSAND:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_shiftingsandflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("You no longer need to fear the quicksand.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_shiftingsandflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Uh-oh, on one side you have a cube and there's a cute asian girl on the other! Both of them are deadly!" :
						"Deadly sandholes appear." );
			known = TRUE;
			break;
		}

	break;

	case SCR_STYX:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_styxflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("You sense the removal of contaminated water.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_styxflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"A constant 'Jason... Jason... Jason...' chant is audible in the distance..." :
						"Hellish green water flows into the dungeon!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_SNOW:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_snowflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("It stops snowing.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_snowflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"A hailstorm! Quick, find a shelter, because otherwise the hailstones will get bigger and bigger until they crush you!" :
						"It starts snowing!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_ASH:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_ashflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The ash solidifies permanently.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_ashflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Mehrunes Dagon has opened yet another gate to Oblivion!" :
						"The floor becomes red." );
			known = TRUE;
			break;
		}

	break;

	case SCR_SAND:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_sandflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The desert is pushed back.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_sandflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Oh no, you're stranded in the desert and your water reserves are depleted! This is the end!" :
						"Soft sand appears in the dungeon." );
			known = TRUE;
			break;
		}

	break;

	case SCR_PAVING:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_pavementflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The pavement turns into normal floor.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_pavementflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Holy shit there are paved roads and large fountains and whoa you absolutely need to play part 2 of this game!" :
						"Suddenly there are paved roads everywhere!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_HIGHWAY:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_highwayflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The highway is deconstructed.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_highwayflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"DUDE! The secret entrance to the right is open! RUN, it will close in 100 turns!" :
						"Highways are being built all over the dungeon!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_GRASSLAND:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_grassflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The grass wilts.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_grassflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"You sense the presence of a bunch of spies." :
						"You feel the grass grow." );
			known = TRUE;
			break;
		}

	break;

	case SCR_NETHER:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_nethermistflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The nether mist dissipates.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_nethermistflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Suddenly Galadriel throws you into the void, and you need to defeat Melkor before you can come back." :
						"Purple mist appears." );
			known = TRUE;
			break;
		}

	break;

	case SCR_STALACTITE:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_stalactiteflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("A bunch of stalactites is broken off.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_stalactiteflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Team Piercer is attacking, even though they're actually rather rare." :
						"Stalactites shoot out of the ceiling!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_CRYPT:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_cryptflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The crypt gets reconsecrated.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_cryptflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Argh! In your stupidity, you forgot to get the torch and now the crypts are completely dark!" :
						"Seems you're in the crypt now." );
			known = TRUE;
			break;
		}

	break;

	case SCR_BUBBLE_BOBBLE:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_bubbleflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("The bubbles pop.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_bubbleflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"There are some air bubbles... maybe you can reach higher places with them?" :
						"Floating bubbles appear!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_RAIN:
		known = TRUE;
		if (confused) {
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_raincloudflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				pline("It stops raining.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_raincloudflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Suddenly, rain starts to pour down from the sky." :
						"It starts to rain." );
			known = TRUE;
			break;
		}

	break;

	case SCR_FLOODING:
		known = TRUE;
		{
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (confused) radius += 5;
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_megaflooding,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Whoa, swimming pools and stuff!" :
						"The dungeon is flooded!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_GROWTH:
		known = TRUE;

		if (sobj && sobj->oartifact == ART_PANIC_IN_GOTHAM_FOREST) {
		    int i, j, bd = 100;
		    for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ == ROOM || levl[u.ux + i][u.uy + j].typ == CORR) {
				levl[u.ux + i][u.uy + j].typ = TREE;
				blockorunblock_point(u.ux + i,u.uy + j);
				if (!(levl[u.ux + i][u.uy + j].wall_info & W_EASYGROWTH)) levl[u.ux + i][u.uy + j].wall_info |= W_HARDGROWTH;
				del_engr_at(u.ux + i, u.uy + j);
	
				newsym(u.ux + i,u.uy + j);
			}

		    }
			pline("Uh-oh... there has been a strange increase in the number of trees lately. This is of course very dangerous :-), because if it turns out that this is the work of Poison Ivy, we'll have a major panic on our hands.");
			known = TRUE;
			break;

		} else if (confused) {
			/* remove lava from vicinity of player */
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_treeflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				You_feel("a lack of greenery.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_treeflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Uh... everything is so... green!?" :
						"You see trees growing out of the ground!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_ICE:
		known = TRUE;
		if (confused) {
			/* remove lava from vicinity of player */
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_iceflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				You("stop feeling cold.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_iceflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Damn, this is giving you the chills!" :
						"The floor crackles with ice!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_STONING:
		known = TRUE;
		if (confused) {

			if (Stoned) fix_petrification();

		} else {

		    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
			!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("You are already stoned.");
				else {
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					pline("You start turning to stone!");
				}
			}
			sprintf(killer_buf, "reading a petrification scroll");
			delayed_killer = killer_buf;
		
		    }

		}

	break;

	case SCR_CLOUDS:
		known = TRUE;
		if (confused) {
			/* remove lava from vicinity of player */
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_cloudflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				You_feel("the fog disappearing.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_cloudflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Wow! Floating clouds..." :
						"Foggy clouds appear out of thin air!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_BARRHING:
		known = TRUE;
		if (confused) {
			/* remove lava from vicinity of player */
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_barflood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				You("have a sense of freedom.");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_barflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Aw shit, this feels like being in a jail!" :
						"Iron bars shoot up from the ground!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_CHAOS_TERRAIN:
		known = TRUE;
		if (confused) {
			/* do nothing */
			pline("It seems this was quite a normal scroll.");
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5-2*bcsign(sobj);
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
				do_clear_areaX(u.ux, u.uy, radius, do_terrainflood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* we do not put these on the player's position. */
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"Oh wow, look at all the stuff that is happening around you!" :
						"What the heck is happening to the dungeon?!" );
			known = TRUE;
			break;
		}

	break;

	case SCR_FLOOD_TIDE:
	{

		int randomx, randomy;
		int randomamount = rnd(250 - 100*bcsign(sobj) );
		if (confused) randomamount *= 5;

		while (randomamount) {
			randomamount--;
			randomx = rn1(COLNO-3,2);
			randomy = rn2(ROWNO);
			if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
				levl[randomx][randomy].typ = MOAT;
				del_engr_at(randomx, randomy);
		
				newsym(randomx,randomy);
	
			}
		}

		pline("A flood surges through the entire dungeon!");

	}

	break;

	case SCR_FROST:
	{

		int randomx, randomy;
		int randomamount = rnd(250 - 100*bcsign(sobj) );
		if (confused) randomamount *= 5;

		while (randomamount) {
			randomamount--;
			randomx = rn1(COLNO-3,2);
			randomy = rn2(ROWNO);
			if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
				levl[randomx][randomy].typ = ICE;
				del_engr_at(randomx, randomy);
		
				newsym(randomx,randomy);
	
			}
		}

		pline("A chilly wind blows through the dungeon...");

	}

	break;

	case SCR_CREATE_FACILITY:
	{

		int randomx, randomy;
		int randomamount = rnd(250 - 100*bcsign(sobj) );
		if (confused) randomamount *= 5;
		int terrainfeature;

		while (randomamount) {
			randomamount--;
			randomx = rn1(COLNO-3,2);
			randomy = rn2(ROWNO);
			if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

				terrainfeature = randomwalltype();
				if (!rn2(50)) terrainfeature = FOUNTAIN;
				else if (!rn2(2000)) terrainfeature = THRONE;
				else if (!rn2(10000)) terrainfeature = PENTAGRAM;
				else if (!rn2(5000)) terrainfeature = WELL;
				else if (!rn2(5000)) terrainfeature = POISONEDWELL;
				else if (!rn2(5000)) terrainfeature = WAGON;
				else if (!rn2(5000)) terrainfeature = BURNINGWAGON;
				else if (!rn2(4000)) terrainfeature = WOODENTABLE;
				else if (!rn2(5000)) terrainfeature = CARVEDBED;
				else if (!rn2(2000)) terrainfeature = STRAWMATTRESS;
				else if (!rn2(100)) terrainfeature = SINK;
				else if (!rn2(200)) terrainfeature = TOILET;
				else if (!rn2(50)) terrainfeature = GRAVE;
				else if (!rn2(10000) && !Is_astralevel(&u.uz) ) terrainfeature = ALTAR;

				levl[randomx][randomy].typ = terrainfeature;
				if (terrainfeature == ALTAR) levl[randomx][randomy].altarmask = Align2amask(A_NONE);


				del_engr_at(randomx, randomy);
		
				newsym(randomx,randomy);
	
			}
		}

		pline("Crash!  Whang!  Klock!  Lots of noises suggest massive changes to the dungeon structure...");

	}

	break;

	case SCR_EBB_TIDE:
	{

		int randomx, randomy;
		int randomamount = rnd(250 - 100*bcsign(sobj) );
		if (confused) randomamount *= 5;

		while (randomamount) {
			randomamount--;
			randomx = rn1(COLNO-3,2);
			randomy = rn2(ROWNO);
			if (isok(randomx, randomy) && (levl[randomx][randomy].typ == POOL || levl[randomx][randomy].typ == WATER || levl[randomx][randomy].typ == MOAT) ) {
				levl[randomx][randomy].typ = ROOM;
				del_engr_at(randomx, randomy);
		
				newsym(randomx,randomy);
	
			}
		}

		pline("The dungeon seems more dry than before.");

	}

	break;

	case SCR_FLOOD:
		known = TRUE;
		if (confused) {
			/* remove water from vicinity of player */
			int maderoom = 0;
			do_clear_areaX(u.ux, u.uy, 4+2*bcsign(sobj),
					undo_flood, (void *)&maderoom);
			if (maderoom) {
				known = TRUE;
				You("are suddenly very dry!");
			}
		} else {
			int madepool = 0;
			int stilldry = -1;
			int x,y,safe_pos=0;
			int radius = 5;
			if (!rn2(3)) radius += rnd(4);
			if (!rn2(10)) radius += rnd(6);
			if (!rn2(25)) radius += rnd(8);
			if (radius > MAX_RADIUS) radius = MAX_RADIUS;
			if (!sobj->cursed)
				do_clear_areaX(u.ux, u.uy, radius, do_flood,
						(void *)&madepool);

			/* check if there are safe tiles around the player */
			for (x = u.ux-1; x <= u.ux+1; x++) {
				for (y = u.uy - 1; y <= u.uy + 1; y++) {
					if (x != u.ux && y != u.uy &&
					    goodpos(x, y, &youmonst, 0)) {
						safe_pos++;
					}
				}
			}

			/* cursed and uncursed might put a water tile on
			 * player's position */
			if (!sobj->blessed && safe_pos > 0)
				do_flood(u.ux, u.uy, (void *)&stilldry);
			if (!madepool && stilldry)
				break;
			if (madepool)
				pline(FunnyHallu ?
						"A totally gnarly wave comes in!" :
						"A flood surges through the area!" );
			if (!stilldry && !Wwalking && !Race_if(PM_KORONST) && !Flying && !Levitation)
				drown();
			known = TRUE;
			break;
		}
		break;
	case SPE_COMMAND_UNDEAD:
		/* Amy edit: nerf - no longer guaranteed to work, and has a small chance of frenzying the monster instead
		 * the chance of failure is much lower than that of charm monster, considering this is undead-specific */
		if (confused) break;
		if (u.uswallow) {
		    if (is_undead(u.ustuck->data) || u.ustuck->egotype_undead) maybe_tame(u.ustuck, sobj);
		} else {
		    int i, j, bd = confused ? 5 : 1;
		    struct monst *mtmp;

		    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0 &&
				(is_undead(mtmp->data) || mtmp->egotype_undead) )
			    if (!rn2(2)) maybe_tame(mtmp, sobj);

			    else if (!rn2(15) && !((rnd(30 - ACURR(A_CHA))) < 4) && !mtmp->mfrenzied && !mtmp->mtame) {
				pline("Instead of being tamed, %s enters a state of frenzy!", mon_nam(mtmp));
				mtmp->mpeaceful = 0;
				mtmp->mfrenzied = 1;
			    }

		    }
		}

		if (!rn2(10)) {
			pline("The spell backfires!");
			badeffect();
		}

		break;
	case SCR_ITEM_GENOCIDE:
		You("have found a scroll of item genocide!");
		known = TRUE;
		char buf[BUFSZ];
		int tries = 0;
retry:
		getlin("What item do you want to genocide?", buf);
		if(buf[0] == '\033') buf[0] = 0;
		struct obj *otmpY, nothing;

		/* If confused, you will always genocide something, and you won't know what. --Amy 
		 * Sometimes it also happens if the scroll was cursed. */

		otmpY = (confused || (sobj->cursed && rn2(2) )) ? mkobj(RANDOM_CLASS, TRUE, FALSE) : readobjnam(buf, &nothing, TRUE, FALSE);
		if (!otmpY) {
		    pline("Nothing fitting that description exists in the game.");
		    if (++tries < 5) goto retry;
		    pline("%s", thats_enough_tries);
			break;
		} else if (otmpY == &nothing) {
		    break;
		} else if ((otmpY->otyp == GOLD_PIECE) || (otmpY->otyp == STRANGE_OBJECT) || (otmpY->otyp == AMULET_OF_YENDOR) || (otmpY->otyp == CANDELABRUM_OF_INVOCATION) || (otmpY->otyp == BELL_OF_OPENING) || (otmpY->otyp == SPE_BOOK_OF_THE_DEAD) || (objects[otmpY->otyp].oc_prob < 1)) {
		    pline("That item cannot be genocided.");
		    if (++tries < 5) goto retry;
		    pline("%s", thats_enough_tries);
			break;

		}
		if (otmpY != &zeroobj) {
			u.unobtainablegeno = otmpY->otyp;
			if (!confused) pline("All %s (%s) items can no longer be generated.", obj_descr[u.unobtainablegeno].oc_name, obj_descr[u.unobtainablegeno].oc_descr);
			else pline("In your confusion, you genocided some item. But you forgot what it is.");
		}

		if (otmpY) {
			if (Has_contents(otmpY))
				delete_contents(otmpY);
			obfree(otmpY, (struct obj *)0);
		}

		break;

	case SCR_GENOCIDE:
		You("have found a scroll of genocide!");
		known = TRUE;

		if (sobj->oartifact == ART_TOTAL_GENOCIDE) do_total_genocide();
		else if (sobj->blessed) {	/* improvement by Amy */
			do_genocide(1);
			do_class_genocide();
			}
		else do_genocide(!sobj->cursed | (2 * !!Confusion));
		break;
	case SCR_LIGHT:
		if(!Blind) known = TRUE;
		litroom(!confused && !sobj->cursed, sobj);
		break;

	case SCR_WORLD_FALL:

		if (confused) {

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			badeffect();
			badeffect();
			badeffect();
			badeffect();
			badeffect();
			badeffect();
			badeffect();
			while (rn2(100)) (void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
			NastinessProblem += rnz(1000 * (monster_difficulty() + 1));

			u.aggravation = 0;

			break;

		}

		You("scream \"EYGOORTS-TOGAAL, JEZEHH!\"");
		known = TRUE;
/* Actually, it's "To win the game you must kill me, John Romero" recorded backwards.
   When I was little, I always thought it said "Eygoorts-togaal, jezehh". --Amy */
		{
		int num;
	      register struct monst *mtmp, *mtmp2;
		num = 0;
		int wflvl = (u.ulevel / 2);
		if (wflvl < 1) wflvl = 1;

		for (mtmp = fmon; mtmp; mtmp = mtmp2) {
			mtmp2 = mtmp->nmon;
			if ( ((mtmp->m_lev < wflvl) || (!rn2(4) && mtmp->m_lev < (2 * wflvl))) && mtmp->mnum != quest_info(MS_NEMESIS) && !(mtmp->data->geno & G_UNIQ) && (!sobj->cursed || rn2(2) ) ) {
				mondead(mtmp);
				num++;
			}
	      }
		pline("Eliminated %d monster%s.", num, plur(num));
		}

		break;

	case SCR_TELEPORTATION:
		if(confused || sobj->cursed) 
			{
		      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
			else pline("You try to teleport, but fail!");
			}
		else {
			if (sobj->blessed && !Teleport_control) {
				known = TRUE;
				HTeleport_control = 2;
				/* if (yn("Do you wish to teleport?")=='n')
				 *         break;
				 */
			}
			tele();
			if(Teleport_control || !couldsee(u.ux0, u.uy0) ||
			   (distu(u.ux0, u.uy0) >= 16))
				known = TRUE;
		}
		break;

	case SCR_RELOCATION:
		known = TRUE;
	      (void) safe_teleds(FALSE);

		break;

	case SCR_PHASE_DOOR:
		known = TRUE;
		if(confused) 
			{
			phase_door(1);
			}
		else {
			phase_door(0);
		}
		break;
	case SCR_TELE_LEVEL:
	      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
		else pline("Hmm... that level teleport scroll didn't do anything.");
		known = TRUE;
		break;
	case SCR_WARPING:
		known = TRUE;
		if (((u.uevent.udemigod || u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); (void) safe_teleds(FALSE); break;}

		if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || (iszapem && !(u.zapemescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
			pline("You're unable to warp!"); break;}

		/* restore the "no cyanide rule"... this is a simple y/n prompt because if you're read-identifying unknown
		 * scrolls already, it's your own damn fault if you then somehow think it's a good idea to skip past the
		 * prompt. If you're not gonna read the scroll effects anyway, why bother reading random ones in the first
		 * place? You're supposed to read what's on the screen when use-testing consumables :P --Amy */
		if (!confused) {
			if (yn_function("You have found a scroll of warping! Do you want to warp to a random dungeon level?", "yn", 'y') == 'n') break;
		}

		make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

		u.cnd_banishmentcount++;
		if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
		else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

		/*(void) safe_teleds(FALSE);

		goto_level((&medusa_level), TRUE, FALSE, FALSE);*/
		register int newlev = rnd(99);
		d_level newlevel;
		get_level(&newlevel, newlev);
		goto_level(&newlevel, TRUE, FALSE, FALSE);

		break;
	case SCR_GOLD_DETECTION:
		if (confused || sobj->cursed) return(trap_detect(sobj));
		else return(gold_detect(sobj));
	case SPE_DETECT_FOOD:
		if (confused) break;
	case SCR_FOOD_DETECTION:
		if (sobj->otyp == SCR_FOOD_DETECTION) u.uhunger += (20 + (sobj->blessed * 30));
		if (food_detect(sobj))
			return(1);	/* nothing detected */
		break;
	case SPE_IDENTIFY:
		if (confused) break;
		if (!rn2(100)) {

			forget(3);
			pline("Oh, no! Your mind has gone blank!");
			return(1);
		}
		cval = rnd(4);
		if (rn2(7)) cval = 1; /* hardcore nerf by Amy */
		goto id;
	case SCR_IDENTIFY:
		/* known = TRUE; */

		if (evilfriday && sobj->cursed && confused) { /* thanks Porkman */
			forget(rnd(10));
			You("forget everything else while identifying this as an identify scroll.");
			break;
		}

		if (sobj->cursed) { /* Credits go to TvTropes for creating this joke. The player doesn't actually die. --Amy */
			pline("You fall below the bottom of the page! You die...");
			if (isevilvariant) {
				char qbuf[QBUFSZ];
				sprintf(qbuf, "Do you want your possessions identified? DYWYPI?");
				yn_function(qbuf, ynqchars, 'y');
			} else pline("Do you want your possessions identified? DYWYPI? [ynq] (n) _");
		}
		if(confused)
			You("identify this as an identify scroll.");
		else
			pline("This is an identify scroll.");
		if (sobj->blessed || (!sobj->cursed && !rn2(3))) {
			cval = rn2(4);
			if (sobj->blessed && !rn2(3) && cval) cval += rnz(2);
			/* Note: if rn2(5)==0, identify all items */
			if (cval == 1 && sobj->blessed && Luck > 0) ++cval;
		} else	cval = 1;
		if(!objects[sobj->otyp].oc_name_known) more_experienced(0,10);
		if (carried(sobj)) useup(sobj);
		else useupf(sobj, 1L);
		makeknown(SCR_IDENTIFY);
	id:
		if(invent && !confused) {
		    identify_pack(cval, 0, sobj->otyp == SPE_IDENTIFY ? TRUE : FALSE);
		}
		if (!rn2(5) && sobj->otyp == SPE_IDENTIFY) {
			pline("The spell backlashes!");
			badeffect();
		}
		return(1);
	case SCR_INVENTORY_ID: /* always identifies the player's entire inventory --Amy */
		makeknown(SCR_INVENTORY_ID);
		if(confused)
			You("identify this as an inventory id scroll.");
		else
			pline("This is an inventory id scroll.");

		if(!objects[sobj->otyp].oc_name_known) more_experienced(0,10);
		if (carried(sobj)) useup(sobj);
		else useupf(sobj, 1L);

		if(invent && !confused) {
		    identify_pack(0, 0, 0);
		}
		return(1);
	case SCR_STANDARD_ID: /* always identifies exactly one item --Amy */
		makeknown(SCR_STANDARD_ID);
		pline("This is a standard identify scroll.");

		if (carried(sobj)) useup(sobj);
		else useupf(sobj, 1L);

		if(invent) {
		    identify_pack(1, 0, 0);
		}
		return(1);
	case SCR_HEALING: /* a basic healing item that can be used to - who would have guessed? - cure wounds! --Amy */
		makeknown(SCR_HEALING);
		You_feel("healthier!");
			if (!rn2(20)) healup(400 + rnz(u.ulevel), 0, FALSE, FALSE);
			else if (!rn2(5)) healup(d(6,8) + rnz(u.ulevel), 0, FALSE, FALSE);
			else healup(d(5,6) + rnz(u.ulevel), 0, FALSE, FALSE);
		break;
	case SCR_EXTRA_HEALING: /* better healing scroll --Amy */
		makeknown(SCR_EXTRA_HEALING);
		You_feel("healthier!");
			if (!rn2(20)) healup(2000 + rnz(u.ulevel * 5), 0, FALSE, FALSE);
			else if (!rn2(5)) healup(d(6,40) + rnz(u.ulevel * 5), 0, FALSE, FALSE);
			else healup(d(5,30) + rnz(u.ulevel * 5), 0, FALSE, FALSE);
		break;
	case SCR_POWER_HEALING:
		makeknown(SCR_POWER_HEALING);
		You_feel("fully healed!");
			u.uhp = u.uhpmax;
			if (Upolyd) u.mh = u.mhmax;
		break;
	case SCR_HEAL_OTHER:
		makeknown(SCR_HEAL_OTHER);
		pline("A healing aura surrounds you...");

		int healamount = (rnd(50) + 30 + rnz(u.ulevel * 3));
		if (uarmc && itemhasappearance(uarmc, APP_NURSE_CLOAK)) healamount *= 2;
		if (uarmh && uarmh->oartifact == ART_SEXYNESS_HAS_A_NAME) {
			healamount *= 2;
			if (Role_if(PM_HEALER)) healamount *= 2;
		}
		if (uwep && uwep->oartifact == ART_HEALHEALHEALHEAL) {
			healamount *= 2;
			if (Role_if(PM_HEALER)) healamount *= 2;
		}
		if (uarmf && uarmf->oartifact == ART_KATIE_MELUA_S_FLEECINESS) {
			healamount *= 2;
			if (Role_if(PM_HEALER)) healamount *= 2;
		}
		if (RngeNursery) healamount *= 2;

		if (uactivesymbiosis) {
			Your("symbiote seems healthier!");
			u.usymbiote.mhp += (rnd(50) + 30 + rnz(u.ulevel * 3));
			if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
			if (u.ualign.type == A_LAWFUL) adjalign(1);
		}
		{

			int i, j, bd = confused ? 6 : 4;
			struct monst *frostmon;

			for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((frostmon = m_at(u.ux + i, u.uy + j)) != 0) {
					if (frostmon->mtame) {
						frostmon->mfrozen = 0;
						frostmon->msleeping = 0;
						frostmon->masleep = 0;
						frostmon->mcanmove = 1;
						frostmon->mflee = 0;
						frostmon->mcansee = 1;
						frostmon->mblinded = 0;
						frostmon->mstun = 0;
						frostmon->mconf = 0;
						pline("%s is cured.", Monnam(frostmon));
						frostmon->mhp += (rnd(50) + 30 + rnz(u.ulevel * 3));
						if (frostmon->mhp > frostmon->mhpmax) frostmon->mhp = frostmon->mhpmax;
						if (u.ualign.type == A_LAWFUL) adjalign(1);
					}
				}
			}
			if (u.usteed) {
				u.usteed->mfrozen = 0;
				u.usteed->msleeping = 0;
				u.usteed->masleep = 0;
				u.usteed->mcanmove = 1;
				u.usteed->mflee = 0;
				u.usteed->mcansee = 1;
				u.usteed->mblinded = 0;
				u.usteed->mstun = 0;
				u.usteed->mconf = 0;
				pline("%s is cured.", Monnam(u.usteed));
				u.usteed->mhp += (rnd(50) + 30 + rnz(u.ulevel * 3));
				if (u.usteed->mhp > u.usteed->mhpmax) u.usteed->mhp = u.usteed->mhpmax;
				if (u.ualign.type == A_LAWFUL) adjalign(1);
			}

		}

		break;
	case SCR_WOUNDS:
		makeknown(SCR_WOUNDS);
		You_feel("bad!");
			if (!rn2(20)) losehp(d(10,8), "a scroll of wounds", KILLED_BY);
			else if (!rn2(5)) losehp(d(6,8), "a scroll of wounds", KILLED_BY);
			else losehp(d(4,6), "a scroll of wounds", KILLED_BY);
		break;
	case SCR_MANA: /* there was no simple mana potion in this game! --Amy */
		makeknown(SCR_MANA);
		You_feel("full of mystic power!");
			if (!rn2(20)) u.uen += (400 + rnz(u.ulevel));
			else if (!rn2(5)) u.uen += (d(6,8) + rnz(u.ulevel));
			else u.uen += (d(5,6) + rnz(u.ulevel));
			if (u.uen > u.uenmax) u.uen = u.uenmax;
		break;
	case SCR_GREATER_MANA_RESTORATION: /* restores more mana --Amy */
		makeknown(SCR_GREATER_MANA_RESTORATION);
		You_feel("full of mystic power!");
			if (!rn2(20)) u.uen += (2000 + rnz(u.ulevel * 5));
			else if (!rn2(5)) u.uen += (d(6,40) + rnz(u.ulevel * 5));
			else u.uen += (d(5,30) + rnz(u.ulevel * 5));
			if (u.uen > u.uenmax) u.uen = u.uenmax;
		break;
	case SCR_CURE: /* cures bad effects --Amy */
		makeknown(SCR_CURE);
		You_feel("revitalized.");
		if (Stoned) fix_petrification();
		    if (Slimed) {
			pline("The slime disappears.");
			Slimed =0;
		    }
		    make_sick(0L, (char *) 0, FALSE, SICK_ALL);
			make_blinded(0L,FALSE);
		    make_stunned(0L,TRUE);
		    make_confused(0L,TRUE);
		    (void) make_hallucinated(0L,FALSE,0L);
		    make_numbed(0L,TRUE);
		    make_feared(0L,TRUE);
		    make_frozen(0L,TRUE);
		    make_burned(0L,TRUE);
		    make_dimmed(0L,TRUE);

		break;
	case SPE_CHARGING:
		if (confused) break;
	case SCR_CHARGING:
		if (confused) {
		    You_feel("charged up!");
		    if (u.uen < u.uenmax)
			u.uen = rn2(20) ? u.uenmax : (u.uenmax += d(5,4));
		    else
			u.uen = (u.uenmax += d(5,4));
		    flags.botl = 1;
		    break;
		}
		known = TRUE;
		pline("You may charge an object."); /* "this is a scroll of charging" is inappropriate for the spell --Amy */
chargingchoice:
		otmp = getobj(all_count, "charge");
		if (!otmp) {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to charge your items.");
			else goto chargingchoice;
			break;
		}
		recharge(otmp, sobj->cursed ? -1 : (sobj->blessed ? 1 : 0));
		break;

	case SCR_RANDOM_ENCHANTMENT:
		known = TRUE;
		pline("You may randomly enchant an object.");
		if (confused) {
confusedrandenchchoice:
			otmp = getobj(all_count, "randomly enchant");
			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to randomly enchant objects.");
				else goto confusedrandenchchoice;
				break;
			}
			randomenchant(otmp, sobj->cursed ? -1 : (sobj->blessed ? 1 : 0), 1);
		} else {
randenchchoice:
			otmp = getobj(all_count, "randomly enchant");
			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to randomly enchant objects.");
				else goto randenchchoice;
				break;
			}
			randomenchant(otmp, sobj->cursed ? -1 : (sobj->blessed ? 1 : 0), 0);
			if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
				if (!otmp->cursed) bless(otmp);
				else uncurse(otmp, FALSE);
			}
		}
		break;

	case SCR_GAIN_MANA:
		    You_feel("full of mystic power!");
		    if (u.uen < u.uenmax)
			u.uen = rn2(20) ? u.uenmax : (u.uenmax += d(5,4));
		    else
			u.uen = (u.uenmax += d(5,4));
		    flags.botl = 1;
		    known = TRUE;
		    break;

	case SCR_WARD: /* half physical damage for a period of time */

		if (confused) {
			if (Upolyd) u.mh -= rn2(u.mh);
			else u.uhp -= rn2(u.uhp);
			pline("A steel plunger crashes down and severely crushes you!");
		} else {
			if (sobj->cursed) {
				HHalf_physical_damage &= ~INTRINSIC;
				HHalf_physical_damage &= ~TIMEOUT;
				You_feel("less protected!");
			} else {
				if (FunnyHallu)
					You_feel("like a tough motherfucker!");
				else
					You("are resistant to normal damage.");
				incr_itimeout(&HHalf_physical_damage, rnd(500));
				if (sobj->blessed) incr_itimeout(&HHalf_physical_damage, rnd(500));
			}
		}
		break;

	case SCR_WARDING: /* half spell damage for a period of time */

		if (confused) {
			u.uen -= rn2(u.uen);
			pline("Your magical power is sucked away!");

		} else {
			if (sobj->cursed) {
				HHalf_spell_damage &= ~INTRINSIC;
				HHalf_spell_damage &= ~TIMEOUT;
				You_feel("more afraid of magic!");
			} else {
				incr_itimeout(&HHalf_spell_damage, rnd(500));
				if (sobj->blessed) incr_itimeout(&HHalf_spell_damage, rnd(500));
				if (FunnyHallu)
					pline("Let the casting commence!");
				else
					You_feel("a sense of spell knowledge.");
			}
		}
		break;

	case SCR_MAGIC_MAPPING:
		if (level.flags.nommap) {
		    Your("mind is filled with crazy lines!");
		    if (FunnyHallu)
			pline("Wow!  Modern art.");
		    else
			Your("%s spins in bewilderment.", body_part(HEAD));
		    make_confused(HConfusion + rnd(30), FALSE);
		    if (!rn2(3)) badeffect();
		    break;
		}
		if (sobj->blessed && !(sobj->oartifact == ART_MARAUDER_S_MAP)) {
		    register int x, y;

		    for (x = 1; x < COLNO; x++)
		    	for (y = 0; y < ROWNO; y++)
		    	    if (levl[x][y].typ == SDOOR)
		    	    	cvt_sdoor_to_door(&levl[x][y]);
		    /* do_mapping() already reveals secret passages */
		}
		known = TRUE;
	case SPE_MAGIC_MAPPING:
		if (confused && sobj->otyp == SPE_MAGIC_MAPPING) break;
		if (level.flags.nommap) {
		    Your("%s spins as %s blocks the spell!", body_part(HEAD), something);
		    make_confused(HConfusion + rnd(30), FALSE);
		    break;
		}
		if(!(sobj->oartifact == ART_MARAUDER_S_MAP)){
		if (sobj->otyp == SPE_MAGIC_MAPPING) pline("You grasp some bits from the current map!");
		else pline("A map coalesces in your mind!");
		cval = (sobj->cursed && !confused);
		if(cval) HConfusion = 1;	/* to screw up map */
		if (sobj->otyp == SPE_MAGIC_MAPPING) do_mappingX();
		else do_mapping();
		if (sobj->blessed && !cval) /* objects, too, pal! */
		  object_detect(sobj,0);
		if(cval) {
		    HConfusion = 0;		/* restore */
		    pline("Unfortunately, you can't grasp the details.");
		}
		}
		else{ /* sobj is an artifact */
			if (u.ublesscnt > 0) {pline("The map reveals nothing."); break;}
			if(sobj->age > monstermoves){
				pline("The map %s hard to see.", vtense((char *)0,"are"));
				nomul(-(rnd(3)), "reading an artifact map", TRUE);
				sobj->age += (long) d(3,10);
			} else sobj->age = monstermoves + (long) d(3,10);
			do_vicinity_map();
			u.ublesscnt += rnz(300);
		}
		break;
	case SCR_WONDER:
		known = TRUE;

		if (sobj->oartifact == ART_ANASTASIA_S_PERILOUS_GAMBL) {
		      u.uprops[EVIL_PATCH_EFFECT].intrinsic |= FROMOUTSIDE;
			int wondertech = rnd(MAXTECH-1);
			if (!tech_known(wondertech)) {
			    	learntech(wondertech, FROMOUTSIDE, 1);
				You("learn how to perform a new technique! But you also gained the intrinsic evilpatch effect...");
			}
		}

		if (sobj->cursed) {
			forget(ALL_SPELLS|ALL_MAP);
			pline("The scroll was cursed! You lose a lot of knowledge...");
			break;
		}
		wonderspell();

		break;
	case SCR_AMNESIA:
		known = TRUE;
		forget(	(!sobj->blessed ? ALL_SPELLS : 0) |
			(!confused || sobj->cursed ? ALL_MAP : 0) );
		if (FunnyHallu) /* Ommmmmm! */
			Your("mind releases itself from mundane concerns.");
		else if (!strncmpi(plname, "Maud", 4) || !strncmpi(plalias, "Maud", 4))
			pline("As your mind turns inward on itself, you forget everything else.");
		else if (rn2(2))
			pline("Who was that Maud person anyway?");
		else
			pline("Thinking of Maud you forget everything else.");
		exercise(A_WIS, FALSE);
		break;
	case SCR_INSTANT_AMNESIA:
		known = FALSE;
		/* not known! You forget about this scroll while reading it :D --Amy */
		forget(	(!sobj->blessed ? ALL_SPELLS : 0) |
			(!confused || sobj->cursed ? ALL_MAP : 0) );
		if (FunnyHallu) /* Ommmmmm! */
			Your("mind releases itself from mundane concerns.");
		else if (!strncmpi(plname, "Maud", 4) || !strncmpi(plalias, "Maud", 4))
			pline("As your mind turns inward on itself, you forget everything else.");
		else if (rn2(2))
			pline("Who was that Maud person anyway?");
		else
			pline("Thinking of Maud you forget everything else.");
		exercise(A_WIS, FALSE);
		known = FALSE;
		break;

	case SCR_FIRE:
		/*
		 * Note: Modifications have been made as of 3.0 to allow for
		 * some damage under all potential cases.
		 */
		cval = bcsign(sobj);
		if(!objects[sobj->otyp].oc_name_known) more_experienced(0,10);
		if (carried(sobj)) useup(sobj);
		else useupf(sobj, 1L);
		makeknown(SCR_FIRE);
		if(confused) {
		    if(Fire_resistance) {
			shieldeff(u.ux, u.uy);
			if(!Blind)
			    pline("Oh, look, what a pretty fire in your %s.",
				makeplural(body_part(HAND)));
			else You_feel("a pleasant warmth in your %s.",
				makeplural(body_part(HAND)));
		    } else {
			pline_The("scroll catches fire and you burn your %s.",
				makeplural(body_part(HAND)));
			losehp(1, "scroll of fire", KILLED_BY_AN);
		    }
		    return(1);
		}
		if (Underwater)
			pline_The("water around you vaporizes violently!");
		else {
		    pline_The("scroll erupts in a tower of flame!");
		    burn_away_slime();
		}
		explode(u.ux, u.uy, ZT_SPELL(ZT_FIRE), (2*(rn1(3, 3) + 2 * cval) + 1)/3,
							SCROLL_CLASS, EXPL_FIERY);
		return(1);
	case SCR_EARTH:

	    /* TODO: handle steeds */
	    if (
#ifdef REINCARNATION
		!Is_rogue_level(&u.uz) && 
#endif
	    	 (!In_endgame(&u.uz) || Is_earthlevel(&u.uz))) {
	    	register int x, y;

	    	/* Identify the scroll */
	    	pline_The("%s rumbles %s you!", ceiling(u.ux,u.uy),
	    			sobj->blessed ? "around" : "above");
	    	known = 1;
	    	if (In_sokoban(&u.uz))
			{change_luck(-1);
			pline("You cheater!");
			if (evilfriday) u.ugangr++;
			}
			/* Sokoban guilt */

	    	/* Loop through the surrounding squares */
	    	if (!sobj->cursed) for (x = u.ux-1; x <= u.ux+1; x++) {
	    	    for (y = u.uy-1; y <= u.uy+1; y++) {

	    	    	/* Is this a suitable spot? */
	    	    	if (isok(x, y) && !closed_door(x, y) &&
	    	    			!IS_ROCK(levl[x][y].typ) &&
	    	    			!IS_AIR(levl[x][y].typ) &&
					(x != u.ux || y != u.uy)) {
			    register struct obj *otmp2;
			    register struct monst *mtmp;

	    	    	    /* Make the object(s) */
	    	    	    otmp2 = mksobj(confused ? ROCK : BOULDER, FALSE, FALSE, FALSE);
	    	    	    if (!otmp2) continue;  /* Shouldn't happen */

				if(!rn2(8)) {
					otmp2->spe = rne(2);
					if (rn2(2)) otmp2->blessed = rn2(2);
					 else	blessorcurse(otmp2, 3);
				} else if(!rn2(10)) {
					if (rn2(10)) curse(otmp2);
					 else	blessorcurse(otmp2, 3);
					otmp2->spe = -rne(2);
				} else	blessorcurse(otmp2, 10);

	    	    	    otmp2->quan = confused ? rn1(5,2) : 1;
	    	    	    otmp2->owt = weight(otmp2);

	    	    	    /* Find the monster here (won't be player) */
	    	    	    mtmp = m_at(x, y);
	    	    	    if (mtmp && !amorphous(mtmp->data) && (!mtmp->egotype_wallwalk) &&
	    	    	    		!passes_walls(mtmp->data) &&
	    	    	    		!noncorporeal(mtmp->data) &&
	    	    	    		!unsolid(mtmp->data)) {
				struct obj *helmet = which_armor(mtmp, W_ARMH);
				int mdmg;

				if (cansee(mtmp->mx, mtmp->my)) {
				    pline("%s is hit by %s!", Monnam(mtmp),
	    	    	    			doname(otmp2));
				    if ((mtmp->minvis && !canspotmon(mtmp)) || mtmp->minvisreal)
					map_invisible(mtmp->mx, mtmp->my);
				}
	    	    	    	mdmg = dmgval(otmp2, mtmp) * otmp2->quan;
				if (helmet) {
				    if(is_metallic(helmet)) {
					if (canspotmon(mtmp))
					    pline("Fortunately, %s is wearing a hard helmet.", mon_nam(mtmp));
					else if (flags.soundok)
					    You_hear("a clanging sound.");
					if (mdmg > 2) mdmg = 2;
				    } else {
					if (canspotmon(mtmp))
					    pline("%s's %s does not protect %s.",
						Monnam(mtmp), xname(helmet),
						mhim(mtmp));
				    }
				}
	    	    	    	mtmp->mhp -= mdmg;
	    	    	    	if (mtmp->mhp <= 0)
	    	    	    	    xkilled(mtmp, 1);
	    	    	    }
	    	    	    /* Drop the rock/boulder to the floor */
	    	    	    if (!flooreffects(otmp2, x, y, "fall")) {
	    	    	    	place_object(otmp2, x, y);
	    	    	    	stackobj(otmp2);
	    	    	    	newsym(x, y);  /* map the rock */
	    	    	    }
	    	    	}
		    }
		}
		/* Attack the player */
		if (!sobj->blessed) {
		    int dmg;
		    struct obj *otmp2;

		    /* Okay, _you_ write this without repeating the code */
		    otmp2 = mksobj(confused ? ROCK : BOULDER, FALSE, FALSE, FALSE);
		    if (!otmp2) break;

			if(!rn2(8)) {
				otmp2->spe = rne(2);
				if (rn2(2)) otmp2->blessed = rn2(2);
				 else	blessorcurse(otmp2, 3);
			} else if(!rn2(10)) {
				if (rn2(10)) curse(otmp2);
				 else	blessorcurse(otmp2, 3);
				otmp2->spe = -rne(2);
			} else	blessorcurse(otmp2, 10);

		    otmp2->quan = confused ? rn1(5,2) : 1;
		    otmp2->owt = weight(otmp2);
		    if (!amorphous(youmonst.data) &&
				!Passes_walls &&
				!noncorporeal(youmonst.data) &&
				!unsolid(youmonst.data)) {
			You("are hit by %s!", doname(otmp2));
			dmg = dmgval(otmp2, &youmonst) * otmp2->quan;
			if (uarmh && !sobj->cursed) {
			    if(is_metallic(uarmh) && !is_etheritem(uarmh)) {
				pline("Fortunately, you are wearing a hard helmet.");
				if (dmg > 2) dmg = 2;
			    } else if (flags.verbose) {
				Your("%s does not protect you.",
						xname(uarmh));
			    }
			}
		    } else
			dmg = 0;
		    /* Must be before the losehp(), for bones files */
		    if (!flooreffects(otmp2, u.ux, u.uy, "fall")) {
			place_object(otmp2, u.ux, u.uy);
			stackobj(otmp2);
			newsym(u.ux, u.uy);
		    }
		    if (dmg) losehp(dmg, "scroll of earth", KILLED_BY_AN);
		}
	    }
	    break;
	case SCR_PUNISHMENT:
		known = TRUE;

		if (confused) {

			if (sobj->blessed) {
				if (u.ugangr) {
					u.ugangr--;
					if (u.ugangr) pline("%s seems %s.", u_gname(), FunnyHallu ? "groovy" : "slightly mollified");
					else pline("%s seems %s.", u_gname(), FunnyHallu ? "cosmic (not a new fact)" : "mollified");
				} else pline("%s seems %s.", u_gname(), FunnyHallu ? "high above the clouds" : "content");
			} else if (sobj->cursed) {
				u.ugangr++;
				prayer_done();
			} else {
				u.ugangr++;
			      You("get the feeling that %s is angry...", u_gname());
			}
			break;
		}

		if (evilfriday && sobj->cursed) {
			u.ugangr++;
		      You("get the feeling that %s is angry...", u_gname());
		}

		if(sobj->blessed) {
			You_feel("guilty.");
			break;
		}
		punish(sobj);
		break;
	case SCR_MASS_MURDER:
		known = TRUE;
		You("have found a scroll of mass murder.");
		do_genocide(sobj->cursed ? 8 : 9);
		if (sobj->blessed) { /* improvement by Amy */
			do_genocide(9);
		}
		break;
	case SCR_ERASURE:
		known = TRUE;
		pline("Now, you can wipe out monsters!");
		if (sobj->blessed) do_class_erasure();
		else if (Confusion || Hallucination) undo_genocide();
		else do_genocide(sobj->cursed ? 8 : 9);

		break;
	case SCR_UNDO_GENOCIDE:
		known = TRUE;
		You("have found a scroll of undo genocide.");
		undo_genocide();
		break;

	case SCR_SECURE_IDENTIFY:
		known = TRUE;
		if(confused) {
			You("identify this as a secure identify scroll.");
			break;
		} else {
			pline("This is a secure identify scroll.");

secureidchoice:
			otmp = getobj(all_count, "secure identify");

			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to secure identify your objects.");
				else goto secureidchoice;
				pline("A feeling of loss comes over you.");
				break;
			}
			if (otmp) {
				makeknown(otmp->otyp);
				if (otmp->oartifact) discover_artifact((int)otmp->oartifact);
				otmp->known = otmp->dknown = otmp->bknown = otmp->rknown = 1;
				if (otmp->otyp == EGG && otmp->corpsenm != NON_PM)
				learn_egg_type(otmp->corpsenm);
				prinv((char *)0, otmp, 0L);
			}

		}
		break;

	case SCR_ARMOR_SPECIALIZATION:
		pline("You may enchant a worn piece of armor.");
armorspecchoice:
		otmp = getobj(all_count, "magically enchant");
		if(!otmp) {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to enchant your armor.");
			else goto armorspecchoice;
			strange_feeling(sobj,"You feel magical for a moment, but the feeling passes.");
			exercise(A_WIS, FALSE);
			return(1);
		}
		if (!(otmp->owornmask & W_ARMOR) ) {

			strange_feeling(sobj, "You have a feeling of loss.");
			return(1);
		}

		if (otmp) {
			if (confused) {
				if (sobj->cursed) {
					if (otmp->spe > -20) {
						otmp->spe -= 10;
						p_glow2(otmp, NH_BLACK);
					}
				} else {
					otmp->spe = 0;
					otmp->blessed = 0;
					otmp->enchantment = 0;
					p_glow2(otmp, NH_GREEN);
				}
			} else {
				if (sobj->cursed) {
					otmp->enchantment = 0;
					p_glow2(otmp, NH_RED);
				} else {
					if (!otmp->enchantment) {
						long savewornmask;
						otmp->enchantment = randenchantment();
						savewornmask = otmp->owornmask;
						setworn((struct obj *)0, otmp->owornmask);
						setworn(otmp, savewornmask);
						p_glow2(otmp, NH_GOLDEN);
					} else pline("A feeling of loss comes over you.");
				}
			}
			if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
				if (!otmp->cursed) bless(otmp);
				else uncurse(otmp, FALSE);
			}
		}

		break;

	case SCR_REVERSE_IDENTIFY:
		known = TRUE;
		You("have found a scroll of reverse identify.");
		{
			int i,j,n;
			long oldgold;
			char buf[BUFSZ];

			n = (sobj->blessed) ? rn2(5)+1 : (sobj->cursed) ? 1 : rnd(3);
			for (i=0;i<n;i++) {
				for (j=0;j<=5;j++) {
					if (j >= 5) {
						pline("%s", thats_enough_tries);
						goto revid_end;
					}
					getlin("What do you want to identify?",buf);
					if (buf[0] == 0) continue;
					oldgold = u.ugold;
					otmp = readobjnam(buf, (struct obj *)0, TRUE, FALSE);
					if (u.ugold != oldgold) {
pline("Don't you date cheat me again! -- Your fault!");
						/* Make them pay */
						u.ugold = oldgold / 2;
						continue;
					}
					if (otmp == &zeroobj || otmp == (struct obj *) 0) {
						pline("That doesn't exist.");
						continue;
					}
					break;
				}
				strcpy(buf,xname(otmp));
				makeknown(otmp->otyp);
				pline("The %s is a %s.",buf,xname(otmp));
				if (Has_contents(otmp))
					delete_contents(otmp);
				obfree(otmp,(struct obj *) 0);
				/*if (otmp->oartifact) artifact_unexist(otmp);*/
			}
		}
revid_end:
		break;

	case SCR_ARTIFACT_JACKPOT:

		/* like artifact creation, but if it's a weapon, armor, ring or amulet, the base type is rerolled --Amy */

		known = TRUE;

		boolean havegifts = u.ugifts;

		if (!havegifts) u.ugifts++;

		acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
		if (acqo) {

		switch (acqo->oclass) {
			case WEAPON_CLASS:

				{

					int wpntype; /* 1 = launcher, 2 = ammo, 3 = melee */
					if (is_launcher(acqo)) wpntype = 1;
					else if (is_ammo(acqo) || is_missile(acqo)) wpntype = 2;
					else wpntype = 3;
reroll:
					acqo->otyp = rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1);
					if (wpntype == 1 && !is_launcher(acqo)) goto reroll;
					if (wpntype == 2 && !is_ammo(acqo) && !is_missile(acqo)) goto reroll;
					if (wpntype == 3 && (is_launcher(acqo) || is_ammo(acqo) || is_missile(acqo))) goto reroll;
				}

				break;
			case ARMOR_CLASS:

				{

					int armortype;
					/* 1 = shield, 2 = helmet, 3 = boots, 4 = gloves, 5 = cloak, 6 = shirt, 7 = suit */
					if (is_shield(acqo)) armortype = 1;
					else if (is_helmet(acqo)) armortype = 2;
					else if (is_boots(acqo)) armortype = 3;
					else if (is_gloves(acqo)) armortype = 4;
					else if (is_cloak(acqo)) armortype = 5;
					else if (is_shirt(acqo)) armortype = 6;
					else armortype = 7;
rerollX:
					acqo->otyp = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
					if (armortype == 1 && !is_shield(acqo)) goto rerollX;
					if (armortype == 2 && !is_helmet(acqo)) goto rerollX;
					if (armortype == 3 && !is_boots(acqo)) goto rerollX;
					if (armortype == 4 && !is_gloves(acqo)) goto rerollX;
					if (armortype == 5 && !is_cloak(acqo)) goto rerollX;
					if (armortype == 6 && !is_shirt(acqo)) goto rerollX;
					if (armortype == 7 && !is_suit(acqo)) goto rerollX;

				}
				break;
			case RING_CLASS:
				acqo->otyp = rnd_class(RIN_ADORNMENT,RIN_TELEPORT_CONTROL);
				break;
			case AMULET_CLASS:
				acqo->otyp = rnd_class(AMULET_OF_CHANGE,AMULET_OF_VULNERABILITY);
				break;
			case IMPLANT_CLASS:
				acqo->otyp = rnd_class(IMPLANT_OF_ABSORPTION,IMPLANT_OF_ENFORCING);
				break;
		}

		    dropy(acqo);

			if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_ISRESTRICTED) {
			    unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
			} else if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_UNSKILLED) {
				unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_BASIC;
			} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_BASIC) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SKILLED;
			} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_SKILLED) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_EXPERT;
			} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_EXPERT) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_MASTER;
			} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_MASTER) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_GRAND_MASTER;
			} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_GRAND_MASTER) {
				P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SUPREME_MASTER;
			}
			if (Race_if(PM_RUSMOT)) {
				if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_ISRESTRICTED) {
				    unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
				} else if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_UNSKILLED) {
					unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_BASIC;
				} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_BASIC) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SKILLED;
				} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_SKILLED) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_EXPERT;
				} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_EXPERT) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_MASTER;
				} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_MASTER) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_GRAND_MASTER;
				} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_GRAND_MASTER) {
					P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SUPREME_MASTER;
				}
			}

		    discover_artifact(acqo->oartifact);

			/* this will enable unaligned artifacts, which has to be done by incrementing the
			 * u.ugifts var; since we don't actually want this scroll to mess with your chances of getting divine
			 * sacrifice gifts, it sets the variable to zero again afterwards --Amy */
			if (!havegifts) u.ugifts--;
			pline("An artifact appeared beneath you!");
		}
		else pline("Opportunity knocked, but nobody was home.  Bummer.");

		break;

	case SCR_SYMBIOSIS:
		known = TRUE;

		if (confused) {

			if (!uactivesymbiosis) {
				pline(FunnyHallu ? "Something nonexistant would have been affected but couldn't." : "The scroll of symbiosis fails to do anything.");
				break;
			}

			if (sobj->cursed) {
				u.shutdowntime += 1000;
				Your("symbiote was shut down!");
				if (flags.showsymbiotehp) flags.botl = TRUE;
			} else {
				if (sobj->blessed) {
					u.usymbiote.mhpmax += rnd(8);
					if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
				}
				u.usymbiote.mhp = u.usymbiote.mhpmax;
				if (flags.showsymbiotehp) flags.botl = TRUE;
				Your("symbiote was healed!");
			}

		} else {
			if (sobj->cursed && uinsymbiosis) {
				u.shutdowntime = 0;
				u.usymbiote.active = 0;
				u.usymbiote.mnum = PM_PLAYERMON;
				u.usymbiote.mhp = 0;
				u.usymbiote.mhpmax = 0;
				u.usymbiote.cursed = u.usymbiote.hvycurse = u.usymbiote.prmcurse = u.usymbiote.bbcurse = u.usymbiote.morgcurse = u.usymbiote.evilcurse = u.usymbiote.stckcurse = 0;
				if (flags.showsymbiotehp) flags.botl = TRUE;
				u.cnd_symbiotesdied++;
			}

			if (uinsymbiosis) {
				getlin("You have found a scroll of symbiosis! Do you want to replace your current symbiote with a random new one? [yes/no]", buf);
				(void) lcase (buf);
				if (!(strcmp (buf, "yes")) ) {
					getrandomsymbiote(sobj->blessed);
					break;
				} else {
					You("decided to not get a random new symbiote.");
					break;
				}
			}

			getrandomsymbiote(sobj->blessed);
		}

		break;

	case SCR_BOSS_COMPANION:
		known = TRUE;

		reset_rndmonst(NON_PM);

		{
			int attempts = 0;
			register struct permonst *ptrZ;
			register struct monst *bossmon;
newbossC:
			do {

				ptrZ = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

			if (ptrZ && ptrZ->geno & G_UNIQ) {
				if (wizard) pline("monster generation: %s", ptrZ->mname);
				bossmon = makemon(ptrZ, u.ux, u.uy, NO_MM_FLAGS);
			}
			else if (rn2(50)) {
				attempts = 0;
				goto newbossC;
			}

			if (bossmon) {
				tamedog(bossmon, (struct obj *) 0, TRUE);
			}

		}

		break;

	case SCR_WISHING:
		known = TRUE;
		pline("You have found a scroll of wishing!");
		if ((sobj->cursed || (!sobj->blessed && Luck+rn2(5) < 0)) && !RngeWishImprovement && !(uarmc && itemhasappearance(uarmc, APP_WISHFUL_CLOAK)) ) {
			makenonworkingwish();
			break;
		}
		makewish(TRUE);
		break;

	case SCR_RAGNAROK:
		known = TRUE;
		ragnarok(TRUE);
		if (evilfriday) evilragnarok(TRUE,level_difficulty());

		break;

	case SCR_OFFLEVEL_ITEM:
		known = TRUE;
		{
			register struct monst *offmon;
newoffmon:
			if ((offmon = makemon((struct permonst *)0, 0, 0, NO_MM_FLAGS)) != 0) {

				register int inventcount = inv_cnt();

				if (inventcount > 0) {
					inventcount /= 8;
					if (inventcount < 1) inventcount = 1;

					while (inv_cnt() && inventcount) {
						char bufof[BUFSZ];
						bufof[0] = '\0';
						steal(offmon, bufof, TRUE, TRUE);
						inventcount--;
					}

				}

				mdrop_special_objs(offmon); /* make sure it doesn't tele to an unreachable place with the book of the dead or something */
				if (u.uevent.udemigod && !u.freeplaymode) break;
				else u_teleport_monB(offmon, FALSE);
				pline("Some of your possessions have been stolen!");

			} else {
				if (rn2(1000)) goto newoffmon;
				else pline("Somehow you feel that you just averted a major crisis.");
			}

		}
		break;

	case SCR_MATERIAL_CHANGE:

		randommaterials();
		pline("Item materials have been shuffled.");

		break;

	case SCR_ASTRALCENSION:
		known = TRUE;
		pline("An aura of extreme radiance floods the dungeon...");
		if (Blind) {
			pline("It's so strong that you can sense it even though you're blind!");
			make_blinded((long)u.ucreamed,TRUE);
		}
		create_mplayers(1, TRUE);
		break;

	case SCR_MISSING_CODE:

		if (uarmu) {
			if (uarmu->spe < 20) {
				uarmu->spe++;
				Your("shirt glows for a moment.");
			} else pline("The code malfunctioned.");
		} else {
			struct obj *uroub;

			uroub = mksobj(T_SHIRT, TRUE, FALSE, FALSE);

			if (uroub) {
				dropy(uroub);
				stackobj(uroub);
				pline("A shirt appeared at your %s!", makeplural(body_part(FOOT)));
			} else pline("The build has errored.");

		}

		u.ugangr++;
		You_hear("thunder in the distance...");

		break;

	case SCR_ARTIFACT_CREATION:
		known = TRUE;
		giftartifact();

		break;

	case SCR_ACQUIREMENT: 
		known = TRUE;

		int acquireditem;
		acquireditem = 0;
		pline("You have found a scroll of acquirement!");
		if ((sobj->cursed || (!sobj->blessed && Luck+rn2(5) < 0)) && !RngeWishImprovement && !(uarmc && itemhasappearance(uarmc, APP_WISHFUL_CLOAK)) ) {
			pline("Unfortunately, nothing happens.");
			break;
		}

		acquireitem();

		break;

	case SCR_ENTHRONIZATION:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build a throne.");
		levl[u.ux][u.uy].typ = THRONE;

		break;

	case SCR_MAKE_PENTAGRAM:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("Suddenly a pentagram is inscribed on the floor!");
		if (FunnyHallu) pline("USEITUSEITUSEITUSEITUSEIT!"); /* thanks Antichthon :D */
		levl[u.ux][u.uy].typ = PENTAGRAM;

		break;

	case SCR_WELL_BUILDING:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build a well.");
		levl[u.ux][u.uy].typ = WELL;

		break;

	case SCR_DRIVING:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build a wagon.");
		levl[u.ux][u.uy].typ = WAGON;

		break;

	case SCR_TABLE_FURNITURE:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build a wooden table.");
		levl[u.ux][u.uy].typ = WOODENTABLE;

		break;

	case SCR_EMBEDDING:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build an ornately carved bed.");
		levl[u.ux][u.uy].typ = CARVEDBED;

		break;

	case SCR_MATTRESS_SLEEPING:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build a rude straw mattress.");
		levl[u.ux][u.uy].typ = STRAWMATTRESS;

		break;

	case SCR_FOUNTAIN_BUILDING:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build a fountain.");
		levl[u.ux][u.uy].typ = FOUNTAIN;

		break;

	case SCR_SINKING:
	case SCR_CREATE_SINK:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build a sink.");
		levl[u.ux][u.uy].typ = SINK;

		break;

	case SCR_WC:

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build a toilet.");
		levl[u.ux][u.uy].typ = TOILET;

		break;

	case SCR_CONSECRATION:
	{
		aligntyp al,ual;
		char c;
		char a;
		int x,y;

		if (Is_astralevel(&u.uz)) {
			You("sense the wrath of the gods.");
			u.ualign.record--;
			break;
		}
		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build an altar.");
		if (is_demon(youmonst.data)) {
			al = A_CHAOTIC;
		} else if (sobj->cursed) {
			al = A_NONE;
		} else if (sobj->blessed) {
			ual = u.ualign.type;
			a = (ual==A_LAWFUL) ? 'l' : ((ual==A_NEUTRAL) ? 'n' : 'c');
			c = yn_function("Which alignment do you want to consecrate the altar to?","lncm",a);
			al = A_NONE;
			switch (c) {
				case 'l':
					al = A_LAWFUL;		break;
				case 'n':
					al = A_NEUTRAL;		break;
				case 'c':
					al = A_CHAOTIC;		break;
				case 'm':
					al = A_NONE;		break;
			}
		} else {
			al = u.ualign.type;
		}
		pline("Pronouncing arcane formulas, you consecrate the altar to %s.",(al == A_NONE) ? (Role_if(PM_GANG_SCHOLAR) ? "Anna" : Role_if(PM_WALSCHOLAR) ? "Anna" : "Moloch") : align_gname(al));
		levl[u.ux][u.uy].typ = ALTAR;
		levl[u.ux][u.uy].altarmask = Align2amask(al);
		x = (al == ual) ? 1 : ((al == A_NONE) ? -3 : -1);

		if (x < 0) y = rnd(abs(x)*100);
		else y = -(rnd(100));
		u.ualign.record += x;
		u.ublesscnt += y;
		if (u.ublesscnt < 0) u.ublesscnt = 0;
		You_feel("%s%scomfortable.",(abs(x)>1) ? "very " : "", (x<0) ? "un" : "");
	} break;

	case SCR_CREATE_ALTAR:

		if (Is_astralevel(&u.uz)) {
			You("sense the wrath of the gods.");
			u.ualign.record--;
			break;
		}
		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
			break;
		}
		known = TRUE;
		pline("You build an altar.");
		levl[u.ux][u.uy].typ = ALTAR;
		levl[u.ux][u.uy].altarmask = Align2amask(A_NONE);
		u.ualign.record -= 50;
		u.ublesscnt += rnz(300);
		You_feel("uncomfortable.");

		break;

	case SCR_CREATE_CREATE_SCROLL:

		{
			struct obj *createdscroll;
			pline("Now that's weird.");
		    	known = FALSE;
		    	createdscroll = mksobj_at(SCR_CREATE_CREATE_SCROLL, u.ux, u.uy, FALSE, FALSE, FALSE);
			if (createdscroll) {
			    	createdscroll->blessed = sobj->blessed;
			    	createdscroll->cursed = sobj->cursed;
			    	createdscroll->hvycurse = sobj->hvycurse;
			    	createdscroll->prmcurse = sobj->prmcurse;
			    	createdscroll->morgcurse = sobj->morgcurse;
			    	createdscroll->evilcurse = sobj->evilcurse;
			    	createdscroll->bbrcurse = sobj->bbrcurse;
			    	createdscroll->stckcurse = sobj->stckcurse;
			}
		}

		break;

	case SCR_SECURE_CURSE_REMOVAL: /* by bug_sniper */

		if (confused) {
			You_feel("as if you need some help.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
			rndcurse();
			break;
		}

		{
			register struct obj *secrem;
			pline("You found a scroll of secure curse removal.");
			known = TRUE;
secremchoice:
			secrem = getobj(all_count, "uncurse");
			if (!secrem) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to remove curses from an item.");
				else goto secremchoice;
				pline("Oh well, if you don't wanna...");
			} else {
				if (stack_too_big(secrem)) {
					pline("The stack was too big! Nothing happens.");
				} else {
					if (sobj->cursed) {
						curse(secrem);
						curse(secrem);
						curse(secrem);
						pline("Your %s is surrounded by an ice-cold aura.", xname(secrem));
						secrem->bknown = TRUE;
					} else {
						secrem->cursed = secrem->hvycurse = secrem->prmcurse = secrem->morgcurse = secrem->evilcurse = secrem->bbrcurse = secrem->stckcurse = 0;
						pline("Your %s is surrounded by a warm aura.", xname(secrem));
						secrem->bknown = TRUE;
					}
				}
			}
		}

		break;

	case SCR_ANTIMAGIC:

		if (confused) {
			u.uprops[DEAC_ANTIMAGIC].intrinsic += rnz(20000);
			pline("You are prevented from having magic resistance!");
		}

		if (sobj->cursed) {
			u.uen = 0;
			drain_en(rnd(u.ulevel) + 1 + rnd(monster_difficulty() + 1));
		}

		if (!confused && !sobj->cursed) {
			pline("You produce an anti-magic shell.");
			known = TRUE;
			u.antimagicshell += rn1( 500 + (500 * bcsign(sobj)), 500);
		}

		break;

	case SCR_RESISTANCE:

		if (confused) {
			attrcurse(); attrcurse(); attrcurse(); attrcurse(); attrcurse();
		}

		if (sobj->cursed) {
			u.uprops[DEAC_SHOCK_RES].intrinsic += rnz(200);
			pline("You are prevented from having shock resistance!");
			u.uprops[DEAC_FIRE_RES].intrinsic += rnz(200);
			pline("You are prevented from having fire resistance!");
			u.uprops[DEAC_COLD_RES].intrinsic += rnz(200);
			pline("You are prevented from having cold resistance!");
			u.uprops[DEAC_SLEEP_RES].intrinsic += rnz(200);
			pline("You are prevented from having sleep resistance!");
			u.uprops[DEAC_ACID_RES].intrinsic += rnz(200);
			pline("You are prevented from having acid resistance!");
		}

		if (!confused && !sobj->cursed) {
			You_feel("resistant to elemental attacks!");
			known = TRUE;
			   HFire_resistance += rn1(100,50 + 25 * bcsign(sobj) );
			   HCold_resistance += rn1(100,50 + 25 * bcsign(sobj) );
			   HShock_resistance += rn1(100,50 + 25 * bcsign(sobj) );
			   HSleep_resistance += rn1(100,50 + 25 * bcsign(sobj) );
			   HAcid_resistance += rn1(100,50 + 25 * bcsign(sobj) );
		}

		break;

	case SCR_SYMMETRY:

		pline("Fearful Symmetry!");
		if (u.totter && !sobj->cursed && !confused) u.totter = 0;
		else u.totter = 1;

		break;

	case SCR_ROOT_PASSWORD_DETECTION:

		if (sobj->oartifact == ART_ERASE_ALL_DATA) {
			u.datadeletedefer = 1;
			datadeleteattack();
			pline("Congratulations, now all your data has been erased. Well done!");
			break;
		}

		You("sense the computer's root password.");
		pline("It is: %s.", sobj->cursed ? "jsdfljfdsh" : (sobj->blessed ? "memalezu" : "xyzzy"));
		pline("(No warranty implied.)");
		break;

	case SCR_STINKING_CLOUD: {
	        coord cc;

		You("have found a scroll of stinking cloud!");
		known = TRUE;
		pline("Where do you want to center the cloud?");
		cc.x = u.ux;
		cc.y = u.uy;
		if (getpos(&cc, TRUE, "the desired position") < 0) {
		    pline("%s", Never_mind);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually I do mind...");
				badeffect();
			}
		    return 0;
		}
		if (!cansee(cc.x, cc.y) || distu(cc.x, cc.y) >= 32) {
		    You("smell rotten eggs.");
		    return 0;
		}
		(void) create_gas_cloud(cc.x, cc.y, 3+bcsign(sobj),
						8+4*bcsign(sobj));
		break;
	}
	default:
		impossible("What weird effect is this? (%ld)", sobj->otyp);
	}
	return(0);
}

#if 0
static void
wand_explode(obj)
register struct obj *obj;
{
    obj->in_use = TRUE;	/* in case losehp() is fatal */
    Your("%s vibrates violently, and explodes!",xname(obj));
    nhbell();
    losehp(rnd(2*(u.uhpmax+1)/3), "exploding wand", KILLED_BY_AN);
    useup(obj);
    exercise(A_STR, FALSE);
}
#endif

/*
 * Low-level lit-field update routine.
 */
STATIC_PTR void
set_lit(x,y,val)
int x, y;
void * val;
{
	if (val)
	    levl[x][y].lit = 1;
	else {
	    levl[x][y].lit = 0;
	    snuff_light_source(x, y);
	}
}

void
litroom(on,obj)
register boolean on;
struct obj *obj;
{
	char is_lit;	/* value is irrelevant; we use its address
			   as a `not null' flag for set_lit() */

	/* first produce the text (provided you're not blind) */
	if(!on) {
		register struct obj *otmp;

		if (!Blind) {
		    if(u.uswallow) {
			pline("It seems even darker in here than before.");
			return;
		    }
		    if (uwep && artifact_light(uwep) && uwep->lamplit)
			pline("Suddenly, the only light left comes from %s!",
				the(xname(uwep)));
		    else
			You("are surrounded by darkness!");
		}

		/* the magic douses lamps, et al, too */
		for(otmp = invent; otmp; otmp = otmp->nobj)
		    if (otmp->lamplit)
			(void) snuff_lit(otmp);
		if (Blind) goto do_it;
	} else {
		if (Blind) goto do_it;
		if(u.uswallow){
			if (is_animal(u.ustuck->data))
				pline("%s %s is lit.",
				        s_suffix(Monnam(u.ustuck)),
					mbodypart(u.ustuck, STOMACH));
			else
				if (is_whirly(u.ustuck->data))
					pline("%s shines briefly.",
					      Monnam(u.ustuck));
				else
					pline("%s glistens.", Monnam(u.ustuck));
			return;
		}
		pline("A lit field surrounds you!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vy vse yeshche ne mozhete videt' dostatochno, potomu chto Sovetskiy sdelal eto tak, chto dostatochno sredniye monstry skryvayutsya tol'ko za predelami etogo radiusa." : "Wschiiiiie!");
	}

do_it:
	/* No-op in water - can only see the adjacent squares and that's it! */
	if (Underwater || Is_waterlevel(&u.uz)) return;
	/*
	 *  If we are darkening the room and the hero is punished but not
	 *  blind, then we have to pick up and replace the ball and chain so
	 *  that we don't remember them if they are out of sight.
	 */
	if (Punished && !on && !Blind)
	    move_bc(1, 0, uball->ox, uball->oy, uchain->ox, uchain->oy);

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) {
	    /* Can't use do_clear_area because MAX_RADIUS is too small */
	    /* rogue lighting must light the entire room */
	    int rnum = levl[u.ux][u.uy].roomno - ROOMOFFSET;
	    int rx, ry;
	    if(rnum >= 0) {
		for(rx = rooms[rnum].lx-1; rx <= rooms[rnum].hx+1; rx++)
		    for(ry = rooms[rnum].ly-1; ry <= rooms[rnum].hy+1; ry++)
			set_lit(rx, ry,
				(void *)(on ? &is_lit : (char *)0));
		rooms[rnum].rlit = on;
	    }
	    /* hallways remain dark on the rogue level */
	} else
#endif

		if (rn2(10) || (obj && obj->oclass==SPBOOK_CLASS) ) {

	    do_clear_area(u.ux,u.uy,
		obj->oartifact ? 12 :
		(obj && obj->oclass==SCROLL_CLASS && obj->blessed) ? 9 : (obj && obj->oclass==SPBOOK_CLASS) ? 3 : 5,
		set_lit, (void *)(on ? &is_lit : (char *)0));

		}
		else {

	    do_clear_areaX(u.ux,u.uy,
		obj->oartifact ? 12 :
		(obj && obj->oclass==SCROLL_CLASS && obj->blessed) ? 9 : 5,
		set_lit, (void *)(on ? &is_lit : (char *)0));

		}

	/*
	 *  If we are not blind, then force a redraw on all positions in sight
	 *  by temporarily blinding the hero.  The vision recalculation will
	 *  correctly update all previously seen positions *and* correctly
	 *  set the waslit bit [could be messed up from above].
	 */
	if (!Blind) {
	    vision_recalc(2);

	    /* replace ball&chain */
	    if (Punished && !on)
		move_bc(0, 0, uball->ox, uball->oy, uchain->ox, uchain->oy);
	}

	/* angel's increased sight doesn't work well in magical darkness... (bullshit downside :P) --Amy */
	if (!on && Race_if(PM_HUMANOID_ANGEL)) WeakSight += rnz(10 * (monster_difficulty() + 1));

	vision_full_recalc = 1;	/* delayed vision recalculation */
}

void
litroomlite(on)
register boolean on;
{
	char is_lit;	/* value is irrelevant; we use its address
			   as a `not null' flag for set_lit() */

	if (!on && shadowprotection()) return;

	if (rn2(10)) {
		do_clear_area(u.ux,u.uy, 7, set_lit, (void *)(on ? &is_lit : (char *)0));
	} else {
		do_clear_areaX(u.ux,u.uy, 7, set_lit, (void *)(on ? &is_lit : (char *)0));
	}

	/* angel's increased sight doesn't work well in magical darkness... (bullshit downside :P) --Amy */
	if (!on && Race_if(PM_HUMANOID_ANGEL)) WeakSight += rnz(10 * (monster_difficulty() + 1));

	/*
	 *  If we are not blind, then force a redraw on all positions in sight
	 *  by temporarily blinding the hero.  The vision recalculation will
	 *  correctly update all previously seen positions *and* correctly
	 *  set the waslit bit [could be messed up from above].
	 */
	if (!Blind) {
	    vision_recalc(2);

	}

	vision_full_recalc = 1;	/* delayed vision recalculation */
}

static void
do_class_genocide()
{
/*WAC adding windowstuff*/
        winid tmpwin;
	anything any;
        int n;
	menu_item *selected;

	int i, j, immunecnt, gonecnt, goodcnt, class, feel_dead = 0;
	char buf[BUFSZ];
	boolean gameover = FALSE;	/* true iff killed self */

	for(j=0; ; j++) {
		if (j >= 5) {
			pline("%s", thats_enough_tries);
			return;
		}
		do {
                    getlin("What class of monsters do you wish to genocide? [? for help]",
			buf);
		    (void)mungspaces(buf);
		} while (buf[0]=='\033' || !buf[0]);
		/* choosing "none" preserves genocideless conduct */
		if (!strcmpi(buf, "none") ||
		    !strcmpi(buf, "nothing")) return;

		if (strlen(buf) == 1) {
		    /*WAC adding "help" for those who use graphical displays*/
                    if (buf[0]=='?'){
                        tmpwin = create_nhwindow(NHW_MENU);
                        start_menu(tmpwin);           
                        any.a_void = 0;         /* zero out all bits */
                        for (i = 1; i < (MAXMCLASSES - MISCMCLASSES); i++) {
                           any.a_int = i;        /* must be non-zero */
			   if (monexplain[i])
			       add_menu(tmpwin, NO_GLYPH, &any,
				   def_monsyms[i], 0, ATR_NONE,
				   an(monexplain[i]), MENU_UNSELECTED);
                        }
                        end_menu(tmpwin, "Monster Types");
                        n = 0;
                        while (n == 0) n = select_menu(tmpwin, PICK_ONE,
                                &selected);
                        destroy_nhwindow(tmpwin);
                        if (n== -1) continue;  /*user hit escape*/
                        class = selected[0].item.a_int;
                    } else {
		    if (buf[0] == ILLOBJ_SYM)
			buf[0] = def_monsyms[S_MIMIC];
		    class = def_char_to_monclass(buf[0]);
                    }
		} else {
		    char buf2[BUFSZ];

		    class = 0;
		    strcpy(buf2, makesingular(buf));
		    strcpy(buf, buf2);
		}
		immunecnt = gonecnt = goodcnt = 0;
		for (i = LOW_PM; i < NUMMONS; i++) {
		    if (class == 0 &&
			    strstri(monexplain[(int)mons[i].mlet], buf) != 0)
			class = mons[i].mlet;

		    if (mons[i].mlet == class) {
			if (!(mons[i].geno & G_GENO)) immunecnt++;
			else if (mons[i].mlet == u.ungenocidable) immunecnt++;
			else if(mvitals[i].mvflags & G_GENOD) gonecnt++;
			else goodcnt++;
		    }
		}
		/*
		 * TODO[?]: If user's input doesn't match any class
		 *	    description, check individual species names.
		 */
		if (!goodcnt && class != mons[urole.malenum].mlet &&
				class != mons[urace.malenum].mlet) {
			if (gonecnt)
	pline("All such monsters are already nonexistent.");
			else if (immunecnt ||
				(buf[0] == DEF_INVISIBLE && buf[1] == '\0'))
	You("aren't permitted to genocide such monsters.");
			else
#ifdef WIZARD	/* to aid in topology testing; remove pesky monsters */
			  if (wizard && buf[0] == '*') {
			    register struct monst *mtmp, *mtmp2;

			    gonecnt = 0;
			    for (mtmp = fmon; mtmp; mtmp = mtmp2) {
				mtmp2 = mtmp->nmon;
			    	if (DEADMONSTER(mtmp)) continue;
				mongone(mtmp);
				gonecnt++;
			    }
	pline("Eliminated %d monster%s.", gonecnt, plur(gonecnt));
			    return;
			} else
#endif
	pline("That symbol does not represent any monster.");
			continue;
		}

		for (i = LOW_PM; i < NUMMONS; i++) {
		    if(mons[i].mlet == class) {
			char nam[BUFSZ];

			strcpy(nam, makeplural(mons[i].mname));
			/* Although "genus" is Latin for race, the hero benefits
			 * from both race and role; thus genocide affects either.
			 */
			if (Your_Own_Role(i) || Your_Own_Race(i) ||
				((mons[i].geno & G_GENO)
				&& !(mvitals[i].mvflags & G_GENOD))) {
			/* This check must be first since player monsters might
			 * have G_GENOD or !G_GENO.
			 */

 			    /* Now give the monsters a resistance check, but only against
 			       the effects of a blessed scroll... */
 
 			    int res; /* lethe patch */
 
 			    res = 35 - u.ulevel + mons[i].mr;
				if (res >= 90) res = 90; 
				if (res <= 15) res = 15; 

 			    pline("Attempting to wipe out all %s.", nam);
 
				if (rnd(100) <= res) pline("The %s resisted genocide.", nam);
 			    else {

			    mvitals[i].mvflags |= (G_GENOD|G_NOCORPSE);
			    reset_rndmonst(i);
			    kill_genocided_monsters();
			    update_inventory();		/* eggs & tins */
			    pline("Wiped out all %s.", nam);
			    if (Upolyd && i == u.umonnum) {
				u.mh = -1;
				if (Unchanging) {
				    if (!feel_dead++) You("die.");
				    /* finish genociding this class of
				       monsters before ultimately dying */
				    gameover = TRUE;
				} else
				    rehumanize();
			    }
			    /* Self-genocide if it matches either your race
			       or role.  Assumption:  male and female forms
			       share same monster class. */
			    if (i == urole.malenum || i == urace.malenum) {
				u.uhp = -1;
				if (Upolyd) {
				    if (!feel_dead++) You_feel("dead inside.");
				} else {
				    if (!feel_dead++) You("die.");
				    gameover = TRUE;
				  }
				}
			    }
			} else if (mvitals[i].mvflags & G_GENOD) {
			    if (!gameover)
				pline("All %s are already nonexistent.", nam);
			} else if (!gameover) {
			  /* suppress feedback about quest beings except
			     for those applicable to our own role */
			  if ((mons[i].msound != MS_LEADER ||
			       quest_info(MS_LEADER) == i)
			   && (mons[i].msound != MS_NEMESIS ||
			       quest_info(MS_NEMESIS) == i)
			   && (mons[i].msound != MS_GUARDIAN ||
			       quest_info(MS_GUARDIAN) == i)
			/* non-leader/nemesis/guardian role-specific monster */
			   && (i != PM_NINJA_GAIDEN ||		/* nuisance */
			       Role_if(PM_SAMURAI))) {
				boolean named, uniq;

				named = type_is_pname(&mons[i]) ? TRUE : FALSE;
				uniq = (mons[i].geno & G_UNIQ) ? TRUE : FALSE;
				/* one special case */
				if (i == PM_HIGH_PRIEST) uniq = FALSE;

				You("aren't permitted to genocide %s%s.",
				    (uniq && !named) ? "the " : "",
				    (uniq || named) ? mons[i].mname : nam);
			    }
			}
		    }
		}
		if (gameover || u.uhp == -1) {
		    killer_format = KILLED_BY_AN;
		    killer = "scroll of genocide";
		    if (gameover) done(GENOCIDED);
		}
		return;
	}
}

static void
do_total_genocide()
{
	int i, j;
	boolean gameover = FALSE;	/* true iff killed self */

	for (i = LOW_PM; i < NUMMONS; i++) {

		if ( !(Your_Own_Role(i) || Your_Own_Race(i)) &&
			((mons[i].geno & G_GENO) && !(mvitals[i].mvflags & G_GENOD)) ) {

		    mvitals[i].mvflags |= (G_GENOD|G_NOCORPSE);
		    reset_rndmonst(i);
		    kill_genocided_monsters();
		    update_inventory();		/* eggs & tins */
		    if (Upolyd && i == u.umonnum) {
			u.mh = -1;
			if (Unchanging) {
			    You("die.");
			    gameover = TRUE;
			} else
		    	    if (!Race_if(PM_UNGENOMOLD)) rehumanize();
			    else polyself(FALSE);
		    }
		}
	}
	if (gameover || u.uhp == -1) {
	    killer_format = KILLED_BY_AN;
	    killer = "scroll of total genocide";
	    if (gameover) done(GENOCIDED);
	}
	return;
}

static void
do_class_erasure()
{
/*WAC adding windowstuff*/
        winid tmpwin;
	anything any;
        int n;
	menu_item *selected;

	int i, j, immunecnt, gonecnt, goodcnt, class, feel_dead = 0;
	char buf[BUFSZ];
	boolean gameover = FALSE;	/* true iff killed self */

	for(j=0; ; j++) {
		if (j >= 5) {
			pline("%s", thats_enough_tries);
			return;
		}
		do {
                    getlin("What class of monsters do you wish to erase? [? for help]",
			buf);
		    (void)mungspaces(buf);
		} while (buf[0]=='\033' || !buf[0]);
		/* choosing "none" preserves genocideless conduct */
		if (!strcmpi(buf, "none") ||
		    !strcmpi(buf, "nothing")) return;

		if (strlen(buf) == 1) {
		    /*WAC adding "help" for those who use graphical displays*/
                    if (buf[0]=='?'){
                        tmpwin = create_nhwindow(NHW_MENU);
                        start_menu(tmpwin);           
                        any.a_void = 0;         /* zero out all bits */
                        for (i = 1; i < (MAXMCLASSES - MISCMCLASSES); i++) {
                           any.a_int = i;        /* must be non-zero */
			   if (monexplain[i])
			       add_menu(tmpwin, NO_GLYPH, &any,
				   def_monsyms[i], 0, ATR_NONE,
				   an(monexplain[i]), MENU_UNSELECTED);
                        }
                        end_menu(tmpwin, "Monster Types");
                        n = 0;
                        while (n == 0) n = select_menu(tmpwin, PICK_ONE,
                                &selected);
                        destroy_nhwindow(tmpwin);
                        if (n== -1) continue;  /*user hit escape*/
                        class = selected[0].item.a_int;
                    } else {
		    if (buf[0] == ILLOBJ_SYM)
			buf[0] = def_monsyms[S_MIMIC];
		    class = def_char_to_monclass(buf[0]);
                    }
		} else {
		    char buf2[BUFSZ];

		    class = 0;
		    strcpy(buf2, makesingular(buf));
		    strcpy(buf, buf2);
		}
		immunecnt = gonecnt = goodcnt = 0;
		for (i = LOW_PM; i < NUMMONS; i++) {
		    if (class == 0 &&
			    strstri(monexplain[(int)mons[i].mlet], buf) != 0)
			class = mons[i].mlet;

		    if (mons[i].mlet == class) {
			if (!(mons[i].geno & G_GENO)) immunecnt++;
			else if (mons[i].mlet == u.ungenocidable) immunecnt++;
			else if(mvitals[i].mvflags & G_GENOD) gonecnt++;
			else goodcnt++;
		    }
		}
		/*
		 * TODO[?]: If user's input doesn't match any class
		 *	    description, check individual species names.
		 */
		if (!goodcnt && class != mons[urole.malenum].mlet &&
				class != mons[urace.malenum].mlet) {
			if (gonecnt)
	pline("All such monsters are already nonexistent.");
			else if (immunecnt ||
				(buf[0] == DEF_INVISIBLE && buf[1] == '\0'))
	You("aren't permitted to genocide such monsters.");
			else
#ifdef WIZARD	/* to aid in topology testing; remove pesky monsters */
			  if (wizard && buf[0] == '*') {
			    register struct monst *mtmp, *mtmp2;

			    gonecnt = 0;
			    for (mtmp = fmon; mtmp; mtmp = mtmp2) {
				mtmp2 = mtmp->nmon;
			    	if (DEADMONSTER(mtmp)) continue;
				mongone(mtmp);
				gonecnt++;
			    }
	pline("Eliminated %d monster%s.", gonecnt, plur(gonecnt));
			    return;
			} else
#endif
	pline("That symbol does not represent any monster.");
			continue;
		}

		pline("Attempting to wipe out monsters.");

	      gonecnt = 0;

		for (i = LOW_PM; i < NUMMONS; i++) {
		    if(mons[i].mlet == class) {
			char nam[BUFSZ];

			strcpy(nam, makeplural(mons[i].mname));
			/* Although "genus" is Latin for race, the hero benefits
			 * from both race and role; thus genocide affects either.
			 */
			if (Your_Own_Role(i) || Your_Own_Race(i) ||
				((mons[i].geno & G_GENO)
				&& !(mvitals[i].mvflags & G_GENOD))) {
			/* This check must be first since player monsters might
			 * have G_GENOD or !G_GENO.
			 */
			    register struct monst *mtmp, *mtmp2;

		  	    for(mtmp = fmon; mtmp; mtmp = mtmp2) {
		  		mtmp2 = mtmp->nmon;

		  		if(mtmp->mnum == i) {
					mondead(mtmp);
					gonecnt++;
				}

			    }
			}
		    }
		}
		pline("Eliminated %d monster%s.", gonecnt, plur(gonecnt));

		if (gameover || u.uhp == -1) {
		    killer_format = KILLED_BY_AN;
		    killer = "scroll of genocide";
		    if (gameover) done(GENOCIDED);
		}
		return;
	}
}

#define REALLY 1
#define PLAYER 2
#define ONTHRONE 4
#define MASS_MURDER 8
void
do_genocide(how)
int how;
/* 0 = no genocide; create monsters (cursed scroll) */
/* 1 = normal genocide */
/* 3 = forced genocide of player */
/* 5 (4 | 1) = normal genocide from throne */
/* x|8 = mass murder; no genocide */
{
	char buf[BUFSZ],bx[BUFSZ];
	register int	i, killplayer = 0;
	register int mndx;
	register struct permonst *ptr;
	const char *which;
	struct monst *mtmp, *mtmp2;

	if (how & PLAYER) {
		mndx = u.umonster;	/* non-polymorphed mon num */
		ptr = &mons[mndx];
		strcpy(buf, ptr->mname);
		killplayer++;
	} else {
	    for(i = 0; ; i++) {
		if(i >= 5) {
		    pline("%s", thats_enough_tries);
		    return;
		}
		sprintf(bx,"What monster do you want to %s? [type the name]",(how & MASS_MURDER) ? "mass murder" : "genocide");
		getlin(bx,buf);
		(void)mungspaces(buf);
		/* choosing "none" preserves genocideless conduct */
		if (!strcmpi(buf, "none") || !strcmpi(buf, "nothing")) {
		    /* ... but no free pass if cursed */
		    if (!(how & REALLY)) {
			ptr = rndmonst();
			if (!ptr) return; /* no message, like normal case */
			mndx = monsndx(ptr);
			break;		/* remaining checks don't apply */
		    } else return;
		}

		mndx = name_to_mon(buf);
		if (mndx == NON_PM || (mvitals[mndx].mvflags & G_GENOD)) {
			pline("Such creatures %s exist in this world.",
			      (mndx == NON_PM) ? "do not" : "no longer");
			continue;
		}
		ptr = &mons[mndx];

		/* jonadab's idea is to have a monster glyph that cannot be genocided, which is rolled on game start. */
		if (ptr->mlet == u.ungenocidable) {
			pline("Tough luck! The RNG decided to make that monster ungenocidable.");
			continue;
		}

		/* Although "genus" is Latin for race, the hero benefits
		 * from both race and role; thus genocide affects either.
		 */
		if (Your_Own_Role(mndx) || Your_Own_Race(mndx)) {
			killplayer++;
			break;
		}
		if (is_human(ptr)) adjalign(-sgn(u.ualign.type));
		if (is_demon(ptr)) adjalign(sgn(u.ualign.type));

		if(!(ptr->geno & G_GENO)) {
			if(flags.soundok) {
	/* fixme: unconditional "caverns" will be silly in some circumstances */
			    if(flags.verbose)
			pline("A thunderous voice booms through the caverns:");
			    verbalize("No, mortal!  That will not be done.");
			}
			continue;
		}
		/* KMH -- Unchanging prevents rehumanization */
		if (Unchanging && ptr == youmonst.data)
		    killplayer++;
		break;
	    }
	}

	which = "all ";
	if (Hallucination) {
	    if (Upolyd && !missingnoprotect)
		strcpy(buf,youmonst.data->mname);
	    else {
		strcpy(buf, (flags.female && urole.name.f) ?
				urole.name.f : urole.name.m);
		buf[0] = lowc(buf[0]);
	    }
	} else {
	    strcpy(buf, ptr->mname); /* make sure we have standard singular */
	    if ((ptr->geno & G_UNIQ) && ptr != &mons[PM_HIGH_PRIEST])
		which = !type_is_pname(ptr) ? "the " : "";
	}
	/*if (how & MASS_MURDER) {	    kill_genocided_monsters();
	    reset_rndmonst(mndx);
	    update_inventory();
	}*/
	if (how & REALLY) {
	    /* setting no-corpse affects wishing and random tin generation */
	    if (!(how & MASS_MURDER)) mvitals[mndx].mvflags |= (G_GENOD | G_NOCORPSE);

		if (how & MASS_MURDER) {
	  	    for(mtmp = fmon; mtmp; mtmp = mtmp2) {
	  		mtmp2 = mtmp->nmon;
	  		if(mtmp->data == ptr) mondead(mtmp);
			}
		}

	    pline("Wiped out %s%s.", which,
		  (*which != 'a') ? buf : makeplural(buf));

	    if (killplayer) {
		/* might need to wipe out dual role */
		if (urole.femalenum != NON_PM && mndx == urole.malenum)
		    mvitals[urole.femalenum].mvflags |= (G_GENOD | G_NOCORPSE);
		if (urole.femalenum != NON_PM && mndx == urole.femalenum)
		    mvitals[urole.malenum].mvflags |= (G_GENOD | G_NOCORPSE);
		if (urace.femalenum != NON_PM && mndx == urace.malenum)
		    mvitals[urace.femalenum].mvflags |= (G_GENOD | G_NOCORPSE);
		if (urace.femalenum != NON_PM && mndx == urace.femalenum)
		    mvitals[urace.malenum].mvflags |= (G_GENOD | G_NOCORPSE);

		u.uhp = -1;
		if (how & PLAYER) {
		    killer_format = KILLED_BY;
		    killer = "genocidal confusion";
		} else if (how & ONTHRONE) {
		    /* player selected while on a throne */
		    killer_format = KILLED_BY_AN;
		    killer = "imperious order";
		} else { /* selected player deliberately, not confused */
		    killer_format = KILLED_BY_AN;
		    killer = "scroll of genocide";
		}

	/* Polymorphed characters will die as soon as they're rehumanized. */
	/* KMH -- Unchanging prevents rehumanization */
		if (Upolyd && ptr != youmonst.data) {
			delayed_killer = killer;
			killer = 0;
			You_feel("dead inside.");
		} else
			done(GENOCIDED);
	    } else if (ptr == youmonst.data) {
		rehumanize();
	    }
	    reset_rndmonst(mndx);
	    kill_genocided_monsters();
	    update_inventory();	/* in case identified eggs were affected */
	} else {
	    int cnt = 0;

	    if (!(mons[mndx].geno & G_UNIQ) &&
		    !(mvitals[mndx].mvflags & (G_GENOD | G_EXTINCT)))
		for (i = rn1(3, 4); i > 0; i--) {
		    if (!makemon(ptr, u.ux, u.uy, NO_MINVENT|MM_ANGRY|MM_FRENZIED|MM_XFRENZIED))
			break;	/* couldn't make one */
		    ++cnt;
		    if (mvitals[mndx].mvflags & G_EXTINCT)
			break;	/* just made last one */
		}
	    if (cnt)
		pline("Sent in some %s.", makeplural(buf));
	    else {
		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
	    }
	    /* Amy addition: revgeno can be really powerful, so let's add backlashes... */
	    if (!rn2(3)) {
		int aggroamount = rnd(6);
		if (isfriday) aggroamount *= 2;
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
		while (aggroamount) {

			u.cnd_aggravateamount++;
			makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			aggroamount--;
			if (aggroamount < 0) aggroamount = 0;
		}
		u.aggravation = 0;
		pline("Several monsters come out of a portal.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

	    }
	    if (!rn2(10)) {
		badeffect();
	    }
	}
}

void undo_genocide(void)
{
	int i;
	char buf[BUFSZ];
	int mn;

	for (i=0;i<5;i++) {
		if (i >= 5) {
			pline("%s", thats_enough_tries);
			return;
		}
		getlin("Which monster do you want to ungenocide [type the name]",buf);
		mn = name_to_mon(buf);
		if (mn == -1) {
			pline("Such creatures do not exist in this world.");
			continue;
		}
		if (!(/*mons[mn].geno*/mvitals[mn].mvflags & G_GENOD)) {

			pline("This creature has not been genocided.");
			continue;
		}
		if (mn == PM_UNGENOMOLD) {

			pline("For some reason, you cannot ungenocide this species!");
			continue;
		}

		/*mons[mn].geno*/mvitals[mn].mvflags &= ~G_GENOD;
		pline("The race of %s now exist again.",makeplural(buf));
		break;
	}
}

void
punish(sobj)
register struct obj	*sobj;
{

    struct obj *otmp;
	/* KMH -- Punishment is still okay when you are riding */
	You("are being punished for your misbehavior!");
	u.cnd_punishmentcount++;

	/*if (Is_waterlevel(&u.uz)) return;*/ /* otherwise it crashes for some weird reason --Amy */


	if(Punished){ /* very heavy iron ball */
		Your("iron ball gets heavier.");
		uball->owt += 600 * (1 + sobj->cursed); /*ball weight increases by 600 now --Amy*/
		return;
	}
	if (amorphous(youmonst.data) || is_whirly(youmonst.data) || unsolid(youmonst.data)) {
		pline("A ball and chain appears, then falls away.");
		dropy(mkobj(BALL_CLASS, TRUE, FALSE));
		return;
	}
	setworn(mkobj(CHAIN_CLASS, TRUE, FALSE), W_CHAIN);
    if (((otmp = carrying(HEAVY_IRON_BALL)) != 0) &&(otmp->oartifact ==
     ART_IRON_BALL_OF_LIBERATION)) {
        setworn(otmp, W_BALL);
        Your("%s chains itself to you!", xname(otmp));
    } else {
	setworn(mkobj(BALL_CLASS, TRUE, FALSE), W_BALL);
    }
	/*uball->spe = 1;*/		/* special ball (see save) */

	/*
	 *  Place ball & chain if not swallowed.  If swallowed, the ball &
	 *  chain variables will be set at the next call to placebc().
	 */
	if (!u.uswallow && isok(u.ux, u.uy)) {
	    placebc();
	    if (Blind) set_bc(1);	/* set up ball and chain variables */
	    newsym(u.ux,u.uy);		/* see ball&chain if can't see self */
	}
}

void
punishx()
{

    struct obj *otmp;
	/* KMH -- Punishment is still okay when you are riding */
	You("are being punished for your misbehavior!");

	/*if (Is_waterlevel(&u.uz)) return;*/ /* otherwise it crashes for some weird reason --Amy */
	u.cnd_punishmentcount++;

	if(Punished){ /* very heavy iron ball */
		Your("iron ball gets heavier.");
		uball->owt += 600;
		return;
	}
	if (amorphous(youmonst.data) || is_whirly(youmonst.data) || unsolid(youmonst.data)) {
		pline("A ball and chain appears, then falls away.");
		dropy(mkobj(BALL_CLASS, TRUE, FALSE));
		return;
	}
	setworn(mkobj(CHAIN_CLASS, TRUE, FALSE), W_CHAIN);
    if (((otmp = carrying(HEAVY_IRON_BALL)) != 0) &&(otmp->oartifact ==
     ART_IRON_BALL_OF_LIBERATION)) {
        setworn(otmp, W_BALL);
        Your("%s chains itself to you!", xname(otmp));
    } else {
	setworn(mkobj(BALL_CLASS, TRUE, FALSE), W_BALL);
    }
	/*uball->spe = 1;*/		/* special ball (see save) */

	/*
	 *  Place ball & chain if not swallowed.  If swallowed, the ball &
	 *  chain variables will be set at the next call to placebc().
	 */
	if (!u.uswallow && isok(u.ux, u.uy) ) {
	    placebc();
	    if (Blind) set_bc(1);	/* set up ball and chain variables */
	    newsym(u.ux,u.uy);		/* see ball&chain if can't see self */
	}
}

void
unpunish()
{	    /* remove the ball and chain */
	struct obj *savechain = uchain;

	obj_extract_self(uchain);
	newsym(uchain->ox,uchain->oy);
	setworn((struct obj *)0, W_CHAIN);
	dealloc_obj(savechain);
	/*uball->spe = 0;*/
	setworn((struct obj *)0, W_BALL);
	if (!rn2(5)) mkobj_at(CHAIN_CLASS, u.ux, u.uy, FALSE, FALSE); /* maybe make a chain, since the original one disappeared --Amy */

}

/* some creatures have special data structures that only make sense in their
 * normal locations -- if the player tries to create one elsewhere, or to revive
 * one, the disoriented creature becomes a zombie
 */
boolean
cant_create(mtype, revival)
int *mtype;
boolean revival;
{

	/* SHOPKEEPERS can be revived now */
	if (*mtype==PM_GUARD || *mtype==PM_MASTER_GUARD || *mtype==PM_ELITE_GUARD || (*mtype==PM_SHOPKEEPER && !revival) || (*mtype==PM_MASTER_SHOPKEEPER && !revival) || (*mtype==PM_ELITE_SHOPKEEPER && !revival)
	     || *mtype==PM_ALIGNED_PRIEST || *mtype==PM_MASTER_PRIEST || *mtype==PM_ELITE_PRIEST || *mtype==PM_ANGEL) {
		*mtype = PM_HUMAN_ZOMBIE;
		return TRUE;
	} else if (*mtype==PM_LONG_WORM_TAIL) {	/* for create_particular() */
		*mtype = PM_LONG_WORM;
		return TRUE;
	}
	return FALSE;
}

/* Terrain-altering wands; they have their functions here, because I'm a very bad programmer :P --Amy */
void
wandofchaosterrain()
{
	int madepool = 0;
	int stilldry = -1;
	int x,y,safe_pos=0;
	int radius = 3 + rn2(5);
	if (!rn2(3)) radius += rnd(4);
	if (!rn2(10)) radius += rnd(6);
	if (!rn2(25)) radius += rnd(8);
	if (radius > MAX_RADIUS) radius = MAX_RADIUS;
		do_clear_areaX(u.ux, u.uy, radius, do_terrainflood,
				(void *)&madepool);

	/* check if there are safe tiles around the player */
	for (x = u.ux-1; x <= u.ux+1; x++) {
		for (y = u.uy - 1; y <= u.uy + 1; y++) {
			if (x != u.ux && y != u.uy &&
			    goodpos(x, y, &youmonst, 0)) {
				safe_pos++;
			}
		}
	}

	/* we do not put these on the player's position. */
	if (!madepool && stilldry)
		return;
	if (madepool)
		pline(FunnyHallu ?
				"Oh wow, look at all the stuff that is happening around you!" :
				"What the heck is happening to the dungeon?!" );
	return;

}

void
wandoffleecyterrain()
{
	int terraintype = rnd(29);

	int madepool = 0;
	int stilldry = -1;
	int x,y,safe_pos=0;
	int radius = 3 + rn2(5);
	if (!rn2(3)) radius += rnd(4);
	if (!rn2(10)) radius += rnd(6);
	if (!rn2(25)) radius += rnd(8);
	if (radius > MAX_RADIUS) radius = MAX_RADIUS;

	switch (terraintype) {

		case 1:
			do_clear_areaX(u.ux, u.uy, radius, do_flood, (void *)&madepool);
			break;
		case 2:
			do_clear_areaX(u.ux, u.uy, radius, do_lavaflood, (void *)&madepool);
			break;
		case 3:
			do_clear_areaX(u.ux, u.uy, radius, do_graveflood, (void *)&madepool);
			break;
		case 4:
			do_clear_areaX(u.ux, u.uy, radius, do_tunnelflood, (void *)&madepool);
			break;
		case 5:
			do_clear_areaX(u.ux, u.uy, radius, do_farmflood, (void *)&madepool);
			break;
		case 6:
			do_clear_areaX(u.ux, u.uy, radius, do_mountainflood, (void *)&madepool);
			break;
		case 7:
			do_clear_areaX(u.ux, u.uy, radius, do_watertunnelflood, (void *)&madepool);
			break;
		case 8:
			do_clear_areaX(u.ux, u.uy, radius, do_crystalwaterflood, (void *)&madepool);
			break;
		case 9:
			do_clear_areaX(u.ux, u.uy, radius, do_moorflood, (void *)&madepool);
			break;
		case 10:
			do_clear_areaX(u.ux, u.uy, radius, do_urineflood, (void *)&madepool);
			break;
		case 11:
			do_clear_areaX(u.ux, u.uy, radius, do_shiftingsandflood, (void *)&madepool);
			break;
		case 12:
			do_clear_areaX(u.ux, u.uy, radius, do_styxflood, (void *)&madepool);
			break;
		case 13:
			do_clear_areaX(u.ux, u.uy, radius, do_snowflood, (void *)&madepool);
			break;
		case 14:
			do_clear_areaX(u.ux, u.uy, radius, do_ashflood, (void *)&madepool);
			break;
		case 15:
			do_clear_areaX(u.ux, u.uy, radius, do_sandflood, (void *)&madepool);
			break;
		case 16:
			do_clear_areaX(u.ux, u.uy, radius, do_pavementflood, (void *)&madepool);
			break;
		case 17:
			do_clear_areaX(u.ux, u.uy, radius, do_highwayflood, (void *)&madepool);
			break;
		case 18:
			do_clear_areaX(u.ux, u.uy, radius, do_grassflood, (void *)&madepool);
			break;
		case 19:
			do_clear_areaX(u.ux, u.uy, radius, do_nethermistflood, (void *)&madepool);
			break;
		case 20:
			do_clear_areaX(u.ux, u.uy, radius, do_stalactiteflood, (void *)&madepool);
			break;
		case 21:
			do_clear_areaX(u.ux, u.uy, radius, do_cryptflood, (void *)&madepool);
			break;
		case 22:
			do_clear_areaX(u.ux, u.uy, radius, do_bubbleflood, (void *)&madepool);
			break;
		case 23:
			do_clear_areaX(u.ux, u.uy, radius, do_raincloudflood, (void *)&madepool);
			break;
		case 24:
			do_clear_areaX(u.ux, u.uy, radius, do_lockflood, (void *)&madepool);
			break;
		case 25:
			do_clear_areaX(u.ux, u.uy, radius, do_treeflood, (void *)&madepool);
			break;
		case 26:
			do_clear_areaX(u.ux, u.uy, radius, do_iceflood, (void *)&madepool);
			break;
		case 27:
			do_clear_areaX(u.ux, u.uy, radius, do_cloudflood, (void *)&madepool);
			break;
		case 28:
			do_clear_areaX(u.ux, u.uy, radius, do_barflood, (void *)&madepool);
			break;
		case 29:
			do_clear_areaX(u.ux, u.uy, radius, do_terrainflood, (void *)&madepool);
			break;

	}

	/* check if there are safe tiles around the player */
	for (x = u.ux-1; x <= u.ux+1; x++) {
		for (y = u.uy - 1; y <= u.uy + 1; y++) {
			if (x != u.ux && y != u.uy &&
			    goodpos(x, y, &youmonst, 0)) {
				safe_pos++;
			}
		}
	}

	pline("Some changes in terrain are happening.");
	return;
}

#ifdef WIZARD
/*
 * Make a new monster with the type controlled by the user.
 *
 * Note:  when creating a monster by class letter, specifying the
 * "strange object" (']') symbol produces a random monster rather
 * than a mimic; this behavior quirk is useful so don't "fix" it...
 */
struct monst *
create_particular()
{
	char buf[BUFSZ], *bufp, monclass = MAXMCLASSES;
	int which, tries, i;
	struct permonst *whichpm;
	struct monst *mtmp = (struct monst *)0;
	boolean maketame, makepeaceful, makehostile;

	tries = 0;
	do {
	    which = urole.malenum;      /* an arbitrary index into mons[] */
	    maketame = makepeaceful = makehostile = FALSE;
	    getlin("Create what kind of monster? [type the name or symbol]",
		   buf);
	    bufp = mungspaces(buf);

	    if (*bufp == '\033') return (struct monst *)0;
	    /* allow the initial disposition to be specified */
	    if (!strncmpi(bufp, "tame ", 5)) {
		bufp += 5;
		maketame = TRUE;
	    } else if (!strncmpi(bufp, "peaceful ", 9)) {
		bufp += 9;
		makepeaceful = TRUE;
	    } else if (!strncmpi(bufp, "hostile ", 8)) {
		bufp += 8;
		makehostile = TRUE;
	    }
	    /* decide whether a valid monster was chosen */
	    if (strlen(bufp) == 1) {
		monclass = def_char_to_monclass(*bufp);
		if (monclass != MAXMCLASSES) break;	/* got one */
	    } else {
		which = name_to_mon(bufp);
		if (which >= LOW_PM) break;		/* got one */
	    }
	    /* no good; try again... */
	    pline("I've never heard of such monsters.");
	} while (++tries < 5);

	if (tries == 5) {
	    pline("%s", thats_enough_tries);
	} else {
	    (void) cant_create(&which, FALSE);
	    whichpm = &mons[which];
	    for (i = 0; i <= multi; i++) {
		if (monclass != MAXMCLASSES)
		    whichpm = mkclass(monclass, 0);
		if (maketame) {
		    mtmp = makemon(whichpm, u.ux, u.uy, MM_EDOG);
		    if (mtmp) {
			initedog(mtmp);
			set_malign(mtmp);
		    }
		} else {
		    mtmp = makemon(whichpm, u.ux, u.uy, NO_MM_FLAGS);
		    if ((makepeaceful || makehostile) && mtmp) {
			mtmp->mtame = 0;        /* sanity precaution */
			mtmp->mpeaceful = makepeaceful ? 1 : 0;
			set_malign(mtmp);
		    }
		}
	    }
	}
	return mtmp;
}
#endif /* WIZARD */

/* For GM mode (Game Master mode): this allows Amy to interfere with user's games that are played online.
 * She can then send mail to the player that causes a monster specificed by her to spawn somewhere on the level :D */
#ifdef GMMODE
void
gmmode_genesis(specifictype)
const char *specifictype;
{
	char buf[BUFSZ], *bufp, monclass = MAXMCLASSES;
	int which, i;
	struct permonst *whichpm;
	struct monst *mtmp = (struct monst *)0;

	which = urole.malenum;      /* an arbitrary index into mons[] */
	bufp = specifictype;
	if (*bufp == '\033') return;

	/* decide whether a valid monster was chosen */
	if (strlen(bufp) == 1) {
		monclass = def_char_to_monclass(*bufp);
		if (monclass != MAXMCLASSES) goto okay;	/* got one */
	} else {
		which = name_to_mon(bufp);
		if (which >= LOW_PM) goto okay;		/* got one */
	}
	/* not a valid one... */
	pline(flags.supergmmode ? "Someone tried to spawn an invalid monster (%s)." : "The Amy tried to spawn an invalid monster (%s).", bufp);
	return;

okay:

	(void) cant_create(&which, FALSE);
	whichpm = &mons[which];
	for (i = 0; i <= multi; i++) {
		if (monclass != MAXMCLASSES)
		    whichpm = mkclass(monclass, 0);
		mtmp = makemon(whichpm, 0, 0, MM_ANGRY);
	}

}
#endif /* GMMODE */

#endif /* OVLB */

/*read.c*/

/*	SCCS Id: @(#)role.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


/*** Table of all roles ***/
/* According to AD&D, HD for some classes (ex. Wizard) should be smaller
 * (4-sided for wizards).  But this is not AD&D, and using the AD&D
 * rule here produces an unplayable character.  Thus I have used a minimum
 * of an 10-sided hit die for everything.  Another AD&D change: wizards get
 * a minimum strength of 4 since without one you can't teleport or cast
 * spells. --KAA
 *
 * As the wizard has been updated (wizard patch 5 jun '96) their HD can be
 * brought closer into line with AD&D. This forces wizards to use magic more
 * and distance themselves from their attackers. --LSZ
 *
 * With the introduction of races, some hit points and energy
 * has been reallocated for each race.  The values assigned
 * to the roles has been reduced by the amount allocated to
 * humans.  --KMH
 *
 * God names use a leading underscore to flag goddesses.
 */

/* thanks to CK for pointing out some stat distribution bugs --Amy */

/* IMPORTANT!!! do_wear.c has code for an artifact that turns the player into a Binder by index number. If any new roles
 * are inserted above the Binder, please update the index number accordingly! --Amy */

const struct Role roles[] = {
{	{"Acid Mage", 0}, {
	{"Oozer",         0},
	{"Slimer",       0},
	{"Sludge Mage",       0},
	{"Spiller",     0},
	{"Corroder",       0},
	{"Acidsplasher",      0},
	{"Hazardous Materials Specialist",   0},
	{"Laboratory Science PhD", 0},
	{"Acid-Master",  0} },
	"The Lord of the Pit", "Goldblight of the Flame", "Warpfire Hellspawn", /* Special */
	"Aci", "Slime Pit", "Electric Power Station",
	PM_ACID_MAGE, NON_PM, PM_UNDEAD_ACID_MAGE, NON_PM, PM_ACID_HOUND_PUP,
	PM_ACIDSNORT, PM_ATTENDANT, PM_LIGHTNINGROD, PM_DYNAMO,
	PM_DROW_MATRON, PM_TITAN, S_ANGEL, S_GIANT,

	ART_SLOW_BLADE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  16, 20, 16, 16, 15, 14 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_ACID_STREAM,        -4, 100 /* From old role.c */
},

{	{"Activistor", 0}, {
	{"Little Rebel",    0},
	{"Do-Gooder",  0},
	{"Demonstrant",       0},
	{"Worker on strike",   0},
	{"Protestor", 0},
	{"Rebellious Individual",         0},
	{"Involved Activist",    0},
	{"Renegade Fighter",        0},
	{"Savior",      0} },
	"Anti-War Movement", "Global Freedom Council", "Human Rights Progression", /* buzzwords used by activists */
	"Act", "Activist Home", "Ghostly Halls",
	PM_ACTIVISTOR, NON_PM, PM_UNDEAD_ACTIVISTOR, NON_PM, PM_ACTIVISTOR,
	PM_ACTIVIST_SPEAKER, PM_ATTENDANT, PM_IMELDA_S_GHOST, PM_GHOST_OF_HEEL,
	PM_ANIMATED_WEDGE_SANDAL, PM_ANIMATED_SEXY_LEATHER_PUMP, S_GOLEM, S_NEMESE,
	ART_ACTIVIST_STICK,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 25, 25, 10, 10, 20 },
	/* new limit values for the six attributes by Amy */
	{  18, 14, 14, 18, 16, 20 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 3,  1, 0 },	/* Hit points */
	{ 20, 0,  0, 4,  0, 4 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_TELEPORT_AWAY,   -4, 50
},

{	{ "Aklyst", 0 }, {
	{ "Club Carrier", 0 },
	{ "Bola Thrower", 0 },
	{ "Range Applier", 0 },
	{ "Melee Detonator", 0 },
	{ "Iron Maul", 0 },
	{ "Sounding Clasher", 0 },
	{ "Two-Square Melee Master", 0 },
	{ "Nun-Chuck", 0 },
	{ "World Whacker", 0 } },
	"_Sulis", "_Sequana", "_Damona", /* celtic */
	"Akl", "Marital Arts Dodo", "Ghosterville",
	PM_AKLYST, NON_PM, PM_UNDEAD_AKLYST, NON_PM, NON_PM,
	PM_THREE_POINT_SIX_DEV, PM_APPRENTICE, PM_GRIMACE_CASPAR, PM_DOUR_LAUGHING_GHOST,
	PM_KRUSHE, PM_TACKLE_JELLY, S_GHOST, S_JELLY,
	ART_NOBREAK, /* aklys, +10 damage and +20 to-hit; instead of breaking, it loses enchantment */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 25, 10, 10, 25, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{ STR19(19), 17, 17, 19, 18, 16 },
	/* Init   Lower  Higher */
	{ 12, 0, 0, 5, 4, 0 }, /* Hit points */
	{  3, 3, 0, 3, 0, 3 },15, /* Energy */
	10, 9, -2,
	6, 12, A_INT, SPE_MAGICTORCH, -4, 30
},

{	{"Altmer", 0}, {
	{"Cantrip Caster",    0},
	{"Stunted Magician",       0},
	{"Vapor Mage",       0},
	{"Ghost Mage",       0},
	{"Mage Dominion",      0},
	{"Mage Empress",     0},
	{"Spellbane",      0},
	{"Armored Mage",        0},
	{"Archmage",      0} },
	"_Eilistraee", "_Kiaransali", "_Lolth", /* Elven */

	"Alt", "Summerset Isle", "Necrotic Caverns",
	PM_ALTMER, NON_PM, PM_UNDEAD_ALTMER, NON_PM, PM_GOLD_GOLEM,
	PM_SINDERION, PM_HIGHEST_ELF, PM_MANNIMARCO, PM_WORM_ANCHORITE,
	PM_NECROMANCER, PM_DARK_SAGE, S_HUMAN, S_HUMAN,

	ART_AMBASSADOR_ROBE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 30, 10, 10, 10 },
	/* new limit values for the six attributes by Amy */
	{  12, 25, 18, 15, 14, 16 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 4,  2, 0 },	/* Hit points */
	{ 40, 0,  0, 12,  0, 15 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_FINGER_OF_DEATH,   -4, 250
},

{	{"Amazon", 0}, {
	{"Villager",    0},
	{"Chosen for Reaping",       0},
	{"Low Tribute",       0},
	{"Tribute",       0},
	{"Career Tribute",      0},
	{"Master Hunter",     0},
	{"Hunger Games Winner",      0},
	{"Peetabane",        0},
	{"Right Hand of Cato",      0} },
	"_Suzanne Collins", "_Effie Trinket", "President Snow", /* hunger games */
	"Ama", "Cornucopia Fresh Forest", "Hunger Games Battleground",
	PM_AMAZON, NON_PM, PM_UNDEAD_AMAZON, NON_PM, PM_VALKYRIE,
	PM_CATO, PM_HUNTER, PM_PEETA, PM_MAD_BUCKING_FASTARD,
	PM_TRIBUTE, PM_TRACKER_JACKER, S_HUMAN, S_ANT,

	ART_GUNBOW,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  8,  7, 10, 17,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15,  8, 25, 14, 18 },
	/* new limit values for the six attributes by Amy */
	{  18, 16, 15, 25, 17, 22 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 5,  4, 0 },	/* Hit points */
	{ 10, 0,  0, 2,  0, 3 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_ENTRAPPING,    -4, 40
},
{	{"Anachronist", 0}, {
	{"Survivor",   0},
	{"Recruit",0},
	{"Insurgent",0},
	{"Trooper",     0},
	{"Rebel Leader",   0},
	{"Chrono Commandant",   0},
	{"Anachrononononaut",0},
	{"Fatebreaker",   0},
	{"Hope of the Future",     0} },
	0, 0, 0, /* chosen randomly from among the other roles */
	"Ana", "the Last Redoubt", "Ground Zero",
	PM_ANACHRONIST, NON_PM, PM_UNDEAD_ANACHRONIST, NON_PM, PM_JEDI,
	PM_SARA__THE_LAST_ORACLE, PM_TROOPER, PM_ELDER_BRAIN, PM_LOWER_BRAIN,
	PM_DOPPELGANGER, PM_MIND_FLAYER, S_TRAPPER, S_UMBER,
	ART_ITCHALAQUIAQUE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/* {  10, 10,  7, 10,  7,  7 }, */
	{   3,  3,  3,  3,  3,  3 },
	{  17, 18, 17, 13, 19, 16 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 14, 18, 20, 12 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 2 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_SLOW_MONSTER,   -9, 20
},

{	{"Anachronounbinder", 		 0}, {
	{"Hexorcist",        0},
	{"Anachrononono-nononono",      0},
	{"Vestige Vanquisher",      0},
	{"Spirit Killer",       0},
	{"Mind Player", 0},
	{"Psionaut", 0},
	{"Void Voider",       0},
	{"Telekinetic",     0},
	{"Keeper of the Gate",    0} },
	"Tariru", "FIQ", "Demo",	/* dnethack players */
	"Acu", "The Elder Pool", "The Far Reach",
	PM_ANACHRONOUNBINDER, NON_PM, PM_UNDEAD_ANACHRONOUNBINDER, NON_PM, PM_BRAIN_GOLEM,
	PM_MASTER_ULITHARID, PM_VERY_MINDLESS_THRALL, /*PM_MENZOBERRANZAN*/PM_DNETHACK_MASTER_DEV, PM_DNETHACK_PLAYER,
	PM_DOPPELGANGER, PM_MIND_FLAYER, S_TRAPPER, S_UMBER,
	ART_ELDER_CEREBRAL_FLUID,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   3,  3,  3,  3,  3,  3 },
	{  17, 18, 17, 13, 19, 16 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 4,  2, 0 },	/* Hit points */
	{  4, 0,  0, 2,  0, 3 },12,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_DETECT_MONSTERS,   -9, 20
},

{	{"Archeologist", 0}, {
	{"Digger",      0},
	{"Field Worker",0},
	{"Investigator",0},
	{"Exhumer",     0},
	{"Excavator",   0},
	{"Spelunker",   0},
	{"Speleologist",0},
	{"Collector",   0},
	{"Curator",     0} },
	"Quetzalcoatl", "Camaxtli", "Huhetotl", /* Central American */
	"Arc", "the College of Archeology", "the Tomb of the Toltec Kings",
	PM_ARCHEOLOGIST, NON_PM, PM_UNDEAD_ARCHEOLOGIST, NON_PM, NON_PM,
	PM_LORD_CARNARVON, PM_STUDENT, PM_MINION_OF_HUHETOTL, PM_MINION_OF_QUETZALCOATL,
	NON_PM, PM_HUMAN_MUMMY, S_SNAKE, S_MUMMY,
#if 0
	ART_WEREBANE, ART_GRAYSWANDIR,
#endif
	ART_ORB_OF_DETECTION,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	  ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
/*	{   7, 10, 10,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 20, 10, 20, 10 }, /* This must add up to 100 or it won't work right (attrib.c uses this code). Again, thanks to CK for pointing it out. --Amy */
	/* new limit values for the six attributes by Amy */
	{  18, 16, 17, 20, 16, 18 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 10, 0,  0, 1,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_MAGIC_MAPPING,   -4, 40
},

{	{"Artist", 0}, {
	{"Paint Splotcher",     0},
	{"Brushswinger",     0},
	{"Color Cribber",     0},
	{"Painter",     0},
	{"Picture Designer",     0},
	{"Successful Artist",     0},
	{"Brush Virtue",     0},
	{"Canvas Enchanter",     0},
	{"Creator of Unique Paintings",     0}},
	"Leonardo", "Picasso", "Dali", /* famous artists */
	"Art", "Painting Studio", "Vandalized Plains",
	PM_ARTIST, NON_PM, PM_UNDEAD_ARTIST, NON_PM, NON_PM,
	PM_PAINTMASTER, PM_GUIDE, PM_BRAGGING_VANDAL, PM_FUCKED_UP_PERSON,
	PM_CRITIC, PM_VANDAL, S_HUMAN, S_HUMAN,

	ART_MIRAGE_TAIL,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7, 10,  6,  7,  7, 10 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  12, 20, 20, 20, 12, 16 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 15, 15, 20, 16, 19 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  0, 0 },	/* Hit points */
	{ 12, 0,  0, 1,  0, 1 },14,	/* Energy */
	0, 5, 1, 2, 10, A_INT, SPE_INVISIBILITY,   -4, 70

},

{	{"Assassin", 0}, {
	{"Dark Brotherhood Initiate",     0},
	{"Whetblade",     0},
	{"Killer",     0},
	{"Contract Killer",     0},
	{"Silent Killer",     0},
	{"Devious Cutthroat",     0},
	{"Knife After Dark",     0},
	{"Deadly Blade",     0},
	{"Merciless Murderer",     0}},
	"Ehud", "Ford", "Ivins", /* unknown origin */
	"Ass", "the Assassins' Guild Hall", "the Thieves' Guild Hall",
	PM_ASSASSIN, NON_PM, PM_UNDEAD_ASSASSIN, NON_PM, PM_HUMAN_THIEF,
	PM_MASTER_OF_ASSASSINS, PM_THUG, PM_MASTER_OF_THIEVES, PM_MASTER_OF_DESASTER, /* also tourist quest nemesis */
	PM_SMIRKING_SNEAK_THIEF, PM_ROGUE, S_HUMAN, S_HUMAN,

	ART_KILLING_EDGE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7,  7, 10,  7,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 15, 5, 30, 15, 5 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 14, 21, 16, 19 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_INVISIBILITY, -4, 40
},

{	{"Astronaut", 0}, {
	{"Rocket Tester",   0},
	{"Vostok Martyr",    0},
	{"Deimos Stevedore",      "Moon Walker"},
	{"Leonov Engineer",     0},
	{"Marauder Pilot",      0},
	{"Von Braun Staff",      0},
	{"LongShot Navigator",      0},
	{"Tie Fighter Ace",      0},
	{"Nostromo Survivor",   0} },
	"Neil Armstrong", "_Sally Ride", "Yuri Gagarin", /* famous astronauts */
	"Ast", "NAFA Headquarter", "Terrorist Underground",
	PM_ASTRONAUT, NON_PM, PM_UNDEAD_ASTRONAUT, NON_PM, PM_UFO,
	PM_NAFA_BOSS, PM_STARNOID, PM_THE_MAN_IN_THE_MOON, PM_DECIMATED_SPINNER,
	PM_NASTY_LIGHT, PM_PSI_RAY, S_LIGHT, S_WORM_TAIL,

	ART_ROCKET_IMPULSE, /* torso armor */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 14, 14, 12,  20, 20 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 19, 19, 20, 20, 19 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 5,  4, 0 },	/* Hit points */
	{ 12, 0,  0, 3,  0, 2 }, 14,	/* Energy */
	10, 6, 0, 2, 10, A_INT, SPE_TIME_STOP,      -4, 100
},

{	{"Augurer", 0}, {
	{"College Student",         0},
	{"Deviant Seer",            0},
	{"Fortune Teller",          0},
	{"Seer Graduate", 0},
	{"Visioneer", 0},
	{"Wise Seer", 0},
	{"Void Diviner",  0},
	{"Psychic Visor",   0},
	{"Hidden Depth Explorer",            0} },
	"Dunlain", "Savos Aren", "_Hert the Vampire", /* Skyrim */
	"Aug", "Icy Depths of Winterhold",
	  "Labyrinthian Den",
	PM_AUGURER, NON_PM, PM_UNDEAD_AUGURER, NON_PM, PM_FLOATING_EYE,
	PM_UPPER_AUGURER, PM_STUDENT, PM_ESTORMO, PM_ANCANO,
	PM_THALMOR, PM_FAIRY_DRAGON, S_DRAGON, S_HUMAN,

	ART_ARCHON_STAFF,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  8,  8,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{   5, 30, 30,  5,  5, 25 },
	/* new limit values for the six attributes by Amy */
	{  10, 22, 25, 17, 13, 16 },
	/* Init   Lower  Higher */
	{ 9, 0,  0, 0,  1, 0 },	/* Hit points */
	{ 18, 4,  0, 4,  0, 8 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_PASSWALL, -4, 200
},

{	{"Barbarian", 0}, {
	{"Plunderer",   "Plunderess"},
	{"Pillager",    0},
	{"Bandit",      0},
	{"Brigand",     0},
	{"Raider",      0},
	{"Reaver",      0},
	{"Slayer",      0},
	{"Chieftain",   "Chieftainess"},
	{"Conqueror",   "Conqueress"} },
	"Mitra", "Crom", "Set", /* Hyborian */
	"Bar", "the Camp of the Duali Tribe", "the Duali Oasis",
	PM_BARBARIAN, NON_PM, PM_UNDEAD_BARBARIAN, NON_PM, NON_PM,
	PM_PELIAS, PM_CHIEFTAIN, PM_THOTH_AMON, PM_THOTH_AMUN,
	PM_OGRE, PM_TROLL, S_OGRE, S_TROLL,
#if 0
	ART_CLEAVER, ART_DEATHSWORD,
#endif
	ART_HEART_OF_AHRIMAN,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30,  6,  7, 20, 30,  7 },
	/* new limit values for the six attributes by Amy */
	{  STR19(22), 8, 8, 16, 20, 8 },
	/* Init   Lower  Higher */
	{ 20, 0,  0,10,  2, 0 },	/* Hit points */
	{  5, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 14, 0, 0,  8, A_INT, SPE_HASTE_SELF,      -4, 15
},

{	{"Bard", 0}, {
	{"Rhymer",      0},
	{"Lyrist",      0},
	{"Sonneteer",   0},
	{"Jongleur",    0},
	{"Troubadour",  0},
	{"Minstrel",    0},
	{"Lorist",      0},
	{"Well-known Bard",        0},
	{"Master Bard", 0} },
	"Garl Glittergold", "Flandal Steelskin", "Urdlen",
	"Brd", "the Conservatorium", "the Island of Anthemoessa",
	PM_BARD, NON_PM, PM_UNDEAD_BARD, NON_PM, NON_PM,
	PM_PINDAR, PM_RHYMER, PM_AGLAOPE, PM_SKYLLA,
	PM_SNAKE, PM_WHITE_UNICORN, S_SNAKE, S_UNICORN,
	ART_LYRE_OF_ORPHEUS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7,  7, 10,  6, 10 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 10, 15, 25, 10, 30 },
	/* new limit values for the six attributes by Amy */
	{  14, 18, 18, 18, 15, 22 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  4, 3,  0, 1,  0, 2 },10,	/* Energy */
	10, 3,-3, 2, 9, A_INT, SPE_SLEEP, -4, 40
},


{	{"Binder", 		 0}, {
	{"Exile",        0},
	{"Heratic",      0},
	{"Cultist",      0},
	{"Real Binder",       0},
	{"Akousmatikoi", 0},
	{"Mathematikoi", 0},
	{"Doctor",       0},
	{"Unbinder",     0},
	{"Gnostikos",    0} },
	"Yaldabaoth", "the void", "_Sophia", /* Gnostic */
	"Bin", "the lost library", "the Tower of Woe",
	PM_BINDER, NON_PM, PM_UNDEAD_BINDER, NON_PM, NON_PM,
	PM_STRANGE_CORPSE, NON_PM, PM_ACERERAK, PM_RECERARAK,
	PM_SKELETON, PM_LICH, S_HUMANOID, S_LICH,
	ART_PEN_OF_THE_VOID,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  6,  6,  6,  6,  6,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  16,  17,  17, 17, 16,  17 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 10,  2, 0 },	/* Hit points */
	{  5, 0,  1, 0,  1, 0 },15,	/* Energy */
	-5, 10, 5, 10,  25, A_INT, SPE_SLEEP,             -14, 30
},

{	{"Bleeder", 		 0}, {
	{"Leg-scratched Boy",        "Leg-scratched Girl"},
	{"Wounded Guy",      "Wounded Gal"},
	{"Dripper",      0},
	{"Pourer",       0},
	{"Red Lake Creator", 0},
	{"Tearshedder", 0},
	{"Unlucky Individual",       0},
	{"Diseased Sibling",     0},
	{"Cursed King",    "Cursed Queen"} },
	"Glycocalyx", "Fibrinogen", "_Hemophilia", /* hereditary disease */
	"Ble", "hemorrhagic hospital", "red-sprinkled battle area",
	PM_BLEEDER, NON_PM, PM_UNDEAD_BLEEDER, NON_PM, PM_ANIMATED_WEDGE_SANDAL,
	PM_DISEASED_HEIR, PM_UNFORTUNATE_VICTIM, PM_BLOODY_BEAUTIES, PM_SCRATCHING_MARIA,
	PM_BATTLE_GIRL, PM_ANIMATED_SEXY_LEATHER_PUMP, S_GOLEM, S_HUMAN,
	ART_BLOODY_BEAUTY,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  6,  6,  6,  6,  6,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  12,  20,  24, 8, 10,  26 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 20, 18, 20, 18 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 16,  12, 0 },	/* Hit points */
	{  8, 0,  4, 0,  4, 0 },15,	/* Energy */
	0, 10, 5, 10,  25, A_INT, SPE_FULL_HEALING,             -4, 35
},

{	{"Bloodseeker", 0}, {
	{"Battlefield Newbie",   0},
	{"Training Fighter",   0},
	{"Weaponbearer",   0},
	{"Battle Veteran",   0},
	{"Honored Fighter",   0},
	{"Cruel Warlord",   0},
	{"Bonecrusher",   0},
	{"Spinebreaker",   0},
	{"Decapitator",   0}},
	"_Rhea Oro", "_Liebea Luna", "_Elenya Pure", /* taken from a fanfic */
	"Blo", "Vlad's Second Tower", "deep in the depths of the dungeon",
	PM_BLOODSEEKER, NON_PM, PM_UNDEAD_BLOODSEEKER, NON_PM, PM_VAMPIRE,
	PM_VLAD_THE_SKEWERER, PM_CHIEFTAIN, PM_MAILER_DAEMON, PM_SPAM_MAIL_DEMON,
	PM_ANIMATED_ARMOR, PM_PRINTER_DAEMON, S_GOLEM, S_DEMON,

	ART_RUPTURER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30,  6,  7, 20, 30,  7 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 12, 14, 18, 18, 12 },
	/* Init   Lower  Higher */
	{ 20, 0,  0,10,  2, 0 },	/* Hit points */
	{  5, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 14, 0, 0,  8, A_INT, SPE_DRAIN_LIFE,      -4, 20
},
{	{"Bosmer", 0}, {
	{"Treehut Dweller",      0},
	{"Flora Watcher",      0},
	{"Timberlander",   0},
	{"Leafcloth Wearer",    0},
	{"Woodrunner",  0},
	{"Jungle Hunter",    0},
	{"Guerilla Bowman",      0},
	{"Bow Sniper",        0},
	{"Master Hunter", 0} },
	"Eddergud", "Vhaeraun", "the black web", /* Elven */

	"Bos", "Valenwood", "Fiery Pits of Despair",
	PM_BOSMER, NON_PM, PM_UNDEAD_BOSMER, NON_PM, PM_DINGO_PUPPY,
	PM_GLARTHIR, PM_HIGHEST_ELF, PM_ELSE_GOD_HATER, PM_MYTHICAL_DAWN_AGENT,
	PM_CHASME, PM_FIRE_DEMON, S_DEMON, S_DEMON,

	ART_BOW_OF_VINES,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 15, 15, 30, 10, 20 },
	/* new limit values for the six attributes by Amy */
	{  18, 19, 20, 24, 13, 20 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  8, 0,  0, 2,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -4, 40
},

{	{"Bully", 0}, {
	{"Money Addict",     0},
	{"Small-time Bully",     0},
	{"Little Mobber",     0},
	{"Clique Bully",     0},
	{"Cape Stealer",     0},
	{"Leather-clad Thief",     0},
	{"Mean Mobbing Bully",     0},
	{"Violent Bully",     0},
	{"Weapon Toting Bully",     0}},
	"_Everella Shrine", "Butch DeLoria", "Draco Malfoy",	/* popular bullies */
	"Bul", "your classroom", "greaseball's home",
	PM_BULLY, NON_PM, PM_UNDEAD_BULLY, NON_PM, PM_CLEFAIRY,
	PM_PRINCIPAL_CLARK, PM_THUG, PM_JOE_THE_GREASER, PM_WALLY_WITH_THE_BROLLY,
	PM_LECTURER, PM_UNDEAD_OFFICER, S_HUMAN, S_HUMAN,
	ART_SHARPENED_TOOTHPICK,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 5, 15, 15, 30, 5 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 12, 10, 19, 18, 18 },
	/* Init   Lower  Higher */
	{  10, 0,  0, 5,  0, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	-10, 5, 0, 2, 10, A_INT, SPE_FINGER,   -4, 30
},

{	{"Butt Lover", 0}, {
	{"Stroking Hands",         0},
	{"Feel-Upper",         0},
	{"Farting Gas Inhaler",         0},
	{"Squeaking Noise Enjoyer",         0},
	{"Curve Lover",         0},
	{"Graceful Body Admirer",         0},
	{"Blissful Air Current",         0},
	{"Sexiness Expert",         0},
	{"Master Softhand",  0} },
	"_Eveline", "_Elena", "_Marlen",
	"But", "Farting Gas Chamber", "Stupid Law Factory",
	PM_BUTT_LOVER, NON_PM, PM_UNDEAD_BUTT_LOVER, NON_PM, PM_JESSICA,
	PM_BEN_ARGIMUNT, PM_MAIDEN, PM_CATCALL_SELINA, PM_ORGASMIC_BITCH,
	PM_FARTING_WOMAN, PM_PRETTY_ASIAN_WOMAN, S_HUMAN, S_HUMAN,

	ART_GROPESTROKER, /* gloves */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 10, 20, 30, 10, 10 },
	/* new limit values for the six attributes by Amy */
	{  18, 12, 15, 20, 14, 12 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 2,  3, 0 },	/* Hit points */
	{  5, 1,  0, 1,  0, 0 }, 10,	/* Energy */
	10, 10, -2, 
	4, 6, A_INT, SPE_STINKING_CLOUD,        -4, 30 /* From old role.c */
},

{	{"Camperstriker", 0}, {
	{"Aimbot User",     0},
	{"Wallhack Exploiter",     0},
	{"Dirty Camper",    0},
	{"Asshole Player",    0},
	{"Kickban Bait",      0},
	{"MMORPG Troll",    0},
	{"Server Hacker",   0},
	{"Godmode Cheater",   0},
	{"Instawin Scriptkiddy",     0} },
	"_the Bitches", "_the Hell Brides", "_the Sluts with Syphilis", /* taken from a fanfic --Amy */
	"Cam", "Tiny Hamlet", "Inside Part",
	PM_CAMPERSTRIKER, NON_PM, PM_UNDEAD_CAMPERSTRIKER, NON_PM, NON_PM,
	PM_MARC, PM_ATTENDANT, PM_ARABELLA, PM_TUFTED_ASIAN_GIRL,
	PM_SMART_TRAPPER, PM_TRAPPER_KING, S_HUMAN, S_HUMAN,
	ART_AMULET_OF_KINGS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   3,  3,  3,  3,  3,  3 },
	{  17, 17, 16, 17, 17, 16 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 4,  1, 0 },	/* Hit points */
	{ 7, 2,  0, 1,  0, 1 },10,	/* Energy */
	0, 12,-2, 0,  10, A_INT, SPE_REMOVE_CURSE,     -4, 100
},

{	{ "Cartomancer", 0 }, {
	{ "Learner", 0 },
	{ "Shuffler", 0 },
	{ "Player", 0 },
	{ "Reader", 0 },
	{ "Strategist", 0 },
	{ "Deck Stacker", 0 },
	{ "Card Collector", 0 },
	{ "Deck Stacker", 0 },
	{ "King of Games", "Queen of Games" } },
	"Johnny", "Spike", "Timmy", /* Card player archetypes */
	"Car", "the Card Training School", "the Great Card Arena",
	PM_CARTOMANCER, NON_PM, PM_UNDEAD_CARTOMANCER, NON_PM, PM_LITTLE_DOG,
	PM_LORD_OF_THE_CARDS, PM_STUDENT, PM_DAL_ZETHIRE, PM_MTG_OVERLORD,
	PM_COBRA, PM_GUARDIAN_NAGA, S_SNAKE, S_NAGA,

	ART_HOLOGRAPHIC_VOID_LILY,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  18, 20, 18, 18, 16, 18 },
	/* Init   Lower  Higher */
	{ 15, 0, 0, 8, 1, 0 }, /* Hit points */
	{ 15, 3, 0, 2, 0, 3 },12, /* Energy */
	0, 1, 0,
	3, 10, A_INT, SPE_CREATE_MONSTER, -4, 50
},

{	{"Caveman", "Cavewoman"}, {
	{"Troglodyte",  0},
	{"Aborigine",   0},
	{"Wanderer",    0},
	{"Vagrant",     0},
	{"Wayfarer",    0},
	{"Roamer",      0},
	{"Nomad",       0},
	{"Rover",       0},
	{"Pioneer",     0} },
	"Anu", "_Ishtar", "Anshar", /* Babylonian */
	"Cav", "the Caves of the Ancestors", "the Dragon's Lair",
	PM_CAVEMAN, PM_CAVEWOMAN, PM_UNDEAD_CAVEMAN, PM_UNDEAD_CAVEWOMAN, PM_LITTLE_DOG,
	PM_SHAMAN_KARNOV, PM_NEANDERTHAL, PM_CHROMATIC_DRAGON, PM_OCTARINE_DRAGON,
	PM_BUGBEAR, PM_HILL_GIANT, S_HUMANOID, S_GIANT,
#if 0
	ART_GIANTKILLER, ART_SKULLCRUSHER,
#endif
	ART_SCEPTRE_OF_MIGHT,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7,  8,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30,  6,  7, 20, 30,  7 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 8, 10, 17, 21, 6 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  2, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 12, 0, 1,  8, A_INT, SPE_DIG,             -4, 20
},

{	{"Cellar Child", 0}, {
	{"Isolated Modder",   0},
	{"Autistic Programmer",    0},
	{"Whiny Crybaby",     0},
	{"Balance Analphabet",      0},
	{"Dumplog Scanner",      0},
	{"Ragefitter",      0},
	{"Accusing Lux",      0},
	{"Repository Deleter",      0},
	{"Self-Appointed Coding God",   0} },
	"_Your Magical Roommate", "_Your Annoying Sister", "_Your Insufferable Mother-In-Law", /* special */
	"Cel", "Lightless Crib", "Agoraphobia Avenue",
	PM_CELLAR_CHILD, NON_PM, PM_UNDEAD_CELLAR_CHILD, NON_PM, PM_MONKEY,
	PM_KRIS_TIAN, PM_ATTENDANT, PM_GRANDMASTER_SVEN, PM_WORLD_PWNZOR,
	PM_VOLCANO_GIANT, PM_ALGROTH, S_GIANT, S_TROLL,

	ART_PEDIATRIC_GAWKING_GANGS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{   5,  5,  5, 75,  5,  5 },
	/* new limit values for the six attributes by Amy */
	{  12, 10, 15, 14, 12,  4 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 1,  1, 0 },	/* Hit points */
	{  12, 0, 0, 5,  0, 4 }, 9,	/* Energy */
	-20, 0, -6, 10, 20, A_INT, SPE_TELEPORT_AWAY,      -4, 200
},

{	{"Chaos Sorceror", 0}, {
	{"Chaotic Dweller",         0},
	{"Enemy of Order",         0},
	{"Madman",         "Madwoman"},
	{"Vicious Diceroller",         0},
	{"Russian Roulette Player",         0},
	{"Spell Randomizer",         0},
	{"Effect Warper",         0},
	{"Cool Calculator",         0},
	{"Master of Entropy",  0} },
	"_Arle Nadja", "_Celine Jules", "_Miki Onimaru", /* anime */
	"Cha", "Motherfucker's Den", "Robber's Crib",
	PM_CHAOS_SORCEROR, NON_PM, PM_UNDEAD_CHAOS_SORCEROR, NON_PM, PM_CHAOS_POLYMORPHER,
	PM_H__THE_MOTHERFUCKER, PM_ATTENDANT, PM_CRISTI_THE_ROBBER, PM_SIGMUND_FROM_DCSS,
	PM_CHANGING_LASER, PM_RNG_LASER, S_LIGHT, S_EYE,
	ART_BLADE_OF_GOTHMOG,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 15, 10, 25, 10 },
	/* new limit values for the six attributes by Amy */
	{  18, 20, 16, 14, 14, 13 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 1,  10, 0 },	/* Hit points */
	{ 17, 4,  0, 6,  0, 1 }, 16,	/* Energy */
	-10, 0, 2, 
	3, 12, A_INT, SPE_HELLISH_BOLT,        -4, 100 /* From old role.c */
},

{	{"Chevalier", 0}, {
	{"Basher",    0},
	{"Undefiler",     0},
	{"Faithful",       0},
	{"Religious One",      0},
	{"Returned One",      0},
	{"Sacred One",       0},
	{"Celestial",        0},
	{"Decomposer",   0},
	{"Annihilator", 0} },
	"Buddha", "Jahwe", "Allah",	/* major real-world religions */
	"Che", "Ancient Temple", "the World-Eater's domain",
	PM_CHEVALIER, NON_PM, PM_UNDEAD_CHEVALIER, NON_PM, NON_PM,
	PM_EREC, PM_ACOLYTE, PM_ALDUIN, PM_WALDUIN,
	PM_TROLL_ZOMBIE, PM_MANTICORE, S_DRAGON, S_DRAGON,
	ART_MAGIC_MIRROR_OF_ARTHUBERT,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7, 10,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 10, 30, 15, 20, 5 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 16, 18, 17, 17, 18 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 9,  1, 0 },	/* Hit points */
	{ 17, 3,  0, 2,  0, 2 },10,	/* Energy */
	0, 3,-2, 2, 10, A_WIS, SPE_REMOVE_CURSE,    -4, 70
},

{	{"Climacterial", 0}, {
	{"Body Juice Worshipper",	0},
	{"Icky Sapper",	0},
	{"Outbleeder",	0},
	{"Orgasmaniac",	0},
	{"Menstrator",	0},
	{"Eggjumper",	0},
	{"Menopause Hesitator",	0},
	{"Big Birthmother",	0},
	{"Smegmatic Reacher",	0} },
	"_Sophie Kinsella", "_Mavis Cheek", "_Charlotte Roche", /* authors */
	"Cli", "Countryside Birthclinic", "The Temple of Xian",
	PM_CLIMACTERIAL, NON_PM, PM_UNDEAD_CLIMACTERIAL, NON_PM, PM_ISABEL,
	PM_NURSE_CATHERINE, PM_ATTENDANT, PM_LARUS, PM_MALE_VERSION_OF_LARA_CROFT,
	PM_BROWN_BEAR, PM_HANGING_SEMEN_EJACULATOR, S_QUADRUPED, S_TURRET,
	ART_CLICKPASS, /* soft chastity belt */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 20, 10, 20, 15, 25 },
	/* new limit values for the six attributes by Amy */
	{  14, 19, 18, 22, 18, 22 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 2,  1, 0 },	/* Hit points */
	{  4, 0,  0, 2,  0, 7 },14,	/* Energy */
	10, 8, -5, 3, 6, A_INT, SPE_BLOOD_STREAM,   -4, 50
},

{	{ "Combatant", 0 }, {
	{ "Fighter Lad", 0 },
	{ "Puncher", 0 },
	{ "Weaponless Adventurer", 0 },
	{ "Spell-Abstainer", 0 },
	{ "Trained Muscle", 0 },
	{ "Lack Compensator", 0 },
	{ "Specialized Minmaxer", 0 },
	{ "Almost Supreme Master", 0 },
	{ "Combat Boss", 0 } },
	"Arev", "U.GUR", "Khaldi", /* armenian */
	"Com", "Battlefist Grounds", "Ontology Stage",
	PM_COMBATANT, NON_PM, PM_UNDEAD_COMBATANT, NON_PM, NON_PM,
	PM_MASTER_KENJI, PM_STUDENT, PM_RIVAL_OF_ENVY, PM_DARK_HAIRED_BOSS,
	PM_APPARITION, PM_SPLICESHEE, S_WRAITH, S_GOLEM,
	ART_MAJOR_PRESENCE, /* gauntlets, +2 to-hit and damage */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 20, 14, 14, 20, 20, 12 },
	/* new limit values for the six attributes by Amy */
	{ STR18(100), 16, 16, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 16, 0, 0, 3, 2, 0 }, /* Hit points */
	{  5, 3, 0, 2, 0, 2 },14, /* Energy */
	10, 7, -2,
	5, 10, A_INT, SPE_GAIN_SPACT, -4, 40
},

{	{"Convict", 0}, {
	{"Detainee",     0},
	{"Inmate",   0},
	{"Jail-bird",0},
	{"Prisoner",0},
	{"Outlaw",    0},
	{"Crook",   0},
	{"Desperado",     0},
	{"Felon",    0},
	{"Fugitive",  0} },
	"Ilmater", "Grumbar", "_Tymora",	/* Faerunian */
	"Con", "Castle Waterdeep Dungeon", "the Warden's Level",
	PM_CONVICT, NON_PM, PM_UNDEAD_CONVICT, NON_PM, PM_SEWER_RAT,
	PM_ROBERT_THE_LIFER, PM_INMATE, PM_WARDEN_ARIANNA, PM_WARDEN_BRIANNA,
	PM_GIANT_BEETLE, PM_SOLDIER_ANT, S_RODENT, S_SPIDER,
	ART_IRON_BALL_OF_LIBERATION,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 12, 12, 20, 18, 18 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	-50, 5, 0, 2, 10, A_INT, SPE_TELEPORT_AWAY,   -4, 20
},

{	{"Cook", 0}, {
	{"one who forgot the salt",     0},
	{"pepper-choker",    0},
	{"second-class waiter",       "second-class waitress"},
	{"waiter",    "waitress"},
	{"kitchen chef",      0},
	{"star cook",     0},
	{"five-star cook",     0},
	{"national championship cook",     0},
	{"world-championship cook",       0} },
	"McDonalds", "Kentucky's Fried Chicken", "Burger King", /* fast food places */
	"Coo", "Goose Cooking Quarters", "that stupid monkey's place",
	PM_COOK, NON_PM, PM_UNDEAD_COOK, NON_PM, PM_LITTLE_DOG,
	PM_KING_K__ROOL, PM_ATTENDANT, PM_DONKEY_KONG, PM_KIDDY_KONG,
	PM_CROCODILE, PM_MASTODON, S_QUADRUPED, S_FELINE,

	ART_KITCHEN_CUTTER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7,  7, 10,  7,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 10, 10, 30, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(10), 16, 14, 19, 16, 19 },
	/* Init   Lower  Higher */
	{ 9, 0,  0, 3,  1, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_FIREBALL, -4, 30
},


{	{"Courier", 0}, {
	{"Vault Dweller",     0},
	{"Hiker",   0},
	{"Urban Legend",0},
	{"Landstalker",0},
	{"Searcher",    0},
	{"Capital Crime Queen",   0},
	{"Opportunist",     0},
	{"Peacebringer",    0},
	{"Messiah",  0} },
	"President Kimball", "Mr. House", "Caesar",	/* Fallout New Vegas */
	"Cou", "in the middle of nowhere", "scum hideout",
	PM_COURIER, NON_PM, PM_UNDEAD_COURIER, NON_PM, PM_LITTLE_DOG,
	PM_YES_MAN, PM_ATTENDANT, PM_SOME_RANDOM_GUY, PM_CLONED_NEW_VEGAS_NPC,
	PM_JACKAL, PM_CAZADOR, S_DOG, S_ANT,
	ART_WITHERED_NINE_MILLIMETER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(20), 14, 10, 14, 16, 10 },
	/* Init   Lower  Higher */
	{  10, 0,  0, 5,  0, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	-10, 5, 0, 2, 10, A_INT, SPE_ENDURE_HEAT,   -4, 20
},

{	{"Cracker", 0}, {
	{"Savescummer",   0},
	{"File Sharer",    0},
	{"W@r3z d00d",      0},
	{"Script Kiddie",     0},
	{"H@x0r",      0},
	{"1337 H@x0r",      0},
	{"Decker",      0},
	{"Virusneaker",      0},
	{"Phreaker",   0} },
	"Wikileaks", "Guccifer 2.0", "Anonymous", /* h@cking */
	"Cra", "Darknet Gateway", "NSA Office",
	PM_CRACKER, NON_PM, PM_UNDEAD_CRACKER, NON_PM, PM_SOFTWARE_BUG,
	PM_YHELOTHAR, PM_HACKER, PM_E_TYPE, PM_W_TYPE,
	PM_DEPHMAH_SEPHIRAH, PM_RNGED_KOP, S_KOP, S_HUMAN,

	ART_ROTATING_THROUGH, /* better iron chain */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 25, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(20), 20, 18, 18, 18, 16 },
	/* Init   Lower  Higher */
	{  15, 0,  0, 4,  3, 0 },	/* Hit points */
	{  12, 0,  0, 5,  0, 2 }, 12,	/* Energy */
	-10, 6, 1, 4, 12, A_INT, SPE_GAIN_SPACT,      -4, 100
},

{	{"Cruel Abuser", 0}, { /* too many role names starting with A, so I added that adjective --Amy */
	{"Whipper",     0},
	{"Lasher",   0},
	{"Sadist",0},
	{"Kinky Fucker",0},
	{"Punisher",    0},
	{"Riding Crop Lover",   0},
	{"BDSM Fetishist",     0},
	{"Painmaker",    0},
	{"Grey wannabe",  0} },
	"Jacob Black", "_Bella Swan", "Edward Cullen",	/* Twilight */
	"Abu", "Red Room of Death", "Little Miss Steele's Apartment",
	PM_CRUEL_ABUSER, NON_PM, PM_UNDEAD_CRUEL_ABUSER, NON_PM, PM_CRUEL_ABUSER,
	PM_CHRISTIAN_GRAY, PM_INMATE, PM_ANASTASIA_STEELE, PM_WHINY_MARY_SUE,
	PM_OFFICER, PM_KOP_KCHIEF, S_KOP, S_HUMAN,
	ART_PAINKILLER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 8, 18, 19, 18 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	-10, 5, 0, 2, 10, A_INT, SPE_FORCE_BOLT,   -4, 40
},

{	{"Cyberninja", 0}, { /* ZAPM ninja */
	{"Eavesdropper",   0},
	{"Looming Shadow",    0},
	{"Infiltrator",      0},
	{"Sabotager",     0},
	{"Terrorist",      0},
	{"Corporate Spy",      0},
	{"Black Hand",      0},
	{"Cyber Assassin",      0},
	{"Unseen Master",   0} },
	"Diddy the Fail Master", "Bantor the Gay Voice", "Conker the Bad Fur Squirrel", /* Diddy Kong Failing :P */
	"Cyb", "Teki-Owned Castle", "Rice Plant Fields",
	PM_CYBERNINJA, NON_PM, PM_UNDEAD_CYBERNINJA, NON_PM, PM_DENRYU,
	PM_PRINCE_SARTOR, PM_ROSHI, PM_FENG_SHAO, PM_ONA_KENUGI,
	PM_VARUNA, PM_KILL_WING, S_GREMLIN, S_GREMLIN,

	ART_GAYSECT, /* 2h axe with bisection attack */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 10,  8, 30, 14,  8 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 17, 19, 20, 18, 19 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  1, 0 },	/* Hit points */
	{  7, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_DEFOG,      -4, 25
},

{	{"Dancer", 0}, {
	{"Swing Student", 0},
	{"Toe Stepper", 0},
	{"Spinner", 0},
	{"Gymnast", 0},
	{"Dancehall Artist", 0},
	{"Sindancer", 0},
	{"Souldancer", 0},
	{"Lifedancer", 0},
	{"World-Class Dancer", 0} },
	"_the Archomentals", "Cryonax", "_the Elder Elemental",
	"Dan", "the Seasonal Festival Grounds", "the Elemental Confluence",
	PM_DANCER, NON_PM, PM_UNDEAD_DANCER, NON_PM, PM_LITTLE_DOG,
	PM_TROUPE_LEADER, PM_BACKUP_DANCER, PM_BOSS_TIME_ELEMENTAL, PM_FORGOTTEN_REALMS_VORTEX,
	PM_THRIAE, PM_GREMLIN, S_GREMLIN, S_ELEMENTAL,

	ART_SOFTSTEP, /* anastasia shoes */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 20, 10, 15, 30, 10, 20 },
	/* new limit values for the six attributes by Amy */
	{ 18, 18, 20, 25, 16, 25 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 2,  0, 2,  0, 2 }, 8,	/* Energy */
	8, 5, -2,
	2, 30, A_WIS, SPE_SLOW_MONSTER,	-4, 50
},

{	{"Death Eater", 0}, {
	{"Filthy Worm",  0},
	{"Creeping Maggot", 0},
	{"Lowly Being", 0},
	{"Black Mage", 0},
	{"Curse Caster",0},
	{"Imperious", 0},
	{"Crucious", 0},
	{"Deathbringer", 0},
	{"Dark Lord's Bodyguard", 0} },
	"Merlin", "Salazar Slytherin", "Lord Voldemort", /* Harry Potter */
	"Dea", "Malfoy Manor", "the Order of the Phoenix",
	PM_DEATH_EATER, NON_PM, PM_UNDEAD_DEATH_EATER, NON_PM, PM_SESSILICHEN,
	PM_BELLATRIX_LESTRANGE, PM_FELLOW_DEATH_EATER, PM_HARRY_POTTER, PM_PARRY_HOTTER,
	PM_AUROR, PM_ELVEN_MASTER_ROGUE, S_ANGEL, S_HUMAN,
	ART_ELDER_STAFF,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  16, 20, 12, 16, 14, 8 },
	/* Init   Lower  Higher */
	{ 17, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_FINGER_OF_DEATH,   -4, 200
},

{	{"Demagogue", 0}, {
	{"Assumption Spreader",   0},
	{"Big Lips",    0},
	{"Loud Agitator",      0},
	{"Tally Heretic",     0},
	{"Mass Rallier",      0},
	{"Mob Mobilizer",      0},
	{"Politic Revolutionizer",      0},
	{"State Usurper",      0},
	{"King of Fake News",   0} },
	"Heimskr", "_Suffragette", "Cromwell", /* annoying NPCs in certain video games */
	"Dem", "Fake News Office", "Recursion Office",
	PM_DEMAGOGUE, NON_PM, PM_UNDEAD_DEMAGOGUE, NON_PM, PM_THALMOR,
	PM_SINISTER_PRESIDENT, PM_PAGE, PM_MISTER_NAYSAYER, PM_SHROUDED_MIND,
	PM_GREYLEVEL_KOP, PM_BLACK_KOPETEER, S_KOP, S_ANGEL,

	ART_MESSAGE_MEGAPHONE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20,  5,  5, 20, 30, 20 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 10, 10, 18, 19, 18 },
	/* Init   Lower  Higher */
	{  20, 0,  0, 8,  4, 0 },	/* Hit points */
	{  1,  0,  0, 1,  0, 1 }, 18,	/* Energy */
	-50, 6, 4, 7, 15, A_INT, SPE_MESSAGE,      -4, 30
},

{	{"Diablist", 0}, {
	{"Stocks That Hunter Inside Dweller",         0},
	{"Cemetery Plunderer",         0},
	{"Catacomb Evil-Fighter",         0},
	{"Desert Caravanist",         0},
	{"Tomb Comber",         0},
	{"Jungle Stinger",         0},
	{"Temple Cleanser",         0},
	{"Heavenly Fortress Defender",         0},
	{"Diablobane",  0} },
	"Athulua", "Bul-Kathos", "Rathma", /* Diablo series */
	"Dia", "Pandemonium Fortress", "Chaos Sanctuary",
	PM_DIABLIST, NON_PM, PM_UNDEAD_DIABLIST, NON_PM, PM_ROGUE_HIRELING,
	PM_TYRAEL, PM_SUPER_WARRIOR, PM_THE_THICKNESSES, PM_DIPPEL,
	PM_STYGIAN_HAG, PM_STORM_CALLER, S_DOG, S_ELEMENTAL,

	ART_HAMMER_OF_HELLFORGE, /* mallet */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  17, 17, 16, 17, 17, 16 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 25, 0,  0, 1,  1, 0 },	/* Hit points */
	{  8, 1,  0, 1,  0, 1 }, 15,	/* Energy */
	0, 8, 0, 
	0, 0, A_INT, SPE_FIRE_GOLEM,        -4, 100 /* From old role.c */
},

{	{"Dissident", 0}, {
	{"Mars Castaway",   "Moon Base Staff"},
	{"Orbital Watch",    0},
	{"Land Agent",      0},
	{"Covert Eye",     0},
	{"Arms Dealer",      0},
	{"Belt Capo",      0},
	{"Planet Conqueror",      0},
	{"System Kingpin",      0},
	{"Sector Crimelord",   0} },
	"_Aung San Suu Kyi", "Ai Weiwei", "Vladimir Bukovsky", /* famous dissidents */
	"Dis", "Secret Underground", "Gavenmengte Location",
	PM_DISSIDENT, NON_PM, PM_UNDEAD_DISSIDENT, NON_PM, PM_CHRISTINE,
	PM_DARKNET_GATER, PM_EXTERMINATOR, PM_SCHWING_JI, PM_DICTATORIAL_PRESIDENT,
	PM_KOPKEEPER, PM_GOVERNMENT_FLUNKIE, S_KOP, S_ANGEL,

	ART_VOICESHUTTER, /* hammer */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 15, 15, 20, 15 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 19, 19 },
	/* Init   Lower  Higher */
	{  20, 0,  0, 4,  4, 0 },	/* Hit points */
	{   8, 0,  0, 2,  0, 3 }, 16,	/* Energy */
	10, 9, -1, 3, 12, A_INT, SPE_NEXUSPORT,      -4, 50
},

{	{"Diver", 0}, {
	{"Beachcomber",  0},
	{"Paddler",        	0},
	{"Snorkeler",   0},
	{"Swimmer",      		0},
	{"Wave-rider",     0},
	{"Cruiser",      0},
	{"Surfer",			0},
	{"Spearfisher",   	0},
	{"Torpedo",  	0} },
	"Tane", "Tangaroa", "Whiro",	/* Polynesian */
	"Div", "Scuba Dock", "Melville's Sea of Doom",
	PM_DIVER, NON_PM, PM_UNDEAD_DIVER, NON_PM, NON_PM,
	PM_JACQUES_COUSTEAU, PM_ATTENDANT, PM_MOBY_DICK, PM_MOBY_DIGGER,
	PM_BLUE_WHALE, PM_TENTACRUEL, S_EEL, S_EEL,
	ART_MAUI_S_FISHHOOK,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   8,  7, 7,  8,  8,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 10, 20, 25, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(60), 18, 17, 19, 16, 19 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 8, 0, 2,  9, A_INT, SPE_DETECT_MONSTERS,    -4, 40
},

{	{"Doll Mistress", 0}, {
	{"Puppet Trainer",    0},
	{"Yin Seal Thrower",    0},
	{"Spell Capturer",    0},
	{"Magic Puppeteer",    0},
	{"Death Forest Survivor",    0},
	{"Mount Fucking Moon Hiker",    0},
	{"Gates of Hell Visitor",    0},
	{"Puppet League Participant",    0},
	{"Puppet League Champion",    0}},
	"_Sakuya", "_Reimu", "_Yukari Yakumo", /* Touhou */
	"Dol", "Touhou Pokemon Center", "Scarlet's Concentration Camp",
	PM_DOLL_MISTRESS, NON_PM, PM_UNDEAD_DOLL_MISTRESS, NON_PM, PM_GOODWIFE,
	PM_TENSHI_AND_MEILING, PM_ATTENDANT, PM_FLANDRE_SCARLET, PM_SCARLET_DEVIL,
	PM_ASIAN_LADY, PM_TUFTED_ASIAN_GIRL, S_GOLEM, S_GOLEM,
	ART_MASTER_BALL,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 20, 10, 30, 10, 20 },
	/* new limit values for the six attributes by Amy */
	{  18, 20, 18, 21, 16, 20 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 6,  1, 0 },	/* Hit points */
	{  4, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_STONE_TO_FLESH,   -4, 50
},

{	{"Doom Marine", 0}, {
	{"Mars Marine",     0},
	{"Demon Blaster",     0},
	{"Gun-Toting Avenger",     0},
	{"Kalashnikov Gunner",     0},
	{"BFG Wielder",     0},
	{"Spiderdemon Hunter",     0},
	{"Nether Realm Traveler",     0},
	{"Hell Gate Closer",     0},
	{"Cyberdemon's Bane",     0}},
	"Zombieman Brigade", "The Revenants", "Cyberdemon MkIII Deluxe", /* Doom 2 */
	"Mar", "Martian Base", "Icon of Sin",
	PM_DOOM_MARINE, NON_PM, PM_UNDEAD_DOOM_MARINE, NON_PM, PM_SOLDIER,
	PM_LAST_SURVIVING_OTHER_HUMAN, PM_SUPER_WARRIOR, PM_JOHN_ROMERO, PM_EYGOORTS_TOGAAL_JEZEHH,
	PM_HERRER, PM_ROCKET_FATGUY, S_DEMON, S_DEMON,
	ART_BRUTAL_CHAINSAW,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 4, 3, 30, 30, 3 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 13, 12, 16, 20, 12 },
	/* Init   Lower  Higher */
	{  15, 0,  0, 10,  0, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },15,	/* Energy */
	10, 10, -4, 6, 18, A_INT, SPE_TELEPORT_AWAY,   -4, 10
},


{	{"DQ Slime", 0}, {
	{"Globule",     0},
	{"Moist",    0},
	{"Wiggly",       0},
	{"Jiggly",    0},
	{"Slurry",      0},
	{"Slime Knight",     0},
	{"Slime Hero",     0},
	{"King Slime",     "Queen Slime"},
	{"Slime God",       "Slime Goddess"} },
	"King Somnus", "_Rubiss", "Malroth", /* Dragon Warrior */
	"Sli", "The Slime Paradise", "The village of Humbleton",
	PM_DQ_SLIME, NON_PM, PM_UNDEAD_DQ_SLIME, NON_PM, NON_PM,
	PM_SLIME_EMPEROR, PM_GLOBULE, PM_LEGENDARY_HERO, PM_PURGATORY_DUDE,
	PM_ROYAL_GUARDIAN, PM_DWARF_KING, S_HUMAN, S_HUMANOID,

	ART_SLIME_CROWN,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7,  7, 10,  7,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 10, 10, 30, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 20, 18, 16, 12 },
	/* Init   Lower  Higher */
	{  5, 0,  0, 2,  8, 0 },	/* Hit points */
	{  1, 0,  0, 2,  0, 8 },15,	/* Energy */
	-20, 8, 0, 6,  14, A_INT, SPE_ACID_STREAM, -4, 60
},

{	{ "Dragonmaster", 0 }, {
	{ "Servant of Scales", 0 },
	{ "Page of Wings", 0 },
	{ "Page of Claws", 0 },
	{ "Page of Scales", 0 },
	{ "Knight of the Skies", 0 },
	{ "Knight of Talons", 0 },
	{ "Knight of Scales", 0 },
	{ "Knight of Power", 0 },
	{ "Knight of Breath", 0 } },
	"Enki", "Enlil", "_Ereshkigal", /* Sumerian */
	"Dra", "the Sacred Nesting Grounds", "the Draconic Boneyard",
	PM_DRAGONMASTER, NON_PM, PM_UNDEAD_DRAGONMASTER, NON_PM, PM_BABY_RED_DRAGON,
	PM_DRAKEMOTHER, PM_DRAKE, PM_SLIPSCALE_THE_BETRAYER, PM_ORDRAGON,
	PM_VAMPIRE, PM_WRAITH, S_ZOMBIE, S_VAMPIRE,
	ART_SHARUR,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 30, 15, 15, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 19, 18, 15, 17, 15 },
	/* Init   Lower  Higher */
	{ 14, 0, 0, 8, 2, 0 }, /* Hit points */
	{  1, 4, 0, 1, 0, 2 },10, /* Energy */
	10, 8, -2,
	0, 9, A_INT, SPE_FIREBALL, -4, 30
},

{	{"Druid", 0}, {
	{"Fire Tower Caster",    0},
	{"Rock Melter",    0},
	{"Eruption Magician",    0},
	{"Windchiller",    0},
	{"Tornado Chief",    0},
	{"Volcano Prince",    0},
	{"Causer of Hurricanes",    0},
	{"Armageddon Mage",    0},
	{"Thundering Earthshaker",      0} },
	"Tal Rascha", "_Blood Raven", "B-a-a-l", /* Diablo 2 */
	"Drd", "Scosglen Forests", "the place where everyone dies",
	PM_DRUID, NON_PM, PM_UNDEAD_DRUID, NON_PM, PM_RAVEN,
	PM_HALBU, PM_HUNTER, PM_HEFFER, PM_THE_SMITH,
	PM_MOVED_SHAMAN, PM_VOLCANIC_ELEMENTAL, S_KOBOLD, S_ELEMENTAL,

	ART_HEFFER_S_BOW,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  8,  7, 10, 17,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15,  8, 25, 14, 18 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 16, 14, 10, 16 },
	/* Init   Lower  Higher */
	{ 8, 0,  0, 1,  1, 0 },	/* Hit points */
	{ 12, 0,  0, 3,  0, 4 },11,	/* Energy */
	10, 7, -3, 2,  9, A_INT, SPE_CREATE_FAMILIAR,    -4, 90
},

{	{"Drunk", 0}, {
	{"Drinker",     0},
	{"Boozer",   0},
	{"Alcoholic",0},
	{"Winetester",0},
	{"Vodkaholic",    0},
	{"Scrapper",   0},
	{"Bar-Brawler",     0},
	{"Beer King",    "Beer Queen"},
	{"Ethanol Addict",  0} },
	"Apollo", "Dionysus", "Pan", /* Drunken */
	"Dru", "Brass Lantern Bar", "Moriarty's Saloon",
	PM_DRUNK, NON_PM, PM_UNDEAD_DRUNK, NON_PM, NON_PM,
	PM_ANDY_STAHL, PM_ATTENDANT, PM_COLIN_MORIARTY, PM_GREEDY_SCOTTISH_BARKEEPER,
	PM_GNOME_WARRIOR, PM_DWARF_KING, S_HUMAN, S_GNOME,
	ART_CLOAK_OF_NEUTRALITY,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 16, 15, 18, 12 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 5, 0, 2, 10, A_INT, SPE_DETECT_FOOD,   -4, 30
},

{	{"Dunmer", 0}, {
	{"Radical Elf",      0},
	{"one who doesn't like foreigners",      0},
	{"S'wit Remover",      0},
	{"N'wah Basher",      0},
	{"causer of suffering",      0},
	{"Merciless Assassin",      0},
	{"Morag Tong member",      0},
	{"Morag Tong leader",      0},
	{"Morag Tong Master Assassin",      0} },
	"Orome", "_Yavanna", "Tulkas", /* Elven */

	"Dun", "Gnisis", "Red Mountain",
	PM_DUNMER, NON_PM, PM_UNDEAD_DUNMER, NON_PM, PM_ZUBAT,
	PM_VIVEC, PM_HIGHEST_ELF, PM_DAGOTH_UR, PM_MEHRUNES_DAGON,
	PM_OBLIVION_DAEDRA, PM_ASH_CLANNFEAR, S_NEMESE, S_NEMESE,

	ART_N_WAH_KILLER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  25, 15, 15, 20, 20, 5 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 17, 14, 18, 16, 16 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  8, 0,  0, 2,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_CANCELLATION,   -4, 40
},

{	{"Electric Mage", 0}, {
	{"Charger",         0},
	{"Bolter",       0},
	{"Buzzer",       0},
	{"Hummer",     0},
	{"Energic",       0},
	{"Arclite",      0},
	{"Volt Technician",   0},
	{"Thundermage", 0},
	{"Shock-Master",  0} },
	"Thomas Alva Edison", "Benjamin Franklin", "_Marilyn Monroe", /* associated with electricity */
	"Ele", "Bolttown", "Celebrity Graveyard",
	PM_ELECTRIC_MAGE, NON_PM, PM_UNDEAD_ELECTRIC_MAGE, NON_PM, PM_SHOCK_HOUND_PUP,
	PM_GUITAR_HERO, PM_SHOCKER, PM_ELVIS_PRESLEY, PM_ROCK_N_ROLL_KING,
	PM_SHOGGOTH, PM_BURBLING_BLOB, S_PUDDING, S_BLOB,
	ART_THUNDER_WHISTLE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  16, 20, 16, 16, 15, 14 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_LIGHTNING,        -4, 100 /* From old role.c */
},

{	{"Elementalist", 0}, {
	{"Fire Fingers",         0},
	{"Icicle Hands",         0},
	{"Air Current Lover",         0},
	{"Earth Digger",         0},
	{"Inferno Strider",         0},
	{"Hurricane Center",         0},
	{"Glacial Avalanche",         0},
	{"Migmatite Carver",         0},
	{"Astral Controller",  0} },
	"Amon Ra", "Hersifon", "Mor Havoc", /* Age of Empires */
	"Elm", "Greenery Grove", "Astral Depths",
	PM_ELEMENTALIST, NON_PM, PM_UNDEAD_ELEMENTALIST, NON_PM, PM_FRY,
	PM_LAURA_THE_ELEMENTAL_LADY, PM_ATTENDANT, PM_LAHIRA_THE_EVIL_ELEMENTALIST, PM_WATER_CONTROL_GIRL,
	PM_THEMATIC_ASTRAL_ELEMENTAL, PM_LAURA_S_HUGGING_BOOT, S_ELEMENTAL, S_ELEMENTAL,
	ART_BEAM_MULTIPLIER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{   5, 25, 20, 25, 15, 10 },
	/* new limit values for the six attributes by Amy */
	{  18, 18, 18, 18, 16, 17 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 4,  2, 0 },	/* Hit points */
	{ 20, 5,  0, 3,  0, 1 },13,	/* Energy */
	10, 0, -2, 
	5, 20, A_INT, SPE_CALL_THE_ELEMENTS,        -4, 100 /* From old role.c */
},

{	{"Elph", 0}, {
	{"Edhel",       "Elleth"},
	{"Edhel",       "Elleth"},      /* elf-maid */
	{"Ohtar",       "Ohtie"},       /* warrior */
	{"Kano",			/* commander (Q.) ['a] */
			"Kanie"},	/* educated guess, until further research- SAC */
	{"Arandur",			/* king's servant, minister (Q.) - guess */
			"Aranduriel"},	/* educated guess */
	{"Hir",         "Hiril"},       /* lord, lady (S.) ['ir] */
	{"Aredhel",     "Arwen"},       /* noble elf, maiden (S.) */
	{"Ernil",       "Elentariel"},  /* prince (S.), elf-maiden (Q.) */
	{"Elentar",     "Elentari"} },	/* Star-king, -queen (Q.) */
	"Solonor Thelandira", "Aerdrie Faenya", "Erevan Ilesere", /* Elven */

	"Elp", "the great Circle of Elwing", "the Goblins' Cave",
	PM_ELPH, NON_PM, PM_UNDEAD_ELPH, NON_PM, PM_LITTLE_DOG, /* Orion & canis major */
	PM_EARENDIL, PM_HIGHEST_ELF, PM_GOBLIN_KING, PM_GOBLIN_SUB_KING,
	PM_URUK_HAI, PM_OGRE, S_ORC, S_OGRE,

	ART_PALANTIR_OF_WESTERNESSE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 10, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  18, 19, 19, 24, 16, 20 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  8, 0,  0, 2,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -4, 60
},

{	{ "Emera", 0 }, {
	{ "Sexhater", 0 },
	{ "Relationship Avoider", 0 },
	{ "Me-Neither-Shouter", 0 },
	{ "Privileged Potato", 0 },
	{ "Butt Resoler", 0 },
	{ "Terrible Dad", 0 },
	{ "Projection Of All That Is Wrong", 0 },
	{ "Militant Emera", 0 },
	{ "Exshooter", 0 } },
	"Slim Jim", "Milwaukee Jon", "Bose Jefferson", /* Road Rash */
	"Eme", "Toxic HQ", "Feminist Center",
	PM_EMERA, NON_PM, PM_UNDEAD_EMERA, NON_PM, PM_EMERA,
	PM_VERNON_DURSLEY, PM_FELLOW_DEATH_EATER, PM_SANITA_EKERENZE, PM_FLOEY_STINT,
	PM_HC_SEX_PRINCESS, PM_FLAX_WOMAN, S_ARCHFIEND, S_ANGEL,
	ART_MEANCANE, /* walking stick */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 30,  5,  5, 10, 45,  5 },
	/* new limit values for the six attributes by Amy */
	{ STR19(20), 8, 6, 15, 20, 5 },
	/* Init   Lower  Higher */
	{ 25, 0, 0, 10, 1, 0 }, /* Hit points */
	{  2, 1, 0, 1, 0, 1 },8, /* Energy */
	-20, 16, 5,
	20, 20, A_INT, SPE_SHINING_WAVE, -4, 10
},

{	{"Empath", 0}, {
	{"Enlightened One",   0},
	{"Mind Mirror",    0},
	{"Thought Catcher",      0},
	{"Subconscious Link",     0},
	{"Psionic Vein",      0},
	{"Brainwave Warper",      0},
	{"Neuroconductor",      0},
	{"Mind Overlord",      0},
	{"Fate Weaver",   0} },
	"_Grolla Seyfarth", "_Pamela & Carl Arwig", "_Iris Sepperin", /* erka_es, and the "Carl Arwig" is an inside joke */
	"Emp", "Magnetic Shield Cave", "Antenna Grounds",
	PM_EMPATH, NON_PM, PM_UNDEAD_EMPATH, NON_PM, PM_NURSE,
	PM_HYACINTOR, PM_INTERN, PM_BRANTER_GUN, PM_RELIGIOUS_HOUSE_NOTICER,
	PM_MIND_FLAYER, PM_SWAPPED_MASTER_MIND_FLAYER, S_HUMANOID, S_HUMANOID,

	ART_CONTROLLER_HAT, /* helm of telepathy, invoke for taming */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 20, 20,  5, 15 },
	/* new limit values for the six attributes by Amy */
	{  18, 25, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  10, 0,  0, 3,  2, 0 },	/* Hit points */
	{  24, 0,  0, 5,  0, 3 }, 18,	/* Energy */
	10, 6, -5, 3, 13, A_INT, SPE_FULL_HEALING,      -4, 150
},

{	{"Erdrick", 0}, {
	{"Young Boy",     "Young Girl"},
	{"Village Hero",    0},
	{"City Hero",       0},
	{"Island Hero",    0},
	{"Country Hero",      0},
	{"Continent Hero",     0},
	{"World Hero",     0},
	{"Super Hero",     0},
	{"Legend",       0} },
	0, 0, 0, /* special deities */
	"Erd", "Tantagel Castle", "Charlock Castle",
	PM_ERDRICK, NON_PM, PM_UNDEAD_ERDRICK, NON_PM, NON_PM,
	PM_KING_LORIK, PM_ROYAL_GUARD, PM_DRAGONLORD, PM_SUPER_OLD_PURPLE_DRAGON,
	PM_SLIME, PM_DRAKEE, S_BLOB, S_BAT,

	ART_BALL_OF_LIGHT,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7,  7, 10,  7,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 10, 10, 30, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 17, 17, 18, 16 },
	/* Init   Lower  Higher */
	{  7, 0,  0, 2,  1, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 1 },16,	/* Energy */
	0, 8, 0, 3,  11, A_INT, SPE_LIGHTNING, -4, 30
},

{	{"Failed Existence", 0}, {
	{"Looser",    0},
	{"Failure",       0},
	{"Bum",   0},
	{"Torn Person", 0},
	{"Chainsmoker",         0},
	{"Walking Beer Bottle",    0},
	{"Limping Tramp",        0},
	{"Luckless Individual",  0},
	{"Choosy Beggar",      0} },
	0, 0, 0, /* special deities */
	"Fai", "Red Carpet", "Violet Carpet",
	PM_FAILED_EXISTENCE, NON_PM, PM_UNDEAD_FAILED_EXISTENCE, NON_PM, NON_PM,
	PM_GRUFF_CO_WORKER, PM_MODEL, PM_IRINA, PM_MODEL_MUMMY,
	PM_UNDEAD_ACTIVISTOR, PM_UNDEAD_FEAT_MASTER, S_HUMAN, S_HUMAN,
	ART_UNOBTAINABLE_BEAUTIES,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 25, 15, 15, 10, 25 },
	/* new limit values for the six attributes by Amy */
	{  12, 16, 16, 21, 14, 25 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 4,  2, 0 },	/* Hit points */
	{  8, 0,  0, 1,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_FORBIDDEN_KNOWLEDGE,   -4, 90
},

{	{"Feat Master", 0}, { /* stunt master, but there were too many role names beginning with S */
	{"Hopper",     0},
	{"Jumper",     0},
	{"Stunter",     0},
	{"Flyer",     0},
	{"Airdasher",     0},
	{"Warpjumper",     0},
	{"Implacable Protagonist",     0},
	{"Main Attraction",     0},
	{"Movie Star",     0}},
	"James Bond", "Chuck Norris", "Jackie Chan", /* movie heroes */
	"Stu", "Recording Studio", "Noob Studio",
	PM_FEAT_MASTER, NON_PM, PM_UNDEAD_FEAT_MASTER, NON_PM, PM_ANTIMATTER_VORTEX,
	PM_STUNT_DOUBLE, PM_PAGE, PM_RON_WEASLEY, PM_HERMIONE_GRANGER,
	PM_FLYING_GUY, PM_HERMIONE_GRANGER, S_HUMAN, S_NEMESE,

	ART_COAT_OF_STYLE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13,  7, 14,  8, 10, 17 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  5,  5,  5, 60, 5, 20 },
	/* new limit values for the six attributes by Amy */
	{  17, 16, 12, 25, 16, 22 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  2, 0 },	/* Hit points */
	{ 10, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_JUMPING,     -4, 50
},

{	{"Feminist", 0}, {
	{"Rebel Woman",    0},
	{"Wannabe Maneater",    0},
	{"Fingernail Scratcher",    0},
	{"Purple Dress Wearer",    0},
	{"Well-known SJW",    0},
	{"Safe Zone Defender",    0},
	{"Nutsmasher",    0},
	{"Rolling Pin Swinger",    0},
	{"Bane of Men Worldwide",      0} },
	"_Tinanya", "_Helen", "_Rejda",
	"Fem", "Designated Safe Zone", "Men-Infested Caverns",
	PM_FEMINIST, NON_PM, PM_UNDEAD_FEMINIST, NON_PM, PM_BLOCK_HEELED_GIRL,
	PM_FEMMY, PM_NYMPHET, PM_DR__FAUST__THE_PATRIARCH, PM_WIFEBEATER_ASSHOLE,
	PM_POWERFUL_IDIOT, PM_PATRIARCH, S_CENTAUR, S_GIANT,
	ART_GENDER_INSPECIFIC_WHIP,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20,  5,  5, 20, 20, 30 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 12, 14, 19, 18, 21 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 4,  4, 0 },	/* Hit points */
	{ 10, 0,  0, 7,  0, 0 }, 15,	/* Energy */
	0, 6, -4, 2, 7, A_INT, SPE_HORRIFY,   -4, 100
},

{	{"Fencer", 0}, {
	{"Alcor",	0},
	{"Dubhe",	0},
	{"Merak",	0},
	{"Phecda",	0},
	{"Megrez",	0},
	{"Alioth",	0},
	{"Mizar",	0},
	{"Benetnash",	0},
	{"Polaris",	0} },
	"Sol", "_Terra", "_Luna",
	"Fen", "the Starry Rift", "Stella Maris",
	PM_FENCER, NON_PM, PM_UNDEAD_FENCER, NON_PM, PM_ORDINATOR,
	PM_RIGIL, PM_SUPER_WARRIOR, PM_TOLIMAN, PM_FLORELLA,
	PM_AIR_ELEMENTAL, PM_SNAKE, S_SNAKE, S_ZOMBIE,
	ART_FORTUNE_SWORD,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 10, 25, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 15, 16, 21, 18, 19 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 9,  2, 0 },	/* Hit points */
	{ 13, 2,  0, 2,  0, 2 },12,	/* Energy */
	0, 3, -2, 2, 6, A_INT, SPE_ENCHANT_WEAPON,        -4, 25
},

{	{"Fighter", 0}, {
	{"Mercury",	0},
	{"Venus",	0},
	{"Mars",	0},
	{"Jupiter",	0},
	{"Saturn",	0},
	{"Uranus",	0},
	{"Neptune",	0},
	{"Pluto",	0},
	{"Moon",	0} },
	"_Selene", "Helios", "_Eos",
	"Fig", "the Castle of Moon", "the Earth",
	PM_FIGHTER, NON_PM, PM_UNDEAD_FIGHTER, NON_PM, PM_KITTEN,
	PM_PRINCESS_OF_MOON, PM_PLANETARY_FIGHTER, PM_JEDEITE, PM_WEDI_KNIGHT,
	PM_EARTH_ELEMENTAL, PM_SNAKE, S_SNAKE, S_ZOMBIE,
	ART_SILVER_CRYSTAL,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  25, 10, 10, 20, 25, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 16, 21, 18, 19, 19 },
	/* Init   Lower  Higher */
	{ 13, 0,  0, 10,  4, 0 },	/* Hit points */
	{ 4,  0,  0, 1,  0, 1 },16,	/* Energy */
	10, 5, -2, 4, 10, A_INT, SPE_HASTE_SELF,        -4, 25
},

{	{"Firefighter", 0}, {
	{"Hazard Trainee",         0},
	{"Brigade Runner",       0},
	{"Watercannon User",       0},
	{"Extinguisher",     0},
	{"Forest Fire Specialist",       0},
	{"Anti-Flame Fighter",      0},
	{"Fire Leader",   0},
	{"Flash Fire Deleter", 0},
	{"Eternal Firefighter",  0} },
	"Prometheus", "Thor", "Arson", /* Fire */
	"Fir", "Fire-Brigade Base", "Smokey Forest",
	PM_FIREFIGHTER, NON_PM, PM_UNDEAD_FIREFIGHTER, NON_PM, PM_LITTLE_DOG,
	PM_SMOKEY, PM_ATTENDANT, PM_EDDIE_THE_PYRO, PM_TEAM_FORTRESS_PYRO,
	PM_FIRE_ELEMENTAL, PM_SALAMANDER, S_ELEMENTAL, S_LIZARD,

	ART_FIRE_BRIGADE_REEL,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(21), 17, 19, 18, 19, 20 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_TELEPORT_AWAY,        -4, 30 /* From old role.c */
},

{	{ "Fjorde", 0 }, {
	{ "Fishtank Dipper", 0 },
	{ "Nibble Arouser", 0 },
	{ "Pondseeker", 0 },
	{ "Submarine Crew", 0 },
	{ "Flood Lover", 0 },
	{ "Player's Competitor", 0 },
	{ "Deep Biter", 0 },
	{ "Monsterfish Fighter", 0 },
	{ "First Place Challenger", 0 } },
	"Cheep Cheep", "Blubber", "Boss Bass", /* Super Mario */
	"Fjo", "Beginner's Aquarium", "The Self-Refilling Fishtank",
	PM_FJORDE, NON_PM, PM_UNDEAD_FJORDE, NON_PM, PM_FLYING_BLOODFISH,
	PM_UNSEEN_RACE_LEADER, PM_PREP, PM_BUSSBITE__THE_GIANT_FISH, PM_FJORDE_S_WET_DREAM,
	PM_LEWD_KRAKEN, PM_WHITE_WHALE, S_EEL, S_FLYFISH,
	ART_SCOOBA_COOBA, /* cloak of magic resistance that protects items from water and lets you swim in lava */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 25, 20, 10, 20, 15, 10 },
	/* new limit values for the six attributes by Amy */
	{ 18, 15, 16, 22, 20, 18 },
	/* Init   Lower  Higher */
	{ 18, 0, 0, 5, 3, 0 }, /* Hit points */
	{  1, 1, 0, 1, 0, 1 },16, /* Energy */
	10, 9, -1,
	4, 8, A_INT, SPE_SWIMMING, -4, 40
},

{	{"Flame Mage", 0}, {
	{"Spark",         0},   /* WAC was Igniter */
	{"Igniter",       0},
	{"Broiler",       0},   /* WAC was Igniter */
	{"Combuster",     0},   /* WAC was Torcher */
	{"Torcher",       0},
	{"Scorcher",      0},   /* WAC was Torcher */
	{"Incinerator",   0},
	{"Disintegrator", 0},   /* WAC was Incinerator */
	{"Flame-Master",  0} },
	"Earth", "Fire", "Ash", /* Special */
	"Fla", "the great Circle of Flame", "the Water Mage's Cave",
	PM_FLAME_MAGE, NON_PM, PM_UNDEAD_FLAME_MAGE, NON_PM, PM_HELL_HOUND_PUP,
	PM_HIGH_FLAME_MAGE, PM_IGNITER, PM_WATER_MAGE, PM_AIR_SPIRIT,
	PM_WATER_ELEMENTAL, PM_RUST_MONSTER, S_ELEMENTAL, S_RUSTMONST,
#if 0
	ART_FIREWALL, ART_FIRE_BRAND,
#endif
	ART_CANDLE_OF_ETERNAL_FLAME,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  15, 20, 18, 14, 14, 16 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_FIREBALL,        -4, 100 /* From old role.c */
},

{	{"Form Changer", 0}, {
	{"Balanced Doppelganger",    0},
	{"Fluctuated Controller",    0},
	{"Restricted Polymorpher",    0},
	{"Armor Preserver",    0},
	{"Shuddering Vibrator",    0},
	{"Skillful Shifter",    0},
	{"Master of Transmogrifying",    0},
	{"Polyself Hero",    0},
	{"Reality Creator",      0} },
	"Polypiling", "Monster Polymorphing", "Self-Polymorph",
	"For", "Everchanging Automata", "Unmappable Level",
	PM_FORM_CHANGER, NON_PM, PM_UNDEAD_FORM_CHANGER, NON_PM, PM_CHAMELEON,
	PM_JONADAB, PM_SHIFTER, PM_PINOBOT, PM_THERESA_FLAUSCHEBUESCHEL_MAY,
	PM_GENETIC_ENGINEER, PM_NIGHTINGALE, S_QUANTMECH, S_LIZARD,
	ART_EXTRA_CONTROL,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  16, 16, 17, 17, 16, 16 },
	/* new limit values for the six attributes by Amy */
	{  18, 20, 16, 18, 16, 15 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 2,  2, 0 },	/* Hit points */
	{ 30, 0,  0, 10,  0, 5 }, 16,	/* Energy */
	10, 8, 0, 5, 13, A_INT, SPE_POLYMORPH,   -4, 100
},

{	{"Foxhound Agent", 0}, {
	{"Recruit",   0},
	{"Octopus",   0},
	{"Raven",   0},
	{"Mantis",   0},
	{"Wolf",   0},
	{"Fox",   0},
	{"Foxhound",   0},
	{"Big Boss",   0},
	{"Solid Snake",   0}},
	"Colonel Campbell", "Deepthroat", "The Patriots", /* probably some TV show or movie? */
	"Fox", "Special Agents Bureau", "Oberon HQ",
	PM_FOXHOUND_AGENT, NON_PM, PM_UNDEAD_FOXHOUND_AGENT, NON_PM, PM_SOLDIER,
	PM_OTACON, PM_SUPER_WARRIOR, PM_OBERON_PRINCIPAL, PM_OBERON_WORKER,
	PM_BALLISTIC_TURRET, PM_ANTI_ELBERETH_OPERATIVE, S_TURRET, S_RUBMONST,

	ART_PORTCHEST,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 12,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  22,  22, 15, 15, 22,  4 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 16, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 8,  2, 0 },	/* Hit points */
	{  2, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 10,-2, 0,  9, A_WIS, SPE_RESTORE_ABILITY,    -4, 20
},
{	{"Gamer", 0}, {
	{"Tetris Player",    0},
	{"Boulderdash Player",    0},
	{"Super Mario Addict",    0},
	{"FIFA Sports Gamer",    0},
	{"Need For Speed Racer",    0},
	{"owner of a dozen consoles",    0},
	{"Wii Champion",    0},
	{"RROD Defeater",    0},
	{"Playstation Master",    0}},
	"Nintendo", "Microsoft", "Sony", /* gaming console producers */
	"Gam", "Console Hall", "Bowser's Airship",
	PM_GAMER, NON_PM, PM_UNDEAD_GAMER, NON_PM, NON_PM,
	PM_THE_ANGRY_VIDEO_GAME_NERD, PM_HACKER, PM_BOWSER, PM_PRINCESS_KIDNAPPING_DRAGON,
	PM_RED_DRAGON, PM_GREEN_DRAGON, S_DRAGON, S_DRAGON,

	ART_GAME_DISC,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10, 10, 10,  8,  9, 7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  8, 20, 20, 40, 7, 5 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 17, 15, 22, 20, 19 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 20, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_GODMODE,   -4, 75
},

{	{"Gang Scholar", 0}, {
	{"Bullying Victim",    0},
	{"Mobfer",    0}, /* german for "bullying victim" */
	{"Pupil Without Pocket Money",    0},
	{"Bang Gang's Punching Bag",    0},
	{"Hussy Club Doormat",    0},
	{"Teacher Friend",    0},
	{"Victim Turned Executioner",    0},
	{"Headhunter Spokesman",    "Headhunter Spokeswoman"},
	{"One Who Will Defeat Anna",      0} },
	"Mr. Maradonna", "_Ms. Unfortunate Forest", "_Ms. Storm",
	"Sco", "Superschool HQ", "Hussy Club Branch Office",
	PM_GANG_SCHOLAR, NON_PM, PM_UNDEAD_GANG_SCHOLAR, NON_PM, PM_SUPERSCHOOL_SPEAKER,
	PM_HANS_WALT, PM_THUG, PM_HUSSY_JILL, PM_VELCRO_BITCH,
	PM_POLINA, PM_JES_SIKA, S_JELLY, S_KOBOLD,
	ART_METEORIC_AC,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 15, 15, 15, 25, 15 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 5,  5, 0 },	/* Hit points */
	{ 7, 0,  0, 3,  0, 3 }, 14,	/* Energy */
	10, 6, -4, 4, 6, A_INT, SPE_MANA_BOLT,   -4, 100
},

{	{"Gangster", 0}, {
	{"Low Thug",     0},
	{"Pickpocketer",   0},
	{"Street Criminal",0},
	{"Carjacker",0},
	{"Wanted Criminal",    0},
	{"Gang Member",   0},
	{"Mafia Member",     0},
	{"The Don's Right Hand",    0},
	{"Mafia Don",  0} },
	"Claude Speed", "Carl CJ Johnson", "Tommy Vercetti",	/* Grand Theft Auto */
	"Gan", "Gang Headquarters", "the enemy gang's hideout",
	PM_GANGSTER, NON_PM, PM_UNDEAD_GANGSTER, NON_PM, PM_PILE_OF_COPPER_COINS,
	PM_DAVE, PM_FELLOW_GANGSTER, PM_AMY_BLUESCREENOFDEATH, PM_AMY_BSOD,
	PM_CUNTGUN_TROOPER, PM_RIFLEMAN, S_HUMAN, S_HUMAN,
	ART_CHEKHOV_S_GUN,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 10, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	-10, 5, 0, 2, 10, A_INT, SPE_CONFUSE_MONSTER,   -4, 30
},

{	{"Geek", 0}, {
	{"Newbie",    0},
	{"BASIC Programmer",        "BASIC Programmeress"},
	{"C Programmer",        "C Programmeress"},
	{"Hacker",        "Hackeress"},
	{"NetHacker", "NetHackeress"},
	{"Nethack Programmer",      "Nethack Programmeress"},
	{"he who uses",       "she who uses"},
	{"he who knows",      "she who knows"},
	{"he who learns",     "she who learns"} },
	"UNIX", "the PDP-7", "VMS", /* Computerian */
	"Gee", "the Development Team's location", "the Microsoft headquarter",
	PM_GEEK, NON_PM, PM_UNDEAD_GEEK, NON_PM, NON_PM,
	PM_THE_DEV_TEAM, PM_HACKER, PM_BILL_GATES, PM_GILL_BATES,
	PM_BUG, PM_PHANTOM, S_XAN, S_PUDDING,
#if 0
	ART_DELUDER, ART_MIRRORBRIGHT,
#endif
	ART_NETHACK_SOURCES,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10, 10, 10,  8,  9, 7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 15, 15, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(70), 19, 17, 19, 14, 16 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 20, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_CURE_SICKNESS,   -4, 50
},

{	{ "Genderstarist", 0 }, {
	{ "Asterisk User", 0 },
	{ "Language Faker", 0 },
	{ "Speech Policer", 0 },
	{ "Aggressive Advocate", 0 },
	{ "Bank Suer", 0 },
	{ "Placation Striker", 0 },
	{ "Inside Squeezer", 0 },
	{ "Truth Minister", 0 },
	{ "Inclusive Spacer", 0 } },
	"Buergerinnen und Buerger", "Buergerlnnen", "Buerger*innen", /* some weird language that pretends to be German; on some fonts "I" and "l" look the same, therefore the neutral god is spelled like that on purpose :P */
	"Gen", "Speech Police Headquarters", "Frisbitch Radio Station",
	PM_GENDERSTARIST, NON_PM, PM_UNDEAD_GENDERSTARIST, NON_PM, PM_GENDERSTARIST,
	PM_XYZ___, PM_VERY_MINDLESS_THRALL, PM_SPEECH_ASSOCIATION_BOSS, PM_STARCLEANER,
	PM_EMERA, PM_USENET_TROLL, S_WALLMONST, S_TURRET,
	ART_FLAGELLATOR, /* whip-class weapon, 1 in 5000 chance to polymorph you into a M2_NEUTER, doesn't drop upon poly */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 10,  5,  5, 70,  5,  5 },
	/* new limit values for the six attributes by Amy */
	{ 12,  6,  6, 15, 12,  4 },
	/* Init   Lower  Higher */
	{  5, 0, 0, 1, 1, 0 }, /* Hit points */
	{  8, 2, 0, 4, 0, 5 },8, /* Energy */
	-100, 4, 0,
	0, 5, A_INT, SPE_BERSERK, -4, 50
},

{	{"Gladiator", 0}, {
	{"Velite",  0},
	{"Thraex",        	0},
	{"Dimachaerus",   0},
	{"Venator",      		0},
	{"Murmillo",     0},
	{"Hoplomachus",      0},
	{"Retiarius",			0},
	{"Secutor",   	0},
	{"Centurion",  	0} }, /* sorry Fyr, but Champion was just way too generic of a title. --Amy */
	"Sulla", "Cicero", "Catilina",	/* Ancient Rome */
	"Gla", "Arena Bloodworks", "Colosseum",
	PM_GLADIATOR, NON_PM, PM_UNDEAD_GLADIATOR, NON_PM, NON_PM,
	PM_TUTOR, PM_ATTENDANT, PM_THE_CHAMPION_OF_POMPEJI, PM_EMPEROR_NERO,
	PM_WEREPANTHER, PM_MINOTAUR, S_HUMAN, S_QUADRUPED,
	ART_IMPERIAL_TOKEN,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   8,  7, 7,  8,  8,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 10, 20, 25, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 16, 12, 18, 20, 17 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 8,  4, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 8, 0, 2,  9, A_INT, SPE_CAUSE_FEAR,    -4, 20
},

{	{"Goff", 0}, {
	{"Fanfic Reader",    0},
	{"Saint Preps Academy Alumnus",       "Saint Preps Academy Alumna"},
	{"Badfic Writer",   0},
	{"Emo Boy", "Emo Girl"},
	{"Blatant Troll",         0},
	{"Motherfucker",    0},
	{"Author Avatar",        0},
	{"Self-insert",  0},
	{"Marty Stu",      "Mary Sue"} },
	"_B'loody Mary", "_Ebony Dark'ness", "Darth Valer", /* taken from a fanfic */
	"Gof", "Hogwarts Castle", "the forbidden forest", 
	PM_GOFF, NON_PM, PM_UNDEAD_GOFF, NON_PM, NON_PM,
	PM_SNAP_AND_LOOPIN, PM_PREP, PM_TARA_GILESBIE, PM_INADVERTENT_TROLL,
	NON_PM, NON_PM, S_VAMPIRE, S_ZOMBIE,
	ART_GOFFIC_BACKPACK,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  25,  5, 15, 25, 25,  5 },
	/* new limit values for the six attributes by Amy */
	{  14, 17, 12, 15,  8, 18 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 4,  2, 0 },	/* Hit points */
	{  8, 0,  0, 1,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_CURE_HALLUCINATION,   -4, 100
},

{	{"Goldminer", 0}, {
	{"Mining Slave",      0},
	{"Mine Worker",      0},
	{"Pick Wielder",      0},
	{"Stonecutter",      0},
	{"Shaft Climber",      0},
	{"Gold Digger",      0},
	{"Gemstone Hoarder",      0},
	{"Retired Goldrusher",      0},
	{"Made Man",      0} },
	"Aurum", "Argentum", "Antimony",
	"Gol", "The Gnome King's Caverns", "Lareth's Cave",
	PM_GOLDMINER, NON_PM, PM_UNDEAD_GOLDMINER, NON_PM, NON_PM,
	PM_UBER_RUGGO, PM_ATTENDANT, PM_LARETH, PM_GLARETH,
	PM_DROW, PM_OGRE, S_OGRE, S_OGRE,
	ART_PICK_OF_FLANDAL_STEELSKIN,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	  ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
/*	{   7, 10, 10,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 20, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  18, 20, 20, 18, 16, 14 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 4,  1, 0 },	/* Hit points */
	{ 6, 0,  0, 1,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_DIG,   -4, 20
},

{	{"Graduate", 0}, {
	{"Junior Assistant",    0},
	{"Ass Assistant",    0},
	{"Mean Assistant",    0},
	{"College Graduate",    0},
	{"University Traveler",    0},
	{"Junior Professor",    0},
	{"Experimental Scientist",    0},
	{"Lab Leader",    0},
	{"Test Row Developer",    0} }, 
	"Jobs", "Wozniak", "Gates", /* geek */
	"Gra", "university campus", "development hell",
	PM_GRADUATE, NON_PM, PM_UNDEAD_GRADUATE, NON_PM, NON_PM,
	PM_UPPER_GRADUATE, PM_HACKER, PM_STUDENT_LEADER, PM_STUDENT_ON_STRIKE,
	PM_STUDENT_ASSISTANT, PM_GREEN_PROSTITUTE, S_HUMAN, S_HUMAN,

	ART_MASTER_BOOT_DISK,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10, 10, 10,  8,  9, 7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 15, 15, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(70), 19, 17, 19, 14, 16 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 20, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_CURE_HALLUCINATION,   -4, 50
},

{	{"Grenadonin", 0}, {
	{"Colorfucker",   0},
	{"Realism Lover",    0},
	{"Dark Day Worshipper",      0},
	{"Opinion Decrier",     0},
	{"Creative Name Inventor",      0},
	{"Good Godparent",      0},
	{"Bullying Enabler",      0},
	{"Cataclysm Squatter",      0},
	{"Meme Machine",   0} },
	"_Margaret", "_Anaconda", "Aerschie-Miesie", /* Amy's nicknames in Half-Life, AHL and Counter-Strike */
	"Gre", "Pregnancy Den", "Chaotic Message Board",
	PM_GRENADONIN, NON_PM, PM_UNDEAD_GRENADONIN, NON_PM, PM_FEMALE_SAMURAI,
	PM_YOUR_FUCKING_COLOR, PM_UNFORTUNATE_VICTIM, PM_AR_EL_GEE_MASTER, PM_SUN_TORCH,
	PM_AMNESIA_GRUE, PM_PILE_OF_DEATH_COINS, S_PUDDING, S_WORM_TAIL,

	ART_TEAM_FORTRESS_GL,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  8,  7, 10, 17,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 10,  8, 30, 14,  8 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 18, 17, 18, 18 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  1, 0 },	/* Hit points */
	{  7, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_CLAIRVOYANCE,    -4, 20
},

{	{"Gunner", 0}, {
	{"Pistol User",     0},
	{"Revolver User",     0},
	{"SMG User",     0},
	{"Shotgun User",     0},
	{"Assault Rifle User",     0},
	{"Machine-Gunner",     0},
	{"Heavy Machine-Gunner",     0},
	{"Rocketeer",     0},
	{"BFG 9000 Master",     0}},
	0, 0, 0,	/* chosen randomly from among the other roles */
	"Gun", "Sniper Range", "Military Airfield",
	PM_GUNNER, NON_PM, PM_UNDEAD_GUNNER, NON_PM, PM_EVASIVE_SNIPER,
	PM_TOP_GUNNER, PM_SUPER_WARRIOR, PM_YET_ANOTHER_GUN_NUT, PM_HIGHWAY_SHARPSHOOTER,
	PM_FEAROW, PM_SPECTRE, S_BAT, S_GHOST,
	ART_CARBON_NANOTUBE_SUIT,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 16, 12, 20, 18, 16 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	-10, 5, 0, 2, 10, A_INT, SPE_ANTI_DISINTEGRATION,   -4, 10
},

{	{"Half Baked", 0}, {
	{"Half Baker",         0},
	{"Bloody Novice",            0},
	{"No-Hoper",          0},
	{"Pupil of Gravel", 0},
	{"Pupil of Fluids", 0},
	{"Pupil of Trash", 0},
	{"Pupil of Blows",  0},
	{"Pupil of Heat",   0},
	{"Self-Appointed Master",            0} },
	"_Nuwa", "Pangu", "Fuxi", /* Chinese */
	"Hal", "the Monastery of Chan-Sune", "the Monastery of the Earth-Lord",
	PM_HALF_BAKED, NON_PM, PM_UNDEAD_HALF_BAKED, NON_PM, NON_PM,
	PM_GRAND_MASTER, PM_ABBOT, PM_MASTER_KAEN, PM_KASTER_MAEN,
	PM_EARTH_ELEMENTAL, PM_XORN, S_ELEMENTAL, S_XORN,
#if 0
	ART_GAUNTLETS_OF_DEFENSE, ART_WHISPERFEET,
#endif
	ART_EYES_OF_THE_HALF_BAKED_WOR,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  8,  8,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  25, 10, 20, 20, 15, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 20, 18, 14, 18 },
	/* Init   Lower  Higher */
	{ 17, 0,  0, 8,  1, 0 },	/* Hit points */
	{  8, 2,  0, 2,  0, 2 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_RESTORE_ABILITY, -4, 50
},


{	{"Healer", 0}, {
	{"Rhizotomist",    0},
	{"Empiric",        0},
	{"Embalmer",       0},
	{"Dresser",        0},
	{"Medicus ossium", "Medica ossium"},
	{"Herbalist",      0},
	{"Magister",       "Magistra"},
	{"Physician",      0},
	{"Chirurgeon",     0} },
	"_Athena", "Hermes", "Poseidon", /* Greek */
	"Hea", "the Temple of Epidaurus", "the Temple of Coeus",
	PM_HEALER, NON_PM, PM_UNDEAD_HEALER, NON_PM, NON_PM,
	PM_HIPPOCRATES, PM_ATTENDANT, PM_CYCLOPS, PM_PYLOPS,
	PM_GIANT_RAT, PM_SNAKE, S_RODENT, S_YETI,
#if 0
	ART_DELUDER, ART_MIRRORBRIGHT,
#endif
	ART_STAFF_OF_AESCULAPIUS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7, 13,  7, 11, 16 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 20, 20, 15, 25, 5 },
	/* new limit values for the six attributes by Amy */
	{  12, 18, 21, 20, 14, 20 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 20, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_CURE_SICKNESS,   -4, 50
},

{	{"Hedderjedi", 0}, {
	{"Lightsaber Youngling",     0},
	{"Inexperienced Padawan",       0},
	{"Experienced Padawan",       0},
	{"Basic Jedi", 0},
	{"Skilled Jedi", 0},
	{"Expert Jedi",    0},
	{"Master Jedi",    0},
	{"Grandmaster Jedi",      0},
	{"Supreme Master Jedi",    0} },
	"the Light Side", "the Force", "the Dark Side",
	"Hed", "the Jedi Temple", "the Outer Rim",
	PM_HEDDERJEDI, NON_PM, PM_UNDEAD_HEDDERJEDI, NON_PM, PM_LITTLE_DOG,
	PM_THE_JEDI_MASTER, PM_PADAWAN, PM_LORD_SIDIOUS, PM_LORD_INSIDIOUS,
	PM_LASER_STORMTROOPER, PM_STORMTROOPER, S_HUMAN, S_HUMAN,
	ART_DEFINITE_LIGHTSABER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7, 14, 12, 10, 14 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 15, 15, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 20, 20, 18, 20 },
	/* Init   Lower  Higher */
	{ 20, 4,  0, 8,  2, 4 },	/* Hit points */
	{  5, 4,  0, 1,  0, 4 },10,	/* Energy */
	0, 12 , -1, 
	2, 10, A_INT, SPE_CHARGING,    -4, 40
},

{	{"Hussy", 0}, {
	{"Annoying Guy",    "Annoying Girl",},
	{"Nasty Fucker",    "Nasty Bitch",},
	{"Teaser Displeaser",    0},
	{"Perfume Bomb",    0},
	{"Stinking Socks Wearer",    0},
	{"Drum Stint Reluctant",    0},
	{"Odorous Uncle",    "Odorous Aunt"},
	{"Clicker Money Thief",    0},
	{"Anna's Personal Bodyguard",      0} },
	"_Madeleine", "_Sofia", "_Wendy", /* hussy names from pager.c */
	"Hus", "Hussy Club Secret Office", "Walt's Crib",
	PM_HUSSY, NON_PM, PM_UNDEAD_HUSSY, NON_PM, PM_ARVOGENIC_TOPMODEL,
	PM_HUSSY_LARISSA, PM_MODEL, PM_GREAT_CORNER_HARD, PM_POLITICALLY_LEFT_GUENTER,
	PM_SUPERSCHOOL_SPEAKER, PM_TIE_FIGHTER, S_HUMAN, S_DEMON,
	ART_CERULEAN_SMASH,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 15, 10, 15, 25, 20 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 12, 20, 19, 22 },
	/* Init   Lower  Higher */
	{ 10, 0, 0, 5,  4, 0 },	/* Hit points */
	{  6, 0, 0, 3,  0, 4 }, 12,	/* Energy */
	0, 5, 4, 5, 8, A_INT, SPE_DETECT_FOOT,   -4, 100
},

{	{"Ice Mage", 0}, {
	{"Cooler",        0},   /* WAC was Chiller */
	{"Condenser",     0},   /* WAC was Chiller */
	{"Chiller",       0},
	{"Froster",       0},
	{"Permafroster",  0},   /* WAC was Froster */
	{"Icer",          0},   /* WAC was Froster */
	{"Freezer",       0},
	{"Sublimer",      0},   /* WAC was Freezer */
	{"Ice-Master",    0} },
	"Air", "Frost", "Smoke", /* Special */
	"Ice", "the great Ring of Ice", "the Earth Mage's Cave",
	PM_ICE_MAGE, NON_PM, PM_UNDEAD_ICE_MAGE, NON_PM, PM_WINTER_WOLF_CUB,
	PM_HIGH_ICE_MAGE, PM_FROSTER, PM_EARTH_MAGE, PM_DIGGING_MAGE,
	PM_RUST_MONSTER, PM_XORN, S_RUSTMONST, S_XORN,
#if 0
	ART_DEEP_FREEZE, ART_FROST_BRAND,
#endif
	ART_STORM_WHISTLE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  15, 20, 18, 14, 14, 16 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_CONE_OF_COLD,    -4, 100 /* From old role.c */
},

{	{"Intel Scribe", 0}, { /* scribe, but there were too many role names beginning with S */
	{"Red Robe",         0},
	{"Reader",         0},
	{"Scribbler",         0},
	{"Writer",         0},
	{"Permanent Magic Marker",         0},
	{"Portable Ink Container",         0},
	{"Scientific Scribe",         0},
	{"Well-trained Scribe",         0},
	{"Highest Order Scribe",         0}},
	"Bowditch", "Peabody", "Rothchild", /* Fallout 3 */
	"Scr", "Peaceful Quarters", "thief lair",
	PM_INTEL_SCRIBE, NON_PM, PM_UNDEAD_INTEL_SCRIBE, NON_PM, PM_LITTLE_DOG,
	PM_MASTER_LIBRARIAN, PM_ATTENDANT, PM_VENTOS_EFFIGIL, PM_PSEUDO_ELF_THIEF,
	PM_ELVEN_MAGE, PM_ELVEN_PRIEST, S_HUMAN, S_HUMAN,

	ART_TRAINING_SMG,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  17, 20, 22, 19, 15, 18 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_CHARGING,        -4, 100 /* From old role.c */
},

{	{"Janitor", 0}, {
	{"Toilet Scrubber",   0},
	{"Mop Boy",    0},
	{"Housekeeper",      0},
	{"Custodian",     0},
	{"Maintenance Man",      0},
	{"Sanitation Freak",      0},
	{"Superintendent",      0},
	{"Property Manager",      0},
	{"Landlord",   0} },
	"Grime", "Dog Shit", "Fag Butt", /* various forms of garbage - the latter is an actual term for "cigarette", you can stop whining about political correctness */
	"Jan", "Waste Disposal Car Pool", "Smoky Mountain",
	PM_JANITOR, NON_PM, PM_UNDEAD_JANITOR, NON_PM, PM_ROOMBA,
	PM_EVRA_THE_MOP_LADY, PM_SERVANT, PM_VOMITANT_BUM, PM_CIGARETTE_FAGGOT,
	PM_OLOG_HAI, PM_HUDDLY_PUDDING, S_TROLL, S_PUDDING,

	ART_SUPERCLEAN_AWAYWIPER, /* broom */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 15, 15, 20, 20, 15 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 17, 17, 19, 18, 18 },
	/* Init   Lower  Higher */
	{  16, 0,  0, 8,  2, 0 },	/* Hit points */
	{   8, 0,  0, 2,  0, 3 }, 14,	/* Energy */
	10, 8, -1, 3, 10, A_INT, SPE_POLYMORPH,      -4, 75
},

{	{"Jedi", 0}, {
	{"Youngling",     0},
	//{"Padawan",       0},
	{"Padawan",       0},
	//{"Jedi Apprentice", 0},
	{"Jedi Apprentice", 0},
	//{"Jedi Knight",    0},
	{"Jedi Knight",    0},
	{"Jedi Hero",      0},
	{"Jedi Master",    0} },
	"the Light Side", "the Force", "the Dark Side",
	"Jed", "the Jedi Temple", "the Outer Rim",
	PM_JEDI, NON_PM, PM_UNDEAD_JEDI, NON_PM, PM_LITTLE_DOG,
	PM_THE_JEDI_MASTER, PM_PADAWAN, PM_LORD_SIDIOUS, PM_LORD_INSIDIOUS,
	PM_STORMTROOPER, PM_LASER_STORMTROOPER, S_HUMAN, S_HUMAN,
	ART_LIGHTSABER_PROTOTYPE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7, 14, 12, 10, 14 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 15, 15, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 20, 20, 18, 20 },
	/* Init   Lower  Higher */
	{ 20, 4,  0, 8,  2, 4 },	/* Hit points */
	{  5, 4,  0, 1,  0, 4 },10,	/* Energy */
	0, 12 , -1, 
	2, 10, A_INT, SPE_CHARM_MONSTER,    -4, 40
},

{	{"Jester", 0}, {
	{"Little Fool",     0},
	{"Juggler",     0},
	{"Court Clown",     0},
	{"Funny Dude",     "Funny Gal"},
	{"Laughing Muscle",     0},
	{"Lollerskater",     0},
	{"Roflcopter",     0},
	{"Joker",     0},
	{"King's Personal Jester",     0}},
	"Barnum", "Bailey", "Shaco", /* ??? */
	"Jes", "The King's Exile", "Deepwater Shipyard",
	PM_JESTER, NON_PM, PM_UNDEAD_JESTER, NON_PM, PM_PRIEST,
	PM_KING_CHESTER, PM_UNFORTUNATE_VICTIM, PM_LADY_ELLY, PM_GOD_FORSAKEN_BITCH,
	PM_DOPPELGANGER, PM_TIGER, S_QUADRUPED, S_EYE,
	ART_KING_S_STOLEN_CROWN,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  13, 20, 13, 14, 20, 20 },
	/* new limit values for the six attributes by Amy */
	{  18, 14, 12, 20, 16, 19 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 5, 0, 2, 10, A_INT, SPE_DETECT_UNSEEN,   -4, 40
},

{	{"Justice Keeper", 0}, {
	/*[Sakusha]*/
	{"Kageman",	0},
	{"Sonicblastman",	0},
	{"Mustleman",	0},
	{"Wingman",	0},
	{"Eightman",	0},
	{"Gatyaman",	0},
	{"Rainbowman",	0},
	{"Tekkaman",	0},
	{"Ultimate Justice Keeper",	0}},
	"Lawful Good", "True Neutral", "Chaotic Evil",	/* Dungeons and Dragons */
	"Jus", "Trial Court Hall", "Robot Facilities",
	PM_JUSTICE_KEEPER, NON_PM, PM_UNDEAD_JUSTICE_KEEPER, NON_PM, PM_MONKEY,
	PM_BIRDMAN, PM_STARNOID, PM_EVIL_ROBOT, PM_SECURITRON_MKII,
	PM_GREMLIN, PM_TIN_GOLEM, S_GREMLIN, S_GOLEM,
	ART_SUPREME_JUSTICE_KEEPER, /* two-handed sword */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13,  7, 14,  8, 10, 17 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 15, 15, 15, 20, 20 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 16, 19, 18, 18, 17 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 4,  4, 0 },	/* Hit points */
	{ 7, 5,  0, 3,  0, 2 },18,	/* Energy */
	10, 1, -4, 1,  7, A_WIS, SPE_SOLAR_BEAM,     -4, 50
},

{	{"Knight", 0}, {
	{"Gallant",     0},
	{"Esquire",     0},
	{"Bachelor",    0},
	{"Sergeant",    0},
	{"Knight",      0},
	{"Banneret",    0},
	{"Cavalier",   "Cavaliere"},
	{"Seignieur",   "Dame"},
	{"Paladin",     0} },
	"Lugh", "_Brigit", "Manannan Mac Lir", /* Celtic */
	"Kni", "Camelot Castle", "the Isle of Glass",
	PM_KNIGHT, NON_PM, PM_UNDEAD_KNIGHT, NON_PM, PM_PONY,
	PM_KING_ARTHUR, PM_PAGE, PM_IXOTH, PM_XIXOT,
	PM_QUASIT, PM_OCHRE_JELLY, S_IMP, S_JELLY,
#if 0
	ART_DRAGONBANE, ART_DEMONBANE,
#endif
	ART_MAGIC_MIRROR_OF_MERLIN,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13,  7, 14,  8, 10, 17 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 15, 15, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 15, 17, 16, 19, 19 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  2, 0 },	/* Hit points */
	{ 10, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4, 40
},

{	{"Korsair", 0}, {
	{"Lubber",  0},
	{"Swabby",        	0},
	{"Deckhand",   0},
	{"Bilge Rat",      		0},
	{"Helmsman",     "Helmswoman"},
	{"Navigator",      0},
	{"Bosun",			0},
	{"Mate",   	0},
	{"Commodore",  	0} },
	"Erzulie Freda", "Marassa Jumeaux", "Papa Legba",	/* Haitian Vodou */
	"Kor", "Queen Anne's Revenge", "Grotto of Souls",
	PM_KORSAIR, NON_PM, PM_UNDEAD_KORSAIR, NON_PM, NON_PM,
	PM_BLACKBEARD, PM_PIRATE_BROTHER, PM_SPEARATHAN, PM_SPEARFISH_OF_SORROW,
	PM_GIANT_EEL, PM_DRAUGR_ZOMBIE, S_EEL, S_ZOMBIE,
	ART_PEARL_OF_WISDOM,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   8,  7, 7,  8,  8,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 10, 20, 25, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 16, 18, 19, 19 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 8, 0, 2,  9, A_INT, SPE_CAUSE_FEAR,    -4, 30
},

{	{"Kurwa", 0}, {
	{"Cheap Sex Slave",    0},
	{"Kidnapped Beauty",    0},
	{"Sex Market Commodity",    0},
	{"Spread Legs",    0},
	{"STD Lover",    0},
	{"Many-Times-Fucked",    0},
	{"Orgasm Bomber",    0},
	{"Men's Wet Dream",    0},
	{"Highest-Selling Wench",      0} },
	"_Ewa", "Lorskel", "Kaczynski", /* Polish names */
	"Kur", "Hardcore-O-Mania Brothel", "Shadow Government realm",
	PM_KURWA, NON_PM, PM_UNDEAD_KURWA, NON_PM, PM_JENNIFER,
	PM_BIG_D, PM_ATTENDANT, PM_CASH_GREEDY_FINANCE_MINISTER, PM_TAX_A_DRIVER,
	PM_GIANT_MUMAK, PM_INVISIBLE_CC, S_BAT, S_QUADRUPED,
	ART_LORSKEL_S_INTEGRITY,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 5, 15, 20, 25 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 19, 18, 25 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 3,  2, 0 },	/* Hit points */
	{  7, 0,  0, 2,  0, 2 },12,	/* Energy */
	10, 5, -3, 2, 4, A_INT, SPE_MAKE_VISIBLE,   -4, 100
},

{	{"Ladiesman", 0}, {
	{"Wiseguy",    0},
	{"Mirror Looker",    0},
	{"Self-admirer",    0},
	{"Beauty Contest Competitor",    0},
	{"Women's Hero",    0},
	{"Chick Magnet",    0},
	{"Celebrity",    0},
	{"Male Topmodel",    0},
	{"Sexiest Man Alive",    0}},
	"Hugh Hefner", "G-boy", "Arsene Lupin", /* famous people, I guess? */
	"Lad", "The Clubhouse", "dirty neighborhood",
	PM_LADIESMAN, NON_PM, PM_UNDEAD_LADIESMAN, NON_PM, NON_PM,
	PM_MADAM_TEQUILA_THE_CLUBHOUSE_OWNER, PM_MODEL, PM_MR__UNCOOL, PM_DUDE_WHO_THINKS_HE_S_COOL,
	PM_ADULT_GENTLEMAN, PM_FLYING_GUY, S_JELLY, S_PIERCER,
	ART_SLOWNESS_SHIRT,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  12, 30, 4, 12, 12, 30 },
	/* new limit values for the six attributes by Amy */
	{  16, 16, 18, 18, 14, 25 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 4,  2, 0 },	/* Hit points */
	{  8, 0,  0, 1,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_CHARM_MONSTER,   -4, 40
},

{	{"Librarian", 0}, {
	{"Intellectual",    0},
	{"Bestseller Reader",    0},
	{"Bookkeeper",    0},
	{"Bookworm",    0},
	{"Beta Reader",    0},
	{"Keeper of Rare Books",    0},
	{"Keeper of Valuable Books",    0},
	{"Magna Charta Keeper",    0},
	{"Keeper of Unique Books",    0}},
	"Leo Tolstoi", "Stephen Hawking", "H. P. Lovecraft", /* famous writers */
	"Lib", "Great Library", "Demonic Fire Pits",
	PM_LIBRARIAN, NON_PM, PM_UNDEAD_LIBRARIAN, NON_PM, NON_PM,
	PM_MASTER_BOOKKEEPER, PM_SERVANT, PM_CRABBE_AND_GOYLE, PM_TEAM_BULLIES,
	PM_RED_DRAGON, PM_PYROLISK, S_DRAGON, S_COCKATRICE,

	ART_BLOOD_MARKER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10, 10, 10,  8,  9, 7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 30, 10, 10, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 19, 20, 18, 16, 20 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 20, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_CURE_HALLUCINATION,   -4, 200
},

{	{"Locksmith", 0}, {
	{"Key User",     0},
	{"Door-opener",    0},
	{"Lock Breaker",       0},
	{"Lock Trickster",    0},
	{"Safecutter",      0},
	{"Keymaster",     0},
	{"Emergency Key Carrier",     0},
	{"Door Safety Assistant",     0},
	{"Supreme Lockpicker",       0} },
	"Nuada", "Dagda", "Morrigan", /* Celtic */
	"Loc", "Emergency Base", "Locked Room",
	PM_LOCKSMITH, NON_PM, PM_UNDEAD_LOCKSMITH, NON_PM, NON_PM,
	PM_MASTER_OF_LOCKS, PM_THUG, PM_BURGLAR_ED, PM_CRIMINAL_SCUMBAG,
	PM_ELVEN_MASTER_ROGUE, PM_COMING_THIEF, S_HUMAN, S_HUMAN,

	ART_NOCTURNAL_KEY,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7,  7, 10,  7,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 10, 10, 30, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 24, 18, 18 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_KNOCK, -4, 40
},

{	{"Lunatic", 0}, {
	{"Howler",     0},
	{"Crazy Person",   0},
	{"Moon Worshipper",0},
	{"Weirdo",0},
	{"Shapeshifter",    0},
	{"Warped Person",   0},
	{"Wereperson",     0},
	{"Asylum Escapee",    0},
	{"Master Shapeshifter",  0} },
	"Eluvian", "Moon", "Lycanthus",	/* from an old SLASH version without extended magic */
	"Lun", "Wolfhowl Castle", "Slayer Domain",
	PM_LUNATIC, NON_PM, PM_UNDEAD_LUNATIC, NON_PM, NON_PM,
	PM_DRIUD_THE_DRUID, PM_ATTENDANT, PM_BUFFY_THE_VAMPIRE_SLAYER, PM_SIMON_BELMONT,
	PM_SOLDIER, PM_CAPTAIN, S_HUMAN, S_HUMAN,
	ART_VAMPIRE_KILLER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 16, 14, 18, 20, 14 },
	/* Init   Lower  Higher */
	{  18, 0,  0, 8,  6, 0 },	/* Hit points */
	{  5, 0,  0, 3,  0, 3 },10,	/* Energy */
	-10, 5, 0, 2, 10, A_INT, SPE_GODMODE,   -4, 30
},

{	{"Mahou Shoujo", 0}, {
	{"Schoolkid",    0},
	{"Teenie",    0},
	{"Preparing for Exams",    0},
	{"High School Graduate",    0},
	{"Final Year Student",    0},
	{"University Newbie",    0},
	{"Bachelor of Art and Craft",    0},
	{"Jesuitic Studies PhD",    0},
	{"Marriage Counseling Professor",    0}},
	"_Queen Serenity", "_Angel Aphrodite", "_Queen Beryl", /* perhaps an anime or manga? */
	"Mah", "Streets of Tokyo", "Demonic Principal Office",
	PM_MAHOU_SHOUJO, NON_PM, PM_UNDEAD_MAHOU_SHOUJO, NON_PM, PM_KICKBOXING_GIRL,
	PM_CELESTE, PM_MODEL, PM_PETTLER, PM_MARIJA_THE_WENCH,
	PM_LEGION_DEVIL_SERGEANT, PM_LEGION_DEVIL_CAPTAIN, S_DEMON, S_IMP,
	ART_SAINT_SOMETHING_FOUR_CRYST,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  12, 12, 30, 4, 12, 30 },
	/* new limit values for the six attributes by Amy */
	{  STR18(30), 20, 16, 14, 12, 20 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  4, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_LEVITATION,   -4, 100
},

{	{"Mason", 0}, {
	{"Bricklayer",    0},
	{"Wall Construction Worker",    0},
	{"Granite Cementer",    0},
	{"Pavement Optimizer",    0},
	{"Housebuilding Apprentice",    0},
	{"Slate Slabber",    0},
	{"Clay Smelter",    0},
	{"Minecraft Master",    0},
	{"World Trade Center Architect",      0} },
	"The Battlehorse", "The Wild Boar", "The Dire Wolf", /* big mean animals */
	"Mas", "Sport-resounded", "Dangerous Ruin",
	PM_MASON, NON_PM, PM_UNDEAD_MASON, NON_PM, NON_PM,
	PM_BLEDA_S_IRON_BOAR, PM_ATTENDANT, PM_DESTRUCTION_DOZER, PM_SIEGE_CATERPILLAR,
	PM_ARBORACHUCK, PM_CHAOS_HULK, S_RODENT, S_UMBER,
	ART_ULTIMATE_PICK,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 15, 15, 20, 15, 5 },
	/* new limit values for the six attributes by Amy */
	{  STR19(21), 12, 14, 19, 20, 16 },
	/* Init   Lower  Higher */
	{ 14, 0, 0, 6,  3, 0 },	/* Hit points */
	{  1, 0, 0, 2,  0, 2 }, 15,	/* Energy */
	10, 12, 0, 10, 20, A_INT, SPE_LOCKOUT,   -4, 25
},

{	{"Mastermind", 0}, {
	{"Far Herald",   0},
	{"Space Brother",    0},
	{"Cult Idol",      0},
	{"First Contact",     0},
	{"Federation Envoy",      0},
	{"Galactic Senator",      0},
	{"Coadunator",      0},
	{"Uplifter",      0},
	{"Terraformer",   0} },
	"_Marie Curie", "Georg Simon Ohm", "Albert Einstein", /* famous scientists */
	"Mam", "Guessing Corner", "Retardation Center",
	PM_MASTERMIND, NON_PM, PM_UNDEAD_MASTERMIND, NON_PM, PM_ELVEN_MAGE,
	PM_DEEP_BLUE__, PM_GLOBULE, PM_EDWARD_NYGMA, PM_THE_RIDDLER,
	PM_ISLAND_WARP_FUNGUS, PM_MIST_IN_THE_AIR, S_FUNGUS, S_BAD_FOOD,

	ART_RED_PIN, /* probably paddle-class weapon */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 40, 30,  5, 10,  5 },
	/* new limit values for the six attributes by Amy */
	{  16, 25, 24, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 4,  6, 0 },	/* Hit points */
	{  20, 0,  0, 6,  0, 6 }, 20,	/* Energy */
	10, 2, 0, 1, 3, A_INT, SPE_ESP,      -4, 250
},

{	{"Medium", 0}, {
	/*adds male names [Sakusha]*/
	{"Pure Boy",		"Pure Girl"},
	{"Jinme Boy",		"Jinme Girl"},
	{"Mist Youth",		"Mist Maiden"},
	{"Moon Youth",		"Moon Maiden"},
	{"Prince Youth",	"Princess Maiden"},
	{"Hanuman Youth",	"Hanuman Maiden"},
	{"Ryujin Youth",	"Ryujin Maiden"},
	{"Kamiko",		0},
	{"Eternal Youth",	"Eternal Maiden"} },
	"_Umbrellarina Madeline", "_Medium Deborah", "_Crueltrainer Cindy",
	"Med", "the Old Shrine", "the cave of fox-spirits",
	PM_MEDIUM, NON_PM, PM_UNDEAD_MEDIUM, NON_PM, PM_WHITE_UNICORN_FOAL,
	PM_SHINTO_PRIEST, PM_MAIDEN, PM_NINE_TAILED_FOX, PM_JUGEN,
	PM_FOX_SPIRIT, PM_HUMAN_ZOMBIE, S_DOG, S_ZOMBIE,
	ART_HOLYDIRK,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  4, 28, 30, 4, 4, 30 },
	/* new limit values for the six attributes by Amy */
	{  17, 20, 25, 19, 16, 19 },
	/* Init   Lower  Higher */
	{  9, 0,  0, 4,  1, 0 },	/* Hit points */
	{ 25, 4,  0, 5,  0, 7 }, 8,	/* Energy */
	10, 0, -5, 1, 4, A_WIS, SPE_MAP_LEVEL,   -4, 150
},

{	{"Midget", 0}, {
	{"Little Midget",   0},
	{"Small Person",  0},
	{"Middle-Earth Miner",     0},
	{"Cave Inhabitant",     0},
	{"Big Midget",     0},
	{"Giant Midget",     0},
	{"Dwarven Master Smith",     0},
	{"Dwarvish Legend",     0},
	{"Biggest Midget Ever",     0} },
	"Moradin", "Dumathoin", "Abbathor",
	"Mid", "Midget Home", "Lonely Mountain",
	PM_MIDGET, NON_PM, PM_UNDEAD_MIDGET, NON_PM, NON_PM,
	PM_THORIN, PM_DWARF_WARRIOR, PM_SMAUG, PM_SMAUG_THE_ADOM_HURTHLING,
	PM_ORC_CAPTAIN, PM_WARG, S_ORC, S_DOG,
	ART_ARKENSTONE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 12,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  25,  9,  9, 20, 30,  7 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 16, 19, 16, 20, 13 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 7,  2, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 3 },10,	/* Energy */
	0, 9, 0, 0,  6, A_INT, SPE_DIG,    -4, 20
},

{	{ "Mill Swallower", 0 }, {
	{ "Trash Eater", 0 },
	{ "Garbage Can Searcher", 0 },
	{ "Waste Collector", 0 },
	{ "Refuse Picker", 0 },
	{ "True Grimer", 0 },
	{ "Reusable Plastic Bag", 0 },
	{ "Bottle Trader", 0 },
	{ "Truckloader", 0 },
	{ "Ocean Cleaner", 0 } },
	"Indra", "Soma", "Ishwara", /* rigvedic */
	"Mil", "Area Waste Deposit", "Litterhome",
	PM_MILL_SWALLOWER, NON_PM, PM_UNDEAD_MILL_SWALLOWER, NON_PM, PM_TASMANIAN_DEVIL,
	PM_WASTE_DEPUTY, PM_SERVANT, PM_ENVIRONMENT_POLLUTION_OVERLORD, PM_CAUSER_OF_CLIMATE_CHANGE,
	PM_FUN_TROLL, PM_CURSED_COLONY, S_FUNGUS, S_TROLL,
	ART_FERTILIZATOR, /* binning kit that creates random sling ammo whenever it trashes a corpse */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 20, 15, 20, 20, 15, 10 },
	/* new limit values for the six attributes by Amy */
	{ STR18(100), 17, 18, 18, 19, 16 },
	/* Init   Lower  Higher */
	{ 20, 0, 0, 6, 4, 0 }, /* Hit points */
	{  4, 3, 0, 3, 0, 2 },15, /* Energy */
	0, 8, 0,
	8, 13, A_INT, SPE_THRONE_GAMBLE, -4, 50
},

{	{"Monk", 0}, {
	{"Candidate",         0},
	{"Novice",            0},
	{"Initiate",          0},
	{"Student of Stones", 0},
	{"Student of Waters", 0},
	{"Student of Metals", 0},
	{"Student of Winds",  0},
	{"Student of Fire",   0},
	{"Master",            0} },
	"Shan Lai Ching", "Chih Sung-tzu", "Huan Ti", /* Chinese */
	"Mon", "the Monastery of Chan-Sune",
	  "the Monastery of the Earth-Lord",
	PM_MONK, NON_PM, PM_UNDEAD_MONK, NON_PM, NON_PM,
	PM_GRAND_MASTER, PM_ABBOT, PM_MASTER_KAEN, PM_KASTER_MAEN,
	PM_EARTH_ELEMENTAL, PM_XORN, S_ELEMENTAL, S_XORN,
#if 0
	ART_GAUNTLETS_OF_DEFENSE, ART_WHISPERFEET,
#endif
	ART_EYES_OF_THE_OVERWORLD,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  8,  8,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  25, 10, 20, 20, 15, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 20, 18, 14, 18 },
	/* Init   Lower  Higher */
	{ 17, 0,  0, 8,  1, 0 },	/* Hit points */
	{  8, 2,  0, 2,  0, 2 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_RESTORE_ABILITY, -4, 50
},

{	{"Murderer", 0}, {
	{"Knife Killer",     0},
	{"Dangerous Criminal",   0},
	{"Mean Contract",0},
	{"Serial Killer",0},
	{"Cold-Blooded Gangster",    0},
	{"Silent Assassin",   0},
	{"Master Hitman",     "Master Hitwoman"},
	{"Professional Cutter",    0},
	{"Bane of all innocent people",  0} },
	"Falis", "_Marfa", "Falaris",	/* Record of Lodoss War */
	"Mur", "Secret Hiding Place", "Central Cop Station",
	PM_MURDERER, NON_PM, PM_UNDEAD_MURDERER, NON_PM, PM_SEWER_RAT,
	PM_KILLED_BILL, PM_INMATE, PM_UPPER_BULL, PM_ANTI_CRIME_SPECIAL_FORCES_OFFICER,
	PM_KOP_KCHIEF, PM_MACKH_SEPHIRAH, S_KOP, S_KOP,
	ART_PITCH_BLADE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 12, 12, 20, 18, 18 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	-50, 5, 0, 2, 10, A_INT, SPE_DISSOLVE_FOOD,   -4, 20
},

{	{"Musician", 0}, {
	{"Doodler", 0},
	{"Whistler", 0},
	{"Flute Player", 0},
	{"Harp Player", 0},
	{"Orchestrator",	0},
	{"Concertist", 0},
	{"Virtue", 0},
	{"Piano Master", 0},
	{"Godly Voice", 0} },
	"Donblas", "Grome", "Arioch", /* Melnibonean */

	"Mus", "Wild West Bar", "High Noon Plains",
	PM_MUSICIAN, NON_PM, PM_UNDEAD_MUSICIAN, NON_PM, PM_GREEN_ELF,
	PM_HARRY_THE_COWBOY, PM_HIGHEST_ELF, PM_DRACO_THE_SHARPSHOOTER, PM_WILDWEST_GUNMAN,
	PM_ENEMY_TROOPER, PM_OFFICER, S_HUMAN, S_HUMAN,

	ART_BLACKHARP,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 10, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(20), 18, 18, 20, 18, 19 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  8, 0,  0, 2,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_CAUSE_FEAR,   -4, 40
},

{	{"Mystic", 		 0}, {
	{"Insert rank title here",        0},
	{"Insert clever name here",        0},
	{"Insert witty saying here",        0},
	{"Insert random string here",        0},
	{"Insert cool stuff here",        0},
	{"Insert what you want here",        0},
	{"Insert appellation here",        0},
	{"Insert heroic deeds here",        0},
	{"Insert mysterious message here",        0}},
	0, 0, 0,	/* chosen randomly from among the other roles */
	"Mys", "Lichen Land", "Newt Domain",
	PM_MYSTIC, NON_PM, PM_UNDEAD_MYSTIC, NON_PM, PM_GRIMER,
	PM_FWAGGLEBRAPO, PM_STUDENT, PM_YELDUD_TWEN, PM_DUDLEY_S_DUNGEON_TERMINATOR,
	PM_CHARIZARD, PM_CHAOS_LIZARD, S_LIZARD, S_LIZARD,
	ART_MYSTERIOUS_SPIKES,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  6,  6,  6,  6,  6,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  16,  17,  17, 17, 16,  17 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 10,  2, 0 },	/* Hit points */
	{  5, 0,  1, 0,  1, 0 },15,	/* Energy */
	5, 10, 5, 10,  25, A_INT, SPE_DETECT_FOOT,             -4, 50
},


{	{"Necromancer", 0}, {
	{"Gravedigger",  0},
	{"Embalmer", 0},
	{"Mortician", 0},
	{"Zombie Lord", 0},
	{"Ghoul Master",0},
	{"Necromancer", 0},
	{"Necromancer", 0},
	{"Undead Master", 0},
	{"Lich Lord", 0} },
	"Nharlotep", "Zugguthobal", "Gothuulbe", /* Assorted slimy things */
	"Nec", "the Tower of the Dark Lord", "the Lair of Maugneshaagar",
	PM_NECROMANCER, NON_PM, PM_UNDEAD_NECROMANCER, NON_PM, PM_GHOUL,
	PM_DARK_LORD, PM_EMBALMER, PM_MAUGNESHAAGAR, PM_MAUGNESHNAAGAR,
	PM_NUPPERIBO, PM_MONGBAT, S_BAT, S_IMP,
#if 0
	ART_SERPENT_S_TONGUE, ART_GRIMTOOTH,
#endif
	ART_GREAT_DAGGER_OF_GLAURGNAA,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  14, 18, 17, 14, 12, 10 },
	/* Init   Lower  Higher */
	{ 17, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_SUMMON_UNDEAD,   -4, 100
},

{	{"Ninja", 0}, {
	{"Sword Trainee",    0},
	{"Dual-wield wannabee",       0},
	{"Whiffer", 0},
	{"Bladebearer",       0},
	{"Double-edge Sword",      0},
	{"Master Blade",     0},
	{"Katana Knight",      0},
	{"Sword Acrobat",        0},
	{"Shadowblade",      0} },
	"Votishal", "Raiden", "Rat God", /* Nehwon */
	"Nin", "Green Plains", "Demon Orb Lair",
	PM_NINJA, NON_PM, PM_UNDEAD_NINJA, NON_PM, PM_LITTLE_DOG,
	PM_KEN_HAYABUSA, PM_ROSHI, PM_JAQUIO, PM_UBERJAQUIO,
	PM_CARNIVOROUS_APE, PM_NIGHTGAUNT, S_YETI, S_BAT,

	ART_DRAGONCLAN_SWORD,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  8,  7, 10, 17,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 10,  8, 30, 14,  8 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 17, 19, 20, 18, 19 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  1, 0 },	/* Hit points */
	{  7, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_ENCHANT_WEAPON,    -4, 25
},

{	{"Nobleman", "Noblewoman"}, {
	{"Pargar",       0},
	{"Cneaz",	     0},
	{"Ban",		     0},
	{"Jude",	     0},
	{"Boier",	     0},
	{"Cupar",	     0},
	{"Clucer",	     0},
	{"Domn",	     0},
	{"Domnitor",     0} },
	"God the Father", "_Mother Earth", "the Satan", /* Romanian, sorta */
	"Nob", "your ancestral home",
	  "the rebel village",
	PM_NOBLEMAN, PM_NOBLEWOMAN, PM_UNDEAD_NOBLEMAN, PM_UNDEAD_NOBLEWOMAN, PM_PONY,
	PM_OLD_GYPSY_WOMAN, PM_SERVANT, PM_REBEL_RINGLEADER, PM_REBEL_RINGFAILURE,
	PM_SOLDIER, PM_PEASANT, S_HUMANOID, S_HUMAN,
	ART_MANTLE_OF_HEAVEN,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   10,10,  7, 10,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 18, 10, 20, 15, 17 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 16, 19, 18, 17, 22 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 2,  0, 2,  0, 2 },10,	/* Energy */
	10, 4,-4, 4, 8, A_INT, SPE_PROTECTION, -24, 40
},

{	{"Noob Mode Barb", 0}, {
	{"Complete Scrub",   0},
	{"Skill Agnostic",    0},
	{"Whippersnapper",      0},
	{"Whiny Lamer",     0},
	{"Terrible Rookie",      0},
	{"Bad Player",      0},
	{"Mindless Mower",      0},
	{"Filthy Cheater",   0},
	{"Biggest Noob Ever",   0} },
	"P-Point", "I Have To Reload", "Upper Failure Kornop",
	"Noo", "the Camp of the Duali Tribe", "the Duali Oasis",
	PM_NOOB_MODE_BARB, NON_PM, PM_UNDEAD_NOOB_MODE_BARB, NON_PM, NON_PM,
	PM_PELIAS, PM_CHIEFTAIN, PM_THOTH_AMON, PM_THOTH_AMUN,
	PM_OGRE, PM_TROLL, S_OGRE, S_TROLL,
	ART_HEART_OF_OTHER_AHRIMAN,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30,  6,  7, 20, 30,  7 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 6, 8, 6, 20, 6 },
	/* Init   Lower  Higher */
	{ 80, 0,  0,20,  4, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 },10,	/* Energy */
	10, 50, 20, 20,  50, A_INT, SPE_HASTE_SELF,      -4, 5
},

{	{"Nuclear Physicist", 0}, {
	{"Safety Officer",    0},
	{"Burning Stick Controller",    0},
	{"Uranium Handler",    0},
	{"Walking Geiger Counter",    0},
	{"Atomic Counselor",    0},
	{"Ordinance Observer",    0},
	{"HEV Suit Wearer",    0},
	{"Resonance Cascade Averter",    0},
	{"Gordon Freeman Clone",      0} },
	"_Gina", "Gordon Freeman", "G-Man", /* Half-Life */
	"Nuc", "Anomalous Materials Laboratory", "Sector E Propulsion Chamber",
	PM_NUCLEAR_PHYSICIST, NON_PM, PM_UNDEAD_NUCLEAR_PHYSICIST, NON_PM, PM_BRAIN_EATING_GHOUL,
	PM_THESCI, PM_ATTENDANT, PM_INTERNATIONAL_ENERGY_COMMISSIONER, PM_ATOMIC_TESTER,
	PM_SKELETON, PM_GREATER_UGOD, S_UMBER, S_GHOST,
	ART_NUCLEAR_SPEAR,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 35, 20, 15, 15,  5 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 19, 18, 19, 14, 19 },
	/* Init   Lower  Higher */
	{ 9,  0, 0, 4,  1, 0 },	/* Hit points */
	{ 16, 0, 0, 5,  0, 3 }, 10,	/* Energy */
	-10, 8, -1, 8, 20, A_INT, SPE_ACIDSHIELD,   -4, 50
},

{	{"Occult Master", 0}, {
	{"Risky Apprentice",         0},
	{"Dark Magician",         0},
	{"Venture Caster",         0},
	{"Tome Mage",         0},
	{"Arcane Arts Student",         0},
	{"Promising Scholar",         0},
	{"Controller of Dark Arts",         0},
	{"Occult Library Owner",         0},
	{"Backlash Master",  0} },
	"_Kumiromi of Harvest", "Opatos of Earth", "_Ehekatl of Luck",
	"Occ", "Occultist Cave", "Spell Dungeons",
	PM_OCCULT_MASTER, NON_PM, PM_UNDEAD_OCCULT_MASTER, NON_PM, PM_DARK_NIGHTMARE,
	PM_NADJA_THE_OCCULTIST, PM_ATTENDANT, PM_VILJA_THE_BITCH, PM_IRON_GIRL,
	PM_FLAMING_SKULL, PM_OCTARINE_GRUE, S_ZOMBIE, S_GRUE,
	ART_WHISTLE_OF_PROTECTION,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 30, 10, 10, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(10), 19, 15, 19, 17, 16 },
	/* Init   Lower  Higher */
	{  6, 0,  0, 2,  2, 0 },	/* Hit points */
	{ 30, 5,  0, 6,  0, 8 }, 8,	/* Energy */
	0, 0, 0, 
	1, 6, A_INT, SPE_GOOD_NIGHT,        -4, 100 /* From old role.c */
},

{	{"Officer", 0}, {
	{"Street Cop",     0},
	{"Traffic Officer",   0},
	{"Patrolling Cop",0},
	{"Strifer",0},
	{"Crime Scene Cop",    0},
	{"Junior Sheriff",   0},
	{"Deputy Sheriff",     0},
	{"Wild-West Sheriff",    0},
	{"Cop Chief",  0} },
	"Magnum", "Smith", "Wesson", /* guns */
	"Off", "Central Cop Station", "Tank Breaker Hideout",
	PM_OFFICER, NON_PM, PM_UNDEAD_OFFICER, NON_PM, PM_OFFICER,
	PM_COMMISSIONER_HUNTER, PM_STUDENT, PM_GRANDPA_TANK_BREAKER, PM_OUT_OF_JAIL_CONVICT,
	PM_TANK_BREAKER, PM_KOP_KCHIEF, S_HUMAN, S_KOP,
	ART_SHINY_MAGNUM,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 14, 19, 19, 19 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_RESIST_PETRIFICATION,   -4, 40
},

{	{"Ordinator", 0}, {
	{"Battler",     0},
	{"Duelist",     0},
	{"Bladefencer",     0},
	{"Battlemaster",     0},
	{"Overlord",     0},
	{"War Machine",     0},
	{"Three-handed Swordsman",     0},
	{"Walking Weapon Factory",     0},
	{"Fucking Hammer of Thor",     0} },
	"_Vaire", "_Varda Elentari", "_Nessa", /* Elven */

	"Ord", "Top Secret Base", "Evil Lair",
	PM_ORDINATOR, NON_PM, PM_UNDEAD_ORDINATOR, NON_PM, PM_DANCING_GIRL,
	PM_SONTAIRE, PM_HIGHEST_ELF, PM_JYGGALAG, PM_SHEOROGATH,
	PM_LILLEND, PM_NESSIAN_PIT_FIEND, S_ANGEL, S_DEMON,

	ART_VIVEC_BLADE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  50, 10, 10, 10, 10, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 10, 10, 25, 25, 6 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 5,  1, 0 },	/* Hit points */
	{ 10, 0,  0, 2,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_ENCHANT_WEAPON,   -4, 30
},

{	{"Otaku", 0}, {
	{"Heyaduo",         0},
	{"Zhozshi",         0},
	{"Mr. Yu",         "Ms. Yu"},
	{"Songye",         0},
	{"Zhidao",         0},
	{"Juzi",         0},
	{"Wintry, Poplar and Willow",         0},
	{"Yifuji",         0},
	{"Du",         0}},
	"Brahma", "Vishnu", "_Shiva", /* India? */
	"Ota", "Shambala Temple", "Trapped Treasure Room",
	PM_OTAKU, NON_PM, PM_UNDEAD_OTAKU, NON_PM, PM_VAMPIRE,
	PM_SENIOR_OTAKU, PM_ATTENDANT, PM_DANA_DYNAMITE_BEAUTY_MISS, PM_FLEECY_WHITNEY,
	PM_UNDEAD_WIZARD, PM_UNDEAD_UNDERTAKER, S_GOLEM, S_HUMAN,

	ART_ALTAR_CARVER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  17, 17, 16, 19, 15, 18 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_ENTRAPPING,        -4, 50 /* From old role.c */
},

{	{"Paladin", 0}, {
	{"Fighter of Law",    0},
	{"Chivalrous Warrior",    0},
	{"Knightly Strider",    0},
	{"Courageous Battler",    0},
	{"Holy Warrior",    0},
	{"Celestial Warrior",    0},
	{"Peacebringing Knight",    0},
	{"Knight in shiny armor",    0},
	{"God-gifted Warrior",    0}},
	"Ariel", "Tyrael", "Gabriel",	/* angels */
	"Pal", "Paladin Order", "Dark Ritual Cave",
	PM_PALADIN, NON_PM, PM_UNDEAD_PALADIN, NON_PM, NON_PM,
	PM_PALADIN_COLVIN, PM_ACOLYTE, PM_MYSTERIOUS_NECROMANCER, PM_RANDOM_BLACK_MAGE,
	PM_NECROMANCER, PM_UNDEAD_NECROMANCER, S_HUMAN, S_HUMAN,
	ART_ORB_OF_RESISTANCE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7, 10,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{   5, 15, 20, 15, 25, 20 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 19, 15, 17, 19 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 9,  1, 0 },	/* Hit points */
	{ 17, 3,  0, 2,  0, 2 },10,	/* Energy */
	0, 3,-2, 2, 10, A_WIS, SPE_REMOVE_CURSE,    -4, 50
},

{	{"Pickpocket", 0}, {
	{"Cash Stealer",     0},
	{"Money Thief",     0},
	{"Tricky Hands",     0},
	{"Trained Fingers",     0},
	{"Trickster Thief",     0},
	{"Mean Stealer",     0},
	{"Expert Thief",     0},
	{"Moneybag Remover",     0},
	{"Money Van Kidnapper",     0}},
	"Danzai", "Milanor", "Daini", /* ??? */
	"Pic", "Black Market", "Thief Brigand Den",
	PM_PICKPOCKET, NON_PM, PM_UNDEAD_PICKPOCKET, NON_PM, NON_PM,
	PM_SOME_GUY_CALLED_SHADE, PM_THUG, PM_DON_RIO_THE_MASTER_THIEF, PM_GIACOMO_CASANOVA,
	PM_ASSASSIN, PM_UNDEAD_ASSASSIN, S_NYMPH, S_LEPRECHAUN,

	ART_CUTTHROAT_BLADE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7,  7, 10,  7,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  12, 12, 12, 30, 4, 30 },
	/* new limit values for the six attributes by Amy */
	{  18, 17, 16, 25, 16, 18 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_DETECT_TREASURE, -4, 30
},


{	{"Pirate", 0}, {
	{"Landlubber",  0},
	{"Swabbie",        	0},
	{"Cutthroat",   0},
	{"Bosun",      		0},
	{"Second Mate",     0},
	{"First Mate",      0},
	{"Captain",			0},
	{"Pirate Lord",   	0},
	{"Dread Pirate",  	0} },
	"the Lord", "_the deep blue sea", "the Devil",	/* Christian, sorta */
	"Pir", "Tortuga", "Shipwreck Island",
	PM_PIRATE, NON_PM, PM_UNDEAD_PIRATE, NON_PM, NON_PM,
	PM_MAYOR_CUMMERBUND, PM_PIRATE_BROTHER, PM_BLACKBEARD_S_GHOST, PM_WHITEBEARD_S_GHOST,
	PM_SKELETAL_PIRATE, PM_SOLDIER, S_RODENT, S_ELEMENTAL, /* Ghost pirates, soldiers, rats in the food stores, and the occasional storm*/
	ART_TREASURY_OF_PROTEUS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   8,  7, 7,  8,  8,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 10, 20, 25, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 16, 18, 19, 19 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 8, 0, 2,  9, A_INT, SPE_CAUSE_FEAR,    -4, 30
},

{	{"Poison Mage", 0}, {
	{"Poison Mixer",         0},
	{"Venom Scatterer",       0},
	{"Green Poisoner",       0},
	{"Purple Poisoner",     0},
	{"Poison Exploder",       0},
	{"Contaminator",      0},
	{"Stinking Cloud Creator",   0},
	{"Gas Attacker", 0},
	{"Poison Bomber",  0} },
	"Seveso Toxin", "Fukoshima Radiation", "Mustard Gas", /* poison */
	"Poi", "Overrun Base", "Alien UFO",
	PM_POISON_MAGE, NON_PM, PM_UNDEAD_POISON_MAGE, NON_PM, PM_VENOM_FUNGUS,
	PM_CREATOR_OF_POISON, PM_ATTENDANT, PM_GENEWORM, PM_PITWORM,
	PM_GANGRENOUS_WYRM, PM_ALIENATED_UFO_PART, S_WORM, S_HUMAN,
	ART_IMMUNITY_RING,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  18, 18, 19, 18, 20, 19 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_POISON_BLAST,        -4, 100 /* From old role.c */
},

{	{"Pokemon", 0}, {
	{"CROC",    0},
	{"ALIGE",       0},
	{"BEIL",   0},
	{"MADAM", 0},
	{"GUAIL",         0},
	{"BANQ",    0},	/* One skilled at crossbows */
	{"PIJIA",        0},
	{"CLAU",  0},
	{"MIY",      0} },
	"A'En", "Dr. Oujide", "Team Missile Bomb", /* Pokemon Vietnamese Crystal */
	"Pok", "ELF GRANDFATHER RESEARCH LAB", "JOIN-CAVE",
	PM_POKEMON, NON_PM, PM_UNDEAD_POKEMON, NON_PM, PM_EEVEE,
	PM_DR__WUSIJI, PM_POKEMON_TRAINER, PM_HO_OH, PM_PHIX,
	PM_KOFFING, PM_RHYHORN, S_EYE, S_QUADRUPED,
	ART_GAUNTLETS_OF_ILLUSION,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 10, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 19, 19, 19, 19, 20 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  4, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -4, 120
},

{	{"Politician", 0}, {
	{"Voter",     0},
	{"Councillor",   0},
	{"District Councillor",0},
	{"Backbencher",0},
	{"Minister",    0},
	{"Secretary of State",   0},
	{"Chancellor",     0},
	{"Home Secretary",    0},
	{"Prime Minister",  0} },
	"Democracy", "Communism", "Despotism", /* political structures */
	"Pol", "Houses of Parliament", "Lair of Anarchy",
	PM_POLITICIAN, NON_PM, PM_UNDEAD_POLITICIAN, NON_PM, PM_SOLDIER,
	PM_THE_QUEEN, PM_YEOMAN_WARDER, PM_BNP_LEADER, PM_APEC_WORKER,
	PM_GNOLL_GANGSTER, PM_CHAOS_BROTHER, S_GNOME, S_CENTAUR,

	ART_PRIME_MINISTER_S_TUXEDO,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7, 10,  6,  7,  7, 10 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 10, 10,  5, 10, 50 },
	/* new limit values for the six attributes by Amy */
	{  16, 16, 18, 16, 16, 20 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 6,  0, 0 },	/* Hit points */
	{ 10, 0,  0, 1,  0, 1 },14,	/* Energy */
	0, 5, 1, 2, 10, A_INT, SPE_CHARM_MONSTER,   -4, 100
},

{	{ "Practicant", 0 }, {
	{ "Oppressed Lab Worker", 0 },
	{ "Will-less Slave", 0 },
	{ "One Who Fears Assistants", 0 },
	{ "Beach Drama Experiencer", 0 },
	{ "Despotism Endurer", 0 },
	{ "Annoyed Ramming Support", 0 },
	{ "Rebel Leader", 0 },
	{ "System Overthrower", 0 },
	{ "Accomplished Diploma Student", 0 } },
	"Divert", "Oat Camper", "_Jeannine", /* assistants */
	"Pra", "Joy Mountain Hall", "Assistant's Office",
	PM_PRACTICANT, NON_PM, PM_UNDEAD_PRACTICANT, NON_PM, PM_OXYGEN_GOLEM, /* 50% chance of a sulfur golem instead */
	PM_LITTLE_BIA, PM_STUDENT, PM_ASSISTANT_NOROELA, PM_CRUEL_FEMALE_DICTATOR,
	PM_STUDENT_ASSISTANT, PM_UNDEAD_GRADUATE, S_SNAKE, S_UMBER,
	ART_TOO_LATE_PRIZE, /* chemistry set, invoke for summon water elemental and does nothing else */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 15, 20, 20, 20, 10, 15 },
	/* new limit values for the six attributes by Amy */
	{ 18, 20, 20, 22, 12, 18 },
	/* Init   Lower  Higher */
	{ 12, 0, 0, 4, 1, 0 }, /* Hit points */
	{  8, 4, 0, 6, 0, 4 },12, /* Energy */
	10, 6, -3,
	6, 12, A_INT, SPE_BATTERING_RAM, -4, 70
},

{	{"Preversioner", 0}, {
	{"Newcomer",         0},
	{"Green Cross Delver",         0},
	{"Moneyless Adventurer",         0},
	{"Balance Grumbler",         0},
	{"Crash Bug Encounterer",         0},
	{"Bad Game Design Bitcher",         0},
	{"Fook-Yoo-Sayer",         0},
	{"Ch3at0r",         0},
	{"Ascension Run Skipper",  0} },
	"0.54A", "Level Change UAE", "Reset Button Without A Confirmation", /* still salty about the latter... :-P --Amy */
	"Pre", "Northeast Quadrant", "Some Nondescript Dungeon Level",
	PM_PREVERSIONER, NON_PM, PM_UNDEAD_PREVERSIONER, NON_PM, NON_PM,
	PM_POLGARA, PM_HACKER, PM_EXTREMELY_ANCIENT_RED_DRAGON, PM_YET_ANOTHER_RED_KITE,
	PM_OLD_RED_DRAGON, PM_VERY_OLD_BLUE_DRAGON, S_DRAGON, S_DEMON,

	ART_DOWNSIZE_GLASS, /* mirror that cuts boss monsters' health in half, but only for preversioner role */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 2,  2, 0 },	/* Hit points */
	{  7, 1,  0, 1,  0, 1 }, 14,	/* Energy */
	0, 8, -1, 
	2, 3, A_INT, SPE_ACIDSHIELD,        -4, 100 /* From old role.c */
},

{	{"Priest", "Priestess"}, {
	{"Aspirant",    0},
	{"Acolyte",     0},
	{"Adept",       0},
	{"Priest",      "Priestess"},
	{"Curate",      0},
	{"Canon",       "Canoness"},
	{"Lama",        0},
	{"Patriarch",   "Matriarch"},
	{"High Priest", "High Priestess"} },
	0, 0, 0,	/* chosen randomly from among the other roles */
	"Pri", "the Great Temple", "the Temple of Nalzok",
	PM_PRIEST, PM_PRIESTESS, PM_UNDEAD_PRIEST, PM_UNDEAD_PRIESTESS, NON_PM,
	PM_ARCH_PRIEST, PM_ACOLYTE, PM_NALZOK, PM_ZALNOK,
	PM_HUMAN_ZOMBIE, PM_WRAITH, S_ZOMBIE, S_WRAITH,
#if 0
	ART_DISRUPTER, ART_SUNSWORD,
#endif
	ART_MITRE_OF_HOLINESS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7, 10,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 10, 30, 15, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  17, 18, 22, 14, 15, 19 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 17, 3,  0, 2,  0, 2 },10,	/* Energy */
	0, 3,-2, 2, 10, A_WIS, SPE_REMOVE_CURSE,    -4, 75
},

{	{"Prostitute", 0}, {
	{"Light Girl",    0},
	{"Attractive Hooker",    0},
	{"Sexy Butt Cheeks",    0},
	{"Money Body",    0},
	{"Noble Prostitute",    0},
	{"Beautiful Harlot",    0},
	{"Sexbomb",    0},
	{"Top-Class Working Girl",    0},
	{"King's Secret Love",      0} },
	"_Mercedes", "_Misty", "_Kendl", /* Grand Theft Auto */
	"Pro", "Luigi's Sex Club", "Center of Human Hate",
	PM_PROSTITUTE, NON_PM, PM_UNDEAD_PROSTITUTE, NON_PM, PM_ELIF,
	PM_LUIGI, PM_ATTENDANT, PM_FOAMING_WHITE_KNIGHT, PM_SOCIAL_JUSTICE_IDIOT,
	PM_MUMAK, PM_CARRION_BIRD, S_BAT, S_QUADRUPED,
	ART_HIGHEST_FEELING,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 5, 15, 20, 25 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 19, 18, 25 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 3,  2, 0 },	/* Hit points */
	{  7, 0,  0, 2,  0, 2 },12,	/* Energy */
	10, 5, -3, 2, 4, A_INT, SPE_TAKE_SELFIE,   -4, 100
},

{	{"Psion", 0}, {
	{"Cantripper",         0},
	{"Spoonbender",            0},
	{"Kinetic",          0},
	{"Seer", 0},
	{"Psychic", 0},
	{"Oracle", 0},
	{"Levitator",  0},
	{"Visionary",   0},
	{"Master Psion",            0} },
	"Bickney", "Corridor", "Lockney", /* Egyptian */
	"Psi", "the Monastery of Psionics",
	  "the Monastery of the Swamp",
	PM_PSION, NON_PM, PM_UNDEAD_PSION, NON_PM, NON_PM,
	PM_PSI_MASTER, PM_ABBOT, PM_MASTER_SABRINA, PM_SABRAIN,
	PM_DOPPELGANGER, PM_MULTICOLOR_GRUE, S_GRUE, S_HUMAN,
#if 0
	ART_GAUNTLETS_OF_DEFENSE, ART_WHISPERFEET,
#endif
	ART_GAUNTLETS_OF_OFFENSE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  8,  8,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{   5, 30, 30,  5,  5, 25 },
	/* new limit values for the six attributes by Amy */
	{  14, 20, 25, 18, 12, 18 },
	/* Init   Lower  Higher */
	{ 5, 0,  0, 0,  0, 0 },	/* Hit points */
	{ 20, 4,  0, 4,  0, 8 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_WIZARD_LOCK, -4, 200
},

{	{"Psyker", 0}, { /* ZAPM psion */
	{"Odd Ball",   0},
	{"Weirdo",    0},
	{"Mind Reader",      0},
	{"Spoon Bender",     0},
	{"Freakazoid",      0},
	{"Telepath",      0},
	{"Spyion",      0},
	{"Master Psyker",      0},
	{"Farseer",   0} },
	"Bill Rizer", "Lance Bean", "Haggle Man", /* classic NES games */
	"Psy", "Psionic Domain", "Unbalanced Waste",
	PM_PSYKER, NON_PM, PM_UNDEAD_PSYKER, NON_PM, PM_QUADRENNIAL_SODDEN_RATCHET,
	PM_THE_DUNGEON_MASTER, PM_VERY_MINDLESS_THRALL, PM_ROCKFALL_GRIEFER, PM_NONBEHAVING_PLAYER,
	PM_SYNDICATE_GODSEND_READILY, PM_STEEL_MISSINGNO, S_BAD_FOOD, S_WORM_TAIL,

	ART_PSIGUN, /* kalashnikov with psi res */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{   5, 25, 40, 20,  5,  5 },
	/* new limit values for the six attributes by Amy */
	{  15, 25, 25, 21, 12, 15 },
	/* Init   Lower  Higher */
	{   8, 0,  0,  2,  2, 0 },	/* Hit points */
	{  30, 0,  0, 10,  0, 9 }, 12,	/* Energy */
	10, 4, -2, 1, 6, A_INT, SPE_PSYBEAM,      -4, 200
},

{	{"Quarterback", 0}, {
	{"Towel Boy",   0},
	{"Bench Warmer",    0},
	{"Starter",      0},
	{"Jock",     0},
	{"Star Player",      0},
	{"Team Captain",      0},
	{"MVP",      0},
	{"Pro Bowler",      0},
	{"Hall Of Famer",   0} },
	"Roger Staubach", "Peyton Manning", "Tom Brady", /* famous quarterbacks */
	"Qua", "National Training Camp", "No-Rules Playing Field",
	PM_QUARTERBACK, NON_PM, PM_UNDEAD_QUARTERBACK, NON_PM, PM_MISTER_HEAD,
	PM_BASEBALL_LEGEND, PM_HUNTER, PM_RUGBY_FOULER, PM_BONEBREAKER,
	PM_COUNTER_ACE, PM_M__TO_M___, S_TROLL, S_ZOMBIE,

	ART_WORLD_LEAGUE_HITTER, /* baseball bat */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 10,  5, 15, 50,  5 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 14, 20, 25, 18 },
	/* Init   Lower  Higher */
	{  25, 0,  0, 10,  5, 0 },	/* Hit points */
	{   1, 0,  0,  1,  0, 1 }, 16,	/* Energy */
	10, 8, 0, 2, 12, A_INT, SPE_SHUFFLE_MONSTER,      -4, 40
},

  /* Note:  Rogue precedes Ranger so that use of `-R' on the command line
     retains its traditional meaning. */
	/* Amy edit - we enabled a new sorting system so this is moot. */

{	{"Ranger", 0}, {
#if 0	/* OBSOLETE */
	{"Edhel",       "Elleth"},
	{"Edhel",       "Elleth"},      /* elf-maid */
	{"Ohtar",       "Ohtie"},       /* warrior */
	{"Kano",			/* commander (Q.) ['a] */
			"Kanie"},	/* educated guess, until further research- SAC */
	{"Arandur",			/* king's servant, minister (Q.) - guess */
			"Aranduriel"},	/* educated guess */
	{"Hir",         "Hiril"},       /* lord, lady (S.) ['ir] */
	{"Aredhel",     "Arwen"},       /* noble elf, maiden (S.) */
	{"Ernil",       "Elentariel"},  /* prince (S.), elf-maiden (Q.) */
	{"Elentar",     "Elentari"},	/* Star-king, -queen (Q.) */
	"Solonor Thelandira", "Aerdrie Faenya", "Lolth", /* Elven */
#endif
	{"Tenderfoot",    0},
	{"Lookout",       0},
	{"Trailblazer",   0},
	{"Reconnoiterer", "Reconnoiteress"},
	{"Scout",         0},
	{"Arbalester",    0},	/* One skilled at crossbows */
	{"Archer",        0},
	{"Sharpshooter",  0},
	{"Marksman",      "Markswoman"} },
	"Mercury", "_Venus", "Mars", /* Roman/planets */
	"Ran", "Orion's camp", "the cave of the wumpus",
	PM_RANGER, NON_PM, PM_UNDEAD_RANGER, NON_PM, PM_LITTLE_DOG /* Orion & canis major */,
	PM_ORION, PM_HUNTER, PM_SCORPIUS, PM_CRABBIUS,
	PM_FOREST_CENTAUR, PM_SCORPION, S_CENTAUR, S_SPIDER,
#if 0
	0, 0,
#endif
	ART_LONGBOW_OF_DIANA,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 10, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 16, 25, 16, 19 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  4, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -4, 40
},

{	{"Ringseeker", 0}, {
	{"Hobbit Townie",      0},
	{"Drunken Hobbit",      0},
	{"Celebrating Hobbit",      0},
	{"Ringquester",      0},
	{"Fellow of the Ring",      0},
	{"Moria Survivor",      0},
	{"Blarog Basher",      0},
	{"Mount Doom Hiker",      0},
	{"Hero who defeated Sauron",      0} },
	"Lurtz", "Saruman", "Morgoth",
	"Rin", "Bree", "Misty Mountains",
	PM_RINGSEEKER, NON_PM, PM_UNDEAD_RINGSEEKER, NON_PM, NON_PM,
	PM_BILBO_BAGGINS, PM_PROUDFOOT, PM_GOLLUM, PM_PSEUDO_GOLLUM,
	PM_SWAMP_KOBOLD, PM_URUK_HAI, S_KOBOLD, S_ORC,
	ART_ONE_RING,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 10, 20 },
	/* new limit values for the six attributes by Amy */
	{  16, 20, 20, 20, 14, 16 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 6,  1, 0 },	/* Hit points */
	{ 15, 4,  0, 2,  0, 2 },12,	/* Energy */
	0, 0, -2, 4, 12, A_INT, SPE_INVISIBILITY,   -4, 30
},

{	{"Rocker", 0}, {
	{"Whistleblower",     0},
	{"Tooter",   0},
	{"Hooter",0},
	{"Violinist",0},
	{"Guitarist",    0},
	{"Bassist",   0},
	{"Percussionist",     0},
	{"Harper",    0},
	{"Synthesizer",  0} },
	"Classic Rock", "Symphonic Metal", "Hardcore Punk",	/* music styles */
	"Roc", "Bigband Studio", "Black Metal Den",
	PM_ROCKER, NON_PM, PM_UNDEAD_ROCKER, NON_PM, PM_SPEEDHORSE,
	PM_ROCKIN_ROLLER, PM_ATTENDANT, PM_DEATH_METAL_ORCHESTRA_LEADER, PM_GOD_OF_SPEED_METAL,
	PM_DEATH_METAL_FREAK, PM_DEATH_METAL_DRUMMER, S_HUMAN, S_UNICORN,
	ART_ROCKER_SLING,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 14, 17, 18, 19, 19 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	-10, 5, 0, 2, 10, A_INT, SPE_FORCE_BOLT,   -4, 30
},

{	{"Rogue", 0}, {
	{"Footpad",     0},
	{"Cutpurse",    0},
	{"Rogue",       0},
	{"Pilferer",    0},
	{"Robber",      0},
	{"Burglar",     0},
	{"Filcher",     0},
	{"Magsman",     "Magswoman"},
	{"Thief",       0} },
	"Issek", "Mog", "Kos", /* Nehwon */
	"Rog", "the Thieves' Guild Hall", "the Assassins' Guild Hall",
	PM_ROGUE, NON_PM, PM_UNDEAD_ROGUE, NON_PM, NON_PM,
	PM_MASTER_OF_THIEVES, PM_THUG, PM_MASTER_ASSASSIN, PM_DEMIMASTER_ASSASSIN,
	PM_LEPRECHAUN, PM_GUARDIAN_NAGA, S_NYMPH, S_NAGA,
#if 0
	ART_DOOMBLADE, ART_BAT_FROM_HELL,
#endif
	ART_MASTER_KEY_OF_THIEVERY,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7,  7,  7, 10,  7,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 10, 10, 30, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 15, 25, 17, 20 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_DETECT_TREASURE, -4, 30
},

{	{"Sage", 0}, {
	{"Invoker",      0},
	{"Summoner",      0},
	{"Secret Keeper",      0},
	{"Mage Adept",      0},
	{"Witchcrafter",      0},
	{"Illusionist",      0},
	{"Geomancer",      0},
	{"Astral Caster",      0},
	{"Master Of All Spells",      0}},
	"_Cassandra", "Menelaos", "_Helen of Troy", /* Greek history */
	"Sag", "Ancient Sage College", "The Trojan Horse",
	PM_SAGE, NON_PM, PM_UNDEAD_SAGE, NON_PM, PM_KITTEN,
	PM_ANCIENT_SAGE, PM_APPRENTICE, PM_RODNEY_S_TWIN, PM_RODNEY_S_SISTER,
	PM_AIR_SPIRIT, PM_RODNEY_S_SISTER, S_NEMESE, S_NEMESE,

	ART_SHILLELAGH,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 30, 10, 10, 10 },
	/* new limit values for the six attributes by Amy */
	{  13, 22, 25, 17, 13, 19 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 25, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 3, 10, A_INT, SPE_CREATE_FAMILIAR,   -4, 200
},

{	{"Saiyan", 0}, {
	{"Little Wannabe Fighter",         0},
	{"Saiyan Amateur",          0},
	{"Saiyan Apprentice",            0},
	{"Anime Hero", 0},
	{"Monkey Boy", 0},
	{"Z-Warrior", 0},
	{"Saiyan Prince",  0},
	{"Saiyan General",   0},
	{"Saiyan Grandmaster",            0} },
	"Moori", "King Kai", "Vegeta", /* Dragonball series */
	"Sai", "Kame House", "Goku's arrival ship",
	PM_SAIYAN, NON_PM, PM_UNDEAD_SAIYAN, NON_PM, PM_DRAGONBALL_KID,
	PM_MASTER_ROSHI, PM_ROSHI, PM_DARK_GOKU, PM_FRIEZA,
	PM_KAMEHAMEHA_FIGHTER, PM_SUPER_SAIYAN, S_HUMAN, S_DEMON,

	ART_MODIFIED_Z_SWORD,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  8,  8,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  25, 10, 20, 20, 15, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 17, 18, 19, 18, 16 },
	/* Init   Lower  Higher */
	{ 17, 0,  0, 8,  1, 0 },	/* Hit points */
	{  8, 2,  0, 2,  0, 2 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_FINGER_OF_DEATH, -4, 40
},


{	{"Samurai", 0}, {
	{"Hatamoto",    0},  /* Banner Knight */
	{"Ronin",       0},  /* no allegiance */
	{"Ninja Gaiden",       "Kunoichi"},  /* secret society */
	{"Joshu",       0},  /* heads a castle */
	{"Ryoshu",      0},  /* has a territory */
	{"Kokushu",     0},  /* heads a province */
	{"Daimyo",      0},  /* a samurai lord */
	{"Kuge",        0},  /* Noble of the Court */
	{"Shogun",      0} },/* supreme commander, warlord */
	"_Amaterasu Omikami", "Raijin", "Susanowo", /* Japanese */
	"Sam", "the Castle of the Taro Clan", "the Shogun's Castle",
	PM_SAMURAI, NON_PM, PM_UNDEAD_SAMURAI, NON_PM, PM_LITTLE_DOG,
	PM_LORD_SATO, PM_ROSHI, PM_ASHIKAGA_TAKAUJI, PM_OSAYO_NARAKAMI,
	PM_WOLF, PM_STALKER, S_DOG, S_ELEMENTAL,
#if 0
	ART_SNICKERSNEE, ART_DRAGONBANE,
#endif
	ART_TSURUGI_OF_MURAMASA,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  8,  7, 10, 17,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 10,  8, 30, 14,  8 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 18, 17, 18, 18 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  1, 0 },	/* Hit points */
	{  7, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_CLAIRVOYANCE,    -4, 20
},

{	{"Scientist", 0}, {
	{"Tube Mixer",    0},
	{"Practician",	0},
	{"Advanced Practician",	0},
	{"Experimentator",	0},
	{"Test Runner", 0},
	{"Graduate Scientist", 0},
	{"Simpleton with a PhD",0},
	{"Rocket Scientist",0},
	{"Nobel-Prized Scientist",0} },
	"Nikola Tesla", "Erwin Schroedinger", "Wernher von Braun", /* famous scientists */
	"Sci", "Black Mesa Research Facility", "Xen",
	PM_SCIENTIST, NON_PM, PM_UNDEAD_SCIENTIST, NON_PM, NON_PM,
	PM_GORDON_FREEMAN, PM_HACKER, PM_NIHILANTH, PM_BIG_FLYING_BABY,
	PM_INFANTRYMAN, PM_GI_TROOPER, S_GOLEM, S_WALLMONST,

	ART_OPERATIONAL_SCALPEL,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10, 10, 10,  8,  9, 7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 20, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 19, 18, 19, 14, 19 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 3,  1, 0 },	/* Hit points */
	{ 20, 5,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_IDENTIFY,   -4, 35
},

{	{"Secret Advice Member", 0}, {
	{"Hidden Operative",         0},
	{"Covert OP",         0},
	{"Underground Agitator",         0},
	{"Secret Schemer",         0},
	{"Gun Hider",         0},
	{"Shady Executive",         0},
	{"Banishment Wielder",         0},
	{"Top-Secret Advice Member",         0},
	{"Boss of the Secret Advice",  0} },
	"_Katrin", "Len-kind", "Coffin Nail",
	"Sec", "Secret Advice Hiding Place", "Revoluzzer Force HQ",
	PM_SECRET_ADVICE_MEMBER, NON_PM, PM_UNDEAD_SECRET_ADVICE_MEMBER, NON_PM, PM_BUST_SUPERSECRET_ADVICE_RIFLING_UNVERIFIED_BOSOMING,
	PM_CHAIRMAN_LEON, PM_TROOPER, PM_REVOLUTINO_MARI, PM_ASS_GAS_SPREADER,
	PM_BODYBUILDER_LICH, PM_BACKFIRING_LICH, S_ZOMBIE, S_LICH,

	ART_ASS_DEAF_AUGER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 20, 15, 20, 15, 15 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 20, 20, 20, 20, 20 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 4,  5, 0 },	/* Hit points */
	{ 10, 2,  0, 2,  0, 3 }, 17,	/* Energy */
	10, 6, -3, 
	4, 5, A_WIS, SPE_TELEPORT_AWAY,        -4, 100 /* From old role.c */
},

{	{"Shadow Jedi", 0}, {
	{"Dark Lighter",    0},
	{"Hunkbasher",    0},
	{"Lightsaber Form Trainer",    0},
	{"Style Changer",    0},
	{"Whirler",    0},
	{"Jedi Without Jedi Powers",    0},
	{"Chargeless Power",    0},
	{"Fizzle-Fizzer",    0},
	{"Blacklight Master",    0} },
	"the Light Side", "the Force", "the Dark Side",
	"Sdw", "the Jedi Temple", "the Outer Rim",
	PM_SHADOW_JEDI, NON_PM, PM_UNDEAD_SHADOW_JEDI, NON_PM, PM_LITTLE_DOG,
	PM_THE_JEDI_MASTER, PM_PADAWAN, PM_LORD_SIDIOUS, PM_LORD_INSIDIOUS,
	PM_SHOTGUN_STORMTROOPER, PM_LASER_STORMTROOPER, S_HUMAN, S_HUMAN,
	ART_GAMMASABER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7, 14, 12, 10, 14 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30, 15, 15, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 20, 20, 18, 20 },
	/* Init   Lower  Higher */
	{ 20, 4,  0, 8,  2, 4 },	/* Hit points */
	{  5, 4,  0, 1,  0, 4 },10,	/* Energy */
	0, 12 , -1, 
	2, 10, A_INT, SPE_CHARM_MONSTER,    -4, 40
},

{	{"Shapeshifter", 0}, {
	{"Polymorph Trainee",    0},
	{"Item Zapper",    0},
	{"Polymorphitis Hero",    0},
	{"Wild Changer",    0},
	{"Polypiler",    0},
	{"Human Chameleon",    0},
	{"Master Shapechanger",    0},
	{"Grandmaster Shapechanger",    0},
	{"Ultimate Shapechanger",    0} },
	0, 0, 0,	/* chosen randomly from among the other roles */
	"Sha", "Transformer College", "Mutation Cave",
	PM_SHAPESHIFTER, NON_PM, PM_UNDEAD_SHAPESHIFTER, NON_PM, PM_CHAMELEON,
	PM_MASTER_SHIFTER, PM_SHIFTER, PM_TRANSMUTER, PM_TRANSFORMATOR,
	PM_CHAMELEON, PM_GUARDIAN_NAGA, S_LIZARD, S_NAGA,
	ART_MEDALLION_OF_SHIFTERS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 25, 25, 10, 10, 20 },
	/* new limit values for the six attributes by Amy */
	{  16, 17, 15, 14, 10, 12 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 3,  1, 0 },	/* Hit points */
	{ 20, 0,  0, 4,  0, 4 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_POLYMORPH,   -4, 30
},

{	{"Shoe Fetishist", 0}, {
	{"Socksmeller",         0},
	{"Combat Boot Slave",         0},
	{"Shin-Smashed Sneaker",         0},
	{"Bathtub Despairer",         0},
	{"Dogshit Challenger",         0},
	{"Final Dancer",         0},
	{"Fall-From-Grace",         0},
	{"Toilet Puzzler",         0},
	{"Winner At Last",  0} },
	"Birkenstock", "Tamaris", "Peter Kaiser", /* German shoe brands */
	"Sho", "Heel Hall", "Underway Simulation",
	PM_SHOE_FETISHIST, NON_PM, PM_UNDEAD_SHOE_FETISHIST, NON_PM, PM_KATI_S_STRIPED_SNEAKER,
	PM_SIMULATION_LEADER, PM_MODEL, PM_ARABELLA, PM_TUFTED_ASIAN_GIRL, /* make sure Arabella doesn't get the wand! */
	PM_ANTJE_S_BLOCK_HEELED_BOOT, PM_HENRIETTA_S_THICK_BLOCK_HEELED_BOOT, S_RODENT, S_SPIDER,

	ART_KATHARINA_S_LOVELINESS, /* platform boots */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 20, 15, 15, 10, 30 },
	/* new limit values for the six attributes by Amy */
	{  16, 18, 18, 18, 14, 18 },
	/* Init   Lower  Higher */
	{  9, 0,  0, 1,  1, 0 },	/* Hit points */
	{  0, 0,  0, 1,  0, 1 }, 14,	/* Energy */
	10, 10, 2, 
	12, 10, A_INT, SPE_HOLD_AIR,        -4, 30 /* From old role.c */
},

{	{"Singslave", 0}, {
	{"Imprisoned Fool",         0},
	{"Mushfingers",         0},
	{"Shocked Cleaner",         0},
	{"Dogshit Remover",         0},
	{"Mastercurser",         0},
	{"Hard-Working Slave",         0},
	{"Re-Imprisoned",         0},
	{"Work Refuser",         0},
	{"Escaped Ex-Slave",  0} },
	"_Blue-Heeled Ute", "_Red-Heeled Nancy", "_Ski-Heeled Brigitte",
	"Sin", "Tinbrook Shop", "Shoe Torture Chamber",
	PM_SINGSLAVE, NON_PM, PM_UNDEAD_SINGSLAVE, NON_PM, PM_SLAVE,
	PM_TUFTRIKE, PM_SERVANT, PM_PROFILE_KATI, PM_BROWN_BOOT_BITCH,
	PM_NEREID, PM_LILLDESEAM, S_NYMPH, S_ANGEL,

	ART_SABRINA_S_RESCUE, /* platform boot */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 15, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 17, 16, 15, 18, 15 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 3,  2, 0 },	/* Hit points */
	{  2, 1,  0, 1,  0, 0 },11,	/* Energy */
	10, 6, -2, 
	4, 12, A_INT, SPE_REPAIR_ARMOR,        -4, 20 /* From old role.c */
},


{	{"Slave Master", 0}, {
	{"Slave Trader",      0},
	{"People Seller",      0},
	{"Human Abducter",      0},
	{"Professional Slaver",      0},
	{"Kidnapper",      0},
	{"Poocher",      0},
	{"Whipmaster",      0},
	{"Master Slaver",      0},
	{"King of Slavers",      "Queen of Slavers"}},
	"Airyaman", "Gandarewa", "Daevas", /* Persian */
	"Sla", "Prassia", "Ankh-Morpork",
	PM_SLAVE_MASTER, NON_PM, PM_UNDEAD_SLAVE_MASTER, NON_PM, PM_BIG_DOG,
	PM_SLAVE_EMPEROR, PM_STUDENT, PM_RINCEWIND, PM_FAILED_SORCEROR,
	PM_FROST_MONSTER, PM_AQUA_MUNCHER, S_JABBERWOCK, S_RUSTMONST,

	ART_DRAGON_WHIP,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	  ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
/*	{   7, 10, 10,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 5, 15, 30, 30, 5 }, 
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 16, 8, 19, 20, 19 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 10, 0,  0, 1,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_COMMAND_UNDEAD,   -4, 40
},

{	{"Social Justice Warrior", 0}, {
	{"Safe Zone Kid",   0},
	{"Low Gatekeeper",    0},
	{"Cesspool Dweller",      0},
	{"Intolerant Fanatic",     0},
	{"Echo Chamber Sounder",      0},
	{"Banhammer Wielder",      0},
	{"Party Line Ensurer",      0},
	{"Miss-Leader",      0},
	{"Major Minoritist",   0} },
	"_Zoe Quinn", "_Brianna Wu", "_Anita Sarkeesian", /* feminists */
	"Soc", "Secluded Forum", "Evil Wide World",
	PM_SOCIAL_JUSTICE_WARRIOR, NON_PM, PM_UNDEAD_SOCIAL_JUSTICE_WARRIOR, NON_PM, PM_OFFICER_JENNY,
	PM_ZEALOUS_MODERATOR, PM_THUG, PM_FAT_GROPER, PM_STROKE_FEEL_UPPER,
	PM_CLONED_NEW_VEGAS_NPC, PM_ULTRA_DESTRUCTIVE_MONSTER, S_ZOMBIE, S_GOLEM,

	ART_MARKER_OF_SAFE_SPEECH,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 10, 10, 20, 20, 25 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 15, 16, 19, 20, 18 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 7,  1, 0 },	/* Hit points */
	{  7, 0,  0, 1,  0, 6 }, 15,	/* Energy */
	0, 4, -2, 6, 12, A_WIS, SPE_PROTECTION,      -4, 70
},

{	{"Software Engineer", 0}, {
	{"Summer Intern",   0},
	{"Q/A Tester",    0},
	{"Web Designer",      0},
	{"Help Desk Jockey",     0},
	{"Junior Programmer",      0},
	{"Sysadmin",      0},
	{"Lead Programmer",      0},
	{"VP Engineering",      0},
	{"High Programmer",   0} },
	"Linus Torvalds", "Bjarne Stroustrup", "Mark Zuckerberg", /* famous software engineers */
	"Sof", "Devteam Codebase", "H@cker Central",
	PM_SOFTWARE_ENGINEER, NON_PM, PM_UNDEAD_SOFTWARE_ENGINEER, NON_PM, PM_GEEK,
	PM_THE_DEV_TEAM, PM_HACKER, PM_ANONYMOUS_LEADER, PM__CHAN_POSTER,
	PM_WINDOWS_____, PM_WINDOWS____SMARTPHONE_EDITION_, S_XAN, S_XAN,

	ART_FLAILMOUSE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 20, 20, 10, 10 },
	/* new limit values for the six attributes by Amy */
	{  16, 24, 20, 18, 15, 16 },
	/* Init   Lower  Higher */
	{  10, 0,  0, 6,  2, 0 },	/* Hit points */
	{  12, 0,  0, 6,  0, 4 }, 14,	/* Energy */
	10, 5, 0, 1, 5, A_INT, SPE_CODE_EDITING,      -4, 200
},

{	{"Space Marine", 0}, {
	{"Private",   0},
	{"Corporal",    0},
	{"Space Sergeant",      0},
	{"Space Cadet",     0},
	{"Space Lieutenant",      0},
	{"Space Captain",      0},
	{"Space Major",      0},
	{"Space Colonel",      0},
	{"Space General",   0} },
	"Krog", "Winston", "Cyrus", /* Clan EIT; Cyrus is also the ZAPM lead dev */
	"Sma", "Ye Olde Space Base", "Delta Caves",
	PM_SPACE_MARINE, NON_PM, PM_UNDEAD_SPACE_MARINE, NON_PM, PM_CUNTGUN_TROOPER,
	PM_EITBO, PM_TROOPER, PM_THE_SPIDER_MASTERMIND, PM_SPIDERDEMON_CLONE,
	PM_KLINGON_CAPTAIN, PM_SPACE_ORC_BOSS, S_ORC, S_HUMAN,

	ART_PIN_RACKET, /* some sort of melee weapon */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30,  5,  5, 20, 30, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 15, 15, 18, 22, 18 },
	/* Init   Lower  Higher */
	{  24, 0,  0, 10,  5, 0 },	/* Hit points */
	{   1, 0,  0,  1,  0, 1 }, 15,	/* Energy */
	10, 16, 4, 6, 20, A_WIS, SPE_SHINING_WAVE,      -4, 30
},

{	{"Spacewars Fighter", 0}, {
	{"Roguelike Beginner",     0},
	{"Dungeon Explorer",     0},
	{"Sword Swinger",    0},
	{"Burly Combatant",    0},
	{"Heroic Mage",      0},
	{"Battlemage",    0},
	{"Spellsword",   0},
	{"Nightblade",   0},
	{"King's True Heir",     0} },
	"_Lady Ariane", "Lord Stahngnir", "Sven Fanara", /* from a certain book --Amy */
	"Spa", "Tiny Hamlet", "Inside Part",
	PM_SPACEWARS_FIGHTER, NON_PM, PM_UNDEAD_SPACEWARS_FIGHTER, NON_PM, NON_PM,
	PM_MARC, PM_ATTENDANT, PM_ARABELLA, PM_TUFTED_ASIAN_GIRL,
	PM_WIZARD, PM_MANTICORE, S_HUMAN, S_DRAGON,
	ART_HELM_OF_STORMS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   3,  3,  3,  3,  3,  3 },
	{  17, 17, 16, 17, 17, 16 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  2, 0 },	/* Hit points */
	{ 10, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_INT, SPE_DETECT_MONSTERS,     -4, 100
},

{	{"Stormboy", 0}, {
	{"Yoof",   0},
	{"Cannon Fodder",    0},
	{"Meatshield",      0},
	{"Warbuddy",     0},
	{"Skarboy",      0},
	{"Mob Kaptain",      0},
	{"Nob",      0},
	{"Meganob",      0},
	{"Warboss",   0} },
	"Cortege", "Tache", "Pillory", /* from Pokemon Vietnamese Crystal */
	"Sto", "Stormy Mountain", "Deepsea Plane",
	PM_STORMBOY, NON_PM, PM_UNDEAD_STORMBOY, NON_PM, PM_DRATINI,
	PM_STINKBUG_UNBLEMISHED, PM_FELLOW_DEATH_EATER, PM_PRESAGE_OCTUPLE_GULPING, PM_POSTERN_BREW,
	PM_GREATER_WATER_ELEMENTAL, PM_RUST_VORTEX, S_ELEMENTAL, S_VORTEX,

	ART_THUNDERSTRIKE, /* melee weapon with shock damage */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  25, 10,  5, 20, 30, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 15, 19, 21, 16 },
	/* Init   Lower  Higher */
	{  18, 0,  0, 7,  4, 0 },	/* Hit points */
	{   2, 0,  0, 2,  0, 3 }, 12,	/* Energy */
	0, 12, 0, 4, 15, A_INT, SPE_THUNDER,      -4, 20
},

{	{"Supermarket Cashier", 0}, {
	{"Low Worker",     0},
	{"Shift Worker",     0},
	{"Accord Worker",     0},
	{"Bored Cashier",     0},
	{"Shoplifter Detector",     0},
	{"Detective disguised as Cashier",     0},
	{"Gun-toting Shopkeeper",     0},
	{"Thief's Bane",     0},
	{"Safemaster",     0}},
	"the Homies", "the Robbers", "the Motherfuckers", /* taken from a fanfic */
	"Sup", "the local Wal-Mart", "those filthy spying inspectors' office",
	PM_SUPERMARKET_CASHIER, NON_PM, PM_UNDEAD_SUPERMARKET_CASHIER, NON_PM, PM_WOLF,
	PM_MUSTACHED_NIGHTSHIFT_SUPERMARKET_MANAGER, PM_GUIDE, PM_THE_LOCAL_HEALTH_INSPECTOR, PM_INCAPABLE_MALE_NURSE,
	PM_STORECLERK, PM_WERERAT, S_HUMAN, S_RODENT,

	ART_CREDEX_GOLD,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7, 10,  6,  7,  7, 10 },*/
	{   3,  3,  3,  3,  3,  3 },
	{   5, 5, 15, 15, 30, 30 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 19, 16, 19 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  0, 0 },	/* Hit points */
	{ 12, 0,  0, 1,  0, 1 },14,	/* Energy */
	0, 5, 1, 2, 10, A_INT, SPE_TELEPORT_AWAY,   -4, 40
},

{	{ "Symbiant", 0 }, {
	{ "Moldhome", 0 },
	{ "Piggybacker", 0 },
	{ "Stationary Seeker", 0 },
	{ "Symbiote Powerer", 0 },
	{ "Master of Greater Powers", 0 },
	{ "Sym-Breather", 0 },
	{ "Elite Symbiant", 0 },
	{ "Master Jelly Farmer", 0 },
	{ "Supreme Symbiosis Teacher", 0 } },
	"Erotic Air Current Noises", "Wonderful Rubbing Noises", "Sexy Licking Noises", /* Amyism :D */
	"Sym", "Lothlorien Forest", "The Princess's Another Castle",
	PM_SYMBIANT, NON_PM, PM_UNDEAD_SYMBIANT, NON_PM, PM_SPIKE_MOLD,
	PM_TURGON, PM_HIGHEST_ELF, PM_ILLUSORY_REFLECTOR_GOLEM, PM_PSEUDO_CASTLE_BOSS,
	PM_TRIP_GRUE, PM_HUGE_PILE_OF_KILLER_COINS, S_GRUE, S_BAD_COINS,
	ART_HUNGERHEALER, /* H2G2, passively increases hunger and heals symbiote while it's in your inventory */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 10, 20, 30, 15, 10, 15 },
	/* new limit values for the six attributes by Amy */
	{ 16, 18, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  9, 0, 0, 1, 1, 0 }, /* Hit points */
	{  8, 4, 0, 6, 0, 4 },15, /* Energy */
	10, 6, -8,
	5, 10, A_WIS, SPE_SYMHEAL, -4, 75
},

{	{"Thalmor", 0}, {
	{"Concordate Agent",     0},
	{"Whitegold Ambassador",     0},
	{"Heretic Hunter",     0},
	{"enemy of Talos",     0},
	{"Hunter of Blades",     0},
	{"Elven Secret Agent",     0},
	{"Aldmeri Dominion",     0},
	{"Elf at War",     0},
	{"Summerset Dictator",     0} },
	"Manwe Sulimo", "Mandos", "Lorien", /* Elven */

	"Tha", "Thalmor Embassy", "Cloud Ruler Temple",
	PM_THALMOR, NON_PM, PM_UNDEAD_THALMOR, NON_PM, PM_THALMOR,
	PM_ELENWEN, PM_HIGHEST_ELF, PM_ESBERN, PM_JAUFFRE,
	PM_HEDROW_BLADEMASTER, PM_BLACKBELT, S_HUMAN, S_HUMAN,

	ART_SUMMONED_SWORD,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  17, 17, 17, 17, 16, 16 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 19, 16, 18, 17, 20 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 4,  4, 0 },	/* Hit points */
	{ 10, 0,  0, 4,  0, 4 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_FULL_HEALING,   -4, 50
},

{	{"Topmodel", 0}, {
	{"Sweet Girl",    0},
	{"Catwalk Tester",       0},
	{"Curved Woman",   0},
	{"Playboy Bunny", 0},
	{"Erotic Lady",         0},
	{"Lovely Chick",    0},
	{"Sexy Poser",        0},
	{"ANTM Semi-Finalist",  0},
	{"ANTM Finalist",      0} },
	"Speedy Gonzales", "Dan Naginati", "_Kylie Lum", /* taken from a fanfic */
	"Top", "Red Carpet", "Violet Carpet", /* transvestite enemies like purple and violet */
	PM_TOPMODEL, NON_PM, PM_UNDEAD_TOPMODEL, NON_PM, NON_PM,
	PM_CAMP_PORTER, PM_MODEL, PM_ACTIVIST_LEADER, PM_FEMEN_WARLORD,
	PM_TRANSVESTITE, PM_ACTIVIST, S_HUMAN, S_HUMAN,
	ART_GOLDEN_HIGH_HEELS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 25, 15, 15, 10, 25 },
	/* new limit values for the six attributes by Amy */
	{  12, 16, 16, 21, 14, 25 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 4,  2, 0 },	/* Hit points */
	{  8, 0,  0, 1,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_CHARM_MONSTER,   -4, 90
},

{	{ "Tosser", 0 }, {
	{ "Javazon Wannabe", 0 },
	{ "Peeker", 0 },
	{ "Graystabber", 0 },
	{ "Stack Increaser", 0 },
	{ "Uncommon Flinger", 0 },
	{ "Wood Chooser", 0 },
	{ "Javelin Forger", 0 },
	{ "Bigstacker", 0 },
	{ "Torpedo Launcher", 0 } },
	"Arihant", "Siddha", "Acharya", /* Jain */
	"Tos", "Measurer Spear Base", "Sierra Madre Vault",
	PM_TOSSER, NON_PM, PM_UNDEAD_TOSSER, NON_PM, NON_PM,
	PM_MUTE_CHRISTINE, PM_HUNTER, PM_FATHER_ELIJAH, PM_TRAITORIOUS_ELDER,
	PM_FETTER_GHOST, PM_MUTANT_NINJA_TURTLE, S_GHOST, S_UMBER,
	ART_CLEAN_MAULER, /* some kind of spear, always hits as a +4 weapon and +20 damage, +10 to-hit but dulls quickly like ceramic */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 20, 15, 15, 20, 15, 15 },
	/* new limit values for the six attributes by Amy */
	{ STR18(100), 18, 18, 21, 18, 18 },
	/* Init   Lower  Higher */
	{ 12, 0, 0, 4, 3, 0 }, /* Hit points */
	{  4, 2, 0, 2, 0, 2 },14, /* Energy */
	10, 8, -2,
	4, 12, A_INT, SPE_STEALTH, -4, 40
},

{	{"Tourist", 0}, {
	{"Rambler",     0},
	{"Sightseer",   0},
	{"Excursionist",0},
	{"Peregrinator","Peregrinatrix"},
	{"Traveler",    0},
	{"Journeyer",   0},
	{"Voyager",     0},
	{"Explorer",    0},
	{"Adventurer",  0} },
	"Blind Io", "_The Lady", "Offler", /* Discworld */
	"Tou", "Ankh-Morpork", "the Thieves' Guild Hall",
	PM_TOURIST, NON_PM, PM_UNDEAD_TOURIST, NON_PM, NON_PM,
	PM_TWOFLOWER, PM_GUIDE, PM_MASTER_OF_THIEVES, PM_MASTER_OF_DESASTER,
	PM_GIANT_SPIDER, PM_FOREST_CENTAUR, S_SPIDER, S_CENTAUR,
#if 0
	ART_WHISPERFEET, ART_LUCKBLADE,
#endif
	ART_YENDORIAN_EXPRESS_CARD,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7, 10,  6,  7,  7, 10 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 10, 10, 15, 30, 20 },
	/* new limit values for the six attributes by Amy */
	{  STR18(20), 16, 15, 18, 14, 22 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  0, 0 },	/* Hit points */
	{ 12, 0,  0, 1,  0, 1 },14,	/* Energy */
	0, 5, 1, 2, 10, A_INT, SPE_CHARM_MONSTER,   -4, 40
},

{	{"Tracer", 0}, {
	{"Cursed Beginner",    0},
	{"RNG Whim",    0},
	{"Would-Be Creator",    0},
	{"Fated Grumbler",    0},
	{"Bad Wielder",    0},
	{"Sent Manipulator",    0},
	{"Evasive Switcher",    0},
	{"Random Ascension Kit Wearer",    0},
	{"Nastiness Survivor",      0} },
	"Fate", "Unlimited Blade Works", "Heaven's Feel",
	"Trc", "Game Alike Hall", "Trapped Corridor",
	PM_TRACER, NON_PM, PM_UNDEAD_TRACER, NON_PM, PM_ROTHE,
	PM_STEREOTYPICAL_QUEST_GIVER, PM_STARNOID, PM_EVIL_PATCH_PERPETRATOR, PM_FIGMENT_OF_NASTINESS,
	PM_BAOBHAN_SITH, PM_SNAKEMAN_BRUTE, S_HUMANOID, S_QUANTMECH,
	ART_SWORD_OF_GILGAMESH,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 15,  5, 30, 10, 25 },
	/* new limit values for the six attributes by Amy */
	{  STR18(40), 18, 16, 19, 15, 18 },
	/* Init   Lower  Higher */
	{ 8,  0,  0, 1,  7, 0 },	/* Hit points */
	{ 35, 0,  0, 1,  0, 1 }, 20,	/* Energy */
	0, 12, -6, 2, 4, A_INT, SPE_POLYFORM,   -4, 50
},

{	{"Transsylvanian", 0}, {
	{"Heel Wearer",    0},
	{"Stiletto Lover",       0},
	{"Khmer-Speaking Transvestite",   0},
	{"Dragshow Attendant",   0},
	{"Sexy Malemodel",   0},
	{"Hippie Rocker",   0},
	{"High-Heel-Loving Guy",   0},
	{"Androgynous Beauty",   0},
	{"Fleecy Sex Symbol",      0} },
	0, 0, 0, /* special deities */
	"Trs", "Topmodel Boot Camp", "Secret Activist Base",
	PM_TRANSSYLVANIAN, NON_PM, PM_UNDEAD_TRANSSYLVANIAN, NON_PM, NON_PM,
	PM_G_CHEATER, PM_ATTENDANT, PM_HELEN_THE_ACTIVIST_LEADER, PM_SUPREME_FEMEN_WARCHIEF,
	PM_ACTIVIST, PM_SLUT_WITH_SYPHILIS, S_HUMAN, S_NYMPH,
	ART_KISS_BOOTS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 10, 10, 20, 20, 20 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 14, 19, 16, 22 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 6,  1, 0 },	/* Hit points */
	{  4, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_LEVELPORT,   -4, 50
},


{	{"Transvestite", 0}, {
	{"Clacker",    0},
	{"Staggerer",       0},
	{"Pseudo Model",   0},
	{"Disguiser", 0},
	{"Carnevalist",         0},
	{"Heeler",    0},
	{"Crossdresser",        0},
	{"Drag Lord",  0},
	{"Drag Queen",      0} },
	"_Olivia", "Peyman", "_Lady Gaga", /* weird fashion sense */
	"Tra", "Topmodel Boot Camp", "World 3 Airship",
	PM_TRANSVESTITE, NON_PM, PM_UNDEAD_TRANSVESTITE, NON_PM, NON_PM,
	PM_CROWNED_DRAG_QUEEN, PM_ATTENDANT, PM_WENDY, PM_DENWY,
	PM_ADULT_LADY, PM_ADULT_GENTLEMAN, S_HUMAN, S_HUMAN,
	ART_LOVELY_PINK_PUMPS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 10, 10, 20, 20, 20 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 14, 19, 16, 22 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 6,  1, 0 },	/* Hit points */
	{  4, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -4, 50
},

{	{"Twelph", 0}, {
	{"Twedhel",       "Twelleth"},
	{"Twedhel",       "Twelleth"},      /* elf-maid */
	{"Twohtar",       "Twohtie"},       /* warrior */
	{"Twekano",			/* commander (Q.) ['a] */
			"Twekanie"},	/* educated guess, until further research- SAC */
	{"Twarandur",			/* king's servant, minister (Q.) - guess */
			"Twaranduriel"},	/* educated guess */
	{"Twehir",         "Twehiril"},       /* lord, lady (S.) ['ir] */
	{"Twaredhel",     "Twarwen"},       /* noble elf, maiden (S.) */
	{"Twernil",       "Twelentariel"},  /* prince (S.), elf-maiden (Q.) */
	{"Twelentar",     "Twelentari"} },	/* Star-king, -queen (Q.) */
	"Legolas", "_Eowyn", "Eol", /* Elven */

	"Twe", "the great Circle of Lolth", "the Light Elves' Cave",
	PM_TWELPH, NON_PM, PM_UNDEAD_TWELPH, NON_PM, PM_DROW,
	PM_LOLTH, PM_HIGHEST_ELF, PM_TWEARENDIL, PM_MEAN_ELVEN_BASTARD,
	PM_GREY_ELF, PM_ELVENKING, S_HUMAN, S_HUMAN,

	ART_TENTACLE_STAFF,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 20, 20, 10, 10 },
	/* new limit values for the six attributes by Amy */
	{  17, 19, 19, 23, 15, 19 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 4,  1, 0 },	/* Hit points */
	{ 12, 0,  0, 4,  0, 3 },12,	/* Energy */
	10, 0, 0, 4, 25, A_INT, SPE_DRAIN_LIFE,   -4, 30
},

{	{"Unbeliever", 0}, {
	{"Rookie",    0},
	{"Soldier",    0},
	{"Mercenary",    0},
	{"Veteran",    0},
	{"Champion",    0},
	{"Hero",    0},
	{"Baron",    0},
	{"Duke",    0},
	{"Lord",    0} },
	"No God", "No God", "No God", /* Unbeliever does not believe in gods! */
	"Unb", "Bree", "Khazad-Doom",
	PM_UNBELIEVER, NON_PM, PM_UNDEAD_UNBELIEVER, NON_PM, PM_MARTTI_IHRASAARI,
	PM_MELINDA_PROUDFOOT, PM_SUPER_WARRIOR, PM_MORGOTH, PM_MELKOR,
	PM_MASTER_LICH, PM_BALOR, S_DEMON, S_LICH,
	ART_TOME_DARK_SWORD,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30,  5,  5, 25, 30,  5 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 19, 19, 17, 20, 18 },
	/* Init   Lower  Higher */
	{ 24, 0,  0,12,  4, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 },10,	/* Energy */
	10, 30, 0, 10,  20, A_INT, SPE_BLANK_PAPER,      -4, 5
},

{	{"Undead Slayer", 0}, {
	{"Assistant",    0},
	{"Eliminator",   0},
	{"Eliminator",   0},
	{"Exterminator", 0},
	{"Exterminator", 0},
	{"Destroyer",   0},
	{"Vindicator",  0},
	{"Vindicator",  0},
	{"Undead Slayer", 0} },
	"Seeker", "_Osiris", "Seth", /* Egyptian */
	"Und", "the Temple of Light", "the Crypt of Dracula",
	PM_UNDEAD_SLAYER, NON_PM, PM_NON_UNDEAD_SLAYER, NON_PM, NON_PM,
	PM_VAN_HELSING, PM_EXTERMINATOR, PM_COUNT_DRACULA, PM_COUNT_URCULA,
	PM_HUMAN_MUMMY, PM_VAMPIRE, S_MUMMY, S_VAMPIRE,
#if 0
	ART_HOLY_SPEAR_OF_LIGHT, ART_SUNSWORD,
#endif
	ART_STAKE_OF_VAN_HELSING,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */ /* Modified from Knight */
	/*{  13,  7, 14,  8, 10, 10 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 15, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 15, 17, 14, 18, 16 },
	/* Init   Lower  Higher */
	{ 22, 0,  0, 8,  2, 0 },	/* Hit points */
	{ 16, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4, 20
},

{	{"Undertaker", 0}, {
	{"Hearse Driver",    0},
	{"Disposer",   0},
	{"Grave Creator",   0},
	{"Open Casketeer", 0},
	{"Closed Casketeer", 0},
	{"Dumper",   0},
	{"Shoveler",  0},
	{"Crematorist",  0},
	{"Corpse Burner", 0} },
	"_Hel", "Pluto", "Orcus", /* Death */
	"Unt", "New Graveyard", "Underground Demon Lair",
	PM_UNDERTAKER, NON_PM, PM_UNDEAD_UNDERTAKER, NON_PM, PM_DEATH_DOG,
	PM_AJELA, PM_EXTERMINATOR, PM_SEDUCER_SAINT, PM_OBLIVION_DAEDRA,
	PM_NALFESHNEE, PM_SHAYATEEN, S_DEMON, S_DEMON,

	ART_BLACK_DEATH,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */ /* Modified from Knight */
	/*{  13,  7, 14,  8, 10, 10 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 15, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 17, 18, 17, 17 },
	/* Init   Lower  Higher */
	{ 22, 0,  0, 8,  2, 0 },	/* Hit points */
	{ 16, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4, 25
},

{	{"User of Stand", 0}, {
	/*from Silly JNetHack*/
	/*and arrange [Sakusha]*/
	{"Arrow Pierced",	0},
	{"Possessed",	0},
	{"Conjurer",	0},
	{"Magician",	0},
	{"Stand user",	0},
	{"Journeyer",	0},
	{"Voyager",	0},
	{"Explorer",	0},
	{"Adventurer",	0} },
	"Jonathan Joestar", "Hirohiko Araki", "Cars", /* Jojo */
	"Use", "the House of Kujyou", "the Palace of Dio",
	PM_STAND_USER, NON_PM, PM_UNDEAD_STAND_USER, NON_PM, PM_BARD,
	PM_MESSENGER_OF_SPW,  PM_GUIDE, PM_DIO, PM_UNO,
	PM_VAMPIRE, PM_HUMAN_ZOMBIE, S_ZOMBIE, S_ZOMBIE,
	ART_RED_STONE_OF_EIGIA,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */ /* Modified from Knight */
	/*{  13,  7, 14,  8, 10, 10 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 15, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{    18, 19, 18, 19, 17, 18 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 4,  1, 0 },	/* Hit points */
	{  8, 2,  0, 0,  0, 1 },10,	/* Energy */
	10, 4,-2, 0,  9, A_INT, SPE_AGGRAVATE_MONSTER,     -4, 200
},

{	{"Valkyrie", 0}, {
	{"Stripling",   0},
	{"Skirmisher",  0},
	{"Fighter",     0},
	{"Man-at-arms", "Woman-at-arms"},
	{"Great Warrior",     0},
	{"Swashbuckler",0},
	{"Hero",        "Heroine"},
	{"Champion",    0},
	{"Lord",        "Lady"} },
	"Tyr", "Odin", "Loki", /* Norse */
	"Val", "the Shrine of Destiny", "the cave of Surtur",
	PM_VALKYRIE, NON_PM, PM_UNDEAD_VALKYRIE, NON_PM, NON_PM /*PM_WINTER_WOLF_CUB*/,
	PM_NORN, PM_SUPER_WARRIOR, PM_LORD_SURTUR, PM_LORD_SULTUR,
	PM_FIRE_ANT, PM_FIRE_GIANT, S_ANT, S_GIANT,
#if 0
	ART_MJOLLNIR, ART_FROST_BRAND,
#endif
	ART_ORB_OF_FATE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 12,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30,  6,  7, 20, 30,  7 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 15, 15, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 8,  2, 0 },	/* Hit points */
	{  2, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 10,-2, 0,  9, A_WIS, SPE_LIGHTNING,    -4, 20
},

{	{"Vanilla Valk", 0}, {
	{"Stripped Girl",   0},
	{"Carnage Woman",  0},
	{"Feighter",     0},
	{"Armsbearing Lady", 0},
	{"Vanilla Warrior",     0},
	{"Protecter-Shielder",0},
	{"Female Heroine",        0},
	{"Fanatic Champ",    0},
	{"Battlefield Chief",        0} },
	"Tyr", "Odin", "Loki", /* Norse */
	"Van", "the Shrine of Destiny", "the cave of Surtur",
	PM_VANILLA_VALK, NON_PM, PM_UNDEAD_VANILLA_VALK, NON_PM, NON_PM /*PM_WINTER_WOLF_CUB*/,
	PM_NORN, PM_SUPER_WARRIOR, PM_LORD_SURTUR, PM_LORD_SULTUR,
	PM_FIRE_ANT, PM_FIRE_GIANT, S_ANT, S_GIANT,
	ART_ORB_OF_WEIGHT,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 12,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  30,  6,  7, 20, 30,  7 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 15, 15, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  2, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 10,-2, 0,  9, A_WIS, SPE_CONE_OF_COLD,    -4, 20
},

{	{"Walscholar", 0}, {
	{"Alpha Mission Team",   0},
	{"Sector Beta Sweeper",    0},
	{"Order Acceptor",      0},
	{"Diamond Waller",     0},
	{"Distress Disposer",      0},
	{"Teacher's Favorite",      0},
	{"Treebark Protector",      0},
	{"Superior Hider",      0},
	{"Bane of Robbers",   0} },
	"Speaker Walt", "Great Corner-Hard", "Left Guenter", /* superschool people */
	"Wal", "7c space", "Hussy Main Office",
	PM_WALSCHOLAR, NON_PM, PM_UNDEAD_WALSCHOLAR, NON_PM, PM_SUPERSCHOOL_SPEAKER,
	PM_GAUNT_ELKE, PM_THUG, PM_HUSSY_KATRIN, PM_EVIL_PURPLE_GIRL,
	PM_ARVOGENE, PM_KEWITRIN, S_LIGHT, S_WORM_TAIL,

	ART_STAHNGNIR_S_GIANT_CRUSHER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   3,  3,  3,  3,  3,  3 },
	{  17, 17, 16, 17, 17, 16 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 4,  1, 0 },	/* Hit points */
	{ 7, 2,  0, 1,  0, 1 },10,	/* Energy */
	0, 12,-2, 0,  10, A_INT, SPE_GRAVE,     -4, 100
},

{	{"Wandkeeper", 0}, {
	{"Zapper",    0},
	{"Wand Carrier",    0},
	{"Magic Shooter",    0},
	{"Zapbolter",    0},
	{"Recharger",    0},
	{"Charge Master",    0},
	{"Wandmaker",    0},
	{"Senior Wandmaker",    0},
	{"Wishing Wand Creator",    0}},
	"Larry Koopa", "Roy Koopa", "Morton Koopa Jr.", /* Super Mario Bros */
	"Wan", "Caster Temple", "Blaster Crypts",
	PM_WANDKEEPER, NON_PM, PM_UNDEAD_WANDKEEPER, NON_PM, PM_DARK_GRUE,
	PM_MASTER_CASTER, PM_EXTERMINATOR, PM_PLASTER_BLASTER, PM_CRASHBUG_CAUSER,
	PM_DAUGHTER_LILY, PM_MARIJA_THE_WENCH, S_ARCHFIEND, S_NEMESE,

	ART_CHARGED_USB_STICK,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */ /* Modified from Knight */
	/*{  13,  7, 14,  8, 10, 10 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 19, 19, 19, 13, 15 },
	/* new limit values for the six attributes by Amy */
	{  STR18(30), 16, 18, 16, 15, 19 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 6,  2, 0 },	/* Hit points */
	{ 10, 4,  0, 2,  0, 3 },10,	/* Energy */
	5, 2,-2, 10,  9, A_WIS, SPE_POLYMORPH,     -4, 75
},
{	{"Warrior", 0}, {
	{"Swordsman",     0},
	{"Longswordsman",     0},
	{"Two-Handed Swordsman",    0},
	{"Legionnaire",    0},
	{"Crusader",      0},
	{"Baron",    "Baroness"},
	{"Count",   "Countess"},
	{"Duke",   "Duchess"},
	{"Patriarch",     "Matriarch"} },
	"Talos", "_Meridia", "Clavicus Vile", /* The Elder Scrolls */
	"War", "Acro Castle", "the Isle of the Damned",
	PM_WARRIOR, NON_PM, PM_UNDEAD_WARRIOR, NON_PM, PM_GREEN_NIGHTMARE,
	PM_SIR_LANCELOT, PM_PAGE, PM_ARCHNEMESIS, PM_ETERNAL_EVIL,
	PM_LOCUST, PM_WOOD_NYMPH, S_ANT, S_NYMPH,
#if 0
	ART_DRAGONBANE, ART_DEMONBANE,
#endif
	ART_MAGIC_MIRROR_OF_JASON,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  18,  7, 11, 10, 12, 14 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  35, 10, 14, 13, 22, 6 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 14, 14, 18, 18, 16 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 15,  5, 0 },	/* Hit points */
	{ 10, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4, 20
},

{	{"Weirdboy", 0}, {
	{"Crazy One",   0},
	{"Very Mad Orc",    0},
	{"Frazzler",      0},
	{"Humming Head",     0},
	{"Brain Warper",      0},
	{"Walking Psi Bomb",      0},
	{"One Orc Storm",      0},
	{"Warptide",      0},
	{"Warp Unleasher",   0} },
	"Tax Evasion", "Murder and Arson", "Prison Outbreak", /* crimes, listed in order of severity from minor to major :P */
	"Wei", "Lunatic Asylum", "Sanity Clinic",
	PM_WEIRDBOY, NON_PM, PM_UNDEAD_WEIRDBOY, NON_PM, PM_SCHINDA,
	PM_EXPLODING_BRAIN, PM_VERY_MINDLESS_THRALL, PM_WELL_PAID_DOCTOR, PM_DEGREE_SIMPLETON,
	PM_EXTREMELY_CONTAMINATED_BUG, PM_IT, S_XAN, S_GRUE,

	ART_BUS_ERROR, /* segfault venom */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 10, 20, 40, 10,  5 },
	/* new limit values for the six attributes by Amy */
	{  16, 18, 12, 22, 14, 17 },
	/* Init   Lower  Higher */
	{   6, 0,  0, 10, 0, 0 },	/* Hit points */
	{  17, 0,  0, 0,  0, 3 }, 11,	/* Energy */
	0, 17, -6, 1, 1, A_INT, SPE_GIANT_FOOT,      -4, 80
},

{	{"Wild Talent", 		 0}, {
	{"Poschengband Noob",        0},
	{"Camelot Explorer",        0},
	{"Ambush Avoider",        0},
	{"Morivant Merchant",        0},
	{"Angband Diver",        0},
	{"Ocean Traveler",        0},
	{"666 Delver",        0},
	{"555 Survivor",        0},
	{"One Who Killed The Serpent",        0}},
	0, 0, 0,	/* chosen randomly from among the other roles */
	"Wil", "Source Code Base", "Closed Source Quarters",
	PM_WILD_TALENT, NON_PM, PM_UNDEAD_WILD_TALENT, NON_PM, PM_CHAMELEON,
	PM_ELLI_PSEUDO, PM_STUDENT, PM_CHRIS_CHENG, PM_POS_RECURSIVE,
	PM_RANDOM_HORROR, PM_GAZER_HORROR, S_UMBER, S_BAD_COINS,
	ART_ELLI_S_PSEUDOBAND_OF_POS,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  6,  6,  6,  6,  6,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  16,  17,  17, 17, 16,  17 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 4,  4, 0 },	/* Hit points */
	{  8, 0,  4, 0,  4, 0 }, 12,	/* Energy */
	5, 8, 0,  5, 10, A_WIS, SPE_RUSSIAN_ROULETTE,             -4, 80
},

{	{"Wizard", 0}, {
	{"Evoker",      0},
	{"Conjurer",    0},
	{"Thaumaturge", 0},
	{"Magician",    0},
	{"Warlock",     "Witch"},
	{"Enchanter",   "Enchantress"},
	{"Sorcerer",    "Sorceress"},
	{"Wizard",      0},
	{"Mage",        0} },
	"Ptah", "Thoth", "Anhur", /* Egyptian */
	"Wiz", "the Lonely Tower", "the Tower of Darkness",
	PM_WIZARD, NON_PM, PM_UNDEAD_WIZARD, NON_PM, PM_KITTEN,
	PM_NEFERET_THE_GREEN, PM_APPRENTICE, PM_DARK_ONE, PM_LIGHT_ONE,
	PM_VAMPIRE_BAT, PM_XORN, S_BAT, S_WRAITH,
#if 0
	ART_MAGICBANE, ART_DELUDER,
#endif
	ART_EYE_OF_THE_AETHIOPICA,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{   7, 10,  7,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  10, 30, 10, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{    12, 22, 19, 16, 14, 16 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 25, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 3, 10, A_INT, SPE_MAGIC_MISSILE,   -4, 200
},

{	{"Womanizer", 0}, {
	{"Cheekstroker",	0},
	{"Curve Adorer",	0},
	{"Sweetheart Lover",	0},
	{"Turn-On-Jockey",	0},
	{"Speech Charmer",	0},
	{"Promise Keeper",	0},
	{"Engaged Man",	0},
	{"Married Luck Mushroom",	0},
	{"Happiness Enjoyer",	0} },
	"Charlie Sheen", "Tiger Woods", "George Clooney", /* famous womanizers */
	"Wom", "Neverending Party Hall", "Socjus Brigade Center",
	PM_WOMANIZER, NON_PM, PM_UNDEAD_WOMANIZER, NON_PM, PM_EROTICLOUDY,
	PM_HUGE_HEFNER, PM_MODEL, PM_BERBAL, PM_NUTCRUNCHER_BITCH,
	PM_SOCIAL_JUSTICE_WARRIOR, PM_ANGRY_PROSTITUTE, S_HUMAN, S_DOG,
	ART_SPECTRUM_PLING, /* covelline or whatever that gem's name was, reflection and stuff */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  15, 10, 15, 15, 25, 20 },
	/* new limit values for the six attributes by Amy */
	{  18, 17, 17, 18, 19, 17 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 4,  4, 0 },	/* Hit points */
	{  6, 0,  0, 3,  0, 2 }, 16,	/* Energy */
	0, 12, 0, 5, 14, A_INT, SPE_TAKE_SELFIE,   -4, 20
},

{	{"XelNaga", 0}, {
	{"Outworlder",   "Void Explorer"},
	{"Genesis Witness",    0},
	{"Chaos Shaper",      0},
	{"Nebulae Catcher",     0},
	{"Star Assembler",      0},
	{"Planet Shifter",      0},
	{"Galaxy Designer",      0},
	{"DNA Programmer",      0},
	{"Species Creator",   0} },
	"Jim Raynor", "Hierarch Artanis", "_Sarah Kerrigan", /* starcraft */
	"Xel", "Terra Base", "Brooding Chaos",
	PM_XELNAGA, NON_PM, PM_UNDEAD_XELNAGA, NON_PM, PM_BIRSEN,
	PM_SHADURIEL, PM_STARNOID, PM_INSECT_HIVEMIND, PM_VERY_LARGE_BUG,
	PM_LAG_DEFENSE_TOWER, PM_PUTJANA, S_WALLMONST, S_TURRET,

	ART_SNAKELASH, /* whip */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{   25, 15, 10, 15, 30, 5 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 16, 14, 18, 19, 10 },
	/* Init   Lower  Higher */
	{   18, 0,  0, 8,  3, 0 },	/* Hit points */
	{    2, 0,  0, 2,  0, 2 },14,	/* Energy */
	-20, 8, -1, 4, 14, A_INT, SPE_ACIDSHIELD,      -4, 40
},

{	{"Yautja", 0}, {
	{"Unblooded",   0},
	{"Blooded",    0},
	{"Honored One",      0},
	{"Master Hunter",     0},
	{"Vanguard",      0},
	{"Elite",      0},
	{"Clan Leader",      0},
	{"Elder",      0},
	{"Adjudicator",   0} },
	"Macbeth", "Zoness", "_Titania", /* Star Fox 64 */
	"Yau", "Asteroid Field", "Barren Toxic Waste",
	PM_YAUTJA, NON_PM, PM_UNDEAD_YAUTJA, NON_PM, PM_ROBOT,
	PM_THE_OUTERSPACE_OCTOPUS, PM_STARNOID, PM_LUKE_SKYWALKER, PM_HAN_SOLO,
	PM_TOXIC_FUNGUS, PM_COLOSSAL_SLIME, S_FUNGUS, S_PUDDING,

	ART_EYEHANDER, /* blindfold */
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  16,  7,  7, 15, 16,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  25, 10, 10, 25, 25,  5 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 16, 16, 20, 20, 8 },
	/* Init   Lower  Higher */
	{  20, 0,  0, 7,  7, 0 },	/* Hit points */
	{   6, 0,  0, 3,  0, 2 }, 15,	/* Energy */
	-10, 10, 0, 2, 7, A_INT, SPE_SLUDGE,      -4, 25
},

{	{"Yeoman", 0}, {
	/* Landowner titles, naval ranks and positions */
	/* We intentionally avoid Lieutenant and Captain */
	{"Usher",          0},
	{"Steward",        "Stewardess"},
	{"Keeper",         0},
	{"Marshal",        0},
	{"Master Steward", "Master Stewardess"},
	{"Chamberlain",    0},
	{"Constable",      0},
	{"Chancellor",     0},
	{"Regent",         0} },
	"His Majesty", "His Holiness", "The Commons", /* The three estates */
	"Yeo", "London", "the inner ward",
	PM_YEOMAN, NON_PM, PM_UNDEAD_YEOMAN, NON_PM, PM_PONY,
	PM_CHIEF_YEOMAN_WARDER, PM_YEOMAN_WARDER, PM_COLONEL_BLOOD, PM_COLONEL_B__ASTARD,
	PM_RAVEN, PM_WEREWOLF, S_RODENT, S_DOG,
#if 0
	ART_REAPER, ART_SWORD_OF_JUSTICE,
#endif
	ART_CROWN_OF_SAINT_EDWARD,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  12,  7, 10, 12, 12,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 15, 15, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 16, 16, 19, 18 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  2, 0 },	/* Hit points */
	{  3, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_KNOCK,     -4, 20
},

{	{"YSexymate", 0}, {
	/*from Silly JNetHack*/
	{"Hiyokko",	0},
	{"Flutty",	0},
	{"Miso Souper",	0},
	{"Fitness Strider",	0},
	{"Black Mate",      0},
	{"Knee Supporter",	0},
	{"Beard Mantlet",	0},
	{"Global Pertner",	0},
	{"Sexy King",	"Sexy Queen"} },
	"Yoroshiku", "Ma Shin", "Sexy Sai", /* Chinese */
	"Sex", "Mistranslated Entrance", "Japanese Quest Goal Area",
	PM_SEXYMATE, NON_PM, PM_UNDEAD_SEXYMATE, NON_PM, PM_SEXY_GIRL,
	PM_JUDO_CAPTAIN, PM_STUDENT, PM_GIGOCK, PM_GIGOLO,
	PM_SILLY_NYMPH, PM_HIGE_MESO, S_TROLL, S_MIMIC,
	ART_CHARMPOINT,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  13, 13, 13,  9, 13,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  12, 30, 4, 12, 12, 30 },
	/* new limit values for the six attributes by Amy */
	{    16, 18, 18, 20, 16, 25 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 4,  2, 0 },	/* Hit points */
	{  8, 0,  0, 1,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_REPAIR_ARMOR,   -4, 30
},

{	{"Zookeeper", 0}, {
	{"Dog Feeder",      0},
	{"Cat Tamer",0},
	{"Wolf Feeder",0},
	{"Lion Tamer",     0},
	{"Mastodon Feeder",   0},
	{"Dragon Tamer",   0},
	{"Tyrannosaurus Feeder",0},
	{"Kangaroo Tamer",   0},
	{"Zouthern Petkeeper",     0} },
	"Balder", "_Edda", "Hagen", /* Norse mythology */
	"Zoo", "Yellowstone Park", "Jurassic Park",
	PM_ZOOKEEPER, NON_PM, PM_UNDEAD_ZOOKEEPER, NON_PM, PM_PLATYPUS,
	PM_MASTER_ZOOKEEPER, PM_STUDENT, PM_FEARFUL_TRICERATOPS, PM_ASH_CLANNFEAR,
	PM_TYRANNOSAUR, PM_UNDEAD_KANGAROO, S_LIZARD, S_ZOUTHERN,

	ART_LIONTAMER,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	  ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
/*	{   7, 10, 10,  7,  7,  7 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  20, 20, 20, 10, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(30), 16, 17, 18, 17, 15 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 10, 0,  0, 1,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_CHARM_MONSTER,   -4, 40
},

{	{"Zyborg", 0}, {
	{"Tin Can",     0},
	{"Artificial Stupidity",     0},
	{"Metal Man",   "Metal Woman"},
	{"Automaton",0},
	{"Mechwarrior",0},
	{"Android",    0},
	{"Advanced Robot",   0},
	{"Artificial Intelligence",    0},
	{"Synthetic Man",  "Synthetic Woman"} },
	"Andromorph", "Technix", "Mechatron",	/* made-up names by Amy */
	"Zyb", "Tech Facility", "Chaos Factors",
	PM_ZYBORG, NON_PM, PM_UNDEAD_ZYBORG, NON_PM, NON_PM,
	PM_MISTER_MULBERRY, PM_ATTENDANT, PM_MASTER_BRAIN, PM_IDIOT_BRAIN,
	PM_UNDEAD_DRAGON, PM_GRIFFIN_ROGUE, S_WALLMONST, S_GRUE,
	ART_VERBAL_BLADE,
	MH_HUMAN|MH_ELF|MH_DWARF|MH_GNOME|MH_HOBBIT|MH_VAMPIRE|MH_WERE|MH_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	/*{  10,  7,  7,  7, 13,  6 },*/
	{   3,  3,  3,  3,  3,  3 },
	{  18, 18, 18, 18, 18, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  20, 0,  0, 5,  2, 0 },	/* Hit points */
	{  4, 2,  0, 4,  0, 4 },10,	/* Energy */
	0, 5, 0, 2, 10, A_INT, SPE_SOLAR_BEAM,   -4, 75
},

/* Array terminator */
{{0, 0}}
};


/* The player's role, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Role urole =
{	{"Undefined", 0}, { {0, 0}, {0, 0}, {0, 0},
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	"L", "N", "C", "Xxx", "home", "locate",
	NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM,
	NON_PM, NON_PM, 0, 0, 
#if 0
	0, 0,
#endif
	0, 0,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7,  7,  7,  7 },
	{  20, 15, 15, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 25, 25, 25, 25, 25 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 3 },14,	/* Energy */
	0, 10, 0, 0,  4, A_INT, 0, -3
};

struct Role ustartrole =
{	{"Undefined", 0}, { {0, 0}, {0, 0}, {0, 0},
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	"L", "N", "C", "Xxx", "home", "locate",
	NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM,
	NON_PM, NON_PM, 0, 0, 
#if 0
	0, 0,
#endif
	0, 0,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7,  7,  7,  7 },
	{  20, 15, 15, 20, 20, 10 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 25, 25, 25, 25, 25 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 3 },14,	/* Energy */
	0, 10, 0, 0,  4, A_INT, 0, -3
};

/* Table of all races */
const struct Race races[] = {
{	"addict", "addicted", "addiction", "Add",
	{0, 0},
	PM_ADDICT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 25, 25, 25, 25, 25 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"aggravator", "aggravator", "aggravation", "Agg",
	{0, 0},
	PM_AGGRAVATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 20, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"ak Thief is dead!", "ak Thief is dead!", "vailable, exit anyway?", "Akt", /* garbled strings --Amy */
	{0, 0},
	PM_AK_THIEF_IS_DEAD_, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_WERE | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_WERE, 0, MH_ELF|MH_GNOME|MH_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 18, 18, 19, 22, 14 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 2,  2, 0 },	/* Hit points */
	{  5, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"albae", "albae", "albaehood", "Alb",
	{0, 0},
	PM_ALBAE, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 20, 14, 20, 12, 25 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 6,  4, 0 },	/* Hit points */
	{  8, 0,  6, 0,  4, 0 }		/* Energy */
},
{	"alchemist", "alchemist", "alchemy", "Alc",
	{0, 0},
	PM_ALCHEMIST, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 16, 18, 12, 10 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"alien", "alien", "alienhood", "Ali",
	{0, 0},
	PM_ALIEN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(60), 16, 16, 16, 16, 12 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"american", "american", "patriotism", "Ame",
	{0, 0},
	PM_AMERICAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, MH_ELF, MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 20, 16 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"amnesiac", "amnesiac", "Alzheimer's", "Amn",
	{0, 0},
	PM_AMNESIAC, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 20, 18, 18, 19 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"ancient", "ancient", "ancienty", "Anc",
	{0, 0},
	PM_ANCIENT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 19, 20, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"ancipital", "ancipital", "ancipitalism", "Acp",
	{0, 0},
	PM_ANCIPITAL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 20, 15 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  2, 0,  3, 0,  4, 0 }		/* Energy */
},
{	"android", "android", "android-kind", "And",
	{0, 0},
	PM_PLAYER_ANDROID, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{ STR19(20), 18, 16, 22, 22, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 3,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 },	/* Energy */
},
{	"angbander", "angbander", "angband followship", "Agb",
	{0, 0},
	PM_ANGBANDER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, MH_ELF, MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 20, 20, 20, 20, 20 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"angel", "angelic", "angelhood", "Ang",
	{0, 0},
	PM_HUMANOID_ANGEL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 18, 18, 14, 25 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  2, 0 },	/* Hit points */
	{  2, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"aquarian", "aquarian", "aquarianship", "Aqu",
	{0, 0},
	PM_AQUATIC_MONSTER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 20 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"argonian", "argonian", "argohood", "Arg",
	{0, 0},
	PM_ARGONIAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 18, 16, 20, 18, 10 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"asgardian", "asgardish", "asgardhood", "Asg",
	{0, 0},
	PM_ASGARDIAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  4, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"asura", "asura", "hindu mythology", "Asu",
	{0, 0},
	PM_PLAYER_ASURA, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 16, 16, 19, 18, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"atlantean", "atlantean", "atlantis", "Atl",
	{0, 0},
	PM_PLAYER_ATLANTEAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 19, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"aztpok", "aztpok", "aztpokland", "Azt",
	{0, 0},
	PM_AZTPOK, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"babylonian", "babylonian", "babylon", "Bab",
	{0, 0},
	PM_BABYLONIAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     18,     18, 18, 17, 21, 16 },
	/* Init   Lower  Higher */
	{  6, 0,  0, 6,  8, 0 },	/* Hit points */
	{  0, 0,  1, 0,  0, 0 }		/* Energy */
},
{	"bacteria", "bacteria", "meshella", "Bac",
	{0, 0},
	PM_BACTERIA, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 18, 18, 16, 25, 18 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 10,  10, 0 },	/* Hit points */
	{  8, 0,  12, 0,  15, 0 }		/* Energy */
},
{	"bastard", "bastard", "bastardry", "Bas",
	{0, 0},
	PM_BASTARD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"batman", "bat", "bathood", "Bat",
	{0, 0},
	PM_BATMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 18, 18, 22, 18, 20 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 3,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  3, 0 }		/* Energy */
},
{	"beacher", "beacher", "beach drama", "Bea",
	{0, 0},
	PM_BEACHER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"blait", "blait", "black and white", "Bla",
	{0, 0},
	PM_BLAIT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"bodymorpher", "bodymorpher", "morphing bodies", "Bod",
	{0, 0},
	PM_BODYMORPHER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"borg", "borg", "borgship", "Bor",
	{0, 0},
	PM_BORG, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 14, 15, 18, 12 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 0,  1, 0 },	/* Hit points */
	{  1, 0,  0, 0,  1, 0 }		/* Energy */
},
{	"bossrusher", "bossrusher", "boss rush", "Bos",
	{0, 0},
	PM_BOSSRUSHER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"bover", "bover", "bovinedia", "Bov",
	{0, 0},
	PM_BOVER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HOBBIT,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 22, 18 },
	/* Init   Lower  Higher */
	{  6, 0,  0, 4,  3, 0 },	/* Hit points */
	{  1, 0,  2, 0,  1, 0 }		/* Energy */
},
{	"breton", "bretonian", "bretonhood", "Bre",
	{0, 0},
	PM_BRETON, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     18,     25, 25, 18, 12, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  5, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"buldozgar", "buldozgar", "bulborderland", "Bul",
	{0, 0},
	PM_BULDOZGAR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_DWARF|MH_ELF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 22, 20 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 5,  4, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"burninator", "burning", "burning forest", "Bur",
	{0, 0},
	PM_BURNINATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 16, 16, 15, 22, 15 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"carthage", "carthage", "carthago", "Car",
	{0, 0},
	PM_CARTHAGE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{        16, 16, 16, 16, 16, 16 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 10,  10, 0 },	/* Hit points */
	{  2, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"celtic", "celtic", "celtic empire", "Cel",
	{0, 0},
	PM_CELTIC, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(50), 16, 15, 19, 21, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"centaur", "centaur", "centaurhood", "Cen",
	{0, 0},
	PM_HUMANOID_CENTAUR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(21), 16, 18, 20, 18, 16 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  4, 0 },	/* Hit points */
	{  2, 0,  2, 0,  3, 0 }		/* Energy */
},
{	"cerberus", "cerberus", "tartarus", "Cer",
	{0, 0},
	PM_PLAYER_CERBERUS, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 18, 18, 20, 24, 12 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 6,  7, 0 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"challenger", "challenger", "challenge", "Cha",
	{0, 0},
	PM_CHALLENGER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"changeling", "changeling", "chhangelinger", "Chg",
	{0, 0},
	PM_PLAYER_CHANGELING, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"chiquai", "chiquai", "chiquai gavenmengte", "Chu",
	{0, 0},
	PM_CHIQUAI, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"chiropteran", "chiropteric", "chiropteran-hood", "Chi",
	{0, 0},
	PM_CHIROPTERAN, NON_PM, PM_CHIROPTERAN_MUMMY, PM_CHIROPTERAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_CHAOTIC|ROLE_NEUTRAL,
	MH_HUMAN, 0, MH_GNOME|MH_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20),  16, 16, 20, 14, 16 },
	/* Init   Lower  Higher */
	{  2, 0,  1, 0,  0, 1 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"clockwork automaton", "automatic", "clockwork-kind", "Clk",
	{0, 0},
	PM_CLOCKWORK_AUTOMATON, NON_PM, NON_PM, NON_PM,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, MH_DWARF, MH_GNOME,
	/*    Str     Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  3, 0,  3, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 },	/* Energy */
},
{	"cockatrice", "cockatrice", "rubberchickenhood", "Coc",
	{0, 0},
	PM_ARMED_COCKATRICE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{      18,    18, 18, 18, 16, 10 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"colorator", "colorator", "fleecy colors <3", "Col",
	{0, 0},
	PM_COLORATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"cortex", "cortex", "cortexhood", "Cor",
	{0, 0},
	PM_CORTEX, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 18, 18, 18, 12 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  3, 0,  3, 0,  4, 0 }		/* Energy */
},
{	"cupid", "cupid", "cupidity", "Cup",
	{0, 0},
	PM_CUPID, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     12,     12, 12, 21, 12, 25 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"curser", "curser", "curserhood", "Cur",
	{0, 0},
	PM_CURSER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 19, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"dark seducer", "mazke", "mazken", "Sed",
	{0, 0},
	PM_MAZKE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 20, 18, 16, 20 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"deathmold", "deathmold", "deathmoldhood", "Dea",
	{0, 0},
	PM_DEATHMOLD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 19, 14, 18, 18,  3 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"deep elf", "deep elven", "deep elvenkind", "Dee",
	{0, 0},
	PM_DEEP_ELF, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ELF, MH_ELF, MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     18,    20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  7, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"destabilizer", "destabilizer", "destabilization", "Des",
	{0, 0},
	PM_DESTABILIZER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(21), 18, 18, 18, 20, 14 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"developer", "developer", "DevTeam", "Dvp",
	{0, 0},
	PM_DEVELOPER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 25, 25, 25, 25, 25 },
	/* Init   Lower  Higher */
	{  6, 0,  0, 8,  12, 0 },	/* Hit points */
	{  5, 0,  9, 0,  13, 0 }		/* Energy */
},
{	"devil", "devilish", "devilhood", "Dev",
	{0, 0},
	PM_HUMANOID_DEVIL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(22), 18, 20, 20, 21, 16 },
	/* Init   Lower  Higher */
	{  5, 0,  0, 5,  3, 0 },	/* Hit points */
	{  3, 0,  5, 0,  5, 0 }		/* Energy */
},
{	"dictionary attack", "dictionary attack", "lauren epsom delirium", "Dic",
	{0, 0},
	PM_DICTIONARY_ATTACK, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"dinosaur", "dinosaur", "jurassic park", "Din",
	{0, 0},
	PM_WEAPONIZED_DINOSAUR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 14, 14, 15, 25, 12 },
	/* Init   Lower  Higher */
	{  10, 0,  0, 10,  10, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"dolgsman", "dolgsman", "dolgskind", "Dol",
	{0, 0},
	PM_PLAYER_DOLGSMAN, NON_PM, NON_PM, NON_PM,
	MH_HOBBIT | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HOBBIT, MH_HOBBIT, MH_GNOME|MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 20, 18, 8, 14 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  4, 0,  2, 1,  2, 0 }		/* Energy */
},
{	"doppelganger", "doppelganger", "doppelganger-kind", "Dop",
	{0, 0},
	PM_DOPPELGANGER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, MH_WERE, MH_ELF|MH_GNOME|MH_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 20, 20, 20, 15 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 1,  1, 0 },	/* Hit points */
	{  7, 0,  5, 0,  5, 0 }		/* Energy */
},
{	"dorian", "dorian", "explora-dora", "Dor",
	{0, 0},
	PM_DORIAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"dragon", "draconian", "dragonhood", "Dra",
	{0, 0},
	PM_HUMANLIKE_DRAGON, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(23), 18, 20, 16, 22, 18 },
	/* Init   Lower  Higher */
	{  6, 0,  0, 6,  2, 0 },	/* Hit points */
	{  3, 0,  3, 0,  2, 0 }		/* Energy */
},
{	"drow", "droven", "drovenkind", "Dro",
	{0, 0},
	PM_DROW, NON_PM, PM_DROW_MUMMY, PM_DROW_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ELF, MH_ELF, MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  18,    20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  7, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"dryad", "dryad", "dryadhood", "Dry",
	{0, 0},
	PM_HUMANOID_DRYAD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     16,     18, 18, 22, 14, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"dufflepud", "dufflepud", "duffling", "Duf",
	{0, 0},
	PM_DUFFLEPUD, NON_PM, PM_DWARF_MUMMY, PM_DWARF_ZOMBIE,
	MH_DWARF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_DWARF, 0, MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 10, 18, 13 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  2, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"dunadan", "dunadan", "dunedain", "Dun",
	{0, 0},
	PM_DUNADAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  3, 0 },	/* Hit points */
	{  3, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"duthol", "duthol", "dutholland", "Dut",
	{0, 0},
	PM_DUTHOL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 3,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"dwarf", "dwarven", "dwarvenkind", "Dwa",
	{0, 0},
	PM_DWARF, NON_PM, PM_DWARF_MUMMY, PM_DWARF_ZOMBIE,
	MH_DWARF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_DWARF, MH_DWARF|MH_GNOME, MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 16, 20, 20, 16 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  2, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"dynamo", "dynamo", "dynamo headquarters", "Dyn",
	{0, 0},
	PM_PLAYER_DYNAMO, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"egymid", "egymid", "egymides", "Egy",
	{0, 0},
	PM_EGYMID, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(20), 21, 21, 16, 12, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  3, 0,  3, 0,  4, 0 }		/* Energy */
},
{	"elemental", "elemental", "elementalhood", "Ele",
	{0, 0},
	PM_ELEMENTAL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_DWARF|MH_HUMAN|MH_ELF|MH_HOBBIT,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 15 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"elf", "elven", "elvenkind", "Elf",
	{0, 0},
	PM_ELF, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ELF, MH_ELF, MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  18,    20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  8, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"engchip", "engchip", "eng country", "Eng",
	{0, 0},
	PM_ENGCHIP, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 16, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"ent", "ent", "enthood", "Ent",
	{0, 0},
	PM_LOWER_ENT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 16, 18, 14, 24,  6 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 4,  5, 0 },	/* Hit points */
	{  2, 0,  3, 0,  4, 0 }		/* Energy */
},
{	"erosator", "erosator", "erosion system", "Ero",
	{0, 0},
	PM_EROSATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 25 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"etherealoid", "etherealoidic", "etherealoid-kind", "Eth",
	{0, 0},
	PM_ETHEREALOID, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{   18,    18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  3, 0,  3, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"evilvariant", "evilvariant", "Evil Variant", "Evi",
	{0, 0},
	PM_EVILVARIANT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"expert", "expert", "expertise", "Exp",
	{0, 0},
	PM_EXPERT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"extravator", "extravator", "extra aggravation", "Ext",
	{0, 0},
	PM_EXTRAVATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 20, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"fairy", "fairy", "fairy castle", "Fai",
	{0, 0},
	PM_PLAYER_FAIRY, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     16,    22, 22, 20,  6, 22 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  4, 0,  6, 0,  5, 0 }		/* Energy */
},
{	"fawn", "fawn", "fawning", "Faw",
	{0, 0},
	PM_FAWN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(20), 20, 20, 24, 12, 22 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"felid", "felid", "felinity", "Fel",
	{0, 0},
	PM_FELID, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 16, 25, 18, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  2, 0,  1, 0,  0, 0 }		/* Energy */
},
{	"feminizer", "feminizer", "feminism", "Fem",
	{0, 0},
	PM_FEMINIZER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"fenek", "fenek", "fenekhood", "Fen",
	{0, 0},
	PM_FENEK, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(60), 18, 18, 19, 18, 15 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 4,  4, 0 },	/* Hit points */
	{  2, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"fiend", "fiendish", "fiendly", "Fie",
	{0, 0},
	PM_SUCKING_FIEND, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_VAMPIRE | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_VAMPIRE, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 18, 18, 16, 10 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  4, 0,  4, 0,  3, 0 }		/* Energy */
},
{	"fixer", "fixer", "fixing techniques", "Fix",
	{0, 0},
	PM_FIXER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"frendian", "frendian", "frendia", "Fre",
	{0, 0},
	PM_FRENDIAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"fro", "fro", "froos", "Fro",
	{0, 0},
	PM_FRO, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"gastly", "ghast", "ghasthood", "Gha",
	{0, 0},
	PM_GASTLY, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 16, 18, 18, 18, 12 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"gavil", "gavilish", "devil's gate", "Gav",
	{0, 0},
	PM_GAVIL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(22), 18, 20, 20, 21, 16 },
	/* Init   Lower  Higher */
	{  5, 0,  0, 5,  3, 0 },	/* Hit points */
	{  3, 0,  5, 0,  5, 0 }		/* Energy */
},
{	"gelatinous cube", "gelatinous", "cubic gelatine", "Gel",
	{0, 0},
	PM_WEAPON_CUBE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 6 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 4,  8, 0 },	/* Hit points */
	{  5, 0,  5, 0,  5, 0 }		/* Energy */
},
{	"gerteut", "gerteut", "teumany", "Teu",
	{0, 0},
	PM_GERTEUT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"giant", "gigantic", "giganthood", "Gig",
	{0, 0},
	PM_GIGANT, NON_PM, PM_GIANT_MUMMY, PM_GIANT_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 15, 15, 14, 24, 18 },
	/* Init   Lower  Higher */
	{  5, 0,  0, 4,  4, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"glorkum", "glorkum", "glorkum 0 0 0's", "Glo",
	{0, 0},
	PM_PLAYER_GLORKUM, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     15,     23,  8, 19,  4,  17 },
	/* Init   Lower  Higher */
	{  4,  0,  0, 12,  0, 0 },	/* Hit points */
	{  29, 0,  1, 0,   7, 0 }		/* Energy */
},
{	"gnome", "gnomish", "gnomehood", "Gno",
	{0, 0},
	PM_GNOME, NON_PM, PM_GNOME_MUMMY, PM_GNOME_ZOMBIE,
	MH_GNOME | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_GNOME, MH_DWARF|MH_GNOME, MH_HUMAN|MH_HOBBIT,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50),19, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"goauld", "goauld", "stargate symbiosis", "Goa", /* by Demo, who illogically didn't call it by its proper name :P */
	{0, 0},
	PM_GOAULD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{      12,    25, 25, 25, 12, 15 },
	/* Init   Lower  Higher */
	{  2, 0,  1, 0,  0, 1 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 },	/* Energy */
},
{	"golden saint", "aureal", "aureale", "Aur",
	{0, 0},
	PM_AUREAL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 20, 18, 16, 20 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"golem", "golem", "golemity", "Gol",
	{0, 0},
	PM_PLAYER_GOLEM, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 14, 18, 20, 16 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"green slime", "slimy", "slimehood", "Sli",
	{0, 0},
	PM_PLAYER_SLIME, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{      18,    16, 12, 18, 15,  5 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  5, 0,  4, 0,  3, 0 }		/* Energy */
},
{	"gremlin", "gremlin", "grem county", "Gre",
	{0, 0},
	PM_PLAYER_GREMLIN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 18, 20, 18, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  2, 0,  2, 0,  3, 0 }		/* Energy */
},
{	"greuro", "greuro", "greuro zone", "Gru",
	{0, 0},
	PM_GREURO, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"grid bug", "gridbug", "gridbughood", "Grd",
	{0, 0},
	PM_WEAPON_BUG, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_DWARF|MH_HUMAN|MH_ELF|MH_HOBBIT,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  5, 0,  0, 7,  9, 0 },	/* Hit points */
	{  5, 0,  7, 0,  9, 0 }		/* Energy */
},
{	"grouper", "grouper", "group spawn", "Gro",
	{0, 0},
	PM_GROUPER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"hallucinator", "hallucinator", "acid trip", "Hal",
	{0, 0},
	PM_HALLUCINATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"hardmoder", "hardmoder", "hard mode", "Har",
	{0, 0},
	PM_HARDMODER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"haxor", "haxorish", "haxorhood", "Hax",
	{0, 0},
	PM_HAXOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  4, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"HC alien", "HC alien", "hardcore planet", "Hca",
	{0, 0},
	PM_HC_ALIEN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 25, 18, 18, 3 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  2, 0,  2, 0,  5, 0 }		/* Energy */
},
{	"hemi-doppelganger", "hemi-doppelganger", "hemi-doppelganger-kind", "Hem",
	{0, 0},
	PM_HEMI_DOPPELGANGER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, MH_WERE, MH_ELF|MH_GNOME|MH_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 19, 19, 19, 19, 16 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 1,  1, 0 },	/* Hit points */
	{  7, 0,  5, 0,  5, 0 }		/* Energy */
},
{	"hemophage", "hemophage", "hemophagehood", "Hep",
	{0, 0},
	PM_HEMOPHAGE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 20, 20 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  2, 0,  3, 0,  4, 0 }		/* Energy */
},
{	"herald", "herald", "heraldicdom", "Hrl",
	{0, 0},
	PM_HERALD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 16, 16, 16, 20, 16 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"herbalist", "herbalist", "herbalism", "Hrb",
	{0, 0},
	PM_HERBALIST, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{      15,    18, 18, 18, 14, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"heretic", "heretic", "hereticism", "Her",
	{0, 0},
	PM_HERETIC, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 17 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  5, 0 },	/* Hit points */
	{  3, 0,  3, 0,  5, 0 }		/* Energy */
},
{	"hobbit", "hobbit", "hobbit-kind", "Hob",
	{0, 0},
	PM_HOBBIT, NON_PM, NON_PM, NON_PM,
	MH_HOBBIT | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HOBBIT, MH_HOBBIT, MH_GNOME|MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  18,     16, 18, 18, 20, 20 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  4, 0,  2, 1,  2, 0 }		/* Energy */
},
{	"homicider", "homicidal", "homiciderhood", "Hom",
	{0, 0},
	PM_HOMICIDER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 20, 20, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"hound", "hound", "zephyr hounds", "Hou",
	{0, 0},
	PM_PLAYER_HOUND, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_HUMAN,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 19, 17 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  4, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"hidden elf", "hidden elven", "hidden elvenkind", "Hid",
	{0, 0},
	PM_HIDDEN_ELF, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ELF, MH_ELF, MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{    18,    20, 20, 20, 15, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  8, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"human", "human", "humanity", "Hum",
	{"man", "woman"},
	PM_HUMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"hunkajunk", "hunkajunk", "Hunk of Junk", "Hun", /* by Demo */
	{0, 0},
	PM_HUNKAJUNK, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"hybridragon", "half-dragon", "bastard dragons", "Hyb",
	{0, 0},
	PM_HYBRIDRAGON, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 18, 19, 17, 20, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  1, 0 },	/* Hit points */
	{  2, 0,  2, 0,  1, 0 }		/* Energy */
},
{	"hypothermic", "hypothermic", "hypothermia", "Hyp",
	{0, 0},
	PM_HYPOTHERMIC, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 19, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"illithid", "illithid", "mindflayerhood", "Ill",
	{0, 0},
	PM_ILLITHID, NON_PM, PM_DWARF_MUMMY, PM_DWARF_ZOMBIE,
	MH_DWARF | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_DWARF, MH_DWARF, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 25, 20, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 2,  2, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"immunizer", "immunizer", "immunizing", "Imm",
	{0, 0},
	PM_IMMUNIZER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(80), 15, 15, 18, 20, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"imp", "impish", "imphood", "Wim", /* "weapon imp" because there's already the imperial race --Amy */
	{0, 0},
	PM_WEAPON_IMP, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     16,     16, 16, 18, 12, 14 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"imperial", "imperial", "imperialism", "Imp",
	{0, 0},
	PM_IMPERIAL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(90), 18, 18, 18, 17, 20 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  5, 0 },	/* Hit points */
	{  3, 0,  3, 0,  5, 0 }		/* Energy */
},
{	"incantifier", "incantifier", "wanterkind", "Inc",
	{0, 0},
	PM_INCANTIFIER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  0, 2 },	/* Hit points */
	{  0, 0,  10, 0, 10, 0 },		/* Energy */
},
{	"incorporealoid", "incorporealoidic", "incorporealoid-kind", "Ico",
	{0, 0},
	PM_INCORPOREALOID, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{   18,    18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  3, 0,  3, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"indraenian", "indraenian", "indraenia", "Ind",
	{0, 0},
	PM_INDRAENIAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"inheritor", "inheritor", "heritage", "Inh",
	{0, 0},
	PM_INHERITOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"inka", "inka", "inka civilization", "Ink",
	{0, 0},
	PM_INKA, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ELF, MH_ELF, 0,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 22, 18, 20, 24 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 2,  3, 0 },	/* Hit points */
	{  4, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"insectoid", "insectoid", "insecthood", "Ins",
	{0, 0},
	PM_INSECTOID, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 20, 16, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"iraha", "iraha", "ira-hate", "Ira",
	{0, 0},
	PM_IRAHA, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 15, 15, 19, 18, 16 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"ironman", "ironman", "ironman event", "Iro",
	{0, 0},
	PM_IRONMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 20, 19 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 5,  5, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"irritator", "irritator", "irritation", "Irr",
	{0, 0},
	PM_IRRITATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 20, 18, 17 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"itaque", "itaque", "itaquely", "Ita",
	{0, 0},
	PM_ITAQUE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 25, 18 },
	/* Init   Lower  Higher */
	{  5, 0,  0, 5,  5, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"jabberwock", "jabberwock", "jabberwock poem", "Jab",
	{0, 0},
	PM_PLAYER_JABBERWOCK, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(24), 16, 18, 19, 24, 16 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 8,  7, 0 },	/* Hit points */
	{  2, 0,  2, 0,  3, 0 }		/* Energy */
},
{	"japura", "japura", "japuray", "Jap",
	{0, 0},
	PM_JAPURA, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"java", "java", "javaland", "Jav",
	{0, 0},
	PM_JAVA, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"jelly", "jelly", "jellyhood", "Jel",
	{0, 0},
	PM_JELLY, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     18,     20, 21, 18, 16, 12 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 4,  8, 0 },	/* Hit points */
	{  5, 0,  5, 0,  5, 0 }		/* Energy */
},
{	"khajiit", "khajiit", "cathood", "Kha",
	{0, 0},
	PM_KHAJIIT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 22, 18, 17 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 3,  3, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"klackon", "klackon", "klackchengband", "Kla",
	{0, 0},
	PM_KLACKON, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20),  6,  6, 25, 20,  6 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 3,  2, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"knowledgable", "knowledgable", "fountain of knowledge", "Kno",
	{0, 0},
	PM_KNOWLEDGABLE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{        12, 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"kobold", "koboldic", "kobolthood", "Kob",
	{0, 0},
	PM_KOBOLT, NON_PM, PM_KOBOLD_MUMMY, PM_KOBOLD_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 17, 17, 18, 19, 16 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 2,  2, 0 },	/* Hit points */
	{  3, 0,  2, 0,  1, 0 }		/* Energy */
},
{	"kop", "kop", "kophood", "Kop",
	{0, 0},
	PM_KOP, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 19, 20, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"koronst", "koronst", "koronster cave", "Kor",
	{0, 0},
	PM_KORONST, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"kutar", "kutar", "emotionless expressions", "Kut",
	{0, 0},
	PM_KUTAR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(60), 17, 17, 17, 17, 23 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 1,  1, 0 },	/* Hit points */
	{  2, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"leprechaun", "leprechaun", "leprechaunism", "Lep",
	{0, 0},
	PM_HUMANOID_LEPRECHAUN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     18,     18, 18, 25, 16, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 1,  1, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"levelscaler", "levelscaler", "scaling system", "Lvl",
	{0, 0},
	PM_LEVELSCALER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"levitator", "levitating", "levitatorship", "Lev",
	{0, 0},
	PM_LEVITATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 20, 20, 24, 18, 20 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 6,  4, 0 },	/* Hit points */
	{  8, 0,  6, 0,  4, 0 }		/* Energy */
},
{	"lich", "lich", "lichhood", "Lic",
	{0, 0},
	PM_LICH_WARRIOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(22), 20, 20, 14, 18, 10 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"listener", "listener", "listening outpost", "Lis",
	{0, 0},
	PM_LISTENER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 19, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"lizardman", "lizard", "lizardhood", "Liz",
	{0, 0},
	PM_LIZARDMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 18, 18, 19, 20, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 2,  3, 0 },	/* Hit points */
	{  1, 0,  1, 0,  2, 0 }		/* Energy */
},
{	"loli", "loli", "loli culture", "Lol",
	{0, 0},
	PM_LOLI, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 18, 18, 14, 22 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"lycanthrope", "lycanthropic", "lycanthropehood", "Lyc",
	{0, 0},
	PM_HUMAN_WEREWOLF, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_WERE | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_WERE, 0, MH_ELF|MH_GNOME|MH_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 15, 15, 20, 19, 15 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 2,  2, 0 },	/* Hit points */
	{  5, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"mactheist", "mactheist", "atheistonia", "Mac",
	{0, 0},
	PM_MACTHEIST, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"magyar", "magyar", "non gladly", "Mag",
	{0, 0},
	PM_MAGYAR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"maia", "maian", "maiahood", "Mai",
	{0, 0},
	PM_MAIA, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 19, 18, 20, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"matrayser", "matrayser", "material raise", "Mat",
	{0, 0},
	PM_MATRAYSER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"matrixer", "matrixer", "The Matrix", "Max",
	{0, 0},
	PM_MATRIXER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"maymes", "maymes", "maymestry", "May",
	{0, 0},
	PM_MAYMES, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 19, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"mazewalker", "mazewalking", "mazemastership", "Maz",
	{0, 0},
	PM_MAZEWALKER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"mazin", "mazin", "magic enhancement", "Mzi",
	{0, 0},
	PM_MAZIN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 25, 22, 18, 17, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 1,  1, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"metal", "metal", "heavy metal", "Met",
	{0, 0},
	PM_METAL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  18, 16, 18, 16, 16, 25 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"mimic", "mimic", "mimicry", "Mim",
	{0, 0},
	PM_PLAYER_MIMIC, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 18, 18, 20, 14, 16 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 4,  5, 0 },	/* Hit points */
	{  3, 0,  4, 0,  5, 0 }		/* Energy */
},
{	"minimalist", "minimalist", "minus world", "Min",
	{0, 0},
	PM_MINIMALIST, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, MH_ELF, MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 20, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"missingno", "missing", "missing faction", "Mis",
	{0, 0},
	PM_MISSINGNO, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(23), 17, 23,  9, 11,  4 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 6,  7, 0 },	/* Hit points */
	{  6, 0,  6, 0,  6, 0 }		/* Energy */
},
{	"mongung", "mongung", "mongoss", "Mog",
	{0, 0},
	PM_MONGUNG, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 6, 6, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"monkey", "monkey", "monkeydom", "Mon",
	{0, 0},
	PM_HUMAN_MONKEY, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 10, 10, 20, 19, 13 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 2,  2, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"monster", "monster", "monster generator", "Mns",
	{0, 0},
	PM_MANSTER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 16, 16, 16, 20, 16 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"moon elf", "moonelven", "moonelvenkind", "Moo",
	{0, 0},
	PM_MOON_ELF, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ELF, MH_ELF, MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{       17, 21, 21, 19, 14, 19 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  6, 0,  2, 0,  1, 0 }		/* Energy */
},
{	"mould", "mouldic", "mouldhood", "Mou",
	{0, 0},
	PM_MOULD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 16, 16, 12 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"mummy", "mummified", "mummyhood", "Mum",
	{0, 0},
	PM_MUMMY, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 16, 18, 18, 11 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"mushroom", "mushroom", "mushroom kingdom", "Mus",
	{0, 0},
	PM_PLAYER_MUSHROOM, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 6, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 0,  0, 0 },	/* Hit points */
	{  2, 0,  3, 0,  5, 0 }		/* Energy */
},
{	"myrkalfr", "myrkalfar", "myrkalfrkind", "Myr",
	{0, 0},
	PM_PLAYER_MYRKALFR, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ELF, MH_ELF, MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  18,    20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 },	/* Energy */
},
{	"naga", "naga", "nagahood", "Nag",
	{0, 0},
	PM_HUMANLIKE_NAGA, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 20, 20, 19, 20, 14 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  1, 0 },	/* Hit points */
	{  2, 0,  2, 0,  3, 0 }		/* Energy */
},
{	"nastinator", "nastinator", "nastiness", "Nas",
	{0, 0},
	PM_NASTINATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  4, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"navi", "navi", "navihood", "Nav",
	{0, 0},
	PM_NAVI, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 22, 18, 20 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"nemesis", "nemesis", "quest nemesis land", "Nem",
	{0, 0},
	PM_NEMESIS, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  7, 0,  0, 7,  7, 0 },	/* Hit points */
	{  7, 0,  7, 0,  7, 0 }		/* Energy */
},
{	"nibelung", "nibelung", "nibelung lieo", "Nib",
	{0, 0},
	PM_PLAYER_NIBELUNG, NON_PM, PM_DWARF_MUMMY, PM_DWARF_ZOMBIE,
	MH_DWARF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_DWARF, 0, MH_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 16, 20, 20, 16 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  2, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"nord", "nordish", "nordhood", "Nor",
	{0, 0},
	PM_NORD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 17, 17, 17, 19, 17 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  2, 0 },	/* Hit points */
	{  3, 0,  2, 0,  1, 0 }		/* Energy */
},
{	"null", "null", "null", "Nul",
	{0, 0},
	PM_NULL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"nymph", "nymphian", "nymphhood", "Nym", /* suggestion by BellisColdwine */
	{0, 0},
	PM_NYMPH, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 25, 14, 25 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 1,  1, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"octopode", "octopode", "octopussy", "Oct",
	{0, 0},
	PM_OCTOPODE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 20, 14, 10 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"ogre", "ogrotic", "ogrohood", "Ogr",
	{0, 0},
	PM_OGRO, NON_PM, PM_OGRE_MUMMY, PM_OGRE_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 12, 12, 16, 24,  8 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  3, 0 },	/* Hit points */
	{  3, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"orc", "orcish", "orcdom", "Orc",
	{0, 0},
	PM_ORC, NON_PM, PM_ORC_MUMMY, PM_ORC_ZOMBIE,
	MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ORC, 0, MH_HUMAN|MH_ELF|MH_DWARF|MH_HOBBIT,
	/*  Str    Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50),16, 16, 18, 18, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"outsider", "outsider", "far planets", "Out",
	{0, 0},
	PM_OUTSIDER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 20, 20, 20, 20, 20 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  4, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"peacemaker", "peacemaker", "activism", "Pea",
	{0, 0},
	PM_PEACEMAKER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 12, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"pervert", "pervert", "perversion", "Per",
	{0, 0},
	PM_PERVERT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"phantom", "phantom", "phantomhood", "Pha",
	{0, 0},
	PM_PHANTOM_GHOST, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 14 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"piece", "piece", "chess pieces", "Pic",
	{0, 0},
	PM_PIECE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 17, 17, 18, 20, 16 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"piercer", "piercer", "piercing studio", "Pie",
	{0, 0},
	PM_PIERCER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 22, 19, 20, 14 },
	/* Init   Lower  Higher */
	{  5, 0,  0, 2,  1, 0 },	/* Hit points */
	{  2, 0,  3, 0,  2, 0 }		/* Energy */
},
{	"poisoner", "poisoner", "poisoning", "Poi",
	{0, 0},
	PM_POISONER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 20, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"polyinitor", "polyinitor", "polymorph initialization", "Pol",
	{0, 0},
	PM_POLYINITOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(21), 19, 19, 19, 21, 14 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"problematic", "problematic", "problems", "Pro",
	{0, 0},
	PM_PROBLEMATIC, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"quantum mechanic", "quantum mechanic", "quants", "Qua",
	{0, 0},
	PM_PLAYER_MECHANIC, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(70), 19, 17, 17, 17, 15 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 3,  0, 0 },	/* Hit points */
	{  1, 0,  3, 0,  1, 0 }		/* Energy */
},
{	"race that doesn't exist", "nonexistant", "pork realm", "Nex",
	{0, 0},
	PM_RACE_THAT_DOESN_T_EXIST, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     18,     18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"race X", "x-race", "x-race", "Xra",
	{0, 0},
	PM_RACE_X, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 16, 18, 16, 21, 14 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  4, 0 },	/* Hit points */
	{  8, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"randomizer", "randomized", "rng worship", "Ran",
	{0, 0},
	PM_RANDOMIZER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"redditor", "redditor", "reddit community", "Rdt",
	{0, 0},
	PM_REDDITOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 20, 18, 18 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"redguard", "redguardian", "redguardhood", "Red",
	{0, 0},
	PM_REDGUARD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 14, 12, 18, 22, 18 },
	/* Init   Lower  Higher */
	{  10, 0,  0, 6,  4, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"releasier", "releasier", "Super Releasio 64", "Rel",
	{0, 0},
	PM_RELEASIER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(40), 18, 18, 14, 18, 16 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 1,  1, 0 },	/* Hit points */
	{  2, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"reticulan", "reticulan", "ZAPM planet", "Ret",
	{0, 0},
	PM_RETICULAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 24, 24, 18, 20, 10 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  10, 0, 8, 0,  12, 0}		/* Energy */
},
{	"rodneyan", "rodneyan", "rodneyhood", "Rod",
	{0, 0},
	PM_RODNEYAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(21), 25, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  4, 0 },	/* Hit points */
	{  7, 0,  7, 0,  7, 0 }		/* Energy */
},
{	"rohirrim", "rohirrim", "rohan", "Roh",
	{0, 0},
	PM_ROHIRRIM, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 16, 18, 20, 19 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"roommate", "roommate", "room sharing", "Roo",
	{0, 0},
	PM_ROOMMATE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 20 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"rougelike", "rougelike", "angles of satin", "Rog",
	{0, 0},
	PM_ROUGELIKE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 20, 17, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"rusmot", "rusmot", "motherus", "Rus",
	{0, 0},
	PM_RUSMOT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"salamander", "salamander", "salamanderhood", "Sal",
	{0, 0},
	PM_PLAYER_SALAMANDER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 18, 18, 16, 19, 16 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 3,  4, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"satre", "satre", "satredom", "Sat",
	{0, 0},
	PM_SATRE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(20), 22, 22, 25, 12, 25 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  6, 0 },	/* Hit points */
	{  4, 0,  6, 0,  9, 0 }		/* Energy */
},
{	"scriptor", "scriptor", "jonadab's perlscript", "Scr",
	{0, 0},
	PM_SCRIPTOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"scurrier", "scurrier", "scurriership", "Scu",  /* by jonadab */
	{0, 0},
      PM_SCURRIER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     15,     18, 18, 20, 14, 18 },
	/* Init   Lower  Higher */
      {  1, 0,  0, 1,  1, 0 },	/* Hit points */
      {  1, 0,  2, 0,  2, 0 } 	/* Energy */
},
{	"sea elf", "sea elven", "sea elvenkind", "Sea",
	{0, 0},
	PM_SEA_ELF, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ELF, MH_ELF, MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{   18,    20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  6, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"segfaulter", "segfaulting", "SIGSEGV", "Seg",
	{0, 0},
	PM_SEGFAULTER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"senser", "senser", "senserhood", "Sen",
	{0, 0},
	PM_SENSER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 19, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"serb", "serb", "sergro", "Ser",
	{0, 0},
	PM_SERB, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"sheep", "sheep", "sheepishness", "Shp",
	{0, 0},
	PM_PLAYER_SHEEP, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(80), 15, 16, 16, 18, 16 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 2,  2, 0 },	/* Hit points */
	{  1, 0,  1, 0,  4, 0 }		/* Energy */
},
{	"shell", "shell", "shelldom", "She",
	{0, 0},
	PM_SHELL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 19, 18, 24, 16 },
	/* Init   Lower  Higher */
	{  5, 0,  0, 5,  5, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"shoe", "shoe", "wonderful lady shoes", "Sho",
	{0, 0},
	PM_SHOE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 18, 18, 19, 18, 25 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 5,  6, 0 },	/* Hit points */
	{  3, 0,  4, 0,  7, 0 }		/* Energy */
},
{	"sinner", "sinful", "sinnerhood", "Sin",
	{0, 0},
	PM_SINNER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 24, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"skeleton", "skeleton", "skeletal remains", "Ske",
	{0, 0},
	PM_PLAYER_SKELETON, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 16, 16, 18, 21, 10 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"skillor", "skillor", "pure skill", "Ski",
	{0, 0},
	PM_SKILLOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"snail", "snail", "snailing", "Snl",
	{0, 0},
	PM_ELONA_SNAIL, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{     18,     18, 18, 18, 20, 18 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 4,  0, 0 },	/* Hit points */
	{  8, 0,  4, 0,  0, 0 }		/* Energy */
},
{	"snakeman", "snakey", "snakehood", "Sna",
	{0, 0},
	PM_SNAKEMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 18, 18, 20, 18, 14 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 2,  2, 0 },	/* Hit points */
	{  2, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"sokosolver", "sokosolver", "sokoban league", "Sok",
	{0, 0},
	PM_SOKOSOLVER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 19, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"soviet", "soviet", "soviet union", "Sov",
	{0, 0},
	PM_SOVIET, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HUMAN|MH_DWARF|MH_HOBBIT|MH_WERE|MH_VAMPIRE,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ 16, 16, 16, 16, 16, 16 },	/* in Soviet Russia, races don't automatically have a max of 25 for every attribute */
	/* new limit values for the six attributes by Amy */
	{     15,     15, 15, 15, 15, 15 }, /* intentionally sucky :P */
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"spammer", "spammer", "spam mail folder", "Spm",
	{0, 0},
	PM_SPAMMER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"spard", "spard", "spardin", "Spa",
	{0, 0},
	PM_SPARD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"specialist", "specialist", "speciality", "Spe",
	{0, 0},
	PM_SPECIALIST, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"spiderman", "spider", "spiderhood", "Spi",
	{0, 0},
	PM_SPIDERMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 20, 16, 19 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 2,  3, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"spirit", "spiritual", "spirithood", "Esp", /* "spi" = spider, so I'm abbreviating "esprit" instead --Amy */
	{0, 0},
	PM_SPIRIT, NON_PM, NON_PM, NON_PM,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(30), 18, 18, 18, 12, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"spriggan", "spriggan", "sprigganship", "Spr",
	{0, 0},
	PM_SPRIGGAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{      18,    18, 20, 20, 12, 19 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  4, 0,  5, 0,  5, 0 }		/* Energy */
},
{	"stabilisator", "stabilisator", "stability", "Stb",
	{0, 0},
	PM_STABILISATOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"stairseeker", "stairseeker", "search for the stairs", "Sta",
	{0, 0},
	PM_STAIRSEEKER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"sticker", "sticky", "stickiness", "Sti",
	{0, 0},
	PM_STICKER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 19, 19, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"sustainer", "sustainer", "sustainability", "Sus",
	{0, 0},
	PM_SUSTAINER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 25, 25, 25, 25, 25 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"suxxor", "suxxor", "suxxorhood", "Sux",
	{0, 0},
	PM_SUXXOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  6, 0,  0, 6,  6, 0 },	/* Hit points */
	{  6, 0,  6, 0,  6, 0 }		/* Energy */
},
{	"swikni", "swikni", "swikzerni", "Swi",
	{0, 0},
	PM_SWIKNI, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 19, 19, 24, 20, 17 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 3,  2, 0 },	/* Hit points */
	{  2, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"sylph", "sylph", "sylphood", "Syl",
	{0,0},
	PM_SYLPH, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*   Str    Int Wis Dex Con Cha */
	{    3,      3,  3,  3,  3,  3},
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 20, 20, 18, 24 },
	/* Init   Lower  Higher */
	{2, 0, 1, 1, 1, 1},        /* Hit points */
	{2, 0, 3, 0, 2, 2} /* Energy */
},
{	"tayiin", "tayiin", "tayiind", "Tay",
	{0, 0},
	PM_TAYIIN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 19, 10, 19 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"techless", "techless", "techlessness", "Tec",
	{0, 0},
	PM_TECHLESS, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"thunderlord", "thunderlord", "pern", "Thu",
	{0, 0},
	PM_THUNDERLORD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 19, 19, 18, 20, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  3, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"tonberry", "tonberry", "poschengband's mascot", "Ton",
	{0, 0},
	PM_TONBERRY, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(23), 18, 16, 16, 16, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 2,  2, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"trainer", "trainer", "training", "Trn",
	{0, 0},
	PM_TRAINER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 18, 18, 18, 19, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"transformer", "transformer", "transferhood", "Tra",
	{0, 0},
	PM_TRANSFORMER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 20, 17, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"trapper", "trapper", "trapperhood", "Trp",
	{0, 0},
	PM_WEAPON_TRAPPER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 22, 18, 18 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 8,  10, 0 },	/* Hit points */
	{  10, 0,  8, 0,  8, 0 }		/* Energy */
},
{	"troll", "troll", "trollhood", "Tro",
	{0, 0},
	PM_TROLLOR, NON_PM, PM_TROLL_MUMMY, PM_TROLL_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(22), 12, 12, 15, 24,  8 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"tumblrer", "tumblrer", "tumblr ownz joo", "Tum", /* idea by kerio */
	{0, 0},
	PM_TUMBLRER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"turmene", "turmene", "turmeney", "Tme",
	{0, 0},
	PM_TURMENE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 17, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"turtle", "turtle", "turtleship", "Tur", /* Khor wants to be a ninja turtle, now he can. :) --Amy */
	{0, 0},
	PM_TURTLE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 25, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 4,  5, 0 },	/* Hit points */
	{  2, 0,  3, 0,  4, 0 }		/* Energy */
},
{	"umber hulk", "umber", "uber empire", "Umb",
	{0, 0},
	PM_PLAYER_HULK, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 20, 16 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"unalignment thing", "unaligned", "unalignmenthood", "Una",
	{0, 0},
	PM_UNALIGNMENT_THING, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 20, 20, 18, 20, 14 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  4, 0 },	/* Hit points */
	{  7, 0,  7, 0,  7, 0 }		/* Energy */
},
{	"unbalancor", "unbalancor", "unbalanced game", "Unb",
	{0, 0},
	PM_UNBALANCOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"undefined", "undefined", "undefinement", "Und",
	{0, 0},
	PM_UNDEFINED_CREATURE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"ungenomold", "ungenomoldic", "ungenomoldhood", "Ung",
	{0, 0},
	PM_UNGENOMOLD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 18, 18, 18, 20, 13 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  4, 0 },	/* Hit points */
	{  7, 0,  7, 0,  7, 0 }		/* Energy */
},
{	"unicorn", "unicorn", "unihorning", "Uni",
	{0, 0},
	PM_PLAYER_UNICORN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 16, 16, 20, 15, 24 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"unmagic", "unmagic", "magic void", "Unm",
	{0, 0},
	PM_UNMAGIC_FISH, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 16, 16, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"urian", "urian", "uria", "Uri",
	{0, 0},
	PM_URIAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"vamgoyle", "vamgoyle", "vamgoylehood", "Vgo",
	{0, 0},
	PM_VAMGOYLE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_VAMPIRE | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_VAMPIRE, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 16 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 2,  2, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"vampire", "vampiric", "vampirehood", "Vam",
	{0, 0},
	PM_VAMPIRE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_VAMPIRE | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_VAMPIRE, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 18, 18, 20, 20, 20 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  5, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"veela", "veela", "veelahood", "Vee",
	{0, 0},
	PM_VEELA, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 19, 16, 19, 18, 24 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"venture capitalist", "venturean", "capitalism", "Ven",
	{0, 0},
	PM_VENTURE_CAPITALIST, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF|MH_HOBBIT|MH_DWARF|MH_HUMAN,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 25 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"vietis", "vietis", "vietem", "Vie",
	{0, 0},
	PM_VIETIS, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  3, 0 }		/* Energy */
},
{	"viking", "viking", "greenland", "Vik",
	{0, 0},
	PM_VIKING, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 17, 17, 18, 20, 16 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  3, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"vortex", "vortex", "vortexhood", "Vor",
	{0, 0},
	PM_VORTEX, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 19, 19, 17 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  4, 0 },	/* Hit points */
	{  4, 0,  3, 0,  4, 0 }		/* Energy */
},
{	"warper", "warping", "warpzone", "War",
	{0, 0},
	PM_WARPER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"wind inhabitant", "wind", "windy well", "Win",
	{0, 0},
	PM_WIND_INHABITANT, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"wisp", "wisp", "wisp kingdom", "Wis",
	{0, 0},
	PM_WISP, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(50), 20, 20, 18, 16, 15 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  8, 0,  8, 0,  8, 0 }		/* Energy */
},
{	"wookie", "wookie", "bedside carpet station", "Woo",
	{0, 0},
	PM_WOOKIE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  16, 9, 9, 12, 15, 5 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 0,  1, 0 },	/* Hit points */
	{  1, 0,  0, 0,  1, 0 }		/* Energy */
},
{	"worm that walks", "worm", "wormhood", "Wor",
	{0, 0},
	PM_WORM_THAT_WALKS, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF|MH_GNOME|MH_HOBBIT|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 18, 18, 18, 18, 12 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  4, 0,  4, 0,  4, 0 }		/* Energy */
},
{	"wraith", "wraith", "wraithhood", "Wra",
	{0, 0},
	PM_HUMAN_WRAITH, NON_PM, NON_PM, NON_PM,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(20), 20, 20, 18, 20, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"xorn", "xorn", "xorndom", "Xor",
	{0, 0},
	PM_WEAPON_XORN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(19), 17, 17, 17, 19, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  2, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"yeek", "yeek", "yeekdom", "Yee",
	{0, 0},
	PM_YEEK, NON_PM, PM_ORC_MUMMY, PM_ORC_ZOMBIE,
	MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_ORC, 0, MH_HUMAN|MH_ELF|MH_DWARF|MH_HOBBIT,
	/*  Str    Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  16,     16, 16, 16, 16, 14 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
{	"yokuda", "yokuda", "yokudahood", "Yok",
	{0, 0},
	PM_REDGUARD, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 15, 14, 18, 20, 18 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 4,  3, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
{	"yugger", "yugger", "yugavia", "Yug",
	{0, 0},
	PM_YUGGER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{     18,     18, 18, 18, 18, 18 }, /* hard cap! */
	/* new limit values for the six attributes by Amy */
	{  STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
{	"yuki-onna", "yuki-onnic", "feyhood", "Yuk",
	{0, 0},
	PM_YUKI_PLAYA, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_ELF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  16,     18, 18, 20, 14, 20 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 0,  0, 0 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 }		/* Energy */
},
{	"zruty", "zruty", "save the zruties", "Zru",
	{0, 0},
	PM_PLAYER_ZRUTY, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC|MH_ELF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25), 125, 125, 125, 125, 125 },
	/* new limit values for the six attributes by Amy */
	{  STR19(22), 12, 15, 17, 20, 14 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 6,  5, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
/* Array terminator */
{ 0, 0, 0, 0 }};


/* The player's race, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Race urace =
{	"something", "undefined", "something", "Xxx",
	{0, 0},
	NON_PM, NON_PM, NON_PM, NON_PM,
	0, 0, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 25, 25, 25, 25, 25 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
};
struct Race ustartrace =
{	"something", "undefined", "something", "Xxx",
	{0, 0},
	NON_PM, NON_PM, NON_PM, NON_PM,
	0, 0, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* new limit values for the six attributes by Amy */
	{  STR19(25), 25, 25, 25, 25, 25 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
};

/* Table of all genders */
const struct Gender genders[] = {
	{"male",	"he",	"him",	"his",	"Mal",	ROLE_MALE},
	{"female",	"she",	"her",	"her",	"Fem",	ROLE_FEMALE},
	{"neuter",	"it",	"it",	"its",	"Ntr",	ROLE_NEUTER}
};

#ifdef MAC_MPW
const size_t maxGender = sizeof genders/sizeof genders[0];
#endif /* MAC_MPW */

/* Table of all alignments */
const struct Align aligns[] = {
	{"law",		"lawful",	"Law",	ROLE_LAWFUL,	A_LAWFUL},
	{"balance",	"neutral",	"Neu",	ROLE_NEUTRAL,	A_NEUTRAL},
	{"chaos",	"chaotic",	"Cha",	ROLE_CHAOTIC,	A_CHAOTIC},
	{"evil",	"unaligned",	"Una",	0,		A_NONE}
};

STATIC_DCL char * promptsep(char *, int);
STATIC_DCL int role_gendercount(int);
STATIC_DCL int race_alignmentcount(int);

/* used by str2XXX() */
static char NEARDATA randomstr[] = "random";

#ifdef MAC_MPW
const size_t maxAlign = sizeof aligns/sizeof aligns[0];
#endif /* MAC_MPW */

boolean
validrole(rolenum)
	int rolenum;
{
	return (rolenum >= 0 && rolenum < SIZE(roles)-1);
}


int
randrole()
{
	return (rn2(SIZE(roles)-1));
}

void
rivalroleinit()
{
	int rivalrolenum = randrole();
	flags.rivalsavenum = rivalrolenum;

	u.rivalcode = roles[rivalrolenum].filecode;
	u.rivalneminum = roles[rivalrolenum].neminum;
	u.rivalenemy1num = roles[rivalrolenum].enemy1num;
	u.rivalenemy2num = roles[rivalrolenum].enemy2num;
	u.rivalenemy1sym = roles[rivalrolenum].enemy1sym;
	u.rivalenemy2sym = roles[rivalrolenum].enemy2sym;
}

void
rivalrolerestore()
{
	int rivalrolenum = flags.rivalsavenum;

	u.rivalcode = roles[rivalrolenum].filecode;
	u.rivalneminum = roles[rivalrolenum].neminum;
	u.rivalenemy1num = roles[rivalrolenum].enemy1num;
	u.rivalenemy2num = roles[rivalrolenum].enemy2num;
	u.rivalenemy1sym = roles[rivalrolenum].enemy1sym;
	u.rivalenemy2sym = roles[rivalrolenum].enemy2sym;
}

int
str2role(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; roles[i].name.m; i++) {
	    /* Does it match the male name? */
	    if (!strncmpi(str, roles[i].name.m, len))
		return i;
	    /* Or the female name? */
	    if (roles[i].name.f && !strncmpi(str, roles[i].name.f, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, roles[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validrace(rolenum, racenum)
	int rolenum, racenum;
{
	/* Assumes validrole */
	/* WAC -- checks ROLE_GENDMASK and ROLE_ALIGNMASK as well (otherwise, there 
	 * might not be an allowed gender or alignment for that role
	 */
	return (racenum >= 0 && racenum < SIZE(races)-1 &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_RACEMASK) &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_GENDMASK) &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_ALIGNMASK));
}


int
randrace(rolenum)
	int rolenum;
{
	int i, n = 0;

	/* Count the number of valid races */
	for (i = 0; races[i].noun; i++)
/*	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK)*/
	    if (validrace(rolenum,i))
	    	n++;

	/* Pick a random race */
	/* Use a factor of 100 in case of bad random number generators */
	if (n) n = rn2(n*100)/100;
	for (i = 0; races[i].noun; i++)
/*	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK) {*/
	    if (validrace(rolenum,i)) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role has no permitted races? */
	return (rn2(SIZE(races)-1));
}

/*
 * [ALI] Find the player equivalent race for a monster from its M2 flags.
 */

int
mrace2race(mflags2)
	int mflags2;
{
	int i;

	/* Look for a race with the correct selfmask */
	for (i = 0; races[i].noun; i++)
	    if (mflags2 & races[i].selfmask) {
		/* Where more than one player race has the same monster race,
		 * return the base race.
		 */
		if (mflags2 & MH_HUMAN && races[i].malenum != PM_HUMAN)
		    continue;
		if (mflags2 & MH_ELF && races[i].malenum != PM_ELF)
		    continue;
		return i;
	    }
	return ROLE_NONE;
}

int
str2race(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; races[i].noun; i++) {
	    /* Does it match the noun? */
	    if (!strncmpi(str, races[i].noun, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, races[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validgend(rolenum, racenum, gendnum)
	int rolenum, racenum, gendnum;
{
	/* Assumes validrole and validrace */
	return (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		(roles[rolenum].allow & races[racenum].allow &
		 genders[gendnum].allow & ROLE_GENDMASK));
}


int
randgend(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid genders */
	for (i = 0; i < ROLE_GENDERS; i++)
/*	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK) */
	    if (validgend(rolenum, racenum, i))
	    	n++;

	/* Pick a random gender */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_GENDERS; i++)
/*	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK) {*/
	    if (validgend(rolenum, racenum, i)) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role/race has no permitted genders? */
	return (rn2(ROLE_GENDERS));
}


int
str2gend(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_GENDERS; i++) {
	    /* Does it match the adjective? */
	    if (!strncmpi(str, genders[i].adj, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, genders[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validalign(rolenum, racenum, alignnum)
	int rolenum, racenum, alignnum;
{
	/* Assumes validrole and validrace */
	return (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		(roles[rolenum].allow & races[racenum].allow &
		 aligns[alignnum].allow & ROLE_ALIGNMASK));
}


int
randalign(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid alignments */
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK)
	    	n++;

	/* Pick a random alignment */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role/race has no permitted alignments? */
	return (rn2(ROLE_ALIGNS));
}


int
str2align(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    /* Does it match the adjective? */
	    if (!strncmpi(str, aligns[i].adj, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, aligns[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}

/* is rolenum compatible with any racenum/gendnum/alignnum constraints? */
boolean
ok_role(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (rolenum >= 0 && rolenum < SIZE(roles)-1) {
	allow = roles[rolenum].allow;

	if (racenum >= 0 && racenum < SIZE(races)-1)
	    allow &= races[racenum].allow;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;

	if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		!(allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(roles)-1; i++) {
	    allow = roles[i].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1)
		allow &= races[racenum].allow;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
		continue;
	    if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		    !(allow & ROLE_ALIGNMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random role subject to any racenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a role is returned only if there is  */
/* a single possibility */
int
pick_role(racenum, gendnum, alignnum, pickhow)
int racenum, gendnum, alignnum, pickhow;
{
    int i;
    int roles_ok = 0;

    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum))
	    roles_ok++;
    }
    if (roles_ok == 0 || (roles_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    roles_ok = rn2(roles_ok);
    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum)) {
	    if (roles_ok == 0)
		return i;
	    else
		roles_ok--;
	}
    }
    return ROLE_NONE;
}

/* is racenum compatible with any rolenum/gendnum/alignnum constraints? */
boolean
ok_race(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (racenum >= 0 && racenum < SIZE(races)-1) {
	allow = races[racenum].allow;

	if (rolenum >= 0 && rolenum < SIZE(roles)-1)
	    allow &= roles[rolenum].allow;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;

	if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		!(allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(races)-1; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1)
		allow &= roles[rolenum].allow;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
		continue;
	    if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		    !(allow & ROLE_ALIGNMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random race subject to any rolenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a race is returned only if there is  */
/* a single possibility */
int
pick_race(rolenum, gendnum, alignnum, pickhow)
int rolenum, gendnum, alignnum, pickhow;
{
    int i;
    int races_ok = 0;

    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum))
	    races_ok++;
    }
    if (races_ok == 0 || (races_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    races_ok = rn2(races_ok);
    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum)) {
	    if (races_ok == 0) {

		    if (!rn2(10) && !flags.hybridcancel && flags.randomhybrids) {	/* hybrid races --Amy */

			flags.hybridization++;
			switch (rnd(44)) {

				case 1:
					flags.hybridangbander = 1; break;
				case 2:
					flags.hybridaquarian = 1; break;
				case 3:
					flags.hybridcurser = 1; break;
				case 4:
					flags.hybridhaxor = 1; break;
				case 5:
					flags.hybridhomicider = 1; break;
				case 6:
					flags.hybridsuxxor = 1; break;
				case 7:
					flags.hybridwarper = 1; break;
				case 8:
					flags.hybridrandomizer = 1; break;
				case 9:
					flags.hybridnullrace = 1; break;
				case 10:
					flags.hybridmazewalker = 1; break;
				case 11:
					flags.hybridsoviet = 1; break;
				case 12:
					flags.hybridxrace = 1; break;
				case 13:
					flags.hybridheretic = 1; break;
				case 14:
					flags.hybridsokosolver = 1; break;
				case 15:
					flags.hybridspecialist = 1; break;
				case 16:
					flags.hybridamerican = 1; break;
				case 17:
					flags.hybridminimalist = 1; break;
				case 18:
					flags.hybridnastinator = 1; break;
				case 19:
					flags.hybridrougelike = 1; break;
				case 20:
					flags.hybridsegfaulter = 1; break;
				case 21:
					flags.hybridironman = 1; break;
				case 22:
					flags.hybridamnesiac = 1; break;
				case 23:
					flags.hybridproblematic = 1; break;
				case 24:
					flags.hybridwindinhabitant = 1; break;
				case 25:
					flags.hybridaggravator = 1; break;
				case 26:
					flags.hybridevilvariant = 1; break;
				case 27:
					flags.hybridlevelscaler = 1; break;
				case 28:
					flags.hybriderosator = 1; break;
				case 29:
					flags.hybridroommate = 1; break;
				case 30:
					flags.hybridextravator = 1; break;
				case 31:
					flags.hybridhallucinator = 1; break;
				case 32:
					flags.hybridbossrusher = 1; break;
				case 33:
					flags.hybriddorian = 1; break;
				case 34:
					flags.hybridtechless = 1; break;
				case 35:
					flags.hybridblait = 1; break;
				case 36:
					flags.hybridgrouper = 1; break;
				case 37:
					flags.hybridscriptor = 1; break;
				case 38:
					flags.hybridunbalancor = 1; break;
				case 39:
					flags.hybridbeacher = 1; break;
				case 40:
					flags.hybridstairseeker = 1; break;
				case 41:
					flags.hybridmatrayser = 1; break;
				case 42:
					flags.hybridfeminizer = 1; break;
				case 43:
					flags.hybridchallenger = 1; break;
				case 44:
					flags.hybridhardmoder = 1; break;

			}

			while ((rnd(7)) < 3) {

				switch (rnd(44)) {
	
					case 1:
						if (!(flags.hybridangbander)) {flags.hybridangbander = 1; flags.hybridization++; break;
						}
					case 2:
						if (!(flags.hybridaquarian)) {flags.hybridaquarian = 1; flags.hybridization++; break;
						}
					case 3:
						if (!(flags.hybridcurser)) {flags.hybridcurser = 1; flags.hybridization++; break;
						}
					case 4:
						if (!(flags.hybridhaxor)) {flags.hybridhaxor = 1; flags.hybridization++; break;
						}
					case 5:
						if (!(flags.hybridhomicider)) {flags.hybridhomicider = 1; flags.hybridization++; break;
						}
					case 6:
						if (!(flags.hybridsuxxor)) {flags.hybridsuxxor = 1; flags.hybridization++; break;
						}
					case 7:
						if (!(flags.hybridwarper)) {flags.hybridwarper = 1; flags.hybridization++; break;
						}
					case 8:
						if (!(flags.hybridrandomizer)) {flags.hybridrandomizer = 1; flags.hybridization++; break;
						}
					case 9:
						if (!(flags.hybridnullrace)) {flags.hybridnullrace = 1; flags.hybridization++; break;
						}
					case 10:
						if (!(flags.hybridmazewalker)) {flags.hybridmazewalker = 1; flags.hybridization++; break;
						}
					case 11:
						if (!(flags.hybridsoviet)) {flags.hybridsoviet = 1; flags.hybridization++; break;
						}
					case 12:
						if (!(flags.hybridxrace)) {flags.hybridxrace = 1; flags.hybridization++; break;
						}
					case 13:
						if (!(flags.hybridheretic)) {flags.hybridheretic = 1; flags.hybridization++; break;
						}
					case 14:
						if (!(flags.hybridsokosolver)) {flags.hybridsokosolver = 1; flags.hybridization++; break;
						}
					case 15:
						if (!(flags.hybridspecialist)) {flags.hybridspecialist = 1; flags.hybridization++; break;
						}
					case 16:
						if (!(flags.hybridamerican)) {flags.hybridamerican = 1; flags.hybridization++; break;
						}
					case 17:
						if (!(flags.hybridminimalist)) {flags.hybridminimalist = 1; flags.hybridization++; break;
						}
					case 18:
						if (!(flags.hybridnastinator)) {flags.hybridnastinator = 1; flags.hybridization++; break;
						}
					case 19:
						if (!(flags.hybridrougelike)) {flags.hybridrougelike = 1; flags.hybridization++; break;
						}
					case 20:
						if (!(flags.hybridsegfaulter)) {flags.hybridsegfaulter = 1; flags.hybridization++; break;
						}
					case 21:
						if (!(flags.hybridironman)) {flags.hybridironman = 1; flags.hybridization++; break;
						}
					case 22:
						if (!(flags.hybridamnesiac)) {flags.hybridamnesiac = 1; flags.hybridization++; break;
						}
					case 23:
						if (!(flags.hybridproblematic)) {flags.hybridproblematic = 1; flags.hybridization++; break;
						}
					case 24:
						if (!(flags.hybridwindinhabitant)) {flags.hybridwindinhabitant = 1; flags.hybridization++; break;
						}
					case 25:
						if (!(flags.hybridaggravator)) {flags.hybridaggravator = 1; flags.hybridization++; break;
						}
					case 26:
						if (!(flags.hybridevilvariant)) {flags.hybridevilvariant = 1; flags.hybridization++; break;
						}
					case 27:
						if (!(flags.hybridlevelscaler)) {flags.hybridlevelscaler = 1; flags.hybridization++; break;
						}
					case 28:
						if (!(flags.hybriderosator)) {flags.hybriderosator = 1; flags.hybridization++; break;
						}
					case 29:
						if (!(flags.hybridroommate)) {flags.hybridroommate = 1; flags.hybridization++; break;
						}
					case 30:
						if (!(flags.hybridextravator)) {flags.hybridextravator = 1; flags.hybridization++; break;
						}
					case 31:
						if (!(flags.hybridhallucinator)) {flags.hybridhallucinator = 1; flags.hybridization++; break;
						}
					case 32:
						if (!(flags.hybridbossrusher)) {flags.hybridbossrusher = 1; flags.hybridization++; break;
						}
					case 33:
						if (!(flags.hybriddorian)) {flags.hybriddorian = 1; flags.hybridization++; break;
						}
					case 34:
						if (!(flags.hybridtechless)) {flags.hybridtechless = 1; flags.hybridization++; break;
						}
					case 35:
						if (!(flags.hybridblait)) {flags.hybridblait = 1; flags.hybridization++; break;
						}
					case 36:
						if (!(flags.hybridgrouper)) {flags.hybridgrouper = 1; flags.hybridization++; break;
						}
					case 37:
						if (!(flags.hybridscriptor)) {flags.hybridscriptor = 1; flags.hybridization++; break;
						}
					case 38:
						if (!(flags.hybridunbalancor)) {flags.hybridunbalancor = 1; flags.hybridization++; break;
						}
					case 39:
						if (!(flags.hybridbeacher)) {flags.hybridbeacher = 1; flags.hybridization++; break;
						}
					case 40:
						if (!(flags.hybridstairseeker)) {flags.hybridstairseeker = 1; flags.hybridization++; break;
						}
					case 41:
						if (!(flags.hybridmatrayser)) {flags.hybridmatrayser = 1; flags.hybridization++; break;
						}
					case 42:
						if (!(flags.hybridfeminizer)) {flags.hybridfeminizer = 1; flags.hybridization++; break;
						}
					case 43:
						if (!(flags.hybridchallenger)) {flags.hybridchallenger = 1; flags.hybridization++; break;
						}
					case 44:
						if (!(flags.hybridhardmoder)) {flags.hybridhardmoder = 1; flags.hybridization++; break;
						}
	
				}
			}

		    }

		return i;

		}
	    else
		races_ok--;
	}
    }

    return ROLE_NONE;
}

/* is gendnum compatible with any rolenum/racenum/alignnum constraints? */
/* gender and alignment are not comparable (and also not constrainable) */
boolean
ok_gend(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (gendnum >= 0 && gendnum < ROLE_GENDERS) {
	allow = genders[gendnum].allow;

	if (rolenum >= 0 && rolenum < SIZE(roles)-1)
	    allow &= roles[rolenum].allow;
	if (racenum >= 0 && racenum < SIZE(races)-1)
	    allow &= races[racenum].allow;
		    
	if (!(allow & ROLE_GENDMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_GENDERS; i++) {
	    allow = genders[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1)
		allow &= roles[rolenum].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1)
		allow &= races[racenum].allow;
	    if (allow & ROLE_GENDMASK)
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random gender subject to any rolenum/racenum/alignnum constraints */
/* gender and alignment are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID a gender is returned only if there is  */
/* a single possibility */
int
pick_gend(rolenum, racenum, alignnum, pickhow)
int rolenum, racenum, alignnum, pickhow;
{
    int i;
    int gends_ok = 0;

    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum))
	    gends_ok++;
    }
    if (gends_ok == 0 || (gends_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    gends_ok = rn2(gends_ok);
    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum)) {
	    if (gends_ok == 0)
		return i;
	    else
		gends_ok--;
	}
    }
    return ROLE_NONE;
}

/* is alignnum compatible with any rolenum/racenum/gendnum constraints? */
/* alignment and gender are not comparable (and also not constrainable) */
boolean
ok_align(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (alignnum >= 0 && alignnum < ROLE_ALIGNS) {
	allow = aligns[alignnum].allow;

	if (rolenum >= 0 && rolenum < SIZE(roles)-1)
	    allow &= roles[rolenum].allow;
	if (racenum >= 0 && racenum < SIZE(races)-1)
	    allow &= races[racenum].allow;
		    
	if (!(allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1)
		allow &= roles[rolenum].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1)
		allow &= races[racenum].allow;
	    if (allow & ROLE_ALIGNMASK)
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random alignment subject to any rolenum/racenum/gendnum constraints */
/* alignment and gender are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID an alignment is returned only if there is  */
/* a single possibility */
int
pick_align(rolenum, racenum, gendnum, pickhow)
int rolenum, racenum, gendnum, pickhow;
{
    int i;
    int aligns_ok = 0;

    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i))
	    aligns_ok++;
    }
    if (aligns_ok == 0 || (aligns_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    aligns_ok = rn2(aligns_ok);
    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i)) {
	    if (aligns_ok == 0)
		return i;
	    else
		aligns_ok--;
	}
    }
    return ROLE_NONE;
}

/* change player's hybridization: 0 = random, 1 = always remove, 2 = always give one --Amy
 * special case: if you have none and it's type 0, you always get one. The other extreme, where you have all the
 * hybridizations, is so rare that I decide to not specialcase it :P */
void
changehybridization(type)
int type;
{
	int tryct = 0;

	if (type == 1 || (type == 0 && rn2(2) && flags.hybridization) ) {

		while (tryct++ < 1000) {

			switch (rnd(44)) {

			case 1:
				if (flags.hybridangbander) {
					tryct = 1000;
					flags.hybridangbander = FALSE;
					flags.hybridization--;
					pline("You no longer have the angbander hybrid race.");
				}
				break;
			case 2:
				if (flags.hybridaquarian) {
					tryct = 1000;
					flags.hybridaquarian = FALSE;
					flags.hybridization--;
					pline("You no longer have the aquarian hybrid race.");
				}
				break;
			case 3:
				if (flags.hybridcurser) {
					tryct = 1000;
					flags.hybridcurser = FALSE;
					flags.hybridization--;
					pline("You no longer have the curser hybrid race.");
				}
				break;
			case 4:
				if (flags.hybridhaxor) {
					tryct = 1000;
					flags.hybridhaxor = FALSE;
					flags.hybridization--;
					pline("You no longer have the haxor hybrid race.");
				}
				break;
			case 5:
				if (flags.hybridhomicider) {
					tryct = 1000;
					flags.hybridhomicider = FALSE;
					flags.hybridization--;
					pline("You no longer have the homicider hybrid race.");
				}
				break;
			case 6:
				if (flags.hybridsuxxor) {
					tryct = 1000;
					flags.hybridsuxxor = FALSE;
					flags.hybridization--;
					pline("You no longer have the suxxor hybrid race.");
				}
				break;
			case 7:
				if (flags.hybridwarper) {
					tryct = 1000;
					flags.hybridwarper = FALSE;
					flags.hybridization--;
					pline("You no longer have the warper hybrid race.");
				}
				break;
			case 8:
				if (flags.hybridrandomizer) {
					tryct = 1000;
					flags.hybridrandomizer = FALSE;
					flags.hybridization--;
					pline("You no longer have the randomizer hybrid race.");
				}
				break;
			case 9:
				if (flags.hybridnullrace) {
					tryct = 1000;
					flags.hybridnullrace = FALSE;
					flags.hybridization--;
					pline("You no longer have the null hybrid race.");
				}
				break;
			case 10:
				if (flags.hybridmazewalker) {
					tryct = 1000;
					flags.hybridmazewalker = FALSE;
					flags.hybridization--;
					pline("You no longer have the mazewalker hybrid race.");
				}
				break;
			case 11:
				if (flags.hybridsoviet) {
					tryct = 1000;
					flags.hybridsoviet = FALSE;
					flags.hybridization--;
					pline("You no longer have the soviet hybrid race.");
				}
				break;
			case 12:
				if (flags.hybridxrace) {
					tryct = 1000;
					flags.hybridxrace = FALSE;
					flags.hybridization--;
					pline("You no longer have the x-race hybrid race.");
				}
				break;
			case 13:
				if (flags.hybridheretic) {
					tryct = 1000;
					flags.hybridheretic = FALSE;
					flags.hybridization--;
					pline("You no longer have the heretic hybrid race.");
				}
				break;
			case 14:
				if (flags.hybridsokosolver) {
					tryct = 1000;
					flags.hybridsokosolver = FALSE;
					flags.hybridization--;
					pline("You no longer have the sokosolver hybrid race.");
				}
				break;
			case 15:
				if (flags.hybridspecialist) {
					tryct = 1000;
					flags.hybridspecialist = FALSE;
					flags.hybridization--;
					pline("You no longer have the specialist hybrid race.");
				}
				break;
			case 16:
				if (flags.hybridamerican) {
					tryct = 1000;
					flags.hybridamerican = FALSE;
					flags.hybridization--;
					pline("You no longer have the american hybrid race.");
				}
				break;
			case 17:
				if (flags.hybridminimalist) {
					tryct = 1000;
					flags.hybridminimalist = FALSE;
					flags.hybridization--;
					pline("You no longer have the minimalist hybrid race.");
				}
				break;
			case 18:
				if (flags.hybridnastinator) {
					tryct = 1000;
					flags.hybridnastinator = FALSE;
					flags.hybridization--;
					pline("You no longer have the nastinator hybrid race.");
				}
				break;
			case 19:
				if (flags.hybridrougelike) {
					tryct = 1000;
					flags.hybridrougelike = FALSE;
					flags.hybridization--;
					pline("You no longer have the rougelike hybrid race.");
				}
				break;
			case 20:
				if (flags.hybridsegfaulter) {
					tryct = 1000;
					flags.hybridsegfaulter = FALSE;
					flags.hybridization--;
					pline("You no longer have the segfaulter hybrid race. Yay. No more crashes!");
				}
				break;
			case 21:
				if (flags.hybridironman) {
					tryct = 1000;
					flags.hybridironman = FALSE;
					flags.hybridization--;
					pline("You no longer have the ironman hybrid race.");
				}
				break;
			case 22:
				if (flags.hybridamnesiac) {
					tryct = 1000;
					flags.hybridamnesiac = FALSE;
					flags.hybridization--;
					pline("You no longer have the amnesiac hybrid race.");
				}
				break;
			case 23:
				if (flags.hybridproblematic) {
					tryct = 1000;
					flags.hybridproblematic = FALSE;
					flags.hybridization--;
					pline("You no longer have the problematic hybrid race.");
				}
				break;
			case 24:
				if (flags.hybridwindinhabitant) {
					tryct = 1000;
					flags.hybridwindinhabitant = FALSE;
					flags.hybridization--;
					pline("You no longer have the wind inhabitant hybrid race.");
				}
				break;
			case 25:
				if (flags.hybridaggravator) {
					tryct = 1000;
					flags.hybridaggravator = FALSE;
					flags.hybridization--;
					pline("You no longer have the aggravator hybrid race.");
				}
				break;
			case 26:
				if (flags.hybridevilvariant) {
					tryct = 1000;
					flags.hybridevilvariant = FALSE;
					flags.hybridization--;
					pline("You no longer have the evilvariant hybrid race.");
				}
				break;
			case 27:
				if (flags.hybridlevelscaler) {
					tryct = 1000;
					flags.hybridlevelscaler = FALSE;
					flags.hybridization--;
					pline("You no longer have the levelscaler hybrid race.");
				}
				break;
			case 28:
				if (flags.hybriderosator) {
					tryct = 1000;
					flags.hybriderosator = FALSE;
					flags.hybridization--;
					pline("You no longer have the erosator hybrid race.");
				}
				break;
			case 29:
				if (flags.hybridroommate) {
					tryct = 1000;
					flags.hybridroommate = FALSE;
					flags.hybridization--;
					pline("You no longer have the roommate hybrid race.");
				}
				break;
			case 30:
				if (flags.hybridextravator) {
					tryct = 1000;
					flags.hybridextravator = FALSE;
					flags.hybridization--;
					pline("You no longer have the extravator hybrid race.");
				}
				break;
			case 31:
				if (flags.hybridhallucinator) {
					tryct = 1000;
					flags.hybridhallucinator = FALSE;
					flags.hybridization--;
					pline("You no longer have the hallucinator hybrid race.");
				}
				break;
			case 32:
				if (flags.hybridbossrusher) {
					tryct = 1000;
					flags.hybridbossrusher = FALSE;
					flags.hybridization--;
					pline("You no longer have the bossrusher hybrid race.");
				}
				break;
			case 33:
				if (flags.hybriddorian) {
					tryct = 1000;
					flags.hybriddorian = FALSE;
					flags.hybridization--;
					pline("You no longer have the dorian hybrid race.");
				}
				break;
			case 34:
				if (flags.hybridtechless) {
					tryct = 1000;
					flags.hybridtechless = FALSE;
					flags.hybridization--;
					pline("You no longer have the techless hybrid race.");
				}
				break;
			case 35:
				if (flags.hybridblait) {
					tryct = 1000;
					flags.hybridblait = FALSE;
					flags.hybridization--;
					pline("You no longer have the blait hybrid race.");
				}
				break;
			case 36:
				if (flags.hybridgrouper) {
					tryct = 1000;
					flags.hybridgrouper = FALSE;
					flags.hybridization--;
					pline("You no longer have the grouper hybrid race.");
				}
				break;
			case 37:
				if (flags.hybridscriptor) {
					tryct = 1000;
					flags.hybridscriptor = FALSE;
					flags.hybridization--;
					pline("You no longer have the scriptor hybrid race.");
				}
				break;
			case 38:
				if (flags.hybridunbalancor) {
					tryct = 1000;
					flags.hybridunbalancor = FALSE;
					flags.hybridization--;
					pline("You no longer have the unbalancor hybrid race.");
				}
				break;
			case 39:
				if (flags.hybridbeacher) {
					tryct = 1000;
					flags.hybridbeacher = FALSE;
					flags.hybridization--;
					pline("You no longer have the beacher hybrid race.");
				}
				break;
			case 40:
				if (flags.hybridstairseeker) {
					tryct = 1000;
					flags.hybridstairseeker = FALSE;
					flags.hybridization--;
					pline("You no longer have the stairseeker hybrid race.");
				}
				break;
			case 41:
				if (flags.hybridmatrayser) {
					tryct = 1000;
					flags.hybridmatrayser = FALSE;
					flags.hybridization--;
					pline("You no longer have the matrayser hybrid race.");
				}
				break;
			case 42:
				if (flags.hybridfeminizer) {
					tryct = 1000;
					flags.hybridfeminizer = FALSE;
					flags.hybridization--;
					pline("You no longer have the feminizer hybrid race.");
				}
				break;
			case 43:
				if (flags.hybridchallenger) {
					tryct = 1000;
					flags.hybridchallenger = FALSE;
					flags.hybridization--;
					pline("You no longer have the challenger hybrid race.");
				}
				break;
			case 44:
				if (flags.hybridhardmoder) {
					tryct = 1000;
					flags.hybridhardmoder = FALSE;
					flags.hybridization--;
					pline("You no longer have the hardmoder hybrid race.");
				}
				break;

			} /* end switch case */
		}

	} else {

		while (tryct++ < 1000) {

			switch (rnd(44)) {

			case 1:
				if (!flags.hybridangbander) {
					tryct = 1000;
					flags.hybridangbander = TRUE;
					flags.hybridization++;
					pline("You now have the angbander hybrid race.");
				}
				break;
			case 2:
				if (!flags.hybridaquarian) {
					tryct = 1000;
					flags.hybridaquarian = TRUE;
					flags.hybridization++;
					pline("You now have the aquarian hybrid race.");
				}
				break;
			case 3:
				if (!flags.hybridcurser) {
					tryct = 1000;
					flags.hybridcurser = TRUE;
					flags.hybridization++;
					pline("You now have the curser hybrid race.");
				}
				break;
			case 4:
				if (!flags.hybridhaxor) {
					tryct = 1000;
					flags.hybridhaxor = TRUE;
					flags.hybridization++;
					pline("You now have the haxor hybrid race.");
				}
				break;
			case 5:
				if (!flags.hybridhomicider) {
					tryct = 1000;
					flags.hybridhomicider = TRUE;
					flags.hybridization++;
					pline("You now have the homicider hybrid race.");
				}
				break;
			case 6:
				if (!flags.hybridsuxxor) {
					tryct = 1000;
					flags.hybridsuxxor = TRUE;
					flags.hybridization++;
					pline("You now have the suxxor hybrid race.");
				}
				break;
			case 7:
				if (!flags.hybridwarper) {
					tryct = 1000;
					flags.hybridwarper = TRUE;
					flags.hybridization++;
					pline("You now have the warper hybrid race.");
				}
				break;
			case 8:
				if (!flags.hybridrandomizer) {
					tryct = 1000;
					flags.hybridrandomizer = TRUE;
					flags.hybridization++;
					pline("You now have the randomizer hybrid race.");
				}
				break;
			case 9:
				if (!flags.hybridnullrace) {
					tryct = 1000;
					flags.hybridnullrace = TRUE;
					flags.hybridization++;
					pline("You now have the null hybrid race.");
				}
				break;
			case 10:
				if (!flags.hybridmazewalker) {
					tryct = 1000;
					flags.hybridmazewalker = TRUE;
					flags.hybridization++;
					pline("You now have the mazewalker hybrid race.");
				}
				break;
			case 11:
				if (!flags.hybridsoviet) {
					tryct = 1000;
					flags.hybridsoviet = TRUE;
					flags.hybridization++;
					pline("You now have the soviet hybrid race.");
				}
				break;
			case 12:
				if (!flags.hybridxrace) {
					tryct = 1000;
					flags.hybridxrace = TRUE;
					flags.hybridization++;
					pline("You now have the x-race hybrid race.");
				}
				break;
			case 13:
				if (!flags.hybridheretic) {
					tryct = 1000;
					flags.hybridheretic = TRUE;
					flags.hybridization++;
					pline("You now have the heretic hybrid race.");
				}
				break;
			case 14:
				if (!flags.hybridsokosolver) {
					tryct = 1000;
					flags.hybridsokosolver = TRUE;
					flags.hybridization++;
					pline("You now have the sokosolver hybrid race.");
				}
				break;
			case 15:
				if (!flags.hybridspecialist) {
					tryct = 1000;
					flags.hybridspecialist = TRUE;
					flags.hybridization++;
					pline("You now have the specialist hybrid race.");
				}
				break;
			case 16:
				if (!flags.hybridamerican) {
					tryct = 1000;
					flags.hybridamerican = TRUE;
					flags.hybridization++;
					pline("You now have the american hybrid race.");
				}
				break;
			case 17:
				if (!flags.hybridminimalist) {
					tryct = 1000;
					flags.hybridminimalist = TRUE;
					flags.hybridization++;
					pline("You now have the minimalist hybrid race.");
				}
				break;
			case 18:
				if (!flags.hybridnastinator) {
					tryct = 1000;
					flags.hybridnastinator = TRUE;
					flags.hybridization++;
					pline("You now have the nastinator hybrid race.");
				}
				break;
			case 19:
				if (!flags.hybridrougelike) {
					tryct = 1000;
					flags.hybridrougelike = TRUE;
					flags.hybridization++;
					pline("You now have the rougelike hybrid race.");
				}
				break;
			case 20:
				if (!flags.hybridsegfaulter) {
					tryct = 1000;
					flags.hybridsegfaulter = TRUE;
					flags.hybridization++;
					pline("You now have the segfaulter hybrid race. Oh no, the game's gonna crash!");
				}
				break;
			case 21:
				if (!flags.hybridironman) {
					tryct = 1000;
					flags.hybridironman = TRUE;
					flags.hybridization++;
					pline("You now have the ironman hybrid race.");
				}
				break;
			case 22:
				if (!flags.hybridamnesiac) {
					tryct = 1000;
					flags.hybridamnesiac = TRUE;
					flags.hybridization++;
					pline("You now have the amnesiac hybrid race.");
				}
				break;
			case 23:
				if (!flags.hybridproblematic) {
					tryct = 1000;
					flags.hybridproblematic = TRUE;
					flags.hybridization++;
					pline("You now have the problematic hybrid race.");
				}
				break;
			case 24:
				if (!flags.hybridwindinhabitant) {
					tryct = 1000;
					flags.hybridwindinhabitant = TRUE;
					flags.hybridization++;
					pline("You now have the wind inhabitant hybrid race.");
				}
				break;
			case 25:
				if (!flags.hybridaggravator) {
					tryct = 1000;
					flags.hybridaggravator = TRUE;
					flags.hybridization++;
					pline("You now have the aggravator hybrid race.");
				}
				break;
			case 26:
				if (!flags.hybridevilvariant) {
					tryct = 1000;
					flags.hybridevilvariant = TRUE;
					flags.hybridization++;
					pline("You now have the evilvariant hybrid race.");
				}
				break;
			case 27:
				if (!flags.hybridlevelscaler) {
					tryct = 1000;
					flags.hybridlevelscaler = TRUE;
					flags.hybridization++;
					pline("You now have the levelscaler hybrid race.");
				}
				break;
			case 28:
				if (!flags.hybriderosator) {
					tryct = 1000;
					flags.hybriderosator = TRUE;
					flags.hybridization++;
					pline("You now have the erosator hybrid race.");
				}
				break;
			case 29:
				if (!flags.hybridroommate) {
					tryct = 1000;
					flags.hybridroommate = TRUE;
					flags.hybridization++;
					pline("You now have the roommate hybrid race.");
				}
				break;
			case 30:
				if (!flags.hybridextravator) {
					tryct = 1000;
					flags.hybridextravator = TRUE;
					flags.hybridization++;
					pline("You now have the extravator hybrid race.");
				}
				break;
			case 31:
				if (!flags.hybridhallucinator) {
					tryct = 1000;
					flags.hybridhallucinator = TRUE;
					flags.hybridization++;
					pline("You now have the hallucinator hybrid race.");
				}
				break;
			case 32:
				if (!flags.hybridbossrusher) {
					tryct = 1000;
					flags.hybridbossrusher = TRUE;
					flags.hybridization++;
					pline("You now have the bossrusher hybrid race.");
				}
				break;
			case 33:
				if (!flags.hybriddorian) {
					tryct = 1000;
					flags.hybriddorian = TRUE;
					flags.hybridization++;
					pline("You now have the dorian hybrid race.");
				}
				break;
			case 34:
				if (!flags.hybridtechless) {
					tryct = 1000;
					flags.hybridtechless = TRUE;
					flags.hybridization++;
					pline("You now have the techless hybrid race.");
				}
				break;
			case 35:
				if (!flags.hybridblait) {
					tryct = 1000;
					flags.hybridblait = TRUE;
					flags.hybridization++;
					pline("You now have the blait hybrid race.");
				}
				break;
			case 36:
				if (!flags.hybridgrouper) {
					tryct = 1000;
					flags.hybridgrouper = TRUE;
					flags.hybridization++;
					pline("You now have the grouper hybrid race.");
				}
				break;
			case 37:
				if (!flags.hybridscriptor) {
					tryct = 1000;
					flags.hybridscriptor = TRUE;
					flags.hybridization++;
					pline("You now have the scriptor hybrid race.");
				}
				break;
			case 38:
				if (!flags.hybridunbalancor) {
					tryct = 1000;
					flags.hybridunbalancor = TRUE;
					flags.hybridization++;
					pline("You now have the unbalancor hybrid race.");
				}
				break;
			case 39:
				if (!flags.hybridbeacher) {
					tryct = 1000;
					flags.hybridbeacher = TRUE;
					flags.hybridization++;
					pline("You now have the beacher hybrid race.");
				}
				break;
			case 40:
				if (!flags.hybridstairseeker) {
					tryct = 1000;
					flags.hybridstairseeker = TRUE;
					flags.hybridization++;
					pline("You now have the stairseeker hybrid race.");
				}
				break;
			case 41:
				if (!flags.hybridmatrayser) {
					tryct = 1000;
					flags.hybridmatrayser = TRUE;
					flags.hybridization++;
					pline("You now have the matrayser hybrid race.");
				}
				break;
			case 42:
				if (!flags.hybridfeminizer) {
					tryct = 1000;
					flags.hybridfeminizer = TRUE;
					flags.hybridization++;
					pline("You now have the feminizer hybrid race.");
				}
				break;
			case 43:
				if (!flags.hybridchallenger) {
					tryct = 1000;
					flags.hybridchallenger = TRUE;
					flags.hybridization++;
					pline("You now have the challenger hybrid race.");
				}
				break;
			case 44:
				if (!flags.hybridhardmoder) {
					tryct = 1000;
					flags.hybridhardmoder = TRUE;
					flags.hybridization++;
					pline("You now have the hardmoder hybrid race.");
				}
				break;

			} /* end switch case */

		}

	}
}

void
recursioneffect()
{
	u.temprecursion = 0;
	u.temprecursiontime = 0;
	u.demagoguerecursion = 0;
	u.demagoguerecursiontime = 0;
	u.oldrecursionrole = -1;
	u.oldrecursionrace = -1;

	if (!rn2(4)) {

	flags.initrole = randrole();
	flags.initrace = randrace(flags.initrole);
	urole = roles[flags.initrole];
	urace = races[flags.initrace];

	pline("You suddenly feel like a completely different person!");
	pline("You're a %s %s now!", urace.noun, (flags.female && urole.name.f) ? urole.name.f : urole.name.m);

	} else if (!rn2(2)) {

	flags.initrole = randrole();
	urole = roles[flags.initrole];

	pline("You suddenly feel like you have a different profession!");
	pline("You're a %s now!", (flags.female && urole.name.f) ? urole.name.f : urole.name.m);

	} else {

	flags.initrace = randrace(flags.initrole);
	urace = races[flags.initrace];

	pline("You suddenly feel like your genes got exchanged!");
	pline("You're a %s now!", urace.noun);

	}

	init_uasmon();

	if (Race_if(PM_UNGENOMOLD) && !Upolyd) polyself(FALSE);

}

void
rolerecursioneffect()
{
	u.temprecursion = 0;
	u.temprecursiontime = 0;
	u.demagoguerecursion = 0;
	u.demagoguerecursiontime = 0;
	u.oldrecursionrole = -1;
	u.oldrecursionrace = -1;

	flags.initrole = randrole();
	urole = roles[flags.initrole];

	init_uasmon();

}

void
greenslimetransformation()
{
	u.temprecursion = 0;
	u.temprecursiontime = 0;
	u.demagoguerecursion = 0;
	u.demagoguerecursiontime = 0;
	u.oldrecursionrole = -1;
	u.oldrecursionrace = -1;
	
	flags.initrace = str2race("Sli");
	urace = races[flags.initrace];

	init_uasmon();

}

void
bindertransformation()
{
	u.temprecursion = 0;
	u.temprecursiontime = 0;
	u.demagoguerecursion = 0;
	u.demagoguerecursiontime = 0;
	u.oldrecursionrole = -1;
	u.oldrecursionrace = -1;
	
	flags.initrole = str2role("Bin");
	urole = roles[flags.initrole];

	init_uasmon();

}

void
temprecursioneffect()
{
	u.temprecursiontime = rnz(2500 + rnd(5000));
	u.temprecursion = 1;
	u.demagoguerecursion = 0;
	u.demagoguerecursiontime = 0;

	if (!rn2(4)) {

		u.oldrecursionrole = flags.initrole;
		u.oldrecursionrace = flags.initrace;
		flags.initrole = randrole();
		flags.initrace = randrace(flags.initrole);
		urole = roles[flags.initrole];
		urace = races[flags.initrace];

		pline("You temporarily feel like someone else!");
		pline("You're temporarily a %s %s now!", urace.noun, (flags.female && urole.name.f) ? urole.name.f : urole.name.m);

	} else if (!rn2(2)) {

		u.oldrecursionrole = flags.initrole;
		u.oldrecursionrace = -1;
		flags.initrole = randrole();
		urole = roles[flags.initrole];

		pline("You suddenly feel like trying another profession for a while!");
		pline("You're temporarily a %s now!", (flags.female && urole.name.f) ? urole.name.f : urole.name.m);

	} else {

		u.oldrecursionrace = flags.initrace;
		u.oldrecursionrole = -1;
		flags.initrace = randrace(flags.initrole);
		urace = races[flags.initrace];

		pline("You suddenly feel a genetic injection!");
		pline("You're temporarily a %s now!", urace.noun);

	}

	init_uasmon();

	if (Race_if(PM_UNGENOMOLD) && !Upolyd) polyself(FALSE);

}

void
climrecursioneffect()
{
	if (u.temprecursion) return;
	u.demagoguerecursion = 0;
	u.demagoguerecursiontime = 0;

	u.temprecursiontime = 1000;
	u.temprecursion = 1;

	u.oldrecursionrace = flags.initrace;
	u.oldrecursionrole = -1;
	flags.initrace = randrace(flags.initrole);
	urace = races[flags.initrace];
	pline("You suddenly feel a genetic injection!");
	pline("You're temporarily a %s now!", urace.noun);

	init_uasmon();

	if (Race_if(PM_UNGENOMOLD) && !Upolyd) polyself(FALSE);

}

void
demagoguerecursioneffect()
{
	u.demagoguerecursiontime = rnz(250);
	u.demagoguerecursion = 1;

	u.oldrecursionrole = flags.initrole;
	u.oldrecursionrace = -1;
	flags.initrole = randrole();
	urole = roles[flags.initrole];

	pline("You turn into a %s!", (flags.female && urole.name.f) ? urole.name.f : urole.name.m);

	init_uasmon();

	if (Race_if(PM_UNGENOMOLD) && !Upolyd) polyself(FALSE);

}

void
rigid_role_checks()
{
    /* Some roles are limited to a single race, alignment, or gender and
     * calling this routine prior to XXX_player_selection() will help
     * prevent an extraneous prompt that actually doesn't allow
     * you to choose anything further. Note the use of PICK_RIGID which
     * causes the pick_XX() routine to return a value only if there is one
     * single possible selection, otherwise it returns ROLE_NONE.
     *
     */
    if (flags.initrole == ROLE_RANDOM) {
	/* If the role was explicitly specified as ROLE_RANDOM
	 * via -uXXXX-@ then choose the role in here to narrow down
	 * later choices. Pick a random role in this case.
	 */
	flags.initrole = pick_role(flags.initrace, flags.initgend,
					flags.initalign, PICK_RANDOM);
	if (flags.initrole < 0)
	    flags.initrole = randrole();
    }
    if (flags.initrole != ROLE_NONE) {
	if (flags.initrace == ROLE_NONE)
	     flags.initrace = pick_race(flags.initrole, flags.initgend,
						flags.initalign, PICK_RIGID);
	if (flags.initalign == ROLE_NONE)
	     flags.initalign = pick_align(flags.initrole, flags.initrace,
						flags.initgend, PICK_RIGID);
	if (flags.initgend == ROLE_NONE)
	     flags.initgend = pick_gend(flags.initrole, flags.initrace,
						flags.initalign, PICK_RIGID);
    }
}

#define BP_ALIGN	0
#define BP_GEND		1
#define BP_RACE		2
#define BP_ROLE		3
#define NUM_BP		4

STATIC_VAR char pa[NUM_BP], post_attribs;

STATIC_OVL char *
promptsep(buf, num_post_attribs)
char *buf;
int num_post_attribs;
{
	const char *conj = "and ";
	if (num_post_attribs > 1
	    && post_attribs < num_post_attribs && post_attribs > 1)
	 	strcat(buf, ","); 
	strcat(buf, " ");
	--post_attribs;
	if (!post_attribs && num_post_attribs > 1) strcat(buf, conj);
	return buf;
}

STATIC_OVL int
role_gendercount(rolenum)
int rolenum;
{
	int gendcount = 0;
	if (validrole(rolenum)) {
		if (roles[rolenum].allow & ROLE_MALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_FEMALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_NEUTER) ++gendcount;
	}
	return gendcount;
}

STATIC_OVL int
race_alignmentcount(racenum)
int racenum;
{
	int aligncount = 0;
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (races[racenum].allow & ROLE_CHAOTIC) ++aligncount;
		if (races[racenum].allow & ROLE_LAWFUL) ++aligncount;
		if (races[racenum].allow & ROLE_NEUTRAL) ++aligncount;
	}
	return aligncount;
}

char *
root_plselection_prompt(suppliedbuf, buflen, rolenum, racenum, gendnum, alignnum)
char *suppliedbuf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
	int k, gendercount = 0, aligncount = 0;
	char buf[BUFSZ];
	static char err_ret[] = " character's";
	boolean donefirst = FALSE;

	if (!suppliedbuf || buflen < 1) return err_ret;

	/* initialize these static variables each time this is called */
	post_attribs = 0;
	for (k=0; k < NUM_BP; ++k)
		pa[k] = 0;
	buf[0] = '\0';
	*suppliedbuf = '\0';
	
	/* How many alignments are allowed for the desired race? */
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM)
		aligncount = race_alignmentcount(racenum);

	if (alignnum != ROLE_NONE && alignnum != ROLE_RANDOM) {
		/* if race specified, and multiple choice of alignments for it */
		if ((racenum >= 0) && (aligncount > 1)) {
			if (donefirst) strcat(buf, " ");
			strcat(buf, aligns[alignnum].adj);
			donefirst = TRUE;
		} else {
			if (donefirst) strcat(buf, " ");
			strcat(buf, aligns[alignnum].adj);
			donefirst = TRUE;
		}
	} else {
		/* if alignment not specified, but race is specified
			and only one choice of alignment for that race then
			don't include it in the later list */
		if ((((racenum != ROLE_NONE && racenum != ROLE_RANDOM) &&
			ok_race(rolenum, racenum, gendnum, alignnum))
		      && (aligncount > 1))
		     || (racenum == ROLE_NONE || racenum == ROLE_RANDOM)) {
			pa[BP_ALIGN] = 1;
			post_attribs++;
		}
	}
	/* <your lawful> */

	/* How many genders are allowed for the desired role? */
	if (validrole(rolenum))
		gendercount = role_gendercount(rolenum);

	if (gendnum != ROLE_NONE  && gendnum != ROLE_RANDOM) {
		if (validrole(rolenum)) {
		     /* if role specified, and multiple choice of genders for it,
			and name of role itself does not distinguish gender */
			if ((rolenum != ROLE_NONE) && (gendercount > 1)
						&& !roles[rolenum].name.f) {
				if (donefirst) strcat(buf, " ");
				strcat(buf, genders[gendnum].adj);
				donefirst = TRUE;
			}
	        } else {
			if (donefirst) strcat(buf, " ");
	        	strcat(buf, genders[gendnum].adj);
			donefirst = TRUE;
	        }
	} else {
		/* if gender not specified, but role is specified
			and only one choice of gender then
			don't include it in the later list */
		if ((validrole(rolenum) && (gendercount > 1)) || !validrole(rolenum)) {
			pa[BP_GEND] = 1;
			post_attribs++;
		}
	}
	/* <your lawful female> */

	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (validrole(rolenum) && ok_race(rolenum, racenum, gendnum, alignnum)) {
			if (donefirst) strcat(buf, " "); 
			strcat(buf, (rolenum == ROLE_NONE) ?
				races[racenum].noun :
				races[racenum].adj);
			donefirst = TRUE;
		} else if (!validrole(rolenum)) {
			if (donefirst) strcat(buf, " ");
			strcat(buf, races[racenum].noun);
			donefirst = TRUE;
		} else {
			pa[BP_RACE] = 1;
			post_attribs++;
		}
	} else {
		pa[BP_RACE] = 1;
		post_attribs++;
	}
	/* <your lawful female gnomish> || <your lawful female gnome> */

	if (validrole(rolenum)) {
		if (donefirst) strcat(buf, " ");
		if (gendnum != ROLE_NONE) {
		    if (gendnum == 1  && roles[rolenum].name.f)
			strcat(buf, roles[rolenum].name.f);
		    else
  			strcat(buf, roles[rolenum].name.m);
		} else {
			if (roles[rolenum].name.f) {
				strcat(buf, roles[rolenum].name.m);
				strcat(buf, "/");
				strcat(buf, roles[rolenum].name.f);
			} else 
				strcat(buf, roles[rolenum].name.m);
		}
		donefirst = TRUE;
	} else if (rolenum == ROLE_NONE) {
		pa[BP_ROLE] = 1;
		post_attribs++;
	}
	
	if ((racenum == ROLE_NONE || racenum == ROLE_RANDOM) && !validrole(rolenum)) {
		if (donefirst) strcat(buf, " ");
		strcat(buf, "character");
		donefirst = TRUE;
	}
	/* <your lawful female gnomish cavewoman> || <your lawful female gnome>
	 *    || <your lawful female character>
	 */
	if (buflen > (int) (strlen(buf) + 1)) {
		strcpy(suppliedbuf, buf);
		return suppliedbuf;
	} else
		return err_ret;
}

char *
build_plselection_prompt(buf, buflen, rolenum, racenum, gendnum, alignnum)
char *buf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
	const char *defprompt = "Shall I pick a character for you? [ynq] ";
	int num_post_attribs = 0;
	char tmpbuf[BUFSZ];
	
	if (buflen < QBUFSZ)
		return (char *)defprompt;

	strcpy(tmpbuf, "Shall I pick ");
	if (racenum != ROLE_NONE || validrole(rolenum))
		strcat(tmpbuf, "your ");
	else {
		strcat(tmpbuf, "a ");
	}
	/* <your> */

	(void)  root_plselection_prompt(eos(tmpbuf), buflen - strlen(tmpbuf),
					rolenum, racenum, gendnum, alignnum);
	sprintf(buf, "%s", s_suffix(tmpbuf));

	/* buf should now be:
	 * < your lawful female gnomish cavewoman's> || <your lawful female gnome's>
	 *    || <your lawful female character's>
	 *
         * Now append the post attributes to it
	 */

	num_post_attribs = post_attribs;
	if (post_attribs) {
		if (pa[BP_RACE]) {
			(void) promptsep(eos(buf), num_post_attribs);
			strcat(buf, "race");
		}
		if (pa[BP_ROLE]) {
			(void) promptsep(eos(buf), num_post_attribs);
			strcat(buf, "role");
		}
		if (pa[BP_GEND]) {
			(void) promptsep(eos(buf), num_post_attribs);
			strcat(buf, "gender");
		}
		if (pa[BP_ALIGN]) {
			(void) promptsep(eos(buf), num_post_attribs);
			strcat(buf, "alignment");
		}
	}
	strcat(buf, " for you? [ynq] ");
	return buf;
}

#undef BP_ALIGN
#undef BP_GEND
#undef BP_RACE
#undef BP_ROLE
#undef NUM_BP

void
plnamesuffix()
{
	char *sptr, *eptr;
	int i;

	/* Look for tokens delimited by '-' */
	if ((eptr = index(plname, '-')) != (char *) 0)
	    *eptr++ = '\0';
	while (eptr) {
	    /* Isolate the next token */
	    sptr = eptr;
	    if ((eptr = index(sptr, '-')) != (char *)0)
		*eptr++ = '\0';

	    /* Try to match it to something */
	    if ((i = str2role(sptr)) != ROLE_NONE)
		flags.initrole = i;
	    else if ((i = str2race(sptr)) != ROLE_NONE)
		flags.initrace = i;
	    else if ((i = str2gend(sptr)) != ROLE_NONE)
		flags.initgend = i;
	    else if ((i = str2align(sptr)) != ROLE_NONE)
		flags.initalign = i;
	}
	if(!plname[0]) {
	    askname();
	    plnamesuffix();
	}

	/* commas in the plname confuse the record file, convert to spaces */
	for (sptr = plname; *sptr; sptr++) {
		if (*sptr == ',') *sptr = ' ';
	}
}

/*
 *	Special setup modifications here:
 *
 *	Unfortunately, this is going to have to be done
 *	on each newgame or restore, because you lose the permonst mods
 *	across a save/restore.  :-)
 *
 *	1 - The Rogue Leader is the Tourist Nemesis.
 *	2 - Priests start with a random alignment - convert the leader and
 *	    guardians here.
 *	3 - Elves can have one of two different leaders, but can't work it
 *	    out here because it requires hacking the level file data (see
 *	    sp_lev.c).
 *
 * This code also replaces quest_init().
 */
void
role_init()
{
	int alignmnt;

	/* Strip the role letter out of the player name.
	 * This is included for backwards compatibility.
	 */
	plnamesuffix();

	/* Check for a valid role.  Try flags.initrole first. */
	if (!validrole(flags.initrole)) {
	    /* Try the player letter second */
	    if ((flags.initrole = str2role(pl_character)) < 0)
	    	/* None specified; pick a random role */
	    	flags.initrole = randrole();
	}

	/* We now have a valid role index.  Copy the role name back. */
	/* This should become OBSOLETE */
	strcpy(pl_character, roles[flags.initrole].name.m);
	pl_character[PL_CSIZ-1] = '\0';

	/* Check for a valid race */
	if (!validrace(flags.initrole, flags.initrace))
	    flags.initrace = randrace(flags.initrole);

	/* Check for a valid gender.  If new game, check both initgend
	 * and female.  On restore, assume flags.female is correct. */
	if (flags.pantheon == -1) {	/* new game */
	    if (!validgend(flags.initrole, flags.initrace, flags.female))
		flags.female = !flags.female;
	}
	if (!validgend(flags.initrole, flags.initrace, flags.initgend))
	    /* Note that there is no way to check for an unspecified gender. */
	    flags.initgend = flags.female;

	/* Check for a valid alignment */
	if (!validalign(flags.initrole, flags.initrace, flags.initalign))
	    /* Pick a random alignment */
	    flags.initalign = randalign(flags.initrole, flags.initrace);
	alignmnt = aligns[flags.initalign].value;

	/* Initialize urole and urace */
	urole = roles[flags.initrole];
	urace = races[flags.initrace];
	ustartrole = roles[flags.startingrole];
	ustartrace = races[flags.startingrace];

	/* Fix up the quest leader */
	if (urole.ldrnum != NON_PM) {
	    mons[urole.ldrnum].msound = MS_LEADER;
	    mons[urole.ldrnum].mflags2 |= (M2_PEACEFUL);
	    mons[urole.ldrnum].mflags3 |= M3_CLOSE;
	    mons[urole.ldrnum].maligntyp = alignmnt * 3;
	}

	/* Fix up the quest guardians */
	if (urole.guardnum != NON_PM) {
	    mons[urole.guardnum].mflags2 |= (M2_PEACEFUL);
	    mons[urole.guardnum].maligntyp = alignmnt * 3;
	}

	/* Fix up the quest nemesis */
	if (urole.neminum != NON_PM) {
	    mons[urole.neminum].msound = MS_NEMESIS;
	    mons[urole.neminum].mflags2 &= ~(M2_PEACEFUL);
	    mons[urole.neminum].mflags2 |= (M2_NASTY|M2_STALK|M2_HOSTILE);
	    mons[urole.neminum].mflags3 |= M3_WANTSARTI | M3_WAITFORU;
	}

	/* Fix up the god names */
	if (flags.pantheon == -1) {		/* new game */

		    flags.pantheon = flags.initrole;	/* use own gods */
		    while (!roles[flags.pantheon].lgod)	/* unless they're missing */
			flags.pantheon = randrole();
	}
	if (!urole.lgod && !isheretic ) {
	    urole.lgod = roles[flags.pantheon].lgod;
	    urole.ngod = roles[flags.pantheon].ngod;
	    urole.cgod = roles[flags.pantheon].cgod;
	}
	/*if (Race_if(PM_HERETIC) ) {
	    urole.lgod = roles[u.uhereticgodlawful].lgod;
	    urole.ngod = roles[u.uhereticgodneutral].ngod;
	    urole.cgod = roles[u.uhereticgodchaotic].cgod;

	    u.uhereticlawfulnew = u.uhereticneutralnew = u.uhereticchaoticnew = 0;

	    if (!rn2(3)) { urole.lgod = hereticgods[rn2(SIZE(hereticgods))]; u.uhereticlawfulnew = 1; }
	    if (!rn2(3)) { urole.ngod = hereticgods[rn2(SIZE(hereticgods))]; u.uhereticneutralnew = 1; }
	    if (!rn2(3)) { urole.cgod = hereticgods[rn2(SIZE(hereticgods))]; u.uhereticchaoticnew = 1; }
	}*/

#if 0 /* Now in polyself.c, init_uasmon() */
	/* Fix up infravision */
	if (mons[urace.malenum].mflags3 & M3_INFRAVISION) {
	    /* although an infravision intrinsic is possible, infravision
	     * is purely a property of the physical race.  This means that we
	     * must put the infravision flag in the player's current race
	     * (either that or have separate permonst entries for
	     * elven/non-elven members of each class).  The side effect is that
	     * all NPCs of that class will have (probably bogus) infravision,
	     * but since infravision has no effect for NPCs anyway we can
	     * ignore this.
	     */
	    mons[urole.malenum].mflags3 |= M3_INFRAVISION;
	    if (urole.femalenum != NON_PM)
	    	mons[urole.femalenum].mflags3 |= M3_INFRAVISION;
	}
#endif

	/* Artifacts are fixed in hack_artifacts() */

	/* Success! */
	return;
}

const char *
Hello(mtmp)
struct monst *mtmp;
{
	if (isevilvariant) return("Palim-Palim"); /* idea by bhaak */

	if (issoviet || (youmonst.data->msound == MS_SOVIET)) {
		if (mtmp && mtmp->data == &mons[PM_SHOPKEEPER]) return("O, kakoy slabak");
		else return("Smekh novichok");
	}
	if (youmonst.data->msound == MS_JAPANESE) {
		if (mtmp && mtmp->data == &mons[PM_SHOPKEEPER]) return("Irasshaimase");
		else return("Konnichi wa");
	}

	switch (Role_switch) {
	case PM_KNIGHT:
	case PM_CHEVALIER:
	case PM_PALADIN:
	    return ("Salutations"); /* Olde English */
	case PM_SAMURAI:
	case PM_NINJA:
	case PM_OTAKU:
	    return (mtmp && mtmp->data == &mons[PM_SHOPKEEPER] ?
	    		"Irasshaimase" : "Konnichi wa"); /* Japanese */
	case PM_PIRATE:
	case PM_KORSAIR:
		return ("Ahoy");
	case PM_TOURIST:
	    return ("Aloha");       /* Hawaiian */
	case PM_VALKYRIE:
	case PM_VANILLA_VALK:
	    return (
#ifdef MAIL
	    		mtmp && mtmp->data == &mons[PM_MAIL_DAEMON] ? "Hallo" :
#endif
	    		"Velkommen");   /* Norse */
	default:
	    return ("Hello");
	}
}

const char *
Goodbye()
{
	if (isevilvariant) return("Buh-Bye");

	if (issoviet || (youmonst.data->msound == MS_SOVIET)) {
		return("Proshchay, neudachnik");
	}
	if (youmonst.data->msound == MS_JAPANESE) {
		return("Sayonara");
	}

	switch (Role_switch) {
	case PM_KNIGHT:
	case PM_CHEVALIER:
	case PM_PALADIN:
	    return ("Fare thee well");  /* Olde English */
	case PM_SAMURAI:
	case PM_NINJA:
	case PM_OTAKU:
	    return ("Sayonara");        /* Japanese */
	case PM_TOURIST:
	    return ("Aloha");           /* Hawaiian */
	case PM_VALKYRIE:
	case PM_VANILLA_VALK:
	    return ("Farvel");          /* Norse */
	default:
	    return ("Goodbye");
	}
}

/* role.c */

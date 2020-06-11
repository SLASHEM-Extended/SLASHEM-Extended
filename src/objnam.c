/*	SCCS Id: @(#)objnam.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


/* "an uncursed greased partly eaten guardian naga hatchling [corpse]" */
/* "a thoroughly greased thoroughly rusty thoroughly corroded enchanted (of contamination resistance)" */
#define PREFIX	200	/* was 80 in vanilla, increased by Amy */
#define SCHAR_LIM 127
#define NUMOBUF 12

STATIC_DCL char *strprepend(char *,const char *);
#ifdef OVLB
static boolean wishymatch(const char *,const char *,BOOLEAN_P);
#endif
static char *nextobuf(void);
static void add_erosion_words(struct obj *, char *);

STATIC_DCL char *xname2(struct obj *);

char *
enchname(prop)
int prop;
{
	switch (prop) {
		case FIRE_RES:
			return "fire resistance";
		case COLD_RES:
			return "cold resistance";
		case SLEEP_RES:
			return "sleep resistance";
		case DISINT_RES:
			return "disintegration resistance";
		case SHOCK_RES:
			return "shock resistance";
		case POISON_RES:
			return "poison resistance";
		case ACID_RES:
			return "acid resistance";
		case STONE_RES:
			return "petrification resistance";
		case REGENERATION:
			return "regeneration";
		case SEARCHING:
			return "searching";
		case SEE_INVIS:
			return "see invisible";
		case INVIS:
			return "invisibility";
		case TELEPORT:
			return "teleportitis";
		case TELEPORT_CONTROL:
			return "teleport control";
		case POLYMORPH:
			return "polymorphitis";
		case POLYMORPH_CONTROL:
			return "polymorph control";
		case LEVITATION:
			return "levitation";
		case STEALTH:
			return "stealth";
		case AGGRAVATE_MONSTER:
			return "aggravate monster";
		case CONFLICT:
			return "conflict";
		case WARNING:
			return "warning";
		case TELEPAT:
			return "telepathy";
		case FAST:
			return "speed";
		case PASSES_WALLS:
			return "wallwalking";
		case HUNGER:
			return "hunger";
		case REFLECTING:
			return "reflection";
		case ANTIMAGIC:
			return "magic resistance";
		case ENERGY_REGENERATION:
			return "energy regeneration";
		case MAGICAL_BREATHING:
			return "magical breathing";
		case HALF_SPDAM:
			return "half spell damage";
		case HALF_PHDAM:
			return "half physical damage";
		case SICK_RES:
			return "sickness resistance";
		case DRAIN_RES:
			return "drain resistance";
		case WARN_UNDEAD:
			return "undead warning";
		case FREE_ACTION:
			return "free action";
		case FLYING:
			return "flying";
		case SLOW_DIGESTION:
			return "slow digestion";
		case INFRAVISION:
			return "infravision";
		case FEAR_RES:
			return "fear resistance";
		case DISPLACED:
			return "displacement";
		case SWIMMING:
			return "swimming";
		case FIXED_ABIL:
			return "sustain ability";
		case KEEN_MEMORY:
			return "keen memory";
		case THE_FORCE:
			return "the Force";
		case STUNNED:
			return "stun";
		case CONFUSION:
			return "confusion";
		case CLAIRVOYANT:
			return "clairvoyance";
		case FEARED:
			return "fear";
		case FROZEN:
			return "freezing";
		case GLIB:
			return "glibbery hands";
		case HALLUC:
			return "hallucination";
		case BURNED:
			return "burning";
		case DIMMED:
			return "dimness";
		case WOUNDED_LEGS:
			return "legscratching";
		case NUMBED:
			return "numbness";
		case TRAP_REVEALING:
			return "trap revealing";
		case BLINDED:
			return "blindness";
		case PREMDEATH:
			return "premature death";
		case SIGHT_BONUS:
			return "improved vision";
		case VERSUS_CURSES:
			return "curse resistance";
		case STUN_RES:
			return "stun resistance";
		case CONF_RES:
			return "confusion resistance";
		case PSI_RES:
			return "psi resistance";
		case EXTRA_WEAPON_PRACTICE:
			return "practice";
		case DTBEEM_RES:
			return "death ray resistance";
		case DOUBLE_ATTACK:
			return "double attacks";
		case QUAD_ATTACK:
			return "quad attacks";
		case MANALEECH:
			return "manaleech";
		case PEACEVISION:
			return "peacevision";
		case MAP_AMNESIA:
			return "map amnesia";
		case DETECT_MONSTERS:
			return "detect monsters";
		case JUMPING:
			return "jumping";
		case HALLU_PARTY:
			return "hallu party";
		case DRUNKEN_BOXING:
			return "drunken boxing";
		case STUNNOPATHY:
			return "stunnopathy";
		case NUMBOPATHY:
			return "numbopathy";
		case DIMMOPATHY:
			return "dimmopathy";
		case FREEZOPATHY:
			return "freezopathy";
		case STONED_CHILLER:
			return "stoned chilling";
		case CORROSIVITY:
			return "corrosivity";
		case FEAR_FACTOR:
			return "fear factor";
		case BURNOPATHY:
			return "burnopathy";
		case SICKOPATHY:
			return "sickopathy";
		case WONDERLEGS:
			return "wonderlegs";
		case GLIB_COMBAT:
			return "glib combat";

		case RNGE_IRREGULARITY:
			return "irregularity";
		case RNGE_INTERNET_ACCESS:
			return "internet access";
		case RNGE_ANTI_GOVERNMENT:
			return "anti-government";
		case RNGE_GAS_FILTERING:
			return "gas filtering";
		case RNGE_VAMPIRIC_DRAIN:
			return "vampiric drain";
		case RNGE_PSEUDO_IDENTIFICATION:
			return "pseudo identification";
		case RNGE_GHOST_SUMMONING:
			return "ghost summoning";
		case RNGE_PRAYER:
			return "prayer";
		case RNGE_ANTI_BEHEADING:
			return "anti-beheading";
		case RNGE_POLAND:
			return "Poland";
		case RNGE_KICKING:
			return "kicking";
		case RNGE_TOESTOMPING:
			return "toestomping";
		case RNGE_JUNGLE_ACTION:
			return "jungle action";
		case RNGE_UNLIKABILITY:
			return "unlikability";
		case RNGE_SPELL_DISRUPTION:
			return "spell disruption";
		case RNGE_BDSM:
			return "BDSM";
		case RNGE_SLIPPING:
			return "slipping";
		case RNGE_MIRRORING:
			return "mirroring";
		case RNGE_TRUE_SIGHT:
			return "true sight";
		case RNGE_LIQUID_DIET:
			return "liquid diet";
		case RNGE_ANGRY_GODS:
			return "angry gods";
		case RNGE_RADIO_BROADCASTS:
			return "radio broadcasts";
		case RNGE_OCCASIONAL_DAMAGE:
			return "occasional damage";
		case RNGE_NOISE:
			return "noise";
		case RNGE_VORTICES:
			return "vortices";
		case RNGE_EXPLOSIONS:
			return "explosions";
		case RNGE_FATAL_ATTRACTION:
			return "fatal attraction";
		case RNGE_VOLTAGE:
			return "voltage";
		case RNGE_WEEPING:
			return "weeping";
		case RNGE_CASTLEVANIA:
			return "Castlevania";
		case RNGE_ENGLAND:
			return "England";
		case RNGE_FORGETTING:
			return "forgetting";
		case RNGE_ANTIMAGIC_A:
			return "antimagic (10%)";
		case RNGE_ANTIMAGIC_B:
			return "antimagic (20%)";
		case RNGE_ANTIMAGIC_C:
			return "antimagic (50%)";
		case RNGE_ANTIMAGIC_D:
			return "antimagic (100%)";
		case RNGE_CHINA:
			return "China";
		case RNGE_LIGHT_ABSORPTION:
			return "light absorption";
		case RNGE_CHILDBIRTH:
			return "childbirth";
		case RNGE_RED_ATTUNEMENT:
			return "red attunement";
		case RNGE_POLAR_OPPOSITES:
			return "polar opposites";
		case RNGE_SICKNESS:
			return "sickness";
		case RNGE_COQUETRY:
			return "coquetry";
		case RNGE_IGNORANCE:
			return "ignorance";
		case RNGE_VENGEANCE:
			return "vengeance";
		case RNGE_HIGH_HEELING:
			return "high heeling";
		case RNGE_GRAVITY:
			return "gravity";
		case RNGE_WISH_IMPROVEMENT:
			return "wish improvement";
		case RNGE_POCKET_MONSTERS:
			return "pocket monsters";
		case RNGE_LEVUNTATION:
			return "levuntation";
		case RNGE_PERMANENT_OCCUPATION:
			return "permanent occupation";
		case RNGE_NURSERY:
			return "nursery";
		case RNGE_SLEXUALITY:
			return "slexuality";
		case RNGE_ANGBAND:
			return "Angband";
		case RNGE_ANOREXIA:
			return "anorexia";
		case RNGE_DNETHACK:
			return "dnethack";
		case RNGE_UNNETHACK:
			return "unnethack";
		case RNGE_NETHACK_FOURK:
			return "nethack fourk";
		case RNGE_MARITAL_ARTS:
			return "marital arts";
		case RNGE_TEAM_SPLAT:
			return "Team Splat";
		case RNGE_ABOMINATIONS:
			return "abominations";
		case RNGE_PUNISHMENT:
			return "punishment";
		case RNGE_BLOODLUST:
			return "bloodlust";
		case RNGE_IMMOBILITY:
			return "immobility";
		case RNGE_BOSS_ENCOUNTERS:
			return "boss encounters";
		case RNGE_IRONMAN_MODE:
			return "ironman mode";
		case RNGE_RIVERS:
			return "rivers";
		case RNGE_LOUDSPEAKERS:
			return "loudspeakers";
		case RNGE_ARMOR_PRESERVATION:
			return "armor preservation";
		case RNGE_WEAKNESS:
			return "weakness";
		case RNGE_FRAILNESS:
			return "frailness";
		case RNGE_IRON_MAIDEN:
			return "iron maiden";
		case RNGE_DONORS:
			return "donors";
		case RNGE_AIDS:
			return "AIDS";
		case RNGE_TRAP_ALERT:
			return "trap alert";
		case RNGE_EXTINCTION:
			return "extinction";
		case RNGE_DEFUSING:
			return "defusing";
		case RNGE_DENASTIFICATION:
			return "denastification";
		case CONT_RES:
			return "contamination resistance";
		case DISCOUNT_ACTION:
			return "discount action";
		case FULL_NUTRIENT:
			return "full nutrients";
		case TECHNICALITY:
			return "technicality";
		case RNGE_MULTISHOT:
			return "multishot";
		case RNGE_SKILL_REVEAL:
			return "skill revealing";
		case RNGE_TECH_INSIGHT:
			return "tech insight";
		case RNGE_LISTENING:
			return "listening";
		case RNGE_CANNIBALISM:
			return "cannibalism";
		case RNGE_POLYVISION:
			return "polyvision";
		case RNGE_HEMOPHILIA:
			return "hemophilia";
		case RNGE_BLAST_SHIELDING:
			return "blast shielding";
		case RNGE_FUNNY_HALLU:
			return "funny hallu";
		case RNGE_POLYPILING:
			return "polypiling";
		case RNGE_SAP_GENERATION:
			return "sap generation";
		case RNGE_WEALTH:
			return "wealth";
		case DIMINISHED_BLEEDING:
			return "diminished bleeding";
		case FEMTRAP_FEMMY:
			return "Femmy's curse";
		case FEMTRAP_MADELEINE:
			return "Madeleine's curse";
		case FEMTRAP_MARLENA:
			return "Marlena's curse";
		case FEMTRAP_ANASTASIA:
			return "Anastasia's curse";
		case FEMTRAP_JESSICA:
			return "Jessica's curse";
		case FEMTRAP_SOLVEJG:
			return "Solvejg's curse";
		case FEMTRAP_WENDY:
			return "Wendy's curse";
		case FEMTRAP_KATHARINA:
			return "Katharina's curse";
		case FEMTRAP_ELENA:
			return "Elena's curse";
		case FEMTRAP_THAI:
			return "Thai's curse";
		case FEMTRAP_ELIF:
			return "Elif's curse";
		case FEMTRAP_NADJA:
			return "Nadja's curse";
		case FEMTRAP_SANDRA:
			return "Sandra's curse";
		case FEMTRAP_NATALJE:
			return "Natalje's curse";
		case FEMTRAP_JEANETTA:
			return "Jeanetta's curse";
		case FEMTRAP_YVONNE:
			return "Yvonne's curse";
		case FEMTRAP_MAURAH:
			return "Maurah's curse";
		case FEMTRAP_MELTEM:
			return "Meltem's curse";
		case FEMTRAP_SARAH:
			return "Sarah's curse";
		case FEMTRAP_CLAUDIA:
			return "Claudia's curse";
		case FEMTRAP_LUDGERA:
			return "Ludgera's curse";
		case FEMTRAP_KATI:
			return "Kati's curse";
		case FEMTRAP_NELLY:
			return "Nelly's curse";
		case FEMTRAP_EVELINE:
			return "Eveline's curse";
		case FEMTRAP_KARIN:
			return "Karin's curse";
		case FEMTRAP_JUEN:
			return "Juen's curse";
		case FEMTRAP_KRISTINA:
			return "Kristina's curse";
		case FEMTRAP_LOU:
			return "Lou's curse";
		case SCENT_VIEW:
			return "scent view";

		default:
			return "Team Nastytrap";

	}

}


struct Jitem {
	int item;
	const char *name;
};

/* true for gems/rocks that should have " stone" appended to their names */
#define GemStone(typ)	(typ == FLINT ||				\
			 (objects[typ].oc_material == MT_GEMSTONE &&	\
			  (typ != DILITHIUM_CRYSTAL && typ != RUBY &&	\
			   typ != DIAMOND && typ != SAPPHIRE &&		\
			   typ != BLACK_OPAL && 	\
			   typ != MALACHITE && 	\
			   typ != SPINEL && 	\
			   typ != LAPIS_LAZULI && 	\
			   typ != TOURMALINE && 	\
			   typ != PERIDOT && 	\
			   typ != VOLCANIC_GLASS_FRAGMENT && 	\
			   typ != BONE_FRAGMENT && 	\
			   typ != LEAD_CLUMP && 	\
			   typ != SLING_AMMO && 	\
			   typ != MOONSTONE && 	\
			   typ != WONDER_STONE && 	\
			   typ != MAGICITE_CRYSTAL && 	\
			   typ != CYAN_STONE && 	\
			   typ != EMERALD && typ != OPAL)))

#ifndef OVLB

STATIC_DCL struct Jitem ZAPM_items[];
STATIC_DCL struct Jitem Japanese_items[];
STATIC_OVL struct Jitem Pirate_items[];
STATIC_OVL struct Jitem Soviet_items[];
STATIC_OVL struct Jitem Ancient_items[];

#else /* OVLB */

STATIC_OVL struct Jitem ZAPM_items[] = {
	{ GOLD_PIECE, "buckazoid" },
	{0, "" }
};

STATIC_OVL struct Jitem Japanese_items[] = {
	{ APPLE, "ringo" },
	{ AXE, "ono" },
	{ BANDAGE, "hotai" },
	{ BASEBALL_BAT, "jo" },	/* Close enough, club is a bokken for now */
	{ BATTLE_AXE, "batoruakkusu" },
	{ BLINDFOLD, "mekakushi" },
	{ BOW, "dai-kyu" }, /* Nethack-- */
	{ BRASS_LANTERN, "toro" },
	{ BROADSWORD, "ninja-to" },
	{ BRONZE_PLATE_MAIL, "tanko" },
	{ BULLET, "dangan" },
	{ BULLWHIP, "muchi" },
	{ CANDY_BAR, "kittokatto" },
	{ CHAIN_MAIL, "kusari-katabira" },
	{ CHEST, "kinko" },
	{ CLOVE_OF_GARLIC, "nin'niku" },
	{ CLUB, "bokken" },
	{ CROSSBOW, "ishiyumi" },
	{ CROSSBOW_BOLT, "kurosuboboruto" },
	{ CRYSTAL_PLATE_MAIL, "jade o-yoroi" },
	{ DAGGER, "kunai" },
	{ DART, "bo-shuriken" },
	{ DENTED_POT, "kubonda nabe" },
	{ DWARVISH_MATTOCK, "dwarvish zaghnal" },
	{ EGG, "tamago" },
	{ FLAIL, "nunchaku" },
	{ FLY_SWATTER, "tataki tobu" },
	{ FOOD_RATION, "gunyoki" },
	{ GAUNTLETS_OF_FUMBLING, "kote of fumbling" },
	{ GAUNTLETS_OF_POWER, "kote of power" },
	{ GLAIVE, "naginata" },
	{ GRAPPLING_HOOK, "kaginawa" },
	{ GUISARME, "kamayari" },
	{ HALBERD, "bisento" },
	{ HELMET, "kabuto" },
	{ ICE_BOX, "bento box" },
	{ INSECT_SQUASHER, "konchu furattona" },
	{ KELP_FROND, "nori" },
	{ KNIFE, "shito" },
	{ LANCE, "uma-yari" },
	{ LARGE_BOX, "okina hako" },
	{ LEATHER_GLOVES, "yugake" },
	{ LOCK_PICK, "osaku" },
	{ LONG_SWORD, "chokuto" },
	{ LOW_BOOTS, "kutsu" },
	{ LUCERN_HAMMER, "rutsu-erunhanma" },
	{ MORNING_STAR, "akenomyosei" },
	{ PANCAKE, "okonomiyaki" },
	{ PEAR, "uma" },
	{ PHIAL, "yakubin" },
	{ PICK_AXE, "tsuruhashi" },
	{ PLATE_MAIL, "o-yoroi" },
	{ POT_BOOZE, "sake" },
	{ QUARTERSTAFF, "bo" },
	{ ROBE, "kimono" },
	{ ROCK, "koishi" },
	{ RUNESWORD, "run no ken" },
	{ SACK, "fukuro" },
	{ SCALPEL, "geka-yo mesu" },
	{ SEDGE_HAT, "sugegasa" },
	{ SHORT_SWORD, "wakizashi" },
	{ SHOTGUN, "sandan ju" },
	{ SHURIKEN, "hira-shuriken" },
	{ SILVER_DAGGER, "jade-hilted kunai" },
	{ SILVER_SPEAR, "jade-set yari" },
	{ SMALL_SHIELD, "timbe" }, /* Nethack-- */
	{ SPEAR, "yari" },
	{ SPLINT_MAIL, "dou-maru" },
	{ SPRIG_OF_WOLFSBANE, "torikabuto" },
	{ STEEL_WHIP, "haganenomuchi" },
	{ STETHOSCOPE, "choshinki" },
	{ STILETTO, "shoken" },
	{ TALLOW_CANDLE, "rosoku" },
	{ TORPEDO, "gyorai" },
	{ TRIDENT, "magari yari" },
	{ TWO_HANDED_SWORD, "no-dachi" },
	{ UNICORN_HORN, "yunikon no tsuno" },
	{ WAR_HAMMER, "dai tsuchi" },
	{ WAR_HAT, "jingasa" }, 
	{ WAX_CANDLE, "kyandoru" },
	{ WATER_WALKING_BOOTS, "mizugumo" },
	{ WEDGE_SANDALS, "jika-tabi" }, /* 5lo: I know they're not the same, close enough though */
	{ WOODEN_STAKE, "kikui" },
	{ WOODEN_HARP, "koto" },
	{0, "" }
};

STATIC_OVL struct Jitem Pirate_items[] = {
	{ ASSAULT_RIFLE, "burp gun" },
	{ AUTO_SHOTGUN, "repeating blunderbuss" },
	{ BAG_OF_HOLDING, "duffle" },
	{ BLINDFOLD, "eyepatches" },
	{ BROADSWORD, "lugsail" },
	{ BULLWHIP, "cat o' nine tails" },
	{ CHEST, "coffer" },
	{ CLUB, "belaying pin" },
	{ CRAM_RATION, "sea biscuit" },
	{ DAGGER, "gully" },
	{ EGG, "cackle fruit" },
	{ FLY_SWATTER, "oar" },
	{ FRAG_GRENADE, "hot potato" },
	{ FUMBLE_BOOTS, "leg irons" },
	{ FOOD_RATION, "doughboy" },
	{ GRENADE_LAUNCHER, "big bertha" },
	{ HACKER_S_FOOD, "salmagundi" },
	{ HEAVY_MACHINE_GUN, "fire-spitting gun" },
	{ IRON_CHAIN, "gibbet" },
	{ JAVELIN, "harpoon" },
	{ KNIFE, "chivey" },
	{ LARGE_BOX, "foot locker" },
	{ LEATHER_JACKET, "monkey jacket" },
	{ LEATHER_CLOAK, "long clothes" },
	{ LEMBAS_WAFER, "hardtack" },
	{ LONG_SWORD, "yardarm" },
	{ PISTOL, "barking iron" },
	{ POT_BOOZE, "rum" },
	{ RIFLE, "cuntgun" },
	{ ROCKET, "chain shot" },
	{ ROCKET_LAUNCHER, "six pounders" },
	{ SACK, "ditty bag" },
	{ SCIMITAR, "cutlass" },
	{ SHORT_SWORD, "futtock" },
	{ SHOTGUN, "blunderbuss" },
	{ SHOTGUN_SHELL, "case shot" },
	{ SMALL_SHIELD, "buckler" },
	{ SNIPER_RIFLE, "telescope gun" },
	{ STETHOSCOPE, "spyglass" },
	{ SUBMACHINE_GUN, "banging piece" },
	{ T_SHIRT, "baggypants" },
	{ WHETSTONE, "holystone" },
	{ WOODEN_FLUTE, "hornpipe" },
	{ WOODEN_STAKE, "peg leg" },
	{ VICTORIAN_UNDERWEAR, "victorian stockings" },
	{0, "" }
};

STATIC_OVL struct Jitem Soviet_items[] = {
	{ ASSAULT_RIFLE, "shturmovaya vintovka" },
	{ AUTO_SHOTGUN, "avto ruzh'ye" },
	{ AXE, "topor" },
	{ BAG_OF_HOLDING, "meshok provedeniye" },
	{ BANDAGE, "perevyazochnyy material" },
	{ BASEBALL_BAT, "beysbol'naya bita" },
	{ BATTLE_AXE, "boyevoy topor" },
	{ BLINDFOLD, "s zavyazannymi glazami" },
	{ BOW, "luk" }, /* Nethack-- */
	{ BRASS_LANTERN, "latun' fonar'" },
	{ BROADSWORD, "palash" },
	{ BRONZE_PLATE_MAIL, "bronza laty" },
	{ BULLET, "pulya" },
	{ BULLWHIP, "byk knut" },
	{ CHEST, "yashchik" },
	{ CLOVE_OF_GARLIC, "zubok chesnoka" },
	{ CLUB, "dubina" },
	{ CRAM_RATION, "davka" },
	{ CROSSBOW, "samostrel" },
	{ CROSSBOW_BOLT, "zadvizhka" },
	{ CRYSTAL_PLATE_MAIL, "kristall laty" },
	{ DAGGER, "kinzhal" },
	{ DART, "strela" },
	{ DENTED_POT, "prolomlennyy gorshok" },
	{ DWARVISH_MATTOCK, "gnomov motyga" },
	{ EGG, "yaytso" },
	{ FLAIL, "tsep" },
	{ FLY_SWATTER, "mukhoboyka" },
	{ FRAG_GRENADE, "oskolochnaya granata" },
	{ FUMBLE_BOOTS, "sharit' sapogi" },
	{ FOOD_RATION, "ratsion pitaniya" },
	{ GAUNTLETS_OF_FUMBLING, "rukavitsy sharit'" },
	{ GAUNTLETS_OF_POWER, "rukavitsy vlasti" },
	{ GLAIVE, "brosat' kop'ye" },
	{ GRAPPLING_HOOK, "kryuk" },
	{ GRENADE_LAUNCHER, "granatomet" },
	{ GUISARME, "drevkom" },
	{ HALBERD, "alebarda" },
	{ HACKER_S_FOOD, "khakery yeda" },
	{ HEAVY_MACHINE_GUN, "tyazhelyy pulemet" },
	{ HELMET, "shlem" },
	{ INSECT_SQUASHER, "obzhimnyy nasekomykh" },
	{ IRON_CHAIN, "zheleznaya tsep'" },
	{ JAVELIN, "drotik" },
	{ KNIFE, "nozh" },
	{ LANCE, "ostroga" },
	{ LARGE_BOX, "bol'shaya korobka" },
	{ LEATHER_JACKET, "kozhanyy pidzhak" },
	{ LEATHER_CLOAK, "kozhanyy plashch" },
	{ LEATHER_GLOVES, "kozhanyye perchatki" },
	{ LEMBAS_WAFER, "el'fiyskiy vafel'nyye" },
	{ LOCK_PICK, "otmychka" },
	{ LONG_SWORD, "dlinnyy mech" },
	{ LUCERN_HAMMER, "lyutserny molotok" },
	{ MORNING_STAR, "utrennyaya zvezda" },
	{ PANCAKE, "blin" },
	{ PHIAL, "puzyrek" },
	{ PICK_AXE, "kirka" },
	{ PISTOL, "oruzhiye" },
	{ PLATE_MAIL, "laty" },
	{ POT_BOOZE, "vodka" },
	{ QUARTERSTAFF, "dubina s zheleznym nakonechnikom" },
	{ RIFLE, "vintovka" },
	{ ROBE, "khalat" },
	{ ROCK, "kamen'" },
	{ ROCKET, "vechernitsa" },
	{ ROCKET_LAUNCHER, "puskovaya ustanovka" },
	{ RUNESWORD, "runa mech" },
	{ SACK, "meshok" },
	{ SCALPEL, "nozhnitsy" },
	{ SCIMITAR, "yatagan" },
	{ SHORT_SWORD, "korotkiy mech" },
	{ SHOTGUN, "drobovik" },
	{ SHOTGUN_SHELL, "ruzh'ye obolochki" },
	{ SHURIKEN, "metaniye zvezdy" },
	{ SILVER_DAGGER, "serebryanyy kinzhal" },
	{ SILVER_SPEAR, "serebro kop'ye" },
	{ SMALL_SHIELD, "nebol'shoy shchit" },
	{ SNIPER_RIFLE, "snayperskaya vintovka" },
	{ SPEAR, "kop'ye" },
	{ SPLINT_MAIL, "shina pochty" },
	{ SPRIG_OF_WOLFSBANE, "vetochka volkov otrava" },
	{ STEEL_WHIP, "stal' knut" },
	{ STETHOSCOPE, "zond" },
	{ STILETTO, "koroche kinzhal" },
	{ SUBMACHINE_GUN, "pistolet-pulemet" },
	{ TALLOW_CANDLE, "sal'naya svecha" },
	{ TORPEDO, "gangster-telokhranitel'" },
	{ TRIDENT, "trezubets" },
	{ T_SHIRT, "rubashka" },
	{ TWO_HANDED_SWORD, "dvuruchnyy mech" },
	{ UNICORN_HORN, "rog yedinoroga" },
	{ WAR_HAMMER, "voyna molotok" },
	{ WAX_CANDLE, "voskovaya svecha" },
	{ WATER_WALKING_BOOTS, "voda khod'be sapogi" },
	{ WEDGE_SANDALS, "klin sandalii" },
	{ WOODEN_FLUTE, "fleyta" },
	{ WOODEN_STAKE, "derevyannyy kol" },
	{ WOODEN_HARP, "arfa" },
	{ WHETSTONE, "tochil'nyy kamen'" },
	{ VICTORIAN_UNDERWEAR, "viktorianskiy bel'ye" },

	{ WORM_TOOTH, "cherv' zub" },
	{ CRYSKNIFE, "kristall nozh" },
	{ KATANA, "samurayskiy mech" },
	{ ELECTRIC_SWORD, "elektricheskiy mech" },
	{ TSURUGI, "dolgo samurayskiy mech" },
	{ SILVER_SABER, "serebro sabli" },
	{ GOLDEN_SABER, "zolotoy sabley" },
	{ MACE, "bulava" },
	{ SILVER_MACE, "serebro bulava" },
	{ FLANGED_MACE, "flantsevoye bulava" },
	{ JAGGED_STAR, "zubchatyye zvezdy" },
	{ DEVIL_STAR, "d'yavol zvezda" },
	{ MALLET, "kolotushka" },
	{ WEDGED_LITTLE_GIRL_SANDAL, "vklinivayetsya malo devushka sandalii" },
	{ SOFT_GIRL_SNEAKER, "myagkaya devushka krossovok" },
	{ STURDY_PLATEAU_BOOT_FOR_GIRLS, "nadezhnoye zagruzki plato dlya devochek" },
	{ HUGGING_BOOT, "obnimat' zagruzki" },
	{ BLOCK_HEELED_COMBAT_BOOT, "blok kablukakh voyennyye botinki" },
	{ WOODEN_GETA, "derevyannyy geta" },
	{ LACQUERED_DANCING_SHOE, "lakirovannyy tantsy obuvi" },
	{ HIGH_HEELED_SANDAL, "na vysokikh kablukakh sandalii" },
	{ SEXY_LEATHER_PUMP, "seksual'naya kozha nasos" },
	{ SPIKED_BATTLE_BOOT, "shipami bitva zagruzki" },
	{ BATTLE_STAFF, "bitva personal" },
	{ PARTISAN, "storonnik" },
	{ SPETUM, "bol'shaya sekira" },
	{ RANSEUR, "protazan" },
	{ BARDICHE, "berdysh" },
	{ VOULGE, "zemlya rytsar' kop'ye" },
	{ FAUCHARD, "kosa" },
	{ BILL_GUISARME, "narusheniye bar" },
	{ BEC_DE_CORBIN, "gigant kosa" },
	{ SPIRIT_THROWER, "dukh metatel'" },
	{ STYGIAN_PIKE, "stigiyskiy shchuka" },
	{ SLING, "remen'" },
	{ RUBBER_HOSE, "rezinovyy shlang" },
	{ STRIPED_SHIRT, "polosatoy rubashke" },
	{ RUFFLED_SHIRT, "trepal rubashke" },
	{ PLASTEEL_ARMOR, "plastikovyye dospekhi" },
	{ MUMMY_WRAPPING, "mumiya obertyvaniya" },
	{ LAB_COAT, "laboratornyy khalat" },
	{ POISONOUS_CLOAK, "yadovity plashch" },
	{ FEDORA, "myagkaya fetrovaya shlyapa" },
	{ DUNCE_CAP, "duratskiy kolpak" },
	{ HELM_OF_OPPOSITE_ALIGNMENT, "shlem protivopolozhnogo vyravnivaniya" },
	{ STEEL_SHIELD, "stal'noy shchit" },
	{ SHIELD_OF_REFLECTION, "otrazhayushchiye shchit" },
	{ SHIELD_OF_MOBILITY, "besplatno shchit deystviye" },
	{ DANCING_SHOES, "tantseval'naya obuv'" },
	{ SWEET_MOCASSINS, "sladkiye mokasiny" },
	{ SOFT_SNEAKERS, "myagkiye krossovki" },
	{ FEMININE_PUMPS, "zhenskiye nasosy" },
	{ LEATHER_PEEP_TOES, "kozha pip-pal'tsy" },
	{ HIPPIE_HEELS, "khippi kabluki" },
	{ COMBAT_STILETTOS, "boyevyye stilety" },
	{ FIRE_BOOTS, "sapogi na ogne" },
	{ ZIPPER_BOOTS, "sadistskiye sapogi" },
	{ BAG_OF_TRICKS, "slozhno meshok" },
	{ BUGLE, "gorn" },
	{ LAND_MINE, "fugas" },
	{ BEARTRAP, "medved' lovushka" },
	{ SPOON, "lozhka" },
	{ TORCH, "gorelka" },
	{ LASER_SWATTER, "lazernaya mukhoboyka" },
	{ EXPENSIVE_CAMERA, "dorogaya kamera" },
	{ MIRROR, "zerkalo" },
	{ CRYSTAL_BALL, "khrustal'nyy shar" },
	{ LENSES, "linzy" },
	{ TOWEL, "polotentse" },
	{ LEATHER_SADDLE, "sedlovina" },
	{ LEATHER_LEASH, "privyaz'" },
	{ TINNING_KIT, "luzheniya komplekt" },
	{ MEDICAL_KIT, "aptechka" },
	{ TIN_OPENER, "otkryvashka" },
	{ CAN_OF_GREASE, "mozhet smazki" },
	{ MAGIC_MARKER, "volshebnaya ruchka" },
	{ SWITCHER, "pereklyuchatel'" },
	{ TRIPE_RATION, "rubets ratsion" },
	{ CORPSE, "trup" },
	{ KELP_FROND, "laminariya vetv'" },
	{ EUCALYPTUS_LEAF, "evkalipta list'yev" },
	{ APPLE, "yabloko" },
	{ CARROT, "morkov'" },
	{ PEAR, "grusha" },
	{ ASIAN_PEAR, "rossii grusha" },
	{ LEMON, "negodnaya veshch'" },
	{ ORANGE, "oranzhevyy" },
	{ MUSHROOM, "grib" },
	{ MELON, "dynya" },
	{ LUMP_OF_ROYAL_JELLY, "matochnoye molochko" },
	{ CREAM_PIE, "kremovyy pirog" },
	{ SANDWICH, "buterbrod" },
	{ CANDY_BAR, "monoblok" },
	{ FORTUNE_COOKIE, "pechen'ye" },
	{ CHEESE, "syr" },
	{ HOLY_WAFER, "svyatoy vafel'nyye" },
	{ TIN, "olovo" },
	{ DILITHIUM_CRYSTAL, "dilitiyevaya kristall" },
	{ DIAMOND, "almaz" },
	{ LOADSTONE, "ochen' tyazhelyy kamen'" },
	{ SALT_CHUNK, "sol' kusok" },
	{ FLINT, "kremen' kamen'" },
	{ BOULDER, "valun" },
	{ LOADBOULDER, "tyazhelyy valun" },
	{ STATUE, "izvayaniye" },
	{ FIGURINE, "opisaniye" },
	{ HEAVY_IRON_BALL, "tyazhelyy shar zheleza" },
	{ ROTATING_CHAIN, "vrashchayushchiyesya tsepi" },
	{ TAIL_SPIKES, "khvost shipy" },
	{ FAERIE_FLOSS_RHING, "feya muline" },
	{ SEGFAULT_VENOM, "otrava oshibku segmentatsii" },
	
	{ RIN_ADORNMENT, "ukrasheniye" },
	{ RIN_HUNGER, "golod" },
	{ RIN_DISARMING, "obezoruzhivayushchiy" },
	{ RIN_NUMBNESS, "onemeniye" },
	{ RIN_HALLUCINATION, "gallyutsinatsiya" },
	{ RIN_CURSE, "proklyatiye" },
	{ RIN_MOOD, "nastroyeniye" },
	{ RIN_PROTECTION, "zashchita" },
	{ RIN_PROTECTION_FROM_SHAPE_CHAN, "net oborotni" },
	{ RIN_SLEEPING, "spal'nyy" },
	{ RIN_STEALTH, "khitrost'" },
	{ RIN_SUSTAIN_ABILITY, "ustoychivost'" },
	{ RIN_WARNING, "preduprezhdeniye" },
	{ RIN_AGGRAVATE_MONSTER, "usugubit' vragov" },
	{ RIN_COLD_RESISTANCE, "protivostoyat' kholodnym" },
	{ RIN_FEAR_RESISTANCE, "protivostoyat' strakh" },
	{ RIN_GAIN_CONSTITUTION, "poluchit' konstitutsiyu" },
	{ RIN_GAIN_DEXTERITY, "poluchit' lovkost'" },
	{ RIN_GAIN_INTELLIGENCE, "poluchit' intellekta" },
	{ RIN_GAIN_STRENGTH, "nabirat'sya sil" },
	{ RIN_GAIN_WISDOM, "obresti mudrost'" },
	{ RIN_TIMELY_BACKUP, "svoyevremennoye rezervnoye kopirovaniye" },
	{ RIN_INCREASE_ACCURACY, "povysit' tochnost'" },
	{ RIN_INCREASE_DAMAGE, "uvelichivayet uron" },
	{ RIN_SLOW_DIGESTION, "medlennoye pishchevareniye" },
	{ RIN_INVISIBILITY, "nevidimost'" },
	{ RIN_POISON_RESISTANCE, "soprotivleniye otravleniyu" },
	{ RIN_SEE_INVISIBLE, "videt' nevidimykh" },
	{ RIN_SHOCK_RESISTANCE, "protivostoyat' molnii" },
	{ RIN_SICKNESS_RESISTANCE, "immunitet k bolezni" },
	{ RIN_FIRE_RESISTANCE, "protivostoyat' ogon'" },
	{ RIN_FREE_ACTION, "soprotivleniye paralich" },
	{ RIN_LEVITATION, "svobodnoye pareniye" },
	{ RIN_REGENERATION, "vosstanovleniye" },
	{ RIN_SEARCHING, "poisk" },
	{ RIN_TELEPORTATION, "luchezarnyy" },
	{ RIN_CONFLICT, "protivorechiye" },
	{ RIN_POLYMORPH, "prevrashchat'" },
	{ RIN_POLYMORPH_CONTROL, "prevrashchat' kontrol'" },
	{ RIN_TELEPORT_CONTROL, "siyayushchiy kontrol'" },
	{ AMULET_OF_CHANGE, "amulet of izmeneniye pola" },
	{ AMULET_OF_DRAIN_RESISTANCE, "amulet of protivostoyat' zhizn' sliva" },
	{ AMULET_OF_ESP, "amulet of ekstra-sensornoye vospriyatiye" },
	{ AMULET_OF_UNDEAD_WARNING, "amulet of nezhit' preduprezhdeniye" },
	{ AMULET_OF_OWN_RACE_WARNING, "amulet of sobstvennogo preduprezhdeniye gonki" },
	{ AMULET_OF_POISON_WARNING, "amulet of preduprezhdeniye yad" },
	{ AMULET_OF_COVETOUS_WARNING, "amulet of likhoimtsy preduprezhdeniye" },
	{ AMULET_OF_FLYING, "amulet of polet" },
	{ AMULET_OF_LIFE_SAVING, "amulet of dopolnitel'nuyu zhizn'" },
	{ AMULET_OF_MAGICAL_BREATHING, "amulet of volshebnyy dykhaniye" },
	{ AMULET_OF_REFLECTION, "amulet of otrazheniye" },
	{ AMULET_OF_RESTFUL_SLEEP, "amulet of glubokiy son" },
	{ AMULET_OF_BLINDNESS, "amulet of slepota" },
	{ AMULET_OF_STRANGULATION, "amulet of udusheniye" },
	{ AMULET_OF_UNCHANGING, "amulet of neizmennaya" },
	{ AMULET_VERSUS_POISON, "amulet of soprotivleniye otravleniyu" },
	{ AMULET_VERSUS_STONE, "amulet of lecheniye okameneniya" },
	{ AMULET_OF_DEPETRIFY, "amulet of pobivaniye kamnyami immunitet" },
	{ AMULET_OF_MAGIC_RESISTANCE, "amulet of soprotivleniye magii" },
	{ AMULET_OF_SICKNESS_RESISTANCE, "amulet of immunitet k bolezni" },
	{ AMULET_OF_SWIMMING, "amulet of plavaniye" },
	{ AMULET_OF_DISINTEGRATION_RESIS, "amulet of protivostoyat' raspadu" },
	{ AMULET_OF_ACID_RESISTANCE, "amulet of protivostoyat' kisloty" },
	{ AMULET_OF_REGENERATION, "amulet of vosstanovleniye" },
	{ AMULET_OF_CONFLICT, "amulet of protivorechiye" },
	{ AMULET_OF_FUMBLING, "amulet of nelovkiy" },
	{ AMULET_OF_SECOND_CHANCE, "amulet of vtoroy shans" },
	{ AMULET_OF_DATA_STORAGE, "amulet of khranilishche dannykh" },
	{ AMULET_OF_WATERWALKING, "amulet of voda khod'be" },
	{ AMULET_OF_HUNGER, "amulet of golod" },
	{ AMULET_OF_POWER, "amulet of moshchnost'" },
	{ POT_FRUIT_JUICE, "fruktovyy sok" },
	{ POT_SEE_INVISIBLE, "videt' nevidimykh" },
	{ POT_SICKNESS, "bolezni" },
	{ POT_SLEEPING, "spal'nyy" },
	{ POT_CLAIRVOYANCE, "yasnovideniye" },
	{ POT_CONFUSION, "putanitsa" },
	{ POT_HALLUCINATION, "gallyutsinatsiya" },
	{ POT_HEALING, "istseleniye" },
	{ POT_EXTRA_HEALING, "dopolnitel'naya istseleniye" },
	{ POT_RESTORE_ABILITY, "vosstanovit' sposobnost'" },
	{ POT_BLINDNESS, "slepota" },
	{ POT_ESP, "ekstra-sensornoye vospriyatiye" },
	{ POT_GAIN_ENERGY, "poluchit' energiyu" },
	{ POT_GAIN_HEALTH, "zdorov'ye usileniya" },
	{ POT_BANISHING_FEAR, "izgonyaya strakh" },
	{ POT_ICE, "led" },
	{ POT_FIRE, "ogon'" },
	{ POT_FEAR, "strakh" },
	{ POT_STUNNING, "oglushat'" },
	{ POT_NUMBNESS, "onemeniye" },
	{ POT_SLIME, "sliz'" },
	{ POT_URINE, "mochit'sya" },
	{ POT_CANCELLATION, "annulirovaniye" },
	{ POT_INVISIBILITY, "nevidimost'" },
	{ POT_MONSTER_DETECTION, "obnaruzhit' monstrov" },
	{ POT_OBJECT_DETECTION, "obnaruzhivat' ob'yekty" },
	{ POT_ENLIGHTENMENT, "prosveshcheniye" },
	{ POT_FULL_HEALING, "polnogo zazhivleniya" },
	{ POT_LEVITATION, "svobodnoye pareniye" },
	{ POT_POLYMORPH, "prevrashchat'" },
	{ POT_MUTATION, "mutatsiya" },
	{ POT_SPEED, "skorost'" },
	{ POT_ACID, "kislota" },
	{ POT_OIL, "maslo" },
	{ POT_SALT_WATER, "solenaya voda" },
	{ POT_GAIN_ABILITY, "vozmozhnost' usileniya" },
	{ POT_GAIN_LEVEL, "uroven' povyshen" },
	{ POT_INVULNERABILITY, "neuyazvimost'" },
	{ POT_PARALYSIS, "paralich" },
	{ POT_EXTREME_POWER, "isklyuchitel'naya moshchnost'" },
	{ POT_RECOVERY, "vosstanovleniye" },
	{ POT_HEROISM, "doblest'" },
	{ POT_CYANIDE, "sol' tsianovoy kisloty" },
	{ POT_RADIUM, "radiy" },
	{ POT_JOLT_COLA, "el'f" },
	{ POT_PAN_GALACTIC_GARGLE_BLASTE, "mezhgalakticheskogo poloskaniye vodka" },
	{ POT_WATER, "voda" },
	{ POT_BLOOD, "krov'" },
	{ POT_VAMPIRE_BLOOD, "krov' vampira" },
	{ POT_AMNESIA, "poterya pamyati" },
	{ SCR_CREATE_MONSTER, "vyzov monstra" },
	{ SCR_SUMMON_UNDEAD, "prizvat' nezhit'" },
	{ SCR_TAMING, "ukroshcheniye" },
	{ SCR_LIGHT, "svet" },
	{ SCR_FOOD_DETECTION, "obnaruzhit' pishchu" },
	{ SCR_GOLD_DETECTION, "obnaruzhit' zoloto" },
	{ SCR_IDENTIFY, "identifitsirovat'" },
	{ SCR_INVENTORY_ID, "zavershit' opredelit'" },
	{ SCR_MAGIC_MAPPING, "magiya otobrazheniye" },
	{ SCR_FLOOD, "navodneniye" },
	{ SCR_GAIN_MANA, "usileniya many" },
	{ SCR_CONFUSE_MONSTER, "putayut chudovishche" },
	{ SCR_SCARE_MONSTER, "napugat' monstra" },
	{ SCR_ENCHANT_WEAPON, "chary dlya oruzhiya" },
	{ SCR_ENCHANT_ARMOR, "ocharovat' bronyu" },
	{ SCR_REMOVE_CURSE, "udalit' proklyatiye" },
	{ SCR_TELEPORTATION, "luchezarnyy" },
	{ SCR_TELE_LEVEL, "uroven' siyayushcheye" },
	{ SCR_WARPING, "iskrivleniye" },
	{ SCR_FIRE, "ogon'" },
	{ SCR_EARTH, "zemlya" },
	{ SCR_DESTROY_ARMOR, "unichtozhit' dospekhi" },
	{ SCR_AMNESIA, "poterya pamyati" },
	{ SCR_BAD_EFFECT, "plokhoye vliyaniye" },
	{ SCR_HEALING, "istseleniye" },
	{ SCR_MANA, "volshebnaya sila" },
	{ SCR_CURE, "lecheniye" },
	{ SCR_TRAP_DISARMING, "razoruzhit' lovushki" },
	{ SCR_STANDARD_ID, "identifikatsiya odin punkt" },
	{ SCR_CHARGING, "zaryadka" },
	{ SCR_GENOCIDE, "iskoreneniye" },
	{ SCR_PUNISHMENT, "nakazaniye" },
	{ SCR_STINKING_CLOUD, "vonyuchiy oblako" },
	{ SCR_TRAP_DETECTION, "obnaruzhit' lovushki" },
	{ SCR_ACQUIREMENT, "priobreteniye" },
	{ SCR_PROOF_ARMOR, "dokazatel'stvo broni" },
	{ SCR_PROOF_WEAPON, "dokazatel'stvo oruzhiye" },
	{ SCR_MASS_MURDER, "massovoye ubiystvo" },
	{ SCR_UNDO_GENOCIDE, "otmenit' genotsid" },
	{ SCR_REVERSE_IDENTIFY, "obratnaya opredelit'" },
	{ SCR_WISHING, "zhelayushchikh" },
	{ SCR_CONSECRATION, "osvyashcheniye" },
	{ SCR_ENTHRONIZATION, "postroit' tron" },
	{ SCR_FOUNTAIN_BUILDING, "fontan zdaniye" },
	{ SCR_SINKING, "tonushchiy" },
	{ SCR_WC, "ubornaya" },
	{ SCR_LAVA, "rasplavlennaya poroda" },
	{ SCR_GROWTH, "rost" },
	{ SCR_ICE, "led" },
	{ SCR_CLOUDS, "oblaka" },
	{ SCR_BARRHING, "za isklyucheniyem" },
	{ SCR_LOCKOUT, "zakhvat" },
	{ SCR_STONING, "pobivaniye kamnyami" },
	{ SCR_ROOT_PASSWORD_DETECTION, "obnaruzheniye koren' parol'" },
	{ SCR_TRAP_CREATION, "sozdayut lovushki" },
	{ SCR_SLEEP, "son" },
	{ SCR_BLANK_PAPER, "chistyy list bumagi" },
	{ SPE_FORCE_BOLT, "sila zadvizhka" },
	{ SPE_CREATE_MONSTER, "vyzov monstra" },
	{ SPE_DRAIN_LIFE, "pokhishcheniye zhizni" },
	{ SPE_COMMAND_UNDEAD, "komanda nezhit'" },
	{ SPE_SUMMON_UNDEAD, "prizvat' nezhit'" },
	{ SPE_DISINTEGRATION, "raspad" },
	{ SPE_STONE_TO_FLESH, "kamen' ploti" },
	{ SPE_HEALING, "istseleniye" },
	{ SPE_CURE_BLINDNESS, "lecheniye slepoty" },
	{ SPE_CURE_NUMBNESS, "lecheniye onemeniya" },
	{ SPE_BLIND_SELF, "slepoy samoupravleniya" },
	{ SPE_CURE_SICKNESS, "lecheniye bolezni" },
	{ SPE_CURE_HALLUCINATION, "lecheniye gallyutsinatsii" },
	{ SPE_CURE_CONFUSION, "lecheniye putanitsa" },
	{ SPE_CURE_BURN, "lecheniye ozhogov" },
	{ SPE_CONFUSE_SELF, "zaputat' sebya" },
	{ SPE_CURE_STUN, "lecheniye oglushayet" },
	{ SPE_STUN_SELF, "oglushayet samoupravleniya" },
	{ SPE_EXTRA_HEALING, "dopolnitel'naya istseleniye" },
	{ SPE_FULL_HEALING, "polnogo zazhivleniya" },
	{ SPE_RESTORE_ABILITY, "vosstanovit' sposobnost'" },
	{ SPE_BANISHING_FEAR, "izgonyaya strakh" },
	{ SPE_CURE_FREEZE, "lecheniye zamorazhivaniya" },
	{ SPE_CREATE_FAMILIAR, "sozdat' znakomy" },
	{ SPE_LIGHT, "svet" },
	{ SPE_DARKNESS, "t'ma" },
	{ SPE_DETECT_MONSTERS, "obnaruzhit' monstrov" },
	{ SPE_DETECT_FOOD, "obnaruzhit' pishchu" },
	{ SPE_DISSOLVE_FOOD, "rastvorit' pishchu" },
	{ SPE_DETECT_ARMOR_ENCHANTMENT, "obnaruzhit' broni chary" },
	{ SPE_CLAIRVOYANCE, "yasnovideniye" },
	{ SPE_DETECT_UNSEEN, "obnaruzhit' nevidimyy" },
	{ SPE_IDENTIFY, "identifitsirovat'" },
	{ SPE_DETECT_TREASURE, "obnaruzhit' sokrovishcha" },
	{ SPE_MAGIC_MAPPING, "magiya otobrazheniye" },
	{ SPE_ENTRAPPING, "obnaruzhit' lovushki" },
	{ SPE_FINGER, "palets" },
	{ SPE_CHEMISTRY, "khimiya" },
	{ SPE_DETECT_FOOT, "obnaruzhit' nogu" },
	{ SPE_FORBIDDEN_KNOWLEDGE, "zapreshcheno znaniya" },
	{ SPE_CONFUSE_MONSTER, "putayut chudovishche" },
	{ SPE_SLOW_MONSTER, "medlenno monstr" },
	{ SPE_CAUSE_FEAR, "strakh" },
	{ SPE_CHARM_MONSTER, "ukroshcheniye" },
	{ SPE_ENCHANT_WEAPON, "chary dlya oruzhiya" },
	{ SPE_ENCHANT_ARMOR, "ocharovat' bronyu" },
	{ SPE_CHARGING, "zaryadka" },
	{ SPE_GENOCIDE, "iskoreneniye" },
	{ SPE_PROTECTION, "zashchita" },
	{ SPE_RESIST_POISON, "soprotivleniye otravleniyu" },
	{ SPE_RESIST_SLEEP, "protivostoyat' son" },
	{ SPE_ENDURE_COLD, "terpet' kholod" },
	{ SPE_ENDURE_HEAT, "terpet' zharu" },
	{ SPE_INSULATE, "izolirovat'" },
	{ SPE_REMOVE_CURSE, "udalit' proklyatiye" },
	{ SPE_REMOVE_BLESSING, "udalit' blagosloveniye" },
	{ SPE_TURN_UNDEAD, "povorot nezhit'" },
	{ SPE_ANTI_DISINTEGRATION, "ni raspad" },
	{ SPE_BOTOX_RESIST, "botoks soprotivlyat'sya" },
	{ SPE_ACIDSHIELD, "kisloty shchit" },
	{ SPE_GODMODE, "rezhim boga" },
	{ SPE_RESIST_PETRIFICATION, "protivostoyat' okameneniya" },
	{ SPE_JUMPING, "pryzhki" },
	{ SPE_HASTE_SELF, "pospeshnost' samoupravleniya" },
	{ SPE_ENLIGHTEN, "prosvetit'" },
	{ SPE_INVISIBILITY, "nevidimost'" },
	{ SPE_AGGRAVATE_MONSTER, "usugubit' vragov" },
	{ SPE_LEVITATION, "svobodnoye pareniye" },
	{ SPE_TELEPORT_AWAY, "teleportirovat'sya" },
	{ SPE_PASSWALL, "prokhod stena" },
	{ SPE_POLYMORPH, "prevrashchat'" },
	{ SPE_MUTATION, "mutatsiya" },
	{ SPE_LEVELPORT, "uroven' siyayushcheye" },
	{ SPE_KNOCK, "stuchat'" },
	{ SPE_FLAME_SPHERE, "sfera plameni" },
	{ SPE_FREEZE_SPHERE, "zamorozit' sfera" },
	{ SPE_SHOCKING_SPHERE, "shokiruyet sfera" },
	{ SPE_ACID_SPHERE, "kislota sfera" },
	{ SPE_WIZARD_LOCK, "master blokirovki" },
	{ SPE_DIG, "kopat'" },
	{ SPE_CANCELLATION, "annulirovaniye" },
	{ SPE_REFLECTION, "otrazheniye" },
	{ SPE_PARALYSIS, "paralich" },
	{ SPE_REPAIR_ARMOR, "remont broni" },
	{ SPE_CORRODE_METAL, "korroziyu metalla" },
	{ SPE_PETRIFY, "stolbenet'" },
	{ SPE_MAGIC_MISSILE, "magicheskaya raketa" },
	{ SPE_FIREBALL, "ognennyy shar" },
	{ SPE_CONE_OF_COLD, "konus kholoda" },
	{ SPE_SLEEP, "son" },
	{ SPE_FINGER_OF_DEATH, "luch smerti" },
	{ SPE_LIGHTNING, "molniya" },
	{ SPE_POISON_BLAST, "yad vzryv" },
	{ SPE_ACID_STREAM, "potok kisloty" },
	{ SPE_SOLAR_BEAM, "solnechnaya luch" },
	{ SPE_BLANK_PAPER, "chistyy list bumagi" },
	{ SPE_STINKING_CLOUD, "vonyuchiy oblako" },
	{ SPE_TIME_STOP, "vremya ostanovki" },
	{ SPE_MAP_LEVEL, "uroven' otobrazheniye" },
	{ SPE_GAIN_LEVEL, "uroven' povyshen" },
	{ WAN_LIGHT, "svet" },
	{ WAN_NOTHING, "nichego" },
	{ WAN_ENLIGHTENMENT, "prosveshcheniye" },
	{ WAN_HEALING, "istseleniye" },
	{ WAN_LOCKING, "blokirovka" },
	{ WAN_MAKE_INVISIBLE, "sdelat' nevidimym" },
	{ WAN_MAKE_VISIBLE, "sdelat' vidimym" },
	{ WAN_IDENTIFY, "identifitsirovat'" },
	{ WAN_REMOVE_CURSE, "udalit' proklyatiye" },
	{ WAN_PUNISHMENT, "nakazaniye" },
	{ WAN_OPENING, "otkrytiye" },
	{ WAN_PROBING, "zondirovaniye" },
	{ WAN_SECRET_DOOR_DETECTION, "skrytyy obnaruzheniya dver'" },
	{ WAN_TRAP_DISARMING, "razoruzhit' lovushki" },
	{ WAN_ENTRAPPING, "obnaruzhit' lovushki" },
	{ WAN_STINKING_CLOUD, "vonyuchiy oblako" },
	{ WAN_TIME_STOP, "vremya ostanovki" },
	{ WAN_GENOCIDE, "iskoreneniye" },
	{ WAN_TELE_LEVEL, "uroven' siyayushcheye" },
	{ WAN_TRAP_CREATION, "sozdayut lovushki" },
	{ WAN_DARKNESS, "t'ma" },
	{ WAN_MAGIC_MAPPING, "magiya otobrazheniye" },
	{ WAN_DETECT_MONSTERS, "obnaruzhit' monstrov" },
	{ WAN_OBJECTION, "obnaruzhivat' ob'yekty" },
	{ WAN_SLOW_MONSTER, "medlenno monstr" },
	{ WAN_SPEED_MONSTER, "skorost' monstra" },
	{ WAN_HASTE_MONSTER, "pospeshnost' monstr" },
	{ WAN_STRIKING, "porazitel'nyy" },
	{ WAN_PARALYSIS, "paralich" },
	{ WAN_DISINTEGRATION, "raspad" },
	{ WAN_STONING, "pobivaniye kamnyami" },
	{ WAN_UNDEAD_TURNING, "nezhit' povorotnym" },
	{ WAN_DRAINING, "osusheniye" },
	{ WAN_CANCELLATION, "annulirovaniye" },
	{ WAN_CREATE_MONSTER, "vyzov monstra" },
	{ WAN_BAD_EFFECT, "plokhoye vliyaniye" },
	{ WAN_CURSE_ITEMS, "proklyatiye predmety" },
	{ WAN_AMNESIA, "poterya pamyati" },
	{ WAN_BAD_LUCK, "neudacha" },
	{ WAN_REMOVE_RESISTANCE, "udalit' soprotivleniye" },
	{ WAN_CORROSION, "rzhavleniye" },
	{ WAN_FUMBLING, "nelovkiy" },
	{ WAN_STARVATION, "golodaniye" },
	{ WAN_SUMMON_UNDEAD, "prizvat' nezhit'" },
	{ WAN_FEAR, "strakh" },
	{ WAN_WIND, "veter" },
	{ WAN_POLYMORPH, "prevrashchat'" },
	{ WAN_MUTATION, "mutatsiya" },
	{ WAN_TELEPORTATION, "luchezarnyy" },
	{ WAN_BANISHMENT, "izgnaniye" },
	{ WAN_CREATE_HORDE, "sozdat' ordu" },
	{ WAN_EXTRA_HEALING, "dopolnitel'naya istseleniye" },
	{ WAN_FULL_HEALING, "polnogo zazhivleniya" },
	{ WAN_WONDER, "udivlyat'sya" },
	{ WAN_BUGGING, "ustanovka apparatury dlya taynogo nablyudeniya" },
	{ WAN_WISHING, "zhelayushchikh" },
	{ WAN_ACQUIREMENT, "priobreteniye" },
	{ WAN_CLONE_MONSTER, "umnozhit' monstr" },
	{ WAN_CHARGING, "zaryadka" },
	{ WAN_DIGGING, "ryt'ye" },
	{ WAN_MAGIC_MISSILE, "magicheskaya raketa" },
	{ WAN_FIRE, "ogon'" },
	{ WAN_COLD, "kholodnyy" },
	{ WAN_SLEEP, "son" },
	{ WAN_DEATH, "smert'" },
	{ WAN_LIGHTNING, "molniya" },
	{ WAN_FIREBALL, "ognennyy shar" },
	{ WAN_ACID, "kislota" },
	{ WAN_SOLAR_BEAM, "solnechnaya luch" },
	{ WAN_MANA, "volshebnaya sila" },
	{ WAN_GAIN_LEVEL, "uroven' povyshen" },

	{ BANANA, "obez'yana yedy" },
	{ RIGHT_MOUSE_BUTTON_STONE, "kamen' pravoy knopkoy myshi knopku poteri" },
	{ DISPLAY_LOSS_STONE, "poterya displey kamen'" },
	{ SPELL_LOSS_STONE, "zaklinaniye poteri kamen'" },
	{ YELLOW_SPELL_STONE, "zheltyy zaklinaniye kamen'" },
	{ AUTO_DESTRUCT_STONE, "avto razrushat' kamen'" },
	{ MEMORY_LOSS_STONE, "poterya pamyati kamen'" },
	{ INVENTORY_LOSS_STONE, "poterya Kamennyy inventar'" },
	{ BLACKY_STONE, "bleki kamen'" },
	{ MENU_BUG_STONE, "menyu oshibka kamen'" },
	{ SPEEDBUG_STONE, "skorost' oshibka kamen'" },
	{ SUPERSCROLLER_STONE, "bol'shoy kamen' skroller" },
	{ FREE_HAND_BUG_STONE, "svobodnoy rukoy oshibka kamen'" },
	{ UNIDENTIFY_STONE, "vernut'sya opredelit' kamen'" },
	{ STONE_OF_THIRST, "kamen' ot zhazhdy" },
	{ UNLUCKY_STONE, "povezlo kamen'" },
	{ SHADES_OF_GREY_STONE, "ottenki serogo kamnya" },
	{ STONE_OF_FAINTING, "kamen' obmoroka" },
	{ STONE_OF_CURSING, "kamen' zlosloviya" },
	{ STONE_OF_DIFFICULTY, "kamen' slozhnosti" },
	{ DEAFNESS_STONE, "glukhota kamen'" },
	{ ANTIMAGIC_STONE, "zaklinatel' problema kamen'" },
	{ WEAKNESS_STONE, "slabost' kamen'" },
	{ ROT_THIRTEEN_STONE, "gnit' trinadtsat' kamen'" },
	{ BISHOP_STONE, "yepiskop kamen'" },
	{ CONFUSION_STONE, "putanitsa kamen'" },
	{ DROPBUG_STONE, "padeniye oshibke kamen'" },
	{ DSTW_STONE, "ne pokhozhe na rabotu kamen'" },
	{ STATUS_STONE, "tyazhelyy kamen' bolezn'" },
	{ ALIGNMENT_STONE, "vyravnivaniye kamen'" },
	{ STAIRSTRAP_STONE, "lestnitsy lovushka kamen'" },
	{ UNINFORMATION_STONE, "kamen' nedostayushchey informatsii" },
	{ WAN_CREATE_FAMILIAR, "sozdat' znakomy" },
	{ SCR_CREATE_FAMILIAR, "sozdat' znakomy" },
	{ RIN_INTRINSIC_LOSS, "vnutrennyaya poterya" },
	{ RIN_TRAP_REVEALING, "lovushka pokazatel'nym" },
	{ RIN_BLOOD_LOSS, "poterya krovi" },
	{ RIN_NASTINESS, "zlobnost'" },
	{ RIN_BAD_EFFECT, "plokhiye veshchi" },
	{ RIN_SUPERSCROLLING, "bol'shoy prokrutka" },
	{ AMULET_OF_RMB_LOSS, "amulet of pravoy knopkoy myshi knopku otkaz" },
	{ AMULET_OF_ITEM_TELEPORTATION, "amulet of punkt pereseleniye" },
	{ HELM_OF_OBSCURED_DISPLAY, "shlem zavualiro-displeye" },
	{ HELM_OF_LOSE_IDENTIFICATION, "shlem teryayut identifikatsii" },
	{ HELM_OF_THIRST, "shlem ot zhazhdy" },
	{ BLACKY_HELMET, "bleki shlem" },
	{ ANTI_DRINKER_HELMET, "predotvratit' kompaniyakh izredka shlem" },
	{ WHISPERING_HELMET, "shepchet shlem" }, /* rumors a la trap %s */
	{ CYPHER_HELM, "shifr rulya" },
	{ HELM_OF_BAD_ALIGNMENT, "shlem plokhoy vyravnivaniya" },
	{ SOUNDPROOF_HELMET, "zvukoizolyatsionnyye shlem" },
	{ OUT_OF_MEMORY_HELMET, "iz shlema pamyati" },
	{ CLOAK_OF_UNSPELLING, "spina ne-pravopisaniya" },
	{ ANTI_CASTER_CLOAK, "predotvratit' zaklinaniy plashch" },
	{ HEAVY_STATUS_CLOAK, "tyazhelaya bolezn' plashch" },
	{ CLOAK_OF_LUCK_NEGATION, "plashch udachi otritsaniya" },
	{ YELLOW_SPELL_CLOAK, "zheltyy plashch zaklinaniye" },
	{ VULNERABILITY_CLOAK, "uyazvimost' plashch" },
	{ CLOAK_OF_INVENTORYLESSNESS, "plashch bez inventarizatsii" },
	{ MENU_NOSE_GLOVES, "nos menyu perchatki" },
	{ UNWIELDY_GLOVES, "gromozdkiye perchatki" },
	{ CONFUSING_GLOVES, "zaputannoy perchatki" },
	{ UNDROPPABLE_GLOVES, "perchatki kotoryye ne mogut byt' udaleny" },
	{ GAUNTLETS_OF_MISSING_INFORMATI, "perchatki nedostayushchey informatsii" },
	{ GAUNTLETS_OF_TRAP_CREATION, "perchatki sozdaniya lovushki" },
	{ SADO_MASO_GLOVES, "sadistskiye perchatki" },
	{ AUTODESTRUCT_DE_VICE_BOOTS, "avtomaticheskoye ustroystvo unichtozheniye" },
	{ SPEEDBUG_BOOTS, "skorost' oshibka sapogi" },
	{ SENTIENT_HIGH_HEELED_SHOES, "zhivyye vysokiye kabluki" },
	{ BOOTS_OF_FAINTING, "sapogi obmoroka" },
	{ DIFFICULT_BOOTS, "slozhnyye sapogi" },
	{ BOOTS_OF_WEAKNESS, "sapogi slabosti" },
	{ GRIDBUG_CONDUCT_BOOTS, "setka oshibka provodit' sapogi" },
	{ STAIRWELL_STOMPING_BOOTS, "lestnichnaya kletka topat' sapogi" },

	{ STONE_OF_INTRINSIC_LOSS, "vnutrennyaya poterya kamen'" },
	{ BLOOD_LOSS_STONE, "poterya krovi kamen'" },
	{ BAD_EFFECT_STONE, "plokhoye vliyaniye kamen'" },
	{ TRAP_CREATION_STONE, "sozdayut lovushki kamen'" },
	{ STONE_OF_VULNERABILITY, "uyazvimost' kamen'" },
	{ ITEM_TELEPORTING_STONE, "punkt pereseleniye kamen'" },
	{ NASTY_STONE, "zlobnost' kamen'" },

	{ RIN_INFRAVISION, "infravideniye" },
	{ SCR_PHASE_DOOR, "faza dver'" },
	{ SCR_RANDOM_ENCHANTMENT, "razrusheniye char" },
	{ WAN_INCREASE_MAX_HITPOINTS, "povysheniye zdorov'ya" },
	{ WAN_REDUCE_MAX_HITPOINTS, "snizheniye zdorov'ya" },
	{ SCALE_MAIL, "shkala pochty" },
	{ ACID_VENOM, "kisloty yad" },
	{ ARROW, "strelka" },
	{ GAUNTLETS_OF_SWIMMING, "rukavitsy plavaniya" },
	{ DWARVISH_MITHRIL_COAT, "gnomov mifrilovaya pal'to" },
	{ ELVEN_MITHRIL_COAT, "el'fiyskiy mifrilovaya pal'to" },
	{ STUDDED_LEATHER_ARMOR, "shipovannykh kozhanyy dospekh" },
	{ MAGIC_WHISTLE, "magiya svistok" },
	{ TIN_WHISTLE, "svistul'ka" },
	{ JUMPING_BOOTS, "pryzhki sapogi" },
	{ HELM_OF_BRILLIANCE, "blestyashchiy shlem" },
	{ OILSKIN_SACK, "nepromokayemyy plashch meshok" },
	{ BAG_OF_DIGESTION, "meshok pishchevareniya" },
	{ DIODE, "poluprovodnik" },
	{ TRANSISTOR, "kristallotriod" },
	{ IC, "integral'naya skhema" },
	{ BOTTLE, "butylka" },
	{ ICE_BOX, "korobka l'da" },
	{ SKELETON_KEY, "skeletnyy klyuch" },
	{ ELVEN_ARROW, "el'fiyskaya strela" },
	{ AKLYS, "stringi klub" },
	{ YUMI, "yaponskiy luk" },
	{ YA, "yaponskiy strelka" },

	{ ORCISH_DAGGER, "orkov kinzhal" },
	{ DROVEN_DAGGER, "steklo kinzhal" },
	{ ATHAME, "ritual kinzhal" },
	{ ELVEN_DAGGER, "el'fiyskiy kinzhal" },
	{ DARK_ELVEN_DAGGER, "el'fiyskiy kinzhal temno" },
	{ GREAT_DAGGER, "bol'shoy kinzhal" },
	{ TOOTH_OF_AN_ALGOLIAN_SUNTIGER, "ochen' ostryy zub" },
	{ ORCISH_SHORT_SWORD, "orkov korotkiy mech" },
	{ SILVER_SHORT_SWORD, "serebro korotkiy mech" },
	{ DWARVISH_SHORT_SWORD, "gnomov korotkiy mech" },
	{ ELVEN_SHORT_SWORD, "el'fiyskiy korotkiy mech" },
	{ DROVEN_SHORT_SWORD, "steklo korotkiy mech" },
	{ DARK_ELVEN_SHORT_SWORD, "temnykh el'fov korotkiy mech" },
	{ ELVEN_BROADSWORD, "el'fiyskiy shirokiy mech" },
	{ SILVER_LONG_SWORD, "serebro dlinnyy mech" },
	{ DROVEN_GREATSWORD, "steklo dvuruchnyy mech" },
	{ BENT_SABLE, "sognuty sobol'" },
	{ RAPIER, "rapira" },
	{ METAL_CLUB, "metall klub" },
	{ REINFORCED_MACE, "usilennyy bulava" },
	{ KNOUT, "knut" },
	{ OBSID, "bich tsep" },
	{ HEAVY_HAMMER, "tyazhelyy molotok" },
	{ ORCISH_SPEAR, "orkov kop'ye" },
	{ DROVEN_SPEAR, "steklo kop'ye" },
	{ DWARVISH_SPEAR, "gnomov kop'ye" },
	{ ELVEN_SPEAR, "el'fiyskiy kop'ye" },
	{ COURSE_LANCE, "konechno kop'ye" },
	{ FORCE_PIKE, "sila shchuka" },
	{ DROVEN_LANCE, "steklo ostroga" },
	{ ORCISH_BOW, "orkov luk" },
	{ ELVEN_BOW, "el'fiyskiy luk" },
	{ DARK_ELVEN_BOW, "el'fiyskiy luk temno" },
	{ DROVEN_BOW, "steklo luk" },
	{ ORCISH_ARROW, "orkov strelka" },
	{ SILVER_ARROW, "serebryanaya strela" },
	{ ELVEN_ARROW, "el'fiyskiy strela" },
	{ DROVEN_ARROW, "steklo strelka" },
	{ DARK_ELVEN_ARROW, "temnykh el'fov strelki" },
	{ CATAPULT, "katapul'ta" },
	{ SILVER_BULLET, "serebryanaya pulya" },
	{ GAS_GRENADE, "gazovoy granatoy" },
	{ STICK_OF_DYNAMITE, "palka dinamita" },
	{ DROVEN_CROSSBOW, "steklo arbalet" },
	{ DEMON_CROSSBOW, "d'yavol arbalet" },
	{ DROVEN_BOLT, "arbalet boyepripasy stekla" },
	{ BOOMERANG, "bumerang" },
	{ HAWAIIAN_SHIRT, "gavayskoy rubashke" },
	{ DROVEN_PLATE_MAIL, "steklyannaya plastina pochty" },
	{ BANDED_MAIL, "ob''yedinilis' pochty" },
	{ DARK_ELVEN_MITHRIL_COAT, "temnykh el'fov mifrilovyye pal'to" },
	{ GNOMISH_SUIT, "gnomov kostyum" },
	{ CHAIN_MAIL, "kol'chuga" },
	{ DROVEN_CHAIN_MAIL, "steklo kol'chuga" },
	{ ORCISH_CHAIN_MAIL, "orkov kol'chuga" },
	{ RING_MAIL, "kol'chuzhnaya" },
	{ ORCISH_RING_MAIL, "orkov kol'tso pochta" },
	{ LEATHER_ARMOR, "kozhanyy dospekh" },
	{ ROBE_OF_PROTECTION, "khalat zashchity" },
	{ ROBE_OF_POWER, "khalat vlasti" },
	{ ROBE_OF_WEAKNESS, "khalat slabosti" },
	{ GRAY_DRAGON_SCALE_MAIL, "seryy drakon masshtab pochty" },
	{ SILVER_DRAGON_SCALE_MAIL, "serebryanyy drakon masshtab pochty" },
	{ MERCURIAL_DRAGON_SCALE_MAIL, "rtutnyy drakon masshtab pochty" },
	{ SHIMMERING_DRAGON_SCALE_MAIL, "mertsayushchiy drakon masshtab pochty" },
	{ DEEP_DRAGON_SCALE_MAIL, "gluboko drakon masshtab pochty" },
	{ RED_DRAGON_SCALE_MAIL, "krasnyy drakon masshtab pochty" },
	{ WHITE_DRAGON_SCALE_MAIL, "belyy drakon masshtab pochty" },
	{ ORANGE_DRAGON_SCALE_MAIL, "oranzhevyy drakon masshtab pochty" },
	{ BLACK_DRAGON_SCALE_MAIL, "chernyy drakon masshtab pochty" },
	{ BLUE_DRAGON_SCALE_MAIL, "siniy drakon masshtab pochty" },
	{ GREEN_DRAGON_SCALE_MAIL, "zelenyy drakon masshtab pochty" },
	{ GOLDEN_DRAGON_SCALE_MAIL, "zolotoy drakon masshtab pochty" },
	{ STONE_DRAGON_SCALE_MAIL, "shkala kamen' drakona pochty" },
	{ CYAN_DRAGON_SCALE_MAIL, "goluboy drakon masshtab pochty" },
	{ YELLOW_DRAGON_SCALE_MAIL, "zheltyy drakon masshtab pochty" },
	{ GRAY_DRAGON_SCALES, "seryye shkaly drakona" },
	{ SILVER_DRAGON_SCALES, "serebryanyye vesy drakon" },
	{ MERCURIAL_DRAGON_SCALES, "rtutnyye vesy drakon" },
	{ SHIMMERING_DRAGON_SCALES, "mertsayushchiye vesy drakon" },
	{ DEEP_DRAGON_SCALES, "glubokiye vesy drakon" },
	{ RED_DRAGON_SCALES, "krasnyy drakon vesy" },
	{ WHITE_DRAGON_SCALES, "belyye cheshuyki drakona" },
	{ ORANGE_DRAGON_SCALES, "oranzhevyye vesy drakon" },
	{ BLACK_DRAGON_SCALES, "chernyye cheshuyki drakona" },
	{ BLUE_DRAGON_SCALES, "siniy drakon vesy" },
	{ GREEN_DRAGON_SCALES, "zelenyye cheshuyki drakona" },
	{ GOLDEN_DRAGON_SCALES, "zolotyye vesy drakon" },
	{ STONE_DRAGON_SCALES, "kamen' drakon vesy" },
	{ CYAN_DRAGON_SCALES, "goluboy drakon vesy" },
	{ YELLOW_DRAGON_SCALES, "zheltyye cheshuyki drakona" },
	{ ORCISH_CLOAK, "orkov plashch" },
	{ DWARVISH_CLOAK, "gnomov plashch" },
	{ OILSKIN_CLOAK, "tonkaya kleyenka plashch" },
	{ ELVEN_CLOAK, "el'fiyskiy plashch" },
	{ DROVEN_CLOAK, "steklo plashch" },
	{ PLASTEEL_CLOAK, "plastikovyye plashch" },
	{ CLOAK_OF_PROTECTION, "plashch zashchity" },
	{ CLOAK_OF_DEATH, "plashch smerti" },
	{ CLOAK_OF_INVISIBILITY, "plashch nevidimosti" },
	{ CLOAK_OF_FUMBLING, "spina bormotat'" },
	{ CLOAK_OF_MAGIC_RESISTANCE, "plashch soprotivleniya magii" },
	{ CLOAK_OF_DRAIN_RESISTANCE, "plashch soprotivleniya slivnoy" },
	{ CLOAK_OF_REFLECTION, "plashch otrazheniya" },
	{ MANACLOAK, "many plashch" },
	{ CLOAK_OF_CONFUSION, "plashch smyateniya" },
	{ CLOAK_OF_WARMTH, "spina teplo" },
	{ CLOAK_OF_GROUNDING, "plashch zazemleniya" },
	{ CLOAK_OF_QUENCHING, "plashch tusheniya" },
	{ CLOAK_OF_RESPAWNING, "spina pakov" },
	{ SPAWN_CLOAK, "ikru plashch" },
	{ ADOM_CLOAK, "drevniye domeny zagadochnym plashch" },
	{ EGOIST_CLOAK, "egoizm plashch" },
	{ CLOAK_OF_TIME, "spina vremya" },
	{ CLOAK_OF_DISPLACEMENT, "plashch peremeshcheniya" },
	{ ELVEN_LEATHER_HELM, "el'fiyskiy shlem kozha" },
	{ GNOMISH_HELM, "gnom shlem" },
	{ ORCISH_HELM, "shlem orkov" },
	{ DWARVISH_IRON_HELM, "gnomov zheleznyy shlem" },
	{ DROVEN_HELM, "steklo rulya" },
	{ CORNUTHAUM, "master rulya" },
	{ PLASTEEL_HELM, "plastik rulya" },
	{ HELM_OF_DETECT_MONSTERS, "shlem obnaruzhit' monstrov" },
	{ HELM_OF_STORMS, "shlem bur'" },
	{ HELM_OF_STEEL, "shlem iz stali" },
	{ HELM_OF_DRAIN_RESISTANCE, "shlem soprotivleniya slivnoy" },
	{ HELM_OF_FEAR, "shlem strakha" },
	{ HELM_OF_HUNGER, "shlem goloda" },
	{ HELM_OF_DISCOVERY, "shlem otkrytiya" },
	{ FIRE_HELMET, "ogon' shlem" },
	{ ANGER_HELM, "gnev rulya" },
	{ CAPTCHA_HELM, "avtomatizirovannyy publichnyy test rulya" },
	{ HELM_OF_AMNESIA, "shlem amnezii" },
	{ HELM_OF_SENSORY_DEPRIVATION, "shlem sensornoy deprivatsii" },
	{ BIGSCRIPT_HELM, "bol'shoy shlem stsenariy" },
	{ QUIZ_HELM, "test rulya" },
	{ DIZZY_HELMET, "golovokruzheniye shlem" },
	{ MUTING_HELM, "priglusheniya rulya" },
	{ ULCH_HELMET, "ul'chskogo shlem" },
	{ HELM_OF_TELEPATHY, "shlem telepatii" },
	{ PLASTEEL_GLOVES, "plastikovyye perchatki" },
	{ GAUNTLETS_OF_PANIC, "rukavitsy paniki" },
	{ GAUNTLETS_OF_SLOWING, "rukavitsy zamedleniyem" },
	{ OILSKIN_GLOVES, "kleyenchatom perchatki" },
	{ GAUNTLETS_OF_TYPING, "rukavitsy tipirovaniya" },
	{ GAUNTLETS_OF_STEEL, "rukavitsy stali" },
	{ GAUNTLETS_OF_REFLECTION, "rukavitsy otrazheniya" },
	{ GAUNTLETS_OF_THE_FORCE, "rukavitsy sily" },
	{ BANKING_GLOVES, "bankovskiye perchatki" },
	{ DIFFICULT_GLOVES, "trudnyye perchatki" },
	{ CHAOS_GLOVES, "khaos perchatki" },
	{ GAUNTLETS_OF_DEXTERITY, "rukavitsy lovkosti" },
	{ ELVEN_SHIELD, "el'fiyskiy shchit" },
	{ URUK_HAI_SHIELD, "uruk-khay shchit" },
	{ ORCISH_SHIELD, "orkov shchit" },
	{ LARGE_SHIELD, "bol'shoy shchit" },
	{ DWARVISH_ROUNDSHIELD, "gnomov kruglyy shchit" },
	{ FLAME_SHIELD, "plamya shchit" },
	{ ICE_SHIELD, "led shchit" },
	{ VENOM_SHIELD, "yad shchit" },
	{ LIGHTNING_SHIELD, "shchit molniy" },
	{ SHIELD_OF_LIGHT, "shchit sveta" },
	{ GRAY_DRAGON_SCALE_SHIELD, "seryy drakon'yey shchit" },
	{ SILVER_DRAGON_SCALE_SHIELD, "serebro drakon'yey shchit" },
	{ MERCURIAL_DRAGON_SCALE_SHIELD, "rtutnyy drakon'yey shchit" },
	{ SHIMMERING_DRAGON_SCALE_SHIELD, "mertsayushchiy shchit drakon'yey" },
	{ DEEP_DRAGON_SCALE_SHIELD, "gluboko drakon'yey shchit" },
	{ RED_DRAGON_SCALE_SHIELD, "krasnyy shchit drakon'yey" },
	{ WHITE_DRAGON_SCALE_SHIELD, "belyy shchit drakon'yey" },
	{ ORANGE_DRAGON_SCALE_SHIELD, "oranzhevyy drakon'yey shchit" },
	{ BLACK_DRAGON_SCALE_SHIELD, "chernyy drakon'yey shchit" },
	{ BLUE_DRAGON_SCALE_SHIELD, "siniy shchit drakon'yey" },
	{ GREEN_DRAGON_SCALE_SHIELD, "zelenyy shchit drakon'yey" },
	{ GOLDEN_DRAGON_SCALE_SHIELD, "zolotoy shchit drakon'yey" },
	{ STONE_DRAGON_SCALE_SHIELD, "kamen' drakon'yey shchit" },
	{ CYAN_DRAGON_SCALE_SHIELD, "goluboy shchit drakon'yey" },
	{ YELLOW_DRAGON_SCALE_SHIELD, "zheltyy shchit drakon'yey" },
	{ PLASTEEL_BOOTS, "plastikovyye botinki" },
	{ LOW_BOOTS, "polubotinki" },
	{ IRON_SHOES, "zheleza obuv'" },
	{ GNOMISH_BOOTS, "gnomskiye sapogi" },
	{ HIGH_BOOTS, "vysokiye botinki" },
	{ SPEED_BOOTS, "skorost' sapogi" },
	{ BOOTS_OF_MOLASSES, "sapogi melassy" },
	{ FLYING_BOOTS, "letayushchiye botinki" },
	{ ELVEN_BOOTS, "el'fiyskiye sapogi" },
	{ KICKING_BOOTS, "butsy" },
	{ DISENCHANTING_BOOTS, "pokupayetsya sapogi" },
	{ LEVITATION_BOOTS, "levitatsii sapogi" },
	{ RIN_DRAIN_RESISTANCE, "slit' soprotivleniye" },
	{ RIN_MEMORY, "pamyat'" },
	{ RIN_FAST_METABOLISM, "bystro obmen veshchestv" },
	{ RIN_DISENGRAVING, "udalit' gravirovka" },
	{ RIN_NO_SKILL, "net navyk" },
	{ RIN_LOW_STATS, "nizkiye statistika" },
	{ RIN_FAILED_TRAINING, "ne udalos' obucheniya" },
	{ RIN_FAILED_EXERCISE, "ne udalos' uprazhneniya" },
	{ AMULET_OF_POLYMORPH, "amulet of polimorf" },
	{ AMULET_OF_PREMATURE_DEATH, "amulet of prezhdevremennaya smert'" },
	{ AMULET_OF_ANTI_TELEPORTATION, "amulet of predotvratit' teleportatsii" },
	{ AMULET_OF_EXPLOSION, "amulet of vzryv" },
	{ AMULET_OF_WRONG_SEEING, "amulet of nepravil'no videniye" },
	{ AMULET_OF_YENDOR, "amulet of astral'nyye plany" },
	{ FAKE_AMULET_OF_YENDOR, "ne nastoyashchiye amulet of astral'nyye plany" },
	{ CREDIT_CARD, "kreditnaya karta" },
	{ MAGIC_CANDLE, "magiya svechi" },
	{ OIL_LAMP, "maslyanaya lampa" },
	{ MAGIC_LAMP, "volshebnaya lampa" },
	{ MAGIC_FLUTE, "volshebnaya fleyta" },
	{ TOOLED_HORN, "tisneniyem rog" },
	{ FIRE_HORN, "ogon' rog" },
	{ FROST_HORN, "moroz rog" },
	{ HORN_OF_PLENTY, "rog izobiliya" },
	{ MAGIC_HARP, "volshebnaya arfa" },
	{ BELL, "kolokol" },
	{ LEATHER_DRUM, "kozha barabana" },
	{ DRUM_OF_EARTHQUAKE, "baraban zemletryaseniya" },
	{ FISHING_POLE, "udochka" },
	{ GREEN_LIGHTSABER, "zelenyy svetovoy" },
	{ BLUE_LIGHTSABER, "siniy svetovoy mech" },
	{ RED_LIGHTSABER, "krasnyy svetovoy mech" },
	{ RED_DOUBLE_LIGHTSABER, "krasnyy dvoynoy svetovoy mech" },
	{ FELT_TIP_MARKER, "flomasterom" },
	{ HITCHHIKER_S_GUIDE_TO_THE_GALA, "avtostopom po galaktike" },
	{ PACK_OF_FLOPPIES, "paket disket" },
	{ GOD_O_METER, "detektor taym-aut molitva" },
	{ RELAY, "rele" },
	{ CHEMISTRY_SET, "cunyy khimik" },
	{ CANDELABRUM_OF_INVOCATION, "kandelyabr prizyva" },
	{ BELL_OF_OPENING, "kolokol otkrytiya" },
	{ MEATBALL, "frikadel'ka" },
	{ MEAT_STICK, "myaso palku" },
	{ HUGE_CHUNK_OF_MEAT, "ogromnyy kusok myasa" },
	{ MEAT_RING, "myaso kol'tso" },
	{ EYEBALL, "glaznoye yabloko" },
	{ SEVERED_HAND, "otrublennaya ruka" },
	{ PEANUT_BAG, "arakhisovoye meshok" },
	{ TORTILLA, "ploskaya maisovaya lepeshka" },
	{ UGH_MEMORY_TO_CREATE_INVENTORY, "chno pamyati dlya sozdaniya okna inventarizatsii" },
	{ TWELVE_COURSE_DINNER, "dvenadtsat' blyud" },
	{ PILL, "tabletka" },
	{ K_RATION, "vtoraya mirovaya voyna ratsion" },
	{ C_RATION, "amerikanskiye voyennyye ratsion" },
	{ POT_FIRE_RESISTANCE, "ognestoykost'" },
	{ SCR_CHAOS_TERRAIN, "khaos mestnosti" },
	{ SCR_REPAIR_ITEM, "remont punkt" },
	{ SCR_SUMMON_BOSS, "prizvat' bossa" },
	{ SCR_WOUNDS, "rany" },
	{ SCR_BULLSHIT, "bred sivoy kobyly" },
	{ SCR_ITEM_GENOCIDE, "punkt genotsid" },
	{ SCR_NASTINESS, "zlobnost'" },
	{ SCR_ELEMENTALISM, "elementnyye issledovaniya" },
	{ SCR_DEMONOLOGY, "issledovaniya d'yavol" },
	{ SCR_GIRLINESS, "zhenskaya" },
	{ SPE_AMNESIA, "poterya pamyati" },
	{ SPE_REPAIR_WEAPON, "remont oruzhiya" },
	{ SPE_BOOK_OF_THE_DEAD, "kniga mertvykh" },
	{ WAN_CONFUSION, "putanitsa" },
	{ WAN_SLIMING, "shlamoobrazovaniye" },
	{ WAN_LYCANTHROPY, "likantropiyu" },
	{ WAN_SUMMON_SEXY_GIRL, "vyzvat' seksual'nyye devushki" },
	{ GOLD_PIECE, "zolotaya moneta" },
	{ WONDER_STONE, "udivitel'no, kamen'" },
	{ LUCKSTONE, "udachi kamen'" },
	{ HEALTHSTONE, "zdorov'ye kamen'" },
	{ MANASTONE, "many kamen'" },
	{ STONE_OF_MAGIC_RESISTANCE, "kamen' soprotivleniye magii" },
	{ SLEEPSTONE, "kamen' sna" },
	{ TOUCHSTONE, "probnyy kamen'" },
	{ FARLOOK_STONE, "daleko vzglyad kamen'" },
	{ RESPAWN_STONE, "re-ikru kamnya" },
	{ CAPTCHA_STONE, "test t'yuringa kamen'" },
	{ AMNESIA_STONE, "protiv protrite kamen'" },
	{ BIGSCRIPT_STONE, "bol'shoy kamen' stsenariy" },
	{ BANK_STONE, "bankovskiy kamen'" },
	{ MAP_STONE, "karta kamen'" },
	{ TECHNIQUE_STONE, "tekhnika kamen'" },
	{ DISENCHANTMENT_STONE, "razocharovaniye kamen'" },
	{ VERISIERT_STONE, "chayut kamen'" },
	{ CHAOS_TERRAIN_STONE, "khaos mestnosti kamen'" },
	{ MUTENESS_STONE, "nemota kamen'" },
	{ ENGRAVING_STONE, "gravirovka kamnya" },
	{ MAGIC_DEVICE_STONE, "volshebnyy kamen' ustroystvo" },
	{ BOOK_STONE, "kniga kamen'" },
	{ LEVEL_STONE, "uroven' kamen'" },
	{ QUIZ_STONE, "test kamen'" },
	{ METABOLIC_STONE, "metabolicheskiy kamen'" },
	{ STONE_OF_NO_RETURN, "kamen' otkuda net vozvrata" },
	{ EGOSTONE, "egoizm kamen'" },
	{ FAST_FORWARD_STONE, "bystraya peremotka vpered kamen'" },
	{ ROTTEN_STONE, "trepela" },
	{ UNSKILLED_STONE, "nekvalifitsirovannyy kamen'" },
	{ LOW_STAT_STONE, "nizkiy statistika kamen'" },
	{ TRAINING_STONE, "obucheniye kamen'" },
	{ EXERCISE_STONE, "uprazhneniya kamen'" },
	{ HEAVY_STONE_BALL, "tyazhelyy kamen' myach" },
	{ HEAVY_GLASS_BALL, "tyazhelyy steklyannyy shar" },
	{ QUITE_HEAVY_IRON_BALL, "dovol'no tyazhelyy shar zheleza" },
	{ HEAVY_GOLD_BALL, "tyazhelaya zolotaya myach" },
	{ HEAVY_CLAY_BALL, "tyazhelyy shar gliny" },
	{ REALLY_HEAVY_IRON_BALL, "deystvitel'no tyazhelyy shar zheleza" },
	{ HEAVY_ELYSIUM_BALL, "tyazhelaya vnezemnoy myach" },
	{ HEAVY_GRANITE_BALL, "tyazhelyy granit myach" },
	{ EXTREMELY_HEAVY_IRON_BALL, "chrezvychayno tyazhelyy shar zheleza" },
	{ HEAVY_CONUNDRUM_BALL, "tyazhelaya zagadka myach" },
	{ HEAVY_CONCRETE_BALL, "tyazhelyy beton myach" },
	{ IMPOSSIBLY_HEAVY_IRON_BALL, "nevozmozhno tyazhelyy shar zheleza" },
	{ IMPOSSIBLY_HEAVY_GLASS_BALL, "nevozmozhno tyazhelyy steklyannyy shar" },
	{ IMPOSSIBLY_HEAVY_MINERAL_BALL, "nevozmozhno tyazhelyy mineral myach" },
	{ IMPOSSIBLY_HEAVY_ELYSIUM_BALL, "nevozmozhno tyazhelyy vnezemnoy myach" },
	{ STONE_CHAIN, "kamen' tsepi" },
	{ GLASS_CHAIN, "steklo tsepi" },
	{ GOLD_CHAIN, "zolotaya tsepochka" },
	{ CLAY_CHAIN, "glina tsepi" },
	{ SCOURGE, "bich" },
	{ ELYSIUM_SCOURGE, "vnezemnaya bich" },
	{ GRANITE_SCOURGE, "granit bich" },
	{ NUNCHIAKU, "nindzya tsepi" },
	{ CONUNDRUM_NUNCHIAKU, "golovolomka nindzya tsepi" },
	{ CONCRETE_NUNCHIAKU, "beton nindzya tsepi" },
	{ HOSTAGE_CHAIN, "zalozhnikom tsepi" },
	{ GLASS_HOSTAGE_CHAIN, "steklo zalozhnikom tsepi" },
	{ MINERAL_HOSTAGE_CHAIN, "mineral'naya tsep' zalozhnikom" },
	{ ELYSIUM_HOSTAGE_CHAIN, "vnezemnaya zalozhnikom tsepi" },
	{ BLINDING_VENOM, "osleplyaya yad" },

	{ GOLDEN_ARROW, "zolotaya strela" },
	{ ANCIENT_ARROW, "drevnyaya strela" },
	{ SPIKE, "ship" },
	{ SICKLE, "serp" },
	{ ELVEN_SICKLE, "el'fiyskiy serp" },
	{ SCYTHE, "sad kosa" },
	{ MOON_AXE, "luna topor" },
	{ VIBROBLADE, "vibratsii lezviya" },
	{ CRYSTAL_SWORD, "khrustal'nyy mech" },
	{ ELVEN_LANCE, "el'fiyskiy ostroga" },
	{ ELVEN_MACE, "el'fiyskiy bulava" },
	{ SILVER_KHAKKHARA, "serebryanyy delitsya na chetyrekh sotrudnikov" },
	{ IRON_BAR, "zheleznyy slitok" },
	{ FLINTLOCK, "kremnevoye ruzh'ye" },
	{ BFG, "bol'shoy grebanyy pistolet" },
	{ HAND_BLASTER, "ruka energiya pistolet" },
	{ ARM_BLASTER, "energiya ruka pistolet" },
	{ CUTTING_LASER, "lazernoy rezki" },
	{ RAYGUN, "luchevaya pushka" },
	{ BLASTER_BOLT, "blastera" },
	{ HEAVY_BLASTER_BOLT, "boyepripasov tyazhelyye energii" },
	{ LASER_BEAM, "lazernyy luch" },
	{ BFG_AMMO, "bol'shoy grebanyy pistolet boyepripasy" },
	{ SEDGE_HAT, "osoka shlyape" },
	{ SKULLCAP, "tyubeteyka" },
	{ FLACK_HELMET, "zenitnaya artilleriya shlem" },
	{ CRYSTAL_HELM, "kristall rulya" },
	{ ELVEN_TOGA, "el'fiyskiy khalat" },
	{ NOBLE_S_DRESS, "blagorodnyy plat'ye" },
	{ CONSORT_S_SUIT, "supruga kostyum" },
	{ FORCE_ARMOR, "sila bronya" },
	{ HEALER_UNIFORM, "tselitel' ravnomernoye" },
	{ JUMPSUIT, "kombinezon" },
	{ BLACK_DRESS, "chernoye plat'ye" },
	{ BODYGLOVE, "perchatki tela" },
	{ ALCHEMY_SMOCK, "alkhimiya khalat" },
	{ LEO_NEMAEUS_HIDE, "moguchiy lev pal'to" },
	{ CRYSTAL_SHIELD, "kristall shchit" },
	{ ORIHALCYON_GAUNTLETS, "redkiye mineral'nyye perchatki" },
	{ CRYSTAL_BOOTS, "khrustal'nyye sapogi" },
	{ RIN_ALACRITY, "gotovnost'" },
	{ AMULET_VERSUS_CURSES, "amulet of predotvrashcheniye proklyatiye" },
	{ SCR_WARD, "podopechnyy" },
	{ SCR_WARDING, "obereg" },
	{ MAGICITE_CRYSTAL, "okonchatel'nyy kristallicheskiy fantaziya" },
	{ SMALL_PIECE_OF_UNREFINED_MITHR, "malen'kiy kusochek nerafinirovannogo mifrilom" },
	{ SILVER_SLINGSTONE, "serebro brosaya kamen'" },

	{ KOKKEN, "vostochnoy arbalet boyepripasy" },
	{ NEEDLE, "igla" },
	{ CALTROP, "provolochnyye yezhi" },
	{ BRONZE_SPEAR, "bronza kop'ye" },
	{ FLINT_SPEAR, "kremen' kop'ye" },
	{ LONG_STAKE, "dolgo aktsiy" },
	{ BAMBOO_SPEAR, "bambuk kop'ye" },
	{ TWO_HANDED_TRIDENT, "dvuruchnyy trezubets" },
	{ MERCURIAL_ATHAME, "rtutnyy ritual kinzhal" },
	{ SURVIVAL_KNIFE, "nozh vyzhivaniya" },
	{ OBSIDIAN_AXE, "vulkanicheskoye steklo topor" },
	{ DWARVISH_BATTLE_AXE, "gnomov boyevoy topor" },
	{ IRON_SABER, "zheleza sablya" },
	{ BLACK_AESTIVALIS, "chernyy yaponskiy palash" },
	{ WHITE_FLOWER_SWORD, "belyy tsvetok mech" },
	{ SUGUHANOKEN, "stranno dlinnyy mech" },
	{ GREAT_HOUCHOU, "bol'shoy neverno nazvany dlinnyy mech" },
	{ CHAINSWORD, "tsep' mech" },
	{ BASTERD_SWORD, "polutornyy mech" },
	{ PITCHFORK, "vily" },
	{ BLACK_HALBERD, "chernyy alebardoy" },
	{ BRONZE_MORNING_STAR, "bronza utrennyaya zvezda" },
	{ SPINED_BALL, "kolyuchkami shar" },
	{ SLEDGE_HAMMER, "kuvalda" },
	{ BONE_CLUB, "kosti klub" },
	{ SPIKED_CLUB, "shipami klub" },
	{ HUGE_CLUB, "ogromnyy klub" },
	{ LOG, "zhurnal" },
	{ RUNED_ROD, "runicheskiy zhezl" },
	{ STAR_ROD, "zvezdochnyy sterzhen'" },
	{ FIRE_HOOK, "ogon' kryuk" },
	{ PLATINUM_FIRE_HOOK, "platina ogon' kryuk" },
	{ CHAIN_AND_SICKLE, "tsep' i serp" },
	{ TWO_HANDED_FLAIL, "dvuruchnyy tsep" },
	{ CHAINWHIP, "tsep' knutom" },
	{ MITHRIL_WHIP, "mifrilovyy knut" },
	{ FLAME_WHIP, "plamya knut" },
	{ ROSE_WHIP, "roza knut" },
	{ BROOM, "metla" },
	{ MOP, "shvabra" },
	{ SPECIAL_MOP, "spetsial'naya shvabra" },
	{ BOAT_OAR, "lodka veslo" },
	{ MAGICAL_PAINTBRUSH, "volshebnyy kist'" },
	{ FUTON_SWATTER, "matras mukhoboyka" },
	{ CARDBOARD_FAN, "karton ventilyator" },
	{ OTAMA, "polnost'yu neuznavayemym veslo" },
	{ PARASOL, "zontik" },
	{ UMBRELLA, "dozhd' personal" },
	{ PILE_BUNKER, "kucha shuter" },
	{ NURSE_CAP, "medsestra shapka" },
	{ KATYUSHA, "shlem, kotoryy nazvan v chest' devushki" },
	{ BUNNY_EAR, "krolik ukha" },
	{ DRAGON_HORNED_HEADPIECE, "drakon-rogatyy golovnoy ubor" },
	{ STRAW_HAT, "solomennaya shlyapa" },
	{ SPEEDWAGON_S_HAT, "skorost' universal shlyapa" },
	{ MECHA_IRAZU, "nepriznannaya shlem" },
	{ SCHOOL_CAP, "shkola kryshka" },
	{ CROWN, "korona" },
	{ ANTENNA, "poluchatel'" },
	{ CHAIN_COIF, "tsep' chepets" },
	{ COLOR_CONE, "tsvet konusa" },
	{ MINING_HELM, "dobycha rulya" },
	{ FIELD_HELM, "pole rulya" },
	{ HELM_OF_SAFEGUARD, "shlem garantii" },
	{ HELM_OF_CHAOTIC, "shlem khaotichno" },
	{ HELM_OF_NEUTRAL, "shlem neytral'nogo" },
	{ HELM_OF_LAWFUL, "shlem zakonnym" },
	{ HELM_OF_UNDERWATER_ACTION, "shlem podvodnogo deystviya" },
	{ HELM_OF_JAMMING, "shlem pomekh" },
	{ RAINBOW_DRAGON_SCALE_MAIL, "shkala drakon radugi pochty" },
	{ BLOOD_DRAGON_SCALE_MAIL, "shkala drakon krovi pochty" },
	{ PLAIN_DRAGON_SCALE_MAIL, "ravniny drakon masshtab pochty" },
	{ SKY_DRAGON_SCALE_MAIL, "shkala drakon nebo pochty" },
	{ WATER_DRAGON_SCALE_MAIL, "shkala drakon vody pochty" },
	{ RAINBOW_DRAGON_SCALES, "raduga drakon vesy" },
	{ BLOOD_DRAGON_SCALES, "drakon krovi vesy" },
	{ PLAIN_DRAGON_SCALES, "prostyye vesy drakon" },
	{ SKY_DRAGON_SCALES, "nebo drakon vesy" },
	{ WATER_DRAGON_SCALES, "voda drakon vesy" },
	{ OLIHARCON_SPLINT_MAIL, "redkiy mineral shinu pochty" },
	{ BAMBOO_MAIL, "bambuk pochty" },
	{ SAILOR_BLOUSE, "moryak bluzka" },
	{ SAFEGUARD_SUIT, "garantiya kostyum" },
	{ FEATHER_ARMOR, "pero broni" },
	{ SCHOOL_UNIFORM, "shkol'naya forma" },
	{ BUNNY_UNIFORM, "zaychik ravnomernoye" },
	{ MAID_DRESS, "plat'ye gornichnoy" },
	{ NURSE_UNIFORM, "medsestra ravnomernoye" },
	{ COMMANDER_SUIT, "komandir kostyum" },
	{ CAMOUFLAGED_CLOTHES, "zamaskirovannyye odezhdy" },
	{ SPECIAL_CAMOUFLAGED_CLOTHES, "spetsial'nyy zamaskirovannyy odezhda" },
	{ SHOULDER_RINGS, "plechevyye kol'tsa" },
	{ PRINTED_SHIRT, "pechatayetsya rubashki" },
	{ BATH_TOWEL, "bannoye polotentse" },
	{ PLUGSUIT, "plagin kostyum" },
	{ MEN_S_UNDERWEAR, "muzhskoye nizhneye bel'ye" },
	{ AYANAMI_WRAPPING, "vymyshlennyy personazh upakovka" },
	{ RUBBER_APRON, "rezinovyy fartuk" },
	{ KITCHEN_APRON, "kukhonnyye fartuki" },
	{ FRILLED_APRON, "gofrirovannyy fartuk" },
	{ SUPER_MANTLE, "ochen' moshchnyye mantii" },
	{ WINGS_OF_ANGEL, "kryl'ya angela" },
	{ DUMMY_WINGS, "fiktivnyye kryl'ya" },
	{ FUR, "mekh" },
	{ HIDE, "skryvat'" },
	{ DISPLACER_BEAST_HIDE, "buyka zver' spryatat'" },
	{ THE_NEMEAN_LION_HIDE, "nemeyskogo lev spryatat'" },
	{ CLOAK_OF_SPRAY, "spina sprey" },
	{ CLOAK_OF_FLAME, "spina plameni" },
	{ CLOAK_OF_INSULATION, "plashch izolyatsii" },
	{ CLOAK_OF_MATADOR, "plashch matadora" },
	{ ORCISH_GUARD_SHIELD, "orkov okhrannik shchit" },
	{ SHIELD, "shchit" },
	{ SILVER_SHIELD, "serebryanyy shchit" },
	{ MIRROR_SHIELD, "zerkalo shchit" },
	{ RAPIRAPI, "yaponskiy komiks shchit kharakter" },
	{ RAINBOW_DRAGON_SCALE_SHIELD, "raduga masshtab drakon shchit" },
	{ BLOOD_DRAGON_SCALE_SHIELD, "krov' drakona shchit masshtab" },
	{ PLAIN_DRAGON_SCALE_SHIELD, "ravniny drakon masshtab shchit" },
	{ SKY_DRAGON_SCALE_SHIELD, "nebo masshtab drakon shchit" },
	{ WATER_DRAGON_SCALE_SHIELD, "voda drakon masshtab shchit" },
	{ GAUNTLETS_OF_SAFEGUARD, "perchatki garantii" },
	{ GAUNTLETS_OF_PLUGSUIT, "perchatki plagina kostyum" },
	{ COMMANDER_GLOVES, "komandir perchatki" },
	{ FIELD_GLOVES, "polevyye perchatki" },
	{ GAUNTLETS, "rukavitsy" },
	{ SILVER_GAUNTLETS, "serebryanyye perchatki" },
	{ GAUNTLETS_OF_FAST_CASTING, "rukavitsy bystroy lit'ya" },
	{ GAUNTLETS_OF_NO_FLICTION, "Rukavitsy ne treniyem" },
	{ ATSUZOKO_BOOTS, "deystvitel'no vysokiye sapogi" },
	{ RUBBER_BOOTS, "rezinovyye sapogi" },
	{ LEATHER_SHOES, "kozhanyye botinki" },
	{ SNEAKERS, "krossovki" },
	{ MULTI_SHOES, "mul'ti obuv'" },
	{ BOOTS_OF_PLUGSUIT, "sapogi plagina kostyum" },
	{ ROLLER_BLADE, "rolikakh" },
	{ FIELD_BOOTS, "polevyye sapogi" },
	{ BOOTS_OF_SAFEGUARD, "sapogi garantii" },
	{ STOMPING_BOOTS, "topat' sapogi" },
	{ CARRYING_BOOTS, "provedeniya sapogi" },
	{ FREEZING_BOOTS, "zamorazhivaniye sapogi" },
	{ RIN_PRACTICE, "praktika" },
	{ RIN_CONFUSION_RESISTANCE, "soprotivleniye putanitsa" },
	{ RIN_RESTRATION, "oglushayet soprotivleniye" },
	{ RIN_HEAVY_ATTACK, "massirovannyy udar" },
	{ AMULET_OF_PRISM, "amulet of prizma" },
	{ AMULET_OF_WARP_DIMENSION, "amulet of razmer osnovy" },
	{ AMULET_OF_D_TYPE_EQUIPMENT, "amulet of d-tipa oborudovaniya" },
	{ AMULET_VERSUS_DEATH_SPELL, "amulet of predotvrashcheniya smerti zaklinaniye" },
	{ AMULET_OF_QUICK_ATTACK, "amulet of bystraya ataka" },
	{ AMULET_OF_QUADRUPLE_ATTACK, "amulet of chetyrekhmestnyy ataka" },
	{ PENDANT, "kulon" },
	{ NECKLACE, "ozherel'ye" },
	{ JAPAN_WAX_CANDLE, "yaponiya voskovaya svecha" },
	{ OIL_CANDLE, "maslyanaya svecha" },
	{ TEMPEST_HORN, "burya rog" },
	{ SHEAF_OF_STRAW, "puchok solomy" },
	{ COTTON, "khlopok" },
	{ ONION, "s''yedobnyye luk" },
	{ WELSH_ONION, "batuna" },
	{ WATERMELON, "arbuz" },
	{ WHITE_PEACH, "belyy persik" },
	{ SENTOU, "nekotoryye sluchaynyye aziatskoy kukhni" },
	{ BEAN, "fasol'" },
	{ SENZU, "misticheskoye fasoli" },
	{ PARFAIT, "parfe" },
	{ X_MAS_CAKE, "rozhdestvo tort" },
	{ BUNNY_CAKE, "krolik tort" },
	{ BAKED_SWEET_POTATO, "zapechennyye sladkiy kartofel'" },
	{ BREAD, "khleb" },
	{ PASTA, "makaronnyye izdeliya" },
	{ CHARRED_BREAD, "obuglennyye khleb" },
	{ SLICE_OF_PIZZA, "kusok pitstsy" },
	{ WHITE_SWEETS, "belyye konfety" },
	{ BROWN_SWEETS, "korichnevyye konfety" },
	{ GREEN_SWEETS, "zelenyye konfety" },
	{ PINK_SWEETS, "rozovyye sladosti" },
	{ BLACK_SWEETS, "chernyye konfety" },
	{ YELLOW_SWEETS, "zheltyye konfety" },
	{ BOTA_MOCHI, "ris fasol' tort" },
	{ KIBI_DANGO, "bylaya yaponskaya yeda" },
	{ SAKURA_MOCHI, "yaponskaya vesna sladosti" },
	{ KOUHAKU_MANJYUU, "konfety ili drugiye produkty pitaniya" },
	{ YOUKAN, "sakhar sladosti" },
	{ CHOCOLATE, "shokolad" },
	{ CHOCOEGG, "shokoladnoye yaytso" },
	{ WAKAME, "buryye vodorosli" },
	{ MAGIC_BANANA, "magiya banan" },
	{ LUNCH_OF_BOILED_EGG, "lanch varenym yaytsom" },
	{ PIZZA, "pitstsa" },
	{ POT_PORTER, "port'ye" },
	{ POT_WONDER, "zadavat'sya voprosom" },
	{ SCR_WONDER, "zadavat'sya voprosom" },
	{ POT_TERCES_DLU, "polnaya sluchaynost'" },
	{ POT_HIDING, "pryachetsya" },
	{ POT_DECOY_MAKING, "primanka resheniy" },
	{ POT_DOWN_LEVEL, "vniz uroven'" },
	{ POT_KEEN_MEMORY, "stremitsya pamyati" },
	{ POT_CURE_WOUNDS, "izlecheniya ran" },
	{ POT_CURE_SERIOUS_WOUNDS, "lecheniye ser'yeznykh ran" },
	{ POT_NIGHT_VISION, "nochnoye videniye" },
	{ POT_CURE_CRITICAL_WOUNDS, "vylechit' rany kriticheskiye" },
	{ POT_RESISTANCE, "soprotivleniye" },
	{ POT_POISON, "yad" },
	{ POT_COFFEE, "kofe" },
	{ POT_RED_TEA, "krasnyy chay" },
	{ POT_OOLONG_TEA, "ulun" },
	{ POT_GREEN_TEA, "zelenyy chay" },
	{ POT_COCOA, "kakao" },
	{ POT_GREEN_MATE, "zelenyy pomoshchnik" },
	{ POT_TERERE, "traditsionnyy napitok" },
	{ POT_AOJIRU, "rastitel'nogo napitok" },
	{ POT_WINE, "vino" },
	{ POT_ULTIMATE_TSUYOSHI_SPECIAL, "konechnaya glupost' spetsial'nyy" },
	{ POT_MEHOHO_BURUSASAN_G, "seksual'naya kommandos napitok" },
	{ SCR_FLOOD_TIDE, "priliv" },
	{ SCR_EBB_TIDE, "otliv" },
	{ SCR_COPYING, "kopirovaniye" },
	{ SCR_CREATE_VICTIM, "sozdat' zhertvu" },
	{ SCR_CREATE_FACILITY, "sozdat' tsentr" },
	{ SCR_ERASURE, "stiraniye" },
	{ SCR_CURE_BLINDNESS, "lecheniye slepoty" },
	{ SCR_POWER_HEALING, "tselitel'naya sila" },
	{ SCR_SKILL_UP, "umeniye do" },
	{ SCR_FROST, "moroz" },
	{ SCR_CREATE_ALTAR, "sozdat' sayt zhertvuya" },
	{ SCR_CREATE_TRAP, "sozdat' lovushku" },
	{ SCR_CREATE_SINK, "sozdat' rakovinu" },
	{ SCR_SYMMETRY, "simmetriya" },
	{ SCR_CREATE_CREATE_SCROLL, "sozdavat' svitok" },
	{ SCR_DESTROY_WEAPON, "unichtozhit' oruzhiye" },
	{ SPE_KNOW_ENCHANTMENT, "znayete chary" },
	{ SPE_MAGICTORCH, "magiya fakel" },
	{ SPE_DISPLACEMENT, "smeshcheniye" },
	{ SPE_MASS_HEALING, "massa istseleniye" },
	{ SPE_TIME_SHIFT, "sdvig vremeni" },
	{ WAN_MISFIRE, "osechka" },
	{ WAN_VENOM_SCATTERING, "yad rasseyaniya" },
	{ WAN_SHARE_PAIN, "dolya bol'" },
	{ WAN_POISON, "yad" },
	{ MORION, "shlem kamen'" },
	{ TALC, "zhidkost' tal'k" },
	{ GRAPHITE, "grafit" },
	{ VOLCANIC_GLASS_FRAGMENT, "vulkanicheskoye steklo oskolok" },
	{ STARLIGHTSTONE, "zvezda svet kamen'" },
	{ SCR_ALTER_REALITY, "izmenit' real'nost'" },
	{ SPE_ALTER_REALITY, "izmenit' real'nost'" },
	{ FULL_PLATE_MAIL, "polnyy laty" },

	{ WAN_DISINTEGRATION_BEAM, "raspad puchka" },
	{ SPE_DISINTEGRATION_BEAM, "raspad puchka" },
	{ SCR_GROUP_SUMMONING, "gruppa vyzov" },
	{ SPE_FLYING, "letayushchiy" },
	{ WAN_CHROMATIC_BEAM, "khromaticheskoy sveta" },
	{ SPE_CHROMATIC_BEAM, "khromaticheskoy sveta" },
	{ SPE_FUMBLING, "nelovkiy" },
	{ SPE_MAKE_VISIBLE, "sdelat' vidimym" },
	{ SPE_WARPING, "iskrivleniye" },
	{ SPE_TRAP_CREATION, "sozdayut lovushki" },
	{ WAN_STUN_MONSTER, "oglushayet monstr" },
	{ SPE_STUN_MONSTER, "oglushayet monstr" },
	{ SPE_CURSE_ITEMS, "proklyatiye predmety" },
	{ SPE_CHARACTER_RECURSION, "kharakter rekursii" },
	{ SPE_CLONE_MONSTER, "umnozhit' monstr" },
	{ SCR_WORLD_FALL, "mir osen'yu" },
	{ SCR_RESURRECTION, "dopolnitel'nuyu zhizn'" },
	{ SCR_SUMMON_GHOST, "proklyatyye dukh" },
	{ SCR_MEGALOAD, "tyazhelyy gruz" },
	{ SPE_DESTROY_ARMOR, "unichtozhit' dospekhi" },
	{ SCR_ENRAGE, "besit'" },
	{ WAN_TIDAL_WAVE, "prilivnaya volna" },
	{ SCR_ANTIMATTER, "antiveshchestvo" },
	{ SCR_SUMMON_ELM, "vyzvat' vyaz" },
	{ WAN_SUMMON_ELM, "vyzvat' vyaz" },
	{ SCR_RELOCATION, "pereseleniye" },
	{ WAN_DRAIN_MANA, "slit' magicheskuyu energiyu" },
	{ WAN_FINGER_BENDING, "palets izgib" },
	{ SCR_IMMOBILITY, "nepodvizhnost'" },
	{ WAN_IMMOBILITY, "nepodvizhnost'" },
	{ SCR_FLOODING, "ryba navodneniya" },
	{ SCR_EGOISM, "sebyalyubiye" },
	{ WAN_EGOISM, "sebyalyubiye" },
	{ SCR_RUMOR, "slukh" },
	{ SCR_MESSAGE, "soobshcheniye" },
	{ SCR_SIN, "grekh" },
	{ WAN_SIN, "grekh" },
	{ WAN_INERTIA, "inertsiya" },
	{ SPE_INERTIA, "inertsiya" },
	{ WAN_TIME, "vremya" },
	{ SPE_TIME, "vremya" },
	{ AMULET_OF_SPEED, "amulet of skorost'" },
	{ AMULET_OF_TELEPORT_CONTROL, "amulet of siyayushchiy kontrol'" },
	{ AMULET_OF_POLYMORPH_CONTROL, "amulet of prevrashchat' kontrol'" },
	{ HELM_OF_SPEED, "skorost' shlem" },
	{ HELMET_OF_UNDEAD_WARNING, "nezhit' preduprezhdeniye shlem" },

	{ TURN_LIMIT_STONE, "vklyuchit' predel'noye kamen'" },
	{ LIMITATION_BOOTS, "ogranicheniye sapogi" },
	{ WEAK_SIGHT_STONE, "slabym zreniyem kamen'" },
	{ DIMMER_HELMET, "nizkiy shlem zreniye" },
	{ CHATTER_STONE, "boltovnya kamen'" },
	{ CHATBOX_CLOAK, "chata plashch" },

	{ SILVER_CHAKRAM, "serebro shakram" },
	{ HIGH_ELVEN_WARSWORD, "vysshikh el'fov voyna mech" },
	{ WAR_HAT, "voyna shlem" },
	{ ELVEN_HELM, "el'fiyskiy shlem" },
	{ HIGH_ELVEN_HELM, "vysshikh el'fov rulya" },
	{ GENTLEMAN_S_SUIT, "dzhentl'men kostyum" },
	{ GENTLEWOMAN_S_DRESS, "dama plat'ye" },
	{ STRAITJACKET, "smiritel'naya rubashka" },
	{ CURING_UNIFORM, "otverzhdeniya ravnomernoye" },
	{ HAWAIIAN_SHORTS, "gavayskiye shorty" },

	{ CONDOME, "prezervativ" },
	{ HYDRA_BOW, "gidra luk" },
	{ MANCATCHER, "chelovek lovli kop'ye" },
	{ MARE_TRIDENT, "kobyla trezubets" },
	{ HELO_CROSSBOW, "ballisty arbaleta" },
	{ BATARANG, "bitoy bumerang" },

	{ ICE_BOX_OF_HOLDING, "ledovoy korobke kholdinga" },
	{ ICE_BOX_OF_WATERPROOFING, "korobka l'da gidroizolyatsii" },
	{ ICE_BOX_OF_DIGESTION, "korobka l'da pishchevareniya" },
	{ LARGE_BOX_OF_DIGESTION, "bol'shaya korobka pishchevareniya" },
	{ CHEST_OF_HOLDING, "grud' kholdinga" },

	{ SCR_ANTIMAGIC, "antimagicheskoye" },
	{ SCR_RESISTANCE, "soprotivleniye" },

	{ GENERAL_CANDLE, "obshchaya svecha" },
	{ NATURAL_CANDLE, "yestestvennaya svecha" },

	{ RIN_DOOM, "gibel'" },
	{ RIN_ELEMENTS, "elementy" },
	{ RIN_LIGHT, "legkiy" },
	{ WAN_LEVITATION, "levitatsiya" },
	{ PSYCHIC_DRAGON_SCALE_MAIL, "psikhicheskaya cheshuya drakona pochta" },
	{ PSYCHIC_DRAGON_SCALE_SHIELD, "psikhicheskaya cheshuya drakona shchit" },
	{ PSYCHIC_DRAGON_SCALES, "Psikhicheskiye cheshuyu drakona" },
	{ WAN_PSYBEAM, "psikho luch" },
	{ SPE_PSYBEAM, "psikho luch" },
	{ HELM_OF_TELEPORTATION, "shlem teleportatsii" },
	{ SWIMSUIT, "kupal'nik" },
	{ TROLL_LEATHER_ARMOR, "troll' kozhanyy dospekh" },
	{ TROLL_HIDE, "troll' skryt'" },
	{ GAUNTLETS_OF_FREE_ACTION, "perchatki svobodnogo deystviya" },
	{ GAUNTLETS_OF_GOOD_FORTUNE, "perchatki udachi" },
	{ BOOTS_OF_FREEDOM, "sapogi svobody" },
	{ AMULET_OF_INSOMNIA, "amulet of bessonnitsa" },
	{ RIN_MAGIC_RESISTANCE, "soprotivleniye magii" },
	{ RIN_MATERIAL_STABILITY, "material'naya stabil'nost'" },
	{ RIN_MIND_SHIELDING, "um ekranirovaniye" },
	{ AMULET_OF_MENTAL_STABILITY, "amulet of psikhicheskaya ustoychivost'" },
	{ WAN_DEBUGGING, "otladka" },
	{ WAN_HYPER_BEAM, "giper luch" },
	{ SPE_HYPER_BEAM, "giper luch" },
	{ HELM_OF_TELEPORT_CONTROL, "kormilo upravleniya teleporta" },

	{ HERETIC_CLOAK, "yeretik plashch" },
	{ HELM_OF_STARVATION, "pul't upravleniya ot goloda" },
	{ RIN_ANTI_DROP, "anti padeniye" },
	{ AMULET_OF_WEAK_MAGIC, "amulet of slabaya magiya" },
	{ RIN_ENSNARING, "zalovit'" },
	{ EERIE_CLOAK, "zhutkiy plashch" },
	{ QUAFFER_HELMET, "shlem zaglatyvat'" },
	{ PET_STOMPING_PLATFORM_BOOTS, "lyubimchik-topat' sapogi na platforme" },
	{ AMULET_OF_DIRECTIONAL_SWAP, "amulet of napravlennyy svop" },
	{ INCORRECTLY_ADJUSTED_HELMET, "nepravil'no otregulirovan shlem" },
	{ AMULET_OF_SUDDEN_CURSE, "amulet of vnezapnoye proklyatiye" },
	{ CLOAK_OF_NAKEDNESS, "plashch nagote" },
	{ AMULET_OF_ANTI_EXPERIENCE, "amulet of tetushka opyt" },
	{ GAUNTLETS_OF_STEALING, "rukavitsy krazhi" },
	{ ASS_KICKER_BOOTS, "zadnitsu kiker sapogi" },
	{ RIN_DIARRHEA, "ponos" },
	{ BOOTS_OF_TOTAL_STABILITY, "sapogi obshchey stabil'nosti" },
	{ HELM_OF_TRUE_SIGHT, "shlem istinnogo vzglyada" },
	{ ELVEN_GAUNTLETS, "el'fiyskiye rukovitsakh" },
	{ CLOAK_OF_AGGRAVATION, "plashch obostreniya" },
	{ CLOAK_OF_CONFLICT, "spina konflikta" },
	{ HELM_OF_WARNING, "kormilo preduprezhdeniya" },
	{ BOOTS_OF_DISPLACEMENT, "sapogi peremeshcheniya" },
	{ CLOAK_OF_MAGICAL_BREATHING, "plashch magicheskogo dykhaniya" },
	{ HELM_OF_DETOXIFICATION, "pul't upravleniya dezintoksikatsii" },
	{ BOOTS_OF_SWIMMING, "sapogi plavaniya" },
	{ CLOAK_OF_STAT_LOCK, "plashch zamka statusa" },
	{ WING_CLOAK, "krylo plashch" },
	{ HELM_OF_NO_DIGESTION, "Shlem ne pishchevareniya" },
	{ CLOAK_OF_PREMATURE_DEATH, "plashch prezhdevremennoy smerti" },
	{ ANTI_CURSE_BOOTS, "anti proklyatiye sapogi" },
	{ TINFOIL_HELMET, "fol'ga shlem" },
	{ ANTIDEATH_CLOAK, "anti plashch smerti" },
	{ PARANOIA_HELMET, "shlem paranoyya" },
	{ GAUNTLETS_OF_MISFIRING, "rukavitsy osechki" },
	{ DEMENTIA_BOOTS, "slaboumiye sapogi" },
	{ NONSACRED_STONE, "ne svyashchennyy kamen'" },
	{ STARVATION_STONE, "golodaniye kamen'" },
	{ DROPLESS_STONE, "padeniye men'she kamen'" },
	{ LOW_EFFECT_STONE, "nizkiy kamen' effekt" },
	{ INVISO_STONE, "nevidimyy kamen'" },
	{ GHOSTLY_STONE, "prizrachnoye kamen'" },
	{ DEHYDRATING_STONE, "obezvozhivaya kamen'" },
	{ STONE_OF_HATE, "kamen' nenavisti" },
	{ DIRECTIONAL_SWAP_STONE, "napravleniye svop kamen'" },
	{ NONINTRINSICAL_STONE, "ne svoystvennyy kamen'" },
	{ DROPCURSE_STONE, "padeniye proklyatiye kamen'" },
	{ STONE_OF_NAKED_STRIPPING, "kamen' golyy obnazhat'" },
	{ ANTILEVEL_STONE, "anti kamen' uroven'" },
	{ STEALER_STONE, "pokhititel' kamen'" },
	{ REBEL_STONE, "buntar' kamen'" },
	{ SHIT_STONE, "der'mo kamen'" },
	{ STONE_OF_MISFIRING, "kamen' osechki" },
	{ STONE_OF_PERMANENCE, "kamen' postoyanstvu" },
	{ BOOGEYMAN_HELMET, "bugimen shlem" },
	{ SPE_FIRE_BOLT, "ogon' snaryadom" },

	{ PLATINUM_DRAGON_SCALE_SHIELD, "platiny cheshuya drakona shchit" },
	{ BRASS_DRAGON_SCALE_SHIELD, "latun' cheshuya drakona shchit" },
	{ COPPER_DRAGON_SCALE_SHIELD, "mednyy shchit cheshuya drakona" },
	{ EMERALD_DRAGON_SCALE_SHIELD, "izumrudnaya cheshuya drakona shchit" },
	{ RUBY_DRAGON_SCALE_SHIELD, "rubin cheshuya drakona shchit" },
	{ SAPPHIRE_DRAGON_SCALE_SHIELD, "sapfirovoye cheshuya drakona shchit" },
	{ DIAMOND_DRAGON_SCALE_SHIELD, "almaznyy cheshuya drakona shchit" },
	{ AMETHYST_DRAGON_SCALE_SHIELD, "ametist cheshuya drakona shchit" },

	{ PLATINUM_DRAGON_SCALES, "platiny drakona vesy" },
	{ BRASS_DRAGON_SCALES, "latuni drakona vesy" },
	{ COPPER_DRAGON_SCALES, "med' drakona vesy" },
	{ EMERALD_DRAGON_SCALES, "izumrud drakona vesy" },
	{ RUBY_DRAGON_SCALES, "rubinovyy drakon vesy" },
	{ SAPPHIRE_DRAGON_SCALES, "sapfirovyy drakon vesy" },
	{ DIAMOND_DRAGON_SCALES, "almaz drakona vesy" },
	{ AMETHYST_DRAGON_SCALES, "ametist drakona vesy" },

	{ PLATINUM_DRAGON_SCALE_MAIL, "platiny drakona masshtab pochty" },
	{ BRASS_DRAGON_SCALE_MAIL, "latun' cheshuya drakona pochta" },
	{ COPPER_DRAGON_SCALE_MAIL, "mednyy drakon masshtab pochty" },
	{ EMERALD_DRAGON_SCALE_MAIL, "izumrudnaya cheshuya drakona pochta" },
	{ RUBY_DRAGON_SCALE_MAIL, "rubinovogo drakona masshtab pochta" },
	{ SAPPHIRE_DRAGON_SCALE_MAIL, "sapfirovyy drakon masshtab pochta" },
	{ DIAMOND_DRAGON_SCALE_MAIL, "almazov drakona masshtab pochty" },
	{ AMETHYST_DRAGON_SCALE_MAIL, "ametist cheshuya drakona pochta" },

	{ RANDOMIZED_HELMET, "randomizirovannoye shlem" },
	{ HIGH_STILETTOS, "vysokiye stilety" },
	{ UNKNOWN_GAUNTLETS, "neizvestnyye rukovitsakh" },
	{ MISSING_CLOAK, "nedostayushcheye plashch" },

	{ AMULET_OF_THE_RNG, "amulet of gsch" },
	{ AMULET_OF_INFINITY, "amulet of beskonechnost'" },
	{ RIN_RANDOM_EFFECTS, "sluchaynyye effekty" },
	{ RIN_SPECIAL_EFFECTS, "spetseffekty" },

	{ PURPLE_DRAGON_SCALE_SHIELD, "fioletovyy drakon masshtaba shchit" },
	{ PURPLE_DRAGON_SCALES, "fioletovyy drakon vesy" },
	{ PURPLE_DRAGON_SCALE_MAIL, "fioletovyy drakon masshtab pochta" },

	{ DARK_MAGIC_WHISTLE, "temnaya magiya svistok" },
	{ SPE_INFERNO, "ad" },
	{ WAN_INFERNO, "ad" },
	{ SPE_ICE_BEAM, "ledyanoy luch" },
	{ WAN_ICE_BEAM, "ledyanoy luch" },
	{ SPE_THUNDER, "grom" },
	{ WAN_THUNDER, "grom" },
	{ SPE_SLUDGE, "otstoy" },
	{ WAN_SLUDGE, "otstoy" },
	{ SPE_TOXIC, "toksichnyy" },
	{ WAN_TOXIC, "toksichnyy" },
	{ SPE_NETHER_BEAM, "luch pustoty" },
	{ WAN_NETHER_BEAM, "luch pustoty" },
	{ SPE_AURORA_BEAM, "polyarnyye siyaniya lucha" },
	{ WAN_AURORA_BEAM, "polyarnyye siyaniya lucha" },
	{ SPE_GRAVITY_BEAM, "gravitatsionnyy luch" },
	{ WAN_GRAVITY_BEAM, "gravitatsionnyy luch" },
	{ SPE_CHLOROFORM, "khloroform" },
	{ WAN_CHLOROFORM, "khloroform" },
	{ SPE_DREAM_EATER, "pozhiratel' snov" },
	{ WAN_DREAM_EATER, "pozhiratel' snov" },
	{ SPE_BUBBLEBEAM, "puzyr' luch" },
	{ WAN_BUBBLEBEAM, "puzyr' luch" },
	{ SPE_GOOD_NIGHT, "dobroy nochi" },
	{ WAN_GOOD_NIGHT, "dobroy nochi" },
	{ ANTIMATTER_BULLET, "antiveshchestvo pulya" },
	{ CLOAK_OF_LEECH, "plashch piyavki" },
	{ GAUNTLETS_OF_LEECH, "rukavitsy piyavki" },
	{ AMULET_OF_LEECH, "amulet of piyavki" },
	{ RIN_LEECH, "piyavki" },
	{ KYRT_SHIRT, "kurt rubashka" },
	{ FILLER_CLOAK, "napolnitel' plashch" },
	{ DART_OF_DISINTEGRATION, "strela raspad" },
	{ SCR_ARMOR_SPECIALIZATION, "bronya spetsializatsii" },
	{ SCR_SECURE_IDENTIFY, "bezopasnaya identichnost'" },
	{ AMULET_OF_DANGER, "amulet of opasnost'" },
	{ RIN_DANGER, "opasnost'" },
	{ NASTY_CLOAK, "protivnym plashch" },
	{ UNWANTED_HELMET, "nezhelatel'nyy shlem" },
	{ EVIL_GLOVES, "zlyye perchatki" },
	{ UNFAIR_STILETTOS, "nespravedlivyye stilety" },
	{ SPECIAL_CLOAK, "spetsial'nyy plashch" },
	{ WONDER_HELMET, "shlem udivitel'no" },
	{ ARCANE_GAUNTLETS, "taynyye rukovitsakh" },
	{ SKY_HIGH_HEELS, "zaoblachnyye kabluki" },
	{ PLAIN_CLOAK, "odnotsvetnyy plashch" },
	{ POINTED_HELMET, "ostrokonechnyy shlem" },
	{ PLACEHOLDER_GLOVES, "zapolniteli perchatki" },
	{ PREHISTORIC_BOOTS, "doistoricheskiye sapogi" },
	{ ARCHAIC_CLOAK, "arkhaichnaya plashch" },
	{ BOG_STANDARD_HELMET, "bolotno-standartnyy shlem" },
	{ PROTECTIVE_GLOVES, "zashchitnyye perchatki" },
	{ SYNTHETIC_SANDALS, "sinteticheskiye sandalii" },
	{ SPE_FIXING, "fiksatsiya" },

	{ SCR_ARTIFACT_CREATION, "artefakt sozdaniye" },
	{ LETHE_CLOAK, "leta plashch" },
	{ UGG_BOOTS, "uggi" },
	{ AMULET_OF_STONE, "amulet of kamen'" },
	{ AMULET_OF_MAP_AMNESIA, "amulet of karta amneziya" },
	{ CLOAK_OF_MAP_AMNESIA, "plashch karty amnezii" },
	{ AMULET_OF_DEPRESSION, "amulet of depressiya" },
	{ CLOAK_OF_POLYMORPH, "plashch polimorfa" },
	{ CLOAK_OF_TRANSFORMATION, "spina transformatsii" },
	{ CLOAK_OF_WATER_SQUIRTING, "spina vody bryzgayet" },
	{ CLOAK_OF_PARALYSIS, "plashch paralicha" },
	{ CLOAK_OF_SICKNESS, "plashch bolezni" },
	{ CLOAK_OF_SLIMING, "plashch zashlam" },
	{ CLOAK_OF_STARVING, "plashch golodayut" },
	{ CLOAK_OF_CURSE, "plashch proklyatiya" },
	{ CLOAK_OF_DISENCHANTMENT, "plashch razocharovannosti" },
	{ CLOAK_OF_OUTRIGHT_EVILNESS, "plashch pryamogo zlobnosti" },
	{ CLOAK_OF_STONE, "plashch iz kamnya" },
	{ CLOAK_OF_LYCANTHROPY, "plashch oborotnichestvom" },
	{ CLOAK_OF_UNLIGHT, "plashch bez sveta" },
	{ CLOAK_OF_ESCALATION, "spina eskalatsii" },
	{ CLOAK_OF_MAGICAL_DRAINAGE, "plashch magicheskogo drenazha" },
	{ CLOAK_OF_ANGRINESS, "plashch serdit'sya" },
	{ CLOAK_OF_CANCELLATION, "plashch otmeny" },
	{ CLOAK_OF_TURN_LOSS, "plashch poteri oborota" },
	{ CLOAK_OF_ATTRIBUTE_LOSS, "spina poteri atributov" },
	{ CLOAK_OF_TOTTER, "plashch idti nevernoy pokhodkoy" },
	{ CLOAK_OF_DRAIN_LIFE, "spina slivnogo zhizni" },
	{ CLOAK_OF_AWAKENING, "plashch probuzhdeniya" },
	{ CLOAK_OF_STABILITY, "plashch stabil'nosti" },
	{ ANTI_DISQUIET_CLOAK, "anti plashch bespokoystvo" },
	{ HUGGING_GOWN, "plat'ye obnimat'sya" },
	{ COCLOAK, "sovmestno plashch" },
	{ CLOAK_OF_HEALTH, "plashch zdorov'ya" },
	{ CLOAK_OF_DISCOVERY, "spina otkrytiya" },
	{ BIONIC_CLOAK, "bionicheskiy plashch" },
	{ CLOAK_OF_PORTATION, "plashch tirovki" },
	{ CLOAK_OF_CONTROL, "plashch iz-pod kontrolya" },
	{ CLOAK_OF_SHIFTING, "plashch sdviga" },
	{ FLOATCLOAK, "plavayushchiy plashch" },
	{ CLOAK_OF_PRESCIENCE, "plashch predvideniya" },
	{ SENSOR_CLOAK, "plashch datchik" },
	{ CLOAK_OF_SPEED, "spina skorosti" },
	{ VAULT_CLOAK, "svod plashch" },
	{ CLOAK_OF_SPELL_RESISTANCE, "plashch soprotivleniya orfografii" },
	{ CLOAK_OF_PHYSICAL_RESISTANCE, "plashch fizicheskogo soprotivleniya" },
	{ OPERATION_CLOAK, "operatsiya plashch" },
	{ BESTEST_CLOAK, "absolyutnyy luchshiy plashch" },
	{ CLOAK_OF_FREEDOM, "plashch svobody" },
	{ BIKINI, "zhenskiy kupal'nik" },
	{ CLOAK_OF_PERMANENCE, "spina postoyanstvu" },
	{ CLOAK_OF_SLOW_DIGESTION, "plashch medlennogo pishchevareniya" },
	{ CLOAK_OF_INFRAVISION, "plashch infravideniyem" },
	{ CLOAK_OF_BANISHING_FEAR, "plashch progonyaya strakha" },
	{ CLOAK_OF_MEMORY, "spina pamyati" },
	{ CLOAK_OF_THE_FORCE, "spina sily" },
	{ CLOAK_OF_SEEING, "plashch videniya" },
	{ CLOAK_OF_CURSE_CATCHING, "plashch proklyatiya lovli" },
	{ LION_CLOAK, "lev plashch" },
	{ TIGER_CLOAK, "tigr plashch" },
	{ CLOAK_OF_PRACTICE, "spina praktiki" },
	{ CLOAK_OF_ELEMENTALISM, "plashch iz elementov" },
	{ PSIONIC_CLOAK, "psionicheskaya plashch" },
	{ SAWED_OFF_SHOTGUN, "obrez" },
	{ SCR_INSTANT_AMNESIA, "mgnovennaya amneziya" },
	{ ROBE_OF_MAGIC_RESISTANCE, "riza soprotivleniye magii" },
	{ ROBE_OF_PERMANENCE, "riza postoyanstvu" },
	{ ROBE_OF_SPELL_POWER, "riza sily zaklinaniy" },
	{ ROBE_OF_FAST_CASTING, "riza bystrogo lit'ya" },
	{ ROBE_OF_ENERGY_SUCTION, "riza vsasyvaniya energii" },
	{ ROBE_OF_RANDOMNESS, "khalat sluchaynosti" },
	{ ROBE_OF_DEFENSE, "khalat oborony" },
	{ ROBE_OF_SPECIALTY, "riza spetsial'nosti" },
	{ ROBE_OF_NASTINESS, "riza zlobnosti" },
	{ ROBE_OF_PSIONICS, "psi khalat" },
	{ UNSPECIFIED_CANDLE, "neopredelennyye svechi" },
	{ POWER_CROSSBOW, "moshchnost' arbaleta" },

	{ SPE_CHAOS_TERRAIN, "khaos mestnosti" },
	{ SPE_RANDOM_SPEED, "sluchaynaya skorost'" },
	{ SPE_VANISHING, "ischezayushchiy" },
	{ SPE_WISHING, "zhelayushchikh" },
	{ SPE_ACQUIREMENT, "priobreteniye" },
	{ SPE_CHAOS_BOLT, "boltom khaos" },
	{ SPE_HELLISH_BOLT, "adskaya boltom" },
	{ SPE_EARTHQUAKE, "zemletryaseniye" },
	{ SPE_LYCANTHROPY, "likantropiyu" },
	{ SPE_BUC_RANDOMIZATION, "buc randomizatsii" },
	{ SPE_LOCK_MANIPULATION, "manipulyatsiya zamok" },
	{ SPE_POLYFORM, "poliform" },
	{ SPE_MESSAGE, "soobshcheniye" },
	{ SPE_RUMOR, "slukh" },
	{ SPE_CURE_RANDOM_STATUS, "vylechit' sluchaynyy status" },
	{ SPE_RESIST_RANDOM_ELEMENT, "soprotivlyayutsya sluchaynyy element" },
	{ SPE_RUSSIAN_ROULETTE, "russkaya ruletka" },
	{ SPE_POSSESSION, "vladeniye" },
	{ SPE_TOTEM_SUMMONING, "totem vyzova" },
	{ SPE_MIMICRY, "mimikriya" },
	{ SPE_HORRIFY, "shokirovat'" },
	{ SPE_TERROR, "uzhas" },
	{ SPE_PHASE_DOOR, "faza dveri" },
	{ SPE_TRAP_DISARMING, "lovushka snyatiya s okhrany" },
	{ SPE_NEXUS_FIELD, "svyazuyushchey pole" },
	{ SPE_COMMAND_DEMON, "komanda demonov" },
	{ SPE_FIRE_GOLEM, "ogon' golemov" },
	{ SPE_DISRUPTION_SHIELD, "razrusheniye shchita" },
	{ SPE_SPELLBINDER, "orator, uvlekayushchiy svoyu auditoriyu" },
	{ SPE_TRACKER, "treker" },
	{ SPE_INERTIA_CONTROL, "kontrol' inertsii" },
	{ SPE_CODE_EDITING, "redaktirovaniye koda" },
	{ SPE_FORGOTTEN_SPELL, "zabyli zaklinaniye" },
	{ SPE_FLOOD, "navodneniye" },
	{ SPE_LAVA, "lavovyy" },
	{ SPE_IRON_PRISON, "zheleznaya tyur'ma" },
	{ SPE_LOCKOUT, "lokaut" },
	{ SPE_CLOUDS, "oblaka" },
	{ SPE_ICE, "led" },
	{ SPE_GROW_TREES, "rastut derev'ya" },
	{ SPE_DRIPPING_TREAD, "kapayet protektora" },
	{ SPE_GEOLYSIS, "geologicheskoye resheniye" },
	{ SPE_ELEMENTAL_BEAM, "elementarnyy puchok" },
	{ SPE_STERILIZE, "sterilizovat'" },
	{ SPE_WIND, "veter" },
	{ SPE_FIRE, "ogon'" },
	{ SPE_ELEMENTAL_MINION, "elementnyy min'on" },
	{ SPE_WATER_BOLT, "boltov vody" },
	{ SPE_AIR_CURRENT, "potok vozdukha" },
	{ SPE_DASHING, "likhoy" },
	{ SPE_MELTDOWN, "rasplavleniye" },
	{ SPE_POISON_BRAND, "yad brend" },
	{ SPE_STEAM_VENOM, "para otrava" },
	{ SPE_HOLD_AIR, "uderzhivat' vozdukh" },
	{ SPE_SWIMMING, "plavaniye" },
	{ SPE_VOLT_ROCK, "vol'tovyy rok" },
	{ SPE_WATER_FLAME, "plamya vody" },
	{ SPE_AVALANCHE, "lavina" },
	{ SPE_MANA_BOLT, "boltov many" },
	{ SPE_ENERGY_BOLT, "boltov energii" },
	{ SPE_ACID_INGESTION, "kislota proglatyvaniye" },
	{ SPE_INDUCE_VOMITING, "vyzvat' rvotu" },
	{ SPE_REBOOT, "perezagruzhat'" },
	{ SPE_HOLY_SHIELD, "svyatoy shchit" },

	{ SPE_FROST, "moroz" },
	{ SPE_TRUE_SIGHT, "istinnoye zreniye" },
	{ SPE_BERSERK, "neistovyy" },
	{ SPE_BLINDING_RAY, "oslepitel'nogo lucha" },
	{ SPE_MAGIC_SHIELD, "zashchita ot magii" },
	{ SPE_WORLD_FALL, "mir osen'yu" },
	{ SPE_ESP, "ekstra-sensornoye vospriyatiye" },
	{ SPE_RADAR, "radiolokatsionnyy" },
	{ SPE_SEARCHING, "poisk" },
	{ SPE_INFRAVISION, "infravideniye" },
	{ SPE_STEALTH, "khitrost'" },
	{ SPE_CONFLICT, "protivorechiye" },
	{ SPE_REGENERATION, "vosstanovleniye" },
	{ SPE_FREE_ACTION, "soprotivleniye paralich" },
	{ SPE_MULTIBEAM, "na neskol'kikh luch" },
	{ SPE_NO_EFFECT, "net effekta" },
	{ SPE_SELFDESTRUCT, "samorazrusheniye" },
	{ SPE_THUNDER_WAVE, "grom volna" },
	{ SPE_BATTERING_RAM, "taran" },
	{ SPE_BURROW, "nora" },
	{ SPE_GAIN_CORRUPTION, "poluchit' korruptsiyu" },
	{ SPE_SWITCHEROO, "obmanchiv razvorot" },
	{ SPE_THRONE_GAMBLE, "tron avantyura" },
	{ SPE_BACKFIRE, "obratnaya vspyshka" },
	{ SPE_DEMEMORIZE, "bol'she ne zapominat'" },
	{ SPE_CALL_THE_ELEMENTS, "nazyvat' elementy" },
	{ SPE_NATURE_BEAM, "priroda sveta" },
	{ SPE_WHISPERS_FROM_BEYOND, "shepoty iz-za" },
	{ SPE_STASIS, "staz" },
	{ SPE_CRYOGENICS, "fizika nizkikh temperatur" },
	{ SPE_REDEMPTION, "vykup" },
	{ SPE_HYPERSPACE_SUMMON, "giperprostranstvo prizvat'" },
	{ SPE_SATISFY_HUNGER, "utolit' golod" },
	{ SPE_RAIN_CLOUD, "dozhdevyye oblaka" },
	{ SPE_POWER_FAILURE, "sboy pitaniya" },
	{ SPE_VAPORIZE, "isparyat'sya" },
	{ SPE_TUNNELIZATION, "tunnel'naya" },
	{ SPE_BOMBING, "bombardirovka" },
	{ SPE_DRAGON_BLOOD, "drakon krovi" },
	{ SPE_ANTI_MAGIC_FIELD, "antimagicheskiy pole" },
	{ SPE_ANTI_MAGIC_SHELL, "antimagicheskiy obolochki" },
	{ SPE_CURE_WOUNDED_LEGS, "vylechit' ranenyye nogi" },
	{ SPE_ANGER_PEACEFUL_MONSTER, "gnev mirnyy monstr" },
	{ SPE_UNTAME_MONSTER, "dikiy monstr" },
	{ SPE_UNLEVITATE, "ostanovit' levitatsiyu" },
	{ SPE_DETECT_WATER, "obnaruzheniya vody" },
	{ SPE_APPLY_NAIL_POLISH, "primenit' lak dlya nogtey" },
	{ SPE_ENCHANT, "ocharovyvat'" },
	{ SPE_DRY_UP_FOUNTAIN, "vysykhayut fontan" },
	{ SPE_TAKE_SELFIE, "prinyat' selfi" },
	{ SPE_SNIPER_BEAM, "snayperskiy luch" },
	{ SPE_CURE_GLIB, "vylechit' boykim" },
	{ SPE_CURE_MONSTER, "vylechit' monstra" },
	{ SPE_MANA_BATTERY, "batareya many" },
	{ SPE_THORNS, "shipy" },
	{ SPE_REROLL_ARTIFACT, "randomizatsii artefakt" },
	{ SPE_FINAL_EXPLOSION, "okonchatel'nyy vzryv" },
	{ SPE_CUTTING, "rezka" },

	{ SCR_DETECT_WATER, "obnaruzheniya vody" },
	{ WAN_SPELLBINDER, "orator, uvlekayushchiy svoyu auditoriyu" },
	{ WAN_INERTIA_CONTROL, "kontrol' inertsii" },
	{ SCR_GEOLYSIS, "geologicheskoye resheniye" },
	{ WAN_STERILIZE, "sterilizovat'" },
	{ UNAFFECTED_CANDLE, "nezatronutyy svechi" },
	{ SPECIFIC_CANDLE, "konkretnaya svecha" },
	{ __CANDLE, "0 svecha" },

	{ SCR_VILENESS, "podlost'" },

	{ INKA_LEASH, "inka povodok" },
	{ INKA_SADDLE, "inka sedlo" },
	{ UNSTABLE_STETHOSCOPE, "neustoychivyy stetoskopa" },
	{ PAPER_SHIELD, "bumaga shchit" },
	{ RADIOACTIVE_DAGGER, "radioaktivnyy kinzhal" },
	{ SECRETION_DAGGER, "sekretsiya kinzhal" },
	{ BITUKNIFE, "bitum nozh" },
	{ MEASURER, "izmeritel'nyy pribor" },
	{ COLLUSION_KNIFE, "sgovor nozh" },
	{ SPIRIT_AXE, "dukh akh" },
	{ SOFT_MATTOCK, "myagkiy motyga" },
	{ INKA_BLADE, "inka lezviye" },
	{ ETERNIUM_BLADE, "eterniyevaya lezviye" },
	{ PAPER_SWORD, "bumaga mech" },
	{ MEATSWORD, "myaso mech" },
	{ ICKY_BLADE, "nepriglyadnoye lezviye" },
	{ GRANITE_IMPALER, "granit tsepesh" },
	{ ORGANOBLADE, "organicheskoye lezviye" },
	{ BIDENHANDER, "obe ruki oruzhiye" },
	{ INKUTLASS, "chernila kortik" },
	{ HOE_SABLE, "motygoy sobolya" },
	{ YATAGAN, "persidskaya mech" },
	{ PLATINUM_SABER, "platina sabli" },
	{ WILD_BLADE, "dikiy lezviye" },
	{ LEATHER_SABER, "kozha sabli" },
	{ ARCANE_RAPIER, "arkan rapira" },
	{ NATURAL_STICK, "natural'nyy palochka" },
	{ POURED_CLUB, "vlivayut klub" },
	{ DIAMOND_SMASHER, "almaz razbivali klub" },
	{ VERMIN_SWATTER, "nechist' mukhoboyka" },
	{ PLASTIC_MACE, "plastikovyye bulava" },
	{ BRONZE_MACE, "bronzovaya bulava" },
	{ MILL_PAIL, "mel'nitsa vederko" },
	{ BACKHAND_MACE, "naotmash' bulava" },
	{ ASTERISK, "zvezdochka" },
	{ RHYTHMIC_STAR, "ritmichnoye zvezda" },
	{ YESTERDAY_STAR, "vchera zvezda" },
	{ FLOGGER, "vyporot' ustroystvo" },
	{ RIDING_CROP, "khlyst" },
	{ NOVICE_HAMMER, "nachinayushchiy molotok" },
	{ THUNDER_HAMMER, "grom molotok" },
	{ BRIDGE_MUZZLE, "most morda" },
	{ INKA_BOOT, "inka zagruzki" },
	{ SOFT_LADY_SHOE, "myagkaya ledi obuvi" },
	{ STEEL_CAPPED_SANDAL, "stal' shapkami sandalovoye" },
	{ DOGSHIT_BOOT, "zagruzochnyy sobach'yego der'ma" },
	{ IMPACT_STAFF, "personal vozdeystviya" },
	{ TROUTSTAFF, "personal foreli" },
	{ FIRE_STICK, "ogon' palkoy" },
	{ OLDEST_STAFF, "stareyshikh sotrudnikov" },
	{ COLOSSUS_BLADE, "koloss lezviye" },
	{ TUBING_PLIERS, "trubnyye kleshchi" },
	{ CHEMISTRY_SPACE_AXE, "khimiya prostranstvo topor" },
	{ OSBANE_KATANA, "vysokiy uroven' yaponskiy mech" },
	{ WALKING_STICK, "trost'" },
	{ RAIN_PIPE, "vodostochnyye" },
	{ PENIS_POLE, "penis polyus" },
	{ GARDEN_FORK, "sad vilkoy" },
	{ PIKE, "shchuka" },
	{ PHYSICIAN_BAR, "vrach-bar" },
	{ HELMET_BEARD, "shlem boroda" },
	{ TRAFFIC_LIGHT, "svetofor" },
	{ GIANT_SCYTHE, "gigantskaya kosa" },
	{ THRESHER, "molotilka" },
	{ INKA_SPEAR, "inka kop'ye" },
	{ SILK_SPEAR, "shelk kop'ye" },
	{ BRITTLE_SPEAR, "lomkimi kop'ye" },
	{ DRAGON_SPEAR, "drakona kop'ye" },
	{ ASBESTOS_JAVELIN, "asbest drotik" },
	{ HOMING_TORPEDO, "samonavedeniya torpedy" },
	{ COURSE_JAVELIN, "konechno, drotik" },
	{ FOURDENT, "chetyre vmyatiny tsepesh" },
	{ PLOW, "plug" },
	{ POKER_STICK, "poker palki" },
	{ BRONZE_LANCE, "bronza ostroga" },
	{ COMPOST_BOW, "kompost luk" },
	{ FORBIDDEN_ARROW, "zapreshcheno strelka" },
	{ WILDHILD_BOW, "dikaya zhenshchina luk" },
	{ ODOR_SHOT, "zapakh vystrel" },
	{ BRONZE_ARROW, "bronza strelka" },
	{ PAPER_ARROW, "bumaga strelka" },
	{ METAL_SLING, "metall stropa" },
	{ INKA_SLING, "inka stropa" },
	{ PAPER_SHOTGUN, "bumaga drobovika" },
	{ HUNTING_RIFLE, "okhotnich'ye ruzh'ye" },
	{ PROCESS_CARD, "protsess karty" },
	{ ZOOM_SHOT_CROSSBOW, "zum vystrel iz arbaleta" },
	{ BALLISTA, "drevniy arbalet" },
	{ FLEECE_BOLT, "arbalet boyepripasy flis" },
	{ MINERAL_BOLT, "mineral'nyy arbalet boyepripasy" },
	{ PIN_BOLT, "kontaktnyy arbalet boyepripasy" },
	{ INKA_STINGER, "inka zhalo" },
	{ FLIMSY_DART, "nadumannyye drotik" },
	{ SOFT_STAR, "myagkaya zvezda" },
	{ TAR_STAR, "degot' zvezda" },
	{ INKA_SHACKLE, "inka duzhka" },
	{ BULLETPROOF_CHAINWHIP, "puleneprobivayemyye zhuan'byan'" },
	{ SECRET_WHIP, "sekret knut" },
	{ BEAUTIFUL_SHIRT, "krasivaya rubashka" },
	{ RADIOACTIVE_UNDERGARMENT, "radioaktivnyy bel'" },
	{ PETA_COMPLIANT_SHIRT, "PETA-sovmestimyy rubashka" },
	{ PARTIAL_PLATE_MAIL, "chastichnaya laty" },
	{ RIBBED_PLATE_MAIL, "rebristyye laty" },
	{ METAL_LAMELLAR_ARMOR, "metallicheskaya plastinchataya bronya" },
	{ BAR_CHAIN_MAIL, "bar kol'chuga" },
	{ TAR_CHAIN_MAIL, "degot' kol'chuga" },
	{ PEACEKEEPER_MAIL, "mirotvorets pochta" },
	{ GOTHIC_PLATE_MAIL, "goticheskiye laty" },
	{ EMBOSSED_PLATE_MAIL, "rel'yefnyy laty" },
	{ INKA_MITHRIL_COAT, "inka mifrilovaya-pal'to" },
	{ DROVEN_MITHRIL_COAT, "steklo mifrilovaya-pal'to" },
	{ SILK_MAIL, "shelk pochta" },
	{ HEAVY_MAIL, "tyazhelaya pochta" },
	{ CLOAK_OF_PEACE, "plashch mira" },
	{ CLOAK_OF_DIMNESS, "plashch potemneniya" },
	{ ICKY_SHIELD, "nepriglyadnoye shchit" },
	{ HEAVY_SHIELD, "tyazhelyy shchit" },
	{ BARRIER_SHIELD, "bar'yer shchit" },
	{ TROLL_SHIELD, "troll' shchit" },
	{ TARRIER, "degot' bar'yer" },
	{ SHIELD_OF_PEACE, "shchit mira" },
	{ RIN_DIMNESS, "tusklost'" },
	{ AMULET_OF_PEACE, "amulet of mir" },
	{ SECRET_KEY, "sekretnyy klyuch" },
	{ HAIRCLIP, "zazhim dlya volos" },
	{ DATA_CHIP, "chipom dannykh" },
	{ GRASS_WHISTLE, "trava svistok" },
	{ FOG_HORN, "tumannyy gorn" },
	{ CONGLOMERATE_PICK, "konglomerat vybor" },
	{ BRONZE_PICK, "bronzovyy vybor" },
	{ GUITAR, "gitara" },
	{ PIANO, "pianino" },
	{ RADIOGLASSES, "radio ochki" },
	{ EYECLOSER, "glaz blizhe" },
	{ BOSS_VISOR, "boss kozyr'ka" },
	{ DRAGON_EYEPATCH, "drakon glaz patchi" },
	{ SOFT_CHASTITY_BELT, "myagkiy poyas tselomudriya" },
	{ BINNING_KIT, "bining komplekt" },
	{ BUDO_NO_SASU, "ne budo ne sasu" },
	{ LUBRICANT_CAN, "smazka mozhet" },
	{ VERY_HEAVY_BALL, "ochen' tyazhelyy shar" },
	{ HEAVY_CHAIN, "tyazheloy tsepi" },
	{ HEAVY_COMPOST_BALL, "tyazhelyy kompost myach" },
	{ COMPOST_CHAIN, "kompost tsep'" },
	{ DISGUSTING_BALL, "otvratitel'noye myach" },
	{ DISGUSTING_CHAIN, "otvratitel'naya tsep'" },
	{ HEAVY_ELASTHAN_BALL, "tyazhelyy elastan myach" },
	{ ELASTHAN_CHAIN, "elastan tsep'" },
	{ IMPOSSIBLY_HEAVY_NUCLEAR_BALL, "neveroyatno tyazhelyy atomnyy myach" },
	{ NUCLEAR_HOSTAGE_CHAIN, "atomnyy tsep'" },
	{ POT_DIMNESS, "tusklost'" },
	{ SPE_CURE_DIM, "lecheniye tusklost'" },
	{ DISCONNECTED_BOOTS, "otklyuchennyye sapogi" },
	{ DISCONNECT_STONE, "otsoyedinit' kamen'" },
	{ AMULET_OF_SCREWY_INTERFACE, "choknutyy interfeys" },
	{ SCREW_STONE, "vint kamen'" },
	{ BOSS_BOOTS, "boss sapogi" },
	{ BOSSFIGHT_STONE, "boss boy kamen'" },
	{ RARE_HELMET, "redkiy shlem" },
	{ ENTIRE_LEVEL_STONE, "ves' kamen' uroven'" },
	{ AMULET_OF_BONES, "kosti" },
	{ BONE_STONE, "kosti kamen'" },
	{ RIN_AUTOCURSING, "avtomaticheskoye chertykhayas'" },
	{ AUTOCURSE_STONE, "avto proklyatiye kamen'" },
	{ LEVELING_GLOVES, "vyravnivayushchiye perchatki" },
	{ HIGHLEVEL_STONE, "kamen' vysokogo urovnya" },
	{ AMULET_OF_SPELL_FORGETTING, "zaklinaniye zabyt'" },
	{ SPELL_MEMORY_STONE, "zaklinaniye kamen' pamyati" },
	{ SOUND_EFFECT_HELMET, "zvukovoy effekt shlema" },
	{ SOUND_EFFECT_STONE, "zvukovoy effekt kamen'" },
	{ RIN_TIME_SPENDING, "provodit' vremya" },
	{ TIME_USE_STONE, "vremya ispol'zovaniya kamnya" },

	{ EVIL_DRAGON_SCALE_MAIL, "zloy drakon masshtaba elektronnoy pochty" },
	{ EVIL_DRAGON_SCALES, "zlyye drakon'i cheshui" },
	{ MAGIC_DRAGON_SCALE_MAIL, "magiya drakona masshtab pochty" },
	{ MAGIC_DRAGON_SCALES, "magicheskiye drakon'yeskiye vesy" },
	{ CHANTER_SHIRT, "khoreografiya" },
	{ BAD_SHIRT, "plokhaya rubashka" },
	{ DIFFICULT_SHIELD, "slozhnyy shchit" },
	{ MAGICAL_SHIELD, "magicheskiy shchit" },
	{ SPECIAL_SHIELD, "spetsial'nyy shchit" },
	{ EVIL_PLATE_MAIL, "zlaya plastina pochty" },
	{ EVIL_LEATHER_ARMOR, "zlaya kozhanaya bronya" },
	{ SPECIAL_LEATHER_ARMOR, "spetsial'naya kozhanaya bronya" },
	{ MAGE_PLATE_MAIL, "magicheskaya pochta" },
	{ EVIL_DRAGON_SCALE_SHIELD, "zloy shchit drakona" },
	{ MAGIC_DRAGON_SCALE_SHIELD, "magicheskiy shchit drakona" },
	{ AMULET_OF_VULNERABILITY, "amulet of uyazvimost'" },
	{ SCR_ARTIFACT_JACKPOT, "artefaktnyy dzhekpot" },
	{ SCR_BOSS_COMPANION, "boze-kompan'on" },

	{ RIN_LOOTCUT, "otrub dobycha" },
	{ LOOTCUT_STONE, "latunnyy kamen'" },
	{ COMPETITION_BOOTS, "botinki dlya sorevnovaniy" },
	{ MONSTER_SPEED_STONE, "monstr skorost' kamen'" },
	{ SCALER_MITTENS, "cheshuychatyye varezhki" },
	{ SCALING_STONE, "masshtabiruyushchiy kamen'" },
	{ AMULET_OF_HOSTILITY, "amulet of vrazhdebnost'" },
	{ INIMICAL_STONE, "vrazhdebnyy kamen'" },
	{ WHITE_SPELL_CLOAK, "plashch belogo zaklinaniya" },
	{ WHITE_SPELL_STONE, "belyy kamen' zaklinaniy" },
	{ GREYOUT_CLOAK, "zatumanivat'sya plashch" },
	{ GREYOUT_STONE, "zatumanivat'sya kamen'" },
	{ QUASIMODULAR_BOOTS, "kvazimodulyarnyye sapogi" },
	{ QUASAR_STONE, "kvazar kamen'" },
	{ MOMHAT, "mama shlyapu" },
	{ MOMMY_STONE, "namochka" },
	{ CARTRIDGE_OF_HAVING_A_HORROR, "patron s uzhasom" },
	{ HORROR_STONE, "kamen' uzhasa" },
	{ AMULET_OF_EVIL_CRAFTING, "amulet of zloy kraft" },
	{ ARTIFICIAL_STONE, "iskusstvennyy kamen'" },
	{ RIN_FORM_SHIFTING, "izmeneniye formy" },
	{ WEREFORM_STONE, "kamennyy kamen'" },
	{ SINFUL_HEELS, "grekhovnyye kabluki" },
	{ ANTIPRAYER_STONE, "kamennyy antipirat" },
	{ TRUMP_COAT, "kozyrek" },
	{ EVIL_PATCH_STONE, "zloy patch-kamen'" },
	{ BLOODSUCKING_SHOES, "krovososushchiye obuv'" },
	{ HARD_MODE_STONE, "kamen' zhestkogo rezhima" },
	{ CLOAK_OF_SUDDEN_ATTACK, "plashch vnezapnoy ataki" },
	{ SECRET_ATTACK_STONE, "sekretnyy kamen' ataki" },
	{ AMULET_OF_EDIBILITY, "amulet of s''yedobnost'" },
	{ EATER_STONE, "lyudoyed" },
	{ COVETED_BOOTS, "zhelannyye sapogi" },
	{ COVETOUS_STONE, "alchnyy kamen'" },
	{ BORDERLESS_HELMET, "shlem bez poley" },
	{ NON_SEEING_STONE, "ne vidyashchiy kamen'" },
	{ LIGHTLESS_BOOTS, "legkiye sapogi" },
	{ DARKMODE_STONE, "temno-seryy kamen'" },
	{ HELMET_OF_ANTI_SEARCHING, "shlem anti poiska" },
	{ UNFINDABLE_STONE, "neustoychivyy kamen'" },
	{ KILLER_HEELS, "tufli na kablukakh" },
	{ HOMICIDE_STONE, "kamen' ubiystva" },
	{ CLOAK_OF_BAD_TRAPPING, "plashch plokhogo zakhvata" },
	{ MULTITRAPPING_STONE, "mnogozaryadnyy kamen'" },
	{ AMULET_OF_WAKING, "amulet of bodrstvuyushchiy" },
	{ WAKEUP_CALL_STONE, "pobuditel'nyy kamen' vyzova" },
	{ GRAYOUT_CLOAK, "plashch serogo tsveta" },
	{ GRAYOUT_STONE, "seryy kamen'" },
	{ PSEUDO_TELEPORTER_CLOAK, "plashch s psevdo-teleportom" },
	{ GRAY_CENTER_STONE, "seryy tsentral'nyy kamen'" },
	{ CHECKER_BOOTS, "sapogi-sapogi" },
	{ CHECKERBOARD_STONE, "shakhmatnyy kamen'" },
	{ CLOCKLOAK, "chasovoy plashch" },
	{ CLOCKWISE_STONE, "kamen' po chasovoy strelke" },
	{ HELM_OF_COUNTER_ROTATION, "rupor vstrechnogo vrashcheniya" },
	{ COUNTERCLOCKWISE_STONE, "protiv chasovoy strelki" },
	{ RIN_LAGGING, "otstayushchiy" },
	{ LAG_STONE, "lag-kamen'" },
	{ RIN_BLESSCURSING, "blagoslovlyayte proklyatiyem" },
	{ BLESSCURSE_STONE, "kamen' blagosloveniya" },
	{ DELIGHT_HELMET, "voskhititel'nyy shlem" },
	{ DELIGHT_STONE, "voskhititel'nyy kamen'" },
	{ GLOVES_OF_ENERGY_DRAINING, "perchatki dlya otvoda energii" },
	{ DISCHARGE_STONE, "vygruzhayushchiy kamen'" },
	{ AMULET_OF_TRASH, "amulet of musor" },
	{ TRASH_STONE, "musornyy kamen'" },
	{ MESSAGE_FILTER_HELMET, "shlem fil'tra soobshcheniy" },
	{ FILTERING_STONE, "fil'truyushchiy kamen'" },
	{ NOKEDEX_CLOAK, "plashch net monstra entsiklopediya" },
	{ DEFORMATTING_STONE, "deformiruyushchiy kamen'" },
	{ FLICKER_VISOR, "mertsayushchiy shchitok" },
	{ FLICKER_STRIP_STONE, "kamen' s flikker-poloskami" },
	{ AMULET_OF_UNDRESSING, "amulet of razdevayas'" },
	{ UNDRESSING_STONE, "razdevaniye kamnya" },
	{ ELVIS_SHOES, "bosonozhki" },
	{ HYPER_BLUE_STONE, "giper goluboy kamen'" },
	{ NAYLIGHT_CLOAK, "plashch s zakruglennymi uglami" },
	{ NO_LIGHT_STONE, "kamen' bez sveta" },
	{ AIRSTEP_BOOTS, "vozdushnyye shapki" },
	{ PARANOIA_STONE, "paranoyya kamen'" },
	{ SCRIPTED_HELMET, "skriptovyy shlem" },
	{ FLEECE_STONE, "flisovoy kamen'" },
	{ BOOTS_OF_INTERRUPTION, "sapogi preryvaniya" },
	{ INTERRUPTION_STONE, "preryvistyy kamen'" },
	{ RIN_ILLITERACY, "negramotnost'" },
	{ DUSTBIN_STONE, "pylesbornik kamen'" },
	{ BATTERY_CLOAK, "plashch akkumulyatornoy batarei" },
	{ BATTERY_STONE, "akkumulyatornyy kamen'" },
	{ MARY_SUE_GLOVES, "meri perchatki" },
	{ BUTTERFINGER_STONE, "kamen' s maslyanymi kraskami" },
	{ GAUNTLETS_OF_BAD_CASTING, "perchatki plokhogo lit'ya" },
	{ MISCASTING_STONE, "kamen' dlya izvlecheniya" },
	{ EMPTY_LINE_HELMET, "pustoy shlem linii" },
	{ MESSAGE_SUPPRESSION_STONE, "kamen' podavleniya soobshcheniy" },
	{ CLOAK_OF_WRONG_ANNOUNCEMENT, "plashch nepravil'nogo ob''yavleniya" },
	{ STUCK_ANNOUNCEMENT_STONE, "zastryavshiy reklamnyy kamen'" },
	{ STORMY_CLOAK, "shtormovoy plashch" },
	{ STORM_STONE, "shtormovoy kamen'" },
	{ GIMP_CLOAK, "plashch iz kamuflyazha" },
	{ MAXIMUM_DAMAGE_STONE, "maksimal'nyy kamen' urona" },
	{ HIGH_HEELED_SKIERS, "lyzhniki na vysokikh kablukakh" },
	{ LATENCY_STONE, "latentnyy kamen'" },
	{ AMULET_OF_STARLIGHT, "amulet of zvezdnyy svet" },
	{ STARLIT_SKY_STONE, "zvezdnyy kamen' neba" },
	{ SNARENET_CLOAK, "plashch sarrene" },
	{ TRAP_KNOWLEDGE_STONE, "lovushka znaniy" },
	{ HIGH_SCORING_HEELS, "vysokiye kabluki" },
	{ HIGHSCORE_STONE, "rekordnyy kamen'" },
	{ PINK_SPELL_CLOAK, "rozovaya charodeyskaya mantiya" },
	{ PINK_SPELL_STONE, "rozovyy kamen' zaklinaniy" },
	{ GREEN_SPELL_HELMET, "zelenyy shlem zaklinaniy" },
	{ GREEN_SPELL_STONE, "zelenyy kamen' zaklinaniy" },
	{ EVENCORE_CLOAK, "plashch dazhe yadro" },
	{ EVC_STONE, "kamen' chetnoye izobrazheniye yadra" },
	{ UNDERLAYER_CLOAK, "podsherstka" },
	{ UNDERLAID_STONE, "podlyy kamen'" },
	{ METER_GAUNTLETS, "netr perchatki" },
	{ DAMAGE_METER_STONE, "kamkorder povrezhdeniy" },
	{ WEIGHTING_GAUNTLETS, "nagrudnyye perchatki" },
	{ WEIGHT_STONE, "vesovoy kamen'" },
	{ INFOLESS_HELMET, "bezlikiy shlem" },
	{ INFOFUCK_STONE, "infograficheskiy kamen'" },
	{ BLACK_SPELL_GAUNTLETS, "chernyye orfografii perchatki" },
	{ BLACK_SPELL_STONE, "chernyy kamen' zaklinaniy" },
	{ CYAN_SPELL_CLOAK, "plashch s golubym zaklinaniyem" },
	{ CYAN_SPELL_STONE, "goluboy kamen' zaklinaniy" },
	{ REPEATABLE_BOOTS, "povtoryayushchiyesya sapogi" },
	{ HEAP_STONE, "kucha kamney" },
	{ BLUE_SPELL_HELMET, "siniy shlem zaklinaniy" },
	{ BLUE_SPELL_STONE, "siniy kamen' zaklinaniy" },
	{ TRON_BOOTS, "tronnyye sapogi" },
	{ TRON_STONE, "tronnyy kamen'" },
	{ RED_SPELL_HEELS, "krasnyye pyatna zaklinaniy" },
	{ RED_SPELL_STONE, "krasnyy kamen' zaklinaniy" },
	{ HEAVY_GRABBING_GLOVES, "tyazhelyye zakhvatyvayushchiye perchatki" },
	{ TOO_HEAVY_STONE, "slishkom tyazhelyy kamen'" },
	{ ELONGATION_CLOAK, "plashch s udlinennym vyrezom" },
	{ ELONGATED_STONE, "udlinennyy kamen'" },
	{ GAUNTLETS_OF_REVERSE_ENCHANTME, "rukavitsy obratnogo char" },
	{ WRAPOVER_STONE, "obertochnyy kamen'" },
	{ DESTRUCTIVE_HEELS, "destruktivnyye pyatki" },
	{ DESTRUCTION_STONE, "razrushayushchiy kamen'" },
	{ FUCKUP_MELEE_GAUNTLETS, "isportit' rukopashniki v blizhnem boyu" },
	{ MELEE_PREFIX_STONE, "prefiksnyy kamen' rukopashnogo boya" },
	{ MORE_HELMET, "bol'she shlema" },
	{ AUTOMORE_STONE, "avtomobil'nyy kamen'" },
	{ UNFAIR_ATTACK_CLOAK, "plashch s nespravedlivoy atakoy" },
	{ UNFAIR_ATTACK_STONE, "nechestnyy kamen' napadeniya" },
	{ TREASURE_CHEST, "sunduk s sokrovishchami" },
	{ HELM_OF_DECONTAMINATION, "obezzarazhivaniye shlema" },
	{ AMULET_OF_CONTAMINATION_RESIST, "amulet of ustoychivost' k zagryazneniyu" },
	{ CLOAK_OF_CONTAMINATION_RESISTA, "plashch ustoychivost' k zagryazneniyu" },
	{ SCR_EXTRA_HEALING, "dopolnitel'noye istseleniye" },
	{ SCR_GREATER_MANA_RESTORATION, "bol'shaya regeneratsiya many" },
	{ CHARGER, "zaryadnoye ustroystvo" },
	{ MATERIAL_KIT, "nabor materialov" },
	{ POT_BENEFICIAL_EFFECT, "blagopriyatnyy effekt" },
	{ POT_TRAINING, "obucheniye" },
	{ POT_RANDOM_INTRINSIC, "sluchaynyy vnutrenniy" },
	{ INTELLIGENCE_PACK, "paket razvedki" },
	{ HELM_VERSUS_DEATH, "shlem protiv smerti" },
	{ RIN_IMMUNITY_TO_DRAWBRIDGES, "immunitet k razvodyam" },
	{ AMULET_OF_NECK_BRACE, "amulet of sheynyy korset" },
	{ HELM_OF_BEGINNER_S_LUCK, "shlem pobedy novichka" },
	{ WOOLEN_SHIRT, "sherstyanaya rubashka" },
	{ RIN_DISCOUNT_ACTION, "deystviye skidok" },
	{ CLOAK_OF_DISCOUNT_ACTION, "plashch so skidkoy" },
	{ CLOAK_OF_TECHNICALITY, "plashch tekhnichnosti" },
	{ CLOAK_OF_FULL_NUTRITION, "plashch polnogo pitaniya" },
	{ AMULET_OF_TECHNICALITY, "formal'nost'" },
	{ HELM_OF_FULL_NUTRITION, "rud polnogo pitaniya" },
	{ YELLOW_LIGHTSABER, "zheltyy svetovoy mech" },
	{ WHITE_LIGHTSABER, "belyy svetovoy mech" },
	{ VIOLET_LIGHTSABER, "fioletovyy svetovoy mech" },
	{ WHITE_DOUBLE_LIGHTSABER, "belyy dvoynoy svetovoy mech" },

	{ SCR_ASTRALCENSION, "astral'noye voskhozhdeniye" },
	{ SCR_RAGNAROK, "bogi sumerki" },
	{ SCR_MATERIAL_CHANGE, "izmeneniye materiala" },
	{ SCR_OFFLEVEL_ITEM, "tovar ot urovnya" },
	{ STICKSHOE, "palochka" },
	{ NOOB_POLLAX, "novichok polyusnyy topor" },
	{ GREAT_POLLAX, "bol'shoy topor" },
	{ WOODEN_BAR, "derevyannyy brus" },

	{ ORANGE_SPELL_STONE, "oranzhevyy kamen' zaklinaniy" },
	{ VIOLET_SPELL_STONE, "fioletovyy kamen' zaklinaniya" },
	{ LONGING_STONE, "toska" },
	{ CURSED_PART_STONE, "proklyatyy kamen'" },
	{ QUAVERSAL_STONE, "kvartsevyy kamen'" },
	{ APPEARANCE_SHUFFLING_STONE, "peretasovannyy kamen'" },
	{ BROWN_SPELL_STONE, "korichnevyy kamen' zaklinaniy" },
	{ CHOICELESS_STONE, "besshumnyy kamen'" },
	{ GOLDSPELL_STONE, "zolotoy kamen'" },
	{ DEPROVEMENT_STONE, "kamen' dlya uluchsheniya" },
	{ INITIALIZATION_STONE, "kamen' initsializatsii" },
	{ GUSHLUSH_STONE, "kamen' gushlyusha" },
	{ SOILTYPE_STONE, "pochvennyy kamen'" },
	{ DANGEROUS_TERRAIN_STONE, "opasnyy landshaftnyy kamen'" },
	{ FALLOUT_STONE, "osadok" },
	{ MOJIBAKE_STONE, "mokhibakskiy kamen'" },
	{ GRAVATION_STONE, "gravitatsionnyy kamen'" },
	{ UNCALLED_STONE, "neobrabotannyy kamen'" },
	{ EXPLODING_DICE_STONE, "vzryvayushchiysya kamen' v kosti" },
	{ PERMACURSE_STONE, "kamen' permakul'tury" },
	{ SHROUDED_IDENTITY_STONE, "okutannyy kamen' identichnosti" },
	{ FEELER_GAUGE_STONE, "izmeritel'nyy kamen'" },
	{ LONG_SCREWUP_STONE, "dlinnyy otvratitel'nyy kamen'" },
	{ WING_YELLOW_STONE, "krylo zheltogo kamnya" },
	{ LIFE_SAVING_STONE, "spasatel'nyy kamen'" },
	{ CURSEUSE_STONE, "zaklinayushchiy kamen'" },
	{ CUT_NUTRITION_STONE, "razrezannyy kamennyy kamen'" },
	{ SKILL_LOSS_STONE, "kamen' umeniya" },
	{ AUTOPILOT_STONE, "kamen' avtopilota" },
	{ FORCE_STONE, "silovoy kamen'" },
	{ MONSTER_GLYPH_STONE, "kamen' glista monstra" },
	{ CHANGING_DIRECTIVE_STONE, "izmeneniye direktivnogo kamnya" },
	{ CONTAINER_KABOOM_STONE, "konteynernyy kamen'" },
	{ STEAL_DEGRADE_STONE, "ukrast' degradirovat' kamen'" },
	{ LEFT_INVENTORY_STONE, "ostavlennyy kamennyy inventar'" },
	{ FLUCTUATING_SPEED_STONE, "koleblyushchiysya kamen' skorosti" },
	{ TARMUSTROKINGNORA_STONE, "tarma poglazhivaya kamen' nory" },
	{ FAILURE_STONE, "otkaz kamen'" },
	{ BRIGHT_CYAN_SPELL_STONE, "yarkiy goluboy kamen' zaklinaniya" },
	{ FREQUENTATION_SPAWN_STONE, "chastyy kamen'" },
	{ PET_AI_STONE, "zhivotnoye iskusstvennyy kamen'" },
	{ SATAN_STONE, "satanskiy kamen'" },
	{ REMEMBERANCE_STONE, "pamyatnyy kamen'" },
	{ POKELIE_STONE, "kamen' tykat' lozh'yu" },
	{ AUTOPICKUP_STONE, "kamen' dlya avtopilota" },
	{ DYWYPI_STONE, "kamen' vy khotite, chtoby vashe imushchestvo bylo identifitsirovano" },
	{ SILVER_SPELL_STONE, "serebryanyy kamen' zaklinaniy" },
	{ METAL_SPELL_STONE, "metallicheskiy kamen' zaklinaniy" },
	{ PLATINUM_SPELL_STONE, "platinovyy kamen' zaklinaniy" },
	{ MANLER_STONE, "chelovek kamen'" },
	{ DOORNING_STONE, "dvernoy kamen'" },
	{ NOWNSIBLE_STONE, "neob''yatnyy kamen'" },
	{ ELM_STREET_STONE, "vyaza ulichnyy kamen'" },
	{ MONNOISE_STONE, "monakh" },
	{ RANG_CALL_STONE, "pozvonil kamnyu" },
	{ RECURRING_SPELL_LOSS_STONE, "povtoryayushchiysya kamen' poteri zaklinaniy" },
	{ ANTI_TRAINING_STONE, "anti trenirovochnyy kamen'" },
	{ TECHOUT_STONE, "tekhnicheskiy kamen'" },
	{ STAT_DECAY_STONE, "staticheskiy kamen' raspada" },
	{ MOVEMORK_STONE, "peremeshchat' morkov' kamen'" },

	{ BROWN_SHIT_CLOAK, "korichnevyy der'movyy plashch" },
	{ ANTICALL_CLOAK, "antikvarnyy plashch" },
	{ YELLOW_WING, "zheltoye krylo" },
	{ ELESDE_CLOAK, "plashch eleyd" },
	{ CLOAK_OF_GRAVATION, "plashch gravitatsii" },
	{ CLOAK_OF_NONFOLLOWING, "plashch neprivyazannogo" },
	{ SPELL_RETAIN_CLOAK, "plashch, sokhranyayushchiy plashch" },
	{ CLOAK_OF_FAST_DECAY, "plashch bystrogo raspada" },
	{ QUAVERSAL_HELMET, "kvartsevyy shlem" },
	{ HELM_OF_SHUFFLING, "shlem peretasovki" },
	{ GOLDSPELL_HELMET, "shlem zolotogo shlema" },
	{ AIRHEAD_CAP, "kryshka vozdushnoy podushki" },
	{ FALLOUT_HELMET, "vypadshiy shlem" },
	{ IDENTIFY_CURD_HELMET, "opredelit' tvorozhnyy shlem" },
	{ BAEAEAEP_SPY_HELMET, "shlem shpiona baeaeaep" },
	{ CRUNCHER_HELMET, "shlemofon" },
	{ DISTORTED_GRIMACE, "iskazhennaya grimasa" },
	{ ELM_ET, "il'm-dr" },
	{ SANEMAKER_HELMET, "shlem sandalii" },
	{ ORANGE_SPELL_GLOVES, "oranzhevyye orfografii perchatki" },
	{ MOJIBAKE_GLOVES, "perchatki modzheykey" },
	{ DIXPLOSION_GLOVES, "perchatki dlya vzryv v kosti" },
	{ LEFT_APPENDAGE_GLOVES, "levyye pridatochnyye perchatki" },
	{ POKELIE_GLOVES, "perchatki tykat' lozh'yu" },
	{ ALLPICKING_GLOVES, "perforirovannyye perchatki" },
	{ SILVER_SPELL_GLOVES, "serebryanyye orfografii perchatki" },
	{ SPELLDRAIN_GLOVES, "perchatki utechka zaklinaniy" },
	{ VIOLET_BEAUTY_HEELS, "fioletovyye pyatki krasoty" },
	{ LONG_POINTY_HEELS, "dlinnyye ostrokonechnyye kabluki" },
	{ SELF_WILLED_HEELS, "svoyevol'nyye kabluki" },
	{ SOIL_CLINGING_BOOTS, "sapogi s pochvoy" },
	{ PERMANENTLY_BLACK_SHOES, "postoyanno chernyye tufli" },
	{ AUTOSCOOTER_HEELS, "avtomobil'nyye kabluki" },
	{ FORCEFUL_BOOTS, "sil'nyye botinki" },
	{ BUM_BUM_BOOTS, "zadniye botinki" },
	{ SADISTIC_BOOTS, "polucheniye udovol'stviya sapogi" },
	{ FLUCKERING_BOOTS, "mertsayushchiy sapogi" },
	{ SPRAP_BOOTS, "remen' pressa sapogi" },
	{ FAILED_ATTEMPT_BOOTS, "neudachnyye popytki zagruzki" },
	{ BRIGHT_CYAN_BEAUTIES, "yarkiye golubyye krasoty" },
	{ SIA_BOOTS, "ooo sapogi" },
	{ PLATINUM_SPELL_BOOTS, "platinovyye orfografii sapogi" },
	{ INVIS_WALKERS, "khodoki nevidimost'" },
	{ NOISY_BOOTS, "shumnyye sapogi" },
	{ MORK_BOOTS, "bol'she vilki sapogi" },
	{ RIN_WIMPINESS, "buduchi malodushnym" },
	{ RIN_USING_HAZARD, "s ispol'zovaniyem opasnosti" },
	{ RIN_EXERTION_LOSS, "poterya nagruzki" },
	{ RIN_PETCAT, "domashnyaya koshka" },
	{ RIN_POSSESSION_IDENTIFICATION, "identifikatsiya vladeniya" },
	{ RIN_DAYSHIFT, "dnevnaya smena" },
	{ RIN_DECONSTRUCTION, "dekonstruktsiya" },
	{ AMULET_OF_ITEM_INSECURITY, "amulet of neuverennost' v pozitsii" },
	{ AMULET_OF_STOPPED_EVOLUTION, "amulet of prekrashcheniye evolyutsii" },
	{ AMULET_OF_INITIALIZATION_FAILU, "amulet of sboy initsializatsii" },
	{ AMULET_OF_REAL_SCREWERY, "amulet of nastoyashchaya neudacha" },
	{ AMULET_OF_ENEMY_SAVING, "amulet of spaseniye protivnika" },
	{ AMULET_OF_INCREASED_FREQUENCY, "amulet of uvelichennaya chastota" },
	{ AMULET_OF_SPELL_METAL, "amulet of orfografii zaklinaniye" },
	{ AMULET_OF_TECHOUT, "amulet of tekhnika iz" },

	{ POT_SANITY, "vmenyayemost'" },
	{ POT_CURE_INSANITY, "istselyayushcheye bezumiye" },
	{ WAN_INSANITY, "nevmenyayemost'" },
	{ SCR_BAD_EQUIPMENT, "plokhoye oborudovaniye" },
	{ WAN_BAD_EQUIPMENT, "plokhoye oborudovaniye" },

	{ CLOAK_OF_BAD_PART, "plashch plokhoy chasti" },
	{ AMULET_OF_BAD_PART, "amulet of plokhoy chasti" },
	{ AMULET_OF_EVIL_VARIANT, "amulet of zlogo varianta" },
	{ BAD_PART_STONE, "plokhaya chast' kamnya" },
	{ COMPLETELY_BAD_PART_STONE, "polnost'yu plokhaya chast' kamnya" },
	{ EVIL_VARIANT_STONE, "zloy variant kamnya" },

	{ SANE_TREBLE_STONE, "vmenyayemyy troynoy kamen'" },
	{ STATCREASE_STONE, "kamen' dlya pokhudeniya" },
	{ SIMEOUT_STONE, "vytashchit' kamen'" },
	{ AMULET_OF_SANITY_TREBLE, "amulet of troystvennost' zdravomysliya" },
	{ RIN_STAT_DECREASE, "umen'sheniye statov" },
	{ RIN_SANITY_TIMEOUT, "vremya ozhidaniya" },

	{ LIZARD_SCALE_MAIL, "yashcheritsa masshtabnaya pochta" },
	{ LIZARD_SCALES, "vesy yashcheritsy" },
	{ FLAME_MOUNTAIN, "gora plameni" },
	{ BEAM_REFLECTOR_GUN, "luch otrazhatelya" },
	{ TENNIS_RACKET, "tennisnaya raketka" },
	{ ENERGY_SAP, "zhivitsa" },

	{ SCR_NASTY_CURSE, "protivnoye proklyatiye" },
	{ SCR_HYBRIDIZATION, "gibridizatsiya" },
	{ SCR_SECURE_CURSE_REMOVAL, "bezopasnoye snyatiye proklyatiya" },

	{ WAN_DISENCHANTMENT, "razocharovaniye" },
	{ WAN_CONTAMINATION, "zagryazneniye" },
	{ WAN_TREMBLING, "drozh'" },

	{ SCR_MAKE_PENTAGRAM, "sdelat' pentagrammu" },
	{ SCR_WELL_BUILDING, "stroitel'stvo skvazhin" },
	{ SCR_DRIVING, "vozhdeniye" },
	{ SCR_TABLE_FURNITURE, "nastol'naya mebel'" },
	{ SCR_EMBEDDING, "vlozheniya" },
	{ SCR_MATTRESS_SLEEPING, "spal'nyy matras" },

	{ SCR_GRAVE, "mogila" },
	{ SCR_TUNNELS, "tunneli" },
	{ SCR_FARMING, "sel'skoye khozyaystvo" },
	{ SCR_MOUNTAINS, "gory" },
	{ SCR_DIVING, "dayving" },
	{ SCR_CRYSTALLIZATION, "kristallizatsiya" },
	{ SCR_MOORLAND, "vereshchatnik" },
	{ SCR_URINE, "mocha" },
	{ SCR_QUICKSAND, "plyvun" },
	{ SCR_STYX, "stiks" },
	{ SCR_SNOW, "sneg" },
	{ SCR_ASH, "yasen'" },
	{ SCR_SAND, "pesok" },
	{ SCR_PAVING, "moshcheniye" },
	{ SCR_HIGHWAY, "shosse" },
	{ SCR_GRASSLAND, "lug" },
	{ SCR_NETHER, "nizhniy" },
	{ SCR_STALACTITE, "stalaktit" },
	{ SCR_CRYPT, "kripta" },
	{ SCR_BUBBLE_BOBBLE, "puzyr'" },
	{ SCR_RAIN, "dozhd'" },

	{ WAN_CHAOS_TERRAIN, "khaos mestnosti" },
	{ WAN_FLEECY_TERRAIN, "vorsistyy rel'yef" },

	{ SPE_GRAVE, "mogila" },
	{ SPE_TUNNELS, "tunneli" },
	{ SPE_FARMING, "sel'skoye khozyaystvo" },
	{ SPE_MOUNTAINS, "gory" },
	{ SPE_DIVING, "dayving" },
	{ SPE_CRYSTALLIZATION, "kristallizatsiya" },
	{ SPE_MOORLAND, "vereshchatnik" },
	{ SPE_URINE, "mocha" },
	{ SPE_QUICKSAND, "plyvun" },
	{ SPE_STYX, "stiks" },
	{ SPE_ATTUNE_MAGIC, "magiya nastroyki" },
	{ SPE_SNOW, "sneg" },
	{ SPE_ASH, "yasen'" },
	{ SPE_SAND, "pesok" },
	{ SPE_PAVING, "moshcheniye" },
	{ SPE_HIGHWAY, "shosse" },
	{ SPE_GRASSLAND, "lug" },
	{ SPE_NETHER_MIST, "nizhniy tuman" },
	{ SPE_STALACTITE, "stalaktit" },
	{ SPE_CRYPT, "kripta" },
	{ SPE_BUBBLE_BOBBLE, "puzyr'" },
	{ SPE_RAIN, "dozhd'" },
	{ DARK_BAR, "temnaya polosa" },
	{ YITH_TENTACLE, "shchupal'tse" },
	{ BOHEMIAN_EAR_SPOON, "bogemnaya ushnaya lozhka" },
	{ POLE_LANTERN, "fonarnyy stolb" },
	{ NASTYPOLE, "protivnyy stolb" },
	{ SPE_GAIN_SPACT, "poluchit' tolchok" },
	{ SPE_METAL_GUARD, "metallicheskaya zashchita" },
	{ SCR_MISSING_CODE, "nedostayushchiy kod" },
	{ YOGA_PANTS, "shtany dlya yogi" },
	{ WINDMILL_BLADE, "lezviye vetryanoy mel'nitsy" },
	{ GRAIN_SCYTHE, "zernovaya kosa" },
	{ ORNATE_MACE, "bogato ukrashennaya bulava" },
	{ RAZOR_WHIP, "britva knut" },
	{ HELM_OF_OPAQUE_THOUGHTS, "shlem neprozrachnykh mysley" },
	{ SECOND_SKIN, "vtoraya kozha" },
	{ CLOAK_OF_FLIGHT, "plashch poleta" },
	{ HIDE_SHIELD, "spryatat' shchit" },
	{ ROGUES_GLOVES, "zhul'nicheskiye perchatki" },
	{ GREEN_GOWN, "zelenoye plat'ye" },
	{ HELM_OF_ANTI_MAGIC, "shlem antimagii" },
	{ OILSKIN_COIF, "kapyushon" },
	{ KALASHNIKOV, "russkaya shturmovaya vintovka" },
	{ SPE_MAGIC_WHISTLING, "volshebnyy svist" },
	{ RIN_JUMPING, "pryzhki" },
	{ RIN_ILLNESS, "bolezn'" },

	{ SPE_ORE_MINING, "dobycha rudy" },
	{ SPE_BOILER_KABOOM, "zvuk vzryva kotla" },
	{ SPE_DEFOG, "predotvrashcheniye zapotevaniya" },
	{ SPE_SWAP_POSITION, "pozitsiya svop" },
	{ SPE_SHUFFLE_MONSTER, "sluchaynyy monstr" },
	{ SPE_PET_SYRINGE, "domashneye zhivotnoye shprits" },
	{ SPE_BUC_KNOWLEDGE, "blagoslovennoye proklyatoye znaniye" },
	{ SPE_PREACHING, "propovedovaniye" },
	{ SPE_RESIST_PARALYSIS, "soprotivlyat'sya paralichu" },
	{ SPE_KEEP_SATIATION, "prodolzhay nasyshchat'sya" },
	{ SPE_TECH_BOOST, "tekhnicheskaya podderzhka" },
	{ SPE_CONTINGENCY, "nepredvidennyye obstoyatel'stva" },
	{ SPE_AULE_SMITHING, "kuznets" },
	{ SPE_HORSE_HOP, "konnyy pryzhok" },
	{ SPE_LINE_LOSS, "poterya linii" },
	{ SPE_TACTICAL_NUKE, "takticheskoye yadernoye oruzhiye" },
	{ SPE_RAGNAROK, "bozhiy rassvet" },
	{ SPE_ARMOR_SMASH, "razrusheniye dospekhov" },
	{ SPE_STRANGLING, "zadykhayushchiysya" },
	{ SPE_PARTICLE_CANNON, "pushka chastits" },
	{ SPE_ONE_POINT_SHOOT, "odna tochka strelyat'" },
	{ SPE_BLOOD_STREAM, "krovotok" },
	{ SPE_SHINING_WAVE, "siyayushchaya volna" },
	{ SPE_GROUND_STOMP, "topat' zemlyu" },
	{ SPE_DIRECTIVE, "direktiva" },
	{ SPE_NERVE_POISON, "nervnyy yad" },
	{ SPE_POWDER_SPRAY, "poroshkovyy sprey" },
	{ SPE_FIREWORKS, "feyyerverk" },
	{ SPE_AIMBOT_LIGHTNING, "pritsel molnii" },
	{ SPE_ENHANCE_BREATH, "uluchshit' dykhaniye" },
	{ SPE_GOUGE_DICK, "vybit' chlen" },
	{ SPE_BODYFLUID_STRENGTHENING, "ukrepleniye telesnoy zhidkosti" },
	{ SPE_GEYSER, "geyzer" },
	{ SPE_BUBBLING_HOLE, "puzyrchataya dyra" },
	{ SPE_PURIFICATION, "ochistka" },
	{ SPE_ADD_SPELL_MEMORY, "dobavit' pamyat' zaklinaniy" },
	{ SPE_NEXUSPORT, "port svyazi" },
	{ SPE_GIANT_FOOT, "gigantskaya noga" },
	{ SPE_ANTI_TELEPORTATION, "anti-teleportatsiya" },
	{ FEMINISM_DRAGON_SCALES, "feminizm drakon'yey cheshui" },
	{ FEMINISM_DRAGON_SCALE_MAIL, "feminizm drakon'yey pochty" },
	{ FEMINISM_DRAGON_SCALE_SHIELD, "feminizm shchit drakon'yey cheshui" },
	{ NEGATIVE_DRAGON_SCALES, "otritsatel'nyye vesy drakona" },
	{ NEGATIVE_DRAGON_SCALE_MAIL, "otritsatel'naya cheshuya drakona" },
	{ NEGATIVE_DRAGON_SCALE_SHIELD, "otritsatel'nyy shchit masshtaba drakona" },
	{ HEROIC_DRAGON_SCALES, "geroicheskiye vesy drakona" },
	{ HEROIC_DRAGON_SCALE_MAIL, "geroicheskaya cheshuya drakona" },
	{ HEROIC_DRAGON_SCALE_SHIELD, "shchit geroicheskoy drakon'yey cheshui" },
	{ LEAD_CLUMP, "svinets" },
	{ BONE_FRAGMENT, "oskolok kosti" },
	{ HEAVY_BRICK_BALL, "tyazhelyy kirpichnyy myach" },
	{ HEAVY_PAPER_BALL, "tyazhelyy bumazhnyy sharik" },
	{ HEAVY_CHROME_BALL, "tyazhelyy khromirovannyy shar" },
	{ HEAVY_NANO_BALL, "tyazhelyy miniatyurnyy shar" },
	{ IMPOSSIBLY_HEAVY_SAND_BALL, "neveroyatno tyazhelyy sharik s peskom" },
	{ HEAVY_METAL_BALL, "tyazhelyy metallicheskiy shar" },
	{ HEAVY_PLASTIC_BALL, "tyazhelyy plastikovyy shar" },
	{ HEAVY_SHADOW_BALL, "tyazhelyy tenevoy shar" },
	{ HEAVY_LEAD_BALL, "tyazhelyy svintsovyy myach" },
	{ IMPOSSIBLY_HEAVY_ETHER_BALL, "neveroyatno tyazhelyy efirnyy shar" },
	{ HEAVY_WAX_BALL, "tyazhelyy vosk" },
	{ HEAVY_WOOD_BALL, "tyazhelyy derevyannyy shar" },
	{ HEAVY_COPPER_BALL, "tyazhelyy mednyy shar" },
	{ HEAVY_SILVER_BALL, "tyazhelyy serebryanyy shar" },
	{ IMPOSSIBLY_HEAVY_OBSIDIAN_BALL, "neveroyatno tyazhelyy shar iz obsidiana" },
	{ BULKY_POURPOOR_BALL, "gromozdkiy shar" },
	{ BULKY_GEMSTONE_BALL, "gromozdkiy shar iz dragotsennykh kamney" },
	{ BULKY_FLESH_BALL, "gromozdkiy myachik" },
	{ BULKY_MITHRIL_BALL, "gromozdkiy mifrilovyy shar" },
	{ HEAVY_CLOTH_BALL, "tyazhelyy shar iz tkani" },
	{ HEAVY_TAR_BALL, "tyazhelyy gudron" },
	{ HEAVY_SILK_BALL, "tyazhelyy shelkovyy shar" },
	{ HEAVY_LEATHER_BALL, "tyazhelyy kozhanyy myach" },
	{ WONDER_BALL, "chudo-shar" },
	{ NULL_BALL, "nulevoy myach" },
	{ LIQUID_BALL, "zhidkiy shar" },
	{ BRICK_CHAIN, "kirpichnaya tsep'" },
	{ PAPER_CHAIN, "bumazhnaya tsepochka" },
	{ CHROME_SCOURGE, "khrom bich" },
	{ NANO_NUNCHIAKU, "miniatyurnyy yaponskiy instrument dlya razbivaniya" },
	{ SAND_HOSTAGE_CHAIN, "pesochnaya tsepochka zalozhnikov" },
	{ METAL_CHAIN, "metallicheskaya tsep'" },
	{ PLASTIC_CHAIN, "plastikovaya tsep'" },
	{ SHADOW_SCOURGE, "bich teney" },
	{ LEAD_NUNCHIAKU, "vedushchiy yaponskiy instrument" },
	{ ETHER_HOSTAGE_CHAIN, "set' zalozhnikov efira" },
	{ WAX_CHAIN, "voskovaya tsep'" },
	{ WOOD_CHAIN, "derevyannaya tsep'" },
	{ COPPER_SCOURGE, "mednyy bich" },
	{ SILVER_NUNCHIAKU, "serebryanyy yaponskiy instrument dlya razbivaniya" },
	{ OBSIDIAN_HOSTAGE_CHAIN, "obsidianovaya tsep' zalozhnikov" },
	{ POURPOOR_LASH, "oblivat'" },
	{ GEMSTONE_LASH, "resnitsa dragotsennogo kamnya" },
	{ FLESH_LASH, "plot' plet'" },
	{ MITHRIL_LASH, "mifrilovaya plet'" },
	{ CLOTH_CHAIN, "tkanevaya tsepochka" },
	{ TAR_CHAIN, "gudron tsep'" },
	{ SILK_SCOURGE, "shelkovyy bich" },
	{ LEATHER_NUNCHIAKU, "yaponskiy instrument dlya naneseniya udara po kozhe" },
	{ WONDER_CHAIN, "chudo tsepi" },
	{ NULL_CHAIN, "nulevaya tsep'" },
	{ LIQUID_CHAIN, "zhidkaya tsep'" },
	{ ETHER_HORN, "efirnyy rog" },
	{ SHADOW_HORN, "tenevoy rog" },
	{ CHROME_HORN, "khromovyy rog" },
	{ BRICK_PICK, "vybor kirpicha" },
	{ NANO_PICK, "nanorazmernyy vybor" },
	{ DARK_HORN, "temnyy rog" },
	{ ARCANE_HORN, "taynyy rog" },
	{ NANO_HAMMER, "nanorazmernyy molotok" },
	{ LIGHTWHIP, "khlystom" },
	{ CERAMIC_KNIFE, "nozh keramicheskiy" },
	{ NANO_AXE, "nanorazmernyy topor" },
	{ ETERNIUM_MATTOCK, "vechnaya motyga" },
	{ SAND_SWORD, "pesochnyy mech" },
	{ VOLCANIC_BROADSWORD, "vulkanicheskiy mech" },
	{ HEAVY_LONG_SWORD, "tyazhelyy dlinnyy mech" },
	{ AUTOMATIC_KATANA, "avtomaticheskiy yaponskiy mech" },
	{ SHADOWBLADE, "klinok teney" },
	{ ETHER_SAW, "efirnaya pila" },
	{ DESERT_SWORD, "mech pustyni" },
	{ CHROME_BLADE, "khromirovannoye lezviye" },
	{ ETERNIUM_SABER, "vechnaya sablya" },
	{ DARKNESS_CLUB, "klub t'my" },
	{ LEAD_FILLED_MACE, "svintsovaya bulava" },
	{ SHINY_STAR, "blestyashchaya zvezda" },
	{ ETHER_KNOUT, "efirnyy knut" },
	{ SPIKERACK, "zubchataya reyka" },
	{ MASSIVE_STAFF, "massivnyy personal" },
	{ CHROME_STAFF, "khromirovannyy posokh" },
	{ LEADBAR, "vedushchiy polyus" },
	{ NANO_POLE, "nanorazmernyy polyus" },
	{ ETERNAL_POLE, "vechnyy polyus" },
	{ CERAMIC_SPEAR, "keramicheskoye kop'ye" },
	{ BRICK_MISSILE, "kirpichnaya raketa" },
	{ DIFFICULT_TRIDENT, "trudnyy trezubets" },
	{ BRICKLAYER_BAR, "kamenshchik polyus" },
	{ WIND_BOW, "veter luk" },
	{ SNIPESLING, "strelyat' iz bekasa" },
	{ LEAD_BULLET, "svintsovaya pulya" },
	{ LEAD_SHOT, "vedushchiy vystrel" },
	{ POISON_BOLT, "yadovityy snaryad" },
	{ HEAVY_CROSSBOW_BOLT, "tyazhelyy arbalet" },
	{ ETHER_BOLT, "efirnyy snaryad" },
	{ SAND_DART, "pesochnyy drotik" },
	{ NANO_SHURIKEN, "nanorazmernaya zvezda" },
	{ DARK_BATARANG, "temnyy letuchaya mysh' bumerang" },
	{ ETHER_WHIP, "efirnyy chlyst" },
	{ METAL_SHIRT, "metallicheskaya rubashka" },
	{ FULL_LEAD_SUIT, "polnyy svintsovyy kostyum" },
	{ NANO_SUIT, "kostyum nanomasshtaba" },
	{ CLOAK_OF_COAGULATION, "plashch svertyvaniya" },
	{ CLOAK_OF_SCENT, "plashch aromata" },
	{ SCENTY_HELMET, "aromatnyy shlem" },
	{ CHROME_SHIELD, "khromirovannyy shchit" },
	{ ANTISHADOW_SHIELD, "anti tenevoy shchit" },
	{ LADY_BOOTS, "zhenskiye boyevyye sapogi" },
	{ STILETTO_SANDALS, "bosonozhki na shpil'ke" },
	{ RIN_DIMINISHED_BLEEDING, "umen'shennoye krovotecheniye" },
	{ AMULET_OF_SCENT, "amulet of nyukh" },
	{ RED_STRING, "krasnaya stroka" },
	{ TOILET_ROLL, "rulon tualetnoy bumagi" },
	{ WONDER_PLATE, "chudo-tarelka" },
	{ MEDIUM_MITHRIL_COAT, "srednyaya mifrilovaya shuba" },
	{ TAPERED_MAIL, "konicheskaya pochta" },
	{ EXCITING_ARMOR, "zakhvatyvayushchaya bronya" },
	{ LORICATED_CLOAK, "lorirovannyy plashch" },
	{ BASINET, "kotel shlem" },
	{ ROCKET_GAUNTLETS, "rukavitsy rakety" },
	{ ANCIENT_SHIELD, "drevniy shchit" },
	{ ITALIAN_HEELS, "ital'yanskiye kabluki" },
	{ WONDER_DAGGER, "chudo-kinzhal" },
	{ UNKNOWN_KNIFE, "neizvestnyy nozh" },
	{ SHARP_AXE, "ostryy topor" },
	{ SHORT_BLADE, "korotkiy klinok" },
	{ ELEGANT_BROADSWORD, "elegantnyy mech" },
	{ MAIN_SWORD, "glavnyy mech" },
	{ ROMAN_SWORD, "rimskiy mech" },
	{ MYTHICAL_SABLE, "mificheskiy sobol'" },
	{ CRYPTIC_SABER, "zagadochnaya sablya" },
	{ BLOW_AKLYS, "udar aklis" },
	{ TELEPHONE, "obzvanivat'" },
	{ BATLETH, "bet'lekh" },
	{ ELITE_BATLETH, "elitnyy bat'let" },
	{ CALCULATOR, "vychislitel'" },
	{ WEIGHTED_FLAIL, "vzveshennyy tsep" },
	{ BLOCK_HEELED_SANDAL, "sandalii na vysokom kabluke" },
	{ PROSTITUTE_SHOE, "prostitutka obuv'" },
	{ BO_STAFF, "obucheniye personala" },
	{ LONG_POLE, "dlinnyy stolb" },
	{ SHARP_POLE, "ostryy polyus" },
	{ RANDOSPEAR, "sluchaynoye kop'ye" },
	{ STACK_JAVELIN, "stek drotik" },
	{ SPEC_LANCE, "spekulyatsiya kop'ye" },
	{ META_BOW, "meta-luk" },
	{ WONDER_ARROW, "chudo strelka" },
	{ MATERIAL_BOLT, "veshchestvo shkvoren'" },
	{ RANDOM_DART, "sluchaynyy drotik" },
	{ CUBIC_STAR, "kubicheskaya zvezda" },
	{ SEXPLAY_WHIP, "seksual'naya igra knut" },
	{ MYSTERY_PICK, "tayna vybora" },
	{ MYSTERY_LIGHTSABER, "taynyy svetovoy mech" },
	{ SLING_AMMO, "strop boyepripasy" },
	{ CANCEL_DRAGON_SCALES, "otmenit' vesy drakona" },
	{ CANCEL_DRAGON_SCALE_MAIL, "otmenit' pochtu masshtaba drakona" },
	{ CANCEL_DRAGON_SCALE_SHIELD, "otmenit' shchit iz cheshui drakona" },
	{ SYMBIOTE, "simbiont" },
	{ SPE_SYMHEAL, "sim lechit'" },
	{ SCR_SYMBIOSIS, "simbioz" },
	{ SCR_TERRAFORMING, "terraformirovaniye" },
	{ SCR_INFERIOR_MATERIAL, "khudshiy material" },
	{ SCR_REGULAR_MATERIAL, "obychnyy material" },
	{ SCR_SUPERIOR_MATERIAL, "prevoskhodnyy material" },
	{ SCR_ILLUSION, "illyuziya" },
	{ SCR_EVIL_VARIANT, "zloy variant" },
	{ SCR_FEMINISM, "feminizm" },
	{ WAN_BLEEDING, "krovotecheniye" },
	{ WAN_UNDRESSING, "razdevaniya" },
	{ SCR_HEAL_OTHER, "lechit' drugikh" },
	{ CORONA_DRAGON_SCALES, "cheshuyki korony drakona" },
	{ CORONA_DRAGON_SCALE_MAIL, "korona drakon'yey cheshui" },
	{ CORONA_DRAGON_SCALE_SHIELD, "korona shchit iz cheshui drakona" },

	/* todo area */


	{0, "" }
};

STATIC_OVL struct Jitem Ancient_items[] = {
	{ ASSAULT_RIFLE, "husum miltiq" },
	{ AUTO_SHOTGUN, "avtomatik shar miltiq" },
	{ AXE, "bolta" },
	{ BAG_OF_HOLDING, "o'tkazish xalta" },
	{ BANDAGE, "bint" },
	{ BASEBALL_BAT, "beysbol ko'rshapalak" },
	{ BATTLE_AXE, "urush ax" },
	{ BLINDFOLD, "ko'zi bog'lik holda" },
	{ BOW, "yoy" }, /* Nethack-- */
	{ BRASS_LANTERN, "guruch mum" },
	{ BROADSWORD, "keng dudama qilich" },
	{ BRONZE_PLATE_MAIL, "bronza plastinka pochta" },
	{ BULLET, "o'q" },
	{ BULLWHIP, "qamchi" },
	{ CHEST, "ko'krak" },
	{ CLOVE_OF_GARLIC, "sarimsoq chinnigullar" },
	{ CLUB, "yo'g'oon tayoq" },
	{ CRAM_RATION, "tiqilinch ratsion" },
	{ CROSSBOW, "qo'ndoqli kamon" },
	{ CROSSBOW_BOLT, "surma zulfin" },
	{ CRYSTAL_PLATE_MAIL, "kristalli plastinka pochta" },
	{ DAGGER, "xanjar" },
	{ DART, "tortish" },
	{ DENTED_POT, "zarar etkazilgan tova" },
	{ DWARVISH_MATTOCK, "mitti qo'shbosh" },
	{ EGG, "tuxum" },
	{ FLAIL, "qimirlatmoq" },
	{ FLY_SWATTER, "delicisi uchib" },
	{ FRAG_GRENADE, "parcalanma bombasi" },
	{ FUMBLE_BOOTS, "siypalamoq chizilmasin" },
	{ FOOD_RATION, "oziq-ovqat ratsion" },
	{ GAUNTLETS_OF_FUMBLING, "paypaslab izlay oxirgi qo'lqop" },
	{ GAUNTLETS_OF_POWER, "kuch qo'lqop" },
	{ GLAIVE, "tashlab nayza" },
	{ GRAPPLING_HOOK, "qayiqchani tuzoq" },
	{ GRENADE_LAUNCHER, "granatamyot" },
	{ GUISARME, "taxta qurol" },
	{ HALBERD, "oybolta" },
	{ HACKER_S_FOOD, "qani oziq-ovqat" },
	{ HEAVY_MACHINE_GUN, "og'ir mashina injektori" },
	{ HELMET, "dubulg'a" },
	{ INSECT_SQUASHER, "hasharotlar buzuvchi" },
	{ IRON_CHAIN, "temir zanjir" },
	{ JAVELIN, "otiladigan nayza" },
	{ KNIFE, "pichoq" },
	{ LANCE, "nishtar" },
	{ LARGE_BOX, "katta zarba" },
	{ LEATHER_JACKET, "teri ko'ylagi" },
	{ LEATHER_CLOAK, "charm plash" },
	{ LEATHER_GLOVES, "charm qo'lqop" },
	{ LEMBAS_WAFER, "kichkina kapsula" },
	{ LOCK_PICK, "qulf terimiga" },
	{ LONG_SWORD, "uzoq qilich" },
	{ LUCERN_HAMMER, "beda bolg'a" },
	{ MORNING_STAR, "ertalab yulduz" },
	{ PANCAKE, "blin" },
	{ PHIAL, "shishasiga" },
	{ PICK_AXE, "terimiga bolta" },
	{ PISTOL, "to'pponcha" },
	{ PLATE_MAIL, "plastinka pochta" },
	{ POT_BOOZE, "olam" },
	{ QUARTERSTAFF, "chorak xodimlari" },
	{ RIFLE, "miltiq" },
	{ ROBE, "rido" },
	{ ROCK, "qoya" },
	{ ROCKET, "reaktiv snaryad" },
	{ ROCKET_LAUNCHER, "raketa uchirish" },
	{ RUNESWORD, "runi qilich" },
	{ SACK, "xalta" },
	{ SCALPEL, "jarroh pichog'i" },
	{ SCIMITAR, "egri olmos" },
	{ SHORT_SWORD, "qisqa qilich" },
	{ SHOTGUN, "shar miltiq" },
	{ SHOTGUN_SHELL, "guvala" },
	{ SHURIKEN, "uchirish yulduz" },
	{ SILVER_DAGGER, "kumush xanjar" },
	{ SILVER_SPEAR, "kumush nayza" },
	{ SMALL_SHIELD, "kichik qalqon" },
	{ SNIPER_RIFLE, "mergan otuvchi miltik" },
	{ SPEAR, "nayza" },
	{ SPLINT_MAIL, "parcha pochta" },
	{ SPRIG_OF_WOLFSBANE, "hookatmayanadam shox" },
	{ STEEL_WHIP, "po'lat qamchi" },
	{ STETHOSCOPE, "tibbiy asbob" },
	{ STILETTO, "yuqori poshnalar" },
	{ SUBMACHINE_GUN, "makinaly kun" },
	{ TALLOW_CANDLE, "moy sham" },
	{ TORPEDO, "suv osti raketa" },
	{ TRIDENT, "vilka" },
	{ T_SHIRT, "ko'ylak" },
	{ TWO_HANDED_SWORD, "ikki qo'l qilich" },
	{ UNICORN_HORN, "bir shoxli afsonaviy hayvon shox" },
	{ WAR_HAMMER, "urush bolg'a" },
	{ WAX_CANDLE, "mum sham" },
	{ WATER_WALKING_BOOTS, "suv yurish chizilmasin" },
	{ WEDGE_SANDALS, "xanjar sandal" },
	{ WOODEN_FLUTE, "yog'och nay" },
	{ WOODEN_STAKE, "yog'och ustun" },
	{ WOODEN_HARP, "yog'och arfa" },
	{ WHETSTONE, "qayroqtosh" },
	{ VICTORIAN_UNDERWEAR, "qurilish ichki" },

	{ WORM_TOOTH, "chuvalchang tish" },
	{ CRYSKNIFE, "billur pichoq" },
	{ KATANA, "feodal jangchi qilich" },
	{ ELECTRIC_SWORD, "elektr qilich" },
	{ TSURUGI, "pichoq kesmoq" },
	{ SILVER_SABER, "kumush qilich" },
	{ GOLDEN_SABER, "oltin qilich" },
	{ MACE, "moja" },
	{ SILVER_MACE, "kumush moja" },
	{ FLANGED_MACE, "flansli moja" },
	{ JAGGED_STAR, "taram yulduz" },
	{ DEVIL_STAR, "shayton yulduz" },
	{ MALLET, "to'qmoq" },
	{ WEDGED_LITTLE_GIRL_SANDAL, "xususda kichik qiz shippak" },
	{ SOFT_GIRL_SNEAKER, "yumshoq qiz sport poyafzal" },
	{ STURDY_PLATEAU_BOOT_FOR_GIRLS, "qizchalar uchun mustahkam plato bagajnik" },
	{ HUGGING_BOOT, "havola etdi bagajnik" },
	{ BLOCK_HEELED_COMBAT_BOOT, "blok-o'tish jangovar bagajnik" },
	{ WOODEN_GETA, "yog'och geta" },
	{ LACQUERED_DANCING_SHOE, "leyk raqs poyafzal" },
	{ HIGH_HEELED_SANDAL, "yuqori o'tish shippak" },
	{ SEXY_LEATHER_PUMP, "amaliyotni charm nasos" },
	{ SPIKED_BATTLE_BOOT, "kirpi urush yuklash" },
	{ BATTLE_STAFF, "urush xodimlari" },
	{ PARTISAN, "tarafdor" },
	{ SPETUM, "uzoq nayza" },
	{ RANSEUR, "egri kutuplu" },
	{ BARDICHE, "taxta kutuplu" },
	{ VOULGE, "er ot nayza" },
	{ FAUCHARD, "hodisalar kutuplu" },
	{ BILL_GUISARME, "taqdim etgan kutuplu" },
	{ BEC_DE_CORBIN, "chalg'i" },
	{ SPIRIT_THROWER, "ruh irg'ituvchi" },
	{ STYGIAN_PIKE, "do'zax payk" },
	{ SLING, "tasma g'ov" },
	{ RUBBER_HOSE, "rezina shlang" },
	{ STRIPED_SHIRT, "bula ko'ylak" },
	{ RUFFLED_SHIRT, "ajoyib ko'ylak" },
	{ PLASTEEL_ARMOR, "plastik sovuti" },
	{ MUMMY_WRAPPING, "mumya o'tkazish" },
	{ LAB_COAT, "laboratoriya palto" },
	{ POISONOUS_CLOAK, "zaharli plash" },
	{ FEDORA, "yumshoq fetr shlapa" },
	{ DUNCE_CAP, "befahm idish" },
	{ HELM_OF_OPPOSITE_ALIGNMENT, "qarama-qarshi muvofiq rul" },
	{ STEEL_SHIELD, "po'lat qalqoni" },
	{ SHIELD_OF_REFLECTION, "aks ettirish qalqon" },
	{ SHIELD_OF_MOBILITY, "harakat qalqon" },
	{ DANCING_SHOES, "raqslari poyafzali" },
	{ SWEET_MOCASSINS, "shirin hindiston poyafzal" },
	{ SOFT_SNEAKERS, "yumshoq shippak" },
	{ FEMININE_PUMPS, "nazokatli haydaydi" },
	{ LEATHER_PEEP_TOES, "charm dikiz-oyoq barmoqlari" },
	{ HIPPIE_HEELS, "uchratdim to'piqlarni" },
	{ COMBAT_STILETTOS, "jangovar sandal chizilmasin" },
	{ FIRE_BOOTS, "yong'in chizilmasin" },
	{ ZIPPER_BOOTS, "fermuar chizilmasin" },
	{ BAG_OF_TRICKS, "fokuslar yukxalta" },
	{ BUGLE, "shisha munchoq" },
	{ LAND_MINE, "fugas" },
	{ BEARTRAP, "oyi makr" },
	{ SPOON, "qoshiq" },
	{ TORCH, "mash'al" },
	{ LASER_SWATTER, "lazer delicisi" },
	{ EXPENSIVE_CAMERA, "qimmat foto mashinasi" },
	{ MIRROR, "oyna" },
	{ CRYSTAL_BALL, "kristalli to'p" },
	{ LENSES, "linzalari" },
	{ TOWEL, "sochiq" },
	{ LEATHER_SADDLE, "egar" },
	{ LEATHER_LEASH, "qilsin" },
	{ TINNING_KIT, "kalaylama to'plam" },
	{ MEDICAL_KIT, "tibbiy to'plam" },
	{ TIN_OPENER, "kalay ochishi" },
	{ CAN_OF_GREASE, "surtma mumkin" },
	{ MAGIC_MARKER, "sehrli isaretleyici" },
	{ SWITCHER, "o'zgartirish tugmasini" },
	{ TRIPE_RATION, "qorin ratsion" },
	{ CORPSE, "murda" },
	{ KELP_FROND, "o'tlar barg" },
	{ EUCALYPTUS_LEAF, "evkalipt barg" },
	{ APPLE, "olma" },
	{ CARROT, "sabzi" },
	{ PEAR, "nok" },
	{ ASIAN_PEAR, "osiyolik nok" },
	{ LEMON, "limon" },
	{ ORANGE, "apelsin" },
	{ MUSHROOM, "qo'ziqorin" },
	{ MELON, "qovun" },
	{ LUMP_OF_ROYAL_JELLY, "ari suti olib borib" },
	{ CREAM_PIE, "solmay pirojnoe" },
	{ SANDWICH, "buterbrod" },
	{ CANDY_BAR, "shakar bo'lagi" },
	{ FORTUNE_COOKIE, "mulk cookie fayl" },
	{ CHEESE, "pishloq" },
	{ HOLY_WAFER, "muqaddas kapsula" },
	{ TIN, "qalay" },
	{ DILITHIUM_CRYSTAL, "ikki kishilik litiy billur" },
	{ DIAMOND, "olmos" },
	{ LOADSTONE, "jalb hech kim" },
	{ SALT_CHUNK, "tuz bo'lak" },
	{ FLINT, "zajigalka tosh" },
	{ BOULDER, "yumaloq katta tosh" },
	{ LOADBOULDER, "og'ir shag'al" },
	{ STATUE, "haykal" },
	{ FIGURINE, "haykalcha" },
	{ HEAVY_IRON_BALL, "og'ir temir to'p" }, /* juda og'ir temir to'p = very heavy iron ball */
	{ ROTATING_CHAIN, "qaytib zanjir" },
	{ TAIL_SPIKES, "quyruq tikan" },
	{ FAERIE_FLOSS_RHING, "maftun qiladigan xom ipak rishtasi" },
	{ SEGFAULT_VENOM, "segmentatsiyasi aybi zahar" },
	
	{ RIN_ADORNMENT, "bezash" },
	{ RIN_HUNGER, "ochlik" },
	{ RIN_DISARMING, "qurolsizlantirish" },
	{ RIN_NUMBNESS, "hissizlik" },
	{ RIN_HALLUCINATION, "gallutsinatsiya" },
	{ RIN_CURSE, "qarg'ish" },
	{ RIN_MOOD, "kayfiyat" },
	{ RIN_PROTECTION, "himoya" },
	{ RIN_PROTECTION_FROM_SHAPE_CHAN, "shakl almashtirayotganlarning himoya" },
	{ RIN_SLEEPING, "uyqu" },
	{ RIN_STEALTH, "ayyorlik" },
	{ RIN_SUSTAIN_ABILITY, "qobiliyatini davom ettirish" },
	{ RIN_WARNING, "ogohlantirish" },
	{ RIN_AGGRAVATE_MONSTER, "hayvon og'irlashtiradigan" },
	{ RIN_COLD_RESISTANCE, "sovuq qarshilik" },
	{ RIN_FEAR_RESISTANCE, "qo'rquv qarshilik" },
	{ RIN_GAIN_CONSTITUTION, "konstitutsiyani ega" },
	{ RIN_GAIN_DEXTERITY, "ko'nikma ega" },
	{ RIN_GAIN_INTELLIGENCE, "aql ega" },
	{ RIN_GAIN_STRENGTH, "kuch ega" },
	{ RIN_GAIN_WISDOM, "donolik ega" },
	{ RIN_TIMELY_BACKUP, "o'z vaqtida zaxira" },
	{ RIN_INCREASE_ACCURACY, "aniqligini oshirish" },
	{ RIN_INCREASE_DAMAGE, "zarar oshirish" },
	{ RIN_SLOW_DIGESTION, "sekin hazm" },
	{ RIN_INVISIBILITY, "ko'rinmas" },
	{ RIN_POISON_RESISTANCE, "zahar qarshilik" },
	{ RIN_SEE_INVISIBLE, "ko'rinmas qarang" },
	{ RIN_SHOCK_RESISTANCE, "chaqmoqni qarshilik" },
	{ RIN_SICKNESS_RESISTANCE, "kasallik immunitetning" },
	{ RIN_FIRE_RESISTANCE, "yong'in qarshilik" },
	{ RIN_FREE_ACTION, "bepul aksiyalar" },
	{ RIN_LEVITATION, "ko'rib ularni" },
	{ RIN_REGENERATION, "yangilanishi" },
	{ RIN_SEARCHING, "puxta" },
	{ RIN_TELEPORTATION, "teleportatsiya" },
	{ RIN_CONFLICT, "mojaro" },
	{ RIN_POLYMORPH, "bir polimorfunun" },
	{ RIN_POLYMORPH_CONTROL, "bir polimorfunun nazorat" },
	{ RIN_TELEPORT_CONTROL, "nazorat gulan" },
	{ AMULET_OF_CHANGE, "amulet of o'zgarish" },
	{ AMULET_OF_DRAIN_RESISTANCE, "amulet of qarshilik drenaj" },
	{ AMULET_OF_ESP, "amulet of qo'shimcha hissiy idrok qilish" },
	{ AMULET_OF_UNDEAD_WARNING, "amulet of bo'lmagan-o'lik ogohlantirish" },
	{ AMULET_OF_OWN_RACE_WARNING, "amulet of o'z irqi ogohlantirish" },
	{ AMULET_OF_POISON_WARNING, "amulet of zahar ogohlantirish" },
	{ AMULET_OF_COVETOUS_WARNING, "amulet of ochko'z ogohlantirish" },
	{ AMULET_OF_FLYING, "amulet of uchib" },
	{ AMULET_OF_LIFE_SAVING, "amulet of hayot tejash" },
	{ AMULET_OF_MAGICAL_BREATHING, "amulet of sehrli nafas" },
	{ AMULET_OF_REFLECTION, "amulet of aks" },
	{ AMULET_OF_RESTFUL_SLEEP, "amulet of tinchlantiradigan uyqu" },
	{ AMULET_OF_BLINDNESS, "amulet of ko'rlik" },
	{ AMULET_OF_STRANGULATION, "amulet of bo'g'ilish" },
	{ AMULET_OF_UNCHANGING, "amulet of o'zgarmagan" },
	{ AMULET_VERSUS_POISON, "amulet of zahar nisbatan" },
	{ AMULET_VERSUS_STONE, "amulet of tosh nisbatan" },
	{ AMULET_OF_DEPETRIFY, "amulet of teskari toshbo'ron" },
	{ AMULET_OF_MAGIC_RESISTANCE, "amulet of sehrli qarshilik" },
	{ AMULET_OF_SICKNESS_RESISTANCE, "amulet of kasallik immunitetning" },
	{ AMULET_OF_SWIMMING, "amulet of suzish" },
	{ AMULET_OF_DISINTEGRATION_RESIS, "amulet of parchalanish qarshilik" },
	{ AMULET_OF_ACID_RESISTANCE, "amulet of kislota qarshilik" },
	{ AMULET_OF_REGENERATION, "amulet of yangilanishi" },
	{ AMULET_OF_CONFLICT, "amulet of mojaro" },
	{ AMULET_OF_FUMBLING, "amulet of paypaslab izlay oxirgi" },
	{ AMULET_OF_SECOND_CHANCE, "amulet of ikkinchi imkoniyat" },
	{ AMULET_OF_DATA_STORAGE, "amulet of ma'lumotlarni saqlash" },
	{ AMULET_OF_WATERWALKING, "amulet of suv yurish" },
	{ AMULET_OF_HUNGER, "amulet of ochlik" },
	{ AMULET_OF_POWER, "amulet of davlat" },
	{ POT_FRUIT_JUICE, "mevali sharbat" },
	{ POT_SEE_INVISIBLE, "ko'rinmas qarang" },
	{ POT_SICKNESS, "kasallik" },
	{ POT_SLEEPING, "uyqu" },
	{ POT_CLAIRVOYANCE, "oldindan bilish" },
	{ POT_CONFUSION, "xijolat tortish" },
	{ POT_HALLUCINATION, "gallutsinatsiya" },
	{ POT_HEALING, "davolash" },
	{ POT_EXTRA_HEALING, "qo'shimcha davolash" },
	{ POT_RESTORE_ABILITY, "qobiliyatini tiklash" },
	{ POT_BLINDNESS, "ko'rlik" },
	{ POT_ESP, "qo'shimcha hissiy idrok qilish" },
	{ POT_GAIN_ENERGY, "energiya ega" },
	{ POT_GAIN_HEALTH, "daromad sog'liqni saqlash" },
	{ POT_BANISHING_FEAR, "voz kechishning qo'rquv" },
	{ POT_ICE, "muz" },
	{ POT_FIRE, "yong'in" },
	{ POT_FEAR, "qo'rqinch" },
	{ POT_STUNNING, "ajoyib" },
	{ POT_NUMBNESS, "hissizlik" },
	{ POT_SLIME, "shilliq" },
	{ POT_URINE, "isemek" },
	{ POT_CANCELLATION, "bekor qilish" },
	{ POT_INVISIBILITY, "ko'rinmas" },
	{ POT_MONSTER_DETECTION, "hayvonlar aniqlash" },
	{ POT_OBJECT_DETECTION, "moslamalarni aniqlash" },
	{ POT_ENLIGHTENMENT, "ma'lumotlar" },
	{ POT_FULL_HEALING, "to'liq shifo" },
	{ POT_LEVITATION, "ko'rib ularni" },
	{ POT_POLYMORPH, "bir polimorfunun" },
	{ POT_MUTATION, "o'zgarish" },
	{ POT_SPEED, "tezlik" },
	{ POT_ACID, "kislota" },
	{ POT_OIL, "neft" },
	{ POT_SALT_WATER, "tuz suv" },
	{ POT_GAIN_ABILITY, "daromad qobiliyati" },
	{ POT_GAIN_LEVEL, "daromad darajasi" },
	{ POT_INVULNERABILITY, "yaralanmazlik" },
	{ POT_PARALYSIS, "falaj" },
	{ POT_EXTREME_POWER, "haddan tashqari kuch" },
	{ POT_RECOVERY, "qayta tiklash" },
	{ POT_HEROISM, "qahramonlik" },
	{ POT_CYANIDE, "sianid" },
	{ POT_RADIUM, "radiy" },
	{ POT_JOLT_COLA, "turtki qfyib" },
	{ POT_PAN_GALACTIC_GARGLE_BLASTE, "kastrulka galaktikalar chayqash imkon" },
	{ POT_WATER, "suv" },
	{ POT_BLOOD, "qon" },
	{ POT_VAMPIRE_BLOOD, "yovuz qon" },
	{ POT_AMNESIA, "es-hushini yo'qotish" },
	{ SCR_CREATE_MONSTER, "hayvon yaratish" },
	{ SCR_SUMMON_UNDEAD, "bo'lmagan-o'lik chaqirish" },
	{ SCR_TAMING, "qo'lga o'rganmoq" },
	{ SCR_LIGHT, "engil" },
	{ SCR_FOOD_DETECTION, "oziq-ovqat aniqlash" },
	{ SCR_GOLD_DETECTION, "oltin aniqlash" },
	{ SCR_IDENTIFY, "aniqlash" },
	{ SCR_INVENTORY_ID, "inventarizatsiya-keng aniqlash" },
	{ SCR_MAGIC_MAPPING, "sehrli xaritalash" },
	{ SCR_FLOOD, "suv toshqini" },
	{ SCR_GAIN_MANA, "sirli kuch topish" },
	{ SCR_CONFUSE_MONSTER, "hayvon aralashtirmang" },
	{ SCR_SCARE_MONSTER, "sarosima hayvon" },
	{ SCR_ENCHANT_WEAPON, "qurol sehrlamoq" },
	{ SCR_ENCHANT_ARMOR, "sovuti sehrlamoq" },
	{ SCR_REMOVE_CURSE, "la'natni olib tashlash" },
	{ SCR_TELEPORTATION, "teleportatsiya" },
	{ SCR_TELE_LEVEL, "seminarlar darajasi" },
	{ SCR_WARPING, "coezgue" },
	{ SCR_FIRE, "yong'in" },
	{ SCR_EARTH, "yer" },
	{ SCR_DESTROY_ARMOR, "sovuti halok" },
	{ SCR_AMNESIA, "es-hushini yo'qotish" },
	{ SCR_BAD_EFFECT, "yomon ta'sir" },
	{ SCR_HEALING, "davolash" },
	{ SCR_MANA, "sirli kuch" },
	{ SCR_CURE, "qattiqlashishi" },
	{ SCR_TRAP_DISARMING, "makr qurolsizlantirish" },
	{ SCR_STANDARD_ID, "yagona identifikatsiya" },
	{ SCR_CHARGING, "zaryad" },
	{ SCR_GENOCIDE, "irqiy qirg'inchilikni" },
	{ SCR_PUNISHMENT, "jazo" },
	{ SCR_STINKING_CLOUD, "titilgan bulut" },
	{ SCR_TRAP_DETECTION, "makr aniqlash" },
	{ SCR_ACQUIREMENT, "olish" },
	{ SCR_PROOF_ARMOR, "dalil zirh" },
	{ SCR_PROOF_WEAPON, "dalil qurol" },
	{ SCR_MASS_MURDER, "ommaviy qotillik" },
	{ SCR_UNDO_GENOCIDE, "irqiy qirg'inchilikni qaytarib" },
	{ SCR_REVERSE_IDENTIFY, "aniqlash teskari" },
	{ SCR_WISHING, "xohlovchi" },
	{ SCR_CONSECRATION, "bag'ishlash" },
	{ SCR_ENTHRONIZATION, "so'rilarini" },
	{ SCR_FOUNTAIN_BUILDING, "fontan qurilishi" },
	{ SCR_SINKING, "botgan" },
	{ SCR_WC, "suv maxfiy" },
	{ SCR_LAVA, "lobonning" },
	{ SCR_GROWTH, "o'sish" },
	{ SCR_ICE, "muz" },
	{ SCR_CLOUDS, "bulutlar" },
	{ SCR_BARRHING, "temir majmuasi" },
	{ SCR_LOCKOUT, "qulflash" },
	{ SCR_STONING, "toshbo'ron" },
	{ SCR_ROOT_PASSWORD_DETECTION, "ildiz parol ochish" },
	{ SCR_TRAP_CREATION, "makr yaratish" },
	{ SCR_SLEEP, "uyqu" },
	{ SCR_BLANK_PAPER, "bo'sh qog'oz" },
	{ SPE_FORCE_BOLT, "kuch murvat" },
	{ SPE_CREATE_MONSTER, "hayvon yaratish" },
	{ SPE_DRAIN_LIFE, "drenaj hayot" },
	{ SPE_COMMAND_UNDEAD, "buyruq bo'lmagan o'lik" },
	{ SPE_SUMMON_UNDEAD, "bo'lmagan-o'lik chaqirish" },
	{ SPE_DISINTEGRATION, "parchalanish" },
	{ SPE_STONE_TO_FLESH, "go'sht uchun tosh" },
	{ SPE_HEALING, "davolash" },
	{ SPE_CURE_BLINDNESS, "qattiqlashishi ko'rlik" },
	{ SPE_CURE_NUMBNESS, "qattiqlashishi qattiq uyqu" },
	{ SPE_BLIND_SELF, "ko'r o'zini o'zi" },
	{ SPE_CURE_SICKNESS, "qattiqlashishi kasallik" },
	{ SPE_CURE_HALLUCINATION, "qattiqlashishi gallutsinatsiya" },
	{ SPE_CURE_CONFUSION, "qattiqlashishi tartibsizlik" },
	{ SPE_CURE_BURN, "qattiqlashishi kuygan" },
	{ SPE_CONFUSE_SELF, "o'zini o'zi aralashtirmang" },
	{ SPE_CURE_STUN, "qattiqlashishi bayiltici" },
	{ SPE_STUN_SELF, "bayiltici o'zini o'zi" },
	{ SPE_EXTRA_HEALING, "qo'shimcha davolash" },
	{ SPE_FULL_HEALING, "to'liq shifo" },
	{ SPE_RESTORE_ABILITY, "qobiliyatini tiklash" },
	{ SPE_BANISHING_FEAR, "voz kechishning qo'rquv" },
	{ SPE_CURE_FREEZE, "qattiqlashishi ishga solmay" },
	{ SPE_CREATE_FAMILIAR, "tanish yaratish" },
	{ SPE_LIGHT, "engil" },
	{ SPE_DARKNESS, "qorong'ilik" },
	{ SPE_DETECT_MONSTERS, "hayvonlar aniqlash" },
	{ SPE_DETECT_FOOD, "oziq-ovqat aniqlash" },
	{ SPE_DISSOLVE_FOOD, "oziq-ovqat tarqatib" },
	{ SPE_DETECT_ARMOR_ENCHANTMENT, "zirh sehr aniqlash" },
	{ SPE_CLAIRVOYANCE, "oldindan bilish" },
	{ SPE_DETECT_UNSEEN, "g'aybni aniqlash" },
	{ SPE_IDENTIFY, "aniqlash" },
	{ SPE_DETECT_TREASURE, "xazina aniqlash" },
	{ SPE_MAGIC_MAPPING, "sehrli xaritalash" },
	{ SPE_ENTRAPPING, "kabarciklarinin" },
	{ SPE_FINGER, "barmoq" },
	{ SPE_CHEMISTRY, "kimyo" },
	{ SPE_DETECT_FOOT, "oyoq aniqlash" },
	{ SPE_FORBIDDEN_KNOWLEDGE, "man ilm" },
	{ SPE_CONFUSE_MONSTER, "hayvon aralashtirmang" },
	{ SPE_SLOW_MONSTER, "sekin hayvon" },
	{ SPE_CAUSE_FEAR, "sabab qo'rquv" },
	{ SPE_CHARM_MONSTER, "jozibasi hayvon" },
	{ SPE_ENCHANT_WEAPON, "qurol sehrlamoq" },
	{ SPE_ENCHANT_ARMOR, "sovuti sehrlamoq" },
	{ SPE_CHARGING, "zaryad" },
	{ SPE_GENOCIDE, "irqiy qirg'inchilikni" },
	{ SPE_PROTECTION, "himoya" },
	{ SPE_RESIST_POISON, "zahar qarshilik" },
	{ SPE_RESIST_SLEEP, "uyqu qarshilik" },
	{ SPE_ENDURE_COLD, "sovuq sabr-toqat" },
	{ SPE_ENDURE_HEAT, "issiqlik bardosh" },
	{ SPE_INSULATE, "yakkalamoq" },
	{ SPE_REMOVE_CURSE, "la'natni olib tashlash" },
	{ SPE_REMOVE_BLESSING, "ne'matini olib tashlash" },
	{ SPE_TURN_UNDEAD, "bo'lmagan-o'lik o'girib" },
	{ SPE_ANTI_DISINTEGRATION, "piyodalarga parchalanish" },
	{ SPE_BOTOX_RESIST, "Oziq-ovqat zaharlanishi qarshilik" },
	{ SPE_ACIDSHIELD, "kislota qalqon" },
	{ SPE_GODMODE, "alloh xil" },
	{ SPE_RESIST_PETRIFICATION, "toshbo'ron qarshilik" },
	{ SPE_JUMPING, "o'tish" },
	{ SPE_HASTE_SELF, "shoshilish o'zini o'zi" },
	{ SPE_ENLIGHTEN, "ma'lumotlar" },
	{ SPE_INVISIBILITY, "ko'rinmas" },
	{ SPE_AGGRAVATE_MONSTER, "hayvon og'irlashtiradigan" },
	{ SPE_LEVITATION, "ko'rib ularni" },
	{ SPE_TELEPORT_AWAY, "nur uzoqda" },
	{ SPE_PASSWALL, "o'tkazgan devor" },
	{ SPE_POLYMORPH, "bir polimorfunun" },
	{ SPE_MUTATION, "o'zgarish" },
	{ SPE_LEVELPORT, "seminarlar darajasi" },
	{ SPE_KNOCK, "taqillatmoq" },
	{ SPE_FLAME_SPHERE, "olov shar" },
	{ SPE_FREEZE_SPHERE, "ishga solmay soha" },
	{ SPE_SHOCKING_SPHERE, "yomon soha" },
	{ SPE_ACID_SPHERE, "kislota soha" },
	{ SPE_WIZARD_LOCK, "ustasi qulf" },
	{ SPE_DIG, "chopmoq" },
	{ SPE_CANCELLATION, "bekor qilish" },
	{ SPE_REFLECTION, "aks" },
	{ SPE_PARALYSIS, "falaj" },
	{ SPE_REPAIR_ARMOR, "ta'mirlash zirh" },
	{ SPE_CORRODE_METAL, "metall yemoq" },
	{ SPE_PETRIFY, "toshga aylanmoq" },
	{ SPE_MAGIC_MISSILE, "sehrli raketa" },
	{ SPE_FIREBALL, "yong'in to'p" },
	{ SPE_CONE_OF_COLD, "sovuq konusning" },
	{ SPE_SLEEP, "uyqu" },
	{ SPE_FINGER_OF_DEATH, "o'lim nuridir" },
	{ SPE_LIGHTNING, "chaqmoq" },
	{ SPE_POISON_BLAST, "zahar portlash" },
	{ SPE_ACID_STREAM, "kislota oqimi" },
	{ SPE_SOLAR_BEAM, "quyosh nur" },
	{ SPE_BLANK_PAPER, "bo'sh qog'oz" },
	{ SPE_STINKING_CLOUD, "titilgan bulut" },
	{ SPE_TIME_STOP, "vaqt to'xtatish" },
	{ SPE_MAP_LEVEL, "xarita darajasi" },
	{ SPE_GAIN_LEVEL, "daromad darajasi" },
	{ WAN_LIGHT, "engil" },
	{ WAN_NOTHING, "hech nima" },
	{ WAN_ENLIGHTENMENT, "ma'lumotlar" },
	{ WAN_HEALING, "davolash" },
	{ WAN_LOCKING, "qulflash" },
	{ WAN_MAKE_INVISIBLE, "ko'rinmas qilmoq" },
	{ WAN_MAKE_VISIBLE, "paydo qilish" },
	{ WAN_IDENTIFY, "aniqlash" },
	{ WAN_REMOVE_CURSE, "la'natni olib tashlash" },
	{ WAN_PUNISHMENT, "jazo" },
	{ WAN_OPENING, "ochilish" },
	{ WAN_PROBING, "o'lchash" },
	{ WAN_SECRET_DOOR_DETECTION, "yashirin eshik ochish" },
	{ WAN_TRAP_DISARMING, "makr qurolsizlantirish" },
	{ WAN_ENTRAPPING, "kabarciklarinin" },
	{ WAN_STINKING_CLOUD, "titilgan bulut" },
	{ WAN_TIME_STOP, "vaqt to'xtatish" },
	{ WAN_GENOCIDE, "irqiy qirg'inchilikni" },
	{ WAN_TELE_LEVEL, "seminarlar darajasi" },
	{ WAN_TRAP_CREATION, "makr yaratish" },
	{ WAN_DARKNESS, "qorong'ilik" },
	{ WAN_MAGIC_MAPPING, "sehrli xaritalash" },
	{ WAN_DETECT_MONSTERS, "hayvonlar aniqlash" },
	{ WAN_OBJECTION, "eslagandir" },
	{ WAN_SLOW_MONSTER, "sekin hayvon" },
	{ WAN_SPEED_MONSTER, "tezlik hayvon" },
	{ WAN_HASTE_MONSTER, "shoshilish hayvon" },
	{ WAN_STRIKING, "ajoyib" },
	{ WAN_PARALYSIS, "falaj" },
	{ WAN_DISINTEGRATION, "parchalanish" },
	{ WAN_STONING, "toshbo'ron" },
	{ WAN_UNDEAD_TURNING, "bo'lmagan-o'lik burilish" },
	{ WAN_DRAINING, "drenaj" },
	{ WAN_CANCELLATION, "bekor qilish" },
	{ WAN_CREATE_MONSTER, "hayvon yaratish" },
	{ WAN_BAD_EFFECT, "yomon ta'sir" },
	{ WAN_CURSE_ITEMS, "la'nat mahsulot" },
	{ WAN_AMNESIA, "es-hushini yo'qotish" },
	{ WAN_BAD_LUCK, "yomon taqdir" },
	{ WAN_REMOVE_RESISTANCE, "qarshilik olib tashlash" },
	{ WAN_CORROSION, "chirish" },
	{ WAN_FUMBLING, "paypaslab izlay oxirgi" },
	{ WAN_STARVATION, "ochlik" },
	{ WAN_SUMMON_UNDEAD, "bo'lmagan-o'lik chaqirish" },
	{ WAN_FEAR, "qo'rqinch" },
	{ WAN_WIND, "shamol" },
	{ WAN_POLYMORPH, "bir polimorfunun" },
	{ WAN_MUTATION, "o'zgarish" },
	{ WAN_TELEPORTATION, "teleportatsiya" },
	{ WAN_BANISHMENT, "haydash" },
	{ WAN_CREATE_HORDE, "olomon yaratish" },
	{ WAN_EXTRA_HEALING, "qo'shimcha davolash" },
	{ WAN_FULL_HEALING, "to'liq shifo" },
	{ WAN_WONDER, "ajablandingizmi" },
	{ WAN_BUGGING, "adamcagiz" },
	{ WAN_WISHING, "xohlovchi" },
	{ WAN_ACQUIREMENT, "olish" },
	{ WAN_CLONE_MONSTER, "klonudur hayvon" },
	{ WAN_CHARGING, "zaryad" },
	{ WAN_DIGGING, "qazishga" },
	{ WAN_MAGIC_MISSILE, "sehrli raketa" },
	{ WAN_FIRE, "yong'in" },
	{ WAN_COLD, "sovuq" },
	{ WAN_SLEEP, "uyqu" },
	{ WAN_DEATH, "o'lim nuridir" },
	{ WAN_LIGHTNING, "chaqmoq" },
	{ WAN_FIREBALL, "yong'in to'p" },
	{ WAN_ACID, "kislota" },
	{ WAN_SOLAR_BEAM, "quyosh nur" },
	{ WAN_MANA, "sirli kuch" },
	{ WAN_GAIN_LEVEL, "daromad darajasi" },
	{ BANANA, "o'rmon meva" },
	{ RIGHT_MOUSE_BUTTON_STONE, "sichqonchaning o'ng tugmasini tosh" },
	{ DISPLAY_LOSS_STONE, "ko'rsatish ziyon tosh" },
	{ SPELL_LOSS_STONE, "zarar tosh sehr" },
	{ YELLOW_SPELL_STONE, "sariq sehr tosh" },
	{ AUTO_DESTRUCT_STONE, "avtomatik vayron tosh" },
	{ MEMORY_LOSS_STONE, "xotira yo'qotish tosh" },
	{ INVENTORY_LOSS_STONE, "inventarizatsiyadan zarar tosh" },
	{ BLACKY_STONE, "qora tosh" },
	{ MENU_BUG_STONE, "menyu xato tosh" },
	{ SPEEDBUG_STONE, "tezlik xato tosh" },
	{ SUPERSCROLLER_STONE, "katta kaydirac tosh" },
	{ FREE_HAND_BUG_STONE, "bepul qo'l xato tosh" },
	{ UNIDENTIFY_STONE, "yo'q-aniqlash toshni" },
	{ STONE_OF_THIRST, "tashnalikdan tosh" },
	{ UNLUCKY_STONE, "omadsiz tosh" },
	{ SHADES_OF_GREY_STONE, "kulrang toshdan ko'lanka" },
	{ STONE_OF_FAINTING, "bexushlik holati tosh" },
	{ STONE_OF_CURSING, "zaqqumga tosh" },
	{ STONE_OF_DIFFICULTY, "qiyinchilik tosh" },
	{ DEAFNESS_STONE, "karlik tosh" },
	{ ANTIMAGIC_STONE, "piyodalarga sehrli tosh" },
	{ WEAKNESS_STONE, "kuchsizligi tosh" },
	{ ROT_THIRTEEN_STONE, "o'n uch toshni hidlanib" },
	{ BISHOP_STONE, "episkop tosh" },
	{ CONFUSION_STONE, "tartibsizlik tosh" },
	{ DROPBUG_STONE, "xato toshni tushish" },
	{ DSTW_STONE, "toshni ishlash uchun ko'rinmaydi" },
	{ STATUS_STONE, "holat tosh" },
	{ ALIGNMENT_STONE, "hizalama tosh" },
	{ STAIRSTRAP_STONE, "narvon makr tosh" },
	{ UNINFORMATION_STONE, "axborot tosh etishmasligi" },
	{ WAN_CREATE_FAMILIAR, "tanish yaratish" },
	{ SCR_CREATE_FAMILIAR, "tanish yaratish" },
	{ RIN_INTRINSIC_LOSS, "ichki zarar" },
	{ RIN_TRAP_REVEALING, "makr oshkor" },
	{ RIN_BLOOD_LOSS, "qon yo'qotish" },
	{ RIN_NASTINESS, "iflos ishlar" },
	{ RIN_BAD_EFFECT, "yomon ta'sir" },
	{ AMULET_OF_RMB_LOSS, "amulet of sichqonchaning o'ng tugmasini yo'qotish" },
	{ HELM_OF_OBSCURED_DISPLAY, "maxfiy ekranning rul" },
	{ CLOAK_OF_UNSPELLING, "hech qanday yozilishini plash" },
	{ YELLOW_SPELL_CLOAK, "sariq sehr plash" },
	{ AUTODESTRUCT_DE_VICE_BOOTS, "avtomatik vayron qurilma" },
	{ OUT_OF_MEMORY_HELMET, "xotira dubulg'asiga amalga" },
	{ CLOAK_OF_INVENTORYLESSNESS, "no inventarizatsiyadan plash" },
	{ BLACKY_HELMET, "qora zarbdan" },
	{ MENU_NOSE_GLOVES, "menyu burun qo'lqop" },
	{ SPEEDBUG_BOOTS, "tezlik xato chizilmasin" },
	{ RIN_SUPERSCROLLING, "katta o'tish" },
	{ GAUNTLETS_OF_TRAP_CREATION, "makr yaratish sportchisi" },
	{ SENTIENT_HIGH_HEELED_SHOES, "ongli yuqori poshnalar" },
	{ VULNERABILITY_CLOAK, "zaiflik plash" },
	{ UNWIELDY_GLOVES, "katta sportchisi" },
	{ HELM_OF_LOSE_IDENTIFICATION, "yo'qotish aniqlash rul" },
	{ HELM_OF_THIRST, "tashnalikdan elkan" },
	{ CLOAK_OF_LUCK_NEGATION, "omad yadsinmaya plash" },
	{ SADO_MASO_GLOVES, "berahm qo'lqop" },
	{ AMULET_OF_ITEM_TELEPORTATION, "amulet of xabar xavfli ekanini" },
	{ BOOTS_OF_FAINTING, "bexushlik holati chizilmasin" },
	{ DIFFICULT_BOOTS, "qiyin chizilmasin" },
	{ SOUNDPROOF_HELMET, "tovush o'tkazmaydigan zarbdan" },
	{ ANTI_CASTER_CLOAK, "piyodalarga sariyog 'plash" },
	{ BOOTS_OF_WEAKNESS, "zaiflik chizilmasin" },
	{ CYPHER_HELM, "shifr elkan" },
	{ GRIDBUG_CONDUCT_BOOTS, "panjara xato boshlovchi qo'lqop" },
	{ CONFUSING_GLOVES, "chalkash qo'lqop" },
	{ UNDROPPABLE_GLOVES, "to'xtashi mumkin emas qo'lqop" },
	{ ANTI_DRINKER_HELMET, "piyodalarga chekayotgan zarbdan" },
	{ HEAVY_STATUS_CLOAK, "og'ir holat plash" },
	{ HELM_OF_BAD_ALIGNMENT, "yomon muvofiq dubulg'a" },
	{ STAIRWELL_STOMPING_BOOTS, "shirin oyoq osti chizilmasin" },
	{ GAUNTLETS_OF_MISSING_INFORMATI, "etishmayotgan ma'lumotlar sportchisi" },
	{ WHISPERING_HELMET, "pichirlardi zarbdan" },

	{ STONE_OF_INTRINSIC_LOSS, "ichki zarar tosh" },
	{ BLOOD_LOSS_STONE, "qon yo'qotish tosh" },
	{ BAD_EFFECT_STONE, "yomon ta'sir tosh" },
	{ TRAP_CREATION_STONE, "makr yaratish tosh" },
	{ STONE_OF_VULNERABILITY, "zaiflik tosh" },
	{ ITEM_TELEPORTING_STONE, "xabar xavfli ekanini tosh" },
	{ NASTY_STONE, "iflos ishlar tosh" },

	{ RIN_INFRAVISION, "infraqizil ko'rish" },
	{ SCR_PHASE_DOOR, "bosqich eshik" },
	{ SCR_RANDOM_ENCHANTMENT, "tasodifiy sehr" },
	{ WAN_INCREASE_MAX_HITPOINTS, "sog'liqni saqlash jadallashtirishidir" },
	{ WAN_REDUCE_MAX_HITPOINTS, "sog'liqni saqlash qisqartirish" },
	{ SCALE_MAIL, "fermer pochta" },
	{ ACID_VENOM, "kislota zahar" },
	{ ARROW, "strelka" },
	{ GAUNTLETS_OF_SWIMMING, "suzish sportchisi" },
	{ DWARVISH_MITHRIL_COAT, "mitti engil palto" },
	{ ELVEN_MITHRIL_COAT, "jinlaridan engil palto" },
	{ STUDDED_LEATHER_ARMOR, "shitirlashi charm zirh" },
	{ MAGIC_WHISTLE, "sehrli hushtak" },
	{ TIN_WHISTLE, "kalay hushtak" },
	{ JUMPING_BOOTS, "chizilmasin o'tish" },
	{ HELM_OF_BRILLIANCE, "ajoyib zarbdan" },
	{ OILSKIN_SACK, "yupqa kleyonka xaltadan" },
	{ BAG_OF_DIGESTION, "hazm yukxalta" },
	{ DIODE, "yarimo'tkazgich" },
	{ TRANSISTOR, "tranzistorlar" },
	{ IC, "integratsiyalashgan elektron" },
	{ BOTTLE, "butilka" },
	{ ICE_BOX, "muz qutisi" },
	{ SKELETON_KEY, "skelet asosiy" },
	{ ELVEN_ARROW, "jinlaridan o'q" },
	{ AKLYS, "tasma klubi" },
	{ YUMI, "yaponiya kamon oralig'idek" },
	{ YA, "yaponiya o'q" },

	{ ORCISH_DAGGER, "yomon xanjar" },
	{ DROVEN_DAGGER, "shisha xanjar" },
	{ ATHAME, "marosim xanjar" },
	{ ELVEN_DAGGER, "erda alf xanjar" },
	{ DARK_ELVEN_DAGGER, "qorong'u erda alf xanjar" },
	{ GREAT_DAGGER, "katta xanjar" },
	{ TOOTH_OF_AN_ALGOLIAN_SUNTIGER, "juda o'tkir tish" },
	{ ORCISH_SHORT_SWORD, "yomon qisqa qilich" },
	{ SILVER_SHORT_SWORD, "kumush qisqa qilich" },
	{ DWARVISH_SHORT_SWORD, "mitti qisqa qilich" },
	{ ELVEN_SHORT_SWORD, "erda alf qisqa qilich" },
	{ DROVEN_SHORT_SWORD, "shisha qisqa qilich" },
	{ DARK_ELVEN_SHORT_SWORD, "qorong'u erda alf qisqa qilich" },
	{ ELVEN_BROADSWORD, "erda alf keng dudama qilich" },
	{ SILVER_LONG_SWORD, "kumush uzoq qilich" },
	{ DROVEN_GREATSWORD, "shisha avtomobil" },
	{ BENT_SABLE, "egilgan samur" },
	{ RAPIER, "mec" },
	{ AKLYS, "tasma klubi" },
	{ METAL_CLUB, "metall klubi" },
	{ REINFORCED_MACE, "dori-moja" },
	{ KNOUT, "qamchi qimirlatmoq" },
	{ OBSID, "ofat qimirlatmoq" },
	{ HEAVY_HAMMER, "og'ir bolg'acha" },
	{ ORCISH_SPEAR, "yomon nayza" },
	{ DROVEN_SPEAR, "shisha nayza" },
	{ DWARVISH_SPEAR, "mitti nayza" },
	{ ELVEN_SPEAR, "erda alf nayza" },
	{ COURSE_LANCE, "albatta lens" },
	{ FORCE_PIKE, "kuch payk bilan" },
	{ DROVEN_LANCE, "shisha lens" },
	{ ORCISH_BOW, "yomon yoy" },
	{ ELVEN_BOW, "erda alf yoy" },
	{ DARK_ELVEN_BOW, "qorong'u erda alf yoy" },
	{ DROVEN_BOW, "shisha yoy" },
	{ ORCISH_ARROW, "yomon o'q" },
	{ SILVER_ARROW, "kumush o'q" },
	{ ELVEN_ARROW, "erda alf o'q" },
	{ DROVEN_ARROW, "shisha o'q" },
	{ DARK_ELVEN_ARROW, "qorong'u erda alf o'q" },
	{ CATAPULT, "manjaniq" },
	{ SILVER_BULLET, "kumush modda belgisi" },
	{ GAS_GRENADE, "gaz bombasi" },
	{ STICK_OF_DYNAMITE, "dinamit tayoq" },
	{ DROVEN_CROSSBOW, "shisha yayli miltiq" },
	{ DEMON_CROSSBOW, "shayton yayli miltiq" },
	{ DROVEN_BOLT, "shisha murvat" },
	{ BOOMERANG, "bumerang" },
	{ HAWAIIAN_SHIRT, "gavaycha ko'ylak" },
	{ DROVEN_PLATE_MAIL, "shisha plastinka pochta" },
	{ BANDED_MAIL, "hoshiyali pochta" },
	{ DARK_ELVEN_MITHRIL_COAT, "quyuq jinlaridan engil palto" },
	{ GNOMISH_SUIT, "mitti odam kostyum" },
	{ CHAIN_MAIL, "zanjir pochta" },
	{ DROVEN_CHAIN_MAIL, "shisha zanjir pochta" },
	{ ORCISH_CHAIN_MAIL, "yomon zanjir pochta" },
	{ RING_MAIL, "rishtasi pochta" },
	{ ORCISH_RING_MAIL, "yomon rishtasi pochta" },
	{ LEATHER_ARMOR, "charm zirh" },
	{ ROBE_OF_PROTECTION, "himoya libos" },
	{ ROBE_OF_POWER, "hokimiyat libos" },
	{ ROBE_OF_WEAKNESS, "zaiflik libos" },
	{ GRAY_DRAGON_SCALE_MAIL, "kul ajdaho ko'lamli pochta" },
	{ SILVER_DRAGON_SCALE_MAIL, "kumush ajdaho ko'lamli pochta" },
	{ MERCURIAL_DRAGON_SCALE_MAIL, "simob ajdaho ko'lamli pochta" },
	{ SHIMMERING_DRAGON_SCALE_MAIL, "jimirlagan ajdaho ko'lamli pochta" },
	{ DEEP_DRAGON_SCALE_MAIL, "chuqur ajdaho ko'lamli pochta" },
	{ RED_DRAGON_SCALE_MAIL, "qizil ajdaho ko'lamli pochta" },
	{ WHITE_DRAGON_SCALE_MAIL, "oq ajdar ko'lamli pochta" },
	{ ORANGE_DRAGON_SCALE_MAIL, "apelsin ajdaho ko'lamli pochta" },
	{ BLACK_DRAGON_SCALE_MAIL, "qora ajdaho ko'lamli pochta" },
	{ BLUE_DRAGON_SCALE_MAIL, "ko'k ajdaho ko'lamli pochta" },
	{ GREEN_DRAGON_SCALE_MAIL, "yashil ajdaho ko'lamli pochta" },
	{ GOLDEN_DRAGON_SCALE_MAIL, "oltin ajdaho ko'lamli pochta" },
	{ STONE_DRAGON_SCALE_MAIL, "tosh ajdaho ko'lamli pochta" },
	{ CYAN_DRAGON_SCALE_MAIL, "osmon ajdaho ko'lamli pochta" },
	{ YELLOW_DRAGON_SCALE_MAIL, "sariq ajdaho ko'lamli pochta" },
	{ GRAY_DRAGON_SCALES, "kulrang ajdaho farqlari" },
	{ SILVER_DRAGON_SCALES, "kumush ajdaho farqlari" },
	{ MERCURIAL_DRAGON_SCALES, "simob ajdaho farqlari" },
	{ SHIMMERING_DRAGON_SCALES, "jimirlagan ajdaho farqlari" },
	{ DEEP_DRAGON_SCALES, "chuqur ajdaho farqlari" },
	{ RED_DRAGON_SCALES, "qizil ajdaho farqlari" },
	{ WHITE_DRAGON_SCALES, "oq ajdar farqlari" },
	{ ORANGE_DRAGON_SCALES, "apelsin ajdaho farqlari" },
	{ BLACK_DRAGON_SCALES, "qora ajdaho farqlari" },
	{ BLUE_DRAGON_SCALES, "ko'k ajdaho farqlari" },
	{ GREEN_DRAGON_SCALES, "yashil ajdaho farqlari" },
	{ GOLDEN_DRAGON_SCALES, "oltin ajdaho farqlari" },
	{ STONE_DRAGON_SCALES, "tosh ajdaho farqlari" },
	{ CYAN_DRAGON_SCALES, "osmon ajdaho farqlari" },
	{ YELLOW_DRAGON_SCALES, "sariq ajdaho farqlari" },
	{ ORCISH_CLOAK, "yomon plash" },
	{ DWARVISH_CLOAK, "mitti plash" },
	{ OILSKIN_CLOAK, "yupqa kleyonka plash" },
	{ ELVEN_CLOAK, "erda alf plash" },
	{ DROVEN_CLOAK, "shisha plash" },
	{ PLASTEEL_CLOAK, "plastik plash" },
	{ CLOAK_OF_PROTECTION, "himoya plash" },
	{ CLOAK_OF_DEATH, "o'lim plash" },
	{ CLOAK_OF_INVISIBILITY, "goeruenmezlik plash" },
	{ CLOAK_OF_FUMBLING, "paypaslab izlay oxirgi of plash" },
	{ CLOAK_OF_MAGIC_RESISTANCE, "sehrli qarshilik plash" },
	{ CLOAK_OF_DRAIN_RESISTANCE, "drenaj qarshilik plash" },
	{ CLOAK_OF_REFLECTION, "aks ettirish plash" },
	{ MANACLOAK, "sehr energiya plash" },
	{ CLOAK_OF_CONFUSION, "tartibsizlik plash" },
	{ CLOAK_OF_WARMTH, "harorat plash" },
	{ CLOAK_OF_GROUNDING, "yerga of plash" },
	{ CLOAK_OF_QUENCHING, "qaytarmaslik plash" },
	{ CLOAK_OF_RESPAWNING, "yumurtlama of plash" },
	{ SPAWN_CLOAK, "avlod plash" },
	{ ADOM_CLOAK, "sir rido kiygan qadimiy domenlari" },
	{ EGOIST_CLOAK, "xudbin odam plash" },
	{ CLOAK_OF_TIME, "vaqt plash" },
	{ CLOAK_OF_DISPLACEMENT, "joyidan plash" },
	{ ELVEN_LEATHER_HELM, "erda alf charm rul" },
	{ GNOMISH_HELM, "mitti odam rul" },
	{ ORCISH_HELM, "yomon rul" },
	{ DWARVISH_IRON_HELM, "mitti temir rul" },
	{ DROVEN_HELM, "shisha rul" },
	{ CORNUTHAUM, "sehrgar rul" },
	{ PLASTEEL_HELM, "plastik rul" },
	{ HELM_OF_DETECT_MONSTERS, "hayvonlar aniqlash bo'yicha rul" },
	{ HELM_OF_STORMS, "bo'ron rul" },
	{ HELM_OF_STEEL, "po'lat rul" },
	{ HELM_OF_DRAIN_RESISTANCE, "drenaj qarshilik rul" },
	{ HELM_OF_FEAR, "qo'rquv rul" },
	{ HELM_OF_HUNGER, "ochlik rul" },
	{ HELM_OF_DISCOVERY, "kashfiyot rul" },
	{ FIRE_HELMET, "yong'in zarbdan" },
	{ ANGER_HELM, "g'azab rul" },
	{ CAPTCHA_HELM, "to'liq avtomatlashtirilgan davlat turing rul" },
	{ HELM_OF_AMNESIA, "amneziya of rul" },
	{ HELM_OF_SENSORY_DEPRIVATION, "hissiyotdan mahrum qilish rul" },
	{ BIGSCRIPT_HELM, "katta buyruq fayli rul" },
	{ QUIZ_HELM, "viktorinasi rul" },
	{ DIZZY_HELMET, "bosh aylanishi zarbdan" },
	{ MUTING_HELM, "bostirish rul" },
	{ ULCH_HELMET, "yomon oziq-ovqat zarbdan" },
	{ HELM_OF_TELEPATHY, "shahid of rul" },
	{ PLASTEEL_GLOVES, "plastik qo'lqop" },
	{ GAUNTLETS_OF_PANIC, "vahima qalin qo'lqop" },
	{ GAUNTLETS_OF_SLOWING, "pastlab qalin qo'lqop" },
	{ OILSKIN_GLOVES, "yupqa kleyonka qo'lqop" },
	{ GAUNTLETS_OF_TYPING, "yozib qalin qo'lqop" },
	{ GAUNTLETS_OF_STEEL, "po'lat qalin qo'lqop" },
	{ GAUNTLETS_OF_REFLECTION, "aks ettirish qalin qo'lqop" },
	{ GAUNTLETS_OF_THE_FORCE, "kuch qalin qo'lqop" },
	{ BANKING_GLOVES, "bankchilik qo'lqop" },
	{ DIFFICULT_GLOVES, "qiyin qo'lqop" },
	{ CHAOS_GLOVES, "betartiblik qo'lqop" },
	{ GAUNTLETS_OF_DEXTERITY, "al qobiliyati qalin qo'lqop" },
	{ ELVEN_SHIELD, "erda alf qalqon" },
	{ URUK_HAI_SHIELD, "xayol qalqon" },
	{ ORCISH_SHIELD, "yomon qalqon" },
	{ LARGE_SHIELD, "qalqoni" },
	{ DWARVISH_ROUNDSHIELD, "mitti dumaloq qalqoni" },
	{ FLAME_SHIELD, "olov qalqon" },
	{ ICE_SHIELD, "muz qalqoni" },
	{ VENOM_SHIELD, "achchiq qalqon" },
	{ LIGHTNING_SHIELD, "chaqmoq qalqon" },
	{ SHIELD_OF_LIGHT, "nur qalqon" },
	{ GRAY_DRAGON_SCALE_SHIELD, "kulrang ajdaho yashirish qalqon" },
	{ SILVER_DRAGON_SCALE_SHIELD, "kumush ajdaho yashirish qalqon" },
	{ MERCURIAL_DRAGON_SCALE_SHIELD, "simob ajdaho yashirish qalqon" },
	{ SHIMMERING_DRAGON_SCALE_SHIELD, "jimirlagan ajdaho yashirish qalqon" },
	{ DEEP_DRAGON_SCALE_SHIELD, "chuqur ajdaho yashirish qalqon" },
	{ RED_DRAGON_SCALE_SHIELD, "qizil ajdaho yashirish qalqon" },
	{ WHITE_DRAGON_SCALE_SHIELD, "oq ajdar yashirish qalqon" },
	{ ORANGE_DRAGON_SCALE_SHIELD, "apelsin ajdaho yashirish qalqon" },
	{ BLACK_DRAGON_SCALE_SHIELD, "qora ajdaho yashirish qalqon" },
	{ BLUE_DRAGON_SCALE_SHIELD, "ko'k ajdaho yashirish qalqon" },
	{ GREEN_DRAGON_SCALE_SHIELD, "yashil ajdaho yashirish qalqon" },
	{ GOLDEN_DRAGON_SCALE_SHIELD, "oltin ajdaho yashirish qalqon" },
	{ STONE_DRAGON_SCALE_SHIELD, "tosh ajdaho yashirish qalqon" },
	{ CYAN_DRAGON_SCALE_SHIELD, "osmon ajdaho yashirish qalqon" },
	{ YELLOW_DRAGON_SCALE_SHIELD, "sariq ajdaho yashirish qalqon" },
	{ PLASTEEL_BOOTS, "plastik chizilmasin" },
	{ LOW_BOOTS, "past chizilmasin" },
	{ IRON_SHOES, "temir poyafzali" },
	{ GNOMISH_BOOTS, "mitti odam chizilmasin" },
	{ HIGH_BOOTS, "yuqori chizilmasin" },
	{ SPEED_BOOTS, "tezlik chizilmasin" },
	{ BOOTS_OF_MOLASSES, "pekmez chizilmasin" },
	{ FLYING_BOOTS, "uchib chizilmasin" },
	{ ELVEN_BOOTS, "erda alf chizilmasin" },
	{ KICKING_BOOTS, "botlarni tepib" },
	{ DISENCHANTING_BOOTS, "ko'ngilni qoldirmoq chizilmasin" },
	{ LEVITATION_BOOTS, "botlarni suzuvchi" },
	{ RIN_DRAIN_RESISTANCE, "qarshilik drenaj" },
	{ RIN_MEMORY, "xotira" },
	{ RIN_FAST_METABOLISM, "tez almashinuvining" },
	{ RIN_DISENGRAVING, "zarbxona olib tashlash" },
	{ RIN_NO_SKILL, "yo'q san'at" },
	{ RIN_LOW_STATS, "past statistikasini" },
	{ RIN_FAILED_TRAINING, "muvaffaqiyatsiz ta'lim" },
	{ RIN_FAILED_EXERCISE, "muvaffaqiyatsiz mashqlar" },
	{ AMULET_OF_POLYMORPH, "amulet of polimorf" },
	{ AMULET_OF_PREMATURE_DEATH, "amulet of bevaqt o'lim" },
	{ AMULET_OF_ANTI_TELEPORTATION, "amulet of teleportatsiya oldini olish" },
	{ AMULET_OF_EXPLOSION, "amulet of portlash" },
	{ AMULET_OF_WRONG_SEEING, "amulet of noto'g'ri ko'ruvchi" },
	{ AMULET_OF_YENDOR, "amulet of yulduz samolyotlar" },
	{ FAKE_AMULET_OF_YENDOR, "qalbaki narsa amulet of yulduz samolyotlar" },
	{ CREDIT_CARD, "kredit kartasi" },
	{ MAGIC_CANDLE, "sehrli sham" },
	{ OIL_LAMP, "neft chiroq" },
	{ MAGIC_LAMP, "sehrli chiroq" },
	{ MAGIC_FLUTE, "sehr nay" },
	{ TOOLED_HORN, "shakl shox" },
	{ FIRE_HORN, "yong'in shox" },
	{ FROST_HORN, "erix shox" },
	{ HORN_OF_PLENTY, "ko'p shox" },
	{ MAGIC_HARP, "sehr harb" },
	{ BELL, "qo'ng'iroq" },
	{ LEATHER_DRUM, "charm davul" },
	{ DRUM_OF_EARTHQUAKE, "zilzila davul" },
	{ FISHING_POLE, "baliqchilar cgangagi" },
	{ GREEN_LIGHTSABER, "yashil nur" },
	{ BLUE_LIGHTSABER, "ko'k nur" },
	{ RED_LIGHTSABER, "qizil nur" },
	{ RED_DOUBLE_LIGHTSABER, "qizil ikki kishilik nur" },
	{ FELT_TIP_MARKER, "his-uchi isaretleyici" },
	{ HITCHHIKER_S_GUIDE_TO_THE_GALA, "galaktika otostopcu qo'llanma" },
	{ PACK_OF_FLOPPIES, "floppi to'plami" },
	{ GOD_O_METER, "Alloh-o-metr" },
	{ RELAY, "o'z o'rni" },
	{ CHEMISTRY_SET, "kimyo majmui" },
	{ CANDELABRUM_OF_INVOCATION, "duosidan qandil" },
	{ BELL_OF_OPENING, "ochilish qo'ng'iroq" },
	{ MEATBALL, "koefte" },
	{ MEAT_STICK, "go'sht tayoq" },
	{ HUGE_CHUNK_OF_MEAT, "go'sht ulkan bo'lak" },
	{ MEAT_RING, "go'sht rishtasi" },
	{ EYEBALL, "ko'z soqqasi" },
	{ SEVERED_HAND, "o'pirilgan qo'l" },
	{ PEANUT_BAG, "eman yukxalta" },
	{ TORTILLA, "un tez" },
	{ UGH_MEMORY_TO_CREATE_INVENTORY, "rli xotira inventarizatsiyadan oynasini yaratish" },
	{ TWELVE_COURSE_DINNER, "o'n ikki kurs kechki ovqat" },
	{ PILL, "dori" },
	{ K_RATION, "jahon urushi ikki ulush" },
	{ C_RATION, "Amerika Qo'shma Shtatlari harbiy ratsion" },
	{ POT_FIRE_RESISTANCE, "yong'in qarshilik" },
	{ SCR_CHAOS_TERRAIN, "betartiblik joy" },
	{ SCR_REPAIR_ITEM, "ta'mirlash xabar" },
	{ SCR_SUMMON_BOSS, "xo'jayin chaqirish" },
	{ SCR_WOUNDS, "yaralar" },
	{ SCR_BULLSHIT, "bema'nilik" },
	{ SCR_ITEM_GENOCIDE, "xabar genotsid" },
	{ SCR_NASTINESS, "iflos ishlar" },
	{ SCR_ELEMENTALISM, "asosiy tadqiqotlar" },
	{ SCR_DEMONOLOGY, "shayton tadqiqotlar" },
	{ SCR_GIRLINESS, "ayol bo'lish" },
	{ SPE_AMNESIA, "xotira halok" },
	{ SPE_REPAIR_WEAPON, "ta'mirlash qurol" },
	{ SPE_BOOK_OF_THE_DEAD, "o'lganlarning kitob" },
	{ WAN_CONFUSION, "xijolat tortish" },
	{ WAN_SLIMING, "shilimshiq yuzlanib" },
	{ WAN_LYCANTHROPY, "bo'ri-insoniyat" },
	{ WAN_SUMMON_SEXY_GIRL, "amaliyotni qiz chaqirish" },
	{ GOLD_PIECE, "oltin qism" },
	{ WONDER_STONE, "ajablanarli tosh" },
	{ LUCKSTONE, "omad tosh" },
	{ HEALTHSTONE, "sog'liqni saqlash tosh" },
	{ MANASTONE, "sehr energiya tosh" },
	{ STONE_OF_MAGIC_RESISTANCE, "sehrli qarshilik tosh" },
	{ SLEEPSTONE, "uyqu tosh" },
	{ TOUCHSTONE, "toshi" },
	{ FARLOOK_STONE, "uzoq qarash tosh" },
	{ RESPAWN_STONE, "qayta chiqish tosh" },
	{ CAPTCHA_STONE, "davlat sinov tosh" },
	{ AMNESIA_STONE, "xotira halok tosh" },
	{ BIGSCRIPT_STONE, "katta buyruq fayli tosh" },
	{ BANK_STONE, "bankchilik tosh" },
	{ MAP_STONE, "xarita tosh" },
	{ TECHNIQUE_STONE, "texnikasi tosh" },
	{ DISENCHANTMENT_STONE, "sehr olishdan tosh" },
	{ VERISIERT_STONE, "dalangan tosh" },
	{ CHAOS_TERRAIN_STONE, "betartiblik joy tosh" },
	{ MUTENESS_STONE, "soqov tosh" },
	{ ENGRAVING_STONE, "zarbxona tosh" },
	{ MAGIC_DEVICE_STONE, "sehrli qurilma tosh" },
	{ BOOK_STONE, "kitob tosh" },
	{ LEVEL_STONE, "darajasi tosh" },
	{ QUIZ_STONE, "viktorinasi tosh" },
	{ METABOLIC_STONE, "metabolik tosh" },
	{ STONE_OF_NO_RETURN, "yo'q qaytish tosh" },
	{ EGOSTONE, "benlik tosh" },
	{ FAST_FORWARD_STONE, "tez oldinga tosh" },
	{ ROTTEN_STONE, "chirigan tosh" },
	{ UNSKILLED_STONE, "malakasiz tosh" },
	{ LOW_STAT_STONE, "past Fernando tosh" },
	{ TRAINING_STONE, "ta'lim tosh" },
	{ EXERCISE_STONE, "mashqlar tosh" },
	{ HEAVY_STONE_BALL, "og'ir tosh to'p" },
	{ HEAVY_GLASS_BALL, "og'ir shisha to'p" },
	{ QUITE_HEAVY_IRON_BALL, "to'liq og'ir temir to'p" },
	{ HEAVY_GOLD_BALL, "og'ir oltin to'p" },
	{ HEAVY_CLAY_BALL, "og'ir tuproq to'p" },
	{ REALLY_HEAVY_IRON_BALL, "albatta, og'ir temir to'p" },
	{ HEAVY_ELYSIUM_BALL, "og'ir eliziy to'p" },
	{ HEAVY_GRANITE_BALL, "og'ir granit to'p" },
	{ EXTREMELY_HEAVY_IRON_BALL, "nihoyatda og'ir temir to'p" },
	{ HEAVY_CONUNDRUM_BALL, "og'ir muammo to'p" },
	{ HEAVY_CONCRETE_BALL, "og'ir aniq to'p" },
	{ IMPOSSIBLY_HEAVY_IRON_BALL, "imkonsiz og'ir temir to'p" },
	{ IMPOSSIBLY_HEAVY_GLASS_BALL, "imkonsiz og'ir shisha to'p" },
	{ IMPOSSIBLY_HEAVY_MINERAL_BALL, "imkonsiz og'ir ma'dan to'p" },
	{ IMPOSSIBLY_HEAVY_ELYSIUM_BALL, "imkonsiz og'ir eliziy to'p" },
	{ STONE_CHAIN, "tosh zanjir" },
	{ GLASS_CHAIN, "shisha zanjir" },
	{ GOLD_CHAIN, "oltin zanjir" },
	{ CLAY_CHAIN, "tuproq zanjir" },
	{ SCOURGE, "ofat" },
	{ ELYSIUM_SCOURGE, "eliziy ofat" },
	{ GRANITE_SCOURGE, "granit ofat" },
	{ NUNCHIAKU, "ninjasi zanjir" },
	{ CONUNDRUM_NUNCHIAKU, "muammo ninjasi zanjir" },
	{ CONCRETE_NUNCHIAKU, "aniq ninjasi zanjir" },
	{ HOSTAGE_CHAIN, "garovga zanjir" },
	{ GLASS_HOSTAGE_CHAIN, "shisha garovga zanjir" },
	{ MINERAL_HOSTAGE_CHAIN, "ma'dan garovga zanjir" },
	{ ELYSIUM_HOSTAGE_CHAIN, "eliziy garovga zanjir" },
	{ BLINDING_VENOM, "ko'r zahar" },

	{ GOLDEN_ARROW, "oltin o'q" },
	{ ANCIENT_ARROW, "qadimgi o'q" },
	{ SPIKE, "boshoq" },
	{ SICKLE, "o'roqsimon" },
	{ ELVEN_SICKLE, "erda alf o'roq" },
	{ SCYTHE, "bog 'chalg'i" },
	{ MOON_AXE, "oy bolta" },
	{ VIBROBLADE, "tebranish pichoq" },
	{ CRYSTAL_SWORD, "billur qilich" },
	{ ELVEN_LANCE, "erda alf lens" },
	{ ELVEN_MACE, "erda alf moja" },
	{ SILVER_KHAKKHARA, "to'rt xodimlari tomonidan bo'lingan kumush bir" },
	{ IRON_BAR, "temir bo'lagi" },
	{ FLINTLOCK, "cakmakli" },
	{ BFG, "katta iblisona kun" },
	{ HAND_BLASTER, "qo'l imkon" },
	{ ARM_BLASTER, "osongina imkon" },
	{ CUTTING_LASER, "chiqib ketish lazer" },
	{ RAYGUN, "nuridir kun" },
	{ BLASTER_BOLT, "imkon murvat" },
	{ HEAVY_BLASTER_BOLT, "og'ir imkon murvat" },
	{ LASER_BEAM, "lazer nur" },
	{ BFG_AMMO, "katta iblisona kun o'q" },
	{ SEDGE_HAT, "qamish xet" },
	{ SKULLCAP, "do'ppi" },
	{ FLACK_HELMET, "zenit artilleriyasi zarbdan" },
	{ CRYSTAL_HELM, "billur rul" },
	{ ELVEN_TOGA, "erda alf ustki" },
	{ NOBLE_S_DRESS, "ezgu kiyim" },
	{ CONSORT_S_SUIT, "er kostyum" },
	{ FORCE_ARMOR, "kuch zirh" },
	{ HEALER_UNIFORM, "davolovchi bir xil" },
	{ JUMPSUIT, "ueniforma" },
	{ BLACK_DRESS, "qora libos" },
	{ BODYGLOVE, "tana qo'lqop" },
	{ ALCHEMY_SMOCK, "simya maxsus kiyim" },
	{ LEO_NEMAEUS_HIDE, "qudratli yo'lbars plash" },
	{ CRYSTAL_SHIELD, "kristalli qalqoni" },
	{ ORIHALCYON_GAUNTLETS, "nodir ma'dan qo'lqop" },
	{ CRYSTAL_BOOTS, "billur chizilmasin" },
	{ RIN_ALACRITY, "tayyor turish" },
	{ AMULET_VERSUS_CURSES, "la'nati oldini olish" },
	{ SCR_WARD, "palata" },
	{ SCR_WARDING, "saqlanish" },
	{ MAGICITE_CRYSTAL, "yakuniy hayoliy billur" },
	{ SMALL_PIECE_OF_UNREFINED_MITHR, "qayta nodir metall kichik bo'lak" },
	{ SILVER_SLINGSTONE, "kumush ketgan tosh" },

	{ KOKKEN, "sharqiy yayli miltiq murvat" },
	{ NEEDLE, "nina" },
	{ CALTROP, "sim to'siq" },
	{ BRONZE_SPEAR, "bronza nayza" },
	{ FLINT_SPEAR, "zajigalka nayza" },
	{ LONG_STAKE, "uzoq ustun" },
	{ BAMBOO_SPEAR, "bambuk nayza" },
	{ TWO_HANDED_TRIDENT, "ikki qo'l sacayagimiz" },
	{ MERCURIAL_ATHAME, "simob marosim xanjar" },
	{ SURVIVAL_KNIFE, "qoldiq pichoq" },
	{ OBSIDIAN_AXE, "teaneck bolta" },
	{ DWARVISH_BATTLE_AXE, "mitti urush bolta" },
	{ IRON_SABER, "temir qilich" },
	{ BLACK_AESTIVALIS, "qora yaponiya keng dudama qilich" },
	{ WHITE_FLOWER_SWORD, "oq gul qilich" },
	{ SUGUHANOKEN, "g'alati yaponiya uzoq qilich" },
	{ GREAT_HOUCHOU, "uzoq qilich missiyalar-nomidagi" },
	{ CHAINSWORD, "zanjir qilich" },
	{ BASTERD_SWORD, "pic qilich" },
	{ PITCHFORK, "panshaxa" },
	{ BLACK_HALBERD, "qora oybolta" },
	{ BRONZE_MORNING_STAR, "bronza ertalab yulduz" },
	{ SPINED_BALL, "orqa miya to'p" },
	{ SLEDGE_HAMMER, "chana bolg'acha" },
	{ BONE_CLUB, "suyak klubi" },
	{ SPIKED_CLUB, "boshoqli klubi" },
	{ HUGE_CLUB, "katta klub" },
	{ LOG, "kirish" },
	{ RUNED_ROD, "yodimga tushyapti tayog'i" },
	{ STAR_ROD, "yulduz tayog'i" },
	{ FIRE_HOOK, "yong'in tuzoq" },
	{ PLATINUM_FIRE_HOOK, "platina yong'in tuzoq" },
	{ CHAIN_AND_SICKLE, "zanjir va o'roqsimon" },
	{ TWO_HANDED_FLAIL, "ikki qo'l qimirlatmoq" },
	{ CHAINWHIP, "zanjir qamchi" },
	{ MITHRIL_WHIP, "nodir metall qamchi" },
	{ FLAME_WHIP, "olov qamchi" },
	{ ROSE_WHIP, "gul qamchi" },
	{ BROOM, "supurgi" },
	{ MOP, "polyuvg'ich" },
	{ SPECIAL_MOP, "maxsus polyuvg'ich" },
	{ BOAT_OAR, "qayiq eshkak" },
	{ MAGICAL_PAINTBRUSH, "sehrli mo'yqalam" },
	{ FUTON_SWATTER, "yaponiya konining delicisi" },
	{ CARDBOARD_FAN, "karton muxlisi sifatida ro'yxatdan" },
	{ OTAMA, "yaponiya ketma" },
	{ PARASOL, "soyabon" },
	{ UMBRELLA, "uyingizda" },
	{ PILE_BUNKER, "hav yonilg'i" },
	{ NURSE_CAP, "hamshira idish" },
	{ KATYUSHA, "bir qiz nomidagi idish" },
	{ BUNNY_EAR, "quyon quloq" },
	{ DRAGON_HORNED_HEADPIECE, "ajdaho shoxli ustki qismi" },
	{ STRAW_HAT, "somon shlyapa" },
	{ SPEEDWAGON_S_HAT, "tezlik vagon xet" },
	{ MECHA_IRAZU, "noma'lum yaponiya zarbdan" },
	{ SCHOOL_CAP, "maktab idish" },
	{ CROWN, "toj" },
	{ ANTENNA, "antennalarga ega" },
	{ CHAIN_COIF, "zanjir lasan" },
	{ COLOR_CONE, "rang konusning" },
	{ MINING_HELM, "kon rul" },
	{ FIELD_HELM, "dala rul" },
	{ HELM_OF_SAFEGUARD, "himoya qilish rul" },
	{ HELM_OF_CHAOTIC, "xaotik of rul" },
	{ HELM_OF_NEUTRAL, "neytral of rul" },
	{ HELM_OF_LAWFUL, "qonuniy of rul" },
	{ HELM_OF_UNDERWATER_ACTION, "suv osti harakat rul" },
	{ HELM_OF_JAMMING, "qisib qoladigan rul" },
	{ RAINBOW_DRAGON_SCALE_MAIL, "kamalak ajdaho ko'lamli pochta" },
	{ BLOOD_DRAGON_SCALE_MAIL, "qon ajdaho ko'lamli pochta" },
	{ PLAIN_DRAGON_SCALE_MAIL, "tekis ajdaho ko'lamli pochta" },
	{ SKY_DRAGON_SCALE_MAIL, "havo ajdaho ko'lamli pochta" },
	{ WATER_DRAGON_SCALE_MAIL, "suv ajdaho ko'lamli pochta" },
	{ RAINBOW_DRAGON_SCALES, "kamalak ajdaho farqlari" },
	{ BLOOD_DRAGON_SCALES, "qon ajdaho farqlari" },
	{ PLAIN_DRAGON_SCALES, "tekis ajdaho farqlari" },
	{ SKY_DRAGON_SCALES, "havo ajdaho farqlari" },
	{ WATER_DRAGON_SCALES, "suv ajdaho farqlari" },
	{ OLIHARCON_SPLINT_MAIL, "nodir ma'dan parcha pochta" },
	{ BAMBOO_MAIL, "bambuk pochta" },
	{ SAILOR_BLOUSE, "kemachi bluzka" },
	{ SAFEGUARD_SUIT, "himoya kostyum" },
	{ FEATHER_ARMOR, "tuklar zirh" },
	{ SCHOOL_UNIFORM, "maktab forma" },
	{ BUNNY_UNIFORM, "quyon forma" },
	{ MAID_DRESS, "qiz kiyim" },
	{ NURSE_UNIFORM, "hamshira forma" },
	{ COMMANDER_SUIT, "qo'mondoni kostyum" },
	{ CAMOUFLAGED_CLOTHES, "kamufle kiyim" },
	{ SPECIAL_CAMOUFLAGED_CLOTHES, "maxsus kamufle kiyim" },
	{ SHOULDER_RINGS, "elka uzuk" },
	{ PRINTED_SHIRT, "bosilgan ko'ylak" },
	{ BATH_TOWEL, "hammom sochiq" },
	{ PLUGSUIT, "vilkasi kostyum" },
	{ MEN_S_UNDERWEAR, "erkaklar ichki" },
	{ AYANAMI_WRAPPING, "o'ylab o'tkazish" },
	{ RUBBER_APRON, "rezina perroni" },
	{ KITCHEN_APRON, "oshxona perroni" },
	{ FRILLED_APRON, "qirrali kesilgan perroni" },
	{ SUPER_MANTLE, "juda kuchli mantiya" },
	{ WINGS_OF_ANGEL, "farishtaning qanotlari" },
	{ DUMMY_WINGS, "qo'g'irchoqboz qanotlari" },
	{ FUR, "mo'yna" },
	{ HIDE, "yashirish" },
	{ DISPLACER_BEAST_HIDE, "ko'chirgichni hayvon yashirish" },
	{ THE_NEMEAN_LION_HIDE, "ajoyib sher yashirish" },
	{ CLOAK_OF_SPRAY, "buzadigan amallar plash" },
	{ CLOAK_OF_FLAME, "olov plash" },
	{ CLOAK_OF_INSULATION, "izolyatsiya plash" },
	{ CLOAK_OF_MATADOR, "buqa kurashchi plash" },
	{ ORCISH_GUARD_SHIELD, "yomon qorovul qalqon" },
	{ SHIELD, "qalqon" },
	{ SILVER_SHIELD, "kumush qalqon" },
	{ MIRROR_SHIELD, "oyna qalqon" },
	{ RAPIRAPI, "maxsus yaponiya qalqon" },
	{ RAINBOW_DRAGON_SCALE_SHIELD, "kamalak ajdaho yashirish qalqon" },
	{ BLOOD_DRAGON_SCALE_SHIELD, "qon ajdaho yashirish qalqon" },
	{ PLAIN_DRAGON_SCALE_SHIELD, "tekis ajdaho yashirishqalqon" },
	{ SKY_DRAGON_SCALE_SHIELD, "havo ajdaho yashirish qalqon" },
	{ WATER_DRAGON_SCALE_SHIELD, "suv ajdaho yashirish qalqon" },
	{ GAUNTLETS_OF_SAFEGUARD, "himoya qilish sportchisi" },
	{ GAUNTLETS_OF_PLUGSUIT, "vilkasi kostyumi qo'lqop" },
	{ COMMANDER_GLOVES, "qo'mondoni qo'lqop" },
	{ FIELD_GLOVES, "maydon sportchisi" },
	{ GAUNTLETS, "qo'lqop" },
	{ SILVER_GAUNTLETS, "kumush qo'lqop" },
	{ GAUNTLETS_OF_FAST_CASTING, "tez to'qimalarining qo'lqop" },
	{ GAUNTLETS_OF_NO_FLICTION, "yo'q ishqalanish qo'lqop" },
	{ ATSUZOKO_BOOTS, "albatta, yuqori chizilmasin" },
	{ RUBBER_BOOTS, "rezina chizilmasin" },
	{ LEATHER_SHOES, "charm poyabzal" },
	{ SNEAKERS, "shippak" },
	{ MULTI_SHOES, "ko'p poyafzali" },
	{ BOOTS_OF_PLUGSUIT, "vilkasi kostyumi chizilmasin" },
	{ ROLLER_BLADE, "rolikli pichoq" },
	{ FIELD_BOOTS, "dala chizilmasin" },
	{ BOOTS_OF_SAFEGUARD, "himoya qilish chizilmasin" },
	{ STOMPING_BOOTS, "botlarni oyoq osti" },
	{ CARRYING_BOOTS, "botlarni olib" },
	{ FREEZING_BOOTS, "muzlash chizilmasin" },
	{ RIN_PRACTICE, "amaliyot" },
	{ RIN_CONFUSION_RESISTANCE, "tartibsizlik qarshilik" },
	{ RIN_RESTRATION, "bayiltici qarshilik" },
	{ RIN_HEAVY_ATTACK, "og'ir hujum" },
	{ AMULET_OF_PRISM, "amulet of bir tekis" },
	{ AMULET_OF_WARP_DIMENSION, "amulet of coezgue kattalik" },
	{ AMULET_OF_D_TYPE_EQUIPMENT, "amulet of d-turi uskunalari" },
	{ AMULET_VERSUS_DEATH_SPELL, "amulet of o'lim sehr oldini olish" },
	{ AMULET_OF_QUICK_ATTACK, "amulet of tez hujum" },
	{ AMULET_OF_QUADRUPLE_ATTACK, "amulet of to'rt kishilik hujum" },
	{ PENDANT, "marjonlarni" },
	{ NECKLACE, "marjon" },
	{ JAPAN_WAX_CANDLE, "yaponiya sham sham" },
	{ OIL_CANDLE, "neft sham" },
	{ TEMPEST_HORN, "tug'yon shox" },
	{ SHEAF_OF_STRAW, "somon dasta" },
	{ COTTON, "paxta" },
	{ ONION, "piyoz" },
	{ WELSH_ONION, "quloq tutmaslik piyoz" },
	{ WATERMELON, "tarvuz" },
	{ WHITE_PEACH, "oq shaftoli" },
	{ SENTOU, "xavfli oziq-ovqat" },
	{ BEAN, "loviya" },
	{ SENZU, "maxsus loviya" },
	{ PARFAIT, "muz meva shirin" },
	{ X_MAS_CAKE, "rojdestvo kek" },
	{ BUNNY_CAKE, "quyon kek" },
	{ BAKED_SWEET_POTATO, "pishirilgan shirin kartoshka" },
	{ BREAD, "non" },
	{ PASTA, "makaron" },
	{ CHARRED_BREAD, "kuygan non" },
	{ SLICE_OF_PIZZA, "sarkueteri tilim" },
	{ WHITE_SWEETS, "oq shirinliklar" },
	{ BROWN_SWEETS, "jigarrang shirinliklar" },
	{ GREEN_SWEETS, "yashil pishiriqlar" },
	{ PINK_SWEETS, "pushti Pishiriqlar" },
	{ BLACK_SWEETS, "qora shirinliklar" },
	{ YELLOW_SWEETS, "sariq pishiriqlar" },
	{ BOTA_MOCHI, "guruch kek" },
	{ KIBI_DANGO, "o'tgan yaponiya oziq-ovqat" },
	{ SAKURA_MOCHI, "yaponiya shirinliklar" },
	{ KOUHAKU_MANJYUU, "sehrli shirinliklar" },
	{ YOUKAN, "shakar pishiriqlar" },
	{ CHOCOLATE, "shokolad" },
	{ CHOCOEGG, "shokolad tuxum" },
	{ WAKAME, "jigarrang o'tlar" },
	{ MAGIC_BANANA, "sehr banan" },
	{ LUNCH_OF_BOILED_EGG, "qaynatilgan tuxum tushlik" },
	{ PIZZA, "pitstsa" },
	{ POT_PORTER, "yuk tashuvchi" },
	{ POT_WONDER, "hayron" },
	{ SCR_WONDER, "hayron" },
	{ POT_TERCES_DLU, "maxsus ta'siri" },
	{ POT_HIDING, "yashirish" },
	{ POT_DECOY_MAKING, "qarmoq qilish" },
	{ POT_DOWN_LEVEL, "pastga darajasi" },
	{ POT_KEEN_MEMORY, "o'tkir xotira" },
	{ POT_CURE_WOUNDS, "davolash yaralar" },
	{ POT_CURE_SERIOUS_WOUNDS, "jiddiy jarohatlarni davolash" },
	{ POT_NIGHT_VISION, "kecha ko'rish" },
	{ POT_CURE_CRITICAL_WOUNDS, "tanqidiy jarohatlarni davolash" },
	{ POT_RESISTANCE, "qarshilik" },
	{ POT_POISON, "zahar" },
	{ POT_COFFEE, "qahva" },
	{ POT_RED_TEA, "qizil choy" },
	{ POT_OOLONG_TEA, "sazavor bo'lgan degerdedir choy" },
	{ POT_GREEN_TEA, "yashil choy" },
	{ POT_COCOA, "kakao" },
	{ POT_GREEN_MATE, "yashil umr yo'ldosh" },
	{ POT_TERERE, "tetiklashtiradigan ichimlik" },
	{ POT_AOJIRU, "sabzavot ichimlik" },
	{ POT_WINE, "vino" },
	{ POT_ULTIMATE_TSUYOSHI_SPECIAL, "yakuniy xayol maxsus" },
	{ POT_MEHOHO_BURUSASAN_G, "harakat qahramon ichimlik" },
	{ SCR_FLOOD_TIDE, "sel yo'l" },
	{ SCR_EBB_TIDE, "tushkunlik yo'l" },
	{ SCR_COPYING, "nusxa ko'chirish" },
	{ SCR_CREATE_VICTIM, "qurboni yaratish" },
	{ SCR_CREATE_FACILITY, "ta'sis yaratish" },
	{ SCR_ERASURE, "tozalash" },
	{ SCR_CURE_BLINDNESS, "davolash ko'rlik" },
	{ SCR_POWER_HEALING, "kuch shifo" },
	{ SCR_SKILL_UP, "san'at qadar" },
	{ SCR_FROST, "ayoz" },
	{ SCR_CREATE_ALTAR, "qurbongoh yaratish" },
	{ SCR_CREATE_TRAP, "tuzoq yaratish" },
	{ SCR_CREATE_SINK, "qiluvchini yaratish" },
	{ SCR_SYMMETRY, "simmetriya" },
	{ SCR_CREATE_CREATE_SCROLL, "kitobni yaratish" },
	{ SCR_DESTROY_WEAPON, "qurol halok" },
	{ SPE_KNOW_ENCHANTMENT, "sehr bilaman" },
	{ SPE_MAGICTORCH, "sehr mash'alasi" },
	{ SPE_DISPLACEMENT, "olish" },
	{ SPE_MASS_HEALING, "ommaviy shifo" },
	{ SPE_TIME_SHIFT, "vaqt o'zgarish" },
	{ WAN_MISFIRE, "muvaffaqiyatsizlik" },
	{ WAN_VENOM_SCATTERING, "achchiq sacilma" },
	{ WAN_SHARE_PAIN, "ulushi og'riq" },
	{ WAN_POISON, "zahar" },
	{ MORION, "harakat tosh" },
	{ TALC, "bilan talk" },
	{ GRAPHITE, "grafitning" },
	{ VOLCANIC_GLASS_FRAGMENT, "vulqon shisha qism" },
	{ STARLIGHTSTONE, "yulduz nur tosh" },
	{ SCR_ALTER_REALITY, "haqiqatni o'zgartirish" },
	{ SPE_ALTER_REALITY, "haqiqatni o'zgartirish" },
	{ FULL_PLATE_MAIL, "to'liq plastinka pochta" },

	{ WAN_DISINTEGRATION_BEAM, "parchalanish nur" },
	{ SPE_DISINTEGRATION_BEAM, "parchalanish nur" },
	{ SCR_GROUP_SUMMONING, "guruh chaqirishni" },
	{ SPE_FLYING, "uchib" },
	{ WAN_CHROMATIC_BEAM, "rang nur" },
	{ SPE_CHROMATIC_BEAM, "rang nur" },
	{ SPE_FUMBLING, "paypaslab izlay oxirgi" },
	{ SPE_MAKE_VISIBLE, "paydo qilish" },
	{ SPE_WARPING, "coezgue" },
	{ SPE_TRAP_CREATION, "makr yaratish" },
	{ WAN_STUN_MONSTER, "bayiltici hayvon" },
	{ SPE_STUN_MONSTER, "bayiltici hayvon" },
	{ SPE_CURSE_ITEMS, "la'nat mahsulot" },
	{ SPE_CHARACTER_RECURSION, "belgilar oezyineleme" },
	{ SPE_CLONE_MONSTER, "klonudur hayvon" },
	{ SCR_WORLD_FALL, "qiyomat" },
	{ SCR_RESURRECTION, "qo'shimcha hayot" },
	{ SCR_SUMMON_GHOST, "la'nati ruh" },
	{ SCR_MEGALOAD, "og'ir yuk" },
	{ SPE_DESTROY_ARMOR, "sovuti halok" },
	{ SCR_ENRAGE, "g'azabini" },
	{ WAN_TIDAL_WAVE, "fasllari to'lqin" },
	{ SCR_ANTIMATTER, "qarshi modda" },
	{ SCR_SUMMON_ELM, "karaagaca chaqirish" },
	{ WAN_SUMMON_ELM, "karaagaca chaqirish" },
	{ SCR_RELOCATION, "ko'chirish" },
	{ WAN_DRAIN_MANA, "sehrli energiya drenaj" },
	{ WAN_FINGER_BENDING, "barmoq etma" },
	{ SCR_IMMOBILITY, "qimirlamaslik" },
	{ WAN_IMMOBILITY, "qimirlamaslik" },
	{ SCR_FLOODING, "baliq sel" },
	{ SCR_EGOISM, "xudbinlik" },
	{ WAN_EGOISM, "xudbinlik" },
	{ SCR_RUMOR, "mish-mish gaplar" },
	{ SCR_MESSAGE, "xabar" },
	{ SCR_SIN, "gunoh" },
	{ WAN_SIN, "gunoh" },
	{ WAN_INERTIA, "dangasalik" },
	{ SPE_INERTIA, "dangasalik" },
	{ WAN_TIME, "vaqt" },
	{ SPE_TIME, "vaqt" },
	{ AMULET_OF_SPEED, "amulet of tezlik" },
	{ AMULET_OF_TELEPORT_CONTROL, "amulet of nazorat gulan" },
	{ AMULET_OF_POLYMORPH_CONTROL, "amulet of bir polimorfunun nazorat" },
	{ HELM_OF_SPEED, "tezlik zarbdan" },
	{ HELMET_OF_UNDEAD_WARNING, "bo'lmagan-o'lik ogohlantirish zarbdan" },

	{ TURN_LIMIT_STONE, "chegarasi toshni o'girib" },
	{ LIMITATION_BOOTS, "cheklash chizilmasin" },
	{ WEAK_SIGHT_STONE, "zaif ko'rish tosh" },
	{ DIMMER_HELMET, "karartma xususiyati dubulg'a" },
	{ CHATTER_STONE, "safsata tosh" },
	{ CHATBOX_CLOAK, "nejmeddin erbakan rido" },

	{ SILVER_CHAKRAM, "kumush uchirish majmui" },
	{ HIGH_ELVEN_WARSWORD, "yuqori erda alf urush qilich" },
	{ WAR_HAT, "urush xet" },
	{ ELVEN_HELM, "erda alf elkan" },
	{ HIGH_ELVEN_HELM, "yuqori erda alf elkan" },
	{ GENTLEMAN_S_SUIT, "bey afandi kostyum" },
	{ GENTLEWOMAN_S_DRESS, "xonim liboslar" },
	{ STRAITJACKET, "tor kamzul" },
	{ CURING_UNIFORM, "tuzatish yagona" },
	{ HAWAIIAN_SHORTS, "gavayi shortilar" },

	{ CONDOME, "prezervativ" },
	{ HYDRA_BOW, "gidra kamon" },
	{ MANCATCHER, "odam qiluvchi" },
	{ MARE_TRIDENT, "marek sacayagimiz" },
	{ HELO_CROSSBOW, "xalon yayli miltiq" },
	{ BATARANG, "botsheva bumerang" },

	{ ICE_BOX_OF_HOLDING, "o'tkazish muz qutisini" },
	{ ICE_BOX_OF_WATERPROOFING, "gidroizolyatsiya bo'yicha muzlatgich" },
	{ ICE_BOX_OF_DIGESTION, "hazm muzlatgich" },
	{ LARGE_BOX_OF_DIGESTION, "hazm katta qutisini" },
	{ CHEST_OF_HOLDING, "o'tkazish ko'krak" },

	{ SCR_ANTIMAGIC, "qarshi sehr" },
	{ SCR_RESISTANCE, "qarshilik" },

	{ GENERAL_CANDLE, "umumiy sham" },
	{ NATURAL_CANDLE, "tabiiy sham" },

	{ RIN_DOOM, "halokat" },
	{ RIN_ELEMENTS, "elementlar" },
	{ RIN_LIGHT, "yorug'lik" },
	{ WAN_LEVITATION, "suzuvchi" },
	{ PSYCHIC_DRAGON_SCALE_MAIL, "ruhiy ajdar ko'lamli zirh" },
	{ PSYCHIC_DRAGON_SCALE_SHIELD, "ruhiy ajdar ko'lamli qalqon" },
	{ PSYCHIC_DRAGON_SCALES, "ruhiy ajdar tarozilari" },
	{ WAN_PSYBEAM, "psixologik nur" },
	{ SPE_PSYBEAM, "psixologik nur" },
	{ HELM_OF_TELEPORTATION, "teletasinmasi rul" },
	{ SWIMSUIT, "mayo" },
	{ TROLL_LEATHER_ARMOR, "troll charm zirh" },
	{ TROLL_HIDE, "troll yashir" },
	{ GAUNTLETS_OF_FREE_ACTION, "bepul harakatlar qo'lqop" },
	{ GAUNTLETS_OF_GOOD_FORTUNE, "yaxshi boylik qo'lqop" },
	{ BOOTS_OF_FREEDOM, "erkinlik chizilmasin" },
	{ AMULET_OF_INSOMNIA, "amulet of uyqusizlik" },
	{ RIN_MAGIC_RESISTANCE, "sehrli qarshilik" },
	{ RIN_MATERIAL_STABILITY, "moddiy barqarorlik" },
	{ RIN_MIND_SHIELDING, "aqli himoya" },
	{ AMULET_OF_MENTAL_STABILITY, "amulet of ruhiy barqarorlik" },
	{ WAN_DEBUGGING, "xato topish" },
	{ WAN_HYPER_BEAM, "hiper ish nur" },
	{ SPE_HYPER_BEAM, "hiper ish nur" },
	{ HELM_OF_TELEPORT_CONTROL, "coezgue nazorat rul" },

	{ HERETIC_CLOAK, "bid'atchi plash" },
	{ HELM_OF_STARVATION, "haqiqiy ochlikdan rul" },
	{ RIN_ANTI_DROP, "anti tomchi" },
	{ AMULET_OF_WEAK_MAGIC, "amulet of zaif sehr" },
	{ RIN_ENSNARING, "soxta" },
	{ EERIE_CLOAK, "dahshatli plash" },
	{ QUAFFER_HELMET, "ichimlik dubulg'a" },
	{ PET_STOMPING_PLATFORM_BOOTS, "platformasi etik mavjud ezib" },
	{ AMULET_OF_DIRECTIONAL_SWAP, "amulet of yo'nalish harakat o'zgarishi" },
	{ INCORRECTLY_ADJUSTED_HELMET, "noto'g'ri yo'lga dubulg'a" },
	{ AMULET_OF_SUDDEN_CURSE, "amulet of to'satdan la'nati" },
	{ CLOAK_OF_NAKEDNESS, "yalang'och plash" },
	{ AMULET_OF_ANTI_EXPERIENCE, "amulet of xola tajribasi" },
	{ GAUNTLETS_OF_STEALING, "o'g'irlaganlik qo'lqop" },
	{ ASS_KICKER_BOOTS, "eshak ro'yxatdan beradi chizilmasin" },
	{ RIN_DIARRHEA, "ichburug'" },
	{ BOOTS_OF_TOTAL_STABILITY, "umumiy barqarorlik chizilmasin" },
	{ HELM_OF_TRUE_SIGHT, "haqiqiy ko'zdan rul" },
	{ ELVEN_GAUNTLETS, "alf qo'lqop" },
	{ CLOAK_OF_AGGRAVATION, "yomonlashganiga of plash" },
	{ CLOAK_OF_CONFLICT, "mojaro plash" },
	{ HELM_OF_WARNING, "ogohlantirish rul" },
	{ BOOTS_OF_DISPLACEMENT, "joyidan chizilmasin" },
	{ CLOAK_OF_MAGICAL_BREATHING, "sehrli nafas plash" },
	{ HELM_OF_DETOXIFICATION, "detoksifikasyon rul" },
	{ BOOTS_OF_SWIMMING, "suzish chizilmasin" },
	{ CLOAK_OF_STAT_LOCK, "holati himoyasi plash" },
	{ WING_CLOAK, "qanot plash" },
	{ HELM_OF_NO_DIGESTION, "hech hazm rul" },
	{ CLOAK_OF_PREMATURE_DEATH, "erta o'lim plash" },
	{ ANTI_CURSE_BOOTS, "anti la'nati chizilmasin" },
	{ TINFOIL_HELMET, "kalay dubulg'a" },
	{ ANTIDEATH_CLOAK, "anti o'lim plash" },
	{ PARANOIA_HELMET, "paranoya dubulg'a" },
	{ GAUNTLETS_OF_MISFIRING, "noto'g'ri atesleme of qo'lqop" },
	{ DEMENTIA_BOOTS, "bu demans chizilmasin" },
	{ NONSACRED_STONE, "bo'lmagan muqaddas tosh" },
	{ STARVATION_STONE, "ochlik tosh" },
	{ DROPLESS_STONE, "kam tosh tomchi" },
	{ LOW_EFFECT_STONE, "past ta'siri tosh" },
	{ INVISO_STONE, "ko'rinmas tosh" },
	{ GHOSTLY_STONE, "diniy tosh" },
	{ DEHYDRATING_STONE, "dehidre tosh" },
	{ STONE_OF_HATE, "nafrat tosh" },
	{ DIRECTIONAL_SWAP_STONE, "yo'nalishi harakat o'zgarishi tosh" },
	{ NONINTRINSICAL_STONE, "bo'lmagan ichki tosh" },
	{ DROPCURSE_STONE, "la'nati toshni tomchi" },
	{ STONE_OF_NAKED_STRIPPING, "yalang'och qazish tosh" },
	{ ANTILEVEL_STONE, "aksil darajasi tosh" },
	{ STEALER_STONE, "o'g'rilik qilgan tosh" },
	{ REBEL_STONE, "isyonchilar tosh" },
	{ SHIT_STONE, "boktan tosh" },
	{ STONE_OF_MISFIRING, "atesleme tosh" },
	{ STONE_OF_PERMANENCE, "turar tosh" },
	{ BOOGEYMAN_HELMET, "yomon odam dubulg'a" },
	{ SPE_FIRE_BOLT, "yong'in mermilerinin" },

	{ PLATINUM_DRAGON_SCALE_SHIELD, "platina ajdaho ko'lamli qalqon" },
	{ BRASS_DRAGON_SCALE_SHIELD, "jez ajdaho ko'lamli qalqon" },
	{ COPPER_DRAGON_SCALE_SHIELD, "mis ajdaho ko'lamli qalqon" },
	{ EMERALD_DRAGON_SCALE_SHIELD, "zumrad ajdaho ko'lamli qalqon" },
	{ RUBY_DRAGON_SCALE_SHIELD, "yoqut ajdaho ko'lamli qalqon" },
	{ SAPPHIRE_DRAGON_SCALE_SHIELD, "ko'kish yoqut ajdaho ko'lamli qalqon" },
	{ DIAMOND_DRAGON_SCALE_SHIELD, "olmos ajdaho ko'lamli qalqon" },
	{ AMETHYST_DRAGON_SCALE_SHIELD, "ametist ajdaho ko'lamli qalqon" },

	{ PLATINUM_DRAGON_SCALES, "platina ajdaho tarozilari" },
	{ BRASS_DRAGON_SCALES, "jez ajdaho tarozilari" },
	{ COPPER_DRAGON_SCALES, "mis ajdaho tarozilari" },
	{ EMERALD_DRAGON_SCALES, "zumrad ajdaho tarozilari" },
	{ RUBY_DRAGON_SCALES, "yoqut ajdaho tarozilari" },
	{ SAPPHIRE_DRAGON_SCALES, "ko'kish yoqut ajdaho tarozilari" },
	{ DIAMOND_DRAGON_SCALES, "olmos ajdaho tarozilari" },
	{ AMETHYST_DRAGON_SCALES, "ametist toshlarni ajdaho tarozilari" },

	{ PLATINUM_DRAGON_SCALE_MAIL, "platina ajdaho ko'lamli zirh" },
	{ BRASS_DRAGON_SCALE_MAIL, "jez ajdaho ko'lamli zirh" },
	{ COPPER_DRAGON_SCALE_MAIL, "mis ajdaho ko'lamli zirh" },
	{ EMERALD_DRAGON_SCALE_MAIL, "zumrad ajdaho ko'lamli zirh" },
	{ RUBY_DRAGON_SCALE_MAIL, "yoqut ajdaho ko'lamli zirh" },
	{ SAPPHIRE_DRAGON_SCALE_MAIL, "ko'kish yoqut ajdaho ko'lamli zirh" },
	{ DIAMOND_DRAGON_SCALE_MAIL, "olmos ajdaho ko'lamli zirh" },
	{ AMETHYST_DRAGON_SCALE_MAIL, "ametist ajdaho ko'lamli zirh" },

	{ RANDOMIZED_HELMET, "borilayotgan dubulg'a" },
	{ HIGH_STILETTOS, "yuqori sandal chizilmasin" },
	{ UNKNOWN_GAUNTLETS, "noma'lum qo'lqop" },
	{ MISSING_CLOAK, "yo'qolgan plash" },

	{ AMULET_OF_THE_RNG, "amulet of tasodifiy soni ishlab chiqaruvchi" },
	{ AMULET_OF_INFINITY, "amulet of cheksizlik" },
	{ RIN_RANDOM_EFFECTS, "tasodifiy ta'sir" },
	{ RIN_SPECIAL_EFFECTS, "maxsus effektlar" },

	{ PURPLE_DRAGON_SCALE_SHIELD, "safsar ajdaho ko'lamli qalqon" },
	{ PURPLE_DRAGON_SCALES, "safsar ajdaho tarozilari" },
	{ PURPLE_DRAGON_SCALE_MAIL, "safsar ajdaho ko'lamli zirh" },

	{ DARK_MAGIC_WHISTLE, "qora sehr hushtak" },
	{ SPE_INFERNO, "do'zax" },
	{ WAN_INFERNO, "do'zax" },
	{ SPE_ICE_BEAM, "muz nur" },
	{ WAN_ICE_BEAM, "muz nur" },
	{ SPE_THUNDER, "momaqaldiroq" },
	{ WAN_THUNDER, "momaqaldiroq" },
	{ SPE_SLUDGE, "baliqli ko'lining" },
	{ WAN_SLUDGE, "baliqli ko'lining" },
	{ SPE_TOXIC, "zaharli" },
	{ WAN_TOXIC, "zaharli" },
	{ SPE_NETHER_BEAM, "juda past nur" },
	{ WAN_NETHER_BEAM, "juda past nur" },
	{ SPE_AURORA_BEAM, "aurora o'rganish nur" },
	{ WAN_AURORA_BEAM, "aurora o'rganish nur" },
	{ SPE_GRAVITY_BEAM, "agar tortishish kuchi nur" },
	{ WAN_GRAVITY_BEAM, "agar tortishish kuchi nur" },
	{ SPE_CHLOROFORM, "xloroform" },
	{ WAN_CHLOROFORM, "xloroform" },
	{ SPE_DREAM_EATER, "egan orzu" },
	{ WAN_DREAM_EATER, "egan orzu" },
	{ SPE_BUBBLEBEAM, "qabariq nur" },
	{ WAN_BUBBLEBEAM, "qabariq nur" },
	{ SPE_GOOD_NIGHT, "xayrli tun" },
	{ WAN_GOOD_NIGHT, "xayrli tun" },
	{ ANTIMATTER_BULLET, "qarshi modda o'q" },
	{ CLOAK_OF_LEECH, "suluk oid plash" },
	{ GAUNTLETS_OF_LEECH, "suluk oid qo'lqop" },
	{ AMULET_OF_LEECH, "amulet of zuluk" },
	{ RIN_LEECH, "zuluk" },
	{ KYRT_SHIRT, "kurt ko'ylak" },
	{ FILLER_CLOAK, "plomba plash" },
	{ DART_OF_DISINTEGRATION, "tortish parchalanish" },
	{ SCR_ARMOR_SPECIALIZATION, "zirh ixtisoslashuvi" },
	{ SCR_SECURE_IDENTIFY, "xavfsiz kimligi" },
	{ AMULET_OF_DANGER, "amulet of xavf" },
	{ RIN_DANGER, "xavf" },
	{ NASTY_CLOAK, "xavfli plash" },
	{ UNWANTED_HELMET, "istalmagan dubulg'a" },
	{ EVIL_GLOVES, "yomon qo'lqop" },
	{ UNFAIR_STILETTOS, "adolatsiz sandal chizilmasin" },
	{ SPECIAL_CLOAK, "maxsus plash" },
	{ WONDER_HELMET, "ajablanarli dubulg'a" },
	{ ARCANE_GAUNTLETS, "urug'dan qo'lqop" },
	{ SKY_HIGH_HEELS, "baland poshnalar" },
	{ PLAIN_CLOAK, "ochiq-oydin plash" },
	{ POINTED_HELMET, "o'tkir dubulg'a" },
	{ PLACEHOLDER_GLOVES, "er qattiq qo'l qo'lqop" },
	{ PREHISTORIC_BOOTS, "vaqt oldin chizilmasin" },
	{ ARCHAIC_CLOAK, "arkaik plash" },
	{ BOG_STANDARD_HELMET, "serbotqoqqa-namunaviy dubulg'a" },
	{ PROTECTIVE_GLOVES, "himoya qo'lqop" },
	{ SYNTHETIC_SANDALS, "sintetik kavushlari" },
	{ SPE_FIXING, "ekranga" },

	{ SCR_ARTIFACT_CREATION, "qo'lyozmasi yaratish" },
	{ LETHE_CLOAK, "leta plash" },
	{ UGG_BOOTS, "ugg botlarni" },
	{ AMULET_OF_STONE, "amulet of tosh" },
	{ AMULET_OF_MAP_AMNESIA, "amulet of xaritasi amneziya" },
	{ CLOAK_OF_MAP_AMNESIA, "xaritasi amneziya of plash" },
	{ AMULET_OF_DEPRESSION, "amulet of depressiya" },
	{ CLOAK_OF_POLYMORPH, "polimorf plash" },
	{ CLOAK_OF_TRANSFORMATION, "o'zgartirish plash" },
	{ CLOAK_OF_WATER_SQUIRTING, "suv bilan belgila plash" },
	{ CLOAK_OF_PARALYSIS, "falaj plash" },
	{ CLOAK_OF_SICKNESS, "kasallik plash" },
	{ CLOAK_OF_SLIMING, "shilimshik plash" },
	{ CLOAK_OF_STARVING, "och plash" },
	{ CLOAK_OF_CURSE, "la'nat plash" },
	{ CLOAK_OF_DISENCHANTMENT, "ko'ngilsizlik dastidan of plash" },
	{ CLOAK_OF_OUTRIGHT_EVILNESS, "ochiq yomon vujudini plash" },
	{ CLOAK_OF_STONE, "tosh plash" },
	{ CLOAK_OF_LYCANTHROPY, "bo'ri insoniyat plash" },
	{ CLOAK_OF_UNLIGHT, "hech yorug'lik plash" },
	{ CLOAK_OF_ESCALATION, "keskinlashtirib of plash" },
	{ CLOAK_OF_MAGICAL_DRAINAGE, "sehrli drenaj plash" },
	{ CLOAK_OF_ANGRINESS, "g'azablangan bo'lish plash" },
	{ CLOAK_OF_CANCELLATION, "bekor plash" },
	{ CLOAK_OF_TURN_LOSS, "navbat zarar plash" },
	{ CLOAK_OF_ATTRIBUTE_LOSS, "xususiyati zarar plash" },
	{ CLOAK_OF_TOTTER, "oqsoqlanmoq of plash" },
	{ CLOAK_OF_DRAIN_LIFE, "drenaj hayot plash" },
	{ CLOAK_OF_AWAKENING, "uyg'onish plash" },
	{ CLOAK_OF_STABILITY, "barqarorlik plash" },
	{ ANTI_DISQUIET_CLOAK, "aksil tashvish plash" },
	{ HUGGING_GOWN, "havola etdi libosi" },
	{ COCLOAK, "hamkorlik plash" },
	{ CLOAK_OF_HEALTH, "sog'liqni saqlash plash" },
	{ CLOAK_OF_DISCOVERY, "kashf plash" },
	{ BIONIC_CLOAK, "bionik plash" },
	{ CLOAK_OF_PORTATION, "port etish plash" },
	{ CLOAK_OF_CONTROL, "nazorat plash" },
	{ CLOAK_OF_SHIFTING, "o'zgaruvchan etish plash" },
	{ FLOATCLOAK, "suzuvchi plash" },
	{ CLOAK_OF_PRESCIENCE, "sergak etish plash" },
	{ SENSOR_CLOAK, "o'lchash datchigi plash" },
	{ CLOAK_OF_SPEED, "tezlik plash" },
	{ VAULT_CLOAK, "bank seyfi plash" },
	{ CLOAK_OF_SPELL_RESISTANCE, "afsun qarshilik plash" },
	{ CLOAK_OF_PHYSICAL_RESISTANCE, "jismoniy qarshilik plash" },
	{ OPERATION_CLOAK, "operatsiya plash" },
	{ BESTEST_CLOAK, "mutlaq eng yaxshi plash" },
	{ CLOAK_OF_FREEDOM, "erkinlik plash" },
	{ BIKINI, "ayol mayo" },
	{ CLOAK_OF_PERMANENCE, "turar plash" },
	{ CLOAK_OF_SLOW_DIGESTION, "sekin hazm plash" },
	{ CLOAK_OF_INFRAVISION, "infraqizil ko'rish plash" },
	{ CLOAK_OF_BANISHING_FEAR, "voz kechishning qo'rquv plash" },
	{ CLOAK_OF_MEMORY, "xotira plash" },
	{ CLOAK_OF_THE_FORCE, "kuch plash" },
	{ CLOAK_OF_SEEING, "ko'rish plash" },
	{ CLOAK_OF_CURSE_CATCHING, "la'nat plash yuqumli" },
	{ LION_CLOAK, "sher plash" },
	{ TIGER_CLOAK, "yo'lbars plash" },
	{ CLOAK_OF_PRACTICE, "amaliyotining plash" },
	{ CLOAK_OF_ELEMENTALISM, "elementlarning plash" },
	{ PSIONIC_CLOAK, "ruhiy plash" },
	{ SAWED_OFF_SHOTGUN, "o'chiring arra maqsadida ov miltiq" },
	{ SCR_INSTANT_AMNESIA, "lahzali amneziya" },
	{ ROBE_OF_MAGIC_RESISTANCE, "sehrli qarshilik to'n" },
	{ ROBE_OF_PERMANENCE, "turar to'n" },
	{ ROBE_OF_SPELL_POWER, "afsun hokimiyat to'n" },
	{ ROBE_OF_FAST_CASTING, "tez ushaqlar to'n" },
	{ ROBE_OF_ENERGY_SUCTION, "energiya assimilyatsiya to'n" },
	{ ROBE_OF_RANDOMNESS, "rasgelelik ridosini" },
	{ ROBE_OF_DEFENSE, "mudofaa to'n" },
	{ ROBE_OF_SPECIALTY, "mutaxassisligi ridosini" },
	{ ROBE_OF_NASTINESS, "iflos ishlar ridosini" },
	{ ROBE_OF_PSIONICS, "ruhiy to'n" },
	{ UNSPECIFIED_CANDLE, "noma'lum sham" },
	{ POWER_CROSSBOW, "elektr yayli miltiq" },

	{ SPE_CHAOS_TERRAIN, "betartiblik yerlarni" },
	{ SPE_RANDOM_SPEED, "tasodifiy tezligi" },
	{ SPE_VANISHING, "barham topish davri" },
	{ SPE_WISHING, "xohlovchi" },
	{ SPE_ACQUIREMENT, "olish" },
	{ SPE_CHAOS_BOLT, "betartiblik murvat" },
	{ SPE_HELLISH_BOLT, "jahannamiy dardning asoratidan murvat" },
	{ SPE_EARTHQUAKE, "zilzila" },
	{ SPE_LYCANTHROPY, "bo'ri inson shakli" },
	{ SPE_BUC_RANDOMIZATION, "buc randomizasyon" },
	{ SPE_LOCK_MANIPULATION, "qulf hiyla" },
	{ SPE_POLYFORM, "belgigacha bo'lgan poli shakli" },
	{ SPE_MESSAGE, "xabar" },
	{ SPE_RUMOR, "mish-mish gaplar" },
	{ SPE_CURE_RANDOM_STATUS, "tasodifiy maqomini davolash" },
	{ SPE_RESIST_RANDOM_ELEMENT, "tasodifiy elementi qarshi" },
	{ SPE_RUSSIAN_ROULETTE, "rus ruletka" },
	{ SPE_POSSESSION, "egalik" },
	{ SPE_TOTEM_SUMMONING, "totemlarga chaqirishni" },
	{ SPE_MIMICRY, "mimika taqlid" },
	{ SPE_HORRIFY, "qo'rqitmoq" },
	{ SPE_TERROR, "kishi terror hujumini" },
	{ SPE_PHASE_DOOR, "bosqichi eshik" },
	{ SPE_TRAP_DISARMING, "qaytsin qurolsizlantirish" },
	{ SPE_NEXUS_FIELD, "aloqa faoliyat" },
	{ SPE_COMMAND_DEMON, "buyrug'i jin" },
	{ SPE_FIRE_GOLEM, "yong'in gol" },
	{ SPE_DISRUPTION_SHIELD, "buzilish qalqon" },
	{ SPE_SPELLBINDER, "majburiy imlo" },
	{ SPE_TRACKER, "kuzatishdan" },
	{ SPE_INERTIA_CONTROL, "inertsiya nazorat" },
	{ SPE_CODE_EDITING, "kodi tahrir" },
	{ SPE_FORGOTTEN_SPELL, "unutilgan afsun" },
	{ SPE_FLOOD, "suv toshqini" },
	{ SPE_LAVA, "lava oqimlarida" },
	{ SPE_IRON_PRISON, "temir qamoqxona" },
	{ SPE_LOCKOUT, "qulflash" },
	{ SPE_CLOUDS, "bulutlar" },
	{ SPE_ICE, "muz" },
	{ SPE_GROW_TREES, "daraxtlar o'sadi" },
	{ SPE_DRIPPING_TREAD, "terlaganda qadam" },
	{ SPE_GEOLYSIS, "geologik hal" },
	{ SPE_ELEMENTAL_BEAM, "tabiiy nur" },
	{ SPE_STERILIZE, "sterillamoq" },
	{ SPE_WIND, "shamol" },
	{ SPE_FIRE, "yong'in" },
	{ SPE_ELEMENTAL_MINION, "asosiy arzanda" },
	{ SPE_WATER_BOLT, "suv murvat" },
	{ SPE_AIR_CURRENT, "havo joriy" },
	{ SPE_DASHING, "qo'li bilan qo'ygan jimjimador" },
	{ SPE_MELTDOWN, "erish" },
	{ SPE_POISON_BRAND, "zahar brendi" },
	{ SPE_STEAM_VENOM, "bug 'zahar" },
	{ SPE_HOLD_AIR, "havo o'tkazadi" },
	{ SPE_SWIMMING, "suzish" },
	{ SPE_VOLT_ROCK, "sarma jinslar" },
	{ SPE_WATER_FLAME, "suv olov" },
	{ SPE_AVALANCHE, "qor ko'chimi" },
	{ SPE_MANA_BOLT, "sehrli murvat" },
	{ SPE_ENERGY_BOLT, "energiya murvat" },
	{ SPE_ACID_INGESTION, "kislota biriktirish" },
	{ SPE_INDUCE_VOMITING, "kusturun" },
	{ SPE_REBOOT, "qayta ishga tushirish" },
	{ SPE_HOLY_SHIELD, "muqaddas qalqon" },

	{ SPE_FROST, "ayoz" },
	{ SPE_TRUE_SIGHT, "haqiqiy ko'rish" },
	{ SPE_BERSERK, "g'azablanib" },
	{ SPE_BLINDING_RAY, "ko'r nurlarning" },
	{ SPE_MAGIC_SHIELD, "sehrli qalqon" },
	{ SPE_WORLD_FALL, "qiyomat" },
	{ SPE_ESP, "qo'shimcha hissiy idrok qilish" },
	{ SPE_RADAR, "aniqlovchi asbob" },
	{ SPE_SEARCHING, "puxta" },
	{ SPE_INFRAVISION, "infraqizil ko'rish" },
	{ SPE_STEALTH, "ayyorlik" },
	{ SPE_CONFLICT, "mojaro" },
	{ SPE_REGENERATION, "yangilanishi" },
	{ SPE_FREE_ACTION, "bepul aksiyalar" },
	{ SPE_MULTIBEAM, "ko'p nur" },
	{ SPE_NO_EFFECT, "hech ta'sir" },
	{ SPE_SELFDESTRUCT, "o'zini vayron" },
	{ SPE_THUNDER_WAVE, "momaqaldiroq to'lqin" },
	{ SPE_BATTERING_RAM, "quvvatli qo'chqor" },
	{ SPE_BURROW, "in" },
	{ SPE_GAIN_CORRUPTION, "korruptsiyaga qozonish" },
	{ SPE_SWITCHEROO, "aldamchi bekor qilish" },
	{ SPE_THRONE_GAMBLE, "taxt qimor" },
	{ SPE_BACKFIRE, "o'ziga qarshi ishlay boshlashi" },
	{ SPE_DEMEMORIZE, "endi yod" },
	{ SPE_CALL_THE_ELEMENTS, "elementlarni qo'ng'iroq" },
	{ SPE_NATURE_BEAM, "tabiat nur" },
	{ SPE_WHISPERS_FROM_BEYOND, "narigi vasvasa" },
	{ SPE_STASIS, "staz" },
	{ SPE_CRYOGENICS, "kriyojenikler" },
	{ SPE_REDEMPTION, "sotib" },
	{ SPE_HYPERSPACE_SUMMON, "hiper kosmik chaqirish" },
	{ SPE_SATISFY_HUNGER, "ochlik qondirish" },
	{ SPE_RAIN_CLOUD, "yomg'ir bulut" },
	{ SPE_POWER_FAILURE, "elektr etishmovchiligi" },
	{ SPE_VAPORIZE, "bug'lanmoq" },
	{ SPE_TUNNELIZATION, "harakati" },
	{ SPE_BOMBING, "bombardimon" },
	{ SPE_DRAGON_BLOOD, "ajdaho qon" },
	{ SPE_ANTI_MAGIC_FIELD, "qarshisehrli faoliyat" },
	{ SPE_ANTI_MAGIC_SHELL, "qarshisehrli qobiq" },
	{ SPE_CURE_WOUNDED_LEGS, "yaralangan oyoqlari davolash" },
	{ SPE_ANGER_PEACEFUL_MONSTER, "g'azab tinch maxluq" },
	{ SPE_UNTAME_MONSTER, "dovdirashlariga maxluq" },
	{ SPE_UNLEVITATE, "suzuvchi to'xtatish" },
	{ SPE_DETECT_WATER, "suv aniqlash" },
	{ SPE_APPLY_NAIL_POLISH, "tirnoq jilo amal" },
	{ SPE_ENCHANT, "sehrlamoqchi" },
	{ SPE_DRY_UP_FOUNTAIN, "bulog'ini quritib" },
	{ SPE_TAKE_SELFIE, "o'zingiz rasmini olib" },
	{ SPE_SNIPER_BEAM, "snayper nur" },
	{ SPE_CURE_GLIB, "davolash yengil" },
	{ SPE_CURE_MONSTER, "davolash maxluq" },
	{ SPE_MANA_BATTERY, "sehrli energiya batareya" },
	{ SPE_THORNS, "tikanlar" },
	{ SPE_REROLL_ARTIFACT, "parazit tasodifiy" },
	{ SPE_FINAL_EXPLOSION, "oxirgi portlash" },
	{ SPE_CUTTING, "kesish" },

	{ SCR_DETECT_WATER, "suv aniqlash" },
	{ WAN_SPELLBINDER, "majburiy imlo" },
	{ WAN_INERTIA_CONTROL, "inertsiya nazorat" },
	{ SCR_GEOLYSIS, "geologik hal" },
	{ WAN_STERILIZE, "sterillamoq" },
	{ UNAFFECTED_CANDLE, "samimiy sham" },
	{ SPECIFIC_CANDLE, "muayyan sham" },
 	{ __CANDLE, "0 sham" },

	{ SCR_VILENESS, "razil bo'lgan" },

	{ INKA_LEASH, "inka qilsin" },
	{ INKA_SADDLE, "inka egar" },
	{ UNSTABLE_STETHOSCOPE, "beqaror stetoskop" },
	{ PAPER_SHIELD, "qog'oz qalqon" },
	{ RADIOACTIVE_DAGGER, "radioaktiv xanjar" },
	{ SECRETION_DAGGER, "yuqumli xanjar" },
	{ BITUKNIFE, "asfalt pichoq" },
	{ MEASURER, "o'lchaganini" },
	{ COLLUSION_KNIFE, "maxfiy kelishuv pichoq" },
	{ SPIRIT_AXE, "ruh bolta" },
	{ SOFT_MATTOCK, "yumshoq qo'shbosh" },
	{ INKA_BLADE, "inka arra pichoqni" },
	{ ETERNIUM_BLADE, "abadiy arra pichoqni" },
	{ PAPER_SWORD, "qog'oz qilich" },
	{ MEATSWORD, "go'sht qilich" },
	{ ICKY_BLADE, "hunuk arra pichoqni" },
	{ GRANITE_IMPALER, "granit qurol ustunga" },
	{ ORGANOBLADE, "organik arra pichoqni" },
	{ BIDENHANDER, "ikki qo'li qurol" },
	{ INKUTLASS, "siyoh kesilgan qilich" },
	{ HOE_SABLE, "samur ketmon" },
	{ YATAGAN, "tuebitak tushakka" },
	{ PLATINUM_SABER, "platina qilich" },
	{ WILD_BLADE, "yirtqich arra pichoqni" },
	{ LEATHER_SABER, "charm qilich" },
	{ ARCANE_RAPIER, "urug'dan rapira" },
	{ NATURAL_STICK, "tabiiy tayoq" },
	{ POURED_CLUB, "quydi klubi" },
	{ DIAMOND_SMASHER, "olmos bankrot bo'lish klubi" },
	{ VERMIN_SWATTER, "parazit delicisi" },
	{ PLASTIC_MACE, "plastik moja" },
	{ BRONZE_MACE, "bronza moja" },
	{ MILL_PAIL, "tegirmon chelak" },
	{ BACKHAND_MACE, "orqaga, qo'l moja" },
	{ ASTERISK, "yulduzcha" },
	{ RHYTHMIC_STAR, "badiiy yulduz" },
	{ YESTERDAY_STAR, "kecha yulduz" },
	{ FLOGGER, "urish qurol" },
	{ RIDING_CROP, "minib hosil" },
	{ NOVICE_HAMMER, "ajam bolg'a" },
	{ THUNDER_HAMMER, "momaqaldiroq bolg'a" },
	{ BRIDGE_MUZZLE, "ko'prik tumshuq" },
	{ INKA_BOOT, "inka yuklash" },
	{ SOFT_LADY_SHOE, "yumshoq ayol poyafzal" },
	{ STEEL_CAPPED_SANDAL, "po'lat qalpoqcha kiygan shippagi" },
	{ DOGSHIT_BOOT, "dogshit yuklash" },
	{ IMPACT_STAFF, "ta'sir xodimlari" },
	{ TROUTSTAFF, "alabalik xodimlari" },
	{ FIRE_STICK, "yong'in tayoq" },
	{ OLDEST_STAFF, "eng keksa xodimlar" },
	{ COLOSSUS_BLADE, "juda katta haykal arra pichoqni" },
	{ TUBING_PLIERS, "quvurlar qisqich" },
	{ CHEMISTRY_SPACE_AXE, "kimyo kosmik bolta" },
	{ OSBANE_KATANA, "yuqori darajadagi yaponiya qilich" },
	{ WALKING_STICK, "yurgan tayoq" },
	{ RAIN_PIPE, "yomg'ir quvur" },
	{ PENIS_POLE, "penis bosqich" },
	{ GARDEN_FORK, "bog 'sanchqi" },
	{ PIKE, "payk bilan" },
	{ PHYSICIAN_BAR, "tabib bar" },
	{ HELMET_BEARD, "dubulg'a soqol" },
	{ TRAFFIC_LIGHT, "svetofor" },
	{ GIANT_SCYTHE, "gigant chalg'i" },
	{ THRESHER, "yanchish mashinasi" },
	{ INKA_SPEAR, "inka nayza" },
	{ SILK_SPEAR, "ipak nayza" },
	{ BRITTLE_SPEAR, "mo'rt nayza" },
	{ DRAGON_SPEAR, "ajdar nayza" },
	{ ASBESTOS_JAVELIN, "asbest tele" },
	{ HOMING_TORPEDO, "bosh joyga aylantirish torpedo" },
	{ COURSE_JAVELIN, "albatta tele" },
	{ FOURDENT, "to'rt-o'yilgan kutupludur" },
	{ PLOW, "er haydash" },
	{ POKER_STICK, "poker tayoq" },
	{ BRONZE_LANCE, "bronza nishtar" },
	{ COMPOST_BOW, "kompost kamon" },
	{ FORBIDDEN_ARROW, "ta'qiqlanadi kamon o'qi" },
	{ WILDHILD_BOW, "yirtqich ayol kamon" },
	{ ODOR_SHOT, "hid bilan zarbida to'p" },
	{ BRONZE_ARROW, "bronza kamon o'qi" },
	{ PAPER_ARROW, "qog'oz kamon o'qi" },
	{ METAL_SLING, "metall tasma g'ov" },
	{ INKA_SLING, "inka tasma g'ov" },
	{ PAPER_SHOTGUN, "qog'oz ov miltiq" },
	{ HUNTING_RIFLE, "ov miltiq" },
	{ PROCESS_CARD, "jarayon karta" },
	{ ZOOM_SHOT_CROSSBOW, "g'uvullamoq bilan zarbida to'p yayli miltiq" },
	{ BALLISTA, "qadimiy yayli miltiq" },
	{ FLEECE_BOLT, "tozalamoq civatasi" },
	{ MINERAL_BOLT, "ma'dan civatasi" },
	{ PIN_BOLT, "qadab civatasi" },
	{ INKA_STINGER, "inka caqmoq" },
	{ FLIMSY_DART, "yengil nayza" },
	{ SOFT_STAR, "yumshoq yulduz" },
	{ TAR_STAR, "sol yulduz" },
	{ INKA_SHACKLE, "inka zanjir" },
	{ BULLETPROOF_CHAINWHIP, "o'q zanjir qamchi" },
	{ SECRET_WHIP, "yashirin qamchi" },
	{ BEAUTIFUL_SHIRT, "chiroyli ko'ylak" },
	{ RADIOACTIVE_UNDERGARMENT, "radioaktiv ichki kiyim" },
	{ PETA_COMPLIANT_SHIRT, "PETA-mos ko'ylak" },
	{ PARTIAL_PLATE_MAIL, "qisman plitasi pochta" },
	{ RIBBED_PLATE_MAIL, "qirrali plastinka pochta" },
	{ METAL_LAMELLAR_ARMOR, "metall lamelli zirh" },
	{ BAR_CHAIN_MAIL, "bar zanjir pochta" },
	{ TAR_CHAIN_MAIL, "sol zanjir pochta" },
	{ PEACEKEEPER_MAIL, "tinchlik posboni pochta" },
	{ GOTHIC_PLATE_MAIL, "general plitasi pochta" },
	{ EMBOSSED_PLATE_MAIL, "bo'rtib chiqqan plastinka pochta" },
	{ INKA_MITHRIL_COAT, "inka engil palto" },
	{ DROVEN_MITHRIL_COAT, "qorong'i alf engil palto" },
	{ SILK_MAIL, "ipak pochta" },
	{ HEAVY_MAIL, "og'ir pochta" },
	{ CLOAK_OF_PEACE, "tinchlik plash" },
	{ CLOAK_OF_DIMNESS, "xira plash" },
	{ ICKY_SHIELD, "hunuk qalqon" },
	{ HEAVY_SHIELD, "og'ir qalqon" },
	{ BARRIER_SHIELD, "to'siq qalqon" },
	{ TROLL_SHIELD, "troll qalqon" },
	{ TARRIER, "sol to'siq" },
	{ SHIELD_OF_PEACE, "tinchlik qalqoni" },
	{ RIN_DIMNESS, "xira" },
	{ AMULET_OF_PEACE, "amulet of tinchlik" },
	{ SECRET_KEY, "maxfiy kalit" },
	{ HAIRCLIP, "soch foydalanuvchi klip" },
	{ DATA_CHIP, "ma'lumotlar tarasha" },
	{ GRASS_WHISTLE, "o't hushtak" },
	{ FOG_HORN, "tuman shox" },
	{ CONGLOMERATE_PICK, "konglomerat terishga" },
	{ BRONZE_PICK, "bronza terishga" },
	{ GUITAR, "gitara" },
	{ PIANO, "fortepiano" },
	{ RADIOGLASSES, "radioaktiv ko'zoynaklar" },
	{ EYECLOSER, "ko'z yaqinroq" },
	{ BOSS_VISOR, "boss soyabon" },
	{ DRAGON_EYEPATCH, "ajdaho ko'z tuzatish ekin" },
	{ SOFT_CHASTITY_BELT, "yumshoq nomusidan kamar" },
	{ BINNING_KIT, "omborda saqlamoq simsiz aloqa to'plami" },
	{ BUDO_NO_SASU, "hech sasu budo" },
	{ LUBRICANT_CAN, "moy mumkin" },
	{ VERY_HEAVY_BALL, "juda og'ir to'p" },
	{ HEAVY_CHAIN, "og'ir zanjir" },
	{ HEAVY_COMPOST_BALL, "og'ir kompost to'p" },
	{ COMPOST_CHAIN, "kompost zanjir" },
	{ DISGUSTING_BALL, "jirkanch to'p" },
	{ DISGUSTING_CHAIN, "jirkanch zanjir" },
	{ HEAVY_ELASTHAN_BALL, "og'ir elastan to'p" },
	{ ELASTHAN_CHAIN, "elastan zanjir" },
	{ IMPOSSIBLY_HEAVY_NUCLEAR_BALL, "imkonsiz og'ir yadro to'p" },
	{ NUCLEAR_HOSTAGE_CHAIN, "atom garovga zanjir" },
	{ POT_DIMNESS, "xira" },
	{ SPE_CURE_DIM, "qattiqlashishi xira" },

	{ DISCONNECTED_BOOTS, "aloqasi kesilgan chizilmasin" },
	{ DISCONNECT_STONE, "uzish tosh" },
	{ AMULET_OF_SCREWY_INTERFACE, "jinni interfeysi" },
	{ SCREW_STONE, "murvat tosh" },
	{ BOSS_BOOTS, "boss chizilmasin" },
	{ BOSSFIGHT_STONE, "boss kurash tosh" },
	{ RARE_HELMET, "nodir dubulg'a" },
	{ ENTIRE_LEVEL_STONE, "butun darajasi tosh" },
	{ AMULET_OF_BONES, "suyaklar" },
	{ BONE_STONE, "suyak tosh" },
	{ RIN_AUTOCURSING, "avtomatik la'natlab so'kayotganini" },
	{ AUTOCURSE_STONE, "avtomatik la'nati tosh" },
	{ LEVELING_GLOVES, "tekislash qo'lqop" },
	{ HIGHLEVEL_STONE, "yuqori darajadagi tosh" },
	{ AMULET_OF_SPELL_FORGETTING, "unutib imlo" },
	{ SPELL_MEMORY_STONE, "xotira toshni imlo" },
	{ SOUND_EFFECT_HELMET, "tovush effekti dubulg'a" },
	{ SOUND_EFFECT_STONE, "tovush effekti tosh" },
	{ RIN_TIME_SPENDING, "vaqt sarf" },
	{ TIME_USE_STONE, "vaqt foydalanish tosh" },

	{ EVIL_DRAGON_SCALE_MAIL, "yomon ajdaho ko'lamli pochta" },
	{ EVIL_DRAGON_SCALES, "yomon ajdaho tarozilari" },
	{ MAGIC_DRAGON_SCALE_MAIL, "sehr ajdaho ko'lamli pochta" },
	{ MAGIC_DRAGON_SCALES, "sehr ajdaho tarozilari" },
	{ CHANTER_SHIRT, "qo'shiq ko'ylak" },
	{ BAD_SHIRT, "yomon ko'ylak" },
	{ DIFFICULT_SHIELD, "qiyin qalqon" },
	{ MAGICAL_SHIELD, "sehrli qalqon" },
	{ SPECIAL_SHIELD, "maxsus qalqon" },
	{ EVIL_PLATE_MAIL, "yomon plitasi pochta" },
	{ EVIL_LEATHER_ARMOR, "yomon charm zirh" },
	{ SPECIAL_LEATHER_ARMOR, "maxsus charm zirh" },
	{ MAGE_PLATE_MAIL, "kapsuellerin plitasi pochta" },
	{ EVIL_DRAGON_SCALE_SHIELD, "yomon ajdaho ko'lamli qalqon" },
	{ MAGIC_DRAGON_SCALE_SHIELD, "sehr ajdaho ko'lamli qalqon" },
	{ AMULET_OF_VULNERABILITY, "amulet of zaifliklarni" },
	{ SCR_ARTIFACT_JACKPOT, "qo'lyozmasi bir ikramiye" },
	{ SCR_BOSS_COMPANION, "bose hamroh" },

	{ RIN_LOOTCUT, "talamoq kesilgan" },
	{ LOOTCUT_STONE, "talamoq kesilgan tosh" },
	{ COMPETITION_BOOTS, "tanlov etigi" },
	{ MONSTER_SPEED_STONE, "maxluq tezligi tosh" },
	{ SCALER_MITTENS, "oelcekleyici sportchisi" },
	{ SCALING_STONE, "miqyosi-tosh" },
	{ AMULET_OF_HOSTILITY, "amulet of dushmanlik" },
	{ INIMICAL_STONE, "dushman tosh" },
	{ WHITE_SPELL_CLOAK, "oq afsun plash" },
	{ WHITE_SPELL_STONE, "oq afsun tosh" },
	{ GREYOUT_CLOAK, "chiqarib kulrang plash" },
	{ GREYOUT_STONE, "chiqarib kulrang tosh" },
	{ QUASIMODULAR_BOOTS, "go'yoki modulli etigi" },
	{ QUASAR_STONE, "yildizsi goekcismi tosh" },
	{ MOMHAT, "ona xet" },
	{ MOMMY_STONE, "oyi tosh" },
	{ CARTRIDGE_OF_HAVING_A_HORROR, "bir qo'rquv ega zapravka" },
	{ HORROR_STONE, "dahshat tosh" },
	{ AMULET_OF_EVIL_CRAFTING, "amulet of yomon san'at" },
	{ ARTIFICIAL_STONE, "sun'iy tosh" },
	{ RIN_FORM_SHIFTING, "shakli o'zgaruvchan" },
	{ WEREFORM_STONE, "edi shakli tosh" },
	{ SINFUL_HEELS, "gunohkor poshnalar" },
	{ ANTIPRAYER_STONE, "aksil namoz tosh" },
	{ TRUMP_COAT, "etdikki palto" },
	{ EVIL_PATCH_STONE, "yomon patch tosh" },
	{ BLOODSUCKING_SHOES, "qon so'rish qobiliyati poyabzal" },
	{ HARD_MODE_STONE, "qattiq rejimi tosh" },
	{ CLOAK_OF_SUDDEN_ATTACK, "to'satdan hujum plash" },
	{ SECRET_ATTACK_STONE, "maxfiy hujum tosh" },
	{ AMULET_OF_EDIBILITY, "amulet of yesa bo'ladiganlik" },
	{ EATER_STONE, "iste'molchi tosh" },
	{ COVETED_BOOTS, "hasad qilingan etigi" },
	{ COVETOUS_STONE, "baxillik tosh" },
	{ BORDERLESS_HELMET, "chegarasiz dubulg'a" },
	{ NON_SEEING_STONE, "non-ko'rib tosh" },
	{ LIGHTLESS_BOOTS, "yorug'lik kam etigi" },
	{ DARKMODE_STONE, "qorong'i rejimi tosh" },
	{ HELMET_OF_ANTI_SEARCHING, "aksil qidiruv dubulg'a" },
	{ UNFINDABLE_STONE, "topilmadi tosh" },
	{ KILLER_HEELS, "qotil poshnalar" },
	{ HOMICIDE_STONE, "o'ldirish tosh" },
	{ CLOAK_OF_BAD_TRAPPING, "yomon ta'qib plash" },
	{ MULTITRAPPING_STONE, "ko'p ta'qib tosh" },
	{ AMULET_OF_WAKING, "amulet of uyg'oq" },
	{ WAKEUP_CALL_STONE, "ogohlikka da'vat tosh" },
	{ GRAYOUT_CLOAK, "amalga kulrang plash" },
	{ GRAYOUT_STONE, "amalga kulrang tosh" },
	{ PSEUDO_TELEPORTER_CLOAK, "psevdo uzoq masofa ko'chirish xizmatlar plash" },
	{ GRAY_CENTER_STONE, "kulrang markazi tosh" },
	{ CHECKER_BOOTS, "tekshiruvi etigi" },
	{ CHECKERBOARD_STONE, "shaxmat taxtasi tosh" },
	{ CLOCKLOAK, "soat plash" },
	{ CLOCKWISE_STONE, "soat mili tosh" },
	{ HELM_OF_COUNTER_ROTATION, "hisoblagich aylanish rul" },
	{ COUNTERCLOCKWISE_STONE, "soat ko'rsatgichiga qarshi tosh" },
	{ RIN_LAGGING, "ortda qolgan" },
	{ LAG_STONE, "lag tosh" },
	{ RIN_BLESSCURSING, "la'natlay alayhi" },
	{ BLESSCURSE_STONE, "la'natni alayhi tosh" },
	{ DELIGHT_HELMET, "zavq dubulg'a" },
	{ DELIGHT_STONE, "zavq tosh" },
	{ GLOVES_OF_ENERGY_DRAINING, "energiya hayotlariga qo'lqop" },
	{ DISCHARGE_STONE, "bajarish tosh" },
	{ AMULET_OF_TRASH, "amulet of axlat" },
	{ TRASH_STONE, "axlat tosh" },
	{ MESSAGE_FILTER_HELMET, "xabar filtri dubulg'a" },
	{ FILTERING_STONE, "filtrlash tosh" },
	{ NOKEDEX_CLOAK, "hech maxluq ensiklopediya plash" },
	{ DEFORMATTING_STONE, "formatlashtirish qaytadi tosh" },
	{ FLICKER_VISOR, "titroq soyabon" },
	{ FLICKER_STRIP_STONE, "titroq strip tosh" },
	{ AMULET_OF_UNDRESSING, "amulet of choponi" },
	{ UNDRESSING_STONE, "echinish tosh" },
	{ ELVIS_SHOES, "qoya sum shohi poyabzal" },
	{ HYPER_BLUE_STONE, "hiper ish ko'k tosh" },
	{ NAYLIGHT_CLOAK, "yo'q yorug'lik plash" },
	{ NO_LIGHT_STONE, "no-yorug'lik tosh" },
	{ AIRSTEP_BOOTS, "havo qadam etigi" },
	{ PARANOIA_STONE, "paranoya tosh" },
	{ SCRIPTED_HELMET, "buyruq fayli dubulg'a" },
	{ FLEECE_STONE, "tozalamoq tosh" },
	{ BOOTS_OF_INTERRUPTION, "to'xtagan etigi" },
	{ INTERRUPTION_STONE, "to'siq tosh" },
	{ RIN_ILLITERACY, "savodsizlik" },
	{ DUSTBIN_STONE, "axlat qutisi tosh" },
	{ BATTERY_CLOAK, "batareya plash" },
	{ BATTERY_STONE, "batareya tosh" },
	{ MARY_SUE_GLOVES, "meri duk qo'lqop" },
	{ BUTTERFINGER_STONE, "sariyog 'barmoq tosh" },
	{ GAUNTLETS_OF_BAD_CASTING, "yomon to'qimalarining qo'lqop" },
	{ MISCASTING_STONE, "muvaffaqiyatsiz tashlash tosh" },
	{ EMPTY_LINE_HELMET, "bo'sh satr dubulg'a" },
	{ MESSAGE_SUPPRESSION_STONE, "xabar bostirish tosh" },
	{ CLOAK_OF_WRONG_ANNOUNCEMENT, "noto'g'ri e'lon plash" },
	{ STUCK_ANNOUNCEMENT_STONE, "xususda e'lon tosh" },
	{ STORMY_CLOAK, "bo'ronli plash" },
	{ STORM_STONE, "bo'ron tosh" },
	{ GIMP_CLOAK, "bo'yicha plash" },
	{ MAXIMUM_DAMAGE_STONE, "maksimal zarar tosh" },
	{ HIGH_HEELED_SKIERS, "baland poshnali chang'isi ustalari" },
	{ LATENCY_STONE, "kechikish tosh" },
	{ AMULET_OF_STARLIGHT, "amulet of foydalanish" },
	{ STARLIT_SKY_STONE, "yulduzlar yorug'ligida osmon tosh" },
	{ SNARENET_CLOAK, "snarenet plash" },
	{ TRAP_KNOWLEDGE_STONE, "tuzoq bilim tosh" },
	{ HIGH_SCORING_HEELS, "yuqori skoring o'tish" },
	{ HIGHSCORE_STONE, "foydalanuvchining oliy skor tosh" },
	{ PINK_SPELL_CLOAK, "pushti afsun plash" },
	{ PINK_SPELL_STONE, "pushti afsun tosh" },
	{ GREEN_SPELL_HELMET, "yashil afsun dubulg'a" },
	{ GREEN_SPELL_STONE, "yashil afsun tosh" },
	{ EVENCORE_CLOAK, "hatto yadro plash" },
	{ EVC_STONE, "hatto yadro rasm tosh" },
	{ UNDERLAYER_CLOAK, "qatlami ostida plash" },
	{ UNDERLAID_STONE, "ostida qo'ydi tosh" },
	{ METER_GAUNTLETS, "metr qo'lqop" },
	{ DAMAGE_METER_STONE, "zarar metr tosh" },
	{ WEIGHTING_GAUNTLETS, "tarozida qo'lqop" },
	{ WEIGHT_STONE, "vazn tosh" },
	{ INFOLESS_HELMET, "ma'lumot kam dubulg'a" },
	{ INFOFUCK_STONE, "ma'lumot jinsiy yaqinlik bor tosh" },
	{ BLACK_SPELL_GAUNTLETS, "qora afsun qo'lqop" },
	{ BLACK_SPELL_STONE, "qora afsun tosh" },
	{ CYAN_SPELL_CLOAK, "ko'k afsun plash" },
	{ CYAN_SPELL_STONE, "shisha kindigi afsun tosh" },
	{ REPEATABLE_BOOTS, "tekrarlanabilir etigi" },
	{ HEAP_STONE, "to'p tosh" },
	{ BLUE_SPELL_HELMET, "ko'k afsun dubulg'a" },
	{ BLUE_SPELL_STONE, "ko'k afsun tosh" },
	{ TRON_BOOTS, "panjara etigi" },
	{ TRON_STONE, "panjara tosh" },
	{ RED_SPELL_HEELS, "qizil afsun poshnalar" },
	{ RED_SPELL_STONE, "qizil afsun tosh" },
	{ HEAVY_GRABBING_GLOVES, "og'ir changallab qo'lqop" },
	{ TOO_HEAVY_STONE, "juda og'ir tosh" },
	{ ELONGATION_CLOAK, "uzaytirish plash" },
	{ ELONGATED_STONE, "cho'zilgan tosh" },
	{ GAUNTLETS_OF_REVERSE_ENCHANTME, "teskari sehrning qo'lqop" },
	{ WRAPOVER_STONE, "ustidan o'rash tosh" },
	{ DESTRUCTIVE_HEELS, "zararli poshnalar" },
	{ DESTRUCTION_STONE, "halokat tosh" },
	{ FUCKUP_MELEE_GAUNTLETS, "uchramoqda farishta qo'lqop" },
	{ MELEE_PREFIX_STONE, "farishta prefiks tosh" },
	{ MORE_HELMET, "yana dubulg'a" },
	{ AUTOMORE_STONE, "avtomatik ko'proq tosh" },
	{ UNFAIR_ATTACK_CLOAK, "adolatsiz hujum plash" },
	{ UNFAIR_ATTACK_STONE, "adolatsiz hujum tosh" },
	{ TREASURE_CHEST, "xazina ko'krak" },
	{ HELM_OF_DECONTAMINATION, "dekontaminatsiya dubulg'a" },
	{ AMULET_OF_CONTAMINATION_RESIST, "amulet of ifloslanish qarshiligi" },
	{ CLOAK_OF_CONTAMINATION_RESISTA, "ifloslanish qarshiligi plash" },
	{ SCR_EXTRA_HEALING, "qo'shimcha shifo" },
	{ SCR_GREATER_MANA_RESTORATION, "katta sehrli energiya tiklash" },
	{ CHARGER, "zaryadlovchi qurilma" },
	{ MATERIAL_KIT, "materiallar to'plami" },
	{ POT_BENEFICIAL_EFFECT, "foydali ta'sir" },
	{ POT_TRAINING, "ta'lim" },
	{ POT_RANDOM_INTRINSIC, "tasodifiy ichki" },
	{ INTELLIGENCE_PACK, "razvedka to'plami" },
	{ HELM_VERSUS_DEATH, "o'limga qarshi boshqaruvchi" },
	{ RIN_IMMUNITY_TO_DRAWBRIDGES, "cho'kmalar uchun immunitet" },
	{ AMULET_OF_NECK_BRACE, "amulet of bo'yin aloqa" },
	{ HELM_OF_BEGINNER_S_LUCK, "boshlang'ich muvaffaqiyati boshlig'i" },
	{ WOOLEN_SHIRT, "jun ko'ylak" },
	{ RIN_DISCOUNT_ACTION, "chegirmali ish" },
	{ CLOAK_OF_DISCOUNT_ACTION, "chegirmali aktsiyaning plashi" },
	{ CLOAK_OF_TECHNICALITY, "texnikaviy plash" },
	{ CLOAK_OF_FULL_NUTRITION, "to'liq ovqatlanish pardasi" },
	{ AMULET_OF_TECHNICALITY, "texniklik" },
	{ HELM_OF_FULL_NUTRITION, "to'liq oziqlanish boshlig'i" },
	{ YELLOW_LIGHTSABER, "sariq chiroqlar" },
	{ WHITE_LIGHTSABER, "oq rangli chiroqlar" },
	{ VIOLET_LIGHTSABER, "binafsha rangli chiroqlar" },
	{ WHITE_DOUBLE_LIGHTSABER, "oq juft nurli" },

	{ SCR_ASTRALCENSION, "ajralish" },
	{ SCR_RAGNAROK, "xudo timsoli" },
	{ SCR_MATERIAL_CHANGE, "moddiy o'zgarish" },
	{ SCR_OFFLEVEL_ITEM, "o'chirmoq darajasi darajasida" },
	{ STICKSHOE, "poyafzal tayoqchasi" },
	{ NOOB_POLLAX, "yangi ustun bolta" },
	{ GREAT_POLLAX, "katta qutb bolta" },
	{ WOODEN_BAR, "yog'och paneli" },

	{ ORANGE_SPELL_STONE, "to'q sariq surat" },
	{ VIOLET_SPELL_STONE, "binafsha rang sehrli tosh" },
	{ LONGING_STONE, "xushchaqchaq tosh" },
	{ CURSED_PART_STONE, "langar qism tosh" },
	{ QUAVERSAL_STONE, "kvadrat tosh" },
	{ APPEARANCE_SHUFFLING_STONE, "toshning ko'rinishi" },
	{ BROWN_SPELL_STONE, "jigarrang sehrli tosh" },
	{ CHOICELESS_STONE, "tanlovsiz tosh" },
	{ GOLDSPELL_STONE, "oltin toshli tosh" },
	{ DEPROVEMENT_STONE, "ogohlantiruvchi tosh" },
	{ INITIALIZATION_STONE, "boshlovchi tosh" },
	{ GUSHLUSH_STONE, "guruch guruch tosh" },
	{ SOILTYPE_STONE, "tuproq turi tosh" },
	{ DANGEROUS_TERRAIN_STONE, "xavfli er toshlari" },
	{ FALLOUT_STONE, "toshqin tosh" },
	{ MOJIBAKE_STONE, "mojibak tosh" },
	{ GRAVATION_STONE, "travma toshlari" },
	{ UNCALLED_STONE, "toshsiz tosh" },
	{ EXPLODING_DICE_STONE, "portlash zarbasi" },
	{ PERMACURSE_STONE, "doimiy la'nat tosh" },
	{ SHROUDED_IDENTITY_STONE, "ashyoviy dalillar toshlari" },
	{ FEELER_GAUGE_STONE, "gullarni o'lchash" },
	{ LONG_SCREWUP_STONE, "uzun vintlardek tosh" },
	{ WING_YELLOW_STONE, "sariq sariq tosh" },
	{ LIFE_SAVING_STONE, "hayot qutqaruvchi tosh" },
	{ CURSEUSE_STONE, "lafzda tosh" },
	{ CUT_NUTRITION_STONE, "kesilgan ovqatlanish toshlari" },
	{ SKILL_LOSS_STONE, "mahorat qobiliyatini yo'qotgan tosh" },
	{ AUTOPILOT_STONE, "avtopilot toshlari" },
	{ FORCE_STONE, "kuch-quvvat" },
	{ MONSTER_GLYPH_STONE, "hayvon glifi toshlari" },
	{ CHANGING_DIRECTIVE_STONE, "o'zgaruvchan direktiv tosh" },
	{ CONTAINER_KABOOM_STONE, "konteyner kabobali tosh" },
	{ STEAL_DEGRADE_STONE, "toshqini toshni o'g'irlash" },
	{ LEFT_INVENTORY_STONE, "chap inventarizatsiyalashgan tosh" },
	{ FLUCTUATING_SPEED_STONE, "tezligi o'zgaruvchan tosh" },
	{ TARMUSTROKINGNORA_STONE, "tarmokli nora tosh" },
	{ FAILURE_STONE, "qobiliyatsiz tosh" },
	{ BRIGHT_CYAN_SPELL_STONE, "yorqin jigarrang sehrli tosh" },
	{ FREQUENTATION_SPAWN_STONE, "tez-tez uchib ketuvchi tosh" },
	{ PET_AI_STONE, "uy hayvonlari A.I. tosh" },
	{ SATAN_STONE, "shayton tosh" },
	{ REMEMBERANCE_STONE, "esdalik tosh" },
	{ POKELIE_STONE, "pokeley tosh" },
	{ AUTOPICKUP_STONE, "otopikup tosh" },
	{ DYWYPI_STONE, "dyupy tosh" },
	{ SILVER_SPELL_STONE, "kumush sehrli tosh" },
	{ METAL_SPELL_STONE, "metall qalpoqli tosh" },
	{ PLATINUM_SPELL_STONE, "platina sehrli tosh" },
	{ MANLER_STONE, "toshli tosh" },
	{ DOORNING_STONE, "eshik toshlari" },
	{ NOWNSIBLE_STONE, "nanosim toshlar" },
	{ ELM_STREET_STONE, "ko'cha ko'chasi" },
	{ MONNOISE_STONE, "mono toshlar" },
	{ RANG_CALL_STONE, "chaqiriq chizig'i" },
	{ RECURRING_SPELL_LOSS_STONE, "takrorlanuvchi jilosiz toshlar" },
	{ ANTI_TRAINING_STONE, "antistatik toshlar" },
	{ TECHOUT_STONE, "toshdan yasalgan tosh" },
	{ STAT_DECAY_STONE, "stat pasayishi" },
	{ MOVEMORK_STONE, "moviy tosh" },

	{ BROWN_SHIT_CLOAK, "jigarrang bokira pardasi" },
	{ ANTICALL_CLOAK, "oldingi plash" },
	{ YELLOW_WING, "sariq qanot" },
	{ ELESDE_CLOAK, "elkada plash" },
	{ CLOAK_OF_GRAVATION, "travma plashi" },
	{ CLOAK_OF_NONFOLLOWING, "tushunmayotgan plash" },
	{ SPELL_RETAIN_CLOAK, "sehrlash plashni ushlab turish" },
	{ CLOAK_OF_FAST_DECAY, "tez parchalanish pardasi" },
	{ QUAVERSAL_HELMET, "kvars zarbasi" },
	{ HELM_OF_SHUFFLING, "aralashish boshqaruvchisi" },
	{ GOLDSPELL_HELMET, "oltin dubulg'asi" },
	{ AIRHEAD_CAP, "havo kallak qopqog'i" },
	{ FALLOUT_HELMET, "dafn marosimi" },
	{ IDENTIFY_CURD_HELMET, "kivi dubulg'asini aniqlang" },
	{ BAEAEAEP_SPY_HELMET, "baeaeaep josus dubulg'asi" },
	{ CRUNCHER_HELMET, "qisqichbaqalar kaskasi" },
	{ DISTORTED_GRIMACE, "buzuq qiyshiqlik" },
	{ ELM_ET, "qayrag'och qil" },
	{ SANEMAKER_HELMET, "ahmoq bo'ling zarbasi" },
	{ ORANGE_SPELL_GLOVES, "to'q sariq qo'lqoplar" },
	{ MOJIBAKE_GLOVES, "mojibak qo'lqop" },
	{ DIXPLOSION_GLOVES, "zar zarbasi sportchisi" },
	{ LEFT_APPENDAGE_GLOVES, "chap apendiks sportchisi" },
	{ POKELIE_GLOVES, "yolg'on gapirish sportchisi" },
	{ ALLPICKING_GLOVES, "barcha qo'lqoplar" },
	{ SILVER_SPELL_GLOVES, "kumush sehrlash qo'lqoplar" },
	{ SPELLDRAIN_GLOVES, "jarlik qo'lqoplari" },
	{ VIOLET_BEAUTY_HEELS, "violet go'zallik to'pig'i" },
	{ LONG_POINTY_HEELS, "uzoq burchak to'pi" },
	{ SELF_WILLED_HEELS, "o'z-o'zidan to'plangan to'piq" },
	{ SOIL_CLINGING_BOOTS, "tuproqni yopishtiruvchi botlar" },
	{ PERMANENTLY_BLACK_SHOES, "doimiy qora poyafzal" },
	{ AUTOSCOOTER_HEELS, "avtoskotserlar to'plami" },
	{ FORCEFUL_BOOTS, "kuchli tayoq" },
	{ BUM_BUM_BOOTS, "sershox botlarni" },
	{ SADISTIC_BOOTS, "sadist chiziqlar" },
	{ FLUCKERING_BOOTS, "chizishga chizish" },
	{ SPRAP_BOOTS, "spirt botlarni" },
	{ FAILED_ATTEMPT_BOOTS, "muvaffaqiyatsiz urinish botlari" },
	{ BRIGHT_CYAN_BEAUTIES, "yorqin moviy go'zallik" },
	{ SIA_BOOTS, "si'a poyafzallari" },
	{ PLATINUM_SPELL_BOOTS, "platina sehrlash botlarni" },
	{ INVIS_WALKERS, "ko'rinmas yuradiganlar" },
	{ NOISY_BOOTS, "shovqinli poyafzal" },
	{ MORK_BOOTS, "ko'proq vilkalar botlar" },
	{ RIN_WIMPINESS, "mo''tadillik" },
	{ RIN_USING_HAZARD, "xavf ostida" },
	{ RIN_EXERTION_LOSS, "kuch yo'qolishi" },
	{ RIN_PETCAT, "mushuk mushuk" },
	{ RIN_POSSESSION_IDENTIFICATION, "egalik identifikatori" },
	{ RIN_DAYSHIFT, "kunduzgi o'tish" },
	{ RIN_DECONSTRUCTION, "dekonstruksiya" },
	{ AMULET_OF_ITEM_INSECURITY, "amulet of element ishonchsizligi" },
	{ AMULET_OF_STOPPED_EVOLUTION, "amulet of evolyutsiyani to'xtatdi" },
	{ AMULET_OF_INITIALIZATION_FAILU, "amulet of boshlash xatosi" },
	{ AMULET_OF_REAL_SCREWERY, "amulet of haqiqiy vidolashish" },
	{ AMULET_OF_ENEMY_SAVING, "amulet of dushmanni saqlash" },
	{ AMULET_OF_INCREASED_FREQUENCY, "amulet of chastotani ko'paytirish" },
	{ AMULET_OF_SPELL_METAL, "amulet of mo''jizaviy metall" },
	{ AMULET_OF_TECHOUT, "amulet of texnologiya tashqaridan" },

	{ POT_SANITY, "aql-idrok" },
	{ POT_CURE_INSANITY, "jinnilikni davolash" },
	{ WAN_INSANITY, "jinnilik" },
	{ SCR_BAD_EQUIPMENT, "yomon uskunalar" },
	{ WAN_BAD_EQUIPMENT, "yomon uskunalar" },

	{ CLOAK_OF_BAD_PART, "yomon qismning plashi" },
	{ AMULET_OF_BAD_PART, "amulet of yomon qism" },
	{ AMULET_OF_EVIL_VARIANT, "amulet of yomon tuyalar tulusi" },
	{ BAD_PART_STONE, "yomon qismi tosh" },
	{ COMPLETELY_BAD_PART_STONE, "butunlay yomon qism tosh" },
	{ EVIL_VARIANT_STONE, "yomon har xil tosh" },

	{ SANE_TREBLE_STONE, "yupqa tosh" },
	{ STATCREASE_STONE, "toshni toshga aylantirish" },
	{ SIMEOUT_STONE, "toshdan tashqariga chiqish" },
	{ AMULET_OF_SANITY_TREBLE, "amulet of aktyorlik yuqori" },
	{ RIN_STAT_DECREASE, "holat pasayishi" },
	{ RIN_SANITY_TIMEOUT, "aql-idrok vaqt tugadi" },

	{ LIZARD_SCALE_MAIL, "kertenkele o'lchamli pochta" },
	{ LIZARD_SCALES, "kertenkele o'lchamlari" },
	{ FLAME_MOUNTAIN, "olov tog'i" },
	{ BEAM_REFLECTOR_GUN, "nurli reflektorli qurol" },
	{ TENNIS_RACKET, "tennis raketasi" },
	{ ENERGY_SAP, "po'st teskari o'girish oq osti" },

	{ SCR_NASTY_CURSE, "yomon la'nat" },
	{ SCR_HYBRIDIZATION, "hibridizatsiya" },
	{ SCR_SECURE_CURSE_REMOVAL, "xavfsiz lanetalarni yo'q qilish" },

	{ WAN_DISENCHANTMENT, "noqulaylik" },
	{ WAN_CONTAMINATION, "ifloslanish" },
	{ WAN_TREMBLING, "titroq" },

	{ SCR_MAKE_PENTAGRAM, "besh nuqta yulduz qilish" },
	{ SCR_WELL_BUILDING, "yaxshi bino" },
	{ SCR_DRIVING, "haydash" },
	{ SCR_TABLE_FURNITURE, "stol mebellari" },
	{ SCR_EMBEDDING, "joylashtirish" },
	{ SCR_MATTRESS_SLEEPING, "to'shakda yotish" },

	{ SCR_GRAVE, "qabr" },
	{ SCR_TUNNELS, "tunnellar" },
	{ SCR_FARMING, "qishloq xo'jaligi" },
	{ SCR_MOUNTAINS, "tog'lar" },
	{ SCR_DIVING, "sho'ng'in" },
	{ SCR_CRYSTALLIZATION, "kristallanish" },
	{ SCR_MOORLAND, "tog'li erlar" },
	{ SCR_URINE, "siydik" },
	{ SCR_QUICKSAND, "bilqillama qumloq" },
	{ SCR_STYX, "stiks" },
	{ SCR_SNOW, "qor" },
	{ SCR_ASH, "kul" },
	{ SCR_SAND, "qum" },
	{ SCR_PAVING, "slaydni yopish" },
	{ SCR_HIGHWAY, "avtomobil yo'li" },
	{ SCR_GRASSLAND, "chorvachilik" },
	{ SCR_NETHER, "o'shanda" },
	{ SCR_STALACTITE, "sarkitit" },
	{ SCR_CRYPT, "yer ostidagi sag'ana" },
	{ SCR_BUBBLE_BOBBLE, "kabarcikli noto'g'ri ziplatma" },
	{ SCR_RAIN, "yomg'ir" },

	{ WAN_CHAOS_TERRAIN, "betartiblik joy" },
	{ WAN_FLEECY_TERRAIN, "gavyum erlar" },

	{ SPE_GRAVE, "qabr" },
	{ SPE_TUNNELS, "tunnellar" },
	{ SPE_FARMING, "qishloq xo'jaligi" },
	{ SPE_MOUNTAINS, "tog'lar" },
	{ SPE_DIVING, "sho'ng'in" },
	{ SPE_CRYSTALLIZATION, "kristallanish" },
	{ SPE_MOORLAND, "tog'li erlar" },
	{ SPE_URINE, "siydik" },
	{ SPE_QUICKSAND, "bilqillama qumloq" },
	{ SPE_STYX, "stiks" },
	{ SPE_ATTUNE_MAGIC, "hamohang qilmoq sehrli" },
	{ SPE_SNOW, "qor" },
	{ SPE_ASH, "kul" },
	{ SPE_SAND, "qum" },
	{ SPE_PAVING, "slaydni yopish" },
	{ SPE_HIGHWAY, "avtomobil yo'li" },
	{ SPE_GRASSLAND, "chorvachilik" },
	{ SPE_NETHER_MIST, "o'tkir tuman" },
	{ SPE_STALACTITE, "sarkitit" },
	{ SPE_CRYPT, "yer ostidagi sag'ana" },
	{ SPE_BUBBLE_BOBBLE, "kabarcikli noto'g'ri ziplatma" },
	{ SPE_RAIN, "yomg'ir" },
	{ DARK_BAR, "qora satr" },
	{ YITH_TENTACLE, "paypaslagich bilan" },
	{ BOHEMIAN_EAR_SPOON, "bohem quloq qoshiq" },
	{ POLE_LANTERN, "qutbli chiroq" },
	{ NASTYPOLE, "yomon qutb" },
	{ SPE_GAIN_SPACT, "kuch-qudratga ega bo'lish" },
	{ SPE_METAL_GUARD, "metall muhofazachi" },
	{ SCR_MISSING_CODE, "etishmayotgan kod" },
	{ YOGA_PANTS, "yoga jinsida" },
	{ WINDMILL_BLADE, "shamol tegirmoni" },
	{ GRAIN_SCYTHE, "donli shiling" },
	{ ORNATE_MACE, "bezak mahsuli" },
	{ RAZOR_WHIP, "ustara qamchi" },
	{ HELM_OF_OPAQUE_THOUGHTS, "shaffof fikrlarning boshlig'i" },
	{ SECOND_SKIN, "ikkinchi teri" },
	{ CLOAK_OF_FLIGHT, "parvozning plashi" },
	{ HIDE_SHIELD, "qalqonni yashiring" },
	{ ROGUES_GLOVES, "nogironlar qo'lqoplari" },
	{ GREEN_GOWN, "yashil libos" },
	{ HELM_OF_ANTI_MAGIC, "sehrgarlikning hukmdori" },
	{ OILSKIN_COIF, "yog 'qatlami" },
	{ KALASHNIKOV, "rus avtomashinasi" },
	{ SPE_MAGIC_WHISTLING, "sehrli hushtak" },
	{ RIN_JUMPING, "o'tish" },
	{ RIN_ILLNESS, "kasallik" },

	{ SPE_ORE_MINING, "ruda qazib olish" },
	{ SPE_BOILER_KABOOM, "qozon kabeli" },
	{ SPE_DEFOG, "ham tuman" },
	{ SPE_SWAP_POSITION, "almashtirish holati" },
	{ SPE_SHUFFLE_MONSTER, "aralashuvchi hayvon" },
	{ SPE_PET_SYRINGE, "uyda ishlatiladigan shprits" },
	{ SPE_BUC_KNOWLEDGE, "muborak la'nati la'natlangan bilim" },
	{ SPE_PREACHING, "voizlik qilish" },
	{ SPE_RESIST_PARALYSIS, "falajga qarshi turish" },
	{ SPE_KEEP_SATIATION, "to'yinishni saqlang" },
	{ SPE_TECH_BOOST, "texnologiyani kuchaytirish" },
	{ SPE_CONTINGENCY, "ehtimollik" },
	{ SPE_AULE_SMITHING, "juda kuchli soqchi ishi" },
	{ SPE_HORSE_HOP, "ot otdi" },
	{ SPE_LINE_LOSS, "yo'nalish yo'qolishi" },
	{ SPE_TACTICAL_NUKE, "taktik yadro raketasi" },
	{ SPE_RAGNAROK, "xudo tongi" },
	{ SPE_ARMOR_SMASH, "qurol-yarog'" },
	{ SPE_STRANGLING, "bo'g'ilib qolgan" },
	{ SPE_PARTICLE_CANNON, "zarracha to'pi" },
	{ SPE_ONE_POINT_SHOOT, "bitta nuqta olov" },
	{ SPE_BLOOD_STREAM, "qon oqimi" },
	{ SPE_SHINING_WAVE, "to'lqin to'lqinlari" },
	{ SPE_GROUND_STOMP, "er osti suvlari" },
	{ SPE_DIRECTIVE, "direktiv" },
	{ SPE_NERVE_POISON, "asab zahari" },
	{ SPE_POWDER_SPRAY, "chang spreyi" },
	{ SPE_FIREWORKS, "havo fisekleri" },
	{ SPE_AIMBOT_LIGHTNING, "bot botishini maqsad" },
	{ SPE_ENHANCE_BREATH, "nafasni kuchaytirish" },
	{ SPE_GOUGE_DICK, "o'ymok dik" },
	{ SPE_BODYFLUID_STRENGTHENING, "jasadni mustahkamlash" },
	{ SPE_GEYSER, "geyzer" },
	{ SPE_BUBBLING_HOLE, "qabariq teshik" },
	{ SPE_PURIFICATION, "tozalash" },
	{ SPE_ADD_SPELL_MEMORY, "josus xotira qo'shing" },
	{ SPE_NEXUSPORT, "teleportatsiya elementining porti" },
	{ SPE_GIANT_FOOT, "ulkan oyoq" },
	{ SPE_ANTI_TELEPORTATION, "qarshi teleportatsiya" },
	{ FEMINISM_DRAGON_SCALES, "feminizm ajdaho tarozilari" },
	{ FEMINISM_DRAGON_SCALE_MAIL, "feminizm ajdaho miqyosidagi pochta" },
	{ FEMINISM_DRAGON_SCALE_SHIELD, "feminizm ajdaho shkalasi" },
	{ NEGATIVE_DRAGON_SCALES, "salbiy ajdaho tarozilari" },
	{ NEGATIVE_DRAGON_SCALE_MAIL, "salbiy ajdaho o'lchovli pochta" },
	{ NEGATIVE_DRAGON_SCALE_SHIELD, "salbiy ajdaho shkalasi qalqoni" },
	{ HEROIC_DRAGON_SCALES, "qahramon ajdaho tarozilari" },
	{ HEROIC_DRAGON_SCALE_MAIL, "qahramon ajdaho miqyosidagi pochta" },
	{ HEROIC_DRAGON_SCALE_SHIELD, "qahramon ajdaho shkalasi qalqoni" },
	{ LEAD_CLUMP, "qo'rg'oshin to'planishi" },
	{ BONE_FRAGMENT, "suyak bo'lagi" },
	{ HEAVY_BRICK_BALL, "og'ir g'isht to'pi" },
	{ HEAVY_PAPER_BALL, "og'ir qog'oz to'pi" },
	{ HEAVY_CHROME_BALL, "og'ir krom to'pi" },
	{ HEAVY_NANO_BALL, "og'ir nanoskal to'p" },
	{ IMPOSSIBLY_HEAVY_SAND_BALL, "ehtimol og'ir qum to'pi" },
	{ HEAVY_METAL_BALL, "og'ir metall to'p" },
	{ HEAVY_PLASTIC_BALL, "og'ir plastik to'p" },
	{ HEAVY_SHADOW_BALL, "og'ir soya to'pi" },
	{ HEAVY_LEAD_BALL, "og'ir qo'rg'oshin to'pi" },
	{ IMPOSSIBLY_HEAVY_ETHER_BALL, "mumkin bo'lmagan og'ir eter to'pi" },
	{ HEAVY_WAX_BALL, "og'ir mumi to'pi" },
	{ HEAVY_WOOD_BALL, "og'ir yog'och to'pi" },
	{ HEAVY_COPPER_BALL, "og'ir mis to'p" },
	{ HEAVY_SILVER_BALL, "og'ir kumush to'p" },
	{ IMPOSSIBLY_HEAVY_OBSIDIAN_BALL, "ehtimol vulkanik to'p" },
	{ BULKY_POURPOOR_BALL, "katta hajmdagi koptok to'p" },
	{ BULKY_GEMSTONE_BALL, "katta toshli to'p" },
	{ BULKY_FLESH_BALL, "katta go'shtli to'p" },
	{ BULKY_MITHRIL_BALL, "katta hajmli mitril to'pi" },
	{ HEAVY_CLOTH_BALL, "og'ir mato to'pi" },
	{ HEAVY_TAR_BALL, "og'ir bitum to'pi" },
	{ HEAVY_SILK_BALL, "og'ir ipak to'pi" },
	{ HEAVY_LEATHER_BALL, "og'ir charm to'p" },
	{ WONDER_BALL, "ajoyib to'p" },
	{ NULL_BALL, "to'p yo'q" },
	{ LIQUID_BALL, "suyuq to'p" },
	{ BRICK_CHAIN, "g'isht zanjiri" },
	{ PAPER_CHAIN, "qog'oz zanjiri" },
	{ CHROME_SCOURGE, "xrom ofati" },
	{ NANO_NUNCHIAKU, "nano miqyosidagi yaponiyani yo'q qilish vositasi" },
	{ SAND_HOSTAGE_CHAIN, "qumni garovga olish zanjiri" },
	{ METAL_CHAIN, "metall zanjir" },
	{ PLASTIC_CHAIN, "plastik zanjir" },
	{ SHADOW_SCOURGE, "soya ofati" },
	{ LEAD_NUNCHIAKU, "qo'rg'oshin yaponiyani zararsizlantirish vositasi" },
	{ ETHER_HOSTAGE_CHAIN, "garovga olingan efir zanjiri" },
	{ WAX_CHAIN, "mum zanjiri" },
	{ WOOD_CHAIN, "yog'och zanjiri" },
	{ COPPER_SCOURGE, "mis ofat" },
	{ SILVER_NUNCHIAKU, "kumush yaponcha zarba beruvchi vosita" },
	{ OBSIDIAN_HOSTAGE_CHAIN, "vulkanik garov zanjiri" },
	{ POURPOOR_LASH, "kirpiklarni tushiring" },
	{ GEMSTONE_LASH, "toshli kirpik" },
	{ FLESH_LASH, "go'sht kirpigi" },
	{ MITHRIL_LASH, "mitril kirpik" },
	{ CLOTH_CHAIN, "mato zanjiri" },
	{ TAR_CHAIN, "bitum zanjiri" },
	{ SILK_SCOURGE, "ipak ofat" },
	{ LEATHER_NUNCHIAKU, "charm yaponni sindiradigan vosita" },
	{ WONDER_CHAIN, "ajoyib zanjir" },
	{ NULL_CHAIN, "zanjir yo'q" },
	{ LIQUID_CHAIN, "suyuq zanjir" },
	{ ETHER_HORN, "efir shoxi" },
	{ SHADOW_HORN, "soyali shox" },
	{ CHROME_HORN, "krom shoxi" },
	{ BRICK_PICK, "g'isht terish" },
	{ NANO_PICK, "miniatyura tanlovi" },
	{ DARK_HORN, "qorong'i shox" },
	{ ARCANE_HORN, "arqon shoxi" },
	{ NANO_HAMMER, "nanoskali bolg'a" },
	{ LIGHTWHIP, "yorug'lik kemasi" },
	{ CERAMIC_KNIFE, "sopol pichoq" },
	{ NANO_AXE, "nano miqyosidagi bolta" },
	{ ETERNIUM_MATTOCK, "abadiy qazish apparati" },
	{ SAND_SWORD, "qum qilich" },
	{ VOLCANIC_BROADSWORD, "vulkanik keng to'lqinli" },
	{ HEAVY_LONG_SWORD, "og'ir uzun qilich" },
	{ AUTOMATIC_KATANA, "avtomatik yaponcha qilich" },
	{ SHADOWBLADE, "soyali pichoq" },
	{ ETHER_SAW, "eter ko'rdi" },
	{ DESERT_SWORD, "sahro qilichi" },
	{ CHROME_BLADE, "xrom pichoq" },
	{ ETERNIUM_SABER, "abadiy egilgan qilich" },
	{ DARKNESS_CLUB, "zulmat klubi" },
	{ LEAD_FILLED_MACE, "qo'rg'oshin bilan to'ldirilgan zarba tayog'i" },
	{ SHINY_STAR, "yorqin yulduz" },
	{ ETHER_KNOUT, "eterni taqillatish" },
	{ SPIKERACK, "boshoqli tokcha" },
	{ MASSIVE_STAFF, "ommaviy xodimlar" },
	{ CHROME_STAFF, "krom xodimlari" },
	{ LEADBAR, "qo'rg'oshin qutblari" },
	{ NANO_POLE, "nanoskal qutb" },
	{ ETERNAL_POLE, "abadiy qutb" },
	{ CERAMIC_SPEAR, "sopol nayza" },
	{ BRICK_MISSILE, "g'ishtli raketa" },
	{ DIFFICULT_TRIDENT, "qiyin uch qirrali tayoq" },
	{ BRICKLAYER_BAR, "g'isht teruvchi qutb" },
	{ WIND_BOW, "shamol kamon" },
	{ SNIPESLING, "snayper otish" },
	{ LEAD_BULLET, "qo'rg'oshin o'qi" },
	{ LEAD_SHOT, "qo'rg'oshin zarbasi" },
	{ POISON_BOLT, "zahar murvat" },
	{ HEAVY_CROSSBOW_BOLT, "og'ir boltli murvat" },
	{ ETHER_BOLT, "eter murvat" },
	{ SAND_DART, "qum otish o'qi" },
	{ NANO_SHURIKEN, "nano miqyosidagi otish yulduz" },
	{ DARK_BATARANG, "qorong'i batter bumerang" },
	{ ETHER_WHIP, "efir qamchi" },
	{ METAL_SHIRT, "metall ko'ylak" },
	{ FULL_LEAD_SUIT, "to'liq qo'rg'oshin kostyumi" },
	{ NANO_SUIT, "nano o'lchovli kostyum" },
	{ CLOAK_OF_COAGULATION, "koagulyatsion plash" },
	{ CLOAK_OF_SCENT, "xushbo'y hid" },
	{ SCENTY_HELMET, "xushbichim dubulg'a" },
	{ CHROME_SHIELD, "krom qalqoni" },
	{ ANTISHADOW_SHIELD, "soyaga qarshi qalqon" },
	{ LADY_BOOTS, "ayol etiklari" },
	{ STILETTO_SANDALS, "yupqa poshnali sandal" },
	{ RIN_DIMINISHED_BLEEDING, "kamaygan qon ketish" },
	{ AMULET_OF_SCENT, "amulet of hidi" },
	{ RED_STRING, "qizil simli" },
	{ TOILET_ROLL, "hojatxona rulosi" },
	{ WONDER_PLATE, "hayratlanarli plastinka" },
	{ MEDIUM_MITHRIL_COAT, "o'rta mitril palto" },
	{ TAPERED_MAIL, "lentali pochta" },
	{ EXCITING_ARMOR, "hayajonli zirh" },
	{ LORICATED_CLOAK, "rasmli plash" },
	{ BASINET, "taglik" },
	{ ROCKET_GAUNTLETS, "raketa tutqichlari" },
	{ ANCIENT_SHIELD, "qadimgi qalqon" },
	{ ITALIAN_HEELS, "italyan poshnalari" },
	{ WONDER_DAGGER, "ajablantiradigan xanjar" },
	{ UNKNOWN_KNIFE, "noma'lum pichoq" },
	{ SHARP_AXE, "o'tkir bolta" },
	{ SHORT_BLADE, "kalta pichoq" },
	{ ELEGANT_BROADSWORD, "oqlangan keng kenglik" },
	{ MAIN_SWORD, "asosiy qilich" },
	{ ROMAN_SWORD, "rimliklar qilichi" },
	{ MYTHICAL_SABLE, "afsonaviy qilich" },
	{ CRYPTIC_SABER, "sirli qilich" },
	{ BLOW_AKLYS, "aklylarni urish" },
	{ TELEPHONE, "qo'ng'iroq qilmoq" },
	{ BATLETH, "g'alati begona qurol" },
	{ ELITE_BATLETH, "elita g'alati begona qurol" },
	{ CALCULATOR, "hisoblash mashinasi" },
	{ WEIGHTED_FLAIL, "vazn toifasi" },
	{ BLOCK_HEELED_SANDAL, "g'o'la poshnali sandal" },
	{ PROSTITUTE_SHOE, "fohisha poyafzal" },
	{ BO_STAFF, "kadrlar tayyorlash" },
	{ LONG_POLE, "uzun qutb" },
	{ SHARP_POLE, "o'tkir qutb" },
	{ RANDOSPEAR, "tasodifiy nayza" },
	{ STACK_JAVELIN, "suyak javoni" },
	{ SPEC_LANCE, "maxsus nayza" },
	{ META_BOW, "o'zidan-o'zi murojaat qiladigan kamon" },
	{ WONDER_ARROW, "ajoyib o'q" },
	{ MATERIAL_BOLT, "ashyo murvat" },
	{ RANDOM_DART, "tasodifiy irg'itiladigan o'q" },
	{ CUBIC_STAR, "kub yulduz" },
	{ SEXPLAY_WHIP, "jinsiy o'yin qamchi" },
	{ MYSTERY_PICK, "sir tanlovi" },
	{ MYSTERY_LIGHTSABER, "sir chiroqlar" },
	{ SLING_AMMO, "tasma g'ov o'qi" },
	{ CANCEL_DRAGON_SCALES, "ajdaho shkalasini bekor qiling" },
	{ CANCEL_DRAGON_SCALE_MAIL, "ajdaho miqyosidagi pochtani bekor qilish" },
	{ CANCEL_DRAGON_SCALE_SHIELD, "ajdaho shkalasini qalqonni bekor qiling" },
	{ SYMBIOTE, "simbiot" },
	{ SPE_SYMHEAL, "simbiot shifo" },
	{ SCR_SYMBIOSIS, "simbioz" },
	{ SCR_TERRAFORMING, "yerga aylantiring" },
	{ SCR_INFERIOR_MATERIAL, "past material" },
	{ SCR_REGULAR_MATERIAL, "muntazam material" },
	{ SCR_SUPERIOR_MATERIAL, "ustun material" },
	{ SCR_ILLUSION, "xayol" },
	{ SCR_EVIL_VARIANT, "yomon variant" },
	{ SCR_FEMINISM, "feminizm" },
	{ WAN_BLEEDING, "qon ketishi" },
	{ WAN_UNDRESSING, "yechintirish" },
	{ SCR_HEAL_OTHER, "boshqalarni davolaydi" },
	{ CORONA_DRAGON_SCALES, "quyosh toji ajdaho tarozilari" },
	{ CORONA_DRAGON_SCALE_MAIL, "quyosh toji miqyosidagi pochta" },
	{ CORONA_DRAGON_SCALE_SHIELD, "quyosh toji shkalasi qalqoni" },

	/* todo area */


	{0, "" }
};

#endif /* OVLB */

STATIC_DCL const char *Alternate_item_name(int i, struct Jitem * );

#ifdef OVL1

STATIC_OVL char *
strprepend(s,pref)
register char *s;
register const char *pref;
{
	register int i = (int)strlen(pref);

	if(i > PREFIX) {
		impossible("PREFIX too short (for %d).", i);
		return(s);
	}
	s -= i;
	(void) strncpy(s, pref, i);	/* do not copy trailing 0 */
	return(s);
}

#endif /* OVL1 */
#ifdef OVLB

/* manage a pool of BUFSZ buffers, so callers don't have to */
static char *
nextobuf()
{
	static char NEARDATA bufs[NUMOBUF][BUFSZ];
	static int bufidx = 0;

	bufidx = (bufidx + 1) % NUMOBUF;
	return bufs[bufidx];
}

char *
obj_typename(otyp)
register int otyp;
{
	char *buf = nextobuf();
	register struct objclass *ocl = &objects[otyp];
	register const char *actualn = OBJ_NAME(*ocl);
	register const char *dn = OBJ_DESCR(*ocl);
	register const char *un = ocl->oc_uname;
	register int nn = ocl->oc_name_known;

	if (zapmrename() && Alternate_item_name(otyp,ZAPM_items))
		actualn = Alternate_item_name(otyp,ZAPM_items);
	if (Role_if(PM_SAMURAI) && Alternate_item_name(otyp,Japanese_items))
		actualn = Alternate_item_name(otyp,Japanese_items);
	if (Role_if(PM_NINJA) && Alternate_item_name(otyp,Japanese_items))
		actualn = Alternate_item_name(otyp,Japanese_items);
	if (Role_if(PM_OTAKU) && Alternate_item_name(otyp,Japanese_items))
		actualn = Alternate_item_name(otyp,Japanese_items);
	if ( (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) && !issoviet && Alternate_item_name(otyp,Pirate_items))
		actualn = Alternate_item_name(otyp,Pirate_items);
	if ( (issoviet || (uarmc && uarmc->oartifact == ART_GAGARIN_S_TRANSLATOR)) && Alternate_item_name(otyp,Soviet_items))
		actualn = Alternate_item_name(otyp,Soviet_items);
	if (Race_if(PM_ANCIENT) && !issoviet && Alternate_item_name(otyp,Ancient_items))
		actualn = Alternate_item_name(otyp,Ancient_items);
	switch(ocl->oc_class) {
	case COIN_CLASS:
		strcpy(buf, "coin");
		break;
	case POTION_CLASS:
		strcpy(buf, "potion");
		break;
	case SCROLL_CLASS:
		strcpy(buf, "scroll");
		break;
	case WAND_CLASS:
		strcpy(buf, "wand");
		break;
	case SPBOOK_CLASS:
		strcpy(buf, "spellbook");
		break;
	case RING_CLASS:
		strcpy(buf, "ring");
		break;
	case AMULET_CLASS:
		if(nn)
			strcpy(buf,actualn);
		else
			strcpy(buf,"amulet");
		if(un)
			sprintf(eos(buf)," called %s",un);
		if(dn)
			sprintf(eos(buf)," (%s)",dn);
		return(buf);
	case IMPLANT_CLASS:
		if(nn)
			strcpy(buf,actualn);
		else
			strcpy(buf,"implant");
		if(un)
			sprintf(eos(buf)," called %s",un);
		if(dn)
			sprintf(eos(buf)," (%s)",dn);
		return(buf);
	default:
		if(nn) {
			strcpy(buf, actualn);
			if (GemStone(otyp))
				strcat(buf, " stone");
			if(un)
				sprintf(eos(buf), " called %s", un);
			if(dn)
				sprintf(eos(buf), " (%s)", dn);
		} else {
			strcpy(buf, dn ? dn : actualn);
			if(ocl->oc_class == GEM_CLASS)
				strcat(buf, (ocl->oc_material == MT_MINERAL || otyp == SMALL_PIECE_OF_UNREFINED_MITHR || otyp == VOLCANIC_GLASS_FRAGMENT || otyp == LEAD_CLUMP || otyp == SLING_AMMO || otyp == BONE_FRAGMENT || otyp == SILVER_SLINGSTONE ) ?
						" stone" : " gem");
			if(un)
				sprintf(eos(buf), " called %s", un);
		}
		return(buf);
	}
	/* here for ring/scroll/potion/wand */
	if(nn) {
	    if (ocl->oc_unique)
		strcpy(buf, actualn); /* avoid spellbook of Book of the Dead */
	    /* KMH -- "mood ring" instead of "ring of mood" */
	    else if (otyp == RIN_MOOD)
		sprintf(buf, "%s ring", actualn);
	    else
		sprintf(eos(buf), " of %s", actualn);
	}
	if(un)
		sprintf(eos(buf), " called %s", un);
	if(dn)
		sprintf(eos(buf), " (%s)", dn);
	return(buf);
}

/* less verbose result than obj_typename(); either the actual name
   or the description (but not both); user-assigned name is ignored */
char *
simple_typename(otyp)
int otyp;
{
    char *bufp, *pp, *save_uname = objects[otyp].oc_uname;

    objects[otyp].oc_uname = 0;		/* suppress any name given by user */
    bufp = obj_typename(otyp);
    objects[otyp].oc_uname = save_uname;
    if ((pp = strstri(bufp, " (")) != 0)
	*pp = '\0';		/* strip the appended description */
    return bufp;
}

boolean
obj_is_pname(obj)
register struct obj *obj;
{
    return((boolean)(obj->dknown && obj->known && obj->onamelth &&
		     /* Since there aren't any objects which are both
		        artifacts and unique, the last check is redundant. */
		     (obj->oartifact || obj->fakeartifact) && !objects[obj->otyp].oc_unique));
}

/* Give the name of an object seen at a distance.  Unlike xname/doname,
 * we don't want to set dknown if it's not set already.  The kludge used is
 * to temporarily set Blind so that xname() skips the dknown setting.  This
 * assumes that we don't want to do this too often; if this function becomes
 * frequently used, it'd probably be better to pass a parameter to xname()
 * or doname() instead.
 */
char *
distant_name(obj, func)
register struct obj *obj;
char *(*func)(OBJ_P);
{
	char *str;

	long save_Blinded = Blinded;
	Blinded = 1;

	/* eyes of the overworld were overpowered enough already --Amy */
	boolean save_blindfox = flags.blindfox;
	if (!flags.blindfox) {
		flags.blindfox = 1;
	}

	str = (*func)(obj);
	Blinded = save_Blinded;
	flags.blindfox = save_blindfox;
	return str;
}

/* convert player specified fruit name into corresponding fruit juice name
   ("slice of pizza" -> "pizza juice" rather than "slice of pizza juice") */
char *
fruitname(juice)
boolean juice;	/* whether or not to append " juice" to the name */
{
    char *buf = nextobuf();
    const char *fruit_nam = strstri(pl_fruit, " of ");

    if (fruit_nam)
	fruit_nam += 4;		/* skip past " of " */
    else
	fruit_nam = pl_fruit;	/* use it as is */

    sprintf(buf, "%s%s", makesingular(fruit_nam), juice ? " juice" : "");
    return buf;
}

#endif /* OVLB */
#ifdef OVL1

char *
xname2(obj)
register struct obj *obj;
{	/* Hallu */

	if (!obj) {
		impossible("xname2 called with nonexistant object");
		return("???");
	}

	register char *buf;
	register int typ = obj->otyp;
	register struct objclass *ocl = &objects[typ];

	register int nn = ocl->oc_name_known;
	register const char *actualn = OBJ_NAME(*ocl);
	register const char *dn = OBJ_DESCR(*ocl);
	register const char *un = ocl->oc_uname;

	buf = nextobuf() + PREFIX;	/* leave room for "17 -3 " */
	if (zapmrename() && Alternate_item_name(typ,ZAPM_items))
		actualn = Alternate_item_name(typ,ZAPM_items);
	if (Role_if(PM_SAMURAI) && Alternate_item_name(typ,Japanese_items))
		actualn = Alternate_item_name(typ,Japanese_items);
	if (Role_if(PM_NINJA) && Alternate_item_name(typ,Japanese_items))
		actualn = Alternate_item_name(typ,Japanese_items);
	if (Role_if(PM_OTAKU) && Alternate_item_name(typ,Japanese_items))
		actualn = Alternate_item_name(typ,Japanese_items);
	if ( (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) && !issoviet && Alternate_item_name(typ,Pirate_items))
		actualn = Alternate_item_name(typ,Pirate_items);
	if ( (issoviet || (uarmc && uarmc->oartifact == ART_GAGARIN_S_TRANSLATOR)) && Alternate_item_name(typ,Soviet_items))
		actualn = Alternate_item_name(typ,Soviet_items);
	if (Race_if(PM_ANCIENT) && !issoviet && Alternate_item_name(typ,Ancient_items))
		actualn = Alternate_item_name(typ,Ancient_items);

	if (!dn && restoring) dn = "???";
	buf[0] = '\0';
	/*
	 * clean up known when it's tied to oc_name_known, eg after AD_DRIN
	 * This is only required for unique objects since the article
	 * printed for the object is tied to the combination of the two
	 * and printing the wrong article gives away information.
	 */

	if (!nn && ocl->oc_uses_known && ocl->oc_unique) obj->known = 0;
	if (!Blind && ((!obj->oinvis || See_invisible) && !obj->oinvisreal) ) obj->dknown = TRUE;
	if (Role_if(PM_PRIEST) || Role_if(PM_NECROMANCER) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA)) obj->bknown = TRUE;

	/* We could put a switch(obj->oclass) here but currently only this one case exists */
	if (obj->oclass == WEAPON_CLASS && obj->opoisoned)
		strcpy(buf, flags.simpledescs ? "pois " : "poisoned ");

	/*if (obj_is_pname(obj))
	    goto nameit;*/
	switch (obj->oclass) {
	    case AMULET_CLASS:
		if (!obj->dknown || PlayerUninformation)
			strcpy(buf, "amulet");
		else if (typ == AMULET_OF_YENDOR ||
			 typ == FAKE_AMULET_OF_YENDOR)
			/* each must be identified individually */
			strcpy(buf, obj->known ? actualn : dn);
		else if (nn) {
			strcpy(buf, actualn);
			if (un) {
				strcat(buf," called ");
				strcat(buf,un);
			}
		} else if (un)
			sprintf(buf,"amulet called %s", un);
		else
			sprintf(buf,"%s amulet", dn);
		break;
	    case IMPLANT_CLASS:
		if (!obj->dknown || PlayerUninformation)
			strcpy(buf, "implant");
		else if (nn) {
			strcpy(buf, actualn);
			if (un) {
				strcat(buf," called ");
				strcat(buf,un);
			}
		} else if (un)
			sprintf(buf,"implant called %s", un);
		else
			sprintf(buf,"%s implant", dn);
		break;
	    case WEAPON_CLASS:
	    case VENOM_CLASS:
	    case TOOL_CLASS:
		if (typ == MATERIAL_KIT) {
			sprintf(buf, "%s ", materialnm[obj->shirtmessage % (LASTMATERIAL + 1)]);
		}
		if (typ == LENSES)
			strcpy(buf, "pair of ");
		if (typ == RADIOGLASSES)
			strcpy(buf, "pair of ");

		if (!obj->dknown || PlayerUninformation)
			strcat(buf, dn ? dn : actualn);
		else if (nn) {
			strcat(buf, actualn);
			if (un) {
				strcat(buf," called ");
				strcat(buf,un);
			}
		} else if (un) {
			strcat(buf, dn ? dn : actualn);
			strcat(buf, " called ");
			strcat(buf, un);
		} else
			strcat(buf, dn ? dn : actualn);
		/* If we use an() here we'd have to remember never to use */
		/* it whenever calling doname() or xname(). */
		if (typ == FIGURINE && obj->corpsenm > PM_PLAYERMON && obj->corpsenm < NUMMONS && !PlayerUninformation)
		    sprintf(eos(buf), " of a%s %s",
			index(vowels,*(mons[obj->corpsenm].mname)) ? "n" : "",
			mons[obj->corpsenm].mname);
		if (typ == ENERGY_SAP && obj->corpsenm > PM_PLAYERMON && obj->corpsenm < NUMMONS && !PlayerUninformation)
		    sprintf(eos(buf), " of a%s %s",
			index(vowels,*(mons[obj->corpsenm].mname)) ? "n" : "",
			mons[obj->corpsenm].mname);

		if (is_lightsaber(obj) && nn && obj->known) sprintf(eos(buf), " (%ld)", obj->age);
		if (age_is_relative(obj) && nn && obj->known) sprintf(eos(buf), " (%ld)", obj->age);

		break;
	    case ARMOR_CLASS:
		/* depends on order of the dragon scales objects */

		/* Armor can randomly be enchanted. I don't want players without the identify spell to have to guess
		 * which armor items are enchanted, so I provide a little clue by default. --Amy */

		/* Grunthack makes it so that you must identify them to know that they have an enchantment.
		 * In the Evil Variant(TM), you must know the rustproofing, i.e. probably burn an ID scroll :P
		 * Unlike Grunthack, there are no scrolls of detect magic and the wizard doesn't get free knowledge here. */

		if (obj->enchantment && !(isevilvariant && !(obj->rknown)) && !(PlayerUninformation) )
			strcat(buf, flags.simpledescs ? "ench " : "enchanted ");

		if(obj->enchantment && !(isevilvariant && !(obj->rknown)) && obj->known && !(PlayerUninformation) ) {
			sprintf(eos(buf), flags.simpledescs ? "(%s) " : "(of %s) ", enchname(obj->enchantment) );
		}

		if (typ >= GRAY_DRAGON_SCALES && typ <= YELLOW_DRAGON_SCALES) {
			sprintf(buf, "set of %s", actualn);
			break;
		}
		if(is_boots(obj) || is_gloves(obj)) strcat(buf,"pair of ");

		if(obj->otyp >= ELVEN_SHIELD && obj->otyp <= ORCISH_SHIELD
				&& !obj->dknown) {
			strcpy(buf, "shield");
			break;
		}
		if(obj->otyp == SHIELD_OF_REFLECTION && !obj->dknown) {
			strcpy(buf, "smooth shield");
			break;
		}

		if (dn && (PlayerUninformation) )
			strcat(buf, dn);
		else if(nn)	{
			strcat(buf, actualn);
			if (un) {
				strcat(buf," called ");
				strcat(buf,un);
			}
		} else if(un && !PlayerUninformation) {
			if(is_boots(obj))
				strcpy(buf,"boots");
			else if(is_gloves(obj))
				strcpy(buf,"gloves");
			else if(is_cloak(obj))
				strcpy(buf,"cloak");
			else if(is_helmet(obj))
				strcpy(buf,"helmet");
			else if(is_shield(obj))
				strcpy(buf,"shield");
			else
				strcpy(buf,"armor");
			strcat(buf, " called ");
			strcat(buf, un);
		} else	strcat(buf, dn);
		break;
	    case FOOD_CLASS:
		if (typ == SLIME_MOLD) {
			register struct fruit *f;

			for(f=ffruit; f; f = f->nextf) {
				if(f->fid == obj->spe) {
					strcpy(buf, f->fname);
					break;
				}
			}
			if (!f) /*impossible("Bad fruit #%d?", obj->spe);*/strcpy(buf, "slime mold");
			break;
		}

		strcpy(buf, actualn);
		if (typ == TIN && obj->known && !PlayerUninformation) {
		    if(obj->spe == 1)
			strcat(buf, " of spinach");
		    else if(obj->spe == 2)
			strcat(buf, " of beans");
		    else if(obj->spe == 3)
			strcat(buf, " of peaches");
		    else if(obj->spe == 4)
			strcat(buf, " of fish");
		    else if (obj->corpsenm == NON_PM)
		        strcpy(buf, "empty tin");
		    else if (vegetarian(&mons[obj->corpsenm]))
			sprintf(eos(buf), " of %s", mons[obj->corpsenm].mname);
		    else
			sprintf(eos(buf), " of %s meat", mons[obj->corpsenm].mname);
		}
		break;
	    case COIN_CLASS:
	    case CHAIN_CLASS:
		strcpy(buf, actualn);
		break;
	    case ROCK_CLASS:
		if (typ == STATUE && obj->corpsenm > PM_PLAYERMON && obj->corpsenm < NUMMONS && !PlayerUninformation)
		    sprintf(buf, "%s%s of %s%s",
			(Role_if(PM_ARCHEOLOGIST) && (obj->spe & STATUE_HISTORIC)) ? "historic " : "" ,
			actualn,
			type_is_pname(&mons[obj->corpsenm]) ? "" :
			  (mons[obj->corpsenm].geno & G_UNIQ) ? "the " :
			    (index(vowels,*(mons[obj->corpsenm].mname)) ?
								"an " : "a "),
			(!obj->corpsenm) ? mons[u.statuetrapname].mname : mons[obj->corpsenm].mname);
		else strcpy(buf, actualn);
		break;
	    case BALL_CLASS:
		if (typ == HEAVY_IRON_BALL && !issoviet) {
			sprintf(buf, "%sheavy iron ball", (obj->owt > ocl->oc_weight) ? "very " : "");
		}
		if (typ == HEAVY_IRON_BALL && issoviet) {
			sprintf(buf, "%styazhelyy shar zheleza", (obj->owt > ocl->oc_weight) ? "ochen' " : "");
		}
		else strcpy(buf, actualn);
		break;
	    case POTION_CLASS:
		if (obj->dknown && obj->odiluted && !PlayerUninformation)
			strcpy(buf, "diluted ");
		if(nn || un || !obj->dknown || PlayerUninformation) {
			strcat(buf, "potion");
			if(!obj->dknown || PlayerUninformation) break;
			if(nn) {
			    strcat(buf, " of ");
			    if (typ == POT_WATER &&
				obj->bknown && (obj->blessed || obj->cursed)) {
				strcat(buf, obj->blessed ? "holy " : "unholy ");
			    }
			    strcat(buf, actualn);
				if (un) {
					strcat(buf," called ");
					strcat(buf,un);
				}
			} else {
				strcat(buf, " called ");
				strcat(buf, un);
			}
		} else {
			strcat(buf, dn);
			strcat(buf, " potion");
		}
		if (age_is_relative(obj) && nn && obj->known) sprintf(eos(buf), " (%ld)", obj->age);
		break;
	case SCROLL_CLASS:
		strcpy(buf, "scroll");
		if(!obj->dknown || PlayerUninformation) break;
		if(nn) {
			strcat(buf, " of ");
			strcat(buf, actualn);
			if (un) {
				strcat(buf," called ");
				strcat(buf,un);
			}
		} else if(un) {
			strcat(buf, " called ");
			strcat(buf, un);
		} else if (ocl->oc_magic) {
			strcat(buf, " labeled ");
			strcat(buf, dn);
		} else {
			strcpy(buf, dn);
			strcat(buf, " scroll");
		}
		break;
	case WAND_CLASS:
		if(!obj->dknown || PlayerUninformation)
			strcpy(buf, "wand");
		else if(nn) {
			sprintf(buf, "wand of %s", actualn);
			if (un) {
				strcat(buf," called ");
				strcat(buf,un);
			}
		} else if(un)
			sprintf(buf, "wand called %s", un);
		else
			sprintf(buf, "%s wand", dn);
		break;
	case SPBOOK_CLASS:
		if (!obj->dknown || PlayerUninformation) {
			strcpy(buf, "spellbook");
		} else if (nn) {
			if (typ != SPE_BOOK_OF_THE_DEAD)
			    strcpy(buf, "spellbook of ");
			strcat(buf, actualn);
			if (un) {
				strcat(buf," called ");
				strcat(buf,un);
			}
		} else if (un) {
			sprintf(buf, "spellbook called %s", un);
		} else
			sprintf(buf, "%s spellbook", dn);
		break;
	case RING_CLASS:
		if(!obj->dknown || PlayerUninformation)
			strcpy(buf, "ring");
		else if(nn) {
			/* KMH -- "mood ring" instead of "ring of mood" */
			if (typ == RIN_MOOD)
				sprintf(buf, "%s ring", actualn);
			else
			sprintf(buf, "ring of %s", actualn);

			if (un) {
				strcat(buf," called ");
				strcat(buf,un);
			}
		} else if(un)
			sprintf(buf, "ring called %s", un);
		else
			sprintf(buf, "%s ring", dn);
		break;
	case GEM_CLASS:
	    {
		const char *rock =
			    (ocl->oc_material == MT_MINERAL || typ == SMALL_PIECE_OF_UNREFINED_MITHR || typ == VOLCANIC_GLASS_FRAGMENT || typ == LEAD_CLUMP || typ == SLING_AMMO || typ == BONE_FRAGMENT || typ == SILVER_SLINGSTONE) ? "stone" : "gem";
		if (!obj->dknown || PlayerUninformation) {
		    strcpy(buf, rock);
		} else if (!nn) {
		    if (un) sprintf(buf,"%s called %s", rock, un);
		    else sprintf(buf, "%s %s", dn, rock);
		} else {
		    strcpy(buf, actualn);
		    if (GemStone(typ)) strcat(buf, " stone");
			if (un) {
				strcat(buf," called ");
				strcat(buf,un);
			}
		}
		break;
	    }
	default:
		sprintf(buf,"glorkum %d %d %d", obj->oclass, typ, obj->spe);
	}
	if ((obj->quan != 1L) && !PlayerUninformation) strcpy(buf, makeplural(buf));

	if (obj->onamelth && obj->dknown && !PlayerUninformation) {
		strcat(buf, " named ");
nameit:
		strcat(buf, ONAME(obj));
	}

	if (!strncmpi(buf, "the ", 4)) buf += 4;
	return(buf);
} /* end Hallu */

/* WAC calls the above xname2 */
char *
xname(obj)
register struct obj *obj;
{
/* WAC moved hallucination here */
	register struct obj *hobj;
#ifdef LINT     /* lint may handle static decl poorly -- static char bufr[]; */
	char bufr[BUFSZ];
#else
	static char bufr[BUFSZ];
#endif
	register char *buf = &(bufr[PREFIX]);   /* leave room for "17 -3 " */

	if (Hallucination && !program_state.gameover) {
		hobj = mkobj(obj->oclass, 2, FALSE);
		if (hobj) {
			hobj->quan = obj->quan;
			/* WAC clean up */
			buf = xname2(hobj);

			/* fix a VERY aggravating bug that could corrupt saves with obj_is_local and timer errors --Amy */
			if (Has_contents(hobj))
				delete_contents(hobj);

			obj_extract_self(hobj);
			dealloc_obj(hobj);

			return (buf);
		} else return xname2(obj);
	} else return xname2(obj);
}

/* xname() output augmented for multishot missile feedback */
char *
mshot_xname(obj)
struct obj *obj;
{
    char tmpbuf[BUFSZ];
    char *onm = xname(obj);

    if (m_shot.n > 1 && m_shot.o == obj->otyp) {
	/* copy xname's result so that we can reuse its return buffer */
	strcpy(tmpbuf, onm);
	/* "the Nth arrow"; value will eventually be passed to an() or
	   The(), both of which correctly handle this "the " prefix */
	sprintf(onm, "the %d%s %s", m_shot.i, ordin(m_shot.i), tmpbuf);
    }

    return onm;
}

#endif /* OVL1 */
#ifdef OVL0

/* used for naming "the unique_item" instead of "a unique_item" */
boolean
the_unique_obj(obj)
register struct obj *obj;
{
    if (!obj->dknown)
	return FALSE;
    else if (obj->otyp == FAKE_AMULET_OF_YENDOR && !obj->known)
	return TRUE;		/* lie */
    else
	return (boolean)(objects[obj->otyp].oc_unique &&
			 (obj->known || obj->otyp == AMULET_OF_YENDOR));
}

static void
add_erosion_words(obj,prefix)
struct obj *obj;
char *prefix;
{
	boolean iscrys = (obj->otyp == CRYSKNIFE);


	if (/*!is_damageable(obj) && !iscrys || */Hallucination) return;

	/* The only cases where any of these bits do double duty are for
	 * rotted food and diluted potions, which are all not is_damageable().
	 */
	if (obj->oeroded && !iscrys) {
		switch (obj->oeroded) {
			case 2:	strcat(prefix, flags.simpledescs ? "2x" : "very "); break;
			case 3:	strcat(prefix, flags.simpledescs ? "3x" : "thoroughly "); break;
		}			
		strcat(prefix, is_rustprone(obj) ? "rusty " : "burnt ");
	}
	if (obj->oeroded2 && !iscrys) {
		switch (obj->oeroded2) {
			case 2:	strcat(prefix, flags.simpledescs ? "2x" : "very "); break;
			case 3:	strcat(prefix, flags.simpledescs ? "3x" : "thoroughly "); break;
		}			
		strcat(prefix, is_corrodeable(obj) ? (flags.simpledescs ? "etched " : "corroded ") : "rotted ");
	}
	if (obj->otyp == BELL_OF_OPENING) {
		if (u.bellimbued) strcat(prefix, flags.simpledescs ? "imb " : "imbued ");
		else strcat(prefix, flags.simpledescs ? "0imb " : "non-imbued ");
	}
	if (obj->otyp == AMULET_OF_YENDOR || obj->otyp == FAKE_AMULET_OF_YENDOR) {
		if (u.amuletcompletelyimbued) strcat(prefix, flags.simpledescs ? "3ximbued " : "fully imbued ");
		else {
			int countimbues = 0;
			if (u.amuletimbued1) countimbues++;
			if (u.amuletimbued2) countimbues++;
			if (u.amuletimbued3) countimbues++;
			switch (countimbues) {
				case 0:
					strcat(prefix, flags.simpledescs ? "plain " : "non-imbued ");
					break;
				case 1:
					strcat(prefix, flags.simpledescs ? "1ximb " : "partly imbued (1 of 3) ");
					break;
				case 2:
					strcat(prefix, flags.simpledescs ? "2ximb " : "partly imbued (2 of 3) ");
					break;
				case 3:
					strcat(prefix, flags.simpledescs ? "f-imb " : "fully imbued ");
					break;
			}
		}
	}

	if (obj->rknown && obj->oerodeproof)
		strcat(prefix,
		       (iscrys || flags.simpledescs) ? "fixed " :
			/* Amy grepping target: "materialeffect" */
			(objects[(obj)->otyp].oc_material == MT_VIVA) ? "fissionproof " :
			(objects[(obj)->otyp].oc_material == MT_INKA) ? "beautified " :
			(objects[(obj)->otyp].oc_material == MT_SECREE) ? "coagulated " :
			(objects[(obj)->otyp].oc_material == MT_ARCANIUM) ? "erosionproof " :
			(objects[(obj)->otyp].oc_material == MT_COMPOST) ? "preserved " :
			(objects[(obj)->otyp].oc_material == MT_ETERNIUM) ? "everlasting " :
			(objects[(obj)->otyp].oc_material == MT_ETHER) ? "windy " :
			(objects[(obj)->otyp].oc_material == MT_NANOMACHINE) ? "laser-protected " :
			(objects[(obj)->otyp].oc_material == MT_CELESTIUM) ? "heavenly " :
			(objects[(obj)->otyp].oc_material == MT_CONUNDRUM) ? "unravelled " :
			(objects[(obj)->otyp].oc_material == MT_PWN_BUBBLE) ? "pwning " :
			(objects[(obj)->otyp].oc_material == MT_METEOSTEEL) ? "super-ammo " :
			(objects[(obj)->otyp].oc_material == MT_BRICK) ? "rock-solid " :
			(objects[(obj)->otyp].oc_material == MT_ANTIDOTIUM) ? "covid-repellant " :
		       is_rustprone(obj) ? "rustproof " :
		       is_corrodeable(obj) ? "corrodeproof " :	/* "stainless"? */
		       is_flammable(obj) ? "fireproof " : "stainless "); /* Amy edit: let's use that :-) */
}

char *
doname(obj)
register struct obj *obj;
{
	boolean ispoisoned = FALSE;
	char prefix[PREFIX];
	char tmpbuf[PREFIX+1];
	/* when we have to add something at the start of prefix instead of the
	 * end (strcat is used on the end)
	 */
	register char *bp = xname(obj);
	register int typ = obj->otyp;
	register struct objclass *ocl = &objects[typ];
	register int nn = ocl->oc_name_known;

	/* When using xname, we want "poisoned arrow", and when using
	 * doname, we want "poisoned +0 arrow".  This kludge is about the only
	 * way to do it, at least until someone overhauls xname() and doname(),
	 * combining both into one function taking a parameter.
	 */
	/* must check opoisoned--someone can have a weirdly-named fruit */
	if (!strncmp(bp, "poisoned ", 9) && obj->opoisoned) {
		bp += 9;
		ispoisoned = TRUE;
	}

	if(obj->quan != 1L)
		sprintf(prefix, "%ld ", obj->quan);
	else if (!Hallucination && (obj_is_pname(obj) || the_unique_obj(obj))) {
		if (!strncmpi(bp, "the ", 4))
		    bp += 4;
		strcpy(prefix, "the ");
	} else strcpy(prefix, "a ");

	if (obj->selfmade && !PlayerUninformation) {
		strcat(prefix, flags.simpledescs ? "own " : "selfmade ");
	}

	if (obj->oinvisreal && !PlayerUninformation) strcat(prefix,"hidden ");
	if (obj->oinvis && !obj->oinvisreal && !PlayerUninformation) strcat(prefix, flags.simpledescs ? "invis " : "invisible ");
	if (/*wizard && */is_hazy(obj) && !PlayerUninformation) strcat(prefix,"hazy ");
/* there is absolutely no reason to not display this outside of wizard mode! --Amy */

	/* warn player if items are made of glass or ether --Amy */
	/* Amy grepping target: "materialeffect" */

	if (!PlayerUninformation && obj->otyp != FAKE_AMULET_OF_YENDOR && obj->otyp != AMULET_OF_YENDOR && !Hallucination) {
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && objects[obj->otyp].oc_material == MT_GLASS && ((obj->oclass != POTION_CLASS && obj->oclass != GEM_CLASS) || flags.materialglyph) ) strcat(prefix,"* ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && objects[obj->otyp].oc_material == MT_ETHER) strcat(prefix,"! ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && objects[obj->otyp].oc_material == MT_OBSIDIAN) strcat(prefix,"^ ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_MYSTERIOUS) strcat(prefix,"0 ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_LIQUID) strcat(prefix,"} ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_WAX) strcat(prefix,") ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_VEGGY) strcat(prefix,"v-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_FLESH) strcat(prefix,"% ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_PAPER) strcat(prefix,"? ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_CLOTH) strcat(prefix,"c-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_LEATHER) strcat(prefix,"l-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_WOOD) strcat(prefix,"w-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_BONE) strcat(prefix,"b-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_DRAGON_HIDE) strcat(prefix,"D-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_IRON) strcat(prefix,"i-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_METAL) strcat(prefix,"m-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_COPPER) strcat(prefix,"C-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_SILVER) strcat(prefix,"S-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_GOLD) strcat(prefix,"g-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_PLATINUM) strcat(prefix,"P-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_MITHRIL) strcat(prefix,"M-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_PLASTIC) strcat(prefix,"p-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_GEMSTONE) strcat(prefix,"$ ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_MINERAL) strcat(prefix,"` ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_VIVA) strcat(prefix,"V-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_INKA) strcat(prefix,"I-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_TAR) strcat(prefix,"t-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_SILK) strcat(prefix,"s-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_ARCANIUM) strcat(prefix,"a-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_SECREE) strcat(prefix,"= ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_POURPOOR) strcat(prefix,"] ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_COMPOST) strcat(prefix,"o-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_ETERNIUM) strcat(prefix,"E-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_BRICK) strcat(prefix,"B-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_SAND) strcat(prefix,". ");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_SHADOWSTUFF) strcat(prefix,"d-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_LEAD) strcat(prefix,"L-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_CHROME) strcat(prefix,"8-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_CERAMIC) strcat(prefix,"7-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_NANOMACHINE) strcat(prefix,"N-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_CELESTIUM) strcat(prefix,"h-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_CONUNDRUM) strcat(prefix,"O-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_PWN_BUBBLE) strcat(prefix,"W-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_METEOSTEEL) strcat(prefix,"T-");
		if (obj->dknown && (nn || (obj->oclass != TOOL_CLASS && obj->oclass != GEM_CLASS)) && flags.materialglyph && objects[obj->otyp].oc_material == MT_ANTIDOTIUM) strcat(prefix,"A-");
	}

	if ((!Hallucination || Role_if(PM_PRIEST) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA) || Role_if(PM_NECROMANCER)) &&
	    obj->bknown && !PlayerUninformation &&
	    obj->oclass != COIN_CLASS &&
	    (obj->otyp != POT_WATER || !objects[POT_WATER].oc_name_known
		|| (!obj->cursed && !obj->blessed) || Hallucination)) {
	    /* allow 'blessed clear potion' if we don't know it's holy water;
	     * always allow "uncursed potion of water"
	     */
	    if (Hallucination ? !rn2(10) : obj->cursed) {
		if (Hallucination ? !rn2(1000) : obj->stckcurse) strcat(prefix, flags.simpledescs ? "stk " : "sticky ");
		if (Hallucination ? !rn2(1000) : obj->morgcurse) strcat(prefix, flags.simpledescs ? "morg cursed " : "morgothian cursed ");
		if (Hallucination ? !rn2(1000) : obj->evilcurse) strcat(prefix, "foul cursed ");
		if (Hallucination ? !rn2(1000) : obj->bbrcurse) strcat(prefix, flags.simpledescs ? "bbr cursed " : "blackbreath cursed ");
		if (!(obj->morgcurse || obj->evilcurse || obj->bbrcurse)) {
			if (Hallucination ? !rn2(100) : obj->prmcurse) strcat(prefix, flags.simpledescs ? "prm cursed " : "prime cursed ");
			else if (Hallucination ? !rn2(10) : obj->hvycurse) strcat(prefix, flags.simpledescs ? "hvy cursed " : "heavily cursed ");
			else strcat(prefix, "cursed ");
			}
		}
	    else if (Hallucination ? !rn2(10) : obj->blessed)
		strcat(prefix, "blessed ");
	    else strcat(prefix, flags.simpledescs ? "normal " : "uncursed "); /*if ((!obj->known || !objects[obj->otyp].oc_charged ||
		      (obj->oclass == ARMOR_CLASS ||
		       obj->oclass == RING_CLASS)) */
		/* For most items with charges or +/-, if you know how many
		 * charges are left or what the +/- is, then you must have
		 * totally identified the item, so "uncursed" is unneccesary,
		 * because an identified object not described as "blessed" or
		 * "cursed" must be uncursed.
		 *
		 * If the charges or +/- is not known, "uncursed" must be
		 * printed to avoid ambiguity between an item whose curse
		 * status is unknown, and an item known to be uncursed.
		 */
		/* Applied showbuc patch. --Amy */
/*#ifdef MAIL
			&& obj->otyp != SCR_MAIL
#endif
			&& obj->otyp != FAKE_AMULET_OF_YENDOR
			&& obj->otyp != AMULET_OF_YENDOR
			&& !Role_if(PM_PRIEST) && !Role_if(PM_CHEVALIER) && !Race_if(PM_VEELA) && !Role_if(PM_NECROMANCER))
		strcat(prefix, flags.simpledescs ? "normal " : "uncursed ");*/
	}

	if (Hallucination ? !rn2(100) : (obj->greased && !PlayerUninformation) ) strcat(prefix, (obj->greased == 3) ? (flags.simpledescs ? "3xgreased " : "thoroughly greased ") : (obj->greased == 2) ? (flags.simpledescs ? "2xgreased " : "strongly greased ") : "greased ");

	if (Hallucination ? !rn2(100) : obj->finalcancel) strcat(prefix, flags.simpledescs ? "final " : "finalized ");

	switch(obj->oclass) {
	case SCROLL_CLASS:
		if (!PlayerUninformation) add_erosion_words(obj, prefix);
		break;
	case AMULET_CLASS:
		if (!PlayerUninformation) add_erosion_words(obj, prefix);
		if(obj->owornmask & W_AMUL)
			strcat(bp, flags.simpledescs ? " (worn)" : " (being worn)");
		break;
	case IMPLANT_CLASS:
		if (!PlayerUninformation) add_erosion_words(obj, prefix);
		if(obj->owornmask & W_IMPLANT)
			strcat(bp, flags.simpledescs ? " (worn)" : " (being worn)");

		if (Hallucination)
			break;

		if(obj->known && objects[obj->otyp].oc_charged) {
			strcat(prefix, sitoa(obj->spe));
			strcat(prefix, " ");
		}

		break;
	case WEAPON_CLASS:
		if(ispoisoned && !PlayerUninformation)
			strcat(prefix, flags.simpledescs ? "pois " : "poisoned ");
plus:
		add_erosion_words(obj, prefix);
		if (Hallucination)
			break;
		if(obj->known || (Role_if(PM_DOOM_MARINE) && (is_firearm(obj) || is_bullet(obj) ) ) ) {
			strcat(prefix, sitoa(obj->spe));
			strcat(prefix, " ");
		}
		if (has_firemodes(obj)) {
			if (obj->altmode == WP_MODE_BURST) strcat(bp, " (burst)");
			else if (obj->altmode == WP_MODE_SINGLE) strcat(bp, " (single)");
			else strcat(bp, " (auto)");
		}
		if (
			is_lightsaber(obj) ||
			obj->otyp == STICK_OF_DYNAMITE) {
		    if (obj->lamplit) {
			if ((obj->otyp == RED_DOUBLE_LIGHTSABER || obj->otyp == WHITE_DOUBLE_LIGHTSABER) && obj->altmode) strcat(bp, " (2xlit)");
			else strcat(bp, " (lit)");
		    }
#  ifdef DEBUG
		    sprintf(eos(bp), " (%d)", obj->age);		
#  endif
		} else if (is_grenade(obj))
		    if (obj->oarmed) strcat(bp, " (armed)");
		break;
	case ARMOR_CLASS:
		if(obj->owornmask & W_ARMOR)
			strcat(bp, (obj == uskin) ? (flags.simpledescs ? " (skin)" : " (embedded in your skin)") :
				(flags.simpledescs ? " (worn)" : " (being worn)") );
		goto plus;
	case TOOL_CLASS:
		/* weptools already get this done when we go to the +n code */
		if (!is_weptool(obj))
		    add_erosion_words(obj, prefix);
		if (Hallucination)
			break;
		if(obj->owornmask & (W_TOOL /* blindfold */
				| W_SADDLE
				)) {
			strcat(bp, flags.simpledescs ? " (worn)" : " (being worn)");
			break;
		}
		if (obj->otyp == LEATHER_LEASH && obj->leashmon != 0) {
			strcat(bp, " (in use)");
			break;
		}
		if (obj->otyp == INKA_LEASH && obj->leashmon != 0) {
			strcat(bp, " (in use)");
			break;
		}
		if (obj->otyp == CANDELABRUM_OF_INVOCATION) {
			if (!obj->spe)
			    strcpy(tmpbuf, "no");
			else
			    sprintf(tmpbuf, "%d", obj->spe);
			sprintf(eos(bp), " (%s candle%s%s)",
				tmpbuf, plur(obj->spe),
				!obj->lamplit ? " attached" : ", lit");
			break;
		} else if (obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
			obj->otyp == BRASS_LANTERN || obj->otyp == TORCH ||
			   Is_candle(obj)) {
			if (Is_candle(obj) &&
			    /* WAC - magic candles are never "partly used" */
			    obj->otyp != MAGIC_CANDLE &&
			    obj->age < 20L * (long)objects[obj->otyp].oc_cost)
				strcat(prefix, "partly used ");
			if(obj->lamplit)
				strcat(bp, " (lit)");
		}
		if (is_weptool(obj))
			goto plus;
		if(objects[obj->otyp].oc_charged)
		    goto charges;
		break;
	case SPBOOK_CLASS: /* WAC spellbooks have charges now */
		add_erosion_words(obj, prefix);
#ifdef WIZARD
		if (wizard) {
		    if (Hallucination)
			break;
		    if (obj->known)
			sprintf(eos(bp), " (%d:%d,%d)",
			  (int)obj->recharged, obj->spe, obj->spestudied);
		    break;
		} else
#endif
		goto charges;
	case WAND_CLASS:
		add_erosion_words(obj, prefix);
charges:
		if (Hallucination)
			break;
		if(obj->known || Role_if(PM_WANDKEEPER) ) /* The charges and recharge count of wands are automatically known by the wandkeeper class. */
		    sprintf(eos(bp), " (%d:%d)", (int)obj->recharged, obj->spe);
		break;
	case POTION_CLASS:
		if (Hallucination)
			break;
		if (obj->otyp == POT_OIL && obj->lamplit)
		    strcat(bp, " (lit)");
		add_erosion_words(obj, prefix);
		break;
	case RING_CLASS:
		add_erosion_words(obj, prefix);
ring:
		if(obj->owornmask & W_RINGR) strcat(bp, flags.simpledescs ? " (right " : " (on right ");
		if(obj->owornmask & W_RINGL) strcat(bp, flags.simpledescs ? " (left " : " (on left ");
		if(obj->owornmask & W_RING) {
		    strcat(bp, body_part(HAND));
		    strcat(bp, ")");
		}
		if (Hallucination)
			break;
		if(obj->known && objects[obj->otyp].oc_charged) {
			strcat(prefix, sitoa(obj->spe));
			strcat(prefix, " ");
		}
		break;
	case FOOD_CLASS:
		add_erosion_words(obj, prefix);
		if (obj->otyp == CORPSE && obj->odrained && !PlayerUninformation) {
#ifdef WIZARD
		    if (wizard && obj->oeaten < drainlevel(obj))
			strcpy(tmpbuf, "over-drained ");
		    else
#endif
		    sprintf(tmpbuf, "%sdrained ",
		      (obj->oeaten > drainlevel(obj)) ? "partly " : "");
		}
		else if (obj->oeaten && !PlayerUninformation)
		    strcpy(tmpbuf, "partly eaten ");
		else
		    tmpbuf[0] = '\0';
		strcat(prefix, tmpbuf);
		if (obj->otyp == CORPSE && obj->corpsenm > PM_PLAYERMON && obj->corpsenm < NUMMONS && !Hallucination && !PlayerUninformation) {
		    if (mons[obj->corpsenm].geno & G_UNIQ) {
			sprintf(prefix, "%s%s ",
				(type_is_pname(&mons[obj->corpsenm]) ?
					"" : "the "),
				s_suffix(mons[obj->corpsenm].mname));
			strcat(prefix, tmpbuf);
		    } else {
			strcat(prefix, mons[obj->corpsenm].mname);
			strcat(prefix, " ");
		    }
		} else if (obj->otyp == EGG && !PlayerUninformation) {
#if 0	/* corpses don't tell if they're stale either */
		    if (obj->known && stale_egg(obj))
			strcat(prefix, "stale ");
#endif
		    if (obj->corpsenm >= LOW_PM &&
			    (obj->known ||
			    mvitals[obj->corpsenm].mvflags & MV_KNOWS_EGG)) {
			strcat(prefix, mons[obj->corpsenm].mname);
			strcat(prefix, " ");
			if (obj->spe == 2)
			    strcat(bp, flags.simpledescs ? " (mark)" : " (with your markings)");
			else if (obj->spe)
			    strcat(bp, flags.simpledescs ? " (yours)" : " (laid by you)");
		    }
		}
		if (obj->otyp == MEAT_RING) goto ring;
		break;
	case VENOM_CLASS:
	case ROCK_CLASS:
		add_erosion_words(obj, prefix);
		break;
	case BALL_CLASS:
	case CHAIN_CLASS:
		goto plus;
		add_erosion_words(obj, prefix);
		if(obj->owornmask & W_BALL)
			strcat(bp, flags.simpledescs ? " (chained)" : " (chained to you)");
			break;
	case GEM_CLASS:
		/*add_erosion_words(obj, prefix);*/
		goto plus;
		break;
	}
	if((obj->owornmask & W_WEP) && !mrg_to_wielded) {
		if (obj->quan != 1L) {
			strcat(bp, " (wielded)");
		} else {
			const char *hand_s = body_part(HAND);

			if (bimanual(obj)) hand_s = makeplural(hand_s);
			sprintf(eos(bp), flags.simpledescs ? " (%s)" : " (weapon in %s)", hand_s);
		}
	}
	if(obj->owornmask & W_SWAPWEP) {
		if (u.twoweap)
			sprintf(eos(bp), flags.simpledescs ? " (%s2)" : " (wielded in other %s)", body_part(HAND));
		else
			strcat(bp, flags.simpledescs ? " (swap)" : " (alternate weapon; not wielded)");
	}
	if(obj->owornmask & W_QUIVER) strcat(bp, flags.simpledescs ? " (quiver)" : " (in quiver)");
	if (!Hallucination && obj->unpaid) {
		xchar ox, oy; 
		long quotedprice = unpaid_cost(obj);
		struct monst *shkp = (struct monst *)0;

		if (Has_contents(obj) &&
		    get_obj_location(obj, &ox, &oy, BURIED_TOO|CONTAINED_TOO) &&
		    costly_spot(ox, oy) &&
		    (shkp = shop_keeper(*in_rooms(ox, oy, SHOPBASE))))
			quotedprice += contained_cost(obj, shkp, 0L, FALSE, TRUE);
		sprintf(eos(bp), " (unpaid, %ld %s)",
			quotedprice, currency(quotedprice));
	}
#ifdef WIZARD
	if (wizard && obj->in_use)	/* Can't use "(in use)", see leashes */
		strcat(bp, " (finishing)");	/* always a bug */
#endif
	if (!strncmp(prefix, "a ", 2) &&
			index(vowels, *(prefix+2) ? *(prefix+2) : *bp)
			&& (*(prefix+2) || (strncmp(bp, "uranium", 7)
				&& strncmp(bp, "unicorn", 7)
				&& strncmp(bp, "eucalyptus", 10)))) {
		strcpy(tmpbuf, prefix);
		strcpy(prefix, "an ");
		strcpy(prefix+3, tmpbuf+2);
	}

#ifdef SHOW_WEIGHT
	  /* [max] weight inventory */
	if ((obj->otyp != BOULDER) || !throws_rocks (youmonst.data))
	  if ((obj->otyp <= ACID_VENOM) /* && (obj->otyp != CHEST) && (obj->otyp != LARGE_BOX) && */
&& (obj->otyp != LUCKSTONE) && (obj->otyp != HEALTHSTONE) && (obj->otyp != LOADSTONE) && (obj->otyp != TOUCHSTONE)
&& (obj->otyp != WHETSTONE) && (obj->otyp != MANASTONE) && (obj->otyp != SLEEPSTONE) && (obj->otyp != LOADBOULDER) && (obj->otyp != STARLIGHTSTONE) && (obj->otyp != TALC) && (obj->otyp != GRAPHITE) && (obj->otyp != BONE_FRAGMENT) && (obj->otyp != LEAD_CLUMP) && (obj->otyp != SLING_AMMO) && (obj->otyp != VOLCANIC_GLASS_FRAGMENT) && (obj->otyp != STONE_OF_MAGIC_RESISTANCE) && (obj->otyp != FLINT) && (obj->otyp != SALT_CHUNK) && (obj->otyp != SILVER_SLINGSTONE) && (obj->otyp != SMALL_PIECE_OF_UNREFINED_MITHR) && (obj->otyp != AMULET_OF_YENDOR) && (obj->otyp != FAKE_AMULET_OF_YENDOR) && (!is_nastygraystone(obj))
	      /*(obj->otyp != ICE_BOX) */ && (!Hallucination && flags.invweight && !WeightDisplayIsArbitrary))
		        sprintf (eos(bp), " {%d}", obj->owt);
/* show the freaking weight of all items! --Amy */
#endif

	bp = strprepend(bp, prefix);
	return(bp);
}

#endif /* OVL0 */
#ifdef OVLB

/* used from invent.c */
boolean
not_fully_identified(otmp)
register struct obj *otmp;
{
#ifdef GOLDOBJ
    /* gold doesn't have any interesting attributes [yet?] */
    if (otmp->oclass == COIN_CLASS) return FALSE;	/* always fully ID'd */
#endif
    /* check fundamental ID hallmarks first */
    if (!otmp->known || !otmp->dknown ||
#ifdef MAIL
	    (!otmp->bknown && otmp->otyp != SCR_MAIL) ||
#else
	    !otmp->bknown ||
#endif
	    !objects[otmp->otyp].oc_name_known)	/* ?redundant? */
	return TRUE;
    if (otmp->oartifact && undiscovered_artifact(otmp->oartifact))
	return TRUE;
    /* otmp->rknown is the only item of interest if we reach here */
       /*
	*  Note:  if a revision ever allows scrolls to become fireproof or
	*  rings to become shockproof, this checking will need to be revised.
	*  `rknown' ID only matters if xname() will provide the info about it.
	*/
    if (otmp->rknown || (otmp->oclass != ARMOR_CLASS &&
			 otmp->oclass != WEAPON_CLASS &&
			 !is_weptool(otmp) &&		    /* (redunant) */
			 otmp->oclass != BALL_CLASS))	    /* (useless) */
	return FALSE;
    else	/* lack of `rknown' only matters for vulnerable objects */
	return (boolean)(is_rustprone(otmp) ||
			 is_corrodeable(otmp) ||
			 is_flammable(otmp));
}

char *
corpse_xname(otmp, ignore_oquan)
struct obj *otmp;
boolean ignore_oquan;	/* to force singular */
{
	char *nambuf = nextobuf();

	sprintf(nambuf, "%s corpse",
		mons[Hallucination ? rndmonnum() : otmp->corpsenm].mname);

	if (ignore_oquan || otmp->quan < 2)
	    return nambuf;
	else
	    return makeplural(nambuf);
}

/* xname, unless it's a corpse, then corpse_xname(obj, FALSE) */
char *
cxname(obj)
struct obj *obj;
{
	if (obj->otyp == CORPSE)
	    return corpse_xname(obj, FALSE);
	return xname(obj);
}

char *
cxname2(obj)
struct obj *obj;
{
       if (obj->otyp == CORPSE)
           return corpse_xname(obj, TRUE);
       return xname2(obj);
}

/* treat an object as fully ID'd when it might be used as reason for death */
char *
killer_xname(obj)
struct obj *obj;
{
    struct obj save_obj;
    unsigned save_ocknown;
    char *buf, *save_ocuname;

    /* remember original settings for core of the object;
       oname and oattached extensions don't matter here--since they
       aren't modified they don't need to be saved and restored */
    save_obj = *obj;
    /* killer name should be more specific than general xname; however, exact
       info like blessed/cursed and rustproof makes things be too verbose */
    obj->known = obj->dknown = 1;
    obj->bknown = obj->rknown = obj->greased = 0;
    /* if character is a priest[ess], bknown will get toggled back on */
    obj->blessed = obj->cursed = obj->hvycurse = obj->prmcurse = obj->morgcurse = obj->evilcurse = obj->bbrcurse = obj->stckcurse = 0;
    /* "killed by poisoned <obj>" would be misleading when poison is
       not the cause of death and "poisoned by poisoned <obj>" would
       be redundant when it is, so suppress "poisoned" prefix */
    obj->opoisoned = 0;
    /* strip user-supplied name; artifacts keep theirs */
    if (!obj->oartifact) obj->onamelth = 0;
    /* temporarily identify the type of object */
    save_ocknown = objects[obj->otyp].oc_name_known;
    objects[obj->otyp].oc_name_known = 1;
    save_ocuname = objects[obj->otyp].oc_uname;
    objects[obj->otyp].oc_uname = 0;	/* avoid "foo called bar" */

    buf = xname2(obj);
    if (obj->quan == 1L) buf = obj_is_pname(obj) ? the(buf) : an(buf);

    objects[obj->otyp].oc_name_known = save_ocknown;
    objects[obj->otyp].oc_uname = save_ocuname;
    *obj = save_obj;	/* restore object's core settings */

    return buf;
}

char *
killer_cxname(obj, ignore_oquan)
struct obj *obj;
boolean ignore_oquan;	/* to force singular */
{
    char *buf;
    if (obj->otyp == CORPSE) {
	buf = nextobuf();

	sprintf(buf, "%s%s corpse",
		Hallucination ? "hallucinogen-distorted " : "",
		mons[obj->corpsenm].mname);

	if (!ignore_oquan && obj->quan >= 2)
	    buf = makeplural(buf);
    } else
	buf = killer_xname(obj);
    return buf;
}

/*
 * Used if only one of a collection of objects is named (e.g. in eat.c).
 */
const char *
singular(otmp, func)
register struct obj *otmp;
char *(*func)(OBJ_P);
{
	long savequan;
#ifdef SHOW_WEIGHT
	unsigned saveowt;
#endif
	char *nam;

	/* Note: using xname for corpses will not give the monster type */
	if (otmp->otyp == CORPSE && func == xname && !Hallucination)
		return corpse_xname(otmp, TRUE);

	savequan = otmp->quan;
	otmp->quan = 1L;
#ifdef SHOW_WEIGHT
	saveowt = otmp->owt;
	otmp->owt = weight(otmp);
#endif
	nam = (*func)(otmp);
	otmp->quan = savequan;
#ifdef SHOW_WEIGHT
	otmp->owt = saveowt;
#endif
	return nam;
}

char *
an(str)
register const char *str;
{
	char *buf = nextobuf();

	buf[0] = '\0';

	if (strncmpi(str, "the ", 4) &&
	    strcmp(str, "molten lava") &&
	    strcmp(str, "iron bars") &&
	    strcmp(str, "ice")) {
		if (index(vowels, *str) &&
		    strncmp(str, "one-", 4) &&
		    strncmp(str, "useful", 6) &&
		    strncmp(str, "unicorn", 7) &&
		    strncmp(str, "uranium", 7) &&
		    strncmp(str, "eucalyptus", 10))
			strcpy(buf, "an ");
		else
			strcpy(buf, "a ");
	}

	strcat(buf, str);
	return buf;
}

char *
An(str)
const char *str;
{
	register char *tmp = an(str);
	*tmp = highc(*tmp);
	return tmp;
}

/*
 * Prepend "the" if necessary; assumes str is a subject derived from xname.
 * Use type_is_pname() for monster names, not the().  the() is idempotent.
 */
char *
the(str)
const char *str;
{
	char *buf = nextobuf();
	boolean insert_the = FALSE;

	if (!strncmpi(str, "the ", 4)) {
	    buf[0] = lowc(*str);
	    strcpy(&buf[1], str+1);
	    return buf;
	} else if (*str < 'A' || *str > 'Z') {
	    /* not a proper name, needs an article */
	    insert_the = TRUE;
	} else {
	    /* Probably a proper name, might not need an article */
	    register char *tmp, *named, *called;
	    int l;

	    /* some objects have capitalized adjectives in their names */
	    if(((tmp = rindex(str, ' ')) || (tmp = rindex(str, '-'))) &&
	       (tmp[1] < 'A' || tmp[1] > 'Z'))
		insert_the = TRUE;
	    else if (tmp && index(str, ' ') < tmp) {	/* has spaces */
		/* it needs an article if the name contains "of" */
		tmp = strstri(str, " of ");
		named = strstri(str, " named ");
		called = strstri(str, " called ");
		if (called && (!named || called < named)) named = called;

		if (tmp && (!named || tmp < named))	/* found an "of" */
		    insert_the = TRUE;
		/* stupid special case: lacks "of" but needs "the" */
		else if (!named && (l = strlen(str)) >= 31 &&
		      !strcmp(&str[l - 31], "Platinum Yendorian Express Card"))
		    insert_the = TRUE;
	    }
	}
	if (insert_the)
	    strcpy(buf, "the ");
	else
	    buf[0] = '\0';
	strcat(buf, str);

	return buf;
}

char *
The(str)
const char *str;
{
    register char *tmp = the(str);
    *tmp = highc(*tmp);
    return tmp;
}

/* returns "count cxname(otmp)" or just cxname(otmp) if count == 1 */
char *
aobjnam(otmp,verb)
register struct obj *otmp;
register const char *verb;
{
	register char *bp = cxname(otmp);
	char prefix[PREFIX];

	if(otmp->quan != 1L) {
		sprintf(prefix, "%ld ", otmp->quan);
		bp = strprepend(bp, prefix);
	}

	if(verb) {
	    strcat(bp, " ");
	    strcat(bp, otense(otmp, verb));
	}
	return(bp);
}

/* like aobjnam, but prepend "The", not count, and use xname */
char *
Tobjnam(otmp, verb)
register struct obj *otmp;
register const char *verb;
{
	char *bp = The(xname(otmp));

	if(verb) {
	    strcat(bp, " ");
	    strcat(bp, otense(otmp, verb));
	}
	return(bp);
}

/* return form of the verb (input plural) if xname(otmp) were the subject */
char *
otense(otmp, verb)
register struct obj *otmp;
register const char *verb;
{
	char *buf;

	/*
	 * verb is given in plural (without trailing s).  Return as input
	 * if the result of xname(otmp) would be plural.  Don't bother
	 * recomputing xname(otmp) at this time.
	 */
	if (!is_plural(otmp))
	    return vtense((char *)0, verb);

	buf = nextobuf();
	strcpy(buf, verb);
	return buf;
}

/* various singular words that vtense would otherwise categorize as plural */
static const char * const special_subjs[] = {
	"erinys",
	"manes",		/* this one is ambiguous */
	"Cyclops",
	"Hippocrates",
	"Pelias",
	"aklys",
	"amnesia",
	"paralysis",
	0
};

/* return form of the verb (input plural) for present tense 3rd person subj */
char *
vtense(subj, verb)
register const char *subj;
register const char *verb;
{
	char *buf = nextobuf();
	int len, ltmp;
	const char *sp, *spot;
	const char * const *spec;

	/*
	 * verb is given in plural (without trailing s).  Return as input
	 * if subj appears to be plural.  Add special cases as necessary.
	 * Many hard cases can already be handled by using otense() instead.
	 * If this gets much bigger, consider decomposing makeplural.
	 * Note: monster names are not expected here (except before corpse).
	 *
	 * special case: allow null sobj to get the singular 3rd person
	 * present tense form so we don't duplicate this code elsewhere.
	 */
	if( ( Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) && !strcmp(verb,"are")) {
		strcpy(buf,"be");
		return buf;
	}

	if (subj) {
	    if (!strncmpi(subj, "a ", 2) || !strncmpi(subj, "an ", 3))
		goto sing;
	    spot = (const char *)0;
	    for (sp = subj; (sp = index(sp, ' ')) != 0; ++sp) {
		if (!strncmp(sp, " of ", 4) ||
		    !strncmp(sp, " from ", 6) ||
		    !strncmp(sp, " called ", 8) ||
		    !strncmp(sp, " named ", 7) ||
		    !strncmp(sp, " labeled ", 9)) {
		    if (sp != subj) spot = sp - 1;
		    break;
		}
	    }
	    len = (int) strlen(subj);
	    if (!spot) spot = subj + len - 1;

	    /*
	     * plural: anything that ends in 's', but not '*us' or '*ss'.
	     * Guess at a few other special cases that makeplural creates.
	     */
	    if ((*spot == 's' && spot != subj &&
			(*(spot-1) != 'u' && *(spot-1) != 's')) ||
		((spot - subj) >= 4 && !strncmp(spot-3, "eeth", 4)) ||
		((spot - subj) >= 3 && !strncmp(spot-3, "feet", 4)) ||
		((spot - subj) >= 2 && !strncmp(spot-1, "ia", 2)) ||
		((spot - subj) >= 2 && !strncmp(spot-1, "ae", 2))) {
		/* check for special cases to avoid false matches */
		len = (int)(spot - subj) + 1;
		for (spec = special_subjs; *spec; spec++) {
		    ltmp = strlen(*spec);
		    if (len == ltmp && !strncmpi(*spec, subj, len)) goto sing;
		    /* also check for <prefix><space><special_subj>
		       to catch things like "the invisible erinys" */
		    if (len > ltmp && *(spot - ltmp) == ' ' &&
			   !strncmpi(*spec, spot - ltmp + 1, ltmp)) goto sing;
		}

		return strcpy(buf, verb);
	    }
	    /*
	     * 3rd person plural doesn't end in telltale 's';
	     * 2nd person singular behaves as if plural.
	     */
	    if (!strcmpi(subj, "they") || !strcmpi(subj, "you"))
		return strcpy(buf, verb);
	}

 sing:
	len = strlen(verb);
	spot = verb + len - 1;

	if (!strcmp(verb, "are"))
	    strcpy(buf, "is");
	else if (!strcmp(verb, "have"))
	    strcpy(buf, "has");
	else if (index("zxs", *spot) ||
		 (len >= 2 && *spot=='h' && index("cs", *(spot-1))) ||
		 (len == 2 && *spot == 'o')) {
	    /* Ends in z, x, s, ch, sh; add an "es" */
	    strcpy(buf, verb);
	    strcat(buf, "es");
	} else if (*spot == 'y' && (!index(vowels, *(spot-1)))) {
	    /* like "y" case in makeplural */
	    strcpy(buf, verb);
	    strcpy(buf + len - 1, "ies");
	} else {
	    strcpy(buf, verb);
	    strcat(buf, "s");
	}

	return buf;
}

/* capitalized variant of doname() */
char *
Doname2(obj)
register struct obj *obj;
{
	register char *s = doname(obj);

	*s = highc(*s);
	return(s);
}

/* returns "your xname(obj)" or "Foobar's xname(obj)" or "the xname(obj)" */
char *
yname(obj)
struct obj *obj;
{
	char *outbuf = nextobuf();
	char *s = shk_your(outbuf, obj);	/* assert( s == outbuf ); */
	int space_left = BUFSZ - strlen(s) - sizeof " ";

	return strncat(strcat(s, " "), cxname(obj), space_left);
}

/* capitalized variant of yname() */
char *
Yname2(obj)
struct obj *obj;
{
	char *s = yname(obj);

	*s = highc(*s);
	return s;
}

/* returns "your simple_typename(obj->otyp)"
 * or "Foobar's simple_typename(obj->otyp)"
 * or "the simple_typename(obj-otyp)"
 */
char *
ysimple_name(obj)
struct obj *obj;
{
	char *outbuf = nextobuf();
	char *s = shk_your(outbuf, obj);	/* assert( s == outbuf ); */
	int space_left = BUFSZ - strlen(s) - sizeof " ";

	return strncat(strcat(s, " "), simple_typename(obj->otyp), space_left);
}

/* capitalized variant of ysimple_name() */
char *
Ysimple_name2(obj)
struct obj *obj;
{
	char *s = ysimple_name(obj);

	*s = highc(*s);
	return s;
}

static const char *wrp[] = {
	"wand", "ring", "potion", "scroll", "gem", "amulet", "implant",
	"spellbook", "spell book",
	/* for non-specific wishes */
	"weapon", "armor", "armour", "tool", "food", "comestible",
};
static const char wrpsym[] = {
	WAND_CLASS, RING_CLASS, POTION_CLASS, SCROLL_CLASS, GEM_CLASS,
	AMULET_CLASS, IMPLANT_CLASS, SPBOOK_CLASS, SPBOOK_CLASS,
	WEAPON_CLASS, ARMOR_CLASS, ARMOR_CLASS, TOOL_CLASS, FOOD_CLASS,
	FOOD_CLASS
};

#endif /* OVLB */
#ifdef OVL0

/* Plural routine; chiefly used for user-defined fruits.  We have to try to
 * account for everything reasonable the player has; something unreasonable
 * can still break the code.  However, it's still a lot more accurate than
 * "just add an s at the end", which Rogue uses...
 *
 * Also used for plural monster names ("Wiped out all homunculi.")
 * and body parts.
 *
 * Also misused by muse.c to convert 1st person present verbs to 2nd person.
 */
char *
makeplural(oldstr)
const char *oldstr;
{
	/* Note: cannot use strcmpi here -- it'd give MATZot, CAVEMeN,... */
	register char *spot;
	char *str = nextobuf();
	const char *excess = (char *)0;
	int len;

	while (*oldstr==' ') oldstr++;
	if (!oldstr || !*oldstr) {
		impossible("plural of null?");
		strcpy(str, "s");
		return str;
	}
	strcpy(str, oldstr);

	/*
	 * Skip changing "pair of" to "pairs of".  According to Webster, usual
	 * English usage is use pairs for humans, e.g. 3 pairs of dancers,
	 * and pair for objects and non-humans, e.g. 3 pair of boots.  We don't
	 * refer to pairs of humans in this game so just skip to the bottom.
	 */
	if (!strncmp(str, "pair of ", 8))
		goto bottom;

	/* Search for common compounds, ex. lump of royal jelly */
	for(spot=str; *spot; spot++) {
		if (!strncmp(spot, " of ", 4)
				|| !strncmp(spot, " labeled ", 9)
				|| !strncmp(spot, " called ", 8)
				|| !strncmp(spot, " named ", 7)
				|| !strcmp(spot, " above") /* lurkers above */
				|| !strncmp(spot, " versus ", 8)
				|| !strncmp(spot, " from ", 6)
				|| !strncmp(spot, " in ", 4)
				|| !strncmp(spot, " on ", 4)
				|| !strncmp(spot, " a la ", 6)
				|| !strncmp(spot, " with", 5)	/* " with "? */
				|| !strncmp(spot, " de ", 4)
				|| !strncmp(spot, " d'", 3)
				|| !strncmp(spot, " du ", 4)) {
			excess = oldstr + (int) (spot - str);
			*spot = 0;
			break;
		}
	}
	spot--;
	while (*spot==' ') spot--; /* Strip blanks from end */
	*(spot+1) = 0;
	/* Now spot is the last character of the string */

	len = strlen(str);

	/* Single letters */
	if (len==1 || !letter(*spot)) {
		strcpy(spot+1, "'s");
		goto bottom;
	}

	/* Same singular and plural; mostly Japanese words except for "manes" */
	if ((len == 2 && !strcmp(str, "ya")) ||
	    (len >= 2 && !strcmp(spot-1, "ai")) || /* samurai, Uruk-hai */
	    (len >= 3 && !strcmp(spot-2, " ya")) ||
	    (len >= 4 &&
	     (!strcmp(spot-3, "fish") || !strcmp(spot-3, "tuna") ||
	      !strcmp(spot-3, "deer") || !strcmp(spot-3, "yaki") ||
	      !strcmp(spot-3, "drow"))) ||
	    (len >= 5 && (!strcmp(spot-4, "sheep") ||
			!strcmp(spot-4, "ninja") ||
			!strcmp(spot-4, "shito") ||
			!strcmp(spot-7, "shuriken") ||
			!strcmp(spot-4, "tengu") ||
			!strcmp(spot-4, "manes"))) ||
	    (len >= 6 && (!strcmp(spot-5, "ki-rin") ||
			!strcmp(spot-5, "Nazgul"))) ||
	    (len >= 7 && !strcmp(spot-6, "gunyoki")))
		goto bottom;

	/* man/men ("Wiped out all cavemen.") */
	if (len >= 3 && !strcmp(spot-2, "man") &&
			(len<6 || strcmp(spot-5, "shaman")) &&
			(len<5 || strcmp(spot-4, "human"))) {
		*(spot-1) = 'e';
		goto bottom;
	}

	/* tooth/teeth */
	if (len >= 5 && !strcmp(spot-4, "tooth")) {
		strcpy(spot-3, "eeth");
		goto bottom;
	}

	/* knife/knives, etc... */
	if (!strcmp(spot-1, "fe")) {
		strcpy(spot-1, "ves");
		goto bottom;
	} else if (*spot == 'f') {
		if (index("lr", *(spot-1)) || index(vowels, *(spot-1))) {
			strcpy(spot, "ves");
			goto bottom;
		} else if (len >= 5 && !strncmp(spot-4, "staf", 4)) {
			strcpy(spot-1, "ves");
			goto bottom;
		}
	}

	/* foot/feet (body part) */
	if (len >= 4 && !strcmp(spot-3, "foot")) {
		strcpy(spot-2, "eet");
		goto bottom;
	}

	/* ium/ia (mycelia, baluchitheria) */
	if (len >= 3 && !strcmp(spot-2, "ium")) {
		*(spot--) = (char)0;
		*spot = 'a';
		goto bottom;
	}

	/* algae, larvae, hyphae (another fungus part) */
	if ((len >= 4 && !strcmp(spot-3, "alga")) ||
	    (len >= 5 &&
	     (!strcmp(spot-4, "hypha") || !strcmp(spot-4, "larva")))) {
		strcpy(spot, "ae");
		goto bottom;
	}

	/* fungus/fungi, homunculus/homunculi, but buses, lotuses, wumpuses */
	if (len > 3 && !strcmp(spot-1, "us") &&
	    (len < 5 || (strcmp(spot-4, "lotus") &&
			 (len < 6 || strcmp(spot-5, "wumpus"))))) {
		*(spot--) = (char)0;
		*spot = 'i';
		goto bottom;
	}

	/* vortex/vortices */
	if (len >= 6 && !strcmp(spot-3, "rtex")) {
		strcpy(spot-1, "ices");
		goto bottom;
	}

	/* sephirah/sephiroth */
	if (len >= 8 && !strcmp(spot-7, "sephirah")) {
		strcpy(spot-7, "sephiroth");
		goto bottom;
	}

	/* djinni/djinn (note: also efreeti/efreet) */
	if (len >= 6 && !strcmp(spot-5, "djinni")) {
		*spot = (char)0;
		goto bottom;
	}

	/* mumak/mumakil */
	if (len >= 5 && !strcmp(spot-4, "mumak")) {
		strcpy(spot+1, "il");
		goto bottom;
	}

	/* sis/ses (nemesis) */
	if (len >= 3 && !strcmp(spot-2, "sis")) {
		*(spot-1) = 'e';
		goto bottom;
	}

	/* erinys/erinyes */
	if (len >= 6 && !strcmp(spot-5, "erinys")) {
		strcpy(spot, "es");
		goto bottom;
	}

	/* mouse/mice,louse/lice (not a monster, but possible in food names) */
	if (len >= 5 && !strcmp(spot-3, "ouse") && index("MmLl", *(spot-4))) {
		strcpy(spot-3, "ice");
		goto bottom;
	}

	/* matzoh/matzot, possible food name */
	if (len >= 6 && (!strcmp(spot-5, "matzoh")
					|| !strcmp(spot-5, "matzah"))) {
		strcpy(spot-1, "ot");
		goto bottom;
	}
	if (len >= 5 && (!strcmp(spot-4, "matzo")
					|| !strcmp(spot-5, "matza"))) {
		strcpy(spot, "ot");
		goto bottom;
	}

	/* child/children (for wise guys who give their food funny names) */
	if (len >= 5 && !strcmp(spot-4, "child")) {
		strcpy(spot, "dren");
		goto bottom;
	}

	/* note: -eau/-eaux (gateau, bordeau...) */
	/* note: ox/oxen, VAX/VAXen, goose/geese */

	/* Ends in z, x, s, ch, sh; add an "es" */
	if (index("zxs", *spot)
			|| (len >= 2 && *spot=='h' && index("cs", *(spot-1)))
	/* Kludge to get "tomatoes" and "potatoes" right */
			|| (len >= 4 && !strcmp(spot-2, "ato"))) {
		strcpy(spot+1, "es");
		goto bottom;
	}

	/* Ends in y preceded by consonant (note: also "qu") change to "ies" */
	if (*spot == 'y' &&
	    (!index(vowels, *(spot-1)))) {
		strcpy(spot, "ies");
		goto bottom;
	}

	/* Default: append an 's' */
	strcpy(spot+1, "s");

bottom:	if (excess) strcpy(eos(str), excess);
	return str;
}

#endif /* OVL0 */

struct o_range {
	const char *name, oclass;
	int  f_o_range, l_o_range;
};

#ifndef OVLB

STATIC_DCL const struct o_range o_ranges[];

#else /* OVLB */

/* wishable subranges of objects */
/* KMH, balance patch -- fixed */
STATIC_OVL NEARDATA const struct o_range o_ranges[] = {
	{ "bag",	TOOL_CLASS,   SACK,	      BAG_OF_TRICKS },
	{ "lamp",	TOOL_CLASS,   OIL_LAMP,	      MAGIC_LAMP },
	{ "candle",     TOOL_CLASS,   TALLOW_CANDLE,  MAGIC_CANDLE },
	{ "horn",	TOOL_CLASS,   TOOLED_HORN,    HORN_OF_PLENTY },
	{ "shield",	ARMOR_CLASS,  SMALL_SHIELD,   SHIELD_OF_REFLECTION },
	{ "helm",	ARMOR_CLASS,  ELVEN_LEATHER_HELM, HELM_OF_TELEPATHY },
	{ "gloves",	ARMOR_CLASS,  LEATHER_GLOVES, GAUNTLETS_OF_DEXTERITY },
	{ "gauntlets",	ARMOR_CLASS,  LEATHER_GLOVES, GAUNTLETS_OF_DEXTERITY },
	{ "boots",	ARMOR_CLASS,  LOW_BOOTS,      LEVITATION_BOOTS },
	{ "shoes",	ARMOR_CLASS,  LOW_BOOTS,      IRON_SHOES },
	{ "cloak",	ARMOR_CLASS,  MUMMY_WRAPPING, CLOAK_OF_DISPLACEMENT },
	{ "shirt",	ARMOR_CLASS,  HAWAIIAN_SHIRT, T_SHIRT },
	{ "dragon scales",
			ARMOR_CLASS,  GRAY_DRAGON_SCALES, YELLOW_DRAGON_SCALES },
	{ "dragon scale",
			ARMOR_CLASS,  GRAY_DRAGON_SCALES, YELLOW_DRAGON_SCALES },
	{ "dragon scale mail",
			ARMOR_CLASS,  GRAY_DRAGON_SCALE_MAIL, YELLOW_DRAGON_SCALE_MAIL },
	{ "dragon scale shield",
			ARMOR_CLASS,  GRAY_DRAGON_SCALE_SHIELD, YELLOW_DRAGON_SCALE_SHIELD },
	{ "scales",
			ARMOR_CLASS,  GRAY_DRAGON_SCALES, YELLOW_DRAGON_SCALES },
	{ "DSM",
			ARMOR_CLASS,  GRAY_DRAGON_SCALE_MAIL, YELLOW_DRAGON_SCALE_MAIL },
	{ "sword",      WEAPON_CLASS, ORCISH_SHORT_SWORD,    TSURUGI },
	{ "polearm",    WEAPON_CLASS, PARTISAN, LANCE },
	{ "lightsaber", WEAPON_CLASS, GREEN_LIGHTSABER, WHITE_DOUBLE_LIGHTSABER },
	{ "firearm", 	WEAPON_CLASS, PISTOL, AUTO_SHOTGUN },
	{ "gun", 	WEAPON_CLASS, PISTOL, AUTO_SHOTGUN },
	{ "grenade", 	WEAPON_CLASS, FRAG_GRENADE, GAS_GRENADE },
#ifdef WIZARD
	{ "venom",	VENOM_CLASS,  BLINDING_VENOM, ACID_VENOM },
#endif
	{ "gray stone",	GEM_CLASS,    LUCKSTONE,      FLINT },
	{ "grey stone",	GEM_CLASS,    LUCKSTONE,      FLINT },
};

#define BSTRCMP(base,ptr,string) ((ptr) < base || strcmp((ptr),string))
#define BSTRCMPI(base,ptr,string) ((ptr) < base || strcmpi((ptr),string))
#define BSTRNCMP(base,ptr,string,num) ((ptr)<base || strncmp((ptr),string,num))
#define BSTRNCMPI(base,ptr,string,num) ((ptr)<base||strncmpi((ptr),string,num))

/*
 * Singularize a string the user typed in; this helps reduce the complexity
 * of readobjnam, and is also used in pager.c to singularize the string
 * for which help is sought.
 * WAC made most of the STRCMP ==> STRCMPI so that they are case insensitive
 * catching things like "bag of Tricks"
 */
char *
makesingular(oldstr)
const char *oldstr;
{
	register char *p, *bp;
	char *str = nextobuf();

	if (!oldstr || !*oldstr) {
		impossible("singular of null?");
		str[0] = 0;
		return str;
	}
	strcpy(str, oldstr);
	bp = str;

	while (*bp == ' ') bp++;
	/* find "cloves of garlic", "worthless pieces of blue glass" */
	if ((p = strstri(bp, "s of ")) != 0) {
	    /* but don't singularize "gauntlets", "boots", "Eyes of the.." */
	    if (BSTRNCMPI(bp, p-3, "Eye", 3) &&
		BSTRNCMP(bp, p-4, "boot", 4) &&
		BSTRNCMP(bp, p-8, "gauntlet", 8)
		&& BSTRNCMP(bp, p-7, "peanuts", 7)
		)
		while ((*p = *(p+1)) != 0) p++;
	    return bp;
	}

	/* remove -s or -es (boxes) or -ies (rubies) */
	p = eos(bp);
	if (p >= bp+1 && p[-1] == 's') {
		if (p >= bp+2 && p[-2] == 'e') {
			if (p >= bp+3 && p[-3] == 'i') {
				if(!BSTRCMPI(bp, p-7, "cookies") ||
				   !BSTRCMPI(bp, p-4, "pies"))
					goto mins;
				strcpy(p-3, "y");
				return bp;
			}

			/* note: cloves / knives from clove / knife */
			if(!BSTRCMPI(bp, p-6, "knives")) {
				strcpy(p-3, "fe");
				return bp;
			}
			if(!BSTRCMPI(bp, p-6, "staves")) {
				strcpy(p-3, "ff");
				return bp;
			}
			if (!BSTRCMPI(bp, p-6, "leaves")) {
				strcpy(p-3, "f");
				return bp;
			}
			if (!BSTRCMP(bp, p-8, "vortices")) {
				strcpy(p-4, "ex");
				return bp;
			}

			/* note: nurses, axes but boxes */
			if (!BSTRCMP(bp, p-5, "boxes") ||
			    !BSTRCMP(bp, p-4, "ches") ) {
				p[-2] = 0;
				return bp;
			}

			if (!BSTRCMPI(bp, p-6, "gloves") ||
                      !BSTRCMPI(bp, p-7, "clothes") ||
			    !BSTRCMP(bp, p-6, "lenses") ||
			    !BSTRCMP(bp, p-6, "shorts") ||
                            !BSTRCMPI(bp, p-5, "shoes") ||
                            !BSTRCMPI(bp, p-5, "trees") ||
                           !BSTRCMPI(bp, p-4, "toes") ||
                           !BSTRCMPI(bp, p-9, "overknees") ||
                           !BSTRCMPI(bp, p-13, "versus curses") ||
                           !BSTRCMPI(bp, p-8, "floppies") ||
                           !BSTRCMPI(bp, p-13, "glass devices") ||
                           !BSTRCMPI(bp, p-11, "tail spikes") ||
                           !BSTRCMPI(bp, p-10, "tailspikes") ||
                            !BSTRCMPI(bp, p-6, "scales"))
				return bp;

		} else if (!BSTRCMPI(bp, p-5, "boots") ||
			   !BSTRCMP(bp, p-9, "gauntlets") ||
                           !BSTRCMPI(bp, p-6, "tricks") ||
                           !BSTRCMPI(bp, p-9, "paralysis") ||
                           !BSTRCMPI(bp, p-9, "symbiosis") ||
                           !BSTRCMPI(bp, p-8, "geolysis") ||
                           !BSTRCMPI(bp, p-5, "glass") ||
                           !BSTRCMPI(bp, p-5, "dress") ||
                           !BSTRCMPI(bp, p-5, "girls") ||
                           !BSTRCMPI(bp, p-6, "storms") ||
                           !BSTRCMPI(bp, p-6, "thorns") ||
                           !BSTRCMPI(bp, p-6, "stasis") ||
                           !BSTRCMPI(bp, p-6, "status") ||
                           !BSTRCMPI(bp, p-7, "sandals") ||
                           !BSTRCMPI(bp, p-9, "mocassins") ||
                           !BSTRCMPI(bp, p-8, "sneakers") ||
                           !BSTRCMPI(bp, p-8, "vileness") ||
                           !BSTRCMPI(bp, p-8, "psionics") ||
                           !BSTRCMPI(bp, p-8, "elements") ||
                           !BSTRCMPI(bp, p-10, "cryogenics") ||
                           !BSTRCMPI(bp, p-5, "pumps") ||
                           !BSTRCMPI(bp, p-5, "heels") ||
                           !BSTRCMPI(bp, p-14, "shoulder rings") ||
                           !BSTRCMPI(bp, p-13, "strange rings") ||
                           !BSTRCMPI(bp, p-10, "aestivalis") ||
                           !BSTRCMPI(bp, p-16, "combat stilettos") ||
                           !BSTRCMPI(bp, p-14, "high stilettos") ||
                           !BSTRCMPI(bp, p-16, "winter stilettos") ||
                           !BSTRCMPI(bp, p-16, "unfair stilettos") ||
                           !BSTRCMPI(bp, p-4, "loss") ||
                           !BSTRCMPI(bp, p-5, "stats") ||
                           !BSTRCMPI(bp, p-5, "wings") ||
                           !BSTRCMPI(bp, p-5, "cards") ||
                           !BSTRCMPI(bp, p-6, "clouds") ||
                           !BSTRCMPI(bp, p-7, "effects") ||
                           !BSTRCMPI(bp, p-6, "sweets") ||
                           !BSTRCMPI(bp, p-4, "boss") ||
                           !BSTRCMPI(bp, p-5, "glass") ||
                           !BSTRCMPI(bp, p-6, "wounds") ||
                           !BSTRCMPI(bp, p-13, "max hitpoints") ||
                           !BSTRCMPI(bp, p-5, "items") ||
                           !BSTRCMPI(bp, p-4, "ness") ||
                           !BSTRCMPI(bp, p-14, "shape changers") ||
                           !BSTRCMPI(bp, p-15, "detect monsters") ||
                           !BSTRCMPI(bp, p-21, "Medallion of Shifters") ||
                                /* WAC added */
                           !BSTRCMPI(bp, p-12, "Key of Chaos") ||
                           !BSTRCMPI(bp, p-7, "Perseus") || /* WAC added */
			   !BSTRCMPI(bp, p-11, "Aesculapius") || /* staff */
			   !BSTRCMP(bp, p-10, "eucalyptus") ||
#ifdef WIZARD
			   !BSTRCMP(bp, p-9, "iron bars") ||
#endif
			   !BSTRCMP(bp, p-5, "aklys") ||
			   !BSTRCMP(bp, p-6, "fungus"))
				return bp;
	mins:
		p[-1] = 0;

	} else {

                if(!BSTRCMPI(bp, p-5, "teeth")) {
			strcpy(p-5, "tooth");
			return bp;
		}

		if (!BSTRCMP(bp, p-5, "fungi")) {
			strcpy(p-5, "fungus");
			return bp;
		}

		/* here we cannot find the plural suffix */
	}
	return bp;
}

/* compare user string against object name string using fuzzy matching */
static boolean
wishymatch(u_str, o_str, retry_inverted)
const char *u_str;	/* from user, so might be variant spelling */
const char *o_str;	/* from objects[], so is in canonical form */
boolean retry_inverted;	/* optional extra "of" handling */
{
	/* special case: wizards can wish for traps.  The object is "beartrap"
	 * and the trap is "bear trap", so to let wizards wish for both we
	 * must not fuzzymatch.
	 */
#ifdef WIZARD
	if (wizard && !strcmp(o_str, "beartrap"))
	    return !strncmpi(o_str, u_str, 8);
#endif

	/* ignore spaces & hyphens and upper/lower case when comparing */
	if (fuzzymatch(u_str, o_str, " -", TRUE)) return TRUE;

	if (retry_inverted) {
	    const char *u_of, *o_of;
	    char *p, buf[BUFSZ];

	    /* when just one of the strings is in the form "foo of bar",
	       convert it into "bar foo" and perform another comparison */
	    u_of = strstri(u_str, " of ");
	    o_of = strstri(o_str, " of ");
	    if (u_of && !o_of) {
		strcpy(buf, u_of + 4);
		p = eos(strcat(buf, " "));
		while (u_str < u_of) *p++ = *u_str++;
		*p = '\0';
		return fuzzymatch(buf, o_str, " -", TRUE);
	    } else if (o_of && !u_of) {
		strcpy(buf, o_of + 4);
		p = eos(strcat(buf, " "));
		while (o_str < o_of) *p++ = *o_str++;
		*p = '\0';
		return fuzzymatch(u_str, buf, " -", TRUE);
	    }
	}

	/* [note: if something like "elven speed boots" ever gets added, these
	   special cases should be changed to call wishymatch() recursively in
	   order to get the "of" inversion handling] */
	if (!strncmp(o_str, "dwarvish ", 9)) {
	    if (!strncmpi(u_str, "dwarven ", 8))
		return fuzzymatch(u_str + 8, o_str + 9, " -", TRUE);
	} else if (!strncmp(o_str, "elven ", 6)) {
	    if (!strncmpi(u_str, "elvish ", 7))
		return fuzzymatch(u_str + 7, o_str + 6, " -", TRUE);
	    else if (!strncmpi(u_str, "elfin ", 6))
		return fuzzymatch(u_str + 6, o_str + 6, " -", TRUE);
	} else if (!strcmp(o_str, "aluminum")) {
		/* this special case doesn't really fit anywhere else... */
		/* (note that " wand" will have been stripped off by now) */
	    if (!strcmpi(u_str, "aluminium"))
		return fuzzymatch(u_str + 9, o_str + 8, " -", TRUE);
	}

	return FALSE;
}

/* alternate spellings; if the difference is only the presence or
   absence of spaces and/or hyphens (such as "pickaxe" vs "pick axe"
   vs "pick-axe") then there is no need for inclusion in this list;
   likewise for ``"of" inversions'' ("boots of speed" vs "speed boots") */
struct alt_spellings {
	const char *sp;
	int ob;
} spellings[] = {
	{ "pickax", PICK_AXE },
	{ "whip", BULLWHIP },
	{ "saber", SILVER_SABER },
	{ "silver sabre", SILVER_SABER },
	{ "smooth shield", SHIELD_OF_REFLECTION },
	{ "grey dragon scale mail", GRAY_DRAGON_SCALE_MAIL },
	{ "tailspikes", TAIL_SPIKES },
	{ "grey dragon scales", GRAY_DRAGON_SCALES },
	{ "enchant armour", SCR_ENCHANT_ARMOR },
	{ "destroy armour", SCR_DESTROY_ARMOR },
	{ "scroll of enchant armour", SCR_ENCHANT_ARMOR },
	{ "scroll of destroy armour", SCR_DESTROY_ARMOR },
	{ "leather armour", LEATHER_ARMOR },
	{ "studded leather armour", STUDDED_LEATHER_ARMOR },
	{ "iron ball", HEAVY_IRON_BALL },
	{ "lantern", BRASS_LANTERN },
	{ "mattock", DWARVISH_MATTOCK },
	{ "amulet of poison resistance", AMULET_VERSUS_POISON },
	/*{ "stone", ROCK },*/
	{ "can", TIN },
	{ "can opener", TIN_OPENER },
	{ "kelp", KELP_FROND },
	{ "eucalyptus", EUCALYPTUS_LEAF },
	{ "grapple", GRAPPLING_HOOK },
	/* KMH, balance patch -- new items */
	{ "amulet versus stoning", AMULET_VERSUS_STONE },
	{ "amulet of stone resistance", AMULET_VERSUS_STONE },
	{ "health stone", HEALTHSTONE },
	{ "handgun", PISTOL },
	{ "hand gun", PISTOL },
	{ "revolver", PISTOL },
	{ "bazooka", ROCKET_LAUNCHER },
	{ "hand grenade", FRAG_GRENADE },
	{ "dynamite", STICK_OF_DYNAMITE },
#ifdef ZOUTHERN
	{ "kiwifruit", APPLE },
	{ "kiwi fruit", APPLE },
	{ "kiwi", APPLE }, /* Actually refers to the bird */
#endif
#ifdef WIZARD
/* KMH, balance patch -- How lazy are we going to let the players get? */
/* WAC Added Abbreviations */
/* Tools */
    { "BoH", BAG_OF_HOLDING },
    { "BoO", BELL_OF_OPENING },
    { "ML", MAGIC_LAMP },
    { "MM", MAGIC_MARKER },
    { "UH", UNICORN_HORN },
/* Rings */
    { "RoC", RIN_CONFLICT },
    { "RoPC", RIN_POLYMORPH_CONTROL },
    { "RoTC", RIN_TELEPORT_CONTROL },
/* Scrolls */
    { "SoC", SCR_CHARGING },
    { "SoEA", SCR_ENCHANT_ARMOR },
    { "SoEW", SCR_ENCHANT_WEAPON },
    { "SoG", SCR_GENOCIDE },
    { "SoI", SCR_IDENTIFY },
    { "SoRC", SCR_REMOVE_CURSE },
/* Potions */
    { "PoEH",  POT_EXTRA_HEALING},
    { "PoGL",  POT_GAIN_LEVEL},
    { "PoW",  POT_WATER},
/* Amulet */
    { "AoESP",  AMULET_OF_ESP},
    { "AoLS",  AMULET_OF_LIFE_SAVING},
    { "AoY",  AMULET_OF_YENDOR},
/* Wands */
    { "WoW",  WAN_WISHING},
    { "WoCM",  WAN_CREATE_MONSTER},
    { "WoT",  WAN_TELEPORTATION},
    { "WoUT",  WAN_UNDEAD_TURNING},
/* Armour */
    { "BoL",  LEVITATION_BOOTS},
    { "BoS",  SPEED_BOOTS},
    { "SB",  SPEED_BOOTS},
    { "BoWW",  WATER_WALKING_BOOTS},
    { "WWB",  WATER_WALKING_BOOTS},
    { "CoD",  CLOAK_OF_DISPLACEMENT},
    { "CoI",  CLOAK_OF_INVISIBILITY},
    { "CoMR",  CLOAK_OF_MAGIC_RESISTANCE},
    { "GoD",  GAUNTLETS_OF_DEXTERITY},
    { "GoP",  GAUNTLETS_OF_POWER},
    { "HoB",  HELM_OF_BRILLIANCE},
    { "HoOA",  HELM_OF_OPPOSITE_ALIGNMENT},
    { "HoT",  HELM_OF_TELEPATHY},
    { "SoR",  SHIELD_OF_REFLECTION},
#endif
	{ "camera", EXPENSIVE_CAMERA },
	{ "T shirt", T_SHIRT },
	{ "tee shirt", T_SHIRT },
	{ (const char *)0, 0 },
};

/*
 * Return something wished for.  Specifying a null pointer for
 * the user request string results in a random object.  Otherwise,
 * if asking explicitly for "nothing" (or "nil") return no_wish;
 * if not an object return &zeroobj; if an error (no matching object),
 * return null.
 * If from_user is false, we're reading from the wizkit, nothing was typed in.
 */
struct obj *
readobjnam(bp, no_wish, from_user, actualwish)
register char *bp;
struct obj *no_wish;
boolean from_user;
boolean actualwish;
{
	register char *p;
	register int i;
	register struct obj *otmp;
	int cnt, spe, spesgn, typ, very, rechrg;
	int blessed, uncursed, iscursed, ispoisoned, isgreased, isdrained;
	int eroded, eroded2, erodeproof;
	int isinvisible;
	int halfeaten, halfdrained, mntmp, contents;
	int islit, unlabeled, ishistoric, isdiluted;
	struct fruit *f;
	int ftype = current_fruit;
	char fruitbuf[BUFSZ];
	/* Fruits may not mess up the ability to wish for real objects (since
	 * you can leave a fruit in a bones file and it will be added to
	 * another person's game), so they must be checked for last, after
	 * stripping all the possible prefixes and seeing if there's a real
	 * name in there.  So we have to save the full original name.  However,
	 * it's still possible to do things like "uncursed burnt Alaska",
	 * or worse yet, "2 burned 5 course meals", so we need to loop to
	 * strip off the prefixes again, this time stripping only the ones
	 * possible on food.
	 * We could get even more detailed so as to allow food names with
	 * prefixes that _are_ possible on food, so you could wish for
	 * "2 3 alarm chilis".  Currently this isn't allowed; options.c
	 * automatically sticks 'candied' in front of such names.
	 */

	char oclass;
	char *un, *dn, *actualn;
	const char *name=0;


	cnt = spe = spesgn = typ = very = rechrg =
		blessed = uncursed = iscursed = isdrained = halfdrained =
		isinvisible =
		ispoisoned = isgreased = eroded = eroded2 = erodeproof =
		halfeaten = islit = unlabeled = ishistoric = isdiluted = 0;
	mntmp = NON_PM;
#define UNDEFINED 0
#define EMPTY 1
#define SPINACH 2
#define BEANS 3
#define PEACHES 4
#define FISH 5
	contents = UNDEFINED;
	oclass = 0;
	actualn = dn = un = 0;

	if (!bp) goto any;
	/* first, remove extra whitespace they may have typed */
	(void)mungspaces(bp);
	/* allow wishing for "nothing" to preserve wishless conduct...
	   [now requires "wand of nothing" if that's what was really wanted] */

	if (!strncmpi(bp, "%s", 2)) return (struct obj *)0;

	if (!strcmpi(bp, "nothing") || !strcmpi(bp, "nil") ||
	    !strcmpi(bp, "none")) return no_wish;
	/* save the [nearly] unmodified choice string */
	strcpy(fruitbuf, bp);

	for(;;) {
		register int l;

		if (!bp || !*bp) goto any;
		if (!strncmpi(bp, "an ", l=3) ||
		    !strncmpi(bp, "a ", l=2)) {
			cnt = 1;
		} else if (!strncmpi(bp, "the ", l=4)) {
			;	/* just increment `bp' by `l' below */
		} else if (!cnt && digit(*bp) && strcmp(bp, "0")) {
			cnt = atoi(bp);
			while(digit(*bp)) bp++;
			while(*bp == ' ') bp++;
			l = 0;
		} else if (*bp == '+' || *bp == '-') {
			spesgn = (*bp++ == '+') ? 1 : -1;
			spe = atoi(bp);
			while(digit(*bp)) bp++;
			while(*bp == ' ') bp++;
			l = 0;
		} else if (!strncmpi(bp, "blessed ", l=8)
/*WAC removed this.  Holy is in some artifact weapon names
                || !strncmpi(bp, "holy ", l=5)
*/
                           ) {
			blessed = 1;
		} else if (!strncmpi(bp, "cursed ", l=7) ||
			   !strncmpi(bp, "unholy ", l=7)) {
			iscursed = 1;
		} else if (!strncmpi(bp, "uncursed ", l=9)) {
			uncursed = 1;
		} else if (!strncmpi(bp, "visible ", l=8)) {
			isinvisible = -1;
		} else if (!strncmpi(bp, "invisible ", l=10)) {
			isinvisible = 1;
		} else if (!strncmpi(bp, "rustproof ", l=10) ||
			   !strncmpi(bp, "erodeproof ", l=11) ||
			   !strncmpi(bp, "corrodeproof ", l=13) ||
			   !strncmpi(bp, "fixed ", l=6) ||
			   !strncmpi(bp, "fireproof ", l=10) ||
			   !strncmpi(bp, "stainless ", l=10) ||
			   !strncmpi(bp, "rotproof ", l=9)) {
			erodeproof = 1;
		} else if (!strncmpi(bp,"lit ", l=4) ||
			   !strncmpi(bp,"burning ", l=8)) {
			islit = 1;
		} else if (!strncmpi(bp,"unlit ", l=6) ||
			   !strncmpi(bp,"extinguished ", l=13)) {
			islit = 0;
		/* "unlabeled" and "blank" are synonymous */
		} else if (!strncmpi(bp,"unlabeled ", l=10) ||
			   !strncmpi(bp,"unlabelled ", l=11) ||
			   !strncmpi(bp,"blank ", l=6)) {
			unlabeled = 1;
		} else if(!strncmpi(bp, "poisoned ",l=9)
#ifdef WIZARD
			  || (wizard && !strncmpi(bp, "trapped ",l=8))
#endif
			  ) {
			ispoisoned=1;
		} else if(!strncmpi(bp, "greased ",l=8)) {
			isgreased=1;
		} else if(!strncmpi(bp, "bgf ",l=4)) {
			isgreased = 1;
			blessed = 1;
			erodeproof = 1;
		} else if(!strncmpi(bp, "bgfi ",l=4)) {
			isgreased = 1;
			blessed = 1;
			erodeproof = 1;
			isinvisible = 1;
		} else if (!strncmpi(bp, "very ", l=5)) {
			/* very rusted very heavy iron ball */
			very = 1;
		} else if (!strncmpi(bp, "thoroughly ", l=11)) {
			very = 2;
		} else if (!strncmpi(bp, "rusty ", l=6) ||
			   !strncmpi(bp, "rusted ", l=7) ||
			   !strncmpi(bp, "burnt ", l=6) ||
			   !strncmpi(bp, "burned ", l=7)) {
			eroded = 1 + very;
			very = 0;
		} else if (!strncmpi(bp, "corroded ", l=9) ||
			   !strncmpi(bp, "rotted ", l=7)) {
			eroded2 = 1 + very;
			very = 0;
		} else if (!strncmpi(bp, "partly drained ", l=15)) {
			isdrained = 1;
			halfdrained = 1;
		} else if (!strncmpi(bp, "drained ", l=8)) {
			isdrained = 1;
			halfdrained = 0;
		} else if (!strncmpi(bp, "partly eaten ", l=13)) {
			halfeaten = 1;
		} else if (!strncmpi(bp, "historic ", l=9)) {
			ishistoric = 1;
		} else if (!strncmpi(bp, "diluted ", l=8)) {
			isdiluted = 1;
		} else if(!strncmpi(bp, "empty ", l=6)) {
			contents = EMPTY;
		} else break;
		bp += l;
	}
	if(!cnt) cnt = 1;		/* %% what with "gems" etc. ? */
	if (strlen(bp) > 1) {
	    if ((p = rindex(bp, '(')) != 0) {
		if (p > bp && p[-1] == ' ') p[-1] = 0;
		else *p = 0;
		p++;
		if (!strcmpi(p, "lit)")) {
		    islit = 1;
		} else {
		    spe = atoi(p);
		    while(digit(*p)) p++;
		    if (*p == ':') {
			p++;
			rechrg = spe;
			spe = atoi(p);
			while (digit(*p)) p++;
		    }
		    if (*p != ')') {
			spe = rechrg = 0;
		    } else {
			spesgn = 1;
			p++;
			if (*p) strcat(bp, p);
		    }
		}
	    }
	}
/*
   otmp->spe is type schar; so we don't want spe to be any bigger or smaller.
   also, spe should always be positive  -- some cheaters may try to confuse
   atoi()
*/
	if (spe < 0) {
		spesgn = -1;	/* cheaters get what they deserve */
		spe = abs(spe);
	}
	if (spe > SCHAR_LIM)
		spe = SCHAR_LIM;
	if (rechrg < 0 || rechrg > 7) rechrg = 7;	/* recharge_limit */

	/* now we have the actual name, as delivered by xname, say
		green potions called whisky
		scrolls labeled "QWERTY"
		egg
		fortune cookies
		very heavy iron ball named hoei
		wand of wishing
		elven cloak
	*/
	if ((p = strstri(bp, " named ")) != 0 && actualwish) {
		*p = 0;
		name = p+7;
	}
	if ((p = strstri(bp, " called ")) != 0) {
		*p = 0;
		un = p+8;
		/* "helmet called telepathy" is not "helmet" (a specific type)
		 * "shield called reflection" is not "shield" (a general type)
		 */
		for(i = 0; i < SIZE(o_ranges); i++)
		    if(!strcmpi(bp, o_ranges[i].name)) {
			oclass = o_ranges[i].oclass;
			goto srch;
		    }
	}
	if ((p = strstri(bp, " labeled ")) != 0) {
		*p = 0;
		dn = p+9;
	} else if ((p = strstri(bp, " labelled ")) != 0) {
		*p = 0;
		dn = p+10;
	}
	if ((p = strstri(bp, " of spinach")) != 0) {
		*p = 0;
		contents = SPINACH;
	}
	if ((p = strstri(bp, " of beans")) != 0) {
		*p = 0;
		contents = BEANS;
	}
	if ((p = strstri(bp, " of peaches")) != 0) {
		*p = 0;
		contents = PEACHES;
	}
	if ((p = strstri(bp, " of fish")) != 0) {
		*p = 0;
		contents = FISH;
	}

	/*
	Skip over "pair of ", "pairs of", "set of" and "sets of".

	Accept "3 pair of boots" as well as "3 pairs of boots". It is valid
	English either way.  See makeplural() for more on pair/pairs.

	We should only double count if the object in question is not
	refered to as a "pair of".  E.g. We should double if the player
	types "pair of spears", but not if the player types "pair of
	lenses".  Luckily (?) all objects that are refered to as pairs
	-- boots, gloves, and lenses -- are also not mergable, so cnt is
	ignored anyway.
	*/
	if(!strncmpi(bp, "pair of ",8)) {
		bp += 8;
		cnt *= 2;
	} else if(cnt > 1 && !strncmpi(bp, "pairs of ",9)) {
		bp += 9;
		cnt *= 2;
	} else if (!strncmpi(bp, "set of ",7)) {
		bp += 7;
	} else if (!strncmpi(bp, "sets of ",8)) {
		bp += 8;
	}

	/*
	 * Find corpse type using "of" (figurine of an orc, tin of orc meat)
	 * Don't check if it's a wand or spellbook.
	 * (avoid "wand/finger of death" confusion).
	 * (WAC avoid "hand/eye of vecna", "wallet of perseus" 
	 *  "medallion of shifters", "stake of van helsing" similarly
	 *  ALI "potion of vampire blood" also).
	 */
	if (wizard)
	if (!strstri(bp, "wand ")
	 && !strstri(bp, "spellbook ")
         && !strstri(bp, "hand ")
         && !strstri(bp, "eye ")
         && !strstri(bp, "medallion ")
         && !strstri(bp, "stake ")
         && !strstri(bp, "potion ")
         && !strstri(bp, "potions ")
	 && !strstri(bp, "finger ")) {
	    if ((p = strstri(bp, " of ")) != 0
		&& (mntmp = name_to_mon(p+4)) >= LOW_PM)
		*p = 0;
	}

	/* Find corpse type w/o "of" (red dragon scale mail, yeti corpse) */
	if (strncmpi(bp, "samurai sword", 13)) /* not the "samurai" monster! */
	if (strncmpi(bp, "wizard lock", 11)) /* not the "wizard" monster! */
	if (strncmpi(bp, "ninja-to", 8)) /* not the "ninja" rank */
	if (strncmpi(bp, "master key", 10)) /* not the "master" rank */
	if (strncmpi(bp, "magenta", 7)) /* not the "mage" rank */
        if (strncmpi(bp, "Thiefbane", 9)) /* not the "thief" rank */
        if (strncmpi(bp, "Ogresmasher", 11)) /* not the "ogre" monster */
        if (strncmpi(bp, "Bat from Hell", 13)) /* not the "bat" monster */
        if (strncmpi(bp, "vampire blood", 13)) /* not the "vampire" monster */
	if (wizard)
	if (mntmp < LOW_PM && strlen(bp) > 2 &&
	    (mntmp = name_to_mon(bp)) >= LOW_PM ) {
		int mntmptoo, mntmplen;	/* double check for rank title */
		char *obp = bp;
		mntmptoo = title_to_mon(bp, (int *)0, &mntmplen);
		bp += mntmp != mntmptoo ? (int)strlen(mons[mntmp].mname) : mntmplen;
		if (*bp == ' ') bp++;
		else if (!strncmpi(bp, "s ", 2)) bp += 2;
		else if (!strncmpi(bp, "es ", 3)) bp += 3;
		else if (!*bp && !actualn && !dn && !un && !oclass) {
		    /* no referent; they don't really mean a monster type */
		    bp = obp;
		    mntmp = NON_PM;
		}
	}

	/* first change to singular if necessary */
	if (*bp) {
		char *sng = makesingular(bp);
		if (strcmp(bp, sng)) {
			if (cnt == 1) cnt = 2;
			strcpy(bp, sng);
		}
	}

	/* Alternate spellings (pick-ax, silver sabre, &c) */
    {
	struct alt_spellings *as = spellings;

		while(as->sp) {
		if (fuzzymatch(bp, as->sp, " -", TRUE)) {
			typ = as->ob;
			goto typfnd;
		}
		as++;
	}
	/* can't use spellings list for this one due to shuffling */
	if (!strncmpi(bp, "grey spell", 10))
		*(bp + 2) = 'a';
    }

	/* dragon scales - assumes order of dragons */
	if(!strcmpi(bp, "scales") &&
			mntmp >= PM_GRAY_DRAGON && mntmp <= PM_YELLOW_DRAGON) {
		typ = GRAY_DRAGON_SCALES + mntmp - PM_GRAY_DRAGON;
		mntmp = NON_PM;	/* no monster */
		goto typfnd;
	}

	p = eos(bp);
	if(!BSTRCMPI(bp, p-10, "holy water")) {
		typ = POT_WATER;
		if ((p-bp) >= 12 && *(p-12) == 'u')
			iscursed = 1; /* unholy water */
		else blessed = 1;
		goto typfnd;
	}
	if(unlabeled && !BSTRCMPI(bp, p-6, "scroll")) {
		typ = SCR_BLANK_PAPER;
		goto typfnd;
	}
	if(unlabeled && !BSTRCMPI(bp, p-9, "spellbook")) {
		typ = SPE_BLANK_PAPER;
		goto typfnd;
	}
	/*
	 * NOTE: Gold pieces are handled as objects nowadays, and therefore
	 * this section should probably be reconsidered as well as the entire
	 * gold/money concept.  Maybe we want to add other monetary units as
	 * well in the future. (TH)
	 */
	if(!BSTRCMPI(bp, p-10, "gold piece") || !BSTRCMPI(bp, p-7, "zorkmid") ||
	   !strcmpi(bp, "gold") || !strcmpi(bp, "money") ||
	   !strcmpi(bp, "coin") || *bp == GOLD_SYM) {
			if (cnt > 5000
#ifdef WIZARD
					&& !wizard
#endif
						) cnt=5000;
		if (cnt < 1) cnt=1;
#ifndef GOLDOBJ
		if (from_user && actualwish)
		    pline("%d gold piece%s.", cnt, plur(cnt));
		if (actualwish) u.ugold += cnt;
		flags.botl=1;
		return (&zeroobj);
#else
		if (!actualwish) return (&zeroobj);

                otmp = mksobj(GOLD_PIECE, FALSE, FALSE, FALSE);
		otmp->quan = cnt;
                otmp->owt = weight(otmp);
		flags.botl=1;
		return (otmp);
#endif
	}
	if (strlen(bp) == 1 &&
	   (i = def_char_to_objclass(*bp)) < MAXOCLASSES && i > ILLOBJ_CLASS
#ifdef WIZARD
	    && (wizard || i != VENOM_CLASS)
#else
	    && i != VENOM_CLASS
#endif
	    ) {
		oclass = i;
		goto any;
	}

	/* Search for class names: XXXXX potion, scroll of XXXXX.  Avoid */
	/* false hits on, e.g., rings for "ring mail". */
    /* false hits on "GrayWAND", "Staff of WitheRING"  -- WAC */
	if(strncmpi(bp, "enchant ", 8) &&
	   strncmpi(bp, "destroy ", 8) &&
	   strncmpi(bp, "proof ", 6) &&
	   strncmpi(bp, "food detection", 14) &&
	   strncmpi(bp, "ring mail", 9) &&
	   strncmpi(bp, "studded leather arm", 19) &&
	   strncmpi(bp, "leather arm", 11) &&
	   strncmpi(bp, "tooled horn", 11) &&
       strncmpi(bp, "graywand", 8) &&
       strncmpi(bp, "staff of withering", 18) &&
       strncmpi(bp, "one ring", 8) &&
	   strncmpi(bp, "food ration", 11) &&
	   strncmpi(bp, "meat ring", 9)
	   && strncmpi(bp, "plasteel armor", 14)
	)
	for (i = 0; i < (int)(sizeof wrpsym); i++) {
		register int j = strlen(wrp[i]);
		if(!strncmpi(bp, wrp[i], j)){
			oclass = wrpsym[i];
			if(oclass != AMULET_CLASS) {
			    bp += j;
			    if(!strncmpi(bp, " of ", 4)) actualn = bp+4;
			    /* else if(*bp) ?? */
			} else
			    actualn = bp;
			goto srch;
		}
		if(!BSTRCMPI(bp, p-j, wrp[i])){
			oclass = wrpsym[i];
			p -= j;
			*p = 0;
			if(p > bp && p[-1] == ' ') p[-1] = 0;
			actualn = dn = bp;
			goto srch;
		}
	}

	/* "grey stone" check must be before general "stone" */
	for (i = 0; i < SIZE(o_ranges); i++)
	    if(!strcmpi(bp, o_ranges[i].name)) {
		typ = rnd_class(o_ranges[i].f_o_range, o_ranges[i].l_o_range);
		goto typfnd;
	    }

	/*if (!BSTRCMPI(bp, p-6, " stone")) {
		p[-6] = 0;
		oclass = GEM_CLASS;
		dn = actualn = bp;
		goto srch;
	} else */ if (!strcmpi(bp, "looking glass")) {
		;	/* avoid false hit on "* glass" */
	} else if (!BSTRCMPI(bp, p-6, " glass") || !strcmpi(bp, "glass")) {
		register char *g = bp;
		if (strstri(g, "broken")) return (struct obj *)0;
		if (!strncmpi(g, "worthless ", 10)) g += 10;
		if (!strncmpi(g, "piece of ", 9)) g += 9;
		if (!strncmpi(g, "colored ", 8)) g += 8;
		else if (!strncmpi(g, "coloured ", 9)) g += 9;
		if (!strcmpi(g, "glass")) {	/* choose random color */
			/* 9 different kinds */
			typ = LAST_GEM + rnd(9);
			if (objects[typ].oc_class == GEM_CLASS) goto typfnd;
			else typ = 0;	/* somebody changed objects[]? punt */
		} else {		/* try to construct canonical form */
			char tbuf[BUFSZ];
			strcpy(tbuf, "worthless piece of ");
			strcat(tbuf, g);  /* assume it starts with the color */
			strcpy(bp, tbuf);
		}
	}

	actualn = bp;
	if (!dn) dn = actualn; /* ex. "skull cap" */
srch:
	/* check real names of gems first */
	if(!oclass && actualn) {
	    for(i = bases[GEM_CLASS]; i <= LAST_GEM; i++) {
		register const char *zn;

		if((zn = OBJ_NAME(objects[i])) && !strcmpi(actualn, zn)) {
		    typ = i;
		    goto typfnd;
		}
	    }
	}
	i = oclass ? bases[(int)oclass] : 1;
	while(i < NUM_OBJECTS && (!oclass || objects[i].oc_class == oclass)){
		register const char *zn;

		if (actualn && (zn = OBJ_NAME(objects[i])) != 0 &&
			    wishymatch(actualn, zn, TRUE)) {
			typ = i;
			goto typfnd;
		}
		if (dn && (zn = OBJ_DESCR(objects[i])) != 0 &&
			    wishymatch(dn, zn, FALSE)) {
			/* don't match extra descriptions (w/o real name) */
			if (!OBJ_NAME(objects[i])) return (struct obj *)0;
			typ = i;
			goto typfnd;
		}
		if (un && (zn = objects[i].oc_uname) != 0 &&
			    wishymatch(un, zn, FALSE)) {
			typ = i;
			goto typfnd;
		}
		i++;
	}
	if (actualn) {
		struct Jitem *j[] = {ZAPM_items,Japanese_items,Pirate_items,Soviet_items,Ancient_items};
		for(i=0;i<sizeof(j)/sizeof(j[0]);i++)
		{
		while(j[i]->item) {
			if (actualn && !strcmpi(actualn, j[i]->name)) {
				typ = j[i]->item;
				goto typfnd;
			}
			j[i]++;
		}
		}
	}
	if (!strcmpi(bp, "spinach")) {
		contents = SPINACH;
		typ = TIN;
		goto typfnd;
	}
	if (!strcmpi(bp, "beans")) {
		contents = BEANS;
		typ = TIN;
		goto typfnd;
	}
	if (!strcmpi(bp, "peaches")) {
		contents = PEACHES;
		typ = TIN;
		goto typfnd;
	}
	if (!strcmpi(bp, "fish")) {
		contents = FISH;
		typ = TIN;
		goto typfnd;
	}
	/* Note: not strncmpi.  2 fruits, one capital, one not, are possible. */
	{
	    char *fp;
	    int l, cntf;
	    int blessedf, iscursedf, uncursedf, halfeatenf;

	    blessedf = iscursedf = uncursedf = halfeatenf = 0;
	    cntf = 0;

	    fp = fruitbuf;
	    for(;;) {
		if (!fp || !*fp) break;
		if (!strncmpi(fp, "an ", l=3) ||
		    !strncmpi(fp, "a ", l=2)) {
			cntf = 1;
		} else if (!cntf && digit(*fp)) {
			cntf = atoi(fp);
			while(digit(*fp)) fp++;
			while(*fp == ' ') fp++;
			l = 0;
		} else if (!strncmpi(fp, "blessed ", l=8)) {
			blessedf = 1;
		} else if (!strncmpi(fp, "cursed ", l=7)) {
			iscursedf = 1;
		} else if (!strncmpi(fp, "uncursed ", l=9)) {
			uncursedf = 1;
		} else if (!strncmpi(fp, "partly eaten ", l=13)) {
			halfeatenf = 1;
		} else break;
		fp += l;
	    }

	    for(f=ffruit; f; f = f->nextf) {
		char *f1 = f->fname, *f2 = makeplural(f->fname);

		if(!strncmp(fp, f1, strlen(f1)) ||
					!strncmp(fp, f2, strlen(f2))) {
			typ = SLIME_MOLD;
			blessed = blessedf;
			iscursed = iscursedf;
			uncursed = uncursedf;
			halfeaten = halfeatenf;
			cnt = cntf;
			ftype = f->fid;
			goto typfnd;
		}
	    }
	}

	if(!oclass && actualn) {
	    /*short*/int objtyp;

	    /* Perhaps it's an artifact specified by name, not type */
	    name = artifact_name(actualn, &objtyp);
	    if(name) {
		typ = objtyp;
		goto typfnd;
	    }
	}
#ifdef WIZARD
	/* Let wizards wish for traps --KAA */
	/* must come after objects check so wizards can still wish for
	 * trap objects like beartraps
	 */
	if (wizard && from_user) {
		int trap;

		for (trap = NO_TRAP+1; trap < TRAPNUM; trap++) {
			const char *tname;

			tname = defsyms[trap_to_defsym(trap)].explanation;
			if (!strncmpi(tname, bp, strlen(tname))) {
				/* avoid stupid mistakes */
				if((trap == TRAPDOOR || trap == HOLE || trap == SHAFT_TRAP || trap == CURRENT_SHAFT)
				      && !Can_fall_thru(&u.uz) && !Is_stronghold(&u.uz) ) trap = ROCKTRAP;
				(void) maketrap(u.ux, u.uy, trap, 0, TRUE);
				pline("%s.", An(tname));
				return(&zeroobj);
			}
		}
		/* or some other dungeon features -dlc */
		p = eos(bp);
		if(!BSTRCMP(bp, p-8, "fountain")) {
			levl[u.ux][u.uy].typ = FOUNTAIN;
			level.flags.nfountains++;
			if(!strncmpi(bp, "magic ", 6))
				levl[u.ux][u.uy].blessedftn = 1;
			pline("A %sfountain.",
			      levl[u.ux][u.uy].blessedftn ? "magic " : "");
			newsym(u.ux, u.uy);
			return(&zeroobj);
		}
		if(!BSTRCMP(bp, p-6, "throne")) {
			levl[u.ux][u.uy].typ = THRONE;
			pline("A throne.");
			newsym(u.ux, u.uy);
			return(&zeroobj);
		}
		if(!BSTRCMP(bp, p-9, "headstone") || !BSTRCMP(bp, p-5, "grave")) {
			levl[u.ux][u.uy].typ = GRAVE;
			make_grave(u.ux, u.uy, (char *) 0);
			pline("A grave.");
			newsym(u.ux, u.uy);
			return(&zeroobj);
		}
		if(!BSTRCMP(bp, p-4, "tree")) {
			levl[u.ux][u.uy].typ = TREE;
			pline("A tree.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-10, "grave wall")) {
			levl[u.ux][u.uy].typ = GRAVEWALL;
			pline("A grave wall.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-11, "watertunnel")) {
			levl[u.ux][u.uy].typ = WATERTUNNEL;
			pline("A water tunnel.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-6, "tunnel")) {
			levl[u.ux][u.uy].typ = TUNNELWALL;
			pline("A tunnel.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-8, "farmland")) {
			levl[u.ux][u.uy].typ = FARMLAND;
			pline("Farmland.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-8, "mountain")) {
			levl[u.ux][u.uy].typ = MOUNTAIN;
			pline("A mountain.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-13, "crystal water")) {
			levl[u.ux][u.uy].typ = CRYSTALWATER;
			pline("Crystal water.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-8, "moorland")) {
			levl[u.ux][u.uy].typ = MOORLAND;
			pline("Moorland.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-10, "urine lake")) {
			levl[u.ux][u.uy].typ = URINELAKE;
			pline("A urine lake.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-13, "shifting sand")) {
			levl[u.ux][u.uy].typ = SHIFTINGSAND;
			pline("Shifting sand.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-10, "styx river")) {
			levl[u.ux][u.uy].typ = STYXRIVER;
			pline("A styx river.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-9, "pentagram")) {
			levl[u.ux][u.uy].typ = PENTAGRAM;
			pline("A pentagram.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-12, "poisonedwell")) {
			levl[u.ux][u.uy].typ = POISONEDWELL;
			pline("A poisoned well.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-4, "well")) {
			levl[u.ux][u.uy].typ = WELL;
			pline("A well.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-12, "burningwagon")) {
			levl[u.ux][u.uy].typ = BURNINGWAGON;
			pline("A burning wagon.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-5, "wagon")) {
			levl[u.ux][u.uy].typ = WAGON;
			pline("A wagon.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-12, "wooden table")) {
			levl[u.ux][u.uy].typ = WOODENTABLE;
			pline("A wooden table.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-10, "carved bed")) {
			levl[u.ux][u.uy].typ = CARVEDBED;
			pline("A carved bed.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-14, "straw mattress")) {
			levl[u.ux][u.uy].typ = STRAWMATTRESS;
			pline("A straw mattress.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-4, "snow")) {
			levl[u.ux][u.uy].typ = SNOW;
			pline("Snow.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-3, "ash")) {
			levl[u.ux][u.uy].typ = ASH;
			pline("Ash.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-4, "sand")) {
			levl[u.ux][u.uy].typ = SAND;
			pline("Sand.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-11, "paved floor")) {
			levl[u.ux][u.uy].typ = PAVEDFLOOR;
			pline("Paved floor.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-7, "highway")) {
			levl[u.ux][u.uy].typ = HIGHWAY;
			pline("A highway.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-9, "grassland")) {
			levl[u.ux][u.uy].typ = GRASSLAND;
			pline("Grassland.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-11, "nether mist")) {
			levl[u.ux][u.uy].typ = NETHERMIST;
			pline("Nether mist.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-10, "stalactite")) {
			levl[u.ux][u.uy].typ = STALACTITE;
			pline("A stalactite.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-11, "crypt floor")) {
			levl[u.ux][u.uy].typ = CRYPTFLOOR;
			pline("A crypt floor.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-6, "bubble")) {
			levl[u.ux][u.uy].typ = BUBBLES;
			pline("A bubble.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-10, "rainycloud")) {
			levl[u.ux][u.uy].typ = RAINCLOUD;
			pline("A rain cloud.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-4, "sink")) {
			levl[u.ux][u.uy].typ = SINK;
			level.flags.nsinks++;
			pline("A sink.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-6, "toilet")) {
			levl[u.ux][u.uy].typ = TOILET;
			level.flags.nsinks++;
			pline("A toilet.");
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if(!BSTRCMP(bp, p-4, "pool")) {
			levl[u.ux][u.uy].typ = POOL;
			del_engr_at(u.ux, u.uy);
			pline("A pool.");
			/* Must manually make kelp! */
			water_damage(level.objects[u.ux][u.uy], FALSE, TRUE);
			newsym(u.ux, u.uy);
			return &zeroobj;
		}
		if (!BSTRCMP(bp, p-4, "lava")) {  /* also matches "molten lava" */
			levl[u.ux][u.uy].typ = LAVAPOOL;
			del_engr_at(u.ux, u.uy);
			pline("A pool of molten lava.");
			if (!(Levitation || Flying)) (void) lava_effects();
			newsym(u.ux, u.uy);
			return &zeroobj;
		}

		if(!BSTRCMP(bp, p-5, "altar")) {
		    aligntyp al;

		    levl[u.ux][u.uy].typ = ALTAR;
		    if(!strncmpi(bp, "chaotic ", 8))
			al = A_CHAOTIC;
		    else if(!strncmpi(bp, "neutral ", 8))
			al = A_NEUTRAL;
		    else if(!strncmpi(bp, "lawful ", 7))
			al = A_LAWFUL;
		    else if(!strncmpi(bp, "unaligned ", 10))
			al = A_NONE;
		    else /* -1 - A_CHAOTIC, 0 - A_NEUTRAL, 1 - A_LAWFUL */
			al = (!rn2(6)) ? A_NONE : rn2((int)A_LAWFUL+2) - 1;
		    levl[u.ux][u.uy].altarmask = Align2amask( al );
		    pline("%s altar.", An(align_str(al)));
		    newsym(u.ux, u.uy);
		    return(&zeroobj);
		}

		if(!BSTRCMP(bp, p-5, "grave") || !BSTRCMP(bp, p-9, "headstone")) {
		    make_grave(u.ux, u.uy, (char *) 0);
		    pline("A grave.");
		    newsym(u.ux, u.uy);
		    return(&zeroobj);
		}

		if(!BSTRCMP(bp, p-4, "tree")) {
		    levl[u.ux][u.uy].typ = TREE;
		    pline("A tree.");
		    newsym(u.ux, u.uy);
		    block_point(u.ux, u.uy);
		    return &zeroobj;
		}

		if(!BSTRCMP(bp, p-4, "bars")) {
		    levl[u.ux][u.uy].typ = IRONBARS;
		    pline("Iron bars.");
		    newsym(u.ux, u.uy);
		    return &zeroobj;
		}
	}
#endif
	if(!oclass) return((struct obj *)0);
any:
	if(!oclass) oclass = wrpsym[rn2((int)sizeof(wrpsym))];
typfnd:
	if (typ) oclass = objects[typ].oc_class;

	/* check for some objects that are not allowed */
	if (typ && objects[typ].oc_unique) {
#ifdef WIZARD
	    if (wizard)
		;	/* allow unique objects */
	    else
#endif
	    switch (typ) {
		case AMULET_OF_YENDOR:
		    typ = FAKE_AMULET_OF_YENDOR;
		    break;
		case CANDELABRUM_OF_INVOCATION:
		    typ = rnd_class(TALLOW_CANDLE, WAX_CANDLE);
		    break;
		case BELL_OF_OPENING:
		    typ = BELL;
		    break;
		case SPE_BOOK_OF_THE_DEAD:
		    typ = SPE_BLANK_PAPER;
		    break;
	    }
	}

	/* catch any other non-wishable objects */
	if (objects[typ].oc_nowish
#ifdef WIZARD
	    && !wizard
#endif
	    )
	    return((struct obj *)0);

	/* convert magic lamps to regular lamps before lighting them or setting
	   the charges */
	if ((typ == MAGIC_LAMP || typ == TREASURE_CHEST)
#ifdef WIZARD
				&& !wizard
#endif
						)
	    typ = OIL_LAMP;

	/* obviously don't allow wishing for scrolls of wishing --Amy */
	if ((typ == SCR_WISHING || typ == SCR_ASTRALCENSION || typ == SCR_ARTIFACT_CREATION || typ == SCR_ARTIFACT_JACKPOT || typ == SCR_RESURRECTION || typ == SCR_WORLD_FALL || typ == SCR_ACQUIREMENT || typ == SCR_ENTHRONIZATION || typ == SCR_MAKE_PENTAGRAM || typ == SCR_FOUNTAIN_BUILDING)
#ifdef WIZARD
				&& !wizard
#endif
	   ) typ = SCR_BLANK_PAPER;

	if (typ == WAN_DESLEXIFICATION && !wizard && !issoviet) typ = WAN_NOTHING;

	if(typ) {
		otmp = mksobj(typ, TRUE, FALSE, FALSE);
	} else {
		otmp = mkobj(oclass, FALSE, FALSE);
		if (otmp) typ = otmp->otyp;
	}
	if (!otmp) {
		pline("Unfortunately it failed.");
		return (struct obj *)0;
	}

	if (islit &&
		(typ == OIL_LAMP || typ == MAGIC_LAMP || 
		 typ == BRASS_LANTERN || typ == TORCH || 
		 Is_candle(otmp) || typ == POT_OIL)) {
	    place_object(otmp, u.ux, u.uy);  /* make it viable light source */
	    begin_burn(otmp, FALSE);
	    obj_extract_self(otmp);	 /* now release it for caller's use */
	}

	if(cnt > 0 && objects[typ].oc_merge && oclass != SPBOOK_CLASS &&
		(typ != CORPSE || !is_reviver(&mons[mntmp])) &&
		(cnt < rnd(6) ||
#ifdef WIZARD
		wizard ||
#endif
		 (cnt <= 7 && Is_candle(otmp)) ||
		 (cnt <= 7 && (typ == BATARANG) && Race_if(PM_BATMAN)) ||
		 (cnt <= 7 && (typ == DARK_BATARANG) && Race_if(PM_BATMAN)) ||
		 (cnt <= 100 && (typ != BOOMERANG) && (typ != SILVER_CHAKRAM) && (typ != BATARANG) && (typ != DARK_BATARANG) &&	/* idea by stenno - wishing for ammo can give up to 100 rounds of ammo */
		  ((oclass == WEAPON_CLASS && is_ammo(otmp))
				|| typ == ROCK || is_missile(otmp)))))
			otmp->quan = (long) cnt;

#ifdef WIZARD
	if (oclass == VENOM_CLASS) otmp->spe = 1;
#endif

	if (spesgn == 0) spe = otmp->spe;
#ifdef WIZARD
	else if (wizard) /* no alteration to spe */ ;
#endif
	else if (oclass == ARMOR_CLASS || oclass == WEAPON_CLASS || oclass == BALL_CLASS || oclass == CHAIN_CLASS ||
		 is_weptool(otmp) ||
			(oclass==RING_CLASS && objects[typ].oc_charged)) {
		if(spe > rnd(5) && spe > otmp->spe) spe = 0;
		if(spe > 2 && Luck < 0) spesgn = -1;
	} else {
		if (oclass == WAND_CLASS) {
			if (spe > 1 && spesgn == -1) spe = 1;
		} else {
			if (spe > 0 && spesgn == -1) spe = 0;
		}
		if (spe > otmp->spe) spe = otmp->spe;
	}

	if (spesgn == -1) spe = -spe;

	/* set otmp->spe.  This may, or may not, use spe... */
	switch (typ) {
		case TIN: if (contents==EMPTY) {
				otmp->corpsenm = NON_PM;
				otmp->spe = 0;
			} else if (contents==SPINACH) {
				otmp->corpsenm = NON_PM;
				otmp->spe = 1;
			} else if (contents==BEANS) {
				otmp->corpsenm = NON_PM;
				otmp->spe = 2;
			} else if (contents==PEACHES) {
				otmp->corpsenm = NON_PM;
				otmp->spe = 3;
			} else if (contents==FISH) {
				otmp->corpsenm = NON_PM;
				otmp->spe = 4;
			}
			break;
		case SLIME_MOLD: otmp->spe = ftype;
			/* Fall through */
		case SKELETON_KEY: case SECRET_KEY: case CHEST: case LARGE_BOX:
		/*case HEAVY_IRON_BALL:*/ /*case IRON_CHAIN:*/ case STATUE:
			/* otmp->cobj already done in mksobj() */
				break;
#ifdef MAIL
		case SCR_MAIL: otmp->spe = 1; break;
#endif
		case WAN_WISHING:
		case WAN_ACQUIREMENT:
#ifdef WIZARD
			if (!wizard) {
#endif
				otmp->spe = (rn2(10) ? -1 : 0);
				break;
#ifdef WIZARD
			}
			/* fall through, if wizard */
#endif
		default: otmp->spe = spe;
	}

	/* set otmp->corpsenm or dragon scale [mail] */
	if (mntmp >= LOW_PM) {
		if (mntmp == PM_LONG_WORM_TAIL) mntmp = PM_LONG_WORM;

		switch (typ) {
		case TIN:
			otmp->spe = 0; /* No spinach */
			if (dead_species(mntmp, FALSE)) {
			    otmp->corpsenm = NON_PM;	/* it's empty */
			} else if (!(mons[mntmp].geno & G_UNIQ) &&
				   !(mvitals[mntmp].mvflags & G_NOCORPSE) &&
				   mons[mntmp].cnutrit != 0) {
			    otmp->corpsenm = mntmp;
			}
			break;
		case CORPSE:
                        if
# ifdef WIZARD
                                ((wizard) ||
# endif /* WIZARD */
                                (!(mons[mntmp].geno & G_UNIQ) && rn2(2) && /* allow this wish to fail sometimes --Amy */
                                !(mons[mntmp].mlet == S_TROVE) && /* no wishing for exploitable troves --Amy */
					  !(mvitals[mntmp].mvflags & G_NOCORPSE))
# ifdef WIZARD
                                )
# endif /* WIZARD */
                                {
			    /* beware of random troll or lizard corpse,
			       or of ordinary one being forced to such */
			    if (otmp->timed) obj_stop_timers(otmp);
			    if (mons[mntmp].msound == MS_GUARDIAN)
			    	otmp->corpsenm = genus(mntmp,1);
			    else
				otmp->corpsenm = mntmp;
			    start_corpse_timeout(otmp);
			}
			break;
		case FIGURINE:
		case ENERGY_SAP:
                        if
# ifdef WIZARD
                                ((wizard) ||
# endif /* WIZARD */
                                ((!(mons[mntmp].geno & G_UNIQ) && !(mons[mntmp].mlet == S_TROVE)
			    && !is_human(&mons[mntmp]) && rn2(2) /* allow wishes for solar figurines to randomly fail --Amy */
# ifdef WIZARD
                                )
# endif /* WIZARD */
#ifdef MAIL
			    && mntmp != PM_MAIL_DAEMON
#endif
							))
				otmp->corpsenm = mntmp;
			break;
		case EGG:
			mntmp = can_be_hatched(mntmp);
			if (mntmp != NON_PM && !(mons[mntmp].mlet == S_TROVE) && (wizard || rn2(2)) ) { /* sometimes fail --Amy */
			    otmp->corpsenm = mntmp;
			    if (!dead_species(mntmp, TRUE))
				attach_egg_hatch_timeout(otmp);
			    else
				kill_egg(otmp);
			}
			break;
		case STATUE: if (wizard || (!rn2(5) && !(mons[mntmp].mlet == S_TROVE) ) ) otmp->corpsenm = mntmp;
			/* otherwise, you could wish for a statue of Master Kaen and cast stone to flesh!
			 * Well, you still can, but at least there's an 80% chance of the wish failing. --Amy */
			if (Has_contents(otmp) && verysmall(&mons[mntmp]))
			    delete_contents(otmp);	/* no spellbook */
			otmp->spe = ishistoric ? STATUE_HISTORIC : 0;
			break;
		case SCALE_MAIL:
			/* Dragon mail - depends on the order of objects */
			/*		 & dragons.			 */
			if (mntmp >= PM_GRAY_DRAGON &&
						mntmp <= PM_YELLOW_DRAGON)
			    otmp->otyp = GRAY_DRAGON_SCALE_MAIL +
						    mntmp - PM_GRAY_DRAGON;
			break;
		}
	}

	/* set blessed/cursed -- setting the fields directly is safe
	 * since weight() is called below and addinv() will take care
	 * of luck */
	if (iscursed) {
		curse(otmp);
	} else if (uncursed) {
		otmp->blessed = 0;
		otmp->cursed = (Luck < 0
#ifdef WIZARD
					 && !wizard
#endif
							);
	} else if (blessed) {
		otmp->blessed = (Luck >= 0
#ifdef WIZARD
					 || wizard
#endif
							);
		otmp->cursed = (Luck < 0
#ifdef WIZARD
					 && !wizard
#endif
							);
	} else if (spesgn < 0) {
		curse(otmp);
	}

	if (isinvisible)
	    otmp->oinvis = isinvisible > 0 && !always_visible(otmp);

	/* set eroded */
	/*if (is_damageable(otmp) || otmp->otyp == CRYSKNIFE) {*/ /* go away unnecessary check! --Amy */
	if (eroded && (is_flammable(otmp) || is_rustprone(otmp)))
		otmp->oeroded = eroded;
	if (eroded2 && (is_corrodeable(otmp) || is_rottable(otmp)))
		otmp->oeroded2 = eroded2;

	/* set erodeproof */
	if (erodeproof && !eroded && !eroded2)
	    otmp->oerodeproof = (Luck >= 0
#ifdef WIZARD
	     || wizard
#endif
		);
	/*}*/

	/* set otmp->recharged */
	if (oclass == WAND_CLASS) {
	    /* prevent wishing abuse */
	    if ( (otmp->otyp == WAN_WISHING || otmp->otyp == WAN_ACQUIREMENT)
#ifdef WIZARD
		    && !wizard
#endif
		) rechrg = 1;

		if (!wizard || (wizard && yn_function("Use non-wizmode recharging limit?", ynchars, 'y') == 'y') )
	    otmp->recharged = (unsigned)rechrg;
	}

	/* set poisoned */
	if (ispoisoned) {
	    if (is_poisonable(otmp))
		otmp->opoisoned = (Luck >= 0);
	    else if (Is_box(otmp) || typ == TIN)
		otmp->otrapped = 1;
	    else if (oclass == FOOD_CLASS)
		/* try to taint by making it as old as possible */
		otmp->age = 1L;
	}

	if (isgreased) otmp->greased = 1;

	if (isdiluted && otmp->oclass == POTION_CLASS &&
			otmp->otyp != POT_WATER)
		otmp->odiluted = 1;

	if (name && actualwish) {
		const char *aname;
		/*short*/int objtyp;
		char nname[256];
		strcpy(nname,name);

		/* an artifact name might need capitalization fixing */
		aname = artifact_name(name, &objtyp);
		if (aname && objtyp == otmp->otyp) name = aname;

# ifdef NOARTIFACTWISH
		/* Tom -- not always getting what you're wishing for... */                
		if (restrict_name(otmp, nname) && !rn2(3) && !wizard) {
		    int n = rn2((int)strlen(nname));
		    register char c1, c2;
		    c1 = lowc(nname[n]);
		    do c2 = 'a' + rn2('z'-'a'); while (c1 == c2);
		    nname[n] = (nname[n] == c1) ? c2 : highc(c2);  /* keep same case */
		}
# endif
		place_object(otmp, u.ux, u.uy);/* make it viable light source */
		otmp = oname(otmp, nname);
		obj_extract_self(otmp);	 /* now release it for caller's use */
		if (otmp->oartifact) {
			otmp->quan = 1L;
			u.uconduct.wisharti++;	/* KMH, conduct */
		}
	}

	/* more wishing abuse: don't allow wishing for certain artifacts */
	/* and make them pay; charge them for the wish anyway! */
	if ((is_quest_artifact(otmp) ||
	    /* [ALI] Can't wish for artifacts which have a set location */
	    (otmp->oartifact &&
	       (otmp->oartifact == ART_KEY_OF_CHAOS ||
	        otmp->oartifact == ART_KEY_OF_NEUTRALITY ||
	        otmp->oartifact == ART_KEY_OF_LAW ||
	        otmp->oartifact == ART_GAUNTLET_KEY ||
	        otmp->oartifact == ART_HAND_OF_VECNA ||
	        otmp->oartifact == ART_EYE_OF_THE_BEHOLDER ||
	        otmp->oartifact == ART_NIGHTHORN ||
	        otmp->oartifact == ART_MOTHERFUCKER_TROPHY ||
	        otmp->oartifact == ART_HELM_OF_KNOWLEDGE ||
	        otmp->oartifact == ART_BOOTS_OF_THE_MACHINE ||
	        otmp->oartifact == ART_ARKENSTONE_OF_THRAIN ||
	        otmp->oartifact == ART_BIZARRO_ORGASMATRON ||
	        otmp->oartifact == ART_KATIA_S_SOFT_COTTON ||
	        otmp->oartifact == ART_THIEFBANE)) ||
# ifdef NOARTIFACTWISH
/* Wishing for a "weak" artifact is easier than for a stronger one */
	(otmp->oartifact &&
	       (otmp->oartifact != ART_STING &&
		otmp->oartifact != ART_ELFRIST &&
		otmp->oartifact != ART_ORCRIST &&
		otmp->oartifact != ART_WEREBANE &&
		otmp->oartifact != ART_GRIMTOOTH &&
		otmp->oartifact != ART_DISRUPTER &&
		otmp->oartifact != ART_DEMONBANE &&
		otmp->oartifact != ART_DRAGONBANE &&
		otmp->oartifact != ART_TROLLSBANE &&
		otmp->oartifact != ART_GIANTKILLER &&
		otmp->oartifact != ART_OGRESMASHER &&
		otmp->oartifact != ART_SWORD_OF_BALANCE)) ||
# endif
	     (otmp->oartifact && nartifact_exist() == 0 && !rn2(20) ) ||
	     (otmp->oartifact && nartifact_exist() == 1 && !rn2(18) ) ||
	     (otmp->oartifact && nartifact_exist() == 2 && !rn2(16) ) ||
	     (otmp->oartifact && nartifact_exist() == 3 && !rn2(14) ) ||
	     (otmp->oartifact && nartifact_exist() == 4 && !rn2(12) ) ||
	     (otmp->oartifact && nartifact_exist() == 5 && !rn2(10) ) ||
	     (otmp->oartifact && nartifact_exist() == 6 && !rn2(9) ) ||
	     (otmp->oartifact && nartifact_exist() == 7 && !rn2(8) ) ||
	     (otmp->oartifact && nartifact_exist() == 8 && !rn2(7) ) ||
	     (otmp->oartifact && nartifact_exist() == 9 && !rn2(6) ) ||
	     (otmp->oartifact && nartifact_exist() == 10 && !rn2(5) ) ||
	     (otmp->oartifact && nartifact_exist() == 11 && !rn2(4) ) ||
	     (otmp->oartifact && nartifact_exist() == 12 && !rn2(3) ) ||
/* there should be a chance of an artifact wish failing even if none have been created yet --Amy 
 * but since there are so many more artifacts generated now, we increase the failure chance more slowly */
	     (otmp->oartifact && (nartifact_exist() > 12) && rn2(nartifact_exist()) > 8) ||
		 (Role_if(PM_PIRATE) && otmp->oartifact == ART_REAVER) || (isfriday && !rn2(5)) )
#ifdef WIZARD
	    && !wizard
#endif
	    ) {
	    artifact_exists(otmp, ONAME(otmp), FALSE);
	    if (Has_contents(otmp))
		delete_contents(otmp);
	    obfree(otmp, (struct obj *) 0);
	    otmp = &zeroobj;

	    if (!actualwish) return (&zeroobj);

	    pline("For a moment, you feel %s in your %s, but it disappears!",
		  something,
		  makeplural(body_part(HAND)));

		/* but you'll get another random artifact instead! --Amy */
		if (!issoviet) {

			boolean havegifts = u.ugifts;

			if (!havegifts) u.ugifts++;

			if (!rn2(isfriday ? 2 : 3)) {
				bad_artifact();
				return (&zeroobj);
			}

			otmp = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
			if (!otmp) return(&zeroobj);
			otmp->owt = weight(otmp);
			if (!havegifts) u.ugifts--;
			pline("But the RNG decided to grant you another artifact instead!");
			return(otmp);
		} else {
		/* In Soviet Russia, consolation prizes don't exist. After all, only hard work counts in communism, and no one is ever allowed to get anything for free. --Amy */

			pline("Kha kha kha KHAR KHAR zhelaniye ne udalas', poskol'ku Sovetskiy Pyat' Lo nenavidit vsekh zhivykh sushchestv i osobenno vas.");
			return (&zeroobj);
		}
	}

	if (halfeaten && otmp->oclass == FOOD_CLASS) {
		if (otmp->otyp == CORPSE)
			otmp->oeaten = mons[otmp->corpsenm].cnutrit;
		else otmp->oeaten = objects[otmp->otyp].oc_nutrition;
		/* (do this adjustment before setting up object's weight) */
		consume_oeaten(otmp, 1);
	}
	if (isdrained && otmp->otyp == CORPSE && mons[otmp->corpsenm].cnutrit) {
		int amt;
		otmp->odrained = 1;
		amt = mons[otmp->corpsenm].cnutrit - drainlevel(otmp);
		if (halfdrained) {
		    amt /= 2;
		    if (amt == 0)
			amt++;
		}
		/* (do this adjustment before setting up object's weight) */
		consume_oeaten(otmp, -amt);
	}
	otmp->owt = weight(otmp);
	if (very && otmp->otyp == HEAVY_IRON_BALL) otmp->owt += 600;

	return(otmp);
}

int
rnd_class(first,last)
int first,last;
{
	int i, x, sum=0;

	if (first > last) {
		impossible("rnd_class called with invalid values. first = %d, last = %d", first, last);
		return 0;
	}

	if (first == last)
	    return (first);
	for(i=first; i<=last; i++)
		sum += objects[i].oc_prob;
	if (!sum) /* all zero */
		return first + rn2(last-first+1);
	x = rnd(sum);
	for(i=first; i<=last; i++)
		if (objects[i].oc_prob && (x -= objects[i].oc_prob) <= 0)
			return i;
	return 0;
}

STATIC_OVL const char *
Alternate_item_name(i,alternate_items)
int i;
struct Jitem *alternate_items;
{
	while(alternate_items->item) {
		if (i == alternate_items->item)
			return alternate_items->name;
		alternate_items++;
	}
	return (const char *)0;
}

const char *
cloak_simple_name(cloak)
struct obj *cloak;
{
    if (cloak) {
	switch (cloak->otyp) {
	case ROBE:
	    return "robe";
	case MUMMY_WRAPPING:
	    return "wrapping";
	case LAB_COAT:
	    return "coat";
	case ALCHEMY_SMOCK:
	    return (objects[cloak->otyp].oc_name_known &&
			cloak->dknown) ? "smock" : "apron";
	default:
	    break;
	}
    }
    return "cloak";
}

const char *
mimic_obj_name(mtmp)
struct monst *mtmp;
{
	if (mtmp->m_ap_type == M_AP_OBJECT && mtmp->mappearance != STRANGE_OBJECT) {
		int idx = objects[mtmp->mappearance].oc_descr_idx;
		if (mtmp->mappearance == GOLD_PIECE) return "gold";
		return obj_descr[idx].oc_name;
	}
	return "whatcha-may-callit";
}
#endif /* OVLB */

/*objnam.c*/

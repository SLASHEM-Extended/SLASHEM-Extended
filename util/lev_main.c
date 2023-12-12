/*	SCCS Id: @(#)lev_main.c	3.4	2002/03/27	*/
/*	Copyright (c) 1989 by Jean-Christophe Collet */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This file contains the main function for the parser
 * and some useful functions needed by yacc
 */

/* IMPORTANT!!! If this file is changed, the changed version should be
 * both in src and util, otherwise the level compiler might not recognize new stuff! --Amy */

#define LEVEL_COMPILER	1	/* Enable definition of internal structures */

#include "hack.h"
#include "verinfo.h"
#include "sp_lev.h"
#ifdef STRICT_REF_DEF
#include "tcap.h"
#endif
/* ALI - We need the list of artifact names for artifact doors */
#define MAKEDEFS_C      /* We only want the names, not the complete defn */
#include "artilist.h"
#undef MAKEDEFS_C

#ifdef MAC
# ifdef MAC_MPW
#  define MPWTOOL
#  include <CursorCtl.h>
# else
#  if !defined(__MACH__)
#  define PREFIX ":lib:"	/* place output files here */
# endif
# endif
#endif

#ifdef WIN_CE
#define PREFIX "\\nethack\\dat\\"
#endif

#ifndef MPWTOOL
# define SpinCursor(x)
#endif

#if defined(AMIGA) && defined(DLB)
# define PREFIX "NH:slib/"
#endif

#ifndef O_WRONLY
#include <fcntl.h>
#endif
#ifndef O_CREAT	/* some older BSD systems do not define O_CREAT in <fcntl.h> */
#include <sys/file.h>
#endif
#ifndef O_BINARY	/* used for micros, no-op for others */
# define O_BINARY 0
#endif

#if defined(MICRO) || defined(WIN32)
# define OMASK FCMASK
#else
# define OMASK 0644
#endif

#define MAX_REGISTERS	10
#define ERR		(-1)

#define NewTab(type, size)	(type **) alloc(sizeof(type *) * size)
#define Write(fd, item, size)	if (write(fd, (void *)(item), size) != size) return FALSE;

#if defined(__BORLANDC__) && !defined(_WIN32)
extern unsigned _stklen = STKSIZ;
#endif
#define MAX_ERRORS	25

extern int  yyparse(void);
extern void init_yyin(FILE *);
extern void init_yyout(FILE *);

int  main(int, char **);
void yyerror(const char *);
void yywarning(const char *);
int  yywrap(void);
int get_artifact_id(char *);
int get_floor_type(CHAR_P);
int get_room_type(char *);
int get_trap_type(char *);
int get_monster_id(char *,CHAR_P);
int get_object_id(char *,CHAR_P);
boolean check_monster_char(CHAR_P);
boolean check_object_char(CHAR_P);
char what_map_char(CHAR_P);
void scan_map(char *);
void wallify_map(void);
boolean check_subrooms(void);
void check_coord(int,int,const char *);
void store_part(void);
void store_room(void);
boolean write_level_file(char *,splev *,specialmaze *);
void free_rooms(splev *);

extern void objects_init(void);
extern void decl_init(void);

static boolean write_common_data(int,int,lev_init *,long);
static boolean write_monsters(int,char *,monster ***);
static boolean write_objects(int,char *,object ***);
static boolean write_engravings(int,char *,engraving ***);
static boolean write_maze(int,specialmaze *);
static boolean write_rooms(int,splev *);
static void init_obj_classes(void);

static struct {
	const char *name;
	int type;
} trap_types[] = {
	{ "arrow",	ARROW_TRAP },
	{ "dart",	DART_TRAP },
	{ "falling rock", ROCKTRAP },
	{ "board",	SQKY_BOARD },
	{ "bear",	BEAR_TRAP },
	{ "land mine",	LANDMINE },
	{ "rolling boulder",	ROLLING_BOULDER_TRAP },
	{ "sleep gas",	SLP_GAS_TRAP },
	{ "rust",	RUST_TRAP },
	{ "fire",	FIRE_TRAP },
	{ "pit",	PIT },
	{ "spiked pit",	SPIKED_PIT },
	{ "hole",	HOLE },
	{ "trap door",	TRAPDOOR },
	{ "teleport",	TELEP_TRAP },
	{ "level teleport", LEVEL_TELEP },
	{ "magic portal",   MAGIC_PORTAL },
	{ "web",	WEB },
	{ "statue",	STATUE_TRAP },
	{ "magic",	MAGIC_TRAP },
	{ "anti magic",	ANTI_MAGIC },
	{ "polymorph",	POLY_TRAP },
	{ "ice",	ICE_TRAP },
	{ "spear", SPEAR_TRAP },
	{ "collapse",   COLLAPSE_TRAP },
	{ "magic beam", MAGIC_BEAM_TRAP },
	{ "shit trap", SHIT_TRAP },
	{ "animation trap", ANIMATION_TRAP },
	{ "glyph of warding", GLYPH_OF_WARDING },
	{ "blade trap", SCYTHING_BLADE },
	{ "bolt trap", BOLT_TRAP },
	{ "acidpool", ACID_POOL },
	{ "waterpool", WATER_POOL },
	{ "poison gas", POISON_GAS_TRAP },
	{ "slow gas", SLOW_GAS_TRAP },
	{ "shock", SHOCK_TRAP },
	{ "rmb loss", RMB_LOSS_TRAP },
	{ "display", DISPLAY_TRAP },
	{ "spell loss", SPELL_LOSS_TRAP },
	{ "yellow spell", YELLOW_SPELL_TRAP },
	{ "auto destruct", AUTO_DESTRUCT_TRAP },
	{ "memory", MEMORY_TRAP },
	{ "inventory", INVENTORY_TRAP },
	{ "black ng wall", BLACK_NG_WALL_TRAP },
	{ "menu", MENU_TRAP },
	{ "speed", SPEED_TRAP },
	{ "superscroller", SUPERSCROLLER_TRAP },
	{ "active superscroller", ACTIVE_SUPERSCROLLER_TRAP },
	{ "swarm", SWARM_TRAP },
	{ "switcher", AUTOMATIC_SWITCHER },
	{ "unknown trap", UNKNOWN_TRAP },
	{ "heel trap", HEEL_TRAP },
	{ "vulnerability trap", VULN_TRAP },

	{ "lava trap", LAVA_TRAP},
	{ "flood trap", FLOOD_TRAP},
	{ "drain trap", DRAIN_TRAP},
	{ "free hand", FREE_HAND_TRAP},
	{ "disintegration", DISINTEGRATION_TRAP},
	{ "unidentify", UNIDENTIFY_TRAP},
	{ "thirst", THIRST_TRAP},
	{ "luck", LUCK_TRAP},
	{ "shades of grey", SHADES_OF_GREY_TRAP},
	{ "item teleporter", ITEM_TELEP_TRAP},
	{ "gravity", GRAVITY_TRAP},
	{ "death trap", DEATH_TRAP},
	{ "stone to flesh trap", STONE_TO_FLESH_TRAP},
	{ "quicksand", QUICKSAND_TRAP},
	{ "faint", FAINT_TRAP},
	{ "curse", CURSE_TRAP},
	{ "difficulty", DIFFICULTY_TRAP},
	{ "sound", SOUND_TRAP},
	{ "caster", CASTER_TRAP},
	{ "weakness", WEAKNESS_TRAP},
	{ "rot thirteen", ROT_THIRTEEN_TRAP},
	{ "bishop", BISHOP_TRAP},
	{ "confusion", CONFUSION_TRAP},
	{ "nupesell", NUPESELL_TRAP},
	{ "drop", DROP_TRAP},
	{ "dstw", DSTW_TRAP},
	{ "status", STATUS_TRAP},
	{ "alignment", ALIGNMENT_TRAP},
	{ "stairs", STAIRS_TRAP},

	{ "pestilence", PESTILENCE_TRAP},
	{ "famine", FAMINE_TRAP},

	{ "relocation", RELOCATION_TRAP},

	{ "uninformation", UNINFORMATION_TRAP},

	{ "intrinsic loss", INTRINSIC_LOSS_TRAP},
	{ "blood loss", BLOOD_LOSS_TRAP},
	{ "bad effect", BAD_EFFECT_TRAP},
	{ "multiply", MULTIPLY_TRAP},
	{ "auto vulnerability", AUTO_VULN_TRAP},
	{ "tele items", TELE_ITEMS_TRAP},
	{ "nastiness", NASTINESS_TRAP},

	{ "recursion", RECURSION_TRAP},
	{ "respawn", RESPAWN_TRAP},
	{ "warpzone", WARP_ZONE},
	{ "captcha", CAPTCHA_TRAP},
	{ "mind wipe", MIND_WIPE_TRAP},
	{ "throwing star", THROWING_STAR_TRAP},
	{ "shit pit", SHIT_PIT},
	{ "shaft", SHAFT_TRAP},
	{ "lock trap", LOCK_TRAP},
	{ "MC trap", MAGIC_CANCELLATION_TRAP},
	{ "farlook", FARLOOK_TRAP},
	{ "gateway from hell", GATEWAY_FROM_HELL},
	{ "growing", GROWING_TRAP},
	{ "cooling", COOLING_TRAP},
	{ "bars", BAR_TRAP},
	{ "locking", LOCKING_TRAP},
	{ "air", AIR_TRAP},
	{ "terrain", TERRAIN_TRAP},

	{ "loudspeaker", LOUDSPEAKER},
	{ "nest", NEST_TRAP},
	{ "bullet", BULLET_TRAP},
	{ "paralysis", PARALYSIS_TRAP},
	{ "cyanide", CYANIDE_TRAP},
	{ "laser", LASER_TRAP},
	{ "giant chasm", GIANT_CHASM},
	{ "fart", FART_TRAP},
	{ "confuse", CONFUSE_TRAP},
	{ "stun", STUN_TRAP},
	{ "hallu", HALLUCINATION_TRAP},
	{ "stoning", PETRIFICATION_TRAP},
	{ "numbness", NUMBNESS_TRAP},
	{ "freezing", FREEZING_TRAP},
	{ "burning", BURNING_TRAP},
	{ "fear", FEAR_TRAP},
	{ "blindness", BLINDNESS_TRAP},
	{ "glib", GLIB_TRAP},
	{ "slime", SLIME_TRAP},
	{ "inertia", INERTIA_TRAP},
	{ "time", TIME_TRAP},
	{ "lycanthropy", LYCANTHROPY_TRAP},
	{ "unlight", UNLIGHT_TRAP},
	{ "elemental", ELEMENTAL_TRAP},
	{ "escalating", ESCALATING_TRAP},
	{ "negative", NEGATIVE_TRAP},
	{ "mana", MANA_TRAP},
	{ "sin", SIN_TRAP},
	{ "destroy armor", DESTROY_ARMOR_TRAP},
	{ "divine anger", DIVINE_ANGER_TRAP},
	{ "genetic", GENETIC_TRAP},
	{ "missingno", MISSINGNO_TRAP},
	{ "cancellation", CANCELLATION_TRAP},
	{ "hostility", HOSTILITY_TRAP},
	{ "boss", BOSS_TRAP},
	{ "wishing", WISHING_TRAP},
	{ "amnesia", RECURRING_AMNESIA_TRAP},
	{ "bigscript", BIGSCRIPT_TRAP},
	{ "bank", BANK_TRAP},
	{ "only", ONLY_TRAP},
	{ "map", MAP_TRAP},
	{ "tech", TECH_TRAP},
	{ "disenchant", DISENCHANT_TRAP},
	{ "verisiert", VERISIERT},
	{ "chaos", CHAOS_TRAP},
	{ "muteness", MUTENESS_TRAP},
	{ "ntll", NTLL_TRAP},
	{ "engraving", ENGRAVING_TRAP},
	{ "magic device", MAGIC_DEVICE_TRAP},
	{ "book", BOOK_TRAP},
	{ "level", LEVEL_TRAP},
	{ "quiz", QUIZ_TRAP},

	{ "falling boulder", FALLING_BOULDER_TRAP},
	{ "glass arrow", GLASS_ARROW_TRAP},
	{ "glass bolt", GLASS_BOLT_TRAP},
	{ "out of magic", OUT_OF_MAGIC_TRAP},
	{ "plasma", PLASMA_TRAP},
	{ "bomb", BOMB_TRAP},
	{ "earthquake", EARTHQUAKE_TRAP},
	{ "noise", NOISE_TRAP},
	{ "glue", GLUE_TRAP},
	{ "guillotine", GUILLOTINE_TRAP},
	{ "bisection", BISECTION_TRAP},
	{ "volt", VOLT_TRAP},
	{ "horde", HORDE_TRAP},
	{ "immobility", IMMOBILITY_TRAP},
	{ "green glyph", GREEN_GLYPH},
	{ "blue glyph", BLUE_GLYPH},
	{ "yellow glyph", YELLOW_GLYPH},
	{ "orange glyph", ORANGE_GLYPH},
	{ "black glyph", BLACK_GLYPH},
	{ "purple glyph", PURPLE_GLYPH},

	{ "metabolic", METABOLIC_TRAP},
	{ "no return", TRAP_OF_NO_RETURN},
	{ "egotype", EGOTRAP},
	{ "fast forward", FAST_FORWARD_TRAP},
	{ "rottenness", TRAP_OF_ROTTENNESS},
	{ "unskilled", UNSKILLED_TRAP},
	{ "low stats", LOW_STATS_TRAP},
	{ "training", TRAINING_TRAP},
	{ "exercise", EXERCISE_TRAP},
	{ "falling loadstone", FALLING_LOADSTONE_TRAP},
	{ "undead summoning", SUMMON_UNDEAD_TRAP},
	{ "falling nastystone", FALLING_NASTYSTONE_TRAP},

	{ "spined ball",	SPINED_BALL_TRAP },
	{ "pendulum",	PENDULUM_TRAP },
	{ "turn table",	TURN_TABLE },
	{ "scent",	SCENT_TRAP },
	{ "banana",	BANANA_TRAP },
	{ "falling tub",	FALLING_TUB_TRAP },
	{ "alarm",	ALARM },
	{ "caltrops",	CALTROPS_TRAP },
	{ "blade wire",	BLADE_WIRE },
	{ "magnet",	MAGNET_TRAP },
	{ "slingshot",	SLINGSHOT_TRAP },
	{ "cannon",	CANNON_TRAP },
	{ "venom sprinkler",	VENOM_SPRINKLER },
	{ "fumarole",	FUMAROLE },

	{ "mana pit",	MANA_PIT },
	{ "elemental portal",	ELEMENTAL_PORTAL },
	{ "girliness",	GIRLINESS_TRAP },
	{ "fumbling",	FUMBLING_TRAP },
	{ "egomonster",	EGOMONSTER_TRAP },
	{ "flooding",	FLOODING_TRAP },
	{ "monster cube",	MONSTER_CUBE },
	{ "cursed grave",	CURSED_GRAVE },
	{ "limitation",	LIMITATION_TRAP },
	{ "weak sight",	WEAK_SIGHT_TRAP },
	{ "random message",	RANDOM_MESSAGE_TRAP },

	{ "desecration",	DESECRATION_TRAP },
	{ "starvation",	STARVATION_TRAP },
	{ "dropless",	DROPLESS_TRAP },
	{ "low effect",	LOW_EFFECT_TRAP },
	{ "invis",	INVISIBLE_TRAP },
	{ "ghost world",	GHOST_WORLD_TRAP },
	{ "dehydration",	DEHYDRATION_TRAP },
	{ "hate",	HATE_TRAP },
	{ "spacewars",	SPACEWARS_TRAP },
	{ "temporary recursion",	TEMPORARY_RECURSION_TRAP },
	{ "totter",	TOTTER_TRAP },
	{ "nonintrinsic",	NONINTRINSICAL_TRAP },
	{ "dropcurse",	DROPCURSE_TRAP },
	{ "nakedness",	NAKEDNESS_TRAP },
	{ "antilevel",	ANTILEVEL_TRAP },
	{ "ventilator",	VENTILATOR },
	{ "stealer",	STEALER_TRAP },
	{ "rebellion",	REBELLION_TRAP },
	{ "crap",	CRAP_TRAP },
	{ "misfire",	MISFIRE_TRAP },
	{ "walls",	TRAP_OF_WALLS },

	{ "disconnect",	DISCONNECT_TRAP },
	{ "interface screw",	INTERFACE_SCREW_TRAP },
	{ "dimness",	DIMNESS_TRAP },
	{ "evil artifact",	EVIL_ARTIFACT_TRAP },
	{ "bossfight",	BOSSFIGHT_TRAP },
	{ "entire level",	ENTIRE_LEVEL_TRAP },
	{ "bones",	BONES_TRAP },
	{ "rodney",	RODNEY_TRAP },
	{ "autocurse",	AUTOCURSE_TRAP },
	{ "highlevel",	HIGHLEVEL_TRAP },
	{ "spell forgetting",	SPELL_FORGETTING_TRAP },
	{ "sound effect",	SOUND_EFFECT_TRAP },

	{ "trap percents", TRAP_PERCENTS },

	{ "nexus", NEXUS_TRAP },
	{ "leg", LEG_TRAP },
	{ "artifact jackpot", ARTIFACT_JACKPOT_TRAP },
	{ "map amnesia", MAP_AMNESIA_TRAP },

	{ "spreading", SPREADING_TRAP },
	{ "adjacent", ADJACENT_TRAP },
	{ "superthing", SUPERTHING_TRAP },
	{ "current shaft", CURRENT_SHAFT },
	{ "levitation", LEVITATION_TRAP },
	{ "bowel cramps", BOWEL_CRAMPS_TRAP },
	{ "unequipping", UNEQUIPPING_TRAP },
	{ "good artifact", GOOD_ARTIFACT_TRAP },
	{ "gender", GENDER_TRAP },
	{ "opposite alignment", TRAP_OF_OPPOSITE_ALIGNMENT },
	{ "sincount", SINCOUNT_TRAP },
	{ "beamer", BEAMER_TRAP },
	{ "level beamer", LEVEL_BEAMER },
	{ "pet", PET_TRAP },
	{ "piercing beam", PIERCING_BEAM_TRAP },
	{ "wrenching", WRENCHING_TRAP },
	{ "tracker", TRACKER_TRAP },
	{ "nurse", NURSE_TRAP },
	{ "back to start", BACK_TO_START_TRAP },
	{ "nemesis", NEMESIS_TRAP },
	{ "strew", STREW_TRAP },
	{ "outta depth", OUTTA_DEPTH_TRAP },
	{ "punishment", PUNISHMENT_TRAP },
	{ "boon", BOON_TRAP },
	{ "fountain", FOUNTAIN_TRAP },
	{ "throne", THRONE_TRAP },
	{ "anoxic pit", ANOXIC_PIT },
	{ "arabella speaker", ARABELLA_SPEAKER },
	{ "femmy", FEMMY_TRAP },
	{ "madeleine", MADELEINE_TRAP },
	{ "marlena", MARLENA_TRAP },
	{ "sabrina", SABRINA_TRAP },
	{ "anastasia", ANASTASIA_TRAP },
	{ "filler", FILLER_TRAP },
	{ "toxic venom", TOXIC_VENOM_TRAP },
	{ "insanity", INSANITY_TRAP },
	{ "madness", MADNESS_TRAP },
	{ "jessica", JESSICA_TRAP },
	{ "solvejg", SOLVEJG_TRAP },
	{ "wendy", WENDY_TRAP },
	{ "katharina", KATHARINA_TRAP },
	{ "elena", ELENA_TRAP },
	{ "thai", THAI_TRAP },
	{ "elif", ELIF_TRAP },
	{ "nadja", NADJA_TRAP },
	{ "sandra", SANDRA_TRAP },
	{ "natalje", NATALJE_TRAP },
	{ "jeanetta", JEANETTA_TRAP },
	{ "yvonne", YVONNE_TRAP },
	{ "maurah", MAURAH_TRAP },

	{ "lootcut", LOOTCUT_TRAP },
	{ "monster speed", MONSTER_SPEED_TRAP },
	{ "scaling", SCALING_TRAP },
	{ "enmity", ENMITY_TRAP },
	{ "white spell", WHITE_SPELL_TRAP },
	{ "complete gray spell", COMPLETE_GRAY_SPELL_TRAP },
	{ "quasar", QUASAR_TRAP },
	{ "momma", MOMMA_TRAP },
	{ "horror", HORROR_TRAP },
	{ "artificer", ARTIFICER_TRAP },
	{ "wereform", WEREFORM_TRAP },
	{ "nonprayer", NON_PRAYER_TRAP },
	{ "evil patch", EVIL_PATCH_TRAP },
	{ "hard mode", HARD_MODE_TRAP },
	{ "secret attack", SECRET_ATTACK_TRAP },
	{ "eater", EATER_TRAP },
	{ "covetousness", COVETOUSNESS_TRAP },
	{ "not seen", NOT_SEEN_TRAP },
	{ "dark mode", DARK_MODE_TRAP },
	{ "antisearch", ANTISEARCH_TRAP },
	{ "homicide", HOMICIDE_TRAP },
	{ "nasty nation", NASTY_NATION_TRAP },
	{ "wakeup call", WAKEUP_CALL_TRAP },
	{ "grayout", GRAYOUT_TRAP },
	{ "gray center", GRAY_CENTER_TRAP },
	{ "checkerboard", CHECKERBOARD_TRAP },
	{ "clockwise spin", CLOCKWISE_SPIN_TRAP },
	{ "counterclockwise spin", COUNTERCLOCKWISE_SPIN_TRAP },
	{ "lag", LAG_TRAP },
	{ "blesscurse", BLESSCURSE_TRAP },
	{ "delight", DE_LIGHT_TRAP },
	{ "discharge", DISCHARGE_TRAP },
	{ "trashing", TRASHING_TRAP },
	{ "filtering", FILTERING_TRAP },
	{ "deformatting", DEFORMATTING_TRAP },
	{ "flicker strip", FLICKER_STRIP_TRAP },
	{ "undressing", UNDRESSING_TRAP },
	{ "hyperbluewall", HYPERBLUEWALL_TRAP },
	{ "nolite", NOLITE_TRAP },
	{ "paranoia", PARANOIA_TRAP },
	{ "fleecescript", FLEECESCRIPT_TRAP },
	{ "interrupt", INTERRUPT_TRAP },
	{ "dustbin", DUSTBIN_TRAP },
	{ "mana battery", MANA_BATTERY_TRAP },
	{ "monsterfingers", MONSTERFINGERS_TRAP },
	{ "miscast", MISCAST_TRAP },
	{ "message suppression", MESSAGE_SUPPRESSION_TRAP },
	{ "stuck announcement", STUCK_ANNOUNCEMENT_TRAP },
	{ "bloodthirsty", BLOODTHIRSTY_TRAP },
	{ "maximum damage", MAXIMUM_DAMAGE_TRAP },
	{ "latency", LATENCY_TRAP },
	{ "starlit", STARLIT_TRAP },
	{ "knowledge", KNOWLEDGE_TRAP },
	{ "highscore", HIGHSCORE_TRAP },
	{ "pink spell", PINK_SPELL_TRAP },
	{ "green spell", GREEN_SPELL_TRAP },
	{ "evc", EVC_TRAP },
	{ "underlayer", UNDERLAYER_TRAP },
	{ "damage meter", DAMAGE_METER_TRAP },
	{ "arbitrary weight", ARBITRARY_WEIGHT_TRAP },
	{ "fucked info", FUCKED_INFO_TRAP },
	{ "black spell", BLACK_SPELL_TRAP },
	{ "cyan spell", CYAN_SPELL_TRAP },
	{ "heap", HEAP_TRAP },
	{ "blue spell", BLUE_SPELL_TRAP },
	{ "tron", TRON_TRAP },
	{ "red spell", RED_SPELL_TRAP },
	{ "too heavy", TOO_HEAVY_TRAP },
	{ "elongation", ELONGATION_TRAP },
	{ "wrapover", WRAPOVER_TRAP },
	{ "destruction", DESTRUCTION_TRAP },
	{ "melee prefix", MELEE_PREFIX_TRAP },
	{ "automore", AUTOMORE_TRAP },
	{ "unfair attack", UNFAIR_ATTACK_TRAP },

	{ "kop cube", KOP_CUBE },
	{ "boss spawner", BOSS_SPAWNER },
	{ "contamination", CONTAMINATION_TRAP },

	{ "evil heel", EVIL_HEEL_TRAP },
	{ "bad equipment", BAD_EQUIPMENT_TRAP },
	{ "tempoconflict", TEMPOCONFLICT_TRAP },
	{ "tempohunger", TEMPOHUNGER_TRAP },
	{ "teleportitis", TELEPORTITIS_TRAP },
	{ "polymorphitis", POLYMORPHITIS_TRAP },
	{ "premature death", PREMATURE_DEATH_TRAP },
	{ "lasting amnesia", LASTING_AMNESIA_TRAP },
	{ "ragnarok", RAGNAROK_TRAP },
	{ "single disenchant", SINGLE_DISENCHANT_TRAP },
	{ "acid pit", ACID_PIT },

	{ "severe disenchant", SEVERE_DISENCHANT_TRAP },
	{ "pain", PAIN_TRAP },
	{ "trembling", TREMBLING_TRAP },
	{ "techcap", TECHCAP_TRAP },
	{ "spell memory", SPELL_MEMORY_TRAP },
	{ "skill reduction", SKILL_REDUCTION_TRAP },
	{ "skillcap", SKILLCAP_TRAP },
	{ "permanent stat damage", PERMANENT_STAT_DAMAGE_TRAP },

	{ "orange spell", ORANGE_SPELL_TRAP },
	{ "violet spell", VIOLET_SPELL_TRAP },
	{ "longing", TRAP_OF_LONGING },
	{ "cursed part", CURSED_PART_TRAP },
	{ "quaversal", QUAVERSAL_TRAP },
	{ "appearance shuffling", APPEARANCE_SHUFFLING_TRAP },
	{ "brown spell", BROWN_SPELL_TRAP },
	{ "choiceless", CHOICELESS_TRAP },
	{ "goldspell", GOLDSPELL_TRAP },
	{ "deprovement", DEPROVEMENT_TRAP },
	{ "initialization", INITIALIZATION_TRAP },
	{ "gushlush", GUSHLUSH_TRAP },
	{ "soiltype", SOILTYPE_TRAP },
	{ "dangerous terrain", DANGEROUS_TERRAIN_TRAP },
	{ "fallout", FALLOUT_TRAP },
	{ "mojibake", MOJIBAKE_TRAP },
	{ "gravation", GRAVATION_TRAP },
	{ "uncalled", UNCALLED_TRAP },
	{ "exploding dice", EXPLODING_DICE_TRAP },
	{ "permacurse", PERMACURSE_TRAP },
	{ "shrouded identity", SHROUDED_IDENTITY_TRAP },
	{ "feeler gauges", FEELER_GAUGES_TRAP },
	{ "long screwup", LONG_SCREWUP_TRAP },
	{ "wing yellow changer", WING_YELLOW_CHANGER },
	{ "life saving", LIFE_SAVING_TRAP },
	{ "curseuse", CURSEUSE_TRAP },
	{ "cut nutrition", CUT_NUTRITION_TRAP },
	{ "skill loss", SKILL_LOSS_TRAP },
	{ "autopilot", AUTOPILOT_TRAP },
	{ "force", FORCE_TRAP },
	{ "monster glyph", MONSTER_GLYPH_TRAP },
	{ "changing directive", CHANGING_DIRECTIVE_TRAP },
	{ "container kaboom", CONTAINER_KABOOM_TRAP },
	{ "steal degrade", STEAL_DEGRADE_TRAP },
	{ "left inventory", LEFT_INVENTORY_TRAP },
	{ "fluctuating speed", FLUCTUATING_SPEED_TRAP },
	{ "tarmustrokingnora", TARMUSTROKINGNORA_TRAP },
	{ "failure", FAILURE_TRAP },
	{ "bright cyan spell", BRIGHT_CYAN_SPELL_TRAP },
	{ "frequentation spawn", FREQUENTATION_SPAWN_TRAP },
	{ "pet ai", PET_AI_TRAP },
	{ "satan", SATAN_TRAP },
	{ "rememberance", REMEMBERANCE_TRAP },
	{ "pokelie", POKELIE_TRAP },
	{ "autopickup", AUTOPICKUP_TRAP },
	{ "dywypi", DYWYPI_TRAP },
	{ "silver spell", SILVER_SPELL_TRAP },
	{ "metal spell", METAL_SPELL_TRAP },
	{ "platinum spell", PLATINUM_SPELL_TRAP },
	{ "manler", MANLER_TRAP },
	{ "doorning", DOORNING_TRAP },
	{ "nownsible", NOWNSIBLE_TRAP },
	{ "elm street", ELM_STREET_TRAP },
	{ "monnoise", MONNOISE_TRAP },
	{ "rang call", RANG_CALL_TRAP },
	{ "recurring spell loss", RECURRING_SPELL_LOSS_TRAP },
	{ "antitraining", ANTITRAINING_TRAP },
	{ "techout", TECHOUT_TRAP },
	{ "stat decay", STAT_DECAY_TRAP },
	{ "movemork", MOVEMORK_TRAP },

	{ "hybrid", HYBRID_TRAP },
	{ "shapechange", SHAPECHANGE_TRAP },
	{ "meltem", MELTEM_TRAP },
	{ "miguc", MIGUC_TRAP },
	{ "directive", DIRECTIVE_TRAP },
	{ "satatue", SATATUE_TRAP },
	{ "farting web", FARTING_WEB },
	{ "cataclysm", CATACLYSM_TRAP },
	{ "data delete", DATA_DELETE_TRAP },
	{ "elder tentacling", ELDER_TENTACLING_TRAP },
	{ "footerer", FOOTERER_TRAP },

	{ "bad part", BAD_PART_TRAP },
	{ "completely bad part", COMPLETELY_BAD_PART_TRAP },
	{ "evil variant", EVIL_VARIANT_TRAP },

	{ "grave wall", GRAVE_WALL_TRAP },
	{ "tunnel", TUNNEL_TRAP },
	{ "farmland", FARMLAND_TRAP },
	{ "mountain", MOUNTAIN_TRAP },
	{ "water tunnel", WATER_TUNNEL_TRAP },
	{ "crystal flood", CRYSTAL_FLOOD_TRAP },
	{ "moorland", MOORLAND_TRAP },
	{ "urine", URINE_TRAP },
	{ "shifting sand", SHIFTING_SAND_TRAP },
	{ "styx", STYX_TRAP },
 	{ "pentagram", PENTAGRAM_TRAP },
	{ "snow", SNOW_TRAP },
	{ "ash", ASH_TRAP },
	{ "sand", SAND_TRAP },
	{ "pavement", PAVEMENT_TRAP },
	{ "highway", HIGHWAY_TRAP },
	{ "grassland", GRASSLAND_TRAP },
	{ "nether mist", NETHER_MIST_TRAP },
	{ "stalactite", STALACTITE_TRAP },
	{ "cryptfloor", CRYPTFLOOR_TRAP },
	{ "bubble", BUBBLE_TRAP },
	{ "rain cloud", RAIN_CLOUD_TRAP },

	{ "item nastification", ITEM_NASTIFICATION_TRAP },
	{ "sanity increase", SANITY_INCREASE_TRAP },
	{ "psi", PSI_TRAP },
	{ "gay", GAY_TRAP },

	{ "sarah", SARAH_TRAP },
	{ "claudia", CLAUDIA_TRAP },
	{ "ludgera", LUDGERA_TRAP },
	{ "kati", KATI_TRAP },

	{ "sanity treble", SANITY_TREBLE_TRAP },
	{ "stat decrease", STAT_DECREASE_TRAP },
	{ "simeout", SIMEOUT_TRAP },
	{ "spressing", S_PRESSING_TRAP },

	{ "nelly", NELLY_TRAP },
	{ "eveline", EVELINE_TRAP },
	{ "karin", KARIN_TRAP },
	{ "juen", JUEN_TRAP },
	{ "kristina", KRISTINA_TRAP },
	{ "lou", LOU_TRAP },
	{ "almut", ALMUT_TRAP },
	{ "julietta", JULIETTA_TRAP },
	{ "arabella", ARABELLA_TRAP },

	{ "kristin", KRISTIN_TRAP },
	{ "anna", ANNA_TRAP },
	{ "ruea", RUEA_TRAP },
	{ "dora", DORA_TRAP },
	{ "marike", MARIKE_TRAP },
	{ "jette", JETTE_TRAP },
	{ "ina", INA_TRAP },
	{ "sing", SING_TRAP },
	{ "victoria", VICTORIA_TRAP },
	{ "melissa", MELISSA_TRAP },
	{ "anita", ANITA_TRAP },
	{ "henrietta", HENRIETTA_TRAP },
	{ "verena", VERENA_TRAP },

	{ "annemarie", ANNEMARIE_TRAP },
	{ "jil", JIL_TRAP },
	{ "jana", JANA_TRAP },
	{ "katrin", KATRIN_TRAP },
	{ "gudrun", GUDRUN_TRAP },
	{ "ella", ELLA_TRAP },
	{ "manuela", MANUELA_TRAP },
	{ "jennifer", JENNIFER_TRAP },
	{ "patricia", PATRICIA_TRAP },
	{ "antje", ANTJE_TRAP },
	{ "antjex", ANTJE_TRAP_X },
	{ "kerstin", KERSTIN_TRAP },
	{ "laura", LAURA_TRAP },
	{ "larissa", LARISSA_TRAP },
	{ "nora", NORA_TRAP },
	{ "natalia", NATALIA_TRAP },
	{ "susanne", SUSANNE_TRAP },
	{ "lisa", LISA_TRAP },
	{ "bridghitte", BRIDGHITTE_TRAP },
	{ "julia", JULIA_TRAP },
	{ "nicole", NICOLE_TRAP },
	{ "rita", RITA_TRAP },
	{ "janina", JANINA_TRAP },
	{ "rosa", ROSA_TRAP },

	{ "ksenia", KSENIA_TRAP },
	{ "lydia", LYDIA_TRAP },
	{ "conny", CONNY_TRAP },
	{ "katia", KATIA_TRAP },
	{ "mariya", MARIYA_TRAP },
	{ "elise", ELISE_TRAP },
	{ "ronja", RONJA_TRAP },
	{ "ariane", ARIANE_TRAP },
	{ "johanna", JOHANNA_TRAP },
	{ "inge", INGE_TRAP },

	{ "walltrap", WALL_TRAP },
	{ "monster generator", MONSTER_GENERATOR },
	{ "potion dispenser", POTION_DISPENSER },
	{ "spacewars spawn", SPACEWARS_SPAWN_TRAP },
	{ "tv tropes", TV_TROPES_TRAP },
	{ "symbiote", SYMBIOTE_TRAP },
	{ "kill symbiote", KILL_SYMBIOTE_TRAP },
	{ "symbiote replacement", SYMBIOTE_REPLACEMENT_TRAP },
	{ "shutdown", SHUTDOWN_TRAP },
	{ "corona", CORONA_TRAP },
	{ "unproofing", UNPROOFING_TRAP },
	{ "visibility", VISIBILITY_TRAP },
	{ "feminism stone", FEMINISM_STONE_TRAP },
	{ "branch teleporter", BRANCH_TELEPORTER },
	{ "branch beamer", BRANCH_BEAMER },
	{ "poison arrow", POISON_ARROW_TRAP },
	{ "poison bolt", POISON_BOLT_TRAP },
	{ "macetrap", MACE_TRAP },
	{ "shueft", SHUEFT_TRAP },
	{ "moth larvae", MOTH_LARVAE_TRAP },
	{ "worthiness", WORTHINESS_TRAP },
	{ "conduct", CONDUCT_TRAP },
	{ "strikethrough", STRIKETHROUGH_TRAP },
	{ "multiple gather", MULTIPLE_GATHER_TRAP },
	{ "vivisection", VIVISECTION_TRAP },
	{ "instafeminism", INSTAFEMINISM_TRAP },
	{ "instanasty", INSTANASTY_TRAP },
	{ "skill point loss", SKILL_POINT_LOSS_TRAP },
	{ "perfect match", PERFECT_MATCH_TRAP },
	{ "dumbie lightsaber", DUMBIE_LIGHTSABER_TRAP },
	{ "wrong stairs", WRONG_STAIRS },
	{ "techstop", TECHSTOP_TRAP },
	{ "amnesia switch", AMNESIA_SWITCH_TRAP },
	{ "skill swap", SKILL_SWAP_TRAP },
	{ "skill upordown", SKILL_UPORDOWN_TRAP },
	{ "skill randomize", SKILL_RANDOMIZE_TRAP },

	{ "skill multiply", SKILL_MULTIPLY_TRAP },
	{ "trapwalk", TRAPWALK_TRAP },
	{ "cluster", CLUSTER_TRAP },
	{ "field", FIELD_TRAP },
	{ "monicide", MONICIDE_TRAP },
	{ "trap creation", TRAP_CREATION_TRAP },
	{ "leold", LEOLD_TRAP },
	{ "animeband", ANIMEBAND_TRAP },
	{ "perfume", PERFUME_TRAP },
	{ "court", COURT_TRAP },
	{ "elderscrolls", ELDER_SCROLLS_TRAP },
	{ "joke", JOKE_TRAP },
	{ "dungeon lords", DUNGEON_LORDS_TRAP },
	{ "fortytwo", FORTYTWO_TRAP },
	{ "randomize", RANDOMIZE_TRAP },
	{ "evilroom", EVILROOM_TRAP },
	{ "aoe", AOE_TRAP },
	{ "elona", ELONA_TRAP },
	{ "religion", RELIGION_TRAP },
	{ "steamband", STEAMBAND_TRAP },
	{ "hardcore", HARDCORE_TRAP },
	{ "machine", MACHINE_TRAP },
	{ "bee", BEE_TRAP },
	{ "migo", MIGO_TRAP },
	{ "angband", ANGBAND_TRAP },
	{ "dnethack", DNETHACK_TRAP },
	{ "evil spawn", EVIL_SPAWN_TRAP },
	{ "shoe", SHOE_TRAP },
	{ "inside", INSIDE_TRAP },
	{ "doom", DOOM_TRAP },
	{ "military", MILITARY_TRAP },
	{ "illusion", ILLUSION_TRAP },
	{ "diablo", DIABLO_TRAP },

	{ "giant explorer", GIANT_EXPLORER_TRAP },
	{ "trapwarp", TRAPWARP_TRAP },
	{ "yawm", YAWM_TRAP },
	{ "cradle of chaos", CRADLE_OF_CHAOS_TRAP },
	{ "tez", TEZCATLIPOCA_TRAP },
	{ "enthu", ENTHUMESIS_TRAP },
	{ "mikra", MIKRAANESIS_TRAP },
	{ "gotstogood", GOTS_TOO_GOOD_TRAP },
	{ "killer room", KILLER_ROOM_TRAP },
	{ "no fun walls", NO_FUN_WALLS_TRAP },

	{ "ruth", RUTH_TRAP },
	{ "magdalena", MAGDALENA_TRAP },
	{ "marleen", MARLEEN_TRAP },
	{ "klara", KLARA_TRAP },
	{ "friederike", FRIEDERIKE_TRAP },
	{ "naomi", NAOMI_TRAP },
	{ "ute", UTE_TRAP },
	{ "jasieen", JASIEEN_TRAP },
	{ "yasaman", YASAMAN_TRAP },
	{ "maybritt", MAY_BRITT_TRAP },

	{ "calling out", CALLING_OUT_TRAP },
	{ "field break", FIELD_BREAK_TRAP },
	{ "tenth", TENTH_TRAP },
	{ "debt", DEBT_TRAP },
	{ "inversion", INVERSION_TRAP },
	{ "wince", WINCE_TRAP },
	{ "u have been", U_HAVE_BEEN_TRAP },

	{ "really bad", REALLY_BAD_TRAP },
	{ "covid", COVID_TRAP },
	{ "artiblast", ARTIBLAST_TRAP },

	{ "persistent fart", PERSISTENT_FART_TRAP },
	{ "attacking heel", ATTACKING_HEEL_TRAP },
	{ "trap teleporter", TRAP_TELEPORTER },
	{ "alignment trash", ALIGNMENT_TRASH_TRAP },
	{ "reshuffle", RESHUFFLE_TRAP },
	{ "musehand", MUSEHAND_TRAP },
	{ "dogside", DOGSIDE_TRAP },
	{ "bankrupt", BANKRUPT_TRAP },
	{ "fillup", FILLUP_TRAP },
	{ "airstrike", AIRSTRIKE_TRAP },
	{ "dynamite", DYNAMITE_TRAP },
	{ "malevolence", MALEVOLENCE_TRAP },
	{ "leaflet", LEAFLET_TRAP },
	{ "tentadeep", TENTADEEP_TRAP },
	{ "stathalf", STATHALF_TRAP },
	{ "cutstat", CUTSTAT_TRAP },
	{ "rare spawn", RARE_SPAWN_TRAP },
	{ "you are an idiot", YOU_ARE_AN_IDIOT_TRAP },
	{ "nastycurse", NASTYCURSE_TRAP },
	{ "repeating nastycurse", REPEATING_NASTYCURSE_TRAP },

	{ "nadine", NADINE_TRAP },
	{ "luisa", LUISA_TRAP },
	{ "irina", IRINA_TRAP },
	{ "liselotte", LISELOTTE_TRAP },
	{ "greta", GRETA_TRAP },
	{ "jane", JANE_TRAP },
	{ "sue lyn", SUE_LYN_TRAP },
	{ "charlotte", CHARLOTTE_TRAP },
	{ "hannah", HANNAH_TRAP },
	{ "little marie", LITTLE_MARIE_TRAP },

	{ "real lie", REAL_LIE_TRAP },
	{ "escape past", ESCAPE_PAST_TRAP },
	{ "pethate", PETHATE_TRAP },
	{ "pet lashout", PET_LASHOUT_TRAP },
	{ "petstarve", PETSTARVE_TRAP },
	{ "petscrew", PETSCREW_TRAP },
	{ "tech loss", TECH_LOSS_TRAP },
	{ "proofloss", PROOFLOSS_TRAP },
	{ "un-invis", UN_INVIS_TRAP },
	{ "detectation", DETECTATION_TRAP },

	{ "option", OPTION_TRAP },
	{ "miscolor", MISCOLOR_TRAP },
	{ "one rainbow", ONE_RAINBOW_TRAP },
	{ "colorshift", COLORSHIFT_TRAP },
	{ "topline", TOP_LINE_TRAP },
	{ "caps", CAPS_TRAP },
	{ "un-knowledge", UN_KNOWLEDGE_TRAP },
	{ "darkhance", DARKHANCE_TRAP },
	{ "dschueueuet", DSCHUEUEUET_TRAP },
	{ "nopeskill", NOPESKILL_TRAP },
	{ "fuckfuckfuck", FUCKFUCKFUCK_TRAP },

	{ "burden", BURDEN_TRAP },
	{ "magic vacuum", MAGIC_VACUUM_TRAP },

	{ "sonja", SONJA_TRAP },
	{ "tanja", TANJA_TRAP },
	{ "lara", LARA_TRAP },
	{ "rhea", RHEA_TRAP },

	{ "fuck over", FUCK_OVER_TRAP },

	{ "daggertrap", DAGGER_TRAP },
	{ "razor", RAZOR_TRAP },
	{ "phosgene", PHOSGENE_TRAP },
	{ "chloroform", CHLOROFORM_TRAP },
	{ "corrosion", CORROSION_TRAP },
	{ "flame", FLAME_TRAP },
	{ "wither", WITHER_TRAP },
	{ "phaseporter", PHASEPORTER },
	{ "phasebeamer", PHASE_BEAMER },
	{ "vulnerate", VULNERATE_TRAP },

	{ "epvi", EPVI_TRAP },

	{ "timerun", TIMERUN_TRAP },
	{ 0, 0 }
};

static struct {
	const char *name;
	int type;
} room_types[] = {
	/* for historical reasons, room types are not contiguous numbers */
	/* (type 1 is skipped) */
	{ "ordinary",	 OROOM },
	{ "throne",	 COURT },
	{ "swamp",	 SWAMP },
	{ "vault",	 VAULT },
	{ "beehive",	 BEEHIVE },
	{ "morgue",	 MORGUE },
	{ "barracks",	 BARRACKS },
	{ "zoo",	 ZOO },
	{ "delphi",	 DELPHI },
	{ "temple",	 TEMPLE },
	{ "realzoo",	 REALZOO },
	{ "giantcourt",	 GIANTCOURT },
	{ "dragonlair",  DRAGONLAIR },
	{ "badfoodshop", BADFOODSHOP },
	{ "anthole",	 ANTHOLE },
	{ "cocknest",	 COCKNEST },
	{ "leprehall",	 LEPREHALL },
	{ "lemurepit",   LEMUREPIT },
	{ "migohive",    MIGOHIVE },
	{ "fungusfarm",  FUNGUSFARM },
	{ "clinic",  CLINIC },
	{ "terrorhall",  TERRORHALL },
	{ "elemhall",  ELEMHALL },
	{ "angelhall",  ANGELHALL },
	{ "mimichall",  MIMICHALL },
	{ "nymphhall",  NYMPHHALL },
	{ "spiderhall",  SPIDERHALL },
	{ "trollhall",  TROLLHALL },
	{ "humanhall",  HUMANHALL },
	{ "golemhall",  GOLEMHALL },
	{ "coinhall",  COINHALL },
	{ "dougroom",  DOUGROOM },
	{ "armory",  ARMORY },
	{ "tenshall",  TENSHALL },
	{ "traproom",  TRAPROOM },
	{ "poolroom",  POOLROOM },
	{ "grueroom",  GRUEROOM },
	{ "statueroom",  STATUEROOM },
	{ "insideroom",  INSIDEROOM },
	{ "riverroom",  RIVERROOM },
	{ "emptynest",  EMPTYNEST },
	{ "cryptroom",  CRYPTROOM },
	{ "troublezone",  TROUBLEZONE },
	{ "weaponchamber",  WEAPONCHAMBER },
	{ "hellpit",  HELLPIT },
	{ "feminismroom",  FEMINISMROOM },
	{ "meadowroom",  MEADOWROOM },
	{ "coolingchamber",  COOLINGCHAMBER },
	{ "voidroom",  VOIDROOM },
	{ "hamletroom",  HAMLETROOM },
	{ "kopstation",  KOPSTATION },
	{ "bossroom",  BOSSROOM },
	{ "rngcenter",  RNGCENTER },
	{ "wizardsdorm",  WIZARDSDORM },
	{ "doomedbarracks",  DOOMEDBARRACKS },
	{ "sleepingroom",  SLEEPINGROOM },
	{ "diverparadise",  DIVERPARADISE },
	{ "menagerie",  MENAGERIE },
	{ "nastycentral",  NASTYCENTRAL },
	{ "emptydesert",  EMPTYDESERT },
	{ "rarityroom",  RARITYROOM },
	{ "exhibitroom",  EXHIBITROOM },
	{ "prisonchamber",  PRISONCHAMBER },
	{ "nuclearchamber",  NUCLEARCHAMBER },
	{ "levelseventyroom",  LEVELSEVENTYROOM },
	{ "robbercave",  ROBBERCAVE },
	{ "sanitationcentral",  SANITATIONCENTRAL },
	{ "casinoroom",  CASINOROOM },

	{ "evilroom", EVILROOM },
	{ "religioncenter", RELIGIONCENTER },
	{ "chaosroom", CHAOSROOM },
	{ "cursedmummyroom", CURSEDMUMMYROOM },
	{ "mixedpool", MIXEDPOOL },
	{ "arduousmountain", ARDUOUSMOUNTAIN },
	{ "levelffroom", LEVELFFROOM },
	{ "verminroom", VERMINROOM },
	{ "miraspa", MIRASPA },
	{ "machineroom", MACHINEROOM },
	{ "showerroom", SHOWERROOM },
	{ "greencrossroom", GREENCROSSROOM },
	{ "centraltedium", CENTRALTEDIUM },
	{ "ruinedchurch", RUINEDCHURCH },
	{ "rampageroom", RAMPAGEROOM },
	{ "gamecorner", GAMECORNER },
	{ "illusionroom", ILLUSIONROOM },

	{ "lettersaladroom", LETTERSALADROOM },
	{ "thearearoom", THE_AREA_ROOM },
	{ "changingroom", CHANGINGROOM },
	{ "questorroom", QUESTORROOM },

	{ "variantroom",  VARIANTROOM },
	{ "playercentral", PLAYERCENTRAL },
	{ "fullroom", FULLROOM },
	{ "randomroom",  RANDOMROOM },
	{ "shop",	 SHOPBASE },
	{ "armor shop",	 ARMORSHOP },
	{ "scroll shop", SCROLLSHOP },
	{ "potion shop", POTIONSHOP },
	{ "weapon shop", WEAPONSHOP },
	{ "food shop",	 FOODSHOP },
	{ "ring shop",	 RINGSHOP },
	{ "wand shop",	 WANDSHOP },
	{ "tool shop",	 TOOLSHOP },
	{ "pet shop",    PETSHOP },	/* Stephen White */
	{ "tin shop",    TINSHOP }, /* Robin Johnson */
	{ "book shop",	 BOOKSHOP },
	{ "fresh food shop",	 USELESSSHOP },
	{ "gun shop",	 GUNSHOP },
	{ "banging shop",	 BANGSHOP },
	{ "ammo shop",	 AMMOSHOP },
	{ "lightning shop",	 LITENSHOP },
	{ "boulder shop",	 BOULDSHOP },
	{ "rock shop",	 ROCKSSHOP },
	{ "wierd shop",	 WIERDSHOP },
	{ "zorkmid shop",	 ZORKSHOP },
	{ "random shop",	 RANDOMSHOP },
	{ "candle shop", CANDLESHOP },
	{ "black market", BLACKSHOP },
	{ 0, 0 }
};

const char *fname = "(stdin)";
int fatal_error = 0;
int want_warnings = 0;

#ifdef FLEX23_BUG
/* Flex 2.3 bug work around; not needed for 2.3.6 or later */
int yy_more_len = 0;
#endif

extern char tmpmessage[];
extern altar *tmpaltar[];
extern lad *tmplad[];
extern stair *tmpstair[];
extern digpos *tmpdig[];
extern digpos *tmppass[];
extern char *tmpmap[];
extern region *tmpreg[];
extern lev_region *tmplreg[];
extern door *tmpdoor[];
extern room_door *tmprdoor[];
extern trap *tmptrap[];
extern monster *tmpmonst[];
extern object *tmpobj[];
extern drawbridge *tmpdb[];
extern walk *tmpwalk[];
extern gold *tmpgold[];
extern fountain *tmpfountain[];
extern sink *tmpsink[];
extern pool *tmppool[];
extern engraving *tmpengraving[];
extern mazepart *tmppart[];
extern room *tmproom[];
extern lev_region *tmprndlreg[];

extern int n_olist, n_mlist, n_plist;

extern unsigned int nrndlreg;
extern unsigned int nlreg, nreg, ndoor, ntrap, nmons, nobj;
extern unsigned int ndb, nwalk, npart, ndig, npass, nlad, nstair;
extern unsigned int naltar, ncorridor, nrooms, ngold, nengraving;
extern unsigned int nfountain, npool, nsink;

extern unsigned int max_x_map, max_y_map;

extern int line_number, colon_line_number;

int
main(argc, argv)
int argc;
char **argv;
{
	FILE *fin;
	int i;
	boolean errors_encountered = FALSE;
#if defined(MAC) && (defined(THINK_C) || defined(__MWERKS__))
	static char *mac_argv[] = {	"lev_comp",	/* dummy argv[0] */
	/* KMH -- had to add more from SLASH'EM */
				":dat:Arch.des",
				":dat:Barb.des",
				":dat:Caveman.des",
				":dat:Flame.des",
				":dat:Healer.des",
				":dat:Hobbit.des",
				":dat:Ice.des",
				":dat:sokoban.des",
				":dat:Knight.des",
				":dat:Lycn.des",
				":dat:Monk.des",
				":dat:Necro.des",
				":dat:Priest.des",
				":dat:Rogue.des",
				":dat:Ranger.des",
				":dat:Samurai.des",
				":dat:Tourist.des",
				":dat:Slayer.des",		/* Should be Uslayer */
				":dat:Valkyrie.des",
				":dat:Wizard.des",
				":dat:Yeoman.des",
#ifdef ZOUTHERN
				":dat:Zouthern.des"
#endif
				":dat:beholder.des",
				":dat:bigroom.des",
				":dat:blkmar.des",
				":dat:castle.des",
				":dat:caves.des",
				":dat:dragons.des",
				":dat:endgame.des",
				":dat:exmall.des",
				":dat:frnknstn.des",
				":dat:gehennom.des",
				":dat:giants.des",
				":dat:guild.des",
				":dat:knox.des",
				":dat:kobold-1.des",
				":dat:kobold-2.des",
				":dat:lich.des",
				":dat:mall-1.des",
				":dat:mall-2.des",
				":dat:mall-3.des",
				":dat:medusa.des",
				":dat:mtemple.des",
				":dat:mines.des",
				":dat:newmall.des",
				":dat:nightmar.des",
				":dat:nymph.des",
				":dat:oracle.des",
				":dat:rats.des",
				":dat:sea.des",
				":dat:sokoban.des",
				":dat:spiders.des",
				":dat:stor-1.des",
				":dat:stor-2.des",
				":dat:stor-3.des",
				":dat:tomb.des",
				":dat:tower.des",
				":dat:yendor.des"
				};

	argc = SIZE(mac_argv);
	argv = mac_argv;
#endif
	/* Note:  these initializers don't do anything except guarantee that
		we're linked properly.
	*/
	monst_globals_init();
	objects_init();
	decl_init();
	/* this one does something... */
	init_obj_classes();

	init_yyout(stdout);
	if (argc == 1) {		/* Read standard input */
	    init_yyin(stdin);
	    (void) yyparse();
	    if (fatal_error > 0) {
		    errors_encountered = TRUE;
	    }
	} else {			/* Otherwise every argument is a filename */
	    for(i=1; i<argc; i++) {
		    fname = argv[i];
		    if(!strcmp(fname, "-w")) {
			want_warnings++;
			continue;
		    }
		    fin = freopen(fname, "r", stdin);
		    if (!fin) {
			(void) fprintf(stderr,"Can't open \"%s\" for input.\n",
						fname);
			perror(fname);
			errors_encountered = TRUE;
		    } else {
			init_yyin(fin);
			(void) yyparse();
			line_number = 1;
			if (fatal_error > 0) {
				errors_encountered = TRUE;
				fatal_error = 0;
			}
		    }
	    }
	}
	exit(errors_encountered ? EXIT_FAILURE : EXIT_SUCCESS);
	/*NOTREACHED*/
	return 0;
}

/*
 * Each time the parser detects an error, it uses this function.
 * Here we take count of the errors. To continue farther than
 * MAX_ERRORS wouldn't be reasonable.
 * Assume that explicit calls from lev_comp.y have the 1st letter
 * capitalized, to allow printing of the line containing the start of
 * the current declaration, instead of the beginning of the next declaration.
 */
void
yyerror(s)
const char *s;
{
	(void) fprintf(stderr,
#ifndef MAC_MPW
	  "%s: line %d : %s\n",
#else
	  "File %s ; Line %d :# %s\n",
#endif
	  fname, (*s >= 'A' && *s <= 'Z') ? colon_line_number : line_number, s);
	if (++fatal_error > MAX_ERRORS) {
		(void) fprintf(stderr,"Too many errors, good bye!\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * Just display a warning (that is : a non fatal error)
 */
void
yywarning(s)
const char *s;
{
	(void) fprintf(stderr,
#ifndef MAC_MPW
	  "%s: line %d : WARNING : %s\n",
#else
	  "File %s ; Line %d # WARNING : %s\n",
#endif
				fname, colon_line_number, s);
}

/*
 * Stub needed for lex interface.
 */
int
yywrap()
{
	return 1;
}

/*
 * Find the index of an artifact in the table, knowing its name.
 */
int
get_artifact_id(s)
char *s;
{
	register int i;

	SpinCursor(3);
	for(i=0; artifact_names[i]; i++)
	    if (!strcmp(s, artifact_names[i]))
		return ((int) i);
	return ERR;
}

/*
 * Find the type of floor, knowing its char representation.
 */
int
get_floor_type(c)
char c;
{
	int val;

	SpinCursor(3);
	val = what_map_char(c);
	if(val == INVALID_TYPE) {
	    val = ERR;
	    yywarning("Invalid fill character in MAZE declaration");
	}
	return val;
}

/*
 * Find the type of a room in the table, knowing its name.
 */
int
get_room_type(s)
char *s;
{
	register int i;

	SpinCursor(3);
	for(i=0; room_types[i].name; i++)
	    if (!strcmp(s, room_types[i].name))
		return ((int) room_types[i].type);
	return ERR;
}

/*
 * Find the type of a trap in the table, knowing its name.
 */
int
get_trap_type(s)
char *s;
{
	register int i;

	SpinCursor(3);
	for (i=0; trap_types[i].name; i++)
	    if(!strcmp(s,trap_types[i].name))
		return trap_types[i].type;
	return ERR;
}

/*
 * Find the index of a monster in the table, knowing its name.
 */
int
get_monster_id(s, c)
char *s;
char c;
{
	register int i, class;

	SpinCursor(3);
	class = c ? def_char_to_monclass(c) : 0;
	if (class == MAXMCLASSES) return ERR;

	for (i = LOW_PM; i < NUMMONS; i++)
	    if (!class || class == mons[i].mlet)
		if (!strcmp(s, mons[i].mname)) return i;
	return ERR;
}

/*
 * Find the index of an object in the table, knowing its name.
 */
int
get_object_id(s, c)
char *s;
char c;		/* class */
{
	int i, class;
	const char *objname;

	SpinCursor(3);
	class = (c > 0) ? def_char_to_objclass(c) : 0;
	if (class == MAXOCLASSES) return ERR;

	for (i = class ? bases[class] : 0; i < NUM_OBJECTS; i++) {
	    if (class && objects[i].oc_class != class) break;
	    objname = obj_descr[i].oc_name;
	    if (objname && !strcmp(s, objname))
		return i;
	}
	return ERR;
}

static void
init_obj_classes()
{
	int i, class, prev_class;

	prev_class = -1;
	for (i = 0; i < NUM_OBJECTS; i++) {
	    class = objects[i].oc_class;
	    if (class != prev_class) {
		bases[class] = i;
		prev_class = class;
	    }
	}
}

/*
 * Is the character 'c' a valid monster class ?
 */
boolean
check_monster_char(c)
char c;
{
	return (def_char_to_monclass(c) != MAXMCLASSES);
}

/*
 * Is the character 'c' a valid object class ?
 */
boolean
check_object_char(c)
char c;
{
	return (def_char_to_objclass(c) != MAXOCLASSES);
}

/*
 * Convert .des map letter into floor type.
 */
char
what_map_char(c)
char c;
{
	SpinCursor(3);
	switch(c) {
		  case ' '  : return(STONE);
		  case '#'  : return(CORR);
		  case '.'  : return(ROOM);
		  case '-'  : return(HWALL);
		  case '|'  : return(VWALL);
		  case '+'  : return(DOOR);
		  case 'A'  : return(AIR);
		  case 'B'  : return(CROSSWALL); /* hack: boundary location */
		  case 'C'  : return(CLOUD);
		  case 'S'  : return(SDOOR);
		  case 'H'  : return(SCORR);
		  case '{'  : return(FOUNTAIN);
		  case '\\' : return(THRONE);
		  case 'K'  : return(SINK);
		  case '}'  : return(MOAT);
		  case 'P'  : return(POOL);
		  case 'L'  : return(LAVAPOOL);
		  case 'I'  : return(ICE);
		  case 'W'  : return(WATER);
		  case 'T'	: return (TREE);
		  case 'F'	: return (IRONBARS);	/* Fe = iron */
		  case 'Z'  : return(TOILET);	/* Kazi */
		  case 'G'	: return (GRAVEWALL);
		  case 'U'	: return (TUNNELWALL);
		  case 'J'	: return (FARMLAND);
		  case 'M'	: return (MOUNTAIN);
		  case 'X'	: return (WATERTUNNEL);
		  case 'Y'	: return (CRYSTALWATER);
		  case 'O'	: return (MOORLAND);
		  case 'R'	: return (URINELAKE);
		  case 's'	: return (SHIFTINGSAND);
		  case 'y'	: return (STYXRIVER);
		  case 'p'	: return (PENTAGRAM);
		  case 'w'	: return (WELL);
		  case 'o'	: return (POISONEDWELL);
		  case 'a'	: return (WAGON);
		  case 'b'	: return (BURNINGWAGON);
		  case 't'	: return (WOODENTABLE);
		  case 'c'	: return (CARVEDBED);
		  case 'm'	: return (STRAWMATTRESS);
		  case 'n'	: return (SNOW);
		  case 'h'	: return (ASH);
		  case 'd'	: return (SAND);
		  case 'v'	: return (PAVEDFLOOR);
		  case 'i'	: return (HIGHWAY);
		  case 'g'	: return (GRASSLAND);
		  case 'r'	: return (NETHERMIST);
		  case 'l'	: return (STALACTITE);
		  case 'f'	: return (CRYPTFLOOR);
		  case 'u'	: return (BUBBLES);
		  case 'q'	: return (RAINCLOUD);
		  case 'e'	: return (ROCKWALL);
	    }
	return(INVALID_TYPE);
}

/*
 * Yep! LEX gives us the map in a raw mode.
 * Just analyze it here.
 */
void
scan_map(map)
char *map;
{
	register int i, len;
	register char *s1, *s2;
	int max_len = 0;
	int max_hig = 0;
	char msg[256];

	/* First, strip out digits 0-9 (line numbering) */
	for (s1 = s2 = map; *s1; s1++)
	    if (*s1 < '0' || *s1 > '9')
		*s2++ = *s1;
	*s2 = '\0';

	/* Second, find the max width of the map */
	s1 = map;
	while (s1 && *s1) {
		s2 = index(s1, '\n');
		if (s2) {
			len = (int) (s2 - s1);
			s1 = s2 + 1;
		} else {
			len = (int) strlen(s1);
			s1 = (char *) 0;
		}
		if (len > max_len) max_len = len;
	}

	/* Then parse it now */
	while (map && *map) {
		tmpmap[max_hig] = (char *) alloc(max_len);
		s1 = index(map, '\n');
		if (s1) {
			len = (int) (s1 - map);
			s1++;
		} else {
			len = (int) strlen(map);
			s1 = map + len;
		}
		for(i=0; i<len; i++)
		  if((tmpmap[max_hig][i] = what_map_char(map[i])) == INVALID_TYPE) {
		      sprintf(msg,
			 "Invalid character @ (%d, %d) - replacing with stone",
			      max_hig, i);
		      yywarning(msg);
		      tmpmap[max_hig][i] = STONE;
		    }
		while(i < max_len)
		    tmpmap[max_hig][i++] = STONE;
		map = s1;
		max_hig++;
	}

	/* Memorize boundaries */

	max_x_map = max_len - 1;
	max_y_map = max_hig - 1;

	/* Store the map into the mazepart structure */

	if(max_len > MAP_X_LIM || max_hig > MAP_Y_LIM) {
	    sprintf(msg, "Map too large! (max %d x %d, is %d x %d)", MAP_X_LIM, MAP_Y_LIM, max_len, max_hig);
	    yyerror(msg);
	}

	tmppart[npart]->xsize = max_len;
	tmppart[npart]->ysize = max_hig;
	tmppart[npart]->map = (char **) alloc(max_hig*sizeof(char *));
	for(i = 0; i< max_hig; i++)
	    tmppart[npart]->map[i] = tmpmap[i];
}

/*
 *	If we have drawn a map without walls, this allows us to
 *	auto-magically wallify it.
 */
#define Map_point(x,y) *(tmppart[npart]->map[y] + x)

void
wallify_map()
{
	unsigned int x, y, xx, yy, lo_xx, lo_yy, hi_xx, hi_yy;

	for (y = 0; y <= max_y_map; y++) {
	    SpinCursor(3);
	    lo_yy = (y > 0) ? y - 1 : 0;
	    hi_yy = (y < max_y_map) ? y + 1 : max_y_map;
	    for (x = 0; x <= max_x_map; x++) {
		if (Map_point(x,y) != STONE) continue;
		lo_xx = (x > 0) ? x - 1 : 0;
		hi_xx = (x < max_x_map) ? x + 1 : max_x_map;
		for (yy = lo_yy; yy <= hi_yy; yy++)
		    for (xx = lo_xx; xx <= hi_xx; xx++)
			if (IS_ROOM(Map_point(xx,yy)) ||
				Map_point(xx,yy) == CROSSWALL) {
			    Map_point(x,y) = (yy != y) ? HWALL : VWALL;
			    yy = hi_yy;		/* end `yy' loop */
			    break;		/* end `xx' loop */
			}
	    }
	}
}

/*
 * We need to check the subrooms apartenance to an existing room.
 */
boolean
check_subrooms()
{
	unsigned i, j, n_subrooms;
	boolean	found, ok = TRUE;
	char	*last_parent, msg[256];

	for (i = 0; i < nrooms; i++)
	    if (tmproom[i]->parent) {
		found = FALSE;
		for(j = 0; j < nrooms; j++)
		    if (tmproom[j]->name &&
			    !strcmp(tmproom[i]->parent, tmproom[j]->name)) {
			found = TRUE;
			break;
		    }
		if (!found) {
		    sprintf(msg,
			    "Subroom error : parent room '%s' not found!",
			    tmproom[i]->parent);
		    yyerror(msg);
		    ok = FALSE;
		}
	    }

	msg[0] = '\0';
	last_parent = msg;
	for (i = 0; i < nrooms; i++)
	    if (tmproom[i]->parent) {
		n_subrooms = 0;
		for(j = i; j < nrooms; j++) {
/*
 *	This is by no means perfect, but should cut down the duplicate error
 *	messages by over 90%.  The only problem will be when either subrooms
 *	are mixed in the level definition (not likely but possible) or rooms
 *	have subrooms that have subrooms.
 */
		    if (!strcmp(tmproom[i]->parent, last_parent)) continue;
		    if (tmproom[j]->parent &&
			    !strcmp(tmproom[i]->parent, tmproom[j]->parent)) {
			n_subrooms++;
			if(n_subrooms > MAX_SUBROOMS) {

			    sprintf(msg,
	      "Subroom error: too many subrooms attached to parent room '%s'!",
				    tmproom[i]->parent);
			    yyerror(msg);
			    last_parent = tmproom[i]->parent;
			    ok = FALSE;
			    break;
			}
		    }
		}
	    }
	return ok;
}

/*
 * Check that coordinates (x,y) are roomlike locations.
 * Print warning "str" if they aren't.
 */
void
check_coord(x, y, str)
int x, y;
const char *str;
{
    char ebuf[60];

	/* let's silence this warning - this is slex, things in walls are not errors */
	/*
    if (x >= 0 && y >= 0 && x <= (int)max_x_map && y <= (int)max_y_map &&
	(IS_ROCK(tmpmap[y][x]) || IS_DOOR(tmpmap[y][x]))) {
	sprintf(ebuf, "%s placed in wall at (%02d,%02d)?!", str, x, y);
	yywarning(ebuf);
    }
	*/

}

/*
 * Here we want to store the maze part we just got.
 */
void
store_part()
{
	register unsigned i;

	/* Ok, We got the whole part, now we store it. */

	/* The Regions */

	if ((tmppart[npart]->nreg = nreg) != 0) {
		tmppart[npart]->regions = NewTab(region, nreg);
		for(i=0;i<nreg;i++)
		    tmppart[npart]->regions[i] = tmpreg[i];
	}
	nreg = 0;

	/* The Level Regions */

	if ((tmppart[npart]->nlreg = nlreg) != 0) {
		tmppart[npart]->lregions = NewTab(lev_region, nlreg);
		for(i=0;i<nlreg;i++)
		    tmppart[npart]->lregions[i] = tmplreg[i];
	}
	nlreg = 0;

	/* the random level regions */

	if ((tmppart[npart]->nrndlreg = nrndlreg) != 0) {
		tmppart[npart]->rndlregions = NewTab(lev_region, nrndlreg);
		for(i=0;i<nrndlreg;i++)
		    tmppart[npart]->rndlregions[i] = tmprndlreg[i];
	}
	nrndlreg = 0;

	/* the doors */

	if ((tmppart[npart]->ndoor = ndoor) != 0) {
		tmppart[npart]->doors = NewTab(door, ndoor);
		for(i=0;i<ndoor;i++)
		    tmppart[npart]->doors[i] = tmpdoor[i];
	}
	ndoor = 0;

	/* the drawbridges */

	if ((tmppart[npart]->ndrawbridge = ndb) != 0) {
		tmppart[npart]->drawbridges = NewTab(drawbridge, ndb);
		for(i=0;i<ndb;i++)
		    tmppart[npart]->drawbridges[i] = tmpdb[i];
	}
	ndb = 0;

	/* The walkmaze directives */

	if ((tmppart[npart]->nwalk = nwalk) != 0) {
		tmppart[npart]->walks = NewTab(walk, nwalk);
		for(i=0;i<nwalk;i++)
		    tmppart[npart]->walks[i] = tmpwalk[i];
	}
	nwalk = 0;

	/* The non_diggable directives */

	if ((tmppart[npart]->ndig = ndig) != 0) {
		tmppart[npart]->digs = NewTab(digpos, ndig);
		for(i=0;i<ndig;i++)
		    tmppart[npart]->digs[i] = tmpdig[i];
	}
	ndig = 0;

	/* The non_passwall directives */

	if ((tmppart[npart]->npass = npass) != 0) {
		tmppart[npart]->passs = NewTab(digpos, npass);
		for(i=0;i<npass;i++)
		    tmppart[npart]->passs[i] = tmppass[i];
	}
	npass = 0;

	/* The ladders */

	if ((tmppart[npart]->nlad = nlad) != 0) {
		tmppart[npart]->lads = NewTab(lad, nlad);
		for(i=0;i<nlad;i++)
		    tmppart[npart]->lads[i] = tmplad[i];
	}
	nlad = 0;

	/* The stairs */

	if ((tmppart[npart]->nstair = nstair) != 0) {
		tmppart[npart]->stairs = NewTab(stair, nstair);
		for(i=0;i<nstair;i++)
		    tmppart[npart]->stairs[i] = tmpstair[i];
	}
	nstair = 0;

	/* The altars */
	if ((tmppart[npart]->naltar = naltar) != 0) {
		tmppart[npart]->altars = NewTab(altar, naltar);
		for(i=0;i<naltar;i++)
		    tmppart[npart]->altars[i] = tmpaltar[i];
	}
	naltar = 0;

	/* The fountains */

	if ((tmppart[npart]->nfountain = nfountain) != 0) {
		tmppart[npart]->fountains = NewTab(fountain, nfountain);
		for(i=0;i<nfountain;i++)
		    tmppart[npart]->fountains[i] = tmpfountain[i];
	}
	nfountain = 0;

	/* the traps */

	if ((tmppart[npart]->ntrap = ntrap) != 0) {
		tmppart[npart]->traps = NewTab(trap, ntrap);
		for(i=0;i<ntrap;i++)
		    tmppart[npart]->traps[i] = tmptrap[i];
	}
	ntrap = 0;

	/* the monsters */

	if ((tmppart[npart]->nmonster = nmons) != 0) {
		tmppart[npart]->monsters = NewTab(monster, nmons);
		for(i=0;i<nmons;i++)
		    tmppart[npart]->monsters[i] = tmpmonst[i];
	} else
		tmppart[npart]->monsters = 0;
	nmons = 0;

	/* the objects */

	if ((tmppart[npart]->nobject = nobj) != 0) {
		tmppart[npart]->objects = NewTab(object, nobj);
		for(i=0;i<nobj;i++)
		    tmppart[npart]->objects[i] = tmpobj[i];
	} else
		tmppart[npart]->objects = 0;
	nobj = 0;

	/* The gold piles */

	if ((tmppart[npart]->ngold = ngold) != 0) {
		tmppart[npart]->golds = NewTab(gold, ngold);
		for(i=0;i<ngold;i++)
		    tmppart[npart]->golds[i] = tmpgold[i];
	}
	ngold = 0;

	/* The engravings */

	if ((tmppart[npart]->nengraving = nengraving) != 0) {
		tmppart[npart]->engravings = NewTab(engraving, nengraving);
		for(i=0;i<nengraving;i++)
		    tmppart[npart]->engravings[i] = tmpengraving[i];
	} else
		tmppart[npart]->engravings = 0;
	nengraving = 0;

	npart++;
	n_plist = n_mlist = n_olist = 0;
}

/*
 * Here we want to store the room part we just got.
 */
void
store_room()
{
	register unsigned i;

	/* Ok, We got the whole room, now we store it. */

	/* the doors */

	if ((tmproom[nrooms]->ndoor = ndoor) != 0) {
		tmproom[nrooms]->doors = NewTab(room_door, ndoor);
		for(i=0;i<ndoor;i++)
		    tmproom[nrooms]->doors[i] = tmprdoor[i];
	}
	ndoor = 0;

	/* The stairs */

	if ((tmproom[nrooms]->nstair = nstair) != 0) {
		tmproom[nrooms]->stairs = NewTab(stair, nstair);
		for(i=0;i<nstair;i++)
		    tmproom[nrooms]->stairs[i] = tmpstair[i];
	}
	nstair = 0;

	/* The altars */
	if ((tmproom[nrooms]->naltar = naltar) != 0) {
		tmproom[nrooms]->altars = NewTab(altar, naltar);
		for(i=0;i<naltar;i++)
		    tmproom[nrooms]->altars[i] = tmpaltar[i];
	}
	naltar = 0;

	/* The fountains */

	if ((tmproom[nrooms]->nfountain = nfountain) != 0) {
		tmproom[nrooms]->fountains = NewTab(fountain, nfountain);
		for(i=0;i<nfountain;i++)
		    tmproom[nrooms]->fountains[i] = tmpfountain[i];
	}
	nfountain = 0;

	/* The sinks */

	if ((tmproom[nrooms]->nsink = nsink) != 0) {
		tmproom[nrooms]->sinks = NewTab(sink, nsink);
		for(i=0;i<nsink;i++)
		    tmproom[nrooms]->sinks[i] = tmpsink[i];
	}
	nsink = 0;

	/* The pools */

	if ((tmproom[nrooms]->npool = npool) != 0) {
		tmproom[nrooms]->pools = NewTab(pool, npool);
		for(i=0;i<npool;i++)
		    tmproom[nrooms]->pools[i] = tmppool[i];
	}
	npool = 0;

	/* the traps */

	if ((tmproom[nrooms]->ntrap = ntrap) != 0) {
		tmproom[nrooms]->traps = NewTab(trap, ntrap);
		for(i=0;i<ntrap;i++)
		    tmproom[nrooms]->traps[i] = tmptrap[i];
	}
	ntrap = 0;

	/* the monsters */

	if ((tmproom[nrooms]->nmonster = nmons) != 0) {
		tmproom[nrooms]->monsters = NewTab(monster, nmons);
		for(i=0;i<nmons;i++)
		    tmproom[nrooms]->monsters[i] = tmpmonst[i];
	} else
		tmproom[nrooms]->monsters = 0;
	nmons = 0;

	/* the objects */

	if ((tmproom[nrooms]->nobject = nobj) != 0) {
		tmproom[nrooms]->objects = NewTab(object, nobj);
		for(i=0;i<nobj;i++)
		    tmproom[nrooms]->objects[i] = tmpobj[i];
	} else
		tmproom[nrooms]->objects = 0;
	nobj = 0;

	/* The gold piles */

	if ((tmproom[nrooms]->ngold = ngold) != 0) {
		tmproom[nrooms]->golds = NewTab(gold, ngold);
		for(i=0;i<ngold;i++)
		    tmproom[nrooms]->golds[i] = tmpgold[i];
	}
	ngold = 0;

	/* The engravings */

	if ((tmproom[nrooms]->nengraving = nengraving) != 0) {
		tmproom[nrooms]->engravings = NewTab(engraving, nengraving);
		for(i=0;i<nengraving;i++)
		    tmproom[nrooms]->engravings[i] = tmpengraving[i];
	} else
		tmproom[nrooms]->engravings = 0;
	nengraving = 0;

	nrooms++;
}

void
store_place_list(int npart, int nlist, int nloc, const struct coord *plist)
{
	int i;
	char msg[256];
	if (!tmppart[npart]->nloc) {
	    tmppart[npart]->nloc = (char *) alloc(MAX_REGISTERS);
	    tmppart[npart]->rloc_x = NewTab(char, MAX_REGISTERS);
	    tmppart[npart]->rloc_y = NewTab(char, MAX_REGISTERS);
	}
	if (nlist < tmppart[npart]->nlocset) {
	    sprintf(msg,
		    "Location registers for place list %d already initialized!",
		    nlist);
	    yyerror(msg);
	} else if (nlist > tmppart[npart]->nlocset) {
	    sprintf(msg, "Place list %d out of order!", nlist);
	    yyerror(msg);
	} else {
	    tmppart[npart]->nlocset++;
	    tmppart[npart]->rloc_x[nlist] = (char *) alloc(n_plist);
	    tmppart[npart]->rloc_y[nlist] = (char *) alloc(n_plist);
	    for(i = 0; i < n_plist; i++) {
		tmppart[npart]->rloc_x[nlist][i] = plist[i].x;
		tmppart[npart]->rloc_y[nlist][i] = plist[i].y;
	    }
	    tmppart[npart]->nloc[nlist] = nloc;
	}
}

/*
 * Output some info common to all special levels.
 */
static boolean
write_common_data(fd, typ, init, flgs)
int fd, typ;
lev_init *init;
long flgs;
{
	char c;
	uchar len;
	static struct version_info version_data = {
			VERSION_NUMBER, VERSION_FEATURES,
			VERSION_SANITY1, VERSION_SANITY2
	};

	Write(fd, &version_data, sizeof version_data);
	c = typ;
	Write(fd, &c, sizeof(c));	/* 1 byte header */
	Write(fd, init, sizeof(lev_init));
	Write(fd, &flgs, sizeof flgs);

	len = (uchar) strlen(tmpmessage);
	Write(fd, &len, sizeof len);
	if (len) Write(fd, tmpmessage, (int) len);
	tmpmessage[0] = '\0';
	return TRUE;
}

/*
 * Output monster info, which needs string fixups, then release memory.
 */
static boolean
write_monsters(fd, nmonster_p, monsters_p)
int fd;
char *nmonster_p;
monster ***monsters_p;
{
	monster *m;
	char *name, *appr;
	int j, n = (int)*nmonster_p;

	Write(fd, nmonster_p, sizeof *nmonster_p);
	for (j = 0; j < n; j++) {
	    m = (*monsters_p)[j];
	    name = m->name.str;
	    appr = m->appear_as.str;
	    m->name.str = m->appear_as.str = 0;
	    m->name.len = name ? strlen(name) : 0;
	    m->appear_as.len = appr ? strlen(appr) : 0;
	    Write(fd, m, sizeof *m);
	    if (name) {
		Write(fd, name, m->name.len);
		free(name);
	    }
	    if (appr) {
		Write(fd, appr, m->appear_as.len);
		free(appr);
	    }
	    free(m);
	}
	if (*monsters_p) {
	    free(*monsters_p);
	    *monsters_p = 0;
	}
	*nmonster_p = 0;
	return TRUE;
}

/*
 * Output object info, which needs string fixup, then release memory.
 */
static boolean
write_objects(fd, nobject_p, objects_p)
int fd;
char *nobject_p;
object ***objects_p;
{
	object *o;
	char *name;
	int j, n = (int)*nobject_p;

	Write(fd, nobject_p, sizeof *nobject_p);
	for (j = 0; j < n; j++) {
	    o = (*objects_p)[j];
	    name = o->name.str;
	    o->name.str = 0;	/* reset in case `len' is narrower */
	    o->name.len = name ? strlen(name) : 0;
	    Write(fd, o, sizeof *o);
	    if (name) {
		Write(fd, name, o->name.len);
		free(name);
	    }
	    free(o);
	}
	if (*objects_p) {
	    free(*objects_p);
	    *objects_p = 0;
	}
	*nobject_p = 0;
	return TRUE;
}

/*
 * Output engraving info, which needs string fixup, then release memory.
 */
static boolean
write_engravings(fd, nengraving_p, engravings_p)
int fd;
char *nengraving_p;
engraving ***engravings_p;
{
	engraving *e;
	char *engr;
	int j, n = (int)*nengraving_p;

	Write(fd, nengraving_p, sizeof *nengraving_p);
	for (j = 0; j < n; j++) {
	    e = (*engravings_p)[j];
	    engr = e->engr.str;
	    e->engr.str = 0;	/* reset in case `len' is narrower */
	    e->engr.len = strlen(engr);
	    Write(fd, e, sizeof *e);
	    Write(fd, engr, e->engr.len);
	    free(engr);
	    free(e);
	}
	if (*engravings_p) {
	    free(*engravings_p);
	    *engravings_p = 0;
	}
	*nengraving_p = 0;
	return TRUE;
}

/*
 * Open and write maze or rooms file, based on which pointer is non-null.
 * Return TRUE on success, FALSE on failure.
 */
boolean
write_level_file(filename, room_level, maze_level)
char *filename;
splev *room_level;
specialmaze *maze_level;
{
	int fout;
	char lbuf[60];

	lbuf[0] = '\0';
#ifdef PREFIX
	strcat(lbuf, PREFIX);
#endif
	strcat(lbuf, filename);
	strcat(lbuf, LEV_EXT);

	fout = open(lbuf, O_WRONLY|O_CREAT|O_BINARY, OMASK);
	if (fout < 0) return FALSE;

	if (room_level) {
	    if (!write_rooms(fout, room_level))
		return FALSE;
	} else if (maze_level) {
	    if (!write_maze(fout, maze_level))
		return FALSE;
	} else
	    panic("write_level_file");

	(void) close(fout);
	return TRUE;
}

/*
 * Here we write the structure of the maze in the specified file (fd).
 * Also, we have to free the memory allocated via alloc().
 */
static boolean
write_maze(fd, maze)
int fd;
specialmaze *maze;
{
	short i,j;
	mazepart *pt;

	if (!write_common_data(fd, SP_LEV_MAZE, &(maze->init_lev), maze->flags))
	    return FALSE;

	Write(fd, &(maze->filling), sizeof(maze->filling));
	Write(fd, &(maze->numpart), sizeof(maze->numpart));
					 /* Number of parts */
	for(i=0;i<maze->numpart;i++) {
	    pt = maze->parts[i];

	    /* First, write the map */

	    Write(fd, &(pt->halign), sizeof(pt->halign));
	    Write(fd, &(pt->valign), sizeof(pt->valign));
	    Write(fd, &(pt->xsize), sizeof(pt->xsize));
	    Write(fd, &(pt->ysize), sizeof(pt->ysize));
	    for(j=0;j<pt->ysize;j++) {
		if(!maze->init_lev.init_present ||
		   pt->xsize > 1 || pt->ysize > 1) {
#if !defined(_MSC_VER) && !defined(__BORLANDC__)
			Write(fd, pt->map[j], pt->xsize * sizeof *pt->map[j]);
#else
			/*
			 * On MSVC and Borland C compilers the Write macro above caused:
			 * warning '!=' : signed/unsigned mismatch
			 */
			unsigned reslt, sz = pt->xsize * sizeof *pt->map[j];
			reslt = write(fd, (void *)(pt->map[j]), sz);
			if (reslt != sz) return FALSE;
#endif
		}
		free(pt->map[j]);
	    }
	    free(pt->map);

	    /* level region stuff */
	    Write(fd, &pt->nlreg, sizeof pt->nlreg);
	    for (j = 0; j < pt->nlreg; j++) {
		lev_region *l = pt->lregions[j];
		char *rname = l->rname.str;
		l->rname.str = 0;	/* reset in case `len' is narrower */
		l->rname.len = rname ? strlen(rname) : 0;
		Write(fd, l, sizeof *l);
		if (rname) {
		    Write(fd, rname, l->rname.len);
		    free(rname);
		}
		free(l);
	    }
	    if (pt->nlreg > 0)
		free(pt->lregions);

	    /* random level regions registers */
	    Write(fd, &pt->nrndlreg, sizeof pt->nrndlreg);
	    for (j = 0; j < pt->nrndlreg; j++) {
		lev_region *l = pt->rndlregions[j];
		char *rname = l->rname.str;
		l->rname.str = 0;	/* reset in case `len' is narrower */
		l->rname.len = rname ? strlen(rname) : 0;
		Write(fd, l, sizeof *l);
		if (rname) {
		    Write(fd, rname, l->rname.len);
		    free(rname);
		}
		free(l);
	    }
	    if (pt->nrndlreg > 0)
		free(pt->rndlregions);

	    /* The random registers */
	    Write(fd, &(pt->nrobjects), sizeof(pt->nrobjects));
	    if(pt->nrobjects) {
		    Write(fd, pt->robjects, pt->nrobjects);
		    free(pt->robjects);
	    }
	    Write(fd, &(pt->nlocset), sizeof(pt->nlocset));
	    if (pt->nlocset) {
		Write(fd, pt->nloc, pt->nlocset);
		for (j = 0; j < pt->nlocset; j++) {
		    Write(fd, pt->rloc_x[j], pt->nloc[j]);
		    Write(fd, pt->rloc_y[j], pt->nloc[j]);
		    free(pt->rloc_x[j]);
		    free(pt->rloc_y[j]);
		}
		free(pt->nloc);
		free(pt->rloc_x);
		free(pt->rloc_y);
	    }
	    Write(fd, &(pt->nrmonst), sizeof(pt->nrmonst));
	    if(pt->nrmonst) {
		    Write(fd, pt->rmonst, pt->nrmonst);
		    free(pt->rmonst);
	    }

	    /* subrooms */
	    Write(fd, &(pt->nreg), sizeof(pt->nreg));
	    for(j=0;j<pt->nreg;j++) {
		    Write(fd, pt->regions[j], sizeof(region));
		    free(pt->regions[j]);
	    }
	    if(pt->nreg > 0)
		    free(pt->regions);

	    /* the doors */
	    Write(fd, &(pt->ndoor), sizeof(pt->ndoor));
	    for(j=0;j<pt->ndoor;j++) {
		    Write(fd, pt->doors[j], sizeof(door));
		    free(pt->doors[j]);
	    }
	    if (pt->ndoor > 0)
		    free(pt->doors);

	    /* The drawbridges */
	    Write(fd, &(pt->ndrawbridge), sizeof(pt->ndrawbridge));
	    for(j=0;j<pt->ndrawbridge;j++) {
		    Write(fd, pt->drawbridges[j], sizeof(drawbridge));
		    free(pt->drawbridges[j]);
	    }
	    if(pt->ndrawbridge > 0)
		    free(pt->drawbridges);

	    /* The mazewalk directives */
	    Write(fd, &(pt->nwalk), sizeof(pt->nwalk));
	    for(j=0; j<pt->nwalk; j++) {
		    Write(fd, pt->walks[j], sizeof(walk));
		    free(pt->walks[j]);
	    }
	    if (pt->nwalk > 0)
		    free(pt->walks);

	    /* The non_diggable directives */
	    Write(fd, &(pt->ndig), sizeof(pt->ndig));
	    for(j=0;j<pt->ndig;j++) {
		    Write(fd, pt->digs[j], sizeof(digpos));
		    free(pt->digs[j]);
	    }
	    if (pt->ndig > 0)
		    free(pt->digs);

	    /* The non_passwall directives */
	    Write(fd, &(pt->npass), sizeof(pt->npass));
	    for(j=0;j<pt->npass;j++) {
		    Write(fd, pt->passs[j], sizeof(digpos));
		    free(pt->passs[j]);
	    }
	    if (pt->npass > 0)
		    free(pt->passs);

	    /* The ladders */
	    Write(fd, &(pt->nlad), sizeof(pt->nlad));
	    for(j=0;j<pt->nlad;j++) {
		    Write(fd, pt->lads[j], sizeof(lad));
		    free(pt->lads[j]);
	    }
	    if (pt->nlad > 0)
		    free(pt->lads);

	    /* The stairs */
	    Write(fd, &(pt->nstair), sizeof(pt->nstair));
	    for(j=0;j<pt->nstair;j++) {
		    Write(fd, pt->stairs[j], sizeof(stair));
		    free(pt->stairs[j]);
	    }
	    if (pt->nstair > 0)
		    free(pt->stairs);

	    /* The altars */
	    Write(fd, &(pt->naltar), sizeof(pt->naltar));
	    for(j=0;j<pt->naltar;j++) {
		    Write(fd, pt->altars[j], sizeof(altar));
		    free(pt->altars[j]);
	    }
	    if (pt->naltar > 0)
		    free(pt->altars);

	    /* The fountains */
	    Write(fd, &(pt->nfountain), sizeof(pt->nfountain));
	    for(j=0;j<pt->nfountain;j++) {
		Write(fd, pt->fountains[j], sizeof(fountain));
		free(pt->fountains[j]);
	    }
	    if (pt->nfountain > 0)
		    free(pt->fountains);

	    /* The traps */
	    Write(fd, &(pt->ntrap), sizeof(pt->ntrap));
	    for(j=0;j<pt->ntrap;j++) {
		    Write(fd, pt->traps[j], sizeof(trap));
		    free(pt->traps[j]);
	    }
	    if (pt->ntrap)
		    free(pt->traps);

	    /* The monsters */
	    if (!write_monsters(fd, &pt->nmonster, &pt->monsters))
		    return FALSE;

	    /* The objects */
	    if (!write_objects(fd, &pt->nobject, &pt->objects))
		    return FALSE;

	    /* The gold piles */
	    Write(fd, &(pt->ngold), sizeof(pt->ngold));
	    for(j=0;j<pt->ngold;j++) {
		    Write(fd, pt->golds[j], sizeof(gold));
		    free(pt->golds[j]);
	    }
	    if (pt->ngold > 0)
		    free(pt->golds);

	    /* The engravings */
	    if (!write_engravings(fd, &pt->nengraving, &pt->engravings))
		    return FALSE;

	    free(pt);
	}

	free(maze->parts);
	maze->parts = (mazepart **)0;
	maze->numpart = 0;
	return TRUE;
}

/*
 * Here we write the structure of the room level in the specified file (fd).
 */
static boolean
write_rooms(fd, lev)
int fd;
splev *lev;
{
	short i,j, size;
	room *pt;

	if (!write_common_data(fd, SP_LEV_ROOMS, &(lev->init_lev), lev->flags))
		return FALSE;

	/* Random registers */

	Write(fd, &lev->nrobjects, sizeof(lev->nrobjects));
	if (lev->nrobjects)
		Write(fd, lev->robjects, lev->nrobjects);
	Write(fd, &lev->nrmonst, sizeof(lev->nrmonst));
	if (lev->nrmonst)
		Write(fd, lev->rmonst, lev->nrmonst);

	Write(fd, &(lev->nroom), sizeof(lev->nroom));
							/* Number of rooms */
	for(i=0;i<lev->nroom;i++) {
		pt = lev->rooms[i];

		/* Room characteristics */

		size = (short) (pt->name ? strlen(pt->name) : 0);
		Write(fd, &size, sizeof(size));
		if (size)
			Write(fd, pt->name, size);

		size = (short) (pt->parent ? strlen(pt->parent) : 0);
		Write(fd, &size, sizeof(size));
		if (size)
			Write(fd, pt->parent, size);

		Write(fd, &(pt->x), sizeof(pt->x));
		Write(fd, &(pt->y), sizeof(pt->y));
		Write(fd, &(pt->w), sizeof(pt->w));
		Write(fd, &(pt->h), sizeof(pt->h));
		Write(fd, &(pt->xalign), sizeof(pt->xalign));
		Write(fd, &(pt->yalign), sizeof(pt->yalign));
		Write(fd, &(pt->rtype), sizeof(pt->rtype));
		Write(fd, &(pt->chance), sizeof(pt->chance));
		Write(fd, &(pt->rlit), sizeof(pt->rlit));
		Write(fd, &(pt->filled), sizeof(pt->filled));

		/* the doors */
		Write(fd, &(pt->ndoor), sizeof(pt->ndoor));
		for(j=0;j<pt->ndoor;j++)
			Write(fd, pt->doors[j], sizeof(room_door));

		/* The stairs */
		Write(fd, &(pt->nstair), sizeof(pt->nstair));
		for(j=0;j<pt->nstair;j++)
			Write(fd, pt->stairs[j], sizeof(stair));

		/* The altars */
		Write(fd, &(pt->naltar), sizeof(pt->naltar));
		for(j=0;j<pt->naltar;j++)
			Write(fd, pt->altars[j], sizeof(altar));

		/* The fountains */
		Write(fd, &(pt->nfountain), sizeof(pt->nfountain));
		for(j=0;j<pt->nfountain;j++)
			Write(fd, pt->fountains[j], sizeof(fountain));

		/* The sinks */
		Write(fd, &(pt->nsink), sizeof(pt->nsink));
		for(j=0;j<pt->nsink;j++)
			Write(fd, pt->sinks[j], sizeof(sink));

		/* The pools */
		Write(fd, &(pt->npool), sizeof(pt->npool));
		for(j=0;j<pt->npool;j++)
			Write(fd, pt->pools[j], sizeof(pool));

		/* The traps */
		Write(fd, &(pt->ntrap), sizeof(pt->ntrap));
		for(j=0;j<pt->ntrap;j++)
			Write(fd, pt->traps[j], sizeof(trap));

		/* The monsters */
		if (!write_monsters(fd, &pt->nmonster, &pt->monsters))
			return FALSE;

		/* The objects */
		if (!write_objects(fd, &pt->nobject, &pt->objects))
			return FALSE;

		/* The gold piles */
		Write(fd, &(pt->ngold), sizeof(pt->ngold));
		for(j=0;j<pt->ngold;j++)
			Write(fd, pt->golds[j], sizeof(gold));

		/* The engravings */
		if (!write_engravings(fd, &pt->nengraving, &pt->engravings))
			return FALSE;

	}

	/* The corridors */
	Write(fd, &lev->ncorr, sizeof(lev->ncorr));
	for (i=0; i < lev->ncorr; i++)
		Write(fd, lev->corrs[i], sizeof(corridor));
	return TRUE;
}

/*
 * Release memory allocated to a rooms-style special level; maze-style
 * levels have the fields freed as they're written; monsters, objects, and
 * engravings are freed as written for both styles, so not handled here.
 */
void
free_rooms(lev)
splev *lev;
{
	room *r;
	int j, n = lev->nroom;

	while(n--) {
		r = lev->rooms[n];
		free(r->name);
		free(r->parent);
		if ((j = r->ndoor) != 0) {
			while(j--)
				free(r->doors[j]);
			free(r->doors);
		}
		if ((j = r->nstair) != 0) {
			while(j--)
				free(r->stairs[j]);
			free(r->stairs);
		}
		if ((j = r->naltar) != 0) {
			while (j--)
				free(r->altars[j]);
			free(r->altars);
		}
		if ((j = r->nfountain) != 0) {
			while(j--)
				free(r->fountains[j]);
			free(r->fountains);
		}
		if ((j = r->nsink) != 0) {
			while(j--)
				free(r->sinks[j]);
			free(r->sinks);
		}
		if ((j = r->npool) != 0) {
			while(j--)
				free(r->pools[j]);
			free(r->pools);
		}
		if ((j = r->ntrap) != 0) {
			while (j--)
				free(r->traps[j]);
			free(r->traps);
		}
		if ((j = r->ngold) != 0) {
			while(j--)
				free(r->golds[j]);
			free(r->golds);
		}
		free(r);
		lev->rooms[n] = (room *)0;
	}
	free(lev->rooms);
	lev->rooms = (room **)0;
	lev->nroom = 0;

	for (j = 0; j < lev->ncorr; j++) {
		free(lev->corrs[j]);
		lev->corrs[j] = (corridor *)0;
	}
	free(lev->corrs);
	lev->corrs = (corridor **)0;
	lev->ncorr = 0;

	free(lev->robjects);
	lev->robjects = (char *)0;
	lev->nrobjects = 0;
	free(lev->rmonst);
	lev->rmonst = (char *)0;
	lev->nrmonst = 0;
}

#ifdef STRICT_REF_DEF
/*
 * Any globals declared in hack.h and descendents which aren't defined
 * in the modules linked into lev_comp should be defined here.  These
 * definitions can be dummies:  their sizes shouldn't matter as long as
 * as their types are correct; actual values are irrelevant.
 */
#define ARBITRARY_SIZE 1
/* attrib.c */
struct attribs attrmax, attrmin;
/* files.c */
const char *configfile;
char lock[ARBITRARY_SIZE];
char SAVEF[ARBITRARY_SIZE];
# ifdef MICRO
char SAVEP[ARBITRARY_SIZE];
# endif
/* termcap.c */
struct tc_lcl_data tc_lcl_data;
# ifdef TEXTCOLOR
#  ifdef TOS
const char *hilites[CLR_MAX];
#  else
char NEARDATA *hilites[CLR_MAX];
#  endif
# endif
/* trap.c */
const char *traps[TRAPNUM];
/* window.c */
struct window_procs windowprocs;
/* xxxtty.c */
# ifdef DEFINE_OSPEED
short ospeed;
# endif
#endif	/* STRICT_REF_DEF */

/*lev_main.c*/
